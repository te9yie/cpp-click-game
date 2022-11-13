#pragma once
#include "asset.h"

namespace si::asset {

// Loader.
class Loader : public AssetRemoveObserver {
 private:
  AssetStorage* storage_ = nullptr;
  std::unordered_multimap<std::uint32_t, HandleId> assets_;

 public:
  explicit Loader(AssetStorage* storage);

  AssetHandle load(std::string_view path);

  void apply_remove();

 public:
  virtual void on_remove(const HandleId& id, std::shared_ptr<Asset> p) override;
};

}  // namespace si::asset