/**
 * @file   gpio.h
 * @brief  GPIOコンポーネント・外部公開定義
 */

#ifndef __GPIO_H__
#define __GPIO_H__

//======================================================
// インクルード
//======================================================
#include "typedef.h"

//======================================================
// マクロ定義
//======================================================

//======================================================
// 型定義
//======================================================
/**
 * @brief gpioピンのgpio番号定義
 */
typedef enum
{
    GPIO0 = 0,
    GPIO1,
    GPIO2,
    GPIO3,
    GPIO4,
    GPIO5,
    GPIO6,
    GPIO7,
    GPIO8,
    GPIO9,
    GPIO10,
    GPIO11,
    GPIO12,
    GPIO13,
    GPIO14,
    GPIO15,
    GPIO16,
    GPIO17,
    GPIO18,
    GPIO19,
    GPIO20,
    GPIO21,
    GPIO22,
    GPIO23,
    GPIO24,
    GPIO25,
    GPIO26,
    GPIO27,
    GPIO28,
    GPIO29 = 29,
} GPIO_num_t;

/**
 * @brief gpioピンのFunction番号定義
 * @details ADCは兼用機能とは本来別扱いだが、同様に設定できるよう本定義に含める。
 */
typedef enum
{
    F1 = 1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F_ADC,
    NONE, /**< 設定無し */
} GPIO_function_num_t;

/**
 * @brief gpioピンの入出力方向定義
 */
typedef enum
{
    IN = 0,
    OUT = 1,
    DEFAULT, /**< 設定無し */
} GPIO_io_direction_t;

/**
 * @brief GPIOピンの機能設定リスト定義（config用）
 * @details 各GPIOピンに対して何の機能を有効化するかをFunction番号を指定することで設定する
 *          configでこの定義に対する実パラメータを設定し、初期化関数に渡す事で設定を反映させる
 *          Function番号と機能内容の対応については以下を参照
 *          　データシート：https://pip-assets.raspberrypi.com/categories/814-rp2040/documents/RP-008371-DS-1-rp2040-datasheet.pdf?disposition=inline
 *          　1.4.3. GPIO Functions
 */
typedef struct
{
    GPIO_num_t gpio_num;          /**< gpio番号 */
    GPIO_function_num_t func_num; /**< Function番号 */
} GPIO_function_list_t;

/**
 * @brief GPIOピンの入出力方向設定リスト定義（config用）
 * @details 各GPIOピンに対して入出力方向を設定する
 *          configでこの定義に対する実パラメータを設定し、初期化関数に渡す事で設定を反映させる
 */
typedef struct
{
    GPIO_num_t gpio_num;              /**< gpio番号 */
    GPIO_io_direction_t io_direction; /**< IO方向 */
} GPIO_direction_list_t;

//======================================================
// グローバル変数・定数extern宣言
//======================================================

//======================================================
// グローバル関数extern宣言
//======================================================
/* init */
extern void GPIO_initialize(const GPIO_function_list_t *gpioPin_func_list, const GPIO_direction_list_t *direction_list);

/* ops */
extern bool GPIO_get_input(GPIO_num_t gpio_num);
extern void GPIO_set_output(GPIO_num_t gpio_num, bool output_level);

#endif /* __GPIO_H__ */