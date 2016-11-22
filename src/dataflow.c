/**
 * @file dataflow.c
 * @author William Randall (16002374)
 * @date 22nd November 2016
 * @brief all dataflow opcode implementations.
 */

#include "dataflow.h"

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

/* JMP - Loads memory into program counter. */
void jmp() { program_counter = fetch_address_abs(); }

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
