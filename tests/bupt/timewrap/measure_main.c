// ReConstruct: measure whole-main CPU cycles on the BUPT FFT/MIMO images.
// The linker wraps main -> __wrap_main; boot, DDR load and Vivado are outside.
#include <stdint.h>
#include <klib.h>

static inline uint64_t bupt_rd_mcycle(void) {
    uint64_t v;
    asm volatile("rdcycle %0" : "=r"(v) :: "memory");
    return v;
}

int __real_main(void);

int __wrap_main(void) {
    uint64_t t0 = bupt_rd_mcycle();
    int rc = __real_main();
    uint64_t t1 = bupt_rd_mcycle();
    uint64_t d = t1 - t0;

    // 30 MHz CPU: 1 us = 30 cycles, 1 ms = 30000 cycles.
    printf("BUPT_TIME main_cycles=%llu us=%llu ms=%llu\n",
           (unsigned long long)d,
           (unsigned long long)(d / 30ULL),
           (unsigned long long)(d / 30000ULL));
    return rc;
}
