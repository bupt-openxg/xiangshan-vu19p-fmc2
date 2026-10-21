#ifndef MIMO_RVV_H
#define MIMO_RVV_H

#define MIMO_MSTATUS_VS_SHIFT 9
#define MIMO_MSTATUS_VS_MASK (0x3UL << MIMO_MSTATUS_VS_SHIFT)
#define MIMO_MSTATUS_VS_CLEAN (0x2UL << MIMO_MSTATUS_VS_SHIFT)

#ifndef read_csr
#define read_csr(csr) ({ unsigned long __v; asm volatile("csrr %0, " #csr : "=r"(__v)); __v; })
#endif

#ifndef write_csr
#define write_csr(csr, val) ({ unsigned long __v = (unsigned long)(val); asm volatile("csrw " #csr ", %0" :: "r"(__v)); })
#endif

static inline void mimo_rvv_set_vs_clean(void)
{
    unsigned long mstatus = read_csr(mstatus);
    mstatus &= ~MIMO_MSTATUS_VS_MASK;
    mstatus |= MIMO_MSTATUS_VS_CLEAN;
    write_csr(mstatus, mstatus);
}

void mimo_rvv_runtime_init(void);

#endif
