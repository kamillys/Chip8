#pragma once

#include <stdint.h>

namespace Chip8 {

class Video {
public:
  virtual void reset() = 0;

  virtual bool flipSprite(uint8_t x, uint8_t y, uint8_t v) = 0;
  virtual bool flipBit(uint8_t x, uint8_t y, bool v) = 0;
  virtual void dump() = 0;
};
}
