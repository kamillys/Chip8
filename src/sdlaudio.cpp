#include "sdlaudio.h"
#include <SDL2/SDL.h>
#include <array>
#include <cstdio>

namespace Chip8 {

void audioCallback(void *userdata, Uint8 *stream, int len) {
  reinterpret_cast<SDLAudio *>(userdata)->audioCb(stream, len);
}

void SDLAudio::audioCb(uint8_t *stream, int len) {
  // TODO: Write proper generator
  uint16_t *audio = reinterpret_cast<uint16_t *>(stream);
  len = len / 2;
  if (beep()) {
    for (int i = 0; i < len; ++i) {
      const int MOD = 256;
      int v = i % (MOD);
      if (v < (MOD / 2))
        audio[i] = 0x0FFF;
      else
        audio[i] = 0;
    }
  } else {
    for (int i = 0; i < len; ++i)
      audio[i] = 0;
  }
}

SDLAudio::SDLAudio() {
  SDL_AudioSpec desiredSpec;

  desiredSpec.freq = 44100;
  desiredSpec.format = AUDIO_U16SYS;
  desiredSpec.channels = 1;
  desiredSpec.samples = 2048;
  desiredSpec.callback = audioCallback;
  desiredSpec.userdata = this;

  SDL_AudioSpec obtainedSpec;

  SDL_OpenAudio(&desiredSpec, &obtainedSpec);

  // start play audio
  SDL_PauseAudio(0);
}

SDLAudio::~SDLAudio() { SDL_CloseAudio(); }

void SDLAudio::reset() { endBeep(); }

void SDLAudio::beginBeep() {
  SDL_LockAudio();
  Audio::beginBeep();
  SDL_UnlockAudio();
}

void SDLAudio::endBeep() {
  SDL_LockAudio();
  Audio::endBeep();
  SDL_UnlockAudio();
}

} // namespace Chip8
