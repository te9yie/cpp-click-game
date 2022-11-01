#pragma once

#include <SDL.h>

#define CHECK_SDL_VIDEO(exp)                                    \
  if ((exp) < 0) {                                              \
    SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "%s", SDL_GetError()); \
  }
