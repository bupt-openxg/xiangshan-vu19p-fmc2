/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this file
 * except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.openairinterface.org/?page_id=698
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */

#include "fft_test_common.h"
#include "fft_pow2_twiddles_precomputed.h"
#include "fft_pow2_rvv_twiddles_precomputed.h"
#include "fft_pow2_rvv_twiddles_4096_precomputed.h"
#include "klib.h"
#include <stdint.h>
#include <riscv_vector.h>

#define MSTATUS_VS_SHIFT        9
#define MSTATUS_VS_MASK         (0x3UL << MSTATUS_VS_SHIFT)
#define MSTATUS_VS_CLEAN        (0x2UL << MSTATUS_VS_SHIFT)

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

#include "fft_mainline_config.h"

#ifndef FFT_IMPL_DISPLAY_NAME
#define FFT_IMPL_DISPLAY_NAME "RVV"
#endif

#ifndef FFT_IMPL_SOURCE_NAME
#define FFT_IMPL_SOURCE_NAME "dft_rvv.c"
#endif

#ifndef FFT_IMPL_VALIDATE_PRECHECK
#define FFT_IMPL_VALIDATE_PRECHECK() (1)
#endif

#ifndef FFT_CUSTOM_ROTATE_ADD_HELPERS
#define FFT_CUSTOM_ROTATE_ADD_HELPERS 0
#endif

/* Keep the legacy master switch for ABI/signature compatibility, but allow
 * each radix stage to be evaluated independently for bit-exactness. */
#ifndef FFT_CUSTOM_ROTATE_ADD_DFT16
#define FFT_CUSTOM_ROTATE_ADD_DFT16 FFT_CUSTOM_ROTATE_ADD_HELPERS
#endif
#ifndef FFT_CUSTOM_ROTATE_ADD_COMBINE64
#define FFT_CUSTOM_ROTATE_ADD_COMBINE64 FFT_CUSTOM_ROTATE_ADD_HELPERS
#endif
#ifndef FFT_CUSTOM_ROTATE_ADD_COMBINE256
#define FFT_CUSTOM_ROTATE_ADD_COMBINE256 FFT_CUSTOM_ROTATE_ADD_HELPERS
#endif
#ifndef FFT_CUSTOM_ROTATE_ADD_COMBINE1024
#define FFT_CUSTOM_ROTATE_ADD_COMBINE1024 FFT_CUSTOM_ROTATE_ADD_HELPERS
#endif
#ifndef FFT_CUSTOM_ROTATE_ADD_COMBINE4096
#define FFT_CUSTOM_ROTATE_ADD_COMBINE4096 FFT_CUSTOM_ROTATE_ADD_HELPERS
#endif

/* 0032 experiment: replace each ordinary +/-j rotation independently.
 * This is intentionally separate from the fused pair switches because the
 * latter changes the reference saturation order. */
#ifndef FFT_EXT_INDIVIDUAL_ROTATE_DFT16
#define FFT_EXT_INDIVIDUAL_ROTATE_DFT16 0
#endif
#ifndef FFT_EXT_INDIVIDUAL_ROTATE_COMBINE64
#define FFT_EXT_INDIVIDUAL_ROTATE_COMBINE64 0
#endif
#ifndef FFT_EXT_ROTATE_BOUNDARY_FALLBACK
#define FFT_EXT_ROTATE_BOUNDARY_FALLBACK 0
#endif
#if FFT_EXT_ROTATE_BOUNDARY_FALLBACK && FFT_CUSTOM_ROTATE_ADD_HELPERS
#error "boundary fallback requires the ordinary RVV rotate operands"
#endif
#define FFT_CUSTOM_ROTATE_ADD_ANY \
    (FFT_CUSTOM_ROTATE_ADD_DFT16 || FFT_CUSTOM_ROTATE_ADD_COMBINE64 || \
     FFT_CUSTOM_ROTATE_ADD_COMBINE256 || FFT_CUSTOM_ROTATE_ADD_COMBINE1024 || \
     FFT_CUSTOM_ROTATE_ADD_COMBINE4096)

#ifndef FFT_PRELAYOUT_N256
#define FFT_PRELAYOUT_N256 0
#endif

#ifndef FFT_RVV_PARTIAL_LEAF_RESIDENT_N256
#define FFT_RVV_PARTIAL_LEAF_RESIDENT_N256 0
#endif

#ifndef FFT_PRELAYOUT_N64
#define FFT_PRELAYOUT_N64 0
#endif

#ifndef FFT_PRELAYOUT_N128
#define FFT_PRELAYOUT_N128 0
#endif

#ifndef FFT_PRELAYOUT_N512
#define FFT_PRELAYOUT_N512 0
#endif

#ifndef FFT_PRELAYOUT_N1024
#define FFT_PRELAYOUT_N1024 0
#endif

#ifndef FFT_PRELAYOUT_N2048
#define FFT_PRELAYOUT_N2048 0
#endif

#ifndef FFT_PRELAYOUT_N4096
#define FFT_PRELAYOUT_N4096 0
#endif

#ifndef FFT_EXT_FUSED_LEAF_COMBINE_N256
#define FFT_EXT_FUSED_LEAF_COMBINE_N256 0
#endif

#ifndef FFT_EXT_FUSED_LEAF_COMBINE_N64
#define FFT_EXT_FUSED_LEAF_COMBINE_N64 0
#endif

#ifndef FFT_EXT_FUSED_LEAF_COMBINE_N128
#define FFT_EXT_FUSED_LEAF_COMBINE_N128 0
#endif

#ifndef FFT_EXT_FUSED_LEAF_COMBINE_N512
#define FFT_EXT_FUSED_LEAF_COMBINE_N512 0
#endif

#ifndef FFT_EXT_FUSED_LEAF_COMBINE_N1024
#define FFT_EXT_FUSED_LEAF_COMBINE_N1024 0
#endif

#ifndef FFT_EXT_FUSED_LEAF_COMBINE_N2048
#define FFT_EXT_FUSED_LEAF_COMBINE_N2048 0
#endif

#ifndef FFT_EXT_FUSED_LEAF_COMBINE_N4096
#define FFT_EXT_FUSED_LEAF_COMBINE_N4096 0
#endif

#ifndef FFT_EXT_LEAF_TWIDDLE_RESIDENT
#define FFT_EXT_LEAF_TWIDDLE_RESIDENT 0
#endif

/* The native-layout wrapper is needed only when a prelayout is active.  Keep
 * the no-prelayout path on the established dispatcher so the A baseline is
 * not changed by an unused pointer contract. */
#define FFT_NATIVE_LAYOUT_CONTRACT \
    (FFT_PRELAYOUT_N64 || FFT_PRELAYOUT_N128 || FFT_PRELAYOUT_N256 || \
     FFT_PRELAYOUT_N512 || FFT_PRELAYOUT_N1024 || FFT_PRELAYOUT_N2048 || \
     FFT_PRELAYOUT_N4096)

#if FFT_EXT_FUSED_LEAF_COMBINE_N64
static void fft_ext_fused_dft64_forward(const int16_t *src,
                                        int16_t *dst,
                                        unsigned shift);
static void fft_ext_fused_dft64_inverse(const int16_t *src,
                                        int16_t *dst,
                                        unsigned shift);
#endif

#if FFT_EXT_FUSED_LEAF_COMBINE_N128
static void fft_ext_dft128_fused_forward(const int16_t *x, int16_t *y);
static void fft_ext_dft128_fused_inverse(const int16_t *x, int16_t *y);
#endif

#if FFT_EXT_FUSED_LEAF_COMBINE_N256
static void fft_ext_dft256_fused_forward(const int16_t *x,
                                         int16_t *y,
                                         unsigned shift);
static void fft_ext_dft256_fused_inverse(const int16_t *x,
                                         int16_t *y,
                                         unsigned shift);
#endif

#if FFT_EXT_FUSED_LEAF_COMBINE_N512
static void fft_ext_dft512_fused_forward(const int16_t *x, int16_t *y);
static void fft_ext_dft512_fused_inverse(const int16_t *x, int16_t *y);
#endif

#if FFT_EXT_FUSED_LEAF_COMBINE_N1024
static void fft_ext_dft1024_fused_forward(const int16_t *x,
                                          int16_t *y,
                                          unsigned scale);
static void fft_ext_dft1024_fused_inverse(const int16_t *x,
                                          int16_t *y,
                                          unsigned scale);
#endif

#if FFT_EXT_FUSED_LEAF_COMBINE_N2048
static void fft_ext_dft2048_fused_forward(const int16_t *x, int16_t *y);
static void fft_ext_dft2048_fused_inverse(const int16_t *x, int16_t *y);
#endif

#if FFT_EXT_FUSED_LEAF_COMBINE_N4096
static void fft_ext_dft4096_fused_forward(const int16_t *x, int16_t *y);
static void fft_ext_dft4096_fused_inverse(const int16_t *x, int16_t *y);
#endif

#if FFT_CUSTOM_PACKED_CMUL_HELPERS || FFT_CUSTOM_ROTATE_ADD_ANY || \
    FFT_EXT_INDIVIDUAL_ROTATE_DFT16 || FFT_EXT_INDIVIDUAL_ROTATE_COMBINE64
#define FFT_EXT_SET_E16M1_VL8() ext_set_e16m1_vl8()
#else
#define FFT_EXT_SET_E16M1_VL8() ((void)0)
#endif

#ifndef FFT_VALIDATE_ALLOW_NUMERIC_ERROR
#define FFT_VALIDATE_ALLOW_NUMERIC_ERROR 0
#endif

// Maintained RVV mainline policy:
// - the 16-point leaf uses a register-shuffle transpose
// - 4x4 transpose / leaf16 layout transforms use e32 strided load/store
// - 256/512/1024/2048/4096 keep only the split-to-leaf16 recursive mainline
// - 2048 now follows the same direct-to-leaf16 fast path as the other large sizes

// Packed complex multiply keeps the source in [re0, im0, re1, im1, ...].
// For W = c - j*s, use:
//   a * W = a * [c, c, ...] + swap(a) * [s, -s, ...]
// where swap(a) = [im0, re0, im1, re1, ...].
static const int16_t tw16_re[24] __attribute__((aligned(16))) = {
    32767, 32767, 30272, 30272, 23169, 23169, 12539, 12539,
    32767, 32767, 23169, 23169,     0,     0,-23170,-23170,
    32767, 32767, 12539, 12539,-23170,-23170,-30273,-30273
};

static const int16_t tw16_im[24] __attribute__((aligned(16))) = {
        0,     0, 12540,-12540, 23170,-23170, 30273,-30273,
        0,     0, 23170,-23170, 32767,-32767, 23170,-23170,
        0,     0, 30273,-30273, 23170,-23170,-12539, 12539
};

static const int16_t tw16_im_idft[24] __attribute__((aligned(16))) = {
        0,     0,-12540, 12540,-23170, 23170,-30273, 30273,
        0,     0,-23170, 23170,-32767, 32767,-23170, 23170,
        0,     0,-30273, 30273,-23170, 23170, 12539,-12539
};

static const uint16_t swap_re_im_idx16[8] __attribute__((aligned(16))) = {
    1, 0, 3, 2, 5, 4, 7, 6
};

static const int16_t minus_j_sign16[8] __attribute__((aligned(16))) = {
    1, -1, 1, -1, 1, -1, 1, -1
};

static const uint32_t split_even_idx32[4] __attribute__((aligned(16))) = {
    0, 2, 0, 0
};

static const uint32_t split_odd_idx32[4] __attribute__((aligned(16))) = {
    1, 3, 0, 0
};

static volatile int16_t g_fft_perf_sink;
static int16_t g_rvv4096_split_tmp[2 * 4096] __attribute__((aligned(16)));
static int16_t g_rvv2048_split_tmp[2 * 2048] __attribute__((aligned(16)));
static int16_t g_rvv1024_split_tmp[2 * 1024] __attribute__((aligned(16)));
static int16_t g_rvv512_split_tmp[2 * 512] __attribute__((aligned(16)));
static int16_t g_rvv256_split_tmp[2 * 256] __attribute__((aligned(16)));
static int16_t g_rvv128_split_tmp[2 * 128] __attribute__((aligned(16)));
static int16_t g_rvv64_split_tmp[2 * 64] __attribute__((aligned(16)));
static int16_t g_rvv64_leaf_tmp[2 * 64] __attribute__((aligned(16)));
static const int16_t *const g_rvv64_tw_re_dup[3] = {
    rvv64_tw_re_dup_0,
    rvv64_tw_re_dup_1,
    rvv64_tw_re_dup_2,
};
static const int16_t *const g_rvv64_tw_im_fwd[3] = {
    rvv64_tw_im_fwd_0,
    rvv64_tw_im_fwd_1,
    rvv64_tw_im_fwd_2,
};
static const int16_t *const g_rvv64_tw_im_inv[3] = {
    rvv64_tw_im_inv_0,
    rvv64_tw_im_inv_1,
    rvv64_tw_im_inv_2,
};
static int16_t g_fft_main_input[2 * FFT_MAIN_MAX_POINTS] __attribute__((aligned(16)));
static int16_t g_fft_main_output[2 * FFT_MAIN_MAX_POINTS] __attribute__((aligned(16)));
#if FFT_VALIDATE
static int16_t g_fft_main_ref[2 * FFT_MAIN_MAX_POINTS] __attribute__((aligned(16)));
#endif

#ifndef read_csr
#define read_csr(csr)       ({ unsigned long __v; __asm__ volatile ("csrr %0, " #csr : "=r" (__v)); __v; })
#endif

#ifndef write_csr
#define write_csr(csr, val) ({ unsigned long __v = (unsigned long)(val); __asm__ volatile ("csrw " #csr ", %0" :: "r" (__v)); })
#endif

#ifndef __RISCV_VXRM_RDN
#define __RISCV_VXRM_RDN 2
#endif

static inline vint32m1_t interleave_pairs_i32m1(vint32m1_t a,
                                                vint32m1_t b,
                                                vuint32m1_t idx,
                                                vbool32_t mask_odd,
                                                size_t vl32)
{
    vint32m1_t a_dup = __riscv_vrgather_vv_i32m1(a, idx, vl32);
    vint32m1_t b_dup = __riscv_vrgather_vv_i32m1(b, idx, vl32);
    return __riscv_vmerge_vvm_i32m1(a_dup, b_dup, mask_odd, vl32);
}

static inline vint32m1_t pack_selected_lanes_i32m1(vint32m1_t lo,
                                                   vint32m1_t hi,
                                                   vuint32m1_t idx,
                                                   size_t vl32_pairs)
{
    vint32m1_t lo_sel = __riscv_vrgather_vv_i32m1(lo, idx, vl32_pairs);
    vint32m1_t hi_sel = __riscv_vrgather_vv_i32m1(hi, idx, vl32_pairs);
    return __riscv_vslideup_vx_i32m1(lo_sel, hi_sel, 2, 4);
}

static inline vint16m1_t vnclip_q15_rdn_i16m1(vint32m2_t src)
{
    vint16m1_t dst;

    // Rely on rvv_runtime_init() to keep vxrm at RDN so the hot path does
    // not need to rewrite the CSR every call.
    asm volatile ("vnclip.wi %0, %1, 15" : "=vr" (dst) : "vr" (src));
    return dst;
}

static void set_mstatus_vs_field(uintptr_t vs_value) {
    uintptr_t mstatus_val;

    // 1. 读取当前的 mstatus 值
    mstatus_val = read_csr(mstatus);

    // 2. 清除当前的 VS 域 (Bit 10:9)
    mstatus_val &= ~MSTATUS_VS_MASK;

    // 3. 设置新的 VS 域值
    // 通常设置为 MSTATUS_VS_CLEAN (10) 以启用向量单元
    mstatus_val |= vs_value;

    // 4. 写入新的 mstatus 值
    write_csr(mstatus, mstatus_val);
}

static void __attribute__((noinline)) rvv_runtime_init(void)
{
    unsigned long vcsr_val;

    set_mstatus_vs_field(MSTATUS_VS_CLEAN);
    asm volatile ("" ::: "memory");

    asm volatile ("csrr %0, vcsr" : "=r"(vcsr_val) :: "memory");
    vcsr_val = (vcsr_val & ~0x6UL) | (2UL << 1);
    asm volatile ("csrw vcsr, %0" :: "r"(vcsr_val) : "memory");
}

static inline vint16m1_t swap_re_im_with_idx_i16m1(vint16m1_t v,
                                                   vuint16m1_t swap_idx,
                                                   size_t vl)
{
    return __riscv_vrgather_vv_i16m1(v, swap_idx, vl);
}

static inline vint16m1_t swap_re_im_i16m1(vint16m1_t v, size_t vl)
{
    vuint16m1_t vid = __riscv_vid_v_u16m1(vl);
    vuint16m1_t idx = __riscv_vxor_vx_u16m1(vid, 1u, vl);
    return swap_re_im_with_idx_i16m1(v, idx, vl);
}

// Equivalent to packed complex multiply by -j: (re, im) -> (im, -re).
static inline vint16m1_t complex_mul_minus_j_with_idx_sign_i16m1(vint16m1_t x,
                                                                 vuint16m1_t swap_idx,
                                                                 vint16m1_t sign,
                                                                 size_t vl)
{
    vint16m1_t swapped = swap_re_im_with_idx_i16m1(x, swap_idx, vl);
    return __riscv_vmul_vv_i16m1(swapped, sign, vl);
}

#if FFT_EXT_INDIVIDUAL_ROTATE_DFT16 || FFT_EXT_INDIVIDUAL_ROTATE_COMBINE64
static inline __attribute__((always_inline))
void ext_rotate_minus_j_pair_exact_i16m1(vint16m1_t x0,
                                         vint16m1_t x1,
                                         vint16m1_t zero,
                                         vuint16m1_t swap_idx,
                                         vint16m1_t sign,
                                         size_t vl,
                                         vint16m1_t *r0,
                                         vint16m1_t *r1)
{
#if FFT_EXT_ROTATE_BOUNDARY_FALLBACK
    vint16m1_t pair_min = __riscv_vmin_vv_i16m1(x0, x1, vl);
    vbool16_t has_min =
        __riscv_vmseq_vx_i16m1_b16(pair_min, INT16_MIN, vl);

    if (__riscv_vcpop_m_b16(has_min, vl) != 0) {
        *r0 = complex_mul_minus_j_with_idx_sign_i16m1(x0, swap_idx, sign, vl);
        *r1 = complex_mul_minus_j_with_idx_sign_i16m1(x1, swap_idx, sign, vl);
        return;
    }
#else
    (void)swap_idx;
    (void)sign;
    (void)vl;
#endif
    ext_rotate_minus_j_pair_i16m1(x0, x1, zero, r0, r1);
}
#endif

#if FFT_CUSTOM_ROTATE_ADD_ANY
static inline void radix4_forward_rot_pair_i16m1(vint16m1_t base,
                                                 vint16m1_t x1,
                                                 vint16m1_t x3,
                                                 size_t vl,
                                                 vint16m1_t *y1,
                                                 vint16m1_t *y3)
{
    vint16m1_t delta = __riscv_vssub_vv_i16m1(x1, x3, vl);
    ext_rot_pair_forward_i16m1(delta, base, y1, y3);
}

static inline void radix4_inverse_rot_pair_i16m1(vint16m1_t base,
                                                 vint16m1_t x1,
                                                 vint16m1_t x3,
                                                 size_t vl,
                                                 vint16m1_t *y1,
                                                 vint16m1_t *y3)
{
    vint16m1_t delta = __riscv_vssub_vv_i16m1(x1, x3, vl);
    ext_rot_pair_inverse_i16m1(delta, base, y1, y3);
}
#endif

#ifndef FFT_CUSTOM_PACKED_CMUL_HELPERS
static inline void packed_cmul3_q15_i16m1(vint16m1_t *a0,
                                          vint16m1_t *a1,
                                          vint16m1_t *a2,
                                          vint16m1_t tw_re0,
                                          vint16m1_t tw_im0,
                                          vint16m1_t tw_re1,
                                          vint16m1_t tw_im1,
                                          vint16m1_t tw_re2,
                                          vint16m1_t tw_im2,
                                          vuint16m1_t swap_idx,
                                          size_t vl)
{
    vint16m1_t a0_sw = swap_re_im_with_idx_i16m1(*a0, swap_idx, vl);
    vint16m1_t a1_sw = swap_re_im_with_idx_i16m1(*a1, swap_idx, vl);
    vint16m1_t a2_sw = swap_re_im_with_idx_i16m1(*a2, swap_idx, vl);

    vint32m2_t acc0 = __riscv_vwmul_vv_i32m2(*a0, tw_re0, vl);
    vint32m2_t acc1 = __riscv_vwmul_vv_i32m2(*a1, tw_re1, vl);
    vint32m2_t acc2 = __riscv_vwmul_vv_i32m2(*a2, tw_re2, vl);
    acc0 = __riscv_vwmacc_vv_i32m2(acc0, a0_sw, tw_im0, vl);
    acc1 = __riscv_vwmacc_vv_i32m2(acc1, a1_sw, tw_im1, vl);
    acc2 = __riscv_vwmacc_vv_i32m2(acc2, a2_sw, tw_im2, vl);

    *a0 = vnclip_q15_rdn_i16m1(acc0);
    *a1 = vnclip_q15_rdn_i16m1(acc1);
    *a2 = vnclip_q15_rdn_i16m1(acc2);
}

static inline vint16m1_t packed_cmul_q15_i16m1(vint16m1_t a,
                                               vint16m1_t tw_re,
                                               vint16m1_t tw_im,
                                               vuint16m1_t swap_idx,
                                               size_t vl)
{
    vint16m1_t a_sw = swap_re_im_with_idx_i16m1(a, swap_idx, vl);
    vint32m2_t acc = __riscv_vwmul_vv_i32m2(a, tw_re, vl);
    acc = __riscv_vwmacc_vv_i32m2(acc, a_sw, tw_im, vl);
    return vnclip_q15_rdn_i16m1(acc);
}
#endif

static inline vint16m1_t rvv_mulhrs_q15_i16m1(vint16m1_t v, int16_t coeff, size_t vl)
{
    vint32m2_t prod = __riscv_vwmul_vx_i32m2(v, coeff, vl);
    prod = __riscv_vadd_vx_i32m2(prod, 0x4000, vl);
    prod = __riscv_vsra_vx_i32m2(prod, 15, vl);
    return __riscv_vncvt_x_x_w_i16m1(prod, vl);
}

static inline void transpose4x4_c32_shuffle_rvv(vint16m1_t *x0,
                                                vint16m1_t *x1,
                                                vint16m1_t *x2,
                                                vint16m1_t *x3)
{
    const size_t vl32 = 4;
    vint32m1_t r0 = __riscv_vreinterpret_v_i16m1_i32m1(*x0);
    vint32m1_t r1 = __riscv_vreinterpret_v_i16m1_i32m1(*x1);
    vint32m1_t r2 = __riscv_vreinterpret_v_i16m1_i32m1(*x2);
    vint32m1_t r3 = __riscv_vreinterpret_v_i16m1_i32m1(*x3);
    vuint32m1_t vid = __riscv_vid_v_u32m1(vl32);
    vuint32m1_t idx_lo = __riscv_vsrl_vx_u32m1(vid, 1, vl32);
    vuint32m1_t idx_hi = __riscv_vadd_vx_u32m1(idx_lo, 2u, vl32);
    vuint32m1_t pair_lo_idx = __riscv_vand_vx_u32m1(vid, 1u, vl32);
    vuint32m1_t pair_hi_idx = __riscv_vadd_vx_u32m1(pair_lo_idx, 2u, vl32);
    vbool32_t mask_odd =
        __riscv_vmsne_vx_u32m1_b32(__riscv_vand_vx_u32m1(vid, 1u, vl32), 0u, vl32);

    vint32m1_t ab_lo = interleave_pairs_i32m1(r0, r1, idx_lo, mask_odd, vl32);
    vint32m1_t ab_hi = interleave_pairs_i32m1(r0, r1, idx_hi, mask_odd, vl32);
    vint32m1_t cd_lo = interleave_pairs_i32m1(r2, r3, idx_lo, mask_odd, vl32);
    vint32m1_t cd_hi = interleave_pairs_i32m1(r2, r3, idx_hi, mask_odd, vl32);

    r0 = pack_selected_lanes_i32m1(ab_lo, cd_lo, pair_lo_idx, vl32);
    r1 = pack_selected_lanes_i32m1(ab_lo, cd_lo, pair_hi_idx, vl32);
    r2 = pack_selected_lanes_i32m1(ab_hi, cd_hi, pair_lo_idx, vl32);
    r3 = pack_selected_lanes_i32m1(ab_hi, cd_hi, pair_hi_idx, vl32);

    *x0 = __riscv_vreinterpret_v_i32m1_i16m1(r0);
    *x1 = __riscv_vreinterpret_v_i32m1_i16m1(r1);
    *x2 = __riscv_vreinterpret_v_i32m1_i16m1(r2);
    *x3 = __riscv_vreinterpret_v_i32m1_i16m1(r3);
}

static inline void load_transposed4x4_c32_vlse32_i16m1_rvv(const int16_t *src_block,
                                                           vint16m1_t *x0,
                                                           vint16m1_t *x1,
                                                           vint16m1_t *x2,
                                                           vint16m1_t *x3)
{
    const size_t vl32 = 4;
    const intptr_t row_stride_bytes = (intptr_t)(4 * sizeof(int32_t));
    const int32_t *src32 = (const int32_t *)src_block;

    vint32m1_t r0 = __riscv_vlse32_v_i32m1(src32 + 0, row_stride_bytes, vl32);
    vint32m1_t r1 = __riscv_vlse32_v_i32m1(src32 + 1, row_stride_bytes, vl32);
    vint32m1_t r2 = __riscv_vlse32_v_i32m1(src32 + 2, row_stride_bytes, vl32);
    vint32m1_t r3 = __riscv_vlse32_v_i32m1(src32 + 3, row_stride_bytes, vl32);

    *x0 = __riscv_vreinterpret_v_i32m1_i16m1(r0);
    *x1 = __riscv_vreinterpret_v_i32m1_i16m1(r1);
    *x2 = __riscv_vreinterpret_v_i32m1_i16m1(r2);
    *x3 = __riscv_vreinterpret_v_i32m1_i16m1(r3);
}

static void rvv_split_radix4_64_rvv(const int16_t *src, int16_t *dst)
{
    const size_t vl8 = 8;

    for (int block = 0; block < 4; ++block) {
        const int16_t *src_block = src + block * 32;
        const int dst_off = block * 8;
        vint16m1_t x0, x1, x2, x3;
        load_transposed4x4_c32_vlse32_i16m1_rvv(src_block, &x0, &x1, &x2, &x3);

        __riscv_vse16_v_i16m1(dst + 0 * 32 + dst_off, x0, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 32 + dst_off, x1, vl8);
        __riscv_vse16_v_i16m1(dst + 2 * 32 + dst_off, x2, vl8);
        __riscv_vse16_v_i16m1(dst + 3 * 32 + dst_off, x3, vl8);
    }
}

static void rvv_split_radix4_256_to_leaf16_rvv(const int16_t *src, int16_t *dst)
{
    const size_t vl32 = 4;
    const intptr_t leaf_stride_bytes = (intptr_t)(16 * sizeof(int32_t));

    // Emit the final child-major / leaf-major 16-point inputs directly.
    // Each top-level 4x4 transpose yields one vector per child; a strided
    // 32-bit store then fans that vector out across the four leaf arrays
    // inside that child without materializing the intermediate 64-point split.
    for (int block = 0; block < 16; ++block) {
        const int16_t *src_block = src + block * 32;
        vint16m1_t x0, x1, x2, x3;
        load_transposed4x4_c32_vlse32_i16m1_rvv(src_block, &x0, &x1, &x2, &x3);

        __riscv_vsse32_v_i32m1((int32_t *)(dst + 0 * 128 + block * 2),
                               leaf_stride_bytes,
                               __riscv_vreinterpret_v_i16m1_i32m1(x0),
                               vl32);
        __riscv_vsse32_v_i32m1((int32_t *)(dst + 1 * 128 + block * 2),
                               leaf_stride_bytes,
                               __riscv_vreinterpret_v_i16m1_i32m1(x1),
                               vl32);
        __riscv_vsse32_v_i32m1((int32_t *)(dst + 2 * 128 + block * 2),
                               leaf_stride_bytes,
                               __riscv_vreinterpret_v_i16m1_i32m1(x2),
                               vl32);
        __riscv_vsse32_v_i32m1((int32_t *)(dst + 3 * 128 + block * 2),
                               leaf_stride_bytes,
                               __riscv_vreinterpret_v_i16m1_i32m1(x3),
                               vl32);
    }
}

static void rvv_split_radix2_128_to_leaf16_rvv(const int16_t *src, int16_t *dst)
{
    const size_t vl32_in = 4;
    const size_t vl32_pairs = 2;
    const size_t vl32_out = 4;
    const intptr_t leaf_stride_bytes = (intptr_t)(16 * sizeof(int32_t));
    vuint32m1_t even_idx = __riscv_vle32_v_u32m1(split_even_idx32, vl32_pairs);
    vuint32m1_t odd_idx = __riscv_vle32_v_u32m1(split_odd_idx32, vl32_pairs);

    // Each 8-point source block already covers one final leaf slot for the
    // two 64-point children, so emit the child-major / leaf-major layout
    // directly instead of first materializing full 64-point child arrays.
    for (int block = 0; block < 16; ++block) {
        const int16_t *src_block = src + block * 16;
        vint32m1_t lo = __riscv_vle32_v_i32m1((const int32_t *)(src_block + 0), vl32_in);
        vint32m1_t hi = __riscv_vle32_v_i32m1((const int32_t *)(src_block + 8), vl32_in);
        vint32m1_t even = pack_selected_lanes_i32m1(lo, hi, even_idx, vl32_pairs);
        vint32m1_t odd = pack_selected_lanes_i32m1(lo, hi, odd_idx, vl32_pairs);

        __riscv_vsse32_v_i32m1((int32_t *)(dst + 0 * 128 + block * 2),
                               leaf_stride_bytes, even, vl32_out);
        __riscv_vsse32_v_i32m1((int32_t *)(dst + 1 * 128 + block * 2),
                               leaf_stride_bytes, odd, vl32_out);
    }
}

static void rvv_split_radix2_512_to_leaf16_rvv(const int16_t *src, int16_t *dst)
{
    const size_t vl32_pairs = 2;
    const size_t vl32_out = 4;
    const intptr_t leaf_stride_bytes = (intptr_t)(16 * sizeof(int32_t));
    vuint32m1_t even_idx = __riscv_vle32_v_u32m1(split_even_idx32, vl32_pairs);
    vuint32m1_t odd_idx = __riscv_vle32_v_u32m1(split_odd_idx32, vl32_pairs);

    // Each 32-point block covers one final leaf position for both top-level
    // 256-point children. This fuses the old `512 -> 2x256` split and the two
    // child-side `256 -> leaf16` splits into one pass.
    for (int block = 0; block < 16; ++block) {
        const int16_t *src_block = src + block * 64;
        vint16m1_t x0, x1, x2, x3;
        vint16m1_t x4, x5, x6, x7;
        load_transposed4x4_c32_vlse32_i16m1_rvv(src_block +  0, &x0, &x1, &x2, &x3);
        load_transposed4x4_c32_vlse32_i16m1_rvv(src_block + 32, &x4, &x5, &x6, &x7);

        vint32m1_t lo0 = __riscv_vreinterpret_v_i16m1_i32m1(x0);
        vint32m1_t lo1 = __riscv_vreinterpret_v_i16m1_i32m1(x1);
        vint32m1_t lo2 = __riscv_vreinterpret_v_i16m1_i32m1(x2);
        vint32m1_t lo3 = __riscv_vreinterpret_v_i16m1_i32m1(x3);
        vint32m1_t hi0 = __riscv_vreinterpret_v_i16m1_i32m1(x4);
        vint32m1_t hi1 = __riscv_vreinterpret_v_i16m1_i32m1(x5);
        vint32m1_t hi2 = __riscv_vreinterpret_v_i16m1_i32m1(x6);
        vint32m1_t hi3 = __riscv_vreinterpret_v_i16m1_i32m1(x7);

        vint32m1_t even_r0 = pack_selected_lanes_i32m1(lo0, hi0, even_idx, vl32_pairs);
        vint32m1_t even_r1 = pack_selected_lanes_i32m1(lo2, hi2, even_idx, vl32_pairs);
        vint32m1_t even_r2 = pack_selected_lanes_i32m1(lo0, hi0, odd_idx, vl32_pairs);
        vint32m1_t even_r3 = pack_selected_lanes_i32m1(lo2, hi2, odd_idx, vl32_pairs);
        vint32m1_t odd_r0 = pack_selected_lanes_i32m1(lo1, hi1, even_idx, vl32_pairs);
        vint32m1_t odd_r1 = pack_selected_lanes_i32m1(lo3, hi3, even_idx, vl32_pairs);
        vint32m1_t odd_r2 = pack_selected_lanes_i32m1(lo1, hi1, odd_idx, vl32_pairs);
        vint32m1_t odd_r3 = pack_selected_lanes_i32m1(lo3, hi3, odd_idx, vl32_pairs);

        __riscv_vsse32_v_i32m1((int32_t *)(dst + 0 * 512 + 0 * 128 + block * 2),
                               leaf_stride_bytes, even_r0, vl32_out);
        __riscv_vsse32_v_i32m1((int32_t *)(dst + 0 * 512 + 1 * 128 + block * 2),
                               leaf_stride_bytes, even_r1, vl32_out);
        __riscv_vsse32_v_i32m1((int32_t *)(dst + 0 * 512 + 2 * 128 + block * 2),
                               leaf_stride_bytes, even_r2, vl32_out);
        __riscv_vsse32_v_i32m1((int32_t *)(dst + 0 * 512 + 3 * 128 + block * 2),
                               leaf_stride_bytes, even_r3, vl32_out);
        __riscv_vsse32_v_i32m1((int32_t *)(dst + 1 * 512 + 0 * 128 + block * 2),
                               leaf_stride_bytes, odd_r0, vl32_out);
        __riscv_vsse32_v_i32m1((int32_t *)(dst + 1 * 512 + 1 * 128 + block * 2),
                               leaf_stride_bytes, odd_r1, vl32_out);
        __riscv_vsse32_v_i32m1((int32_t *)(dst + 1 * 512 + 2 * 128 + block * 2),
                               leaf_stride_bytes, odd_r2, vl32_out);
        __riscv_vsse32_v_i32m1((int32_t *)(dst + 1 * 512 + 3 * 128 + block * 2),
                               leaf_stride_bytes, odd_r3, vl32_out);
    }
}

static void rvv_split_radix2_2048_to_leaf16_rvv(const int16_t *src, int16_t *dst)
{
    const size_t vl32_in = 4;
    const size_t vl32_pairs = 2;
    const size_t vl32_out = 4;
    const intptr_t child256_stride_bytes = (intptr_t)(256 * sizeof(int32_t));
    vuint32m1_t even_idx = __riscv_vle32_v_u32m1(split_even_idx32, vl32_pairs);
    vuint32m1_t odd_idx = __riscv_vle32_v_u32m1(split_odd_idx32, vl32_pairs);

    for (int block = 0; block < 256; ++block) {
        const int16_t *src_block = src + block * 16;
        const int j = block >> 4;
        const int leaf16 = (block >> 2) & 0x3;
        const int child64 = block & 0x3;
        const int dst_off = child64 * 128 + leaf16 * 32 + j * 2;
        vint32m1_t lo = __riscv_vle32_v_i32m1((const int32_t *)(src_block + 0), vl32_in);
        vint32m1_t hi = __riscv_vle32_v_i32m1((const int32_t *)(src_block + 8), vl32_in);
        vint32m1_t even = pack_selected_lanes_i32m1(lo, hi, even_idx, vl32_pairs);
        vint32m1_t odd = pack_selected_lanes_i32m1(lo, hi, odd_idx, vl32_pairs);

        __riscv_vsse32_v_i32m1((int32_t *)(dst + 0 * 2048 + dst_off),
                               child256_stride_bytes, even, vl32_out);
        __riscv_vsse32_v_i32m1((int32_t *)(dst + 1 * 2048 + dst_off),
                               child256_stride_bytes, odd, vl32_out);
    }
}

static void rvv_split_radix4_1024_to_leaf16_rvv(const int16_t *src, int16_t *dst)
{
    const size_t vl32 = 4;
    const intptr_t child64_stride_bytes = (intptr_t)(64 * sizeof(int32_t));

    // Each 64-point source block already contains one final leaf slot for all
    // four 256-point children and their four 64-point subchildren.
    for (int block = 0; block < 16; ++block) {
        const int16_t *src_block = src + block * 128;

        for (int leaf = 0; leaf < 4; ++leaf) {
            const int16_t *leaf_block = src_block + leaf * 32;
            const int dst_off = leaf * 32 + block * 2;
            vint16m1_t x0, x1, x2, x3;
            load_transposed4x4_c32_vlse32_i16m1_rvv(leaf_block, &x0, &x1, &x2, &x3);

            __riscv_vsse32_v_i32m1((int32_t *)(dst + 0 * 512 + dst_off),
                                   child64_stride_bytes,
                                   __riscv_vreinterpret_v_i16m1_i32m1(x0),
                                   vl32);
            __riscv_vsse32_v_i32m1((int32_t *)(dst + 1 * 512 + dst_off),
                                   child64_stride_bytes,
                                   __riscv_vreinterpret_v_i16m1_i32m1(x1),
                                   vl32);
            __riscv_vsse32_v_i32m1((int32_t *)(dst + 2 * 512 + dst_off),
                                   child64_stride_bytes,
                                   __riscv_vreinterpret_v_i16m1_i32m1(x2),
                                   vl32);
            __riscv_vsse32_v_i32m1((int32_t *)(dst + 3 * 512 + dst_off),
                                   child64_stride_bytes,
                                   __riscv_vreinterpret_v_i16m1_i32m1(x3),
                                   vl32);
        }
    }
}

static void rvv_split_radix4_4096_to_leaf16_rvv(const int16_t *src, int16_t *dst)
{
    const size_t vl32 = 4;
    const intptr_t child256_stride_bytes = (intptr_t)(256 * sizeof(int32_t));

    // For N=4096 the maintained recursion is radix4 -> radix4 -> radix4 -> leaf16.
    // Each contiguous 16-point source block is already one 4x4 tile over the
    // two outer radix4 digits, so transpose once and write it straight into
    // the final child-major / leaf-major 16-point layout.
    for (int block = 0; block < 256; ++block) {
        const int16_t *src_block = src + block * 32;
        const int i = block >> 4;
        const int leaf64 = (block >> 2) & 0x3;
        const int child64 = block & 0x3;
        const int dst_off = child64 * 128 + leaf64 * 32 + i * 2;

        vint16m1_t x0, x1, x2, x3;
        load_transposed4x4_c32_vlse32_i16m1_rvv(src_block, &x0, &x1, &x2, &x3);

        __riscv_vsse32_v_i32m1((int32_t *)(dst + 0 * 2048 + dst_off),
                               child256_stride_bytes,
                               __riscv_vreinterpret_v_i16m1_i32m1(x0),
                               vl32);
        __riscv_vsse32_v_i32m1((int32_t *)(dst + 1 * 2048 + dst_off),
                               child256_stride_bytes,
                               __riscv_vreinterpret_v_i16m1_i32m1(x1),
                               vl32);
        __riscv_vsse32_v_i32m1((int32_t *)(dst + 2 * 2048 + dst_off),
                               child256_stride_bytes,
                               __riscv_vreinterpret_v_i16m1_i32m1(x2),
                               vl32);
        __riscv_vsse32_v_i32m1((int32_t *)(dst + 3 * 2048 + dst_off),
                               child256_stride_bytes,
                               __riscv_vreinterpret_v_i16m1_i32m1(x3),
                               vl32);
    }
}

static inline __attribute__((always_inline)) void dft16_forward_kernel_i16m1(vint16m1_t x0,
                                                                             vint16m1_t x1,
                                                                             vint16m1_t x2,
                                                                             vint16m1_t x3,
#if FFT_EXT_LEAF_TWIDDLE_RESIDENT
                                                                             vint16m1_t leaf_tw0,
                                                                             vint16m1_t leaf_tw1,
                                                                             vint16m1_t leaf_tw2,
#endif
#if !FFT_CUSTOM_ROTATE_ADD_HELPERS
                                                                             vuint16m1_t swap_idx16,
                                                                             vint16m1_t minus_j_sign,
#endif
                                                                             vint16m1_t *y0,
                                                                             vint16m1_t *y1,
                                                                             vint16m1_t *y2,
                                                                             vint16m1_t *y3)
{
    const size_t vl8 = 8;
#if FFT_EXT_INDIVIDUAL_ROTATE_DFT16
    vint16m1_t rotate_zero = __riscv_vxor_vv_i16m1(x0, x0, vl8);
#endif
    vint16m1_t x02t = __riscv_vsadd_vv_i16m1(x0, x2, vl8);
    vint16m1_t x13t = __riscv_vsadd_vv_i16m1(x1, x3, vl8);
    vint16m1_t t0 = __riscv_vsadd_vv_i16m1(x02t, x13t, vl8);
    vint16m1_t t2 = __riscv_vssub_vv_i16m1(x02t, x13t, vl8);
    x02t = __riscv_vssub_vv_i16m1(x0, x2, vl8);
    vint16m1_t t1;
    vint16m1_t t3;
#if FFT_EXT_INDIVIDUAL_ROTATE_DFT16
    vint16m1_t x1f;
    vint16m1_t x3f;
#if FFT_EXT_ROTATE_BOUNDARY_FALLBACK
    ext_rotate_minus_j_pair_exact_i16m1(x1, x3, rotate_zero,
                                        swap_idx16, minus_j_sign, vl8,
                                        &x1f, &x3f);
#else
    ext_rotate_minus_j_pair_i16m1(x1, x3, rotate_zero, &x1f, &x3f);
#endif
    x13t = __riscv_vssub_vv_i16m1(x1f, x3f, vl8);
    t1 = __riscv_vsadd_vv_i16m1(x02t, x13t, vl8);
    t3 = __riscv_vssub_vv_i16m1(x02t, x13t, vl8);
#elif FFT_CUSTOM_ROTATE_ADD_DFT16
    radix4_forward_rot_pair_i16m1(x02t, x1, x3, vl8, &t1, &t3);
#else
    vint16m1_t x1f = complex_mul_minus_j_with_idx_sign_i16m1(x1, swap_idx16, minus_j_sign, vl8);
    vint16m1_t x3f = complex_mul_minus_j_with_idx_sign_i16m1(x3, swap_idx16, minus_j_sign, vl8);
    x13t = __riscv_vssub_vv_i16m1(x1f, x3f, vl8);
    t1 = __riscv_vsadd_vv_i16m1(x02t, x13t, vl8);
    t3 = __riscv_vssub_vv_i16m1(x02t, x13t, vl8);
#endif

    transpose4x4_c32_shuffle_rvv(&t0, &t1, &t2, &t3);

    // dft16 switches to e32 for the in-register transpose, so reopen the
    // e16 region once here and let the remaining ext ops share it.
    FFT_EXT_SET_E16M1_VL8();

#ifdef FFT_CUSTOM_PACKED_CMUL_HELPERS
#if FFT_EXT_LEAF_TWIDDLE_RESIDENT
    ext_packed_cmul3_leaf_resident_i16m1(&t1, &t2, &t3,
                                         leaf_tw0, leaf_tw1, leaf_tw2);
#else
    vint16m1_t tw0 = __riscv_vle16_v_i16m1(ext16_tw_fwd[0], vl8);
    vint16m1_t tw1 = __riscv_vle16_v_i16m1(ext16_tw_fwd[1], vl8);
    vint16m1_t tw2 = __riscv_vle16_v_i16m1(ext16_tw_fwd[2], vl8);
    packed_cmul3_q15_i16m1(&t1, &t2, &t3,
                           tw0, tw0,
                           tw1, tw1,
                           tw2, tw2,
                           swap_idx16, vl8);
#endif
#else
    vint16m1_t tw_re0 = __riscv_vle16_v_i16m1(tw16_re +  0, vl8);
    vint16m1_t tw_im0 = __riscv_vle16_v_i16m1(tw16_im +  0, vl8);
    vint16m1_t tw_re1 = __riscv_vle16_v_i16m1(tw16_re +  8, vl8);
    vint16m1_t tw_im1 = __riscv_vle16_v_i16m1(tw16_im +  8, vl8);
    vint16m1_t tw_re2 = __riscv_vle16_v_i16m1(tw16_re + 16, vl8);
    vint16m1_t tw_im2 = __riscv_vle16_v_i16m1(tw16_im + 16, vl8);
    packed_cmul3_q15_i16m1(&t1, &t2, &t3,
                           tw_re0, tw_im0,
                           tw_re1, tw_im1,
                           tw_re2, tw_im2,
                           swap_idx16, vl8);
#endif

    x02t = __riscv_vsadd_vv_i16m1(t0, t2, vl8);
    x13t = __riscv_vsadd_vv_i16m1(t1, t3, vl8);
    *y0 = __riscv_vsadd_vv_i16m1(x02t, x13t, vl8);
    *y2 = __riscv_vssub_vv_i16m1(x02t, x13t, vl8);
    x02t = __riscv_vssub_vv_i16m1(t0, t2, vl8);
#if FFT_EXT_INDIVIDUAL_ROTATE_DFT16
    vint16m1_t y1f;
    vint16m1_t y3f;
#if FFT_EXT_ROTATE_BOUNDARY_FALLBACK
    ext_rotate_minus_j_pair_exact_i16m1(t1, t3, rotate_zero,
                                        swap_idx16, minus_j_sign, vl8,
                                        &y1f, &y3f);
#else
    ext_rotate_minus_j_pair_i16m1(t1, t3, rotate_zero, &y1f, &y3f);
#endif
    x13t = __riscv_vssub_vv_i16m1(y1f, y3f, vl8);
    *y1 = __riscv_vsadd_vv_i16m1(x02t, x13t, vl8);
    *y3 = __riscv_vssub_vv_i16m1(x02t, x13t, vl8);
#elif FFT_CUSTOM_ROTATE_ADD_DFT16
    radix4_forward_rot_pair_i16m1(x02t, t1, t3, vl8, y1, y3);
#if FFT_VALIDATE && 0
    {
        int16_t dbg_ref_y1[8] __attribute__((aligned(16)));
        int16_t dbg_ref_y3[8] __attribute__((aligned(16)));
        int16_t dbg_dut_y1[8] __attribute__((aligned(16)));
        int16_t dbg_dut_y3[8] __attribute__((aligned(16)));
        vint16m1_t dbg_y1f =
            complex_mul_minus_j_with_idx_sign_i16m1(t1, dbg_swap_idx16, dbg_minus_j_sign, vl8);
        vint16m1_t dbg_y3f =
            complex_mul_minus_j_with_idx_sign_i16m1(t3, dbg_swap_idx16, dbg_minus_j_sign, vl8);
        vint16m1_t dbg_x13t = __riscv_vssub_vv_i16m1(dbg_y1f, dbg_y3f, vl8);
        vint16m1_t dbg_y1 = __riscv_vsadd_vv_i16m1(x02t, dbg_x13t, vl8);
        vint16m1_t dbg_y3 = __riscv_vssub_vv_i16m1(x02t, dbg_x13t, vl8);
        __riscv_vse16_v_i16m1(dbg_ref_y1, dbg_y1, vl8);
        __riscv_vse16_v_i16m1(dbg_ref_y3, dbg_y3, vl8);
        __riscv_vse16_v_i16m1(dbg_dut_y1, *y1, vl8);
        __riscv_vse16_v_i16m1(dbg_dut_y3, *y3, vl8);
        if (!fft_compare_complex_vector(dbg_ref_y1, dbg_dut_y1, 4) ||
            !fft_compare_complex_vector(dbg_ref_y3, dbg_dut_y3, 4)) {
            printf("%s dft16 stage2 rotpair mismatch\n", FFT_IMPL_DISPLAY_NAME);
            fft_print_complex_vector("stage2 ref y1", dbg_ref_y1, 4);
            fft_print_complex_vector("stage2 dut y1", dbg_dut_y1, 4);
            fft_print_complex_vector("stage2 ref y3", dbg_ref_y3, 4);
            fft_print_complex_vector("stage2 dut y3", dbg_dut_y3, 4);
        }
    }
#endif
#else
    vint16m1_t y1f = complex_mul_minus_j_with_idx_sign_i16m1(t1, swap_idx16, minus_j_sign, vl8);
    vint16m1_t y3f = complex_mul_minus_j_with_idx_sign_i16m1(t3, swap_idx16, minus_j_sign, vl8);
    x13t = __riscv_vssub_vv_i16m1(y1f, y3f, vl8);
    *y1 = __riscv_vsadd_vv_i16m1(x02t, x13t, vl8);
    *y3 = __riscv_vssub_vv_i16m1(x02t, x13t, vl8);
#endif
}

static inline __attribute__((always_inline)) void dft16_inverse_kernel_i16m1(vint16m1_t x0,
                                                                             vint16m1_t x1,
                                                                             vint16m1_t x2,
                                                                             vint16m1_t x3,
#if FFT_EXT_LEAF_TWIDDLE_RESIDENT
                                                                             vint16m1_t leaf_tw0,
                                                                             vint16m1_t leaf_tw1,
                                                                             vint16m1_t leaf_tw2,
#endif
#if !FFT_CUSTOM_ROTATE_ADD_HELPERS
                                                                             vuint16m1_t swap_idx16,
                                                                             vint16m1_t minus_j_sign,
#endif
                                                                             vint16m1_t *y0,
                                                                             vint16m1_t *y1,
                                                                             vint16m1_t *y2,
                                                                             vint16m1_t *y3)
{
    const size_t vl8 = 8;
#if FFT_EXT_INDIVIDUAL_ROTATE_DFT16
    vint16m1_t rotate_zero = __riscv_vxor_vv_i16m1(x0, x0, vl8);
#endif
    vint16m1_t x02t = __riscv_vsadd_vv_i16m1(x0, x2, vl8);
    vint16m1_t x13t = __riscv_vsadd_vv_i16m1(x1, x3, vl8);
    vint16m1_t t0 = __riscv_vsadd_vv_i16m1(x02t, x13t, vl8);
    vint16m1_t t2 = __riscv_vssub_vv_i16m1(x02t, x13t, vl8);
    x02t = __riscv_vssub_vv_i16m1(x0, x2, vl8);
    vint16m1_t t1;
    vint16m1_t t3;
#if FFT_EXT_INDIVIDUAL_ROTATE_DFT16
    vint16m1_t x1f;
    vint16m1_t x3f;
#if FFT_EXT_ROTATE_BOUNDARY_FALLBACK
    ext_rotate_minus_j_pair_exact_i16m1(x1, x3, rotate_zero,
                                        swap_idx16, minus_j_sign, vl8,
                                        &x1f, &x3f);
#else
    ext_rotate_minus_j_pair_i16m1(x1, x3, rotate_zero, &x1f, &x3f);
#endif
    x13t = __riscv_vssub_vv_i16m1(x1f, x3f, vl8);
    t3 = __riscv_vsadd_vv_i16m1(x02t, x13t, vl8);
    t1 = __riscv_vssub_vv_i16m1(x02t, x13t, vl8);
#elif FFT_CUSTOM_ROTATE_ADD_DFT16
    radix4_inverse_rot_pair_i16m1(x02t, x1, x3, vl8, &t1, &t3);
#else
    vint16m1_t x1f = complex_mul_minus_j_with_idx_sign_i16m1(x1, swap_idx16, minus_j_sign, vl8);
    vint16m1_t x3f = complex_mul_minus_j_with_idx_sign_i16m1(x3, swap_idx16, minus_j_sign, vl8);
    x13t = __riscv_vssub_vv_i16m1(x1f, x3f, vl8);
    t3 = __riscv_vsadd_vv_i16m1(x02t, x13t, vl8);
    t1 = __riscv_vssub_vv_i16m1(x02t, x13t, vl8);
#endif

    transpose4x4_c32_shuffle_rvv(&t0, &t1, &t2, &t3);

    // dft16 switches to e32 for the in-register transpose, so reopen the
    // e16 region once here and let the remaining ext ops share it.
    FFT_EXT_SET_E16M1_VL8();

#ifdef FFT_CUSTOM_PACKED_CMUL_HELPERS
#if FFT_EXT_LEAF_TWIDDLE_RESIDENT
    ext_packed_cmul3_leaf_resident_i16m1(&t1, &t2, &t3,
                                         leaf_tw0, leaf_tw1, leaf_tw2);
#else
    vint16m1_t tw0 = __riscv_vle16_v_i16m1(ext16_tw_inv[0], vl8);
    vint16m1_t tw1 = __riscv_vle16_v_i16m1(ext16_tw_inv[1], vl8);
    vint16m1_t tw2 = __riscv_vle16_v_i16m1(ext16_tw_inv[2], vl8);
    packed_cmul3_q15_i16m1(&t1, &t2, &t3,
                           tw0, tw0,
                           tw1, tw1,
                           tw2, tw2,
                           swap_idx16, vl8);
#endif
#else
    vint16m1_t tw_re0 = __riscv_vle16_v_i16m1(tw16_re +  0, vl8);
    vint16m1_t tw_im0 = __riscv_vle16_v_i16m1(tw16_im_idft +  0, vl8);
    vint16m1_t tw_re1 = __riscv_vle16_v_i16m1(tw16_re +  8, vl8);
    vint16m1_t tw_im1 = __riscv_vle16_v_i16m1(tw16_im_idft +  8, vl8);
    vint16m1_t tw_re2 = __riscv_vle16_v_i16m1(tw16_re + 16, vl8);
    vint16m1_t tw_im2 = __riscv_vle16_v_i16m1(tw16_im_idft + 16, vl8);
    packed_cmul3_q15_i16m1(&t1, &t2, &t3,
                           tw_re0, tw_im0,
                           tw_re1, tw_im1,
                           tw_re2, tw_im2,
                           swap_idx16, vl8);
#endif

    x02t = __riscv_vsadd_vv_i16m1(t0, t2, vl8);
    x13t = __riscv_vsadd_vv_i16m1(t1, t3, vl8);
    *y0 = __riscv_vsadd_vv_i16m1(x02t, x13t, vl8);
    *y2 = __riscv_vssub_vv_i16m1(x02t, x13t, vl8);
    x02t = __riscv_vssub_vv_i16m1(t0, t2, vl8);
#if FFT_EXT_INDIVIDUAL_ROTATE_DFT16
    vint16m1_t y1f;
    vint16m1_t y3f;
#if FFT_EXT_ROTATE_BOUNDARY_FALLBACK
    ext_rotate_minus_j_pair_exact_i16m1(t1, t3, rotate_zero,
                                        swap_idx16, minus_j_sign, vl8,
                                        &y1f, &y3f);
#else
    ext_rotate_minus_j_pair_i16m1(t1, t3, rotate_zero, &y1f, &y3f);
#endif
    x13t = __riscv_vssub_vv_i16m1(y1f, y3f, vl8);
    *y3 = __riscv_vsadd_vv_i16m1(x02t, x13t, vl8);
    *y1 = __riscv_vssub_vv_i16m1(x02t, x13t, vl8);
#elif FFT_CUSTOM_ROTATE_ADD_DFT16
    radix4_inverse_rot_pair_i16m1(x02t, t1, t3, vl8, y1, y3);
#else
    vint16m1_t y1f = complex_mul_minus_j_with_idx_sign_i16m1(t1, swap_idx16, minus_j_sign, vl8);
    vint16m1_t y3f = complex_mul_minus_j_with_idx_sign_i16m1(t3, swap_idx16, minus_j_sign, vl8);
    x13t = __riscv_vssub_vv_i16m1(y1f, y3f, vl8);
    *y3 = __riscv_vsadd_vv_i16m1(x02t, x13t, vl8);
    *y1 = __riscv_vssub_vv_i16m1(x02t, x13t, vl8);
#endif
}

static inline __attribute__((always_inline)) void dft16_forward_leaf_i16m1(int16_t *x,
                                                                           int16_t *y,
                                                                           int need_entry_e16)
{
    const size_t vl8 = 8;
#if !FFT_CUSTOM_ROTATE_ADD_HELPERS
    vuint16m1_t swap_idx16 = __riscv_vle16_v_u16m1(swap_re_im_idx16, vl8);
    vint16m1_t minus_j_sign = __riscv_vle16_v_i16m1(minus_j_sign16, vl8);
#endif

    vint16m1_t x0 = __riscv_vle16_v_i16m1(x +  0, vl8);
    vint16m1_t x1 = __riscv_vle16_v_i16m1(x +  8, vl8);
    vint16m1_t x2 = __riscv_vle16_v_i16m1(x + 16, vl8);
    vint16m1_t x3 = __riscv_vle16_v_i16m1(x + 24, vl8);

    vint16m1_t y0;
    vint16m1_t y1;
    vint16m1_t y2;
    vint16m1_t y3;

#if FFT_EXT_LEAF_TWIDDLE_RESIDENT
    vint16m1_t leaf_tw0 = __riscv_vle16_v_i16m1(ext16_tw_fwd[0], vl8);
    vint16m1_t leaf_tw1 = __riscv_vle16_v_i16m1(ext16_tw_fwd[1], vl8);
    vint16m1_t leaf_tw2 = __riscv_vle16_v_i16m1(ext16_tw_fwd[2], vl8);
#endif

#if FFT_CUSTOM_ROTATE_ADD_DFT16 || FFT_EXT_INDIVIDUAL_ROTATE_DFT16
    if (need_entry_e16) {
        FFT_EXT_SET_E16M1_VL8();
    }
#else
    (void)need_entry_e16;
#endif
    dft16_forward_kernel_i16m1(x0, x1, x2, x3,
#if FFT_EXT_LEAF_TWIDDLE_RESIDENT
                               leaf_tw0, leaf_tw1, leaf_tw2,
#endif
#if !FFT_CUSTOM_ROTATE_ADD_HELPERS
                               swap_idx16, minus_j_sign,
#endif
                               &y0, &y1, &y2, &y3);

    __riscv_vse16_v_i16m1(y +  0, y0, vl8);
    __riscv_vse16_v_i16m1(y +  8, y1, vl8);
    __riscv_vse16_v_i16m1(y + 16, y2, vl8);
    __riscv_vse16_v_i16m1(y + 24, y3, vl8);
}

static void __attribute__((noinline)) dft16_rvv(int16_t *x,int16_t *y)
{
    dft16_forward_leaf_i16m1(x, y, 1);
}

static inline __attribute__((always_inline)) void dft16_inverse_leaf_i16m1(int16_t *x,
                                                                           int16_t *y,
                                                                           int need_entry_e16)
{
    const size_t vl8 = 8;
#if !FFT_CUSTOM_ROTATE_ADD_HELPERS
    vuint16m1_t swap_idx16 = __riscv_vle16_v_u16m1(swap_re_im_idx16, vl8);
    vint16m1_t minus_j_sign = __riscv_vle16_v_i16m1(minus_j_sign16, vl8);
#endif

    vint16m1_t x0 = __riscv_vle16_v_i16m1(x +  0, vl8);
    vint16m1_t x1 = __riscv_vle16_v_i16m1(x +  8, vl8);
    vint16m1_t x2 = __riscv_vle16_v_i16m1(x + 16, vl8);
    vint16m1_t x3 = __riscv_vle16_v_i16m1(x + 24, vl8);

    vint16m1_t y0;
    vint16m1_t y1;
    vint16m1_t y2;
    vint16m1_t y3;

#if FFT_EXT_LEAF_TWIDDLE_RESIDENT
    vint16m1_t leaf_tw0 = __riscv_vle16_v_i16m1(ext16_tw_inv[0], vl8);
    vint16m1_t leaf_tw1 = __riscv_vle16_v_i16m1(ext16_tw_inv[1], vl8);
    vint16m1_t leaf_tw2 = __riscv_vle16_v_i16m1(ext16_tw_inv[2], vl8);
#endif

#if FFT_CUSTOM_ROTATE_ADD_DFT16 || FFT_EXT_INDIVIDUAL_ROTATE_DFT16
    if (need_entry_e16) {
        FFT_EXT_SET_E16M1_VL8();
    }
#else
    (void)need_entry_e16;
#endif
    dft16_inverse_kernel_i16m1(x0, x1, x2, x3,
#if FFT_EXT_LEAF_TWIDDLE_RESIDENT
                               leaf_tw0, leaf_tw1, leaf_tw2,
#endif
#if !FFT_CUSTOM_ROTATE_ADD_HELPERS
                               swap_idx16, minus_j_sign,
#endif
                               &y0, &y1, &y2, &y3);

    __riscv_vse16_v_i16m1(y +  0, y0, vl8);
    __riscv_vse16_v_i16m1(y +  8, y1, vl8);
    __riscv_vse16_v_i16m1(y + 16, y2, vl8);
    __riscv_vse16_v_i16m1(y + 24, y3, vl8);
}

static void __attribute__((noinline)) idft16_rvv(int16_t *x, int16_t *y)
{
    dft16_inverse_leaf_i16m1(x, y, 1);
}

static inline vint16m1_t rvv_shift_right_i16m1(vint16m1_t v, unsigned shift, size_t vl)
{
    return shift ? __riscv_vsra_vx_i16m1(v, shift, vl) : v;
}

static inline void rvv_combine_radix4_forward_64_block_i16m1(vint16m1_t x0,
                                                             vint16m1_t x1,
                                                             vint16m1_t x2,
                                                             vint16m1_t x3,
                                                             int off,
                                                             unsigned shift,
#if !FFT_CUSTOM_ROTATE_ADD_HELPERS
                                                             vuint16m1_t swap_idx16,
                                                             vint16m1_t minus_j_sign,
#endif
                                                             vint16m1_t *y0,
                                                             vint16m1_t *y1,
                                                             vint16m1_t *y2,
                                                             vint16m1_t *y3)
{
    const size_t vl8 = 8;
#if FFT_EXT_INDIVIDUAL_ROTATE_COMBINE64
    vint16m1_t rotate_zero = __riscv_vxor_vv_i16m1(x0, x0, vl8);
#endif
#ifdef FFT_CUSTOM_PACKED_CMUL_HELPERS
    vint16m1_t tw_re0 = __riscv_vle16_v_i16m1(ext64_tw_fwd[0] + off, vl8);
    vint16m1_t tw_re1 = __riscv_vle16_v_i16m1(ext64_tw_fwd[1] + off, vl8);
    vint16m1_t tw_re2 = __riscv_vle16_v_i16m1(ext64_tw_fwd[2] + off, vl8);
#else
    vint16m1_t tw_re0 = __riscv_vle16_v_i16m1(g_rvv64_tw_re_dup[0] + off, vl8);
    vint16m1_t tw_im0 = __riscv_vle16_v_i16m1(g_rvv64_tw_im_fwd[0] + off, vl8);
    vint16m1_t tw_re1 = __riscv_vle16_v_i16m1(g_rvv64_tw_re_dup[1] + off, vl8);
    vint16m1_t tw_im1 = __riscv_vle16_v_i16m1(g_rvv64_tw_im_fwd[1] + off, vl8);
    vint16m1_t tw_re2 = __riscv_vle16_v_i16m1(g_rvv64_tw_re_dup[2] + off, vl8);
    vint16m1_t tw_im2 = __riscv_vle16_v_i16m1(g_rvv64_tw_im_fwd[2] + off, vl8);
#endif

    packed_cmul3_q15_i16m1(&x1, &x2, &x3,
                           tw_re0, tw_im0,
                           tw_re1, tw_im1,
                           tw_re2, tw_im2,
                           swap_idx16, vl8);

    vint16m1_t x02 = __riscv_vsadd_vv_i16m1(x0, x2, vl8);
    vint16m1_t x13 = __riscv_vsadd_vv_i16m1(x1, x3, vl8);
    *y0 = rvv_shift_right_i16m1(__riscv_vsadd_vv_i16m1(x02, x13, vl8), shift, vl8);
    *y2 = rvv_shift_right_i16m1(__riscv_vssub_vv_i16m1(x02, x13, vl8), shift, vl8);
    vint16m1_t x02d = __riscv_vssub_vv_i16m1(x0, x2, vl8);
#if FFT_EXT_INDIVIDUAL_ROTATE_COMBINE64
    vint16m1_t x1mj;
    vint16m1_t x3mj;
#if FFT_EXT_ROTATE_BOUNDARY_FALLBACK
    ext_rotate_minus_j_pair_exact_i16m1(x1, x3, rotate_zero,
                                        swap_idx16, minus_j_sign, vl8,
                                        &x1mj, &x3mj);
#else
    ext_rotate_minus_j_pair_i16m1(x1, x3, rotate_zero, &x1mj, &x3mj);
#endif
    vint16m1_t x13d = __riscv_vssub_vv_i16m1(x1mj, x3mj, vl8);
    *y1 = rvv_shift_right_i16m1(__riscv_vsadd_vv_i16m1(x02d, x13d, vl8), shift, vl8);
    *y3 = rvv_shift_right_i16m1(__riscv_vssub_vv_i16m1(x02d, x13d, vl8), shift, vl8);
#elif FFT_CUSTOM_ROTATE_ADD_COMBINE64
    vint16m1_t y1_raw;
    vint16m1_t y3_raw;
    radix4_forward_rot_pair_i16m1(x02d, x1, x3, vl8, &y1_raw, &y3_raw);
    *y1 = rvv_shift_right_i16m1(y1_raw, shift, vl8);
    *y3 = rvv_shift_right_i16m1(y3_raw, shift, vl8);
#else
    vint16m1_t x1mj =
        complex_mul_minus_j_with_idx_sign_i16m1(x1, swap_idx16, minus_j_sign, vl8);
    vint16m1_t x3mj =
        complex_mul_minus_j_with_idx_sign_i16m1(x3, swap_idx16, minus_j_sign, vl8);
    vint16m1_t x13d = __riscv_vssub_vv_i16m1(x1mj, x3mj, vl8);
    *y1 = rvv_shift_right_i16m1(__riscv_vsadd_vv_i16m1(x02d, x13d, vl8), shift, vl8);
    *y3 = rvv_shift_right_i16m1(__riscv_vssub_vv_i16m1(x02d, x13d, vl8), shift, vl8);
#endif
}

static inline void rvv_combine_radix4_inverse_64_block_i16m1(vint16m1_t x0,
                                                             vint16m1_t x1,
                                                             vint16m1_t x2,
                                                             vint16m1_t x3,
                                                             int off,
                                                             unsigned shift,
#if !FFT_CUSTOM_ROTATE_ADD_HELPERS
                                                             vuint16m1_t swap_idx16,
                                                             vint16m1_t minus_j_sign,
#endif
                                                             vint16m1_t *y0,
                                                             vint16m1_t *y1,
                                                             vint16m1_t *y2,
                                                             vint16m1_t *y3)
{
    const size_t vl8 = 8;
#if FFT_EXT_INDIVIDUAL_ROTATE_COMBINE64
    vint16m1_t rotate_zero = __riscv_vxor_vv_i16m1(x0, x0, vl8);
#endif
#ifdef FFT_CUSTOM_PACKED_CMUL_HELPERS
    vint16m1_t tw_re0 = __riscv_vle16_v_i16m1(ext64_tw_inv[0] + off, vl8);
    vint16m1_t tw_re1 = __riscv_vle16_v_i16m1(ext64_tw_inv[1] + off, vl8);
    vint16m1_t tw_re2 = __riscv_vle16_v_i16m1(ext64_tw_inv[2] + off, vl8);
#else
    vint16m1_t tw_re0 = __riscv_vle16_v_i16m1(g_rvv64_tw_re_dup[0] + off, vl8);
    vint16m1_t tw_im0 = __riscv_vle16_v_i16m1(g_rvv64_tw_im_inv[0] + off, vl8);
    vint16m1_t tw_re1 = __riscv_vle16_v_i16m1(g_rvv64_tw_re_dup[1] + off, vl8);
    vint16m1_t tw_im1 = __riscv_vle16_v_i16m1(g_rvv64_tw_im_inv[1] + off, vl8);
    vint16m1_t tw_re2 = __riscv_vle16_v_i16m1(g_rvv64_tw_re_dup[2] + off, vl8);
    vint16m1_t tw_im2 = __riscv_vle16_v_i16m1(g_rvv64_tw_im_inv[2] + off, vl8);
#endif

    packed_cmul3_q15_i16m1(&x1, &x2, &x3,
                           tw_re0, tw_im0,
                           tw_re1, tw_im1,
                           tw_re2, tw_im2,
                           swap_idx16, vl8);

    vint16m1_t x02 = __riscv_vsadd_vv_i16m1(x0, x2, vl8);
    vint16m1_t x13 = __riscv_vsadd_vv_i16m1(x1, x3, vl8);
    *y0 = rvv_shift_right_i16m1(__riscv_vsadd_vv_i16m1(x02, x13, vl8), shift, vl8);
    *y2 = rvv_shift_right_i16m1(__riscv_vssub_vv_i16m1(x02, x13, vl8), shift, vl8);
    vint16m1_t x02d = __riscv_vssub_vv_i16m1(x0, x2, vl8);
#if FFT_EXT_INDIVIDUAL_ROTATE_COMBINE64
    vint16m1_t x1mj;
    vint16m1_t x3mj;
#if FFT_EXT_ROTATE_BOUNDARY_FALLBACK
    ext_rotate_minus_j_pair_exact_i16m1(x1, x3, rotate_zero,
                                        swap_idx16, minus_j_sign, vl8,
                                        &x1mj, &x3mj);
#else
    ext_rotate_minus_j_pair_i16m1(x1, x3, rotate_zero, &x1mj, &x3mj);
#endif
    vint16m1_t x13d = __riscv_vssub_vv_i16m1(x1mj, x3mj, vl8);
    *y3 = rvv_shift_right_i16m1(__riscv_vsadd_vv_i16m1(x02d, x13d, vl8), shift, vl8);
    *y1 = rvv_shift_right_i16m1(__riscv_vssub_vv_i16m1(x02d, x13d, vl8), shift, vl8);
#elif FFT_CUSTOM_ROTATE_ADD_COMBINE64
    vint16m1_t y1_raw;
    vint16m1_t y3_raw;
    radix4_inverse_rot_pair_i16m1(x02d, x1, x3, vl8, &y1_raw, &y3_raw);
    *y1 = rvv_shift_right_i16m1(y1_raw, shift, vl8);
    *y3 = rvv_shift_right_i16m1(y3_raw, shift, vl8);
#else
    vint16m1_t x1mj =
        complex_mul_minus_j_with_idx_sign_i16m1(x1, swap_idx16, minus_j_sign, vl8);
    vint16m1_t x3mj =
        complex_mul_minus_j_with_idx_sign_i16m1(x3, swap_idx16, minus_j_sign, vl8);
    vint16m1_t x13d = __riscv_vssub_vv_i16m1(x1mj, x3mj, vl8);
    *y3 = rvv_shift_right_i16m1(__riscv_vsadd_vv_i16m1(x02d, x13d, vl8), shift, vl8);
    *y1 = rvv_shift_right_i16m1(__riscv_vssub_vv_i16m1(x02d, x13d, vl8), shift, vl8);
#endif
}

static void rvv_combine_radix4_forward_64_rvv(const int16_t *src, int16_t *dst, unsigned shift)
{
    const size_t vl8 = 8;
#if !FFT_CUSTOM_ROTATE_ADD_HELPERS
    vuint16m1_t swap_idx16 = __riscv_vle16_v_u16m1(swap_re_im_idx16, vl8);
    vint16m1_t minus_j_sign = __riscv_vle16_v_i16m1(minus_j_sign16, vl8);
#endif
    for (int block = 0; block < 4; ++block) {
        const int off = block * 8;
        vint16m1_t x0 = __riscv_vle16_v_i16m1(src + 0 * 32 + off, vl8);
        vint16m1_t x1 = __riscv_vle16_v_i16m1(src + 1 * 32 + off, vl8);
        vint16m1_t x2 = __riscv_vle16_v_i16m1(src + 2 * 32 + off, vl8);
        vint16m1_t x3 = __riscv_vle16_v_i16m1(src + 3 * 32 + off, vl8);
        vint16m1_t y0;
        vint16m1_t y1;
        vint16m1_t y2;
        vint16m1_t y3;

        rvv_combine_radix4_forward_64_block_i16m1(x0, x1, x2, x3, off, shift,
#if !FFT_CUSTOM_ROTATE_ADD_HELPERS
                                                  swap_idx16, minus_j_sign,
#endif
                                                  &y0, &y1, &y2, &y3);

        __riscv_vse16_v_i16m1(dst + 0 * 32 + off, y0, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 32 + off, y1, vl8);
        __riscv_vse16_v_i16m1(dst + 2 * 32 + off, y2, vl8);
        __riscv_vse16_v_i16m1(dst + 3 * 32 + off, y3, vl8);
    }
}

static void rvv_combine_radix4_inverse_64_rvv(const int16_t *src, int16_t *dst, unsigned shift)
{
    const size_t vl8 = 8;
#if !FFT_CUSTOM_ROTATE_ADD_HELPERS
    vuint16m1_t swap_idx16 = __riscv_vle16_v_u16m1(swap_re_im_idx16, vl8);
    vint16m1_t minus_j_sign = __riscv_vle16_v_i16m1(minus_j_sign16, vl8);
#endif
    for (int block = 0; block < 4; ++block) {
        const int off = block * 8;
        vint16m1_t x0 = __riscv_vle16_v_i16m1(src + 0 * 32 + off, vl8);
        vint16m1_t x1 = __riscv_vle16_v_i16m1(src + 1 * 32 + off, vl8);
        vint16m1_t x2 = __riscv_vle16_v_i16m1(src + 2 * 32 + off, vl8);
        vint16m1_t x3 = __riscv_vle16_v_i16m1(src + 3 * 32 + off, vl8);
        vint16m1_t y0;
        vint16m1_t y1;
        vint16m1_t y2;
        vint16m1_t y3;

        rvv_combine_radix4_inverse_64_block_i16m1(x0, x1, x2, x3, off, shift,
#if !FFT_CUSTOM_ROTATE_ADD_HELPERS
                                                  swap_idx16, minus_j_sign,
#endif
                                                  &y0, &y1, &y2, &y3);

        __riscv_vse16_v_i16m1(dst + 0 * 32 + off, y0, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 32 + off, y1, vl8);
        __riscv_vse16_v_i16m1(dst + 2 * 32 + off, y2, vl8);
        __riscv_vse16_v_i16m1(dst + 3 * 32 + off, y3, vl8);
    }
}

static void rvv_combine_radix4_forward_256_rvv(const int16_t *src, int16_t *dst, unsigned shift)
{
    const size_t vl8 = 8;
#if !FFT_CUSTOM_ROTATE_ADD_HELPERS
    vuint16m1_t swap_idx16 = __riscv_vle16_v_u16m1(swap_re_im_idx16, vl8);
    vint16m1_t minus_j_sign = __riscv_vle16_v_i16m1(minus_j_sign16, vl8);
#endif
    for (int block = 0; block < 16; ++block) {
        const int off = block * 8;
        vint16m1_t x0 = __riscv_vle16_v_i16m1(src + 0 * 128 + off, vl8);
        vint16m1_t x1 = __riscv_vle16_v_i16m1(src + 1 * 128 + off, vl8);
        vint16m1_t x2 = __riscv_vle16_v_i16m1(src + 2 * 128 + off, vl8);
        vint16m1_t x3 = __riscv_vle16_v_i16m1(src + 3 * 128 + off, vl8);

#ifdef FFT_CUSTOM_PACKED_CMUL_HELPERS
        vint16m1_t tw_re0 = __riscv_vle16_v_i16m1(ext256_tw_fwd[0] + off, vl8);
        vint16m1_t tw_re1 = __riscv_vle16_v_i16m1(ext256_tw_fwd[1] + off, vl8);
        vint16m1_t tw_re2 = __riscv_vle16_v_i16m1(ext256_tw_fwd[2] + off, vl8);
#else
        vint16m1_t tw_re0 = __riscv_vle16_v_i16m1(rvv256_tw_re_dup[0] + off, vl8);
        vint16m1_t tw_im0 = __riscv_vle16_v_i16m1(rvv256_tw_im_fwd[0] + off, vl8);
        vint16m1_t tw_re1 = __riscv_vle16_v_i16m1(rvv256_tw_re_dup[1] + off, vl8);
        vint16m1_t tw_im1 = __riscv_vle16_v_i16m1(rvv256_tw_im_fwd[1] + off, vl8);
        vint16m1_t tw_re2 = __riscv_vle16_v_i16m1(rvv256_tw_re_dup[2] + off, vl8);
        vint16m1_t tw_im2 = __riscv_vle16_v_i16m1(rvv256_tw_im_fwd[2] + off, vl8);
#endif

        packed_cmul3_q15_i16m1(&x1, &x2, &x3,
                               tw_re0, tw_im0,
                               tw_re1, tw_im1,
                               tw_re2, tw_im2,
                               swap_idx16, vl8);

        vint16m1_t x02 = __riscv_vsadd_vv_i16m1(x0, x2, vl8);
        vint16m1_t x13 = __riscv_vsadd_vv_i16m1(x1, x3, vl8);
        vint16m1_t y0 = rvv_shift_right_i16m1(__riscv_vsadd_vv_i16m1(x02, x13, vl8), shift, vl8);
        vint16m1_t y2 = rvv_shift_right_i16m1(__riscv_vssub_vv_i16m1(x02, x13, vl8), shift, vl8);
        vint16m1_t x02d = __riscv_vssub_vv_i16m1(x0, x2, vl8);
#if FFT_CUSTOM_ROTATE_ADD_COMBINE256
        vint16m1_t y1_raw;
        vint16m1_t y3_raw;
        radix4_forward_rot_pair_i16m1(x02d, x1, x3, vl8, &y1_raw, &y3_raw);
        vint16m1_t y1 = rvv_shift_right_i16m1(y1_raw, shift, vl8);
        vint16m1_t y3 = rvv_shift_right_i16m1(y3_raw, shift, vl8);
#else
        vint16m1_t x1mj =
            complex_mul_minus_j_with_idx_sign_i16m1(x1, swap_idx16, minus_j_sign, vl8);
        vint16m1_t x3mj =
            complex_mul_minus_j_with_idx_sign_i16m1(x3, swap_idx16, minus_j_sign, vl8);
        vint16m1_t x13d = __riscv_vssub_vv_i16m1(x1mj, x3mj, vl8);
        vint16m1_t y1 = rvv_shift_right_i16m1(__riscv_vsadd_vv_i16m1(x02d, x13d, vl8), shift, vl8);
        vint16m1_t y3 = rvv_shift_right_i16m1(__riscv_vssub_vv_i16m1(x02d, x13d, vl8), shift, vl8);
#endif

        __riscv_vse16_v_i16m1(dst + 0 * 128 + off, y0, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 128 + off, y1, vl8);
        __riscv_vse16_v_i16m1(dst + 2 * 128 + off, y2, vl8);
        __riscv_vse16_v_i16m1(dst + 3 * 128 + off, y3, vl8);
    }
}

static void rvv_combine_radix4_inverse_256_rvv(const int16_t *src, int16_t *dst, unsigned shift)
{
    const size_t vl8 = 8;
#if !FFT_CUSTOM_ROTATE_ADD_HELPERS
    vuint16m1_t swap_idx16 = __riscv_vle16_v_u16m1(swap_re_im_idx16, vl8);
    vint16m1_t minus_j_sign = __riscv_vle16_v_i16m1(minus_j_sign16, vl8);
#endif
    for (int block = 0; block < 16; ++block) {
        const int off = block * 8;
        vint16m1_t x0 = __riscv_vle16_v_i16m1(src + 0 * 128 + off, vl8);
        vint16m1_t x1 = __riscv_vle16_v_i16m1(src + 1 * 128 + off, vl8);
        vint16m1_t x2 = __riscv_vle16_v_i16m1(src + 2 * 128 + off, vl8);
        vint16m1_t x3 = __riscv_vle16_v_i16m1(src + 3 * 128 + off, vl8);

#ifdef FFT_CUSTOM_PACKED_CMUL_HELPERS
        vint16m1_t tw_re0 = __riscv_vle16_v_i16m1(ext256_tw_inv[0] + off, vl8);
        vint16m1_t tw_re1 = __riscv_vle16_v_i16m1(ext256_tw_inv[1] + off, vl8);
        vint16m1_t tw_re2 = __riscv_vle16_v_i16m1(ext256_tw_inv[2] + off, vl8);
#else
        vint16m1_t tw_re0 = __riscv_vle16_v_i16m1(rvv256_tw_re_dup[0] + off, vl8);
        vint16m1_t tw_im0 = __riscv_vle16_v_i16m1(rvv256_tw_im_inv[0] + off, vl8);
        vint16m1_t tw_re1 = __riscv_vle16_v_i16m1(rvv256_tw_re_dup[1] + off, vl8);
        vint16m1_t tw_im1 = __riscv_vle16_v_i16m1(rvv256_tw_im_inv[1] + off, vl8);
        vint16m1_t tw_re2 = __riscv_vle16_v_i16m1(rvv256_tw_re_dup[2] + off, vl8);
        vint16m1_t tw_im2 = __riscv_vle16_v_i16m1(rvv256_tw_im_inv[2] + off, vl8);
#endif

        packed_cmul3_q15_i16m1(&x1, &x2, &x3,
                               tw_re0, tw_im0,
                               tw_re1, tw_im1,
                               tw_re2, tw_im2,
                               swap_idx16, vl8);

        vint16m1_t x02 = __riscv_vsadd_vv_i16m1(x0, x2, vl8);
        vint16m1_t x13 = __riscv_vsadd_vv_i16m1(x1, x3, vl8);
        vint16m1_t y0 = rvv_shift_right_i16m1(__riscv_vsadd_vv_i16m1(x02, x13, vl8), shift, vl8);
        vint16m1_t y2 = rvv_shift_right_i16m1(__riscv_vssub_vv_i16m1(x02, x13, vl8), shift, vl8);
        vint16m1_t x02d = __riscv_vssub_vv_i16m1(x0, x2, vl8);
#if FFT_CUSTOM_ROTATE_ADD_COMBINE256
        vint16m1_t y1_raw;
        vint16m1_t y3_raw;
        radix4_inverse_rot_pair_i16m1(x02d, x1, x3, vl8, &y1_raw, &y3_raw);
        vint16m1_t y1 = rvv_shift_right_i16m1(y1_raw, shift, vl8);
        vint16m1_t y3 = rvv_shift_right_i16m1(y3_raw, shift, vl8);
#else
        vint16m1_t x1mj =
            complex_mul_minus_j_with_idx_sign_i16m1(x1, swap_idx16, minus_j_sign, vl8);
        vint16m1_t x3mj =
            complex_mul_minus_j_with_idx_sign_i16m1(x3, swap_idx16, minus_j_sign, vl8);
        vint16m1_t x13d = __riscv_vssub_vv_i16m1(x1mj, x3mj, vl8);
        vint16m1_t y3 = rvv_shift_right_i16m1(__riscv_vsadd_vv_i16m1(x02d, x13d, vl8), shift, vl8);
        vint16m1_t y1 = rvv_shift_right_i16m1(__riscv_vssub_vv_i16m1(x02d, x13d, vl8), shift, vl8);
#endif

        __riscv_vse16_v_i16m1(dst + 0 * 128 + off, y0, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 128 + off, y1, vl8);
        __riscv_vse16_v_i16m1(dst + 2 * 128 + off, y2, vl8);
        __riscv_vse16_v_i16m1(dst + 3 * 128 + off, y3, vl8);
    }
}

static void rvv_combine_radix2_forward_128_rvv(const int16_t *src, int16_t *dst, unsigned scale)
{
    const size_t vl8 = 8;
#ifdef FFT_CUSTOM_PACKED_CMUL_HELPERS
    for (int block = 0; block < 16; block += 4) {
        const int off0 = (block + 0) * 8;
        const int off1 = (block + 1) * 8;
        const int off2 = (block + 2) * 8;
        const int off3 = (block + 3) * 8;
        vint16m1_t x0_0 = __riscv_vle16_v_i16m1(src + 0 * 128 + off0, vl8);
        vint16m1_t x1_0 = __riscv_vle16_v_i16m1(src + 1 * 128 + off0, vl8);
        vint16m1_t tw0 = __riscv_vle16_v_i16m1(ext128_tw_fwd + off0, vl8);
        vint16m1_t x0_1 = __riscv_vle16_v_i16m1(src + 0 * 128 + off1, vl8);
        vint16m1_t x1_1 = __riscv_vle16_v_i16m1(src + 1 * 128 + off1, vl8);
        vint16m1_t tw1 = __riscv_vle16_v_i16m1(ext128_tw_fwd + off1, vl8);
        vint16m1_t x0_2 = __riscv_vle16_v_i16m1(src + 0 * 128 + off2, vl8);
        vint16m1_t x1_2 = __riscv_vle16_v_i16m1(src + 1 * 128 + off2, vl8);
        vint16m1_t tw2 = __riscv_vle16_v_i16m1(ext128_tw_fwd + off2, vl8);
        vint16m1_t x0_3 = __riscv_vle16_v_i16m1(src + 0 * 128 + off3, vl8);
        vint16m1_t x1_3 = __riscv_vle16_v_i16m1(src + 1 * 128 + off3, vl8);
        vint16m1_t tw3 = __riscv_vle16_v_i16m1(ext128_tw_fwd + off3, vl8);

        ext_packed_cmul4_i16m1(&x1_0, &x1_1, &x1_2, &x1_3, tw0, tw1, tw2, tw3);

        vint16m1_t y0_0 = __riscv_vsadd_vv_i16m1(x0_0, x1_0, vl8);
        vint16m1_t y1_0 = __riscv_vssub_vv_i16m1(x0_0, x1_0, vl8);
        vint16m1_t y0_1 = __riscv_vsadd_vv_i16m1(x0_1, x1_1, vl8);
        vint16m1_t y1_1 = __riscv_vssub_vv_i16m1(x0_1, x1_1, vl8);
        vint16m1_t y0_2 = __riscv_vsadd_vv_i16m1(x0_2, x1_2, vl8);
        vint16m1_t y1_2 = __riscv_vssub_vv_i16m1(x0_2, x1_2, vl8);
        vint16m1_t y0_3 = __riscv_vsadd_vv_i16m1(x0_3, x1_3, vl8);
        vint16m1_t y1_3 = __riscv_vssub_vv_i16m1(x0_3, x1_3, vl8);

        if (scale) {
            y0_0 = rvv_mulhrs_q15_i16m1(y0_0, 23170, vl8);
            y1_0 = rvv_mulhrs_q15_i16m1(y1_0, 23170, vl8);
            y0_1 = rvv_mulhrs_q15_i16m1(y0_1, 23170, vl8);
            y1_1 = rvv_mulhrs_q15_i16m1(y1_1, 23170, vl8);
            y0_2 = rvv_mulhrs_q15_i16m1(y0_2, 23170, vl8);
            y1_2 = rvv_mulhrs_q15_i16m1(y1_2, 23170, vl8);
            y0_3 = rvv_mulhrs_q15_i16m1(y0_3, 23170, vl8);
            y1_3 = rvv_mulhrs_q15_i16m1(y1_3, 23170, vl8);
        }

        __riscv_vse16_v_i16m1(dst + 0 * 128 + off0, y0_0, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 128 + off0, y1_0, vl8);
        __riscv_vse16_v_i16m1(dst + 0 * 128 + off1, y0_1, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 128 + off1, y1_1, vl8);
        __riscv_vse16_v_i16m1(dst + 0 * 128 + off2, y0_2, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 128 + off2, y1_2, vl8);
        __riscv_vse16_v_i16m1(dst + 0 * 128 + off3, y0_3, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 128 + off3, y1_3, vl8);
    }
#else
    vuint16m1_t swap_idx16 = __riscv_vle16_v_u16m1(swap_re_im_idx16, vl8);

    for (int block = 0; block < 16; ++block) {
        const int off = block * 8;
        vint16m1_t x0 = __riscv_vle16_v_i16m1(src + 0 * 128 + off, vl8);
        vint16m1_t x1 = __riscv_vle16_v_i16m1(src + 1 * 128 + off, vl8);
        vint16m1_t tw_re = __riscv_vle16_v_i16m1(rvv128_tw_re_dup + off, vl8);
        vint16m1_t tw_im = __riscv_vle16_v_i16m1(rvv128_tw_im_fwd + off, vl8);

        x1 = packed_cmul_q15_i16m1(x1, tw_re, tw_im, swap_idx16, vl8);

        vint16m1_t y0 = __riscv_vsadd_vv_i16m1(x0, x1, vl8);
        vint16m1_t y1 = __riscv_vssub_vv_i16m1(x0, x1, vl8);

        if (scale) {
            y0 = rvv_mulhrs_q15_i16m1(y0, 23170, vl8);
            y1 = rvv_mulhrs_q15_i16m1(y1, 23170, vl8);
        }

        __riscv_vse16_v_i16m1(dst + 0 * 128 + off, y0, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 128 + off, y1, vl8);
    }
#endif
}

static void rvv_combine_radix2_inverse_128_rvv(const int16_t *src, int16_t *dst, unsigned scale)
{
    const size_t vl8 = 8;
#ifdef FFT_CUSTOM_PACKED_CMUL_HELPERS
    for (int block = 0; block < 16; block += 4) {
        const int off0 = (block + 0) * 8;
        const int off1 = (block + 1) * 8;
        const int off2 = (block + 2) * 8;
        const int off3 = (block + 3) * 8;
        vint16m1_t x0_0 = __riscv_vle16_v_i16m1(src + 0 * 128 + off0, vl8);
        vint16m1_t x1_0 = __riscv_vle16_v_i16m1(src + 1 * 128 + off0, vl8);
        vint16m1_t tw0 = __riscv_vle16_v_i16m1(ext128_tw_inv + off0, vl8);
        vint16m1_t x0_1 = __riscv_vle16_v_i16m1(src + 0 * 128 + off1, vl8);
        vint16m1_t x1_1 = __riscv_vle16_v_i16m1(src + 1 * 128 + off1, vl8);
        vint16m1_t tw1 = __riscv_vle16_v_i16m1(ext128_tw_inv + off1, vl8);
        vint16m1_t x0_2 = __riscv_vle16_v_i16m1(src + 0 * 128 + off2, vl8);
        vint16m1_t x1_2 = __riscv_vle16_v_i16m1(src + 1 * 128 + off2, vl8);
        vint16m1_t tw2 = __riscv_vle16_v_i16m1(ext128_tw_inv + off2, vl8);
        vint16m1_t x0_3 = __riscv_vle16_v_i16m1(src + 0 * 128 + off3, vl8);
        vint16m1_t x1_3 = __riscv_vle16_v_i16m1(src + 1 * 128 + off3, vl8);
        vint16m1_t tw3 = __riscv_vle16_v_i16m1(ext128_tw_inv + off3, vl8);

        ext_packed_cmul4_i16m1(&x1_0, &x1_1, &x1_2, &x1_3, tw0, tw1, tw2, tw3);

        vint16m1_t y0_0 = __riscv_vsadd_vv_i16m1(x0_0, x1_0, vl8);
        vint16m1_t y1_0 = __riscv_vssub_vv_i16m1(x0_0, x1_0, vl8);
        vint16m1_t y0_1 = __riscv_vsadd_vv_i16m1(x0_1, x1_1, vl8);
        vint16m1_t y1_1 = __riscv_vssub_vv_i16m1(x0_1, x1_1, vl8);
        vint16m1_t y0_2 = __riscv_vsadd_vv_i16m1(x0_2, x1_2, vl8);
        vint16m1_t y1_2 = __riscv_vssub_vv_i16m1(x0_2, x1_2, vl8);
        vint16m1_t y0_3 = __riscv_vsadd_vv_i16m1(x0_3, x1_3, vl8);
        vint16m1_t y1_3 = __riscv_vssub_vv_i16m1(x0_3, x1_3, vl8);

        if (scale) {
            y0_0 = rvv_mulhrs_q15_i16m1(y0_0, 23170, vl8);
            y1_0 = rvv_mulhrs_q15_i16m1(y1_0, 23170, vl8);
            y0_1 = rvv_mulhrs_q15_i16m1(y0_1, 23170, vl8);
            y1_1 = rvv_mulhrs_q15_i16m1(y1_1, 23170, vl8);
            y0_2 = rvv_mulhrs_q15_i16m1(y0_2, 23170, vl8);
            y1_2 = rvv_mulhrs_q15_i16m1(y1_2, 23170, vl8);
            y0_3 = rvv_mulhrs_q15_i16m1(y0_3, 23170, vl8);
            y1_3 = rvv_mulhrs_q15_i16m1(y1_3, 23170, vl8);
        }

        __riscv_vse16_v_i16m1(dst + 0 * 128 + off0, y0_0, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 128 + off0, y1_0, vl8);
        __riscv_vse16_v_i16m1(dst + 0 * 128 + off1, y0_1, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 128 + off1, y1_1, vl8);
        __riscv_vse16_v_i16m1(dst + 0 * 128 + off2, y0_2, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 128 + off2, y1_2, vl8);
        __riscv_vse16_v_i16m1(dst + 0 * 128 + off3, y0_3, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 128 + off3, y1_3, vl8);
    }
#else
    vuint16m1_t swap_idx16 = __riscv_vle16_v_u16m1(swap_re_im_idx16, vl8);

    for (int block = 0; block < 16; ++block) {
        const int off = block * 8;
        vint16m1_t x0 = __riscv_vle16_v_i16m1(src + 0 * 128 + off, vl8);
        vint16m1_t x1 = __riscv_vle16_v_i16m1(src + 1 * 128 + off, vl8);
        vint16m1_t tw_re = __riscv_vle16_v_i16m1(rvv128_tw_re_dup + off, vl8);
        vint16m1_t tw_im = __riscv_vle16_v_i16m1(rvv128_tw_im_inv + off, vl8);

        x1 = packed_cmul_q15_i16m1(x1, tw_re, tw_im, swap_idx16, vl8);

        vint16m1_t y0 = __riscv_vsadd_vv_i16m1(x0, x1, vl8);
        vint16m1_t y1 = __riscv_vssub_vv_i16m1(x0, x1, vl8);

        if (scale) {
            y0 = rvv_mulhrs_q15_i16m1(y0, 23170, vl8);
            y1 = rvv_mulhrs_q15_i16m1(y1, 23170, vl8);
        }

        __riscv_vse16_v_i16m1(dst + 0 * 128 + off, y0, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 128 + off, y1, vl8);
    }
#endif
}

static void rvv_combine_radix2_forward_512_rvv(const int16_t *src, int16_t *dst, unsigned scale)
{
    const size_t vl8 = 8;
#ifdef FFT_CUSTOM_PACKED_CMUL_HELPERS
    for (int block = 0; block < 64; block += 4) {
        const int off0 = (block + 0) * 8;
        const int off1 = (block + 1) * 8;
        const int off2 = (block + 2) * 8;
        const int off3 = (block + 3) * 8;
        vint16m1_t x0_0 = __riscv_vle16_v_i16m1(src + 0 * 512 + off0, vl8);
        vint16m1_t x1_0 = __riscv_vle16_v_i16m1(src + 1 * 512 + off0, vl8);
        vint16m1_t tw0 = __riscv_vle16_v_i16m1(ext512_tw_fwd + off0, vl8);
        vint16m1_t x0_1 = __riscv_vle16_v_i16m1(src + 0 * 512 + off1, vl8);
        vint16m1_t x1_1 = __riscv_vle16_v_i16m1(src + 1 * 512 + off1, vl8);
        vint16m1_t tw1 = __riscv_vle16_v_i16m1(ext512_tw_fwd + off1, vl8);
        vint16m1_t x0_2 = __riscv_vle16_v_i16m1(src + 0 * 512 + off2, vl8);
        vint16m1_t x1_2 = __riscv_vle16_v_i16m1(src + 1 * 512 + off2, vl8);
        vint16m1_t tw2 = __riscv_vle16_v_i16m1(ext512_tw_fwd + off2, vl8);
        vint16m1_t x0_3 = __riscv_vle16_v_i16m1(src + 0 * 512 + off3, vl8);
        vint16m1_t x1_3 = __riscv_vle16_v_i16m1(src + 1 * 512 + off3, vl8);
        vint16m1_t tw3 = __riscv_vle16_v_i16m1(ext512_tw_fwd + off3, vl8);

        ext_packed_cmul4_i16m1(&x1_0, &x1_1, &x1_2, &x1_3, tw0, tw1, tw2, tw3);

        vint16m1_t y0_0 = __riscv_vsadd_vv_i16m1(x0_0, x1_0, vl8);
        vint16m1_t y1_0 = __riscv_vssub_vv_i16m1(x0_0, x1_0, vl8);
        vint16m1_t y0_1 = __riscv_vsadd_vv_i16m1(x0_1, x1_1, vl8);
        vint16m1_t y1_1 = __riscv_vssub_vv_i16m1(x0_1, x1_1, vl8);
        vint16m1_t y0_2 = __riscv_vsadd_vv_i16m1(x0_2, x1_2, vl8);
        vint16m1_t y1_2 = __riscv_vssub_vv_i16m1(x0_2, x1_2, vl8);
        vint16m1_t y0_3 = __riscv_vsadd_vv_i16m1(x0_3, x1_3, vl8);
        vint16m1_t y1_3 = __riscv_vssub_vv_i16m1(x0_3, x1_3, vl8);

        if (scale) {
            y0_0 = rvv_mulhrs_q15_i16m1(y0_0, 23170, vl8);
            y1_0 = rvv_mulhrs_q15_i16m1(y1_0, 23170, vl8);
            y0_1 = rvv_mulhrs_q15_i16m1(y0_1, 23170, vl8);
            y1_1 = rvv_mulhrs_q15_i16m1(y1_1, 23170, vl8);
            y0_2 = rvv_mulhrs_q15_i16m1(y0_2, 23170, vl8);
            y1_2 = rvv_mulhrs_q15_i16m1(y1_2, 23170, vl8);
            y0_3 = rvv_mulhrs_q15_i16m1(y0_3, 23170, vl8);
            y1_3 = rvv_mulhrs_q15_i16m1(y1_3, 23170, vl8);
        }

        __riscv_vse16_v_i16m1(dst + 0 * 512 + off0, y0_0, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 512 + off0, y1_0, vl8);
        __riscv_vse16_v_i16m1(dst + 0 * 512 + off1, y0_1, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 512 + off1, y1_1, vl8);
        __riscv_vse16_v_i16m1(dst + 0 * 512 + off2, y0_2, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 512 + off2, y1_2, vl8);
        __riscv_vse16_v_i16m1(dst + 0 * 512 + off3, y0_3, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 512 + off3, y1_3, vl8);
    }
#else
    vuint16m1_t swap_idx16 = __riscv_vle16_v_u16m1(swap_re_im_idx16, vl8);

    for (int block = 0; block < 64; ++block) {
        const int off = block * 8;
        vint16m1_t x0 = __riscv_vle16_v_i16m1(src + 0 * 512 + off, vl8);
        vint16m1_t x1 = __riscv_vle16_v_i16m1(src + 1 * 512 + off, vl8);
        vint16m1_t tw_re = __riscv_vle16_v_i16m1(rvv512_tw_re_dup + off, vl8);
        vint16m1_t tw_im = __riscv_vle16_v_i16m1(rvv512_tw_im_fwd + off, vl8);

        x1 = packed_cmul_q15_i16m1(x1, tw_re, tw_im, swap_idx16, vl8);

        vint16m1_t y0 = __riscv_vsadd_vv_i16m1(x0, x1, vl8);
        vint16m1_t y1 = __riscv_vssub_vv_i16m1(x0, x1, vl8);

        if (scale) {
            y0 = rvv_mulhrs_q15_i16m1(y0, 23170, vl8);
            y1 = rvv_mulhrs_q15_i16m1(y1, 23170, vl8);
        }

        __riscv_vse16_v_i16m1(dst + 0 * 512 + off, y0, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 512 + off, y1, vl8);
    }
#endif
}

static void rvv_combine_radix2_inverse_512_rvv(const int16_t *src, int16_t *dst, unsigned scale)
{
    const size_t vl8 = 8;
#ifdef FFT_CUSTOM_PACKED_CMUL_HELPERS
    for (int block = 0; block < 64; block += 4) {
        const int off0 = (block + 0) * 8;
        const int off1 = (block + 1) * 8;
        const int off2 = (block + 2) * 8;
        const int off3 = (block + 3) * 8;
        vint16m1_t x0_0 = __riscv_vle16_v_i16m1(src + 0 * 512 + off0, vl8);
        vint16m1_t x1_0 = __riscv_vle16_v_i16m1(src + 1 * 512 + off0, vl8);
        vint16m1_t tw0 = __riscv_vle16_v_i16m1(ext512_tw_inv + off0, vl8);
        vint16m1_t x0_1 = __riscv_vle16_v_i16m1(src + 0 * 512 + off1, vl8);
        vint16m1_t x1_1 = __riscv_vle16_v_i16m1(src + 1 * 512 + off1, vl8);
        vint16m1_t tw1 = __riscv_vle16_v_i16m1(ext512_tw_inv + off1, vl8);
        vint16m1_t x0_2 = __riscv_vle16_v_i16m1(src + 0 * 512 + off2, vl8);
        vint16m1_t x1_2 = __riscv_vle16_v_i16m1(src + 1 * 512 + off2, vl8);
        vint16m1_t tw2 = __riscv_vle16_v_i16m1(ext512_tw_inv + off2, vl8);
        vint16m1_t x0_3 = __riscv_vle16_v_i16m1(src + 0 * 512 + off3, vl8);
        vint16m1_t x1_3 = __riscv_vle16_v_i16m1(src + 1 * 512 + off3, vl8);
        vint16m1_t tw3 = __riscv_vle16_v_i16m1(ext512_tw_inv + off3, vl8);

        ext_packed_cmul4_i16m1(&x1_0, &x1_1, &x1_2, &x1_3, tw0, tw1, tw2, tw3);

        vint16m1_t y0_0 = __riscv_vsadd_vv_i16m1(x0_0, x1_0, vl8);
        vint16m1_t y1_0 = __riscv_vssub_vv_i16m1(x0_0, x1_0, vl8);
        vint16m1_t y0_1 = __riscv_vsadd_vv_i16m1(x0_1, x1_1, vl8);
        vint16m1_t y1_1 = __riscv_vssub_vv_i16m1(x0_1, x1_1, vl8);
        vint16m1_t y0_2 = __riscv_vsadd_vv_i16m1(x0_2, x1_2, vl8);
        vint16m1_t y1_2 = __riscv_vssub_vv_i16m1(x0_2, x1_2, vl8);
        vint16m1_t y0_3 = __riscv_vsadd_vv_i16m1(x0_3, x1_3, vl8);
        vint16m1_t y1_3 = __riscv_vssub_vv_i16m1(x0_3, x1_3, vl8);

        if (scale) {
            y0_0 = rvv_mulhrs_q15_i16m1(y0_0, 23170, vl8);
            y1_0 = rvv_mulhrs_q15_i16m1(y1_0, 23170, vl8);
            y0_1 = rvv_mulhrs_q15_i16m1(y0_1, 23170, vl8);
            y1_1 = rvv_mulhrs_q15_i16m1(y1_1, 23170, vl8);
            y0_2 = rvv_mulhrs_q15_i16m1(y0_2, 23170, vl8);
            y1_2 = rvv_mulhrs_q15_i16m1(y1_2, 23170, vl8);
            y0_3 = rvv_mulhrs_q15_i16m1(y0_3, 23170, vl8);
            y1_3 = rvv_mulhrs_q15_i16m1(y1_3, 23170, vl8);
        }

        __riscv_vse16_v_i16m1(dst + 0 * 512 + off0, y0_0, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 512 + off0, y1_0, vl8);
        __riscv_vse16_v_i16m1(dst + 0 * 512 + off1, y0_1, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 512 + off1, y1_1, vl8);
        __riscv_vse16_v_i16m1(dst + 0 * 512 + off2, y0_2, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 512 + off2, y1_2, vl8);
        __riscv_vse16_v_i16m1(dst + 0 * 512 + off3, y0_3, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 512 + off3, y1_3, vl8);
    }
#else
    vuint16m1_t swap_idx16 = __riscv_vle16_v_u16m1(swap_re_im_idx16, vl8);

    for (int block = 0; block < 64; ++block) {
        const int off = block * 8;
        vint16m1_t x0 = __riscv_vle16_v_i16m1(src + 0 * 512 + off, vl8);
        vint16m1_t x1 = __riscv_vle16_v_i16m1(src + 1 * 512 + off, vl8);
        vint16m1_t tw_re = __riscv_vle16_v_i16m1(rvv512_tw_re_dup + off, vl8);
        vint16m1_t tw_im = __riscv_vle16_v_i16m1(rvv512_tw_im_inv + off, vl8);

        x1 = packed_cmul_q15_i16m1(x1, tw_re, tw_im, swap_idx16, vl8);

        vint16m1_t y0 = __riscv_vsadd_vv_i16m1(x0, x1, vl8);
        vint16m1_t y1 = __riscv_vssub_vv_i16m1(x0, x1, vl8);

        if (scale) {
            y0 = rvv_mulhrs_q15_i16m1(y0, 23170, vl8);
            y1 = rvv_mulhrs_q15_i16m1(y1, 23170, vl8);
        }

        __riscv_vse16_v_i16m1(dst + 0 * 512 + off, y0, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 512 + off, y1, vl8);
    }
#endif
}

static void rvv_combine_radix4_forward_1024_rvv(const int16_t *src, int16_t *dst, unsigned scale)
{
    const size_t vl8 = 8;
#if !FFT_CUSTOM_ROTATE_ADD_HELPERS
    vuint16m1_t swap_idx16 = __riscv_vle16_v_u16m1(swap_re_im_idx16, vl8);
    vint16m1_t minus_j_sign = __riscv_vle16_v_i16m1(minus_j_sign16, vl8);
#endif
    for (int block = 0; block < 64; ++block) {
        const int off = block * 8;
        vint16m1_t x0 = __riscv_vle16_v_i16m1(src + 0 * 512 + off, vl8);
        vint16m1_t x1 = __riscv_vle16_v_i16m1(src + 1 * 512 + off, vl8);
        vint16m1_t x2 = __riscv_vle16_v_i16m1(src + 2 * 512 + off, vl8);
        vint16m1_t x3 = __riscv_vle16_v_i16m1(src + 3 * 512 + off, vl8);

#ifdef FFT_CUSTOM_PACKED_CMUL_HELPERS
        vint16m1_t tw_re0 = __riscv_vle16_v_i16m1(ext1024_tw_fwd[0] + off, vl8);
        vint16m1_t tw_re1 = __riscv_vle16_v_i16m1(ext1024_tw_fwd[1] + off, vl8);
        vint16m1_t tw_re2 = __riscv_vle16_v_i16m1(ext1024_tw_fwd[2] + off, vl8);
#else
        vint16m1_t tw_re0 = __riscv_vle16_v_i16m1(rvv1024_tw_re_dup[0] + off, vl8);
        vint16m1_t tw_im0 = __riscv_vle16_v_i16m1(rvv1024_tw_im_fwd[0] + off, vl8);
        vint16m1_t tw_re1 = __riscv_vle16_v_i16m1(rvv1024_tw_re_dup[1] + off, vl8);
        vint16m1_t tw_im1 = __riscv_vle16_v_i16m1(rvv1024_tw_im_fwd[1] + off, vl8);
        vint16m1_t tw_re2 = __riscv_vle16_v_i16m1(rvv1024_tw_re_dup[2] + off, vl8);
        vint16m1_t tw_im2 = __riscv_vle16_v_i16m1(rvv1024_tw_im_fwd[2] + off, vl8);
#endif

        packed_cmul3_q15_i16m1(&x1, &x2, &x3,
                               tw_re0, tw_im0,
                               tw_re1, tw_im1,
                               tw_re2, tw_im2,
                               swap_idx16, vl8);

        vint16m1_t x02 = __riscv_vsadd_vv_i16m1(x0, x2, vl8);
        vint16m1_t x13 = __riscv_vsadd_vv_i16m1(x1, x3, vl8);
        vint16m1_t y0 = __riscv_vsadd_vv_i16m1(x02, x13, vl8);
        vint16m1_t y2 = __riscv_vssub_vv_i16m1(x02, x13, vl8);
        vint16m1_t x02d = __riscv_vssub_vv_i16m1(x0, x2, vl8);
#if FFT_CUSTOM_ROTATE_ADD_COMBINE1024
        vint16m1_t y1;
        vint16m1_t y3;
        radix4_forward_rot_pair_i16m1(x02d, x1, x3, vl8, &y1, &y3);
#else
        vint16m1_t x1mj =
            complex_mul_minus_j_with_idx_sign_i16m1(x1, swap_idx16, minus_j_sign, vl8);
        vint16m1_t x3mj =
            complex_mul_minus_j_with_idx_sign_i16m1(x3, swap_idx16, minus_j_sign, vl8);
        vint16m1_t x13d = __riscv_vssub_vv_i16m1(x1mj, x3mj, vl8);
        vint16m1_t y1 = __riscv_vsadd_vv_i16m1(x02d, x13d, vl8);
        vint16m1_t y3 = __riscv_vssub_vv_i16m1(x02d, x13d, vl8);
#endif

        if (scale) {
            y0 = rvv_mulhrs_q15_i16m1(y0, 23170, vl8);
            y1 = rvv_mulhrs_q15_i16m1(y1, 23170, vl8);
            y2 = rvv_mulhrs_q15_i16m1(y2, 23170, vl8);
            y3 = rvv_mulhrs_q15_i16m1(y3, 23170, vl8);
        }

        __riscv_vse16_v_i16m1(dst + 0 * 512 + off, y0, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 512 + off, y1, vl8);
        __riscv_vse16_v_i16m1(dst + 2 * 512 + off, y2, vl8);
        __riscv_vse16_v_i16m1(dst + 3 * 512 + off, y3, vl8);
    }
}

static void rvv_combine_radix4_inverse_1024_rvv(const int16_t *src, int16_t *dst, unsigned scale)
{
    const size_t vl8 = 8;
#if !FFT_CUSTOM_ROTATE_ADD_HELPERS
    vuint16m1_t swap_idx16 = __riscv_vle16_v_u16m1(swap_re_im_idx16, vl8);
    vint16m1_t minus_j_sign = __riscv_vle16_v_i16m1(minus_j_sign16, vl8);
#endif
    for (int block = 0; block < 64; ++block) {
        const int off = block * 8;
        vint16m1_t x0 = __riscv_vle16_v_i16m1(src + 0 * 512 + off, vl8);
        vint16m1_t x1 = __riscv_vle16_v_i16m1(src + 1 * 512 + off, vl8);
        vint16m1_t x2 = __riscv_vle16_v_i16m1(src + 2 * 512 + off, vl8);
        vint16m1_t x3 = __riscv_vle16_v_i16m1(src + 3 * 512 + off, vl8);

#ifdef FFT_CUSTOM_PACKED_CMUL_HELPERS
        vint16m1_t tw_re0 = __riscv_vle16_v_i16m1(ext1024_tw_inv[0] + off, vl8);
        vint16m1_t tw_re1 = __riscv_vle16_v_i16m1(ext1024_tw_inv[1] + off, vl8);
        vint16m1_t tw_re2 = __riscv_vle16_v_i16m1(ext1024_tw_inv[2] + off, vl8);
#else
        vint16m1_t tw_re0 = __riscv_vle16_v_i16m1(rvv1024_tw_re_dup[0] + off, vl8);
        vint16m1_t tw_im0 = __riscv_vle16_v_i16m1(rvv1024_tw_im_inv[0] + off, vl8);
        vint16m1_t tw_re1 = __riscv_vle16_v_i16m1(rvv1024_tw_re_dup[1] + off, vl8);
        vint16m1_t tw_im1 = __riscv_vle16_v_i16m1(rvv1024_tw_im_inv[1] + off, vl8);
        vint16m1_t tw_re2 = __riscv_vle16_v_i16m1(rvv1024_tw_re_dup[2] + off, vl8);
        vint16m1_t tw_im2 = __riscv_vle16_v_i16m1(rvv1024_tw_im_inv[2] + off, vl8);
#endif

        packed_cmul3_q15_i16m1(&x1, &x2, &x3,
                               tw_re0, tw_im0,
                               tw_re1, tw_im1,
                               tw_re2, tw_im2,
                               swap_idx16, vl8);

        vint16m1_t x02 = __riscv_vsadd_vv_i16m1(x0, x2, vl8);
        vint16m1_t x13 = __riscv_vsadd_vv_i16m1(x1, x3, vl8);
        vint16m1_t y0 = __riscv_vsadd_vv_i16m1(x02, x13, vl8);
        vint16m1_t y2 = __riscv_vssub_vv_i16m1(x02, x13, vl8);
        vint16m1_t x02d = __riscv_vssub_vv_i16m1(x0, x2, vl8);
#if FFT_CUSTOM_ROTATE_ADD_COMBINE1024
        vint16m1_t y1;
        vint16m1_t y3;
        radix4_inverse_rot_pair_i16m1(x02d, x1, x3, vl8, &y1, &y3);
#else
        vint16m1_t x1mj =
            complex_mul_minus_j_with_idx_sign_i16m1(x1, swap_idx16, minus_j_sign, vl8);
        vint16m1_t x3mj =
            complex_mul_minus_j_with_idx_sign_i16m1(x3, swap_idx16, minus_j_sign, vl8);
        vint16m1_t x13d = __riscv_vssub_vv_i16m1(x1mj, x3mj, vl8);
        vint16m1_t y3 = __riscv_vsadd_vv_i16m1(x02d, x13d, vl8);
        vint16m1_t y1 = __riscv_vssub_vv_i16m1(x02d, x13d, vl8);
#endif

        if (scale) {
            y0 = rvv_mulhrs_q15_i16m1(y0, 23170, vl8);
            y1 = rvv_mulhrs_q15_i16m1(y1, 23170, vl8);
            y2 = rvv_mulhrs_q15_i16m1(y2, 23170, vl8);
            y3 = rvv_mulhrs_q15_i16m1(y3, 23170, vl8);
        }

        __riscv_vse16_v_i16m1(dst + 0 * 512 + off, y0, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 512 + off, y1, vl8);
        __riscv_vse16_v_i16m1(dst + 2 * 512 + off, y2, vl8);
        __riscv_vse16_v_i16m1(dst + 3 * 512 + off, y3, vl8);
    }
}

static void rvv_combine_radix2_forward_2048_rvv(const int16_t *src, int16_t *dst, unsigned scale)
{
    const size_t vl8 = 8;
#ifdef FFT_CUSTOM_PACKED_CMUL_HELPERS
    for (int block = 0; block < 256; block += 4) {
        const int off0 = (block + 0) * 8;
        const int off1 = (block + 1) * 8;
        const int off2 = (block + 2) * 8;
        const int off3 = (block + 3) * 8;
        vint16m1_t x0_0 = __riscv_vle16_v_i16m1(src + 0 * 2048 + off0, vl8);
        vint16m1_t x1_0 = __riscv_vle16_v_i16m1(src + 1 * 2048 + off0, vl8);
        vint16m1_t tw0 = __riscv_vle16_v_i16m1(ext2048_tw_fwd + off0, vl8);
        vint16m1_t x0_1 = __riscv_vle16_v_i16m1(src + 0 * 2048 + off1, vl8);
        vint16m1_t x1_1 = __riscv_vle16_v_i16m1(src + 1 * 2048 + off1, vl8);
        vint16m1_t tw1 = __riscv_vle16_v_i16m1(ext2048_tw_fwd + off1, vl8);
        vint16m1_t x0_2 = __riscv_vle16_v_i16m1(src + 0 * 2048 + off2, vl8);
        vint16m1_t x1_2 = __riscv_vle16_v_i16m1(src + 1 * 2048 + off2, vl8);
        vint16m1_t tw2 = __riscv_vle16_v_i16m1(ext2048_tw_fwd + off2, vl8);
        vint16m1_t x0_3 = __riscv_vle16_v_i16m1(src + 0 * 2048 + off3, vl8);
        vint16m1_t x1_3 = __riscv_vle16_v_i16m1(src + 1 * 2048 + off3, vl8);
        vint16m1_t tw3 = __riscv_vle16_v_i16m1(ext2048_tw_fwd + off3, vl8);

        ext_packed_cmul4_i16m1(&x1_0, &x1_1, &x1_2, &x1_3, tw0, tw1, tw2, tw3);

        vint16m1_t y0_0 = __riscv_vsadd_vv_i16m1(x0_0, x1_0, vl8);
        vint16m1_t y1_0 = __riscv_vssub_vv_i16m1(x0_0, x1_0, vl8);
        vint16m1_t y0_1 = __riscv_vsadd_vv_i16m1(x0_1, x1_1, vl8);
        vint16m1_t y1_1 = __riscv_vssub_vv_i16m1(x0_1, x1_1, vl8);
        vint16m1_t y0_2 = __riscv_vsadd_vv_i16m1(x0_2, x1_2, vl8);
        vint16m1_t y1_2 = __riscv_vssub_vv_i16m1(x0_2, x1_2, vl8);
        vint16m1_t y0_3 = __riscv_vsadd_vv_i16m1(x0_3, x1_3, vl8);
        vint16m1_t y1_3 = __riscv_vssub_vv_i16m1(x0_3, x1_3, vl8);

        if (scale) {
            y0_0 = rvv_mulhrs_q15_i16m1(y0_0, 23170, vl8);
            y1_0 = rvv_mulhrs_q15_i16m1(y1_0, 23170, vl8);
            y0_1 = rvv_mulhrs_q15_i16m1(y0_1, 23170, vl8);
            y1_1 = rvv_mulhrs_q15_i16m1(y1_1, 23170, vl8);
            y0_2 = rvv_mulhrs_q15_i16m1(y0_2, 23170, vl8);
            y1_2 = rvv_mulhrs_q15_i16m1(y1_2, 23170, vl8);
            y0_3 = rvv_mulhrs_q15_i16m1(y0_3, 23170, vl8);
            y1_3 = rvv_mulhrs_q15_i16m1(y1_3, 23170, vl8);
        }

        __riscv_vse16_v_i16m1(dst + 0 * 2048 + off0, y0_0, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 2048 + off0, y1_0, vl8);
        __riscv_vse16_v_i16m1(dst + 0 * 2048 + off1, y0_1, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 2048 + off1, y1_1, vl8);
        __riscv_vse16_v_i16m1(dst + 0 * 2048 + off2, y0_2, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 2048 + off2, y1_2, vl8);
        __riscv_vse16_v_i16m1(dst + 0 * 2048 + off3, y0_3, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 2048 + off3, y1_3, vl8);
    }
#else
    vuint16m1_t swap_idx16 = __riscv_vle16_v_u16m1(swap_re_im_idx16, vl8);

    for (int block = 0; block < 256; ++block) {
        const int off = block * 8;
        vint16m1_t x0 = __riscv_vle16_v_i16m1(src + 0 * 2048 + off, vl8);
        vint16m1_t x1 = __riscv_vle16_v_i16m1(src + 1 * 2048 + off, vl8);
        vint16m1_t tw_re = __riscv_vle16_v_i16m1(rvv2048_tw_re_dup + off, vl8);
        vint16m1_t tw_im = __riscv_vle16_v_i16m1(rvv2048_tw_im_fwd + off, vl8);

        x1 = packed_cmul_q15_i16m1(x1, tw_re, tw_im, swap_idx16, vl8);

        vint16m1_t y0 = __riscv_vsadd_vv_i16m1(x0, x1, vl8);
        vint16m1_t y1 = __riscv_vssub_vv_i16m1(x0, x1, vl8);

        if (scale) {
            y0 = rvv_mulhrs_q15_i16m1(y0, 23170, vl8);
            y1 = rvv_mulhrs_q15_i16m1(y1, 23170, vl8);
        }

        __riscv_vse16_v_i16m1(dst + 0 * 2048 + off, y0, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 2048 + off, y1, vl8);
    }
#endif
}

static void rvv_combine_radix2_inverse_2048_rvv(const int16_t *src, int16_t *dst, unsigned scale)
{
    const size_t vl8 = 8;
#ifdef FFT_CUSTOM_PACKED_CMUL_HELPERS
    for (int block = 0; block < 256; block += 4) {
        const int off0 = (block + 0) * 8;
        const int off1 = (block + 1) * 8;
        const int off2 = (block + 2) * 8;
        const int off3 = (block + 3) * 8;
        vint16m1_t x0_0 = __riscv_vle16_v_i16m1(src + 0 * 2048 + off0, vl8);
        vint16m1_t x1_0 = __riscv_vle16_v_i16m1(src + 1 * 2048 + off0, vl8);
        vint16m1_t tw0 = __riscv_vle16_v_i16m1(ext2048_tw_inv + off0, vl8);
        vint16m1_t x0_1 = __riscv_vle16_v_i16m1(src + 0 * 2048 + off1, vl8);
        vint16m1_t x1_1 = __riscv_vle16_v_i16m1(src + 1 * 2048 + off1, vl8);
        vint16m1_t tw1 = __riscv_vle16_v_i16m1(ext2048_tw_inv + off1, vl8);
        vint16m1_t x0_2 = __riscv_vle16_v_i16m1(src + 0 * 2048 + off2, vl8);
        vint16m1_t x1_2 = __riscv_vle16_v_i16m1(src + 1 * 2048 + off2, vl8);
        vint16m1_t tw2 = __riscv_vle16_v_i16m1(ext2048_tw_inv + off2, vl8);
        vint16m1_t x0_3 = __riscv_vle16_v_i16m1(src + 0 * 2048 + off3, vl8);
        vint16m1_t x1_3 = __riscv_vle16_v_i16m1(src + 1 * 2048 + off3, vl8);
        vint16m1_t tw3 = __riscv_vle16_v_i16m1(ext2048_tw_inv + off3, vl8);

        ext_packed_cmul4_i16m1(&x1_0, &x1_1, &x1_2, &x1_3, tw0, tw1, tw2, tw3);

        vint16m1_t y0_0 = __riscv_vsadd_vv_i16m1(x0_0, x1_0, vl8);
        vint16m1_t y1_0 = __riscv_vssub_vv_i16m1(x0_0, x1_0, vl8);
        vint16m1_t y0_1 = __riscv_vsadd_vv_i16m1(x0_1, x1_1, vl8);
        vint16m1_t y1_1 = __riscv_vssub_vv_i16m1(x0_1, x1_1, vl8);
        vint16m1_t y0_2 = __riscv_vsadd_vv_i16m1(x0_2, x1_2, vl8);
        vint16m1_t y1_2 = __riscv_vssub_vv_i16m1(x0_2, x1_2, vl8);
        vint16m1_t y0_3 = __riscv_vsadd_vv_i16m1(x0_3, x1_3, vl8);
        vint16m1_t y1_3 = __riscv_vssub_vv_i16m1(x0_3, x1_3, vl8);

        if (scale) {
            y0_0 = rvv_mulhrs_q15_i16m1(y0_0, 23170, vl8);
            y1_0 = rvv_mulhrs_q15_i16m1(y1_0, 23170, vl8);
            y0_1 = rvv_mulhrs_q15_i16m1(y0_1, 23170, vl8);
            y1_1 = rvv_mulhrs_q15_i16m1(y1_1, 23170, vl8);
            y0_2 = rvv_mulhrs_q15_i16m1(y0_2, 23170, vl8);
            y1_2 = rvv_mulhrs_q15_i16m1(y1_2, 23170, vl8);
            y0_3 = rvv_mulhrs_q15_i16m1(y0_3, 23170, vl8);
            y1_3 = rvv_mulhrs_q15_i16m1(y1_3, 23170, vl8);
        }

        __riscv_vse16_v_i16m1(dst + 0 * 2048 + off0, y0_0, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 2048 + off0, y1_0, vl8);
        __riscv_vse16_v_i16m1(dst + 0 * 2048 + off1, y0_1, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 2048 + off1, y1_1, vl8);
        __riscv_vse16_v_i16m1(dst + 0 * 2048 + off2, y0_2, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 2048 + off2, y1_2, vl8);
        __riscv_vse16_v_i16m1(dst + 0 * 2048 + off3, y0_3, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 2048 + off3, y1_3, vl8);
    }
#else
    vuint16m1_t swap_idx16 = __riscv_vle16_v_u16m1(swap_re_im_idx16, vl8);

    for (int block = 0; block < 256; ++block) {
        const int off = block * 8;
        vint16m1_t x0 = __riscv_vle16_v_i16m1(src + 0 * 2048 + off, vl8);
        vint16m1_t x1 = __riscv_vle16_v_i16m1(src + 1 * 2048 + off, vl8);
        vint16m1_t tw_re = __riscv_vle16_v_i16m1(rvv2048_tw_re_dup + off, vl8);
        vint16m1_t tw_im = __riscv_vle16_v_i16m1(rvv2048_tw_im_inv + off, vl8);

        x1 = packed_cmul_q15_i16m1(x1, tw_re, tw_im, swap_idx16, vl8);

        vint16m1_t y0 = __riscv_vsadd_vv_i16m1(x0, x1, vl8);
        vint16m1_t y1 = __riscv_vssub_vv_i16m1(x0, x1, vl8);

        if (scale) {
            y0 = rvv_mulhrs_q15_i16m1(y0, 23170, vl8);
            y1 = rvv_mulhrs_q15_i16m1(y1, 23170, vl8);
        }

        __riscv_vse16_v_i16m1(dst + 0 * 2048 + off, y0, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 2048 + off, y1, vl8);
    }
#endif
}

static void rvv_combine_radix4_forward_4096_rvv(const int16_t *src, int16_t *dst, unsigned shift)
{
    const size_t vl8 = 8;
#if !FFT_CUSTOM_ROTATE_ADD_HELPERS
    vuint16m1_t swap_idx16 = __riscv_vle16_v_u16m1(swap_re_im_idx16, vl8);
    vint16m1_t minus_j_sign = __riscv_vle16_v_i16m1(minus_j_sign16, vl8);
#endif
    for (int block = 0; block < 256; ++block) {
        const int off = block * 8;
        vint16m1_t x0 = __riscv_vle16_v_i16m1(src + 0 * 2048 + off, vl8);
        vint16m1_t x1 = __riscv_vle16_v_i16m1(src + 1 * 2048 + off, vl8);
        vint16m1_t x2 = __riscv_vle16_v_i16m1(src + 2 * 2048 + off, vl8);
        vint16m1_t x3 = __riscv_vle16_v_i16m1(src + 3 * 2048 + off, vl8);
#ifdef FFT_CUSTOM_PACKED_CMUL_HELPERS
        vint16m1_t tw_re0 = __riscv_vle16_v_i16m1(ext4096_tw_fwd[0] + off, vl8);
        vint16m1_t tw_re1 = __riscv_vle16_v_i16m1(ext4096_tw_fwd[1] + off, vl8);
        vint16m1_t tw_re2 = __riscv_vle16_v_i16m1(ext4096_tw_fwd[2] + off, vl8);
#else
        vint16m1_t tw_re0 = __riscv_vle16_v_i16m1(rvv4096_tw_re_dup[0] + off, vl8);
        vint16m1_t tw_im0 = __riscv_vle16_v_i16m1(rvv4096_tw_im_fwd[0] + off, vl8);
        vint16m1_t tw_re1 = __riscv_vle16_v_i16m1(rvv4096_tw_re_dup[1] + off, vl8);
        vint16m1_t tw_im1 = __riscv_vle16_v_i16m1(rvv4096_tw_im_fwd[1] + off, vl8);
        vint16m1_t tw_re2 = __riscv_vle16_v_i16m1(rvv4096_tw_re_dup[2] + off, vl8);
        vint16m1_t tw_im2 = __riscv_vle16_v_i16m1(rvv4096_tw_im_fwd[2] + off, vl8);
#endif

        packed_cmul3_q15_i16m1(&x1, &x2, &x3,
                               tw_re0, tw_im0,
                               tw_re1, tw_im1,
                               tw_re2, tw_im2,
                               swap_idx16, vl8);

        vint16m1_t x02 = __riscv_vsadd_vv_i16m1(x0, x2, vl8);
        vint16m1_t x13 = __riscv_vsadd_vv_i16m1(x1, x3, vl8);
        vint16m1_t y0 = rvv_shift_right_i16m1(__riscv_vsadd_vv_i16m1(x02, x13, vl8), shift, vl8);
        vint16m1_t y2 = rvv_shift_right_i16m1(__riscv_vssub_vv_i16m1(x02, x13, vl8), shift, vl8);
        vint16m1_t x02d = __riscv_vssub_vv_i16m1(x0, x2, vl8);
#if FFT_CUSTOM_ROTATE_ADD_COMBINE4096
        vint16m1_t y1_raw;
        vint16m1_t y3_raw;
        radix4_forward_rot_pair_i16m1(x02d, x1, x3, vl8, &y1_raw, &y3_raw);
        vint16m1_t y1 = rvv_shift_right_i16m1(y1_raw, shift, vl8);
        vint16m1_t y3 = rvv_shift_right_i16m1(y3_raw, shift, vl8);
#else
        vint16m1_t x1mj =
            complex_mul_minus_j_with_idx_sign_i16m1(x1, swap_idx16, minus_j_sign, vl8);
        vint16m1_t x3mj =
            complex_mul_minus_j_with_idx_sign_i16m1(x3, swap_idx16, minus_j_sign, vl8);
        vint16m1_t x13d = __riscv_vssub_vv_i16m1(x1mj, x3mj, vl8);
        vint16m1_t y1 = rvv_shift_right_i16m1(__riscv_vsadd_vv_i16m1(x02d, x13d, vl8), shift, vl8);
        vint16m1_t y3 = rvv_shift_right_i16m1(__riscv_vssub_vv_i16m1(x02d, x13d, vl8), shift, vl8);
#endif

        __riscv_vse16_v_i16m1(dst + 0 * 2048 + off, y0, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 2048 + off, y1, vl8);
        __riscv_vse16_v_i16m1(dst + 2 * 2048 + off, y2, vl8);
        __riscv_vse16_v_i16m1(dst + 3 * 2048 + off, y3, vl8);
    }
}

static void rvv_combine_radix4_inverse_4096_rvv(const int16_t *src, int16_t *dst, unsigned shift)
{
    const size_t vl8 = 8;
#if !FFT_CUSTOM_ROTATE_ADD_HELPERS
    vuint16m1_t swap_idx16 = __riscv_vle16_v_u16m1(swap_re_im_idx16, vl8);
    vint16m1_t minus_j_sign = __riscv_vle16_v_i16m1(minus_j_sign16, vl8);
#endif
    for (int block = 0; block < 256; ++block) {
        const int off = block * 8;
        vint16m1_t x0 = __riscv_vle16_v_i16m1(src + 0 * 2048 + off, vl8);
        vint16m1_t x1 = __riscv_vle16_v_i16m1(src + 1 * 2048 + off, vl8);
        vint16m1_t x2 = __riscv_vle16_v_i16m1(src + 2 * 2048 + off, vl8);
        vint16m1_t x3 = __riscv_vle16_v_i16m1(src + 3 * 2048 + off, vl8);
#ifdef FFT_CUSTOM_PACKED_CMUL_HELPERS
        vint16m1_t tw_re0 = __riscv_vle16_v_i16m1(ext4096_tw_inv[0] + off, vl8);
        vint16m1_t tw_re1 = __riscv_vle16_v_i16m1(ext4096_tw_inv[1] + off, vl8);
        vint16m1_t tw_re2 = __riscv_vle16_v_i16m1(ext4096_tw_inv[2] + off, vl8);
#else
        vint16m1_t tw_re0 = __riscv_vle16_v_i16m1(rvv4096_tw_re_dup[0] + off, vl8);
        vint16m1_t tw_im0 = __riscv_vle16_v_i16m1(rvv4096_tw_im_inv[0] + off, vl8);
        vint16m1_t tw_re1 = __riscv_vle16_v_i16m1(rvv4096_tw_re_dup[1] + off, vl8);
        vint16m1_t tw_im1 = __riscv_vle16_v_i16m1(rvv4096_tw_im_inv[1] + off, vl8);
        vint16m1_t tw_re2 = __riscv_vle16_v_i16m1(rvv4096_tw_re_dup[2] + off, vl8);
        vint16m1_t tw_im2 = __riscv_vle16_v_i16m1(rvv4096_tw_im_inv[2] + off, vl8);
#endif

        packed_cmul3_q15_i16m1(&x1, &x2, &x3,
                               tw_re0, tw_im0,
                               tw_re1, tw_im1,
                               tw_re2, tw_im2,
                               swap_idx16, vl8);

        vint16m1_t x02 = __riscv_vsadd_vv_i16m1(x0, x2, vl8);
        vint16m1_t x13 = __riscv_vsadd_vv_i16m1(x1, x3, vl8);
        vint16m1_t y0 = rvv_shift_right_i16m1(__riscv_vsadd_vv_i16m1(x02, x13, vl8), shift, vl8);
        vint16m1_t y2 = rvv_shift_right_i16m1(__riscv_vssub_vv_i16m1(x02, x13, vl8), shift, vl8);
        vint16m1_t x02d = __riscv_vssub_vv_i16m1(x0, x2, vl8);
#if FFT_CUSTOM_ROTATE_ADD_COMBINE4096
        vint16m1_t y1_raw;
        vint16m1_t y3_raw;
        radix4_inverse_rot_pair_i16m1(x02d, x1, x3, vl8, &y1_raw, &y3_raw);
        vint16m1_t y1 = rvv_shift_right_i16m1(y1_raw, shift, vl8);
        vint16m1_t y3 = rvv_shift_right_i16m1(y3_raw, shift, vl8);
#else
        vint16m1_t x1mj =
            complex_mul_minus_j_with_idx_sign_i16m1(x1, swap_idx16, minus_j_sign, vl8);
        vint16m1_t x3mj =
            complex_mul_minus_j_with_idx_sign_i16m1(x3, swap_idx16, minus_j_sign, vl8);
        vint16m1_t x13d = __riscv_vssub_vv_i16m1(x1mj, x3mj, vl8);
        vint16m1_t y3 = rvv_shift_right_i16m1(__riscv_vsadd_vv_i16m1(x02d, x13d, vl8), shift, vl8);
        vint16m1_t y1 = rvv_shift_right_i16m1(__riscv_vssub_vv_i16m1(x02d, x13d, vl8), shift, vl8);
#endif

        __riscv_vse16_v_i16m1(dst + 0 * 2048 + off, y0, vl8);
        __riscv_vse16_v_i16m1(dst + 1 * 2048 + off, y1, vl8);
        __riscv_vse16_v_i16m1(dst + 2 * 2048 + off, y2, vl8);
        __riscv_vse16_v_i16m1(dst + 3 * 2048 + off, y3, vl8);
    }
}

static void __attribute__((noinline)) dft64_rvv(int16_t *x, int16_t *y, unsigned char scale)
{
    rvv_split_radix4_64_rvv(x, g_rvv64_split_tmp);
    dft16_forward_leaf_i16m1(g_rvv64_split_tmp + 0 * 32, g_rvv64_leaf_tmp + 0 * 32, 1);
    dft16_forward_leaf_i16m1(g_rvv64_split_tmp + 1 * 32, g_rvv64_leaf_tmp + 1 * 32, 0);
    dft16_forward_leaf_i16m1(g_rvv64_split_tmp + 2 * 32, g_rvv64_leaf_tmp + 2 * 32, 0);
    dft16_forward_leaf_i16m1(g_rvv64_split_tmp + 3 * 32, g_rvv64_leaf_tmp + 3 * 32, 0);
    rvv_combine_radix4_forward_64_rvv(g_rvv64_leaf_tmp, y, scale ? 3u : 0u);
}

static void __attribute__((noinline)) idft64_rvv(int16_t *x, int16_t *y, unsigned char scale)
{
    rvv_split_radix4_64_rvv(x, g_rvv64_split_tmp);
    dft16_inverse_leaf_i16m1(g_rvv64_split_tmp + 0 * 32, g_rvv64_leaf_tmp + 0 * 32, 1);
    dft16_inverse_leaf_i16m1(g_rvv64_split_tmp + 1 * 32, g_rvv64_leaf_tmp + 1 * 32, 0);
    dft16_inverse_leaf_i16m1(g_rvv64_split_tmp + 2 * 32, g_rvv64_leaf_tmp + 2 * 32, 0);
    dft16_inverse_leaf_i16m1(g_rvv64_split_tmp + 3 * 32, g_rvv64_leaf_tmp + 3 * 32, 0);
    rvv_combine_radix4_inverse_64_rvv(g_rvv64_leaf_tmp, y, scale ? 3u : 0u);
}

static void dft64_from_leaf16_rvv_e16(int16_t *x, int16_t *y, unsigned char scale)
{
    dft16_forward_leaf_i16m1(x + 0 * 32, g_rvv64_leaf_tmp + 0 * 32, 0);
    dft16_forward_leaf_i16m1(x + 1 * 32, g_rvv64_leaf_tmp + 1 * 32, 0);
    dft16_forward_leaf_i16m1(x + 2 * 32, g_rvv64_leaf_tmp + 2 * 32, 0);
    dft16_forward_leaf_i16m1(x + 3 * 32, g_rvv64_leaf_tmp + 3 * 32, 0);
    rvv_combine_radix4_forward_64_rvv(g_rvv64_leaf_tmp, y, scale ? 3u : 0u);
}

static void __attribute__((noinline)) dft64_from_leaf16_rvv(int16_t *x, int16_t *y,
                                                            unsigned char scale)
{
    dft16_forward_leaf_i16m1(x + 0 * 32, g_rvv64_leaf_tmp + 0 * 32, 1);
    dft16_forward_leaf_i16m1(x + 1 * 32, g_rvv64_leaf_tmp + 1 * 32, 0);
    dft16_forward_leaf_i16m1(x + 2 * 32, g_rvv64_leaf_tmp + 2 * 32, 0);
    dft16_forward_leaf_i16m1(x + 3 * 32, g_rvv64_leaf_tmp + 3 * 32, 0);
    rvv_combine_radix4_forward_64_rvv(g_rvv64_leaf_tmp, y, scale ? 3u : 0u);
}

static void idft64_from_leaf16_rvv_e16(int16_t *x, int16_t *y, unsigned char scale)
{
    dft16_inverse_leaf_i16m1(x + 0 * 32, g_rvv64_leaf_tmp + 0 * 32, 0);
    dft16_inverse_leaf_i16m1(x + 1 * 32, g_rvv64_leaf_tmp + 1 * 32, 0);
    dft16_inverse_leaf_i16m1(x + 2 * 32, g_rvv64_leaf_tmp + 2 * 32, 0);
    dft16_inverse_leaf_i16m1(x + 3 * 32, g_rvv64_leaf_tmp + 3 * 32, 0);
    rvv_combine_radix4_inverse_64_rvv(g_rvv64_leaf_tmp, y, scale ? 3u : 0u);
}

static void __attribute__((noinline)) idft64_from_leaf16_rvv(int16_t *x, int16_t *y,
                                                             unsigned char scale)
{
    dft16_inverse_leaf_i16m1(x + 0 * 32, g_rvv64_leaf_tmp + 0 * 32, 1);
    dft16_inverse_leaf_i16m1(x + 1 * 32, g_rvv64_leaf_tmp + 1 * 32, 0);
    dft16_inverse_leaf_i16m1(x + 2 * 32, g_rvv64_leaf_tmp + 2 * 32, 0);
    dft16_inverse_leaf_i16m1(x + 3 * 32, g_rvv64_leaf_tmp + 3 * 32, 0);
    rvv_combine_radix4_inverse_64_rvv(g_rvv64_leaf_tmp, y, scale ? 3u : 0u);
}

#if FFT_RVV_PARTIAL_LEAF_RESIDENT_N256
#if FFT_CUSTOM_PACKED_CMUL_HELPERS || FFT_CUSTOM_ROTATE_ADD_HELPERS
#error FFT_RVV_PARTIAL_LEAF_RESIDENT_N256 is a pure-RVV candidate
#endif

static inline __attribute__((always_inline))
void rvv_partial_forward_leaf(const int16_t *src,
                              vuint16m1_t swap_idx16,
                              vint16m1_t minus_j_sign,
                              vint16m1_t *y0,
                              vint16m1_t *y1,
                              vint16m1_t *y2,
                              vint16m1_t *y3)
{
    const size_t vl8 = 8;
    vint16m1_t x0 = __riscv_vle16_v_i16m1(src + 0, vl8);
    vint16m1_t x1 = __riscv_vle16_v_i16m1(src + 8, vl8);
    vint16m1_t x2 = __riscv_vle16_v_i16m1(src + 16, vl8);
    vint16m1_t x3 = __riscv_vle16_v_i16m1(src + 24, vl8);

    dft16_forward_kernel_i16m1(x0, x1, x2, x3,
                               swap_idx16, minus_j_sign,
                               y0, y1, y2, y3);
}

static inline __attribute__((always_inline))
void rvv_partial_inverse_leaf(const int16_t *src,
                              vuint16m1_t swap_idx16,
                              vint16m1_t minus_j_sign,
                              vint16m1_t *y0,
                              vint16m1_t *y1,
                              vint16m1_t *y2,
                              vint16m1_t *y3)
{
    const size_t vl8 = 8;
    vint16m1_t x0 = __riscv_vle16_v_i16m1(src + 0, vl8);
    vint16m1_t x1 = __riscv_vle16_v_i16m1(src + 8, vl8);
    vint16m1_t x2 = __riscv_vle16_v_i16m1(src + 16, vl8);
    vint16m1_t x3 = __riscv_vle16_v_i16m1(src + 24, vl8);

    dft16_inverse_kernel_i16m1(x0, x1, x2, x3,
                               swap_idx16, minus_j_sign,
                               y0, y1, y2, y3);
}

static inline __attribute__((always_inline))
void rvv_partial_forward_block(vint16m1_t x0,
                               vint16m1_t x1,
                               vint16m1_t x2,
                               vint16m1_t x3,
                               int off,
                               int16_t *dst,
                               vuint16m1_t swap_idx16,
                               vint16m1_t minus_j_sign)
{
    const size_t vl8 = 8;
    vint16m1_t y0, y1, y2, y3;

    rvv_combine_radix4_forward_64_block_i16m1(
        x0, x1, x2, x3, off, 3u, swap_idx16, minus_j_sign,
        &y0, &y1, &y2, &y3);
    __riscv_vse16_v_i16m1(dst + 0 * 32 + off, y0, vl8);
    __riscv_vse16_v_i16m1(dst + 1 * 32 + off, y1, vl8);
    __riscv_vse16_v_i16m1(dst + 2 * 32 + off, y2, vl8);
    __riscv_vse16_v_i16m1(dst + 3 * 32 + off, y3, vl8);
}

static inline __attribute__((always_inline))
void rvv_partial_inverse_block(vint16m1_t x0,
                               vint16m1_t x1,
                               vint16m1_t x2,
                               vint16m1_t x3,
                               int off,
                               int16_t *dst,
                               vuint16m1_t swap_idx16,
                               vint16m1_t minus_j_sign)
{
    const size_t vl8 = 8;
    vint16m1_t y0, y1, y2, y3;

    rvv_combine_radix4_inverse_64_block_i16m1(
        x0, x1, x2, x3, off, 3u, swap_idx16, minus_j_sign,
        &y0, &y1, &y2, &y3);
    __riscv_vse16_v_i16m1(dst + 0 * 32 + off, y0, vl8);
    __riscv_vse16_v_i16m1(dst + 1 * 32 + off, y1, vl8);
    __riscv_vse16_v_i16m1(dst + 2 * 32 + off, y2, vl8);
    __riscv_vse16_v_i16m1(dst + 3 * 32 + off, y3, vl8);
}

static void __attribute__((noinline))
rvv_partial_dft64_forward(const int16_t *src, int16_t *dst)
{
    const size_t vl8 = 8;
    vuint16m1_t swap_idx16 = __riscv_vle16_v_u16m1(swap_re_im_idx16, vl8);
    vint16m1_t minus_j_sign = __riscv_vle16_v_i16m1(minus_j_sign16, vl8);
    vint16m1_t l0_0, l0_1, l0_2, l0_3;
    vint16m1_t l1_0, l1_1, l1_2, l1_3;
    vint16m1_t l2_0, l2_1, l2_2, l2_3;
    vint16m1_t l3_0, l3_1, l3_2, l3_3;

    rvv_partial_forward_leaf(src + 0 * 32, swap_idx16, minus_j_sign,
                             &l0_0, &l0_1, &l0_2, &l0_3);
    __riscv_vse16_v_i16m1(g_rvv64_leaf_tmp + 0 * 32 + 16, l0_2, vl8);
    __riscv_vse16_v_i16m1(g_rvv64_leaf_tmp + 0 * 32 + 24, l0_3, vl8);
    rvv_partial_forward_leaf(src + 1 * 32, swap_idx16, minus_j_sign,
                             &l1_0, &l1_1, &l1_2, &l1_3);
    __riscv_vse16_v_i16m1(g_rvv64_leaf_tmp + 1 * 32 + 16, l1_2, vl8);
    __riscv_vse16_v_i16m1(g_rvv64_leaf_tmp + 1 * 32 + 24, l1_3, vl8);
    rvv_partial_forward_leaf(src + 2 * 32, swap_idx16, minus_j_sign,
                             &l2_0, &l2_1, &l2_2, &l2_3);
    __riscv_vse16_v_i16m1(g_rvv64_leaf_tmp + 2 * 32 + 16, l2_2, vl8);
    __riscv_vse16_v_i16m1(g_rvv64_leaf_tmp + 2 * 32 + 24, l2_3, vl8);
    rvv_partial_forward_leaf(src + 3 * 32, swap_idx16, minus_j_sign,
                             &l3_0, &l3_1, &l3_2, &l3_3);
    __riscv_vse16_v_i16m1(g_rvv64_leaf_tmp + 3 * 32 + 16, l3_2, vl8);
    __riscv_vse16_v_i16m1(g_rvv64_leaf_tmp + 3 * 32 + 24, l3_3, vl8);

    rvv_partial_forward_block(l0_0, l1_0, l2_0, l3_0,
                              0, dst, swap_idx16, minus_j_sign);
    rvv_partial_forward_block(l0_1, l1_1, l2_1, l3_1,
                              8, dst, swap_idx16, minus_j_sign);
    rvv_partial_forward_block(
        __riscv_vle16_v_i16m1(g_rvv64_leaf_tmp + 0 * 32 + 16, vl8),
        __riscv_vle16_v_i16m1(g_rvv64_leaf_tmp + 1 * 32 + 16, vl8),
        __riscv_vle16_v_i16m1(g_rvv64_leaf_tmp + 2 * 32 + 16, vl8),
        __riscv_vle16_v_i16m1(g_rvv64_leaf_tmp + 3 * 32 + 16, vl8),
        16, dst, swap_idx16, minus_j_sign);
    rvv_partial_forward_block(
        __riscv_vle16_v_i16m1(g_rvv64_leaf_tmp + 0 * 32 + 24, vl8),
        __riscv_vle16_v_i16m1(g_rvv64_leaf_tmp + 1 * 32 + 24, vl8),
        __riscv_vle16_v_i16m1(g_rvv64_leaf_tmp + 2 * 32 + 24, vl8),
        __riscv_vle16_v_i16m1(g_rvv64_leaf_tmp + 3 * 32 + 24, vl8),
        24, dst, swap_idx16, minus_j_sign);
}

static void __attribute__((noinline))
rvv_partial_dft64_inverse(const int16_t *src, int16_t *dst)
{
    const size_t vl8 = 8;
    vuint16m1_t swap_idx16 = __riscv_vle16_v_u16m1(swap_re_im_idx16, vl8);
    vint16m1_t minus_j_sign = __riscv_vle16_v_i16m1(minus_j_sign16, vl8);
    vint16m1_t l0_0, l0_1, l0_2, l0_3;
    vint16m1_t l1_0, l1_1, l1_2, l1_3;
    vint16m1_t l2_0, l2_1, l2_2, l2_3;
    vint16m1_t l3_0, l3_1, l3_2, l3_3;

    rvv_partial_inverse_leaf(src + 0 * 32, swap_idx16, minus_j_sign,
                             &l0_0, &l0_1, &l0_2, &l0_3);
    __riscv_vse16_v_i16m1(g_rvv64_leaf_tmp + 0 * 32 + 16, l0_2, vl8);
    __riscv_vse16_v_i16m1(g_rvv64_leaf_tmp + 0 * 32 + 24, l0_3, vl8);
    rvv_partial_inverse_leaf(src + 1 * 32, swap_idx16, minus_j_sign,
                             &l1_0, &l1_1, &l1_2, &l1_3);
    __riscv_vse16_v_i16m1(g_rvv64_leaf_tmp + 1 * 32 + 16, l1_2, vl8);
    __riscv_vse16_v_i16m1(g_rvv64_leaf_tmp + 1 * 32 + 24, l1_3, vl8);
    rvv_partial_inverse_leaf(src + 2 * 32, swap_idx16, minus_j_sign,
                             &l2_0, &l2_1, &l2_2, &l2_3);
    __riscv_vse16_v_i16m1(g_rvv64_leaf_tmp + 2 * 32 + 16, l2_2, vl8);
    __riscv_vse16_v_i16m1(g_rvv64_leaf_tmp + 2 * 32 + 24, l2_3, vl8);
    rvv_partial_inverse_leaf(src + 3 * 32, swap_idx16, minus_j_sign,
                             &l3_0, &l3_1, &l3_2, &l3_3);
    __riscv_vse16_v_i16m1(g_rvv64_leaf_tmp + 3 * 32 + 16, l3_2, vl8);
    __riscv_vse16_v_i16m1(g_rvv64_leaf_tmp + 3 * 32 + 24, l3_3, vl8);

    rvv_partial_inverse_block(l0_0, l1_0, l2_0, l3_0,
                              0, dst, swap_idx16, minus_j_sign);
    rvv_partial_inverse_block(l0_1, l1_1, l2_1, l3_1,
                              8, dst, swap_idx16, minus_j_sign);
    rvv_partial_inverse_block(
        __riscv_vle16_v_i16m1(g_rvv64_leaf_tmp + 0 * 32 + 16, vl8),
        __riscv_vle16_v_i16m1(g_rvv64_leaf_tmp + 1 * 32 + 16, vl8),
        __riscv_vle16_v_i16m1(g_rvv64_leaf_tmp + 2 * 32 + 16, vl8),
        __riscv_vle16_v_i16m1(g_rvv64_leaf_tmp + 3 * 32 + 16, vl8),
        16, dst, swap_idx16, minus_j_sign);
    rvv_partial_inverse_block(
        __riscv_vle16_v_i16m1(g_rvv64_leaf_tmp + 0 * 32 + 24, vl8),
        __riscv_vle16_v_i16m1(g_rvv64_leaf_tmp + 1 * 32 + 24, vl8),
        __riscv_vle16_v_i16m1(g_rvv64_leaf_tmp + 2 * 32 + 24, vl8),
        __riscv_vle16_v_i16m1(g_rvv64_leaf_tmp + 3 * 32 + 24, vl8),
        24, dst, swap_idx16, minus_j_sign);
}

static void rvv_partial_dft256_forward(const int16_t *x, int16_t *y)
{
    for (int child = 0; child < 4; ++child) {
        rvv_partial_dft64_forward(x + child * 128, y + child * 128);
    }
    rvv_combine_radix4_forward_256_rvv(y, y, 0u);
}

static void rvv_partial_dft256_inverse(const int16_t *x, int16_t *y)
{
    for (int child = 0; child < 4; ++child) {
        rvv_partial_dft64_inverse(x + child * 128, y + child * 128);
    }
    rvv_combine_radix4_inverse_256_rvv(y, y, 0u);
}
#endif

static void __attribute__((noinline)) dft128_from_leaf16_rvv(int16_t *x, int16_t *y,
                                                             unsigned char scale)
{
    dft64_from_leaf16_rvv(x + 0 * 128, y + 0 * 128, 1);
    dft64_from_leaf16_rvv_e16(x + 1 * 128, y + 1 * 128, 1);
    rvv_combine_radix2_forward_128_rvv(y, y, scale ? 1u : 0u);
}

static void __attribute__((noinline)) idft128_from_leaf16_rvv(int16_t *x, int16_t *y,
                                                              unsigned char scale)
{
    idft64_from_leaf16_rvv(x + 0 * 128, y + 0 * 128, 1);
    idft64_from_leaf16_rvv_e16(x + 1 * 128, y + 1 * 128, 1);
    rvv_combine_radix2_inverse_128_rvv(y, y, scale ? 1u : 0u);
}

static void dft256_from_leaf16_rvv_e16(int16_t *x, int16_t *y, unsigned char scale)
{
    dft64_from_leaf16_rvv_e16(x + 0 * 128, y + 0 * 128, 1);
    dft64_from_leaf16_rvv_e16(x + 1 * 128, y + 1 * 128, 1);
    dft64_from_leaf16_rvv_e16(x + 2 * 128, y + 2 * 128, 1);
    dft64_from_leaf16_rvv_e16(x + 3 * 128, y + 3 * 128, 1);
    rvv_combine_radix4_forward_256_rvv(y, y, scale ? 1u : 0u);
}

static void __attribute__((noinline)) dft256_from_leaf16_rvv(int16_t *x, int16_t *y,
                                                             unsigned char scale)
{
    dft64_from_leaf16_rvv(x + 0 * 128, y + 0 * 128, 1);
    dft64_from_leaf16_rvv_e16(x + 1 * 128, y + 1 * 128, 1);
    dft64_from_leaf16_rvv_e16(x + 2 * 128, y + 2 * 128, 1);
    dft64_from_leaf16_rvv_e16(x + 3 * 128, y + 3 * 128, 1);
    rvv_combine_radix4_forward_256_rvv(y, y, scale ? 1u : 0u);
}

static void idft256_from_leaf16_rvv_e16(int16_t *x, int16_t *y, unsigned char scale)
{
    idft64_from_leaf16_rvv_e16(x + 0 * 128, y + 0 * 128, 1);
    idft64_from_leaf16_rvv_e16(x + 1 * 128, y + 1 * 128, 1);
    idft64_from_leaf16_rvv_e16(x + 2 * 128, y + 2 * 128, 1);
    idft64_from_leaf16_rvv_e16(x + 3 * 128, y + 3 * 128, 1);
    rvv_combine_radix4_inverse_256_rvv(y, y, scale ? 1u : 0u);
}

static void __attribute__((noinline)) idft256_from_leaf16_rvv(int16_t *x, int16_t *y,
                                                              unsigned char scale)
{
    idft64_from_leaf16_rvv(x + 0 * 128, y + 0 * 128, 1);
    idft64_from_leaf16_rvv_e16(x + 1 * 128, y + 1 * 128, 1);
    idft64_from_leaf16_rvv_e16(x + 2 * 128, y + 2 * 128, 1);
    idft64_from_leaf16_rvv_e16(x + 3 * 128, y + 3 * 128, 1);
    rvv_combine_radix4_inverse_256_rvv(y, y, scale ? 1u : 0u);
}

static void __attribute__((noinline)) dft128_rvv(int16_t *x, int16_t *y, unsigned char scale)
{
    rvv_split_radix2_128_to_leaf16_rvv(x, g_rvv128_split_tmp);
    dft128_from_leaf16_rvv(g_rvv128_split_tmp, y, scale);
}

static void __attribute__((noinline)) idft128_rvv(int16_t *x, int16_t *y, unsigned char scale)
{
    rvv_split_radix2_128_to_leaf16_rvv(x, g_rvv128_split_tmp);
    idft128_from_leaf16_rvv(g_rvv128_split_tmp, y, scale);
}

static void __attribute__((noinline)) dft256_rvv(int16_t *x, int16_t *y, unsigned char scale)
{
    rvv_split_radix4_256_to_leaf16_rvv(x, g_rvv256_split_tmp);
    dft256_from_leaf16_rvv(g_rvv256_split_tmp, y, scale);
}

static void __attribute__((noinline)) idft256_rvv(int16_t *x, int16_t *y, unsigned char scale)
{
    rvv_split_radix4_256_to_leaf16_rvv(x, g_rvv256_split_tmp);
    idft256_from_leaf16_rvv(g_rvv256_split_tmp, y, scale);
}

static void __attribute__((noinline)) dft512_rvv(int16_t *x, int16_t *y, unsigned char scale)
{
    rvv_split_radix2_512_to_leaf16_rvv(x, g_rvv512_split_tmp);
    dft256_from_leaf16_rvv(g_rvv512_split_tmp + 0 * 512, g_rvv512_split_tmp + 0 * 512, 1);
    dft256_from_leaf16_rvv_e16(g_rvv512_split_tmp + 1 * 512, g_rvv512_split_tmp + 1 * 512, 1);
    rvv_combine_radix2_forward_512_rvv(g_rvv512_split_tmp, y, scale ? 1u : 0u);
}

static void __attribute__((noinline)) idft512_rvv(int16_t *x, int16_t *y, unsigned char scale)
{
    rvv_split_radix2_512_to_leaf16_rvv(x, g_rvv512_split_tmp);
    idft256_from_leaf16_rvv(g_rvv512_split_tmp + 0 * 512, g_rvv512_split_tmp + 0 * 512, 1);
    idft256_from_leaf16_rvv_e16(g_rvv512_split_tmp + 1 * 512, g_rvv512_split_tmp + 1 * 512, 1);
    rvv_combine_radix2_inverse_512_rvv(g_rvv512_split_tmp, y, scale ? 1u : 0u);
}

static void dft1024_from_leaf16_rvv_e16(int16_t *x, int16_t *y, unsigned char scale)
{
    dft256_from_leaf16_rvv_e16(x + 0 * 512, y + 0 * 512, 1);
    dft256_from_leaf16_rvv_e16(x + 1 * 512, y + 1 * 512, 1);
    dft256_from_leaf16_rvv_e16(x + 2 * 512, y + 2 * 512, 1);
    dft256_from_leaf16_rvv_e16(x + 3 * 512, y + 3 * 512, 1);
    rvv_combine_radix4_forward_1024_rvv(y, y, scale ? 1u : 0u);
}

static void __attribute__((noinline)) dft1024_from_leaf16_rvv(int16_t *x, int16_t *y,
                                                              unsigned char scale)
{
    dft256_from_leaf16_rvv(x + 0 * 512, y + 0 * 512, 1);
    dft256_from_leaf16_rvv_e16(x + 1 * 512, y + 1 * 512, 1);
    dft256_from_leaf16_rvv_e16(x + 2 * 512, y + 2 * 512, 1);
    dft256_from_leaf16_rvv_e16(x + 3 * 512, y + 3 * 512, 1);
    rvv_combine_radix4_forward_1024_rvv(y, y, scale ? 1u : 0u);
}

static void idft1024_from_leaf16_rvv_e16(int16_t *x, int16_t *y, unsigned char scale)
{
    idft256_from_leaf16_rvv_e16(x + 0 * 512, y + 0 * 512, 1);
    idft256_from_leaf16_rvv_e16(x + 1 * 512, y + 1 * 512, 1);
    idft256_from_leaf16_rvv_e16(x + 2 * 512, y + 2 * 512, 1);
    idft256_from_leaf16_rvv_e16(x + 3 * 512, y + 3 * 512, 1);
    rvv_combine_radix4_inverse_1024_rvv(y, y, scale ? 1u : 0u);
}

static void __attribute__((noinline)) idft1024_from_leaf16_rvv(int16_t *x, int16_t *y,
                                                               unsigned char scale)
{
    idft256_from_leaf16_rvv(x + 0 * 512, y + 0 * 512, 1);
    idft256_from_leaf16_rvv_e16(x + 1 * 512, y + 1 * 512, 1);
    idft256_from_leaf16_rvv_e16(x + 2 * 512, y + 2 * 512, 1);
    idft256_from_leaf16_rvv_e16(x + 3 * 512, y + 3 * 512, 1);
    rvv_combine_radix4_inverse_1024_rvv(y, y, scale ? 1u : 0u);
}

static void __attribute__((noinline)) dft2048_from_leaf16_rvv(int16_t *x, int16_t *y,
                                                              unsigned char scale)
{
    dft1024_from_leaf16_rvv(x + 0 * 2048, y + 0 * 2048, 1);
    dft1024_from_leaf16_rvv_e16(x + 1 * 2048, y + 1 * 2048, 1);
    rvv_combine_radix2_forward_2048_rvv(y, y, scale ? 1u : 0u);
}

static void __attribute__((noinline)) idft2048_from_leaf16_rvv(int16_t *x, int16_t *y,
                                                               unsigned char scale)
{
    idft1024_from_leaf16_rvv(x + 0 * 2048, y + 0 * 2048, 1);
    idft1024_from_leaf16_rvv_e16(x + 1 * 2048, y + 1 * 2048, 1);
    rvv_combine_radix2_inverse_2048_rvv(y, y, scale ? 1u : 0u);
}

static void __attribute__((noinline)) dft4096_from_leaf16_rvv(int16_t *x, int16_t *y,
                                                              unsigned char scale)
{
    dft1024_from_leaf16_rvv(x + 0 * 2048, y + 0 * 2048, 1);
    dft1024_from_leaf16_rvv_e16(x + 1 * 2048, y + 1 * 2048, 1);
    dft1024_from_leaf16_rvv_e16(x + 2 * 2048, y + 2 * 2048, 1);
    dft1024_from_leaf16_rvv_e16(x + 3 * 2048, y + 3 * 2048, 1);
    rvv_combine_radix4_forward_4096_rvv(y, y, scale ? 1u : 0u);
}

static void __attribute__((noinline)) idft4096_from_leaf16_rvv(int16_t *x, int16_t *y,
                                                               unsigned char scale)
{
    idft1024_from_leaf16_rvv(x + 0 * 2048, y + 0 * 2048, 1);
    idft1024_from_leaf16_rvv_e16(x + 1 * 2048, y + 1 * 2048, 1);
    idft1024_from_leaf16_rvv_e16(x + 2 * 2048, y + 2 * 2048, 1);
    idft1024_from_leaf16_rvv_e16(x + 3 * 2048, y + 3 * 2048, 1);
    rvv_combine_radix4_inverse_4096_rvv(y, y, scale ? 1u : 0u);
}

static void __attribute__((noinline)) dft1024_rvv(int16_t *x, int16_t *y, unsigned char scale)
{
    rvv_split_radix4_1024_to_leaf16_rvv(x, g_rvv1024_split_tmp);
    dft1024_from_leaf16_rvv(g_rvv1024_split_tmp, y, scale);
}

static void __attribute__((noinline)) idft1024_rvv(int16_t *x, int16_t *y, unsigned char scale)
{
    rvv_split_radix4_1024_to_leaf16_rvv(x, g_rvv1024_split_tmp);
    idft1024_from_leaf16_rvv(g_rvv1024_split_tmp, y, scale);
}

static void __attribute__((noinline)) dft2048_rvv(int16_t *x, int16_t *y, unsigned char scale)
{
    rvv_split_radix2_2048_to_leaf16_rvv(x, g_rvv2048_split_tmp);
    dft2048_from_leaf16_rvv(g_rvv2048_split_tmp, y, scale);
}

static void __attribute__((noinline)) idft2048_rvv(int16_t *x, int16_t *y, unsigned char scale)
{
    rvv_split_radix2_2048_to_leaf16_rvv(x, g_rvv2048_split_tmp);
    idft2048_from_leaf16_rvv(g_rvv2048_split_tmp, y, scale);
}

static void __attribute__((noinline)) dft4096_rvv(int16_t *x, int16_t *y, unsigned char scale)
{
    rvv_split_radix4_4096_to_leaf16_rvv(x, g_rvv4096_split_tmp);
    dft4096_from_leaf16_rvv(g_rvv4096_split_tmp, y, scale);
}

static void __attribute__((noinline)) idft4096_rvv(int16_t *x, int16_t *y, unsigned char scale)
{
    rvv_split_radix4_4096_to_leaf16_rvv(x, g_rvv4096_split_tmp);
    idft4096_from_leaf16_rvv(g_rvv4096_split_tmp, y, scale);
}

#define FFT_POW2_BASE_DFT16 dft16_rvv
#define FFT_POW2_BASE_IDFT16 idft16_rvv
#include "fft_pow2_scalar_impl.h"
#undef FFT_POW2_BASE_DFT16
#undef FFT_POW2_BASE_IDFT16

static int rvv_run_forward(int points, int16_t *x, int16_t *y)
{
    if (points == 64) {
        dft64_rvv(x, y, 0);
        return 1;
    }
    if (points == 128) {
        dft128_rvv(x, y, 0);
        return 1;
    }
    if (points == 256) {
        dft256_rvv(x, y, 0);
        return 1;
    }
    if (points == 512) {
        dft512_rvv(x, y, 0);
        return 1;
    }
    if (points == 1024) {
        dft1024_rvv(x, y, 0);
        return 1;
    }
    if (points == 2048) {
        dft2048_rvv(x, y, 0);
        return 1;
    }
    if (points == 4096) {
        dft4096_rvv(x, y, 0);
        return 1;
    }

    return fft_pow2_run_forward(points, x, y);
}

static int rvv_run_inverse(int points, int16_t *x, int16_t *y)
{
    if (points == 64) {
        idft64_rvv(x, y, 0);
        return 1;
    }
    if (points == 128) {
        idft128_rvv(x, y, 0);
        return 1;
    }
    if (points == 256) {
        idft256_rvv(x, y, 0);
        return 1;
    }
    if (points == 512) {
        idft512_rvv(x, y, 0);
        return 1;
    }
    if (points == 1024) {
        idft1024_rvv(x, y, 0);
        return 1;
    }
    if (points == 2048) {
        idft2048_rvv(x, y, 0);
        return 1;
    }
    if (points == 4096) {
        idft4096_rvv(x, y, 0);
        return 1;
    }

    return fft_pow2_run_inverse(points, x, y);
}

static int rvv_run_forward_contract(int points,
                                    int16_t *logical_x,
                                    int16_t *native_x,
                                    int16_t *y)
{
#if FFT_PRELAYOUT_N64
    if (points == 64) {
#if FFT_EXT_FUSED_LEAF_COMBINE_N64
        fft_ext_fused_dft64_forward(native_x, y, 0u);
#else
        dft64_from_leaf16_rvv(native_x, y, 0);
#endif
        return 1;
    }
#endif
#if FFT_PRELAYOUT_N128
    if (points == 128) {
#if FFT_EXT_FUSED_LEAF_COMBINE_N128
        fft_ext_dft128_fused_forward(native_x, y);
#else
        dft128_from_leaf16_rvv(native_x, y, 0);
#endif
        return 1;
    }
#endif
#if FFT_PRELAYOUT_N256
    if (points == 256) {
#if FFT_RVV_PARTIAL_LEAF_RESIDENT_N256
        rvv_partial_dft256_forward(native_x, y);
#elif FFT_EXT_FUSED_LEAF_COMBINE_N256
        fft_ext_dft256_fused_forward(native_x, y, 0u);
#else
        dft256_from_leaf16_rvv(native_x, y, 0);
#endif
        return 1;
    }
#else
    (void)native_x;
#endif
#if FFT_PRELAYOUT_N512
    if (points == 512) {
#if FFT_EXT_FUSED_LEAF_COMBINE_N512
        fft_ext_dft512_fused_forward(native_x, y);
#else
        dft256_from_leaf16_rvv(native_x + 0 * 512, y + 0 * 512, 1);
        dft256_from_leaf16_rvv_e16(native_x + 1 * 512, y + 1 * 512, 1);
        rvv_combine_radix2_forward_512_rvv(y, y, 0);
#endif
        return 1;
    }
#endif
#if FFT_PRELAYOUT_N1024
    if (points == 1024) {
#if FFT_EXT_FUSED_LEAF_COMBINE_N1024
        fft_ext_dft1024_fused_forward(native_x, y, 0u);
#else
        dft256_from_leaf16_rvv(native_x + 0 * 512, y + 0 * 512, 1);
        dft256_from_leaf16_rvv_e16(native_x + 1 * 512, y + 1 * 512, 1);
        dft256_from_leaf16_rvv_e16(native_x + 2 * 512, y + 2 * 512, 1);
        dft256_from_leaf16_rvv_e16(native_x + 3 * 512, y + 3 * 512, 1);
        rvv_combine_radix4_forward_1024_rvv(y, y, 0);
#endif
        return 1;
    }
#endif
#if FFT_PRELAYOUT_N2048
    if (points == 2048) {
#if FFT_EXT_FUSED_LEAF_COMBINE_N2048
        fft_ext_dft2048_fused_forward(native_x, y);
#else
        dft1024_from_leaf16_rvv(native_x + 0 * 2048, y + 0 * 2048, 1);
        dft1024_from_leaf16_rvv_e16(native_x + 1 * 2048, y + 1 * 2048, 1);
        rvv_combine_radix2_forward_2048_rvv(y, y, 0);
#endif
        return 1;
    }
#endif
#if FFT_PRELAYOUT_N4096
    if (points == 4096) {
#if FFT_EXT_FUSED_LEAF_COMBINE_N4096
        fft_ext_dft4096_fused_forward(native_x, y);
#else
        dft1024_from_leaf16_rvv(native_x + 0 * 2048, y + 0 * 2048, 1);
        dft1024_from_leaf16_rvv_e16(native_x + 1 * 2048, y + 1 * 2048, 1);
        dft1024_from_leaf16_rvv_e16(native_x + 2 * 2048, y + 2 * 2048, 1);
        dft1024_from_leaf16_rvv_e16(native_x + 3 * 2048, y + 3 * 2048, 1);
        rvv_combine_radix4_forward_4096_rvv(y, y, 0);
#endif
        return 1;
    }
#endif
    return rvv_run_forward(points, logical_x, y);
}

static int rvv_run_inverse_contract(int points,
                                    int16_t *logical_x,
                                    int16_t *native_x,
                                    int16_t *y)
{
#if FFT_PRELAYOUT_N64
    if (points == 64) {
#if FFT_EXT_FUSED_LEAF_COMBINE_N64
        fft_ext_fused_dft64_inverse(native_x, y, 0u);
#else
        idft64_from_leaf16_rvv(native_x, y, 0);
#endif
        return 1;
    }
#endif
#if FFT_PRELAYOUT_N128
    if (points == 128) {
#if FFT_EXT_FUSED_LEAF_COMBINE_N128
        fft_ext_dft128_fused_inverse(native_x, y);
#else
        idft128_from_leaf16_rvv(native_x, y, 0);
#endif
        return 1;
    }
#endif
#if FFT_PRELAYOUT_N256
    if (points == 256) {
#if FFT_RVV_PARTIAL_LEAF_RESIDENT_N256
        rvv_partial_dft256_inverse(native_x, y);
#elif FFT_EXT_FUSED_LEAF_COMBINE_N256
        fft_ext_dft256_fused_inverse(native_x, y, 0u);
#else
        idft256_from_leaf16_rvv(native_x, y, 0);
#endif
        return 1;
    }
#else
    (void)native_x;
#endif
#if FFT_PRELAYOUT_N512
    if (points == 512) {
#if FFT_EXT_FUSED_LEAF_COMBINE_N512
        fft_ext_dft512_fused_inverse(native_x, y);
#else
        idft256_from_leaf16_rvv(native_x + 0 * 512, y + 0 * 512, 1);
        idft256_from_leaf16_rvv_e16(native_x + 1 * 512, y + 1 * 512, 1);
        rvv_combine_radix2_inverse_512_rvv(y, y, 0);
#endif
        return 1;
    }
#endif
#if FFT_PRELAYOUT_N1024
    if (points == 1024) {
#if FFT_EXT_FUSED_LEAF_COMBINE_N1024
        fft_ext_dft1024_fused_inverse(native_x, y, 0u);
#else
        idft256_from_leaf16_rvv(native_x + 0 * 512, y + 0 * 512, 1);
        idft256_from_leaf16_rvv_e16(native_x + 1 * 512, y + 1 * 512, 1);
        idft256_from_leaf16_rvv_e16(native_x + 2 * 512, y + 2 * 512, 1);
        idft256_from_leaf16_rvv_e16(native_x + 3 * 512, y + 3 * 512, 1);
        rvv_combine_radix4_inverse_1024_rvv(y, y, 0);
#endif
        return 1;
    }
#endif
#if FFT_PRELAYOUT_N2048
    if (points == 2048) {
#if FFT_EXT_FUSED_LEAF_COMBINE_N2048
        fft_ext_dft2048_fused_inverse(native_x, y);
#else
        idft1024_from_leaf16_rvv(native_x + 0 * 2048, y + 0 * 2048, 1);
        idft1024_from_leaf16_rvv_e16(native_x + 1 * 2048, y + 1 * 2048, 1);
        rvv_combine_radix2_inverse_2048_rvv(y, y, 0);
#endif
        return 1;
    }
#endif
#if FFT_PRELAYOUT_N4096
    if (points == 4096) {
#if FFT_EXT_FUSED_LEAF_COMBINE_N4096
        fft_ext_dft4096_fused_inverse(native_x, y);
#else
        idft1024_from_leaf16_rvv(native_x + 0 * 2048, y + 0 * 2048, 1);
        idft1024_from_leaf16_rvv_e16(native_x + 1 * 2048, y + 1 * 2048, 1);
        idft1024_from_leaf16_rvv_e16(native_x + 2 * 2048, y + 2 * 2048, 1);
        idft1024_from_leaf16_rvv_e16(native_x + 3 * 2048, y + 3 * 2048, 1);
        rvv_combine_radix4_inverse_4096_rvv(y, y, 0);
#endif
        return 1;
    }
#endif
    return rvv_run_inverse(points, logical_x, y);
}

#if FFT_VALIDATE && FFT_CUSTOM_ROTATE_ADD_ANY
static int run_fast_rotpair_demo(void)
{
    const size_t vl8 = 8;
    static const int16_t base_data[8] __attribute__((aligned(16))) = {
        100, -200, 300, 400, -500, 600, 700, -800
    };
    static const int16_t x1_data[8] __attribute__((aligned(16))) = {
        11, 22, -33, 44, 55, -66, 77, 88
    };
    static const int16_t x3_data[8] __attribute__((aligned(16))) = {
        -9, 5, 13, -17, -21, 25, 31, -35
    };
    int16_t ref_y1[8] __attribute__((aligned(16)));
    int16_t ref_y3[8] __attribute__((aligned(16)));
    int16_t dut_y1[8] __attribute__((aligned(16)));
    int16_t dut_y3[8] __attribute__((aligned(16)));
    vuint16m1_t swap_idx16 = __riscv_vle16_v_u16m1(swap_re_im_idx16, vl8);
    vint16m1_t minus_j_sign = __riscv_vle16_v_i16m1(minus_j_sign16, vl8);
    vint16m1_t base = __riscv_vle16_v_i16m1(base_data, vl8);
    vint16m1_t x1 = __riscv_vle16_v_i16m1(x1_data, vl8);
    vint16m1_t x3 = __riscv_vle16_v_i16m1(x3_data, vl8);
    vint16m1_t x1_mj = complex_mul_minus_j_with_idx_sign_i16m1(x1, swap_idx16, minus_j_sign, vl8);
    vint16m1_t x3_mj = complex_mul_minus_j_with_idx_sign_i16m1(x3, swap_idx16, minus_j_sign, vl8);
    vint16m1_t x13 = __riscv_vssub_vv_i16m1(x1_mj, x3_mj, vl8);
    vint16m1_t ref1 = __riscv_vsadd_vv_i16m1(base, x13, vl8);
    vint16m1_t ref3 = __riscv_vssub_vv_i16m1(base, x13, vl8);
    vint16m1_t dut1;
    vint16m1_t dut3;

    __riscv_vse16_v_i16m1(ref_y1, ref1, vl8);
    __riscv_vse16_v_i16m1(ref_y3, ref3, vl8);

    FFT_EXT_SET_E16M1_VL8();
    radix4_forward_rot_pair_i16m1(base, x1, x3, vl8, &dut1, &dut3);
    __riscv_vse16_v_i16m1(dut_y1, dut1, vl8);
    __riscv_vse16_v_i16m1(dut_y3, dut3, vl8);

    if (!fft_compare_complex_vector(ref_y1, dut_y1, 4) ||
        !fft_compare_complex_vector(ref_y3, dut_y3, 4)) {
        printf("%s rotpair demo: FAIL\n", FFT_IMPL_DISPLAY_NAME);
        fft_print_complex_vector("rotpair ref y1", ref_y1, 4);
        fft_print_complex_vector("rotpair dut y1", dut_y1, 4);
        fft_print_complex_vector("rotpair ref y3", ref_y3, 4);
        fft_print_complex_vector("rotpair dut y3", dut_y3, 4);
        return 0;
    }

    printf("%s rotpair demo: PASS\n", FFT_IMPL_DISPLAY_NAME);
    return 1;
}

static int run_fast_rotpair_clobber_demo(void)
{
    const size_t vl8 = 8;
    static const int16_t delta_data[8] __attribute__((aligned(16))) = {
        20, 17, -46, 61, 76, -91, 46, 123
    };
    static const int16_t base_data[8] __attribute__((aligned(16))) = {
        100, -200, 300, 400, -500, 600, 700, -800
    };
    static const int16_t keep_v2_data[8] __attribute__((aligned(16))) = {
        1, 2, 3, 4, 5, 6, 7, 8
    };
    static const int16_t keep_v4_data[8] __attribute__((aligned(16))) = {
        -11, 12, -13, 14, -15, 16, -17, 18
    };
    vint16m1_t delta = __riscv_vle16_v_i16m1(delta_data, vl8);
    vint16m1_t base = __riscv_vle16_v_i16m1(base_data, vl8);
    vint16m1_t keep_v2 = __riscv_vle16_v_i16m1(keep_v2_data, vl8);
    vint16m1_t keep_v4 = __riscv_vle16_v_i16m1(keep_v4_data, vl8);
    vint16m1_t out_v2;
    vint16m1_t out_v4;
    int16_t dut_v2[8] __attribute__((aligned(16)));
    int16_t dut_v4[8] __attribute__((aligned(16)));

    FFT_EXT_SET_E16M1_VL8();
    asm volatile(
        "vmv1r.v v2, %2\n\t"
        "vmv1r.v v4, %3\n\t"
        "vmv1r.v v20, %4\n\t"
        "vmv1r.v v24, %5\n\t"
        "vmv1r.v v21, %4\n\t"
        "vmv1r.v v25, %5\n\t"
        ".word 0xE74C0857\n\t"
        ".word 0xE35C88D7\n\t"
        "vmv1r.v %0, v2\n\t"
        "vmv1r.v %1, v4\n\t"
        : "=vr"(out_v2), "=vr"(out_v4)
        : "vr"(keep_v2), "vr"(keep_v4), "vr"(delta), "vr"(base)
        : "v16", "v17", "v20", "v21", "v24", "v25", "v2", "v4", "memory"
    );

    __riscv_vse16_v_i16m1(dut_v2, out_v2, vl8);
    __riscv_vse16_v_i16m1(dut_v4, out_v4, vl8);

    if (!fft_compare_complex_vector(keep_v2_data, dut_v2, 4) ||
        !fft_compare_complex_vector(keep_v4_data, dut_v4, 4)) {
        printf("%s rotpair clobber demo: FAIL\n", FFT_IMPL_DISPLAY_NAME);
        fft_print_complex_vector("clobber ref v2", keep_v2_data, 4);
        fft_print_complex_vector("clobber dut v2", dut_v2, 4);
        fft_print_complex_vector("clobber ref v4", keep_v4_data, 4);
        fft_print_complex_vector("clobber dut v4", dut_v4, 4);
        return 0;
    }

    printf("%s rotpair clobber demo: PASS\n", FFT_IMPL_DISPLAY_NAME);
    return 1;
}
#endif

extern int oai_fft_dft_reference_points(int points, int16_t *x, int16_t *y);
extern int oai_fft_idft_reference_points(int points, int16_t *x, int16_t *y);

int main(void)
{
    int16_t *x = g_fft_main_input;
    int16_t *y_rvv = g_fft_main_output;
    int16_t *native_x = x;

    // GCC may lower the fixed-size input copy into RVV instructions, so the
    // vector unit must be enabled before any seemingly scalar setup work.
    rvv_runtime_init();

    if (!fft_pow2_supported_points(FFT_MAIN_POINTS)) {
        printf("Unsupported FFT_MAIN_POINTS=%d in %s\n",
               FFT_MAIN_POINTS,
               FFT_IMPL_SOURCE_NAME);
        return 1;
    }

    if (!fft_load_test_input(x, FFT_MAIN_POINTS)) {
        printf("Failed to load FFT test input for %d points\n", FFT_MAIN_POINTS);
        return 1;
    }

#if FFT_PRELAYOUT_N64
    if (FFT_MAIN_POINTS == 64) {
        rvv_split_radix4_64_rvv(x, g_rvv64_split_tmp);
        native_x = g_rvv64_split_tmp;
    }
#endif

#if FFT_PRELAYOUT_N128
    if (FFT_MAIN_POINTS == 128) {
        rvv_split_radix2_128_to_leaf16_rvv(x, g_rvv128_split_tmp);
        native_x = g_rvv128_split_tmp;
    }
#endif

#if FFT_PRELAYOUT_N256
    if (FFT_MAIN_POINTS == 256) {
        rvv_split_radix4_256_to_leaf16_rvv(x, g_rvv256_split_tmp);
        native_x = g_rvv256_split_tmp;
    }
#endif

#if FFT_PRELAYOUT_N512
    if (FFT_MAIN_POINTS == 512) {
        rvv_split_radix2_512_to_leaf16_rvv(x, g_rvv512_split_tmp);
        native_x = g_rvv512_split_tmp;
    }
#endif

#if FFT_PRELAYOUT_N1024
    if (FFT_MAIN_POINTS == 1024) {
        rvv_split_radix4_1024_to_leaf16_rvv(x, g_rvv1024_split_tmp);
        native_x = g_rvv1024_split_tmp;
    }
#endif

#if FFT_PRELAYOUT_N2048
    if (FFT_MAIN_POINTS == 2048) {
        rvv_split_radix2_2048_to_leaf16_rvv(x, g_rvv2048_split_tmp);
        native_x = g_rvv2048_split_tmp;
    }
#endif

#if FFT_PRELAYOUT_N4096
    if (FFT_MAIN_POINTS == 4096) {
        rvv_split_radix4_4096_to_leaf16_rvv(x, g_rvv4096_split_tmp);
        native_x = g_rvv4096_split_tmp;
    }
#endif

#if FFT_VALIDATE
    if (!FFT_IMPL_VALIDATE_PRECHECK()) {
        return 1;
    }
#if FFT_CUSTOM_ROTATE_ADD_ANY
    if (!run_fast_rotpair_demo()) {
        return 1;
    }
    if (!run_fast_rotpair_clobber_demo()) {
        return 1;
    }
#endif

    int16_t *y_ref = g_fft_main_ref;

    for (int i = 0; i < FFT_MAIN_REPEAT; ++i) {
        const int ok = FFT_MAIN_RUN_IDFT
                           ? oai_fft_idft_reference_points(FFT_MAIN_POINTS, x, y_ref)
                           : oai_fft_dft_reference_points(FFT_MAIN_POINTS, x, y_ref);
        if (!ok) {
            printf("Reference path does not support points=%d idft=%d\n",
                   FFT_MAIN_POINTS, FFT_MAIN_RUN_IDFT);
            return 1;
        }
    }

    for (int i = 0; i < FFT_MAIN_REPEAT; ++i) {
        const int ok = FFT_MAIN_RUN_IDFT
#if FFT_NATIVE_LAYOUT_CONTRACT
                           ? rvv_run_inverse_contract(FFT_MAIN_POINTS, x, native_x, y_rvv)
                           : rvv_run_forward_contract(FFT_MAIN_POINTS, x, native_x, y_rvv);
#else
                           ? rvv_run_inverse(FFT_MAIN_POINTS, x, y_rvv)
                           : rvv_run_forward(FFT_MAIN_POINTS, x, y_rvv);
#endif
        if (!ok) {
            printf("%s dispatch failed for %d points\n",
                   FFT_IMPL_DISPLAY_NAME,
                   FFT_MAIN_POINTS);
            return 1;
        }
    }

    if (FFT_MAIN_POINTS <= 64) {
        fft_print_complex_vector("Input", x, FFT_MAIN_POINTS);
        fft_print_complex_vector(FFT_MAIN_RUN_IDFT ? "OAI reference IDFT" : "OAI reference DFT",
                                 y_ref,
                                 FFT_MAIN_POINTS);
        char rvv_label[80];
        if (FFT_MAIN_POINTS == 64) {
            snprintf(rvv_label, sizeof(rvv_label), "%s %s result [pure64 + stride leaf]",
                     FFT_IMPL_DISPLAY_NAME,
                     FFT_MAIN_RUN_IDFT ? "idft" : "dft");
        } else {
            snprintf(rvv_label, sizeof(rvv_label), "%s %s result [stride leaf]",
                     FFT_IMPL_DISPLAY_NAME,
                     FFT_MAIN_RUN_IDFT ? "idft" : "dft");
        }
        fft_print_complex_vector(rvv_label, y_rvv, FFT_MAIN_POINTS);
    } else {
        printf("Vector dump omitted for %d points in validate mode\n", FFT_MAIN_POINTS);
    }

    if (fft_compare_complex_vector(y_ref, y_rvv, FFT_MAIN_POINTS)) {
        printf("CHECK: PASS\n");
        return 0;
    }

    printf("CHECK: FAIL\n");
    return 1;
#else
    int ok = 1;

    for (int i = 0; i < FFT_MAIN_WARMUP_REPEAT; ++i) {
        ok = FFT_MAIN_RUN_IDFT
#if FFT_NATIVE_LAYOUT_CONTRACT
                 ? rvv_run_inverse_contract(FFT_MAIN_POINTS, x, native_x, y_rvv)
                 : rvv_run_forward_contract(FFT_MAIN_POINTS, x, native_x, y_rvv);
#else
                 ? rvv_run_inverse(FFT_MAIN_POINTS, x, y_rvv)
                 : rvv_run_forward(FFT_MAIN_POINTS, x, y_rvv);
#endif
        if (!ok) {
            break;
        }
    }

    if (!ok) {
        printf("%s dispatch failed during warmup for %d points\n",
               FFT_IMPL_DISPLAY_NAME,
               FFT_MAIN_POINTS);
        return 1;
    }

    fft_roi_stats_t roi_stats;
    fft_roi_prepare(&roi_stats);
    const fft_roi_stamp_t roi_start = fft_roi_begin(&roi_stats);

    for (int i = 0; i < FFT_MAIN_REPEAT; ++i) {
        ok = FFT_MAIN_RUN_IDFT
#if FFT_NATIVE_LAYOUT_CONTRACT
                 ? rvv_run_inverse_contract(FFT_MAIN_POINTS, x, native_x, y_rvv)
                 : rvv_run_forward_contract(FFT_MAIN_POINTS, x, native_x, y_rvv);
#else
                 ? rvv_run_inverse(FFT_MAIN_POINTS, x, y_rvv)
                 : rvv_run_forward(FFT_MAIN_POINTS, x, y_rvv);
#endif
        if (!ok) {
            break;
        }
    }

    fft_roi_end(&roi_stats, roi_start, FFT_MAIN_REPEAT);
    if (!ok) {
        printf("%s dispatch failed for %d points\n",
               FFT_IMPL_DISPLAY_NAME,
               FFT_MAIN_POINTS);
        return 1;
    }

    fft_print_steady_state_config(FFT_MAIN_WARMUP_REPEAT, FFT_MAIN_REPEAT);
    fft_print_roi_summary(&roi_stats);
    g_fft_perf_sink = (int16_t)(y_rvv[0] ^ y_rvv[1]);
    return (g_fft_perf_sink == (int16_t)0x7fff) ? 1 : 0;
#endif
}
