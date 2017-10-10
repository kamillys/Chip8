#pragma once

#include <Chip8/Audio.h>
#include <array>

namespace Chip8 {

class SDLAudio : public Audio {
protected:
  virtual void beginBeep();
  virtual void endBeep();

public:
  SDLAudio();
  ~SDLAudio();
  virtual void reset();

  void audioCb(uint8_t *stream, int len);
};
} // namespace Chip8
