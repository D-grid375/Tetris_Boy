/**
 * @file   address.h
 * @brief  アドレス定義
 */

#ifndef __ADDRESS_H__
#define __ADDRESS_H__

//======================================================
// インクルード
//======================================================

//======================================================
// 各種ベースアドレス定義
//======================================================
#define ROM_VECTOR_TABLE_BASE  0x00000000 // ベクタテーブル初期値　ROMへの配置なので、割り込み用に書き換えるためにはRAMへのコピー必須
#define CLOCKS_BASE            0x40008000
#define RESETS_BASE            0x4000C000
#define IO_BANK0_BASE          0x40014000
#define PADS_BANK0_BASE        0x4001c000
#define XOSC_BASE              0x40024000
#define PLL_SYS_BASE           0x40028000
#define I2Cn_BASE(n)           0x40044000 + 0x4000 * n
#define ADC_BASE               0x4004c000
#define TIMER_BASE             0x40054000
#define WATCHDOG_BASE          0x40058000
#define PPB_BASE               0xe0000000
#define SIO_BASE               0xD0000000

#endif /* __ADDRESS_H__ */