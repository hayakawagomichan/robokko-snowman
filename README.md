# ロボっこの雪だるま — ATOM サーボ＋ライト

**テストワークショップ用（2026年11月2日）。**

ボタンを押すとサーボモーターが往復し、本体のLEDが虹色に光るプログラムです。
もう一度押すと中心に戻って止まり、白またはオレンジに点灯します。

提供されたプログラム・書き込み用ファイル・説明書を、そのまま収録しています。
買い物リストや組み立て説明は、テスト後に整備する予定です。

## 使い方

**[同梱の説明書（README.txt）](README.txt)** に、配線・書き込み・操作・設定変更の手順があります。

| 使う本体 | 停止中の色 | 書き込み用ファイル |
| --- | --- | --- |
| ATOM Lite | 白 | [ATOM-Lite_white.bin](firmware/ATOM-Lite_white.bin) |
| ATOM Lite | オレンジ | [ATOM-Lite_orange.bin](firmware/ATOM-Lite_orange.bin) |
| ATOMS3 Lite | 白 | [ATOMS3-Lite_white.bin](firmware/ATOMS3-Lite_white.bin) |
| ATOMS3 Lite | オレンジ | [ATOMS3-Lite_orange.bin](firmware/ATOMS3-Lite_orange.bin) |

機種に合ったファイルを選んでください。GitHubからまとめて入手する場合は **Code → Download ZIP** を使います。

## プログラム

- [source/src/main.cpp](source/src/main.cpp)：サーボ・LED・ボタンの動作
- [source/platformio.ini](source/platformio.ini)：PlatformIOの機種・色別設定

編集する場合は `source` フォルダーをVS Code＋PlatformIOで開きます。
今回のアップロードでは、再ビルド・本体への書き込み・実機動作確認は行っていません。

## ロボっこシリーズ

- [なかよし！ロボっこ](https://github.com/hayakawagomichan/robokko)
- [ロボっこのクリスマスベル](https://github.com/hayakawagomichan/robokko-bell)
