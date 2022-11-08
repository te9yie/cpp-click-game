#include "asset.h"

namespace asset {

Asset::Asset(std::string_view path) : path_(path) {}

bool Asset::load() {
  auto f = SDL_RWFromFile(path_.c_str(), "rb");
  if (!f) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "load_asset: %s",
                 SDL_GetError());
    return false;
  }
  auto len = SDL_RWsize(f);
  data_.resize(len);
  Sint64 i = 0;
  while (i < len) {
    auto n = SDL_RWread(f, data_.data() + i, 1, len - i);
    if (n == 0) break;
    i += n;
  }
  SDL_RWclose(f);
  return i == len;
}

}  // namespace asset