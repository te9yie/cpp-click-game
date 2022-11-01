#include "core.h"

#include <SDL.h>

#include "imgui_impl_sdl.h"

namespace core {

// init_core.
bool init_core(Core* c) {
  if (SDL_Init(SDL_INIT_TIMER) < 0) {
    SDL_LogCritical(SDL_LOG_CATEGORY_SYSTEM, "error: %s", SDL_GetError());
    return false;
  }

  c->shutdown.func = []() { SDL_Quit(); };
  return true;
}

// handle_events.
void handle_events(task::EventSender<AppEvent> sender) {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    ImGui_ImplSDL2_ProcessEvent(&e);
    if (e.type == SDL_QUIT) {
      sender.send(AppEvent::Quit);
    }
  }
}

}  // namespace core