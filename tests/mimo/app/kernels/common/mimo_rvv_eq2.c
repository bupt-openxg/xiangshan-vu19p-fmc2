#include <stddef.h>
#include <stdint.h>

#include <riscv_vector.h>

#include "mimo_kernels.h"

#if MIMO_ENABLE_EQ2
#if MIMO_USE_RVV_EQ2_M1
#define vint16mf2_t vint16m1_t
#define vuint16mf2_t vuint16m1_t
#define vint32m1_t vint32m2_t
#define vuint32m1_t vuint32m2_t
#define __riscv_vsetvl_e16mf2 __riscv_vsetvl_e16m1
#define __riscv_vle16_v_i16mf2 __riscv_vle16_v_i16m1
#define __riscv_vadd_vv_i16mf2 __riscv_vadd_vv_i16m1
#define __riscv_vsub_vv_i16mf2 __riscv_vsub_vv_i16m1
#define __riscv_vwmul_vv_i32m1 __riscv_vwmul_vv_i32m2
#define __riscv_vwmulsu_vv_i32m1 __riscv_vwmulsu_vv_i32m2
#define __riscv_vwmacc_vv_i32m1 __riscv_vwmacc_vv_i32m2
#define __riscv_vneg_v_i16mf2 __riscv_vneg_v_i16m1
#define __riscv_vsadd_vx_i16mf2 __riscv_vsadd_vx_i16m1
#define __riscv_vssra_vx_i32m1 __riscv_vssra_vx_i32m2
#define __riscv_vadd_vv_i32m1 __riscv_vadd_vv_i32m2
#define __riscv_vsub_vv_i32m1 __riscv_vsub_vv_i32m2
#define __riscv_vneg_v_i32m1 __riscv_vneg_v_i32m2
#define __riscv_vadd_vx_i32m1 __riscv_vadd_vx_i32m2
#define __riscv_vnclip_wx_i16mf2 __riscv_vnclip_wx_i16m1
#define __riscv_vsra_vx_i32m1 __riscv_vsra_vx_i32m2
#define __riscv_vmul_vv_i32m1 __riscv_vmul_vv_i32m2
#define __riscv_vmacc_vv_i32m1 __riscv_vmacc_vv_i32m2
#define __riscv_vsub_vx_i32m1 __riscv_vsub_vx_i32m2
#define __riscv_vreinterpret_v_i32m1_u32m1 \
    __riscv_vreinterpret_v_i32m2_u32m2
#define __riscv_vsll_vx_u32m1 __riscv_vsll_vx_u32m2
#define __riscv_vluxei32_v_i32m1 __riscv_vluxei32_v_i32m2
#define __riscv_vluxei32_v_u16mf2 __riscv_vluxei32_v_u16m1
#define __riscv_vreinterpret_v_u16mf2_i16mf2 \
    __riscv_vreinterpret_v_u16m1_i16m1
#define __riscv_vsll_vx_i16mf2 __riscv_vsll_vx_i16m1
#define __riscv_vwadd_vx_i32m1 __riscv_vwadd_vx_i32m2
#define __riscv_vreinterpret_v_i16mf2_u16mf2 \
    __riscv_vreinterpret_v_i16m1_u16m1
#define __riscv_vzext_vf2_u32m1 __riscv_vzext_vf2_u32m2
#define __riscv_vreinterpret_v_u32m1_i32m1 \
    __riscv_vreinterpret_v_u32m2_i32m2
#define __riscv_vand_vx_i32m1 __riscv_vand_vx_i32m2
#define __riscv_vsll_vx_i32m1 __riscv_vsll_vx_i32m2
#define __riscv_vor_vv_i32m1 __riscv_vor_vv_i32m2
#define __riscv_vse32_v_i32m1 __riscv_vse32_v_i32m2
#define __riscv_vsse32_v_i32m1 __riscv_vsse32_v_i32m2
#define __riscv_vse16_v_i16mf2 __riscv_vse16_v_i16m1
#endif

#ifndef MIMO_USE_RVV_EQ2_RI_SOA_OUTPUT
#define MIMO_USE_RVV_EQ2_RI_SOA_OUTPUT 0
#endif

#ifndef MIMO_USE_RVV_EQ2_STAGED_NARROW_BUILD
#define MIMO_USE_RVV_EQ2_STAGED_NARROW_BUILD 0
#endif

#ifndef MIMO_USE_RVV_EQ2_IMPLICIT_VXRM
#define MIMO_USE_RVV_EQ2_IMPLICIT_VXRM 0
#endif

#ifndef MIMO_USE_RVV_EQ2_UQ12_SCALE
#define MIMO_USE_RVV_EQ2_UQ12_SCALE 0
#endif

#ifndef MIMO_USE_RVV_EQ2_EARLY_RECIP_OVERLAP
#define MIMO_USE_RVV_EQ2_EARLY_RECIP_OVERLAP 0
#endif

#ifndef MIMO_USE_RVV_EQ2_MMSE_FULL_TRUNC_SCALE
#define MIMO_USE_RVV_EQ2_MMSE_FULL_TRUNC_SCALE 0
#endif

#if MIMO_USE_RVV_EQ2_MMSE_FULL_TRUNC_SCALE && \
    (!MIMO_USE_RVV_EQ2_M1 || MIMO_USE_RVV_EQ2_UQ12_SCALE || \
     MIMO_USE_EQ2_ZF_DIRECT_H)
#error "EQ2 MMSE full-trunc scale requires wide-m1 Gramian and the Q30 scale path"
#endif

#if MIMO_USE_RVV_EQ2_EARLY_RECIP_OVERLAP && \
    (!MIMO_USE_RVV_EQ2_M1 || MIMO_USE_RVV_EQ2_UQ12_SCALE)
#error "EQ2 early reciprocal overlap requires wide m1 and the Q30 scale path"
#endif

#ifndef MIMO_USE_RVV_EQ2_BLOCK_SOA
#define MIMO_USE_RVV_EQ2_BLOCK_SOA 0
#endif

#ifndef MIMO_USE_RVV_EQ2_GROUP_LOAD
#define MIMO_USE_RVV_EQ2_GROUP_LOAD 0
#endif

#ifndef MIMO_USE_RVV_EQ2_GROUP_LOAD_M4
#define MIMO_USE_RVV_EQ2_GROUP_LOAD_M4 0
#endif

#if MIMO_USE_RVV_EQ2_GROUP_LOAD && !MIMO_USE_RVV_EQ2_BLOCK_SOA
#error "EQ2 group load requires the block-SoA layout"
#endif

#if MIMO_USE_RVV_EQ2_GROUP_LOAD_M4 && !MIMO_USE_RVV_EQ2_GROUP_LOAD
#error "EQ2 m4 group load requires group load"
#endif

#if MIMO_USE_RVV_EQ2_BLOCK_SOA && \
    (!MIMO_USE_RVV_EQ2_M1 || MIMO_USE_RVV_EQ2_SPLIT_BUILD)
#error "EQ2 block-SoA requires wide m1 and a non-split build"
#endif

#if MIMO_USE_RVV_EQ2_BLOCK_SOA && \
    (MIMO_EQ2_SITE_COUNT % MIMO_EQ2_BLOCK_SOA_LANES != 0)
#error "EQ2 block-SoA requires a whole number of 8-site blocks"
#endif

#ifndef MIMO_USE_EQ2_DIRECT_Q13_INV
#define MIMO_USE_EQ2_DIRECT_Q13_INV 0
#endif

#ifndef MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP
#define MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP 0
#endif

#if MIMO_USE_EQ2_DIRECT_Q13_INV && !MIMO_USE_EQ2_ZF_DIRECT_H
#error "EQ2 Q13 inverse coefficient is valid only for the ZF direct-H path"
#endif

#if MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP && \
    (!MIMO_USE_EQ2_ZF_DIRECT_H || MIMO_USE_EQ2_DIRECT_Q13_INV || \
     MIMO_USE_RVV_EQ2_UQ12_SCALE || !MIMO_USE_RVV_EQ2_IMPLICIT_VXRM || \
     !MIMO_USE_RVV_EQ2_M1)
#error "RVV EQ2 guard1 requires wide-m1 direct-H, implicit RNU, Q30 table path, and no Q13 inverse"
#endif

#if MIMO_USE_RVV_EQ2_IMPLICIT_VXRM && \
    !MIMO_USE_EQ2_ZF_DIRECT_H && !MIMO_USE_RVV_EQ2_STAGED_NARROW_BUILD
#error "EQ2 implicit-vxrm narrowing requires ZF direct-H or staged build"
#endif

#if MIMO_USE_RVV_EQ2_RI_SOA_OUTPUT && \
    (MIMO_USE_RVV_EQ2_TX_SOA_STORE || MIMO_USE_RVV_EQ2_ZEXT_PACK)
#error "EQ2 RI-SoA output replaces TX-SoA packed output and zext packing"
#endif

#if MIMO_EQ2_RECIP_MODE != 5
#error "The initial RVV EQ2 path requires direct1 reciprocal mode5"
#endif

#if MIMO_EQ2_RECIP_NUM_SHIFT != 13
#error "The initial RVV EQ2 path requires reciprocal numerator shift13"
#endif

#if MIMO_USE_RVV_EQ2_FUSED_NCLIP_BUILD && \
    MIMO_USE_RVV_EQ2_ALWAYS_ADD_LAMBDA
#error "Fused EQ2 build narrowing and the rejected always-add candidate are mutually exclusive"
#endif

#if MIMO_USE_RVV_EQ2_STAGED_NARROW_BUILD && \
    (!MIMO_USE_RVV_EQ2_FUSED_NCLIP_BUILD || \
     MIMO_USE_RVV_EQ2_SPLIT_BUILD || MIMO_USE_RVV_EQ2_3M_BUILD)
#error "EQ2 staged narrowing requires fused-nclip, non-split, 4M build"
#endif

#if MIMO_USE_EQ2_ZF_DIRECT_H && \
    (!MIMO_USE_RVV_EQ2_RI_SOA_OUTPUT || \
     MIMO_USE_RVV_EQ2_ALWAYS_ADD_LAMBDA || \
     MIMO_USE_RVV_EQ2_SPLIT_BUILD || MIMO_USE_RVV_EQ2_FUSED_NCLIP_BUILD || \
     MIMO_USE_RVV_EQ2_TX_SOA_STORE || MIMO_USE_RVV_EQ2_3M_BUILD || \
     MIMO_USE_RVV_EQ2_ZEXT_PACK || MIMO_USE_RVV_EQ2_STAGED_NARROW_BUILD)
#error "RVV EQ2 direct-H requires RI-SoA output and no Gramian-only build/store knobs"
#endif

#define MIMO_EQ2_DET_Q15_RECIP_MIN 3400
#define MIMO_EQ2_DET_Q15_RECIP_ENTRY(rel_) \
    ((int32_t)(1073741824ull / \
               (3400u + (uint32_t)(rel_))) - \
     ((uint32_t)(rel_) == 837u ? 10 : 0))

static const int32_t mimo_eq2_rvv_det_recip_direct1_q30[] MIMO_ALIGN64 = {
#define MIMO_SCHUR4_DET_Q15_RECIP_DIRECT1_ENTRY(rel_) \
    MIMO_EQ2_DET_Q15_RECIP_ENTRY(rel_)
#include "mimo_schur_recip_direct1.inc"
};

#undef MIMO_SCHUR4_DET_Q15_RECIP_DIRECT1_ENTRY

#if MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP
#define MIMO_EQ2_DET_Q16_RECIP_MIN 6800
static const int32_t mimo_eq2_rvv_det_recip_direct_half_q15[] MIMO_ALIGN64 = {
#define MIMO_SCHUR4_DET_Q15_RECIP_DIRECT1_ENTRY(rel_) \
    ((int32_t)(2147483648ull / \
               (6800u + 2u * (uint32_t)(rel_)))), \
    ((int32_t)(2147483648ull / \
               (6800u + 2u * (uint32_t)(rel_) + 1u)))
#include "mimo_schur_recip_direct1.inc"
};

#undef MIMO_SCHUR4_DET_Q15_RECIP_DIRECT1_ENTRY
#endif

#if MIMO_USE_EQ2_DIRECT_Q13_INV
static const uint16_t mimo_eq2_rvv_det_recip_direct1_q26[] MIMO_ALIGN64 = {
#define MIMO_SCHUR4_DET_Q15_RECIP_DIRECT1_ENTRY(rel_) \
    ((uint16_t)((1073741824ull / (3400u + (uint32_t)(rel_))) >> 4))
#include "mimo_schur_recip_direct1.inc"
};

#undef MIMO_SCHUR4_DET_Q15_RECIP_DIRECT1_ENTRY
#endif

#if MIMO_USE_RVV_EQ2_UQ12_SCALE
static const uint16_t mimo_eq2_rvv_det_recip_direct1_uq12[] MIMO_ALIGN64 = {
#define MIMO_SCHUR4_DET_Q15_RECIP_DIRECT1_ENTRY(rel_) \
    ((uint16_t)(134217728ull / (3400u + (uint32_t)(rel_))))
#include "mimo_schur_recip_direct1.inc"
};

#undef MIMO_SCHUR4_DET_Q15_RECIP_DIRECT1_ENTRY
#endif
#undef MIMO_EQ2_DET_Q15_RECIP_ENTRY

static MIMO_ALWAYS_INLINE vint32m1_t mimo_eq2_conj_dot2_re(
    vint16mf2_t ar0,
    vint16mf2_t ai0,
    vint16mf2_t br0,
    vint16mf2_t bi0,
    vint16mf2_t ar1,
    vint16mf2_t ai1,
    vint16mf2_t br1,
    vint16mf2_t bi1,
    size_t vl)
{
    vint32m1_t acc = __riscv_vwmul_vv_i32m1(ar0, br0, vl);

    acc = __riscv_vwmacc_vv_i32m1(acc, ai0, bi0, vl);
    acc = __riscv_vwmacc_vv_i32m1(acc, ar1, br1, vl);
    return __riscv_vwmacc_vv_i32m1(acc, ai1, bi1, vl);
}

static MIMO_ALWAYS_INLINE vint32m1_t mimo_eq2_conj_dot2_im(
    vint16mf2_t ar0,
    vint16mf2_t ai0,
    vint16mf2_t br0,
    vint16mf2_t bi0,
    vint16mf2_t ar1,
    vint16mf2_t ai1,
    vint16mf2_t br1,
    vint16mf2_t bi1,
    size_t vl)
{
    vint32m1_t acc = __riscv_vwmul_vv_i32m1(ar0, bi0, vl);
    const vint16mf2_t neg_ai0 = __riscv_vneg_v_i16mf2(ai0, vl);
    const vint16mf2_t neg_ai1 = __riscv_vneg_v_i16mf2(ai1, vl);

    acc = __riscv_vwmacc_vv_i32m1(acc, neg_ai0, br0, vl);
    acc = __riscv_vwmacc_vv_i32m1(acc, ar1, bi1, vl);
    return __riscv_vwmacc_vv_i32m1(acc, neg_ai1, br1, vl);
}

static MIMO_ALWAYS_INLINE vint32m1_t mimo_eq2_abs2_dot2(
    vint16mf2_t ar0,
    vint16mf2_t ai0,
    vint16mf2_t ar1,
    vint16mf2_t ai1,
    size_t vl)
{
    vint32m1_t acc = __riscv_vwmul_vv_i32m1(ar0, ar0, vl);

    acc = __riscv_vwmacc_vv_i32m1(acc, ai0, ai0, vl);
    acc = __riscv_vwmacc_vv_i32m1(acc, ar1, ar1, vl);
    return __riscv_vwmacc_vv_i32m1(acc, ai1, ai1, vl);
}

#if MIMO_USE_RVV_EQ2_3M_BUILD
#define MIMO_EQ2_CONJ_DOT2_3M(out_re_, out_im_, \
                              ar0_, ai0_, br0_, bi0_, \
                              ar1_, ai1_, br1_, bi1_, vl_) \
    do { \
        const vint16mf2_t lhs0_ = \
            __riscv_vsub_vv_i16mf2((ar0_), (ai0_), (vl_)); \
        const vint16mf2_t rhs0_ = \
            __riscv_vadd_vv_i16mf2((br0_), (bi0_), (vl_)); \
        const vint16mf2_t lhs1_ = \
            __riscv_vsub_vv_i16mf2((ar1_), (ai1_), (vl_)); \
        const vint16mf2_t rhs1_ = \
            __riscv_vadd_vv_i16mf2((br1_), (bi1_), (vl_)); \
        vint32m1_t p1_ = \
            __riscv_vwmul_vv_i32m1((ar0_), (br0_), (vl_)); \
        vint32m1_t p2_ = \
            __riscv_vwmul_vv_i32m1((ai0_), (bi0_), (vl_)); \
        vint32m1_t p3_ = \
            __riscv_vwmul_vv_i32m1(lhs0_, rhs0_, (vl_)); \
        p1_ = __riscv_vwmacc_vv_i32m1( \
            p1_, (ar1_), (br1_), (vl_)); \
        p2_ = __riscv_vwmacc_vv_i32m1( \
            p2_, (ai1_), (bi1_), (vl_)); \
        p3_ = __riscv_vwmacc_vv_i32m1( \
            p3_, lhs1_, rhs1_, (vl_)); \
        (out_re_) = __riscv_vadd_vv_i32m1(p1_, p2_, (vl_)); \
        (out_im_) = __riscv_vadd_vv_i32m1( \
            __riscv_vsub_vv_i32m1(p3_, p1_, (vl_)), p2_, (vl_)); \
    } while (0)
#endif

static MIMO_ALWAYS_INLINE void mimo_eq2_scale4_q30_to_q15(
    vint32m1_t *n0,
    vint32m1_t *n1,
    vint32m1_t *n2,
    vint32m1_t *n3,
    vint32m1_t scale_q30)
{
    vint32m1_t x0 = *n0;
    vint32m1_t x1 = *n1;
    vint32m1_t x2 = *n2;
    vint32m1_t x3 = *n3;

    asm volatile(
        "vssra.vi %[x0], %[x0], 15\n\t"
        "vssra.vi %[x1], %[x1], 15\n\t"
        "vssra.vi %[x2], %[x2], 15\n\t"
        "vssra.vi %[x3], %[x3], 15\n\t"
        "vmul.vv %[x0], %[x0], %[scale]\n\t"
        "vmul.vv %[x1], %[x1], %[scale]\n\t"
        "vmul.vv %[x2], %[x2], %[scale]\n\t"
        "vmul.vv %[x3], %[x3], %[scale]\n\t"
        "vssra.vi %[x0], %[x0], 15\n\t"
        "vssra.vi %[x1], %[x1], 15\n\t"
        "vssra.vi %[x2], %[x2], 15\n\t"
        "vssra.vi %[x3], %[x3], 15\n\t"
        : [x0] "+&vr"(x0), [x1] "+&vr"(x1),
          [x2] "+&vr"(x2), [x3] "+&vr"(x3)
        : [scale] "vr"(scale_q30));

    *n0 = x0;
    *n1 = x1;
    *n2 = x2;
    *n3 = x3;
}

#if MIMO_USE_RVV_EQ2_MMSE_FULL_TRUNC_SCALE
static MIMO_ALWAYS_INLINE vint32m1_t mimo_eq2_scale_q30_rnu(
    vint32m1_t numerator_q30,
    vint32m1_t scale_q30)
{
    vint32m1_t out = numerator_q30;

    asm volatile(
        "vwmul.vv v28, %[out], %[scale]\n\t"
        "vnclip.wi %[out], v28, 30"
        : [out] "+&vr"(out)
        : [scale] "vr"(scale_q30)
        : "v28", "v29", "v30", "v31");

    return out;
}
#endif

#if MIMO_USE_RVV_EQ2_UQ12_SCALE
#if MIMO_USE_RVV_EQ2_M1
#define MIMO_EQ2_UQ12_VSET "vsetvli zero, %[vl], e16, m1, ta, ma\n\t"
#else
#define MIMO_EQ2_UQ12_VSET "vsetvli zero, %[vl], e16, mf2, ta, ma\n\t"
#endif

static MIMO_ALWAYS_INLINE void mimo_eq2_scale4_q30_with_uq12(
    vint16mf2_t *x0,
    vint16mf2_t *x1,
    vint16mf2_t *x2,
    vint16mf2_t *x3,
    vint32m1_t n0,
    vint32m1_t n1,
    vint32m1_t n2,
    vint32m1_t n3,
    vuint32m1_t byte_index,
    size_t vl)
{
    vuint16mf2_t scale_uq12;
    vint16mf2_t out0;
    vint16mf2_t out1;
    vint16mf2_t out2;
    vint16mf2_t out3;

    asm volatile(
        MIMO_EQ2_UQ12_VSET
        "vluxei32.v %[scale], (%[table]), %[index]\n\t"
        "vnclip.wi %[out0], %[n0], 15\n\t"
        "vnclip.wi %[out1], %[n1], 15\n\t"
        "vnclip.wi %[out2], %[n2], 15\n\t"
        "vnclip.wi %[out3], %[n3], 15\n\t"
        "vwmulsu.vv %[n0], %[out0], %[scale]\n\t"
        "vwmulsu.vv %[n1], %[out1], %[scale]\n\t"
        "vwmulsu.vv %[n2], %[out2], %[scale]\n\t"
        "vwmulsu.vv %[n3], %[out3], %[scale]\n\t"
        "vnclip.wi %[out0], %[n0], 12\n\t"
        "vnclip.wi %[out1], %[n1], 12\n\t"
        "vnclip.wi %[out2], %[n2], 12\n\t"
        "vnclip.wi %[out3], %[n3], 12"
        : [n0] "+&vr"(n0), [n1] "+&vr"(n1),
          [n2] "+&vr"(n2), [n3] "+&vr"(n3),
          [scale] "=&vr"(scale_uq12),
          [out0] "=&vr"(out0), [out1] "=&vr"(out1),
          [out2] "=&vr"(out2), [out3] "=&vr"(out3)
        : [table] "r"(mimo_eq2_rvv_det_recip_direct1_uq12),
          [index] "vr"(byte_index), [vl] "r"(vl)
        : "memory");

    *x0 = out0;
    *x1 = out1;
    *x2 = out2;
    *x3 = out3;
}
#undef MIMO_EQ2_UQ12_VSET
#endif

#if MIMO_USE_RVV_EQ2_IMPLICIT_VXRM
static MIMO_ALWAYS_INLINE vint16mf2_t mimo_eq2_vnclip15_implicit(
    vint32m1_t src)
{
#if MIMO_USE_RVV_EQ2_M1 && MIMO_USE_RVV_EQ2_STAGED_NARROW_BUILD
    asm volatile("vnclip.wi %0, %0, 15" : "+vr"(src));
    return __riscv_vget_v_i16m2_i16m1(
        __riscv_vreinterpret_v_i32m2_i16m2(src), 0);
#else
    vint16mf2_t out;

    asm volatile("vnclip.wi %0, %1, 15" : "=vr"(out) : "vr"(src));
    return out;
#endif
}
#define MIMO_EQ2_RVV_NARROW_Q15(src_, vl_) \
    mimo_eq2_vnclip15_implicit((src_))
#else
#define MIMO_EQ2_RVV_NARROW_Q15(src_, vl_) \
    __riscv_vnclip_wx_i16mf2((src_), MIMO_Q15_SHIFT, 0, (vl_))
#endif

#if MIMO_USE_EQ2_ZF_DIRECT_H
#if MIMO_USE_RVV_EQ2_IMPLICIT_VXRM
static MIMO_ALWAYS_INLINE vint16mf2_t mimo_eq2_direct_vnclip15_implicit(
    vint32m1_t src)
{
    vint16mf2_t out;

#if MIMO_USE_RVV_EQ2_GROUP_LOAD_M4
    asm volatile("vnclip.wi %0, %1, 15" : "=&vr"(out) : "vr"(src));
#else
    asm volatile("vnclip.wi %0, %1, 15" : "=vr"(out) : "vr"(src));
#endif
    return out;
}
static MIMO_ALWAYS_INLINE vint16mf2_t mimo_eq2_direct_vnclip0(
    vint32m1_t src)
{
    vint16mf2_t out;

#if MIMO_USE_RVV_EQ2_GROUP_LOAD_M4
    asm volatile("vnclip.wi %0, %1, 0" : "=&vr"(out) : "vr"(src));
#else
    asm volatile("vnclip.wi %0, %1, 0" : "=vr"(out) : "vr"(src));
#endif
    return out;
}
#define MIMO_EQ2_DIRECT_NARROW_Q15(src_, vl_) \
    mimo_eq2_direct_vnclip15_implicit((src_))
#define MIMO_EQ2_DIRECT_NARROW_ZERO(src_, vl_) \
    mimo_eq2_direct_vnclip0((src_))
#else
#define MIMO_EQ2_DIRECT_NARROW_Q15(src_, vl_) \
    __riscv_vnclip_wx_i16mf2((src_), MIMO_Q15_SHIFT, 0, (vl_))
#define MIMO_EQ2_DIRECT_NARROW_ZERO(src_, vl_) \
    __riscv_vnclip_wx_i16mf2((src_), 0, 0, (vl_))
#endif

static MIMO_ALWAYS_INLINE void mimo_eq2_rvv_direct_h_loop(
    size_t site_count,
    const int16_t *restrict h_ri_soa,
    const int16_t *restrict y_ri_soa,
    mimo_c16_t *restrict x)
{
#if MIMO_USE_RVV_EQ2_BLOCK_SOA
    const int16_t *h_block = h_ri_soa;
    const int16_t *y_block = y_ri_soa;
#else
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
#endif
    int16_t *const x_ri = (int16_t *)(void *)x;

    for (size_t site = 0; site < site_count;) {
        const size_t vl = __riscv_vsetvl_e16mf2(site_count - site);
#if MIMO_USE_RVV_EQ2_BLOCK_SOA
#if MIMO_USE_RVV_EQ2_GROUP_LOAD
#if MIMO_USE_RVV_EQ2_GROUP_LOAD_M4
        const vint16m4_t h_lo_group = __riscv_vle16_v_i16m4(
            h_block, 4 * MIMO_EQ2_BLOCK_SOA_LANES);
        const vint16m4_t h_hi_group = __riscv_vle16_v_i16m4(
            h_block + 4 * MIMO_EQ2_BLOCK_SOA_LANES,
            4 * MIMO_EQ2_BLOCK_SOA_LANES);
        const vint16m4_t y_group = __riscv_vle16_v_i16m4(
            y_block, 4 * MIMO_EQ2_BLOCK_SOA_LANES);
        const vint16mf2_t h00r =
            __riscv_vget_v_i16m4_i16m1(h_lo_group, 0);
        const vint16mf2_t h00i =
            __riscv_vget_v_i16m4_i16m1(h_lo_group, 1);
        const vint16mf2_t h01r =
            __riscv_vget_v_i16m4_i16m1(h_lo_group, 2);
        const vint16mf2_t h01i =
            __riscv_vget_v_i16m4_i16m1(h_lo_group, 3);
        const vint16mf2_t h10r =
            __riscv_vget_v_i16m4_i16m1(h_hi_group, 0);
        const vint16mf2_t h10i =
            __riscv_vget_v_i16m4_i16m1(h_hi_group, 1);
        const vint16mf2_t h11r =
            __riscv_vget_v_i16m4_i16m1(h_hi_group, 2);
        const vint16mf2_t h11i =
            __riscv_vget_v_i16m4_i16m1(h_hi_group, 3);
#else
        const vint16m8_t h_group = __riscv_vle16_v_i16m8(
            h_block,
            MIMO_EQ2_H_RI_COMPONENTS * MIMO_EQ2_BLOCK_SOA_LANES);
        const vint16m4_t y_group = __riscv_vle16_v_i16m4(
            y_block,
            MIMO_EQ2_Y_RI_COMPONENTS * MIMO_EQ2_BLOCK_SOA_LANES);
        const vint16mf2_t h00r =
            __riscv_vget_v_i16m8_i16m1(h_group, 0);
        const vint16mf2_t h00i =
            __riscv_vget_v_i16m8_i16m1(h_group, 1);
        const vint16mf2_t h01r =
            __riscv_vget_v_i16m8_i16m1(h_group, 2);
        const vint16mf2_t h01i =
            __riscv_vget_v_i16m8_i16m1(h_group, 3);
        const vint16mf2_t h10r =
            __riscv_vget_v_i16m8_i16m1(h_group, 4);
        const vint16mf2_t h10i =
            __riscv_vget_v_i16m8_i16m1(h_group, 5);
        const vint16mf2_t h11r =
            __riscv_vget_v_i16m8_i16m1(h_group, 6);
        const vint16mf2_t h11i =
            __riscv_vget_v_i16m8_i16m1(h_group, 7);
#endif
        const vint16mf2_t y0r =
            __riscv_vget_v_i16m4_i16m1(y_group, 0);
        const vint16mf2_t y0i =
            __riscv_vget_v_i16m4_i16m1(y_group, 1);
        const vint16mf2_t y1r =
            __riscv_vget_v_i16m4_i16m1(y_group, 2);
        const vint16mf2_t y1i =
            __riscv_vget_v_i16m4_i16m1(y_group, 3);
#else
        const vint16mf2_t h00r = __riscv_vle16_v_i16mf2(
            &h_block[0 * MIMO_EQ2_BLOCK_SOA_LANES], vl);
        const vint16mf2_t h00i = __riscv_vle16_v_i16mf2(
            &h_block[1 * MIMO_EQ2_BLOCK_SOA_LANES], vl);
        const vint16mf2_t h01r = __riscv_vle16_v_i16mf2(
            &h_block[2 * MIMO_EQ2_BLOCK_SOA_LANES], vl);
        const vint16mf2_t h01i = __riscv_vle16_v_i16mf2(
            &h_block[3 * MIMO_EQ2_BLOCK_SOA_LANES], vl);
        const vint16mf2_t h10r = __riscv_vle16_v_i16mf2(
            &h_block[4 * MIMO_EQ2_BLOCK_SOA_LANES], vl);
        const vint16mf2_t h10i = __riscv_vle16_v_i16mf2(
            &h_block[5 * MIMO_EQ2_BLOCK_SOA_LANES], vl);
        const vint16mf2_t h11r = __riscv_vle16_v_i16mf2(
            &h_block[6 * MIMO_EQ2_BLOCK_SOA_LANES], vl);
        const vint16mf2_t h11i = __riscv_vle16_v_i16mf2(
            &h_block[7 * MIMO_EQ2_BLOCK_SOA_LANES], vl);
        const vint16mf2_t y0r = __riscv_vle16_v_i16mf2(
            &y_block[0 * MIMO_EQ2_BLOCK_SOA_LANES], vl);
        const vint16mf2_t y0i = __riscv_vle16_v_i16mf2(
            &y_block[1 * MIMO_EQ2_BLOCK_SOA_LANES], vl);
        const vint16mf2_t y1r = __riscv_vle16_v_i16mf2(
            &y_block[2 * MIMO_EQ2_BLOCK_SOA_LANES], vl);
        const vint16mf2_t y1i = __riscv_vle16_v_i16mf2(
            &y_block[3 * MIMO_EQ2_BLOCK_SOA_LANES], vl);
#endif
#else
        const vint16mf2_t h00r =
            __riscv_vle16_v_i16mf2(&h00r_base[site], vl);
        const vint16mf2_t h00i =
            __riscv_vle16_v_i16mf2(&h00i_base[site], vl);
        const vint16mf2_t h01r =
            __riscv_vle16_v_i16mf2(&h01r_base[site], vl);
        const vint16mf2_t h01i =
            __riscv_vle16_v_i16mf2(&h01i_base[site], vl);
        const vint16mf2_t h10r =
            __riscv_vle16_v_i16mf2(&h10r_base[site], vl);
        const vint16mf2_t h10i =
            __riscv_vle16_v_i16mf2(&h10i_base[site], vl);
        const vint16mf2_t h11r =
            __riscv_vle16_v_i16mf2(&h11r_base[site], vl);
        const vint16mf2_t h11i =
            __riscv_vle16_v_i16mf2(&h11i_base[site], vl);
        const vint16mf2_t y0r =
            __riscv_vle16_v_i16mf2(&y0r_base[site], vl);
        const vint16mf2_t y0i =
            __riscv_vle16_v_i16mf2(&y0i_base[site], vl);
        const vint16mf2_t y1r =
            __riscv_vle16_v_i16mf2(&y1r_base[site], vl);
        const vint16mf2_t y1i =
            __riscv_vle16_v_i16mf2(&y1i_base[site], vl);
#endif
        const vint16mf2_t neg_h00i = __riscv_vneg_v_i16mf2(h00i, vl);
        const vint16mf2_t neg_h01r = __riscv_vneg_v_i16mf2(h01r, vl);
        const vint16mf2_t neg_h01i = __riscv_vneg_v_i16mf2(h01i, vl);
        const vint16mf2_t neg_h10r = __riscv_vneg_v_i16mf2(h10r, vl);
        const vint16mf2_t neg_h10i = __riscv_vneg_v_i16mf2(h10i, vl);
        const vint16mf2_t neg_h11i = __riscv_vneg_v_i16mf2(h11i, vl);
        vint32m1_t dr_q30 = __riscv_vwmul_vv_i32m1(h00r, h11r, vl);
        dr_q30 = __riscv_vwmacc_vv_i32m1(dr_q30, neg_h00i, h11i, vl);
        dr_q30 = __riscv_vwmacc_vv_i32m1(dr_q30, neg_h01r, h10r, vl);
        dr_q30 = __riscv_vwmacc_vv_i32m1(dr_q30, h01i, h10i, vl);
        vint32m1_t di_q30 = __riscv_vwmul_vv_i32m1(h00r, h11i, vl);
        di_q30 = __riscv_vwmacc_vv_i32m1(di_q30, h00i, h11r, vl);
        di_q30 = __riscv_vwmacc_vv_i32m1(di_q30, neg_h01r, h10i, vl);
        di_q30 = __riscv_vwmacc_vv_i32m1(di_q30, neg_h01i, h10r, vl);

        vint32m1_t n0r_q30 = __riscv_vwmul_vv_i32m1(h11r, y0r, vl);
        n0r_q30 = __riscv_vwmacc_vv_i32m1(n0r_q30, neg_h11i, y0i, vl);
        n0r_q30 = __riscv_vwmacc_vv_i32m1(n0r_q30, neg_h01r, y1r, vl);
        n0r_q30 = __riscv_vwmacc_vv_i32m1(n0r_q30, h01i, y1i, vl);
        vint32m1_t n0i_q30 = __riscv_vwmul_vv_i32m1(h11r, y0i, vl);
        n0i_q30 = __riscv_vwmacc_vv_i32m1(n0i_q30, h11i, y0r, vl);
        n0i_q30 = __riscv_vwmacc_vv_i32m1(n0i_q30, neg_h01r, y1i, vl);
        n0i_q30 = __riscv_vwmacc_vv_i32m1(n0i_q30, neg_h01i, y1r, vl);

#if MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP
#else
        vint16mf2_t dr = MIMO_EQ2_DIRECT_NARROW_Q15(dr_q30, vl);
        vint16mf2_t di = MIMO_EQ2_DIRECT_NARROW_Q15(di_q30, vl);
        vint16mf2_t n0r = MIMO_EQ2_DIRECT_NARROW_Q15(n0r_q30, vl);
        vint16mf2_t n0i = MIMO_EQ2_DIRECT_NARROW_Q15(n0i_q30, vl);
#endif
        vint32m1_t n1r_q30 = __riscv_vwmul_vv_i32m1(h00r, y1r, vl);
        n1r_q30 = __riscv_vwmacc_vv_i32m1(n1r_q30, neg_h00i, y1i, vl);
        n1r_q30 = __riscv_vwmacc_vv_i32m1(n1r_q30, neg_h10r, y0r, vl);
        n1r_q30 = __riscv_vwmacc_vv_i32m1(n1r_q30, h10i, y0i, vl);
        vint32m1_t n1i_q30 = __riscv_vwmul_vv_i32m1(h00r, y1i, vl);
        n1i_q30 = __riscv_vwmacc_vv_i32m1(n1i_q30, h00i, y1r, vl);
        n1i_q30 = __riscv_vwmacc_vv_i32m1(n1i_q30, neg_h10r, y0i, vl);
        n1i_q30 = __riscv_vwmacc_vv_i32m1(n1i_q30, neg_h10i, y0r, vl);
#if MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP
        asm volatile(
            "vsetvli zero, %[vl], e32, m2, ta, ma\n\t"
            "vssra.vi %[dr], %[dr], 14\n\t"
            "vssra.vi %[di], %[di], 14\n\t"
            "vssra.vi %[n0r], %[n0r], 14\n\t"
            "vssra.vi %[n0i], %[n0i], 14\n\t"
            "vssra.vi %[n1r], %[n1r], 14\n\t"
            "vssra.vi %[n1i], %[n1i], 14"
            : [dr] "+&vr"(dr_q30), [di] "+&vr"(di_q30),
              [n0r] "+&vr"(n0r_q30), [n0i] "+&vr"(n0i_q30),
              [n1r] "+&vr"(n1r_q30), [n1i] "+&vr"(n1i_q30)
            : [vl] "r"(vl));
        vint32m1_t dr = dr_q30;
        vint32m1_t di = di_q30;
        vint32m1_t n0r = n0r_q30;
        vint32m1_t n0i = n0i_q30;
        vint32m1_t n1r = n1r_q30;
        vint32m1_t n1i = n1i_q30;
#else
        vint16mf2_t n1r = MIMO_EQ2_DIRECT_NARROW_Q15(n1r_q30, vl);
        vint16mf2_t n1i = MIMO_EQ2_DIRECT_NARROW_Q15(n1i_q30, vl);
#endif
#if MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP
        vint16mf2_t x0r16;
        vint16mf2_t x0i16;
        vint16mf2_t x1r16;
        vint16mf2_t x1i16;
        uintptr_t out_cursor;

        asm volatile(
            "vmul.vv v28, %[dr], %[dr]\n\t"
            "vmacc.vv v28, %[di], %[di]\n\t"
            "vssra.vi v28, v28, 16\n\t"
            "vsub.vx v28, v28, %[det_min]\n\t"
            "vsll.vi v28, v28, 2\n\t"
            "vluxei32.v v30, (%[table]), v28\n\t"
            "vmul.vv v20, %[n0r], %[dr]\n\t"
            "vmacc.vv v20, %[n0i], %[di]\n\t"
            "vmul.vv v22, %[n0i], %[dr]\n\t"
            "vnmsac.vv v22, %[n0r], %[di]\n\t"
            "vmul.vv v24, %[n1r], %[dr]\n\t"
            "vmacc.vv v24, %[n1i], %[di]\n\t"
            "vmul.vv v26, %[n1i], %[dr]\n\t"
            "vnmsac.vv v26, %[n1r], %[di]\n\t"
            "vmulh.vv v20, v20, v30\n\t"
            "vmulh.vv v22, v22, v30\n\t"
            "vmulh.vv v24, v24, v30\n\t"
            "vmulh.vv v26, v26, v30\n\t"
            "vsetvli zero, %[vl], e16, m1, ta, ma\n\t"
            "vnclip.wi %[x0r], v20, 0\n\t"
            "vnclip.wi %[x0i], v22, 0\n\t"
            "vnclip.wi %[x1r], v24, 0\n\t"
            "vnclip.wi %[x1i], v26, 0\n\t"
            "vse16.v %[x0r], (%[out0])\n\t"
            "sh1add %[out], %[stride], %[out0]\n\t"
            "vse16.v %[x0i], (%[out])\n\t"
            "sh1add %[out], %[stride], %[out]\n\t"
            "vse16.v %[x1r], (%[out])\n\t"
            "sh1add %[out], %[stride], %[out]\n\t"
            "vse16.v %[x1i], (%[out])"
            : [x0r] "=&vr"(x0r16), [x0i] "=&vr"(x0i16),
              [x1r] "=&vr"(x1r16), [x1i] "=&vr"(x1i16),
              [out] "=&r"(out_cursor)
            : [dr] "vr"(dr), [di] "vr"(di),
              [n0r] "vr"(n0r), [n0i] "vr"(n0i),
              [n1r] "vr"(n1r), [n1i] "vr"(n1i),
              [det_min] "r"(MIMO_EQ2_DET_Q16_RECIP_MIN),
              [table] "r"(mimo_eq2_rvv_det_recip_direct_half_q15),
              [vl] "r"(vl), [out0] "r"(&x_ri[site]),
              [stride] "r"(site_count)
            : "v20", "v21", "v22", "v23", "v24", "v25",
              "v26", "v27", "v28", "v29", "v30", "v31", "memory");
#else
#if MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP
        const vint32m1_t neg_di = __riscv_vneg_v_i32m1(di, vl);
        vint32m1_t det_q30 = __riscv_vmul_vv_i32m1(dr, dr, vl);
        det_q30 = __riscv_vmacc_vv_i32m1(det_q30, di, di, vl);

        vint32m1_t det_q15 = det_q30;
        asm volatile("vssra.vi %0, %0, 16" : "+vr"(det_q15));
        const vint32m1_t rel = __riscv_vsub_vx_i32m1(
            det_q15, MIMO_EQ2_DET_Q16_RECIP_MIN, vl);
        const vuint32m1_t byte_index = __riscv_vsll_vx_u32m1(
            __riscv_vreinterpret_v_i32m1_u32m1(rel), 2, vl);
        const vint32m1_t scale_q30 = __riscv_vluxei32_v_i32m1(
            mimo_eq2_rvv_det_recip_direct_half_q15, byte_index, vl);
        asm volatile("" : : "vr"(scale_q30));
#else
        const vint16mf2_t neg_di = __riscv_vneg_v_i16mf2(di, vl);
        vint32m1_t det_q30 = __riscv_vwmul_vv_i32m1(dr, dr, vl);
        det_q30 = __riscv_vwmacc_vv_i32m1(det_q30, di, di, vl);
#endif
#if !MIMO_USE_EQ2_DIRECT_Q13_INV
#if MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP
        vint32m1_t c0r = __riscv_vmul_vv_i32m1(n0r, dr, vl);
        c0r = __riscv_vmacc_vv_i32m1(c0r, n0i, di, vl);
        vint32m1_t c0i = __riscv_vmul_vv_i32m1(n0i, dr, vl);
        c0i = __riscv_vmacc_vv_i32m1(c0i, n0r, neg_di, vl);
        vint32m1_t c1r = __riscv_vmul_vv_i32m1(n1r, dr, vl);
        c1r = __riscv_vmacc_vv_i32m1(c1r, n1i, di, vl);
        vint32m1_t c1i = __riscv_vmul_vv_i32m1(n1i, dr, vl);
        c1i = __riscv_vmacc_vv_i32m1(c1i, n1r, neg_di, vl);
#else
        vint32m1_t c0r = __riscv_vwmul_vv_i32m1(n0r, dr, vl);
        c0r = __riscv_vwmacc_vv_i32m1(c0r, n0i, di, vl);
        vint32m1_t c0i = __riscv_vwmul_vv_i32m1(n0i, dr, vl);
        c0i = __riscv_vwmacc_vv_i32m1(c0i, n0r, neg_di, vl);
        vint32m1_t c1r = __riscv_vwmul_vv_i32m1(n1r, dr, vl);
        c1r = __riscv_vwmacc_vv_i32m1(c1r, n1i, di, vl);
        vint32m1_t c1i = __riscv_vwmul_vv_i32m1(n1i, dr, vl);
        c1i = __riscv_vwmacc_vv_i32m1(c1i, n1r, neg_di, vl);
#endif
#else
        asm volatile("" : "+vr"(det_q30));
#endif

#if MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP
#elif MIMO_USE_RVV_EQ2_IMPLICIT_VXRM
        const vint32m1_t det_rounded = __riscv_vadd_vx_i32m1(
            det_q30, MIMO_Q15_ROUND, vl);
        const vint32m1_t det_q15 = __riscv_vsra_vx_i32m1(
            det_rounded, MIMO_Q15_SHIFT, vl);
        const vint32m1_t rel = __riscv_vsub_vx_i32m1(
            det_q15, MIMO_EQ2_DET_Q15_RECIP_MIN, vl);
#else
        const vint32m1_t det_q15 = __riscv_vssra_vx_i32m1(
            det_q30, MIMO_Q15_SHIFT, 0, vl);
        const vint32m1_t rel = __riscv_vsub_vx_i32m1(
            det_q15, MIMO_EQ2_DET_Q15_RECIP_MIN, vl);
#endif
#if MIMO_USE_EQ2_DIRECT_Q13_INV
        const vuint32m1_t byte_index = __riscv_vsll_vx_u32m1(
            __riscv_vreinterpret_v_i32m1_u32m1(rel), 1, vl);
        const vint16mf2_t scale_q26 =
            __riscv_vreinterpret_v_u16mf2_i16mf2(
                __riscv_vluxei32_v_u16mf2(
                    mimo_eq2_rvv_det_recip_direct1_q26, byte_index, vl));
        vint32m1_t inv_r_q30 = __riscv_vwmul_vv_i32m1(dr, scale_q26, vl);
        vint32m1_t inv_i_q30 =
            __riscv_vwmul_vv_i32m1(neg_di, scale_q26, vl);
        const vint16mf2_t inv_r = __riscv_vsll_vx_i16mf2(
            MIMO_EQ2_DIRECT_NARROW_Q15(inv_r_q30, vl), 2, vl);
        const vint16mf2_t inv_i = __riscv_vsll_vx_i16mf2(
            MIMO_EQ2_DIRECT_NARROW_Q15(inv_i_q30, vl), 2, vl);
        const vint16mf2_t neg_inv_i = __riscv_vneg_v_i16mf2(inv_i, vl);

        vint32m1_t x0r_q30 = __riscv_vwmul_vv_i32m1(n0r, inv_r, vl);
        x0r_q30 = __riscv_vwmacc_vv_i32m1(x0r_q30, n0i, neg_inv_i, vl);
        vint32m1_t x0i_q30 = __riscv_vwmul_vv_i32m1(n0r, inv_i, vl);
        x0i_q30 = __riscv_vwmacc_vv_i32m1(x0i_q30, n0i, inv_r, vl);
        vint32m1_t x1r_q30 = __riscv_vwmul_vv_i32m1(n1r, inv_r, vl);
        x1r_q30 = __riscv_vwmacc_vv_i32m1(x1r_q30, n1i, neg_inv_i, vl);
        vint32m1_t x1i_q30 = __riscv_vwmul_vv_i32m1(n1r, inv_i, vl);
        x1i_q30 = __riscv_vwmacc_vv_i32m1(x1i_q30, n1i, inv_r, vl);
        vint16mf2_t x0r16 = __riscv_vsll_vx_i16mf2(
            MIMO_EQ2_DIRECT_NARROW_Q15(x0r_q30, vl), 2, vl);
        vint16mf2_t x0i16 = __riscv_vsll_vx_i16mf2(
            MIMO_EQ2_DIRECT_NARROW_Q15(x0i_q30, vl), 2, vl);
        vint16mf2_t x1r16 = __riscv_vsll_vx_i16mf2(
            MIMO_EQ2_DIRECT_NARROW_Q15(x1r_q30, vl), 2, vl);
        vint16mf2_t x1i16 = __riscv_vsll_vx_i16mf2(
            MIMO_EQ2_DIRECT_NARROW_Q15(x1i_q30, vl), 2, vl);
#elif MIMO_USE_RVV_EQ2_UQ12_SCALE
        const vuint32m1_t byte_index = __riscv_vsll_vx_u32m1(
            __riscv_vreinterpret_v_i32m1_u32m1(rel), 1, vl);
        vint16mf2_t x0r16;
        vint16mf2_t x0i16;
        vint16mf2_t x1r16;
        vint16mf2_t x1i16;
        mimo_eq2_scale4_q30_with_uq12(
            &x0r16, &x0i16, &x1r16, &x1i16,
            c0r, c0i, c1r, c1i, byte_index, vl);
#else
#if MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP
#else
        const vuint32m1_t byte_index = __riscv_vsll_vx_u32m1(
            __riscv_vreinterpret_v_i32m1_u32m1(rel), 2, vl);
        const vint32m1_t scale_q30 = __riscv_vluxei32_v_i32m1(
            mimo_eq2_rvv_det_recip_direct1_q30, byte_index, vl);
#endif
        vint32m1_t x0r = c0r;
        vint32m1_t x0i = c0i;
        vint32m1_t x1r = c1r;
        vint32m1_t x1i = c1i;
#if MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP
        asm volatile(
            "vmulh.vv %[x0r], %[x0r], %[scale]\n\t"
            "vmulh.vv %[x0i], %[x0i], %[scale]\n\t"
            "vmulh.vv %[x1r], %[x1r], %[scale]\n\t"
            "vmulh.vv %[x1i], %[x1i], %[scale]"
            : [x0r] "+&vr"(x0r), [x0i] "+&vr"(x0i),
              [x1r] "+&vr"(x1r), [x1i] "+&vr"(x1i)
            : [scale] "vr"(scale_q30));
#else
        mimo_eq2_scale4_q30_to_q15(
            &x0r, &x0i, &x1r, &x1i, scale_q30);
#endif
        asm volatile(""
                     : "+vr"(x0r), "+vr"(x0i),
                       "+vr"(x1r), "+vr"(x1i));
#if MIMO_USE_RVV_EQ2_IMPLICIT_VXRM
#if MIMO_USE_RVV_EQ2_M1
        asm volatile("vsetvli zero, %0, e16, m1, ta, ma"
                     : : "r"(vl));
#else
        asm volatile("vsetvli zero, %0, e16, mf2, ta, ma"
                     : : "r"(vl));
#endif
#endif
        vint16mf2_t x0r16 = MIMO_EQ2_DIRECT_NARROW_ZERO(x0r, vl);
        vint16mf2_t x0i16 = MIMO_EQ2_DIRECT_NARROW_ZERO(x0i, vl);
        vint16mf2_t x1r16 = MIMO_EQ2_DIRECT_NARROW_ZERO(x1r, vl);
        vint16mf2_t x1i16 = MIMO_EQ2_DIRECT_NARROW_ZERO(x1i, vl);
#endif
#endif
#if !MIMO_USE_RVV_EQ2_UQ12_SCALE && \
    !MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP
        asm volatile(""
                     : "+vr"(x0r16), "+vr"(x0i16),
                       "+vr"(x1r16), "+vr"(x1i16));
#endif

#if !MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP
        __riscv_vse16_v_i16mf2(&x_ri[site], x0r16, vl);
        __riscv_vse16_v_i16mf2(&x_ri[site_count + site], x0i16, vl);
        __riscv_vse16_v_i16mf2(&x_ri[2 * site_count + site], x1r16, vl);
        __riscv_vse16_v_i16mf2(&x_ri[3 * site_count + site], x1i16, vl);
#endif
#if MIMO_USE_RVV_EQ2_BLOCK_SOA
        h_block += MIMO_EQ2_H_RI_COMPONENTS * MIMO_EQ2_BLOCK_SOA_LANES;
        y_block += MIMO_EQ2_Y_RI_COMPONENTS * MIMO_EQ2_BLOCK_SOA_LANES;
#endif
        site += vl;
    }
}

#undef MIMO_EQ2_DIRECT_NARROW_Q15
#undef MIMO_EQ2_DIRECT_NARROW_ZERO
#endif

static MIMO_ALWAYS_INLINE void mimo_eq2_rvv_ri_soa_loop(
    size_t site_count,
    const int16_t *restrict h_ri_soa,
    const int16_t *restrict y_ri_soa,
    int32_t lambda_q15,
    mimo_c16_t *restrict x)
{
#if MIMO_USE_RVV_EQ2_BLOCK_SOA
    const int16_t *h_block = h_ri_soa;
    const int16_t *y_block = y_ri_soa;
#else
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
#endif

    for (size_t site = 0; site < site_count;) {
        const size_t vl = __riscv_vsetvl_e16mf2(site_count - site);
#if MIMO_USE_RVV_EQ2_BLOCK_SOA
#if MIMO_USE_RVV_EQ2_GROUP_LOAD
#if MIMO_USE_RVV_EQ2_GROUP_LOAD_M4
        const vint16m4_t h_lo_group = __riscv_vle16_v_i16m4(
            h_block, 4 * MIMO_EQ2_BLOCK_SOA_LANES);
        const vint16m4_t h_hi_group = __riscv_vle16_v_i16m4(
            h_block + 4 * MIMO_EQ2_BLOCK_SOA_LANES,
            4 * MIMO_EQ2_BLOCK_SOA_LANES);
        const vint16m4_t y_group = __riscv_vle16_v_i16m4(
            y_block, 4 * MIMO_EQ2_BLOCK_SOA_LANES);
        const vint16mf2_t h00r =
            __riscv_vget_v_i16m4_i16m1(h_lo_group, 0);
        const vint16mf2_t h00i =
            __riscv_vget_v_i16m4_i16m1(h_lo_group, 1);
        const vint16mf2_t h01r =
            __riscv_vget_v_i16m4_i16m1(h_lo_group, 2);
        const vint16mf2_t h01i =
            __riscv_vget_v_i16m4_i16m1(h_lo_group, 3);
        const vint16mf2_t h10r =
            __riscv_vget_v_i16m4_i16m1(h_hi_group, 0);
        const vint16mf2_t h10i =
            __riscv_vget_v_i16m4_i16m1(h_hi_group, 1);
        const vint16mf2_t h11r =
            __riscv_vget_v_i16m4_i16m1(h_hi_group, 2);
        const vint16mf2_t h11i =
            __riscv_vget_v_i16m4_i16m1(h_hi_group, 3);
#else
        const vint16m8_t h_group = __riscv_vle16_v_i16m8(
            h_block,
            MIMO_EQ2_H_RI_COMPONENTS * MIMO_EQ2_BLOCK_SOA_LANES);
        const vint16m4_t y_group = __riscv_vle16_v_i16m4(
            y_block,
            MIMO_EQ2_Y_RI_COMPONENTS * MIMO_EQ2_BLOCK_SOA_LANES);
        const vint16mf2_t h00r =
            __riscv_vget_v_i16m8_i16m1(h_group, 0);
        const vint16mf2_t h00i =
            __riscv_vget_v_i16m8_i16m1(h_group, 1);
        const vint16mf2_t h01r =
            __riscv_vget_v_i16m8_i16m1(h_group, 2);
        const vint16mf2_t h01i =
            __riscv_vget_v_i16m8_i16m1(h_group, 3);
        const vint16mf2_t h10r =
            __riscv_vget_v_i16m8_i16m1(h_group, 4);
        const vint16mf2_t h10i =
            __riscv_vget_v_i16m8_i16m1(h_group, 5);
        const vint16mf2_t h11r =
            __riscv_vget_v_i16m8_i16m1(h_group, 6);
        const vint16mf2_t h11i =
            __riscv_vget_v_i16m8_i16m1(h_group, 7);
#endif
        const vint16mf2_t y0r =
            __riscv_vget_v_i16m4_i16m1(y_group, 0);
        const vint16mf2_t y0i =
            __riscv_vget_v_i16m4_i16m1(y_group, 1);
        const vint16mf2_t y1r =
            __riscv_vget_v_i16m4_i16m1(y_group, 2);
        const vint16mf2_t y1i =
            __riscv_vget_v_i16m4_i16m1(y_group, 3);
#else
        const vint16mf2_t h00r = __riscv_vle16_v_i16mf2(
            &h_block[0 * MIMO_EQ2_BLOCK_SOA_LANES], vl);
        const vint16mf2_t h00i = __riscv_vle16_v_i16mf2(
            &h_block[1 * MIMO_EQ2_BLOCK_SOA_LANES], vl);
        const vint16mf2_t h01r = __riscv_vle16_v_i16mf2(
            &h_block[2 * MIMO_EQ2_BLOCK_SOA_LANES], vl);
        const vint16mf2_t h01i = __riscv_vle16_v_i16mf2(
            &h_block[3 * MIMO_EQ2_BLOCK_SOA_LANES], vl);
        const vint16mf2_t h10r = __riscv_vle16_v_i16mf2(
            &h_block[4 * MIMO_EQ2_BLOCK_SOA_LANES], vl);
        const vint16mf2_t h10i = __riscv_vle16_v_i16mf2(
            &h_block[5 * MIMO_EQ2_BLOCK_SOA_LANES], vl);
        const vint16mf2_t h11r = __riscv_vle16_v_i16mf2(
            &h_block[6 * MIMO_EQ2_BLOCK_SOA_LANES], vl);
        const vint16mf2_t h11i = __riscv_vle16_v_i16mf2(
            &h_block[7 * MIMO_EQ2_BLOCK_SOA_LANES], vl);
        const vint16mf2_t y0r = __riscv_vle16_v_i16mf2(
            &y_block[0 * MIMO_EQ2_BLOCK_SOA_LANES], vl);
        const vint16mf2_t y0i = __riscv_vle16_v_i16mf2(
            &y_block[1 * MIMO_EQ2_BLOCK_SOA_LANES], vl);
        const vint16mf2_t y1r = __riscv_vle16_v_i16mf2(
            &y_block[2 * MIMO_EQ2_BLOCK_SOA_LANES], vl);
        const vint16mf2_t y1i = __riscv_vle16_v_i16mf2(
            &y_block[3 * MIMO_EQ2_BLOCK_SOA_LANES], vl);
#endif
#else
        const vint16mf2_t h00r =
            __riscv_vle16_v_i16mf2(&h00r_base[site], vl);
        const vint16mf2_t h00i =
            __riscv_vle16_v_i16mf2(&h00i_base[site], vl);
        const vint16mf2_t h01r =
            __riscv_vle16_v_i16mf2(&h01r_base[site], vl);
        const vint16mf2_t h01i =
            __riscv_vle16_v_i16mf2(&h01i_base[site], vl);
        const vint16mf2_t h10r =
            __riscv_vle16_v_i16mf2(&h10r_base[site], vl);
        const vint16mf2_t h10i =
            __riscv_vle16_v_i16mf2(&h10i_base[site], vl);
        const vint16mf2_t h11r =
            __riscv_vle16_v_i16mf2(&h11r_base[site], vl);
        const vint16mf2_t h11i =
            __riscv_vle16_v_i16mf2(&h11i_base[site], vl);
#if !MIMO_USE_RVV_EQ2_SPLIT_BUILD
        const vint16mf2_t y0r =
            __riscv_vle16_v_i16mf2(&y0r_base[site], vl);
        const vint16mf2_t y0i =
            __riscv_vle16_v_i16mf2(&y0i_base[site], vl);
        const vint16mf2_t y1r =
            __riscv_vle16_v_i16mf2(&y1r_base[site], vl);
        const vint16mf2_t y1i =
            __riscv_vle16_v_i16mf2(&y1i_base[site], vl);
#endif
#endif
        vint16mf2_t a00;
        vint16mf2_t a01r;
        vint16mf2_t a01i;
        vint16mf2_t a11;
        vint16mf2_t b0r;
        vint16mf2_t b0i;
        vint16mf2_t b1r;
        vint16mf2_t b1i;

#if MIMO_USE_RVV_EQ2_SPLIT_BUILD
        vint32m1_t a00q30 = mimo_eq2_abs2_dot2(
            h00r, h00i, h10r, h10i, vl);
#if MIMO_USE_RVV_EQ2_3M_BUILD
        vint32m1_t a01rq30;
        vint32m1_t a01iq30;
        MIMO_EQ2_CONJ_DOT2_3M(
            a01rq30, a01iq30,
            h00r, h00i, h01r, h01i,
            h10r, h10i, h11r, h11i, vl);
#else
        vint32m1_t a01rq30 = mimo_eq2_conj_dot2_re(
            h00r, h00i, h01r, h01i, h10r, h10i, h11r, h11i, vl);
        vint32m1_t a01iq30 = mimo_eq2_conj_dot2_im(
            h00r, h00i, h01r, h01i, h10r, h10i, h11r, h11i, vl);
#endif
        vint32m1_t a11q30 = mimo_eq2_abs2_dot2(
            h01r, h01i, h11r, h11i, vl);

#if MIMO_USE_RVV_EQ2_FUSED_NCLIP_BUILD
        a00 = __riscv_vnclip_wx_i16mf2(
            a00q30, MIMO_Q15_SHIFT, 0, vl);
        a01r = __riscv_vnclip_wx_i16mf2(
            a01rq30, MIMO_Q15_SHIFT, 0, vl);
        a01i = __riscv_vnclip_wx_i16mf2(
            a01iq30, MIMO_Q15_SHIFT, 0, vl);
        a11 = __riscv_vnclip_wx_i16mf2(
            a11q30, MIMO_Q15_SHIFT, 0, vl);
        if (lambda_q15 != 0) {
            a00 = __riscv_vsadd_vx_i16mf2(a00, lambda_q15, vl);
            a11 = __riscv_vsadd_vx_i16mf2(a11, lambda_q15, vl);
        }
#else
        a00q30 = __riscv_vssra_vx_i32m1(
            a00q30, MIMO_Q15_SHIFT, 0, vl);
        a01rq30 = __riscv_vssra_vx_i32m1(
            a01rq30, MIMO_Q15_SHIFT, 0, vl);
        a01iq30 = __riscv_vssra_vx_i32m1(
            a01iq30, MIMO_Q15_SHIFT, 0, vl);
        a11q30 = __riscv_vssra_vx_i32m1(
            a11q30, MIMO_Q15_SHIFT, 0, vl);
        if (lambda_q15 != 0) {
            a00q30 = __riscv_vadd_vx_i32m1(a00q30, lambda_q15, vl);
            a11q30 = __riscv_vadd_vx_i32m1(a11q30, lambda_q15, vl);
        }
        a00 = __riscv_vnclip_wx_i16mf2(a00q30, 0, 0, vl);
        a01r = __riscv_vnclip_wx_i16mf2(a01rq30, 0, 0, vl);
        a01i = __riscv_vnclip_wx_i16mf2(a01iq30, 0, 0, vl);
        a11 = __riscv_vnclip_wx_i16mf2(a11q30, 0, 0, vl);
#endif
        asm volatile(""
                     : "+vr"(a00), "+vr"(a01r),
                       "+vr"(a01i), "+vr"(a11)
                     :
                     : "memory");

        const vint16mf2_t y0r =
            __riscv_vle16_v_i16mf2(&y0r_base[site], vl);
        const vint16mf2_t y0i =
            __riscv_vle16_v_i16mf2(&y0i_base[site], vl);
        const vint16mf2_t y1r =
            __riscv_vle16_v_i16mf2(&y1r_base[site], vl);
        const vint16mf2_t y1i =
            __riscv_vle16_v_i16mf2(&y1i_base[site], vl);
#if MIMO_USE_RVV_EQ2_3M_BUILD
        vint32m1_t b0rq30;
        vint32m1_t b0iq30;
        vint32m1_t b1rq30;
        vint32m1_t b1iq30;
        MIMO_EQ2_CONJ_DOT2_3M(
            b0rq30, b0iq30,
            h00r, h00i, y0r, y0i,
            h10r, h10i, y1r, y1i, vl);
        MIMO_EQ2_CONJ_DOT2_3M(
            b1rq30, b1iq30,
            h01r, h01i, y0r, y0i,
            h11r, h11i, y1r, y1i, vl);
#else
        vint32m1_t b0rq30 = mimo_eq2_conj_dot2_re(
            h00r, h00i, y0r, y0i, h10r, h10i, y1r, y1i, vl);
        vint32m1_t b0iq30 = mimo_eq2_conj_dot2_im(
            h00r, h00i, y0r, y0i, h10r, h10i, y1r, y1i, vl);
        vint32m1_t b1rq30 = mimo_eq2_conj_dot2_re(
            h01r, h01i, y0r, y0i, h11r, h11i, y1r, y1i, vl);
        vint32m1_t b1iq30 = mimo_eq2_conj_dot2_im(
            h01r, h01i, y0r, y0i, h11r, h11i, y1r, y1i, vl);
#endif

#if MIMO_USE_RVV_EQ2_FUSED_NCLIP_BUILD
        b0r = __riscv_vnclip_wx_i16mf2(
            b0rq30, MIMO_Q15_SHIFT, 0, vl);
        b0i = __riscv_vnclip_wx_i16mf2(
            b0iq30, MIMO_Q15_SHIFT, 0, vl);
        b1r = __riscv_vnclip_wx_i16mf2(
            b1rq30, MIMO_Q15_SHIFT, 0, vl);
        b1i = __riscv_vnclip_wx_i16mf2(
            b1iq30, MIMO_Q15_SHIFT, 0, vl);
#else
        b0rq30 = __riscv_vssra_vx_i32m1(
            b0rq30, MIMO_Q15_SHIFT, 0, vl);
        b0iq30 = __riscv_vssra_vx_i32m1(
            b0iq30, MIMO_Q15_SHIFT, 0, vl);
        b1rq30 = __riscv_vssra_vx_i32m1(
            b1rq30, MIMO_Q15_SHIFT, 0, vl);
        b1iq30 = __riscv_vssra_vx_i32m1(
            b1iq30, MIMO_Q15_SHIFT, 0, vl);
        b0r = __riscv_vnclip_wx_i16mf2(b0rq30, 0, 0, vl);
        b0i = __riscv_vnclip_wx_i16mf2(b0iq30, 0, 0, vl);
        b1r = __riscv_vnclip_wx_i16mf2(b1rq30, 0, 0, vl);
        b1i = __riscv_vnclip_wx_i16mf2(b1iq30, 0, 0, vl);
#endif
#else
#if MIMO_USE_RVV_EQ2_STAGED_NARROW_BUILD
        const vint32m1_t a00q30 =
            mimo_eq2_abs2_dot2(h00r, h00i, h10r, h10i, vl);
        const vint32m1_t a01rq30 = mimo_eq2_conj_dot2_re(
            h00r, h00i, h01r, h01i,
            h10r, h10i, h11r, h11i, vl);
        const vint32m1_t a01iq30 = mimo_eq2_conj_dot2_im(
            h00r, h00i, h01r, h01i,
            h10r, h10i, h11r, h11i, vl);
        const vint32m1_t a11q30 =
            mimo_eq2_abs2_dot2(h01r, h01i, h11r, h11i, vl);

        a00 = MIMO_EQ2_RVV_NARROW_Q15(a00q30, vl);
        a01r = MIMO_EQ2_RVV_NARROW_Q15(a01rq30, vl);
        a01i = MIMO_EQ2_RVV_NARROW_Q15(a01iq30, vl);
        a11 = MIMO_EQ2_RVV_NARROW_Q15(a11q30, vl);
        asm volatile(""
                     : "+vr"(a00), "+vr"(a01r),
                       "+vr"(a01i), "+vr"(a11));
        const vint32m1_t b0rq30 = mimo_eq2_conj_dot2_re(
            h00r, h00i, y0r, y0i,
            h10r, h10i, y1r, y1i, vl);
        const vint32m1_t b0iq30 = mimo_eq2_conj_dot2_im(
            h00r, h00i, y0r, y0i,
            h10r, h10i, y1r, y1i, vl);
        const vint32m1_t b1rq30 = mimo_eq2_conj_dot2_re(
            h01r, h01i, y0r, y0i,
            h11r, h11i, y1r, y1i, vl);
        const vint32m1_t b1iq30 = mimo_eq2_conj_dot2_im(
            h01r, h01i, y0r, y0i,
            h11r, h11i, y1r, y1i, vl);

        b0r = MIMO_EQ2_RVV_NARROW_Q15(b0rq30, vl);
        b0i = MIMO_EQ2_RVV_NARROW_Q15(b0iq30, vl);
        b1r = MIMO_EQ2_RVV_NARROW_Q15(b1rq30, vl);
        b1i = MIMO_EQ2_RVV_NARROW_Q15(b1iq30, vl);
        asm volatile(""
                     : "+vr"(b0r), "+vr"(b0i),
                       "+vr"(b1r), "+vr"(b1i));
        if (lambda_q15 != 0) {
            a00 = __riscv_vsadd_vx_i16mf2(a00, lambda_q15, vl);
            a11 = __riscv_vsadd_vx_i16mf2(a11, lambda_q15, vl);
        }
#else
        vint32m1_t a00q30 = mimo_eq2_abs2_dot2(
            h00r, h00i, h10r, h10i, vl);
#if MIMO_USE_RVV_EQ2_3M_BUILD
        vint32m1_t a01rq30;
        vint32m1_t a01iq30;
        MIMO_EQ2_CONJ_DOT2_3M(
            a01rq30, a01iq30,
            h00r, h00i, h01r, h01i,
            h10r, h10i, h11r, h11i, vl);
#else
        vint32m1_t a01rq30 = mimo_eq2_conj_dot2_re(
            h00r, h00i, h01r, h01i, h10r, h10i, h11r, h11i, vl);
        vint32m1_t a01iq30 = mimo_eq2_conj_dot2_im(
            h00r, h00i, h01r, h01i, h10r, h10i, h11r, h11i, vl);
#endif
        vint32m1_t a11q30 = mimo_eq2_abs2_dot2(
            h01r, h01i, h11r, h11i, vl);
#if MIMO_USE_RVV_EQ2_3M_BUILD
        vint32m1_t b0rq30;
        vint32m1_t b0iq30;
        vint32m1_t b1rq30;
        vint32m1_t b1iq30;
        MIMO_EQ2_CONJ_DOT2_3M(
            b0rq30, b0iq30,
            h00r, h00i, y0r, y0i,
            h10r, h10i, y1r, y1i, vl);
        MIMO_EQ2_CONJ_DOT2_3M(
            b1rq30, b1iq30,
            h01r, h01i, y0r, y0i,
            h11r, h11i, y1r, y1i, vl);
#else
        vint32m1_t b0rq30 = mimo_eq2_conj_dot2_re(
            h00r, h00i, y0r, y0i, h10r, h10i, y1r, y1i, vl);
        vint32m1_t b0iq30 = mimo_eq2_conj_dot2_im(
            h00r, h00i, y0r, y0i, h10r, h10i, y1r, y1i, vl);
        vint32m1_t b1rq30 = mimo_eq2_conj_dot2_re(
            h01r, h01i, y0r, y0i, h11r, h11i, y1r, y1i, vl);
        vint32m1_t b1iq30 = mimo_eq2_conj_dot2_im(
            h01r, h01i, y0r, y0i, h11r, h11i, y1r, y1i, vl);
#endif

#if MIMO_USE_RVV_EQ2_FUSED_NCLIP_BUILD
        a00 = __riscv_vnclip_wx_i16mf2(
            a00q30, MIMO_Q15_SHIFT, 0, vl);
        a01r = __riscv_vnclip_wx_i16mf2(
            a01rq30, MIMO_Q15_SHIFT, 0, vl);
        a01i = __riscv_vnclip_wx_i16mf2(
            a01iq30, MIMO_Q15_SHIFT, 0, vl);
        a11 = __riscv_vnclip_wx_i16mf2(
            a11q30, MIMO_Q15_SHIFT, 0, vl);
        b0r = __riscv_vnclip_wx_i16mf2(
            b0rq30, MIMO_Q15_SHIFT, 0, vl);
        b0i = __riscv_vnclip_wx_i16mf2(
            b0iq30, MIMO_Q15_SHIFT, 0, vl);
        b1r = __riscv_vnclip_wx_i16mf2(
            b1rq30, MIMO_Q15_SHIFT, 0, vl);
        b1i = __riscv_vnclip_wx_i16mf2(
            b1iq30, MIMO_Q15_SHIFT, 0, vl);
        if (lambda_q15 != 0) {
            a00 = __riscv_vsadd_vx_i16mf2(a00, lambda_q15, vl);
            a11 = __riscv_vsadd_vx_i16mf2(a11, lambda_q15, vl);
        }
#else
        a00q30 = __riscv_vssra_vx_i32m1(
            a00q30, MIMO_Q15_SHIFT, 0, vl);
        a01rq30 = __riscv_vssra_vx_i32m1(
            a01rq30, MIMO_Q15_SHIFT, 0, vl);
        a01iq30 = __riscv_vssra_vx_i32m1(
            a01iq30, MIMO_Q15_SHIFT, 0, vl);
        a11q30 = __riscv_vssra_vx_i32m1(
            a11q30, MIMO_Q15_SHIFT, 0, vl);
        b0rq30 = __riscv_vssra_vx_i32m1(
            b0rq30, MIMO_Q15_SHIFT, 0, vl);
        b0iq30 = __riscv_vssra_vx_i32m1(
            b0iq30, MIMO_Q15_SHIFT, 0, vl);
        b1rq30 = __riscv_vssra_vx_i32m1(
            b1rq30, MIMO_Q15_SHIFT, 0, vl);
        b1iq30 = __riscv_vssra_vx_i32m1(
            b1iq30, MIMO_Q15_SHIFT, 0, vl);
#if MIMO_USE_RVV_EQ2_ALWAYS_ADD_LAMBDA
        asm volatile("" : "+r"(lambda_q15));
        a00q30 = __riscv_vadd_vx_i32m1(a00q30, lambda_q15, vl);
        a11q30 = __riscv_vadd_vx_i32m1(a11q30, lambda_q15, vl);
#else
        if (lambda_q15 != 0) {
            a00q30 = __riscv_vadd_vx_i32m1(a00q30, lambda_q15, vl);
            a11q30 = __riscv_vadd_vx_i32m1(a11q30, lambda_q15, vl);
        }
#endif
        asm volatile(""
                     : "+vr"(a00q30), "+vr"(a01rq30),
                       "+vr"(a01iq30), "+vr"(a11q30),
                       "+vr"(b0rq30), "+vr"(b0iq30),
                       "+vr"(b1rq30), "+vr"(b1iq30));

        a00 = __riscv_vnclip_wx_i16mf2(a00q30, 0, 0, vl);
        a01r = __riscv_vnclip_wx_i16mf2(a01rq30, 0, 0, vl);
        a01i = __riscv_vnclip_wx_i16mf2(a01iq30, 0, 0, vl);
        a11 = __riscv_vnclip_wx_i16mf2(a11q30, 0, 0, vl);
        b0r = __riscv_vnclip_wx_i16mf2(b0rq30, 0, 0, vl);
        b0i = __riscv_vnclip_wx_i16mf2(b0iq30, 0, 0, vl);
        b1r = __riscv_vnclip_wx_i16mf2(b1rq30, 0, 0, vl);
        b1i = __riscv_vnclip_wx_i16mf2(b1iq30, 0, 0, vl);
#endif
#endif
#endif
#if MIMO_USE_RVV_EQ2_EARLY_RECIP_OVERLAP
        vint32m1_t det_q30 = __riscv_vwmul_vv_i32m1(a00, a11, vl);
        const vint16mf2_t neg_a01r = __riscv_vneg_v_i16mf2(a01r, vl);
        const vint16mf2_t neg_a01i = __riscv_vneg_v_i16mf2(a01i, vl);

        vint32m1_t n0r = __riscv_vwmul_vv_i32m1(a11, b0r, vl);
        asm volatile(""
                     : "+vr"(det_q30), "+vr"(n0r));

        asm volatile(
            "vwmacc.vv %[det], %[neg_r], %[a01r]\n\t"
            "vwmacc.vv %[det], %[neg_i], %[a01i]"
            : [det] "+&vr"(det_q30)
            : [neg_r] "vr"(neg_a01r), [a01r] "vr"(a01r),
              [neg_i] "vr"(neg_a01i), [a01i] "vr"(a01i));

        const vint32m1_t det_q15 =
            __riscv_vssra_vx_i32m1(det_q30, MIMO_Q15_SHIFT, 0, vl);
        const vint32m1_t rel = __riscv_vsub_vx_i32m1(
            det_q15, MIMO_EQ2_DET_Q15_RECIP_MIN, vl);
        const vuint32m1_t byte_index = __riscv_vsll_vx_u32m1(
            __riscv_vreinterpret_v_i32m1_u32m1(rel), 2, vl);
        vint32m1_t scale_q30 = __riscv_vluxei32_v_i32m1(
            mimo_eq2_rvv_det_recip_direct1_q30, byte_index, vl);
        asm volatile(""
                     : "+vr"(scale_q30), "+vr"(n0r)
                     : : "memory");

        vint32m1_t n0i = __riscv_vwmul_vv_i32m1(a11, b0i, vl);
        vint32m1_t n1r = __riscv_vwmul_vv_i32m1(a00, b1r, vl);
        vint32m1_t n1i = __riscv_vwmul_vv_i32m1(a00, b1i, vl);
        n0r = __riscv_vwmacc_vv_i32m1(n0r, neg_a01r, b1r, vl);
        n0r = __riscv_vwmacc_vv_i32m1(n0r, a01i, b1i, vl);

        n0i = __riscv_vwmacc_vv_i32m1(n0i, neg_a01r, b1i, vl);
        n0i = __riscv_vwmacc_vv_i32m1(n0i, neg_a01i, b1r, vl);

        n1r = __riscv_vwmacc_vv_i32m1(n1r, neg_a01r, b0r, vl);
        n1r = __riscv_vwmacc_vv_i32m1(n1r, neg_a01i, b0i, vl);

        n1i = __riscv_vwmacc_vv_i32m1(n1i, neg_a01r, b0i, vl);
        n1i = __riscv_vwmacc_vv_i32m1(n1i, a01i, b0r, vl);
        asm volatile(""
                     : "+vr"(n0r), "+vr"(n0i),
                       "+vr"(n1r), "+vr"(n1i));
#else
        vint32m1_t det_q30 = __riscv_vwmul_vv_i32m1(a00, a11, vl);
        const vint16mf2_t neg_a01r = __riscv_vneg_v_i16mf2(a01r, vl);
        const vint16mf2_t neg_a01i = __riscv_vneg_v_i16mf2(a01i, vl);

        det_q30 = __riscv_vwmacc_vv_i32m1(
            det_q30, neg_a01r, a01r, vl);
        det_q30 = __riscv_vwmacc_vv_i32m1(
            det_q30, neg_a01i, a01i, vl);

        vint32m1_t n0r = __riscv_vwmul_vv_i32m1(a11, b0r, vl);
        n0r = __riscv_vwmacc_vv_i32m1(n0r, neg_a01r, b1r, vl);
        n0r = __riscv_vwmacc_vv_i32m1(n0r, a01i, b1i, vl);

        vint32m1_t n0i = __riscv_vwmul_vv_i32m1(a11, b0i, vl);
        n0i = __riscv_vwmacc_vv_i32m1(n0i, neg_a01r, b1i, vl);
        n0i = __riscv_vwmacc_vv_i32m1(n0i, neg_a01i, b1r, vl);

        vint32m1_t n1r = __riscv_vwmul_vv_i32m1(a00, b1r, vl);
        n1r = __riscv_vwmacc_vv_i32m1(n1r, neg_a01r, b0r, vl);
        n1r = __riscv_vwmacc_vv_i32m1(n1r, neg_a01i, b0i, vl);

        vint32m1_t n1i = __riscv_vwmul_vv_i32m1(a00, b1i, vl);
        n1i = __riscv_vwmacc_vv_i32m1(n1i, neg_a01r, b0i, vl);
        n1i = __riscv_vwmacc_vv_i32m1(n1i, a01i, b0r, vl);
        asm volatile(""
                     : "+vr"(det_q30), "+vr"(n0r), "+vr"(n0i),
                       "+vr"(n1r), "+vr"(n1i));

        const vint32m1_t det_q15 =
            __riscv_vssra_vx_i32m1(det_q30, MIMO_Q15_SHIFT, 0, vl);
        const vint32m1_t rel = __riscv_vsub_vx_i32m1(
            det_q15, MIMO_EQ2_DET_Q15_RECIP_MIN, vl);
#if MIMO_USE_RVV_EQ2_UQ12_SCALE
        const vuint32m1_t byte_index = __riscv_vsll_vx_u32m1(
            __riscv_vreinterpret_v_i32m1_u32m1(rel), 1, vl);
        vint16mf2_t x0r16;
        vint16mf2_t x0i16;
        vint16mf2_t x1r16;
        vint16mf2_t x1i16;
        mimo_eq2_scale4_q30_with_uq12(
            &x0r16, &x0i16, &x1r16, &x1i16,
            n0r, n0i, n1r, n1i, byte_index, vl);
#else
        const vuint32m1_t byte_index = __riscv_vsll_vx_u32m1(
            __riscv_vreinterpret_v_i32m1_u32m1(rel), 2, vl);
        const vint32m1_t scale_q30 = __riscv_vluxei32_v_i32m1(
            mimo_eq2_rvv_det_recip_direct1_q30, byte_index, vl);
#endif
#endif

#if MIMO_USE_RVV_EQ2_MMSE_FULL_TRUNC_SCALE && \
    MIMO_USE_RVV_EQ2_RI_SOA_OUTPUT
        int16_t *const x_ri = (int16_t *)(void *)x;
        uintptr_t out_cursor;

        asm volatile(
            "vsetvli zero, %[vl], e32, m2, ta, ma\n\t"
            "vwmul.vv v28, %[x0r], %[scale]\n\t"
            "vnclip.wi %[x0r], v28, 30\n\t"
            "vwmul.vv v28, %[x0i], %[scale]\n\t"
            "vnclip.wi %[x0i], v28, 30\n\t"
            "vwmul.vv v28, %[x1r], %[scale]\n\t"
            "vnclip.wi %[x1r], v28, 30\n\t"
            "vwmul.vv v28, %[x1i], %[scale]\n\t"
            "vnclip.wi %[x1i], v28, 30\n\t"
            "vsetvli zero, %[vl], e16, m1, ta, ma\n\t"
            "vnclip.wi %[x0r], %[x0r], 0\n\t"
            "vnclip.wi %[x0i], %[x0i], 0\n\t"
            "vnclip.wi %[x1r], %[x1r], 0\n\t"
            "vnclip.wi %[x1i], %[x1i], 0\n\t"
            "vse16.v %[x0r], (%[out0])\n\t"
            "sh1add %[out], %[stride], %[out0]\n\t"
            "vse16.v %[x0i], (%[out])\n\t"
            "sh1add %[out], %[stride], %[out]\n\t"
            "vse16.v %[x1r], (%[out])\n\t"
            "sh1add %[out], %[stride], %[out]\n\t"
            "vse16.v %[x1i], (%[out])"
            : [x0r] "+&vr"(n0r), [x0i] "+&vr"(n0i),
              [x1r] "+&vr"(n1r), [x1i] "+&vr"(n1i),
              [out] "=&r"(out_cursor)
            : [scale] "vr"(scale_q30), [vl] "r"(vl),
              [out0] "r"(&x_ri[site]), [stride] "r"(site_count)
            : "v28", "v29", "v30", "v31", "memory");
#else
#if !MIMO_USE_RVV_EQ2_UQ12_SCALE
        vint32m1_t x0r = n0r;
        vint32m1_t x0i = n0i;
        vint32m1_t x1r = n1r;
        vint32m1_t x1i = n1i;
#if MIMO_USE_RVV_EQ2_EARLY_RECIP_OVERLAP && \
    !MIMO_USE_RVV_EQ2_MMSE_FULL_TRUNC_SCALE
        asm volatile("vsetvli zero, %0, e32, m2, ta, ma"
                     : : "r"(vl));
#endif
#if MIMO_USE_RVV_EQ2_MMSE_FULL_TRUNC_SCALE
        asm volatile("vsetvli zero, %0, e32, m2, ta, ma"
                     : : "r"(vl));
        x0r = mimo_eq2_scale_q30_rnu(x0r, scale_q30);
        x0i = mimo_eq2_scale_q30_rnu(x0i, scale_q30);
        x1r = mimo_eq2_scale_q30_rnu(x1r, scale_q30);
        x1i = mimo_eq2_scale_q30_rnu(x1i, scale_q30);
#else
        mimo_eq2_scale4_q30_to_q15(
            &x0r, &x0i, &x1r, &x1i, scale_q30);
#endif

        vint16mf2_t x0r16 = __riscv_vnclip_wx_i16mf2(x0r, 0, 0, vl);
        vint16mf2_t x0i16 = __riscv_vnclip_wx_i16mf2(x0i, 0, 0, vl);
        vint16mf2_t x1r16 = __riscv_vnclip_wx_i16mf2(x1r, 0, 0, vl);
        vint16mf2_t x1i16 = __riscv_vnclip_wx_i16mf2(x1i, 0, 0, vl);
#endif
#if !MIMO_USE_RVV_EQ2_UQ12_SCALE
        asm volatile(""
                     : "+vr"(x0r16), "+vr"(x0i16),
                       "+vr"(x1r16), "+vr"(x1i16));
#endif
#if MIMO_USE_RVV_EQ2_RI_SOA_OUTPUT
        int16_t *const x_ri = (int16_t *)(void *)x;

        __riscv_vse16_v_i16mf2(&x_ri[site], x0r16, vl);
        __riscv_vse16_v_i16mf2(&x_ri[site_count + site], x0i16, vl);
        __riscv_vse16_v_i16mf2(&x_ri[2 * site_count + site], x1r16, vl);
        __riscv_vse16_v_i16mf2(&x_ri[3 * site_count + site], x1i16, vl);
#else
#if MIMO_USE_RVV_EQ2_ZEXT_PACK
        const vint32m1_t x0r32 = __riscv_vreinterpret_v_u32m1_i32m1(
            __riscv_vzext_vf2_u32m1(
                __riscv_vreinterpret_v_i16mf2_u16mf2(x0r16), vl));
        const vint32m1_t x0i32 = __riscv_vreinterpret_v_u32m1_i32m1(
            __riscv_vzext_vf2_u32m1(
                __riscv_vreinterpret_v_i16mf2_u16mf2(x0i16), vl));
        const vint32m1_t x1r32 = __riscv_vreinterpret_v_u32m1_i32m1(
            __riscv_vzext_vf2_u32m1(
                __riscv_vreinterpret_v_i16mf2_u16mf2(x1r16), vl));
        const vint32m1_t x1i32 = __riscv_vreinterpret_v_u32m1_i32m1(
            __riscv_vzext_vf2_u32m1(
                __riscv_vreinterpret_v_i16mf2_u16mf2(x1i16), vl));
#else
        const vint32m1_t x0r32 = __riscv_vwadd_vx_i32m1(x0r16, 0, vl);
        const vint32m1_t x0i32 = __riscv_vwadd_vx_i32m1(x0i16, 0, vl);
        const vint32m1_t x1r32 = __riscv_vwadd_vx_i32m1(x1r16, 0, vl);
        const vint32m1_t x1i32 = __riscv_vwadd_vx_i32m1(x1i16, 0, vl);
#endif
        const vint32m1_t x0_packed = __riscv_vor_vv_i32m1(
#if MIMO_USE_RVV_EQ2_ZEXT_PACK
            x0r32,
#else
            __riscv_vand_vx_i32m1(x0r32, 0xffff, vl),
#endif
            __riscv_vsll_vx_i32m1(x0i32, 16, vl), vl);
        const vint32m1_t x1_packed = __riscv_vor_vv_i32m1(
#if MIMO_USE_RVV_EQ2_ZEXT_PACK
            x1r32,
#else
            __riscv_vand_vx_i32m1(x1r32, 0xffff, vl),
#endif
            __riscv_vsll_vx_i32m1(x1i32, 16, vl), vl);

#if MIMO_USE_RVV_EQ2_TX_SOA_STORE
        __riscv_vse32_v_i32m1((int32_t *)&x[site], x0_packed, vl);
        __riscv_vse32_v_i32m1(
            (int32_t *)&x[MIMO_EQ2_SITE_COUNT + site], x1_packed, vl);
#else
        __riscv_vsse32_v_i32m1((int32_t *)&x[site * MIMO_N2],
                                sizeof(mimo_c16_t) * MIMO_N2,
                                x0_packed, vl);
        __riscv_vsse32_v_i32m1((int32_t *)&x[site * MIMO_N2 + 1],
                                sizeof(mimo_c16_t) * MIMO_N2,
                                x1_packed, vl);
#endif
#endif
#endif
#if MIMO_USE_RVV_EQ2_BLOCK_SOA
        h_block += MIMO_EQ2_H_RI_COMPONENTS * MIMO_EQ2_BLOCK_SOA_LANES;
        y_block += MIMO_EQ2_Y_RI_COMPONENTS * MIMO_EQ2_BLOCK_SOA_LANES;
#endif
        site += vl;
    }
}

void mimo_eq2_zf_rvv_ri_soa_q15(size_t site_count,
                                const int16_t *restrict h_ri_soa,
                                const int16_t *restrict y_ri_soa,
                                mimo_c16_t *restrict x)
{
#if MIMO_USE_EQ2_ZF_DIRECT_H
    mimo_eq2_rvv_direct_h_loop(site_count, h_ri_soa, y_ri_soa, x);
#else
    mimo_eq2_rvv_ri_soa_loop(site_count, h_ri_soa, y_ri_soa, 0, x);
#endif
}

void mimo_eq2_mmse_rvv_ri_soa_q15(size_t site_count,
                                  const int16_t *restrict h_ri_soa,
                                  const int16_t *restrict y_ri_soa,
                                  mimo_c16_t *restrict x)
{
    mimo_eq2_rvv_ri_soa_loop(site_count, h_ri_soa, y_ri_soa,
                              MIMO_MMSE_LAMBDA_Q15, x);
}

#undef MIMO_EQ2_DET_Q15_RECIP_MIN
#undef MIMO_EQ2_RVV_NARROW_Q15
#if MIMO_USE_RVV_EQ2_3M_BUILD
#undef MIMO_EQ2_CONJ_DOT2_3M
#endif

#if MIMO_USE_RVV_EQ2_M1
#undef __riscv_vsll_vx_i16mf2
#undef __riscv_vreinterpret_v_u16mf2_i16mf2
#undef __riscv_vluxei32_v_u16mf2
#undef __riscv_vse16_v_i16mf2
#undef __riscv_vsse32_v_i32m1
#undef __riscv_vse32_v_i32m1
#undef __riscv_vor_vv_i32m1
#undef __riscv_vsll_vx_i32m1
#undef __riscv_vand_vx_i32m1
#undef __riscv_vreinterpret_v_u32m1_i32m1
#undef __riscv_vzext_vf2_u32m1
#undef __riscv_vreinterpret_v_i16mf2_u16mf2
#undef __riscv_vwadd_vx_i32m1
#undef __riscv_vluxei32_v_i32m1
#undef __riscv_vsll_vx_u32m1
#undef __riscv_vreinterpret_v_i32m1_u32m1
#undef __riscv_vsub_vx_i32m1
#undef __riscv_vmul_vv_i32m1
#undef __riscv_vsra_vx_i32m1
#undef __riscv_vnclip_wx_i16mf2
#undef __riscv_vadd_vx_i32m1
#undef __riscv_vsub_vv_i32m1
#undef __riscv_vadd_vv_i32m1
#undef __riscv_vssra_vx_i32m1
#undef __riscv_vsadd_vx_i16mf2
#undef __riscv_vneg_v_i16mf2
#undef __riscv_vwmacc_vv_i32m1
#undef __riscv_vwmul_vv_i32m1
#undef __riscv_vsub_vv_i16mf2
#undef __riscv_vadd_vv_i16mf2
#undef __riscv_vle16_v_i16mf2
#undef __riscv_vsetvl_e16mf2
#undef vuint32m1_t
#undef vint32m1_t
#undef vuint16mf2_t
#undef vint16mf2_t
#endif
#endif
