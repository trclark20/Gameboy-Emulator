#include "gameboy.h"
#include <stdio.h>
#include <string>

void gameboy::initialize()
{
	//Initialize registers, memory, etc.
	PC = 0x100;
	opcode = 0x0;
	SP = 0xFFFE;
	RegisterAF.reg = 0x01B0;
	RegisterBC.reg = 0x0013;
	RegisterDE.reg = 0x00D8;
	RegisterHL.reg = 0x014D;

	//clear display
	for (int i = 0; i < 160 * 144; i++)
		gfx[i] = 0x0;

	//Clear memory, cart memory
	for (int i = 0; i < 0x10000; i++)
	{
		mem[i] = 0x0;
	}

	for (int i = 0; i < 0x200000; i++)
	{
		cmem[i] = 0x0;
	}

	//Initialize ram
	mem[0xFF05] = 0x00;
	mem[0xFF06] = 0x00;
	mem[0xFF07] = 0x00;
	mem[0xFF10] = 0x80;
	mem[0xFF11] = 0xBF;
	mem[0xFF12] = 0xF3;
	mem[0xFF14] = 0xBF;
	mem[0xFF16] = 0x3F;
	mem[0xFF17] = 0x00;
	mem[0xFF19] = 0xBF;
	mem[0xFF1A] = 0x7F;
	mem[0xFF1B] = 0xFF;
	mem[0xFF1C] = 0x9F;
	mem[0xFF1E] = 0xBF;
	mem[0xFF20] = 0xFF;
	mem[0xFF21] = 0x00;
	mem[0xFF22] = 0x00;
	mem[0xFF23] = 0xBF;
	mem[0xFF24] = 0x77;
	mem[0xFF25] = 0xF3;
	mem[0xFF26] = 0xF1;
	mem[0xFF40] = 0x91;
	mem[0xFF42] = 0x00;
	mem[0xFF43] = 0x00;
	mem[0xFF45] = 0x00;
	mem[0xFF47] = 0xFC;
	mem[0xFF48] = 0xFF;
	mem[0xFF49] = 0xFF;
	mem[0xFF4A] = 0x00;
	mem[0xFF4B] = 0x00;
	mem[0xFFFF] = 0x00;
	
	//Rom/Ram banking intialize
	CurrentROMBank = 1;

	memset(&RAMbanks, 0, sizeof(RAMbanks));
	CurrentRAMBank = 0;
}

void gameboy::loadGame(const char* filePath)
{
	_CRT_SECURE_NO_WARNINGS;
	memset(cmem, 0, sizeof(cmem));

	FILE* file;
	file = fopen(filePath, "rb");
	fread(cmem, 1, 0x200000, file);
	fclose(file);

	//rom banking
	switch (cmem[0x147])
	{
	case 1: MBC1 = true; break;
	case 2: MBC1 = true; break;
	case 3: MBC1 = true; break;
	case 5: MBC2 = true; break;
	case 6: MBC2 = true; break;
	default: break;
	}

	//ram banking
	switch (cmem[0x149])
	{
	case 1: 
	}
}

int gameboy::executeOpcode(uint8_t opcode)
{
	switch (opcode)
	{
		//8bit load/store/move instructions
	case 0x02:
		CPU_8BIT_LOAD()


	}
}

uint8_t gameboy::ReadMemory(uint16_t address) const
{
	//reading from rom
	if ((address >= 0x4000) && (address <= 0x7FFF))
	{
		uint16_t newAddress = address - 0x4000;
		return cmem[newAddress + ()]
	}
}

void gameboy::CPU_8BIT_LOAD(uint8_t reg)
{
	uint8_t n = ReadMemory(PC);
	PC++;
	reg = n;
}