#include "si/asset/loader.h"

namespace {

struct DestroyWindow {
  void operator()(SDL_Window* w) const { SDL_DestroyWindow(w); }
};
using WindowPtr = std::unique_ptr<SDL_Window, DestroyWindow>;

struct DestroyRenderer {
  void operator()(SDL_Renderer* r) const { SDL_DestroyRenderer(r); }
};
using RendererPtr = std::unique_ptr<SDL_Renderer, DestroyRenderer>;

}  // namespace

int main(int /*argc*/, char* /*argv*/[]) {
#if defined(_MSC_VER)
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

  si::asset::AssetStorage assets;
  si::asset::Loader loader(&assets);
  { auto f = loader.load("test.cpp"); }
  loader.apply_remove();

  const char* TITLE = "Click Game";
  const int SCREEN_WIDTH = 16 * 60;
  const int SCREEN_HEIGHT = 9 * 60;

  if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) < 0) {
    SDL_LogCritical(SDL_LOG_CATEGORY_SYSTEM, "error: %s", SDL_GetError());
    return EXIT_FAILURE;
  }
  atexit(SDL_Quit);

  WindowPtr window(SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                                    SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE));
  if (!window) {
    SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "error: %s", SDL_GetError());
    return EXIT_FAILURE;
  }

  RendererPtr renderer(SDL_CreateRenderer(
      window.get(), -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED));
  if (!renderer) {
    SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "error: %s", SDL_GetError());
    return EXIT_FAILURE;
  }

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGui_ImplSDL2_InitForSDLRenderer(window.get(), renderer.get());
  ImGui_ImplSDLRenderer_Init(renderer.get());

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
            e.window.windowID == SDL_GetWindowID(window.get())) {
          loop = false;
        }
      }
    }

    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();

    ImGui::NewFrame();

    ImGui::ShowDemoWindow();

    ImGui::Render();

    SDL_SetRenderDrawColor(renderer.get(), 0x12, 0x34, 0x56, 0xff);
    SDL_RenderClear(renderer.get());

    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

    SDL_RenderPresent(renderer.get());
  }

  ImGui_ImplSDLRenderer_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  return EXIT_SUCCESS;
}