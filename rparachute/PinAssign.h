#ifndef PIN_ASSIGN_HEADER_FILE
#define PIN_ASSIGN_HEADER_FILE

#include<pins_arduino.h>

// 初期化後に点灯させるLEDのピン
const int PIN_LED = 13;

// リーフィング1段階目のサーボピン
const int PIN_1ST_SERVO_A = 2;
const int PIN_1ST_SERVO_B = 3;

// リーフィング2段階目のサーボピン
const int PIN_2ND_SERVO_A = 4;
const int PIN_2ND_SERVO_B = 5;

// ONのときMODE_SETTINGからMODE_READYに遷移するスイッチ
const int PIN_SWITCH = 12;


#endif
