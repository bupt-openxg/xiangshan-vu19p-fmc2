#ifndef FFT_TEST_COMMON_H
#define FFT_TEST_COMMON_H

#include "fft_roi_pmu.h"
#include "klib.h"
#include <stdint.h>

typedef struct {
    uint64_t cycle_total;
    uint64_t instret_total;
    uint32_t trials;
    fft_roi_hpm_state_t hpm;
} fft_roi_stats_t;

typedef struct {
    uint64_t cycle_start;
    uint64_t instret_start;
} fft_roi_stamp_t;

static inline uint64_t fft_read_cycle(void)
{
    uint64_t r = 0;
    asm volatile("rdcycle %0" : "=r"(r) :: "memory");
    return r;
}

static inline uint64_t fft_read_instret(void)
{
    uint64_t r = 0;
    asm volatile("rdinstret %0" : "=r"(r) :: "memory");
    return r;
}

static inline void fft_roi_reset(fft_roi_stats_t *stats)
{
    stats->cycle_total = 0;
    stats->instret_total = 0;
    stats->trials = 0;
    fft_roi_hpm_reset(&stats->hpm);
}

static inline void fft_roi_prepare(fft_roi_stats_t *stats)
{
    fft_roi_reset(stats);
    fft_roi_hpm_prepare(&stats->hpm);
}

static inline fft_roi_stamp_t fft_roi_begin(fft_roi_stats_t *stats)
{
    fft_roi_stamp_t stamp;

    asm volatile("" ::: "memory");
    stamp.instret_start = fft_read_instret();
    stamp.cycle_start = fft_read_cycle();
    fft_roi_hpm_begin(&stats->hpm);
    return stamp;
}

static inline void fft_roi_end(fft_roi_stats_t *stats,
                               fft_roi_stamp_t stamp,
                               uint32_t repeat)
{
    fft_roi_hpm_end(&stats->hpm);
    const uint64_t end_cycle = fft_read_cycle();
    const uint64_t end_instret = fft_read_instret();
    asm volatile("" ::: "memory");
    stats->cycle_total = end_cycle - stamp.cycle_start;
    stats->instret_total = end_instret - stamp.instret_start;
    stats->trials = repeat;
}

static inline void fft_print_roi_hpm_summary(const fft_roi_stats_t *stats)
{
    for (uint32_t i = 0; i < fft_roi_hpm_event_count(); ++i) {
        const fft_roi_hpm_event_desc_t *desc = fft_roi_hpm_event_desc(i);
        printf("FFT_ROI_HPM group=%s event=%s value=%llu\n",
               desc->group,
               desc->event,
               (unsigned long long)stats->hpm.values[i]);
    }
}

static inline void fft_print_roi_summary(const fft_roi_stats_t *stats)
{
    printf("FFT_ROI cycle=%llu instret=%llu repeat=%u\n",
           (unsigned long long)stats->cycle_total,
           (unsigned long long)stats->instret_total,
           stats->trials);
    fft_print_roi_hpm_summary(stats);
}

static inline void fft_print_steady_state_config(uint32_t warmup_repeat,
                                                 uint32_t measured_repeat)
{
    printf("FFT_STEADY_STATE warmup=%u measured_repeat=%u\n",
           warmup_repeat,
           measured_repeat);
}

static const int16_t k_fft16_test_input[32] __attribute__((aligned(16))) = {
    5, -8, 16, 2, 29, 13, 28, 25,
    36, 17, 59, 29, 53, 52, 57, 22,
    -1, 13, -7, 23, 12, 32, 18, 41,
    14, 41, 22, 48, 33, 12, 49, 19,
};

static inline int fft_load_test_input(int16_t *dst, int complex_n)
{
    if (complex_n <= 0 || complex_n > 4096) {
        return 0;
    }

    if (complex_n == 16) {
        for (int i = 0; i < complex_n * 2; ++i) {
            dst[i] = k_fft16_test_input[i];
        }
        return 1;
    }

    for (int i = 0; i < complex_n; ++i) {
        int32_t real_part =
            ((i * 17) % 97) +
            ((i * 11) % 29) -
            ((i * 7) % 31) +
            (i % 9) * 3 -
            (i % 5) * 4 -
            48;

        int32_t imag_part =
            ((i * 13) % 89) -
            ((i * 19) % 43) +
            (i % 7) * 5 -
            (i % 11) * 3 +
            ((i / 3) % 17) * 2 -
            40;

        dst[2 * i] = (int16_t)(real_part % 64);
        dst[2 * i + 1] = (int16_t)(imag_part % 64);
    }

    return 1;
}

static inline void fft_print_complex_vector(const char *label,
                                            const int16_t *x,
                                            int complex_n)
{
    printf("%s (%d points):\n", label, complex_n);
    for (int i = 0; i < complex_n; ++i) {
        printf("[%2d] = %6d + %6dj\n", i, x[2 * i], x[2 * i + 1]);
    }
}

static inline int fft_compare_complex_vector(const int16_t *ref,
                                             const int16_t *dut,
                                             int complex_n)
{
    int pass = 1;

    for (int i = 0; i < complex_n; ++i) {
        const int re_idx = 2 * i;
        const int im_idx = re_idx + 1;

        if (ref[re_idx] != dut[re_idx] || ref[im_idx] != dut[im_idx]) {
            printf("Mismatch at [%2d]: ref=%d + %dj, dut=%d + %dj\n",
                   i,
                   ref[re_idx], ref[im_idx],
                   dut[re_idx], dut[im_idx]);
            pass = 0;
        }
    }

    return pass;
}

#endif
