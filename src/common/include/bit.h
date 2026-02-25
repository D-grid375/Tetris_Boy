/**
 * @file   bit.h
 * @brief  BIT操作用汎用マクロ定義
 */

#ifndef __COMMON_H__
#define __COMMON_H__

//======================================================
// インクルード
//======================================================

//======================================================
// 汎用マクロ定義
//======================================================
// ビットマスク定義
#define MASK_nBIT(n) ((1ULL << (n)) - 1ULL)

#define MASK_1BIT MASK_nBIT(1)
#define MASK_2BIT MASK_nBIT(2)
#define MASK_3BIT MASK_nBIT(3)
#define MASK_4BIT MASK_nBIT(4)
#define MASK_5BIT MASK_nBIT(5)
#define MASK_6BIT MASK_nBIT(6)
#define MASK_7BIT MASK_nBIT(7)
#define MASK_8BIT MASK_nBIT(8)
#define MASK_9BIT MASK_nBIT(9)
#define MASK_10BIT MASK_nBIT(10)
#define MASK_11BIT MASK_nBIT(11)
#define MASK_12BIT MASK_nBIT(12)
#define MASK_13BIT MASK_nBIT(13)
#define MASK_14BIT MASK_nBIT(14)
#define MASK_15BIT MASK_nBIT(15)
#define MASK_16BIT MASK_nBIT(16)
#define MASK_17BIT MASK_nBIT(17)
#define MASK_18BIT MASK_nBIT(18)
#define MASK_19BIT MASK_nBIT(19)
#define MASK_20BIT MASK_nBIT(20)
#define MASK_21BIT MASK_nBIT(21)
#define MASK_22BIT MASK_nBIT(22)
#define MASK_23BIT MASK_nBIT(23)
#define MASK_24BIT MASK_nBIT(24)
#define MASK_25BIT MASK_nBIT(25)
#define MASK_26BIT MASK_nBIT(26)
#define MASK_27BIT MASK_nBIT(27)
#define MASK_28BIT MASK_nBIT(28)
#define MASK_29BIT MASK_nBIT(29)
#define MASK_30BIT MASK_nBIT(30)
#define MASK_31BIT MASK_nBIT(31)
#define MASK_32BIT MASK_nBIT(32)

#endif /* __COMMON_H__ */