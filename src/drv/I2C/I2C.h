/**
 * @file   I2C.h
 * @brief  I2Cコンポーネント・外部公開定義
 */

#ifndef __I2C_H__
#define __I2C_H__

//======================================================
// インクルード
//======================================================
#include "typedef.h"
#include "gpio.h"

//======================================================
// マクロ定義
//======================================================

//======================================================
// 型定義
//======================================================
/**
 * @brief I2Cチャネル
 */
typedef enum
{
    I2C0,
    I2C1,
} I2C_ch_t;

/**
 * @brief I2C動作モード
 */
typedef enum
{
    master,
    slave,
} I2C_mode_t;

/**
 * @brief I2C通信速度モード
 */
typedef enum
{
    standard_mode = 1,
    fast_mode,
    // high_speed_mode, 本ICでは設定不可（MAX_SPEED_MODE参照）
} I2C_speed_t;

/**
 * @brief I2Cアドレッシングモード
 */
typedef enum
{
    addressing_7bit,
    addressing_10bit,
} I2C_addressing_mode_t;

/**
 * @brief マスター送信コマンド種別
 */
typedef enum
{
    master_write,
    master_read,
} I2C_master_cmd_t;

/**
 * @brief マスター送信時のコンディション制御種別
 */
typedef enum
{
    no_condition,              /**< コンディション無し */
    stop_condition,            /**< ストップコンディション */
    restart_condition,         /**< リスタートコンディション */
    restart_and_stop_condition /**< リスタートおよびストップコンディション */
} I2C_condition_control_t;

/**
 * @brief I2C初期化設定定義（config用）
 * @details 使用するI2Cチャネル及びそのチャネルに対するピン割当や動作設定を指定する
 *          configでこの定義に対する実パラメータを設定し、初期化関数に渡す事で設定を反映させる
 */
typedef struct
{
    I2C_ch_t ch;
    GPIO_num_t gpioPin_SDA;                /**< SDAに割り当てるGPIOピン */
    GPIO_num_t gpioPin_SCL;                /**< SCLに割り当てるGPIOピン */
    I2C_mode_t mode;                       /**< I2C動作モード：master or slave */
    I2C_speed_t speed;                     /**< I2C通信速度モード */
    I2C_addressing_mode_t addressing_mode; /**< I2Cアドレッシングモード */
    uint16_t default_address;              /**< masterの場合はターゲットアドレス、slaveの場合は自身のアドレス */
} I2C_config_t;

/**
 * @brief I2C読み出し結果ステータス
 */
typedef enum
{
    input_error,   /**< 入力エラー（要求バイト数が読出先バッファ長を超過等） */
    read_fail,     /**< 読み出し失敗 */
    read_succsess, /**< 読み出し成功 */
} I2C_read_status_t;

//======================================================
// グローバル変数・定数extern宣言
//======================================================

//======================================================
// グローバル関数extern宣言
//======================================================
/* init */
extern void I2C_initialize(I2C_config_t config);

/* ctrl */
extern I2C_read_status_t I2C_read_RX_bytes(uint8_t *dst_bytes, uint8_t dst_bytes_len, uint8_t read_byte, I2C_ch_t ch);
extern I2C_read_status_t I2C_pop_RX(uint8_t *dst_bytes, uint8_t dst_bytes_len, I2C_ch_t ch);
extern void I2C_send_TX_bytes_as_slave(uint8_t *send_bytes, uint8_t send_bytes_len, I2C_ch_t ch);
extern void I2C_response_rd_request(uint8_t *send_bytes, uint8_t send_bytes_len, I2C_ch_t ch);

/* ops */
extern uint8_t I2C_read_RX_fifo_level(I2C_ch_t ch);
extern bool I2C_check_RX_fifo_full(I2C_ch_t ch);
extern uint8_t I2C_read_RX_FIFO_data(I2C_ch_t ch);
extern bool I2C_check_TX_fifo_full(I2C_ch_t ch);
extern uint8_t I2C_read_TX_fifo_level(I2C_ch_t ch);
extern void I2C_set_TX_FIFO_data_master(I2C_ch_t ch, uint8_t data, I2C_master_cmd_t master_cmd, I2C_condition_control_t condition);
extern void I2C_set_TX_FIFO_data_slave(I2C_ch_t ch, uint8_t data);
extern bool I2C_read_TX_abrt(I2C_ch_t ch);
extern void I2C_clear_TX_abrt(I2C_ch_t ch);

#endif /* __I2C_H__ */