/**
 * @file   tetris_display_ctrl.c
 * @brief  tetris・描画出力制御実装
 */

//======================================================
// インクルード
//======================================================
#include "tetris.h"
#include "tetris_internal.h"
#include "SH1107.h"
#include "math_lib.h"
#include "bitmap_lib.h"

//======================================================
// マクロ定義
//======================================================
#define VISUALIZE_MINO_DEF_LENGTH 24
#define VISUALIZE_OFFSET_X 1
#define VISUALIZE_OFFSET_Y 4

//======================================================
// 型定義
//======================================================

//======================================================
// 変数・定数
//======================================================
static uint8_t game_restarted_counter = 0; // ゲーム起動・再起動のカウンター（起動・再起動を検知するためだけに使用　オーバーフローを許容する）
static bitmap_128_t previous_layer;        // ゲームオーバー時のベースレイヤ用　ゲーム実行中の描画データを保持しておく

//======================================================
// プロトタイプ宣言
//======================================================
static void update_previous_base_layer(const bitmap_128_t current_bitmap);
static void overlay_Fixed_UI(bitmap_128_t dst_bitmap);
static void overlay_field_layer(bitmap_128_t dst_bitmap, tetris_compute_state_t *mino_compute_data);
static void overlay_information_layer(bitmap_128_t dst_bitmap, tetris_compute_state_t *mino_compute_data);
static void get_number_string_bitmap(bitmap_128_t dst_bitmap, uint16_t num);
static void get_number_bitmap(bitmap_128_t dst_bitmap, uint8_t num);
static void get_visualize_mino_bitmap(bitmap_128_t dst, const bitmap_128_t visualize_mino_definition_1, const bitmap_128_t visualize_mino_definition_2, tetris_mino_type_t mino_type, tetris_mino_turn_state_t turn);

//======================================================
// 公開関数定義
//======================================================
/**
 * @brief ゲーム開始待機画面表示
 * @return なし
 * @details 一定周期で開始メッセージの表示をトグルし、点滅表示を行う
 */
void tetris_display_waiting_start()
{
    static bool enable_message = false;         // メッセージ点滅表示選択
    static uint8_t blink_cycle_counter = 50;    // メッセージ点滅サイクルカウンター
    static bitmap_128_t base_layer_initializer; // レイヤ初期値

    // 等時間間隔でメッセージを点滅させる
    if (blink_cycle_counter++ >= 50) // 数値は適当
    {
        // レイヤ初期化
        bitmap_128_t base_layer = {0};
        overlay_Fixed_UI(base_layer);

        // メッセージ表示有無をトグル
        enable_message ^= true;
        if (enable_message)
        {
            BITMAP_or(base_layer, tetris_bitmap_def_start_message); // スタートメッセージを重ねる
        }

        // 描画用データ送信
        SH1107_display_bitmap_data(base_layer);

        blink_cycle_counter = 0;
    }
}

/**
 * @brief ゲーム実行中 描画メイン処理
 * @param compute_state_ptr 演算状態
 * @return なし
 * @details 固定UI、フィールドレイヤ、各種情報レイヤを合成し、ディスプレイICに送信して表示させる
 */
void tetris_display_ctrl_in_game(tetris_compute_state_t *compute_state_ptr)
{
    // レイヤ初期化
    bitmap_128_t base_layer = {0};
    overlay_Fixed_UI(base_layer); // 固定UIをオーバーレイ

    // 各種レイヤをオーバーレイ
    overlay_field_layer(base_layer, compute_state_ptr);       // 左画面に表示するプレイフィールドを生成してオーバーレイ
    overlay_information_layer(base_layer, compute_state_ptr); // 右画面に表示するスコアやレベルなどの可変UIを生成してオーバーレイ

    // 描画用データ送信
    SH1107_display_bitmap_data(base_layer);

    // 前回送信データとして保持しておく（保持したレイヤーはゲームオーバー時に使用する）
    BITMAP_copy(previous_layer, base_layer);
}

/**
 * @brief ゲームリスタート待機画面表示
 * @return なし
 * @details 再開メッセージを点滅表示させる
 *          ゲームオーバー時の画面に再開メッセージを重ねる形で画面生成している
 *          これによって、再開メッセージ表示中もゲームオーバー時のスコア等を見れるようにしている
 */
void tetris_display_waiting_restart()
{
    static uint8_t previous_game_restarted_local = 0; // 起動・再起動検知用

    static bool enable_message;                 // メッセージ点滅表示選択
    static uint8_t blink_cycle_counter;         // メッセージ点滅サイクルカウンター
    static bitmap_128_t base_layer_initializer; // レイヤ初期値

    // ゲーム起動・再起動後の初回コール時のみ、各パラメータの初期値を設定する
    if (previous_game_restarted_local != game_restarted_counter)
    {
        enable_message = false;
        blink_cycle_counter = 0;
        previous_game_restarted_local = game_restarted_counter;
    }

    // 等時間間隔でメッセージを点滅させる
    if (blink_cycle_counter++ >= 50) // 数値は適当
    {
        // レイヤ初期化
        bitmap_128_t base_layer = {0};
        BITMAP_copy(base_layer, previous_layer); // previous_layerにはゲームオーバー時の表示画面がそのまま保持されている

        // メッセージ表示有無をトグル
        enable_message ^= true;
        if (enable_message)
        {
            BITMAP_not(base_layer, tetris_bitmap_def_restart_message_bold); // リスタートメッセージを重ねる（太字で空白を作る）
            BITMAP_or(base_layer, tetris_bitmap_def_restart_message);       // リスタートメッセージを重ねる（細字で上書き）
        }

        // 描画用データ送信
        SH1107_display_bitmap_data(base_layer);

        blink_cycle_counter = 0;
    }
}

/**
 * @brief 描画制御再起動通知更新
 * @return なし
 * @details ゲーム開始時＆ゲームオーバー後のゲームリスタート時に呼ばれる
 *          カウンタが更新されることによって、内部関数がリスタートを検知できる
 */
void tetris_initialize_display_ctrl()
{
    game_restarted_counter = game_restarted_counter + 1; // リスタート通知（オーバーフロー許容）
}

//======================================================
// 内部関数定義
//======================================================
/**
 * @brief フィールド関連レイヤ合成
 * @param dst_bitmap 出力先ビットマップ
 * @param compute_state_ptr 演算状態
 * @return なし
 * @details ディスプレイの左画面に表示するプレイフィールドのビットマップを生成
 * @note 計算量が多く最適化余地あり
 */
static void overlay_field_layer(bitmap_128_t dst_bitmap, tetris_compute_state_t *compute_state_ptr)
{
    bitmap_128_t base_bitmap = {0};
    bitmap_128_t base_bitmap_extracted = {0};
    bitmap_128_t base_bitmap_enlarged = {0};
    bitmap_128_t falling_point_bitmap = {0};
    bitmap_128_t falling_point_bitmap_enlarged = {0};

    // 演算用ビットマップを重ねる（この時点では1ブロック1ドット）
    BITMAP_or(base_bitmap, compute_state_ptr->field_parameter.bitmap); // フィールドのビットマップをオーバーレイ
    BITMAP_or(base_bitmap, compute_state_ptr->mino_parameter.bitmap);  // ミノのビットマップをオーバーレイ

    // 重ねた演算用ビットマップをディスプレイ表示用に拡大＆調整する
    BITMAP_shift(base_bitmap, -1, -4);                               // ボックス内側の左上のドットが0,0に来るようシフトする
    BITMAP_extract(base_bitmap_extracted, base_bitmap, 0, 9, 0, 19); // ボックス枠を削除（ボックス枠は固定UI側で表示するため）
    BITMAP_enlarge(base_bitmap_enlarged, base_bitmap_extracted, 6);  // 拡大表示する
    BITMAP_shift(base_bitmap_enlarged, 6, 6);                        // 固定UIに合わせて位置調整
    BITMAP_and(base_bitmap_enlarged, tetris_bitmap_def_field_layer); // ミノに描画用レイヤを適用する

    // 上記とは別で落下地点表示のビットマップを生成する
    BITMAP_copy(falling_point_bitmap, compute_state_ptr->mino_parameter.bitmap);                       // ミノのビットマップを取得
    BITMAP_shift(falling_point_bitmap, -1, compute_state_ptr->mino_parameter.distance_to_landing - 4); // 落下地点にシフト＆ボックス内の左上のドットが0,0に来るようシフトする
    BITMAP_enlarge(falling_point_bitmap_enlarged, falling_point_bitmap, 6);                            // 拡大表示する
    BITMAP_shift(falling_point_bitmap_enlarged, 6, 6);                                                 // 固定UIに合わせて位置調整
    BITMAP_and(falling_point_bitmap_enlarged, tetris_bitmap_def_falling_point_layer);                  // 落下地点レイヤー専用表示を適用

    // 最終的なビットマップを合成
    BITMAP_or(base_bitmap_enlarged, falling_point_bitmap_enlarged); // 拡大した演算用ビットマップと落下地点表示ビットマップを重ねる
    BITMAP_or(dst_bitmap, base_bitmap_enlarged);                    // 戻り値の格納
}

/**
 * @brief 固定UIレイヤ取得
 * @param dst_bitmap 出力先ビットマップ
 * @return なし
 * @details 定数のビットマップレイヤーを取得するだけ
 */
static void overlay_Fixed_UI(bitmap_128_t dst_bitmap)
{
    BITMAP_or(dst_bitmap, tetris_bitmap_def_fixed_UI);
}

/**
 * @brief 情報レイヤ合成
 * @param dst_bitmap 出力先ビットマップ
 * @param compute_state_ptr 演算状態
 * @details ディスプレイの右画面に表示するパラメータ表示のビットマップを生成
 * @return なし
 */
static void overlay_information_layer(bitmap_128_t dst_bitmap, tetris_compute_state_t *compute_state_ptr)
{
    // 初期化
    bitmap_128_t next_bitmap = {0};
    bitmap_128_t level_bitmap = {0};
    bitmap_128_t row_bitmap = {0};
    bitmap_128_t score_bitmap = {0};

    // ネクストミノ、レベル、消去行、スコア情報のビットマップを取得する
    get_visualize_mino_bitmap(next_bitmap, tetris_bitmap_def_next_mino_1, tetris_bitmap_def_next_mino_2, compute_state_ptr->mino_parameter.next_mino_type, r_no_turn);
    get_number_string_bitmap(level_bitmap, compute_state_ptr->game_parameter.level);
    get_number_string_bitmap(row_bitmap, compute_state_ptr->game_parameter.row_deleted);
    get_number_string_bitmap(score_bitmap, compute_state_ptr->game_parameter.score);

    // 上記で取得したビットマップを全て重ねる
    BITMAP_or_with_shift(dst_bitmap, next_bitmap, 85, 17);   // 位置は手動設定
    BITMAP_or_with_shift(dst_bitmap, level_bitmap, 91, 63);  // 位置は手動設定
    BITMAP_or_with_shift(dst_bitmap, row_bitmap, 91, 90);    // 位置は手動設定
    BITMAP_or_with_shift(dst_bitmap, score_bitmap, 91, 116); // 位置は手動設定
}

/**
 * @brief 10進数文字列ビットマップ生成
 * @param dst_bitmap 出力先ビットマップ
 * @param num 変換対象数値
 * @return なし
 * @details 数値を10進数の文字列に分解し、各桁のビットマップを取得して重ねることで、数値全体のビットマップを生成する
 */
static void get_number_string_bitmap(bitmap_128_t dst_bitmap, uint16_t num)
{
    bitmap_128_t ret_bitmap = {0};

    int num_array[100]; // 100のサイズは適当
    int digits = MATH_split_digits(num_array, num);

    for (uint8_t d = 0; d < digits; d++)
    {
        bitmap_128_t temp_bitmap = {0};
        get_number_bitmap(temp_bitmap, num_array[d]);
        BITMAP_shift(temp_bitmap, d * 5, 0);

        BITMAP_or(ret_bitmap, temp_bitmap);
    }

    BITMAP_copy(dst_bitmap, ret_bitmap); // ビットマップを上書き
}

/**
 * @brief 数値ビットマップ抽出
 * @param dst_bitmap 出力先ビットマップ
 * @param num 抽出対象数値
 * @return なし
 * @details 0～9の数字ビットマップは、1枚の128×128ビットマップに横に並べて埋め込んでいる（容量削減のため）
 *          数値を指定することで、その数値に対応したビットマップを抽出する
 */
static void get_number_bitmap(bitmap_128_t dst_bitmap, uint8_t num)
{
    // ビットマップからの抽出対象座標計算
    uint8_t start_x = num * 5;
    uint8_t end_X = num * 5 + 3;
    uint8_t start_y = 0;
    uint8_t end_y = 6;

    // ビットマップデータ抽出
    BITMAP_extract(dst_bitmap, tetris_bitmap_def_numbers, start_x, end_X, start_y, end_y);
}

/**
 * @brief 描画用ミノビットマップ抽出
 * @param dst 出力先ビットマップ
 * @param visualize_mino_definition_1 定義ビットマップ1
 * @param visualize_mino_definition_2 定義ビットマップ2
 * @param mino_type ミノ種別
 * @param turn 回転状態
 * @return なし
 * @details 描画用ミノのビットマップは、2枚の128×128ビットマップに複数のミノを並べて埋め込んでいる（容量削減のため）
 *          ミノの種別と回転状態を指定することで、その2枚のビットマップから欲しいミノを抽出する
 */
static void get_visualize_mino_bitmap(bitmap_128_t dst, const bitmap_128_t visualize_mino_definition_1, const bitmap_128_t visualize_mino_definition_2, tetris_mino_type_t mino_type, tetris_mino_turn_state_t turn)
{
    // ミノ定義のビットマップを2枚に分けているので、どちらを参照するかを選択
    uint8_t select_bitmap_num = (mino_type < mino_S) ? 1 : 2;

    // ビットマップからの抽出対象座標計算
    uint8_t start_x = turn * VISUALIZE_MINO_DEF_LENGTH;
    uint8_t end_X = ((turn + 1) * VISUALIZE_MINO_DEF_LENGTH) - 1;
    uint8_t start_y = (mino_type - (select_bitmap_num - 1) * mino_S) * VISUALIZE_MINO_DEF_LENGTH;
    uint8_t end_y = ((mino_type - (select_bitmap_num - 1) * mino_S + 1) * VISUALIZE_MINO_DEF_LENGTH) - 1;

    // ビットマップデータ抽出
    if (1 == select_bitmap_num)
    {
        BITMAP_extract(dst, visualize_mino_definition_1, start_x, end_X, start_y, end_y);
    }
    else
    {
        BITMAP_extract(dst, visualize_mino_definition_2, start_x, end_X, start_y, end_y);
    }
}