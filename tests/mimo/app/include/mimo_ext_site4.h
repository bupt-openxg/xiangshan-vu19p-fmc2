#ifndef MIMO_EXT_SITE4_H
#define MIMO_EXT_SITE4_H

#include "mimo_ext.h"
#include "mimo_kernels.h"
#include "mimo_rvv_solve4.h"

#ifndef MIMO_USE_EXT_ZF4_RVV_ROUND_ONCE_BUILD
#define MIMO_USE_EXT_ZF4_RVV_ROUND_ONCE_BUILD 0
#endif

#if MIMO_USE_EXT_ZF4_RVV_ROUND_ONCE_BUILD
#include "mimo_rvv_site4.h"
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

#ifndef MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED
#define MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED 0
#endif

#ifndef MIMO_USE_SITE4_HERMITIAN_UPPER_ROWFUSE
#define MIMO_USE_SITE4_HERMITIAN_UPPER_ROWFUSE 0
#endif

#ifndef MIMO_USE_SITE4_HERMITIAN_UPPER_ROWPAIR
#define MIMO_USE_SITE4_HERMITIAN_UPPER_ROWPAIR 0
#endif

#ifndef MIMO_USE_SITE4_PRELUDE_HOIST
#define MIMO_USE_SITE4_PRELUDE_HOIST 0
#endif

#ifndef MIMO_USE_SITE4_BUILD_SOLVE_PIPELINE2
#define MIMO_USE_SITE4_BUILD_SOLVE_PIPELINE2 0
#endif

#ifndef MIMO_USE_EXT_ZF4_LOCAL_SOLVE
#define MIMO_USE_EXT_ZF4_LOCAL_SOLVE 0
#endif

#ifndef MIMO_USE_EXT_ZF4_RVV_NODIV_SOLVE
#define MIMO_USE_EXT_ZF4_RVV_NODIV_SOLVE 0
#endif

#ifndef MIMO_USE_EXT_ZF4_RVV_NODIV_FAST_SOLVE
#define MIMO_USE_EXT_ZF4_RVV_NODIV_FAST_SOLVE 0
#endif

#ifndef MIMO_USE_EXT_ZF4_SCHUR_RVV_SOLVE
#define MIMO_USE_EXT_ZF4_SCHUR_RVV_SOLVE 0
#endif

#ifndef MIMO_USE_EXT_MMSE4_SCHUR_RVV_SOLVE
#define MIMO_USE_EXT_MMSE4_SCHUR_RVV_SOLVE 0
#endif

#ifndef MIMO_USE_EXT_MMSE4_FUSED_BUILD_SOLVE
#define MIMO_USE_EXT_MMSE4_FUSED_BUILD_SOLVE 0
#endif

#ifndef MIMO_USE_EXT_MMSE4_NARROW_WORK
#define MIMO_USE_EXT_MMSE4_NARROW_WORK 0
#endif

#ifndef MIMO_USE_EXT_MMSE4_LIVE_BUILD
#define MIMO_USE_EXT_MMSE4_LIVE_BUILD 0
#endif

#ifndef MIMO_USE_EXT_MMSE4_STREAM_WORK_SOLVE
#define MIMO_USE_EXT_MMSE4_STREAM_WORK_SOLVE 0
#endif

#ifndef MIMO_USE_EXT_MMSE4_TRUE16B_INLINE_SOLVE
#define MIMO_USE_EXT_MMSE4_TRUE16B_INLINE_SOLVE 0
#endif

#ifndef MIMO_USE_EXT_MMSE4_TRUE16B_LIVE_TAIL_SOLVE
#define MIMO_USE_EXT_MMSE4_TRUE16B_LIVE_TAIL_SOLVE 0
#endif

#ifndef MIMO_USE_EXT_ZF4_TRUE16B_WORK
#define MIMO_USE_EXT_ZF4_TRUE16B_WORK 0
#endif

#ifndef MIMO_USE_EXT_ZF4_PACK32_WORK
#define MIMO_USE_EXT_ZF4_PACK32_WORK 0
#endif

#ifndef MIMO_USE_EXT_ZF4_VSCMACC_BUILD
#define MIMO_USE_EXT_ZF4_VSCMACC_BUILD 0
#endif

#ifndef MIMO_USE_EXT_ZF4_VSCMACC_FUSED_BUILD
#define MIMO_USE_EXT_ZF4_VSCMACC_FUSED_BUILD 0
#endif

#ifndef MIMO_USE_EXT_ZF4_VSCMACC_SEED_BUILD
#define MIMO_USE_EXT_ZF4_VSCMACC_SEED_BUILD 0
#endif

#ifndef MIMO_USE_EXT_ZF4_INLINE_UPPER_I16_SOLVE
#define MIMO_USE_EXT_ZF4_INLINE_UPPER_I16_SOLVE 0
#endif

#ifndef MIMO_USE_EXT_ZF4_LIVE_TAIL_SCHUR
#define MIMO_USE_EXT_ZF4_LIVE_TAIL_SCHUR 0
#endif

#ifndef MIMO_USE_EXT_ZF4_TAIL_SCRATCH_REUSE
#define MIMO_USE_EXT_ZF4_TAIL_SCRATCH_REUSE 0
#endif

#ifndef MIMO_USE_EXT_ZF4_INLINE_HEAD_WORK_REUSE
#define MIMO_USE_EXT_ZF4_INLINE_HEAD_WORK_REUSE 0
#endif

#ifndef MIMO_USE_EXT_ZF4_TOP2_MACC_SUM
#define MIMO_USE_EXT_ZF4_TOP2_MACC_SUM 0
#endif

#ifndef MIMO_USE_EXT_ZF4_SOLVE2_DIRECT_STORE
#define MIMO_USE_EXT_ZF4_SOLVE2_DIRECT_STORE 0
#endif

#define MIMO_EXT_USE_LIVE_TAIL_SCHUR \
    (MIMO_USE_EXT_ZF4_LIVE_TAIL_SCHUR || \
     MIMO_USE_EXT_MMSE4_TRUE16B_LIVE_TAIL_SOLVE)

#ifndef MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_ONLY
#define MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_ONLY 0
#endif

#ifndef MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_MASK
#define MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_MASK 0
#endif

#ifndef MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE
#define MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE 0
#endif

#ifndef MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SPLIT_SOLVE
#define MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SPLIT_SOLVE 0
#endif

#ifndef MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_STORE
#define MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_STORE 0
#endif

#ifndef MIMO_USE_EXT_MMSE4_RVV_I32_VEC_LAMBDA
#define MIMO_USE_EXT_MMSE4_RVV_I32_VEC_LAMBDA 0
#endif

#ifndef MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PRESERVE_PRELUDE
#define MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PRESERVE_PRELUDE 0
#endif

#ifndef MIMO_USE_EXT_ZF4_UPPER_RVV_SOLVE
#define MIMO_USE_EXT_ZF4_UPPER_RVV_SOLVE 0
#endif

#ifndef MIMO_USE_EXT_ZF4_MIXED_UPDATE_SOLVE
#define MIMO_USE_EXT_ZF4_MIXED_UPDATE_SOLVE 0
#endif

#ifndef MIMO_DEBUG_EXT_ZF4_RVV_NODIV_COMPARE
#define MIMO_DEBUG_EXT_ZF4_RVV_NODIV_COMPARE 0
#endif

#ifndef MIMO_DEBUG_ZF4_BUILD_ONLY
#define MIMO_DEBUG_ZF4_BUILD_ONLY 0
#endif

#ifndef MIMO_SOLVE4_HERMITIAN_Q30_JUMP_ONLY_MASK
#define MIMO_SOLVE4_HERMITIAN_Q30_JUMP_ONLY_MASK 0
#endif

#ifndef MIMO_SOLVE4_HERMITIAN_Q15_JUMP_ONLY_MASK
#define MIMO_SOLVE4_HERMITIAN_Q15_JUMP_ONLY_MASK 0
#endif

#if MIMO_DEBUG_EXT_ZF4_RVV_NODIV_COMPARE
#include "klib.h"
#endif

#if (MIMO_USE_EXT_ZF4_RVV_NODIV_SOLVE || \
     MIMO_USE_EXT_ZF4_RVV_NODIV_FAST_SOLVE) && \
    MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED
#error "EXT ZF4 RVV no-div solve requires unpacked Hermitian upper SoA"
#endif

#if (MIMO_USE_EXT_ZF4_LOCAL_SOLVE + MIMO_USE_EXT_ZF4_RVV_NODIV_SOLVE + \
     MIMO_USE_EXT_ZF4_RVV_NODIV_FAST_SOLVE + \
     MIMO_USE_EXT_ZF4_SCHUR_RVV_SOLVE + \
     MIMO_USE_EXT_ZF4_TRUE16B_WORK + \
     MIMO_USE_EXT_ZF4_UPPER_RVV_SOLVE + \
     MIMO_USE_EXT_ZF4_MIXED_UPDATE_SOLVE) > 1
#error "Select at most one EXT ZF4 specialized solve path"
#endif

#if MIMO_USE_EXT_ZF4_SCHUR_RVV_SOLVE && \
    (!MIMO_USE_SITE4_SOLVE_SOA || !MIMO_USE_SITE4_HERMITIAN_UPPER_BUILD || \
     !MIMO_USE_SOLVE4_SCHUR || MIMO_USE_SOLVE4_SCHUR_PAIR2 || \
     MIMO_USE_SOLVE4_SCHUR_DET_SCALE || MIMO_USE_SOLVE4_SCHUR_BINV_Q15 || \
     MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 4 || \
     MIMO_USE_SOLVE4_SCHUR_FUSED_INNER || \
     MIMO_USE_SOLVE4_SCHUR_ZF_EXPLICIT || \
     MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT != 13 || \
     MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED || MIMO_COLLECT_SCHUR_STATS)
#error "EXT ZF4 Schur RVV solve requires unpacked SoA upper-build ZF Schur q15recip mode4 num_shift=13"
#endif

#if MIMO_USE_EXT_MMSE4_SCHUR_RVV_SOLVE && \
    (!MIMO_USE_SITE4_SOLVE_SOA || !MIMO_USE_SITE4_HERMITIAN_UPPER_BUILD || \
     !MIMO_USE_SITE4_HERMITIAN_UPPER_MMSE || !MIMO_USE_SOLVE4_SCHUR || \
     MIMO_USE_SOLVE4_SCHUR_PAIR2 || MIMO_USE_SOLVE4_SCHUR_DET_SCALE || \
     MIMO_USE_SOLVE4_SCHUR_BINV_Q15 || MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 5 || \
     MIMO_USE_SOLVE4_SCHUR_FUSED_INNER || \
     MIMO_USE_SOLVE4_SCHUR_ZF_EXPLICIT || \
     MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT != 13 || \
     MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED || MIMO_COLLECT_SCHUR_STATS)
#error "EXT MMSE4 Schur RVV solve requires unpacked SoA upper-build MMSE Schur q15recip mode5 num_shift=13"
#endif

#if MIMO_USE_EXT_MMSE4_FUSED_BUILD_SOLVE && \
    (!MIMO_USE_EXT_MMSE4_SCHUR_RVV_SOLVE || \
     !MIMO_USE_SITE4_HERMITIAN_UPPER_ROWFUSE || \
     MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED || MIMO_COLLECT_SCHUR_STATS)
#error "EXT MMSE4 fused build/solve requires EXT MMSE Schur RVV rowfuse unpacked SoA path"
#endif

#if MIMO_USE_EXT_MMSE4_NARROW_WORK && !MIMO_USE_EXT_MMSE4_FUSED_BUILD_SOLVE
#error "MIMO_USE_EXT_MMSE4_NARROW_WORK requires MIMO_USE_EXT_MMSE4_FUSED_BUILD_SOLVE=1"
#endif

#if MIMO_USE_EXT_MMSE4_LIVE_BUILD && !MIMO_USE_EXT_MMSE4_FUSED_BUILD_SOLVE
#error "MIMO_USE_EXT_MMSE4_LIVE_BUILD requires MIMO_USE_EXT_MMSE4_FUSED_BUILD_SOLVE=1"
#endif

#if MIMO_USE_EXT_MMSE4_LIVE_BUILD
#error "MIMO_USE_EXT_MMSE4_LIVE_BUILD is disabled after local NEMU trap: GCC vr outputs can allocate odd widening destination registers; redesign with explicit even-reg compaction before enabling"
#endif

#if MIMO_USE_EXT_MMSE4_LIVE_BUILD && MIMO_USE_EXT_MMSE4_NARROW_WORK
#error "MIMO_USE_EXT_MMSE4_LIVE_BUILD and MIMO_USE_EXT_MMSE4_NARROW_WORK are mutually exclusive candidates"
#endif

#if MIMO_USE_EXT_MMSE4_STREAM_WORK_SOLVE && !MIMO_USE_EXT_MMSE4_FUSED_BUILD_SOLVE
#error "MIMO_USE_EXT_MMSE4_STREAM_WORK_SOLVE requires MIMO_USE_EXT_MMSE4_FUSED_BUILD_SOLVE=1"
#endif

#if MIMO_USE_EXT_MMSE4_STREAM_WORK_SOLVE && \
    (MIMO_USE_EXT_MMSE4_NARROW_WORK || MIMO_USE_EXT_MMSE4_LIVE_BUILD)
#error "MIMO_USE_EXT_MMSE4_STREAM_WORK_SOLVE is mutually exclusive with narrow/live build candidates"
#endif

#if MIMO_USE_EXT_ZF4_TRUE16B_WORK && \
    (!MIMO_USE_SITE4_SOLVE_SOA || !MIMO_USE_SITE4_HERMITIAN_UPPER_BUILD || \
     !MIMO_USE_SITE4_HERMITIAN_UPPER_ROWFUSE || !MIMO_USE_SITE4_PRELUDE_HOIST || \
     !MIMO_USE_SOLVE4_SCHUR || \
     (MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER != 3 && \
      MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER != 5) || \
     (MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 4 && \
      MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 5) || \
     MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT != 13 || \
     MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED || \
     MIMO_USE_SITE4_BUILD_SOLVE_PIPELINE2 || \
     MIMO_COLLECT_SCHUR_STATS)
#error "MIMO_USE_EXT_ZF4_TRUE16B_WORK is only valid for current ZF4 rowfuse/prelude Schur EXT-inner3/5 q15recip path"
#endif

#if MIMO_USE_EXT_ZF4_RVV_ROUND_ONCE_BUILD && \
    (!MIMO_USE_SITE4_ROUND_ONCE_DOT || !MIMO_USE_RVV_SITE4_MF2 || \
     !MIMO_USE_SITE4_HERMITIAN_UPPER_ROWFUSE || \
     MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED || MIMO_COLLECT_SCHUR_STATS)
#error "MIMO_USE_EXT_ZF4_RVV_ROUND_ONCE_BUILD is a local unpacked MF2 rowfuse round-once build experiment"
#endif

#if MIMO_USE_EXT_ZF4_PACK32_WORK && !MIMO_USE_EXT_ZF4_TRUE16B_WORK
#error "MIMO_USE_EXT_ZF4_PACK32_WORK requires MIMO_USE_EXT_ZF4_TRUE16B_WORK=1"
#endif

#if MIMO_USE_EXT_ZF4_VSCMACC_BUILD && \
    (!MIMO_USE_EXT_ZF4_TRUE16B_WORK || !MIMO_USE_EXT_ZF4_PACK32_WORK)
#error "MIMO_USE_EXT_ZF4_VSCMACC_BUILD requires TRUE16B_WORK=1 and PACK32_WORK=1"
#endif

#if MIMO_USE_EXT_ZF4_VSCMACC_FUSED_BUILD && \
    (!MIMO_USE_EXT_ZF4_VSCMACC_BUILD || \
     MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER != 5)
#error "MIMO_USE_EXT_ZF4_VSCMACC_FUSED_BUILD requires VSCMACC_BUILD=1 and EXT_INNER=5"
#endif

#if MIMO_USE_EXT_ZF4_VSCMACC_SEED_BUILD && \
    !MIMO_USE_EXT_ZF4_VSCMACC_FUSED_BUILD
#error "MIMO_USE_EXT_ZF4_VSCMACC_SEED_BUILD requires fused VSCMACC build"
#endif

#if MIMO_USE_EXT_ZF4_INLINE_UPPER_I16_SOLVE && \
    (!MIMO_USE_EXT_ZF4_TRUE16B_WORK || !MIMO_USE_EXT_ZF4_PACK32_WORK || \
     !MIMO_USE_EXT_ZF4_VSCMACC_BUILD || \
     !MIMO_USE_EXT_ZF4_VSCMACC_FUSED_BUILD || \
     MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER != 5 || \
     MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 5 || \
     MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT != 13 || \
     MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE || \
     MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_BSOLVE || \
     MIMO_USE_EXT_ZF4_SCALAR_X23_STORE)
#error "MIMO_USE_EXT_ZF4_INLINE_UPPER_I16_SOLVE only supports current true16b/pack32/fused EXT_INNER=5 mode5 path"
#endif

#if MIMO_USE_EXT_MMSE4_TRUE16B_INLINE_SOLVE && \
    (!MIMO_USE_EXT_ZF4_TRUE16B_WORK || !MIMO_USE_EXT_ZF4_PACK32_WORK || \
     !MIMO_USE_EXT_ZF4_VSCMACC_BUILD || \
     !MIMO_USE_EXT_ZF4_VSCMACC_FUSED_BUILD || \
     !MIMO_USE_EXT_ZF4_INLINE_UPPER_I16_SOLVE || \
     MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER != 5 || \
     MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 5 || \
     MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT != 13 || \
     !MIMO_USE_SITE4_HERMITIAN_UPPER_MMSE || \
     MIMO_USE_EXT_MMSE4_SCHUR_RVV_SOLVE || \
     MIMO_USE_EXT_ZF4_LIVE_TAIL_SCHUR || \
     MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE || \
     MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_BSOLVE || \
     MIMO_USE_EXT_ZF4_SCALAR_X23_STORE)
#error "MIMO_USE_EXT_MMSE4_TRUE16B_INLINE_SOLVE requires true16b/pack32/VSCMACC fused inline mode5 with MMSE upper build, no live-tail, and no RVV solve"
#endif

#if MIMO_USE_EXT_MMSE4_TRUE16B_LIVE_TAIL_SOLVE && \
    (!MIMO_USE_EXT_ZF4_TRUE16B_WORK || !MIMO_USE_EXT_ZF4_PACK32_WORK || \
     !MIMO_USE_EXT_ZF4_VSCMACC_BUILD || \
     !MIMO_USE_EXT_ZF4_VSCMACC_FUSED_BUILD || \
     !MIMO_USE_EXT_ZF4_INLINE_UPPER_I16_SOLVE || \
     MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER != 5 || \
     MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 5 || \
     MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT != 13 || \
     !MIMO_USE_SITE4_HERMITIAN_UPPER_MMSE || \
     MIMO_USE_EXT_MMSE4_TRUE16B_INLINE_SOLVE || \
     MIMO_USE_EXT_MMSE4_SCHUR_RVV_SOLVE || \
     MIMO_USE_EXT_ZF4_LIVE_TAIL_SCHUR || \
     MIMO_USE_EXT_ZF4_TAIL_SCRATCH_REUSE || \
     MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE || \
     MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_BSOLVE || \
     MIMO_USE_EXT_ZF4_SCALAR_X23_STORE)
#error "MIMO_USE_EXT_MMSE4_TRUE16B_LIVE_TAIL_SOLVE requires MMSE true16b/pack32/VSCMACC fused inline live-tail mode5, with dedicated lambda injection and no RVV solve"
#endif

#if MIMO_USE_EXT_ZF4_LIVE_TAIL_SCHUR && \
    (!MIMO_USE_EXT_ZF4_INLINE_UPPER_I16_SOLVE || \
     !MIMO_USE_EXT_ZF4_TRUE16B_WORK || !MIMO_USE_EXT_ZF4_PACK32_WORK || \
     !MIMO_USE_EXT_ZF4_VSCMACC_BUILD || \
     !MIMO_USE_EXT_ZF4_VSCMACC_FUSED_BUILD || \
     MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER != 5 || \
     MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 5 || \
     MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT != 13 || \
     MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE || \
     MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_BSOLVE || \
     MIMO_USE_EXT_ZF4_SCALAR_X23_STORE)
#error "MIMO_USE_EXT_ZF4_LIVE_TAIL_SCHUR is only valid for the inline current true16b/pack32/fused EXT_INNER=5 path"
#endif

#if MIMO_USE_EXT_ZF4_TOP2_MACC_SUM && \
    (!MIMO_USE_EXT_ZF4_INLINE_UPPER_I16_SOLVE || \
     !MIMO_USE_SOLVE4_SCHUR_ZF_EXT_MACC_INNER)
#error "MIMO_USE_EXT_ZF4_TOP2_MACC_SUM requires inline EXT Schur solve with VSCMACC inner enabled"
#endif

#if MIMO_USE_EXT_ZF4_TAIL_SCRATCH_REUSE && \
    (!MIMO_USE_EXT_ZF4_INLINE_UPPER_I16_SOLVE || \
     !MIMO_USE_EXT_ZF4_LIVE_TAIL_SCHUR || \
     !MIMO_USE_EXT_ZF4_TRUE16B_WORK || !MIMO_USE_EXT_ZF4_PACK32_WORK || \
     !MIMO_USE_EXT_ZF4_VSCMACC_BUILD || \
     !MIMO_USE_EXT_ZF4_VSCMACC_FUSED_BUILD || \
     MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER != 5 || \
     MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 5 || \
     MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT != 13 || \
     MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE || \
     MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_BSOLVE || \
     MIMO_USE_EXT_ZF4_SCALAR_X23_STORE)
#error "MIMO_USE_EXT_ZF4_TAIL_SCRATCH_REUSE is only valid for the inline live-tail current-best EXT path"
#endif

#if MIMO_USE_EXT_ZF4_INLINE_HEAD_WORK_REUSE && \
    (!MIMO_USE_EXT_ZF4_INLINE_UPPER_I16_SOLVE || \
     !MIMO_USE_EXT_ZF4_LIVE_TAIL_SCHUR || \
     MIMO_USE_EXT_ZF4_TAIL_SCRATCH_REUSE || \
     !MIMO_USE_EXT_ZF4_TRUE16B_WORK || !MIMO_USE_EXT_ZF4_PACK32_WORK || \
     !MIMO_USE_EXT_ZF4_VSCMACC_BUILD || \
     !MIMO_USE_EXT_ZF4_VSCMACC_FUSED_BUILD || \
     MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER != 5 || \
     MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 5 || \
     MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT != 13 || \
     MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE || \
     MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_BSOLVE || \
     MIMO_USE_EXT_ZF4_SCALAR_X23_STORE)
#error "MIMO_USE_EXT_ZF4_INLINE_HEAD_WORK_REUSE is only valid for the inline live-tail current-best EXT path without tail scratch"
#endif

#if MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE && \
    (!MIMO_USE_EXT_ZF4_TRUE16B_WORK || !MIMO_USE_EXT_ZF4_PACK32_WORK || \
     MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER != 5 || \
     MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 5 || \
     MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT != 13)
#error "MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE requires true16b+pack32 Schur inner5 q15recip mode5 shift=13"
#endif

#if MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SPLIT_SOLVE && \
    !MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE
#error "MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SPLIT_SOLVE requires MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE=1"
#endif

#if MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SPLIT_SOLVE < 0 || \
    MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SPLIT_SOLVE > 3
#error "MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SPLIT_SOLVE supports 0=off, 1=staged RVV, 2=staged RVV+EXT Schur/top, 3=staged RVV+EXT Schur+RVV top"
#endif

#if MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_STORE && \
    !MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE
#error "MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_STORE requires MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE=1"
#endif

#if MIMO_USE_EXT_MMSE4_RVV_I32_VEC_LAMBDA && \
    !MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE
#error "MIMO_USE_EXT_MMSE4_RVV_I32_VEC_LAMBDA requires TRUE16B_RVV_I32_SOLVE=1"
#endif

#if MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PRESERVE_PRELUDE && \
    (!MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE || !MIMO_USE_SITE4_PRELUDE_HOIST)
#error "MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PRESERVE_PRELUDE requires TRUE16B_RVV_I32_SOLVE=1 and SITE4_PRELUDE_HOIST=1"
#endif

#if MIMO_USE_SITE4_HERMITIAN_UPPER_MMSE && \
    (!MIMO_USE_SITE4_SOLVE_SOA || !MIMO_USE_SOLVE4_SCHUR || \
     !MIMO_USE_SITE4_HERMITIAN_UPPER_BUILD)
#error "MIMO_USE_SITE4_HERMITIAN_UPPER_MMSE requires SoA Schur upper-build path"
#endif

#if MIMO_USE_SITE4_HERMITIAN_UPPER_ROWFUSE && \
    (!MIMO_USE_SITE4_HERMITIAN_UPPER_BUILD || \
     MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED || MIMO_COLLECT_SCHUR_STATS)
#error "MIMO_USE_SITE4_HERMITIAN_UPPER_ROWFUSE requires unpacked upper-build without Schur stats"
#endif

#if MIMO_USE_SITE4_HERMITIAN_UPPER_ROWPAIR && \
    (!MIMO_USE_SITE4_HERMITIAN_UPPER_BUILD || \
     MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED || MIMO_COLLECT_SCHUR_STATS)
#error "MIMO_USE_SITE4_HERMITIAN_UPPER_ROWPAIR requires unpacked upper-build without Schur stats"
#endif

#if MIMO_USE_SITE4_HERMITIAN_UPPER_ROWPAIR && \
    MIMO_USE_SITE4_HERMITIAN_UPPER_ROWFUSE
#error "MIMO_USE_SITE4_HERMITIAN_UPPER_ROWPAIR and ROWFUSE are mutually exclusive"
#endif

#if MIMO_USE_SITE4_PRELUDE_HOIST && \
    (MIMO_USE_EXT_ZF4_RVV_NODIV_SOLVE || \
     MIMO_USE_EXT_ZF4_RVV_NODIV_FAST_SOLVE || \
     MIMO_USE_EXT_ZF4_SCHUR_RVV_SOLVE || \
     MIMO_USE_EXT_MMSE4_SCHUR_RVV_SOLVE || \
     MIMO_USE_EXT_ZF4_UPPER_RVV_SOLVE || \
     MIMO_USE_EXT_ZF4_MIXED_UPDATE_SOLVE || MIMO_USE_RVV_SOLVE4)
#error "MIMO_USE_SITE4_PRELUDE_HOIST is only valid when the site4 solve path does not use RVV"
#endif

#if MIMO_USE_SITE4_BUILD_SOLVE_PIPELINE2 && \
    (!MIMO_USE_SITE4_SOLVE_SOA || !MIMO_USE_SOLVE4_HERMITIAN || \
     !MIMO_USE_SOLVE4_SCHUR || MIMO_USE_SOLVE4_SCHUR_Q15_RECIP != 4 || \
     MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT != 13 || \
     !MIMO_USE_SITE4_HERMITIAN_UPPER_BUILD || \
     !MIMO_USE_SITE4_HERMITIAN_UPPER_ROWFUSE || \
     !MIMO_USE_SITE4_PRELUDE_HOIST || \
     MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED || MIMO_COLLECT_SCHUR_STATS || \
     MIMO_USE_EXT_ZF4_RVV_NODIV_SOLVE || \
     MIMO_USE_EXT_ZF4_RVV_NODIV_FAST_SOLVE || \
     MIMO_USE_EXT_ZF4_SCHUR_RVV_SOLVE || \
     MIMO_USE_EXT_ZF4_UPPER_RVV_SOLVE || \
     MIMO_USE_EXT_ZF4_MIXED_UPDATE_SOLVE || MIMO_USE_RVV_SOLVE4)
#error "MIMO_USE_SITE4_BUILD_SOLVE_PIPELINE2 is only valid for the current ZF4 rowfuse/prelude Schur q15recip mode4 no-RVV path"
#endif

static MIMO_ALWAYS_INLINE const mimo_c16_t *mimo_ext_site4_h4_vec(
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

static MIMO_ALWAYS_INLINE const mimo_c16_t *mimo_ext_site4_y4_vec(
    const mimo_c16_t *y_site4,
    size_t block,
    int rx)
{
    const size_t idx =
        (block * MIMO_N4 + (size_t)rx) * MIMO_SITE4_LANES;
    return &y_site4[idx];
}

static MIMO_ALWAYS_INLINE void mimo_ext_store_zero4_site4(
    mimo_c16_t *x_block)
{
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        for (int tx = 0; tx < MIMO_N4; ++tx) {
            x_block[lane * MIMO_N4 + tx] = mimo_make_c16(0, 0);
        }
    }
}

#define MIMO_EXT_SITE4_PRELUDE_ASM \
    "vsetivli zero, 8, e16, m1, ta, ma\n\t" \
    "vid.v v28\n\t" \
    "vsll.vi v28, v28, 1\n\t" \
    "vadd.vi v30, v28, 1\n\t" \
    "vmsltu.vi v0, v28, 8\n\t"

static MIMO_ALWAYS_INLINE void mimo_ext_site4_prelude_asm(void)
{
    asm volatile(MIMO_EXT_SITE4_PRELUDE_ASM ::: "memory");
}

#define MIMO_EXT_SITE4_PRELUDE_STATE_BYTES 48

static MIMO_ALWAYS_INLINE void
mimo_ext_site4_save_prelude_state(void *state)
{
    asm volatile(
        "vs1r.v v0, (%[state])\n\t"
        "addi t0, %[state], 16\n\t"
        "vs1r.v v28, (t0)\n\t"
        "addi t0, %[state], 32\n\t"
        "vs1r.v v30, (t0)\n\t"
        :
        : [state] "r"(state)
        : "t0", "memory");
}

static MIMO_ALWAYS_INLINE void
mimo_ext_site4_restore_prelude_state(void *state)
{
    asm volatile(
        "vl1re8.v v0, (%[state])\n\t"
        "addi t0, %[state], 16\n\t"
        "vl1re8.v v28, (t0)\n\t"
        "addi t0, %[state], 32\n\t"
        "vl1re8.v v30, (t0)\n\t"
        "vsetivli zero, 8, e16, m1, ta, ma\n\t"
        :
        : [state] "r"(state)
        : "t0", "memory");
}

#define MIMO_EXT_SITE4_ACCUM_ASM(STORES) \
    "vmv.v.i v8, 0\n\t" \
    "vmv.v.i v9, 0\n\t" \
    "vmv.v.i v10, 0\n\t" \
    "vmv.v.i v11, 0\n\t" \
    "li t0, 4\n\t" \
    "1:\n\t" \
    "vle16.v v20, (%[pa])\n\t" \
    "vle16.v v24, (%[pb])\n\t" \
    ".word 0xCF4C0857\n\t" \
    "vrgather.vv v18, v16, v28\n\t" \
    "vrgather.vv v19, v16, v30\n\t" \
    "vwadd.wv v8, v8, v18\n\t" \
    "vwadd.wv v10, v10, v19\n\t" \
    "addi %[pa], %[pa], 16\n\t" \
    "addi %[pb], %[pb], 16\n\t" \
    "addi t0, t0, -1\n\t" \
    "bnez t0, 1b\n\t" \
        STORES

#define MIMO_EXT_SITE4_ACCUM_REAL_ASM(STORES) \
    "vmv.v.i v8, 0\n\t" \
    "li t0, 4\n\t" \
    "1:\n\t" \
    "vle16.v v20, (%[pa])\n\t" \
    "vle16.v v24, (%[pb])\n\t" \
    ".word 0xCF4C0857\n\t" \
    "vrgather.vv v18, v16, v28\n\t" \
    "vwadd.wv v8, v8, v18\n\t" \
    "addi %[pa], %[pa], 16\n\t" \
    "addi %[pb], %[pb], 16\n\t" \
    "addi t0, t0, -1\n\t" \
    "bnez t0, 1b\n\t" \
    STORES

#define MIMO_EXT_SITE4_ACCUM_NEG_IM_ASM(STORES) \
    "vmv.v.i v8, 0\n\t" \
    "vmv.v.i v9, 0\n\t" \
    "vmv.v.i v10, 0\n\t" \
    "vmv.v.i v11, 0\n\t" \
    "vmv.v.i v12, 0\n\t" \
    "vmv.v.i v13, 0\n\t" \
    "li t0, 4\n\t" \
    "1:\n\t" \
    "vle16.v v20, (%[pa])\n\t" \
    "vle16.v v24, (%[pb])\n\t" \
    ".word 0xCF4C0857\n\t" \
    "vrgather.vv v18, v16, v28\n\t" \
    "vrgather.vv v19, v16, v30\n\t" \
    "vwadd.wv v8, v8, v18\n\t" \
    "vwadd.wv v10, v10, v19\n\t" \
    "vwsub.wv v12, v12, v19\n\t" \
    "addi %[pa], %[pa], 16\n\t" \
    "addi %[pb], %[pb], 16\n\t" \
    "addi t0, t0, -1\n\t" \
    "bnez t0, 1b\n\t" \
    STORES

static MIMO_ALWAYS_INLINE void mimo_ext_dot4_sites_store_real_q15(
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
    const mimo_c16_t *pa = mimo_ext_site4_h4_vec(h_site4, block, tx, 0);
    const mimo_c16_t *pb = rhs_is_h
                               ? mimo_ext_site4_h4_vec(rhs_site4, block,
                                                       rhs_tx, 0)
                               : mimo_ext_site4_y4_vec(rhs_site4, block, 0);

    if (re_dst1 == (int32_t *)0) {
        asm volatile(
            MIMO_EXT_SITE4_ACCUM_REAL_ASM(
                "vsse32.v v8, (%[re0]), %[rs0], v0.t\n\t")
            : [pa] "+r"(pa), [pb] "+r"(pb)
            : [re0] "r"(re_dst0), [rs0] "r"(re_stride0)
            : "t0", "memory");
        return;
    }

    asm volatile(
        MIMO_EXT_SITE4_ACCUM_REAL_ASM(
            "vsse32.v v8, (%[re0]), %[rs0], v0.t\n\t"
            "vsse32.v v8, (%[re1]), %[rs1], v0.t\n\t")
        : [pa] "+r"(pa), [pb] "+r"(pb)
        : [re0] "r"(re_dst0), [rs0] "r"(re_stride0),
          [re1] "r"(re_dst1), [rs1] "r"(re_stride1)
        : "t0", "memory");
}

static MIMO_ALWAYS_INLINE void mimo_ext_dot4_sites_store_conj_q15(
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
    const mimo_c16_t *pa = mimo_ext_site4_h4_vec(h_site4, block, tx, 0);
    const mimo_c16_t *pb = rhs_is_h
                               ? mimo_ext_site4_h4_vec(rhs_site4, block,
                                                       rhs_tx, 0)
                               : mimo_ext_site4_y4_vec(rhs_site4, block, 0);

    if (re_dst1 == (int32_t *)0) {
        asm volatile(
            MIMO_EXT_SITE4_ACCUM_ASM(
                "vsse32.v v8, (%[re0]), %[rs0], v0.t\n\t"
                "vsse32.v v10, (%[im0]), %[is0], v0.t\n\t")
            : [pa] "+r"(pa), [pb] "+r"(pb)
            : [re0] "r"(re_dst0), [rs0] "r"(re_stride0),
              [im0] "r"(im_dst0), [is0] "r"(im_stride0)
            : "t0", "memory");
        return;
    }

    if (conj_dst1) {
        asm volatile(
            MIMO_EXT_SITE4_ACCUM_NEG_IM_ASM(
                "vsse32.v v8, (%[re0]), %[rs0], v0.t\n\t"
                "vsse32.v v10, (%[im0]), %[is0], v0.t\n\t"
                "vsse32.v v8, (%[re1]), %[rs1], v0.t\n\t"
                "vsse32.v v12, (%[im1]), %[is1], v0.t\n\t")
            : [pa] "+r"(pa), [pb] "+r"(pb)
            : [re0] "r"(re_dst0), [rs0] "r"(re_stride0),
              [im0] "r"(im_dst0), [is0] "r"(im_stride0),
              [re1] "r"(re_dst1), [rs1] "r"(re_stride1),
              [im1] "r"(im_dst1), [is1] "r"(im_stride1)
            : "t0", "memory");
        return;
    }

    asm volatile(
        MIMO_EXT_SITE4_ACCUM_ASM(
            "vsse32.v v8, (%[re0]), %[rs0], v0.t\n\t"
            "vsse32.v v10, (%[im0]), %[is0], v0.t\n\t"
            "vsse32.v v8, (%[re1]), %[rs1], v0.t\n\t"
            "vsse32.v v10, (%[im1]), %[is1], v0.t\n\t")
        : [pa] "+r"(pa), [pb] "+r"(pb)
        : [re0] "r"(re_dst0), [rs0] "r"(re_stride0),
          [im0] "r"(im_dst0), [is0] "r"(im_stride0),
          [re1] "r"(re_dst1), [rs1] "r"(re_stride1),
          [im1] "r"(im_dst1), [is1] "r"(im_stride1)
        : "t0", "memory");
}

static MIMO_ALWAYS_INLINE void mimo_ext_build_normal4_site4_q15(
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

    asm volatile(MIMO_EXT_SITE4_PRELUDE_ASM ::: "memory");

    for (int tx = 0; tx < MIMO_N4; ++tx) {
        mimo_ext_dot4_sites_store_conj_q15(h_site4, y_site4, block, tx, 0, 0,
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
                mimo_ext_dot4_sites_store_real_q15(
                    h_site4, h_site4, block, tx_i, tx_j, 1,
                    &a_lane[0][upper].r, a_stride,
                    (int32_t *)0, 0);
#else
                mimo_ext_dot4_sites_store_conj_q15(
                    h_site4, h_site4, block, tx_i, tx_j, 1,
                    &a_lane[0][upper].r, a_stride,
                    &a_lane[0][upper].i, a_stride,
                    (int32_t *)0, 0,
                    (int32_t *)0, 0, 0);
#endif
            } else {
                mimo_ext_dot4_sites_store_conj_q15(
                    h_site4, h_site4, block, tx_i, tx_j, 1,
                    &a_lane[0][upper].r, a_stride,
                    &a_lane[0][upper].i, a_stride,
                    &a_lane[0][lower].r, a_stride,
                    &a_lane[0][lower].i, a_stride, 1);
            }
        }
    }
}

static MIMO_ALWAYS_INLINE void mimo_ext_build_hermitian4_site4_upper_q15(
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

    asm volatile(MIMO_EXT_SITE4_PRELUDE_ASM ::: "memory");

    for (int tx = 0; tx < MIMO_N4; ++tx) {
        mimo_ext_dot4_sites_store_conj_q15(h_site4, y_site4, block, tx, 0, 0,
                                           &b_lane[0][tx].r, b_stride,
                                           &b_lane[0][tx].i, b_stride,
                                           (int32_t *)0, 0,
                                           (int32_t *)0, 0, 0);
    }

    for (int tx_i = 0; tx_i < MIMO_N4; ++tx_i) {
        for (int tx_j = tx_i; tx_j < MIMO_N4; ++tx_j) {
            const size_t upper = (size_t)(tx_i * MIMO_N4 + tx_j);

            if (tx_i == tx_j) {
                mimo_ext_dot4_sites_store_real_q15(
                    h_site4, h_site4, block, tx_i, tx_j, 1,
                    &a_lane[0][upper].r, a_stride,
                    (int32_t *)0, 0);
            } else {
                mimo_ext_dot4_sites_store_conj_q15(
                    h_site4, h_site4, block, tx_i, tx_j, 1,
                    &a_lane[0][upper].r, a_stride,
                    &a_lane[0][upper].i, a_stride,
                    (int32_t *)0, 0,
                    (int32_t *)0, 0, 0);
            }
        }
    }
}

static MIMO_ALWAYS_INLINE void mimo_ext_dot4_sites_store_conj_soa_q15(
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
    const mimo_c16_t *pa = mimo_ext_site4_h4_vec(h_site4, block, tx, 0);
    const mimo_c16_t *pb = rhs_is_h
                               ? mimo_ext_site4_h4_vec(rhs_site4, block,
                                                       rhs_tx, 0)
                               : mimo_ext_site4_y4_vec(rhs_site4, block, 0);

    if (re_dst1 == (int32_t *)0) {
        asm volatile(
            MIMO_EXT_SITE4_ACCUM_ASM(
                "vse32.v v8, (%[re0]), v0.t\n\t"
                "vse32.v v10, (%[im0]), v0.t\n\t")
            : [pa] "+r"(pa), [pb] "+r"(pb)
            : [re0] "r"(re_dst0), [im0] "r"(im_dst0)
            : "t0", "memory");
        return;
    }

    if (conj_dst1) {
        asm volatile(
            MIMO_EXT_SITE4_ACCUM_NEG_IM_ASM(
                "vse32.v v8, (%[re0]), v0.t\n\t"
                "vse32.v v10, (%[im0]), v0.t\n\t"
                "vse32.v v8, (%[re1]), v0.t\n\t"
                "vse32.v v12, (%[im1]), v0.t\n\t")
            : [pa] "+r"(pa), [pb] "+r"(pb)
            : [re0] "r"(re_dst0), [im0] "r"(im_dst0),
              [re1] "r"(re_dst1), [im1] "r"(im_dst1)
            : "t0", "memory");
        return;
    }

    asm volatile(
        MIMO_EXT_SITE4_ACCUM_ASM(
            "vse32.v v8, (%[re0]), v0.t\n\t"
            "vse32.v v10, (%[im0]), v0.t\n\t"
            "vse32.v v8, (%[re1]), v0.t\n\t"
            "vse32.v v10, (%[im1]), v0.t\n\t")
        : [pa] "+r"(pa), [pb] "+r"(pb)
        : [re0] "r"(re_dst0), [im0] "r"(im_dst0),
          [re1] "r"(re_dst1), [im1] "r"(im_dst1)
        : "t0", "memory");
}

static MIMO_ALWAYS_INLINE void mimo_ext_dot4_sites_store_real_soa_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *rhs_site4,
    size_t block,
    int tx,
    int rhs_tx,
    int rhs_is_h,
    int32_t *re_dst0,
    int32_t *re_dst1)
{
    const mimo_c16_t *pa = mimo_ext_site4_h4_vec(h_site4, block, tx, 0);
    const mimo_c16_t *pb = rhs_is_h
                               ? mimo_ext_site4_h4_vec(rhs_site4, block,
                                                       rhs_tx, 0)
                               : mimo_ext_site4_y4_vec(rhs_site4, block, 0);

    if (re_dst1 == (int32_t *)0) {
        asm volatile(
            MIMO_EXT_SITE4_ACCUM_REAL_ASM(
                "vse32.v v8, (%[re0]), v0.t\n\t")
            : [pa] "+r"(pa), [pb] "+r"(pb)
            : [re0] "r"(re_dst0)
            : "t0", "memory");
        return;
    }

    asm volatile(
        MIMO_EXT_SITE4_ACCUM_REAL_ASM(
            "vse32.v v8, (%[re0]), v0.t\n\t"
            "vse32.v v8, (%[re1]), v0.t\n\t")
        : [pa] "+r"(pa), [pb] "+r"(pb)
        : [re0] "r"(re_dst0), [re1] "r"(re_dst1)
        : "t0", "memory");
}

#if MIMO_USE_SITE4_HERMITIAN_UPPER_ROWFUSE || \
    MIMO_USE_SITE4_HERMITIAN_UPPER_ROWPAIR
static MIMO_ALWAYS_INLINE void mimo_ext_build_hermitian4_site4_upper_row0_soa_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_site4_soa4_t *work)
{
    const mimo_c16_t *p0 = mimo_ext_site4_h4_vec(h_site4, block, 0, 0);
    const mimo_c16_t *p1 = mimo_ext_site4_h4_vec(h_site4, block, 1, 0);
    const mimo_c16_t *p2 = mimo_ext_site4_h4_vec(h_site4, block, 2, 0);
    const mimo_c16_t *p3 = mimo_ext_site4_h4_vec(h_site4, block, 3, 0);
    const mimo_c16_t *py = mimo_ext_site4_y4_vec(y_site4, block, 0);

    asm volatile(
        "vmv.v.i v2, 0\n\t"
        "vmv.v.i v4, 0\n\t"
        "vmv.v.i v6, 0\n\t"
        "vmv.v.i v8, 0\n\t"
        "vmv.v.i v10, 0\n\t"
        "vmv.v.i v12, 0\n\t"
        "vmv.v.i v14, 0\n\t"
        "vmv.v.i v22, 0\n\t"
        "vmv.v.i v26, 0\n\t"
        "li t0, 4\n\t"
        "1:\n\t"
        "vle16.v v20, (%[p0])\n\t"
        "vle16.v v24, (%[py])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv v2, v2, v18\n\t"
        "vwadd.wv v4, v4, v19\n\t"
        "vmv.v.v v24, v20\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vwadd.wv v6, v6, v18\n\t"
        "vle16.v v24, (%[p1])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv v8, v8, v18\n\t"
        "vwadd.wv v10, v10, v19\n\t"
        "vle16.v v24, (%[p2])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv v12, v12, v18\n\t"
        "vwadd.wv v14, v14, v19\n\t"
        "vle16.v v24, (%[p3])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv v22, v22, v18\n\t"
        "vwadd.wv v26, v26, v19\n\t"
        "addi %[p0], %[p0], 16\n\t"
        "addi %[p1], %[p1], 16\n\t"
        "addi %[p2], %[p2], 16\n\t"
        "addi %[p3], %[p3], 16\n\t"
        "addi %[py], %[py], 16\n\t"
        "addi t0, t0, -1\n\t"
        "bnez t0, 1b\n\t"
        "vse32.v v2, (%[b0r]), v0.t\n\t"
        "vse32.v v4, (%[b0i]), v0.t\n\t"
        "vse32.v v6, (%[a00r]), v0.t\n\t"
        "vse32.v v8, (%[a01r]), v0.t\n\t"
        "vse32.v v10, (%[a01i]), v0.t\n\t"
        "vse32.v v12, (%[a02r]), v0.t\n\t"
        "vse32.v v14, (%[a02i]), v0.t\n\t"
        "vse32.v v22, (%[a03r]), v0.t\n\t"
        "vse32.v v26, (%[a03i]), v0.t\n\t"
        : [p0] "+r"(p0), [p1] "+r"(p1), [p2] "+r"(p2),
          [p3] "+r"(p3), [py] "+r"(py)
        : [b0r] "r"(&work->b_re[0][0]),
          [b0i] "r"(&work->b_im[0][0]),
          [a00r] "r"(&work->a_re[0][0]),
          [a01r] "r"(&work->a_re[1][0]),
          [a01i] "r"(&work->a_im[1][0]),
          [a02r] "r"(&work->a_re[2][0]),
          [a02i] "r"(&work->a_im[2][0]),
          [a03r] "r"(&work->a_re[3][0]),
          [a03i] "r"(&work->a_im[3][0])
        : "t0", "memory");
}

static MIMO_ALWAYS_INLINE void mimo_ext_build_hermitian4_site4_upper_row1_soa_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_site4_soa4_t *work)
{
    const mimo_c16_t *p1 = mimo_ext_site4_h4_vec(h_site4, block, 1, 0);
    const mimo_c16_t *p2 = mimo_ext_site4_h4_vec(h_site4, block, 2, 0);
    const mimo_c16_t *p3 = mimo_ext_site4_h4_vec(h_site4, block, 3, 0);
    const mimo_c16_t *py = mimo_ext_site4_y4_vec(y_site4, block, 0);

    asm volatile(
        "vmv.v.i v2, 0\n\t"
        "vmv.v.i v4, 0\n\t"
        "vmv.v.i v6, 0\n\t"
        "vmv.v.i v8, 0\n\t"
        "vmv.v.i v10, 0\n\t"
        "vmv.v.i v12, 0\n\t"
        "vmv.v.i v14, 0\n\t"
        "li t0, 4\n\t"
        "1:\n\t"
        "vle16.v v20, (%[p1])\n\t"
        "vle16.v v24, (%[py])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv v2, v2, v18\n\t"
        "vwadd.wv v4, v4, v19\n\t"
        "vmv.v.v v24, v20\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vwadd.wv v6, v6, v18\n\t"
        "vle16.v v24, (%[p2])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv v8, v8, v18\n\t"
        "vwadd.wv v10, v10, v19\n\t"
        "vle16.v v24, (%[p3])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv v12, v12, v18\n\t"
        "vwadd.wv v14, v14, v19\n\t"
        "addi %[p1], %[p1], 16\n\t"
        "addi %[p2], %[p2], 16\n\t"
        "addi %[p3], %[p3], 16\n\t"
        "addi %[py], %[py], 16\n\t"
        "addi t0, t0, -1\n\t"
        "bnez t0, 1b\n\t"
        "vse32.v v2, (%[b1r]), v0.t\n\t"
        "vse32.v v4, (%[b1i]), v0.t\n\t"
        "vse32.v v6, (%[a11r]), v0.t\n\t"
        "vse32.v v8, (%[a12r]), v0.t\n\t"
        "vse32.v v10, (%[a12i]), v0.t\n\t"
        "vse32.v v12, (%[a13r]), v0.t\n\t"
        "vse32.v v14, (%[a13i]), v0.t\n\t"
        : [p1] "+r"(p1), [p2] "+r"(p2), [p3] "+r"(p3),
          [py] "+r"(py)
        : [b1r] "r"(&work->b_re[1][0]),
          [b1i] "r"(&work->b_im[1][0]),
          [a11r] "r"(&work->a_re[5][0]),
          [a12r] "r"(&work->a_re[6][0]),
          [a12i] "r"(&work->a_im[6][0]),
          [a13r] "r"(&work->a_re[7][0]),
          [a13i] "r"(&work->a_im[7][0])
        : "t0", "memory");
}

static MIMO_ALWAYS_INLINE void mimo_ext_build_hermitian4_site4_upper_row2_soa_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_site4_soa4_t *work)
{
    const mimo_c16_t *p2 = mimo_ext_site4_h4_vec(h_site4, block, 2, 0);
    const mimo_c16_t *p3 = mimo_ext_site4_h4_vec(h_site4, block, 3, 0);
    const mimo_c16_t *py = mimo_ext_site4_y4_vec(y_site4, block, 0);

    asm volatile(
        "vmv.v.i v2, 0\n\t"
        "vmv.v.i v4, 0\n\t"
        "vmv.v.i v6, 0\n\t"
        "vmv.v.i v8, 0\n\t"
        "vmv.v.i v10, 0\n\t"
        "li t0, 4\n\t"
        "1:\n\t"
        "vle16.v v20, (%[p2])\n\t"
        "vle16.v v24, (%[py])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv v2, v2, v18\n\t"
        "vwadd.wv v4, v4, v19\n\t"
        "vmv.v.v v24, v20\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vwadd.wv v6, v6, v18\n\t"
        "vle16.v v24, (%[p3])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv v8, v8, v18\n\t"
        "vwadd.wv v10, v10, v19\n\t"
        "addi %[p2], %[p2], 16\n\t"
        "addi %[p3], %[p3], 16\n\t"
        "addi %[py], %[py], 16\n\t"
        "addi t0, t0, -1\n\t"
        "bnez t0, 1b\n\t"
        "vse32.v v2, (%[b2r]), v0.t\n\t"
        "vse32.v v4, (%[b2i]), v0.t\n\t"
        "vse32.v v6, (%[a22r]), v0.t\n\t"
        "vse32.v v8, (%[a23r]), v0.t\n\t"
        "vse32.v v10, (%[a23i]), v0.t\n\t"
        : [p2] "+r"(p2), [p3] "+r"(p3), [py] "+r"(py)
        : [b2r] "r"(&work->b_re[2][0]),
          [b2i] "r"(&work->b_im[2][0]),
          [a22r] "r"(&work->a_re[10][0]),
          [a23r] "r"(&work->a_re[11][0]),
          [a23i] "r"(&work->a_im[11][0])
        : "t0", "memory");
}

static MIMO_ALWAYS_INLINE void mimo_ext_build_hermitian4_site4_upper_row3_soa_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_site4_soa4_t *work)
{
    const mimo_c16_t *p3 = mimo_ext_site4_h4_vec(h_site4, block, 3, 0);
    const mimo_c16_t *py = mimo_ext_site4_y4_vec(y_site4, block, 0);

    asm volatile(
        "vmv.v.i v2, 0\n\t"
        "vmv.v.i v4, 0\n\t"
        "vmv.v.i v6, 0\n\t"
        "li t0, 4\n\t"
        "1:\n\t"
        "vle16.v v20, (%[p3])\n\t"
        "vle16.v v24, (%[py])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv v2, v2, v18\n\t"
        "vwadd.wv v4, v4, v19\n\t"
        "vmv.v.v v24, v20\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vwadd.wv v6, v6, v18\n\t"
        "addi %[p3], %[p3], 16\n\t"
        "addi %[py], %[py], 16\n\t"
        "addi t0, t0, -1\n\t"
        "bnez t0, 1b\n\t"
        "vse32.v v2, (%[b3r]), v0.t\n\t"
        "vse32.v v4, (%[b3i]), v0.t\n\t"
        "vse32.v v6, (%[a33r]), v0.t\n\t"
        : [p3] "+r"(p3), [py] "+r"(py)
        : [b3r] "r"(&work->b_re[3][0]),
          [b3i] "r"(&work->b_im[3][0]),
          [a33r] "r"(&work->a_re[15][0])
        : "t0", "memory");
}

static MIMO_ALWAYS_INLINE void mimo_ext_build_hermitian4_site4_upper_rowfuse_soa_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_site4_soa4_t *work)
{
    mimo_ext_build_hermitian4_site4_upper_row0_soa_q15(
        h_site4, y_site4, block, work);
    mimo_ext_build_hermitian4_site4_upper_row1_soa_q15(
        h_site4, y_site4, block, work);
    mimo_ext_build_hermitian4_site4_upper_row2_soa_q15(
        h_site4, y_site4, block, work);
    mimo_ext_build_hermitian4_site4_upper_row3_soa_q15(
        h_site4, y_site4, block, work);
}

#if MIMO_USE_EXT_MMSE4_FUSED_BUILD_SOLVE
static MIMO_ALWAYS_INLINE void mimo_ext_build_hermitian4_site4_upper_soa_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_site4_soa4_t *work);

#if MIMO_USE_EXT_MMSE4_LIVE_BUILD
static MIMO_ALWAYS_INLINE void
mimo_ext_build_hermitian4_site4_upper_row01_live_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    vint32m1_t *b0r_o,
    vint32m1_t *b0i_o,
    vint32m1_t *a00r_o,
    vint32m1_t *a01r_o,
    vint32m1_t *a01i_o,
    vint32m1_t *a02r_o,
    vint32m1_t *a02i_o,
    vint32m1_t *a03r_o,
    vint32m1_t *a03i_o,
    vint32m1_t *b1r_o,
    vint32m1_t *b1i_o,
    vint32m1_t *a11r_o,
    vint32m1_t *a12r_o,
    vint32m1_t *a12i_o,
    vint32m1_t *a13r_o,
    vint32m1_t *a13i_o)
{
    const mimo_c16_t *p0 = mimo_ext_site4_h4_vec(h_site4, block, 0, 0);
    const mimo_c16_t *p1 = mimo_ext_site4_h4_vec(h_site4, block, 1, 0);
    const mimo_c16_t *p2 = mimo_ext_site4_h4_vec(h_site4, block, 2, 0);
    const mimo_c16_t *p3 = mimo_ext_site4_h4_vec(h_site4, block, 3, 0);
    const mimo_c16_t *py = mimo_ext_site4_y4_vec(y_site4, block, 0);
    vint32m1_t b0r, b0i, a00r, a01r, a01i, a02r, a02i, a03r, a03i;
    vint32m1_t b1r, b1i, a11r, a12r, a12i, a13r, a13i;

    asm volatile(
        MIMO_EXT_SITE4_PRELUDE_ASM
        "vmv.v.i %[b0r], 0\n\t"
        "vmv.v.i %[b0i], 0\n\t"
        "vmv.v.i %[a00r], 0\n\t"
        "vmv.v.i %[a01r], 0\n\t"
        "vmv.v.i %[a01i], 0\n\t"
        "vmv.v.i %[a02r], 0\n\t"
        "vmv.v.i %[a02i], 0\n\t"
        "vmv.v.i %[a03r], 0\n\t"
        "vmv.v.i %[a03i], 0\n\t"
        "vmv.v.i %[b1r], 0\n\t"
        "vmv.v.i %[b1i], 0\n\t"
        "vmv.v.i %[a11r], 0\n\t"
        "vmv.v.i %[a12r], 0\n\t"
        "vmv.v.i %[a12i], 0\n\t"
        "vmv.v.i %[a13r], 0\n\t"
        "vmv.v.i %[a13i], 0\n\t"
        "li t0, 4\n\t"
        "1:\n\t"
        "vle16.v v20, (%[p0])\n\t"
        "vle16.v v24, (%[py])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv %[b0r], %[b0r], v18\n\t"
        "vwadd.wv %[b0i], %[b0i], v19\n\t"
        "vmv.v.v v24, v20\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vwadd.wv %[a00r], %[a00r], v18\n\t"
        "vle16.v v24, (%[p1])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv %[a01r], %[a01r], v18\n\t"
        "vwadd.wv %[a01i], %[a01i], v19\n\t"
        "vmv.v.v v20, v24\n\t"
        "vle16.v v24, (%[p2])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv %[a02r], %[a02r], v18\n\t"
        "vwadd.wv %[a02i], %[a02i], v19\n\t"
        "vle16.v v24, (%[p3])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv %[a03r], %[a03r], v18\n\t"
        "vwadd.wv %[a03i], %[a03i], v19\n\t"
        "vle16.v v24, (%[py])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv %[b1r], %[b1r], v18\n\t"
        "vwadd.wv %[b1i], %[b1i], v19\n\t"
        "vmv.v.v v24, v20\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vwadd.wv %[a11r], %[a11r], v18\n\t"
        "vle16.v v24, (%[p2])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv %[a12r], %[a12r], v18\n\t"
        "vwadd.wv %[a12i], %[a12i], v19\n\t"
        "vle16.v v24, (%[p3])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv %[a13r], %[a13r], v18\n\t"
        "vwadd.wv %[a13i], %[a13i], v19\n\t"
        "addi %[p0], %[p0], 16\n\t"
        "addi %[p1], %[p1], 16\n\t"
        "addi %[p2], %[p2], 16\n\t"
        "addi %[p3], %[p3], 16\n\t"
        "addi %[py], %[py], 16\n\t"
        "addi t0, t0, -1\n\t"
        "bnez t0, 1b\n\t"
        : [p0] "+r"(p0), [p1] "+r"(p1), [p2] "+r"(p2),
          [p3] "+r"(p3), [py] "+r"(py),
          [b0r] "=&vr"(b0r), [b0i] "=&vr"(b0i),
          [a00r] "=&vr"(a00r),
          [a01r] "=&vr"(a01r), [a01i] "=&vr"(a01i),
          [a02r] "=&vr"(a02r), [a02i] "=&vr"(a02i),
          [a03r] "=&vr"(a03r), [a03i] "=&vr"(a03i),
          [b1r] "=&vr"(b1r), [b1i] "=&vr"(b1i),
          [a11r] "=&vr"(a11r),
          [a12r] "=&vr"(a12r), [a12i] "=&vr"(a12i),
          [a13r] "=&vr"(a13r), [a13i] "=&vr"(a13i)
        :
        : "t0", "v16", "v18", "v19", "v20", "v24", "v28", "v30",
          "memory");

    *b0r_o = b0r;
    *b0i_o = b0i;
    *a00r_o = a00r;
    *a01r_o = a01r;
    *a01i_o = a01i;
    *a02r_o = a02r;
    *a02i_o = a02i;
    *a03r_o = a03r;
    *a03i_o = a03i;
    *b1r_o = b1r;
    *b1i_o = b1i;
    *a11r_o = a11r;
    *a12r_o = a12r;
    *a12i_o = a12i;
    *a13r_o = a13r;
    *a13i_o = a13i;
}

static MIMO_ALWAYS_INLINE void
mimo_ext_build_hermitian4_site4_upper_row23_live_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    vint32m1_t *b2r_o,
    vint32m1_t *b2i_o,
    vint32m1_t *a22r_o,
    vint32m1_t *a23r_o,
    vint32m1_t *a23i_o,
    vint32m1_t *b3r_o,
    vint32m1_t *b3i_o,
    vint32m1_t *a33r_o)
{
    const mimo_c16_t *p2 = mimo_ext_site4_h4_vec(h_site4, block, 2, 0);
    const mimo_c16_t *p3 = mimo_ext_site4_h4_vec(h_site4, block, 3, 0);
    const mimo_c16_t *py = mimo_ext_site4_y4_vec(y_site4, block, 0);
    vint32m1_t b2r, b2i, a22r, a23r, a23i, b3r, b3i, a33r;

    asm volatile(
        MIMO_EXT_SITE4_PRELUDE_ASM
        "vmv.v.i %[b2r], 0\n\t"
        "vmv.v.i %[b2i], 0\n\t"
        "vmv.v.i %[a22r], 0\n\t"
        "vmv.v.i %[a23r], 0\n\t"
        "vmv.v.i %[a23i], 0\n\t"
        "vmv.v.i %[b3r], 0\n\t"
        "vmv.v.i %[b3i], 0\n\t"
        "vmv.v.i %[a33r], 0\n\t"
        "li t0, 4\n\t"
        "1:\n\t"
        "vle16.v v20, (%[p2])\n\t"
        "vle16.v v24, (%[py])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv %[b2r], %[b2r], v18\n\t"
        "vwadd.wv %[b2i], %[b2i], v19\n\t"
        "vmv.v.v v24, v20\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vwadd.wv %[a22r], %[a22r], v18\n\t"
        "vle16.v v24, (%[p3])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv %[a23r], %[a23r], v18\n\t"
        "vwadd.wv %[a23i], %[a23i], v19\n\t"
        "vmv.v.v v20, v24\n\t"
        "vle16.v v24, (%[py])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv %[b3r], %[b3r], v18\n\t"
        "vwadd.wv %[b3i], %[b3i], v19\n\t"
        "vmv.v.v v24, v20\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vwadd.wv %[a33r], %[a33r], v18\n\t"
        "addi %[p2], %[p2], 16\n\t"
        "addi %[p3], %[p3], 16\n\t"
        "addi %[py], %[py], 16\n\t"
        "addi t0, t0, -1\n\t"
        "bnez t0, 1b\n\t"
        : [p2] "+r"(p2), [p3] "+r"(p3), [py] "+r"(py),
          [b2r] "=&vr"(b2r), [b2i] "=&vr"(b2i),
          [a22r] "=&vr"(a22r),
          [a23r] "=&vr"(a23r), [a23i] "=&vr"(a23i),
          [b3r] "=&vr"(b3r), [b3i] "=&vr"(b3i),
          [a33r] "=&vr"(a33r)
        :
        : "t0", "v16", "v18", "v19", "v20", "v24", "v28", "v30",
          "memory");

    *b2r_o = b2r;
    *b2i_o = b2i;
    *a22r_o = a22r;
    *a23r_o = a23r;
    *a23i_o = a23i;
    *b3r_o = b3r;
    *b3i_o = b3i;
    *a33r_o = a33r;
}
#endif

#if MIMO_USE_EXT_MMSE4_NARROW_WORK
static MIMO_ALWAYS_INLINE vint32m1_t
mimo_ext_load_narrow_i32(const int32_t *src, size_t vl)
{
    return __riscv_vle32_v_i32m1(src, vl);
}

static MIMO_ALWAYS_INLINE void
mimo_ext_build_hermitian4_site4_upper_row0_narrow_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_site4_upper_rhs_soa4_t *work)
{
    const mimo_c16_t *p0 = mimo_ext_site4_h4_vec(h_site4, block, 0, 0);
    const mimo_c16_t *p1 = mimo_ext_site4_h4_vec(h_site4, block, 1, 0);
    const mimo_c16_t *p2 = mimo_ext_site4_h4_vec(h_site4, block, 2, 0);
    const mimo_c16_t *p3 = mimo_ext_site4_h4_vec(h_site4, block, 3, 0);
    const mimo_c16_t *py = mimo_ext_site4_y4_vec(y_site4, block, 0);

    asm volatile(
        "vmv.v.i v2, 0\n\t"
        "vmv.v.i v4, 0\n\t"
        "vmv.v.i v6, 0\n\t"
        "vmv.v.i v8, 0\n\t"
        "vmv.v.i v10, 0\n\t"
        "vmv.v.i v12, 0\n\t"
        "vmv.v.i v14, 0\n\t"
        "vmv.v.i v22, 0\n\t"
        "vmv.v.i v26, 0\n\t"
        "li t0, 4\n\t"
        "1:\n\t"
        "vle16.v v20, (%[p0])\n\t"
        "vle16.v v24, (%[py])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv v2, v2, v18\n\t"
        "vwadd.wv v4, v4, v19\n\t"
        "vmv.v.v v24, v20\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vwadd.wv v6, v6, v18\n\t"
        "vle16.v v24, (%[p1])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv v8, v8, v18\n\t"
        "vwadd.wv v10, v10, v19\n\t"
        "vle16.v v24, (%[p2])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv v12, v12, v18\n\t"
        "vwadd.wv v14, v14, v19\n\t"
        "vle16.v v24, (%[p3])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv v22, v22, v18\n\t"
        "vwadd.wv v26, v26, v19\n\t"
        "addi %[p0], %[p0], 16\n\t"
        "addi %[p1], %[p1], 16\n\t"
        "addi %[p2], %[p2], 16\n\t"
        "addi %[p3], %[p3], 16\n\t"
        "addi %[py], %[py], 16\n\t"
        "addi t0, t0, -1\n\t"
        "bnez t0, 1b\n\t"
        "vse32.v v2, (%[b0r]), v0.t\n\t"
        "vse32.v v4, (%[b0i]), v0.t\n\t"
        "vse32.v v6, (%[a00r]), v0.t\n\t"
        "vse32.v v8, (%[a01r]), v0.t\n\t"
        "vse32.v v10, (%[a01i]), v0.t\n\t"
        "vse32.v v12, (%[a02r]), v0.t\n\t"
        "vse32.v v14, (%[a02i]), v0.t\n\t"
        "vse32.v v22, (%[a03r]), v0.t\n\t"
        "vse32.v v26, (%[a03i]), v0.t\n\t"
        : [p0] "+r"(p0), [p1] "+r"(p1), [p2] "+r"(p2),
          [p3] "+r"(p3), [py] "+r"(py)
        : [b0r] "r"(&work->b0r[0]),
          [b0i] "r"(&work->b0i[0]),
          [a00r] "r"(&work->a00r[0]),
          [a01r] "r"(&work->a01r[0]),
          [a01i] "r"(&work->a01i[0]),
          [a02r] "r"(&work->a02r[0]),
          [a02i] "r"(&work->a02i[0]),
          [a03r] "r"(&work->a03r[0]),
          [a03i] "r"(&work->a03i[0])
        : "t0", "memory");
}

static MIMO_ALWAYS_INLINE void
mimo_ext_build_hermitian4_site4_upper_row1_narrow_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_site4_upper_rhs_soa4_t *work)
{
    const mimo_c16_t *p1 = mimo_ext_site4_h4_vec(h_site4, block, 1, 0);
    const mimo_c16_t *p2 = mimo_ext_site4_h4_vec(h_site4, block, 2, 0);
    const mimo_c16_t *p3 = mimo_ext_site4_h4_vec(h_site4, block, 3, 0);
    const mimo_c16_t *py = mimo_ext_site4_y4_vec(y_site4, block, 0);

    asm volatile(
        "vmv.v.i v2, 0\n\t"
        "vmv.v.i v4, 0\n\t"
        "vmv.v.i v6, 0\n\t"
        "vmv.v.i v8, 0\n\t"
        "vmv.v.i v10, 0\n\t"
        "vmv.v.i v12, 0\n\t"
        "vmv.v.i v14, 0\n\t"
        "li t0, 4\n\t"
        "1:\n\t"
        "vle16.v v20, (%[p1])\n\t"
        "vle16.v v24, (%[py])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv v2, v2, v18\n\t"
        "vwadd.wv v4, v4, v19\n\t"
        "vmv.v.v v24, v20\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vwadd.wv v6, v6, v18\n\t"
        "vle16.v v24, (%[p2])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv v8, v8, v18\n\t"
        "vwadd.wv v10, v10, v19\n\t"
        "vle16.v v24, (%[p3])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv v12, v12, v18\n\t"
        "vwadd.wv v14, v14, v19\n\t"
        "addi %[p1], %[p1], 16\n\t"
        "addi %[p2], %[p2], 16\n\t"
        "addi %[p3], %[p3], 16\n\t"
        "addi %[py], %[py], 16\n\t"
        "addi t0, t0, -1\n\t"
        "bnez t0, 1b\n\t"
        "vse32.v v2, (%[b1r]), v0.t\n\t"
        "vse32.v v4, (%[b1i]), v0.t\n\t"
        "vse32.v v6, (%[a11r]), v0.t\n\t"
        "vse32.v v8, (%[a12r]), v0.t\n\t"
        "vse32.v v10, (%[a12i]), v0.t\n\t"
        "vse32.v v12, (%[a13r]), v0.t\n\t"
        "vse32.v v14, (%[a13i]), v0.t\n\t"
        : [p1] "+r"(p1), [p2] "+r"(p2), [p3] "+r"(p3),
          [py] "+r"(py)
        : [b1r] "r"(&work->b1r[0]),
          [b1i] "r"(&work->b1i[0]),
          [a11r] "r"(&work->a11r[0]),
          [a12r] "r"(&work->a12r[0]),
          [a12i] "r"(&work->a12i[0]),
          [a13r] "r"(&work->a13r[0]),
          [a13i] "r"(&work->a13i[0])
        : "t0", "memory");
}

static MIMO_ALWAYS_INLINE void
mimo_ext_build_hermitian4_site4_upper_row2_narrow_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_site4_upper_rhs_soa4_t *work)
{
    const mimo_c16_t *p2 = mimo_ext_site4_h4_vec(h_site4, block, 2, 0);
    const mimo_c16_t *p3 = mimo_ext_site4_h4_vec(h_site4, block, 3, 0);
    const mimo_c16_t *py = mimo_ext_site4_y4_vec(y_site4, block, 0);

    asm volatile(
        "vmv.v.i v2, 0\n\t"
        "vmv.v.i v4, 0\n\t"
        "vmv.v.i v6, 0\n\t"
        "vmv.v.i v8, 0\n\t"
        "vmv.v.i v10, 0\n\t"
        "li t0, 4\n\t"
        "1:\n\t"
        "vle16.v v20, (%[p2])\n\t"
        "vle16.v v24, (%[py])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv v2, v2, v18\n\t"
        "vwadd.wv v4, v4, v19\n\t"
        "vmv.v.v v24, v20\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vwadd.wv v6, v6, v18\n\t"
        "vle16.v v24, (%[p3])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv v8, v8, v18\n\t"
        "vwadd.wv v10, v10, v19\n\t"
        "addi %[p2], %[p2], 16\n\t"
        "addi %[p3], %[p3], 16\n\t"
        "addi %[py], %[py], 16\n\t"
        "addi t0, t0, -1\n\t"
        "bnez t0, 1b\n\t"
        "vse32.v v2, (%[b2r]), v0.t\n\t"
        "vse32.v v4, (%[b2i]), v0.t\n\t"
        "vse32.v v6, (%[a22r]), v0.t\n\t"
        "vse32.v v8, (%[a23r]), v0.t\n\t"
        "vse32.v v10, (%[a23i]), v0.t\n\t"
        : [p2] "+r"(p2), [p3] "+r"(p3), [py] "+r"(py)
        : [b2r] "r"(&work->b2r[0]),
          [b2i] "r"(&work->b2i[0]),
          [a22r] "r"(&work->a22r[0]),
          [a23r] "r"(&work->a23r[0]),
          [a23i] "r"(&work->a23i[0])
        : "t0", "memory");
}

static MIMO_ALWAYS_INLINE void
mimo_ext_build_hermitian4_site4_upper_row3_narrow_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_site4_upper_rhs_soa4_t *work)
{
    const mimo_c16_t *p3 = mimo_ext_site4_h4_vec(h_site4, block, 3, 0);
    const mimo_c16_t *py = mimo_ext_site4_y4_vec(y_site4, block, 0);

    asm volatile(
        "vmv.v.i v2, 0\n\t"
        "vmv.v.i v4, 0\n\t"
        "vmv.v.i v6, 0\n\t"
        "li t0, 4\n\t"
        "1:\n\t"
        "vle16.v v20, (%[p3])\n\t"
        "vle16.v v24, (%[py])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv v2, v2, v18\n\t"
        "vwadd.wv v4, v4, v19\n\t"
        "vmv.v.v v24, v20\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vwadd.wv v6, v6, v18\n\t"
        "addi %[p3], %[p3], 16\n\t"
        "addi %[py], %[py], 16\n\t"
        "addi t0, t0, -1\n\t"
        "bnez t0, 1b\n\t"
        "vse32.v v2, (%[b3r]), v0.t\n\t"
        "vse32.v v4, (%[b3i]), v0.t\n\t"
        "vse32.v v6, (%[a33r]), v0.t\n\t"
        : [p3] "+r"(p3), [py] "+r"(py)
        : [b3r] "r"(&work->b3r[0]),
          [b3i] "r"(&work->b3i[0]),
          [a33r] "r"(&work->a33r[0])
        : "t0", "memory");
}

static MIMO_ALWAYS_INLINE void
mimo_ext_build_hermitian4_site4_upper_narrow_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_site4_upper_rhs_soa4_t *work)
{
#if !MIMO_USE_SITE4_PRELUDE_HOIST
    mimo_ext_site4_prelude_asm();
#endif
    mimo_ext_build_hermitian4_site4_upper_row0_narrow_q15(
        h_site4, y_site4, block, work);
    mimo_ext_build_hermitian4_site4_upper_row1_narrow_q15(
        h_site4, y_site4, block, work);
    mimo_ext_build_hermitian4_site4_upper_row2_narrow_q15(
        h_site4, y_site4, block, work);
    mimo_ext_build_hermitian4_site4_upper_row3_narrow_q15(
        h_site4, y_site4, block, work);
}
#endif

#if MIMO_USE_EXT_MMSE4_STREAM_WORK_SOLVE
static MIMO_ALWAYS_INLINE void
mimo_ext_mmse4_schur_rvv_solve_stream_work_q15(
    const mimo_site4_soa4_t *restrict work,
    mimo_c16_t *restrict x_block)
{
    const size_t vl = __riscv_vsetvl_e32m1(MIMO_SITE4_LANES);
    const ptrdiff_t x_stride = (ptrdiff_t)(MIMO_N4 * sizeof(mimo_c16_t));

    const vint32m1_t a00r = __riscv_vadd_vx_i32m1(
        mimo_rvv_solve4_load_soa_a_re(work, 0, vl), MIMO_MMSE_LAMBDA_Q15, vl);
    const vint32m1_t a01r = mimo_rvv_solve4_load_soa_a_re(work, 1, vl);
    const vint32m1_t a01i = mimo_rvv_solve4_load_soa_a_im(work, 1, vl);
    const vint32m1_t a11r = __riscv_vadd_vx_i32m1(
        mimo_rvv_solve4_load_soa_a_re(work, 5, vl), MIMO_MMSE_LAMBDA_Q15, vl);

    const vint32m1_t b_det_q30 =
        mimo_rvv_schur4_det_q30_i32(a00r, a01r, a01i, a11r, vl);
    const vint32m1_t b_scale =
        mimo_rvv_schur4_scale_from_det_q30_i32(b_det_q30, vl);

    vint32m1_t y0r, y0i, y1r, y1i;
    mimo_rvv_schur4_solve2_i32(
        a00r, a01r, a01i, a11r, b_scale,
        mimo_rvv_solve4_load_soa_b_re(work, 0, vl),
        mimo_rvv_solve4_load_soa_b_im(work, 0, vl),
        mimo_rvv_solve4_load_soa_b_re(work, 1, vl),
        mimo_rvv_solve4_load_soa_b_im(work, 1, vl),
        &y0r, &y0i, &y1r, &y1i,
        MIMO_MMSE4_SCHUR_SCALE_ARG(1), vl);

    const vint32m1_t a02r = mimo_rvv_solve4_load_soa_a_re(work, 2, vl);
    const vint32m1_t a02i = mimo_rvv_solve4_load_soa_a_im(work, 2, vl);
    const vint32m1_t a12r = mimo_rvv_solve4_load_soa_a_re(work, 6, vl);
    const vint32m1_t a12i = mimo_rvv_solve4_load_soa_a_im(work, 6, vl);

    vint32m1_t w00r, w00i, w10r, w10i;
    mimo_rvv_schur4_solve2_i32(
        a00r, a01r, a01i, a11r, b_scale,
        a02r, a02i, a12r, a12i, &w00r, &w00i, &w10r, &w10i,
        MIMO_MMSE4_SCHUR_SCALE_ARG(1), vl);

    const vint32m1_t a03r = mimo_rvv_solve4_load_soa_a_re(work, 3, vl);
    const vint32m1_t a03i = mimo_rvv_solve4_load_soa_a_im(work, 3, vl);
    const vint32m1_t a13r = mimo_rvv_solve4_load_soa_a_re(work, 7, vl);
    const vint32m1_t a13i = mimo_rvv_solve4_load_soa_a_im(work, 7, vl);

    vint32m1_t w01r, w01i, w11r, w11i;
    mimo_rvv_schur4_solve2_i32(
        a00r, a01r, a01i, a11r, b_scale,
        a03r, a03i, a13r, a13i, &w01r, &w01i, &w11r, &w11i,
        MIMO_MMSE4_SCHUR_SCALE_ARG(1), vl);

    vint32m1_t inner_r;
    vint32m1_t inner_i;

    mimo_rvv_schur4_inner_conj_q15_i32(
        a02r, a02i, a12r, a12i, w00r, w00i, w10r, w10i,
        &inner_r, &inner_i, vl);
    const vint32m1_t s00_inner_r = inner_r;

    mimo_rvv_schur4_inner_conj_q15_i32(
        a02r, a02i, a12r, a12i, w01r, w01i, w11r, w11i,
        &inner_r, &inner_i, vl);
    const vint32m1_t s01_inner_r = inner_r;
    const vint32m1_t s01_inner_i = inner_i;

    mimo_rvv_schur4_inner_conj_q15_i32(
        a02r, a02i, a12r, a12i, y0r, y0i, y1r, y1i,
        &inner_r, &inner_i, vl);
    const vint32m1_t r0_inner_r = inner_r;
    const vint32m1_t r0_inner_i = inner_i;

    mimo_rvv_schur4_inner_conj_q15_i32(
        a03r, a03i, a13r, a13i, w01r, w01i, w11r, w11i,
        &inner_r, &inner_i, vl);
    const vint32m1_t s11_inner_r = inner_r;

    mimo_rvv_schur4_inner_conj_q15_i32(
        a03r, a03i, a13r, a13i, y0r, y0i, y1r, y1i,
        &inner_r, &inner_i, vl);
    const vint32m1_t r1_inner_r = inner_r;
    const vint32m1_t r1_inner_i = inner_i;

    const vint32m1_t s00r = __riscv_vsub_vv_i32m1(
        __riscv_vadd_vx_i32m1(mimo_rvv_solve4_load_soa_a_re(work, 10, vl),
                              MIMO_MMSE_LAMBDA_Q15, vl),
        s00_inner_r, vl);
    const vint32m1_t s01r = __riscv_vsub_vv_i32m1(
        mimo_rvv_solve4_load_soa_a_re(work, 11, vl), s01_inner_r, vl);
    const vint32m1_t s01i = __riscv_vsub_vv_i32m1(
        mimo_rvv_solve4_load_soa_a_im(work, 11, vl), s01_inner_i, vl);
    const vint32m1_t s11r = __riscv_vsub_vv_i32m1(
        __riscv_vadd_vx_i32m1(mimo_rvv_solve4_load_soa_a_re(work, 15, vl),
                              MIMO_MMSE_LAMBDA_Q15, vl),
        s11_inner_r, vl);
    const vint32m1_t r0r = __riscv_vsub_vv_i32m1(
        mimo_rvv_solve4_load_soa_b_re(work, 2, vl), r0_inner_r, vl);
    const vint32m1_t r0i = __riscv_vsub_vv_i32m1(
        mimo_rvv_solve4_load_soa_b_im(work, 2, vl), r0_inner_i, vl);
    const vint32m1_t r1r = __riscv_vsub_vv_i32m1(
        mimo_rvv_solve4_load_soa_b_re(work, 3, vl), r1_inner_r, vl);
    const vint32m1_t r1i = __riscv_vsub_vv_i32m1(
        mimo_rvv_solve4_load_soa_b_im(work, 3, vl), r1_inner_i, vl);

    const vint32m1_t s_det_q30 =
        mimo_rvv_schur4_det_q30_i32(s00r, s01r, s01i, s11r, vl);
    const vint32m1_t s_scale =
        mimo_rvv_schur4_scale_from_det_q30_i32(s_det_q30, vl);
    vint32m1_t x2r, x2i, x3r, x3i;

    mimo_rvv_schur4_solve2_i32(
        s00r, s01r, s01i, s11r, s_scale,
        r0r, r0i, r1r, r1i, &x2r, &x2i, &x3r, &x3i,
        MIMO_MMSE4_SCHUR_SCALE_ARG(1), vl);

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

static MIMO_ALWAYS_INLINE void
mimo_ext_eq4_build_solve_fused_rowfuse_soa_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_c16_t *x_block)
{
#if MIMO_USE_EXT_MMSE4_LIVE_BUILD
    vint32m1_t b0r, b0i, b1r, b1i, b2r, b2i, b3r, b3i;
    vint32m1_t a00r, a01r, a01i, a02r, a02i, a03r, a03i;
    vint32m1_t a11r, a12r, a12i, a13r, a13i;
    vint32m1_t a22r, a23r, a23i, a33r;

    mimo_ext_build_hermitian4_site4_upper_row01_live_q15(
        h_site4, y_site4, block,
        &b0r, &b0i, &a00r, &a01r, &a01i, &a02r, &a02i,
        &a03r, &a03i, &b1r, &b1i, &a11r, &a12r, &a12i,
        &a13r, &a13i);
    mimo_ext_build_hermitian4_site4_upper_row23_live_q15(
        h_site4, y_site4, block,
        &b2r, &b2i, &a22r, &a23r, &a23i, &b3r, &b3i, &a33r);

    const size_t vl = __riscv_vsetvl_e32m1(MIMO_SITE4_LANES);
    a00r = __riscv_vadd_vx_i32m1(a00r, MIMO_MMSE_LAMBDA_Q15, vl);
    a11r = __riscv_vadd_vx_i32m1(a11r, MIMO_MMSE_LAMBDA_Q15, vl);
    a22r = __riscv_vadd_vx_i32m1(a22r, MIMO_MMSE_LAMBDA_Q15, vl);
    a33r = __riscv_vadd_vx_i32m1(a33r, MIMO_MMSE_LAMBDA_Q15, vl);

    mimo_rvv_zf4_schur_rvv_solve_vec_q15(
        a00r, a01r, a01i, a02r, a02i, a03r, a03i,
        a11r, a12r, a12i, a13r, a13i,
        a22r, a23r, a23i, a33r,
        b0r, b0i, b1r, b1i, b2r, b2i, b3r, b3i,
        MIMO_MMSE4_SCHUR_SCALE_ARG(1),
        x_block);
#elif MIMO_USE_EXT_MMSE4_NARROW_WORK
    mimo_site4_upper_rhs_soa4_t work MIMO_ALIGN64;

    mimo_ext_build_hermitian4_site4_upper_narrow_q15(
        h_site4, y_site4, block, &work);

    const size_t vl = __riscv_vsetvl_e32m1(MIMO_SITE4_LANES);
    const vint32m1_t a00r = __riscv_vadd_vx_i32m1(
        mimo_ext_load_narrow_i32(&work.a00r[0], vl), MIMO_MMSE_LAMBDA_Q15, vl);
    const vint32m1_t a11r = __riscv_vadd_vx_i32m1(
        mimo_ext_load_narrow_i32(&work.a11r[0], vl), MIMO_MMSE_LAMBDA_Q15, vl);
    const vint32m1_t a22r = __riscv_vadd_vx_i32m1(
        mimo_ext_load_narrow_i32(&work.a22r[0], vl), MIMO_MMSE_LAMBDA_Q15, vl);
    const vint32m1_t a33r = __riscv_vadd_vx_i32m1(
        mimo_ext_load_narrow_i32(&work.a33r[0], vl), MIMO_MMSE_LAMBDA_Q15, vl);

    mimo_rvv_zf4_schur_rvv_solve_vec_q15(
        a00r,
        mimo_ext_load_narrow_i32(&work.a01r[0], vl),
        mimo_ext_load_narrow_i32(&work.a01i[0], vl),
        mimo_ext_load_narrow_i32(&work.a02r[0], vl),
        mimo_ext_load_narrow_i32(&work.a02i[0], vl),
        mimo_ext_load_narrow_i32(&work.a03r[0], vl),
        mimo_ext_load_narrow_i32(&work.a03i[0], vl),
        a11r,
        mimo_ext_load_narrow_i32(&work.a12r[0], vl),
        mimo_ext_load_narrow_i32(&work.a12i[0], vl),
        mimo_ext_load_narrow_i32(&work.a13r[0], vl),
        mimo_ext_load_narrow_i32(&work.a13i[0], vl),
        a22r,
        mimo_ext_load_narrow_i32(&work.a23r[0], vl),
        mimo_ext_load_narrow_i32(&work.a23i[0], vl),
        a33r,
        mimo_ext_load_narrow_i32(&work.b0r[0], vl),
        mimo_ext_load_narrow_i32(&work.b0i[0], vl),
        mimo_ext_load_narrow_i32(&work.b1r[0], vl),
        mimo_ext_load_narrow_i32(&work.b1i[0], vl),
        mimo_ext_load_narrow_i32(&work.b2r[0], vl),
        mimo_ext_load_narrow_i32(&work.b2i[0], vl),
        mimo_ext_load_narrow_i32(&work.b3r[0], vl),
        mimo_ext_load_narrow_i32(&work.b3i[0], vl),
        MIMO_MMSE4_SCHUR_SCALE_ARG(1),
        x_block);
#elif MIMO_USE_EXT_MMSE4_STREAM_WORK_SOLVE
    mimo_site4_soa4_t work MIMO_ALIGN64;

    mimo_ext_build_hermitian4_site4_upper_soa_q15(
        h_site4, y_site4, block, &work);
    mimo_ext_mmse4_schur_rvv_solve_stream_work_q15(&work, x_block);
#else
    mimo_site4_soa4_t work MIMO_ALIGN64;

    mimo_ext_build_hermitian4_site4_upper_soa_q15(
        h_site4, y_site4, block, &work);

    const size_t vl = __riscv_vsetvl_e32m1(MIMO_SITE4_LANES);
    const vint32m1_t a00r = __riscv_vadd_vx_i32m1(
        mimo_rvv_solve4_load_soa_a_re(&work, 0, vl), MIMO_MMSE_LAMBDA_Q15, vl);
    const vint32m1_t a11r = __riscv_vadd_vx_i32m1(
        mimo_rvv_solve4_load_soa_a_re(&work, 5, vl), MIMO_MMSE_LAMBDA_Q15, vl);
    const vint32m1_t a22r = __riscv_vadd_vx_i32m1(
        mimo_rvv_solve4_load_soa_a_re(&work, 10, vl), MIMO_MMSE_LAMBDA_Q15, vl);
    const vint32m1_t a33r = __riscv_vadd_vx_i32m1(
        mimo_rvv_solve4_load_soa_a_re(&work, 15, vl), MIMO_MMSE_LAMBDA_Q15, vl);

    mimo_rvv_zf4_schur_rvv_solve_vec_q15(
        a00r,
        mimo_rvv_solve4_load_soa_a_re(&work, 1, vl),
        mimo_rvv_solve4_load_soa_a_im(&work, 1, vl),
        mimo_rvv_solve4_load_soa_a_re(&work, 2, vl),
        mimo_rvv_solve4_load_soa_a_im(&work, 2, vl),
        mimo_rvv_solve4_load_soa_a_re(&work, 3, vl),
        mimo_rvv_solve4_load_soa_a_im(&work, 3, vl),
        a11r,
        mimo_rvv_solve4_load_soa_a_re(&work, 6, vl),
        mimo_rvv_solve4_load_soa_a_im(&work, 6, vl),
        mimo_rvv_solve4_load_soa_a_re(&work, 7, vl),
        mimo_rvv_solve4_load_soa_a_im(&work, 7, vl),
        a22r,
        mimo_rvv_solve4_load_soa_a_re(&work, 11, vl),
        mimo_rvv_solve4_load_soa_a_im(&work, 11, vl),
        a33r,
        mimo_rvv_solve4_load_soa_b_re(&work, 0, vl),
        mimo_rvv_solve4_load_soa_b_im(&work, 0, vl),
        mimo_rvv_solve4_load_soa_b_re(&work, 1, vl),
        mimo_rvv_solve4_load_soa_b_im(&work, 1, vl),
        mimo_rvv_solve4_load_soa_b_re(&work, 2, vl),
        mimo_rvv_solve4_load_soa_b_im(&work, 2, vl),
        mimo_rvv_solve4_load_soa_b_re(&work, 3, vl),
        mimo_rvv_solve4_load_soa_b_im(&work, 3, vl),
        MIMO_MMSE4_SCHUR_SCALE_ARG(1),
        x_block);
#endif
}
#endif

#if MIMO_USE_EXT_ZF4_TRUE16B_WORK
#if MIMO_USE_EXT_ZF4_PACK32_WORK
#define MIMO_EXT_SITE4_PACK_C16_FIELD_ASM(VR_, VI_, PTR_) \
    "vnclip.wi v16, " VR_ ", 0\n\t" \
    "vnclip.wi v17, " VI_ ", 0\n\t" \
    "vsseg2e16.v v16, (%[" PTR_ "]), v0.t\n\t"
#define MIMO_EXT_SITE4_PACK_REAL_FIELD_ASM(VR_, PTR_) \
    "vnclip.wi v16, " VR_ ", 0\n\t" \
    "vmv.v.i v17, 0\n\t" \
    "vsseg2e16.v v16, (%[" PTR_ "]), v0.t\n\t"
#else
#define MIMO_EXT_SITE4_STORE_I16_FIELD_ASM(VREG_, PTR_) \
    "vnclip.wi v16, " VREG_ ", 0\n\t" \
    "vsse16.v v16, (%[" PTR_ "]), t1, v0.t\n\t"
#define MIMO_EXT_SITE4_ZERO_I16_FIELD_ASM(PTR_) \
    "vmv.v.i v16, 0\n\t" \
    "vsse16.v v16, (%[" PTR_ "]), t1, v0.t\n\t"
#endif

static MIMO_ALWAYS_INLINE void
mimo_ext_build_hermitian4_site4_upper_row0_i16_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_site4_upper_i16_t *work)
{
    const mimo_c16_t *p0 = mimo_ext_site4_h4_vec(h_site4, block, 0, 0);
    const mimo_c16_t *p1 = mimo_ext_site4_h4_vec(h_site4, block, 1, 0);
    const mimo_c16_t *p2 = mimo_ext_site4_h4_vec(h_site4, block, 2, 0);
    const mimo_c16_t *p3 = mimo_ext_site4_h4_vec(h_site4, block, 3, 0);
    const mimo_c16_t *py = mimo_ext_site4_y4_vec(y_site4, block, 0);

    asm volatile(
        "vmv.v.i v2, 0\n\t"
        "vmv.v.i v4, 0\n\t"
        "vmv.v.i v6, 0\n\t"
        "vmv.v.i v8, 0\n\t"
        "vmv.v.i v10, 0\n\t"
        "vmv.v.i v12, 0\n\t"
        "vmv.v.i v14, 0\n\t"
        "vmv.v.i v22, 0\n\t"
        "vmv.v.i v26, 0\n\t"
        "li t0, 4\n\t"
        "1:\n\t"
        "vle16.v v20, (%[p0])\n\t"
        "vle16.v v24, (%[py])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv v2, v2, v18\n\t"
        "vwadd.wv v4, v4, v19\n\t"
        "vmv.v.v v24, v20\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vwadd.wv v6, v6, v18\n\t"
        "vle16.v v24, (%[p1])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv v8, v8, v18\n\t"
        "vwadd.wv v10, v10, v19\n\t"
        "vle16.v v24, (%[p2])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv v12, v12, v18\n\t"
        "vwadd.wv v14, v14, v19\n\t"
        "vle16.v v24, (%[p3])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv v22, v22, v18\n\t"
        "vwadd.wv v26, v26, v19\n\t"
        "addi %[p0], %[p0], 16\n\t"
        "addi %[p1], %[p1], 16\n\t"
        "addi %[p2], %[p2], 16\n\t"
        "addi %[p3], %[p3], 16\n\t"
        "addi %[py], %[py], 16\n\t"
        "addi t0, t0, -1\n\t"
        "bnez t0, 1b\n\t"
#if MIMO_USE_EXT_ZF4_PACK32_WORK
        MIMO_EXT_SITE4_PACK_C16_FIELD_ASM("v2", "v4", "b0")
        MIMO_EXT_SITE4_PACK_REAL_FIELD_ASM("v6", "a00")
        MIMO_EXT_SITE4_PACK_C16_FIELD_ASM("v8", "v10", "a01")
        MIMO_EXT_SITE4_PACK_C16_FIELD_ASM("v12", "v14", "a02")
        MIMO_EXT_SITE4_PACK_C16_FIELD_ASM("v22", "v26", "a03")
#else
        "li t1, 4\n\t"
        MIMO_EXT_SITE4_STORE_I16_FIELD_ASM("v2", "b0r")
        MIMO_EXT_SITE4_STORE_I16_FIELD_ASM("v4", "b0i")
        MIMO_EXT_SITE4_STORE_I16_FIELD_ASM("v6", "a00r")
        MIMO_EXT_SITE4_ZERO_I16_FIELD_ASM("a00i")
        MIMO_EXT_SITE4_STORE_I16_FIELD_ASM("v8", "a01r")
        MIMO_EXT_SITE4_STORE_I16_FIELD_ASM("v10", "a01i")
        MIMO_EXT_SITE4_STORE_I16_FIELD_ASM("v12", "a02r")
        MIMO_EXT_SITE4_STORE_I16_FIELD_ASM("v14", "a02i")
        MIMO_EXT_SITE4_STORE_I16_FIELD_ASM("v22", "a03r")
        MIMO_EXT_SITE4_STORE_I16_FIELD_ASM("v26", "a03i")
#endif
        : [p0] "+r"(p0), [p1] "+r"(p1), [p2] "+r"(p2),
          [p3] "+r"(p3), [py] "+r"(py)
#if MIMO_USE_EXT_ZF4_PACK32_WORK
        : [b0] "r"(&work->b[0][0]),
          [a00] "r"(&work->a[0][0]),
          [a01] "r"(&work->a[1][0]),
          [a02] "r"(&work->a[2][0]),
          [a03] "r"(&work->a[3][0])
        : "t0", "memory");
#else
        : [b0r] "r"(&work->b[0][0].r),
          [b0i] "r"(&work->b[0][0].i),
          [a00r] "r"(&work->a[0][0].r),
          [a00i] "r"(&work->a[0][0].i),
          [a01r] "r"(&work->a[1][0].r),
          [a01i] "r"(&work->a[1][0].i),
          [a02r] "r"(&work->a[2][0].r),
          [a02i] "r"(&work->a[2][0].i),
          [a03r] "r"(&work->a[3][0].r),
          [a03i] "r"(&work->a[3][0].i)
        : "t0", "t1", "memory");
#endif
}

static MIMO_ALWAYS_INLINE void
mimo_ext_build_hermitian4_site4_upper_row1_i16_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_site4_upper_i16_t *work)
{
    const mimo_c16_t *p1 = mimo_ext_site4_h4_vec(h_site4, block, 1, 0);
    const mimo_c16_t *p2 = mimo_ext_site4_h4_vec(h_site4, block, 2, 0);
    const mimo_c16_t *p3 = mimo_ext_site4_h4_vec(h_site4, block, 3, 0);
    const mimo_c16_t *py = mimo_ext_site4_y4_vec(y_site4, block, 0);

    asm volatile(
        "vmv.v.i v2, 0\n\t"
        "vmv.v.i v4, 0\n\t"
        "vmv.v.i v6, 0\n\t"
        "vmv.v.i v8, 0\n\t"
        "vmv.v.i v10, 0\n\t"
        "vmv.v.i v12, 0\n\t"
        "vmv.v.i v14, 0\n\t"
        "li t0, 4\n\t"
        "1:\n\t"
        "vle16.v v20, (%[p1])\n\t"
        "vle16.v v24, (%[py])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv v2, v2, v18\n\t"
        "vwadd.wv v4, v4, v19\n\t"
        "vmv.v.v v24, v20\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vwadd.wv v6, v6, v18\n\t"
        "vle16.v v24, (%[p2])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv v8, v8, v18\n\t"
        "vwadd.wv v10, v10, v19\n\t"
        "vle16.v v24, (%[p3])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv v12, v12, v18\n\t"
        "vwadd.wv v14, v14, v19\n\t"
        "addi %[p1], %[p1], 16\n\t"
        "addi %[p2], %[p2], 16\n\t"
        "addi %[p3], %[p3], 16\n\t"
        "addi %[py], %[py], 16\n\t"
        "addi t0, t0, -1\n\t"
        "bnez t0, 1b\n\t"
#if MIMO_USE_EXT_ZF4_PACK32_WORK
        MIMO_EXT_SITE4_PACK_C16_FIELD_ASM("v2", "v4", "b1")
        MIMO_EXT_SITE4_PACK_REAL_FIELD_ASM("v6", "a11")
        MIMO_EXT_SITE4_PACK_C16_FIELD_ASM("v8", "v10", "a12")
        MIMO_EXT_SITE4_PACK_C16_FIELD_ASM("v12", "v14", "a13")
#else
        "li t1, 4\n\t"
        MIMO_EXT_SITE4_STORE_I16_FIELD_ASM("v2", "b1r")
        MIMO_EXT_SITE4_STORE_I16_FIELD_ASM("v4", "b1i")
        MIMO_EXT_SITE4_STORE_I16_FIELD_ASM("v6", "a11r")
        MIMO_EXT_SITE4_ZERO_I16_FIELD_ASM("a11i")
        MIMO_EXT_SITE4_STORE_I16_FIELD_ASM("v8", "a12r")
        MIMO_EXT_SITE4_STORE_I16_FIELD_ASM("v10", "a12i")
        MIMO_EXT_SITE4_STORE_I16_FIELD_ASM("v12", "a13r")
        MIMO_EXT_SITE4_STORE_I16_FIELD_ASM("v14", "a13i")
#endif
        : [p1] "+r"(p1), [p2] "+r"(p2), [p3] "+r"(p3),
          [py] "+r"(py)
#if MIMO_USE_EXT_ZF4_PACK32_WORK
        : [b1] "r"(&work->b[1][0]),
          [a11] "r"(&work->a[4][0]),
          [a12] "r"(&work->a[5][0]),
          [a13] "r"(&work->a[6][0])
        : "t0", "memory");
#else
        : [b1r] "r"(&work->b[1][0].r),
          [b1i] "r"(&work->b[1][0].i),
          [a11r] "r"(&work->a[4][0].r),
          [a11i] "r"(&work->a[4][0].i),
          [a12r] "r"(&work->a[5][0].r),
          [a12i] "r"(&work->a[5][0].i),
          [a13r] "r"(&work->a[6][0].r),
          [a13i] "r"(&work->a[6][0].i)
        : "t0", "t1", "memory");
#endif
}

static MIMO_ALWAYS_INLINE void
mimo_ext_build_hermitian4_site4_upper_row2_i16_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_site4_upper_i16_t *work)
{
    const mimo_c16_t *p2 = mimo_ext_site4_h4_vec(h_site4, block, 2, 0);
    const mimo_c16_t *p3 = mimo_ext_site4_h4_vec(h_site4, block, 3, 0);
    const mimo_c16_t *py = mimo_ext_site4_y4_vec(y_site4, block, 0);

    asm volatile(
        "vmv.v.i v2, 0\n\t"
        "vmv.v.i v4, 0\n\t"
        "vmv.v.i v6, 0\n\t"
        "vmv.v.i v8, 0\n\t"
        "vmv.v.i v10, 0\n\t"
        "li t0, 4\n\t"
        "1:\n\t"
        "vle16.v v20, (%[p2])\n\t"
        "vle16.v v24, (%[py])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv v2, v2, v18\n\t"
        "vwadd.wv v4, v4, v19\n\t"
        "vmv.v.v v24, v20\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vwadd.wv v6, v6, v18\n\t"
        "vle16.v v24, (%[p3])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv v8, v8, v18\n\t"
        "vwadd.wv v10, v10, v19\n\t"
        "addi %[p2], %[p2], 16\n\t"
        "addi %[p3], %[p3], 16\n\t"
        "addi %[py], %[py], 16\n\t"
        "addi t0, t0, -1\n\t"
        "bnez t0, 1b\n\t"
#if MIMO_USE_EXT_ZF4_PACK32_WORK
        MIMO_EXT_SITE4_PACK_C16_FIELD_ASM("v2", "v4", "b2")
        MIMO_EXT_SITE4_PACK_REAL_FIELD_ASM("v6", "a22")
        MIMO_EXT_SITE4_PACK_C16_FIELD_ASM("v8", "v10", "a23")
#else
        "li t1, 4\n\t"
        MIMO_EXT_SITE4_STORE_I16_FIELD_ASM("v2", "b2r")
        MIMO_EXT_SITE4_STORE_I16_FIELD_ASM("v4", "b2i")
        MIMO_EXT_SITE4_STORE_I16_FIELD_ASM("v6", "a22r")
        MIMO_EXT_SITE4_ZERO_I16_FIELD_ASM("a22i")
        MIMO_EXT_SITE4_STORE_I16_FIELD_ASM("v8", "a23r")
        MIMO_EXT_SITE4_STORE_I16_FIELD_ASM("v10", "a23i")
#endif
        : [p2] "+r"(p2), [p3] "+r"(p3), [py] "+r"(py)
#if MIMO_USE_EXT_ZF4_PACK32_WORK
        : [b2] "r"(&work->b[2][0]),
          [a22] "r"(&work->a[7][0]),
          [a23] "r"(&work->a[8][0])
        : "t0", "memory");
#else
        : [b2r] "r"(&work->b[2][0].r),
          [b2i] "r"(&work->b[2][0].i),
          [a22r] "r"(&work->a[7][0].r),
          [a22i] "r"(&work->a[7][0].i),
          [a23r] "r"(&work->a[8][0].r),
          [a23i] "r"(&work->a[8][0].i)
        : "t0", "t1", "memory");
#endif
}

static MIMO_ALWAYS_INLINE void
mimo_ext_build_hermitian4_site4_upper_row3_i16_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_site4_upper_i16_t *work)
{
    const mimo_c16_t *p3 = mimo_ext_site4_h4_vec(h_site4, block, 3, 0);
    const mimo_c16_t *py = mimo_ext_site4_y4_vec(y_site4, block, 0);

    asm volatile(
        "vmv.v.i v2, 0\n\t"
        "vmv.v.i v4, 0\n\t"
        "vmv.v.i v6, 0\n\t"
        "li t0, 4\n\t"
        "1:\n\t"
        "vle16.v v20, (%[p3])\n\t"
        "vle16.v v24, (%[py])\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv v2, v2, v18\n\t"
        "vwadd.wv v4, v4, v19\n\t"
        "vmv.v.v v24, v20\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vwadd.wv v6, v6, v18\n\t"
        "addi %[p3], %[p3], 16\n\t"
        "addi %[py], %[py], 16\n\t"
        "addi t0, t0, -1\n\t"
        "bnez t0, 1b\n\t"
#if MIMO_USE_EXT_ZF4_PACK32_WORK
        MIMO_EXT_SITE4_PACK_C16_FIELD_ASM("v2", "v4", "b3")
        MIMO_EXT_SITE4_PACK_REAL_FIELD_ASM("v6", "a33")
#else
        "li t1, 4\n\t"
        MIMO_EXT_SITE4_STORE_I16_FIELD_ASM("v2", "b3r")
        MIMO_EXT_SITE4_STORE_I16_FIELD_ASM("v4", "b3i")
        MIMO_EXT_SITE4_STORE_I16_FIELD_ASM("v6", "a33r")
        MIMO_EXT_SITE4_ZERO_I16_FIELD_ASM("a33i")
#endif
        : [p3] "+r"(p3), [py] "+r"(py)
#if MIMO_USE_EXT_ZF4_PACK32_WORK
        : [b3] "r"(&work->b[3][0]),
          [a33] "r"(&work->a[9][0])
        : "t0", "memory");
#else
        : [b3r] "r"(&work->b[3][0].r),
          [b3i] "r"(&work->b[3][0].i),
          [a33r] "r"(&work->a[9][0].r),
          [a33i] "r"(&work->a[9][0].i)
        : "t0", "t1", "memory");
#endif
}

#if MIMO_USE_EXT_ZF4_VSCMACC_BUILD
static MIMO_ALWAYS_INLINE void
mimo_ext_build_hermitian4_site4_upper_row0_vscmacc_i16_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_site4_upper_i16_t *work)
{
    const mimo_c16_t *p0 = mimo_ext_site4_h4_vec(h_site4, block, 0, 0);
    const mimo_c16_t *p1 = mimo_ext_site4_h4_vec(h_site4, block, 1, 0);
    const mimo_c16_t *p2 = mimo_ext_site4_h4_vec(h_site4, block, 2, 0);
    const mimo_c16_t *p3 = mimo_ext_site4_h4_vec(h_site4, block, 3, 0);
    const mimo_c16_t *py = mimo_ext_site4_y4_vec(y_site4, block, 0);

    asm volatile(
        "vmv.v.i v2, 0\n\t"
        "vmv.v.i v4, 0\n\t"
        "vmv.v.i v6, 0\n\t"
        "vmv.v.i v8, 0\n\t"
        "vmv.v.i v10, 0\n\t"
        "li t0, 4\n\t"
        "1:\n\t"
        "vle16.v v20, (%[p0])\n\t"
        "vle16.v v24, (%[py])\n\t"
        ".word 0xDB4C0157\n\t"
        "vmv.v.v v24, v20\n\t"
        ".word 0xDB4C0257\n\t"
        "vle16.v v24, (%[p1])\n\t"
        ".word 0xDB4C0357\n\t"
        "vle16.v v24, (%[p2])\n\t"
        ".word 0xDB4C0457\n\t"
        "vle16.v v24, (%[p3])\n\t"
        ".word 0xDB4C0557\n\t"
        "addi %[p0], %[p0], 16\n\t"
        "addi %[p1], %[p1], 16\n\t"
        "addi %[p2], %[p2], 16\n\t"
        "addi %[p3], %[p3], 16\n\t"
        "addi %[py], %[py], 16\n\t"
        "addi t0, t0, -1\n\t"
        "bnez t0, 1b\n\t"
        "vse16.v v2, (%[b0])\n\t"
        "vse16.v v4, (%[a00])\n\t"
        "vse16.v v6, (%[a01])\n\t"
        "vse16.v v8, (%[a02])\n\t"
        "vse16.v v10, (%[a03])\n\t"
        : [p0] "+r"(p0), [p1] "+r"(p1), [p2] "+r"(p2),
          [p3] "+r"(p3), [py] "+r"(py)
        : [b0] "r"(&work->b[0][0]),
          [a00] "r"(&work->a[0][0]),
          [a01] "r"(&work->a[1][0]),
          [a02] "r"(&work->a[2][0]),
          [a03] "r"(&work->a[3][0])
        : "t0", "memory");
}

static MIMO_ALWAYS_INLINE void
mimo_ext_build_hermitian4_site4_upper_row1_vscmacc_i16_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_site4_upper_i16_t *work)
{
    const mimo_c16_t *p1 = mimo_ext_site4_h4_vec(h_site4, block, 1, 0);
    const mimo_c16_t *p2 = mimo_ext_site4_h4_vec(h_site4, block, 2, 0);
    const mimo_c16_t *p3 = mimo_ext_site4_h4_vec(h_site4, block, 3, 0);
    const mimo_c16_t *py = mimo_ext_site4_y4_vec(y_site4, block, 0);

    asm volatile(
        "vmv.v.i v2, 0\n\t"
        "vmv.v.i v4, 0\n\t"
        "vmv.v.i v6, 0\n\t"
        "vmv.v.i v8, 0\n\t"
        "li t0, 4\n\t"
        "1:\n\t"
        "vle16.v v20, (%[p1])\n\t"
        "vle16.v v24, (%[py])\n\t"
        ".word 0xDB4C0157\n\t"
        "vmv.v.v v24, v20\n\t"
        ".word 0xDB4C0257\n\t"
        "vle16.v v24, (%[p2])\n\t"
        ".word 0xDB4C0357\n\t"
        "vle16.v v24, (%[p3])\n\t"
        ".word 0xDB4C0457\n\t"
        "addi %[p1], %[p1], 16\n\t"
        "addi %[p2], %[p2], 16\n\t"
        "addi %[p3], %[p3], 16\n\t"
        "addi %[py], %[py], 16\n\t"
        "addi t0, t0, -1\n\t"
        "bnez t0, 1b\n\t"
        "vse16.v v2, (%[b1])\n\t"
        "vse16.v v4, (%[a11])\n\t"
        "vse16.v v6, (%[a12])\n\t"
        "vse16.v v8, (%[a13])\n\t"
        : [p1] "+r"(p1), [p2] "+r"(p2), [p3] "+r"(p3),
          [py] "+r"(py)
        : [b1] "r"(&work->b[1][0]),
          [a11] "r"(&work->a[4][0]),
          [a12] "r"(&work->a[5][0]),
          [a13] "r"(&work->a[6][0])
        : "t0", "memory");
}

static MIMO_ALWAYS_INLINE void
mimo_ext_build_hermitian4_site4_upper_row2_vscmacc_i16_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_site4_upper_i16_t *work)
{
    const mimo_c16_t *p2 = mimo_ext_site4_h4_vec(h_site4, block, 2, 0);
    const mimo_c16_t *p3 = mimo_ext_site4_h4_vec(h_site4, block, 3, 0);
    const mimo_c16_t *py = mimo_ext_site4_y4_vec(y_site4, block, 0);

    asm volatile(
        "vmv.v.i v2, 0\n\t"
        "vmv.v.i v4, 0\n\t"
        "vmv.v.i v6, 0\n\t"
        "li t0, 4\n\t"
        "1:\n\t"
        "vle16.v v20, (%[p2])\n\t"
        "vle16.v v24, (%[py])\n\t"
        ".word 0xDB4C0157\n\t"
        "vmv.v.v v24, v20\n\t"
        ".word 0xDB4C0257\n\t"
        "vle16.v v24, (%[p3])\n\t"
        ".word 0xDB4C0357\n\t"
        "addi %[p2], %[p2], 16\n\t"
        "addi %[p3], %[p3], 16\n\t"
        "addi %[py], %[py], 16\n\t"
        "addi t0, t0, -1\n\t"
        "bnez t0, 1b\n\t"
        "vse16.v v2, (%[b2])\n\t"
        "vse16.v v4, (%[a22])\n\t"
        "vse16.v v6, (%[a23])\n\t"
        : [p2] "+r"(p2), [p3] "+r"(p3), [py] "+r"(py)
        : [b2] "r"(&work->b[2][0]),
          [a22] "r"(&work->a[7][0]),
          [a23] "r"(&work->a[8][0])
        : "t0", "memory");
}

static MIMO_ALWAYS_INLINE void
mimo_ext_build_hermitian4_site4_upper_row3_vscmacc_i16_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_site4_upper_i16_t *work)
{
    const mimo_c16_t *p3 = mimo_ext_site4_h4_vec(h_site4, block, 3, 0);
    const mimo_c16_t *py = mimo_ext_site4_y4_vec(y_site4, block, 0);

    asm volatile(
        "vmv.v.i v2, 0\n\t"
        "vmv.v.i v4, 0\n\t"
        "li t0, 4\n\t"
        "1:\n\t"
        "vle16.v v20, (%[p3])\n\t"
        "vle16.v v24, (%[py])\n\t"
        ".word 0xDB4C0157\n\t"
        "vmv.v.v v24, v20\n\t"
        ".word 0xDB4C0257\n\t"
        "addi %[p3], %[p3], 16\n\t"
        "addi %[py], %[py], 16\n\t"
        "addi t0, t0, -1\n\t"
        "bnez t0, 1b\n\t"
        "vse16.v v2, (%[b3])\n\t"
        "vse16.v v4, (%[a33])\n\t"
        : [p3] "+r"(p3), [py] "+r"(py)
        : [b3] "r"(&work->b[3][0]),
          [a33] "r"(&work->a[9][0])
        : "t0", "memory");
}

#if MIMO_USE_EXT_ZF4_VSCMACC_FUSED_BUILD
static MIMO_ALWAYS_INLINE void
mimo_ext_build_hermitian4_site4_upper_fused_vscmacc_i16_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_site4_upper_i16_t *work)
{
    const mimo_c16_t *p0 = mimo_ext_site4_h4_vec(h_site4, block, 0, 0);
    const mimo_c16_t *p1 = mimo_ext_site4_h4_vec(h_site4, block, 1, 0);
    const mimo_c16_t *p2 = mimo_ext_site4_h4_vec(h_site4, block, 2, 0);
    const mimo_c16_t *p3 = mimo_ext_site4_h4_vec(h_site4, block, 3, 0);
    const mimo_c16_t *py = mimo_ext_site4_y4_vec(y_site4, block, 0);

    asm volatile(
#if MIMO_USE_EXT_ZF4_VSCMACC_SEED_BUILD
        "vle16.v v20, (%[p0])\n\t"
        "vle16.v v22, (%[p1])\n\t"
        "vle16.v v24, (%[p2])\n\t"
        "vle16.v v26, (%[p3])\n\t"
        "vle16.v v28, (%[py])\n\t"
        ".word 0xCF4E0157\n\t"
        ".word 0xCF4A0257\n\t"
        ".word 0xCF4B0357\n\t"
        ".word 0xCF4C0457\n\t"
        ".word 0xCF4D0557\n\t"
        ".word 0xCF6E0657\n\t"
        ".word 0xCF6B0757\n\t"
        ".word 0xCF6C0857\n\t"
        ".word 0xCF6D0957\n\t"
        ".word 0xCF8E0F57\n\t"
        ".word 0xCF8C00D7\n\t"
        ".word 0xCF8D01D7\n\t"
        ".word 0xCFAE02D7\n\t"
        ".word 0xCFAD03D7\n\t"
        "addi %[p0], %[p0], 16\n\t"
        "addi %[p1], %[p1], 16\n\t"
        "addi %[p2], %[p2], 16\n\t"
        "addi %[p3], %[p3], 16\n\t"
        "addi %[py], %[py], 16\n\t"
        "li t0, 3\n\t"
#else
        "vmv.v.i v2, 0\n\t"
        "vmv.v.i v4, 0\n\t"
        "vmv.v.i v6, 0\n\t"
        "vmv.v.i v8, 0\n\t"
        "vmv.v.i v10, 0\n\t"
        "vmv.v.i v12, 0\n\t"
        "vmv.v.i v14, 0\n\t"
        "vmv.v.i v16, 0\n\t"
        "vmv.v.i v18, 0\n\t"
        "vmv.v.i v30, 0\n\t"
        "vmv.v.i v1, 0\n\t"
        "vmv.v.i v3, 0\n\t"
        "vmv.v.i v5, 0\n\t"
        "vmv.v.i v7, 0\n\t"
        "li t0, 4\n\t"
#endif
        "1:\n\t"
        "vle16.v v20, (%[p0])\n\t"
        "vle16.v v22, (%[p1])\n\t"
        "vle16.v v24, (%[p2])\n\t"
        "vle16.v v26, (%[p3])\n\t"
        "vle16.v v28, (%[py])\n\t"
        ".word 0xDB4E0157\n\t"
        ".word 0xDB4A0257\n\t"
        ".word 0xDB4B0357\n\t"
        ".word 0xDB4C0457\n\t"
        ".word 0xDB4D0557\n\t"
        ".word 0xDB6E0657\n\t"
        ".word 0xDB6B0757\n\t"
        ".word 0xDB6C0857\n\t"
        ".word 0xDB6D0957\n\t"
        ".word 0xDB8E0F57\n\t"
        ".word 0xDB8C00D7\n\t"
        ".word 0xDB8D01D7\n\t"
        ".word 0xDBAE02D7\n\t"
        ".word 0xDBAD03D7\n\t"
        "addi %[p0], %[p0], 16\n\t"
        "addi %[p1], %[p1], 16\n\t"
        "addi %[p2], %[p2], 16\n\t"
        "addi %[p3], %[p3], 16\n\t"
        "addi %[py], %[py], 16\n\t"
        "addi t0, t0, -1\n\t"
        "bnez t0, 1b\n\t"
        "vse16.v v2, (%[b0])\n\t"
        "vse16.v v4, (%[a00])\n\t"
        "vse16.v v6, (%[a01])\n\t"
        "vse16.v v8, (%[a02])\n\t"
        "vse16.v v10, (%[a03])\n\t"
        "vse16.v v12, (%[b1])\n\t"
        "vse16.v v14, (%[a11])\n\t"
        "vse16.v v16, (%[a12])\n\t"
        "vse16.v v18, (%[a13])\n\t"
#if !MIMO_EXT_USE_LIVE_TAIL_SCHUR
        "vse16.v v30, (%[b2])\n\t"
        "vse16.v v1, (%[a22])\n\t"
        "vse16.v v3, (%[a23])\n\t"
        "vse16.v v5, (%[b3])\n\t"
        "vse16.v v7, (%[a33])\n\t"
#endif
        : [p0] "+r"(p0), [p1] "+r"(p1), [p2] "+r"(p2),
          [p3] "+r"(p3), [py] "+r"(py)
        : [b0] "r"(&work->b[0][0]),
          [a00] "r"(&work->a[0][0]),
          [a01] "r"(&work->a[1][0]),
          [a02] "r"(&work->a[2][0]),
          [a03] "r"(&work->a[3][0]),
          [b1] "r"(&work->b[1][0]),
          [a11] "r"(&work->a[4][0]),
          [a12] "r"(&work->a[5][0]),
          [a13] "r"(&work->a[6][0])
#if !MIMO_EXT_USE_LIVE_TAIL_SCHUR
          ,
          [b2] "r"(&work->b[2][0]),
          [a22] "r"(&work->a[7][0]),
          [a23] "r"(&work->a[8][0]),
          [b3] "r"(&work->b[3][0]),
          [a33] "r"(&work->a[9][0])
#endif
        : "t0", "memory");
}
#endif
#endif

static MIMO_ALWAYS_INLINE void
mimo_ext_build_hermitian4_site4_upper_i16_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_site4_upper_i16_t *work)
{
#if !MIMO_USE_SITE4_PRELUDE_HOIST
    mimo_ext_site4_prelude_asm();
#endif
#if MIMO_USE_EXT_ZF4_VSCMACC_BUILD
#if MIMO_USE_EXT_ZF4_VSCMACC_FUSED_BUILD
    mimo_ext_build_hermitian4_site4_upper_fused_vscmacc_i16_q15(
        h_site4, y_site4, block, work);
    return;
#endif
    mimo_ext_build_hermitian4_site4_upper_row0_vscmacc_i16_q15(
        h_site4, y_site4, block, work);
    mimo_ext_build_hermitian4_site4_upper_row1_vscmacc_i16_q15(
        h_site4, y_site4, block, work);
    mimo_ext_build_hermitian4_site4_upper_row2_vscmacc_i16_q15(
        h_site4, y_site4, block, work);
    mimo_ext_build_hermitian4_site4_upper_row3_vscmacc_i16_q15(
        h_site4, y_site4, block, work);
    return;
#endif
    mimo_ext_build_hermitian4_site4_upper_row0_i16_q15(
        h_site4, y_site4, block, work);
    mimo_ext_build_hermitian4_site4_upper_row1_i16_q15(
        h_site4, y_site4, block, work);
    mimo_ext_build_hermitian4_site4_upper_row2_i16_q15(
        h_site4, y_site4, block, work);
    mimo_ext_build_hermitian4_site4_upper_row3_i16_q15(
        h_site4, y_site4, block, work);
}

#if MIMO_USE_EXT_ZF4_PACK32_WORK
#undef MIMO_EXT_SITE4_PACK_C16_FIELD_ASM
#undef MIMO_EXT_SITE4_PACK_REAL_FIELD_ASM
#else
#undef MIMO_EXT_SITE4_STORE_I16_FIELD_ASM
#undef MIMO_EXT_SITE4_ZERO_I16_FIELD_ASM
#endif
#endif
#endif

#if MIMO_USE_SITE4_HERMITIAN_UPPER_ROWPAIR
static MIMO_ALWAYS_INLINE void mimo_ext_build_hermitian4_site4_upper_row23_soa_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_site4_soa4_t *work)
{
    const mimo_c16_t *p2 = mimo_ext_site4_h4_vec(h_site4, block, 2, 0);
    const mimo_c16_t *p3 = mimo_ext_site4_h4_vec(h_site4, block, 3, 0);
    const mimo_c16_t *py = mimo_ext_site4_y4_vec(y_site4, block, 0);

    asm volatile(
        "vmv.v.i v2, 0\n\t"
        "vmv.v.i v4, 0\n\t"
        "vmv.v.i v6, 0\n\t"
        "vmv.v.i v8, 0\n\t"
        "vmv.v.i v10, 0\n\t"
        "vmv.v.i v12, 0\n\t"
        "vmv.v.i v14, 0\n\t"
        "vmv.v.i v22, 0\n\t"
        "li t0, 4\n\t"
        "1:\n\t"
        "vle16.v v20, (%[p2])\n\t"
        "vle16.v v24, (%[py])\n\t"
        "vmv.v.v v27, v24\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv v2, v2, v18\n\t"
        "vwadd.wv v4, v4, v19\n\t"
        "vmv.v.v v24, v20\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vwadd.wv v6, v6, v18\n\t"
        "vle16.v v24, (%[p3])\n\t"
        "vmv.v.v v25, v24\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv v8, v8, v18\n\t"
        "vwadd.wv v10, v10, v19\n\t"
        "vmv.v.v v20, v25\n\t"
        "vmv.v.v v24, v27\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vrgather.vv v19, v16, v30\n\t"
        "vwadd.wv v12, v12, v18\n\t"
        "vwadd.wv v14, v14, v19\n\t"
        "vmv.v.v v24, v25\n\t"
        ".word 0xCF4C0857\n\t"
        "vrgather.vv v18, v16, v28\n\t"
        "vwadd.wv v22, v22, v18\n\t"
        "addi %[p2], %[p2], 16\n\t"
        "addi %[p3], %[p3], 16\n\t"
        "addi %[py], %[py], 16\n\t"
        "addi t0, t0, -1\n\t"
        "bnez t0, 1b\n\t"
        "vse32.v v2, (%[b2r]), v0.t\n\t"
        "vse32.v v4, (%[b2i]), v0.t\n\t"
        "vse32.v v6, (%[a22r]), v0.t\n\t"
        "vse32.v v8, (%[a23r]), v0.t\n\t"
        "vse32.v v10, (%[a23i]), v0.t\n\t"
        "vse32.v v12, (%[b3r]), v0.t\n\t"
        "vse32.v v14, (%[b3i]), v0.t\n\t"
        "vse32.v v22, (%[a33r]), v0.t\n\t"
        : [p2] "+r"(p2), [p3] "+r"(p3), [py] "+r"(py)
        : [b2r] "r"(&work->b_re[2][0]),
          [b2i] "r"(&work->b_im[2][0]),
          [a22r] "r"(&work->a_re[10][0]),
          [a23r] "r"(&work->a_re[11][0]),
          [a23i] "r"(&work->a_im[11][0]),
          [b3r] "r"(&work->b_re[3][0]),
          [b3i] "r"(&work->b_im[3][0]),
          [a33r] "r"(&work->a_re[15][0])
        : "t0", "memory");
}

static MIMO_ALWAYS_INLINE void mimo_ext_build_hermitian4_site4_upper_rowpair_soa_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_site4_soa4_t *work)
{
    mimo_ext_build_hermitian4_site4_upper_row0_soa_q15(
        h_site4, y_site4, block, work);
    mimo_ext_build_hermitian4_site4_upper_row1_soa_q15(
        h_site4, y_site4, block, work);
    mimo_ext_build_hermitian4_site4_upper_row23_soa_q15(
        h_site4, y_site4, block, work);
}
#endif

static MIMO_ALWAYS_INLINE void mimo_ext_build_normal4_site4_soa_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_site4_soa4_t *work)
{
    asm volatile(MIMO_EXT_SITE4_PRELUDE_ASM ::: "memory");

    for (int tx = 0; tx < MIMO_N4; ++tx) {
        mimo_ext_dot4_sites_store_conj_soa_q15(
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
                mimo_ext_dot4_sites_store_real_soa_q15(
                    h_site4, h_site4, block, tx_i, tx_j, 1,
                    &work->a_re[upper][0], (int32_t *)0);
#else
                mimo_ext_dot4_sites_store_conj_soa_q15(
                    h_site4, h_site4, block, tx_i, tx_j, 1,
                    &work->a_re[upper][0], &work->a_im[upper][0],
                    (int32_t *)0, (int32_t *)0, 0);
#endif
            } else {
                mimo_ext_dot4_sites_store_conj_soa_q15(
                    h_site4, h_site4, block, tx_i, tx_j, 1,
                    &work->a_re[upper][0], &work->a_im[upper][0],
                    &work->a_re[lower][0], &work->a_im[lower][0], 1);
            }
        }
    }
}

static MIMO_ALWAYS_INLINE void mimo_ext_build_hermitian4_site4_upper_soa_q15(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    size_t block,
    mimo_site4_soa4_t *work)
{
#if !MIMO_USE_SITE4_PRELUDE_HOIST
    mimo_ext_site4_prelude_asm();
#endif

#if MIMO_USE_SITE4_HERMITIAN_UPPER_ROWPAIR
    mimo_ext_build_hermitian4_site4_upper_rowpair_soa_q15(
        h_site4, y_site4, block, work);
    return;
#endif

#if MIMO_USE_SITE4_HERMITIAN_UPPER_ROWFUSE
    mimo_ext_build_hermitian4_site4_upper_rowfuse_soa_q15(
        h_site4, y_site4, block, work);
    return;
#endif

    for (int tx = 0; tx < MIMO_N4; ++tx) {
        mimo_ext_dot4_sites_store_conj_soa_q15(
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
                mimo_ext_dot4_sites_store_real_soa_q15(
                    h_site4, h_site4, block, tx_i, tx_j, 1,
                    &work->a_re[upper][0], (int32_t *)0);
            } else {
                mimo_ext_dot4_sites_store_conj_soa_q15(
                    h_site4, h_site4, block, tx_i, tx_j, 1,
                    &work->a_re[upper][0], &work->a_im[upper][0],
                    (int32_t *)0, (int32_t *)0, 0);
            }
        }
    }
}

#if MIMO_USE_EXT_ZF4_LOCAL_SOLVE || MIMO_USE_EXT_ZF4_RVV_NODIV_SOLVE || \
    MIMO_USE_EXT_ZF4_RVV_NODIV_FAST_SOLVE
#if (MIMO_USE_EXT_ZF4_LOCAL_SOLVE || MIMO_USE_EXT_ZF4_RVV_NODIV_SOLVE) && \
    (!MIMO_USE_SOLVE4_HERMITIAN || !MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE || \
     !MIMO_USE_SOLVE4_HERMITIAN_Q15_ONECORR || \
     !MIMO_USE_SOLVE4_HERMITIAN_POS_PIVOT_SCALE || \
     !MIMO_USE_SOLVE4_HERMITIAN_TRUST_PIVOT_RANGE || \
     !MIMO_USE_SOLVE4_HERMITIAN_TRUST_PIVOT_NONZERO || \
     MIMO_USE_SOLVE4_HERMITIAN_RECIP_STEP16 || \
     MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED || \
     MIMO_COLLECT_PIVOT_SCALE_STATS || MIMO_COLLECT_SCHUR_STATS)
#error "EXT ZF4 local/RVV no-div solve only supports the current no-harddiv ZF4 SoA Hermitian path"
#endif
#if MIMO_USE_EXT_ZF4_RVV_NODIV_FAST_SOLVE && \
    (!MIMO_USE_SOLVE4_HERMITIAN || \
     !MIMO_USE_SOLVE4_HERMITIAN_POS_PIVOT_SCALE || \
     !MIMO_USE_SOLVE4_HERMITIAN_TRUST_PIVOT_RANGE || \
     !MIMO_USE_SOLVE4_HERMITIAN_TRUST_PIVOT_NONZERO || \
     MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED || \
     MIMO_COLLECT_PIVOT_SCALE_STATS || MIMO_COLLECT_SCHUR_STATS)
#error "EXT ZF4 RVV fast no-div solve requires trusted positive-range unpacked Hermitian SoA"
#endif

#define MIMO_EXT_ZF4_PIVOT_RECIP_MIN 10560u
#define MIMO_EXT_ZF4_PIVOT_RECIP_STEP_SHIFT 5u
#define MIMO_EXT_ZF4_PIVOT_RECIP_LAST 44u

static const uint32_t mimo_ext_zf4_recip_base_q30[] = {
    101680, 101373, 101068, 100764, 100462, 100162,
    99864, 99568, 99273, 98981, 98690, 98400,
    98112, 97826, 97542, 97259, 96978, 96699,
    96421, 96145, 95870, 95597, 95325, 95055,
    94787, 94520, 94254, 93990, 93727, 93466,
    93207, 92949, 92692, 92436, 92183, 91930,
    91679, 91429, 91181, 90933, 90688, 90443,
    90200, 89958, 89718,
};

static const int16_t mimo_ext_zf4_recip_delta_q30[] = {
    -307, -305, -304, -302, -300, -298,
    -296, -295, -292, -291, -290, -288,
    -286, -284, -283, -281, -279, -278,
    -276, -275, -273, -272, -270, -268,
    -267, -266, -264, -263, -261, -259,
    -258, -257, -256, -253, -253, -251,
    -250, -248, -248, -245, -245, -243,
    -242, -240, -30,
};

typedef struct {
    uint32_t mag_den;
    uint32_t scale_q30;
} mimo_ext_zf4_pivot_scale_t;

static MIMO_ALWAYS_INLINE mimo_c32_t mimo_ext_zf4_c32_make(int32_t r,
                                                           int32_t i)
{
    mimo_c32_t x = {r, i};
    return x;
}

static MIMO_ALWAYS_INLINE mimo_c32_t mimo_ext_zf4_c32_sub(mimo_c32_t a,
                                                          mimo_c32_t b)
{
    return mimo_ext_zf4_c32_make(a.r - b.r, a.i - b.i);
}

static MIMO_ALWAYS_INLINE int32_t mimo_ext_zf4_q15_round_shift_i32(int64_t x)
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

static MIMO_ALWAYS_INLINE int32_t mimo_ext_zf4_q30_round_shift_i32(int64_t x)
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

static MIMO_ALWAYS_INLINE int16_t mimo_ext_zf4_store_i16(int32_t x)
{
#if MIMO_USE_SOLVE_BRANCHLIGHT_ROUND
    return (int16_t)x;
#else
    if (x > 32767) {
        return 32767;
    }
    if (x < -32768) {
        return -32768;
    }
    return (int16_t)x;
#endif
}

static MIMO_ALWAYS_INLINE mimo_c32_t mimo_ext_zf4_c32_mul_q15(mimo_c32_t a,
                                                              mimo_c32_t b)
{
    return mimo_ext_zf4_c32_make(
        mimo_ext_zf4_q15_round_shift_i32((int64_t)a.r * b.r -
                                         (int64_t)a.i * b.i),
        mimo_ext_zf4_q15_round_shift_i32((int64_t)a.r * b.i +
                                         (int64_t)a.i * b.r));
}

static MIMO_ALWAYS_INLINE mimo_c32_t mimo_ext_zf4_c32_mul_q30_q15(
    mimo_c32_t a_q30,
    mimo_c32_t b_q15)
{
    return mimo_ext_zf4_c32_make(
        mimo_ext_zf4_q30_round_shift_i32((int64_t)a_q30.r * b_q15.r -
                                         (int64_t)a_q30.i * b_q15.i),
        mimo_ext_zf4_q30_round_shift_i32((int64_t)a_q30.r * b_q15.i +
                                         (int64_t)a_q30.i * b_q15.r));
}

static MIMO_ALWAYS_INLINE int32_t mimo_ext_zf4_c32_mul_q30_q15_real(
    mimo_c32_t a_q30,
    mimo_c32_t b_q15)
{
    return mimo_ext_zf4_q30_round_shift_i32((int64_t)a_q30.r * b_q15.r -
                                           (int64_t)a_q30.i * b_q15.i);
}

static MIMO_ALWAYS_INLINE uint32_t mimo_ext_zf4_recip_scale_q30_from_mag(
    uint32_t mag)
{
    const uint32_t rel = mag - MIMO_EXT_ZF4_PIVOT_RECIP_MIN;
    const uint32_t seg = rel >> MIMO_EXT_ZF4_PIVOT_RECIP_STEP_SHIFT;
    const uint32_t off =
        rel & ((1u << MIMO_EXT_ZF4_PIVOT_RECIP_STEP_SHIFT) - 1u);
    const uint32_t base = mimo_ext_zf4_recip_base_q30[seg];
    const uint32_t drop =
        (uint32_t)(-mimo_ext_zf4_recip_delta_q30[seg]) * off;

    if (seg == MIMO_EXT_ZF4_PIVOT_RECIP_LAST) {
        return base - ((drop + 2u) >> 2);
    }
    return base - ((drop + 16u) >> MIMO_EXT_ZF4_PIVOT_RECIP_STEP_SHIFT);
}

static MIMO_ALWAYS_INLINE mimo_ext_zf4_pivot_scale_t
mimo_ext_zf4_pivot_scale_init(int32_t den)
{
    mimo_ext_zf4_pivot_scale_t s;

    s.mag_den = (uint32_t)den;
    s.scale_q30 = mimo_ext_zf4_recip_scale_q30_from_mag(s.mag_den);
    return s;
}

static MIMO_ALWAYS_INLINE uint32_t mimo_ext_zf4_corr_delta(
    uint64_t residual,
    const mimo_ext_zf4_pivot_scale_t *scale)
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

static MIMO_ALWAYS_INLINE uint32_t mimo_ext_zf4_apply_jump_corr(
    uint32_t mag_q,
    uint64_t target,
    const mimo_ext_zf4_pivot_scale_t *scale)
{
    const uint64_t scaled = (uint64_t)mag_q * scale->mag_den;

    if (scaled > target) {
        const uint32_t delta =
            mimo_ext_zf4_corr_delta(scaled - target, scale);
        return delta >= mag_q ? 0 : mag_q - delta;
    }
    if (scaled < target) {
        const uint32_t delta =
            mimo_ext_zf4_corr_delta(target - scaled, scale);
        const uint32_t room = UINT32_MAX - mag_q;
        return mag_q + (delta > room ? room : delta);
    }
    return mag_q;
}

static MIMO_ALWAYS_INLINE int32_t mimo_ext_zf4_div_to_q30_scaled(
    int32_t num,
    const mimo_ext_zf4_pivot_scale_t *scale)
{
    const int32_t q =
        (int32_t)((int64_t)num * (int64_t)scale->scale_q30);
    const int neg = num < 0;
    uint32_t mag_num = num < 0 ? (uint32_t)(-num) : (uint32_t)num;
    uint32_t mag_q = q < 0 ? (uint32_t)(-q) : (uint32_t)q;
    const uint64_t target = (uint64_t)mag_num << 30;

    mag_q = mimo_ext_zf4_apply_jump_corr(mag_q, target, scale);
    while (mag_q > 0 && (uint64_t)mag_q * scale->mag_den > target) {
        --mag_q;
    }
    while ((uint64_t)(mag_q + 1u) * scale->mag_den <= target) {
        ++mag_q;
    }
    return neg ? -(int32_t)mag_q : (int32_t)mag_q;
}

#if MIMO_SOLVE4_HERMITIAN_Q30_JUMP_ONLY_MASK
static MIMO_ALWAYS_INLINE int32_t mimo_ext_zf4_div_to_q30_jump_only_scaled(
    int32_t num,
    const mimo_ext_zf4_pivot_scale_t *scale)
{
    const int32_t q =
        (int32_t)((int64_t)num * (int64_t)scale->scale_q30);
    const int neg = num < 0;
    uint32_t mag_num = num < 0 ? (uint32_t)(-num) : (uint32_t)num;
    uint32_t mag_q = q < 0 ? (uint32_t)(-q) : (uint32_t)q;
    const uint64_t target = (uint64_t)mag_num << 30;

    mag_q = mimo_ext_zf4_apply_jump_corr(mag_q, target, scale);
    return neg ? -(int32_t)mag_q : (int32_t)mag_q;
}

#define MIMO_EXT_ZF4_DIV_Q30_STAGE(NUM, SCALE, ID) \
    ((((MIMO_SOLVE4_HERMITIAN_Q30_JUMP_ONLY_MASK) >> (ID)) & 1) \
         ? mimo_ext_zf4_div_to_q30_jump_only_scaled((NUM), (SCALE)) \
         : mimo_ext_zf4_div_to_q30_scaled((NUM), (SCALE)))
#else
#define MIMO_EXT_ZF4_DIV_Q30_STAGE(NUM, SCALE, ID) \
    mimo_ext_zf4_div_to_q30_scaled((NUM), (SCALE))
#endif

static MIMO_ALWAYS_INLINE int32_t mimo_ext_zf4_div_to_q15_scaled(
    int32_t num,
    const mimo_ext_zf4_pivot_scale_t *scale)
{
    const int32_t q =
        mimo_ext_zf4_q15_round_shift_i32((int64_t)num *
                                         (int64_t)scale->scale_q30);
    const int neg = num < 0;
    uint32_t mag_num = num < 0 ? (uint32_t)(-num) : (uint32_t)num;
    uint32_t mag_q = q < 0 ? (uint32_t)(-q) : (uint32_t)q;
    const uint64_t target = (uint64_t)mag_num << MIMO_Q15_SHIFT;

    mag_q = mimo_ext_zf4_apply_jump_corr(mag_q, target, scale);
    if (mag_q > 0 && (uint64_t)mag_q * scale->mag_den > target) {
        --mag_q;
    }
    if ((uint64_t)(mag_q + 1u) * scale->mag_den <= target) {
        ++mag_q;
    }
    return neg ? -(int32_t)mag_q : (int32_t)mag_q;
}

#if MIMO_SOLVE4_HERMITIAN_Q15_JUMP_ONLY_MASK
static MIMO_ALWAYS_INLINE int32_t mimo_ext_zf4_div_to_q15_jump_only_scaled(
    int32_t num,
    const mimo_ext_zf4_pivot_scale_t *scale)
{
    const int32_t q =
        mimo_ext_zf4_q15_round_shift_i32((int64_t)num *
                                         (int64_t)scale->scale_q30);
    const int neg = num < 0;
    uint32_t mag_num = num < 0 ? (uint32_t)(-num) : (uint32_t)num;
    uint32_t mag_q = q < 0 ? (uint32_t)(-q) : (uint32_t)q;
    const uint64_t target = (uint64_t)mag_num << MIMO_Q15_SHIFT;

    mag_q = mimo_ext_zf4_apply_jump_corr(mag_q, target, scale);
    return neg ? -(int32_t)mag_q : (int32_t)mag_q;
}

#define MIMO_EXT_ZF4_DIV_Q15_STAGE(NUM, SCALE, ID) \
    ((((MIMO_SOLVE4_HERMITIAN_Q15_JUMP_ONLY_MASK) >> (ID)) & 1) \
         ? mimo_ext_zf4_div_to_q15_jump_only_scaled((NUM), (SCALE)) \
         : mimo_ext_zf4_div_to_q15_scaled((NUM), (SCALE)))
#else
#define MIMO_EXT_ZF4_DIV_Q15_STAGE(NUM, SCALE, ID) \
    mimo_ext_zf4_div_to_q15_scaled((NUM), (SCALE))
#endif

static MIMO_ALWAYS_INLINE mimo_c32_t mimo_ext_zf4_factor_q30(
    mimo_c32_t a,
    const mimo_ext_zf4_pivot_scale_t *scale)
{
    return mimo_ext_zf4_c32_make(
        mimo_ext_zf4_div_to_q30_scaled(a.r, scale),
        mimo_ext_zf4_div_to_q30_scaled(-a.i, scale));
}

static MIMO_ALWAYS_INLINE mimo_c32_t mimo_ext_zf4_factor_q30_stage(
    mimo_c32_t a,
    const mimo_ext_zf4_pivot_scale_t *scale,
    int stage)
{
    return mimo_ext_zf4_c32_make(
        MIMO_EXT_ZF4_DIV_Q30_STAGE(a.r, scale, stage),
        MIMO_EXT_ZF4_DIV_Q30_STAGE(-a.i, scale, stage));
}

static MIMO_ALWAYS_INLINE mimo_c32_t mimo_ext_zf4_div_real_to_q15(
    mimo_c32_t a,
    const mimo_ext_zf4_pivot_scale_t *scale)
{
    return mimo_ext_zf4_c32_make(
        mimo_ext_zf4_div_to_q15_scaled(a.r, scale),
        mimo_ext_zf4_div_to_q15_scaled(a.i, scale));
}

static MIMO_ALWAYS_INLINE mimo_c32_t mimo_ext_zf4_div_real_to_q15_stage(
    mimo_c32_t a,
    const mimo_ext_zf4_pivot_scale_t *scale,
    int stage)
{
    return mimo_ext_zf4_c32_make(
        MIMO_EXT_ZF4_DIV_Q15_STAGE(a.r, scale, stage),
        MIMO_EXT_ZF4_DIV_Q15_STAGE(a.i, scale, stage));
}

#if MIMO_USE_EXT_ZF4_RVV_NODIV_SOLVE && \
    MIMO_DEBUG_EXT_ZF4_RVV_NODIV_COMPARE
static MIMO_ALWAYS_INLINE void mimo_ext_zf4_local_solve_soa_q15(
    mimo_site4_soa4_t *restrict work,
    mimo_c16_t *restrict x_block);
#endif

#if MIMO_USE_EXT_ZF4_RVV_NODIV_SOLVE
static MIMO_ALWAYS_INLINE void mimo_ext_zf4_rvv_init_scales(
    vint32m1_t pivot,
    mimo_ext_zf4_pivot_scale_t scale[MIMO_SITE4_LANES],
    size_t vl)
{
    int32_t den[MIMO_SITE4_LANES] MIMO_ALIGN64;

    __riscv_vse32_v_i32m1(den, pivot, vl);
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        scale[lane] = mimo_ext_zf4_pivot_scale_init(den[lane]);
    }
}

static MIMO_ALWAYS_INLINE vint32m1_t mimo_ext_zf4_rvv_div_q30_lanes(
    vint32m1_t num,
    const mimo_ext_zf4_pivot_scale_t scale[MIMO_SITE4_LANES],
    int stage,
    size_t vl)
{
    int32_t in[MIMO_SITE4_LANES] MIMO_ALIGN64;
    int32_t out[MIMO_SITE4_LANES] MIMO_ALIGN64;

    __riscv_vse32_v_i32m1(in, num, vl);
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        out[lane] = MIMO_EXT_ZF4_DIV_Q30_STAGE(
            in[lane], &scale[lane], stage);
    }
    return __riscv_vle32_v_i32m1(out, vl);
}

static MIMO_ALWAYS_INLINE vint32m1_t mimo_ext_zf4_rvv_div_q15_lanes(
    vint32m1_t num,
    const mimo_ext_zf4_pivot_scale_t scale[MIMO_SITE4_LANES],
    int stage,
    size_t vl)
{
    int32_t in[MIMO_SITE4_LANES] MIMO_ALIGN64;
    int32_t out[MIMO_SITE4_LANES] MIMO_ALIGN64;

    __riscv_vse32_v_i32m1(in, num, vl);
    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        out[lane] = MIMO_EXT_ZF4_DIV_Q15_STAGE(
            in[lane], &scale[lane], stage);
    }
    return __riscv_vle32_v_i32m1(out, vl);
}

static MIMO_ALWAYS_INLINE void mimo_ext_zf4_rvv_factor_q30(
    vint32m1_t a_re,
    vint32m1_t a_im,
    const mimo_ext_zf4_pivot_scale_t scale[MIMO_SITE4_LANES],
    vint32m1_t *f_re,
    vint32m1_t *f_im,
    int stage,
    size_t vl)
{
    const vint32m1_t zero = __riscv_vmv_v_x_i32m1(0, vl);

    *f_re = mimo_ext_zf4_rvv_div_q30_lanes(a_re, scale, stage, vl);
    *f_im = mimo_ext_zf4_rvv_div_q30_lanes(
        __riscv_vsub_vv_i32m1(zero, a_im, vl), scale, stage, vl);
}

static MIMO_ALWAYS_INLINE void mimo_ext_zf4_rvv_div_q15_pair(
    vint32m1_t sum_re,
    vint32m1_t sum_im,
    const mimo_ext_zf4_pivot_scale_t scale[MIMO_SITE4_LANES],
    vint32m1_t *x_re,
    vint32m1_t *x_im,
    int stage,
    size_t vl)
{
    *x_re = mimo_ext_zf4_rvv_div_q15_lanes(sum_re, scale, stage, vl);
    *x_im = mimo_ext_zf4_rvv_div_q15_lanes(sum_im, scale, stage, vl);
}

static MIMO_NOINLINE void mimo_ext_zf4_rvv_nodiv_solve_soa_q15(
    const mimo_site4_soa4_t *restrict work,
    mimo_c16_t *restrict x_block)
{
    const size_t vl = __riscv_vsetvl_e32m1(MIMO_SITE4_LANES);
    mimo_ext_zf4_pivot_scale_t s0[MIMO_SITE4_LANES];
    mimo_ext_zf4_pivot_scale_t s1[MIMO_SITE4_LANES];
    mimo_ext_zf4_pivot_scale_t s2[MIMO_SITE4_LANES];
    mimo_ext_zf4_pivot_scale_t s3[MIMO_SITE4_LANES];
    const vint32m1_t zero = __riscv_vmv_v_x_i32m1(0, vl);

    vint32m1_t m00r = mimo_rvv_solve4_load_soa_a_re(work, 0, vl);
    vint32m1_t m01r = mimo_rvv_solve4_load_soa_a_re(work, 1, vl);
    vint32m1_t m01i = mimo_rvv_solve4_load_soa_a_im(work, 1, vl);
    vint32m1_t m02r = mimo_rvv_solve4_load_soa_a_re(work, 2, vl);
    vint32m1_t m02i = mimo_rvv_solve4_load_soa_a_im(work, 2, vl);
    vint32m1_t m03r = mimo_rvv_solve4_load_soa_a_re(work, 3, vl);
    vint32m1_t m03i = mimo_rvv_solve4_load_soa_a_im(work, 3, vl);
    vint32m1_t m11r = mimo_rvv_solve4_load_soa_a_re(work, 5, vl);
    vint32m1_t m11i = zero;
    vint32m1_t m12r = mimo_rvv_solve4_load_soa_a_re(work, 6, vl);
    vint32m1_t m12i = mimo_rvv_solve4_load_soa_a_im(work, 6, vl);
    vint32m1_t m13r = mimo_rvv_solve4_load_soa_a_re(work, 7, vl);
    vint32m1_t m13i = mimo_rvv_solve4_load_soa_a_im(work, 7, vl);
    vint32m1_t m22r = mimo_rvv_solve4_load_soa_a_re(work, 10, vl);
    vint32m1_t m22i = zero;
    vint32m1_t m23r = mimo_rvv_solve4_load_soa_a_re(work, 11, vl);
    vint32m1_t m23i = mimo_rvv_solve4_load_soa_a_im(work, 11, vl);
    vint32m1_t m33r = mimo_rvv_solve4_load_soa_a_re(work, 15, vl);
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
    const ptrdiff_t x_stride = (ptrdiff_t)(MIMO_N4 * sizeof(mimo_c16_t));

    mimo_ext_zf4_rvv_init_scales(m00r, s0, vl);
    mimo_ext_zf4_rvv_factor_q30(m01r, m01i, s0, &f_re, &f_im, 0, vl);
    mimo_rvv_solve4_submul_q30_q15(&m11r, &m11i, m01r, m01i, f_re, f_im, vl);
    mimo_rvv_solve4_submul_q30_q15(&m12r, &m12i, m02r, m02i, f_re, f_im, vl);
    mimo_rvv_solve4_submul_q30_q15(&m13r, &m13i, m03r, m03i, f_re, f_im, vl);
    mimo_rvv_solve4_submul_q30_q15(&r1r, &r1i, r0r, r0i, f_re, f_im, vl);

    mimo_ext_zf4_rvv_factor_q30(m02r, m02i, s0, &f_re, &f_im, 1, vl);
    mimo_rvv_solve4_submul_q30_q15(&m22r, &m22i, m02r, m02i, f_re, f_im, vl);
    mimo_rvv_solve4_submul_q30_q15(&m23r, &m23i, m03r, m03i, f_re, f_im, vl);
    mimo_rvv_solve4_submul_q30_q15(&r2r, &r2i, r0r, r0i, f_re, f_im, vl);

    mimo_ext_zf4_rvv_factor_q30(m03r, m03i, s0, &f_re, &f_im, 2, vl);
    mimo_rvv_solve4_submul_q30_q15(&m33r, &m33i, m03r, m03i, f_re, f_im, vl);
    mimo_rvv_solve4_submul_q30_q15(&r3r, &r3i, r0r, r0i, f_re, f_im, vl);

    mimo_ext_zf4_rvv_init_scales(m11r, s1, vl);
    mimo_ext_zf4_rvv_factor_q30(m12r, m12i, s1, &f_re, &f_im, 3, vl);
    mimo_rvv_solve4_submul_q30_q15(&m22r, &m22i, m12r, m12i, f_re, f_im, vl);
    mimo_rvv_solve4_submul_q30_q15(&m23r, &m23i, m13r, m13i, f_re, f_im, vl);
    mimo_rvv_solve4_submul_q30_q15(&r2r, &r2i, r1r, r1i, f_re, f_im, vl);

    mimo_ext_zf4_rvv_factor_q30(m13r, m13i, s1, &f_re, &f_im, 4, vl);
    mimo_rvv_solve4_submul_q30_q15(&m33r, &m33i, m13r, m13i, f_re, f_im, vl);
    mimo_rvv_solve4_submul_q30_q15(&r3r, &r3i, r1r, r1i, f_re, f_im, vl);

    mimo_ext_zf4_rvv_init_scales(m22r, s2, vl);
    mimo_ext_zf4_rvv_factor_q30(m23r, m23i, s2, &f_re, &f_im, 5, vl);
    mimo_rvv_solve4_submul_q30_q15(&m33r, &m33i, m23r, m23i, f_re, f_im, vl);
    mimo_rvv_solve4_submul_q30_q15(&r3r, &r3i, r2r, r2i, f_re, f_im, vl);

    mimo_ext_zf4_rvv_init_scales(m33r, s3, vl);
    mimo_ext_zf4_rvv_div_q15_pair(r3r, r3i, s3, &x3r, &x3i, 0, vl);
    mimo_rvv_solve4_submul_q15(&r2r, &r2i, m23r, m23i, x3r, x3i, vl);
    mimo_ext_zf4_rvv_div_q15_pair(r2r, r2i, s2, &x2r, &x2i, 1, vl);

    mimo_rvv_solve4_submul_q15(&r1r, &r1i, m12r, m12i, x2r, x2i, vl);
    mimo_rvv_solve4_submul_q15(&r1r, &r1i, m13r, m13i, x3r, x3i, vl);
    mimo_ext_zf4_rvv_div_q15_pair(r1r, r1i, s1, &x1r, &x1i, 2, vl);

    mimo_rvv_solve4_submul_q15(&r0r, &r0i, m01r, m01i, x1r, x1i, vl);
    mimo_rvv_solve4_submul_q15(&r0r, &r0i, m02r, m02i, x2r, x2i, vl);
    mimo_rvv_solve4_submul_q15(&r0r, &r0i, m03r, m03i, x3r, x3i, vl);
    mimo_ext_zf4_rvv_div_q15_pair(r0r, r0i, s0, &x0r, &x0i, 3, vl);

#if MIMO_DEBUG_EXT_ZF4_RVV_NODIV_COMPARE
    {
        static int printed = 0;

        if (!printed) {
            printed = 1;
            mimo_site4_soa4_t scalar_work = *work;
            mimo_c16_t scalar_x[MIMO_SITE4_LANES * MIMO_N4] MIMO_ALIGN64;
            int32_t rvv_re[MIMO_N4][MIMO_SITE4_LANES] MIMO_ALIGN64;
            int32_t rvv_im[MIMO_N4][MIMO_SITE4_LANES] MIMO_ALIGN64;

            __riscv_vse32_v_i32m1(&rvv_re[0][0], x0r, vl);
            __riscv_vse32_v_i32m1(&rvv_im[0][0], x0i, vl);
            __riscv_vse32_v_i32m1(&rvv_re[1][0], x1r, vl);
            __riscv_vse32_v_i32m1(&rvv_im[1][0], x1i, vl);
            __riscv_vse32_v_i32m1(&rvv_re[2][0], x2r, vl);
            __riscv_vse32_v_i32m1(&rvv_im[2][0], x2i, vl);
            __riscv_vse32_v_i32m1(&rvv_re[3][0], x3r, vl);
            __riscv_vse32_v_i32m1(&rvv_im[3][0], x3i, vl);
            mimo_ext_zf4_local_solve_soa_q15(&scalar_work, scalar_x);
            for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
                for (int tx = 0; tx < MIMO_N4; ++tx) {
                    const int16_t rr =
                        mimo_ext_zf4_store_i16(rvv_re[tx][lane]);
                    const int16_t ri =
                        mimo_ext_zf4_store_i16(rvv_im[tx][lane]);

                    printf("MIMO_DEBUG_EXT_ZF4_RVV_NODIV_COMPARE "
                           "lane=%d tx=%d scalar=(%d,%d) rvv=(%d,%d) "
                           "raw=(%d,%d)\n",
                           lane, tx,
                           scalar_x[lane * MIMO_N4 + tx].r,
                           scalar_x[lane * MIMO_N4 + tx].i,
                           rr, ri,
                           rvv_re[tx][lane], rvv_im[tx][lane]);
                }
            }
        }
    }
#endif

    mimo_rvv_solve4_store_x(x_block, 0, x0r, x0i, x_stride, vl);
    mimo_rvv_solve4_store_x(x_block, 1, x1r, x1i, x_stride, vl);
    mimo_rvv_solve4_store_x(x_block, 2, x2r, x2i, x_stride, vl);
    mimo_rvv_solve4_store_x(x_block, 3, x3r, x3i, x_stride, vl);
}
#endif

#if MIMO_USE_EXT_ZF4_RVV_NODIV_FAST_SOLVE
#ifndef MIMO_EXT_ZF4_FAST_CORR_ITERS
#define MIMO_EXT_ZF4_FAST_CORR_ITERS 8
#endif
#ifndef MIMO_EXT_ZF4_FAST_SCALE_BIAS_Q30
#define MIMO_EXT_ZF4_FAST_SCALE_BIAS_Q30 0
#endif
#ifndef MIMO_EXT_ZF4_FAST_REUSE_SCALE
#define MIMO_EXT_ZF4_FAST_REUSE_SCALE 0
#endif
#define MIMO_EXT_ZF4_FAST_RECIP_BASE_Q30 101680
#define MIMO_EXT_ZF4_FAST_RECIP_SLOPE_Q30 2171
#define MIMO_EXT_ZF4_FAST_RECIP_SLOPE_SHIFT 8
#define MIMO_EXT_ZF4_FAST_RECIP_ROUND \
    (1 << (MIMO_EXT_ZF4_FAST_RECIP_SLOPE_SHIFT - 1))

static MIMO_ALWAYS_INLINE vint32m1_t mimo_ext_zf4_rvv_fast_scale_q30(
    vint32m1_t pivot,
    size_t vl)
{
    const vint32m1_t min_den =
        __riscv_vmv_v_x_i32m1((int32_t)MIMO_EXT_ZF4_PIVOT_RECIP_MIN, vl);
    const vint32m1_t base =
        __riscv_vmv_v_x_i32m1(MIMO_EXT_ZF4_FAST_RECIP_BASE_Q30, vl);
    vint32m1_t rel = __riscv_vsub_vv_i32m1(pivot, min_den, vl);
    vint32m1_t drop =
        __riscv_vmul_vx_i32m1(rel, MIMO_EXT_ZF4_FAST_RECIP_SLOPE_Q30, vl);

    drop = __riscv_vadd_vx_i32m1(
        drop, MIMO_EXT_ZF4_FAST_RECIP_ROUND, vl);
    drop = __riscv_vsra_vx_i32m1(
        drop, MIMO_EXT_ZF4_FAST_RECIP_SLOPE_SHIFT, vl);
    vint32m1_t scale = __riscv_vsub_vv_i32m1(base, drop, vl);
    const vint64m2_t prod = __riscv_vwmul_vv_i64m2(pivot, scale, vl);
    const vint64m2_t two_q30 = __riscv_vmv_v_x_i64m2(2LL << 30, vl);
    const vint64m2_t scale64 = __riscv_vsext_vf2_i64m2(scale, vl);
    vint64m2_t refined =
        __riscv_vmul_vv_i64m2(scale64,
                              __riscv_vsub_vv_i64m2(two_q30, prod, vl),
                              vl);

    refined = __riscv_vadd_vx_i64m2(refined, 1LL << 29, vl);
    refined = __riscv_vsra_vx_i64m2(refined, 30, vl);
    scale = __riscv_vncvt_x_x_w_i32m1(refined, vl);

    const vint64m2_t prod2 = __riscv_vwmul_vv_i64m2(pivot, scale, vl);
    const vint64m2_t scale64_2 = __riscv_vsext_vf2_i64m2(scale, vl);
    refined = __riscv_vmul_vv_i64m2(
        scale64_2, __riscv_vsub_vv_i64m2(two_q30, prod2, vl), vl);
    refined = __riscv_vadd_vx_i64m2(refined, 1LL << 29, vl);
    refined = __riscv_vsra_vx_i64m2(refined, 30, vl);
    scale = __riscv_vncvt_x_x_w_i32m1(refined, vl);
#if MIMO_EXT_ZF4_FAST_SCALE_BIAS_Q30
    scale = __riscv_vadd_vx_i32m1(
        scale, MIMO_EXT_ZF4_FAST_SCALE_BIAS_Q30, vl);
#endif
    return scale;
}

static MIMO_ALWAYS_INLINE vint32m1_t mimo_ext_zf4_rvv_fast_correct_trunc(
    vint32m1_t num,
    vint32m1_t pivot,
    vint32m1_t q,
    int shift,
    size_t vl)
{
#if MIMO_EXT_ZF4_FAST_CORR_ITERS == 0
    (void)num;
    (void)pivot;
    (void)shift;
    (void)vl;
    return q;
#else
    const vint32m1_t sign = __riscv_vsra_vx_i32m1(num, 31, vl);
    const vint32m1_t zero = __riscv_vmv_v_x_i32m1(0, vl);
    vint32m1_t mag_num = __riscv_vxor_vv_i32m1(num, sign, vl);
    vint32m1_t mag_q = __riscv_vsra_vx_i32m1(q, 31, vl);
    const vbool32_t neg_mask = __riscv_vmsne_vx_i32m1_b32(sign, 0, vl);

    mag_num = __riscv_vsub_vv_i32m1(mag_num, sign, vl);
    mag_q = __riscv_vsub_vv_i32m1(
        __riscv_vxor_vv_i32m1(q, mag_q, vl), mag_q, vl);

    const vint64m2_t target =
        __riscv_vsll_vx_i64m2(__riscv_vsext_vf2_i64m2(mag_num, vl),
                              (uint8_t)shift, vl);
    const vint64m2_t pivot64 = __riscv_vsext_vf2_i64m2(pivot, vl);
    vint64m2_t scaled = __riscv_vwmul_vv_i64m2(mag_q, pivot, vl);

    for (int iter = 0; iter < MIMO_EXT_ZF4_FAST_CORR_ITERS; ++iter) {
        const vbool32_t dec_mask =
            __riscv_vmsgt_vv_i64m2_b32(scaled, target, vl);
        mag_q = __riscv_vmerge_vvm_i32m1(
            mag_q, __riscv_vsub_vx_i32m1(mag_q, 1, vl), dec_mask, vl);
        scaled = __riscv_vmerge_vvm_i64m2(
            scaled, __riscv_vsub_vv_i64m2(scaled, pivot64, vl), dec_mask, vl);

        const vint64m2_t scaled_next =
            __riscv_vadd_vv_i64m2(scaled, pivot64, vl);
        const vbool32_t inc_mask =
            __riscv_vmsle_vv_i64m2_b32(scaled_next, target, vl);
        mag_q = __riscv_vmerge_vvm_i32m1(
            mag_q, __riscv_vadd_vx_i32m1(mag_q, 1, vl), inc_mask, vl);
        scaled = __riscv_vmerge_vvm_i64m2(
            scaled, scaled_next, inc_mask, vl);
    }

    return __riscv_vmerge_vvm_i32m1(
        mag_q, __riscv_vsub_vv_i32m1(zero, mag_q, vl), neg_mask, vl);
#endif
}

static MIMO_ALWAYS_INLINE vint32m1_t mimo_ext_zf4_rvv_fast_div_q30(
    vint32m1_t num,
    vint32m1_t pivot,
    vint32m1_t scale_q30,
    size_t vl)
{
    const vint64m2_t prod = __riscv_vwmul_vv_i64m2(num, scale_q30, vl);

    return mimo_ext_zf4_rvv_fast_correct_trunc(
        num, pivot, __riscv_vncvt_x_x_w_i32m1(prod, vl), 30, vl);
}

static MIMO_ALWAYS_INLINE vint32m1_t mimo_ext_zf4_rvv_fast_div_q15(
    vint32m1_t num,
    vint32m1_t pivot,
    vint32m1_t scale_q30,
    size_t vl)
{
    const vint64m2_t prod = __riscv_vwmul_vv_i64m2(num, scale_q30, vl);
    const vint32m1_t q = mimo_rvv_solve4_round_shift_i64_to_i32(
        prod, MIMO_Q15_SHIFT, 0, vl);

    return mimo_ext_zf4_rvv_fast_correct_trunc(
        num, pivot, q, MIMO_Q15_SHIFT, vl);
}

static MIMO_ALWAYS_INLINE void mimo_ext_zf4_rvv_fast_factor_q30(
    vint32m1_t a_re,
    vint32m1_t a_im,
    vint32m1_t pivot,
    vint32m1_t scale_q30,
    vint32m1_t *f_re,
    vint32m1_t *f_im,
    size_t vl)
{
    const vint32m1_t zero = __riscv_vmv_v_x_i32m1(0, vl);

    *f_re = mimo_ext_zf4_rvv_fast_div_q30(a_re, pivot, scale_q30, vl);
    *f_im = mimo_ext_zf4_rvv_fast_div_q30(
        __riscv_vsub_vv_i32m1(zero, a_im, vl), pivot, scale_q30, vl);
}

static MIMO_ALWAYS_INLINE void mimo_ext_zf4_rvv_fast_div_q15_pair(
    vint32m1_t sum_re,
    vint32m1_t sum_im,
    vint32m1_t pivot,
    vint32m1_t scale_q30,
    vint32m1_t *x_re,
    vint32m1_t *x_im,
    size_t vl)
{
    *x_re = mimo_ext_zf4_rvv_fast_div_q15(sum_re, pivot, scale_q30, vl);
    *x_im = mimo_ext_zf4_rvv_fast_div_q15(sum_im, pivot, scale_q30, vl);
}

static MIMO_NOINLINE void mimo_ext_zf4_rvv_nodiv_fast_solve_soa_q15(
    const mimo_site4_soa4_t *restrict work,
    mimo_c16_t *restrict x_block)
{
    const size_t vl = __riscv_vsetvl_e32m1(MIMO_SITE4_LANES);
    const ptrdiff_t x_stride = (ptrdiff_t)(MIMO_N4 * sizeof(mimo_c16_t));
    const vint32m1_t zero = __riscv_vmv_v_x_i32m1(0, vl);

    vint32m1_t m00r = mimo_rvv_solve4_load_soa_a_re(work, 0, vl);
    vint32m1_t m01r = mimo_rvv_solve4_load_soa_a_re(work, 1, vl);
    vint32m1_t m01i = mimo_rvv_solve4_load_soa_a_im(work, 1, vl);
    vint32m1_t m02r = mimo_rvv_solve4_load_soa_a_re(work, 2, vl);
    vint32m1_t m02i = mimo_rvv_solve4_load_soa_a_im(work, 2, vl);
    vint32m1_t m03r = mimo_rvv_solve4_load_soa_a_re(work, 3, vl);
    vint32m1_t m03i = mimo_rvv_solve4_load_soa_a_im(work, 3, vl);
    vint32m1_t m11r = mimo_rvv_solve4_load_soa_a_re(work, 5, vl);
    vint32m1_t m11i = zero;
    vint32m1_t m12r = mimo_rvv_solve4_load_soa_a_re(work, 6, vl);
    vint32m1_t m12i = mimo_rvv_solve4_load_soa_a_im(work, 6, vl);
    vint32m1_t m13r = mimo_rvv_solve4_load_soa_a_re(work, 7, vl);
    vint32m1_t m13i = mimo_rvv_solve4_load_soa_a_im(work, 7, vl);
    vint32m1_t m22r = mimo_rvv_solve4_load_soa_a_re(work, 10, vl);
    vint32m1_t m22i = zero;
    vint32m1_t m23r = mimo_rvv_solve4_load_soa_a_re(work, 11, vl);
    vint32m1_t m23i = mimo_rvv_solve4_load_soa_a_im(work, 11, vl);
    vint32m1_t m33r = mimo_rvv_solve4_load_soa_a_re(work, 15, vl);
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

    vint32m1_t scale0 = mimo_ext_zf4_rvv_fast_scale_q30(m00r, vl);
    mimo_ext_zf4_rvv_fast_factor_q30(
        m01r, m01i, m00r, scale0, &f_re, &f_im, vl);
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
    mimo_rvv_solve4_submul_q30_q15_real(&m11r, m01r, m01i, f_re, f_im, vl);
#else
    mimo_rvv_solve4_submul_q30_q15(&m11r, &m11i, m01r, m01i, f_re, f_im, vl);
#endif
    mimo_rvv_solve4_submul_q30_q15(&m12r, &m12i, m02r, m02i, f_re, f_im, vl);
    mimo_rvv_solve4_submul_q30_q15(&m13r, &m13i, m03r, m03i, f_re, f_im, vl);
    mimo_rvv_solve4_submul_q30_q15(&r1r, &r1i, r0r, r0i, f_re, f_im, vl);

    mimo_ext_zf4_rvv_fast_factor_q30(
        m02r, m02i, m00r, scale0, &f_re, &f_im, vl);
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
    mimo_rvv_solve4_submul_q30_q15_real(&m22r, m02r, m02i, f_re, f_im, vl);
#else
    mimo_rvv_solve4_submul_q30_q15(&m22r, &m22i, m02r, m02i, f_re, f_im, vl);
#endif
    mimo_rvv_solve4_submul_q30_q15(&m23r, &m23i, m03r, m03i, f_re, f_im, vl);
    mimo_rvv_solve4_submul_q30_q15(&r2r, &r2i, r0r, r0i, f_re, f_im, vl);

    mimo_ext_zf4_rvv_fast_factor_q30(
        m03r, m03i, m00r, scale0, &f_re, &f_im, vl);
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
    mimo_rvv_solve4_submul_q30_q15_real(&m33r, m03r, m03i, f_re, f_im, vl);
#else
    mimo_rvv_solve4_submul_q30_q15(&m33r, &m33i, m03r, m03i, f_re, f_im, vl);
#endif
    mimo_rvv_solve4_submul_q30_q15(&r3r, &r3i, r0r, r0i, f_re, f_im, vl);

    vint32m1_t scale1 = mimo_ext_zf4_rvv_fast_scale_q30(m11r, vl);
    mimo_ext_zf4_rvv_fast_factor_q30(
        m12r, m12i, m11r, scale1, &f_re, &f_im, vl);
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
    mimo_rvv_solve4_submul_q30_q15_real(&m22r, m12r, m12i, f_re, f_im, vl);
#else
    mimo_rvv_solve4_submul_q30_q15(&m22r, &m22i, m12r, m12i, f_re, f_im, vl);
#endif
    mimo_rvv_solve4_submul_q30_q15(&m23r, &m23i, m13r, m13i, f_re, f_im, vl);
    mimo_rvv_solve4_submul_q30_q15(&r2r, &r2i, r1r, r1i, f_re, f_im, vl);

    mimo_ext_zf4_rvv_fast_factor_q30(
        m13r, m13i, m11r, scale1, &f_re, &f_im, vl);
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
    mimo_rvv_solve4_submul_q30_q15_real(&m33r, m13r, m13i, f_re, f_im, vl);
#else
    mimo_rvv_solve4_submul_q30_q15(&m33r, &m33i, m13r, m13i, f_re, f_im, vl);
#endif
    mimo_rvv_solve4_submul_q30_q15(&r3r, &r3i, r1r, r1i, f_re, f_im, vl);

    vint32m1_t scale2 = mimo_ext_zf4_rvv_fast_scale_q30(m22r, vl);
    mimo_ext_zf4_rvv_fast_factor_q30(
        m23r, m23i, m22r, scale2, &f_re, &f_im, vl);
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
    mimo_rvv_solve4_submul_q30_q15_real(&m33r, m23r, m23i, f_re, f_im, vl);
#else
    mimo_rvv_solve4_submul_q30_q15(&m33r, &m33i, m23r, m23i, f_re, f_im, vl);
#endif
    mimo_rvv_solve4_submul_q30_q15(&r3r, &r3i, r2r, r2i, f_re, f_im, vl);

    vint32m1_t scale3 = mimo_ext_zf4_rvv_fast_scale_q30(m33r, vl);
    mimo_ext_zf4_rvv_fast_div_q15_pair(
        r3r, r3i, m33r, scale3, &x3r, &x3i, vl);
    mimo_rvv_solve4_submul_q15(&r2r, &r2i, m23r, m23i, x3r, x3i, vl);

#if !MIMO_EXT_ZF4_FAST_REUSE_SCALE
    scale2 = mimo_ext_zf4_rvv_fast_scale_q30(m22r, vl);
#endif
    mimo_ext_zf4_rvv_fast_div_q15_pair(
        r2r, r2i, m22r, scale2, &x2r, &x2i, vl);

    mimo_rvv_solve4_submul_q15(&r1r, &r1i, m12r, m12i, x2r, x2i, vl);
    mimo_rvv_solve4_submul_q15(&r1r, &r1i, m13r, m13i, x3r, x3i, vl);
#if !MIMO_EXT_ZF4_FAST_REUSE_SCALE
    scale1 = mimo_ext_zf4_rvv_fast_scale_q30(m11r, vl);
#endif
    mimo_ext_zf4_rvv_fast_div_q15_pair(
        r1r, r1i, m11r, scale1, &x1r, &x1i, vl);

    mimo_rvv_solve4_submul_q15(&r0r, &r0i, m01r, m01i, x1r, x1i, vl);
    mimo_rvv_solve4_submul_q15(&r0r, &r0i, m02r, m02i, x2r, x2i, vl);
    mimo_rvv_solve4_submul_q15(&r0r, &r0i, m03r, m03i, x3r, x3i, vl);
#if !MIMO_EXT_ZF4_FAST_REUSE_SCALE
    scale0 = mimo_ext_zf4_rvv_fast_scale_q30(m00r, vl);
#endif
    mimo_ext_zf4_rvv_fast_div_q15_pair(
        r0r, r0i, m00r, scale0, &x0r, &x0i, vl);

    mimo_rvv_solve4_store_x(x_block, 0, x0r, x0i, x_stride, vl);
    mimo_rvv_solve4_store_x(x_block, 1, x1r, x1i, x_stride, vl);
    mimo_rvv_solve4_store_x(x_block, 2, x2r, x2i, x_stride, vl);
    mimo_rvv_solve4_store_x(x_block, 3, x3r, x3i, x_stride, vl);
}

#undef MIMO_EXT_ZF4_FAST_RECIP_BASE_Q30
#undef MIMO_EXT_ZF4_FAST_RECIP_SLOPE_Q30
#undef MIMO_EXT_ZF4_FAST_RECIP_SLOPE_SHIFT
#undef MIMO_EXT_ZF4_FAST_RECIP_ROUND
#undef MIMO_EXT_ZF4_FAST_CORR_ITERS
#undef MIMO_EXT_ZF4_FAST_SCALE_BIAS_Q30
#undef MIMO_EXT_ZF4_FAST_REUSE_SCALE
#endif

static MIMO_ALWAYS_INLINE void mimo_ext_zf4_local_solve_soa_q15(
    mimo_site4_soa4_t *restrict work,
    mimo_c16_t *restrict x_block)
{
    int32_t (*restrict a_re)[MIMO_SITE4_LANES] = work->a_re;
    int32_t (*restrict a_im)[MIMO_SITE4_LANES] = work->a_im;
    int32_t (*restrict b_re)[MIMO_SITE4_LANES] = work->b_re;
    int32_t (*restrict b_im)[MIMO_SITE4_LANES] = work->b_im;

    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        mimo_c32_t m00 = mimo_ext_zf4_c32_make(a_re[0][lane], 0);
        mimo_c32_t m01 = mimo_ext_zf4_c32_make(a_re[1][lane], a_im[1][lane]);
        mimo_c32_t m02 = mimo_ext_zf4_c32_make(a_re[2][lane], a_im[2][lane]);
        mimo_c32_t m03 = mimo_ext_zf4_c32_make(a_re[3][lane], a_im[3][lane]);
        mimo_c32_t m11 = mimo_ext_zf4_c32_make(a_re[5][lane], 0);
        mimo_c32_t m12 = mimo_ext_zf4_c32_make(a_re[6][lane], a_im[6][lane]);
        mimo_c32_t m13 = mimo_ext_zf4_c32_make(a_re[7][lane], a_im[7][lane]);
        mimo_c32_t m22 = mimo_ext_zf4_c32_make(a_re[10][lane], 0);
        mimo_c32_t m23 = mimo_ext_zf4_c32_make(a_re[11][lane], a_im[11][lane]);
        mimo_c32_t m33 = mimo_ext_zf4_c32_make(a_re[15][lane], 0);
        mimo_c32_t r0 = mimo_ext_zf4_c32_make(b_re[0][lane], b_im[0][lane]);
        mimo_c32_t r1 = mimo_ext_zf4_c32_make(b_re[1][lane], b_im[1][lane]);
        mimo_c32_t r2 = mimo_ext_zf4_c32_make(b_re[2][lane], b_im[2][lane]);
        mimo_c32_t r3 = mimo_ext_zf4_c32_make(b_re[3][lane], b_im[3][lane]);

        const mimo_ext_zf4_pivot_scale_t s0 =
            mimo_ext_zf4_pivot_scale_init(m00.r);
        mimo_c32_t f = mimo_ext_zf4_factor_q30_stage(m01, &s0, 0);
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
        m11.r -= mimo_ext_zf4_c32_mul_q30_q15_real(f, m01);
#else
        m11 = mimo_ext_zf4_c32_sub(m11,
                                   mimo_ext_zf4_c32_mul_q30_q15(f, m01));
#endif
        m12 = mimo_ext_zf4_c32_sub(m12,
                                   mimo_ext_zf4_c32_mul_q30_q15(f, m02));
        m13 = mimo_ext_zf4_c32_sub(m13,
                                   mimo_ext_zf4_c32_mul_q30_q15(f, m03));
        r1 = mimo_ext_zf4_c32_sub(r1,
                                  mimo_ext_zf4_c32_mul_q30_q15(f, r0));

        f = mimo_ext_zf4_factor_q30_stage(m02, &s0, 1);
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
        m22.r -= mimo_ext_zf4_c32_mul_q30_q15_real(f, m02);
#else
        m22 = mimo_ext_zf4_c32_sub(m22,
                                   mimo_ext_zf4_c32_mul_q30_q15(f, m02));
#endif
        m23 = mimo_ext_zf4_c32_sub(m23,
                                   mimo_ext_zf4_c32_mul_q30_q15(f, m03));
        r2 = mimo_ext_zf4_c32_sub(r2,
                                  mimo_ext_zf4_c32_mul_q30_q15(f, r0));

        f = mimo_ext_zf4_factor_q30_stage(m03, &s0, 2);
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
        m33.r -= mimo_ext_zf4_c32_mul_q30_q15_real(f, m03);
#else
        m33 = mimo_ext_zf4_c32_sub(m33,
                                   mimo_ext_zf4_c32_mul_q30_q15(f, m03));
#endif
        r3 = mimo_ext_zf4_c32_sub(r3,
                                  mimo_ext_zf4_c32_mul_q30_q15(f, r0));

        const mimo_ext_zf4_pivot_scale_t s1 =
            mimo_ext_zf4_pivot_scale_init(m11.r);
        f = mimo_ext_zf4_factor_q30_stage(m12, &s1, 3);
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
        m22.r -= mimo_ext_zf4_c32_mul_q30_q15_real(f, m12);
#else
        m22 = mimo_ext_zf4_c32_sub(m22,
                                   mimo_ext_zf4_c32_mul_q30_q15(f, m12));
#endif
        m23 = mimo_ext_zf4_c32_sub(m23,
                                   mimo_ext_zf4_c32_mul_q30_q15(f, m13));
        r2 = mimo_ext_zf4_c32_sub(r2,
                                  mimo_ext_zf4_c32_mul_q30_q15(f, r1));

        f = mimo_ext_zf4_factor_q30_stage(m13, &s1, 4);
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
        m33.r -= mimo_ext_zf4_c32_mul_q30_q15_real(f, m13);
#else
        m33 = mimo_ext_zf4_c32_sub(m33,
                                   mimo_ext_zf4_c32_mul_q30_q15(f, m13));
#endif
        r3 = mimo_ext_zf4_c32_sub(r3,
                                  mimo_ext_zf4_c32_mul_q30_q15(f, r1));

        const mimo_ext_zf4_pivot_scale_t s2 =
            mimo_ext_zf4_pivot_scale_init(m22.r);
        f = mimo_ext_zf4_factor_q30_stage(m23, &s2, 5);
#if MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
        m33.r -= mimo_ext_zf4_c32_mul_q30_q15_real(f, m23);
#else
        m33 = mimo_ext_zf4_c32_sub(m33,
                                   mimo_ext_zf4_c32_mul_q30_q15(f, m23));
#endif
        r3 = mimo_ext_zf4_c32_sub(r3,
                                  mimo_ext_zf4_c32_mul_q30_q15(f, r2));

        const mimo_ext_zf4_pivot_scale_t s3 =
            mimo_ext_zf4_pivot_scale_init(m33.r);
        const mimo_c32_t x3 = mimo_ext_zf4_div_real_to_q15_stage(r3, &s3, 0);
        mimo_c32_t sum =
            mimo_ext_zf4_c32_sub(r2, mimo_ext_zf4_c32_mul_q15(m23, x3));
        const mimo_c32_t x2 = mimo_ext_zf4_div_real_to_q15_stage(sum, &s2, 1);

        sum = mimo_ext_zf4_c32_sub(r1,
                                   mimo_ext_zf4_c32_mul_q15(m12, x2));
        sum = mimo_ext_zf4_c32_sub(sum,
                                   mimo_ext_zf4_c32_mul_q15(m13, x3));
        const mimo_c32_t x1 = mimo_ext_zf4_div_real_to_q15_stage(sum, &s1, 2);

        sum = mimo_ext_zf4_c32_sub(r0,
                                   mimo_ext_zf4_c32_mul_q15(m01, x1));
        sum = mimo_ext_zf4_c32_sub(sum,
                                   mimo_ext_zf4_c32_mul_q15(m02, x2));
        sum = mimo_ext_zf4_c32_sub(sum,
                                   mimo_ext_zf4_c32_mul_q15(m03, x3));
        const mimo_c32_t x0 = mimo_ext_zf4_div_real_to_q15_stage(sum, &s0, 3);
        mimo_c16_t *x = &x_block[lane * MIMO_N4];

        x[0].r = mimo_ext_zf4_store_i16(x0.r);
        x[0].i = mimo_ext_zf4_store_i16(x0.i);
        x[1].r = mimo_ext_zf4_store_i16(x1.r);
        x[1].i = mimo_ext_zf4_store_i16(x1.i);
        x[2].r = mimo_ext_zf4_store_i16(x2.r);
        x[2].i = mimo_ext_zf4_store_i16(x2.i);
        x[3].r = mimo_ext_zf4_store_i16(x3.r);
        x[3].i = mimo_ext_zf4_store_i16(x3.i);
    }
}

#undef MIMO_EXT_ZF4_PIVOT_RECIP_LAST
#undef MIMO_EXT_ZF4_PIVOT_RECIP_STEP_SHIFT
#undef MIMO_EXT_ZF4_PIVOT_RECIP_MIN
#endif

#if MIMO_USE_EXT_ZF4_SCHUR_RVV_SOLVE || MIMO_USE_EXT_MMSE4_SCHUR_RVV_SOLVE
#define MIMO_EXT_SCHUR4_DET_Q15_RECIP_MIN 3400
#define MIMO_EXT_SCHUR4_DET_Q15_RECIP_STEP_SHIFT 4

static const int32_t mimo_ext_schur4_det_recip_base_q30[] = {
    315806, 314327, 312861, 311410, 309971, 308546, 307134, 305735,
    304348, 302974, 301612, 300263, 298925, 297600, 296286, 294984,
    293693, 292413, 291144, 289887, 288640, 287404, 286178, 284963,
    283758, 282563, 281378, 280204, 279038, 277883, 276737, 275601,
    274473, 273355, 272246, 271146, 270055, 268973, 267899, 266834,
    265777, 264729, 263689, 262657, 261632, 260616, 259608, 258608,
    257615, 256630, 255652, 254682, 253719, 252764, 251815, 250874,
    249939, 249012, 248091, 247178, 246271, 245370, 244476, 243589,
};

static const int32_t mimo_ext_schur4_det_recip_delta_q30[] = {
    -1479, -1466, -1451, -1439, -1425, -1412, -1399, -1387,
    -1374, -1362, -1349, -1338, -1325, -1314, -1302, -1291,
    -1280, -1269, -1257, -1247, -1236, -1226, -1215, -1205,
    -1195, -1185, -1174, -1166, -1155, -1146, -1136, -1128,
    -1118, -1109, -1100, -1091, -1082, -1074, -1065, -1057,
    -1048, -1040, -1032, -1025, -1016, -1008, -1000, -993,
    -985, -978, -970, -963, -955, -949, -941, -935,
    -927, -921, -913, -907, -901, -894, -887, 0,
};

static MIMO_ALWAYS_INLINE vint32m1_t mimo_ext_schur4_round_q15_i64(
    vint64m2_t x,
    size_t vl)
{
#if MIMO_USE_SOLVE_Q15_I64_ARSHIFT_ROUND
    x = __riscv_vadd_vx_i64m2(x, MIMO_Q15_ROUND, vl);
    x = __riscv_vsra_vx_i64m2(x, MIMO_Q15_SHIFT, vl);
    return __riscv_vncvt_x_x_w_i32m1(x, vl);
#else
    return mimo_rvv_solve4_round_shift_i64_to_i32(
        x, MIMO_Q15_SHIFT, MIMO_Q15_ROUND, vl);
#endif
}

static MIMO_ALWAYS_INLINE vint32m1_t mimo_ext_schur4_round_q15_i32(
    vint32m1_t x,
    size_t vl)
{
#if MIMO_USE_SOLVE_Q15_I64_ARSHIFT_ROUND
    x = __riscv_vadd_vx_i32m1(x, MIMO_Q15_ROUND, vl);
    return __riscv_vsra_vx_i32m1(x, MIMO_Q15_SHIFT, vl);
#else
    return mimo_rvv_solve4_round_shift_i32_to_i32(
        x, MIMO_Q15_SHIFT, MIMO_Q15_ROUND, vl);
#endif
}

static MIMO_ALWAYS_INLINE vint64m2_t mimo_ext_schur4_det_q30_rvv(
    vint32m1_t a00r,
    vint32m1_t a01r,
    vint32m1_t a01i,
    vint32m1_t a11r,
    size_t vl)
{
    vint64m2_t det = __riscv_vwmul_vv_i64m2(a00r, a11r, vl);
    vint64m2_t tmp = __riscv_vwmul_vv_i64m2(a01r, a01r, vl);

    det = __riscv_vsub_vv_i64m2(det, tmp, vl);
    tmp = __riscv_vwmul_vv_i64m2(a01i, a01i, vl);
    return __riscv_vsub_vv_i64m2(det, tmp, vl);
}

static MIMO_ALWAYS_INLINE vint32m1_t mimo_ext_schur4_scale_from_det_q30_rvv(
    vint64m2_t den_q30,
    size_t vl)
{
    den_q30 = __riscv_vadd_vx_i64m2(den_q30, MIMO_Q15_ROUND, vl);
    den_q30 = __riscv_vsra_vx_i64m2(den_q30, MIMO_Q15_SHIFT, vl);

    const vint32m1_t den_q15 = __riscv_vncvt_x_x_w_i32m1(den_q30, vl);
    const vint32m1_t rel = __riscv_vsub_vx_i32m1(
        den_q15, MIMO_EXT_SCHUR4_DET_Q15_RECIP_MIN, vl);
    const vuint32m1_t rel_u = __riscv_vreinterpret_v_i32m1_u32m1(rel);
    const vuint32m1_t seg =
        __riscv_vsrl_vx_u32m1(rel_u,
                              MIMO_EXT_SCHUR4_DET_Q15_RECIP_STEP_SHIFT, vl);
    const vuint32m1_t idx = __riscv_vsll_vx_u32m1(seg, 2, vl);
    const vint32m1_t off = __riscv_vand_vx_i32m1(
        rel, (1 << MIMO_EXT_SCHUR4_DET_Q15_RECIP_STEP_SHIFT) - 1, vl);
    const vint32m1_t base = __riscv_vluxei32_v_i32m1(
        mimo_ext_schur4_det_recip_base_q30, idx, vl);
    const vint32m1_t delta = __riscv_vluxei32_v_i32m1(
        mimo_ext_schur4_det_recip_delta_q30, idx, vl);
    const vint32m1_t zero = __riscv_vmv_v_x_i32m1(0, vl);
    vint32m1_t drop =
        __riscv_vmul_vv_i32m1(__riscv_vsub_vv_i32m1(zero, delta, vl),
                              off, vl);

    drop = __riscv_vadd_vx_i32m1(drop, 8, vl);
    drop = __riscv_vsra_vx_i32m1(
        drop, MIMO_EXT_SCHUR4_DET_Q15_RECIP_STEP_SHIFT, vl);
    return __riscv_vsub_vv_i32m1(base, drop, vl);
}

static MIMO_ALWAYS_INLINE vint32m1_t mimo_ext_schur4_scale_q30_to_q15_rvv(
    vint64m2_t num_q30,
    vint32m1_t scale_q30,
    size_t vl)
{
    num_q30 = __riscv_vadd_vx_i64m2(
        num_q30, 1 << (MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT - 1), vl);
    num_q30 = __riscv_vsra_vx_i64m2(
        num_q30, MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT, vl);

    const vint32m1_t num_qs = __riscv_vncvt_x_x_w_i32m1(num_q30, vl);
    const vint32m1_t prod =
        __riscv_vmul_vv_i32m1(num_qs, scale_q30, vl);

    return __riscv_vsra_vx_i32m1(
        prod, 30 - MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT, vl);
}

static MIMO_ALWAYS_INLINE void mimo_ext_schur4_solve2_q15_rvv(
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
    size_t vl)
{
    vint64m2_t num0r = __riscv_vwmul_vv_i64m2(v0r, a11r, vl);
    vint64m2_t tmp = __riscv_vwmul_vv_i64m2(a01r, v1r, vl);

    num0r = __riscv_vsub_vv_i64m2(num0r, tmp, vl);
    tmp = __riscv_vwmul_vv_i64m2(a01i, v1i, vl);
    num0r = __riscv_vadd_vv_i64m2(num0r, tmp, vl);

    vint64m2_t num0i = __riscv_vwmul_vv_i64m2(v0i, a11r, vl);
    tmp = __riscv_vwmul_vv_i64m2(a01r, v1i, vl);
    num0i = __riscv_vsub_vv_i64m2(num0i, tmp, vl);
    tmp = __riscv_vwmul_vv_i64m2(a01i, v1r, vl);
    num0i = __riscv_vsub_vv_i64m2(num0i, tmp, vl);

    vint64m2_t num1r = __riscv_vwmul_vv_i64m2(v1r, a00r, vl);
    tmp = __riscv_vwmul_vv_i64m2(a01r, v0r, vl);
    num1r = __riscv_vsub_vv_i64m2(num1r, tmp, vl);
    tmp = __riscv_vwmul_vv_i64m2(a01i, v0i, vl);
    num1r = __riscv_vsub_vv_i64m2(num1r, tmp, vl);

    vint64m2_t num1i = __riscv_vwmul_vv_i64m2(v1i, a00r, vl);
    tmp = __riscv_vwmul_vv_i64m2(a01r, v0i, vl);
    num1i = __riscv_vsub_vv_i64m2(num1i, tmp, vl);
    tmp = __riscv_vwmul_vv_i64m2(a01i, v0r, vl);
    num1i = __riscv_vadd_vv_i64m2(num1i, tmp, vl);

    *out0r = mimo_ext_schur4_scale_q30_to_q15_rvv(num0r, scale_q30, vl);
    *out0i = mimo_ext_schur4_scale_q30_to_q15_rvv(num0i, scale_q30, vl);
    *out1r = mimo_ext_schur4_scale_q30_to_q15_rvv(num1r, scale_q30, vl);
    *out1i = mimo_ext_schur4_scale_q30_to_q15_rvv(num1i, scale_q30, vl);
}

static MIMO_ALWAYS_INLINE void mimo_ext_schur4_cmul_q15_rvv(
    vint32m1_t ar,
    vint32m1_t ai,
    vint32m1_t br,
    vint32m1_t bi,
    vint32m1_t *out_re,
    vint32m1_t *out_im,
    size_t vl)
{
#if MIMO_USE_RVV_SOLVE4_Q15_I32_MUL
    vint32m1_t re = __riscv_vmul_vv_i32m1(ar, br, vl);
    vint32m1_t tmp = __riscv_vmul_vv_i32m1(ai, bi, vl);

    re = __riscv_vsub_vv_i32m1(re, tmp, vl);

    vint32m1_t im = __riscv_vmul_vv_i32m1(ar, bi, vl);
    tmp = __riscv_vmul_vv_i32m1(ai, br, vl);
    im = __riscv_vadd_vv_i32m1(im, tmp, vl);

    *out_re = mimo_ext_schur4_round_q15_i32(re, vl);
    *out_im = mimo_ext_schur4_round_q15_i32(im, vl);
#else
    vint64m2_t re = __riscv_vwmul_vv_i64m2(ar, br, vl);
    vint64m2_t tmp = __riscv_vwmul_vv_i64m2(ai, bi, vl);

    re = __riscv_vsub_vv_i64m2(re, tmp, vl);

    vint64m2_t im = __riscv_vwmul_vv_i64m2(ar, bi, vl);
    tmp = __riscv_vwmul_vv_i64m2(ai, br, vl);
    im = __riscv_vadd_vv_i64m2(im, tmp, vl);

    *out_re = mimo_ext_schur4_round_q15_i64(re, vl);
    *out_im = mimo_ext_schur4_round_q15_i64(im, vl);
#endif
}

static MIMO_ALWAYS_INLINE void mimo_ext_schur4_inner_conj_q15_rvv(
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
#if MIMO_USE_RVV_SOLVE4_Q15_I32_MUL
    vint32m1_t re = __riscv_vmul_vv_i32m1(e0r, w0r, vl);
    vint32m1_t tmp = __riscv_vmul_vv_i32m1(e0i, w0i, vl);

    re = __riscv_vadd_vv_i32m1(re, tmp, vl);

    vint32m1_t im = __riscv_vmul_vv_i32m1(e0r, w0i, vl);
    tmp = __riscv_vmul_vv_i32m1(e0i, w0r, vl);
    im = __riscv_vsub_vv_i32m1(im, tmp, vl);

    vint32m1_t p0r = mimo_ext_schur4_round_q15_i32(re, vl);
    vint32m1_t p0i = mimo_ext_schur4_round_q15_i32(im, vl);

    re = __riscv_vmul_vv_i32m1(e1r, w1r, vl);
    tmp = __riscv_vmul_vv_i32m1(e1i, w1i, vl);
    re = __riscv_vadd_vv_i32m1(re, tmp, vl);

    im = __riscv_vmul_vv_i32m1(e1r, w1i, vl);
    tmp = __riscv_vmul_vv_i32m1(e1i, w1r, vl);
    im = __riscv_vsub_vv_i32m1(im, tmp, vl);

    *out_re = __riscv_vadd_vv_i32m1(
        p0r, mimo_ext_schur4_round_q15_i32(re, vl), vl);
    *out_im = __riscv_vadd_vv_i32m1(
        p0i, mimo_ext_schur4_round_q15_i32(im, vl), vl);
#else
    vint64m2_t re = __riscv_vwmul_vv_i64m2(e0r, w0r, vl);
    vint64m2_t tmp = __riscv_vwmul_vv_i64m2(e0i, w0i, vl);

    re = __riscv_vadd_vv_i64m2(re, tmp, vl);

    vint64m2_t im = __riscv_vwmul_vv_i64m2(e0r, w0i, vl);
    tmp = __riscv_vwmul_vv_i64m2(e0i, w0r, vl);
    im = __riscv_vsub_vv_i64m2(im, tmp, vl);

    vint32m1_t p0r = mimo_ext_schur4_round_q15_i64(re, vl);
    vint32m1_t p0i = mimo_ext_schur4_round_q15_i64(im, vl);

    re = __riscv_vwmul_vv_i64m2(e1r, w1r, vl);
    tmp = __riscv_vwmul_vv_i64m2(e1i, w1i, vl);
    re = __riscv_vadd_vv_i64m2(re, tmp, vl);

    im = __riscv_vwmul_vv_i64m2(e1r, w1i, vl);
    tmp = __riscv_vwmul_vv_i64m2(e1i, w1r, vl);
    im = __riscv_vsub_vv_i64m2(im, tmp, vl);

    *out_re = __riscv_vadd_vv_i32m1(
        p0r, mimo_ext_schur4_round_q15_i64(re, vl), vl);
    *out_im = __riscv_vadd_vv_i32m1(
        p0i, mimo_ext_schur4_round_q15_i64(im, vl), vl);
#endif
}

static MIMO_ALWAYS_INLINE void mimo_ext_schur4_submul_q15_rvv(
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

    mimo_ext_schur4_cmul_q15_rvv(ar, ai, br, bi, &prod_re, &prod_im, vl);
    *acc_re = __riscv_vsub_vv_i32m1(*acc_re, prod_re, vl);
    *acc_im = __riscv_vsub_vv_i32m1(*acc_im, prod_im, vl);
}

static MIMO_NOINLINE void mimo_ext_zf4_schur_rvv_solve_soa_q15(
    const mimo_site4_soa4_t *restrict work,
    mimo_c16_t *restrict x_block)
{
    const size_t vl = __riscv_vsetvl_e32m1(MIMO_SITE4_LANES);
    const ptrdiff_t x_stride = (ptrdiff_t)(MIMO_N4 * sizeof(mimo_c16_t));
    const vint32m1_t zero = __riscv_vmv_v_x_i32m1(0, vl);

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

    const vint64m2_t b_det_q30 =
        mimo_ext_schur4_det_q30_rvv(a00r, a01r, a01i, a11r, vl);
    const vint32m1_t b_scale =
        mimo_ext_schur4_scale_from_det_q30_rvv(b_det_q30, vl);
    vint32m1_t w00r, w00i, w10r, w10i;
    vint32m1_t w01r, w01i, w11r, w11i;
    vint32m1_t y0r, y0i, y1r, y1i;

    mimo_ext_schur4_solve2_q15_rvv(
        a00r, a01r, a01i, a11r, b_scale,
        a02r, a02i, a12r, a12i, &w00r, &w00i, &w10r, &w10i, vl);
    mimo_ext_schur4_solve2_q15_rvv(
        a00r, a01r, a01i, a11r, b_scale,
        a03r, a03i, a13r, a13i, &w01r, &w01i, &w11r, &w11i, vl);
    mimo_ext_schur4_solve2_q15_rvv(
        a00r, a01r, a01i, a11r, b_scale,
        b0r, b0i, b1r, b1i, &y0r, &y0i, &y1r, &y1i, vl);

    vint32m1_t inner_r;
    vint32m1_t inner_i;

    mimo_ext_schur4_inner_conj_q15_rvv(
        a02r, a02i, a12r, a12i, w00r, w00i, w10r, w10i,
        &inner_r, &inner_i, vl);
    const vint32m1_t s00r = __riscv_vsub_vv_i32m1(a22r, inner_r, vl);

    mimo_ext_schur4_inner_conj_q15_rvv(
        a02r, a02i, a12r, a12i, w01r, w01i, w11r, w11i,
        &inner_r, &inner_i, vl);
    const vint32m1_t s01r = __riscv_vsub_vv_i32m1(a23r, inner_r, vl);
    const vint32m1_t s01i = __riscv_vsub_vv_i32m1(a23i, inner_i, vl);

    mimo_ext_schur4_inner_conj_q15_rvv(
        a03r, a03i, a13r, a13i, w01r, w01i, w11r, w11i,
        &inner_r, &inner_i, vl);
    const vint32m1_t s11r = __riscv_vsub_vv_i32m1(a33r, inner_r, vl);

    mimo_ext_schur4_inner_conj_q15_rvv(
        a02r, a02i, a12r, a12i, y0r, y0i, y1r, y1i,
        &inner_r, &inner_i, vl);
    const vint32m1_t r0r = __riscv_vsub_vv_i32m1(b2r, inner_r, vl);
    const vint32m1_t r0i = __riscv_vsub_vv_i32m1(b2i, inner_i, vl);

    mimo_ext_schur4_inner_conj_q15_rvv(
        a03r, a03i, a13r, a13i, y0r, y0i, y1r, y1i,
        &inner_r, &inner_i, vl);
    const vint32m1_t r1r = __riscv_vsub_vv_i32m1(b3r, inner_r, vl);
    const vint32m1_t r1i = __riscv_vsub_vv_i32m1(b3i, inner_i, vl);

    const vint64m2_t s_det_q30 =
        mimo_ext_schur4_det_q30_rvv(s00r, s01r, s01i, s11r, vl);
    const vint32m1_t s_scale =
        mimo_ext_schur4_scale_from_det_q30_rvv(s_det_q30, vl);
    vint32m1_t x2r, x2i, x3r, x3i;

    mimo_ext_schur4_solve2_q15_rvv(
        s00r, s01r, s01i, s11r, s_scale,
        r0r, r0i, r1r, r1i, &x2r, &x2i, &x3r, &x3i, vl);

    vint32m1_t top0r = y0r;
    vint32m1_t top0i = y0i;
    vint32m1_t top1r = y1r;
    vint32m1_t top1i = y1i;

    mimo_ext_schur4_submul_q15_rvv(
        &top0r, &top0i, w00r, w00i, x2r, x2i, vl);
    mimo_ext_schur4_submul_q15_rvv(
        &top0r, &top0i, w01r, w01i, x3r, x3i, vl);
    mimo_ext_schur4_submul_q15_rvv(
        &top1r, &top1i, w10r, w10i, x2r, x2i, vl);
    mimo_ext_schur4_submul_q15_rvv(
        &top1r, &top1i, w11r, w11i, x3r, x3i, vl);

    (void)zero;
    mimo_rvv_solve4_store_x(x_block, 0, top0r, top0i, x_stride, vl);
    mimo_rvv_solve4_store_x(x_block, 1, top1r, top1i, x_stride, vl);
    mimo_rvv_solve4_store_x(x_block, 2, x2r, x2i, x_stride, vl);
    mimo_rvv_solve4_store_x(x_block, 3, x3r, x3i, x_stride, vl);
}

#if MIMO_USE_EXT_MMSE4_SCHUR_RVV_SOLVE
static MIMO_NOINLINE void mimo_ext_mmse4_schur_rvv_solve_soa_q15(
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

#undef MIMO_EXT_SCHUR4_DET_Q15_RECIP_STEP_SHIFT
#undef MIMO_EXT_SCHUR4_DET_Q15_RECIP_MIN
#endif

#if MIMO_USE_EXT_ZF4_INLINE_UPPER_I16_SOLVE
#define MIMO_EXT_INLINE_SCHUR4_DET_Q15_RECIP_MIN 3400u
#define MIMO_SCHUR4_DET_Q15_RECIP_DIRECT1_ENTRY(rel_) \
    ((uint32_t)(1073741824ull / \
                (MIMO_EXT_INLINE_SCHUR4_DET_Q15_RECIP_MIN + \
                 (uint32_t)(rel_))))

static const uint32_t mimo_ext_inline_schur4_recip_direct1_q30[] = {
#include "../kernels/common/mimo_schur_recip_direct1.inc"
};

#undef MIMO_SCHUR4_DET_Q15_RECIP_DIRECT1_ENTRY

typedef struct {
    uint32_t scale_q30;
} mimo_ext_inline_schur4_scale_t;

typedef struct {
#if !MIMO_USE_EXT_ZF4_INLINE_HEAD_WORK_REUSE
    mimo_c16_t w00[MIMO_SITE4_LANES];
    mimo_c16_t w10[MIMO_SITE4_LANES];
    mimo_c16_t w01[MIMO_SITE4_LANES];
    mimo_c16_t w11[MIMO_SITE4_LANES];
    mimo_c16_t y0[MIMO_SITE4_LANES];
    mimo_c16_t y1[MIMO_SITE4_LANES];
#endif
#if !MIMO_USE_EXT_ZF4_TAIL_SCRATCH_REUSE
    mimo_c16_t s00[MIMO_SITE4_LANES];
    mimo_c16_t s01[MIMO_SITE4_LANES];
    mimo_c16_t s11[MIMO_SITE4_LANES];
    mimo_c16_t r0[MIMO_SITE4_LANES];
    mimo_c16_t r1[MIMO_SITE4_LANES];
#endif
    mimo_c16_t x2[MIMO_SITE4_LANES];
    mimo_c16_t x3[MIMO_SITE4_LANES];
} mimo_ext_inline_upper_i16_pack_t;

static MIMO_ALWAYS_INLINE mimo_c16_t
mimo_ext_inline_state_c16(int16_t r, int16_t i)
{
    mimo_c16_t x = {r, i};
    return x;
}

static MIMO_ALWAYS_INLINE int16_t
mimo_ext_inline_store_i16(int32_t x)
{
#if MIMO_USE_SOLVE_BRANCHLIGHT_ROUND || MIMO_USE_SOLVE_STORE_TRUST_I16
    return (int16_t)x;
#else
    if (x > 32767) {
        return 32767;
    }
    if (x < -32768) {
        return -32768;
    }
    return (int16_t)x;
#endif
}

static MIMO_ALWAYS_INLINE mimo_ext_inline_schur4_scale_t
mimo_ext_inline_schur4_scale_from_det_q30(int64_t den_q30)
{
    const uint32_t den_q15 =
        (uint32_t)((den_q30 + MIMO_Q15_ROUND) >> MIMO_Q15_SHIFT);
    const uint32_t rel =
        den_q15 - MIMO_EXT_INLINE_SCHUR4_DET_Q15_RECIP_MIN;
    mimo_ext_inline_schur4_scale_t s;

    s.scale_q30 = mimo_ext_inline_schur4_recip_direct1_q30[rel];
    return s;
}

static MIMO_ALWAYS_INLINE int32_t
mimo_ext_inline_scale_q30_to_q15(int64_t num_q30,
                                 mimo_ext_inline_schur4_scale_t scale)
{
    const int32_t num_qs = (int32_t)(
        (num_q30 +
         (1ll << (MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT - 1))) >>
        MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT);
    const int32_t prod = num_qs * (int32_t)scale.scale_q30;

    return prod >> (30 - MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT);
}

static MIMO_ALWAYS_INLINE int32_t
mimo_ext_inline_scale_q30_to_q15_trunc(int64_t num_q30,
                                       mimo_ext_inline_schur4_scale_t scale)
{
    const int32_t num_qs = (int32_t)(
        (num_q30 +
         (1ll << (MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT - 1))) >>
        MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT);
    int32_t prod = num_qs * (int32_t)scale.scale_q30;
    const int32_t final_bias =
        (1 << (30 - MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT)) - 1;

    if (prod < 0) {
        prod += final_bias;
    }
    return prod >> (30 - MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT);
}

#if MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_ONLY || \
    MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_MASK
static MIMO_ALWAYS_INLINE int32_t
mimo_ext_inline_scale_q30_to_q15_arshift(int64_t num_q30,
                                         mimo_ext_inline_schur4_scale_t scale)
{
    const int32_t num_qs =
        (int32_t)(num_q30 >> MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT);
    const int32_t prod = num_qs * (int32_t)scale.scale_q30;

    return prod >> (30 - MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT);
}

static MIMO_ALWAYS_INLINE int32_t
mimo_ext_inline_scale_q30_to_q15_select(int64_t num_q30,
                                        mimo_ext_inline_schur4_scale_t scale,
                                        int use_arshift,
                                        int use_trunc)
{
    if (use_trunc) {
        return mimo_ext_inline_scale_q30_to_q15_trunc(num_q30, scale);
    }
    return use_arshift
               ? mimo_ext_inline_scale_q30_to_q15_arshift(num_q30, scale)
               : mimo_ext_inline_scale_q30_to_q15(num_q30, scale);
}
#define MIMO_EXT_INLINE_SOLVE2_ADJUST_PARAM , int arshift_mask, int trunc_mask
#define MIMO_EXT_INLINE_SOLVE2_ADJUST_ARG(a_, t_) , (a_), (t_)
#else
#if MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK
static MIMO_ALWAYS_INLINE int32_t
mimo_ext_inline_scale_q30_to_q15_select(int64_t num_q30,
                                        mimo_ext_inline_schur4_scale_t scale,
                                        int use_trunc)
{
    return use_trunc ? mimo_ext_inline_scale_q30_to_q15_trunc(num_q30, scale)
                     : mimo_ext_inline_scale_q30_to_q15(num_q30, scale);
}
#define MIMO_EXT_INLINE_SOLVE2_ADJUST_PARAM , int trunc_mask
#define MIMO_EXT_INLINE_SOLVE2_ADJUST_ARG(a_, t_) , (t_)
#else
#define MIMO_EXT_INLINE_SOLVE2_ADJUST_PARAM
#define MIMO_EXT_INLINE_SOLVE2_ADJUST_ARG(a_, t_)
#endif
#endif

static MIMO_ALWAYS_INLINE int64_t
mimo_ext_inline_det2_q30(int16_t a00r,
                         int16_t a01r,
                         int16_t a01i,
                         int16_t a11r)
{
    return (int64_t)a00r * a11r -
           ((int64_t)a01r * a01r + (int64_t)a01i * a01i);
}

static MIMO_ALWAYS_INLINE void
mimo_ext_inline_solve2_q15(int16_t a00r,
                           int16_t a01r,
                           int16_t a01i,
                           int16_t a11r,
                           mimo_ext_inline_schur4_scale_t scale,
                           int16_t v0r,
                           int16_t v0i,
                           int16_t v1r,
                           int16_t v1i,
                           int16_t *restrict out0r,
                           int16_t *restrict out0i,
                           int16_t *restrict out1r,
                           int16_t *restrict out1i
                           MIMO_EXT_INLINE_SOLVE2_ADJUST_PARAM)
{
    const int32_t num0r =
        (int32_t)v0r * a11r -
        ((int32_t)a01r * v1r - (int32_t)a01i * v1i);
    const int32_t num0i =
        (int32_t)v0i * a11r -
        ((int32_t)a01r * v1i + (int32_t)a01i * v1r);
    const int32_t num1r =
        -(int32_t)a01r * v0r - (int32_t)a01i * v0i +
        (int32_t)v1r * a00r;
    const int32_t num1i =
        -(int32_t)a01r * v0i + (int32_t)a01i * v0r +
        (int32_t)v1i * a00r;

#if MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_ONLY || \
    MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_MASK
    *out0r = (int16_t)mimo_ext_inline_scale_q30_to_q15_select(
        num0r, scale, arshift_mask & 1, trunc_mask & 1);
    *out0i = (int16_t)mimo_ext_inline_scale_q30_to_q15_select(
        num0i, scale, arshift_mask & 2, trunc_mask & 2);
    *out1r = (int16_t)mimo_ext_inline_scale_q30_to_q15_select(
        num1r, scale, arshift_mask & 4, trunc_mask & 4);
    *out1i = (int16_t)mimo_ext_inline_scale_q30_to_q15_select(
        num1i, scale, arshift_mask & 8, trunc_mask & 8);
#elif MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK
    *out0r = (int16_t)mimo_ext_inline_scale_q30_to_q15_select(
        num0r, scale, trunc_mask & 1);
    *out0i = (int16_t)mimo_ext_inline_scale_q30_to_q15_select(
        num0i, scale, trunc_mask & 2);
    *out1r = (int16_t)mimo_ext_inline_scale_q30_to_q15_select(
        num1r, scale, trunc_mask & 4);
    *out1i = (int16_t)mimo_ext_inline_scale_q30_to_q15_select(
        num1i, scale, trunc_mask & 8);
#else
    *out0r = (int16_t)mimo_ext_inline_scale_q30_to_q15(num0r, scale);
    *out0i = (int16_t)mimo_ext_inline_scale_q30_to_q15(num0i, scale);
    *out1r = (int16_t)mimo_ext_inline_scale_q30_to_q15(num1r, scale);
    *out1i = (int16_t)mimo_ext_inline_scale_q30_to_q15(num1i, scale);
#endif
}

#if MIMO_USE_EXT_ZF4_SOLVE2_DIRECT_STORE
static MIMO_ALWAYS_INLINE void
mimo_ext_inline_solve2_store_q15(int16_t a00r,
                                 int16_t a01r,
                                 int16_t a01i,
                                 int16_t a11r,
                                 mimo_ext_inline_schur4_scale_t scale,
                                 int16_t v0r,
                                 int16_t v0i,
                                 int16_t v1r,
                                 int16_t v1i,
                                 mimo_c16_t out0[restrict MIMO_SITE4_LANES],
                                 mimo_c16_t out1[restrict MIMO_SITE4_LANES],
                                 int lane
                                 MIMO_EXT_INLINE_SOLVE2_ADJUST_PARAM)
{
    const int32_t num0r =
        (int32_t)v0r * a11r -
        ((int32_t)a01r * v1r - (int32_t)a01i * v1i);
    const int32_t num0i =
        (int32_t)v0i * a11r -
        ((int32_t)a01r * v1i + (int32_t)a01i * v1r);
    const int32_t num1r =
        -(int32_t)a01r * v0r - (int32_t)a01i * v0i +
        (int32_t)v1r * a00r;
    const int32_t num1i =
        -(int32_t)a01r * v0i + (int32_t)a01i * v0r +
        (int32_t)v1i * a00r;

#if MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_ONLY || \
    MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_MASK
    out0[lane] = mimo_ext_inline_state_c16(
        (int16_t)mimo_ext_inline_scale_q30_to_q15_select(
            num0r, scale, arshift_mask & 1, trunc_mask & 1),
        (int16_t)mimo_ext_inline_scale_q30_to_q15_select(
            num0i, scale, arshift_mask & 2, trunc_mask & 2));
    out1[lane] = mimo_ext_inline_state_c16(
        (int16_t)mimo_ext_inline_scale_q30_to_q15_select(
            num1r, scale, arshift_mask & 4, trunc_mask & 4),
        (int16_t)mimo_ext_inline_scale_q30_to_q15_select(
            num1i, scale, arshift_mask & 8, trunc_mask & 8));
#elif MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK
    out0[lane] = mimo_ext_inline_state_c16(
        (int16_t)mimo_ext_inline_scale_q30_to_q15_select(
            num0r, scale, trunc_mask & 1),
        (int16_t)mimo_ext_inline_scale_q30_to_q15_select(
            num0i, scale, trunc_mask & 2));
    out1[lane] = mimo_ext_inline_state_c16(
        (int16_t)mimo_ext_inline_scale_q30_to_q15_select(
            num1r, scale, trunc_mask & 4),
        (int16_t)mimo_ext_inline_scale_q30_to_q15_select(
            num1i, scale, trunc_mask & 8));
#else
    out0[lane] = mimo_ext_inline_state_c16(
        (int16_t)mimo_ext_inline_scale_q30_to_q15(num0r, scale),
        (int16_t)mimo_ext_inline_scale_q30_to_q15(num0i, scale));
    out1[lane] = mimo_ext_inline_state_c16(
        (int16_t)mimo_ext_inline_scale_q30_to_q15(num1r, scale),
        (int16_t)mimo_ext_inline_scale_q30_to_q15(num1i, scale));
#endif
}
#endif

static MIMO_ALWAYS_INLINE void
mimo_ext_inline_schur5_q15(
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

#if MIMO_EXT_USE_LIVE_TAIL_SCHUR
static MIMO_ALWAYS_INLINE void
mimo_ext_inline_schur5_live_tail_q15(
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
    mimo_c16_t s00[restrict MIMO_SITE4_LANES],
    mimo_c16_t s01[restrict MIMO_SITE4_LANES],
    mimo_c16_t s11[restrict MIMO_SITE4_LANES],
    mimo_c16_t r0[restrict MIMO_SITE4_LANES],
    mimo_c16_t r1[restrict MIMO_SITE4_LANES])
{
    /*
     * The fused build leaves a22/a23/a33/b2/b3 in v1/v3/v7/v30/v5.
     * This candidate is restricted to the inline solve path, so no ABI
     * boundary sits between that build asm and this Schur asm.
     */
#if MIMO_USE_SOLVE4_SCHUR_ZF_EXT_MACC_INNER
    asm volatile(
        "vmv.v.i v16, 0\n\t"
        "vle16.v v20, (%[e02])\n\t"
        "vle16.v v24, (%[w00])\n\t"
        ".word 0xDB4C0857\n\t"
        "vle16.v v20, (%[e12])\n\t"
        "vle16.v v24, (%[w10])\n\t"
        ".word 0xDB4C0857\n\t"
        "vsub.vv v10, v1, v16\n\t"
        "vse16.v v10, (%[s00])\n\t"

        "vmv.v.i v16, 0\n\t"
        "vle16.v v20, (%[e02])\n\t"
        "vle16.v v24, (%[w01])\n\t"
        ".word 0xDB4C0857\n\t"
        "vle16.v v20, (%[e12])\n\t"
        "vle16.v v24, (%[w11])\n\t"
        ".word 0xDB4C0857\n\t"
        "vsub.vv v10, v3, v16\n\t"
        "vse16.v v10, (%[s01])\n\t"

        "vmv.v.i v16, 0\n\t"
        "vle16.v v20, (%[e03])\n\t"
        "vle16.v v24, (%[w01])\n\t"
        ".word 0xDB4C0857\n\t"
        "vle16.v v20, (%[e13])\n\t"
        "vle16.v v24, (%[w11])\n\t"
        ".word 0xDB4C0857\n\t"
        "vsub.vv v10, v7, v16\n\t"
        "vse16.v v10, (%[s11])\n\t"

        "vmv.v.i v16, 0\n\t"
        "vle16.v v20, (%[e02])\n\t"
        "vle16.v v24, (%[y0])\n\t"
        ".word 0xDB4C0857\n\t"
        "vle16.v v20, (%[e12])\n\t"
        "vle16.v v24, (%[y1])\n\t"
        ".word 0xDB4C0857\n\t"
        "vsub.vv v10, v30, v16\n\t"
        "vse16.v v10, (%[r0])\n\t"

        "vmv.v.i v16, 0\n\t"
        "vle16.v v20, (%[e03])\n\t"
        "vle16.v v24, (%[y0])\n\t"
        ".word 0xDB4C0857\n\t"
        "vle16.v v20, (%[e13])\n\t"
        "vle16.v v24, (%[y1])\n\t"
        ".word 0xDB4C0857\n\t"
        "vsub.vv v10, v5, v16\n\t"
        "vse16.v v10, (%[r1])\n\t"
        :
        : [e02] "r"(e02), [e12] "r"(e12), [e03] "r"(e03),
          [e13] "r"(e13), [w00] "r"(w00), [w10] "r"(w10),
          [w01] "r"(w01), [w11] "r"(w11), [y0] "r"(y0),
          [y1] "r"(y1), [s00] "r"(s00), [s01] "r"(s01),
          [s11] "r"(s11), [r0] "r"(r0), [r1] "r"(r1)
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
        "vsub.vv v10, v1, v8\n\t"
        "vse16.v v10, (%[s00])\n\t"

        "vle16.v v20, (%[e02])\n\t"
        "vle16.v v24, (%[w01])\n\t"
        ".word 0xCF4C0857\n\t"
        "vmv.v.v v8, v16\n\t"
        "vle16.v v20, (%[e12])\n\t"
        "vle16.v v24, (%[w11])\n\t"
        ".word 0xCF4C0857\n\t"
        "vadd.vv v8, v8, v16\n\t"
        "vsub.vv v10, v3, v8\n\t"
        "vse16.v v10, (%[s01])\n\t"

        "vle16.v v20, (%[e03])\n\t"
        "vle16.v v24, (%[w01])\n\t"
        ".word 0xCF4C0857\n\t"
        "vmv.v.v v8, v16\n\t"
        "vle16.v v20, (%[e13])\n\t"
        "vle16.v v24, (%[w11])\n\t"
        ".word 0xCF4C0857\n\t"
        "vadd.vv v8, v8, v16\n\t"
        "vsub.vv v10, v7, v8\n\t"
        "vse16.v v10, (%[s11])\n\t"

        "vle16.v v20, (%[e02])\n\t"
        "vle16.v v24, (%[y0])\n\t"
        ".word 0xCF4C0857\n\t"
        "vmv.v.v v8, v16\n\t"
        "vle16.v v20, (%[e12])\n\t"
        "vle16.v v24, (%[y1])\n\t"
        ".word 0xCF4C0857\n\t"
        "vadd.vv v8, v8, v16\n\t"
        "vsub.vv v10, v30, v8\n\t"
        "vse16.v v10, (%[r0])\n\t"

        "vle16.v v20, (%[e03])\n\t"
        "vle16.v v24, (%[y0])\n\t"
        ".word 0xCF4C0857\n\t"
        "vmv.v.v v8, v16\n\t"
        "vle16.v v20, (%[e13])\n\t"
        "vle16.v v24, (%[y1])\n\t"
        ".word 0xCF4C0857\n\t"
        "vadd.vv v8, v8, v16\n\t"
        "vsub.vv v10, v5, v8\n\t"
        "vse16.v v10, (%[r1])\n\t"
        :
        : [e02] "r"(e02), [e12] "r"(e12), [e03] "r"(e03),
          [e13] "r"(e13), [w00] "r"(w00), [w10] "r"(w10),
          [w01] "r"(w01), [w11] "r"(w11), [y0] "r"(y0),
          [y1] "r"(y1), [s00] "r"(s00), [s01] "r"(s01),
          [s11] "r"(s11), [r0] "r"(r0), [r1] "r"(r1)
        : "memory");
#endif
}
#endif

static MIMO_ALWAYS_INLINE void
mimo_ext_inline_top2_store32_q15(
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
#if MIMO_USE_EXT_ZF4_TOP2_MACC_SUM
        "vmv.v.i v16, 0\n\t"
        "vle16.v v8, (%[y0])\n\t"
        "vle16.v v20, (%[w00])\n\t"
        "vle16.v v24, (%[x2])\n\t"
        ".word 0xD34C0857\n\t"
        "vle16.v v20, (%[w01])\n\t"
        "vle16.v v24, (%[x3])\n\t"
        ".word 0xD34C0857\n\t"
        "vsub.vv v8, v8, v16\n\t"
        "vsse32.v v8, (%[x0]), t1, v0.t\n\t"
        "vmv.v.i v16, 0\n\t"
        "vle16.v v10, (%[y1])\n\t"
        "vle16.v v20, (%[w10])\n\t"
        "vle16.v v24, (%[x2])\n\t"
        ".word 0xD34C0857\n\t"
        "vle16.v v20, (%[w11])\n\t"
        "vle16.v v24, (%[x3])\n\t"
        ".word 0xD34C0857\n\t"
        "vsub.vv v10, v10, v16\n\t"
#else
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
#endif
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

static MIMO_ALWAYS_INLINE void
mimo_ext_inline_upper_i16_schur_zf_q15(
    mimo_site4_upper_i16_t *restrict work,
    int mmse,
    mimo_c16_t *restrict x_block)
{
    mimo_ext_inline_upper_i16_pack_t p MIMO_ALIGN64;
#if MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_ONLY || \
    MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_MASK
    const int b_all_arshift =
        MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_ONLY ||
        (MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_MASK & 1);
    const int b_w0_arshift_mask =
        (b_all_arshift || (MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_MASK & 4))
            ? 15
            : 0;
    const int b_w1_arshift_mask =
        (b_all_arshift || (MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_MASK & 8))
            ? 15
            : 0;
    const int b_y_arshift_mask =
        (b_all_arshift || (MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_MASK & 16))
            ? 15
            : ((MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_MASK >> 5) & 15);
    const int s_arshift_mask =
        MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_ONLY ||
                (MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_MASK & 2)
            ? 15
            : ((MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_MASK >> 9) & 15);
#endif
    const int b_w0_trunc_mask =
        mmse && (MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK & 1) ? 15 : 0;
    const int b_w1_trunc_mask =
        mmse && (MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK & 2) ? 15 : 0;
    const int b_y_trunc_mask =
        mmse && (MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK & 4) ? 15 : 0;
    const int s_trunc_mask =
        mmse && ((MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK & 8) ||
                 MIMO_USE_EXT_MMSE4_SCHUR_FINAL_SIGNED_TRUNC_SCALE)
            ? 15
            : 0;

#if MIMO_USE_EXT_ZF4_INLINE_HEAD_WORK_REUSE
#define MIMO_EXT_INLINE_W00 work->a[0]
#define MIMO_EXT_INLINE_W10 work->a[1]
#define MIMO_EXT_INLINE_W01 work->a[4]
#define MIMO_EXT_INLINE_W11 work->b[0]
#define MIMO_EXT_INLINE_Y0 work->b[1]
#define MIMO_EXT_INLINE_Y1 work->a[7]
#else
#define MIMO_EXT_INLINE_W00 p.w00
#define MIMO_EXT_INLINE_W10 p.w10
#define MIMO_EXT_INLINE_W01 p.w01
#define MIMO_EXT_INLINE_W11 p.w11
#define MIMO_EXT_INLINE_Y0 p.y0
#define MIMO_EXT_INLINE_Y1 p.y1
#endif

    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
        const int16_t a00r = work->a[0][lane].r;
        const int16_t a01r = work->a[1][lane].r;
        const int16_t a01i = work->a[1][lane].i;
        const int16_t a02r = work->a[2][lane].r;
        const int16_t a02i = work->a[2][lane].i;
        const int16_t a03r = work->a[3][lane].r;
        const int16_t a03i = work->a[3][lane].i;
        const int16_t a11r = work->a[4][lane].r;
        const int16_t a12r = work->a[5][lane].r;
        const int16_t a12i = work->a[5][lane].i;
        const int16_t a13r = work->a[6][lane].r;
        const int16_t a13i = work->a[6][lane].i;
        const int16_t b0r = work->b[0][lane].r;
        const int16_t b0i = work->b[0][lane].i;
        const int16_t b1r = work->b[1][lane].r;
        const int16_t b1i = work->b[1][lane].i;
        const int64_t b_det_q30 =
            mimo_ext_inline_det2_q30(a00r, a01r, a01i, a11r);
        const mimo_ext_inline_schur4_scale_t b_scale =
            mimo_ext_inline_schur4_scale_from_det_q30(b_det_q30);

#if MIMO_USE_EXT_ZF4_SOLVE2_DIRECT_STORE
        mimo_ext_inline_solve2_store_q15(
            a00r, a01r, a01i, a11r, b_scale,
            a02r, a02i, a12r, a12i,
            MIMO_EXT_INLINE_W00, MIMO_EXT_INLINE_W10, lane
            MIMO_EXT_INLINE_SOLVE2_ADJUST_ARG(
                b_w0_arshift_mask, b_w0_trunc_mask));
        mimo_ext_inline_solve2_store_q15(
            a00r, a01r, a01i, a11r, b_scale,
            a03r, a03i, a13r, a13i,
            MIMO_EXT_INLINE_W01, MIMO_EXT_INLINE_W11, lane
            MIMO_EXT_INLINE_SOLVE2_ADJUST_ARG(
                b_w1_arshift_mask, b_w1_trunc_mask));
        mimo_ext_inline_solve2_store_q15(
            a00r, a01r, a01i, a11r, b_scale,
            b0r, b0i, b1r, b1i,
            MIMO_EXT_INLINE_Y0, MIMO_EXT_INLINE_Y1, lane
            MIMO_EXT_INLINE_SOLVE2_ADJUST_ARG(
                b_y_arshift_mask, b_y_trunc_mask));
#else
        int16_t w00r, w00i, w10r, w10i;
        int16_t w01r, w01i, w11r, w11i;
        int16_t y0r, y0i, y1r, y1i;

        mimo_ext_inline_solve2_q15(a00r, a01r, a01i, a11r, b_scale,
                                   a02r, a02i, a12r, a12i,
                                   &w00r, &w00i, &w10r, &w10i
                                   MIMO_EXT_INLINE_SOLVE2_ADJUST_ARG(
                                       b_w0_arshift_mask, b_w0_trunc_mask));
        mimo_ext_inline_solve2_q15(a00r, a01r, a01i, a11r, b_scale,
                                   a03r, a03i, a13r, a13i,
                                   &w01r, &w01i, &w11r, &w11i
                                   MIMO_EXT_INLINE_SOLVE2_ADJUST_ARG(
                                       b_w1_arshift_mask, b_w1_trunc_mask));
        mimo_ext_inline_solve2_q15(a00r, a01r, a01i, a11r, b_scale,
                                   b0r, b0i, b1r, b1i,
                                   &y0r, &y0i, &y1r, &y1i
                                   MIMO_EXT_INLINE_SOLVE2_ADJUST_ARG(
                                       b_y_arshift_mask, b_y_trunc_mask));

        MIMO_EXT_INLINE_W00[lane] = mimo_ext_inline_state_c16(w00r, w00i);
        MIMO_EXT_INLINE_W10[lane] = mimo_ext_inline_state_c16(w10r, w10i);
        MIMO_EXT_INLINE_W01[lane] = mimo_ext_inline_state_c16(w01r, w01i);
        MIMO_EXT_INLINE_W11[lane] = mimo_ext_inline_state_c16(w11r, w11i);
        MIMO_EXT_INLINE_Y0[lane] = mimo_ext_inline_state_c16(y0r, y0i);
        MIMO_EXT_INLINE_Y1[lane] = mimo_ext_inline_state_c16(y1r, y1i);
#endif
    }

#if MIMO_EXT_USE_LIVE_TAIL_SCHUR
    mimo_ext_inline_schur5_live_tail_q15(
        work->a[2], work->a[5], work->a[3], work->a[6],
        MIMO_EXT_INLINE_W00, MIMO_EXT_INLINE_W10,
        MIMO_EXT_INLINE_W01, MIMO_EXT_INLINE_W11,
        MIMO_EXT_INLINE_Y0, MIMO_EXT_INLINE_Y1,
#if MIMO_USE_EXT_ZF4_TAIL_SCRATCH_REUSE
        work->a[7], work->a[8], work->a[9], work->b[2], work->b[3]);
#else
        p.s00, p.s01, p.s11, p.r0, p.r1);
#endif
#else
    mimo_ext_inline_schur5_q15(
        work->a[2], work->a[5], work->a[3], work->a[6],
        MIMO_EXT_INLINE_W00, MIMO_EXT_INLINE_W10,
        MIMO_EXT_INLINE_W01, MIMO_EXT_INLINE_W11,
        MIMO_EXT_INLINE_Y0, MIMO_EXT_INLINE_Y1,
        work->a[7], work->a[8], work->a[9],
        work->b[2], work->b[3],
        p.s00, p.s01, p.s11, p.r0, p.r1);
#endif

    for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
 #if MIMO_USE_EXT_ZF4_TAIL_SCRATCH_REUSE
        const int16_t s00r = work->a[7][lane].r;
        const int16_t s01r = work->a[8][lane].r;
        const int16_t s01i = work->a[8][lane].i;
        const int16_t s11r = work->a[9][lane].r;
        const mimo_c16_t r0_lane = work->b[2][lane];
        const mimo_c16_t r1_lane = work->b[3][lane];
 #else
        const int16_t s00r = p.s00[lane].r;
        const int16_t s01r = p.s01[lane].r;
        const int16_t s01i = p.s01[lane].i;
        const int16_t s11r = p.s11[lane].r;
        const mimo_c16_t r0_lane = p.r0[lane];
        const mimo_c16_t r1_lane = p.r1[lane];
 #endif
        const int64_t s_det_q30 =
            mimo_ext_inline_det2_q30(s00r, s01r, s01i, s11r);
        const mimo_ext_inline_schur4_scale_t s_scale =
            mimo_ext_inline_schur4_scale_from_det_q30(s_det_q30);

#if MIMO_USE_EXT_ZF4_SOLVE2_DIRECT_STORE
        mimo_ext_inline_solve2_store_q15(s00r, s01r, s01i, s11r, s_scale,
                                         r0_lane.r, r0_lane.i,
                                         r1_lane.r, r1_lane.i,
                                         p.x2, p.x3, lane
                                         MIMO_EXT_INLINE_SOLVE2_ADJUST_ARG(
                                             s_arshift_mask, s_trunc_mask));
#else
        int16_t x2r, x2i, x3r, x3i;

        mimo_ext_inline_solve2_q15(s00r, s01r, s01i, s11r, s_scale,
                                   r0_lane.r, r0_lane.i,
                                   r1_lane.r, r1_lane.i,
                                   &x2r, &x2i, &x3r, &x3i
                                   MIMO_EXT_INLINE_SOLVE2_ADJUST_ARG(
                                       s_arshift_mask, s_trunc_mask));
        p.x2[lane] = mimo_ext_inline_state_c16(x2r, x2i);
        p.x3[lane] = mimo_ext_inline_state_c16(x3r, x3i);
#endif
    }

    mimo_ext_inline_top2_store32_q15(
        MIMO_EXT_INLINE_W00, MIMO_EXT_INLINE_W10,
        MIMO_EXT_INLINE_W01, MIMO_EXT_INLINE_W11,
        p.x2, p.x3, MIMO_EXT_INLINE_Y0, MIMO_EXT_INLINE_Y1, x_block);

#undef MIMO_EXT_INLINE_W00
#undef MIMO_EXT_INLINE_W10
#undef MIMO_EXT_INLINE_W01
#undef MIMO_EXT_INLINE_W11
#undef MIMO_EXT_INLINE_Y0
#undef MIMO_EXT_INLINE_Y1
}

#undef MIMO_EXT_INLINE_SOLVE2_ADJUST_PARAM
#undef MIMO_EXT_INLINE_SOLVE2_ADJUST_ARG
#undef MIMO_EXT_INLINE_SCHUR4_DET_Q15_RECIP_MIN
#endif

#if MIMO_USE_EXT_MMSE4_TRUE16B_INLINE_SOLVE || \
    MIMO_USE_EXT_MMSE4_TRUE16B_LIVE_TAIL_SOLVE || \
    MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE || \
    MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_BSOLVE || \
    MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SPLIT_SOLVE
static MIMO_ALWAYS_INLINE int16_t mimo_ext_upper_i16_store_i16(int32_t x)
{
#if MIMO_USE_SOLVE_BRANCHLIGHT_ROUND || MIMO_USE_SOLVE_STORE_TRUST_I16
    return (int16_t)x;
#else
    if (x > 32767) {
        return 32767;
    }
    if (x < -32768) {
        return -32768;
    }
    return (int16_t)x;
#endif
}

static MIMO_ALWAYS_INLINE void
mimo_ext_upper_i16_add_mmse_lambda_q15(mimo_site4_upper_i16_t *work)
{
    const int diag_slots[4] = {0, 4, 7, 9};

    for (int d = 0; d < 4; ++d) {
        const int slot = diag_slots[d];
        for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
            work->a[slot][lane].r = mimo_ext_upper_i16_store_i16(
                (int32_t)work->a[slot][lane].r + MIMO_MMSE_LAMBDA_Q15);
        }
    }
}

#if MIMO_USE_EXT_MMSE4_TRUE16B_LIVE_TAIL_SOLVE
static MIMO_ALWAYS_INLINE void
mimo_ext_upper_i16_add_mmse_lambda_head_q15(mimo_site4_upper_i16_t *work)
{
    const int diag_slots[2] = {0, 4};

    for (int d = 0; d < 2; ++d) {
        const int slot = diag_slots[d];
        for (int lane = 0; lane < MIMO_SITE4_LANES; ++lane) {
            work->a[slot][lane].r = mimo_ext_upper_i16_store_i16(
                (int32_t)work->a[slot][lane].r + MIMO_MMSE_LAMBDA_Q15);
        }
    }
}

static MIMO_ALWAYS_INLINE void
mimo_ext_mmse4_live_tail_add_lambda_q15(void)
{
    asm volatile(
        "vid.v v8\n\t"
        "vand.vi v8, v8, 1\n\t"
        "vrsub.vi v8, v8, 1\n\t"
        "vsll.vi v8, v8, 9\n\t"
        "vsadd.vv v1, v1, v8\n\t"
        "vsadd.vv v7, v7, v8\n\t"
        :
        :
        : "memory");
}
#endif
#endif

static MIMO_ALWAYS_INLINE void mimo_ext_eq4_site4_run(
    const mimo_c16_t *h_site4,
    const mimo_c16_t *y_site4,
    int mmse,
    mimo_c16_t *x)
{
    const size_t blocks = MIMO_EQ4_SITE_COUNT / MIMO_SITE4_LANES;

    mimo_ext_set_vxrm_rnu_once();
#if MIMO_USE_SITE4_PRELUDE_HOIST && \
    (!MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE || \
     MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PRESERVE_PRELUDE)
    mimo_ext_site4_prelude_asm();
#endif
#if MIMO_USE_RVV_SOLVE4 && !MIMO_USE_RECIP_TABLE && \
    !MIMO_USE_SOLVE4_UNROLL_V2A && \
    !MIMO_USE_SOLVE4_HERMITIAN && !MIMO_USE_SOLVE4_LDLH && \
    !MIMO_USE_SOLVE4_SCHUR && \
    !MIMO_COLLECT_SOLVE_DEN_STATS && !MIMO_COLLECT_SOLVE_VALUE_STATS && \
    !MIMO_COLLECT_SCHUR_STATS
    mimo_site4_soa4_t work MIMO_ALIGN64;
    for (size_t block = 0; block < blocks; ++block) {
        mimo_ext_build_normal4_site4_soa_q15(h_site4, y_site4, block, &work);
        mimo_rvv_solve_normal4_site4_q15(
            &work, mmse, &x[block * MIMO_SITE4_LANES * MIMO_N4]);
    }
#else
#if MIMO_USE_SITE4_SOLVE_SOA
#if MIMO_USE_EXT_ZF4_TRUE16B_WORK
    if (!mmse || MIMO_USE_EXT_MMSE4_TRUE16B_INLINE_SOLVE ||
        MIMO_USE_EXT_MMSE4_TRUE16B_LIVE_TAIL_SOLVE ||
        MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE ||
        MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_BSOLVE ||
        MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SPLIT_SOLVE) {
        mimo_site4_upper_i16_t work MIMO_ALIGN64;
#if MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PRESERVE_PRELUDE
        uint8_t prelude_state[MIMO_EXT_SITE4_PRELUDE_STATE_BYTES] MIMO_ALIGN64;
#endif

        for (size_t block = 0; block < blocks; ++block) {
#if MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE && \
    !MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PRESERVE_PRELUDE
            mimo_ext_site4_prelude_asm();
#endif
            mimo_ext_build_hermitian4_site4_upper_i16_q15(
                h_site4, y_site4, block, &work);
            if (mmse) {
#if MIMO_USE_EXT_MMSE4_TRUE16B_INLINE_SOLVE || \
    MIMO_USE_EXT_MMSE4_TRUE16B_LIVE_TAIL_SOLVE || \
    MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE || \
    MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_BSOLVE || \
    MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SPLIT_SOLVE
#if MIMO_USE_EXT_MMSE4_TRUE16B_LIVE_TAIL_SOLVE
                mimo_ext_upper_i16_add_mmse_lambda_head_q15(&work);
                mimo_ext_mmse4_live_tail_add_lambda_q15();
#elif MIMO_USE_EXT_MMSE4_RVV_I32_VEC_LAMBDA && \
    MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE
                /* The RVV i32 solve helper adds the MMSE lambda after load. */
#else
                mimo_ext_upper_i16_add_mmse_lambda_q15(&work);
#endif
#else
                __builtin_unreachable();
#endif
            }
#if MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE
#if MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PRESERVE_PRELUDE
            mimo_ext_site4_save_prelude_state(prelude_state);
#endif
 #if MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SPLIT_SOLVE >= 2
            mimo_solve_normal4_site4_upper_i16_schur_zf_rvv_i32_split_ext_q15(
                &work, mmse, &x[block * MIMO_SITE4_LANES * MIMO_N4]);
 #elif MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SPLIT_SOLVE == 1
            mimo_solve_normal4_site4_upper_i16_schur_zf_rvv_i32_split_q15(
                &work, mmse, &x[block * MIMO_SITE4_LANES * MIMO_N4]);
 #else
            mimo_solve_normal4_site4_upper_i16_schur_zf_rvv_i32_q15(
                &work, mmse, &x[block * MIMO_SITE4_LANES * MIMO_N4]);
 #endif
#if MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PRESERVE_PRELUDE
            mimo_ext_site4_restore_prelude_state(prelude_state);
#endif
#else
#if MIMO_USE_EXT_ZF4_INLINE_UPPER_I16_SOLVE
            mimo_ext_inline_upper_i16_schur_zf_q15(
                &work, mmse, &x[block * MIMO_SITE4_LANES * MIMO_N4]);
#else
            mimo_solve_normal4_site4_upper_i16_schur_zf_q15(
                &work, mmse, &x[block * MIMO_SITE4_LANES * MIMO_N4]);
#endif
#endif
        }
        return;
    }
#endif
#if MIMO_USE_SITE4_BUILD_SOLVE_PIPELINE2
    if (!mmse) {
        mimo_site4_soa4_t work0 MIMO_ALIGN64;
        mimo_site4_soa4_t work1 MIMO_ALIGN64;

        mimo_ext_build_hermitian4_site4_upper_soa_q15(
            h_site4, y_site4, 0, &work0);
        for (size_t block = 1; block < blocks; ++block) {
            mimo_site4_soa4_t *build_work =
                (block & 1u) ? &work1 : &work0;
            mimo_site4_soa4_t *solve_work =
                (block & 1u) ? &work0 : &work1;

            mimo_ext_build_hermitian4_site4_upper_soa_q15(
                h_site4, y_site4, block, build_work);
            mimo_solve_normal4_site4_soa_schur_zf_q15(
                solve_work,
                &x[(block - 1u) * MIMO_SITE4_LANES * MIMO_N4]);
        }

        mimo_site4_soa4_t *last_work =
            ((blocks - 1u) & 1u) ? &work1 : &work0;
        mimo_solve_normal4_site4_soa_schur_zf_q15(
            last_work, &x[(blocks - 1u) * MIMO_SITE4_LANES * MIMO_N4]);
        return;
    }
#endif
    mimo_site4_soa4_t work MIMO_ALIGN64;
    for (size_t block = 0; block < blocks; ++block) {
#if MIMO_USE_SOLVE4_HERMITIAN && MIMO_USE_SITE4_HERMITIAN_UPPER_BUILD && \
    !MIMO_COLLECT_SCHUR_STATS
        if (!mmse || MIMO_USE_SITE4_HERMITIAN_UPPER_MMSE) {
#if MIMO_USE_EXT_MMSE4_FUSED_BUILD_SOLVE
            if (mmse) {
                mimo_ext_eq4_build_solve_fused_rowfuse_soa_q15(
                    h_site4, y_site4, block,
                    &x[block * MIMO_SITE4_LANES * MIMO_N4]);
                continue;
            }
#endif
#if MIMO_USE_EXT_ZF4_RVV_ROUND_ONCE_BUILD
            mimo_rvv_build_hermitian4_site4_upper_soa_q15(
                h_site4, y_site4, block, &work);
 #if MIMO_USE_SITE4_PRELUDE_HOIST
            mimo_ext_site4_prelude_asm();
 #endif
#else
            mimo_ext_build_hermitian4_site4_upper_soa_q15(
                h_site4, y_site4, block, &work);
#endif
#if MIMO_DEBUG_ZF4_BUILD_ONLY
            if (!mmse) {
                mimo_ext_store_zero4_site4(
                    &x[block * MIMO_SITE4_LANES * MIMO_N4]);
                continue;
            }
#endif
            if (mmse) {
#if MIMO_USE_SOLVE4_SCHUR
#if MIMO_USE_EXT_MMSE4_SCHUR_RVV_SOLVE
#if MIMO_USE_EXT_MMSE4_FUSED_BUILD_SOLVE
                mimo_ext_eq4_build_solve_fused_rowfuse_soa_q15(
                    h_site4, y_site4, block,
                    &x[block * MIMO_SITE4_LANES * MIMO_N4]);
#else
                mimo_ext_mmse4_schur_rvv_solve_soa_q15(
                    &work, &x[block * MIMO_SITE4_LANES * MIMO_N4]);
#endif
#elif MIMO_USE_SOLVE4_SCHUR_MMSE_SPECIALIZE
                mimo_solve_normal4_site4_soa_schur_mmse_q15(
                    &work, &x[block * MIMO_SITE4_LANES * MIMO_N4]);
#else
                mimo_solve_normal4_site4_soa_schur_q15(
                    &work, 1, &x[block * MIMO_SITE4_LANES * MIMO_N4]);
#endif
                continue;
#endif
            }
#if MIMO_USE_EXT_ZF4_SCHUR_RVV_SOLVE
            mimo_ext_zf4_schur_rvv_solve_soa_q15(
                &work, &x[block * MIMO_SITE4_LANES * MIMO_N4]);
#elif MIMO_USE_SOLVE4_SCHUR
            mimo_solve_normal4_site4_soa_schur_zf_q15(
                &work, &x[block * MIMO_SITE4_LANES * MIMO_N4]);
#elif MIMO_USE_EXT_ZF4_LOCAL_SOLVE
            mimo_ext_zf4_local_solve_soa_q15(
                &work, &x[block * MIMO_SITE4_LANES * MIMO_N4]);
#elif MIMO_USE_EXT_ZF4_UPPER_RVV_SOLVE
            mimo_rvv_solve_zf4_upper_hermitian_site4_q15(
                &work, &x[block * MIMO_SITE4_LANES * MIMO_N4]);
#elif MIMO_USE_EXT_ZF4_MIXED_UPDATE_SOLVE
            mimo_rvv_solve_zf4_upper_hermitian_site4_mixed_update_q15(
                &work, &x[block * MIMO_SITE4_LANES * MIMO_N4]);
#elif MIMO_USE_EXT_ZF4_RVV_NODIV_FAST_SOLVE
            mimo_ext_zf4_rvv_nodiv_fast_solve_soa_q15(
                &work, &x[block * MIMO_SITE4_LANES * MIMO_N4]);
#elif MIMO_USE_EXT_ZF4_RVV_NODIV_SOLVE
            mimo_ext_zf4_rvv_nodiv_solve_soa_q15(
                &work, &x[block * MIMO_SITE4_LANES * MIMO_N4]);
#else
            mimo_solve_normal4_site4_soa_hermitian_zf_q15(
                &work, &x[block * MIMO_SITE4_LANES * MIMO_N4]);
#endif
            continue;
        }
#endif
        mimo_ext_build_normal4_site4_soa_q15(h_site4, y_site4, block, &work);
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
            mimo_ext_build_hermitian4_site4_upper_q15(
                h_site4, y_site4, block, a_lane, b_lane);
            mimo_solve_normal4_site4_hermitian_zf_q15(
                a_lane, b_lane,
                &x[block * MIMO_SITE4_LANES * MIMO_N4]);
            continue;
        }
#endif
        mimo_ext_build_normal4_site4_q15(h_site4, y_site4, block, a_lane,
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

#undef MIMO_EXT_SITE4_PRELUDE_ASM
#undef MIMO_EXT_SITE4_ACCUM_ASM
#undef MIMO_EXT_SITE4_ACCUM_REAL_ASM
#undef MIMO_EXT_SITE4_ACCUM_NEG_IM_ASM

#endif
