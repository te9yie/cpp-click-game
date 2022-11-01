#pragma once
#include <functional>

#include "noncopyable.h"

namespace t9 {

// Defer.
struct Defer final : NonCopyable {
  std::function<void(void)> func;

  ~Defer() {
    if (func) {
      func();
    }
  }
};

}  // namespace t9