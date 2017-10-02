#include <Chip8/Memory.h>

namespace Chip8 {

void Memory::reset() {
  std::fill(m_data.begin(), m_data.end(), 0);
  // Store font data
  // TODO: Move to board?
  const std::vector<std::vector<uint8_t>> font_data = {
      /* 0 */ {0xF0, 0x90, 0x90, 0x90, 0xF0},
      /* 1 */ {0x20, 0x60, 0x20, 0x20, 0x70},
      /* 2 */ {0xF0, 0x10, 0xF0, 0x80, 0xF0},
      /* 3 */ {0xF0, 0x10, 0xF0, 0x10, 0xF0},
      /* 4 */ {0x90, 0x90, 0xF0, 0x10, 0x10},
      /* 5 */ {0xF0, 0x80, 0xF0, 0x10, 0xF0},
      /* 6 */ {0xF0, 0x80, 0xF0, 0x90, 0xF0},
      /* 7 */ {0xF0, 0x10, 0x20, 0x40, 0x40},
      /* 8 */ {0xF0, 0x90, 0xF0, 0x90, 0xF0},
      /* 9 */ {0xF0, 0x90, 0xF0, 0x10, 0xF0},
      /* A */ {0xF0, 0x90, 0xF0, 0x90, 0x90},
      /* B */ {0xE0, 0x90, 0xE0, 0x90, 0xE0},
      /* C */ {0xF0, 0x80, 0x80, 0x80, 0xF0},
      /* D */ {0xE0, 0x90, 0x90, 0x90, 0xE0},
      /* E */ {0xF0, 0x80, 0xF0, 0x80, 0xF0},
      /* F */ {0xF0, 0x80, 0xF0, 0x80, 0x80},
  };
  for (uint8_t font = 0; font < font_data.size(); ++font) {
    uint16_t offset;
    font_ptr(font, offset); // TODO
    for (const auto &sprite : font_data[font]) {
      write(offset++, sprite); // TODO
    }
  }
}

ResultType Memory::read(uint16_t offset, uint8_t &data) {
  if (offset >= m_data.size())
    return ResultType::OutOfRange;
  data = m_data.at(offset);
  return ResultType::Ok;
}

ResultType Memory::write(uint16_t offset, uint8_t data) {
  if (offset >= m_data.size())
    return ResultType::OutOfRange;
  m_data.at(offset) = data;
  return ResultType::Ok;
}

ResultType Memory::write_bulk(uint16_t offset, const std::vector<uint8_t> &data,
                              uint16_t &count) {
  count = 0;
  for (auto b : data) {
    auto tmp = write(offset++, b);
    CHIP8_CHECK_RESULT(tmp);
    count += 1;
  }
  return ResultType::Ok;
}

ResultType Memory::font_ptr(uint8_t font, uint16_t &offset) {
  if (font > 0x0F) {
    return ResultType::OutOfRange;
  }
  offset = 0x050 + font * 5;
  return ResultType::Ok;
}

} // namespace Chip8
