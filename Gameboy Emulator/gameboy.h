#pragma once

#include <cstdint>
#include "mmu.h"

class gameboy
{
private:
	mmu& memory;

	union Register
	{
		uint16_t reg;
		struct
		{
			uint8_t low;
			uint8_t high;
		};
	};

	Register RegisterAF;
	Register RegisterBC;
	Register RegisterDE;
	Register RegisterHL;

	/* Flag register
	7 6 5 4 3 2 1 0
	Z N H C 0 0 0 0
	Zero, Subtract, Half Carry, Carry, Unused */

	#define FLAG_Z 7
	#define FLAG_N 6
	#define FLAG_H 5
	#define FLAG_C 4
	#define Z_MASK 0x80;
	#define N_MASK 0x40;
	#define H_MASK 0x20;
	#define C_MASK 0x10;

	uint16_t SP;
	uint16_t PC;
	uint16_t opcode;

	/* 0000-3FFF 16KB ROM Bank 00 (in cartridge, fixed at bank 00)
	4000-7FFF 16KB ROM Bank 01..NN (in cartridge, switchable bank number)
	8000-9FFF 8KB Video RAM (VRAM) (switchable bank 0-1 in CGB Mode)
	A000-BFFF 8KB External RAM (in cartridge, switchable bank, if any)
	C000-CFFF 4KB Work RAM Bank 0 (WRAM)
	D000-DFFF 4KB Work RAM Bank 1 (WRAM) (switchable bank 1-7 in CGB Mode)
	E000-FDFF Same as C000-DDFF (ECHO) (typically not used)
	FE00-FE9F Sprite Attribute Table (OAM)
	FEA0-FEFF Not Usable
	FF00-FF7F I/O Ports
	FF80-FFFE High RAM (HRAM)
	FFFF Interrupt Enable Register*/
public:
	gameboy(mmu& _mmu) : memory(_mmu) {}

	void initialize();
	void loadGame(const char* filePath);
	int ExecuteNextOpcode();
	int ExecuteOpcode(uint8_t opcode);

	//misc
	void SetZ(int n);
	void SetN(int n);
	void SetH(int n);
	void SetC(int n);
	void CheckHalfBorrow8Bit(uint8_t operandOne, uint8_t operandTwo);
	void CheckBorrow8Bit(uint8_t operandOne, uint8_t operandTwo);
	void CheckHalfCarry8Bit(uint8_t operandOne, uint8_t operandTwo);
	void CheckHalfCarry16Bit(uint16_t operandOne, uint16_t operandTwo);
	void CheckCarry8Bit(uint8_t operandOne, uint8_t operandTwo);
	void CheckCarry16Bit(uint16_t operandOne, uint16_t operandTwo);
	void CheckZero(uint16_t value);

	//8bit loads
	void CPU_8BIT_LOAD_FROM_IM(uint8_t &destinationReg);
	void CPU_8BIT_LOAD_TO_MEMORY(uint16_t address, uint8_t data);
	void CPU_8BIT_LOAD_FROM_MEMORY(uint8_t &destinationReg, uint16_t address);
	void CPU_8BIT_LOAD_TO_MEMORY_FROM_IM(uint16_t address);
	void CPU_8BIT_LOAD_FROM_REG(uint8_t &destinationReg, uint8_t sourceReg);
	void CPU_8BIT_LOAD_TO_MEMORY_PLUS_OFFSET_IM_FROM_REG(uint8_t sourceReg);
	void CPU_8BIT_LOAD_TO_MEMORY_PLUS_OFFSET_FROM_REG(uint8_t address, uint8_t sourceReg);
	void CPU_16BIT_LOAD_TO_MEMORY_IM_FROM_REG(uint8_t sourceReg);
	void CPU_8BIT_LOAD_TO_REG_FROM_IM_PLUS_OFFSET(uint8_t& destinationReg);
	void CPU_8BIT_LOAD_TO_REG_FROM_REG_PLUS_OFFSET(uint8_t& destinationReg, uint8_t sourceReg);
	void CPU_16BIT_LOAD_TO_REG_FROM_MEMORY_IM(uint8_t& destinationReg);

	//jumps/return/call/rst
	int CPU_JUMP_RELATIVE();
	int CPU_JUMP_RELATIVE_F(int flag, int zeroOrOne);
	int CPU_RETURN();
	int CPU_RETURN_F(int flag, int zeroOrOne);
	int CPU_JUMP_IMMEDIATE(bool useHL);
	int CPU_JUMP_IMMEDIATE_F(int flag, int zeroOrOne);
	int CPU_CALL();
	int CPU_CALL_F(int flag, int zeroOrOne);
	int CPU_RST(uint8_t address);

	//16bit loads
	int CPU_16BIT_LOAD_IM(uint16_t &destinationReg);
	int CPU_16BIT_LOAD_IM_MEM_SP();
	int CPU_16BIT_LOAD_HL_SP_IM();
	int CPU_16BIT_LOAD_SP_HL();

	//pop/push
	int CPU_POP(uint16_t &destinationRegister);
	int CPU_PUSH(uint16_t sourceRegister);

	//control instructions
	int CPU_NOP();
	int CPU_STOP();
	int CPU_HALT();
	int CPU_DI();
	int CPU_EI();

	//8bit logic
	int CPU_8BIT_INC(uint8_t &sourceRegister);
	int CPU_8BIT_DEC(uint8_t &sourceRegister);
	int CPU_8BIT_INC_HL();
	int CPU_8BIT_DEC_HL();
	int CPU_DAA();
	int CPU_CPL();
	int CPU_SCF();
	int CPU_CCF();
	int CPU_ADD_A(uint8_t &sourceRegister);
	int CPU_ADD_A_HL();
	int CPU_ADC_A(uint8_t &sourceRegister);
	int CPU_ADC_A_HL();
	int CPU_SUB_A(uint8_t &sourceRegister);
	int CPU_SUB_A_HL();
	int CPU_SBC_A(uint8_t &sourceRegister);
	int CPU_SBC_A_HL();
	int CPU_AND_A(uint8_t &sourceRegister);
	int CPU_AND_A_HL();
	int CPU_XOR_A(uint8_t &sourceRegister);
	int CPU_XOR_A_HL();
	int CPU_OR_A(uint8_t &sourceRegister);
	int CPU_OR_A_HL();
	int CPU_CP_A(uint8_t &sourceRegister);
	int CPU_CP_A_HL();
	int CPU_ADD_A_IM();
	int CPU_ADC_A_IM();
	int CPU_SUB_A_IM();
	int CPU_SBC_A_IM();
	int CPU_AND_A_IM();
	int CPU_XOR_A_IM();
	int CPU_OR_A_IM();
	int CPU_CP_A_IM();

	//16bit logic
	int CPU_16BIT_INC(uint16_t &sourceRegister);
	int CPU_ADD_HL(uint16_t &sourceRegister);
	int CPU_16BIT_DEC(uint16_t &sourceRegister);
	int CPU_ADD_SP_IM();

	//8bit rotation/shifts
	int CPU_RLC_A();
	int CPU_RRC_A();
	int CPU_RL_A();
	int CPU_RR_A();
	int CPU_RLC(uint8_t &sourceRegister);
	int CPU_RLC_HL();
	int CPU_RRC(uint8_t &sourceRegister);
	int CPU_RRC_HL();
	int CPU_RL(uint8_t &sourceRegister);
	int CPU_RL_HL();
	int CPU_RR(uint8_t &sourceRegister);
	int CPU_RR_HL();
	int CPU_SLA(uint8_t &sourceRegister);
	int CPU_SLA_HL();
	int CPU_SRA(uint8_t &sourceRegister);
	int CPU_SRA_HL();
	int CPU_SWAP(uint8_t &sourceRegister);
	int CPU_SWAP_HL();
	int CPU_SRL(uint8_t &sourceRegister);
	int CPU_SRL_HL();
	int CPU_BIT(int bitNumber, uint8_t sourceRegister);
	int CPU_BIT_HL(int bitNumber);
	int CPU_RES(int bitNumber, uint8_t &sourceRegister);
	int CPU_RES_HL(int bitNumber);
	int CPU_SET(int bitNumber, uint8_t &sourceRegister);
	int CPU_SET_HL(int bitNumber);
};