/**
 * @file   timer_init.c
 * @brief  TIMERコンポーネント・初期化実装
 */

//======================================================
// インクルード
//======================================================
#include "timer.h"
#include "timer_internal.h"
#include "interrupt.h"

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
 * @brief  timer機能初期化
 * @return なし
 * @note 現状は割り込みハンドラへのコールバック関数登録のみ。TIMERの固定コールバック関数を割り込み時のコールバック関数に登録している
 *       外部関数のコールバック関数登録は別途必要。TIMER_set_alarm_callback_functionから行うこと
 *       コールバックの流れは以下
 *       ・割り込み発生
 *       ・TIMER_IRQ_n_Handler がコールバック
 *       ・timer_alarmn_interrupt_callback がコールバック
 *       ・TIMER_set_alarm_callback_functionで登録された外部関数 がコールバック
 */
void TIMER_initialize()
{
    INTERRUPT_set_callback_function((INTERRUPT_callback_func_pointer_t)timer_alarm0_interrupt_callback, TIMER_IRQ_0);
    INTERRUPT_set_callback_function((INTERRUPT_callback_func_pointer_t)timer_alarm1_interrupt_callback, TIMER_IRQ_1);
    INTERRUPT_set_callback_function((INTERRUPT_callback_func_pointer_t)timer_alarm2_interrupt_callback, TIMER_IRQ_2);
    INTERRUPT_set_callback_function((INTERRUPT_callback_func_pointer_t)timer_alarm3_interrupt_callback, TIMER_IRQ_3);
}

//======================================================
// 内部関数定義
//======================================================