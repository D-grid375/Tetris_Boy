/**
 * @file   adc_init.c
 * @brief  ADCコンポーネント・初期化実装
 */

//======================================================
// インクルード
//======================================================
#include "adc.h"
#include "adc_internal.h"
#include "timer.h"

//======================================================
// マクロ定義
//======================================================
#define NUMBER_OF_ADC_CH 5  // ADCチャネル数（0-4ch）

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
 * @brief  ADCの初期化処理
 * @details ADCの有効化ch及び設定値を受け取り、そのchに対し初期化を行う
 *          ADCのモードとして単発・ループの2種類を定義している
 *          ・単発変換モードの場合：初期化設定のみ行う。実際にAD変換する際は変換実行用APIをコールする
 *          ・ループ変換モードの場合：初期化の後、即座に周期的変換を開始する
 *          また、AD変換結果の読出しを単一レジスタ・FIFOバッファのどちらから行うかを選択可能
 * @param  ch_config ADC有効ch選択
 * @param  parameter_config ADC設定パラメータ（変換モード＆分周値）
 * @return なし
 * @todo   アプリケーション実行中のモード切り替えには未対応。必要になった場合は設定処理は内部関数として切り出し、初期化関数からはそれをコールするよう変える
 * @note   ループ変換の場合はFIFO利用が前提となっていることに注意
 */
void ADC_initialize(ADC_ch_config_t ch_config, ADC_parameter_config_t parameter_config)
{
    uint8_t is_set_first_conv_ch = false;
    ADC_ch_t first_conv_ch;
    uint8_t enabled_ch_count = 0;

    // ループ変換の場合の初期化処理
    if (loop_conv == parameter_config.conv_mode)
    {
        // 有効化チャネル数のカウントと初回変換チャネルの探索
        for (ADC_ch_t ch = adc_ch0; ch < NUMBER_OF_ADC_CH; ch++)
        {
            if ((ch_config.word >> ch) & 1U) // adc_nが有効化対象の場合
            {
                enabled_ch_count++;

                // 有効化対象chの内、最小のchを抽出
                if (!is_set_first_conv_ch)
                {
                    first_conv_ch = ch;
                    is_set_first_conv_ch = true;
                }
            }
        }

        // 複数チャネルでループ変換する場合：ラウンドロビン設定を行う
        if (enabled_ch_count >= 2)
        {
            adc_set_roundrobin(ch_config.word);
        }

        // AD変換周期設定
        adc_set_clock_divider(parameter_config.div_int, parameter_config.div_frac);
    }

    // FIFO利用の場合 → FIFO初期化処理
    if (fifobuffer_read == parameter_config.rx_read_mode)
    {
        adc_set_fifobuffer_enable(ENABLE);

        // FIFOバッファクリア
        while (ADC_count_fifobuffer_result())
        {
            (void)ADC_read_fifobuffer();
        }
    }

    // ADCの起動
    adc_set_enable(ENABLE); // ADC利用有効化
    TIMER_wait_ms(50);      // ADC利用有効待ち（時間は適当）

    // ループ変換の場合、即座に変換開始
    if (loop_conv == parameter_config.conv_mode)
    {
        adc_set_loop_enable(ENABLE);

        // 有効化されているchに関わらず、最初に変換されるのはch0で固定
        // そのため、0chが有効化されていない場合の初回変換値はゴミ値になるので捨てる
        if (adc_ch0 != first_conv_ch)
        {
            while (1 > ADC_count_fifobuffer_result())
            {
                // 初回変換待ち
            }

            (void)ADC_read_fifobuffer(); // FIFO使用前提で初期値を読み捨て
        }
    }
}

//======================================================
// 内部関数定義
//======================================================