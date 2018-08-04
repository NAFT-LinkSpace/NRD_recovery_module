#ifndef PARAMETERS_HEADER_FILE
#define PARAMETERS_HEADER_FILE

// 点火検知は加速度のノルムが
// ACCEL_IGNITION_THRESHOLD[g]を超えた状態を
// TIME_IGNITION_THRESHOLD[s]以上の時間継続した場合，点火と判断する
#define IGNITION_THRESHOLD_ACCEL (4.0) 
#define IGNITION_THRESHOLD_TIME  (0.1)

// エンジン燃焼時間
#define BURNING_TIME (3.0) 

// パラシュートは，
// 最高高度に達した時間からOPEN_TIME_FROM_TOP_REACHED[s] 後，
// もしくは状態が遷移してから OPEN_TIME_FROM_MODE_CHANGED[s] 後に開傘する
#define OPEN_TIME_FROM_TOP_REACHED  (0.5)
#define OPEN_TIME_FROM_MODE_CHANGED (11.0)

// パラシュートを絞った状態で降下する時間[s]
#define OPEN_ONLY_HALF_TIME (50.0)

// 地上からの高度がOPEN_HEIGHT_THRESHOLD[m]未満になった場合，
// MODE_HALF_OPENからMODE_FULL_OPENに遷移する
#define OPEN_HEIGHT_THRESHOLD (100.0)

// 1ループの待機時間[ms]
#define WAIT_TIME (10)

// バッテリー試験をする際にはこれをコメントイン
//#define BATTERY_TEST

// シリアル通信で挙動をテストする場合はこれをコメントイン
//#define SERIAL_TEST

#endif
