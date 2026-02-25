/**
 * @file   timer.h
 * @brief  TIMERコンポーネント・外部公開定義
 */

#ifndef __TIMER_H__
#define __TIMER_H__

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
 * @brief ストップウォッチ情報
 */
typedef struct
{
    uint64_t start_us; /**< ストップウォッチ開始時のタイムスタンプ（単位：us） */
} TIMER_stopwatch_t;

/**
 * @brief 時間単位
 */
typedef enum
{
    ms,
    us,
} TIMER_time_unit_t;

/**
 * @brief アラーム番号
 * @note RP2040では4つのアラームがあり、各アラーム個別で割り込みを利用できる
 */
typedef enum
{
    alarm0 = 0,
    alarm1,
    alarm2,
    alarm3,
} TIMER_alarm_num_t;

/**
 * @brief TIMERコールバック関数設定用ポインタ
 * @details 外部コンポーネントが割り込みコールバック関数を登録する際、関数ポインタをこの型にキャストして渡させる
 */
typedef void (*TIMER_callback_func_pointer_t)();

//======================================================
// グローバル変数・定数extern宣言
//======================================================

//======================================================
// グローバル関数extern宣言
//======================================================
/* init */
extern void TIMER_initialize();

/* ops */
extern uint64_t TIMER_get_time_us();
extern void TIMER_wait_us(uint64_t us);
extern void TIMER_wait_ms(uint64_t ms);
extern TIMER_stopwatch_t TIMER_start_stopwatch();
extern uint64_t TIMER_stop_stopwatch(TIMER_stopwatch_t stopwatch, TIMER_time_unit_t unit);
extern void TIMER_set_alarm(uint32_t fire_time_us, TIMER_alarm_num_t alarm_num);
extern void TIMER_enable_alarm_interrupt(bool is_enable, TIMER_alarm_num_t alarm_num);
extern void TIMER_set_alarm_callback_function(TIMER_callback_func_pointer_t callback_func, TIMER_alarm_num_t alarm_num);

#endif /* __TIMER_H__ */