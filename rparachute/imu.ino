#include "Wire.h"
#include "MPU9150.h"
#include "EEPROM.h"
#include "Utility.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

const uint8_t MPU9250_DEVICE_ID = 0x39;

const float ACCEL_GAIN = 16.0 / (float)(1L << 15);
const float GYRO_GAIN = 500.0 / (float)(1L << 15);
const float MAG_GAIN = 0.48 / (float)(1L << 14);

MPU9150 accelGyroMag;
Adafruit_BMP280 bmp;
SensorBias bias;

void ImuInit() {
  Wire.begin();

  // mpu9250を初期化
  Serial.print("Initializing accel, gyro, mag...");
  accelGyroMag.initialize();
  if (accelGyroMag.getDeviceID() == MPU9250_DEVICE_ID) {
    Serial.println("ok");
  } else {
    Serial.println("ng");
    delay(1000);
    ResetMicrocontroller();
  }

  // bmp280を初期化
  Serial.print("Initializing bmp280...");
  if (bmp.begin(0x76)) {
    Serial.println("ok");
  } else {
    Serial.println("ng");
    delay(1000);
    ResetMicrocontroller();
  }

  // スケールを設定
  accelGyroMag.setFullScaleGyroRange(1); // ±500 dps
  accelGyroMag.setFullScaleAccelRange(3); // ±16 g

  // バイアスデータをEEPROMから読み取り
  bias = read_from_eeprom<SensorBias>();
}

void ImuReadAccel(float *ax, float *ay, float *az) {
  int16_t ax_i, ay_i, az_i;

  accelGyroMag.getAcceleration(&ax_i, &ay_i, &az_i);

  *ax = ax_i * ACCEL_GAIN;
  *ay = ay_i * ACCEL_GAIN;
  *az = az_i * ACCEL_GAIN;
}

void ImuGetMotion9(
  float *ax, float *ay, float *az,
  float *gx, float *gy, float *gz,
  float *mx, float *my, float *mz) {
  int16_t ax_i, ay_i, az_i;
  int16_t gx_i, gy_i, gz_i;
  int16_t mx_i, my_i, mz_i;

  // データをの読み取り
  accelGyroMag.getMotion9(
    &ax_i, &ay_i, &az_i, &gx_i, &gy_i, &gz_i, &mx_i, &my_i, &mz_i);

  *ax = ax_i * ACCEL_GAIN;
  *ay = ay_i * ACCEL_GAIN;
  *az = az_i * ACCEL_GAIN;

  *gx = gx_i * GYRO_GAIN - bias.gx;
  *gy = gy_i * GYRO_GAIN - bias.gy;
  *gz = gz_i * GYRO_GAIN - bias.gz;

  *mx = mx_i * MAG_GAIN - bias.mx;
  *my = my_i * MAG_GAIN - bias.my;
  *mz = mz_i * MAG_GAIN - bias.mz;
}

// 気圧を単位[hPa]として読み込む
void ImuReadPressure(float* p) {
  *p = bmp.readPressure() / 100.0;
}

// バイアスを設定する関数
void ImuDetectBias() {
  bias.gx = bias.gy = bias.gz = 0;
  bias.mx = bias.my = bias.mz = 0;

  ImuDetectGyroBias(&bias);
  ImuDetectMagBias(&bias);

  write_to_eeprom<SensorBias>(bias);
}

// ジャイロバイアス推定機
void ImuDetectGyroBias(SensorBias *bias) {
  Serial.print("Detect gyro bias...\r\n");
  Serial.print("Don't move sensor.\r\n");
  Serial.print("Detecting will start after 3 sec.\r\n");
  delay(3000);

  float sum_x = 0.0, sum_y = 0.0, sum_z = 0.0;
  float ax, ay, az;
  float gx, gy, gz;
  float mx, my, mz;

  for (int i = 0 ; i < 100 ; i++ ) {
    ImuGetMotion9(&ax, &ay, &az, &gx, &gy, &gz, &mx, &my, &mz);
    sum_x += gx;
    sum_y += gy;
    sum_z += gz;

    if ( i % 10 == 9 ) {
      Serial.print(i + 1);
      Serial.print("percent fin.\r\n");
    }
    delay(100);
  }

  gx = sum_x / 100.0f;
  gy = sum_y / 100.0f;
  gz = sum_z / 100.0f;

  Serial.print("(gx,gy,gz)=");
  Serial.print(gx);
  Serial.print(",");
  Serial.print(gy);
  Serial.print(",");
  Serial.print(gz);
  Serial.print("\r\n");

  bias->gx = gx;
  bias->gy = gy;
  bias->gz = gz;
}

// 磁気バイアス推定機
void ImuDetectMagBias(SensorBias *bias)
{
  Serial.print("Detect mag bias...\r\n");
  Serial.print("Move sensor.\r\n");
  Serial.print("Detecting will start after 3 sec.\r\n");
  delay(3000);

  float a, b, c, r;
  float output[4];
  float ax, ay, az;
  float gx, gy, gz;
  float mx, my, mz;
  float A[4][4];
  float B[4];

  for (int i = 0 ; i < 4 ; i++ ) {
    B[i] = 0.0f;
    output[i] = 0.0f;
    for (int j = 0; j < 4; j++) {
      A[i][j] = 0.0f;
    }
  }

  for (int i = 0 ; i < 100 ; i++ ) {
    // センサーの値を読み取る
    ImuGetMotion9(&ax, &ay, &az, &gx, &gy, &gz, &mx, &my, &mz);

    // 左辺(係数行列)
    A[0][0] += 2.0f * mx * mx;
    A[0][1] += 2.0f * mx * my;
    A[0][2] += 2.0f * mx * mz;
    A[0][3] += 2.0f * mx;

    A[1][0] += 2.0f * my * mx;
    A[1][1] += 2.0f * my * my;
    A[1][2] += 2.0f * my * mz;
    A[1][3] += 2.0f * my;

    A[2][0] += 2.0f * mz * mx;
    A[2][1] += 2.0f * mz * my;
    A[2][2] += 2.0f * mz * mz;
    A[2][3] += 2.0f * mz;

    A[3][0] += 2.0f * mx;
    A[3][1] += 2.0f * my;
    A[3][2] += 2.0f * mz;
    A[3][3] += 2.0f * 1.0f;

    // 右辺
    B[0] += mx * (mx * mx + my * my + mz * mz);
    B[1] += my * (mx * mx + my * my + mz * mz);
    B[2] += mz * (mx * mx + my * my + mz * mz);
    B[3] += mx * mx + my * my + mz * mz;

    if ( i % 10 == 9 ) {
      Serial.print( i + 1);
      Serial.println(" percent fin.");
    }
    delay(100);
  }
  solve<4>(A, output, B); // 連立方程式を計算

  // a,b,c,dのパラメータがB[0],B[1],B[2],B[3]に代入されているので
  // a,b,c,rを代入して関数を終わる
  a = output[0];
  b = output[1];
  c = output[2];
  r = sqrt(a * a + b * b + c * c + 2 * output[3]);

  Serial.print("(mbx,mby,mbz,r)=");
  Serial.print(a);
  Serial.print(",");
  Serial.print(b);
  Serial.print(",");
  Serial.print(c);
  Serial.print(",");
  Serial.print(r);
  Serial.print("\r\n");

  bias->gx = a;
  bias->gy = b;
  bias->gz = c;
}

