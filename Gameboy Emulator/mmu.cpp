#include "mmu.h"
#include "util.h"
#include <malloc.h>
#include <stdio.h>
#include <string>
#include <iostream>

mmu::mmu()
{
	finishedBios = internMem.addr + 0xFF50;
}

void mmu::LoadBios()
{
	memcpy((void*)bios, (const void*)BIOS, sizeof(BIOS));
	memcpy((void*)internMem.addr, (const void*)bios, sizeof(bios));
	*finishedBios = false;
}

uint8_t mmu::ReadMemory(uint16_t address) const
{
	////reading from rom
	//if ((address >= 0x4000) && (address <= 0x7FFF))
	//{
	//	uint16_t newAddress = address - 0x4000;
	//	return cmem[newAddress + (CurrentROMBank * 0x4000)];
	//}
	//
	////reading from ram memory bank
	//if ((address >= 0xA000) && (address <= 0xBFFF))
	//{
	//	uint16_t newAddress = address - 0xA000;
	//	return RAMbanks[newAddress + (CurrentRAMBank * 0x2000)];
	//}
	//
	//return mem[address];

	//if finishedBios == false, we are running the boot bios
	//write to 0xC1D8 causes issues with *finishedBios?????
	if (!(*finishedBios) && ((address >= 0x00) && (address <= 0xFF)))
		return bios[address];

	bool exists = std::find(std::begin(cartHeadAddresses), std::end(cartHeadAddresses), address) != std::end(cartHeadAddresses);
	bool exists2 = std::find(std::begin(cartChecksumAddresses), std::end(cartChecksumAddresses), address) != std::end(cartChecksumAddresses);
	if (!(*finishedBios) && (exists))
		return internMem.rom[address];

	if (!(*finishedBios) && (exists2))
		return internMem.rom[address];

	if (*finishedBios && (address >= 0x0000 && address <= 0x7FFF))
		return internMem.rom[address];

	if (*finishedBios && (address >= 0xC000 && address <= 0xDFFF))
		return internMem.wram[address - 0xC000];

	return internMem.addr[(address)];
}

void mmu::WriteMemory(uint16_t address, uint8_t data)
{
	////rom/ram banking
	//if (address <= 0x7FFF)
	//	HandleBanking(address, data);
	//
	//else if ((address >= 0xA000) && (address <= 0xBFFF))
	//{
	//	if (EnableRAM)
	//	{
	//		uint16_t newAddress = address - 0xA000;
	//		RAMbanks[newAddress + (CurrentRAMBank * 0x2000)] = data;
	//	}
	//}
	//
	////read only memory
	//if (address <= 0x7FFF)
	//{
	//}
	//
	////writing to echo ram, write to RAM too
	//else if ((address >= 0xE000) && (address <= 0xFDFF))
	//{
	//	mem[address] = data;
	//	WriteMemory(address - 0x2000, data);
	//}
	//
	////restricted
	//else if ((address >= 0xFEA0) && (address <= 0xFEFE))
	//{
	//}
	//
	//else
	//	mem[address] = data;
	
	//debug
	if (address == 0xFF02)
		printf("0xFF01 VALUE: %08X\n", ReadMemory(0xFF01));

	//cannot write to ROM
	else if (address <= 0x7FFF)
	{
	}
	//echo ram writes to regular ram also
	else if (address >= 0xE000 && address <= 0xFDFF)
	{
		internMem.wrams[address - 0xE000] = data;
		WriteMemory(address - 0x2000, data);
	}
	else if (address >= 0xC000 && address <= 0xDFFF)
		internMem.wram[address - 0xC000] = data;
	else
		internMem.addr[address] = data;

	//check if finished with bootstrap
	if (internMem.addr[0xFF50] == 1 && (*finishedBios != true))
	{
		*finishedBios = true;
	}
}

void mmu::HandleBanking(uint16_t address, uint8_t data)
{
	//do ram enabling
	if (address <= 0x1FFF)
	{
		if (MBC1 || MBC2)
			DoRAMBankEnable(address, data);
	}

	//do rom bank change
	else if ((address >= 0x200) && (address <= 0x3FFF))
	{
		if (MBC1 || MBC2)
			DoChangeLoROMBank(data);
	}

	//do rom or ram bank change
	else if ((address >= 0x4000) && (address < 0x6000))
	{
		//no rambank in mcb2 so use 0 always
		if (MBC1)
		{
			if (ROMBanking)
				DoChangeHiROMBank(data);
			else
				DoRAMBankChange(data);
		}
	}

	//changes whether rom banking or ram banking with above if statement
	else if ((address >= 0x6000) && (address < 0x8000))
	{
		if (MBC1)
			DoChangeROMRAMMode(data);
	}
}

void mmu::DoRAMBankEnable(uint16_t address, uint8_t data)
{
	if (MBC2)
	{
		//TODO fix this maybe
		if (utility.TestBit((uint8_t)(address >> 8), 0) == 1) //LSB of upper byte must be 0 to continue
			return;
	}

	uint8_t testData = data & 0xF;
	if (testData == 0xA)
		EnableRAM = true;
	else if (testData == 0x0)
		EnableRAM = false;
}

void mmu::DoChangeLoROMBank(uint8_t data)
{
	if (MBC2)
	{
		CurrentROMBank = data & 0xF;
		if (CurrentROMBank == 0)
			CurrentROMBank++;
		return;
	}

	uint8_t lower5 = data & 31;
	CurrentROMBank &= 224; //turn off lower 5
	CurrentROMBank |= lower5;
	if (CurrentROMBank == 0)
		CurrentROMBank++;
}

void mmu::DoChangeHiROMBank(uint8_t data)
{
	//turn off upper 3 bits of current rom
	CurrentROMBank &= 31;

	//turn off lower 5 bits of data
	data &= 224;
	CurrentROMBank |= data;
	if (CurrentROMBank == 0)
		CurrentROMBank++;
}

void mmu::DoRAMBankChange(uint8_t data)
{
	CurrentRAMBank = data & 0x3;
}

void mmu::DoChangeROMRAMMode(uint8_t data)
{
	uint8_t newData = data & 0x1;
	ROMBanking = (newData == 0) ? true : false;
	if (ROMBanking)
		CurrentRAMBank = 0;
}