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

/**
 * Gets the next instruction.
 */
BYTE fetch() {
  if (ProgramCounter < 0 || ProgramCounter > MEMORY_SIZE)
    memory_in_range = false;
  else
    return Memory[ProgramCounter++];
  return 0;
}

/**
 * Updates the zero flag depending on the contents of a register.
 *
 * @param data the register value.
 */
void set_flag_z(BYTE data) {
  if (data == 0)
    Flags = Flags | FLAG_Z;
  else
    Flags = Flags & (0xFF - FLAG_Z);
}

/**
 * Updates the negative flag depending on the contents of a register.
 *
 * @param data the register value.
 */
void set_flag_n(BYTE data) {
  if ((data & 0x80) == 0x80)
    Flags = Flags | FLAG_N;
  else
    Flags = Flags & (0xFF - FLAG_N);
}

/**
 * Updates the carry flag depending on the contents of a register.
 *
 * @param data the register value.
 */
void set_flag_c(WORD data) {
  if (data >= 0x100)
    Flags = Flags | FLAG_C;
  else
    Flags = Flags & (0xFF - FLAG_C);
}

/**
 * Updates the ZNC flags depending on the contents of a register.
 *
 * @param data the register value.
 */
void set_flags_znc(WORD data) {
  set_flag_c(data);
  set_flag_n(data);
  set_flag_z(data);
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

void sal_memory(WORD address) {
  // Update the carry flag.
  if (Memory[address] < 128)
    Flags = Flags & (0xFF - FLAG_C);
  else
    Flags = Flags | FLAG_C;

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
  WORD result = Registers[accumulator] << 1;
  Registers[accumulator] = result;
  set_flags_znc(result);
}

/**
 * Arithmetic shift right memory.
 *
 * @param address the memory address.
 */
void sar_memory(WORD address) {
  // Shift memory to the right.
  BYTE result = Memory[address] >> 1;

  if (Memory[address] > 127)
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
 * Arithmetic shift right accumulator.
 *
 * @param the accumulator to use.
 */
void sar_accumulator(int accumulator) {
  // Shift accumulator to the right.
  BYTE result = Registers[accumulator] >> 1;

  if (Registers[accumulator] > 127)
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
 * Shift right memory.
 *
 * @param address the memory address.
 */
void lsr_memory(WORD address) {
  // Update the carry flag.
  if (Memory[address] % 2 == 0)
    Flags = Flags & (0xFF - FLAG_C);
  else
    Flags = Flags | FLAG_C;

  // Update memory and set negative and zero flags.
  Memory[address] >>= 1;
  set_flag_n(Memory[address]);
  set_flag_z(Memory[address]);
}

/**
 * Shift right accumulator.
 *
 * @param accumulator the accumulator to use.
 */
void lsr_accumulator(int accumulator) {
  // Update the carry flag.
  if (Registers[accumulator] % 2 == 0)
    Flags = Flags & (0xFF - FLAG_C);
  else
    Flags = Flags | FLAG_C;

  // Update memory and set negative and zero flags.
  Registers[accumulator] >>= 1;
  set_flag_n(Registers[accumulator]);
  set_flag_z(Registers[accumulator]);
}

/**
 * Negate memory.
 *
 * @param address the memory address.
 */
void com_memory(WORD address) {
  WORD result = ~Memory[address];
  Memory[address] = result;
  set_flags_znc(result);
}

/**
 * Negate accumulator.
 *
 * @param accumulator the accumulator to use.
 */
void com_accumulator(int accumulator) {
  WORD result = ~Registers[accumulator];
  Registers[accumulator] = result;
  set_flags_znc(result);
}

/**
 * Rotate memory to the left without carry.
 *
 * @param address the memory address.
 */
void rol_memory(WORD address) {
  // Rotate memory to the left.
  BYTE result = Memory[address] << 1;

  if (Memory[address] >= 128)
    result++;

  // Update memory and set negative and zero flags.
  Memory[address] = result;
  set_flag_n(result);
  set_flag_z(result);
}

/**
 * Rotate accumulator to the left without carry.
 *
 * @param accumulator the accumulator to use.
 */
void rol_accumulator(int accumulator) {
  // Rotate accumulator to the left.
  BYTE result = Registers[accumulator] << 1;

  if (Registers[accumulator] >= 128)
    result++;

  // Update accumulator and set negative and zero flags.
  Registers[accumulator] = result;
  set_flag_n(result);
  set_flag_z(result);
}

/**
 * Rotate memory to the right without carry.
 *
 * @param address the memory address.
 */
void rr_memory(WORD address) {
  // Rotate memory to the right.
  BYTE result = Memory[address] >> 1;

  if (Memory[address] % 2 != 0)
    result += 128;

  // Update memory and set negative and zero flags.
  Memory[address] = result;
  set_flag_n(result);
  set_flag_z(result);
}

/**
 * Rotate accumulator to the right without carry.
 *
 * @param accumulator the accumulator to use.
 */
void rr_accumulator(int accumulator) {
  // Rotate accumulator to the right.
  BYTE result = Registers[accumulator] >> 1;

  if (Registers[accumulator] % 2 != 0)
    result += 128;

  // Update accumulator and set negative and zero flags.
  Registers[accumulator] = result;
  set_flag_n(result);
  set_flag_z(result);
}

/**
 * Loads stackpointer from memory.
 *
 * @param id method for retrieving address.
 */
void load_stackpointer(WORD address) {
  StackPointer = (Memory[address] << 8) + Memory[address + 1];
}

/**
 * Saves stackpointer into memory.
 *
 * @param id method for retrieving address.
 */
void save_stackpointer(WORD address) {
  Memory[address] = StackPointer >> 8;
  Memory[address + 1] = StackPointer;
}

/**
 * Swaps the registers contents.
 *
 * @param reg1 the first register.
 * @param reg2 the second register.
 */
void xchg(int reg1, int reg2) {
  BYTE data = Registers[reg1];
  Registers[reg1] = Registers[reg2];
  Registers[reg2] = data;
}

/**
 * Performs a jump if the condition is true.
 *
 * @param condition the condition to check.
 */
void jump(bool condition) {
  WORD address = fetch_address_abs();
  if (condition) {
    ProgramCounter = address;
  }
}

/**
 * Performs a call if the condition is true.
 *
 * @param condition the condition to check.
 */
void call(bool condition) {
  WORD address = fetch_address_abs();
  if (condition) {
    Memory[StackPointer--] = ProgramCounter >> 8;
    Memory[StackPointer--] = ProgramCounter;
    ProgramCounter = address;
  }
}

/* LDAA - Loads memory into accumulator A. */
void ldaa_imm() { Registers[REGISTER_A] = fetch(); }
void ldaa_abs() { Registers[REGISTER_A] = Memory[fetch_address_abs()]; }
void ldaa_abs_x() { Registers[REGISTER_A] = Memory[fetch_address_abs_x()]; }
void ldaa_abs_y() { Registers[REGISTER_A] = Memory[fetch_address_abs_y()]; }
void ldaa_indir() { Registers[REGISTER_A] = Memory[fetch_address_indir()]; }
void ldaa_indir_x() { Registers[REGISTER_A] = Memory[fetch_address_indir_x()]; }

/* LDAB - Loads memory into accumulator B. */
void ldab_imm() { Registers[REGISTER_B] = fetch(); }
void ldab_abs() { Registers[REGISTER_B] = Memory[fetch_address_abs()]; }
void ldab_abs_x() { Registers[REGISTER_B] = Memory[fetch_address_abs_x()]; }
void ldab_abs_y() { Registers[REGISTER_B] = Memory[fetch_address_abs_y()]; }
void ldab_indir() { Registers[REGISTER_B] = Memory[fetch_address_indir()]; }
void ldab_indir_x() { Registers[REGISTER_B] = Memory[fetch_address_indir_x()]; }

/* STORA - Stores accumulator A into memory. */
void stora_abs() { Memory[fetch_address_abs()] = Registers[REGISTER_A]; }
void stora_abs_x() { Memory[fetch_address_abs_x()] = Registers[REGISTER_A]; }
void stora_abs_y() { Memory[fetch_address_abs_y()] = Registers[REGISTER_A]; }
void stora_indir() { Memory[fetch_address_indir()] = Registers[REGISTER_A]; }
void stora_indir_x() { Memory[fetch_address_indir_x()] = Registers[REGISTER_A]; }

/* STORB - Stores accumulator B into memory. */
void storb_abs() { Memory[fetch_address_abs()] = Registers[REGISTER_B]; }
void storb_abs_x() { Memory[fetch_address_abs_x()] = Registers[REGISTER_B]; }
void storb_abs_y() { Memory[fetch_address_abs_y()] = Registers[REGISTER_B]; }
void storb_indir() { Memory[fetch_address_indir()] = Registers[REGISTER_B]; }
void storb_indir_x() { Memory[fetch_address_indir_x()] = Registers[REGISTER_B]; }

/* ADC - Adds register to accumulator with carry. */
void adc_al() { adc(REGISTER_A, REGISTER_L); }
void adc_ah() { adc(REGISTER_A, REGISTER_H); }
void adc_am() { adc(REGISTER_A, REGISTER_M); }
void adc_bl() { adc(REGISTER_B, REGISTER_L); }
void adc_bh() { adc(REGISTER_B, REGISTER_H); }
void adc_bm() { adc(REGISTER_B, REGISTER_M); }

/* SBC - Subtracts register from accumulator with carry. */
void sbc_al() { sbc(REGISTER_A, REGISTER_L); }
void sbc_ah() { sbc(REGISTER_A, REGISTER_H); }
void sbc_am() { sbc(REGISTER_A, REGISTER_M); }
void sbc_bl() { sbc(REGISTER_B, REGISTER_L); }
void sbc_bh() { sbc(REGISTER_B, REGISTER_H); }
void sbc_bm() { sbc(REGISTER_B, REGISTER_M); }

/* ADD - Adds register to accumulator. */
void add_al() { add(REGISTER_A, REGISTER_L); }
void add_ah() { add(REGISTER_A, REGISTER_H); }
void add_am() { add(REGISTER_A, REGISTER_M); }
void add_bl() { add(REGISTER_B, REGISTER_L); }
void add_bh() { add(REGISTER_B, REGISTER_H); }
void add_bm() { add(REGISTER_B, REGISTER_M); }

/* SUB - Subtracts register from accumulator. */
void sub_al() { sub(REGISTER_A, REGISTER_L); }
void sub_ah() { sub(REGISTER_A, REGISTER_H); }
void sub_am() { sub(REGISTER_A, REGISTER_M); }
void sub_bl() { sub(REGISTER_B, REGISTER_L); }
void sub_bh() { sub(REGISTER_B, REGISTER_H); }
void sub_bm() { sub(REGISTER_B, REGISTER_M); }

/* CMP - Compares register to accumulator. */
void cmp_al() { cmp(REGISTER_A, REGISTER_L); }
void cmp_ah() { cmp(REGISTER_A, REGISTER_H); }
void cmp_am() { cmp(REGISTER_A, REGISTER_M); }
void cmp_bl() { cmp(REGISTER_B, REGISTER_L); }
void cmp_bh() { cmp(REGISTER_B, REGISTER_H); }
void cmp_bm() { cmp(REGISTER_B, REGISTER_M); }

/* OR - Register bitwise inclusive or with accumulator. */
void or_al() { _or(REGISTER_A, REGISTER_L); }
void or_ah() { _or(REGISTER_A, REGISTER_H); }
void or_am() { _or(REGISTER_A, REGISTER_M); }
void or_bl() { _or(REGISTER_B, REGISTER_L); }
void or_bh() { _or(REGISTER_B, REGISTER_H); }
void or_bm() { _or(REGISTER_B, REGISTER_M); }

/* AND - Register bitwise and with accumulator */
void and_al() { _and(REGISTER_A, REGISTER_L); }
void and_ah() { _and(REGISTER_A, REGISTER_H); }
void and_am() { _and(REGISTER_A, REGISTER_M); }
void and_bl() { _and(REGISTER_B, REGISTER_L); }
void and_bh() { _and(REGISTER_B, REGISTER_H); }
void and_bm() { _and(REGISTER_B, REGISTER_M); }

/* XOR - Register bitwise exclusive or with accumulator. */
void xor_al() { _xor(REGISTER_A, REGISTER_L); }
void xor_ah() { _xor(REGISTER_A, REGISTER_H); }
void xor_am() { _xor(REGISTER_A, REGISTER_M); }
void xor_bl() { _xor(REGISTER_B, REGISTER_L); }
void xor_bh() { _xor(REGISTER_B, REGISTER_H); }
void xor_bm() { _xor(REGISTER_B, REGISTER_M); }

/* BIT - Register bit tested with accumulator. */
void bit_al() { _bit(REGISTER_A, REGISTER_L); }
void bit_ah() { _bit(REGISTER_A, REGISTER_H); }
void bit_am() { _bit(REGISTER_A, REGISTER_M); }
void bit_bl() { _bit(REGISTER_B, REGISTER_L); }
void bit_bh() { _bit(REGISTER_B, REGISTER_H); }
void bit_bm() { _bit(REGISTER_B, REGISTER_M); }

/* SBIA - Data subtracted from accumulator A with carry. */
void sbia() { sbi_accumulator(REGISTER_A); }

/* SBIB - Data subtracted from accumulator B with carry. */
void sbib() { sbi_accumulator(REGISTER_B); }

/* CPIA - Data compared to accumulator A. */
void cpia() { set_flags_znc(fetch() - Registers[REGISTER_A]); }

/* CPIB - Data compared to accumulator B. */
void cpib() { set_flags_znc(fetch() - Registers[REGISTER_B]); }

/* ORIA - Data bitwise inclusive or with accumulator A. */
void oria() { ori_accumulator(REGISTER_A); }

/* ORIB - Data bitwise inclusive or with accumulator B. */
void orib() { ori_accumulator(REGISTER_B); }

/* INC - Increment memory. */
void inc_abs() { inc_memory(fetch_address_abs()); }
void inc_abs_x() { inc_memory(fetch_address_abs_x()); }
void inc_abs_y() { inc_memory(fetch_address_abs_y()); }

/* INCA - Increment accumulator A. */
void inca() {
  Registers[REGISTER_A]++;
  set_flag_z(Registers[REGISTER_A]);
  set_flag_n(Registers[REGISTER_A]);
}

/* INCB - Increment accumulator B. */
void incb() {
  Registers[REGISTER_B]++;
  set_flag_z(Registers[REGISTER_B]);
  set_flag_n(Registers[REGISTER_B]);
}

/* DEC - Decrement memory. */
void dec_abs() { dec_memory(fetch_address_abs()); }
void dec_abs_x() { dec_memory(fetch_address_abs_x()); }
void dec_abs_y() { dec_memory(fetch_address_abs_y()); }

/* DECA - Decrement accumulator A. */
void deca() {
  Registers[REGISTER_A]--;
  set_flag_z(Registers[REGISTER_A]);
  set_flag_n(Registers[REGISTER_A]);
}

/* DECB - Decrement accumulator B. */
void decb() {
  Registers[REGISTER_B]--;
  set_flag_z(Registers[REGISTER_B]);
  set_flag_n(Registers[REGISTER_B]);
}

/* RRC - Rotate right through carry memory. */
void rrc_abs() { rrc_memory(fetch_address_abs()); }
void rrc_abs_x() { rrc_memory(fetch_address_abs_x()); }
void rrc_abs_y() { rrc_memory(fetch_address_abs_y()); }

/* RRCA - Rotate right through carry accumulator A. */
void rrca() { rrc_accumulator(REGISTER_A); }

/* RRCB - Rotate right through carry accumulator B. */
void rrcb() { rrc_accumulator(REGISTER_B); }

/* RLC - Rotate left through carry memory. */
void rlc_abs() { rlc_memory(fetch_address_abs()); }
void rlc_abs_x() { rlc_memory(fetch_address_abs_x()); }
void rlc_abs_y() { rlc_memory(fetch_address_abs_y()); }

/* RLCA - Rotate left through carry accumulator A. */
void rlca() { rlc_accumulator(REGISTER_A); }

/* RLCB - Rotate left through carry accumulator B. */
void rlcb() { rlc_accumulator(REGISTER_B); }

/* SAL - Arithmetic shift left memory. */
void sal_abs() { sal_memory(fetch_address_abs()); }
void sal_abs_x() { sal_memory(fetch_address_abs_x()); }
void sal_abs_y() { sal_memory(fetch_address_abs_y()); }

/* SALA - Arithmetic shift left accumulator A. */
void sala() { sal_accumulator(REGISTER_A); }

/* SALB - Arithmetic shift left accumulator B. */
void salb() { sal_accumulator(REGISTER_B); }

/* SAR - Arithmetic shift right memory. */
void sar_abs() { sar_memory(fetch_address_abs()); }
void sar_abs_x() { sar_memory(fetch_address_abs_x()); }
void sar_abs_y() { sar_memory(fetch_address_abs_y()); }

/* SARA - Arithmetic shift right accumulator A. */
void sara() { sar_accumulator(REGISTER_A); }

/* SARB - Arithmetic shift right accumulator B. */
void sarb() { sar_accumulator(REGISTER_B); }

/* LSR - Shift right memory. */
void lsr_abs() { lsr_memory(fetch_address_abs()); }
void lsr_abs_x() { lsr_memory(fetch_address_abs_x()); }
void lsr_abs_y() { lsr_memory(fetch_address_abs_y()); }

/* LSRA - Shift right accumulator A. */
void lsra() { lsr_accumulator(REGISTER_A); }

/* LSRB - Shift right accumulator B. */
void lsrb() { lsr_accumulator(REGISTER_B); }

/* COM - Negate memory. */
void com_abs() { com_memory(fetch_address_abs()); }
void com_abs_x() { com_memory(fetch_address_abs_x()); }
void com_abs_y() { com_memory(fetch_address_abs_y()); }

/* COMA - Negate accumulator A. */
void coma() { com_accumulator(REGISTER_A); }

/* COMB - Negate accumulator B. */
void comb() { com_accumulator(REGISTER_B); }

/* ROL - Rotate memory left without carry. */
void rol_abs() { rol_memory(fetch_address_abs()); }
void rol_abs_x() { rol_memory(fetch_address_abs_x()); }
void rol_abs_y() { rol_memory(fetch_address_abs_y()); }

/* ROLA - Rotate accumulator A left without carry. */
void rola() { rol_accumulator(REGISTER_A); }

/* ROLB - Rotate accumulator B left without carry. */
void rolb() { rol_accumulator(REGISTER_B); }

/* RR - Rotate memory right without carry. */
void rr_abs() { rr_memory(fetch_address_abs()); }
void rr_abs_x() { rr_memory(fetch_address_abs_x()); }
void rr_abs_y() { rr_memory(fetch_address_abs_y()); }

/* RRA - Rotate accumulator A right without carry. */
void rra() { rr_accumulator(REGISTER_A); }

/* RRB - Rotate accumulator B right without carry. */
void rrb() { rr_accumulator(REGISTER_B); }

/* LDX - Loads memory into index register X. */
void ldx_imm() { Index_Registers[REGISTER_X] = fetch(); }
void ldx_abs() { Index_Registers[REGISTER_X] = Memory[fetch_address_abs()]; }
void ldx_abs_x() { Index_Registers[REGISTER_X] = Memory[fetch_address_abs_x()]; }
void ldx_abs_y() { Index_Registers[REGISTER_X] = Memory[fetch_address_abs_y()]; }
void ldx_indir() { Index_Registers[REGISTER_X] = Memory[fetch_address_indir()]; }
void ldx_indir_x() { Index_Registers[REGISTER_X] = Memory[fetch_address_indir_x()]; }

/* STOX - Stores index register X into memory. */
void stox_abs() { Memory[fetch_address_abs()] = Index_Registers[REGISTER_X]; }
void stox_abs_x() { Memory[fetch_address_abs_x()] = Index_Registers[REGISTER_X]; }
void stox_abs_y() { Memory[fetch_address_abs_y()] = Index_Registers[REGISTER_X]; }
void stox_indir() { Memory[fetch_address_indir()] = Index_Registers[REGISTER_X]; }
void stox_indir_x() { Memory[fetch_address_indir_x()] = Index_Registers[REGISTER_X]; }

/* DECX - Decrements index register X. */
void decx() { set_flag_z(--Index_Registers[REGISTER_X]); }

/* INCX - Increments index register X. */
void incx() { set_flag_z(++Index_Registers[REGISTER_X]); }

/* LDY - Loads memory into index register Y. */
void ldy_imm() { Index_Registers[REGISTER_Y] = fetch(); }
void ldy_abs() { Index_Registers[REGISTER_Y] = Memory[fetch_address_abs()]; }
void ldy_abs_x() { Index_Registers[REGISTER_Y] = Memory[fetch_address_abs_x()]; }
void ldy_abs_y() { Index_Registers[REGISTER_Y] = Memory[fetch_address_abs_y()]; }
void ldy_indir() { Index_Registers[REGISTER_Y] = Memory[fetch_address_indir()]; }
void ldy_indir_x() { Index_Registers[REGISTER_Y] = Memory[fetch_address_indir_x()]; }

/* STOY - Stores index register Y into memory. */
void stoy_abs() { Memory[fetch_address_abs()] = Index_Registers[REGISTER_Y]; }
void stoy_abs_x() { Memory[fetch_address_abs_x()] = Index_Registers[REGISTER_Y]; }
void stoy_abs_y() { Memory[fetch_address_abs_y()] = Index_Registers[REGISTER_Y]; }
void stoy_indir() { Memory[fetch_address_indir()] = Index_Registers[REGISTER_Y]; }
void stoy_indir_x() { Memory[fetch_address_indir_x()] = Index_Registers[REGISTER_Y]; }

/* CAY - Transfers accumulator A to index register Y. */
void cay() {
  Index_Registers[REGISTER_Y] = Registers[REGISTER_A];
  set_flag_n(Index_Registers[REGISTER_Y]);
}

/* MYA - Transfers index register Y to accumulator A. */
void mya() { Registers[REGISTER_A] = Index_Registers[REGISTER_Y]; }

/* DECY - Decrements index register Y. */
void decy() { set_flag_z(--Index_Registers[REGISTER_Y]); }

/* INCY - Increments index register Y. */
void incy() { set_flag_z(++Index_Registers[REGISTER_Y]); }

/* LODS - Loads memory into stackpointer. */
void lods_imm() { StackPointer = (fetch() << 8) + fetch(); }
void lods_abs() { load_stackpointer(fetch_address_abs()); }
void lods_abs_x() { load_stackpointer(fetch_address_abs_x()); }
void lods_abs_y() { load_stackpointer(fetch_address_abs_y()); }
void lods_indir() { load_stackpointer(fetch_address_indir()); }
void lods_indir_x() { load_stackpointer(fetch_address_indir_x()); }

/* STOS - Stores stackpointer into memory. */
void stos_abs() { save_stackpointer(fetch_address_abs()); }
void stos_abs_x() { save_stackpointer(fetch_address_abs_x()); }
void stos_abs_y() { save_stackpointer(fetch_address_abs_y()); }
void stos_indir() { save_stackpointer(fetch_address_indir()); }
void stos_indir_x() { save_stackpointer(fetch_address_indir_x()); }

/* CSA - Transfers status register to accumulator A. */
void csa() { Registers[REGISTER_A] = Flags; }

/* PUSH - Pushes register onto the stack. */
void push_a() { Memory[StackPointer--] = Registers[REGISTER_A]; }
void push_b() { Memory[StackPointer--] = Registers[REGISTER_B]; }
void push_f() { Memory[StackPointer--] = Registers[Flags]; }
void push_l() { Memory[StackPointer--] = Registers[REGISTER_L]; }
void push_h() { Memory[StackPointer--] = Registers[REGISTER_H]; }

/* POP - Pop the top of the stack to the register. */
void pop_a() { Registers[REGISTER_A] = Memory[++StackPointer]; }
void pop_b() { Registers[REGISTER_B] = Memory[++StackPointer]; }
void pop_f() { Registers[Flags] = Memory[++StackPointer]; }
void pop_l() { Registers[REGISTER_L] = Memory[++StackPointer]; }
void pop_h() { Registers[REGISTER_H] = Memory[++StackPointer]; }

/* LX - Loads memory into register pair. */
void lx() {
  Registers[REGISTER_H] = fetch();
  Registers[REGISTER_L] = fetch();
}

/* JMP - Loads memory into program counter. */
void jmp() { ProgramCounter = fetch_address_abs(); }

/* ABA - Adds accumulator B into accumulator A. */
void aba() {
  Registers[REGISTER_A] += Registers[REGISTER_B];
  set_flags_znc(Registers[REGISTER_A]);
}

/* SBA - Subtracts accumulator B from accumulator A. */
void sba() {
  Registers[REGISTER_A] -= Registers[REGISTER_B];
  set_flags_znc(Registers[REGISTER_A]);
}

/* AAB - Adds accumulator A into accumulator B. */
void aab() {
  Registers[REGISTER_B] += Registers[REGISTER_A];
  set_flags_znc(Registers[REGISTER_B]);
}

/* SAB - Subtracts accumulator A from accumulator B. */
void sab() {
  Registers[REGISTER_B] -= Registers[REGISTER_A];
  set_flags_znc(Registers[REGISTER_B]);
}

/* MVI - Loads memory into register. */
void mvi_l() { Registers[REGISTER_L] = fetch(); }
void mvi_h() { Registers[REGISTER_H] = fetch(); }

/* ADCP - Adds register pair into accumulator pair. */
void adcp() { adc(REGISTER_A, REGISTER_L); }

/* SBCP - Subtracts register pair into accumulator pair. */
void sbcp() { sbc(REGISTER_A, REGISTER_L); }

/* XCHG - Swaps the registers contents. */
void xchg_al() { xchg(REGISTER_A, REGISTER_L); }

/* JSR - Jump subroutine. */
void jsr() {
  WORD address = fetch_address_abs();
  Memory[StackPointer--] = ProgramCounter >> 8;
  Memory[StackPointer--] = ProgramCounter;
  ProgramCounter = address;
}

/* RET - Return from subroutine. */
void ret() {
  BYTE higher = Memory[StackPointer++];
  BYTE lower = Memory[StackPointer++];
  ProgramCounter = join_address(higher, lower);
}

/* JCC - Jump on carry clear. */
void jcc() { jump((Flags & FLAG_C) == 0); }

/* JCS - Jump on carry set. */
void jcs() { jump((Flags & FLAG_C) != 0); }

/* JNE - Jump on result not zero. */
void jne() { jump((Flags & FLAG_Z) != FLAG_Z); }

/* JEQ - Jump on result equal to zero. */
void jeq() { jump((Flags & FLAG_Z) == FLAG_Z); }

/* JMI - Jump on negative result. */
void jmi() { jump((Flags & FLAG_N) != 0); }

/* JPL - Jump on positive result. */
void jpl() { jump((Flags & FLAG_N) == 0); }

/* JHI - Jump on result same or lower. */
void jhi() { jump((Flags & FLAG_Z) != 0 || (Flags & FLAG_C) != 0); }

/* JLE - Jump on result higher. */
void jle() { jump(!((Flags == (Flags | FLAG_C)) || (Flags == (Flags | FLAG_Z)))); }

/* CCC - Call on carry clear. */
void ccc() { call((Flags & FLAG_C) != FLAG_C); }

/* CCS - Call on carry set. */
void ccs() { call((Flags & FLAG_C) == FLAG_C); }

/* CNE - Call on not zero. */
void cne() { call((Flags & FLAG_Z) != FLAG_Z); }

/* CEQ - Call on result equal to zero. */
void ceq() { call((Flags & FLAG_Z) != 0); }

/* CMI - Call on negative result. */
void cmi() { call((Flags & FLAG_N) == FLAG_N); }

/* CPL - Call on positive result. */
void cpl() { call((Flags & FLAG_N) != FLAG_N); }

/* CHI - Call on result same or lower. */
void chi() { call((Flags == (Flags | FLAG_C)) || (Flags == (Flags | FLAG_Z))); }

/* CLE - Call on result higher. */
void cle() { call(!((Flags == (Flags | FLAG_C)) || (Flags == (Flags | FLAG_Z)))); }

/* CLC - Clear carry flag. */
void clc() { Flags = Flags & (0xFF - FLAG_C); }

/* STC - Set carry flag. */
void stc() { Flags = Flags | FLAG_C; }

/* CLI - Clear interrupt flag. */
void cli() { Flags = Flags & (0xFF - FLAG_I); }

/* STI - Set interrupt flag. */
void sti() { Flags = Flags | FLAG_I; }

/* NOP - No operation. */
void nop() {}

/* HLT - Wait for interrupt. */
void hlt() { halt = true; }

/* SWI - Software interrupt. */
void swi() {
  Flags |= FLAG_I;
  Memory[StackPointer--] = Registers[REGISTER_A];
  Memory[StackPointer--] = Registers[REGISTER_B];
  Memory[StackPointer--] = Registers[Flags];
  Memory[StackPointer--] = Registers[REGISTER_L];
  Memory[StackPointer--] = Registers[REGISTER_H];
}

/* RTI - Return from software interrupt. */
void rti() {
  Registers[REGISTER_L] = Memory[++StackPointer];
  Registers[REGISTER_H] = Memory[++StackPointer];
  Registers[Flags] = Memory[++StackPointer];
  Registers[REGISTER_B] = Memory[++StackPointer];
  Registers[REGISTER_A] = Memory[++StackPointer];
}

/* MOVE - Transfers from one register to another. */
void move_aa() {}
void move_ab() { Registers[REGISTER_A] = Registers[REGISTER_B]; }
void move_al() { Registers[REGISTER_A] = Registers[REGISTER_L]; }
void move_ah() { Registers[REGISTER_A] = Registers[REGISTER_H]; }
void move_am() { Registers[REGISTER_A] = Registers[REGISTER_M]; }
void move_ba() { Registers[REGISTER_B] = Registers[REGISTER_A]; }
void move_bb() {}
void move_bl() { Registers[REGISTER_B] = Registers[REGISTER_L]; }
void move_bh() { Registers[REGISTER_B] = Registers[REGISTER_H]; }
void move_bm() { Registers[REGISTER_B] = Registers[REGISTER_M]; }
void move_la() { Registers[REGISTER_L] = Registers[REGISTER_A]; }
void move_lb() { Registers[REGISTER_L] = Registers[REGISTER_B]; }
void move_ll() {}
void move_lh() { Registers[REGISTER_L] = Registers[REGISTER_H]; }
void move_lm() { Registers[REGISTER_L] = Registers[REGISTER_M]; }
void move_ha() { Registers[REGISTER_H] = Registers[REGISTER_A]; }
void move_hb() { Registers[REGISTER_H] = Registers[REGISTER_B]; }
void move_hl() { Registers[REGISTER_H] = Registers[REGISTER_L]; }
void move_hh() {}
void move_hm() { Registers[REGISTER_H] = Registers[REGISTER_M]; }
void move_ma() { Registers[REGISTER_M] = Registers[REGISTER_A]; }
void move_mb() { Registers[REGISTER_M] = Registers[REGISTER_B]; }
void move_ml() { Registers[REGISTER_M] = Registers[REGISTER_L]; }
void move_mh() { Registers[REGISTER_M] = Registers[REGISTER_H]; }
void move_mm() {}

void (*operations[])() = {
  nop, decx, incx, decy, incy, clc, stc, cli,
  sti, nop, ldaa_imm, ldab_imm, lx, lx, ldx_imm, ldy_imm,
  jmp, jcc, jcs, jne, jeq, jmi, jpl, jhi,
  jle, nop, ldaa_abs, ldab_abs, mvi_l, mvi_h, ldx_abs, ldy_abs,
  lods_imm, jsr, ccc, ccs, cne, ceq, cmi, cpl,
  chi, cle, ldaa_abs_x, ldab_abs_x, nop, hlt, ldx_abs_x, ldy_abs_x,
  lods_abs, adc_al, sbc_al, add_al, sub_al, cmp_al, or_al, and_al,
  xor_al, bit_al, ldaa_abs_y, ldab_abs_y, nop, nop, ldx_abs_y, ldy_abs_y,
  lods_abs_x, adc_ah, sbc_ah, add_ah, sub_ah, cmp_ah, or_ah, and_ah,
  xor_ah, bit_ah, ldaa_indir, ldab_indir, ret, nop, ldx_indir, ldy_indir,
  lods_abs_y, adc_am, sbc_am, add_am, sub_am, cmp_am, or_am, and_am,
  xor_am, bit_am, ldaa_indir_x, ldab_indir_x, swi, rti, ldx_indir_x, ldy_indir_x,
  lods_indir, adc_bl, sbc_bl, add_bl, sub_bl, cmp_bl, or_bl, and_bl,
  xor_bl, bit_bl, stos_abs, move_aa, move_ba, move_la, move_ha, move_ma,
  lods_indir_x, adc_bh, sbc_bh, add_bh, sub_bh, cmp_bh, or_bh, and_bh,
  xor_bh, bit_bh, stos_abs_x, move_ab, move_bb, move_lb, move_hb, move_mb,
  nop, adc_bm, sbc_bm, add_bm, sub_bm, cmp_bm, or_bm, and_bm,
  xor_bm, bit_bm, stos_abs_y, move_al, move_bl, move_ll, move_hl, move_ml,
  nop, nop, nop, sbia, sbib, cpia, cpib, oria,
  orib, nop, stos_indir, move_ah, move_bh, move_lh, move_hh, move_mh,
  inc_abs, dec_abs, rrc_abs, rlc_abs, sal_abs, sar_abs, lsr_abs, com_abs,
  rol_abs, rr_abs, stos_indir_x, move_am, move_bm, move_lm, move_hm, move_mm,
  inc_abs_x, dec_abs_x, rrc_abs_x, rlc_abs_x, sal_abs_x, sar_abs_x, lsr_abs_x, com_abs_x,
  rol_abs_x, rr_abs_x, stora_abs, storb_abs, stox_abs, stoy_abs, push_a, pop_a,
  inc_abs_y, dec_abs_y, rrc_abs_y, rlc_abs_y, sal_abs_y, sar_abs_y, lsr_abs_y, com_abs_y,
  rol_abs_y, rr_abs_y, stora_abs_x, storb_abs_x, stox_abs_x, stoy_abs_x, push_b, pop_b,
  inca, deca, rrca, rlca, sala, sara, lsra, coma,
  rola, rra, stora_abs_y, storb_abs_y, stox_abs_y, stoy_abs_y, push_f, pop_f,
  incb, decb, rrcb, rlcb, salb, sarb, lsrb, comb,
  rolb, rrb, stora_indir, storb_indir, stox_indir, stoy_indir, push_l, pop_l,
  cay, mya, csa, aba, sba, aab, sab, adcp,
  sbcp, xchg_al, stora_indir_x, storb_indir_x, stox_indir_x, stoy_indir_x, push_h, pop_h
};

void execute(BYTE opcode) {
  operations[opcode]();
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
