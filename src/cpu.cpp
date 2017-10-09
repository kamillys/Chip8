#include <Chip8/Board.h>
#include <Chip8/Cpu.h>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace Chip8 {

void Cpu::reset() {
  m_Pc = Chip8::ProgramStartLocation;
  m_I = 0;
  m_Sp = 0;
  m_Dt = 0;
  m_St = 0;
  std::fill(m_Regs.begin(), m_Regs.end(), 0);
  std::fill(m_Stack.begin(), m_Stack.end(), 0);
  m_await = false;
  m_regKey = 0;
}

ResultType Cpu::timerStep(Board *board) {
  decDt();
  decSt();
  if (St() > 0) {
    board->startBeep();
  } else {
    board->stopBeep();
  }
}

void Cpu::invalid_opcode(const Instruction &instr, Board *board) {
  std::fprintf(stderr, "Aborting, invalid opcode or not implemented\n"
                       "Fault code: %s\n",
               instr.disasm().c_str());
  std::fprintf(stderr, "PC: %.4X\n", pc());
  board->setBreak(true);
}

Cpu::Cpu() { reset(); }

ResultType Cpu::step(Board *board) {
  if (m_await) {
    return ResultType::Ok;
  }
  ResultType rv;
  // Perform single instruction
  std::uint16_t opcode;
  rv = board->memoryRead(pc(), opcode);
  CHIP8_CHECK_RESULT(rv);

  Instruction instr(opcode);
  // printf("\t%.3X: %.4X\t%s\n", pc(), opcode, instr.disasm().c_str());

  switch (instr.type()) {
  case 0x0: {
    switch (instr.code()) {
    case 0x00E0: // CLS
      board->clearScreen();
      setPc(pc() + 2);
      return ResultType::Ok;
    case 0x00EE: // RET
    {
      uint16_t tmp;
      decSp();
      SpVal(tmp);
      setPc(tmp);
      SetSpVal(0);
      return ResultType::Ok;
    }
    default:
      invalid_opcode(instr, board);
      return ResultType::InvalidOpcode;
    }
    break;
  }
  case 0x1: // JP addr
    setPc(instr.NNN());
    return ResultType::Ok;
  case 0x2: // CALL addr
    SetSpVal(pc() + 2);
    addSp();
    setPc(instr.NNN());
    return ResultType::Ok;
  case 0x3: // SE Vx, byte // Skip if Vx == byte
    if (Vx(instr.X()) == instr.NN()) {
      setPc(pc() + 2);
    }
    setPc(pc() + 2);
    return ResultType::Ok;
  case 0x4: // SE Vx, byte // Skip if Vx == byte
    if (Vx(instr.X()) != instr.NN()) {
      setPc(pc() + 2);
    }
    setPc(pc() + 2);
    return ResultType::Ok;
  case 0x5: // SE Vx, Vy, skip if Vx = Vy
    if (Vx(instr.X()) == Vx(instr.Y())) {
      setPc(pc() + 2);
    }
    setPc(pc() + 2);
    return ResultType::Ok;
  case 0x6: // LD Vx, byte
    setVx(instr.X(), instr.NN());
    setPc(pc() + 2);
    return ResultType::Ok;
  case 0x7: // ADD Vx, byte
    setVx(instr.X(), instr.NN() + Vx(instr.X()));
    setPc(pc() + 2);
    return ResultType::Ok;
  case 0x8:
    switch (instr.subtype1()) {
    case 0x0: // LD Vx, Vy
      setVx(instr.X(), Vx(instr.Y()));
      setPc(pc() + 2);
      return ResultType::Ok;
    case 0x1: // OR Vx, Vy
      setVx(instr.X(), Vx(instr.X()) | Vx(instr.Y()));
      setPc(pc() + 2);
      invalid_opcode(instr, board);
      return ResultType::Ok;
    case 0x2: // AND Vx, Vy
      setVx(instr.X(), Vx(instr.X()) & Vx(instr.Y()));
      setPc(pc() + 2);
      return ResultType::Ok;
    case 0x3: // XOR Vx, Vy
      setVx(instr.X(), Vx(instr.X()) ^ Vx(instr.Y()));
      setPc(pc() + 2);
      return ResultType::Ok;
    case 0x4: // ADD Vx, Vy with carry
    {
      std::uint16_t val = Vx(instr.X());
      val += (std::uint16_t)Vx(instr.Y());
      setVx(instr.X(), val & 0xFF);
      setVx(0xF, 0x1 & (val >> 8));
      setPc(pc() + 2);
      return ResultType::Ok;
    }
    case 0x5: // SUB Vx, Vy with NOT
    {
      std::uint8_t x = Vx(instr.X());
      std::uint8_t y = Vx(instr.Y());
      setVx(0xF, (x > y) ? 0x1 : 0);
      setVx(instr.X(), x - y);
      setPc(pc() + 2);
      return ResultType::Ok;
    }
    case 0x6: // SHR Vx {, Vy}
    {
      std::uint16_t val = Vx(instr.X());
      setVx(0xF, 0x1 & val);
      val >>= 1;
      setVx(instr.X(), val & 0xFF);
      setPc(pc() + 2);
      return ResultType::Ok;
    }
    case 0x7: // SUBN Vx, Vy
    {
      std::uint8_t x = Vx(instr.X());
      std::uint8_t y = Vx(instr.Y());
      setVx(0xF, (y > x) ? 0x1 : 0);
      setVx(instr.X(), y - x);
      setPc(pc() + 2);
      return ResultType::Ok;
    }
    case 0xe: // SHL Vx {, Vy}
    {
      std::uint16_t val = Vx(instr.X());
      val <<= 1;
      setVx(0xF, 0x1 & (val >> 8));
      setVx(instr.X(), val & 0xFF);
      setPc(pc() + 2);
      return ResultType::Ok;
    }
    default:
      invalid_opcode(instr, board);
      return ResultType::InvalidOpcode;
    }
    break;
  case 0x9: // SNE Vx, Vy, skip if Vx != Vy
    if (Vx(instr.X()) != Vx(instr.Y())) {
      setPc(pc() + 2);
    }
    setPc(pc() + 2);
    return ResultType::Ok;
  case 0xa: // LD I, addr
    setI(instr.NNN());
    setPc(pc() + 2);
    return ResultType::Ok;
  case 0xb: // JP V0, addr // Jump to V0 + addr
    setPc(Vx(0) + instr.NNN());
    return ResultType::Ok;
  case 0xc: // RND Vx, byte
    setVx(instr.X(), random() & instr.NN());
    setPc(pc() + 2);
    return ResultType::Ok;
  case 0xd: // DRW Vx, Vy, nibble
  {
    // TODO: Collision
    bool VF = false;
    bool res = false;
    for (uint8_t it = 0; it < instr.N(); ++it) {
      // Display n-byte sprite starting at memory location I at (Vx, Vy), set
      // VF
      // = collision.
      uint8_t value;
      board->memoryRead(I() + it, value);
      board->flipSprite(Vx(instr.X()), Vx(instr.Y()) + it, value, res);
      VF |= res;
    }
    setVx(0xF, VF ? 1 : 0);
    setPc(pc() + 2);
    return ResultType::Ok;
  }
  case 0xe: {
    switch (instr.subtype2()) {
    case 0x9E: // SKP Vx, skip next instr if Vx PRESSED
    {
      if (board->isKeyDown(Vx(instr.X()))) {
        setPc(pc() + 2);
      }
      setPc(pc() + 2);
      return ResultType::Ok;
    }
    case 0xA1: // SKNP Vx, skip next instr if Vx NOT PRESSED
      if (!board->isKeyDown(Vx(instr.X()))) {
        setPc(pc() + 2);
      }
      setPc(pc() + 2);
      return ResultType::Ok;
    default:
      invalid_opcode(instr, board);
      return ResultType::InvalidOpcode;
    }
    break;
  }
  case 0xf: {
    switch (instr.subtype2()) {
    case 0x07: // LD Vx, DT
      setVx(instr.X(), Dt());
      setPc(pc() + 2);
      return ResultType::Ok;
    case 0x0A: // LD Vx, K
      setAwaitKey(instr.X());
      setPc(pc() + 2);
      return ResultType::Ok;
    case 0x15: // LD DT, Vx
      SetDt(Vx(instr.X()));
      setPc(pc() + 2);
      return ResultType::Ok;
    case 0x18: // LD ST, Vx
      SetSt(Vx(instr.X()));
      setPc(pc() + 2);
      return ResultType::Ok;
    case 0x1E: // ADD I, Vx
      setI(I() + Vx(instr.X()));
      setPc(pc() + 2);
      return ResultType::Ok;
    case 0x29: // LD F, Vx
    {
      // Set I = location of sprite for digit Vx.
      std::uint16_t offset;
      board->fontPtr(Vx(instr.X()), offset);
      setI(offset);
      setPc(pc() + 2);
      return ResultType::Ok;
    }
    case 0x33: // LD B, Vx
    {
      // 1) split [Vx] into B0, B1, B2
      // Store mem[i] <- B0, mem[i+1] <- B1...
      std::uint8_t value = Vx(instr.X());
      board->memoryWrite(I() + 0,
                         static_cast<std::uint8_t>((value / 100) % 10));
      board->memoryWrite(I() + 1, static_cast<std::uint8_t>((value / 10) % 10));
      board->memoryWrite(I() + 2, static_cast<std::uint8_t>((value) % 10));
      // board->memory()->write_bulk(
      //    I(), {
      //             static_cast<std::uint8_t>((value / 100) % 10),
      //                static_cast<std::uint8_t>((value / 10) % 10),
      //                static_cast<std::uint8_t>((value) % 10),
      //         });
      setPc(pc() + 2);
      return ResultType::Ok;
    }
    case 0x55: {
      for (uint8_t it = 0; it <= instr.X(); ++it) {
        uint8_t value = Vx(it);
        board->memoryWrite(I(), value);
        setI(I() + 1);
      }
      setPc(pc() + 2);
      return ResultType::Ok;
    }
    case 0x65: {
      for (uint8_t it = 0; it <= instr.X(); ++it) {
        uint8_t value;
        board->memoryRead(I(), value);
        setVx(it, value);
        setI(I() + 1);
      }
      setPc(pc() + 2);
      return ResultType::Ok;
    }
    default:
      invalid_opcode(instr, board);
      return ResultType::InvalidOpcode;
    }

    break;
  }
  default:
    invalid_opcode(instr, board);
    return ResultType::InvalidOpcode;
  }
  invalid_opcode(instr, board);
  return ResultType::InvalidOpcode;
}

ResultType Cpu::keyStep(Board * /*board*/, uint8_t key) {
  if (isKeyAwait()) {
    keyActive(key);
  }
  return ResultType::Ok;
}

uint8_t Cpu::random() {
  // TODO: Better random
  return rand() % 255;
}

} // namespace Chip8
