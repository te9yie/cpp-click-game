#include "renderer.h"

namespace graphics {

// init_renderer.
bool init_renderer(Renderer* r, const RendererConfig* config) {
  const RendererConfig default_config;
  if (!config) {
    config = &default_config;
  }

  if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
    SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "error: %s", SDL_GetError());
    return false;
  }
  r->shutdown.func = []() { SDL_QuitSubSystem(SDL_INIT_VIDEO); };

  WindowPtr window(
      SDL_CreateWindow(config->window_title.c_str(), SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, config->screen_width,
                       config->screen_height, config->window_flags));
  if (!window) {
    SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "error: %s", SDL_GetError());
    return false;
  }

  RendererPtr renderer(
      SDL_CreateRenderer(window.get(), -1, config->renderer_flags));
  if (!renderer) {
    SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "error: %s", SDL_GetError());
    return false;
  }

  r->window = std::move(window);
  r->renderer = std::move(renderer);

  return true;
}

// begin_render.
void begin_render(Renderer* r) {
  SDL_SetRenderDrawColor(r->renderer.get(), 0x12, 0x34, 0x56, 0xff);
  SDL_RenderClear(r->renderer.get());
}

// end_render.
void end_render(Renderer* r) { SDL_RenderPresent(r->renderer.get()); }

}  // namespace graphics
