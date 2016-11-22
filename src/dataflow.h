/**
 * @file dataflow.h
 * @author William Randall (16002374)
 * @date 22nd November 2016
 * @brief all dataflow opcode function prototypes.
 */

#include "main.h"

// Pushes register onto the stack.
void push_a(); // 0x6A
void push_b(); // 0x7A
void push_f(); // 0x8A
void push_l(); // 0x9A
void push_h(); // 0xAA

// Pop the top of the stack into the register.
void pop_a(); // 0xBF
void pop_b(); // 0xCF
void pop_f(); // 0xDF
void pop_l(); // 0xEF
void pop_h(); // 0xFF

// Loads memory into program counter.
void jmp(); // 0x10

// Jump to subroutine.
void jsr(); // 0x21

// Return from subroutine.
void ret(); // 0x4C

// Jump on specific condition.
void jump(bool condition);
void jcc(); // 0x11 - Carry clear
void jcs(); // 0x12 - Carry set
void jne(); // 0x13 - Zero clear
void jeq(); // 0x14 - Zero set
void jmi(); // 0x15 - Negative set
void jpl(); // 0x16 - Negative clear
void jhi(); // 0x17 - Carry and zero set
void jle(); // 0x18 - Carry and zero clear

// Call on specific condition.
void call(bool condition);
void ccc(); // 0x22 - Carry clear
void ccs(); // 0x23 - Carry set
void cne(); // 0x24 - Zero clear
void ceq(); // 0x25 - Zero set
void cmi(); // 0x26 - Negative set
void cpl(); // 0x27 - Negative clear
void chi(); // 0x28 - Carry and zero set
void cle(); // 0x29 - Carry and zero clear

// Clear carry flag.
void clc(); // 0x05

// Set carry flag.
void stc(); // 0x08

// Clear interrupt flag.
void cli(); // 0x07

// Set interrupt flag.
void sti(); // 0x08

// No operation.
void nop(); // 0x2C

// Wait for interrupt.
void hlt(); // 0x2D

// Software interrupt.
void swi(); // 0x5C

// Return from software interrupt.
void rti(); // 0x5D
