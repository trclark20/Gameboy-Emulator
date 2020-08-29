#include <iostream>
#include "gameboy.h"

int main(int argc, char** argv)
{
    gameboy* _gb = new gameboy();
    _gb->loadGame(argv[1]);

}