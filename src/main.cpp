#if defined(_MSC_VER)
#include <crtdbg.h>
#endif

#include <SDL.h>

#include <cstdlib>

#include "graphics/renderer.h"
#include "graphics/sprite.h"
#include "gui/gui.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"

int main(int /*argc*/, char* /*argv*/[]) {
#if defined(_MSC_VER)
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

  if (SDL_Init(SDL_INIT_TIMER) < 0) {
    SDL_LogCritical(SDL_LOG_CATEGORY_SYSTEM, "error: %s", SDL_GetError());
    return EXIT_FAILURE;
  }
  atexit(SDL_Quit);

  graphics::RendererConfig config;
  config.window_title = "Click Game";

  graphics::Renderer renderer;
  if (!init_renderer(&renderer, &config)) {
    return EXIT_FAILURE;
  }

  gui::Gui gui;
  if (!init_gui(&gui, &renderer)) {
    return EXIT_FAILURE;
  }

  graphics::Sprite sprite;
  sprite.rect = SDL_Rect{100, 100, 200, 50};
  sprite.material.color = graphics::Rgba{0xff, 0x00, 0x00};

  bool loop = true;
  while (loop) {
    {
      SDL_Event e;
      while (SDL_PollEvent(&e)) {
        ImGui_ImplSDL2_ProcessEvent(&e);
        if (e.type == SDL_QUIT) {
          loop = false;
        }
        if (e.type == SDL_WINDOWEVENT &&
            e.window.event == SDL_WINDOWEVENT_CLOSE &&
            e.window.windowID == SDL_GetWindowID(renderer.window.get())) {
          loop = false;
        }
      }
    }

    pre_update_gui(&gui);
    post_update_gui(&gui);

    begin_render(&renderer);

    render_sprite(renderer.renderer.get(), sprite);

    render_gui(&gui, &renderer);
    end_render(&renderer);
  }

  return EXIT_SUCCESS;
}
