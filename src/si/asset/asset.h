#pragma once
#include "../handle.h"
#include "../noncopyable.h"

namespace si::asset {

// Asset.
class Asset : public NonCopyable {
 public:
  enum class State {
    NONE,
    LOADING,
    LOADED,
  };
  enum class ErrorCode {
    OK,
    NOT_FOUND,
    LOADING_ERROR,
  };

 private:
  std::string path_;
  std::vector<std::uint8_t> data_;
  State state_ = State::NONE;
  ErrorCode error_ = ErrorCode::OK;

 public:
  explicit Asset(std::string_view path);

  void load();

  std::string_view path() const { return path_; }
  const std::vector<std::uint8_t>& data() const { return data_; }
  State state() const { return state_; }
  bool is_ready() const {
    return state_ == State::LOADED && error_ == ErrorCode::OK;
  }
  ErrorCode err() const { return error_; }
};

using AssetHandle = Handle<Asset>;
using AssetRemoveObserver = HandleRemoveObserver<Asset>;
using AssetStorage = HandleStorage<Asset>;

}  // namespace si::asset