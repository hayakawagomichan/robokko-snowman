ATOM サーボ＋ライト プログラム
================================

M5Stack の ATOM に Grove でつないだサーボモーターを往復させて、
本体のLEDを光らせるプログラムです。


■ 動き
-------
・本体のボタンを押すたびに、モーターの往復が スタート / ストップ
・モーター：88度〜108度 の間を往復（1往復 1秒）
・往復中のライト：虹色になめらかに変化（3秒で一周）
・止まっているときのライト：白 または オレンジ で点灯（どちらかを選んで書き込みます）


■ 中身
-------
firmware/  ……… 書き込み用ファイル（下の表から1つ選ぶ）
source/    ……… プログラム本体（改造したい人向け）

  使うATOM       止まっているときの色   ファイル
  -----------    --------------------   ------------------------
  ATOM Lite      白                     ATOM-Lite_white.bin
  ATOM Lite      オレンジ               ATOM-Lite_orange.bin
  ATOMS3 Lite    白                     ATOMS3-Lite_white.bin
  ATOMS3 Lite    オレンジ               ATOMS3-Lite_orange.bin

※ ATOM Lite と ATOMS3 Lite は中のチップが違うので、必ず自分の機種のファイルを選んでください。
※ ATOMS3 Lite は本体に「S3」と書いてあります。


■ 配線
-------
サーボモーターを Grove ケーブルで ATOM の Grove 端子につなぎます。
サーボの信号線は、Grove ケーブルの黄色・白色のどちらにつながっていても動きます。
（赤 = 5V、黒 = GND）


■ 書き込み方（ブラウザだけでできる方法）
------------------------------------------
パソコンの Google Chrome か Microsoft Edge を使います（Safari では動きません）。

 1. ATOM を USB ケーブルでパソコンにつなぐ
    ※ 充電専用のケーブルだと認識しません。データ通信できるケーブルを使ってください。

 2. Chrome で次のページを開く
    https://espressif.github.io/esptool-js/

 3. 「Program」の欄で Baudrate を 115200 にして「Connect」を押す
    → 出てきた一覧から ATOM のポートを選んで「接続」
      （「usbserial」や「usbmodem」、「USB JTAG/serial」などと書いてあるもの）

 4. Flash Address を「0x0」にする（最初から 0x1000 などになっていたら 0x0 に直す）
    「ファイルを選択」で、上の表から選んだ .bin ファイルを選ぶ

 5. 「Program」を押す
    → 進み具合が表示されて、100% になったら完了

 6. ATOM の USB を一度抜いて挿し直す（またはリセットボタンを押す）
    → ライトが白（またはオレンジ）に点いたら成功です
    → ボタンを押すとモーターが動き出して、ライトが虹色になります


■ うまくいかないとき
--------------------
・Connect でポートが出てこない
   → ケーブルを変える / 別のUSBポートにつなぐ

・ATOMS3 Lite で接続に失敗する
   → 本体のリセットボタン（横の小さいボタン）を、緑のランプが点くまで2秒くらい長押ししてから、
     もう一度 Connect する（書き込みモードになります）
     書き込み後は、USB を挿し直してください。

・モーターが動かない
   → Grove ケーブルがしっかり挿さっているか確認
   → USB の電力が足りないことがあるので、パソコン直結か、しっかりしたUSB電源につなぐ


■ 改造したい人向け（PlatformIO）
--------------------------------
source/ フォルダを VSCode + PlatformIO で開くと、角度や色を変えて書き込めます。

  角度・速さ：src/main.cpp の centerDeg（中心）/ sweepDeg（振れ幅）/ periodMs（1往復の時間）
  明るさ    ：src/main.cpp の brightness（0〜255）
  止まっているときの色：platformio.ini の IDLE_COLOR（16進のRGB、例 0xFF3000）

  書き込みコマンド：
    ATOM Lite   白       pio run -e white     -t upload
    ATOM Lite   オレンジ pio run -e orange    -t upload
    ATOMS3 Lite 白       pio run -e s3-white  -t upload
    ATOMS3 Lite オレンジ pio run -e s3-orange -t upload

書き込みし直さずに試したいときは、シリアルモニタ（115200bps）からコマンドを送れます。
（電源を切ると元に戻ります）
  w20   振れ幅（度）           c98   中心の角度（度）
  p1000 1往復の時間（ミリ秒）    m     モーターのスタート / ストップ
  r3000 虹色が一周する時間（ミリ秒）
  b200  明るさ（0〜255）
  l FF3000   止まっているときの色
  ?     今の設定を表示
