#pragma once
#include "t9/handle.h"
#include "t9/noncopyable.h"

namespace asset {

// Asset.
class Asset : private t9::NonCopyable {
 private:
  std::string path_;
  std::vector<std::uint8_t> data_;

 public:
  Asset(std::string_view path);

  bool load();

  std::string_view path() const { return path_; }
  const std::vector<std::uint8_t>& data() const { return data_; }
};

using AssetHandle = t9::Handle<Asset>;
using AssetStorage = t9::HandleStorage<Asset>;

}  // namespace asset