#include "mimo_kernels.h"

#if MIMO_ENABLE_EQ2
MIMO_NOINLINE
void mimo_eq_mmse_2x2_scalar(
    const mimo_c16_t h[MIMO_EQ2_SITE_COUNT][MIMO_N2][MIMO_N2],
    const mimo_c16_t y[MIMO_EQ2_SITE_COUNT][MIMO_N2],
    mimo_c16_t x[MIMO_EQ2_SITE_COUNT][MIMO_N2])
{
    mimo_eq2_mmse_scalar_fused_q15(MIMO_EQ2_SITE_COUNT, &h[0][0][0],
                                    &y[0][0], &x[0][0]);
}
#endif

MIMO_NOINLINE
void mimo_eq_mmse_4x4_scalar(const mimo_c16_t h[MIMO_EQ4_SITE_COUNT][MIMO_N4][MIMO_N4],
                             const mimo_c16_t y[MIMO_EQ4_SITE_COUNT][MIMO_N4],
                             mimo_c16_t x[MIMO_EQ4_SITE_COUNT][MIMO_N4])
{
    mimo_eq_mmse4_scalar_fused_q15(MIMO_EQ4_SITE_COUNT, &h[0][0][0],
                                    &y[0][0], &x[0][0]);
}
