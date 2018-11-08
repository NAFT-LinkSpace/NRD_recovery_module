#ifndef DATA_BUFFER_HEADER_FILE
#define DATA_BUFFER_HEADER_FILE
#include<inttypes.h>

class DataBuffer {
  public:

    // constructor
    // buffer_size: the size of buffer
    DataBuffer(size_t buff_size) {
      buff_ = (uint8_t*)malloc(buff_size * sizeof(uint8_t));
      buff_size_ = buff_size;
      len_ = 0;
    }

    // destructor
    ~DataBuffer() {
      free(buff_);
    }

    // push a data to buffer
    // data: data which you want to push
    void Push(uint8_t data) {
      if (len_ == buff_size_) {
        // buffer over flow
        return;
      } else {
        // save data to buffer
        buff_[len_] = data;
        len_++;
      }
    }

    uint16_t GetLength() const {
      return len_;
    }

    bool IsOverflow()const {
      return len_ == buff_size_;
    }

    bool StartsWith(const uint8_t *start_data, const size_t start_length) const {
      if ( len_ < start_length )return false;

      for (size_t i = 0 ; i < start_length ; i++ ) {
        if (start_data[i] != buff_[i])
          return false;
      }

      return true;
    }

    bool EndsWith(const uint8_t *end_data, const size_t end_length) const {
      if ( len_ < end_length )return false;

      for (size_t i = 0 ; i < end_length ; i++ ) {
        if ( end_data[i] != buff_[i + len_ - end_length])
          return false;
      }

      return true;
    }

    void Copy(uint8_t *copy_to) const {
      for (size_t i = 0 ; i < len_ ; i++ ) {
        copy_to[i] = buff_[i];
      }
      copy_to[len_] = 0;
    }

    void Clear() {
      len_ = 0;
    }

  private:
    uint16_t len_;
    uint8_t *buff_;
    uint16_t buff_size_;
};
#endif

