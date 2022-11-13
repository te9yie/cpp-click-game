
#include "asset.h"

namespace si::asset {

/*explicit*/ Asset::Asset(std::string_view path) : path_(path) {}

void Asset::load() {
  state_ = State::LOADING;
  error_ = ErrorCode::OK;
  if (auto f = SDL_RWFromFile(path_.c_str(), "rb"); f) {
    auto size = SDL_RWsize(f);
    data_.resize(size);
    Sint64 total = 0;
    while (total < size) {
      auto n = SDL_RWread(f, data_.data() + total, 1, size - total);
      if (n == 0) break;
      total += n;
    }
    if (total < size) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Asset::load: %s",
                   SDL_GetError());
      error_ = ErrorCode::LOADING_ERROR;
    }
  } else {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Asset::load: %s",
                 SDL_GetError());
    error_ = ErrorCode::NOT_FOUND;
  }
  state_ = State::LOADED;
}

}  // namespace si::asset