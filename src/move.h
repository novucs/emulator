/**
 * @file move.h
 * @author William Randall (16002374)
 * @date 22nd November 2016
 * @brief all move opcode function prototypes.
 */

#include "main.h"

// Transfer from one register to another.
void move_ab(); // 0x7B
void move_al(); // 0x8B
void move_ah(); // 0x9B
void move_am(); // 0xAB
void move_ba(); // 0x6C
void move_bl(); // 0x8C
void move_bh(); // 0x9C
void move_bm(); // 0xAC
void move_la(); // 0x6D
void move_lb(); // 0x7D
void move_lh(); // 0x9D
void move_lm(); // 0xAD
void move_ha(); // 0x6E
void move_hb(); // 0x7E
void move_hl(); // 0x8E
void move_hm(); // 0xAE
void move_ma(); // 0x6F
void move_mb(); // 0x7F
void move_ml(); // 0x8F
void move_mh(); // 0x9F
