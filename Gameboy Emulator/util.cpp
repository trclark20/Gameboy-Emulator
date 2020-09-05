#include "util.h"
#include "mmu.h"
#include <iostream>
#include <string>

int util::TestBit(uint8_t source, int bitNumber)
{
	switch (bitNumber)
	{
		case 7: return (((source & 0x80) >> 7) == 1) ? 1 : 0;
		case 6: return (((source & 0x40) >> 6) == 1) ? 1 : 0;
		case 5: return (((source & 0x20) >> 5) == 1) ? 1 : 0;
		case 4: return (((source & 0x10) >> 4) == 1) ? 1 : 0;
		case 3: return (((source & 0x08) >> 3) == 1) ? 1 : 0;
		case 2: return (((source & 0x04) >> 2) == 1) ? 1 : 0;
		case 1: return (((source & 0x02) >> 1) == 1) ? 1 : 0;
		case 0: return (((source & 0x01) >> 0) == 1) ? 1 : 0;
	}
}

void util::SetBreakpoint(uint16_t reg, uint16_t address)
{
	std::string str;
	if (reg == address)
	{
		std::cout << "PC = 0x" << std::hex << address;
		std::getline(std::cin, str);
	}
}