#include <Chip8/Board.h>
#include <Chip8/Cpu.h>
#include <Chip8/Memory.h>
#include <Chip8/Video.h>

#include "debugger.h"
#include "sdlvideo.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_keyboard.h>
#include <csignal>
#include <cstdio>
#include <fstream>
#include <map>
#include <memory>

namespace {

const std::map<SDL_Scancode, std::uint16_t> kKeyMap = {
    {SDL_SCANCODE_1, 0x1}, {SDL_SCANCODE_2, 0x2},
    {SDL_SCANCODE_3, 0x3}, {SDL_SCANCODE_4, 0xC},

    {SDL_SCANCODE_Q, 0x4}, {SDL_SCANCODE_W, 0x5},
    {SDL_SCANCODE_E, 0x6}, {SDL_SCANCODE_R, 0xD},

    {SDL_SCANCODE_A, 0x7}, {SDL_SCANCODE_S, 0x8},
    {SDL_SCANCODE_D, 0x9}, {SDL_SCANCODE_F, 0xE},

    {SDL_SCANCODE_Z, 0xA}, {SDL_SCANCODE_X, 0x0},
    {SDL_SCANCODE_C, 0xB}, {SDL_SCANCODE_V, 0xF},
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
  // Initialize Board
  auto board = std::make_shared<Chip8::Board>(video);
  board->LoadBinary(binaryBlob);

  // Initialize debugger
  auto debugger = std::make_shared<Chip8::Debugger>(board);

  Uint32 last_timer_tick = SDL_GetTicks();
  Uint32 last_board_tick = last_timer_tick;

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
        if (SDL_SCANCODE_ESCAPE == event.key.keysym.scancode) {
          should_quit = 1;
          break;
        }
        auto keyEntry = kKeyMap.find(event.key.keysym.scancode);
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
      video->update();
      board->step();
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
      // Board should tick at 50 Hz rate
      if (next_timer_tick - last_board_tick > 20) {
        last_board_tick = next_timer_tick;
        board->step();
      }
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
