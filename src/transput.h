#include "main.h"

void ldaa_imm();
void ldaa_abs();
void ldaa_abs_x();
void ldaa_abs_y();
void ldaa_indir();
void ldaa_indir_x();

void ldab_imm();
void ldab_abs();
void ldab_abs_x();
void ldab_abs_y();
void ldab_indir();
void ldab_indir_x();

void stora_abs();
void stora_abs_x();
void stora_abs_y();
void stora_indir();
void stora_indir_x();

void storb_abs();
void storb_abs_x();
void storb_abs_y();
void storb_indir();
void storb_indir_x();

void ldx_imm();
void ldx_abs();
void ldx_abs_x();
void ldx_abs_y();
void ldx_indir();
void ldx_indir_x();

void stox_abs();
void stox_abs_x();
void stox_abs_y();
void stox_indir();
void stox_indir_x();

void ldy_imm();
void ldy_abs();
void ldy_abs_x();
void ldy_abs_y();
void ldy_indir();
void ldy_indir_x();

void stoy_abs();
void stoy_abs_x();
void stoy_abs_y();
void stoy_indir();
void stoy_indir_x();

void load_stackpointer(WORD address);
void lods_imm();
void lods_abs();
void lods_abs_x();
void lods_abs_y();
void lods_indir();
void lods_indir_x();

void save_stackpointer(WORD address);
void stos_abs();
void stos_abs_x();
void stos_abs_y();
void stos_indir();
void stos_indir_x();

void csa();

void lx();

void mvi_l();
void mvi_h();

void xchg();
