#include "transput.h"

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


/**
 * Loads stackpointer from memory.
 *
 * @param id method for retrieving address.
 */
void load_stackpointer(WORD address) {
  stack_pointer = (memory[address] << 8) + memory[address + 1];
}

/* LODS - Loads stackpointer from memory. */
void lods_imm() { stack_pointer = (fetch() << 8) + fetch(); }
void lods_abs() { load_stackpointer(fetch_address_abs()); }
void lods_abs_x() { load_stackpointer(fetch_address_abs_x()); }
void lods_abs_y() { load_stackpointer(fetch_address_abs_y()); }
void lods_indir() { load_stackpointer(fetch_address_indir()); }
void lods_indir_x() { load_stackpointer(fetch_address_indir_x()); }

/**
 * Saves stackpointer into memory.
 *
 * @param id method for retrieving address.
 */
void save_stackpointer(WORD address) {
  memory[address] = stack_pointer >> 8;
  memory[address + 1] = stack_pointer;
}

/* STOS - Saves stackpointer into memory. */
void stos_abs() { save_stackpointer(fetch_address_abs()); }
void stos_abs_x() { save_stackpointer(fetch_address_abs_x()); }
void stos_abs_y() { save_stackpointer(fetch_address_abs_y()); }
void stos_indir() { save_stackpointer(fetch_address_indir()); }
void stos_indir_x() { save_stackpointer(fetch_address_indir_x()); }

/* CSA - Transfers status register to accumulator A. */
void csa() { registers[REGISTER_A] = flags; }

/* LX - Loads memory into register pair. */
void lx() {
  registers[REGISTER_H] = fetch();
  registers[REGISTER_L] = fetch();
}

/* MVI - Loads memory into register. */
void mvi_l() { registers[REGISTER_L] = fetch(); }
void mvi_h() { registers[REGISTER_H] = fetch(); }

/* XCHG - Swaps the registers contents. */
void xchg() {
  BYTE data = registers[REGISTER_A];
  registers[REGISTER_A] = registers[REGISTER_L];
  registers[REGISTER_L] = data;
}
