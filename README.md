# Tetris Boy
![IMG_6336](https://github.com/user-attachments/assets/a5d35288-6ec1-4e75-aa3e-88aabaf0c257)

個人開発した小型ゲーム機の設計解説記事に対応する **実装全体の公開リポジトリ** です。

設計記事 : [【個人開発】Raspi+基板+3Dプリントで小型ゲーム機を作る【電子工作】](https://qiita.com/D-grid/items/9f522fb13760feb94512)

本コードはソフトウェア配布ではなく、設計内容の補足資料として公開しています。
個人による環境構築の上でインストール・動作させる事は可能ですが、導入サポートは行いませんのでご注意下さい。

---

## このリポジトリの位置付け

このコードは以下を目的として公開しています。

- 設計記事の補足資料
- 実装構造の参照
- 学習用コードリーディング素材

再利用・流用・製品利用は想定していません。

---

## ソフトウェア機能概要

個人開発の小型ゲーム機を動作させ、テトリスをプレイできる。
アナログスティック+ボタン4つでプレイする。

---

## フォルダ構成
| フォルダ | 概要 |
|---|---|
|bitmap|ビットマップ用PNG/ビットマップ変換ツール|
|cmake|ビルド構成（参考）|
|src/app|ソースコード：テトリスゲームロジック|
|src/mid|ソースコード：外部デバイス制御（ディスプレイ/アナログスティック/スイッチ）|
|src/drv|ソースコード：マイコンペリフェラル制御|
|src/common|ソースコード：汎用ユーティリティ|

※設計意図は記事を参照

---

## 想定ハードウェア（主要部品）

| 種類 | 概要 |
|---|---|
| マイコンボード | [Raspi-Pico H](https://amzn.asia/d/070Dqocu) |
| ディスプレイモジュール | [GME128128-01-IIC](https://amzn.asia/d/0flXHYrp) |
| アナログスティック | [ADA-2765](https://amzn.asia/d/00nz8Xhm) |
| タクトスイッチ | [SKHCBEA010](https://www.sengoku.co.jp/mod/sgk_cart/detail.php?code=EEHD-08BK) |

※回路図等は記事を参照

---

## 動作確認環境

- MCU: RP2040 (Raspberry Pi Pico H)
- Toolchain: arm-none-eabi-gcc
- SDK: pico-sdk

※ビルドは可能ですが、動作には対象ハードウェアが必要です。
ビルド構成の参考としてCMakeLists.txtを上げています。

## サポートについて
動作不良等の対応は行いません。

## License
This repository is reference-only.
Redistribution and commercial use are prohibited.
See LICENSE for details.
