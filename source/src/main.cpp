// M5Stack ATOM Lite / ATOMS3 Lite + Grove接続サーボ
//  - ボタン(G39)を押すとサーボのスイープ ON/OFF
//  - モード1: スイープ中はLEDが虹色にゆっくり変化、停止中は白(またはオレンジ)の常時点灯
//  - シリアルモニタ(115200bps)から角度・速さ・色をその場で調整できる
#include <Arduino.h>
#include <ESP32Servo.h>
#include <FastLED.h>

// ===== ピン =====
#ifdef ATOMS3_LITE
// ATOMS3 Lite: Groveの黄(G2)・白(G1)どちらにサーボ信号線がつながっても動くよう両方に出す
constexpr int PIN_SERVO  = 2;
constexpr int PIN_SERVO2 = 1;
constexpr int PIN_BUTTON = 41;
constexpr int PIN_LED    = 35;
constexpr int NUM_LEDS   = 1;
#else
// ATOM Lite / Matrix
constexpr int PIN_SERVO  = 26;  // Grove 黄色線(信号)
constexpr int PIN_SERVO2 = 32;  // Grove 白線
constexpr int PIN_BUTTON = 39;
constexpr int PIN_LED    = 27;
constexpr int NUM_LEDS   = 25;  // ATOM Liteは1個。25にしておけばATOM Matrixでも全部光る
#endif

// ===== モード1 =====
// ===== サーボ設定（シリアルから変更可） =====
float centerDeg = 98;      // スイープの中心角度（88〜108度）
float sweepDeg  = 20;      // 往復の振れ幅（中心±10度 = 20度）
uint32_t periodMs = 1000;  // 1往復にかける時間

// ===== LED設定（シリアルから変更可） =====
uint32_t rainbowMs = 3000;  // スイープ中: 虹色を一周する時間
// 停止中: 色を順番に切り替えていく。点滅させたいときは CRGB::Black を挟む
// 停止中の常時点灯の色。platformio.ini の env ごとに IDLE_COLOR で切り替え（白 / オレンジ）
#ifndef IDLE_COLOR
#define IDLE_COLOR 0xFFFFFF
#endif
CRGB colors[8] = {CRGB(IDLE_COLOR)};
int numColors = 1;
uint32_t ledIntervalMs = 500;  // 色を切り替える間隔
uint8_t brightness = 200;      // 0-255

Servo servo, servo2;
CRGB leds[NUM_LEDS];
bool motorOn = false;
uint32_t sweepStart = 0;

void setLeds(const CRGB& c) {
  fill_solid(leds, NUM_LEDS, c);
  FastLED.show();
}

void servoWrite(float deg) {
  int us = map(deg * 10, 0, 1800, 500, 2400);
  servo.writeMicroseconds(us);
  servo2.writeMicroseconds(us);
}

void updateServo() {
  if (!motorOn) return;
  // サイン波でなめらかに往復させる
  float phase = (float)((millis() - sweepStart) % periodMs) / periodMs;
  float deg = centerDeg + (sweepDeg / 2) * sinf(phase * 2 * PI);
  servoWrite(deg);
}

void updateLed() {
  static uint32_t last = 0;
  static int idx = 0;
  if (motorOn) {
    // ゲーミング風: 色相を一定速度で回してなめらかに色を変える
    uint8_t hue = (uint32_t)((millis() - sweepStart) % rainbowMs) * 256 / rainbowMs;
    setLeds(CHSV(hue, 255, 255));
    last = 0;  // 止めたらすぐ点滅パターンに戻す
    return;
  }
  if (millis() - last < ledIntervalMs) return;
  last = millis();
  idx = (idx + 1) % numColors;
  setLeds(colors[idx]);
}

void setMotor(bool on) {
  motorOn = on;
  sweepStart = millis();
  if (!on) servoWrite(centerDeg);  // 止めたら中心に戻す
  Serial.printf("motor %s\n", on ? "ON" : "OFF");
}

void updateButton() {
  static bool lastPressed = false;
  static uint32_t lastChange = 0;
  bool pressed = digitalRead(PIN_BUTTON) == LOW;
  if (pressed != lastPressed && millis() - lastChange > 50) {  // チャタリング除去
    lastChange = millis();
    lastPressed = pressed;
    if (pressed) setMotor(!motorOn);
  }
}

void printStatus() {
  Serial.printf("center=%.0f sweep=%.0f period=%lums | rainbow=%lums led interval=%lums brightness=%u colors=",
                centerDeg, sweepDeg, periodMs, rainbowMs, ledIntervalMs, brightness);
  for (int i = 0; i < numColors; i++)
    Serial.printf("%02X%02X%02X ", colors[i].r, colors[i].g, colors[i].b);
  Serial.println();
}

void printHelp() {
  Serial.println(
      "commands:\n"
      "  w20        スイープ幅(度)\n"
      "  c90        中心角度(度)\n"
      "  p1000      1往復の時間(ms)\n"
      "  m          モーター ON/OFF\n"
      "  r3000      スイープ中の虹色一周の時間(ms)\n"
      "  i500       停止中のLED切り替え間隔(ms)\n"
      "  b40        LED明るさ(0-255)\n"
      "  l FF0000 000000   LEDの色リスト(16進RGBを空白区切り、最大8色)\n"
      "  ?          現在の設定を表示");
}

void handleSerial() {
  if (!Serial.available()) return;
  String line = Serial.readStringUntil('\n');
  line.trim();
  if (line.isEmpty()) return;
  char cmd = line[0];
  String arg = line.substring(1);
  arg.trim();
  switch (cmd) {
    case 'w': sweepDeg = constrain(arg.toFloat(), 0, 180); break;
    case 'c': centerDeg = constrain(arg.toFloat(), 0, 180); break;
    case 'p': periodMs = max(100L, arg.toInt()); break;
    case 'm': setMotor(!motorOn); break;
    case 'r': rainbowMs = max(100L, arg.toInt()); break;
    case 'i': ledIntervalMs = max(10L, arg.toInt()); break;
    case 'b':
      brightness = constrain(arg.toInt(), 0, 255);
      FastLED.setBrightness(brightness);
      break;
    case 'l': {
      int n = 0;
      while (arg.length() && n < 8) {
        int sp = arg.indexOf(' ');
        String tok = sp < 0 ? arg : arg.substring(0, sp);
        colors[n++] = CRGB(strtoul(tok.c_str(), nullptr, 16));
        arg = sp < 0 ? "" : arg.substring(sp + 1);
        arg.trim();
      }
      if (n > 0) numColors = n;
      break;
    }
    case '?': break;
    default: printHelp(); return;
  }
  if (!motorOn) servoWrite(centerDeg);
  printStatus();
}

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(20);
  pinMode(PIN_BUTTON, INPUT);  // G39は外部プルアップ済み

  FastLED.addLeds<WS2812B, PIN_LED, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(brightness);
  setLeds(colors[0]);

  servo.setPeriodHertz(50);
  servo.attach(PIN_SERVO, 500, 2400);
  servo2.setPeriodHertz(50);
  servo2.attach(PIN_SERVO2, 500, 2400);
  servoWrite(centerDeg);

  delay(300);
  printHelp();
  printStatus();
}

void loop() {
  updateButton();
  updateServo();
  updateLed();
  handleSerial();
  delay(5);
}
