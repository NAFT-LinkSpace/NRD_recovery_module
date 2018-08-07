#ifndef MODE_BASE_HEADER_FILE
#define MODE_BASE_HEADER_FILE

#include "Utility.h"
#include "TopDetector.h"
#include "Parameters.h"

// 状態の挙動を定義するためのクラスの基底クラス
class ModeBase {
  public:
    virtual ~ModeBase() {}

    // センサーの値から出力および内部状態を決定する
    virtual OutputInfo Control(const SensorInfo sensors) = 0;

    // 状態遷移直後に初期化を行うメソッド
    virtual void Reset() = 0;

    // 状態が遷移した時間をもつ関数
    void SetStartedTime(uint32_t time_ms) {
      started_time_ms_ = time_ms;
    }

  protected:
    // 状態が遷移した時間[ms]
    uint32_t started_time_ms_;
};

// サーボを開閉してパラシュートを格納したり動作確認を行う状態
class ModeSetting : ModeBase {
  public:
    ModeSetting() {
      Reset();
    }

    OutputInfo Control(const SensorInfo sensors) override {
      OutputInfo outputs;
      const float delta_time = 1e-3 * (float)(sensors.time_ms - started_time_ms_);

      is_1st_servo_open_ ^= sensors.cmd == RESULT_CHANGE_1ST;
      is_2nd_servo_open_ ^= sensors.cmd == RESULT_CHANGE_2ND;

      outputs.is_1st_servo_open = is_1st_servo_open_;
      outputs.is_2nd_servo_open = is_2nd_servo_open_;

      if (sensors.switch_pushed) {
        outputs.next_mode = MODE_READY;
      } else {
        outputs.next_mode = MODE_SETTING;
      }


      return outputs;
    }

    void Reset() {
      is_1st_servo_open_ = true;
      is_2nd_servo_open_ = true;
    }

  private:
    bool is_1st_servo_open_;
    bool is_2nd_servo_open_;
};

// 発射待機状態
class ModeReady : ModeBase {
  public:
    ModeReady() {
      Reset();
    }

    OutputInfo Control(const SensorInfo sensors) override {
      OutputInfo outputs;

      // 高加速度を記録しているかどうかの判定を行う
      const float is_high_accel = sensors.accel_norm > IGNITION_THRESHOLD_ACCEL;

      outputs.is_1st_servo_open = false;
      outputs.is_2nd_servo_open = false;

      if ( is_high_accel && is_high_accel_before_ ) {
        // 何秒間しきい値を超えているかを計算
        const float delta_time = 1e-3 * (float)(sensors.time_ms - high_accel_started_time_);

        if ( IGNITION_THRESHOLD_TIME < delta_time ) {
          outputs.next_mode = MODE_BURNING;  // 長時間高加速度なので点火と判断
        } else {
          outputs.next_mode = MODE_READY;
        }
      } else if (is_high_accel) {
        // 高加速度が始まったので開始時刻を記録
        outputs.next_mode = MODE_READY;
        high_accel_started_time_ = sensors.time_ms;
      } else {
        outputs.next_mode = MODE_READY;
      }

      is_high_accel_before_ = is_high_accel;


      return outputs;
    }

    void Reset() {
      is_high_accel_before_ = false;
    }
  private:

    bool is_high_accel_before_;         // 前回の処理で高加速度を記録した場合true
    uint32_t high_accel_started_time_;  // 高加速度を最初に記録した時間[ms]
};

// エンジンが燃焼している状態
class ModeBurning : ModeBase {
  public:
    ModeBurning() {
      Reset();
    }

    OutputInfo Control(const SensorInfo sensors) override {
      OutputInfo outputs;
      const float delta_time = 1e-3 * (float)(sensors.time_ms - started_time_ms_);

      outputs.is_1st_servo_open = false;
      outputs.is_2nd_servo_open = false;

      // BURNING_TIME [s]経過したら遷移
      if ( BURNING_TIME < delta_time ) {
        outputs.next_mode = MODE_FREEFALL;
      } else {
        outputs.next_mode = MODE_BURNING;
      }

      return outputs;
    }

    void Reset() {

    }
  private:

};

// エンジン燃焼が終了し，放物線運動を始めた状態
class ModeFreefall : ModeBase {
  public:
    ModeFreefall()  {
      Reset();
    }

    OutputInfo Control(const SensorInfo sensors) override {
      OutputInfo outputs;
      const float delta_time = 1e-3 * (float)(sensors.time_ms - started_time_ms_);
      float top_time;

      top_detector_.Push(delta_time, sensors.height);
      top_time = top_detector_.GetHeighestTime();

      outputs.is_1st_servo_open = false;
      outputs.is_2nd_servo_open = false;

      if (OPEN_TIME_FROM_TOP_REACHED < delta_time - top_time) {
        // 予想最高高度到達時間からの経過時間による遷移
        outputs.next_mode = MODE_HALF_OPEN;
      } else if (OPEN_TIME_FROM_MODE_CHANGED < delta_time) {
        // 気圧センサの故障を考慮したタイマー条件での遷移
        outputs.next_mode = MODE_HALF_OPEN;
      } else {
        // まだ放物線運動をしている
        outputs.next_mode = MODE_FREEFALL;
      }

      return outputs;
    }

    void Reset() {
      top_detector_.Reset();
    }

  private:
    TopDetector top_detector_;
};

// 第1段階のサーボモータを開き，パラシュートが絞られた状態
class ModeHalfOpen : ModeBase {
  public:
    ModeHalfOpen() {
      Reset();
    }

    OutputInfo Control(const SensorInfo sensors) override {
      OutputInfo outputs;
      const float delta_time = 1e-3 * (float)(sensors.time_ms - started_time_ms_);
      const float height_from_ground = sensors.height - sensors.height_at_ground;

      outputs.is_1st_servo_open = true;
      outputs.is_2nd_servo_open = false;

      if (OPEN_ONLY_HALF_TIME < delta_time ) {
        // タイマー条件
        outputs.next_mode = MODE_FULL_OPEN;
      } else if ( height_from_ground < OPEN_HEIGHT_THRESHOLD ) {
        // 地上からの高度がOPEN_HEIGHT_THRESHOLD未満になったという条件
        outputs.next_mode = MODE_FULL_OPEN;
      } else {
        outputs.next_mode = MODE_HALF_OPEN;
      }

      return outputs;
    }

    void Reset() {

    }
  private:

};

// 第1段階，第2段階のサーボモータが開き，パラシュートが完全に開いた状態
class ModeFullOpen : ModeBase {
  public:
    ModeFullOpen() {
      Reset();
    }

    OutputInfo Control(const SensorInfo sensors) override {
      OutputInfo outputs;

      outputs.is_1st_servo_open = true;
      outputs.is_2nd_servo_open = true;
      outputs.next_mode = MODE_FULL_OPEN;

      return outputs;
    }

    void Reset() {

    }
  private:

};

#endif

