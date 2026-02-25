/**
 * @file   main.c
 * @brief  システム初期化 → アプリケーション起動
 */

//======================================================
// インクルード
//======================================================
#include "gpio.h"
#include "adc.h"
#include "interrupt.h"
#include "timer.h"
#include "I2C.h"
#include "button.h"
#include "debug_com.h"
#include "analogStick.h"
#include "SH1107.h"
#include "tetris.h"
#include "config.h"
#include "typedef.h"
#include "bitmap_lib.h"

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

//======================================================
// 公開関数定義
//======================================================
void main(void)
{
    /* ドライバ層初期化 */
    GPIO_initialize(gpioPin_func_list, gpioPin_dir_list); // GPIO初期化
    ADC_initialize(adc_ch_config, adc_parameter_config);  // ADC初期化
    I2C_initialize(config_I2C0_display);                  // I2C初期化（ch0）
    TIMER_initialize();                                   // タイマー初期化
    INTERRUPT_initialize();                               // 割り込み初期化

    /* ミドル層初期化 */
    BUTTON_class_t B_button = BUTTON_initialize_instance(config_B_button);                    // ボタン初期化
    BUTTON_class_t A_button = BUTTON_initialize_instance(config_A_button);                    // ボタン初期化
    BUTTON_class_t Y_button = BUTTON_initialize_instance(config_Y_button);                    // ボタン初期化
    BUTTON_class_t X_button = BUTTON_initialize_instance(config_X_button);                    // ボタン初期化
    ANALOGSTICK_class_t analog_stick = ANALOGSTICK_initialize_instance(config_analogStick_1); // アナログスティック初期化
    SH1107_initialize(config_SH1107_1);                                                       // SH1107初期化（I2Cch割り当て、IC起動シーケンス実行）

    // アプリケーション向け入力初期化
    TETRIS_input_parameter_t tetris_input_parameter = {
        .turnR_button = B_button,
        .turnL_button = A_button,
        .control_button1 = Y_button,
        .control_button2 = X_button,
        .analog_stick = analog_stick,
    };

    // アプリケーション起動
    TETRIS_main(&tetris_input_parameter);
}