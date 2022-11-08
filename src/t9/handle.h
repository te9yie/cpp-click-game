#pragma once
#include "noncopyable.h"

namespace t9 {

// HandleId.
struct HandleId {
  std::size_t revision = 0;
  std::size_t index = 0;

  bool is_valid() const { return revision > 0; }

  bool operator==(const HandleId& rhs) const {
    return revision == rhs.revision && index == rhs.index;
  }
  bool operator!=(const HandleId& rhs) const { return !operator==(rhs); }
};

// HandleObserver.
class HandleObserver {
 public:
  virtual ~HandleObserver() = default;
  virtual void on_grab(const HandleId& id) = 0;
  virtual void on_drop(const HandleId& id) = 0;
};

// Handle.
template <typename T>
class Handle final {
 private:
  HandleId id_;
  HandleObserver* observer_ = nullptr;

 public:
  Handle() = default;
  Handle(const HandleId& id, HandleObserver* ob) : id_(id), observer_(ob) {
    if (observer_) {
      observer_->on_grab(id_);
    }
  }
  Handle(const Handle& handle) : Handle(handle.id_, handle.observer_) {}
  Handle(Handle&& handle) : id_(handle.id_), observer_(handle.observer_) {
    handle.id_ = HandleId{};
    handle.observer_ = nullptr;
  }
  ~Handle() {
    if (observer_) {
      observer_->on_drop(id_);
    }
  }

  const HandleId& id() const { return id_; }
  explicit operator bool() const { return id_.is_valid(); }

  Handle& operator=(const Handle& rhs) {
    Handle(rhs).swap(*this);
    return *this;
  }
  Handle& operator=(Handle&& rhs) {
    Handle(std::move(rhs)).swap(*this);
    return *this;
  }

  void swap(Handle& rhs) {
    using std::swap;
    swap(id_, rhs.id_);
    swap(observer_, rhs.observer_);
  }
};

template <typename T>
inline void swap(Handle<T>& lhs, Handle<T>& rhs) {
  lhs.swap(rhs);
}

// HandleEntry.
template <typename T>
struct HandleEntry {
  std::atomic_int ref_count = 0;
  std::size_t revision = 1;
  std::shared_ptr<T> x;
};

// HandleRemoveObserver.
template <typename T>
class HandleRemoveObserver {
 public:
  virtual ~HandleRemoveObserver() = default;
  virtual void on_remove(const HandleId& id, std::shared_ptr<T> p) = 0;
};

// HandleStorage
template <typename T>
class HandleStorage : private NonCopyable, private HandleObserver {
 private:
  std::size_t peak_index_ = 0;
  std::deque<std::size_t> index_stock_;
  std::deque<HandleEntry<T>> entries_;
  std::deque<HandleId> remove_ids_;
  std::mutex remove_mutex_;

 public:
  Handle<T> add(std::shared_ptr<T> x) {
    auto index = find_index_();
    auto& entry = entries_[index];
    HandleId id{entry.revision, index};
    entry.x = std::move(x);
    return Handle<T>(id, this);
  }

  void update(HandleRemoveObserver<T>* observer = nullptr) {
    std::unique_lock lock(remove_mutex_);
    for (auto id : remove_ids_) {
      assert(id.index < entries_.size());
      auto& entry = entries_[id.index];
      if (entry.revision != id.revision) continue;
      if (entry.ref_count > 0) continue;
      if (observer) {
        observer->on_remove(id, entry.x);
      }
      entry.revision = std::max<std::size_t>(entry.revision + 1, 1);
      entry.x.reset();
      index_stock_.emplace_back(id.index);
    }
    remove_ids_.clear();
  }

  Handle<T> make_handle(const HandleId& id) {
    assert(id.index < entries_.size());
    auto& entry = entries_[id.index];
    if (id.revision != entry.revision) return Handle<T>{};
    return Handle<T>(id, this);
  }

  const T* get(const HandleId& id) const {
    assert(id.index < entries_.size());
    auto& entry = entries_[id.index];
    if (id.revision != entry.revision) return nullptr;
    return entry.x.get();
  }
  T* get_mut(const HandleId& id) const {
    assert(id.index < entries_.size());
    auto& entry = entries_[id.index];
    if (id.revision != entry.revision) return nullptr;
    return entry.x.get();
  }

  bool exists(const HandleId& id) const {
    assert(id.index < entries_.size());
    auto& entry = entries_[handle.id.index];
    return id.revision == entry.revision;
  }

  template <typename F>
  void each(F f) const {
    std::for_each(entries_.begin(), entries_.end(),
                  [f](const HandleEntry<T>& e) {
                    if (e.x) {
                      f(e.x.get());
                    }
                  });
  }

 private:
  std::size_t find_index_() {
    if (index_stock_.empty()) {
      auto index = peak_index_++;
      entries_.emplace_back();
      return index;
    } else {
      auto index = index_stock_.front();
      index_stock_.pop_front();
      return index;
    }
  }

 private:
  // HandleObserver.
  virtual void on_grab(const HandleId& id) override {
    if (id.revision == 0) return;
    if (entries_.size() <= id.index) return;
    auto& entry = entries_[id.index];
    if (id.revision != entry.revision) return;
    ++entry.ref_count;
  }
  virtual void on_drop(const HandleId& id) override {
    if (id.revision == 0) return;
    if (entries_.size() <= id.index) return;
    auto& entry = entries_[id.index];
    if (id.revision != entry.revision) return;
    assert(entry.ref_count > 0);
    if (--entry.ref_count > 0) return;

    std::unique_lock lock(remove_mutex_);
    remove_ids_.emplace_back(id);
  }
};

}  // namespace t9