#include "arithmetic.h"

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

/* ADC - Adds register to accumulator with carry. */
void adc_al() { adc(REGISTER_A, REGISTER_L); }
void adc_ah() { adc(REGISTER_A, REGISTER_H); }
void adc_am() { adc(REGISTER_A, REGISTER_M); }
void adc_bl() { adc(REGISTER_B, REGISTER_L); }
void adc_bh() { adc(REGISTER_B, REGISTER_H); }
void adc_bm() { adc(REGISTER_B, REGISTER_M); }

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

/* SBC - Subtracts register from accumulator with carry. */
void sbc_al() { sbc(REGISTER_A, REGISTER_L); }
void sbc_ah() { sbc(REGISTER_A, REGISTER_H); }
void sbc_am() { sbc(REGISTER_A, REGISTER_M); }
void sbc_bl() { sbc(REGISTER_B, REGISTER_L); }
void sbc_bh() { sbc(REGISTER_B, REGISTER_H); }
void sbc_bm() { sbc(REGISTER_B, REGISTER_M); }

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

/* ADD - Adds register to accumulator. */
void add_al() { add(REGISTER_A, REGISTER_L); }
void add_ah() { add(REGISTER_A, REGISTER_H); }
void add_am() { add(REGISTER_A, REGISTER_M); }
void add_bl() { add(REGISTER_B, REGISTER_L); }
void add_bh() { add(REGISTER_B, REGISTER_H); }
void add_bm() { add(REGISTER_B, REGISTER_M); }

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

/* SUB - Subtracts register from accumulator. */
void sub_al() { sub(REGISTER_A, REGISTER_L); }
void sub_ah() { sub(REGISTER_A, REGISTER_H); }
void sub_am() { sub(REGISTER_A, REGISTER_M); }
void sub_bl() { sub(REGISTER_B, REGISTER_L); }
void sub_bh() { sub(REGISTER_B, REGISTER_H); }
void sub_bm() { sub(REGISTER_B, REGISTER_M); }

/**
 * Register compared to accumulator.
 *
 * @param accumulator the accumulator to compare.
 * @param reg         the register to compare.
 */
void cmp(int accumulator, int reg) {
  set_flags_znc(registers[accumulator] - registers[reg]);
}

/* CMP - Compares register to accumulator. */
void cmp_al() { cmp(REGISTER_A, REGISTER_L); }
void cmp_ah() { cmp(REGISTER_A, REGISTER_H); }
void cmp_am() { cmp(REGISTER_A, REGISTER_M); }
void cmp_bl() { cmp(REGISTER_B, REGISTER_L); }
void cmp_bh() { cmp(REGISTER_B, REGISTER_H); }
void cmp_bm() { cmp(REGISTER_B, REGISTER_M); }

/**
 * Register bitwise inclusive or with accumulator.
 *
 * @param accumulator the accumulator.
 * @param reg         the register.
 */
void _or(int accumulator, int reg) {
  set_flags_zn(registers[accumulator] |= registers[reg]);
}

/* OR - Register bitwise inclusive or with accumulator. */
void or_al() { _or(REGISTER_A, REGISTER_L); }
void or_ah() { _or(REGISTER_A, REGISTER_H); }
void or_am() { _or(REGISTER_A, REGISTER_M); }
void or_bl() { _or(REGISTER_B, REGISTER_L); }
void or_bh() { _or(REGISTER_B, REGISTER_H); }
void or_bm() { _or(REGISTER_B, REGISTER_M); }

/**
 * Register bitwise and with accumulator.
 *
 * @param accumulator the accumulator.
 * @param reg         the register.
 */
void _and(int accumulator, int reg) {
  set_flags_zn(registers[accumulator] &= registers[reg]);
}

/* AND - Register bitwise and with accumulator */
void and_al() { _and(REGISTER_A, REGISTER_L); }
void and_ah() { _and(REGISTER_A, REGISTER_H); }
void and_am() { _and(REGISTER_A, REGISTER_M); }
void and_bl() { _and(REGISTER_B, REGISTER_L); }
void and_bh() { _and(REGISTER_B, REGISTER_H); }
void and_bm() { _and(REGISTER_B, REGISTER_M); }

/**
 * Register bitwise exclusive or with accumulator.
 *
 * @param accumulator the accumulator.
 * @param reg         the register.
 */
void _xor(int accumulator, int reg) {
  set_flags_zn(registers[accumulator] ^= registers[reg]);
}

/* XOR - Register bitwise exclusive or with accumulator. */
void xor_al() { _xor(REGISTER_A, REGISTER_L); }
void xor_ah() { _xor(REGISTER_A, REGISTER_H); }
void xor_am() { _xor(REGISTER_A, REGISTER_M); }
void xor_bl() { _xor(REGISTER_B, REGISTER_L); }
void xor_bh() { _xor(REGISTER_B, REGISTER_H); }
void xor_bm() { _xor(REGISTER_B, REGISTER_M); }

/**
 * Register bit tested with with accumulator.
 *
 * @param accumulator the accumulator.
 * @param reg         the register.
 */
void _bit(int accumulator, int reg) {
  set_flags_zn(registers[accumulator] & registers[reg]);
}

/* BIT - Register bit tested with accumulator. */
void bit_al() { _bit(REGISTER_A, REGISTER_L); }
void bit_ah() { _bit(REGISTER_A, REGISTER_H); }
void bit_am() { _bit(REGISTER_A, REGISTER_M); }
void bit_bl() { _bit(REGISTER_B, REGISTER_L); }
void bit_bh() { _bit(REGISTER_B, REGISTER_H); }
void bit_bm() { _bit(REGISTER_B, REGISTER_M); }

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

/* SBIA - Data subtracted from accumulator A with carry. */
void sbia() { sbi(REGISTER_A); }

/* SBIB - Data subtracted from accumulator B with carry. */
void sbib() { sbi(REGISTER_B); }

/* CPIA - Data compared to accumulator A. */
void cpia() { set_flags_znc(fetch() - registers[REGISTER_A]); }

/* CPIB - Data compared to accumulator B. */
void cpib() { set_flags_znc(fetch() - registers[REGISTER_B]); }

/**
 * Data bitwise inclusive or with accumulator.
 *
 * @param accumulator the accumulator to use.
 */
void ori(BYTE accumulator) {
  set_flags_zn(registers[accumulator] |= fetch());
}

/* ORIA - Data bitwise inclusive or with accumulator A. */
void oria() { ori(REGISTER_A); }

/* ORIB - Data bitwise inclusive or with accumulator B. */
void orib() { ori(REGISTER_B); }

/**
 * Increment memory.
 *
 * @param address the memory address.
 */
void inc(WORD address) {
  set_flags_zn(++memory[address]);
}

/* INC - Increment memory. */
void inc_abs() { inc(fetch_address_abs()); }
void inc_abs_x() { inc(fetch_address_abs_x()); }
void inc_abs_y() { inc(fetch_address_abs_y()); }

/* INCA - Increment accumulator A. */
void inca() { set_flags_zn(++registers[REGISTER_A]); }

/* INCB - Increment accumulator B. */
void incb() { set_flags_zn(++registers[REGISTER_B]); }

/**
 * Decrement memory.
 *
 * @param address the memory address.
 */
void dec(WORD address) {
  set_flags_zn(--memory[address]);
}

/* DEC - Decrement memory. */
void dec_abs() { dec(fetch_address_abs()); }
void dec_abs_x() { dec(fetch_address_abs_x()); }
void dec_abs_y() { dec(fetch_address_abs_y()); }

/* DECA - Decrement accumulator A. */
void deca() { set_flags_zn(--registers[REGISTER_A]); }

/* DECB - Decrement accumulator B. */
void decb() { set_flags_zn(--registers[REGISTER_B]); }

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

/* RRC - Rotate right through carry memory. */
void rrc_abs() { rrc(&memory[fetch_address_abs()]); }
void rrc_abs_x() { rrc(&memory[fetch_address_abs_x()]); }
void rrc_abs_y() { rrc(&memory[fetch_address_abs_y()]); }

/* RRCA - Rotate right through carry accumulator A. */
void rrca() { rrc(&registers[REGISTER_A]); }

/* RRCB - Rotate right through carry accumulator B. */
void rrcb() { rrc(&registers[REGISTER_B]); }

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

/* RLC - Rotate left through carry memory. */
void rlc_abs() { rlc(&memory[fetch_address_abs()]); }
void rlc_abs_x() { rlc(&memory[fetch_address_abs_x()]); }
void rlc_abs_y() { rlc(&memory[fetch_address_abs_y()]); }

/* RLCA - Rotate left through carry accumulator A. */
void rlca() { rlc(&registers[REGISTER_A]); }

/* RLCB - Rotate left through carry accumulator B. */
void rlcb() { rlc(&registers[REGISTER_B]); }

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

/* SAL - Arithmetic shift left memory. */
void sal_abs() { sal(&memory[fetch_address_abs()]); }
void sal_abs_x() { sal(&memory[fetch_address_abs_x()]); }
void sal_abs_y() { sal(&memory[fetch_address_abs_y()]); }

/* SALA - Arithmetic shift left accumulator A. */
void sala() { sal(&registers[REGISTER_A]); }

/* SALB - Arithmetic shift left accumulator B. */
void salb() { sal(&registers[REGISTER_B]); }

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

/* SAR - Arithmetic shift right memory. */
void sar_abs() { sar(&memory[fetch_address_abs()]); }
void sar_abs_x() { sar(&memory[fetch_address_abs_x()]); }
void sar_abs_y() { sar(&memory[fetch_address_abs_y()]); }

/* SARA - Arithmetic shift right accumulator A. */
void sara() { sar(&registers[REGISTER_A]); }

/* SARB - Arithmetic shift right accumulator B. */
void sarb() { sar(&registers[REGISTER_B]); }

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

/* LSR - Shift right memory. */
void lsr_abs() { lsr(&memory[fetch_address_abs()]); }
void lsr_abs_x() { lsr(&memory[fetch_address_abs_x()]); }
void lsr_abs_y() { lsr(&memory[fetch_address_abs_y()]); }

/* LSRA - Shift right accumulator A. */
void lsra() { lsr(&registers[REGISTER_A]); }

/* LSRB - Shift right accumulator B. */
void lsrb() { lsr(&registers[REGISTER_B]); }

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

/* COM - Negate memory. */
void com_abs() { com(&memory[fetch_address_abs()]); }
void com_abs_x() { com(&memory[fetch_address_abs_x()]); }
void com_abs_y() { com(&memory[fetch_address_abs_y()]); }

/* COMA - Negate accumulator A. */
void coma() { com(&registers[REGISTER_A]); }

/* COMB - Negate accumulator B. */
void comb() { com(&registers[REGISTER_B]); }

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

/* ROL - Rotate memory left without carry. */
void rol_abs() { rol(&memory[fetch_address_abs()]); }
void rol_abs_x() { rol(&memory[fetch_address_abs_x()]); }
void rol_abs_y() { rol(&memory[fetch_address_abs_y()]); }

/* ROLA - Rotate accumulator A left without carry. */
void rola() { rol(&registers[REGISTER_A]); }

/* ROLB - Rotate accumulator B left without carry. */
void rolb() { rol(&registers[REGISTER_B]); }

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

/* RR - Rotate memory right without carry. */
void rr_abs() { rr(&memory[fetch_address_abs()]); }
void rr_abs_x() { rr(&memory[fetch_address_abs_x()]); }
void rr_abs_y() { rr(&memory[fetch_address_abs_y()]); }

/* RRA - Rotate accumulator A right without carry. */
void rra() { rr(&registers[REGISTER_A]); }

/* RRB - Rotate accumulator B right without carry. */
void rrb() { rr(&registers[REGISTER_B]); }

/* DECX - Decrements index register X. */
void decx() { set_flag_z(--index_registers[REGISTER_X]); }

/* INCX - Increments index register X. */
void incx() { set_flag_z(++index_registers[REGISTER_X]); }

/* CAY - Transfers accumulator A to index register Y. */
void cay() { set_flag_n(index_registers[REGISTER_Y] = registers[REGISTER_A]); }

/* MYA - Transfers index register Y to accumulator A. */
void mya() { registers[REGISTER_A] = index_registers[REGISTER_Y]; }

/* DECY - Decrements index register Y. */
void decy() { set_flag_z(--index_registers[REGISTER_Y]); }

/* INCY - Increments index register Y. */
void incy() { set_flag_z(++index_registers[REGISTER_Y]); }

/* ABA - Adds accumulator B into accumulator A. */
void aba() { set_flags_znc(registers[REGISTER_A] += registers[REGISTER_B]); }

/* SBA - Subtracts accumulator B from accumulator A. */
void sba() { set_flags_znc(registers[REGISTER_A] -= registers[REGISTER_B]); }

/* AAB - Adds accumulator A into accumulator B. */
void aab() { set_flags_znc(registers[REGISTER_B] += registers[REGISTER_A]); }

/* SAB - Subtracts accumulator A from accumulator B. */
void sab() { set_flags_znc(registers[REGISTER_B] -= registers[REGISTER_A]); }

/* ADCP - Adds register pair into accumulator pair. */
void adcp() { adc(REGISTER_A, REGISTER_L); }

/* SBCP - Subtracts register pair into accumulator pair. */
void sbcp() { sbc(REGISTER_A, REGISTER_L); }
