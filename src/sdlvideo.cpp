#include "sdlvideo.h"
#include <cstdio>
#include <unistd.h>

namespace Chip8 {

constexpr int VIDEO_SCALE = 16;

SDLVideo::SDLVideo() {
  window = SDL_CreateWindow("Chip8 emulator", SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, 64 * VIDEO_SCALE,
                            32 * VIDEO_SCALE, SDL_WINDOW_HIDDEN);
  if (!window) {
    std::fprintf(stderr, "Cannot create SDL video: %s\n", SDL_GetError());
    exit(1);
  }
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    std::fprintf(stderr, "Cannot create SDL renderer: %s\n", SDL_GetError());
    exit(1);
  }
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                              SDL_TEXTUREACCESS_TARGET, 64, 32);
  if (!texture) {
    std::fprintf(stderr, "Cannot create SDL texture: %s\n", SDL_GetError());
    exit(1);
  }

  for (auto &row : m_screen)
    for (auto &bit : row)
      bit = false;
}

SDLVideo::~SDLVideo() {}

void SDLVideo::update() {
  for (uint8_t y = 0; y < m_screen.size(); ++y) {
    for (uint8_t x = 0; x < m_screen[y].size(); ++x) {
      auto &val = m_ledBuffer[y][x];
      constexpr uint8_t showDiffVal = 0x55;
      constexpr uint8_t hideDiffVal = 0x15;
      if (m_screen[y][x]) {
        if (val < 0xFF - showDiffVal)
          val += showDiffVal;
        else
          val = 0xFF;
      } else {
        if (val > hideDiffVal)
          val -= hideDiffVal;
        else
          val = 0;
      }
    }
  }

  SDL_SetRenderTarget(renderer, texture);
  // SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
  // SDL_RenderClear(renderer);
  for (uint8_t y = 0; y < m_screen.size(); ++y) {
    for (uint8_t x = 0; x < m_screen[y].size(); ++x) {
      auto val = m_ledBuffer[y][x];
      SDL_SetRenderDrawColor(renderer, val, val, val, 0xff);
      SDL_RenderDrawPoint(renderer, x, y);
    }
  }

  SDL_SetRenderTarget(renderer, NULL);
  // SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
  // SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);
}

void SDLVideo::show() { SDL_ShowWindow(window); }
}
