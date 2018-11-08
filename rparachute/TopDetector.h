#ifndef TOP_DETECTOR_HEADER_FILE
#define TOP_DETECTOR_HEADER_FILE

#include <math.h>

#define GRAVITY_ACCEL   (9.8)
#define SOLVE_N_DATA    (20)
#define MAX_N_DATA      (64)

class TopDetector {
  public:
    // コンストラクタ
    TopDetector() {
      Reset();
    }

    // 変数の初期化を行うメソッド
    void Reset() {
      x_[0] = x_[1] = 0;
      b_[0] = b_[1] = 0;
      A_[0][0] = A_[0][1] = 0;
      A_[1][0] = A_[1][1] = 0;
      n_data_ = 0;
    }

    // データを追加するメソッド
    void Push(float t, float y) {
      const float t0 = 1;
      const float t1 = t0 * t;
      const float t2 = t1 * t;
      const float t3 = t2 * t;

      // ある程度沢山データが集まった場合はA,bを減衰させる
      // すなわち新しいデータを重視させる
      if ( n_data_ == MAX_N_DATA ) {
        const float decay = (float)(MAX_N_DATA - 1) / (float(MAX_N_DATA));

        A_[0][0] *= decay;
        A_[0][1] *= decay;
        A_[1][0] *= decay;
        A_[1][1] *= decay;

        b_[0] *= decay;
        b_[1] *= decay;

      } else {
        n_data_++;
      }

      A_[0][0] += t2;
      A_[0][1] += t1;

      A_[1][0] += t1;
      A_[1][1] += t0;

      b_[0] += t1 * y + 0.5 * GRAVITY_ACCEL * t3;
      b_[1] += t0 * y + 0.5 * GRAVITY_ACCEL * t2;


      if ( SOLVE_N_DATA <= n_data_ ) {
        Calc();
      }
    }

    // データが有れば放物線の係数を計算するメソッド
    void Calc() {
      float detA;

      if ( n_data_ == 0 )
        return;

      detA = A_[0][0] * A_[1][1] - A_[0][1] * A_[1][0];
      x_[0] = (+b_[0] * A_[1][1] - b_[1] * A_[0][1]) / detA;
      x_[1] = (-b_[0] * A_[1][0] + b_[1] * A_[1][1]) / detA;
    }

    // 追加されたデータ数を返すメソッド
    int GetDataNumber() {
      return n_data_;
    }

    // 予測された最高高度到達時間[s]
    // 未計算の場合は1e10を返す
    float GetHeighestTime() {
      if ( x_[1] == 0.0 )
        return 1e+10;
      else
        return x_[0] / GRAVITY_ACCEL;
    }

  private:

    float A_[2][2];     // 係数行列
    float b_[2];        // 出力ベクトル
    float x_[2];        // 解ベクトル

    int n_data_;        // 測定個数
};

#endif

