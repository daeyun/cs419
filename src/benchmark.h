//
// Created by daeyun on 4/11/17.
//

#ifndef CS419_CPP_OS_UTILS_H
#define CS419_CPP_OS_UTILS_H

#include <chrono>

namespace render {
long MilliSecondsSinceEpoch();
long NanoSecondsSinceEpoch();

class Timer {
 public:
  Timer() {
    Reset();
  }

  void Tic() {
    last_tic_ = std::chrono::high_resolution_clock::now();
  }

  void Toc() {
    auto now = std::chrono::high_resolution_clock::now();
    cumulative_duration_ = cumulative_duration_ + (now - last_tic_);
    ++count_;
  }

  long TotalElapsedMilliSeconds() const {
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(cumulative_duration_);
    return ms.count();
  }

  long MeanElapsedMilliSeconds() const {
    return TotalElapsedMilliSeconds() / count_;
  }

  void Reset() {
    Tic();
    cumulative_duration_ = std::chrono::duration<unsigned long>::zero();
    count_ = 0;
  }

 private:
  std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds> last_tic_;
  std::chrono::duration<unsigned long, std::nano> cumulative_duration_;
  unsigned long count_;
};

}

#endif //CS419_CPP_OS_UTILS_H
