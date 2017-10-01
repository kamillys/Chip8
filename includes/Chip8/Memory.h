#pragma once

namespace Chip8
{
class Memory;
}

#include <array>
#include <vector>

namespace Chip8
{

class Memory
{
    std::array<uint8_t, 0x1000> m_data;
public:
    void reset();
    //return bytes readen/written
    int read(size_t offset, uint8_t& data);
    int write(size_t offset, uint8_t data);
    int write_bulk(size_t offset, const std::vector<uint8_t>& data);
    int font_ptr(uint8_t font, size_t &offset);
};

}
