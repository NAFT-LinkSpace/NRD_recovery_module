#ifndef LPF_HEADER_FILE
#define LPF_HEADER_FILE

#include <math.h>
//#include <malloc.h///>

// TODO: IIR
// http://ufcpp.net/study/sp/digital_filter/biquad/

class LPF {
  public:
    LPF(size_t n) {
      n_ = n;
      current_n_ = 0;
      index_ = 0;
      buff_ = (float*)malloc(n * sizeof(float));
    }

    ~LPF() {
      free(buff_);
    }

    float Push(float data) {
      float result = 0;
      if (current_n_ < n_) {
        buff_[current_n_] = data;
        current_n_++;
        index_ ++;
      } else {
        buff_[index_] = data;
        index_ ++;
      }

      index_ = index_ >= n_ ? 0 : index_;
      
      for(size_t i = 0 ; i < current_n_ ; i++){
        result += buff_[i];
      }

      return result / (float)(current_n_);
    }

  private:
    size_t n_;
    size_t current_n_;
    size_t index_;
    float *buff_;
};

#endif

