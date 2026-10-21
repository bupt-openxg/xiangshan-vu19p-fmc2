#include <stddef.h>
#include <stdint.h>

#include "mimo_ext.h"
#include "mimo_kernels.h"

#ifndef MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID
#define MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID 0
#endif

#if MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID
#include <riscv_vector.h>
#endif

#if MIMO_ENABLE_EQ2
#ifndef MIMO_USE_EXT_EQ2_RHS_INTERLEAVE
#define MIMO_USE_EXT_EQ2_RHS_INTERLEAVE 0
#endif

#ifndef MIMO_USE_EXT_EQ2_MMSE_ADAPTIVE_Q13
#define MIMO_USE_EXT_EQ2_MMSE_ADAPTIVE_Q13 0
#endif

#if MIMO_USE_EXT_EQ2_RHS_INTERLEAVE && !MIMO_USE_EXT_EQ2_INVCOEFF
#error "EQ2 RHS interleave is defined only for the Q13 inverse-coefficient path"
#endif

#if MIMO_USE_EXT_EQ2_MMSE_ADAPTIVE_Q13 && \
    (!MIMO_USE_EXT_EQ2_INVCOEFF || MIMO_USE_EXT_EQ2_RHS_INTERLEAVE || \
     MIMO_USE_SOLVE_STORE_TRUST_I16)
#error "EQ2 MMSE adaptive Q13 requires inverse coefficients and saturated output"
#endif

#if MIMO_USE_EQ2_ZF_DIRECT_H && \
    (!MIMO_USE_EXT_EQ2_M2_BATCH || MIMO_USE_EXT_EQ2_INVCOEFF || \
     MIMO_USE_EXT_EQ2_RHS_INTERLEAVE)
#error "The initial EXT EQ2 direct-H path requires m2 batch and no Gramian solve knobs"
#endif

#if MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID && \
    (!MIMO_USE_EQ2_ZF_DIRECT_H || !MIMO_USE_EXT_EQ2_M2_BATCH || \
     MIMO_USE_EXT_EQ2_INVCOEFF || MIMO_USE_EXT_EQ2_RHS_INTERLEAVE || \
     MIMO_SELECTED_LAYOUT != MIMO_LAYOUT_EQ2_PACKED)
#error "EXT EQ2 hybrid guard1 requires direct-H, m2, no inverse-coefficient knobs, and packed input"
#endif

#if MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID && \
    (MIMO_EQ2_SITE_COUNT % MIMO_EQ2_PACKED_LANES != 0)
#error "EXT EQ2 hybrid guard1 requires complete 8-site blocks"
#endif

#ifndef MIMO_USE_EQ2_MMSE_DIRECT_H
#define MIMO_USE_EQ2_MMSE_DIRECT_H 0
#endif

#if MIMO_USE_EQ2_MMSE_DIRECT_H && \
    (!MIMO_USE_EXT_EQ2_M2_BATCH || !MIMO_USE_EXT_EQ2_INVCOEFF || \
     !MIMO_USE_EXT_EQ2_MMSE_ADAPTIVE_Q13 || \
     MIMO_USE_EXT_EQ2_RHS_INTERLEAVE || MIMO_USE_SOLVE_STORE_TRUST_I16)
#error "EXT EQ2 MMSE direct-H requires the m2 adaptive-Q13 no-harddiv fallback profile"
#endif

#if MIMO_USE_EQ2_MMSE_DIRECT_H && MIMO_MMSE_LAMBDA_Q15 != 512
#error "The EXT EQ2 MMSE direct-H shifts require lambda_q15=512"
#endif

#define MIMO_EQ2_EXT_DET_Q15_MIN 3400u
#define MIMO_SCHUR4_DET_Q15_RECIP_DIRECT1_ENTRY(rel_) \
    ((uint16_t)((1073741824ull / \
                 (MIMO_EQ2_EXT_DET_Q15_MIN + (uint32_t)(rel_))) \
                >> 4))

static const uint16_t mimo_eq2_ext_recip_q26_padded[] MIMO_ALIGN64 = {
    [MIMO_EQ2_EXT_DET_Q15_MIN] =
#include "mimo_schur_recip_direct1.inc"
};

#undef MIMO_SCHUR4_DET_Q15_RECIP_DIRECT1_ENTRY

#if MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID
#define MIMO_EQ2_EXT_DET_Q16_MIN 6800u
#define MIMO_SCHUR4_DET_Q15_RECIP_DIRECT1_ENTRY(rel_) \
    ((uint16_t)(134217728ull / \
                (MIMO_EQ2_EXT_DET_Q16_MIN + 2u * (uint32_t)(rel_)))), \
    ((uint16_t)(134217728ull / \
                (MIMO_EQ2_EXT_DET_Q16_MIN + 2u * (uint32_t)(rel_) + 1u)))

static const uint16_t mimo_eq2_ext_recip_q27_half_padded[] MIMO_ALIGN64 = {
    [MIMO_EQ2_EXT_DET_Q16_MIN] =
#include "mimo_schur_recip_direct1.inc"
};

#undef MIMO_SCHUR4_DET_Q15_RECIP_DIRECT1_ENTRY

static const uint16_t mimo_eq2_ext_pack_index[2 * MIMO_EQ2_PACKED_LANES]
    MIMO_ALIGN64 = {
        0, 8, 1, 9, 2, 10, 3, 11,
        4, 12, 5, 13, 6, 14, 7, 15,
    };

static MIMO_ALWAYS_INLINE int16_t mimo_eq2_ext_scalar_rnu_q15(
    int64_t value)
{
    return mimo_sat_i16((int32_t)((value + (1ll << 14)) >> 15));
}

static MIMO_ALWAYS_INLINE int16_t mimo_eq2_ext_scalar_rnu_q16(
    int64_t value)
{
    return mimo_sat_i16((int32_t)((value + (1ll << 13)) >> 14));
}

static MIMO_ALWAYS_INLINE int16_t mimo_eq2_ext_scalar_wrap_lshift1(
    int16_t value)
{
    return (int16_t)((uint16_t)value << 1);
}

static MIMO_ALWAYS_INLINE mimo_c16_t mimo_eq2_ext_scalar_cmul_q15(
    mimo_c16_t a,
    mimo_c16_t b,
    int conjugate_a)
{
    const int64_t ar = a.r;
    const int64_t ai = conjugate_a ? -(int64_t)a.i : (int64_t)a.i;
    const int64_t br = b.r;
    const int64_t bi = b.i;

    return mimo_make_c16(
        mimo_eq2_ext_scalar_rnu_q15(ar * br - ai * bi),
        mimo_eq2_ext_scalar_rnu_q15(ar * bi + ai * br));
}

static MIMO_NOINLINE void mimo_eq2_ext_guard1_packed_block(
    const mimo_c16_t *restrict p0,
    const mimo_c16_t *restrict p1,
    const mimo_c16_t *restrict py,
    mimo_c16_t *restrict px)
{
    for (size_t lane = 0; lane < MIMO_EQ2_PACKED_LANES; ++lane) {
        const mimo_c16_t h00 = p0[lane];
        const mimo_c16_t h10 = p0[MIMO_EQ2_PACKED_LANES + lane];
        const mimo_c16_t h01 = p1[lane];
        const mimo_c16_t h11 = p1[MIMO_EQ2_PACKED_LANES + lane];
        const mimo_c16_t y0 = py[lane];
        const mimo_c16_t y1 = py[MIMO_EQ2_PACKED_LANES + lane];
        const mimo_c16_t determinant = mimo_make_c16(
            mimo_eq2_ext_scalar_rnu_q16(
                (int64_t)h00.r * h11.r - (int64_t)h00.i * h11.i -
                (int64_t)h01.r * h10.r + (int64_t)h01.i * h10.i),
            mimo_eq2_ext_scalar_rnu_q16(
                (int64_t)h00.r * h11.i + (int64_t)h00.i * h11.r -
                (int64_t)h01.r * h10.i - (int64_t)h01.i * h10.r));
        const mimo_c16_t numerator0 = mimo_make_c16(
            mimo_eq2_ext_scalar_rnu_q16(
                (int64_t)h11.r * y0.r - (int64_t)h11.i * y0.i -
                (int64_t)h01.r * y1.r + (int64_t)h01.i * y1.i),
            mimo_eq2_ext_scalar_rnu_q16(
                (int64_t)h11.r * y0.i + (int64_t)h11.i * y0.r -
                (int64_t)h01.r * y1.i - (int64_t)h01.i * y1.r));
        const mimo_c16_t numerator1 = mimo_make_c16(
            mimo_eq2_ext_scalar_rnu_q16(
                (int64_t)h00.r * y1.r - (int64_t)h00.i * y1.i -
                (int64_t)h10.r * y0.r + (int64_t)h10.i * y0.i),
            mimo_eq2_ext_scalar_rnu_q16(
                (int64_t)h00.r * y1.i + (int64_t)h00.i * y1.r -
                (int64_t)h10.r * y0.i - (int64_t)h10.i * y0.r));
        const mimo_c16_t det_work = mimo_eq2_ext_scalar_cmul_q15(
            determinant, determinant, 1);
        const uint32_t det_q16 = (uint32_t)(((int32_t)det_work.r + 1) >> 1);
        const int16_t table_scale =
            (int16_t)mimo_eq2_ext_recip_q27_half_padded[det_q16];
        const mimo_c16_t inverse_base = mimo_eq2_ext_scalar_cmul_q15(
            determinant, mimo_make_c16(table_scale, 0), 1);
        const mimo_c16_t inverse = mimo_make_c16(
            mimo_eq2_ext_scalar_wrap_lshift1(inverse_base.r),
            mimo_eq2_ext_scalar_wrap_lshift1(inverse_base.i));
        const mimo_c16_t value0 = mimo_eq2_ext_scalar_cmul_q15(
            numerator0, inverse, 0);
        const mimo_c16_t value1 = mimo_eq2_ext_scalar_cmul_q15(
            numerator1, inverse, 0);

        px[lane] = mimo_make_c16(
            mimo_sat_i16((int32_t)value0.r + value0.r),
            mimo_sat_i16((int32_t)value0.i + value0.i));
        px[MIMO_EQ2_PACKED_LANES + lane] = mimo_make_c16(
            mimo_sat_i16((int32_t)value1.r + value1.r),
            mimo_sat_i16((int32_t)value1.i + value1.i));
    }
}
#endif

__asm__(
    ".macro mimo_eq2_get_vreg_num outreg, invreg\n\t"
    "  .set \\outreg, -1\n\t"
    "  .set _mimo_eq2_vreg_i, 0\n\t"
    "  .irp v, v0,v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13,v14,v15,v16,v17,v18,v19,v20,v21,v22,v23,v24,v25,v26,v27,v28,v29,v30,v31\n\t"
    "    .ifc \\invreg, \\v\n\t"
    "      .set \\outreg, _mimo_eq2_vreg_i\n\t"
    "    .endif\n\t"
    "    .set _mimo_eq2_vreg_i, _mimo_eq2_vreg_i + 1\n\t"
    "  .endr\n\t"
    "  .if \\outreg < 0\n\t"
    "    .error \"Invalid EQ2 vector register mapping\"\n\t"
    "  .endif\n\t"
    ".endm\n\t"
    ".macro mimo_eq2_emit_ext_vv base, vd, vs2, vs1\n\t"
    "  mimo_eq2_get_vreg_num _mimo_eq2_vd, \\vd\n\t"
    "  mimo_eq2_get_vreg_num _mimo_eq2_vs2, \\vs2\n\t"
    "  mimo_eq2_get_vreg_num _mimo_eq2_vs1, \\vs1\n\t"
    "  .word \\base | (_mimo_eq2_vd << 7) | (_mimo_eq2_vs1 << 15) | (_mimo_eq2_vs2 << 20)\n\t"
    ".endm\n\t"
    ".macro mimo_eq2_vscmul vd, vs2, vs1\n\t"
    "  mimo_eq2_emit_ext_vv 0xCA000057, \\vd, \\vs2, \\vs1\n\t"
    ".endm\n\t"
    ".macro mimo_eq2_vscmulcj vd, vs2, vs1\n\t"
    "  mimo_eq2_emit_ext_vv 0xCE000057, \\vd, \\vs2, \\vs1\n\t"
    ".endm\n\t"
    ".macro mimo_eq2_vscmacc vd, vs2, vs1\n\t"
    "  mimo_eq2_emit_ext_vv 0xD2000057, \\vd, \\vs2, \\vs1\n\t"
    ".endm\n\t"
    ".macro mimo_eq2_vscmacccj vd, vs2, vs1\n\t"
    "  mimo_eq2_emit_ext_vv 0xDA000057, \\vd, \\vs2, \\vs1\n\t"
    ".endm\n\t"
    ".macro mimo_eq2_vscaddconj vd, vs2, vs1\n\t"
    "  mimo_eq2_emit_ext_vv 0xDE000057, \\vd, \\vs2, \\vs1\n\t"
    ".endm\n\t");

__asm__(
    ".macro mimo_eq2_join_m1_pair vd, lo, hi\n\t"
    "  mimo_eq2_get_vreg_num _mimo_eq2_join_vd, \\vd\n\t"
    "  mimo_eq2_get_vreg_num _mimo_eq2_join_lo, \\lo\n\t"
    "  mimo_eq2_get_vreg_num _mimo_eq2_join_hi, \\hi\n\t"
    "  .if (_mimo_eq2_join_vd & 1) || (_mimo_eq2_join_vd >= 31)\n\t"
    "    .error \"EQ2 joined m2 destination must be even and below v31\"\n\t"
    "  .endif\n\t"
    "  .word 0x9E003057 | (_mimo_eq2_join_vd << 7) | "
    "(_mimo_eq2_join_lo << 20)\n\t"
    "  .word 0x9E003057 | ((_mimo_eq2_join_vd + 1) << 7) | "
    "(_mimo_eq2_join_hi << 20)\n\t"
    ".endm\n\t");

#if MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID
static MIMO_ALWAYS_INLINE vint16m2_t mimo_eq2_ext_pack_complex_m2(
    vint16m1_t real,
    vint16m1_t imag,
    vuint16m2_t index)
{
    vint16m2_t joined;
    vint16m2_t packed;

    asm volatile(
        "mimo_eq2_join_m1_pair %[joined], %[real], %[imag]\n\t"
        "vrgather.vv %[packed], %[joined], %[index]"
        : [joined] "=&vr"(joined), [packed] "=&vr"(packed)
        : [real] "vr"(real), [imag] "vr"(imag),
          [index] "vr"(index));
    return packed;
}

static MIMO_ALWAYS_INLINE void mimo_eq2_ext_hybrid_solve_block(
    vint16m2_t determinant,
    vint16m2_t numerator0,
    vint16m2_t numerator1,
    int16_t *restrict output)
{
    vint16m2_t work;
    vint16m2_t table_scale;
    vint16m2_t output0;
    vint16m2_t output1_value;
    int16_t *const output1 = output + 2 * MIMO_EQ2_PACKED_LANES;

    asm volatile(
        "mimo_eq2_vscmulcj %[work], %[d], %[d]\n\t"
        "vssra.vi %[work], %[work], 1\n\t"
        "vsll.vi %[work], %[work], 1\n\t"
        "vluxei16.v %[table_scale], (%[table]), %[work]\n\t"
        "mimo_eq2_vscmulcj %[work], %[d], %[table_scale]\n\t"
        "vsll.vi %[work], %[work], 1\n\t"
        "mimo_eq2_vscmul %[result0], %[n0], %[work]\n\t"
        "mimo_eq2_vscmul %[result1], %[n1], %[work]\n\t"
        "vsadd.vv %[result0], %[result0], %[result0]\n\t"
        "vsadd.vv %[result1], %[result1], %[result1]\n\t"
        "vse16.v %[result0], (%[out0])\n\t"
        "vse16.v %[result1], (%[out1])"
        : [work] "=&vr"(work), [table_scale] "=&vr"(table_scale),
          [result0] "=&vr"(output0), [result1] "=&vr"(output1_value)
        : [d] "vr"(determinant), [n0] "vr"(numerator0),
          [n1] "vr"(numerator1),
          [table] "r"(mimo_eq2_ext_recip_q27_half_padded),
          [out0] "r"(output), [out1] "r"(output1)
        : "memory");
}

static MIMO_ALWAYS_INLINE void mimo_eq2_ext_direct_h_guard1_hybrid_run(
    const int16_t *restrict h_ri_soa,
    const int16_t *restrict y_ri_soa,
    mimo_c16_t *restrict x)
{
    const size_t site_count = MIMO_EQ2_SITE_COUNT;
    const int16_t *const h00r_base = &h_ri_soa[0 * site_count];
    const int16_t *const h00i_base = &h_ri_soa[1 * site_count];
    const int16_t *const h01r_base = &h_ri_soa[2 * site_count];
    const int16_t *const h01i_base = &h_ri_soa[3 * site_count];
    const int16_t *const h10r_base = &h_ri_soa[4 * site_count];
    const int16_t *const h10i_base = &h_ri_soa[5 * site_count];
    const int16_t *const h11r_base = &h_ri_soa[6 * site_count];
    const int16_t *const h11i_base = &h_ri_soa[7 * site_count];
    const int16_t *const y0r_base = &y_ri_soa[0 * site_count];
    const int16_t *const y0i_base = &y_ri_soa[1 * site_count];
    const int16_t *const y1r_base = &y_ri_soa[2 * site_count];
    const int16_t *const y1i_base = &y_ri_soa[3 * site_count];
    int16_t *output = (int16_t *)(void *)x;
    const size_t packed_vl = __riscv_vsetvl_e16m2(
        2 * MIMO_EQ2_PACKED_LANES);
    const vuint16m2_t pack_index = __riscv_vle16_v_u16m2(
        mimo_eq2_ext_pack_index, packed_vl);

    mimo_ext_set_vxrm_rnu_once();
    for (size_t site = 0; site < site_count;) {
        const size_t vl = __riscv_vsetvl_e16m1(site_count - site);
        const vint16m1_t h00r = __riscv_vle16_v_i16m1(&h00r_base[site], vl);
        const vint16m1_t h00i = __riscv_vle16_v_i16m1(&h00i_base[site], vl);
        const vint16m1_t h01r = __riscv_vle16_v_i16m1(&h01r_base[site], vl);
        const vint16m1_t h01i = __riscv_vle16_v_i16m1(&h01i_base[site], vl);
        const vint16m1_t h10r = __riscv_vle16_v_i16m1(&h10r_base[site], vl);
        const vint16m1_t h10i = __riscv_vle16_v_i16m1(&h10i_base[site], vl);
        const vint16m1_t h11r = __riscv_vle16_v_i16m1(&h11r_base[site], vl);
        const vint16m1_t h11i = __riscv_vle16_v_i16m1(&h11i_base[site], vl);
        const vint16m1_t y0r = __riscv_vle16_v_i16m1(&y0r_base[site], vl);
        const vint16m1_t y0i = __riscv_vle16_v_i16m1(&y0i_base[site], vl);
        const vint16m1_t y1r = __riscv_vle16_v_i16m1(&y1r_base[site], vl);
        const vint16m1_t y1i = __riscv_vle16_v_i16m1(&y1i_base[site], vl);
        const vint16m1_t neg_h00i = __riscv_vneg_v_i16m1(h00i, vl);
        const vint16m1_t neg_h01r = __riscv_vneg_v_i16m1(h01r, vl);
        const vint16m1_t neg_h01i = __riscv_vneg_v_i16m1(h01i, vl);
        const vint16m1_t neg_h10r = __riscv_vneg_v_i16m1(h10r, vl);
        const vint16m1_t neg_h10i = __riscv_vneg_v_i16m1(h10i, vl);
        const vint16m1_t neg_h11i = __riscv_vneg_v_i16m1(h11i, vl);
        vint32m2_t dr_q30 = __riscv_vwmul_vv_i32m2(h00r, h11r, vl);
        vint32m2_t di_q30 = __riscv_vwmul_vv_i32m2(h00r, h11i, vl);
        vint32m2_t n0r_q30 = __riscv_vwmul_vv_i32m2(h11r, y0r, vl);
        vint32m2_t n0i_q30 = __riscv_vwmul_vv_i32m2(h11r, y0i, vl);

        dr_q30 = __riscv_vwmacc_vv_i32m2(dr_q30, neg_h00i, h11i, vl);
        dr_q30 = __riscv_vwmacc_vv_i32m2(dr_q30, neg_h01r, h10r, vl);
        dr_q30 = __riscv_vwmacc_vv_i32m2(dr_q30, h01i, h10i, vl);
        di_q30 = __riscv_vwmacc_vv_i32m2(di_q30, h00i, h11r, vl);
        di_q30 = __riscv_vwmacc_vv_i32m2(di_q30, neg_h01r, h10i, vl);
        di_q30 = __riscv_vwmacc_vv_i32m2(di_q30, neg_h01i, h10r, vl);
        n0r_q30 = __riscv_vwmacc_vv_i32m2(n0r_q30, neg_h11i, y0i, vl);
        n0r_q30 = __riscv_vwmacc_vv_i32m2(n0r_q30, neg_h01r, y1r, vl);
        n0r_q30 = __riscv_vwmacc_vv_i32m2(n0r_q30, h01i, y1i, vl);
        n0i_q30 = __riscv_vwmacc_vv_i32m2(n0i_q30, h11i, y0r, vl);
        n0i_q30 = __riscv_vwmacc_vv_i32m2(n0i_q30, neg_h01r, y1i, vl);
        n0i_q30 = __riscv_vwmacc_vv_i32m2(n0i_q30, neg_h01i, y1r, vl);

        const vint16m1_t dr = __riscv_vnclip_wx_i16m1(dr_q30, 14, 0, vl);
        const vint16m1_t di = __riscv_vnclip_wx_i16m1(di_q30, 14, 0, vl);
        const vint16m1_t n0r = __riscv_vnclip_wx_i16m1(n0r_q30, 14, 0, vl);
        const vint16m1_t n0i = __riscv_vnclip_wx_i16m1(n0i_q30, 14, 0, vl);
        vint32m2_t n1r_q30 = __riscv_vwmul_vv_i32m2(h00r, y1r, vl);
        vint32m2_t n1i_q30 = __riscv_vwmul_vv_i32m2(h00r, y1i, vl);

        n1r_q30 = __riscv_vwmacc_vv_i32m2(n1r_q30, neg_h00i, y1i, vl);
        n1r_q30 = __riscv_vwmacc_vv_i32m2(n1r_q30, neg_h10r, y0r, vl);
        n1r_q30 = __riscv_vwmacc_vv_i32m2(n1r_q30, h10i, y0i, vl);
        n1i_q30 = __riscv_vwmacc_vv_i32m2(n1i_q30, h00i, y1r, vl);
        n1i_q30 = __riscv_vwmacc_vv_i32m2(n1i_q30, neg_h10r, y0i, vl);
        n1i_q30 = __riscv_vwmacc_vv_i32m2(n1i_q30, neg_h10i, y0r, vl);

        const vint16m1_t n1r = __riscv_vnclip_wx_i16m1(n1r_q30, 14, 0, vl);
        const vint16m1_t n1i = __riscv_vnclip_wx_i16m1(n1i_q30, 14, 0, vl);
        asm volatile(
            "vsetivli zero, 16, e16, m2, ta, ma"
            :
            : "vr"(dr), "vr"(di), "vr"(n0r), "vr"(n0i),
              "vr"(n1r), "vr"(n1i), "vr"(pack_index));
        const vint16m2_t determinant = mimo_eq2_ext_pack_complex_m2(
            dr, di, pack_index);
        const vint16m2_t numerator0 = mimo_eq2_ext_pack_complex_m2(
            n0r, n0i, pack_index);
        const vint16m2_t numerator1 = mimo_eq2_ext_pack_complex_m2(
            n1r, n1i, pack_index);

        mimo_eq2_ext_hybrid_solve_block(
            determinant, numerator0, numerator1, output);
        output += 4 * MIMO_EQ2_PACKED_LANES;
        site += vl;
    }
}

static MIMO_ALWAYS_INLINE void mimo_eq2_ext_direct_h_guard1_handasm_run(
    const int16_t *restrict h_ri_soa,
    const int16_t *restrict y_ri_soa,
    mimo_c16_t *restrict x)
{
    const uintptr_t component_bytes =
        MIMO_EQ2_SITE_COUNT * sizeof(*h_ri_soa);
    const size_t blocks = MIMO_EQ2_SITE_COUNT / MIMO_EQ2_PACKED_LANES;

    asm volatile(
        "vsetivli zero, 16, e16, m2, ta, ma\n\t"
        "vle16.v v30, (%[pack_index])\n\t"
        "csrwi vxrm, 0\n\t"
        "mv t0, %[h]\n\t"
        "mv s4, %[component_bytes]\n\t"
        "add t1, t0, s4\n\t"
        "add t2, t1, s4\n\t"
        "add t3, t2, s4\n\t"
        "add t4, t3, s4\n\t"
        "add t5, t4, s4\n\t"
        "add t6, t5, s4\n\t"
        "add a3, t6, s4\n\t"
        "mv a4, %[y]\n\t"
        "add a5, a4, s4\n\t"
        "add a6, a5, s4\n\t"
        "add a7, a6, s4\n\t"
        "mv s0, %[output]\n\t"
        "addi s3, s0, 32\n\t"
        "mv s1, %[table]\n\t"
        "mv s2, %[blocks]\n\t"
        "vsetivli zero, 8, e16, m1, ta, ma\n\t"
        "1:\n\t"
        "vle16.v v0, (t0)\n\t"
        "vle16.v v1, (t1)\n\t"
        "vle16.v v2, (t2)\n\t"
        "vle16.v v3, (t3)\n\t"
        "vle16.v v4, (t4)\n\t"
        "vle16.v v5, (t5)\n\t"
        "vle16.v v6, (t6)\n\t"
        "vle16.v v7, (a3)\n\t"
        "vle16.v v8, (a4)\n\t"
        "vle16.v v9, (a5)\n\t"
        "vle16.v v10, (a6)\n\t"
        "vle16.v v11, (a7)\n\t"
        "vneg.v v12, v1\n\t"
        "vneg.v v13, v2\n\t"
        "vneg.v v14, v3\n\t"
        "vneg.v v15, v4\n\t"
        "vneg.v v16, v5\n\t"
        "vneg.v v17, v7\n\t"
        "vwmul.vv v18, v0, v6\n\t"
        "vwmacc.vv v18, v12, v7\n\t"
        "vwmacc.vv v18, v13, v4\n\t"
        "vwmacc.vv v18, v3, v5\n\t"
        "vwmul.vv v20, v0, v7\n\t"
        "vwmacc.vv v20, v1, v6\n\t"
        "vwmacc.vv v20, v13, v5\n\t"
        "vwmacc.vv v20, v14, v4\n\t"
        "vwmul.vv v22, v6, v8\n\t"
        "vwmacc.vv v22, v17, v9\n\t"
        "vwmacc.vv v22, v13, v10\n\t"
        "vwmacc.vv v22, v3, v11\n\t"
        "vwmul.vv v24, v6, v9\n\t"
        "vwmacc.vv v24, v7, v8\n\t"
        "vwmacc.vv v24, v13, v11\n\t"
        "vwmacc.vv v24, v14, v10\n\t"
        "vwmul.vv v26, v0, v10\n\t"
        "vwmacc.vv v26, v12, v11\n\t"
        "vwmacc.vv v26, v15, v8\n\t"
        "vwmacc.vv v26, v5, v9\n\t"
        "vwmul.vv v28, v0, v11\n\t"
        "vwmacc.vv v28, v1, v10\n\t"
        "vwmacc.vv v28, v15, v9\n\t"
        "vwmacc.vv v28, v16, v8\n\t"
        "vnclip.wi v18, v18, 14\n\t"
        "vnclip.wi v20, v20, 14\n\t"
        "vnclip.wi v22, v22, 14\n\t"
        "vnclip.wi v24, v24, 14\n\t"
        "vnclip.wi v26, v26, 14\n\t"
        "vnclip.wi v28, v28, 14\n\t"
        "vmv1r.v v0, v18\n\t"
        "vmv1r.v v1, v20\n\t"
        "vmv1r.v v4, v22\n\t"
        "vmv1r.v v5, v24\n\t"
        "vmv1r.v v8, v26\n\t"
        "vmv1r.v v9, v28\n\t"
        "vsetivli zero, 16, e16, m2, ta, ma\n\t"
        "vrgather.vv v2, v0, v30\n\t"
        "vrgather.vv v6, v4, v30\n\t"
        "vrgather.vv v10, v8, v30\n\t"
        "mimo_eq2_vscmulcj v12, v2, v2\n\t"
        "vssra.vi v12, v12, 1\n\t"
        "vsll.vi v12, v12, 1\n\t"
        "vluxei16.v v14, (s1), v12\n\t"
        "mimo_eq2_vscmulcj v12, v2, v14\n\t"
        "vsll.vi v12, v12, 1\n\t"
        "mimo_eq2_vscmul v16, v6, v12\n\t"
        "mimo_eq2_vscmul v18, v10, v12\n\t"
        "vsadd.vv v16, v16, v16\n\t"
        "vsadd.vv v18, v18, v18\n\t"
        "vse16.v v16, (s0)\n\t"
        "vse16.v v18, (s3)\n\t"
        "addi t0, t0, 16\n\t"
        "addi t1, t1, 16\n\t"
        "addi t2, t2, 16\n\t"
        "addi t3, t3, 16\n\t"
        "addi t4, t4, 16\n\t"
        "addi t5, t5, 16\n\t"
        "addi t6, t6, 16\n\t"
        "addi a3, a3, 16\n\t"
        "addi a4, a4, 16\n\t"
        "addi a5, a5, 16\n\t"
        "addi a6, a6, 16\n\t"
        "addi a7, a7, 16\n\t"
        "addi s0, s0, 64\n\t"
        "addi s3, s3, 64\n\t"
        "addi s2, s2, -1\n\t"
        "vsetivli zero, 8, e16, m1, ta, ma\n\t"
        "bnez s2, 1b"
        :
        : [h] "r"(h_ri_soa), [y] "r"(y_ri_soa),
          [output] "r"((int16_t *)(void *)x),
          [component_bytes] "r"(component_bytes), [blocks] "r"(blocks),
          [pack_index] "r"(mimo_eq2_ext_pack_index),
          [table] "r"(mimo_eq2_ext_recip_q27_half_padded)
        : "t0", "t1", "t2", "t3", "t4", "t5", "t6",
          "a3", "a4", "a5", "a6", "a7",
          "s0", "s1", "s2", "s3", "s4",
          "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
          "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15",
          "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23",
          "v24", "v25", "v26", "v27", "v28", "v29", "v30", "v31",
          "memory");
}

static MIMO_ALWAYS_INLINE void mimo_eq2_ext_direct_h_adaptive_run(
    const mimo_c16_t *restrict h,
    const mimo_c16_t *restrict y,
    mimo_c16_t *restrict x)
{
    const size_t column_complex = MIMO_N2 * MIMO_EQ2_PACKED_LANES;
    const size_t h_block_complex = MIMO_N2 * column_complex;
    const size_t y_block_complex = MIMO_N2 * MIMO_EQ2_PACKED_LANES;
    const mimo_c16_t *p0 = h;
    const mimo_c16_t *p1 = h + column_complex;
    const mimo_c16_t *py = y;
    mimo_c16_t *px = x;
    const size_t blocks = MIMO_EQ2_SITE_COUNT / MIMO_EQ2_PACKED_LANES;

    mimo_ext_set_vxrm_rnu_once();
    for (size_t block = 0; block < blocks; ++block) {
        size_t risk_lanes;

        asm volatile(
            "vsetvli t0, zero, e16, m4, ta, ma\n\t"
            "vle16.v v0, (%[p0])\n\t"
            "vle16.v v4, (%[p1])\n\t"
            "vle16.v v8, (%[py])\n\t"
            "vsetvli t0, zero, e16, m2, ta, ma\n\t"
            "vneg.v v12, v4\n\t"
            "vneg.v v18, v2\n\t"
            "mimo_eq2_vscmulcj v28, v0, v0\n\t"
            "mimo_eq2_vscmul v14, v0, v6\n\t"
            "mimo_eq2_vscmacccj v28, v2, v2\n\t"
            "mimo_eq2_vscmacc v14, v12, v2\n\t"
            "mimo_eq2_vscmacccj v28, v4, v4\n\t"
            "mimo_eq2_vscmul v16, v6, v8\n\t"
            "mimo_eq2_vscmacccj v28, v6, v6\n\t"
            "mimo_eq2_vscmacc v16, v12, v10\n\t"
            "mimo_eq2_vscmulcj v28, v28, v28\n\t"
            "mimo_eq2_vscmul v20, v0, v10\n\t"
            "mimo_eq2_vscmacc v20, v18, v8\n\t"
            "mimo_eq2_vscmulcj v22, v14, v14\n\t"
            "vsrl.vi v28, v28, 1\n\t"
            "vsll.vi v30, v22, 1\n\t"
            "vsrl.vi v24, v22, 2\n\t"
            "vadd.vv v30, v30, v24\n\t"
            "vmsgtu.vv v0, v28, v30\n\t"
            "vcpop.m %[risk], v0\n\t"
            "vsll.vi v22, v22, 1\n\t"
            "vluxei16.v v12, (%[table]), v22\n\t"
            "mimo_eq2_vscmulcj v22, v14, v12\n\t"
            "vsll.vi v22, v22, 2\n\t"
            "mimo_eq2_vscmul v24, v16, v22\n\t"
            "mimo_eq2_vscmul v26, v20, v22\n\t"
            "vsetvli t0, zero, e16, m4, ta, ma\n\t"
            "vsadd.vv v24, v24, v24\n\t"
            "vsadd.vv v24, v24, v24\n\t"
            "vse16.v v24, (%[px])"
            : [risk] "=&r"(risk_lanes)
            : [p0] "r"(p0), [p1] "r"(p1), [py] "r"(py),
              [px] "r"(px),
              [table] "r"(mimo_eq2_ext_recip_q26_padded)
            : "t0", "v0", "v1", "v2", "v3", "v4", "v5",
              "v6", "v7", "v8", "v9", "v10", "v11", "v12",
              "v13", "v14", "v15", "v16", "v17", "v18", "v19",
              "v20", "v21", "v22", "v23", "v24", "v25", "v26",
              "v27", "v28", "v29", "v30", "v31", "memory");

        if (risk_lanes != 0) {
            mimo_eq2_ext_guard1_packed_block(p0, p1, py, px);
        }
        p0 += h_block_complex;
        p1 += h_block_complex;
        py += y_block_complex;
        px += y_block_complex;
    }
}
#endif

#if MIMO_USE_EQ2_ZF_DIRECT_H
#if MIMO_USE_SOLVE_STORE_TRUST_I16
#define MIMO_EQ2_EXT_DIRECT_OUTPUT_ASM \
        "vsll.vi v24, v24, 2\n\t"
#else
#define MIMO_EQ2_EXT_DIRECT_OUTPUT_ASM \
        "vsadd.vv v24, v24, v24\n\t" \
        "vsadd.vv v24, v24, v24\n\t"
#endif

#define MIMO_EQ2_EXT_DIRECT_H_LOOP() \
    asm volatile( \
        "vsetvli t0, zero, e16, m4, ta, ma\n\t" \
        "1:\n\t" \
        "vle16.v v0, (%[p0])\n\t" \
        "vle16.v v4, (%[p1])\n\t" \
        "vle16.v v8, (%[py])\n\t" \
        "vsetvli t0, zero, e16, m2, ta, ma\n\t" \
        "vneg.v v12, v4\n\t" \
        "vneg.v v18, v2\n\t" \
        "mimo_eq2_vscmul v14, v0, v6\n\t" \
        "mimo_eq2_vscmacc v14, v12, v2\n\t" \
        "mimo_eq2_vscmul v16, v6, v8\n\t" \
        "mimo_eq2_vscmacc v16, v12, v10\n\t" \
        "mimo_eq2_vscmul v20, v0, v10\n\t" \
        "mimo_eq2_vscmacc v20, v18, v8\n\t" \
        "mimo_eq2_vscmulcj v22, v14, v14\n\t" \
        "vsll.vi v22, v22, 1\n\t" \
        "vluxei16.v v12, (%[table]), v22\n\t" \
        "mimo_eq2_vscmulcj v22, v14, v12\n\t" \
        "vsll.vi v22, v22, 2\n\t" \
        "mimo_eq2_vscmul v24, v16, v22\n\t" \
        "mimo_eq2_vscmul v26, v20, v22\n\t" \
        "vsetvli t0, zero, e16, m4, ta, ma\n\t" \
        MIMO_EQ2_EXT_DIRECT_OUTPUT_ASM \
        "vse16.v v24, (%[px])\n\t" \
        "addi %[p0], %[p0], 128\n\t" \
        "addi %[p1], %[p1], 128\n\t" \
        "addi %[py], %[py], 64\n\t" \
        "addi %[px], %[px], 64\n\t" \
        "addi %[blocks], %[blocks], -1\n\t" \
        "bnez %[blocks], 1b\n\t" \
        : [p0] "+r"(p0), [p1] "+r"(p1), [py] "+r"(py), \
          [px] "+r"(px), [blocks] "+r"(blocks) \
        : [table] "r"(mimo_eq2_ext_recip_q26_padded) \
        : "t0", "v0", "v1", "v2", "v3", "v4", "v5", \
          "v6", "v7", "v8", "v9", "v10", "v11", "v12", \
          "v13", "v14", "v15", "v16", "v17", "v18", "v19", \
          "v20", "v21", "v22", "v23", "v24", "v25", "v26", \
          "v27", "memory")

static MIMO_ALWAYS_INLINE void mimo_eq2_ext_direct_h_run(
    const mimo_c16_t *restrict h,
    const mimo_c16_t *restrict y,
    mimo_c16_t *restrict x)
{
    const size_t column_complex = MIMO_N2 * MIMO_EQ2_PACKED_LANES;
    const mimo_c16_t *p0 = h;
    const mimo_c16_t *p1 = h + column_complex;
    const mimo_c16_t *py = y;
    mimo_c16_t *px = x;
    size_t blocks = MIMO_EQ2_SITE_COUNT / MIMO_EQ2_PACKED_LANES;

    mimo_ext_set_vxrm_rnu_once();
    MIMO_EQ2_EXT_DIRECT_H_LOOP();
}
#endif

#if MIMO_USE_EQ2_MMSE_DIRECT_H
static MIMO_NOINLINE void mimo_eq2_ext_mmse_direct_fallback_block(
    const mimo_c16_t *restrict p0,
    const mimo_c16_t *restrict p1,
    const mimo_c16_t *restrict py,
    mimo_c16_t *restrict px);

#define MIMO_EQ2_EXT_MMSE_DIRECT_H_LOOP() \
    asm volatile( \
        "vsetvli t0, zero, e16, m4, ta, ma\n\t" \
        "vle16.v v0, (%[p0])\n\t" \
        "vle16.v v4, (%[p1])\n\t" \
        "vle16.v v8, (%[py])\n\t" \
        "vsetvli t0, zero, e16, m2, ta, ma\n\t" \
        "vneg.v v14, v4\n\t" \
        "vneg.v v26, v2\n\t" \
        "mimo_eq2_vscmul v12, v0, v6\n\t" \
        "mimo_eq2_vscmacc v12, v14, v2\n\t" \
        "mimo_eq2_vscmulcj v16, v0, v0\n\t" \
        "mimo_eq2_vscmacccj v16, v2, v2\n\t" \
        "mimo_eq2_vscmulcj v18, v4, v4\n\t" \
        "mimo_eq2_vscmacccj v18, v6, v6\n\t" \
        "vadd.vv v16, v16, v18\n\t" \
        "mimo_eq2_vscmulcj v18, v12, v6\n\t" \
        "mimo_eq2_vscmulcj v20, v12, v14\n\t" \
        "mimo_eq2_vscmulcj v22, v12, v26\n\t" \
        "mimo_eq2_vscmulcj v24, v12, v0\n\t" \
        "vssra.vi v0, v0, 6\n\t" \
        "vssra.vi v2, v2, 6\n\t" \
        "vssra.vi v4, v4, 6\n\t" \
        "vssra.vi v6, v6, 6\n\t" \
        "mimo_eq2_vscaddconj v18, v0, v18\n\t" \
        "mimo_eq2_vscaddconj v20, v2, v20\n\t" \
        "mimo_eq2_vscaddconj v22, v4, v22\n\t" \
        "mimo_eq2_vscaddconj v24, v6, v24\n\t" \
        "mimo_eq2_vscmul v0, v18, v8\n\t" \
        "mimo_eq2_vscmacc v0, v20, v10\n\t" \
        "mimo_eq2_vscmul v2, v22, v8\n\t" \
        "mimo_eq2_vscmacc v2, v24, v10\n\t" \
        "mimo_eq2_vscmulcj v4, v12, v12\n\t" \
        "mimo_eq2_vscmulcj v28, v16, v16\n\t" \
        "vsrl.vi v28, v28, 1\n\t" \
        "vsll.vi v30, v4, 1\n\t" \
        "vsrl.vi v24, v4, 2\n\t" \
        "vadd.vv v30, v30, v24\n\t" \
        "vmsgtu.vv v28, v28, v30\n\t" \
        "vcpop.m %[risk], v28\n\t" \
        "vssra.vi v16, v16, 6\n\t" \
        "vadd.vv v4, v4, v16\n\t" \
        "vadd.vi v4, v4, 8\n\t" \
        "vsll.vi v6, v4, 1\n\t" \
        "vluxei16.v v6, (%[table]), v6\n\t" \
        "vsll.vi v0, v0, 4\n\t" \
        "vsll.vi v2, v2, 4\n\t" \
        "mimo_eq2_vscmul v0, v0, v6\n\t" \
        "mimo_eq2_vscmul v2, v2, v6\n\t" \
        "vsetvli t0, zero, e16, m4, ta, ma\n\t" \
        "vse16.v v0, (%[px])\n\t" \
        : [risk] "=&r"(risk_lanes) \
        : [p0] "r"(p0), [p1] "r"(p1), [py] "r"(py), \
          [px] "r"(px), [table] "r"(mimo_eq2_ext_recip_q26_padded) \
        : "t0", "v0", "v1", "v2", "v3", "v4", "v5", \
          "v6", "v7", "v8", "v9", "v10", "v11", "v12", \
          "v13", "v14", "v15", "v16", "v17", "v18", "v19", \
          "v20", "v21", "v22", "v23", "v24", "v25", "v26", \
          "v27", "v28", "v29", "v30", "v31", "memory")

static MIMO_ALWAYS_INLINE void mimo_eq2_ext_mmse_direct_h_run(
    const mimo_c16_t *restrict h,
    const mimo_c16_t *restrict y,
    mimo_c16_t *restrict x)
{
    const size_t column_complex = MIMO_N2 * MIMO_EQ2_PACKED_LANES;
    const mimo_c16_t *p0 = h;
    const mimo_c16_t *p1 = h + column_complex;
    const mimo_c16_t *py = y;
    mimo_c16_t *px = x;
    const size_t h_block_complex = MIMO_N2 * column_complex;
    const size_t y_block_complex = MIMO_N2 * MIMO_EQ2_PACKED_LANES;
    const size_t blocks = MIMO_EQ2_SITE_COUNT / MIMO_EQ2_PACKED_LANES;

    mimo_ext_set_vxrm_rnu_once();
    for (size_t block = 0; block < blocks; ++block) {
        size_t risk_lanes;

        MIMO_EQ2_EXT_MMSE_DIRECT_H_LOOP();
        if (risk_lanes != 0) {
            mimo_eq2_ext_mmse_direct_fallback_block(p0, p1, py, px);
        }
        p0 += h_block_complex;
        p1 += h_block_complex;
        py += y_block_complex;
        px += y_block_complex;
    }
}
#endif

#if MIMO_USE_EXT_EQ2_M2_BATCH
#if MIMO_USE_EXT_EQ2_MMSE_ADAPTIVE_Q13
#define MIMO_EQ2_EXT_SOLVE_SETUP_ASM \
        "li t1, 8191\n\t" \
        "li t2, 0x00020002\n\t" \
        "li t3, 0x00010001\n\t"
#define MIMO_EQ2_EXT_OUTPUT_VSET_ASM ""
#define MIMO_EQ2_EXT_SOLVE_ASM \
        "mimo_eq2_vscmul v8, v16, v6\n\t" \
        "mimo_eq2_vscmul v10, v14, v6\n\t" \
        "mimo_eq2_vscmul v22, v12, v6\n\t" \
        "vsetivli zero, 8, e32, m2, ta, ma\n\t" \
        "vmsgtu.vx v0, v8, t1\n\t" \
        "vmsgtu.vx v1, v22, t1\n\t" \
        "vmor.mm v0, v0, v1\n\t" \
        "vcpop.m t4, v0\n\t" \
        "beqz t4, 2f\n\t" \
        "vmv.v.x v2, t2\n\t" \
        "vmerge.vxm v2, v2, t3, v0\n\t" \
        "vsetivli zero, 16, e16, m2, ta, mu\n\t" \
        "vsll.vv v8, v8, v2\n\t" \
        "vsll.vv v10, v10, v2\n\t" \
        "vsll.vv v22, v22, v2\n\t" \
        "vrsub.vx v10, v10, zero\n\t" \
        "mimo_eq2_vscmul v24, v8, v18\n\t" \
        "mimo_eq2_vscmacc v24, v10, v20\n\t" \
        "mimo_eq2_vscmulcj v26, v10, v18\n\t" \
        "mimo_eq2_vscmacc v26, v22, v20\n\t" \
        "vmseq.vi v0, v2, 1\n\t" \
        "vsadd.vv v24, v24, v24\n\t" \
        "vsadd.vv v26, v26, v26\n\t" \
        "vsadd.vv v24, v24, v24\n\t" \
        "vsadd.vv v26, v26, v26\n\t" \
        "vsadd.vv v24, v24, v24, v0.t\n\t" \
        "vsadd.vv v26, v26, v26, v0.t\n\t" \
        "j 3f\n\t" \
        "2:\n\t" \
        "vsetivli zero, 16, e16, m2, ta, ma\n\t" \
        "vsll.vi v8, v8, 2\n\t" \
        "vsll.vi v10, v10, 2\n\t" \
        "vsll.vi v22, v22, 2\n\t" \
        "vrsub.vx v10, v10, zero\n\t" \
        "mimo_eq2_vscmul v24, v8, v18\n\t" \
        "mimo_eq2_vscmacc v24, v10, v20\n\t" \
        "mimo_eq2_vscmulcj v26, v10, v18\n\t" \
        "mimo_eq2_vscmacc v26, v22, v20\n\t" \
        "vsadd.vv v24, v24, v24\n\t" \
        "vsadd.vv v26, v26, v26\n\t" \
        "vsadd.vv v24, v24, v24\n\t" \
        "vsadd.vv v26, v26, v26\n\t" \
        "3:\n\t"
#define MIMO_EQ2_EXT_OUTPUT_ASM \
        "vsetvli t0, zero, e16, m4, ta, ma\n\t"
#elif MIMO_USE_EXT_EQ2_INVCOEFF
#define MIMO_EQ2_EXT_SOLVE_SETUP_ASM ""
#define MIMO_EQ2_EXT_OUTPUT_VSET_ASM \
        "vsetvli t0, zero, e16, m4, ta, ma\n\t"
#if MIMO_USE_EXT_EQ2_RHS_INTERLEAVE
#define MIMO_EQ2_EXT_SOLVE_ASM \
        "mimo_eq2_vscmul v8, v16, v6\n\t" \
        "mimo_eq2_vscmul v10, v14, v6\n\t" \
        "mimo_eq2_vscmul v22, v12, v6\n\t" \
        "vsll.vi v8, v8, 1\n\t" \
        "vsll.vi v10, v10, 1\n\t" \
        "vsll.vi v22, v22, 1\n\t" \
        "vrsub.vx v10, v10, zero\n\t" \
        "mimo_eq2_vscmul v24, v8, v18\n\t" \
        "mimo_eq2_vscmulcj v26, v10, v18\n\t" \
        "mimo_eq2_vscmacc v24, v10, v20\n\t" \
        "mimo_eq2_vscmacc v26, v22, v20\n\t"
#else
#define MIMO_EQ2_EXT_SOLVE_SETUP_ASM ""
#define MIMO_EQ2_EXT_OUTPUT_VSET_ASM \
        "vsetvli t0, zero, e16, m4, ta, ma\n\t"
#define MIMO_EQ2_EXT_SOLVE_ASM \
        "mimo_eq2_vscmul v8, v16, v6\n\t" \
        "mimo_eq2_vscmul v10, v14, v6\n\t" \
        "mimo_eq2_vscmul v22, v12, v6\n\t" \
        "vsll.vi v8, v8, 1\n\t" \
        "vsll.vi v10, v10, 1\n\t" \
        "vsll.vi v22, v22, 1\n\t" \
        "vrsub.vx v10, v10, zero\n\t" \
        "mimo_eq2_vscmul v24, v8, v18\n\t" \
        "mimo_eq2_vscmacc v24, v10, v20\n\t" \
        "mimo_eq2_vscmulcj v26, v10, v18\n\t" \
        "mimo_eq2_vscmacc v26, v22, v20\n\t"
#endif
#if MIMO_USE_SOLVE_STORE_TRUST_I16
#define MIMO_EQ2_EXT_OUTPUT_ASM \
        "vsll.vi v24, v24, 3\n\t"
#else
#define MIMO_EQ2_EXT_OUTPUT_ASM \
        "vsadd.vv v24, v24, v24\n\t" \
        "vsadd.vv v24, v24, v24\n\t" \
        "vsadd.vv v24, v24, v24\n\t"
#endif
#else
#define MIMO_EQ2_EXT_SOLVE_SETUP_ASM ""
#define MIMO_EQ2_EXT_OUTPUT_VSET_ASM \
        "vsetvli t0, zero, e16, m4, ta, ma\n\t"
#define MIMO_EQ2_EXT_SOLVE_ASM \
        "vrsub.vx v8, v14, zero\n\t" \
        "mimo_eq2_vscmul v24, v16, v18\n\t" \
        "mimo_eq2_vscmacc v24, v8, v20\n\t" \
        "mimo_eq2_vscmul v26, v12, v20\n\t" \
        "mimo_eq2_vscmacccj v26, v8, v18\n\t" \
        "mimo_eq2_vscmul v24, v24, v6\n\t" \
        "mimo_eq2_vscmul v26, v26, v6\n\t"
#if MIMO_USE_SOLVE_STORE_TRUST_I16
#define MIMO_EQ2_EXT_OUTPUT_ASM \
        "vsll.vi v24, v24, 4\n\t"
#else
#define MIMO_EQ2_EXT_OUTPUT_ASM \
        "vsadd.vv v24, v24, v24\n\t" \
        "vsadd.vv v24, v24, v24\n\t" \
        "vsadd.vv v24, v24, v24\n\t" \
        "vsadd.vv v24, v24, v24\n\t"
#endif
#endif

#define MIMO_EQ2_EXT_PACKED_LOOP(lambda_setup_, lambda_add_) \
    asm volatile( \
        MIMO_EQ2_EXT_SOLVE_SETUP_ASM \
        lambda_setup_ \
        "vsetvli t0, zero, e16, m4, ta, ma\n\t" \
        "1:\n\t" \
        "vle16.v v0, (%[p0])\n\t" \
        "vle16.v v4, (%[p1])\n\t" \
        "vle16.v v8, (%[py])\n\t" \
        "vsetvli t0, zero, e16, m2, ta, ma\n\t" \
        "mimo_eq2_vscmulcj v12, v0, v0\n\t" \
        "mimo_eq2_vscmulcj v14, v0, v4\n\t" \
        "mimo_eq2_vscmulcj v16, v4, v4\n\t" \
        "mimo_eq2_vscmulcj v18, v0, v8\n\t" \
        "mimo_eq2_vscmulcj v20, v4, v8\n\t" \
        "mimo_eq2_vscmacccj v12, v2, v2\n\t" \
        "mimo_eq2_vscmacccj v14, v2, v6\n\t" \
        "mimo_eq2_vscmacccj v16, v6, v6\n\t" \
        "mimo_eq2_vscmacccj v18, v2, v10\n\t" \
        "mimo_eq2_vscmacccj v20, v6, v10\n\t" \
        lambda_add_ \
        "mimo_eq2_vscmul v0, v12, v16\n\t" \
        "mimo_eq2_vscmulcj v2, v14, v14\n\t" \
        "vsub.vv v0, v0, v2\n\t" \
        "vsll.vi v4, v0, 1\n\t" \
        "vluxei16.v v6, (%[table]), v4\n\t" \
        MIMO_EQ2_EXT_SOLVE_ASM \
        MIMO_EQ2_EXT_OUTPUT_VSET_ASM \
        MIMO_EQ2_EXT_OUTPUT_ASM \
        "vse16.v v24, (%[px])\n\t" \
        "addi %[p0], %[p0], 128\n\t" \
        "addi %[p1], %[p1], 128\n\t" \
        "addi %[py], %[py], 64\n\t" \
        "addi %[px], %[px], 64\n\t" \
        "addi %[blocks], %[blocks], -1\n\t" \
        "bnez %[blocks], 1b\n\t" \
        : [p0] "+r"(p0), [p1] "+r"(p1), [py] "+r"(py), \
          [px] "+r"(px), [blocks] "+r"(blocks) \
        : [table] "r"(mimo_eq2_ext_recip_q26_padded), \
          [lambda] "r"(MIMO_MMSE_LAMBDA_Q15) \
        : "t0", "t1", "t2", "t3", "t4", "v0", "v1", "v2", "v3", "v4", "v5", \
          "v6", "v7", "v8", "v9", "v10", "v11", "v12", \
          "v13", "v14", "v15", "v16", "v17", "v18", "v19", \
          "v20", "v21", "v22", "v23", "v24", "v25", "v26", \
          "v27", "v30", "v31", "memory")
#else
#if MIMO_USE_EXT_EQ2_MMSE_ADAPTIVE_Q13
#define MIMO_EQ2_EXT_SOLVE_SETUP_ASM \
        "li t1, 8191\n\t"
#define MIMO_EQ2_EXT_SOLVE_ASM \
        "mimo_eq2_vscmul v15, v8, v13\n\t" \
        "mimo_eq2_vscmul v16, v7, v13\n\t" \
        "mimo_eq2_vscmul v17, v6, v13\n\t" \
        "vsetivli zero, 4, e32, m1, ta, ma\n\t" \
        "vmsgtu.vx v0, v15, t1\n\t" \
        "vmsgtu.vx v1, v17, t1\n\t" \
        "vmor.mm v0, v0, v1\n\t" \
        "vsetivli zero, 8, e16, m1, ta, ma\n\t" \
        "vsll.vi v18, v15, 2\n\t" \
        "vsll.vi v19, v16, 2\n\t" \
        "vsll.vi v20, v17, 2\n\t" \
        "vsll.vi v15, v15, 1\n\t" \
        "vsll.vi v16, v16, 1\n\t" \
        "vsll.vi v17, v17, 1\n\t" \
        "vrsub.vx v16, v16, zero\n\t" \
        "vrsub.vx v19, v19, zero\n\t" \
        "vsetivli zero, 4, e32, m1, ta, ma\n\t" \
        "vmerge.vvm v15, v18, v15, v0\n\t" \
        "vmerge.vvm v16, v19, v16, v0\n\t" \
        "vmerge.vvm v17, v20, v17, v0\n\t" \
        "vsetivli zero, 8, e16, m1, ta, ma\n\t" \
        "mimo_eq2_vscmul v22, v15, v9\n\t" \
        "mimo_eq2_vscmacc v22, v16, v10\n\t" \
        "mimo_eq2_vscmulcj v23, v16, v9\n\t" \
        "mimo_eq2_vscmacc v23, v17, v10\n\t"
#define MIMO_EQ2_EXT_OUTPUT_ASM \
        "vsadd.vv v22, v22, v22\n\t" \
        "vsadd.vv v22, v22, v22\n\t" \
        "vsadd.vv v24, v22, v22\n\t" \
        "vsetivli zero, 4, e32, m1, ta, ma\n\t" \
        "vmerge.vvm v22, v22, v24, v0\n\t" \
        "vmerge.vvm v23, v23, v25, v0\n\t" \
        "vsetvli t0, zero, e16, m2, ta, ma\n\t"
#else
#define MIMO_EQ2_EXT_SOLVE_SETUP_ASM ""
#if MIMO_USE_EXT_EQ2_INVCOEFF
#if MIMO_USE_EXT_EQ2_RHS_INTERLEAVE
#define MIMO_EQ2_EXT_SOLVE_ASM \
        "mimo_eq2_vscmul v15, v8, v13\n\t" \
        "mimo_eq2_vscmul v16, v7, v13\n\t" \
        "mimo_eq2_vscmul v17, v6, v13\n\t" \
        "vsll.vi v15, v15, 1\n\t" \
        "vsll.vi v16, v16, 1\n\t" \
        "vsll.vi v17, v17, 1\n\t" \
        "vrsub.vx v16, v16, zero\n\t" \
        "mimo_eq2_vscmul v22, v15, v9\n\t" \
        "mimo_eq2_vscmulcj v23, v16, v9\n\t" \
        "mimo_eq2_vscmacc v22, v16, v10\n\t" \
        "mimo_eq2_vscmacc v23, v17, v10\n\t"
#else
#define MIMO_EQ2_EXT_SOLVE_ASM \
        "mimo_eq2_vscmul v15, v8, v13\n\t" \
        "mimo_eq2_vscmul v16, v7, v13\n\t" \
        "mimo_eq2_vscmul v17, v6, v13\n\t" \
        "vsll.vi v15, v15, 1\n\t" \
        "vsll.vi v16, v16, 1\n\t" \
        "vsll.vi v17, v17, 1\n\t" \
        "vrsub.vx v16, v16, zero\n\t" \
        "mimo_eq2_vscmul v22, v15, v9\n\t" \
        "mimo_eq2_vscmacc v22, v16, v10\n\t" \
        "mimo_eq2_vscmulcj v23, v16, v9\n\t" \
        "mimo_eq2_vscmacc v23, v17, v10\n\t"
#endif
#if MIMO_USE_SOLVE_STORE_TRUST_I16
#define MIMO_EQ2_EXT_OUTPUT_ASM \
        "vsll.vi v22, v22, 3\n\t"
#else
#define MIMO_EQ2_EXT_OUTPUT_ASM \
        "vsadd.vv v22, v22, v22\n\t" \
        "vsadd.vv v22, v22, v22\n\t" \
        "vsadd.vv v22, v22, v22\n\t"
#endif
#else
#define MIMO_EQ2_EXT_SOLVE_ASM \
        "vrsub.vx v14, v7, zero\n\t" \
        "mimo_eq2_vscmul v22, v8, v9\n\t" \
        "mimo_eq2_vscmacc v22, v14, v10\n\t" \
        "mimo_eq2_vscmul v23, v6, v10\n\t" \
        "mimo_eq2_vscmacccj v23, v14, v9\n\t" \
        "mimo_eq2_vscmul v22, v22, v13\n\t" \
        "mimo_eq2_vscmul v23, v23, v13\n\t"
#if MIMO_USE_SOLVE_STORE_TRUST_I16
#define MIMO_EQ2_EXT_OUTPUT_ASM \
        "vsll.vi v22, v22, 4\n\t"
#else
#define MIMO_EQ2_EXT_OUTPUT_ASM \
        "vsadd.vv v22, v22, v22\n\t" \
        "vsadd.vv v22, v22, v22\n\t" \
        "vsadd.vv v22, v22, v22\n\t" \
        "vsadd.vv v22, v22, v22\n\t"
#endif
#endif
#endif

#define MIMO_EQ2_EXT_PACKED_LOOP(lambda_setup_, lambda_add_) \
    asm volatile( \
        MIMO_EQ2_EXT_SOLVE_SETUP_ASM \
        lambda_setup_ \
        "vsetvli t0, zero, e16, m2, ta, ma\n\t" \
        "1:\n\t" \
        "vle16.v v0, (%[p0])\n\t" \
        "vle16.v v2, (%[p1])\n\t" \
        "vle16.v v4, (%[py])\n\t" \
        "vsetivli zero, 8, e16, m1, ta, ma\n\t" \
        "mimo_eq2_vscmulcj v6, v0, v0\n\t" \
        "mimo_eq2_vscmulcj v7, v0, v2\n\t" \
        "mimo_eq2_vscmulcj v8, v2, v2\n\t" \
        "mimo_eq2_vscmulcj v9, v0, v4\n\t" \
        "mimo_eq2_vscmulcj v10, v2, v4\n\t" \
        "mimo_eq2_vscmacccj v6, v1, v1\n\t" \
        "mimo_eq2_vscmacccj v7, v1, v3\n\t" \
        "mimo_eq2_vscmacccj v8, v3, v3\n\t" \
        "mimo_eq2_vscmacccj v9, v1, v5\n\t" \
        "mimo_eq2_vscmacccj v10, v3, v5\n\t" \
        lambda_add_ \
        "mimo_eq2_vscmul v11, v6, v8\n\t" \
        "mimo_eq2_vscmulcj v12, v7, v7\n\t" \
        "vsub.vv v11, v11, v12\n\t" \
        "vsll.vi v12, v11, 1\n\t" \
        "vluxei16.v v13, (%[table]), v12\n\t" \
        MIMO_EQ2_EXT_SOLVE_ASM \
        "vsetvli t0, zero, e16, m2, ta, ma\n\t" \
        MIMO_EQ2_EXT_OUTPUT_ASM \
        "vse16.v v22, (%[px])\n\t" \
        "addi %[p0], %[p0], 64\n\t" \
        "addi %[p1], %[p1], 64\n\t" \
        "addi %[py], %[py], 32\n\t" \
        "addi %[px], %[px], 32\n\t" \
        "addi %[blocks], %[blocks], -1\n\t" \
        "bnez %[blocks], 1b\n\t" \
        : [p0] "+r"(p0), [p1] "+r"(p1), [py] "+r"(py), \
          [px] "+r"(px), [blocks] "+r"(blocks) \
        : [table] "r"(mimo_eq2_ext_recip_q26_padded), \
          [lambda] "r"(MIMO_MMSE_LAMBDA_Q15) \
        : "t0", "t1", "v0", "v1", "v2", "v3", "v4", "v5", \
          "v6", "v7", "v8", "v9", "v10", "v11", "v12", \
          "v13", "v14", "v15", "v16", "v17", "v18", "v19", \
          "v20", "v22", "v23", "v24", "v25", "v30", "memory")
#endif

static MIMO_ALWAYS_INLINE void mimo_eq2_ext_packed_run(
    const mimo_c16_t *restrict h,
    const mimo_c16_t *restrict y,
    int mmse,
    mimo_c16_t *restrict x)
{
    const size_t column_complex = MIMO_N2 * MIMO_EQ2_PACKED_LANES;
    const mimo_c16_t *p0 = h;
    const mimo_c16_t *p1 = h + column_complex;
    const mimo_c16_t *py = y;
    mimo_c16_t *px = x;
    size_t blocks = MIMO_EQ2_SITE_COUNT / MIMO_EQ2_PACKED_LANES;

    mimo_ext_set_vxrm_rnu_once();
    if (mmse) {
#if MIMO_USE_EXT_EQ2_M2_BATCH
        MIMO_EQ2_EXT_PACKED_LOOP(
            "vsetivli zero, 8, e32, m2, ta, ma\n\t"
            "vmv.v.x v30, %[lambda]\n\t",
            "vadd.vv v12, v12, v30\n\t"
            "vadd.vv v16, v16, v30\n\t");
#else
        MIMO_EQ2_EXT_PACKED_LOOP(
            "vsetivli zero, 4, e32, m1, ta, ma\n\t"
            "vmv.v.x v30, %[lambda]\n\t",
            "vadd.vv v6, v6, v30\n\t"
            "vadd.vv v8, v8, v30\n\t");
#endif
    } else {
        MIMO_EQ2_EXT_PACKED_LOOP("", "");
    }
}

#if MIMO_USE_EQ2_MMSE_DIRECT_H
static MIMO_NOINLINE void mimo_eq2_ext_mmse_direct_fallback_block(
    const mimo_c16_t *restrict p0,
    const mimo_c16_t *restrict p1,
    const mimo_c16_t *restrict py,
    mimo_c16_t *restrict px)
{
    size_t blocks = 1;

    mimo_ext_set_vxrm_rnu_once();
    MIMO_EQ2_EXT_PACKED_LOOP(
        "vsetivli zero, 8, e32, m2, ta, ma\n\t"
        "vmv.v.x v30, %[lambda]\n\t",
        "vadd.vv v12, v12, v30\n\t"
        "vadd.vv v16, v16, v30\n\t");
}
#endif

MIMO_NOINLINE
void mimo_eq_zf_2x2_ext(
    const mimo_c16_t h[MIMO_EQ2_SITE_COUNT][MIMO_N2][MIMO_N2],
    const mimo_c16_t y[MIMO_EQ2_SITE_COUNT][MIMO_N2],
    mimo_c16_t x[MIMO_EQ2_SITE_COUNT][MIMO_N2])
{
#if MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID
    mimo_eq2_ext_direct_h_adaptive_run(
        &h[0][0][0], &y[0][0], &x[0][0]);
#elif MIMO_USE_EQ2_ZF_DIRECT_H
    mimo_eq2_ext_direct_h_run(&h[0][0][0], &y[0][0], &x[0][0]);
#else
    mimo_eq2_ext_packed_run(&h[0][0][0], &y[0][0], 0, &x[0][0]);
#endif
}

MIMO_NOINLINE
void mimo_eq_mmse_2x2_ext(
    const mimo_c16_t h[MIMO_EQ2_SITE_COUNT][MIMO_N2][MIMO_N2],
    const mimo_c16_t y[MIMO_EQ2_SITE_COUNT][MIMO_N2],
    mimo_c16_t x[MIMO_EQ2_SITE_COUNT][MIMO_N2])
{
#if MIMO_USE_EQ2_MMSE_DIRECT_H
    mimo_eq2_ext_mmse_direct_h_run(&h[0][0][0], &y[0][0], &x[0][0]);
#else
    mimo_eq2_ext_packed_run(&h[0][0][0], &y[0][0], 1, &x[0][0]);
#endif
}

#undef MIMO_EQ2_EXT_PACKED_LOOP
#undef MIMO_EQ2_EXT_OUTPUT_ASM
#undef MIMO_EQ2_EXT_SOLVE_ASM
#undef MIMO_EQ2_EXT_SOLVE_SETUP_ASM
#if MIMO_USE_EXT_EQ2_M2_BATCH
#undef MIMO_EQ2_EXT_OUTPUT_VSET_ASM
#endif
#if MIMO_USE_EQ2_ZF_DIRECT_H
#undef MIMO_EQ2_EXT_DIRECT_H_LOOP
#undef MIMO_EQ2_EXT_DIRECT_OUTPUT_ASM
#endif
#if MIMO_USE_EQ2_MMSE_DIRECT_H
#undef MIMO_EQ2_EXT_MMSE_DIRECT_H_LOOP
#endif
#undef MIMO_EQ2_EXT_DET_Q15_MIN
#endif
