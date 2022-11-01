#pragma once
#include "config.h"
#include "material.h"

namespace graphics {

// Sprite.
struct Sprite {
  SDL_Rect rect;
  Material material;
};

// render_sprite.
bool render_sprite(SDL_Renderer* r, const Sprite& s);

}  // namespace graphics