#ifndef DATA_TRANSFER_HEADER_FILE
#define DATA_TRANSFER_HEADER_FILE

#include <Arduino.h>
#include <SoftwareSerial.h>

const size_t END_DATA_LENGTH = 3; // checkdigit,\r,\n

const uint8_t CR_LF[2] = {'\r', '\n'};

template <typename T>
class DataTransfer {
  public:
    // constructor
    // buffer_size: the size of buffer
    DataTransfer(uint16_t buff_margin) {
      buff_size_ = buff_margin + END_DATA_LENGTH + sizeof(T);
      buff_ = (uint8_t*)malloc(buff_size_ * sizeof(uint8_t));
      len_ = 0;
      parsed_result_ = false;
    }

    // destructor
    ~DataTransfer() {
      free(buff_);
    }

    // push a data to buffer
    // data: data which you want to push
    bool Encode(uint8_t c) {
      bool flag = false;

      if (len_ == buff_size_) {
        // buffer overflow
        Clear();
      } else {
        // save data to buffer
        buff_[len_] = c;
        len_++;

        if ( EndsWith(CR_LF, 2) ) {
          flag = Parse();
          Clear();
        }
      }

      return flag;
    }

    T GetResult() {
      return result_;
    }

    bool Parse() {
      int i0 = len_ - END_DATA_LENGTH - sizeof(T);
      uint8_t ch = 0;

      // received data is too few
      if (i0 < 0)return false;

      for (size_t i = 0 ; i < sizeof(T) ; i++) {
        ch = ch ^ buff_[i0 + i];
      }

      if ( ch != buff_[i0 + sizeof(T)])return false;

      result_ = *(T*)(buff_ + i0);
      parsed_result_ = true;

      return true;
    }

    void Send(HardwareSerial & serial, T d) {
      uint8_t* data_array = (uint8_t*)(&d);
      uint8_t ch = 0;

      for (size_t i = 0 ; i < sizeof(T) ; i++) {
        serial.write(data_array[i]);
        ch = ch ^ data_array[i];
      }
      serial.write(ch);
      serial.write(CR_LF[0]);
      serial.write(CR_LF[1]);
    }

    void Send(SoftwareSerial & serial, T d) {
      uint8_t* data_array = (uint8_t*)(&d);
      uint8_t ch = 0;

      for (size_t i = 0 ; i < sizeof(T) ; i++) {
        serial.write(data_array[i]);
        ch = ch ^ data_array[i];
      }
      serial.write(ch);
      serial.write(CR_LF[0]);
      serial.write(CR_LF[1]);
    }

    bool IsParsed() {
      return parsed_result_;
    }

    uint16_t GetLength() const {
      return len_;
    }

    bool IsOverflow()const {
      return len_ == buff_size_;
    }

    void Clear() {
      len_ = 0;
    }

    bool EndsWith(const uint8_t *end_data, const uint16_t end_length) const {
      if ( len_ < end_length )return false;

      for (uint16_t i = 0 ; i < end_length ; i++ ) {
        if ( end_data[i] != buff_[i + len_ - end_length])
          return false;
      }

      return true;
    }

  private:
    uint16_t len_;        // current length of data
    uint8_t *buff_;       // buffer
    uint16_t buff_size_;  // size of buffer
    T result_;            // the result of parse
    bool parsed_result_;  // if parsed then true
};

#endif

