#include <Chip8/Video.h>

namespace Chip8 {

void Video::reset() { clearScreen(); }

void Video::clearScreen() {
  for (auto &row : m_screen)
    for (auto &bit : row)
      bit = false;
  for (auto &row : m_ledBuffer)
    for (auto &bit : row)
      bit = 0;
}

bool Video::flipSprite(uint8_t x, uint8_t y, uint8_t v) {
  bool rv = false;
  for (uint8_t it = 0; it < 8; ++it) {
    rv |= flipBit(x + it, y, (v & (1 << (7 - it))) >> (7 - it));
  }
  return rv;
}

bool Video::flipBit(uint8_t x, uint8_t y, bool v) {
  y = y % m_screen.size();
  x = x % m_screen[y].size();
  bool oldv = m_screen[y][x];
  m_screen[y][x] ^= v;
  return oldv && !v;
}

void Video::dump() {
  for (auto row : m_screen) {
    for (auto bit : row)
      std::printf("%c", bit ? '*' : '_');
    std::printf("\n");
  }
}

} // namespace Chip8
