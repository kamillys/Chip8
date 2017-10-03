#pragma once

namespace Chip8 {
class Board;
} // namespace Chip8

#include <Chip8/Cpu.h>
#include <Chip8/Memory.h>
#include <Chip8/Video.h>
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

  CHIP8_DEPRECATED Memory *memory();

public:
  Board(std::shared_ptr<Video> video);
  void LoadBinary(std::vector<uint8_t> data,
                  std::size_t offset = Chip8::ProgramStartLocation);

  CHIP8_DEPRECATED Cpu *cpu();
  CHIP8_DEPRECATED Video *video();

  void reset();
  bool shutdown() const;
  void setShutdown();
  void step();
  void timerStep();

  void setBreak(bool v) { m_break = v; }
  bool isBreak() const { return m_break; }

  void handleKey(uint8_t key, bool down);
  bool isKeyDown(uint8_t key);

  // Memory access over board
  CHIP8_WARN_UNUSED ResultType memoryWrite(std::uint16_t addr,
                                           std::uint8_t val);
  CHIP8_WARN_UNUSED ResultType memoryRead(std::uint16_t addr,
                                          std::uint8_t &out);
  CHIP8_WARN_UNUSED ResultType memoryRead(std::uint16_t addr,
                                          std::uint16_t &out);
  CHIP8_WARN_UNUSED ResultType fontPtr(uint8_t font, uint16_t &offset);

  // Video access over board
  void clearScreen();
  void flipSprite(uint8_t x, uint8_t y, uint8_t sprite, bool &result);

  // Audio access over board
  void startBeep();
  void stopBeep();
};
} // namespace Chip8
