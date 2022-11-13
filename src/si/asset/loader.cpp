#include "loader.h"

#include "../hash.h"

namespace si::asset {

/*explicit*/ Loader::Loader(AssetStorage* storage) : storage_(storage) {}

AssetHandle Loader::load(std::string_view path) {
  auto hash = fxhash(path.data(), path.length());
  auto r = assets_.equal_range(hash);
  for (auto it = r.first; it != r.second; ++it) {
    auto id = it->second;
    if (storage_->exists(id)) {
      return storage_->make_handle(id);
    }
  }
  auto asset = std::make_shared<Asset>(path);
  asset->load();
  auto handle = storage_->add(std::move(asset));
  assets_.emplace(hash, handle.id());
  return handle;
}

void Loader::apply_remove() { storage_->apply_remove(this); }

/*virtual*/ void Loader::on_remove(const HandleId& id,
                                   std::shared_ptr<Asset> p) /*override*/ {
  auto path = p->path();
  auto hash = fxhash(path.data(), path.length());
  auto r = assets_.equal_range(hash);
  for (auto it = r.first; it != r.second; ++it) {
    if (id == it->second) {
      assets_.erase(it);
      break;
    }
  }
}

}  // namespace si::asset
