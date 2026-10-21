#include "mimo_ext_site4.h"
#include "mimo_kernels.h"

MIMO_NOINLINE
void mimo_eq_mmse_4x4_ext(const mimo_c16_t h[MIMO_EQ4_SITE_COUNT][MIMO_N4][MIMO_N4],
                          const mimo_c16_t y[MIMO_EQ4_SITE_COUNT][MIMO_N4],
                          mimo_c16_t x[MIMO_EQ4_SITE_COUNT][MIMO_N4])
{
    mimo_ext_mmse4_packed_live_run(&h[0][0][0], &y[0][0], &x[0][0]);
}
