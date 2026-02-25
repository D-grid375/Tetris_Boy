/**
 * @file   SH1107_internal.h
 * @brief  SH1107コンポーネント・内部公開定義
 */

#ifndef __SH1107_INTERNAL_H__
#define __SH1107_INTERNAL_H__

//======================================================
// インクルード
//======================================================

//======================================================
// マクロ定義
//======================================================
// clang-format off
// コマンド送信における変数部分設定用のマクロ(マクロで統一したいため関数はコピーして利用)
// 各nバイトは送信コマンドの各nビットに対応しており、0,1=変数ビットの設定、F=無効値（非整数部分）
// sh1107_send_commandの引数variable_dataとして渡す
#define CMDx_NO_VARIABLE                0xFFFFFFFF // 変数設定無し
#define CMD1_COLUMNn_LOWER_ADDRESS(n)   sh1107_get_column_address_lower_variable(n)
#define CMD2_COLUMNn_HIGHER_ADDRESS(n)  sh1107_get_column_address_higher_variable(n)
#define CMD7_ENTIRE_DISPLAY_OFF         0xFFFFFFF0
#define CMD7_ENTIRE_DISPLAY_ON          0xFFFFFFF1
#define CMD11_DISPLAY_OFF               0xFFFFFFF0
#define CMD11_DISPLAY_ON                0xFFFFFFF1
#define CMD12_PAGEn_ADDRESS(n)          sh1107_get_page_address_variable(n)
// clang-format on

//======================================================
// 型定義
//======================================================
/**
 * @brief SH1107コントロールバイトCo設定定義
 * @details コントロールバイト送信時、次以降の送信バイト内容を指定する
 * @note 参照：SH1107データシートp16 Figure9 I2CProtocol
 */
typedef enum
{
    last_control = 0,   /**< 次以降の送信は全てdata byte */
    continuous_control, /**< 次の送信はdata byte、その次の送信はcontrol byte */
} sh1107_control_byte_option_t;

/**
 * @brief SH1107コントロールバイトD/C設定定義
 * @details コントロールバイト送信時、そのバイトの制御内容を指定する
 * @note 参照：SH1107データシートp16 Figure9 I2CProtocol
 */
typedef enum
{
    command_operation = 0, /**< このコントロールバイトはコマンド制御を意味する */
    RAM_operation,         /**< このコントロールバイトはRAM操作を意味する */
} sh1107_data_byte_option_t;

/**
 * @brief SH1107コマンドテーブルID定義
 * @details コントロールバイトでコマンド制御を実行する際のコマンドID定義
 * @note 参照：SH1107データシートp41 Command Table
 */
typedef enum
{
    command_1,   /**< 1. Set Column Address 4 lower bits */
    command_2,   /**< 2. Set Column Address 4 higher bits */
    command_3,   /**< 3. Set memory addressing mode */
    command_4A,  /**< 4. The Contrast Control Mode Set */
    command_4B,  /**< 4. Contrast Data Register Set */
    command_5,   /**< 5. Set Segment Re-map (ADC) */
    command_6A,  /**< 6. Set Multiplex Ration */
    command_6B,  /**< 6. Set Multiplex Ration */
    command_7,   /**< 7. Set Entire Display OFF/ON */
    command_8,   /**< 8. Set Normal/Reverse Display */
    command_9A,  /**< 9. Set display offset */
    command_9B,  /**< 9. Set display offset */
    command_10A, /**< 10. DC-DC Control Mode Set */
    command_10B, /**< 10. DC-DC Setting Mode Set */
    command_11,  /**< 11. Display OFF/ON */
    command_12,  /**< 12. Set Page Address */
    command_13,  /**< 13. Set Common Output Scan Direction */
    command_14A, /**< 14. Set Display Divide Ratio/Oscillator Frequency Mode Set */
    command_14B, /**< 14. Divide Ratio/Oscillator Frequency Data Set */
    command_15A, /**< 15. Dis-charge / Pre-charge Period Mode Set */
    command_15B, /**< 15. Dis-charge / Pre-charge Period Data Set */
    command_16A, /**< 16. VCOM Deselect Level Mode Set */
    command_16B, /**< 16. VCOM Deselect Level Data Set */
    command_17A, /**< 17. Set Display Start Line */
    command_17B, /**< 17. Set Display Start Line */
    command_18,  /**< 18. Read-Modify-Write */
    command_19,  /**< 19. End */
    command_20,  /**< 20. NOP */
    command_21,  /**< 21. Write Display Data */
    command_22,  /**< 22. Read ID */
    command_23,  /**< 23. Read Display Data */
} sh1107_command_table_t;

/**
 * @brief SH1107制御用内部ステート定義
 */
typedef struct
{
    I2C_ch_t assign_I2C_ch;                /**< 割り当てI2Cチャネル */
    I2C_condition_control_t I2C_condition; /**< I2Cコンディション制御値 */
} sh1107_internal_state_t;

//======================================================
// グローバル変数・定数extern宣言
//======================================================
extern sh1107_internal_state_t sh1107_internal_state;

//======================================================
// グローバル関数extern宣言
//======================================================
/* init → ops */
extern void SH1107_assign_i2c_ch(I2C_ch_t ch);
extern void sh1107_execute_startup_sequence();

/* ctrl → ops  */
extern void sh1107_send_control_byte(sh1107_control_byte_option_t Co, sh1107_data_byte_option_t DC);
extern void sh1107_send_command(sh1107_command_table_t command, uint32_t variable_data);
extern void sh1107_send_RAM_operation(uint8_t RAM_data);
extern void SH1107_select_i2c_condition(I2C_condition_control_t condition);
extern uint32_t sh1107_get_column_address_lower_variable(uint8_t column_address);
extern uint32_t sh1107_get_column_address_higher_variable(uint8_t column_address);
extern uint32_t sh1107_get_page_address_variable(uint8_t page_num);

#endif /* __SH1107_INTERNAL_H__ */