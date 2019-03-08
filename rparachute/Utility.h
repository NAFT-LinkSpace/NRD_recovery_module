#ifndef UTILITY_HEADER_FILE
#define UTILITY_HEADER_FILE

#include <Arduino.h>
#include <inttypes.h>
#include <math.h>
#include <EEPROM.h>

// 標準大気圧[hPa]
#define STANDARD_ATMOSPHERE (1013.25)

// ロケットの状態を表す列挙型
enum RocketMode {
  MODE_SETTING = 0,
  MODE_READY = 1,
  MODE_BURNING = 2,
  MODE_FREEFALL = 3,
  MODE_HALF_OPEN = 4,
  MODE_FULL_OPEN = 5,
};

// Serialで送られてくる信号の解析結果
enum CommandResult {
  RESULT_TO_SETTING = 0,  // MODE_SETTINGに強制遷移する
  RESULT_TO_READY = 1,    // MODE_READYに強制遷移する
  RESULT_RESET = 2,       // リセットする
  RESULT_CHANGE_1ST = 3,  // MODE_SETTINGの時第1段階モータの開閉を変更する
  RESULT_CHANGE_2ND = 4,  // MODE_SETTINGの時第2段階モータの開閉を変更する
  RESULT_NONE = 5,        // 意味を解析できなかった場合
};

// センサーの値をまとめるSensorInfo構造体
typedef struct _SensorInfo {
  uint32_t time_ms;       // 時間[ms]
  float accel_norm;       // 加速度ノルム
  float height;           // 高度[m]
  float height_at_ground; // 地上の高度
  float battery_servo;    // サーボ用電源電圧
  float battery_arduino;  // マイコン用電源電圧
  enum RocketMode mode;   // 現在の状態
  enum CommandResult cmd; // 命令
} SensorInfo;

// 出力をまとめるOutputInfo構造体
typedef struct _OutputInfo {
  enum RocketMode next_mode;  // 次状態
  bool is_1st_servo_open;     // リーフィング1段階目のサーボモータ
  bool is_2nd_servo_open;     // リーフィング2段階目のサーボモータ
} OutputInfo;

// 磁気，ジャイロのバイアスを格納する構造体
typedef struct {
  float gx, gy, gz;
  float mx, my, mz;
} SensorBias;

// EEPROMにデータを書き込む関数
template <typename T>
void write_to_eeprom(T data) {
  char *buff = (char*)(&data);

  for (size_t i = 0 ; i < sizeof(T) ; i++ ) {
    EEPROM.write(i, buff[i]);
  }
}

// EEPROMからデータを読み込む関数
template <typename T>
T read_from_eeprom() {
  T data;
  char *buff = (char*)(&data);

  for (size_t i = 0 ; i < sizeof(T) ; i++ ) {
    buff[i] = EEPROM.read(i);
  }

  return data;
}

// xを[xmin,xmax]から[ymin,ymax]になるように線形補間した後サチる
int16_t map_constrain(int16_t x, int16_t xmin, int16_t xmax, int16_t ymin, int16_t ymax) {
  int16_t y = map(x, xmin, xmax, ymin, ymax);
  return constrain(y, ymin, ymax);
}

// pressure[hPa]をpres0を基準にして高度[m]に変換する関数
float pressureToAltitudeMeters(float pres, float pres0) {
  return (1 - pow(pres / pres0, 0.190263)) * 44330.8;
}

// マイコンをリセットする関数
void ResetMicrocontroller() {
  // Teensy 3.5(ARMプロセッサ)
  // http://qiita.com/edo_m18/items/a7c747c5bed600dca977
  //  asm volatile ("B _start")/;

  // Arduino Uno他(AVRマイコン)
  asm volatile ("  jmp 0");
}

// N元連立線形方程式を解く
template<int N>
void solve(const float M[N][N], float x[N], const float b[N])
{
  float A[N][N];
  float B[N];
  float temp;

  // コピー
  for (int i = 0 ; i < N ; i++ ) {
    B[i] = b[i];
    for (int j = 0 ; j < N ; j++ ) {
      A[i][j] = M[i][j];
    }
  }
  for (int i = 0 ; i < N ; i++ ) {
    // ピボット選択
    for (int j = i + 1 ; j < N ; j++ ) {
      if ( fabs(A[i][j]) >= 1e-8f )
        break;

      for (int k = 0 ; k < N ; k++ )
        A[i][k] += A[j][k];
      B[i] += B[j];
    }

    // 対角成分を1に
    temp = A[i][i];
    for (int j = i ; j < N ; j++ )
      A[i][j] /= temp;
    B[i] /= temp;

    // 前進消去
    for (int j = i + 1 ; j < N ; j++ ) {
      temp = A[j][i];

      for (int k = i ; k < N ; k++ )
        A[j][k] -= temp * A[i][k];
      B[j] -= temp * B[i];
    }
  }

  // 後進消去
  for (int i = N - 1 ; i >= 0 ; i-- ) {
    for (int j = i - 1 ; j >= 0 ; j-- ) {
      B[j] -= A[j][i] * B[i];
    }
  }

  for (int i = 0 ; i < N ; i++ )
    x[i] = B[i];
}


// RocketMode列挙型を文字列に変換する関数
String to_string(const enum RocketMode mode) {
  switch (mode) {
    case MODE_SETTING:
      return "MODE_SETTING";
    case MODE_READY:
      return "MODE_READY";
    case MODE_BURNING:
      return "MODE_BURNING";
    case MODE_FREEFALL:
      return "MODE_FREEFALL";
    case MODE_HALF_OPEN:
      return "MODE_HALF_OPEN";
    case MODE_FULL_OPEN:
      return "MODE_FULL_OPEN";
    default:
      return "MODE_UNKNOWN";
  }
}

// CommandResult列挙型を文字列に変換する関数
String to_string(const enum CommandResult result) {
  switch (result) {
    case RESULT_TO_SETTING:
      return "RESULT_TO_SETTING";
    case RESULT_TO_READY:
      return "RESULT_TO_READY";
    case RESULT_RESET:
      return "RESULT_RESET";
    case RESULT_CHANGE_1ST:
      return "RESULT_CHANGE_1ST";
    case RESULT_CHANGE_2ND:
      return "RESULT_CHANGE_2ND";
    case RESULT_NONE:
      return "RESULT_NONE";
    default:
      return "RESULT_UNKNOWN";
  }
}

// センサーの値をStringに変換する関数
String to_string(const SensorInfo sensors) {
  String dataString = "";

  dataString += String(sensors.time_ms);
  dataString += ",\t";
  dataString += String(sensors.accel_norm);
  dataString += ",\t";
  dataString += String(sensors.height);
  dataString += ",\t";
  dataString += String(sensors.height_at_ground);
  dataString += ",\t";
  dataString += to_string(sensors.mode);
  dataString += ",\t";
  dataString += to_string(sensors.cmd);
  dataString += ",\t";

  return dataString;
}

// 出力の値をStringに変換する関数
String to_string(const OutputInfo outputs) {
  String dataString = "";

  dataString += to_string(outputs.next_mode);
  dataString += ",\t";
  dataString += String(outputs.is_1st_servo_open);
  dataString += ",\t";
  dataString += String(outputs.is_2nd_servo_open);
  dataString += ",\t";

  return dataString;
}

#endif
