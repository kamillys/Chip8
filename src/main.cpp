#include <Chip8/Board.h>
#include <Chip8/Cpu.h>
#include <Chip8/Memory.h>
#include <Chip8/Video.h>

#include "debugger.h"
#include "sdlvideo.h"

#include <SDL/SDL.h>
#include <csignal>
#include <cstdio>
#include <fstream>
#include <map>
#include <memory>

namespace {

const std::map<SDLKey, std::uint16_t> kKeyMap = {
    {SDLK_1, 0x1}, {SDLK_2, 0x2}, {SDLK_3, 0x3}, {SDLK_4, 0xC},

    {SDLK_q, 0x4}, {SDLK_w, 0x5}, {SDLK_e, 0x6}, {SDLK_r, 0xD},

    {SDLK_a, 0x7}, {SDLK_s, 0x8}, {SDLK_d, 0x9}, {SDLK_f, 0xE},

    {SDLK_z, 0xA}, {SDLK_x, 0x0}, {SDLK_c, 0xB}, {SDLK_v, 0xF},
};

std::vector<uint8_t> LoadFile(const char *file) {
  std::ifstream f{file};
  if (!f) {
    std::fprintf(stderr, "Unable open file %s\n", file);
    return {};
  }
  return std::vector<uint8_t>((std::istreambuf_iterator<char>(f)),
                              std::istreambuf_iterator<char>());
}

volatile std::sig_atomic_t debugger_enabled = false;

void signal_handler(int signal_no) {
  if (SIGINT == signal_no) {
    debugger_enabled = true;
  }
}

int main_loop(const char *file) {
  auto binaryBlob = LoadFile(file);
  if (0 == binaryBlob.size()) {
    std::fprintf(stderr, "File not found or empty file\n");
    return 1;
  }
  auto video = std::make_shared<Chip8::SDLVideo>();
  video->show();
  // Initialize Board
  auto board = std::make_shared<Chip8::Board>(video);
  board->LoadBinary(binaryBlob);

  // Initialize debugger
  auto debugger = std::make_shared<Chip8::Debugger>(board);

  Uint32 last_timer_tick = SDL_GetTicks();
  Uint32 last_gpu_tick = last_timer_tick;

  /* message pump */
  int should_quit = 0;
  SDL_Event event;
  while (!should_quit) {
    /* look for an event */
    if (SDL_PollEvent(&event)) {
      switch (event.type) {
      /* close button clicked */
      case SDL_QUIT:
        should_quit = 1;
        break;

      /* handle the keyboard */
      case SDL_KEYDOWN:
      case SDL_KEYUP: {
        if (SDLK_ESCAPE == event.key.keysym.sym) {
          should_quit = 1;
          break;
        }
        auto keyEntry = kKeyMap.find(event.key.keysym.sym);
        if (keyEntry != kKeyMap.end()) {
          board->handleKey(keyEntry->second, SDL_KEYDOWN == event.type);
        }
        break;
      }
      default:
        break; // Do not handle
      }
    }

    // TODO: Compute to be exact 60 Hz. Now is 62.5 Hz
    Uint32 next_timer_tick = SDL_GetTicks();
    if (next_timer_tick - last_timer_tick > 16) {
      last_timer_tick = next_timer_tick;
      /* Update board timers */
      board->timerStep();
    }
    Uint32 next_gpu_tick = next_timer_tick;
    if (next_gpu_tick - last_gpu_tick > 8) {
      /* update the screen */
      video->update();
    }

    if (debugger_enabled) {
      board->setBreak(true);
      debugger_enabled = false;
    }
    if (board->isBreak()) {
      std::fprintf(stderr, "Debugger enabled\n");
      debugger->debugger_loop();
    }

    if (!board->shutdown()) {
      board->step();
    } else {
      should_quit = 1;
    }
  }
}

} // namespace

int main(int argc, char **argv) {
  if (argc < 2) {
    std::fprintf(stderr, "Usage %s [FILE_PATH]\n", argv[0]);
    return 1;
  }
  int rv;
  // Initialize SDL
  // TODO: Consider SDL_INIT_NOPARACHUTE
  rv = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER |
                SDL_INIT_NOPARACHUTE);
  if (0 != rv) {
    std::fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
    return 1;
  }
  sighandler_t srv = signal(SIGINT, signal_handler);
  if (SIG_ERR == srv) {
    std::fprintf(stderr, "Unable to setup SIGINT handler: %s\n",
                 strerror(errno));
    SDL_Quit();
    return 1;
  }

  main_loop(argv[1]);
  SDL_Quit();
  return 0;
}
