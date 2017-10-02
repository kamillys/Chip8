#pragma once

namespace Chip8 {
class Board;
}

#include "Cpu.h"
#include "Memory.h"
#include "Video.h"
#include <memory>
#include <vector>

namespace Chip8 {

class Board {
  std::shared_ptr<Cpu> m_cpu;
  std::shared_ptr<Memory> m_memory;
  std::shared_ptr<Video> m_video;
  std::array<bool, 16> m_keys;
  bool m_break = false;
  bool m_shutdown = false;

public:
  Board(std::shared_ptr<Video> video);
  void LoadBinary(std::vector<uint8_t> data, std::size_t offset = 0x200);
  Cpu *cpu();
  Memory *memory();
  Video *video();

  void reset();
  bool shutdown() const;
  void setShutdown();

  void setBreak(bool v) { m_break = v; }
  bool isBreak() const { return m_break; }

  void handleKey(uint8_t key, bool down);
  bool isKeyDown(uint8_t key);
};
}
