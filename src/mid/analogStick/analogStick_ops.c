/**
 * @file   analogStick_ops.c
 * @brief  アナログスティックコンポーネント・汎用制御実装
 */

//======================================================
// インクルード
//======================================================
#include "analogStick.h"
#include "analogStick_internal.h"
#include "adc.h"
#include "timer.h"

//======================================================
// マクロ定義
//======================================================
// 使用するアナログスティックの定義（殆ど切り替えないのでconfigではなくマクロで定義）
#define ADA2765
#undef HW504

// X軸Y軸AD値・最大最小定義（X軸は右が正、Y軸は上が正になるよう定義）
#ifdef ADA2765 // X軸・Y軸 どちらも反転
#define AD_X_MAX AD_MIN
#define AD_X_MIN AD_MAX
#define AD_Y_MAX AD_MIN
#define AD_Y_MIN AD_MAX
#elif defined HW504 // X軸・Y軸 反転無し
#define AD_X_MAX AD_MAX
#define AD_X_MIN AD_MIN
#define AD_Y_MAX AD_MAX
#define AD_Y_MIN AD_MIN
#endif

//======================================================
// 型定義
//======================================================

//======================================================
// 変数・定数
//======================================================

//======================================================
// プロトタイプ宣言
//======================================================
static void get_ad_value(ANALOGSTICK_class_t *instance);
static void convert_ad_to_coordinate(ANALOGSTICK_class_t *instance);

//======================================================
// 公開関数定義
//======================================================
/**
 * @brief アナログスティック座標値更新
 * @details X軸とY軸の座標値を取得する
 *          AD値変換（単発）＆AD値の読出し → AD値の座標値への変換を行いインスタンスに格納する
 *          変換後の座標値の範囲（最大値・最小値）はconfig値で指定の上、初期化関数でインスタンスに格納されている
 * @param instance アナログスティックのインスタンス
 * @return なし
 */
void ANALOGSTICK_update_coordinate_value(ANALOGSTICK_class_t *instance)
{
    get_ad_value(instance);
    convert_ad_to_coordinate(instance);
}

//======================================================
// 内部関数定義
//======================================================
/**
 * @brief AD値取得
 * @param instance アナログスティックのインスタンス
 * @return なし
 * @note 単発変換かつ単レジスタ読み出しのみを現状サポートする
 *       別モード追加時は関数分離し、モード選択をconfigへ追加する
 */
static void get_ad_value(ANALOGSTICK_class_t *instance)
{
    instance->x_ad_value = ADC_get_oneshot_ad(instance->config.assign_x_adc_ch);
    instance->y_ad_value = ADC_get_oneshot_ad(instance->config.assign_y_adc_ch);
}

/**
 * @brief AD値から座標値への変換
 * @param instance アナログスティックのインスタンス
 * @return なし
 * @details X軸とY軸のをAD値から座標値へ線形変換する
 */
static void convert_ad_to_coordinate(ANALOGSTICK_class_t *instance)
{
    int32_t shift_value = 2 ^ 19;

    instance->x_coordinate_value = (int32_t)((int64_t)(instance->x_ad_value - AD_X_MIN) * (instance->config.x_max - instance->config.x_min) * shift_value / (AD_X_MAX - AD_X_MIN) / shift_value + instance->config.x_min);
    instance->y_coordinate_value = (int32_t)((int64_t)(instance->y_ad_value - AD_Y_MIN) * (instance->config.y_max - instance->config.y_min) * shift_value / (AD_Y_MAX - AD_Y_MIN) / shift_value + instance->config.y_min);
}