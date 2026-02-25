/**
 * @file   tetris_debug_ctrl.c
 * @brief  tetris・debug機能_共通処理実装
 */

//======================================================
// インクルード
//======================================================
#include "tetris.h"
#include "tetris_internal.h"
#include "debug_com.h"
#include "typedef.h"
#include <string.h>

//======================================================
// マクロ定義
//======================================================

//======================================================
// 型定義
//======================================================

//======================================================
// プロトタイプ宣言
//======================================================
static tetris_cmd_fn_ptr_t find_cmd_fn(uint8_t cmd);
static void execute_cmd_fn(const tetris_cmd_fn_ptr_t fn_ptr, const DEBUG_COM_debug_frame_t *frame_ptr);

//======================================================
// 変数・定数
//======================================================

//======================================================
// 公開関数定義
//======================================================
/**
 * @brief デバッグフレーム処理実行
 * @return なし
 * @details 受信済みフレームを走査し、cmdに対応する実行関数を探索して順次実行する
 */
void tetris_execute_debug_process(void)
{
    DEBUG_COM_debug_frame_t frame;

    // 受信済みフレームを全て処理
    while (DEBUG_COM_read_frame(&frame))
    {
        tetris_cmd_fn_ptr_t fn_ptr = find_cmd_fn(frame.cmd); // cmdに対応した関数を探索
        execute_cmd_fn(fn_ptr, &frame);                      // 関数実行（fn_ptrがNULL以外の時）
    }
}

//======================================================
// 内部関数定義
//======================================================
/**
 * @brief デバッグコマンド関数探索
 * @param cmd コマンド番号
 * @return 対応コマンド関数ポインタ
 */
static tetris_cmd_fn_ptr_t find_cmd_fn(uint8_t cmd)
{
    for (unsigned i = 0; i < tetris_get_sizeof_cmd(); i++)
    {
        if (tetris_cmd_list[i].cmd == cmd)
            return tetris_cmd_list[i].fn;
    }
    return NULL; // 関数未定義の場合
}

/**
 * @brief デバッグコマンド関数実行
 * @param fn_ptr 実行関数ポインタ
 * @param frame_ptr 受信フレーム
 * @return なし
 * @details 関数ポインタがNULLでない場合のみ受信フレームを引数に関数を呼び出す
 */
static void execute_cmd_fn(const tetris_cmd_fn_ptr_t fn_ptr, const DEBUG_COM_debug_frame_t *frame_ptr)
{
    // NULLでなければ関数実行
    if (fn_ptr)
        fn_ptr(frame_ptr);
}