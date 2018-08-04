// シリアル通信のバッファサイズ
const size_t SERIAL_BUFFER_SIZE = 64;
// シリアルう通信用のバッファ
DataBuffer serial_buffer(SERIAL_BUFFER_SIZE);

void SerialSetup() {
  Serial.begin(19200);
}

void SerialReadCommand(SensorInfo *sensor) {
  char buff[SERIAL_BUFFER_SIZE];

  // とりあえず全部初期化
  sensor->command_1st_servo_change = false;
  sensor->command_2nd_servo_change = false;
  sensor->command_to_setting = false;
  sensor->command_to_ready = false;

  // シリアル通信からデータを読み取る
  while (Serial.available()) {
    serial_buffer.Push(Serial.read());

    if ( serial_buffer.EndsWith((uint8_t*)"\r\n", 2) ) {
      // 1行読み取る
      serial_buffer.Copy((uint8_t*)buff);

      // 文字データを解析する
      enum IM920Result ret = SerialParse(buff);

      // sensorの値を変えたりリセット命令が来た場合はリセットする
      if ( ret == RESULT_TO_SETTING ) {
        sensor->command_to_setting = true;
      } else if ( ret == RESULT_TO_READY ) {
        sensor->command_to_ready = true;
      } else if ( ret == RESULT_RESET ) {
        ResetMicrocontroller();
      } else if ( ret == RESULT_CHANGE_1ST ) {
        sensor->command_1st_servo_change = true;
      } else if ( ret == RESULT_CHANGE_2ND ) {
        sensor->command_2nd_servo_change = true;
      }

      serial_buffer.Clear();
    } else if (serial_buffer.IsOverflow()) {
      serial_buffer.Clear();
    }
  }
}

enum IM920Result SerialParse(char *buff) {
  if ( strcmp(buff, "TO_SETTING\r\n") == 0 )
    return RESULT_TO_SETTING;
  else if ( strcmp(buff, "TO_READY\r\n") == 0 )
    return RESULT_TO_READY;
  else if ( strcmp(buff, "RESET\r\n") == 0 )
    return RESULT_RESET;
  else if ( strcmp(buff, "CHANGE_1ST\r\n") == 0 )
    return RESULT_CHANGE_1ST;
  else if ( strcmp(buff, "CHANGE_2ND\r\n") == 0 )
    return RESULT_CHANGE_2ND;
  else
    return RESULT_NONE;
}


