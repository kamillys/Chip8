#pragma once

#define CHIP8_WARN_UNUSED __attribute__((warn_unused_result))
#define CHIP8_DEPRECATED __attribute__((deprecated))

#include <cstdint>

namespace Chip8 {
#define CHIP8_CHECK_RESULT(rv)                                                 \
  do {                                                                         \
    if (ResultType::Ok != rv) {                                                \
      return rv;                                                               \
    }                                                                          \
  } while (0)
enum class ResultType {
  Error, // Generic error
  Ok,
  OutOfRange,
  InvalidOpcode,
};

constexpr std::uint16_t StdRegisterCount = 0x10;
constexpr std::uint16_t MemorySize = 0x1000;
constexpr std::uint16_t StackSize = 0x20;
constexpr std::uint16_t ProgramStartLocation = 0x200;

} // namespace Chip8
