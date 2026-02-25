/**
 * @file   tetris_input_ctrl.c
 * @brief  tetris・入力制御実装
 */

//======================================================
// インクルード
//======================================================
#include "tetris.h"
#include "tetris_internal.h"
#include "typedef.h"
#include "button.h"
#include "analogStick.h"

//======================================================
// マクロ定義
//======================================================
// 入力ありとみなすアナログスティックの入力閾値-100~100設定の前提
#define AD_INPUT_R_TH 50
#define AD_INPUT_L_TH -50
#define AD_INPUT_U_TH -50
#define AD_INPUT_D_TH 50

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
 * @brief ゲーム開始入力受付
 * @param input_handler 入力ハンドラ
 * @param input_state_ptr 入力状態格納先
 * @return なし
 * @details ゲーム開始受付に使用するコントロールボタン1、コントロールボタン2の2つのみ入力取得
 */
void tetris_receive_game_start_input(TETRIS_input_parameter_t *input_handler, tetris_input_state_t *input_state_ptr)
{
    input_state_ptr->is_input_control_button2 = BUTTON_check_pushed_once(&input_handler->control_button2);
    input_state_ptr->is_input_control_button1 = BUTTON_check_pushed_once(&input_handler->control_button1);
}

/**
 * @brief ゲーム実行中入力状態更新
 * @param input_handler 入力ハンドラ
 * @param input_state_ptr 入力状態格納先
 * @return なし
 * @details ゲームプレイに利用するアナログスティックの上下左右入力と右回転ボタン,左回転ボタンの入力を取得する
 */
void tetris_input_ctrl_in_game(TETRIS_input_parameter_t *input_handler, tetris_input_state_t *input_state_ptr)
{
    // スティックAD入力更新
    ANALOGSTICK_update_coordinate_value(&input_handler->analog_stick);

    // アナログスティック座標値が一定以上 = 一定以上スティックを倒した場合に上下左右（U,D,L,R）入力ありと判定する
    input_state_ptr->is_input_R = (AD_INPUT_R_TH < input_handler->analog_stick.x_coordinate_value) ? true : false;
    input_state_ptr->is_input_L = (input_handler->analog_stick.x_coordinate_value < AD_INPUT_L_TH) ? true : false;
    input_state_ptr->is_input_U = (input_handler->analog_stick.y_coordinate_value < AD_INPUT_U_TH) ? true : false;
    input_state_ptr->is_input_D = (AD_INPUT_D_TH < input_handler->analog_stick.y_coordinate_value) ? true : false;

    // ボタン入力更新：押した直後の1周期でのみHigh
    input_state_ptr->is_input_turnR_button = BUTTON_check_pushed_once(&input_handler->turnR_button);
    input_state_ptr->is_input_turnL_button = BUTTON_check_pushed_once(&input_handler->turnL_button);
}

/**
 * @brief ゲームリスタート入力受付
 * @param input_handler 入力ハンドラ
 * @param input_state_ptr 入力状態格納先
 * @return なし
 * @details ゲームリスタート受付に使用するコントロールボタン1、コントロールボタン2の2つのみ入力取得

 */
void tetris_receive_game_restart_input(TETRIS_input_parameter_t *input_handler, tetris_input_state_t *input_state_ptr)
{
    input_state_ptr->is_input_control_button2 = BUTTON_check_pushed_once(&input_handler->control_button2);
    input_state_ptr->is_input_control_button1 = BUTTON_check_pushed_once(&input_handler->control_button1);
}

/**
 * @brief ゲーム入力状態初期化
 * @param input_state_ptr 入力状態格納先
 * @return なし
 * @details ゲーム開始時＆ゲームオーバー後のゲームリスタート時に毎回呼ばれる
 */
void tetris_initialize_input_ctrl(tetris_input_state_t *input_state_ptr)
{
    // 入力状態初期化
    input_state_ptr->is_input_R = false;
    input_state_ptr->is_input_L = false;
    input_state_ptr->is_input_U = false;
    input_state_ptr->is_input_D = false;
    input_state_ptr->is_input_turnR_button = false;
    input_state_ptr->is_input_turnL_button = false;
    input_state_ptr->is_input_control_button2 = false;
    input_state_ptr->is_input_control_button1 = false;
}

//======================================================
// 内部関数定義
//======================================================