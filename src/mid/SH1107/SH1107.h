/**
 * @file   SH1107.h
 * @brief  SH1107コンポーネント・外部公開定義
 */

#ifndef __SH1107_H__
#define __SH1107_H__

//======================================================
// インクルード
//======================================================
#include "I2C.h"
#include "typedef.h"
#include "bitmap_lib.h"

//======================================================
// マクロ定義
//======================================================

//======================================================
// 型定義
//======================================================
/**
 * @brief SH1107初期化設定定義
 * @details 通信に使用するI2Cチャネルを指定する
 *          configでこの定義に対する実パラメータを設定し、初期化関数に渡す事で設定を反映させる
 */
typedef struct
{
    I2C_ch_t assign_I2C_ch;
} SH1107_config_t;

//======================================================
// グローバル変数・定数extern宣言
//======================================================

//======================================================
// グローバル関数extern宣言
//======================================================
/* init */
extern void SH1107_initialize(const SH1107_config_t config_SH1107);

/* ops */

/* ctrl */
extern bool SH1107_display_bitmap_data(bitmap_128_t bitmap);
extern bool SH1107_display_bitmap_all_data(bitmap_128_t bitmap);
extern bool SH1107_display_bitmap_updated_data(bitmap_128_t current_bitmap, bitmap_128_t previous_bitmap);

#endif /* __SH1107_H__ */