#include <Chip8/Board.h>
namespace Chip8 {

Board::Board(std::shared_ptr<Video> video) {
  m_video = video;
  m_cpu = std::make_shared<Chip8::Cpu>();
  m_memory = std::make_shared<Chip8::Memory>();
  reset();
}

void Board::LoadBinary(std::vector<uint8_t> data, std::size_t offset) {
  memory()->write_bulk(offset, data);
}

Memory *Board::memory() { return m_memory.get(); }

void Board::reset() {
  m_shutdown = false;
  m_memory->reset();
  std::fill(m_keys.begin(), m_keys.end(), false);
}

bool Board::shutdown() const { return m_shutdown; }

void Board::setShutdown() { m_shutdown = true; }

void Board::step() { cpu()->step(this); }

void Board::timerStep() { cpu()->timerStep(this); }

void Board::handleKey(uint8_t key, bool down) {
  m_keys[key] = down;
  if (down) {
    cpu()->keyStep(this, key);
    fprintf(stderr, "KEYPRESS: %X\n", key);
  } else {
    fprintf(stderr, "KEYRELEA: %X\n", key);
  }
}

bool Board::isKeyDown(uint8_t key) {
  // bool rv = m_keys[key];
  // m_keys[key] = false;
  // return rv;
  return m_keys[key];
}

ResultType Board::memoryWrite(uint16_t addr, uint8_t val) {
  return memory()->write(addr, val);
}

ResultType Board::memoryRead(uint16_t addr, uint8_t &out) {
  return memory()->read(addr, out);
}

ResultType Board::memoryRead(uint16_t addr, uint16_t &out) {
  ResultType rv;
  uint8_t top;
  uint8_t bottom;
  rv = memoryRead(addr, top);
  CHIP8_CHECK_RESULT(rv);
  rv = memoryRead(addr + 1, bottom);
  CHIP8_CHECK_RESULT(rv);
  out = (top << 8) | bottom;
  return ResultType::Ok;
}

ResultType Board::fontPtr(uint8_t font, uint16_t &offset) {
  return memory()->font_ptr(font, offset);
}

void Board::clearScreen() { video()->clearScreen(); }

void Board::flipSprite(uint8_t x, uint8_t y, uint8_t sprite, bool &result) {
  result = video()->flipSprite(x, y, sprite);
}

void Board::startBeep() {
  // static_assert(false,"");
}

void Board::stopBeep() {
  // static_assert(false,"");
}

Cpu *Board::cpu() { return m_cpu.get(); }
Video *Board::video() { return m_video.get(); }

} // namespace Chip8
