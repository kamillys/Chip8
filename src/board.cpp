#include <Chip8/Board.h>
namespace Chip8 {

Board::Board(std::shared_ptr<Video> video) {
    m_video = video;
    m_cpu = std::make_shared<Chip8::Cpu>();
    m_memory = std::make_shared<Chip8::Memory>();
    m_cpu->setBoard(this);
    reset();
}

void Board::LoadBinary(std::vector<uint8_t> data, std::size_t offset)
{
    auto memory = this->memory();
    memory->write_bulk(offset, data);
}

Memory *Board::memory()
{
    return m_memory.get();
}

void Board::reset()
{
    m_shutdown = false;
    m_memory->reset();
    std::fill(m_keys.begin(), m_keys.end(), false);
}

bool Board::shutdown() const
{
    return m_shutdown;
}

void Board::setShutdown()
{
    m_shutdown = true;
}

void Board::handleKey(uint8_t key, bool down)
{
    m_keys[key] = down;
    if (down)
    {
        fprintf(stderr, "KEYPRESS: %X\n", key);
    }
    else
    {
        fprintf(stderr, "KEYRELEA: %X\n", key);
    }
}

bool Board::isKeyDown(uint8_t key)
{
    return m_keys[key];
}

Cpu *Board::cpu(){return m_cpu.get();}
Video *Board::video() { return m_video.get(); }

}
