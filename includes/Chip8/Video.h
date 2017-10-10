#pragma once

namespace Chip8 {
class Video;
} // namespace Chip8

#include <array>
#include <cstdint>

namespace Chip8 {

class Video {
protected:
  // There should be actual pixel data stored
  std::array<std::array<bool, 64>, 32> m_screen;
  // To implement pixel fadeout, store current value in buffer.
  // Updated in update() method
  std::array<std::array<uint8_t, 64>, 32> m_ledBuffer;

public:
  virtual void reset();
  void clearScreen();
  bool flipSprite(uint8_t x, uint8_t y, uint8_t v);
  bool flipBit(uint8_t x, uint8_t y, bool v);
  void dump();
};

} // namespace Chip8
