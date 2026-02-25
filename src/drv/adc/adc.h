/**
 * @file   adc.h
 * @brief  ADCコンポーネント・外部公開定義
 */
#ifndef __ADC_H__
#define __ADC_H__

//======================================================
// インクルード
//======================================================
#include "typedef.h"

//======================================================
// マクロ定義
//======================================================
#define AD_MAX 0xFFF // 分解能12bit 最大値（IC不具合で12bitの分解能は保証されていない）
#define AD_MIN 0x000 // 分解能12bit 最小値（IC不具合で12bitの分解能は保証されていない）

//======================================================
// 型定義
//======================================================
/**
 * @brief ADC入力チャネル
 */
typedef enum
{
    adc_ch0,
    adc_ch1,
    adc_ch2,
    adc_ch3,
    adc_ch4,
} ADC_ch_t;

/**
 * @brief ADC変換モード
 */
typedef enum
{
    single_conv, /**< 単発変換（AD変換の度に返還要求APIを叩く） */
    loop_conv,   /**< ループ変換（ラウンドロビン） */
} ADC_conv_mode_t;

/**
 * @brief ADC受信データ読み出しモード
 */
typedef enum
{
    single_register_read, /**< 単一レジスタ読み出し */
    fifobuffer_read,      /**< FIFOバッファ読み出し */
} ADC_rx_read_mode_t;

/**
 * @brief ADCチャネル設定定義（config用）
 * @details ループ変換有効時の設定。シングル変換時は都度入力チャネルを指定する。
 *          configでこの定義に対する実パラメータを設定し、初期化関数に渡す事で設定を反映させる
 */
typedef union
{
    struct
    {
        bool enable_adc0 : 1; /**< gpio26 */
        bool enable_adc1 : 1; /**< gpio27 */
        bool enable_adc2 : 1; /**< gpio28 */
        bool enable_adc3 : 1; /**< 使用不可：VSYS/3測定用 */
        bool enable_adc4 : 1; /**< 使用不可：チップ温度測定用 */
        unsigned int reserve : 3;
    } bits;

    uint8_t word;
} ADC_ch_config_t;

/**
 * @brief ADCパラメータ設定定義（config用）
 * @details AD変換の実行モード等に関する設定
 *          configでこの定義に対する実パラメータを設定し、初期化関数に渡す事で設定を反映させる
 */
typedef struct
{
    ADC_conv_mode_t conv_mode;       /**< シングルorループ */
    ADC_rx_read_mode_t rx_read_mode; /**< FIFO利用or単一レジスタ読み出し */
    uint16_t div_int;                /**< 変換周期設定（整数部分。ループ変換有効の場合の設定） */
    uint8_t div_frac;                /**< 変換周期設定（分数部分。ループ変換有効の場合の設定） */

} ADC_parameter_config_t;

//======================================================
// グローバル変数・定数extern宣言
//======================================================

//======================================================
// グローバル関数extern宣言
//======================================================
/* init */
extern void ADC_initialize(ADC_ch_config_t ch_config, ADC_parameter_config_t parameter_config);

/* ctrl */
extern uint16_t ADC_get_oneshot_ad(ADC_ch_t select_ch);

/* ops */
extern void ADC_select_ainsel(ADC_ch_t select_ch);
extern void ADC_start_oneshot();
extern uint16_t ADC_read_result();
extern uint16_t ADC_read_fifobuffer();
extern uint8_t ADC_count_fifobuffer_result();
extern bool ADC_check_fifobuffer_overflow();

#endif /* __ADC_H__ */