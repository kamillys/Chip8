#pragma once

namespace Chip8 {
class Board;
class Cpu;
} // namespace Chip8

#include <Chip8/Common.h>
#include <Chip8/Instruction.h>
#include <array>

namespace Chip8 {

class Cpu {
  std::array<std::uint8_t, Chip8::StdRegisterCount> m_Regs;
  std::array<std::uint16_t, Chip8::StackSize> m_Stack;
  std::uint16_t m_Pc;
  std::uint16_t m_I;
  std::uint8_t m_Sp;
  std::uint8_t m_Dt;
  std::uint8_t m_St;

  bool m_await;
  std::uint8_t m_regKey;

  void invalid_opcode(const Instruction &instr, Board *board);
  void setAwaitKey(uint8_t reg) {
    m_await = true;
    m_regKey = reg;
  }
  void keyActive(uint8_t key) {
    m_await = false;
    m_Regs[m_regKey] = key;
  }

public:
  Cpu();

  void reset();
  CHIP8_WARN_UNUSED ResultType timerStep(Board *board);
  CHIP8_WARN_UNUSED ResultType step(Board *board);
  CHIP8_WARN_UNUSED ResultType keyStep(Board *board, uint8_t key);

  uint8_t random();

  CHIP8_WARN_UNUSED bool isKeyAwait() const { return m_await; }

  CHIP8_DEPRECATED std::uint8_t Vx(std::uint8_t x) { return m_Regs[x]; }
  CHIP8_WARN_UNUSED ResultType Vx(std::uint8_t x, std::uint8_t &out) {
    if (x >= m_Regs.size()) {
      return ResultType::OutOfRange;
    }
    out = m_Regs[x];
    return ResultType::Ok;
  }
  CHIP8_WARN_UNUSED ResultType setVx(std::uint8_t x, std::uint8_t v) {
    if (x >= m_Regs.size()) {
      return ResultType::OutOfRange;
    }
    m_Regs[x] = v;
    return ResultType::Ok;
  }
  CHIP8_WARN_UNUSED std::uint16_t pc() { return m_Pc; }
  CHIP8_WARN_UNUSED ResultType setPc(std::uint16_t v) {
    m_Pc = v;
    return ResultType::Ok;
  }
  CHIP8_WARN_UNUSED std::uint16_t I() { return m_I; }
  CHIP8_WARN_UNUSED ResultType setI(std::uint16_t v) {
    m_I = v;
    return ResultType::Ok;
  }
  CHIP8_DEPRECATED std::uint16_t Sp() { return m_Sp; }
  CHIP8_WARN_UNUSED ResultType SpVal(uint8_t idx, uint16_t &out) {
    if (Chip8::StackSize <= idx)
      return ResultType::OutOfRange;
    out = m_Stack[idx];
    return ResultType::Ok;
  }
  CHIP8_WARN_UNUSED ResultType SpVal(uint16_t &out) { return SpVal(m_Sp, out); }
  CHIP8_WARN_UNUSED ResultType SetSpVal(uint16_t val) {
    if (Chip8::StackSize <= m_Sp)
      return ResultType::OutOfRange;
    m_Stack[m_Sp] = val;
    return ResultType::Ok;
  }
  CHIP8_WARN_UNUSED ResultType addSp() {
    if (Chip8::StackSize <= m_Sp + 1)
      return ResultType::OutOfRange;
    m_Sp++;
    return ResultType::Ok;
  }
  CHIP8_WARN_UNUSED ResultType decSp() {
    if (0 == m_Sp)
      return ResultType::OutOfRange;
    m_Sp--;
    return ResultType::Ok;
  }

  CHIP8_WARN_UNUSED ResultType SetDt(uint8_t val) {
    m_Dt = val;
    return ResultType::Ok;
  }
  CHIP8_WARN_UNUSED std::uint8_t Dt() { return m_Dt; }
  void decDt() {
    if (0 == m_Dt)
      return;
    m_Dt--;
  }

  CHIP8_WARN_UNUSED ResultType SetSt(uint8_t val) {
    m_St = val;
    return ResultType::Ok;
  }
  CHIP8_WARN_UNUSED std::uint8_t St() { return m_St; }
  void decSt() {
    if (0 == m_St)
      return;
    m_St--;
  }
};
} // namespace Chip8
