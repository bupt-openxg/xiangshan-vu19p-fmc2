#ifndef MIMO_KERNELS_H
#define MIMO_KERNELS_H

#include "mimo_types.h"

#ifndef MIMO_USE_SCALAR4_UPPER_ONLY_BUILD
#define MIMO_USE_SCALAR4_UPPER_ONLY_BUILD 0
#endif

#ifndef MIMO_USE_SCALAR_ZF4_Q15ACC_BUILD
#define MIMO_USE_SCALAR_ZF4_Q15ACC_BUILD 0
#endif

#ifndef MIMO_USE_RVV_ZF4_Q15ACC_BUILD
#define MIMO_USE_RVV_ZF4_Q15ACC_BUILD 0
#endif

#ifndef MIMO_USE_SOLVE4_SCHUR
#define MIMO_USE_SOLVE4_SCHUR 0
#endif

#ifndef MIMO_COLLECT_SCHUR_STATS
#define MIMO_COLLECT_SCHUR_STATS 0
#endif

#if MIMO_USE_SCALAR4_UPPER_ONLY_BUILD && \
    (!MIMO_USE_SOLVE4_SCHUR || MIMO_COLLECT_SCHUR_STATS)
#error "MIMO_USE_SCALAR4_UPPER_ONLY_BUILD is only valid for Schur solve with Schur stats disabled"
#endif

void mimo_build_normal_row_q15(int n,
                               const mimo_c16_t *h_row,
                               const mimo_c16_t *y,
                               mimo_c32_t *a,
                               mimo_c32_t *b);
void mimo_build_normal_row4_upper_q15(const mimo_c16_t *h_row,
                                      const mimo_c16_t *y,
                                      mimo_c32_t *a,
                                      mimo_c32_t *b);
void mimo_build_normal_row4_q15acc_q15(const mimo_c16_t *h_row,
                                       const mimo_c16_t *y,
                                       mimo_c32_t *a,
                                       mimo_c32_t *b);
void mimo_eq_mmse4_scalar_fused_q15(size_t site_count,
                                    const mimo_c16_t *restrict h_row,
                                    const mimo_c16_t *restrict y,
                                    mimo_c16_t *restrict x);
void mimo_eq2_zf_scalar_fused_q15(size_t site_count,
                                  const mimo_c16_t *restrict h_row,
                                  const mimo_c16_t *restrict y,
                                  mimo_c16_t *restrict x);
void mimo_eq2_mmse_scalar_fused_q15(size_t site_count,
                                    const mimo_c16_t *restrict h_row,
                                    const mimo_c16_t *restrict y,
                                    mimo_c16_t *restrict x);

void mimo_build_normal_col_q15(int n,
                               const mimo_c16_t *h_col,
                               const mimo_c16_t *y,
                               mimo_c32_t *a,
                               mimo_c32_t *b);
void mimo_init_recip_table(void);
void mimo_print_solve_den_stats(void);

void mimo_solve_normal_q15(int n,
                           const mimo_c32_t *a,
                           const mimo_c32_t *b,
                           int mmse,
                           mimo_c16_t *x);
void mimo_solve_normal4_scalar_q15(const mimo_c32_t *a,
                                   const mimo_c32_t *b,
                                   int mmse,
                                   mimo_c16_t *x);
void mimo_solve_normal4_site4_q15(
    mimo_c32_t a_lane[restrict MIMO_SITE4_LANES][MIMO_N4 * MIMO_N4],
    mimo_c32_t b_lane[restrict MIMO_SITE4_LANES][MIMO_N4],
    int mmse,
    mimo_c16_t *restrict x_block);
void mimo_solve_normal4_site4_hermitian_zf_q15(
    mimo_c32_t a_lane[restrict MIMO_SITE4_LANES][MIMO_N4 * MIMO_N4],
    mimo_c32_t b_lane[restrict MIMO_SITE4_LANES][MIMO_N4],
    mimo_c16_t *restrict x_block);
void mimo_solve_normal4_site4_ldlh_q15(
    mimo_c32_t a_lane[restrict MIMO_SITE4_LANES][MIMO_N4 * MIMO_N4],
    mimo_c32_t b_lane[restrict MIMO_SITE4_LANES][MIMO_N4],
    int mmse,
    mimo_c16_t *restrict x_block);
void mimo_solve_normal4_site4_schur_q15(
    mimo_c32_t a_lane[restrict MIMO_SITE4_LANES][MIMO_N4 * MIMO_N4],
    mimo_c32_t b_lane[restrict MIMO_SITE4_LANES][MIMO_N4],
    int mmse,
    mimo_c16_t *restrict x_block);
void mimo_solve_normal4_site4_soa_q15(mimo_site4_soa4_t *restrict work,
                                      int mmse,
                                      mimo_c16_t *restrict x_block);
void mimo_solve_normal4_site4_soa_schur_q15(
    mimo_site4_soa4_t *restrict work,
    int mmse,
    mimo_c16_t *restrict x_block);
void mimo_solve_normal4_site4_soa_schur_mmse_q15(
    mimo_site4_soa4_t *restrict work,
    mimo_c16_t *restrict x_block);
void mimo_solve_normal4_site4_soa_schur_zf_q15(
    mimo_site4_soa4_t *restrict work,
    mimo_c16_t *restrict x_block);
void mimo_solve_normal4_site4_soa_schur_zf_handasm_q15(
    mimo_site4_soa4_t *restrict work,
    mimo_c16_t *restrict x_block);
void mimo_vschur2h_asm_stub_q15(void);
void mimo_solve_normal4_site4_upper_i16_schur_zf_q15(
    const mimo_site4_upper_i16_t *restrict work,
    int mmse,
    mimo_c16_t *restrict x_block);
void mimo_solve_normal4_site4_upper_i16_schur_zf_rvv_i32_q15(
    const mimo_site4_upper_i16_t *restrict work,
    int mmse,
    mimo_c16_t *restrict x_block);
void mimo_solve_normal4_site4_upper_i16_schur_zf_rvv_i32_split_q15(
    const mimo_site4_upper_i16_t *restrict work,
    int mmse,
    mimo_c16_t *restrict x_block);
void mimo_solve_normal4_site4_upper_i16_schur_zf_rvv_i32_split_ext_q15(
    const mimo_site4_upper_i16_t *restrict work,
    int mmse,
    mimo_c16_t *restrict x_block);
void mimo_ext_mmse4_packed_live_run(const mimo_c16_t *h_site4,
                                    const mimo_c16_t *y_site4,
                                    mimo_c16_t *x);
void mimo_ext_zf4_packed_live_run(const mimo_c16_t *h_site4,
                                  const mimo_c16_t *y_site4,
                                  mimo_c16_t *x);
void mimo_solve_normal4_site4_soa_hermitian_zf_q15(
    mimo_site4_soa4_t *restrict work,
    mimo_c16_t *restrict x_block);
void mimo_solve_normal_ref_q15(int n,
                               const mimo_c32_t *a,
                               const mimo_c32_t *b,
                               int mmse,
                               mimo_c16_t *x);
void mimo_eq4_direct_h_row_q15(size_t site_count,
                               const mimo_c16_t *h_row,
                               const mimo_c16_t *y,
                               mimo_c16_t *x);
void mimo_eq4_direct_h_site4_q15(const mimo_c16_t *h_site4,
                                 const mimo_c16_t *y_site4,
                                 mimo_c16_t *x);

mimo_c16_t mimo_cmul_q15(mimo_c16_t a, mimo_c16_t b);
mimo_c16_t mimo_cmul_conj_a_q15(mimo_c16_t a, mimo_c16_t b);
mimo_c16_t mimo_make_c16(int16_t r, int16_t i);
int16_t mimo_sat_i16(int32_t x);

void mimo_eq_zf_2x2_scalar(const mimo_c16_t h[MIMO_EQ2_SITE_COUNT][MIMO_N2][MIMO_N2],
                           const mimo_c16_t y[MIMO_EQ2_SITE_COUNT][MIMO_N2],
                           mimo_c16_t x[MIMO_EQ2_SITE_COUNT][MIMO_N2]);
void mimo_eq_mmse_2x2_scalar(const mimo_c16_t h[MIMO_EQ2_SITE_COUNT][MIMO_N2][MIMO_N2],
                             const mimo_c16_t y[MIMO_EQ2_SITE_COUNT][MIMO_N2],
                             mimo_c16_t x[MIMO_EQ2_SITE_COUNT][MIMO_N2]);
void mimo_eq_zf_2x2_rvv(const mimo_c16_t h[MIMO_EQ2_SITE_COUNT][MIMO_N2][MIMO_N2],
                        const mimo_c16_t y[MIMO_EQ2_SITE_COUNT][MIMO_N2],
                        mimo_c16_t x[MIMO_EQ2_SITE_COUNT][MIMO_N2]);
void mimo_eq_mmse_2x2_rvv(const mimo_c16_t h[MIMO_EQ2_SITE_COUNT][MIMO_N2][MIMO_N2],
                          const mimo_c16_t y[MIMO_EQ2_SITE_COUNT][MIMO_N2],
                          mimo_c16_t x[MIMO_EQ2_SITE_COUNT][MIMO_N2]);
void mimo_eq_zf_2x2_ext(const mimo_c16_t h[MIMO_EQ2_SITE_COUNT][MIMO_N2][MIMO_N2],
                        const mimo_c16_t y[MIMO_EQ2_SITE_COUNT][MIMO_N2],
                        mimo_c16_t x[MIMO_EQ2_SITE_COUNT][MIMO_N2]);
void mimo_eq_mmse_2x2_ext(const mimo_c16_t h[MIMO_EQ2_SITE_COUNT][MIMO_N2][MIMO_N2],
                          const mimo_c16_t y[MIMO_EQ2_SITE_COUNT][MIMO_N2],
                          mimo_c16_t x[MIMO_EQ2_SITE_COUNT][MIMO_N2]);
void mimo_eq2_zf_rvv_ri_soa_q15(size_t site_count,
                                const int16_t *restrict h_ri_soa,
                                const int16_t *restrict y_ri_soa,
                                mimo_c16_t *restrict x);
void mimo_eq2_mmse_rvv_ri_soa_q15(size_t site_count,
                                  const int16_t *restrict h_ri_soa,
                                  const int16_t *restrict y_ri_soa,
                                  mimo_c16_t *restrict x);

void mimo_eq_zf_4x4_scalar(const mimo_c16_t h[MIMO_EQ4_SITE_COUNT][MIMO_N4][MIMO_N4],
                           const mimo_c16_t y[MIMO_EQ4_SITE_COUNT][MIMO_N4],
                           mimo_c16_t x[MIMO_EQ4_SITE_COUNT][MIMO_N4]);
void mimo_eq_mmse_4x4_scalar(const mimo_c16_t h[MIMO_EQ4_SITE_COUNT][MIMO_N4][MIMO_N4],
                             const mimo_c16_t y[MIMO_EQ4_SITE_COUNT][MIMO_N4],
                             mimo_c16_t x[MIMO_EQ4_SITE_COUNT][MIMO_N4]);

void mimo_eq_zf_4x4_rvv(const mimo_c16_t h[MIMO_EQ4_SITE_COUNT][MIMO_N4][MIMO_N4],
                        const mimo_c16_t y[MIMO_EQ4_SITE_COUNT][MIMO_N4],
                        mimo_c16_t x[MIMO_EQ4_SITE_COUNT][MIMO_N4]);
void mimo_eq_mmse_4x4_rvv(const mimo_c16_t h[MIMO_EQ4_SITE_COUNT][MIMO_N4][MIMO_N4],
                          const mimo_c16_t y[MIMO_EQ4_SITE_COUNT][MIMO_N4],
                          mimo_c16_t x[MIMO_EQ4_SITE_COUNT][MIMO_N4]);

void mimo_eq_zf_4x4_ext(const mimo_c16_t h[MIMO_EQ4_SITE_COUNT][MIMO_N4][MIMO_N4],
                        const mimo_c16_t y[MIMO_EQ4_SITE_COUNT][MIMO_N4],
                        mimo_c16_t x[MIMO_EQ4_SITE_COUNT][MIMO_N4]);
void mimo_eq_mmse_4x4_ext(const mimo_c16_t h[MIMO_EQ4_SITE_COUNT][MIMO_N4][MIMO_N4],
                          const mimo_c16_t y[MIMO_EQ4_SITE_COUNT][MIMO_N4],
                          mimo_c16_t x[MIMO_EQ4_SITE_COUNT][MIMO_N4]);

#endif
