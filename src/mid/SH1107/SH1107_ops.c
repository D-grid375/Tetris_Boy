/**
 * @file   SH1107_ops.c
 * @brief  SH1107コンポーネント・通信制御実装
 */

//======================================================
// インクルード
//======================================================
#include "SH1107.h"
#include "SH1107_internal.h"
#include "bit.h"
#include "I2C.h"
#include "timer.h"

//======================================================
// マクロ定義
//======================================================
#define COMMAND_BIT_LENGTH 8

//======================================================
// 型定義
//======================================================

//======================================================
// 変数・定数
//======================================================
// SH11107コマンドビット列定義 データシートのビット定義から以下ルールで各ビットを16進数化
// 0 → 0  /  1 → 1  /  変数 → F（関数コール時に設定）
// 参照：SH1107データシートp41 Command Table
const static uint32_t command_base_bit[] = {
    0x0000FFFF, // 1. Set Column Address 4 lower bits
    0x00010FFF, // 2. Set Column Address 4 higher bits
    0x0010000F, // 3. Set memory addressing mode
    0x10000001, // 4. The Contrast Control Mode Set
    0xFFFFFFFF, // 4. Contrast Data Register Set
    0x1010000F, // 5. Set Segment Re-map (ADC)
    0x10101000, // 6. Set Multiplex Ration
    0x0FFFFFFF, // 6. Set Multiplex Ration
    0x1010010F, // 7. Set Entire Display OFF/ON
    0x1010011F, // 8. Set Normal/Reverse Display
    0x11010011, // 9. Set display offset
    0x0FFFFFFF, // 9. Set display offset
    0x10101101, // 10. DC-DC Control Mode Set
    0x1000FFFF, // 10. DC-DC Setting Mode Set
    0x1010111F, // 11. Display OFF/ON
    0x1011FFFF, // 12. Set Page Address
    0x1100FFFF, // 13. Set Common Output Scan Direction
    0x11010101, // 14. Set Display Divide Ratio/Oscillator Frequency Mode Set
    0xFFFFFFFF, // 14. Divide Ratio/Oscillator Frequency Data Set
    0x11011001, // 15. Dis-charge / Pre-charge Period Mode Set
    0xFFFFFFFF, // 15. Dis-charge / Pre-charge Period Data Set
    0x11011011, // 16. VCOM Deselect Level Mode Set
    0xFFFFFFFF, // 16. VCOM Deselect Level Data Set
    0x11011100, // 17. Set Display Start Line
    0x0FFFFFFF, // 17. Set Display Start Line
    0x11100000, // 18. Read-Modify-Write
    0x11101110, // 19. End
    0x11100011, // 20. NOP
    0xFFFFFFFF, // 21. Write Display Data
    0xFFFFFFFF, // 22. Read ID
    0xFFFFFFFF, // 23. Read Display Data
};

sh1107_internal_state_t sh1107_internal_state = {
    // 初期値を入れておく　chはSH1107_initの方で設定
    .I2C_condition = no_condition,
};

//======================================================
// プロトタイプ宣言
//======================================================
static void reset_I2C_condition();
static void execute_auto_control();

//======================================================
// 公開関数定義
//======================================================
/**
 * @brief SH1107コントロールバイト送信
 * @param Co Coビット設定
 * @param DC D/Cビット設定
 * @return なし
 */
void sh1107_send_control_byte(sh1107_control_byte_option_t Co, sh1107_data_byte_option_t DC)
{
    uint8_t control_byte = 0;
    control_byte |= (Co << 7) | (DC << 6);

    I2C_set_TX_FIFO_data_master(sh1107_internal_state.assign_I2C_ch, control_byte, master_write, sh1107_internal_state.I2C_condition); // master_writeは固定
    reset_I2C_condition();
}

/**
 * @brief SH1107コマンド送信
 * @param command コマンドテーブルID
 * @param variable_data 可変ビット用データ
 * @return なし
 * @details ベースコマンドと可変データを統合して最終コマンドを生成しI2Cマスターで送信する
 * @note base_commandのnバイト目がFの時、variable_dataのnバイト目を埋め込んで最終的なコマンドを成型する
 *       コマンドの各バイトは0or1になり、この各nバイトをnビットとして解釈して送信する
 */
void sh1107_send_command(sh1107_command_table_t command, uint32_t variable_data)
{
    uint32_t base_command = command_base_bit[command];
    uint8_t finalized_command = 0;

    // 最終コマンド生成処理
    for (uint8_t bit_index = 0; bit_index < COMMAND_BIT_LENGTH; bit_index++)
    {
        uint8_t hex_base_command = (base_command >> bit_index * 4) & MASK_4BIT;   // base_commandのN番目のhex抽出
        uint8_t hex_variable_data = (variable_data >> bit_index * 4) & MASK_4BIT; // variable_dataのN番目のhex抽出

        // base_commandの定数部分とvariable_dataの変数部分を重ね合わせる
        if ((0xF == hex_base_command) && (0xF != hex_variable_data)) // ビットが変数の場合
        {
            finalized_command |= ((hex_variable_data & MASK_1BIT) << bit_index);
        }
        else if ((0xF != hex_base_command) && (0xF == hex_variable_data)) // ビットが定数の場合
        {
            finalized_command |= ((hex_base_command & MASK_1BIT) << bit_index);
        }
        else // 設定違反
        {
            // error
        }
    }

    I2C_set_TX_FIFO_data_master(sh1107_internal_state.assign_I2C_ch, finalized_command, master_write, sh1107_internal_state.I2C_condition); // master_writeは固定
    reset_I2C_condition();
}

/**
 * @brief SH1107描画RAMデータ送信
 * @param RAM_data 送信RAMデータ
 * @return なし
 * @note IC側で設定されているページ番号と列アドレスに対し、その位置の8行1列分のデータを更新する
 */
void sh1107_send_RAM_operation(uint8_t RAM_data)
{
    I2C_set_TX_FIFO_data_master(sh1107_internal_state.assign_I2C_ch, RAM_data, master_write, sh1107_internal_state.I2C_condition); // master_writeは固定
    reset_I2C_condition();
}

/**
 * @brief SH1107割り当てI2Cチャネル設定
 * @param ch 割り当てI2Cチャネル
 * @return なし
 * @details 初期化時のみ使用を想定する
 */
void SH1107_assign_i2c_ch(I2C_ch_t ch)
{
    sh1107_internal_state.assign_I2C_ch = ch;
}

/**
 * @brief SH1107向けI2Cコンディション設定
 * @param condition I2Cコンディション制御値
 * @return なし
 * @details condition設定頻度が低い想定のため内部変数で管理する
 * @note restart_conditionまたはstop_conditionで送信したい場合は送信前に本関数を呼ぶ
 */
void SH1107_select_i2c_condition(I2C_condition_control_t condition)
{
    sh1107_internal_state.I2C_condition = condition;
}

/**
 * @brief Column Address下位変数生成
 * @param column_address 列アドレス
 * @return 下位アドレス変数データ
 * @note command_1の変数生成用関数。ヘッダに定義してあるマクロ経由で使用する
 */
uint32_t sh1107_get_column_address_lower_variable(uint8_t column_address)
{
    uint32_t column_address_var_lower = 0xFFFF0000;

    for (uint8_t bit = 0; bit < 4; bit++)
    {
        column_address_var_lower |= ((column_address >> bit) & (MASK_1BIT)) << (bit * 4);
    }

    return column_address_var_lower;
}

/**
 * @brief Column Address上位変数生成
 * @param column_address 列アドレス
 * @return 上位アドレス変数データ
 * @note command_2の変数生成用関数。ヘッダに定義してあるマクロ経由で使用する
 */
uint32_t sh1107_get_column_address_higher_variable(uint8_t column_address)
{
    uint32_t column_address_var_higher = 0xFFFFF000;

    for (uint8_t bit = 4; bit < 7; bit++)
    {
        column_address_var_higher |= ((column_address >> bit) & (MASK_1BIT)) << ((bit - 4) * 4);
    }

    return column_address_var_higher;
}

/**
 * @brief Page Address変数生成
 * @param page_num ページ番号
 * @return ページアドレス変数データ
 * @note command_12の変数生成用関数。ヘッダに定義してあるマクロ経由で使用する
 */
uint32_t sh1107_get_page_address_variable(uint8_t page_num)
{
    uint32_t address = 0xFFFF0000;

    for (int8_t bit_index = 0; bit_index < 4; bit_index++)
    {
        address |= (((page_num >> bit_index) & MASK_1BIT) << bit_index * 4);
    }

    return address;
}

//======================================================
// 内部関数定義
//======================================================
/**
 * @brief SH1107 I2Cコンディションリセット
 * @return なし
 */
static void reset_I2C_condition()
{
    if (sh1107_internal_state.I2C_condition != no_condition)
    {
        sh1107_internal_state.I2C_condition = no_condition;
    }
}