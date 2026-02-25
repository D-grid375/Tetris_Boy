/**
 * @file   button.h
 * @brief  ボタンコンポーネント・外部公開定義
 */

#ifndef __BUTTON_H__
#define __BUTTON_H__

//======================================================
// インクルード
//======================================================
#include "gpio.h"

//======================================================
// マクロ定義
//======================================================

//======================================================
// 型定義
//======================================================
/**
 * @brief ボタン設定定義（config用）
 * @details ボタン入力に割り当てるGPIOピン番号を指定する
 *          configでこの定義に対する実パラメータを設定し、初期化関数に渡す事で設定を反映させる
 */
typedef struct
{
    GPIO_num_t assign_gpioPin; /**< ボタン入力に割り当てるGPIOピン番号 */
} BUTTON_config_t;

/**
 * @brief ボタンクラス定義
 * @note インスタンス内にconfigパラメータを保持させている
 *       複数のボタンを使用したい場合、その数だけconfig値を別途用意しインスタンスを生成すること
 */
typedef struct
{
    // 初期化設定用config値
    BUTTON_config_t config;

    // 内部ステート
    bool state_push; /**< 直前までのボタン押下状態 */
} BUTTON_class_t;

//======================================================
// グローバル変数・定数extern宣言
//======================================================

//======================================================
// グローバル関数extern宣言
//======================================================
/* init */
extern BUTTON_class_t BUTTON_initialize_instance(BUTTON_config_t config);

/* ops */
extern bool BUTTON_check_pushed_once(BUTTON_class_t *button);
extern bool BUTTON_check_pushed_state(BUTTON_class_t *button);

#endif /* __BUTTON_H__ */