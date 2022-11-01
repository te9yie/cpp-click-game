#include "gui.h"

#include "graphics/renderer.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"

namespace gui {

// init_gui.
bool init_gui(Gui* gui, const graphics::Renderer* r) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGui_ImplSDL2_InitForSDLRenderer(r->window.get(), r->renderer.get());
  ImGui_ImplSDLRenderer_Init(r->renderer.get());

  gui->shutdown.func = []() {
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
  };
  return true;
}

// pre_update_gui.
void pre_update_gui(Gui* /*gui*/) {
  ImGui_ImplSDLRenderer_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();
}

// post_update_gui.
void post_update_gui(Gui* /*gui*/) { ImGui::Render(); }

// render_gui.
void render_gui(Gui* /*gui*/, const graphics::Renderer* /*r*/) {
  ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
}

}  // namespace gui
