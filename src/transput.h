/**
 * @file transput.h
 * @author William Randall (16002374)
 * @date 22nd November 2016
 * @brief all transput opcode function prototypes.
 */

#include "main.h"

// Loads memory into accumulator A.
void ldaa_imm();     // 0x0A
void ldaa_abs();     // 0x1A
void ldaa_abs_x();   // 0x2A
void ldaa_abs_y();   // 0x3A
void ldaa_indir();   // 0x4A
void ldaa_indir_x(); // 0x5A

// Loads memory into accumulator B.
void ldab_imm();     // 0x0B
void ldab_abs();     // 0x1B
void ldab_abs_x();   // 0x2B
void ldab_abs_y();   // 0x3B
void ldab_indir();   // 0x4B
void ldab_indir_x(); // 0x5B

// Stores accumulator A into memory.
void stora_abs();     // 0xBA
void stora_abs_x();   // 0xCA
void stora_abs_y();   // 0xDA
void stora_indir();   // 0xEA
void stora_indir_x(); // 0xFA

// Stores accumulator B into memory.
void storb_abs();     // 0xBB
void storb_abs_x();   // 0xCB
void storb_abs_y();   // 0xDB
void storb_indir();   // 0xEB
void storb_indir_x(); // 0xFB

// Loads memory into register X.
void ldx_imm();     // 0x0E
void ldx_abs();     // 0x1E
void ldx_abs_x();   // 0x2E
void ldx_abs_y();   // 0x3E
void ldx_indir();   // 0x4E
void ldx_indir_x(); // 0x5E

// Stores register X into memory.
void stox_abs();     // 0xBC
void stox_abs_x();   // 0xCC
void stox_abs_y();   // 0xDC
void stox_indir();   // 0xEC
void stox_indir_x(); // 0xFC

// Loads memory into register Y.
void ldy_imm();     // 0x0F
void ldy_abs();     // 0x1F
void ldy_abs_x();   // 0x2F
void ldy_abs_y();   // 0x3F
void ldy_indir();   // 0x4F
void ldy_indir_x(); // 0x5F

// Stores register Y into memory.
void stoy_abs();     // 0xBD
void stoy_abs_x();   // 0xCD
void stoy_abs_y();   // 0xDD
void stoy_indir();   // 0xED
void stoy_indir_x(); // 0xFD

// Loads memory into stackpointer.
void load_stackpointer(WORD address);
void lods_imm();     // 0x20
void lods_abs();     // 0x30
void lods_abs_x();   // 0x40
void lods_abs_y();   // 0x50
void lods_indir();   // 0x60
void lods_indir_x(); // 0x70

// Stores stackpointer into memory.
void save_stackpointer(WORD address);
void stos_abs();     // 0x6A
void stos_abs_x();   // 0x7A
void stos_abs_y();   // 0x8A
void stos_indir();   // 0x9A
void stos_indir_x(); // 0xAA

// Transfers status register into accumulator.
void csa(); // 0xF2

// Loads memory into register pair.
void lx(); // 0x0C + 0x0D

// Loads memory into register.
void mvi_l(); // 0x1C
void mvi_h(); // 0x1D

// Swaps the contents of registers A and L.
void xchg(); // 0xF9
