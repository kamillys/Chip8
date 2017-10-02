#pragma once

namespace Chip8 {
class Cpu;
}

#include "Board.h"
#include <array>

namespace Chip8 {

class Instruction {
  uint16_t m_opcode;

public:
  Instruction(uint16_t opcode) : m_opcode(opcode) {}
  uint16_t opcode() const { return m_opcode; }
  uint8_t X() const { return 0xF & (opcode() >> 8); }
  uint8_t Y() const { return 0xF & (opcode() >> 4); }
  uint16_t NNN() const { return 0xFFF & opcode(); }
  uint16_t NN() const { return 0xFF & opcode(); }
  uint16_t N() const { return 0xF & opcode(); }

  // type extracts Txxx
  uint8_t type() const { return 0xF & (opcode() >> 12); }
  // subtype1 extracts xxxT
  uint8_t subtype1() const { return 0xF & opcode(); }
  // subtype2 extracts xxTT
  uint8_t subtype2() const { return 0xFF & opcode(); }

  std::string disasm() const;
};

class Cpu {
  std::array<uint8_t, 0x10> m_Regs;
  uint16_t m_Pc = 0x200;
  uint16_t m_I = 0;
  std::array<uint16_t, 0x10 | 0xFF> m_Stack;
  uint8_t m_Sp = 0;
  Board *m_board = nullptr;

public:
  void setBoard(Board *board) { m_board = board; }
  Board *board() { return m_board; }
  void step();
  void reset();

  void dump();

  uint8_t Vx(uint8_t x) { return m_Regs[x]; }
  int setVx(uint8_t x, uint8_t v) {
    m_Regs[x] = v;
    return 1;
  } // TODO:
  uint16_t pc() { return m_Pc; }
  int setPc(uint16_t v) {
    m_Pc = v;
    return 1;
  } // TODO:
  uint16_t I() { return m_I; }
  int setI(uint16_t v) {
    m_I = v;
    return 1;
  } // TODO:
  uint16_t Sp() { return m_Sp; }
  int addSp() {
    m_Sp++;
    return 1;
  } // TODO:
  int decSp() {
    m_Sp--;
    return 1;
  } // TODO:

  int fetch(uint16_t addr, uint16_t &out);
  int fetch(uint16_t &out);
};
}
