/**
 * @file   timer_ops.c
 * @brief  TIMERコンポーネント・レジスタ操作実装
 */

//======================================================
// インクルード
//======================================================
#include "timer.h"
#include "timer_internal.h"
#include "register.h"
#include "interrupt.h"
#include "bit.h"
#include "typedef.h"

//======================================================
// マクロ定義
//======================================================

//======================================================
// 型定義
//======================================================

//======================================================
// 変数・定数
//======================================================
static TIMER_callback_func_pointer_t timer_callback_func_list[3] = {NULL};
static const INTERRUPT_exception_number_t alarm_num_to_exception_list[4] = {TIMER_IRQ_0, TIMER_IRQ_1, TIMER_IRQ_2, TIMER_IRQ_3};

//======================================================
// プロトタイプ宣言
//======================================================

//======================================================
// 公開関数定義
//======================================================
/**
 * @brief  現在時刻取得
 * @return 現在時刻[us]
 */
uint64_t TIMER_get_time_us()
{
    return ((uint64_t)TIMER_TIMEHR << 32) | TIMER_TIMELR;
}

/**
 * @brief  指定マイクロ秒待機
 * @param  us 待機時間[us]
 * @return なし
 */
void TIMER_wait_us(uint64_t us)
{
    uint64_t start = TIMER_get_time_us();
    while (true)
    {
        if (TIMER_get_time_us() - start > us)
            return;
    }
}

/**
 * @brief  指定ミリ秒待機
 * @param  ms 待機時間[ms]
 * @return なし
 */
void TIMER_wait_ms(uint64_t ms)
{
    TIMER_wait_us(ms * 1000);
}

/**
 * @brief  ストップウォッチ計測開始
 * @return 計測開始時のストップウォッチ情報
 */
TIMER_stopwatch_t TIMER_start_stopwatch()
{
    TIMER_stopwatch_t stopwatch;

    stopwatch.start_us = TIMER_get_time_us();
    return stopwatch;
}

/**
 * @brief  ストップウォッチ計測終了
 * @param  stopwatch 計測開始時のストップウォッチ情報
 * @param  unit 取得時間単位：ms or us
 * @return ストップウォッチ開始からの経過時間
 */
uint64_t TIMER_stop_stopwatch(TIMER_stopwatch_t stopwatch, TIMER_time_unit_t unit)
{
    uint64_t stop_us = TIMER_get_time_us();

    if (ms == unit)
    {
        return ((stop_us - stopwatch.start_us) / 1000);
    }
    else // us
    {
        return (stop_us - stopwatch.start_us);
    }
}

/**
 * @brief  アラーム発火時刻設定
 * @param  fire_time_us 現在時刻からの発火時間[us]
 * @param  alarm_num アラーム番号
 * @return なし
 * @note アラーム時刻は32ビット値で設定するため対象レジスタへ直接書き込み
 *       また、発火後はレジスタクリア不要
 */
void TIMER_set_alarm(uint32_t fire_time_us, TIMER_alarm_num_t alarm_num)
{
    TIMER_ALARM(alarm_num) = fire_time_us + TIMER_get_time_us();
}

/**
 * @brief  アラーム割り込み有効無効設定
 * @param  is_enable 設定内容（有効or無効）
 * @param  alarm_num アラーム番号
 * @return なし
 */
void TIMER_enable_alarm_interrupt(bool is_enable, TIMER_alarm_num_t alarm_num)
{
    TIMER_INTE = ((TIMER_INTE & ~(MASK_1BIT << alarm_num)) | (is_enable << alarm_num));
    INTERRUPT_enable_IRQn(alarm_num_to_exception_list[alarm_num]);
}

/**
 * @brief  アラームコールバック関数登録
 * @details アラームが発火した際にコールバックさせる関数を登録する
 *          アラーム割り込みが発生した時はまずtimer_alarmN_interrupt_callbackが呼ばれ、
 *          その内部で本関数で登録されたコールバック関数が呼ばれる
 * @param  callback_func 登録コールバック関数
 * @param  alarm_num アラーム番号
 * @return なし
 */
void TIMER_set_alarm_callback_function(TIMER_callback_func_pointer_t callback_func, TIMER_alarm_num_t alarm_num)
{
    timer_callback_func_list[alarm_num] = callback_func;
}

/**
 * @brief  Alarm0コールバック関数
 * @return なし
 */
void timer_alarm0_interrupt_callback()
{
    TIMER_INTR = (1 << alarm0); // Alarm0用の割り込みフラグクリア

    if (timer_callback_func_list[alarm0] != NULL)
        timer_callback_func_list[alarm0]();
}

/**
 * @brief  Alarm1コールバック関数
 * @return なし
 */
void timer_alarm1_interrupt_callback()
{
    TIMER_INTR = (1 << alarm1); // Alarm1用の割り込みフラグクリア

    if (timer_callback_func_list[alarm1] != NULL)
        timer_callback_func_list[alarm1]();
}

/**
 * @brief  Alarm2コールバック関数
 * @return なし
 */
void timer_alarm2_interrupt_callback()
{
    TIMER_INTR = (1 << alarm2); // Alarm2用の割り込みフラグクリア

    if (timer_callback_func_list[alarm2] != NULL)
        timer_callback_func_list[alarm2]();
}

/**
 * @brief  Alarm3コールバック関数
 * @return なし
 */
void timer_alarm3_interrupt_callback()
{
    TIMER_INTR = (1 << alarm3); // Alarm3用の割り込みフラグクリア

    if (timer_callback_func_list[alarm3] != NULL)
        timer_callback_func_list[alarm3]();
}

//======================================================
// 内部関数定義
//======================================================