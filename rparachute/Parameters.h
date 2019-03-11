#ifndef PARAMETERS_HEADER_FILE
#define PARAMETERS_HEADER_FILE

// 時間でMODE_SETTINGからMODE_READYに遷移したい場合は
// TO_READY_TIMEをコメントインするとTO_READY_TIME[s]後に遷移する
// #define ENABLE_TO_READY_BY_TIME
const float TO_READY_TIME = 15*60;

// 点火検知は加速度のノルムが
// ACCEL_IGNITION_THRESHOLD[g]を超えた状態を
// TIME_IGNITION_THRESHOLD[s]以上の時間継続した場合，点火と判断する
const float IGNITION_THRESHOLD_ACCEL = (3.0);
const float IGNITION_THRESHOLD_TIME = (0.1);

// エンジン燃焼時間
const float BURNING_TIME = (5.6);

// パラシュートは，
// 最高高度に達した時間からOPEN_TIME_FROM_TOP_REACHED[s] 後，
// もしくは状態が遷移してから OPEN_TIME_FROM_MODE_CHANGED[s] 後に開傘する
const float OPEN_TIME_FROM_TOP_REACHED = (0.5);
const float OPEN_TIME_FROM_MODE_CHANGED = (11.9);

// パラシュートを絞った状態で降下する最長時間[s]
const float OPEN_ONLY_HALF_TIME = (50.0);

// 地上からの高度がOPEN_HEIGHT_THRESHOLD[m]未満になった場合，
// MODE_HALF_OPENからMODE_FULL_OPENに遷移する
const float OPEN_HEIGHT_THRESHOLD = (90-17.5);

// 1ループの待機時間[ms]
const uint32_t WAIT_TIME_MS = (10);

// シリアルでデータを送信する間隔[ms]
const uint32_t DATA_SEND_INTERVAL_MS = 50;

// バッテリー試験をする際にはこれをコメントイン
//#define BATTERY_TEST

// シリアル通信で挙動をテストする場合はこれをコメントイン
// #define SERIAL_TEST/

// センサーデータなどをSerialに送信する場合はこれをコメントイン
//#define DEBUG_MODE

// コメントインで推定によるHALF_MODEへの遷移をOFFにする
// #define DISABLE_ESTIMATION_TO_HALF
// コメントインでタイマーによるHALF_MODEへの遷移をOFFにする
// #define DISABLE_TIMER_TO_HALF
// コメントインで高度によるFULL_MODEへの遷移をOFFにする
// #define DISABLE_HEIGHT_TO_FULL
// コメントインでタイマーによるHALF_MODEへの遷移をOFFにする
// #define DISABLE_TIMER_TO_FULL

const float ACCEL_BIAS_X = 0.0;
const float ACCEL_BIAS_Y = 0.0;
const float ACCEL_BIAS_Z = 0.37;

//analogReadの返値にかけると電圧が出る係数　抵抗の誤差でちょっと変わる
const float BATTERY_ARDUINO_COEFFICIENT = 9.7215E-3;
const float BATTERY_SERVO_COEFFICIENT = 9.8092E-3;

#endif
