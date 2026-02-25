/**
 * @file   config.h
 * @brief  CONFIG値・外部公開定義
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__

//======================================================
// インクルード
//======================================================
#include "typedef.h"
#include "adc.h"
#include "button.h"
#include "debug_com.h"
#include "analogStick.h"
#include "SH1107.h"
#include "gpio.h"
#include "I2C.h"

//======================================================
// extern宣言
//======================================================
// Configパラメータ
extern const ADC_ch_config_t adc_ch_config;
extern const ADC_parameter_config_t adc_parameter_config;
extern const GPIO_function_list_t gpioPin_func_list[];
extern const GPIO_direction_list_t gpioPin_dir_list[];
extern const I2C_config_t config_I2C0_display;
extern const I2C_config_t config_I2C1_debug;
extern const BUTTON_config_t config_B_button;
extern const BUTTON_config_t config_A_button;
extern const BUTTON_config_t config_Y_button;
extern const BUTTON_config_t config_X_button;
extern const ANALOGSTICK_config_t config_analogStick_1;
extern const SH1107_config_t config_SH1107_1;
extern const DEBUG_COM_config_t config_debug_com;

#endif /* __CONFIG_H__ */