#include <iostream>
#include "gameboy.h"
#include "mmu.h"

int main(int argc, char** argv)
{
    mmu memory;
    gameboy _gb(memory);

    _gb.initialize();
    _gb.loadGame(argv[1]);

    bool quit = false;

    while(!quit)
        _gb.ExecuteNextOpcode();
}