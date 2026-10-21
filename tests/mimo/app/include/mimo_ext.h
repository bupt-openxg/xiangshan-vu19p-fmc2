#ifndef MIMO_EXT_H
#define MIMO_EXT_H

#include "mimo_types.h"

static inline void mimo_ext_set_vxrm_rnu_once(void)
{
    asm volatile("csrwi vxrm, 0" ::: "memory");
}

static inline void mimo_ext_set_vtype_e16m1_8_once(void)
{
    asm volatile("vsetivli zero, 8, e16, m1, ta, ma" ::: "memory");
}

static inline void mimo_ext_vscmulcj4_q15(const mimo_c16_t *a,
                                          const mimo_c16_t *b,
                                          int16_t *prod)
{
    asm volatile(
        "vle16.v v20, (%[pa])\n\t"
        "vle16.v v24, (%[pb])\n\t"
        ".word 0xCF4C0857\n\t"
        "vse16.v v16, (%[po])\n\t"
        :
        : [pa] "r"(a), [pb] "r"(b), [po] "r"(prod)
        : "memory");
}

static inline void mimo_ext_vscmul4_q15(const mimo_c16_t *a,
                                        const mimo_c16_t *b,
                                        int16_t *prod)
{
    asm volatile(
        "vle16.v v20, (%[pa])\n\t"
        "vle16.v v24, (%[pb])\n\t"
        ".word 0xCB4C0857\n\t"
        "vse16.v v16, (%[po])\n\t"
        :
        : [pa] "r"(a), [pb] "r"(b), [po] "r"(prod)
        : "memory");
}

#endif
