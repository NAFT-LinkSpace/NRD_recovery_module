#include <Servo.h>
#include "Utility.h"
#include "PinAssign.h"

// リーフィング1段階目のサーボ-Aの角度
const int ANGLE_1ST_SERVO_A_OPEN  =  10;
const int ANGLE_1ST_SERVO_A_CLOSE =  170;

// リーフィング1段階目のサーボ-Bの角度
const int ANGLE_1ST_SERVO_B_OPEN   = 170;
const int ANGLE_1St_SERVO_B_CLOSE  = 10;

// リーフィング2段階目のサーボ-Aの角度
const int ANGLE_2ND_SERVO_A_OPEN   = 170;
const int ANGLE_2ND_SERVO_A_CLOSE =  80;

// リーフィング2段階目のサーボ-Bの角度
const int ANGLE_2ND_SERVO_B_OPEN  =  10;
const int ANGLE_2ND_SERVO_B_CLOSE  = 170;

Servo servo_1st_a;  // リーフィング1段階目のサーボ-A
Servo servo_1st_b;  // リーフィング1段階目のサーボ-B
Servo servo_2nd_a;  // リーフィング2階目のサーボ-A
Servo servo_2nd_b;  // リーフィング2段階目のサーボ-B

void ControlInit() {
  servo_1st_a.attach(PIN_1ST_SERVO_A);
  servo_1st_b.attach(PIN_1ST_SERVO_B);
  servo_2nd_a.attach(PIN_2ND_SERVO_A);
  servo_2nd_b.attach(PIN_2ND_SERVO_B);
  
  servo_1st_a.write(ANGLE_1ST_SERVO_A_OPEN);
  servo_1st_b.write(ANGLE_1ST_SERVO_B_OPEN);
  servo_2nd_a.write(ANGLE_2ND_SERVO_A_OPEN);
  servo_2nd_b.write(ANGLE_2ND_SERVO_B_OPEN);
}

void ControlLoop(const OutputInfo &outputs) {
  // リーフィング1段階目のサーボの開閉
  if (outputs.is_1st_servo_open) {
    servo_1st_a.write(ANGLE_1ST_SERVO_A_OPEN);
    servo_1st_b.write(ANGLE_1ST_SERVO_B_OPEN);
  } else {
    servo_1st_a.write(ANGLE_1ST_SERVO_A_CLOSE);
    servo_1st_b.write(ANGLE_1St_SERVO_B_CLOSE);
  }

  // リーフィング2段階目のサーボの開閉
  if (outputs.is_2nd_servo_open) {
    servo_2nd_a.write(ANGLE_2ND_SERVO_A_OPEN);
    servo_2nd_b.write(ANGLE_2ND_SERVO_B_OPEN);
  } else {
    servo_2nd_a.write(ANGLE_2ND_SERVO_A_CLOSE);
    servo_2nd_b.write(ANGLE_2ND_SERVO_B_CLOSE);
  }
}

