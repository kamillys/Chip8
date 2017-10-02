#pragma once

#include <Chip8/Video.h>
#include <SDL/SDL_video.h>
#include <array>

namespace Chip8 {

class SDLVideo : public Chip8::Video {
  SDL_Surface *surface = nullptr;
  SDL_Surface *data = nullptr;
  std::array<std::array<bool, 64>, 32> m_screen;

public:
  SDLVideo();
  ~SDLVideo();
  SDLVideo(const SDLVideo &) = delete;
  SDLVideo &operator=(const SDLVideo &) = delete;

  virtual void reset();
  virtual void show();
  virtual void update();
  virtual void clearScreen();
  virtual bool flipSprite(uint8_t x, uint8_t y, uint8_t v);
  virtual bool flipBit(uint8_t x, uint8_t y, bool v);
  virtual void dump();
};

} // namespace Chip8
