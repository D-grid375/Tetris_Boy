/**
 * @file   I2C_internal.h
 * @brief  I2Cコンポーネント・内部公開定義
 */

#ifndef __I2C_INTERNAL_H__
#define __I2C_INTERNAL_H__

//======================================================
// インクルード
//======================================================
#include "I2C.h"

//======================================================
// マクロ定義
//======================================================

//======================================================
// 型定義
//======================================================

//======================================================
// グローバル変数・定数extern宣言
//======================================================

//======================================================
// グローバル関数extern宣言
//======================================================
/* init → ops */
extern void i2c_set_enable(I2C_ch_t ch, bool is_enable);
extern void i2c_select_master_or_slave(I2C_ch_t ch, I2C_mode_t mode);
extern void i2c_set_speed(I2C_ch_t ch, I2C_speed_t speed);
extern void i2c_set_addressing_mode(I2C_ch_t ch, I2C_mode_t mode, I2C_addressing_mode_t addressing_mode);
extern void i2c_set_default_address(I2C_ch_t ch, I2C_mode_t mode, uint16_t default_address);

/* ctrl → ops */
extern void i2c_clear_rd_req(I2C_ch_t ch);
extern bool i2c_check_rd_req(I2C_ch_t ch);

#endif /* __I2C_INTERNAL_H__ */