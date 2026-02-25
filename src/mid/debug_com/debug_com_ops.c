/**
 * @file   debug_com_ops.c
 * @brief  デバッグ通信コンポーネント・通信制御実装
 * @details コマンド非依存の通信処理の未実装する。コマンド定義はアプリ層で持つ
 */

//======================================================
// インクルード
//======================================================
#include "debug_com.h"
#include "debug_com_internal.h"
#include "I2C.h"
#include "bit.h"
#include <string.h>

//======================================================
// マクロ定義
//======================================================

//======================================================
// 型定義
//======================================================
/**
 * @brief デバッグ受信ステート定義
 */
typedef enum
{
    read_cmd,      /**< cmd読取 */
    read_data_len, /**< データ長読取 */
    read_data,     /**< データ読取 */
} rx_state_t;

/**
 * @brief デバッグ受信処理パラメータ定義
 */
typedef struct
{
    rx_state_t state;              /**< 受信ステート */
    DEBUG_COM_debug_frame_t frame; /**< 受信フレーム */
    uint8_t recv_data_len;         /**< 受信済みデータ長 */

} rx_parameter_t;

//======================================================
// 変数・定数
//======================================================
static I2C_ch_t debug_I2C_ch;

//======================================================
// プロトタイプ宣言
//======================================================

//======================================================
// 公開関数定義
//======================================================
/**
 * @brief デバッグフレーム受信
 * @param dst_frame_ptr 受信フレーム格納先
 * @return 受信完了時true、未完了時false
 */
bool DEBUG_COM_read_frame(DEBUG_COM_debug_frame_t *dst_frame_ptr)
{
    // 受信パラメータ初期化
    static rx_parameter_t rx = {
        .state = read_cmd,
        .frame = {0},
        .recv_data_len = 0,
    };

    // メイン処理
    while (true)
    {
        // TODO：通信失敗時のリカバリ処理
        switch (rx.state)
        {
        case read_cmd:
        {
            if (read_succsess == I2C_pop_RX(&rx.frame.cmd, sizeof(rx.frame.cmd), debug_I2C_ch))
            {
                // 読み出し成功 → 次のステートへ
                rx.state = read_data_len;
                break;
            }
            else
            {
                // 読み出し失敗 → 次周期まで待機
                return false;
            }
        }
        case read_data_len: // TODO：サイズ違反の場合の処理（DEBUG_COM_MAX_DATA_LENとの比較処理）
        {
            if (read_succsess == I2C_pop_RX(&rx.frame.data_len, sizeof(rx.frame.data_len), debug_I2C_ch))
            {
                // 読み出し成功 → 次のステートへ
                rx.state = read_data;
                rx.recv_data_len = 0; // 読み出し済みデータ数の初期化
                break;
            }
            else
            {
                // 読み出し失敗 → 次周期まで待機
                return false;
            }
        }
        case read_data:
        {
            // データ数0の場合→即リターン
            if (rx.frame.data_len == 0)
            {
                *dst_frame_ptr = rx.frame; // 戻り値にフレーム格納
                rx.state = read_cmd;
                return true;
            }

            // データ読み出し
            if (read_succsess == I2C_read_RX_bytes(rx.frame.data, sizeof(rx.frame.data), rx.frame.data_len, debug_I2C_ch))
            {
                // 読み出し成功 → 初期ステートへ戻る
                *dst_frame_ptr = rx.frame; // 戻り値にフレーム格納
                rx.state = read_cmd;
                return true;
            }
            else
            {
                // 読み出し失敗 → 次周期まで待機
                return false;
            }
        }
        default:
            rx.state = read_cmd;
            return false;
        }
    }
}

/**
 * @brief デバッグフレーム送信
 * @details cmdとdata_lenを送信 → dataが存在する場合送信する
 * @param cmd コマンドID
 * @param data_len 送信データ長
 * @param data 送信データ先頭ポインタ
 * @return 送信ステータス
 * @note RP2040はスレーブとして送信する際、最後の送信後にACKを受け取ると送信未終了と判断し、SCLをLowに保持し続けてしまう
 *       →SCLがLowのままになるので、マスターがstop conditionを発行できず、通信が正常終了しない。ので、最後はNACKを受け取らないといけない
 *       また、data_lenの値によって何バイト目で通信完了となるかが変わる
 *       これらに対処するため、cmd・data_lenとdataで2回に分けて送信を行うようにしている。これによって、マスタ側が適切なタイミングでNACKを送信できる
 */
DEBUG_COM_status_t DEBUG_COM_send(uint8_t cmd, uint8_t data_len, const uint8_t *data)
{
    // エラー処理
    if (data_len > DEBUG_COM_MAX_DATA_LEN)
        return err_len;

    // cmd,data_lenの送信
    uint8_t frame1[2];
    frame1[0] = cmd;
    frame1[1] = data_len;
    I2C_response_rd_request(frame1, 2, debug_I2C_ch);

    // dataの送信（dataが存在する場合）
    if (data_len && data)
    {
        uint8_t frame2[DEBUG_COM_MAX_DATA_LEN];
        memcpy(&frame2, data, data_len);
        I2C_response_rd_request(frame2, data_len, debug_I2C_ch);
    }

    // 正常終了
    return ok;
}

/**
 * @brief デバッグ通信I2Cチャネル設定
 * @details 初期化関数からのみコールする
 * @param ch 割り当てI2Cチャネル
 * @return なし
 */
void DEBUG_COM_set_i2c_ch(I2C_ch_t ch)
{
    debug_I2C_ch = ch;
}

//======================================================
// 内部関数定義
//======================================================