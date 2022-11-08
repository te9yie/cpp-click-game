#include "loader.h"

#include "t9/hash.h"

namespace asset {

/*explicit*/ Loader::Loader(AssetStorage* storage) : storage_(storage) {}

AssetHandle Loader::load(std::string_view path) {
  auto hash = t9::fxhash(path.data(), path.length());
  auto range = index_.equal_range(hash);
  for (auto it = range.first; it != range.second; ++it) {
    auto id = it->second;
    auto a = storage_->get(id);
    if (!a) continue;
    if (a->path() == path) return storage_->make_handle(id);
  }
  auto a = std::make_shared<Asset>(path);
  a->load();
  auto handle = storage_->add(std::move(a));
  index_.emplace(hash, handle.id());
  return handle;
}

void Loader::update() { storage_->update(this); }

/*virtual*/ void Loader::on_remove(const t9::HandleId& id,
                                   std::shared_ptr<Asset> p) /*override*/ {
  auto hash = t9::fxhash(p->path().data(), p->path().length());
  auto range = index_.equal_range(hash);
  for (auto it = range.first; it != range.second; ++it) {
    if (id == it->second) {
      index_.erase(it);
      break;
    }
  }
}

}  // namespace asset