#include "fft_test_common.h"
#include <stdint.h>

#if defined(__GNUC__)
#define FFT_NOINLINE __attribute__((noinline))
#else
#define FFT_NOINLINE
#endif

#ifndef FFT_MAIN_POINTS
#define FFT_MAIN_POINTS 16
#endif

#ifndef FFT_MAIN_REPEAT
#define FFT_MAIN_REPEAT 1
#endif

#ifndef FFT_MAIN_WARMUP_REPEAT
#define FFT_MAIN_WARMUP_REPEAT 2
#endif

#ifndef FFT_VALIDATE
#define FFT_VALIDATE 1
#endif

#ifndef FFT_MAIN_RUN_IDFT
#define FFT_MAIN_RUN_IDFT 0
#endif

#define FFT_MAIN_MAX_POINTS 4096


typedef struct {
    int16_t re;
    int16_t im;
} cplx16_t;

static const int16_t tw16_dft[24] __attribute__((aligned(16))) = {
    32767, 0, 30272, 12540, 23169, 23170, 12539, 30273,
    32767, 0, 23169, 23170, 0, 32767, -23170, 23170,
    32767, 0, 12539, 30273, -23170, 23170, -30273, -12539
};

static const int16_t tw16_idft[24] __attribute__((aligned(16))) = {
    32767, 0, 30272, -12540, 23169, -23170, 12539, -30273,
    32767, 0, 23169, -23170, 0, -32767, -23170, -23170,
    32767, 0, 12539, -30273, -23170, -23170, -30273, 12539
};

static volatile int16_t g_fft_perf_sink;

static int16_t g_fft_main_input[2 * FFT_MAIN_MAX_POINTS] __attribute__((aligned(16)));
static int16_t g_fft_main_output[2 * FFT_MAIN_MAX_POINTS] __attribute__((aligned(16)));
#if FFT_VALIDATE
static int16_t g_fft_main_ref[2 * FFT_MAIN_MAX_POINTS] __attribute__((aligned(16)));
#endif

static inline int16_t clamp_i16(int32_t v)
{
    if (v > 32767) {
        return 32767;
    }
    if (v < -32768) {
        return -32768;
    }
    return (int16_t)v;
}

static inline int16_t add_sat_i16(int16_t a, int16_t b)
{
    return clamp_i16((int32_t)a + (int32_t)b);
}

static inline int16_t sub_sat_i16(int16_t a, int16_t b)
{
    return clamp_i16((int32_t)a - (int32_t)b);
}

static inline int16_t neg_wrap_i16(int16_t v)
{
    return (int16_t)((uint16_t)(~(uint16_t)v + 1u));
}

static inline cplx16_t add_sat_cplx(cplx16_t a, cplx16_t b)
{
    cplx16_t out = { add_sat_i16(a.re, b.re), add_sat_i16(a.im, b.im) };
    return out;
}

static inline cplx16_t sub_sat_cplx(cplx16_t a, cplx16_t b)
{
    cplx16_t out = { sub_sat_i16(a.re, b.re), sub_sat_i16(a.im, b.im) };
    return out;
}

static inline cplx16_t mul_minus_j_cplx(cplx16_t x)
{
    cplx16_t out = { x.im, neg_wrap_i16(x.re) };
    return out;
}

static inline int16_t narrow_q15_sra15(int32_t v)
{
    int32_t shifted;

    if (v >= 0) {
        shifted = v >> 15;
    } else {
        shifted = -(((-v) + 32767) >> 15);
    }

    return clamp_i16(shifted);
}

static inline cplx16_t packed_cmult_q15_scalar(cplx16_t a, int16_t tw_re, int16_t tw_im)
{
    int32_t re = (int32_t)a.re * (int32_t)tw_re + (int32_t)a.im * (int32_t)tw_im;
    int32_t im = (int32_t)a.im * (int32_t)tw_re - (int32_t)a.re * (int32_t)tw_im;
    cplx16_t out = { narrow_q15_sra15(re), narrow_q15_sra15(im) };
    return out;
}

static inline cplx16_t load_cplx(const int16_t *x, int idx)
{
    cplx16_t out = { x[2 * idx], x[2 * idx + 1] };
    return out;
}

static inline void store_cplx(int16_t *y, int idx, cplx16_t v)
{
    y[2 * idx] = v.re;
    y[2 * idx + 1] = v.im;
}

static inline void swap_cplx(cplx16_t *a, cplx16_t *b)
{
    cplx16_t tmp = *a;
    *a = *b;
    *b = tmp;
}

static inline void transpose4x4_cplx(cplx16_t t0[4],
                                     cplx16_t t1[4],
                                     cplx16_t t2[4],
                                     cplx16_t t3[4])
{
    swap_cplx(&t0[1], &t1[0]);
    swap_cplx(&t0[2], &t2[0]);
    swap_cplx(&t0[3], &t3[0]);
    swap_cplx(&t1[2], &t2[1]);
    swap_cplx(&t1[3], &t3[1]);
    swap_cplx(&t2[3], &t3[2]);
}

static FFT_NOINLINE void dft16_scalar(int16_t *x, int16_t *y)
{
    cplx16_t t0[4];
    cplx16_t t1[4];
    cplx16_t t2[4];
    cplx16_t t3[4];

    for (int i = 0; i < 4; ++i) {
        cplx16_t x0 = load_cplx(x, i);
        cplx16_t x1 = load_cplx(x, i + 4);
        cplx16_t x2 = load_cplx(x, i + 8);
        cplx16_t x3 = load_cplx(x, i + 12);
        cplx16_t x02t = add_sat_cplx(x0, x2);
        cplx16_t x13t = add_sat_cplx(x1, x3);

        t0[i] = add_sat_cplx(x02t, x13t);
        t2[i] = sub_sat_cplx(x02t, x13t);

        x02t = sub_sat_cplx(x0, x2);
        x13t = sub_sat_cplx(mul_minus_j_cplx(x1), mul_minus_j_cplx(x3));
        t1[i] = add_sat_cplx(x02t, x13t);
        t3[i] = sub_sat_cplx(x02t, x13t);
    }

    transpose4x4_cplx(t0, t1, t2, t3);

    for (int i = 0; i < 4; ++i) {
        t1[i] = packed_cmult_q15_scalar(t1[i], tw16_dft[0 + 2 * i], tw16_dft[1 + 2 * i]);
        t2[i] = packed_cmult_q15_scalar(t2[i], tw16_dft[8 + 2 * i], tw16_dft[9 + 2 * i]);
        t3[i] = packed_cmult_q15_scalar(t3[i], tw16_dft[16 + 2 * i], tw16_dft[17 + 2 * i]);
    }

    for (int i = 0; i < 4; ++i) {
        cplx16_t x02t = add_sat_cplx(t0[i], t2[i]);
        cplx16_t x13t = add_sat_cplx(t1[i], t3[i]);
        cplx16_t y0 = add_sat_cplx(x02t, x13t);
        cplx16_t y2 = sub_sat_cplx(x02t, x13t);

        x02t = sub_sat_cplx(t0[i], t2[i]);
        x13t = sub_sat_cplx(mul_minus_j_cplx(t1[i]), mul_minus_j_cplx(t3[i]));
        cplx16_t y1 = add_sat_cplx(x02t, x13t);
        cplx16_t y3 = sub_sat_cplx(x02t, x13t);

        store_cplx(y, i + 0 * 4, y0);
        store_cplx(y, i + 1 * 4, y1);
        store_cplx(y, i + 2 * 4, y2);
        store_cplx(y, i + 3 * 4, y3);
    }
}

static FFT_NOINLINE void idft16_scalar(int16_t *x, int16_t *y)
{
    cplx16_t t0[4];
    cplx16_t t1[4];
    cplx16_t t2[4];
    cplx16_t t3[4];

    for (int i = 0; i < 4; ++i) {
        cplx16_t x0 = load_cplx(x, i);
        cplx16_t x1 = load_cplx(x, i + 4);
        cplx16_t x2 = load_cplx(x, i + 8);
        cplx16_t x3 = load_cplx(x, i + 12);
        cplx16_t x02t = add_sat_cplx(x0, x2);
        cplx16_t x13t = add_sat_cplx(x1, x3);

        t0[i] = add_sat_cplx(x02t, x13t);
        t2[i] = sub_sat_cplx(x02t, x13t);

        x02t = sub_sat_cplx(x0, x2);
        x13t = sub_sat_cplx(mul_minus_j_cplx(x1), mul_minus_j_cplx(x3));
        t3[i] = add_sat_cplx(x02t, x13t);
        t1[i] = sub_sat_cplx(x02t, x13t);
    }

    transpose4x4_cplx(t0, t1, t2, t3);

    for (int i = 0; i < 4; ++i) {
        t1[i] = packed_cmult_q15_scalar(t1[i], tw16_idft[0 + 2 * i], tw16_idft[1 + 2 * i]);
        t2[i] = packed_cmult_q15_scalar(t2[i], tw16_idft[8 + 2 * i], tw16_idft[9 + 2 * i]);
        t3[i] = packed_cmult_q15_scalar(t3[i], tw16_idft[16 + 2 * i], tw16_idft[17 + 2 * i]);
    }

    for (int i = 0; i < 4; ++i) {
        cplx16_t x02t = add_sat_cplx(t0[i], t2[i]);
        cplx16_t x13t = add_sat_cplx(t1[i], t3[i]);
        cplx16_t y0 = add_sat_cplx(x02t, x13t);
        cplx16_t y2 = sub_sat_cplx(x02t, x13t);

        x02t = sub_sat_cplx(t0[i], t2[i]);
        x13t = sub_sat_cplx(mul_minus_j_cplx(t1[i]), mul_minus_j_cplx(t3[i]));
        cplx16_t y3 = add_sat_cplx(x02t, x13t);
        cplx16_t y1 = sub_sat_cplx(x02t, x13t);

        store_cplx(y, i + 0 * 4, y0);
        store_cplx(y, i + 1 * 4, y1);
        store_cplx(y, i + 2 * 4, y2);
        store_cplx(y, i + 3 * 4, y3);
    }
}

#define FFT_POW2_BASE_DFT16 dft16_scalar
#define FFT_POW2_BASE_IDFT16 idft16_scalar
#include "fft_pow2_scalar_impl.h"
#undef FFT_POW2_BASE_DFT16
#undef FFT_POW2_BASE_IDFT16

#if FFT_VALIDATE
extern int oai_fft_dft_reference_points(int points, int16_t *x, int16_t *y);
extern int oai_fft_idft_reference_points(int points, int16_t *x, int16_t *y);
#endif

int main(void)
{
    int16_t *x = g_fft_main_input;
    int16_t *y_scalar = g_fft_main_output;

    if (!fft_pow2_supported_points(FFT_MAIN_POINTS)) {
        printf("Unsupported FFT_MAIN_POINTS=%d in dft_scalar.c\n", FFT_MAIN_POINTS);
        return 1;
    }

    if (!fft_load_test_input(x, FFT_MAIN_POINTS)) {
        printf("Failed to load FFT test input for %d points\n", FFT_MAIN_POINTS);
        return 1;
    }

#if FFT_VALIDATE
    int16_t *y_ref = g_fft_main_ref;

    for (int i = 0; i < FFT_MAIN_REPEAT; ++i) {
        int ok = FFT_MAIN_RUN_IDFT
            ? oai_fft_idft_reference_points(FFT_MAIN_POINTS, x, y_ref)
            : oai_fft_dft_reference_points(FFT_MAIN_POINTS, x, y_ref);
        if (!ok) {
            printf("OAI reference dispatch failed for %d points\n", FFT_MAIN_POINTS);
            return 1;
        }
    }

    for (int i = 0; i < FFT_MAIN_REPEAT; ++i) {
        int ok = FFT_MAIN_RUN_IDFT
            ? fft_pow2_run_inverse(FFT_MAIN_POINTS, x, y_scalar)
            : fft_pow2_run_forward(FFT_MAIN_POINTS, x, y_scalar);
        if (!ok) {
            printf("Scalar dispatch failed for %d points\n", FFT_MAIN_POINTS);
            return 1;
        }
    }

    if (FFT_MAIN_POINTS <= 64) {
        fft_print_complex_vector("Input", x, FFT_MAIN_POINTS);
        fft_print_complex_vector(FFT_MAIN_RUN_IDFT ? "OAI reference IDFT" : "OAI reference DFT",
                                 y_ref,
                                 FFT_MAIN_POINTS);
        fft_print_complex_vector(FFT_MAIN_RUN_IDFT ? "Scalar IDFT result" : "Scalar DFT result",
                                 y_scalar,
                                 FFT_MAIN_POINTS);
    } else {
        printf("Vector dump omitted for %d points in validate mode\n", FFT_MAIN_POINTS);
    }

    if (fft_compare_complex_vector(y_ref, y_scalar, FFT_MAIN_POINTS)) {
        printf("CHECK: PASS\n");
        return 0;
    }

    printf("CHECK: FAIL\n");
    return 1;
#else
    int ok = 1;

    for (int i = 0; i < FFT_MAIN_WARMUP_REPEAT; ++i) {
        ok = FFT_MAIN_RUN_IDFT
            ? fft_pow2_run_inverse(FFT_MAIN_POINTS, x, y_scalar)
            : fft_pow2_run_forward(FFT_MAIN_POINTS, x, y_scalar);
        if (!ok) {
            break;
        }
    }

    if (!ok) {
        printf("Scalar dispatch failed during warmup for %d points\n", FFT_MAIN_POINTS);
        return 1;
    }

    fft_roi_stats_t roi_stats;
    fft_roi_prepare(&roi_stats);
    const fft_roi_stamp_t roi_start = fft_roi_begin(&roi_stats);

    for (int i = 0; i < FFT_MAIN_REPEAT; ++i) {
        ok = FFT_MAIN_RUN_IDFT
            ? fft_pow2_run_inverse(FFT_MAIN_POINTS, x, y_scalar)
            : fft_pow2_run_forward(FFT_MAIN_POINTS, x, y_scalar);
        if (!ok) {
            break;
        }
    }

    fft_roi_end(&roi_stats, roi_start, FFT_MAIN_REPEAT);
    if (!ok) {
        printf("Scalar dispatch failed for %d points\n", FFT_MAIN_POINTS);
        return 1;
    }

    fft_print_steady_state_config(FFT_MAIN_WARMUP_REPEAT, FFT_MAIN_REPEAT);
    fft_print_roi_summary(&roi_stats);
    g_fft_perf_sink = (int16_t)(y_scalar[0] ^ y_scalar[1]);
    return (g_fft_perf_sink == (int16_t)0x7fff) ? 1 : 0;
#endif
}
