#if defined(_MSC_VER)
#include <crtdbg.h>
#endif

#include <SDL.h>

#include <cstdlib>

#include "core/core.h"
#include "graphics/renderer.h"
#include "graphics/sprite.h"
#include "gui/gui.h"

int main(int /*argc*/, char* /*argv*/[]) {
#if defined(_MSC_VER)
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

  core::Core core;
  init_core(&core);

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

  task::Event<core::AppEvent> events;
  std::size_t recv_index = 0;
  task::EventReceiver<core::AppEvent> receiver{&events, &recv_index};

  bool loop = true;
  while (loop) {
    update_events(&events);

    handle_events(task::EventSender<core::AppEvent>{&events});

    pre_update_gui(&gui);
    post_update_gui(&gui);

    begin_render(&renderer);

    render_sprite(renderer.renderer.get(), sprite);

    render_gui(&gui, &renderer);
    end_render(&renderer);

    receiver.each([&](core::AppEvent e) {
      if (e == core::AppEvent::Quit) {
        loop = false;
      }
    });
  }

  return EXIT_SUCCESS;
}
