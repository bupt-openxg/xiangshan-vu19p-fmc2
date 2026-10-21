#include "mimo_rvv.h"

void mimo_rvv_runtime_init(void)
{
    mimo_rvv_set_vs_clean();
    asm volatile("csrwi vxrm, 0" ::: "memory");
    asm volatile("" ::: "memory");
}
