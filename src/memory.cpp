#include <Chip8/Memory.h>

namespace Chip8
{
class Memory;

void Memory::reset()
{
    std::fill(m_data.begin(), m_data.end(), 0);
    //Store font data
    const std::vector<std::vector<uint8_t>> font_data = {
        /* 0 */ { 0xF0, 0x90, 0x90, 0x90, 0xF0},
        /* 1 */ { 0x20, 0x60, 0x20, 0x20, 0x70},
        /* 2 */ { 0xF0, 0x10, 0xF0, 0x80, 0xF0},
        /* 3 */ { 0xF0, 0x10, 0xF0, 0x10, 0xF0},
        /* 4 */ { 0x90, 0x90, 0xF0, 0x10, 0x10},
        /* 5 */ { 0xF0, 0x80, 0xF0, 0x10, 0xF0},
        /* 6 */ { 0xF0, 0x80, 0xF0, 0x90, 0xF0},
        /* 7 */ { 0xF0, 0x10, 0x20, 0x40, 0x40},
        /* 8 */ { 0xF0, 0x90, 0xF0, 0x90, 0xF0},
        /* 9 */ { 0xF0, 0x90, 0xF0, 0x10, 0xF0},
        /* A */ { 0xF0, 0x90, 0xF0, 0x90, 0x90},
        /* B */ { 0xE0, 0x90, 0xE0, 0x90, 0xE0},
        /* C */ { 0xF0, 0x80, 0x80, 0x80, 0xF0},
        /* D */ { 0xE0, 0x90, 0x90, 0x90, 0xE0},
        /* E */ { 0xF0, 0x80, 0xF0, 0x80, 0xF0},
        /* F */ { 0xF0, 0x80, 0xF0, 0x80, 0x80},
    };
    for (uint8_t font = 0; font < font_data.size(); ++font)
    {
        size_t offset;
        font_ptr(font, offset); //TODO
        for(const auto &sprite : font_data[font])
        {
            write(offset++, sprite); // TODO
        }
    }
}

int Memory::read(size_t offset, uint8_t &data)
{
    if (offset >= m_data.size())
        return 0;
    data = m_data.at(offset);
    return 1;
}

int Memory::write(size_t offset, uint8_t data)
{
    if (offset >= m_data.size())
        return 0;
    m_data.at(offset) = data;
    return 1;
}

int Memory::write_bulk(size_t offset, const std::vector<uint8_t> &data)
{
    int rv = 0;
    for (auto b : data)
    {
        int tmp = write(offset++, b);
        if (0 == tmp)
            return rv;
        rv += tmp;
    }
    return rv;
}

int Memory::font_ptr(uint8_t font, size_t &offset)
{
    if (font > 0x0F)
    {
        return 0;
    }
    offset = 0x050 + font * 5;
    return 1;
}

}
