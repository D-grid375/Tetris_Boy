/**
 * @file   bitmap_lib.c
 * @brief  BITMAP汎用ライブラリ実装
 * @details 128×128のビットマップを処理するためのライブラリ
 *          [128][2]の2次元配列形式を想定
 * @todo    128×128形式にしか対応していない。可変サイズに対応した方がベター
 *          ※現状128×128以外を扱う予定は無いため、必須ではない
 */

//======================================================
// インクルード
//======================================================
#include "bitmap_lib.h"

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
 * @brief ビットマップ指定座標のビット値取得
 * @param bitmap 対象ビットマップ
 * @param row 行インデックス
 * @param column 列インデックス
 * @return 指定座標のビット値
 */
bool BITMAP_read(uint64_t bitmap[128][2], uint8_t row, uint8_t column)
{
    uint8_t column_index = column / 64;
    uint8_t column_target = column % 64;
    uint64_t target_64bit = bitmap[row][column_index];

    return ((target_64bit >> (63 - column_target)) & (0b1));
}

/**
 * @brief ビットマップ指定座標へのビット値書き込み
 * @param bitmap 対象ビットマップ
 * @param row 行インデックス
 * @param column 列インデックス
 * @param level 書き込みビット値
 * @return なし
 */
void BITMAP_write(uint64_t bitmap[128][2], uint8_t row, uint8_t column, bool level)
{
    uint8_t column_index = column / 64;  // 0~1
    uint8_t column_target = column % 64; // 0~63

    bitmap[row][column_index] |= ((uint64_t)level << (63 - column_target));
}

/**
 * @brief ビットマップ上下左右シフト処理
 * @param bitmap 対象ビットマップ
 * @param shift_column_level 列方向シフト量
 * @param shift_row_level 行方向シフト量
 * @return なし
 * @details 正値は右方向および下方向、負値は左方向および上方向にシフトする
 */
void BITMAP_shift(uint64_t bitmap[128][2], int64_t shift_column_level, int64_t shift_row_level)
{
    if (0 < shift_column_level && shift_column_level < 128)
    {
        BITMAP_readshift(bitmap, shift_column_level);
    }
    else if (-128 < shift_column_level && shift_column_level < 0)
    {
        BITMAP_lshift(bitmap, -shift_column_level);
    }

    if (0 < shift_row_level && shift_row_level < 128)
    {
        BITMAP_dshift(bitmap, shift_row_level);
    }
    else if (-128 < shift_row_level && shift_row_level < 0)
    {
        BITMAP_ushift(bitmap, -shift_row_level);
    }
}

/**
 * @brief ビットマップ下方向シフト処理
 * @param bitmap 対象ビットマップ
 * @param shift_level 下方向シフト量
 * @return なし
 */
void BITMAP_dshift(uint64_t bitmap[128][2], uint8_t shift_level)
{
    if (shift_level == 0 || shift_level >= 128)
        return;

    for (int row = 127; row >= shift_level; row--)
    {
        bitmap[row][0] = bitmap[row - shift_level][0];
        bitmap[row][1] = bitmap[row - shift_level][1];
    }
    for (int row = shift_level - 1; row >= 0; row--)
    {
        bitmap[row][0] = 0;
        bitmap[row][1] = 0;
    }
}

/**
 * @brief ビットマップ上方向シフト処理
 * @param bitmap 対象ビットマップ
 * @param shift_level 上方向シフト量
 * @return なし
 */
void BITMAP_ushift(uint64_t bitmap[128][2], uint8_t shift_level)
{
    if (shift_level == 0 || shift_level >= 128)
        return;

    for (int row = 0; row <= 127 - shift_level; row++)
    {
        bitmap[row][0] = bitmap[row + shift_level][0];
        bitmap[row][1] = bitmap[row + shift_level][1];
    }
    for (int row = 128 - shift_level; row < 128; row++)
    {
        bitmap[row][0] = 0;
        bitmap[row][1] = 0;
    }
}

/**
 * @brief ビットマップ左方向シフト処理
 * @param bitmap 対象ビットマップ
 * @param shift_level 左方向シフト量
 * @return なし
 */
void BITMAP_lshift(uint64_t bitmap[128][2], uint8_t shift_level)
{
    if (shift_level == 0 || shift_level >= 128)
        return;

    for (int row = 0; row < 128; row++)
    {
        uint64_t high = bitmap[row][0];
        uint64_t low = bitmap[row][1];

        if (shift_level < 64)
        {
            bitmap[row][0] = (high << shift_level) | (low >> (64 - shift_level));
            bitmap[row][1] = (low << shift_level);
        }
        else
        {
            bitmap[row][0] = (low << (shift_level - 64));
            bitmap[row][1] = 0;
        }
    }
}

/**
 * @brief ビットマップ右方向シフト処理
 * @param bitmap 対象ビットマップ
 * @param shift_level 右方向シフト量
 * @return なし
 */
void BITMAP_readshift(uint64_t bitmap[128][2], uint8_t shift_level)
{
    if (shift_level == 0 || shift_level >= 128)
        return;

    for (int row = 0; row < 128; row++)
    {
        uint64_t high = bitmap[row][0];
        uint64_t low = bitmap[row][1];

        if (shift_level < 64)
        {
            bitmap[row][1] = (low >> shift_level) | (high << (64 - shift_level));
            bitmap[row][0] = (high >> shift_level);
        }
        else
        {
            bitmap[row][1] = (high >> (shift_level - 64));
            bitmap[row][0] = 0;
        }
    }
}

/**
 * @brief ビットマップ水平線描画
 * @param bitmap 対象ビットマップ
 * @param start_column 開始列インデックス
 * @param start_row 開始行インデックス
 * @param length 描画長
 * @return なし
 */
void BITMAP_horizontal_line(uint64_t bitmap[128][2], uint8_t start_column, uint8_t start_row, uint8_t length)
{
    // 例外処理
    if ((length == 0 || 128 < length) ||
        (127 < start_column) ||
        (127 < start_row) ||
        (128 - length < start_column))
    {
        return;
    }

    // 直線描画
    for (uint8_t c = 0; c < length; c++)
    {
        BITMAP_write(bitmap, start_row, start_column + c, HIGH);
    }
}

/**
 * @brief ビットマップ垂直線描画
 * @param bitmap 対象ビットマップ
 * @param start_column 開始列インデックス
 * @param start_row 開始行インデックス
 * @param length 描画長
 * @return なし
 */
void BITMAP_vertical_line(uint64_t bitmap[128][2], uint8_t start_column, uint8_t start_row, uint8_t length)
{
    // 例外処理
    if ((length == 0 || 128 < length) ||
        (127 < start_column) ||
        (127 < start_row) ||
        (128 - length < start_row))
    {
        return;
    }

    // 直線描画
    for (uint8_t r = 0; r < length; r++)
    {
        BITMAP_write(bitmap, start_row + r, start_column, HIGH);
    }
}

/**
 * @brief ビットマップ四角形描画
 * @param bitmap 対象ビットマップ
 * @param start_column 開始列インデックス
 * @param start_row 開始行インデックス
 * @param length_column 列方向長
 * @param length_row 行方向長
 * @return なし
 */
void BITMAP_square(uint64_t bitmap[128][2], uint8_t start_column, uint8_t start_row, uint8_t length_column, uint8_t length_row)
{
    // 例外処理
    if (((length_column == 0 || 128 < length_column) ||
         (127 < start_column) ||
         (127 < start_row) ||
         (128 - length_column < start_column)) ||
        ((length_row == 0 || 128 < length_row) ||
         (127 < start_column) ||
         (127 < start_row) ||
         (128 - length_row < start_row)))
    {
        return;
    }

    // 四角形描画
    for (uint8_t r = 0; r < length_row; r++)
    {
        for (uint8_t c = 0; c < length_column; c++)
        {
            BITMAP_write(bitmap, start_row + r, start_column + c, HIGH);
        }
    }
}

/**
 * @brief ビットマップOR演算
 * @param bitmap_dst 演算結果格納先ビットマップ
 * @param bitmap_operand OR演算対象ビットマップ
 * @return なし
 */
void BITMAP_or(uint64_t bitmap_dst[128][2], const uint64_t bitmap_operand[128][2])
{
    for (int row = 0; row < 128; row++)
    {
        bitmap_dst[row][0] = bitmap_dst[row][0] | bitmap_operand[row][0];
        bitmap_dst[row][1] = bitmap_dst[row][1] | bitmap_operand[row][1];
    }
}

/**
 * @brief シフト後ビットマップOR演算
 * @details bitmap_operandを指定シフト量だけシフトした後、bitmapにOR演算する
 * @param bitmap_dst 演算結果格納先ビットマップ
 * @param bitmap_operand OR演算対象ビットマップ
 * @param shift_column_level 列方向シフト量
 * @param shift_row_level 行方向シフト量
 * @return なし
 */
void BITMAP_or_with_shift(uint64_t bitmap_dst[128][2], const uint64_t bitmap_operand[128][2], int64_t shift_column_level, int64_t shift_row_level)
{
    bitmap_128_t temp_bitmap = {0};
    BITMAP_copy(temp_bitmap, bitmap_operand);
    BITMAP_shift(temp_bitmap, shift_column_level, shift_row_level);
    BITMAP_or(bitmap_dst, temp_bitmap);
}

/**
 * @brief ビットマップXOR演算
 * @param bitmap_dst 演算結果格納先ビットマップ
 * @param bitmap_operand XOR演算対象ビットマップ
 * @return なし
 */
void BITMAP_xor(uint64_t bitmap_dst[128][2], const uint64_t bitmap_operand[128][2])
{
    for (int row = 0; row < 128; row++)
    {
        for (int column = 0; column < 2; column++)
        {
            bitmap_dst[row][column] ^= bitmap_operand[row][column];
        }
    }
}

/**
 * @brief ビットマップAND演算
 * @param bitmap_dst 演算結果格納先ビットマップ
 * @param bitmap_operand AND演算対象ビットマップ
 * @return なし
 */
void BITMAP_and(uint64_t bitmap_dst[128][2], const uint64_t bitmap_operand[128][2])
{
    for (int row = 0; row < 128; row++)
    {
        for (int column = 0; column < 2; column++)
        {
            bitmap_dst[row][column] &= bitmap_operand[row][column];
        }
    }
}

/**
 * @brief ビットマップ差集合演算
 * @param bitmap_dst 演算結果格納先ビットマップ
 * @param bitmap_operand 除外対象ビットマップ
 * @return なし
 */
void BITMAP_not(uint64_t bitmap_dst[128][2], const uint64_t bitmap_operand[128][2])
{
    for (int row = 0; row < 128; row++)
    {
        for (int column = 0; column < 2; column++)
        {
            bitmap_dst[row][column] &= ~bitmap_operand[row][column];
        }
    }
}

/**
 * @brief ビットマップ重なり判定
 * @param bitmap1 判定対象ビットマップ1
 * @param bitmap2 判定対象ビットマップ2
 * @return 重なり判定結果（true: 重なりあり, false: 重なりなし）
 */
bool BITMAP_check_overlap(const uint64_t bitmap1[128][2], const uint64_t bitmap2[128][2])
{
    for (int row = 0; row < 128; row++)
    {
        if ((bitmap1[row][0] & bitmap2[row][0]) != 0 || (bitmap1[row][1] & bitmap2[row][1]) != 0)
        {
            return true; // 一致するビットが1つでもあれば終了
        }
    }
    return false; // 最後まで見て一致ビットがなければfalse
}

/**
 * @brief ビットマップ複製
 * @param bitmap_dst コピー先ビットマップ
 * @param bitmap_src コピー元ビットマップ
 * @return なし
 */
void BITMAP_copy(uint64_t bitmap_dst[128][2], const uint64_t bitmap_src[128][2])
{
    for (int row = 0; row < 128; row++)
    {
        bitmap_dst[row][0] = bitmap_src[row][0];
        bitmap_dst[row][1] = bitmap_src[row][1];
    }
}

/**
 * @brief ビットマップ部分領域抽出
 * @param bitmap_dst 抽出結果格納先ビットマップ
 * @param bitmap_src 抽出元ビットマップ
 * @param start_column 開始列インデックス
 * @param end_column 終了列インデックス
 * @param start_row 開始行インデックス
 * @param end_row 終了行インデックス
 * @return なし
 */
void BITMAP_extract(uint64_t bitmap_dst[128][2], const uint64_t bitmap_src[128][2], uint8_t start_column, uint8_t end_column, uint8_t start_row, uint8_t end_row)
{
    // 範囲チェック
    if (start_column > end_column || start_row > end_row ||
        end_column >= 128 || end_row >= 128)
    {
        return;
    }

    // 幅と高さを計算
    uint8_t width = end_column - start_column + 1;
    uint8_t height = end_row - start_row + 1;

    for (uint8_t row = 0; row < height; row++)
    {
        for (uint8_t column = 0; column < width; column++)
        {
            // 元の座標
            uint8_t src_column = start_column + column;
            uint8_t src_row = start_row + row;

            // bitmap_src からビットを取得
            uint8_t col_idx = src_column / 64;
            uint8_t bit_pos = 63 - (src_column % 64);
            uint8_t bit = (bitmap_src[src_row][col_idx] >> bit_pos) & 1;

            // bitmap_dst にビットを書き込み（左上に詰める）
            uint8_t dst_col_idx = column / 64;
            uint8_t dst_bit_pos = 63 - (column % 64);
            if (bit)
            {
                bitmap_dst[row][dst_col_idx] |= ((uint64_t)1 << dst_bit_pos);
            }
        }
    }
}

/**
 * @brief ビットマップ拡大描画
 * @param bitmap_dst 拡大結果格納先ビットマップ
 * @param bitmap_src 拡大元ビットマップ
 * @param scale_factor 拡大倍率
 * @return なし
 * @details 拡大後座標が範囲外となるビットは描画対象外とする
 */
void BITMAP_enlarge(uint64_t bitmap_dst[128][2], const uint64_t bitmap_src[128][2], uint8_t scale_factor)
{
    if (scale_factor == 0 || scale_factor > 128)
    {
        return;
    }

    for (uint8_t row = 0; row < 128; row++)
    {
        for (uint8_t column = 0; column < 128; column++)
        {
            uint8_t col_idx = column / 64;
            uint8_t bit_pos = 63 - (column % 64);
            uint8_t bit = (bitmap_src[row][col_idx] >> bit_pos) & 1;

            if (bit)
            {
                // 拡大後の (column, row) 位置を基準に scale_factor column scale_factor の領域を塗る
                uint16_t base_column = column * scale_factor;
                uint16_t base_row = row * scale_factor;

                // 範囲外書き込みを防ぐ
                if (base_column >= 128 || base_row >= 128)
                    continue;

                for (uint8_t drow = 0; drow < scale_factor; drow++)
                {
                    for (uint8_t dcolumn = 0; dcolumn < scale_factor; dcolumn++)
                    {
                        uint16_t dst_column = base_column + dcolumn;
                        uint16_t dst_row = base_row + drow;

                        if (dst_column >= 128 || dst_row >= 128)
                            continue;

                        uint8_t dst_col_idx = dst_column / 64;
                        uint8_t dst_bit_pos = 63 - (dst_column % 64);
                        bitmap_dst[dst_row][dst_col_idx] |= ((uint64_t)1 << dst_bit_pos);
                    }
                }
            }
        }
    }
}