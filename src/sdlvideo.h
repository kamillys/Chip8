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
  // There should be actual pixel data stored
  std::array<std::array<bool, 64>, 32> m_screen;
  // To implement pixel fadeout, store current value in buffer.
  // Updated in update() method
  std::array<std::array<uint8_t, 64>, 32> m_ledBuffer;

public:
  SDLVideo();
  ~SDLVideo();
  SDLVideo(const SDLVideo &) = delete;
  SDLVideo &operator=(const SDLVideo &) = delete;

  virtual void reset();
  virtual void update();
  void show();
  virtual void clearScreen();
  virtual bool flipSprite(uint8_t x, uint8_t y, uint8_t v);
  virtual bool flipBit(uint8_t x, uint8_t y, bool v);
  virtual void dump();
};

} // namespace Chip8
