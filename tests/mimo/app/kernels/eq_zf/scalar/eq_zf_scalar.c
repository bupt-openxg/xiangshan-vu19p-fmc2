#include "mimo_kernels.h"

#if MIMO_ENABLE_EQ2
MIMO_NOINLINE
void mimo_eq_zf_2x2_scalar(
    const mimo_c16_t h[MIMO_EQ2_SITE_COUNT][MIMO_N2][MIMO_N2],
    const mimo_c16_t y[MIMO_EQ2_SITE_COUNT][MIMO_N2],
    mimo_c16_t x[MIMO_EQ2_SITE_COUNT][MIMO_N2])
{
    mimo_eq2_zf_scalar_fused_q15(MIMO_EQ2_SITE_COUNT, &h[0][0][0],
                                  &y[0][0], &x[0][0]);
}
#endif

static void eq_zf4_scalar_run(size_t site_count,
                              const mimo_c16_t *h,
                              const mimo_c16_t *y,
                              mimo_c16_t *x)
{
    mimo_c32_t a[MIMO_N4 * MIMO_N4];
    mimo_c32_t b[MIMO_N4];

    for (size_t site = 0; site < site_count; ++site) {
        mimo_build_normal_row4_upper_q15(&h[site * MIMO_N4 * MIMO_N4],
                                         &y[site * MIMO_N4], a, b);
        mimo_solve_normal4_scalar_q15(a, b, 0, &x[site * MIMO_N4]);
    }
}

MIMO_NOINLINE
void mimo_eq_zf_4x4_scalar(const mimo_c16_t h[MIMO_EQ4_SITE_COUNT][MIMO_N4][MIMO_N4],
                           const mimo_c16_t y[MIMO_EQ4_SITE_COUNT][MIMO_N4],
                           mimo_c16_t x[MIMO_EQ4_SITE_COUNT][MIMO_N4])
{
    eq_zf4_scalar_run(MIMO_EQ4_SITE_COUNT, &h[0][0][0], &y[0][0],
                      &x[0][0]);
}
