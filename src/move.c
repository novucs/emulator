/**
 * @file move.c
 * @author William Randall (16002374)
 * @date 22nd November 2016
 * @brief all move opcode implementations.
 */

#include "move.h"

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
