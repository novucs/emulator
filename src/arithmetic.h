#include "main.h"

void adc(int accumulator, int reg);
void adc_al();
void adc_ah();
void adc_am();
void adc_bl();
void adc_bh();
void adc_bm();

void sbc(int accumulator, int reg);
void sbc_al();
void sbc_ah();
void sbc_am();
void sbc_bl();
void sbc_bh();
void sbc_bm();

void add(int accumulator, int reg);
void add_al();
void add_ah();
void add_am();
void add_bl();
void add_bh();
void add_bm();

void sub(int accumulator, int reg);
void sub_al();
void sub_ah();
void sub_am();
void sub_bl();
void sub_bh();
void sub_bm();

void cmp(int accumulator, int reg);
void cmp_al();
void cmp_ah();
void cmp_am();
void cmp_bl();
void cmp_bh();
void cmp_bm();

void _or(int accumulator, int reg);
void or_al();
void or_ah();
void or_am();
void or_bl();
void or_bh();
void or_bm();

void _and(int accumulator, int reg);
void and_al();
void and_ah();
void and_am();
void and_bl();
void and_bh();
void and_bm();

void _xor(int accumulator, int reg);
void xor_al();
void xor_ah();
void xor_am();
void xor_bl();
void xor_bh();
void xor_bm();

void _bit(int accumulator, int reg);
void bit_al();
void bit_ah();
void bit_am();
void bit_bl();
void bit_bh();
void bit_bm();

void sbi(BYTE accumulator);
void sbia();
void sbib();

void cpia();
void cpib();

void ori(BYTE accumulator);
void oria();
void orib();

void inc(WORD address);
void inc_abs();
void inc_abs_x();
void inc_abs_y();
void inca();
void incb();

void dec(WORD address);
void dec_abs();
void dec_abs_x();
void dec_abs_y();
void deca();
void decb();

void rrc(BYTE *data);
void rrc_abs();
void rrc_abs_x();
void rrc_abs_y();
void rrca();
void rrcb();

void rlc(BYTE *data);
void rlc_abs();
void rlc_abs_x();
void rlc_abs_y();
void rlca();
void rlcb();

void sal(BYTE *data);
void sal_abs();
void sal_abs_x();
void sal_abs_y();
void sala();
void salb();

void sar(BYTE *data);
void sar_abs();
void sar_abs_x();
void sar_abs_y();
void sara();
void sarb();

void lsr(BYTE *data);
void lsr_abs();
void lsr_abs_x();
void lsr_abs_y();
void lsra();
void lsrb();

void com(BYTE *data);
void com_abs();
void com_abs_x();
void com_abs_y();
void coma();
void comb();

void rol(BYTE *data);
void rol_abs();
void rol_abs_x();
void rol_abs_y();
void rola();
void rolb();

void rr(BYTE *data);
void rr_abs();
void rr_abs_x();
void rr_abs_y();
void rra();
void rrb();

void decx();

void incx();

void cay();

void mya();

void decy();

void incy();

void aba();

void sba();

void aab();

void sab();

void adcp();

void sbcp();
