#include "ModeClasses.h"
#include "Utility.h"

// 現在の状態
enum RocketMode current_mode;
// モードクラス
ModeBase* modes[6];
// 地上の高度
float height_at_ground;

void ModeInit() {
  modes[0] = (ModeBase*)new ModeSetting();
  modes[1] = (ModeBase*)new ModeReady();
  modes[2] = (ModeBase*)new ModeBurning();
  modes[3] = (ModeBase*)new ModeFreefall();
  modes[4] = (ModeBase*)new ModeHalfOpen();
  modes[5] = (ModeBase*)new ModeFullOpen();

  // 初期状態MODE_SETTINGの設定
  ModeChange(MODE_SETTING, millis());
  height_at_ground = 0.0;
}

OutputInfo ModeLoop(const SensorInfo &sensors) {
  // MODE_SETTING,MODE_READYでは地上高度を随時更新
  if ( current_mode == MODE_SETTING || current_mode == MODE_READY )
    height_at_ground = sensors.height;

  // 出力を決定
  OutputInfo outputs = modes[(int)current_mode]->Control(sensors);

  // MODE_SETTINGに移行するように信号が送られてきた場合はnext_modeをそれに
  if ( sensors.cmd == RESULT_TO_SETTING ) {
    outputs.next_mode = MODE_SETTING;
  }

  // MODE_READYに移行するように信号が送られてきた場合はnext_modeをそれに
  if ( sensors.cmd == RESULT_TO_READY  ) {
    outputs.next_mode = MODE_READY;
  }

  // 状態が変化した場合はModeChangeを呼び出し次状態を初期化
  if ( outputs.next_mode != current_mode ) {
    ModeChange(outputs.next_mode, sensors.time_ms);
  }

  return outputs;
}

void ModeChange(enum RocketMode next_mode, uint32_t time_ms) {
  // 次の状態を初期化する
  modes[(size_t)next_mode]->Reset();
  modes[(size_t)next_mode]->SetStartedTime(time_ms);

  // 現在の状態を新しい物にする
  current_mode = next_mode;
}

enum RocketMode ModeGetCurrentMode(){
  return current_mode;
}

float ModeGetHeightAtGround(){
  return height_at_ground;
}

