#ifndef FFT_POW2_SCALAR_IMPL_H
#define FFT_POW2_SCALAR_IMPL_H

#include <stdint.h>

#ifndef FFT_POW2_BASE_DFT16
#error "FFT_POW2_BASE_DFT16 must be defined before including fft_pow2_scalar_impl.h"
#endif

#ifndef FFT_POW2_BASE_IDFT16
#error "FFT_POW2_BASE_IDFT16 must be defined before including fft_pow2_scalar_impl.h"
#endif

#ifndef FFT_NOINLINE
#if defined(__GNUC__)
#define FFT_NOINLINE __attribute__((noinline))
#else
#define FFT_NOINLINE
#endif
#endif

#define FFTP_ONE_OVER_SQRT2_Q15 23170

#include "fft_pow2_twiddles_precomputed.h"

typedef struct {
    int16_t re;
    int16_t im;
} fftp_cplx16_t;

static int16_t fftp_xtmp64[2 * 64];
static int16_t fftp_ytmp64[2 * 64];
static int16_t fftp_xtmp128[2 * 128];
static int16_t fftp_xtmp256[2 * 256];
static int16_t fftp_xtmp512[2 * 512];
static int16_t fftp_xtmp1024[2 * 1024];
static int16_t fftp_xtmp2048[2 * 2048];
static int16_t fftp_xtmp4096[2 * 4096];

#ifndef FFT_SCALAR_PRELAYOUT_N2048
#define FFT_SCALAR_PRELAYOUT_N2048 0
#endif

static inline int16_t fftp_clamp_i16(int32_t v)
{
    if (v > 32767) {
        return 32767;
    }
    if (v < -32768) {
        return -32768;
    }
    return (int16_t)v;
}

static inline int16_t fftp_neg_wrap_i16(int16_t v)
{
    return (int16_t)((uint16_t)(~(uint16_t)v + 1u));
}

static inline fftp_cplx16_t fftp_add_sat(fftp_cplx16_t a, fftp_cplx16_t b)
{
    fftp_cplx16_t out = {
        fftp_clamp_i16((int32_t)a.re + (int32_t)b.re),
        fftp_clamp_i16((int32_t)a.im + (int32_t)b.im),
    };
    return out;
}

static inline fftp_cplx16_t fftp_sub_sat(fftp_cplx16_t a, fftp_cplx16_t b)
{
    fftp_cplx16_t out = {
        fftp_clamp_i16((int32_t)a.re - (int32_t)b.re),
        fftp_clamp_i16((int32_t)a.im - (int32_t)b.im),
    };
    return out;
}

static inline fftp_cplx16_t fftp_mul_minus_j(fftp_cplx16_t x)
{
    fftp_cplx16_t out = { x.im, fftp_neg_wrap_i16(x.re) };
    return out;
}

static inline int16_t fftp_narrow_q15_sra15(int32_t v)
{
    int32_t shifted;

    if (v >= 0) {
        shifted = v >> 15;
    } else {
        shifted = -(((-v) + 32767) >> 15);
    }

    return fftp_clamp_i16(shifted);
}

static inline int16_t fftp_mulhrs_i16(int16_t a, int16_t b)
{
    int32_t prod = (int32_t)a * (int32_t)b + 0x4000;
    return (int16_t)(prod >> 15);
}

static inline fftp_cplx16_t fftp_mul_q15(fftp_cplx16_t a, int16_t tw_re, int16_t tw_im)
{
    int32_t re = (int32_t)a.re * (int32_t)tw_re + (int32_t)a.im * (int32_t)tw_im;
    int32_t im = (int32_t)a.im * (int32_t)tw_re - (int32_t)a.re * (int32_t)tw_im;
    fftp_cplx16_t out = {
        fftp_narrow_q15_sra15(re),
        fftp_narrow_q15_sra15(im),
    };
    return out;
}

static inline fftp_cplx16_t fftp_load_cplx(const int16_t *x, int idx)
{
    fftp_cplx16_t out = { x[2 * idx], x[2 * idx + 1] };
    return out;
}

static inline void fftp_store_cplx(int16_t *y, int idx, fftp_cplx16_t v)
{
    y[2 * idx] = v.re;
    y[2 * idx + 1] = v.im;
}

static inline void fftp_ensure_tables(void)
{
}

static inline void fftp_scale_shift(int16_t *y, int complex_n, unsigned shift)
{
    const int lanes = complex_n * 2;
    for (int i = 0; i < lanes; ++i) {
        y[i] = (int16_t)(y[i] >> shift);
    }
}

static inline void fftp_scale_mulhrs(int16_t *y, int complex_n, int16_t coeff)
{
    const int lanes = complex_n * 2;
    for (int i = 0; i < lanes; ++i) {
        y[i] = fftp_mulhrs_i16(y[i], coeff);
    }
}

static void fftp_split_radix4(const int16_t *src, int16_t *dst, int child_points)
{
    for (int i = 0; i < child_points; ++i) {
        for (int r = 0; r < 4; ++r) {
            const int src_idx = 4 * i + r;
            const int dst_idx = r * child_points + i;
            uint32_t raw;

            __builtin_memcpy(&raw, src + 2 * src_idx, sizeof(raw));
            __builtin_memcpy(dst + 2 * dst_idx, &raw, sizeof(raw));
        }
    }
}

static inline void fftp_copy_cplx_raw(const int16_t *src, int16_t *dst)
{
    uint32_t raw;

    __builtin_memcpy(&raw, src, sizeof(raw));
    __builtin_memcpy(dst, &raw, sizeof(raw));
}

static void fftp_layout_to_leaf16_recursive(const int16_t *src,
                                            int src_stride_points,
                                            int16_t *dst,
                                            int points)
{
    if (points == 16) {
        for (int i = 0; i < 16; ++i) {
            fftp_copy_cplx_raw(src + 2 * i * src_stride_points, dst + 2 * i);
        }
        return;
    }

    const int radix = ((points == 128) || (points == 512) || (points == 2048)) ? 2 : 4;
    const int child_points = points / radix;

    for (int r = 0; r < radix; ++r) {
        fftp_layout_to_leaf16_recursive(src + 2 * r * src_stride_points,
                                        src_stride_points * radix,
                                        dst + 2 * r * child_points,
                                        child_points);
    }
}

static void fftp_layout_to_leaf16(const int16_t *src, int16_t *dst, int points)
{
    fftp_layout_to_leaf16_recursive(src, 1, dst, points);
}

static inline int16_t *fftp_prepare_native_input(int points, int16_t *src)
{
#if FFT_SCALAR_PRELAYOUT_N2048
    if (points == 2048) {
        fftp_layout_to_leaf16(src, fftp_xtmp2048, 2048);
        return fftp_xtmp2048;
    }
#else
    (void)points;
#endif
    return src;
}

static void fftp_combine_radix2_forward(const int16_t *src, int16_t *dst, int child_points, const int16_t *tw)
{
    for (int k = 0; k < child_points; ++k) {
        fftp_cplx16_t a = fftp_load_cplx(src, k);
        fftp_cplx16_t b = fftp_load_cplx(src, child_points + k);
        fftp_cplx16_t bt = fftp_mul_q15(b, tw[2 * k], tw[2 * k + 1]);

        fftp_store_cplx(dst, k, fftp_add_sat(a, bt));
        fftp_store_cplx(dst, child_points + k, fftp_sub_sat(a, bt));
    }
}

static void fftp_combine_radix2_inverse(const int16_t *src, int16_t *dst, int child_points, const int16_t *tw)
{
    for (int k = 0; k < child_points; ++k) {
        fftp_cplx16_t a = fftp_load_cplx(src, k);
        fftp_cplx16_t b = fftp_load_cplx(src, child_points + k);
        fftp_cplx16_t bt = fftp_mul_q15(b, tw[2 * k], fftp_neg_wrap_i16(tw[2 * k + 1]));

        fftp_store_cplx(dst, k, fftp_add_sat(a, bt));
        fftp_store_cplx(dst, child_points + k, fftp_sub_sat(a, bt));
    }
}

static void fftp_combine_radix4_forward(const int16_t *src,
                                        int16_t *dst,
                                        int child_points,
                                        const int16_t *tw)
{
    for (int k = 0; k < child_points; ++k) {
        const int tw_base = 6 * k;
        fftp_cplx16_t x0 = fftp_load_cplx(src, k);
        fftp_cplx16_t x1 = fftp_load_cplx(src, child_points + k);
        fftp_cplx16_t x2 = fftp_load_cplx(src, 2 * child_points + k);
        fftp_cplx16_t x3 = fftp_load_cplx(src, 3 * child_points + k);
        fftp_cplx16_t x1t = fftp_mul_q15(x1, tw[tw_base + 0], tw[tw_base + 1]);
        fftp_cplx16_t x2t = fftp_mul_q15(x2, tw[tw_base + 2], tw[tw_base + 3]);
        fftp_cplx16_t x3t = fftp_mul_q15(x3, tw[tw_base + 4], tw[tw_base + 5]);
        fftp_cplx16_t x02 = fftp_add_sat(x0, x2t);
        fftp_cplx16_t x13 = fftp_add_sat(x1t, x3t);
        fftp_cplx16_t y0 = fftp_add_sat(x02, x13);
        fftp_cplx16_t y2 = fftp_sub_sat(x02, x13);
        fftp_cplx16_t x02d = fftp_sub_sat(x0, x2t);
        fftp_cplx16_t x13d = fftp_sub_sat(fftp_mul_minus_j(x1t), fftp_mul_minus_j(x3t));
        fftp_cplx16_t y1 = fftp_add_sat(x02d, x13d);
        fftp_cplx16_t y3 = fftp_sub_sat(x02d, x13d);

        fftp_store_cplx(dst, k, y0);
        fftp_store_cplx(dst, child_points + k, y1);
        fftp_store_cplx(dst, 2 * child_points + k, y2);
        fftp_store_cplx(dst, 3 * child_points + k, y3);
    }
}

static void fftp_combine_radix4_inverse(const int16_t *src,
                                        int16_t *dst,
                                        int child_points,
                                        const int16_t *tw)
{
    for (int k = 0; k < child_points; ++k) {
        const int tw_base = 6 * k;
        fftp_cplx16_t x0 = fftp_load_cplx(src, k);
        fftp_cplx16_t x1 = fftp_load_cplx(src, child_points + k);
        fftp_cplx16_t x2 = fftp_load_cplx(src, 2 * child_points + k);
        fftp_cplx16_t x3 = fftp_load_cplx(src, 3 * child_points + k);
        fftp_cplx16_t x1t = fftp_mul_q15(x1, tw[tw_base + 0], fftp_neg_wrap_i16(tw[tw_base + 1]));
        fftp_cplx16_t x2t = fftp_mul_q15(x2, tw[tw_base + 2], fftp_neg_wrap_i16(tw[tw_base + 3]));
        fftp_cplx16_t x3t = fftp_mul_q15(x3, tw[tw_base + 4], fftp_neg_wrap_i16(tw[tw_base + 5]));
        fftp_cplx16_t x02 = fftp_add_sat(x0, x2t);
        fftp_cplx16_t x13 = fftp_add_sat(x1t, x3t);
        fftp_cplx16_t y0 = fftp_add_sat(x02, x13);
        fftp_cplx16_t y2 = fftp_sub_sat(x02, x13);
        fftp_cplx16_t x02d = fftp_sub_sat(x0, x2t);
        fftp_cplx16_t x13d = fftp_sub_sat(fftp_mul_minus_j(x1t), fftp_mul_minus_j(x3t));
        fftp_cplx16_t y3 = fftp_add_sat(x02d, x13d);
        fftp_cplx16_t y1 = fftp_sub_sat(x02d, x13d);

        fftp_store_cplx(dst, k, y0);
        fftp_store_cplx(dst, child_points + k, y1);
        fftp_store_cplx(dst, 2 * child_points + k, y2);
        fftp_store_cplx(dst, 3 * child_points + k, y3);
    }
}

static FFT_NOINLINE void fftp_dft64_impl(int16_t *x, int16_t *y, unsigned char scale)
{
    int16_t *xtmp = fftp_xtmp64;
    int16_t *ytmp = fftp_ytmp64;

    fftp_split_radix4(x, xtmp, 16);
    FFT_POW2_BASE_DFT16(xtmp + 0 * 32, ytmp + 0 * 32);
    FFT_POW2_BASE_DFT16(xtmp + 1 * 32, ytmp + 1 * 32);
    FFT_POW2_BASE_DFT16(xtmp + 2 * 32, ytmp + 2 * 32);
    FFT_POW2_BASE_DFT16(xtmp + 3 * 32, ytmp + 3 * 32);
    fftp_combine_radix4_forward(ytmp, y, 16, fftp_tw64);

    if (scale > 0) {
        fftp_scale_shift(y, 64, 3);
    }
}

static FFT_NOINLINE void fftp_idft64_impl(int16_t *x, int16_t *y, unsigned char scale)
{
    int16_t *xtmp = fftp_xtmp64;
    int16_t *ytmp = fftp_ytmp64;

    fftp_split_radix4(x, xtmp, 16);
    FFT_POW2_BASE_IDFT16(xtmp + 0 * 32, ytmp + 0 * 32);
    FFT_POW2_BASE_IDFT16(xtmp + 1 * 32, ytmp + 1 * 32);
    FFT_POW2_BASE_IDFT16(xtmp + 2 * 32, ytmp + 2 * 32);
    FFT_POW2_BASE_IDFT16(xtmp + 3 * 32, ytmp + 3 * 32);
    fftp_combine_radix4_inverse(ytmp, y, 16, fftp_tw64);

    if (scale > 0) {
        fftp_scale_shift(y, 64, 3);
    }
}

static FFT_NOINLINE void fftp_dft64_from_leaf16(int16_t *x, int16_t *y, unsigned char scale)
{
    int16_t *ytmp = fftp_ytmp64;

    FFT_POW2_BASE_DFT16(x + 0 * 32, ytmp + 0 * 32);
    FFT_POW2_BASE_DFT16(x + 1 * 32, ytmp + 1 * 32);
    FFT_POW2_BASE_DFT16(x + 2 * 32, ytmp + 2 * 32);
    FFT_POW2_BASE_DFT16(x + 3 * 32, ytmp + 3 * 32);
    fftp_combine_radix4_forward(ytmp, y, 16, fftp_tw64);

    if (scale > 0) {
        fftp_scale_shift(y, 64, 3);
    }
}

static FFT_NOINLINE void fftp_idft64_from_leaf16(int16_t *x, int16_t *y, unsigned char scale)
{
    int16_t *ytmp = fftp_ytmp64;

    FFT_POW2_BASE_IDFT16(x + 0 * 32, ytmp + 0 * 32);
    FFT_POW2_BASE_IDFT16(x + 1 * 32, ytmp + 1 * 32);
    FFT_POW2_BASE_IDFT16(x + 2 * 32, ytmp + 2 * 32);
    FFT_POW2_BASE_IDFT16(x + 3 * 32, ytmp + 3 * 32);
    fftp_combine_radix4_inverse(ytmp, y, 16, fftp_tw64);

    if (scale > 0) {
        fftp_scale_shift(y, 64, 3);
    }
}

static FFT_NOINLINE void fftp_dft128_from_leaf16(int16_t *x, int16_t *y, unsigned char scale)
{
    fftp_dft64_from_leaf16(x + 0 * 128, y + 0 * 128, 1);
    fftp_dft64_from_leaf16(x + 1 * 128, y + 1 * 128, 1);
    fftp_combine_radix2_forward(y, y, 64, fftp_tw128);

    if (scale > 0) {
        fftp_scale_mulhrs(y, 128, FFTP_ONE_OVER_SQRT2_Q15);
    }
}

static FFT_NOINLINE void fftp_idft128_from_leaf16(int16_t *x, int16_t *y, unsigned char scale)
{
    fftp_idft64_from_leaf16(x + 0 * 128, y + 0 * 128, 1);
    fftp_idft64_from_leaf16(x + 1 * 128, y + 1 * 128, 1);
    fftp_combine_radix2_inverse(y, y, 64, fftp_tw128);

    if (scale > 0) {
        fftp_scale_mulhrs(y, 128, FFTP_ONE_OVER_SQRT2_Q15);
    }
}

static FFT_NOINLINE void fftp_dft256_from_leaf16(int16_t *x, int16_t *y, unsigned char scale)
{
    fftp_dft64_from_leaf16(x + 0 * 128, y + 0 * 128, 1);
    fftp_dft64_from_leaf16(x + 1 * 128, y + 1 * 128, 1);
    fftp_dft64_from_leaf16(x + 2 * 128, y + 2 * 128, 1);
    fftp_dft64_from_leaf16(x + 3 * 128, y + 3 * 128, 1);
    fftp_combine_radix4_forward(y, y, 64, fftp_tw256);

    if (scale > 0) {
        fftp_scale_shift(y, 256, 1);
    }
}

static FFT_NOINLINE void fftp_idft256_from_leaf16(int16_t *x, int16_t *y, unsigned char scale)
{
    fftp_idft64_from_leaf16(x + 0 * 128, y + 0 * 128, 1);
    fftp_idft64_from_leaf16(x + 1 * 128, y + 1 * 128, 1);
    fftp_idft64_from_leaf16(x + 2 * 128, y + 2 * 128, 1);
    fftp_idft64_from_leaf16(x + 3 * 128, y + 3 * 128, 1);
    fftp_combine_radix4_inverse(y, y, 64, fftp_tw256);

    if (scale > 0) {
        fftp_scale_shift(y, 256, 1);
    }
}

static FFT_NOINLINE void fftp_dft512_from_leaf16(int16_t *x, int16_t *y, unsigned char scale)
{
    fftp_dft256_from_leaf16(x + 0 * 512, y + 0 * 512, 1);
    fftp_dft256_from_leaf16(x + 1 * 512, y + 1 * 512, 1);
    fftp_combine_radix2_forward(y, y, 256, fftp_tw512);

    if (scale > 0) {
        fftp_scale_mulhrs(y, 512, FFTP_ONE_OVER_SQRT2_Q15);
    }
}

static FFT_NOINLINE void fftp_idft512_from_leaf16(int16_t *x, int16_t *y, unsigned char scale)
{
    fftp_idft256_from_leaf16(x + 0 * 512, y + 0 * 512, 1);
    fftp_idft256_from_leaf16(x + 1 * 512, y + 1 * 512, 1);
    fftp_combine_radix2_inverse(y, y, 256, fftp_tw512);

    if (scale > 0) {
        fftp_scale_mulhrs(y, 512, FFTP_ONE_OVER_SQRT2_Q15);
    }
}

static FFT_NOINLINE void fftp_dft1024_from_leaf16(int16_t *x, int16_t *y, unsigned char scale)
{
    fftp_dft256_from_leaf16(x + 0 * 512, y + 0 * 512, 1);
    fftp_dft256_from_leaf16(x + 1 * 512, y + 1 * 512, 1);
    fftp_dft256_from_leaf16(x + 2 * 512, y + 2 * 512, 1);
    fftp_dft256_from_leaf16(x + 3 * 512, y + 3 * 512, 1);
    fftp_combine_radix4_forward(y, y, 256, fftp_tw1024);

    if (scale > 0) {
        fftp_scale_mulhrs(y, 1024, FFTP_ONE_OVER_SQRT2_Q15);
    }
}

static FFT_NOINLINE void fftp_idft1024_from_leaf16(int16_t *x, int16_t *y, unsigned char scale)
{
    fftp_idft256_from_leaf16(x + 0 * 512, y + 0 * 512, 1);
    fftp_idft256_from_leaf16(x + 1 * 512, y + 1 * 512, 1);
    fftp_idft256_from_leaf16(x + 2 * 512, y + 2 * 512, 1);
    fftp_idft256_from_leaf16(x + 3 * 512, y + 3 * 512, 1);
    fftp_combine_radix4_inverse(y, y, 256, fftp_tw1024);

    if (scale > 0) {
        fftp_scale_mulhrs(y, 1024, FFTP_ONE_OVER_SQRT2_Q15);
    }
}

static FFT_NOINLINE void fftp_dft2048_from_leaf16(int16_t *x, int16_t *y, unsigned char scale)
{
    fftp_dft1024_from_leaf16(x + 0 * 2048, y + 0 * 2048, 1);
    fftp_dft1024_from_leaf16(x + 1 * 2048, y + 1 * 2048, 1);
    fftp_combine_radix2_forward(y, y, 1024, fftp_tw2048);

    if (scale > 0) {
        fftp_scale_mulhrs(y, 2048, FFTP_ONE_OVER_SQRT2_Q15);
    }
}

static FFT_NOINLINE void fftp_idft2048_from_leaf16(int16_t *x, int16_t *y, unsigned char scale)
{
    fftp_idft1024_from_leaf16(x + 0 * 2048, y + 0 * 2048, 1);
    fftp_idft1024_from_leaf16(x + 1 * 2048, y + 1 * 2048, 1);
    fftp_combine_radix2_inverse(y, y, 1024, fftp_tw2048);

    if (scale > 0) {
        fftp_scale_mulhrs(y, 2048, FFTP_ONE_OVER_SQRT2_Q15);
    }
}

static FFT_NOINLINE void fftp_dft4096_from_leaf16(int16_t *x, int16_t *y, unsigned char scale)
{
    fftp_dft1024_from_leaf16(x + 0 * 2048, y + 0 * 2048, 1);
    fftp_dft1024_from_leaf16(x + 1 * 2048, y + 1 * 2048, 1);
    fftp_dft1024_from_leaf16(x + 2 * 2048, y + 2 * 2048, 1);
    fftp_dft1024_from_leaf16(x + 3 * 2048, y + 3 * 2048, 1);
    fftp_combine_radix4_forward(y, y, 1024, fftp_tw4096);

    if (scale > 0) {
        fftp_scale_shift(y, 4096, 1);
    }
}

static FFT_NOINLINE void fftp_idft4096_from_leaf16(int16_t *x, int16_t *y, unsigned char scale)
{
    fftp_idft1024_from_leaf16(x + 0 * 2048, y + 0 * 2048, 1);
    fftp_idft1024_from_leaf16(x + 1 * 2048, y + 1 * 2048, 1);
    fftp_idft1024_from_leaf16(x + 2 * 2048, y + 2 * 2048, 1);
    fftp_idft1024_from_leaf16(x + 3 * 2048, y + 3 * 2048, 1);
    fftp_combine_radix4_inverse(y, y, 1024, fftp_tw4096);

    if (scale > 0) {
        fftp_scale_shift(y, 4096, 1);
    }
}

static FFT_NOINLINE void fftp_dft128_impl(int16_t *x, int16_t *y, unsigned char scale)
{
    int16_t *xtmp = fftp_xtmp128;

    fftp_layout_to_leaf16(x, xtmp, 128);
    fftp_dft128_from_leaf16(xtmp, y, scale);
}

static FFT_NOINLINE void fftp_idft128_impl(int16_t *x, int16_t *y, unsigned char scale)
{
    int16_t *xtmp = fftp_xtmp128;

    fftp_layout_to_leaf16(x, xtmp, 128);
    fftp_idft128_from_leaf16(xtmp, y, scale);
}

static FFT_NOINLINE void fftp_dft256_impl(int16_t *x, int16_t *y, unsigned char scale)
{
    int16_t *xtmp = fftp_xtmp256;

    fftp_layout_to_leaf16(x, xtmp, 256);
    fftp_dft256_from_leaf16(xtmp, y, scale);
}

static FFT_NOINLINE void fftp_idft256_impl(int16_t *x, int16_t *y, unsigned char scale)
{
    int16_t *xtmp = fftp_xtmp256;

    fftp_layout_to_leaf16(x, xtmp, 256);
    fftp_idft256_from_leaf16(xtmp, y, scale);
}

static FFT_NOINLINE void fftp_dft512_impl(int16_t *x, int16_t *y, unsigned char scale)
{
    int16_t *xtmp = fftp_xtmp512;

    fftp_layout_to_leaf16(x, xtmp, 512);
    fftp_dft512_from_leaf16(xtmp, y, scale);
}

static FFT_NOINLINE void fftp_idft512_impl(int16_t *x, int16_t *y, unsigned char scale)
{
    int16_t *xtmp = fftp_xtmp512;

    fftp_layout_to_leaf16(x, xtmp, 512);
    fftp_idft512_from_leaf16(xtmp, y, scale);
}

static FFT_NOINLINE void fftp_dft1024_impl(int16_t *x, int16_t *y, unsigned char scale)
{
    int16_t *xtmp = fftp_xtmp1024;

    fftp_layout_to_leaf16(x, xtmp, 1024);
    fftp_dft1024_from_leaf16(xtmp, y, scale);
}

static FFT_NOINLINE void fftp_idft1024_impl(int16_t *x, int16_t *y, unsigned char scale)
{
    int16_t *xtmp = fftp_xtmp1024;

    fftp_layout_to_leaf16(x, xtmp, 1024);
    fftp_idft1024_from_leaf16(xtmp, y, scale);
}

static FFT_NOINLINE void fftp_dft2048_impl(int16_t *x, int16_t *y, unsigned char scale)
{
    int16_t *xtmp = fftp_xtmp2048;

    fftp_layout_to_leaf16(x, xtmp, 2048);
    fftp_dft2048_from_leaf16(xtmp, y, scale);
}

static FFT_NOINLINE void fftp_idft2048_impl(int16_t *x, int16_t *y, unsigned char scale)
{
    int16_t *xtmp = fftp_xtmp2048;

    fftp_layout_to_leaf16(x, xtmp, 2048);
    fftp_idft2048_from_leaf16(xtmp, y, scale);
}

static FFT_NOINLINE void fftp_dft4096_impl(int16_t *x, int16_t *y, unsigned char scale)
{
    int16_t *xtmp = fftp_xtmp4096;

    fftp_layout_to_leaf16(x, xtmp, 4096);
    fftp_dft4096_from_leaf16(xtmp, y, scale);
}

static FFT_NOINLINE void fftp_idft4096_impl(int16_t *x, int16_t *y, unsigned char scale)
{
    int16_t *xtmp = fftp_xtmp4096;

    fftp_layout_to_leaf16(x, xtmp, 4096);
    fftp_idft4096_from_leaf16(xtmp, y, scale);
}

static int fft_pow2_supported_points(int points)
{
    switch (points) {
    case 16:
    case 64:
    case 128:
    case 256:
    case 512:
    case 1024:
    case 2048:
    case 4096:
        return 1;
    default:
        return 0;
    }
}

static int fft_pow2_run_forward(int points, int16_t *x, int16_t *y)
{
    fftp_ensure_tables();

    switch (points) {
    case 16:
        FFT_POW2_BASE_DFT16(x, y);
        return 1;
    case 64:
        fftp_dft64_impl(x, y, 0);
        return 1;
    case 128:
        fftp_dft128_impl(x, y, 0);
        return 1;
    case 256:
        fftp_dft256_impl(x, y, 0);
        return 1;
    case 512:
        fftp_dft512_impl(x, y, 0);
        return 1;
    case 1024:
        fftp_dft1024_impl(x, y, 0);
        return 1;
    case 2048:
        fftp_dft2048_impl(x, y, 0);
        return 1;
    case 4096:
        fftp_dft4096_impl(x, y, 0);
        return 1;
    default:
        return 0;
    }
}

static int fft_pow2_run_inverse(int points, int16_t *x, int16_t *y)
{
    fftp_ensure_tables();

    switch (points) {
    case 16:
        FFT_POW2_BASE_IDFT16(x, y);
        return 1;
    case 64:
        fftp_idft64_impl(x, y, 0);
        return 1;
    case 128:
        fftp_idft128_impl(x, y, 0);
        return 1;
    case 256:
        fftp_idft256_impl(x, y, 0);
        return 1;
    case 512:
        fftp_idft512_impl(x, y, 0);
        return 1;
    case 1024:
        fftp_idft1024_impl(x, y, 0);
        return 1;
    case 2048:
        fftp_idft2048_impl(x, y, 0);
        return 1;
    case 4096:
        fftp_idft4096_impl(x, y, 0);
        return 1;
    default:
        return 0;
    }
}

static int fftp_run_forward_native(int points, int16_t *x, int16_t *y)
{
#if FFT_SCALAR_PRELAYOUT_N2048
    if (points == 2048) {
        fftp_dft2048_from_leaf16(x, y, 0);
        return 1;
    }
#else
    (void)x;
#endif
    return fft_pow2_run_forward(points, x, y);
}

static int fftp_run_inverse_native(int points, int16_t *x, int16_t *y)
{
#if FFT_SCALAR_PRELAYOUT_N2048
    if (points == 2048) {
        fftp_idft2048_from_leaf16(x, y, 0);
        return 1;
    }
#else
    (void)x;
#endif
    return fft_pow2_run_inverse(points, x, y);
}

#endif
