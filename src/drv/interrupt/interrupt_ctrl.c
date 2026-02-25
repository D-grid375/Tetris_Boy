/**
 * @file   interrupt_ctrl.c
 * @brief  割り込みコンポーネント・汎用制御実装
 */

//======================================================
// インクルード
//======================================================
#include "interrupt.h"
#include "interrupt_internal.h"
#include "typedef.h"
#include "register.h"
#include "bit.h"
#include "address.h"

//======================================================
// マクロ定義
//======================================================
// RP2040例外割り当て
//     0 : 初期SP
//  1~15 : Exception
// 16~47 : External Interrupt（44~47は割り当て無し）
#define NUMBER_OF_EXEPTION 48     // 例外総数（＝ベクタテーブルのサイズ）
#define NUMBER_OF_IRQ 26          // IRQ総数（0~25）
#define IRQ_OFFSET_IN_EXEPTION 16 // 例外番号中のIRQ番号の変換用オフセット

//======================================================
// 型定義
//======================================================

//======================================================
// 変数・定数
//======================================================
__attribute__((aligned(256))) static void *ram_vector_table[NUMBER_OF_EXEPTION]; // RAM上のベクタテーブル領域　VTOR要件で128バイトアライン必要
static INTERRUPT_callback_func_pointer_t IRQ_callback_func_list[NUMBER_OF_IRQ] = {NULL};

//======================================================
// プロトタイプ宣言
//======================================================
// static void interrupt_copy_vector_table_to_ram();
// 割り込みハンドラ
static void TIMER_IRQ_0_Handler(void);
static void TIMER_IRQ_1_Handler(void);
static void TIMER_IRQ_2_Handler(void);
static void TIMER_IRQ_3_Handler(void);
static void PWM_IRQ_WRAP_Handler(void);
static void USBCTRL_IRQ_Handler(void);
static void XIP_IRQ_Handler(void);
static void PIO0_IRQ_0_Handler(void);
static void PIO0_IRQ_1_Handler(void);
static void PIO1_IRQ_0_Handler(void);
static void PIO1_IRQ_1_Handler(void);
static void DMA_IRQ_0_Handler(void);
static void DMA_IRQ_1_Handler(void);
static void IO_IRQ_BANK0_Handler(void);
static void IO_IRQ_QSPI_Handler(void);
static void SIO_IRQ_PROC0_Handler(void);
static void SIO_IRQ_PROC1_Handler(void);
static void CLOCKS_IRQ_Handler(void);
static void SPI0_IRQ_Handler(void);
static void SPI1_IRQ_Handler(void);
static void UART0_IRQ_Handler(void);
static void UART1_IRQ_Handler(void);
static void ADC_IRQ_FIFO_Handler(void);
static void I2C0_IRQ_Handler(void);
static void I2C1_IRQ_Handler(void);
static void RTC_IRQ_Handler(void);

//======================================================
// 公開関数定義
//======================================================
/**
 * @brief  割り込みコールバック登録
 * @details 割り込みが発生した時にコールバックする関数を登録する
 *          IRQ範囲外の例外番号が指定された場合は処理しない
 * @param[in] callback_func 登録するコールバック関数ポインタ
 * @param[in] exception_number_of_interrupt 登録対象の例外番号（IRQのみ受け付ける）
 * @return なし
 */
void INTERRUPT_set_callback_function(INTERRUPT_callback_func_pointer_t callback_func, INTERRUPT_exception_number_t exception_number_of_interrupt)
{
    if (exception_number_of_interrupt < IRQ_OFFSET_IN_EXEPTION) // 割り込み以外の例外を指定された場合：設定不可
        return;

    IRQ_callback_func_list[exception_number_of_interrupt - IRQ_OFFSET_IN_EXEPTION] = callback_func;
}

/**
 * @brief  NVIC割り込み有効化
 * @param[in] exception_number_of_interrupt 有効化対象の例外番号（IRQのみ受け付ける）
 * @return なし
 * @details IRQ範囲外の例外番号が指定された場合は何もせず復帰する
 */
void INTERRUPT_enable_IRQn(INTERRUPT_exception_number_t exception_number_of_interrupt)
{
    if (exception_number_of_interrupt < IRQ_OFFSET_IN_EXEPTION) // 割り込み以外の例外を指定された場合：設定不可
        return;

    // このレジスタは1のWriteのみ対応（0書き込み不可）
    PPB_NVIC_ISER |= (MASK_1BIT << (exception_number_of_interrupt - IRQ_OFFSET_IN_EXEPTION));
}

/**
 * @brief  ベクタテーブルRAMコピー
 * @return なし
 * @details RAM配置用ベクタテーブルにROMのベクタテーブル内容を全要素コピーする
 */
void interrupt_copy_vector_table_to_ram()
{
    // voidポインタ のポインタ
    void **rom_vector_table = (void **)ROM_VECTOR_TABLE_BASE;

    // ROM → RAM にコピー
    for (int i = 0; i < NUMBER_OF_EXEPTION; ++i)
    {
        // 左辺：voidポインタの配列に対してインデックス指定＝voidポインタ
        // 右辺：voidポインタのポインタに対してインデックスで間接参照＝voidポインタ
        ram_vector_table[i] = rom_vector_table[i];
    }
}

/**
 * @brief  IRQハンドラ割り当て
 * @details RAMに展開したベクタテーブルのIRQ部分に、割り込みハンドラ関数のアドレスをセットする
 *          割り込みコールバック関数の登録とベクタテーブルのRAM展開後にコールする事
 * @return なし
 */
void interrupt_set_IRQ_handler()
{
    INTERRUPT_callback_func_pointer_t irq_handler_list[NUMBER_OF_IRQ] = {
        TIMER_IRQ_0_Handler,   // IRQ 0
        TIMER_IRQ_1_Handler,   // IRQ 1
        TIMER_IRQ_2_Handler,   // IRQ 2
        TIMER_IRQ_3_Handler,   // IRQ 3
        PWM_IRQ_WRAP_Handler,  // IRQ 4
        USBCTRL_IRQ_Handler,   // IRQ 5
        XIP_IRQ_Handler,       // IRQ 6
        PIO0_IRQ_0_Handler,    // IRQ 7
        PIO0_IRQ_1_Handler,    // IRQ 8
        PIO1_IRQ_0_Handler,    // IRQ 9
        PIO1_IRQ_1_Handler,    // IRQ10
        DMA_IRQ_0_Handler,     // IRQ11
        DMA_IRQ_1_Handler,     // IRQ12
        IO_IRQ_BANK0_Handler,  // IRQ13
        IO_IRQ_QSPI_Handler,   // IRQ14
        SIO_IRQ_PROC0_Handler, // IRQ15
        SIO_IRQ_PROC1_Handler, // IRQ16
        CLOCKS_IRQ_Handler,    // IRQ17
        SPI0_IRQ_Handler,      // IRQ18
        SPI1_IRQ_Handler,      // IRQ19
        UART0_IRQ_Handler,     // IRQ20
        UART1_IRQ_Handler,     // IRQ21
        ADC_IRQ_FIFO_Handler,  // IRQ22
        I2C0_IRQ_Handler,      // IRQ23
        I2C1_IRQ_Handler,      // IRQ24
        RTC_IRQ_Handler        // IRQ25
    };

    for (uint8_t irq = 0; irq < NUMBER_OF_IRQ; irq++)
    {
        ram_vector_table[irq + IRQ_OFFSET_IN_EXEPTION] = irq_handler_list[irq];
    }
}

/**
 * @brief  VTOR設定
 * @details VTOR（Vector Table Offset Register）へRAMベクタテーブル先頭アドレスを設定する
 *          ベクタテーブルRAMコピーとIRQハンドラ割り当て後にコールする事
 * @return なし
 */
void interrupt_set_vtor()
{
    PPB_TBLOFF = (uint32_t)(ram_vector_table); // 0~7bitはReserved（128バイトアラインのため）
}

//======================================================
// 内部関数定義
//======================================================
// IRQ0~25の割り込みハンドラ関数（これらがベクタテーブルに割り当てられる）　処理内容は外部関数のコールバックのみ
// clang-format off
static void TIMER_IRQ_0_Handler(void)     { if (IRQ_callback_func_list[0]  != NULL) IRQ_callback_func_list[0](); }
static void TIMER_IRQ_1_Handler(void)     { if (IRQ_callback_func_list[1]  != NULL) IRQ_callback_func_list[1](); }
static void TIMER_IRQ_2_Handler(void)     { if (IRQ_callback_func_list[2]  != NULL) IRQ_callback_func_list[2](); }
static void TIMER_IRQ_3_Handler(void)     { if (IRQ_callback_func_list[3]  != NULL) IRQ_callback_func_list[3](); }
static void PWM_IRQ_WRAP_Handler(void)    { if (IRQ_callback_func_list[4]  != NULL) IRQ_callback_func_list[4](); }
static void USBCTRL_IRQ_Handler(void)     { if (IRQ_callback_func_list[5]  != NULL) IRQ_callback_func_list[5](); }
static void XIP_IRQ_Handler(void)         { if (IRQ_callback_func_list[6]  != NULL) IRQ_callback_func_list[6](); }
static void PIO0_IRQ_0_Handler(void)      { if (IRQ_callback_func_list[7]  != NULL) IRQ_callback_func_list[7](); }
static void PIO0_IRQ_1_Handler(void)      { if (IRQ_callback_func_list[8]  != NULL) IRQ_callback_func_list[8](); }
static void PIO1_IRQ_0_Handler(void)      { if (IRQ_callback_func_list[9]  != NULL) IRQ_callback_func_list[9](); }
static void PIO1_IRQ_1_Handler(void)      { if (IRQ_callback_func_list[10] != NULL) IRQ_callback_func_list[10](); }
static void DMA_IRQ_0_Handler(void)       { if (IRQ_callback_func_list[11] != NULL) IRQ_callback_func_list[11](); }
static void DMA_IRQ_1_Handler(void)       { if (IRQ_callback_func_list[12] != NULL) IRQ_callback_func_list[12](); }
static void IO_IRQ_BANK0_Handler(void)    { if (IRQ_callback_func_list[13] != NULL) IRQ_callback_func_list[13](); }
static void IO_IRQ_QSPI_Handler(void)     { if (IRQ_callback_func_list[14] != NULL) IRQ_callback_func_list[14](); }
static void SIO_IRQ_PROC0_Handler(void)   { if (IRQ_callback_func_list[15] != NULL) IRQ_callback_func_list[15](); }
static void SIO_IRQ_PROC1_Handler(void)   { if (IRQ_callback_func_list[16] != NULL) IRQ_callback_func_list[16](); }
static void CLOCKS_IRQ_Handler(void)      { if (IRQ_callback_func_list[17] != NULL) IRQ_callback_func_list[17](); }
static void SPI0_IRQ_Handler(void)        { if (IRQ_callback_func_list[18] != NULL) IRQ_callback_func_list[18](); }
static void SPI1_IRQ_Handler(void)        { if (IRQ_callback_func_list[19] != NULL) IRQ_callback_func_list[19](); }
static void UART0_IRQ_Handler(void)       { if (IRQ_callback_func_list[20] != NULL) IRQ_callback_func_list[20](); }
static void UART1_IRQ_Handler(void)       { if (IRQ_callback_func_list[21] != NULL) IRQ_callback_func_list[21](); }
static void ADC_IRQ_FIFO_Handler(void)    { if (IRQ_callback_func_list[22] != NULL) IRQ_callback_func_list[22](); }
static void I2C0_IRQ_Handler(void)        { if (IRQ_callback_func_list[23] != NULL) IRQ_callback_func_list[23](); }
static void I2C1_IRQ_Handler(void)        { if (IRQ_callback_func_list[24] != NULL) IRQ_callback_func_list[24](); }
static void RTC_IRQ_Handler(void)         { if (IRQ_callback_func_list[25] != NULL) IRQ_callback_func_list[25](); }
// clang-format on