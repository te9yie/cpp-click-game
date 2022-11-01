#pragma once
#include "graphics/fwd.h"
#include "t9/defer.h"

namespace gui {

// Gui.
struct Gui {
  t9::Defer shutdown;
};

// init_gui.
bool init_gui(Gui* gui, const graphics::Renderer* r);

void handle_event_gui(Gui* gui);
void pre_update_gui(Gui* gui);
void post_update_gui(Gui* gui);

void render_gui(Gui* gui, const graphics::Renderer* r);

}  // namespace gui
