#pragma once

namespace Chip8 {
class Audio;
} // namespace Chip8

#include <stdint.h>

namespace Chip8 {

class Audio {
  bool m_beep = false;

  void setBeep(bool v) { m_beep = v; }

public:
  bool beep() const { return m_beep; }
  Audio();
  void startBeep();
  void stopBeep();
  void reset();

  void audioCb(uint8_t *stream, int len);
};
} // namespace Chip8
