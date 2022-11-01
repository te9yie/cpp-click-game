#include "sprite.h"

namespace graphics {

bool render_sprite(SDL_Renderer* r, const Sprite& s) {
  const auto& m = s.material;
  CHECK_SDL_VIDEO(
      SDL_SetRenderDrawColor(r, m.color.r, m.color.g, m.color.b, m.color.a));
  if (m.wireframe) {
    CHECK_SDL_VIDEO(SDL_RenderDrawRect(r, &s.rect));
  } else {
    CHECK_SDL_VIDEO(SDL_RenderFillRect(r, &s.rect));
  }
  return true;
}

}  // namespace graphics
