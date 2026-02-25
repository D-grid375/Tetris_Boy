/**
 * @file   register.h
 * @brief  レジスタアクセス定義
 */

#ifndef __REGISTER_H__
#define __REGISTER_H__

//======================================================
// インクルードファイル
//======================================================
#include "address.h"
#include "typedef.h"

//======================================================
// 汎用マクロ
//======================================================
#define VOLATILE_ACCESS(address)      (*(volatile uint32_t *)(address))

//======================================================
// GPIO関連レジスタ定義
//======================================================
#define GPIO_N_CTRL(GPIO_NUM)          VOLATILE_ACCESS(IO_BANK0_BASE + 0x04 + 0x08 * GPIO_NUM)   // 各GPIOのCTRLレジスタ
#define GPIO_N_PAD_CONTROL(GPIO_NUM)   VOLATILE_ACCESS(PADS_BANK0_BASE + 0x04 + 0x04 * GPIO_NUM) // 各GPIOのPad Controlレジスタ

//======================================================
// ADC関連レジスタ定義
//======================================================
#define ADC_CS           VOLATILE_ACCESS(ADC_BASE + 0x00) // ADC Control and Status
#define ADC_RESULT       VOLATILE_ACCESS(ADC_BASE + 0x04) // ADC 変換結果（0~11bit）
#define ADC_FCS          VOLATILE_ACCESS(ADC_BASE + 0x08) // ADC FIFO Control and Status
#define ADC_FIFO         VOLATILE_ACCESS(ADC_BASE + 0x0c) // ADC FIFOバッファ
#define ADC_DIV          VOLATILE_ACCESS(ADC_BASE + 0x10) // ADC ループ変換のサンプリング周期設定

//======================================================
// SIO関連レジスタ定義
//======================================================
#define SIO_CPUID        VOLATILE_ACCESS(SIO_BASE + 0x00) // CPUIDの読み出し

#define SIO_GPIO_IN      VOLATILE_ACCESS(SIO_BASE + 0x04) // GPIO入力値の読み出し

#define SIO_GPIO_OUT     VOLATILE_ACCESS(SIO_BASE + 0x10) // GPIO出力値の設定（初期値は0=low）
#define SIO_GPIO_OUT_SET VOLATILE_ACCESS(SIO_BASE + 0x14) // GPIO出力値の設定（セット用）
#define SIO_GPIO_OUT_CLR VOLATILE_ACCESS(SIO_BASE + 0x18) // GPIO出力値の設定（クリア用）

#define SIO_GPIO_OE      VOLATILE_ACCESS(SIO_BASE + 0x20) // GPIO入出力の設定（初期値は0=input）
#define SIO_GPIO_OE_SET  VOLATILE_ACCESS(SIO_BASE + 0x24) // GPIO入出力の設定（セット用）
#define SIO_GPIO_OE_CLR  VOLATILE_ACCESS(SIO_BASE + 0x28) // GPIO入出力の設定（クリア用）

//======================================================
// I2C関連レジスタ定義
//======================================================
#define I2Cn_IC_CON(n)                VOLATILE_ACCESS(I2Cn_BASE(n) + 0x00)
#define I2Cn_IC_TAR(n)                VOLATILE_ACCESS(I2Cn_BASE(n) + 0x04)
#define I2Cn_IC_SAR(n)                VOLATILE_ACCESS(I2Cn_BASE(n) + 0x08)
#define I2Cn_IC_DATA_CMD(n)           VOLATILE_ACCESS(I2Cn_BASE(n) + 0x10)
#define I2Cn_IC_SS_SCL_HCNT(n)        VOLATILE_ACCESS(I2Cn_BASE(n) + 0x14)
#define I2Cn_IC_SS_SCL_LCNT(n)        VOLATILE_ACCESS(I2Cn_BASE(n) + 0x18)
#define I2Cn_IC_FS_SCL_HCNT(n)        VOLATILE_ACCESS(I2Cn_BASE(n) + 0x1C)
#define I2Cn_IC_FS_SCL_LCNT(n)        VOLATILE_ACCESS(I2Cn_BASE(n) + 0x20)
#define I2Cn_IC_INTR_STAT(n)          VOLATILE_ACCESS(I2Cn_BASE(n) + 0x2C)
#define I2Cn_IC_INTR_MASK(n)          VOLATILE_ACCESS(I2Cn_BASE(n) + 0x30)
#define I2Cn_IC_RAW_INTR_STAT(n)      VOLATILE_ACCESS(I2Cn_BASE(n) + 0x34)
#define I2Cn_IC_RX_TL(n)              VOLATILE_ACCESS(I2Cn_BASE(n) + 0x38)
#define I2Cn_IC_TX_TL(n)              VOLATILE_ACCESS(I2Cn_BASE(n) + 0x3C)
#define I2Cn_IC_CLR_INTR(n)           VOLATILE_ACCESS(I2Cn_BASE(n) + 0x40)
#define I2Cn_IC_CLR_RX_UNDER(n)       VOLATILE_ACCESS(I2Cn_BASE(n) + 0x44)
#define I2Cn_IC_CLR_RX_OVER(n)        VOLATILE_ACCESS(I2Cn_BASE(n) + 0x48)
#define I2Cn_IC_CLR_TX_OVER(n)        VOLATILE_ACCESS(I2Cn_BASE(n) + 0x4C)
#define I2Cn_IC_CLR_RD_REQ(n)         VOLATILE_ACCESS(I2Cn_BASE(n) + 0x50)
#define I2Cn_IC_CLR_TX_ABRT(n)        VOLATILE_ACCESS(I2Cn_BASE(n) + 0x54)
#define I2Cn_IC_CLR_RX_DONE(n)        VOLATILE_ACCESS(I2Cn_BASE(n) + 0x58)
#define I2Cn_IC_CLR_ACTIVITY(n)       VOLATILE_ACCESS(I2Cn_BASE(n) + 0x5C)
#define I2Cn_IC_CLR_STOP_DET(n)       VOLATILE_ACCESS(I2Cn_BASE(n) + 0x60)
#define I2Cn_IC_CLR_START_DET(n)      VOLATILE_ACCESS(I2Cn_BASE(n) + 0x64)
#define I2Cn_IC_CLR_GEN_CALL(n)       VOLATILE_ACCESS(I2Cn_BASE(n) + 0x68)
#define I2Cn_IC_ENABLE(n)             VOLATILE_ACCESS(I2Cn_BASE(n) + 0x6C)
#define I2Cn_IC_STATUS(n)             VOLATILE_ACCESS(I2Cn_BASE(n) + 0x70)
#define I2Cn_IC_TXFLR(n)              VOLATILE_ACCESS(I2Cn_BASE(n) + 0x74)
#define I2Cn_IC_RXFLR(n)              VOLATILE_ACCESS(I2Cn_BASE(n) + 0x78)
#define I2Cn_IC_SDA_HOLD(n)           VOLATILE_ACCESS(I2Cn_BASE(n) + 0x7C)
#define I2Cn_IC_TX_ABRT_SOURCE(n)     VOLATILE_ACCESS(I2Cn_BASE(n) + 0x80)
#define I2Cn_IC_SLV_DATA_NACK_ONLY(n) VOLATILE_ACCESS(I2Cn_BASE(n) + 0x84)
#define I2Cn_IC_DMA_CR(n)             VOLATILE_ACCESS(I2Cn_BASE(n) + 0x88)
#define I2Cn_IC_DMA_TDLR(n)           VOLATILE_ACCESS(I2Cn_BASE(n) + 0x8C)
#define I2Cn_IC_DMA_RDLR(n)           VOLATILE_ACCESS(I2Cn_BASE(n) + 0x90)
#define I2Cn_IC_SDA_SETUP(n)          VOLATILE_ACCESS(I2Cn_BASE(n) + 0x94)
#define I2Cn_IC_ACK_GENERAL_CALL(n)   VOLATILE_ACCESS(I2Cn_BASE(n) + 0x98)
#define I2Cn_IC_ENABLE_STATUS(n)      VOLATILE_ACCESS(I2Cn_BASE(n) + 0x9C)
#define I2Cn_IC_FS_SPKLEN(n)          VOLATILE_ACCESS(I2Cn_BASE(n) + 0xA0)
#define I2Cn_IC_CLR_RESTART_DET(n)    VOLATILE_ACCESS(I2Cn_BASE(n) + 0xA8)
#define I2Cn_IC_COMP_PARAM_1(n)       VOLATILE_ACCESS(I2Cn_BASE(n) + 0xF4)
#define I2Cn_IC_COMP_VERSION(n)       VOLATILE_ACCESS(I2Cn_BASE(n) + 0xF8)
#define I2Cn_IC_COMP_TYPE(n)          VOLATILE_ACCESS(I2Cn_BASE(n) + 0xFC)

//======================================================
// Timer関連レジスタ定義
//======================================================
#define TIMER_TIMEHW      VOLATILE_ACCESS(TIMER_BASE + 0x00) // 上位32bit書き込み
#define TIMER_TIMELW      VOLATILE_ACCESS(TIMER_BASE + 0x04) // 下位32bit書き込み
#define TIMER_TIMEHR      VOLATILE_ACCESS(TIMER_BASE + 0x08) // 上位32bit読み出し
#define TIMER_TIMELR      VOLATILE_ACCESS(TIMER_BASE + 0x0c) // 下位32bit読み出し

#define TIMER_ALARM(n)    VOLATILE_ACCESS(TIMER_BASE + 0x10 + 0x04 * n) // ALARMn発火時間設定(n:0~3)

#define TIMER_INTR        VOLATILE_ACCESS(TIMER_BASE + 0x34) // 
#define TIMER_INTE        VOLATILE_ACCESS(TIMER_BASE + 0x38) // 割り込み有効化（ALARM発火）

#define WATCHDOG_TICK     VOLATILE_ACCESS(WATCHDOG_BASE + 0x2c) // ウォッチドッグ＆タイマー用ティック設定

//======================================================
// interrput関連レジスタ定義
//======================================================
#define PPB_NVIC_ISER     VOLATILE_ACCESS(PPB_BASE + 0xe100) // 割り込み有効化　ビットnがIRQnに対応
#define PPB_TBLOFF        VOLATILE_ACCESS(PPB_BASE + 0xed08) // ベクターテーブル オフセットアドレス

#endif /* __REGISTER_H__ */