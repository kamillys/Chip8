#include <Chip8/Instruction.h>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace Chip8 {

std::string Instruction::disasm() const {
  std::stringstream str;
  str << std::hex << std::uppercase;

  switch (type()) {
  case 0x0: {
    switch (code()) {
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
    str << "SE V" << (std::uint16_t)X() << ", 0x" << NN();
    break;
  case 0x4:
    str << "SNE V" << (std::uint16_t)X() << ", 0x" << NN();
    break;
  case 0x5:
    str << "SE V" << (std::uint16_t)X() << ", V" << (std::uint16_t)Y();
    break;
  case 0x6:
    str << "LD V" << (std::uint16_t)X() << ", 0x" << NN();
    break;
  case 0x7:
    str << "ADD V" << (std::uint16_t)X() << ", 0x" << NN();
    break;
  case 0x8:
    switch (subtype1()) {
    case 0x0:
      str << "LD V" << (std::uint16_t)X() << ", V" << (std::uint16_t)Y();
      break;
    case 0x1:
      str << "OR V" << (std::uint16_t)X() << ", V" << (std::uint16_t)Y();
      break;
    case 0x2:
      str << "AND V" << (std::uint16_t)X() << ", V" << (std::uint16_t)Y();
      break;
    case 0x3:
      str << "XOR V" << (std::uint16_t)X() << ", V" << (std::uint16_t)Y();
      break;
    case 0x4:
      str << "ADD V" << (std::uint16_t)X() << ", V" << (std::uint16_t)Y();
      break;
    case 0x5:
      str << "SUB V" << (std::uint16_t)X() << ", V" << (std::uint16_t)Y();
      break;
    case 0x6:
      str << "SHR V" << (std::uint16_t)X() << "{, V" << (std::uint16_t)Y()
          << "}";
      break;
    case 0x7:
      str << "SUBN V" << (std::uint16_t)X() << ", V" << (std::uint16_t)Y();
      break;
    case 0xe:
      str << "SHL V" << (std::uint16_t)X() << "{, V" << (std::uint16_t)Y()
          << "}";
      break;
    default:
      str << "[UNKNOWN]";
      break;
    }
    break;
  case 0x9:
    str << "SNE V" << (std::uint16_t)X() << ", V" << (std::uint16_t)Y();
    break;
  case 0xa:
    str << "LD I, 0x" << NNN();
    break;
  case 0xb:
    str << "JP V0, 0x" << NNN();
    break;
  case 0xc:
    str << "RND V" << (std::uint16_t)X() << ", 0x" << NN();
    break;
  case 0xd:
    str << "DRW V" << (std::uint16_t)X() << ", V" << (std::uint16_t)Y()
        << ", 0x" << N();
    break;
  case 0xe: {
    switch (subtype2()) {
    case 0x9E:
      str << "SKP V" << (std::uint16_t)X();
      break;
    case 0xA1:
      str << "SKNP V" << (std::uint16_t)X();
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
      str << "LD V" << (std::uint16_t)X() << ", DT";
      break;
    case 0x0A:
      str << "LD V" << (std::uint16_t)X() << ", K";
      break;
    case 0x15:
      str << "LD DT, V" << (std::uint16_t)X();
      break;
    case 0x18:
      str << "LD ST, V" << (std::uint16_t)X();
      break;
    case 0x1E:
      str << "ADD I, V" << (std::uint16_t)X();
      break;
    case 0x29:
      str << "LD F, V" << (std::uint16_t)X();
      break;
    case 0x33:
      str << "LD B, V" << (std::uint16_t)X();
      break;
    case 0x55:
      str << "LD [I], V" << (std::uint16_t)X();
      break;
    case 0x65:
      str << "LD V" << (std::uint16_t)X() << ", [I]";
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

} // namespace Chip8
