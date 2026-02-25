/**
 * @file   I2C_ops.c
 * @brief  I2Cコンポーネント・レジスタ操作実装
 */

//======================================================
// インクルード
//======================================================
#include "I2C.h"
#include "I2C_internal.h"
#include "gpio.h"
#include "register.h"
#include "bit.h"

//======================================================
// マクロ定義
//======================================================
#define TX_BUFFER_DEPTH 16
#define RX_BUFFER_DEPTH 16

//======================================================
// 型定義
//======================================================

//======================================================
// 変数・定数
//======================================================

//======================================================
// プロトタイプ宣言
//======================================================

//======================================================
// 公開関数定義
//======================================================
/**
 * @brief I2C 受信FIFO格納レベル取得
 * @param ch 対象I2Cチャネル
 * @return RX FIFO格納データ数
 */
uint8_t I2C_read_RX_fifo_level(I2C_ch_t ch)
{
    return (uint8_t)((I2Cn_IC_RXFLR(ch) >> 0) & (MASK_5BIT));
}

/**
 * @brief I2C 受信FIFO満杯判定
 * @param ch 対象I2Cチャネル
 * @return true: 満杯, false: 空きあり
 */
bool I2C_check_RX_fifo_full(I2C_ch_t ch)
{
    return (RX_BUFFER_DEPTH <= I2C_read_RX_fifo_level(ch));
}

/**
 * @brief I2C 受信FIFO1バイト読み出し
 * @param ch 対象I2Cチャネル
 * @return 読み出しデータ
 */
uint8_t I2C_read_RX_FIFO_data(I2C_ch_t ch)
{
    return (uint8_t)((I2Cn_IC_DATA_CMD(ch) >> 0) & (MASK_8BIT));
}

/**
 * @brief I2C 送信FIFO格納レベル取得
 * @param ch 対象I2Cチャネル
 * @return TX FIFO格納データ数
 */
uint8_t I2C_read_TX_fifo_level(I2C_ch_t ch)
{
    return (uint8_t)((I2Cn_IC_TXFLR(ch) >> 0) & (MASK_5BIT));
}

/**
 * @brief I2C 送信FIFO満杯判定
 * @param ch 対象I2Cチャネル
 * @return true: 満杯, false: 空きあり
 */
bool I2C_check_TX_fifo_full(I2C_ch_t ch)
{
    return (TX_BUFFER_DEPTH <= I2C_read_TX_fifo_level(ch));
}

/**
 * @brief I2C マスター送信用送信FIFO1バイト書き込み
 * @param ch 対象I2Cチャネル
 * @param data 送信データ
 * @param master_cmd マスター送信コマンド種別
 * @param condition STOP/RESTART制御
 * @return なし
 */
void I2C_set_TX_FIFO_data_master(I2C_ch_t ch, uint8_t data, I2C_master_cmd_t master_cmd, I2C_condition_control_t condition)
{
    bool stop_enable = false;
    bool restart_enable = false;

    if (stop_condition == condition || restart_and_stop_condition == condition)
    {
        stop_enable = true;
    }
    if (restart_condition == condition || restart_and_stop_condition == condition)
    {
        restart_enable = true;
    }

    // 送信データ生成
    uint16_t write_data = 0x0000;
    write_data |= (data << 0);
    write_data |= (master_cmd << 8);
    write_data |= (stop_enable << 9);
    write_data |= (restart_enable << 10);

    // バッファ空き確認→送信
    while (I2C_check_TX_fifo_full(ch))
    {
        /* 送信待ち */
    }
    I2Cn_IC_DATA_CMD(ch) = write_data;
}

/**
 * @brief I2C スレーブ送信用送信FIFO1バイト書き込み
 * @param ch 対象I2Cチャネル
 * @param data 送信データ
 * @return なし
 */
void I2C_set_TX_FIFO_data_slave(I2C_ch_t ch, uint8_t data)
{
    // 送信データ生成
    uint16_t write_data = 0x0000;
    write_data |= (data << 0);
    write_data |= (0 << 8); // スレーブ送信

    // バッファ空き確認→送信
    while (I2C_check_TX_fifo_full(ch))
    {
        /* 送信待ち */
    }

    I2Cn_IC_DATA_CMD(ch) = write_data;
}

/**
 * @brief I2C TXアボート割り込み状態取得
 * @param ch 対象I2Cチャネル
 * @return true: TXアボート発生, false: 未発生
 */
bool I2C_read_TX_abrt(I2C_ch_t ch)
{
    return (bool)((I2Cn_IC_RAW_INTR_STAT(ch) >> 6) & MASK_1BIT);
}

/**
 * @brief I2C TXアボート割り込みクリア
 * @param ch 対象I2Cチャネル
 * @return なし
 */
void I2C_clear_TX_abrt(I2C_ch_t ch)
{
    I2Cn_IC_CLR_TX_ABRT(ch);
}

/**
 * @brief I2C 有効/無効設定
 * @param ch 対象I2Cチャネル
 * @param is_enable true: 有効, false: 無効
 * @return なし
 */
void i2c_set_enable(I2C_ch_t ch, bool is_enable)
{
    I2Cn_IC_ENABLE(ch) = (I2Cn_IC_ENABLE(ch) & ~(MASK_1BIT << 0)) | (is_enable << 0);
}

/**
 * @brief I2C マスター/スレーブモード設定
 * @param ch 対象I2Cチャネル
 * @param mode 設定する動作モード
 * @return なし
 */
void i2c_select_master_or_slave(I2C_ch_t ch, I2C_mode_t mode)
{
    // slave設定レジスタは1でslave無効となる
    if (master == mode)
    {
        I2Cn_IC_CON(ch) = (I2Cn_IC_CON(ch) & ~(MASK_1BIT << 6)) | (1 << 6); // slave設定レジスタ
        I2Cn_IC_CON(ch) = (I2Cn_IC_CON(ch) & ~(MASK_1BIT << 0)) | (1 << 0); // master設定レジスタ
    }
    else
    {
        I2Cn_IC_CON(ch) = (I2Cn_IC_CON(ch) & ~(MASK_1BIT << 6)) | (0 << 6); // slave設定レジスタ
        I2Cn_IC_CON(ch) = (I2Cn_IC_CON(ch) & ~(MASK_1BIT << 0)) | (0 << 0); // master設定レジスタ
    }
}

/**
 * @brief I2C 通信速度モード設定
 * @param ch 対象I2Cチャネル
 * @param speed 設定する通信速度モード
 * @return なし
 */
void i2c_set_speed(I2C_ch_t ch, I2C_speed_t speed)
{
    I2Cn_IC_CON(ch) = (I2Cn_IC_CON(ch) & ~(MASK_2BIT << 1)) | (speed << 1);
}

/**
 * @brief I2C アドレッシングモード設定
 * @param ch 対象I2Cチャネル
 * @param mode 設定対象の動作モード
 * @param addressing_mode 設定するアドレッシングモード
 * @return なし
 */
void i2c_set_addressing_mode(I2C_ch_t ch, I2C_mode_t mode, I2C_addressing_mode_t addressing_mode)
{
    if (master == mode)
    {
        I2Cn_IC_CON(ch) = (I2Cn_IC_CON(ch) & ~(MASK_1BIT << 4)) | (addressing_mode << 4);
    }
    else // slave
    {
        I2Cn_IC_CON(ch) = (I2Cn_IC_CON(ch) & ~(MASK_1BIT << 3)) | (addressing_mode << 3);
    }
}

/**
 * @brief I2C デフォルトアドレス設定
 * @param ch 対象I2Cチャネル
 * @param mode 設定対象の動作モード
 * @param default_address 設定するアドレス値
 * @return なし
 */
void i2c_set_default_address(I2C_ch_t ch, I2C_mode_t mode, uint16_t default_address)
{
    if (master == mode)
    {
        I2Cn_IC_TAR(ch) = (I2Cn_IC_TAR(ch) & ~(MASK_10BIT << 0)) | (default_address << 0);
    }
    else // slave
    {
        I2Cn_IC_SAR(ch) = (I2Cn_IC_SAR(ch) & ~(MASK_10BIT << 0)) | (default_address << 0);
    }
}

/**
 * @brief I2C マスター読み出し要求割り込み(RD_REQ)クリア
 * @details マスターからのデータ読み出し要求割り込みをクリアする
 * @param ch 対象I2Cチャネル
 * @return なし
 */
void i2c_clear_rd_req(I2C_ch_t ch)
{
    I2Cn_IC_CLR_RD_REQ(ch); // RD_REQ割り込みをクリア（読むだけでクリアされる）
}

/**
 * @brief I2C マスター読み出し要求割り込み(RD_REQ)状態取得
 * @details 割り込み発生時にHighとなるため、ポーリング監視でスレーブ送信開始判定に使用する
 * @param ch 対象I2Cチャネル
 * @return true: 要求あり, false: 要求なし
 */
bool i2c_check_rd_req(I2C_ch_t ch)
{
    return ((I2Cn_IC_RAW_INTR_STAT(ch) >> 5) & MASK_1BIT); // trueで要求あり
}

//======================================================
// 内部関数定義
//======================================================