// シリアル通信のバッファサイズ
const size_t SERIAL_BUFFER_SIZE = 64;
// シリアルう通信用のバッファ
DataBuffer serial_buffer(SERIAL_BUFFER_SIZE);

void SerialSetup() {
  Serial.begin(19200);
  serial_buffer.Clear();
}

void SerialReadCommand(SensorInfo *sensor) {
  char buff[SERIAL_BUFFER_SIZE];

  // とりあえず初期化
  sensor->cmd = RESULT_NONE;

  // シリアル通信からデータを読み取る
  while (Serial.available()) {
    const char c = Serial.read();
    serial_buffer.Push(c);

    if ( serial_buffer.EndsWith((uint8_t*)"\r\n", 2) ) {
      // 1行読み取る
      serial_buffer.Copy((uint8_t*)buff);

      // 文字データを解析する
      sensor->cmd = SerialParse(buff);
      serial_buffer.Clear();

      if (sensor->cmd != RESULT_NONE ) {
        // RESULT_NONE以外の命令が来た時はループを抜ける
        // →今きた命令を一度処理したあとで次の命令を処理する
        break;
      }
    } else if (serial_buffer.IsOverflow()) {
      serial_buffer.Clear();
    }
  }

  if ( sensor->cmd == RESULT_RESET ) {
    // リセット命令が来た場合はリセットする
    ResetMicrocontroller();
  }
}

enum CommandResult SerialParse(char *buff) {
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


