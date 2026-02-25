/**
 * @file   I2C_ctrl.c
 * @brief  I2Cコンポーネント・汎用制御実装
 */

//======================================================
// インクルード
//======================================================
#include "I2C.h"
#include "I2C_internal.h"
#include "bit.h"
#include "typedef.h"

//======================================================
// マクロ定義
//======================================================
#define READ_RX_BYTES_MAX 8
#define READ_TX_BYTES_MAX 8

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
 * @brief I2C 受信データnByte読み出し
 * @details RX FIFOから指定バイト数を読み出す。バッファが要求バイト数に満たない場合はfailを返す
 * @param dst_bytes 読み出し先バッファ 先頭アドレス
 * @param dst_bytes_len 読み出し先バッファ長[byte]
 * @param read_byte 読み出し要求バイト数[byte]
 * @param ch 対象I2Cチャネル
 * @return 読み出し結果ステータス
 */
I2C_read_status_t I2C_read_RX_bytes(uint8_t *dst_bytes, uint8_t dst_bytes_len, uint8_t read_byte, I2C_ch_t ch)
{
    static int call_counter = 0;

    // エラーチェック
    if (dst_bytes_len < read_byte)
        return input_error;

    // 受信バイト数確認
    if (I2C_read_RX_fifo_level(ch) < read_byte)
        return read_fail;

    // データ取得
    for (uint8_t i = 0; i < read_byte; i++)
    {
        dst_bytes[i] = I2C_read_RX_FIFO_data(ch);
    }
    return read_succsess;
}

/**
 * @brief I2C 受信データ1Byte読み出し
 * @param dst_bytes 読み出し先バッファ 先頭アドレス
 * @param dst_bytes_len 読み出し先バッファ長[byte]
 * @param ch 対象I2Cチャネル
 * @return 読み出し結果ステータス
 */
I2C_read_status_t I2C_pop_RX(uint8_t *dst_bytes, uint8_t dst_bytes_len, I2C_ch_t ch)
{
    return I2C_read_RX_bytes(dst_bytes, dst_bytes_len, 1, ch);
}

/**
 * @brief I2C 送信要求応答
 * @details マスターからのRD_REQ割り込みを待ち、割り込み後に指定データを送信する
 * @param send_bytes 送信データ配列 先頭アドレス
 * @param send_bytes_len 送信データ長[byte]
 * @param ch 対象I2Cチャネル
 * @return なし
 * @note データシート記載と実機動作に差異があるため、実装は実機観測結果を優先している
 *       データシート参照：4.3.10.1.2. Slave-Transmitter Operation for a Single Byte
 *       翻訳：https://chatgpt.com/c/68a73615-2b60-8326-88ff-202319a58908
 */
void I2C_response_rd_request(uint8_t *send_bytes, uint8_t send_bytes_len, I2C_ch_t ch)
{
    while (!i2c_check_rd_req(ch)) // RD_REQ割り込み(送信要求受信)待ち
    {
        /* wait */
    }
    i2c_clear_rd_req(ch);                                       // RD_REQ割り込みをクリア（データシートでは自動クリアみたいに書いてるが、動作見た感じ恐らく嘘）
    I2C_send_TX_bytes_as_slave(send_bytes, send_bytes_len, ch); // データ送信
}

/**
 * @brief I2C マスター向け送信値書き込み
 * @details マスターへの送信用にTX FIFOへ複数バイトを書き込む
 * @param send_bytes 送信データ配列 先頭アドレス
 * @param send_bytes_len 送信データ長[byte]
 * @param ch 対象I2Cチャネル
 * @return なし
 */
void I2C_send_TX_bytes_as_slave(uint8_t *send_bytes, uint8_t send_bytes_len, I2C_ch_t ch)
{
    for (int byte = 0; byte < send_bytes_len; byte++)
    {
        I2C_set_TX_FIFO_data_slave(ch, send_bytes[byte]);
    }
}

//======================================================
// 内部関数定義
//======================================================
