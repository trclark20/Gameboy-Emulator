#include "gameboy.h"
#include "util.h"
#include <stdio.h>
#include <string>
#include <iostream>

void gameboy::initialize()
{
	//Clear memory
	for (int i = 0; i < sizeof(memory.internMem.addr); ++i)
		memory.internMem.addr[i] = 0;

	//Run BIOS
	//PC = 0x00;
	//memory.LoadBios();

	//testing
	*memory.finishedBios = true;
	PC = 0x0100;
	RegisterAF.reg = 0x1180;
	RegisterBC.reg = 0x0000;
	RegisterDE.reg = 0xFF56;
	RegisterHL.reg = 0x000D;
	SP = 0xFFFE;

	//Initialize registers, memory, etc.
	//PC = 0x100;
	//opcode = 0x0;
	//SP = 0xFFFE;
	//RegisterAF.reg = 0x01B0;
	//RegisterBC.reg = 0x0013;
	//RegisterDE.reg = 0x00D8;
	//RegisterHL.reg = 0x014D;
	//
	////Initialize ram
	//memory.internMem.addr[0xFF05] = 0x00;
	//memory.internMem.addr[0xFF06] = 0x00;
	//memory.internMem.addr[0xFF07] = 0x00;
	//memory.internMem.addr[0xFF10] = 0x80;
	//memory.internMem.addr[0xFF11] = 0xBF;
	//memory.internMem.addr[0xFF12] = 0xF3;
	//memory.internMem.addr[0xFF14] = 0xBF;
	//memory.internMem.addr[0xFF16] = 0x3F;
	//memory.internMem.addr[0xFF17] = 0x00;
	//memory.internMem.addr[0xFF19] = 0xBF;
	//memory.internMem.addr[0xFF1A] = 0x7F;
	//memory.internMem.addr[0xFF1B] = 0xFF;
	//memory.internMem.addr[0xFF1C] = 0x9F;
	//memory.internMem.addr[0xFF1E] = 0xBF;
	//memory.internMem.addr[0xFF20] = 0xFF;
	//memory.internMem.addr[0xFF21] = 0x00;
	//memory.internMem.addr[0xFF22] = 0x00;
	//memory.internMem.addr[0xFF23] = 0xBF;
	//memory.internMem.addr[0xFF24] = 0x77;
	//memory.internMem.addr[0xFF25] = 0xF3;
	//memory.internMem.addr[0xFF26] = 0xF1;
	//memory.internMem.addr[0xFF40] = 0x91;
	//memory.internMem.addr[0xFF42] = 0x00;
	//memory.internMem.addr[0xFF43] = 0x00;
	//memory.internMem.addr[0xFF45] = 0x00;
	//memory.internMem.addr[0xFF47] = 0xFC;
	//memory.internMem.addr[0xFF48] = 0xFF;
	//memory.internMem.addr[0xFF49] = 0xFF;
	//memory.internMem.addr[0xFF4A] = 0x00;
	//memory.internMem.addr[0xFF4B] = 0x00;
	//memory.internMem.addr[0xFFFF] = 0x00;
	
	//FORCE VBLANK
	memory.internMem.addr[0xFF44] = 0x90;

	//Rom/Ram banking intialize
	memory.CurrentROMBank = 1;
	memset(&memory.RAMbanks, 0, sizeof(memory.RAMbanks));
	memory.CurrentRAMBank = 0;
}

void gameboy::loadGame(const char* filePath)
{
	_CRT_SECURE_NO_WARNINGS;
	//memset(mmu.mmu_t.rom, 0, sizeof(cmem));

	FILE* file;
	fopen_s(&file, filePath, "rb");
	if (file)
	{
		fseek(file, 0, SEEK_END);
		uint32_t total = ftell(file);
		fseek(file, 0, SEEK_SET);
		if (total > 0)
		{
			fread(memory.internMem.rom, 1, total, file);
		}
	}
	fclose(file);

	//rom banking
	switch (memory.internMem.rom[0x147])
	{
		case 1: memory.MBC1 = true; break;
		case 2: memory.MBC1 = true; break;
		case 3: memory.MBC1 = true; break;
		case 5: memory.MBC2 = true; break;
		case 6: memory.MBC2 = true; break;
		default: break;
	}
}

int gameboy::ExecuteNextOpcode()
{
	int result = 0;

	uint8_t opcode = memory.ReadMemory(PC);

	//utility.SetBreakpoint(PC, 0xC375);

	//skip bootloader
	//utility.SetBreakpoint(PC, 0x0100);
	//utility.SetBreakpoint(PC, 0x020F);
	//utility.SetBreakpoint(PC, 0xC246);
	utility.SetBreakpoint(PC, 0xC012);
	//utility.SetBreakpoint(PC, 0xC375);
	//utility.SetBreakpoint(PC, 0xC3AE);
	//utility.SetBreakpoint(PC, 0xC3E7);
	//utility.SetBreakpoint(PC, 0xC3AF);
	//utility.SetBreakpoint(PC, 0xC3D8);

	//utility.SetBreakpoint(PC, 0xD801);

	//doesnt make it
	//utility.SetBreakpoint(PC, 0xC24F);

	//end of program
	utility.SetBreakpoint(PC, 0xC7D2);

	if (PC >= 0x0100)
	{
		std::cout << std::string(100, '\n');
		printf("AF: %04X\n", RegisterAF.reg);
		printf("BC: %04X\n", RegisterBC.reg);
		printf("DE: %04X\n", RegisterDE.reg);
		printf("HL: %04X\n", RegisterHL.reg);
		printf("SP: %04X\n", SP);
		printf("PC: %04X\n", PC);
	}
	PC++;

	result = ExecuteOpcode(opcode);
	return result;
}

int gameboy::ExecuteOpcode(uint8_t opcode)
{
	//printf("Executing opcode: %08X\n", opcode);
	switch (opcode)
	{
		//relative jumps
		case 0x18: return CPU_JUMP_RELATIVE();
		case 0x20: return CPU_JUMP_RELATIVE_F(FLAG_Z, false);
		case 0x28: return CPU_JUMP_RELATIVE_F(FLAG_Z, true);
		case 0x30: return CPU_JUMP_RELATIVE_F(FLAG_C, false);
		case 0x38: return CPU_JUMP_RELATIVE_F(FLAG_C, true);

		//returns
		case 0xC0: return CPU_RETURN_F(FLAG_Z, false);
		case 0xC8: return CPU_RETURN_F(FLAG_Z, true);
		case 0xC9: return CPU_RETURN();
		case 0xD0: return CPU_RETURN_F(FLAG_C, false);
		case 0xD8: return CPU_RETURN_F(FLAG_C, true);
		case 0xD9: return CPU_RETURN(); //TODO: add interupts (before CPU_RETURN)

		//immediate jumps
		case 0xC2: return CPU_JUMP_IMMEDIATE_F(FLAG_Z, false);
		case 0xC3: return CPU_JUMP_IMMEDIATE(false);
		case 0xCA: return CPU_JUMP_IMMEDIATE_F(FLAG_Z, true);
		case 0xD2: return CPU_JUMP_IMMEDIATE_F(FLAG_C, false);
		case 0xDA: return CPU_JUMP_IMMEDIATE_F(FLAG_C, true);
		case 0xE9: return CPU_JUMP_IMMEDIATE(true);

			//stack should be c378 but is c376, issues :(((((
		//calls
		case 0xC4: return CPU_CALL_F(FLAG_Z, false);
		case 0xCC: return CPU_CALL_F(FLAG_Z, true);
		case 0xCD: return CPU_CALL();
		case 0xD4: return CPU_CALL_F(FLAG_C, false);
		case 0xDC: return CPU_CALL_F(FLAG_C, true);

		//resets
		case 0xC7: return CPU_RST(0x00);
		case 0xCF: return CPU_RST(0x08);
		case 0xD7: return CPU_RST(0x10);
		case 0xDF: return CPU_RST(0x18);
		case 0xE7: return CPU_RST(0x20);
		case 0xEF: return CPU_RST(0x28);
		case 0xF7: return CPU_RST(0x30);
		case 0xFF: return CPU_RST(0x38);

		//16bit loads
		case 0x01: return CPU_16BIT_LOAD_IM(RegisterBC.reg);
		case 0x08: return CPU_16BIT_LOAD_IM_MEM_SP();
		case 0x11: return CPU_16BIT_LOAD_IM(RegisterDE.reg);
		case 0x21: return CPU_16BIT_LOAD_IM(RegisterHL.reg);
		case 0x31: return CPU_16BIT_LOAD_IM(SP);
		case 0xF8: return CPU_16BIT_LOAD_HL_SP_IM();
		case 0xF9: return CPU_16BIT_LOAD_SP_HL();

		//pops
		case 0xC1: return CPU_POP(RegisterBC.reg);
		case 0xD1: return CPU_POP(RegisterDE.reg);
		case 0xE1: return CPU_POP(RegisterHL.reg);
		case 0xF1: return CPU_POP(RegisterAF.reg);

		//pushes
		case 0xC5: return CPU_PUSH(RegisterBC.reg);
		case 0xD5: return CPU_PUSH(RegisterDE.reg);
		case 0xE5: return CPU_PUSH(RegisterHL.reg);
		case 0xF5: return CPU_PUSH(RegisterAF.reg);

		//controls
		case 0x00: return CPU_NOP();
		case 0x10: return CPU_STOP();
		case 0x76: return CPU_HALT();
		case 0xF3: return CPU_DI();
		case 0xFB: return CPU_EI();
		case 0xCB: 
		{
			uint8_t opcode = memory.ReadMemory(PC);
			PC++;
			switch (opcode)
			{
				case 0x00: return CPU_RLC(RegisterBC.high);
				case 0x01: return CPU_RLC(RegisterBC.low);
				case 0x02: return CPU_RLC(RegisterDE.high);
				case 0x03: return CPU_RLC(RegisterDE.low);
				case 0x04: return CPU_RLC(RegisterHL.high);
				case 0x05: return CPU_RLC(RegisterHL.low);
				case 0x06: return CPU_RLC_HL();
				case 0x07: return CPU_RLC(RegisterAF.high);
				case 0x08: return CPU_RRC(RegisterBC.high);
				case 0x09: return CPU_RRC(RegisterBC.low);
				case 0x0A: return CPU_RRC(RegisterDE.high);
				case 0x0B: return CPU_RRC(RegisterDE.low);
				case 0x0C: return CPU_RRC(RegisterHL.high);
				case 0x0D: return CPU_RRC(RegisterHL.low);
				case 0x0E: return CPU_RRC_HL();
				case 0x0F: return CPU_RRC(RegisterAF.high);
				case 0x10: return CPU_RL(RegisterBC.high);
				case 0x11: return CPU_RL(RegisterBC.low);
				case 0x12: return CPU_RL(RegisterDE.high);
				case 0x13: return CPU_RL(RegisterDE.low);
				case 0x14: return CPU_RL(RegisterHL.high);
				case 0x15: return CPU_RL(RegisterHL.low);
				case 0x16: return CPU_RL_HL();
				case 0x17: return CPU_RL(RegisterAF.high);
				case 0x18: return CPU_RR(RegisterBC.high);
				case 0x19: return CPU_RR(RegisterBC.low);
				case 0x1A: return CPU_RR(RegisterDE.high);
				case 0x1B: return CPU_RR(RegisterDE.low);
				case 0x1C: return CPU_RR(RegisterHL.high);
				case 0x1D: return CPU_RR(RegisterHL.low);
				case 0x1E: return CPU_RR_HL();
				case 0x1F: return CPU_RR(RegisterAF.high);
				case 0x20: return CPU_SLA(RegisterBC.high);
				case 0x21: return CPU_SLA(RegisterBC.low);
				case 0x22: return CPU_SLA(RegisterDE.high);
				case 0x23: return CPU_SLA(RegisterDE.low);
				case 0x24: return CPU_SLA(RegisterHL.high);
				case 0x25: return CPU_SLA(RegisterHL.low);
				case 0x26: return CPU_SLA_HL();
				case 0x27: return CPU_SLA(RegisterAF.high);
				case 0x28: return CPU_SRA(RegisterBC.high);
				case 0x29: return CPU_SRA(RegisterBC.low);
				case 0x2A: return CPU_SRA(RegisterDE.high);
				case 0x2B: return CPU_SRA(RegisterDE.low);
				case 0x2C: return CPU_SRA(RegisterHL.high);
				case 0x2D: return CPU_SRA(RegisterHL.low);
				case 0x2E: return CPU_SRA_HL();
				case 0x2F: return CPU_SRA(RegisterAF.high);
				case 0x30: return CPU_SWAP(RegisterBC.high);
				case 0x31: return CPU_SWAP(RegisterBC.low);
				case 0x32: return CPU_SWAP(RegisterDE.high);
				case 0x33: return CPU_SWAP(RegisterDE.low);
				case 0x34: return CPU_SWAP(RegisterHL.high);
				case 0x35: return CPU_SWAP(RegisterHL.low);
				case 0x36: return CPU_SWAP_HL();
				case 0x37: return CPU_SWAP(RegisterAF.high);
				case 0x38: return CPU_SRL(RegisterBC.high);
				case 0x39: return CPU_SRL(RegisterBC.low);
				case 0x3A: return CPU_SRL(RegisterDE.high);
				case 0x3B: return CPU_SRL(RegisterDE.low);
				case 0x3C: return CPU_SRL(RegisterHL.high);
				case 0x3D: return CPU_SRL(RegisterHL.low);
				case 0x3E: return CPU_SRL_HL();
				case 0x3F: return CPU_SRL(RegisterAF.high);
				case 0x40: return CPU_BIT(0, RegisterBC.high);
				case 0x41: return CPU_BIT(0, RegisterBC.low);
				case 0x42: return CPU_BIT(0, RegisterDE.high);
				case 0x43: return CPU_BIT(0, RegisterDE.low);
				case 0x44: return CPU_BIT(0, RegisterHL.high);
				case 0x45: return CPU_BIT(0, RegisterHL.low);
				case 0x46: return CPU_BIT_HL(0);
				case 0x47: return CPU_BIT(0, RegisterAF.high);
				case 0x48: return CPU_BIT(1, RegisterBC.high);
				case 0x49: return CPU_BIT(1, RegisterBC.low);
				case 0x4A: return CPU_BIT(1, RegisterDE.high);
				case 0x4B: return CPU_BIT(1, RegisterDE.low);
				case 0x4C: return CPU_BIT(1, RegisterHL.high);
				case 0x4D: return CPU_BIT(1, RegisterHL.low);
				case 0x4E: return CPU_BIT_HL(1);
				case 0x4F: return CPU_BIT(1, RegisterAF.high);
				case 0x50: return CPU_BIT(2, RegisterBC.high);
				case 0x51: return CPU_BIT(2, RegisterBC.low);
				case 0x52: return CPU_BIT(2, RegisterDE.high);
				case 0x53: return CPU_BIT(2, RegisterDE.low);
				case 0x54: return CPU_BIT(2, RegisterHL.high);
				case 0x55: return CPU_BIT(2, RegisterHL.low);
				case 0x56: return CPU_BIT_HL(2);
				case 0x57: return CPU_BIT(2, RegisterAF.high);
				case 0x58: return CPU_BIT(3, RegisterBC.high);
				case 0x59: return CPU_BIT(3, RegisterBC.low);
				case 0x5A: return CPU_BIT(3, RegisterDE.high);
				case 0x5B: return CPU_BIT(3, RegisterDE.low);
				case 0x5C: return CPU_BIT(3, RegisterHL.high);
				case 0x5D: return CPU_BIT(3, RegisterHL.low);
				case 0x5E: return CPU_BIT_HL(3);
				case 0x5F: return CPU_BIT(3, RegisterAF.high);
				case 0x60: return CPU_BIT(4, RegisterBC.high);
				case 0x61: return CPU_BIT(4, RegisterBC.low);
				case 0x62: return CPU_BIT(4, RegisterDE.high);
				case 0x63: return CPU_BIT(4, RegisterDE.low);
				case 0x64: return CPU_BIT(4, RegisterHL.high);
				case 0x65: return CPU_BIT(4, RegisterHL.low);
				case 0x66: return CPU_BIT_HL(4);
				case 0x67: return CPU_BIT(4, RegisterAF.high);
				case 0x68: return CPU_BIT(5, RegisterBC.high);
				case 0x69: return CPU_BIT(5, RegisterBC.low);
				case 0x6A: return CPU_BIT(5, RegisterDE.high);
				case 0x6B: return CPU_BIT(5, RegisterDE.low);
				case 0x6C: return CPU_BIT(5, RegisterHL.high);
				case 0x6D: return CPU_BIT(5, RegisterHL.low);
				case 0x6E: return CPU_BIT_HL(5);
				case 0x6F: return CPU_BIT(5, RegisterAF.high);
				case 0x70: return CPU_BIT(6, RegisterBC.high);
				case 0x71: return CPU_BIT(6, RegisterBC.low);
				case 0x72: return CPU_BIT(6, RegisterDE.high);
				case 0x73: return CPU_BIT(6, RegisterDE.low);
				case 0x74: return CPU_BIT(6, RegisterHL.high);
				case 0x75: return CPU_BIT(6, RegisterHL.low);
				case 0x76: return CPU_BIT_HL(6);
				case 0x77: return CPU_BIT(6, RegisterAF.high);
				case 0x78: return CPU_BIT(7, RegisterBC.high);
				case 0x79: return CPU_BIT(7, RegisterBC.low);
				case 0x7A: return CPU_BIT(7, RegisterDE.high);
				case 0x7B: return CPU_BIT(7, RegisterDE.low);
				case 0x7C: return CPU_BIT(7, RegisterHL.high);
				case 0x7D: return CPU_BIT(7, RegisterHL.low);
				case 0x7E: return CPU_BIT_HL(7);
				case 0x7F: return CPU_BIT(7, RegisterAF.high);
				case 0x80: return CPU_RES(0, RegisterBC.high);
				case 0x81: return CPU_RES(0, RegisterBC.low);
				case 0x82: return CPU_RES(0, RegisterDE.high);
				case 0x83: return CPU_RES(0, RegisterDE.low);
				case 0x84: return CPU_RES(0, RegisterHL.high);
				case 0x85: return CPU_RES(0, RegisterHL.low);
				case 0x86: return CPU_RES_HL(0);
				case 0x87: return CPU_RES(0, RegisterAF.high);
				case 0x88: return CPU_RES(1, RegisterBC.high);
				case 0x89: return CPU_RES(1, RegisterBC.low);
				case 0x8A: return CPU_RES(1, RegisterDE.high);
				case 0x8B: return CPU_RES(1, RegisterDE.low);
				case 0x8C: return CPU_RES(1, RegisterHL.high);
				case 0x8D: return CPU_RES(1, RegisterHL.low);
				case 0x8E: return CPU_RES_HL(1);
				case 0x8F: return CPU_RES(1, RegisterAF.high);
				case 0x90: return CPU_RES(2, RegisterBC.high);
				case 0x91: return CPU_RES(2, RegisterBC.low);
				case 0x92: return CPU_RES(2, RegisterDE.high);
				case 0x93: return CPU_RES(2, RegisterDE.low);
				case 0x94: return CPU_RES(2, RegisterHL.high);
				case 0x95: return CPU_RES(2, RegisterHL.low);
				case 0x96: return CPU_RES_HL(2);
				case 0x97: return CPU_RES(2, RegisterAF.high);
				case 0x98: return CPU_RES(3, RegisterBC.high);
				case 0x99: return CPU_RES(3, RegisterBC.low);
				case 0x9A: return CPU_RES(3, RegisterDE.high);
				case 0x9B: return CPU_RES(3, RegisterDE.low);
				case 0x9C: return CPU_RES(3, RegisterHL.high);
				case 0x9D: return CPU_RES(3, RegisterHL.low);
				case 0x9E: return CPU_RES_HL(3);
				case 0x9F: return CPU_RES(3, RegisterAF.high);
				case 0xA0: return CPU_RES(4, RegisterBC.high);
				case 0xA1: return CPU_RES(4, RegisterBC.low);
				case 0xA2: return CPU_RES(4, RegisterDE.high);
				case 0xA3: return CPU_RES(4, RegisterDE.low);
				case 0xA4: return CPU_RES(4, RegisterHL.high);
				case 0xA5: return CPU_RES(4, RegisterHL.low);
				case 0xA6: return CPU_RES_HL(4);
				case 0xA7: return CPU_RES(4, RegisterAF.high);
				case 0xA8: return CPU_RES(5, RegisterBC.high);
				case 0xA9: return CPU_RES(5, RegisterBC.low);
				case 0xAA: return CPU_RES(5, RegisterDE.high);
				case 0xAB: return CPU_RES(5, RegisterDE.low);
				case 0xAC: return CPU_RES(5, RegisterHL.high);
				case 0xAD: return CPU_RES(5, RegisterHL.low);
				case 0xAE: return CPU_RES_HL(5);
				case 0xAF: return CPU_RES(5, RegisterAF.high);
				case 0xB0: return CPU_RES(6, RegisterBC.high);
				case 0xB1: return CPU_RES(6, RegisterBC.low);
				case 0xB2: return CPU_RES(6, RegisterDE.high);
				case 0xB3: return CPU_RES(6, RegisterDE.low);
				case 0xB4: return CPU_RES(6, RegisterHL.high);
				case 0xB5: return CPU_RES(6, RegisterHL.low);
				case 0xB6: return CPU_RES_HL(6);
				case 0xB7: return CPU_RES(6, RegisterAF.high);
				case 0xB8: return CPU_RES(7, RegisterBC.high);
				case 0xB9: return CPU_RES(7, RegisterBC.low);
				case 0xBA: return CPU_RES(7, RegisterDE.high);
				case 0xBB: return CPU_RES(7, RegisterDE.low);
				case 0xBC: return CPU_RES(7, RegisterHL.high);
				case 0xBD: return CPU_RES(7, RegisterHL.low);
				case 0xBE: return CPU_RES_HL(7);
				case 0xBF: return CPU_RES(7, RegisterAF.high);
				case 0xC0: return CPU_SET(0, RegisterBC.high);
				case 0xC1: return CPU_SET(0, RegisterBC.low);
				case 0xC2: return CPU_SET(0, RegisterDE.high);
				case 0xC3: return CPU_SET(0, RegisterDE.low);
				case 0xC4: return CPU_SET(0, RegisterHL.high);
				case 0xC5: return CPU_SET(0, RegisterHL.low);
				case 0xC6: return CPU_SET_HL(0);
				case 0xC7: return CPU_SET(0, RegisterAF.high);
				case 0xC8: return CPU_SET(1, RegisterBC.high);
				case 0xC9: return CPU_SET(1, RegisterBC.low);
				case 0xCA: return CPU_SET(1, RegisterDE.high);
				case 0xCB: return CPU_SET(1, RegisterDE.low);
				case 0xCC: return CPU_SET(1, RegisterHL.high);
				case 0xCD: return CPU_SET(1, RegisterHL.low);
				case 0xCE: return CPU_SET_HL(1);
				case 0xCF: return CPU_SET(1, RegisterAF.high);
				case 0xD0: return CPU_SET(2, RegisterBC.high);
				case 0xD1: return CPU_SET(2, RegisterBC.low);
				case 0xD2: return CPU_SET(2, RegisterDE.high);
				case 0xD3: return CPU_SET(2, RegisterDE.low);
				case 0xD4: return CPU_SET(2, RegisterHL.high);
				case 0xD5: return CPU_SET(2, RegisterHL.low);
				case 0xD6: return CPU_SET_HL(2);
				case 0xD7: return CPU_SET(2, RegisterAF.high);
				case 0xD8: return CPU_SET(3, RegisterBC.high);
				case 0xD9: return CPU_SET(3, RegisterBC.low);
				case 0xDA: return CPU_SET(3, RegisterDE.high);
				case 0xDB: return CPU_SET(3, RegisterDE.low);
				case 0xDC: return CPU_SET(3, RegisterHL.high);
				case 0xDD: return CPU_SET(3, RegisterHL.low);
				case 0xDE: return CPU_SET_HL(3);
				case 0xDF: return CPU_SET(3, RegisterAF.high);
				case 0xE0: return CPU_SET(4, RegisterBC.high);
				case 0xE1: return CPU_SET(4, RegisterBC.low);
				case 0xE2: return CPU_SET(4, RegisterDE.high);
				case 0xE3: return CPU_SET(4, RegisterDE.low);
				case 0xE4: return CPU_SET(4, RegisterHL.high);
				case 0xE5: return CPU_SET(4, RegisterHL.low);
				case 0xE6: return CPU_SET_HL(4);
				case 0xE7: return CPU_SET(4, RegisterAF.high);
				case 0xE8: return CPU_SET(5, RegisterBC.high);
				case 0xE9: return CPU_SET(5, RegisterBC.low);
				case 0xEA: return CPU_SET(5, RegisterDE.high);
				case 0xEB: return CPU_SET(5, RegisterDE.low);
				case 0xEC: return CPU_SET(5, RegisterHL.high);
				case 0xED: return CPU_SET(5, RegisterHL.low);
				case 0xEE: return CPU_SET_HL(5);
				case 0xEF: return CPU_SET(5, RegisterAF.high);
				case 0xF0: return CPU_SET(6, RegisterBC.high);
				case 0xF1: return CPU_SET(6, RegisterBC.low);
				case 0xF2: return CPU_SET(6, RegisterDE.high);
				case 0xF3: return CPU_SET(6, RegisterDE.low);
				case 0xF4: return CPU_SET(6, RegisterHL.high);
				case 0xF5: return CPU_SET(6, RegisterHL.low);
				case 0xF6: return CPU_SET_HL(6);
				case 0xF7: return CPU_SET(6, RegisterAF.high);
				case 0xF8: return CPU_SET(7, RegisterBC.high);
				case 0xF9: return CPU_SET(7, RegisterBC.low);
				case 0xFA: return CPU_SET(7, RegisterDE.high);
				case 0xFB: return CPU_SET(7, RegisterDE.low);
				case 0xFC: return CPU_SET(7, RegisterHL.high);
				case 0xFD: return CPU_SET(7, RegisterHL.low);
				case 0xFE: return CPU_SET_HL(7);
				case 0xFF: return CPU_SET(7, RegisterAF.high);

			}
		}

		//8bit logic
		case 0x04: return CPU_8BIT_INC(RegisterBC.high);
		case 0x05: return CPU_8BIT_DEC(RegisterBC.high);
		case 0x0C: return CPU_8BIT_INC(RegisterBC.low);
		case 0x0D: return CPU_8BIT_DEC(RegisterBC.low);
		case 0x14: return CPU_8BIT_INC(RegisterDE.high);
		case 0x15: return CPU_8BIT_DEC(RegisterDE.high);
		case 0x1C: return CPU_8BIT_INC(RegisterDE.low);
		case 0x1D: return CPU_8BIT_DEC(RegisterDE.low);
		case 0x24: return CPU_8BIT_INC(RegisterHL.high);
		case 0x25: return CPU_8BIT_DEC(RegisterHL.high);
		case 0x2C: return CPU_8BIT_INC(RegisterHL.low);
		case 0x2D: return CPU_8BIT_DEC(RegisterHL.low);
		case 0x34: return CPU_8BIT_INC_HL();
		case 0x35: return CPU_8BIT_DEC_HL();
		case 0x3C: return CPU_8BIT_INC(RegisterAF.high);
		case 0x3D: return CPU_8BIT_DEC(RegisterAF.high);
		case 0x27: return CPU_DAA();
		case 0x2F: return CPU_CPL();
		case 0x37: return CPU_SCF();
		case 0x3F: return CPU_CCF();
		case 0x80: return CPU_ADD_A(RegisterBC.high);
		case 0x81: return CPU_ADD_A(RegisterBC.low);
		case 0x82: return CPU_ADD_A(RegisterDE.high);
		case 0x83: return CPU_ADD_A(RegisterDE.low);
		case 0x84: return CPU_ADD_A(RegisterHL.high);
		case 0x85: return CPU_ADD_A(RegisterHL.low);
		case 0x86: return CPU_ADD_A_HL();
		case 0x87: return CPU_ADD_A(RegisterAF.high);
		case 0x88: return CPU_ADC_A(RegisterBC.high);
		case 0x89: return CPU_ADC_A(RegisterBC.low);
		case 0x8A: return CPU_ADC_A(RegisterDE.high);
		case 0x8B: return CPU_ADC_A(RegisterDE.low);
		case 0x8C: return CPU_ADC_A(RegisterHL.high);
		case 0x8D: return CPU_ADC_A(RegisterHL.low);
		case 0x8E: return CPU_ADC_A_HL();
		case 0x8F: return CPU_ADC_A(RegisterAF.high);
		case 0x90: return CPU_SUB_A(RegisterBC.high);
		case 0x91: return CPU_SUB_A(RegisterBC.low);
		case 0x92: return CPU_SUB_A(RegisterDE.high);
		case 0x93: return CPU_SUB_A(RegisterDE.low);
		case 0x94: return CPU_SUB_A(RegisterHL.high);
		case 0x95: return CPU_SUB_A(RegisterHL.low);
		case 0x96: return CPU_SUB_A_HL();
		case 0x97: return CPU_SUB_A(RegisterAF.high);
		case 0x98: return CPU_SBC_A(RegisterBC.high);
		case 0x99: return CPU_SBC_A(RegisterBC.low);
		case 0x9A: return CPU_SBC_A(RegisterDE.high);
		case 0x9B: return CPU_SBC_A(RegisterDE.low);
		case 0x9C: return CPU_SBC_A(RegisterHL.high);
		case 0x9D: return CPU_SBC_A(RegisterHL.low);
		case 0x9E: return CPU_SBC_A_HL();
		case 0x9F: return CPU_SBC_A(RegisterAF.high);
		case 0xA0: return CPU_AND_A(RegisterBC.high);
		case 0xA1: return CPU_AND_A(RegisterBC.low);
		case 0xA2: return CPU_AND_A(RegisterDE.high);
		case 0xA3: return CPU_AND_A(RegisterDE.low);
		case 0xA4: return CPU_AND_A(RegisterHL.high);
		case 0xA5: return CPU_AND_A(RegisterHL.low);
		case 0xA6: return CPU_AND_A_HL();
		case 0xA7: return CPU_AND_A(RegisterAF.high);
		case 0xA8: return CPU_XOR_A(RegisterBC.high);
		case 0xA9: return CPU_XOR_A(RegisterBC.low);
		case 0xAA: return CPU_XOR_A(RegisterDE.high);
		case 0xAB: return CPU_XOR_A(RegisterDE.low);
		case 0xAC: return CPU_XOR_A(RegisterHL.high);
		case 0xAD: return CPU_XOR_A(RegisterHL.low);
		case 0xAE: return CPU_XOR_A_HL();
		case 0xAF: return CPU_XOR_A(RegisterAF.high);
		case 0xB0: return CPU_OR_A(RegisterBC.high);
		case 0xB1: return CPU_OR_A(RegisterBC.low);
		case 0xB2: return CPU_OR_A(RegisterDE.high);
		case 0xB3: return CPU_OR_A(RegisterDE.low);
		case 0xB4: return CPU_OR_A(RegisterHL.high);
		case 0xB5: return CPU_OR_A(RegisterHL.low);
		case 0xB6: return CPU_OR_A_HL();
		case 0xB7: return CPU_OR_A(RegisterAF.high);
		case 0xB8: return CPU_CP_A(RegisterBC.high);
		case 0xB9: return CPU_CP_A(RegisterBC.low);
		case 0xBA: return CPU_CP_A(RegisterDE.high);
		case 0xBB: return CPU_CP_A(RegisterDE.low);
		case 0xBC: return CPU_CP_A(RegisterHL.high);
		case 0xBD: return CPU_CP_A(RegisterHL.low);
		case 0xBE: return CPU_CP_A_HL();
		case 0xBF: return CPU_CP_A(RegisterAF.high);
		case 0xC6: return CPU_ADD_A_IM();
		case 0xCE: return CPU_ADC_A_IM();
		case 0xD6: return CPU_SUB_A_IM();
		case 0xDE: return CPU_SBC_A_IM();
		case 0xE6: return CPU_AND_A_IM();
		case 0xEE: return CPU_XOR_A_IM();
		case 0xF6: return CPU_OR_A_IM();
		case 0xFE: return CPU_CP_A_IM();

		//16bit logic
		case 0x03: return CPU_16BIT_INC(RegisterBC.reg);
		case 0x09: return CPU_ADD_HL(RegisterBC.reg);
		case 0x0B: return CPU_16BIT_DEC(RegisterBC.reg);
		case 0x13: return CPU_16BIT_INC(RegisterDE.reg);
		case 0x19: return CPU_ADD_HL(RegisterDE.reg);
		case 0x1B: return CPU_16BIT_DEC(RegisterDE.reg);
		case 0x23: return CPU_16BIT_INC(RegisterHL.reg);
		case 0x29: return CPU_ADD_HL(RegisterHL.reg);
		case 0x2B: return CPU_16BIT_DEC(RegisterHL.reg);
		case 0x33: return CPU_16BIT_INC(SP);
		case 0x39: return CPU_ADD_HL(SP);
		case 0x3B: return CPU_16BIT_DEC(SP);
		case 0xE8: return CPU_ADD_SP_IM();

		//8bit rotation/shifts
		case 0x07: return CPU_RLC_A();
		case 0x0F: return CPU_RRC_A();
		case 0x17: return CPU_RL_A();
		case 0x1F: return CPU_RR_A();

		//8bit loads
		case 0x02:
			CPU_8BIT_LOAD_TO_MEMORY(RegisterBC.reg, RegisterAF.high);
			return 8;
		case 0x06:
			CPU_8BIT_LOAD_FROM_IM(RegisterBC.high);
			return 8;
		case 0x0A:
			CPU_8BIT_LOAD_FROM_MEMORY(RegisterAF.high, RegisterBC.reg);
			return 8;
		case 0x0E:
			CPU_8BIT_LOAD_FROM_IM(RegisterBC.low);
			return 8;
		case 0x12:
			CPU_8BIT_LOAD_TO_MEMORY(RegisterDE.reg, RegisterAF.high);
			return 8;
		case 0x16:
			CPU_8BIT_LOAD_FROM_IM(RegisterDE.high);
			return 8;
		case 0x1A:
			CPU_8BIT_LOAD_FROM_MEMORY(RegisterAF.high, RegisterDE.reg);
			return 8;
		case 0x1E:
			CPU_8BIT_LOAD_FROM_IM(RegisterDE.low);
			return 8;
		case 0x22:
			CPU_8BIT_LOAD_TO_MEMORY(RegisterHL.reg, RegisterAF.high);
			RegisterHL.reg++;
			return 8;
		case 0x26:
			CPU_8BIT_LOAD_FROM_IM(RegisterHL.high);
			return 8;
		case 0x2A:
			CPU_8BIT_LOAD_FROM_MEMORY(RegisterAF.high, RegisterHL.reg);
			RegisterHL.reg++;
			return 8;
		case 0x2E:
			CPU_8BIT_LOAD_FROM_IM(RegisterHL.low);
			return 8;
		case 0x32:
			CPU_8BIT_LOAD_TO_MEMORY(RegisterHL.reg, RegisterAF.high);
			RegisterHL.reg--;
			return 8;
		case 0x36:
			CPU_8BIT_LOAD_TO_MEMORY_FROM_IM(RegisterHL.reg);
			return 12;
		case 0x3A:
			CPU_8BIT_LOAD_FROM_MEMORY(RegisterAF.high, RegisterHL.reg);
			RegisterHL.reg--;
			return 8;
		case 0x3E:
			CPU_8BIT_LOAD_FROM_IM(RegisterAF.high);
			return 8;
		case 0x40:
			CPU_8BIT_LOAD_FROM_REG(RegisterBC.high, RegisterBC.high);
			return 4;
		case 0x41:
			CPU_8BIT_LOAD_FROM_REG(RegisterBC.high, RegisterBC.low);
			return 4;
		case 0x42:
			CPU_8BIT_LOAD_FROM_REG(RegisterBC.high, RegisterDE.high);
			return 4;
		case 0x43:
			CPU_8BIT_LOAD_FROM_REG(RegisterBC.high, RegisterDE.low);
			return 4;
		case 0x44:
			CPU_8BIT_LOAD_FROM_REG(RegisterBC.high, RegisterHL.high);
			return 4;
		case 0x45:
			CPU_8BIT_LOAD_FROM_REG(RegisterBC.high, RegisterHL.low);
			return 4;
		case 0x46:
			CPU_8BIT_LOAD_FROM_MEMORY(RegisterBC.high, RegisterHL.reg);
			return 8;
		case 0x47:
			CPU_8BIT_LOAD_FROM_REG(RegisterBC.high, RegisterAF.high);
			return 4;
		case 0x48:
			CPU_8BIT_LOAD_FROM_REG(RegisterBC.low, RegisterBC.high);
			return 4;
		case 0x49:
			CPU_8BIT_LOAD_FROM_REG(RegisterBC.low, RegisterBC.low);
			return 4;
		case 0x4A:
			CPU_8BIT_LOAD_FROM_REG(RegisterBC.low, RegisterDE.high);
			return 4;
		case 0x4B:
			CPU_8BIT_LOAD_FROM_REG(RegisterBC.low, RegisterDE.low);
			return 4;
		case 0x4C:
			CPU_8BIT_LOAD_FROM_REG(RegisterBC.low, RegisterHL.high);
			return 4;
		case 0x4D:
			CPU_8BIT_LOAD_FROM_REG(RegisterBC.low, RegisterHL.low);
			return 4;
		case 0x4E:
			CPU_8BIT_LOAD_FROM_MEMORY(RegisterBC.low, RegisterHL.reg);
			return 8;
		case 0x4F:
			CPU_8BIT_LOAD_FROM_REG(RegisterBC.low, RegisterAF.high);
			return 4;
		case 0x50:
			CPU_8BIT_LOAD_FROM_REG(RegisterDE.high, RegisterBC.high);
			return 4;
		case 0x51:
			CPU_8BIT_LOAD_FROM_REG(RegisterDE.high, RegisterBC.low);
			return 4;
		case 0x52:
			CPU_8BIT_LOAD_FROM_REG(RegisterDE.high, RegisterDE.high);
			return 4;
		case 0x53:
			CPU_8BIT_LOAD_FROM_REG(RegisterDE.high, RegisterDE.low);
			return 4;
		case 0x54:
			CPU_8BIT_LOAD_FROM_REG(RegisterDE.high, RegisterHL.high);
			return 4;
		case 0x55:
			CPU_8BIT_LOAD_FROM_REG(RegisterDE.high, RegisterHL.low);
			return 4;
		case 0x56:
			CPU_8BIT_LOAD_FROM_MEMORY(RegisterDE.high, RegisterHL.reg);
			return 8;
		case 0x57:
			CPU_8BIT_LOAD_FROM_REG(RegisterDE.high, RegisterAF.high);
			return 4;
		case 0x58:
			CPU_8BIT_LOAD_FROM_REG(RegisterDE.low, RegisterBC.high);
			return 4;
		case 0x59:
			CPU_8BIT_LOAD_FROM_REG(RegisterDE.low, RegisterBC.low);
			return 4;
		case 0x5A:
			CPU_8BIT_LOAD_FROM_REG(RegisterDE.low, RegisterDE.high);
			return 4;
		case 0x5B:
			CPU_8BIT_LOAD_FROM_REG(RegisterDE.low, RegisterDE.low);
			return 4;
		case 0x5C:
			CPU_8BIT_LOAD_FROM_REG(RegisterDE.low, RegisterHL.high);
			return 4;
		case 0x5D:
			CPU_8BIT_LOAD_FROM_REG(RegisterDE.low, RegisterHL.low);
			return 4;
		case 0x5E:
			CPU_8BIT_LOAD_FROM_MEMORY(RegisterDE.low, RegisterHL.reg);
			return 8;
		case 0x5F:
			CPU_8BIT_LOAD_FROM_REG(RegisterDE.low, RegisterAF.high);
			return 4;
		case 0x60:
			CPU_8BIT_LOAD_FROM_REG(RegisterHL.high, RegisterBC.high);
			return 4;
		case 0x61:
			CPU_8BIT_LOAD_FROM_REG(RegisterHL.high, RegisterBC.low);
			return 4;
		case 0x62:
			CPU_8BIT_LOAD_FROM_REG(RegisterHL.high, RegisterDE.high);
			return 4;
		case 0x63:
			CPU_8BIT_LOAD_FROM_REG(RegisterHL.high, RegisterDE.low);
			return 4;
		case 0x64:
			CPU_8BIT_LOAD_FROM_REG(RegisterHL.high, RegisterHL.high);
			return 4;
		case 0x65:
			CPU_8BIT_LOAD_FROM_REG(RegisterHL.high, RegisterHL.low);
			return 4;
		case 0x66:
			CPU_8BIT_LOAD_FROM_MEMORY(RegisterHL.high, RegisterHL.reg);
			return 8;
		case 0x67:
			CPU_8BIT_LOAD_FROM_REG(RegisterHL.high, RegisterAF.high);
			return 4;
		case 0x68:
			CPU_8BIT_LOAD_FROM_REG(RegisterHL.low, RegisterBC.high);
			return 4;
		case 0x69:
			CPU_8BIT_LOAD_FROM_REG(RegisterHL.low, RegisterBC.low);
			return 4;
		case 0x6A:
			CPU_8BIT_LOAD_FROM_REG(RegisterHL.low, RegisterDE.low);
			return 4;
		case 0x6B:
			CPU_8BIT_LOAD_FROM_REG(RegisterHL.low, RegisterDE.low);
			return 4;
		case 0x6C:
			CPU_8BIT_LOAD_FROM_REG(RegisterHL.low, RegisterHL.high);
			return 4;
		case 0x6D:
			CPU_8BIT_LOAD_FROM_REG(RegisterHL.low, RegisterHL.low);
			return 4;
		case 0x6E:
			CPU_8BIT_LOAD_FROM_MEMORY(RegisterHL.low, RegisterHL.reg);
			return 8;
		case 0x6F:
			CPU_8BIT_LOAD_FROM_REG(RegisterHL.low, RegisterAF.high);
			return 4;
		case 0x70:
			CPU_8BIT_LOAD_TO_MEMORY(RegisterHL.reg, RegisterBC.high);
			return 8;
		case 0x71:
			CPU_8BIT_LOAD_TO_MEMORY(RegisterHL.reg, RegisterBC.low);
			return 8;
		case 0x72:
			CPU_8BIT_LOAD_TO_MEMORY(RegisterHL.reg, RegisterDE.high);
			return 8;
		case 0x73:
			CPU_8BIT_LOAD_TO_MEMORY(RegisterHL.reg, RegisterDE.low);
			return 8;
		case 0x74:
			CPU_8BIT_LOAD_TO_MEMORY(RegisterHL.reg, RegisterHL.high);
			return 8;
		case 0x75:
			CPU_8BIT_LOAD_TO_MEMORY(RegisterHL.reg, RegisterHL.low);
			return 8;
		case 0x77:
			CPU_8BIT_LOAD_TO_MEMORY(RegisterHL.reg, RegisterAF.high);
			return 8;
		case 0x78:
			CPU_8BIT_LOAD_FROM_REG(RegisterAF.high, RegisterBC.high);
			return 4;
		case 0x79:
			CPU_8BIT_LOAD_FROM_REG(RegisterAF.high, RegisterBC.low);
			return 4;
		case 0x7A:
			CPU_8BIT_LOAD_FROM_REG(RegisterAF.high, RegisterDE.high);
			return 4;
		case 0x7B:
			CPU_8BIT_LOAD_FROM_REG(RegisterAF.high, RegisterDE.low);
			return 4;
		case 0x7C:
			CPU_8BIT_LOAD_FROM_REG(RegisterAF.high, RegisterDE.low);
			return 4;
		case 0x7D:
			CPU_8BIT_LOAD_FROM_REG(RegisterAF.high, RegisterHL.low);
			return 4;
		case 0x7E:
			CPU_8BIT_LOAD_FROM_MEMORY(RegisterAF.high, RegisterHL.reg);
			return 8;
		case 0x7F:
			CPU_8BIT_LOAD_FROM_REG(RegisterAF.high, RegisterAF.high);
			return 4;
		case 0xE0:
			CPU_8BIT_LOAD_TO_MEMORY_PLUS_OFFSET_IM_FROM_REG(RegisterAF.high);
			return 12;
		case 0xE2:
			CPU_8BIT_LOAD_TO_MEMORY_PLUS_OFFSET_FROM_REG(RegisterBC.low, RegisterAF.high);
			return 8;
		case 0xEA:
			CPU_16BIT_LOAD_TO_MEMORY_IM_FROM_REG(RegisterAF.high);
			return 16;
		case 0xF0:
			CPU_8BIT_LOAD_TO_REG_FROM_IM_PLUS_OFFSET(RegisterAF.high);
			return 12;
		case 0xF2:
			CPU_8BIT_LOAD_TO_REG_FROM_REG_PLUS_OFFSET(RegisterAF.high, RegisterBC.low);
			return 8;
		case 0xFA:
			CPU_16BIT_LOAD_TO_REG_FROM_MEMORY_IM(RegisterAF.high);
			return 16;
	}
}

void gameboy::SetZ(int n)
{
	if (n == 1)
		RegisterAF.low |= Z_MASK
	else
		RegisterAF.low &= ~Z_MASK
}

void gameboy::SetN(int n)
{
	if (n == 1)
		RegisterAF.low |= N_MASK
	else
		RegisterAF.low &= ~N_MASK
}

void gameboy::SetH(int n)
{
	if (n == 1)
		RegisterAF.low |= H_MASK
	else
		RegisterAF.low &= ~H_MASK
}

void gameboy::SetC(int n)
{
	if (n == 1)
		RegisterAF.low |= C_MASK
	else
		RegisterAF.low &= ~C_MASK
}

void gameboy::CheckHalfBorrow8Bit(uint8_t operandOne, uint8_t operandTwo)
{
	if (((operandOne & 0xf0) - (operandTwo & 0xf0) & 0x08) == 0x08) //half carry (check borrow from bit 4)
		SetH(1);
	else
		SetH(0);
}

void gameboy::CheckBorrow8Bit(uint8_t operandOne, uint8_t operandTwo)
{
	if ((operandOne - operandTwo) < 0)
		SetC(1);
	else
		SetC(0);
}

void gameboy::CheckHalfCarry8Bit(uint8_t operandOne, uint8_t operandTwo)
{
	if (((operandOne & 0x0f) + (operandTwo & 0x0f) & 0x10) == 0x10) //half carry (check carry from bit 3)
		SetH(1);
	else
		SetH(0);
}

void gameboy::CheckHalfCarry16Bit(uint16_t operandOne, uint16_t operandTwo)
{
	if (((operandOne & 0x0f00) + (operandTwo & 0x0f00) & 0x1000) == 0x0100) //half carry (check carry from bit 11)
		SetH(1);
	else
		SetH(0);
}

void gameboy::CheckCarry8Bit(uint8_t operandOne, uint8_t operandTwo)
{
	if ((operandOne + operandTwo) >= 0xFF) //carry (check if overflow)
		SetC(1);
	else
		SetC(0);
}

void gameboy::CheckCarry16Bit(uint16_t operandOne, uint16_t operandTwo)
{
	if ((operandOne + operandTwo) >= 0xFFFF) //carry (check if overflow)
		SetC(1);
	else
		SetC(0);
}

void gameboy::CheckZero(uint16_t value)
{
	if (value == 0)
		SetZ(1);
	else
		SetZ(0);
}

void gameboy::CPU_8BIT_LOAD_FROM_IM(uint8_t &destinationReg)
{
	uint8_t n = memory.ReadMemory(PC);
	PC++;
	destinationReg = n;
}

void gameboy::CPU_8BIT_LOAD_TO_MEMORY(uint16_t address, uint8_t data)
{
	memory.WriteMemory(address, data);
}

void gameboy::CPU_8BIT_LOAD_FROM_MEMORY(uint8_t &destinationReg, uint16_t address)
{
	destinationReg = memory.ReadMemory(address);
}

void gameboy::CPU_8BIT_LOAD_TO_MEMORY_FROM_IM(uint16_t address)
{
	uint8_t n = memory.ReadMemory(PC);
	PC++;
	memory.WriteMemory(address, n);
}

void gameboy::CPU_8BIT_LOAD_FROM_REG(uint8_t &destinationReg, uint8_t sourceReg)
{
	destinationReg = sourceReg;
}

void gameboy::CPU_8BIT_LOAD_TO_MEMORY_PLUS_OFFSET_IM_FROM_REG(uint8_t sourceReg)
{
	uint8_t n = memory.ReadMemory(PC);
	PC++;
	memory.WriteMemory(n + 0xFF00, sourceReg);
}

void gameboy::CPU_8BIT_LOAD_TO_MEMORY_PLUS_OFFSET_FROM_REG(uint8_t address, uint8_t sourceReg)
{
	memory.WriteMemory(address + 0xFF00, sourceReg);
}

void gameboy::CPU_16BIT_LOAD_TO_MEMORY_IM_FROM_REG(uint8_t sourceReg)
{
	uint8_t nLow = memory.ReadMemory(PC);
	PC++;
	uint8_t nHigh = memory.ReadMemory(PC);
	PC++;
	memory.WriteMemory(((uint16_t)nHigh << 8) | nLow, sourceReg);
}

void gameboy::CPU_8BIT_LOAD_TO_REG_FROM_IM_PLUS_OFFSET(uint8_t &destinationReg)
{
	uint8_t n = memory.ReadMemory(PC);
	PC++;
	destinationReg = memory.ReadMemory(n + 0xFF00);
}

void gameboy::CPU_8BIT_LOAD_TO_REG_FROM_REG_PLUS_OFFSET(uint8_t& destinationReg, uint8_t sourceReg)
{
	destinationReg = memory.ReadMemory(sourceReg + 0xFF00);
}

void gameboy::CPU_16BIT_LOAD_TO_REG_FROM_MEMORY_IM(uint8_t& destinationReg)
{
	uint8_t nLow = memory.ReadMemory(PC);
	PC++;
	uint8_t nHigh = memory.ReadMemory(PC);
	PC++;
	destinationReg = memory.ReadMemory(((uint16_t)nHigh << 8) | nLow);
}

int gameboy::CPU_JUMP_RELATIVE()
{
	int8_t n = memory.ReadMemory(PC);
	PC++;
	n = ~n + 1;
	(utility.TestBit(n, 7) == 0) ? PC += n : PC -= n;
	return 12;
}

int gameboy::CPU_JUMP_RELATIVE_F(int flag, int zeroOrOne)
{
	if (utility.TestBit(RegisterAF.low, flag) == zeroOrOne)
	{
		uint8_t n = memory.ReadMemory(PC);
		PC++;
		(utility.TestBit(n, 7) == 0) ? PC += n : PC -= (uint8_t)(~n + 1);
		return 12;
	}
	else
		PC++;
		return 8;
}

int gameboy::CPU_JUMP_IMMEDIATE(bool useHL)
{
	if (useHL)
	{
		PC = memory.ReadMemory(RegisterHL.reg);
		return 4;
	}
	else
	{
		uint8_t nLow = memory.ReadMemory(PC);
		PC++;
		uint8_t nHigh = memory.ReadMemory(PC); //doesn't matter if we increment PC or not

		PC = ((uint16_t)nHigh << 8) | nLow;
		return 16;
	}
}

int gameboy::CPU_JUMP_IMMEDIATE_F(int flag, int zeroOrOne)
{
	if (utility.TestBit(RegisterAF.low, flag) == zeroOrOne)
	{
		uint8_t nLow = memory.ReadMemory(PC);
		PC++;
		uint8_t nHigh = memory.ReadMemory(PC); //doesn't matter if we increment PC or not

		PC = (((uint16_t)nHigh << 8) | nLow);
		return 16;
	}
	else
		return 12;
}

int gameboy::CPU_RETURN()
{
	CPU_POP(PC);
	return 16;
}

int gameboy::CPU_RETURN_F(int flag, int zeroOrOne)
{
	if (utility.TestBit(RegisterAF.low, flag) == zeroOrOne)
	{
		CPU_POP(PC);
		return 20;
	}
	else
		return 8;
}

int gameboy::CPU_CALL()
{
	CPU_PUSH(PC + 2);

	CPU_JUMP_IMMEDIATE(false);
	return 24;
}

int gameboy::CPU_CALL_F(int flag, int zeroOrOne)
{
	if (utility.TestBit(RegisterAF.low, flag) == zeroOrOne)
	{
		CPU_PUSH(PC + 2);
		CPU_JUMP_IMMEDIATE(false);
		return 24;
	}
	else
		return 12;
}

int gameboy::CPU_RST(uint8_t address)
{
	CPU_PUSH(PC);
	PC = (0x0000 + address);
	return 16;
}

int gameboy::CPU_16BIT_LOAD_IM(uint16_t &destinationReg)
{
	uint8_t nLow = memory.ReadMemory(PC);
	PC++;
	uint8_t nHigh = memory.ReadMemory(PC);
	PC++;
	destinationReg = (((uint16_t)nHigh << 8) | nLow);
	return 12;
}

int gameboy::CPU_16BIT_LOAD_IM_MEM_SP()
{
	uint8_t nLow = memory.ReadMemory(PC);
	PC++;
	uint8_t nHigh = memory.ReadMemory(PC);
	PC++;
	uint16_t address = ((uint16_t)nHigh << 8) | (uint16_t)nLow;
	memory.WriteMemory(address, (uint8_t)(SP >> 8)); //write high byte, then low
	memory.WriteMemory(address++, (uint8_t)(SP));

	return 20;
}

int gameboy::CPU_16BIT_LOAD_HL_SP_IM()
{
	RegisterHL.reg = (SP + memory.ReadMemory(PC));
	CheckHalfCarry16Bit(SP, memory.ReadMemory(PC));
	CheckCarry16Bit(SP, memory.ReadMemory(PC));
	PC++;
	SetZ(0);
	SetN(0);
	return 12;
}

int gameboy::CPU_16BIT_LOAD_SP_HL()
{
	SP = RegisterHL.reg;
	return 8;
}

int gameboy::CPU_POP(uint16_t &destinationRegister)
{
	uint8_t nLow = memory.ReadMemory(SP);
	SP++;
	uint8_t nHigh = memory.ReadMemory(SP);
	SP++;
	destinationRegister = (((uint16_t)nHigh << 8) | nLow);
	return 12;
}

int gameboy::CPU_PUSH(uint16_t sourceRegister)
{
	uint8_t nLow = (uint8_t)sourceRegister; //grab lower byte
	uint8_t nHigh = (uint8_t)(sourceRegister >> 8); //grab upper byte
	SP--; //decrement SP first
	memory.WriteMemory(SP, nHigh); //write high value first, then low
	SP--;
	memory.WriteMemory(SP, nLow);
	return 16;
}

int gameboy::CPU_NOP()
{
	return 4;
}

int gameboy::CPU_STOP()
{
	//TODO: implement STOP
	return 4;
}

int gameboy::CPU_HALT()
{
	//TODO: implement HALT
	return 4;
}

int gameboy::CPU_DI()
{
	//TODO: disable interrupts (after instruction after DI is executed)
	return 4;
}

int gameboy::CPU_EI()
{
	//TODO: enable interrupts (after instruction after EI is executed)
	return 4;
}

int gameboy::CPU_8BIT_INC(uint8_t &sourceRegister)
{
	CheckHalfCarry8Bit(sourceRegister, 0x01); //check half carry before incrementing
	sourceRegister++;
	CheckZero(sourceRegister);
	SetN(0);
	return 4;
}

int gameboy::CPU_8BIT_DEC(uint8_t &sourceRegister)
{
	CheckHalfBorrow8Bit(sourceRegister, 0x01); //check borrow before decrementing
	sourceRegister--;
	CheckZero(sourceRegister);
	SetN(1);
	return 4;
}

int gameboy::CPU_8BIT_INC_HL()
{
	uint8_t n = memory.ReadMemory(RegisterHL.reg);
	CheckHalfCarry8Bit(n, 0x01); //check half carry before incrementing
	memory.WriteMemory(RegisterHL.reg, n++);
	CheckZero(n);
	SetN(0);
	return 12;
}

int gameboy::CPU_8BIT_DEC_HL()
{
	uint8_t n = memory.ReadMemory(RegisterHL.reg);
	CheckHalfBorrow8Bit(n, 0x01); //check borrow before decrementing
	memory.WriteMemory(RegisterHL.reg, n--);
	CheckZero(n);
	SetN(1);
	return 12;
}

int gameboy::CPU_DAA()
{
	if (utility.TestBit(RegisterAF.low, FLAG_N) == 0)
	{
		if (utility.TestBit(RegisterAF.low, FLAG_C) || RegisterAF.high > 0x99)
		{
			RegisterAF.high += 0x60;
			SetC(1);
		}
		if (utility.TestBit(RegisterAF.low, FLAG_H) || (RegisterAF.high & 0x0f) > 0x09)
			RegisterAF.high += 0x6;
	}
	else 
	{
		if (utility.TestBit(RegisterAF.low, FLAG_C))
			RegisterAF.high -= 0x60;
		if (utility.TestBit(RegisterAF.low, FLAG_H))
			RegisterAF.high -= 0x6;
	}
	CheckZero((uint16_t)RegisterAF.high);
	SetH(0);
	return 4;
}

int gameboy::CPU_CPL()
{
	RegisterAF.high =~ RegisterAF.high;
	SetN(1);
	SetH(1);
	return 4;
}

int gameboy::CPU_SCF()
{
	SetC(1);
	SetN(0);
	SetH(0);
	return 4;
}

int gameboy::CPU_CCF()
{
	if (utility.TestBit(RegisterAF.low, FLAG_C) == 1)
		SetC(0);
	else
		SetC(1);
	return 4;
}

int gameboy::CPU_ADD_A(uint8_t &sourceRegister)
{
	CheckHalfCarry8Bit(RegisterAF.high, sourceRegister);
	CheckCarry8Bit(RegisterAF.high, sourceRegister);
	RegisterAF.high += sourceRegister;
	CheckZero(RegisterAF.high);
	SetN(0);
	return 4;
}

int gameboy::CPU_ADD_A_HL()
{
	CheckHalfCarry8Bit(RegisterAF.high, memory.ReadMemory(RegisterHL.reg));
	CheckCarry8Bit(RegisterAF.high, memory.ReadMemory(RegisterHL.reg));
	RegisterAF.high += memory.ReadMemory(RegisterHL.reg);
	CheckZero(RegisterAF.high);
	SetN(0);
	return 8;
}

int gameboy::CPU_ADC_A(uint8_t &sourceRegister)
{
	if (utility.TestBit(RegisterAF.low, FLAG_C) == 1)
		RegisterAF.high++;
	CheckHalfCarry8Bit(RegisterAF.high, sourceRegister);
	CheckCarry8Bit(RegisterAF.high, sourceRegister);
	RegisterAF.high += sourceRegister;
	CheckZero(RegisterAF.high);
	SetN(0);
	return 4;
}

int gameboy::CPU_ADC_A_HL()
{
	if (utility.TestBit(RegisterAF.low, FLAG_C) == 1)
		RegisterAF.high++;
	CheckHalfCarry8Bit(RegisterAF.high, memory.ReadMemory(RegisterHL.reg));
	CheckCarry8Bit(RegisterAF.high, memory.ReadMemory(RegisterHL.reg));
	RegisterAF.high += memory.ReadMemory(RegisterHL.reg);
	CheckZero(RegisterAF.high);
	SetN(0);
	return 8;
}

int gameboy::CPU_SUB_A(uint8_t &sourceRegister)
{
	CheckHalfBorrow8Bit(RegisterAF.high, sourceRegister);
	CheckBorrow8Bit(RegisterAF.high, sourceRegister);
	RegisterAF.high -= sourceRegister;
	CheckZero(RegisterAF.high);
	SetN(1);
	return 4;
}

int gameboy::CPU_SUB_A_HL()
{
	CheckHalfBorrow8Bit(RegisterAF.high, memory.ReadMemory(RegisterHL.reg));
	CheckBorrow8Bit(RegisterAF.high, memory.ReadMemory(RegisterHL.reg));
	RegisterAF.high -= memory.ReadMemory(RegisterHL.reg);
	CheckZero(RegisterAF.high);
	SetN(1);
	return 8;
}

int gameboy::CPU_SBC_A(uint8_t &sourceRegister)
{
	if (utility.TestBit(RegisterAF.low, FLAG_C) == 1)
		RegisterAF.high--;
	CheckHalfBorrow8Bit(RegisterAF.high, sourceRegister);
	CheckBorrow8Bit(RegisterAF.high, sourceRegister);
	RegisterAF.high -= sourceRegister;
	CheckZero(RegisterAF.high);
	SetN(1);
	return 4;
}

int gameboy::CPU_SBC_A_HL()
{
	if (utility.TestBit(RegisterAF.low, FLAG_C) == 1)
		RegisterAF.high--;
	CheckHalfBorrow8Bit(RegisterAF.high, memory.ReadMemory(RegisterHL.reg));
	CheckBorrow8Bit(RegisterAF.high, memory.ReadMemory(RegisterHL.reg));
	RegisterAF.high -= memory.ReadMemory(RegisterHL.reg);
	CheckZero(RegisterAF.high);
	SetN(1);
	return 8;
}

int gameboy::CPU_AND_A(uint8_t &sourceRegister)
{
	RegisterAF.high &= sourceRegister;
	CheckZero(RegisterAF.high);
	SetN(0);
	SetH(1);
	SetC(0);
	return 4;
}

int gameboy::CPU_AND_A_HL()
{
	RegisterAF.high &= memory.ReadMemory(RegisterHL.reg);
	CheckZero(RegisterAF.high);
	SetN(0);
	SetH(1);
	SetC(0);
	return 8;
}

int gameboy::CPU_XOR_A(uint8_t &sourceRegister)
{
	RegisterAF.high ^= sourceRegister;
	CheckZero(RegisterAF.high);
	SetN(0);
	SetH(0);
	SetC(0);
	return 4;
}

int gameboy::CPU_XOR_A_HL()
{
	RegisterAF.high ^= memory.ReadMemory(RegisterHL.reg);
	CheckZero(RegisterAF.high);
	SetN(0);
	SetH(0);
	SetC(0);
	return 8;
}

int gameboy::CPU_OR_A(uint8_t &sourceRegister)
{
	RegisterAF.high |= sourceRegister;
	CheckZero(RegisterAF.high);
	SetN(0);
	SetH(0);
	SetC(0);
	return 4;
}

int gameboy::CPU_OR_A_HL()
{
	RegisterAF.high |= memory.ReadMemory(RegisterHL.reg);
	CheckZero(RegisterAF.high);
	SetN(0);
	SetH(0);
	SetC(0);
	return 8;
}

int gameboy::CPU_CP_A(uint8_t &sourceRegister)
{
	CheckHalfBorrow8Bit(RegisterAF.high, sourceRegister);
	CheckBorrow8Bit(RegisterAF.high, sourceRegister);
	CheckZero(RegisterAF.high - sourceRegister);
	SetN(1);
	return 4;
}

int gameboy::CPU_CP_A_HL()
{
	CheckHalfBorrow8Bit(RegisterAF.high, memory.ReadMemory(RegisterHL.reg));
	CheckBorrow8Bit(RegisterAF.high, memory.ReadMemory(RegisterHL.reg));
	CheckZero(RegisterAF.high - memory.ReadMemory(RegisterHL.reg));
	SetN(1);
	return 4;
}

int gameboy::CPU_ADD_A_IM()
{
	uint8_t nLow = memory.ReadMemory(PC);
	PC++;
	CheckHalfCarry8Bit(RegisterAF.high, nLow);
	CheckCarry8Bit(RegisterAF.high, nLow);
	RegisterAF.high += nLow;
	CheckZero(RegisterAF.high);
	SetN(0);
	return 8;
}

int gameboy::CPU_ADC_A_IM()
{
	uint8_t nLow = memory.ReadMemory(PC);
	PC++;
	if (utility.TestBit(RegisterAF.low, FLAG_C) == 1)
		RegisterAF.high++;
	CheckHalfCarry8Bit(RegisterAF.high, nLow);
	CheckCarry8Bit(RegisterAF.high, nLow);
	RegisterAF.high += nLow;
	CheckZero(RegisterAF.high);
	SetN(0);
	return 8;
}

int gameboy::CPU_SUB_A_IM()
{
	uint8_t nLow = memory.ReadMemory(PC);
	PC++;
	CheckHalfBorrow8Bit(RegisterAF.high, nLow);
	CheckBorrow8Bit(RegisterAF.high, nLow);
	RegisterAF.high -= nLow;
	CheckZero(RegisterAF.high);
	SetN(1);
	return 8;
}

int gameboy::CPU_SBC_A_IM()
{
	uint8_t nLow = memory.ReadMemory(PC);
	PC++;
	if (utility.TestBit(RegisterAF.low, FLAG_C) == 1)
		RegisterAF.high--;
	CheckHalfBorrow8Bit(RegisterAF.high, nLow);
	CheckBorrow8Bit(RegisterAF.high, nLow);
	RegisterAF.high -= nLow;
	CheckZero(RegisterAF.high);
	SetN(1);
	return 8;
}

int gameboy::CPU_AND_A_IM()
{
	uint8_t nLow = memory.ReadMemory(PC);
	PC++;
	RegisterAF.high &= nLow;
	CheckZero(RegisterAF.high);
	SetN(0);
	SetH(1);
	SetC(0);
	return 8;
}

int gameboy::CPU_XOR_A_IM()
{
	uint8_t nLow = memory.ReadMemory(PC);
	PC++;
	RegisterAF.high ^= nLow;
	CheckZero(RegisterAF.high);
	SetN(0);
	SetH(0);
	SetC(0);
	return 8;
}

int gameboy::CPU_OR_A_IM()
{
	uint8_t nLow = memory.ReadMemory(PC);
	PC++;
	RegisterAF.high |= nLow;
	CheckZero(RegisterAF.high);
	SetN(0);
	SetH(0);
	SetC(0);
	return 8;
}

int gameboy::CPU_CP_A_IM()
{
	uint8_t nLow = memory.ReadMemory(PC);
	PC++;
	CheckHalfBorrow8Bit(RegisterAF.high, nLow);
	CheckBorrow8Bit(RegisterAF.high, nLow);
	CheckZero(RegisterAF.high - nLow);
	SetN(1);
	return 8;
}

int gameboy::CPU_16BIT_INC(uint16_t &sourceRegister)
{
	sourceRegister++;
	return 8;
}

int gameboy::CPU_ADD_HL(uint16_t &sourceRegister)
{
	CheckHalfCarry16Bit(RegisterHL.reg, sourceRegister);
	CheckCarry16Bit(RegisterHL.reg, sourceRegister);
	RegisterHL.reg += sourceRegister;
	SetN(0);
	return 8;
}

int gameboy::CPU_16BIT_DEC(uint16_t &sourceRegister)
{
	sourceRegister--;
	return 8;
}

int gameboy::CPU_ADD_SP_IM()
{
	uint8_t nLow = memory.ReadMemory(PC);
	PC++;
	CheckHalfCarry16Bit(SP, nLow);
	CheckCarry16Bit(SP, nLow);
	SP += nLow;
	SetZ(0);
	SetN(0);
	return 16;
}

int gameboy::CPU_RLC_A()
{
	SetC(utility.TestBit(RegisterAF.high, 7));
	RegisterAF.high <<= 1;
	SetZ(0);
	SetN(0);
	SetH(0);
	return 4;
}

int gameboy::CPU_RRC_A()
{
	SetC(utility.TestBit(RegisterAF.high, 0));
	RegisterAF.high >>= 1;
	SetZ(0);
	SetN(0);
	SetH(0);
	return 4;
}

int gameboy::CPU_RL_A()
{
	int oldC = utility.TestBit(RegisterAF.low, FLAG_C);
	SetC(utility.TestBit(RegisterAF.high, 7));
	RegisterAF.high <<= 1;
	if (oldC == 1)
		RegisterAF.high |= 0x01;
	else
		RegisterAF.high &= 0xFE;
	SetZ(0);
	SetN(0);
	SetH(0);
	return 4;
}

int gameboy::CPU_RR_A()
{
	int oldC = utility.TestBit(RegisterAF.low, FLAG_C);
	SetC(utility.TestBit(RegisterAF.high, 0));
	RegisterAF.high >>= 1;
	if (oldC == 1)
		RegisterAF.high |= 0x80;
	else
		RegisterAF.high &= 0x7F;
	SetZ(0);
	SetN(0);
	SetH(0);
	return 4;
}

int gameboy::CPU_RLC(uint8_t &sourceRegister)
{
	SetC(utility.TestBit(sourceRegister, 7));
	sourceRegister <<= 1;
	CheckZero(sourceRegister);
	SetN(0);
	SetH(0);
	return 8;
}

int gameboy::CPU_RLC_HL()
{
	uint8_t n = memory.ReadMemory(RegisterHL.reg);
	SetC(utility.TestBit(n, 7));
	memory.WriteMemory(RegisterHL.reg, n <<= 1);
	CheckZero(n);
	SetN(0);
	SetH(0);
	return 16;
}

int gameboy::CPU_RRC(uint8_t &sourceRegister)
{
	SetC(utility.TestBit(sourceRegister, 0));
	sourceRegister >>= 1;
	CheckZero(sourceRegister);
	SetN(0);
	SetH(0);
	return 8;
}

int gameboy::CPU_RRC_HL()
{
	uint8_t n = memory.ReadMemory(RegisterHL.reg);
	SetC(utility.TestBit(n, 0));
	memory.WriteMemory(RegisterHL.reg, n >>= 1);
	CheckZero(n);
	SetN(0);
	SetH(0);
	return 16;
}

int gameboy::CPU_RL(uint8_t &sourceRegister)
{
	int oldC = utility.TestBit(RegisterAF.low, FLAG_C);
	SetC(utility.TestBit(sourceRegister, 7));
	sourceRegister <<= 1;
	if (oldC == 1)
		sourceRegister |= 0x01;
	else
		sourceRegister &= 0xFE;
	CheckZero(sourceRegister);
	SetN(0);
	SetH(0);
	return 8;
}

int gameboy::CPU_RL_HL()
{
	uint8_t n = memory.ReadMemory(RegisterHL.reg);
	int oldC = utility.TestBit(RegisterAF.low, FLAG_C);
	SetC(utility.TestBit(n, 7));
	memory.WriteMemory(RegisterHL.reg, n <<= 1);
	if (oldC == 1)
		memory.WriteMemory(RegisterHL.reg, n |= 0x01);
	else
		memory.WriteMemory(RegisterHL.reg, n &= 0xFE);
	CheckZero(n);
	SetN(0);
	SetH(0);
	return 16;
}

int gameboy::CPU_RR(uint8_t &sourceRegister)
{
	int oldC = utility.TestBit(RegisterAF.low, FLAG_C);
	SetC(utility.TestBit(sourceRegister, 0));
	sourceRegister >>= 1;
	if (oldC == 1)
		sourceRegister |= 0x80;
	else
		sourceRegister &= 0x7F;
	CheckZero(sourceRegister);
	SetN(0);
	SetH(0);
	return 8;
}

int gameboy::CPU_RR_HL()
{
	uint8_t n = memory.ReadMemory(RegisterHL.reg);
	int oldC = utility.TestBit(RegisterAF.low, FLAG_C);
	SetC(utility.TestBit(n, 0));
	memory.WriteMemory(RegisterHL.reg, n >>= 1);
	if (oldC == 1)
		memory.WriteMemory(RegisterHL.reg, n |= 0x80);
	else
		memory.WriteMemory(RegisterHL.reg, n &= 0x7F);
	CheckZero(n);
	SetN(0);
	SetH(0);
	return 16;
}

int gameboy::CPU_SLA(uint8_t &sourceRegister)
{
	SetC(utility.TestBit(sourceRegister, 7));
	sourceRegister <<= 1;
	sourceRegister &= 0xFE;
	CheckZero(sourceRegister);
	SetN(0);
	SetH(0);
	return 8;
}

int gameboy::CPU_SLA_HL()
{
	uint8_t n = memory.ReadMemory(RegisterHL.reg);
	SetC(utility.TestBit(n, 7));
	memory.WriteMemory(RegisterHL.reg, n <<= 1);
	memory.WriteMemory(RegisterHL.reg, n &= 0xFE);
	CheckZero(n);
	SetN(0);
	SetH(0);
	return 16;
}

int gameboy::CPU_SRA(uint8_t &sourceRegister)
{
	int oldMSB = utility.TestBit(sourceRegister, 7);
	sourceRegister >>= 1;
	if (oldMSB == 1)
		sourceRegister |= 0x80;
	else
		sourceRegister &= 0x7F;
	CheckZero(sourceRegister);
	SetN(0);
	SetH(0);
	SetC(0);
	return 8;
}

int gameboy::CPU_SRA_HL()
{
	uint8_t n = memory.ReadMemory(RegisterHL.reg);
	int oldMSB = utility.TestBit(n, 7);
	memory.WriteMemory(RegisterHL.reg, n >>= 1);
	if (oldMSB == 1)
		memory.WriteMemory(RegisterHL.reg, n |= 0x80);
	else
		memory.WriteMemory(RegisterHL.reg, n &= 0x7F);
	CheckZero(n);
	SetN(0);
	SetH(0);
	SetC(0);
	return 16;
}

int gameboy::CPU_SWAP(uint8_t &sourceRegister)
{
	uint8_t low = sourceRegister << 4;
	uint8_t high = sourceRegister >> 4;
	sourceRegister = low + high;
	CheckZero(sourceRegister);
	SetN(0);
	SetH(0);
	SetC(0);
	return 8;
}

int gameboy::CPU_SWAP_HL()
{
	uint8_t n = memory.ReadMemory(RegisterHL.reg);
	uint8_t low = n << 4;
	uint8_t high = n >> 4;
	n = low + high;
	memory.WriteMemory(RegisterHL.reg, n);
	CheckZero(n);
	SetN(0);
	SetH(0);
	SetC(0);
	return 16;
}

int gameboy::CPU_SRL(uint8_t &sourceRegister)
{
	SetC(utility.TestBit(sourceRegister, 0));
	sourceRegister >>= 1;
	sourceRegister &= 0x7F;
	CheckZero(sourceRegister);
	SetN(0);
	SetH(0);
	return 8;
}

int gameboy::CPU_SRL_HL()
{
	int n = memory.ReadMemory(RegisterHL.reg);
	SetC(utility.TestBit(n, 0));
	memory.WriteMemory(RegisterHL.reg, n >>= 1);
	memory.WriteMemory(RegisterHL.reg, n &= 0x7F);
	CheckZero(n);
	SetN(0);
	SetH(0);
	return 16;
}

int gameboy::CPU_BIT(int bitNumber, uint8_t sourceRegister)
{
	if (utility.TestBit(sourceRegister, bitNumber) == 1)
		SetZ(0);
	else
		SetZ(1);
	SetN(0);
	SetH(1);
	return 8;
}

int gameboy::CPU_BIT_HL(int bitNumber)
{
	int n = memory.ReadMemory(RegisterHL.reg);
	if (utility.TestBit(n, bitNumber) == 1)
		SetZ(0);
	else
		SetZ(1);
	SetN(0);
	SetH(1);
	return 16;
}

int gameboy::CPU_RES(int bitNumber, uint8_t &sourceRegister)
{
	switch (bitNumber)
	{
		case 7: (sourceRegister &= 0x7F);
		case 6: (sourceRegister &= 0xBF);
		case 5: (sourceRegister &= 0xDF);
		case 4: (sourceRegister &= 0xEF);
		case 3: (sourceRegister &= 0xF7);
		case 2: (sourceRegister &= 0xFB);
		case 1: (sourceRegister &= 0xFD);
		case 0: (sourceRegister &= 0xFE);
	}
	return 8;
}

int gameboy::CPU_RES_HL(int bitNumber)
{
	int n = memory.ReadMemory(RegisterHL.reg);
	switch (bitNumber)
	{
		case 7: (n &= 0x7F);
		case 6: (n &= 0xBF);
		case 5: (n &= 0xDF);
		case 4: (n &= 0xEF);
		case 3: (n &= 0xF7);
		case 2: (n &= 0xFB);
		case 1: (n &= 0xFD);
		case 0: (n &= 0xFE);
	}
	memory.WriteMemory(RegisterHL.reg, n);
	return 16;
}

int gameboy::CPU_SET(int bitNumber, uint8_t &sourceRegister)
{
	switch (bitNumber)
	{
		case 7: (sourceRegister |= 0x80);
		case 6: (sourceRegister |= 0x40);
		case 5: (sourceRegister |= 0x20);
		case 4: (sourceRegister |= 0x10);
		case 3: (sourceRegister |= 0x08);
		case 2: (sourceRegister |= 0x04);
		case 1: (sourceRegister |= 0x02);
		case 0: (sourceRegister |= 0x01);
	}
	return 8;
}

int gameboy::CPU_SET_HL(int bitNumber)
{
	int n = memory.ReadMemory(RegisterHL.reg);
	switch (bitNumber)
	{
		case 7: (n |= 0x80);
		case 6: (n |= 0x40);
		case 5: (n |= 0x20);
		case 4: (n |= 0x10);
		case 3: (n |= 0x08);
		case 2: (n |= 0x04);
		case 1: (n |= 0x02);
		case 0: (n |= 0x01);
	}
	memory.WriteMemory(RegisterHL.reg, n);
	return 16;
}