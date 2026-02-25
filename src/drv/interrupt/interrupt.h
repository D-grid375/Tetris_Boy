/**
 * @file   interrupt.h
 * @brief  割り込みコンポーネント・外部公開定義
 */

#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

//======================================================
// インクルード
//======================================================

//======================================================
// マクロ定義
//======================================================

//======================================================
// 型定義
//======================================================
/**
 * @brief armv6m例外番号
 * @details 割り込みハンドラへのコールバック関数登録で使用する
 *          参照：DDI0419E_armv6m_arm.pdf - B1.5.2 Exception number definition
 */
typedef enum
{
    // 0:初期スタックポインタ
    Reset = 1,
    NMI = 2,
    HardFault = 3,
    // 4~10:Reserved
    SVCall = 11,
    // 12~13:Reserved
    PendSV = 14,
    SysTick_optional = 15,
    TIMER_IRQ_0 = 16,
    TIMER_IRQ_1 = 17,
    TIMER_IRQ_2 = 18,
    TIMER_IRQ_3 = 19,
    PWM_IRQ_WRAP = 20,
    USBCTRL_IRQ = 21,
    XIP_IRQ = 22,
    PIO0_IRQ_0 = 23,
    PIO0_IRQ_1 = 24,
    PIO1_IRQ_0 = 25,
    PIO1_IRQ_1 = 26,
    DMA_IRQ_0 = 27,
    DMA_IRQ_1 = 28,
    IO_IRQ_BANK0 = 29,
    IO_IRQ_QSPI = 30,
    SIO_IRQ_PROC0 = 31,
    SIO_IRQ_PROC1 = 32,
    CLOCKS_IRQ = 33,
    SPI0_IRQ = 34,
    SPI1_IRQ = 35,
    UART0_IRQ = 36,
    UART1_IRQ = 37,
    ADC_IRQ_FIFO = 38,
    I2C0_IRQ = 39,
    I2C1_IRQ = 40,
    RTC_IRQ = 41,
} INTERRUPT_exception_number_t;

/**
 * @brief コールバック関数設定用ポインタ
 * @details 外部コンポーネントが割り込みコールバック関数を登録する際、関数ポインタをこの型にキャストして渡させる
 */
typedef void (*INTERRUPT_callback_func_pointer_t)();

//======================================================
// グローバル変数・定数extern宣言
//======================================================

//======================================================
// グローバル関数extern宣言
//======================================================
/* init */
extern void INTERRUPT_initialize();

/* ctrl */
extern void INTERRUPT_set_callback_function(INTERRUPT_callback_func_pointer_t callback_func, INTERRUPT_exception_number_t target_exception);
extern void INTERRUPT_enable_IRQn(INTERRUPT_exception_number_t interrupt_source);

#endif /* __INTERRUPT_H__ */