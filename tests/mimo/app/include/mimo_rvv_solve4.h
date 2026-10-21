#ifndef MIMO_RVV_SOLVE4_H
#define MIMO_RVV_SOLVE4_H

#include <riscv_vector.h>

#include "mimo_kernels.h"

#ifndef MIMO_USE_RVV_SOLVE4
#define MIMO_USE_RVV_SOLVE4 0
#endif

#ifndef MIMO_USE_RVV_SOLVE4_STREAMED
#define MIMO_USE_RVV_SOLVE4_STREAMED 0
#endif

#ifndef MIMO_USE_RVV_SOLVE4_VNCLIP_ROUND
#define MIMO_USE_RVV_SOLVE4_VNCLIP_ROUND 0
#endif

#ifndef MIMO_USE_RVV_SOLVE4_Q15_I32_MUL
#define MIMO_USE_RVV_SOLVE4_Q15_I32_MUL 0
#endif

#ifndef MIMO_USE_RVV_ZF4_SCHUR_RVV_SOLVE
#define MIMO_USE_RVV_ZF4_SCHUR_RVV_SOLVE 0
#endif

#ifndef MIMO_USE_RVV_MMSE4_SCHUR_RVV_SOLVE
#define MIMO_USE_RVV_MMSE4_SCHUR_RVV_SOLVE 0
#endif

#ifndef MIMO_USE_EXT_MMSE4_SCHUR_RVV_SOLVE
#define MIMO_USE_EXT_MMSE4_SCHUR_RVV_SOLVE 0
#endif

#ifndef MIMO_USE_RVV_ZF4_FUSED_BUILD_SOLVE
#define MIMO_USE_RVV_ZF4_FUSED_BUILD_SOLVE 0
#endif

#ifndef MIMO_USE_RVV_ZF4_PACK32_STORE
#define MIMO_USE_RVV_ZF4_PACK32_STORE 0
#endif

#ifndef MIMO_USE_MMSE4_SCHUR_NO_MAG_SCALE
#define MIMO_USE_MMSE4_SCHUR_NO_MAG_SCALE 0
#endif

#ifndef MIMO_USE_MMSE4_SCHUR_SIGNED_TRUNC_SCALE
#define MIMO_USE_MMSE4_SCHUR_SIGNED_TRUNC_SCALE 0
#endif

#ifndef MIMO_USE_MMSE4_SCHUR_NUM_ARSHIFT_SCALE
#define MIMO_USE_MMSE4_SCHUR_NUM_ARSHIFT_SCALE 0
#endif

#ifndef MIMO_USE_MMSE4_SCHUR_INNER_Q30_ONCE
#define MIMO_USE_MMSE4_SCHUR_INNER_Q30_ONCE 0
#endif

#ifndef MIMO_USE_RVV_MMSE4_SCHUR_INNER_SYM_ROUND
#define MIMO_USE_RVV_MMSE4_SCHUR_INNER_SYM_ROUND 0
#endif

#ifndef MIMO_USE_RVV_MMSE4_SCHUR_TOP_Q30_ONCE
#define MIMO_USE_RVV_MMSE4_SCHUR_TOP_Q30_ONCE 0
#endif

#ifndef MIMO_USE_RVV_MMSE4_BINV_Q13_REUSE
#define MIMO_USE_RVV_MMSE4_BINV_Q13_REUSE 0
#endif

#ifndef MIMO_USE_RVV_SCHUR4_SOLVE2_PAIR_M2
#define MIMO_USE_RVV_SCHUR4_SOLVE2_PAIR_M2 0
#endif

#ifndef MIMO_USE_RVV_SCHUR4_VSMUL_SCALE
#define MIMO_USE_RVV_SCHUR4_VSMUL_SCALE 0
#endif

#ifndef MIMO_USE_RVV_SCHUR4_NUM_MAC
#define MIMO_USE_RVV_SCHUR4_NUM_MAC 0
#endif

#ifndef MIMO_USE_RVV_SCHUR4_COMPLEX_MAC
#define MIMO_USE_RVV_SCHUR4_COMPLEX_MAC 0
#endif

#ifndef MIMO_USE_RVV_SCHUR4_VSSRA_ROUND
#define MIMO_USE_RVV_SCHUR4_VSSRA_ROUND 0
#endif

#ifndef MIMO_USE_RVV_SCHUR4_DET_MAC
#define MIMO_USE_RVV_SCHUR4_DET_MAC 0
#endif

#ifndef MIMO_USE_RVV_MMSE4_SCHUR_TRUE16B_SOLVE
#define MIMO_USE_RVV_MMSE4_SCHUR_TRUE16B_SOLVE 0
#endif

#ifndef MIMO_USE_RVV_MMSE4_SCHUR_TRUE16B_SHARED_A16
#define MIMO_USE_RVV_MMSE4_SCHUR_TRUE16B_SHARED_A16 0
#endif

#if MIMO_USE_MMSE4_SCHUR_SIGNED_TRUNC_SCALE && !MIMO_USE_MMSE4_SCHUR_NO_MAG_SCALE
#error "MIMO_USE_MMSE4_SCHUR_SIGNED_TRUNC_SCALE requires MIMO_USE_MMSE4_SCHUR_NO_MAG_SCALE=1"
#endif

#if MIMO_USE_RVV_MMSE4_BINV_Q13_REUSE && \
    (!MIMO_USE_RVV_MMSE4_SCHUR_RVV_SOLVE || \
     !MIMO_USE_MMSE4_SCHUR_NO_MAG_SCALE || \
     MIMO_USE_MMSE4_SCHUR_SIGNED_TRUNC_SCALE || \
     !MIMO_USE_RVV_SCHUR4_VSSRA_ROUND || \
     MIMO_USE_RVV_SCHUR4_VSMUL_SCALE)
#error "MMSE4 RVV Q13 B-inverse reuse requires the no-mag vssra ordinary-scale Schur path"
#endif

#define MIMO_MMSE4_SCHUR_SCALE_ARG(mmse_) \
    ((mmse_) ? \
         (MIMO_USE_MMSE4_SCHUR_NO_MAG_SCALE ? \
              (MIMO_USE_MMSE4_SCHUR_SIGNED_TRUNC_SCALE ? 2 : 0) : 1) : \
         0)

#ifndef MIMO_USE_SITE4_SOLVE_SOA
#define MIMO_USE_SITE4_SOLVE_SOA 0
#endif

#ifndef MIMO_USE_SITE4_HERMITIAN_UPPER_BUILD
#define MIMO_USE_SITE4_HERMITIAN_UPPER_BUILD 0
#endif

#ifndef MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED
#define MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED 0
#endif

#ifndef MIMO_USE_SOLVE4_SCHUR
#define MIMO_USE_SOLVE4_SCHUR 0
#endif

#ifndef MIMO_USE_SOLVE4_SCHUR_Q15_RECIP
#define MIMO_USE_SOLVE4_SCHUR_Q15_RECIP 0
#endif

#ifndef MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT
#define MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT 15
#endif

#ifndef MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
#define MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE 0
#endif

#if (MIMO_USE_RVV_ZF4_SCHUR_RVV_SOLVE || \
     MIMO_USE_RVV_MMSE4_SCHUR_RVV_SOLVE || \
     MIMO_USE_EXT_MMSE4_SCHUR_RVV_SOLVE) && \
    (!MIMO_USE_SITE4_SOLVE_SOA || !MIMO_USE_SITE4_HERMITIAN_UPPER_BUILD || \
     !MIMO_USE_SOLVE4_SCHUR || MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED || \
     MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 5 || \
     MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT != 13)
#error "RVV Schur RVV solve requires unpacked SoA upper-build Schur q15recip mode5 num_shift=13"
#endif

static MIMO_ALWAYS_INLINE vint32m1_t mimo_rvv_solve4_load_soa_a_re(
    const mimo_site4_soa4_t *work,
    int idx,
    size_t vl)
{
    return __riscv_vle32_v_i32m1(&work->a_re[idx][0], vl);
}

static MIMO_ALWAYS_INLINE vint32m1_t mimo_rvv_solve4_load_soa_a_im(
    const mimo_site4_soa4_t *work,
    int idx,
    size_t vl)
{
    return __riscv_vle32_v_i32m1(&work->a_im[idx][0], vl);
}

static MIMO_ALWAYS_INLINE vint32m1_t mimo_rvv_solve4_load_soa_b_re(
    const mimo_site4_soa4_t *work,
    int idx,
    size_t vl)
{
    return __riscv_vle32_v_i32m1(&work->b_re[idx][0], vl);
}

static MIMO_ALWAYS_INLINE vint32m1_t mimo_rvv_solve4_load_soa_b_im(
    const mimo_site4_soa4_t *work,
    int idx,
    size_t vl)
{
    return __riscv_vle32_v_i32m1(&work->b_im[idx][0], vl);
}

static MIMO_ALWAYS_INLINE vint32m1_t mimo_rvv_solve4_load_a_re(
    const mimo_c32_t a_lane[MIMO_SITE4_LANES][MIMO_N4 * MIMO_N4],
    int idx,
    ptrdiff_t stride,
    size_t vl)
{
    return __riscv_vlse32_v_i32m1(&a_lane[0][idx].r, stride, vl);
}

static MIMO_ALWAYS_INLINE vint32m1_t mimo_rvv_solve4_load_a_im(
    const mimo_c32_t a_lane[MIMO_SITE4_LANES][MIMO_N4 * MIMO_N4],
    int idx,
    ptrdiff_t stride,
    size_t vl)
{
    return __riscv_vlse32_v_i32m1(&a_lane[0][idx].i, stride, vl);
}

static MIMO_ALWAYS_INLINE void mimo_rvv_solve4_store_a(
    mimo_c32_t a_lane[MIMO_SITE4_LANES][MIMO_N4 * MIMO_N4],
    int idx,
    vint32m1_t re,
    vint32m1_t im,
    ptrdiff_t stride,
    size_t vl)
{
    __riscv_vsse32_v_i32m1(&a_lane[0][idx].r, stride, re, vl);
    __riscv_vsse32_v_i32m1(&a_lane[0][idx].i, stride, im, vl);
}

static MIMO_ALWAYS_INLINE vint32m1_t mimo_rvv_solve4_load_b_re(
    const mimo_c32_t b_lane[MIMO_SITE4_LANES][MIMO_N4],
    int idx,
    ptrdiff_t stride,
    size_t vl)
{
    return __riscv_vlse32_v_i32m1(&b_lane[0][idx].r, stride, vl);
}

static MIMO_ALWAYS_INLINE vint32m1_t mimo_rvv_solve4_load_b_im(
    const mimo_c32_t b_lane[MIMO_SITE4_LANES][MIMO_N4],
    int idx,
    ptrdiff_t stride,
    size_t vl)
{
    return __riscv_vlse32_v_i32m1(&b_lane[0][idx].i, stride, vl);
}

static MIMO_ALWAYS_INLINE void mimo_rvv_solve4_store_b(
    mimo_c32_t b_lane[MIMO_SITE4_LANES][MIMO_N4],
    int idx,
    vint32m1_t re,
    vint32m1_t im,
    ptrdiff_t stride,
    size_t vl)
{
    __riscv_vsse32_v_i32m1(&b_lane[0][idx].r, stride, re, vl);
    __riscv_vsse32_v_i32m1(&b_lane[0][idx].i, stride, im, vl);
}

static MIMO_ALWAYS_INLINE void mimo_rvv_solve4_store_x(
    mimo_c16_t *x_block,
    int tx,
    vint32m1_t re,
    vint32m1_t im,
    ptrdiff_t x_stride,
    size_t vl);

static MIMO_ALWAYS_INLINE vint64m2_t mimo_rvv_solve4_safe_pivot_i64(
    vint32m1_t pivot32,
    size_t vl)
{
    vint64m2_t pivot = __riscv_vsext_vf2_i64m2(pivot32, vl);
    const vbool32_t nonzero = __riscv_vmsne_vx_i64m2_b32(pivot, 0, vl);
    const vint64m2_t one = __riscv_vmv_v_x_i64m2(1, vl);

    return __riscv_vmerge_vvm_i64m2(one, pivot, nonzero, vl);
}

static MIMO_ALWAYS_INLINE vint32m1_t mimo_rvv_solve4_round_shift_i64_to_i32(
    vint64m2_t x,
    int shift,
    int64_t round,
    size_t vl)
{
#if MIMO_USE_RVV_SOLVE4_VNCLIP_ROUND
    (void)round;
    const vint64m2_t sign = __riscv_vsra_vx_i64m2(x, 63, vl);
    vint64m2_t mag = __riscv_vxor_vv_i64m2(x, sign, vl);
    const vbool32_t neg_mask = __riscv_vmsne_vx_i64m2_b32(sign, 0, vl);

    mag = __riscv_vsub_vv_i64m2(mag, sign, vl);
    const vint32m1_t clipped =
        __riscv_vnclip_wx_i32m1(mag, (uint8_t)shift, 0, vl);
    const vint32m1_t zero32 = __riscv_vmv_v_x_i32m1(0, vl);
    const vint32m1_t neg = __riscv_vsub_vv_i32m1(zero32, clipped, vl);

    return __riscv_vmerge_vvm_i32m1(clipped, neg, neg_mask, vl);
#else
    const vint64m2_t sign = __riscv_vsra_vx_i64m2(x, 63, vl);
    vint64m2_t mag = __riscv_vxor_vv_i64m2(x, sign, vl);

    mag = __riscv_vsub_vv_i64m2(mag, sign, vl);
    mag = __riscv_vadd_vx_i64m2(mag, round, vl);
    mag = __riscv_vsra_vx_i64m2(mag, shift, vl);
    mag = __riscv_vxor_vv_i64m2(mag, sign, vl);
    mag = __riscv_vsub_vv_i64m2(mag, sign, vl);

    return __riscv_vncvt_x_x_w_i32m1(mag, vl);
#endif
}

static MIMO_ALWAYS_INLINE vint32m1_t
mimo_rvv_solve4_round_shift_i32_to_i32(vint32m1_t x,
                                       int shift,
                                       int32_t round,
                                       size_t vl)
{
    const vint32m1_t sign = __riscv_vsra_vx_i32m1(x, 31, vl);
    vint32m1_t mag = __riscv_vxor_vv_i32m1(x, sign, vl);

    mag = __riscv_vsub_vv_i32m1(mag, sign, vl);
    mag = __riscv_vadd_vx_i32m1(mag, round, vl);
    mag = __riscv_vsra_vx_i32m1(mag, shift, vl);
    mag = __riscv_vxor_vv_i32m1(mag, sign, vl);
    return __riscv_vsub_vv_i32m1(mag, sign, vl);
}

static MIMO_ALWAYS_INLINE vint32m1_t mimo_rvv_solve4_div_q30(
    vint32m1_t num32,
    vint64m2_t pivot,
    size_t vl)
{
    vint64m2_t num = __riscv_vsext_vf2_i64m2(num32, vl);

    num = __riscv_vsll_vx_i64m2(num, 30, vl);
    return __riscv_vncvt_x_x_w_i32m1(
        __riscv_vdiv_vv_i64m2(num, pivot, vl), vl);
}

static MIMO_ALWAYS_INLINE vint32m1_t mimo_rvv_solve4_div_q15(
    vint32m1_t num32,
    vint64m2_t pivot,
    size_t vl)
{
    vint64m2_t num = __riscv_vsext_vf2_i64m2(num32, vl);

    num = __riscv_vsll_vx_i64m2(num, MIMO_Q15_SHIFT, vl);
    return __riscv_vncvt_x_x_w_i32m1(
        __riscv_vdiv_vv_i64m2(num, pivot, vl), vl);
}

static MIMO_ALWAYS_INLINE void mimo_rvv_solve4_cmul_q30_q15(
    vint32m1_t ar_q30,
    vint32m1_t ai_q30,
    vint32m1_t br_q15,
    vint32m1_t bi_q15,
    vint32m1_t *out_re,
    vint32m1_t *out_im,
    size_t vl)
{
    vint64m2_t re = __riscv_vwmul_vv_i64m2(ar_q30, br_q15, vl);
    vint64m2_t tmp = __riscv_vwmul_vv_i64m2(ai_q30, bi_q15, vl);

    re = __riscv_vsub_vv_i64m2(re, tmp, vl);

    vint64m2_t im = __riscv_vwmul_vv_i64m2(ar_q30, bi_q15, vl);
    tmp = __riscv_vwmul_vv_i64m2(ai_q30, br_q15, vl);
    im = __riscv_vadd_vv_i64m2(im, tmp, vl);

    *out_re = mimo_rvv_solve4_round_shift_i64_to_i32(
        re, 30, 1LL << 29, vl);
    *out_im = mimo_rvv_solve4_round_shift_i64_to_i32(
        im, 30, 1LL << 29, vl);
}

static MIMO_ALWAYS_INLINE void mimo_rvv_solve4_cmul_q15(
    vint32m1_t ar_q15,
    vint32m1_t ai_q15,
    vint32m1_t br_q15,
    vint32m1_t bi_q15,
    vint32m1_t *out_re,
    vint32m1_t *out_im,
    size_t vl)
{
#if MIMO_USE_RVV_SOLVE4_Q15_I32_MUL
    vint32m1_t re = __riscv_vmul_vv_i32m1(ar_q15, br_q15, vl);
    vint32m1_t tmp = __riscv_vmul_vv_i32m1(ai_q15, bi_q15, vl);

    re = __riscv_vsub_vv_i32m1(re, tmp, vl);

    vint32m1_t im = __riscv_vmul_vv_i32m1(ar_q15, bi_q15, vl);
    tmp = __riscv_vmul_vv_i32m1(ai_q15, br_q15, vl);
    im = __riscv_vadd_vv_i32m1(im, tmp, vl);

    *out_re = mimo_rvv_solve4_round_shift_i32_to_i32(
        re, MIMO_Q15_SHIFT, MIMO_Q15_ROUND, vl);
    *out_im = mimo_rvv_solve4_round_shift_i32_to_i32(
        im, MIMO_Q15_SHIFT, MIMO_Q15_ROUND, vl);
#else
    vint64m2_t re = __riscv_vwmul_vv_i64m2(ar_q15, br_q15, vl);
    vint64m2_t tmp = __riscv_vwmul_vv_i64m2(ai_q15, bi_q15, vl);

    re = __riscv_vsub_vv_i64m2(re, tmp, vl);

    vint64m2_t im = __riscv_vwmul_vv_i64m2(ar_q15, bi_q15, vl);
    tmp = __riscv_vwmul_vv_i64m2(ai_q15, br_q15, vl);
    im = __riscv_vadd_vv_i64m2(im, tmp, vl);

    *out_re = mimo_rvv_solve4_round_shift_i64_to_i32(
        re, MIMO_Q15_SHIFT, MIMO_Q15_ROUND, vl);
    *out_im = mimo_rvv_solve4_round_shift_i64_to_i32(
        im, MIMO_Q15_SHIFT, MIMO_Q15_ROUND, vl);
#endif
}

static MIMO_ALWAYS_INLINE void mimo_rvv_solve4_submul_q30_q15(
    vint32m1_t *dst_re,
    vint32m1_t *dst_im,
    vint32m1_t src_re,
    vint32m1_t src_im,
    vint32m1_t factor_re,
    vint32m1_t factor_im,
    size_t vl)
{
    vint32m1_t prod_re;
    vint32m1_t prod_im;

    mimo_rvv_solve4_cmul_q30_q15(
        factor_re, factor_im, src_re, src_im, &prod_re, &prod_im, vl);
    *dst_re = __riscv_vsub_vv_i32m1(*dst_re, prod_re, vl);
    *dst_im = __riscv_vsub_vv_i32m1(*dst_im, prod_im, vl);
}

static MIMO_ALWAYS_INLINE void mimo_rvv_solve4_submul_q15(
    vint32m1_t *dst_re,
    vint32m1_t *dst_im,
    vint32m1_t src_re,
    vint32m1_t src_im,
    vint32m1_t factor_re,
    vint32m1_t factor_im,
    size_t vl)
{
    vint32m1_t prod_re;
    vint32m1_t prod_im;

    mimo_rvv_solve4_cmul_q15(
        factor_re, factor_im, src_re, src_im, &prod_re, &prod_im, vl);
    *dst_re = __riscv_vsub_vv_i32m1(*dst_re, prod_re, vl);
    *dst_im = __riscv_vsub_vv_i32m1(*dst_im, prod_im, vl);
}

static MIMO_ALWAYS_INLINE void mimo_rvv_solve4_submul_q30_q15_real(
    vint32m1_t *dst_re,
    vint32m1_t src_re,
    vint32m1_t src_im,
    vint32m1_t factor_re,
    vint32m1_t factor_im,
    size_t vl)
{
    vint32m1_t prod_re;
    vint32m1_t prod_im;

    mimo_rvv_solve4_cmul_q30_q15(
        factor_re, factor_im, src_re, src_im, &prod_re, &prod_im, vl);
    (void)prod_im;
    *dst_re = __riscv_vsub_vv_i32m1(*dst_re, prod_re, vl);
}

#if MIMO_USE_RVV_ZF4_SCHUR_RVV_SOLVE || \
    MIMO_USE_RVV_MMSE4_SCHUR_RVV_SOLVE || \
    MIMO_USE_EXT_MMSE4_SCHUR_RVV_SOLVE
#define MIMO_RVV_SCHUR4_DET_Q15_RECIP_MIN 3400
#define MIMO_SCHUR4_DET_Q15_RECIP_DIRECT1_ENTRY(rel_) \
    ((int32_t)(1073741824ull / \
               (MIMO_RVV_SCHUR4_DET_Q15_RECIP_MIN + (uint32_t)(rel_))))

static const int32_t mimo_rvv_schur4_det_recip_direct1_q30[] = {
#include "../kernels/common/mimo_schur_recip_direct1.inc"
};

#undef MIMO_SCHUR4_DET_Q15_RECIP_DIRECT1_ENTRY

static MIMO_ALWAYS_INLINE vint32m1_t mimo_rvv_schur4_round_q30_i32_to_q15(
    vint32m1_t x,
    size_t vl)
{
#if MIMO_USE_RVV_SCHUR4_VSSRA_ROUND
    return __riscv_vssra_vx_i32m1(x, MIMO_Q15_SHIFT, 0, vl);
#else
    x = __riscv_vadd_vx_i32m1(x, MIMO_Q15_ROUND, vl);
    return __riscv_vsra_vx_i32m1(x, MIMO_Q15_SHIFT, vl);
#endif
}

static MIMO_ALWAYS_INLINE vint32m1_t
mimo_rvv_schur4_round_q30_i32_to_q15_sym(vint32m1_t x, size_t vl)
{
    const vint32m1_t sign = __riscv_vsra_vx_i32m1(x, 31, vl);
    vint32m1_t mag = __riscv_vxor_vv_i32m1(x, sign, vl);

    mag = __riscv_vsub_vv_i32m1(mag, sign, vl);
    mag = __riscv_vadd_vx_i32m1(mag, MIMO_Q15_ROUND, vl);
    vint32m1_t y = __riscv_vsra_vx_i32m1(mag, MIMO_Q15_SHIFT, vl);

    y = __riscv_vxor_vv_i32m1(y, sign, vl);
    return __riscv_vsub_vv_i32m1(y, sign, vl);
}

static MIMO_ALWAYS_INLINE vint32m1_t mimo_rvv_schur4_det_q30_i32(
    vint32m1_t a00r,
    vint32m1_t a01r,
    vint32m1_t a01i,
    vint32m1_t a11r,
    size_t vl)
{
#if MIMO_USE_RVV_SCHUR4_DET_MAC
    vint32m1_t det = __riscv_vmul_vv_i32m1(a00r, a11r, vl);

    det = __riscv_vnmsac_vv_i32m1(det, a01r, a01r, vl);
    return __riscv_vnmsac_vv_i32m1(det, a01i, a01i, vl);
#else
    vint32m1_t det = __riscv_vmul_vv_i32m1(a00r, a11r, vl);
    vint32m1_t tmp = __riscv_vmul_vv_i32m1(a01r, a01r, vl);

    det = __riscv_vsub_vv_i32m1(det, tmp, vl);
    tmp = __riscv_vmul_vv_i32m1(a01i, a01i, vl);
    return __riscv_vsub_vv_i32m1(det, tmp, vl);
#endif
}

static MIMO_ALWAYS_INLINE vint32m1_t
mimo_rvv_schur4_scale_from_det_q30_i32(vint32m1_t den_q30, size_t vl)
{
#if MIMO_USE_RVV_SCHUR4_VSSRA_ROUND
    const vint32m1_t den_q15 =
        __riscv_vssra_vx_i32m1(den_q30, MIMO_Q15_SHIFT, 0, vl);
#else
    den_q30 = __riscv_vadd_vx_i32m1(den_q30, MIMO_Q15_ROUND, vl);
    const vint32m1_t den_q15 =
        __riscv_vsra_vx_i32m1(den_q30, MIMO_Q15_SHIFT, vl);
#endif
    const vint32m1_t rel = __riscv_vsub_vx_i32m1(
        den_q15, MIMO_RVV_SCHUR4_DET_Q15_RECIP_MIN, vl);
    const vuint32m1_t idx = __riscv_vsll_vx_u32m1(
        __riscv_vreinterpret_v_i32m1_u32m1(rel), 2, vl);

    return __riscv_vluxei32_v_i32m1(
        mimo_rvv_schur4_det_recip_direct1_q30, idx, vl);
}

static MIMO_ALWAYS_INLINE vint32m1_t mimo_rvv_schur4_scale_q30_to_q15_i32(
    vint32m1_t num_q30,
    vint32m1_t scale_q30,
    size_t vl)
{
#if MIMO_USE_RVV_SCHUR4_VSMUL_SCALE
    const vint32m1_t scale_q31 =
        __riscv_vsll_vx_i32m1(scale_q30, 1, vl);

    return __riscv_vsmul_vv_i32m1(num_q30, scale_q31, 0, vl);
#else
#if !MIMO_USE_MMSE4_SCHUR_NUM_ARSHIFT_SCALE
    num_q30 = __riscv_vadd_vx_i32m1(
        num_q30, 1 << (MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT - 1), vl);
#endif
    const vint32m1_t num_qs = __riscv_vsra_vx_i32m1(
        num_q30, MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT, vl);
    const vint32m1_t prod =
        __riscv_vmul_vv_i32m1(num_qs, scale_q30, vl);

    return __riscv_vsra_vx_i32m1(
        prod, 30 - MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT, vl);
#endif
}

static MIMO_ALWAYS_INLINE vint32m1_t
mimo_rvv_schur4_scale_q30_to_q15_i32_trunc(
    vint32m1_t num_q30,
    vint32m1_t scale_q30,
    size_t vl)
{
    num_q30 = __riscv_vadd_vx_i32m1(
        num_q30, 1 << (MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT - 1), vl);
    const vint32m1_t num_qs = __riscv_vsra_vx_i32m1(
        num_q30, MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT, vl);
    vint32m1_t prod =
        __riscv_vmul_vv_i32m1(num_qs, scale_q30, vl);
    const int32_t final_bias =
        (1 << (30 - MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT)) - 1;
    const vint32m1_t neg_bias = __riscv_vand_vx_i32m1(
        __riscv_vsra_vx_i32m1(prod, 31, vl), final_bias, vl);

    prod = __riscv_vadd_vv_i32m1(prod, neg_bias, vl);
    return __riscv_vsra_vx_i32m1(
        prod, 30 - MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT, vl);
}

static MIMO_ALWAYS_INLINE vint32m1_t
mimo_rvv_schur4_scale_q30_to_q15_i32_mag(
    vint32m1_t num_q30,
    vint32m1_t scale_q30,
    size_t vl)
{
    const vint32m1_t sign = __riscv_vsra_vx_i32m1(num_q30, 31, vl);
    vint32m1_t mag = __riscv_vxor_vv_i32m1(num_q30, sign, vl);

    mag = __riscv_vsub_vv_i32m1(mag, sign, vl);
    mag = __riscv_vadd_vx_i32m1(
        mag, 1 << (MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT - 1), vl);
    const vint32m1_t num_qs = __riscv_vsra_vx_i32m1(
        mag, MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT, vl);
    const vint32m1_t prod =
        __riscv_vmul_vv_i32m1(num_qs, scale_q30, vl);
    vint32m1_t mag_q15 = __riscv_vsra_vx_i32m1(
        prod, 30 - MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT, vl);

    mag_q15 = __riscv_vxor_vv_i32m1(mag_q15, sign, vl);
    return __riscv_vsub_vv_i32m1(mag_q15, sign, vl);
}

#if MIMO_USE_RVV_MMSE4_BINV_Q13_REUSE
static MIMO_ALWAYS_INLINE vint32m1_t
mimo_rvv_schur4_inv2_coeff_q13_i32(
    vint32m1_t a_q15,
    vint32m1_t scale_q30,
    size_t vl)
{
    const vint32m1_t scale_q26 =
        __riscv_vsra_vx_i32m1(scale_q30, 4, vl);
    const vint32m1_t prod_q41 =
        __riscv_vmul_vv_i32m1(a_q15, scale_q26, vl);

    return __riscv_vssra_vx_i32m1(prod_q41, 13, 0, vl);
}

static MIMO_ALWAYS_INLINE vint32m1_t
mimo_rvv_schur4_round_q28_i32_to_q15(vint32m1_t x, size_t vl)
{
    return __riscv_vssra_vx_i32m1(x, 13, 0, vl);
}

static MIMO_ALWAYS_INLINE void mimo_rvv_schur4_apply_inv2_q13_i32(
    vint32m1_t inv00,
    vint32m1_t inv01r,
    vint32m1_t inv01i,
    vint32m1_t inv11,
    vint32m1_t v0r,
    vint32m1_t v0i,
    vint32m1_t v1r,
    vint32m1_t v1i,
    vint32m1_t *out0r,
    vint32m1_t *out0i,
    vint32m1_t *out1r,
    vint32m1_t *out1i,
    size_t vl)
{
    vint32m1_t num0r = __riscv_vmul_vv_i32m1(inv00, v0r, vl);
    vint32m1_t num0i = __riscv_vmul_vv_i32m1(inv00, v0i, vl);
    vint32m1_t num1r = __riscv_vmul_vv_i32m1(inv01r, v0r, vl);
    vint32m1_t num1i = __riscv_vmul_vv_i32m1(inv01r, v0i, vl);

    num0r = __riscv_vmacc_vv_i32m1(num0r, inv01r, v1r, vl);
    num0r = __riscv_vnmsac_vv_i32m1(num0r, inv01i, v1i, vl);
    num0i = __riscv_vmacc_vv_i32m1(num0i, inv01r, v1i, vl);
    num0i = __riscv_vmacc_vv_i32m1(num0i, inv01i, v1r, vl);
    num1r = __riscv_vmacc_vv_i32m1(num1r, inv01i, v0i, vl);
    num1r = __riscv_vmacc_vv_i32m1(num1r, inv11, v1r, vl);
    num1i = __riscv_vnmsac_vv_i32m1(num1i, inv01i, v0r, vl);
    num1i = __riscv_vmacc_vv_i32m1(num1i, inv11, v1i, vl);

    *out0r = mimo_rvv_schur4_round_q28_i32_to_q15(num0r, vl);
    *out0i = mimo_rvv_schur4_round_q28_i32_to_q15(num0i, vl);
    *out1r = mimo_rvv_schur4_round_q28_i32_to_q15(num1r, vl);
    *out1i = mimo_rvv_schur4_round_q28_i32_to_q15(num1i, vl);
}
#endif

static MIMO_ALWAYS_INLINE void mimo_rvv_schur4_solve2_i32(
    vint32m1_t a00r,
    vint32m1_t a01r,
    vint32m1_t a01i,
    vint32m1_t a11r,
    vint32m1_t scale_q30,
    vint32m1_t v0r,
    vint32m1_t v0i,
    vint32m1_t v1r,
    vint32m1_t v1i,
    vint32m1_t *out0r,
    vint32m1_t *out0i,
    vint32m1_t *out1r,
    vint32m1_t *out1i,
    int scale_mode,
    size_t vl)
{
#if MIMO_USE_RVV_SCHUR4_NUM_MAC
    vint32m1_t num0r = __riscv_vmul_vv_i32m1(v0r, a11r, vl);

    num0r = __riscv_vnmsac_vv_i32m1(num0r, a01r, v1r, vl);
    num0r = __riscv_vmacc_vv_i32m1(num0r, a01i, v1i, vl);

    vint32m1_t num0i = __riscv_vmul_vv_i32m1(v0i, a11r, vl);

    num0i = __riscv_vnmsac_vv_i32m1(num0i, a01r, v1i, vl);
    num0i = __riscv_vnmsac_vv_i32m1(num0i, a01i, v1r, vl);

    vint32m1_t num1r = __riscv_vmul_vv_i32m1(v1r, a00r, vl);

    num1r = __riscv_vnmsac_vv_i32m1(num1r, a01r, v0r, vl);
    num1r = __riscv_vnmsac_vv_i32m1(num1r, a01i, v0i, vl);

    vint32m1_t num1i = __riscv_vmul_vv_i32m1(v1i, a00r, vl);

    num1i = __riscv_vnmsac_vv_i32m1(num1i, a01r, v0i, vl);
    num1i = __riscv_vmacc_vv_i32m1(num1i, a01i, v0r, vl);
#else
    vint32m1_t num0r = __riscv_vmul_vv_i32m1(v0r, a11r, vl);
    vint32m1_t tmp = __riscv_vmul_vv_i32m1(a01r, v1r, vl);

    num0r = __riscv_vsub_vv_i32m1(num0r, tmp, vl);
    tmp = __riscv_vmul_vv_i32m1(a01i, v1i, vl);
    num0r = __riscv_vadd_vv_i32m1(num0r, tmp, vl);

    vint32m1_t num0i = __riscv_vmul_vv_i32m1(v0i, a11r, vl);
    tmp = __riscv_vmul_vv_i32m1(a01r, v1i, vl);
    num0i = __riscv_vsub_vv_i32m1(num0i, tmp, vl);
    tmp = __riscv_vmul_vv_i32m1(a01i, v1r, vl);
    num0i = __riscv_vsub_vv_i32m1(num0i, tmp, vl);

    vint32m1_t num1r = __riscv_vmul_vv_i32m1(v1r, a00r, vl);
    tmp = __riscv_vmul_vv_i32m1(a01r, v0r, vl);
    num1r = __riscv_vsub_vv_i32m1(num1r, tmp, vl);
    tmp = __riscv_vmul_vv_i32m1(a01i, v0i, vl);
    num1r = __riscv_vsub_vv_i32m1(num1r, tmp, vl);

    vint32m1_t num1i = __riscv_vmul_vv_i32m1(v1i, a00r, vl);
    tmp = __riscv_vmul_vv_i32m1(a01r, v0i, vl);
    num1i = __riscv_vsub_vv_i32m1(num1i, tmp, vl);
    tmp = __riscv_vmul_vv_i32m1(a01i, v0r, vl);
    num1i = __riscv_vadd_vv_i32m1(num1i, tmp, vl);
#endif

    if (scale_mode == 1) {
        *out0r =
            mimo_rvv_schur4_scale_q30_to_q15_i32_mag(num0r, scale_q30, vl);
        *out0i =
            mimo_rvv_schur4_scale_q30_to_q15_i32_mag(num0i, scale_q30, vl);
        *out1r =
            mimo_rvv_schur4_scale_q30_to_q15_i32_mag(num1r, scale_q30, vl);
        *out1i =
            mimo_rvv_schur4_scale_q30_to_q15_i32_mag(num1i, scale_q30, vl);
    } else if (scale_mode == 2) {
        *out0r =
            mimo_rvv_schur4_scale_q30_to_q15_i32_trunc(num0r, scale_q30, vl);
        *out0i =
            mimo_rvv_schur4_scale_q30_to_q15_i32_trunc(num0i, scale_q30, vl);
        *out1r =
            mimo_rvv_schur4_scale_q30_to_q15_i32_trunc(num1r, scale_q30, vl);
        *out1i =
            mimo_rvv_schur4_scale_q30_to_q15_i32_trunc(num1i, scale_q30, vl);
    } else {
        *out0r =
            mimo_rvv_schur4_scale_q30_to_q15_i32(num0r, scale_q30, vl);
        *out0i =
            mimo_rvv_schur4_scale_q30_to_q15_i32(num0i, scale_q30, vl);
        *out1r =
            mimo_rvv_schur4_scale_q30_to_q15_i32(num1r, scale_q30, vl);
        *out1i =
            mimo_rvv_schur4_scale_q30_to_q15_i32(num1i, scale_q30, vl);
    }
}

static MIMO_ALWAYS_INLINE void mimo_rvv_schur4_solve2_true16b_i32(
    vint32m1_t a00r,
    vint32m1_t a01r,
    vint32m1_t a01i,
    vint32m1_t a11r,
    vint32m1_t scale_q30,
    vint32m1_t v0r,
    vint32m1_t v0i,
    vint32m1_t v1r,
    vint32m1_t v1i,
    vint32m1_t *out0r,
    vint32m1_t *out0i,
    vint32m1_t *out1r,
    vint32m1_t *out1i,
    int scale_mode,
    size_t vl)
{
    const vint16mf2_t a00 = __riscv_vnclip_wx_i16mf2(a00r, 0, 0, vl);
    const vint16mf2_t a01re = __riscv_vnclip_wx_i16mf2(a01r, 0, 0, vl);
    const vint16mf2_t a01im = __riscv_vnclip_wx_i16mf2(a01i, 0, 0, vl);
    const vint16mf2_t a11 = __riscv_vnclip_wx_i16mf2(a11r, 0, 0, vl);
    const vint16mf2_t rhs0r = __riscv_vnclip_wx_i16mf2(v0r, 0, 0, vl);
    const vint16mf2_t rhs0i = __riscv_vnclip_wx_i16mf2(v0i, 0, 0, vl);
    const vint16mf2_t rhs1r = __riscv_vnclip_wx_i16mf2(v1r, 0, 0, vl);
    const vint16mf2_t rhs1i = __riscv_vnclip_wx_i16mf2(v1i, 0, 0, vl);

    vint32m1_t num0r = __riscv_vwmul_vv_i32m1(rhs0r, a11, vl);
    vint32m1_t tmp = __riscv_vwmul_vv_i32m1(a01re, rhs1r, vl);

    num0r = __riscv_vsub_vv_i32m1(num0r, tmp, vl);
    tmp = __riscv_vwmul_vv_i32m1(a01im, rhs1i, vl);
    num0r = __riscv_vadd_vv_i32m1(num0r, tmp, vl);

    vint32m1_t num0i = __riscv_vwmul_vv_i32m1(rhs0i, a11, vl);
    tmp = __riscv_vwmul_vv_i32m1(a01re, rhs1i, vl);
    num0i = __riscv_vsub_vv_i32m1(num0i, tmp, vl);
    tmp = __riscv_vwmul_vv_i32m1(a01im, rhs1r, vl);
    num0i = __riscv_vsub_vv_i32m1(num0i, tmp, vl);

    vint32m1_t num1r = __riscv_vwmul_vv_i32m1(rhs1r, a00, vl);
    tmp = __riscv_vwmul_vv_i32m1(a01re, rhs0r, vl);
    num1r = __riscv_vsub_vv_i32m1(num1r, tmp, vl);
    tmp = __riscv_vwmul_vv_i32m1(a01im, rhs0i, vl);
    num1r = __riscv_vsub_vv_i32m1(num1r, tmp, vl);

    vint32m1_t num1i = __riscv_vwmul_vv_i32m1(rhs1i, a00, vl);
    tmp = __riscv_vwmul_vv_i32m1(a01re, rhs0i, vl);
    num1i = __riscv_vsub_vv_i32m1(num1i, tmp, vl);
    tmp = __riscv_vwmul_vv_i32m1(a01im, rhs0r, vl);
    num1i = __riscv_vadd_vv_i32m1(num1i, tmp, vl);

    if (scale_mode == 1) {
        *out0r =
            mimo_rvv_schur4_scale_q30_to_q15_i32_mag(num0r, scale_q30, vl);
        *out0i =
            mimo_rvv_schur4_scale_q30_to_q15_i32_mag(num0i, scale_q30, vl);
        *out1r =
            mimo_rvv_schur4_scale_q30_to_q15_i32_mag(num1r, scale_q30, vl);
        *out1i =
            mimo_rvv_schur4_scale_q30_to_q15_i32_mag(num1i, scale_q30, vl);
    } else if (scale_mode == 2) {
        *out0r =
            mimo_rvv_schur4_scale_q30_to_q15_i32_trunc(num0r, scale_q30, vl);
        *out0i =
            mimo_rvv_schur4_scale_q30_to_q15_i32_trunc(num0i, scale_q30, vl);
        *out1r =
            mimo_rvv_schur4_scale_q30_to_q15_i32_trunc(num1r, scale_q30, vl);
        *out1i =
            mimo_rvv_schur4_scale_q30_to_q15_i32_trunc(num1i, scale_q30, vl);
    } else {
        *out0r =
            mimo_rvv_schur4_scale_q30_to_q15_i32(num0r, scale_q30, vl);
        *out0i =
            mimo_rvv_schur4_scale_q30_to_q15_i32(num0i, scale_q30, vl);
        *out1r =
            mimo_rvv_schur4_scale_q30_to_q15_i32(num1r, scale_q30, vl);
        *out1i =
            mimo_rvv_schur4_scale_q30_to_q15_i32(num1i, scale_q30, vl);
    }
}

static MIMO_ALWAYS_INLINE void mimo_rvv_schur4_solve2_true16b_a16_i32(
    vint16mf2_t a00,
    vint16mf2_t a01re,
    vint16mf2_t a01im,
    vint16mf2_t a11,
    vint32m1_t scale_q30,
    vint32m1_t v0r,
    vint32m1_t v0i,
    vint32m1_t v1r,
    vint32m1_t v1i,
    vint32m1_t *out0r,
    vint32m1_t *out0i,
    vint32m1_t *out1r,
    vint32m1_t *out1i,
    int scale_mode,
    size_t vl)
{
    const vint16mf2_t rhs0r = __riscv_vnclip_wx_i16mf2(v0r, 0, 0, vl);
    const vint16mf2_t rhs0i = __riscv_vnclip_wx_i16mf2(v0i, 0, 0, vl);
    const vint16mf2_t rhs1r = __riscv_vnclip_wx_i16mf2(v1r, 0, 0, vl);
    const vint16mf2_t rhs1i = __riscv_vnclip_wx_i16mf2(v1i, 0, 0, vl);

    vint32m1_t num0r = __riscv_vwmul_vv_i32m1(rhs0r, a11, vl);
    vint32m1_t tmp = __riscv_vwmul_vv_i32m1(a01re, rhs1r, vl);

    num0r = __riscv_vsub_vv_i32m1(num0r, tmp, vl);
    tmp = __riscv_vwmul_vv_i32m1(a01im, rhs1i, vl);
    num0r = __riscv_vadd_vv_i32m1(num0r, tmp, vl);

    vint32m1_t num0i = __riscv_vwmul_vv_i32m1(rhs0i, a11, vl);
    tmp = __riscv_vwmul_vv_i32m1(a01re, rhs1i, vl);
    num0i = __riscv_vsub_vv_i32m1(num0i, tmp, vl);
    tmp = __riscv_vwmul_vv_i32m1(a01im, rhs1r, vl);
    num0i = __riscv_vsub_vv_i32m1(num0i, tmp, vl);

    vint32m1_t num1r = __riscv_vwmul_vv_i32m1(rhs1r, a00, vl);
    tmp = __riscv_vwmul_vv_i32m1(a01re, rhs0r, vl);
    num1r = __riscv_vsub_vv_i32m1(num1r, tmp, vl);
    tmp = __riscv_vwmul_vv_i32m1(a01im, rhs0i, vl);
    num1r = __riscv_vsub_vv_i32m1(num1r, tmp, vl);

    vint32m1_t num1i = __riscv_vwmul_vv_i32m1(rhs1i, a00, vl);
    tmp = __riscv_vwmul_vv_i32m1(a01re, rhs0i, vl);
    num1i = __riscv_vsub_vv_i32m1(num1i, tmp, vl);
    tmp = __riscv_vwmul_vv_i32m1(a01im, rhs0r, vl);
    num1i = __riscv_vadd_vv_i32m1(num1i, tmp, vl);

    if (scale_mode == 1) {
        *out0r =
            mimo_rvv_schur4_scale_q30_to_q15_i32_mag(num0r, scale_q30, vl);
        *out0i =
            mimo_rvv_schur4_scale_q30_to_q15_i32_mag(num0i, scale_q30, vl);
        *out1r =
            mimo_rvv_schur4_scale_q30_to_q15_i32_mag(num1r, scale_q30, vl);
        *out1i =
            mimo_rvv_schur4_scale_q30_to_q15_i32_mag(num1i, scale_q30, vl);
    } else if (scale_mode == 2) {
        *out0r =
            mimo_rvv_schur4_scale_q30_to_q15_i32_trunc(num0r, scale_q30, vl);
        *out0i =
            mimo_rvv_schur4_scale_q30_to_q15_i32_trunc(num0i, scale_q30, vl);
        *out1r =
            mimo_rvv_schur4_scale_q30_to_q15_i32_trunc(num1r, scale_q30, vl);
        *out1i =
            mimo_rvv_schur4_scale_q30_to_q15_i32_trunc(num1i, scale_q30, vl);
    } else {
        *out0r =
            mimo_rvv_schur4_scale_q30_to_q15_i32(num0r, scale_q30, vl);
        *out0i =
            mimo_rvv_schur4_scale_q30_to_q15_i32(num0i, scale_q30, vl);
        *out1r =
            mimo_rvv_schur4_scale_q30_to_q15_i32(num1r, scale_q30, vl);
        *out1i =
            mimo_rvv_schur4_scale_q30_to_q15_i32(num1i, scale_q30, vl);
    }
}

#if MIMO_USE_RVV_SCHUR4_SOLVE2_PAIR_M2
static MIMO_ALWAYS_INLINE vint32m2_t
mimo_rvv_schur4_pair_m2(vint32m1_t lo, vint32m1_t hi)
{
    vint32m2_t z = __riscv_vundefined_i32m2();

    z = __riscv_vset_v_i32m1_i32m2(z, 0, lo);
    return __riscv_vset_v_i32m1_i32m2(z, 1, hi);
}

static MIMO_ALWAYS_INLINE vint32m1_t
mimo_rvv_schur4_pair_m2_lo(vint32m2_t x)
{
    return __riscv_vget_v_i32m2_i32m1(x, 0);
}

static MIMO_ALWAYS_INLINE vint32m1_t
mimo_rvv_schur4_pair_m2_hi(vint32m2_t x)
{
    return __riscv_vget_v_i32m2_i32m1(x, 1);
}

static MIMO_ALWAYS_INLINE vint32m2_t
mimo_rvv_schur4_scale_q30_to_q15_i32_pair_m2(
    vint32m2_t num_q30,
    vint32m2_t scale_q30,
    size_t vl)
{
    const size_t vl2 = vl * 2;

    num_q30 = __riscv_vadd_vx_i32m2(
        num_q30, 1 << (MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT - 1), vl2);
    const vint32m2_t num_qs = __riscv_vsra_vx_i32m2(
        num_q30, MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT, vl2);
    const vint32m2_t prod =
        __riscv_vmul_vv_i32m2(num_qs, scale_q30, vl2);

    return __riscv_vsra_vx_i32m2(
        prod, 30 - MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT, vl2);
}

static MIMO_ALWAYS_INLINE vint32m2_t
mimo_rvv_schur4_scale_q30_to_q15_i32_trunc_pair_m2(
    vint32m2_t num_q30,
    vint32m2_t scale_q30,
    size_t vl)
{
    const size_t vl2 = vl * 2;

    num_q30 = __riscv_vadd_vx_i32m2(
        num_q30, 1 << (MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT - 1), vl2);
    const vint32m2_t num_qs = __riscv_vsra_vx_i32m2(
        num_q30, MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT, vl2);
    vint32m2_t prod =
        __riscv_vmul_vv_i32m2(num_qs, scale_q30, vl2);
    const int32_t final_bias =
        (1 << (30 - MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT)) - 1;
    const vint32m2_t neg_bias = __riscv_vand_vx_i32m2(
        __riscv_vsra_vx_i32m2(prod, 31, vl2), final_bias, vl2);

    prod = __riscv_vadd_vv_i32m2(prod, neg_bias, vl2);
    return __riscv_vsra_vx_i32m2(
        prod, 30 - MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT, vl2);
}

static MIMO_ALWAYS_INLINE vint32m2_t
mimo_rvv_schur4_scale_q30_to_q15_i32_mag_pair_m2(
    vint32m2_t num_q30,
    vint32m2_t scale_q30,
    size_t vl)
{
    const size_t vl2 = vl * 2;
    const vint32m2_t sign = __riscv_vsra_vx_i32m2(num_q30, 31, vl2);
    vint32m2_t mag = __riscv_vxor_vv_i32m2(num_q30, sign, vl2);

    mag = __riscv_vsub_vv_i32m2(mag, sign, vl2);
    mag = __riscv_vadd_vx_i32m2(
        mag, 1 << (MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT - 1), vl2);
    const vint32m2_t num_qs = __riscv_vsra_vx_i32m2(
        mag, MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT, vl2);
    const vint32m2_t prod =
        __riscv_vmul_vv_i32m2(num_qs, scale_q30, vl2);
    vint32m2_t mag_q15 = __riscv_vsra_vx_i32m2(
        prod, 30 - MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT, vl2);

    mag_q15 = __riscv_vxor_vv_i32m2(mag_q15, sign, vl2);
    return __riscv_vsub_vv_i32m2(mag_q15, sign, vl2);
}

static MIMO_ALWAYS_INLINE void mimo_rvv_schur4_solve2_pair_m2_i32(
    vint32m1_t a00r,
    vint32m1_t a01r,
    vint32m1_t a01i,
    vint32m1_t a11r,
    vint32m1_t scale_q30,
    vint32m1_t lhs0_v0r,
    vint32m1_t lhs0_v0i,
    vint32m1_t lhs0_v1r,
    vint32m1_t lhs0_v1i,
    vint32m1_t lhs1_v0r,
    vint32m1_t lhs1_v0i,
    vint32m1_t lhs1_v1r,
    vint32m1_t lhs1_v1i,
    vint32m1_t *lhs0_out0r,
    vint32m1_t *lhs0_out0i,
    vint32m1_t *lhs0_out1r,
    vint32m1_t *lhs0_out1i,
    vint32m1_t *lhs1_out0r,
    vint32m1_t *lhs1_out0i,
    vint32m1_t *lhs1_out1r,
    vint32m1_t *lhs1_out1i,
    int scale_mode,
    size_t vl)
{
    const size_t vl2 = vl * 2;
    const vint32m2_t a00 = mimo_rvv_schur4_pair_m2(a00r, a00r);
    const vint32m2_t a01r2 = mimo_rvv_schur4_pair_m2(a01r, a01r);
    const vint32m2_t a01i2 = mimo_rvv_schur4_pair_m2(a01i, a01i);
    const vint32m2_t a11 = mimo_rvv_schur4_pair_m2(a11r, a11r);
    const vint32m2_t scale = mimo_rvv_schur4_pair_m2(scale_q30, scale_q30);
    const vint32m2_t v0r = mimo_rvv_schur4_pair_m2(lhs0_v0r, lhs1_v0r);
    const vint32m2_t v0i = mimo_rvv_schur4_pair_m2(lhs0_v0i, lhs1_v0i);
    const vint32m2_t v1r = mimo_rvv_schur4_pair_m2(lhs0_v1r, lhs1_v1r);
    const vint32m2_t v1i = mimo_rvv_schur4_pair_m2(lhs0_v1i, lhs1_v1i);

    vint32m2_t num0r = __riscv_vmul_vv_i32m2(v0r, a11, vl2);
    vint32m2_t tmp = __riscv_vmul_vv_i32m2(a01r2, v1r, vl2);

    num0r = __riscv_vsub_vv_i32m2(num0r, tmp, vl2);
    tmp = __riscv_vmul_vv_i32m2(a01i2, v1i, vl2);
    num0r = __riscv_vadd_vv_i32m2(num0r, tmp, vl2);

    vint32m2_t num0i = __riscv_vmul_vv_i32m2(v0i, a11, vl2);
    tmp = __riscv_vmul_vv_i32m2(a01r2, v1i, vl2);
    num0i = __riscv_vsub_vv_i32m2(num0i, tmp, vl2);
    tmp = __riscv_vmul_vv_i32m2(a01i2, v1r, vl2);
    num0i = __riscv_vsub_vv_i32m2(num0i, tmp, vl2);

    vint32m2_t num1r = __riscv_vmul_vv_i32m2(v1r, a00, vl2);
    tmp = __riscv_vmul_vv_i32m2(a01r2, v0r, vl2);
    num1r = __riscv_vsub_vv_i32m2(num1r, tmp, vl2);
    tmp = __riscv_vmul_vv_i32m2(a01i2, v0i, vl2);
    num1r = __riscv_vsub_vv_i32m2(num1r, tmp, vl2);

    vint32m2_t num1i = __riscv_vmul_vv_i32m2(v1i, a00, vl2);
    tmp = __riscv_vmul_vv_i32m2(a01r2, v0i, vl2);
    num1i = __riscv_vsub_vv_i32m2(num1i, tmp, vl2);
    tmp = __riscv_vmul_vv_i32m2(a01i2, v0r, vl2);
    num1i = __riscv_vadd_vv_i32m2(num1i, tmp, vl2);

    if (scale_mode == 1) {
        num0r = mimo_rvv_schur4_scale_q30_to_q15_i32_mag_pair_m2(num0r, scale, vl);
        num0i = mimo_rvv_schur4_scale_q30_to_q15_i32_mag_pair_m2(num0i, scale, vl);
        num1r = mimo_rvv_schur4_scale_q30_to_q15_i32_mag_pair_m2(num1r, scale, vl);
        num1i = mimo_rvv_schur4_scale_q30_to_q15_i32_mag_pair_m2(num1i, scale, vl);
    } else if (scale_mode == 2) {
        num0r = mimo_rvv_schur4_scale_q30_to_q15_i32_trunc_pair_m2(num0r, scale, vl);
        num0i = mimo_rvv_schur4_scale_q30_to_q15_i32_trunc_pair_m2(num0i, scale, vl);
        num1r = mimo_rvv_schur4_scale_q30_to_q15_i32_trunc_pair_m2(num1r, scale, vl);
        num1i = mimo_rvv_schur4_scale_q30_to_q15_i32_trunc_pair_m2(num1i, scale, vl);
    } else {
        num0r = mimo_rvv_schur4_scale_q30_to_q15_i32_pair_m2(num0r, scale, vl);
        num0i = mimo_rvv_schur4_scale_q30_to_q15_i32_pair_m2(num0i, scale, vl);
        num1r = mimo_rvv_schur4_scale_q30_to_q15_i32_pair_m2(num1r, scale, vl);
        num1i = mimo_rvv_schur4_scale_q30_to_q15_i32_pair_m2(num1i, scale, vl);
    }

    *lhs0_out0r = mimo_rvv_schur4_pair_m2_lo(num0r);
    *lhs0_out0i = mimo_rvv_schur4_pair_m2_lo(num0i);
    *lhs0_out1r = mimo_rvv_schur4_pair_m2_lo(num1r);
    *lhs0_out1i = mimo_rvv_schur4_pair_m2_lo(num1i);
    *lhs1_out0r = mimo_rvv_schur4_pair_m2_hi(num0r);
    *lhs1_out0i = mimo_rvv_schur4_pair_m2_hi(num0i);
    *lhs1_out1r = mimo_rvv_schur4_pair_m2_hi(num1r);
    *lhs1_out1i = mimo_rvv_schur4_pair_m2_hi(num1i);
}
#endif

static MIMO_ALWAYS_INLINE void mimo_rvv_schur4_cmul_q15_i32(
    vint32m1_t ar,
    vint32m1_t ai,
    vint32m1_t br,
    vint32m1_t bi,
    vint32m1_t *out_re,
    vint32m1_t *out_im,
    size_t vl)
{
#if MIMO_USE_RVV_SCHUR4_COMPLEX_MAC
    vint32m1_t re = __riscv_vmul_vv_i32m1(ar, br, vl);
    vint32m1_t im = __riscv_vmul_vv_i32m1(ar, bi, vl);

    re = __riscv_vnmsac_vv_i32m1(re, ai, bi, vl);
    im = __riscv_vmacc_vv_i32m1(im, ai, br, vl);
#else
    vint32m1_t re = __riscv_vmul_vv_i32m1(ar, br, vl);
    vint32m1_t tmp = __riscv_vmul_vv_i32m1(ai, bi, vl);

    re = __riscv_vsub_vv_i32m1(re, tmp, vl);

    vint32m1_t im = __riscv_vmul_vv_i32m1(ar, bi, vl);
    tmp = __riscv_vmul_vv_i32m1(ai, br, vl);
    im = __riscv_vadd_vv_i32m1(im, tmp, vl);
#endif

    *out_re = mimo_rvv_schur4_round_q30_i32_to_q15(re, vl);
    *out_im = mimo_rvv_schur4_round_q30_i32_to_q15(im, vl);
}

static MIMO_ALWAYS_INLINE void mimo_rvv_schur4_inner_conj_q15_i32(
    vint32m1_t e0r,
    vint32m1_t e0i,
    vint32m1_t e1r,
    vint32m1_t e1i,
    vint32m1_t w0r,
    vint32m1_t w0i,
    vint32m1_t w1r,
    vint32m1_t w1i,
    vint32m1_t *out_re,
    vint32m1_t *out_im,
    size_t vl)
{
#if MIMO_USE_MMSE4_SCHUR_INNER_Q30_ONCE
#if MIMO_USE_RVV_SCHUR4_COMPLEX_MAC
    vint32m1_t re = __riscv_vmul_vv_i32m1(e0r, w0r, vl);
    vint32m1_t im = __riscv_vmul_vv_i32m1(e0r, w0i, vl);

    re = __riscv_vmacc_vv_i32m1(re, e0i, w0i, vl);
    re = __riscv_vmacc_vv_i32m1(re, e1r, w1r, vl);
    re = __riscv_vmacc_vv_i32m1(re, e1i, w1i, vl);
    im = __riscv_vnmsac_vv_i32m1(im, e0i, w0r, vl);
    im = __riscv_vmacc_vv_i32m1(im, e1r, w1i, vl);
    im = __riscv_vnmsac_vv_i32m1(im, e1i, w1r, vl);
#else
    vint32m1_t re = __riscv_vmul_vv_i32m1(e0r, w0r, vl);
    vint32m1_t tmp = __riscv_vmul_vv_i32m1(e0i, w0i, vl);

    re = __riscv_vadd_vv_i32m1(re, tmp, vl);
    tmp = __riscv_vmul_vv_i32m1(e1r, w1r, vl);
    re = __riscv_vadd_vv_i32m1(re, tmp, vl);
    tmp = __riscv_vmul_vv_i32m1(e1i, w1i, vl);
    re = __riscv_vadd_vv_i32m1(re, tmp, vl);

    vint32m1_t im = __riscv_vmul_vv_i32m1(e0r, w0i, vl);
    tmp = __riscv_vmul_vv_i32m1(e0i, w0r, vl);
    im = __riscv_vsub_vv_i32m1(im, tmp, vl);
    tmp = __riscv_vmul_vv_i32m1(e1r, w1i, vl);
    im = __riscv_vadd_vv_i32m1(im, tmp, vl);
    tmp = __riscv_vmul_vv_i32m1(e1i, w1r, vl);
    im = __riscv_vsub_vv_i32m1(im, tmp, vl);
#endif

#if MIMO_USE_RVV_MMSE4_SCHUR_INNER_SYM_ROUND
    *out_re = mimo_rvv_schur4_round_q30_i32_to_q15_sym(re, vl);
    *out_im = mimo_rvv_schur4_round_q30_i32_to_q15_sym(im, vl);
#else
    *out_re = mimo_rvv_schur4_round_q30_i32_to_q15(re, vl);
    *out_im = mimo_rvv_schur4_round_q30_i32_to_q15(im, vl);
#endif
#else
    vint32m1_t p0r;
    vint32m1_t p0i;
    vint32m1_t p1r;
    vint32m1_t p1i;

#if MIMO_USE_RVV_SCHUR4_COMPLEX_MAC
    vint32m1_t re = __riscv_vmul_vv_i32m1(e0r, w0r, vl);
    vint32m1_t im = __riscv_vmul_vv_i32m1(e0r, w0i, vl);

    re = __riscv_vmacc_vv_i32m1(re, e0i, w0i, vl);
    im = __riscv_vnmsac_vv_i32m1(im, e0i, w0r, vl);
#else
    vint32m1_t re = __riscv_vmul_vv_i32m1(e0r, w0r, vl);
    vint32m1_t tmp = __riscv_vmul_vv_i32m1(e0i, w0i, vl);

    re = __riscv_vadd_vv_i32m1(re, tmp, vl);

    vint32m1_t im = __riscv_vmul_vv_i32m1(e0r, w0i, vl);
    tmp = __riscv_vmul_vv_i32m1(e0i, w0r, vl);
    im = __riscv_vsub_vv_i32m1(im, tmp, vl);
#endif

    p0r = mimo_rvv_schur4_round_q30_i32_to_q15(re, vl);
    p0i = mimo_rvv_schur4_round_q30_i32_to_q15(im, vl);

#if MIMO_USE_RVV_SCHUR4_COMPLEX_MAC
    re = __riscv_vmul_vv_i32m1(e1r, w1r, vl);
    im = __riscv_vmul_vv_i32m1(e1r, w1i, vl);
    re = __riscv_vmacc_vv_i32m1(re, e1i, w1i, vl);
    im = __riscv_vnmsac_vv_i32m1(im, e1i, w1r, vl);
#else
    re = __riscv_vmul_vv_i32m1(e1r, w1r, vl);
    tmp = __riscv_vmul_vv_i32m1(e1i, w1i, vl);
    re = __riscv_vadd_vv_i32m1(re, tmp, vl);

    im = __riscv_vmul_vv_i32m1(e1r, w1i, vl);
    tmp = __riscv_vmul_vv_i32m1(e1i, w1r, vl);
    im = __riscv_vsub_vv_i32m1(im, tmp, vl);
#endif

    p1r = mimo_rvv_schur4_round_q30_i32_to_q15(re, vl);
    p1i = mimo_rvv_schur4_round_q30_i32_to_q15(im, vl);
    *out_re = __riscv_vadd_vv_i32m1(p0r, p1r, vl);
    *out_im = __riscv_vadd_vv_i32m1(p0i, p1i, vl);
#endif
}

static MIMO_ALWAYS_INLINE void mimo_rvv_schur4_submul_q15_i32(
    vint32m1_t *acc_re,
    vint32m1_t *acc_im,
    vint32m1_t ar,
    vint32m1_t ai,
    vint32m1_t br,
    vint32m1_t bi,
    size_t vl)
{
    vint32m1_t prod_re;
    vint32m1_t prod_im;

    mimo_rvv_schur4_cmul_q15_i32(
        ar, ai, br, bi, &prod_re, &prod_im, vl);
    *acc_re = __riscv_vsub_vv_i32m1(*acc_re, prod_re, vl);
    *acc_im = __riscv_vsub_vv_i32m1(*acc_im, prod_im, vl);
}

static MIMO_ALWAYS_INLINE void mimo_rvv_schur4_submul2_q30_once_i32(
    vint32m1_t *acc_re,
    vint32m1_t *acc_im,
    vint32m1_t a0r,
    vint32m1_t a0i,
    vint32m1_t b0r,
    vint32m1_t b0i,
    vint32m1_t a1r,
    vint32m1_t a1i,
    vint32m1_t b1r,
    vint32m1_t b1i,
    size_t vl)
{
#if MIMO_USE_RVV_SCHUR4_COMPLEX_MAC
    vint32m1_t re = __riscv_vmul_vv_i32m1(a0r, b0r, vl);
    vint32m1_t im = __riscv_vmul_vv_i32m1(a0r, b0i, vl);

    re = __riscv_vnmsac_vv_i32m1(re, a0i, b0i, vl);
    re = __riscv_vmacc_vv_i32m1(re, a1r, b1r, vl);
    re = __riscv_vnmsac_vv_i32m1(re, a1i, b1i, vl);
    im = __riscv_vmacc_vv_i32m1(im, a0i, b0r, vl);
    im = __riscv_vmacc_vv_i32m1(im, a1r, b1i, vl);
    im = __riscv_vmacc_vv_i32m1(im, a1i, b1r, vl);
#else
    vint32m1_t re = __riscv_vmul_vv_i32m1(a0r, b0r, vl);
    vint32m1_t tmp = __riscv_vmul_vv_i32m1(a0i, b0i, vl);

    re = __riscv_vsub_vv_i32m1(re, tmp, vl);
    tmp = __riscv_vmul_vv_i32m1(a1r, b1r, vl);
    re = __riscv_vadd_vv_i32m1(re, tmp, vl);
    tmp = __riscv_vmul_vv_i32m1(a1i, b1i, vl);
    re = __riscv_vsub_vv_i32m1(re, tmp, vl);

    vint32m1_t im = __riscv_vmul_vv_i32m1(a0r, b0i, vl);
    tmp = __riscv_vmul_vv_i32m1(a0i, b0r, vl);
    im = __riscv_vadd_vv_i32m1(im, tmp, vl);
    tmp = __riscv_vmul_vv_i32m1(a1r, b1i, vl);
    im = __riscv_vadd_vv_i32m1(im, tmp, vl);
    tmp = __riscv_vmul_vv_i32m1(a1i, b1r, vl);
    im = __riscv_vadd_vv_i32m1(im, tmp, vl);
#endif

    *acc_re = __riscv_vsub_vv_i32m1(
        *acc_re, mimo_rvv_schur4_round_q30_i32_to_q15(re, vl), vl);
    *acc_im = __riscv_vsub_vv_i32m1(
        *acc_im, mimo_rvv_schur4_round_q30_i32_to_q15(im, vl), vl);
}

static MIMO_ALWAYS_INLINE void mimo_rvv_zf4_schur_rvv_solve_vec_q15(
    vint32m1_t a00r,
    vint32m1_t a01r,
    vint32m1_t a01i,
    vint32m1_t a02r,
    vint32m1_t a02i,
    vint32m1_t a03r,
    vint32m1_t a03i,
    vint32m1_t a11r,
    vint32m1_t a12r,
    vint32m1_t a12i,
    vint32m1_t a13r,
    vint32m1_t a13i,
    vint32m1_t a22r,
    vint32m1_t a23r,
    vint32m1_t a23i,
    vint32m1_t a33r,
    vint32m1_t b0r,
    vint32m1_t b0i,
    vint32m1_t b1r,
    vint32m1_t b1i,
    vint32m1_t b2r,
    vint32m1_t b2i,
    vint32m1_t b3r,
    vint32m1_t b3i,
    int use_mag_scale,
    mimo_c16_t *restrict x_block)
{
    const size_t vl = __riscv_vsetvl_e32m1(MIMO_SITE4_LANES);
    const ptrdiff_t x_stride = (ptrdiff_t)(MIMO_N4 * sizeof(mimo_c16_t));

    const vint32m1_t b_det_q30 =
        mimo_rvv_schur4_det_q30_i32(a00r, a01r, a01i, a11r, vl);
    const vint32m1_t b_scale =
        mimo_rvv_schur4_scale_from_det_q30_i32(b_det_q30, vl);
    vint32m1_t w00r, w00i, w10r, w10i;
    vint32m1_t w01r, w01i, w11r, w11i;
    vint32m1_t y0r, y0i, y1r, y1i;

    mimo_rvv_schur4_solve2_i32(
        a00r, a01r, a01i, a11r, b_scale,
        a02r, a02i, a12r, a12i, &w00r, &w00i, &w10r, &w10i,
        use_mag_scale, vl);
    mimo_rvv_schur4_solve2_i32(
        a00r, a01r, a01i, a11r, b_scale,
        a03r, a03i, a13r, a13i, &w01r, &w01i, &w11r, &w11i,
        use_mag_scale, vl);
    mimo_rvv_schur4_solve2_i32(
        a00r, a01r, a01i, a11r, b_scale,
        b0r, b0i, b1r, b1i, &y0r, &y0i, &y1r, &y1i,
        use_mag_scale, vl);

    vint32m1_t inner_r;
    vint32m1_t inner_i;

    mimo_rvv_schur4_inner_conj_q15_i32(
        a02r, a02i, a12r, a12i, w00r, w00i, w10r, w10i,
        &inner_r, &inner_i, vl);
    const vint32m1_t s00r = __riscv_vsub_vv_i32m1(a22r, inner_r, vl);

    mimo_rvv_schur4_inner_conj_q15_i32(
        a02r, a02i, a12r, a12i, w01r, w01i, w11r, w11i,
        &inner_r, &inner_i, vl);
    const vint32m1_t s01r = __riscv_vsub_vv_i32m1(a23r, inner_r, vl);
    const vint32m1_t s01i = __riscv_vsub_vv_i32m1(a23i, inner_i, vl);

    mimo_rvv_schur4_inner_conj_q15_i32(
        a03r, a03i, a13r, a13i, w01r, w01i, w11r, w11i,
        &inner_r, &inner_i, vl);
    const vint32m1_t s11r = __riscv_vsub_vv_i32m1(a33r, inner_r, vl);

    mimo_rvv_schur4_inner_conj_q15_i32(
        a02r, a02i, a12r, a12i, y0r, y0i, y1r, y1i,
        &inner_r, &inner_i, vl);
    const vint32m1_t r0r = __riscv_vsub_vv_i32m1(b2r, inner_r, vl);
    const vint32m1_t r0i = __riscv_vsub_vv_i32m1(b2i, inner_i, vl);

    mimo_rvv_schur4_inner_conj_q15_i32(
        a03r, a03i, a13r, a13i, y0r, y0i, y1r, y1i,
        &inner_r, &inner_i, vl);
    const vint32m1_t r1r = __riscv_vsub_vv_i32m1(b3r, inner_r, vl);
    const vint32m1_t r1i = __riscv_vsub_vv_i32m1(b3i, inner_i, vl);

    const vint32m1_t s_det_q30 =
        mimo_rvv_schur4_det_q30_i32(s00r, s01r, s01i, s11r, vl);
    const vint32m1_t s_scale =
        mimo_rvv_schur4_scale_from_det_q30_i32(s_det_q30, vl);
    vint32m1_t x2r, x2i, x3r, x3i;

    mimo_rvv_schur4_solve2_i32(
        s00r, s01r, s01i, s11r, s_scale,
        r0r, r0i, r1r, r1i, &x2r, &x2i, &x3r, &x3i,
        use_mag_scale, vl);

    vint32m1_t top0r = y0r;
    vint32m1_t top0i = y0i;
    vint32m1_t top1r = y1r;
    vint32m1_t top1i = y1i;

#if MIMO_USE_RVV_MMSE4_SCHUR_TOP_Q30_ONCE
    mimo_rvv_schur4_submul2_q30_once_i32(
        &top0r, &top0i, w00r, w00i, x2r, x2i,
        w01r, w01i, x3r, x3i, vl);
    mimo_rvv_schur4_submul2_q30_once_i32(
        &top1r, &top1i, w10r, w10i, x2r, x2i,
        w11r, w11i, x3r, x3i, vl);
#else
    mimo_rvv_schur4_submul_q15_i32(
        &top0r, &top0i, w00r, w00i, x2r, x2i, vl);
    mimo_rvv_schur4_submul_q15_i32(
        &top0r, &top0i, w01r, w01i, x3r, x3i, vl);
    mimo_rvv_schur4_submul_q15_i32(
        &top1r, &top1i, w10r, w10i, x2r, x2i, vl);
    mimo_rvv_schur4_submul_q15_i32(
        &top1r, &top1i, w11r, w11i, x3r, x3i, vl);
#endif

    mimo_rvv_solve4_store_x(x_block, 0, top0r, top0i, x_stride, vl);
    mimo_rvv_solve4_store_x(x_block, 1, top1r, top1i, x_stride, vl);
    mimo_rvv_solve4_store_x(x_block, 2, x2r, x2i, x_stride, vl);
    mimo_rvv_solve4_store_x(x_block, 3, x3r, x3i, x_stride, vl);
}

#if MIMO_USE_RVV_ZF4_SCHUR_RVV_SOLVE
static MIMO_NOINLINE __attribute__((unused)) void mimo_rvv_zf4_schur_rvv_solve_soa_q15(
    const mimo_site4_soa4_t *restrict work,
    mimo_c16_t *restrict x_block)
{
    const size_t vl = __riscv_vsetvl_e32m1(MIMO_SITE4_LANES);
    const ptrdiff_t x_stride = (ptrdiff_t)(MIMO_N4 * sizeof(mimo_c16_t));

    const vint32m1_t a00r = mimo_rvv_solve4_load_soa_a_re(work, 0, vl);
    const vint32m1_t a01r = mimo_rvv_solve4_load_soa_a_re(work, 1, vl);
    const vint32m1_t a01i = mimo_rvv_solve4_load_soa_a_im(work, 1, vl);
    const vint32m1_t a02r = mimo_rvv_solve4_load_soa_a_re(work, 2, vl);
    const vint32m1_t a02i = mimo_rvv_solve4_load_soa_a_im(work, 2, vl);
    const vint32m1_t a03r = mimo_rvv_solve4_load_soa_a_re(work, 3, vl);
    const vint32m1_t a03i = mimo_rvv_solve4_load_soa_a_im(work, 3, vl);
    const vint32m1_t a11r = mimo_rvv_solve4_load_soa_a_re(work, 5, vl);
    const vint32m1_t a12r = mimo_rvv_solve4_load_soa_a_re(work, 6, vl);
    const vint32m1_t a12i = mimo_rvv_solve4_load_soa_a_im(work, 6, vl);
    const vint32m1_t a13r = mimo_rvv_solve4_load_soa_a_re(work, 7, vl);
    const vint32m1_t a13i = mimo_rvv_solve4_load_soa_a_im(work, 7, vl);
    const vint32m1_t a22r = mimo_rvv_solve4_load_soa_a_re(work, 10, vl);
    const vint32m1_t a23r = mimo_rvv_solve4_load_soa_a_re(work, 11, vl);
    const vint32m1_t a23i = mimo_rvv_solve4_load_soa_a_im(work, 11, vl);
    const vint32m1_t a33r = mimo_rvv_solve4_load_soa_a_re(work, 15, vl);
    const vint32m1_t b0r = mimo_rvv_solve4_load_soa_b_re(work, 0, vl);
    const vint32m1_t b0i = mimo_rvv_solve4_load_soa_b_im(work, 0, vl);
    const vint32m1_t b1r = mimo_rvv_solve4_load_soa_b_re(work, 1, vl);
    const vint32m1_t b1i = mimo_rvv_solve4_load_soa_b_im(work, 1, vl);
    const vint32m1_t b2r = mimo_rvv_solve4_load_soa_b_re(work, 2, vl);
    const vint32m1_t b2i = mimo_rvv_solve4_load_soa_b_im(work, 2, vl);
    const vint32m1_t b3r = mimo_rvv_solve4_load_soa_b_re(work, 3, vl);
    const vint32m1_t b3i = mimo_rvv_solve4_load_soa_b_im(work, 3, vl);

    const vint32m1_t b_det_q30 =
        mimo_rvv_schur4_det_q30_i32(a00r, a01r, a01i, a11r, vl);
    const vint32m1_t b_scale =
        mimo_rvv_schur4_scale_from_det_q30_i32(b_det_q30, vl);
    vint32m1_t w00r, w00i, w10r, w10i;
    vint32m1_t w01r, w01i, w11r, w11i;
    vint32m1_t y0r, y0i, y1r, y1i;

    mimo_rvv_schur4_solve2_i32(
        a00r, a01r, a01i, a11r, b_scale,
        a02r, a02i, a12r, a12i, &w00r, &w00i, &w10r, &w10i, 0, vl);
    mimo_rvv_schur4_solve2_i32(
        a00r, a01r, a01i, a11r, b_scale,
        a03r, a03i, a13r, a13i, &w01r, &w01i, &w11r, &w11i, 0, vl);
    mimo_rvv_schur4_solve2_i32(
        a00r, a01r, a01i, a11r, b_scale,
        b0r, b0i, b1r, b1i, &y0r, &y0i, &y1r, &y1i, 0, vl);

    vint32m1_t inner_r;
    vint32m1_t inner_i;

    mimo_rvv_schur4_inner_conj_q15_i32(
        a02r, a02i, a12r, a12i, w00r, w00i, w10r, w10i,
        &inner_r, &inner_i, vl);
    const vint32m1_t s00r = __riscv_vsub_vv_i32m1(a22r, inner_r, vl);

    mimo_rvv_schur4_inner_conj_q15_i32(
        a02r, a02i, a12r, a12i, w01r, w01i, w11r, w11i,
        &inner_r, &inner_i, vl);
    const vint32m1_t s01r = __riscv_vsub_vv_i32m1(a23r, inner_r, vl);
    const vint32m1_t s01i = __riscv_vsub_vv_i32m1(a23i, inner_i, vl);

    mimo_rvv_schur4_inner_conj_q15_i32(
        a03r, a03i, a13r, a13i, w01r, w01i, w11r, w11i,
        &inner_r, &inner_i, vl);
    const vint32m1_t s11r = __riscv_vsub_vv_i32m1(a33r, inner_r, vl);

    mimo_rvv_schur4_inner_conj_q15_i32(
        a02r, a02i, a12r, a12i, y0r, y0i, y1r, y1i,
        &inner_r, &inner_i, vl);
    const vint32m1_t r0r = __riscv_vsub_vv_i32m1(b2r, inner_r, vl);
    const vint32m1_t r0i = __riscv_vsub_vv_i32m1(b2i, inner_i, vl);

    mimo_rvv_schur4_inner_conj_q15_i32(
        a03r, a03i, a13r, a13i, y0r, y0i, y1r, y1i,
        &inner_r, &inner_i, vl);
    const vint32m1_t r1r = __riscv_vsub_vv_i32m1(b3r, inner_r, vl);
    const vint32m1_t r1i = __riscv_vsub_vv_i32m1(b3i, inner_i, vl);

    const vint32m1_t s_det_q30 =
        mimo_rvv_schur4_det_q30_i32(s00r, s01r, s01i, s11r, vl);
    const vint32m1_t s_scale =
        mimo_rvv_schur4_scale_from_det_q30_i32(s_det_q30, vl);
    vint32m1_t x2r, x2i, x3r, x3i;

    mimo_rvv_schur4_solve2_i32(
        s00r, s01r, s01i, s11r, s_scale,
        r0r, r0i, r1r, r1i, &x2r, &x2i, &x3r, &x3i, 0, vl);

    vint32m1_t top0r = y0r;
    vint32m1_t top0i = y0i;
    vint32m1_t top1r = y1r;
    vint32m1_t top1i = y1i;

    mimo_rvv_schur4_submul_q15_i32(
        &top0r, &top0i, w00r, w00i, x2r, x2i, vl);
    mimo_rvv_schur4_submul_q15_i32(
        &top0r, &top0i, w01r, w01i, x3r, x3i, vl);
    mimo_rvv_schur4_submul_q15_i32(
        &top1r, &top1i, w10r, w10i, x2r, x2i, vl);
    mimo_rvv_schur4_submul_q15_i32(
        &top1r, &top1i, w11r, w11i, x3r, x3i, vl);

    mimo_rvv_solve4_store_x(x_block, 0, top0r, top0i, x_stride, vl);
    mimo_rvv_solve4_store_x(x_block, 1, top1r, top1i, x_stride, vl);
    mimo_rvv_solve4_store_x(x_block, 2, x2r, x2i, x_stride, vl);
    mimo_rvv_solve4_store_x(x_block, 3, x3r, x3i, x_stride, vl);
}
#endif

#if MIMO_USE_RVV_MMSE4_SCHUR_RVV_SOLVE
static MIMO_NOINLINE __attribute__((unused)) void mimo_rvv_mmse4_schur_rvv_solve_soa_q15(
    const mimo_site4_soa4_t *restrict work,
    mimo_c16_t *restrict x_block)
{
    const size_t vl = __riscv_vsetvl_e32m1(MIMO_SITE4_LANES);

    const vint32m1_t lambda =
        __riscv_vmv_v_x_i32m1(MIMO_MMSE_LAMBDA_Q15, vl);
    const vint32m1_t a00r = __riscv_vadd_vv_i32m1(
        mimo_rvv_solve4_load_soa_a_re(work, 0, vl), lambda, vl);
    const vint32m1_t a11r = __riscv_vadd_vv_i32m1(
        mimo_rvv_solve4_load_soa_a_re(work, 5, vl), lambda, vl);
    const vint32m1_t a22r = __riscv_vadd_vv_i32m1(
        mimo_rvv_solve4_load_soa_a_re(work, 10, vl), lambda, vl);
    const vint32m1_t a33r = __riscv_vadd_vv_i32m1(
        mimo_rvv_solve4_load_soa_a_re(work, 15, vl), lambda, vl);

    mimo_rvv_zf4_schur_rvv_solve_vec_q15(
        a00r,
        mimo_rvv_solve4_load_soa_a_re(work, 1, vl),
        mimo_rvv_solve4_load_soa_a_im(work, 1, vl),
        mimo_rvv_solve4_load_soa_a_re(work, 2, vl),
        mimo_rvv_solve4_load_soa_a_im(work, 2, vl),
        mimo_rvv_solve4_load_soa_a_re(work, 3, vl),
        mimo_rvv_solve4_load_soa_a_im(work, 3, vl),
        a11r,
        mimo_rvv_solve4_load_soa_a_re(work, 6, vl),
        mimo_rvv_solve4_load_soa_a_im(work, 6, vl),
        mimo_rvv_solve4_load_soa_a_re(work, 7, vl),
        mimo_rvv_solve4_load_soa_a_im(work, 7, vl),
        a22r,
        mimo_rvv_solve4_load_soa_a_re(work, 11, vl),
        mimo_rvv_solve4_load_soa_a_im(work, 11, vl),
        a33r,
        mimo_rvv_solve4_load_soa_b_re(work, 0, vl),
        mimo_rvv_solve4_load_soa_b_im(work, 0, vl),
        mimo_rvv_solve4_load_soa_b_re(work, 1, vl),
        mimo_rvv_solve4_load_soa_b_im(work, 1, vl),
        mimo_rvv_solve4_load_soa_b_re(work, 2, vl),
        mimo_rvv_solve4_load_soa_b_im(work, 2, vl),
        mimo_rvv_solve4_load_soa_b_re(work, 3, vl),
        mimo_rvv_solve4_load_soa_b_im(work, 3, vl),
        MIMO_MMSE4_SCHUR_SCALE_ARG(1),
        x_block);
}
#endif

#undef MIMO_RVV_SCHUR4_DET_Q15_RECIP_MIN
#endif

static MIMO_ALWAYS_INLINE void mimo_rvv_solve_zf4_upper_hermitian_site4_q15(
    const mimo_site4_soa4_t *work,
    mimo_c16_t *x_block)
{
    const size_t vl = __riscv_vsetvl_e32m1(MIMO_SITE4_LANES);
    const ptrdiff_t x_stride = (ptrdiff_t)(MIMO_N4 * sizeof(mimo_c16_t));
    const vint32m1_t zero = __riscv_vmv_v_x_i32m1(0, vl);

#if MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED
#define MIMO_RVV_ZF4_A00 0
#define MIMO_RVV_ZF4_A01 1
#define MIMO_RVV_ZF4_A02 2
#define MIMO_RVV_ZF4_A03 3
#define MIMO_RVV_ZF4_A11 4
#define MIMO_RVV_ZF4_A12 5
#define MIMO_RVV_ZF4_A13 6
#define MIMO_RVV_ZF4_A22 7
#define MIMO_RVV_ZF4_A23 8
#define MIMO_RVV_ZF4_A33 9
#else
#define MIMO_RVV_ZF4_A00 0
#define MIMO_RVV_ZF4_A01 1
#define MIMO_RVV_ZF4_A02 2
#define MIMO_RVV_ZF4_A03 3
#define MIMO_RVV_ZF4_A11 5
#define MIMO_RVV_ZF4_A12 6
#define MIMO_RVV_ZF4_A13 7
#define MIMO_RVV_ZF4_A22 10
#define MIMO_RVV_ZF4_A23 11
#define MIMO_RVV_ZF4_A33 15
#endif

    vint32m1_t m00r = mimo_rvv_solve4_load_soa_a_re(work, MIMO_RVV_ZF4_A00, vl);
    vint32m1_t m01r = mimo_rvv_solve4_load_soa_a_re(work, MIMO_RVV_ZF4_A01, vl);
    vint32m1_t m01i = mimo_rvv_solve4_load_soa_a_im(work, MIMO_RVV_ZF4_A01, vl);
    vint32m1_t m02r = mimo_rvv_solve4_load_soa_a_re(work, MIMO_RVV_ZF4_A02, vl);
    vint32m1_t m02i = mimo_rvv_solve4_load_soa_a_im(work, MIMO_RVV_ZF4_A02, vl);
    vint32m1_t m03r = mimo_rvv_solve4_load_soa_a_re(work, MIMO_RVV_ZF4_A03, vl);
    vint32m1_t m03i = mimo_rvv_solve4_load_soa_a_im(work, MIMO_RVV_ZF4_A03, vl);
    vint32m1_t m11r = mimo_rvv_solve4_load_soa_a_re(work, MIMO_RVV_ZF4_A11, vl);
    vint32m1_t m11i = zero;
    vint32m1_t m12r = mimo_rvv_solve4_load_soa_a_re(work, MIMO_RVV_ZF4_A12, vl);
    vint32m1_t m12i = mimo_rvv_solve4_load_soa_a_im(work, MIMO_RVV_ZF4_A12, vl);
    vint32m1_t m13r = mimo_rvv_solve4_load_soa_a_re(work, MIMO_RVV_ZF4_A13, vl);
    vint32m1_t m13i = mimo_rvv_solve4_load_soa_a_im(work, MIMO_RVV_ZF4_A13, vl);
    vint32m1_t m22r = mimo_rvv_solve4_load_soa_a_re(work, MIMO_RVV_ZF4_A22, vl);
    vint32m1_t m22i = zero;
    vint32m1_t m23r = mimo_rvv_solve4_load_soa_a_re(work, MIMO_RVV_ZF4_A23, vl);
    vint32m1_t m23i = mimo_rvv_solve4_load_soa_a_im(work, MIMO_RVV_ZF4_A23, vl);
    vint32m1_t m33r = mimo_rvv_solve4_load_soa_a_re(work, MIMO_RVV_ZF4_A33, vl);
    vint32m1_t m33i = zero;
    vint32m1_t r0r = mimo_rvv_solve4_load_soa_b_re(work, 0, vl);
    vint32m1_t r0i = mimo_rvv_solve4_load_soa_b_im(work, 0, vl);
    vint32m1_t r1r = mimo_rvv_solve4_load_soa_b_re(work, 1, vl);
    vint32m1_t r1i = mimo_rvv_solve4_load_soa_b_im(work, 1, vl);
    vint32m1_t r2r = mimo_rvv_solve4_load_soa_b_re(work, 2, vl);
    vint32m1_t r2i = mimo_rvv_solve4_load_soa_b_im(work, 2, vl);
    vint32m1_t r3r = mimo_rvv_solve4_load_soa_b_re(work, 3, vl);
    vint32m1_t r3i = mimo_rvv_solve4_load_soa_b_im(work, 3, vl);
    vint32m1_t f_re;
    vint32m1_t f_im;
    vint32m1_t x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;

#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
    (void)m11i;
    (void)m22i;
    (void)m33i;
#endif

    vint64m2_t pivot = mimo_rvv_solve4_safe_pivot_i64(m00r, vl);
    f_re = mimo_rvv_solve4_div_q30(m01r, pivot, vl);
    f_im = mimo_rvv_solve4_div_q30(
        __riscv_vsub_vv_i32m1(zero, m01i, vl), pivot, vl);
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
    mimo_rvv_solve4_submul_q30_q15_real(&m11r, m01r, m01i, f_re, f_im, vl);
#else
    mimo_rvv_solve4_submul_q30_q15(&m11r, &m11i, m01r, m01i, f_re, f_im, vl);
#endif
    mimo_rvv_solve4_submul_q30_q15(&m12r, &m12i, m02r, m02i, f_re, f_im, vl);
    mimo_rvv_solve4_submul_q30_q15(&m13r, &m13i, m03r, m03i, f_re, f_im, vl);
    mimo_rvv_solve4_submul_q30_q15(&r1r, &r1i, r0r, r0i, f_re, f_im, vl);

    f_re = mimo_rvv_solve4_div_q30(m02r, pivot, vl);
    f_im = mimo_rvv_solve4_div_q30(
        __riscv_vsub_vv_i32m1(zero, m02i, vl), pivot, vl);
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
    mimo_rvv_solve4_submul_q30_q15_real(&m22r, m02r, m02i, f_re, f_im, vl);
#else
    mimo_rvv_solve4_submul_q30_q15(&m22r, &m22i, m02r, m02i, f_re, f_im, vl);
#endif
    mimo_rvv_solve4_submul_q30_q15(&m23r, &m23i, m03r, m03i, f_re, f_im, vl);
    mimo_rvv_solve4_submul_q30_q15(&r2r, &r2i, r0r, r0i, f_re, f_im, vl);

    f_re = mimo_rvv_solve4_div_q30(m03r, pivot, vl);
    f_im = mimo_rvv_solve4_div_q30(
        __riscv_vsub_vv_i32m1(zero, m03i, vl), pivot, vl);
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
    mimo_rvv_solve4_submul_q30_q15_real(&m33r, m03r, m03i, f_re, f_im, vl);
#else
    mimo_rvv_solve4_submul_q30_q15(&m33r, &m33i, m03r, m03i, f_re, f_im, vl);
#endif
    mimo_rvv_solve4_submul_q30_q15(&r3r, &r3i, r0r, r0i, f_re, f_im, vl);

    pivot = mimo_rvv_solve4_safe_pivot_i64(m11r, vl);
    f_re = mimo_rvv_solve4_div_q30(m12r, pivot, vl);
    f_im = mimo_rvv_solve4_div_q30(
        __riscv_vsub_vv_i32m1(zero, m12i, vl), pivot, vl);
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
    mimo_rvv_solve4_submul_q30_q15_real(&m22r, m12r, m12i, f_re, f_im, vl);
#else
    mimo_rvv_solve4_submul_q30_q15(&m22r, &m22i, m12r, m12i, f_re, f_im, vl);
#endif
    mimo_rvv_solve4_submul_q30_q15(&m23r, &m23i, m13r, m13i, f_re, f_im, vl);
    mimo_rvv_solve4_submul_q30_q15(&r2r, &r2i, r1r, r1i, f_re, f_im, vl);

    f_re = mimo_rvv_solve4_div_q30(m13r, pivot, vl);
    f_im = mimo_rvv_solve4_div_q30(
        __riscv_vsub_vv_i32m1(zero, m13i, vl), pivot, vl);
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
    mimo_rvv_solve4_submul_q30_q15_real(&m33r, m13r, m13i, f_re, f_im, vl);
#else
    mimo_rvv_solve4_submul_q30_q15(&m33r, &m33i, m13r, m13i, f_re, f_im, vl);
#endif
    mimo_rvv_solve4_submul_q30_q15(&r3r, &r3i, r1r, r1i, f_re, f_im, vl);

    pivot = mimo_rvv_solve4_safe_pivot_i64(m22r, vl);
    f_re = mimo_rvv_solve4_div_q30(m23r, pivot, vl);
    f_im = mimo_rvv_solve4_div_q30(
        __riscv_vsub_vv_i32m1(zero, m23i, vl), pivot, vl);
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
    mimo_rvv_solve4_submul_q30_q15_real(&m33r, m23r, m23i, f_re, f_im, vl);
#else
    mimo_rvv_solve4_submul_q30_q15(&m33r, &m33i, m23r, m23i, f_re, f_im, vl);
#endif
    mimo_rvv_solve4_submul_q30_q15(&r3r, &r3i, r2r, r2i, f_re, f_im, vl);

    pivot = mimo_rvv_solve4_safe_pivot_i64(m33r, vl);
    x3r = mimo_rvv_solve4_div_q15(r3r, pivot, vl);
    x3i = mimo_rvv_solve4_div_q15(r3i, pivot, vl);

    mimo_rvv_solve4_submul_q15(&r2r, &r2i, m23r, m23i, x3r, x3i, vl);
    pivot = mimo_rvv_solve4_safe_pivot_i64(m22r, vl);
    x2r = mimo_rvv_solve4_div_q15(r2r, pivot, vl);
    x2i = mimo_rvv_solve4_div_q15(r2i, pivot, vl);

    mimo_rvv_solve4_submul_q15(&r1r, &r1i, m12r, m12i, x2r, x2i, vl);
    mimo_rvv_solve4_submul_q15(&r1r, &r1i, m13r, m13i, x3r, x3i, vl);
    pivot = mimo_rvv_solve4_safe_pivot_i64(m11r, vl);
    x1r = mimo_rvv_solve4_div_q15(r1r, pivot, vl);
    x1i = mimo_rvv_solve4_div_q15(r1i, pivot, vl);

    mimo_rvv_solve4_submul_q15(&r0r, &r0i, m01r, m01i, x1r, x1i, vl);
    mimo_rvv_solve4_submul_q15(&r0r, &r0i, m02r, m02i, x2r, x2i, vl);
    mimo_rvv_solve4_submul_q15(&r0r, &r0i, m03r, m03i, x3r, x3i, vl);
    pivot = mimo_rvv_solve4_safe_pivot_i64(m00r, vl);
    x0r = mimo_rvv_solve4_div_q15(r0r, pivot, vl);
    x0i = mimo_rvv_solve4_div_q15(r0i, pivot, vl);

    mimo_rvv_solve4_store_x(x_block, 0, x0r, x0i, x_stride, vl);
    mimo_rvv_solve4_store_x(x_block, 1, x1r, x1i, x_stride, vl);
    mimo_rvv_solve4_store_x(x_block, 2, x2r, x2i, x_stride, vl);
    mimo_rvv_solve4_store_x(x_block, 3, x3r, x3i, x_stride, vl);

#undef MIMO_RVV_ZF4_A00
#undef MIMO_RVV_ZF4_A01
#undef MIMO_RVV_ZF4_A02
#undef MIMO_RVV_ZF4_A03
#undef MIMO_RVV_ZF4_A11
#undef MIMO_RVV_ZF4_A12
#undef MIMO_RVV_ZF4_A13
#undef MIMO_RVV_ZF4_A22
#undef MIMO_RVV_ZF4_A23
#undef MIMO_RVV_ZF4_A33
}

static MIMO_ALWAYS_INLINE int32_t mimo_rvv_solve4_safe_div_den_i32(int32_t den)
{
    return den == 0 ? 1 : den;
}

static MIMO_ALWAYS_INLINE int32_t mimo_rvv_solve4_scalar_div_q30_i32(
    int32_t num,
    int32_t den)
{
    return (int32_t)(((int64_t)num << 30) /
                     (int64_t)mimo_rvv_solve4_safe_div_den_i32(den));
}

static MIMO_ALWAYS_INLINE int32_t mimo_rvv_solve4_scalar_div_q15_i32(
    int32_t num,
    int32_t den)
{
    return (int32_t)(((int64_t)num << MIMO_Q15_SHIFT) /
                     (int64_t)mimo_rvv_solve4_safe_div_den_i32(den));
}

static MIMO_ALWAYS_INLINE void mimo_rvv_solve4_store_safe_den(
    int32_t den[MIMO_SITE4_LANES],
    vint32m1_t pivot,
    size_t vl)
{
    __riscv_vse32_v_i32m1(den, pivot, vl);
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        den[lane] = mimo_rvv_solve4_safe_div_den_i32(den[lane]);
    }
}

static MIMO_ALWAYS_INLINE void mimo_rvv_solve4_scalar_div_q30_pair_lanes(
    vint32m1_t num_re,
    vint32m1_t num_im,
    const int32_t den[MIMO_SITE4_LANES],
    vint32m1_t *out_re,
    vint32m1_t *out_im,
    int negate_imag,
    int32_t tmp_re[MIMO_SITE4_LANES],
    int32_t tmp_im[MIMO_SITE4_LANES],
    int32_t div_re[MIMO_SITE4_LANES],
    int32_t div_im[MIMO_SITE4_LANES],
    size_t vl)
{
    __riscv_vse32_v_i32m1(tmp_re, num_re, vl);
    __riscv_vse32_v_i32m1(tmp_im, num_im, vl);
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        const int32_t im = negate_imag ? -tmp_im[lane] : tmp_im[lane];

        div_re[lane] = mimo_rvv_solve4_scalar_div_q30_i32(tmp_re[lane],
                                                          den[lane]);
        div_im[lane] = mimo_rvv_solve4_scalar_div_q30_i32(im, den[lane]);
    }
    *out_re = __riscv_vle32_v_i32m1(div_re, vl);
    *out_im = __riscv_vle32_v_i32m1(div_im, vl);
}

static MIMO_ALWAYS_INLINE void mimo_rvv_solve4_scalar_div_q15_pair_lanes(
    vint32m1_t num_re,
    vint32m1_t num_im,
    const int32_t den[MIMO_SITE4_LANES],
    vint32m1_t *out_re,
    vint32m1_t *out_im,
    int32_t tmp_re[MIMO_SITE4_LANES],
    int32_t tmp_im[MIMO_SITE4_LANES],
    int32_t div_re[MIMO_SITE4_LANES],
    int32_t div_im[MIMO_SITE4_LANES],
    size_t vl)
{
    __riscv_vse32_v_i32m1(tmp_re, num_re, vl);
    __riscv_vse32_v_i32m1(tmp_im, num_im, vl);
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        div_re[lane] = mimo_rvv_solve4_scalar_div_q15_i32(tmp_re[lane],
                                                          den[lane]);
        div_im[lane] = mimo_rvv_solve4_scalar_div_q15_i32(tmp_im[lane],
                                                          den[lane]);
    }
    *out_re = __riscv_vle32_v_i32m1(div_re, vl);
    *out_im = __riscv_vle32_v_i32m1(div_im, vl);
}

static MIMO_ALWAYS_INLINE void
mimo_rvv_solve_zf4_upper_hermitian_site4_mixed_update_q15(
    const mimo_site4_soa4_t *work,
    mimo_c16_t *x_block)
{
    const size_t vl = __riscv_vsetvl_e32m1(MIMO_SITE4_LANES);
    const ptrdiff_t x_stride = (ptrdiff_t)(MIMO_N4 * sizeof(mimo_c16_t));
    const vint32m1_t zero = __riscv_vmv_v_x_i32m1(0, vl);
    int32_t den0[MIMO_SITE4_LANES] MIMO_ALIGN64;
    int32_t den1[MIMO_SITE4_LANES] MIMO_ALIGN64;
    int32_t den2[MIMO_SITE4_LANES] MIMO_ALIGN64;
    int32_t den3[MIMO_SITE4_LANES] MIMO_ALIGN64;
    int32_t tmp_re[MIMO_SITE4_LANES] MIMO_ALIGN64;
    int32_t tmp_im[MIMO_SITE4_LANES] MIMO_ALIGN64;
    int32_t div_re[MIMO_SITE4_LANES] MIMO_ALIGN64;
    int32_t div_im[MIMO_SITE4_LANES] MIMO_ALIGN64;

#if MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED
#define MIMO_RVV_ZF4_MIX_A00 0
#define MIMO_RVV_ZF4_MIX_A01 1
#define MIMO_RVV_ZF4_MIX_A02 2
#define MIMO_RVV_ZF4_MIX_A03 3
#define MIMO_RVV_ZF4_MIX_A11 4
#define MIMO_RVV_ZF4_MIX_A12 5
#define MIMO_RVV_ZF4_MIX_A13 6
#define MIMO_RVV_ZF4_MIX_A22 7
#define MIMO_RVV_ZF4_MIX_A23 8
#define MIMO_RVV_ZF4_MIX_A33 9
#else
#define MIMO_RVV_ZF4_MIX_A00 0
#define MIMO_RVV_ZF4_MIX_A01 1
#define MIMO_RVV_ZF4_MIX_A02 2
#define MIMO_RVV_ZF4_MIX_A03 3
#define MIMO_RVV_ZF4_MIX_A11 5
#define MIMO_RVV_ZF4_MIX_A12 6
#define MIMO_RVV_ZF4_MIX_A13 7
#define MIMO_RVV_ZF4_MIX_A22 10
#define MIMO_RVV_ZF4_MIX_A23 11
#define MIMO_RVV_ZF4_MIX_A33 15
#endif

    vint32m1_t m00r =
        mimo_rvv_solve4_load_soa_a_re(work, MIMO_RVV_ZF4_MIX_A00, vl);
    vint32m1_t m01r =
        mimo_rvv_solve4_load_soa_a_re(work, MIMO_RVV_ZF4_MIX_A01, vl);
    vint32m1_t m01i =
        mimo_rvv_solve4_load_soa_a_im(work, MIMO_RVV_ZF4_MIX_A01, vl);
    vint32m1_t m02r =
        mimo_rvv_solve4_load_soa_a_re(work, MIMO_RVV_ZF4_MIX_A02, vl);
    vint32m1_t m02i =
        mimo_rvv_solve4_load_soa_a_im(work, MIMO_RVV_ZF4_MIX_A02, vl);
    vint32m1_t m03r =
        mimo_rvv_solve4_load_soa_a_re(work, MIMO_RVV_ZF4_MIX_A03, vl);
    vint32m1_t m03i =
        mimo_rvv_solve4_load_soa_a_im(work, MIMO_RVV_ZF4_MIX_A03, vl);
    vint32m1_t m11r =
        mimo_rvv_solve4_load_soa_a_re(work, MIMO_RVV_ZF4_MIX_A11, vl);
    vint32m1_t m11i = zero;
    vint32m1_t m12r =
        mimo_rvv_solve4_load_soa_a_re(work, MIMO_RVV_ZF4_MIX_A12, vl);
    vint32m1_t m12i =
        mimo_rvv_solve4_load_soa_a_im(work, MIMO_RVV_ZF4_MIX_A12, vl);
    vint32m1_t m13r =
        mimo_rvv_solve4_load_soa_a_re(work, MIMO_RVV_ZF4_MIX_A13, vl);
    vint32m1_t m13i =
        mimo_rvv_solve4_load_soa_a_im(work, MIMO_RVV_ZF4_MIX_A13, vl);
    vint32m1_t m22r =
        mimo_rvv_solve4_load_soa_a_re(work, MIMO_RVV_ZF4_MIX_A22, vl);
    vint32m1_t m22i = zero;
    vint32m1_t m23r =
        mimo_rvv_solve4_load_soa_a_re(work, MIMO_RVV_ZF4_MIX_A23, vl);
    vint32m1_t m23i =
        mimo_rvv_solve4_load_soa_a_im(work, MIMO_RVV_ZF4_MIX_A23, vl);
    vint32m1_t m33r =
        mimo_rvv_solve4_load_soa_a_re(work, MIMO_RVV_ZF4_MIX_A33, vl);
    vint32m1_t m33i = zero;
    vint32m1_t r0r = mimo_rvv_solve4_load_soa_b_re(work, 0, vl);
    vint32m1_t r0i = mimo_rvv_solve4_load_soa_b_im(work, 0, vl);
    vint32m1_t r1r = mimo_rvv_solve4_load_soa_b_re(work, 1, vl);
    vint32m1_t r1i = mimo_rvv_solve4_load_soa_b_im(work, 1, vl);
    vint32m1_t r2r = mimo_rvv_solve4_load_soa_b_re(work, 2, vl);
    vint32m1_t r2i = mimo_rvv_solve4_load_soa_b_im(work, 2, vl);
    vint32m1_t r3r = mimo_rvv_solve4_load_soa_b_re(work, 3, vl);
    vint32m1_t r3i = mimo_rvv_solve4_load_soa_b_im(work, 3, vl);
    vint32m1_t f_re;
    vint32m1_t f_im;
    vint32m1_t x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;

#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
    (void)m11i;
    (void)m22i;
    (void)m33i;
#endif

    mimo_rvv_solve4_store_safe_den(den0, m00r, vl);
    mimo_rvv_solve4_scalar_div_q30_pair_lanes(
        m01r, m01i, den0, &f_re, &f_im, 1, tmp_re, tmp_im, div_re, div_im,
        vl);
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
    mimo_rvv_solve4_submul_q30_q15_real(&m11r, m01r, m01i, f_re, f_im, vl);
#else
    mimo_rvv_solve4_submul_q30_q15(&m11r, &m11i, m01r, m01i, f_re, f_im, vl);
#endif
    mimo_rvv_solve4_submul_q30_q15(&m12r, &m12i, m02r, m02i, f_re, f_im, vl);
    mimo_rvv_solve4_submul_q30_q15(&m13r, &m13i, m03r, m03i, f_re, f_im, vl);
    mimo_rvv_solve4_submul_q30_q15(&r1r, &r1i, r0r, r0i, f_re, f_im, vl);

    mimo_rvv_solve4_scalar_div_q30_pair_lanes(
        m02r, m02i, den0, &f_re, &f_im, 1, tmp_re, tmp_im, div_re, div_im,
        vl);
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
    mimo_rvv_solve4_submul_q30_q15_real(&m22r, m02r, m02i, f_re, f_im, vl);
#else
    mimo_rvv_solve4_submul_q30_q15(&m22r, &m22i, m02r, m02i, f_re, f_im, vl);
#endif
    mimo_rvv_solve4_submul_q30_q15(&m23r, &m23i, m03r, m03i, f_re, f_im, vl);
    mimo_rvv_solve4_submul_q30_q15(&r2r, &r2i, r0r, r0i, f_re, f_im, vl);

    mimo_rvv_solve4_scalar_div_q30_pair_lanes(
        m03r, m03i, den0, &f_re, &f_im, 1, tmp_re, tmp_im, div_re, div_im,
        vl);
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
    mimo_rvv_solve4_submul_q30_q15_real(&m33r, m03r, m03i, f_re, f_im, vl);
#else
    mimo_rvv_solve4_submul_q30_q15(&m33r, &m33i, m03r, m03i, f_re, f_im, vl);
#endif
    mimo_rvv_solve4_submul_q30_q15(&r3r, &r3i, r0r, r0i, f_re, f_im, vl);

    mimo_rvv_solve4_store_safe_den(den1, m11r, vl);
    mimo_rvv_solve4_scalar_div_q30_pair_lanes(
        m12r, m12i, den1, &f_re, &f_im, 1, tmp_re, tmp_im, div_re, div_im,
        vl);
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
    mimo_rvv_solve4_submul_q30_q15_real(&m22r, m12r, m12i, f_re, f_im, vl);
#else
    mimo_rvv_solve4_submul_q30_q15(&m22r, &m22i, m12r, m12i, f_re, f_im, vl);
#endif
    mimo_rvv_solve4_submul_q30_q15(&m23r, &m23i, m13r, m13i, f_re, f_im, vl);
    mimo_rvv_solve4_submul_q30_q15(&r2r, &r2i, r1r, r1i, f_re, f_im, vl);

    mimo_rvv_solve4_scalar_div_q30_pair_lanes(
        m13r, m13i, den1, &f_re, &f_im, 1, tmp_re, tmp_im, div_re, div_im,
        vl);
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
    mimo_rvv_solve4_submul_q30_q15_real(&m33r, m13r, m13i, f_re, f_im, vl);
#else
    mimo_rvv_solve4_submul_q30_q15(&m33r, &m33i, m13r, m13i, f_re, f_im, vl);
#endif
    mimo_rvv_solve4_submul_q30_q15(&r3r, &r3i, r1r, r1i, f_re, f_im, vl);

    mimo_rvv_solve4_store_safe_den(den2, m22r, vl);
    mimo_rvv_solve4_scalar_div_q30_pair_lanes(
        m23r, m23i, den2, &f_re, &f_im, 1, tmp_re, tmp_im, div_re, div_im,
        vl);
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
    mimo_rvv_solve4_submul_q30_q15_real(&m33r, m23r, m23i, f_re, f_im, vl);
#else
    mimo_rvv_solve4_submul_q30_q15(&m33r, &m33i, m23r, m23i, f_re, f_im, vl);
#endif
    mimo_rvv_solve4_submul_q30_q15(&r3r, &r3i, r2r, r2i, f_re, f_im, vl);

    mimo_rvv_solve4_store_safe_den(den3, m33r, vl);
    mimo_rvv_solve4_scalar_div_q15_pair_lanes(
        r3r, r3i, den3, &x3r, &x3i, tmp_re, tmp_im, div_re, div_im, vl);

    mimo_rvv_solve4_submul_q15(&r2r, &r2i, m23r, m23i, x3r, x3i, vl);
    mimo_rvv_solve4_scalar_div_q15_pair_lanes(
        r2r, r2i, den2, &x2r, &x2i, tmp_re, tmp_im, div_re, div_im, vl);

    mimo_rvv_solve4_submul_q15(&r1r, &r1i, m12r, m12i, x2r, x2i, vl);
    mimo_rvv_solve4_submul_q15(&r1r, &r1i, m13r, m13i, x3r, x3i, vl);
    mimo_rvv_solve4_scalar_div_q15_pair_lanes(
        r1r, r1i, den1, &x1r, &x1i, tmp_re, tmp_im, div_re, div_im, vl);

    mimo_rvv_solve4_submul_q15(&r0r, &r0i, m01r, m01i, x1r, x1i, vl);
    mimo_rvv_solve4_submul_q15(&r0r, &r0i, m02r, m02i, x2r, x2i, vl);
    mimo_rvv_solve4_submul_q15(&r0r, &r0i, m03r, m03i, x3r, x3i, vl);
    mimo_rvv_solve4_scalar_div_q15_pair_lanes(
        r0r, r0i, den0, &x0r, &x0i, tmp_re, tmp_im, div_re, div_im, vl);

    mimo_rvv_solve4_store_x(x_block, 0, x0r, x0i, x_stride, vl);
    mimo_rvv_solve4_store_x(x_block, 1, x1r, x1i, x_stride, vl);
    mimo_rvv_solve4_store_x(x_block, 2, x2r, x2i, x_stride, vl);
    mimo_rvv_solve4_store_x(x_block, 3, x3r, x3i, x_stride, vl);

#undef MIMO_RVV_ZF4_MIX_A00
#undef MIMO_RVV_ZF4_MIX_A01
#undef MIMO_RVV_ZF4_MIX_A02
#undef MIMO_RVV_ZF4_MIX_A03
#undef MIMO_RVV_ZF4_MIX_A11
#undef MIMO_RVV_ZF4_MIX_A12
#undef MIMO_RVV_ZF4_MIX_A13
#undef MIMO_RVV_ZF4_MIX_A22
#undef MIMO_RVV_ZF4_MIX_A23
#undef MIMO_RVV_ZF4_MIX_A33
}

static MIMO_ALWAYS_INLINE void mimo_rvv_solve4_eliminate(
    mimo_c32_t a_lane[MIMO_SITE4_LANES][MIMO_N4 * MIMO_N4],
    mimo_c32_t b_lane[MIMO_SITE4_LANES][MIMO_N4],
    int k,
    int i,
    vint64m2_t pivot,
    ptrdiff_t a_stride,
    ptrdiff_t b_stride,
    size_t vl)
{
    const int ik = i * MIMO_N4 + k;
    const vint32m1_t factor_re =
        mimo_rvv_solve4_div_q30(
            mimo_rvv_solve4_load_a_re(a_lane, ik, a_stride, vl), pivot, vl);
    const vint32m1_t factor_im =
        mimo_rvv_solve4_div_q30(
            mimo_rvv_solve4_load_a_im(a_lane, ik, a_stride, vl), pivot, vl);
    vint32m1_t prod_re;
    vint32m1_t prod_im;

    for (int j = k + 1; j < MIMO_N4; ++j) {
        const int kj = k * MIMO_N4 + j;
        const int ij = i * MIMO_N4 + j;

        mimo_rvv_solve4_cmul_q30_q15(
            factor_re, factor_im,
            mimo_rvv_solve4_load_a_re(a_lane, kj, a_stride, vl),
            mimo_rvv_solve4_load_a_im(a_lane, kj, a_stride, vl),
            &prod_re, &prod_im, vl);
        mimo_rvv_solve4_store_a(
            a_lane, ij,
            __riscv_vsub_vv_i32m1(
                mimo_rvv_solve4_load_a_re(a_lane, ij, a_stride, vl),
                prod_re, vl),
            __riscv_vsub_vv_i32m1(
                mimo_rvv_solve4_load_a_im(a_lane, ij, a_stride, vl),
                prod_im, vl),
            a_stride, vl);
    }

    mimo_rvv_solve4_cmul_q30_q15(
        factor_re, factor_im,
        mimo_rvv_solve4_load_b_re(b_lane, k, b_stride, vl),
        mimo_rvv_solve4_load_b_im(b_lane, k, b_stride, vl),
        &prod_re, &prod_im, vl);
    mimo_rvv_solve4_store_b(
        b_lane, i,
        __riscv_vsub_vv_i32m1(
            mimo_rvv_solve4_load_b_re(b_lane, i, b_stride, vl), prod_re, vl),
        __riscv_vsub_vv_i32m1(
            mimo_rvv_solve4_load_b_im(b_lane, i, b_stride, vl), prod_im, vl),
        b_stride, vl);

    const vint32m1_t zero = __riscv_vmv_v_x_i32m1(0, vl);
    mimo_rvv_solve4_store_a(a_lane, ik, zero, zero, a_stride, vl);
}

static MIMO_ALWAYS_INLINE void mimo_rvv_solve4_store_x(
    mimo_c16_t *x_block,
    int tx,
    vint32m1_t re,
    vint32m1_t im,
    ptrdiff_t x_stride,
    size_t vl)
{
#if MIMO_USE_RVV_ZF4_PACK32_STORE
    const vint16mf2_t re16 = __riscv_vnclip_wx_i16mf2(re, 0, 0, vl);
    const vint16mf2_t im16 = __riscv_vnclip_wx_i16mf2(im, 0, 0, vl);
    const vint32m1_t re32 = __riscv_vwadd_vx_i32m1(re16, 0, vl);
    const vint32m1_t im32 = __riscv_vwadd_vx_i32m1(im16, 0, vl);
    const vint32m1_t re_lo = __riscv_vand_vx_i32m1(re32, 0xffff, vl);
    const vint32m1_t im_hi = __riscv_vsll_vx_i32m1(im32, 16, vl);
    const vint32m1_t packed = __riscv_vor_vv_i32m1(re_lo, im_hi, vl);

    __riscv_vsse32_v_i32m1((int32_t *)&x_block[tx], x_stride, packed, vl);
#else
    const vint16mf2_t re16 = __riscv_vnclip_wx_i16mf2(re, 0, 0, vl);
    const vint16mf2_t im16 = __riscv_vnclip_wx_i16mf2(im, 0, 0, vl);

    __riscv_vsse16_v_i16mf2(&x_block[tx].r, x_stride, re16, vl);
    __riscv_vsse16_v_i16mf2(&x_block[tx].i, x_stride, im16, vl);
#endif
}

#if MIMO_USE_RVV_SOLVE4_STREAMED
static MIMO_ALWAYS_INLINE void mimo_rvv_solve4_store_soa_a(
    mimo_site4_soa4_t *work,
    int idx,
    vint32m1_t re,
    vint32m1_t im,
    size_t vl)
{
    __riscv_vse32_v_i32m1(&work->a_re[idx][0], re, vl);
    __riscv_vse32_v_i32m1(&work->a_im[idx][0], im, vl);
}

static MIMO_ALWAYS_INLINE void mimo_rvv_solve4_store_soa_a_re(
    mimo_site4_soa4_t *work,
    int idx,
    vint32m1_t re,
    size_t vl)
{
    __riscv_vse32_v_i32m1(&work->a_re[idx][0], re, vl);
}

static MIMO_ALWAYS_INLINE void mimo_rvv_solve4_store_soa_b(
    mimo_site4_soa4_t *work,
    int idx,
    vint32m1_t re,
    vint32m1_t im,
    size_t vl)
{
    __riscv_vse32_v_i32m1(&work->b_re[idx][0], re, vl);
    __riscv_vse32_v_i32m1(&work->b_im[idx][0], im, vl);
}

static MIMO_ALWAYS_INLINE void mimo_rvv_solve4_eliminate_soa(
    mimo_site4_soa4_t *work,
    int k,
    int i,
    vint64m2_t pivot,
    size_t vl)
{
    const int ik = i * MIMO_N4 + k;
    const vint32m1_t factor_re =
        mimo_rvv_solve4_div_q30(
            mimo_rvv_solve4_load_soa_a_re(work, ik, vl), pivot, vl);
    const vint32m1_t factor_im =
        mimo_rvv_solve4_div_q30(
            mimo_rvv_solve4_load_soa_a_im(work, ik, vl), pivot, vl);

    for (int j = k + 1; j < MIMO_N4; ++j) {
        const int kj = k * MIMO_N4 + j;
        const int ij = i * MIMO_N4 + j;
        vint32m1_t dst_re = mimo_rvv_solve4_load_soa_a_re(work, ij, vl);
        vint32m1_t dst_im = mimo_rvv_solve4_load_soa_a_im(work, ij, vl);
        vint32m1_t prod_re;
        vint32m1_t prod_im;

        mimo_rvv_solve4_cmul_q30_q15(
            factor_re, factor_im,
            mimo_rvv_solve4_load_soa_a_re(work, kj, vl),
            mimo_rvv_solve4_load_soa_a_im(work, kj, vl),
            &prod_re, &prod_im, vl);
        dst_re = __riscv_vsub_vv_i32m1(dst_re, prod_re, vl);
        dst_im = __riscv_vsub_vv_i32m1(dst_im, prod_im, vl);
        mimo_rvv_solve4_store_soa_a(work, ij, dst_re, dst_im, vl);
    }

    {
        vint32m1_t dst_re = mimo_rvv_solve4_load_soa_b_re(work, i, vl);
        vint32m1_t dst_im = mimo_rvv_solve4_load_soa_b_im(work, i, vl);
        vint32m1_t prod_re;
        vint32m1_t prod_im;

        mimo_rvv_solve4_cmul_q30_q15(
            factor_re, factor_im,
            mimo_rvv_solve4_load_soa_b_re(work, k, vl),
            mimo_rvv_solve4_load_soa_b_im(work, k, vl),
            &prod_re, &prod_im, vl);
        dst_re = __riscv_vsub_vv_i32m1(dst_re, prod_re, vl);
        dst_im = __riscv_vsub_vv_i32m1(dst_im, prod_im, vl);
        mimo_rvv_solve4_store_soa_b(work, i, dst_re, dst_im, vl);
    }

}

static MIMO_ALWAYS_INLINE void mimo_rvv_solve4_backsub_soa(
    mimo_site4_soa4_t *work,
    int i,
    size_t vl)
{
    vint32m1_t sum_re = mimo_rvv_solve4_load_soa_b_re(work, i, vl);
    vint32m1_t sum_im = mimo_rvv_solve4_load_soa_b_im(work, i, vl);

    for (int j = i + 1; j < MIMO_N4; ++j) {
        vint32m1_t prod_re;
        vint32m1_t prod_im;
        const int ij = i * MIMO_N4 + j;

        mimo_rvv_solve4_cmul_q15(
            mimo_rvv_solve4_load_soa_a_re(work, ij, vl),
            mimo_rvv_solve4_load_soa_a_im(work, ij, vl),
            mimo_rvv_solve4_load_soa_b_re(work, j, vl),
            mimo_rvv_solve4_load_soa_b_im(work, j, vl),
            &prod_re, &prod_im, vl);
        sum_re = __riscv_vsub_vv_i32m1(sum_re, prod_re, vl);
        sum_im = __riscv_vsub_vv_i32m1(sum_im, prod_im, vl);
    }

    const int ii = i * MIMO_N4 + i;
    const vint64m2_t pivot =
        mimo_rvv_solve4_safe_pivot_i64(
            mimo_rvv_solve4_load_soa_a_re(work, ii, vl), vl);

    mimo_rvv_solve4_store_soa_b(
        work, i,
        mimo_rvv_solve4_div_q15(sum_re, pivot, vl),
        mimo_rvv_solve4_div_q15(sum_im, pivot, vl),
        vl);
}

static MIMO_ALWAYS_INLINE void mimo_rvv_solve_normal4_site4_q15(
    mimo_site4_soa4_t *work,
    int mmse,
    mimo_c16_t *x_block)
{
    const size_t vl = __riscv_vsetvl_e32m1(MIMO_SITE4_LANES);
    const ptrdiff_t x_stride = (ptrdiff_t)(MIMO_N4 * sizeof(mimo_c16_t));

    if (mmse) {
        mimo_rvv_solve4_store_soa_a_re(
            work, 0,
            __riscv_vadd_vx_i32m1(mimo_rvv_solve4_load_soa_a_re(work, 0, vl),
                                  MIMO_MMSE_LAMBDA_Q15, vl), vl);
        mimo_rvv_solve4_store_soa_a_re(
            work, 5,
            __riscv_vadd_vx_i32m1(mimo_rvv_solve4_load_soa_a_re(work, 5, vl),
                                  MIMO_MMSE_LAMBDA_Q15, vl), vl);
        mimo_rvv_solve4_store_soa_a_re(
            work, 10,
            __riscv_vadd_vx_i32m1(mimo_rvv_solve4_load_soa_a_re(work, 10, vl),
                                  MIMO_MMSE_LAMBDA_Q15, vl), vl);
        mimo_rvv_solve4_store_soa_a_re(
            work, 15,
            __riscv_vadd_vx_i32m1(mimo_rvv_solve4_load_soa_a_re(work, 15, vl),
                                  MIMO_MMSE_LAMBDA_Q15, vl), vl);
    }

    {
        vint64m2_t pivot = mimo_rvv_solve4_safe_pivot_i64(
            mimo_rvv_solve4_load_soa_a_re(work, 0, vl), vl);
        mimo_rvv_solve4_eliminate_soa(work, 0, 1, pivot, vl);
        mimo_rvv_solve4_eliminate_soa(work, 0, 2, pivot, vl);
        mimo_rvv_solve4_eliminate_soa(work, 0, 3, pivot, vl);
    }

    {
        vint64m2_t pivot = mimo_rvv_solve4_safe_pivot_i64(
            mimo_rvv_solve4_load_soa_a_re(work, 5, vl), vl);
        mimo_rvv_solve4_eliminate_soa(work, 1, 2, pivot, vl);
        mimo_rvv_solve4_eliminate_soa(work, 1, 3, pivot, vl);
    }

    {
        vint64m2_t pivot = mimo_rvv_solve4_safe_pivot_i64(
            mimo_rvv_solve4_load_soa_a_re(work, 10, vl), vl);
        mimo_rvv_solve4_eliminate_soa(work, 2, 3, pivot, vl);
    }

    mimo_rvv_solve4_backsub_soa(work, 3, vl);
    mimo_rvv_solve4_backsub_soa(work, 2, vl);
    mimo_rvv_solve4_backsub_soa(work, 1, vl);
    mimo_rvv_solve4_backsub_soa(work, 0, vl);

    mimo_rvv_solve4_store_x(x_block, 0,
                            mimo_rvv_solve4_load_soa_b_re(work, 0, vl),
                            mimo_rvv_solve4_load_soa_b_im(work, 0, vl),
                            x_stride, vl);
    mimo_rvv_solve4_store_x(x_block, 1,
                            mimo_rvv_solve4_load_soa_b_re(work, 1, vl),
                            mimo_rvv_solve4_load_soa_b_im(work, 1, vl),
                            x_stride, vl);
    mimo_rvv_solve4_store_x(x_block, 2,
                            mimo_rvv_solve4_load_soa_b_re(work, 2, vl),
                            mimo_rvv_solve4_load_soa_b_im(work, 2, vl),
                            x_stride, vl);
    mimo_rvv_solve4_store_x(x_block, 3,
                            mimo_rvv_solve4_load_soa_b_re(work, 3, vl),
                            mimo_rvv_solve4_load_soa_b_im(work, 3, vl),
                            x_stride, vl);
}

#else
static MIMO_ALWAYS_INLINE void mimo_rvv_solve_normal4_site4_q15(
    const mimo_site4_soa4_t *work,
    int mmse,
    mimo_c16_t *x_block)
{
    const size_t vl = __riscv_vsetvl_e32m1(MIMO_SITE4_LANES);
    const ptrdiff_t x_stride = (ptrdiff_t)(MIMO_N4 * sizeof(mimo_c16_t));
    const vint32m1_t zero = __riscv_vmv_v_x_i32m1(0, vl);
    vint32m1_t a00r, a01r, a01i, a02r, a02i, a03r, a03i;
    vint32m1_t a10r, a10i, a11r, a11i, a12r, a12i, a13r, a13i;
    vint32m1_t a20r, a20i, a21r, a21i, a22r, a22i, a23r, a23i;
    vint32m1_t a30r, a30i, a31r, a31i, a32r, a32i, a33r, a33i;
    vint32m1_t b0r, b0i, b1r, b1i, b2r, b2i, b3r, b3i;
    vint32m1_t x0_re, x0_im, x1_re, x1_im, x2_re, x2_im, x3_re, x3_im;

#define MIMO_RVV_SOLVE4_LOAD_A_PAIR(name, idx) \
    name##r = mimo_rvv_solve4_load_soa_a_re(work, idx, vl); \
    name##i = mimo_rvv_solve4_load_soa_a_im(work, idx, vl)
#define MIMO_RVV_SOLVE4_LOAD_B_PAIR(name, idx) \
    name##r = mimo_rvv_solve4_load_soa_b_re(work, idx, vl); \
    name##i = mimo_rvv_solve4_load_soa_b_im(work, idx, vl)

    a00r = mimo_rvv_solve4_load_soa_a_re(work, 0, vl);
    MIMO_RVV_SOLVE4_LOAD_A_PAIR(a01, 1);
    MIMO_RVV_SOLVE4_LOAD_A_PAIR(a02, 2);
    MIMO_RVV_SOLVE4_LOAD_A_PAIR(a03, 3);
    MIMO_RVV_SOLVE4_LOAD_A_PAIR(a10, 4);
    MIMO_RVV_SOLVE4_LOAD_A_PAIR(a11, 5);
    MIMO_RVV_SOLVE4_LOAD_A_PAIR(a12, 6);
    MIMO_RVV_SOLVE4_LOAD_A_PAIR(a13, 7);
    MIMO_RVV_SOLVE4_LOAD_A_PAIR(a20, 8);
    MIMO_RVV_SOLVE4_LOAD_A_PAIR(a21, 9);
    MIMO_RVV_SOLVE4_LOAD_A_PAIR(a22, 10);
    MIMO_RVV_SOLVE4_LOAD_A_PAIR(a23, 11);
    MIMO_RVV_SOLVE4_LOAD_A_PAIR(a30, 12);
    MIMO_RVV_SOLVE4_LOAD_A_PAIR(a31, 13);
    MIMO_RVV_SOLVE4_LOAD_A_PAIR(a32, 14);
    MIMO_RVV_SOLVE4_LOAD_A_PAIR(a33, 15);
    MIMO_RVV_SOLVE4_LOAD_B_PAIR(b0, 0);
    MIMO_RVV_SOLVE4_LOAD_B_PAIR(b1, 1);
    MIMO_RVV_SOLVE4_LOAD_B_PAIR(b2, 2);
    MIMO_RVV_SOLVE4_LOAD_B_PAIR(b3, 3);

#undef MIMO_RVV_SOLVE4_LOAD_A_PAIR
#undef MIMO_RVV_SOLVE4_LOAD_B_PAIR

    if (mmse) {
        a00r = __riscv_vadd_vx_i32m1(a00r, MIMO_MMSE_LAMBDA_Q15, vl);
        a11r = __riscv_vadd_vx_i32m1(a11r, MIMO_MMSE_LAMBDA_Q15, vl);
        a22r = __riscv_vadd_vx_i32m1(a22r, MIMO_MMSE_LAMBDA_Q15, vl);
        a33r = __riscv_vadd_vx_i32m1(a33r, MIMO_MMSE_LAMBDA_Q15, vl);
    }
    a11i = zero;
    a22i = zero;
    a33i = zero;

    vint64m2_t pivot = mimo_rvv_solve4_safe_pivot_i64(a00r, vl);
    const vint32m1_t f10r = mimo_rvv_solve4_div_q30(a10r, pivot, vl);
    const vint32m1_t f10i = mimo_rvv_solve4_div_q30(a10i, pivot, vl);
    const vint32m1_t f20r = mimo_rvv_solve4_div_q30(a20r, pivot, vl);
    const vint32m1_t f20i = mimo_rvv_solve4_div_q30(a20i, pivot, vl);
    const vint32m1_t f30r = mimo_rvv_solve4_div_q30(a30r, pivot, vl);
    const vint32m1_t f30i = mimo_rvv_solve4_div_q30(a30i, pivot, vl);

    mimo_rvv_solve4_submul_q30_q15(&a11r, &a11i, a01r, a01i, f10r, f10i, vl);
    mimo_rvv_solve4_submul_q30_q15(&a12r, &a12i, a02r, a02i, f10r, f10i, vl);
    mimo_rvv_solve4_submul_q30_q15(&a13r, &a13i, a03r, a03i, f10r, f10i, vl);
    mimo_rvv_solve4_submul_q30_q15(&b1r, &b1i, b0r, b0i, f10r, f10i, vl);

    mimo_rvv_solve4_submul_q30_q15(&a21r, &a21i, a01r, a01i, f20r, f20i, vl);
    mimo_rvv_solve4_submul_q30_q15(&a22r, &a22i, a02r, a02i, f20r, f20i, vl);
    mimo_rvv_solve4_submul_q30_q15(&a23r, &a23i, a03r, a03i, f20r, f20i, vl);
    mimo_rvv_solve4_submul_q30_q15(&b2r, &b2i, b0r, b0i, f20r, f20i, vl);

    mimo_rvv_solve4_submul_q30_q15(&a31r, &a31i, a01r, a01i, f30r, f30i, vl);
    mimo_rvv_solve4_submul_q30_q15(&a32r, &a32i, a02r, a02i, f30r, f30i, vl);
    mimo_rvv_solve4_submul_q30_q15(&a33r, &a33i, a03r, a03i, f30r, f30i, vl);
    mimo_rvv_solve4_submul_q30_q15(&b3r, &b3i, b0r, b0i, f30r, f30i, vl);

    pivot = mimo_rvv_solve4_safe_pivot_i64(a11r, vl);
    const vint32m1_t f21r = mimo_rvv_solve4_div_q30(a21r, pivot, vl);
    const vint32m1_t f21i = mimo_rvv_solve4_div_q30(a21i, pivot, vl);
    const vint32m1_t f31r = mimo_rvv_solve4_div_q30(a31r, pivot, vl);
    const vint32m1_t f31i = mimo_rvv_solve4_div_q30(a31i, pivot, vl);

    mimo_rvv_solve4_submul_q30_q15(&a22r, &a22i, a12r, a12i, f21r, f21i, vl);
    mimo_rvv_solve4_submul_q30_q15(&a23r, &a23i, a13r, a13i, f21r, f21i, vl);
    mimo_rvv_solve4_submul_q30_q15(&b2r, &b2i, b1r, b1i, f21r, f21i, vl);

    mimo_rvv_solve4_submul_q30_q15(&a32r, &a32i, a12r, a12i, f31r, f31i, vl);
    mimo_rvv_solve4_submul_q30_q15(&a33r, &a33i, a13r, a13i, f31r, f31i, vl);
    mimo_rvv_solve4_submul_q30_q15(&b3r, &b3i, b1r, b1i, f31r, f31i, vl);

    pivot = mimo_rvv_solve4_safe_pivot_i64(a22r, vl);
    const vint32m1_t f32r = mimo_rvv_solve4_div_q30(a32r, pivot, vl);
    const vint32m1_t f32i = mimo_rvv_solve4_div_q30(a32i, pivot, vl);

    mimo_rvv_solve4_submul_q30_q15(&a33r, &a33i, a23r, a23i, f32r, f32i, vl);
    mimo_rvv_solve4_submul_q30_q15(&b3r, &b3i, b2r, b2i, f32r, f32i, vl);

    pivot = mimo_rvv_solve4_safe_pivot_i64(a33r, vl);
    x3_re = mimo_rvv_solve4_div_q15(b3r, pivot, vl);
    x3_im = mimo_rvv_solve4_div_q15(b3i, pivot, vl);

    mimo_rvv_solve4_submul_q15(&b2r, &b2i, a23r, a23i, x3_re, x3_im, vl);
    pivot = mimo_rvv_solve4_safe_pivot_i64(a22r, vl);
    x2_re = mimo_rvv_solve4_div_q15(b2r, pivot, vl);
    x2_im = mimo_rvv_solve4_div_q15(b2i, pivot, vl);

    mimo_rvv_solve4_submul_q15(&b1r, &b1i, a12r, a12i, x2_re, x2_im, vl);
    mimo_rvv_solve4_submul_q15(&b1r, &b1i, a13r, a13i, x3_re, x3_im, vl);
    pivot = mimo_rvv_solve4_safe_pivot_i64(a11r, vl);
    x1_re = mimo_rvv_solve4_div_q15(b1r, pivot, vl);
    x1_im = mimo_rvv_solve4_div_q15(b1i, pivot, vl);

    mimo_rvv_solve4_submul_q15(&b0r, &b0i, a01r, a01i, x1_re, x1_im, vl);
    mimo_rvv_solve4_submul_q15(&b0r, &b0i, a02r, a02i, x2_re, x2_im, vl);
    mimo_rvv_solve4_submul_q15(&b0r, &b0i, a03r, a03i, x3_re, x3_im, vl);
    pivot = mimo_rvv_solve4_safe_pivot_i64(a00r, vl);
    x0_re = mimo_rvv_solve4_div_q15(b0r, pivot, vl);
    x0_im = mimo_rvv_solve4_div_q15(b0i, pivot, vl);

    mimo_rvv_solve4_store_x(x_block, 0, x0_re, x0_im, x_stride, vl);
    mimo_rvv_solve4_store_x(x_block, 1, x1_re, x1_im, x_stride, vl);
    mimo_rvv_solve4_store_x(x_block, 2, x2_re, x2_im, x_stride, vl);
    mimo_rvv_solve4_store_x(x_block, 3, x3_re, x3_im, x_stride, vl);
}
#endif

#endif
