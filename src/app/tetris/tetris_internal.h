/**
 * @file   tetris_internal.h
 * @brief  tetris・内部公開定義
 */

#ifndef __TETRIS_INTERNAL_H__
#define __TETRIS_INTERNAL_H__

//======================================================
// インクルード
//======================================================
#include "debug_com.h"
#include "typedef.h"
#include "bitmap_lib.h"

//======================================================
// マクロ定義
//======================================================

//======================================================
// 型定義
//======================================================
/**
 * @brief ゲームステート定義
 */
typedef enum
{
    game_waiting_start = 0,    /**< スタート待機中 */
    game_start_initialization, /**< 初期化中 */
    game_running,              /**< ゲーム実行中 */
    game_over,                 /**< ゲームオーバー */
    game_pause,                /**< ポーズ中 */
} tetris_game_state_t;

/**
 * @brief 入力ステート定義
 */
typedef struct
{
    bool is_input_R;               /**< 右入力（アナログスティック） */
    bool is_input_L;               /**< 左入力（アナログスティック） */
    bool is_input_U;               /**< 上入力（アナログスティック） */
    bool is_input_D;               /**< 下入力（アナログスティック） */
    bool is_input_turnR_button;    /**< 右回転ボタン入力 */
    bool is_input_turnL_button;    /**< 左回転ボタン入力 */
    bool is_input_control_button2; /**< コントロールボタン2入力 */
    bool is_input_control_button1; /**< コントロールボタン1入力 */
} tetris_input_state_t;

/**
 * @brief ミノ種別定義
 */
typedef enum
{
    mino_I = 0, /**< I字型ミノ */
    mino_J,     /**< J字型ミノ */
    mino_L,     /**< L字型ミノ */
    mino_O,     /**< O字型ミノ */
    mino_S,     /**< S字型ミノ */
    mino_T,     /**< T字型ミノ */
    mino_Z,     /**< Z字型ミノ */
} tetris_mino_type_t;

/**
 * @brief ミノ回転状態定義
 */
typedef enum
{
    r_no_turn = 0, /**< 回転無し（基準状態） */
    r_1_turn,      /**< 右に1回転の状態 */
    r_2_turn,      /**< 右に2回転の状態 */
    r_3_turn,      /**< 右に3回転の状態 */
} tetris_mino_turn_state_t;

/**
 * @brief ミノ演算パラメータ定義
 */
typedef struct
{
    bitmap_128_t bitmap;                 /**< ミノの演算用ビットマップ */
    uint8_t reference_x;                 /**< ミノの基準点（X軸） */
    uint8_t reference_y;                 /**< ミノの基準点（Y軸） */
    uint8_t distance_to_landing;         /**< ミノの現在地点から着地点までの距離 */
    tetris_mino_turn_state_t turn_state; /**< ミノの回転状態 */
    tetris_mino_type_t mino_type;        /**< ミノの種別 */
    tetris_mino_type_t next_mino_type;   /**< ネクストミノの種別 */
    bool is_next_mino_generate;          /**< 次回ループでのミノ新規生成フラグ */
} tetris_mino_parameter_t;

/**
 * @brief フィールド演算ビットマップ定義
 * @details フィールドは接地済みミノとボックス枠で構成され、詳細は以下
 *          0～1行：バッファ、
 *          2～3行：操作ミノ生成、
 *          4～23行：ブロック描画範囲
 */
typedef struct
{
    bitmap_128_t bitmap; /**< フィールド演算用ビットマップ */
} tetris_field_parameter_t;

/**
 * @brief ゲーム制御パラメータ定義
 */
typedef struct
{
    uint8_t level;        /**< UI表示用ゲームレベル */
    uint16_t row_deleted; /**< UI表示用合計消去行数 */
    uint16_t score;       /**< UI表示用ゲームスコア */
    bool is_updated;      /**< ゲーム制御パラメータ更新有無 */
} tetris_game_parameter_t;

/**
 * @brief 演算ステート定義
 */
typedef struct
{
    tetris_mino_parameter_t mino_parameter;   /**< ミノ演算パラメータ */
    tetris_field_parameter_t field_parameter; /**< フィールド演算パラメータ */
    tetris_game_parameter_t game_parameter;   /**< ゲーム制御パラメータ */
} tetris_compute_state_t;

// デバッグ実行関数ポインタ定義
typedef void (*tetris_cmd_fn_ptr_t)(const DEBUG_COM_debug_frame_t *);

/**
 * @brief デバッグコマンド構造定義
 */
typedef struct
{
    uint8_t cmd;            /**< デバッグコマンド */
    tetris_cmd_fn_ptr_t fn; /**< デバッグコマンドに対する実行関数ポインタ */
} cmd_list_t;

//======================================================
// グローバル変数・定数extern宣言
//======================================================
/* const_bitmap */
extern const bitmap_128_t tetris_bitmap_def_fixed_UI;
extern const bitmap_128_t tetris_bitmap_def_numbers;
extern const bitmap_128_t tetris_bitmap_def_start_message;
extern const bitmap_128_t tetris_bitmap_def_restart_message;
extern const bitmap_128_t tetris_bitmap_def_restart_message_bold;
extern const bitmap_128_t tetris_bitmap_def_falling_point_layer;
extern const bitmap_128_t tetris_bitmap_def_field_layer;
extern const bitmap_128_t tetris_bitmap_def_mino;
extern const bitmap_128_t tetris_bitmap_def_next_mino_1;
extern const bitmap_128_t tetris_bitmap_def_next_mino_2;
extern const bitmap_128_t tetris_bitmap_def_box;
extern const bitmap_128_t tetris_bitmap_def_check_box_full_layer;
extern const bitmap_128_t tetris_bitmap_def_zero;

/* debug_cmd_def */
extern const cmd_list_t tetris_cmd_list[];

//======================================================
// グローバル関数extern宣言
//======================================================
/* main → input_ctrl */
extern void tetris_initialize_input_ctrl(tetris_input_state_t *input_state_ptr);
extern void tetris_receive_game_start_input(TETRIS_input_parameter_t *input_handler, tetris_input_state_t *input_state_ptr);
extern void tetris_input_ctrl_in_game(TETRIS_input_parameter_t *input_handler, tetris_input_state_t *input_state_ptr);
extern void tetris_receive_game_restart_input(TETRIS_input_parameter_t *input_handler, tetris_input_state_t *input_state_ptr);

/* main → data_compute */
extern void tetris_initialize_data_compute(tetris_compute_state_t *mino_compute_data);
extern tetris_game_state_t tetris_judge_game_start(tetris_input_state_t *input_state_ptr);
extern tetris_game_state_t tetris_data_compute_in_game(tetris_input_state_t *input_state_ptr, tetris_compute_state_t *mino_compute_data);
extern tetris_game_state_t tetris_judge_game_restart(tetris_input_state_t *input_state_ptr);

/* main → display_ctrl */
extern void tetris_initialize_display_ctrl();
extern void tetris_display_waiting_start();
extern void tetris_display_ctrl_in_game(tetris_compute_state_t *mino_compute_data);
extern void tetris_display_waiting_restart();

/* main → debug_ctrl */
extern void tetris_execute_debug_process(void);

/* debug_ctrl → debug_cmd_def */
extern uint8_t tetris_get_sizeof_cmd();

/* debug_cmd_def → main */
extern void tetris_debug_pause_enale(bool is_enable);
extern tetris_game_state_t tetris_get_game_state();

#endif /* __TETRIS_INTERNAL_H__ */