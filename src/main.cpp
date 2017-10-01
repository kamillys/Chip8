#include <Chip8/Board.h>
#include <Chip8/Cpu.h>
#include <Chip8/Memory.h>
#include <Chip8/Video.h>

#include "sdlvideo.h"
#include "debugger.h"

#include <memory>
#include <SDL/SDL.h>
#include <cstdio>
#include <fstream>
#include <csignal>

std::vector<uint8_t> LoadFile(const char *file)
{
    std::ifstream f {file};
    if (!f)
    {
        std::fprintf(stderr, "Unable open file %s\n", file);
        return {};
    }
    return std::vector<uint8_t>((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
}

volatile std::sig_atomic_t debugger_enabled = false;

void signal_handler(int signal_no)
{
    if (SIGINT == signal_no)
    {
        debugger_enabled = true;
    }
}

int main_loop(const char *file)
{
    auto binaryBlob = LoadFile(file);
    if (0 == binaryBlob.size())
    {
        std::fprintf(stderr, "File not found or empty file\n");
        return 1;
    }
    auto video = std::make_shared<Chip8::SDLVideo>();
    video->show();
    //Initialize Board
    auto board = std::make_shared<Chip8::Board>(video);
    board->LoadBinary(binaryBlob);

    //Initialize debugger
    auto debugger = std::make_shared<Chip8::Debugger>(board);

    /* message pump */
    int gameover = 0;
    SDL_Event event;
    while (!gameover)
    {
        if (debugger_enabled)
        {
            board->setBreak(true);
            debugger_enabled = false;
        }
        if (board->isBreak())
        {
            std::fprintf(stderr, "Debugger enabled\n");
            debugger->debugger_loop();
        }

        if (!board->shutdown())
        {
            board->cpu()->step();
        } else {
            gameover = 1;
        }
        /* look for an event */
        if (SDL_PollEvent(&event)) {
            /* an event was found */
            switch (event.type) {
            /* close button clicked */
            case SDL_QUIT:
                gameover = 1;
                break;

                /* handle the keyboard */
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    gameover = 1;
                    break;
                case SDLK_1: board->handleKey(0x1, SDL_KEYDOWN == event.type); break;
                case SDLK_2: board->handleKey(0x2, SDL_KEYDOWN == event.type); break;
                case SDLK_3: board->handleKey(0x3, SDL_KEYDOWN == event.type); break;
                case SDLK_4: board->handleKey(0xC, SDL_KEYDOWN == event.type); break;

                case SDLK_q: board->handleKey(0x4, SDL_KEYDOWN == event.type); break;
                case SDLK_w: board->handleKey(0x5, SDL_KEYDOWN == event.type); break;
                case SDLK_e: board->handleKey(0x6, SDL_KEYDOWN == event.type); break;
                case SDLK_r: board->handleKey(0xD, SDL_KEYDOWN == event.type); break;

                case SDLK_a: board->handleKey(0x7, SDL_KEYDOWN == event.type); break;
                case SDLK_s: board->handleKey(0x8, SDL_KEYDOWN == event.type); break;
                case SDLK_d: board->handleKey(0x9, SDL_KEYDOWN == event.type); break;
                case SDLK_f: board->handleKey(0xE, SDL_KEYDOWN == event.type); break;

                case SDLK_z: board->handleKey(0xA, SDL_KEYDOWN == event.type); break;
                case SDLK_x: board->handleKey(0x0, SDL_KEYDOWN == event.type); break;
                case SDLK_c: board->handleKey(0xB, SDL_KEYDOWN == event.type); break;
                case SDLK_v: board->handleKey(0xF, SDL_KEYDOWN == event.type); break;
                    break;
                }
                break;
            }
        }

        /* draw the background */
        //SDL_BlitSurface(bg, NULL, screen, NULL);
        /* update the screen */
        video->update();
    }
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::fprintf(stderr, "Usage %s [FILE_PATH]\n", argv[0]);
        return 1;
    }
    int rv;
    //Initialize SDL
    //TODO: Consider SDL_INIT_NOPARACHUTE
    rv = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_NOPARACHUTE);
    if (0 != rv)
    {
        std::fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
        return 1;
    }
    atexit(SDL_Quit);
    sighandler_t srv = signal(SIGINT, signal_handler);
    if (SIG_ERR == srv)
    {
        std::fprintf(stderr, "Unable to setup SIGINT handler: %s\n", strerror(errno));
        return 1;
    }

    main_loop(argv[1]);
    return 0;
}
