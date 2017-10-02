#include <Chip8/Cpu.h>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace Chip8 {

std::string Instruction::disasm() const {
  std::stringstream str;
  str << std::hex << std::uppercase;

  switch (type()) {
  case 0x0: {
    switch (opcode()) {
    case 0x00E0:
      str << "CLS";
      break;
    case 0x00EE:
      str << "RET";
      break;
    default:
      str << "[UNKNOWN]";
      break;
    }
    break;
  }
  case 0x1:
    str << "JP " << NNN();
    break;
  case 0x2:
    str << "CALL " << NNN();
    break;
  case 0x3:
    str << "SE V" << (uint16_t)X() << ", 0x" << NN();
    break;
  case 0x4:
    str << "SNE V" << (uint16_t)X() << ", 0x" << NN();
    break;
  case 0x5:
    str << "SE V" << (uint16_t)X() << ", V" << (uint16_t)Y();
    break;
  case 0x6:
    str << "LD V" << (uint16_t)X() << ", 0x" << NN();
    break;
  case 0x7:
    str << "ADD V" << (uint16_t)X() << ", 0x" << NN();
    break;
  case 0x8:
    switch (subtype1()) {
    case 0x0:
      str << "LD V" << (uint16_t)X() << ", V" << (uint16_t)Y();
      break;
    case 0x1:
      str << "OR V" << (uint16_t)X() << ", V" << (uint16_t)Y();
      break;
    case 0x2:
      str << "AND V" << (uint16_t)X() << ", V" << (uint16_t)Y();
      break;
    case 0x3:
      str << "XOR V" << (uint16_t)X() << ", V" << (uint16_t)Y();
      break;
    case 0x4:
      str << "ADD V" << (uint16_t)X() << ", V" << (uint16_t)Y();
      break;
    case 0x5:
      str << "SUB V" << (uint16_t)X() << ", V" << (uint16_t)Y();
      break;
    case 0x6:
      str << "SHR V" << (uint16_t)X() << "{, V" << (uint16_t)Y() << "}";
      break;
    case 0x7:
      str << "SUBN V" << (uint16_t)X() << ", V" << (uint16_t)Y();
      break;
    case 0xe:
      str << "SHL V" << (uint16_t)X() << "{, V" << (uint16_t)Y() << "}";
      break;
    default:
      str << "[UNKNOWN]";
      break;
    }
    break;
  case 0x9:
    str << "SNE V" << (uint16_t)X() << ", V" << (uint16_t)Y();
    break;
  case 0xa:
    str << "LD I, 0x" << NNN();
    break;
  case 0xb:
    str << "JP V0, 0x" << NNN();
    break;
  case 0xc:
    str << "RND V" << (uint16_t)X() << ", 0x" << NN();
    break;
  case 0xd:
    str << "DRW V" << (uint16_t)X() << ", V" << (uint16_t)Y() << ", 0x" << N();
    break;
  case 0xe: {
    switch (subtype2()) {
    case 0x9E:
      str << "SKP V" << (uint16_t)X();
      break;
    case 0xA1:
      str << "SKNP V" << (uint16_t)X();
      break;
    default:
      str << "[UNKNOWN]";
      break;
    }
    break;
  }
  case 0xf: {
    switch (subtype2()) {
    case 0x07:
      str << "LD V" << (uint16_t)X() << ", DT";
      break;
    case 0x0A:
      str << "LD V" << (uint16_t)X() << ", K";
      break;
    case 0x15:
      str << "LD DT, V" << (uint16_t)X();
      break;
    case 0x18:
      str << "LD ST, V" << (uint16_t)X();
      break;
    case 0x1E:
      str << "ADD I, V" << (uint16_t)X();
      break;
    case 0x29:
      str << "LD F, V" << (uint16_t)X();
      break;
    case 0x33:
      str << "LD B, V" << (uint16_t)X();
      break;
    case 0x55:
      str << "LD [I], V" << (uint16_t)X();
      break;
    case 0x65:
      str << "LD V" << (uint16_t)X() << ", [I]";
      break;
    default:
      str << "[UNKNOWN]";
      break;
    }

    break;
  }
  default:
    str << "[UNKNOWN]";
    break;
  }

  return str.str();
}

void Cpu::reset() {
  m_Pc = 0x200;
  m_I = 0;
  m_Sp = 0;
  std::fill(m_Regs.begin(), m_Regs.end(), 0);
  std::fill(m_Stack.begin(), m_Stack.end(), 0);
}

void Cpu::dump() {
  std::printf("PC: %.4x\n", pc());
  std::printf("SP: %.4x\n", Sp());
  std::printf("I: %.4x", I());
  for (size_t rId = 0; rId < 16; ++rId) {
    if (rId % 4 == 0)
      std::printf("\n");
    std::printf("V%lX: %.2x %3i  ", rId, Vx(rId), Vx(rId));
  }
  std::printf("\n");
  std::printf("Stack:\n");
  for (size_t rId = 0; rId < 16; ++rId) {
    if (rId % 4 == 0)
      std::printf("\n\t");
    std::printf("%.4x  ", m_Stack[rId]);
  }
  std::printf("\n");
}

int Cpu::fetch(uint16_t addr, uint16_t &out) {
  uint8_t top;
  uint8_t bottom;
  int rv = 0;
  rv += board()->memory()->read(addr, top);
  rv += board()->memory()->read(addr + 1, bottom);
  if (rv != 2) {
    return 0;
  }
  out = (top << 8) | bottom;
  return 1;
}

int Cpu::fetch(uint16_t &out) { return fetch(pc(), out); }

void Cpu::step() {
  // Perform single instruction
  uint16_t opcode;
  fetch(opcode);
  Instruction instr(opcode);
  // printf("\t%.3X: %.4X\t%s\n", pc(), opcode, instr.disasm().c_str());
  switch (instr.type()) {
  case 0x0: {
    switch (instr.opcode()) {
    // case 0x00E0:
    //    str << "CLS";
    //    break;
    case 0x00EE:
      decSp(); // TODO: fix
      setPc(m_Stack[Sp()]);
      m_Stack[Sp()] = 0;
      // board()->setBreak(true);
      break;
    default:
      printf("Aborting, not implemented\n");
      dump();
      board()->setBreak(true);
      break;
    }
    break;
  }
  case 0x1:
    setPc(instr.NNN());
    break;
  case 0x2:
    m_Stack[Sp()] = pc() + 2;
    addSp(); // TODO: fix
    setPc(instr.NNN());
    // board()->setBreak(true);
    break;
  case 0x3:
    if (Vx(instr.X()) == instr.NN()) {
      setPc(pc() + 2);
    }
    setPc(pc() + 2);
    break;
  case 0x4:
    if (Vx(instr.X()) != instr.NN()) {
      setPc(pc() + 2);
    }
    setPc(pc() + 2);
    break;
  case 0x6:
    setVx(instr.X(), instr.NN());
    setPc(pc() + 2);
    break;
  case 0x7:
    setVx(instr.X(), instr.NN() + Vx(instr.X()));
    setPc(pc() + 2);
    break;
  case 0x8:
    switch (instr.subtype1()) {
    // case 0x0:
    //    str << "LD V" << (uint16_t)X() << ", V" << (uint16_t)Y();
    //    break;
    // case 0x1:
    //    str << "OR V" << (uint16_t)X() << ", V" << (uint16_t)Y();
    //    break;
    case 0x2:
      setVx(instr.X(), Vx(instr.X()) & Vx(instr.Y()));
      setPc(pc() + 2);
      // str << "ADD V" << (uint16_t)X() << ", V" << (uint16_t)Y();
      break;
    // case 0x3:
    //    str << "XOR V" << (uint16_t)X() << ", V" << (uint16_t)Y();
    //    break;
    case 0x4: {
      uint16_t val = Vx(instr.X());
      val += (uint16_t)Vx(instr.Y());
      setVx(instr.X(), val & 0xFF);
      setVx(0xF, 0x1 & (val >> 8));
      setPc(pc() + 2);
      // str << "ADD V" << (uint16_t)X() << ", V" << (uint16_t)Y();
      break;
    }
    // case 0x5:
    //    str << "SUB V" << (uint16_t)X() << ", V" << (uint16_t)Y();
    //    break;
    // case 0x6:
    //    str << "SHR V" << (uint16_t)X() << "{, V" << (uint16_t)Y() << "}";
    //    break;
    // case 0x7:
    //    str << "SUBN V" << (uint16_t)X() << ", V" << (uint16_t)Y();
    //    break;
    // case 0xe:
    //    str << "SHL V" << (uint16_t)X() << "{, V" << (uint16_t)Y() << "}";
    //    break;
    default:
      printf("Aborting, subtype not implemented\n");
      dump();
      board()->setBreak(true);
      break;
    }
    break;
  case 0xa:
    setI(instr.NNN());
    setPc(pc() + 2);
    break;
  case 0xc:
    setVx(instr.X(), rand() & instr.NN());
    setPc(pc() + 2);
    break;
  case 0xd: {
    I();
    Vx(instr.X());
    Vx(instr.Y());
    // TODO: Collision
    bool VF = false;
    for (uint8_t it = 0; it < instr.N(); ++it) {
      // Display n-byte sprite starting at memory location I at (Vx, Vy), set
      // VF
      // = collision.
      uint8_t value;
      if (1 != board()->memory()->read(I() + it, value)) {
        break;
      }
      VF |= board()->video()->flipSprite(Vx(instr.X()), Vx(instr.Y()) + it,
                                         value);
    }
    setVx(0xF, VF ? 1 : 0);
    setPc(pc() + 2);
    // printf("Drawing instruction\n");
    // dump();
    // board()->setBreak(true);
    break;
  }
  case 0xe: {
    switch (instr.subtype2()) {
    case 0x9E: {
      if (board()->isKeyDown(Vx(instr.X()))) {
        setPc(pc() + 2);
        // std::fprintf(stderr, "SKP: KEY V%X %X PRESSED\n", instr.X(),
        // Vx(instr.X()));
      } else {
        // skipping as not pressed
        // std::fprintf(stderr, "SKP: KEY V%X %X NOT PRESSED\n", instr.X(),
        // Vx(instr.X()));
      }
      setPc(pc() + 2);
      break;
    }
    case 0xA1:
      if (board()->isKeyDown(Vx(instr.X()))) {
        // skipping as pressed
        // std::fprintf(stderr, "SKNP: KEY V%X %X PRESSED\n", instr.X(),
        // Vx(instr.X()));
      } else {
        setPc(pc() + 2);
        // std::fprintf(stderr, "SKNP: KEY V%X %X NOT PRESSED\n", instr.X(),
        // Vx(instr.X()));
      }
      // std::fprintf(stderr, "SKNP: KEY V%X %X NOT PRESSED\n", instr.X(),
      // Vx(instr.X()));
      // str << "SKNP V" << (uint16_t)X();
      setPc(pc() + 2);
      break;
    default:
      printf("Aborting, not implemented\n");
      dump();
      board()->setBreak(true);
      break;
    }
    break;
  }
  case 0xf: {
    switch (instr.subtype2()) {
    case 0x18:
      std::cerr << "LD ST, V" << (uint16_t)instr.X() << std::endl;
      setPc(pc() + 2);
      break;
    case 0x29: {
      // TODO: IMPL
      // Set I = location of sprite for digit Vx.
      size_t offset;
      board()->memory()->font_ptr(Vx(instr.X()), offset); // TODO:
      fprintf(stderr, "LOAD FONT: %X %4X\n", Vx(instr.X()), offset);
      setI(offset);
      setPc(pc() + 2);
      // dump();
      // board()->setBreak(true);
      break;
    }
    case 0x33: {
      // 1) split [Vx] into B0, B1, B2
      // Store mem[i] <- B0, mem[i+1] <- B1...
      // TODO: IMPL
      uint8_t value = Vx(instr.X());
      fprintf(stderr, "INTO BCD: %X %d\n", value, value);
      board()->memory()->write_bulk(
          I(), {
                   (value / 100) % 10, (value / 10) % 10, (value) % 10,
               });
      // TODO: CHECK
      setPc(pc() + 2);
      // printf("%i\n", value);
      // dump();
      // board()->setBreak(true);
      break;
    }
    case 0x55: {
      for (uint8_t it = 0; it <= instr.X(); ++it) {
        uint8_t value = Vx(it);
        if (1 != board()->memory()->write(I(), value)) {
          // TODO: error
          break;
        }
        setI(I() + 1);
      }
      setPc(pc() + 2);
      // printf("Check stuff\n");
      // dump();
      // board()->setBreak(true);
      break;
    }
    case 0x65: {
      for (uint8_t it = 0; it <= instr.X(); ++it) {
        uint8_t value;
        if (1 != board()->memory()->read(I(), value)) {
          // TODO: error
          break;
        }
        setVx(it, value);
        setI(I() + 1);
      }
      setPc(pc() + 2);
      // printf("Check stuff\n");
      // TODO: IMPL
      // dump();
      // board()->setBreak(true);
      break;
    }
    default:
      printf("Aborting, subtype not implemented\n");
      dump();
      board()->setBreak(true);
      break;
    }

    break;
  }
  default:
    printf("Aborting, not implemented\n");
    dump();
    board()->setBreak(true);
    break;
  }
}
}
