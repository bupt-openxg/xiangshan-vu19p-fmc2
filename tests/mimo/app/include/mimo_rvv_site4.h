#ifndef MIMO_RVV_SITE4_H
#define MIMO_RVV_SITE4_H

#include <riscv_vector.h>

#include "mimo_kernels.h"
#include "mimo_rvv_solve4.h"

#ifndef MIMO_USE_RVV_SITE4_MF2
#define MIMO_USE_RVV_SITE4_MF2 0
#endif

#ifndef MIMO_USE_RVV_ZF4_FUSED_BUILD_SOLVE
#define MIMO_USE_RVV_ZF4_FUSED_BUILD_SOLVE 0
#endif

#ifndef MIMO_USE_RVV_ZF4_STAGED_BUILD_SOLVE
#define MIMO_USE_RVV_ZF4_STAGED_BUILD_SOLVE 0
#endif

#ifndef MIMO_USE_RVV_ZF4_SEED_ACC
#define MIMO_USE_RVV_ZF4_SEED_ACC 0
#endif

#ifndef MIMO_USE_RVV_MMSE4_STAGED_BUILD_SOLVE
#define MIMO_USE_RVV_MMSE4_STAGED_BUILD_SOLVE 0
#endif

#ifndef MIMO_USE_RVV_MMSE4_EARLY_TAIL_RHS
#define MIMO_USE_RVV_MMSE4_EARLY_TAIL_RHS 0
#endif

#ifndef MIMO_USE_RVV_MMSE4_CONST_VL
#define MIMO_USE_RVV_MMSE4_CONST_VL 0
#endif

#ifndef MIMO_USE_RVV_MMSE4_SEED_ACC
#define MIMO_USE_RVV_MMSE4_SEED_ACC 0
#endif

#ifndef MIMO_USE_RVV_MMSE4_RI_PACKED_LOAD
#define MIMO_USE_RVV_MMSE4_RI_PACKED_LOAD 0
#endif

#ifndef MIMO_USE_RVV_MMSE4_BINV_Q13_REUSE
#define MIMO_USE_RVV_MMSE4_BINV_Q13_REUSE 0
#endif

#ifndef MIMO_USE_RVV_ZF4_Q15ACC_BUILD
#define MIMO_USE_RVV_ZF4_Q15ACC_BUILD 0
#endif

#ifndef MIMO_USE_SITE4_DIAG_REAL_BUILD
#define MIMO_USE_SITE4_DIAG_REAL_BUILD 0
#endif

#ifndef MIMO_USE_SITE4_HERMITIAN_UPPER_BUILD
#define MIMO_USE_SITE4_HERMITIAN_UPPER_BUILD 0
#endif

#ifndef MIMO_USE_SITE4_HERMITIAN_UPPER_MMSE
#define MIMO_USE_SITE4_HERMITIAN_UPPER_MMSE 0
#endif

#ifndef MIMO_USE_SITE4_HERMITIAN_UPPER_ROWFUSE
#define MIMO_USE_SITE4_HERMITIAN_UPPER_ROWFUSE 0
#endif

#ifndef MIMO_USE_SITE4_HERMITIAN_UPPER_ROWPAIR
#define MIMO_USE_SITE4_HERMITIAN_UPPER_ROWPAIR 0
#endif

#ifndef MIMO_USE_SITE4_ROUND_ONCE_DOT
#define MIMO_USE_SITE4_ROUND_ONCE_DOT 0
#endif

#ifndef MIMO_USE_RVV_SITE4_VSSRA_ROUND
#define MIMO_USE_RVV_SITE4_VSSRA_ROUND 0
#endif

#ifndef MIMO_USE_RVV_SITE4_SEG2_LOAD
#define MIMO_USE_RVV_SITE4_SEG2_LOAD 0
#endif

#ifndef MIMO_USE_RVV_SITE4_RI_SOA
#define MIMO_USE_RVV_SITE4_RI_SOA 0
#endif

#if MIMO_USE_RVV_SITE4_VSSRA_ROUND && !MIMO_USE_SITE4_ROUND_ONCE_DOT
#error "MIMO_USE_RVV_SITE4_VSSRA_ROUND requires MIMO_USE_SITE4_ROUND_ONCE_DOT=1"
#endif

#if MIMO_USE_RVV_SITE4_VSSRA_ROUND < 0 || \
    MIMO_USE_RVV_SITE4_VSSRA_ROUND > 2
#error "MIMO_USE_RVV_SITE4_VSSRA_ROUND must be 0, 1, or 2"
#endif

#if MIMO_USE_RVV_SITE4_VSSRA_ROUND == 2 && \
    (!MIMO_USE_RVV_MMSE4_STAGED_BUILD_SOLVE || \
     MIMO_USE_RVV_MMSE4_EARLY_TAIL_RHS)
#error "MIMO_USE_RVV_SITE4_VSSRA_ROUND=2 requires MMSE4 staged build and no early-tail RHS"
#endif

#if MIMO_USE_RVV_MMSE4_BINV_Q13_REUSE && \
    (!MIMO_USE_RVV_MMSE4_STAGED_BUILD_SOLVE || \
     MIMO_USE_RVV_ZF4_STAGED_BUILD_SOLVE || \
     MIMO_USE_RVV_MMSE4_EARLY_TAIL_RHS)
#error "MMSE4 RVV Q13 B-inverse reuse requires the MMSE-only staged path"
#endif

#if MIMO_USE_RVV_SITE4_SEG2_LOAD && \
    (!MIMO_USE_RVV_SITE4_MF2 || !MIMO_USE_SITE4_HERMITIAN_UPPER_ROWFUSE || \
     MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED)
#error "MIMO_USE_RVV_SITE4_SEG2_LOAD requires pure RVV MF2 unpacked rowfuse"
#endif

#if MIMO_USE_RVV_SITE4_RI_SOA && \
    (!MIMO_USE_RVV_SITE4_MF2 || !MIMO_USE_RVV_ZF4_FUSED_BUILD_SOLVE || \
     !MIMO_USE_SITE4_HERMITIAN_UPPER_ROWFUSE || \
     MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED || \
     MIMO_USE_RVV_SITE4_SEG2_LOAD)
#error "MIMO_USE_RVV_SITE4_RI_SOA requires the pure RVV MF2 fused unpacked rowfuse path"
#endif

#if MIMO_USE_SITE4_ROUND_ONCE_DOT && \
    (!MIMO_USE_RVV_SITE4_MF2 || MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED || \
     (!MIMO_USE_SITE4_HERMITIAN_UPPER_ROWFUSE && \
      !MIMO_USE_SITE4_HERMITIAN_UPPER_ROWPAIR))
#error "MIMO_USE_SITE4_ROUND_ONCE_DOT is a local RVV MF2 upper-rowfuse/rowpair experiment"
#endif

#if MIMO_USE_SITE4_HERMITIAN_UPPER_ROWPAIR && \
    (!MIMO_USE_RVV_SITE4_MF2 || MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED || \
     MIMO_COLLECT_SCHUR_STATS)
#error "MIMO_USE_SITE4_HERMITIAN_UPPER_ROWPAIR requires pure RVV MF2 unpacked upper-build without Schur stats"
#endif

#if MIMO_USE_SITE4_HERMITIAN_UPPER_ROWPAIR && \
    MIMO_USE_SITE4_HERMITIAN_UPPER_ROWFUSE
#error "MIMO_USE_SITE4_HERMITIAN_UPPER_ROWPAIR and ROWFUSE are mutually exclusive"
#endif

#if MIMO_USE_RVV_ZF4_FUSED_BUILD_SOLVE && \
    (!MIMO_USE_RVV_SITE4_MF2 || \
     (!MIMO_USE_RVV_ZF4_SCHUR_RVV_SOLVE && \
      !MIMO_USE_RVV_MMSE4_SCHUR_RVV_SOLVE) || \
     !MIMO_USE_SITE4_SOLVE_SOA || !MIMO_USE_SITE4_HERMITIAN_UPPER_BUILD || \
     !MIMO_USE_SITE4_HERMITIAN_UPPER_ROWFUSE || \
     !MIMO_USE_SITE4_ROUND_ONCE_DOT || \
     MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED || MIMO_COLLECT_SCHUR_STATS)
#error "MIMO_USE_RVV_ZF4_FUSED_BUILD_SOLVE requires pure RVV MF2 unpacked upper-rowfuse round-once Schur RVV solve"
#endif

#if MIMO_USE_RVV_ZF4_STAGED_BUILD_SOLVE && !MIMO_USE_RVV_ZF4_FUSED_BUILD_SOLVE
#error "MIMO_USE_RVV_ZF4_STAGED_BUILD_SOLVE requires MIMO_USE_RVV_ZF4_FUSED_BUILD_SOLVE=1"
#endif

#if MIMO_USE_RVV_ZF4_SEED_ACC && !MIMO_USE_RVV_ZF4_FUSED_BUILD_SOLVE
#error "MIMO_USE_RVV_ZF4_SEED_ACC requires MIMO_USE_RVV_ZF4_FUSED_BUILD_SOLVE=1"
#endif

#if MIMO_USE_RVV_ZF4_Q15ACC_BUILD && \
    (!MIMO_USE_RVV_SITE4_MF2 || !MIMO_USE_SITE4_SOLVE_SOA || \
     !MIMO_USE_SITE4_HERMITIAN_UPPER_BUILD || \
     !MIMO_USE_SITE4_HERMITIAN_UPPER_ROWFUSE || \
     MIMO_USE_SITE4_ROUND_ONCE_DOT || \
     MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED || MIMO_COLLECT_SCHUR_STATS)
#error "MIMO_USE_RVV_ZF4_Q15ACC_BUILD requires pure RVV MF2 unpacked upper-rowfuse without round-once"
#endif

#if MIMO_USE_RVV_MMSE4_EARLY_TAIL_RHS && \
    !MIMO_USE_RVV_MMSE4_STAGED_BUILD_SOLVE
#error "MIMO_USE_RVV_MMSE4_EARLY_TAIL_RHS requires MIMO_USE_RVV_MMSE4_STAGED_BUILD_SOLVE=1"
#endif

#if MIMO_USE_RVV_MMSE4_CONST_VL && !MIMO_USE_RVV_MMSE4_STAGED_BUILD_SOLVE
#error "MIMO_USE_RVV_MMSE4_CONST_VL requires MIMO_USE_RVV_MMSE4_STAGED_BUILD_SOLVE=1"
#endif

#if MIMO_USE_RVV_MMSE4_SEED_ACC && !MIMO_USE_RVV_MMSE4_STAGED_BUILD_SOLVE
#error "MIMO_USE_RVV_MMSE4_SEED_ACC requires MIMO_USE_RVV_MMSE4_STAGED_BUILD_SOLVE=1"
#endif

#if MIMO_USE_RVV_MMSE4_RI_PACKED_LOAD && \
    (!MIMO_USE_RVV_MMSE4_STAGED_BUILD_SOLVE || \
     !MIMO_USE_RVV_SITE4_RI_SOA || MIMO_USE_RVV_SITE4_SEG2_LOAD)
#error "MIMO_USE_RVV_MMSE4_RI_PACKED_LOAD requires staged RI-SoA without segment load"
#endif

#if MIMO_USE_RVV_MMSE4_RI_PACKED_LOAD == 2 && \
    (!MIMO_USE_RVV_MMSE4_SEED_ACC || MIMO_USE_RVV_MMSE4_EARLY_TAIL_RHS)
#error "MIMO_USE_RVV_MMSE4_RI_PACKED_LOAD=2 requires seed-acc and no early-tail RHS"
#endif

#if MIMO_USE_RVV_MMSE4_RI_PACKED_LOAD == 3 && \
    (!MIMO_USE_RVV_MMSE4_SEED_ACC || MIMO_USE_RVV_MMSE4_EARLY_TAIL_RHS)
#error "MIMO_USE_RVV_MMSE4_RI_PACKED_LOAD=3 requires seed-acc and no early-tail RHS"
#endif

#if MIMO_USE_RVV_MMSE4_RI_PACKED_LOAD >= 4 && \
    (!MIMO_USE_RVV_MMSE4_SEED_ACC || MIMO_USE_RVV_MMSE4_EARLY_TAIL_RHS)
#error "MIMO_USE_RVV_MMSE4_RI_PACKED_LOAD modes 4-7 require seed-acc and no early-tail RHS"
#endif

#if MIMO_USE_RVV_MMSE4_RI_PACKED_LOAD > 7
#error "MIMO_USE_RVV_MMSE4_RI_PACKED_LOAD must be in [0,7]"
#endif

#if MIMO_USE_SITE4_HERMITIAN_UPPER_MMSE && \
    (!MIMO_USE_SITE4_SOLVE_SOA || !MIMO_USE_SOLVE4_SCHUR || \
     !MIMO_USE_SITE4_HERMITIAN_UPPER_BUILD)
#error "MIMO_USE_SITE4_HERMITIAN_UPPER_MMSE requires RVV SoA Schur upper-build path"
#endif

static MIMO_ALWAYS_INLINE const mimo_c16_t *mimo_site4_h4_vec(
    const mimo_c16_t *h_site4,
    size_t block,
    int tx,
    int rx)
{
    const size_t idx = (((block * MIMO_N4 + (size_t)tx) * MIMO_N4 +
                         (size_t)rx) *
                        MIMO_SITE4_LANES);
    return &h_site4[idx];
}

static MIMO_ALWAYS_INLINE const mimo_c16_t *mimo_site4_y4_vec(
    const mimo_c16_t *y_site4,
    size_t block,
    int rx)
{
    const size_t idx =
        (block * MIMO_N4 + (size_t)rx) * MIMO_SITE4_LANES;
    return &y_site4[idx];
}

#if MIMO_USE_RVV_SITE4_MF2
static MIMO_ALWAYS_INLINE vint16mf2_t mimo_rvv_site4_load_re_mf2(
    const mimo_c16_t *src,
    size_t vl)
{
#if MIMO_USE_RVV_SITE4_RI_SOA
    return __riscv_vle16_v_i16mf2((const int16_t *)src, vl);
#else
    return __riscv_vlse16_v_i16mf2(&src[0].r, sizeof(mimo_c16_t), vl);
#endif
}

static MIMO_ALWAYS_INLINE vint16mf2_t mimo_rvv_site4_load_im_mf2(
    const mimo_c16_t *src,
    size_t vl)
{
#if MIMO_USE_RVV_SITE4_RI_SOA
    return __riscv_vle16_v_i16mf2(
        (const int16_t *)src + MIMO_SITE4_LANES, vl);
#else
    return __riscv_vlse16_v_i16mf2(&src[0].i, sizeof(mimo_c16_t), vl);
#endif
}

#if MIMO_USE_RVV_MMSE4_RI_PACKED_LOAD == 1
#define MIMO_RVV_STAGED_LOAD_RI_MF2(re_name, im_name, src, vl)              \
    const vint16m1_t re_name##_packed =                                   \
        __riscv_vle16_v_i16m1((const int16_t *)(src), 2 * (vl));           \
    const vint16mf2_t re_name =                                            \
        __riscv_vlmul_trunc_v_i16m1_i16mf2(re_name##_packed);              \
    const vint16m1_t im_name##_low = __riscv_vslidedown_vx_i16m1(          \
        re_name##_packed, (vl), 2 * (vl));                                 \
    const vint16mf2_t im_name =                                            \
        __riscv_vlmul_trunc_v_i16m1_i16mf2(im_name##_low)
#else
#define MIMO_RVV_STAGED_LOAD_RI_MF2(re_name, im_name, src, vl)              \
    const vint16mf2_t re_name = mimo_rvv_site4_load_re_mf2((src), (vl));   \
    const vint16mf2_t im_name = mimo_rvv_site4_load_im_mf2((src), (vl))
#endif

#if MIMO_USE_RVV_MMSE4_RI_PACKED_LOAD == 3
static MIMO_ALWAYS_INLINE void mimo_rvv_mmse4_load_head_ri_packed_mf2(
    const mimo_c16_t *h0,
    const mimo_c16_t *h1,
    const mimo_c16_t *h2,
    const mimo_c16_t *h3,
    const mimo_c16_t *y,
    vint16mf2_t *h0r_o,
    vint16mf2_t *h0i_o,
    vint16mf2_t *h1r_o,
    vint16mf2_t *h1i_o,
    vint16mf2_t *h2r_o,
    vint16mf2_t *h2i_o,
    vint16mf2_t *h3r_o,
    vint16mf2_t *h3i_o,
    vint16mf2_t *yr_o,
    vint16mf2_t *yi_o)
{
    vint16mf2_t h0r, h0i, h1r, h1i, h2r, h2i, h3r, h3i, yr, yi;

    asm volatile(
        "vsetivli zero, 8, e16, m1, ta, ma\n\t"
        "vle16.v %[h0r], (%[h0])\n\t"
        "vle16.v %[h1r], (%[h1])\n\t"
        "vle16.v %[h2r], (%[h2])\n\t"
        "vle16.v %[h3r], (%[h3])\n\t"
        "vle16.v %[yr], (%[y])\n\t"
        "vslidedown.vi %[h0i], %[h0r], 4\n\t"
        "vslidedown.vi %[h1i], %[h1r], 4\n\t"
        "vslidedown.vi %[h2i], %[h2r], 4\n\t"
        "vslidedown.vi %[h3i], %[h3r], 4\n\t"
        "vslidedown.vi %[yi], %[yr], 4\n\t"
        : [h0r] "=&vr"(h0r), [h0i] "=&vr"(h0i),
          [h1r] "=&vr"(h1r), [h1i] "=&vr"(h1i),
          [h2r] "=&vr"(h2r), [h2i] "=&vr"(h2i),
          [h3r] "=&vr"(h3r), [h3i] "=&vr"(h3i),
          [yr] "=&vr"(yr), [yi] "=&vr"(yi)
        : [h0] "r"(h0), [h1] "r"(h1), [h2] "r"(h2),
          [h3] "r"(h3), [y] "r"(y)
        : "memory");

    *h0r_o = h0r;
    *h0i_o = h0i;
    *h1r_o = h1r;
    *h1i_o = h1i;
    *h2r_o = h2r;
    *h2i_o = h2i;
    *h3r_o = h3r;
    *h3i_o = h3i;
    *yr_o = yr;
    *yi_o = yi;
}

#define MIMO_RVV_STAGED_LOAD_RI5_MF2(                                     \
    h0r, h0i, h1r, h1i, h2r, h2i, h3r, h3i, yr, yi,                       \
    h0, h1, h2, h3, y)                                                     \
    vint16mf2_t h0r, h0i, h1r, h1i, h2r, h2i, h3r, h3i, yr, yi;           \
    mimo_rvv_mmse4_load_head_ri_packed_mf2(                                \
        (h0), (h1), (h2), (h3), (y),                                      \
        &(h0r), &(h0i), &(h1r), &(h1i), &(h2r), &(h2i),                    \
        &(h3r), &(h3i), &(yr), &(yi))
#else
#define MIMO_RVV_STAGED_LOAD_RI5_MF2(                                     \
    h0r, h0i, h1r, h1i, h2r, h2i, h3r, h3i, yr, yi,                       \
    h0, h1, h2, h3, y)                                                     \
    MIMO_RVV_STAGED_LOAD_RI_MF2(h0r, h0i, (h0), vl);                      \
    MIMO_RVV_STAGED_LOAD_RI_MF2(h1r, h1i, (h1), vl);                      \
    MIMO_RVV_STAGED_LOAD_RI_MF2(h2r, h2i, (h2), vl);                      \
    MIMO_RVV_STAGED_LOAD_RI_MF2(h3r, h3i, (h3), vl);                      \
    MIMO_RVV_STAGED_LOAD_RI_MF2(yr, yi, (y), vl)
#endif

#if MIMO_USE_RVV_MMSE4_RI_PACKED_LOAD >= 4
static MIMO_ALWAYS_INLINE void mimo_rvv_mmse4_build_head_ri_packed_m2_asm(
    const mimo_c16_t *h0,
    const mimo_c16_t *h1,
    const mimo_c16_t *h2,
    const mimo_c16_t *h3,
    const mimo_c16_t *y,
    vint32m1_t *b0r_o,
    vint32m1_t *b0i_o,
    vint32m1_t *b1r_o,
    vint32m1_t *b1i_o,
    vint32m1_t *a00r_o,
    vint32m1_t *a01r_o,
    vint32m1_t *a01i_o,
    vint32m1_t *a02r_o,
    vint32m1_t *a02i_o,
    vint32m1_t *a03r_o,
    vint32m1_t *a03i_o,
    vint32m1_t *a11r_o,
    vint32m1_t *a12r_o,
    vint32m1_t *a12i_o,
    vint32m1_t *a13r_o,
    vint32m1_t *a13i_o)
{
    const int16_t *p0 = (const int16_t *)h0;
    const int16_t *p1 = (const int16_t *)h1;
    const int16_t *p2 = (const int16_t *)h2;
    const int16_t *p3 = (const int16_t *)h3;
    const int16_t *py = (const int16_t *)y;
    register vint32m1_t a13r asm("v16");
    register vint32m1_t a13i asm("v17");
    register vint32m1_t a12r asm("v18");
    register vint32m1_t a12i asm("v19");
    register vint32m1_t b0r asm("v20");
    register vint32m1_t b0i asm("v21");
    register vint32m1_t a00r asm("v22");
    register vint32m1_t a01r asm("v23");
    register vint32m1_t a01i asm("v24");
    register vint32m1_t a02r asm("v25");
    register vint32m1_t a02i asm("v26");
    register vint32m1_t a03r asm("v27");
    register vint32m1_t a03i asm("v28");
    register vint32m1_t b1r asm("v29");
    register vint32m1_t b1i asm("v30");
    register vint32m1_t a11r asm("v31");

    asm volatile(
        /* RX0/RX1: five m2 loads, then seed and accumulate. */
        "vsetivli zero, 16, e16, m2, ta, ma\n\t"
        "vle16.v v0, (%[p0])\n\t"
        "vle16.v v2, (%[p1])\n\t"
        "vle16.v v4, (%[p2])\n\t"
        "vle16.v v6, (%[p3])\n\t"
        "vle16.v v8, (%[py])\n\t"
        "vslidedown.vi v10, v0, 4\n\t"
        "vslidedown.vi v12, v2, 4\n\t"
        "vslidedown.vi v16, v4, 4\n\t"
        "vslidedown.vi v14, v6, 4\n\t"
        "vslidedown.vi v18, v8, 4\n\t"
        "vsetivli zero, 4, e16, mf2, ta, ma\n\t"

        "vwmul.vv %[b0r], v0, v8\n\t"
        "vwmacc.vv %[b0r], v10, v18\n\t"
        "vneg.v %[a11r], v10\n\t"
        "vwmul.vv %[b0i], v0, v18\n\t"
        "vwmacc.vv %[b0i], %[a11r], v8\n\t"
        "vwmacc.vv %[b0r], v1, v9\n\t"
        "vwmacc.vv %[b0r], v11, v19\n\t"
        "vneg.v %[a11r], v11\n\t"
        "vwmacc.vv %[b0i], v1, v19\n\t"
        "vwmacc.vv %[b0i], %[a11r], v9\n\t"

        "vwmul.vv %[a00r], v0, v0\n\t"
        "vwmacc.vv %[a00r], v10, v10\n\t"
        "vwmacc.vv %[a00r], v1, v1\n\t"
        "vwmacc.vv %[a00r], v11, v11\n\t"

        "vwmul.vv %[a01r], v0, v2\n\t"
        "vwmacc.vv %[a01r], v10, v12\n\t"
        "vneg.v %[a11r], v10\n\t"
        "vwmul.vv %[a01i], v0, v12\n\t"
        "vwmacc.vv %[a01i], %[a11r], v2\n\t"
        "vwmacc.vv %[a01r], v1, v3\n\t"
        "vwmacc.vv %[a01r], v11, v13\n\t"
        "vneg.v %[a11r], v11\n\t"
        "vwmacc.vv %[a01i], v1, v13\n\t"
        "vwmacc.vv %[a01i], %[a11r], v3\n\t"

        "vwmul.vv %[a02r], v0, v4\n\t"
        "vwmacc.vv %[a02r], v10, v16\n\t"
        "vneg.v %[a11r], v10\n\t"
        "vwmul.vv %[a02i], v0, v16\n\t"
        "vwmacc.vv %[a02i], %[a11r], v4\n\t"
        "vwmacc.vv %[a02r], v1, v5\n\t"
        "vwmacc.vv %[a02r], v11, v17\n\t"
        "vneg.v %[a11r], v11\n\t"
        "vwmacc.vv %[a02i], v1, v17\n\t"
        "vwmacc.vv %[a02i], %[a11r], v5\n\t"

        "vwmul.vv %[a03r], v0, v6\n\t"
        "vwmacc.vv %[a03r], v10, v14\n\t"
        "vneg.v %[a11r], v10\n\t"
        "vwmul.vv %[a03i], v0, v14\n\t"
        "vwmacc.vv %[a03i], %[a11r], v6\n\t"
        "vwmacc.vv %[a03r], v1, v7\n\t"
        "vwmacc.vv %[a03r], v11, v15\n\t"
        "vneg.v %[a11r], v11\n\t"
        "vwmacc.vv %[a03i], v1, v15\n\t"
        "vwmacc.vv %[a03i], %[a11r], v7\n\t"

        "vwmul.vv %[b1r], v2, v8\n\t"
        "vwmacc.vv %[b1r], v12, v18\n\t"
        "vneg.v %[a11r], v12\n\t"
        "vwmul.vv %[b1i], v2, v18\n\t"
        "vwmacc.vv %[b1i], %[a11r], v8\n\t"
        "vwmacc.vv %[b1r], v3, v9\n\t"
        "vwmacc.vv %[b1r], v13, v19\n\t"
        "vneg.v %[a11r], v13\n\t"
        "vwmacc.vv %[b1i], v3, v19\n\t"
        "vwmacc.vv %[b1i], %[a11r], v9\n\t"

        "vwmul.vv %[a12r], v2, v4\n\t"
        "vwmacc.vv %[a12r], v12, v16\n\t"
        "vneg.v %[a11r], v12\n\t"
        "vwmul.vv %[a12i], v2, v16\n\t"
        "vwmacc.vv %[a12i], %[a11r], v4\n\t"
        "vwmacc.vv %[a12r], v3, v5\n\t"
        "vwmacc.vv %[a12r], v13, v17\n\t"
        "vneg.v %[a11r], v13\n\t"
        "vwmacc.vv %[a12i], v3, v17\n\t"
        "vwmacc.vv %[a12i], %[a11r], v5\n\t"

        "vwmul.vv %[a13r], v2, v6\n\t"
        "vwmacc.vv %[a13r], v12, v14\n\t"
        "vneg.v %[a11r], v12\n\t"
        "vwmul.vv %[a13i], v2, v14\n\t"
        "vwmacc.vv %[a13i], %[a11r], v6\n\t"
        "vwmacc.vv %[a13r], v3, v7\n\t"
        "vwmacc.vv %[a13r], v13, v15\n\t"
        "vneg.v %[a11r], v13\n\t"
        "vwmacc.vv %[a13i], v3, v15\n\t"
        "vwmacc.vv %[a13i], %[a11r], v7\n\t"

        "vwmul.vv %[a11r], v2, v2\n\t"
        "vwmacc.vv %[a11r], v12, v12\n\t"
        "vwmacc.vv %[a11r], v3, v3\n\t"
        "vwmacc.vv %[a11r], v13, v13\n\t"

        /* RX2/RX3: keep all accumulators live and reuse three temp groups. */
        "addi %[p0], %[p0], 32\n\t"
        "addi %[p1], %[p1], 32\n\t"
        "addi %[p2], %[p2], 32\n\t"
        "addi %[p3], %[p3], 32\n\t"
        "addi %[py], %[py], 32\n\t"
        "vsetivli zero, 16, e16, m2, ta, ma\n\t"
        "vle16.v v0, (%[p0])\n\t"
        "vle16.v v2, (%[p1])\n\t"
        "vle16.v v4, (%[p2])\n\t"
        "vle16.v v6, (%[p3])\n\t"
        "vle16.v v8, (%[py])\n\t"
        "vslidedown.vi v10, v0, 4\n\t"
        "vslidedown.vi v12, v2, 4\n\t"
        "vslidedown.vi v14, v8, 4\n\t"
        "vsetivli zero, 4, e16, mf2, ta, ma\n\t"

        "vwmacc.vv %[b0r], v0, v8\n\t"
        "vwmacc.vv %[b0r], v10, v14\n\t"
        "vwmacc.vv %[b0i], v0, v14\n\t"
        "vwmacc.vv %[b0r], v1, v9\n\t"
        "vwmacc.vv %[b0r], v11, v15\n\t"
        "vwmacc.vv %[b0i], v1, v15\n\t"
        "vwmacc.vv %[b1r], v2, v8\n\t"
        "vwmacc.vv %[b1r], v12, v14\n\t"
        "vwmacc.vv %[b1i], v2, v14\n\t"
        "vwmacc.vv %[b1r], v3, v9\n\t"
        "vwmacc.vv %[b1r], v13, v15\n\t"
        "vwmacc.vv %[b1i], v3, v15\n\t"
        "vwmacc.vv %[a00r], v0, v0\n\t"
        "vwmacc.vv %[a00r], v10, v10\n\t"
        "vwmacc.vv %[a00r], v1, v1\n\t"
        "vwmacc.vv %[a00r], v11, v11\n\t"
        "vwmacc.vv %[a01r], v0, v2\n\t"
        "vwmacc.vv %[a01r], v10, v12\n\t"
        "vwmacc.vv %[a01i], v0, v12\n\t"
        "vwmacc.vv %[a01r], v1, v3\n\t"
        "vwmacc.vv %[a01r], v11, v13\n\t"
        "vwmacc.vv %[a01i], v1, v13\n\t"
        "vwmacc.vv %[a11r], v2, v2\n\t"
        "vwmacc.vv %[a11r], v12, v12\n\t"
        "vwmacc.vv %[a11r], v3, v3\n\t"
        "vwmacc.vv %[a11r], v13, v13\n\t"

        "vsetivli zero, 16, e16, m2, ta, ma\n\t"
        "vslidedown.vi v14, v4, 4\n\t"
        "vsetivli zero, 4, e16, mf2, ta, ma\n\t"
        "vwmacc.vv %[a02r], v0, v4\n\t"
        "vwmacc.vv %[a02r], v10, v14\n\t"
        "vwmacc.vv %[a02i], v0, v14\n\t"
        "vwmacc.vv %[a02r], v1, v5\n\t"
        "vwmacc.vv %[a02r], v11, v15\n\t"
        "vwmacc.vv %[a02i], v1, v15\n\t"
        "vwmacc.vv %[a12r], v2, v4\n\t"
        "vwmacc.vv %[a12r], v12, v14\n\t"
        "vwmacc.vv %[a12i], v2, v14\n\t"
        "vwmacc.vv %[a12r], v3, v5\n\t"
        "vwmacc.vv %[a12r], v13, v15\n\t"
        "vwmacc.vv %[a12i], v3, v15\n\t"

        "vsetivli zero, 16, e16, m2, ta, ma\n\t"
        "vslidedown.vi v14, v6, 4\n\t"
        "vsetivli zero, 4, e16, mf2, ta, ma\n\t"
        "vwmacc.vv %[a03r], v0, v6\n\t"
        "vwmacc.vv %[a03r], v10, v14\n\t"
        "vwmacc.vv %[a03i], v0, v14\n\t"
        "vwmacc.vv %[a03r], v1, v7\n\t"
        "vwmacc.vv %[a03r], v11, v15\n\t"
        "vwmacc.vv %[a03i], v1, v15\n\t"
        "vwmacc.vv %[a13r], v2, v6\n\t"
        "vwmacc.vv %[a13r], v12, v14\n\t"
        "vwmacc.vv %[a13i], v2, v14\n\t"
        "vwmacc.vv %[a13r], v3, v7\n\t"
        "vwmacc.vv %[a13r], v13, v15\n\t"
        "vwmacc.vv %[a13i], v3, v15\n\t"

        /* Finish delayed -ai*br terms after all real sources are consumed. */
        "vneg.v v0, v10\n\t"
        "vneg.v v1, v11\n\t"
        "vwmacc.vv %[b0i], v0, v8\n\t"
        "vwmacc.vv %[b0i], v1, v9\n\t"
        "vwmacc.vv %[a01i], v0, v2\n\t"
        "vwmacc.vv %[a01i], v1, v3\n\t"
        "vwmacc.vv %[a02i], v0, v4\n\t"
        "vwmacc.vv %[a02i], v1, v5\n\t"
        "vwmacc.vv %[a03i], v0, v6\n\t"
        "vwmacc.vv %[a03i], v1, v7\n\t"
        "vneg.v v2, v12\n\t"
        "vneg.v v3, v13\n\t"
        "vwmacc.vv %[b1i], v2, v8\n\t"
        "vwmacc.vv %[b1i], v3, v9\n\t"
        "vwmacc.vv %[a12i], v2, v4\n\t"
        "vwmacc.vv %[a12i], v3, v5\n\t"
        "vwmacc.vv %[a13i], v2, v6\n\t"
        "vwmacc.vv %[a13i], v3, v7\n\t"
        : [a13r] "=&vr"(a13r), [a13i] "=&vr"(a13i),
          [a12r] "=&vr"(a12r), [a12i] "=&vr"(a12i),
          [b0r] "=&vr"(b0r), [b0i] "=&vr"(b0i),
          [a00r] "=&vr"(a00r), [a01r] "=&vr"(a01r),
          [a01i] "=&vr"(a01i), [a02r] "=&vr"(a02r),
          [a02i] "=&vr"(a02i), [a03r] "=&vr"(a03r),
          [a03i] "=&vr"(a03i), [b1r] "=&vr"(b1r),
          [b1i] "=&vr"(b1i), [a11r] "=&vr"(a11r),
          [p0] "+r"(p0), [p1] "+r"(p1), [p2] "+r"(p2),
          [p3] "+r"(p3), [py] "+r"(py)
        :
        : "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
          "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15",
          "memory");

    *b0r_o = b0r;
    *b0i_o = b0i;
    *b1r_o = b1r;
    *b1i_o = b1i;
    *a00r_o = a00r;
    *a01r_o = a01r;
    *a01i_o = a01i;
    *a02r_o = a02r;
    *a02i_o = a02i;
    *a03r_o = a03r;
    *a03i_o = a03i;
    *a11r_o = a11r;
    *a12r_o = a12r;
    *a12i_o = a12i;
    *a13r_o = a13r;
    *a13i_o = a13i;
}

static MIMO_ALWAYS_INLINE void mimo_rvv_mmse4_build_tail_ri_packed_m2_asm(
    const mimo_c16_t *h2,
    const mimo_c16_t *h3,
    const mimo_c16_t *y,
    vint32m1_t keep_w00r,
    vint32m1_t keep_w00i,
    vint32m1_t keep_w10r,
    vint32m1_t keep_w10i,
    vint32m1_t keep_w01r,
    vint32m1_t keep_w01i,
    vint32m1_t keep_w11r,
    vint32m1_t keep_w11i,
    vint32m1_t keep_y0r,
    vint32m1_t keep_y0i,
    vint32m1_t keep_y1r,
    vint32m1_t keep_y1i,
    vint32m1_t *b2r_o,
    vint32m1_t *b2i_o,
    vint32m1_t *b3r_o,
    vint32m1_t *b3i_o,
    vint32m1_t *a22r_o,
    vint32m1_t *a23r_o,
    vint32m1_t *a23i_o,
    vint32m1_t *a33r_o)
{
    const int16_t *p2 = (const int16_t *)h2;
    const int16_t *p3 = (const int16_t *)h3;
    const int16_t *py = (const int16_t *)y;
    vint32m1_t b2r, b2i, b3r, b3i, a22r, a23r, a23i, a33r;

    asm volatile(
        "vsetivli zero, 16, e16, m2, ta, ma\n\t"
        "vle16.v v0, (%[p2])\n\t"
        "vle16.v v2, (%[p3])\n\t"
        "vle16.v v4, (%[py])\n\t"
        "vslidedown.vi v6, v0, 4\n\t"
        "vslidedown.vi v8, v2, 4\n\t"
        "vslidedown.vi v10, v4, 4\n\t"
        "vsetivli zero, 4, e16, mf2, ta, ma\n\t"

        "vwmul.vv %[b2r], v0, v4\n\t"
        "vwmacc.vv %[b2r], v6, v10\n\t"
        "vwmul.vv %[b2i], v0, v10\n\t"
        "vwmacc.vv %[b2r], v1, v5\n\t"
        "vwmacc.vv %[b2r], v7, v11\n\t"
        "vwmacc.vv %[b2i], v1, v11\n\t"
        "vwmul.vv %[b3r], v2, v4\n\t"
        "vwmacc.vv %[b3r], v8, v10\n\t"
        "vwmul.vv %[b3i], v2, v10\n\t"
        "vwmacc.vv %[b3r], v3, v5\n\t"
        "vwmacc.vv %[b3r], v9, v11\n\t"
        "vwmacc.vv %[b3i], v3, v11\n\t"
        "vwmul.vv %[a22r], v0, v0\n\t"
        "vwmacc.vv %[a22r], v6, v6\n\t"
        "vwmacc.vv %[a22r], v1, v1\n\t"
        "vwmacc.vv %[a22r], v7, v7\n\t"
        "vwmul.vv %[a23r], v0, v2\n\t"
        "vwmacc.vv %[a23r], v6, v8\n\t"
        "vwmul.vv %[a23i], v0, v8\n\t"
        "vwmacc.vv %[a23r], v1, v3\n\t"
        "vwmacc.vv %[a23r], v7, v9\n\t"
        "vwmacc.vv %[a23i], v1, v9\n\t"
        "vwmul.vv %[a33r], v2, v2\n\t"
        "vwmacc.vv %[a33r], v8, v8\n\t"
        "vwmacc.vv %[a33r], v3, v3\n\t"
        "vwmacc.vv %[a33r], v9, v9\n\t"
        "vneg.v v0, v6\n\t"
        "vneg.v v1, v7\n\t"
        "vwmacc.vv %[b2i], v0, v4\n\t"
        "vwmacc.vv %[b2i], v1, v5\n\t"
        "vwmacc.vv %[a23i], v0, v2\n\t"
        "vwmacc.vv %[a23i], v1, v3\n\t"
        "vneg.v v2, v8\n\t"
        "vneg.v v3, v9\n\t"
        "vwmacc.vv %[b3i], v2, v4\n\t"
        "vwmacc.vv %[b3i], v3, v5\n\t"

        "addi %[p2], %[p2], 32\n\t"
        "addi %[p3], %[p3], 32\n\t"
        "addi %[py], %[py], 32\n\t"
        "vsetivli zero, 16, e16, m2, ta, ma\n\t"
        "vle16.v v0, (%[p2])\n\t"
        "vle16.v v2, (%[p3])\n\t"
        "vle16.v v4, (%[py])\n\t"
        "vslidedown.vi v6, v0, 4\n\t"
        "vslidedown.vi v8, v2, 4\n\t"
        "vslidedown.vi v10, v4, 4\n\t"
        "vsetivli zero, 4, e16, mf2, ta, ma\n\t"
        "vwmacc.vv %[b2r], v0, v4\n\t"
        "vwmacc.vv %[b2r], v6, v10\n\t"
        "vwmacc.vv %[b2i], v0, v10\n\t"
        "vwmacc.vv %[b2r], v1, v5\n\t"
        "vwmacc.vv %[b2r], v7, v11\n\t"
        "vwmacc.vv %[b2i], v1, v11\n\t"
        "vwmacc.vv %[b3r], v2, v4\n\t"
        "vwmacc.vv %[b3r], v8, v10\n\t"
        "vwmacc.vv %[b3i], v2, v10\n\t"
        "vwmacc.vv %[b3r], v3, v5\n\t"
        "vwmacc.vv %[b3r], v9, v11\n\t"
        "vwmacc.vv %[b3i], v3, v11\n\t"
        "vwmacc.vv %[a22r], v0, v0\n\t"
        "vwmacc.vv %[a22r], v6, v6\n\t"
        "vwmacc.vv %[a22r], v1, v1\n\t"
        "vwmacc.vv %[a22r], v7, v7\n\t"
        "vwmacc.vv %[a23r], v0, v2\n\t"
        "vwmacc.vv %[a23r], v6, v8\n\t"
        "vwmacc.vv %[a23i], v0, v8\n\t"
        "vwmacc.vv %[a23r], v1, v3\n\t"
        "vwmacc.vv %[a23r], v7, v9\n\t"
        "vwmacc.vv %[a23i], v1, v9\n\t"
        "vwmacc.vv %[a33r], v2, v2\n\t"
        "vwmacc.vv %[a33r], v8, v8\n\t"
        "vwmacc.vv %[a33r], v3, v3\n\t"
        "vwmacc.vv %[a33r], v9, v9\n\t"
        "vneg.v v0, v6\n\t"
        "vneg.v v1, v7\n\t"
        "vwmacc.vv %[b2i], v0, v4\n\t"
        "vwmacc.vv %[b2i], v1, v5\n\t"
        "vwmacc.vv %[a23i], v0, v2\n\t"
        "vwmacc.vv %[a23i], v1, v3\n\t"
        "vneg.v v2, v8\n\t"
        "vneg.v v3, v9\n\t"
        "vwmacc.vv %[b3i], v2, v4\n\t"
        "vwmacc.vv %[b3i], v3, v5\n\t"
        : [b2r] "=&vr"(b2r), [b2i] "=&vr"(b2i),
          [b3r] "=&vr"(b3r), [b3i] "=&vr"(b3i),
          [a22r] "=&vr"(a22r), [a23r] "=&vr"(a23r),
          [a23i] "=&vr"(a23i), [a33r] "=&vr"(a33r),
          [p2] "+r"(p2), [p3] "+r"(p3), [py] "+r"(py)
        : [keep_w00r] "vr"(keep_w00r), [keep_w00i] "vr"(keep_w00i),
          [keep_w10r] "vr"(keep_w10r), [keep_w10i] "vr"(keep_w10i),
          [keep_w01r] "vr"(keep_w01r), [keep_w01i] "vr"(keep_w01i),
          [keep_w11r] "vr"(keep_w11r), [keep_w11i] "vr"(keep_w11i),
          [keep_y0r] "vr"(keep_y0r), [keep_y0i] "vr"(keep_y0i),
          [keep_y1r] "vr"(keep_y1r), [keep_y1i] "vr"(keep_y1i)
        : "v0", "v1", "v2", "v3", "v4", "v5",
          "v6", "v7", "v8", "v9", "v10", "v11", "memory");

    *b2r_o = b2r;
    *b2i_o = b2i;
    *b3r_o = b3r;
    *b3i_o = b3i;
    *a22r_o = a22r;
    *a23r_o = a23r;
    *a23i_o = a23i;
    *a33r_o = a33r;
}

static MIMO_ALWAYS_INLINE void mimo_rvv_mmse4_build_tail_ri_packed_m1_asm(
    const mimo_c16_t *h2,
    const mimo_c16_t *h3,
    const mimo_c16_t *y,
    vint32m1_t *b2r_o,
    vint32m1_t *b2i_o,
    vint32m1_t *b3r_o,
    vint32m1_t *b3i_o,
    vint32m1_t *a22r_o,
    vint32m1_t *a23r_o,
    vint32m1_t *a23i_o,
    vint32m1_t *a33r_o)
{
    const int16_t *p2 = (const int16_t *)h2;
    const int16_t *p3 = (const int16_t *)h3;
    const int16_t *py = (const int16_t *)y;
    vint32m1_t b2r, b2i, b3r, b3i, a22r, a23r, a23i, a33r;

    asm volatile(
        "vsetivli zero, 8, e16, m1, ta, ma\n\t"
        "vle16.v v0, (%[p2])\n\t"
        "vle16.v v1, (%[p3])\n\t"
        "vle16.v v2, (%[py])\n\t"
        "vslidedown.vi v3, v0, 4\n\t"
        "vslidedown.vi v4, v1, 4\n\t"
        "vslidedown.vi v5, v2, 4\n\t"
        "vsetivli zero, 4, e16, mf2, ta, ma\n\t"
        "vwmul.vv %[b2r], v0, v2\n\t"
        "vwmacc.vv %[b2r], v3, v5\n\t"
        "vwmul.vv %[b2i], v0, v5\n\t"
        "vwmul.vv %[b3r], v1, v2\n\t"
        "vwmacc.vv %[b3r], v4, v5\n\t"
        "vwmul.vv %[b3i], v1, v5\n\t"
        "vwmul.vv %[a22r], v0, v0\n\t"
        "vwmacc.vv %[a22r], v3, v3\n\t"
        "vwmul.vv %[a23r], v0, v1\n\t"
        "vwmacc.vv %[a23r], v3, v4\n\t"
        "vwmul.vv %[a23i], v0, v4\n\t"
        "vwmul.vv %[a33r], v1, v1\n\t"
        "vwmacc.vv %[a33r], v4, v4\n\t"
        "vneg.v v0, v3\n\t"
        "vwmacc.vv %[b2i], v0, v2\n\t"
        "vwmacc.vv %[a23i], v0, v1\n\t"
        "vneg.v v1, v4\n\t"
        "vwmacc.vv %[b3i], v1, v2\n\t"
        ".irp off, 16, 32, 48\n\t"
        "addi t0, %[p2], \\off\n\t"
        "addi t1, %[p3], \\off\n\t"
        "addi t2, %[py], \\off\n\t"
        "vsetivli zero, 8, e16, m1, ta, ma\n\t"
        "vle16.v v0, (t0)\n\t"
        "vle16.v v1, (t1)\n\t"
        "vle16.v v2, (t2)\n\t"
        "vslidedown.vi v3, v0, 4\n\t"
        "vslidedown.vi v4, v1, 4\n\t"
        "vslidedown.vi v5, v2, 4\n\t"
        "vsetivli zero, 4, e16, mf2, ta, ma\n\t"
        "vwmacc.vv %[b2r], v0, v2\n\t"
        "vwmacc.vv %[b2r], v3, v5\n\t"
        "vwmacc.vv %[b2i], v0, v5\n\t"
        "vwmacc.vv %[b3r], v1, v2\n\t"
        "vwmacc.vv %[b3r], v4, v5\n\t"
        "vwmacc.vv %[b3i], v1, v5\n\t"
        "vwmacc.vv %[a22r], v0, v0\n\t"
        "vwmacc.vv %[a22r], v3, v3\n\t"
        "vwmacc.vv %[a23r], v0, v1\n\t"
        "vwmacc.vv %[a23r], v3, v4\n\t"
        "vwmacc.vv %[a23i], v0, v4\n\t"
        "vwmacc.vv %[a33r], v1, v1\n\t"
        "vwmacc.vv %[a33r], v4, v4\n\t"
        "vneg.v v0, v3\n\t"
        "vwmacc.vv %[b2i], v0, v2\n\t"
        "vwmacc.vv %[a23i], v0, v1\n\t"
        "vneg.v v1, v4\n\t"
        "vwmacc.vv %[b3i], v1, v2\n\t"
        ".endr\n\t"
        : [b2r] "=&vr"(b2r), [b2i] "=&vr"(b2i),
          [b3r] "=&vr"(b3r), [b3i] "=&vr"(b3i),
          [a22r] "=&vr"(a22r), [a23r] "=&vr"(a23r),
          [a23i] "=&vr"(a23i), [a33r] "=&vr"(a33r)
        : [p2] "r"(p2), [p3] "r"(p3), [py] "r"(py)
        : "t0", "t1", "t2", "v0", "v1", "v2", "v3", "v4", "v5",
          "memory");

    *b2r_o = b2r;
    *b2i_o = b2i;
    *b3r_o = b3r;
    *b3i_o = b3i;
    *a22r_o = a22r;
    *a23r_o = a23r;
    *a23i_o = a23i;
    *a33r_o = a33r;
}
#endif

#if MIMO_USE_RVV_MMSE4_RI_PACKED_LOAD == 2
static MIMO_ALWAYS_INLINE void mimo_rvv_mmse4_build_head_ri_packed_asm(
    const mimo_c16_t *h0,
    const mimo_c16_t *h1,
    const mimo_c16_t *h2,
    const mimo_c16_t *h3,
    const mimo_c16_t *y,
    vint32m1_t *b0r_o,
    vint32m1_t *b0i_o,
    vint32m1_t *b1r_o,
    vint32m1_t *b1i_o,
    vint32m1_t *a00r_o,
    vint32m1_t *a01r_o,
    vint32m1_t *a01i_o,
    vint32m1_t *a02r_o,
    vint32m1_t *a02i_o,
    vint32m1_t *a03r_o,
    vint32m1_t *a03i_o,
    vint32m1_t *a11r_o,
    vint32m1_t *a12r_o,
    vint32m1_t *a12i_o,
    vint32m1_t *a13r_o,
    vint32m1_t *a13i_o)
{
    const int16_t *p0 = (const int16_t *)h0;
    const int16_t *p1 = (const int16_t *)h1;
    const int16_t *p2 = (const int16_t *)h2;
    const int16_t *p3 = (const int16_t *)h3;
    const int16_t *py = (const int16_t *)y;
    vint32m1_t b0r, b0i, b1r, b1i;
    vint32m1_t a00r, a01r, a01i, a02r, a02i, a03r, a03i;
    vint32m1_t a11r, a12r, a12i, a13r, a13i;
    vint16mf2_t h0p, h1p, h2p, h3p, yp;
    vint16mf2_t h0i, h1i, h2i, h3i;

    asm volatile(
        "vsetivli zero, 8, e16, m1, ta, ma\n\t"
        "vle16.v %[h0p], (%[p0])\n\t"
        "vle16.v %[h1p], (%[p1])\n\t"
        "vle16.v %[h2p], (%[p2])\n\t"
        "vle16.v %[h3p], (%[p3])\n\t"
        "vle16.v %[yp], (%[py])\n\t"
        "vslidedown.vi %[h0i], %[h0p], 4\n\t"
        "vslidedown.vi %[h1i], %[h1p], 4\n\t"
        "vslidedown.vi %[h2i], %[h2p], 4\n\t"
        "vslidedown.vi %[h3i], %[h3p], 4\n\t"
        "vslidedown.vi v30, %[yp], 4\n\t"
        "vsetivli zero, 4, e16, mf2, ta, ma\n\t"
        "vneg.v v31, %[h0i]\n\t"
        "vwmul.vv %[b0r], %[h0p], %[yp]\n\t"
        "vwmacc.vv %[b0r], %[h0i], v30\n\t"
        "vwmul.vv %[b0i], %[h0p], v30\n\t"
        "vwmacc.vv %[b0i], v31, %[yp]\n\t"
        "vwmul.vv %[a00r], %[h0p], %[h0p]\n\t"
        "vwmacc.vv %[a00r], %[h0i], %[h0i]\n\t"
        "vwmul.vv %[a01r], %[h0p], %[h1p]\n\t"
        "vwmacc.vv %[a01r], %[h0i], %[h1i]\n\t"
        "vwmul.vv %[a01i], %[h0p], %[h1i]\n\t"
        "vwmacc.vv %[a01i], v31, %[h1p]\n\t"
        "vwmul.vv %[a02r], %[h0p], %[h2p]\n\t"
        "vwmacc.vv %[a02r], %[h0i], %[h2i]\n\t"
        "vwmul.vv %[a02i], %[h0p], %[h2i]\n\t"
        "vwmacc.vv %[a02i], v31, %[h2p]\n\t"
        "vwmul.vv %[a03r], %[h0p], %[h3p]\n\t"
        "vwmacc.vv %[a03r], %[h0i], %[h3i]\n\t"
        "vwmul.vv %[a03i], %[h0p], %[h3i]\n\t"
        "vwmacc.vv %[a03i], v31, %[h3p]\n\t"
        "vneg.v v31, %[h1i]\n\t"
        "vwmul.vv %[b1r], %[h1p], %[yp]\n\t"
        "vwmacc.vv %[b1r], %[h1i], v30\n\t"
        "vwmul.vv %[b1i], %[h1p], v30\n\t"
        "vwmacc.vv %[b1i], v31, %[yp]\n\t"
        "vwmul.vv %[a11r], %[h1p], %[h1p]\n\t"
        "vwmacc.vv %[a11r], %[h1i], %[h1i]\n\t"
        "vwmul.vv %[a12r], %[h1p], %[h2p]\n\t"
        "vwmacc.vv %[a12r], %[h1i], %[h2i]\n\t"
        "vwmul.vv %[a12i], %[h1p], %[h2i]\n\t"
        "vwmacc.vv %[a12i], v31, %[h2p]\n\t"
        "vwmul.vv %[a13r], %[h1p], %[h3p]\n\t"
        "vwmacc.vv %[a13r], %[h1i], %[h3i]\n\t"
        "vwmul.vv %[a13i], %[h1p], %[h3i]\n\t"
        "vwmacc.vv %[a13i], v31, %[h3p]\n\t"
        ".irp off, 16, 32, 48\n\t"
        "addi t0, %[p0], \\off\n\t"
        "addi t1, %[p1], \\off\n\t"
        "addi t2, %[p2], \\off\n\t"
        "addi t3, %[p3], \\off\n\t"
        "addi t4, %[py], \\off\n\t"
        "vsetivli zero, 8, e16, m1, ta, ma\n\t"
        "vle16.v %[h0p], (t0)\n\t"
        "vle16.v %[h1p], (t1)\n\t"
        "vle16.v %[h2p], (t2)\n\t"
        "vle16.v %[h3p], (t3)\n\t"
        "vle16.v %[yp], (t4)\n\t"
        "vslidedown.vi %[h0i], %[h0p], 4\n\t"
        "vslidedown.vi %[h1i], %[h1p], 4\n\t"
        "vslidedown.vi %[h2i], %[h2p], 4\n\t"
        "vslidedown.vi %[h3i], %[h3p], 4\n\t"
        "vslidedown.vi v30, %[yp], 4\n\t"
        "vsetivli zero, 4, e16, mf2, ta, ma\n\t"
        "vneg.v v31, %[h0i]\n\t"
        "vwmacc.vv %[b0r], %[h0p], %[yp]\n\t"
        "vwmacc.vv %[b0r], %[h0i], v30\n\t"
        "vwmacc.vv %[b0i], %[h0p], v30\n\t"
        "vwmacc.vv %[b0i], v31, %[yp]\n\t"
        "vwmacc.vv %[a00r], %[h0p], %[h0p]\n\t"
        "vwmacc.vv %[a00r], %[h0i], %[h0i]\n\t"
        "vwmacc.vv %[a01r], %[h0p], %[h1p]\n\t"
        "vwmacc.vv %[a01r], %[h0i], %[h1i]\n\t"
        "vwmacc.vv %[a01i], %[h0p], %[h1i]\n\t"
        "vwmacc.vv %[a01i], v31, %[h1p]\n\t"
        "vwmacc.vv %[a02r], %[h0p], %[h2p]\n\t"
        "vwmacc.vv %[a02r], %[h0i], %[h2i]\n\t"
        "vwmacc.vv %[a02i], %[h0p], %[h2i]\n\t"
        "vwmacc.vv %[a02i], v31, %[h2p]\n\t"
        "vwmacc.vv %[a03r], %[h0p], %[h3p]\n\t"
        "vwmacc.vv %[a03r], %[h0i], %[h3i]\n\t"
        "vwmacc.vv %[a03i], %[h0p], %[h3i]\n\t"
        "vwmacc.vv %[a03i], v31, %[h3p]\n\t"
        "vneg.v v31, %[h1i]\n\t"
        "vwmacc.vv %[b1r], %[h1p], %[yp]\n\t"
        "vwmacc.vv %[b1r], %[h1i], v30\n\t"
        "vwmacc.vv %[b1i], %[h1p], v30\n\t"
        "vwmacc.vv %[b1i], v31, %[yp]\n\t"
        "vwmacc.vv %[a11r], %[h1p], %[h1p]\n\t"
        "vwmacc.vv %[a11r], %[h1i], %[h1i]\n\t"
        "vwmacc.vv %[a12r], %[h1p], %[h2p]\n\t"
        "vwmacc.vv %[a12r], %[h1i], %[h2i]\n\t"
        "vwmacc.vv %[a12i], %[h1p], %[h2i]\n\t"
        "vwmacc.vv %[a12i], v31, %[h2p]\n\t"
        "vwmacc.vv %[a13r], %[h1p], %[h3p]\n\t"
        "vwmacc.vv %[a13r], %[h1i], %[h3i]\n\t"
        "vwmacc.vv %[a13i], %[h1p], %[h3i]\n\t"
        "vwmacc.vv %[a13i], v31, %[h3p]\n\t"
        ".endr\n\t"
        : [b0r] "=&vr"(b0r), [b0i] "=&vr"(b0i),
          [b1r] "=&vr"(b1r), [b1i] "=&vr"(b1i),
          [a00r] "=&vr"(a00r),
          [a01r] "=&vr"(a01r), [a01i] "=&vr"(a01i),
          [a02r] "=&vr"(a02r), [a02i] "=&vr"(a02i),
          [a03r] "=&vr"(a03r), [a03i] "=&vr"(a03i),
          [a11r] "=&vr"(a11r),
          [a12r] "=&vr"(a12r), [a12i] "=&vr"(a12i),
          [a13r] "=&vr"(a13r), [a13i] "=&vr"(a13i),
          [h0p] "=&vr"(h0p), [h1p] "=&vr"(h1p),
          [h2p] "=&vr"(h2p), [h3p] "=&vr"(h3p), [yp] "=&vr"(yp),
          [h0i] "=&vr"(h0i), [h1i] "=&vr"(h1i),
          [h2i] "=&vr"(h2i), [h3i] "=&vr"(h3i)
        : [p0] "r"(p0), [p1] "r"(p1), [p2] "r"(p2),
          [p3] "r"(p3), [py] "r"(py)
        : "t0", "t1", "t2", "t3", "t4", "v30", "v31", "memory");

    *b0r_o = b0r;
    *b0i_o = b0i;
    *b1r_o = b1r;
    *b1i_o = b1i;
    *a00r_o = a00r;
    *a01r_o = a01r;
    *a01i_o = a01i;
    *a02r_o = a02r;
    *a02i_o = a02i;
    *a03r_o = a03r;
    *a03i_o = a03i;
    *a11r_o = a11r;
    *a12r_o = a12r;
    *a12i_o = a12i;
    *a13r_o = a13r;
    *a13i_o = a13i;
}
#endif

static MIMO_ALWAYS_INLINE vint32m1_t mimo_rvv_q15acc_sat_i16_i32m1(
    vint32m1_t x,
    size_t vl)
{
    x = __riscv_vmin_vx_i32m1(x, 32767, vl);
    return __riscv_vmax_vx_i32m1(x, -32768, vl);
}

static MIMO_ALWAYS_INLINE vint32m1_t mimo_rvv_round_once_dot_out_mf2(
    vint32m1_t acc,
    size_t vl)
{
#if MIMO_USE_SITE4_ROUND_ONCE_DOT
#if MIMO_USE_RVV_SITE4_VSSRA_ROUND
    return __riscv_vssra_vx_i32m1(acc, MIMO_Q15_SHIFT, 0, vl);
#else
    acc = __riscv_vadd_vx_i32m1(acc, MIMO_Q15_ROUND, vl);
    return __riscv_vsra_vx_i32m1(acc, MIMO_Q15_SHIFT, vl);
#endif
#else
    return acc;
#endif
}

#if MIMO_USE_RVV_SITE4_VSSRA_ROUND == 2
#define MIMO_RVV_MMSE4_VSSRA_HEAD16_INPLACE(                              \
    b0r_, b0i_, b1r_, b1i_, a00r_, a01r_, a01i_, a02r_, a02i_,          \
    a03r_, a03i_, a11r_, a12r_, a12i_, a13r_, a13i_)                    \
    asm volatile(                                                         \
        "vsetivli zero, 4, e32, m1, ta, ma\n\t"                          \
        "vssra.vi %[b0r], %[b0r], 15\n\t"                               \
        "vssra.vi %[b0i], %[b0i], 15\n\t"                               \
        "vssra.vi %[b1r], %[b1r], 15\n\t"                               \
        "vssra.vi %[b1i], %[b1i], 15\n\t"                               \
        "vssra.vi %[a00r], %[a00r], 15\n\t"                             \
        "vssra.vi %[a01r], %[a01r], 15\n\t"                             \
        "vssra.vi %[a01i], %[a01i], 15\n\t"                             \
        "vssra.vi %[a02r], %[a02r], 15\n\t"                             \
        : [b0r] "+vr"(b0r_), [b0i] "+vr"(b0i_),                        \
          [b1r] "+vr"(b1r_), [b1i] "+vr"(b1i_),                        \
          [a00r] "+vr"(a00r_), [a01r] "+vr"(a01r_),                    \
          [a01i] "+vr"(a01i_), [a02r] "+vr"(a02r_));                   \
    asm volatile(                                                         \
        "vsetivli zero, 4, e32, m1, ta, ma\n\t"                          \
        "vssra.vi %[a02i], %[a02i], 15\n\t"                             \
        "vssra.vi %[a03r], %[a03r], 15\n\t"                             \
        "vssra.vi %[a03i], %[a03i], 15\n\t"                             \
        "vssra.vi %[a11r], %[a11r], 15\n\t"                             \
        "vssra.vi %[a12r], %[a12r], 15\n\t"                             \
        "vssra.vi %[a12i], %[a12i], 15\n\t"                             \
        "vssra.vi %[a13r], %[a13r], 15\n\t"                             \
        "vssra.vi %[a13i], %[a13i], 15\n\t"                             \
        : [a02i] "+vr"(a02i_), [a03r] "+vr"(a03r_),                    \
          [a03i] "+vr"(a03i_), [a11r] "+vr"(a11r_),                    \
          [a12r] "+vr"(a12r_), [a12i] "+vr"(a12i_),                    \
          [a13r] "+vr"(a13r_), [a13i] "+vr"(a13i_))

#define MIMO_RVV_MMSE4_VSSRA_TAIL8_INPLACE(                               \
    b2r_, b2i_, b3r_, b3i_, a22r_, a23r_, a23i_, a33r_)                 \
    asm volatile(                                                         \
        "vsetivli zero, 4, e32, m1, ta, ma\n\t"                          \
        "vssra.vi %[b2r], %[b2r], 15\n\t"                               \
        "vssra.vi %[b2i], %[b2i], 15\n\t"                               \
        "vssra.vi %[b3r], %[b3r], 15\n\t"                               \
        "vssra.vi %[b3i], %[b3i], 15\n\t"                               \
        "vssra.vi %[a22r], %[a22r], 15\n\t"                             \
        "vssra.vi %[a23r], %[a23r], 15\n\t"                             \
        "vssra.vi %[a23i], %[a23i], 15\n\t"                             \
        "vssra.vi %[a33r], %[a33r], 15\n\t"                             \
        : [b2r] "+vr"(b2r_), [b2i] "+vr"(b2i_),                        \
          [b3r] "+vr"(b3r_), [b3i] "+vr"(b3i_),                        \
          [a22r] "+vr"(a22r_), [a23r] "+vr"(a23r_),                    \
          [a23i] "+vr"(a23i_), [a33r] "+vr"(a33r_))
#endif

static MIMO_ALWAYS_INLINE void mimo_rvv_rowfuse_acc_conj_mf2(
    vint16mf2_t ar,
    vint16mf2_t ai,
    vint16mf2_t br,
    vint16mf2_t bi,
    vint32m1_t *acc_re,
    vint32m1_t *acc_im,
    size_t vl)
{
#if MIMO_USE_SITE4_ROUND_ONCE_DOT && !MIMO_USE_RVV_ZF4_Q15ACC_BUILD
    *acc_re = __riscv_vwmacc_vv_i32m1(*acc_re, ar, br, vl);
    *acc_re = __riscv_vwmacc_vv_i32m1(*acc_re, ai, bi, vl);

    const vint16mf2_t neg_ai = __riscv_vneg_v_i16mf2(ai, vl);
    *acc_im = __riscv_vwmacc_vv_i32m1(*acc_im, ar, bi, vl);
    *acc_im = __riscv_vwmacc_vv_i32m1(*acc_im, neg_ai, br, vl);
    return;
#endif

    vint32m1_t re = __riscv_vwmul_vv_i32m1(ar, br, vl);
    re = __riscv_vwmacc_vv_i32m1(re, ai, bi, vl);

    vint32m1_t im = __riscv_vwmul_vv_i32m1(ar, bi, vl);
    const vint32m1_t tmp = __riscv_vwmul_vv_i32m1(ai, br, vl);
    im = __riscv_vsub_vv_i32m1(im, tmp, vl);

#if MIMO_USE_RVV_ZF4_Q15ACC_BUILD
    re = __riscv_vadd_vx_i32m1(re, MIMO_Q15_ROUND, vl);
    im = __riscv_vadd_vx_i32m1(im, MIMO_Q15_ROUND, vl);
    re = __riscv_vsra_vx_i32m1(re, MIMO_Q15_SHIFT, vl);
    im = __riscv_vsra_vx_i32m1(im, MIMO_Q15_SHIFT, vl);
    *acc_re = mimo_rvv_q15acc_sat_i16_i32m1(
        __riscv_vadd_vv_i32m1(*acc_re, re, vl), vl);
    *acc_im = mimo_rvv_q15acc_sat_i16_i32m1(
        __riscv_vadd_vv_i32m1(*acc_im, im, vl), vl);
#else
#if !MIMO_USE_SITE4_ROUND_ONCE_DOT
    re = __riscv_vadd_vx_i32m1(re, MIMO_Q15_ROUND, vl);
    im = __riscv_vadd_vx_i32m1(im, MIMO_Q15_ROUND, vl);
    re = __riscv_vsra_vx_i32m1(re, MIMO_Q15_SHIFT, vl);
    im = __riscv_vsra_vx_i32m1(im, MIMO_Q15_SHIFT, vl);
#endif

    *acc_re = __riscv_vadd_vv_i32m1(*acc_re, re, vl);
    *acc_im = __riscv_vadd_vv_i32m1(*acc_im, im, vl);
#endif
}

static MIMO_ALWAYS_INLINE void mimo_rvv_rowfuse_acc_real_mf2(
    vint16mf2_t ar,
    vint16mf2_t ai,
    vint32m1_t *acc_re,
    size_t vl)
{
#if MIMO_USE_SITE4_ROUND_ONCE_DOT && !MIMO_USE_RVV_ZF4_Q15ACC_BUILD
    *acc_re = __riscv_vwmacc_vv_i32m1(*acc_re, ar, ar, vl);
    *acc_re = __riscv_vwmacc_vv_i32m1(*acc_re, ai, ai, vl);
    return;
#endif

    vint32m1_t re = __riscv_vwmul_vv_i32m1(ar, ar, vl);
    re = __riscv_vwmacc_vv_i32m1(re, ai, ai, vl);
#if MIMO_USE_RVV_ZF4_Q15ACC_BUILD
    re = __riscv_vadd_vx_i32m1(re, MIMO_Q15_ROUND, vl);
    re = __riscv_vsra_vx_i32m1(re, MIMO_Q15_SHIFT, vl);
    *acc_re = mimo_rvv_q15acc_sat_i16_i32m1(
        __riscv_vadd_vv_i32m1(*acc_re, re, vl), vl);
#else
#if !MIMO_USE_SITE4_ROUND_ONCE_DOT
    re = __riscv_vadd_vx_i32m1(re, MIMO_Q15_ROUND, vl);
    re = __riscv_vsra_vx_i32m1(re, MIMO_Q15_SHIFT, vl);
#endif
    *acc_re = __riscv_vadd_vv_i32m1(*acc_re, re, vl);
#endif
}

static MIMO_ALWAYS_INLINE void mimo_rvv_rowfuse_seed_conj_mf2(
    vint16mf2_t ar,
    vint16mf2_t ai,
    vint16mf2_t br,
    vint16mf2_t bi,
    vint32m1_t *acc_re,
    vint32m1_t *acc_im,
    size_t vl)
{
    vint32m1_t re = __riscv_vwmul_vv_i32m1(ar, br, vl);
    re = __riscv_vwmacc_vv_i32m1(re, ai, bi, vl);

    vint32m1_t im = __riscv_vwmul_vv_i32m1(ar, bi, vl);
    const vint32m1_t tmp = __riscv_vwmul_vv_i32m1(ai, br, vl);
    im = __riscv_vsub_vv_i32m1(im, tmp, vl);

#if MIMO_USE_RVV_ZF4_Q15ACC_BUILD
    re = __riscv_vadd_vx_i32m1(re, MIMO_Q15_ROUND, vl);
    im = __riscv_vadd_vx_i32m1(im, MIMO_Q15_ROUND, vl);
    re = __riscv_vsra_vx_i32m1(re, MIMO_Q15_SHIFT, vl);
    im = __riscv_vsra_vx_i32m1(im, MIMO_Q15_SHIFT, vl);
    *acc_re = mimo_rvv_q15acc_sat_i16_i32m1(re, vl);
    *acc_im = mimo_rvv_q15acc_sat_i16_i32m1(im, vl);
#else
#if !MIMO_USE_SITE4_ROUND_ONCE_DOT
    re = __riscv_vadd_vx_i32m1(re, MIMO_Q15_ROUND, vl);
    im = __riscv_vadd_vx_i32m1(im, MIMO_Q15_ROUND, vl);
    re = __riscv_vsra_vx_i32m1(re, MIMO_Q15_SHIFT, vl);
    im = __riscv_vsra_vx_i32m1(im, MIMO_Q15_SHIFT, vl);
#endif
    *acc_re = re;
    *acc_im = im;
#endif
}

static MIMO_ALWAYS_INLINE void mimo_rvv_rowfuse_seed_real_mf2(
    vint16mf2_t ar,
    vint16mf2_t ai,
    vint32m1_t *acc_re,
    size_t vl)
{
    vint32m1_t re = __riscv_vwmul_vv_i32m1(ar, ar, vl);
    re = __riscv_vwmacc_vv_i32m1(re, ai, ai, vl);
#if MIMO_USE_RVV_ZF4_Q15ACC_BUILD
    re = __riscv_vadd_vx_i32m1(re, MIMO_Q15_ROUND, vl);
    re = __riscv_vsra_vx_i32m1(re, MIMO_Q15_SHIFT, vl);
    *acc_re = mimo_rvv_q15acc_sat_i16_i32m1(re, vl);
#else
#if !MIMO_USE_SITE4_ROUND_ONCE_DOT
    re = __riscv_vadd_vx_i32m1(re, MIMO_Q15_ROUND, vl);
    re = __riscv_vsra_vx_i32m1(re, MIMO_Q15_SHIFT, vl);
#endif
    *acc_re = re;
#endif
}
#endif

static MIMO_ALWAYS_INLINE void mimo_rvv_dot4_sites_store_conj_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *rhs_site4,
    size_t block,
    int tx,
    int rhs_tx,
    int rhs_is_h,
    int32_t *re_dst0,
    ptrdiff_t re_stride0,
    int32_t *im_dst0,
    ptrdiff_t im_stride0,
    int32_t *re_dst1,
    ptrdiff_t re_stride1,
    int32_t *im_dst1,
    ptrdiff_t im_stride1,
    int conj_dst1)
{
#if MIMO_USE_RVV_SITE4_MF2
    const size_t vl = __riscv_vsetvl_e16mf2(MIMO_SITE4_LANES);
    vint32m1_t acc_re = __riscv_vmv_v_x_i32m1(0, vl);
    vint32m1_t acc_im = __riscv_vmv_v_x_i32m1(0, vl);

    for (int rx = 0; rx < MIMO_N4; ++rx) {
        const mimo_c16_t *a = mimo_site4_h4_vec(h_site4, block, tx, rx);
        const mimo_c16_t *b = rhs_is_h
                                  ? mimo_site4_h4_vec(rhs_site4, block, rhs_tx, rx)
                                  : mimo_site4_y4_vec(rhs_site4, block, rx);

        vint16mf2_t ar =
            __riscv_vlse16_v_i16mf2(&a[0].r, sizeof(mimo_c16_t), vl);
        vint16mf2_t ai =
            __riscv_vlse16_v_i16mf2(&a[0].i, sizeof(mimo_c16_t), vl);
        vint16mf2_t br =
            __riscv_vlse16_v_i16mf2(&b[0].r, sizeof(mimo_c16_t), vl);
        vint16mf2_t bi =
            __riscv_vlse16_v_i16mf2(&b[0].i, sizeof(mimo_c16_t), vl);

        vint32m1_t re = __riscv_vwmul_vv_i32m1(ar, br, vl);
        re = __riscv_vwmacc_vv_i32m1(re, ai, bi, vl);

        vint32m1_t im = __riscv_vwmul_vv_i32m1(ar, bi, vl);
        const vint32m1_t tmp = __riscv_vwmul_vv_i32m1(ai, br, vl);
        im = __riscv_vsub_vv_i32m1(im, tmp, vl);

        re = __riscv_vadd_vx_i32m1(re, MIMO_Q15_ROUND, vl);
        im = __riscv_vadd_vx_i32m1(im, MIMO_Q15_ROUND, vl);
        re = __riscv_vsra_vx_i32m1(re, MIMO_Q15_SHIFT, vl);
        im = __riscv_vsra_vx_i32m1(im, MIMO_Q15_SHIFT, vl);

        acc_re = __riscv_vadd_vv_i32m1(acc_re, re, vl);
        acc_im = __riscv_vadd_vv_i32m1(acc_im, im, vl);
    }

    __riscv_vsse32_v_i32m1(re_dst0, re_stride0, acc_re, vl);
    __riscv_vsse32_v_i32m1(im_dst0, im_stride0, acc_im, vl);

    if (re_dst1 != (int32_t *)0) {
        __riscv_vsse32_v_i32m1(re_dst1, re_stride1, acc_re, vl);
        if (conj_dst1) {
            const vint32m1_t zero = __riscv_vmv_v_x_i32m1(0, vl);
            const vint32m1_t neg_im = __riscv_vsub_vv_i32m1(zero, acc_im, vl);
            __riscv_vsse32_v_i32m1(im_dst1, im_stride1, neg_im, vl);
        } else {
            __riscv_vsse32_v_i32m1(im_dst1, im_stride1, acc_im, vl);
        }
    }
#else
    const size_t vl = __riscv_vsetvl_e16m1(MIMO_SITE4_LANES);
    vint32m2_t acc_re = __riscv_vmv_v_x_i32m2(0, vl);
    vint32m2_t acc_im = __riscv_vmv_v_x_i32m2(0, vl);

    for (int rx = 0; rx < MIMO_N4; ++rx) {
        const mimo_c16_t *a = mimo_site4_h4_vec(h_site4, block, tx, rx);
        const mimo_c16_t *b = rhs_is_h
                                  ? mimo_site4_h4_vec(rhs_site4, block, rhs_tx, rx)
                                  : mimo_site4_y4_vec(rhs_site4, block, rx);

        vint16m1_t ar =
            __riscv_vlse16_v_i16m1(&a[0].r, sizeof(mimo_c16_t), vl);
        vint16m1_t ai =
            __riscv_vlse16_v_i16m1(&a[0].i, sizeof(mimo_c16_t), vl);
        vint16m1_t br =
            __riscv_vlse16_v_i16m1(&b[0].r, sizeof(mimo_c16_t), vl);
        vint16m1_t bi =
            __riscv_vlse16_v_i16m1(&b[0].i, sizeof(mimo_c16_t), vl);

        vint32m2_t re = __riscv_vwmul_vv_i32m2(ar, br, vl);
        re = __riscv_vwmacc_vv_i32m2(re, ai, bi, vl);

        vint32m2_t im = __riscv_vwmul_vv_i32m2(ar, bi, vl);
        const vint32m2_t tmp = __riscv_vwmul_vv_i32m2(ai, br, vl);
        im = __riscv_vsub_vv_i32m2(im, tmp, vl);

        re = __riscv_vadd_vx_i32m2(re, MIMO_Q15_ROUND, vl);
        im = __riscv_vadd_vx_i32m2(im, MIMO_Q15_ROUND, vl);
        re = __riscv_vsra_vx_i32m2(re, MIMO_Q15_SHIFT, vl);
        im = __riscv_vsra_vx_i32m2(im, MIMO_Q15_SHIFT, vl);

        acc_re = __riscv_vadd_vv_i32m2(acc_re, re, vl);
        acc_im = __riscv_vadd_vv_i32m2(acc_im, im, vl);
    }

    __riscv_vsse32_v_i32m2(re_dst0, re_stride0, acc_re, vl);
    __riscv_vsse32_v_i32m2(im_dst0, im_stride0, acc_im, vl);

    if (re_dst1 != (int32_t *)0) {
        __riscv_vsse32_v_i32m2(re_dst1, re_stride1, acc_re, vl);
        if (conj_dst1) {
            const vint32m2_t zero = __riscv_vmv_v_x_i32m2(0, vl);
            const vint32m2_t neg_im = __riscv_vsub_vv_i32m2(zero, acc_im, vl);
            __riscv_vsse32_v_i32m2(im_dst1, im_stride1, neg_im, vl);
        } else {
            __riscv_vsse32_v_i32m2(im_dst1, im_stride1, acc_im, vl);
        }
    }
#endif
}

static MIMO_ALWAYS_INLINE void mimo_rvv_dot4_sites_store_real_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *rhs_site4,
    size_t block,
    int tx,
    int rhs_tx,
    int rhs_is_h,
    int32_t *re_dst0,
    ptrdiff_t re_stride0,
    int32_t *re_dst1,
    ptrdiff_t re_stride1)
{
#if MIMO_USE_RVV_SITE4_MF2
    const size_t vl = __riscv_vsetvl_e16mf2(MIMO_SITE4_LANES);
    vint32m1_t acc_re = __riscv_vmv_v_x_i32m1(0, vl);

    for (int rx = 0; rx < MIMO_N4; ++rx) {
        const mimo_c16_t *a = mimo_site4_h4_vec(h_site4, block, tx, rx);
        const mimo_c16_t *b = rhs_is_h
                                  ? mimo_site4_h4_vec(rhs_site4, block, rhs_tx, rx)
                                  : mimo_site4_y4_vec(rhs_site4, block, rx);

        vint16mf2_t ar =
            __riscv_vlse16_v_i16mf2(&a[0].r, sizeof(mimo_c16_t), vl);
        vint16mf2_t ai =
            __riscv_vlse16_v_i16mf2(&a[0].i, sizeof(mimo_c16_t), vl);
        vint16mf2_t br =
            __riscv_vlse16_v_i16mf2(&b[0].r, sizeof(mimo_c16_t), vl);
        vint16mf2_t bi =
            __riscv_vlse16_v_i16mf2(&b[0].i, sizeof(mimo_c16_t), vl);

        vint32m1_t re = __riscv_vwmul_vv_i32m1(ar, br, vl);
        re = __riscv_vwmacc_vv_i32m1(re, ai, bi, vl);
        re = __riscv_vadd_vx_i32m1(re, MIMO_Q15_ROUND, vl);
        re = __riscv_vsra_vx_i32m1(re, MIMO_Q15_SHIFT, vl);
        acc_re = __riscv_vadd_vv_i32m1(acc_re, re, vl);
    }

    __riscv_vsse32_v_i32m1(re_dst0, re_stride0, acc_re, vl);
    if (re_dst1 != (int32_t *)0) {
        __riscv_vsse32_v_i32m1(re_dst1, re_stride1, acc_re, vl);
    }
#else
    const size_t vl = __riscv_vsetvl_e16m1(MIMO_SITE4_LANES);
    vint32m2_t acc_re = __riscv_vmv_v_x_i32m2(0, vl);

    for (int rx = 0; rx < MIMO_N4; ++rx) {
        const mimo_c16_t *a = mimo_site4_h4_vec(h_site4, block, tx, rx);
        const mimo_c16_t *b = rhs_is_h
                                  ? mimo_site4_h4_vec(rhs_site4, block, rhs_tx, rx)
                                  : mimo_site4_y4_vec(rhs_site4, block, rx);

        vint16m1_t ar =
            __riscv_vlse16_v_i16m1(&a[0].r, sizeof(mimo_c16_t), vl);
        vint16m1_t ai =
            __riscv_vlse16_v_i16m1(&a[0].i, sizeof(mimo_c16_t), vl);
        vint16m1_t br =
            __riscv_vlse16_v_i16m1(&b[0].r, sizeof(mimo_c16_t), vl);
        vint16m1_t bi =
            __riscv_vlse16_v_i16m1(&b[0].i, sizeof(mimo_c16_t), vl);

        vint32m2_t re = __riscv_vwmul_vv_i32m2(ar, br, vl);
        re = __riscv_vwmacc_vv_i32m2(re, ai, bi, vl);
        re = __riscv_vadd_vx_i32m2(re, MIMO_Q15_ROUND, vl);
        re = __riscv_vsra_vx_i32m2(re, MIMO_Q15_SHIFT, vl);
        acc_re = __riscv_vadd_vv_i32m2(acc_re, re, vl);
    }

    __riscv_vsse32_v_i32m2(re_dst0, re_stride0, acc_re, vl);
    if (re_dst1 != (int32_t *)0) {
        __riscv_vsse32_v_i32m2(re_dst1, re_stride1, acc_re, vl);
    }
#endif
}

static MIMO_ALWAYS_INLINE void mimo_rvv_build_normal4_site4_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_c32_t a_lane[MIMO_SITE4_LANES][MIMO_N4 * MIMO_N4],
    mimo_c32_t b_lane[MIMO_SITE4_LANES][MIMO_N4])
{
    const ptrdiff_t a_stride =
        (ptrdiff_t)(MIMO_N4 * MIMO_N4 * sizeof(mimo_c32_t));
    const ptrdiff_t b_stride =
        (ptrdiff_t)(MIMO_N4 * sizeof(mimo_c32_t));

    for (int tx = 0; tx < MIMO_N4; ++tx) {
        mimo_rvv_dot4_sites_store_conj_q15(h_site4, y_site4, block, tx, 0, 0,
                                           &b_lane[0][tx].r, b_stride,
                                           &b_lane[0][tx].i, b_stride,
                                           (int32_t *)0, 0,
                                           (int32_t *)0, 0, 0);
    }

    for (int tx_i = 0; tx_i < MIMO_N4; ++tx_i) {
        for (int tx_j = tx_i; tx_j < MIMO_N4; ++tx_j) {
            const size_t upper = (size_t)(tx_i * MIMO_N4 + tx_j);
            const size_t lower = (size_t)(tx_j * MIMO_N4 + tx_i);

            if (tx_i == tx_j) {
#if MIMO_USE_SITE4_DIAG_REAL_BUILD && !MIMO_COLLECT_SCHUR_STATS
                mimo_rvv_dot4_sites_store_real_q15(
                    h_site4, h_site4, block, tx_i, tx_j, 1,
                    &a_lane[0][upper].r, a_stride,
                    (int32_t *)0, 0);
#else
                mimo_rvv_dot4_sites_store_conj_q15(
                    h_site4, h_site4, block, tx_i, tx_j, 1,
                    &a_lane[0][upper].r, a_stride,
                    &a_lane[0][upper].i, a_stride,
                    (int32_t *)0, 0,
                    (int32_t *)0, 0, 0);
#endif
            } else {
                mimo_rvv_dot4_sites_store_conj_q15(
                    h_site4, h_site4, block, tx_i, tx_j, 1,
                    &a_lane[0][upper].r, a_stride,
                    &a_lane[0][upper].i, a_stride,
                    &a_lane[0][lower].r, a_stride,
                    &a_lane[0][lower].i, a_stride, 1);
            }
        }
    }
}

static MIMO_ALWAYS_INLINE void mimo_rvv_build_hermitian4_site4_upper_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_c32_t a_lane[MIMO_SITE4_LANES][MIMO_N4 * MIMO_N4],
    mimo_c32_t b_lane[MIMO_SITE4_LANES][MIMO_N4])
{
    const ptrdiff_t a_stride =
        (ptrdiff_t)(MIMO_N4 * MIMO_N4 * sizeof(mimo_c32_t));
    const ptrdiff_t b_stride =
        (ptrdiff_t)(MIMO_N4 * sizeof(mimo_c32_t));

    for (int tx = 0; tx < MIMO_N4; ++tx) {
        mimo_rvv_dot4_sites_store_conj_q15(h_site4, y_site4, block, tx, 0, 0,
                                           &b_lane[0][tx].r, b_stride,
                                           &b_lane[0][tx].i, b_stride,
                                           (int32_t *)0, 0,
                                           (int32_t *)0, 0, 0);
    }

    for (int tx_i = 0; tx_i < MIMO_N4; ++tx_i) {
        for (int tx_j = tx_i; tx_j < MIMO_N4; ++tx_j) {
            const size_t upper = (size_t)(tx_i * MIMO_N4 + tx_j);

            if (tx_i == tx_j) {
                mimo_rvv_dot4_sites_store_real_q15(
                    h_site4, h_site4, block, tx_i, tx_j, 1,
                    &a_lane[0][upper].r, a_stride,
                    (int32_t *)0, 0);
            } else {
                mimo_rvv_dot4_sites_store_conj_q15(
                    h_site4, h_site4, block, tx_i, tx_j, 1,
                    &a_lane[0][upper].r, a_stride,
                    &a_lane[0][upper].i, a_stride,
                    (int32_t *)0, 0,
                    (int32_t *)0, 0, 0);
            }
        }
    }
}

static MIMO_ALWAYS_INLINE void mimo_rvv_dot4_sites_store_conj_soa_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *rhs_site4,
    size_t block,
    int tx,
    int rhs_tx,
    int rhs_is_h,
    int32_t *re_dst0,
    int32_t *im_dst0,
    int32_t *re_dst1,
    int32_t *im_dst1,
    int conj_dst1)
{
#if MIMO_USE_RVV_SITE4_MF2
    const size_t vl = __riscv_vsetvl_e16mf2(MIMO_SITE4_LANES);
    vint32m1_t acc_re = __riscv_vmv_v_x_i32m1(0, vl);
    vint32m1_t acc_im = __riscv_vmv_v_x_i32m1(0, vl);

    for (int rx = 0; rx < MIMO_N4; ++rx) {
        const mimo_c16_t *a = mimo_site4_h4_vec(h_site4, block, tx, rx);
        const mimo_c16_t *b = rhs_is_h
                                  ? mimo_site4_h4_vec(rhs_site4, block, rhs_tx, rx)
                                  : mimo_site4_y4_vec(rhs_site4, block, rx);

        vint16mf2_t ar =
            __riscv_vlse16_v_i16mf2(&a[0].r, sizeof(mimo_c16_t), vl);
        vint16mf2_t ai =
            __riscv_vlse16_v_i16mf2(&a[0].i, sizeof(mimo_c16_t), vl);
        vint16mf2_t br =
            __riscv_vlse16_v_i16mf2(&b[0].r, sizeof(mimo_c16_t), vl);
        vint16mf2_t bi =
            __riscv_vlse16_v_i16mf2(&b[0].i, sizeof(mimo_c16_t), vl);

        vint32m1_t re = __riscv_vwmul_vv_i32m1(ar, br, vl);
        re = __riscv_vwmacc_vv_i32m1(re, ai, bi, vl);

        vint32m1_t im = __riscv_vwmul_vv_i32m1(ar, bi, vl);
        const vint32m1_t tmp = __riscv_vwmul_vv_i32m1(ai, br, vl);
        im = __riscv_vsub_vv_i32m1(im, tmp, vl);

        re = __riscv_vadd_vx_i32m1(re, MIMO_Q15_ROUND, vl);
        im = __riscv_vadd_vx_i32m1(im, MIMO_Q15_ROUND, vl);
        re = __riscv_vsra_vx_i32m1(re, MIMO_Q15_SHIFT, vl);
        im = __riscv_vsra_vx_i32m1(im, MIMO_Q15_SHIFT, vl);

        acc_re = __riscv_vadd_vv_i32m1(acc_re, re, vl);
        acc_im = __riscv_vadd_vv_i32m1(acc_im, im, vl);
    }

    __riscv_vse32_v_i32m1(re_dst0, acc_re, vl);
    __riscv_vse32_v_i32m1(im_dst0, acc_im, vl);

    if (re_dst1 != (int32_t *)0) {
        __riscv_vse32_v_i32m1(re_dst1, acc_re, vl);
        if (conj_dst1) {
            const vint32m1_t zero = __riscv_vmv_v_x_i32m1(0, vl);
            const vint32m1_t neg_im = __riscv_vsub_vv_i32m1(zero, acc_im, vl);
            __riscv_vse32_v_i32m1(im_dst1, neg_im, vl);
        } else {
            __riscv_vse32_v_i32m1(im_dst1, acc_im, vl);
        }
    }
#else
    const size_t vl = __riscv_vsetvl_e16m1(MIMO_SITE4_LANES);
    vint32m2_t acc_re = __riscv_vmv_v_x_i32m2(0, vl);
    vint32m2_t acc_im = __riscv_vmv_v_x_i32m2(0, vl);

    for (int rx = 0; rx < MIMO_N4; ++rx) {
        const mimo_c16_t *a = mimo_site4_h4_vec(h_site4, block, tx, rx);
        const mimo_c16_t *b = rhs_is_h
                                  ? mimo_site4_h4_vec(rhs_site4, block, rhs_tx, rx)
                                  : mimo_site4_y4_vec(rhs_site4, block, rx);

        vint16m1_t ar =
            __riscv_vlse16_v_i16m1(&a[0].r, sizeof(mimo_c16_t), vl);
        vint16m1_t ai =
            __riscv_vlse16_v_i16m1(&a[0].i, sizeof(mimo_c16_t), vl);
        vint16m1_t br =
            __riscv_vlse16_v_i16m1(&b[0].r, sizeof(mimo_c16_t), vl);
        vint16m1_t bi =
            __riscv_vlse16_v_i16m1(&b[0].i, sizeof(mimo_c16_t), vl);

        vint32m2_t re = __riscv_vwmul_vv_i32m2(ar, br, vl);
        re = __riscv_vwmacc_vv_i32m2(re, ai, bi, vl);

        vint32m2_t im = __riscv_vwmul_vv_i32m2(ar, bi, vl);
        const vint32m2_t tmp = __riscv_vwmul_vv_i32m2(ai, br, vl);
        im = __riscv_vsub_vv_i32m2(im, tmp, vl);

        re = __riscv_vadd_vx_i32m2(re, MIMO_Q15_ROUND, vl);
        im = __riscv_vadd_vx_i32m2(im, MIMO_Q15_ROUND, vl);
        re = __riscv_vsra_vx_i32m2(re, MIMO_Q15_SHIFT, vl);
        im = __riscv_vsra_vx_i32m2(im, MIMO_Q15_SHIFT, vl);

        acc_re = __riscv_vadd_vv_i32m2(acc_re, re, vl);
        acc_im = __riscv_vadd_vv_i32m2(acc_im, im, vl);
    }

    __riscv_vse32_v_i32m2(re_dst0, acc_re, vl);
    __riscv_vse32_v_i32m2(im_dst0, acc_im, vl);

    if (re_dst1 != (int32_t *)0) {
        __riscv_vse32_v_i32m2(re_dst1, acc_re, vl);
        if (conj_dst1) {
            const vint32m2_t zero = __riscv_vmv_v_x_i32m2(0, vl);
            const vint32m2_t neg_im = __riscv_vsub_vv_i32m2(zero, acc_im, vl);
            __riscv_vse32_v_i32m2(im_dst1, neg_im, vl);
        } else {
            __riscv_vse32_v_i32m2(im_dst1, acc_im, vl);
        }
    }
#endif
}

static MIMO_ALWAYS_INLINE void mimo_rvv_dot4_sites_store_real_soa_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *rhs_site4,
    size_t block,
    int tx,
    int rhs_tx,
    int rhs_is_h,
    int32_t *re_dst0,
    int32_t *re_dst1)
{
#if MIMO_USE_RVV_SITE4_MF2
    const size_t vl = __riscv_vsetvl_e16mf2(MIMO_SITE4_LANES);
    vint32m1_t acc_re = __riscv_vmv_v_x_i32m1(0, vl);

    for (int rx = 0; rx < MIMO_N4; ++rx) {
        const mimo_c16_t *a = mimo_site4_h4_vec(h_site4, block, tx, rx);
        const mimo_c16_t *b = rhs_is_h
                                  ? mimo_site4_h4_vec(rhs_site4, block, rhs_tx, rx)
                                  : mimo_site4_y4_vec(rhs_site4, block, rx);

        vint16mf2_t ar =
            __riscv_vlse16_v_i16mf2(&a[0].r, sizeof(mimo_c16_t), vl);
        vint16mf2_t ai =
            __riscv_vlse16_v_i16mf2(&a[0].i, sizeof(mimo_c16_t), vl);
        vint16mf2_t br =
            __riscv_vlse16_v_i16mf2(&b[0].r, sizeof(mimo_c16_t), vl);
        vint16mf2_t bi =
            __riscv_vlse16_v_i16mf2(&b[0].i, sizeof(mimo_c16_t), vl);

        vint32m1_t re = __riscv_vwmul_vv_i32m1(ar, br, vl);
        re = __riscv_vwmacc_vv_i32m1(re, ai, bi, vl);
        re = __riscv_vadd_vx_i32m1(re, MIMO_Q15_ROUND, vl);
        re = __riscv_vsra_vx_i32m1(re, MIMO_Q15_SHIFT, vl);
        acc_re = __riscv_vadd_vv_i32m1(acc_re, re, vl);
    }

    __riscv_vse32_v_i32m1(re_dst0, acc_re, vl);
    if (re_dst1 != (int32_t *)0) {
        __riscv_vse32_v_i32m1(re_dst1, acc_re, vl);
    }
#else
    const size_t vl = __riscv_vsetvl_e16m1(MIMO_SITE4_LANES);
    vint32m2_t acc_re = __riscv_vmv_v_x_i32m2(0, vl);

    for (int rx = 0; rx < MIMO_N4; ++rx) {
        const mimo_c16_t *a = mimo_site4_h4_vec(h_site4, block, tx, rx);
        const mimo_c16_t *b = rhs_is_h
                                  ? mimo_site4_h4_vec(rhs_site4, block, rhs_tx, rx)
                                  : mimo_site4_y4_vec(rhs_site4, block, rx);

        vint16m1_t ar =
            __riscv_vlse16_v_i16m1(&a[0].r, sizeof(mimo_c16_t), vl);
        vint16m1_t ai =
            __riscv_vlse16_v_i16m1(&a[0].i, sizeof(mimo_c16_t), vl);
        vint16m1_t br =
            __riscv_vlse16_v_i16m1(&b[0].r, sizeof(mimo_c16_t), vl);
        vint16m1_t bi =
            __riscv_vlse16_v_i16m1(&b[0].i, sizeof(mimo_c16_t), vl);

        vint32m2_t re = __riscv_vwmul_vv_i32m2(ar, br, vl);
        re = __riscv_vwmacc_vv_i32m2(re, ai, bi, vl);
        re = __riscv_vadd_vx_i32m2(re, MIMO_Q15_ROUND, vl);
        re = __riscv_vsra_vx_i32m2(re, MIMO_Q15_SHIFT, vl);
        acc_re = __riscv_vadd_vv_i32m2(acc_re, re, vl);
    }

    __riscv_vse32_v_i32m2(re_dst0, acc_re, vl);
    if (re_dst1 != (int32_t *)0) {
        __riscv_vse32_v_i32m2(re_dst1, acc_re, vl);
    }
#endif
}

static MIMO_ALWAYS_INLINE void mimo_rvv_build_normal4_site4_soa_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_site4_soa4_t *work)
{
    for (int tx = 0; tx < MIMO_N4; ++tx) {
        mimo_rvv_dot4_sites_store_conj_soa_q15(
            h_site4, y_site4, block, tx, 0, 0,
            &work->b_re[tx][0], &work->b_im[tx][0],
            (int32_t *)0, (int32_t *)0, 0);
    }

    for (int tx_i = 0; tx_i < MIMO_N4; ++tx_i) {
        for (int tx_j = tx_i; tx_j < MIMO_N4; ++tx_j) {
            const size_t upper = (size_t)(tx_i * MIMO_N4 + tx_j);
            const size_t lower = (size_t)(tx_j * MIMO_N4 + tx_i);

            if (tx_i == tx_j) {
#if MIMO_USE_SITE4_DIAG_REAL_BUILD && !MIMO_COLLECT_SCHUR_STATS
                mimo_rvv_dot4_sites_store_real_soa_q15(
                    h_site4, h_site4, block, tx_i, tx_j, 1,
                    &work->a_re[upper][0], (int32_t *)0);
#else
                mimo_rvv_dot4_sites_store_conj_soa_q15(
                    h_site4, h_site4, block, tx_i, tx_j, 1,
                    &work->a_re[upper][0], &work->a_im[upper][0],
                    (int32_t *)0, (int32_t *)0, 0);
#endif
            } else {
                mimo_rvv_dot4_sites_store_conj_soa_q15(
                    h_site4, h_site4, block, tx_i, tx_j, 1,
                    &work->a_re[upper][0], &work->a_im[upper][0],
                    &work->a_re[lower][0], &work->a_im[lower][0], 1);
            }
        }
    }
}

#if MIMO_USE_RVV_SITE4_MF2
static MIMO_ALWAYS_INLINE void mimo_rvv_build_hermitian4_site4_upper_row0_soa_mf2_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_site4_soa4_t *work)
{
    const size_t vl = __riscv_vsetvl_e16mf2(MIMO_SITE4_LANES);
    const vint32m1_t zero = __riscv_vmv_v_x_i32m1(0, vl);
    vint32m1_t b0r = zero;
    vint32m1_t b0i = zero;
    vint32m1_t a00r = zero;
    vint32m1_t a01r = zero;
    vint32m1_t a01i = zero;
    vint32m1_t a02r = zero;
    vint32m1_t a02i = zero;
    vint32m1_t a03r = zero;
    vint32m1_t a03i = zero;

    for (int rx = 0; rx < MIMO_N4; ++rx) {
        const mimo_c16_t *h0 = mimo_site4_h4_vec(h_site4, block, 0, rx);
        const mimo_c16_t *h1 = mimo_site4_h4_vec(h_site4, block, 1, rx);
        const mimo_c16_t *h2 = mimo_site4_h4_vec(h_site4, block, 2, rx);
        const mimo_c16_t *h3 = mimo_site4_h4_vec(h_site4, block, 3, rx);
        const mimo_c16_t *yy = mimo_site4_y4_vec(y_site4, block, rx);
        const vint16mf2_t h0r =
            __riscv_vlse16_v_i16mf2(&h0[0].r, sizeof(mimo_c16_t), vl);
        const vint16mf2_t h0i =
            __riscv_vlse16_v_i16mf2(&h0[0].i, sizeof(mimo_c16_t), vl);
        const vint16mf2_t yr =
            __riscv_vlse16_v_i16mf2(&yy[0].r, sizeof(mimo_c16_t), vl);
        const vint16mf2_t yi =
            __riscv_vlse16_v_i16mf2(&yy[0].i, sizeof(mimo_c16_t), vl);

        mimo_rvv_rowfuse_acc_conj_mf2(h0r, h0i, yr, yi, &b0r, &b0i, vl);
        mimo_rvv_rowfuse_acc_real_mf2(h0r, h0i, &a00r, vl);
        {
            const vint16mf2_t h1r =
                __riscv_vlse16_v_i16mf2(&h1[0].r, sizeof(mimo_c16_t), vl);
            const vint16mf2_t h1i =
                __riscv_vlse16_v_i16mf2(&h1[0].i, sizeof(mimo_c16_t), vl);
            mimo_rvv_rowfuse_acc_conj_mf2(h0r, h0i, h1r, h1i, &a01r, &a01i, vl);
        }
        {
            const vint16mf2_t h2r =
                __riscv_vlse16_v_i16mf2(&h2[0].r, sizeof(mimo_c16_t), vl);
            const vint16mf2_t h2i =
                __riscv_vlse16_v_i16mf2(&h2[0].i, sizeof(mimo_c16_t), vl);
            mimo_rvv_rowfuse_acc_conj_mf2(h0r, h0i, h2r, h2i, &a02r, &a02i, vl);
        }
        {
            const vint16mf2_t h3r =
                __riscv_vlse16_v_i16mf2(&h3[0].r, sizeof(mimo_c16_t), vl);
            const vint16mf2_t h3i =
                __riscv_vlse16_v_i16mf2(&h3[0].i, sizeof(mimo_c16_t), vl);
            mimo_rvv_rowfuse_acc_conj_mf2(h0r, h0i, h3r, h3i, &a03r, &a03i, vl);
        }
    }

    __riscv_vse32_v_i32m1(&work->b_re[0][0],
                          mimo_rvv_round_once_dot_out_mf2(b0r, vl), vl);
    __riscv_vse32_v_i32m1(&work->b_im[0][0],
                          mimo_rvv_round_once_dot_out_mf2(b0i, vl), vl);
    __riscv_vse32_v_i32m1(&work->a_re[0][0],
                          mimo_rvv_round_once_dot_out_mf2(a00r, vl), vl);
    __riscv_vse32_v_i32m1(&work->a_re[1][0],
                          mimo_rvv_round_once_dot_out_mf2(a01r, vl), vl);
    __riscv_vse32_v_i32m1(&work->a_im[1][0],
                          mimo_rvv_round_once_dot_out_mf2(a01i, vl), vl);
    __riscv_vse32_v_i32m1(&work->a_re[2][0],
                          mimo_rvv_round_once_dot_out_mf2(a02r, vl), vl);
    __riscv_vse32_v_i32m1(&work->a_im[2][0],
                          mimo_rvv_round_once_dot_out_mf2(a02i, vl), vl);
    __riscv_vse32_v_i32m1(&work->a_re[3][0],
                          mimo_rvv_round_once_dot_out_mf2(a03r, vl), vl);
    __riscv_vse32_v_i32m1(&work->a_im[3][0],
                          mimo_rvv_round_once_dot_out_mf2(a03i, vl), vl);
}

static MIMO_ALWAYS_INLINE void mimo_rvv_build_hermitian4_site4_upper_row1_soa_mf2_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_site4_soa4_t *work)
{
    const size_t vl = __riscv_vsetvl_e16mf2(MIMO_SITE4_LANES);
    const vint32m1_t zero = __riscv_vmv_v_x_i32m1(0, vl);
    vint32m1_t b1r = zero;
    vint32m1_t b1i = zero;
    vint32m1_t a11r = zero;
    vint32m1_t a12r = zero;
    vint32m1_t a12i = zero;
    vint32m1_t a13r = zero;
    vint32m1_t a13i = zero;

    for (int rx = 0; rx < MIMO_N4; ++rx) {
        const mimo_c16_t *h1 = mimo_site4_h4_vec(h_site4, block, 1, rx);
        const mimo_c16_t *h2 = mimo_site4_h4_vec(h_site4, block, 2, rx);
        const mimo_c16_t *h3 = mimo_site4_h4_vec(h_site4, block, 3, rx);
        const mimo_c16_t *yy = mimo_site4_y4_vec(y_site4, block, rx);
        const vint16mf2_t h1r =
            __riscv_vlse16_v_i16mf2(&h1[0].r, sizeof(mimo_c16_t), vl);
        const vint16mf2_t h1i =
            __riscv_vlse16_v_i16mf2(&h1[0].i, sizeof(mimo_c16_t), vl);
        const vint16mf2_t yr =
            __riscv_vlse16_v_i16mf2(&yy[0].r, sizeof(mimo_c16_t), vl);
        const vint16mf2_t yi =
            __riscv_vlse16_v_i16mf2(&yy[0].i, sizeof(mimo_c16_t), vl);

        mimo_rvv_rowfuse_acc_conj_mf2(h1r, h1i, yr, yi, &b1r, &b1i, vl);
        mimo_rvv_rowfuse_acc_real_mf2(h1r, h1i, &a11r, vl);
        {
            const vint16mf2_t h2r =
                __riscv_vlse16_v_i16mf2(&h2[0].r, sizeof(mimo_c16_t), vl);
            const vint16mf2_t h2i =
                __riscv_vlse16_v_i16mf2(&h2[0].i, sizeof(mimo_c16_t), vl);
            mimo_rvv_rowfuse_acc_conj_mf2(h1r, h1i, h2r, h2i, &a12r, &a12i, vl);
        }
        {
            const vint16mf2_t h3r =
                __riscv_vlse16_v_i16mf2(&h3[0].r, sizeof(mimo_c16_t), vl);
            const vint16mf2_t h3i =
                __riscv_vlse16_v_i16mf2(&h3[0].i, sizeof(mimo_c16_t), vl);
            mimo_rvv_rowfuse_acc_conj_mf2(h1r, h1i, h3r, h3i, &a13r, &a13i, vl);
        }
    }

    __riscv_vse32_v_i32m1(&work->b_re[1][0],
                          mimo_rvv_round_once_dot_out_mf2(b1r, vl), vl);
    __riscv_vse32_v_i32m1(&work->b_im[1][0],
                          mimo_rvv_round_once_dot_out_mf2(b1i, vl), vl);
    __riscv_vse32_v_i32m1(&work->a_re[5][0],
                          mimo_rvv_round_once_dot_out_mf2(a11r, vl), vl);
    __riscv_vse32_v_i32m1(&work->a_re[6][0],
                          mimo_rvv_round_once_dot_out_mf2(a12r, vl), vl);
    __riscv_vse32_v_i32m1(&work->a_im[6][0],
                          mimo_rvv_round_once_dot_out_mf2(a12i, vl), vl);
    __riscv_vse32_v_i32m1(&work->a_re[7][0],
                          mimo_rvv_round_once_dot_out_mf2(a13r, vl), vl);
    __riscv_vse32_v_i32m1(&work->a_im[7][0],
                          mimo_rvv_round_once_dot_out_mf2(a13i, vl), vl);
}

static MIMO_ALWAYS_INLINE void mimo_rvv_build_hermitian4_site4_upper_row2_soa_mf2_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_site4_soa4_t *work)
{
    const size_t vl = __riscv_vsetvl_e16mf2(MIMO_SITE4_LANES);
    const vint32m1_t zero = __riscv_vmv_v_x_i32m1(0, vl);
    vint32m1_t b2r = zero;
    vint32m1_t b2i = zero;
    vint32m1_t a22r = zero;
    vint32m1_t a23r = zero;
    vint32m1_t a23i = zero;

    for (int rx = 0; rx < MIMO_N4; ++rx) {
        const mimo_c16_t *h2 = mimo_site4_h4_vec(h_site4, block, 2, rx);
        const mimo_c16_t *h3 = mimo_site4_h4_vec(h_site4, block, 3, rx);
        const mimo_c16_t *yy = mimo_site4_y4_vec(y_site4, block, rx);
        const vint16mf2_t h2r =
            __riscv_vlse16_v_i16mf2(&h2[0].r, sizeof(mimo_c16_t), vl);
        const vint16mf2_t h2i =
            __riscv_vlse16_v_i16mf2(&h2[0].i, sizeof(mimo_c16_t), vl);
        const vint16mf2_t yr =
            __riscv_vlse16_v_i16mf2(&yy[0].r, sizeof(mimo_c16_t), vl);
        const vint16mf2_t yi =
            __riscv_vlse16_v_i16mf2(&yy[0].i, sizeof(mimo_c16_t), vl);

        mimo_rvv_rowfuse_acc_conj_mf2(h2r, h2i, yr, yi, &b2r, &b2i, vl);
        mimo_rvv_rowfuse_acc_real_mf2(h2r, h2i, &a22r, vl);
        {
            const vint16mf2_t h3r =
                __riscv_vlse16_v_i16mf2(&h3[0].r, sizeof(mimo_c16_t), vl);
            const vint16mf2_t h3i =
                __riscv_vlse16_v_i16mf2(&h3[0].i, sizeof(mimo_c16_t), vl);
            mimo_rvv_rowfuse_acc_conj_mf2(h2r, h2i, h3r, h3i, &a23r, &a23i, vl);
        }
    }

    __riscv_vse32_v_i32m1(&work->b_re[2][0],
                          mimo_rvv_round_once_dot_out_mf2(b2r, vl), vl);
    __riscv_vse32_v_i32m1(&work->b_im[2][0],
                          mimo_rvv_round_once_dot_out_mf2(b2i, vl), vl);
    __riscv_vse32_v_i32m1(&work->a_re[10][0],
                          mimo_rvv_round_once_dot_out_mf2(a22r, vl), vl);
    __riscv_vse32_v_i32m1(&work->a_re[11][0],
                          mimo_rvv_round_once_dot_out_mf2(a23r, vl), vl);
    __riscv_vse32_v_i32m1(&work->a_im[11][0],
                          mimo_rvv_round_once_dot_out_mf2(a23i, vl), vl);
}

static MIMO_ALWAYS_INLINE void mimo_rvv_build_hermitian4_site4_upper_row3_soa_mf2_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_site4_soa4_t *work)
{
    const size_t vl = __riscv_vsetvl_e16mf2(MIMO_SITE4_LANES);
    const vint32m1_t zero = __riscv_vmv_v_x_i32m1(0, vl);
    vint32m1_t b3r = zero;
    vint32m1_t b3i = zero;
    vint32m1_t a33r = zero;

    for (int rx = 0; rx < MIMO_N4; ++rx) {
        const mimo_c16_t *h3 = mimo_site4_h4_vec(h_site4, block, 3, rx);
        const mimo_c16_t *yy = mimo_site4_y4_vec(y_site4, block, rx);
        const vint16mf2_t h3r =
            __riscv_vlse16_v_i16mf2(&h3[0].r, sizeof(mimo_c16_t), vl);
        const vint16mf2_t h3i =
            __riscv_vlse16_v_i16mf2(&h3[0].i, sizeof(mimo_c16_t), vl);
        const vint16mf2_t yr =
            __riscv_vlse16_v_i16mf2(&yy[0].r, sizeof(mimo_c16_t), vl);
        const vint16mf2_t yi =
            __riscv_vlse16_v_i16mf2(&yy[0].i, sizeof(mimo_c16_t), vl);

        mimo_rvv_rowfuse_acc_conj_mf2(h3r, h3i, yr, yi, &b3r, &b3i, vl);
        mimo_rvv_rowfuse_acc_real_mf2(h3r, h3i, &a33r, vl);
    }

    __riscv_vse32_v_i32m1(&work->b_re[3][0],
                          mimo_rvv_round_once_dot_out_mf2(b3r, vl), vl);
    __riscv_vse32_v_i32m1(&work->b_im[3][0],
                          mimo_rvv_round_once_dot_out_mf2(b3i, vl), vl);
    __riscv_vse32_v_i32m1(&work->a_re[15][0],
                          mimo_rvv_round_once_dot_out_mf2(a33r, vl), vl);
}

static MIMO_ALWAYS_INLINE void mimo_rvv_build_hermitian4_site4_upper_rowfuse_soa_mf2_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_site4_soa4_t *work)
{
    mimo_rvv_build_hermitian4_site4_upper_row0_soa_mf2_q15(
        h_site4, y_site4, block, work);
    mimo_rvv_build_hermitian4_site4_upper_row1_soa_mf2_q15(
        h_site4, y_site4, block, work);
    mimo_rvv_build_hermitian4_site4_upper_row2_soa_mf2_q15(
        h_site4, y_site4, block, work);
    mimo_rvv_build_hermitian4_site4_upper_row3_soa_mf2_q15(
        h_site4, y_site4, block, work);
}

static MIMO_ALWAYS_INLINE void mimo_rvv_build_hermitian4_site4_upper_row23pair_soa_mf2_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_site4_soa4_t *work)
{
    const size_t vl = __riscv_vsetvl_e16mf2(MIMO_SITE4_LANES);
    const vint32m1_t zero = __riscv_vmv_v_x_i32m1(0, vl);
    vint32m1_t b2r = zero;
    vint32m1_t b2i = zero;
    vint32m1_t a22r = zero;
    vint32m1_t a23r = zero;
    vint32m1_t a23i = zero;
    vint32m1_t b3r = zero;
    vint32m1_t b3i = zero;
    vint32m1_t a33r = zero;

    for (int rx = 0; rx < MIMO_N4; ++rx) {
        const mimo_c16_t *h2 = mimo_site4_h4_vec(h_site4, block, 2, rx);
        const mimo_c16_t *h3 = mimo_site4_h4_vec(h_site4, block, 3, rx);
        const mimo_c16_t *yy = mimo_site4_y4_vec(y_site4, block, rx);
        const vint16mf2_t h2r =
            __riscv_vlse16_v_i16mf2(&h2[0].r, sizeof(mimo_c16_t), vl);
        const vint16mf2_t h2i =
            __riscv_vlse16_v_i16mf2(&h2[0].i, sizeof(mimo_c16_t), vl);
        const vint16mf2_t h3r =
            __riscv_vlse16_v_i16mf2(&h3[0].r, sizeof(mimo_c16_t), vl);
        const vint16mf2_t h3i =
            __riscv_vlse16_v_i16mf2(&h3[0].i, sizeof(mimo_c16_t), vl);
        const vint16mf2_t yr =
            __riscv_vlse16_v_i16mf2(&yy[0].r, sizeof(mimo_c16_t), vl);
        const vint16mf2_t yi =
            __riscv_vlse16_v_i16mf2(&yy[0].i, sizeof(mimo_c16_t), vl);

        mimo_rvv_rowfuse_acc_conj_mf2(h2r, h2i, yr, yi, &b2r, &b2i, vl);
        mimo_rvv_rowfuse_acc_real_mf2(h2r, h2i, &a22r, vl);
        mimo_rvv_rowfuse_acc_conj_mf2(h2r, h2i, h3r, h3i, &a23r, &a23i, vl);
        mimo_rvv_rowfuse_acc_conj_mf2(h3r, h3i, yr, yi, &b3r, &b3i, vl);
        mimo_rvv_rowfuse_acc_real_mf2(h3r, h3i, &a33r, vl);
    }

    __riscv_vse32_v_i32m1(&work->b_re[2][0],
                          mimo_rvv_round_once_dot_out_mf2(b2r, vl), vl);
    __riscv_vse32_v_i32m1(&work->b_im[2][0],
                          mimo_rvv_round_once_dot_out_mf2(b2i, vl), vl);
    __riscv_vse32_v_i32m1(&work->a_re[10][0],
                          mimo_rvv_round_once_dot_out_mf2(a22r, vl), vl);
    __riscv_vse32_v_i32m1(&work->a_re[11][0],
                          mimo_rvv_round_once_dot_out_mf2(a23r, vl), vl);
    __riscv_vse32_v_i32m1(&work->a_im[11][0],
                          mimo_rvv_round_once_dot_out_mf2(a23i, vl), vl);
    __riscv_vse32_v_i32m1(&work->b_re[3][0],
                          mimo_rvv_round_once_dot_out_mf2(b3r, vl), vl);
    __riscv_vse32_v_i32m1(&work->b_im[3][0],
                          mimo_rvv_round_once_dot_out_mf2(b3i, vl), vl);
    __riscv_vse32_v_i32m1(&work->a_re[15][0],
                          mimo_rvv_round_once_dot_out_mf2(a33r, vl), vl);
}

static MIMO_ALWAYS_INLINE void mimo_rvv_build_hermitian4_site4_upper_rowpair_soa_mf2_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_site4_soa4_t *work)
{
    mimo_rvv_build_hermitian4_site4_upper_row0_soa_mf2_q15(
        h_site4, y_site4, block, work);
    mimo_rvv_build_hermitian4_site4_upper_row1_soa_mf2_q15(
        h_site4, y_site4, block, work);
    mimo_rvv_build_hermitian4_site4_upper_row23pair_soa_mf2_q15(
        h_site4, y_site4, block, work);
}

#if MIMO_USE_RVV_ZF4_FUSED_BUILD_SOLVE
static MIMO_ALWAYS_INLINE void mimo_rvv_eq4_build_solve_fused_staged_mf2_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    int mmse,
    mimo_c16_t *x_block)
{
#if MIMO_USE_RVV_MMSE4_CONST_VL
    const size_t vl = MIMO_SITE4_LANES;
#else
    const size_t vl = __riscv_vsetvl_e16mf2(MIMO_SITE4_LANES);
#endif
#if MIMO_USE_RVV_MMSE4_RI_PACKED_LOAD == 2 || \
    MIMO_USE_RVV_MMSE4_RI_PACKED_LOAD == 4 || \
    MIMO_USE_RVV_MMSE4_RI_PACKED_LOAD == 5 || \
    MIMO_USE_RVV_MMSE4_RI_PACKED_LOAD == 7
    const mimo_c16_t *h0_seed = mimo_site4_h4_vec(h_site4, block, 0, 0);
    const mimo_c16_t *h1_seed = mimo_site4_h4_vec(h_site4, block, 1, 0);
    const mimo_c16_t *h2_seed = mimo_site4_h4_vec(h_site4, block, 2, 0);
    const mimo_c16_t *h3_seed = mimo_site4_h4_vec(h_site4, block, 3, 0);
    const mimo_c16_t *y_seed = mimo_site4_y4_vec(y_site4, block, 0);
    vint32m1_t b0r, b0i, b1r, b1i;
    vint32m1_t a00r, a01r, a01i, a02r, a02i, a03r, a03i;
    vint32m1_t a11r, a12r, a12i, a13r, a13i;

#if MIMO_USE_RVV_MMSE4_RI_PACKED_LOAD == 4 || \
    MIMO_USE_RVV_MMSE4_RI_PACKED_LOAD == 5 || \
    MIMO_USE_RVV_MMSE4_RI_PACKED_LOAD == 7
    mimo_rvv_mmse4_build_head_ri_packed_m2_asm(
#else
    mimo_rvv_mmse4_build_head_ri_packed_asm(
#endif
        h0_seed, h1_seed, h2_seed, h3_seed, y_seed,
        &b0r, &b0i, &b1r, &b1i,
        &a00r, &a01r, &a01i, &a02r, &a02i, &a03r, &a03i,
        &a11r, &a12r, &a12i, &a13r, &a13i);
#else
#if MIMO_USE_RVV_MMSE4_SEED_ACC || MIMO_USE_RVV_ZF4_SEED_ACC
    const mimo_c16_t *h0_seed = mimo_site4_h4_vec(h_site4, block, 0, 0);
    const mimo_c16_t *h1_seed = mimo_site4_h4_vec(h_site4, block, 1, 0);
    const mimo_c16_t *h2_seed = mimo_site4_h4_vec(h_site4, block, 2, 0);
    const mimo_c16_t *h3_seed = mimo_site4_h4_vec(h_site4, block, 3, 0);
    const mimo_c16_t *y_seed = mimo_site4_y4_vec(y_site4, block, 0);
    MIMO_RVV_STAGED_LOAD_RI5_MF2(
        h0r_seed, h0i_seed, h1r_seed, h1i_seed,
        h2r_seed, h2i_seed, h3r_seed, h3i_seed, yr_seed, yi_seed,
        h0_seed, h1_seed, h2_seed, h3_seed, y_seed);

    vint32m1_t b0r, b0i, b1r, b1i;
    vint32m1_t a00r, a01r, a01i, a02r, a02i, a03r, a03i;
    vint32m1_t a11r, a12r, a12i, a13r, a13i;
    mimo_rvv_rowfuse_seed_conj_mf2(h0r_seed, h0i_seed, yr_seed, yi_seed,
                                   &b0r, &b0i, vl);
    mimo_rvv_rowfuse_seed_real_mf2(h0r_seed, h0i_seed, &a00r, vl);
    mimo_rvv_rowfuse_seed_conj_mf2(h0r_seed, h0i_seed, h1r_seed, h1i_seed,
                                   &a01r, &a01i, vl);
    mimo_rvv_rowfuse_seed_conj_mf2(h0r_seed, h0i_seed, h2r_seed, h2i_seed,
                                   &a02r, &a02i, vl);
    mimo_rvv_rowfuse_seed_conj_mf2(h0r_seed, h0i_seed, h3r_seed, h3i_seed,
                                   &a03r, &a03i, vl);
    mimo_rvv_rowfuse_seed_conj_mf2(h1r_seed, h1i_seed, yr_seed, yi_seed,
                                   &b1r, &b1i, vl);
    mimo_rvv_rowfuse_seed_real_mf2(h1r_seed, h1i_seed, &a11r, vl);
    mimo_rvv_rowfuse_seed_conj_mf2(h1r_seed, h1i_seed, h2r_seed, h2i_seed,
                                   &a12r, &a12i, vl);
    mimo_rvv_rowfuse_seed_conj_mf2(h1r_seed, h1i_seed, h3r_seed, h3i_seed,
                                   &a13r, &a13i, vl);
#if MIMO_USE_RVV_MMSE4_EARLY_TAIL_RHS
    vint32m1_t b2r, b2i, b3r, b3i;
    mimo_rvv_rowfuse_seed_conj_mf2(h2r_seed, h2i_seed, yr_seed, yi_seed,
                                   &b2r, &b2i, vl);
    mimo_rvv_rowfuse_seed_conj_mf2(h3r_seed, h3i_seed, yr_seed, yi_seed,
                                   &b3r, &b3i, vl);
#endif
    for (int rx = 1; rx < MIMO_N4; ++rx) {
#else
    vint32m1_t zero = __riscv_vmv_v_x_i32m1(0, vl);
    vint32m1_t b0r = zero;
    vint32m1_t b0i = zero;
    vint32m1_t b1r = zero;
    vint32m1_t b1i = zero;
#if MIMO_USE_RVV_MMSE4_EARLY_TAIL_RHS
    vint32m1_t b2r = zero;
    vint32m1_t b2i = zero;
    vint32m1_t b3r = zero;
    vint32m1_t b3i = zero;
#endif
    vint32m1_t a00r = zero;
    vint32m1_t a01r = zero;
    vint32m1_t a01i = zero;
    vint32m1_t a02r = zero;
    vint32m1_t a02i = zero;
    vint32m1_t a03r = zero;
    vint32m1_t a03i = zero;
    vint32m1_t a11r = zero;
    vint32m1_t a12r = zero;
    vint32m1_t a12i = zero;
    vint32m1_t a13r = zero;
    vint32m1_t a13i = zero;

    for (int rx = 0; rx < MIMO_N4; ++rx) {
#endif
        const mimo_c16_t *h0 = mimo_site4_h4_vec(h_site4, block, 0, rx);
        const mimo_c16_t *h1 = mimo_site4_h4_vec(h_site4, block, 1, rx);
        const mimo_c16_t *h2 = mimo_site4_h4_vec(h_site4, block, 2, rx);
        const mimo_c16_t *h3 = mimo_site4_h4_vec(h_site4, block, 3, rx);
        const mimo_c16_t *yy = mimo_site4_y4_vec(y_site4, block, rx);
        MIMO_RVV_STAGED_LOAD_RI5_MF2(
            h0r, h0i, h1r, h1i, h2r, h2i, h3r, h3i, yr, yi,
            h0, h1, h2, h3, yy);

        mimo_rvv_rowfuse_acc_conj_mf2(h0r, h0i, yr, yi, &b0r, &b0i, vl);
#if MIMO_USE_RVV_MMSE4_EARLY_TAIL_RHS
        mimo_rvv_rowfuse_acc_conj_mf2(h2r, h2i, yr, yi, &b2r, &b2i, vl);
        mimo_rvv_rowfuse_acc_conj_mf2(h3r, h3i, yr, yi, &b3r, &b3i, vl);
#endif
        mimo_rvv_rowfuse_acc_real_mf2(h0r, h0i, &a00r, vl);
        mimo_rvv_rowfuse_acc_conj_mf2(h0r, h0i, h1r, h1i, &a01r, &a01i, vl);
        mimo_rvv_rowfuse_acc_conj_mf2(h0r, h0i, h2r, h2i, &a02r, &a02i, vl);
        mimo_rvv_rowfuse_acc_conj_mf2(h0r, h0i, h3r, h3i, &a03r, &a03i, vl);

        mimo_rvv_rowfuse_acc_conj_mf2(h1r, h1i, yr, yi, &b1r, &b1i, vl);
        mimo_rvv_rowfuse_acc_real_mf2(h1r, h1i, &a11r, vl);
        mimo_rvv_rowfuse_acc_conj_mf2(h1r, h1i, h2r, h2i, &a12r, &a12i, vl);
        mimo_rvv_rowfuse_acc_conj_mf2(h1r, h1i, h3r, h3i, &a13r, &a13i, vl);
    }
#endif

#if MIMO_USE_RVV_SITE4_VSSRA_ROUND == 2
    MIMO_RVV_MMSE4_VSSRA_HEAD16_INPLACE(
        b0r, b0i, b1r, b1i, a00r, a01r, a01i, a02r, a02i,
        a03r, a03i, a11r, a12r, a12i, a13r, a13i);
#else
    b0r = mimo_rvv_round_once_dot_out_mf2(b0r, vl);
    b0i = mimo_rvv_round_once_dot_out_mf2(b0i, vl);
    b1r = mimo_rvv_round_once_dot_out_mf2(b1r, vl);
    b1i = mimo_rvv_round_once_dot_out_mf2(b1i, vl);
#if MIMO_USE_RVV_MMSE4_EARLY_TAIL_RHS
    b2r = mimo_rvv_round_once_dot_out_mf2(b2r, vl);
    b2i = mimo_rvv_round_once_dot_out_mf2(b2i, vl);
    b3r = mimo_rvv_round_once_dot_out_mf2(b3r, vl);
    b3i = mimo_rvv_round_once_dot_out_mf2(b3i, vl);
#endif
    a00r = mimo_rvv_round_once_dot_out_mf2(a00r, vl);
    a01r = mimo_rvv_round_once_dot_out_mf2(a01r, vl);
    a01i = mimo_rvv_round_once_dot_out_mf2(a01i, vl);
    a02r = mimo_rvv_round_once_dot_out_mf2(a02r, vl);
    a02i = mimo_rvv_round_once_dot_out_mf2(a02i, vl);
    a03r = mimo_rvv_round_once_dot_out_mf2(a03r, vl);
    a03i = mimo_rvv_round_once_dot_out_mf2(a03i, vl);
    a11r = mimo_rvv_round_once_dot_out_mf2(a11r, vl);
    a12r = mimo_rvv_round_once_dot_out_mf2(a12r, vl);
    a12i = mimo_rvv_round_once_dot_out_mf2(a12i, vl);
    a13r = mimo_rvv_round_once_dot_out_mf2(a13r, vl);
    a13i = mimo_rvv_round_once_dot_out_mf2(a13i, vl);
#endif

    if (mmse) {
        a00r = __riscv_vadd_vx_i32m1(a00r, MIMO_MMSE_LAMBDA_Q15, vl);
        a11r = __riscv_vadd_vx_i32m1(a11r, MIMO_MMSE_LAMBDA_Q15, vl);
    }

    const vint32m1_t b_det_q30 =
        mimo_rvv_schur4_det_q30_i32(a00r, a01r, a01i, a11r, vl);
    const vint32m1_t b_scale =
        mimo_rvv_schur4_scale_from_det_q30_i32(b_det_q30, vl);
    vint32m1_t w00r, w00i, w10r, w10i;
    vint32m1_t w01r, w01i, w11r, w11i;
    vint32m1_t y0r, y0i, y1r, y1i;

#if MIMO_USE_RVV_MMSE4_BINV_Q13_REUSE
    const vint32m1_t inv00 =
        mimo_rvv_schur4_inv2_coeff_q13_i32(a11r, b_scale, vl);
    const vint32m1_t inv01r = __riscv_vrsub_vx_i32m1(
        mimo_rvv_schur4_inv2_coeff_q13_i32(a01r, b_scale, vl), 0, vl);
    const vint32m1_t inv01i = __riscv_vrsub_vx_i32m1(
        mimo_rvv_schur4_inv2_coeff_q13_i32(a01i, b_scale, vl), 0, vl);
    const vint32m1_t inv11 =
        mimo_rvv_schur4_inv2_coeff_q13_i32(a00r, b_scale, vl);

    mimo_rvv_schur4_apply_inv2_q13_i32(
        inv00, inv01r, inv01i, inv11,
        a02r, a02i, a12r, a12i, &w00r, &w00i, &w10r, &w10i, vl);
    mimo_rvv_schur4_apply_inv2_q13_i32(
        inv00, inv01r, inv01i, inv11,
        a03r, a03i, a13r, a13i, &w01r, &w01i, &w11r, &w11i, vl);
    mimo_rvv_schur4_apply_inv2_q13_i32(
        inv00, inv01r, inv01i, inv11,
        b0r, b0i, b1r, b1i, &y0r, &y0i, &y1r, &y1i, vl);
#else
#if MIMO_USE_RVV_SCHUR4_SOLVE2_PAIR_M2
    mimo_rvv_schur4_solve2_pair_m2_i32(
        a00r, a01r, a01i, a11r, b_scale,
        a02r, a02i, a12r, a12i,
        a03r, a03i, a13r, a13i,
        &w00r, &w00i, &w10r, &w10i,
        &w01r, &w01i, &w11r, &w11i,
        MIMO_MMSE4_SCHUR_SCALE_ARG(mmse), vl);
#else
#if MIMO_USE_RVV_MMSE4_SCHUR_TRUE16B_SHARED_A16
    const vint16mf2_t b_a00_16 =
        __riscv_vnclip_wx_i16mf2(a00r, 0, 0, vl);
    const vint16mf2_t b_a01r_16 =
        __riscv_vnclip_wx_i16mf2(a01r, 0, 0, vl);
    const vint16mf2_t b_a01i_16 =
        __riscv_vnclip_wx_i16mf2(a01i, 0, 0, vl);
    const vint16mf2_t b_a11_16 =
        __riscv_vnclip_wx_i16mf2(a11r, 0, 0, vl);
    mimo_rvv_schur4_solve2_true16b_a16_i32(
        b_a00_16, b_a01r_16, b_a01i_16, b_a11_16, b_scale,
        a02r, a02i, a12r, a12i, &w00r, &w00i, &w10r, &w10i,
        MIMO_MMSE4_SCHUR_SCALE_ARG(mmse), vl);
#elif MIMO_USE_RVV_MMSE4_SCHUR_TRUE16B_SOLVE
    mimo_rvv_schur4_solve2_true16b_i32(
        a00r, a01r, a01i, a11r, b_scale,
        a02r, a02i, a12r, a12i, &w00r, &w00i, &w10r, &w10i,
        MIMO_MMSE4_SCHUR_SCALE_ARG(mmse), vl);
#else
    mimo_rvv_schur4_solve2_i32(
        a00r, a01r, a01i, a11r, b_scale,
        a02r, a02i, a12r, a12i, &w00r, &w00i, &w10r, &w10i,
        MIMO_MMSE4_SCHUR_SCALE_ARG(mmse), vl);
#endif
#if MIMO_USE_RVV_MMSE4_SCHUR_TRUE16B_SHARED_A16
    mimo_rvv_schur4_solve2_true16b_a16_i32(
        b_a00_16, b_a01r_16, b_a01i_16, b_a11_16, b_scale,
        a03r, a03i, a13r, a13i, &w01r, &w01i, &w11r, &w11i,
        MIMO_MMSE4_SCHUR_SCALE_ARG(mmse), vl);
#elif MIMO_USE_RVV_MMSE4_SCHUR_TRUE16B_SOLVE
    mimo_rvv_schur4_solve2_true16b_i32(
        a00r, a01r, a01i, a11r, b_scale,
        a03r, a03i, a13r, a13i, &w01r, &w01i, &w11r, &w11i,
        MIMO_MMSE4_SCHUR_SCALE_ARG(mmse), vl);
#else
    mimo_rvv_schur4_solve2_i32(
        a00r, a01r, a01i, a11r, b_scale,
        a03r, a03i, a13r, a13i, &w01r, &w01i, &w11r, &w11i,
        MIMO_MMSE4_SCHUR_SCALE_ARG(mmse), vl);
#endif
#endif
#if MIMO_USE_RVV_MMSE4_SCHUR_TRUE16B_SHARED_A16
    mimo_rvv_schur4_solve2_true16b_a16_i32(
        b_a00_16, b_a01r_16, b_a01i_16, b_a11_16, b_scale,
        b0r, b0i, b1r, b1i, &y0r, &y0i, &y1r, &y1i,
        MIMO_MMSE4_SCHUR_SCALE_ARG(mmse), vl);
#elif MIMO_USE_RVV_MMSE4_SCHUR_TRUE16B_SOLVE
    mimo_rvv_schur4_solve2_true16b_i32(
        a00r, a01r, a01i, a11r, b_scale,
        b0r, b0i, b1r, b1i, &y0r, &y0i, &y1r, &y1i,
        MIMO_MMSE4_SCHUR_SCALE_ARG(mmse), vl);
#else
    mimo_rvv_schur4_solve2_i32(
        a00r, a01r, a01i, a11r, b_scale,
        b0r, b0i, b1r, b1i, &y0r, &y0i, &y1r, &y1i,
        MIMO_MMSE4_SCHUR_SCALE_ARG(mmse), vl);
#endif
#endif

#if MIMO_USE_RVV_MMSE4_RI_PACKED_LOAD == 4 || \
    MIMO_USE_RVV_MMSE4_RI_PACKED_LOAD == 6
    const mimo_c16_t *h2_tail = mimo_site4_h4_vec(h_site4, block, 2, 0);
    const mimo_c16_t *h3_tail = mimo_site4_h4_vec(h_site4, block, 3, 0);
    const mimo_c16_t *y_tail = mimo_site4_y4_vec(y_site4, block, 0);
    vint32m1_t b2r, b2i, b3r, b3i;
    vint32m1_t a22r, a23r, a23i, a33r;

    mimo_rvv_mmse4_build_tail_ri_packed_m2_asm(
        h2_tail, h3_tail, y_tail,
        w00r, w00i, w10r, w10i, w01r, w01i, w11r, w11i,
        y0r, y0i, y1r, y1i,
        &b2r, &b2i, &b3r, &b3i, &a22r, &a23r, &a23i, &a33r);
#elif MIMO_USE_RVV_MMSE4_RI_PACKED_LOAD == 7
    const mimo_c16_t *h2_tail = mimo_site4_h4_vec(h_site4, block, 2, 0);
    const mimo_c16_t *h3_tail = mimo_site4_h4_vec(h_site4, block, 3, 0);
    const mimo_c16_t *y_tail = mimo_site4_y4_vec(y_site4, block, 0);
    vint32m1_t b2r, b2i, b3r, b3i;
    vint32m1_t a22r, a23r, a23i, a33r;

    mimo_rvv_mmse4_build_tail_ri_packed_m1_asm(
        h2_tail, h3_tail, y_tail,
        &b2r, &b2i, &b3r, &b3i, &a22r, &a23r, &a23i, &a33r);
#else
#if !MIMO_USE_RVV_MMSE4_EARLY_TAIL_RHS
#if MIMO_USE_RVV_MMSE4_SEED_ACC || MIMO_USE_RVV_ZF4_SEED_ACC
    const mimo_c16_t *h2_tail_seed =
        mimo_site4_h4_vec(h_site4, block, 2, 0);
    const mimo_c16_t *h3_tail_seed =
        mimo_site4_h4_vec(h_site4, block, 3, 0);
    const mimo_c16_t *y_tail_seed = mimo_site4_y4_vec(y_site4, block, 0);
    MIMO_RVV_STAGED_LOAD_RI_MF2(
        h2r_tail_seed, h2i_tail_seed, h2_tail_seed, vl);
    MIMO_RVV_STAGED_LOAD_RI_MF2(
        h3r_tail_seed, h3i_tail_seed, h3_tail_seed, vl);
    MIMO_RVV_STAGED_LOAD_RI_MF2(
        yr_tail_seed, yi_tail_seed, y_tail_seed, vl);
    vint32m1_t b2r, b2i, b3r, b3i;
    mimo_rvv_rowfuse_seed_conj_mf2(h2r_tail_seed, h2i_tail_seed,
                                   yr_tail_seed, yi_tail_seed,
                                   &b2r, &b2i, vl);
    mimo_rvv_rowfuse_seed_conj_mf2(h3r_tail_seed, h3i_tail_seed,
                                   yr_tail_seed, yi_tail_seed,
                                   &b3r, &b3i, vl);
#else
    vint32m1_t b2r = zero;
    vint32m1_t b2i = zero;
    vint32m1_t b3r = zero;
    vint32m1_t b3i = zero;
#endif
#endif
#if MIMO_USE_RVV_MMSE4_SEED_ACC || MIMO_USE_RVV_ZF4_SEED_ACC
    const mimo_c16_t *h2_s_seed = mimo_site4_h4_vec(h_site4, block, 2, 0);
    const mimo_c16_t *h3_s_seed = mimo_site4_h4_vec(h_site4, block, 3, 0);
    MIMO_RVV_STAGED_LOAD_RI_MF2(h2r_s_seed, h2i_s_seed, h2_s_seed, vl);
    MIMO_RVV_STAGED_LOAD_RI_MF2(h3r_s_seed, h3i_s_seed, h3_s_seed, vl);
    vint32m1_t a22r, a23r, a23i, a33r;
    mimo_rvv_rowfuse_seed_real_mf2(h2r_s_seed, h2i_s_seed, &a22r, vl);
    mimo_rvv_rowfuse_seed_conj_mf2(h2r_s_seed, h2i_s_seed,
                                   h3r_s_seed, h3i_s_seed,
                                   &a23r, &a23i, vl);
    mimo_rvv_rowfuse_seed_real_mf2(h3r_s_seed, h3i_s_seed, &a33r, vl);
    for (int rx = 1; rx < MIMO_N4; ++rx) {
#else
    vint32m1_t a22r = zero;
    vint32m1_t a23r = zero;
    vint32m1_t a23i = zero;
    vint32m1_t a33r = zero;

    for (int rx = 0; rx < MIMO_N4; ++rx) {
#endif
        const mimo_c16_t *h2 = mimo_site4_h4_vec(h_site4, block, 2, rx);
        const mimo_c16_t *h3 = mimo_site4_h4_vec(h_site4, block, 3, rx);
#if !MIMO_USE_RVV_MMSE4_EARLY_TAIL_RHS
        const mimo_c16_t *yy = mimo_site4_y4_vec(y_site4, block, rx);
#endif
        MIMO_RVV_STAGED_LOAD_RI_MF2(h2r, h2i, h2, vl);
        MIMO_RVV_STAGED_LOAD_RI_MF2(h3r, h3i, h3, vl);
#if !MIMO_USE_RVV_MMSE4_EARLY_TAIL_RHS
        MIMO_RVV_STAGED_LOAD_RI_MF2(yr, yi, yy, vl);
#endif
#if !MIMO_USE_RVV_MMSE4_EARLY_TAIL_RHS
        mimo_rvv_rowfuse_acc_conj_mf2(h2r, h2i, yr, yi, &b2r, &b2i, vl);
#endif
        mimo_rvv_rowfuse_acc_real_mf2(h2r, h2i, &a22r, vl);
        mimo_rvv_rowfuse_acc_conj_mf2(h2r, h2i, h3r, h3i, &a23r, &a23i, vl);
#if !MIMO_USE_RVV_MMSE4_EARLY_TAIL_RHS
        mimo_rvv_rowfuse_acc_conj_mf2(h3r, h3i, yr, yi, &b3r, &b3i, vl);
#endif
        mimo_rvv_rowfuse_acc_real_mf2(h3r, h3i, &a33r, vl);
    }
#endif

#if MIMO_USE_RVV_SITE4_VSSRA_ROUND == 2
    MIMO_RVV_MMSE4_VSSRA_TAIL8_INPLACE(
        b2r, b2i, b3r, b3i, a22r, a23r, a23i, a33r);
#else
#if !MIMO_USE_RVV_MMSE4_EARLY_TAIL_RHS
    b2r = mimo_rvv_round_once_dot_out_mf2(b2r, vl);
    b2i = mimo_rvv_round_once_dot_out_mf2(b2i, vl);
    b3r = mimo_rvv_round_once_dot_out_mf2(b3r, vl);
    b3i = mimo_rvv_round_once_dot_out_mf2(b3i, vl);
#endif
    a22r = mimo_rvv_round_once_dot_out_mf2(a22r, vl);
    a23r = mimo_rvv_round_once_dot_out_mf2(a23r, vl);
    a23i = mimo_rvv_round_once_dot_out_mf2(a23i, vl);
    a33r = mimo_rvv_round_once_dot_out_mf2(a33r, vl);
#endif

    if (mmse) {
        a22r = __riscv_vadd_vx_i32m1(a22r, MIMO_MMSE_LAMBDA_Q15, vl);
        a33r = __riscv_vadd_vx_i32m1(a33r, MIMO_MMSE_LAMBDA_Q15, vl);
    }

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

#if MIMO_USE_RVV_MMSE4_SCHUR_TRUE16B_SOLVE
    mimo_rvv_schur4_solve2_true16b_i32(
        s00r, s01r, s01i, s11r, s_scale,
        r0r, r0i, r1r, r1i, &x2r, &x2i, &x3r, &x3i,
        MIMO_MMSE4_SCHUR_SCALE_ARG(mmse), vl);
#else
    mimo_rvv_schur4_solve2_i32(
        s00r, s01r, s01i, s11r, s_scale,
        r0r, r0i, r1r, r1i, &x2r, &x2i, &x3r, &x3i,
        MIMO_MMSE4_SCHUR_SCALE_ARG(mmse), vl);
#endif

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

    const ptrdiff_t x_stride = (ptrdiff_t)(MIMO_N4 * sizeof(mimo_c16_t));
    mimo_rvv_solve4_store_x(x_block, 0, top0r, top0i, x_stride, vl);
    mimo_rvv_solve4_store_x(x_block, 1, top1r, top1i, x_stride, vl);
    mimo_rvv_solve4_store_x(x_block, 2, x2r, x2i, x_stride, vl);
    mimo_rvv_solve4_store_x(x_block, 3, x3r, x3i, x_stride, vl);
}

#undef MIMO_RVV_STAGED_LOAD_RI_MF2
#undef MIMO_RVV_STAGED_LOAD_RI5_MF2
#if MIMO_USE_RVV_SITE4_VSSRA_ROUND == 2
#undef MIMO_RVV_MMSE4_VSSRA_HEAD16_INPLACE
#undef MIMO_RVV_MMSE4_VSSRA_TAIL8_INPLACE
#endif

static MIMO_ALWAYS_INLINE void mimo_rvv_eq4_build_solve_fused_rowfuse_mf2_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    int mmse,
    mimo_c16_t *x_block)
{
#if MIMO_USE_RVV_MMSE4_STAGED_BUILD_SOLVE || MIMO_USE_RVV_ZF4_STAGED_BUILD_SOLVE
    if ((mmse && MIMO_USE_RVV_MMSE4_STAGED_BUILD_SOLVE) ||
        (!mmse && MIMO_USE_RVV_ZF4_STAGED_BUILD_SOLVE)) {
        mimo_rvv_eq4_build_solve_fused_staged_mf2_q15(
            h_site4, y_site4, block, mmse, x_block);
        return;
    }
#endif

    const size_t vl = __riscv_vsetvl_e16mf2(MIMO_SITE4_LANES);
#if MIMO_USE_RVV_ZF4_SEED_ACC
    const mimo_c16_t *h0_seed = mimo_site4_h4_vec(h_site4, block, 0, 0);
    const mimo_c16_t *h1_seed = mimo_site4_h4_vec(h_site4, block, 1, 0);
    const mimo_c16_t *h2_seed = mimo_site4_h4_vec(h_site4, block, 2, 0);
    const mimo_c16_t *h3_seed = mimo_site4_h4_vec(h_site4, block, 3, 0);
    const mimo_c16_t *y_seed = mimo_site4_y4_vec(y_site4, block, 0);
    const vint16mf2_t h0r_seed = mimo_rvv_site4_load_re_mf2(h0_seed, vl);
    const vint16mf2_t h0i_seed = mimo_rvv_site4_load_im_mf2(h0_seed, vl);
    const vint16mf2_t h1r_seed = mimo_rvv_site4_load_re_mf2(h1_seed, vl);
    const vint16mf2_t h1i_seed = mimo_rvv_site4_load_im_mf2(h1_seed, vl);
    const vint16mf2_t h2r_seed = mimo_rvv_site4_load_re_mf2(h2_seed, vl);
    const vint16mf2_t h2i_seed = mimo_rvv_site4_load_im_mf2(h2_seed, vl);
    const vint16mf2_t h3r_seed = mimo_rvv_site4_load_re_mf2(h3_seed, vl);
    const vint16mf2_t h3i_seed = mimo_rvv_site4_load_im_mf2(h3_seed, vl);
    const vint16mf2_t yr_seed = mimo_rvv_site4_load_re_mf2(y_seed, vl);
    const vint16mf2_t yi_seed = mimo_rvv_site4_load_im_mf2(y_seed, vl);
    vint32m1_t b0r, b0i, b1r, b1i, b2r, b2i, b3r, b3i;
    vint32m1_t a00r, a01r, a01i, a02r, a02i, a03r, a03i;
    vint32m1_t a11r, a12r, a12i, a13r, a13i;
    vint32m1_t a22r, a23r, a23i, a33r;

    mimo_rvv_rowfuse_seed_conj_mf2(h0r_seed, h0i_seed, yr_seed, yi_seed,
                                   &b0r, &b0i, vl);
    mimo_rvv_rowfuse_seed_conj_mf2(h1r_seed, h1i_seed, yr_seed, yi_seed,
                                   &b1r, &b1i, vl);
    mimo_rvv_rowfuse_seed_conj_mf2(h2r_seed, h2i_seed, yr_seed, yi_seed,
                                   &b2r, &b2i, vl);
    mimo_rvv_rowfuse_seed_conj_mf2(h3r_seed, h3i_seed, yr_seed, yi_seed,
                                   &b3r, &b3i, vl);
    mimo_rvv_rowfuse_seed_real_mf2(h0r_seed, h0i_seed, &a00r, vl);
    mimo_rvv_rowfuse_seed_conj_mf2(h0r_seed, h0i_seed, h1r_seed, h1i_seed,
                                   &a01r, &a01i, vl);
    mimo_rvv_rowfuse_seed_conj_mf2(h0r_seed, h0i_seed, h2r_seed, h2i_seed,
                                   &a02r, &a02i, vl);
    mimo_rvv_rowfuse_seed_conj_mf2(h0r_seed, h0i_seed, h3r_seed, h3i_seed,
                                   &a03r, &a03i, vl);
    mimo_rvv_rowfuse_seed_real_mf2(h1r_seed, h1i_seed, &a11r, vl);
    mimo_rvv_rowfuse_seed_conj_mf2(h1r_seed, h1i_seed, h2r_seed, h2i_seed,
                                   &a12r, &a12i, vl);
    mimo_rvv_rowfuse_seed_conj_mf2(h1r_seed, h1i_seed, h3r_seed, h3i_seed,
                                   &a13r, &a13i, vl);
    mimo_rvv_rowfuse_seed_real_mf2(h2r_seed, h2i_seed, &a22r, vl);
    mimo_rvv_rowfuse_seed_conj_mf2(h2r_seed, h2i_seed, h3r_seed, h3i_seed,
                                   &a23r, &a23i, vl);
    mimo_rvv_rowfuse_seed_real_mf2(h3r_seed, h3i_seed, &a33r, vl);

    for (int rx = 1; rx < MIMO_N4; ++rx) {
#else
    vint32m1_t b0r = __riscv_vmv_v_x_i32m1(0, vl);
    vint32m1_t b0i = b0r;
    vint32m1_t b1r = b0r;
    vint32m1_t b1i = b0r;
    vint32m1_t b2r = b0r;
    vint32m1_t b2i = b0r;
    vint32m1_t b3r = b0r;
    vint32m1_t b3i = b0r;
    vint32m1_t a00r = b0r;
    vint32m1_t a01r = b0r;
    vint32m1_t a01i = b0r;
    vint32m1_t a02r = b0r;
    vint32m1_t a02i = b0r;
    vint32m1_t a03r = b0r;
    vint32m1_t a03i = b0r;
    vint32m1_t a11r = b0r;
    vint32m1_t a12r = b0r;
    vint32m1_t a12i = b0r;
    vint32m1_t a13r = b0r;
    vint32m1_t a13i = b0r;
    vint32m1_t a22r = b0r;
    vint32m1_t a23r = b0r;
    vint32m1_t a23i = b0r;
    vint32m1_t a33r = b0r;

    for (int rx = 0; rx < MIMO_N4; ++rx) {
#endif
        const mimo_c16_t *h0 = mimo_site4_h4_vec(h_site4, block, 0, rx);
        const mimo_c16_t *h1 = mimo_site4_h4_vec(h_site4, block, 1, rx);
        const mimo_c16_t *h2 = mimo_site4_h4_vec(h_site4, block, 2, rx);
        const mimo_c16_t *h3 = mimo_site4_h4_vec(h_site4, block, 3, rx);
        const mimo_c16_t *yy = mimo_site4_y4_vec(y_site4, block, rx);
#if MIMO_USE_RVV_SITE4_SEG2_LOAD
        const vint16mf2x2_t h0ri = __riscv_vlseg2e16_v_i16mf2x2(
            (const int16_t *)h0, vl);
        const vint16mf2x2_t h1ri = __riscv_vlseg2e16_v_i16mf2x2(
            (const int16_t *)h1, vl);
        const vint16mf2x2_t h2ri = __riscv_vlseg2e16_v_i16mf2x2(
            (const int16_t *)h2, vl);
        const vint16mf2x2_t h3ri = __riscv_vlseg2e16_v_i16mf2x2(
            (const int16_t *)h3, vl);
        const vint16mf2x2_t yri = __riscv_vlseg2e16_v_i16mf2x2(
            (const int16_t *)yy, vl);
        const vint16mf2_t h0r =
            __riscv_vget_v_i16mf2x2_i16mf2(h0ri, 0);
        const vint16mf2_t h0i =
            __riscv_vget_v_i16mf2x2_i16mf2(h0ri, 1);
        const vint16mf2_t h1r =
            __riscv_vget_v_i16mf2x2_i16mf2(h1ri, 0);
        const vint16mf2_t h1i =
            __riscv_vget_v_i16mf2x2_i16mf2(h1ri, 1);
        const vint16mf2_t h2r =
            __riscv_vget_v_i16mf2x2_i16mf2(h2ri, 0);
        const vint16mf2_t h2i =
            __riscv_vget_v_i16mf2x2_i16mf2(h2ri, 1);
        const vint16mf2_t h3r =
            __riscv_vget_v_i16mf2x2_i16mf2(h3ri, 0);
        const vint16mf2_t h3i =
            __riscv_vget_v_i16mf2x2_i16mf2(h3ri, 1);
        const vint16mf2_t yr =
            __riscv_vget_v_i16mf2x2_i16mf2(yri, 0);
        const vint16mf2_t yi =
            __riscv_vget_v_i16mf2x2_i16mf2(yri, 1);
#else
        const vint16mf2_t h0r = mimo_rvv_site4_load_re_mf2(h0, vl);
        const vint16mf2_t h0i = mimo_rvv_site4_load_im_mf2(h0, vl);
        const vint16mf2_t h1r = mimo_rvv_site4_load_re_mf2(h1, vl);
        const vint16mf2_t h1i = mimo_rvv_site4_load_im_mf2(h1, vl);
        const vint16mf2_t h2r = mimo_rvv_site4_load_re_mf2(h2, vl);
        const vint16mf2_t h2i = mimo_rvv_site4_load_im_mf2(h2, vl);
        const vint16mf2_t h3r = mimo_rvv_site4_load_re_mf2(h3, vl);
        const vint16mf2_t h3i = mimo_rvv_site4_load_im_mf2(h3, vl);
        const vint16mf2_t yr = mimo_rvv_site4_load_re_mf2(yy, vl);
        const vint16mf2_t yi = mimo_rvv_site4_load_im_mf2(yy, vl);
#endif

        mimo_rvv_rowfuse_acc_conj_mf2(h0r, h0i, yr, yi, &b0r, &b0i, vl);
        mimo_rvv_rowfuse_acc_real_mf2(h0r, h0i, &a00r, vl);
        mimo_rvv_rowfuse_acc_conj_mf2(h0r, h0i, h1r, h1i, &a01r, &a01i, vl);
        mimo_rvv_rowfuse_acc_conj_mf2(h0r, h0i, h2r, h2i, &a02r, &a02i, vl);
        mimo_rvv_rowfuse_acc_conj_mf2(h0r, h0i, h3r, h3i, &a03r, &a03i, vl);

        mimo_rvv_rowfuse_acc_conj_mf2(h1r, h1i, yr, yi, &b1r, &b1i, vl);
        mimo_rvv_rowfuse_acc_real_mf2(h1r, h1i, &a11r, vl);
        mimo_rvv_rowfuse_acc_conj_mf2(h1r, h1i, h2r, h2i, &a12r, &a12i, vl);
        mimo_rvv_rowfuse_acc_conj_mf2(h1r, h1i, h3r, h3i, &a13r, &a13i, vl);

        mimo_rvv_rowfuse_acc_conj_mf2(h2r, h2i, yr, yi, &b2r, &b2i, vl);
        mimo_rvv_rowfuse_acc_real_mf2(h2r, h2i, &a22r, vl);
        mimo_rvv_rowfuse_acc_conj_mf2(h2r, h2i, h3r, h3i, &a23r, &a23i, vl);

        mimo_rvv_rowfuse_acc_conj_mf2(h3r, h3i, yr, yi, &b3r, &b3i, vl);
        mimo_rvv_rowfuse_acc_real_mf2(h3r, h3i, &a33r, vl);
    }

    b0r = mimo_rvv_round_once_dot_out_mf2(b0r, vl);
    b0i = mimo_rvv_round_once_dot_out_mf2(b0i, vl);
    b1r = mimo_rvv_round_once_dot_out_mf2(b1r, vl);
    b1i = mimo_rvv_round_once_dot_out_mf2(b1i, vl);
    b2r = mimo_rvv_round_once_dot_out_mf2(b2r, vl);
    b2i = mimo_rvv_round_once_dot_out_mf2(b2i, vl);
    b3r = mimo_rvv_round_once_dot_out_mf2(b3r, vl);
    b3i = mimo_rvv_round_once_dot_out_mf2(b3i, vl);
    a00r = mimo_rvv_round_once_dot_out_mf2(a00r, vl);
    a01r = mimo_rvv_round_once_dot_out_mf2(a01r, vl);
    a01i = mimo_rvv_round_once_dot_out_mf2(a01i, vl);
    a02r = mimo_rvv_round_once_dot_out_mf2(a02r, vl);
    a02i = mimo_rvv_round_once_dot_out_mf2(a02i, vl);
    a03r = mimo_rvv_round_once_dot_out_mf2(a03r, vl);
    a03i = mimo_rvv_round_once_dot_out_mf2(a03i, vl);
    a11r = mimo_rvv_round_once_dot_out_mf2(a11r, vl);
    a12r = mimo_rvv_round_once_dot_out_mf2(a12r, vl);
    a12i = mimo_rvv_round_once_dot_out_mf2(a12i, vl);
    a13r = mimo_rvv_round_once_dot_out_mf2(a13r, vl);
    a13i = mimo_rvv_round_once_dot_out_mf2(a13i, vl);
    a22r = mimo_rvv_round_once_dot_out_mf2(a22r, vl);
    a23r = mimo_rvv_round_once_dot_out_mf2(a23r, vl);
    a23i = mimo_rvv_round_once_dot_out_mf2(a23i, vl);
    a33r = mimo_rvv_round_once_dot_out_mf2(a33r, vl);

    if (mmse) {
        a00r = __riscv_vadd_vx_i32m1(a00r, MIMO_MMSE_LAMBDA_Q15, vl);
        a11r = __riscv_vadd_vx_i32m1(a11r, MIMO_MMSE_LAMBDA_Q15, vl);
        a22r = __riscv_vadd_vx_i32m1(a22r, MIMO_MMSE_LAMBDA_Q15, vl);
        a33r = __riscv_vadd_vx_i32m1(a33r, MIMO_MMSE_LAMBDA_Q15, vl);
    }

    mimo_rvv_zf4_schur_rvv_solve_vec_q15(
        a00r, a01r, a01i, a02r, a02i, a03r, a03i,
        a11r, a12r, a12i, a13r, a13i,
        a22r, a23r, a23i, a33r,
        b0r, b0i, b1r, b1i, b2r, b2i, b3r, b3i,
        MIMO_MMSE4_SCHUR_SCALE_ARG(mmse), x_block);
}
#endif
#endif

static MIMO_ALWAYS_INLINE void mimo_rvv_build_hermitian4_site4_upper_soa_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_site4_soa4_t *work)
{
#if MIMO_USE_SITE4_HERMITIAN_UPPER_ROWPAIR && \
    MIMO_USE_RVV_SITE4_MF2 && !MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED
    mimo_rvv_build_hermitian4_site4_upper_rowpair_soa_mf2_q15(
        h_site4, y_site4, block, work);
    return;
#endif

#if MIMO_USE_SITE4_HERMITIAN_UPPER_ROWFUSE && \
    MIMO_USE_RVV_SITE4_MF2 && !MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED
    mimo_rvv_build_hermitian4_site4_upper_rowfuse_soa_mf2_q15(
        h_site4, y_site4, block, work);
    return;
#endif

    for (int tx = 0; tx < MIMO_N4; ++tx) {
        mimo_rvv_dot4_sites_store_conj_soa_q15(
            h_site4, y_site4, block, tx, 0, 0,
            &work->b_re[tx][0], &work->b_im[tx][0],
            (int32_t *)0, (int32_t *)0, 0);
    }

    for (int tx_i = 0; tx_i < MIMO_N4; ++tx_i) {
        for (int tx_j = tx_i; tx_j < MIMO_N4; ++tx_j) {
#if MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED
            const size_t upper =
                (size_t)((tx_i * (2 * MIMO_N4 - tx_i + 1)) / 2 +
                         (tx_j - tx_i));
#else
            const size_t upper = (size_t)(tx_i * MIMO_N4 + tx_j);
#endif

            if (tx_i == tx_j) {
                mimo_rvv_dot4_sites_store_real_soa_q15(
                    h_site4, h_site4, block, tx_i, tx_j, 1,
                    &work->a_re[upper][0], (int32_t *)0);
            } else {
                mimo_rvv_dot4_sites_store_conj_soa_q15(
                    h_site4, h_site4, block, tx_i, tx_j, 1,
                    &work->a_re[upper][0], &work->a_im[upper][0],
                    (int32_t *)0, (int32_t *)0, 0);
            }
        }
    }
}

static MIMO_ALWAYS_INLINE void mimo_rvv_eq4_site4_run(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    int mmse,
    mimo_c16_t *x)
{
    const size_t blocks = MIMO_EQ4_SITE_COUNT / MIMO_SITE4_LANES;

#if MIMO_USE_RVV_SOLVE4 && !MIMO_USE_RECIP_TABLE && \
    !MIMO_USE_SOLVE4_UNROLL_V2A && \
    !MIMO_USE_SOLVE4_HERMITIAN && !MIMO_USE_SOLVE4_LDLH && \
    !MIMO_USE_SOLVE4_SCHUR && \
    !MIMO_COLLECT_SOLVE_DEN_STATS && !MIMO_COLLECT_SOLVE_VALUE_STATS && \
    !MIMO_COLLECT_SCHUR_STATS
    mimo_site4_soa4_t work MIMO_ALIGN64;
    for (size_t block = 0; block < blocks; ++block) {
        mimo_rvv_build_normal4_site4_soa_q15(h_site4, y_site4, block, &work);
        mimo_rvv_solve_normal4_site4_q15(
            &work, mmse, &x[block * MIMO_SITE4_LANES * MIMO_N4]);
    }
#else
#if MIMO_USE_SITE4_SOLVE_SOA
    mimo_site4_soa4_t work MIMO_ALIGN64;
    for (size_t block = 0; block < blocks; ++block) {
#if MIMO_USE_SOLVE4_HERMITIAN && MIMO_USE_SITE4_HERMITIAN_UPPER_BUILD && \
    !MIMO_COLLECT_SCHUR_STATS
        if (!mmse || MIMO_USE_SITE4_HERMITIAN_UPPER_MMSE) {
#if MIMO_USE_RVV_ZF4_FUSED_BUILD_SOLVE
            if (!mmse || MIMO_USE_RVV_MMSE4_SCHUR_RVV_SOLVE) {
            mimo_rvv_eq4_build_solve_fused_rowfuse_mf2_q15(
                h_site4, y_site4, block,
                mmse,
                &x[block * MIMO_SITE4_LANES * MIMO_N4]);
            continue;
            }
#else
            mimo_rvv_build_hermitian4_site4_upper_soa_q15(
                h_site4, y_site4, block, &work);
#if MIMO_USE_SOLVE4_SCHUR
            if (mmse) {
#if MIMO_USE_RVV_MMSE4_SCHUR_RVV_SOLVE
                mimo_rvv_mmse4_schur_rvv_solve_soa_q15(
                    &work, &x[block * MIMO_SITE4_LANES * MIMO_N4]);
#elif MIMO_USE_SOLVE4_SCHUR_MMSE_SPECIALIZE
                mimo_solve_normal4_site4_soa_schur_mmse_q15(
                    &work, &x[block * MIMO_SITE4_LANES * MIMO_N4]);
#else
                mimo_solve_normal4_site4_soa_schur_q15(
                    &work, 1, &x[block * MIMO_SITE4_LANES * MIMO_N4]);
#endif
                continue;
            }
#endif
#if MIMO_USE_SOLVE4_SCHUR
#if MIMO_USE_RVV_ZF4_SCHUR_RVV_SOLVE
            mimo_rvv_zf4_schur_rvv_solve_soa_q15(
                &work, &x[block * MIMO_SITE4_LANES * MIMO_N4]);
#else
            mimo_solve_normal4_site4_soa_schur_zf_q15(
                &work, &x[block * MIMO_SITE4_LANES * MIMO_N4]);
#endif
#else
            mimo_solve_normal4_site4_soa_hermitian_zf_q15(
                &work, &x[block * MIMO_SITE4_LANES * MIMO_N4]);
#endif
            continue;
#endif
        }
#endif
        mimo_rvv_build_normal4_site4_soa_q15(h_site4, y_site4, block, &work);
#if MIMO_USE_SOLVE4_SCHUR
#if MIMO_USE_SOLVE4_SCHUR_MMSE_SPECIALIZE
        if (mmse) {
            mimo_solve_normal4_site4_soa_schur_mmse_q15(
                &work, &x[block * MIMO_SITE4_LANES * MIMO_N4]);
        } else {
            mimo_solve_normal4_site4_soa_schur_q15(
                &work, 0, &x[block * MIMO_SITE4_LANES * MIMO_N4]);
        }
#else
        mimo_solve_normal4_site4_soa_schur_q15(
            &work, mmse, &x[block * MIMO_SITE4_LANES * MIMO_N4]);
#endif
#else
        mimo_solve_normal4_site4_soa_q15(
            &work, mmse, &x[block * MIMO_SITE4_LANES * MIMO_N4]);
#endif
    }
#else
    mimo_c32_t a_lane[MIMO_SITE4_LANES][MIMO_N4 * MIMO_N4];
    mimo_c32_t b_lane[MIMO_SITE4_LANES][MIMO_N4];
    for (size_t block = 0; block < blocks; ++block) {
#if MIMO_USE_SOLVE4_HERMITIAN && MIMO_USE_SITE4_HERMITIAN_UPPER_BUILD && \
    !MIMO_COLLECT_SCHUR_STATS
        if (!mmse) {
            mimo_rvv_build_hermitian4_site4_upper_q15(
                h_site4, y_site4, block, a_lane, b_lane);
            mimo_solve_normal4_site4_hermitian_zf_q15(
                a_lane, b_lane,
                &x[block * MIMO_SITE4_LANES * MIMO_N4]);
            continue;
        }
#endif
        mimo_rvv_build_normal4_site4_q15(h_site4, y_site4, block, a_lane,
                                         b_lane);
#if MIMO_USE_SOLVE4_SCHUR
        mimo_solve_normal4_site4_schur_q15(
            a_lane, b_lane, mmse,
            &x[block * MIMO_SITE4_LANES * MIMO_N4]);
#elif MIMO_USE_SOLVE4_LDLH
        mimo_solve_normal4_site4_ldlh_q15(
            a_lane, b_lane, mmse,
            &x[block * MIMO_SITE4_LANES * MIMO_N4]);
#elif MIMO_USE_SOLVE4_HERMITIAN
        if (mmse) {
            mimo_solve_normal4_site4_q15(
                a_lane, b_lane, mmse,
                &x[block * MIMO_SITE4_LANES * MIMO_N4]);
        } else {
            mimo_solve_normal4_site4_hermitian_zf_q15(
                a_lane, b_lane,
                &x[block * MIMO_SITE4_LANES * MIMO_N4]);
        }
#else
        mimo_solve_normal4_site4_q15(
            a_lane, b_lane, mmse,
            &x[block * MIMO_SITE4_LANES * MIMO_N4]);
#endif
    }
#endif
#endif
}

#endif
