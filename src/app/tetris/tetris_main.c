/**
 * @file   tetris_main.c
 * @brief  tetris・メインループ実装
 */

//======================================================
// インクルード
//======================================================
#include "tetris.h"
#include "tetris_internal.h"
#include "typedef.h"
#include "button.h"
#include "timer.h"
#include "analogStick.h"
#include "SH1107.h"

//======================================================
// マクロ定義
//======================================================

//======================================================
// 型定義
//======================================================
/**
 * @brief 周期タスク実行フラグ
 */
typedef struct
{
    bool task_1ms;   /**< 1ms周期で実行するタスク */
    bool task_10ms;  /**< 10ms周期で実行するタスク */
    bool task_100ms; /**< 100ms周期で実行するタスク */
} scheduler_flag_t;

//======================================================
// 変数・定数
//======================================================
static scheduler_flag_t scheduler_flag = {false}; // 周期管理用フラグ（割り込みからコールバックされた関数が参照するのでここで定義必要）
static tetris_input_state_t input_state;          // 入力ステート
static tetris_compute_state_t compute_state;      // 演算ステート
// 描画ステートは入力層・演算層に渡さないので、描画層の内部ステートとして持つ

static tetris_game_state_t game_state_current = game_waiting_start; // ゲームステート（debug関数からのRWがあるのでファイル内グローバル）

//======================================================
// プロトタイプ宣言
//======================================================
static void task_scheduler();
static bool check_task(bool *task_Nms_flag);
static void update_game_state(tetris_game_state_t *state_current_ptr, tetris_game_state_t state_next);

//======================================================
// 公開関数定義
//======================================================
/**
 * @brief テトリス メインループ
 * @param input_handler 入力ハンドラ
 * @return なし
 * @details 全てのステートは入力系処理 → 内部演算系処理 → 描画出力系処理 → ステート更新処理 の順で処理される
 *          現状は全ステート一律で10ms周期での実行
 */
void TETRIS_main(TETRIS_input_parameter_t *input_handler)
{
    /* アプリ初期化 */
    tetris_game_state_t game_state_next = game_state_current;                                 // ゲームステート更新用（次に遷移するステートを保持する）
    bool task_do = false;                                                                     // タスク実行フラグ初期化
    TIMER_set_alarm_callback_function((TIMER_callback_func_pointer_t)task_scheduler, alarm0); // 周期管理用タイマ割り込み設定
    TIMER_enable_alarm_interrupt(ENABLE, alarm0);                                             // 周期管理用タイマ割り込み設定
    task_scheduler();                                                                         // 周期管理開始（アラーム割り込みのループが開始される）

    /* メインルーチン */
    while (true)
    {
        if (check_task(&scheduler_flag.task_10ms)) // 10ms周期で実行
        {
            /* メインステート処理 */
            switch (game_state_current)
            {
            /* ゲーム開始のボタン入力待ち */
            case game_waiting_start:
                tetris_receive_game_start_input(input_handler, &input_state); // 入力系処理
                game_state_next = tetris_judge_game_start(&input_state);      // 内部演算系処理
                tetris_display_waiting_start();                               // 描画出力系処理
                update_game_state(&game_state_current, game_state_next);      // ステート更新処理
                break;

            /* ゲーム実行用パラメータ初期化 */
            case game_start_initialization:
                tetris_initialize_input_ctrl(&input_state);
                tetris_initialize_data_compute(&compute_state);
                tetris_initialize_display_ctrl();
                update_game_state(&game_state_current, game_running);
                break;

            /* ゲーム実行中 */
            case game_running:
                tetris_input_ctrl_in_game(input_handler, &input_state);
                game_state_next = tetris_data_compute_in_game(&input_state, &compute_state);
                tetris_display_ctrl_in_game(&compute_state);
                update_game_state(&game_state_current, game_state_next);
                break;

            /* ゲームオーバー画面＆リスタートのボタン入力待ち */
            case game_over:
                tetris_receive_game_restart_input(input_handler, &input_state);
                game_state_next = tetris_judge_game_restart(&input_state);
                tetris_display_waiting_restart();
                update_game_state(&game_state_current, game_state_next);
                break;

            /* ポーズ中 */
            case game_pause:
                // do nothing
                break;

            default:
                break;
            }

            /* デバッグプロセスはステートに関わらず実行 */
            // tetris_execute_debug_process(); // 現状無効化
        }
    }
}

/**
 * @brief デバッグ用ポーズ状態切替
 * @param is_enable ポーズ有効フラグ
 * @return なし
 * @details デバッグ用通信ツールからコマンド受信した際にコールされ、ポーズ状態が切り替わる
 */
void tetris_debug_pause_enale(bool is_enable)
{
    static bool is_pause_enabled = false;
    static tetris_game_state_t state_previous;

    if (!is_pause_enabled && is_enable) // 通常動作中にポーズ指定
    {
        state_previous = game_state_current;                // 直前のステートを保存
        update_game_state(&game_state_current, game_pause); // ポーズ有効化
        is_pause_enabled = true;
    }
    else if (is_pause_enabled && !is_enable) // ポーズ中に解除指定
    {
        update_game_state(&game_state_current, state_previous); // ステート復元
        is_pause_enabled = false;
    }
}

/**
 * @brief デバッグ用ゲームステート取得
 * @return 現在のゲームステート
 * @details デバッグ用通信ツールへの送信用
 */
tetris_game_state_t tetris_get_game_state()
{
    return game_state_current;
}

//======================================================
// 内部関数定義
//======================================================
/**
 * @brief 周期タスクフラグ更新
 * @return なし
 * @details アラーム割り込みから周期的に呼び出され、1ms/10ms/100msタスク実行フラグを更新する
 */
static void task_scheduler()
{
    static uint32_t counter_1ms = 0;
    static uint32_t counter_10ms = 0;
    static uint32_t counter_100ms = 0;

    counter_1ms++;
    counter_10ms++;
    counter_100ms++;

    if (counter_1ms >= 1) // 可読性重視
    {
        scheduler_flag.task_1ms = true;
        counter_1ms = 0;
    }
    if (counter_10ms >= 10)
    {
        scheduler_flag.task_10ms = true; // 現状10msタスクは無し
        counter_10ms = 0;
    }
    if (counter_100ms >= 100)
    {
        scheduler_flag.task_100ms = true; // 現状100msタスクは無し
        counter_100ms = 0;
    }

    TIMER_set_alarm(1 * 1000, alarm0); // タイマ割り込み設定：1000us = 1ms後に再度コールされる
}

/**
 * @brief タスク実行可否判定
 * @param task_Nms_flag 対象周期タスクフラグ
 * @return 実行対象時true
 */
static bool check_task(bool *task_Nms_flag)
{
    if (*task_Nms_flag)
    {
        *task_Nms_flag = false;
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief ゲームステート更新
 * @param state_current_ptr 現在ステート格納先
 * @param state_next 次ステート
 * @return なし
 */
static void update_game_state(tetris_game_state_t *state_current_ptr, tetris_game_state_t state_next)
{
    *state_current_ptr = state_next;
}