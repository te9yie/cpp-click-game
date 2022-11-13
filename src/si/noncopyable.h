#pragma once

namespace si {

namespace noncopyable_ {

// NonCopyable.
class NonCopyable {
 private:
  NonCopyable(const NonCopyable&) = delete;
  NonCopyable& operator=(const NonCopyable&) = delete;

 public:
  NonCopyable() = default;
};

// NonMovable.
class NonMovable {
 private:
  NonMovable(const NonMovable&) = delete;
  NonMovable& operator=(const NonMovable&) = delete;
  NonMovable(NonMovable&&) = delete;
  NonMovable& operator=(NonMovable&&) = delete;

 public:
  NonMovable() = default;
};

}  // namespace noncopyable_

using NonCopyable = noncopyable_::NonCopyable;
using NonMovable = noncopyable_::NonMovable;

}  // namespace si