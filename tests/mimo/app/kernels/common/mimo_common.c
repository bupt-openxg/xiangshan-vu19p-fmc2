#include "mimo_ext.h"
#include "mimo_kernels.h"

#if MIMO_COLLECT_SOLVE_DEN_STATS || MIMO_COLLECT_SOLVE_VALUE_STATS || \
    MIMO_COLLECT_SCHUR_STATS || MIMO_COLLECT_PIVOT_SCALE_STATS
#include "klib.h"
#endif

#ifndef MIMO_USE_RECIP_TABLE
#define MIMO_USE_RECIP_TABLE 0
#endif

#ifndef MIMO_USE_SCALAR_MMSE4_FUSED_BUILD_SOLVE
#define MIMO_USE_SCALAR_MMSE4_FUSED_BUILD_SOLVE 0
#endif

#ifndef MIMO_USE_SCALAR_MMSE4_FUSED_COMPACT_C32
#define MIMO_USE_SCALAR_MMSE4_FUSED_COMPACT_C32 0
#endif

#ifndef MIMO_USE_SITE4_ROUND_ONCE_DOT
#define MIMO_USE_SITE4_ROUND_ONCE_DOT 0
#endif

#ifndef MIMO_USE_SCALAR4_COMPLEX_MUL_3M
#define MIMO_USE_SCALAR4_COMPLEX_MUL_3M 0
#endif

#ifndef MIMO_USE_SCALAR4_SCHUR_TOP_Q30_ONCE
#define MIMO_USE_SCALAR4_SCHUR_TOP_Q30_ONCE 0
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

#ifndef MIMO_USE_EXT_MMSE4_SCHUR_FINAL_SIGNED_TRUNC_SCALE
#define MIMO_USE_EXT_MMSE4_SCHUR_FINAL_SIGNED_TRUNC_SCALE 0
#endif

#ifndef MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK
#define MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK 0
#endif

#ifndef MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_LOAD
#define MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_LOAD 0
#endif

#ifndef MIMO_USE_EXT_MMSE4_RVV_I32_VEC_LAMBDA
#define MIMO_USE_EXT_MMSE4_RVV_I32_VEC_LAMBDA 0
#endif

#ifndef MIMO_USE_EXT_MMSE4_RVV_I32_LATE_TAIL_LOAD
#define MIMO_USE_EXT_MMSE4_RVV_I32_LATE_TAIL_LOAD 0
#endif

#ifndef MIMO_USE_EXT_MMSE4_RVV_I32_SPECIALIZE
#define MIMO_USE_EXT_MMSE4_RVV_I32_SPECIALIZE 0
#endif

#ifndef MIMO_USE_EXT_MMSE4_RVV_I32_INNER_Q30_ONCE
#define MIMO_USE_EXT_MMSE4_RVV_I32_INNER_Q30_ONCE 0
#endif

#ifndef MIMO_USE_EXT_MMSE4_RVV_I32_TOP_Q30_ONCE
#define MIMO_USE_EXT_MMSE4_RVV_I32_TOP_Q30_ONCE 0
#endif

#ifndef MIMO_USE_EXT_MMSE4_RVV_I32_DIAG_VLSE16_LOAD
#define MIMO_USE_EXT_MMSE4_RVV_I32_DIAG_VLSE16_LOAD 0
#endif

#ifndef MIMO_USE_EXT_MMSE4_RVV_I32_ASSUME_LAMBDA_NOSAT
#define MIMO_USE_EXT_MMSE4_RVV_I32_ASSUME_LAMBDA_NOSAT 0
#endif

#ifndef MIMO_USE_EXT_MMSE4_RVV_I32_BINV_REUSE
#define MIMO_USE_EXT_MMSE4_RVV_I32_BINV_REUSE 0
#endif

#ifndef MIMO_USE_EXT_MMSE4_RVV_I32_PACKED_PHASE
#define MIMO_USE_EXT_MMSE4_RVV_I32_PACKED_PHASE 0
#endif

#ifndef MIMO_EXT_MMSE4_PACKED_TOP_EXT
#define MIMO_EXT_MMSE4_PACKED_TOP_EXT 1
#endif

#ifndef MIMO_EXT_MMSE4_PACKED_SSOLVE_EXT
#define MIMO_EXT_MMSE4_PACKED_SSOLVE_EXT 0
#endif

#ifndef MIMO_USE_EXT_MMSE4_PACKED_LIVE_BUILD
#define MIMO_USE_EXT_MMSE4_PACKED_LIVE_BUILD 0
#endif

#ifndef MIMO_USE_EXT_MMSE4_PACKED_LIVE_BUILD_UNROLL4
#define MIMO_USE_EXT_MMSE4_PACKED_LIVE_BUILD_UNROLL4 0
#endif

#ifndef MIMO_EXT_MMSE4_PACKED_INVCOEFF_EXT
#define MIMO_EXT_MMSE4_PACKED_INVCOEFF_EXT 0
#endif

#ifndef MIMO_USE_EXT_MMSE4_PACKED_VSET_CHAIN
#define MIMO_USE_EXT_MMSE4_PACKED_VSET_CHAIN 0
#endif

#ifndef MIMO_EXT_MMSE4_PACKED_SOLVE_SEED
#define MIMO_EXT_MMSE4_PACKED_SOLVE_SEED 0
#endif

#ifndef MIMO_EXT_MMSE4_PACKED_DET_EXT
#define MIMO_EXT_MMSE4_PACKED_DET_EXT 0
#endif

#ifndef MIMO_USE_EXT_MMSE4_PACKED_FIXED_PTRS
#define MIMO_USE_EXT_MMSE4_PACKED_FIXED_PTRS 0
#endif

#ifndef MIMO_USE_EXT_MMSE4_PACKED_E16_RECIP
#define MIMO_USE_EXT_MMSE4_PACKED_E16_RECIP 0
#endif

#ifndef MIMO_USE_EXT_MMSE4_PACKED_E16_LAMBDA
#define MIMO_USE_EXT_MMSE4_PACKED_E16_LAMBDA 0
#endif

#ifndef MIMO_USE_EXT_MMSE4_PACKED_RX4_M4_LOAD
#define MIMO_USE_EXT_MMSE4_PACKED_RX4_M4_LOAD 0
#endif

#ifndef MIMO_EXT_MMSE4_PACKED_S_RECIP_MODE
#define MIMO_EXT_MMSE4_PACKED_S_RECIP_MODE 0
#endif

#ifndef MIMO_USE_EXT_ZF4_PACKED_LIVE_BUILD
#define MIMO_USE_EXT_ZF4_PACKED_LIVE_BUILD 0
#endif

#ifndef MIMO_USE_EXT_ZF4_VSCMACC_SEED_BUILD
#define MIMO_USE_EXT_ZF4_VSCMACC_SEED_BUILD 0
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

#ifndef MIMO_USE_MMSE4_SCHUR_INNER_Q30_ONCE
#define MIMO_USE_MMSE4_SCHUR_INNER_Q30_ONCE 0
#endif

#if MIMO_USE_SCALAR4_COMPLEX_MUL_3M && !MIMO_USE_SITE4_ROUND_ONCE_DOT
#error "scalar 3M complex multiply requires round-once Q30 build"
#endif

#if MIMO_USE_SCALAR_MMSE4_FUSED_COMPACT_C32 && \
    !MIMO_USE_SCALAR_MMSE4_FUSED_BUILD_SOLVE
#error "scalar MMSE4 compact c32 state requires fused build/solve"
#endif

#if MIMO_USE_MMSE4_SCHUR_SIGNED_TRUNC_SCALE && !MIMO_USE_MMSE4_SCHUR_NO_MAG_SCALE
#error "MIMO_USE_MMSE4_SCHUR_SIGNED_TRUNC_SCALE requires MIMO_USE_MMSE4_SCHUR_NO_MAG_SCALE=1"
#endif

#if MIMO_USE_EXT_MMSE4_SCHUR_FINAL_SIGNED_TRUNC_SCALE && !MIMO_USE_MMSE4_SCHUR_NO_MAG_SCALE
#error "MIMO_USE_EXT_MMSE4_SCHUR_FINAL_SIGNED_TRUNC_SCALE requires MIMO_USE_MMSE4_SCHUR_NO_MAG_SCALE=1"
#endif

#if MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK && !MIMO_USE_MMSE4_SCHUR_NO_MAG_SCALE
#error "MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK requires MIMO_USE_MMSE4_SCHUR_NO_MAG_SCALE=1"
#endif

#if MIMO_USE_EXT_MMSE4_RVV_I32_BINV_REUSE && \
    (!MIMO_USE_EXT_MMSE4_RVV_I32_SPECIALIZE || \
     MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK || \
     MIMO_USE_EXT_MMSE4_SCHUR_FINAL_SIGNED_TRUNC_SCALE || \
     !MIMO_USE_MMSE4_SCHUR_NO_MAG_SCALE)
#error "MMSE4 RVV-i32 B-inverse reuse requires specialized mask0 no-mag-scale path"
#endif

#if MIMO_USE_EXT_MMSE4_RVV_I32_PACKED_PHASE && \
    (!MIMO_USE_EXT_MMSE4_RVV_I32_BINV_REUSE || \
     !MIMO_USE_EXT_MMSE4_RVV_I32_SPECIALIZE || \
     !MIMO_USE_EXT_MMSE4_RVV_I32_ASSUME_LAMBDA_NOSAT || \
     !MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_LOAD || \
     !MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_STORE || \
     !MIMO_USE_RVV_SCHUR4_VSSRA_ROUND || \
     MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK || \
     MIMO_USE_EXT_MMSE4_SCHUR_FINAL_SIGNED_TRUNC_SCALE || \
     !MIMO_USE_MMSE4_SCHUR_NO_MAG_SCALE)
#error "MMSE4 packed EXT phase requires Q13 B-inverse, specialized no-sat mask0, pack32, vssra, and no-mag-scale"
#endif

#if MIMO_EXT_MMSE4_PACKED_SSOLVE_EXT && \
    (!MIMO_USE_EXT_MMSE4_RVV_I32_PACKED_PHASE || \
     !MIMO_EXT_MMSE4_PACKED_TOP_EXT)
#error "packed S-solve EXT requires packed phase and packed EXT top"
#endif

#if MIMO_USE_EXT_MMSE4_PACKED_LIVE_BUILD && \
    !MIMO_USE_EXT_MMSE4_RVV_I32_PACKED_PHASE
#error "MMSE4 packed live-build requires packed EXT phase"
#endif

#if MIMO_USE_EXT_MMSE4_PACKED_LIVE_BUILD_UNROLL4 && \
    (!MIMO_USE_EXT_MMSE4_PACKED_LIVE_BUILD || \
     !MIMO_USE_EXT_ZF4_VSCMACC_SEED_BUILD)
#error "MMSE4 packed live-build unroll4 requires live-build and seeded accumulation"
#endif

#if MIMO_EXT_MMSE4_PACKED_INVCOEFF_EXT && \
    (!MIMO_USE_EXT_MMSE4_RVV_I32_PACKED_PHASE || \
     !MIMO_EXT_MMSE4_PACKED_SSOLVE_EXT || \
     !MIMO_USE_EXT_MMSE4_RVV_I32_ASSUME_LAMBDA_NOSAT)
#error "packed EXT inverse coefficients require packed S-solve and no-saturation lambda"
#endif

#if MIMO_USE_EXT_MMSE4_PACKED_VSET_CHAIN && \
    !MIMO_USE_EXT_MMSE4_RVV_I32_PACKED_PHASE
#error "MMSE4 packed vset chaining requires packed EXT phase"
#endif

#if MIMO_EXT_MMSE4_PACKED_SOLVE_SEED && \
    !MIMO_USE_EXT_MMSE4_RVV_I32_PACKED_PHASE
#error "MMSE4 packed solve seeding requires packed EXT phase"
#endif

#if MIMO_EXT_MMSE4_PACKED_DET_EXT && \
    !MIMO_EXT_MMSE4_PACKED_INVCOEFF_EXT
#error "MMSE4 packed EXT determinant requires packed EXT inverse coefficients"
#endif

#if MIMO_USE_EXT_MMSE4_PACKED_FIXED_PTRS && \
    (!MIMO_USE_EXT_MMSE4_PACKED_LIVE_BUILD || \
     !MIMO_USE_EXT_MMSE4_PACKED_LIVE_BUILD_UNROLL4)
#error "MMSE4 packed fixed pointers require unrolled packed live-build"
#endif

#if MIMO_USE_EXT_MMSE4_PACKED_E16_RECIP && \
    (!MIMO_EXT_MMSE4_PACKED_DET_EXT || \
     !MIMO_EXT_MMSE4_PACKED_INVCOEFF_EXT || \
     !MIMO_EXT_MMSE4_PACKED_SSOLVE_EXT || \
     !MIMO_USE_EXT_MMSE4_PACKED_VSET_CHAIN || \
     MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 5)
#error "MMSE4 packed e16 reciprocal requires packed det/invcoeff/S-solve, vset chain, and direct1 table"
#endif

#if MIMO_USE_EXT_MMSE4_PACKED_E16_LAMBDA && \
    (!MIMO_USE_EXT_MMSE4_RVV_I32_PACKED_PHASE || \
     !MIMO_USE_EXT_MMSE4_PACKED_LIVE_BUILD || \
     !MIMO_EXT_MMSE4_PACKED_INVCOEFF_EXT)
#error "MMSE4 packed e16 lambda requires packed phase/live-build and packed invcoeff"
#endif

#if MIMO_USE_EXT_MMSE4_PACKED_RX4_M4_LOAD && \
    (!MIMO_USE_EXT_MMSE4_PACKED_LIVE_BUILD || \
     !MIMO_USE_EXT_MMSE4_PACKED_LIVE_BUILD_UNROLL4 || \
     !MIMO_USE_EXT_ZF4_VSCMACC_SEED_BUILD || \
     !MIMO_USE_EXT_MMSE4_PACKED_FIXED_PTRS || \
     !MIMO_USE_EXT_MMSE4_PACKED_E16_RECIP || \
     !MIMO_USE_EXT_MMSE4_PACKED_E16_LAMBDA)
#error "MMSE4 packed RX4 m4 load requires the aligned e16 current-best live build"
#endif

#if MIMO_EXT_MMSE4_PACKED_S_RECIP_MODE < 0 || \
    MIMO_EXT_MMSE4_PACKED_S_RECIP_MODE > 2
#error "MMSE4 packed S reciprocal mode must be 0, 1, or 2"
#endif

#if MIMO_EXT_MMSE4_PACKED_S_RECIP_MODE && \
    (!MIMO_USE_EXT_MMSE4_PACKED_RX4_M4_LOAD || \
     !MIMO_EXT_MMSE4_PACKED_DET_EXT || \
     !MIMO_USE_EXT_MMSE4_PACKED_E16_RECIP || \
     !MIMO_USE_EXT_MMSE4_PACKED_E16_LAMBDA)
#error "MMSE4 packed S reciprocal modes require the RX4 m4 e16 frontier"
#endif

#if MIMO_EXT_MMSE4_PACKED_S_RECIP_MODE == 2 && \
    MIMO_MMSE_LAMBDA_Q15 != 512
#error "MMSE4 packed S NR1 currently derives Q11 one from lambda_q15=512"
#endif

#if MIMO_USE_EXT_ZF4_PACKED_LIVE_BUILD && \
    (!MIMO_USE_EXT_MMSE4_RVV_I32_PACKED_PHASE || \
     !MIMO_EXT_MMSE4_PACKED_SSOLVE_EXT || \
     !MIMO_USE_EXT_MMSE4_PACKED_LIVE_BUILD || \
     !MIMO_USE_EXT_MMSE4_PACKED_LIVE_BUILD_UNROLL4 || \
     !MIMO_EXT_MMSE4_PACKED_INVCOEFF_EXT || \
     !MIMO_USE_EXT_MMSE4_PACKED_VSET_CHAIN || \
     !MIMO_EXT_MMSE4_PACKED_SOLVE_SEED || \
     !MIMO_EXT_MMSE4_PACKED_DET_EXT || \
     !MIMO_USE_EXT_MMSE4_PACKED_FIXED_PTRS || \
     !MIMO_USE_EXT_MMSE4_PACKED_E16_RECIP || \
     !MIMO_USE_EXT_MMSE4_PACKED_E16_LAMBDA || \
     !MIMO_USE_EXT_MMSE4_PACKED_RX4_M4_LOAD || \
     !MIMO_USE_EXT_ZF4_VSCMACC_SEED_BUILD || \
     MIMO_EXT_MMSE4_PACKED_S_RECIP_MODE != 0)
#error "ZF4 packed live-build requires the MMSE4 RX4 m4 direct1 packed frontier"
#endif

#if MIMO_USE_EXT_MMSE4_RVV_I32_PACKED_PHASE
__asm__(
    ".macro mimo_get_vreg_num outreg, invreg\n\t"
    "  .set \\outreg, -1\n\t"
    "  .set _mimo_vreg_i, 0\n\t"
    "  .irp v, v0,v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13,v14,v15,v16,v17,v18,v19,v20,v21,v22,v23,v24,v25,v26,v27,v28,v29,v30,v31\n\t"
    "    .ifc \\invreg, \\v\n\t"
    "      .set \\outreg, _mimo_vreg_i\n\t"
    "    .endif\n\t"
    "    .set _mimo_vreg_i, _mimo_vreg_i + 1\n\t"
    "  .endr\n\t"
    "  .if \\outreg < 0\n\t"
    "    .error \"Invalid vector register mapping\"\n\t"
    "  .endif\n\t"
    ".endm\n\t"
    ".macro mimo_emit_ext_vv base, vd, vs2, vs1\n\t"
    "  mimo_get_vreg_num _mimo_vd_num, \\vd\n\t"
    "  mimo_get_vreg_num _mimo_vs2_num, \\vs2\n\t"
    "  mimo_get_vreg_num _mimo_vs1_num, \\vs1\n\t"
    "  .word \\base | (_mimo_vd_num << 7) | (_mimo_vs1_num << 15) | (_mimo_vs2_num << 20)\n\t"
    ".endm\n\t"
    ".macro mimo_emit_vscmacc_vv vd, vs2, vs1\n\t"
    "  mimo_emit_ext_vv 0xD2000057, \\vd, \\vs2, \\vs1\n\t"
    ".endm\n\t"
    ".macro mimo_emit_vscmul_vv vd, vs2, vs1\n\t"
    "  mimo_emit_ext_vv 0xCA000057, \\vd, \\vs2, \\vs1\n\t"
    ".endm\n\t"
    ".macro mimo_emit_vscmulcj_vv vd, vs2, vs1\n\t"
    "  mimo_emit_ext_vv 0xCE000057, \\vd, \\vs2, \\vs1\n\t"
    ".endm\n\t"
    ".macro mimo_emit_vscmacccj_vv vd, vs2, vs1\n\t"
    "  mimo_emit_ext_vv 0xDA000057, \\vd, \\vs2, \\vs1\n\t"
    ".endm\n\t");
#endif

#ifndef MIMO_USE_SOLVE4_UNROLL_V2A
#define MIMO_USE_SOLVE4_UNROLL_V2A 0
#endif

#ifndef MIMO_USE_SOLVE4_HERMITIAN
#define MIMO_USE_SOLVE4_HERMITIAN 0
#endif

#ifndef MIMO_USE_SOLVE4_HERMITIAN_LANE_OUTER
#define MIMO_USE_SOLVE4_HERMITIAN_LANE_OUTER 0
#endif

#ifndef MIMO_USE_SOLVE4_HERMITIAN_SOA_STAGE_OUTER
#define MIMO_USE_SOLVE4_HERMITIAN_SOA_STAGE_OUTER 0
#endif

#ifndef MIMO_USE_SOLVE4_HERMITIAN_FACTOR_PRECOMPUTE
#define MIMO_USE_SOLVE4_HERMITIAN_FACTOR_PRECOMPUTE 0
#endif

#ifndef MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE
#define MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE 0
#endif

#ifndef MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE_HARDDIV
#define MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE_HARDDIV 0
#endif

#ifndef MIMO_USE_SOLVE4_HERMITIAN_HAND_SCHED
#define MIMO_USE_SOLVE4_HERMITIAN_HAND_SCHED 0
#endif

#ifndef MIMO_USE_SOLVE4_HERMITIAN_FACTOR_CALL
#define MIMO_USE_SOLVE4_HERMITIAN_FACTOR_CALL 0
#endif

#ifndef MIMO_USE_SOLVE4_HERMITIAN_FACTOR_Q15
#define MIMO_USE_SOLVE4_HERMITIAN_FACTOR_Q15 0
#endif

#ifndef MIMO_USE_SOLVE4_HERMITIAN_Q15_CALL
#define MIMO_USE_SOLVE4_HERMITIAN_Q15_CALL 0
#endif

#ifndef MIMO_USE_SOLVE4_HERMITIAN_Q15_ONECORR
#define MIMO_USE_SOLVE4_HERMITIAN_Q15_ONECORR 0
#endif

#ifndef MIMO_SOLVE4_HERMITIAN_Q15_JUMP_ONLY_MASK
#define MIMO_SOLVE4_HERMITIAN_Q15_JUMP_ONLY_MASK 0
#endif

#ifndef MIMO_USE_SOLVE4_HERMITIAN_GS
#define MIMO_USE_SOLVE4_HERMITIAN_GS 0
#endif

#ifndef MIMO_SOLVE4_HERMITIAN_GS_ITERS
#define MIMO_SOLVE4_HERMITIAN_GS_ITERS 3
#endif

#ifndef MIMO_USE_SOLVE4_HERMITIAN_Q30_FIVECORR
#define MIMO_USE_SOLVE4_HERMITIAN_Q30_FIVECORR 0
#endif

#ifndef MIMO_USE_SOLVE4_HERMITIAN_Q30_TWOJUMP_CORR
#define MIMO_USE_SOLVE4_HERMITIAN_Q30_TWOJUMP_CORR 0
#endif

#ifndef MIMO_USE_SOLVE4_HERMITIAN_POS_PIVOT_SCALE
#define MIMO_USE_SOLVE4_HERMITIAN_POS_PIVOT_SCALE 0
#endif

#ifndef MIMO_USE_SOLVE4_HERMITIAN_TRUST_PIVOT_RANGE
#define MIMO_USE_SOLVE4_HERMITIAN_TRUST_PIVOT_RANGE 0
#endif

#ifndef MIMO_USE_SOLVE4_HERMITIAN_TRUST_PIVOT_NONZERO
#define MIMO_USE_SOLVE4_HERMITIAN_TRUST_PIVOT_NONZERO 0
#endif

#ifndef MIMO_USE_SOLVE4_HERMITIAN_FULL_RECIP_TABLE
#define MIMO_USE_SOLVE4_HERMITIAN_FULL_RECIP_TABLE 0
#endif

#ifndef MIMO_USE_SOLVE4_HERMITIAN_FLOOR_FULL_RECIP_TABLE
#define MIMO_USE_SOLVE4_HERMITIAN_FLOOR_FULL_RECIP_TABLE 0
#endif

#define MIMO_HERM_PIVOT_RECIP_MIN 10560u
#define MIMO_HERM_PIVOT_RECIP_MAX 11972u
#define MIMO_HERM_PIVOT_RECIP_TABLE_SIZE \
    (MIMO_HERM_PIVOT_RECIP_MAX - MIMO_HERM_PIVOT_RECIP_MIN + 1u)
#define MIMO_HERM_PIVOT_RECIP_BITSET_WORDS \
    ((MIMO_HERM_PIVOT_RECIP_TABLE_SIZE + 31u) / 32u)

#if MIMO_USE_SOLVE4_HERMITIAN_FULL_RECIP_TABLE && \
    !MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE
#error "MIMO_USE_SOLVE4_HERMITIAN_FULL_RECIP_TABLE requires MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE"
#endif

#if MIMO_USE_SOLVE4_HERMITIAN_FLOOR_FULL_RECIP_TABLE && \
    !MIMO_USE_SOLVE4_HERMITIAN_FULL_RECIP_TABLE
#error "MIMO_USE_SOLVE4_HERMITIAN_FLOOR_FULL_RECIP_TABLE requires MIMO_USE_SOLVE4_HERMITIAN_FULL_RECIP_TABLE"
#endif

#if MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE_HARDDIV && \
    !MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE
#error "MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE_HARDDIV requires MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE"
#endif

#if MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE_HARDDIV && \
    (MIMO_USE_SOLVE4_HERMITIAN_FULL_RECIP_TABLE || \
     MIMO_USE_SOLVE4_HERMITIAN_FLOOR_FULL_RECIP_TABLE || \
     MIMO_USE_SOLVE4_HERMITIAN_RECIP_STEP16)
#error "MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE_HARDDIV is mutually exclusive with table reciprocal options"
#endif

#if MIMO_USE_SOLVE4_HERMITIAN_FACTOR_Q15 && \
    MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE
#error "MIMO_USE_SOLVE4_HERMITIAN_FACTOR_Q15 is a direct-div diagnostic and cannot be combined with pivot-scale"
#endif

#ifndef MIMO_USE_SOLVE4_HERMITIAN_RECIP_STEP16
#define MIMO_USE_SOLVE4_HERMITIAN_RECIP_STEP16 0
#endif

#ifndef MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
#define MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE 0
#endif

#ifndef MIMO_USE_SOLVE4_HERMITIAN_DIAG_APPROX
#define MIMO_USE_SOLVE4_HERMITIAN_DIAG_APPROX 0
#endif

#ifndef MIMO_USE_SOLVE4_HERMITIAN_Q30_JUMP_ONLY
#define MIMO_USE_SOLVE4_HERMITIAN_Q30_JUMP_ONLY 0
#endif

#ifndef MIMO_USE_SOLVE4_HERMITIAN_Q30_ONECORR
#define MIMO_USE_SOLVE4_HERMITIAN_Q30_ONECORR 0
#endif

#ifndef MIMO_USE_SOLVE4_HERMITIAN_Q30_PAIR_CORR
#define MIMO_USE_SOLVE4_HERMITIAN_Q30_PAIR_CORR 0
#endif

#ifndef MIMO_SOLVE4_HERMITIAN_Q30_JUMP_ONLY_MASK
#define MIMO_SOLVE4_HERMITIAN_Q30_JUMP_ONLY_MASK 0
#endif

#if MIMO_USE_SOLVE4_HERMITIAN_Q30_PAIR_CORR && \
    (MIMO_USE_SOLVE4_HERMITIAN_Q30_JUMP_ONLY || \
     MIMO_USE_SOLVE4_HERMITIAN_Q30_ONECORR || \
     MIMO_USE_SOLVE4_HERMITIAN_Q30_FIVECORR || \
     MIMO_USE_SOLVE4_HERMITIAN_Q30_TWOJUMP_CORR)
#error "MIMO_USE_SOLVE4_HERMITIAN_Q30_PAIR_CORR only supports the full Q30 correction path"
#endif

#if MIMO_SOLVE4_HERMITIAN_Q30_JUMP_ONLY_MASK && \
    !MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE
#error "MIMO_SOLVE4_HERMITIAN_Q30_JUMP_ONLY_MASK requires pivot-scale division"
#endif

#if MIMO_SOLVE4_HERMITIAN_Q30_JUMP_ONLY_MASK && \
    (MIMO_USE_SOLVE4_HERMITIAN_Q30_JUMP_ONLY || \
     MIMO_USE_SOLVE4_HERMITIAN_Q30_ONECORR || \
     MIMO_USE_SOLVE4_HERMITIAN_Q30_FIVECORR || \
     MIMO_USE_SOLVE4_HERMITIAN_Q30_TWOJUMP_CORR || \
     MIMO_USE_SOLVE4_HERMITIAN_Q30_PAIR_CORR)
#error "MIMO_SOLVE4_HERMITIAN_Q30_JUMP_ONLY_MASK is only valid with the full Q30 correction path"
#endif

#if MIMO_SOLVE4_HERMITIAN_Q30_JUMP_ONLY_MASK && \
    (MIMO_USE_SOLVE4_HERMITIAN_HAND_SCHED || \
     MIMO_USE_SOLVE4_HERMITIAN_FACTOR_CALL)
#error "MIMO_SOLVE4_HERMITIAN_Q30_JUMP_ONLY_MASK requires inline Q30 factor division"
#endif

#if MIMO_SOLVE4_HERMITIAN_Q15_JUMP_ONLY_MASK && \
    !MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE
#error "MIMO_SOLVE4_HERMITIAN_Q15_JUMP_ONLY_MASK requires pivot-scale division"
#endif

#if MIMO_SOLVE4_HERMITIAN_Q15_JUMP_ONLY_MASK && \
    !MIMO_USE_SOLVE4_HERMITIAN_Q15_ONECORR
#error "MIMO_SOLVE4_HERMITIAN_Q15_JUMP_ONLY_MASK requires MIMO_USE_SOLVE4_HERMITIAN_Q15_ONECORR"
#endif

#if MIMO_SOLVE4_HERMITIAN_Q15_JUMP_ONLY_MASK && \
    (MIMO_USE_SOLVE4_HERMITIAN_HAND_SCHED || \
     MIMO_USE_SOLVE4_HERMITIAN_Q15_CALL)
#error "MIMO_SOLVE4_HERMITIAN_Q15_JUMP_ONLY_MASK requires inline Q15 back-sub division"
#endif

#if MIMO_SOLVE4_HERMITIAN_Q15_JUMP_ONLY_MASK > 15
#error "MIMO_SOLVE4_HERMITIAN_Q15_JUMP_ONLY_MASK supports only four Q15 back-sub stages"
#endif

#ifndef MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED
#define MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED 0
#endif

#ifndef MIMO_USE_SOLVE_BRANCHLIGHT_ROUND
#define MIMO_USE_SOLVE_BRANCHLIGHT_ROUND 0
#endif

#ifndef MIMO_USE_SOLVE_Q15_I64_ARSHIFT_ROUND
#define MIMO_USE_SOLVE_Q15_I64_ARSHIFT_ROUND 0
#endif

#ifndef MIMO_USE_SOLVE_STORE_TRUST_I16
#define MIMO_USE_SOLVE_STORE_TRUST_I16 0
#endif

#ifndef MIMO_USE_SOLVE4_LDLH
#define MIMO_USE_SOLVE4_LDLH 0
#endif

#ifndef MIMO_USE_SOLVE4_SCHUR
#define MIMO_USE_SOLVE4_SCHUR 0
#endif

#ifndef MIMO_USE_SOLVE4_SCHUR_PAIR2
#define MIMO_USE_SOLVE4_SCHUR_PAIR2 0
#endif

#ifndef MIMO_USE_SOLVE4_SCHUR_DET_SCALE
#define MIMO_USE_SOLVE4_SCHUR_DET_SCALE 0
#endif

#ifndef MIMO_USE_SOLVE4_SCHUR_BINV_Q15
#define MIMO_USE_SOLVE4_SCHUR_BINV_Q15 0
#endif

#ifndef MIMO_USE_SOLVE4_SCHUR_Q15_RECIP
#define MIMO_USE_SOLVE4_SCHUR_Q15_RECIP 0
#endif

#ifndef MIMO_USE_SOLVE4_SCHUR_SCALE16_APPROX
#define MIMO_USE_SOLVE4_SCHUR_SCALE16_APPROX 0
#endif

#ifndef MIMO_USE_SOLVE4_SCHUR_FUSED_INNER
#define MIMO_USE_SOLVE4_SCHUR_FUSED_INNER 0
#endif

#ifndef MIMO_USE_SOLVE4_SCHUR_FUSED_INNER_MMSE
#define MIMO_USE_SOLVE4_SCHUR_FUSED_INNER_MMSE 0
#endif

#ifndef MIMO_USE_SOLVE4_SCHUR_MMSE_SPECIALIZE
#define MIMO_USE_SOLVE4_SCHUR_MMSE_SPECIALIZE 0
#endif

#ifndef MIMO_USE_SOLVE4_SCHUR_ZF_EXPLICIT
#define MIMO_USE_SOLVE4_SCHUR_ZF_EXPLICIT 0
#endif

#ifndef MIMO_USE_SOLVE4_SCHUR_ZF_RELOAD_MICRO
#define MIMO_USE_SOLVE4_SCHUR_ZF_RELOAD_MICRO 0
#endif

#ifndef MIMO_USE_SOLVE4_SCHUR_ZF_TOP_RESOLVE
#define MIMO_USE_SOLVE4_SCHUR_ZF_TOP_RESOLVE 0
#endif

#ifndef MIMO_USE_SOLVE4_SCHUR_ZF_HANDASM
#define MIMO_USE_SOLVE4_SCHUR_ZF_HANDASM 0
#endif

#ifndef MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER
#define MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER 0
#endif

#ifndef MIMO_USE_SOLVE4_SCHUR_ZF_I32_NUM
#define MIMO_USE_SOLVE4_SCHUR_ZF_I32_NUM 0
#endif

#ifndef MIMO_USE_SOLVE4_SCHUR_ZF_INLINE_HELPER
#define MIMO_USE_SOLVE4_SCHUR_ZF_INLINE_HELPER 0
#endif

#ifndef MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_MODEL
#define MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_MODEL 0
#endif

#ifndef MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_MODEL
#define MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_MODEL 0
#endif

#ifndef MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_ASM
#define MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_ASM 0
#endif

#ifndef MIMO_USE_SOLVE4_SCHUR_ZF_PIVOTSCALE_MODEL
#define MIMO_USE_SOLVE4_SCHUR_ZF_PIVOTSCALE_MODEL 0
#endif

#ifndef MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_ASM_STUB
#define MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_ASM_STUB 0
#endif

#ifndef MIMO_CHECK_SOLVE2H_GROUP_LAYOUT
#define MIMO_CHECK_SOLVE2H_GROUP_LAYOUT 0
#endif

#ifndef MIMO_CHECK_VSOLVE2H2_LAYOUT
#define MIMO_CHECK_VSOLVE2H2_LAYOUT 0
#endif

#ifndef MIMO_USE_EXT_ZF4_TRUE16B_WORK
#define MIMO_USE_EXT_ZF4_TRUE16B_WORK 0
#endif

#ifndef MIMO_USE_EXT_ZF4_PACK32_WORK
#define MIMO_USE_EXT_ZF4_PACK32_WORK 0
#endif

#ifndef MIMO_USE_SOLVE4_SCHUR_ZF_EXT_MACC_INNER
#define MIMO_USE_SOLVE4_SCHUR_ZF_EXT_MACC_INNER 0
#endif

#ifndef MIMO_USE_SOLVE4_SCHUR_ZF_Q15_NUM_SOLVE
#define MIMO_USE_SOLVE4_SCHUR_ZF_Q15_NUM_SOLVE 0
#endif

#ifndef MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE
#define MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE 0
#endif

#ifndef MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_BSOLVE
#define MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_BSOLVE 0
#endif

#ifndef MIMO_USE_EXT_ZF4_SCALAR_X23_STORE
#define MIMO_USE_EXT_ZF4_SCALAR_X23_STORE 0
#endif

#ifndef MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SPLIT_SOLVE
#define MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SPLIT_SOLVE 0
#endif

#ifndef MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_STORE
#define MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_STORE 0
#endif

#if MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_LOAD && \
    !MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE
#error "MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_LOAD currently requires full TRUE16B RVV i32 solve"
#endif

#if MIMO_USE_EXT_MMSE4_RVV_I32_VEC_LAMBDA && \
    !MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE
#error "MIMO_USE_EXT_MMSE4_RVV_I32_VEC_LAMBDA requires full TRUE16B RVV i32 solve"
#endif

#if MIMO_USE_EXT_MMSE4_RVV_I32_LATE_TAIL_LOAD && \
    (!MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE || \
     !MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_LOAD)
#error "MIMO_USE_EXT_MMSE4_RVV_I32_LATE_TAIL_LOAD requires full TRUE16B RVV i32 solve with pack32 load"
#endif

#if MIMO_USE_EXT_MMSE4_RVV_I32_SPECIALIZE && \
    (!MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE || \
     !MIMO_USE_EXT_MMSE4_RVV_I32_VEC_LAMBDA)
#error "MIMO_USE_EXT_MMSE4_RVV_I32_SPECIALIZE requires TRUE16B RVV i32 solve with vector lambda"
#endif

#if MIMO_USE_EXT_MMSE4_RVV_I32_INNER_Q30_ONCE && \
    !MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE
#error "MIMO_USE_EXT_MMSE4_RVV_I32_INNER_Q30_ONCE requires TRUE16B RVV i32 solve"
#endif

#if MIMO_USE_EXT_MMSE4_RVV_I32_TOP_Q30_ONCE && \
    !MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE
#error "MIMO_USE_EXT_MMSE4_RVV_I32_TOP_Q30_ONCE requires TRUE16B RVV i32 solve"
#endif

#if MIMO_USE_EXT_MMSE4_RVV_I32_DIAG_VLSE16_LOAD && \
    (!MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE || \
     !MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_LOAD)
#error "MIMO_USE_EXT_MMSE4_RVV_I32_DIAG_VLSE16_LOAD requires TRUE16B RVV i32 solve with pack32 load"
#endif

#if MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_BSOLVE && \
    !MIMO_USE_EXT_ZF4_TRUE16B_WORK
#error "MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_BSOLVE requires TRUE16B work"
#endif

#if MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_BSOLVE && \
    MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE
#error "MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_BSOLVE is a partial-solve candidate; do not combine it with full RVV i32 solve"
#endif

#if MIMO_USE_EXT_ZF4_SCALAR_X23_STORE && \
    (!MIMO_USE_EXT_ZF4_TRUE16B_WORK || MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER != 5)
#error "MIMO_USE_EXT_ZF4_SCALAR_X23_STORE requires TRUE16B work and EXT_INNER=5"
#endif

#if MIMO_USE_SOLVE4_SCHUR_ZF_Q15_NUM_SOLVE && \
    (!MIMO_USE_EXT_ZF4_TRUE16B_WORK || \
     MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER != 5 || \
     MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 5 || \
     MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT != 13)
#error "MIMO_USE_SOLVE4_SCHUR_ZF_Q15_NUM_SOLVE only supports current TRUE16B EXT_INNER=5 mode5 shift13 path"
#endif

#if MIMO_USE_SOLVE4_SCHUR_ZF_Q15_NUM_SOLVE && \
    MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_BSOLVE
#error "MIMO_USE_SOLVE4_SCHUR_ZF_Q15_NUM_SOLVE is mutually exclusive with BSOLVE"
#endif

#if MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE || \
    MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_BSOLVE
#include <riscv_vector.h>
#endif

#ifndef MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT
#define MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT 15
#endif

#ifndef MIMO_USE_EQ4_DIRECT_H
#define MIMO_USE_EQ4_DIRECT_H 0
#endif

#ifndef MIMO_EQ4_DIRECT_H_MODE
#define MIMO_EQ4_DIRECT_H_MODE 0
#endif

#ifndef MIMO_EQ4_DIRECT_H_ITERS
#define MIMO_EQ4_DIRECT_H_ITERS 3
#endif

#if MIMO_USE_SOLVE4_SCHUR_Q15_RECIP < 0 || \
    MIMO_USE_SOLVE4_SCHUR_Q15_RECIP > 5
#error "MIMO_USE_SOLVE4_SCHUR_Q15_RECIP supports 0=off, 1=step16 interpolation, 2=step4 direct table, 3=step16 trust-positive fastpath, 4=step16 signed-shift diagnostic, 5=step1 direct table diagnostic"
#endif

#if MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT < 12 || \
    MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT > 15
#error "MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT supports 12..15 for 32-bit product range"
#endif

#if MIMO_USE_EQ4_DIRECT_H < 0 || MIMO_USE_EQ4_DIRECT_H > 1
#error "MIMO_USE_EQ4_DIRECT_H supports 0=off, 1=direct-H diagnostic"
#endif

#if MIMO_EQ4_DIRECT_H_MODE < 0 || MIMO_EQ4_DIRECT_H_MODE > 3
#error "MIMO_EQ4_DIRECT_H_MODE supports 0=off, 1=GE, 2=Jacobi, 3=GS"
#endif

#if MIMO_EQ4_DIRECT_H_ITERS < 0 || MIMO_EQ4_DIRECT_H_ITERS > 8
#error "MIMO_EQ4_DIRECT_H_ITERS supports 0..8 diagnostic iterations"
#endif

#if MIMO_USE_EQ4_DIRECT_H && MIMO_EQ4_DIRECT_H_MODE == 0
#error "MIMO_USE_EQ4_DIRECT_H requires MIMO_EQ4_DIRECT_H_MODE=1(GE), 2(Jacobi), or 3(GS)"
#endif

#if ((MIMO_USE_SOLVE4_SCHUR_BINV_Q15 != 0) + \
     (MIMO_USE_SOLVE4_SCHUR_DET_SCALE != 0) + \
     (MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 0)) > 1
#error "MIMO_USE_SOLVE4_SCHUR_* diagnostics are mutually exclusive"
#endif

#if MIMO_USE_SOLVE4_SCHUR_FUSED_INNER && !MIMO_USE_SOLVE4_SCHUR_Q15_RECIP
#error "MIMO_USE_SOLVE4_SCHUR_FUSED_INNER is only defined for Schur q15recip/no-harddiv"
#endif

#if MIMO_USE_SOLVE4_SCHUR_FUSED_INNER_MMSE && !MIMO_USE_SOLVE4_SCHUR_Q15_RECIP
#error "MIMO_USE_SOLVE4_SCHUR_FUSED_INNER_MMSE is only defined for Schur q15recip/no-harddiv"
#endif

#if MIMO_USE_SOLVE4_SCHUR_ZF_EXPLICIT && !MIMO_USE_SOLVE4_SCHUR_Q15_RECIP
#error "MIMO_USE_SOLVE4_SCHUR_ZF_EXPLICIT is only defined for Schur q15recip/no-harddiv"
#endif

#if MIMO_USE_SOLVE4_SCHUR_ZF_RELOAD_MICRO && \
    (MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 4 || \
     MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT != 13)
#error "MIMO_USE_SOLVE4_SCHUR_ZF_RELOAD_MICRO only preserves the current Schur q15recip mode4 num_shift=13 path"
#endif

#if MIMO_USE_SOLVE4_SCHUR_ZF_RELOAD_MICRO && \
    MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED
#error "MIMO_USE_SOLVE4_SCHUR_ZF_RELOAD_MICRO expects the current unpacked upper-SoA work slots"
#endif

#if MIMO_USE_SOLVE4_SCHUR_ZF_RELOAD_MICRO && \
    MIMO_USE_SOLVE4_SCHUR_ZF_EXPLICIT
#error "MIMO_USE_SOLVE4_SCHUR_ZF_RELOAD_MICRO is mutually exclusive with MIMO_USE_SOLVE4_SCHUR_ZF_EXPLICIT"
#endif

#if MIMO_USE_SOLVE4_SCHUR_ZF_TOP_RESOLVE && \
    (!MIMO_USE_SOLVE4_SCHUR_Q15_RECIP || MIMO_USE_SOLVE4_SCHUR_FUSED_INNER)
#error "MIMO_USE_SOLVE4_SCHUR_ZF_TOP_RESOLVE requires non-fused Schur q15recip/no-harddiv"
#endif

#if MIMO_USE_SOLVE4_SCHUR_SCALE16_APPROX && \
    ((MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 4 && \
      MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 5) || \
     MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT != 13)
#error "MIMO_USE_SOLVE4_SCHUR_SCALE16_APPROX is a ZF signed-shift diagnostic for q15recip mode4/5 num_shift=13"
#endif

#if MIMO_USE_SOLVE4_SCHUR_ZF_HANDASM && \
    ((MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 4 && \
      MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 5) || \
     MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT != 13 || \
     MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED || \
     MIMO_USE_SOLVE4_SCHUR_ZF_EXPLICIT || \
     MIMO_USE_SOLVE4_SCHUR_ZF_RELOAD_MICRO || \
     MIMO_USE_SOLVE4_SCHUR_ZF_TOP_RESOLVE || \
     MIMO_USE_SOLVE4_SCHUR_FUSED_INNER || \
     MIMO_USE_SOLVE4_SCHUR_PAIR2 || MIMO_USE_SOLVE4_SCHUR_DET_SCALE || \
     MIMO_USE_SOLVE4_SCHUR_BINV_Q15 || MIMO_COLLECT_SCHUR_STATS)
#error "MIMO_USE_SOLVE4_SCHUR_ZF_HANDASM only preserves unpacked Schur q15recip modes 4/5 num_shift=13 ZF path"
#endif

#if MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER && \
    (((MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER >= 3) && \
      MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 4 && \
      MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 5) || \
     ((MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER < 3) && \
      MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 4) || \
     MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT != 13 || \
     MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED || \
     MIMO_USE_SOLVE4_SCHUR_ZF_EXPLICIT || \
     MIMO_USE_SOLVE4_SCHUR_ZF_RELOAD_MICRO || \
     MIMO_USE_SOLVE4_SCHUR_ZF_TOP_RESOLVE || \
     MIMO_USE_SOLVE4_SCHUR_ZF_HANDASM || \
     MIMO_USE_SOLVE4_SCHUR_FUSED_INNER || \
     MIMO_USE_SOLVE4_SCHUR_PAIR2 || MIMO_USE_SOLVE4_SCHUR_DET_SCALE || \
     MIMO_USE_SOLVE4_SCHUR_BINV_Q15 || MIMO_COLLECT_SCHUR_STATS)
#error "MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER supports unpacked ZF Schur q15recip mode4 num_shift=13; modes 3/4/5 also allow mode5"
#endif

#if MIMO_USE_SOLVE4_SCHUR_ZF_EXPLICIT && \
    MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 4 && \
    MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 5
#error "MIMO_USE_SOLVE4_SCHUR_ZF_EXPLICIT preserves only the signed-shift q15recip modes 4/5"
#endif

#if MIMO_USE_SOLVE4_SCHUR_ZF_EXPLICIT < 0 || \
    MIMO_USE_SOLVE4_SCHUR_ZF_EXPLICIT > 3
#error "MIMO_USE_SOLVE4_SCHUR_ZF_EXPLICIT supports 0=off, 1=i64 explicit, 2=i32-state explicit, 3=i16-state diagnostic"
#endif

#if MIMO_USE_SOLVE4_SCHUR_ZF_RELOAD_MICRO < 0 || \
    MIMO_USE_SOLVE4_SCHUR_ZF_RELOAD_MICRO > 2
#error "MIMO_USE_SOLVE4_SCHUR_ZF_RELOAD_MICRO supports 0=off, 1=volatile reload, 2=nonvolatile reload"
#endif

#if MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER < 0 || \
    MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER > 5
#error "MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER supports 0=off, 1=C pack/unpack, 2=fused vector inner/sub, 3=single-pack fused Schur/top, 4=inner3 direct output store, 5=inner3 direct complex-pair output store"
#endif

#if MIMO_USE_SOLVE4_SCHUR_ZF_I32_NUM && \
    (!MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER || \
     (MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 4 && \
      MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 5) || \
     MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT != 13)
#error "MIMO_USE_SOLVE4_SCHUR_ZF_I32_NUM is only defined for ZF Schur q15recip modes 4/5 num_shift=13 with EXT_INNER enabled"
#endif

#if MIMO_USE_SOLVE4_SCHUR_ZF_INLINE_HELPER && \
    (MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER < 3 || \
     MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER > 5)
#error "MIMO_USE_SOLVE4_SCHUR_ZF_INLINE_HELPER is only defined for ZF Schur EXT_INNER 3/4/5"
#endif

#if MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_MODEL && \
    (MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER < 3 || \
     MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER > 5 || \
     (MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 4 && \
      MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 5) || \
     MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT != 13)
#error "MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_MODEL is only defined for ZF Schur EXT_INNER 3/4/5 q15recip modes 4/5 num_shift=13"
#endif

#if MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_MODEL && \
    (MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER < 3 || \
     MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER > 5 || \
     (MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 4 && \
      MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 5) || \
     MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT != 13)
#error "MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_MODEL is only defined for ZF Schur EXT_INNER 3/4/5 q15recip modes 4/5 num_shift=13"
#endif

#if MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_ASM && \
    (MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER < 3 || \
     MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER > 5 || \
     (MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 4 && \
      MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 5) || \
     MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT != 13)
#error "MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_ASM is only defined for ZF Schur EXT_INNER 3/4/5 q15recip modes 4/5 num_shift=13"
#endif

#if MIMO_USE_SOLVE4_SCHUR_ZF_PIVOTSCALE_MODEL && \
    (MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER < 3 || \
     MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER > 5 || \
     (MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 4 && \
      MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 5) || \
     MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT != 13)
#error "MIMO_USE_SOLVE4_SCHUR_ZF_PIVOTSCALE_MODEL is only defined for ZF Schur EXT_INNER 3/4/5 q15recip modes 4/5 num_shift=13"
#endif

#if MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_MODEL && \
    MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_MODEL
#error "MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_MODEL and MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_MODEL are mutually exclusive"
#endif

#if MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_MODEL && \
    MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_ASM
#error "MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_MODEL and MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_ASM are mutually exclusive"
#endif

#if MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_MODEL && \
    MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_ASM
#error "MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_MODEL and MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_ASM are mutually exclusive"
#endif

#if MIMO_USE_SOLVE4_SCHUR_ZF_PIVOTSCALE_MODEL && \
    (MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_MODEL || \
     MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_MODEL || \
     MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_ASM)
#error "MIMO_USE_SOLVE4_SCHUR_ZF_PIVOTSCALE_MODEL is mutually exclusive with solve2h/vsolve2h2 model and asm paths"
#endif

#if MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_ASM_STUB && \
    (MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER != 5 || \
     MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 5 || \
     MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT != 13 || \
     MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_MODEL || \
     MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_MODEL || \
     MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_ASM || \
     MIMO_USE_SOLVE4_SCHUR_ZF_PIVOTSCALE_MODEL || \
     MIMO_CHECK_SOLVE2H_GROUP_LAYOUT)
#error "MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_ASM_STUB is build-only for current ZF Schur EXT_INNER=5 q15recip mode5 shift=13; it is not a model or executable correctness path"
#endif

#if MIMO_CHECK_SOLVE2H_GROUP_LAYOUT && \
    !MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_MODEL
#error "MIMO_CHECK_SOLVE2H_GROUP_LAYOUT requires MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_MODEL"
#endif

#if MIMO_CHECK_VSOLVE2H2_LAYOUT && \
    !MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_MODEL && \
    !MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_ASM
#error "MIMO_CHECK_VSOLVE2H2_LAYOUT requires MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_MODEL or MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_ASM"
#endif

#if MIMO_USE_SOLVE4_SCHUR_PAIR2 && MIMO_USE_SOLVE4_SCHUR_BINV_Q15
#error "MIMO_USE_SOLVE4_SCHUR_PAIR2 is only defined for direct 2x2 Schur solve"
#endif

#ifndef MIMO_USE_SOLVE_SKIP_LOWER_UPDATE
#define MIMO_USE_SOLVE_SKIP_LOWER_UPDATE 0
#endif

#ifndef MIMO_USE_SITE4_SOLVE_SOA
#define MIMO_USE_SITE4_SOLVE_SOA 0
#endif

#ifndef MIMO_USE_SITE4_SOLVE_SOA_UNROLL
#define MIMO_USE_SITE4_SOLVE_SOA_UNROLL 0
#endif

#ifndef MIMO_USE_SITE4_HERMITIAN_UPPER_BUILD
#define MIMO_USE_SITE4_HERMITIAN_UPPER_BUILD 0
#endif

#ifndef MIMO_COLLECT_SOLVE_DEN_STATS
#define MIMO_COLLECT_SOLVE_DEN_STATS 0
#endif

#ifndef MIMO_COLLECT_SOLVE_VALUE_STATS
#define MIMO_COLLECT_SOLVE_VALUE_STATS 0
#endif

#ifndef MIMO_COLLECT_SCHUR_STATS
#define MIMO_COLLECT_SCHUR_STATS 0
#endif

#ifndef MIMO_COLLECT_PIVOT_SCALE_STATS
#define MIMO_COLLECT_PIVOT_SCALE_STATS 0
#endif

typedef struct {
    int64_t r;
    int64_t i;
} mimo_c64_t;

static void mimo_print_solve_value_stats(void);
static void mimo_print_schur_stats(void);
static void mimo_print_pivot_scale_stats(void);

#if MIMO_COLLECT_SOLVE_DEN_STATS || MIMO_COLLECT_SOLVE_VALUE_STATS || \
    MIMO_COLLECT_SCHUR_STATS
static uint64_t abs_i64_for_stats(int64_t x)
{
    if (x < 0) {
        return (uint64_t)(-(x + 1)) + 1;
    }
    return (uint64_t)x;
}
#endif

#if MIMO_COLLECT_SOLVE_DEN_STATS
typedef struct {
    uint64_t count;
    uint64_t zero;
    uint64_t abs_min;
    uint64_t abs_max;
    uint64_t le_32767;
    uint64_t le_65535;
    uint64_t le_131071;
    uint64_t gt_131071;
} mimo_den_stats_t;

static mimo_den_stats_t solve_factor_den_stats;
static mimo_den_stats_t solve_back_den_stats;

static void record_den_stat(mimo_den_stats_t *stats, int64_t den)
{
    const uint64_t mag = abs_i64_for_stats(den);

    if (den == 0) {
        ++stats->zero;
    }
    if (stats->count == 0 || mag < stats->abs_min) {
        stats->abs_min = mag;
    }
    if (mag > stats->abs_max) {
        stats->abs_max = mag;
    }
    if (mag <= 32767u) {
        ++stats->le_32767;
    } else if (mag <= 65535u) {
        ++stats->le_65535;
    } else if (mag <= 131071u) {
        ++stats->le_131071;
    } else {
        ++stats->gt_131071;
    }
    ++stats->count;
}

static inline int64_t record_factor_den(int64_t den)
{
    record_den_stat(&solve_factor_den_stats, den);
    return den;
}

static inline int64_t record_back_den(int64_t den)
{
    record_den_stat(&solve_back_den_stats, den);
    return den;
}

static void print_den_stat(const char *name, const mimo_den_stats_t *stats)
{
    printf("MIMO_SOLVE_DEN_STATS kind=%s count=%llu zero=%llu abs_min=%llu abs_max=%llu le32767=%llu le65535=%llu le131071=%llu gt131071=%llu\n",
           name,
           (unsigned long long)stats->count,
           (unsigned long long)stats->zero,
           (unsigned long long)stats->abs_min,
           (unsigned long long)stats->abs_max,
           (unsigned long long)stats->le_32767,
           (unsigned long long)stats->le_65535,
           (unsigned long long)stats->le_131071,
           (unsigned long long)stats->gt_131071);
}

void mimo_print_solve_den_stats(void)
{
    print_den_stat("factor", &solve_factor_den_stats);
    print_den_stat("back", &solve_back_den_stats);
    mimo_print_solve_value_stats();
    mimo_print_schur_stats();
    mimo_print_pivot_scale_stats();
}
#else
#define record_factor_den(den) (den)
#define record_back_den(den) (den)

void mimo_print_solve_den_stats(void)
{
    mimo_print_solve_value_stats();
    mimo_print_schur_stats();
    mimo_print_pivot_scale_stats();
}
#endif

#if MIMO_COLLECT_SOLVE_VALUE_STATS
typedef struct {
    uint64_t count;
    uint64_t zero;
    uint64_t abs_min;
    uint64_t abs_max;
    uint64_t le_32767;
    uint64_t le_65535;
    uint64_t le_131071;
    uint64_t le_2147483647;
    uint64_t gt_2147483647;
} mimo_value_stats_t;

static mimo_value_stats_t solve_mat_init_stats;
static mimo_value_stats_t solve_rhs_init_stats;
static mimo_value_stats_t solve_mat_diag_mmse_stats;
static mimo_value_stats_t solve_factor_num_stats;
static mimo_value_stats_t solve_factor_q30_stats;
static mimo_value_stats_t solve_elim_mul_stats;
static mimo_value_stats_t solve_mat_update_stats;
static mimo_value_stats_t solve_rhs_update_stats;
static mimo_value_stats_t solve_back_sum_stats;
static mimo_value_stats_t solve_sol_stats;
static mimo_value_stats_t solve_store_input_stats;

static void record_value_stat(mimo_value_stats_t *stats, int64_t value)
{
    const uint64_t mag = abs_i64_for_stats(value);

    if (value == 0) {
        ++stats->zero;
    }
    if (stats->count == 0 || mag < stats->abs_min) {
        stats->abs_min = mag;
    }
    if (mag > stats->abs_max) {
        stats->abs_max = mag;
    }
    if (mag <= 32767u) {
        ++stats->le_32767;
    } else if (mag <= 65535u) {
        ++stats->le_65535;
    } else if (mag <= 131071u) {
        ++stats->le_131071;
    } else if (mag <= 2147483647u) {
        ++stats->le_2147483647;
    } else {
        ++stats->gt_2147483647;
    }
    ++stats->count;
}

static inline void record_value_stat_c64(mimo_value_stats_t *stats,
                                         mimo_c64_t value)
{
    record_value_stat(stats, value.r);
    record_value_stat(stats, value.i);
}

static void print_value_stat(const char *name, const mimo_value_stats_t *stats)
{
    printf("MIMO_SOLVE_VALUE_STATS kind=%s count=%llu zero=%llu abs_min=%llu abs_max=%llu le32767=%llu le65535=%llu le131071=%llu le2147483647=%llu gt2147483647=%llu\n",
           name,
           (unsigned long long)stats->count,
           (unsigned long long)stats->zero,
           (unsigned long long)stats->abs_min,
           (unsigned long long)stats->abs_max,
           (unsigned long long)stats->le_32767,
           (unsigned long long)stats->le_65535,
           (unsigned long long)stats->le_131071,
           (unsigned long long)stats->le_2147483647,
           (unsigned long long)stats->gt_2147483647);
}

static void mimo_print_solve_value_stats(void)
{
    print_value_stat("mat_init", &solve_mat_init_stats);
    print_value_stat("rhs_init", &solve_rhs_init_stats);
    print_value_stat("mat_diag_mmse", &solve_mat_diag_mmse_stats);
    print_value_stat("factor_num", &solve_factor_num_stats);
    print_value_stat("factor_q30", &solve_factor_q30_stats);
    print_value_stat("elim_mul", &solve_elim_mul_stats);
    print_value_stat("mat_update", &solve_mat_update_stats);
    print_value_stat("rhs_update", &solve_rhs_update_stats);
    print_value_stat("back_sum", &solve_back_sum_stats);
    print_value_stat("sol", &solve_sol_stats);
    print_value_stat("store_input", &solve_store_input_stats);
}

#define RECORD_SOLVE_VALUE_C64(STATS, VALUE) \
    record_value_stat_c64(&(STATS), (VALUE))
#define RECORD_SOLVE_VALUE_NUM(STATS, VALUE) \
    record_value_stat(&(STATS), (VALUE))
#else
static void mimo_print_solve_value_stats(void)
{
}

#define RECORD_SOLVE_VALUE_C64(STATS, VALUE) ((void)0)
#define RECORD_SOLVE_VALUE_NUM(STATS, VALUE) ((void)0)
#endif

#if MIMO_COLLECT_SCHUR_STATS
typedef struct {
    uint64_t count;
    uint64_t zero;
    uint64_t abs_min;
    uint64_t abs_max;
    uint64_t le_32767;
    uint64_t le_65535;
    uint64_t le_131071;
    uint64_t le_2147483647;
    uint64_t gt_2147483647;
} mimo_schur_stats_t;

static mimo_schur_stats_t schur_a_diag_stats;
static mimo_schur_stats_t schur_input_upper_q15_stats;
static mimo_schur_stats_t schur_input_rhs_q15_stats;
static mimo_schur_stats_t schur_b_det_q30_stats;
static mimo_schur_stats_t schur_s_det_q30_stats;
static mimo_schur_stats_t schur_solve_num_q30_stats;
static mimo_schur_stats_t schur_b_inv_vec_q15_stats;
static mimo_schur_stats_t schur_update_q15_stats;
static mimo_schur_stats_t schur_block_q15_stats;
static mimo_schur_stats_t schur_rhs_q15_stats;

static void record_schur_stat(mimo_schur_stats_t *stats, int64_t value)
{
    const uint64_t mag = abs_i64_for_stats(value);

    if (value == 0) {
        ++stats->zero;
    }
    if (stats->count == 0 || mag < stats->abs_min) {
        stats->abs_min = mag;
    }
    if (mag > stats->abs_max) {
        stats->abs_max = mag;
    }
    if (mag <= 32767u) {
        ++stats->le_32767;
    } else if (mag <= 65535u) {
        ++stats->le_65535;
    } else if (mag <= 131071u) {
        ++stats->le_131071;
    } else if (mag <= 2147483647u) {
        ++stats->le_2147483647;
    } else {
        ++stats->gt_2147483647;
    }
    ++stats->count;
}

static void record_schur_stat_c64(mimo_schur_stats_t *stats,
                                  mimo_c64_t value)
{
    record_schur_stat(stats, value.r);
    record_schur_stat(stats, value.i);
}

static void print_schur_stat(const char *name,
                             const mimo_schur_stats_t *stats)
{
    printf("MIMO_SCHUR_STATS kind=%s count=%llu zero=%llu abs_min=%llu abs_max=%llu le32767=%llu le65535=%llu le131071=%llu le2147483647=%llu gt2147483647=%llu\n",
           name,
           (unsigned long long)stats->count,
           (unsigned long long)stats->zero,
           (unsigned long long)stats->abs_min,
           (unsigned long long)stats->abs_max,
           (unsigned long long)stats->le_32767,
           (unsigned long long)stats->le_65535,
           (unsigned long long)stats->le_131071,
           (unsigned long long)stats->le_2147483647,
           (unsigned long long)stats->gt_2147483647);
}

static void mimo_print_schur_stats(void)
{
    print_schur_stat("a_diag", &schur_a_diag_stats);
    print_schur_stat("input_upper_q15", &schur_input_upper_q15_stats);
    print_schur_stat("input_rhs_q15", &schur_input_rhs_q15_stats);
    print_schur_stat("b_det_q30", &schur_b_det_q30_stats);
    print_schur_stat("s_det_q30", &schur_s_det_q30_stats);
    print_schur_stat("solve_num_q30", &schur_solve_num_q30_stats);
    print_schur_stat("b_inv_vec_q15", &schur_b_inv_vec_q15_stats);
    print_schur_stat("schur_update_q15", &schur_update_q15_stats);
    print_schur_stat("schur_block_q15", &schur_block_q15_stats);
    print_schur_stat("schur_rhs_q15", &schur_rhs_q15_stats);
}

#define RECORD_SCHUR_NUM(STATS, VALUE) record_schur_stat(&(STATS), (VALUE))
#define RECORD_SCHUR_C64(STATS, VALUE) record_schur_stat_c64(&(STATS), (VALUE))
#else
static void mimo_print_schur_stats(void)
{
}

#define RECORD_SCHUR_NUM(STATS, VALUE) ((void)0)
#define RECORD_SCHUR_C64(STATS, VALUE) ((void)0)
#endif

#if MIMO_COLLECT_PIVOT_SCALE_STATS
typedef struct {
    uint64_t calls;
    uint64_t jump_changed;
    uint64_t jump_delta_sum;
    uint64_t while_dec_steps;
    uint64_t while_inc_steps;
    uint64_t while_dec_enter;
    uint64_t while_inc_enter;
    uint64_t while_max_steps;
} mimo_pivot_corr_stats_t;

static mimo_pivot_corr_stats_t pivot_corr_q15_stats;
static mimo_pivot_corr_stats_t pivot_corr_q30_stats;
static mimo_pivot_corr_stats_t pivot_corr_q15_stage_stats[4];
static mimo_pivot_corr_stats_t pivot_corr_q30_stage_stats[6];
static int pivot_corr_q15_stage = -1;
static int pivot_corr_q15_stage_active = 0;
static int pivot_corr_q30_stage = -1;
static int pivot_corr_q30_stage_active = 0;

static const char *const pivot_corr_q15_stage_names[4] = {
    "q15_x3_p3",
    "q15_x2_p2",
    "q15_x1_p1",
    "q15_x0_p0",
};

static const char *const pivot_corr_q30_stage_names[6] = {
    "q30_f01_p0",
    "q30_f02_p0",
    "q30_f03_p0",
    "q30_f12_p1",
    "q30_f13_p1",
    "q30_f23_p2",
};

static void record_pivot_corr_stat(mimo_pivot_corr_stats_t *stats,
                                   uint32_t before_jump,
                                   uint32_t after_jump,
                                   uint32_t dec_steps,
                                   uint32_t inc_steps)
{
    ++stats->calls;
    if (before_jump != after_jump) {
        ++stats->jump_changed;
        stats->jump_delta_sum +=
            before_jump > after_jump
                ? (uint64_t)(before_jump - after_jump)
                : (uint64_t)(after_jump - before_jump);
    }
    stats->while_dec_steps += dec_steps;
    stats->while_inc_steps += inc_steps;
    if (dec_steps != 0) {
        ++stats->while_dec_enter;
    }
    if (inc_steps != 0) {
        ++stats->while_inc_enter;
    }
    if ((uint64_t)dec_steps + inc_steps > stats->while_max_steps) {
        stats->while_max_steps = (uint64_t)dec_steps + inc_steps;
    }
}

static void print_pivot_corr_stat(const char *name,
                                  const mimo_pivot_corr_stats_t *stats)
{
    printf("MIMO_PIVOT_SCALE_STATS kind=%s calls=%llu jump_changed=%llu "
           "jump_delta_sum=%llu while_dec_steps=%llu while_inc_steps=%llu "
           "while_dec_enter=%llu while_inc_enter=%llu while_max_steps=%llu\n",
           name,
           (unsigned long long)stats->calls,
           (unsigned long long)stats->jump_changed,
           (unsigned long long)stats->jump_delta_sum,
           (unsigned long long)stats->while_dec_steps,
           (unsigned long long)stats->while_inc_steps,
           (unsigned long long)stats->while_dec_enter,
           (unsigned long long)stats->while_inc_enter,
           (unsigned long long)stats->while_max_steps);
}

static void mimo_print_pivot_scale_stats(void)
{
    print_pivot_corr_stat("q15", &pivot_corr_q15_stats);
    print_pivot_corr_stat("q30", &pivot_corr_q30_stats);
    for (int i = 0; i < 4; ++i) {
        if (pivot_corr_q15_stage_stats[i].calls != 0) {
            print_pivot_corr_stat(pivot_corr_q15_stage_names[i],
                                  &pivot_corr_q15_stage_stats[i]);
        }
    }
    for (int i = 0; i < 6; ++i) {
        if (pivot_corr_q30_stage_stats[i].calls != 0) {
            print_pivot_corr_stat(pivot_corr_q30_stage_names[i],
                                  &pivot_corr_q30_stage_stats[i]);
        }
    }
}
#define RECORD_PIVOT_CORR_Q15(BEFORE, AFTER, DEC, INC) \
    do { \
        record_pivot_corr_stat(&pivot_corr_q15_stats, (BEFORE), (AFTER), \
                               (DEC), (INC)); \
        if (pivot_corr_q15_stage_active && pivot_corr_q15_stage >= 0 && \
            pivot_corr_q15_stage < 4) { \
            record_pivot_corr_stat( \
                &pivot_corr_q15_stage_stats[pivot_corr_q15_stage], \
                (BEFORE), (AFTER), (DEC), (INC)); \
        } \
    } while (0)
#define RECORD_PIVOT_CORR_Q30(BEFORE, AFTER, DEC, INC) \
    do { \
        record_pivot_corr_stat(&pivot_corr_q30_stats, (BEFORE), (AFTER), \
                               (DEC), (INC)); \
        if (pivot_corr_q30_stage_active && pivot_corr_q30_stage >= 0 && \
            pivot_corr_q30_stage < 6) { \
            record_pivot_corr_stat( \
                &pivot_corr_q30_stage_stats[pivot_corr_q30_stage], \
                (BEFORE), (AFTER), (DEC), (INC)); \
        } \
    } while (0)
#define PIVOT_Q15_STAGE_BEGIN(ID) \
    do { \
        pivot_corr_q15_stage = (ID); \
        pivot_corr_q15_stage_active = 1; \
    } while (0)
#define PIVOT_Q15_STAGE_END() \
    do { \
        pivot_corr_q15_stage_active = 0; \
        pivot_corr_q15_stage = -1; \
    } while (0)
#define PIVOT_Q30_STAGE_BEGIN(ID) \
    do { \
        pivot_corr_q30_stage = (ID); \
        pivot_corr_q30_stage_active = 1; \
    } while (0)
#define PIVOT_Q30_STAGE_END() \
    do { \
        pivot_corr_q30_stage_active = 0; \
        pivot_corr_q30_stage = -1; \
    } while (0)
#else
static void mimo_print_pivot_scale_stats(void)
{
}
#define RECORD_PIVOT_CORR_Q15(BEFORE, AFTER, DEC, INC) \
    do { \
        (void)(BEFORE); \
        (void)(AFTER); \
        (void)(DEC); \
        (void)(INC); \
    } while (0)
#define RECORD_PIVOT_CORR_Q30(BEFORE, AFTER, DEC, INC) \
    do { \
        (void)(BEFORE); \
        (void)(AFTER); \
        (void)(DEC); \
        (void)(INC); \
    } while (0)
#define PIVOT_Q15_STAGE_BEGIN(ID) \
    do { \
        (void)(ID); \
    } while (0)
#define PIVOT_Q15_STAGE_END() \
    do { \
    } while (0)
#define PIVOT_Q30_STAGE_BEGIN(ID) \
    do { \
        (void)(ID); \
    } while (0)
#define PIVOT_Q30_STAGE_END() \
    do { \
    } while (0)
#endif

#if MIMO_USE_RECIP_TABLE
#define MIMO_RECIP_TABLE_MAX 32767
#if MIMO_USE_RECIP_TABLE == 2
#define MIMO_RECIP_TABLE_SHIFT 63
#else
#define MIMO_RECIP_TABLE_SHIFT 45
#endif

static uint64_t recip_q45_table[MIMO_RECIP_TABLE_MAX + 1] MIMO_ALIGN64;
#endif

#if MIMO_USE_SOLVE4_HERMITIAN_FULL_RECIP_TABLE
static uint32_t herm_pivot_full_recip_q30[MIMO_HERM_PIVOT_RECIP_TABLE_SIZE]
    MIMO_ALIGN64;

static inline uint32_t herm_pivot_recip_scale_q30_interp_from_mag(
    uint32_t mag);
#endif

#if MIMO_USE_SOLVE4_HERMITIAN_FLOOR_FULL_RECIP_TABLE
#if MIMO_USE_SOLVE4_HERMITIAN_RECIP_STEP16
static const uint32_t
    herm_pivot_floor_step16_add1_bits[MIMO_HERM_PIVOT_RECIP_BITSET_WORDS] = {
        0x00052101u, 0x00010000u, 0x00000000u, 0x40800001u,
        0x49490949u, 0x24812493u, 0x00001113u, 0x30000001u,
        0x00000107u, 0x00000000u, 0x00000000u, 0xffffffffu,
        0xe001c001u, 0x00000101u, 0x8431c100u, 0x1199c621u,
        0x00011111u, 0x00000000u, 0x00058000u, 0x00000000u,
        0x00010000u, 0x40000000u, 0x00000001u, 0x00000001u,
        0x00000000u, 0x00000000u, 0x00000001u, 0x30000001u,
        0x08000107u, 0x000f0707u, 0x00ff0100u, 0x00010000u,
        0xfff80000u, 0x0001f801u, 0x00010000u, 0x00018100u,
        0x00000000u, 0x00000000u, 0x91000000u, 0x0005924du,
        0x21210000u, 0x0215a521u, 0x02010001u, 0x00000155u,
        0x00000000u,
    };

static const uint32_t
    herm_pivot_floor_step16_sub1_bits[MIMO_HERM_PIVOT_RECIP_BITSET_WORDS] = {
        0x00000000u, 0x00000080u, 0x00000000u, 0x00000000u,
        0x00000000u, 0x00000000u, 0x00000000u, 0x0008c420u,
        0xc0c08000u, 0xfc0080c0u, 0x0000ff00u, 0x00000000u,
        0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
        0x24000000u, 0x24926c92u, 0x50000000u, 0x4080084au,
        0x00000080u, 0x00000000u, 0x02000200u, 0xa4a4a420u,
        0x924824a4u, 0x024c926cu, 0x00000000u, 0x0008c420u,
        0x00208000u, 0xc0000000u, 0x00000000u, 0x00000000u,
        0x00000000u, 0x02000000u, 0x0000003eu, 0x08000000u,
        0x08421842u, 0x22222200u, 0x00002222u, 0x48000000u,
        0x00000002u, 0x00000000u, 0x00000000u, 0x00000000u,
        0x00000015u,
    };
#else
static const uint32_t
    herm_pivot_floor_step32_adj1_bits[MIMO_HERM_PIVOT_RECIP_BITSET_WORDS] = {
        0xaf7ad6b4u, 0xfffeffd5u, 0x55555555u, 0x142b54aau,
        0x02024202u, 0x48124800u, 0x33332220u, 0x0318cc66u,
        0xcfcf8e08u, 0xffffffffu, 0x00ffffffu, 0x00000000u,
        0x1e003e00u, 0xe1e1e0e0u, 0x638e3cf1u, 0xcc663198u,
        0xbf7eeeccu, 0xb6fffeffu, 0xffffffedu, 0xefffffffu,
        0xfffeffd5u, 0x15555555u, 0x56ab56aau, 0xefefef6au,
        0xffffffffu, 0x6edffeffu, 0x33333332u, 0x0318cc66u,
        0x10608208u, 0xc7f0f838u, 0xfffffeffu, 0xfffeffffu,
        0x0007ffffu, 0xfbfe07c0u, 0xf9feffffu, 0xefbe7cf1u,
        0xce7bde7bu, 0xbbbbbb99u, 0x0899bbbbu, 0xfffa4920u,
        0x96dedb6fu, 0xad6a5294u, 0xa854aa54u, 0x55555400u,
        0x00000015u,
    };

static const uint32_t
    herm_pivot_floor_step32_adj2_bits[MIMO_HERM_PIVOT_RECIP_BITSET_WORDS] = {
        0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
        0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
        0x00000000u, 0x04008000u, 0x00000000u, 0x00000000u,
        0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
        0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
        0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
        0x00002480u, 0x00000000u, 0x00000000u, 0x00000000u,
        0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
        0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
        0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
        0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
        0x00000000u,
    };
#endif

static inline uint32_t herm_pivot_recip_bit(
    const uint32_t bits[MIMO_HERM_PIVOT_RECIP_BITSET_WORDS],
    uint32_t rel)
{
    return (bits[rel >> 5] >> (rel & 31u)) & 1u;
}
#endif

void mimo_init_recip_table(void)
{
#if MIMO_USE_RECIP_TABLE
    if (recip_q45_table[1] != 0) {
        return;
    }

	recip_q45_table[0] = 0;
	for (uint32_t d = 1; d <= MIMO_RECIP_TABLE_MAX; ++d) {
#if MIMO_USE_RECIP_TABLE == 2
	    recip_q45_table[d] =
	        (((uint64_t)1 << MIMO_RECIP_TABLE_SHIFT) + (d >> 1)) / d;
#else
	    recip_q45_table[d] =
	        ((uint64_t)1 << MIMO_RECIP_TABLE_SHIFT) / d;
#endif
    }
#endif

#if MIMO_USE_SOLVE4_HERMITIAN_FULL_RECIP_TABLE
    if (herm_pivot_full_recip_q30[0] == 0) {
        for (uint32_t d = MIMO_HERM_PIVOT_RECIP_MIN;
             d <= MIMO_HERM_PIVOT_RECIP_MAX; ++d) {
            const uint32_t rel = d - MIMO_HERM_PIVOT_RECIP_MIN;
            uint32_t recip = herm_pivot_recip_scale_q30_interp_from_mag(d);
#if MIMO_USE_SOLVE4_HERMITIAN_FLOOR_FULL_RECIP_TABLE
#if MIMO_USE_SOLVE4_HERMITIAN_RECIP_STEP16
            recip += herm_pivot_recip_bit(herm_pivot_floor_step16_add1_bits,
                                           rel);
            recip -= herm_pivot_recip_bit(herm_pivot_floor_step16_sub1_bits,
                                           rel);
#else
            recip -= herm_pivot_recip_bit(herm_pivot_floor_step32_adj1_bits,
                                           rel);
            recip -= herm_pivot_recip_bit(herm_pivot_floor_step32_adj2_bits,
                                           rel);
#endif
#endif
            herm_pivot_full_recip_q30[rel] = recip;
        }
    }
#endif
}

mimo_c16_t mimo_make_c16(int16_t r, int16_t i)
{
    mimo_c16_t x = {r, i};
    return x;
}

int16_t mimo_sat_i16(int32_t x)
{
    if (x > 32767) {
        return 32767;
    }
    if (x < -32768) {
        return -32768;
    }
    return (int16_t)x;
}

static inline int16_t mimo_solve_store_i16(int32_t x)
{
#if MIMO_USE_SOLVE_BRANCHLIGHT_ROUND || MIMO_USE_SOLVE_STORE_TRUST_I16
    return (int16_t)x;
#else
    return mimo_sat_i16(x);
#endif
}

static int16_t q15_round_sat_i16(int64_t x)
{
    const int64_t y = (x + MIMO_Q15_ROUND) >> MIMO_Q15_SHIFT;

    if (y > 32767) {
        return 32767;
    }
    if (y < -32768) {
        return -32768;
    }
    return (int16_t)y;
}

mimo_c16_t mimo_cmul_q15(mimo_c16_t a, mimo_c16_t b)
{
    const int64_t ar = a.r;
    const int64_t ai = a.i;
    const int64_t br = b.r;
    const int64_t bi = b.i;

    return mimo_make_c16(q15_round_sat_i16(ar * br - ai * bi),
                         q15_round_sat_i16(ar * bi + ai * br));
}

mimo_c16_t mimo_cmul_conj_a_q15(mimo_c16_t a, mimo_c16_t b)
{
    const int64_t ar = a.r;
    const int64_t ai = a.i;
    const int64_t br = b.r;
    const int64_t bi = b.i;

    return mimo_make_c16(q15_round_sat_i16(ar * br + ai * bi),
                         q15_round_sat_i16(ar * bi - ai * br));
}

static inline mimo_c32_t mimo_cmul_conj_a_fast_q15_c32(mimo_c16_t a,
                                                       mimo_c16_t b)
{
    const int32_t ar = a.r;
    const int32_t ai = a.i;
    const int32_t br = b.r;
    const int32_t bi = b.i;
    const mimo_c32_t out = {
        (ar * br + ai * bi + MIMO_Q15_ROUND) >> MIMO_Q15_SHIFT,
        (ar * bi - ai * br + MIMO_Q15_ROUND) >> MIMO_Q15_SHIFT,
    };

    return out;
}

#define DEFINE_MIMO_BUILD_NORMAL_ROW(NAME, N) \
    static void NAME(const mimo_c16_t *h_row, \
                     const mimo_c16_t *y, \
                     mimo_c32_t *a, \
                     mimo_c32_t *b) \
    { \
        enum { n = (N) }; \
 \
        for (int col_i = 0; col_i < n; ++col_i) { \
            mimo_c32_t rhs = {0, 0}; \
 \
            for (int rx = 0; rx < n; ++rx) { \
                const mimo_c32_t prod = \
                    mimo_cmul_conj_a_fast_q15_c32(h_row[rx * n + col_i], y[rx]); \
                rhs.r += prod.r; \
                rhs.i += prod.i; \
            } \
            b[col_i] = rhs; \
 \
            for (int col_j = col_i; col_j < n; ++col_j) { \
                mimo_c32_t gram = {0, 0}; \
 \
                for (int rx = 0; rx < n; ++rx) { \
                    const mimo_c32_t prod = \
                        mimo_cmul_conj_a_fast_q15_c32(h_row[rx * n + col_i], \
                                                      h_row[rx * n + col_j]); \
                    gram.r += prod.r; \
                    gram.i += prod.i; \
                } \
 \
                a[col_i * n + col_j] = gram; \
                if (col_i == col_j) { \
                    a[col_j * n + col_i] = gram; \
                } else { \
                    a[col_j * n + col_i].r = gram.r; \
                    a[col_j * n + col_i].i = -gram.i; \
                } \
            } \
        } \
    }

static inline void mimo_acc_conj_q15_c32(mimo_c32_t *acc,
                                         mimo_c16_t a,
                                         mimo_c16_t b)
{
    const mimo_c32_t prod = mimo_cmul_conj_a_fast_q15_c32(a, b);

    acc->r += prod.r;
    acc->i += prod.i;
}

static inline void mimo_acc_conj_q15acc_c32(mimo_c32_t *acc,
                                            mimo_c16_t a,
                                            mimo_c16_t b)
{
    const mimo_c32_t prod = mimo_cmul_conj_a_fast_q15_c32(a, b);

    acc->r = mimo_sat_i16(acc->r + (int32_t)mimo_sat_i16(prod.r));
    acc->i = mimo_sat_i16(acc->i + (int32_t)mimo_sat_i16(prod.i));
}

#define MIMO_DOT4_HY_Q15ACC(H, Y, C) \
    ({ \
        mimo_c32_t acc = {0, 0}; \
        mimo_acc_conj_q15acc_c32(&acc, (H)[0 * MIMO_N4 + (C)], (Y)[0]); \
        mimo_acc_conj_q15acc_c32(&acc, (H)[1 * MIMO_N4 + (C)], (Y)[1]); \
        mimo_acc_conj_q15acc_c32(&acc, (H)[2 * MIMO_N4 + (C)], (Y)[2]); \
        mimo_acc_conj_q15acc_c32(&acc, (H)[3 * MIMO_N4 + (C)], (Y)[3]); \
        acc; \
    })

#define MIMO_DOT4_HH_Q15ACC(H, CI, CJ) \
    ({ \
        mimo_c32_t acc = {0, 0}; \
        mimo_acc_conj_q15acc_c32(&acc, (H)[0 * MIMO_N4 + (CI)], \
                                      (H)[0 * MIMO_N4 + (CJ)]); \
        mimo_acc_conj_q15acc_c32(&acc, (H)[1 * MIMO_N4 + (CI)], \
                                      (H)[1 * MIMO_N4 + (CJ)]); \
        mimo_acc_conj_q15acc_c32(&acc, (H)[2 * MIMO_N4 + (CI)], \
                                      (H)[2 * MIMO_N4 + (CJ)]); \
        mimo_acc_conj_q15acc_c32(&acc, (H)[3 * MIMO_N4 + (CI)], \
                                      (H)[3 * MIMO_N4 + (CJ)]); \
        acc; \
    })

#if MIMO_USE_SITE4_ROUND_ONCE_DOT
static inline int32_t mimo_round_once_q30_i64_to_i32(int64_t x)
{
    return (int32_t)((x + MIMO_Q15_ROUND) >> MIMO_Q15_SHIFT);
}

static inline void mimo_acc_conj_q30_i64(int64_t *acc_r,
                                         int64_t *acc_i,
                                         mimo_c16_t a,
                                         mimo_c16_t b)
{
    const int64_t ar = a.r;
    const int64_t ai = a.i;
    const int64_t br = b.r;
    const int64_t bi = b.i;

#if MIMO_USE_SCALAR4_COMPLEX_MUL_3M
    int64_t next_r = *acc_r;
    int64_t next_i = *acc_i;
    int64_t ar_work = ar;
    int64_t br_work = br;
    int64_t p1;
    int64_t p2;

    __asm__ volatile(
        "mul %[p1], %[ar], %[br]\n\t"
        "mul %[p2], %[ai], %[bi]\n\t"
        "sub %[ar], %[ar], %[ai]\n\t"
        "add %[br], %[br], %[bi]\n\t"
        "mul %[br], %[ar], %[br]\n\t"
        "add %[sum_r], %[sum_r], %[p1]\n\t"
        "add %[sum_r], %[sum_r], %[p2]\n\t"
        "sub %[br], %[br], %[p1]\n\t"
        "add %[br], %[br], %[p2]\n\t"
        "add %[sum_i], %[sum_i], %[br]"
        : [sum_r] "+&r"(next_r), [sum_i] "+&r"(next_i),
          [ar] "+&r"(ar_work), [br] "+&r"(br_work),
          [p1] "=&r"(p1), [p2] "=&r"(p2)
        : [ai] "r"(ai), [bi] "r"(bi));

    *acc_r = next_r;
    *acc_i = next_i;
#else
    *acc_r += ar * br + ai * bi;
    *acc_i += ar * bi - ai * br;
#endif
}

static inline void mimo_acc_abs2_q30_i64(int64_t *acc_r, mimo_c16_t a)
{
    const int64_t ar = a.r;
    const int64_t ai = a.i;

    *acc_r += ar * ar + ai * ai;
}

#define MIMO_DOT4_HY(H, Y, C) \
    ({ \
        int64_t acc_r = 0; \
        int64_t acc_i = 0; \
        mimo_acc_conj_q30_i64(&acc_r, &acc_i, (H)[0 * MIMO_N4 + (C)], \
                              (Y)[0]); \
        mimo_acc_conj_q30_i64(&acc_r, &acc_i, (H)[1 * MIMO_N4 + (C)], \
                              (Y)[1]); \
        mimo_acc_conj_q30_i64(&acc_r, &acc_i, (H)[2 * MIMO_N4 + (C)], \
                              (Y)[2]); \
        mimo_acc_conj_q30_i64(&acc_r, &acc_i, (H)[3 * MIMO_N4 + (C)], \
                              (Y)[3]); \
        mimo_c32_t acc = {mimo_round_once_q30_i64_to_i32(acc_r), \
                          mimo_round_once_q30_i64_to_i32(acc_i)}; \
        acc; \
    })

#define MIMO_DOT4_HH(H, CI, CJ) \
    ({ \
        int64_t acc_r = 0; \
        int64_t acc_i = 0; \
        mimo_acc_conj_q30_i64(&acc_r, &acc_i, (H)[0 * MIMO_N4 + (CI)], \
                              (H)[0 * MIMO_N4 + (CJ)]); \
        mimo_acc_conj_q30_i64(&acc_r, &acc_i, (H)[1 * MIMO_N4 + (CI)], \
                              (H)[1 * MIMO_N4 + (CJ)]); \
        mimo_acc_conj_q30_i64(&acc_r, &acc_i, (H)[2 * MIMO_N4 + (CI)], \
                              (H)[2 * MIMO_N4 + (CJ)]); \
        mimo_acc_conj_q30_i64(&acc_r, &acc_i, (H)[3 * MIMO_N4 + (CI)], \
                              (H)[3 * MIMO_N4 + (CJ)]); \
        mimo_c32_t acc = {mimo_round_once_q30_i64_to_i32(acc_r), \
                          mimo_round_once_q30_i64_to_i32(acc_i)}; \
        acc; \
    })

#define MIMO_DOT4_HH_DIAG(H, C) \
    ({ \
        int64_t acc_r = 0; \
        mimo_acc_abs2_q30_i64(&acc_r, (H)[0 * MIMO_N4 + (C)]); \
        mimo_acc_abs2_q30_i64(&acc_r, (H)[1 * MIMO_N4 + (C)]); \
        mimo_acc_abs2_q30_i64(&acc_r, (H)[2 * MIMO_N4 + (C)]); \
        mimo_acc_abs2_q30_i64(&acc_r, (H)[3 * MIMO_N4 + (C)]); \
        mimo_c32_t acc = {mimo_round_once_q30_i64_to_i32(acc_r), 0}; \
        acc; \
    })
#else
#define MIMO_DOT4_HY(H, Y, C) \
    ({ \
        mimo_c32_t acc = {0, 0}; \
        mimo_acc_conj_q15_c32(&acc, (H)[0 * MIMO_N4 + (C)], (Y)[0]); \
        mimo_acc_conj_q15_c32(&acc, (H)[1 * MIMO_N4 + (C)], (Y)[1]); \
        mimo_acc_conj_q15_c32(&acc, (H)[2 * MIMO_N4 + (C)], (Y)[2]); \
        mimo_acc_conj_q15_c32(&acc, (H)[3 * MIMO_N4 + (C)], (Y)[3]); \
        acc; \
    })

#define MIMO_DOT4_HH(H, CI, CJ) \
    ({ \
        mimo_c32_t acc = {0, 0}; \
        mimo_acc_conj_q15_c32(&acc, (H)[0 * MIMO_N4 + (CI)], \
                                    (H)[0 * MIMO_N4 + (CJ)]); \
        mimo_acc_conj_q15_c32(&acc, (H)[1 * MIMO_N4 + (CI)], \
                                    (H)[1 * MIMO_N4 + (CJ)]); \
        mimo_acc_conj_q15_c32(&acc, (H)[2 * MIMO_N4 + (CI)], \
                                    (H)[2 * MIMO_N4 + (CJ)]); \
        mimo_acc_conj_q15_c32(&acc, (H)[3 * MIMO_N4 + (CI)], \
                                    (H)[3 * MIMO_N4 + (CJ)]); \
        acc; \
    })

#define MIMO_DOT4_HH_DIAG(H, C) MIMO_DOT4_HH(H, C, C)
#endif

#define MIMO_STORE_HERM4(A, CI, CJ, G) \
    do { \
        (A)[(CI) * MIMO_N4 + (CJ)] = (G); \
        if ((CI) != (CJ)) { \
            (A)[(CJ) * MIMO_N4 + (CI)].r = (G).r; \
            (A)[(CJ) * MIMO_N4 + (CI)].i = -(G).i; \
        } \
    } while (0)

#define MIMO_STORE_UPPER4(A, CI, CJ, G) \
    do { \
        (A)[(CI) * MIMO_N4 + (CJ)] = (G); \
    } while (0)

static void mimo_build_normal_row4_q15(const mimo_c16_t *h_row,
                                       const mimo_c16_t *y,
                                       mimo_c32_t *a,
                                       mimo_c32_t *b)
{
    b[0] = MIMO_DOT4_HY(h_row, y, 0);
    b[1] = MIMO_DOT4_HY(h_row, y, 1);
    b[2] = MIMO_DOT4_HY(h_row, y, 2);
    b[3] = MIMO_DOT4_HY(h_row, y, 3);

    mimo_c32_t g;

    g = MIMO_DOT4_HH_DIAG(h_row, 0);
    MIMO_STORE_HERM4(a, 0, 0, g);
    g = MIMO_DOT4_HH(h_row, 0, 1);
    MIMO_STORE_HERM4(a, 0, 1, g);
    g = MIMO_DOT4_HH(h_row, 0, 2);
    MIMO_STORE_HERM4(a, 0, 2, g);
    g = MIMO_DOT4_HH(h_row, 0, 3);
    MIMO_STORE_HERM4(a, 0, 3, g);
    g = MIMO_DOT4_HH_DIAG(h_row, 1);
    MIMO_STORE_HERM4(a, 1, 1, g);
    g = MIMO_DOT4_HH(h_row, 1, 2);
    MIMO_STORE_HERM4(a, 1, 2, g);
    g = MIMO_DOT4_HH(h_row, 1, 3);
    MIMO_STORE_HERM4(a, 1, 3, g);
    g = MIMO_DOT4_HH_DIAG(h_row, 2);
    MIMO_STORE_HERM4(a, 2, 2, g);
    g = MIMO_DOT4_HH(h_row, 2, 3);
    MIMO_STORE_HERM4(a, 2, 3, g);
    g = MIMO_DOT4_HH_DIAG(h_row, 3);
    MIMO_STORE_HERM4(a, 3, 3, g);
}

void mimo_build_normal_row4_upper_q15(const mimo_c16_t *h_row,
                                      const mimo_c16_t *y,
                                      mimo_c32_t *a,
                                      mimo_c32_t *b)
{
    b[0] = MIMO_DOT4_HY(h_row, y, 0);
    b[1] = MIMO_DOT4_HY(h_row, y, 1);
    b[2] = MIMO_DOT4_HY(h_row, y, 2);
    b[3] = MIMO_DOT4_HY(h_row, y, 3);

    mimo_c32_t g;

    g = MIMO_DOT4_HH_DIAG(h_row, 0);
    MIMO_STORE_UPPER4(a, 0, 0, g);
    g = MIMO_DOT4_HH(h_row, 0, 1);
    MIMO_STORE_UPPER4(a, 0, 1, g);
    g = MIMO_DOT4_HH(h_row, 0, 2);
    MIMO_STORE_UPPER4(a, 0, 2, g);
    g = MIMO_DOT4_HH(h_row, 0, 3);
    MIMO_STORE_UPPER4(a, 0, 3, g);
    g = MIMO_DOT4_HH_DIAG(h_row, 1);
    MIMO_STORE_UPPER4(a, 1, 1, g);
    g = MIMO_DOT4_HH(h_row, 1, 2);
    MIMO_STORE_UPPER4(a, 1, 2, g);
    g = MIMO_DOT4_HH(h_row, 1, 3);
    MIMO_STORE_UPPER4(a, 1, 3, g);
    g = MIMO_DOT4_HH_DIAG(h_row, 2);
    MIMO_STORE_UPPER4(a, 2, 2, g);
    g = MIMO_DOT4_HH(h_row, 2, 3);
    MIMO_STORE_UPPER4(a, 2, 3, g);
    g = MIMO_DOT4_HH_DIAG(h_row, 3);
    MIMO_STORE_UPPER4(a, 3, 3, g);
}

void mimo_build_normal_row4_q15acc_q15(const mimo_c16_t *h_row,
                                       const mimo_c16_t *y,
                                       mimo_c32_t *a,
                                       mimo_c32_t *b)
{
    b[0] = MIMO_DOT4_HY_Q15ACC(h_row, y, 0);
    b[1] = MIMO_DOT4_HY_Q15ACC(h_row, y, 1);
    b[2] = MIMO_DOT4_HY_Q15ACC(h_row, y, 2);
    b[3] = MIMO_DOT4_HY_Q15ACC(h_row, y, 3);

    mimo_c32_t g;

    g = MIMO_DOT4_HH_Q15ACC(h_row, 0, 0);
    MIMO_STORE_HERM4(a, 0, 0, g);
    g = MIMO_DOT4_HH_Q15ACC(h_row, 0, 1);
    MIMO_STORE_HERM4(a, 0, 1, g);
    g = MIMO_DOT4_HH_Q15ACC(h_row, 0, 2);
    MIMO_STORE_HERM4(a, 0, 2, g);
    g = MIMO_DOT4_HH_Q15ACC(h_row, 0, 3);
    MIMO_STORE_HERM4(a, 0, 3, g);
    g = MIMO_DOT4_HH_Q15ACC(h_row, 1, 1);
    MIMO_STORE_HERM4(a, 1, 1, g);
    g = MIMO_DOT4_HH_Q15ACC(h_row, 1, 2);
    MIMO_STORE_HERM4(a, 1, 2, g);
    g = MIMO_DOT4_HH_Q15ACC(h_row, 1, 3);
    MIMO_STORE_HERM4(a, 1, 3, g);
    g = MIMO_DOT4_HH_Q15ACC(h_row, 2, 2);
    MIMO_STORE_HERM4(a, 2, 2, g);
    g = MIMO_DOT4_HH_Q15ACC(h_row, 2, 3);
    MIMO_STORE_HERM4(a, 2, 3, g);
    g = MIMO_DOT4_HH_Q15ACC(h_row, 3, 3);
    MIMO_STORE_HERM4(a, 3, 3, g);
}

void mimo_build_normal_row_q15(int n,
                               const mimo_c16_t *h_row,
                               const mimo_c16_t *y,
                               mimo_c32_t *a,
                               mimo_c32_t *b)
{
    if (n == MIMO_N4) {
        mimo_build_normal_row4_q15(h_row, y, a, b);
        return;
    }

    for (int col_i = 0; col_i < n; ++col_i) {
        mimo_c32_t rhs = {0, 0};

        for (int rx = 0; rx < n; ++rx) {
            const mimo_c32_t prod =
                mimo_cmul_conj_a_fast_q15_c32(h_row[rx * n + col_i], y[rx]);
            rhs.r += prod.r;
            rhs.i += prod.i;
        }
        b[col_i] = rhs;

        for (int col_j = col_i; col_j < n; ++col_j) {
            mimo_c32_t gram = {0, 0};

            for (int rx = 0; rx < n; ++rx) {
                const mimo_c32_t prod =
                    mimo_cmul_conj_a_fast_q15_c32(h_row[rx * n + col_i],
                                                  h_row[rx * n + col_j]);
                gram.r += prod.r;
                gram.i += prod.i;
            }

            a[col_i * n + col_j] = gram;
            if (col_i == col_j) {
                a[col_j * n + col_i] = gram;
            } else {
                a[col_j * n + col_i].r = gram.r;
                a[col_j * n + col_i].i = -gram.i;
            }
        }
    }
}

void mimo_build_normal_col_q15(int n,
                               const mimo_c16_t *h_col,
                               const mimo_c16_t *y,
                               mimo_c32_t *a,
                               mimo_c32_t *b)
{
    for (int col_i = 0; col_i < n; ++col_i) {
        const mimo_c16_t *hi = &h_col[col_i * n];
        mimo_c32_t rhs = {0, 0};

        for (int rx = 0; rx < n; ++rx) {
            const mimo_c32_t prod = mimo_cmul_conj_a_fast_q15_c32(hi[rx], y[rx]);
            rhs.r += prod.r;
            rhs.i += prod.i;
        }
        b[col_i] = rhs;

        for (int col_j = col_i; col_j < n; ++col_j) {
            const mimo_c16_t *hj = &h_col[col_j * n];
            mimo_c32_t gram = {0, 0};

            for (int rx = 0; rx < n; ++rx) {
                const mimo_c32_t prod =
                    mimo_cmul_conj_a_fast_q15_c32(hi[rx], hj[rx]);
                gram.r += prod.r;
                gram.i += prod.i;
            }

            a[col_i * n + col_j] = gram;
            if (col_i == col_j) {
                a[col_j * n + col_i] = gram;
            } else {
                a[col_j * n + col_i].r = gram.r;
                a[col_j * n + col_i].i = -gram.i;
            }
        }
    }
}

static inline int64_t q15_round_shift_i64(int64_t x)
{
#if MIMO_USE_SOLVE_Q15_I64_ARSHIFT_ROUND
    return (x + MIMO_Q15_ROUND) >> MIMO_Q15_SHIFT;
#elif MIMO_USE_SOLVE_BRANCHLIGHT_ROUND
    const int64_t sign = x >> 63;
    const int64_t mag = (x ^ sign) - sign;
    const int64_t y = (mag + MIMO_Q15_ROUND) >> MIMO_Q15_SHIFT;

    return (y ^ sign) - sign;
#else
    if (x >= 0) {
        return (x + MIMO_Q15_ROUND) >> MIMO_Q15_SHIFT;
    }
    return -((-x + MIMO_Q15_ROUND) >> MIMO_Q15_SHIFT);
#endif
}

static inline mimo_c64_t c64_make(int64_t r, int64_t i)
{
    mimo_c64_t x = {r, i};
    return x;
}

static inline mimo_c64_t c64_sub(mimo_c64_t a, mimo_c64_t b)
{
    return c64_make(a.r - b.r, a.i - b.i);
}

static inline mimo_c64_t c64_mul_q15(mimo_c64_t a, mimo_c64_t b)
{
    return c64_make(q15_round_shift_i64(a.r * b.r - a.i * b.i),
                    q15_round_shift_i64(a.r * b.i + a.i * b.r));
}

static inline mimo_c64_t c64_conj(mimo_c64_t a)
{
    return c64_make(a.r, -a.i);
}

static inline mimo_c64_t c64_mul_real_q15(mimo_c64_t a, int64_t s)
{
    return c64_make(q15_round_shift_i64(a.r * s),
                    q15_round_shift_i64(a.i * s));
}

static inline uint64_t c64_mag2_u64(mimo_c64_t a)
{
    const uint64_t ar = (uint64_t)(a.r < 0 ? -a.r : a.r);
    const uint64_t ai = (uint64_t)(a.i < 0 ? -a.i : a.i);

    return ar * ar + ai * ai;
}

static inline mimo_c64_t c64_div_complex_to_q15(mimo_c64_t num,
                                                mimo_c64_t den)
{
    uint64_t mag2 = c64_mag2_u64(den);

    if (mag2 == 0) {
        mag2 = 1;
    }

    const int64_t nr = num.r * den.r + num.i * den.i;
    const int64_t ni = num.i * den.r - num.r * den.i;
    return c64_make((nr << MIMO_Q15_SHIFT) / (int64_t)mag2,
                    (ni << MIMO_Q15_SHIFT) / (int64_t)mag2);
}

static inline int16_t sat_i64_to_i16(int64_t x)
{
    if (x > 32767) {
        return 32767;
    }
    if (x < -32768) {
        return -32768;
    }
    return (int16_t)x;
}

static inline mimo_c16_t c64_to_c16_sat(mimo_c64_t a)
{
    return mimo_make_c16(sat_i64_to_i16(a.r), sat_i64_to_i16(a.i));
}

#if MIMO_USE_EQ4_DIRECT_H
#if MIMO_EQ4_DIRECT_H_MODE == 1
static void solve_eq4_direct_h_ge_one(const mimo_c16_t *h_row,
                                      const mimo_c16_t *y,
                                      mimo_c16_t *x)
{
    mimo_c64_t a[MIMO_N4][MIMO_N4];
    mimo_c64_t b[MIMO_N4];

    for (int row = 0; row < MIMO_N4; ++row) {
        for (int col = 0; col < MIMO_N4; ++col) {
            const mimo_c16_t h = h_row[row * MIMO_N4 + col];
            a[row][col] = c64_make(h.r, h.i);
        }
        b[row] = c64_make(y[row].r, y[row].i);
    }

    for (int k = 0; k < MIMO_N4 - 1; ++k) {
        for (int row = k + 1; row < MIMO_N4; ++row) {
            const mimo_c64_t factor =
                c64_div_complex_to_q15(a[row][k], a[k][k]);

            a[row][k] = c64_make(0, 0);
            for (int col = k + 1; col < MIMO_N4; ++col) {
                a[row][col] =
                    c64_sub(a[row][col], c64_mul_q15(factor, a[k][col]));
            }
            b[row] = c64_sub(b[row], c64_mul_q15(factor, b[k]));
        }
    }

    mimo_c64_t sol[MIMO_N4];
    for (int row = MIMO_N4 - 1; row >= 0; --row) {
        mimo_c64_t sum = b[row];

        for (int col = row + 1; col < MIMO_N4; ++col) {
            sum = c64_sub(sum, c64_mul_q15(a[row][col], sol[col]));
        }
        sol[row] = c64_div_complex_to_q15(sum, a[row][row]);
    }

    for (int i = 0; i < MIMO_N4; ++i) {
        x[i] = c64_to_c16_sat(sol[i]);
    }
}
#endif

#if MIMO_EQ4_DIRECT_H_MODE == 2 || MIMO_EQ4_DIRECT_H_MODE == 3
static void solve_eq4_direct_h_iter_one(const mimo_c16_t *h_row,
                                        const mimo_c16_t *y,
                                        int use_gs,
                                        mimo_c16_t *x)
{
    mimo_c64_t cur[MIMO_N4] = {
        c64_make(0, 0),
        c64_make(0, 0),
        c64_make(0, 0),
        c64_make(0, 0),
    };
    mimo_c64_t next[MIMO_N4] = {
        c64_make(0, 0),
        c64_make(0, 0),
        c64_make(0, 0),
        c64_make(0, 0),
    };

    for (int iter = 0; iter < MIMO_EQ4_DIRECT_H_ITERS; ++iter) {
        for (int row = 0; row < MIMO_N4; ++row) {
            mimo_c64_t sum = c64_make(y[row].r, y[row].i);

            for (int col = 0; col < MIMO_N4; ++col) {
                if (col == row) {
                    continue;
                }
                const mimo_c16_t hij = h_row[row * MIMO_N4 + col];
                const mimo_c64_t h = c64_make(hij.r, hij.i);
                const mimo_c64_t xj = use_gs ? cur[col] : next[col];

                sum = c64_sub(sum, c64_mul_q15(h, xj));
            }

            const mimo_c16_t hii = h_row[row * MIMO_N4 + row];
            cur[row] =
                c64_div_complex_to_q15(sum, c64_make(hii.r, hii.i));
        }

        if (!use_gs) {
            for (int i = 0; i < MIMO_N4; ++i) {
                next[i] = cur[i];
            }
        }
    }

    for (int i = 0; i < MIMO_N4; ++i) {
        x[i] = c64_to_c16_sat(cur[i]);
    }
}
#endif

static void solve_eq4_direct_h_one(const mimo_c16_t *h_row,
                                   const mimo_c16_t *y,
                                   mimo_c16_t *x)
{
#if MIMO_EQ4_DIRECT_H_MODE == 1
    solve_eq4_direct_h_ge_one(h_row, y, x);
#elif MIMO_EQ4_DIRECT_H_MODE == 2
    solve_eq4_direct_h_iter_one(h_row, y, 0, x);
#elif MIMO_EQ4_DIRECT_H_MODE == 3
    solve_eq4_direct_h_iter_one(h_row, y, 1, x);
#else
    (void)h_row;
    (void)y;
    for (int i = 0; i < MIMO_N4; ++i) {
        x[i] = mimo_make_c16(0, 0);
    }
#endif
}

void mimo_eq4_direct_h_row_q15(size_t site_count,
                               const mimo_c16_t *h_row,
                               const mimo_c16_t *y,
                               mimo_c16_t *x)
{
    for (size_t site = 0; site < site_count; ++site) {
        solve_eq4_direct_h_one(&h_row[site * MIMO_N4 * MIMO_N4],
                               &y[site * MIMO_N4],
                               &x[site * MIMO_N4]);
    }
}

void mimo_eq4_direct_h_site4_q15(const mimo_c16_t *h_site4,
                                 const mimo_c16_t *y_site4,
                                 mimo_c16_t *x)
{
    const size_t blocks = MIMO_EQ4_SITE_COUNT / MIMO_SITE4_LANES;
    mimo_c16_t h_row[MIMO_N4 * MIMO_N4];
    mimo_c16_t y_row[MIMO_N4];

    for (size_t block = 0; block < blocks; ++block) {
        for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
            for (int rx = 0; rx < MIMO_N4; ++rx) {
                y_row[rx] =
                    y_site4[(block * MIMO_N4 + (size_t)rx) *
                                MIMO_SITE4_LANES +
                            (size_t)lane];
                for (int tx = 0; tx < MIMO_N4; ++tx) {
                    h_row[rx * MIMO_N4 + tx] =
                        h_site4[(((block * MIMO_N4 + (size_t)tx) *
                                      MIMO_N4 +
                                  (size_t)rx) *
                                     MIMO_SITE4_LANES) +
                                (size_t)lane];
                }
            }
            solve_eq4_direct_h_one(
                h_row, y_row,
                &x[(block * MIMO_SITE4_LANES + (size_t)lane) * MIMO_N4]);
        }
    }
}
#else
void mimo_eq4_direct_h_row_q15(size_t site_count,
                               const mimo_c16_t *h_row,
                               const mimo_c16_t *y,
                               mimo_c16_t *x)
{
    (void)site_count;
    (void)h_row;
    (void)y;
    (void)x;
}

void mimo_eq4_direct_h_site4_q15(const mimo_c16_t *h_site4,
                                 const mimo_c16_t *y_site4,
                                 mimo_c16_t *x)
{
    (void)h_site4;
    (void)y_site4;
    (void)x;
}
#endif

static inline mimo_c64_t c64_div_real_to_q15(mimo_c64_t a, int64_t d)
{
    if (d == 0) {
        d = 1;
    }
    return c64_make((a.r << MIMO_Q15_SHIFT) / d,
                    (a.i << MIMO_Q15_SHIFT) / d);
}

static inline mimo_c64_t c64_mul_q30_q15(mimo_c64_t a_q30, mimo_c64_t b_q15)
{
    const int64_t re = a_q30.r * b_q15.r - a_q30.i * b_q15.i;
    const int64_t im = a_q30.r * b_q15.i + a_q30.i * b_q15.r;

    if (re >= 0 && im >= 0) {
        return c64_make((re + (1LL << 29)) >> 30,
                        (im + (1LL << 29)) >> 30);
    }
    return c64_make(re >= 0 ? (re + (1LL << 29)) >> 30
                            : -((-re + (1LL << 29)) >> 30),
                    im >= 0 ? (im + (1LL << 29)) >> 30
                            : -((-im + (1LL << 29)) >> 30));
}

static inline int64_t safe_pivot_i64(int64_t x)
{
    if (x == 0) {
        return 1;
    }
    return x;
}

#if MIMO_COLLECT_SCHUR_STATS
static inline mimo_c64_t schur_c64_from_c32(mimo_c32_t x)
{
    return c64_make(x.r, x.i);
}

static inline mimo_c64_t schur_c64_add(mimo_c64_t a, mimo_c64_t b)
{
    return c64_make(a.r + b.r, a.i + b.i);
}

static inline mimo_c64_t schur_c64_neg(mimo_c64_t a)
{
    return c64_make(-a.r, -a.i);
}

static inline mimo_c64_t schur_mul_q15_to_q30(mimo_c64_t a, mimo_c64_t b)
{
    return c64_make(a.r * b.r - a.i * b.i,
                    a.r * b.i + a.i * b.r);
}

static inline mimo_c64_t schur_mul_real_q15_to_q30(mimo_c64_t a, int64_t s)
{
    return c64_make(a.r * s, a.i * s);
}

static inline mimo_c64_t schur_div_q30_to_q15(mimo_c64_t num_q30,
                                               int64_t den_q30)
{
    den_q30 = safe_pivot_i64(den_q30);
    return c64_make((num_q30.r << MIMO_Q15_SHIFT) / den_q30,
                    (num_q30.i << MIMO_Q15_SHIFT) / den_q30);
}

static mimo_c64_t schur_solve_b2_vec_q15(mimo_c64_t a00,
                                         mimo_c64_t a01,
                                         mimo_c64_t a11,
                                         int64_t det_q30,
                                         mimo_c64_t v0,
                                         mimo_c64_t v1,
                                         int row)
{
    mimo_c64_t num;

    if (row == 0) {
        num = c64_sub(schur_mul_real_q15_to_q30(v0, a11.r),
                      schur_mul_q15_to_q30(a01, v1));
    } else {
        num = schur_c64_add(
            schur_mul_q15_to_q30(schur_c64_neg(c64_conj(a01)), v0),
            schur_mul_real_q15_to_q30(v1, a00.r));
    }

    RECORD_SCHUR_C64(schur_solve_num_q30_stats, num);
    const mimo_c64_t out = schur_div_q30_to_q15(num, det_q30);
    RECORD_SCHUR_C64(schur_b_inv_vec_q15_stats, out);
    return out;
}

static inline mimo_c64_t schur_inner_eh_w(mimo_c64_t e0,
                                          mimo_c64_t e1,
                                          mimo_c64_t w0,
                                          mimo_c64_t w1)
{
    return schur_c64_add(c64_mul_q15(c64_conj(e0), w0),
                         c64_mul_q15(c64_conj(e1), w1));
}

static void mimo_collect_schur4_q15(const mimo_c32_t *a,
                                    const mimo_c32_t *b,
                                    int mmse)
{
    mimo_c64_t a00 = schur_c64_from_c32(a[0]);
    mimo_c64_t a01 = schur_c64_from_c32(a[1]);
    mimo_c64_t a02 = schur_c64_from_c32(a[2]);
    mimo_c64_t a03 = schur_c64_from_c32(a[3]);
    mimo_c64_t a11 = schur_c64_from_c32(a[5]);
    mimo_c64_t a12 = schur_c64_from_c32(a[6]);
    mimo_c64_t a13 = schur_c64_from_c32(a[7]);
    mimo_c64_t a22 = schur_c64_from_c32(a[10]);
    mimo_c64_t a23 = schur_c64_from_c32(a[11]);
    mimo_c64_t a32 = schur_c64_from_c32(a[14]);
    mimo_c64_t a33 = schur_c64_from_c32(a[15]);
    const mimo_c64_t rhs0 = schur_c64_from_c32(b[0]);
    const mimo_c64_t rhs1 = schur_c64_from_c32(b[1]);
    const mimo_c64_t rhs2 = schur_c64_from_c32(b[2]);
    const mimo_c64_t rhs3 = schur_c64_from_c32(b[3]);

    if (mmse) {
        a00.r += MIMO_MMSE_LAMBDA_Q15;
        a11.r += MIMO_MMSE_LAMBDA_Q15;
        a22.r += MIMO_MMSE_LAMBDA_Q15;
        a33.r += MIMO_MMSE_LAMBDA_Q15;
    }
    a00.i = 0;
    a11.i = 0;
    a22.i = 0;
    a33.i = 0;

    RECORD_SCHUR_C64(schur_input_upper_q15_stats, a00);
    RECORD_SCHUR_C64(schur_input_upper_q15_stats, a01);
    RECORD_SCHUR_C64(schur_input_upper_q15_stats, a02);
    RECORD_SCHUR_C64(schur_input_upper_q15_stats, a03);
    RECORD_SCHUR_C64(schur_input_upper_q15_stats, a11);
    RECORD_SCHUR_C64(schur_input_upper_q15_stats, a12);
    RECORD_SCHUR_C64(schur_input_upper_q15_stats, a13);
    RECORD_SCHUR_C64(schur_input_upper_q15_stats, a22);
    RECORD_SCHUR_C64(schur_input_upper_q15_stats, a23);
    RECORD_SCHUR_C64(schur_input_upper_q15_stats, a33);
    RECORD_SCHUR_C64(schur_input_rhs_q15_stats, rhs0);
    RECORD_SCHUR_C64(schur_input_rhs_q15_stats, rhs1);
    RECORD_SCHUR_C64(schur_input_rhs_q15_stats, rhs2);
    RECORD_SCHUR_C64(schur_input_rhs_q15_stats, rhs3);

    RECORD_SCHUR_C64(schur_a_diag_stats, a00);
    RECORD_SCHUR_C64(schur_a_diag_stats, a11);
    RECORD_SCHUR_C64(schur_a_diag_stats, a22);
    RECORD_SCHUR_C64(schur_a_diag_stats, a33);

    const int64_t det_q30 =
        a00.r * a11.r - (a01.r * a01.r + a01.i * a01.i);
    RECORD_SCHUR_NUM(schur_b_det_q30_stats, det_q30);

    const mimo_c64_t e0r0 = a02;
    const mimo_c64_t e0r1 = a12;
    const mimo_c64_t e1r0 = a03;
    const mimo_c64_t e1r1 = a13;

    const mimo_c64_t w00 =
        schur_solve_b2_vec_q15(a00, a01, a11, det_q30, e0r0, e0r1, 0);
    const mimo_c64_t w10 =
        schur_solve_b2_vec_q15(a00, a01, a11, det_q30, e0r0, e0r1, 1);
    const mimo_c64_t w01 =
        schur_solve_b2_vec_q15(a00, a01, a11, det_q30, e1r0, e1r1, 0);
    const mimo_c64_t w11 =
        schur_solve_b2_vec_q15(a00, a01, a11, det_q30, e1r0, e1r1, 1);

    mimo_c64_t update = schur_inner_eh_w(e0r0, e0r1, w00, w10);
    RECORD_SCHUR_C64(schur_update_q15_stats, update);
    const mimo_c64_t s00 = c64_sub(a22, update);
    RECORD_SCHUR_C64(schur_block_q15_stats, s00);

    update = schur_inner_eh_w(e0r0, e0r1, w01, w11);
    RECORD_SCHUR_C64(schur_update_q15_stats, update);
    const mimo_c64_t s01 = c64_sub(a23, update);
    RECORD_SCHUR_C64(schur_block_q15_stats, s01);

    update = schur_inner_eh_w(e1r0, e1r1, w00, w10);
    RECORD_SCHUR_C64(schur_update_q15_stats, update);
    RECORD_SCHUR_C64(schur_block_q15_stats, c64_sub(a32, update));

    update = schur_inner_eh_w(e1r0, e1r1, w01, w11);
    RECORD_SCHUR_C64(schur_update_q15_stats, update);
    const mimo_c64_t s11 = c64_sub(a33, update);
    RECORD_SCHUR_C64(schur_block_q15_stats, s11);
    RECORD_SCHUR_NUM(schur_s_det_q30_stats,
                     s00.r * s11.r - (s01.r * s01.r + s01.i * s01.i));

    const mimo_c64_t y0 =
        schur_solve_b2_vec_q15(a00, a01, a11, det_q30, rhs0, rhs1, 0);
    const mimo_c64_t y1 =
        schur_solve_b2_vec_q15(a00, a01, a11, det_q30, rhs0, rhs1, 1);

    update = schur_inner_eh_w(e0r0, e0r1, y0, y1);
    RECORD_SCHUR_C64(schur_update_q15_stats, update);
    RECORD_SCHUR_C64(schur_rhs_q15_stats, c64_sub(rhs2, update));

    update = schur_inner_eh_w(e1r0, e1r1, y0, y1);
    RECORD_SCHUR_C64(schur_update_q15_stats, update);
    RECORD_SCHUR_C64(schur_rhs_q15_stats, c64_sub(rhs3, update));
}
#else
#define mimo_collect_schur4_q15(a, b, mmse) ((void)0)
#endif

#if MIMO_USE_RECIP_TABLE
static inline uint64_t abs_i64_to_u64(int64_t x)
{
    return x < 0 ? (uint64_t)(-(x + 1)) + 1 : (uint64_t)x;
}
#endif

#if MIMO_USE_RECIP_TABLE == 1
static inline int64_t fast_div_to_q30_i64(int64_t num, int64_t den)
{
    den = safe_pivot_i64(den);

    const int neg = (num < 0) ^ (den < 0);
    const uint64_t mag_num = abs_i64_to_u64(num);
    const uint64_t mag_den = abs_i64_to_u64(den);
    uint64_t q;

    if (mag_den <= MIMO_RECIP_TABLE_MAX && recip_q45_table[mag_den] != 0) {
        q = (mag_num * recip_q45_table[mag_den]) >>
            (MIMO_RECIP_TABLE_SHIFT - 30);
        if (mag_num <= (UINT64_MAX >> 30)) {
            const uint64_t target = mag_num << 30;

            while (q > 0 && q * mag_den > target) {
                --q;
            }
            while ((q + 1) * mag_den <= target) {
                ++q;
            }
        }
    } else {
        q = (mag_num << 30) / mag_den;
    }

    return neg ? -(int64_t)q : (int64_t)q;
}

static inline mimo_c64_t c64_div_real_to_q15_fast(mimo_c64_t a, int64_t d)
{
    const int64_t qr = fast_div_to_q30_i64(a.r, d);
    const int64_t qi = fast_div_to_q30_i64(a.i, d);
    const int64_t sr = qr < 0 ? -((-qr) >> 15) : (qr >> 15);
    const int64_t si = qi < 0 ? -((-qi) >> 15) : (qi >> 15);

    return c64_make(sr, si);
}
#define MIMO_DIV_TO_Q30(num, den) fast_div_to_q30_i64((num), record_factor_den(den))
#define MIMO_DIV_REAL_TO_Q15(sum, den) c64_div_real_to_q15_fast((sum), record_back_den(den))
#elif MIMO_USE_RECIP_TABLE == 2
static inline int64_t fast_div_to_q30_i64_v2(int64_t num, int64_t den)
{
    den = safe_pivot_i64(den);

    const int neg = (num < 0) ^ (den < 0);
    const uint64_t mag_num = abs_i64_to_u64(num);
    const uint64_t mag_den = abs_i64_to_u64(den);

    /*
     * This v2 path is only for the fixed benchmark input family after
     * MIMO_COLLECT_SOLVE_DEN_STATS proves all hot solve denominators are in
     * 1..32767. It intentionally has no div fallback and no correction loop.
     */
    const unsigned __int128 prod =
        (unsigned __int128)mag_num * recip_q45_table[mag_den];
    uint64_t q = (uint64_t)(prod >> (MIMO_RECIP_TABLE_SHIFT - 30));
    const uint64_t target = mag_num << 30;
    uint64_t q_scaled = q * mag_den;

    const uint64_t dec = q_scaled > target;
    q -= dec;
    q_scaled -= dec * mag_den;

    const uint64_t inc = q_scaled + mag_den <= target;
    q += inc;

    return neg ? -(int64_t)q : (int64_t)q;
}

static inline mimo_c64_t c64_div_real_to_q15_fast_v2(mimo_c64_t a, int64_t d)
{
    const int64_t qr = fast_div_to_q30_i64_v2(a.r, d);
    const int64_t qi = fast_div_to_q30_i64_v2(a.i, d);
    const int64_t sr = qr < 0 ? -((-qr) >> 15) : (qr >> 15);
    const int64_t si = qi < 0 ? -((-qi) >> 15) : (qi >> 15);

    return c64_make(sr, si);
}
#define MIMO_DIV_TO_Q30(num, den) fast_div_to_q30_i64_v2((num), record_factor_den(den))
#define MIMO_DIV_REAL_TO_Q15(sum, den) c64_div_real_to_q15_fast_v2((sum), record_back_den(den))
#else
#define MIMO_DIV_TO_Q30(num, den) (((num) << 30) / record_factor_den(den))
#define MIMO_DIV_REAL_TO_Q15(sum, den) c64_div_real_to_q15((sum), record_back_den(den))
#endif

static inline int32_t q15_round_shift_i32(int64_t x)
{
#if MIMO_USE_SOLVE_BRANCHLIGHT_ROUND
    const int64_t sign = x >> 63;
    const int64_t mag = (x ^ sign) - sign;
    const int64_t y = (mag + MIMO_Q15_ROUND) >> MIMO_Q15_SHIFT;

    return (int32_t)((y ^ sign) - sign);
#else
    const int64_t y = x >= 0 ? (x + MIMO_Q15_ROUND) >> MIMO_Q15_SHIFT
                             : -((-x + MIMO_Q15_ROUND) >> MIMO_Q15_SHIFT);

    if (y > INT32_MAX) {
        return INT32_MAX;
    }
    if (y < INT32_MIN) {
        return INT32_MIN;
    }
    return (int32_t)y;
#endif
}

static inline mimo_c32_t c32_make(int32_t r, int32_t i)
{
    mimo_c32_t x = {r, i};
    return x;
}

static inline mimo_c32_t c32_sub(mimo_c32_t a, mimo_c32_t b)
{
    return c32_make(a.r - b.r, a.i - b.i);
}

static inline mimo_c32_t c32_add(mimo_c32_t a, mimo_c32_t b)
{
    return c32_make(a.r + b.r, a.i + b.i);
}

static inline mimo_c32_t c32_conj(mimo_c32_t a)
{
    return c32_make(a.r, -a.i);
}

static inline mimo_c32_t c32_mul_q15(mimo_c32_t a, mimo_c32_t b)
{
    return c32_make(q15_round_shift_i32((int64_t)a.r * b.r -
                                        (int64_t)a.i * b.i),
                    q15_round_shift_i32((int64_t)a.r * b.i +
                                        (int64_t)a.i * b.r));
}

static inline int32_t c32_mul_q15_real(mimo_c32_t a, mimo_c32_t b)
{
    return q15_round_shift_i32((int64_t)a.r * b.r -
                               (int64_t)a.i * b.i);
}

static inline int32_t q30_round_shift_i32(int64_t x)
{
#if MIMO_USE_SOLVE_BRANCHLIGHT_ROUND
    const int64_t sign = x >> 63;
    const int64_t mag = (x ^ sign) - sign;
    const int64_t y = (mag + (1LL << 29)) >> 30;

    return (int32_t)((y ^ sign) - sign);
#else
    const int64_t y = x >= 0 ? (x + (1LL << 29)) >> 30
                             : -((-x + (1LL << 29)) >> 30);

    if (y > INT32_MAX) {
        return INT32_MAX;
    }
    if (y < INT32_MIN) {
        return INT32_MIN;
    }
    return (int32_t)y;
#endif
}

static inline mimo_c32_t c32_mul_q30_q15(mimo_c32_t a_q30,
                                         mimo_c32_t b_q15)
{
    return c32_make(q30_round_shift_i32((int64_t)a_q30.r * b_q15.r -
                                        (int64_t)a_q30.i * b_q15.i),
                    q30_round_shift_i32((int64_t)a_q30.r * b_q15.i +
                                        (int64_t)a_q30.i * b_q15.r));
}

static inline int32_t c32_mul_q30_q15_real(mimo_c32_t a_q30,
                                           mimo_c32_t b_q15)
{
    return q30_round_shift_i32((int64_t)a_q30.r * b_q15.r -
                               (int64_t)a_q30.i * b_q15.i);
}

static inline mimo_c32_t c32_mul_q30_q30(mimo_c32_t a_q30,
                                         mimo_c32_t b_q30)
{
    return c32_make(q30_round_shift_i32((int64_t)a_q30.r * b_q30.r -
                                        (int64_t)a_q30.i * b_q30.i),
                    q30_round_shift_i32((int64_t)a_q30.r * b_q30.i +
                                        (int64_t)a_q30.i * b_q30.r));
}

static inline mimo_c32_t c32_mul_real_q30_i64(mimo_c32_t a, int64_t s_q30)
{
    return c32_make(q30_round_shift_i32((int64_t)a.r * s_q30),
                    q30_round_shift_i32((int64_t)a.i * s_q30));
}

static inline mimo_c32_t c32_mul_real_q30_to_q15(mimo_c32_t a_q30,
                                                 int32_t s_q15)
{
    return c32_make(q30_round_shift_i32((int64_t)a_q30.r * s_q15),
                    q30_round_shift_i32((int64_t)a_q30.i * s_q15));
}

static inline mimo_c32_t c32_mul_real_q15_to_q30(mimo_c32_t a_q15,
                                                 int64_t s_q30)
{
    return c32_make(q15_round_shift_i32((int64_t)a_q15.r * s_q30),
                    q15_round_shift_i32((int64_t)a_q15.i * s_q30));
}

static inline mimo_c32_t c32_mul_real_q15(mimo_c32_t a, int32_t s)
{
    return c32_make(q15_round_shift_i32((int64_t)a.r * s),
                    q15_round_shift_i32((int64_t)a.i * s));
}

static inline int32_t safe_pivot_i32(int32_t x)
{
    if (x == 0) {
        return 1;
    }
    return x;
}

static inline int32_t div_to_q15_i32(int32_t num, int32_t den)
{
    den = safe_pivot_i32(den);

    int sign = 1;
    uint32_t mag_num;
    uint32_t mag_den;

    if (num < 0) {
        sign = -sign;
        mag_num = (uint32_t)(-num);
    } else {
        mag_num = (uint32_t)num;
    }

    if (den < 0) {
        sign = -sign;
        mag_den = (uint32_t)(-den);
    } else {
        mag_den = (uint32_t)den;
    }

    if (mag_num > 65535u) {
        mag_num = 65535u;
    }
    if (mag_den == 0u) {
        mag_den = 1u;
    }

    const int32_t q = (int32_t)((mag_num << MIMO_Q15_SHIFT) / mag_den);
    return sign < 0 ? -q : q;
}

static inline mimo_c32_t c32_div_real_to_q15(mimo_c32_t a, int32_t d)
{
    return c32_make(div_to_q15_i32(a.r, d), div_to_q15_i32(a.i, d));
}

static inline int32_t div_to_q30_i32(int32_t num, int32_t den)
{
    return (int32_t)MIMO_DIV_TO_Q30((int64_t)num, (int64_t)den);
}

static inline int64_t div_to_q30_i64(int64_t num, int64_t den)
{
    return MIMO_DIV_TO_Q30(num, den);
}

static inline mimo_c32_t c32_div_real_to_q15_exact(mimo_c32_t a, int32_t d)
{
    const mimo_c64_t q =
        MIMO_DIV_REAL_TO_Q15(c64_make(a.r, a.i), (int64_t)d);

    return c32_make((int32_t)q.r, (int32_t)q.i);
}

static inline mimo_c32_t c32_div_real_to_q15_exact_nonzero(mimo_c32_t a,
                                                           int32_t d)
{
    return c32_make((int32_t)(((int64_t)a.r << MIMO_Q15_SHIFT) / (int64_t)d),
                    (int32_t)(((int64_t)a.i << MIMO_Q15_SHIFT) / (int64_t)d));
}

static inline int32_t div_to_q15_i32_exact(int32_t num, int32_t den)
{
    den = safe_pivot_i32(den);
    return (int32_t)(((int64_t)num << MIMO_Q15_SHIFT) / (int64_t)den);
}

static inline mimo_c32_t c32_factor_q15_exact(mimo_c32_t a, int32_t d)
{
    return c32_make(div_to_q15_i32_exact(a.r, d),
                    div_to_q15_i32_exact(-a.i, d));
}

#if MIMO_USE_SOLVE4_HERMITIAN
#if MIMO_USE_SOLVE4_HERMITIAN_TRUST_PIVOT_NONZERO
#define MIMO_HERM_SAFE_PIVOT_I32(X) (X)
#else
#define MIMO_HERM_SAFE_PIVOT_I32(X) safe_pivot_i32((X))
#endif

#if MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE
#if MIMO_USE_SOLVE4_HERMITIAN_RECIP_STEP16
#define MIMO_HERM_PIVOT_RECIP_STEP_SHIFT 4u
#define MIMO_HERM_PIVOT_RECIP_LAST 88u

static const uint32_t herm_pivot_recip_base_q30[] = {
    101680, 101526, 101373, 101220, 101068, 100916,
    100764, 100613, 100462, 100312, 100162, 100013,
    99864, 99716, 99568, 99421, 99273, 99127,
    98981, 98835, 98690, 98545, 98400, 98256,
    98112, 97969, 97826, 97684, 97542, 97400,
    97259, 97118, 96978, 96838, 96699, 96560,
    96421, 96282, 96145, 96007, 95870, 95733,
    95597, 95461, 95325, 95190, 95055, 94921,
    94787, 94653, 94520, 94387, 94254, 94122,
    93990, 93859, 93727, 93597, 93466, 93336,
    93207, 93077, 92949, 92820, 92692, 92564,
    92436, 92309, 92183, 92056, 91930, 91804,
    91679, 91554, 91429, 91305, 91181, 91057,
    90933, 90810, 90688, 90565, 90443, 90321,
    90200, 90079, 89958, 89838, 89718,
};

static const int16_t herm_pivot_recip_delta_q30[] = {
    -154, -153, -153, -152, -152, -152,
    -151, -151, -150, -150, -149, -149,
    -148, -148, -147, -148, -146, -146,
    -146, -145, -145, -145, -144, -144,
    -143, -143, -142, -142, -142, -141,
    -141, -140, -140, -139, -139, -139,
    -139, -137, -138, -137, -137, -136,
    -136, -136, -135, -135, -134, -134,
    -134, -133, -133, -133, -132, -132,
    -131, -132, -130, -131, -130, -129,
    -130, -128, -129, -128, -128, -128,
    -127, -126, -127, -126, -126, -125,
    -125, -125, -124, -124, -124, -124,
    -123, -122, -123, -122, -122, -121,
    -121, -121, -120, -120, -30,
};
#else
#define MIMO_HERM_PIVOT_RECIP_STEP_SHIFT 5u
#define MIMO_HERM_PIVOT_RECIP_LAST 44u

static const uint32_t herm_pivot_recip_base_q30[] = {
    101680, 101373, 101068, 100764, 100462, 100162,
    99864, 99568, 99273, 98981, 98690, 98400,
    98112, 97826, 97542, 97259, 96978, 96699,
    96421, 96145, 95870, 95597, 95325, 95055,
    94787, 94520, 94254, 93990, 93727, 93466,
    93207, 92949, 92692, 92436, 92183, 91930,
    91679, 91429, 91181, 90933, 90688, 90443,
    90200, 89958, 89718,
};

static const int16_t herm_pivot_recip_delta_q30[] = {
    -307, -305, -304, -302, -300, -298,
    -296, -295, -292, -291, -290, -288,
    -286, -284, -283, -281, -279, -278,
    -276, -275, -273, -272, -270, -268,
    -267, -266, -264, -263, -261, -259,
    -258, -257, -256, -253, -253, -251,
    -250, -248, -248, -245, -245, -243,
    -242, -240, -30,
};
#endif

static inline uint32_t herm_pivot_recip_scale_q30_interp_from_mag(uint32_t mag)
{
    const uint32_t rel = mag - MIMO_HERM_PIVOT_RECIP_MIN;
    const uint32_t seg = rel >> MIMO_HERM_PIVOT_RECIP_STEP_SHIFT;
    const uint32_t off = rel & ((1u << MIMO_HERM_PIVOT_RECIP_STEP_SHIFT) - 1u);
    const uint32_t base = herm_pivot_recip_base_q30[seg];
    const uint32_t drop = (uint32_t)(-herm_pivot_recip_delta_q30[seg]) * off;

    if (seg == MIMO_HERM_PIVOT_RECIP_LAST) {
        return base - ((drop + 2u) >> 2);
    }
    return base - ((drop + 16u) >> MIMO_HERM_PIVOT_RECIP_STEP_SHIFT);
}

static inline uint32_t herm_pivot_recip_scale_q30_from_mag(uint32_t mag)
{
#if MIMO_USE_SOLVE4_HERMITIAN_FULL_RECIP_TABLE
    return herm_pivot_full_recip_q30[mag - MIMO_HERM_PIVOT_RECIP_MIN];
#else
    return herm_pivot_recip_scale_q30_interp_from_mag(mag);
#endif
}

typedef struct {
#if !MIMO_USE_SOLVE4_HERMITIAN_POS_PIVOT_SCALE
    int32_t safe;
#endif
    uint32_t mag_den;
    uint32_t scale_q30;
} herm_pivot_scale_t;

static inline herm_pivot_scale_t herm_pivot_scale_init(int32_t den)
{
    herm_pivot_scale_t s;
#if MIMO_USE_SOLVE4_HERMITIAN_POS_PIVOT_SCALE && \
    MIMO_USE_SOLVE4_HERMITIAN_TRUST_PIVOT_RANGE
    s.mag_den = (uint32_t)den;
#elif MIMO_USE_SOLVE4_HERMITIAN_POS_PIVOT_SCALE
    s.mag_den = (uint32_t)safe_pivot_i32(den);
#else
    s.safe = safe_pivot_i32(den);
    s.mag_den = s.safe < 0 ? (uint32_t)(-s.safe) : (uint32_t)s.safe;
#endif

#if !MIMO_USE_SOLVE4_HERMITIAN_TRUST_PIVOT_RANGE && \
    !MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE_HARDDIV
    if (s.mag_den < MIMO_HERM_PIVOT_RECIP_MIN) {
        s.mag_den = MIMO_HERM_PIVOT_RECIP_MIN;
    } else if (s.mag_den > MIMO_HERM_PIVOT_RECIP_MAX) {
        s.mag_den = MIMO_HERM_PIVOT_RECIP_MAX;
    }
#endif

#if MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE_HARDDIV
    s.scale_q30 =
        (uint32_t)(((int64_t)1 << 30) / record_factor_den(s.mag_den));
#else
    s.scale_q30 = herm_pivot_recip_scale_q30_from_mag(s.mag_den);
#endif
    return s;
}

static inline uint32_t herm_pivot_corr_delta(
    uint64_t residual,
    const herm_pivot_scale_t *scale)
{
    const uint64_t coarse = residual >> MIMO_Q15_SHIFT;
    uint64_t delta = (coarse * (uint64_t)scale->scale_q30) >>
                     MIMO_Q15_SHIFT;

    if (delta == 0) {
        return 1;
    }
    if (delta > UINT32_MAX) {
        return UINT32_MAX;
    }
    return (uint32_t)delta;
}

static inline uint32_t herm_pivot_apply_jump_corr(
    uint32_t mag_q,
    uint64_t target,
    const herm_pivot_scale_t *scale)
{
    const uint64_t scaled = (uint64_t)mag_q * scale->mag_den;

    if (scaled > target) {
        const uint32_t delta = herm_pivot_corr_delta(scaled - target, scale);
        return delta >= mag_q ? 0 : mag_q - delta;
    }

    if (scaled < target) {
        const uint32_t delta = herm_pivot_corr_delta(target - scaled, scale);
        const uint32_t room = UINT32_MAX - mag_q;
        return mag_q + (delta > room ? room : delta);
    }

    return mag_q;
}

static inline int32_t herm_pivot_div_to_q15_scaled(
    int32_t num,
    const herm_pivot_scale_t *scale)
{
    const int32_t q =
        q15_round_shift_i32((int64_t)num * (int64_t)scale->scale_q30);
#if MIMO_USE_SOLVE4_HERMITIAN_POS_PIVOT_SCALE
    const int neg = num < 0;
#else
    const int neg = (num < 0) ^ (scale->safe < 0);
#endif
    uint32_t mag_num = num < 0 ? (uint32_t)(-num) : (uint32_t)num;
    uint32_t mag_q = q < 0 ? (uint32_t)(-q) : (uint32_t)q;
    const uint64_t target = (uint64_t)mag_num << MIMO_Q15_SHIFT;
    const uint32_t before_jump = mag_q;
    uint32_t dec_steps = 0;
    uint32_t inc_steps = 0;

    mag_q = herm_pivot_apply_jump_corr(mag_q, target, scale);
    const uint32_t after_jump = mag_q;
#if MIMO_USE_SOLVE4_HERMITIAN_Q15_ONECORR
    if (mag_q > 0 && (uint64_t)mag_q * scale->mag_den > target) {
        --mag_q;
        ++dec_steps;
    }
    if ((uint64_t)(mag_q + 1u) * scale->mag_den <= target) {
        ++mag_q;
        ++inc_steps;
    }
#else
    while (mag_q > 0 && (uint64_t)mag_q * scale->mag_den > target) {
        --mag_q;
        ++dec_steps;
    }
    while ((uint64_t)(mag_q + 1u) * scale->mag_den <= target) {
        ++mag_q;
        ++inc_steps;
    }
#endif

    RECORD_PIVOT_CORR_Q15(before_jump, after_jump, dec_steps, inc_steps);

    return neg ? -(int32_t)mag_q : (int32_t)mag_q;
}

#if MIMO_SOLVE4_HERMITIAN_Q15_JUMP_ONLY_MASK
static inline int32_t herm_pivot_div_to_q15_jump_only_scaled(
    int32_t num,
    const herm_pivot_scale_t *scale)
{
    const int32_t q =
        q15_round_shift_i32((int64_t)num * (int64_t)scale->scale_q30);
#if MIMO_USE_SOLVE4_HERMITIAN_POS_PIVOT_SCALE
    const int neg = num < 0;
#else
    const int neg = (num < 0) ^ (scale->safe < 0);
#endif
    uint32_t mag_num = num < 0 ? (uint32_t)(-num) : (uint32_t)num;
    uint32_t mag_q = q < 0 ? (uint32_t)(-q) : (uint32_t)q;
    const uint64_t target = (uint64_t)mag_num << MIMO_Q15_SHIFT;
    const uint32_t before_jump = mag_q;

    mag_q = herm_pivot_apply_jump_corr(mag_q, target, scale);
    RECORD_PIVOT_CORR_Q15(before_jump, mag_q, 0, 0);

    return neg ? -(int32_t)mag_q : (int32_t)mag_q;
}

#define MIMO_Q15_STAGE_JUMP_ONLY(ID) \
    (((MIMO_SOLVE4_HERMITIAN_Q15_JUMP_ONLY_MASK) >> (ID)) & 1)
#define MIMO_HERM_PIVOT_DIV_Q15_STAGE(NUM, SCALE, ID) \
    (MIMO_Q15_STAGE_JUMP_ONLY(ID) \
         ? herm_pivot_div_to_q15_jump_only_scaled((NUM), &(SCALE)) \
         : herm_pivot_div_to_q15_scaled((NUM), &(SCALE)))
#endif

static inline int32_t herm_pivot_div_to_q30_scaled(
    int32_t num,
    const herm_pivot_scale_t *scale)
{
    const int32_t q =
        (int32_t)((int64_t)num * (int64_t)scale->scale_q30);
#if MIMO_USE_SOLVE4_HERMITIAN_POS_PIVOT_SCALE
    const int neg = num < 0;
#else
    const int neg = (num < 0) ^ (scale->safe < 0);
#endif
    uint32_t mag_num = num < 0 ? (uint32_t)(-num) : (uint32_t)num;
    uint32_t mag_q = q < 0 ? (uint32_t)(-q) : (uint32_t)q;
    const uint64_t target = (uint64_t)mag_num << 30;
    const uint32_t before_jump = mag_q;
    uint32_t dec_steps = 0;
    uint32_t inc_steps = 0;

    mag_q = herm_pivot_apply_jump_corr(mag_q, target, scale);
#if MIMO_USE_SOLVE4_HERMITIAN_Q30_TWOJUMP_CORR
    mag_q = herm_pivot_apply_jump_corr(mag_q, target, scale);
#endif
    const uint32_t after_jump = mag_q;
#if MIMO_USE_SOLVE4_HERMITIAN_Q30_JUMP_ONLY
#elif MIMO_USE_SOLVE4_HERMITIAN_Q30_ONECORR
    if (mag_q > 0 && (uint64_t)mag_q * scale->mag_den > target) {
        --mag_q;
        ++dec_steps;
    }
    if ((uint64_t)(mag_q + 1u) * scale->mag_den <= target) {
        ++mag_q;
        ++inc_steps;
    }
#elif MIMO_USE_SOLVE4_HERMITIAN_Q30_FIVECORR
#define MIMO_Q30_DEC_CORR_ONCE() \
    do { \
        if (mag_q > 0 && (uint64_t)mag_q * scale->mag_den > target) { \
            --mag_q; \
            ++dec_steps; \
        } \
    } while (0)
#define MIMO_Q30_INC_CORR_ONCE() \
    do { \
        if ((uint64_t)(mag_q + 1u) * scale->mag_den <= target) { \
            ++mag_q; \
            ++inc_steps; \
        } \
    } while (0)
    MIMO_Q30_DEC_CORR_ONCE();
    MIMO_Q30_DEC_CORR_ONCE();
    MIMO_Q30_DEC_CORR_ONCE();
    MIMO_Q30_DEC_CORR_ONCE();
    MIMO_Q30_DEC_CORR_ONCE();
    MIMO_Q30_INC_CORR_ONCE();
    MIMO_Q30_INC_CORR_ONCE();
    MIMO_Q30_INC_CORR_ONCE();
    MIMO_Q30_INC_CORR_ONCE();
    MIMO_Q30_INC_CORR_ONCE();
#undef MIMO_Q30_INC_CORR_ONCE
#undef MIMO_Q30_DEC_CORR_ONCE
#elif MIMO_USE_SOLVE4_HERMITIAN_Q30_TWOJUMP_CORR
#define MIMO_Q30_DEC_CORR_ONCE() \
    do { \
        if (mag_q > 0 && (uint64_t)mag_q * scale->mag_den > target) { \
            --mag_q; \
            ++dec_steps; \
        } \
    } while (0)
#define MIMO_Q30_INC_CORR_ONCE() \
    do { \
        if ((uint64_t)(mag_q + 1u) * scale->mag_den <= target) { \
            ++mag_q; \
            ++inc_steps; \
        } \
    } while (0)
#if MIMO_USE_SOLVE4_HERMITIAN_Q30_TWOJUMP_CORR >= 1
    MIMO_Q30_DEC_CORR_ONCE();
    MIMO_Q30_INC_CORR_ONCE();
#endif
#if MIMO_USE_SOLVE4_HERMITIAN_Q30_TWOJUMP_CORR >= 2
    MIMO_Q30_DEC_CORR_ONCE();
    MIMO_Q30_INC_CORR_ONCE();
#endif
#if MIMO_USE_SOLVE4_HERMITIAN_Q30_TWOJUMP_CORR >= 3
    MIMO_Q30_DEC_CORR_ONCE();
    MIMO_Q30_INC_CORR_ONCE();
#endif
#undef MIMO_Q30_INC_CORR_ONCE
#undef MIMO_Q30_DEC_CORR_ONCE
#else
    while (mag_q > 0 && (uint64_t)mag_q * scale->mag_den > target) {
        --mag_q;
        ++dec_steps;
    }
    while ((uint64_t)(mag_q + 1u) * scale->mag_den <= target) {
        ++mag_q;
        ++inc_steps;
    }
#endif

    RECORD_PIVOT_CORR_Q30(before_jump, after_jump, dec_steps, inc_steps);

    return neg ? -(int32_t)mag_q : (int32_t)mag_q;
}

#if MIMO_SOLVE4_HERMITIAN_Q30_JUMP_ONLY_MASK
static inline int32_t herm_pivot_div_to_q30_jump_only_scaled(
    int32_t num,
    const herm_pivot_scale_t *scale)
{
    const int32_t q =
        (int32_t)((int64_t)num * (int64_t)scale->scale_q30);
#if MIMO_USE_SOLVE4_HERMITIAN_POS_PIVOT_SCALE
    const int neg = num < 0;
#else
    const int neg = (num < 0) ^ (scale->safe < 0);
#endif
    uint32_t mag_num = num < 0 ? (uint32_t)(-num) : (uint32_t)num;
    uint32_t mag_q = q < 0 ? (uint32_t)(-q) : (uint32_t)q;
    const uint64_t target = (uint64_t)mag_num << 30;
    const uint32_t before_jump = mag_q;

    mag_q = herm_pivot_apply_jump_corr(mag_q, target, scale);
    RECORD_PIVOT_CORR_Q30(before_jump, mag_q, 0, 0);

    return neg ? -(int32_t)mag_q : (int32_t)mag_q;
}

#define MIMO_Q30_STAGE_JUMP_ONLY(ID) \
    (((MIMO_SOLVE4_HERMITIAN_Q30_JUMP_ONLY_MASK) >> (ID)) & 1)
#define MIMO_HERM_PIVOT_DIV_Q30_STAGE(NUM, SCALE, ID) \
    (MIMO_Q30_STAGE_JUMP_ONLY(ID) \
         ? herm_pivot_div_to_q30_jump_only_scaled((NUM), &(SCALE)) \
         : herm_pivot_div_to_q30_scaled((NUM), &(SCALE)))
#endif

#if MIMO_USE_SOLVE4_HERMITIAN_Q30_PAIR_CORR
static MIMO_ALWAYS_INLINE mimo_c32_t c32_factor_q30_pivotscale_pair(
    mimo_c32_t a,
    const herm_pivot_scale_t *scale)
{
    const int32_t num_r = a.r;
    const int32_t num_i = -a.i;
    const int32_t q_r =
        (int32_t)((int64_t)num_r * (int64_t)scale->scale_q30);
    const int32_t q_i =
        (int32_t)((int64_t)num_i * (int64_t)scale->scale_q30);
#if MIMO_USE_SOLVE4_HERMITIAN_POS_PIVOT_SCALE
    const int neg_r = num_r < 0;
    const int neg_i = num_i < 0;
#else
    const int neg_r = (num_r < 0) ^ (scale->safe < 0);
    const int neg_i = (num_i < 0) ^ (scale->safe < 0);
#endif
    uint32_t mag_num_r = num_r < 0 ? (uint32_t)(-num_r) : (uint32_t)num_r;
    uint32_t mag_num_i = num_i < 0 ? (uint32_t)(-num_i) : (uint32_t)num_i;
    uint32_t mag_q_r = q_r < 0 ? (uint32_t)(-q_r) : (uint32_t)q_r;
    uint32_t mag_q_i = q_i < 0 ? (uint32_t)(-q_i) : (uint32_t)q_i;
    const uint64_t target_r = (uint64_t)mag_num_r << 30;
    const uint64_t target_i = (uint64_t)mag_num_i << 30;
    const uint32_t before_jump_r = mag_q_r;
    const uint32_t before_jump_i = mag_q_i;
    uint32_t dec_steps_r = 0;
    uint32_t inc_steps_r = 0;
    uint32_t dec_steps_i = 0;
    uint32_t inc_steps_i = 0;

    mag_q_r = herm_pivot_apply_jump_corr(mag_q_r, target_r, scale);
    mag_q_i = herm_pivot_apply_jump_corr(mag_q_i, target_i, scale);
    const uint32_t after_jump_r = mag_q_r;
    const uint32_t after_jump_i = mag_q_i;

    for (;;) {
        int changed = 0;

        if (mag_q_r > 0 && (uint64_t)mag_q_r * scale->mag_den > target_r) {
            --mag_q_r;
            ++dec_steps_r;
            changed = 1;
        }
        if (mag_q_i > 0 && (uint64_t)mag_q_i * scale->mag_den > target_i) {
            --mag_q_i;
            ++dec_steps_i;
            changed = 1;
        }
        if (!changed) {
            break;
        }
    }

    for (;;) {
        int changed = 0;

        if ((uint64_t)(mag_q_r + 1u) * scale->mag_den <= target_r) {
            ++mag_q_r;
            ++inc_steps_r;
            changed = 1;
        }
        if ((uint64_t)(mag_q_i + 1u) * scale->mag_den <= target_i) {
            ++mag_q_i;
            ++inc_steps_i;
            changed = 1;
        }
        if (!changed) {
            break;
        }
    }

    RECORD_PIVOT_CORR_Q30(before_jump_r, after_jump_r,
                          dec_steps_r, inc_steps_r);
    RECORD_PIVOT_CORR_Q30(before_jump_i, after_jump_i,
                          dec_steps_i, inc_steps_i);

    return c32_make(neg_r ? -(int32_t)mag_q_r : (int32_t)mag_q_r,
                    neg_i ? -(int32_t)mag_q_i : (int32_t)mag_q_i);
}
#endif

static inline mimo_c32_t c32_div_real_to_q15_pivotscale(
    mimo_c32_t a,
    const herm_pivot_scale_t *scale)
{
    return c32_make(herm_pivot_div_to_q15_scaled(a.r, scale),
                    herm_pivot_div_to_q15_scaled(a.i, scale));
}

#if MIMO_USE_SOLVE4_HERMITIAN_HAND_SCHED || \
    MIMO_USE_SOLVE4_HERMITIAN_FACTOR_CALL
static MIMO_NOINLINE mimo_c32_t c32_factor_q30_pivotscale_call(
    mimo_c32_t a,
    const herm_pivot_scale_t *scale)
{
    return c32_make(herm_pivot_div_to_q30_scaled(a.r, scale),
                    herm_pivot_div_to_q30_scaled(-a.i, scale));
}
#endif

#if MIMO_USE_SOLVE4_HERMITIAN_HAND_SCHED || \
    MIMO_USE_SOLVE4_HERMITIAN_Q15_CALL
static MIMO_NOINLINE mimo_c32_t c32_div_real_to_q15_pivotscale_call(
    mimo_c32_t a,
    const herm_pivot_scale_t *scale)
{
    return c32_div_real_to_q15_pivotscale(a, scale);
}
#endif
#endif

#if MIMO_USE_SOLVE4_HERMITIAN_HAND_SCHED
#if MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE
#define MIMO_HERM_FACTOR_Q30(VALUE, PIVOT, SCALE) \
    c32_factor_q30_pivotscale_call((VALUE), &(SCALE))
#define MIMO_HERM_DIV_Q15(SUM, PIVOT, SCALE) \
    c32_div_real_to_q15_pivotscale_call((SUM), &(SCALE))
#else
#define MIMO_HERM_FACTOR_Q30(VALUE, PIVOT, SCALE) \
    c32_make(div_to_q30_i32((VALUE).r, (PIVOT)), \
             div_to_q30_i32(-(VALUE).i, (PIVOT)))
#define MIMO_HERM_DIV_Q15(SUM, PIVOT, SCALE) \
    c32_div_real_to_q15_exact((SUM), (PIVOT))
#endif

static inline void mimo_solve_normal4_site4_hermitian_zf_hand_lane_q15(
    const mimo_c32_t *restrict a,
    const mimo_c32_t *restrict b,
    mimo_c16_t *restrict x)
{
    mimo_c32_t m00 = a[0];
    mimo_c32_t m01 = a[1];
    mimo_c32_t m02 = a[2];
    mimo_c32_t m03 = a[3];
    mimo_c32_t m11 = a[5];
    mimo_c32_t m12 = a[6];
    mimo_c32_t m13 = a[7];
    mimo_c32_t m22 = a[10];
    mimo_c32_t m23 = a[11];
    mimo_c32_t m33 = a[15];
    mimo_c32_t r0 = b[0];
    mimo_c32_t r1 = b[1];
    mimo_c32_t r2 = b[2];
    mimo_c32_t r3 = b[3];

    m00.i = 0;
    m11.i = 0;
    m22.i = 0;
    m33.i = 0;

    const int32_t p0 = MIMO_HERM_SAFE_PIVOT_I32(m00.r);
#if MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE
    const herm_pivot_scale_t s0 = herm_pivot_scale_init(p0);
#endif
    mimo_c32_t f = MIMO_HERM_FACTOR_Q30(m01, p0, s0);
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
    m11.r -= c32_mul_q30_q15_real(f, m01);
#else
    m11 = c32_sub(m11, c32_mul_q30_q15(f, m01));
#endif
    m12 = c32_sub(m12, c32_mul_q30_q15(f, m02));
    m13 = c32_sub(m13, c32_mul_q30_q15(f, m03));
    r1 = c32_sub(r1, c32_mul_q30_q15(f, r0));

    f = MIMO_HERM_FACTOR_Q30(m02, p0, s0);
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
    m22.r -= c32_mul_q30_q15_real(f, m02);
#else
    m22 = c32_sub(m22, c32_mul_q30_q15(f, m02));
#endif
    m23 = c32_sub(m23, c32_mul_q30_q15(f, m03));
    r2 = c32_sub(r2, c32_mul_q30_q15(f, r0));

    f = MIMO_HERM_FACTOR_Q30(m03, p0, s0);
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
    m33.r -= c32_mul_q30_q15_real(f, m03);
#else
    m33 = c32_sub(m33, c32_mul_q30_q15(f, m03));
#endif
    r3 = c32_sub(r3, c32_mul_q30_q15(f, r0));

    const int32_t p1 = MIMO_HERM_SAFE_PIVOT_I32(m11.r);
#if MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE
    const herm_pivot_scale_t s1 = herm_pivot_scale_init(p1);
#endif
    f = MIMO_HERM_FACTOR_Q30(m12, p1, s1);
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
    m22.r -= c32_mul_q30_q15_real(f, m12);
#else
    m22 = c32_sub(m22, c32_mul_q30_q15(f, m12));
#endif
    m23 = c32_sub(m23, c32_mul_q30_q15(f, m13));
    r2 = c32_sub(r2, c32_mul_q30_q15(f, r1));

    f = MIMO_HERM_FACTOR_Q30(m13, p1, s1);
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
    m33.r -= c32_mul_q30_q15_real(f, m13);
#else
    m33 = c32_sub(m33, c32_mul_q30_q15(f, m13));
#endif
    r3 = c32_sub(r3, c32_mul_q30_q15(f, r1));

    const int32_t p2 = MIMO_HERM_SAFE_PIVOT_I32(m22.r);
#if MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE
    const herm_pivot_scale_t s2 = herm_pivot_scale_init(p2);
#endif
    f = MIMO_HERM_FACTOR_Q30(m23, p2, s2);
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
    m33.r -= c32_mul_q30_q15_real(f, m23);
#else
    m33 = c32_sub(m33, c32_mul_q30_q15(f, m23));
#endif
    r3 = c32_sub(r3, c32_mul_q30_q15(f, r2));

    const int32_t p3 = MIMO_HERM_SAFE_PIVOT_I32(m33.r);
#if MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE
    const herm_pivot_scale_t s3 = herm_pivot_scale_init(p3);
#endif
    const mimo_c32_t x3 = MIMO_HERM_DIV_Q15(r3, p3, s3);
    mimo_c32_t sum = c32_sub(r2, c32_mul_q15(m23, x3));
    const mimo_c32_t x2 = MIMO_HERM_DIV_Q15(sum, p2, s2);

    sum = c32_sub(r1, c32_mul_q15(m12, x2));
    sum = c32_sub(sum, c32_mul_q15(m13, x3));
    const mimo_c32_t x1 = MIMO_HERM_DIV_Q15(sum, p1, s1);

    sum = c32_sub(r0, c32_mul_q15(m01, x1));
    sum = c32_sub(sum, c32_mul_q15(m02, x2));
    sum = c32_sub(sum, c32_mul_q15(m03, x3));
    const mimo_c32_t x0 = MIMO_HERM_DIV_Q15(sum, p0, s0);

    x[0] = mimo_make_c16(mimo_solve_store_i16(x0.r),
                         mimo_solve_store_i16(x0.i));
    x[1] = mimo_make_c16(mimo_solve_store_i16(x1.r),
                         mimo_solve_store_i16(x1.i));
    x[2] = mimo_make_c16(mimo_solve_store_i16(x2.r),
                         mimo_solve_store_i16(x2.i));
    x[3] = mimo_make_c16(mimo_solve_store_i16(x3.r),
                         mimo_solve_store_i16(x3.i));
}

#undef MIMO_HERM_FACTOR_Q30
#undef MIMO_HERM_DIV_Q15
#endif

void mimo_solve_normal4_site4_hermitian_zf_q15(
    mimo_c32_t a_lane[restrict MIMO_SITE4_LANES][MIMO_N4 * MIMO_N4],
    mimo_c32_t b_lane[restrict MIMO_SITE4_LANES][MIMO_N4],
    mimo_c16_t *restrict x_block)
{
#if MIMO_USE_SOLVE4_HERMITIAN_HAND_SCHED && !MIMO_COLLECT_SCHUR_STATS
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        mimo_solve_normal4_site4_hermitian_zf_hand_lane_q15(
            a_lane[lane], b_lane[lane], &x_block[lane * MIMO_N4]);
    }
    return;
#endif

#if MIMO_USE_SOLVE4_HERMITIAN_LANE_OUTER && !MIMO_COLLECT_SCHUR_STATS
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        mimo_c32_t *restrict a = a_lane[lane];
        mimo_c32_t *restrict b = b_lane[lane];

        mimo_collect_schur4_q15(a, b, 0);

#if !MIMO_USE_SITE4_HERMITIAN_UPPER_BUILD
        for (int i = 0; i < MIMO_N4; ++i) {
            a[i * MIMO_N4 + i].i = 0;
        }
#endif

        for (int k = 0; k < MIMO_N4; ++k) {
            const int kk = k * MIMO_N4 + k;
            const int32_t pivot = MIMO_HERM_SAFE_PIVOT_I32(a[kk].r);
#if !MIMO_USE_SITE4_HERMITIAN_UPPER_BUILD
            a[kk].i = 0;
#endif
#if MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE
            const herm_pivot_scale_t factor_scale =
                herm_pivot_scale_init(pivot);
#endif

            for (int i = k + 1; i < MIMO_N4; ++i) {
                const int ki = k * MIMO_N4 + i;
#if MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE
                const mimo_c32_t factor_q30 = c32_make(
                    herm_pivot_div_to_q30_scaled(a[ki].r, &factor_scale),
                    herm_pivot_div_to_q30_scaled(-a[ki].i, &factor_scale));
#else
                const mimo_c32_t factor_q30 = c32_make(
                    div_to_q30_i32(a[ki].r, pivot),
                    div_to_q30_i32(-a[ki].i, pivot));
#endif

                for (int j = i; j < MIMO_N4; ++j) {
                    const int ij = i * MIMO_N4 + j;
                    const int kj = k * MIMO_N4 + j;
                    const mimo_c32_t t =
                        c32_mul_q30_q15(factor_q30, a[kj]);

                    a[ij] = c32_sub(a[ij], t);
                }

                const mimo_c32_t tb = c32_mul_q30_q15(factor_q30, b[k]);
                b[i] = c32_sub(b[i], tb);
            }
        }

        for (int i = MIMO_N4 - 1; i >= 0; --i) {
            mimo_c32_t sum = b[i];

            for (int j = i + 1; j < MIMO_N4; ++j) {
                sum = c32_sub(sum,
                              c32_mul_q15(a[i * MIMO_N4 + j], b[j]));
            }
            const int32_t pivot =
                MIMO_HERM_SAFE_PIVOT_I32(a[i * MIMO_N4 + i].r);
#if MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE
            const herm_pivot_scale_t scale =
                herm_pivot_scale_init(pivot);
            b[i] = c32_div_real_to_q15_pivotscale(sum, &scale);
#else
            b[i] = c32_div_real_to_q15_exact(sum, pivot);
#endif
        }

        mimo_c16_t *x = &x_block[lane * MIMO_N4];

        for (int i = 0; i < MIMO_N4; ++i) {
            x[i] = mimo_make_c16(mimo_solve_store_i16(b[i].r),
                                 mimo_solve_store_i16(b[i].i));
        }
    }
#else
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        mimo_collect_schur4_q15(a_lane[lane], b_lane[lane], 0);
    }

#if !MIMO_USE_SITE4_HERMITIAN_UPPER_BUILD || MIMO_COLLECT_SCHUR_STATS
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        for (int i = 0; i < MIMO_N4; ++i) {
            a_lane[lane][i * MIMO_N4 + i].i = 0;
        }
    }
#endif

    for (int k = 0; k < MIMO_N4; ++k) {
        for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
            const int kk = k * MIMO_N4 + k;
            const int32_t pivot = MIMO_HERM_SAFE_PIVOT_I32(a_lane[lane][kk].r);
#if !MIMO_USE_SITE4_HERMITIAN_UPPER_BUILD || MIMO_COLLECT_SCHUR_STATS
            a_lane[lane][kk].i = 0;
#endif
#if MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE
            const herm_pivot_scale_t factor_scale =
                herm_pivot_scale_init(pivot);
#endif

            for (int i = k + 1; i < MIMO_N4; ++i) {
                const int ki = k * MIMO_N4 + i;
#if MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE
                const mimo_c32_t factor_q30 = c32_make(
                    herm_pivot_div_to_q30_scaled(
                        a_lane[lane][ki].r, &factor_scale),
                    herm_pivot_div_to_q30_scaled(
                        -a_lane[lane][ki].i, &factor_scale));
#else
                const mimo_c32_t factor_q30 = c32_make(
                    div_to_q30_i32(a_lane[lane][ki].r, pivot),
                    div_to_q30_i32(-a_lane[lane][ki].i, pivot));
#endif

                for (int j = i; j < MIMO_N4; ++j) {
                    const int ij = i * MIMO_N4 + j;
                    const int kj = k * MIMO_N4 + j;
                    const mimo_c32_t t = c32_mul_q30_q15(
                        factor_q30, a_lane[lane][kj]);

                    a_lane[lane][ij] =
                        c32_sub(a_lane[lane][ij], t);
                }

                const mimo_c32_t tb =
                    c32_mul_q30_q15(factor_q30, b_lane[lane][k]);
                b_lane[lane][i] = c32_sub(b_lane[lane][i], tb);
            }
        }
    }

    for (int i = MIMO_N4 - 1; i >= 0; --i) {
        for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
            mimo_c32_t sum = b_lane[lane][i];

            for (int j = i + 1; j < MIMO_N4; ++j) {
                sum = c32_sub(sum,
                              c32_mul_q15(a_lane[lane][i * MIMO_N4 + j],
                                          b_lane[lane][j]));
            }
            const int32_t pivot =
                MIMO_HERM_SAFE_PIVOT_I32(a_lane[lane][i * MIMO_N4 + i].r);
#if MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE
            const herm_pivot_scale_t scale =
                herm_pivot_scale_init(pivot);
            b_lane[lane][i] = c32_div_real_to_q15_pivotscale(sum, &scale);
#else
            b_lane[lane][i] = c32_div_real_to_q15_exact(sum, pivot);
#endif
        }
    }

    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        mimo_c16_t *x = &x_block[lane * MIMO_N4];

        for (int i = 0; i < MIMO_N4; ++i) {
            x[i] = mimo_make_c16(
                mimo_solve_store_i16(b_lane[lane][i].r),
                mimo_solve_store_i16(b_lane[lane][i].i));
        }
    }
#endif
}
#endif

#if MIMO_USE_SOLVE4_SCHUR
static inline mimo_c64_t schur4_c64_from_c32(mimo_c32_t x)
{
    return c64_make(x.r, x.i);
}

static inline mimo_c64_t schur4_c64_add(mimo_c64_t a, mimo_c64_t b)
{
    return c64_make(a.r + b.r, a.i + b.i);
}

static inline mimo_c64_t schur4_c64_neg(mimo_c64_t a)
{
    return c64_make(-a.r, -a.i);
}

static inline mimo_c64_t schur4_mul_q15_to_q30(mimo_c64_t a,
                                                mimo_c64_t b)
{
    return c64_make(a.r * b.r - a.i * b.i,
                    a.r * b.i + a.i * b.r);
}

static inline mimo_c64_t schur4_mul_real_q15_to_q30(mimo_c64_t a,
                                                     int64_t s)
{
    return c64_make(a.r * s, a.i * s);
}

#if MIMO_USE_SOLVE4_SCHUR_Q15_RECIP
typedef struct {
    uint32_t scale_q30;
#if MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 3 && \
    MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 4 && \
    MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 5
    int den_neg;
#endif
} schur4_q15_recip_scale_t;

static MIMO_ALWAYS_INLINE mimo_c64_t schur4_div_q30_to_q15_recip(
    mimo_c64_t num_q30,
    int64_t den_q30);
#endif

static inline mimo_c64_t schur4_div_q30_to_q15(mimo_c64_t num_q30,
                                                int64_t den_q30)
{
#if MIMO_USE_SOLVE4_SCHUR_Q15_RECIP
    return schur4_div_q30_to_q15_recip(num_q30, den_q30);
#else
    den_q30 = safe_pivot_i64(den_q30);
    return c64_make((num_q30.r << MIMO_Q15_SHIFT) / den_q30,
                    (num_q30.i << MIMO_Q15_SHIFT) / den_q30);
#endif
}

#if MIMO_USE_SOLVE4_SCHUR_Q15_RECIP
#define MIMO_SCHUR4_DET_Q15_RECIP_MIN 3400u
#define MIMO_SCHUR4_DET_Q15_RECIP_STEP_SHIFT 4u
#define MIMO_SCHUR4_DET_Q15_RECIP_LAST 63u

#if MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 2 && \
    MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 5
static const uint32_t schur4_det_q15_recip_base_q30[] = {
    315806, 314327, 312861, 311410, 309971, 308546, 307134, 305735,
    304348, 302974, 301612, 300263, 298925, 297600, 296286, 294984,
    293693, 292413, 291144, 289887, 288640, 287404, 286178, 284963,
    283758, 282563, 281378, 280204, 279038, 277883, 276737, 275601,
    274473, 273355, 272246, 271146, 270055, 268973, 267899, 266834,
    265777, 264729, 263689, 262657, 261632, 260616, 259608, 258608,
    257615, 256630, 255652, 254682, 253719, 252764, 251815, 250874,
    249939, 249012, 248091, 247178, 246271, 245370, 244476, 243589,
};

static const int16_t schur4_det_q15_recip_delta_q30[] = {
    -1479, -1466, -1451, -1439, -1425, -1412, -1399, -1387,
    -1374, -1362, -1349, -1338, -1325, -1314, -1302, -1291,
    -1280, -1269, -1257, -1247, -1236, -1226, -1215, -1205,
    -1195, -1185, -1174, -1166, -1155, -1146, -1136, -1128,
    -1118, -1109, -1100, -1091, -1082, -1074, -1065, -1057,
    -1048, -1040, -1032, -1025, -1016, -1008, -1000, -993,
    -985, -978, -970, -963, -955, -949, -941, -935,
    -927, -921, -913, -907, -901, -894, -887, 0,
};
#endif

#if MIMO_USE_SOLVE4_SCHUR_Q15_RECIP == 2
#define MIMO_SCHUR4_DET_Q15_RECIP_DIRECT_STEP_SHIFT 2u
#define MIMO_SCHUR4_DET_Q15_RECIP_DIRECT_LAST 252u

static const uint32_t schur4_det_q15_recip_direct_q30[] = {
    315806, 315435, 315065, 314696, 314327, 313960, 313593, 313227,
    312862, 312498, 312134, 311772, 311410, 311049, 310689, 310330,
    309972, 309614, 309257, 308902, 308547, 308192, 307839, 307486,
    307134, 306783, 306433, 306084, 305735, 305387, 305040, 304694,
    304349, 304004, 303660, 303317, 302975, 302633, 302292, 301952,
    301613, 301274, 300937, 300600, 300263, 299928, 299593, 299259,
    298926, 298593, 298262, 297931, 297600, 297271, 296942, 296614,
    296286, 295960, 295634, 295309, 294984, 294660, 294337, 294015,
    293693, 293372, 293052, 292732, 292413, 292095, 291778, 291461,
    291145, 290829, 290515, 290200, 289887, 289574, 289262, 288951,
    288640, 288330, 288021, 287712, 287404, 287097, 286790, 286484,
    286179, 285874, 285570, 285266, 284963, 284661, 284360, 284059,
    283758, 283459, 283160, 282861, 282564, 282267, 281970, 281674,
    281379, 281084, 280790, 280497, 280204, 279912, 279620, 279329,
    279039, 278749, 278460, 278171, 277883, 277596, 277309, 277023,
    276738, 276453, 276168, 275884, 275601, 275318, 275036, 274755,
    274474, 274194, 273914, 273635, 273356, 273078, 272800, 272523,
    272247, 271971, 271696, 271421, 271147, 270873, 270600, 270328,
    270056, 269784, 269514, 269243, 268973, 268704, 268435, 268167,
    267900, 267633, 267366, 267100, 266834, 266569, 266305, 266041,
    265778, 265515, 265252, 264991, 264729, 264468, 264208, 263948,
    263689, 263430, 263172, 262914, 262657, 262400, 262144, 261888,
    261633, 261378, 261124, 260870, 260617, 260364, 260112, 259860,
    259609, 259358, 259108, 258858, 258608, 258359, 258111, 257863,
    257616, 257369, 257122, 256876, 256630, 256385, 256141, 255897,
    255653, 255410, 255167, 254924, 254683, 254441, 254200, 253960,
    253720, 253480, 253241, 253002, 252764, 252526, 252289, 252052,
    251816, 251580, 251344, 251109, 250874, 250640, 250406, 250173,
    249940, 249707, 249475, 249244, 249012, 248782, 248551, 248321,
    248092, 247863, 247634, 247406, 247178, 246951, 246724, 246497,
    246271, 246045, 245820, 245595, 245371, 245147, 244923, 244700,
    244477, 244254, 244032, 243811, 243589,
};
#endif

#if MIMO_USE_SOLVE4_SCHUR_Q15_RECIP == 5
#define MIMO_SCHUR4_DET_Q15_RECIP_DIRECT1_LAST 1008u
#define MIMO_SCHUR4_DET_Q15_RECIP_DIRECT1_ENTRY(rel_) \
    ((uint32_t)(1073741824ull / \
                (MIMO_SCHUR4_DET_Q15_RECIP_MIN + (uint32_t)(rel_))))

static const uint32_t schur4_det_q15_recip_direct1_q30[] = {
#include "mimo_schur_recip_direct1.inc"
};

#undef MIMO_SCHUR4_DET_Q15_RECIP_DIRECT1_ENTRY

#if MIMO_USE_EXT_MMSE4_PACKED_E16_RECIP
#define MIMO_SCHUR4_DET_Q15_RECIP_DIRECT1_ENTRY(rel_) \
    ((uint16_t)((1073741824ull / \
                 (MIMO_SCHUR4_DET_Q15_RECIP_MIN + (uint32_t)(rel_))) \
                >> 4))

static const uint16_t schur4_det_q15_recip_direct1_q26_padded[]
    MIMO_ALIGN64 = {
    [MIMO_SCHUR4_DET_Q15_RECIP_MIN] =
#include "mimo_schur_recip_direct1.inc"
};

#undef MIMO_SCHUR4_DET_Q15_RECIP_DIRECT1_ENTRY
#endif
#endif

static MIMO_ALWAYS_INLINE uint64_t schur4_abs_i64(int64_t x)
{
    return x < 0 ? (uint64_t)(-x) : (uint64_t)x;
}

static MIMO_ALWAYS_INLINE uint32_t schur4_round_q30_mag_to_q15(
    uint64_t mag_q30)
{
    return (uint32_t)((mag_q30 + MIMO_Q15_ROUND) >> MIMO_Q15_SHIFT);
}

static MIMO_ALWAYS_INLINE uint32_t schur4_round_q30_mag_to_recip_num(
    uint64_t mag_q30)
{
    return (uint32_t)(
        (mag_q30 + (1ull << (MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT - 1))) >>
        MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT);
}

static MIMO_ALWAYS_INLINE uint32_t schur4_det_q15_from_q30(int64_t den_q30)
{
    uint32_t den_q15 =
        schur4_round_q30_mag_to_q15(schur4_abs_i64(safe_pivot_i64(den_q30)));
    const uint32_t max_den =
        MIMO_SCHUR4_DET_Q15_RECIP_MIN +
        (MIMO_SCHUR4_DET_Q15_RECIP_LAST
         << MIMO_SCHUR4_DET_Q15_RECIP_STEP_SHIFT);

    if (den_q15 < MIMO_SCHUR4_DET_Q15_RECIP_MIN) {
        den_q15 = MIMO_SCHUR4_DET_Q15_RECIP_MIN;
    } else if (den_q15 > max_den) {
        den_q15 = max_den;
    }
    return den_q15;
}

static MIMO_ALWAYS_INLINE uint32_t schur4_det_q15_recip_scale_q30(
    uint32_t den_q15)
{
#if MIMO_USE_SOLVE4_SCHUR_Q15_RECIP == 2
    uint32_t rel = den_q15 - MIMO_SCHUR4_DET_Q15_RECIP_MIN;
    uint32_t idx =
        (rel + (1u << (MIMO_SCHUR4_DET_Q15_RECIP_DIRECT_STEP_SHIFT - 1u))) >>
        MIMO_SCHUR4_DET_Q15_RECIP_DIRECT_STEP_SHIFT;
    if (idx > MIMO_SCHUR4_DET_Q15_RECIP_DIRECT_LAST) {
        idx = MIMO_SCHUR4_DET_Q15_RECIP_DIRECT_LAST;
    }
    return schur4_det_q15_recip_direct_q30[idx];
#elif MIMO_USE_SOLVE4_SCHUR_Q15_RECIP == 5
    const uint32_t rel = den_q15 - MIMO_SCHUR4_DET_Q15_RECIP_MIN;
    return schur4_det_q15_recip_direct1_q30[rel];
#else
    const uint32_t rel = den_q15 - MIMO_SCHUR4_DET_Q15_RECIP_MIN;
    const uint32_t seg = rel >> MIMO_SCHUR4_DET_Q15_RECIP_STEP_SHIFT;
    const uint32_t off =
        rel & ((1u << MIMO_SCHUR4_DET_Q15_RECIP_STEP_SHIFT) - 1u);
    const uint32_t base = schur4_det_q15_recip_base_q30[seg];
    const uint32_t drop =
        (uint32_t)(-schur4_det_q15_recip_delta_q30[seg]) * off;

    return base - ((drop + 8u) >> MIMO_SCHUR4_DET_Q15_RECIP_STEP_SHIFT);
#endif
}

static MIMO_ALWAYS_INLINE schur4_q15_recip_scale_t
schur4_q15_recip_scale_from_den(int64_t den_q30)
{
#if MIMO_USE_SOLVE4_SCHUR_Q15_RECIP == 3 || \
    MIMO_USE_SOLVE4_SCHUR_Q15_RECIP == 4 || \
    MIMO_USE_SOLVE4_SCHUR_Q15_RECIP == 5
    const uint32_t den_q15 =
        (uint32_t)((den_q30 + MIMO_Q15_ROUND) >> MIMO_Q15_SHIFT);
    schur4_q15_recip_scale_t scale;

    scale.scale_q30 = schur4_det_q15_recip_scale_q30(den_q15);
    return scale;
#else
    const int64_t safe_den = safe_pivot_i64(den_q30);
    const uint32_t den_q15 = schur4_det_q15_from_q30(safe_den);
    schur4_q15_recip_scale_t scale;

    scale.scale_q30 = schur4_det_q15_recip_scale_q30(den_q15);
    scale.den_neg = safe_den < 0;
    return scale;
#endif
}

static MIMO_ALWAYS_INLINE int32_t schur4_div_q30_scalar_to_q15_recip(
    int64_t num_q30,
    schur4_q15_recip_scale_t scale,
    int use_signed_shift)
{
#if MIMO_USE_SOLVE4_SCHUR_Q15_RECIP == 4 || \
    MIMO_USE_SOLVE4_SCHUR_Q15_RECIP == 5
    if (use_signed_shift) {
#if MIMO_USE_MMSE4_SCHUR_NUM_ARSHIFT_SCALE
        const int32_t num_qs =
            (int32_t)(num_q30 >> MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT);
#else
        const int32_t num_qs = (int32_t)(
            (num_q30 +
             (1ll << (MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT - 1))) >>
            MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT);
#endif
#if MIMO_USE_SOLVE4_SCHUR_SCALE16_APPROX
        const int32_t scale_q26 = (int32_t)(scale.scale_q30 >> 4);
        const int32_t prod = num_qs * scale_q26;

        return prod >> (30 - MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT - 4);
#else
        const int32_t prod = num_qs * (int32_t)scale.scale_q30;

        return prod >> (30 - MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT);
#endif
    }
#elif MIMO_USE_SOLVE4_SCHUR_Q15_RECIP == 3
    (void)use_signed_shift;
#else
    (void)use_signed_shift;
#endif
#if MIMO_USE_SOLVE4_SCHUR_Q15_RECIP == 3 || \
    MIMO_USE_SOLVE4_SCHUR_Q15_RECIP == 4 || \
    MIMO_USE_SOLVE4_SCHUR_Q15_RECIP == 5
    const int neg = num_q30 < 0;
    const uint32_t num_q15 =
        schur4_round_q30_mag_to_recip_num(schur4_abs_i64(num_q30));
    const uint32_t prod = num_q15 * scale.scale_q30;
    const uint32_t mag_q15 =
        prod >> (30 - MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT);

    return neg ? -(int32_t)mag_q15 : (int32_t)mag_q15;
#else
    const int neg = (num_q30 < 0) ^ scale.den_neg;
    const uint32_t num_q15 =
        schur4_round_q30_mag_to_recip_num(schur4_abs_i64(num_q30));
    const uint32_t prod = num_q15 * scale.scale_q30;
    const uint32_t mag_q15 =
        prod >> (30 - MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT);

    return neg ? -(int32_t)mag_q15 : (int32_t)mag_q15;
#endif
}

static MIMO_ALWAYS_INLINE mimo_c64_t schur4_scale_q30_to_q15_recip(
    mimo_c64_t num_q30,
    schur4_q15_recip_scale_t scale,
    int use_signed_shift)
{
    return c64_make(
        schur4_div_q30_scalar_to_q15_recip(num_q30.r, scale,
                                           use_signed_shift),
        schur4_div_q30_scalar_to_q15_recip(num_q30.i, scale,
                                           use_signed_shift));
}

static MIMO_ALWAYS_INLINE mimo_c64_t schur4_div_q30_to_q15_recip(
    mimo_c64_t num_q30,
    int64_t den_q30)
{
    return schur4_scale_q30_to_q15_recip(
        num_q30, schur4_q15_recip_scale_from_den(den_q30), 1);
}
#endif

#if MIMO_USE_SOLVE4_SCHUR_DET_SCALE
static MIMO_ALWAYS_INLINE int64_t schur4_det_scale_q45(int64_t den_q30)
{
    den_q30 = safe_pivot_i64(den_q30);
    const int neg = den_q30 < 0;
    const uint64_t mag_den =
        neg ? (uint64_t)(-den_q30) : (uint64_t)den_q30;
    const uint64_t scale = (1ull << 45) / mag_den;

    return neg ? -(int64_t)scale : (int64_t)scale;
}

static MIMO_ALWAYS_INLINE int64_t schur4_apply_det_scale_q15(
    int64_t num_q30,
    int64_t scale_q45)
{
    const int64_t prod = num_q30 * scale_q45;

    if (prod >= 0) {
        return prod >> 30;
    }
    return -((-prod) >> 30);
}

static MIMO_ALWAYS_INLINE mimo_c64_t schur4_scale_q30_to_q15(
    mimo_c64_t num_q30,
    int64_t scale_q45)
{
    return c64_make(schur4_apply_det_scale_q15(num_q30.r, scale_q45),
                    schur4_apply_det_scale_q15(num_q30.i, scale_q45));
}
#endif

static inline int64_t schur4_det2_q30(mimo_c64_t a00,
                                       mimo_c64_t a01,
                                       mimo_c64_t a11)
{
    return a00.r * a11.r - (a01.r * a01.r + a01.i * a01.i);
}

#if MIMO_USE_SOLVE4_SCHUR_BINV_Q15
static MIMO_ALWAYS_INLINE int64_t schur4_div_real_q15(int64_t num_q15,
                                                       int64_t den_q30)
{
    den_q30 = safe_pivot_i64(den_q30);
    return (num_q15 << MIMO_Q15_SHIFT) / den_q30;
}

static MIMO_ALWAYS_INLINE mimo_c64_t schur4_inv2_apply_q15(
    mimo_c64_t inv00,
    mimo_c64_t inv01,
    mimo_c64_t inv11,
    mimo_c64_t v0,
    mimo_c64_t v1,
    int row)
{
    if (row == 0) {
        return schur4_c64_add(c64_mul_real_q15(v0, inv00.r),
                              c64_mul_q15(inv01, v1));
    }

    return schur4_c64_add(c64_mul_q15(c64_conj(inv01), v0),
                          c64_mul_real_q15(v1, inv11.r));
}
#endif

static MIMO_ALWAYS_INLINE mimo_c64_t schur4_solve2_vec_q15(mimo_c64_t a00,
                                                           mimo_c64_t a01,
                                                           mimo_c64_t a11,
                                                           int64_t det_q30,
#if MIMO_USE_SOLVE4_SCHUR_DET_SCALE
                                                           int64_t det_scale_q45,
#elif MIMO_USE_SOLVE4_SCHUR_Q15_RECIP
                                                           schur4_q15_recip_scale_t det_scale,
                                                           int use_signed_shift,
#endif
                                                           mimo_c64_t v0,
                                                           mimo_c64_t v1,
                                                           int row)
{
    mimo_c64_t num;

    if (row == 0) {
        num = c64_sub(schur4_mul_real_q15_to_q30(v0, a11.r),
                      schur4_mul_q15_to_q30(a01, v1));
    } else {
        num = schur4_c64_add(
            schur4_mul_q15_to_q30(schur4_c64_neg(c64_conj(a01)), v0),
            schur4_mul_real_q15_to_q30(v1, a00.r));
    }

#if MIMO_USE_SOLVE4_SCHUR_DET_SCALE
    return schur4_scale_q30_to_q15(num, det_scale_q45);
#elif MIMO_USE_SOLVE4_SCHUR_Q15_RECIP
    (void)det_q30;
    return schur4_scale_q30_to_q15_recip(num, det_scale,
                                         use_signed_shift);
#else
    return schur4_div_q30_to_q15(num, det_q30);
#endif
}

#if MIMO_USE_SOLVE4_SCHUR_FUSED_INNER || MIMO_USE_SOLVE4_SCHUR_FUSED_INNER_MMSE
typedef struct {
    mimo_c64_t row0;
    mimo_c64_t row1;
} schur4_solve2_num_pair_t;

static MIMO_ALWAYS_INLINE schur4_solve2_num_pair_t
schur4_solve2_pair_num_q30(mimo_c64_t a00,
                           mimo_c64_t a01,
                           mimo_c64_t a11,
                           mimo_c64_t v0,
                           mimo_c64_t v1)
{
    schur4_solve2_num_pair_t out;

    out.row0 = c64_sub(schur4_mul_real_q15_to_q30(v0, a11.r),
                       schur4_mul_q15_to_q30(a01, v1));
    out.row1 = schur4_c64_add(
        schur4_mul_q15_to_q30(schur4_c64_neg(c64_conj(a01)), v0),
        schur4_mul_real_q15_to_q30(v1, a00.r));
    return out;
}

static MIMO_ALWAYS_INLINE mimo_c64_t schur4_mul_conj_q15_q30_to_q30(
    mimo_c64_t a_q15,
    mimo_c64_t b_q30)
{
    return c64_make(q15_round_shift_i64(a_q15.r * b_q30.r +
                                        a_q15.i * b_q30.i),
                    q15_round_shift_i64(a_q15.r * b_q30.i -
                                        a_q15.i * b_q30.r));
}

static MIMO_ALWAYS_INLINE mimo_c64_t schur4_inner_num_to_q15_recip(
    mimo_c64_t e0,
    mimo_c64_t e1,
    schur4_solve2_num_pair_t w_num,
    schur4_q15_recip_scale_t det_scale,
    int use_signed_shift)
{
    const mimo_c64_t num = schur4_c64_add(
        schur4_mul_conj_q15_q30_to_q30(e0, w_num.row0),
        schur4_mul_conj_q15_q30_to_q30(e1, w_num.row1));

    return schur4_scale_q30_to_q15_recip(num, det_scale,
                                         use_signed_shift);
}

static MIMO_ALWAYS_INLINE mimo_c64_t schur4_fused_top_rhs_q15(
    mimo_c64_t b,
    mimo_c64_t e0,
    mimo_c64_t e1,
    mimo_c64_t x2,
    mimo_c64_t x3)
{
    mimo_c64_t rhs = c64_sub(b, c64_mul_q15(e0, x2));

    return c64_sub(rhs, c64_mul_q15(e1, x3));
}
#endif

#if MIMO_USE_SOLVE4_SCHUR_PAIR2
typedef struct {
    mimo_c64_t row0;
    mimo_c64_t row1;
} schur4_solve2_pair_t;

static MIMO_ALWAYS_INLINE schur4_solve2_pair_t schur4_solve2_pair_q15(
    mimo_c64_t a00,
    mimo_c64_t a01,
    mimo_c64_t a11,
    int64_t det_q30,
#if MIMO_USE_SOLVE4_SCHUR_DET_SCALE
    int64_t det_scale_q45,
#elif MIMO_USE_SOLVE4_SCHUR_Q15_RECIP
    schur4_q15_recip_scale_t det_scale,
    int use_signed_shift,
#endif
    mimo_c64_t v0,
    mimo_c64_t v1)
{
    const mimo_c64_t num0 =
        c64_sub(schur4_mul_real_q15_to_q30(v0, a11.r),
                schur4_mul_q15_to_q30(a01, v1));
    const mimo_c64_t num1 = schur4_c64_add(
        schur4_mul_q15_to_q30(schur4_c64_neg(c64_conj(a01)), v0),
        schur4_mul_real_q15_to_q30(v1, a00.r));
    schur4_solve2_pair_t out;

#if MIMO_USE_SOLVE4_SCHUR_DET_SCALE
    out.row0 = schur4_scale_q30_to_q15(num0, det_scale_q45);
    out.row1 = schur4_scale_q30_to_q15(num1, det_scale_q45);
#elif MIMO_USE_SOLVE4_SCHUR_Q15_RECIP
    (void)det_q30;
    out.row0 = schur4_scale_q30_to_q15_recip(num0, det_scale,
                                             use_signed_shift);
    out.row1 = schur4_scale_q30_to_q15_recip(num1, det_scale,
                                             use_signed_shift);
#else
    out.row0 = schur4_div_q30_to_q15(num0, det_q30);
    out.row1 = schur4_div_q30_to_q15(num1, det_q30);
#endif
    return out;
}
#endif

static inline mimo_c64_t schur4_inner_eh_w(mimo_c64_t e0,
                                           mimo_c64_t e1,
                                           mimo_c64_t w0,
                                           mimo_c64_t w1)
{
#if MIMO_USE_MMSE4_SCHUR_INNER_Q30_ONCE
    const int64_t re = e0.r * w0.r + e0.i * w0.i +
                       e1.r * w1.r + e1.i * w1.i;
    const int64_t im = e0.r * w0.i - e0.i * w0.r +
                       e1.r * w1.i - e1.i * w1.r;
    return c64_make(q15_round_shift_i64(re), q15_round_shift_i64(im));
#else
    return schur4_c64_add(c64_mul_q15(c64_conj(e0), w0),
                          c64_mul_q15(c64_conj(e1), w1));
#endif
}

#if MIMO_USE_SCALAR4_SCHUR_TOP_Q30_ONCE
static MIMO_ALWAYS_INLINE mimo_c64_t schur4_top_submul2_q30_once(
    mimo_c64_t acc,
    mimo_c64_t a0,
    mimo_c64_t b0,
    mimo_c64_t a1,
    mimo_c64_t b1)
{
    const int64_t re = a0.r * b0.r - a0.i * b0.i +
                       a1.r * b1.r - a1.i * b1.i;
    const int64_t im = a0.r * b0.i + a0.i * b0.r +
                       a1.r * b1.i + a1.i * b1.r;

    return c64_make(acc.r - q15_round_shift_i64(re),
                    acc.i - q15_round_shift_i64(im));
}
#endif

static MIMO_ALWAYS_INLINE void schur4_solve_lane_q15(mimo_c64_t a00,
                                                     mimo_c64_t a01,
                                                     mimo_c64_t a02,
                                                     mimo_c64_t a03,
                                                     mimo_c64_t a11,
                                                     mimo_c64_t a12,
                                                     mimo_c64_t a13,
                                                     mimo_c64_t a22,
                                                     mimo_c64_t a23,
                                                     mimo_c64_t a33,
                                                     mimo_c64_t b0,
                                                     mimo_c64_t b1,
                                                     mimo_c64_t b2,
                                                     mimo_c64_t b3,
                                                     int mmse,
                                                     mimo_c16_t *restrict x)
{
    if (mmse) {
        a00.r += MIMO_MMSE_LAMBDA_Q15;
        a11.r += MIMO_MMSE_LAMBDA_Q15;
        a22.r += MIMO_MMSE_LAMBDA_Q15;
        a33.r += MIMO_MMSE_LAMBDA_Q15;
    }
    a00.i = 0;
    a11.i = 0;
    a22.i = 0;
    a33.i = 0;

    const int64_t b_det_q30 = schur4_det2_q30(a00, a01, a11);
#if MIMO_USE_SOLVE4_SCHUR_DET_SCALE
    const int64_t b_det_scale_q45 = schur4_det_scale_q45(b_det_q30);
#elif MIMO_USE_SOLVE4_SCHUR_Q15_RECIP
    const schur4_q15_recip_scale_t b_det_scale =
        schur4_q15_recip_scale_from_den(b_det_q30);
    const int schur4_use_signed_shift =
        (MIMO_USE_SOLVE4_SCHUR_Q15_RECIP == 4 ||
         MIMO_USE_SOLVE4_SCHUR_Q15_RECIP == 5) && !mmse;
#endif
#if MIMO_USE_SOLVE4_SCHUR_FUSED_INNER || MIMO_USE_SOLVE4_SCHUR_FUSED_INNER_MMSE
    if (!mmse || MIMO_USE_SOLVE4_SCHUR_FUSED_INNER_MMSE) {
        const schur4_solve2_num_pair_t w0_num =
            schur4_solve2_pair_num_q30(a00, a01, a11, a02, a12);
        const schur4_solve2_num_pair_t w1_num =
            schur4_solve2_pair_num_q30(a00, a01, a11, a03, a13);
        const schur4_solve2_num_pair_t y_num =
            schur4_solve2_pair_num_q30(a00, a01, a11, b0, b1);
        mimo_c64_t s00 = c64_sub(
            a22, schur4_inner_num_to_q15_recip(a02, a12, w0_num,
                                               b_det_scale,
                                               schur4_use_signed_shift));
        mimo_c64_t s01 = c64_sub(
            a23, schur4_inner_num_to_q15_recip(a02, a12, w1_num,
                                               b_det_scale,
                                               schur4_use_signed_shift));
        mimo_c64_t s11 = c64_sub(
            a33, schur4_inner_num_to_q15_recip(a03, a13, w1_num,
                                               b_det_scale,
                                               schur4_use_signed_shift));
        s00.i = 0;
        s11.i = 0;

        const mimo_c64_t r0 = c64_sub(
            b2, schur4_inner_num_to_q15_recip(a02, a12, y_num,
                                              b_det_scale,
                                              schur4_use_signed_shift));
        const mimo_c64_t r1 = c64_sub(
            b3, schur4_inner_num_to_q15_recip(a03, a13, y_num,
                                              b_det_scale,
                                              schur4_use_signed_shift));

        const int64_t s_det_q30 = schur4_det2_q30(s00, s01, s11);
        const schur4_q15_recip_scale_t s_det_scale =
            schur4_q15_recip_scale_from_den(s_det_q30);
        const mimo_c64_t x2 =
            schur4_solve2_vec_q15(s00, s01, s11, s_det_q30,
                                  s_det_scale, schur4_use_signed_shift,
                                  r0, r1, 0);
        const mimo_c64_t x3 =
            schur4_solve2_vec_q15(s00, s01, s11, s_det_q30,
                                  s_det_scale, schur4_use_signed_shift,
                                  r0, r1, 1);
        const mimo_c64_t top_rhs0 =
            schur4_fused_top_rhs_q15(b0, a02, a03, x2, x3);
        const mimo_c64_t top_rhs1 =
            schur4_fused_top_rhs_q15(b1, a12, a13, x2, x3);
        const mimo_c64_t top0 =
            schur4_solve2_vec_q15(a00, a01, a11, b_det_q30,
                                  b_det_scale, schur4_use_signed_shift,
                                  top_rhs0, top_rhs1, 0);
        const mimo_c64_t top1 =
            schur4_solve2_vec_q15(a00, a01, a11, b_det_q30,
                                  b_det_scale, schur4_use_signed_shift,
                                  top_rhs0, top_rhs1, 1);

        x[0] = mimo_make_c16(mimo_solve_store_i16((int32_t)top0.r),
                             mimo_solve_store_i16((int32_t)top0.i));
        x[1] = mimo_make_c16(mimo_solve_store_i16((int32_t)top1.r),
                             mimo_solve_store_i16((int32_t)top1.i));
        x[2] = mimo_make_c16(mimo_solve_store_i16((int32_t)x2.r),
                             mimo_solve_store_i16((int32_t)x2.i));
        x[3] = mimo_make_c16(mimo_solve_store_i16((int32_t)x3.r),
                             mimo_solve_store_i16((int32_t)x3.i));
        return;
    }
#endif
#if MIMO_USE_SOLVE4_SCHUR_BINV_Q15
    const mimo_c64_t b_inv00 =
        c64_make(schur4_div_real_q15(a11.r, b_det_q30), 0);
    const mimo_c64_t b_inv01 =
        c64_make(schur4_div_real_q15(-a01.r, b_det_q30),
                 schur4_div_real_q15(-a01.i, b_det_q30));
    const mimo_c64_t b_inv11 =
        c64_make(schur4_div_real_q15(a00.r, b_det_q30), 0);

    const mimo_c64_t w00 =
        schur4_inv2_apply_q15(b_inv00, b_inv01, b_inv11, a02, a12, 0);
    const mimo_c64_t w10 =
        schur4_inv2_apply_q15(b_inv00, b_inv01, b_inv11, a02, a12, 1);
    const mimo_c64_t w01 =
        schur4_inv2_apply_q15(b_inv00, b_inv01, b_inv11, a03, a13, 0);
    const mimo_c64_t w11 =
        schur4_inv2_apply_q15(b_inv00, b_inv01, b_inv11, a03, a13, 1);
    const mimo_c64_t y0 =
        schur4_inv2_apply_q15(b_inv00, b_inv01, b_inv11, b0, b1, 0);
    const mimo_c64_t y1 =
        schur4_inv2_apply_q15(b_inv00, b_inv01, b_inv11, b0, b1, 1);
#elif MIMO_USE_SOLVE4_SCHUR_PAIR2
    const schur4_solve2_pair_t w0_pair =
        schur4_solve2_pair_q15(a00, a01, a11, b_det_q30,
#if MIMO_USE_SOLVE4_SCHUR_DET_SCALE
                               b_det_scale_q45,
#elif MIMO_USE_SOLVE4_SCHUR_Q15_RECIP
                               b_det_scale,
                               schur4_use_signed_shift,
#endif
                               a02, a12);
    const mimo_c64_t w00 = w0_pair.row0;
    const mimo_c64_t w10 = w0_pair.row1;
    const schur4_solve2_pair_t w1_pair =
        schur4_solve2_pair_q15(a00, a01, a11, b_det_q30,
#if MIMO_USE_SOLVE4_SCHUR_DET_SCALE
                               b_det_scale_q45,
#elif MIMO_USE_SOLVE4_SCHUR_Q15_RECIP
                               b_det_scale,
                               schur4_use_signed_shift,
#endif
                               a03, a13);
    const mimo_c64_t w01 = w1_pair.row0;
    const mimo_c64_t w11 = w1_pair.row1;
    const schur4_solve2_pair_t y_pair =
        schur4_solve2_pair_q15(a00, a01, a11, b_det_q30,
#if MIMO_USE_SOLVE4_SCHUR_DET_SCALE
                               b_det_scale_q45,
#elif MIMO_USE_SOLVE4_SCHUR_Q15_RECIP
                               b_det_scale,
                               schur4_use_signed_shift,
#endif
                               b0, b1);
    const mimo_c64_t y0 = y_pair.row0;
    const mimo_c64_t y1 = y_pair.row1;
#else
    const mimo_c64_t w00 =
        schur4_solve2_vec_q15(a00, a01, a11, b_det_q30,
#if MIMO_USE_SOLVE4_SCHUR_DET_SCALE
                              b_det_scale_q45,
#elif MIMO_USE_SOLVE4_SCHUR_Q15_RECIP
                              b_det_scale,
                              schur4_use_signed_shift,
#endif
                              a02, a12, 0);
    const mimo_c64_t w10 =
        schur4_solve2_vec_q15(a00, a01, a11, b_det_q30,
#if MIMO_USE_SOLVE4_SCHUR_DET_SCALE
                              b_det_scale_q45,
#elif MIMO_USE_SOLVE4_SCHUR_Q15_RECIP
                              b_det_scale,
                              schur4_use_signed_shift,
#endif
                              a02, a12, 1);
    const mimo_c64_t w01 =
        schur4_solve2_vec_q15(a00, a01, a11, b_det_q30,
#if MIMO_USE_SOLVE4_SCHUR_DET_SCALE
                              b_det_scale_q45,
#elif MIMO_USE_SOLVE4_SCHUR_Q15_RECIP
                              b_det_scale,
                              schur4_use_signed_shift,
#endif
                              a03, a13, 0);
    const mimo_c64_t w11 =
        schur4_solve2_vec_q15(a00, a01, a11, b_det_q30,
#if MIMO_USE_SOLVE4_SCHUR_DET_SCALE
                              b_det_scale_q45,
#elif MIMO_USE_SOLVE4_SCHUR_Q15_RECIP
                              b_det_scale,
                              schur4_use_signed_shift,
#endif
                              a03, a13, 1);
    const mimo_c64_t y0 =
        schur4_solve2_vec_q15(a00, a01, a11, b_det_q30,
#if MIMO_USE_SOLVE4_SCHUR_DET_SCALE
                              b_det_scale_q45,
#elif MIMO_USE_SOLVE4_SCHUR_Q15_RECIP
                              b_det_scale,
                              schur4_use_signed_shift,
#endif
                              b0, b1, 0);
    const mimo_c64_t y1 =
        schur4_solve2_vec_q15(a00, a01, a11, b_det_q30,
#if MIMO_USE_SOLVE4_SCHUR_DET_SCALE
                              b_det_scale_q45,
#elif MIMO_USE_SOLVE4_SCHUR_Q15_RECIP
                              b_det_scale,
                              schur4_use_signed_shift,
#endif
                              b0, b1, 1);
#endif

    mimo_c64_t s00 = c64_sub(a22, schur4_inner_eh_w(a02, a12,
                                                    w00, w10));
    mimo_c64_t s01 = c64_sub(a23, schur4_inner_eh_w(a02, a12,
                                                    w01, w11));
    mimo_c64_t s11 = c64_sub(a33, schur4_inner_eh_w(a03, a13,
                                                    w01, w11));
    s00.i = 0;
    s11.i = 0;

    const mimo_c64_t r0 =
        c64_sub(b2, schur4_inner_eh_w(a02, a12, y0, y1));
    const mimo_c64_t r1 =
        c64_sub(b3, schur4_inner_eh_w(a03, a13, y0, y1));

    const int64_t s_det_q30 = schur4_det2_q30(s00, s01, s11);
#if MIMO_USE_SOLVE4_SCHUR_DET_SCALE
    const int64_t s_det_scale_q45 = schur4_det_scale_q45(s_det_q30);
#elif MIMO_USE_SOLVE4_SCHUR_Q15_RECIP
    const schur4_q15_recip_scale_t s_det_scale =
        schur4_q15_recip_scale_from_den(s_det_q30);
#endif
#if MIMO_USE_SOLVE4_SCHUR_PAIR2
    const schur4_solve2_pair_t x_tail =
        schur4_solve2_pair_q15(s00, s01, s11, s_det_q30,
#if MIMO_USE_SOLVE4_SCHUR_DET_SCALE
                               s_det_scale_q45,
#elif MIMO_USE_SOLVE4_SCHUR_Q15_RECIP
                               s_det_scale,
                               schur4_use_signed_shift,
#endif
                               r0, r1);
    const mimo_c64_t x2 = x_tail.row0;
    const mimo_c64_t x3 = x_tail.row1;
#else
    const mimo_c64_t x2 =
        schur4_solve2_vec_q15(s00, s01, s11, s_det_q30,
#if MIMO_USE_SOLVE4_SCHUR_DET_SCALE
                              s_det_scale_q45,
#elif MIMO_USE_SOLVE4_SCHUR_Q15_RECIP
                              s_det_scale,
                              schur4_use_signed_shift,
#endif
                              r0, r1, 0);
    const mimo_c64_t x3 =
        schur4_solve2_vec_q15(s00, s01, s11, s_det_q30,
#if MIMO_USE_SOLVE4_SCHUR_DET_SCALE
                              s_det_scale_q45,
#elif MIMO_USE_SOLVE4_SCHUR_Q15_RECIP
                              s_det_scale,
                              schur4_use_signed_shift,
#endif
                              r0, r1, 1);
#endif

#if MIMO_USE_SOLVE4_SCHUR_ZF_TOP_RESOLVE
    if (!mmse) {
        mimo_c64_t top_rhs0 = c64_sub(b0, c64_mul_q15(a02, x2));
        top_rhs0 = c64_sub(top_rhs0, c64_mul_q15(a03, x3));
        mimo_c64_t top_rhs1 = c64_sub(b1, c64_mul_q15(a12, x2));
        top_rhs1 = c64_sub(top_rhs1, c64_mul_q15(a13, x3));
        const mimo_c64_t top0 =
            schur4_solve2_vec_q15(a00, a01, a11, b_det_q30,
#if MIMO_USE_SOLVE4_SCHUR_DET_SCALE
                                  b_det_scale_q45,
#elif MIMO_USE_SOLVE4_SCHUR_Q15_RECIP
                                  b_det_scale,
                                  schur4_use_signed_shift,
#endif
                                  top_rhs0, top_rhs1, 0);
        const mimo_c64_t top1 =
            schur4_solve2_vec_q15(a00, a01, a11, b_det_q30,
#if MIMO_USE_SOLVE4_SCHUR_DET_SCALE
                                  b_det_scale_q45,
#elif MIMO_USE_SOLVE4_SCHUR_Q15_RECIP
                                  b_det_scale,
                                  schur4_use_signed_shift,
#endif
                                  top_rhs0, top_rhs1, 1);

        x[0] = mimo_make_c16(mimo_solve_store_i16((int32_t)top0.r),
                             mimo_solve_store_i16((int32_t)top0.i));
        x[1] = mimo_make_c16(mimo_solve_store_i16((int32_t)top1.r),
                             mimo_solve_store_i16((int32_t)top1.i));
        x[2] = mimo_make_c16(mimo_solve_store_i16((int32_t)x2.r),
                             mimo_solve_store_i16((int32_t)x2.i));
        x[3] = mimo_make_c16(mimo_solve_store_i16((int32_t)x3.r),
                             mimo_solve_store_i16((int32_t)x3.i));
        return;
    }
#endif

#if MIMO_USE_SCALAR4_SCHUR_TOP_Q30_ONCE
    const mimo_c64_t top0 =
        schur4_top_submul2_q30_once(y0, w00, x2, w01, x3);
    const mimo_c64_t top1 =
        schur4_top_submul2_q30_once(y1, w10, x2, w11, x3);
#else
    mimo_c64_t top0 = c64_sub(y0, c64_mul_q15(w00, x2));
    top0 = c64_sub(top0, c64_mul_q15(w01, x3));
    mimo_c64_t top1 = c64_sub(y1, c64_mul_q15(w10, x2));
    top1 = c64_sub(top1, c64_mul_q15(w11, x3));
#endif

    x[0] = mimo_make_c16(mimo_solve_store_i16((int32_t)top0.r),
                         mimo_solve_store_i16((int32_t)top0.i));
    x[1] = mimo_make_c16(mimo_solve_store_i16((int32_t)top1.r),
                         mimo_solve_store_i16((int32_t)top1.i));
    x[2] = mimo_make_c16(mimo_solve_store_i16((int32_t)x2.r),
                         mimo_solve_store_i16((int32_t)x2.i));
    x[3] = mimo_make_c16(mimo_solve_store_i16((int32_t)x3.r),
                         mimo_solve_store_i16((int32_t)x3.i));
}

#if MIMO_USE_SCALAR_MMSE4_FUSED_BUILD_SOLVE
#if MIMO_USE_SCALAR_MMSE4_FUSED_COMPACT_C32
#define MIMO_SCALAR_MMSE4_FUSED_VALUE_T mimo_c32_t
#define MIMO_SCALAR_MMSE4_BUILD_VALUE(V_) (V_)
#define MIMO_SCALAR_MMSE4_WIDEN_VALUE(V_) schur4_c64_from_c32(V_)
#else
#define MIMO_SCALAR_MMSE4_FUSED_VALUE_T mimo_c64_t
#define MIMO_SCALAR_MMSE4_BUILD_VALUE(V_) schur4_c64_from_c32(V_)
#define MIMO_SCALAR_MMSE4_WIDEN_VALUE(V_) (V_)
#endif

void mimo_eq_mmse4_scalar_fused_q15(size_t site_count,
                                    const mimo_c16_t *restrict h_row,
                                    const mimo_c16_t *restrict y,
                                    mimo_c16_t *restrict x)
{
    for (size_t site = 0; site < site_count; ++site) {
        const mimo_c16_t *restrict hs =
            &h_row[site * MIMO_N4 * MIMO_N4];
        const mimo_c16_t *restrict ys = &y[site * MIMO_N4];
        mimo_c16_t *restrict xs = &x[site * MIMO_N4];

        const MIMO_SCALAR_MMSE4_FUSED_VALUE_T b0 =
            MIMO_SCALAR_MMSE4_BUILD_VALUE(MIMO_DOT4_HY(hs, ys, 0));
        const MIMO_SCALAR_MMSE4_FUSED_VALUE_T b1 =
            MIMO_SCALAR_MMSE4_BUILD_VALUE(MIMO_DOT4_HY(hs, ys, 1));
        const MIMO_SCALAR_MMSE4_FUSED_VALUE_T b2 =
            MIMO_SCALAR_MMSE4_BUILD_VALUE(MIMO_DOT4_HY(hs, ys, 2));
        const MIMO_SCALAR_MMSE4_FUSED_VALUE_T b3 =
            MIMO_SCALAR_MMSE4_BUILD_VALUE(MIMO_DOT4_HY(hs, ys, 3));
        const MIMO_SCALAR_MMSE4_FUSED_VALUE_T a00 =
            MIMO_SCALAR_MMSE4_BUILD_VALUE(MIMO_DOT4_HH(hs, 0, 0));
        const MIMO_SCALAR_MMSE4_FUSED_VALUE_T a01 =
            MIMO_SCALAR_MMSE4_BUILD_VALUE(MIMO_DOT4_HH(hs, 0, 1));
        const MIMO_SCALAR_MMSE4_FUSED_VALUE_T a02 =
            MIMO_SCALAR_MMSE4_BUILD_VALUE(MIMO_DOT4_HH(hs, 0, 2));
        const MIMO_SCALAR_MMSE4_FUSED_VALUE_T a03 =
            MIMO_SCALAR_MMSE4_BUILD_VALUE(MIMO_DOT4_HH(hs, 0, 3));
        const MIMO_SCALAR_MMSE4_FUSED_VALUE_T a11 =
            MIMO_SCALAR_MMSE4_BUILD_VALUE(MIMO_DOT4_HH(hs, 1, 1));
        const MIMO_SCALAR_MMSE4_FUSED_VALUE_T a12 =
            MIMO_SCALAR_MMSE4_BUILD_VALUE(MIMO_DOT4_HH(hs, 1, 2));
        const MIMO_SCALAR_MMSE4_FUSED_VALUE_T a13 =
            MIMO_SCALAR_MMSE4_BUILD_VALUE(MIMO_DOT4_HH(hs, 1, 3));
        const MIMO_SCALAR_MMSE4_FUSED_VALUE_T a22 =
            MIMO_SCALAR_MMSE4_BUILD_VALUE(MIMO_DOT4_HH(hs, 2, 2));
        const MIMO_SCALAR_MMSE4_FUSED_VALUE_T a23 =
            MIMO_SCALAR_MMSE4_BUILD_VALUE(MIMO_DOT4_HH(hs, 2, 3));
        const MIMO_SCALAR_MMSE4_FUSED_VALUE_T a33 =
            MIMO_SCALAR_MMSE4_BUILD_VALUE(MIMO_DOT4_HH(hs, 3, 3));

        schur4_solve_lane_q15(
            MIMO_SCALAR_MMSE4_WIDEN_VALUE(a00),
            MIMO_SCALAR_MMSE4_WIDEN_VALUE(a01),
            MIMO_SCALAR_MMSE4_WIDEN_VALUE(a02),
            MIMO_SCALAR_MMSE4_WIDEN_VALUE(a03),
            MIMO_SCALAR_MMSE4_WIDEN_VALUE(a11),
            MIMO_SCALAR_MMSE4_WIDEN_VALUE(a12),
            MIMO_SCALAR_MMSE4_WIDEN_VALUE(a13),
            MIMO_SCALAR_MMSE4_WIDEN_VALUE(a22),
            MIMO_SCALAR_MMSE4_WIDEN_VALUE(a23),
            MIMO_SCALAR_MMSE4_WIDEN_VALUE(a33),
            MIMO_SCALAR_MMSE4_WIDEN_VALUE(b0),
            MIMO_SCALAR_MMSE4_WIDEN_VALUE(b1),
            MIMO_SCALAR_MMSE4_WIDEN_VALUE(b2),
            MIMO_SCALAR_MMSE4_WIDEN_VALUE(b3), 1, xs);
    }
}

#undef MIMO_SCALAR_MMSE4_FUSED_VALUE_T
#undef MIMO_SCALAR_MMSE4_BUILD_VALUE
#undef MIMO_SCALAR_MMSE4_WIDEN_VALUE
#endif

#if MIMO_USE_SOLVE4_SCHUR_ZF_EXPLICIT || \
    MIMO_USE_SOLVE4_SCHUR_ZF_RELOAD_MICRO || \
    MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER
#if MIMO_USE_SOLVE4_SCHUR_ZF_EXPLICIT == 3 || \
    MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER
typedef int16_t schur4_zf_state_t;
#define SCHUR4_ZF_CAST_STATE(X_) ((int16_t)(X_))
#elif MIMO_USE_SOLVE4_SCHUR_ZF_EXPLICIT == 2 || \
      MIMO_USE_SOLVE4_SCHUR_ZF_RELOAD_MICRO
typedef int32_t schur4_zf_state_t;
#define SCHUR4_ZF_CAST_STATE(X_) ((int32_t)(X_))
#else
typedef int64_t schur4_zf_state_t;
#define SCHUR4_ZF_CAST_STATE(X_) (X_)
#endif
#define SCHUR4_ZF_WIDE(X_) ((int64_t)(X_))
#if MIMO_USE_SOLVE4_SCHUR_ZF_I32_NUM
typedef int32_t schur4_zf_num_t;
#define SCHUR4_ZF_NUM_CAST(X_) ((int32_t)(X_))
#else
typedef int64_t schur4_zf_num_t;
#define SCHUR4_ZF_NUM_CAST(X_) SCHUR4_ZF_WIDE(X_)
#endif
#define SCHUR4_ZF_MUL_Q30(A_, B_) \
    ((schur4_zf_num_t)(SCHUR4_ZF_NUM_CAST(A_) * SCHUR4_ZF_NUM_CAST(B_)))
#define SCHUR4_ZF_DET2_Q30(A00R_, A01R_, A01I_, A11R_) \
    ((schur4_zf_num_t)( \
        SCHUR4_ZF_MUL_Q30((A00R_), (A11R_)) - \
        (SCHUR4_ZF_MUL_Q30((A01R_), (A01R_)) + \
         SCHUR4_ZF_MUL_Q30((A01I_), (A01I_)))))

#define SCHUR4_ZF_SCALE_Q15(NUM_, SCALE_) \
    schur4_div_q30_scalar_to_q15_recip((NUM_), (SCALE_), 1)

#if MIMO_USE_SOLVE4_SCHUR_ZF_INLINE_HELPER
#define SCHUR4_ZF_HELPER_ATTR MIMO_ALWAYS_INLINE
#else
#define SCHUR4_ZF_HELPER_ATTR MIMO_NOINLINE
#endif

#define SCHUR4_ZF_SOLVE2_EXPLICIT(A00R_, A01R_, A01I_, A11R_, SCALE_, \
                                  V0R_, V0I_, V1R_, V1I_, \
                                  OUT0R_, OUT0I_, OUT1R_, OUT1I_) \
    do { \
        const schur4_zf_num_t schur4_num0r__ = \
            SCHUR4_ZF_MUL_Q30((V0R_), (A11R_)) - \
            (SCHUR4_ZF_MUL_Q30((A01R_), (V1R_)) - \
             SCHUR4_ZF_MUL_Q30((A01I_), (V1I_))); \
        const schur4_zf_num_t schur4_num0i__ = \
            SCHUR4_ZF_MUL_Q30((V0I_), (A11R_)) - \
            (SCHUR4_ZF_MUL_Q30((A01R_), (V1I_)) + \
             SCHUR4_ZF_MUL_Q30((A01I_), (V1R_))); \
        const schur4_zf_num_t schur4_num1r__ = \
            -SCHUR4_ZF_MUL_Q30((A01R_), (V0R_)) - \
            SCHUR4_ZF_MUL_Q30((A01I_), (V0I_)) + \
            SCHUR4_ZF_MUL_Q30((V1R_), (A00R_)); \
        const schur4_zf_num_t schur4_num1i__ = \
            -SCHUR4_ZF_MUL_Q30((A01R_), (V0I_)) + \
            SCHUR4_ZF_MUL_Q30((A01I_), (V0R_)) + \
            SCHUR4_ZF_MUL_Q30((V1I_), (A00R_)); \
        (OUT0R_) = SCHUR4_ZF_CAST_STATE( \
            SCHUR4_ZF_SCALE_Q15(schur4_num0r__, (SCALE_))); \
        (OUT0I_) = SCHUR4_ZF_CAST_STATE( \
            SCHUR4_ZF_SCALE_Q15(schur4_num0i__, (SCALE_))); \
        (OUT1R_) = SCHUR4_ZF_CAST_STATE( \
            SCHUR4_ZF_SCALE_Q15(schur4_num1r__, (SCALE_))); \
        (OUT1I_) = SCHUR4_ZF_CAST_STATE( \
            SCHUR4_ZF_SCALE_Q15(schur4_num1i__, (SCALE_))); \
    } while (0)

#define SCHUR4_ZF_INNER_EXPLICIT(E0R_, E0I_, E1R_, E1I_, \
                                 W0R_, W0I_, W1R_, W1I_, \
                                 OUTR_, OUTI_) \
    do { \
        const int64_t schur4_p0r__ = \
            q15_round_shift_i64(SCHUR4_ZF_WIDE(E0R_) * \
                                SCHUR4_ZF_WIDE(W0R_) + \
                                SCHUR4_ZF_WIDE(E0I_) * \
                                SCHUR4_ZF_WIDE(W0I_)); \
        const int64_t schur4_p0i__ = \
            q15_round_shift_i64(SCHUR4_ZF_WIDE(E0R_) * \
                                SCHUR4_ZF_WIDE(W0I_) - \
                                SCHUR4_ZF_WIDE(E0I_) * \
                                SCHUR4_ZF_WIDE(W0R_)); \
        const int64_t schur4_p1r__ = \
            q15_round_shift_i64(SCHUR4_ZF_WIDE(E1R_) * \
                                SCHUR4_ZF_WIDE(W1R_) + \
                                SCHUR4_ZF_WIDE(E1I_) * \
                                SCHUR4_ZF_WIDE(W1I_)); \
        const int64_t schur4_p1i__ = \
            q15_round_shift_i64(SCHUR4_ZF_WIDE(E1R_) * \
                                SCHUR4_ZF_WIDE(W1I_) - \
                                SCHUR4_ZF_WIDE(E1I_) * \
                                SCHUR4_ZF_WIDE(W1R_)); \
        (OUTR_) = SCHUR4_ZF_CAST_STATE(schur4_p0r__ + schur4_p1r__); \
        (OUTI_) = SCHUR4_ZF_CAST_STATE(schur4_p0i__ + schur4_p1i__); \
    } while (0)

#define SCHUR4_ZF_SUB_MUL_Q15_EXPLICIT(ACCR_, ACCI_, \
                                       AR_, AI_, BR_, BI_) \
    do { \
        const int64_t schur4_mr__ = \
            q15_round_shift_i64(SCHUR4_ZF_WIDE(AR_) * \
                                SCHUR4_ZF_WIDE(BR_) - \
                                SCHUR4_ZF_WIDE(AI_) * \
                                SCHUR4_ZF_WIDE(BI_)); \
        const int64_t schur4_mi__ = \
            q15_round_shift_i64(SCHUR4_ZF_WIDE(AR_) * \
                                SCHUR4_ZF_WIDE(BI_) + \
                                SCHUR4_ZF_WIDE(AI_) * \
                                SCHUR4_ZF_WIDE(BR_)); \
        (ACCR_) = SCHUR4_ZF_CAST_STATE(SCHUR4_ZF_WIDE(ACCR_) - \
                                       schur4_mr__); \
        (ACCI_) = SCHUR4_ZF_CAST_STATE(SCHUR4_ZF_WIDE(ACCI_) - \
                                       schur4_mi__); \
    } while (0)

#if MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_MODEL || \
    MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_MODEL || \
    MIMO_USE_SOLVE4_SCHUR_ZF_PIVOTSCALE_MODEL || \
    MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_ASM
static MIMO_ALWAYS_INLINE void schur4_zf_solve2h1_q15_primitive_model(
    schur4_zf_state_t a00r,
    schur4_zf_state_t a01r,
    schur4_zf_state_t a01i,
    schur4_zf_state_t a11r,
    schur4_zf_state_t v0r,
    schur4_zf_state_t v0i,
    schur4_zf_state_t v1r,
    schur4_zf_state_t v1i,
    schur4_zf_state_t *restrict out0r,
    schur4_zf_state_t *restrict out0i,
    schur4_zf_state_t *restrict out1r,
    schur4_zf_state_t *restrict out1i)
{
    const schur4_zf_num_t det_q30 =
        SCHUR4_ZF_DET2_Q30(a00r, a01r, a01i, a11r);
    const schur4_q15_recip_scale_t det_scale =
        schur4_q15_recip_scale_from_den(det_q30);

    SCHUR4_ZF_SOLVE2_EXPLICIT(a00r, a01r, a01i, a11r, det_scale,
                              v0r, v0i, v1r, v1i,
                              *out0r, *out0i, *out1r, *out1i);
}

static MIMO_ALWAYS_INLINE void schur4_zf_solve2h3_q15_primitive_model(
    schur4_zf_state_t a00r,
    schur4_zf_state_t a01r,
    schur4_zf_state_t a01i,
    schur4_zf_state_t a11r,
    schur4_zf_state_t v00r,
    schur4_zf_state_t v00i,
    schur4_zf_state_t v01r,
    schur4_zf_state_t v01i,
    schur4_zf_state_t v10r,
    schur4_zf_state_t v10i,
    schur4_zf_state_t v11r,
    schur4_zf_state_t v11i,
    schur4_zf_state_t v20r,
    schur4_zf_state_t v20i,
    schur4_zf_state_t v21r,
    schur4_zf_state_t v21i,
    schur4_zf_state_t *restrict out00r,
    schur4_zf_state_t *restrict out00i,
    schur4_zf_state_t *restrict out01r,
    schur4_zf_state_t *restrict out01i,
    schur4_zf_state_t *restrict out10r,
    schur4_zf_state_t *restrict out10i,
    schur4_zf_state_t *restrict out11r,
    schur4_zf_state_t *restrict out11i,
    schur4_zf_state_t *restrict out20r,
    schur4_zf_state_t *restrict out20i,
    schur4_zf_state_t *restrict out21r,
    schur4_zf_state_t *restrict out21i)
{
    const schur4_zf_num_t det_q30 =
        SCHUR4_ZF_DET2_Q30(a00r, a01r, a01i, a11r);
    const schur4_q15_recip_scale_t det_scale =
        schur4_q15_recip_scale_from_den(det_q30);

    SCHUR4_ZF_SOLVE2_EXPLICIT(a00r, a01r, a01i, a11r, det_scale,
                              v00r, v00i, v01r, v01i,
                              *out00r, *out00i, *out01r, *out01i);
    SCHUR4_ZF_SOLVE2_EXPLICIT(a00r, a01r, a01i, a11r, det_scale,
                              v10r, v10i, v11r, v11i,
                              *out10r, *out10i, *out11r, *out11i);
    SCHUR4_ZF_SOLVE2_EXPLICIT(a00r, a01r, a01i, a11r, det_scale,
                              v20r, v20i, v21r, v21i,
                              *out20r, *out20i, *out21r, *out21i);
}

static MIMO_ALWAYS_INLINE void schur4_zf_solve2h1_group_q15_primitive_model(
    const mimo_c16_t a00[restrict MIMO_SITE4_LANES],
    const mimo_c16_t a01[restrict MIMO_SITE4_LANES],
    const mimo_c16_t a11[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v0[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v1[restrict MIMO_SITE4_LANES],
    mimo_c16_t out0[restrict MIMO_SITE4_LANES],
    mimo_c16_t out1[restrict MIMO_SITE4_LANES])
{
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        schur4_zf_state_t out0r, out0i, out1r, out1i;
        schur4_zf_solve2h1_q15_primitive_model(
            (schur4_zf_state_t)a00[lane].r,
            (schur4_zf_state_t)a01[lane].r,
            (schur4_zf_state_t)a01[lane].i,
            (schur4_zf_state_t)a11[lane].r,
            (schur4_zf_state_t)v0[lane].r,
            (schur4_zf_state_t)v0[lane].i,
            (schur4_zf_state_t)v1[lane].r,
            (schur4_zf_state_t)v1[lane].i,
            &out0r, &out0i, &out1r, &out1i);
        out0[lane] = mimo_make_c16((int16_t)out0r, (int16_t)out0i);
        out1[lane] = mimo_make_c16((int16_t)out1r, (int16_t)out1i);
    }
}

static MIMO_ALWAYS_INLINE void schur4_zf_solve2h3_group_q15_primitive_model(
    const mimo_c16_t a00[restrict MIMO_SITE4_LANES],
    const mimo_c16_t a01[restrict MIMO_SITE4_LANES],
    const mimo_c16_t a11[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v00[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v01[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v10[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v11[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v20[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v21[restrict MIMO_SITE4_LANES],
    mimo_c16_t out00[restrict MIMO_SITE4_LANES],
    mimo_c16_t out01[restrict MIMO_SITE4_LANES],
    mimo_c16_t out10[restrict MIMO_SITE4_LANES],
    mimo_c16_t out11[restrict MIMO_SITE4_LANES],
    mimo_c16_t out20[restrict MIMO_SITE4_LANES],
    mimo_c16_t out21[restrict MIMO_SITE4_LANES])
{
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        schur4_zf_state_t out00r, out00i, out01r, out01i;
        schur4_zf_state_t out10r, out10i, out11r, out11i;
        schur4_zf_state_t out20r, out20i, out21r, out21i;
        schur4_zf_solve2h3_q15_primitive_model(
            (schur4_zf_state_t)a00[lane].r,
            (schur4_zf_state_t)a01[lane].r,
            (schur4_zf_state_t)a01[lane].i,
            (schur4_zf_state_t)a11[lane].r,
            (schur4_zf_state_t)v00[lane].r,
            (schur4_zf_state_t)v00[lane].i,
            (schur4_zf_state_t)v01[lane].r,
            (schur4_zf_state_t)v01[lane].i,
            (schur4_zf_state_t)v10[lane].r,
            (schur4_zf_state_t)v10[lane].i,
            (schur4_zf_state_t)v11[lane].r,
            (schur4_zf_state_t)v11[lane].i,
            (schur4_zf_state_t)v20[lane].r,
            (schur4_zf_state_t)v20[lane].i,
            (schur4_zf_state_t)v21[lane].r,
            (schur4_zf_state_t)v21[lane].i,
            &out00r, &out00i, &out01r, &out01i,
            &out10r, &out10i, &out11r, &out11i,
            &out20r, &out20i, &out21r, &out21i);
        out00[lane] = mimo_make_c16((int16_t)out00r, (int16_t)out00i);
        out01[lane] = mimo_make_c16((int16_t)out01r, (int16_t)out01i);
        out10[lane] = mimo_make_c16((int16_t)out10r, (int16_t)out10i);
        out11[lane] = mimo_make_c16((int16_t)out11r, (int16_t)out11i);
        out20[lane] = mimo_make_c16((int16_t)out20r, (int16_t)out20i);
        out21[lane] = mimo_make_c16((int16_t)out21r, (int16_t)out21i);
    }
}

static MIMO_ALWAYS_INLINE void schur4_zf_solve2h1_regs_q15_primitive_model(
    const mimo_c16_t vs1[3][MIMO_SITE4_LANES],
    const mimo_c16_t vs2[2][MIMO_SITE4_LANES],
    mimo_c16_t vd[2][MIMO_SITE4_LANES])
{
    schur4_zf_solve2h1_group_q15_primitive_model(
        vs1[0], vs1[1], vs1[2], vs2[0], vs2[1], vd[0], vd[1]);
}

static MIMO_ALWAYS_INLINE void schur4_zf_solve2h3_regs_q15_primitive_model(
    const mimo_c16_t vs1[3][MIMO_SITE4_LANES],
    const mimo_c16_t vs2[6][MIMO_SITE4_LANES],
    mimo_c16_t vd[6][MIMO_SITE4_LANES])
{
    schur4_zf_solve2h3_group_q15_primitive_model(
        vs1[0], vs1[1], vs1[2],
        vs2[0], vs2[1], vs2[2], vs2[3], vs2[4], vs2[5],
        vd[0], vd[1], vd[2], vd[3], vd[4], vd[5]);
}

static MIMO_ALWAYS_INLINE void schur4_zf_vsolve2h2_pair_regs_q15_primitive_model(
    const int16_t vs1[restrict 8],
    const int16_t vs2[restrict 8],
    int16_t vd[restrict 8])
{
#if MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_ASM
#error "MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_ASM is disabled on the fixed hardware platform"
#else
    for (int pair_lane = 0; pair_lane < 2; ++pair_lane) {
        const int off = pair_lane * 4;
        schur4_zf_state_t out0r, out0i, out1r, out1i;

        schur4_zf_solve2h1_q15_primitive_model(
            (schur4_zf_state_t)vs1[off + 0],
            (schur4_zf_state_t)vs1[off + 1],
            (schur4_zf_state_t)vs1[off + 2],
            (schur4_zf_state_t)vs1[off + 3],
            (schur4_zf_state_t)vs2[off + 0],
            (schur4_zf_state_t)vs2[off + 1],
            (schur4_zf_state_t)vs2[off + 2],
            (schur4_zf_state_t)vs2[off + 3],
            &out0r, &out0i, &out1r, &out1i);

        vd[off + 0] = (int16_t)out0r;
        vd[off + 1] = (int16_t)out0i;
        vd[off + 2] = (int16_t)out1r;
        vd[off + 3] = (int16_t)out1i;
    }
#endif
}

static MIMO_ALWAYS_INLINE void schur4_zf_vsolve2h2_group_q15_primitive_model(
    const mimo_c16_t a00[restrict MIMO_SITE4_LANES],
    const mimo_c16_t a01[restrict MIMO_SITE4_LANES],
    const mimo_c16_t a11[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v0[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v1[restrict MIMO_SITE4_LANES],
    mimo_c16_t out0[restrict MIMO_SITE4_LANES],
    mimo_c16_t out1[restrict MIMO_SITE4_LANES])
{
    for (int lane_base = 0; lane_base < MIMO_SITE4_LANES; lane_base += 2) {
        int16_t vs1[8] MIMO_ALIGN64;
        int16_t vs2[8] MIMO_ALIGN64;
        int16_t vd[8] MIMO_ALIGN64;

        for (int pair_lane = 0; pair_lane < 2; ++pair_lane) {
            const int lane = lane_base + pair_lane;
            const int off = pair_lane * 4;

            vs1[off + 0] = a00[lane].r;
            vs1[off + 1] = a01[lane].r;
            vs1[off + 2] = a01[lane].i;
            vs1[off + 3] = a11[lane].r;
            vs2[off + 0] = v0[lane].r;
            vs2[off + 1] = v0[lane].i;
            vs2[off + 2] = v1[lane].r;
            vs2[off + 3] = v1[lane].i;
        }

        schur4_zf_vsolve2h2_pair_regs_q15_primitive_model(vs1, vs2, vd);

        for (int pair_lane = 0; pair_lane < 2; ++pair_lane) {
            const int lane = lane_base + pair_lane;
            const int off = pair_lane * 4;

            out0[lane] = mimo_make_c16(vd[off + 0], vd[off + 1]);
            out1[lane] = mimo_make_c16(vd[off + 2], vd[off + 3]);
        }
    }
}

static MIMO_ALWAYS_INLINE void schur4_zf_vsolve2h2_3rhs_group_q15_primitive_model(
    const mimo_c16_t a00[restrict MIMO_SITE4_LANES],
    const mimo_c16_t a01[restrict MIMO_SITE4_LANES],
    const mimo_c16_t a11[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v00[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v01[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v10[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v11[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v20[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v21[restrict MIMO_SITE4_LANES],
    mimo_c16_t out00[restrict MIMO_SITE4_LANES],
    mimo_c16_t out01[restrict MIMO_SITE4_LANES],
    mimo_c16_t out10[restrict MIMO_SITE4_LANES],
    mimo_c16_t out11[restrict MIMO_SITE4_LANES],
    mimo_c16_t out20[restrict MIMO_SITE4_LANES],
    mimo_c16_t out21[restrict MIMO_SITE4_LANES])
{
    schur4_zf_vsolve2h2_group_q15_primitive_model(
        a00, a01, a11, v00, v01, out00, out01);
    schur4_zf_vsolve2h2_group_q15_primitive_model(
        a00, a01, a11, v10, v11, out10, out11);
    schur4_zf_vsolve2h2_group_q15_primitive_model(
        a00, a01, a11, v20, v21, out20, out21);
}

#if MIMO_CHECK_SOLVE2H_GROUP_LAYOUT || MIMO_CHECK_VSOLVE2H2_LAYOUT
static MIMO_ALWAYS_INLINE int schur4_zf_vec4_c16_eq(
    const mimo_c16_t a[restrict MIMO_SITE4_LANES],
    const mimo_c16_t b[restrict MIMO_SITE4_LANES])
{
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        if (a[lane].r != b[lane].r || a[lane].i != b[lane].i) {
            return 0;
        }
    }
    return 1;
}

static MIMO_ALWAYS_INLINE void schur4_zf_vec4_c16_copy(
    mimo_c16_t dst[restrict MIMO_SITE4_LANES],
    const mimo_c16_t src[restrict MIMO_SITE4_LANES])
{
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        dst[lane] = src[lane];
    }
}

static MIMO_ALWAYS_INLINE void schur4_zf_vec4_c16_poison(
    mimo_c16_t dst[restrict MIMO_SITE4_LANES])
{
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        dst[lane] = mimo_make_c16(32767, -32768);
    }
}
#endif

#if MIMO_CHECK_SOLVE2H_GROUP_LAYOUT
static MIMO_ALWAYS_INLINE void schur4_zf_solve2h1_group_layout_checked_q15(
    const mimo_c16_t a00[restrict MIMO_SITE4_LANES],
    const mimo_c16_t a01[restrict MIMO_SITE4_LANES],
    const mimo_c16_t a11[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v0[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v1[restrict MIMO_SITE4_LANES],
    mimo_c16_t out0[restrict MIMO_SITE4_LANES],
    mimo_c16_t out1[restrict MIMO_SITE4_LANES])
{
    mimo_c16_t vs1[3][MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t vs2[2][MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t vd[2][MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t ref0[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t ref1[MIMO_SITE4_LANES] MIMO_ALIGN64;

    schur4_zf_vec4_c16_copy(vs1[0], a00);
    schur4_zf_vec4_c16_copy(vs1[1], a01);
    schur4_zf_vec4_c16_copy(vs1[2], a11);
    schur4_zf_vec4_c16_copy(vs2[0], v0);
    schur4_zf_vec4_c16_copy(vs2[1], v1);

    schur4_zf_solve2h1_regs_q15_primitive_model(vs1, vs2, vd);
    schur4_zf_solve2h1_group_q15_primitive_model(
        a00, a01, a11, v0, v1, ref0, ref1);

    if (!schur4_zf_vec4_c16_eq(vd[0], ref0) ||
        !schur4_zf_vec4_c16_eq(vd[1], ref1)) {
        schur4_zf_vec4_c16_poison(vd[0]);
        schur4_zf_vec4_c16_poison(vd[1]);
    }
    schur4_zf_vec4_c16_copy(out0, vd[0]);
    schur4_zf_vec4_c16_copy(out1, vd[1]);
}

static MIMO_ALWAYS_INLINE void schur4_zf_solve2h3_group_layout_checked_q15(
    const mimo_c16_t a00[restrict MIMO_SITE4_LANES],
    const mimo_c16_t a01[restrict MIMO_SITE4_LANES],
    const mimo_c16_t a11[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v00[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v01[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v10[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v11[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v20[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v21[restrict MIMO_SITE4_LANES],
    mimo_c16_t out00[restrict MIMO_SITE4_LANES],
    mimo_c16_t out01[restrict MIMO_SITE4_LANES],
    mimo_c16_t out10[restrict MIMO_SITE4_LANES],
    mimo_c16_t out11[restrict MIMO_SITE4_LANES],
    mimo_c16_t out20[restrict MIMO_SITE4_LANES],
    mimo_c16_t out21[restrict MIMO_SITE4_LANES])
{
    mimo_c16_t vs1[3][MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t vs2[6][MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t vd[6][MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t ref00[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t ref01[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t ref10[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t ref11[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t ref20[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t ref21[MIMO_SITE4_LANES] MIMO_ALIGN64;

    schur4_zf_vec4_c16_copy(vs1[0], a00);
    schur4_zf_vec4_c16_copy(vs1[1], a01);
    schur4_zf_vec4_c16_copy(vs1[2], a11);
    schur4_zf_vec4_c16_copy(vs2[0], v00);
    schur4_zf_vec4_c16_copy(vs2[1], v01);
    schur4_zf_vec4_c16_copy(vs2[2], v10);
    schur4_zf_vec4_c16_copy(vs2[3], v11);
    schur4_zf_vec4_c16_copy(vs2[4], v20);
    schur4_zf_vec4_c16_copy(vs2[5], v21);

    schur4_zf_solve2h3_regs_q15_primitive_model(vs1, vs2, vd);
    schur4_zf_solve2h3_group_q15_primitive_model(
        a00, a01, a11, v00, v01, v10, v11, v20, v21,
        ref00, ref01, ref10, ref11, ref20, ref21);

    if (!schur4_zf_vec4_c16_eq(vd[0], ref00) ||
        !schur4_zf_vec4_c16_eq(vd[1], ref01) ||
        !schur4_zf_vec4_c16_eq(vd[2], ref10) ||
        !schur4_zf_vec4_c16_eq(vd[3], ref11) ||
        !schur4_zf_vec4_c16_eq(vd[4], ref20) ||
        !schur4_zf_vec4_c16_eq(vd[5], ref21)) {
        schur4_zf_vec4_c16_poison(vd[0]);
        schur4_zf_vec4_c16_poison(vd[1]);
        schur4_zf_vec4_c16_poison(vd[2]);
        schur4_zf_vec4_c16_poison(vd[3]);
        schur4_zf_vec4_c16_poison(vd[4]);
        schur4_zf_vec4_c16_poison(vd[5]);
    }
    schur4_zf_vec4_c16_copy(out00, vd[0]);
    schur4_zf_vec4_c16_copy(out01, vd[1]);
    schur4_zf_vec4_c16_copy(out10, vd[2]);
    schur4_zf_vec4_c16_copy(out11, vd[3]);
    schur4_zf_vec4_c16_copy(out20, vd[4]);
    schur4_zf_vec4_c16_copy(out21, vd[5]);
}
#endif

#if MIMO_CHECK_VSOLVE2H2_LAYOUT
static MIMO_ALWAYS_INLINE void schur4_zf_vsolve2h2_group_layout_checked_q15(
    const mimo_c16_t a00[restrict MIMO_SITE4_LANES],
    const mimo_c16_t a01[restrict MIMO_SITE4_LANES],
    const mimo_c16_t a11[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v0[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v1[restrict MIMO_SITE4_LANES],
    mimo_c16_t out0[restrict MIMO_SITE4_LANES],
    mimo_c16_t out1[restrict MIMO_SITE4_LANES])
{
    mimo_c16_t ref0[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t ref1[MIMO_SITE4_LANES] MIMO_ALIGN64;

    schur4_zf_vsolve2h2_group_q15_primitive_model(
        a00, a01, a11, v0, v1, out0, out1);
    schur4_zf_solve2h1_group_q15_primitive_model(
        a00, a01, a11, v0, v1, ref0, ref1);

    if (!schur4_zf_vec4_c16_eq(out0, ref0) ||
        !schur4_zf_vec4_c16_eq(out1, ref1)) {
        schur4_zf_vec4_c16_poison(out0);
        schur4_zf_vec4_c16_poison(out1);
    }
}

static MIMO_ALWAYS_INLINE void schur4_zf_vsolve2h2_3rhs_group_layout_checked_q15(
    const mimo_c16_t a00[restrict MIMO_SITE4_LANES],
    const mimo_c16_t a01[restrict MIMO_SITE4_LANES],
    const mimo_c16_t a11[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v00[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v01[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v10[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v11[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v20[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v21[restrict MIMO_SITE4_LANES],
    mimo_c16_t out00[restrict MIMO_SITE4_LANES],
    mimo_c16_t out01[restrict MIMO_SITE4_LANES],
    mimo_c16_t out10[restrict MIMO_SITE4_LANES],
    mimo_c16_t out11[restrict MIMO_SITE4_LANES],
    mimo_c16_t out20[restrict MIMO_SITE4_LANES],
    mimo_c16_t out21[restrict MIMO_SITE4_LANES])
{
    mimo_c16_t ref00[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t ref01[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t ref10[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t ref11[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t ref20[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t ref21[MIMO_SITE4_LANES] MIMO_ALIGN64;

    schur4_zf_vsolve2h2_3rhs_group_q15_primitive_model(
        a00, a01, a11, v00, v01, v10, v11, v20, v21,
        out00, out01, out10, out11, out20, out21);
    schur4_zf_solve2h3_group_q15_primitive_model(
        a00, a01, a11, v00, v01, v10, v11, v20, v21,
        ref00, ref01, ref10, ref11, ref20, ref21);

    if (!schur4_zf_vec4_c16_eq(out00, ref00) ||
        !schur4_zf_vec4_c16_eq(out01, ref01) ||
        !schur4_zf_vec4_c16_eq(out10, ref10) ||
        !schur4_zf_vec4_c16_eq(out11, ref11) ||
        !schur4_zf_vec4_c16_eq(out20, ref20) ||
        !schur4_zf_vec4_c16_eq(out21, ref21)) {
        schur4_zf_vec4_c16_poison(out00);
        schur4_zf_vec4_c16_poison(out01);
        schur4_zf_vec4_c16_poison(out10);
        schur4_zf_vec4_c16_poison(out11);
        schur4_zf_vec4_c16_poison(out20);
        schur4_zf_vec4_c16_poison(out21);
    }
}
#endif
#endif

static MIMO_ALWAYS_INLINE void schur4_solve_lane_zf_explicit_q15(
    schur4_zf_state_t a00r,
    schur4_zf_state_t a01r,
    schur4_zf_state_t a01i,
    schur4_zf_state_t a02r,
    schur4_zf_state_t a02i,
    schur4_zf_state_t a03r,
    schur4_zf_state_t a03i,
    schur4_zf_state_t a11r,
    schur4_zf_state_t a12r,
    schur4_zf_state_t a12i,
    schur4_zf_state_t a13r,
    schur4_zf_state_t a13i,
    schur4_zf_state_t a22r,
    schur4_zf_state_t a23r,
    schur4_zf_state_t a23i,
    schur4_zf_state_t a33r,
    schur4_zf_state_t b0r,
    schur4_zf_state_t b0i,
    schur4_zf_state_t b1r,
    schur4_zf_state_t b1i,
    schur4_zf_state_t b2r,
    schur4_zf_state_t b2i,
    schur4_zf_state_t b3r,
    schur4_zf_state_t b3i,
    mimo_c16_t *restrict x)
{
    const schur4_zf_num_t b_det_q30 =
        SCHUR4_ZF_DET2_Q30(a00r, a01r, a01i, a11r);
    const schur4_q15_recip_scale_t b_det_scale =
        schur4_q15_recip_scale_from_den(b_det_q30);
    schur4_zf_state_t w00r, w00i, w10r, w10i;
    schur4_zf_state_t w01r, w01i, w11r, w11i;
    schur4_zf_state_t y0r, y0i, y1r, y1i;
    schur4_zf_state_t inner_r, inner_i;

    SCHUR4_ZF_SOLVE2_EXPLICIT(a00r, a01r, a01i, a11r, b_det_scale,
                              a02r, a02i, a12r, a12i,
                              w00r, w00i, w10r, w10i);
    SCHUR4_ZF_SOLVE2_EXPLICIT(a00r, a01r, a01i, a11r, b_det_scale,
                              a03r, a03i, a13r, a13i,
                              w01r, w01i, w11r, w11i);
    SCHUR4_ZF_SOLVE2_EXPLICIT(a00r, a01r, a01i, a11r, b_det_scale,
                              b0r, b0i, b1r, b1i,
                              y0r, y0i, y1r, y1i);

    SCHUR4_ZF_INNER_EXPLICIT(a02r, a02i, a12r, a12i,
                             w00r, w00i, w10r, w10i,
                             inner_r, inner_i);
    const schur4_zf_state_t s00r =
        SCHUR4_ZF_CAST_STATE(SCHUR4_ZF_WIDE(a22r) - inner_r);

    SCHUR4_ZF_INNER_EXPLICIT(a02r, a02i, a12r, a12i,
                             w01r, w01i, w11r, w11i,
                             inner_r, inner_i);
    const schur4_zf_state_t s01r =
        SCHUR4_ZF_CAST_STATE(SCHUR4_ZF_WIDE(a23r) - inner_r);
    const schur4_zf_state_t s01i =
        SCHUR4_ZF_CAST_STATE(SCHUR4_ZF_WIDE(a23i) - inner_i);

    SCHUR4_ZF_INNER_EXPLICIT(a03r, a03i, a13r, a13i,
                             w01r, w01i, w11r, w11i,
                             inner_r, inner_i);
    const schur4_zf_state_t s11r =
        SCHUR4_ZF_CAST_STATE(SCHUR4_ZF_WIDE(a33r) - inner_r);

    SCHUR4_ZF_INNER_EXPLICIT(a02r, a02i, a12r, a12i,
                             y0r, y0i, y1r, y1i,
                             inner_r, inner_i);
    const schur4_zf_state_t r0r =
        SCHUR4_ZF_CAST_STATE(SCHUR4_ZF_WIDE(b2r) - inner_r);
    const schur4_zf_state_t r0i =
        SCHUR4_ZF_CAST_STATE(SCHUR4_ZF_WIDE(b2i) - inner_i);

    SCHUR4_ZF_INNER_EXPLICIT(a03r, a03i, a13r, a13i,
                             y0r, y0i, y1r, y1i,
                             inner_r, inner_i);
    const schur4_zf_state_t r1r =
        SCHUR4_ZF_CAST_STATE(SCHUR4_ZF_WIDE(b3r) - inner_r);
    const schur4_zf_state_t r1i =
        SCHUR4_ZF_CAST_STATE(SCHUR4_ZF_WIDE(b3i) - inner_i);

    const schur4_zf_num_t s_det_q30 =
        SCHUR4_ZF_DET2_Q30(s00r, s01r, s01i, s11r);
    const schur4_q15_recip_scale_t s_det_scale =
        schur4_q15_recip_scale_from_den(s_det_q30);
    schur4_zf_state_t x2r, x2i, x3r, x3i;

    SCHUR4_ZF_SOLVE2_EXPLICIT(s00r, s01r, s01i, s11r, s_det_scale,
                              r0r, r0i, r1r, r1i,
                              x2r, x2i, x3r, x3i);

    schur4_zf_state_t top0r = y0r;
    schur4_zf_state_t top0i = y0i;
    schur4_zf_state_t top1r = y1r;
    schur4_zf_state_t top1i = y1i;

    SCHUR4_ZF_SUB_MUL_Q15_EXPLICIT(top0r, top0i, w00r, w00i, x2r, x2i);
    SCHUR4_ZF_SUB_MUL_Q15_EXPLICIT(top0r, top0i, w01r, w01i, x3r, x3i);
    SCHUR4_ZF_SUB_MUL_Q15_EXPLICIT(top1r, top1i, w10r, w10i, x2r, x2i);
    SCHUR4_ZF_SUB_MUL_Q15_EXPLICIT(top1r, top1i, w11r, w11i, x3r, x3i);

    x[0] = mimo_make_c16(mimo_solve_store_i16((int32_t)top0r),
                         mimo_solve_store_i16((int32_t)top0i));
    x[1] = mimo_make_c16(mimo_solve_store_i16((int32_t)top1r),
                         mimo_solve_store_i16((int32_t)top1i));
    x[2] = mimo_make_c16(mimo_solve_store_i16((int32_t)x2r),
                         mimo_solve_store_i16((int32_t)x2i));
    x[3] = mimo_make_c16(mimo_solve_store_i16((int32_t)x3r),
                         mimo_solve_store_i16((int32_t)x3i));
}

#if MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER
static MIMO_ALWAYS_INLINE mimo_c16_t schur4_zf_state_c16(
    schur4_zf_state_t r,
    schur4_zf_state_t i)
{
    return mimo_make_c16((int16_t)r, (int16_t)i);
}

static MIMO_ALWAYS_INLINE void schur4_zf_ext_inner_conj2_q15(
    const mimo_c16_t e0[restrict MIMO_SITE4_LANES],
    const mimo_c16_t e1[restrict MIMO_SITE4_LANES],
    const mimo_c16_t w0[restrict MIMO_SITE4_LANES],
    const mimo_c16_t w1[restrict MIMO_SITE4_LANES],
    mimo_c16_t tmp0[restrict MIMO_SITE4_LANES],
    mimo_c16_t tmp1[restrict MIMO_SITE4_LANES],
    mimo_c16_t out[restrict MIMO_SITE4_LANES])
{
    mimo_ext_vscmulcj4_q15(e0, w0, (int16_t *)tmp0);
    mimo_ext_vscmulcj4_q15(e1, w1, (int16_t *)tmp1);
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        out[lane].r = (int16_t)((int32_t)tmp0[lane].r + tmp1[lane].r);
        out[lane].i = (int16_t)((int32_t)tmp0[lane].i + tmp1[lane].i);
    }
}

static MIMO_ALWAYS_INLINE void schur4_zf_ext_mul4_q15(
    const mimo_c16_t a[restrict MIMO_SITE4_LANES],
    const mimo_c16_t b[restrict MIMO_SITE4_LANES],
    mimo_c16_t out[restrict MIMO_SITE4_LANES])
{
    mimo_ext_vscmul4_q15(a, b, (int16_t *)out);
}

#if MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER == 2
static MIMO_ALWAYS_INLINE void schur4_zf_ext_inner_conj2_sum_q15(
    const mimo_c16_t e0[restrict MIMO_SITE4_LANES],
    const mimo_c16_t e1[restrict MIMO_SITE4_LANES],
    const mimo_c16_t w0[restrict MIMO_SITE4_LANES],
    const mimo_c16_t w1[restrict MIMO_SITE4_LANES],
    mimo_c16_t out[restrict MIMO_SITE4_LANES])
{
    asm volatile(
        "vle16.v v20, (%[e0])\n\t"
        "vle16.v v24, (%[w0])\n\t"
        ".word 0xCF4C0857\n\t"
        "vmv.v.v v8, v16\n\t"
        "vle16.v v20, (%[e1])\n\t"
        "vle16.v v24, (%[w1])\n\t"
        ".word 0xCF4C0857\n\t"
        "vadd.vv v8, v8, v16\n\t"
        "vse16.v v8, (%[out])\n\t"
        :
        : [e0] "r"(e0), [e1] "r"(e1), [w0] "r"(w0),
          [w1] "r"(w1), [out] "r"(out)
        : "memory");
}

static MIMO_ALWAYS_INLINE void schur4_zf_ext_inner_conj2_sub_q15(
    const mimo_c16_t e0[restrict MIMO_SITE4_LANES],
    const mimo_c16_t e1[restrict MIMO_SITE4_LANES],
    const mimo_c16_t w0[restrict MIMO_SITE4_LANES],
    const mimo_c16_t w1[restrict MIMO_SITE4_LANES],
    const mimo_c16_t base[restrict MIMO_SITE4_LANES],
    mimo_c16_t out[restrict MIMO_SITE4_LANES])
{
    asm volatile(
        "vle16.v v20, (%[e0])\n\t"
        "vle16.v v24, (%[w0])\n\t"
        ".word 0xCF4C0857\n\t"
        "vmv.v.v v8, v16\n\t"
        "vle16.v v20, (%[e1])\n\t"
        "vle16.v v24, (%[w1])\n\t"
        ".word 0xCF4C0857\n\t"
        "vadd.vv v8, v8, v16\n\t"
        "vle16.v v10, (%[base])\n\t"
        "vsub.vv v10, v10, v8\n\t"
        "vse16.v v10, (%[out])\n\t"
        :
        : [e0] "r"(e0), [e1] "r"(e1), [w0] "r"(w0),
          [w1] "r"(w1), [base] "r"(base), [out] "r"(out)
        : "memory");
}

static MIMO_ALWAYS_INLINE void schur4_zf_ext_mul_sub_inplace_q15(
    const mimo_c16_t a[restrict MIMO_SITE4_LANES],
    const mimo_c16_t b[restrict MIMO_SITE4_LANES],
    mimo_c16_t acc[restrict MIMO_SITE4_LANES])
{
    asm volatile(
        "vle16.v v20, (%[a])\n\t"
        "vle16.v v24, (%[b])\n\t"
        ".word 0xCB4C0857\n\t"
        "vle16.v v8, (%[acc])\n\t"
        "vsub.vv v8, v8, v16\n\t"
        "vse16.v v8, (%[acc])\n\t"
        :
        : [a] "r"(a), [b] "r"(b), [acc] "r"(acc)
        : "memory");
}

static MIMO_NOINLINE void schur4_solve_site4_zf_ext_inner2_q15(
    int32_t (*restrict a_re)[MIMO_SITE4_LANES],
    int32_t (*restrict a_im)[MIMO_SITE4_LANES],
    int32_t (*restrict b_re)[MIMO_SITE4_LANES],
    int32_t (*restrict b_im)[MIMO_SITE4_LANES],
    mimo_c16_t *restrict x_block)
{
    mimo_c16_t e02[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t e12[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t e03[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t e13[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t w00[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t w10[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t w01[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t w11[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t y0[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t y1[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t a23v[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t b2v[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t b3v[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t s00[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t s01[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t s11[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t r0[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t r1[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t x2[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t x3[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t inner[MIMO_SITE4_LANES] MIMO_ALIGN64;
    schur4_zf_state_t a22r[MIMO_SITE4_LANES];
    schur4_zf_state_t a33r[MIMO_SITE4_LANES];

    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        const schur4_zf_state_t a00r = a_re[0][lane];
        const schur4_zf_state_t a01r = a_re[1][lane];
        const schur4_zf_state_t a01i = a_im[1][lane];
        const schur4_zf_state_t a02r = a_re[2][lane];
        const schur4_zf_state_t a02i = a_im[2][lane];
        const schur4_zf_state_t a03r = a_re[3][lane];
        const schur4_zf_state_t a03i = a_im[3][lane];
        const schur4_zf_state_t a11r = a_re[5][lane];
        const schur4_zf_state_t a12r = a_re[6][lane];
        const schur4_zf_state_t a12i = a_im[6][lane];
        const schur4_zf_state_t a13r = a_re[7][lane];
        const schur4_zf_state_t a13i = a_im[7][lane];
        const schur4_zf_state_t b0r = b_re[0][lane];
        const schur4_zf_state_t b0i = b_im[0][lane];
        const schur4_zf_state_t b1r = b_re[1][lane];
        const schur4_zf_state_t b1i = b_im[1][lane];
        schur4_zf_state_t w00r, w00i, w10r, w10i;
        schur4_zf_state_t w01r, w01i, w11r, w11i;
        schur4_zf_state_t y0r, y0i, y1r, y1i;

        const schur4_zf_num_t b_det_q30 =
            SCHUR4_ZF_DET2_Q30(a00r, a01r, a01i, a11r);
        const schur4_q15_recip_scale_t b_det_scale =
            schur4_q15_recip_scale_from_den(b_det_q30);
        SCHUR4_ZF_SOLVE2_EXPLICIT(a00r, a01r, a01i, a11r, b_det_scale,
                                  a02r, a02i, a12r, a12i,
                                  w00r, w00i, w10r, w10i);
        SCHUR4_ZF_SOLVE2_EXPLICIT(a00r, a01r, a01i, a11r, b_det_scale,
                                  a03r, a03i, a13r, a13i,
                                  w01r, w01i, w11r, w11i);
        SCHUR4_ZF_SOLVE2_EXPLICIT(a00r, a01r, a01i, a11r, b_det_scale,
                                  b0r, b0i, b1r, b1i,
                                  y0r, y0i, y1r, y1i);

        e02[lane] = schur4_zf_state_c16(a02r, a02i);
        e12[lane] = schur4_zf_state_c16(a12r, a12i);
        e03[lane] = schur4_zf_state_c16(a03r, a03i);
        e13[lane] = schur4_zf_state_c16(a13r, a13i);
        w00[lane] = schur4_zf_state_c16(w00r, w00i);
        w10[lane] = schur4_zf_state_c16(w10r, w10i);
        w01[lane] = schur4_zf_state_c16(w01r, w01i);
        w11[lane] = schur4_zf_state_c16(w11r, w11i);
        y0[lane] = schur4_zf_state_c16(y0r, y0i);
        y1[lane] = schur4_zf_state_c16(y1r, y1i);
        a23v[lane] = schur4_zf_state_c16(
            (schur4_zf_state_t)a_re[11][lane],
            (schur4_zf_state_t)a_im[11][lane]);
        b2v[lane] = schur4_zf_state_c16((schur4_zf_state_t)b_re[2][lane],
                                        (schur4_zf_state_t)b_im[2][lane]);
        b3v[lane] = schur4_zf_state_c16((schur4_zf_state_t)b_re[3][lane],
                                        (schur4_zf_state_t)b_im[3][lane]);
        a22r[lane] = (schur4_zf_state_t)a_re[10][lane];
        a33r[lane] = (schur4_zf_state_t)a_re[15][lane];
    }

    schur4_zf_ext_inner_conj2_sum_q15(e02, e12, w00, w10, inner);
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        s00[lane] = schur4_zf_state_c16(
            SCHUR4_ZF_CAST_STATE(SCHUR4_ZF_WIDE(a22r[lane]) -
                                 inner[lane].r),
            0);
    }

    schur4_zf_ext_inner_conj2_sub_q15(e02, e12, w01, w11, a23v, s01);

    schur4_zf_ext_inner_conj2_sum_q15(e03, e13, w01, w11, inner);
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        s11[lane] = schur4_zf_state_c16(
            SCHUR4_ZF_CAST_STATE(SCHUR4_ZF_WIDE(a33r[lane]) -
                                 inner[lane].r),
            0);
    }

    schur4_zf_ext_inner_conj2_sub_q15(e02, e12, y0, y1, b2v, r0);
    schur4_zf_ext_inner_conj2_sub_q15(e03, e13, y0, y1, b3v, r1);

    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        const schur4_zf_state_t s00r = s00[lane].r;
        const schur4_zf_state_t s01r = s01[lane].r;
        const schur4_zf_state_t s01i = s01[lane].i;
        const schur4_zf_state_t s11r = s11[lane].r;
        schur4_zf_state_t x2r, x2i, x3r, x3i;

        const schur4_zf_num_t s_det_q30 =
            SCHUR4_ZF_DET2_Q30(s00r, s01r, s01i, s11r);
        const schur4_q15_recip_scale_t s_det_scale =
            schur4_q15_recip_scale_from_den(s_det_q30);

        SCHUR4_ZF_SOLVE2_EXPLICIT(s00r, s01r, s01i, s11r, s_det_scale,
                                  r0[lane].r, r0[lane].i,
                                  r1[lane].r, r1[lane].i,
                                  x2r, x2i, x3r, x3i);
        x2[lane] = schur4_zf_state_c16(x2r, x2i);
        x3[lane] = schur4_zf_state_c16(x3r, x3i);
    }

    schur4_zf_ext_mul_sub_inplace_q15(w00, x2, y0);
    schur4_zf_ext_mul_sub_inplace_q15(w01, x3, y0);
    schur4_zf_ext_mul_sub_inplace_q15(w10, x2, y1);
    schur4_zf_ext_mul_sub_inplace_q15(w11, x3, y1);

    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        mimo_c16_t *x = &x_block[lane * MIMO_N4];

        x[0] = mimo_make_c16(mimo_solve_store_i16(y0[lane].r),
                             mimo_solve_store_i16(y0[lane].i));
        x[1] = mimo_make_c16(mimo_solve_store_i16(y1[lane].r),
                             mimo_solve_store_i16(y1[lane].i));
        x[2] = mimo_make_c16(mimo_solve_store_i16(x2[lane].r),
                             mimo_solve_store_i16(x2[lane].i));
        x[3] = mimo_make_c16(mimo_solve_store_i16(x3[lane].r),
                             mimo_solve_store_i16(x3[lane].i));
    }
}
#endif

#if MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER == 3 || \
    MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER == 4 || \
    MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER == 5
typedef struct {
    mimo_c16_t e02[MIMO_SITE4_LANES];
    mimo_c16_t e12[MIMO_SITE4_LANES];
    mimo_c16_t e03[MIMO_SITE4_LANES];
    mimo_c16_t e13[MIMO_SITE4_LANES];
    mimo_c16_t w00[MIMO_SITE4_LANES];
    mimo_c16_t w10[MIMO_SITE4_LANES];
    mimo_c16_t w01[MIMO_SITE4_LANES];
    mimo_c16_t w11[MIMO_SITE4_LANES];
    mimo_c16_t y0[MIMO_SITE4_LANES];
    mimo_c16_t y1[MIMO_SITE4_LANES];
    mimo_c16_t a22[MIMO_SITE4_LANES];
    mimo_c16_t a23[MIMO_SITE4_LANES];
    mimo_c16_t a33[MIMO_SITE4_LANES];
    mimo_c16_t b2[MIMO_SITE4_LANES];
    mimo_c16_t b3[MIMO_SITE4_LANES];
    mimo_c16_t s00[MIMO_SITE4_LANES];
    mimo_c16_t s01[MIMO_SITE4_LANES];
    mimo_c16_t s11[MIMO_SITE4_LANES];
    mimo_c16_t r0[MIMO_SITE4_LANES];
    mimo_c16_t r1[MIMO_SITE4_LANES];
    mimo_c16_t x2[MIMO_SITE4_LANES];
    mimo_c16_t x3[MIMO_SITE4_LANES];
#if MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER == 3
    mimo_c16_t top0[MIMO_SITE4_LANES];
    mimo_c16_t top1[MIMO_SITE4_LANES];
#endif
} schur4_zf_ext_inner3_pack_t;

#if MIMO_USE_EXT_ZF4_TRUE16B_WORK
typedef struct {
    mimo_c16_t w00[MIMO_SITE4_LANES];
    mimo_c16_t w10[MIMO_SITE4_LANES];
    mimo_c16_t w01[MIMO_SITE4_LANES];
    mimo_c16_t w11[MIMO_SITE4_LANES];
    mimo_c16_t y0[MIMO_SITE4_LANES];
    mimo_c16_t y1[MIMO_SITE4_LANES];
    mimo_c16_t s00[MIMO_SITE4_LANES];
    mimo_c16_t s01[MIMO_SITE4_LANES];
    mimo_c16_t s11[MIMO_SITE4_LANES];
    mimo_c16_t r0[MIMO_SITE4_LANES];
    mimo_c16_t r1[MIMO_SITE4_LANES];
    mimo_c16_t x2[MIMO_SITE4_LANES];
    mimo_c16_t x3[MIMO_SITE4_LANES];
} schur4_zf_upper_i16_pack_t;
#endif

#if MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SPLIT_SOLVE
typedef struct {
    mimo_c16_t w00[MIMO_SITE4_LANES];
    mimo_c16_t w10[MIMO_SITE4_LANES];
    mimo_c16_t w01[MIMO_SITE4_LANES];
    mimo_c16_t w11[MIMO_SITE4_LANES];
    mimo_c16_t y0[MIMO_SITE4_LANES];
    mimo_c16_t y1[MIMO_SITE4_LANES];
    mimo_c16_t s00[MIMO_SITE4_LANES];
    mimo_c16_t s01[MIMO_SITE4_LANES];
    mimo_c16_t s11[MIMO_SITE4_LANES];
    mimo_c16_t r0[MIMO_SITE4_LANES];
    mimo_c16_t r1[MIMO_SITE4_LANES];
    mimo_c16_t x2[MIMO_SITE4_LANES];
    mimo_c16_t x3[MIMO_SITE4_LANES];
} schur4_zf_true16b_split_pack_t;
#endif

static MIMO_ALWAYS_INLINE void schur4_zf_ext_schur5_q15(
    const mimo_c16_t e02[restrict MIMO_SITE4_LANES],
    const mimo_c16_t e12[restrict MIMO_SITE4_LANES],
    const mimo_c16_t e03[restrict MIMO_SITE4_LANES],
    const mimo_c16_t e13[restrict MIMO_SITE4_LANES],
    const mimo_c16_t w00[restrict MIMO_SITE4_LANES],
    const mimo_c16_t w10[restrict MIMO_SITE4_LANES],
    const mimo_c16_t w01[restrict MIMO_SITE4_LANES],
    const mimo_c16_t w11[restrict MIMO_SITE4_LANES],
    const mimo_c16_t y0[restrict MIMO_SITE4_LANES],
    const mimo_c16_t y1[restrict MIMO_SITE4_LANES],
    const mimo_c16_t a22[restrict MIMO_SITE4_LANES],
    const mimo_c16_t a23[restrict MIMO_SITE4_LANES],
    const mimo_c16_t a33[restrict MIMO_SITE4_LANES],
    const mimo_c16_t b2[restrict MIMO_SITE4_LANES],
    const mimo_c16_t b3[restrict MIMO_SITE4_LANES],
    mimo_c16_t s00[restrict MIMO_SITE4_LANES],
    mimo_c16_t s01[restrict MIMO_SITE4_LANES],
    mimo_c16_t s11[restrict MIMO_SITE4_LANES],
    mimo_c16_t r0[restrict MIMO_SITE4_LANES],
    mimo_c16_t r1[restrict MIMO_SITE4_LANES])
{
#if MIMO_USE_SOLVE4_SCHUR_ZF_EXT_MACC_INNER
    asm volatile(
        "vmv.v.i v16, 0\n\t"
        "vle16.v v20, (%[e02])\n\t"
        "vle16.v v24, (%[w00])\n\t"
        ".word 0xDB4C0857\n\t"
        "vle16.v v20, (%[e12])\n\t"
        "vle16.v v24, (%[w10])\n\t"
        ".word 0xDB4C0857\n\t"
        "vle16.v v10, (%[a22])\n\t"
        "vsub.vv v10, v10, v16\n\t"
        "vse16.v v10, (%[s00])\n\t"

        "vmv.v.i v16, 0\n\t"
        "vle16.v v20, (%[e02])\n\t"
        "vle16.v v24, (%[w01])\n\t"
        ".word 0xDB4C0857\n\t"
        "vle16.v v20, (%[e12])\n\t"
        "vle16.v v24, (%[w11])\n\t"
        ".word 0xDB4C0857\n\t"
        "vle16.v v10, (%[a23])\n\t"
        "vsub.vv v10, v10, v16\n\t"
        "vse16.v v10, (%[s01])\n\t"

        "vmv.v.i v16, 0\n\t"
        "vle16.v v20, (%[e03])\n\t"
        "vle16.v v24, (%[w01])\n\t"
        ".word 0xDB4C0857\n\t"
        "vle16.v v20, (%[e13])\n\t"
        "vle16.v v24, (%[w11])\n\t"
        ".word 0xDB4C0857\n\t"
        "vle16.v v10, (%[a33])\n\t"
        "vsub.vv v10, v10, v16\n\t"
        "vse16.v v10, (%[s11])\n\t"

        "vmv.v.i v16, 0\n\t"
        "vle16.v v20, (%[e02])\n\t"
        "vle16.v v24, (%[y0])\n\t"
        ".word 0xDB4C0857\n\t"
        "vle16.v v20, (%[e12])\n\t"
        "vle16.v v24, (%[y1])\n\t"
        ".word 0xDB4C0857\n\t"
        "vle16.v v10, (%[b2])\n\t"
        "vsub.vv v10, v10, v16\n\t"
        "vse16.v v10, (%[r0])\n\t"

        "vmv.v.i v16, 0\n\t"
        "vle16.v v20, (%[e03])\n\t"
        "vle16.v v24, (%[y0])\n\t"
        ".word 0xDB4C0857\n\t"
        "vle16.v v20, (%[e13])\n\t"
        "vle16.v v24, (%[y1])\n\t"
        ".word 0xDB4C0857\n\t"
        "vle16.v v10, (%[b3])\n\t"
        "vsub.vv v10, v10, v16\n\t"
        "vse16.v v10, (%[r1])\n\t"
        :
        : [e02] "r"(e02), [e12] "r"(e12), [e03] "r"(e03),
          [e13] "r"(e13), [w00] "r"(w00), [w10] "r"(w10),
          [w01] "r"(w01), [w11] "r"(w11), [y0] "r"(y0),
          [y1] "r"(y1), [a22] "r"(a22), [a23] "r"(a23),
          [a33] "r"(a33), [b2] "r"(b2), [b3] "r"(b3),
          [s00] "r"(s00), [s01] "r"(s01), [s11] "r"(s11),
          [r0] "r"(r0), [r1] "r"(r1)
        : "memory");
#else
    asm volatile(
        "vle16.v v20, (%[e02])\n\t"
        "vle16.v v24, (%[w00])\n\t"
        ".word 0xCF4C0857\n\t"
        "vmv.v.v v8, v16\n\t"
        "vle16.v v20, (%[e12])\n\t"
        "vle16.v v24, (%[w10])\n\t"
        ".word 0xCF4C0857\n\t"
        "vadd.vv v8, v8, v16\n\t"
        "vle16.v v10, (%[a22])\n\t"
        "vsub.vv v10, v10, v8\n\t"
        "vse16.v v10, (%[s00])\n\t"

        "vle16.v v20, (%[e02])\n\t"
        "vle16.v v24, (%[w01])\n\t"
        ".word 0xCF4C0857\n\t"
        "vmv.v.v v8, v16\n\t"
        "vle16.v v20, (%[e12])\n\t"
        "vle16.v v24, (%[w11])\n\t"
        ".word 0xCF4C0857\n\t"
        "vadd.vv v8, v8, v16\n\t"
        "vle16.v v10, (%[a23])\n\t"
        "vsub.vv v10, v10, v8\n\t"
        "vse16.v v10, (%[s01])\n\t"

        "vle16.v v20, (%[e03])\n\t"
        "vle16.v v24, (%[w01])\n\t"
        ".word 0xCF4C0857\n\t"
        "vmv.v.v v8, v16\n\t"
        "vle16.v v20, (%[e13])\n\t"
        "vle16.v v24, (%[w11])\n\t"
        ".word 0xCF4C0857\n\t"
        "vadd.vv v8, v8, v16\n\t"
        "vle16.v v10, (%[a33])\n\t"
        "vsub.vv v10, v10, v8\n\t"
        "vse16.v v10, (%[s11])\n\t"

        "vle16.v v20, (%[e02])\n\t"
        "vle16.v v24, (%[y0])\n\t"
        ".word 0xCF4C0857\n\t"
        "vmv.v.v v8, v16\n\t"
        "vle16.v v20, (%[e12])\n\t"
        "vle16.v v24, (%[y1])\n\t"
        ".word 0xCF4C0857\n\t"
        "vadd.vv v8, v8, v16\n\t"
        "vle16.v v10, (%[b2])\n\t"
        "vsub.vv v10, v10, v8\n\t"
        "vse16.v v10, (%[r0])\n\t"

        "vle16.v v20, (%[e03])\n\t"
        "vle16.v v24, (%[y0])\n\t"
        ".word 0xCF4C0857\n\t"
        "vmv.v.v v8, v16\n\t"
        "vle16.v v20, (%[e13])\n\t"
        "vle16.v v24, (%[y1])\n\t"
        ".word 0xCF4C0857\n\t"
        "vadd.vv v8, v8, v16\n\t"
        "vle16.v v10, (%[b3])\n\t"
        "vsub.vv v10, v10, v8\n\t"
        "vse16.v v10, (%[r1])\n\t"
        :
        : [e02] "r"(e02), [e12] "r"(e12), [e03] "r"(e03),
          [e13] "r"(e13), [w00] "r"(w00), [w10] "r"(w10),
          [w01] "r"(w01), [w11] "r"(w11), [y0] "r"(y0),
          [y1] "r"(y1), [a22] "r"(a22), [a23] "r"(a23),
          [a33] "r"(a33), [b2] "r"(b2), [b3] "r"(b3),
          [s00] "r"(s00), [s01] "r"(s01), [s11] "r"(s11),
          [r0] "r"(r0), [r1] "r"(r1)
        : "memory");
#endif
}

static MIMO_ALWAYS_INLINE void schur4_zf_ext_top2_q15(
    const mimo_c16_t w00[restrict MIMO_SITE4_LANES],
    const mimo_c16_t w10[restrict MIMO_SITE4_LANES],
    const mimo_c16_t w01[restrict MIMO_SITE4_LANES],
    const mimo_c16_t w11[restrict MIMO_SITE4_LANES],
    const mimo_c16_t x2[restrict MIMO_SITE4_LANES],
    const mimo_c16_t x3[restrict MIMO_SITE4_LANES],
    const mimo_c16_t y0[restrict MIMO_SITE4_LANES],
    const mimo_c16_t y1[restrict MIMO_SITE4_LANES],
    mimo_c16_t top0[restrict MIMO_SITE4_LANES],
    mimo_c16_t top1[restrict MIMO_SITE4_LANES])
{
    asm volatile(
        "vle16.v v8, (%[y0])\n\t"
        "vle16.v v20, (%[w00])\n\t"
        "vle16.v v24, (%[x2])\n\t"
        ".word 0xCB4C0857\n\t"
        "vsub.vv v8, v8, v16\n\t"
        "vle16.v v20, (%[w01])\n\t"
        "vle16.v v24, (%[x3])\n\t"
        ".word 0xCB4C0857\n\t"
        "vsub.vv v8, v8, v16\n\t"
        "vse16.v v8, (%[top0])\n\t"

        "vle16.v v10, (%[y1])\n\t"
        "vle16.v v20, (%[w10])\n\t"
        "vle16.v v24, (%[x2])\n\t"
        ".word 0xCB4C0857\n\t"
        "vsub.vv v10, v10, v16\n\t"
        "vle16.v v20, (%[w11])\n\t"
        "vle16.v v24, (%[x3])\n\t"
        ".word 0xCB4C0857\n\t"
        "vsub.vv v10, v10, v16\n\t"
        "vse16.v v10, (%[top1])\n\t"
        :
        : [w00] "r"(w00), [w10] "r"(w10), [w01] "r"(w01),
          [w11] "r"(w11), [x2] "r"(x2), [x3] "r"(x3),
          [y0] "r"(y0), [y1] "r"(y1), [top0] "r"(top0),
          [top1] "r"(top1)
        : "memory");
}

#if MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER == 4
static MIMO_ALWAYS_INLINE void schur4_zf_ext_top2_store_q15(
    const mimo_c16_t w00[restrict MIMO_SITE4_LANES],
    const mimo_c16_t w10[restrict MIMO_SITE4_LANES],
    const mimo_c16_t w01[restrict MIMO_SITE4_LANES],
    const mimo_c16_t w11[restrict MIMO_SITE4_LANES],
    const mimo_c16_t x2[restrict MIMO_SITE4_LANES],
    const mimo_c16_t x3[restrict MIMO_SITE4_LANES],
    const mimo_c16_t y0[restrict MIMO_SITE4_LANES],
    const mimo_c16_t y1[restrict MIMO_SITE4_LANES],
    mimo_c16_t *restrict x_block)
{
    asm volatile(
        "li t1, 16\n\t"
        "vle16.v v8, (%[y0])\n\t"
        "vle16.v v20, (%[w00])\n\t"
        "vle16.v v24, (%[x2])\n\t"
        ".word 0xCB4C0857\n\t"
        "vsub.vv v8, v8, v16\n\t"
        "vle16.v v20, (%[w01])\n\t"
        "vle16.v v24, (%[x3])\n\t"
        ".word 0xCB4C0857\n\t"
        "vsub.vv v8, v8, v16\n\t"
        "vrgather.vv v18, v8, v28\n\t"
        "vrgather.vv v19, v8, v30\n\t"
        "vsse16.v v18, (%[x0r]), t1, v0.t\n\t"
        "vsse16.v v19, (%[x0i]), t1, v0.t\n\t"

        "vle16.v v10, (%[y1])\n\t"
        "vle16.v v20, (%[w10])\n\t"
        "vle16.v v24, (%[x2])\n\t"
        ".word 0xCB4C0857\n\t"
        "vsub.vv v10, v10, v16\n\t"
        "vle16.v v20, (%[w11])\n\t"
        "vle16.v v24, (%[x3])\n\t"
        ".word 0xCB4C0857\n\t"
        "vsub.vv v10, v10, v16\n\t"
        "vrgather.vv v18, v10, v28\n\t"
        "vrgather.vv v19, v10, v30\n\t"
        "vsse16.v v18, (%[x1r]), t1, v0.t\n\t"
        "vsse16.v v19, (%[x1i]), t1, v0.t\n\t"

        "vle16.v v12, (%[x2])\n\t"
        "vrgather.vv v18, v12, v28\n\t"
        "vrgather.vv v19, v12, v30\n\t"
        "vsse16.v v18, (%[x2r]), t1, v0.t\n\t"
        "vsse16.v v19, (%[x2i]), t1, v0.t\n\t"

        "vle16.v v14, (%[x3])\n\t"
        "vrgather.vv v18, v14, v28\n\t"
        "vrgather.vv v19, v14, v30\n\t"
        "vsse16.v v18, (%[x3r]), t1, v0.t\n\t"
        "vsse16.v v19, (%[x3i]), t1, v0.t\n\t"
        :
        : [w00] "r"(w00), [w10] "r"(w10), [w01] "r"(w01),
          [w11] "r"(w11), [x2] "r"(x2), [x3] "r"(x3),
          [y0] "r"(y0), [y1] "r"(y1),
          [x0r] "r"(&x_block[0].r), [x0i] "r"(&x_block[0].i),
          [x1r] "r"(&x_block[1].r), [x1i] "r"(&x_block[1].i),
          [x2r] "r"(&x_block[2].r), [x2i] "r"(&x_block[2].i),
          [x3r] "r"(&x_block[3].r), [x3i] "r"(&x_block[3].i)
        : "t1", "memory");
}
#endif

#if MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER == 5
static MIMO_ALWAYS_INLINE void schur4_zf_ext_top2_store32_q15(
    const mimo_c16_t w00[restrict MIMO_SITE4_LANES],
    const mimo_c16_t w10[restrict MIMO_SITE4_LANES],
    const mimo_c16_t w01[restrict MIMO_SITE4_LANES],
    const mimo_c16_t w11[restrict MIMO_SITE4_LANES],
    const mimo_c16_t x2[restrict MIMO_SITE4_LANES],
    const mimo_c16_t x3[restrict MIMO_SITE4_LANES],
    const mimo_c16_t y0[restrict MIMO_SITE4_LANES],
    const mimo_c16_t y1[restrict MIMO_SITE4_LANES],
    mimo_c16_t *restrict x_block)
{
    asm volatile(
        "li t1, 16\n\t"
        "vle16.v v8, (%[y0])\n\t"
        "vle16.v v20, (%[w00])\n\t"
        "vle16.v v24, (%[x2])\n\t"
        ".word 0xCB4C0857\n\t"
        "vsub.vv v8, v8, v16\n\t"
        "vle16.v v20, (%[w01])\n\t"
        "vle16.v v24, (%[x3])\n\t"
        ".word 0xCB4C0857\n\t"
        "vsub.vv v8, v8, v16\n\t"
        "vsse32.v v8, (%[x0]), t1, v0.t\n\t"

        "vle16.v v10, (%[y1])\n\t"
        "vle16.v v20, (%[w10])\n\t"
        "vle16.v v24, (%[x2])\n\t"
        ".word 0xCB4C0857\n\t"
        "vsub.vv v10, v10, v16\n\t"
        "vle16.v v20, (%[w11])\n\t"
        "vle16.v v24, (%[x3])\n\t"
        ".word 0xCB4C0857\n\t"
        "vsub.vv v10, v10, v16\n\t"
        "vsse32.v v10, (%[x1]), t1, v0.t\n\t"

        "vle16.v v12, (%[x2])\n\t"
        "vsse32.v v12, (%[x2o]), t1, v0.t\n\t"

        "vle16.v v14, (%[x3])\n\t"
        "vsse32.v v14, (%[x3o]), t1, v0.t\n\t"
        :
        : [w00] "r"(w00), [w10] "r"(w10), [w01] "r"(w01),
          [w11] "r"(w11), [x2] "r"(x2), [x3] "r"(x3),
          [y0] "r"(y0), [y1] "r"(y1),
          [x0] "r"(&x_block[0]), [x1] "r"(&x_block[1]),
          [x2o] "r"(&x_block[2]), [x3o] "r"(&x_block[3])
        : "t1", "memory");
}

#if MIMO_USE_EXT_ZF4_SCALAR_X23_STORE
static MIMO_ALWAYS_INLINE void schur4_zf_ext_top2_store32_toponly_q15(
    const mimo_c16_t w00[restrict MIMO_SITE4_LANES],
    const mimo_c16_t w10[restrict MIMO_SITE4_LANES],
    const mimo_c16_t w01[restrict MIMO_SITE4_LANES],
    const mimo_c16_t w11[restrict MIMO_SITE4_LANES],
    const mimo_c16_t x2[restrict MIMO_SITE4_LANES],
    const mimo_c16_t x3[restrict MIMO_SITE4_LANES],
    const mimo_c16_t y0[restrict MIMO_SITE4_LANES],
    const mimo_c16_t y1[restrict MIMO_SITE4_LANES],
    mimo_c16_t *restrict x_block)
{
    asm volatile(
        "li t1, 16\n\t"
        "vle16.v v8, (%[y0])\n\t"
        "vle16.v v20, (%[w00])\n\t"
        "vle16.v v24, (%[x2])\n\t"
        ".word 0xCB4C0857\n\t"
        "vsub.vv v8, v8, v16\n\t"
        "vle16.v v20, (%[w01])\n\t"
        "vle16.v v24, (%[x3])\n\t"
        ".word 0xCB4C0857\n\t"
        "vsub.vv v8, v8, v16\n\t"
        "vsse32.v v8, (%[x0]), t1, v0.t\n\t"

        "vle16.v v10, (%[y1])\n\t"
        "vle16.v v20, (%[w10])\n\t"
        "vle16.v v24, (%[x2])\n\t"
        ".word 0xCB4C0857\n\t"
        "vsub.vv v10, v10, v16\n\t"
        "vle16.v v20, (%[w11])\n\t"
        "vle16.v v24, (%[x3])\n\t"
        ".word 0xCB4C0857\n\t"
        "vsub.vv v10, v10, v16\n\t"
        "vsse32.v v10, (%[x1]), t1, v0.t\n\t"
        :
        : [w00] "r"(w00), [w10] "r"(w10), [w01] "r"(w01),
          [w11] "r"(w11), [x2] "r"(x2), [x3] "r"(x3),
          [y0] "r"(y0), [y1] "r"(y1),
          [x0] "r"(&x_block[0]), [x1] "r"(&x_block[1])
        : "t1", "memory");
}
#endif
#endif

#if MIMO_USE_SOLVE4_SCHUR_ZF_Q15_NUM_SOLVE
static MIMO_ALWAYS_INLINE int16_t schur4_zf_ext_scale_num_q15_to_q15(
    int16_t num_q15,
    schur4_q15_recip_scale_t scale)
{
    const int64_t prod = (int64_t)num_q15 * (int64_t)scale.scale_q30;
    const int32_t q15 = (int32_t)q15_round_shift_i64(prod);

    return mimo_solve_store_i16(q15);
}

static MIMO_ALWAYS_INLINE void schur4_zf_ext_solve2_q15num_q15(
    const mimo_c16_t a00[restrict MIMO_SITE4_LANES],
    const mimo_c16_t a01[restrict MIMO_SITE4_LANES],
    const mimo_c16_t a11[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v0[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v1[restrict MIMO_SITE4_LANES],
    const schur4_q15_recip_scale_t scale[restrict MIMO_SITE4_LANES],
    mimo_c16_t out0[restrict MIMO_SITE4_LANES],
    mimo_c16_t out1[restrict MIMO_SITE4_LANES])
{
    mimo_c16_t acc[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t tmp[MIMO_SITE4_LANES] MIMO_ALIGN64;

    mimo_ext_vscmul4_q15(a11, v0, (int16_t *)acc);
    mimo_ext_vscmul4_q15(a01, v1, (int16_t *)tmp);
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        const int16_t nr =
            (int16_t)((int32_t)acc[lane].r - tmp[lane].r);
        const int16_t ni =
            (int16_t)((int32_t)acc[lane].i - tmp[lane].i);

        out0[lane] = schur4_zf_state_c16(
            schur4_zf_ext_scale_num_q15_to_q15(nr, scale[lane]),
            schur4_zf_ext_scale_num_q15_to_q15(ni, scale[lane]));
    }

    mimo_ext_vscmul4_q15(a00, v1, (int16_t *)acc);
    mimo_ext_vscmulcj4_q15(a01, v0, (int16_t *)tmp);
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        const int16_t nr =
            (int16_t)((int32_t)acc[lane].r - tmp[lane].r);
        const int16_t ni =
            (int16_t)((int32_t)acc[lane].i - tmp[lane].i);

        out1[lane] = schur4_zf_state_c16(
            schur4_zf_ext_scale_num_q15_to_q15(nr, scale[lane]),
            schur4_zf_ext_scale_num_q15_to_q15(ni, scale[lane]));
    }
}

static MIMO_ALWAYS_INLINE void schur4_zf_ext_solve2h3_q15num_q15(
    const mimo_c16_t a00[restrict MIMO_SITE4_LANES],
    const mimo_c16_t a01[restrict MIMO_SITE4_LANES],
    const mimo_c16_t a11[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v00[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v01[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v10[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v11[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v20[restrict MIMO_SITE4_LANES],
    const mimo_c16_t v21[restrict MIMO_SITE4_LANES],
    const schur4_q15_recip_scale_t scale[restrict MIMO_SITE4_LANES],
    mimo_c16_t out00[restrict MIMO_SITE4_LANES],
    mimo_c16_t out01[restrict MIMO_SITE4_LANES],
    mimo_c16_t out10[restrict MIMO_SITE4_LANES],
    mimo_c16_t out11[restrict MIMO_SITE4_LANES],
    mimo_c16_t out20[restrict MIMO_SITE4_LANES],
    mimo_c16_t out21[restrict MIMO_SITE4_LANES])
{
    schur4_zf_ext_solve2_q15num_q15(
        a00, a01, a11, v00, v01, scale, out00, out01);
    schur4_zf_ext_solve2_q15num_q15(
        a00, a01, a11, v10, v11, scale, out10, out11);
    schur4_zf_ext_solve2_q15num_q15(
        a00, a01, a11, v20, v21, scale, out20, out21);
}
#endif

static SCHUR4_ZF_HELPER_ATTR void schur4_solve_site4_zf_ext_inner3_q15(
    int32_t (*restrict a_re)[MIMO_SITE4_LANES],
    int32_t (*restrict a_im)[MIMO_SITE4_LANES],
    int32_t (*restrict b_re)[MIMO_SITE4_LANES],
    int32_t (*restrict b_im)[MIMO_SITE4_LANES],
    mimo_c16_t *restrict x_block)
{
#if MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_ASM_STUB
    mimo_vschur2h_asm_stub_q15();
#endif

    schur4_zf_ext_inner3_pack_t p MIMO_ALIGN64;
#if MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_MODEL || \
    MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_MODEL || \
    MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_ASM
    mimo_c16_t b_a00[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t b_a01[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t b_a11[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t b0v[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t b1v[MIMO_SITE4_LANES] MIMO_ALIGN64;
#endif

    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        const schur4_zf_state_t a00r = a_re[0][lane];
        const schur4_zf_state_t a01r = a_re[1][lane];
        const schur4_zf_state_t a01i = a_im[1][lane];
        const schur4_zf_state_t a02r = a_re[2][lane];
        const schur4_zf_state_t a02i = a_im[2][lane];
        const schur4_zf_state_t a03r = a_re[3][lane];
        const schur4_zf_state_t a03i = a_im[3][lane];
        const schur4_zf_state_t a11r = a_re[5][lane];
        const schur4_zf_state_t a12r = a_re[6][lane];
        const schur4_zf_state_t a12i = a_im[6][lane];
        const schur4_zf_state_t a13r = a_re[7][lane];
        const schur4_zf_state_t a13i = a_im[7][lane];
        const schur4_zf_state_t b0r = b_re[0][lane];
        const schur4_zf_state_t b0i = b_im[0][lane];
        const schur4_zf_state_t b1r = b_re[1][lane];
        const schur4_zf_state_t b1i = b_im[1][lane];
#if !MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_MODEL && \
    !MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_MODEL && \
    !MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_ASM
        schur4_zf_state_t w00r, w00i, w10r, w10i;
        schur4_zf_state_t w01r, w01i, w11r, w11i;
        schur4_zf_state_t y0r, y0i, y1r, y1i;
#endif

#if MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_MODEL || \
    MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_MODEL || \
    MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_ASM
        b_a00[lane] = schur4_zf_state_c16(a00r, 0);
        b_a01[lane] = schur4_zf_state_c16(a01r, a01i);
        b_a11[lane] = schur4_zf_state_c16(a11r, 0);
        b0v[lane] = schur4_zf_state_c16(b0r, b0i);
        b1v[lane] = schur4_zf_state_c16(b1r, b1i);
#elif MIMO_USE_SOLVE4_SCHUR_ZF_PIVOTSCALE_MODEL
        schur4_zf_solve2h3_q15_primitive_model(
            a00r, a01r, a01i, a11r,
            a02r, a02i, a12r, a12i,
            a03r, a03i, a13r, a13i,
            b0r, b0i, b1r, b1i,
            &w00r, &w00i, &w10r, &w10i,
            &w01r, &w01i, &w11r, &w11i,
            &y0r, &y0i, &y1r, &y1i);
#else
        const schur4_zf_num_t b_det_q30 =
            SCHUR4_ZF_DET2_Q30(a00r, a01r, a01i, a11r);
        const schur4_q15_recip_scale_t b_det_scale =
            schur4_q15_recip_scale_from_den(b_det_q30);

        SCHUR4_ZF_SOLVE2_EXPLICIT(a00r, a01r, a01i, a11r, b_det_scale,
                                  a02r, a02i, a12r, a12i,
                                  w00r, w00i, w10r, w10i);
        SCHUR4_ZF_SOLVE2_EXPLICIT(a00r, a01r, a01i, a11r, b_det_scale,
                                  a03r, a03i, a13r, a13i,
                                  w01r, w01i, w11r, w11i);
        SCHUR4_ZF_SOLVE2_EXPLICIT(a00r, a01r, a01i, a11r, b_det_scale,
                                  b0r, b0i, b1r, b1i,
                                  y0r, y0i, y1r, y1i);
#endif

        p.e02[lane] = schur4_zf_state_c16(a02r, a02i);
        p.e12[lane] = schur4_zf_state_c16(a12r, a12i);
        p.e03[lane] = schur4_zf_state_c16(a03r, a03i);
        p.e13[lane] = schur4_zf_state_c16(a13r, a13i);
#if !MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_MODEL && \
    !MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_MODEL && \
    !MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_ASM
        p.w00[lane] = schur4_zf_state_c16(w00r, w00i);
        p.w10[lane] = schur4_zf_state_c16(w10r, w10i);
        p.w01[lane] = schur4_zf_state_c16(w01r, w01i);
        p.w11[lane] = schur4_zf_state_c16(w11r, w11i);
        p.y0[lane] = schur4_zf_state_c16(y0r, y0i);
        p.y1[lane] = schur4_zf_state_c16(y1r, y1i);
#endif
        p.a22[lane] = schur4_zf_state_c16(
            (schur4_zf_state_t)a_re[10][lane], 0);
        p.a23[lane] = schur4_zf_state_c16(
            (schur4_zf_state_t)a_re[11][lane],
            (schur4_zf_state_t)a_im[11][lane]);
        p.a33[lane] = schur4_zf_state_c16(
            (schur4_zf_state_t)a_re[15][lane], 0);
        p.b2[lane] = schur4_zf_state_c16((schur4_zf_state_t)b_re[2][lane],
                                         (schur4_zf_state_t)b_im[2][lane]);
        p.b3[lane] = schur4_zf_state_c16((schur4_zf_state_t)b_re[3][lane],
                                         (schur4_zf_state_t)b_im[3][lane]);
    }

#if MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_MODEL
#if MIMO_CHECK_SOLVE2H_GROUP_LAYOUT
    schur4_zf_solve2h3_group_layout_checked_q15(
        b_a00, b_a01, b_a11, p.e02, p.e12, p.e03, p.e13, b0v, b1v,
        p.w00, p.w10, p.w01, p.w11, p.y0, p.y1);
#else
    schur4_zf_solve2h3_group_q15_primitive_model(
        b_a00, b_a01, b_a11, p.e02, p.e12, p.e03, p.e13, b0v, b1v,
        p.w00, p.w10, p.w01, p.w11, p.y0, p.y1);
#endif
#elif MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_MODEL || \
      MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_ASM
#if MIMO_CHECK_VSOLVE2H2_LAYOUT
    schur4_zf_vsolve2h2_3rhs_group_layout_checked_q15(
        b_a00, b_a01, b_a11, p.e02, p.e12, p.e03, p.e13, b0v, b1v,
        p.w00, p.w10, p.w01, p.w11, p.y0, p.y1);
#else
    schur4_zf_vsolve2h2_3rhs_group_q15_primitive_model(
        b_a00, b_a01, b_a11, p.e02, p.e12, p.e03, p.e13, b0v, b1v,
        p.w00, p.w10, p.w01, p.w11, p.y0, p.y1);
#endif
#endif

    schur4_zf_ext_schur5_q15(
        p.e02, p.e12, p.e03, p.e13, p.w00, p.w10, p.w01, p.w11,
        p.y0, p.y1, p.a22, p.a23, p.a33, p.b2, p.b3,
        p.s00, p.s01, p.s11, p.r0, p.r1);

#if MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_MODEL
#if MIMO_CHECK_SOLVE2H_GROUP_LAYOUT
    schur4_zf_solve2h1_group_layout_checked_q15(
        p.s00, p.s01, p.s11, p.r0, p.r1, p.x2, p.x3);
#else
    schur4_zf_solve2h1_group_q15_primitive_model(
        p.s00, p.s01, p.s11, p.r0, p.r1, p.x2, p.x3);
#endif
#elif MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_MODEL || \
      MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_ASM
#if MIMO_CHECK_VSOLVE2H2_LAYOUT
    schur4_zf_vsolve2h2_group_layout_checked_q15(
        p.s00, p.s01, p.s11, p.r0, p.r1, p.x2, p.x3);
#else
    schur4_zf_vsolve2h2_group_q15_primitive_model(
        p.s00, p.s01, p.s11, p.r0, p.r1, p.x2, p.x3);
#endif
#else
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        const schur4_zf_state_t s00r = p.s00[lane].r;
        const schur4_zf_state_t s01r = p.s01[lane].r;
        const schur4_zf_state_t s01i = p.s01[lane].i;
        const schur4_zf_state_t s11r = p.s11[lane].r;
        schur4_zf_state_t x2r, x2i, x3r, x3i;

        const schur4_zf_num_t s_det_q30 =
            SCHUR4_ZF_DET2_Q30(s00r, s01r, s01i, s11r);
        const schur4_q15_recip_scale_t s_det_scale =
            schur4_q15_recip_scale_from_den(s_det_q30);
        SCHUR4_ZF_SOLVE2_EXPLICIT(s00r, s01r, s01i, s11r, s_det_scale,
                                  p.r0[lane].r, p.r0[lane].i,
                                  p.r1[lane].r, p.r1[lane].i,
                                  x2r, x2i, x3r, x3i);
        p.x2[lane] = schur4_zf_state_c16(x2r, x2i);
        p.x3[lane] = schur4_zf_state_c16(x3r, x3i);
    }
#endif

#if MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER == 4
    schur4_zf_ext_top2_store_q15(p.w00, p.w10, p.w01, p.w11, p.x2, p.x3,
                                 p.y0, p.y1, x_block);
#elif MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER == 5
    schur4_zf_ext_top2_store32_q15(p.w00, p.w10, p.w01, p.w11, p.x2, p.x3,
                                   p.y0, p.y1, x_block);
#else
    schur4_zf_ext_top2_q15(p.w00, p.w10, p.w01, p.w11, p.x2, p.x3,
                           p.y0, p.y1, p.top0, p.top1);
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        mimo_c16_t *x = &x_block[lane * MIMO_N4];

        x[0] = mimo_make_c16(mimo_solve_store_i16(p.top0[lane].r),
                             mimo_solve_store_i16(p.top0[lane].i));
        x[1] = mimo_make_c16(mimo_solve_store_i16(p.top1[lane].r),
                             mimo_solve_store_i16(p.top1[lane].i));
        x[2] = mimo_make_c16(mimo_solve_store_i16(p.x2[lane].r),
                             mimo_solve_store_i16(p.x2[lane].i));
        x[3] = mimo_make_c16(mimo_solve_store_i16(p.x3[lane].r),
                             mimo_solve_store_i16(p.x3[lane].i));
    }
#endif
}

#if MIMO_USE_EXT_ZF4_TRUE16B_WORK
#if MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE || \
    MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_BSOLVE
static MIMO_ALWAYS_INLINE vint32m1_t zf4_i16_complex_re_to_i32(
    const mimo_c16_t v[restrict MIMO_SITE4_LANES],
    size_t vl)
{
    const vint16mf2_t x =
        __riscv_vlse16_v_i16mf2(&v[0].r, sizeof(mimo_c16_t), vl);
    return __riscv_vsext_vf2_i32m1(x, vl);
}

static MIMO_ALWAYS_INLINE vint32m1_t zf4_i16_complex_im_to_i32(
    const mimo_c16_t v[restrict MIMO_SITE4_LANES],
    size_t vl)
{
    const vint16mf2_t x =
        __riscv_vlse16_v_i16mf2(&v[0].i, sizeof(mimo_c16_t), vl);
    return __riscv_vsext_vf2_i32m1(x, vl);
}

static MIMO_ALWAYS_INLINE vint32m1_t zf4_mmse_lambda_sat_i32(
    vint32m1_t x,
    size_t vl)
{
    x = __riscv_vadd_vx_i32m1(x, MIMO_MMSE_LAMBDA_Q15, vl);
#if MIMO_USE_EXT_MMSE4_RVV_I32_ASSUME_LAMBDA_NOSAT
    return x;
#else
    x = __riscv_vmin_vx_i32m1(x, 32767, vl);
    return __riscv_vmax_vx_i32m1(x, -32768, vl);
#endif
}

#if MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_LOAD
static MIMO_ALWAYS_INLINE vint32m1_t zf4_i16_complex_pack32_load(
    const mimo_c16_t v[restrict MIMO_SITE4_LANES],
    size_t vl)
{
    return __riscv_vle32_v_i32m1((const int32_t *)v, vl);
}

static MIMO_ALWAYS_INLINE vint32m1_t zf4_i16_pack32_re_to_i32(
    vint32m1_t packed,
    size_t vl)
{
    return __riscv_vsra_vx_i32m1(
        __riscv_vsll_vx_i32m1(packed, 16, vl), 16, vl);
}

static MIMO_ALWAYS_INLINE vint32m1_t zf4_i16_pack32_im_to_i32(
    vint32m1_t packed,
    size_t vl)
{
    return __riscv_vsra_vx_i32m1(packed, 16, vl);
}

static MIMO_ALWAYS_INLINE vint32m1_t zf4_i16_complex_pack32_re_to_i32(
    const mimo_c16_t v[restrict MIMO_SITE4_LANES],
    size_t vl)
{
    return zf4_i16_pack32_re_to_i32(zf4_i16_complex_pack32_load(v, vl), vl);
}

static MIMO_ALWAYS_INLINE void zf4_i16_complex_pack32_to_i32(
    const mimo_c16_t v[restrict MIMO_SITE4_LANES],
    vint32m1_t *restrict re,
    vint32m1_t *restrict im,
    size_t vl)
{
    const vint32m1_t packed = zf4_i16_complex_pack32_load(v, vl);

    *re = zf4_i16_pack32_re_to_i32(packed, vl);
    *im = zf4_i16_pack32_im_to_i32(packed, vl);
}
#endif

static MIMO_ALWAYS_INLINE vint32m1_t zf4_round_q30_i32_to_q15(
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

static MIMO_ALWAYS_INLINE vint32m1_t zf4_det_q30_i32(
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

static MIMO_ALWAYS_INLINE vint32m1_t zf4_scale_from_det_q30_i32(
    vint32m1_t den_q30,
    size_t vl)
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
        den_q15, (int32_t)MIMO_SCHUR4_DET_Q15_RECIP_MIN, vl);
    const vuint32m1_t idx = __riscv_vsll_vx_u32m1(
        __riscv_vreinterpret_v_i32m1_u32m1(rel), 2, vl);

    return __riscv_vluxei32_v_i32m1(
        (const int32_t *)schur4_det_q15_recip_direct1_q30, idx, vl);
}

#if MIMO_EXT_MMSE4_PACKED_DET_EXT
static MIMO_ALWAYS_INLINE vint32m1_t zf4_scale_from_det_q15_i32(
    vint32m1_t den_q15,
    size_t vl)
{
    const vint32m1_t rel = __riscv_vsub_vx_i32m1(
        den_q15, (int32_t)MIMO_SCHUR4_DET_Q15_RECIP_MIN, vl);
    const vuint32m1_t idx = __riscv_vsll_vx_u32m1(
        __riscv_vreinterpret_v_i32m1_u32m1(rel), 2, vl);

    return __riscv_vluxei32_v_i32m1(
        (const int32_t *)schur4_det_q15_recip_direct1_q30, idx, vl);
}

#if MIMO_USE_EXT_MMSE4_PACKED_E16_RECIP
static MIMO_ALWAYS_INLINE vint32m1_t
zf4_scale_q26_packed_from_det_q15_e16(vint32m1_t den_q15)
{
    vint32m1_t byte_index, scale_q26;

    asm volatile(
        "vsll.vi %[index], %[den], 1\n\t"
        "vluxei16.v %[scale], (%[table]), %[index]\n\t"
        : [index] "=&vr"(byte_index), [scale] "=&vr"(scale_q26)
        : [den] "vr"(den_q15),
          [table] "r"(schur4_det_q15_recip_direct1_q26_padded)
        : "memory");

    return scale_q26;
}

#if MIMO_EXT_MMSE4_PACKED_S_RECIP_MODE == 2
static MIMO_ALWAYS_INLINE vint32m1_t
zf4_scale_q26_packed_nr1(vint32m1_t den_q15,
                         vint32m1_t seed_q26,
                         vint32m1_t one_q11)
{
    vint32m1_t product_q11, error_q11, correction_q26, scale_q26;

    asm volatile(
        "mimo_emit_vscmul_vv %[product], %[den], %[seed]\n\t"
        "vsub.vv %[error], %[one], %[product]\n\t"
        "mimo_emit_vscmul_vv %[correction], %[seed], %[error]\n\t"
        "vsll.vi %[correction], %[correction], 4\n\t"
        "vadd.vv %[scale], %[seed], %[correction]\n\t"
        : [product] "=&vr"(product_q11), [error] "=&vr"(error_q11),
          [correction] "=&vr"(correction_q26),
          [scale] "=&vr"(scale_q26)
        : [den] "vr"(den_q15), [seed] "vr"(seed_q26),
          [one] "vr"(one_q11)
        : "memory");

    return scale_q26;
}

static MIMO_ALWAYS_INLINE vint32m1_t
zf4_recip_one_q11_packed(vint32m1_t lambda_packed)
{
    vint32m1_t one_q11;

    asm volatile(
        "vsll.vi %[one], %[lambda], 2\n\t"
        : [one] "=&vr"(one_q11)
        : [lambda] "vr"(lambda_packed)
        : "memory");

    return one_q11;
}
#endif
#endif

static MIMO_ALWAYS_INLINE vint32m1_t zf4_det_q15_packed_ext(
    vint32m1_t a00p,
    vint32m1_t a01p,
    vint32m1_t a11p,
    int input_is_e16)
{
    vint32m1_t diag, mag;

    if (!input_is_e16) {
        asm volatile(
            "vsetivli zero, 8, e16, m1, ta, ma\n\t"
            :
            :
            : "memory");
    }
    asm volatile(
        "mimo_emit_vscmul_vv %[diag], %[a00], %[a11]\n\t"
        "mimo_emit_vscmulcj_vv %[mag], %[a01], %[a01]\n\t"
        "vsub.vv %[diag], %[diag], %[mag]\n\t"
        : [diag] "=&vr"(diag), [mag] "=&vr"(mag)
        : [a00] "vr"(a00p), [a01] "vr"(a01p), [a11] "vr"(a11p)
        : "memory");

    return diag;
}
#endif

static MIMO_ALWAYS_INLINE vint32m1_t zf4_scale_q30_to_q15_i32(
    vint32m1_t num_q30,
    vint32m1_t scale_q30,
    size_t vl)
{
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
}

static MIMO_ALWAYS_INLINE vint32m1_t zf4_scale_q30_to_q15_i32_trunc(
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

static MIMO_ALWAYS_INLINE void zf4_solve2_i32(
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
    int signed_trunc_scale,
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

#if MIMO_USE_MMSE4_SCHUR_SIGNED_TRUNC_SCALE
    (void)signed_trunc_scale;
    *out0r = zf4_scale_q30_to_q15_i32_trunc(num0r, scale_q30, vl);
    *out0i = zf4_scale_q30_to_q15_i32_trunc(num0i, scale_q30, vl);
    *out1r = zf4_scale_q30_to_q15_i32_trunc(num1r, scale_q30, vl);
    *out1i = zf4_scale_q30_to_q15_i32_trunc(num1i, scale_q30, vl);
#else
    if (signed_trunc_scale) {
        *out0r = zf4_scale_q30_to_q15_i32_trunc(num0r, scale_q30, vl);
        *out0i = zf4_scale_q30_to_q15_i32_trunc(num0i, scale_q30, vl);
        *out1r = zf4_scale_q30_to_q15_i32_trunc(num1r, scale_q30, vl);
        *out1i = zf4_scale_q30_to_q15_i32_trunc(num1i, scale_q30, vl);
    } else {
        *out0r = zf4_scale_q30_to_q15_i32(num0r, scale_q30, vl);
        *out0i = zf4_scale_q30_to_q15_i32(num0i, scale_q30, vl);
        *out1r = zf4_scale_q30_to_q15_i32(num1r, scale_q30, vl);
        *out1i = zf4_scale_q30_to_q15_i32(num1i, scale_q30, vl);
    }
#endif
}

#if MIMO_USE_EXT_MMSE4_RVV_I32_BINV_REUSE
static MIMO_ALWAYS_INLINE vint32m1_t zf4_inv2_coeff_q13_i32(
    vint32m1_t a_q15,
    vint32m1_t scale_q30,
    size_t vl)
{
    const vint32m1_t scale_q26 =
        __riscv_vsra_vx_i32m1(scale_q30, 4, vl);
    const vint32m1_t prod_q41 =
        __riscv_vmul_vv_i32m1(a_q15, scale_q26, vl);

#if MIMO_USE_RVV_SCHUR4_VSSRA_ROUND
    return __riscv_vssra_vx_i32m1(prod_q41, 13, 0, vl);
#else
    return __riscv_vsra_vx_i32m1(
        __riscv_vadd_vx_i32m1(prod_q41, 1 << 12, vl), 13, vl);
#endif
}

#if MIMO_EXT_MMSE4_PACKED_INVCOEFF_EXT
static MIMO_ALWAYS_INLINE void zf4_inv2_coeff_q13_packed_ext(
    vint32m1_t a00p,
    vint32m1_t a01p,
    vint32m1_t a11p,
    vint32m1_t scale_q30,
    vint32m1_t *inv00p_o,
    vint32m1_t *inv01p_o,
    vint32m1_t *inv11p_o)
{
#if MIMO_USE_EXT_MMSE4_PACKED_E16_RECIP
    vint32m1_t inv00p, inv01p, inv11p;

    asm volatile(
        "mimo_emit_vscmul_vv %[inv00], %[a11], %[scale]\n\t"
        "mimo_emit_vscmul_vv %[inv01], %[a01], %[scale]\n\t"
        "mimo_emit_vscmul_vv %[inv11], %[a00], %[scale]\n\t"
        "vsll.vi %[inv00], %[inv00], 2\n\t"
        "vsll.vi %[inv01], %[inv01], 2\n\t"
        "vsll.vi %[inv11], %[inv11], 2\n\t"
        "vrsub.vi %[inv01], %[inv01], 0\n\t"
        : [inv00] "=&vr"(inv00p), [inv01] "=&vr"(inv01p),
          [inv11] "=&vr"(inv11p)
        : [scale] "vr"(scale_q30), [a00] "vr"(a00p),
          [a01] "vr"(a01p), [a11] "vr"(a11p)
        : "memory");
#else
    vint32m1_t scale_q26, inv00p, inv01p, inv11p;

    asm volatile(
        "vsra.vi %[scale], %[scale_q30], 4\n\t"
        "vsetivli zero, 8, e16, m1, ta, ma\n\t"
        "mimo_emit_vscmul_vv %[inv00], %[a11], %[scale]\n\t"
        "mimo_emit_vscmul_vv %[inv01], %[a01], %[scale]\n\t"
        "mimo_emit_vscmul_vv %[inv11], %[a00], %[scale]\n\t"
        "vsll.vi %[inv00], %[inv00], 2\n\t"
        "vsll.vi %[inv01], %[inv01], 2\n\t"
        "vsll.vi %[inv11], %[inv11], 2\n\t"
        "vrsub.vi %[inv01], %[inv01], 0\n\t"
#if !MIMO_USE_EXT_MMSE4_PACKED_VSET_CHAIN
        "vsetivli zero, 4, e32, m1, ta, ma\n\t"
#endif
        : [scale] "=&vr"(scale_q26), [inv00] "=&vr"(inv00p),
          [inv01] "=&vr"(inv01p), [inv11] "=&vr"(inv11p)
        : [scale_q30] "vr"(scale_q30), [a00] "vr"(a00p),
          [a01] "vr"(a01p), [a11] "vr"(a11p)
        : "memory");
#endif

    *inv00p_o = inv00p;
    *inv01p_o = inv01p;
    *inv11p_o = inv11p;
}
#endif

static MIMO_ALWAYS_INLINE vint32m1_t zf4_round_q28_i32_to_q15(
    vint32m1_t x,
    size_t vl)
{
#if MIMO_USE_RVV_SCHUR4_VSSRA_ROUND
    return __riscv_vssra_vx_i32m1(x, 13, 0, vl);
#else
    x = __riscv_vadd_vx_i32m1(x, 1 << 12, vl);
    return __riscv_vsra_vx_i32m1(x, 13, vl);
#endif
}

static MIMO_ALWAYS_INLINE void zf4_apply_inv2_q13_i32(
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

    *out0r = zf4_round_q28_i32_to_q15(num0r, vl);
    *out0i = zf4_round_q28_i32_to_q15(num0i, vl);
    *out1r = zf4_round_q28_i32_to_q15(num1r, vl);
    *out1i = zf4_round_q28_i32_to_q15(num1i, vl);
}
#endif

static MIMO_ALWAYS_INLINE void zf4_cmul_q15_i32(
    vint32m1_t ar,
    vint32m1_t ai,
    vint32m1_t br,
    vint32m1_t bi,
    vint32m1_t *out_r,
    vint32m1_t *out_i,
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

    *out_r = zf4_round_q30_i32_to_q15(re, vl);
    *out_i = zf4_round_q30_i32_to_q15(im, vl);
}

static MIMO_ALWAYS_INLINE void zf4_cmul_conj_q15_i32(
    vint32m1_t ar,
    vint32m1_t ai,
    vint32m1_t br,
    vint32m1_t bi,
    vint32m1_t *out_r,
    vint32m1_t *out_i,
    size_t vl)
{
#if MIMO_USE_RVV_SCHUR4_COMPLEX_MAC
    vint32m1_t re = __riscv_vmul_vv_i32m1(ar, br, vl);
    vint32m1_t im = __riscv_vmul_vv_i32m1(ar, bi, vl);

    re = __riscv_vmacc_vv_i32m1(re, ai, bi, vl);
    im = __riscv_vnmsac_vv_i32m1(im, ai, br, vl);
#else
    vint32m1_t re = __riscv_vmul_vv_i32m1(ar, br, vl);
    vint32m1_t tmp = __riscv_vmul_vv_i32m1(ai, bi, vl);

    re = __riscv_vadd_vv_i32m1(re, tmp, vl);

    vint32m1_t im = __riscv_vmul_vv_i32m1(ar, bi, vl);
    tmp = __riscv_vmul_vv_i32m1(ai, br, vl);
    im = __riscv_vsub_vv_i32m1(im, tmp, vl);
#endif

    *out_r = zf4_round_q30_i32_to_q15(re, vl);
    *out_i = zf4_round_q30_i32_to_q15(im, vl);
}

static MIMO_ALWAYS_INLINE void zf4_inner_conj2_q15_i32(
    vint32m1_t e0r,
    vint32m1_t e0i,
    vint32m1_t e1r,
    vint32m1_t e1i,
    vint32m1_t w0r,
    vint32m1_t w0i,
    vint32m1_t w1r,
    vint32m1_t w1i,
    vint32m1_t *out_r,
    vint32m1_t *out_i,
    size_t vl)
{
#if MIMO_USE_EXT_MMSE4_RVV_I32_INNER_Q30_ONCE || \
    MIMO_USE_MMSE4_SCHUR_INNER_Q30_ONCE
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

    *out_r = zf4_round_q30_i32_to_q15(re, vl);
    *out_i = zf4_round_q30_i32_to_q15(im, vl);
#else
    vint32m1_t p0r, p0i, p1r, p1i;

    zf4_cmul_conj_q15_i32(e0r, e0i, w0r, w0i, &p0r, &p0i, vl);
    zf4_cmul_conj_q15_i32(e1r, e1i, w1r, w1i, &p1r, &p1i, vl);
    *out_r = __riscv_vadd_vv_i32m1(p0r, p1r, vl);
    *out_i = __riscv_vadd_vv_i32m1(p0i, p1i, vl);
#endif
}

static MIMO_ALWAYS_INLINE void zf4_submul_q15_i32(
    vint32m1_t *acc_r,
    vint32m1_t *acc_i,
    vint32m1_t ar,
    vint32m1_t ai,
    vint32m1_t br,
    vint32m1_t bi,
    size_t vl)
{
    vint32m1_t pr, pi;

    zf4_cmul_q15_i32(ar, ai, br, bi, &pr, &pi, vl);
    *acc_r = __riscv_vsub_vv_i32m1(*acc_r, pr, vl);
    *acc_i = __riscv_vsub_vv_i32m1(*acc_i, pi, vl);
}

static MIMO_ALWAYS_INLINE void zf4_submul2_q30_once_i32(
    vint32m1_t *acc_r,
    vint32m1_t *acc_i,
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

    *acc_r = __riscv_vsub_vv_i32m1(
        *acc_r, zf4_round_q30_i32_to_q15(re, vl), vl);
    *acc_i = __riscv_vsub_vv_i32m1(
        *acc_i, zf4_round_q30_i32_to_q15(im, vl), vl);
}

static MIMO_ALWAYS_INLINE vint32m1_t zf4_pack_complex_i32(
    vint32m1_t re,
    vint32m1_t im,
    size_t vl)
{
    const vint32m1_t re_lo = __riscv_vand_vx_i32m1(re, 0xffff, vl);
    const vint32m1_t im_hi = __riscv_vsll_vx_i32m1(im, 16, vl);
    return __riscv_vor_vv_i32m1(re_lo, im_hi, vl);
}

#if MIMO_USE_EXT_MMSE4_RVV_I32_PACKED_PHASE
static MIMO_ALWAYS_INLINE vint32m1_t zf4_pack_real_i32(
    vint32m1_t re,
    size_t vl)
{
    return __riscv_vand_vx_i32m1(re, 0xffff, vl);
}

static MIMO_ALWAYS_INLINE void zf4_ext_binv_inner5_packed_q15(
    vint32m1_t inv00,
    vint32m1_t inv01,
    vint32m1_t inv11,
    vint32m1_t a02,
    vint32m1_t a12,
    vint32m1_t a03,
    vint32m1_t a13,
    vint32m1_t b0,
    vint32m1_t b1,
    vint32m1_t a22,
    vint32m1_t a23,
    vint32m1_t a33,
    vint32m1_t b2,
    vint32m1_t b3,
    vint32m1_t *w00_o,
    vint32m1_t *w10_o,
    vint32m1_t *w01_o,
    vint32m1_t *w11_o,
    vint32m1_t *y0_o,
    vint32m1_t *y1_o,
    vint32m1_t *s00_o,
    vint32m1_t *s01_o,
    vint32m1_t *s11_o,
    vint32m1_t *r0_o,
    vint32m1_t *r1_o)
{
    vint32m1_t w00, w10, w01, w11, y0, y1;
    vint32m1_t s00, s01, s11, r0, r1;

    asm volatile(
#if !(MIMO_USE_EXT_MMSE4_PACKED_VSET_CHAIN && \
      MIMO_EXT_MMSE4_PACKED_INVCOEFF_EXT)
        "vsetivli zero, 8, e16, m1, ta, ma\n\t"
#endif

#if MIMO_EXT_MMSE4_PACKED_SOLVE_SEED
        "mimo_emit_vscmul_vv %[w00], %[inv00], %[a02]\n\t"
        "mimo_emit_vscmulcj_vv %[w10], %[inv01], %[a02]\n\t"
        "mimo_emit_vscmul_vv %[w01], %[inv00], %[a03]\n\t"
        "mimo_emit_vscmulcj_vv %[w11], %[inv01], %[a03]\n\t"
        "mimo_emit_vscmul_vv %[y0], %[inv00], %[b0]\n\t"
        "mimo_emit_vscmulcj_vv %[y1], %[inv01], %[b0]\n\t"
#else
        "vmv.v.i %[w00], 0\n\t"
        "vmv.v.i %[w10], 0\n\t"
        "vmv.v.i %[w01], 0\n\t"
        "vmv.v.i %[w11], 0\n\t"
        "vmv.v.i %[y0], 0\n\t"
        "vmv.v.i %[y1], 0\n\t"
        "mimo_emit_vscmacc_vv %[w00], %[inv00], %[a02]\n\t"
        "mimo_emit_vscmacccj_vv %[w10], %[inv01], %[a02]\n\t"
        "mimo_emit_vscmacc_vv %[w01], %[inv00], %[a03]\n\t"
        "mimo_emit_vscmacccj_vv %[w11], %[inv01], %[a03]\n\t"
        "mimo_emit_vscmacc_vv %[y0], %[inv00], %[b0]\n\t"
        "mimo_emit_vscmacccj_vv %[y1], %[inv01], %[b0]\n\t"
#endif
        "mimo_emit_vscmacc_vv %[w00], %[inv01], %[a12]\n\t"
        "mimo_emit_vscmacc_vv %[w10], %[inv11], %[a12]\n\t"
        "mimo_emit_vscmacc_vv %[w01], %[inv01], %[a13]\n\t"
        "mimo_emit_vscmacc_vv %[w11], %[inv11], %[a13]\n\t"
        "mimo_emit_vscmacc_vv %[y0], %[inv01], %[b1]\n\t"
        "mimo_emit_vscmacc_vv %[y1], %[inv11], %[b1]\n\t"
        "vsll.vi %[w00], %[w00], 2\n\t"
        "vsll.vi %[w10], %[w10], 2\n\t"
        "vsll.vi %[w01], %[w01], 2\n\t"
        "vsll.vi %[w11], %[w11], 2\n\t"
        "vsll.vi %[y0], %[y0], 2\n\t"
        "vsll.vi %[y1], %[y1], 2\n\t"

#if MIMO_EXT_MMSE4_PACKED_SOLVE_SEED
        "mimo_emit_vscmulcj_vv %[s00], %[a02], %[w00]\n\t"
        "mimo_emit_vscmulcj_vv %[s01], %[a02], %[w01]\n\t"
        "mimo_emit_vscmulcj_vv %[s11], %[a03], %[w01]\n\t"
        "mimo_emit_vscmulcj_vv %[r0], %[a02], %[y0]\n\t"
        "mimo_emit_vscmulcj_vv %[r1], %[a03], %[y0]\n\t"
#else
        "vmv.v.i %[s00], 0\n\t"
        "vmv.v.i %[s01], 0\n\t"
        "vmv.v.i %[s11], 0\n\t"
        "vmv.v.i %[r0], 0\n\t"
        "vmv.v.i %[r1], 0\n\t"
        "mimo_emit_vscmacccj_vv %[s00], %[a02], %[w00]\n\t"
        "mimo_emit_vscmacccj_vv %[s01], %[a02], %[w01]\n\t"
        "mimo_emit_vscmacccj_vv %[s11], %[a03], %[w01]\n\t"
        "mimo_emit_vscmacccj_vv %[r0], %[a02], %[y0]\n\t"
        "mimo_emit_vscmacccj_vv %[r1], %[a03], %[y0]\n\t"
#endif
        "mimo_emit_vscmacccj_vv %[s00], %[a12], %[w10]\n\t"
        "mimo_emit_vscmacccj_vv %[s01], %[a12], %[w11]\n\t"
        "mimo_emit_vscmacccj_vv %[s11], %[a13], %[w11]\n\t"
        "mimo_emit_vscmacccj_vv %[r0], %[a12], %[y1]\n\t"
        "mimo_emit_vscmacccj_vv %[r1], %[a13], %[y1]\n\t"
        "vsub.vv %[s00], %[a22], %[s00]\n\t"
        "vsub.vv %[s01], %[a23], %[s01]\n\t"
        "vsub.vv %[s11], %[a33], %[s11]\n\t"
        "vsub.vv %[r0], %[b2], %[r0]\n\t"
        "vsub.vv %[r1], %[b3], %[r1]\n\t"

#if !MIMO_USE_EXT_MMSE4_PACKED_VSET_CHAIN
        "vsetivli zero, 4, e32, m1, ta, ma\n\t"
#endif
        : [w00] "=&vr"(w00), [w10] "=&vr"(w10),
          [w01] "=&vr"(w01), [w11] "=&vr"(w11),
          [y0] "=&vr"(y0), [y1] "=&vr"(y1),
          [s00] "=&vr"(s00), [s01] "=&vr"(s01),
          [s11] "=&vr"(s11), [r0] "=&vr"(r0), [r1] "=&vr"(r1)
        : [inv00] "vr"(inv00), [inv01] "vr"(inv01),
          [inv11] "vr"(inv11), [a02] "vr"(a02), [a12] "vr"(a12),
          [a03] "vr"(a03), [a13] "vr"(a13), [b0] "vr"(b0),
          [b1] "vr"(b1), [a22] "vr"(a22), [a23] "vr"(a23),
          [a33] "vr"(a33), [b2] "vr"(b2), [b3] "vr"(b3)
        : "memory");

    *w00_o = w00;
    *w10_o = w10;
    *w01_o = w01;
    *w11_o = w11;
    *y0_o = y0;
    *y1_o = y1;
    *s00_o = s00;
    *s01_o = s01;
    *s11_o = s11;
    *r0_o = r0;
    *r1_o = r1;
}

static MIMO_ALWAYS_INLINE void zf4_ext_top2_packed_q15(
    vint32m1_t w00,
    vint32m1_t w10,
    vint32m1_t w01,
    vint32m1_t w11,
    vint32m1_t y0,
    vint32m1_t y1,
    vint32m1_t x2,
    vint32m1_t x3,
    vint32m1_t *top0_o,
    vint32m1_t *top1_o)
{
    vint32m1_t top0, top1;

    asm volatile(
        "vsetivli zero, 8, e16, m1, ta, ma\n\t"
#if MIMO_EXT_MMSE4_PACKED_SOLVE_SEED
        "mimo_emit_vscmul_vv %[top0], %[w00], %[x2]\n\t"
        "mimo_emit_vscmul_vv %[top1], %[w10], %[x2]\n\t"
#else
        "vmv.v.i %[top0], 0\n\t"
        "vmv.v.i %[top1], 0\n\t"
        "mimo_emit_vscmacc_vv %[top0], %[w00], %[x2]\n\t"
        "mimo_emit_vscmacc_vv %[top1], %[w10], %[x2]\n\t"
#endif
        "mimo_emit_vscmacc_vv %[top0], %[w01], %[x3]\n\t"
        "mimo_emit_vscmacc_vv %[top1], %[w11], %[x3]\n\t"
        "vsub.vv %[top0], %[y0], %[top0]\n\t"
        "vsub.vv %[top1], %[y1], %[top1]\n\t"
#if !MIMO_USE_EXT_MMSE4_PACKED_VSET_CHAIN
        "vsetivli zero, 4, e32, m1, ta, ma\n\t"
#endif
        : [top0] "=&vr"(top0), [top1] "=&vr"(top1)
        : [w00] "vr"(w00), [w10] "vr"(w10), [w01] "vr"(w01),
          [w11] "vr"(w11), [y0] "vr"(y0), [y1] "vr"(y1),
          [x2] "vr"(x2), [x3] "vr"(x3)
        : "memory");

    *top0_o = top0;
    *top1_o = top1;
}

#if MIMO_EXT_MMSE4_PACKED_SSOLVE_EXT
static MIMO_ALWAYS_INLINE void zf4_ext_ssolve_top_packed_q15(
    vint32m1_t inv00,
    vint32m1_t inv01,
    vint32m1_t inv11,
    vint32m1_t r0,
    vint32m1_t r1,
    vint32m1_t w00,
    vint32m1_t w10,
    vint32m1_t w01,
    vint32m1_t w11,
    vint32m1_t y0,
    vint32m1_t y1,
    vint32m1_t *top0_o,
    vint32m1_t *top1_o,
    vint32m1_t *x2_o,
    vint32m1_t *x3_o)
{
    vint32m1_t top0, top1, x2, x3;

    asm volatile(
#if !(MIMO_USE_EXT_MMSE4_PACKED_VSET_CHAIN && \
      MIMO_EXT_MMSE4_PACKED_INVCOEFF_EXT)
        "vsetivli zero, 8, e16, m1, ta, ma\n\t"
#endif
#if MIMO_EXT_MMSE4_PACKED_SOLVE_SEED
        "mimo_emit_vscmul_vv %[x2], %[inv00], %[r0]\n\t"
        "mimo_emit_vscmulcj_vv %[x3], %[inv01], %[r0]\n\t"
#else
        "vmv.v.i %[x2], 0\n\t"
        "vmv.v.i %[x3], 0\n\t"
        "mimo_emit_vscmacc_vv %[x2], %[inv00], %[r0]\n\t"
        "mimo_emit_vscmacccj_vv %[x3], %[inv01], %[r0]\n\t"
#endif
        "mimo_emit_vscmacc_vv %[x2], %[inv01], %[r1]\n\t"
        "mimo_emit_vscmacc_vv %[x3], %[inv11], %[r1]\n\t"
        "vsll.vi %[x2], %[x2], 2\n\t"
        "vsll.vi %[x3], %[x3], 2\n\t"
#if MIMO_EXT_MMSE4_PACKED_SOLVE_SEED
        "mimo_emit_vscmul_vv %[top0], %[w00], %[x2]\n\t"
        "mimo_emit_vscmul_vv %[top1], %[w10], %[x2]\n\t"
#else
        "vmv.v.i %[top0], 0\n\t"
        "vmv.v.i %[top1], 0\n\t"
        "mimo_emit_vscmacc_vv %[top0], %[w00], %[x2]\n\t"
        "mimo_emit_vscmacc_vv %[top1], %[w10], %[x2]\n\t"
#endif
        "mimo_emit_vscmacc_vv %[top0], %[w01], %[x3]\n\t"
        "mimo_emit_vscmacc_vv %[top1], %[w11], %[x3]\n\t"
        "vsub.vv %[top0], %[y0], %[top0]\n\t"
        "vsub.vv %[top1], %[y1], %[top1]\n\t"
#if !MIMO_USE_EXT_MMSE4_PACKED_VSET_CHAIN
        "vsetivli zero, 4, e32, m1, ta, ma\n\t"
#endif
        : [top0] "=&vr"(top0), [top1] "=&vr"(top1),
          [x2] "=&vr"(x2), [x3] "=&vr"(x3)
        : [inv00] "vr"(inv00), [inv01] "vr"(inv01),
          [inv11] "vr"(inv11), [r0] "vr"(r0), [r1] "vr"(r1),
          [w00] "vr"(w00), [w10] "vr"(w10), [w01] "vr"(w01),
          [w11] "vr"(w11), [y0] "vr"(y0), [y1] "vr"(y1)
        : "memory");

    *top0_o = top0;
    *top1_o = top1;
    *x2_o = x2;
    *x3_o = x3;
}
#endif

#if MIMO_USE_EXT_MMSE4_PACKED_LIVE_BUILD
static MIMO_ALWAYS_INLINE void zf4_ext_build_live_packed_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    vint32m1_t *a00_o,
    vint32m1_t *a01_o,
    vint32m1_t *a02_o,
    vint32m1_t *a03_o,
    vint32m1_t *a11_o,
    vint32m1_t *a12_o,
    vint32m1_t *a13_o,
    vint32m1_t *a22_o,
    vint32m1_t *a23_o,
    vint32m1_t *a33_o,
    vint32m1_t *b0_o,
    vint32m1_t *b1_o,
    vint32m1_t *b2_o,
    vint32m1_t *b3_o)
{
    const size_t h_base =
        block * MIMO_N4 * MIMO_N4 * MIMO_SITE4_LANES;
    const size_t h_col_stride = MIMO_N4 * MIMO_SITE4_LANES;
#if MIMO_USE_EXT_MMSE4_PACKED_FIXED_PTRS
    register const mimo_c16_t *p0 asm("a6") = &h_site4[h_base];
    register const mimo_c16_t *p1 asm("a7") =
        &h_site4[h_base + h_col_stride];
    register const mimo_c16_t *p2 asm("t4") =
        &h_site4[h_base + 2 * h_col_stride];
    register const mimo_c16_t *p3 asm("t6") =
        &h_site4[h_base + 3 * h_col_stride];
    register const mimo_c16_t *py asm("t2") =
        &y_site4[block * MIMO_N4 * MIMO_SITE4_LANES];
#else
    const mimo_c16_t *p0 = &h_site4[h_base];
    const mimo_c16_t *p1 = &h_site4[h_base + h_col_stride];
    const mimo_c16_t *p2 = &h_site4[h_base + 2 * h_col_stride];
    const mimo_c16_t *p3 = &h_site4[h_base + 3 * h_col_stride];
    const mimo_c16_t *py =
        &y_site4[block * MIMO_N4 * MIMO_SITE4_LANES];
#endif
#if MIMO_USE_EXT_MMSE4_PACKED_RX4_M4_LOAD
    register vint32m1_t a00 asm("v16");
    register vint32m1_t a01 asm("v17");
    register vint32m1_t a02 asm("v18");
    register vint32m1_t a03 asm("v19");
    register vint32m1_t b0 asm("v20");
    register vint32m1_t b1 asm("v21");
    register vint32m1_t b2 asm("v22");
    register vint32m1_t b3 asm("v23");
    register vint32m1_t a11 asm("v24");
    register vint32m1_t a12 asm("v25");
    register vint32m1_t a13 asm("v26");
    register vint32m1_t a22 asm("v27");
    register vint32m1_t a23 asm("v28");
    register vint32m1_t a33 asm("v29");
#else
    vint32m1_t a00, a01, a02, a03, a11, a12, a13;
    vint32m1_t a22, a23, a33, b0, b1, b2, b3;
#endif

#if MIMO_USE_EXT_MMSE4_PACKED_RX4_M4_LOAD
    asm volatile(
        "vsetvli t0, zero, e16, m4, ta, ma\n\t"
        "vle16.v v0, (%[p0])\n\t"
        "vle16.v v4, (%[p1])\n\t"
        "vle16.v v8, (%[p2])\n\t"
        "vle16.v v12, (%[p3])\n\t"
        "vle16.v v16, (%[py])\n\t"
        "vsetivli zero, 8, e16, m1, ta, ma\n\t"
        "mimo_emit_vscmulcj_vv %[b0], v0, v16\n\t"
        "mimo_emit_vscmacccj_vv %[b0], v1, v17\n\t"
        "mimo_emit_vscmacccj_vv %[b0], v2, v18\n\t"
        "mimo_emit_vscmacccj_vv %[b0], v3, v19\n\t"
        "mimo_emit_vscmulcj_vv %[b1], v4, v16\n\t"
        "mimo_emit_vscmacccj_vv %[b1], v5, v17\n\t"
        "mimo_emit_vscmacccj_vv %[b1], v6, v18\n\t"
        "mimo_emit_vscmacccj_vv %[b1], v7, v19\n\t"
        "mimo_emit_vscmulcj_vv %[b2], v8, v16\n\t"
        "mimo_emit_vscmacccj_vv %[b2], v9, v17\n\t"
        "mimo_emit_vscmacccj_vv %[b2], v10, v18\n\t"
        "mimo_emit_vscmacccj_vv %[b2], v11, v19\n\t"
        "mimo_emit_vscmulcj_vv %[b3], v12, v16\n\t"
        "mimo_emit_vscmacccj_vv %[b3], v13, v17\n\t"
        "mimo_emit_vscmacccj_vv %[b3], v14, v18\n\t"
        "mimo_emit_vscmacccj_vv %[b3], v15, v19\n\t"
        "mimo_emit_vscmulcj_vv %[a00], v0, v0\n\t"
        "mimo_emit_vscmulcj_vv %[a01], v0, v4\n\t"
        "mimo_emit_vscmulcj_vv %[a02], v0, v8\n\t"
        "mimo_emit_vscmulcj_vv %[a03], v0, v12\n\t"
        "mimo_emit_vscmulcj_vv %[a11], v4, v4\n\t"
        "mimo_emit_vscmulcj_vv %[a12], v4, v8\n\t"
        "mimo_emit_vscmulcj_vv %[a13], v4, v12\n\t"
        "mimo_emit_vscmulcj_vv %[a22], v8, v8\n\t"
        "mimo_emit_vscmulcj_vv %[a23], v8, v12\n\t"
        "mimo_emit_vscmulcj_vv %[a33], v12, v12\n\t"
        "mimo_emit_vscmacccj_vv %[a00], v1, v1\n\t"
        "mimo_emit_vscmacccj_vv %[a01], v1, v5\n\t"
        "mimo_emit_vscmacccj_vv %[a02], v1, v9\n\t"
        "mimo_emit_vscmacccj_vv %[a03], v1, v13\n\t"
        "mimo_emit_vscmacccj_vv %[a11], v5, v5\n\t"
        "mimo_emit_vscmacccj_vv %[a12], v5, v9\n\t"
        "mimo_emit_vscmacccj_vv %[a13], v5, v13\n\t"
        "mimo_emit_vscmacccj_vv %[a22], v9, v9\n\t"
        "mimo_emit_vscmacccj_vv %[a23], v9, v13\n\t"
        "mimo_emit_vscmacccj_vv %[a33], v13, v13\n\t"
        "mimo_emit_vscmacccj_vv %[a00], v2, v2\n\t"
        "mimo_emit_vscmacccj_vv %[a01], v2, v6\n\t"
        "mimo_emit_vscmacccj_vv %[a02], v2, v10\n\t"
        "mimo_emit_vscmacccj_vv %[a03], v2, v14\n\t"
        "mimo_emit_vscmacccj_vv %[a11], v6, v6\n\t"
        "mimo_emit_vscmacccj_vv %[a12], v6, v10\n\t"
        "mimo_emit_vscmacccj_vv %[a13], v6, v14\n\t"
        "mimo_emit_vscmacccj_vv %[a22], v10, v10\n\t"
        "mimo_emit_vscmacccj_vv %[a23], v10, v14\n\t"
        "mimo_emit_vscmacccj_vv %[a33], v14, v14\n\t"
        "mimo_emit_vscmacccj_vv %[a00], v3, v3\n\t"
        "mimo_emit_vscmacccj_vv %[a01], v3, v7\n\t"
        "mimo_emit_vscmacccj_vv %[a02], v3, v11\n\t"
        "mimo_emit_vscmacccj_vv %[a03], v3, v15\n\t"
        "mimo_emit_vscmacccj_vv %[a11], v7, v7\n\t"
        "mimo_emit_vscmacccj_vv %[a12], v7, v11\n\t"
        "mimo_emit_vscmacccj_vv %[a13], v7, v15\n\t"
        "mimo_emit_vscmacccj_vv %[a22], v11, v11\n\t"
        "mimo_emit_vscmacccj_vv %[a23], v11, v15\n\t"
        "mimo_emit_vscmacccj_vv %[a33], v15, v15\n\t"
        : [a00] "=&vr"(a00), [a01] "=&vr"(a01),
          [a02] "=&vr"(a02), [a03] "=&vr"(a03),
          [a11] "=&vr"(a11), [a12] "=&vr"(a12),
          [a13] "=&vr"(a13), [a22] "=&vr"(a22),
          [a23] "=&vr"(a23), [a33] "=&vr"(a33),
          [b0] "=&vr"(b0), [b1] "=&vr"(b1),
          [b2] "=&vr"(b2), [b3] "=&vr"(b3)
        : [p0] "r"(p0), [p1] "r"(p1), [p2] "r"(p2),
          [p3] "r"(p3), [py] "r"(py)
        : "t0", "v0", "v1", "v2", "v3",
          "v4", "v5", "v6", "v7", "v8", "v9",
          "v10", "v11", "v12", "v13", "v14", "v15",
          "memory");
#else
    asm volatile(
        "vsetivli zero, 8, e16, m1, ta, ma\n\t"
#if MIMO_USE_EXT_ZF4_VSCMACC_SEED_BUILD
        "vle16.v v20, (%[p0])\n\t"
        "vle16.v v22, (%[p1])\n\t"
        "vle16.v v24, (%[p2])\n\t"
        "vle16.v v26, (%[p3])\n\t"
        "vle16.v v28, (%[py])\n\t"
        "mimo_emit_vscmulcj_vv %[b0], v20, v28\n\t"
        "mimo_emit_vscmulcj_vv %[a00], v20, v20\n\t"
        "mimo_emit_vscmulcj_vv %[a01], v20, v22\n\t"
        "mimo_emit_vscmulcj_vv %[a02], v20, v24\n\t"
        "mimo_emit_vscmulcj_vv %[a03], v20, v26\n\t"
        "mimo_emit_vscmulcj_vv %[b1], v22, v28\n\t"
        "mimo_emit_vscmulcj_vv %[a11], v22, v22\n\t"
        "mimo_emit_vscmulcj_vv %[a12], v22, v24\n\t"
        "mimo_emit_vscmulcj_vv %[a13], v22, v26\n\t"
        "mimo_emit_vscmulcj_vv %[b2], v24, v28\n\t"
        "mimo_emit_vscmulcj_vv %[a22], v24, v24\n\t"
        "mimo_emit_vscmulcj_vv %[a23], v24, v26\n\t"
        "mimo_emit_vscmulcj_vv %[b3], v26, v28\n\t"
        "mimo_emit_vscmulcj_vv %[a33], v26, v26\n\t"
        "addi %[p0], %[p0], 16\n\t"
        "addi %[p1], %[p1], 16\n\t"
        "addi %[p2], %[p2], 16\n\t"
        "addi %[p3], %[p3], 16\n\t"
        "addi %[py], %[py], 16\n\t"
#if !MIMO_USE_EXT_MMSE4_PACKED_LIVE_BUILD_UNROLL4
        "li t0, 3\n\t"
#endif
#else
        "vmv.v.i %[a00], 0\n\t"
        "vmv.v.i %[a01], 0\n\t"
        "vmv.v.i %[a02], 0\n\t"
        "vmv.v.i %[a03], 0\n\t"
        "vmv.v.i %[a11], 0\n\t"
        "vmv.v.i %[a12], 0\n\t"
        "vmv.v.i %[a13], 0\n\t"
        "vmv.v.i %[a22], 0\n\t"
        "vmv.v.i %[a23], 0\n\t"
        "vmv.v.i %[a33], 0\n\t"
        "vmv.v.i %[b0], 0\n\t"
        "vmv.v.i %[b1], 0\n\t"
        "vmv.v.i %[b2], 0\n\t"
        "vmv.v.i %[b3], 0\n\t"
        "li t0, 4\n\t"
#endif
#if MIMO_USE_EXT_MMSE4_PACKED_LIVE_BUILD_UNROLL4
        "vle16.v v20, (%[p0])\n\t"
        "vle16.v v22, (%[p1])\n\t"
        "vle16.v v24, (%[p2])\n\t"
        "vle16.v v26, (%[p3])\n\t"
        "vle16.v v28, (%[py])\n\t"
        "mimo_emit_vscmacccj_vv %[b0], v20, v28\n\t"
        "mimo_emit_vscmacccj_vv %[a00], v20, v20\n\t"
        "mimo_emit_vscmacccj_vv %[a01], v20, v22\n\t"
        "mimo_emit_vscmacccj_vv %[a02], v20, v24\n\t"
        "mimo_emit_vscmacccj_vv %[a03], v20, v26\n\t"
        "mimo_emit_vscmacccj_vv %[b1], v22, v28\n\t"
        "mimo_emit_vscmacccj_vv %[a11], v22, v22\n\t"
        "mimo_emit_vscmacccj_vv %[a12], v22, v24\n\t"
        "mimo_emit_vscmacccj_vv %[a13], v22, v26\n\t"
        "mimo_emit_vscmacccj_vv %[b2], v24, v28\n\t"
        "mimo_emit_vscmacccj_vv %[a22], v24, v24\n\t"
        "mimo_emit_vscmacccj_vv %[a23], v24, v26\n\t"
        "mimo_emit_vscmacccj_vv %[b3], v26, v28\n\t"
        "mimo_emit_vscmacccj_vv %[a33], v26, v26\n\t"
        "addi %[p0], %[p0], 16\n\t"
        "addi %[p1], %[p1], 16\n\t"
        "addi %[p2], %[p2], 16\n\t"
        "addi %[p3], %[p3], 16\n\t"
        "addi %[py], %[py], 16\n\t"
        "vle16.v v20, (%[p0])\n\t"
        "vle16.v v22, (%[p1])\n\t"
        "vle16.v v24, (%[p2])\n\t"
        "vle16.v v26, (%[p3])\n\t"
        "vle16.v v28, (%[py])\n\t"
        "mimo_emit_vscmacccj_vv %[b0], v20, v28\n\t"
        "mimo_emit_vscmacccj_vv %[a00], v20, v20\n\t"
        "mimo_emit_vscmacccj_vv %[a01], v20, v22\n\t"
        "mimo_emit_vscmacccj_vv %[a02], v20, v24\n\t"
        "mimo_emit_vscmacccj_vv %[a03], v20, v26\n\t"
        "mimo_emit_vscmacccj_vv %[b1], v22, v28\n\t"
        "mimo_emit_vscmacccj_vv %[a11], v22, v22\n\t"
        "mimo_emit_vscmacccj_vv %[a12], v22, v24\n\t"
        "mimo_emit_vscmacccj_vv %[a13], v22, v26\n\t"
        "mimo_emit_vscmacccj_vv %[b2], v24, v28\n\t"
        "mimo_emit_vscmacccj_vv %[a22], v24, v24\n\t"
        "mimo_emit_vscmacccj_vv %[a23], v24, v26\n\t"
        "mimo_emit_vscmacccj_vv %[b3], v26, v28\n\t"
        "mimo_emit_vscmacccj_vv %[a33], v26, v26\n\t"
        "addi %[p0], %[p0], 16\n\t"
        "addi %[p1], %[p1], 16\n\t"
        "addi %[p2], %[p2], 16\n\t"
        "addi %[p3], %[p3], 16\n\t"
        "addi %[py], %[py], 16\n\t"
        "vle16.v v20, (%[p0])\n\t"
        "vle16.v v22, (%[p1])\n\t"
        "vle16.v v24, (%[p2])\n\t"
        "vle16.v v26, (%[p3])\n\t"
        "vle16.v v28, (%[py])\n\t"
        "mimo_emit_vscmacccj_vv %[b0], v20, v28\n\t"
        "mimo_emit_vscmacccj_vv %[a00], v20, v20\n\t"
        "mimo_emit_vscmacccj_vv %[a01], v20, v22\n\t"
        "mimo_emit_vscmacccj_vv %[a02], v20, v24\n\t"
        "mimo_emit_vscmacccj_vv %[a03], v20, v26\n\t"
        "mimo_emit_vscmacccj_vv %[b1], v22, v28\n\t"
        "mimo_emit_vscmacccj_vv %[a11], v22, v22\n\t"
        "mimo_emit_vscmacccj_vv %[a12], v22, v24\n\t"
        "mimo_emit_vscmacccj_vv %[a13], v22, v26\n\t"
        "mimo_emit_vscmacccj_vv %[b2], v24, v28\n\t"
        "mimo_emit_vscmacccj_vv %[a22], v24, v24\n\t"
        "mimo_emit_vscmacccj_vv %[a23], v24, v26\n\t"
        "mimo_emit_vscmacccj_vv %[b3], v26, v28\n\t"
        "mimo_emit_vscmacccj_vv %[a33], v26, v26\n\t"
#else
        "1:\n\t"
        "vle16.v v20, (%[p0])\n\t"
        "vle16.v v22, (%[p1])\n\t"
        "vle16.v v24, (%[p2])\n\t"
        "vle16.v v26, (%[p3])\n\t"
        "vle16.v v28, (%[py])\n\t"
        "mimo_emit_vscmacccj_vv %[b0], v20, v28\n\t"
        "mimo_emit_vscmacccj_vv %[a00], v20, v20\n\t"
        "mimo_emit_vscmacccj_vv %[a01], v20, v22\n\t"
        "mimo_emit_vscmacccj_vv %[a02], v20, v24\n\t"
        "mimo_emit_vscmacccj_vv %[a03], v20, v26\n\t"
        "mimo_emit_vscmacccj_vv %[b1], v22, v28\n\t"
        "mimo_emit_vscmacccj_vv %[a11], v22, v22\n\t"
        "mimo_emit_vscmacccj_vv %[a12], v22, v24\n\t"
        "mimo_emit_vscmacccj_vv %[a13], v22, v26\n\t"
        "mimo_emit_vscmacccj_vv %[b2], v24, v28\n\t"
        "mimo_emit_vscmacccj_vv %[a22], v24, v24\n\t"
        "mimo_emit_vscmacccj_vv %[a23], v24, v26\n\t"
        "mimo_emit_vscmacccj_vv %[b3], v26, v28\n\t"
        "mimo_emit_vscmacccj_vv %[a33], v26, v26\n\t"
        "addi %[p0], %[p0], 16\n\t"
        "addi %[p1], %[p1], 16\n\t"
        "addi %[p2], %[p2], 16\n\t"
        "addi %[p3], %[p3], 16\n\t"
        "addi %[py], %[py], 16\n\t"
        "addi t0, t0, -1\n\t"
        "bnez t0, 1b\n\t"
#endif
        : [a00] "=&vr"(a00), [a01] "=&vr"(a01),
          [a02] "=&vr"(a02), [a03] "=&vr"(a03),
          [a11] "=&vr"(a11), [a12] "=&vr"(a12),
          [a13] "=&vr"(a13), [a22] "=&vr"(a22),
          [a23] "=&vr"(a23), [a33] "=&vr"(a33),
          [b0] "=&vr"(b0), [b1] "=&vr"(b1),
          [b2] "=&vr"(b2), [b3] "=&vr"(b3),
          [p0] "+r"(p0), [p1] "+r"(p1), [p2] "+r"(p2),
          [p3] "+r"(p3), [py] "+r"(py)
        :
        : "t0", "v20", "v22", "v24", "v26", "v28", "memory");
#endif

    *a00_o = a00;
    *a01_o = a01;
    *a02_o = a02;
    *a03_o = a03;
    *a11_o = a11;
    *a12_o = a12;
    *a13_o = a13;
    *a22_o = a22;
    *a23_o = a23;
    *a33_o = a33;
    *b0_o = b0;
    *b1_o = b1;
    *b2_o = b2;
    *b3_o = b3;
}
#endif

#if MIMO_USE_EXT_MMSE4_PACKED_E16_LAMBDA
static MIMO_ALWAYS_INLINE vint32m1_t zf4_mmse_lambda_packed_e16(void)
{
    vint32m1_t lambda_packed;
    const int32_t lambda_q15 = MIMO_MMSE_LAMBDA_Q15;

    asm volatile(
        "vsetivli zero, 4, e32, m1, ta, ma\n\t"
        "vmv.v.x %[packed], %[lambda]\n\t"
        : [packed] "=&vr"(lambda_packed)
        : [lambda] "r"(lambda_q15)
        : "memory");

    return lambda_packed;
}
#endif

static MIMO_ALWAYS_INLINE void zf4_eq4_packed_vectors_q15(
    vint32m1_t a00p,
    vint32m1_t a01p,
    vint32m1_t a02p,
    vint32m1_t a03p,
    vint32m1_t a11p,
    vint32m1_t a12p,
    vint32m1_t a13p,
    vint32m1_t a22p,
    vint32m1_t a23p,
    vint32m1_t a33p,
    vint32m1_t b0p,
    vint32m1_t b1p,
    vint32m1_t b2p,
    vint32m1_t b3p,
#if MIMO_USE_EXT_MMSE4_PACKED_E16_LAMBDA
    vint32m1_t lambda_packed,
#endif
#if MIMO_EXT_MMSE4_PACKED_S_RECIP_MODE == 2
    vint32m1_t recip_one_q11,
#endif
    int mmse,
    mimo_c16_t *restrict x_block)
{
#if MIMO_USE_EXT_MMSE4_PACKED_E16_LAMBDA
    const size_t vl = MIMO_SITE4_LANES;

    if (mmse) {
        asm volatile(
            "vadd.vv %[a00], %[a00], %[lambda]\n\t"
            "vadd.vv %[a11], %[a11], %[lambda]\n\t"
            "vadd.vv %[a22], %[a22], %[lambda]\n\t"
            "vadd.vv %[a33], %[a33], %[lambda]\n\t"
            : [a00] "+vr"(a00p), [a11] "+vr"(a11p),
              [a22] "+vr"(a22p), [a33] "+vr"(a33p)
            : [lambda] "vr"(lambda_packed)
            : "memory");
    }
#else
    const size_t vl = __riscv_vsetvl_e32m1(MIMO_SITE4_LANES);
    if (mmse) {
        a22p = __riscv_vadd_vx_i32m1(a22p, MIMO_MMSE_LAMBDA_Q15, vl);
        a33p = __riscv_vadd_vx_i32m1(a33p, MIMO_MMSE_LAMBDA_Q15, vl);
#if MIMO_EXT_MMSE4_PACKED_INVCOEFF_EXT
        a00p = __riscv_vadd_vx_i32m1(a00p, MIMO_MMSE_LAMBDA_Q15, vl);
        a11p = __riscv_vadd_vx_i32m1(a11p, MIMO_MMSE_LAMBDA_Q15, vl);
#endif
    }
#endif
#if MIMO_EXT_MMSE4_PACKED_INVCOEFF_EXT
#if !MIMO_EXT_MMSE4_PACKED_DET_EXT
    const vint32m1_t a00r = zf4_i16_pack32_re_to_i32(a00p, vl);
    const vint32m1_t a11r = zf4_i16_pack32_re_to_i32(a11p, vl);
#endif
#else
    const vint32m1_t a00r = zf4_mmse_lambda_sat_i32(
        zf4_i16_pack32_re_to_i32(a00p, vl), vl);
    const vint32m1_t a11r = zf4_mmse_lambda_sat_i32(
        zf4_i16_pack32_re_to_i32(a11p, vl), vl);
#endif
#if MIMO_EXT_MMSE4_PACKED_DET_EXT
    const vint32m1_t b_det_q15 =
        zf4_det_q15_packed_ext(
            a00p, a01p, a11p,
            MIMO_USE_EXT_MMSE4_PACKED_E16_LAMBDA);
#if MIMO_USE_EXT_MMSE4_PACKED_E16_RECIP
    const vint32m1_t b_scale =
        zf4_scale_q26_packed_from_det_q15_e16(b_det_q15);
#else
    const vint32m1_t b_scale =
        zf4_scale_from_det_q15_i32(b_det_q15, vl);
#endif
#else
    const vint32m1_t a01r = zf4_i16_pack32_re_to_i32(a01p, vl);
    const vint32m1_t a01i = zf4_i16_pack32_im_to_i32(a01p, vl);
    const vint32m1_t b_det_q30 =
        zf4_det_q30_i32(a00r, a01r, a01i, a11r, vl);
    const vint32m1_t b_scale =
        zf4_scale_from_det_q30_i32(b_det_q30, vl);
#endif
#if MIMO_EXT_MMSE4_PACKED_INVCOEFF_EXT
    vint32m1_t inv00p, inv01p, inv11p;

    zf4_inv2_coeff_q13_packed_ext(
        a00p, a01p, a11p, b_scale, &inv00p, &inv01p, &inv11p);
#else
    const vint32m1_t inv00 = zf4_inv2_coeff_q13_i32(a11r, b_scale, vl);
    const vint32m1_t inv01r = __riscv_vrsub_vx_i32m1(
        zf4_inv2_coeff_q13_i32(a01r, b_scale, vl), 0, vl);
    const vint32m1_t inv01i = __riscv_vrsub_vx_i32m1(
        zf4_inv2_coeff_q13_i32(a01i, b_scale, vl), 0, vl);
    const vint32m1_t inv11 = zf4_inv2_coeff_q13_i32(a00r, b_scale, vl);
    const vint32m1_t inv00p = zf4_pack_real_i32(inv00, vl);
    const vint32m1_t inv01p = zf4_pack_complex_i32(inv01r, inv01i, vl);
    const vint32m1_t inv11p = zf4_pack_real_i32(inv11, vl);
#endif
    vint32m1_t w00p, w10p, w01p, w11p, y0p, y1p;
    vint32m1_t s00p, s01p, s11p, r0p, r1p;

    zf4_ext_binv_inner5_packed_q15(
        inv00p, inv01p, inv11p,
        a02p, a12p, a03p, a13p, b0p, b1p,
        a22p, a23p, a33p, b2p, b3p,
        &w00p, &w10p, &w01p, &w11p, &y0p, &y1p,
        &s00p, &s01p, &s11p, &r0p, &r1p);

#if MIMO_EXT_MMSE4_PACKED_S_RECIP_MODE == 1
    const vint32m1_t s_scale = b_scale;
#elif MIMO_EXT_MMSE4_PACKED_DET_EXT
    const vint32m1_t s_det_q15 =
        zf4_det_q15_packed_ext(
            s00p, s01p, s11p, MIMO_USE_EXT_MMSE4_PACKED_VSET_CHAIN);
#if MIMO_EXT_MMSE4_PACKED_S_RECIP_MODE == 2
    const vint32m1_t s_scale =
        zf4_scale_q26_packed_nr1(s_det_q15, b_scale, recip_one_q11);
#elif MIMO_USE_EXT_MMSE4_PACKED_E16_RECIP
    const vint32m1_t s_scale =
        zf4_scale_q26_packed_from_det_q15_e16(s_det_q15);
#else
    const vint32m1_t s_scale =
        zf4_scale_from_det_q15_i32(s_det_q15, vl);
#endif
#else
    const vint32m1_t s00r = zf4_i16_pack32_re_to_i32(s00p, vl);
    const vint32m1_t s01r = zf4_i16_pack32_re_to_i32(s01p, vl);
    const vint32m1_t s01i = zf4_i16_pack32_im_to_i32(s01p, vl);
    const vint32m1_t s11r = zf4_i16_pack32_re_to_i32(s11p, vl);
    const vint32m1_t s_det_q30 =
        zf4_det_q30_i32(s00r, s01r, s01i, s11r, vl);
    const vint32m1_t s_scale =
        zf4_scale_from_det_q30_i32(s_det_q30, vl);
#endif
    const ptrdiff_t stride = (ptrdiff_t)(MIMO_N4 * sizeof(mimo_c16_t));
#if MIMO_EXT_MMSE4_PACKED_SSOLVE_EXT
#if MIMO_EXT_MMSE4_PACKED_INVCOEFF_EXT
    vint32m1_t s_inv00p, s_inv01p, s_inv11p;

    zf4_inv2_coeff_q13_packed_ext(
        s00p, s01p, s11p, s_scale,
        &s_inv00p, &s_inv01p, &s_inv11p);
#else
    const vint32m1_t s_inv00 =
        zf4_inv2_coeff_q13_i32(s11r, s_scale, vl);
    const vint32m1_t s_inv01r = __riscv_vrsub_vx_i32m1(
        zf4_inv2_coeff_q13_i32(s01r, s_scale, vl), 0, vl);
    const vint32m1_t s_inv01i = __riscv_vrsub_vx_i32m1(
        zf4_inv2_coeff_q13_i32(s01i, s_scale, vl), 0, vl);
    const vint32m1_t s_inv11 =
        zf4_inv2_coeff_q13_i32(s00r, s_scale, vl);
    const vint32m1_t s_inv00p = zf4_pack_real_i32(s_inv00, vl);
    const vint32m1_t s_inv01p =
        zf4_pack_complex_i32(s_inv01r, s_inv01i, vl);
    const vint32m1_t s_inv11p = zf4_pack_real_i32(s_inv11, vl);
#endif
    vint32m1_t top0p, top1p, x2p, x3p;

    zf4_ext_ssolve_top_packed_q15(
        s_inv00p, s_inv01p, s_inv11p, r0p, r1p,
        w00p, w10p, w01p, w11p, y0p, y1p,
        &top0p, &top1p, &x2p, &x3p);

    __riscv_vsse32_v_i32m1((int32_t *)&x_block[0], stride, top0p, vl);
    __riscv_vsse32_v_i32m1((int32_t *)&x_block[1], stride, top1p, vl);
    __riscv_vsse32_v_i32m1((int32_t *)&x_block[2], stride, x2p, vl);
    __riscv_vsse32_v_i32m1((int32_t *)&x_block[3], stride, x3p, vl);
#else
    const vint32m1_t r0r = zf4_i16_pack32_re_to_i32(r0p, vl);
    const vint32m1_t r0i = zf4_i16_pack32_im_to_i32(r0p, vl);
    const vint32m1_t r1r = zf4_i16_pack32_re_to_i32(r1p, vl);
    const vint32m1_t r1i = zf4_i16_pack32_im_to_i32(r1p, vl);
    vint32m1_t x2r, x2i, x3r, x3i;

    zf4_solve2_i32(s00r, s01r, s01i, s11r, s_scale,
                   r0r, r0i, r1r, r1i,
                   &x2r, &x2i, &x3r, &x3i, 0, vl);

    const vint32m1_t x2p = zf4_pack_complex_i32(x2r, x2i, vl);
    const vint32m1_t x3p = zf4_pack_complex_i32(x3r, x3i, vl);
#if MIMO_EXT_MMSE4_PACKED_TOP_EXT
    vint32m1_t top0p, top1p;

    zf4_ext_top2_packed_q15(
        w00p, w10p, w01p, w11p, y0p, y1p, x2p, x3p,
        &top0p, &top1p);

    __riscv_vsse32_v_i32m1((int32_t *)&x_block[0], stride, top0p, vl);
    __riscv_vsse32_v_i32m1((int32_t *)&x_block[1], stride, top1p, vl);
    __riscv_vsse32_v_i32m1((int32_t *)&x_block[2], stride, x2p, vl);
    __riscv_vsse32_v_i32m1((int32_t *)&x_block[3], stride, x3p, vl);
#else
    vint32m1_t top0r = zf4_i16_pack32_re_to_i32(y0p, vl);
    vint32m1_t top0i = zf4_i16_pack32_im_to_i32(y0p, vl);
    vint32m1_t top1r = zf4_i16_pack32_re_to_i32(y1p, vl);
    vint32m1_t top1i = zf4_i16_pack32_im_to_i32(y1p, vl);

    zf4_submul2_q30_once_i32(
        &top0r, &top0i,
        zf4_i16_pack32_re_to_i32(w00p, vl),
        zf4_i16_pack32_im_to_i32(w00p, vl), x2r, x2i,
        zf4_i16_pack32_re_to_i32(w01p, vl),
        zf4_i16_pack32_im_to_i32(w01p, vl), x3r, x3i, vl);
    zf4_submul2_q30_once_i32(
        &top1r, &top1i,
        zf4_i16_pack32_re_to_i32(w10p, vl),
        zf4_i16_pack32_im_to_i32(w10p, vl), x2r, x2i,
        zf4_i16_pack32_re_to_i32(w11p, vl),
        zf4_i16_pack32_im_to_i32(w11p, vl), x3r, x3i, vl);

    const vint32m1_t top0p = zf4_pack_complex_i32(top0r, top0i, vl);
    const vint32m1_t top1p = zf4_pack_complex_i32(top1r, top1i, vl);

    __riscv_vsse32_v_i32m1((int32_t *)&x_block[0], stride, top0p, vl);
    __riscv_vsse32_v_i32m1((int32_t *)&x_block[1], stride, top1p, vl);
    __riscv_vsse32_v_i32m1((int32_t *)&x_block[2], stride, x2p, vl);
    __riscv_vsse32_v_i32m1((int32_t *)&x_block[3], stride, x3p, vl);
#endif
#endif
}

static MIMO_ALWAYS_INLINE void zf4_mmse_packed_phase_q15(
    const mimo_site4_upper_i16_t *restrict work,
    mimo_c16_t *restrict x_block)
{
    const size_t vl = __riscv_vsetvl_e32m1(MIMO_SITE4_LANES);
    const vint32m1_t a00p = zf4_i16_complex_pack32_load(work->a[0], vl);
    const vint32m1_t a01p = zf4_i16_complex_pack32_load(work->a[1], vl);
    const vint32m1_t a02p = zf4_i16_complex_pack32_load(work->a[2], vl);
    const vint32m1_t a03p = zf4_i16_complex_pack32_load(work->a[3], vl);
    const vint32m1_t a11p = zf4_i16_complex_pack32_load(work->a[4], vl);
    const vint32m1_t a12p = zf4_i16_complex_pack32_load(work->a[5], vl);
    const vint32m1_t a13p = zf4_i16_complex_pack32_load(work->a[6], vl);
    const vint32m1_t a22p = zf4_i16_complex_pack32_load(work->a[7], vl);
    const vint32m1_t a23p = zf4_i16_complex_pack32_load(work->a[8], vl);
    const vint32m1_t a33p = zf4_i16_complex_pack32_load(work->a[9], vl);
    const vint32m1_t b0p = zf4_i16_complex_pack32_load(work->b[0], vl);
    const vint32m1_t b1p = zf4_i16_complex_pack32_load(work->b[1], vl);
    const vint32m1_t b2p = zf4_i16_complex_pack32_load(work->b[2], vl);
    const vint32m1_t b3p = zf4_i16_complex_pack32_load(work->b[3], vl);
#if MIMO_USE_EXT_MMSE4_PACKED_E16_LAMBDA
    const vint32m1_t lambda_packed = zf4_mmse_lambda_packed_e16();

    asm volatile(
        "vsetivli zero, 8, e16, m1, ta, ma\n\t"
        :
        :
        : "memory");
#endif
#if MIMO_EXT_MMSE4_PACKED_S_RECIP_MODE == 2
    const vint32m1_t recip_one_q11 =
        zf4_recip_one_q11_packed(lambda_packed);
#endif

    zf4_eq4_packed_vectors_q15(
        a00p, a01p, a02p, a03p, a11p, a12p, a13p,
        a22p, a23p, a33p, b0p, b1p, b2p, b3p,
#if MIMO_USE_EXT_MMSE4_PACKED_E16_LAMBDA
        lambda_packed,
#endif
#if MIMO_EXT_MMSE4_PACKED_S_RECIP_MODE == 2
        recip_one_q11,
#endif
        1,
        x_block);
}

#if MIMO_USE_EXT_MMSE4_PACKED_LIVE_BUILD
#if MIMO_USE_EXT_MMSE4_PACKED_E16_RECIP && \
    MIMO_USE_EXT_MMSE4_PACKED_E16_LAMBDA
__attribute__((aligned(4)))
#endif
MIMO_NOINLINE
void mimo_ext_mmse4_packed_live_run(const mimo_c16_t *h_site4,
                                    const mimo_c16_t *y_site4,
                                    mimo_c16_t *x)
{
    const size_t blocks = MIMO_EQ4_SITE_COUNT / MIMO_SITE4_LANES;

    mimo_ext_set_vxrm_rnu_once();
#if MIMO_USE_EXT_MMSE4_PACKED_E16_LAMBDA
    const vint32m1_t lambda_packed = zf4_mmse_lambda_packed_e16();
#endif
#if MIMO_EXT_MMSE4_PACKED_S_RECIP_MODE == 2
    const vint32m1_t recip_one_q11 =
        zf4_recip_one_q11_packed(lambda_packed);
#endif
#if MIMO_USE_EXT_MMSE4_PACKED_E16_RECIP && \
    MIMO_USE_EXT_MMSE4_PACKED_E16_LAMBDA && \
    MIMO_USE_EXT_MMSE4_PACKED_FIXED_PTRS
    asm volatile("c.nop\n\t" : : : "memory");
#if MIMO_USE_EXT_MMSE4_PACKED_RX4_M4_LOAD
    asm volatile("c.nop\n\t" : : : "memory");
#endif
#endif
    for (size_t block = 0; block < blocks; ++block) {
        vint32m1_t a00p, a01p, a02p, a03p, a11p, a12p, a13p;
        vint32m1_t a22p, a23p, a33p, b0p, b1p, b2p, b3p;

        zf4_ext_build_live_packed_q15(
            h_site4, y_site4, block,
            &a00p, &a01p, &a02p, &a03p, &a11p, &a12p, &a13p,
            &a22p, &a23p, &a33p, &b0p, &b1p, &b2p, &b3p);
        zf4_eq4_packed_vectors_q15(
            a00p, a01p, a02p, a03p, a11p, a12p, a13p,
            a22p, a23p, a33p, b0p, b1p, b2p, b3p,
#if MIMO_USE_EXT_MMSE4_PACKED_E16_LAMBDA
            lambda_packed,
#endif
#if MIMO_EXT_MMSE4_PACKED_S_RECIP_MODE == 2
            recip_one_q11,
#endif
            1,
            &x[block * MIMO_SITE4_LANES * MIMO_N4]);
    }
}

#if MIMO_USE_EXT_ZF4_PACKED_LIVE_BUILD
__attribute__((aligned(4)))
MIMO_NOINLINE
void mimo_ext_zf4_packed_live_run(const mimo_c16_t *h_site4,
                                  const mimo_c16_t *y_site4,
                                  mimo_c16_t *x)
{
    const size_t blocks = MIMO_EQ4_SITE_COUNT / MIMO_SITE4_LANES;

    mimo_ext_set_vxrm_rnu_once();
    asm volatile("c.nop\n\tc.nop\n\t" : : : "memory");
    for (size_t block = 0; block < blocks; ++block) {
        vint32m1_t a00p, a01p, a02p, a03p, a11p, a12p, a13p;
        vint32m1_t a22p, a23p, a33p, b0p, b1p, b2p, b3p;

        zf4_ext_build_live_packed_q15(
            h_site4, y_site4, block,
            &a00p, &a01p, &a02p, &a03p, &a11p, &a12p, &a13p,
            &a22p, &a23p, &a33p, &b0p, &b1p, &b2p, &b3p);
        zf4_eq4_packed_vectors_q15(
            a00p, a01p, a02p, a03p, a11p, a12p, a13p,
            a22p, a23p, a33p, b0p, b1p, b2p, b3p,
#if MIMO_USE_EXT_MMSE4_PACKED_E16_LAMBDA
            a00p,
#endif
            0,
            &x[block * MIMO_SITE4_LANES * MIMO_N4]);
    }
}
#endif
#endif
#endif

static MIMO_ALWAYS_INLINE void zf4_store_complex_i32(
    mimo_c16_t v[restrict MIMO_SITE4_LANES],
    vint32m1_t re,
    vint32m1_t im,
    size_t vl)
{
    const vint32m1_t packed = zf4_pack_complex_i32(re, im, vl);

    __riscv_vse32_v_i32m1((int32_t *)v, packed, vl);
}

static MIMO_ALWAYS_INLINE void zf4_ext_site4_prelude_asm(void)
{
    asm volatile(
        "vsetivli zero, 8, e16, m1, ta, ma\n\t"
        "vid.v v28\n\t"
        "vsll.vi v28, v28, 1\n\t"
        "vadd.vi v30, v28, 1\n\t"
        "vmsltu.vi v0, v28, 8\n\t"
        :
        :
        : "memory");
}

static MIMO_ALWAYS_INLINE void zf4_rvv_e32m1_vl4_asm(void)
{
    asm volatile(
        "vsetivli zero, 4, e32, m1, ta, ma\n\t"
        :
        :
        : "memory");
}

static MIMO_ALWAYS_INLINE void zf4_store_x_i32(
    mimo_c16_t *restrict x_block,
    int tx,
    vint32m1_t re,
    vint32m1_t im,
    size_t vl)
{
    const ptrdiff_t stride = (ptrdiff_t)(MIMO_N4 * sizeof(mimo_c16_t));
#if MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_STORE
    const vint32m1_t packed = zf4_pack_complex_i32(re, im, vl);

    __riscv_vsse32_v_i32m1((int32_t *)&x_block[tx], stride, packed, vl);
#else
    const vint16mf2_t re16 = __riscv_vncvt_x_x_w_i16mf2(re, vl);
    const vint16mf2_t im16 = __riscv_vncvt_x_x_w_i16mf2(im, vl);

    __riscv_vsse16_v_i16mf2(&x_block[tx].r, stride, re16, vl);
    __riscv_vsse16_v_i16mf2(&x_block[tx].i, stride, im16, vl);
#endif
}

#if MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE
void mimo_solve_normal4_site4_upper_i16_schur_zf_rvv_i32_q15(
    const mimo_site4_upper_i16_t *restrict work,
    int mmse,
    mimo_c16_t *restrict x_block)
{
#if MIMO_USE_EXT_MMSE4_RVV_I32_PACKED_PHASE
    (void)mmse;
    zf4_mmse_packed_phase_q15(work, x_block);
    return;
#endif
    const size_t vl = __riscv_vsetvl_e32m1(MIMO_SITE4_LANES);
#if MIMO_USE_EXT_MMSE4_RVV_I32_SPECIALIZE
    (void)mmse;
#endif

#if MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_LOAD
#if MIMO_USE_EXT_MMSE4_RVV_I32_DIAG_VLSE16_LOAD
    vint32m1_t a00r = zf4_i16_complex_re_to_i32(work->a[0], vl);
#else
    vint32m1_t a00r =
        zf4_i16_complex_pack32_re_to_i32(work->a[0], vl);
#endif
    vint32m1_t a01r, a01i;
    vint32m1_t a02r, a02i;
    vint32m1_t a03r, a03i;
#if MIMO_USE_EXT_MMSE4_RVV_I32_DIAG_VLSE16_LOAD
    vint32m1_t a11r = zf4_i16_complex_re_to_i32(work->a[4], vl);
#else
    vint32m1_t a11r =
        zf4_i16_complex_pack32_re_to_i32(work->a[4], vl);
#endif
    vint32m1_t a12r, a12i;
    vint32m1_t a13r, a13i;
    vint32m1_t a22r;
    vint32m1_t a23r, a23i;
    vint32m1_t a33r;
    vint32m1_t b0r, b0i;
    vint32m1_t b1r, b1i;
    vint32m1_t b2r, b2i;
    vint32m1_t b3r, b3i;

    zf4_i16_complex_pack32_to_i32(work->a[1], &a01r, &a01i, vl);
    zf4_i16_complex_pack32_to_i32(work->a[2], &a02r, &a02i, vl);
    zf4_i16_complex_pack32_to_i32(work->a[3], &a03r, &a03i, vl);
    zf4_i16_complex_pack32_to_i32(work->a[5], &a12r, &a12i, vl);
    zf4_i16_complex_pack32_to_i32(work->a[6], &a13r, &a13i, vl);
#if !MIMO_USE_EXT_MMSE4_RVV_I32_LATE_TAIL_LOAD
#if MIMO_USE_EXT_MMSE4_RVV_I32_DIAG_VLSE16_LOAD
    a22r = zf4_i16_complex_re_to_i32(work->a[7], vl);
#else
    a22r = zf4_i16_complex_pack32_re_to_i32(work->a[7], vl);
#endif
    zf4_i16_complex_pack32_to_i32(work->a[8], &a23r, &a23i, vl);
#if MIMO_USE_EXT_MMSE4_RVV_I32_DIAG_VLSE16_LOAD
    a33r = zf4_i16_complex_re_to_i32(work->a[9], vl);
#else
    a33r = zf4_i16_complex_pack32_re_to_i32(work->a[9], vl);
#endif
#endif
    zf4_i16_complex_pack32_to_i32(work->b[0], &b0r, &b0i, vl);
    zf4_i16_complex_pack32_to_i32(work->b[1], &b1r, &b1i, vl);
#if !MIMO_USE_EXT_MMSE4_RVV_I32_LATE_TAIL_LOAD
    zf4_i16_complex_pack32_to_i32(work->b[2], &b2r, &b2i, vl);
    zf4_i16_complex_pack32_to_i32(work->b[3], &b3r, &b3i, vl);
#endif
#else
    vint32m1_t a00r = zf4_i16_complex_re_to_i32(work->a[0], vl);
    const vint32m1_t a01r = zf4_i16_complex_re_to_i32(work->a[1], vl);
    const vint32m1_t a01i = zf4_i16_complex_im_to_i32(work->a[1], vl);
    const vint32m1_t a02r = zf4_i16_complex_re_to_i32(work->a[2], vl);
    const vint32m1_t a02i = zf4_i16_complex_im_to_i32(work->a[2], vl);
    const vint32m1_t a03r = zf4_i16_complex_re_to_i32(work->a[3], vl);
    const vint32m1_t a03i = zf4_i16_complex_im_to_i32(work->a[3], vl);
    vint32m1_t a11r = zf4_i16_complex_re_to_i32(work->a[4], vl);
    const vint32m1_t a12r = zf4_i16_complex_re_to_i32(work->a[5], vl);
    const vint32m1_t a12i = zf4_i16_complex_im_to_i32(work->a[5], vl);
    const vint32m1_t a13r = zf4_i16_complex_re_to_i32(work->a[6], vl);
    const vint32m1_t a13i = zf4_i16_complex_im_to_i32(work->a[6], vl);
    vint32m1_t a22r = zf4_i16_complex_re_to_i32(work->a[7], vl);
    const vint32m1_t a23r = zf4_i16_complex_re_to_i32(work->a[8], vl);
    const vint32m1_t a23i = zf4_i16_complex_im_to_i32(work->a[8], vl);
    vint32m1_t a33r = zf4_i16_complex_re_to_i32(work->a[9], vl);
    const vint32m1_t b0r = zf4_i16_complex_re_to_i32(work->b[0], vl);
    const vint32m1_t b0i = zf4_i16_complex_im_to_i32(work->b[0], vl);
    const vint32m1_t b1r = zf4_i16_complex_re_to_i32(work->b[1], vl);
    const vint32m1_t b1i = zf4_i16_complex_im_to_i32(work->b[1], vl);
    const vint32m1_t b2r = zf4_i16_complex_re_to_i32(work->b[2], vl);
    const vint32m1_t b2i = zf4_i16_complex_im_to_i32(work->b[2], vl);
    const vint32m1_t b3r = zf4_i16_complex_re_to_i32(work->b[3], vl);
    const vint32m1_t b3i = zf4_i16_complex_im_to_i32(work->b[3], vl);
#endif

#if MIMO_USE_EXT_MMSE4_RVV_I32_SPECIALIZE
    a00r = zf4_mmse_lambda_sat_i32(a00r, vl);
    a11r = zf4_mmse_lambda_sat_i32(a11r, vl);
#if !MIMO_USE_EXT_MMSE4_RVV_I32_LATE_TAIL_LOAD
    a22r = zf4_mmse_lambda_sat_i32(a22r, vl);
    a33r = zf4_mmse_lambda_sat_i32(a33r, vl);
#endif
#elif MIMO_USE_EXT_MMSE4_RVV_I32_VEC_LAMBDA
    if (mmse) {
        a00r = zf4_mmse_lambda_sat_i32(a00r, vl);
        a11r = zf4_mmse_lambda_sat_i32(a11r, vl);
#if !MIMO_USE_EXT_MMSE4_RVV_I32_LATE_TAIL_LOAD
        a22r = zf4_mmse_lambda_sat_i32(a22r, vl);
        a33r = zf4_mmse_lambda_sat_i32(a33r, vl);
#endif
    }
#endif

    const vint32m1_t b_det_q30 =
        zf4_det_q30_i32(a00r, a01r, a01i, a11r, vl);
    const vint32m1_t b_scale =
        zf4_scale_from_det_q30_i32(b_det_q30, vl);
    vint32m1_t w00r, w00i, w10r, w10i;
    vint32m1_t w01r, w01i, w11r, w11i;
    vint32m1_t y0r, y0i, y1r, y1i;
#if MIMO_USE_EXT_MMSE4_RVV_I32_SPECIALIZE
    const int trunc_w0 = (MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK & 1) != 0;
    const int trunc_w1 = (MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK & 2) != 0;
    const int trunc_y = (MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK & 4) != 0;
    const int trunc_s =
        ((MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK & 8) ||
         MIMO_USE_EXT_MMSE4_SCHUR_FINAL_SIGNED_TRUNC_SCALE) != 0;
#else
    const int trunc_w0 =
        mmse && (MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK & 1);
    const int trunc_w1 =
        mmse && (MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK & 2);
    const int trunc_y =
        mmse && (MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK & 4);
    const int trunc_s =
        mmse && ((MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK & 8) ||
                 MIMO_USE_EXT_MMSE4_SCHUR_FINAL_SIGNED_TRUNC_SCALE);
#endif

#if MIMO_USE_EXT_MMSE4_RVV_I32_BINV_REUSE
    /* The coefficient-reuse path does not consume the three per-solve
     * truncation selectors computed for the alternative implementation. */
    (void)trunc_w0;
    (void)trunc_w1;
    (void)trunc_y;
    const vint32m1_t inv00 = zf4_inv2_coeff_q13_i32(a11r, b_scale, vl);
    const vint32m1_t inv01r = __riscv_vrsub_vx_i32m1(
        zf4_inv2_coeff_q13_i32(a01r, b_scale, vl), 0, vl);
    const vint32m1_t inv01i = __riscv_vrsub_vx_i32m1(
        zf4_inv2_coeff_q13_i32(a01i, b_scale, vl), 0, vl);
    const vint32m1_t inv11 = zf4_inv2_coeff_q13_i32(a00r, b_scale, vl);

    zf4_apply_inv2_q13_i32(inv00, inv01r, inv01i, inv11,
                           a02r, a02i, a12r, a12i,
                           &w00r, &w00i, &w10r, &w10i, vl);
    zf4_apply_inv2_q13_i32(inv00, inv01r, inv01i, inv11,
                           a03r, a03i, a13r, a13i,
                           &w01r, &w01i, &w11r, &w11i, vl);
    zf4_apply_inv2_q13_i32(inv00, inv01r, inv01i, inv11,
                           b0r, b0i, b1r, b1i,
                           &y0r, &y0i, &y1r, &y1i, vl);
#else
    zf4_solve2_i32(a00r, a01r, a01i, a11r, b_scale,
                   a02r, a02i, a12r, a12i,
                   &w00r, &w00i, &w10r, &w10i, trunc_w0, vl);
    zf4_solve2_i32(a00r, a01r, a01i, a11r, b_scale,
                   a03r, a03i, a13r, a13i,
                   &w01r, &w01i, &w11r, &w11i, trunc_w1, vl);
    zf4_solve2_i32(a00r, a01r, a01i, a11r, b_scale,
                   b0r, b0i, b1r, b1i,
                   &y0r, &y0i, &y1r, &y1i, trunc_y, vl);
#endif

#if MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_LOAD && \
    MIMO_USE_EXT_MMSE4_RVV_I32_LATE_TAIL_LOAD
#if MIMO_USE_EXT_MMSE4_RVV_I32_DIAG_VLSE16_LOAD
    a22r = zf4_i16_complex_re_to_i32(work->a[7], vl);
#else
    a22r = zf4_i16_complex_pack32_re_to_i32(work->a[7], vl);
#endif
    zf4_i16_complex_pack32_to_i32(work->a[8], &a23r, &a23i, vl);
#if MIMO_USE_EXT_MMSE4_RVV_I32_DIAG_VLSE16_LOAD
    a33r = zf4_i16_complex_re_to_i32(work->a[9], vl);
#else
    a33r = zf4_i16_complex_pack32_re_to_i32(work->a[9], vl);
#endif
    zf4_i16_complex_pack32_to_i32(work->b[2], &b2r, &b2i, vl);
    zf4_i16_complex_pack32_to_i32(work->b[3], &b3r, &b3i, vl);
#if MIMO_USE_EXT_MMSE4_RVV_I32_SPECIALIZE
    a22r = zf4_mmse_lambda_sat_i32(a22r, vl);
    a33r = zf4_mmse_lambda_sat_i32(a33r, vl);
#elif MIMO_USE_EXT_MMSE4_RVV_I32_VEC_LAMBDA
    if (mmse) {
        a22r = zf4_mmse_lambda_sat_i32(a22r, vl);
        a33r = zf4_mmse_lambda_sat_i32(a33r, vl);
    }
#endif
#endif

    vint32m1_t inner_r;
    vint32m1_t inner_i;

    zf4_inner_conj2_q15_i32(a02r, a02i, a12r, a12i,
                            w00r, w00i, w10r, w10i,
                            &inner_r, &inner_i, vl);
    const vint32m1_t s00r = __riscv_vsub_vv_i32m1(a22r, inner_r, vl);

    zf4_inner_conj2_q15_i32(a02r, a02i, a12r, a12i,
                            w01r, w01i, w11r, w11i,
                            &inner_r, &inner_i, vl);
    const vint32m1_t s01r = __riscv_vsub_vv_i32m1(a23r, inner_r, vl);
    const vint32m1_t s01i = __riscv_vsub_vv_i32m1(a23i, inner_i, vl);

    zf4_inner_conj2_q15_i32(a03r, a03i, a13r, a13i,
                            w01r, w01i, w11r, w11i,
                            &inner_r, &inner_i, vl);
    const vint32m1_t s11r = __riscv_vsub_vv_i32m1(a33r, inner_r, vl);

    zf4_inner_conj2_q15_i32(a02r, a02i, a12r, a12i,
                            y0r, y0i, y1r, y1i,
                            &inner_r, &inner_i, vl);
    const vint32m1_t r0r = __riscv_vsub_vv_i32m1(b2r, inner_r, vl);
    const vint32m1_t r0i = __riscv_vsub_vv_i32m1(b2i, inner_i, vl);

    zf4_inner_conj2_q15_i32(a03r, a03i, a13r, a13i,
                            y0r, y0i, y1r, y1i,
                            &inner_r, &inner_i, vl);
    const vint32m1_t r1r = __riscv_vsub_vv_i32m1(b3r, inner_r, vl);
    const vint32m1_t r1i = __riscv_vsub_vv_i32m1(b3i, inner_i, vl);

    const vint32m1_t s_det_q30 =
        zf4_det_q30_i32(s00r, s01r, s01i, s11r, vl);
    const vint32m1_t s_scale =
        zf4_scale_from_det_q30_i32(s_det_q30, vl);
    vint32m1_t x2r, x2i, x3r, x3i;

    zf4_solve2_i32(s00r, s01r, s01i, s11r, s_scale,
                   r0r, r0i, r1r, r1i,
                   &x2r, &x2i, &x3r, &x3i, trunc_s, vl);

    vint32m1_t top0r = y0r;
    vint32m1_t top0i = y0i;
    vint32m1_t top1r = y1r;
    vint32m1_t top1i = y1i;

#if MIMO_USE_EXT_MMSE4_RVV_I32_TOP_Q30_ONCE
    zf4_submul2_q30_once_i32(&top0r, &top0i,
                             w00r, w00i, x2r, x2i,
                             w01r, w01i, x3r, x3i, vl);
    zf4_submul2_q30_once_i32(&top1r, &top1i,
                             w10r, w10i, x2r, x2i,
                             w11r, w11i, x3r, x3i, vl);
#else
    zf4_submul_q15_i32(&top0r, &top0i, w00r, w00i, x2r, x2i, vl);
    zf4_submul_q15_i32(&top0r, &top0i, w01r, w01i, x3r, x3i, vl);
    zf4_submul_q15_i32(&top1r, &top1i, w10r, w10i, x2r, x2i, vl);
    zf4_submul_q15_i32(&top1r, &top1i, w11r, w11i, x3r, x3i, vl);
#endif

    zf4_store_x_i32(x_block, 0, top0r, top0i, vl);
    zf4_store_x_i32(x_block, 1, top1r, top1i, vl);
    zf4_store_x_i32(x_block, 2, x2r, x2i, vl);
    zf4_store_x_i32(x_block, 3, x3r, x3i, vl);
}

#if MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SPLIT_SOLVE
void mimo_solve_normal4_site4_upper_i16_schur_zf_rvv_i32_split_q15(
    const mimo_site4_upper_i16_t *restrict work,
    int mmse,
    mimo_c16_t *restrict x_block)
{
    const size_t vl = __riscv_vsetvl_e32m1(MIMO_SITE4_LANES);
    const vint32m1_t zero = __riscv_vmv_v_x_i32m1(0, vl);
    schur4_zf_true16b_split_pack_t p MIMO_ALIGN64;

    {
        const vint32m1_t a00r = zf4_i16_complex_re_to_i32(work->a[0], vl);
        const vint32m1_t a01r = zf4_i16_complex_re_to_i32(work->a[1], vl);
        const vint32m1_t a01i = zf4_i16_complex_im_to_i32(work->a[1], vl);
        const vint32m1_t a02r = zf4_i16_complex_re_to_i32(work->a[2], vl);
        const vint32m1_t a02i = zf4_i16_complex_im_to_i32(work->a[2], vl);
        const vint32m1_t a03r = zf4_i16_complex_re_to_i32(work->a[3], vl);
        const vint32m1_t a03i = zf4_i16_complex_im_to_i32(work->a[3], vl);
        const vint32m1_t a11r = zf4_i16_complex_re_to_i32(work->a[4], vl);
        const vint32m1_t a12r = zf4_i16_complex_re_to_i32(work->a[5], vl);
        const vint32m1_t a12i = zf4_i16_complex_im_to_i32(work->a[5], vl);
        const vint32m1_t a13r = zf4_i16_complex_re_to_i32(work->a[6], vl);
        const vint32m1_t a13i = zf4_i16_complex_im_to_i32(work->a[6], vl);
        const vint32m1_t b0r = zf4_i16_complex_re_to_i32(work->b[0], vl);
        const vint32m1_t b0i = zf4_i16_complex_im_to_i32(work->b[0], vl);
        const vint32m1_t b1r = zf4_i16_complex_re_to_i32(work->b[1], vl);
        const vint32m1_t b1i = zf4_i16_complex_im_to_i32(work->b[1], vl);

        const vint32m1_t b_det_q30 =
            zf4_det_q30_i32(a00r, a01r, a01i, a11r, vl);
        const vint32m1_t b_scale =
            zf4_scale_from_det_q30_i32(b_det_q30, vl);
        vint32m1_t w00r, w00i, w10r, w10i;
        vint32m1_t w01r, w01i, w11r, w11i;
        vint32m1_t y0r, y0i, y1r, y1i;
        const int trunc_w0 =
            mmse && (MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK & 1);
        const int trunc_w1 =
            mmse && (MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK & 2);
        const int trunc_y =
            mmse && (MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK & 4);

        zf4_solve2_i32(a00r, a01r, a01i, a11r, b_scale,
                       a02r, a02i, a12r, a12i,
                       &w00r, &w00i, &w10r, &w10i, trunc_w0, vl);
        zf4_store_complex_i32(p.w00, w00r, w00i, vl);
        zf4_store_complex_i32(p.w10, w10r, w10i, vl);

        zf4_solve2_i32(a00r, a01r, a01i, a11r, b_scale,
                       a03r, a03i, a13r, a13i,
                       &w01r, &w01i, &w11r, &w11i, trunc_w1, vl);
        zf4_store_complex_i32(p.w01, w01r, w01i, vl);
        zf4_store_complex_i32(p.w11, w11r, w11i, vl);

        zf4_solve2_i32(a00r, a01r, a01i, a11r, b_scale,
                       b0r, b0i, b1r, b1i,
                       &y0r, &y0i, &y1r, &y1i, trunc_y, vl);
        zf4_store_complex_i32(p.y0, y0r, y0i, vl);
        zf4_store_complex_i32(p.y1, y1r, y1i, vl);
    }

    {
        const vint32m1_t a02r = zf4_i16_complex_re_to_i32(work->a[2], vl);
        const vint32m1_t a02i = zf4_i16_complex_im_to_i32(work->a[2], vl);
        const vint32m1_t a12r = zf4_i16_complex_re_to_i32(work->a[5], vl);
        const vint32m1_t a12i = zf4_i16_complex_im_to_i32(work->a[5], vl);
        const vint32m1_t a03r = zf4_i16_complex_re_to_i32(work->a[3], vl);
        const vint32m1_t a03i = zf4_i16_complex_im_to_i32(work->a[3], vl);
        const vint32m1_t a13r = zf4_i16_complex_re_to_i32(work->a[6], vl);
        const vint32m1_t a13i = zf4_i16_complex_im_to_i32(work->a[6], vl);
        const vint32m1_t w00r = zf4_i16_complex_re_to_i32(p.w00, vl);
        const vint32m1_t w00i = zf4_i16_complex_im_to_i32(p.w00, vl);
        const vint32m1_t w10r = zf4_i16_complex_re_to_i32(p.w10, vl);
        const vint32m1_t w10i = zf4_i16_complex_im_to_i32(p.w10, vl);
        const vint32m1_t w01r = zf4_i16_complex_re_to_i32(p.w01, vl);
        const vint32m1_t w01i = zf4_i16_complex_im_to_i32(p.w01, vl);
        const vint32m1_t w11r = zf4_i16_complex_re_to_i32(p.w11, vl);
        const vint32m1_t w11i = zf4_i16_complex_im_to_i32(p.w11, vl);
        const vint32m1_t y0r = zf4_i16_complex_re_to_i32(p.y0, vl);
        const vint32m1_t y0i = zf4_i16_complex_im_to_i32(p.y0, vl);
        const vint32m1_t y1r = zf4_i16_complex_re_to_i32(p.y1, vl);
        const vint32m1_t y1i = zf4_i16_complex_im_to_i32(p.y1, vl);
        vint32m1_t inner_r;
        vint32m1_t inner_i;

        zf4_inner_conj2_q15_i32(a02r, a02i, a12r, a12i,
                                w00r, w00i, w10r, w10i,
                                &inner_r, &inner_i, vl);
        zf4_store_complex_i32(
            p.s00,
            __riscv_vsub_vv_i32m1(
                zf4_i16_complex_re_to_i32(work->a[7], vl), inner_r, vl),
            zero, vl);

        zf4_inner_conj2_q15_i32(a02r, a02i, a12r, a12i,
                                w01r, w01i, w11r, w11i,
                                &inner_r, &inner_i, vl);
        zf4_store_complex_i32(
            p.s01,
            __riscv_vsub_vv_i32m1(
                zf4_i16_complex_re_to_i32(work->a[8], vl), inner_r, vl),
            __riscv_vsub_vv_i32m1(
                zf4_i16_complex_im_to_i32(work->a[8], vl), inner_i, vl),
            vl);

        zf4_inner_conj2_q15_i32(a03r, a03i, a13r, a13i,
                                w01r, w01i, w11r, w11i,
                                &inner_r, &inner_i, vl);
        zf4_store_complex_i32(
            p.s11,
            __riscv_vsub_vv_i32m1(
                zf4_i16_complex_re_to_i32(work->a[9], vl), inner_r, vl),
            zero, vl);

        zf4_inner_conj2_q15_i32(a02r, a02i, a12r, a12i,
                                y0r, y0i, y1r, y1i,
                                &inner_r, &inner_i, vl);
        zf4_store_complex_i32(
            p.r0,
            __riscv_vsub_vv_i32m1(
                zf4_i16_complex_re_to_i32(work->b[2], vl), inner_r, vl),
            __riscv_vsub_vv_i32m1(
                zf4_i16_complex_im_to_i32(work->b[2], vl), inner_i, vl),
            vl);

        zf4_inner_conj2_q15_i32(a03r, a03i, a13r, a13i,
                                y0r, y0i, y1r, y1i,
                                &inner_r, &inner_i, vl);
        zf4_store_complex_i32(
            p.r1,
            __riscv_vsub_vv_i32m1(
                zf4_i16_complex_re_to_i32(work->b[3], vl), inner_r, vl),
            __riscv_vsub_vv_i32m1(
                zf4_i16_complex_im_to_i32(work->b[3], vl), inner_i, vl),
            vl);
    }

    {
        const vint32m1_t s00r = zf4_i16_complex_re_to_i32(p.s00, vl);
        const vint32m1_t s01r = zf4_i16_complex_re_to_i32(p.s01, vl);
        const vint32m1_t s01i = zf4_i16_complex_im_to_i32(p.s01, vl);
        const vint32m1_t s11r = zf4_i16_complex_re_to_i32(p.s11, vl);
        const vint32m1_t r0r = zf4_i16_complex_re_to_i32(p.r0, vl);
        const vint32m1_t r0i = zf4_i16_complex_im_to_i32(p.r0, vl);
        const vint32m1_t r1r = zf4_i16_complex_re_to_i32(p.r1, vl);
        const vint32m1_t r1i = zf4_i16_complex_im_to_i32(p.r1, vl);
        const vint32m1_t s_det_q30 =
            zf4_det_q30_i32(s00r, s01r, s01i, s11r, vl);
        const vint32m1_t s_scale =
            zf4_scale_from_det_q30_i32(s_det_q30, vl);
        vint32m1_t x2r, x2i, x3r, x3i;
        const int trunc_s =
            mmse && ((MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK & 8) ||
                     MIMO_USE_EXT_MMSE4_SCHUR_FINAL_SIGNED_TRUNC_SCALE);

        zf4_solve2_i32(s00r, s01r, s01i, s11r, s_scale,
                       r0r, r0i, r1r, r1i,
                       &x2r, &x2i, &x3r, &x3i, trunc_s, vl);

        vint32m1_t top0r = zf4_i16_complex_re_to_i32(p.y0, vl);
        vint32m1_t top0i = zf4_i16_complex_im_to_i32(p.y0, vl);
        vint32m1_t top1r = zf4_i16_complex_re_to_i32(p.y1, vl);
        vint32m1_t top1i = zf4_i16_complex_im_to_i32(p.y1, vl);

        zf4_submul_q15_i32(&top0r, &top0i,
                           zf4_i16_complex_re_to_i32(p.w00, vl),
                           zf4_i16_complex_im_to_i32(p.w00, vl),
                           x2r, x2i, vl);
        zf4_submul_q15_i32(&top0r, &top0i,
                           zf4_i16_complex_re_to_i32(p.w01, vl),
                           zf4_i16_complex_im_to_i32(p.w01, vl),
                           x3r, x3i, vl);
        zf4_submul_q15_i32(&top1r, &top1i,
                           zf4_i16_complex_re_to_i32(p.w10, vl),
                           zf4_i16_complex_im_to_i32(p.w10, vl),
                           x2r, x2i, vl);
        zf4_submul_q15_i32(&top1r, &top1i,
                           zf4_i16_complex_re_to_i32(p.w11, vl),
                           zf4_i16_complex_im_to_i32(p.w11, vl),
                           x3r, x3i, vl);

        zf4_store_x_i32(x_block, 0, top0r, top0i, vl);
        zf4_store_x_i32(x_block, 1, top1r, top1i, vl);
        zf4_store_x_i32(x_block, 2, x2r, x2i, vl);
        zf4_store_x_i32(x_block, 3, x3r, x3i, vl);
    }
}
#endif

#if MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SPLIT_SOLVE >= 2
void mimo_solve_normal4_site4_upper_i16_schur_zf_rvv_i32_split_ext_q15(
    const mimo_site4_upper_i16_t *restrict work,
    int mmse,
    mimo_c16_t *restrict x_block)
{
    const size_t vl = __riscv_vsetvl_e32m1(MIMO_SITE4_LANES);
    schur4_zf_true16b_split_pack_t p MIMO_ALIGN64;

    {
        const vint32m1_t a00r = zf4_i16_complex_re_to_i32(work->a[0], vl);
        const vint32m1_t a01r = zf4_i16_complex_re_to_i32(work->a[1], vl);
        const vint32m1_t a01i = zf4_i16_complex_im_to_i32(work->a[1], vl);
        const vint32m1_t a02r = zf4_i16_complex_re_to_i32(work->a[2], vl);
        const vint32m1_t a02i = zf4_i16_complex_im_to_i32(work->a[2], vl);
        const vint32m1_t a03r = zf4_i16_complex_re_to_i32(work->a[3], vl);
        const vint32m1_t a03i = zf4_i16_complex_im_to_i32(work->a[3], vl);
        const vint32m1_t a11r = zf4_i16_complex_re_to_i32(work->a[4], vl);
        const vint32m1_t a12r = zf4_i16_complex_re_to_i32(work->a[5], vl);
        const vint32m1_t a12i = zf4_i16_complex_im_to_i32(work->a[5], vl);
        const vint32m1_t a13r = zf4_i16_complex_re_to_i32(work->a[6], vl);
        const vint32m1_t a13i = zf4_i16_complex_im_to_i32(work->a[6], vl);
        const vint32m1_t b0r = zf4_i16_complex_re_to_i32(work->b[0], vl);
        const vint32m1_t b0i = zf4_i16_complex_im_to_i32(work->b[0], vl);
        const vint32m1_t b1r = zf4_i16_complex_re_to_i32(work->b[1], vl);
        const vint32m1_t b1i = zf4_i16_complex_im_to_i32(work->b[1], vl);

        const vint32m1_t b_det_q30 =
            zf4_det_q30_i32(a00r, a01r, a01i, a11r, vl);
        const vint32m1_t b_scale =
            zf4_scale_from_det_q30_i32(b_det_q30, vl);
        vint32m1_t w00r, w00i, w10r, w10i;
        vint32m1_t w01r, w01i, w11r, w11i;
        vint32m1_t y0r, y0i, y1r, y1i;
        const int trunc_w0 =
            mmse && (MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK & 1);
        const int trunc_w1 =
            mmse && (MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK & 2);
        const int trunc_y =
            mmse && (MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK & 4);

        zf4_solve2_i32(a00r, a01r, a01i, a11r, b_scale,
                       a02r, a02i, a12r, a12i,
                       &w00r, &w00i, &w10r, &w10i, trunc_w0, vl);
        zf4_store_complex_i32(p.w00, w00r, w00i, vl);
        zf4_store_complex_i32(p.w10, w10r, w10i, vl);

        zf4_solve2_i32(a00r, a01r, a01i, a11r, b_scale,
                       a03r, a03i, a13r, a13i,
                       &w01r, &w01i, &w11r, &w11i, trunc_w1, vl);
        zf4_store_complex_i32(p.w01, w01r, w01i, vl);
        zf4_store_complex_i32(p.w11, w11r, w11i, vl);

        zf4_solve2_i32(a00r, a01r, a01i, a11r, b_scale,
                       b0r, b0i, b1r, b1i,
                       &y0r, &y0i, &y1r, &y1i, trunc_y, vl);
        zf4_store_complex_i32(p.y0, y0r, y0i, vl);
        zf4_store_complex_i32(p.y1, y1r, y1i, vl);
    }

    zf4_ext_site4_prelude_asm();
    schur4_zf_ext_schur5_q15(
        work->a[2], work->a[5], work->a[3], work->a[6],
        p.w00, p.w10, p.w01, p.w11,
        p.y0, p.y1, work->a[7], work->a[8], work->a[9],
        work->b[2], work->b[3],
        p.s00, p.s01, p.s11, p.r0, p.r1);

    {
        zf4_rvv_e32m1_vl4_asm();
        const size_t vl_tail = MIMO_SITE4_LANES;
        const vint32m1_t s00r = zf4_i16_complex_re_to_i32(p.s00, vl_tail);
        const vint32m1_t s01r = zf4_i16_complex_re_to_i32(p.s01, vl_tail);
        const vint32m1_t s01i = zf4_i16_complex_im_to_i32(p.s01, vl_tail);
        const vint32m1_t s11r = zf4_i16_complex_re_to_i32(p.s11, vl_tail);
        const vint32m1_t r0r = zf4_i16_complex_re_to_i32(p.r0, vl_tail);
        const vint32m1_t r0i = zf4_i16_complex_im_to_i32(p.r0, vl_tail);
        const vint32m1_t r1r = zf4_i16_complex_re_to_i32(p.r1, vl_tail);
        const vint32m1_t r1i = zf4_i16_complex_im_to_i32(p.r1, vl_tail);
        const vint32m1_t s_det_q30 =
            zf4_det_q30_i32(s00r, s01r, s01i, s11r, vl_tail);
        const vint32m1_t s_scale =
            zf4_scale_from_det_q30_i32(s_det_q30, vl_tail);
        vint32m1_t x2r, x2i, x3r, x3i;
        const int trunc_s =
            mmse && ((MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK & 8) ||
                     MIMO_USE_EXT_MMSE4_SCHUR_FINAL_SIGNED_TRUNC_SCALE);

        zf4_solve2_i32(s00r, s01r, s01i, s11r, s_scale,
                       r0r, r0i, r1r, r1i,
                       &x2r, &x2i, &x3r, &x3i, trunc_s, vl_tail);
#if MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SPLIT_SOLVE == 3
        vint32m1_t top0r = zf4_i16_complex_re_to_i32(p.y0, vl_tail);
        vint32m1_t top0i = zf4_i16_complex_im_to_i32(p.y0, vl_tail);
        vint32m1_t top1r = zf4_i16_complex_re_to_i32(p.y1, vl_tail);
        vint32m1_t top1i = zf4_i16_complex_im_to_i32(p.y1, vl_tail);

        zf4_submul_q15_i32(&top0r, &top0i,
                           zf4_i16_complex_re_to_i32(p.w00, vl_tail),
                           zf4_i16_complex_im_to_i32(p.w00, vl_tail),
                           x2r, x2i, vl_tail);
        zf4_submul_q15_i32(&top0r, &top0i,
                           zf4_i16_complex_re_to_i32(p.w01, vl_tail),
                           zf4_i16_complex_im_to_i32(p.w01, vl_tail),
                           x3r, x3i, vl_tail);
        zf4_submul_q15_i32(&top1r, &top1i,
                           zf4_i16_complex_re_to_i32(p.w10, vl_tail),
                           zf4_i16_complex_im_to_i32(p.w10, vl_tail),
                           x2r, x2i, vl_tail);
        zf4_submul_q15_i32(&top1r, &top1i,
                           zf4_i16_complex_re_to_i32(p.w11, vl_tail),
                           zf4_i16_complex_im_to_i32(p.w11, vl_tail),
                           x3r, x3i, vl_tail);

        zf4_store_x_i32(x_block, 0, top0r, top0i, vl_tail);
        zf4_store_x_i32(x_block, 1, top1r, top1i, vl_tail);
        zf4_store_x_i32(x_block, 2, x2r, x2i, vl_tail);
        zf4_store_x_i32(x_block, 3, x3r, x3i, vl_tail);
#else
        zf4_store_complex_i32(p.x2, x2r, x2i, vl_tail);
        zf4_store_complex_i32(p.x3, x3r, x3i, vl_tail);
#endif
    }

#if MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SPLIT_SOLVE == 2
    zf4_ext_site4_prelude_asm();
    schur4_zf_ext_top2_store32_q15(p.w00, p.w10, p.w01, p.w11,
                                   p.x2, p.x3, p.y0, p.y1, x_block);
#endif
}
#endif
#endif
#endif

void mimo_solve_normal4_site4_upper_i16_schur_zf_q15(
    const mimo_site4_upper_i16_t *restrict work,
    int mmse,
    mimo_c16_t *restrict x_block)
{
    schur4_zf_upper_i16_pack_t p MIMO_ALIGN64;

#if MIMO_USE_SOLVE4_SCHUR_ZF_Q15_NUM_SOLVE
    {
        schur4_q15_recip_scale_t b_scale[MIMO_SITE4_LANES] MIMO_ALIGN64;

        for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
            const schur4_zf_num_t b_det_q30 =
                SCHUR4_ZF_DET2_Q30(work->a[0][lane].r,
                                    work->a[1][lane].r,
                                    work->a[1][lane].i,
                                    work->a[4][lane].r);

            b_scale[lane] = schur4_q15_recip_scale_from_den(b_det_q30);
        }

        schur4_zf_ext_solve2h3_q15num_q15(
            work->a[0], work->a[1], work->a[4],
            work->a[2], work->a[5],
            work->a[3], work->a[6],
            work->b[0], work->b[1], b_scale,
            p.w00, p.w10, p.w01, p.w11, p.y0, p.y1);
    }
#elif MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_BSOLVE
    {
        const size_t vl = __riscv_vsetvl_e32m1(MIMO_SITE4_LANES);
        const vint32m1_t a00r = zf4_i16_complex_re_to_i32(work->a[0], vl);
        const vint32m1_t a01r = zf4_i16_complex_re_to_i32(work->a[1], vl);
        const vint32m1_t a01i = zf4_i16_complex_im_to_i32(work->a[1], vl);
        const vint32m1_t a02r = zf4_i16_complex_re_to_i32(work->a[2], vl);
        const vint32m1_t a02i = zf4_i16_complex_im_to_i32(work->a[2], vl);
        const vint32m1_t a03r = zf4_i16_complex_re_to_i32(work->a[3], vl);
        const vint32m1_t a03i = zf4_i16_complex_im_to_i32(work->a[3], vl);
        const vint32m1_t a11r = zf4_i16_complex_re_to_i32(work->a[4], vl);
        const vint32m1_t a12r = zf4_i16_complex_re_to_i32(work->a[5], vl);
        const vint32m1_t a12i = zf4_i16_complex_im_to_i32(work->a[5], vl);
        const vint32m1_t a13r = zf4_i16_complex_re_to_i32(work->a[6], vl);
        const vint32m1_t a13i = zf4_i16_complex_im_to_i32(work->a[6], vl);
        const vint32m1_t b0r = zf4_i16_complex_re_to_i32(work->b[0], vl);
        const vint32m1_t b0i = zf4_i16_complex_im_to_i32(work->b[0], vl);
        const vint32m1_t b1r = zf4_i16_complex_re_to_i32(work->b[1], vl);
        const vint32m1_t b1i = zf4_i16_complex_im_to_i32(work->b[1], vl);
        const vint32m1_t b_det_q30 =
            zf4_det_q30_i32(a00r, a01r, a01i, a11r, vl);
        const vint32m1_t b_scale =
            zf4_scale_from_det_q30_i32(b_det_q30, vl);
        vint32m1_t w00r, w00i, w10r, w10i;
        vint32m1_t w01r, w01i, w11r, w11i;
        vint32m1_t y0r, y0i, y1r, y1i;
        const int trunc_w0 =
            mmse && (MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK & 1);
        const int trunc_w1 =
            mmse && (MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK & 2);
        const int trunc_y =
            mmse && (MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK & 4);

        zf4_solve2_i32(a00r, a01r, a01i, a11r, b_scale,
                       a02r, a02i, a12r, a12i,
                       &w00r, &w00i, &w10r, &w10i, trunc_w0, vl);
        zf4_store_complex_i32(p.w00, w00r, w00i, vl);
        zf4_store_complex_i32(p.w10, w10r, w10i, vl);

        zf4_solve2_i32(a00r, a01r, a01i, a11r, b_scale,
                       a03r, a03i, a13r, a13i,
                       &w01r, &w01i, &w11r, &w11i, trunc_w1, vl);
        zf4_store_complex_i32(p.w01, w01r, w01i, vl);
        zf4_store_complex_i32(p.w11, w11r, w11i, vl);

        zf4_solve2_i32(a00r, a01r, a01i, a11r, b_scale,
                       b0r, b0i, b1r, b1i,
                       &y0r, &y0i, &y1r, &y1i, trunc_y, vl);
        zf4_store_complex_i32(p.y0, y0r, y0i, vl);
        zf4_store_complex_i32(p.y1, y1r, y1i, vl);
    }
#else
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        const schur4_zf_state_t a00r = work->a[0][lane].r;
        const schur4_zf_state_t a01r = work->a[1][lane].r;
        const schur4_zf_state_t a01i = work->a[1][lane].i;
        const schur4_zf_state_t a02r = work->a[2][lane].r;
        const schur4_zf_state_t a02i = work->a[2][lane].i;
        const schur4_zf_state_t a03r = work->a[3][lane].r;
        const schur4_zf_state_t a03i = work->a[3][lane].i;
        const schur4_zf_state_t a11r = work->a[4][lane].r;
        const schur4_zf_state_t a12r = work->a[5][lane].r;
        const schur4_zf_state_t a12i = work->a[5][lane].i;
        const schur4_zf_state_t a13r = work->a[6][lane].r;
        const schur4_zf_state_t a13i = work->a[6][lane].i;
        const schur4_zf_state_t b0r = work->b[0][lane].r;
        const schur4_zf_state_t b0i = work->b[0][lane].i;
        const schur4_zf_state_t b1r = work->b[1][lane].r;
        const schur4_zf_state_t b1i = work->b[1][lane].i;
        schur4_zf_state_t w00r, w00i, w10r, w10i;
        schur4_zf_state_t w01r, w01i, w11r, w11i;
        schur4_zf_state_t y0r, y0i, y1r, y1i;

        const schur4_zf_num_t b_det_q30 =
            SCHUR4_ZF_DET2_Q30(a00r, a01r, a01i, a11r);
        const schur4_q15_recip_scale_t b_det_scale =
            schur4_q15_recip_scale_from_den(b_det_q30);

        SCHUR4_ZF_SOLVE2_EXPLICIT(a00r, a01r, a01i, a11r, b_det_scale,
                                  a02r, a02i, a12r, a12i,
                                  w00r, w00i, w10r, w10i);
        SCHUR4_ZF_SOLVE2_EXPLICIT(a00r, a01r, a01i, a11r, b_det_scale,
                                  a03r, a03i, a13r, a13i,
                                  w01r, w01i, w11r, w11i);
        SCHUR4_ZF_SOLVE2_EXPLICIT(a00r, a01r, a01i, a11r, b_det_scale,
                                  b0r, b0i, b1r, b1i,
                                  y0r, y0i, y1r, y1i);

        p.w00[lane] = schur4_zf_state_c16(w00r, w00i);
        p.w10[lane] = schur4_zf_state_c16(w10r, w10i);
        p.w01[lane] = schur4_zf_state_c16(w01r, w01i);
        p.w11[lane] = schur4_zf_state_c16(w11r, w11i);
        p.y0[lane] = schur4_zf_state_c16(y0r, y0i);
        p.y1[lane] = schur4_zf_state_c16(y1r, y1i);
    }
#endif

#if MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_BSOLVE
    zf4_ext_site4_prelude_asm();
#endif
    schur4_zf_ext_schur5_q15(
        work->a[2], work->a[5], work->a[3], work->a[6],
        p.w00, p.w10, p.w01, p.w11,
        p.y0, p.y1, work->a[7], work->a[8], work->a[9],
        work->b[2], work->b[3],
        p.s00, p.s01, p.s11, p.r0, p.r1);

#if MIMO_USE_SOLVE4_SCHUR_ZF_Q15_NUM_SOLVE
    {
        schur4_q15_recip_scale_t s_scale[MIMO_SITE4_LANES] MIMO_ALIGN64;

        for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
            const schur4_zf_num_t s_det_q30 =
                SCHUR4_ZF_DET2_Q30(p.s00[lane].r, p.s01[lane].r,
                                    p.s01[lane].i, p.s11[lane].r);

            s_scale[lane] = schur4_q15_recip_scale_from_den(s_det_q30);
        }

        schur4_zf_ext_solve2_q15num_q15(
            p.s00, p.s01, p.s11, p.r0, p.r1, s_scale, p.x2, p.x3);

#if MIMO_USE_EXT_ZF4_SCALAR_X23_STORE
        for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
            mimo_c16_t *x = &x_block[lane * MIMO_N4];

            x[2] = mimo_make_c16(mimo_solve_store_i16(p.x2[lane].r),
                                 mimo_solve_store_i16(p.x2[lane].i));
            x[3] = mimo_make_c16(mimo_solve_store_i16(p.x3[lane].r),
                                 mimo_solve_store_i16(p.x3[lane].i));
        }
#endif
    }
#else
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        const schur4_zf_state_t s00r = p.s00[lane].r;
        const schur4_zf_state_t s01r = p.s01[lane].r;
        const schur4_zf_state_t s01i = p.s01[lane].i;
        const schur4_zf_state_t s11r = p.s11[lane].r;
        schur4_zf_state_t x2r, x2i, x3r, x3i;

        const schur4_zf_num_t s_det_q30 =
            SCHUR4_ZF_DET2_Q30(s00r, s01r, s01i, s11r);
        const schur4_q15_recip_scale_t s_det_scale =
            schur4_q15_recip_scale_from_den(s_det_q30);

        SCHUR4_ZF_SOLVE2_EXPLICIT(s00r, s01r, s01i, s11r, s_det_scale,
                                  p.r0[lane].r, p.r0[lane].i,
                                  p.r1[lane].r, p.r1[lane].i,
                                  x2r, x2i, x3r, x3i);
        p.x2[lane] = schur4_zf_state_c16(x2r, x2i);
        p.x3[lane] = schur4_zf_state_c16(x3r, x3i);
#if MIMO_USE_EXT_ZF4_SCALAR_X23_STORE
        mimo_c16_t *x = &x_block[lane * MIMO_N4];

        x[2] = mimo_make_c16(mimo_solve_store_i16(x2r),
                             mimo_solve_store_i16(x2i));
        x[3] = mimo_make_c16(mimo_solve_store_i16(x3r),
                             mimo_solve_store_i16(x3i));
#endif
    }
#endif

#if MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER == 5
#if MIMO_USE_EXT_ZF4_SCALAR_X23_STORE
    schur4_zf_ext_top2_store32_toponly_q15(p.w00, p.w10, p.w01, p.w11,
                                           p.x2, p.x3, p.y0, p.y1,
                                           x_block);
#else
    schur4_zf_ext_top2_store32_q15(p.w00, p.w10, p.w01, p.w11, p.x2, p.x3,
                                   p.y0, p.y1, x_block);
#endif
#else
    schur4_zf_ext_top2_q15(p.w00, p.w10, p.w01, p.w11, p.x2, p.x3,
                           p.y0, p.y1, p.top0, p.top1);

    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        mimo_c16_t *x = &x_block[lane * MIMO_N4];

        x[0] = mimo_make_c16(mimo_solve_store_i16(p.top0[lane].r),
                             mimo_solve_store_i16(p.top0[lane].i));
        x[1] = mimo_make_c16(mimo_solve_store_i16(p.top1[lane].r),
                             mimo_solve_store_i16(p.top1[lane].i));
        x[2] = mimo_make_c16(mimo_solve_store_i16(p.x2[lane].r),
                             mimo_solve_store_i16(p.x2[lane].i));
        x[3] = mimo_make_c16(mimo_solve_store_i16(p.x3[lane].r),
                             mimo_solve_store_i16(p.x3[lane].i));
    }
#endif
}
#endif
#endif

#if MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER == 1
static MIMO_NOINLINE void schur4_solve_site4_zf_ext_inner_q15(
    int32_t (*restrict a_re)[MIMO_SITE4_LANES],
    int32_t (*restrict a_im)[MIMO_SITE4_LANES],
    int32_t (*restrict b_re)[MIMO_SITE4_LANES],
    int32_t (*restrict b_im)[MIMO_SITE4_LANES],
    mimo_c16_t *restrict x_block)
{
    mimo_c16_t e02[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t e12[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t e03[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t e13[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t w00[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t w10[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t w01[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t w11[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t y0[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t y1[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t b2v[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t b3v[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t s00[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t s01[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t s11[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t r0[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t r1[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t x2[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t x3[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t top0[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t top1[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t inner[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t tmp0[MIMO_SITE4_LANES] MIMO_ALIGN64;
    mimo_c16_t tmp1[MIMO_SITE4_LANES] MIMO_ALIGN64;
    schur4_zf_state_t a22r[MIMO_SITE4_LANES];
    schur4_zf_state_t a23r[MIMO_SITE4_LANES];
    schur4_zf_state_t a23i[MIMO_SITE4_LANES];
    schur4_zf_state_t a33r[MIMO_SITE4_LANES];

    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        const schur4_zf_state_t a00r = a_re[0][lane];
        const schur4_zf_state_t a01r = a_re[1][lane];
        const schur4_zf_state_t a01i = a_im[1][lane];
        const schur4_zf_state_t a02r = a_re[2][lane];
        const schur4_zf_state_t a02i = a_im[2][lane];
        const schur4_zf_state_t a03r = a_re[3][lane];
        const schur4_zf_state_t a03i = a_im[3][lane];
        const schur4_zf_state_t a11r = a_re[5][lane];
        const schur4_zf_state_t a12r = a_re[6][lane];
        const schur4_zf_state_t a12i = a_im[6][lane];
        const schur4_zf_state_t a13r = a_re[7][lane];
        const schur4_zf_state_t a13i = a_im[7][lane];
        const schur4_zf_state_t b0r = b_re[0][lane];
        const schur4_zf_state_t b0i = b_im[0][lane];
        const schur4_zf_state_t b1r = b_re[1][lane];
        const schur4_zf_state_t b1i = b_im[1][lane];
        schur4_zf_state_t w00r, w00i, w10r, w10i;
        schur4_zf_state_t w01r, w01i, w11r, w11i;
        schur4_zf_state_t y0r, y0i, y1r, y1i;

        const schur4_zf_num_t b_det_q30 =
            SCHUR4_ZF_DET2_Q30(a00r, a01r, a01i, a11r);
        const schur4_q15_recip_scale_t b_det_scale =
            schur4_q15_recip_scale_from_den(b_det_q30);

        SCHUR4_ZF_SOLVE2_EXPLICIT(a00r, a01r, a01i, a11r, b_det_scale,
                                  a02r, a02i, a12r, a12i,
                                  w00r, w00i, w10r, w10i);
        SCHUR4_ZF_SOLVE2_EXPLICIT(a00r, a01r, a01i, a11r, b_det_scale,
                                  a03r, a03i, a13r, a13i,
                                  w01r, w01i, w11r, w11i);
        SCHUR4_ZF_SOLVE2_EXPLICIT(a00r, a01r, a01i, a11r, b_det_scale,
                                  b0r, b0i, b1r, b1i,
                                  y0r, y0i, y1r, y1i);

        e02[lane] = schur4_zf_state_c16(a02r, a02i);
        e12[lane] = schur4_zf_state_c16(a12r, a12i);
        e03[lane] = schur4_zf_state_c16(a03r, a03i);
        e13[lane] = schur4_zf_state_c16(a13r, a13i);
        w00[lane] = schur4_zf_state_c16(w00r, w00i);
        w10[lane] = schur4_zf_state_c16(w10r, w10i);
        w01[lane] = schur4_zf_state_c16(w01r, w01i);
        w11[lane] = schur4_zf_state_c16(w11r, w11i);
        y0[lane] = schur4_zf_state_c16(y0r, y0i);
        y1[lane] = schur4_zf_state_c16(y1r, y1i);
        b2v[lane] = schur4_zf_state_c16((schur4_zf_state_t)b_re[2][lane],
                                        (schur4_zf_state_t)b_im[2][lane]);
        b3v[lane] = schur4_zf_state_c16((schur4_zf_state_t)b_re[3][lane],
                                        (schur4_zf_state_t)b_im[3][lane]);
        a22r[lane] = (schur4_zf_state_t)a_re[10][lane];
        a23r[lane] = (schur4_zf_state_t)a_re[11][lane];
        a23i[lane] = (schur4_zf_state_t)a_im[11][lane];
        a33r[lane] = (schur4_zf_state_t)a_re[15][lane];
    }

    schur4_zf_ext_inner_conj2_q15(e02, e12, w00, w10, tmp0, tmp1, inner);
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        s00[lane] = schur4_zf_state_c16(
            SCHUR4_ZF_CAST_STATE(SCHUR4_ZF_WIDE(a22r[lane]) -
                                 inner[lane].r),
            0);
    }

    schur4_zf_ext_inner_conj2_q15(e02, e12, w01, w11, tmp0, tmp1, inner);
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        s01[lane] = schur4_zf_state_c16(
            SCHUR4_ZF_CAST_STATE(SCHUR4_ZF_WIDE(a23r[lane]) -
                                 inner[lane].r),
            SCHUR4_ZF_CAST_STATE(SCHUR4_ZF_WIDE(a23i[lane]) -
                                 inner[lane].i));
    }

    schur4_zf_ext_inner_conj2_q15(e03, e13, w01, w11, tmp0, tmp1, inner);
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        s11[lane] = schur4_zf_state_c16(
            SCHUR4_ZF_CAST_STATE(SCHUR4_ZF_WIDE(a33r[lane]) -
                                 inner[lane].r),
            0);
    }

    schur4_zf_ext_inner_conj2_q15(e02, e12, y0, y1, tmp0, tmp1, inner);
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        r0[lane] = schur4_zf_state_c16(
            SCHUR4_ZF_CAST_STATE(SCHUR4_ZF_WIDE(b2v[lane].r) -
                                 inner[lane].r),
            SCHUR4_ZF_CAST_STATE(SCHUR4_ZF_WIDE(b2v[lane].i) -
                                 inner[lane].i));
    }

    schur4_zf_ext_inner_conj2_q15(e03, e13, y0, y1, tmp0, tmp1, inner);
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        r1[lane] = schur4_zf_state_c16(
            SCHUR4_ZF_CAST_STATE(SCHUR4_ZF_WIDE(b3v[lane].r) -
                                 inner[lane].r),
            SCHUR4_ZF_CAST_STATE(SCHUR4_ZF_WIDE(b3v[lane].i) -
                                 inner[lane].i));
    }

    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        const schur4_zf_state_t s00r = s00[lane].r;
        const schur4_zf_state_t s01r = s01[lane].r;
        const schur4_zf_state_t s01i = s01[lane].i;
        const schur4_zf_state_t s11r = s11[lane].r;
        schur4_zf_state_t x2r, x2i, x3r, x3i;

        const schur4_zf_num_t s_det_q30 =
            SCHUR4_ZF_DET2_Q30(s00r, s01r, s01i, s11r);
        const schur4_q15_recip_scale_t s_det_scale =
            schur4_q15_recip_scale_from_den(s_det_q30);

        SCHUR4_ZF_SOLVE2_EXPLICIT(s00r, s01r, s01i, s11r, s_det_scale,
                                  r0[lane].r, r0[lane].i,
                                  r1[lane].r, r1[lane].i,
                                  x2r, x2i, x3r, x3i);
        x2[lane] = schur4_zf_state_c16(x2r, x2i);
        x3[lane] = schur4_zf_state_c16(x3r, x3i);
        top0[lane] = y0[lane];
        top1[lane] = y1[lane];
    }

    schur4_zf_ext_mul4_q15(w00, x2, tmp0);
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        top0[lane].r = (int16_t)((int32_t)top0[lane].r - tmp0[lane].r);
        top0[lane].i = (int16_t)((int32_t)top0[lane].i - tmp0[lane].i);
    }
    schur4_zf_ext_mul4_q15(w01, x3, tmp0);
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        top0[lane].r = (int16_t)((int32_t)top0[lane].r - tmp0[lane].r);
        top0[lane].i = (int16_t)((int32_t)top0[lane].i - tmp0[lane].i);
    }
    schur4_zf_ext_mul4_q15(w10, x2, tmp0);
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        top1[lane].r = (int16_t)((int32_t)top1[lane].r - tmp0[lane].r);
        top1[lane].i = (int16_t)((int32_t)top1[lane].i - tmp0[lane].i);
    }
    schur4_zf_ext_mul4_q15(w11, x3, tmp0);
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        top1[lane].r = (int16_t)((int32_t)top1[lane].r - tmp0[lane].r);
        top1[lane].i = (int16_t)((int32_t)top1[lane].i - tmp0[lane].i);
    }

    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        mimo_c16_t *x = &x_block[lane * MIMO_N4];

        x[0] = mimo_make_c16(mimo_solve_store_i16(top0[lane].r),
                             mimo_solve_store_i16(top0[lane].i));
        x[1] = mimo_make_c16(mimo_solve_store_i16(top1[lane].r),
                             mimo_solve_store_i16(top1[lane].i));
        x[2] = mimo_make_c16(mimo_solve_store_i16(x2[lane].r),
                             mimo_solve_store_i16(x2[lane].i));
        x[3] = mimo_make_c16(mimo_solve_store_i16(x3[lane].r),
                             mimo_solve_store_i16(x3[lane].i));
    }
}
#endif
#endif

#if MIMO_USE_SOLVE4_SCHUR_ZF_RELOAD_MICRO
static MIMO_NOINLINE void schur4_solve_lane_zf_reload_micro_q15(
    const int32_t (*restrict a_re)[MIMO_SITE4_LANES],
    const int32_t (*restrict a_im)[MIMO_SITE4_LANES],
    const int32_t (*restrict b_re)[MIMO_SITE4_LANES],
    const int32_t (*restrict b_im)[MIMO_SITE4_LANES],
    int lane,
    mimo_c16_t *restrict x)
{
#if MIMO_USE_SOLVE4_SCHUR_ZF_RELOAD_MICRO == 2
#define SCHUR4_ZF_RELOAD_A_RE(S_) \
    ((schur4_zf_state_t)a_re[(S_)][lane])
#define SCHUR4_ZF_RELOAD_A_IM(S_) \
    ((schur4_zf_state_t)a_im[(S_)][lane])
#define SCHUR4_ZF_RELOAD_B_RE(S_) \
    ((schur4_zf_state_t)b_re[(S_)][lane])
#define SCHUR4_ZF_RELOAD_B_IM(S_) \
    ((schur4_zf_state_t)b_im[(S_)][lane])
#else
#define SCHUR4_ZF_RELOAD_A_RE(S_) \
    ((schur4_zf_state_t)(*(const volatile int32_t *)&a_re[(S_)][lane]))
#define SCHUR4_ZF_RELOAD_A_IM(S_) \
    ((schur4_zf_state_t)(*(const volatile int32_t *)&a_im[(S_)][lane]))
#define SCHUR4_ZF_RELOAD_B_RE(S_) \
    ((schur4_zf_state_t)(*(const volatile int32_t *)&b_re[(S_)][lane]))
#define SCHUR4_ZF_RELOAD_B_IM(S_) \
    ((schur4_zf_state_t)(*(const volatile int32_t *)&b_im[(S_)][lane]))
#endif

    const schur4_zf_num_t b_det_q30 =
        SCHUR4_ZF_DET2_Q30(SCHUR4_ZF_RELOAD_A_RE(0),
                           SCHUR4_ZF_RELOAD_A_RE(1),
                           SCHUR4_ZF_RELOAD_A_IM(1),
                           SCHUR4_ZF_RELOAD_A_RE(5));
    const schur4_q15_recip_scale_t b_det_scale =
        schur4_q15_recip_scale_from_den(b_det_q30);
    schur4_zf_state_t w00r, w00i, w10r, w10i;
    schur4_zf_state_t w01r, w01i, w11r, w11i;
    schur4_zf_state_t y0r, y0i, y1r, y1i;
    schur4_zf_state_t inner_r, inner_i;

    SCHUR4_ZF_SOLVE2_EXPLICIT(SCHUR4_ZF_RELOAD_A_RE(0),
                              SCHUR4_ZF_RELOAD_A_RE(1),
                              SCHUR4_ZF_RELOAD_A_IM(1),
                              SCHUR4_ZF_RELOAD_A_RE(5), b_det_scale,
                              SCHUR4_ZF_RELOAD_A_RE(2),
                              SCHUR4_ZF_RELOAD_A_IM(2),
                              SCHUR4_ZF_RELOAD_A_RE(6),
                              SCHUR4_ZF_RELOAD_A_IM(6),
                              w00r, w00i, w10r, w10i);
    SCHUR4_ZF_SOLVE2_EXPLICIT(SCHUR4_ZF_RELOAD_A_RE(0),
                              SCHUR4_ZF_RELOAD_A_RE(1),
                              SCHUR4_ZF_RELOAD_A_IM(1),
                              SCHUR4_ZF_RELOAD_A_RE(5), b_det_scale,
                              SCHUR4_ZF_RELOAD_A_RE(3),
                              SCHUR4_ZF_RELOAD_A_IM(3),
                              SCHUR4_ZF_RELOAD_A_RE(7),
                              SCHUR4_ZF_RELOAD_A_IM(7),
                              w01r, w01i, w11r, w11i);
    SCHUR4_ZF_SOLVE2_EXPLICIT(SCHUR4_ZF_RELOAD_A_RE(0),
                              SCHUR4_ZF_RELOAD_A_RE(1),
                              SCHUR4_ZF_RELOAD_A_IM(1),
                              SCHUR4_ZF_RELOAD_A_RE(5), b_det_scale,
                              SCHUR4_ZF_RELOAD_B_RE(0),
                              SCHUR4_ZF_RELOAD_B_IM(0),
                              SCHUR4_ZF_RELOAD_B_RE(1),
                              SCHUR4_ZF_RELOAD_B_IM(1),
                              y0r, y0i, y1r, y1i);

    SCHUR4_ZF_INNER_EXPLICIT(SCHUR4_ZF_RELOAD_A_RE(2),
                             SCHUR4_ZF_RELOAD_A_IM(2),
                             SCHUR4_ZF_RELOAD_A_RE(6),
                             SCHUR4_ZF_RELOAD_A_IM(6),
                             w00r, w00i, w10r, w10i,
                             inner_r, inner_i);
    const schur4_zf_state_t s00r = SCHUR4_ZF_CAST_STATE(
        SCHUR4_ZF_WIDE(SCHUR4_ZF_RELOAD_A_RE(10)) - inner_r);

    SCHUR4_ZF_INNER_EXPLICIT(SCHUR4_ZF_RELOAD_A_RE(2),
                             SCHUR4_ZF_RELOAD_A_IM(2),
                             SCHUR4_ZF_RELOAD_A_RE(6),
                             SCHUR4_ZF_RELOAD_A_IM(6),
                             w01r, w01i, w11r, w11i,
                             inner_r, inner_i);
    const schur4_zf_state_t s01r = SCHUR4_ZF_CAST_STATE(
        SCHUR4_ZF_WIDE(SCHUR4_ZF_RELOAD_A_RE(11)) - inner_r);
    const schur4_zf_state_t s01i = SCHUR4_ZF_CAST_STATE(
        SCHUR4_ZF_WIDE(SCHUR4_ZF_RELOAD_A_IM(11)) - inner_i);

    SCHUR4_ZF_INNER_EXPLICIT(SCHUR4_ZF_RELOAD_A_RE(3),
                             SCHUR4_ZF_RELOAD_A_IM(3),
                             SCHUR4_ZF_RELOAD_A_RE(7),
                             SCHUR4_ZF_RELOAD_A_IM(7),
                             w01r, w01i, w11r, w11i,
                             inner_r, inner_i);
    const schur4_zf_state_t s11r = SCHUR4_ZF_CAST_STATE(
        SCHUR4_ZF_WIDE(SCHUR4_ZF_RELOAD_A_RE(15)) - inner_r);

    SCHUR4_ZF_INNER_EXPLICIT(SCHUR4_ZF_RELOAD_A_RE(2),
                             SCHUR4_ZF_RELOAD_A_IM(2),
                             SCHUR4_ZF_RELOAD_A_RE(6),
                             SCHUR4_ZF_RELOAD_A_IM(6),
                             y0r, y0i, y1r, y1i,
                             inner_r, inner_i);
    const schur4_zf_state_t r0r = SCHUR4_ZF_CAST_STATE(
        SCHUR4_ZF_WIDE(SCHUR4_ZF_RELOAD_B_RE(2)) - inner_r);
    const schur4_zf_state_t r0i = SCHUR4_ZF_CAST_STATE(
        SCHUR4_ZF_WIDE(SCHUR4_ZF_RELOAD_B_IM(2)) - inner_i);

    SCHUR4_ZF_INNER_EXPLICIT(SCHUR4_ZF_RELOAD_A_RE(3),
                             SCHUR4_ZF_RELOAD_A_IM(3),
                             SCHUR4_ZF_RELOAD_A_RE(7),
                             SCHUR4_ZF_RELOAD_A_IM(7),
                             y0r, y0i, y1r, y1i,
                             inner_r, inner_i);
    const schur4_zf_state_t r1r = SCHUR4_ZF_CAST_STATE(
        SCHUR4_ZF_WIDE(SCHUR4_ZF_RELOAD_B_RE(3)) - inner_r);
    const schur4_zf_state_t r1i = SCHUR4_ZF_CAST_STATE(
        SCHUR4_ZF_WIDE(SCHUR4_ZF_RELOAD_B_IM(3)) - inner_i);

    const schur4_zf_num_t s_det_q30 =
        SCHUR4_ZF_DET2_Q30(s00r, s01r, s01i, s11r);
    const schur4_q15_recip_scale_t s_det_scale =
        schur4_q15_recip_scale_from_den(s_det_q30);
    schur4_zf_state_t x2r, x2i, x3r, x3i;

    SCHUR4_ZF_SOLVE2_EXPLICIT(s00r, s01r, s01i, s11r, s_det_scale,
                              r0r, r0i, r1r, r1i,
                              x2r, x2i, x3r, x3i);

    schur4_zf_state_t top0r = y0r;
    schur4_zf_state_t top0i = y0i;
    schur4_zf_state_t top1r = y1r;
    schur4_zf_state_t top1i = y1i;

    SCHUR4_ZF_SUB_MUL_Q15_EXPLICIT(top0r, top0i, w00r, w00i, x2r, x2i);
    SCHUR4_ZF_SUB_MUL_Q15_EXPLICIT(top0r, top0i, w01r, w01i, x3r, x3i);
    SCHUR4_ZF_SUB_MUL_Q15_EXPLICIT(top1r, top1i, w10r, w10i, x2r, x2i);
    SCHUR4_ZF_SUB_MUL_Q15_EXPLICIT(top1r, top1i, w11r, w11i, x3r, x3i);

    x[0].r = mimo_solve_store_i16((int32_t)top0r);
    x[0].i = mimo_solve_store_i16((int32_t)top0i);
    x[1].r = mimo_solve_store_i16((int32_t)top1r);
    x[1].i = mimo_solve_store_i16((int32_t)top1i);
    x[2].r = mimo_solve_store_i16((int32_t)x2r);
    x[2].i = mimo_solve_store_i16((int32_t)x2i);
    x[3].r = mimo_solve_store_i16((int32_t)x3r);
    x[3].i = mimo_solve_store_i16((int32_t)x3i);

#undef SCHUR4_ZF_RELOAD_B_IM
#undef SCHUR4_ZF_RELOAD_B_RE
#undef SCHUR4_ZF_RELOAD_A_IM
#undef SCHUR4_ZF_RELOAD_A_RE
}
#endif

#undef SCHUR4_ZF_SUB_MUL_Q15_EXPLICIT
#undef SCHUR4_ZF_INNER_EXPLICIT
#undef SCHUR4_ZF_SOLVE2_EXPLICIT
#undef SCHUR4_ZF_SCALE_Q15
#undef SCHUR4_ZF_HELPER_ATTR
#undef SCHUR4_ZF_DET2_Q30
#undef SCHUR4_ZF_MUL_Q30
#undef SCHUR4_ZF_NUM_CAST
#undef SCHUR4_ZF_WIDE
#undef SCHUR4_ZF_CAST_STATE
#endif

void mimo_solve_normal4_site4_schur_q15(
    mimo_c32_t a_lane[restrict MIMO_SITE4_LANES][MIMO_N4 * MIMO_N4],
    mimo_c32_t b_lane[restrict MIMO_SITE4_LANES][MIMO_N4],
    int mmse,
    mimo_c16_t *restrict x_block)
{
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        mimo_collect_schur4_q15(a_lane[lane], b_lane[lane], mmse);

        mimo_c64_t a00 = schur4_c64_from_c32(a_lane[lane][0]);
        mimo_c64_t a01 = schur4_c64_from_c32(a_lane[lane][1]);
        mimo_c64_t a02 = schur4_c64_from_c32(a_lane[lane][2]);
        mimo_c64_t a03 = schur4_c64_from_c32(a_lane[lane][3]);
        mimo_c64_t a11 = schur4_c64_from_c32(a_lane[lane][5]);
        mimo_c64_t a12 = schur4_c64_from_c32(a_lane[lane][6]);
        mimo_c64_t a13 = schur4_c64_from_c32(a_lane[lane][7]);
        mimo_c64_t a22 = schur4_c64_from_c32(a_lane[lane][10]);
        mimo_c64_t a23 = schur4_c64_from_c32(a_lane[lane][11]);
        mimo_c64_t a33 = schur4_c64_from_c32(a_lane[lane][15]);
        const mimo_c64_t b0 = schur4_c64_from_c32(b_lane[lane][0]);
        const mimo_c64_t b1 = schur4_c64_from_c32(b_lane[lane][1]);
        const mimo_c64_t b2 = schur4_c64_from_c32(b_lane[lane][2]);
        const mimo_c64_t b3 = schur4_c64_from_c32(b_lane[lane][3]);
        mimo_c16_t *x = &x_block[lane * MIMO_N4];

        schur4_solve_lane_q15(a00, a01, a02, a03, a11, a12, a13,
                              a22, a23, a33, b0, b1, b2, b3, mmse, x);
    }
}

void mimo_solve_normal4_site4_soa_schur_q15(
    mimo_site4_soa4_t *restrict work,
    int mmse,
    mimo_c16_t *restrict x_block)
{
    int32_t (*restrict a_re)[MIMO_SITE4_LANES] = work->a_re;
    int32_t (*restrict a_im)[MIMO_SITE4_LANES] = work->a_im;
    int32_t (*restrict b_re)[MIMO_SITE4_LANES] = work->b_re;
    int32_t (*restrict b_im)[MIMO_SITE4_LANES] = work->b_im;

    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        mimo_c64_t a00 = c64_make(a_re[0][lane], 0);
        mimo_c64_t a01 = c64_make(a_re[1][lane], a_im[1][lane]);
        mimo_c64_t a02 = c64_make(a_re[2][lane], a_im[2][lane]);
        mimo_c64_t a03 = c64_make(a_re[3][lane], a_im[3][lane]);
        mimo_c64_t a11 = c64_make(a_re[5][lane], 0);
        mimo_c64_t a12 = c64_make(a_re[6][lane], a_im[6][lane]);
        mimo_c64_t a13 = c64_make(a_re[7][lane], a_im[7][lane]);
        mimo_c64_t a22 = c64_make(a_re[10][lane], 0);
        mimo_c64_t a23 = c64_make(a_re[11][lane], a_im[11][lane]);
        mimo_c64_t a33 = c64_make(a_re[15][lane], 0);
        const mimo_c64_t b0 = c64_make(b_re[0][lane], b_im[0][lane]);
        const mimo_c64_t b1 = c64_make(b_re[1][lane], b_im[1][lane]);
        const mimo_c64_t b2 = c64_make(b_re[2][lane], b_im[2][lane]);
        const mimo_c64_t b3 = c64_make(b_re[3][lane], b_im[3][lane]);
        mimo_c16_t *x = &x_block[lane * MIMO_N4];

        schur4_solve_lane_q15(a00, a01, a02, a03, a11, a12, a13,
                              a22, a23, a33, b0, b1, b2, b3, mmse, x);
    }
}

void mimo_solve_normal4_site4_soa_schur_mmse_q15(
    mimo_site4_soa4_t *restrict work,
    mimo_c16_t *restrict x_block)
{
    int32_t (*restrict a_re)[MIMO_SITE4_LANES] = work->a_re;
    int32_t (*restrict a_im)[MIMO_SITE4_LANES] = work->a_im;
    int32_t (*restrict b_re)[MIMO_SITE4_LANES] = work->b_re;
    int32_t (*restrict b_im)[MIMO_SITE4_LANES] = work->b_im;

    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        mimo_c64_t a00 = c64_make(a_re[0][lane], 0);
        mimo_c64_t a01 = c64_make(a_re[1][lane], a_im[1][lane]);
        mimo_c64_t a02 = c64_make(a_re[2][lane], a_im[2][lane]);
        mimo_c64_t a03 = c64_make(a_re[3][lane], a_im[3][lane]);
        mimo_c64_t a11 = c64_make(a_re[5][lane], 0);
        mimo_c64_t a12 = c64_make(a_re[6][lane], a_im[6][lane]);
        mimo_c64_t a13 = c64_make(a_re[7][lane], a_im[7][lane]);
        mimo_c64_t a22 = c64_make(a_re[10][lane], 0);
        mimo_c64_t a23 = c64_make(a_re[11][lane], a_im[11][lane]);
        mimo_c64_t a33 = c64_make(a_re[15][lane], 0);
        const mimo_c64_t b0 = c64_make(b_re[0][lane], b_im[0][lane]);
        const mimo_c64_t b1 = c64_make(b_re[1][lane], b_im[1][lane]);
        const mimo_c64_t b2 = c64_make(b_re[2][lane], b_im[2][lane]);
        const mimo_c64_t b3 = c64_make(b_re[3][lane], b_im[3][lane]);
        mimo_c16_t *x = &x_block[lane * MIMO_N4];

        schur4_solve_lane_q15(a00, a01, a02, a03, a11, a12, a13,
                              a22, a23, a33, b0, b1, b2, b3, 1, x);
    }
}

void mimo_solve_normal4_site4_soa_schur_zf_q15(
    mimo_site4_soa4_t *restrict work,
    mimo_c16_t *restrict x_block)
{
#if MIMO_USE_SOLVE4_SCHUR_ZF_HANDASM
    mimo_solve_normal4_site4_soa_schur_zf_handasm_q15(work, x_block);
    return;
#else
    int32_t (*restrict a_re)[MIMO_SITE4_LANES] = work->a_re;
    int32_t (*restrict a_im)[MIMO_SITE4_LANES] = work->a_im;
    int32_t (*restrict b_re)[MIMO_SITE4_LANES] = work->b_re;
    int32_t (*restrict b_im)[MIMO_SITE4_LANES] = work->b_im;

#if MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER == 3 || \
    MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER == 4 || \
    MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER == 5
    schur4_solve_site4_zf_ext_inner3_q15(a_re, a_im, b_re, b_im, x_block);
    return;
#elif MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER == 2
    schur4_solve_site4_zf_ext_inner2_q15(a_re, a_im, b_re, b_im, x_block);
    return;
#elif MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER
    schur4_solve_site4_zf_ext_inner_q15(a_re, a_im, b_re, b_im, x_block);
    return;
#endif

    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
#if MIMO_USE_SOLVE4_SCHUR_ZF_RELOAD_MICRO
        mimo_c16_t *x = &x_block[lane * MIMO_N4];

        schur4_solve_lane_zf_reload_micro_q15(
            a_re, a_im, b_re, b_im, lane, x);
#else
#if MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED
#if MIMO_USE_SOLVE4_SCHUR_ZF_EXPLICIT
        const schur4_zf_state_t a00r = a_re[0][lane];
        const schur4_zf_state_t a01r = a_re[1][lane];
        const schur4_zf_state_t a01i = a_im[1][lane];
        const schur4_zf_state_t a02r = a_re[2][lane];
        const schur4_zf_state_t a02i = a_im[2][lane];
        const schur4_zf_state_t a03r = a_re[3][lane];
        const schur4_zf_state_t a03i = a_im[3][lane];
        const schur4_zf_state_t a11r = a_re[4][lane];
        const schur4_zf_state_t a12r = a_re[5][lane];
        const schur4_zf_state_t a12i = a_im[5][lane];
        const schur4_zf_state_t a13r = a_re[6][lane];
        const schur4_zf_state_t a13i = a_im[6][lane];
        const schur4_zf_state_t a22r = a_re[7][lane];
        const schur4_zf_state_t a23r = a_re[8][lane];
        const schur4_zf_state_t a23i = a_im[8][lane];
        const schur4_zf_state_t a33r = a_re[9][lane];
#else
        mimo_c64_t a00 = c64_make(a_re[0][lane], 0);
        mimo_c64_t a01 = c64_make(a_re[1][lane], a_im[1][lane]);
        mimo_c64_t a02 = c64_make(a_re[2][lane], a_im[2][lane]);
        mimo_c64_t a03 = c64_make(a_re[3][lane], a_im[3][lane]);
        mimo_c64_t a11 = c64_make(a_re[4][lane], 0);
        mimo_c64_t a12 = c64_make(a_re[5][lane], a_im[5][lane]);
        mimo_c64_t a13 = c64_make(a_re[6][lane], a_im[6][lane]);
        mimo_c64_t a22 = c64_make(a_re[7][lane], 0);
        mimo_c64_t a23 = c64_make(a_re[8][lane], a_im[8][lane]);
        mimo_c64_t a33 = c64_make(a_re[9][lane], 0);
#endif
#else
#if MIMO_USE_SOLVE4_SCHUR_ZF_EXPLICIT
        const schur4_zf_state_t a00r = a_re[0][lane];
        const schur4_zf_state_t a01r = a_re[1][lane];
        const schur4_zf_state_t a01i = a_im[1][lane];
        const schur4_zf_state_t a02r = a_re[2][lane];
        const schur4_zf_state_t a02i = a_im[2][lane];
        const schur4_zf_state_t a03r = a_re[3][lane];
        const schur4_zf_state_t a03i = a_im[3][lane];
        const schur4_zf_state_t a11r = a_re[5][lane];
        const schur4_zf_state_t a12r = a_re[6][lane];
        const schur4_zf_state_t a12i = a_im[6][lane];
        const schur4_zf_state_t a13r = a_re[7][lane];
        const schur4_zf_state_t a13i = a_im[7][lane];
        const schur4_zf_state_t a22r = a_re[10][lane];
        const schur4_zf_state_t a23r = a_re[11][lane];
        const schur4_zf_state_t a23i = a_im[11][lane];
        const schur4_zf_state_t a33r = a_re[15][lane];
#else
        mimo_c64_t a00 = c64_make(a_re[0][lane], 0);
        mimo_c64_t a01 = c64_make(a_re[1][lane], a_im[1][lane]);
        mimo_c64_t a02 = c64_make(a_re[2][lane], a_im[2][lane]);
        mimo_c64_t a03 = c64_make(a_re[3][lane], a_im[3][lane]);
        mimo_c64_t a11 = c64_make(a_re[5][lane], 0);
        mimo_c64_t a12 = c64_make(a_re[6][lane], a_im[6][lane]);
        mimo_c64_t a13 = c64_make(a_re[7][lane], a_im[7][lane]);
        mimo_c64_t a22 = c64_make(a_re[10][lane], 0);
        mimo_c64_t a23 = c64_make(a_re[11][lane], a_im[11][lane]);
        mimo_c64_t a33 = c64_make(a_re[15][lane], 0);
#endif
#endif
#if MIMO_USE_SOLVE4_SCHUR_ZF_EXPLICIT
        const schur4_zf_state_t b0r = b_re[0][lane];
        const schur4_zf_state_t b0i = b_im[0][lane];
        const schur4_zf_state_t b1r = b_re[1][lane];
        const schur4_zf_state_t b1i = b_im[1][lane];
        const schur4_zf_state_t b2r = b_re[2][lane];
        const schur4_zf_state_t b2i = b_im[2][lane];
        const schur4_zf_state_t b3r = b_re[3][lane];
        const schur4_zf_state_t b3i = b_im[3][lane];
        mimo_c16_t *x = &x_block[lane * MIMO_N4];

        schur4_solve_lane_zf_explicit_q15(
            a00r, a01r, a01i, a02r, a02i, a03r, a03i, a11r,
            a12r, a12i, a13r, a13i, a22r, a23r, a23i, a33r,
            b0r, b0i, b1r, b1i, b2r, b2i, b3r, b3i, x);
#else
        const mimo_c64_t b0 = c64_make(b_re[0][lane], b_im[0][lane]);
        const mimo_c64_t b1 = c64_make(b_re[1][lane], b_im[1][lane]);
        const mimo_c64_t b2 = c64_make(b_re[2][lane], b_im[2][lane]);
        const mimo_c64_t b3 = c64_make(b_re[3][lane], b_im[3][lane]);
        mimo_c16_t *x = &x_block[lane * MIMO_N4];

        schur4_solve_lane_q15(a00, a01, a02, a03, a11, a12, a13,
                              a22, a23, a33, b0, b1, b2, b3, 0, x);
#endif
#endif
    }
#endif
}

#if MIMO_USE_SOLVE4_LDLH
void mimo_solve_normal4_site4_ldlh_q15(
    mimo_c32_t a_lane[restrict MIMO_SITE4_LANES][MIMO_N4 * MIMO_N4],
    mimo_c32_t b_lane[restrict MIMO_SITE4_LANES][MIMO_N4],
    int mmse,
    mimo_c16_t *restrict x_block)
{
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        mimo_collect_schur4_q15(a_lane[lane], b_lane[lane], mmse);

        int32_t d[MIMO_N4];
        mimo_c32_t z[MIMO_N4];
        mimo_c32_t y[MIMO_N4];
        mimo_c32_t sol[MIMO_N4];

        if (mmse) {
            for (int i = 0; i < MIMO_N4; ++i) {
                const int ii = i * MIMO_N4 + i;
                a_lane[lane][ii].r += MIMO_MMSE_LAMBDA_Q15;
                a_lane[lane][ii].i = 0;
            }
        } else {
            for (int i = 0; i < MIMO_N4; ++i) {
                a_lane[lane][i * MIMO_N4 + i].i = 0;
            }
        }

        for (int i = 0; i < MIMO_N4; ++i) {
            int32_t diag = a_lane[lane][i * MIMO_N4 + i].r;

            for (int k = 0; k < i; ++k) {
                mimo_c32_t term = c32_mul_q30_q30(
                    a_lane[lane][i * MIMO_N4 + k],
                    c32_conj(a_lane[lane][i * MIMO_N4 + k]));
                term = c32_mul_real_q30_to_q15(term, d[k]);
                diag -= term.r;
            }

            d[i] = safe_pivot_i32(diag);

            for (int j = i + 1; j < MIMO_N4; ++j) {
                mimo_c32_t num = a_lane[lane][j * MIMO_N4 + i];

                for (int k = 0; k < i; ++k) {
                    mimo_c32_t term = c32_mul_q30_q30(
                        a_lane[lane][j * MIMO_N4 + k],
                        c32_conj(a_lane[lane][i * MIMO_N4 + k]));
                    term = c32_mul_real_q30_to_q15(term, d[k]);
                    num = c32_sub(num, term);
                }

                a_lane[lane][j * MIMO_N4 + i] = c32_make(
                    (int32_t)div_to_q30_i64(num.r, d[i]),
                    (int32_t)div_to_q30_i64(num.i, d[i]));
            }
        }

        for (int i = 0; i < MIMO_N4; ++i) {
            mimo_c32_t sum = b_lane[lane][i];

            for (int k = 0; k < i; ++k) {
                sum = c32_sub(
                    sum,
                    c32_mul_q30_q15(a_lane[lane][i * MIMO_N4 + k],
                                     z[k]));
            }

            z[i] = sum;
            y[i] = c32_div_real_to_q15_exact(sum, d[i]);
        }

        for (int i = MIMO_N4 - 1; i >= 0; --i) {
            mimo_c32_t sum = y[i];

            for (int k = i + 1; k < MIMO_N4; ++k) {
                sum = c32_sub(
                    sum,
                    c32_mul_q30_q15(
                        c32_conj(a_lane[lane][k * MIMO_N4 + i]), sol[k]));
            }

            sol[i] = sum;
        }

        for (int i = 0; i < MIMO_N4; ++i) {
            x_block[lane * MIMO_N4 + i] =
                mimo_make_c16(mimo_solve_store_i16(sol[i].r),
                              mimo_solve_store_i16(sol[i].i));
        }
    }
}
#endif
#endif

void mimo_solve_normal4_site4_q15(
    mimo_c32_t a_lane[restrict MIMO_SITE4_LANES][MIMO_N4 * MIMO_N4],
    mimo_c32_t b_lane[restrict MIMO_SITE4_LANES][MIMO_N4],
    int mmse,
    mimo_c16_t *restrict x_block)
{
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        mimo_collect_schur4_q15(a_lane[lane], b_lane[lane], mmse);
    }

    if (mmse) {
        for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
            for (int i = 0; i < MIMO_N4; ++i) {
                a_lane[lane][i * MIMO_N4 + i].r += MIMO_MMSE_LAMBDA_Q15;
                a_lane[lane][i * MIMO_N4 + i].i = 0;
            }
        }
    } else {
        for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
            for (int i = 0; i < MIMO_N4; ++i) {
                a_lane[lane][i * MIMO_N4 + i].i = 0;
            }
        }
    }

    for (int k = 0; k < MIMO_N4; ++k) {
        for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
            const int32_t pivot =
                safe_pivot_i32(a_lane[lane][k * MIMO_N4 + k].r);
            a_lane[lane][k * MIMO_N4 + k].i = 0;

            for (int i = k + 1; i < MIMO_N4; ++i) {
                const mimo_c32_t factor_q30 = c32_make(
                    div_to_q30_i32(a_lane[lane][i * MIMO_N4 + k].r, pivot),
                    div_to_q30_i32(a_lane[lane][i * MIMO_N4 + k].i, pivot));

                for (int j = k + MIMO_USE_SOLVE_SKIP_LOWER_UPDATE;
                     j < MIMO_N4; ++j) {
                    const mimo_c32_t t =
                        c32_mul_q30_q15(factor_q30,
                                        a_lane[lane][k * MIMO_N4 + j]);
                    a_lane[lane][i * MIMO_N4 + j] =
                        c32_sub(a_lane[lane][i * MIMO_N4 + j], t);
                }

                const mimo_c32_t tb =
                    c32_mul_q30_q15(factor_q30, b_lane[lane][k]);
                b_lane[lane][i] = c32_sub(b_lane[lane][i], tb);
                a_lane[lane][i * MIMO_N4 + k] = c32_make(0, 0);
            }
        }
    }

    for (int i = MIMO_N4 - 1; i >= 0; --i) {
        for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
            mimo_c32_t sum = b_lane[lane][i];

            for (int j = i + 1; j < MIMO_N4; ++j) {
                sum = c32_sub(sum,
                              c32_mul_q15(a_lane[lane][i * MIMO_N4 + j],
                                          b_lane[lane][j]));
            }
            b_lane[lane][i] = c32_div_real_to_q15_exact(
                sum, safe_pivot_i32(a_lane[lane][i * MIMO_N4 + i].r));
        }
    }

    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        mimo_c16_t *x = &x_block[lane * MIMO_N4];

        for (int i = 0; i < MIMO_N4; ++i) {
            x[i] = mimo_make_c16(
                mimo_solve_store_i16(b_lane[lane][i].r),
                mimo_solve_store_i16(b_lane[lane][i].i));
        }
    }
}

#if MIMO_USE_SITE4_SOLVE_SOA
#if MIMO_USE_SITE4_SOLVE_SOA_UNROLL
static inline void mimo_solve_normal4_site4_soa_unroll_lane_q15(
    int32_t (*restrict a_re)[MIMO_SITE4_LANES],
    int32_t (*restrict a_im)[MIMO_SITE4_LANES],
    int32_t (*restrict b_re)[MIMO_SITE4_LANES],
    int32_t (*restrict b_im)[MIMO_SITE4_LANES],
    int lane,
    int mmse,
    mimo_c16_t *restrict x)
{
    mimo_c32_t m00 = c32_make(a_re[0][lane], a_im[0][lane]);
    mimo_c32_t m01 = c32_make(a_re[1][lane], a_im[1][lane]);
    mimo_c32_t m02 = c32_make(a_re[2][lane], a_im[2][lane]);
    mimo_c32_t m03 = c32_make(a_re[3][lane], a_im[3][lane]);
    mimo_c32_t m10 = c32_make(a_re[4][lane], a_im[4][lane]);
    mimo_c32_t m11 = c32_make(a_re[5][lane], a_im[5][lane]);
    mimo_c32_t m12 = c32_make(a_re[6][lane], a_im[6][lane]);
    mimo_c32_t m13 = c32_make(a_re[7][lane], a_im[7][lane]);
    mimo_c32_t m20 = c32_make(a_re[8][lane], a_im[8][lane]);
    mimo_c32_t m21 = c32_make(a_re[9][lane], a_im[9][lane]);
    mimo_c32_t m22 = c32_make(a_re[10][lane], a_im[10][lane]);
    mimo_c32_t m23 = c32_make(a_re[11][lane], a_im[11][lane]);
    mimo_c32_t m30 = c32_make(a_re[12][lane], a_im[12][lane]);
    mimo_c32_t m31 = c32_make(a_re[13][lane], a_im[13][lane]);
    mimo_c32_t m32 = c32_make(a_re[14][lane], a_im[14][lane]);
    mimo_c32_t m33 = c32_make(a_re[15][lane], a_im[15][lane]);
    mimo_c32_t r0 = c32_make(b_re[0][lane], b_im[0][lane]);
    mimo_c32_t r1 = c32_make(b_re[1][lane], b_im[1][lane]);
    mimo_c32_t r2 = c32_make(b_re[2][lane], b_im[2][lane]);
    mimo_c32_t r3 = c32_make(b_re[3][lane], b_im[3][lane]);

    if (mmse) {
        m00.r += MIMO_MMSE_LAMBDA_Q15;
        m11.r += MIMO_MMSE_LAMBDA_Q15;
        m22.r += MIMO_MMSE_LAMBDA_Q15;
        m33.r += MIMO_MMSE_LAMBDA_Q15;
    }
    m00.i = 0;
    m11.i = 0;
    m22.i = 0;
    m33.i = 0;

    const int32_t p0 = safe_pivot_i32(m00.r);
    mimo_c32_t f = c32_make(div_to_q30_i32(m10.r, p0),
                            div_to_q30_i32(m10.i, p0));
    m11 = c32_sub(m11, c32_mul_q30_q15(f, m01));
    m12 = c32_sub(m12, c32_mul_q30_q15(f, m02));
    m13 = c32_sub(m13, c32_mul_q30_q15(f, m03));
    r1 = c32_sub(r1, c32_mul_q30_q15(f, r0));

    f = c32_make(div_to_q30_i32(m20.r, p0),
                 div_to_q30_i32(m20.i, p0));
    m21 = c32_sub(m21, c32_mul_q30_q15(f, m01));
    m22 = c32_sub(m22, c32_mul_q30_q15(f, m02));
    m23 = c32_sub(m23, c32_mul_q30_q15(f, m03));
    r2 = c32_sub(r2, c32_mul_q30_q15(f, r0));

    f = c32_make(div_to_q30_i32(m30.r, p0),
                 div_to_q30_i32(m30.i, p0));
    m31 = c32_sub(m31, c32_mul_q30_q15(f, m01));
    m32 = c32_sub(m32, c32_mul_q30_q15(f, m02));
    m33 = c32_sub(m33, c32_mul_q30_q15(f, m03));
    r3 = c32_sub(r3, c32_mul_q30_q15(f, r0));

    m11.i = 0;
    const int32_t p1 = safe_pivot_i32(m11.r);
    f = c32_make(div_to_q30_i32(m21.r, p1),
                 div_to_q30_i32(m21.i, p1));
    m22 = c32_sub(m22, c32_mul_q30_q15(f, m12));
    m23 = c32_sub(m23, c32_mul_q30_q15(f, m13));
    r2 = c32_sub(r2, c32_mul_q30_q15(f, r1));

    f = c32_make(div_to_q30_i32(m31.r, p1),
                 div_to_q30_i32(m31.i, p1));
    m32 = c32_sub(m32, c32_mul_q30_q15(f, m12));
    m33 = c32_sub(m33, c32_mul_q30_q15(f, m13));
    r3 = c32_sub(r3, c32_mul_q30_q15(f, r1));

    m22.i = 0;
    const int32_t p2 = safe_pivot_i32(m22.r);
    f = c32_make(div_to_q30_i32(m32.r, p2),
                 div_to_q30_i32(m32.i, p2));
    m33 = c32_sub(m33, c32_mul_q30_q15(f, m23));
    r3 = c32_sub(r3, c32_mul_q30_q15(f, r2));

    m33.i = 0;
    const mimo_c32_t x3 = c32_div_real_to_q15_exact(
        r3, safe_pivot_i32(m33.r));
    mimo_c32_t sum = c32_sub(r2, c32_mul_q15(m23, x3));
    const mimo_c32_t x2 = c32_div_real_to_q15_exact(
        sum, safe_pivot_i32(m22.r));

    sum = c32_sub(r1, c32_mul_q15(m12, x2));
    sum = c32_sub(sum, c32_mul_q15(m13, x3));
    const mimo_c32_t x1 = c32_div_real_to_q15_exact(
        sum, safe_pivot_i32(m11.r));

    sum = c32_sub(r0, c32_mul_q15(m01, x1));
    sum = c32_sub(sum, c32_mul_q15(m02, x2));
    sum = c32_sub(sum, c32_mul_q15(m03, x3));
    const mimo_c32_t x0 = c32_div_real_to_q15_exact(
        sum, safe_pivot_i32(m00.r));

    x[0] = mimo_make_c16(mimo_solve_store_i16(x0.r),
                         mimo_solve_store_i16(x0.i));
    x[1] = mimo_make_c16(mimo_solve_store_i16(x1.r),
                         mimo_solve_store_i16(x1.i));
    x[2] = mimo_make_c16(mimo_solve_store_i16(x2.r),
                         mimo_solve_store_i16(x2.i));
    x[3] = mimo_make_c16(mimo_solve_store_i16(x3.r),
                         mimo_solve_store_i16(x3.i));
}
#endif

void mimo_solve_normal4_site4_soa_q15(mimo_site4_soa4_t *restrict work,
                                      int mmse,
                                      mimo_c16_t *restrict x_block)
{
    int32_t (*restrict a_re)[MIMO_SITE4_LANES] = work->a_re;
    int32_t (*restrict a_im)[MIMO_SITE4_LANES] = work->a_im;
    int32_t (*restrict b_re)[MIMO_SITE4_LANES] = work->b_re;
    int32_t (*restrict b_im)[MIMO_SITE4_LANES] = work->b_im;

#if MIMO_COLLECT_SCHUR_STATS
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        mimo_c32_t a[MIMO_N4 * MIMO_N4];
        mimo_c32_t b[MIMO_N4];

        for (int idx = 0; idx < MIMO_N4 * MIMO_N4; ++idx) {
            a[idx] = c32_make(a_re[idx][lane], a_im[idx][lane]);
        }
        for (int i = 0; i < MIMO_N4; ++i) {
            b[i] = c32_make(b_re[i][lane], b_im[i][lane]);
        }
        mimo_collect_schur4_q15(a, b, mmse);
    }
#endif

#if MIMO_USE_SITE4_SOLVE_SOA_UNROLL && \
    !MIMO_COLLECT_SOLVE_VALUE_STATS && !MIMO_COLLECT_SCHUR_STATS
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        mimo_solve_normal4_site4_soa_unroll_lane_q15(
            a_re, a_im, b_re, b_im, lane, mmse,
            &x_block[lane * MIMO_N4]);
    }
    return;
#endif

    if (mmse) {
        for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
            for (int i = 0; i < MIMO_N4; ++i) {
                const int idx = i * MIMO_N4 + i;
                a_re[idx][lane] += MIMO_MMSE_LAMBDA_Q15;
                a_im[idx][lane] = 0;
            }
        }
    } else {
        for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
            for (int i = 0; i < MIMO_N4; ++i) {
                a_im[i * MIMO_N4 + i][lane] = 0;
            }
        }
    }

    for (int k = 0; k < MIMO_N4; ++k) {
        for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
            const int kk = k * MIMO_N4 + k;
            const int32_t pivot = safe_pivot_i32(a_re[kk][lane]);
            a_im[kk][lane] = 0;

            for (int i = k + 1; i < MIMO_N4; ++i) {
                const int ik = i * MIMO_N4 + k;
                const mimo_c32_t factor_q30 = c32_make(
                    div_to_q30_i32(a_re[ik][lane], pivot),
                    div_to_q30_i32(a_im[ik][lane], pivot));

                for (int j = k + MIMO_USE_SOLVE_SKIP_LOWER_UPDATE;
                     j < MIMO_N4; ++j) {
                    const int kj = k * MIMO_N4 + j;
                    const int ij = i * MIMO_N4 + j;
                    const mimo_c32_t t = c32_mul_q30_q15(
                        factor_q30, c32_make(a_re[kj][lane],
                                             a_im[kj][lane]));

                    a_re[ij][lane] -= t.r;
                    a_im[ij][lane] -= t.i;
                }

                const mimo_c32_t tb = c32_mul_q30_q15(
                    factor_q30,
                    c32_make(b_re[k][lane], b_im[k][lane]));

                b_re[i][lane] -= tb.r;
                b_im[i][lane] -= tb.i;
                a_re[ik][lane] = 0;
                a_im[ik][lane] = 0;
            }
        }
    }

    for (int i = MIMO_N4 - 1; i >= 0; --i) {
        for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
            mimo_c32_t sum = c32_make(b_re[i][lane], b_im[i][lane]);

            for (int j = i + 1; j < MIMO_N4; ++j) {
                const int ij = i * MIMO_N4 + j;
                sum = c32_sub(
                    sum,
                    c32_mul_q15(c32_make(a_re[ij][lane], a_im[ij][lane]),
                                c32_make(b_re[j][lane], b_im[j][lane])));
            }

            const int ii = i * MIMO_N4 + i;
            const mimo_c32_t sol = c32_div_real_to_q15_exact(
                sum, safe_pivot_i32(a_re[ii][lane]));

            b_re[i][lane] = sol.r;
            b_im[i][lane] = sol.i;
        }
    }

    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        mimo_c16_t *x = &x_block[lane * MIMO_N4];

        for (int i = 0; i < MIMO_N4; ++i) {
            x[i] = mimo_make_c16(mimo_solve_store_i16(b_re[i][lane]),
                                 mimo_solve_store_i16(b_im[i][lane]));
        }
    }
}

#if MIMO_USE_SOLVE4_HERMITIAN
#if MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE && \
    MIMO_USE_SOLVE4_HERMITIAN_Q30_PAIR_CORR
#define MIMO_SOA_HERM_FACTOR_Q30(VALUE, PIVOT, SCALE) \
    c32_factor_q30_pivotscale_pair((VALUE), &(SCALE))
#if MIMO_USE_SOLVE4_HERMITIAN_HAND_SCHED || \
    MIMO_USE_SOLVE4_HERMITIAN_Q15_CALL
#define MIMO_SOA_HERM_DIV_Q15(SUM, PIVOT, SCALE) \
    c32_div_real_to_q15_pivotscale_call((SUM), &(SCALE))
#else
#define MIMO_SOA_HERM_DIV_Q15(SUM, PIVOT, SCALE) \
    c32_div_real_to_q15_pivotscale((SUM), &(SCALE))
#if MIMO_SOLVE4_HERMITIAN_Q15_JUMP_ONLY_MASK
#define MIMO_SOA_HERM_DIV_Q15_STAGE(SUM, PIVOT, SCALE, ID) \
    c32_make(MIMO_HERM_PIVOT_DIV_Q15_STAGE((SUM).r, (SCALE), (ID)), \
             MIMO_HERM_PIVOT_DIV_Q15_STAGE((SUM).i, (SCALE), (ID)))
#endif
#endif
#elif MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE && \
    (MIMO_USE_SOLVE4_HERMITIAN_HAND_SCHED || \
     MIMO_USE_SOLVE4_HERMITIAN_FACTOR_CALL || \
     MIMO_USE_SOLVE4_HERMITIAN_Q15_CALL)
#if MIMO_USE_SOLVE4_HERMITIAN_HAND_SCHED || \
    MIMO_USE_SOLVE4_HERMITIAN_FACTOR_CALL
#define MIMO_SOA_HERM_FACTOR_Q30(VALUE, PIVOT, SCALE) \
    c32_factor_q30_pivotscale_call((VALUE), &(SCALE))
#else
#define MIMO_SOA_HERM_FACTOR_Q30(VALUE, PIVOT, SCALE) \
    c32_make(herm_pivot_div_to_q30_scaled((VALUE).r, &(SCALE)), \
             herm_pivot_div_to_q30_scaled(-(VALUE).i, &(SCALE)))
#if MIMO_SOLVE4_HERMITIAN_Q30_JUMP_ONLY_MASK
#define MIMO_SOA_HERM_FACTOR_Q30_STAGE(VALUE, PIVOT, SCALE, ID) \
    c32_make(MIMO_HERM_PIVOT_DIV_Q30_STAGE((VALUE).r, (SCALE), (ID)), \
             MIMO_HERM_PIVOT_DIV_Q30_STAGE(-(VALUE).i, (SCALE), (ID)))
#endif
#endif
#if MIMO_USE_SOLVE4_HERMITIAN_HAND_SCHED || \
    MIMO_USE_SOLVE4_HERMITIAN_Q15_CALL
#define MIMO_SOA_HERM_DIV_Q15(SUM, PIVOT, SCALE) \
    c32_div_real_to_q15_pivotscale_call((SUM), &(SCALE))
#else
#define MIMO_SOA_HERM_DIV_Q15(SUM, PIVOT, SCALE) \
    c32_div_real_to_q15_pivotscale((SUM), &(SCALE))
#if MIMO_SOLVE4_HERMITIAN_Q15_JUMP_ONLY_MASK
#define MIMO_SOA_HERM_DIV_Q15_STAGE(SUM, PIVOT, SCALE, ID) \
    c32_make(MIMO_HERM_PIVOT_DIV_Q15_STAGE((SUM).r, (SCALE), (ID)), \
             MIMO_HERM_PIVOT_DIV_Q15_STAGE((SUM).i, (SCALE), (ID)))
#endif
#endif
#elif MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE
#define MIMO_SOA_HERM_FACTOR_Q30(VALUE, PIVOT, SCALE) \
    c32_make(herm_pivot_div_to_q30_scaled((VALUE).r, &(SCALE)), \
             herm_pivot_div_to_q30_scaled(-(VALUE).i, &(SCALE)))
#if MIMO_SOLVE4_HERMITIAN_Q30_JUMP_ONLY_MASK
#define MIMO_SOA_HERM_FACTOR_Q30_STAGE(VALUE, PIVOT, SCALE, ID) \
    c32_make(MIMO_HERM_PIVOT_DIV_Q30_STAGE((VALUE).r, (SCALE), (ID)), \
             MIMO_HERM_PIVOT_DIV_Q30_STAGE(-(VALUE).i, (SCALE), (ID)))
#endif
#define MIMO_SOA_HERM_DIV_Q15(SUM, PIVOT, SCALE) \
    c32_div_real_to_q15_pivotscale((SUM), &(SCALE))
#if MIMO_SOLVE4_HERMITIAN_Q15_JUMP_ONLY_MASK
#define MIMO_SOA_HERM_DIV_Q15_STAGE(SUM, PIVOT, SCALE, ID) \
    c32_make(MIMO_HERM_PIVOT_DIV_Q15_STAGE((SUM).r, (SCALE), (ID)), \
             MIMO_HERM_PIVOT_DIV_Q15_STAGE((SUM).i, (SCALE), (ID)))
#endif
#else
#define MIMO_SOA_HERM_FACTOR_Q30(VALUE, PIVOT, SCALE) \
    c32_make(div_to_q30_i32((VALUE).r, (PIVOT)), \
             div_to_q30_i32(-(VALUE).i, (PIVOT)))
#if MIMO_USE_SOLVE4_HERMITIAN_TRUST_PIVOT_NONZERO
#define MIMO_SOA_HERM_DIV_Q15(SUM, PIVOT, SCALE) \
    c32_div_real_to_q15_exact_nonzero((SUM), (PIVOT))
#else
#define MIMO_SOA_HERM_DIV_Q15(SUM, PIVOT, SCALE) \
    c32_div_real_to_q15_exact((SUM), (PIVOT))
#endif
#endif
#if MIMO_USE_SOLVE4_HERMITIAN_FACTOR_Q15
#undef MIMO_SOA_HERM_FACTOR_Q30
#undef MIMO_SOA_HERM_FACTOR_Q30_STAGE
#define MIMO_SOA_HERM_FACTOR_Q30(VALUE, PIVOT, SCALE) \
    c32_factor_q15_exact((VALUE), (PIVOT))
#define MIMO_SOA_HERM_FACTOR_Q30_STAGE(VALUE, PIVOT, SCALE, ID) \
    c32_factor_q15_exact((VALUE), (PIVOT))
#define MIMO_SOA_HERM_FACTOR_MUL(F, VALUE) c32_mul_q15((F), (VALUE))
#define MIMO_SOA_HERM_FACTOR_MUL_REAL(F, VALUE) c32_mul_q15_real((F), (VALUE))
#else
#define MIMO_SOA_HERM_FACTOR_MUL(F, VALUE) c32_mul_q30_q15((F), (VALUE))
#define MIMO_SOA_HERM_FACTOR_MUL_REAL(F, VALUE) \
    c32_mul_q30_q15_real((F), (VALUE))
#endif
#ifndef MIMO_SOA_HERM_FACTOR_Q30_STAGE
#define MIMO_SOA_HERM_FACTOR_Q30_STAGE(VALUE, PIVOT, SCALE, ID) \
    MIMO_SOA_HERM_FACTOR_Q30((VALUE), (PIVOT), (SCALE))
#endif
#ifndef MIMO_SOA_HERM_DIV_Q15_STAGE
#define MIMO_SOA_HERM_DIV_Q15_STAGE(SUM, PIVOT, SCALE, ID) \
    MIMO_SOA_HERM_DIV_Q15((SUM), (PIVOT), (SCALE))
#endif

#if MIMO_USE_SOLVE4_HERMITIAN_SOA_STAGE_OUTER
static MIMO_ALWAYS_INLINE int mimo_herm4_upper_slot(int row, int col)
{
#if MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED
    return (row * (2 * MIMO_N4 - row + 1)) / 2 + (col - row);
#else
    return row * MIMO_N4 + col;
#endif
}

static void mimo_solve_normal4_site4_soa_hermitian_stage_outer_zf_q15(
    mimo_site4_soa4_t *restrict work,
    mimo_c16_t *restrict x_block)
{
    int32_t (*restrict a_re)[MIMO_SITE4_LANES] = work->a_re;
    int32_t (*restrict a_im)[MIMO_SITE4_LANES] = work->a_im;
    int32_t (*restrict b_re)[MIMO_SITE4_LANES] = work->b_re;
    int32_t (*restrict b_im)[MIMO_SITE4_LANES] = work->b_im;
    int q30_stage = 0;

    for (int k = 0; k < MIMO_N4 - 1; ++k) {
        const int kk = mimo_herm4_upper_slot(k, k);

        for (int i = k + 1; i < MIMO_N4; ++i) {
            const int ki = mimo_herm4_upper_slot(k, i);
            const int stage = q30_stage++;

            for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
                const int32_t pivot = MIMO_HERM_SAFE_PIVOT_I32(a_re[kk][lane]);
#if MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE
                const herm_pivot_scale_t scale = herm_pivot_scale_init(pivot);
#else
                const int scale = 0;
#endif
                (void)stage;
                (void)scale;
                const mimo_c32_t src =
                    c32_make(a_re[ki][lane], a_im[ki][lane]);
                const mimo_c32_t f =
                    MIMO_SOA_HERM_FACTOR_Q30_STAGE(src, pivot, scale, stage);

                for (int j = i; j < MIMO_N4; ++j) {
                    const int ij = mimo_herm4_upper_slot(i, j);
                    const int kj = mimo_herm4_upper_slot(k, j);
                    const mimo_c32_t v =
                        c32_make(a_re[kj][lane], a_im[kj][lane]);

#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
                    if (i == j) {
                        a_re[ij][lane] -= MIMO_SOA_HERM_FACTOR_MUL_REAL(f, v);
                    } else
#endif
                    {
                        const mimo_c32_t t = MIMO_SOA_HERM_FACTOR_MUL(f, v);
                        a_re[ij][lane] -= t.r;
                        a_im[ij][lane] -= t.i;
                    }
                }

                const mimo_c32_t rhs =
                    c32_make(b_re[k][lane], b_im[k][lane]);
                const mimo_c32_t tb = MIMO_SOA_HERM_FACTOR_MUL(f, rhs);

                b_re[i][lane] -= tb.r;
                b_im[i][lane] -= tb.i;
            }
        }
    }

    int q15_stage = 0;

    for (int i = MIMO_N4 - 1; i >= 0; --i) {
        const int ii = mimo_herm4_upper_slot(i, i);
        const int stage = q15_stage++;

        for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
            mimo_c32_t sum = c32_make(b_re[i][lane], b_im[i][lane]);

            for (int j = i + 1; j < MIMO_N4; ++j) {
                const int ij = mimo_herm4_upper_slot(i, j);
                const mimo_c32_t aij =
                    c32_make(a_re[ij][lane], a_im[ij][lane]);
                const mimo_c32_t xj =
                    c32_make(b_re[j][lane], b_im[j][lane]);

                sum = c32_sub(sum, c32_mul_q15(aij, xj));
            }

            const int32_t pivot = MIMO_HERM_SAFE_PIVOT_I32(a_re[ii][lane]);
#if MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE
            const herm_pivot_scale_t scale = herm_pivot_scale_init(pivot);
#else
            const int scale = 0;
#endif
            (void)stage;
            (void)scale;
            const mimo_c32_t sol =
                MIMO_SOA_HERM_DIV_Q15_STAGE(sum, pivot, scale, stage);

            b_re[i][lane] = sol.r;
            b_im[i][lane] = sol.i;
        }
    }

    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        mimo_c16_t *x = &x_block[lane * MIMO_N4];

        for (int i = 0; i < MIMO_N4; ++i) {
            x[i] = mimo_make_c16(mimo_solve_store_i16(b_re[i][lane]),
                                 mimo_solve_store_i16(b_im[i][lane]));
        }
    }
}
#endif

#if MIMO_USE_SOLVE4_HERMITIAN_GS
#if !MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE
#error "MIMO_USE_SOLVE4_HERMITIAN_GS requires pivot-scale division to keep the candidate no-harddiv"
#endif
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_APPROX
#error "MIMO_USE_SOLVE4_HERMITIAN_GS and MIMO_USE_SOLVE4_HERMITIAN_DIAG_APPROX are mutually exclusive"
#endif
#if MIMO_SOLVE4_HERMITIAN_GS_ITERS < 1
#error "MIMO_SOLVE4_HERMITIAN_GS_ITERS must be at least 1"
#endif
static void mimo_solve_normal4_site4_soa_hermitian_gs_q15(
    mimo_site4_soa4_t *restrict work,
    mimo_c16_t *restrict x_block)
{
    int32_t (*restrict a_re)[MIMO_SITE4_LANES] = work->a_re;
    int32_t (*restrict a_im)[MIMO_SITE4_LANES] = work->a_im;
    int32_t (*restrict b_re)[MIMO_SITE4_LANES] = work->b_re;
    int32_t (*restrict b_im)[MIMO_SITE4_LANES] = work->b_im;

    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
#if MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED
        const mimo_c32_t m00 = c32_make(a_re[0][lane], 0);
        const mimo_c32_t m01 = c32_make(a_re[1][lane], a_im[1][lane]);
        const mimo_c32_t m02 = c32_make(a_re[2][lane], a_im[2][lane]);
        const mimo_c32_t m03 = c32_make(a_re[3][lane], a_im[3][lane]);
        const mimo_c32_t m11 = c32_make(a_re[4][lane], 0);
        const mimo_c32_t m12 = c32_make(a_re[5][lane], a_im[5][lane]);
        const mimo_c32_t m13 = c32_make(a_re[6][lane], a_im[6][lane]);
        const mimo_c32_t m22 = c32_make(a_re[7][lane], 0);
        const mimo_c32_t m23 = c32_make(a_re[8][lane], a_im[8][lane]);
        const mimo_c32_t m33 = c32_make(a_re[9][lane], 0);
#else
        const mimo_c32_t m00 = c32_make(a_re[0][lane], 0);
        const mimo_c32_t m01 = c32_make(a_re[1][lane], a_im[1][lane]);
        const mimo_c32_t m02 = c32_make(a_re[2][lane], a_im[2][lane]);
        const mimo_c32_t m03 = c32_make(a_re[3][lane], a_im[3][lane]);
        const mimo_c32_t m11 = c32_make(a_re[5][lane], 0);
        const mimo_c32_t m12 = c32_make(a_re[6][lane], a_im[6][lane]);
        const mimo_c32_t m13 = c32_make(a_re[7][lane], a_im[7][lane]);
        const mimo_c32_t m22 = c32_make(a_re[10][lane], 0);
        const mimo_c32_t m23 = c32_make(a_re[11][lane], a_im[11][lane]);
        const mimo_c32_t m33 = c32_make(a_re[15][lane], 0);
#endif
        const mimo_c32_t r0 = c32_make(b_re[0][lane], b_im[0][lane]);
        const mimo_c32_t r1 = c32_make(b_re[1][lane], b_im[1][lane]);
        const mimo_c32_t r2 = c32_make(b_re[2][lane], b_im[2][lane]);
        const mimo_c32_t r3 = c32_make(b_re[3][lane], b_im[3][lane]);

        const int32_t p0 = MIMO_HERM_SAFE_PIVOT_I32(m00.r);
        const int32_t p1 = MIMO_HERM_SAFE_PIVOT_I32(m11.r);
        const int32_t p2 = MIMO_HERM_SAFE_PIVOT_I32(m22.r);
        const int32_t p3 = MIMO_HERM_SAFE_PIVOT_I32(m33.r);
        const herm_pivot_scale_t s0 = herm_pivot_scale_init(p0);
        const herm_pivot_scale_t s1 = herm_pivot_scale_init(p1);
        const herm_pivot_scale_t s2 = herm_pivot_scale_init(p2);
        const herm_pivot_scale_t s3 = herm_pivot_scale_init(p3);
        mimo_c32_t x0 = c32_make(0, 0);
        mimo_c32_t x1 = c32_make(0, 0);
        mimo_c32_t x2 = c32_make(0, 0);
        mimo_c32_t x3 = c32_make(0, 0);

        for (int iter = 0; iter < MIMO_SOLVE4_HERMITIAN_GS_ITERS; ++iter) {
            mimo_c32_t sum = c32_sub(r0, c32_mul_q15(m01, x1));
            sum = c32_sub(sum, c32_mul_q15(m02, x2));
            sum = c32_sub(sum, c32_mul_q15(m03, x3));
            x0 = MIMO_SOA_HERM_DIV_Q15(sum, p0, s0);

            sum = c32_sub(r1, c32_mul_q15(c32_conj(m01), x0));
            sum = c32_sub(sum, c32_mul_q15(m12, x2));
            sum = c32_sub(sum, c32_mul_q15(m13, x3));
            x1 = MIMO_SOA_HERM_DIV_Q15(sum, p1, s1);

            sum = c32_sub(r2, c32_mul_q15(c32_conj(m02), x0));
            sum = c32_sub(sum, c32_mul_q15(c32_conj(m12), x1));
            sum = c32_sub(sum, c32_mul_q15(m23, x3));
            x2 = MIMO_SOA_HERM_DIV_Q15(sum, p2, s2);

            sum = c32_sub(r3, c32_mul_q15(c32_conj(m03), x0));
            sum = c32_sub(sum, c32_mul_q15(c32_conj(m13), x1));
            sum = c32_sub(sum, c32_mul_q15(c32_conj(m23), x2));
            x3 = MIMO_SOA_HERM_DIV_Q15(sum, p3, s3);
        }

        mimo_c16_t *x = &x_block[lane * MIMO_N4];

        x[0] = mimo_make_c16(mimo_solve_store_i16(x0.r),
                             mimo_solve_store_i16(x0.i));
        x[1] = mimo_make_c16(mimo_solve_store_i16(x1.r),
                             mimo_solve_store_i16(x1.i));
        x[2] = mimo_make_c16(mimo_solve_store_i16(x2.r),
                             mimo_solve_store_i16(x2.i));
        x[3] = mimo_make_c16(mimo_solve_store_i16(x3.r),
                             mimo_solve_store_i16(x3.i));
    }
}
#endif

#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_APPROX
#if !MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE
#error "MIMO_USE_SOLVE4_HERMITIAN_DIAG_APPROX requires pivot-scale division to keep the candidate no-harddiv"
#endif
static void mimo_solve_normal4_site4_soa_hermitian_diag_approx_zf_q15(
    mimo_site4_soa4_t *restrict work,
    mimo_c16_t *restrict x_block)
{
    int32_t (*restrict a_re)[MIMO_SITE4_LANES] = work->a_re;
    int32_t (*restrict b_re)[MIMO_SITE4_LANES] = work->b_re;
    int32_t (*restrict b_im)[MIMO_SITE4_LANES] = work->b_im;

    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        const int32_t p0 = MIMO_HERM_SAFE_PIVOT_I32(a_re[0][lane]);
        const int32_t p1 = MIMO_HERM_SAFE_PIVOT_I32(a_re[5][lane]);
        const int32_t p2 = MIMO_HERM_SAFE_PIVOT_I32(a_re[10][lane]);
        const int32_t p3 = MIMO_HERM_SAFE_PIVOT_I32(a_re[15][lane]);
        const herm_pivot_scale_t s0 = herm_pivot_scale_init(p0);
        const herm_pivot_scale_t s1 = herm_pivot_scale_init(p1);
        const herm_pivot_scale_t s2 = herm_pivot_scale_init(p2);
        const herm_pivot_scale_t s3 = herm_pivot_scale_init(p3);
        const mimo_c32_t x0 = MIMO_SOA_HERM_DIV_Q15(
            c32_make(b_re[0][lane], b_im[0][lane]), p0, s0);
        const mimo_c32_t x1 = MIMO_SOA_HERM_DIV_Q15(
            c32_make(b_re[1][lane], b_im[1][lane]), p1, s1);
        const mimo_c32_t x2 = MIMO_SOA_HERM_DIV_Q15(
            c32_make(b_re[2][lane], b_im[2][lane]), p2, s2);
        const mimo_c32_t x3 = MIMO_SOA_HERM_DIV_Q15(
            c32_make(b_re[3][lane], b_im[3][lane]), p3, s3);
        mimo_c16_t *x = &x_block[lane * MIMO_N4];

        x[0] = mimo_make_c16(mimo_solve_store_i16(x0.r),
                             mimo_solve_store_i16(x0.i));
        x[1] = mimo_make_c16(mimo_solve_store_i16(x1.r),
                             mimo_solve_store_i16(x1.i));
        x[2] = mimo_make_c16(mimo_solve_store_i16(x2.r),
                             mimo_solve_store_i16(x2.i));
        x[3] = mimo_make_c16(mimo_solve_store_i16(x3.r),
                             mimo_solve_store_i16(x3.i));
    }
}
#endif

void mimo_solve_normal4_site4_soa_hermitian_zf_q15(
    mimo_site4_soa4_t *restrict work,
    mimo_c16_t *restrict x_block)
{
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_APPROX
    mimo_solve_normal4_site4_soa_hermitian_diag_approx_zf_q15(work, x_block);
    return;
#endif
#if MIMO_USE_SOLVE4_HERMITIAN_GS
    mimo_solve_normal4_site4_soa_hermitian_gs_q15(work, x_block);
    return;
#endif
#if MIMO_USE_SOLVE4_HERMITIAN_SOA_STAGE_OUTER
    mimo_solve_normal4_site4_soa_hermitian_stage_outer_zf_q15(work, x_block);
    return;
#endif
    int32_t (*restrict a_re)[MIMO_SITE4_LANES] = work->a_re;
    int32_t (*restrict a_im)[MIMO_SITE4_LANES] = work->a_im;
    int32_t (*restrict b_re)[MIMO_SITE4_LANES] = work->b_re;
    int32_t (*restrict b_im)[MIMO_SITE4_LANES] = work->b_im;

    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
#if MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED
        mimo_c32_t m00 = c32_make(a_re[0][lane], 0);
        mimo_c32_t m01 = c32_make(a_re[1][lane], a_im[1][lane]);
        mimo_c32_t m02 = c32_make(a_re[2][lane], a_im[2][lane]);
        mimo_c32_t m03 = c32_make(a_re[3][lane], a_im[3][lane]);
        mimo_c32_t m11 = c32_make(a_re[4][lane], 0);
        mimo_c32_t m12 = c32_make(a_re[5][lane], a_im[5][lane]);
        mimo_c32_t m13 = c32_make(a_re[6][lane], a_im[6][lane]);
        mimo_c32_t m22 = c32_make(a_re[7][lane], 0);
        mimo_c32_t m23 = c32_make(a_re[8][lane], a_im[8][lane]);
        mimo_c32_t m33 = c32_make(a_re[9][lane], 0);
#else
        mimo_c32_t m00 = c32_make(a_re[0][lane], 0);
        mimo_c32_t m01 = c32_make(a_re[1][lane], a_im[1][lane]);
        mimo_c32_t m02 = c32_make(a_re[2][lane], a_im[2][lane]);
        mimo_c32_t m03 = c32_make(a_re[3][lane], a_im[3][lane]);
        mimo_c32_t m11 = c32_make(a_re[5][lane], 0);
        mimo_c32_t m12 = c32_make(a_re[6][lane], a_im[6][lane]);
        mimo_c32_t m13 = c32_make(a_re[7][lane], a_im[7][lane]);
        mimo_c32_t m22 = c32_make(a_re[10][lane], 0);
        mimo_c32_t m23 = c32_make(a_re[11][lane], a_im[11][lane]);
        mimo_c32_t m33 = c32_make(a_re[15][lane], 0);
#endif
        mimo_c32_t r0 = c32_make(b_re[0][lane], b_im[0][lane]);
        mimo_c32_t r1 = c32_make(b_re[1][lane], b_im[1][lane]);
        mimo_c32_t r2 = c32_make(b_re[2][lane], b_im[2][lane]);
        mimo_c32_t r3 = c32_make(b_re[3][lane], b_im[3][lane]);

        const int32_t p0 = MIMO_HERM_SAFE_PIVOT_I32(m00.r);
#if MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE
        const herm_pivot_scale_t s0 = herm_pivot_scale_init(p0);
#endif
#if MIMO_USE_SOLVE4_HERMITIAN_FACTOR_PRECOMPUTE
        PIVOT_Q30_STAGE_BEGIN(0);
        const mimo_c32_t f01 =
            MIMO_SOA_HERM_FACTOR_Q30_STAGE(m01, p0, s0, 0);
        PIVOT_Q30_STAGE_END();
        PIVOT_Q30_STAGE_BEGIN(1);
        const mimo_c32_t f02 =
            MIMO_SOA_HERM_FACTOR_Q30_STAGE(m02, p0, s0, 1);
        PIVOT_Q30_STAGE_END();
        PIVOT_Q30_STAGE_BEGIN(2);
        const mimo_c32_t f03 =
            MIMO_SOA_HERM_FACTOR_Q30_STAGE(m03, p0, s0, 2);
        PIVOT_Q30_STAGE_END();
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
        m11.r -= MIMO_SOA_HERM_FACTOR_MUL_REAL(f01, m01);
#else
        m11 = c32_sub(m11, MIMO_SOA_HERM_FACTOR_MUL(f01, m01));
#endif
        m12 = c32_sub(m12, MIMO_SOA_HERM_FACTOR_MUL(f01, m02));
        m13 = c32_sub(m13, MIMO_SOA_HERM_FACTOR_MUL(f01, m03));
        r1 = c32_sub(r1, MIMO_SOA_HERM_FACTOR_MUL(f01, r0));

#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
        m22.r -= MIMO_SOA_HERM_FACTOR_MUL_REAL(f02, m02);
#else
        m22 = c32_sub(m22, MIMO_SOA_HERM_FACTOR_MUL(f02, m02));
#endif
        m23 = c32_sub(m23, MIMO_SOA_HERM_FACTOR_MUL(f02, m03));
        r2 = c32_sub(r2, MIMO_SOA_HERM_FACTOR_MUL(f02, r0));

#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
        m33.r -= MIMO_SOA_HERM_FACTOR_MUL_REAL(f03, m03);
#else
        m33 = c32_sub(m33, MIMO_SOA_HERM_FACTOR_MUL(f03, m03));
#endif
        r3 = c32_sub(r3, MIMO_SOA_HERM_FACTOR_MUL(f03, r0));
#else
        PIVOT_Q30_STAGE_BEGIN(0);
        mimo_c32_t f = MIMO_SOA_HERM_FACTOR_Q30_STAGE(m01, p0, s0, 0);
        PIVOT_Q30_STAGE_END();
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
        m11.r -= MIMO_SOA_HERM_FACTOR_MUL_REAL(f, m01);
#else
        m11 = c32_sub(m11, MIMO_SOA_HERM_FACTOR_MUL(f, m01));
#endif
        m12 = c32_sub(m12, MIMO_SOA_HERM_FACTOR_MUL(f, m02));
        m13 = c32_sub(m13, MIMO_SOA_HERM_FACTOR_MUL(f, m03));
        r1 = c32_sub(r1, MIMO_SOA_HERM_FACTOR_MUL(f, r0));

        PIVOT_Q30_STAGE_BEGIN(1);
        f = MIMO_SOA_HERM_FACTOR_Q30_STAGE(m02, p0, s0, 1);
        PIVOT_Q30_STAGE_END();
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
        m22.r -= MIMO_SOA_HERM_FACTOR_MUL_REAL(f, m02);
#else
        m22 = c32_sub(m22, MIMO_SOA_HERM_FACTOR_MUL(f, m02));
#endif
        m23 = c32_sub(m23, MIMO_SOA_HERM_FACTOR_MUL(f, m03));
        r2 = c32_sub(r2, MIMO_SOA_HERM_FACTOR_MUL(f, r0));

        PIVOT_Q30_STAGE_BEGIN(2);
        f = MIMO_SOA_HERM_FACTOR_Q30_STAGE(m03, p0, s0, 2);
        PIVOT_Q30_STAGE_END();
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
        m33.r -= MIMO_SOA_HERM_FACTOR_MUL_REAL(f, m03);
#else
        m33 = c32_sub(m33, MIMO_SOA_HERM_FACTOR_MUL(f, m03));
#endif
        r3 = c32_sub(r3, MIMO_SOA_HERM_FACTOR_MUL(f, r0));
#endif

        const int32_t p1 = MIMO_HERM_SAFE_PIVOT_I32(m11.r);
#if MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE
        const herm_pivot_scale_t s1 = herm_pivot_scale_init(p1);
#endif
#if MIMO_USE_SOLVE4_HERMITIAN_FACTOR_PRECOMPUTE
        PIVOT_Q30_STAGE_BEGIN(3);
        const mimo_c32_t f12 =
            MIMO_SOA_HERM_FACTOR_Q30_STAGE(m12, p1, s1, 3);
        PIVOT_Q30_STAGE_END();
        PIVOT_Q30_STAGE_BEGIN(4);
        const mimo_c32_t f13 =
            MIMO_SOA_HERM_FACTOR_Q30_STAGE(m13, p1, s1, 4);
        PIVOT_Q30_STAGE_END();
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
        m22.r -= MIMO_SOA_HERM_FACTOR_MUL_REAL(f12, m12);
#else
        m22 = c32_sub(m22, MIMO_SOA_HERM_FACTOR_MUL(f12, m12));
#endif
        m23 = c32_sub(m23, MIMO_SOA_HERM_FACTOR_MUL(f12, m13));
        r2 = c32_sub(r2, MIMO_SOA_HERM_FACTOR_MUL(f12, r1));

#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
        m33.r -= MIMO_SOA_HERM_FACTOR_MUL_REAL(f13, m13);
#else
        m33 = c32_sub(m33, MIMO_SOA_HERM_FACTOR_MUL(f13, m13));
#endif
        r3 = c32_sub(r3, MIMO_SOA_HERM_FACTOR_MUL(f13, r1));
#else
        PIVOT_Q30_STAGE_BEGIN(3);
        f = MIMO_SOA_HERM_FACTOR_Q30_STAGE(m12, p1, s1, 3);
        PIVOT_Q30_STAGE_END();
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
        m22.r -= MIMO_SOA_HERM_FACTOR_MUL_REAL(f, m12);
#else
        m22 = c32_sub(m22, MIMO_SOA_HERM_FACTOR_MUL(f, m12));
#endif
        m23 = c32_sub(m23, MIMO_SOA_HERM_FACTOR_MUL(f, m13));
        r2 = c32_sub(r2, MIMO_SOA_HERM_FACTOR_MUL(f, r1));

        PIVOT_Q30_STAGE_BEGIN(4);
        f = MIMO_SOA_HERM_FACTOR_Q30_STAGE(m13, p1, s1, 4);
        PIVOT_Q30_STAGE_END();
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
        m33.r -= MIMO_SOA_HERM_FACTOR_MUL_REAL(f, m13);
#else
        m33 = c32_sub(m33, MIMO_SOA_HERM_FACTOR_MUL(f, m13));
#endif
        r3 = c32_sub(r3, MIMO_SOA_HERM_FACTOR_MUL(f, r1));
#endif

        const int32_t p2 = MIMO_HERM_SAFE_PIVOT_I32(m22.r);
#if MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE
        const herm_pivot_scale_t s2 = herm_pivot_scale_init(p2);
#endif
        PIVOT_Q30_STAGE_BEGIN(5);
#if MIMO_USE_SOLVE4_HERMITIAN_FACTOR_PRECOMPUTE
        const mimo_c32_t f23 =
            MIMO_SOA_HERM_FACTOR_Q30_STAGE(m23, p2, s2, 5);
#else
        f = MIMO_SOA_HERM_FACTOR_Q30_STAGE(m23, p2, s2, 5);
#endif
        PIVOT_Q30_STAGE_END();
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
        m33.r -= MIMO_SOA_HERM_FACTOR_MUL_REAL(
#if MIMO_USE_SOLVE4_HERMITIAN_FACTOR_PRECOMPUTE
            f23,
#else
            f,
#endif
            m23);
#else
        m33 = c32_sub(m33, MIMO_SOA_HERM_FACTOR_MUL(
#if MIMO_USE_SOLVE4_HERMITIAN_FACTOR_PRECOMPUTE
                                   f23,
#else
                                   f,
#endif
                                   m23));
#endif
        r3 = c32_sub(r3, MIMO_SOA_HERM_FACTOR_MUL(
#if MIMO_USE_SOLVE4_HERMITIAN_FACTOR_PRECOMPUTE
                               f23,
#else
                               f,
#endif
                               r2));

        const int32_t p3 = MIMO_HERM_SAFE_PIVOT_I32(m33.r);
#if MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE
        const herm_pivot_scale_t s3 = herm_pivot_scale_init(p3);
#endif
        PIVOT_Q15_STAGE_BEGIN(0);
        const mimo_c32_t x3 = MIMO_SOA_HERM_DIV_Q15_STAGE(r3, p3, s3, 0);
        PIVOT_Q15_STAGE_END();
        mimo_c32_t sum = c32_sub(r2, c32_mul_q15(m23, x3));
        PIVOT_Q15_STAGE_BEGIN(1);
        const mimo_c32_t x2 = MIMO_SOA_HERM_DIV_Q15_STAGE(sum, p2, s2, 1);
        PIVOT_Q15_STAGE_END();

        sum = c32_sub(r1, c32_mul_q15(m12, x2));
        sum = c32_sub(sum, c32_mul_q15(m13, x3));
        PIVOT_Q15_STAGE_BEGIN(2);
        const mimo_c32_t x1 = MIMO_SOA_HERM_DIV_Q15_STAGE(sum, p1, s1, 2);
        PIVOT_Q15_STAGE_END();

        sum = c32_sub(r0, c32_mul_q15(m01, x1));
        sum = c32_sub(sum, c32_mul_q15(m02, x2));
        sum = c32_sub(sum, c32_mul_q15(m03, x3));
        PIVOT_Q15_STAGE_BEGIN(3);
        const mimo_c32_t x0 = MIMO_SOA_HERM_DIV_Q15_STAGE(sum, p0, s0, 3);
        PIVOT_Q15_STAGE_END();

        mimo_c16_t *x = &x_block[lane * MIMO_N4];

        x[0] = mimo_make_c16(mimo_solve_store_i16(x0.r),
                             mimo_solve_store_i16(x0.i));
        x[1] = mimo_make_c16(mimo_solve_store_i16(x1.r),
                             mimo_solve_store_i16(x1.i));
        x[2] = mimo_make_c16(mimo_solve_store_i16(x2.r),
                             mimo_solve_store_i16(x2.i));
        x[3] = mimo_make_c16(mimo_solve_store_i16(x3.r),
                             mimo_solve_store_i16(x3.i));
    }
}

#undef MIMO_SOA_HERM_FACTOR_Q30
#undef MIMO_SOA_HERM_FACTOR_Q30_STAGE
#undef MIMO_SOA_HERM_FACTOR_MUL
#undef MIMO_SOA_HERM_FACTOR_MUL_REAL
#undef MIMO_SOA_HERM_DIV_Q15
#undef MIMO_SOA_HERM_DIV_Q15_STAGE
#undef MIMO_HERM_SAFE_PIVOT_I32
#endif
#endif

void mimo_solve_normal_ref_q15(int n,
                               const mimo_c32_t *a,
                               const mimo_c32_t *b,
                               int mmse,
                               mimo_c16_t *x)
{
    mimo_c64_t mat[MIMO_MAX_N * MIMO_MAX_N];
    mimo_c64_t rhs[MIMO_MAX_N];
    mimo_c64_t sol[MIMO_MAX_N];

    for (int i = 0; i < n; ++i) {
        rhs[i] = c64_make(b[i].r, b[i].i);
        sol[i] = c64_make(0, 0);
        for (int j = 0; j < n; ++j) {
            mat[i * n + j] = c64_make(a[i * n + j].r, a[i * n + j].i);
        }
        if (mmse) {
            mat[i * n + i].r += MIMO_MMSE_LAMBDA_Q15;
            mat[i * n + i].i = 0;
        }
    }

    for (int k = 0; k < n; ++k) {
        const int64_t pivot = safe_pivot_i64(mat[k * n + k].r);
        mat[k * n + k].i = 0;

        for (int i = k + 1; i < n; ++i) {
            const mimo_c64_t factor_q30 =
                c64_make((mat[i * n + k].r << 30) / pivot,
                         (mat[i * n + k].i << 30) / pivot);

            for (int j = k; j < n; ++j) {
                const mimo_c64_t t =
                    c64_mul_q30_q15(factor_q30, mat[k * n + j]);
                mat[i * n + j].r -= t.r;
                mat[i * n + j].i -= t.i;
            }
            const mimo_c64_t tb = c64_mul_q30_q15(factor_q30, rhs[k]);
            rhs[i].r -= tb.r;
            rhs[i].i -= tb.i;
            mat[i * n + k] = c64_make(0, 0);
        }
    }

    for (int i = n - 1; i >= 0; --i) {
        mimo_c64_t sum = rhs[i];

        for (int j = i + 1; j < n; ++j) {
            sum = c64_sub(sum, c64_mul_q15(mat[i * n + j], sol[j]));
        }
        sol[i] = c64_div_real_to_q15(sum, safe_pivot_i64(mat[i * n + i].r));
    }

    for (int i = 0; i < n; ++i) {
        x[i] = mimo_make_c16(mimo_sat_i16((int32_t)sol[i].r),
                             mimo_sat_i16((int32_t)sol[i].i));
    }
}

#define DEFINE_MIMO_SOLVE_NORMAL_EXACT(NAME, N) \
    static void NAME(const mimo_c32_t *a, \
                     const mimo_c32_t *b, \
                     int mmse, \
                     mimo_c16_t *x) \
    { \
        enum { n = (N) }; \
        mimo_c64_t mat[(N) * (N)]; \
        mimo_c64_t rhs[(N)]; \
        mimo_c64_t sol[(N)]; \
 \
        for (int i = 0; i < n; ++i) { \
            rhs[i] = c64_make(b[i].r, b[i].i); \
            sol[i] = c64_make(0, 0); \
            RECORD_SOLVE_VALUE_C64(solve_rhs_init_stats, rhs[i]); \
            for (int j = 0; j < n; ++j) { \
                mat[i * n + j] = c64_make(a[i * n + j].r, a[i * n + j].i); \
                RECORD_SOLVE_VALUE_C64(solve_mat_init_stats, mat[i * n + j]); \
            } \
            if (mmse) { \
                mat[i * n + i].r += MIMO_MMSE_LAMBDA_Q15; \
                mat[i * n + i].i = 0; \
                RECORD_SOLVE_VALUE_C64(solve_mat_diag_mmse_stats, \
                                       mat[i * n + i]); \
            } \
        } \
 \
        for (int k = 0; k < n; ++k) { \
            const int64_t pivot = safe_pivot_i64(mat[k * n + k].r); \
            mat[k * n + k].i = 0; \
 \
            for (int i = k + 1; i < n; ++i) { \
                RECORD_SOLVE_VALUE_C64(solve_factor_num_stats, \
                                       mat[i * n + k]); \
                const mimo_c64_t factor_q30 = \
                    c64_make(MIMO_DIV_TO_Q30(mat[i * n + k].r, pivot), \
                             MIMO_DIV_TO_Q30(mat[i * n + k].i, pivot)); \
                RECORD_SOLVE_VALUE_C64(solve_factor_q30_stats, factor_q30); \
 \
                for (int j = k + MIMO_USE_SOLVE_SKIP_LOWER_UPDATE; j < n; ++j) { \
                    const mimo_c64_t t = \
                        c64_mul_q30_q15(factor_q30, mat[k * n + j]); \
                    RECORD_SOLVE_VALUE_C64(solve_elim_mul_stats, t); \
                    mat[i * n + j].r -= t.r; \
                    mat[i * n + j].i -= t.i; \
                    RECORD_SOLVE_VALUE_C64(solve_mat_update_stats, \
                                           mat[i * n + j]); \
                } \
                const mimo_c64_t tb = c64_mul_q30_q15(factor_q30, rhs[k]); \
                RECORD_SOLVE_VALUE_C64(solve_elim_mul_stats, tb); \
                rhs[i].r -= tb.r; \
                rhs[i].i -= tb.i; \
                RECORD_SOLVE_VALUE_C64(solve_rhs_update_stats, rhs[i]); \
                mat[i * n + k] = c64_make(0, 0); \
            } \
        } \
 \
        for (int i = n - 1; i >= 0; --i) { \
            mimo_c64_t sum = rhs[i]; \
            RECORD_SOLVE_VALUE_C64(solve_back_sum_stats, sum); \
 \
            for (int j = i + 1; j < n; ++j) { \
                sum = c64_sub(sum, c64_mul_q15(mat[i * n + j], sol[j])); \
                RECORD_SOLVE_VALUE_C64(solve_back_sum_stats, sum); \
            } \
            sol[i] = MIMO_DIV_REAL_TO_Q15(sum, \
                                          safe_pivot_i64(mat[i * n + i].r)); \
            RECORD_SOLVE_VALUE_C64(solve_sol_stats, sol[i]); \
        } \
 \
        for (int i = 0; i < n; ++i) { \
            RECORD_SOLVE_VALUE_NUM(solve_store_input_stats, \
                                   (int64_t)sol[i].r); \
            RECORD_SOLVE_VALUE_NUM(solve_store_input_stats, \
                                   (int64_t)sol[i].i); \
            x[i] = mimo_make_c16(mimo_solve_store_i16((int32_t)sol[i].r), \
                                 mimo_solve_store_i16((int32_t)sol[i].i)); \
        } \
    }

#if MIMO_USE_SOLVE4_UNROLL_V2A
static void mimo_solve_normal_4_exact(const mimo_c32_t *a,
                                      const mimo_c32_t *b,
                                      int mmse,
                                      mimo_c16_t *x)
{
    mimo_c64_t m00 = c64_make(a[0].r, a[0].i);
    mimo_c64_t m01 = c64_make(a[1].r, a[1].i);
    mimo_c64_t m02 = c64_make(a[2].r, a[2].i);
    mimo_c64_t m03 = c64_make(a[3].r, a[3].i);
    mimo_c64_t m10 = c64_make(a[4].r, a[4].i);
    mimo_c64_t m11 = c64_make(a[5].r, a[5].i);
    mimo_c64_t m12 = c64_make(a[6].r, a[6].i);
    mimo_c64_t m13 = c64_make(a[7].r, a[7].i);
    mimo_c64_t m20 = c64_make(a[8].r, a[8].i);
    mimo_c64_t m21 = c64_make(a[9].r, a[9].i);
    mimo_c64_t m22 = c64_make(a[10].r, a[10].i);
    mimo_c64_t m23 = c64_make(a[11].r, a[11].i);
    mimo_c64_t m30 = c64_make(a[12].r, a[12].i);
    mimo_c64_t m31 = c64_make(a[13].r, a[13].i);
    mimo_c64_t m32 = c64_make(a[14].r, a[14].i);
    mimo_c64_t m33 = c64_make(a[15].r, a[15].i);
    mimo_c64_t r0 = c64_make(b[0].r, b[0].i);
    mimo_c64_t r1 = c64_make(b[1].r, b[1].i);
    mimo_c64_t r2 = c64_make(b[2].r, b[2].i);
    mimo_c64_t r3 = c64_make(b[3].r, b[3].i);

    if (mmse) {
        m00.r += MIMO_MMSE_LAMBDA_Q15;
        m00.i = 0;
        m11.r += MIMO_MMSE_LAMBDA_Q15;
        m11.i = 0;
        m22.r += MIMO_MMSE_LAMBDA_Q15;
        m22.i = 0;
        m33.r += MIMO_MMSE_LAMBDA_Q15;
        m33.i = 0;
    }

    m00.i = 0;
    const int64_t p0 = safe_pivot_i64(m00.r);

    mimo_c64_t f = c64_make(MIMO_DIV_TO_Q30(m10.r, p0),
                            MIMO_DIV_TO_Q30(m10.i, p0));
    m11 = c64_sub(m11, c64_mul_q30_q15(f, m01));
    m12 = c64_sub(m12, c64_mul_q30_q15(f, m02));
    m13 = c64_sub(m13, c64_mul_q30_q15(f, m03));
    r1 = c64_sub(r1, c64_mul_q30_q15(f, r0));

    f = c64_make(MIMO_DIV_TO_Q30(m20.r, p0),
                 MIMO_DIV_TO_Q30(m20.i, p0));
    m21 = c64_sub(m21, c64_mul_q30_q15(f, m01));
    m22 = c64_sub(m22, c64_mul_q30_q15(f, m02));
    m23 = c64_sub(m23, c64_mul_q30_q15(f, m03));
    r2 = c64_sub(r2, c64_mul_q30_q15(f, r0));

    f = c64_make(MIMO_DIV_TO_Q30(m30.r, p0),
                 MIMO_DIV_TO_Q30(m30.i, p0));
    m31 = c64_sub(m31, c64_mul_q30_q15(f, m01));
    m32 = c64_sub(m32, c64_mul_q30_q15(f, m02));
    m33 = c64_sub(m33, c64_mul_q30_q15(f, m03));
    r3 = c64_sub(r3, c64_mul_q30_q15(f, r0));

    m11.i = 0;
    const int64_t p1 = safe_pivot_i64(m11.r);

    f = c64_make(MIMO_DIV_TO_Q30(m21.r, p1),
                 MIMO_DIV_TO_Q30(m21.i, p1));
    m22 = c64_sub(m22, c64_mul_q30_q15(f, m12));
    m23 = c64_sub(m23, c64_mul_q30_q15(f, m13));
    r2 = c64_sub(r2, c64_mul_q30_q15(f, r1));

    f = c64_make(MIMO_DIV_TO_Q30(m31.r, p1),
                 MIMO_DIV_TO_Q30(m31.i, p1));
    m32 = c64_sub(m32, c64_mul_q30_q15(f, m12));
    m33 = c64_sub(m33, c64_mul_q30_q15(f, m13));
    r3 = c64_sub(r3, c64_mul_q30_q15(f, r1));

    m22.i = 0;
    const int64_t p2 = safe_pivot_i64(m22.r);

    f = c64_make(MIMO_DIV_TO_Q30(m32.r, p2),
                 MIMO_DIV_TO_Q30(m32.i, p2));
    m33 = c64_sub(m33, c64_mul_q30_q15(f, m23));
    r3 = c64_sub(r3, c64_mul_q30_q15(f, r2));

    m33.i = 0;

    const mimo_c64_t x3 = MIMO_DIV_REAL_TO_Q15(r3, safe_pivot_i64(m33.r));
    mimo_c64_t sum = c64_sub(r2, c64_mul_q15(m23, x3));
    const mimo_c64_t x2 = MIMO_DIV_REAL_TO_Q15(sum, safe_pivot_i64(m22.r));

    sum = c64_sub(r1, c64_mul_q15(m12, x2));
    sum = c64_sub(sum, c64_mul_q15(m13, x3));
    const mimo_c64_t x1 = MIMO_DIV_REAL_TO_Q15(sum, safe_pivot_i64(m11.r));

    sum = c64_sub(r0, c64_mul_q15(m01, x1));
    sum = c64_sub(sum, c64_mul_q15(m02, x2));
    sum = c64_sub(sum, c64_mul_q15(m03, x3));
    const mimo_c64_t x0 = MIMO_DIV_REAL_TO_Q15(sum, safe_pivot_i64(m00.r));

    x[0] = mimo_make_c16(mimo_solve_store_i16((int32_t)x0.r),
                         mimo_solve_store_i16((int32_t)x0.i));
    x[1] = mimo_make_c16(mimo_solve_store_i16((int32_t)x1.r),
                         mimo_solve_store_i16((int32_t)x1.i));
    x[2] = mimo_make_c16(mimo_solve_store_i16((int32_t)x2.r),
                         mimo_solve_store_i16((int32_t)x2.i));
    x[3] = mimo_make_c16(mimo_solve_store_i16((int32_t)x3.r),
                         mimo_solve_store_i16((int32_t)x3.i));
}
#else
#if !MIMO_USE_SOLVE4_SCHUR
DEFINE_MIMO_SOLVE_NORMAL_EXACT(mimo_solve_normal_4_exact, MIMO_N4)
#endif
#endif

#if MIMO_USE_SOLVE4_SCHUR
static void mimo_solve_normal_4_schur_scalar(const mimo_c32_t *a,
                                             const mimo_c32_t *b,
                                             int mmse,
                                             mimo_c16_t *x)
{
    mimo_collect_schur4_q15(a, b, mmse);

    const mimo_c64_t a00 = schur4_c64_from_c32(a[0]);
    const mimo_c64_t a01 = schur4_c64_from_c32(a[1]);
    const mimo_c64_t a02 = schur4_c64_from_c32(a[2]);
    const mimo_c64_t a03 = schur4_c64_from_c32(a[3]);
    const mimo_c64_t a11 = schur4_c64_from_c32(a[5]);
    const mimo_c64_t a12 = schur4_c64_from_c32(a[6]);
    const mimo_c64_t a13 = schur4_c64_from_c32(a[7]);
    const mimo_c64_t a22 = schur4_c64_from_c32(a[10]);
    const mimo_c64_t a23 = schur4_c64_from_c32(a[11]);
    const mimo_c64_t a33 = schur4_c64_from_c32(a[15]);
    const mimo_c64_t b0 = schur4_c64_from_c32(b[0]);
    const mimo_c64_t b1 = schur4_c64_from_c32(b[1]);
    const mimo_c64_t b2 = schur4_c64_from_c32(b[2]);
    const mimo_c64_t b3 = schur4_c64_from_c32(b[3]);

    schur4_solve_lane_q15(a00, a01, a02, a03, a11, a12, a13,
                          a22, a23, a33, b0, b1, b2, b3, mmse, x);
}
#endif

void mimo_solve_normal4_scalar_q15(const mimo_c32_t *a,
                                   const mimo_c32_t *b,
                                   int mmse,
                                   mimo_c16_t *x)
{
#if MIMO_USE_SOLVE4_SCHUR
    mimo_solve_normal_4_schur_scalar(a, b, mmse, x);
#else
    mimo_collect_schur4_q15(a, b, mmse);
    mimo_solve_normal_4_exact(a, b, mmse, x);
#endif
}

void mimo_solve_normal_q15(int n,
                           const mimo_c32_t *a,
                           const mimo_c32_t *b,
                           int mmse,
                           mimo_c16_t *x)
{
    if (n == MIMO_N4) {
        mimo_solve_normal4_scalar_q15(a, b, mmse, x);
    } else {
        for (int i = 0; i < n; ++i) {
            x[i] = mimo_make_c16(0, 0);
        }
    }
}

#if MIMO_ENABLE_EQ2
#ifndef MIMO_EQ2_RECIP_MODE
#define MIMO_EQ2_RECIP_MODE 5
#endif

#ifndef MIMO_EQ2_RECIP_NUM_SHIFT
#define MIMO_EQ2_RECIP_NUM_SHIFT 13
#endif

#ifndef MIMO_USE_EQ2_DIRECT_Q13_INV
#define MIMO_USE_EQ2_DIRECT_Q13_INV 0
#endif

#ifndef MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP
#define MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP 0
#endif

#ifndef MIMO_USE_EQ2_MMSE_DIRECT_H
#define MIMO_USE_EQ2_MMSE_DIRECT_H 0
#endif

#ifndef MIMO_USE_SCALAR_EQ2_FUSED_LAMBDA
#define MIMO_USE_SCALAR_EQ2_FUSED_LAMBDA 0
#endif

#ifndef MIMO_USE_SCALAR_EQ2_MMSE_FULL_TRUNC_SCALE
#define MIMO_USE_SCALAR_EQ2_MMSE_FULL_TRUNC_SCALE 0
#endif

#if MIMO_USE_EQ2_DIRECT_Q13_INV && !MIMO_USE_EQ2_ZF_DIRECT_H
#error "EQ2 Q13 inverse coefficient is valid only for the ZF direct-H path"
#endif

#if MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP && !MIMO_USE_EQ2_ZF_DIRECT_H
#error "EQ2 guard1 reciprocal is valid only for the ZF direct-H path"
#endif

#if MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP && MIMO_USE_EQ2_DIRECT_Q13_INV
#error "EQ2 guard1 reciprocal and Q13 inverse coefficient are mutually exclusive"
#endif

#if MIMO_EQ2_RECIP_MODE != 5
#error "The initial EQ2 path requires the no-harddiv direct1 reciprocal mode"
#endif

#if MIMO_EQ2_RECIP_NUM_SHIFT != 13
#error "The initial EQ2 path requires reciprocal numerator shift13"
#endif

#define MIMO_EQ2_DET_Q15_RECIP_MIN 3400u
#define MIMO_EQ2_DET_Q15_RECIP_LAST 1008u
#define MIMO_EQ2_DET_Q15_RECIP_ENTRY(rel_) \
    ((uint32_t)(1073741824ull / \
                (MIMO_EQ2_DET_Q15_RECIP_MIN + (uint32_t)(rel_))) - \
     ((uint32_t)(rel_) == 837u ? 10u : 0u))

static const uint32_t mimo_eq2_det_q15_recip_direct1_q30[] MIMO_ALIGN64 = {
#define MIMO_SCHUR4_DET_Q15_RECIP_DIRECT1_ENTRY(rel_) \
    MIMO_EQ2_DET_Q15_RECIP_ENTRY(rel_)
#include "mimo_schur_recip_direct1.inc"
};

#undef MIMO_SCHUR4_DET_Q15_RECIP_DIRECT1_ENTRY
#undef MIMO_EQ2_DET_Q15_RECIP_ENTRY

#if MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP
#define MIMO_EQ2_DET_Q16_RECIP_MIN 6800u
static const uint32_t mimo_eq2_det_q16_recip_direct_half_q15[] MIMO_ALIGN64 = {
#define MIMO_SCHUR4_DET_Q15_RECIP_DIRECT1_ENTRY(rel_) \
    ((uint32_t)(2147483648ull / \
                (MIMO_EQ2_DET_Q16_RECIP_MIN + 2u * (uint32_t)(rel_)))), \
    ((uint32_t)(2147483648ull / \
                (MIMO_EQ2_DET_Q16_RECIP_MIN + 2u * (uint32_t)(rel_) + 1u)))
#include "mimo_schur_recip_direct1.inc"
};

#undef MIMO_SCHUR4_DET_Q15_RECIP_DIRECT1_ENTRY
#endif

static MIMO_ALWAYS_INLINE int32_t mimo_eq2_round_q30_to_q15(int64_t value)
{
    return (int32_t)((value + MIMO_Q15_ROUND) >> MIMO_Q15_SHIFT);
}

#if MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP
static MIMO_ALWAYS_INLINE int32_t mimo_eq2_round_q30_to_q16(int64_t value)
{
    return (int32_t)((value + (1ll << 13)) >> 14);
}

static MIMO_ALWAYS_INLINE int32_t mimo_eq2_scale_q32_to_q15_guard1(
    int64_t numerator_q32,
    uint32_t scale_q15)
{
    return (int32_t)((numerator_q32 * (int64_t)scale_q15) >> 32);
}
#endif

static MIMO_ALWAYS_INLINE int32_t mimo_eq2_scale_q30_to_q15(
    int64_t numerator_q30,
    uint32_t scale_q30)
{
    const int32_t numerator_q13 =
        (int32_t)(numerator_q30 >> MIMO_EQ2_RECIP_NUM_SHIFT);
    const int64_t product = (int64_t)numerator_q13 * scale_q30;

    return (int32_t)(product >> (30 - MIMO_EQ2_RECIP_NUM_SHIFT));
}

#if MIMO_USE_SCALAR_EQ2_MMSE_FULL_TRUNC_SCALE
static MIMO_ALWAYS_INLINE int64_t mimo_eq2_scale_q30_full_rnu(
    int64_t numerator_q30,
    uint32_t scale_q30)
{
    const int64_t product = numerator_q30 * (int64_t)scale_q30;

    return (product + (1ll << 29)) >> 30;
}
#endif

static MIMO_ALWAYS_INLINE int16_t mimo_eq2_store_i16(int64_t value)
{
#if MIMO_USE_SOLVE_STORE_TRUST_I16
    return (int16_t)value;
#else
    const int64_t narrowed = (int16_t)value;

    if (__builtin_expect(value != narrowed, 0)) {
        return value < 0 ? -32768 : 32767;
    }
    return (int16_t)narrowed;
#endif
}

static MIMO_ALWAYS_INLINE void mimo_eq2_scalar_direct_h_loop(
    size_t site_count,
    const mimo_c16_t *restrict h_row,
    const mimo_c16_t *restrict y,
    mimo_c16_t *restrict x)
{
    for (size_t site = 0; site < site_count; ++site) {
        const mimo_c16_t *const h = &h_row[site * MIMO_N2 * MIMO_N2];
        const mimo_c16_t *const rhs = &y[site * MIMO_N2];
        mimo_c16_t *const out = &x[site * MIMO_N2];
        const int64_t h00r = h[0].r;
        const int64_t h00i = h[0].i;
        const int64_t h01r = h[1].r;
        const int64_t h01i = h[1].i;
        const int64_t h10r = h[2].r;
        const int64_t h10i = h[2].i;
        const int64_t h11r = h[3].r;
        const int64_t h11i = h[3].i;
        const int64_t y0r = rhs[0].r;
        const int64_t y0i = rhs[0].i;
        const int64_t y1r = rhs[1].r;
        const int64_t y1i = rhs[1].i;

#if MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP
        const int32_t dr = mimo_eq2_round_q30_to_q16(
#else
        const int32_t dr = mimo_eq2_round_q30_to_q15(
#endif
            h00r * h11r - h00i * h11i -
            h01r * h10r + h01i * h10i);
#if MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP
        const int32_t di = mimo_eq2_round_q30_to_q16(
#else
        const int32_t di = mimo_eq2_round_q30_to_q15(
#endif
            h00r * h11i + h00i * h11r -
            h01r * h10i - h01i * h10r);
#if MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP
        const int32_t n0r = mimo_eq2_round_q30_to_q16(
#else
        const int32_t n0r = mimo_eq2_round_q30_to_q15(
#endif
            h11r * y0r - h11i * y0i -
            h01r * y1r + h01i * y1i);
#if MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP
        const int32_t n0i = mimo_eq2_round_q30_to_q16(
#else
        const int32_t n0i = mimo_eq2_round_q30_to_q15(
#endif
            h11r * y0i + h11i * y0r -
            h01r * y1i - h01i * y1r);
#if MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP
        const int32_t n1r = mimo_eq2_round_q30_to_q16(
#else
        const int32_t n1r = mimo_eq2_round_q30_to_q15(
#endif
            h00r * y1r - h00i * y1i -
            h10r * y0r + h10i * y0i);
#if MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP
        const int32_t n1i = mimo_eq2_round_q30_to_q16(
#else
        const int32_t n1i = mimo_eq2_round_q30_to_q15(
#endif
            h00r * y1i + h00i * y1r -
            h10r * y0i - h10i * y0r);
#if MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP
        const int64_t det_q32 = (int64_t)dr * dr + (int64_t)di * di;
        const uint32_t det_q16 =
            (uint32_t)((det_q32 + (1ll << 15)) >> 16);
        const uint32_t scale_q15 =
            mimo_eq2_det_q16_recip_direct_half_q15[
                det_q16 - MIMO_EQ2_DET_Q16_RECIP_MIN];
        const int64_t c0r = (int64_t)n0r * dr + (int64_t)n0i * di;
        const int64_t c0i = (int64_t)n0i * dr - (int64_t)n0r * di;
        const int64_t c1r = (int64_t)n1r * dr + (int64_t)n1i * di;
        const int64_t c1i = (int64_t)n1i * dr - (int64_t)n1r * di;

        out[0] = mimo_make_c16(
            mimo_eq2_store_i16(
                mimo_eq2_scale_q32_to_q15_guard1(c0r, scale_q15)),
            mimo_eq2_store_i16(
                mimo_eq2_scale_q32_to_q15_guard1(c0i, scale_q15)));
        out[1] = mimo_make_c16(
            mimo_eq2_store_i16(
                mimo_eq2_scale_q32_to_q15_guard1(c1r, scale_q15)),
            mimo_eq2_store_i16(
                mimo_eq2_scale_q32_to_q15_guard1(c1i, scale_q15)));
#else
        const int64_t det_q30 = (int64_t)dr * dr + (int64_t)di * di;
        const uint32_t det_q15 =
            (uint32_t)mimo_eq2_round_q30_to_q15(det_q30);
        const uint32_t scale_q30 =
            mimo_eq2_det_q15_recip_direct1_q30[
                det_q15 - MIMO_EQ2_DET_Q15_RECIP_MIN];
#if MIMO_USE_EQ2_DIRECT_Q13_INV
        const uint32_t scale_q26 = (uint16_t)(scale_q30 >> 4);
        const int32_t inv_r =
            mimo_eq2_round_q30_to_q15((int64_t)dr * scale_q26) << 2;
        const int32_t inv_i =
            mimo_eq2_round_q30_to_q15((int64_t)(-di) * scale_q26) << 2;
        const int64_t x0r_q30 =
            (int64_t)n0r * inv_r - (int64_t)n0i * inv_i;
        const int64_t x0i_q30 =
            (int64_t)n0r * inv_i + (int64_t)n0i * inv_r;
        const int64_t x1r_q30 =
            (int64_t)n1r * inv_r - (int64_t)n1i * inv_i;
        const int64_t x1i_q30 =
            (int64_t)n1r * inv_i + (int64_t)n1i * inv_r;

        out[0] = mimo_make_c16(
            mimo_eq2_store_i16(mimo_eq2_round_q30_to_q15(x0r_q30) << 2),
            mimo_eq2_store_i16(mimo_eq2_round_q30_to_q15(x0i_q30) << 2));
        out[1] = mimo_make_c16(
            mimo_eq2_store_i16(mimo_eq2_round_q30_to_q15(x1r_q30) << 2),
            mimo_eq2_store_i16(mimo_eq2_round_q30_to_q15(x1i_q30) << 2));
#else
        const int64_t c0r = (int64_t)n0r * dr + (int64_t)n0i * di;
        const int64_t c0i = (int64_t)n0i * dr - (int64_t)n0r * di;
        const int64_t c1r = (int64_t)n1r * dr + (int64_t)n1i * di;
        const int64_t c1i = (int64_t)n1i * dr - (int64_t)n1r * di;

        out[0] = mimo_make_c16(
            mimo_eq2_store_i16(mimo_eq2_scale_q30_to_q15(c0r, scale_q30)),
            mimo_eq2_store_i16(mimo_eq2_scale_q30_to_q15(c0i, scale_q30)));
        out[1] = mimo_make_c16(
            mimo_eq2_store_i16(mimo_eq2_scale_q30_to_q15(c1r, scale_q30)),
            mimo_eq2_store_i16(mimo_eq2_scale_q30_to_q15(c1i, scale_q30)));
#endif
#endif
    }
}

static MIMO_ALWAYS_INLINE void mimo_eq2_scalar_fused_loop(
    size_t site_count,
    const mimo_c16_t *restrict h_row,
    const mimo_c16_t *restrict y,
    int32_t lambda_q15,
    mimo_c16_t *restrict x)
{
    for (size_t site = 0; site < site_count; ++site) {
        const mimo_c16_t *const h = &h_row[site * MIMO_N2 * MIMO_N2];
        const mimo_c16_t *const rhs = &y[site * MIMO_N2];
        mimo_c16_t *const out = &x[site * MIMO_N2];

        const int64_t h00r = h[0].r;
        const int64_t h00i = h[0].i;
        const int64_t h01r = h[1].r;
        const int64_t h01i = h[1].i;
        const int64_t h10r = h[2].r;
        const int64_t h10i = h[2].i;
        const int64_t h11r = h[3].r;
        const int64_t h11i = h[3].i;
        const int64_t y0r = rhs[0].r;
        const int64_t y0i = rhs[0].i;
        const int64_t y1r = rhs[1].r;
        const int64_t y1i = rhs[1].i;

        const int32_t a00 = mimo_eq2_round_q30_to_q15(
            h00r * h00r + h00i * h00i + h10r * h10r + h10i * h10i
#if MIMO_USE_SCALAR_EQ2_FUSED_LAMBDA
            + ((int64_t)lambda_q15 << MIMO_Q15_SHIFT)
#endif
            )
#if !MIMO_USE_SCALAR_EQ2_FUSED_LAMBDA
            + lambda_q15
#endif
            ;
        const int32_t a01r = mimo_eq2_round_q30_to_q15(
            h00r * h01r + h00i * h01i + h10r * h11r + h10i * h11i);
        const int32_t a01i = mimo_eq2_round_q30_to_q15(
            h00r * h01i - h00i * h01r + h10r * h11i - h10i * h11r);
        const int32_t a11 = mimo_eq2_round_q30_to_q15(
            h01r * h01r + h01i * h01i + h11r * h11r + h11i * h11i
#if MIMO_USE_SCALAR_EQ2_FUSED_LAMBDA
            + ((int64_t)lambda_q15 << MIMO_Q15_SHIFT)
#endif
            )
#if !MIMO_USE_SCALAR_EQ2_FUSED_LAMBDA
            + lambda_q15
#endif
            ;

        const int32_t b0r = mimo_eq2_round_q30_to_q15(
            h00r * y0r + h00i * y0i + h10r * y1r + h10i * y1i);
        const int32_t b0i = mimo_eq2_round_q30_to_q15(
            h00r * y0i - h00i * y0r + h10r * y1i - h10i * y1r);
        const int32_t b1r = mimo_eq2_round_q30_to_q15(
            h01r * y0r + h01i * y0i + h11r * y1r + h11i * y1i);
        const int32_t b1i = mimo_eq2_round_q30_to_q15(
            h01r * y0i - h01i * y0r + h11r * y1i - h11i * y1r);

        const int64_t det_q30 =
            (int64_t)a00 * a11 - (int64_t)a01r * a01r -
            (int64_t)a01i * a01i;
        const uint32_t det_q15 =
            (uint32_t)((det_q30 + MIMO_Q15_ROUND) >> MIMO_Q15_SHIFT);
        const uint32_t scale_q30 =
            mimo_eq2_det_q15_recip_direct1_q30[
                det_q15 - MIMO_EQ2_DET_Q15_RECIP_MIN];

        const int64_t n0r =
            (int64_t)a11 * b0r - (int64_t)a01r * b1r +
            (int64_t)a01i * b1i;
        const int64_t n0i =
            (int64_t)a11 * b0i - (int64_t)a01r * b1i -
            (int64_t)a01i * b1r;
        const int64_t n1r =
            (int64_t)a00 * b1r - (int64_t)a01r * b0r -
            (int64_t)a01i * b0i;
        const int64_t n1i =
            (int64_t)a00 * b1i - (int64_t)a01r * b0i +
            (int64_t)a01i * b0r;

        RECORD_SCHUR_NUM(schur_s_det_q30_stats, det_q30);
        RECORD_SCHUR_NUM(schur_solve_num_q30_stats, n0r);
        RECORD_SCHUR_NUM(schur_solve_num_q30_stats, n0i);
        RECORD_SCHUR_NUM(schur_solve_num_q30_stats, n1r);
        RECORD_SCHUR_NUM(schur_solve_num_q30_stats, n1i);

#if MIMO_USE_SCALAR_EQ2_MMSE_FULL_TRUNC_SCALE
        if (lambda_q15 != 0) {
            out[0] = mimo_make_c16(
                mimo_eq2_store_i16(mimo_eq2_scale_q30_full_rnu(
                    n0r, scale_q30)),
                mimo_eq2_store_i16(mimo_eq2_scale_q30_full_rnu(
                    n0i, scale_q30)));
            out[1] = mimo_make_c16(
                mimo_eq2_store_i16(mimo_eq2_scale_q30_full_rnu(
                    n1r, scale_q30)),
                mimo_eq2_store_i16(mimo_eq2_scale_q30_full_rnu(
                    n1i, scale_q30)));
            continue;
        }
#endif

        out[0] = mimo_make_c16(
            mimo_eq2_store_i16(mimo_eq2_scale_q30_to_q15(n0r, scale_q30)),
            mimo_eq2_store_i16(mimo_eq2_scale_q30_to_q15(n0i, scale_q30)));
        out[1] = mimo_make_c16(
            mimo_eq2_store_i16(mimo_eq2_scale_q30_to_q15(n1r, scale_q30)),
            mimo_eq2_store_i16(mimo_eq2_scale_q30_to_q15(n1i, scale_q30)));
    }
}

#if MIMO_USE_EQ2_MMSE_DIRECT_H
static MIMO_ALWAYS_INLINE void mimo_eq2_scalar_mmse_direct_h_loop(
    size_t site_count,
    const mimo_c16_t *restrict h_row,
    const mimo_c16_t *restrict y,
    mimo_c16_t *restrict x)
{
    const int64_t lambda_q15 = MIMO_MMSE_LAMBDA_Q15;

    for (size_t site = 0; site < site_count; ++site) {
        const mimo_c16_t *const h = &h_row[site * MIMO_N2 * MIMO_N2];
        const mimo_c16_t *const rhs = &y[site * MIMO_N2];
        mimo_c16_t *const out = &x[site * MIMO_N2];
        const int64_t h00r = h[0].r;
        const int64_t h00i = h[0].i;
        const int64_t h01r = h[1].r;
        const int64_t h01i = h[1].i;
        const int64_t h10r = h[2].r;
        const int64_t h10i = h[2].i;
        const int64_t h11r = h[3].r;
        const int64_t h11i = h[3].i;
        const int64_t y0r = rhs[0].r;
        const int64_t y0i = rhs[0].i;
        const int64_t y1r = rhs[1].r;
        const int64_t y1i = rhs[1].i;

        const int32_t dr = mimo_eq2_round_q30_to_q15(
            h00r * h11r - h00i * h11i -
            h01r * h10r + h01i * h10i);
        const int32_t di = mimo_eq2_round_q30_to_q15(
            h00r * h11i + h00i * h11r -
            h01r * h10i - h01i * h10r);
        const int32_t n0r = mimo_eq2_round_q30_to_q15(
            h11r * y0r - h11i * y0i -
            h01r * y1r + h01i * y1i);
        const int32_t n0i = mimo_eq2_round_q30_to_q15(
            h11r * y0i + h11i * y0r -
            h01r * y1i - h01i * y1r);
        const int32_t n1r = mimo_eq2_round_q30_to_q15(
            h00r * y1r - h00i * y1i -
            h10r * y0r + h10i * y0i);
        const int32_t n1i = mimo_eq2_round_q30_to_q15(
            h00r * y1i + h00i * y1r -
            h10r * y0i - h10i * y0r);
        const int32_t b0r = mimo_eq2_round_q30_to_q15(
            h00r * y0r + h00i * y0i + h10r * y1r + h10i * y1i);
        const int32_t b0i = mimo_eq2_round_q30_to_q15(
            h00r * y0i - h00i * y0r + h10r * y1i - h10i * y1r);
        const int32_t b1r = mimo_eq2_round_q30_to_q15(
            h01r * y0r + h01i * y0i + h11r * y1r + h11i * y1i);
        const int32_t b1i = mimo_eq2_round_q30_to_q15(
            h01r * y0i - h01i * y0r + h11r * y1i - h11i * y1r);
        const int32_t norm_q15 = mimo_eq2_round_q30_to_q15(
            h00r * h00r + h00i * h00i +
            h01r * h01r + h01i * h01i +
            h10r * h10r + h10i * h10i +
            h11r * h11r + h11i * h11i);

        const int64_t den_q30 =
            (int64_t)dr * dr + (int64_t)di * di +
            lambda_q15 * norm_q15 + lambda_q15 * lambda_q15;
        const uint32_t den_q15 =
            (uint32_t)mimo_eq2_round_q30_to_q15(den_q30);
        const uint32_t scale_q30 =
            mimo_eq2_det_q15_recip_direct1_q30[
                den_q15 - MIMO_EQ2_DET_Q15_RECIP_MIN];
        const int64_t c0r =
            (int64_t)n0r * dr + (int64_t)n0i * di + lambda_q15 * b0r;
        const int64_t c0i =
            (int64_t)n0i * dr - (int64_t)n0r * di + lambda_q15 * b0i;
        const int64_t c1r =
            (int64_t)n1r * dr + (int64_t)n1i * di + lambda_q15 * b1r;
        const int64_t c1i =
            (int64_t)n1i * dr - (int64_t)n1r * di + lambda_q15 * b1i;

        out[0] = mimo_make_c16(
            mimo_eq2_store_i16(mimo_eq2_scale_q30_to_q15(c0r, scale_q30)),
            mimo_eq2_store_i16(mimo_eq2_scale_q30_to_q15(c0i, scale_q30)));
        out[1] = mimo_make_c16(
            mimo_eq2_store_i16(mimo_eq2_scale_q30_to_q15(c1r, scale_q30)),
            mimo_eq2_store_i16(mimo_eq2_scale_q30_to_q15(c1i, scale_q30)));
    }
}
#endif

void mimo_eq2_zf_scalar_fused_q15(size_t site_count,
                                  const mimo_c16_t *restrict h_row,
                                  const mimo_c16_t *restrict y,
                                  mimo_c16_t *restrict x)
{
#if MIMO_USE_EQ2_ZF_DIRECT_H
    mimo_eq2_scalar_direct_h_loop(site_count, h_row, y, x);
#else
    mimo_eq2_scalar_fused_loop(site_count, h_row, y, 0, x);
#endif
}

void mimo_eq2_mmse_scalar_fused_q15(size_t site_count,
                                    const mimo_c16_t *restrict h_row,
                                    const mimo_c16_t *restrict y,
                                    mimo_c16_t *restrict x)
{
#if MIMO_USE_EQ2_MMSE_DIRECT_H
    mimo_eq2_scalar_mmse_direct_h_loop(site_count, h_row, y, x);
#else
    mimo_eq2_scalar_fused_loop(site_count, h_row, y,
                               MIMO_MMSE_LAMBDA_Q15, x);
#endif
}

#undef MIMO_EQ2_DET_Q15_RECIP_LAST
#undef MIMO_EQ2_DET_Q15_RECIP_MIN
#endif
