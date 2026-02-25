/**
 * @file   gpio_ops.c
 * @brief  GPIOコンポーネント・レジスタ操作実装
 */

//======================================================
// インクルード
//======================================================
#include "gpio.h"
#include "gpio_internal.h"
#include "typedef.h"
#include "register.h"
#include "bit.h"

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
/**
 * @brief  GPIOピンの入力レベルを取得
 * @param  gpio_num 取得対象のGPIOピン番号
 * @return 入力レベル（true: High、false: Low）
 */
bool GPIO_get_input(GPIO_num_t gpio_num)
{
    return (SIO_GPIO_IN & (MASK_1BIT << gpio_num));
}

/**
 * @brief  GPIOピンの出力レベルを設定
 * @param  gpio_num 設定対象のGPIOピン番号
 * @param  output_level 出力レベル（true: High、false: Low）
 * @return なし
 */
void GPIO_set_output(GPIO_num_t gpio_num, bool output_level)
{
    SIO_GPIO_OUT = (SIO_GPIO_OUT & ~(MASK_1BIT << gpio_num)) | (output_level << gpio_num);
}

/**
 * @brief  GPIOピンのFunctionを設定
 * @details NONE指定時は設定を行わず、F_ADC指定時はADC用のPAD設定を行い、
 *          それ以外は兼用機能レジスタにFunction番号を設定する。
 * @param  gpio_num 設定対象のGPIOピン番号
 * @param  func_num 設定するFunction番号
 * @return なし
 */
void gpio_select_function(GPIO_num_t gpio_num, GPIO_function_num_t func_num)
{
    if (NONE == func_num) // 機能設定無し
    {
        // do nothing
    }
    else if (F_ADC == func_num) // ADC利用の場合のみ、兼用機能設定とは別処理
    {
        GPIO_N_PAD_CONTROL(gpio_num) &= ~(MASK_1BIT << 6); // Input EnableをLowに
        GPIO_N_PAD_CONTROL(gpio_num) |= (MASK_1BIT << 7);  // Output DisableをHighに
    }
    else // 兼用機能設定
    {
        GPIO_N_CTRL(gpio_num) = (GPIO_N_CTRL(gpio_num) & ~(MASK_5BIT << 0)) | (func_num << 0);
    }
}

/**
 * @brief  GPIOピンの入出力方向を設定
 * @param  gpio_num 設定対象のGPIOピン番号
 * @param  direction 設定する入出力方向
 * @return なし
 */
void gpio_set_direction(GPIO_num_t gpio_num, GPIO_io_direction_t direction)
{
    if (!(DEFAULT == direction)) // DEFAULTの場合 → do nothing
    {
        SIO_GPIO_OE = (SIO_GPIO_OE & ~(MASK_1BIT << gpio_num)) | (direction << gpio_num); // 入出力設定
    }
}

//======================================================
// 内部関数定義
//======================================================