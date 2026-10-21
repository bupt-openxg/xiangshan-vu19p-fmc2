#include "mimo_kernels.h"
#include "mimo_rvv_site4.h"

#if MIMO_ENABLE_EQ2
MIMO_NOINLINE
void mimo_eq_mmse_2x2_rvv(
    const mimo_c16_t h[MIMO_EQ2_SITE_COUNT][MIMO_N2][MIMO_N2],
    const mimo_c16_t y[MIMO_EQ2_SITE_COUNT][MIMO_N2],
    mimo_c16_t x[MIMO_EQ2_SITE_COUNT][MIMO_N2])
{
    mimo_eq2_mmse_rvv_ri_soa_q15(MIMO_EQ2_SITE_COUNT,
                                  (const int16_t *)(const void *)h,
                                  (const int16_t *)(const void *)y,
                                  &x[0][0]);
}
#endif

MIMO_NOINLINE
void mimo_eq_mmse_4x4_rvv(const mimo_c16_t h[MIMO_EQ4_SITE_COUNT][MIMO_N4][MIMO_N4],
                          const mimo_c16_t y[MIMO_EQ4_SITE_COUNT][MIMO_N4],
                          mimo_c16_t x[MIMO_EQ4_SITE_COUNT][MIMO_N4])
{
    mimo_rvv_eq4_site4_run(&h[0][0][0], &y[0][0], 1, &x[0][0]);
}
