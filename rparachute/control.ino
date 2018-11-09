#include <Servo.h>
#include "Utility.h"
#include "PinAssign.h"

// リーフィング1段階目のサーボ-Aの角度
const int ANGLE_1ST_SERVO_A_OPEN  =  180;
const int ANGLE_1ST_SERVO_A_CLOSE =  100;

// リーフィング1段階目のサーボ-Bの角度
const int ANGLE_1ST_SERVO_B_OPEN   = 0;
const int ANGLE_1St_SERVO_B_CLOSE  = 90;

// リーフィング2段階目のサーボ-Aの角度
const int ANGLE_2ND_SERVO_A_OPEN   = 30;
const int ANGLE_2ND_SERVO_A_CLOSE =  75;

// リーフィング2段階目のサーボ-Bの角度
const int ANGLE_2ND_SERVO_B_OPEN  =  30;
const int ANGLE_2ND_SERVO_B_CLOSE  = 75;

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

  switch (outputs.next_mode) {
    case MODE_SETTING:
      BlinkLed(PIN_LED, 500, 50);
      break;
    case MODE_READY:
      BlinkLed(PIN_LED, 1000, 10);
      break;
    case MODE_BURNING:
      BlinkLed(PIN_LED, 100, 50);
      break;
    case MODE_FREEFALL:
      BlinkLed(PIN_LED, 500, 50);
      break;
    case MODE_HALF_OPEN:
      BlinkLed(PIN_LED, 1000, 50);
      break;
    case MODE_FULL_OPEN:
      BlinkLed(PIN_LED, 3000, 50);
      break;
  }
}

void BlinkLed(int pin, int32_t period_ms, int32_t blink_ratio_percent) {
  int now_value = (millis() % period_ms) * 100 / period_ms;
  digitalWrite(pin, now_value < blink_ratio_percent);
}
