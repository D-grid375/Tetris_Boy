/**
 * @file   gpio_init.c
 * @brief  GPIOコンポーネント・初期化実装
 */

//======================================================
// インクルード
//======================================================
#include "gpio.h"
#include "gpio_internal.h"
#include "typedef.h"
#include "register.h"

//======================================================
// マクロ定義
//======================================================
#define NUMBER_OF_GPIO 30 // 0~29 29はPico-wでは未実装？

//======================================================
// 型定義
//======================================================

//======================================================
// 変数・定数
//======================================================

//======================================================
// プロトタイプ宣言
//======================================================
static void GPIO_initialize_function(const GPIO_function_list_t *gpioPin_func_list);
static void GPIO_initialize_direction(const GPIO_direction_list_t *direction_list);

//======================================================
// 公開関数定義
//======================================================
/**
 * @brief  GPIOピンの初期化
 * @details 各GPIOピンに対し、割当Functionと入出力方向の設定を行う
 *          GPIO以外の機能を利用する場合、本関数で機能選択をした後に各コンポーネント側で別途初期化が必要
 * @param  gpioPin_func_list 各GPIOピンに割り当てるFunctionリスト
 * @param  direction_list 各GPIOピンの入出力方向設定リスト
 * @return なし
 */
void GPIO_initialize(const GPIO_function_list_t *gpioPin_func_list, const GPIO_direction_list_t *direction_list)
{
    GPIO_initialize_function(gpioPin_func_list);
    GPIO_initialize_direction(direction_list);
}

//======================================================
// 内部関数定義
//======================================================
/**
 * @brief  各GPIOピンに対する割当Functionの初期化設定
 * @param  gpioPin_func_list 各GPIOピンに割り当てるFunctionリスト
 * @return なし
 */
static void GPIO_initialize_function(const GPIO_function_list_t *gpioPin_func_list)
{
    for (int i = 0; i < NUMBER_OF_GPIO; i++)
    {
        gpio_select_function(gpioPin_func_list[i].gpio_num, gpioPin_func_list[i].func_num);
    }
}

/**
 * @brief  各GPIOピンに対する入出力方向の初期化設定
 * @param  direction_list 各GPIOピンの入出力方向設定リスト
 * @return なし
 */
static void GPIO_initialize_direction(const GPIO_direction_list_t *direction_list)
{
    for (int i = 0; i < NUMBER_OF_GPIO; i++)
    {
        gpio_set_direction(direction_list[i].gpio_num, direction_list[i].io_direction);
    }
}