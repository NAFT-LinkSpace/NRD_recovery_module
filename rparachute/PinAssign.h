#ifndef PIN_ASSIGN_HEADER_FILE
#define PIN_ASSIGN_HEADER_FILE

#include<pins_arduino.h>

// 初期化後に点灯させるLEDのピン
const int PIN_LED = 13;

// リーフィング1段階目のサーボピン
const int PIN_1ST_SERVO_A = 5;
const int PIN_1ST_SERVO_B = 6;

// リーフィング2段階目のサーボピン
const int PIN_2ND_SERVO_A = 9;
const int PIN_2ND_SERVO_B = 10;

//バッテリー電圧測定用のピン

const int PIN_BATTERY_ARDUINO = 7;
const int PIN_BATTERY_SERVO = 6;

#endif
