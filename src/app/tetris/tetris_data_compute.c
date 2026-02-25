/**
 * @file   tetris_data_compute.c
 * @brief  tetris・演算処理実装
 */

//======================================================
// インクルード
//======================================================
#include "tetris.h"
#include "tetris_internal.h"
#include "typedef.h"
#include "bitmap_lib.h"
#include "math_lib.h"
#include "timer.h"

//======================================================
// マクロ定義
//======================================================
// 入力値に対するミノ移動の閾値（移動カウンタがこの閾値を超えた時にミノを1ドット分移動させる）
#define MINO_MOVE_L_TH 2
#define MINO_MOVE_R_TH 2
#define MINO_MOVE_D_TH 100

// ミノの初期位置定義（ボックスビットマップ依存）
#define MINO_X_INITIAL 4
#define MINO_Y_INITIAL 5

// ミノ定義ビットマップのパラメータ（任意のミノデータを抽出する際に使用）
#define MINO_DEF_LENGTH 4

// ミノ種類数
#define NUMBER_MINO_TYPES 7

// 消去行数に対するスコア倍率
#define ERASE_ROW_MAX 4 // 一度に消去可能な最大行数
#define SCORE_POWER_RATE_1ROW 10
#define SCORE_POWER_RATE_2ROW 13
#define SCORE_POWER_RATE_3ROW 20
#define SCORE_POWER_RATE_4ROW 30

// ゲーム最大レベル
#define MAXIMUM_LEVEL 9

//======================================================
// 型定義
//======================================================
/**
 * @brief ミノ衝突判定定義
 */
typedef enum
{
    not_collided = 0, /**< 衝突無し */
    collided,         /**< 衝突有り */
} mino_is_collide_t;

/**
 * @brief ミノ移動カウンター定義
 */
typedef struct
{
    int L; /**< 左移動カウンタ */
    int R; /**< 右移動カウンタ */
    int D; /**< 下移動カウンタ */
} mino_move_counter_t;

//======================================================
// 変数・定数
//======================================================
// ファイル内グローバル変数
static mino_move_counter_t mino_move_counter = {0}; // ミノ 移動演算用カウンタ
static uint8_t row_erased = 0;                      // ミノ接地により消去された行数（スコア計算用）
static bool allow_down_shift = false;               // 下入力による高速落下の許可フラグ

// 定数
static const uint8_t free_fall_confficient[MAXIMUM_LEVEL + 1] = {0, 5, 7, 10, 13, 16, 21, 26, 34, 51}; // ミノの自由落下係数（レベルで増加）
static const uint8_t score_power_rate[ERASE_ROW_MAX + 1] = {0, SCORE_POWER_RATE_1ROW, SCORE_POWER_RATE_2ROW, SCORE_POWER_RATE_3ROW, SCORE_POWER_RATE_4ROW};
static const uint16_t next_level_need_row[MAXIMUM_LEVEL] = {0, 3, 6, 9, 13, 17, 21, 28, 35};

//======================================================
// プロトタイプ宣言
//======================================================
static void generate_new_mino(tetris_mino_parameter_t *mino_parameter_ptr);
static void move_mino_initial_position(tetris_compute_state_t *compute_state_ptr);
static void turn_mino(tetris_compute_state_t *compute_state_ptr, tetris_input_state_t *input_state_ptr);
static mino_is_collide_t move_mino(tetris_compute_state_t *compute_state_ptr, tetris_input_state_t *input_state_ptr);
static void caluclate_distance_to_landing(tetris_compute_state_t *compute_state_ptr);
static bool check_is_game_over(tetris_compute_state_t *compute_state_ptr);
static void erase_field_row(bitmap_128_t field_bitmap);
static mino_is_collide_t shift_mino(tetris_compute_state_t *compute_state_ptr, int8_t shift_x_level, int8_t shift_y_level);
static void get_mino_bitmap(bitmap_128_t array_dst, const bitmap_128_t mino_definition, tetris_mino_type_t mino_type, tetris_mino_turn_state_t turn);
static void update_game_parameter(tetris_game_parameter_t *game_parameter_ptr);

//======================================================
// 公開関数定義
//======================================================
/**
 * @brief ゲーム開始判定
 * @param input_state_ptr 入力状態
 * @return 次ゲームステート
 * @details コントロールボタン1、コントロールボタン2のいずれかが押された場合ゲーム開始判定とする
 */
tetris_game_state_t tetris_judge_game_start(tetris_input_state_t *input_state_ptr)
{
    return (input_state_ptr->is_input_control_button2 || input_state_ptr->is_input_control_button1) ? game_start_initialization : game_waiting_start;
}

/**
 * @brief ゲーム実行中 演算メイン処理
 * @param input_state_ptr 入力状態
 * @param compute_state_ptr 演算状態
 * @return 次ゲームステート
 * @details ゲーム実行中の演算フロー全体を処理する
 *          ミノ生成・回転・移動・接地判定・行消去・ゲームパラメータ更新を実行し、遷移先ステートを返す
 */
tetris_game_state_t tetris_data_compute_in_game(tetris_input_state_t *input_state_ptr, tetris_compute_state_t *compute_state_ptr)
{
    // ミノの生成と初期配置（ゲーム開始直後 or 前回周期でミノが接地した場合に実行）
    if (compute_state_ptr->mino_parameter.is_next_mino_generate)
    {
        generate_new_mino(&compute_state_ptr->mino_parameter); // 座標0,0に新ミノ生成
        move_mino_initial_position(compute_state_ptr);         // 初期位置にミノをシフト（できなければゲームオーバーになる）
        allow_down_shift = false;                              // 下シフト禁止（直前の入力からの誤入力防止）
    }

    // ミノ回転処理
    turn_mino(compute_state_ptr, input_state_ptr);

    // ミノ移動処理＆下面接地判定
    mino_is_collide_t is_collided_bottom = move_mino(compute_state_ptr, input_state_ptr);

    bool is_gameover = false;
    if (is_collided_bottom)
    {
        // 下面に衝突 → ゲームオーバー判定＆得点処理
        BITMAP_or(compute_state_ptr->field_parameter.bitmap, compute_state_ptr->mino_parameter.bitmap); // ボックスのビットマップにミノを加える
        BITMAP_copy(compute_state_ptr->mino_parameter.bitmap, tetris_bitmap_def_zero);                  // 操作ミノを消去する（操作ミノ用のビットマップレイヤーを0クリアする）
        erase_field_row(compute_state_ptr->field_parameter.bitmap);                                     // ブロック行消去判定
        update_game_parameter(&compute_state_ptr->game_parameter);                                      // スコア等更新処理
        is_gameover = check_is_game_over(compute_state_ptr);                                            // ゲームオーバー判定
    }
    else
    {
        // 下面に衝突なし → 落下までの距離を計算（落下位置の描画用に使う）
        caluclate_distance_to_landing(compute_state_ptr);
    }

    // ステート移行判定：ゲームオーバーorゲーム継続実行を返す
    return (is_gameover) ? game_over : game_running;
}

/**
 * @brief ゲームリスタート判定
 * @param input_state_ptr 入力状態
 * @return 次ゲームステート
 * @details コントロールボタン1、コントロールボタン2のいずれかが押された場合ゲームリスタート判定とする
 */
tetris_game_state_t tetris_judge_game_restart(tetris_input_state_t *input_state_ptr)
{
    return (input_state_ptr->is_input_control_button2 || input_state_ptr->is_input_control_button1) ? game_start_initialization : game_over;
}

/**
 * @brief 演算状態初期化
 * @param compute_state_ptr 演算状態格納先
 * @return なし
 * @details 各種演算用パラメータをゲーム開始時の初期値へ設定する
 *          ゲーム開始時＆ゲームオーバー後のゲームリスタート時に毎回呼ばれる
 */
void tetris_initialize_data_compute(tetris_compute_state_t *compute_state_ptr)
{
    // ミノパラメータ初期化（ネクスト以外はミノ生成関数で初期化されるので不要）
    compute_state_ptr->mino_parameter.next_mino_type = TIMER_get_time_us() % NUMBER_MINO_TYPES; // 疑似乱数　TODO：mid層に関数実装
    compute_state_ptr->mino_parameter.is_next_mino_generate = true;                             // ミノ生成

    // フィールドパラメータ初期化（ボックスをコピーしてくる）
    BITMAP_copy(compute_state_ptr->field_parameter.bitmap, tetris_bitmap_def_box);

    // ゲームパラメータ初期化
    compute_state_ptr->game_parameter.level = 1;
    compute_state_ptr->game_parameter.row_deleted = 0;
    compute_state_ptr->game_parameter.score = 0;
    compute_state_ptr->game_parameter.is_updated = true; // UIを表示させる必要があるためtrue
}

//======================================================
// 内部関数定義
//======================================================
/**
 * @brief 新規ミノ生成
 * @param mino_parameter_ptr ミノパラメータ
 * @return なし
 * @details 現在のネクストミノ種別からミノを生成し、次のネクストミノを疑似乱数で更新する
 * @todo ネクストミノの疑似乱数生成はmid層に移動を検討（再利用性のため）
 */
static void generate_new_mino(tetris_mino_parameter_t *mino_parameter_ptr)
{
    // ミノ種別のパラメータ更新
    tetris_mino_type_t mino_type = mino_parameter_ptr->next_mino_type;            // 今回生成するミノ種別
    mino_parameter_ptr->next_mino_type = TIMER_get_time_us() % NUMBER_MINO_TYPES; // 疑似乱数でネクストミノ種別を決定する

    // 今回生成するミノ種別のビットマップを取得する
    get_mino_bitmap(mino_parameter_ptr->bitmap, tetris_bitmap_def_mino, mino_type, r_no_turn);

    // ミノ新規生成後のパラメータ初期化
    mino_parameter_ptr->reference_x = 0;
    mino_parameter_ptr->reference_y = 0;
    mino_parameter_ptr->mino_type = mino_type;
    mino_parameter_ptr->turn_state = r_no_turn;
    mino_parameter_ptr->is_next_mino_generate = false;
}

/**
 * @brief ミノ初期位置移動
 * @param compute_state_ptr 演算状態
 * @return なし
 * @details ミノは新規生成直後はビットマップの左上に位置しているため、初期位置（ゲームオーバーラインの中央真上）に移動する
 *          既に積まれているブロックに移動を阻害される場合はゲームオーバーになる（これは本来生じない状況のはずだが、念のため）
 */
static void move_mino_initial_position(tetris_compute_state_t *compute_state_ptr)
{
    // X軸方向に移動（プレイフィールドの中央に寄せる）
    (void)shift_mino(compute_state_ptr, MINO_X_INITIAL, 0);

    // Y方向に移動（ゲームオーバーラインの上。接触の可能性があるので1つずつずらす）
    uint8_t y_shift_counter = MINO_Y_INITIAL;
    while (y_shift_counter)
    {
        if (shift_mino(compute_state_ptr, 0, 1))
            break;

        y_shift_counter--;
    }
}

/**
 * @brief ミノ回転処理
 * @param compute_state_ptr 演算状態
 * @param input_state_ptr 入力状態
 * @return なし
 * @details ボタン入力に応じてミノを90°回転させる
 *          回転後ビットマップを生成し、フィールド衝突しない場合のみ回転状態を反映する
 */
static void turn_mino(tetris_compute_state_t *compute_state_ptr, tetris_input_state_t *input_state_ptr)
{
    // 回転数算出（正で右回転、負で左回転）
    int turnR_value = (int)(input_state_ptr->is_input_turnR_button) - (int)(input_state_ptr->is_input_turnL_button);
    if (!turnR_value) // 回転無し→処理せず即リターン
        return;

    // 回転後のミノを衝突判定用に生成
    bitmap_128_t turned_mino = {0};
    tetris_mino_turn_state_t state_after_turned = MATH_modulo(compute_state_ptr->mino_parameter.turn_state + turnR_value, r_3_turn + 1);
    get_mino_bitmap(turned_mino, tetris_bitmap_def_mino, compute_state_ptr->mino_parameter.mino_type, state_after_turned);
    BITMAP_shift(turned_mino, compute_state_ptr->mino_parameter.reference_x, compute_state_ptr->mino_parameter.reference_y); // 演算用ミノの基準点を使って位置を再現する

    // 回転後のミノとボックスの衝突判定＝回転させられるか判定する
    bool is_collide = BITMAP_check_overlap(turned_mino, compute_state_ptr->field_parameter.bitmap);

    // 衝突しない場合のみ、演算用ミノを上書きして終了
    if (!is_collide)
    {
        BITMAP_copy(compute_state_ptr->mino_parameter.bitmap, turned_mino);
        compute_state_ptr->mino_parameter.turn_state = state_after_turned;
    }
}

/**
 * @brief ミノ移動処理
 * @param compute_state_ptr 演算状態
 * @param input_state_ptr 入力状態
 * @return 下面接地判定結果
 * @details 操作中のミノの左右・下移動を処理する
 *          左右移動はスティックの左右入力があった時のみ行われる
 *          下移動はスティックの下入力が無い場合も自由落下が行われる。下入力があった場合は高速落下になる。また、落下後にミノが接地したかを判定する
 *          スティックの入力があれば即移動とするのではなく、入力が何周期連続しているかをカウントし、そのカウントが閾値を超えた時に移動処理を実行する
 */
static mino_is_collide_t move_mino(tetris_compute_state_t *compute_state_ptr, tetris_input_state_t *input_state_ptr)
{
    /* 左右移動 */
    // 入力方向に対するカウンターのインクリメント　入力無しなら0クリア
    mino_move_counter.L = (input_state_ptr->is_input_L) ? mino_move_counter.L + 1 : 0;
    mino_move_counter.R = (input_state_ptr->is_input_R) ? mino_move_counter.R + 1 : 0;

    // カウンターに応じた左右移動処理
    if (MINO_MOVE_L_TH < mino_move_counter.L) // MINO_MOVE_L_THを超えて左移動の入力が継続された場合、1ドット分左シフトする
    {
        shift_mino(compute_state_ptr, -1, 0); // 左に1ブロック移動させる
        mino_move_counter.L = 0;
    }
    if (MINO_MOVE_R_TH < mino_move_counter.R) // MINO_MOVE_R_THを超えて右移動の入力が継続された場合、1ドット分右シフトする
    {
        shift_mino(compute_state_ptr, 1, 0); // 右に1ブロック移動させる
        mino_move_counter.R = 0;
    }

    /* 下移動 */
    // カウンターのインクリメント
    if (!allow_down_shift && input_state_ptr->is_input_D) // ミノを再生成した時、直前からの下入力が継続されている場合
    {
        // この場合は誤入力防止のため高速落下させない = 入力に関わらず自由落下分しかカウンタを増やさない
        mino_move_counter.D += free_fall_confficient[compute_state_ptr->game_parameter.level];
    }
    else // それ以外の時
    {
        // 下入力されていれば高速落下、されてなければ自由落下分カウンタを増やす
        allow_down_shift = true;
        mino_move_counter.D = (input_state_ptr->is_input_D) ? MINO_MOVE_D_TH + 1 : mino_move_counter.D + free_fall_confficient[compute_state_ptr->game_parameter.level];
    }

    // カウンターに応じた下移動処理
    mino_is_collide_t is_collided_bottom = false;
    if (MINO_MOVE_D_TH < mino_move_counter.D) // 下入力あれば必ず閾値を超える = 最速落下になるようにしている
    {
        is_collided_bottom = shift_mino(compute_state_ptr, 0, 1); // 下に1ブロック移動させる。落下が実行できなかった時は下面に接地と判定する
        mino_move_counter.D = 0;
    }

    // 下移動で下面に接地したかを返す
    return is_collided_bottom;
}

/**
 * @brief 落下予測距離算出
 * @param compute_state_ptr 演算状態
 * @return なし
 * @details 現在操作中のミノが接地するまでにあと何ブロック分落下必要かを算出する
 *          ミノを1ドットずつ下げて、フィールドと衝突するまでの距離をカウントする
 */
static void caluclate_distance_to_landing(tetris_compute_state_t *compute_state_ptr)
{
    bitmap_128_t mino_copy; // 落下判定用のコピー
    BITMAP_copy(mino_copy, compute_state_ptr->mino_parameter.bitmap);

    // ミノを1ドットずつ下げて、落下までの距離をカウントする
    uint8_t falling_counter = 0;
    while (falling_counter < 127) // バグによる無限ループ防止
    {
        BITMAP_shift(mino_copy, 0, 1);
        if (BITMAP_check_overlap(mino_copy, compute_state_ptr->field_parameter.bitmap))
        {
            break;
        }
        else
        {
            falling_counter++;
        }
    }
    compute_state_ptr->mino_parameter.distance_to_landing = falling_counter;
}

/**
 * @brief ゲームオーバー判定
 * @param compute_state_ptr 演算状態
 * @return ゲームオーバー判定結果
 * @details フィールドのビットマップと、ゲームオーバー判定用のビットマップ（ゲームオーバーラインが埋まっている）との重なりをチェックすることで
 *          ゲームオーバーかどうかを判定する
 */
static bool check_is_game_over(tetris_compute_state_t *compute_state_ptr)
{
    if (BITMAP_check_overlap(compute_state_ptr->field_parameter.bitmap, tetris_bitmap_def_check_box_full_layer))
    {
        // ゲームオーバー確定
        return true;
    }
    else
    {
        // ゲームオーバーではない事確定
        compute_state_ptr->mino_parameter.is_next_mino_generate = true; // 次のループでのミノ再生成を予約
        return false;
    }
}

/**
 * @brief フィールド行消去処理
 * @param field_bitmap フィールドビットマップ
 * @return なし
 * @details 横1列にブロックが揃っている行を検出して消去する
 *          消去した場合はその消去行数を更新する。この数値はスコア等の更新処理に使われる
 */
static void erase_field_row(bitmap_128_t field_bitmap)
{
    // 行が揃っているかの判定
    for (int y_check = 23; 23 - 18 < y_check; y_check--)
    {
        bool is_row_full = true;

        for (int x_check = 1; x_check < 1 + 10; x_check++)
        {
            if (!BITMAP_read(field_bitmap, y_check, x_check))
            {
                is_row_full = false;
                break;
            }
        }

        // 揃った行の消去＆段下げ
        if (is_row_full)
        {
            for (int y_update = y_check; 23 - 18 < y_update; y_update--)
            {
                // Boxの壁ごとコピーする。少し気持ち悪いがこちらの方が早い
                field_bitmap[y_update][0] = field_bitmap[y_update - 1][0];
                field_bitmap[y_update][1] = field_bitmap[y_update - 1][1]; // 一応
            }

            y_check++;    // これが無いと消えた行に下がってきた行を判定できない
            row_erased++; // 消去した行数。スコア計算用
        }
        else
        {
        }
    }
}

/**
 * @brief ゲームパラメータ更新
 * @param game_parameter_ptr ゲームパラメータ
 * @return なし
 * @details ミノの接地による行消去があった場合、その消去行数に応じてスコアやレベルなどのゲームパラメータを更新する
 */
static void update_game_parameter(tetris_game_parameter_t *game_parameter_ptr)
{
    if (!row_erased) // 行の消去無し → パラメータ更新無し
    {
        game_parameter_ptr->is_updated = false;
    }
    else // 行の消去有り → パラメータ更新実行
    {
        // 消去行総数更新
        game_parameter_ptr->row_deleted += row_erased;
        // スコア更新：一度に多くの行を消去する程スコア増
        game_parameter_ptr->score += (score_power_rate[row_erased] * row_erased) * (9 + game_parameter_ptr->level);
        // レベル更新：消去行総数が一定を超える毎にレベルアップ
        if ((game_parameter_ptr->level < MAXIMUM_LEVEL) && (next_level_need_row[game_parameter_ptr->level] < game_parameter_ptr->row_deleted))
        {
            game_parameter_ptr->level++;
        }

        row_erased = 0;                        // 行消去数リセット
        game_parameter_ptr->is_updated = true; // パラメータ更新したのでUIを更新させる必要があるためtrue
    }
}

/**
 * @brief ミノシフト処理
 * @param compute_state_ptr 演算状態
 * @param shift_x_level X方向シフト量
 * @param shift_y_level Y方向シフト量
 * @return シフト時衝突判定結果
 * @details ビットマップと基準点を同時に移動し、衝突時は状態更新を行わない
 */
static mino_is_collide_t shift_mino(tetris_compute_state_t *compute_state_ptr, int8_t shift_x_level, int8_t shift_y_level)
{
    // ミノとボックスの衝突判定
    bitmap_128_t mino_copy;
    BITMAP_copy(mino_copy, compute_state_ptr->mino_parameter.bitmap);
    BITMAP_shift(mino_copy, shift_x_level, shift_y_level);
    bool is_collide = BITMAP_check_overlap(mino_copy, compute_state_ptr->field_parameter.bitmap);

    if (is_collide) // 衝突する場合：シフトしない
    {
        return collided;
    }
    else // 衝突しない場合：シフト処理
    {
        BITMAP_shift(compute_state_ptr->mino_parameter.bitmap, shift_x_level, shift_y_level);
        compute_state_ptr->mino_parameter.reference_x += shift_x_level;
        compute_state_ptr->mino_parameter.reference_y += shift_y_level;
        return not_collided;
    }
}

/**
 * @brief ミノビットマップ抽出
 * @param array_dst 出力先ビットマップ
 * @param mino_definition ミノ定義ビットマップ
 * @param mino_type ミノ種別
 * @param turn 回転状態
 * @return なし
 * @details ネクストミノなどのビットマップは、1枚の128×128ビットマップに複数のミノを並べて埋め込んでいる（容量削減のため）
 *          ミノの種別と回転状態を指定することで、その1枚のビットマップから欲しいミノを抽出する
 */
static void get_mino_bitmap(bitmap_128_t array_dst, const bitmap_128_t mino_definition, tetris_mino_type_t mino_type, tetris_mino_turn_state_t turn)
{
    BITMAP_copy(array_dst, tetris_bitmap_def_zero);

    // ビットマップからの抽出対象座標計算（これでうまく抽出できるようにミノが配置されている）
    uint8_t start_x = turn * MINO_DEF_LENGTH;
    uint8_t end_X = ((turn + 1) * MINO_DEF_LENGTH) - 1;
    uint8_t start_y = mino_type * MINO_DEF_LENGTH;
    uint8_t end_y = ((mino_type + 1) * MINO_DEF_LENGTH) - 1;

    // ビットマップデータ抽出
    BITMAP_extract(array_dst, mino_definition, start_x, end_X, start_y, end_y);
}