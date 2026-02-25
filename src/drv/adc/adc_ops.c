/**
 * @file   adc_ops.c
 * @brief  ADCコンポーネント・レジスタ操作実装
 */

//======================================================
// インクルード
//======================================================
#include "adc.h"
#include "adc_internal.h"
#include "typedef.h"
#include "register.h"
#include "bit.h"

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
 * @brief  ADC入力チャネルの選択
 * @details 変換対象chを選択する。単発変換開始前にコール必要
 * @param  select_ch ADCチャネル (0-4)
 * @return なし
 */
void ADC_select_ainsel(ADC_ch_t select_ch)
{
    ADC_CS = (ADC_CS & ~(MASK_3BIT << 12)) | ((select_ch & MASK_3BIT) << 12);
}

/**
 * @brief  ADC単発変換の開始
 * @details 単発変換開始。セルフクリア&ループ変換有効時は無視される
 * @return なし
 */
void ADC_start_oneshot()
{
    ADC_CS = ADC_CS | (1 << 2);
}

/**
 * @brief  ADC変換結果の読み出し
 * @return 12bit ADC変換結果 (0-4095)
 */
uint16_t ADC_read_result()
{
    return (ADC_RESULT & MASK_12BIT);
}

/**
 * @brief  ADC FIFOバッファからの読み出し
 * @return 12bit ADC変換結果 (0-4095)
 */
uint16_t ADC_read_fifobuffer()
{
    return (ADC_FIFO & MASK_12BIT);
}

/**
 * @brief  ADC FIFO内の有効データ数の取得
 * @return FIFO内のサンプル数 (0-15)
 */
uint8_t ADC_count_fifobuffer_result()
{
    return ((ADC_FCS >> 16) & MASK_4BIT);
}

/**
 * @brief  ADC FIFOオーバーフロー状態の確認
 * @return true：オーバーフロー発生、false：オーバーフロー未発生
 */
bool ADC_check_fifobuffer_overflow()
{
    return ((ADC_FCS >> 11) & MASK_1BIT);
}

/**
 * @brief  ADC変換完了状態の確認
 * @return true：変換完了、false：変換中
 */
bool adc_check_is_adc_ready()
{
    return ((ADC_CS >> 8) & MASK_1BIT);
}

/**
 * @brief  ADCの起動/停止を設定
 * @param  is_enable true：有効、false：無効
 * @return なし
 * @note   ADCが起動完了するまでWaitが必要（起動するまではAD変換実行できない）
 */
void adc_set_enable(bool is_enable)
{
    ADC_CS = (ADC_CS & ~(MASK_1BIT << 0)) | (is_enable << 0);
}

/**
 * @brief  ADC FIFOバッファ機能の有効/無効を設定
 * @param  is_enable true：有効、false：無効
 * @return なし
 */
void adc_set_fifobuffer_enable(bool is_enable)
{
    ADC_FCS = (ADC_FCS & ~(MASK_1BIT << 0)) | (is_enable << 0);
}

/**
 * @brief  ADC クロック分周値の設定
 * @details ADC をループ変換で使用する場合の変換周期を設定。
 *          div_int+1 サイクル毎に変換がトリガーされる
 * @param  div_int   分周値（整数部分）
 * @param  div_frac  分周値（分数部分）
 * @return なし
 * @note   データシート上の推奨値は div_int ≥ 96
 */
void adc_set_clock_divider(uint16_t div_int, uint8_t div_frac)
{
    ADC_DIV = (ADC_DIV & ~(MASK_8BIT << 0)) | (div_frac << 0);
    ADC_DIV = (ADC_DIV & ~(MASK_16BIT << 8)) | (div_int << 8);
}

/**
 * @brief  ADC ラウンドロビンモードの設定
 * @param  enable_ch_bit ビットマスク（各ビットが ch0-4 に対応、1=有効）
 * @return なし
 * @note   デフォルトは0=ラウンドロビン無し（単一チャネル利用）
 *         最初に変換されるのは必ず 0ch である事に注意
 */
void adc_set_roundrobin(uint8_t enable_ch_bit)
{
    ADC_CS = (ADC_CS & ~(MASK_5BIT << 16)) | ((enable_ch_bit & MASK_5BIT) << 16);
}

/**
 * @brief  設定中のADC変換モード（単発または ループ（ラウンドロビン））の読み出し
 * @param  select_ch ADCチャネル (0-4)
 * @return ADC_conv_mode_t 変換モード (single_conv or loop_conv)
 */
ADC_conv_mode_t adc_read_conv_mode(ADC_ch_t select_ch)
{
    return (ADC_CS >> (16 + select_ch) == 1) ? loop_conv : single_conv;
}

/**
 * @brief  ADC ループ変換の有効/無効を設定
 * @param  is_enable true：有効、false：無効
 * @return なし
 */
void adc_set_loop_enable(bool is_enable)
{
    ADC_CS = (ADC_CS & ~(MASK_1BIT << 3)) | (is_enable << 3);
}

//======================================================
// 内部関数定義
//======================================================