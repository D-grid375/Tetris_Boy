/**
 * @file   tetris_debug_cmd_def.c
 * @brief  tetris・debug機能_コマンド定義
 */

//======================================================
// インクルード
//======================================================
#include "tetris.h"
#include "tetris_internal.h"
#include "register.h"
#include "typedef.h"
#include "bit.h"

//======================================================
// マクロ定義
//======================================================
#define NO_DATA_LEN 0
#define NO_DATA NULL

//======================================================
// 型定義
//======================================================

//======================================================
// プロトタイプ宣言
//======================================================
// cmd実行関数を定義
static void enable_game_pause(const DEBUG_COM_debug_frame_t *receive_frame);
static void read_register(const DEBUG_COM_debug_frame_t *receive_frame);
static void read_game_state(const DEBUG_COM_debug_frame_t *receive_frame);

//======================================================
// 変数・定数
//======================================================
// コマンドリスト：cmd番号,cmd実行関数を定義
const cmd_list_t tetris_cmd_list[] = {
    {0x55, enable_game_pause}, // ポーズ有効・無効
    {0x56, read_game_state},   // ゲームステート読み出し
    {0x60, read_register},     // 汎用レジスタ読み出し
};

//======================================================
// 公開関数定義
//======================================================
/**
 * @brief デバッグコマンド数取得
 * @return コマンド定義数
 */
uint8_t tetris_get_sizeof_cmd()
{
    return sizeof(tetris_cmd_list) / sizeof(tetris_cmd_list[0]);
}

//======================================================
// 内部関数定義
//======================================================
/**
 * @brief ゲームポーズ有効化コマンド実行
 * @param receive_frame 受信デバッグフレーム
 * @return なし
 */
static void enable_game_pause(const DEBUG_COM_debug_frame_t *receive_frame)
{
    bool is_pause_enable = receive_frame->data[0];
    tetris_debug_pause_enale(is_pause_enable); // tetris_main内関数

    DEBUG_COM_send(receive_frame->cmd, NO_DATA_LEN, NO_DATA);
}

/**
 * @brief ゲームステート読出しコマンド実行
 * @param receive_frame 受信デバッグフレーム
 * @return なし
 */
static void read_game_state(const DEBUG_COM_debug_frame_t *receive_frame)
{
    uint8_t game_state = (uint8_t)(tetris_get_game_state());

    DEBUG_COM_send(receive_frame->cmd, 1, &game_state);
}

/**
 * @brief レジスタ値読出しコマンド実行
 * @param receive_frame 受信デバッグフレーム
 * @return なし
 */
static void read_register(const DEBUG_COM_debug_frame_t *receive_frame)
{
    uint32_t register_address = 0;
    register_address |= (receive_frame->data[0] << 0);
    register_address |= (receive_frame->data[1] << 8);
    register_address |= (receive_frame->data[2] << 16);
    register_address |= (receive_frame->data[3] << 24);

    uint32_t register_value = VOLATILE_ACCESS(register_address);

    uint8_t response_data[4];
    response_data[0] = (register_value >> 0) & MASK_8BIT;
    response_data[1] = (register_value >> 8) & MASK_8BIT;
    response_data[2] = (register_value >> 16) & MASK_8BIT;
    response_data[3] = (register_value >> 24) & MASK_8BIT;

    DEBUG_COM_send(receive_frame->cmd, sizeof(response_data), response_data);
}