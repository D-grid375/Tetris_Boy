/**
 * @file   debug_com.h
 * @brief  デバッグ通信コンポーネント・外部公開定義
 */

#ifndef __DEBUG_COM_H__
#define __DEBUG_COM_H__

//======================================================
// インクルード
//======================================================
#include "I2C.h"
#include "typedef.h"

//======================================================
// マクロ定義
//======================================================
#define DEBUG_COM_MAX_DATA_LEN 16u

//======================================================
// 型定義
//======================================================
/**
 * @brief デバッグ通信フレーム定義
 */
typedef struct
{
    uint8_t cmd;                          /**< cmd：1byte */
    uint8_t data_len;                     /**< データ長：0~8byte */
    uint8_t data[DEBUG_COM_MAX_DATA_LEN]; /**< データ：data_lenで指定された長さのデータ */
} DEBUG_COM_debug_frame_t;

/**
 * @brief デバッグ通信 送信ステータス定義
 */
typedef enum
{
    ok = 0,       /**< 送信成功 */
    err_len = -1, /**< データ長異常 */
} DEBUG_COM_status_t;

/**
 * @brief デバッグ通信初期化設定定義
 * @details デバッグ通信に使用するI2Cチャネルを指定する
 *          configでこの定義に対する実パラメータを設定し、初期化関数に渡す事で設定を反映させる
 */
typedef struct
{
    I2C_ch_t I2C_ch; /**< デバッグ通信に使用するI2Cチャネル */
} DEBUG_COM_config_t;

//======================================================
// グローバル変数・定数extern宣言
//======================================================

//======================================================
// グローバル関数extern宣言
//======================================================
/* init */
extern bool DEBUG_COM_initialize(DEBUG_COM_config_t config);

/* ops */
extern bool DEBUG_COM_read_frame(DEBUG_COM_debug_frame_t *out);
extern DEBUG_COM_status_t DEBUG_COM_send(uint8_t cmd, uint8_t data_len, const uint8_t *data);

#endif /* __DEBUG_COM_H__ */