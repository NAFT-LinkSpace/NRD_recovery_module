#include <Wire.h>
#include <Servo.h>
#include "Utility.h"
#include "DataBuffer.h"
#include "DataTransfer.h"
#include "PinAssign.h"
#include "Parameters.h"
#include "LPF.h"

// 気圧センサの出力値にかけるLPF
LPF lpf_pres(32);
// スリープ用変数
uint32_t old_time_ms;
// シリアル通信でテストする時用のやつ
DataBuffer my_buff(16);
// データをシリアルで送信するためのもの
DataTransfer<SensorInfo> data_transfer(0);

void setup() {
  // 初期化されていない場合はLEDを消灯
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  SerialSetup();
  Serial.println("initializing...");
  delay(100);

  // Wire初期化
  Wire.begin();
  delay(100);

  // 制御関係の初期化
  ControlInit();
  delay(100);

  // imu関係の初期化
  ImuInit();
  delay(100);

  // 各状態の初期化
  ModeInit();
  delay(100);

  // このファイルで定義したグローバル変数の初期化
  old_time_ms = millis();

  // 初期化されたことを伝える
  Serial.println("initialized");
  digitalWrite(PIN_LED, HIGH);
}

void loop() {
  SensorInfo sensors;
  OutputInfo outputs;

#ifdef SERIAL_TEST
  // TODO: 実機試験
  sensors = ReadSensorsFromSerial();
#else
  // センサーの読み取り
  sensors = ReadSensors();
#endif

  // 制御値の決定
  outputs = ModeLoop(sensors);

#ifdef BATTERY_TEST
  // TODO: バッテリー試験
  outputs.is_1st_servo_open ^= (sensors.time_ms / (30 * 60 * 1000) ) % 2 == 0;
  outputs.is_2nd_servo_open ^= (sensors.time_ms / (30 * 60 * 1000) ) % 2 == 0;
#endif

  // 出力
  ControlLoop(outputs);

#ifdef DEBUG_MODE
  Serial.print(to_string(sensors));
  Serial.println(to_string(outputs));
#else
  static uint32_t last_send_time_ms = millis();
  if ( sensors.time_ms - last_send_time_ms > DATA_SEND_INTERVAL_MS ) {
    data_transfer.Send(Serial, sensors);
    last_send_time_ms = sensors.time_ms;
  }
#endif

  // 待機
  const uint32_t dt = sensors.time_ms - old_time_ms;
  delay(WAIT_TIME_MS < dt ? 0 : WAIT_TIME_MS - dt);
  old_time_ms = sensors.time_ms;
  //  Serial.println(dt);
}

// センサーを読み取る関数
SensorInfo ReadSensors() {
  SensorInfo sensors;
  float ax, ay, az;
  float temp;

  // 現在の状態を取得
  sensors.mode = ModeGetCurrentMode();

  // 地上高度の取得
  sensors.height_at_ground = ModeGetHeightAtGround();

  // 時間[ms]の読み取り
  sensors.time_ms = millis();

  // 加速度[g]の読み取り
  ImuReadAccel(&ax, &ay, &az);

  ax -= ACCEL_BIAS_X;
  ay -= ACCEL_BIAS_Y;
  az -= ACCEL_BIAS_Z;

  sensors.accel_norm = sqrt(ax * ax + ay * ay + az * az);

  // 気圧[hPa]の読み取り
  ImuReadPressure(&temp);
  temp = lpf_pres.Push(temp);
  sensors.height = pressureToAltitudeMeters(temp, STANDARD_ATMOSPHERE);

  // バッテリー電圧の測定
  sensors.battery_arduino = (float)analogRead(PIN_BATTERY_ARDUINO)*BATTERY_ARDUINO_COEFFICIENT;
  sensors.battery_servo = (float)analogRead(PIN_BATTERY_SERVO)*BATTERY_SERVO_COEFFICIENT;

  // コマンドの読み取り
  SerialReadCommand(&sensors);

  return sensors;
}


// 実機のプログラム試験用
// シリアル通信で受け取ったデータから挙動を決定する
SensorInfo ReadSensorsFromSerial() {
  SensorInfo sensors;
  uint8_t data;

  // 0初期化
  memset(&sensors, 0, sizeof(SensorInfo));

  while (true) {
    if ( not Serial.available() ) {
      delay(1);
    } else {
      data = Serial.read();
      if (my_buff.GetLength() == 0) {
        if ( data == 0xAA )
          my_buff.Push(data);
      } else {
        my_buff.Push(data);
      }

      if (my_buff.GetLength() == 16) {
        uint8_t starts[2] = {0xAA, 0xBB};
        uint8_t ends[2] = {0xCC, 0xDD};
        uint8_t temp[16];
        uint32_t *t;
        float *h, *a;

        if ( my_buff.StartsWith(starts, 2) && my_buff.EndsWith(ends, 2) ) {
          my_buff.Copy(temp);
          t = (uint32_t*)(temp + 2);
          h = (float*)(temp + 6);
          a = (float*)(temp + 10);


          sensors.time_ms = *t;
          sensors.height = *h;
          sensors.accel_norm = *a;

          break;
        }

        my_buff.Clear();
      }
    }
  }

  my_buff.Clear();
  return sensors;
}
