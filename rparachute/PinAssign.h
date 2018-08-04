#ifndef PIN_ASSIGN_HEADER_FILE
#define PIN_ASSIGN_HEADER_FILE

#include<pins_arduino.h>

// MEMO: https://www.arduino.cc/en/Reference/Servo
// MEMO: サーボのせいで pins 9 and 10, 使えない
// MEMO: https://www.arduino.cc/en/Reference/SoftwareSerial
//　Not all pins on the Mega and Mega 2560 support change interrupts,
// so only the following can be used for RX:
// 10, 11, 12, 13, 14, 15, 50, 51, 52, 53, A8 (62), A9 (63), A10 (64), A11 (65), A12 (66), A13 (67), A14 (68), A15 (69).

// 初期化後に点灯させるLEDのピン
const int PIN_LED = 13;

// リーフィング1段階目のサーボピン
const int PIN_1ST_SERVO_A = 5;
const int PIN_1ST_SERVO_B = 4;

// リーフィング2段階目のサーボピン
const int PIN_2ND_SERVO_A = 3;
const int PIN_2ND_SERVO_B = 2;

// ONのときMODE_SETTINGからMODE_READYに遷移するスイッチ
//#define PIN_READY_SW      4


#endif

