#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>

#include <cstdlib>
#include <memory>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"

namespace {

struct DestroyWindow {
  void operator()(SDL_Window* w) const { SDL_DestroyWindow(w); }
};
using WindowPtr = std::unique_ptr<SDL_Window, DestroyWindow>;

struct DeleteContext {
  void operator()(SDL_GLContext c) const { SDL_GL_DeleteContext(c); }
};
using ContextPtr =
    std::unique_ptr<std::remove_pointer_t<SDL_GLContext>, DeleteContext>;

}  // namespace

int main(int /*argc*/, char* /*argv*/[]) {
  const char* TITLE = "Click Game";
  const int SCREEN_WIDTH = 4 * 200;
  const int SCREEN_HEIGHT = 3 * 200;

  if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) < 0) {
    SDL_LogCritical(SDL_LOG_CATEGORY_SYSTEM, "%s", SDL_GetError());
    return EXIT_FAILURE;
  }
  atexit(SDL_Quit);

  WindowPtr window(SDL_CreateWindow(
      TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
      SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL));
  if (!window) {
    SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "%s", SDL_GetError());
    return EXIT_FAILURE;
  }

  ContextPtr context(SDL_GL_CreateContext(window.get()));
  if (!context) {
    SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "%s", SDL_GetError());
    return EXIT_FAILURE;
  }
  SDL_GL_MakeCurrent(window.get(), context.get());
  SDL_GL_SetSwapInterval(1);

  if (auto r = glewInit(); r != GLEW_OK) {
    SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "%s", glewGetErrorString(r));
    return EXIT_FAILURE;
  }

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGui_ImplSDL2_InitForOpenGL(window.get(), context.get());
  ImGui_ImplOpenGL3_Init("#version 130");

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
    int w, h;
    SDL_GL_GetDrawableSize(window.get(), &w, &h);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowDemoWindow();

    ImGui::Render();

    glViewport(0, 0, w, h);
    glClearColor(0.12f, 0.34f, 0.56f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window.get());
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  return 0;
}