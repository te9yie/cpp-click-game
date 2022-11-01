#pragma once
#include "t9/defer.h"
#include "task/event.h"

namespace core {

// Core.
struct Core {
  t9::Defer shutdown;
};

// AppEvent.
enum class AppEvent {
  Quit,
};

// init_core.
bool init_core(Core* c);

// handle_events.
void handle_events(task::EventSender<AppEvent> sender);

}  // namespace core