/**
 * @file arithmetic.h
 * @author William Randall (16002374)
 * @date 22nd November 2016
 * @brief all arithmetic opcode function prototypes.
 */

#include "main.h"

// Register added to accumulator with carry.
void adc(int accumulator, int reg);
void adc_al(); // 0x31
void adc_ah(); // 0x41
void adc_am(); // 0x51
void adc_bl(); // 0x61
void adc_bh(); // 0x71
void adc_bm(); // 0x81

// Register subtracted to accumulator with carry.
void sbc(int accumulator, int reg);
void sbc_al(); // 0x32
void sbc_ah(); // 0x42
void sbc_am(); // 0x52
void sbc_bl(); // 0x62
void sbc_bh(); // 0x72
void sbc_bm(); // 0x82

// Register added to accumulator.
void add(int accumulator, int reg);
void add_al(); // 0x33
void add_ah(); // 0x43
void add_am(); // 0x53
void add_bl(); // 0x63
void add_bh(); // 0x73
void add_bm(); // 0x83

// Register subtracted to accumulator.
void sub(int accumulator, int reg);
void sub_al(); // 0x34
void sub_ah(); // 0x44
void sub_am(); // 0x54
void sub_bl(); // 0x64
void sub_bh(); // 0x74
void sub_bm(); // 0x84

// Register compared to accumulator.
void cmp(int accumulator, int reg);
void cmp_al(); // 0x35
void cmp_ah(); // 0x45
void cmp_am(); // 0x55
void cmp_bl(); // 0x65
void cmp_bh(); // 0x75
void cmp_bm(); // 0x85

// Register bitwise inclusive or with accumulator.
void _or(int accumulator, int reg);
void or_al(); // 0x36
void or_ah(); // 0x46
void or_am(); // 0x56
void or_bl(); // 0x66
void or_bh(); // 0x76
void or_bm(); // 0x86

// Register bitwise and with accumulator.
void _and(int accumulator, int reg);
void and_al(); // 0x37
void and_ah(); // 0x47
void and_am(); // 0x57
void and_bl(); // 0x67
void and_bh(); // 0x77
void and_bm(); // 0x87

// Register bitwise exclusive or with accumulator.
void _xor(int accumulator, int reg);
void xor_al(); // 0x38
void xor_ah(); // 0x48
void xor_am(); // 0x58
void xor_bl(); // 0x68
void xor_bh(); // 0x78
void xor_bm(); // 0x88

// Register bit tested with accumulator.
void _bit(int accumulator, int reg);
void bit_al(); // 0x39
void bit_ah(); // 0x49
void bit_am(); // 0x59
void bit_bl(); // 0x69
void bit_bh(); // 0x79
void bit_bm(); // 0x89

// Data subtracted to accumulator with carry.
void sbi(BYTE accumulator);
void sbia(); // 0x93
void sbib(); // 0x94

// Data compared to accumulator.
void cpia(); // 0x95
void cpib(); // 0x96

// Data bitwise inclusive or with accumulator.
void ori(BYTE accumulator);
void oria(); // 0x97
void orib(); // 0x98

// Increment memory or accumulator.
void inc(WORD address);
void inc_abs();   // 0xA0
void inc_abs_x(); // 0xB0
void inc_abs_y(); // 0xC0
void inca();      // 0xD0
void incb();      // 0xE0

// Decrement memory or accumulator.
void dec(WORD address);
void dec_abs();   // 0xA1
void dec_abs_x(); // 0xB1
void dec_abs_y(); // 0xC1
void deca();      // 0xD1
void decb();      // 0xE1

// Rotate right through carry memory or accumulator.
void rrc(BYTE *data);
void rrc_abs();   // 0xA2
void rrc_abs_x(); // 0xB2
void rrc_abs_y(); // 0xC2
void rrca();      // 0xD2
void rrcb();      // 0xE2

// Rotate left through carry memory or accumulator.
void rlc(BYTE *data);
void rlc_abs();   // 0xA3
void rlc_abs_x(); // 0xB3
void rlc_abs_y(); // 0xC3
void rlca();      // 0xD3
void rlcb();      // 0xE3

// Arithmetic shift left memory or accumulator.
void sal(BYTE *data);
void sal_abs();   // 0xA4
void sal_abs_x(); // 0xB4
void sal_abs_y(); // 0xC4
void sala();      // 0xD4
void salb();      // 0xE4

// Arithmetic shift right memory or accumulator.
void sar(BYTE *data);
void sar_abs();   // 0xA5
void sar_abs_x(); // 0xB5
void sar_abs_y(); // 0xC5
void sara();      // 0xD5
void sarb();      // 0xE5

// Shift right memory or accumulator.
void lsr(BYTE *data);
void lsr_abs();   // 0xA6
void lsr_abs_x(); // 0xB6
void lsr_abs_y(); // 0xC6
void lsra();      // 0xD6
void lsrb();      // 0xE6

// Negate memory or accumulator.
void com(BYTE *data);
void com_abs();   // 0xA7
void com_abs_x(); // 0xB7
void com_abs_y(); // 0xC7
void coma();      // 0xD7
void comb();      // 0xE7

// Rotate left without carry memory or accumulator.
void rol(BYTE *data);
void rol_abs();   // 0xA8
void rol_abs_x(); // 0xB8
void rol_abs_y(); // 0xC8
void rola();      // 0xD8
void rolb();      // 0xE8

// Rotate right without carry memory or accumulator.
void rr(BYTE *data);
void rr_abs();   // 0xA9
void rr_abs_x(); // 0xB9
void rr_abs_y(); // 0xC9
void rra();      // 0xD9
void rrb();      // 0xE9

// Decrements register X.
void decx(); // 0x01

// Increments register X.
void incx(); // 0x02

// Transfers accumulator to register Y.
void cay(); // 0xF0

// Transfers register Y to accumulator.
void mya(); // 0xF1

// Decrements register Y.
void decy(); // 0x03

// Increments register Y.
void incy(); // 0x04

// Adds accumulator B into accumulator A.
void aba(); // 0xF3

// Subtracts accumulator B from accumulator A.
void sba(); // 0xF4

// Adds accumulator A into accumulator B.
void aab(); // 0xF5

// Subtracts accumulator A from accumulator B.
void sab(); // 0xF6

// Adds register pair into accumulator pair.
void adcp(); // 0xF7

// Subtracts register pair into accumulator pair.
void sbcp(); // 0xF8
