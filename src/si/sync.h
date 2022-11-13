#pragma once
#include "noncopyable.h"

namespace si {

namespace sync_ {

struct DestroyMutex {
  void operator()(SDL_mutex* m) const { SDL_DestroyMutex(m); }
};

struct DestroyCondition {
  void operator()(SDL_cond* c) const { SDL_DestroyCond(c); }
};

}  // namespace sync_

using MutexPtr = std::unique_ptr<SDL_mutex, sync_::DestroyMutex>;
using ConditionPtr = std::unique_ptr<SDL_cond, sync_::DestroyCondition>;

// ScopedLock.
class ScopedLock final : public NonMovable {
 private:
  SDL_mutex* mutex_ = nullptr;

 public:
  explicit ScopedLock(SDL_mutex* m) : mutex_(m) { SDL_LockMutex(mutex_); }
  ~ScopedLock() { SDL_UnlockMutex(mutex_); }
};

}  // namespace si