#ifndef MIMO_BENCH_H
#define MIMO_BENCH_H

#include <stdint.h>

#include "klib.h"

typedef struct {
    uint64_t cycle_start;
    uint64_t instret_start;
} mimo_roi_stamp_t;

typedef struct {
    uint64_t cycle_total;
    uint64_t instret_total;
    uint32_t repeat;
} mimo_roi_stats_t;

static inline uint64_t mimo_read_cycle(void)
{
    uint64_t r = 0;
    asm volatile("rdcycle %0" : "=r"(r) :: "memory");
    return r;
}

static inline uint64_t mimo_read_instret(void)
{
    uint64_t r = 0;
    asm volatile("rdinstret %0" : "=r"(r) :: "memory");
    return r;
}

static inline mimo_roi_stamp_t mimo_roi_begin(void)
{
    mimo_roi_stamp_t stamp;

    asm volatile("" ::: "memory");
    stamp.instret_start = mimo_read_instret();
    stamp.cycle_start = mimo_read_cycle();
    return stamp;
}

static inline void mimo_roi_end(mimo_roi_stats_t *stats,
                                mimo_roi_stamp_t stamp,
                                uint32_t repeat)
{
    const uint64_t cycle_end = mimo_read_cycle();
    const uint64_t instret_end = mimo_read_instret();

    asm volatile("" ::: "memory");
    stats->cycle_total = cycle_end - stamp.cycle_start;
    stats->instret_total = instret_end - stamp.instret_start;
    stats->repeat = repeat;
}

static inline void mimo_print_steady_state_config(uint32_t warmup_repeat,
                                                  uint32_t measured_repeat)
{
    printf("MIMO_STEADY_STATE warmup=%u measured_repeat=%u\n",
           warmup_repeat,
           measured_repeat);
}

static inline void mimo_print_roi_summary(const char *kernel,
                                          const char *backend,
                                          const mimo_roi_stats_t *stats)
{
    printf("MIMO_ROI kernel=%s backend=%s cycle=%llu instret=%llu repeat=%u\n",
           kernel,
           backend,
           (unsigned long long)stats->cycle_total,
           (unsigned long long)stats->instret_total,
           stats->repeat);
}

#endif
