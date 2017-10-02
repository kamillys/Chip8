#pragma once

#include <Chip8/Board.h>

namespace Chip8 {

class Debugger {
  std::shared_ptr<Board> m_board;

  Board *board();

public:
  Debugger(const std::shared_ptr<Board> &board);
  ~Debugger() = default;
  Debugger(const Debugger &) = delete;
  Debugger &operator=(const Debugger &) = delete;

  void dumpMemory(std::uint16_t offset, size_t size);
  void dumpDisasm(std::uint16_t offset, size_t count);
  void dumpCpu();
  // void dumpSprite(std::uint16_t offset, size_t count);
  void dumpSpriteLine(std::uint16_t offset);

  void debugger_loop();
};

} // namespace Chip8
