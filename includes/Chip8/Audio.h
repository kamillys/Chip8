#pragma once

namespace Chip8 {
class Audio;
} // namespace Chip8

#include <stdint.h>

namespace Chip8 {

class Audio {
  bool m_beep = false;

  void setBeep(bool v) { m_beep = v; }

protected:
  virtual void beginBeep();
  virtual void endBeep();

public:
  Audio();
  ~Audio();
  void startBeep();
  void stopBeep();
  bool beep() const { return m_beep; }
  virtual void reset();
};
} // namespace Chip8
