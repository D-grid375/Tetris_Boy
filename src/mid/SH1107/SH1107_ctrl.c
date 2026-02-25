/**
 * @file   SH1107_ctrl.c
 * @brief  SH1107コンポーネント・汎用制御実装
 */

//======================================================
// インクルード
//======================================================
#include "SH1107.h"
#include "SH1107_internal.h"
#include "bit.h"
#include "timer.h"
#include "bitmap_lib.h"

//======================================================
// マクロ定義
//======================================================
// clang-format off
#define COLUMN_LENGTH 128 // ディスプレイの列数 
#define PAGE_LENGTH   16  // ディスプレイのページ数（1ページ8行 = 行数は128）
// clang-format on

//======================================================
// 型定義
//======================================================

//======================================================
// 変数・定数
//======================================================

//======================================================
// プロトタイプ宣言
//======================================================
static void initialize_entire_display();

//======================================================
// 公開関数定義
//======================================================
/**
 * @brief SH1107起動シーケンス実行
 * @return なし
 * @details 画面初期化からDisplay ONまでを実行する。最低約150msを要する
 * @todo リスタートおよびストップコンディション設定方針の整理
 */
void sh1107_execute_startup_sequence()
{
    // TODO：リスタート・ストップコンディション設定の考え方を整理 現状、送信関数の頭ではリスタート、最後ではストップで統一　※論理的な通信単位で通信をスタート・ストップして区切るのが普通
    // 明示的にRestart（一応、指定しなくてもMCUの仕様で自動設定される）
    SH1107_select_i2c_condition(restart_condition);

    // 周波数設定
    sh1107_send_control_byte(continuous_control, command_operation);
    sh1107_send_command(command_14A, CMDx_NO_VARIABLE); // 周波数設定モード
    sh1107_send_control_byte(continuous_control, command_operation);
    sh1107_send_command(command_14B, 0x01010000); // 周波数設定（現状初期値）

    // ディスプレイ初期化：★50ms必要(400Kbpsでの理論値は40.96ms)
    initialize_entire_display(); // 関数内でwaitがかかるので明示的waitは不要

    // ディスプレイ起動：★100ms必要
    sh1107_send_control_byte(continuous_control, command_operation);
    sh1107_send_command(command_11, CMD11_DISPLAY_ON);

    // stop conditionを送信
    SH1107_select_i2c_condition(stop_condition);
    sh1107_send_control_byte(continuous_control, command_operation); // ダミー送信(stop conditionを認識させるため)

    // データシート指定の最小wait時間（Tetrisアプリではこの初期化後の次回通信は100ms以内に開始されるため、明示的に待つ必要あり）
    TIMER_wait_ms(100);
}

/**
 * @brief 128x128ビットマップ汎用描画
 * @param bitmap 描画対象ビットマップ
 * @return 描画成功時true、失敗時false
 * @details 送信したビットマップを毎回内部で保持し、次回送信時はそのビットマップと今回描画したいビットマップとの差分のみを送信する
 *          送信失敗した場合、次回は描画したいビットマップ全体を送信する。でないとディスプレイの描画内容が不整合になるため
 *          これにより正しい描画を行いつつ通信時間を低減している。尚、差分送信と全体送信は別関数で実装している
 */
bool SH1107_display_bitmap_data(bitmap_128_t bitmap)
{
    static bitmap_128_t previous_bitmap = {0}; // 前回送信したビットマップ
    bool is_success;                           // 送信バッファ書き込み中にエラーが出た場合false

    if (I2C_read_TX_abrt(0)) // 前回送信が正常終了したかをチェック
    {
        // 前回送信失敗時：画面全体を描画し直す。previous_bitmapは参照しない
        I2C_clear_TX_abrt(0);
        is_success = SH1107_display_bitmap_all_data(bitmap);
        BITMAP_copy(previous_bitmap, bitmap);
    }
    else
    {
        // 前回送信成功時：差分のみ描画。初回送信時にはprevious_bitmapのオール0に対する差分描画になる（遅延あるが誤差なので許容）
        is_success = SH1107_display_bitmap_updated_data(bitmap, previous_bitmap);
        BITMAP_copy(previous_bitmap, bitmap);
    }

    return is_success;
}

/**
 * @brief 128x128ビットマップ全体描画
 * @param bitmap 描画対象ビットマップ
 * @return 描画成功時true、失敗時false
 * @details ビットマップの内容によらず、受け取ったビットマップ全体をディスプレイに送信する
 *          差分送信は一切行わない
 */
bool SH1107_display_bitmap_all_data(bitmap_128_t bitmap)
{
    // リスタート
    SH1107_select_i2c_condition(restart_condition);

    // 列指定：この関数がコールされる前に通信が止まり、IC側の列カウンタが変な所を指していた場合の対策
    sh1107_send_control_byte(continuous_control, command_operation);
    sh1107_send_command(command_1, CMD1_COLUMNn_LOWER_ADDRESS(0));
    sh1107_send_control_byte(continuous_control, command_operation);
    sh1107_send_command(command_2, CMD2_COLUMNn_HIGHER_ADDRESS(0));

    for (uint8_t page = 0; page < 16; page++)
    {
        // リスタート
        SH1107_select_i2c_condition(restart_condition);

        // ページ指定
        sh1107_send_control_byte(continuous_control, command_operation);
        sh1107_send_command(command_12, CMD12_PAGEn_ADDRESS(page));

        // 指定したページの列全体をRAM操作
        sh1107_send_control_byte(last_control, RAM_operation);
        for (uint8_t column = 0; column < COLUMN_LENGTH; column++)
        {
            uint8_t send_data = 0x00;

            for (uint8_t bit = 0; bit < 8; bit++)
            {
                send_data |= ((BITMAP_read(bitmap, page * 8 + bit, column)) << bit); // 描画ビット列生成
            }

            while (I2C_read_TX_fifo_level(sh1107_internal_state.assign_I2C_ch) > 2) // バッファが詰まっている状態で更にバッファに突っ込むと破綻するので待つ 2という数値は適当
            {
                // 送信待ち
            }

            // RAMデータ送信
            sh1107_send_RAM_operation(send_data);
        }

        // 送信バッファ書き込みに失敗したら即リターン
        if (I2C_read_TX_abrt(sh1107_internal_state.assign_I2C_ch))
            return false;
    }

    // 全データ送信後はstop conditionを送信
    SH1107_select_i2c_condition(restart_and_stop_condition);
    sh1107_send_control_byte(continuous_control, command_operation); // ダミー送信(stop conditionを認識させるため)

    // 最後にも念のため送信アボートを見ておく
    if (I2C_read_TX_abrt(sh1107_internal_state.assign_I2C_ch))
        return false;

    // 正常終了
    return true;
}

/**
 * @brief 128x128ビットマップ差分描画
 * @param current_bitmap 現在ビットマップ
 * @param previous_bitmap 前回ビットマップ
 * @return 描画成功時true、失敗時false
 * @details previous_bitmapからcurrent_bitmapへの差分のみ送信して通信時間を低減する
 */
bool SH1107_display_bitmap_updated_data(bitmap_128_t current_bitmap, bitmap_128_t previous_bitmap)
{
    // リスタート
    SH1107_select_i2c_condition(restart_condition);

    // ディスプレイIC側で選択されている列アドレス（この関数がコールされる前に列アドレスが0以外に設定されてるとバグるので注意）
    // ただし、送信失敗時にはSH1107_display_bitmap_dataでディスプレイ全体を再描画するのでそれは起こらない（はず）
    uint8_t column_IC = 0;

    for (uint8_t page = 0; page < 16; page++)
    {
        // ページ指定
        sh1107_send_control_byte(continuous_control, command_operation);
        sh1107_send_command(command_12, CMD12_PAGEn_ADDRESS(page));

        // 列毎の操作：前回送信と今回送信のビットマップの各列を比較し、差分がある場合のみ送信を行う
        for (uint8_t column = 0; column < COLUMN_LENGTH; column++)
        {
            /* バイト間の差分チェック */
            uint8_t current_byte = 0x00;
            uint8_t previous_byte = 0x00;
            for (uint8_t bit = 0; bit < 8; bit++)
            {
                current_byte |= ((BITMAP_read(current_bitmap, page * 8 + bit, column)) << bit);
                previous_byte |= ((BITMAP_read(previous_bitmap, page * 8 + bit, column)) << bit);
            }

            // 差分ありの場合→バイトRAMデータ送信
            if (!(current_byte == previous_byte))
            {
                while (I2C_read_TX_fifo_level(sh1107_internal_state.assign_I2C_ch) > 2) // バッファが詰まっている状態で更にバッファに突っ込むと破綻するので待つ 2という数値は適当
                {
                    // 送信待ち
                }

                // 列アドレス設定　※連続する列にデータ送信する場合は不要なので処理されない
                if ((column != column_IC))
                {
                    sh1107_send_control_byte(continuous_control, command_operation);
                    sh1107_send_command(command_1, CMD1_COLUMNn_LOWER_ADDRESS(column));
                    sh1107_send_control_byte(continuous_control, command_operation);
                    sh1107_send_command(command_2, CMD2_COLUMNn_HIGHER_ADDRESS(column));
                }

                // RAMデータ送信
                sh1107_send_control_byte(continuous_control, RAM_operation);
                sh1107_send_RAM_operation(current_byte);

                // 送信後、IC側の指定Columnアドレスは自動で+1される
                column_IC = (column + 1) % COLUMN_LENGTH;
            }

            // 送信バッファ書き込みに失敗したら即リターン
            if (I2C_read_TX_abrt(sh1107_internal_state.assign_I2C_ch))
                return false;
        }
    }
    // 次送信のために列アドレス設定をクリアしておく
    sh1107_send_control_byte(continuous_control, command_operation);
    sh1107_send_command(command_1, CMD1_COLUMNn_LOWER_ADDRESS(0));
    sh1107_send_control_byte(continuous_control, command_operation);
    sh1107_send_command(command_2, CMD2_COLUMNn_HIGHER_ADDRESS(0));

    // 全データ送信後はstop conditionを送信
    SH1107_select_i2c_condition(stop_condition);
    sh1107_send_control_byte(continuous_control, command_operation); // ダミー送信(stop conditionを認識させるため)

    // 最後にも念のため送信アボートを見ておく
    if (I2C_read_TX_abrt(sh1107_internal_state.assign_I2C_ch))
        return false;

    // 正常終了
    return true;
}

//======================================================
// 内部関数定義
//======================================================
/**
 * @brief ディスプレイ全消灯初期化
 * @return なし
 * @details 初期化シーケンス用に全体描画で消灯する。
 *          初期化前RAM状態が不定のため差分描画は使用しない
 */
static void initialize_entire_display()
{
    bitmap_128_t initial_bitmap = {0};
    SH1107_display_bitmap_all_data(initial_bitmap);
}