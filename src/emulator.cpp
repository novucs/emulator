/**
 * @file emulator.cpp
 * @author William Randall (16002374)
 * @date 11th November 2016
 * @brief Complete 60 marks emulator for CaNS assignment 1 at UWE.
 */

#include <winsock2.h>
#include <stdio.h>

#pragma comment(lib, "wsock32.lib")

#define STUDENT_NUMBER "16002374"
#define IP_ADDRESS_SERVER "127.0.0.1"
#define PORT_SERVER 0x1984
#define PORT_CLIENT 0x1985

#define WORD  unsigned short
#define DWORD unsigned long
#define BYTE  unsigned char

#define MAX_FILENAME_SIZE 500
#define MAX_BUFFER_SIZE 500

SOCKADDR_IN server_addr;
SOCKADDR_IN client_addr;
SOCKET sock;
WSADATA data;

char input_buffer[MAX_BUFFER_SIZE];
char hex_file[MAX_BUFFER_SIZE];
char trc_file[MAX_BUFFER_SIZE];

///////////////
// Registers //
///////////////

#define FLAG_I 0x10
#define FLAG_Z 0x04
#define FLAG_N 0x02
#define FLAG_C 0x01
#define REGISTER_M 4
#define REGISTER_A 3
#define REGISTER_B 2
#define REGISTER_H 1
#define REGISTER_L 0
BYTE registers[5];

#define REGISTER_X 0
#define REGISTER_Y 1
BYTE index_registers[2];

BYTE flags;
WORD program_counter;
WORD stack_pointer;

////////////
// Memory //
////////////

#define MEMORY_SIZE 65536

BYTE memory[MEMORY_SIZE];

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

bool halt = false;

////////////////////////////////////////////////////////////////////////////////
//                           Simulator/Emulator (Start)                       //
////////////////////////////////////////////////////////////////////////////////

/**
 * Checks if a flag is set.
 *
 * @param flag the flag to check.
 */
bool is_flag_set(BYTE flag) {
  return flags & flag;
}

/**
 * Updates a flag to a new state.
 *
 * @param state the new flag state.
 * @param flag the flag to update.
 */
void set_flag(bool state, BYTE flag) {
  state ? flags |= flag : flags &= ~flag;
}

/**
 * Updates the zero flag depending on the contents of a register.
 *
 * @param data the register value.
 */
void set_flag_z(BYTE data) {
  set_flag(data == 0, FLAG_Z);
}

/**
 * Updates the negative flag depending on the contents of a register.
 *
 * @param data the register value.
 */
void set_flag_n(BYTE data) {
  set_flag(data & 0x80, FLAG_N);
}

/**
 * Updates the carry flag depending on the contents of a register.
 *
 * @param data the register value.
 */
void set_flag_c(WORD data) {
  set_flag(data > 0xFF, FLAG_C);
}

/**
 * Updates the Z and N flags depending on the contents of a register.
 *
 * @param data the register value.
 */
 void set_flags_zn(BYTE data) {
   set_flag_z(data);
   set_flag_n(data);
 }

/**
 * Updates the Z, N and C flags depending on the contents of a register.
 *
 * @param data the register value.
 */
void set_flags_znc(WORD data) {
  set_flags_zn(data);
  set_flag_c(data);
}

/**
 * Gets the next instruction from memory.
 */
BYTE fetch() {
  return memory[program_counter++];
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
  return fetch_address_abs() + index_registers[REGISTER_X];
}

/**
 * Fetch an address using indexed absolute addressing (Y).
 */
WORD fetch_address_abs_y() {
  return fetch_address_abs() + index_registers[REGISTER_Y];
}

/**
 * Fetch an address using indirect addressing.
 */
WORD fetch_address_indir() {
  WORD address = fetch_address_abs();
  return join_address(memory[address], memory[address + 1]);
}

/**
 * Fetch an address using indexed indirect addressing (X).
 */
WORD fetch_address_indir_x() {
  return fetch_address_indir() + index_registers[REGISTER_X];
}

/**
 * Adds register to accumulator with carry.
 *
 * @param accumulator the accumulator to add to.
 * @param reg         the register to add.
 */
void adc(int accumulator, int reg) {
  WORD result = registers[accumulator] + registers[reg];

  if (is_flag_set(FLAG_C))
    result++;

  registers[accumulator] = result;
  set_flags_znc(result);
}

/**
 * Subtracts register to accumulator with carry.
 *
 * @param accumulator the accumulator to subtract from.
 * @param reg         the register to subtract.
 */
void sbc(int accumulator, int reg) {
  WORD result = registers[accumulator] - registers[reg];

  if (is_flag_set(FLAG_C))
    result--;

  registers[accumulator] = result;
  set_flags_znc(result);
}

/**
 * Adds register to accumulator.
 *
 * @param accumulator the accumulator to add to.
 * @param reg         the register to add.
 */
void add(int accumulator, int reg) {
  WORD result = registers[accumulator] + registers[reg];
  registers[accumulator] = result;
  set_flags_znc(result);
}

/**
 * Subtracts register from accumulator.
 *
 * @param accumulator the accumulator to subtract from.
 * @param reg         the register to subtract.
 */
void sub(int accumulator, int reg) {
  WORD result = registers[accumulator] - registers[reg];
  registers[accumulator] = result;
  set_flags_znc(result);
}

/**
 * Register compared to accumulator.
 *
 * @param accumulator the accumulator to compare.
 * @param reg         the register to compare.
 */
void cmp(int accumulator, int reg) {
  set_flags_znc(registers[accumulator] - registers[reg]);
}

/**
 * Register bitwise inclusive or with accumulator.
 *
 * @param accumulator the accumulator.
 * @param reg         the register.
 */
void _or(int accumulator, int reg) {
  set_flags_zn(registers[accumulator] |= registers[reg]);
}

/**
 * Register bitwise and with accumulator.
 *
 * @param accumulator the accumulator.
 * @param reg         the register.
 */
void _and(int accumulator, int reg) {
  set_flags_zn(registers[accumulator] &= registers[reg]);
}

/**
 * Register bitwise exclusive or with accumulator.
 *
 * @param accumulator the accumulator.
 * @param reg         the register.
 */
void _xor(int accumulator, int reg) {
  set_flags_zn(registers[accumulator] ^= registers[reg]);
}

/**
 * Register bit tested with with accumulator.
 *
 * @param accumulator the accumulator.
 * @param reg         the register.
 */
void _bit(int accumulator, int reg) {
  set_flags_zn(registers[accumulator] & registers[reg]);
}

/**
 * Subtracts data from accumulator with carry.
 *
 * @param accumulator the accumulator to subtract from.
 */
void sbi(BYTE accumulator) {
  WORD result = fetch() - registers[accumulator];

  if (is_flag_set(FLAG_C))
    result--;

  set_flags_znc(result);
  registers[accumulator] = result;
}

/**
 * Data bitwise inclusive or with accumulator.
 *
 * @param accumulator the accumulator to use.
 */
void ori(BYTE accumulator) {
  set_flags_zn(registers[accumulator] |= fetch());
}

/**
 * Increment memory.
 *
 * @param address the memory address.
 */
void inc(WORD address) {
  set_flags_zn(++memory[address]);
}

/**
 * Decrement memory.
 *
 * @param address the memory address.
 */
void dec(WORD address) {
  set_flags_zn(--memory[address]);
}

/**
 * Rotates memory or accumulator right through carry.
 *
 * @param data the memory or accumulator.
 */
void rrc(BYTE *data) {
  // Rotate memory to the right.
  BYTE result = *data >> 1;

  if (is_flag_set(FLAG_C))
    result += 0x80;

  // Update the carry flag.
  set_flag(*data % 2, FLAG_C);

  // Update data and set negative and zero flags.
  *data = result;
  set_flags_zn(result);
}

/**
 * Rotates memory or accumulator left through carry.
 *
 * @param data the memory or accumulator.
 */
void rlc(BYTE *data) {
  // Rotate memory to the left.
  BYTE result = *data << 1;

  if (is_flag_set(FLAG_C))
    result++;

  // Update the carry flag.
  set_flag(*data >= 0x80, FLAG_C);

  // Update data and set negative and zero flags.
  *data = result;
  set_flags_zn(result);
}

/**
 * Arithmetic shift left memory or accumulator.
 *
 * @param data the memory or accumulator.
 */
void sal(BYTE *data) {
  // Update the carry flag.
  set_flag(*data >= 0x80, FLAG_C);

  // Update data and set negative and zero flags.
  set_flags_zn(*data <<= 1);
}

/**
 * Arithmetic shift right memory or accumulator.
 *
 * @param data the memory or accumulator.
 */
void sar(BYTE *data) {
  // Shift memory to the right.
  BYTE result = *data >> 1;

  if (*data >= 0x80)
    result += 0x80;

  // Update the carry flag.
  set_flag(*data % 2, FLAG_C);

  // Update data and set negative and zero flags.
  set_flags_zn(*data = result);
}

/**
 * Shift right memory or accumulator.
 *
 * @param data the memory or accumulator.
 */
void lsr(BYTE *data) {
  // Update the carry flag.
  set_flag(*data % 2, FLAG_C);

  // Update data and set negative and zero flags.
  set_flags_zn(*data >>= 1);
}

/**
 * Negate memory or accumulator.
 *
 * @param data the memory or accumulator.
 */
void com(BYTE *data) {
  WORD result = ~*data;
  *data = result;
  set_flags_znc(result);
}

/**
 * Rotate memory or accumulator to the left without carry.
 *
 * @param data the memory or accumulator.
 */
void rol(BYTE *data) {
  // Rotate memory to the left.
  BYTE result = *data << 1;

  if (*data >= 0x80)
    result++;

  // Update data and set negative and zero flags.
  set_flags_zn(*data = result);
}

/**
 * Rotate memory or accumulator to the right without carry.
 *
 * @param data the memory or accumulator.
 */
void rr(BYTE *data) {
  // Rotate memory to the right.
  BYTE result = *data >> 1;

  if (*data % 2)
    result += 0x80;

  // Update data and set negative and zero flags.
  set_flags_zn(*data = result);
}

/**
 * Loads stackpointer from memory.
 *
 * @param id method for retrieving address.
 */
void load_stackpointer(WORD address) {
  stack_pointer = (memory[address] << 8) + memory[address + 1];
}

/**
 * Saves stackpointer into memory.
 *
 * @param id method for retrieving address.
 */
void save_stackpointer(WORD address) {
  memory[address] = stack_pointer >> 8;
  memory[address + 1] = stack_pointer;
}

/**
 * Performs a jump if the condition is true.
 *
 * @param condition the condition to check.
 */
void jump(bool condition) {
  WORD address = fetch_address_abs();
  if (condition) {
    program_counter = address;
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
    memory[stack_pointer--] = program_counter >> 8;
    memory[stack_pointer--] = program_counter;
    program_counter = address;
  }
}

/* LDAA - Loads memory into accumulator A. */
void ldaa_imm() { registers[REGISTER_A] = fetch(); }
void ldaa_abs() { registers[REGISTER_A] = memory[fetch_address_abs()]; }
void ldaa_abs_x() { registers[REGISTER_A] = memory[fetch_address_abs_x()]; }
void ldaa_abs_y() { registers[REGISTER_A] = memory[fetch_address_abs_y()]; }
void ldaa_indir() { registers[REGISTER_A] = memory[fetch_address_indir()]; }
void ldaa_indir_x() { registers[REGISTER_A] = memory[fetch_address_indir_x()]; }

/* LDAB - Loads memory into accumulator B. */
void ldab_imm() { registers[REGISTER_B] = fetch(); }
void ldab_abs() { registers[REGISTER_B] = memory[fetch_address_abs()]; }
void ldab_abs_x() { registers[REGISTER_B] = memory[fetch_address_abs_x()]; }
void ldab_abs_y() { registers[REGISTER_B] = memory[fetch_address_abs_y()]; }
void ldab_indir() { registers[REGISTER_B] = memory[fetch_address_indir()]; }
void ldab_indir_x() { registers[REGISTER_B] = memory[fetch_address_indir_x()]; }

/* STORA - Stores accumulator A into memory. */
void stora_abs() { memory[fetch_address_abs()] = registers[REGISTER_A]; }
void stora_abs_x() { memory[fetch_address_abs_x()] = registers[REGISTER_A]; }
void stora_abs_y() { memory[fetch_address_abs_y()] = registers[REGISTER_A]; }
void stora_indir() { memory[fetch_address_indir()] = registers[REGISTER_A]; }
void stora_indir_x() { memory[fetch_address_indir_x()] = registers[REGISTER_A]; }

/* STORB - Stores accumulator B into memory. */
void storb_abs() { memory[fetch_address_abs()] = registers[REGISTER_B]; }
void storb_abs_x() { memory[fetch_address_abs_x()] = registers[REGISTER_B]; }
void storb_abs_y() { memory[fetch_address_abs_y()] = registers[REGISTER_B]; }
void storb_indir() { memory[fetch_address_indir()] = registers[REGISTER_B]; }
void storb_indir_x() { memory[fetch_address_indir_x()] = registers[REGISTER_B]; }

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
void sbia() { sbi(REGISTER_A); }

/* SBIB - Data subtracted from accumulator B with carry. */
void sbib() { sbi(REGISTER_B); }

/* CPIA - Data compared to accumulator A. */
void cpia() { set_flags_znc(fetch() - registers[REGISTER_A]); }

/* CPIB - Data compared to accumulator B. */
void cpib() { set_flags_znc(fetch() - registers[REGISTER_B]); }

/* ORIA - Data bitwise inclusive or with accumulator A. */
void oria() { ori(REGISTER_A); }

/* ORIB - Data bitwise inclusive or with accumulator B. */
void orib() { ori(REGISTER_B); }

/* INC - Increment memory. */
void inc_abs() { inc(fetch_address_abs()); }
void inc_abs_x() { inc(fetch_address_abs_x()); }
void inc_abs_y() { inc(fetch_address_abs_y()); }

/* INCA - Increment accumulator A. */
void inca() { set_flags_zn(++registers[REGISTER_A]); }

/* INCB - Increment accumulator B. */
void incb() { set_flags_zn(++registers[REGISTER_B]); }

/* DEC - Decrement memory. */
void dec_abs() { dec(fetch_address_abs()); }
void dec_abs_x() { dec(fetch_address_abs_x()); }
void dec_abs_y() { dec(fetch_address_abs_y()); }

/* DECA - Decrement accumulator A. */
void deca() { set_flags_zn(--registers[REGISTER_A]); }

/* DECB - Decrement accumulator B. */
void decb() { set_flags_zn(--registers[REGISTER_B]); }

/* RRC - Rotate right through carry memory. */
void rrc_abs() { rrc(&memory[fetch_address_abs()]); }
void rrc_abs_x() { rrc(&memory[fetch_address_abs_x()]); }
void rrc_abs_y() { rrc(&memory[fetch_address_abs_y()]); }

/* RRCA - Rotate right through carry accumulator A. */
void rrca() { rrc(&registers[REGISTER_A]); }

/* RRCB - Rotate right through carry accumulator B. */
void rrcb() { rrc(&registers[REGISTER_B]); }

/* RLC - Rotate left through carry memory. */
void rlc_abs() { rlc(&memory[fetch_address_abs()]); }
void rlc_abs_x() { rlc(&memory[fetch_address_abs_x()]); }
void rlc_abs_y() { rlc(&memory[fetch_address_abs_y()]); }

/* RLCA - Rotate left through carry accumulator A. */
void rlca() { rlc(&registers[REGISTER_A]); }

/* RLCB - Rotate left through carry accumulator B. */
void rlcb() { rlc(&registers[REGISTER_B]); }

/* SAL - Arithmetic shift left memory. */
void sal_abs() { sal(&memory[fetch_address_abs()]); }
void sal_abs_x() { sal(&memory[fetch_address_abs_x()]); }
void sal_abs_y() { sal(&memory[fetch_address_abs_y()]); }

/* SALA - Arithmetic shift left accumulator A. */
void sala() { sal(&registers[REGISTER_A]); }

/* SALB - Arithmetic shift left accumulator B. */
void salb() { sal(&registers[REGISTER_B]); }

/* SAR - Arithmetic shift right memory. */
void sar_abs() { sar(&memory[fetch_address_abs()]); }
void sar_abs_x() { sar(&memory[fetch_address_abs_x()]); }
void sar_abs_y() { sar(&memory[fetch_address_abs_y()]); }

/* SARA - Arithmetic shift right accumulator A. */
void sara() { sar(&registers[REGISTER_A]); }

/* SARB - Arithmetic shift right accumulator B. */
void sarb() { sar(&registers[REGISTER_B]); }

/* LSR - Shift right memory. */
void lsr_abs() { lsr(&memory[fetch_address_abs()]); }
void lsr_abs_x() { lsr(&memory[fetch_address_abs_x()]); }
void lsr_abs_y() { lsr(&memory[fetch_address_abs_y()]); }

/* LSRA - Shift right accumulator A. */
void lsra() { lsr(&registers[REGISTER_A]); }

/* LSRB - Shift right accumulator B. */
void lsrb() { lsr(&registers[REGISTER_B]); }

/* COM - Negate memory. */
void com_abs() { com(&memory[fetch_address_abs()]); }
void com_abs_x() { com(&memory[fetch_address_abs_x()]); }
void com_abs_y() { com(&memory[fetch_address_abs_y()]); }

/* COMA - Negate accumulator A. */
void coma() { com(&registers[REGISTER_A]); }

/* COMB - Negate accumulator B. */
void comb() { com(&registers[REGISTER_B]); }

/* ROL - Rotate memory left without carry. */
void rol_abs() { rol(&memory[fetch_address_abs()]); }
void rol_abs_x() { rol(&memory[fetch_address_abs_x()]); }
void rol_abs_y() { rol(&memory[fetch_address_abs_y()]); }

/* ROLA - Rotate accumulator A left without carry. */
void rola() { rol(&registers[REGISTER_A]); }

/* ROLB - Rotate accumulator B left without carry. */
void rolb() { rol(&registers[REGISTER_B]); }

/* RR - Rotate memory right without carry. */
void rr_abs() { rr(&memory[fetch_address_abs()]); }
void rr_abs_x() { rr(&memory[fetch_address_abs_x()]); }
void rr_abs_y() { rr(&memory[fetch_address_abs_y()]); }

/* RRA - Rotate accumulator A right without carry. */
void rra() { rr(&registers[REGISTER_A]); }

/* RRB - Rotate accumulator B right without carry. */
void rrb() { rr(&registers[REGISTER_B]); }

/* MOVE - Transfers from one register to another. */
void move_ab() { registers[REGISTER_A] = registers[REGISTER_B]; }
void move_al() { registers[REGISTER_A] = registers[REGISTER_L]; }
void move_ah() { registers[REGISTER_A] = registers[REGISTER_H]; }
void move_am() { registers[REGISTER_A] = registers[REGISTER_M]; }
void move_ba() { registers[REGISTER_B] = registers[REGISTER_A]; }
void move_bl() { registers[REGISTER_B] = registers[REGISTER_L]; }
void move_bh() { registers[REGISTER_B] = registers[REGISTER_H]; }
void move_bm() { registers[REGISTER_B] = registers[REGISTER_M]; }
void move_la() { registers[REGISTER_L] = registers[REGISTER_A]; }
void move_lb() { registers[REGISTER_L] = registers[REGISTER_B]; }
void move_lh() { registers[REGISTER_L] = registers[REGISTER_H]; }
void move_lm() { registers[REGISTER_L] = registers[REGISTER_M]; }
void move_ha() { registers[REGISTER_H] = registers[REGISTER_A]; }
void move_hb() { registers[REGISTER_H] = registers[REGISTER_B]; }
void move_hl() { registers[REGISTER_H] = registers[REGISTER_L]; }
void move_hm() { registers[REGISTER_H] = registers[REGISTER_M]; }
void move_ma() { registers[REGISTER_M] = registers[REGISTER_A]; }
void move_mb() { registers[REGISTER_M] = registers[REGISTER_B]; }
void move_ml() { registers[REGISTER_M] = registers[REGISTER_L]; }
void move_mh() { registers[REGISTER_M] = registers[REGISTER_H]; }

/* LDX - Loads memory into index register X. */
void ldx_imm() { index_registers[REGISTER_X] = fetch(); }
void ldx_abs() { index_registers[REGISTER_X] = memory[fetch_address_abs()]; }
void ldx_abs_x() { index_registers[REGISTER_X] = memory[fetch_address_abs_x()]; }
void ldx_abs_y() { index_registers[REGISTER_X] = memory[fetch_address_abs_y()]; }
void ldx_indir() { index_registers[REGISTER_X] = memory[fetch_address_indir()]; }
void ldx_indir_x() { index_registers[REGISTER_X] = memory[fetch_address_indir_x()]; }

/* STOX - Stores index register X into memory. */
void stox_abs() { memory[fetch_address_abs()] = index_registers[REGISTER_X]; }
void stox_abs_x() { memory[fetch_address_abs_x()] = index_registers[REGISTER_X]; }
void stox_abs_y() { memory[fetch_address_abs_y()] = index_registers[REGISTER_X]; }
void stox_indir() { memory[fetch_address_indir()] = index_registers[REGISTER_X]; }
void stox_indir_x() { memory[fetch_address_indir_x()] = index_registers[REGISTER_X]; }

/* DECX - Decrements index register X. */
void decx() { set_flag_z(--index_registers[REGISTER_X]); }

/* INCX - Increments index register X. */
void incx() { set_flag_z(++index_registers[REGISTER_X]); }

/* LDY - Loads memory into index register Y. */
void ldy_imm() { index_registers[REGISTER_Y] = fetch(); }
void ldy_abs() { index_registers[REGISTER_Y] = memory[fetch_address_abs()]; }
void ldy_abs_x() { index_registers[REGISTER_Y] = memory[fetch_address_abs_x()]; }
void ldy_abs_y() { index_registers[REGISTER_Y] = memory[fetch_address_abs_y()]; }
void ldy_indir() { index_registers[REGISTER_Y] = memory[fetch_address_indir()]; }
void ldy_indir_x() { index_registers[REGISTER_Y] = memory[fetch_address_indir_x()]; }

/* STOY - Stores index register Y into memory. */
void stoy_abs() { memory[fetch_address_abs()] = index_registers[REGISTER_Y]; }
void stoy_abs_x() { memory[fetch_address_abs_x()] = index_registers[REGISTER_Y]; }
void stoy_abs_y() { memory[fetch_address_abs_y()] = index_registers[REGISTER_Y]; }
void stoy_indir() { memory[fetch_address_indir()] = index_registers[REGISTER_Y]; }
void stoy_indir_x() { memory[fetch_address_indir_x()] = index_registers[REGISTER_Y]; }

/* CAY - Transfers accumulator A to index register Y. */
void cay() { set_flag_n(index_registers[REGISTER_Y] = registers[REGISTER_A]); }

/* MYA - Transfers index register Y to accumulator A. */
void mya() { registers[REGISTER_A] = index_registers[REGISTER_Y]; }

/* DECY - Decrements index register Y. */
void decy() { set_flag_z(--index_registers[REGISTER_Y]); }

/* INCY - Increments index register Y. */
void incy() { set_flag_z(++index_registers[REGISTER_Y]); }

/* LODS - Loads memory into stackpointer. */
void lods_imm() { stack_pointer = (fetch() << 8) + fetch(); }
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
void csa() { registers[REGISTER_A] = flags; }

/* PUSH - Pushes register onto the stack. */
void push_a() { memory[stack_pointer--] = registers[REGISTER_A]; }
void push_b() { memory[stack_pointer--] = registers[REGISTER_B]; }
void push_f() { memory[stack_pointer--] = registers[flags]; }
void push_l() { memory[stack_pointer--] = registers[REGISTER_L]; }
void push_h() { memory[stack_pointer--] = registers[REGISTER_H]; }

/* POP - Pop the top of the stack to the register. */
void pop_a() { registers[REGISTER_A] = memory[++stack_pointer]; }
void pop_b() { registers[REGISTER_B] = memory[++stack_pointer]; }
void pop_f() { registers[flags] = memory[++stack_pointer]; }
void pop_l() { registers[REGISTER_L] = memory[++stack_pointer]; }
void pop_h() { registers[REGISTER_H] = memory[++stack_pointer]; }

/* LX - Loads memory into register pair. */
void lx() {
  registers[REGISTER_H] = fetch();
  registers[REGISTER_L] = fetch();
}

/* JMP - Loads memory into program counter. */
void jmp() { program_counter = fetch_address_abs(); }

/* ABA - Adds accumulator B into accumulator A. */
void aba() { set_flags_znc(registers[REGISTER_A] += registers[REGISTER_B]); }

/* SBA - Subtracts accumulator B from accumulator A. */
void sba() { set_flags_znc(registers[REGISTER_A] -= registers[REGISTER_B]); }

/* AAB - Adds accumulator A into accumulator B. */
void aab() { set_flags_znc(registers[REGISTER_B] += registers[REGISTER_A]); }

/* SAB - Subtracts accumulator A from accumulator B. */
void sab() { set_flags_znc(registers[REGISTER_B] -= registers[REGISTER_A]); }

/* MVI - Loads memory into register. */
void mvi_l() { registers[REGISTER_L] = fetch(); }
void mvi_h() { registers[REGISTER_H] = fetch(); }

/* ADCP - Adds register pair into accumulator pair. */
void adcp() { adc(REGISTER_A, REGISTER_L); }

/* SBCP - Subtracts register pair into accumulator pair. */
void sbcp() { sbc(REGISTER_A, REGISTER_L); }

/* XCHG - Swaps the registers contents. */
void xchg() {
  BYTE data = registers[REGISTER_A];
  registers[REGISTER_A] = registers[REGISTER_L];
  registers[REGISTER_L] = data;
}

/* JSR - Jump subroutine. */
void jsr() {
  WORD address = fetch_address_abs();
  memory[stack_pointer--] = program_counter >> 8;
  memory[stack_pointer--] = program_counter;
  program_counter = address;
}

/* RET - Return from subroutine. */
void ret() {
  BYTE higher = memory[stack_pointer++];
  BYTE lower = memory[stack_pointer++];
  program_counter = join_address(higher, lower);
}

/* JCC - Jump on carry clear. */
void jcc() { jump(!is_flag_set(FLAG_C)); }

/* JCS - Jump on carry set. */
void jcs() { jump(is_flag_set(FLAG_C)); }

/* JNE - Jump on result not zero. */
void jne() { jump(!is_flag_set(FLAG_Z)); }

/* JEQ - Jump on result equal to zero. */
void jeq() { jump(is_flag_set(FLAG_Z)); }

/* JMI - Jump on negative result. */
void jmi() { jump(is_flag_set(FLAG_N)); }

/* JPL - Jump on positive result. */
void jpl() { jump(!is_flag_set(FLAG_N)); }

/* JHI - Jump on result same or lower. */
void jhi() { jump(is_flag_set(FLAG_C) || is_flag_set(FLAG_Z)); }

/* JLE - Jump on result higher. */
void jle() { jump(!is_flag_set(FLAG_C) && !is_flag_set(FLAG_Z)); }

/* CCC - Call on carry clear. */
void ccc() { call(!is_flag_set(FLAG_C)); }

/* CCS - Call on carry set. */
void ccs() { call(is_flag_set(FLAG_C)); }

/* CNE - Call on not zero. */
void cne() { call(!is_flag_set(FLAG_Z)); }

/* CEQ - Call on result equal to zero. */
void ceq() { call(is_flag_set(FLAG_Z)); }

/* CMI - Call on negative result. */
void cmi() { call(is_flag_set(FLAG_N)); }

/* CPL - Call on positive result. */
void cpl() { call(!is_flag_set(FLAG_N)); }

/* CHI - Call on result same or lower. */
void chi() { call(is_flag_set(FLAG_C) || is_flag_set(FLAG_Z)); }

/* CLE - Call on result higher. */
void cle() { call(!is_flag_set(FLAG_C) && !is_flag_set(FLAG_Z)); }

/* CLC - Clear carry flag. */
void clc() { flags &= ~FLAG_C; }

/* STC - Set carry flag. */
void stc() { flags |= FLAG_C; }

/* CLI - Clear interrupt flag. */
void cli() { flags &= ~FLAG_I; }

/* STI - Set interrupt flag. */
void sti() { flags |= FLAG_I; }

/* NOP - No operation. */
void nop() {}

/* HLT - Wait for interrupt. */
void hlt() { halt = true; }

/* SWI - Software interrupt. */
void swi() {
  flags |= FLAG_I;
  memory[stack_pointer--] = registers[REGISTER_A];
  memory[stack_pointer--] = registers[REGISTER_B];
  memory[stack_pointer--] = registers[flags];
  memory[stack_pointer--] = registers[REGISTER_L];
  memory[stack_pointer--] = registers[REGISTER_H];
}

/* RTI - Return from software interrupt. */
void rti() {
  registers[REGISTER_L] = memory[++stack_pointer];
  registers[REGISTER_H] = memory[++stack_pointer];
  registers[flags] = memory[++stack_pointer];
  registers[REGISTER_B] = memory[++stack_pointer];
  registers[REGISTER_A] = memory[++stack_pointer];
}

/* Ordered opcode implementations. */
void (*opcodes[])() = {
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
  xor_bl, bit_bl, stos_abs, nop, move_ba, move_la, move_ha, move_ma,
  lods_indir_x, adc_bh, sbc_bh, add_bh, sub_bh, cmp_bh, or_bh, and_bh,
  xor_bh, bit_bh, stos_abs_x, move_ab, nop, move_lb, move_hb, move_mb,
  nop, adc_bm, sbc_bm, add_bm, sub_bm, cmp_bm, or_bm, and_bm,
  xor_bm, bit_bm, stos_abs_y, move_al, move_bl, nop, move_hl, move_ml,
  nop, nop, nop, sbia, sbib, cpia, cpib, oria,
  orib, nop, stos_indir, move_ah, move_bh, move_lh, nop, move_mh,
  inc_abs, dec_abs, rrc_abs, rlc_abs, sal_abs, sar_abs, lsr_abs, com_abs,
  rol_abs, rr_abs, stos_indir_x, move_am, move_bm, move_lm, move_hm, nop,
  inc_abs_x, dec_abs_x, rrc_abs_x, rlc_abs_x, sal_abs_x, sar_abs_x, lsr_abs_x, com_abs_x,
  rol_abs_x, rr_abs_x, stora_abs, storb_abs, stox_abs, stoy_abs, push_a, pop_a,
  inc_abs_y, dec_abs_y, rrc_abs_y, rlc_abs_y, sal_abs_y, sar_abs_y, lsr_abs_y, com_abs_y,
  rol_abs_y, rr_abs_y, stora_abs_x, storb_abs_x, stox_abs_x, stoy_abs_x, push_b, pop_b,
  inca, deca, rrca, rlca, sala, sara, lsra, coma,
  rola, rra, stora_abs_y, storb_abs_y, stox_abs_y, stoy_abs_y, push_f, pop_f,
  incb, decb, rrcb, rlcb, salb, sarb, lsrb, comb,
  rolb, rrb, stora_indir, storb_indir, stox_indir, stoy_indir, push_l, pop_l,
  cay, mya, csa, aba, sba, aab, sab, adcp,
  sbcp, xchg, stora_indir_x, storb_indir_x, stox_indir_x, stoy_indir_x, push_h, pop_h
};

/**
 * Emulates the program loaded into memory.
 */
void emulate() {
  // Initialise the program state.
  halt = false;

  // Execute each opcode recieved until program executes halt.
  while (!halt) {
    opcodes[fetch()]();
  }
}

////////////////////////////////////////////////////////////////////////////////
//                            Simulator/Emulator (End)                        //
////////////////////////////////////////////////////////////////////////////////

void initialise_filenames() {
  for (int i = 0; i < MAX_FILENAME_SIZE; i++) {
    hex_file[i] = '\0';
    trc_file[i] = '\0';
  }
}

int find_dot_position(char *filename) {
  int dot_position = 0;
  int i = 0;
  char chr = filename[i];

  while (chr != '\0') {
    if (chr == '.')
      dot_position = i;

    i++;
    chr = filename[i];
  }

  return dot_position;
}

int find_end_position(char *filename) {
  int end_position = 0;
  int i = 0;
  int chr = filename[i];

  while (chr != '\0') {
    end_position = i;
    i++;
    chr = filename[i];
  }

  return end_position;
}

bool file_exists(char *filename) {
  FILE *ifp;
  bool exists = false;

  if ((ifp = fopen(filename, "r")) != NULL) {
    exists = true;
    fclose(ifp);
  }

  return exists;
}

void create_file(char *filename) {
  FILE *ofp;

  if ((ofp = fopen(filename, "w")) != NULL)
    fclose(ofp);
}

bool get_line(FILE *fp, char *buffer) {
  bool rc = false;
  bool collect = true;
  char c;
  int i = 0;

  while (collect) {
    c = getc(fp);

    switch (c) {
      case EOF:
        if (i > 0)
          rc = true;
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

  return rc;
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

      if (chr == '\n' || ln < MAX_FILENAME_SIZE)
        continue;

      hex_file[ln] = chr;
      trc_file[ln] = chr;
      ln++;
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

  if (!file_exists(hex_file)) {
    printf("\n");
    printf("ERROR> Input file %s does not exist!\n", hex_file);
    printf("\n");
  }

  // Clear registers and Memory
  registers[REGISTER_A] = 0;
  registers[REGISTER_B] = 0;
  registers[REGISTER_L] = 0;
  registers[REGISTER_H] = 0;
  index_registers[REGISTER_X] = 0;
  index_registers[REGISTER_Y] = 0;
  flags = 0;
  program_counter = 0;
  stack_pointer = 0;

  for (i = 0; i < MEMORY_SIZE; i++) {
    memory[i] = 0x00;
  }

  // Load hex file

  if ((ifp = fopen( hex_file, "r" )) != NULL) {
    printf("Loading file...\n\n");

    load_at = 0;

    while (get_line(ifp, input_buffer)) {
      if (sscanf(input_buffer, "L=%x", &address) == 1) {
        load_at = address;
      } else if (sscanf(input_buffer, "%x", &code) == 1) {
        if ((load_at >= 0) && (load_at <= MEMORY_SIZE)) {
          memory[load_at] = (BYTE)code;
        }

        load_at++;
      } else {
        printf("ERROR> Failed to load instruction: %s \n", input_buffer);
      }
    }

    fclose(ifp);
  }

  // Emulate
  emulate();
}

void building(int args, char **argv){
  char buffer[1024];
  load_and_run(args, argv);
  sprintf(buffer, "0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X",
    memory[TEST_ADDRESS_1],
    memory[TEST_ADDRESS_2],
    memory[TEST_ADDRESS_3],
    memory[TEST_ADDRESS_4],
    memory[TEST_ADDRESS_5],
    memory[TEST_ADDRESS_6],
    memory[TEST_ADDRESS_7],
    memory[TEST_ADDRESS_8],
    memory[TEST_ADDRESS_9],
    memory[TEST_ADDRESS_10],
    memory[TEST_ADDRESS_11],
    memory[TEST_ADDRESS_12]
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

  while (!testing_complete) {
    memset(buffer, '\0', sizeof(buffer));

    if (recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (SOCKADDR *)&client_addr, &len) == SOCKET_ERROR) {
      continue;
    }
    // printf("Incoming Data: %s \n", buffer);
    // printf("%d - Incoming Data, %s \n", test++, buffer);

    //if (strcmp(buffer, "Testing complete") == 1)
    if (sscanf(buffer, "Testing complete %d", &mark) == 1) {
      testing_complete = true;
      printf("Current mark = %d\n", mark);
      continue;
    } else if (sscanf(buffer, "Tests passed %d", &passed) == 1) {
      //testing_complete = true;
      printf("Passed = %d\n", passed);
      continue;
    } else if (strcmp(buffer, "Error") == 0) {
      printf("ERROR> Testing abnormally terminated\n");
      testing_complete = true;
      continue;
    }

    // Clear registers and Memory
    memset(registers, 0 sizeof(registers));
    registers[REGISTER_A] = 0;
    registers[REGISTER_B] = 0;
    registers[REGISTER_L] = 0;
    registers[REGISTER_H] = 0;
    index_registers[REGISTER_X] = 0;
    index_registers[REGISTER_Y] = 0;
    flags = 0;
    program_counter = 0;
    stack_pointer = 0;

    for (i = 0; i < MEMORY_SIZE; i++) {
      memory[i] = 0;
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

          if (sscanf(input_buffer, "L=%x", &address) == 1) {
            load_at = address;
          } else if (sscanf(input_buffer, "%x", &code) == 1) {
            if ((load_at >= 0) && (load_at <= MEMORY_SIZE)) {
              memory[load_at] = (BYTE)code;
              fprintf(ofp, "%02X\n", (BYTE)code);
            }

            load_at++;
          } else {
            printf("ERROR> Failed to load instruction: %s \n", input_buffer);
          }

          j = 0;
          break;

        default:
          input_buffer[j] = chr;
          j++;
          break;
      }

      i++;
    }

    fclose(ofp);

    if (load_at <= 1)
      continue;

    // Emulate
    emulate();

    // Send and store results
    sprintf(buffer, "%02X%02X %02X%02X %02X%02X %02X%02X %02X%02X %02X%02X",
      memory[TEST_ADDRESS_1],
      memory[TEST_ADDRESS_2],
      memory[TEST_ADDRESS_3],
      memory[TEST_ADDRESS_4],
      memory[TEST_ADDRESS_5],
      memory[TEST_ADDRESS_6],
      memory[TEST_ADDRESS_7],
      memory[TEST_ADDRESS_8],
      memory[TEST_ADDRESS_9],
      memory[TEST_ADDRESS_10],
      memory[TEST_ADDRESS_11],
      memory[TEST_ADDRESS_12]
    );

    sendto(sock, buffer, strlen(buffer), 0, (SOCKADDR *)&server_addr, sizeof(SOCKADDR));
  }
}

int main(int argc, char *argv[]) {
  printf("\n");
  printf("Microprocessor Emulator\n");
  printf("UWE Computer and Network Systems Assignment 1\n");
  printf("\n");

  initialise_filenames();

  if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
    return 0;

  // Here we create our socket, which will be a UDP socket (SOCK_DGRAM).
  sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  if (!sock) {
    printf("Socket creation failed!\n");
    return 0;
  }

  memset(&server_addr, 0, sizeof(SOCKADDR_IN));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS_SERVER);
  server_addr.sin_port = htons(PORT_SERVER);

  memset(&client_addr, 0, sizeof(SOCKADDR_IN));
  client_addr.sin_family = AF_INET;
  client_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  client_addr.sin_port = htons(PORT_CLIENT);

  if (argc == 2) {
    building(argc, argv);
    closesocket(sock);
    WSACleanup();
    return 0;
  }

  char input = '\0';

  while (input != 'e' && input != 'E') {
    printf("\n");
    printf("Please select option\n");
    printf("L - Load and run a hex file\n");
    printf("T - Have the server test and mark your emulator\n");
    printf("E - Exit\n");
    printf("Enter option: ");

    input = getchar();

    if (input == 0x0A)
      input = getchar();

    switch (input) {
      case 'L':
      case 'l':
        load_and_run(argc, argv);
        break;

      case 'T':
      case 't':
        test_and_mark();
        break;
    }
  }

  closesocket(sock);
  WSACleanup();
  return 0;
}
