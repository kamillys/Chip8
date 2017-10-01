#include "debugger.h"
#include <readline/readline.h>
#include <readline/history.h>

namespace Chip8 {

Debugger::Debugger(const std::shared_ptr<Board> &board) : m_board(board){}

Board *Debugger::board(){return m_board.get();}

void Debugger::dumpMemory(uint16_t offset, size_t size)
{
    size_t dump_line = size/16;
    // Display 16 bytes of memory
    uint8_t byte = 0;
    for (; dump_line; dump_line--)
    {
        std::printf("0x%.3x\t", offset);
        size_t dump_size = 16;
        for (; dump_size; dump_size--)
        {
            if (dump_size % 4 == 0)
            {
                std::printf("  ");
            }
            if (dump_size % 2 == 0)
            {
                std::printf("  ");
            }
            if (1 == board()->memory()->read(offset++, byte))
            {
                std::printf("%.2x ", byte);
            }
        }
        std::printf("\n");
    }
    std::printf("\n");
}

void Debugger::dumpDisasm(uint16_t offset, size_t count)
{
    for(;count > 0;--count, offset+=2)
    {
        uint16_t opcode;
        if (1 == board()->cpu()->fetch(offset, opcode))
        {
            Instruction instr(opcode);
            std::printf("0x%.3X\t%.4X\t%s\n", offset, opcode, instr.disasm().c_str());
        }
    }
}

void Debugger::dumpSpriteLine(uint16_t offset)
{
    uint8_t sprite;
    if (1 != board()->memory()->read(offset, sprite))
        return;
    std::string line;
    line.resize(8);
    for (uint it = 0; it < 8; ++it)
    {
        uint8_t v = (sprite & (0x1 << (7-it))) >> (7-it);
        line[it] = (v == 0) ? '_' : '*';
    }
    std::printf("0x%.3X\t%.2X\t[%s]\n", offset, sprite, line.c_str());
}

void Debugger::debugger_loop()
{
    int rv;
    while(true)
    {
        char *line = readline("> ");
        if (nullptr == line){
            continue;
        }

        line = strtok(line, " ");
        if (nullptr == line){
            continue;
        }

        if (false){}
        else if (0==strcmp(line, "q"))
        {
            std::fprintf(stderr, "Quit\n");
            board()->setShutdown();
            board()->setBreak(false);
            return;
        }
        else if (0==strcmp(line, "cpu"))
        {
            board()->cpu()->dump();
        }
        else if (0==strcmp(line, "s"))
        {
            std::fprintf(stderr, "single cpu step\n");
            return;
        }
        else if (0==strcmp(line, "c"))
        {
            std::fprintf(stderr, "Continue\n");
            board()->setBreak(false);
            return;
        }
        else if (0==strcmp(line, "da"))
        {
            std::fprintf(stderr, "Disasm\n");
            uint32_t offset = board()->cpu()->pc();
            const size_t instrCount = 8;
            line += strlen(line);
            line = strtok(NULL, " ");
            if (nullptr == line)
            {
                dumpDisasm(offset, instrCount);
                continue;
            }
            rv = std::sscanf(line, "0x%x", &offset);
            if (rv != 1)
            {
                rv = std::sscanf(line, "%u", &offset);
            }
            dumpDisasm(offset, instrCount);
        }
        else if (0==strcmp(line, "d"))
        {
            line += strlen(line);
            line = strtok(NULL, " ");
            if (nullptr == line)
            {
                std::fprintf(stderr, "USAGE: d OFFSET\n");
                continue;
            }
            uint32_t offset = 0;
            rv = std::sscanf(line, "0x%x", &offset);
            if (rv != 1)
            {
                rv = std::sscanf(line, "%u", &offset);
            }
            dumpMemory(offset, 16*16);
        }
        else if (0==strcmp(line, "i"))
        {
            std::fprintf(stderr, "Print image line\n");
            uint32_t offset = board()->cpu()->I();
            line += strlen(line);
            line = strtok(NULL, " ");
            if (nullptr == line)
            {
                dumpSpriteLine(offset);
                continue;
            }
            rv = std::sscanf(line, "0x%x", &offset);
            if (rv != 1)
            {
                rv = std::sscanf(line, "%u", &offset);
            }
            dumpSpriteLine(offset);
        }
        else if (0==strcmp(line, "v"))
        {
            std::fprintf(stderr, "dump video\n");
            board()->video()->dump();
        }
        else
        {
            std::fprintf(stderr, "Unknown command [%s]\n", line);
        }
    }
}

}
