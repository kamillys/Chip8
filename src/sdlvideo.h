#pragma once

#include <Chip8/Video.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <array>

namespace Chip8 {

class SDLVideo : public Chip8::Video {
  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;
  SDL_Texture *texture = nullptr;

public:
  SDLVideo();
  ~SDLVideo();
  SDLVideo(const SDLVideo &) = delete;
  SDLVideo &operator=(const SDLVideo &) = delete;

  void update();
  void show();
};

} // namespace Chip8
