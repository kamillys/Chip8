#include <Chip8/Audio.h>

namespace Chip8 {

void Audio::beginBeep() { setBeep(true); }

void Audio::endBeep() { setBeep(false); }

Audio::Audio() {}

Audio::~Audio() {}

void Audio::startBeep() {
  if (beep())
    return;
  beginBeep();
}

void Audio::stopBeep() {
  if (!beep())
    return;
  endBeep();
}

void Audio::reset() { setBeep(false); }

} // namespace Chip8
