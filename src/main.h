/**
 * @file main.h
 * @author William Randall (16002374)
 * @date 22nd November 2016
 * @brief Complete 60 marks emulator for CaNS assignment 1 at UWE.
 */

#ifndef MAIN_H
#define MAIN_H

#pragma comment(lib, "wsock32.lib")

typedef enum { false, true } bool;

#define WORD unsigned short
#define BYTE unsigned char

#include "arithmetic.h"
#include "dataflow.h"
#include "move.h"
#include "transput.h"

#define STUDENT_NUMBER "16002374"
#define IP_ADDRESS_SERVER "127.0.0.1"
#define PORT_SERVER 0x1984
#define PORT_CLIENT 0x1985
#define MAX_FILENAME_SIZE 500
#define MAX_BUFFER_SIZE 500

///////////////
// Registers //
///////////////

#define FLAG_I 0x10
#define FLAG_Z 0x04
#define FLAG_N 0x02
#define FLAG_C 0x01
extern BYTE flags;

#define REGISTER_M 4
#define REGISTER_A 3
#define REGISTER_B 2
#define REGISTER_H 1
#define REGISTER_L 0
extern BYTE registers[5];

#define REGISTER_X 0
#define REGISTER_Y 1
extern BYTE index_registers[2];

extern WORD program_counter;
extern WORD stack_pointer;

////////////
// Memory //
////////////

#define MEMORY_SIZE 65536

extern BYTE memory[MEMORY_SIZE];

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

extern bool halt;

///////////////
// Functions //
///////////////

bool is_flag_set(BYTE flag);
void set_flag(bool state, BYTE flag);
void set_flag_z(BYTE data);
void set_flag_n(BYTE data);
void set_flag_c(WORD data);
void set_flags_zn(BYTE data);
void set_flags_znc(WORD data);

BYTE fetch();

WORD join_address(BYTE higher, BYTE lower);

WORD fetch_address_abs();
WORD fetch_address_abs_x();
WORD fetch_address_abs_y();
WORD fetch_address_indir();
WORD fetch_address_indir_x();

#endif /* End of include guard: MAIN_H */
