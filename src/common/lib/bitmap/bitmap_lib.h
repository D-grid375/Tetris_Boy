/**
 * @file   bitmap_lib.h
 * @brief  BITMAP汎用ライブラリ・外部公開定義
 */

#ifndef __BITMAP_LIB_H__
#define __BITMAP_LIB_H__

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
 * @brief 128×128ビットマップ型定義
 */
typedef uint64_t bitmap_128_t[128][2];

//======================================================
// グローバル変数・定数extern宣言
//======================================================

//======================================================
// グローバル関数extern宣言
//======================================================
extern bool BITMAP_read(uint64_t bitmap[128][2], uint8_t row, uint8_t column);
extern void BITMAP_write(uint64_t bitmap[128][2], uint8_t row, uint8_t column, bool level);
extern void BITMAP_shift(uint64_t bitmap[128][2], int64_t shift_column_level, int64_t shift_row_level);
extern void BITMAP_dshift(uint64_t bitmap[128][2], uint8_t shift_level);
extern void BITMAP_ushift(uint64_t bitmap[128][2], uint8_t shift_level);
extern void BITMAP_lshift(uint64_t bitmap[128][2], uint8_t shift_level);
extern void BITMAP_readshift(uint64_t bitmap[128][2], uint8_t shift_level);
extern void BITMAP_horizontal_line(uint64_t bitmap[128][2], uint8_t start_column, uint8_t start_row, uint8_t length);
extern void BITMAP_vertical_line(uint64_t bitmap[128][2], uint8_t start_column, uint8_t start_row, uint8_t length);
extern void BITMAP_square(uint64_t bitmap[128][2], uint8_t start_column, uint8_t start_row, uint8_t length_column, uint8_t length_row);
extern void BITMAP_or(uint64_t bitmap_dst[128][2], const uint64_t bitmap_operand[128][2]);
extern void BITMAP_or_with_shift(uint64_t bitmap_dst[128][2], const uint64_t bitmap_operand[128][2], int64_t shift_column_level, int64_t shift_row_level);
extern void BITMAP_xor(uint64_t bitmap_dst[128][2], const uint64_t bitmap_operand[128][2]);
extern void BITMAP_and(uint64_t bitmap_dst[128][2], const uint64_t bitmap_operand[128][2]);
extern void BITMAP_not(uint64_t bitmap_dst[128][2], const uint64_t bitmap_operand[128][2]);
extern bool BITMAP_check_overlap(const uint64_t bitmap1[128][2], const uint64_t bitmap2[128][2]);
extern void BITMAP_copy(uint64_t bitmap_dst[128][2], const uint64_t bitmap_src[128][2]);
extern void BITMAP_extract(uint64_t bitmap_dst[128][2], const uint64_t bitmap_src[128][2], uint8_t start_column, uint8_t end_column, uint8_t start_row, uint8_t end_row);
extern void BITMAP_enlarge(uint64_t bitmap_dst[128][2], const uint64_t bitmap_src[128][2], uint8_t scale_factor);

#endif /* __BITMAP_LIB_H__ */