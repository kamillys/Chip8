#include "sdlvideo.h"
#include <cstdio>
#include <unistd.h>

constexpr int VIDEO_SCALE = 16;

Chip8::SDLVideo::SDLVideo() {
  int videoMode = SDL_SWSURFACE | SDL_DOUBLEBUF | SDL_ANYFORMAT;
  surface = SDL_SetVideoMode(64 * VIDEO_SCALE, 32 * VIDEO_SCALE, 8, videoMode);
  if (!surface) {
    std::fprintf(stderr, "Cannot create SDL video: %s\n", SDL_GetError());
    exit(1);
  }
  data =
      SDL_CreateRGBSurface(SDL_SWSURFACE, 64 * VIDEO_SCALE, 32 * VIDEO_SCALE,
                           32, surface->format->Rmask, surface->format->Gmask,
                           surface->format->Bmask, 0);
  if (!data) {
    std::fprintf(stderr, "Cannot create SDL video: %s\n", SDL_GetError());
    exit(1);
  }
  SDL_WM_SetCaption("SDL Test", "SDL Test");

  for (auto &row : m_screen)
    for (auto &bit : row)
      bit = false;
}

Chip8::SDLVideo::~SDLVideo() {}

void Chip8::SDLVideo::reset() {
  for (auto &row : m_screen)
    for (auto &bit : row)
      bit = false;
}

void Chip8::SDLVideo::show() {}

void Chip8::SDLVideo::update() {
  if (SDL_MUSTLOCK(data)) {
    SDL_LockSurface(data);
  }
  for (uint8_t y = 0; y < m_screen.size(); ++y) {
    for (uint8_t x = 0; x < m_screen[y].size(); ++x) {
      uint32_t value = 0x00000000;
      if (m_screen[y][x]) {
        value = 0xFFFFFF;
      }
      for (uint32_t Y = 0; Y < VIDEO_SCALE; ++Y)
        for (uint32_t X = 0; X < VIDEO_SCALE; ++X) {
          Uint8 *p = (Uint8 *)data->pixels +
                     (y * VIDEO_SCALE + Y) * data->pitch +
                     (x * VIDEO_SCALE + X) * data->format->BytesPerPixel;
          *p = value;
        }
    }
  }
  if (SDL_MUSTLOCK(data)) {
    SDL_UnlockSurface(data);
  }

  SDL_BlitSurface(data, 0, surface, 0);
  SDL_UpdateRect(surface, 0, 0, 0, 0);
}

bool Chip8::SDLVideo::flipSprite(uint8_t x, uint8_t y, uint8_t v) {
  bool rv = false;
  for (uint8_t it = 0; it < 8; ++it) {
    rv |= flipBit(x + it, y, (v & (1 << (7 - it))) >> (7 - it));
  }
  return rv;
}

bool Chip8::SDLVideo::flipBit(uint8_t x, uint8_t y, bool v) {
  y = y % m_screen.size();
  x = x % m_screen[y].size();
  bool oldv = m_screen[y][x];
  m_screen[y][x] ^= v;
  return oldv && !v;
}

void Chip8::SDLVideo::dump() {
  for (auto row : m_screen) {
    for (auto bit : row)
      std::printf("%c", bit ? '*' : '_');
    std::printf("\n");
  }
}
