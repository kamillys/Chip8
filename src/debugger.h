#pragma once

#include <Chip8/Board.h>

namespace Chip8 {

class Debugger {
  std::shared_ptr<Board> m_board;

public:
  Debugger(const std::shared_ptr<Board> &board);
  ~Debugger() = default;
  Debugger(const Debugger &) = delete;
  Debugger &operator=(const Debugger &) = delete;

  Board *board();

  void dumpMemory(uint16_t offset, size_t size);
  void dumpDisasm(uint16_t offset, size_t count);
  // void dumpSprite(uint16_t offset, size_t count);
  void dumpSpriteLine(uint16_t offset);

  void debugger_loop();
};
}
