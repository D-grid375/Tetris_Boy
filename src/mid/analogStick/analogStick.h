/**
 * @file   analogStick.h
 * @brief  アナログスティックコンポーネント・外部公開定義
 */

#ifndef __analogStick_H__
#define __analogStick_H__

//======================================================
// インクルード
//======================================================
#include "adc.h"
#include "typedef.h"

//======================================================
// マクロ定義
//======================================================

//======================================================
// 型定義
//======================================================
/**
 * @brief アナログスティック設定定義（config用）
 * @details スティックのAD値読取に割り当てるADCチャネル及びアプリ層に渡す座標値範囲を設定する
 *          configでこの定義に対する実パラメータを作成し、初期化関数に渡す事で設定を反映させる
 */
typedef struct
{
    ADC_ch_t assign_x_adc_ch; /**< スティックのX軸読取に割り当てるADCチャネル */
    ADC_ch_t assign_y_adc_ch; /**< スティックのY軸読取に割り当てるADCチャネル */
    // ↓取得したX軸・Y軸のAD値は、以下で設定するX軸・Y軸の最大最小範囲に線形変換して返される
    int32_t x_max; /**< X軸の座標値 最大値 */
    int32_t x_min; /**< X軸の座標値 最小値 */
    int32_t y_max; /**< Y軸の座標値 最大値 */
    int32_t y_min; /**< Y軸の座標値 最小値 */
} ANALOGSTICK_config_t;

/**
 * @brief アナログスティッククラス定義
 * @note インスタンス内にconfigパラメータを保持させている
 *       複数のアナログスティックを使用したい場合、その数だけconfig値を別途用意しインスタンスを生成すること

 */
typedef struct
{
    // 初期化設定用config値
    ANALOGSTICK_config_t config;

    // 内部ステート
    int32_t x_ad_value;         /**< X軸のAD値 */
    int32_t y_ad_value;         /**< Y軸のAD値 */
    int32_t x_coordinate_value; /**< X軸の座標値（AD値を線形変換後の数値） */
    int32_t y_coordinate_value; /**< Y軸の座標値（AD値を線形変換後の数値） */

} ANALOGSTICK_class_t;

//======================================================
// グローバル変数・定数extern宣言
//======================================================

//======================================================
// グローバル関数extern宣言
//======================================================
/* init */
extern ANALOGSTICK_class_t ANALOGSTICK_initialize_instance(ANALOGSTICK_config_t config);

/* ops */
extern void ANALOGSTICK_update_coordinate_value(ANALOGSTICK_class_t *instance);

#endif /* __analogStick_H__ */