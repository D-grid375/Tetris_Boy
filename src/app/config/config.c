/**
 * @file   config.c
 * @brief  CONFIGパラメータ定義
 * @details Driver層・Middle層の各コンポーネントにおいてconfigとして定義しているパラメータの設定を行う
 *          アプリ層においてこのパラメータを各コンポーネントの初期化関数に渡す事で設定値を反映させる
 *          複数の設定値を切り替えたい場合、複数のconfig設定を作成の上で初期化関数に渡すconfig値を切り替えること
 */

//======================================================
// インクルード
//======================================================
#include "config.h"
#include "debug_com.h"

//======================================================
// マクロ定義
//======================================================

//======================================================
// ADC　CONFIGパラメータ
//======================================================
// ADCチャネルの使用設定
const ADC_ch_config_t adc_ch_config = {
    .bits.enable_adc0 = false, // ADC0
    .bits.enable_adc1 = true,  // ADC1 → アナログスティックに使用
    .bits.enable_adc2 = true,  // ADC2 → アナログスティックに使用
    .bits.enable_adc3 = false, // ADC3 → 使用不可（VSYS/3測定用）
    .bits.enable_adc4 = false, // ADC4 → 使用不可（チップ温度測定用）
    .bits.reserve = 0,
};

// ADCパラメータ設定
const ADC_parameter_config_t adc_parameter_config = {
    .conv_mode = single_conv,             // 単発変換
    .rx_read_mode = single_register_read, // 単一レジスタ読み出し
    .div_int = 10000,                     // 変換周期設定（整数部分。ループ変換有効の場合の設定）★96以上推奨byデータシート
    .div_frac = 0,                        // 変換周期設定（分数部分。ループ変換有効の場合の設定）
};

//======================================================
// GPIO　CONFIGパラメータ
//======================================================
// GPIOピンの機能設定リスト
const GPIO_function_list_t gpioPin_func_list[] = {
    {GPIO0, NONE},
    {GPIO1, NONE},
    {GPIO2, NONE},
    {GPIO3, NONE},
    {GPIO4, NONE},
    {GPIO5, NONE},
    {GPIO6, NONE},
    {GPIO7, NONE},
    {GPIO8, NONE},
    {GPIO9, NONE},
    {GPIO10, NONE},
    {GPIO11, NONE},
    {GPIO12, F5}, // Bボタン=ゲーム機の下のボタン。tetrisにおける右回転ボタンに使用
    {GPIO13, F5}, // Aボタン=ゲーム機の右のボタン。tetrisにおける左回転ボタンに使用
    {GPIO14, F5}, // Yボタン=ゲーム機の左のボタン。tetrisにおけるコントロールボタン1に使用
    {GPIO15, F5}, // Xボタン=ゲーム機の上のボタン。tetrisにおけるコントロールボタン2に使用
    {GPIO16, F3}, // ディスプレイとのI2C通信用
    {GPIO17, F3}, // ディスプレイとのI2C通信用
    {GPIO18, NONE},
    {GPIO19, NONE},
    {GPIO20, NONE},
    {GPIO21, NONE},
    {GPIO22, NONE},
    {GPIO23, NONE},
    {GPIO24, NONE},
    {GPIO25, NONE},
    {GPIO26, NONE},  // ADC0設定可
    {GPIO27, F_ADC}, // ADC1設定可 → アナログスティックに使用
    {GPIO28, F_ADC}, // ADC2設定可 → アナログスティックに使用
    {GPIO29, NONE},  // 常時NONE（VSYS/3測定用）
};

// GPIOピンのIO設定リスト（初期値で入力なので、出力のみの設定でも可）
const GPIO_direction_list_t gpioPin_dir_list[] = {
    {GPIO0, DEFAULT},
    {GPIO1, DEFAULT},
    {GPIO2, DEFAULT},
    {GPIO3, DEFAULT},
    {GPIO4, DEFAULT},
    {GPIO5, DEFAULT},
    {GPIO6, DEFAULT},
    {GPIO7, DEFAULT},
    {GPIO8, DEFAULT},
    {GPIO9, DEFAULT},
    {GPIO10, DEFAULT},
    {GPIO11, DEFAULT},
    {GPIO12, IN}, // Bボタン（tetrisにおける右回転ボタン）に入力設定で使用
    {GPIO13, IN}, // Aボタン（tetrisにおける左回転ボタン）に入力設定で使用
    {GPIO14, IN}, // Yボタン（tetrisにおけるコントロールボタン1）に入力設定で使用
    {GPIO15, IN}, // Xボタン（tetrisにおけるコントロールボタン2）に入力設定で使用
    {GPIO16, DEFAULT},
    {GPIO17, DEFAULT},
    {GPIO18, DEFAULT},
    {GPIO19, DEFAULT},
    {GPIO20, DEFAULT},
    {GPIO21, DEFAULT},
    {GPIO22, DEFAULT},
    {GPIO23, DEFAULT},
    {GPIO24, DEFAULT},
    {GPIO25, DEFAULT},
    {GPIO26, DEFAULT},
    {GPIO27, DEFAULT},
    {GPIO28, DEFAULT},
    {GPIO29, DEFAULT}, // 使用不可なので常時DEFAULT
};

//======================================================
// I2C　CONFIGパラメータ（オブジェクト設定）
//======================================================
// ディスプレイ通信用I2Cの設定
const I2C_config_t config_I2C0_display = {
    .ch = I2C0,
    .gpioPin_SDA = GPIO16,
    .gpioPin_SCL = GPIO17,
    .mode = master,         // マイコンがマスター、ディスプレイがスレーブ
    .speed = standard_mode, // 通信速度は標準モード
    .addressing_mode = addressing_7bit,
    .default_address = 0x3C, // ディスプレイのアドレス。SA0はGNDなのでこっち
    // 参照：https://ktechnics.com/product/1-5-inch-128x128-oled-shield-screen-module-i2c/
};

// デバッグ用PCツールとの通信用I2Cの設定（※現状未使用）
const I2C_config_t config_I2C1_debug = {
    .ch = I2C1,
    .gpioPin_SDA = GPIO18,
    .gpioPin_SCL = GPIO19,
    .mode = slave, // マイコンがスレーブ、PCツールがマスター
    .speed = standard_mode,
    .addressing_mode = addressing_7bit,
    .default_address = 0x50, // 適当
};

//======================================================
// BUTTON　CONFIGパラメータ（オブジェクト設定）
//======================================================
// 各ボタンへのGPIO番号を割り当て
const BUTTON_config_t config_B_button = {
    .assign_gpioPin = GPIO12,
};
const BUTTON_config_t config_A_button = {
    .assign_gpioPin = GPIO13,
};
const BUTTON_config_t config_Y_button = {
    .assign_gpioPin = GPIO14,
};
const BUTTON_config_t config_X_button = {
    .assign_gpioPin = GPIO15,
};

//======================================================
// analogStick　CONFIGパラメータ（オブジェクト設定）
//======================================================
// アナログスティックパラメータ設定
const ANALOGSTICK_config_t config_analogStick_1 = {
    .assign_x_adc_ch = adc_ch1,
    .assign_y_adc_ch = adc_ch2,
    .x_max = 100,  // スティックを右に最大まで倒したときのX座標値
    .x_min = -100, // スティックを左に最大まで倒したときのX座標値
    .y_max = 100,  // スティックを上に最大まで倒したときのY座標値
    .y_min = -100, // スティックを下に最大まで倒したときのY座標値
};

//======================================================
// SH1107　CONFIGパラメータ
//======================================================
// SH1107パラメータ設定（割り当てるI2Cchのみ設定）
const SH1107_config_t config_SH1107_1 = {
    .assign_I2C_ch = I2C0,
};

//======================================================
// debug_com　CONFIGパラメータ
//======================================================
// デバッグ通信設定（※現状未使用）
const DEBUG_COM_config_t config_debug_com = {
    .I2C_ch = I2C1,
};