/**
 * @file   I2C_init.c
 * @brief  I2Cコンポーネント・初期化実装
 */

//======================================================
// インクルード
//======================================================
#include "I2C.h"
#include "I2C_internal.h"
#include "register.h"

//======================================================
// マクロ定義
//======================================================

//======================================================
// 型定義
//======================================================

//======================================================
// 変数・定数
//======================================================

//======================================================
// プロトタイプ宣言
//======================================================
static void initialize_master_mode(I2C_config_t config);
static void initialize_slave_mode(I2C_config_t config);

//======================================================
// 公開関数定義
//======================================================
/**
 * @brief I2C 初期化
 * @param  config I2C設定用パラメータ
 * @return なし
 */
void I2C_initialize(I2C_config_t config)
{
    // masterとslaveで初期化シーケンスが異なる
    if (master == config.mode)
    {
        initialize_master_mode(config);
    }
    else // slave
    {
        initialize_slave_mode(config);
    }
}

//======================================================
// 内部関数定義
//======================================================
/**
 * @brief I2C マスターモード初期化
 * @param config I2C設定パラメータ
 * @return なし
 */
static void initialize_master_mode(I2C_config_t config)
{
    /* Config情報を元にI2Cを初期化：データシート 4.3.10.2.1. マスター初期構成手順を参照 */

    // I2Cを一度無効化
    i2c_set_enable(config.ch, DISABLE);

    // 各種パラメータ設定
    i2c_set_speed(config.ch, config.speed);
    i2c_set_addressing_mode(config.ch, master, config.addressing_mode);
    i2c_select_master_or_slave(config.ch, master);
    i2c_set_default_address(config.ch, master, config.default_address);

    // SCLのHigh/Lowカウント設定（= SCL周期 ≒ H+L）
    // 410KHz TODO：計算がクソ適当なのでデータシート読んで修正する
    I2Cn_IC_SS_SCL_HCNT(config.ch) = 120; // 高期間 (単位: ic_clk)
    I2Cn_IC_SS_SCL_LCNT(config.ch) = 144; // 低期間 (単位: ic_clk)

    // spike除去 (optional)
    I2Cn_IC_FS_SPKLEN(config.ch) = 1; // 10ns抑制など（条件により異なる）

    // I2Cを有効化
    i2c_set_enable(config.ch, ENABLE);
}

/**
 * @brief I2C スレーブモード初期化
 * @param config I2C設定パラメータ
 * @return なし
 */
static void initialize_slave_mode(I2C_config_t config)
{
    /* Config情報を元にI2Cを初期化：データシート 4.3.10.1.1. スレーブ初期構成手順を参照 */

    // I2Cを一度無効化
    i2c_set_enable(config.ch, DISABLE);

    // 各種パラメータ設定
    i2c_set_default_address(config.ch, slave, config.default_address); // アドレス設定
    i2c_set_addressing_mode(config.ch, slave, config.addressing_mode); // アドレッシングモード設定
    i2c_select_master_or_slave(config.ch, slave);                      // スレーブモードに設定

    // I2Cを有効化
    i2c_set_enable(config.ch, ENABLE);
}