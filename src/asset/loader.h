#pragma once
#include "asset.h"
#include "t9/noncopyable.h"

namespace asset {

// Loader.
class Loader : private t9::NonCopyable,
               private t9::HandleRemoveObserver<Asset> {
 private:
  AssetStorage* storage_ = nullptr;
  std::unordered_multimap<std::uint32_t, t9::HandleId> index_;

 public:
  explicit Loader(AssetStorage* storage);

  AssetHandle load(std::string_view path);

  void update();

 private:
  virtual void on_remove(const t9::HandleId& id,
                         std::shared_ptr<Asset> p) override;
};

}  // namespace asset