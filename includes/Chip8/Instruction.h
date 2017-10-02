#pragma once

namespace Chip8 {
class Instruction;
} // namespace Chip8

#include <cstdint>
#include <string>

namespace Chip8 {

class Instruction {
  std::uint16_t m_code;

public:
  Instruction(std::uint16_t code) : m_code(code) {}
  std::uint16_t code() const { return m_code; }
  // Opcode opcode() const;
  std::uint8_t X() const { return 0xF & (code() >> 8); }
  std::uint8_t Y() const { return 0xF & (code() >> 4); }
  std::uint16_t NNN() const { return 0xFFF & code(); }
  std::uint16_t NN() const { return 0xFF & code(); }
  std::uint16_t N() const { return 0xF & code(); }

  // type extracts Txxx
  std::uint8_t type() const { return 0xF & (code() >> 12); }
  // subtype1 extracts xxxT
  std::uint8_t subtype1() const { return 0xF & code(); }
  // subtype2 extracts xxTT
  std::uint8_t subtype2() const { return 0xFF & code(); }

  std::string disasm() const;
};
} // namespace Chip8
