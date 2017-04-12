//
// Created by daeyun on 4/11/17.
//

#include <chrono>

#include "benchmark.h"

namespace render {
long MilliSecondsSinceEpoch() {
  return std::chrono::duration_cast<std::chrono::milliseconds>
      (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

long NanoSecondsSinceEpoch() {
  return std::chrono::duration_cast<std::chrono::nanoseconds>
      (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}
}