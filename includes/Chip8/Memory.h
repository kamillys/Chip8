#pragma once

namespace Chip8 {
class Memory;
} // namespace Chip8

#include <Chip8/Common.h>
#include <array>
#include <vector>

namespace Chip8 {

class Memory {
  std::array<uint8_t, Chip8::MemorySize> m_data;

public:
  void reset();
  // return bytes readen/written
  CHIP8_WARN_UNUSED ResultType read(uint16_t offset, uint8_t &data);
  CHIP8_WARN_UNUSED ResultType write(uint16_t offset, uint8_t data);
  CHIP8_WARN_UNUSED ResultType write_bulk(uint16_t offset,
                                          const std::vector<uint8_t> &data,
                                          uint16_t &count);
  CHIP8_WARN_UNUSED ResultType write_bulk(uint16_t offset,
                                          const std::vector<uint8_t> &data) {
    uint16_t count;
    return write_bulk(offset, data, count);
  }

  CHIP8_WARN_UNUSED ResultType font_ptr(uint8_t font, uint16_t &offset);
};
} // namespace Chip8
