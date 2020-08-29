#include <cstdint>

class gameboy
{
private:
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

	#define FLAG_Z 7;
	#define FLAG_N 6;
	#define FLAG_H 5;
	#define FLAG_C 4;

	uint16_t SP;
	uint16_t PC;
	uint16_t opcode;

	//uint8_t  ram[8192];
	//uint8_t vram[8192];

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
	uint8_t mem[0x10000];
	uint8_t cmem[0x200000];

	uint8_t gfx[160 * 144];
	
	//Rom banking
	bool MBC1 = false;
	bool MBC2 = false;
	uint8_t CurrentROMBank;
	uint8_t RAMbanks[0x8000];
	uint8_t CurrentRAMBank;
public:
	void initialize();
	void loadGame(const char* filePath);
	int executeOpcode(uint8_t opcode);

	//memory
	uint8_t ReadMemory(uint16_t address) const;

	//cpu
	void CPU_8BIT_LOAD(uint8_t reg);

};