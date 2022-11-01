#pragma once
#include <string>

#include "config.h"
#include "t9/defer.h"

namespace graphics {

// WindowPtr.
struct DestroyWindow {
  void operator()(SDL_Window* w) const { SDL_DestroyWindow(w); }
};
using WindowPtr = std::unique_ptr<SDL_Window, DestroyWindow>;

// RendererPtr.
struct DestroyRenderer {
  void operator()(SDL_Renderer* r) const { SDL_DestroyRenderer(r); }
};
using RendererPtr = std::unique_ptr<SDL_Renderer, DestroyRenderer>;

// Renderer.
struct Renderer {
  t9::Defer shutdown;
  WindowPtr window;
  RendererPtr renderer;
};

// RendererConfig.
struct RendererConfig {
  std::string window_title = "Game";
  int screen_width = 4 * 160;
  int screen_height = 3 * 160;
  Uint32 window_flags = 0;
  Uint32 renderer_flags = SDL_RENDERER_ACCELERATED;
};

// init_renderer.
bool init_renderer(Renderer* r, const RendererConfig* config);

void begin_render(Renderer* r);
void end_render(Renderer* r);

}  // namespace graphics
