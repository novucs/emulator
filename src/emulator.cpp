#include <winsock2.h>
#include <stdio.h>

#pragma comment(lib, "wsock32.lib")

#define STUDENT_NUMBER "16002374"
#define IP_ADDRESS_SERVER "127.0.0.1"
#define PORT_SERVER 0x1984 // We define a port that we are going to use.
#define PORT_CLIENT 0x1985 // We define a port that we are going to use.

#define WORD  unsigned short
#define DWORD unsigned long
#define BYTE  unsigned char

#define MAX_FILENAME_SIZE 500
#define MAX_BUFFER_SIZE 500

SOCKADDR_IN server_addr;
SOCKADDR_IN client_addr;

SOCKET sock;  // This is our socket, it is the handle to the IO address to read/write packets

WSADATA data;

char InputBuffer[MAX_BUFFER_SIZE];

char hex_file[MAX_BUFFER_SIZE];
char trc_file[MAX_BUFFER_SIZE];

//////////////////////////
//   Registers          //
//////////////////////////

#define FLAG_I 0x10
#define FLAG_Z 0x04
#define FLAG_N 0x02
#define FLAG_C 0x01
#define REGISTER_M 4
#define REGISTER_A 3
#define REGISTER_B 2
#define REGISTER_H 1
#define REGISTER_L 0
BYTE Registers[5];

#define REGISTER_X 0
#define REGISTER_Y 1
BYTE Index_Registers[2];

BYTE Flags;
WORD ProgramCounter;
WORD StackPointer;

////////////
// Memory //
////////////

#define MEMORY_SIZE 65536

BYTE Memory[MEMORY_SIZE];

#define TEST_ADDRESS_1 0x01FA
#define TEST_ADDRESS_2 0x01FB
#define TEST_ADDRESS_3 0x01FC
#define TEST_ADDRESS_4 0x01FD
#define TEST_ADDRESS_5 0x01FE
#define TEST_ADDRESS_6 0x01FF
#define TEST_ADDRESS_7 0x0200
#define TEST_ADDRESS_8 0x0201
#define TEST_ADDRESS_9 0x0202
#define TEST_ADDRESS_10 0x0203
#define TEST_ADDRESS_11 0x0204
#define TEST_ADDRESS_12 0x0205

///////////////////////
// Control variables //
///////////////////////

bool memory_in_range = true;
bool halt = false;

///////////////////////
// Disassembly table //
///////////////////////

char opcode_mneumonics[][14] = {
  "ILLEGAL     ",
  "DECX impl    ",
  "INCX impl    ",
  "DEY impl     ",
  "INCY impl    ",
  "CLC impl     ",
  "STC impl     ",
  "CLI impl     ",
  "STI impl     ",
  "ILLEGAL     ",
  "LDAA  #      ",
  "LDAB  #      ",
  "LX  #,L      ",
  "LX  #,L      ",
  "LDX  #       ",
  "LDY  #       ",

  "JMP abs      ",
  "JCC abs      ",
  "JCS abs      ",
  "JNE abs      ",
  "JEQ abs      ",
  "JMI abs      ",
  "JPL abs      ",
  "JHI abs      ",
  "JLE abs      ",
  "ILLEGAL     ",
  "LDAA abs     ",
  "LDAB abs     ",
  "MVI  #,L     ",
  "MVI  #,H     ",
  "LDX abs      ",
  "LDY abs      ",

  "LODS  #      ",
  "JSR abs      ",
  "CCC abs      ",
  "CCS abs      ",
  "CNE abs      ",
  "CEQ abs      ",
  "CMI abs      ",
  "CPL abs      ",
  "CHI abs      ",
  "CLE abs      ",
  "LDAA abs,X   ",
  "LDAB abs,X   ",
  "NOP impl     ",
  "HLT impl     ",
  "LDX abs,X    ",
  "LDY abs,X    ",

  "LODS abs     ",
  "ADC A,L      ",
  "SBC A,L      ",
  "ADD A,L      ",
  "SUB A,L      ",
  "CMP A,L      ",
  "OR A,L       ",
  "AND A,L      ",
  "XOR A,L      ",
  "BIT A,L      ",
  "LDAA abs,Y   ",
  "LDAB abs,Y   ",
  "ILLEGAL     ",
  "ILLEGAL     ",
  "LDX abs,Y    ",
  "LDY abs,Y    ",

  "LODS abs,X   ",
  "ADC A,H      ",
  "SBC A,H      ",
  "ADD A,H      ",
  "SUB A,H      ",
  "CMP A,H      ",
  "OR A,H       ",
  "AND A,H      ",
  "XOR A,H      ",
  "BIT A,H      ",
  "LDAA (ind)   ",
  "LDAB (ind)   ",
  "RET impl     ",
  "ILLEGAL     ",
  "LDX (ind)    ",
  "LDY (ind)    ",

  "LODS abs,Y   ",
  "ADC A,M      ",
  "SBC A,M      ",
  "ADD A,M      ",
  "SUB A,M      ",
  "CMP A,M      ",
  "OR A,M       ",
  "AND A,M      ",
  "XOR A,M      ",
  "BIT A,M      ",
  "LDAA (ind,X) ",
  "LDAB (ind,X) ",
  "SWI impl     ",
  "RTI impl     ",
  "LDX (ind,X)  ",
  "LDY (ind,X)  ",

  "LODS (ind)   ",
  "ADC B,L      ",
  "SBC B,L      ",
  "ADD B,L      ",
  "SUB B,L      ",
  "CMP B,L      ",
  "OR B,L       ",
  "AND B,L      ",
  "XOR B,L      ",
  "BIT B,L      ",
  "STOS abs     ",
  "MOVE A,A     ",
  "MOVE B,A     ",
  "MOVE L,A     ",
  "MOVE H,A     ",
  "MOVE M,A     ",

  "LODS (ind,X) ",
  "ADC B,H      ",
  "SBC B,H      ",
  "ADD B,H      ",
  "SUB B,H      ",
  "CMP B,H      ",
  "OR B,H       ",
  "AND B,H      ",
  "XOR B,H      ",
  "BIT B,H      ",
  "STOS abs,X   ",
  "MOVE A,B     ",
  "MOVE B,B     ",
  "MOVE L,B     ",
  "MOVE H,B     ",
  "MOVE M,B     ",

  "ILLEGAL     ",
  "ADC B,M      ",
  "SBC B,M      ",
  "ADD B,M      ",
  "SUB B,M      ",
  "CMP B,M      ",
  "OR B,M       ",
  "AND B,M      ",
  "XOR B,M      ",
  "BIT B,M      ",
  "STOS abs,Y   ",
  "MOVE A,L     ",
  "MOVE B,L     ",
  "MOVE L,L     ",
  "MOVE H,L     ",
  "MOVE M,L     ",

  "ILLEGAL     ",
  "ILLEGAL     ",
  "ILLEGAL     ",
  "SBIA  #      ",
  "SBIB  #      ",
  "CPIA  #      ",
  "CPIB  #      ",
  "ORIA  #      ",
  "ORIB  #      ",
  "ILLEGAL     ",
  "STOS (ind)   ",
  "MOVE A,H     ",
  "MOVE B,H     ",
  "MOVE L,H     ",
  "MOVE H,H     ",
  "MOVE M,H     ",

  "INC abs      ",
  "DEC abs      ",
  "RRC abs      ",
  "RLC abs      ",
  "SAL abs      ",
  "SAR abs      ",
  "LSR abs      ",
  "COM abs      ",
  "ROL abs      ",
  "RR abs       ",
  "STOS (ind,X) ",
  "MOVE A,M     ",
  "MOVE B,M     ",
  "MOVE L,M     ",
  "MOVE H,M     ",
  "MOVE -,-     ",

  "INC abs,X    ",
  "DEC abs,X    ",
  "RRC abs,X    ",
  "RLC abs,X    ",
  "SAL abs,X    ",
  "SAR abs,X    ",
  "LSR abs,X    ",
  "COM abs,X    ",
  "ROL abs,X    ",
  "RR abs,X     ",
  "STORA abs    ",
  "STORB abs    ",
  "STOX abs     ",
  "STOY abs     ",
  "PUSH  ,A     ",
  "POP A,       ",

  "INC abs,Y    ",
  "DEC abs,Y    ",
  "RRC abs,Y    ",
  "RLC abs,Y    ",
  "SAL abs,Y    ",
  "SAR abs,Y    ",
  "LSR abs,Y    ",
  "COM abs,Y    ",
  "ROL abs,Y    ",
  "RR abs,Y     ",
  "STORA abs,X  ",
  "STORB abs,X  ",
  "STOX abs,X   ",
  "STOY abs,X   ",
  "PUSH  ,B     ",
  "POP B,       ",

  "INCA A,A     ",
  "DECA A,A     ",
  "RRCA A,A     ",
  "RLCA A,A     ",
  "SALA A,A     ",
  "SARA A,A     ",
  "LSRA A,A     ",
  "COMA A,A     ",
  "ROLA A,A     ",
  "RRA A,A      ",
  "STORA abs,Y  ",
  "STORB abs,Y  ",
  "STOX abs,Y   ",
  "STOY abs,Y   ",
  "PUSH  ,s     ",
  "POP s,       ",

  "INCB B,B     ",
  "DECB B,B     ",
  "RRCB B,B     ",
  "RLCB B,B     ",
  "SALB B,B     ",
  "SARB B,B     ",
  "LSRB B,B     ",
  "COMB B,B     ",
  "ROLB B,B     ",
  "RRB B,B      ",
  "STORA (ind)  ",
  "STORB (ind)  ",
  "STOX (ind)   ",
  "STOY (ind)   ",
  "PUSH  ,L     ",
  "POP L,       ",

  "CAY impl     ",
  "MYA impl     ",
  "CSA impl     ",
  "ABA impl     ",
  "SBA impl     ",
  "AAB impl     ",
  "SAB impl     ",
  "ADCP A,L     ",
  "SBCP A,L     ",
  "XCHG A,L     ",
  "STORA (ind,X)",
  "STORB (ind,X)",
  "STOX (ind,X) ",
  "STOY (ind,X) ",
  "PUSH  ,H     ",
  "POP H,       ",
};

////////////////////////////////////////////////////////////////////////////////
//                           Simulator/Emulator (Start)                       //
////////////////////////////////////////////////////////////////////////////////

BYTE fetch() {
  BYTE byte = 0;

  if (ProgramCounter >= 0 && ProgramCounter <= MEMORY_SIZE) {
    memory_in_range = true;
    byte = Memory[ProgramCounter];
    ProgramCounter++;
  } else {
    memory_in_range = false;
  }

  return byte;
}

// carry flag c
// zerp flag z
// interrupt flag i
// negative flag n

/**
 * Updates the zero flag depending on the contents of a register.
 *
 * @param data the register value.
 */
void set_flag_z(BYTE data) {
  if (data == 0) {
    // Set the zero flag.
    Flags = Flags | FLAG_Z;
  } else {
    // Clear the zero flag.
    Flags = Flags & (0xFF - FLAG_Z);
  }
}

/**
 * Updates the negative flag depending on the contents of a register.
 *
 * @param data the register value.
 */
void set_flag_n(BYTE data) {
  if ((data & 0x80) == 0x80) {
    // Set the negative flag.
    Flags = Flags | FLAG_N;
  } else {
    // Clear the negative flag.
    Flags = Flags & (0xFF - FLAG_N);
  }
}

/**
 * Updates the carry flag depending on the contents of a register.
 *
 * @param data the register value.
 */
void set_flag_c(WORD data) {
  if (data >= 0x100) {
    // Set the carry flag.
    Flags = Flags | FLAG_C;
  } else {
    // Clear the carry flag.
    Flags = Flags & (0xFF - FLAG_C);
  }
}

/**
 * Sets flags ZNC for the last calculation.
 *
 * @param inReg the number last calculated.
 */
void set_flags_znc(WORD inReg) {
  set_flag_c(inReg);
  set_flag_n((BYTE) inReg);
  set_flag_z((BYTE) inReg);
}

/**
 * Forms a memory address by joining two bytes.
 *
 * @param higher the higher byte.
 * @param lower  the lower byte.
 */
WORD join_address(BYTE higher, BYTE lower) {
  return (higher << 8) + lower;
}

/**
 * Fetch an address using absolute addressing.
 */
WORD fetch_address_abs() {
  BYTE higher = fetch();
  BYTE lower = fetch();
  return join_address(higher, lower);
}

/**
 * Fetch an address using indexed absolute addressing (X).
 */
WORD fetch_address_abs_x() {
  return fetch_address_abs() + Index_Registers[REGISTER_X];
}

/**
 * Fetch an address using indexed absolute addressing (Y).
 */
WORD fetch_address_abs_y() {
  return fetch_address_abs() + Index_Registers[REGISTER_Y];
}

/**
 * Fetch an address using indirect addressing.
 */
WORD fetch_address_indir() {
  WORD address = fetch_address_abs();
  return join_address(Memory[address], Memory[address + 1]);
}

/**
 * Fetch an address using indexed indirect addressing (X).
 */
WORD fetch_address_indir_x() {
  return fetch_address_indir() + Index_Registers[REGISTER_X];
}

WORD fetch_address(int id) {
  WORD address;

  switch (id) {
    case 1:
      address = fetch_address_abs();
      break;
    case 2:
      address = fetch_address_abs_x();
      break;
    case 3:
      address = fetch_address_abs_y();
      break;
    case 4:
      address = fetch_address_indir();
      break;
    case 5:
      address = fetch_address_indir_x();
      break;
  }

  return address;
}

/**
 * Adds register to accumulator with carry.
 *
 * @param accumulator the accumulator to add to.
 * @param reg         the register to add.
 */
void adc(int accumulator, int reg) {
  WORD result = Registers[accumulator] + Registers[reg];
  if ((Flags & FLAG_C) != 0) {
    result++;
  }

  Registers[accumulator] = result;
  set_flags_znc(result);
}

/**
 * Subtracts register to accumulator with carry.
 *
 * @param accumulator the accumulator to subtract from.
 * @param reg         the register to subtract.
 */
void sbc(int accumulator, int reg) {
  WORD result = Registers[accumulator] - Registers[reg];

  if (Flags & FLAG_C != 0)
    result--;

  Registers[accumulator] = result;
  set_flags_znc(result);
}

/**
 * Adds register to accumulator.
 *
 * @param accumulator the accumulator to add to.
 * @param reg         the register to add.
 */
void add(int accumulator, int reg) {
  WORD result = Registers[accumulator] + Registers[reg];
  Registers[accumulator] = result;
  set_flags_znc(result);
}

/**
 * Subtracts register from accumulator.
 *
 * @param accumulator the accumulator to subtract from.
 * @param reg         the register to subtract.
 */
void sub(int accumulator, int reg) {
  WORD result = Registers[accumulator] - Registers[reg];
  Registers[accumulator] = result;
  set_flags_znc(result);
}

/**
 * Register compared to accumulator.
 *
 * @param accumulator the accumulator to compare.
 * @param reg         the register to compare.
 */
void cmp(int accumulator, int reg) {
  WORD result = Registers[accumulator] - Registers[reg];
  set_flags_znc(result);
}

/**
 * Register bitwise inclusive or with accumulator.
 *
 * @param accumulator the accumulator.
 * @param reg         the register.
 */
void _or(int accumulator, int reg) {
  Registers[accumulator] |= Registers[reg];
  set_flag_z(Registers[accumulator]);
  set_flag_n(Registers[accumulator]);
}

/**
 * Register bitwise and with accumulator.
 *
 * @param accumulator the accumulator.
 * @param reg         the register.
 */
void _and(int accumulator, int reg) {
  Registers[accumulator] &= Registers[reg];
  set_flag_z(Registers[accumulator]);
  set_flag_n(Registers[accumulator]);
}

/**
 * Register bitwise exclusive or with accumulator.
 *
 * @param accumulator the accumulator.
 * @param reg         the register.
 */
void _xor(int accumulator, int reg) {
  Registers[accumulator] ^= Registers[reg];
  set_flag_z(Registers[accumulator]);
  set_flag_n(Registers[accumulator]);
}

/**
 * Register bit tested with with accumulator.
 *
 * @param accumulator the accumulator.
 * @param reg         the register.
 */
void _bit(int accumulator, int reg) {
  BYTE result = Registers[accumulator] & Registers[reg];
  set_flag_z(result);
  set_flag_n(result);
}

/**
 * Subtracts data from accumulator with carry.
 *
 * @param accumulator the accumulator to subtract from.
 */
void sbi_accumulator(int accumulator) {
  WORD result = fetch() - Registers[accumulator];

  if (Flags & FLAG_C != 0)
    result--;

  set_flags_znc(result);
  Registers[accumulator] = result;
}

/**
 * Data bitwise inclusive or with accumulator.
 *
 * @param accumulator the accumulator to use.
 */
void ori_accumulator(int accumulator) {
  BYTE result = Registers[accumulator] |= fetch();
  set_flag_n(result);
  set_flag_z(result);
}

/**
 * Increment memory.
 *
 * @param address the memory address.
 */
void inc_memory(WORD address) {
  BYTE result = Memory[address]++;
  set_flag_n(result);
  set_flag_z(result);
}

/**
 * Decrement memory.
 *
 * @param address the memory address.
 */
void dec_memory(WORD address) {
  BYTE result = Memory[address]--;
  set_flag_n(result);
  set_flag_z(result);
}

/**
 * Rotates memory right through carry.
 *
 * @param address the memory address.
 */
void rrc_memory(WORD address) {
  // Rotate memory to the right.
  BYTE result = Memory[address] >> 1;

	if (Flags & FLAG_C != 0)
    result += 128;

  // Update the carry flag.
	if (Memory[address] % 2 == 0)
		Flags = Flags & (0xFF - FLAG_C);
	else
		Flags = Flags | FLAG_C;

  // Update memory and set negative and zero flags.
  Memory[address] = result;
  set_flag_n(result);
  set_flag_z(result);
}

/**
 * Rotates accumulator right through carry.
 *
 * @param accumulator the accumulator to use.
 */
void rrc_accumulator(int accumulator) {
  // Rotate accumulator to the right.
  BYTE result = Registers[accumulator] >> 1;

	if (Flags & FLAG_C != 0)
    result += 128;

  // Update the carry flag.
	if (Registers[accumulator] % 2 == 0)
		Flags = Flags & (0xFF - FLAG_C);
	else
		Flags = Flags | FLAG_C;

  // Update accumulator and set negative and zero flags.
  Registers[accumulator] = result;
  set_flag_n(result);
  set_flag_z(result);
}

/**
 * Rotates memory left through carry.
 *
 * @param address the memory address.
 */
void rlc_memory(WORD address) {
  // Rotate memory to the left.
  BYTE result = Memory[address] << 1;

	if (Flags & FLAG_C != 0)
    result += 1;

  // Update the carry flag.
	if (Memory[address] < 128)
		Flags = Flags & (0xFF - FLAG_C);
  else
		Flags = Flags | FLAG_C;

  // Update accumulator and set negative and zero flags.
  Memory[address] = result;
  set_flag_n(result);
  set_flag_z(result);
}

/**
 * Rotates accumulator right through carry.
 *
 * @param accumulator the accumulator to use.
 */
void rlc_accumulator(int accumulator) {
  // Rotate accumulator to the left.
  BYTE result = Registers[accumulator] << 1;

	if (Flags & FLAG_C != 0)
    result += 1;

  // Update the carry flag.
	if (Registers[accumulator] < 128)
		Flags = Flags & (0xFF - FLAG_C);
	else
		Flags = Flags | FLAG_C;

  // Update accumulator and set negative and zero flags.
  Registers[accumulator] = result;
  set_flag_n(result);
  set_flag_z(result);
}

void sal_memory(int id) {
  // Get the address.
  WORD address = fetch_address(id);

  // Do nothing if address is out of range.
	if (address < 0 || address >= MEMORY_SIZE) {
    return;
  }

  // Set the carry flag.
	if (Memory[address] < 128) {
		Flags = Flags & (0xFF - FLAG_C);
	} else {
		Flags = Flags | FLAG_C;
	}

  // Update memory and set negative and zero flags.
  Memory[address] <<= 1;
  set_flag_n(Memory[address]);
  set_flag_z(Memory[address]);
}

/**
 * Arithmetic shift left accumulator.
 *
 * @param accumulator the accumulator.
 */
void sal_accumulator(int accumulator) {
  WORD answer = Registers[accumulator] << 1;
  Registers[accumulator] = (BYTE) answer;
  set_flags_znc(answer);
}

void sar_memory(int id) {
  WORD address = fetch_address(id);
  if (address < 0 || address >= MEMORY_SIZE) {
    return;
  }

  if (Memory[address] % 2 == 0) {
    Flags = Flags & (0xFF - FLAG_C);
  } else {
    Flags = Flags | FLAG_C;
  }

  BYTE data = Memory[address] >> 1;

  if (Memory[address] > 127) {
    data += 128;
  }

  Memory[address] = data;
  set_flag_n(Memory[address]);
  set_flag_z(Memory[address]);
}

void sar_accumulator(int accumulator) {
  if (Registers[accumulator] % 2 == 0) {
    Flags = Flags & (0xFF - FLAG_C);
  } else {
    Flags = Flags | FLAG_C;
  }

  BYTE data = Registers[accumulator] >> 1;

  if (Registers[accumulator] > 127) {
    data += 128;
  }

  Registers[accumulator] = data;
  set_flag_n(Registers[accumulator]);
  set_flag_z(Registers[accumulator]);
}

void lsr_memory(int id) {
  WORD address = fetch_address(id);
  if (address < 0 || address >= MEMORY_SIZE) {
    return;
  }

  if (Memory[address] % 2 == 0) {
    Flags = Flags & (0xFF - FLAG_C);
  } else {
    Flags = Flags | FLAG_C;
  }

  Memory[address] >>= 1;
  set_flag_n(Memory[address]);
  set_flag_z(Memory[address]);
}

void lsr_accumulator(int accumulator) {
	if (Registers[accumulator] % 2 == 0) {
		Flags = Flags & (0xFF - FLAG_C);
	} else {
		Flags = Flags | FLAG_C;
	}

  Registers[accumulator] >>= 1;
  set_flag_n(Registers[accumulator]);
  set_flag_z(Registers[accumulator]);
}

void com_memory(int id) {
	WORD address = fetch_address(id);
	if (address < 0 && address >= MEMORY_SIZE) {
		return;
	}

	WORD data = ~Memory[address];
	Memory[address] = (BYTE) data;
	set_flags_znc(data);
}

void com_accumulator(int accumulator) {
	WORD data = ~Registers[accumulator];
  Registers[accumulator] = (BYTE) data;
  set_flags_znc(data);
}

void rol_memory(int id) {
  WORD address = fetch_address(id);
  if (address < 0 || address >= MEMORY_SIZE) {
    return;
  }

  BYTE data = Memory[address] << 1;

  if (Memory[address] >= 128) {
    data++;
  }

  Memory[address] = data;
  set_flag_n(Memory[address]);
  set_flag_z(Memory[address]);
}

void rol_accumulator(int accumulator) {
  BYTE data = Registers[accumulator] << 1;

  if (Registers[accumulator] >= 128) {
    data++;
	}

	Registers[accumulator] = data;
  set_flag_n(Registers[accumulator]);
  set_flag_z(Registers[accumulator]);
}

void rr_memory(int id) {
  WORD address = fetch_address(id);
  if (address < 0 || address >= MEMORY_SIZE) {
    return;
  }

  BYTE data = Memory[address] >> 1;

  if (Memory[address] % 2 != 0) {
    data += 128;
  }

  Memory[address] = data;
  set_flag_n(Memory[address]);
  set_flag_z(Memory[address]);
}

void rr_accumulator(int accumulator) {
  BYTE data = Registers[accumulator] >> 1;

  if (Registers[accumulator] % 2 != 0) {
    data += 128;
  }

  Registers[accumulator] = data;
  set_flag_n(Registers[accumulator]);
  set_flag_z(Registers[accumulator]);
}

/**
 * Loads register from memory.
 *
 * @param id  method for retrieving address.
 * @param reg the register to use.
 */
void load_index_register(int id, int reg) {
  // ID 0 does not require an address.
  if (id == 0) {
    Index_Registers[reg] = fetch();
    return;
  }

  // Fetch the address.
  WORD address = fetch_address(id);

  // Load memory into accumulator.
  if (address >= 0 && address < MEMORY_SIZE) {
    Index_Registers[reg] = Memory[address];
  }
}

/**
 * Saves register into memory.
 *
 * @param id  method for retrieving address.
 * @param reg the register to use.
 */
void save_index_register(int id, int reg) {
  // Fetch the address.
  WORD address = fetch_address(id);

  // Store accumulator into memory if address is valid.
  if (address >= 0 && address < MEMORY_SIZE) {
    Memory[address] = Index_Registers[reg];
  }
}

/**
 * Loads stackpointer from memory.
 *
 * @param id method for retrieving address.
 */
void load_stackpointer(int id) {
  // Convert ID to LDAA/LDAB format for reusing the address fetching function.
  id -= 2;

  // ID 0 does not require an address.
  if (id == 0) {
    StackPointer = (fetch() << 8) + fetch();
    return;
  }

  // Fetch the address.
  WORD address = fetch_address(id);

  // Load stackpointer from memory if address is valid.
  if (address >= 0 && address < MEMORY_SIZE - 1) {
    StackPointer = ((WORD) Memory[address] << 8) + Memory[address + 1];
  }
}

/**
 * Saves stackpointer into memory.
 *
 * @param id method for retrieving address.
 */
void save_stackpointer(int id) {
  // Convert ID to LDAA/LDAB format for reusing the address fetching function.
  id -= 5;

  // Fetch the address.
  WORD address = fetch_address(id);

  // Store stackpointer into memory if address is valid.
  if (address >= 0 && address < MEMORY_SIZE - 1) {
    Memory[address] = (WORD) (StackPointer >> 8);
    Memory[address + 1] = (WORD) StackPointer;
  }
}

void xchg(int reg1, int reg2) {
  BYTE data = Registers[reg1];
  Registers[reg1] = Registers[reg2];
  Registers[reg2] = data;
}

void jsr() {
  WORD address = fetch_address(1);
  Memory[StackPointer--] = ProgramCounter >> 8;
  Memory[StackPointer--] = ProgramCounter;
  ProgramCounter = address;
}

void ret() {
  BYTE higher = Memory[StackPointer++];
  BYTE lower = Memory[StackPointer++];
  ProgramCounter = join_address(higher, lower);
}

void jump(bool condition) {
  WORD address = fetch_address(1);
  if (condition) {
    ProgramCounter = address;
  }
}

void jump_subroutine(bool condition) {
  WORD address = fetch_address(1);
  if (condition) {
    if (StackPointer >= 2 && StackPointer < MEMORY_SIZE) {
      Memory[StackPointer--] = ProgramCounter >> 8;
      Memory[StackPointer--] = ProgramCounter;
    }
    ProgramCounter = address;
  }
}

void Group_1(BYTE opcode) {
  int id = opcode >> 4;

  switch (opcode) {
    /* LDAA - Loads memory into accumulator A. */
    case 0x0A: // Immediate addressing
      Registers[REGISTER_A] = fetch();
      break;
    case 0x1A: // Absolute addressing
      Registers[REGISTER_A] = Memory[fetch_address_abs()];
      break;
    case 0x2A: // Indexed absolute addressing (X)
      Registers[REGISTER_A] = Memory[fetch_address_abs_x()];
      break;
    case 0x3A: // Indexed absolute addressing (Y)
      Registers[REGISTER_A] = Memory[fetch_address_abs_y()];
      break;
    case 0x4A: // Indirect addressing
      Registers[REGISTER_A] = Memory[fetch_address_indir()];
      break;
    case 0x5A: // Indexed indirect addressing (X)
      Registers[REGISTER_A] = Memory[fetch_address_indir_x()];
      break;

    /* LDAB - Loads memory into accumulator B. */
    case 0x0B: // Immediate addressing
      Registers[REGISTER_B] = fetch();
      break;
    case 0x1B: // Absolute addressing
      Registers[REGISTER_B] = Memory[fetch_address_abs()];
      break;
    case 0x2B: // Indexed absolute addressing (X)
      Registers[REGISTER_B] = Memory[fetch_address_abs_x()];
      break;
    case 0x3B: // Indexed absolute addressing (Y)
      Registers[REGISTER_B] = Memory[fetch_address_abs_y()];
      break;
    case 0x4B: // Indirect addressing
      Registers[REGISTER_B] = Memory[fetch_address_indir()];
      break;
    case 0x5B: // Indexed indirect addressing (X)
      Registers[REGISTER_B] = Memory[fetch_address_indir_x()];
      break;

    /* STORA - Stores accumulator A into memory. */
    case 0xBA: // Absolute addressing
      Memory[fetch_address_abs()] = Registers[REGISTER_A];
      break;
    case 0xCA: // Indexed absolute addressing (X)
      Memory[fetch_address_abs_x()] = Registers[REGISTER_A];
      break;
    case 0xDA: // Indexed absolute addressing (Y)
      Memory[fetch_address_abs_y()] = Registers[REGISTER_A];
      break;
    case 0xEA: // Indirect addressing
      Memory[fetch_address_indir()] = Registers[REGISTER_A];
      break;
    case 0xFA: // Indexed indirect addressing (X)
      Memory[fetch_address_indir_x()] = Registers[REGISTER_A];
      break;

    /* STORB - Stores accumulator B into memory. */
    case 0xBB: // Absolute addressing
      Memory[fetch_address_abs()] = Registers[REGISTER_B];
      break;
    case 0xCB: // Indexed absolute addressing (X)
      Memory[fetch_address_abs_x()] = Registers[REGISTER_B];
      break;
    case 0xDB: // Indexed absolute addressing (Y)
      Memory[fetch_address_abs_y()] = Registers[REGISTER_B];
      break;
    case 0xEB: // Indirect addressing
      Memory[fetch_address_indir()] = Registers[REGISTER_B];
      break;
    case 0xFB: // Indexed indirect addressing (X)
      Memory[fetch_address_indir_x()] = Registers[REGISTER_B];
      break;

    /* ADC - Adds register to accumulator with carry. */
    case 0x31: // A + L
      adc(REGISTER_A, REGISTER_L);
      break;
    case 0x41: // A + H
      adc(REGISTER_A, REGISTER_H);
      break;
    case 0x51: // A + M
      adc(REGISTER_A, REGISTER_M);
      break;
    case 0x61: // B + L
      adc(REGISTER_B, REGISTER_L);
      break;
    case 0x71: // B + H
      adc(REGISTER_B, REGISTER_H);
      break;
    case 0x81: // B + M
      adc(REGISTER_B, REGISTER_M);
      break;

    /* SBC - Subtracts register from accumulator with carry. */
    case 0x32: // A - L
      sbc(REGISTER_A, REGISTER_L);
      break;
    case 0x42: // A - H
      sbc(REGISTER_A, REGISTER_H);
      break;
    case 0x52: // A - M
      sbc(REGISTER_A, REGISTER_M);
      break;
    case 0x62: // B - L
      sbc(REGISTER_B, REGISTER_L);
      break;
    case 0x72: // B - H
      sbc(REGISTER_B, REGISTER_H);
      break;
    case 0x82: // B - M
      sbc(REGISTER_B, REGISTER_M);
      break;

    /* ADD - Adds register to accumulator. */
    case 0x33: // A + L
      add(REGISTER_A, REGISTER_L);
      break;
    case 0x43: // A + H
      add(REGISTER_A, REGISTER_H);
      break;
    case 0x53: // A + M
      add(REGISTER_A, REGISTER_M);
      break;
    case 0x63: // B + L
      add(REGISTER_B, REGISTER_L);
      break;
    case 0x73: // B + H
      add(REGISTER_B, REGISTER_H);
      break;
    case 0x83: // B + M
      add(REGISTER_B, REGISTER_M);
      break;

    /* SUB - Subtracts register from accumulator. */
    case 0x34: // A - L
      sub(REGISTER_A, REGISTER_L);
      break;
    case 0x44: // A - H
      sub(REGISTER_A, REGISTER_H);
      break;
    case 0x54: // A - M
      sub(REGISTER_A, REGISTER_M);
      break;
    case 0x64: // B - L
      sub(REGISTER_B, REGISTER_L);
      break;
    case 0x74: // B - H
      sub(REGISTER_B, REGISTER_H);
      break;
    case 0x84: // B - M
      sub(REGISTER_B, REGISTER_M);
      break;

    /* CMP - Compares register to accumulator. */
    case 0x35: // A - L
      cmp(REGISTER_A, REGISTER_L);
      break;
    case 0x45: // A - H
      cmp(REGISTER_A, REGISTER_H);
      break;
    case 0x55: // A - M
      cmp(REGISTER_A, REGISTER_M);
      break;
    case 0x65: // B - L
      cmp(REGISTER_B, REGISTER_L);
      break;
    case 0x75: // B - H
      cmp(REGISTER_B, REGISTER_H);
      break;
    case 0x85: // B - M
      cmp(REGISTER_B, REGISTER_M);
      break;

    /* OR - Register bitwise inclusive or with accumulator. */
    case 0x36: // A | L
      _or(REGISTER_A, REGISTER_L);
      break;
    case 0x46: // A | H
      _or(REGISTER_A, REGISTER_H);
      break;
    case 0x56: // A | M
      _or(REGISTER_A, REGISTER_M);
      break;
    case 0x66: // B | L
      _or(REGISTER_B, REGISTER_L);
      break;
    case 0x76: // B | H
      _or(REGISTER_B, REGISTER_H);
      break;
    case 0x86: // B | M
      _or(REGISTER_B, REGISTER_M);
      break;

    /* AND - Register bitwise and with accumulator */
    case 0x37: // A & L
      _and(REGISTER_A, REGISTER_L);
      break;
    case 0x47: // A & H
      _and(REGISTER_A, REGISTER_H);
      break;
    case 0x57: // A & M
      _and(REGISTER_A, REGISTER_M);
      break;
    case 0x67: // B & L
      _and(REGISTER_B, REGISTER_L);
      break;
    case 0x77: // B & H
      _and(REGISTER_B, REGISTER_H);
      break;
    case 0x87: // B & M
      _and(REGISTER_B, REGISTER_M);
      break;

    /* XOR - Register bitwise exclusive or with accumulator. */
    case 0x38: // A ^ L
      _xor(REGISTER_A, REGISTER_L);
      break;
    case 0x48: // A ^ H
      _xor(REGISTER_A, REGISTER_H);
      break;
    case 0x58: // A ^ M
      _xor(REGISTER_A, REGISTER_M);
      break;
    case 0x68: // B ^ L
      _xor(REGISTER_B, REGISTER_L);
      break;
    case 0x78: // B ^ H
      _xor(REGISTER_B, REGISTER_H);
      break;
    case 0x88: // B ^ M
      _xor(REGISTER_B, REGISTER_M);
      break;

    /* BIT - Register bit tested with accumulator. */
    case 0x39: // A & L
      _bit(REGISTER_A, REGISTER_L);
      break;
    case 0x49: // A & H
      _bit(REGISTER_A, REGISTER_H);
      break;
    case 0x59: // A & M
      _bit(REGISTER_A, REGISTER_M);
      break;
    case 0x69: // B & L
      _bit(REGISTER_B, REGISTER_L);
      break;
    case 0x79: // B & H
      _bit(REGISTER_B, REGISTER_H);
      break;
    case 0x89: // B & M
      _bit(REGISTER_B, REGISTER_M);
      break;

    /* SBIA - Data subtracted from accumulator A with carry. */
    case 0x93:
      sbi_accumulator(REGISTER_A);
      break;

    /* SBIB - Data subtracted from accumulator B with carry. */
    case 0x94:
      sbi_accumulator(REGISTER_B);
      break;

    /* CPIA - Data compared to accumulator A. */
    case 0x95:
    	set_flags_znc(fetch() - Registers[REGISTER_A]);
      break;

    /* CPIB - Data compared to accumulator B. */
    case 0x96:
    	set_flags_znc(fetch() - Registers[REGISTER_B]);
      break;

    /* ORIA - Data bitwise inclusive or with accumulator A. */
    case 0x97:
      ori_accumulator(REGISTER_A);
      break;

    /* ORIB - Data bitwise inclusive or with accumulator B. */
    case 0x98:
      ori_accumulator(REGISTER_B);
      break;

    /* INC - Increment memory. */
    case 0xA0: // Absolute addressing
      inc_memory(fetch_address_abs());
      break;
    case 0xB0: // Indexed absolute addressing (X)
      inc_memory(fetch_address_abs_x());
      break;
    case 0xC0: // Indexed absolute addressing (Y)
      inc_memory(fetch_address_abs_y());
      break;

    /* INCA - Increment accumulator A. */
    case 0xD0:
      Registers[REGISTER_A]++;
      set_flag_z(Registers[REGISTER_A]);
      set_flag_n(Registers[REGISTER_A]);
      break;

    /* INCB - Increment accumulator B. */
    case 0xE0:
      Registers[REGISTER_B]++;
      set_flag_z(Registers[REGISTER_B]);
      set_flag_n(Registers[REGISTER_B]);
      break;

    /* DEC - Decrement memory. */
    case 0xA1: // Absolute addressing
      dec_memory(fetch_address_abs());
      break;
    case 0xB1: // Indexed absolute addressing (X)
      dec_memory(fetch_address_abs_x());
      break;
    case 0xC1: // Indexed absolute addressing (Y)
      dec_memory(fetch_address_abs_y());
      break;

    /* DECA - Decrement accumulator A. */
    case 0xD1:
      Registers[REGISTER_A]--;
      set_flag_z(Registers[REGISTER_A]);
      set_flag_n(Registers[REGISTER_A]);
      break;

    /* DECB - Decrement accumulator B. */
    case 0xE1:
      Registers[REGISTER_B]--;
      set_flag_z(Registers[REGISTER_B]);
      set_flag_n(Registers[REGISTER_B]);
      break;

    /* RRC - Rotate right through carry memory. */
    case 0xA2: // Absolute addressing
      rrc_memory(fetch_address_abs());
      break;
    case 0xB2: // Indexed absolute addressing (X)
      rrc_memory(fetch_address_abs_x());
      break;
    case 0xC2: // Indexed absolute addressing (Y)
      rrc_memory(fetch_address_abs_y());
      break;

    /* RRCA - Rotate right through carry accumulator A. */
    case 0xD2:
      rrc_accumulator(REGISTER_A);
      break;

    /* RRCB - Rotate right through carry accumulator B. */
    case 0xE2:
      rrc_accumulator(REGISTER_B);
      break;

    /* RRC - Rotate left through carry memory. */
    case 0xA3: // Absolute addressing
      rlc_memory(fetch_address_abs());
      break;
    case 0xB3: // Indexed absolute addressing (X)
      rlc_memory(fetch_address_abs_x());
      break;
    case 0xC3: // Indexed absolute addressing (Y)
      rlc_memory(fetch_address_abs_y());
      break;

    /* RLCA - Rotate left through carry accumulator A. */
    case 0xD3:
      rlc_accumulator(REGISTER_A);
      break;

    /* RLCB - Rotate left through carry accumulator B. */
    case 0xE3:
      rlc_accumulator(REGISTER_B);
      break;

    // SAL - Arithmetic shift left memory (abs)
    case 0xA4:
      sal_memory(1);
      break;

    // SAL - Arithmetic shift left memory (abs X)
    case 0xB4:
      sal_memory(2);
      break;

    // SAL - Arithmetic shift left memory (abs Y)
    case 0xC4:
      sal_memory(3);
      break;

    // SALA - Arithmetic shift left accumulator
    case 0xD4:
      sal_accumulator(REGISTER_A);
      break;

    // SALB - Arithmetic shift left accumulator
    case 0xE4:
      sal_accumulator(REGISTER_B);
      break;

    // SAR - Arithmetic shift right memory (abs)
    case 0xA5:
      sar_memory(1);
      break;

    // SAR - Arithmetic shift right memory (abs X)
    case 0xB5:
      sar_memory(2);
      break;

    // SAR - Arithmetic shift right memory (abs Y)
    case 0xC5:
      sar_memory(3);
      break;

    // SARA - Arithmetic shift right accumulator
    case 0xD5:
      sar_accumulator(REGISTER_A);
      break;

    // SARB - Arithmetic shift right accumulator
    case 0xE5:
      sar_accumulator(REGISTER_B);
      break;

    // LSR - Shift right memory (abs)
    case 0xA6:
      lsr_memory(1);
      break;

    // LSR - Shift right memory (abs X)
    case 0xB6:
      lsr_memory(2);
      break;

    // LSR - Shift right memory (abs Y)
    case 0xC6:
      lsr_memory(3);
      break;

    // LSRA - Shift right accumulator
    case 0xD6:
      lsr_accumulator(REGISTER_A);
      break;

    // LSRB - Shift right accumulator
    case 0xE6:
      lsr_accumulator(REGISTER_B);
      break;

    // COM - Negate memory (abs)
    case 0xA7:
      com_memory(1);
      break;

    // COM - Negate memory (abs X)
    case 0xB7:
      com_memory(2);
      break;

    // COM - Negate memory (abs Y)
    case 0xC7:
      com_memory(3);
      break;

    // COMA - Negate accumulator
    case 0xD7:
      com_accumulator(REGISTER_A);
      break;

    // COMB - Negate accumulator
    case 0xE7:
      com_accumulator(REGISTER_B);
      break;

		// ROL - Rotate memory left without carry (abs)
		case 0xA8:
			rol_memory(1);
			break;

		// ROL - Rotate memory left without carry (abs X)
		case 0xB8:
			rol_memory(2);
			break;

		// ROL - Rotate memory left without carry (abs Y)
		case 0xC8:
			rol_memory(3);
			break;

		// ROLA - Rotate accumulator left without carry
		case 0xD8:
			rol_accumulator(REGISTER_A);
			break;

		// ROLB - Rotate accumulator left without carry
		case 0xE8:
			rol_accumulator(REGISTER_B);
			break;

		// RR - Rotate memory right without carry (abs)
		case 0xA9:
			rr_memory(1);
			break;

		// RR - Rotate memory right without carry (abs X)
		case 0xB9:
			rr_memory(2);
			break;

		// RR - Rotate memory right without carry (abs Y)
		case 0xC9:
			rr_memory(3);
			break;

		// RRA - Rotate accumulator right without carry
		case 0xD9:
			rr_accumulator(REGISTER_A);
			break;

		// RRB - Rotate accumulator right without carry
		case 0xE9:
			rr_accumulator(REGISTER_B);
			break;

		// LDX
		case 0x0E:
			load_index_register(0, REGISTER_X);
			break;
		case 0x1E:
			load_index_register(1, REGISTER_X);
			break;
		case 0x2E:
			load_index_register(2, REGISTER_X);
			break;
		case 0x3E:
			load_index_register(3, REGISTER_X);
			break;
		case 0x4E:
			load_index_register(4, REGISTER_X);
			break;
		case 0x5E:
			load_index_register(5, REGISTER_X);
			break;

		// STOX
		case 0xBC:
			save_index_register(1, REGISTER_X);
			break;
		case 0xCC:
			save_index_register(2, REGISTER_X);
			break;
		case 0xDC:
			save_index_register(3, REGISTER_X);
			break;
		case 0xEC:
			save_index_register(4, REGISTER_X);
			break;
		case 0xFC:
			save_index_register(5, REGISTER_X);
			break;

		// DECX - Decrements register X
		case 0x01:
			set_flag_z(--Index_Registers[REGISTER_X]);
			break;

		// INCX - Increments register X
		case 0x02:
			set_flag_z(++Index_Registers[REGISTER_X]);
			break;

		// LDY
		case 0x0F:
			load_index_register(0, REGISTER_Y);
			break;
		case 0x1F:
			load_index_register(1, REGISTER_Y);
			break;
		case 0x2F:
			load_index_register(2, REGISTER_Y);
			break;
		case 0x3F:
			load_index_register(3, REGISTER_Y);
			break;
		case 0x4F:
			load_index_register(4, REGISTER_Y);
			break;
		case 0x5F:
			load_index_register(5, REGISTER_Y);
			break;

		// STOY
		case 0xBD:
			save_index_register(1, REGISTER_Y);
			break;
		case 0xCD:
			save_index_register(2, REGISTER_Y);
			break;
		case 0xDD:
			save_index_register(3, REGISTER_Y);
			break;
		case 0xED:
			save_index_register(4, REGISTER_Y);
			break;
		case 0xFD:
			save_index_register(5, REGISTER_Y);
			break;

		// CAY - Transfers accumulator to register Y
		case 0xF0:
			Index_Registers[REGISTER_Y] = Registers[REGISTER_A];
			set_flag_n(Index_Registers[REGISTER_Y]);
			break;

		// MYA - Transfers register Y to accumulator
		case 0xF1:
			Registers[REGISTER_A] = Index_Registers[REGISTER_Y];
			break;

		// DECY - Decrements register Y
		case 0x03:
			set_flag_z(--Index_Registers[REGISTER_Y]);
			break;

		// INCY - Increments register Y
		case 0x04:
			set_flag_z(++Index_Registers[REGISTER_Y]);
			break;

		// LODS
		case 0x20:
		case 0x30:
		case 0x40:
		case 0x50:
		case 0x60:
		case 0x70:
			load_stackpointer(id);
			break;

		// STOS
		case 0x6A:
		case 0x7A:
		case 0x8A:
		case 0x9A:
		case 0xAA:
			save_stackpointer(id);
			break;

		// CSA - Transfers status register to accumulator
		case 0xF2:
			Registers[REGISTER_A] = Flags;
			break;

		// PUSH - Pushes register onto the stack
		case 0xBE:
			Memory[StackPointer--] = Registers[REGISTER_A];
			break;
		case 0xCE:
			Memory[StackPointer--] = Registers[REGISTER_B];
			break;
		case 0xDE:
			Memory[StackPointer--] = Registers[Flags];
			break;
		case 0xEE:
			Memory[StackPointer--] = Registers[REGISTER_L];
			break;
		case 0xFE:
			Memory[StackPointer--] = Registers[REGISTER_H];
			break;

		// POP - Pop the top of the stack to the register
		case 0xBF:
			Registers[REGISTER_A] = Memory[++StackPointer];
			break;
		case 0xCF:
			Registers[REGISTER_B] = Memory[++StackPointer];
			break;
		case 0xDF:
			Registers[Flags] = Memory[++StackPointer];
			break;
		case 0xEF:
			Registers[REGISTER_L] = Memory[++StackPointer];
			break;
		case 0xFF:
			Registers[REGISTER_H] = Memory[++StackPointer];
			break;

		// LX - Loads memory into register pair
		case 0x0C:
		case 0x0D:
			Registers[REGISTER_H] = fetch();
			Registers[REGISTER_L] = fetch();
			break;

		// JMP - Loads memory into program counter
		case 0x10:
			ProgramCounter = fetch_address(1);
			break;

		// ABA - Adds accumulator B into accumulator A
		case 0xF3:
			Registers[REGISTER_A] += Registers[REGISTER_B];
			set_flags_znc(Registers[REGISTER_A]);
			break;

		// SBA - Subtracts accumulator B from accumulator A
		case 0xF4:
			Registers[REGISTER_A] -= Registers[REGISTER_B];
			set_flags_znc(Registers[REGISTER_A]);
			break;

		// AAB - Adds accumulator A into accumulator B
		case 0xF5:
			Registers[REGISTER_B] += Registers[REGISTER_A];
			set_flags_znc(Registers[REGISTER_B]);
			break;

		// SAB - Subtracts accumulator A from accumulator B
		case 0xF6:
			Registers[REGISTER_B] -= Registers[REGISTER_A];
			set_flags_znc(Registers[REGISTER_B]);
			break;

		// MVI - Loads memory into register
		case 0x1C:
			Registers[REGISTER_L] = fetch();
			break;
		case 0x1D:
			Registers[REGISTER_H] = fetch();
			break;

		// ADCP - Adds register pair into accumulator pair
		case 0xF7:
			adc(REGISTER_A, REGISTER_L);
			break;

		// SBCP - Subtracts register pair into accumulator pair
		case 0xF8:
			sbc(REGISTER_A, REGISTER_L);
			break;

		// XCHG - Swaps the registers contents
		case 0x92:
			xchg(REGISTER_A, REGISTER_L);
			break;

  	// JSR - Jump subroutine
	  case 0x21:
      jsr();
	    break;

    // RET - Return from subroutine
	  case 0x4C:
      ret();
	    break;

		// JCC - Jump on carry clear
		case 0x11:
			jump((Flags & FLAG_C) == 0);
			break;

		// JCS - Jump on carry set
		case 0x12:
			jump((Flags & FLAG_C) != 0);
			break;

		// JNE - Jump on result not zero
		case 0x13:
			jump((Flags & FLAG_Z) != FLAG_Z);
			break;

		// JEQ - Jump on result equal to zero
		case 0x14:
			jump((Flags & FLAG_Z) == FLAG_Z);
			break;

		// JMI - Jump on negative result
		case 0x15:
			jump((Flags & FLAG_N) != 0);
			break;

		// JPL - Jump on positive result
		case 0x16:
			jump((Flags & FLAG_N) == 0);
			break;

		// JHI - Jump on result same or lower
		case 0x17:
			jump((Flags & FLAG_Z) != 0 || (Flags & FLAG_C) != 0);
			break;

		// JLE - Jump on result higher
		case 0x18:
			jump(!((Flags == (Flags | FLAG_C)) || (Flags == (Flags | FLAG_Z))));
			break;

    // CCC - Call on carry clear
	  case 0x22:
			jump_subroutine((Flags & FLAG_C) != FLAG_C);
	    break;

    // CCS - Call on carry set
	  case 0x23:
			jump_subroutine((Flags & FLAG_C) == FLAG_C);
	    break;

    // CNE - Call on not zero
	  case 0x24:
			jump_subroutine((Flags & FLAG_Z) != FLAG_Z);
	    break;

		// CEQ - Call on result equal to zero
		case 0x25:
			jump_subroutine((Flags & FLAG_Z) != 0);
			break;

    // CMI - Call on negative result
	  case 0x26:
			jump_subroutine((Flags & FLAG_N) == FLAG_N);
	    break;

    // CPL - Call on positive result
	  case 0x27:
			jump_subroutine((Flags & FLAG_N) != FLAG_N);
	    break;

    // CHI - Call on result same or lower
	  case 0x28:
			jump_subroutine((Flags == (Flags | FLAG_C)) || (Flags == (Flags | FLAG_Z)));
	    break;

	    //START OF CLE - CALL ON RESULT HIGHER
	  case 0x29:
			jump_subroutine(!((Flags == (Flags | FLAG_C)) || (Flags == (Flags | FLAG_Z))));
	    break;

		// CLC - Clear carry flag
		case 0x05:
			Flags = Flags & (0xFF - FLAG_C);
			break;

		// STC - Set carry flag
		case 0x06:
			Flags = Flags | FLAG_C;
			break;

		// CLI - Clear interrupt flag
		case 0x07:
			Flags = Flags & (0xFF - FLAG_I);
			break;

		// STI - Set interrupt flag
		case 0x08:
			Flags = Flags | FLAG_I;
			break;

		// NOP - No operation
		case 0x2C:
			break;

		// HLT - Wait for interrupt
		case 0x2D:
			halt = true;
			break;

		// SWI - Software interrupt
		case 0x5C:
			Flags |= FLAG_I;
			Memory[StackPointer--] = Registers[REGISTER_A];
			Memory[StackPointer--] = Registers[REGISTER_B];
			Memory[StackPointer--] = Registers[Flags];
			Memory[StackPointer--] = Registers[REGISTER_L];
			Memory[StackPointer--] = Registers[REGISTER_H];
			break;

		// RTI - Return from software interrupt
		case 0x5D:
			Registers[REGISTER_L] = Memory[++StackPointer];
			Registers[REGISTER_H] = Memory[++StackPointer];
			Registers[Flags] = Memory[++StackPointer];
			Registers[REGISTER_B] = Memory[++StackPointer];
			Registers[REGISTER_A] = Memory[++StackPointer];
			break;
  }
}

void Group_2_Move(BYTE opcode) {
  BYTE source = opcode >> 4;
  BYTE destination = opcode & 0x0F;
  int destReg;
  int sourceReg;

  switch (source) {
    case 0x06:
      sourceReg = REGISTER_A;
      break;
    case 0x07:
      sourceReg = REGISTER_B;
      break;
    case 0x08:
      sourceReg = REGISTER_L;
      break;
    case 0x09:
      sourceReg = REGISTER_H;
      break;
    case 0x0A:
      sourceReg = REGISTER_M;
      break;
  }

  switch (destination) {
    case 0x0B:
      destReg = REGISTER_A;
      break;
    case 0x0C:
      destReg = REGISTER_B;
      break;
    case 0x0D:
      destReg = REGISTER_L;
      break;
    case 0x0E:
      destReg = REGISTER_H;
      break;
    case 0x0F:
      destReg = REGISTER_M;
      break;
  }

  Registers[destReg] = Registers[sourceReg];
}

void execute(BYTE opcode) {
  if (((opcode >= 0x6B) && (opcode <= 0x6F)) ||
      ((opcode >= 0x7B) && (opcode <= 0x7F)) ||
      ((opcode >= 0x8B) && (opcode <= 0x8F)) ||
      ((opcode >= 0x9B) && (opcode <= 0x9F)) ||
      ((opcode >= 0xAB) && (opcode <= 0xAF))) {
    Group_2_Move(opcode);
  } else {
    Group_1(opcode);
  }
}

void emulate() {
  BYTE opcode;
  int sanity;

  ProgramCounter = 0;
  halt = false;
  memory_in_range = true;
  sanity = 0;

  // printf("                    A  B  L  H  X  Y  SP\n");

  while ((!halt) && (memory_in_range) && (sanity < 200)) {
    // printf("%04X ", ProgramCounter);                       // Print current address
    opcode = fetch();
    execute(opcode);

    // printf("%s  ", opcode_mneumonics[opcode]);              // Print current opcode
    //
    // printf("%02X ", Registers[REGISTER_A]);
    // printf("%02X ", Registers[REGISTER_B]);
    // printf("%02X ", Registers[REGISTER_L]);
    // printf("%02X ", Registers[REGISTER_H]);
    // printf("%02X ", Index_Registers[REGISTER_X]);
    // printf("%02X ", Index_Registers[REGISTER_Y]);
    // printf("%04X ", StackPointer);                          // Print Stack Pointer
    //
    // if ((Flags & FLAG_I) == FLAG_I) {
    //   printf("I=1 ");
    // } else {
    //   printf("I=0 ");
    // }
    //
    // if ((Flags & FLAG_Z) == FLAG_Z) {
    //   printf("Z=1 ");
    // } else {
    //   printf("Z=0 ");
    // }
    //
    // if ((Flags & FLAG_N) == FLAG_N) {
    //   printf("N=1 ");
    // } else {
    //   printf("N=0 ");
    // }
    //
    // if ((Flags & FLAG_C) == FLAG_C) {
    //   printf("C=1 ");
    // } else {
    //   printf("C=0 ");
    // }
    //
    // printf("\n");              // New line
    sanity++;
  }

  // printf("\n");        // New line
}

////////////////////////////////////////////////////////////////////////////////
//                            Simulator/Emulator (End)                        //
////////////////////////////////////////////////////////////////////////////////

void initialise_filenames() {
  int i;

  for (i = 0; i < MAX_FILENAME_SIZE; i++) {
    hex_file[i] = '\0';
    trc_file[i] = '\0';
  }
}

int find_dot_position(char *filename) {
  int dot_position;
  int i;
  char chr;

  dot_position = 0;
  i = 0;
  chr = filename[i];

  while (chr != '\0') {
    if (chr == '.') {
      dot_position = i;
    }

    i++;
    chr = filename[i];
  }

  return (dot_position);
}

int find_end_position(char *filename) {
  int end_position;
  int i;
  char chr;

  end_position = 0;
  i = 0;
  chr = filename[i];

  while (chr != '\0') {
    end_position = i;
    i++;
    chr = filename[i];
  }

  return (end_position);
}

bool file_exists(char *filename) {
  bool exists;
  FILE *ifp;

  exists = false;

  if ((ifp = fopen( filename, "r" )) != NULL) {
    exists = true;
    fclose(ifp);
  }

  return (exists);
}

void create_file(char *filename) {
  FILE *ofp;

  if ((ofp = fopen( filename, "w" )) != NULL) {
    fclose(ofp);
  }
}

bool getLine(FILE *fp, char *buffer) {
  bool rc;
  bool collect;
  char c;
  int i;

  rc = false;
  collect = true;

  i = 0;

  while (collect) {
    c = getc(fp);

    switch (c) {
      case EOF:

        if (i > 0) {
          rc = true;
        }

        collect = false;
        break;

      case '\n':

        if (i > 0) {
          rc = true;
          collect = false;
          buffer[i] = '\0';
        }

        break;

      default:
        buffer[i] = c;
        i++;
        break;
    }
  }

  return (rc);
}

void load_and_run(int args, char **argv) {
  char chr;
  int ln;
  int dot_position;
  int end_position;
  long i;
  FILE *ifp;
  long address;
  long load_at;
  int code;

  // Prompt for the .hex file

  printf("\n");
  printf("Enter the hex filename (.hex): ");

  if (args == 2) {
    ln = 0;
    chr = argv[1][ln];

    while (chr != '\0') {
      if (ln < MAX_FILENAME_SIZE) {
        hex_file[ln] = chr;
        trc_file[ln] = chr;
        ln++;
      }

      chr = argv[1][ln];
    }
  } else {
    ln = 0;
    chr = '\0';

    while (chr != '\n') {
      chr = getchar();

      switch (chr) {
        case '\n':
          break;
        default:

          if (ln < MAX_FILENAME_SIZE) {
            hex_file[ln] = chr;
            trc_file[ln] = chr;
            ln++;
          }

          break;
      }
    }
  }

  // Tidy up the file names

  dot_position = find_dot_position(hex_file);

  if (dot_position == 0) {
    end_position = find_end_position(hex_file);

    hex_file[end_position + 1] = '.';
    hex_file[end_position + 2] = 'h';
    hex_file[end_position + 3] = 'e';
    hex_file[end_position + 4] = 'x';
    hex_file[end_position + 5] = '\0';
  } else {
    hex_file[dot_position + 0] = '.';
    hex_file[dot_position + 1] = 'h';
    hex_file[dot_position + 2] = 'e';
    hex_file[dot_position + 3] = 'x';
    hex_file[dot_position + 4] = '\0';
  }

  dot_position = find_dot_position(trc_file);

  if (dot_position == 0) {
    end_position = find_end_position(trc_file);

    trc_file[end_position + 1] = '.';
    trc_file[end_position + 2] = 't';
    trc_file[end_position + 3] = 'r';
    trc_file[end_position + 4] = 'c';
    trc_file[end_position + 5] = '\0';
  } else {
    trc_file[dot_position + 0] = '.';
    trc_file[dot_position + 1] = 't';
    trc_file[dot_position + 2] = 'r';
    trc_file[dot_position + 3] = 'c';
    trc_file[dot_position + 4] = '\0';
  }

  if (file_exists(hex_file)) {
    // Clear Registers and Memory

    Registers[REGISTER_A] = 0;
    Registers[REGISTER_B] = 0;
    Registers[REGISTER_L] = 0;
    Registers[REGISTER_H] = 0;
    Index_Registers[REGISTER_X] = 0;
    Index_Registers[REGISTER_Y] = 0;
    Flags = 0;
    ProgramCounter = 0;
    StackPointer = 0;

    for (i = 0; i < MEMORY_SIZE; i++) {
      Memory[i] = 0x00;
    }

    // Load hex file

    if ((ifp = fopen( hex_file, "r" )) != NULL) {
      printf("Loading file...\n\n");

      load_at = 0;

      while (getLine(ifp, InputBuffer)) {
        if (sscanf(InputBuffer, "L=%x", &address) == 1) {
          load_at = address;
        } else if (sscanf(InputBuffer, "%x", &code) == 1) {
          if ((load_at >= 0) && (load_at <= MEMORY_SIZE)) {
            Memory[load_at] = (BYTE)code;
          }

          load_at++;
        } else {
          printf("ERROR> Failed to load instruction: %s \n", InputBuffer);
        }
      }

      fclose(ifp);
    }

    // Emulate

    emulate();
  } else {
    printf("\n");
    printf("ERROR> Input file %s does not exist!\n", hex_file);
    printf("\n");
  }
}

void building(int args, char **argv){
  char buffer[1024];
  load_and_run(args, argv);
  sprintf(buffer, "0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X",
      Memory[TEST_ADDRESS_1],
      Memory[TEST_ADDRESS_2],
      Memory[TEST_ADDRESS_3],
      Memory[TEST_ADDRESS_4],
      Memory[TEST_ADDRESS_5],
      Memory[TEST_ADDRESS_6],
      Memory[TEST_ADDRESS_7],
      Memory[TEST_ADDRESS_8],
      Memory[TEST_ADDRESS_9],
      Memory[TEST_ADDRESS_10],
      Memory[TEST_ADDRESS_11],
      Memory[TEST_ADDRESS_12]
      );
  sendto(sock, buffer, strlen(buffer), 0, (SOCKADDR *)&server_addr, sizeof(SOCKADDR));
}

void test_and_mark() {
  char buffer[1024];
  bool testing_complete;
  int len = sizeof(SOCKADDR);
  char chr;
  int i;
  int j;
  bool end_of_program;
  long address;
  long load_at;
  int code;
  int mark;
  int passed;

  printf("\n");
  printf("Automatic Testing and Marking\n");
  printf("\n");

  testing_complete = false;

  sprintf(buffer, "Test Student %s", STUDENT_NUMBER);
  sendto(sock, buffer, strlen(buffer), 0, (SOCKADDR *)&server_addr, sizeof(SOCKADDR));

  int test = 0;
  while (!testing_complete) {
    memset(buffer, '\0', sizeof(buffer));

    if (recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (SOCKADDR *)&client_addr, &len) != SOCKET_ERROR) {
      // printf("Incoming Data: %s \n", buffer);
      // printf("%d - Incoming Data, %s \n", test++, buffer);

      //if (strcmp(buffer, "Testing complete") == 1)
      if (sscanf(buffer, "Testing complete %d", &mark) == 1) {
        testing_complete = true;
        printf("Current mark = %d\n", mark);
      } else if (sscanf(buffer, "Tests passed %d", &passed) == 1) {
        //testing_complete = true;
        printf("Passed = %d\n", passed);
      } else if (strcmp(buffer, "Error") == 0) {
        printf("ERROR> Testing abnormally terminated\n");
        testing_complete = true;
      } else {
        // Clear Registers and Memory

        Registers[REGISTER_A] = 0;
        Registers[REGISTER_B] = 0;
        Registers[REGISTER_L] = 0;
        Registers[REGISTER_H] = 0;
        Index_Registers[REGISTER_X] = 0;
        Index_Registers[REGISTER_Y] = 0;
        Flags = 0;
        ProgramCounter = 0;
        StackPointer = 0;

        for (i = 0; i < MEMORY_SIZE; i++) {
          Memory[i] = 0;
        }

        // Load hex file

        i = 0;
        j = 0;
        load_at = 0;
        end_of_program = false;
        FILE *ofp;
        fopen_s(&ofp, "branch.txt", "a");

        while (!end_of_program) {
          chr = buffer[i];

          switch (chr) {
            case '\0':
              end_of_program = true;

            case ',':

              if (sscanf(InputBuffer, "L=%x", &address) == 1) {
                load_at = address;
              } else if (sscanf(InputBuffer, "%x", &code) == 1) {
                if ((load_at >= 0) && (load_at <= MEMORY_SIZE)) {
                  Memory[load_at] = (BYTE)code;
                  fprintf(ofp, "%02X\n", (BYTE)code);
                }

                load_at++;
              } else {
                printf("ERROR> Failed to load instruction: %s \n", InputBuffer);
              }

              j = 0;
              break;

            default:
              InputBuffer[j] = chr;
              j++;
              break;
          }

          i++;
        }

        fclose(ofp);
        // Emulate

        if (load_at > 1) {
          emulate();
          // Send and store results
          sprintf(buffer, "%02X%02X %02X%02X %02X%02X %02X%02X %02X%02X %02X%02X",
              Memory[TEST_ADDRESS_1],
              Memory[TEST_ADDRESS_2],
              Memory[TEST_ADDRESS_3],
              Memory[TEST_ADDRESS_4],
              Memory[TEST_ADDRESS_5],
              Memory[TEST_ADDRESS_6],
              Memory[TEST_ADDRESS_7],
              Memory[TEST_ADDRESS_8],
              Memory[TEST_ADDRESS_9],
              Memory[TEST_ADDRESS_10],
              Memory[TEST_ADDRESS_11],
              Memory[TEST_ADDRESS_12]
              );
          sendto(sock, buffer, strlen(buffer), 0, (SOCKADDR *)&server_addr, sizeof(SOCKADDR));
        }
      }
    }
  }
}

int main(int argc, char *argv[]) {
  char chr;
  char dummy;

  printf("\n");
  printf("Microprocessor Emulator\n");
  printf("UWE Computer and Network Systems Assignment 1\n");
  printf("\n");

  initialise_filenames();

  if (WSAStartup(MAKEWORD(2, 2), &data) != 0) return (0);

  sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);        // Here we create our socket, which will be a UDP socket (SOCK_DGRAM).

  if (!sock) {
    // Creation failed!
  }

  memset(&server_addr, 0, sizeof(SOCKADDR_IN));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS_SERVER);
  server_addr.sin_port = htons(PORT_SERVER);

  memset(&client_addr, 0, sizeof(SOCKADDR_IN));
  client_addr.sin_family = AF_INET;
  client_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  client_addr.sin_port = htons(PORT_CLIENT);

  chr = '\0';

  while ((chr != 'e') && (chr != 'E')) {
    printf("\n");
    printf("Please select option\n");
    printf("L - Load and run a hex file\n");
    printf("T - Have the server test and mark your emulator\n");
    printf("E - Exit\n");

    if (argc == 2) {
      building(argc, argv); exit(0);
    }

    printf("Enter option: ");
    chr = getchar();

    if (chr != 0x0A) {
      dummy = getchar();                    // read in the <CR>
    }

    printf("\n");

    switch (chr) {
      case 'L':
      case 'l':
        load_and_run(argc, argv);
        break;

      case 'T':
      case 't':
        test_and_mark();
        break;

      default:
        break;
    }
  }

  closesocket(sock);
  WSACleanup();

  return 0;
}
