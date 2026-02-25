/**
 * @file   adc_internal.h
 * @brief  ADCコンポーネント・内部公開定義
 */
#ifndef __ADC_INTERNAL_H__
#define __ADC_INTERNAL_H__

//======================================================
// インクルード
//======================================================
#include "adc.h"

//======================================================
// マクロ定義
//======================================================

//======================================================
// 型定義
//======================================================

//======================================================
// グローバル変数・定数extern宣言
//======================================================

//======================================================
// グローバル関数extern宣言
//======================================================
/* init → ops */
extern void adc_set_enable(bool is_enable);
extern void adc_set_fifobuffer_enable(bool is_enable);
extern void adc_set_clock_divider(uint16_t div_int, uint8_t div_frac);
extern void adc_set_roundrobin(uint8_t enable_ch);
extern void adc_set_loop_enable(bool is_enable);

/* ctrl → ops */
extern bool adc_check_is_adc_ready();
extern ADC_conv_mode_t adc_read_conv_mode(ADC_ch_t select_ch);

#endif /* __ADC_INTERNAL_H__ */