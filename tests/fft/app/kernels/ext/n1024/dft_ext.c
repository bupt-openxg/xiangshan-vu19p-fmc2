#include "fft_test_common.h"
#include "fft_pow2_ext_twiddles_precomputed.h"
#include "fft_pow2_ext_twiddles_4096_precomputed.h"
#include "klib.h"
#include <stddef.h>
#include <stdint.h>
#include <riscv_vector.h>

#ifndef FFT_MAIN_POINTS
#define FFT_MAIN_POINTS 16
#endif

#ifndef FFT_MAINLINE_EXT
#define FFT_MAINLINE_EXT 1
#endif

#include "fft_mainline_config.h"

#ifndef FFT_VALIDATE
#define FFT_VALIDATE 1
#endif

#ifndef read_csr
#define read_csr(csr)       ({ unsigned long __v; __asm__ volatile ("csrr %0, " #csr : "=r" (__v)); __v; })
#endif

#ifndef write_csr
#define write_csr(csr, val) ({ unsigned long __v = (unsigned long)(val); __asm__ volatile ("csrw " #csr ", %0" :: "r" (__v)); })
#endif

#ifndef FFT_CUSTOM_PACKED_CMUL_HELPERS
#define FFT_CUSTOM_PACKED_CMUL_HELPERS 1
#endif

#ifndef FFT_CUSTOM_ROTATE_ADD_HELPERS
#define FFT_CUSTOM_ROTATE_ADD_HELPERS 1
#endif

#ifndef FFT_EXT_ROT_PAIR_REUSE_SOURCES
#define FFT_EXT_ROT_PAIR_REUSE_SOURCES 0
#endif

#ifndef FFT_EXT_LEAF_TWIDDLE_RESIDENT
#define FFT_EXT_LEAF_TWIDDLE_RESIDENT 0
#endif

#ifndef FFT_EXT_FULL_LEAF_RESIDENT
#define FFT_EXT_FULL_LEAF_RESIDENT 0
#endif

#ifndef FFT_EXT_LMUL2_RADIX4_N1024
#define FFT_EXT_LMUL2_RADIX4_N1024 0
#endif

#ifndef FFT_EXT_LMUL2_RADIX4_N256_SHIFT1_N1024
#define FFT_EXT_LMUL2_RADIX4_N256_SHIFT1_N1024 0
#endif

#ifndef FFT_IMPL_DISPLAY_NAME
#define FFT_IMPL_DISPLAY_NAME "EXT"
#endif

#ifndef FFT_IMPL_SOURCE_NAME
#define FFT_IMPL_SOURCE_NAME "dft_ext.c"
#endif

#ifndef FFT_IMPL_VALIDATE_PRECHECK
#define FFT_IMPL_VALIDATE_PRECHECK() run_ext_instruction_demo()
#endif

#ifndef FFT_EXT_PACKED_VL
#define FFT_EXT_PACKED_VL 8
#endif

// Maintained EXT mainline policy:
// - this file is the only maintained ext mainline implementation
// - packed complex multiply and rotate-add helpers are both enabled here
// - no parallel ext build flavors are maintained in-tree

static inline __attribute__((always_inline)) void ext_set_e16m1_vl8(void)
{
    asm volatile("vsetivli zero, 8, e16, m1, ta, ma" ::: "memory");
}

static inline __attribute__((always_inline))
vint16m1_t ext_cmul_i16m1(vint16m1_t a, vint16m1_t b)
{
    register vint16m1_t ra asm("v20") = a;
    register vint16m1_t rb asm("v24") = b;
    register vint16m1_t rd asm("v16");
    asm volatile(
        ".word 0xCB4C0857\n\t"
        : "=vr"(rd)
        : "vr"(ra), "vr"(rb)
        : "memory"
    );
    return rd;
}

static inline __attribute__((always_inline))
vint16m1_t ext_cmul_conj_vs2_i16m1(vint16m1_t vs2_conj_src, vint16m1_t vs1)
{
    register vint16m1_t ra asm("v20") = vs2_conj_src;
    register vint16m1_t rb asm("v24") = vs1;
    register vint16m1_t rd asm("v16");
    asm volatile(
        ".word 0xCF4C0857\n\t"
        : "=vr"(rd)
        : "vr"(ra), "vr"(rb)
        : "memory"
    );
    return rd;
}

static inline __attribute__((always_inline))
vint16m1_t ext_cmulacc_i16m1(vint16m1_t acc_vd, vint16m1_t vs2, vint16m1_t vs1)
{
    register vint16m1_t ra asm("v20") = vs2;
    register vint16m1_t rb asm("v24") = vs1;
    register vint16m1_t rd asm("v16") = acc_vd;
    asm volatile(
        ".word 0xD34C0857\n\t"
        : "+vr"(rd)
        : "vr"(ra), "vr"(rb)
        : "memory"
    );
    return rd;
}

static inline __attribute__((always_inline))
vint16m1_t ext_cmulacc_conj_vs2_i16m1(vint16m1_t acc_vd,
                                      vint16m1_t vs2_conj_src,
                                      vint16m1_t vs1)
{
    register vint16m1_t ra asm("v20") = vs2_conj_src;
    register vint16m1_t rb asm("v24") = vs1;
    register vint16m1_t rd asm("v16") = acc_vd;
    asm volatile(
        ".word 0xDB4C0857\n\t"
        : "+vr"(rd)
        : "vr"(ra), "vr"(rb)
        : "memory"
    );
    return rd;
}

static inline __attribute__((always_inline))
vint16m1_t ext_addconj_i16m1(vint16m1_t vs2_conj_src, vint16m1_t vs1_addend)
{
    register vint16m1_t ra asm("v20") = vs2_conj_src;
    register vint16m1_t rb asm("v24") = vs1_addend;
    register vint16m1_t rd asm("v16");
    asm volatile(
        ".word 0xDF4C0857\n\t"
        : "=vr"(rd)
        : "vr"(ra), "vr"(rb)
        : "memory"
    );
    return rd;
}

static inline __attribute__((always_inline))
vint16m1_t ext_addrq_i16m1(vint16m1_t vs2_rot_src, vint16m1_t vs1_addend)
{
    register vint16m1_t ra asm("v20") = vs2_rot_src;
    register vint16m1_t rb asm("v24") = vs1_addend;
    register vint16m1_t rd asm("v16");
    asm volatile(
        ".word 0xE34C0857\n\t"
        : "=vr"(rd)
        : "vr"(ra), "vr"(rb)
        : "memory"
    );
    return rd;
}

static inline __attribute__((always_inline))
vint16m1_t ext_addrtq_i16m1(vint16m1_t vs2_rot_src, vint16m1_t vs1_addend)
{
    register vint16m1_t ra asm("v20") = vs2_rot_src;
    register vint16m1_t rb asm("v24") = vs1_addend;
    register vint16m1_t rd asm("v16");
    asm volatile(
        ".word 0xE74C0857\n\t"
        : "=vr"(rd)
        : "vr"(ra), "vr"(rb)
        : "memory"
    );
    return rd;
}

#if FFT_EXT_INDIVIDUAL_ROTATE_DFT16 || FFT_EXT_INDIVIDUAL_ROTATE_COMBINE64
static inline __attribute__((always_inline))
void ext_rotate_minus_j_pair_i16m1(vint16m1_t x0,
                                   vint16m1_t x1,
                                   vint16m1_t zero,
                                   vint16m1_t *r0,
                                   vint16m1_t *r1)
{
    register vint16m1_t in0 asm("v20") = x0;
    register vint16m1_t in1 asm("v21") = x1;
    register vint16m1_t zero_reg asm("v24") = zero;
    register vint16m1_t out0 asm("v16");
    register vint16m1_t out1 asm("v17");

    asm volatile(
        ".word 0xE74C0857\n\t"
        ".word 0xE75C08D7\n\t"
        : "=vr"(out0), "=vr"(out1)
        : "vr"(in0), "vr"(in1), "vr"(zero_reg)
        : "memory"
    );

    *r0 = out0;
    *r1 = out1;
}
#endif

static inline __attribute__((always_inline))
void ext_rot_pair_forward_i16m1(vint16m1_t delta13,
                                vint16m1_t base,
                                vint16m1_t *y1,
                                vint16m1_t *y3)
{
#if FFT_EXT_ROT_PAIR_REUSE_SOURCES
    register vint16m1_t ra asm("v20") = delta13;
    register vint16m1_t rb asm("v24") = base;
    register vint16m1_t out_y1 asm("v16");
    register vint16m1_t out_y3 asm("v17");

    asm volatile(
        ".word 0xE74C0857\n\t"
        ".word 0xE34C08D7\n\t"
        : "=vr"(out_y1), "=vr"(out_y3)
        : "vr"(ra), "vr"(rb)
        : "memory"
    );
#else
    register vint16m1_t ra0 asm("v20") = delta13;
    register vint16m1_t rb0 asm("v24") = base;
    register vint16m1_t ra1 asm("v21") = delta13;
    register vint16m1_t rb1 asm("v25") = base;
    register vint16m1_t out_y1 asm("v16");
    register vint16m1_t out_y3 asm("v17");

    asm volatile(
        ".word 0xE74C0857\n\t"
        ".word 0xE35C88D7\n\t"
        : "=vr"(out_y1), "=vr"(out_y3)
        : "vr"(ra0), "vr"(rb0), "vr"(ra1), "vr"(rb1)
        : "memory"
    );
#endif

    *y1 = out_y1;
    *y3 = out_y3;
}

static inline __attribute__((always_inline))
void ext_rot_pair_inverse_i16m1(vint16m1_t delta13,
                                vint16m1_t base,
                                vint16m1_t *y1,
                                vint16m1_t *y3)
{
#if FFT_EXT_ROT_PAIR_REUSE_SOURCES
    register vint16m1_t ra asm("v20") = delta13;
    register vint16m1_t rb asm("v24") = base;
    register vint16m1_t out_y1 asm("v16");
    register vint16m1_t out_y3 asm("v17");

    asm volatile(
        ".word 0xE34C0857\n\t"
        ".word 0xE74C08D7\n\t"
        : "=vr"(out_y1), "=vr"(out_y3)
        : "vr"(ra), "vr"(rb)
        : "memory"
    );
#else
    register vint16m1_t ra0 asm("v20") = delta13;
    register vint16m1_t rb0 asm("v24") = base;
    register vint16m1_t ra1 asm("v21") = delta13;
    register vint16m1_t rb1 asm("v25") = base;
    register vint16m1_t out_y1 asm("v16");
    register vint16m1_t out_y3 asm("v17");

    asm volatile(
        ".word 0xE34C0857\n\t"
        ".word 0xE75C88D7\n\t"
        : "=vr"(out_y1), "=vr"(out_y3)
        : "vr"(ra0), "vr"(rb0), "vr"(ra1), "vr"(rb1)
        : "memory"
    );
#endif

    *y1 = out_y1;
    *y3 = out_y3;
}

#if FFT_EXT_LMUL2_RADIX4_N1024 || FFT_EXT_LMUL2_RADIX4_N256_SHIFT1_N1024
/* Two adjacent radix-4 blocks share one e16,m2 group per logical vector. */
static inline __attribute__((always_inline))
void ext_packed_cmul3_i16m2(vint16m2_t *a0,
                            vint16m2_t *a1,
                            vint16m2_t *a2,
                            vint16m2_t tw0,
                            vint16m2_t tw1,
                            vint16m2_t tw2)
{
    register vint16m2_t in0 asm("v20") = *a0;
    register vint16m2_t in1 asm("v22") = *a1;
    register vint16m2_t in2 asm("v24") = *a2;
    register vint16m2_t tw0_reg asm("v26") = tw0;
    register vint16m2_t tw1_reg asm("v28") = tw1;
    register vint16m2_t tw2_reg asm("v30") = tw2;
    register vint16m2_t out0 asm("v8");
    register vint16m2_t out1 asm("v10");
    register vint16m2_t out2 asm("v12");

    asm volatile(
        ".word 0xcb4d0457\n\t"
        ".word 0xcb6e0557\n\t"
        ".word 0xcb8f0657\n\t"
        : "=vr"(out0), "=vr"(out1), "=vr"(out2)
        : "vr"(in0), "vr"(tw0_reg),
          "vr"(in1), "vr"(tw1_reg),
          "vr"(in2), "vr"(tw2_reg)
        : "memory"
    );

    *a0 = out0;
    *a1 = out1;
    *a2 = out2;
}

static inline __attribute__((always_inline))
void ext_rot_pair_forward_i16m2(vint16m2_t delta13,
                                vint16m2_t base,
                                vint16m2_t *y1,
                                vint16m2_t *y3)
{
    register vint16m2_t delta_reg asm("v20") = delta13;
    register vint16m2_t base_reg asm("v24") = base;
    register vint16m2_t out_y1 asm("v16");
    register vint16m2_t out_y3 asm("v18");

    asm volatile(
        ".word 0xe74c0857\n\t"
        ".word 0xe34c0957\n\t"
        : "=vr"(out_y1), "=vr"(out_y3)
        : "vr"(delta_reg), "vr"(base_reg)
        : "memory"
    );

    *y1 = out_y1;
    *y3 = out_y3;
}

static inline __attribute__((always_inline))
void ext_rot_pair_inverse_i16m2(vint16m2_t delta13,
                                vint16m2_t base,
                                vint16m2_t *y1,
                                vint16m2_t *y3)
{
    register vint16m2_t delta_reg asm("v20") = delta13;
    register vint16m2_t base_reg asm("v24") = base;
    register vint16m2_t out_y1 asm("v16");
    register vint16m2_t out_y3 asm("v18");

    asm volatile(
        ".word 0xe34c0857\n\t"
        ".word 0xe74c0957\n\t"
        : "=vr"(out_y1), "=vr"(out_y3)
        : "vr"(delta_reg), "vr"(base_reg)
        : "memory"
    );

    *y1 = out_y1;
    *y3 = out_y3;
}
#endif

static inline __attribute__((always_inline))
vint16m1_t ext_packed_cmul_i16m1(vint16m1_t a, vint16m1_t tw)
{
    return ext_cmul_i16m1(a, tw);
}

static inline __attribute__((always_inline))
void ext_packed_cmul3_i16m1(vint16m1_t *a0,
                            vint16m1_t *a1,
                            vint16m1_t *a2,
                            vint16m1_t tw0,
                            vint16m1_t tw1,
                            vint16m1_t tw2)
{
    register vint16m1_t in0 asm("v20") = *a0;
    register vint16m1_t tw0_reg asm("v24") = tw0;
    register vint16m1_t in1 asm("v21") = *a1;
    register vint16m1_t tw1_reg asm("v25") = tw1;
    register vint16m1_t in2 asm("v22") = *a2;
    register vint16m1_t tw2_reg asm("v26") = tw2;
    register vint16m1_t out0 asm("v8");
    register vint16m1_t out1 asm("v9");
    register vint16m1_t out2 asm("v10");

    asm volatile(
        ".word 0xCB4C0457\n\t"
        ".word 0xCB5C84D7\n\t"
        ".word 0xCB6D0557\n\t"
        : "=vr"(out0), "=vr"(out1), "=vr"(out2)
        : "vr"(in0), "vr"(tw0_reg),
          "vr"(in1), "vr"(tw1_reg),
          "vr"(in2), "vr"(tw2_reg)
        : "memory"
    );

    *a0 = out0;
    *a1 = out1;
    *a2 = out2;
}

#if FFT_EXT_LEAF_TWIDDLE_RESIDENT
static inline __attribute__((always_inline))
void ext_packed_cmul3_leaf_resident_i16m1(vint16m1_t *a0,
                                          vint16m1_t *a1,
                                          vint16m1_t *a2,
                                          vint16m1_t tw0,
                                          vint16m1_t tw1,
                                          vint16m1_t tw2)
{
    register vint16m1_t in0 asm("v20") = *a0;
    register vint16m1_t in1 asm("v21") = *a1;
    register vint16m1_t in2 asm("v22") = *a2;
    register vint16m1_t tw0_reg asm("v28") = tw0;
    register vint16m1_t tw1_reg asm("v29") = tw1;
    register vint16m1_t tw2_reg asm("v30") = tw2;
    register vint16m1_t out0 asm("v8");
    register vint16m1_t out1 asm("v9");
    register vint16m1_t out2 asm("v10");

    asm volatile(
        ".word 0xcb4e0457\n\t"
        ".word 0xcb5e84d7\n\t"
        ".word 0xcb6f0557\n\t"
        : "=vr"(out0), "=vr"(out1), "=vr"(out2)
        : "vr"(in0), "vr"(tw0_reg),
          "vr"(in1), "vr"(tw1_reg),
          "vr"(in2), "vr"(tw2_reg)
        : "memory"
    );

    *a0 = out0;
    *a1 = out1;
    *a2 = out2;
}
#endif

static inline __attribute__((always_inline))
void ext_packed_cmul4_i16m1(vint16m1_t *a0,
                            vint16m1_t *a1,
                            vint16m1_t *a2,
                            vint16m1_t *a3,
                            vint16m1_t tw0,
                            vint16m1_t tw1,
                            vint16m1_t tw2,
                            vint16m1_t tw3)
{
    register vint16m1_t in0 asm("v20") = *a0;
    register vint16m1_t tw0_reg asm("v24") = tw0;
    register vint16m1_t in1 asm("v21") = *a1;
    register vint16m1_t tw1_reg asm("v25") = tw1;
    register vint16m1_t in2 asm("v22") = *a2;
    register vint16m1_t tw2_reg asm("v26") = tw2;
    register vint16m1_t in3 asm("v23") = *a3;
    register vint16m1_t tw3_reg asm("v27") = tw3;
    register vint16m1_t out0 asm("v8");
    register vint16m1_t out1 asm("v9");
    register vint16m1_t out2 asm("v10");
    register vint16m1_t out3 asm("v11");

    asm volatile(
        ".word 0xCB4C0457\n\t"
        ".word 0xCB5C84D7\n\t"
        ".word 0xCB6D0557\n\t"
        ".word 0xCB7D85D7\n\t"
        : "=vr"(out0), "=vr"(out1), "=vr"(out2), "=vr"(out3)
        : "vr"(in0), "vr"(tw0_reg),
          "vr"(in1), "vr"(tw1_reg),
          "vr"(in2), "vr"(tw2_reg),
          "vr"(in3), "vr"(tw3_reg)
        : "memory"
    );

    *a0 = out0;
    *a1 = out1;
    *a2 = out2;
    *a3 = out3;
}

#define packed_cmul3_q15_i16m1(a0, a1, a2, tw_re0, tw_im0, tw_re1, tw_im1, tw_re2, tw_im2, swap_idx, vl) \
    ext_packed_cmul3_i16m1((a0), (a1), (a2), (tw_re0), (tw_re1), (tw_re2))

#define packed_cmul_q15_i16m1(a, tw_re, tw_im, swap_idx, vl) \
    ext_packed_cmul_i16m1((a), (tw_re))

#if FFT_VALIDATE
static inline void ext_clear_vxsat_flag(void)
{
    uintptr_t vcsr_val = read_csr(vcsr);
    write_csr(vcsr, vcsr_val & ~1UL);
}

static inline unsigned ext_read_vxsat_flag(void)
{
    return (unsigned)(read_csr(vcsr) & 1UL);
}

static inline int64_t ext_round_q15_value(int64_t value)
{
    const uintptr_t vxrm = (read_csr(vcsr) >> 1) & 0x3UL;
    const uint64_t lsb = 1ULL << 15;
    const uint64_t lsb_half = lsb >> 1;
    const uint64_t uvalue = (uint64_t)value;

    switch (vxrm) {
    case 0:
        value += (int64_t)lsb_half;
        break;
    case 1:
        if ((uvalue & lsb_half) &&
            ((uvalue & (lsb_half - 1)) || (uvalue & lsb))) {
            value += (int64_t)lsb;
        }
        break;
    case 2:
        break;
    default:
        if (uvalue & (lsb - 1)) {
            value |= (int64_t)lsb;
        }
        break;
    }

    return value;
}

static inline int16_t ext_narrow_q15_sat_i16(int64_t value, unsigned *sat)
{
    value = ext_round_q15_value(value);
    value >>= 15;
    if (value > 32767) {
        *sat = 1;
        return 32767;
    }
    if (value < -32768) {
        *sat = 1;
        return (int16_t)0x8000;
    }
    return (int16_t)value;
}

static inline int16_t ext_sat_add_i16_ref(int16_t a, int16_t b, unsigned *sat)
{
    const int32_t sum = (int32_t)a + (int32_t)b;
    if (sum > 32767) {
        *sat = 1;
        return 32767;
    }
    if (sum < -32768) {
        *sat = 1;
        return (int16_t)0x8000;
    }
    return (int16_t)sum;
}

static inline int16_t ext_sat_sub_i16_ref(int16_t a, int16_t b, unsigned *sat)
{
    const int32_t diff = (int32_t)a - (int32_t)b;
    if (diff > 32767) {
        *sat = 1;
        return 32767;
    }
    if (diff < -32768) {
        *sat = 1;
        return (int16_t)0x8000;
    }
    return (int16_t)diff;
}

static void ext_ref_cmul_core_i16(int16_t *dst,
                                  const int16_t *vs2,
                                  const int16_t *vs1,
                                  const int16_t *acc,
                                  int conj_vs2,
                                  int with_acc,
                                  unsigned *sat_any)
{
    for (int i = 0; i < 4; ++i) {
        const int16_t vs2_re = vs2[2 * i];
        const int16_t vs2_im = vs2[2 * i + 1];
        const int16_t vs1_re = vs1[2 * i];
        const int16_t vs1_im = vs1[2 * i + 1];
        int64_t temp_re;
        int64_t temp_im;
        unsigned sat_local = 0;

        if (conj_vs2) {
            temp_re = (int64_t)vs1_re * (int64_t)vs2_re +
                      (int64_t)vs1_im * (int64_t)vs2_im;
            temp_im = -(int64_t)vs1_re * (int64_t)vs2_im +
                      (int64_t)vs1_im * (int64_t)vs2_re;
        } else {
            temp_re = (int64_t)vs1_re * (int64_t)vs2_re -
                      (int64_t)vs1_im * (int64_t)vs2_im;
            temp_im = (int64_t)vs1_re * (int64_t)vs2_im +
                      (int64_t)vs1_im * (int64_t)vs2_re;
        }

        if (with_acc) {
            temp_re += (int64_t)acc[2 * i] << 15;
            temp_im += (int64_t)acc[2 * i + 1] << 15;
        }

        dst[2 * i] = ext_narrow_q15_sat_i16(temp_re, &sat_local);
        *sat_any |= sat_local;
        sat_local = 0;
        dst[2 * i + 1] = ext_narrow_q15_sat_i16(temp_im, &sat_local);
        *sat_any |= sat_local;
    }
}

static void ext_ref_addconj_i16(int16_t *dst,
                                const int16_t *vs2,
                                const int16_t *vs1,
                                unsigned *sat_any)
{
    for (int i = 0; i < 4; ++i) {
        unsigned sat_local = 0;
        dst[2 * i] = ext_sat_add_i16_ref(vs2[2 * i], vs1[2 * i], &sat_local);
        *sat_any |= sat_local;
        sat_local = 0;
        dst[2 * i + 1] =
            ext_sat_sub_i16_ref(vs1[2 * i + 1], vs2[2 * i + 1], &sat_local);
        *sat_any |= sat_local;
    }
}

static void ext_ref_addrq_i16(int16_t *dst,
                              const int16_t *vs2,
                              const int16_t *vs1,
                              unsigned *sat_any)
{
    for (int i = 0; i < 4; ++i) {
        unsigned sat_local = 0;
        dst[2 * i] =
            ext_sat_sub_i16_ref(vs1[2 * i], vs2[2 * i + 1], &sat_local);
        *sat_any |= sat_local;
        sat_local = 0;
        dst[2 * i + 1] =
            ext_sat_add_i16_ref(vs2[2 * i], vs1[2 * i + 1], &sat_local);
        *sat_any |= sat_local;
    }
}

static void ext_ref_addrtq_i16(int16_t *dst,
                               const int16_t *vs2,
                               const int16_t *vs1,
                               unsigned *sat_any)
{
    for (int i = 0; i < 4; ++i) {
        unsigned sat_local = 0;
        dst[2 * i] =
            ext_sat_add_i16_ref(vs2[2 * i + 1], vs1[2 * i], &sat_local);
        *sat_any |= sat_local;
        sat_local = 0;
        dst[2 * i + 1] =
            ext_sat_sub_i16_ref(vs1[2 * i + 1], vs2[2 * i], &sat_local);
        *sat_any |= sat_local;
    }
}

static int ext_demo_check_result(const char *name,
                                 const int16_t *ref,
                                 const int16_t *dut,
                                 unsigned ref_vxsat)
{
    int pass = fft_compare_complex_vector(ref, dut, 4);
    const unsigned dut_vxsat = ext_read_vxsat_flag();

    if (dut_vxsat != ref_vxsat) {
        printf("%s vxsat mismatch: ref=%u dut=%u\n", name, ref_vxsat, dut_vxsat);
        pass = 0;
    }

    if (!pass) {
        fft_print_complex_vector("EXT demo ref", ref, 4);
        fft_print_complex_vector("EXT demo dut", dut, 4);
    }

    printf("EXT demo %-11s: %s\n", name, pass ? "PASS" : "FAIL");
    return pass;
}

static int run_ext_instruction_demo(void)
{
    const size_t vl8 = 8;
    static const int16_t demo_vs2[8] __attribute__((aligned(16))) = {
        32767, 32767, -32768, 32767, -30000, -20000, 12345, -32768
    };
    static const int16_t demo_vs1[8] __attribute__((aligned(16))) = {
        32767, -32768, 32767, 32767, 30000, -25000, -32768, 20000
    };
    static const int16_t demo_acc[8] __attribute__((aligned(16))) = {
        1000, -1000, 32767, -32768, 20000, -25000, -12345, 23456
    };
    int16_t ref[8] __attribute__((aligned(16)));
    int16_t dut[8] __attribute__((aligned(16)));
    int pass = 1;
    unsigned ref_vxsat;

    ext_set_e16m1_vl8();

    ref_vxsat = 0;
    ext_ref_cmul_core_i16(ref, demo_vs2, demo_vs1, (const int16_t *)0, 0, 0, &ref_vxsat);
    ext_clear_vxsat_flag();
    __riscv_vse16_v_i16m1(
        dut,
        ext_cmul_i16m1(__riscv_vle16_v_i16m1(demo_vs2, vl8),
                       __riscv_vle16_v_i16m1(demo_vs1, vl8)),
        vl8);
    pass &= ext_demo_check_result("vscmul", ref, dut, ref_vxsat);

    ref_vxsat = 0;
    ext_ref_cmul_core_i16(ref, demo_vs2, demo_vs1, (const int16_t *)0, 1, 0, &ref_vxsat);
    ext_clear_vxsat_flag();
    __riscv_vse16_v_i16m1(
        dut,
        ext_cmul_conj_vs2_i16m1(__riscv_vle16_v_i16m1(demo_vs2, vl8),
                                __riscv_vle16_v_i16m1(demo_vs1, vl8)),
        vl8);
    pass &= ext_demo_check_result("vscmulcj", ref, dut, ref_vxsat);

    ref_vxsat = 0;
    ext_ref_cmul_core_i16(ref, demo_vs2, demo_vs1, demo_acc, 0, 1, &ref_vxsat);
    ext_clear_vxsat_flag();
    __riscv_vse16_v_i16m1(
        dut,
        ext_cmulacc_i16m1(__riscv_vle16_v_i16m1(demo_acc, vl8),
                          __riscv_vle16_v_i16m1(demo_vs2, vl8),
                          __riscv_vle16_v_i16m1(demo_vs1, vl8)),
        vl8);
    pass &= ext_demo_check_result("vscmacc", ref, dut, ref_vxsat);

    ref_vxsat = 0;
    ext_ref_cmul_core_i16(ref, demo_vs2, demo_vs1, demo_acc, 1, 1, &ref_vxsat);
    ext_clear_vxsat_flag();
    __riscv_vse16_v_i16m1(
        dut,
        ext_cmulacc_conj_vs2_i16m1(__riscv_vle16_v_i16m1(demo_acc, vl8),
                                   __riscv_vle16_v_i16m1(demo_vs2, vl8),
                                   __riscv_vle16_v_i16m1(demo_vs1, vl8)),
        vl8);
    pass &= ext_demo_check_result("vscmacccj", ref, dut, ref_vxsat);

    ref_vxsat = 0;
    ext_ref_addconj_i16(ref, demo_vs2, demo_vs1, &ref_vxsat);
    ext_clear_vxsat_flag();
    __riscv_vse16_v_i16m1(
        dut,
        ext_addconj_i16m1(__riscv_vle16_v_i16m1(demo_vs2, vl8),
                          __riscv_vle16_v_i16m1(demo_vs1, vl8)),
        vl8);
    pass &= ext_demo_check_result("vscaddconj", ref, dut, ref_vxsat);

    ref_vxsat = 0;
    ext_ref_addrq_i16(ref, demo_vs2, demo_vs1, &ref_vxsat);
    ext_clear_vxsat_flag();
    __riscv_vse16_v_i16m1(
        dut,
        ext_addrq_i16m1(__riscv_vle16_v_i16m1(demo_vs2, vl8),
                        __riscv_vle16_v_i16m1(demo_vs1, vl8)),
        vl8);
    pass &= ext_demo_check_result("vscaddrq", ref, dut, ref_vxsat);

    ref_vxsat = 0;
    ext_ref_addrtq_i16(ref, demo_vs2, demo_vs1, &ref_vxsat);
    ext_clear_vxsat_flag();
    __riscv_vse16_v_i16m1(
        dut,
        ext_addrtq_i16m1(__riscv_vle16_v_i16m1(demo_vs2, vl8),
                         __riscv_vle16_v_i16m1(demo_vs1, vl8)),
        vl8);
    pass &= ext_demo_check_result("vscaddrtq", ref, dut, ref_vxsat);

    if (!pass) {
        printf("EXT instruction demo: FAIL\n");
        return 0;
    }

    printf("EXT instruction demo: PASS\n");
    return 1;
}
#endif

#include "dft_rvv.c"
#if FFT_EXT_FUSED_LEAF_COMBINE_N256 || FFT_EXT_FUSED_LEAF_COMBINE_N64 || FFT_EXT_FUSED_LEAF_COMBINE_N128 || FFT_EXT_FUSED_LEAF_COMBINE_N512 || FFT_EXT_FUSED_LEAF_COMBINE_N1024 || FFT_EXT_FUSED_LEAF_COMBINE_N2048 || FFT_EXT_FUSED_LEAF_COMBINE_N4096

static inline __attribute__((always_inline))
void fft_ext_fused_forward_leaf(const int16_t *src,
                                vuint16m1_t swap_idx16,
                                vint16m1_t minus_j_sign,
#if FFT_EXT_LEAF_TWIDDLE_RESIDENT
                                vint16m1_t leaf_tw0,
                                vint16m1_t leaf_tw1,
                                vint16m1_t leaf_tw2,
#endif
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

#if !FFT_CUSTOM_ROTATE_ADD_HELPERS
    dft16_forward_kernel_i16m1(x0, x1, x2, x3,
#if FFT_EXT_LEAF_TWIDDLE_RESIDENT
                                leaf_tw0, leaf_tw1, leaf_tw2,
#endif
                                swap_idx16, minus_j_sign,
                                y0, y1, y2, y3);
#else
    dft16_forward_kernel_i16m1(x0, x1, x2, x3,
#if FFT_EXT_LEAF_TWIDDLE_RESIDENT
                                leaf_tw0, leaf_tw1, leaf_tw2,
#endif
                                y0, y1, y2, y3);
#endif
}

static inline __attribute__((always_inline))
void fft_ext_fused_inverse_leaf(const int16_t *src,
                                vuint16m1_t swap_idx16,
                                vint16m1_t minus_j_sign,
#if FFT_EXT_LEAF_TWIDDLE_RESIDENT
                                vint16m1_t leaf_tw0,
                                vint16m1_t leaf_tw1,
                                vint16m1_t leaf_tw2,
#endif
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

#if !FFT_CUSTOM_ROTATE_ADD_HELPERS
    dft16_inverse_kernel_i16m1(x0, x1, x2, x3,
#if FFT_EXT_LEAF_TWIDDLE_RESIDENT
                                leaf_tw0, leaf_tw1, leaf_tw2,
#endif
                                swap_idx16, minus_j_sign,
                                y0, y1, y2, y3);
#else
    dft16_inverse_kernel_i16m1(x0, x1, x2, x3,
#if FFT_EXT_LEAF_TWIDDLE_RESIDENT
                                leaf_tw0, leaf_tw1, leaf_tw2,
#endif
                                y0, y1, y2, y3);
#endif
}

static inline __attribute__((always_inline))
void fft_ext_fused_forward_block(vint16m1_t x0,
                                 vint16m1_t x1,
                                 vint16m1_t x2,
                                 vint16m1_t x3,
                                 int off,
                                 unsigned shift,
                                 int16_t *dst,
                                 vuint16m1_t swap_idx16,
                                 vint16m1_t minus_j_sign)
{
    const size_t vl8 = 8;
    vint16m1_t y0;
    vint16m1_t y1;
    vint16m1_t y2;
    vint16m1_t y3;

#if !FFT_CUSTOM_ROTATE_ADD_HELPERS
    rvv_combine_radix4_forward_64_block_i16m1(
        x0, x1, x2, x3, off, shift, swap_idx16, minus_j_sign,
        &y0, &y1, &y2, &y3);
#else
    rvv_combine_radix4_forward_64_block_i16m1(
        x0, x1, x2, x3, off, shift, &y0, &y1, &y2, &y3);
#endif

    __riscv_vse16_v_i16m1(dst + 0 * 32 + off, y0, vl8);
    __riscv_vse16_v_i16m1(dst + 1 * 32 + off, y1, vl8);
    __riscv_vse16_v_i16m1(dst + 2 * 32 + off, y2, vl8);
    __riscv_vse16_v_i16m1(dst + 3 * 32 + off, y3, vl8);
}

static inline __attribute__((always_inline))
void fft_ext_fused_inverse_block(vint16m1_t x0,
                                 vint16m1_t x1,
                                 vint16m1_t x2,
                                 vint16m1_t x3,
                                 int off,
                                 unsigned shift,
                                 int16_t *dst,
                                 vuint16m1_t swap_idx16,
                                 vint16m1_t minus_j_sign)
{
    const size_t vl8 = 8;
    vint16m1_t y0;
    vint16m1_t y1;
    vint16m1_t y2;
    vint16m1_t y3;

#if !FFT_CUSTOM_ROTATE_ADD_HELPERS
    rvv_combine_radix4_inverse_64_block_i16m1(
        x0, x1, x2, x3, off, shift, swap_idx16, minus_j_sign,
        &y0, &y1, &y2, &y3);
#else
    rvv_combine_radix4_inverse_64_block_i16m1(
        x0, x1, x2, x3, off, shift, &y0, &y1, &y2, &y3);
#endif

    __riscv_vse16_v_i16m1(dst + 0 * 32 + off, y0, vl8);
    __riscv_vse16_v_i16m1(dst + 1 * 32 + off, y1, vl8);
    __riscv_vse16_v_i16m1(dst + 2 * 32 + off, y2, vl8);
    __riscv_vse16_v_i16m1(dst + 3 * 32 + off, y3, vl8);
}

/* Keep all four blocks resident only for the isolated 0017 candidate. */
static void __attribute__((noinline))
fft_ext_fused_dft64_forward(const int16_t *src,
                            int16_t *dst,
                            unsigned shift)
{
    const size_t vl8 = 8;
    FFT_EXT_SET_E16M1_VL8();
    vuint16m1_t swap_idx16 = __riscv_vle16_v_u16m1(swap_re_im_idx16, vl8);
    vint16m1_t minus_j_sign = __riscv_vle16_v_i16m1(minus_j_sign16, vl8);
#if FFT_EXT_LEAF_TWIDDLE_RESIDENT
    register vint16m1_t leaf_tw0 asm("v28") = __riscv_vle16_v_i16m1(ext16_tw_fwd[0], vl8);
    register vint16m1_t leaf_tw1 asm("v29") = __riscv_vle16_v_i16m1(ext16_tw_fwd[1], vl8);
    register vint16m1_t leaf_tw2 asm("v30") = __riscv_vle16_v_i16m1(ext16_tw_fwd[2], vl8);
#endif

    vint16m1_t l0_0, l0_1, l0_2, l0_3;
    vint16m1_t l1_0, l1_1, l1_2, l1_3;
    vint16m1_t l2_0, l2_1, l2_2, l2_3;
    vint16m1_t l3_0, l3_1, l3_2, l3_3;

    fft_ext_fused_forward_leaf(src + 0 * 32, swap_idx16, minus_j_sign,
#if FFT_EXT_LEAF_TWIDDLE_RESIDENT
                               leaf_tw0, leaf_tw1, leaf_tw2,
#endif
                               &l0_0, &l0_1, &l0_2, &l0_3);
#if !FFT_EXT_FULL_LEAF_RESIDENT
    __riscv_vse16_v_i16m1(g_rvv64_leaf_tmp + 0 * 32 + 16, l0_2, vl8);
    __riscv_vse16_v_i16m1(g_rvv64_leaf_tmp + 0 * 32 + 24, l0_3, vl8);
#endif

    fft_ext_fused_forward_leaf(src + 1 * 32, swap_idx16, minus_j_sign,
#if FFT_EXT_LEAF_TWIDDLE_RESIDENT
                               leaf_tw0, leaf_tw1, leaf_tw2,
#endif
                               &l1_0, &l1_1, &l1_2, &l1_3);
#if !FFT_EXT_FULL_LEAF_RESIDENT
    __riscv_vse16_v_i16m1(g_rvv64_leaf_tmp + 1 * 32 + 16, l1_2, vl8);
    __riscv_vse16_v_i16m1(g_rvv64_leaf_tmp + 1 * 32 + 24, l1_3, vl8);
#endif

    fft_ext_fused_forward_leaf(src + 2 * 32, swap_idx16, minus_j_sign,
#if FFT_EXT_LEAF_TWIDDLE_RESIDENT
                               leaf_tw0, leaf_tw1, leaf_tw2,
#endif
                               &l2_0, &l2_1, &l2_2, &l2_3);
#if !FFT_EXT_FULL_LEAF_RESIDENT
    __riscv_vse16_v_i16m1(g_rvv64_leaf_tmp + 2 * 32 + 16, l2_2, vl8);
    __riscv_vse16_v_i16m1(g_rvv64_leaf_tmp + 2 * 32 + 24, l2_3, vl8);
#endif

    fft_ext_fused_forward_leaf(src + 3 * 32, swap_idx16, minus_j_sign,
#if FFT_EXT_LEAF_TWIDDLE_RESIDENT
                               leaf_tw0, leaf_tw1, leaf_tw2,
#endif
                               &l3_0, &l3_1, &l3_2, &l3_3);
#if !FFT_EXT_FULL_LEAF_RESIDENT
    __riscv_vse16_v_i16m1(g_rvv64_leaf_tmp + 3 * 32 + 16, l3_2, vl8);
    __riscv_vse16_v_i16m1(g_rvv64_leaf_tmp + 3 * 32 + 24, l3_3, vl8);
#endif

    fft_ext_fused_forward_block(l0_0, l1_0, l2_0, l3_0, 0, shift, dst,
                                swap_idx16, minus_j_sign);
    fft_ext_fused_forward_block(l0_1, l1_1, l2_1, l3_1, 8, shift, dst,
                                swap_idx16, minus_j_sign);

#if FFT_EXT_FULL_LEAF_RESIDENT
    fft_ext_fused_forward_block(l0_2, l1_2, l2_2, l3_2,
                                16, shift, dst, swap_idx16, minus_j_sign);
    fft_ext_fused_forward_block(l0_3, l1_3, l2_3, l3_3,
                                24, shift, dst, swap_idx16, minus_j_sign);
#else
    fft_ext_fused_forward_block(
        __riscv_vle16_v_i16m1(g_rvv64_leaf_tmp + 0 * 32 + 16, vl8),
        __riscv_vle16_v_i16m1(g_rvv64_leaf_tmp + 1 * 32 + 16, vl8),
        __riscv_vle16_v_i16m1(g_rvv64_leaf_tmp + 2 * 32 + 16, vl8),
        __riscv_vle16_v_i16m1(g_rvv64_leaf_tmp + 3 * 32 + 16, vl8),
        16, shift, dst, swap_idx16, minus_j_sign);
    fft_ext_fused_forward_block(
        __riscv_vle16_v_i16m1(g_rvv64_leaf_tmp + 0 * 32 + 24, vl8),
        __riscv_vle16_v_i16m1(g_rvv64_leaf_tmp + 1 * 32 + 24, vl8),
        __riscv_vle16_v_i16m1(g_rvv64_leaf_tmp + 2 * 32 + 24, vl8),
        __riscv_vle16_v_i16m1(g_rvv64_leaf_tmp + 3 * 32 + 24, vl8),
        24, shift, dst, swap_idx16, minus_j_sign);
#endif
}

static void __attribute__((noinline))
fft_ext_fused_dft64_inverse(const int16_t *src,
                            int16_t *dst,
                            unsigned shift)
{
    const size_t vl8 = 8;
    FFT_EXT_SET_E16M1_VL8();
    vuint16m1_t swap_idx16 = __riscv_vle16_v_u16m1(swap_re_im_idx16, vl8);
    vint16m1_t minus_j_sign = __riscv_vle16_v_i16m1(minus_j_sign16, vl8);
#if FFT_EXT_LEAF_TWIDDLE_RESIDENT
    register vint16m1_t leaf_tw0 asm("v28") = __riscv_vle16_v_i16m1(ext16_tw_inv[0], vl8);
    register vint16m1_t leaf_tw1 asm("v29") = __riscv_vle16_v_i16m1(ext16_tw_inv[1], vl8);
    register vint16m1_t leaf_tw2 asm("v30") = __riscv_vle16_v_i16m1(ext16_tw_inv[2], vl8);
#endif

    vint16m1_t l0_0, l0_1, l0_2, l0_3;
    vint16m1_t l1_0, l1_1, l1_2, l1_3;
    vint16m1_t l2_0, l2_1, l2_2, l2_3;
    vint16m1_t l3_0, l3_1, l3_2, l3_3;

    fft_ext_fused_inverse_leaf(src + 0 * 32, swap_idx16, minus_j_sign,
#if FFT_EXT_LEAF_TWIDDLE_RESIDENT
                               leaf_tw0, leaf_tw1, leaf_tw2,
#endif
                               &l0_0, &l0_1, &l0_2, &l0_3);
#if !FFT_EXT_FULL_LEAF_RESIDENT
    __riscv_vse16_v_i16m1(g_rvv64_leaf_tmp + 0 * 32 + 16, l0_2, vl8);
    __riscv_vse16_v_i16m1(g_rvv64_leaf_tmp + 0 * 32 + 24, l0_3, vl8);
#endif

    fft_ext_fused_inverse_leaf(src + 1 * 32, swap_idx16, minus_j_sign,
#if FFT_EXT_LEAF_TWIDDLE_RESIDENT
                               leaf_tw0, leaf_tw1, leaf_tw2,
#endif
                               &l1_0, &l1_1, &l1_2, &l1_3);
#if !FFT_EXT_FULL_LEAF_RESIDENT
    __riscv_vse16_v_i16m1(g_rvv64_leaf_tmp + 1 * 32 + 16, l1_2, vl8);
    __riscv_vse16_v_i16m1(g_rvv64_leaf_tmp + 1 * 32 + 24, l1_3, vl8);
#endif

    fft_ext_fused_inverse_leaf(src + 2 * 32, swap_idx16, minus_j_sign,
#if FFT_EXT_LEAF_TWIDDLE_RESIDENT
                               leaf_tw0, leaf_tw1, leaf_tw2,
#endif
                               &l2_0, &l2_1, &l2_2, &l2_3);
#if !FFT_EXT_FULL_LEAF_RESIDENT
    __riscv_vse16_v_i16m1(g_rvv64_leaf_tmp + 2 * 32 + 16, l2_2, vl8);
    __riscv_vse16_v_i16m1(g_rvv64_leaf_tmp + 2 * 32 + 24, l2_3, vl8);
#endif

    fft_ext_fused_inverse_leaf(src + 3 * 32, swap_idx16, minus_j_sign,
#if FFT_EXT_LEAF_TWIDDLE_RESIDENT
                               leaf_tw0, leaf_tw1, leaf_tw2,
#endif
                               &l3_0, &l3_1, &l3_2, &l3_3);
#if !FFT_EXT_FULL_LEAF_RESIDENT
    __riscv_vse16_v_i16m1(g_rvv64_leaf_tmp + 3 * 32 + 16, l3_2, vl8);
    __riscv_vse16_v_i16m1(g_rvv64_leaf_tmp + 3 * 32 + 24, l3_3, vl8);
#endif

    fft_ext_fused_inverse_block(l0_0, l1_0, l2_0, l3_0, 0, shift, dst,
                                swap_idx16, minus_j_sign);
    fft_ext_fused_inverse_block(l0_1, l1_1, l2_1, l3_1, 8, shift, dst,
                                swap_idx16, minus_j_sign);

#if FFT_EXT_FULL_LEAF_RESIDENT
    fft_ext_fused_inverse_block(l0_2, l1_2, l2_2, l3_2,
                                16, shift, dst, swap_idx16, minus_j_sign);
    fft_ext_fused_inverse_block(l0_3, l1_3, l2_3, l3_3,
                                24, shift, dst, swap_idx16, minus_j_sign);
#else
    fft_ext_fused_inverse_block(
        __riscv_vle16_v_i16m1(g_rvv64_leaf_tmp + 0 * 32 + 16, vl8),
        __riscv_vle16_v_i16m1(g_rvv64_leaf_tmp + 1 * 32 + 16, vl8),
        __riscv_vle16_v_i16m1(g_rvv64_leaf_tmp + 2 * 32 + 16, vl8),
        __riscv_vle16_v_i16m1(g_rvv64_leaf_tmp + 3 * 32 + 16, vl8),
        16, shift, dst, swap_idx16, minus_j_sign);
    fft_ext_fused_inverse_block(
        __riscv_vle16_v_i16m1(g_rvv64_leaf_tmp + 0 * 32 + 24, vl8),
        __riscv_vle16_v_i16m1(g_rvv64_leaf_tmp + 1 * 32 + 24, vl8),
        __riscv_vle16_v_i16m1(g_rvv64_leaf_tmp + 2 * 32 + 24, vl8),
        __riscv_vle16_v_i16m1(g_rvv64_leaf_tmp + 3 * 32 + 24, vl8),
        24, shift, dst, swap_idx16, minus_j_sign);
#endif
}

static void fft_ext_dft256_fused_forward(const int16_t *x,
                                         int16_t *y,
                                         unsigned shift)
{
    for (int child = 0; child < 4; ++child) {
        fft_ext_fused_dft64_forward(x + child * 128, y + child * 128, 3u);
    }
    rvv_combine_radix4_forward_256_rvv(y, y, shift);
}

static void fft_ext_dft128_fused_forward(const int16_t *x, int16_t *y)
{
    fft_ext_fused_dft64_forward(x + 0 * 128, y + 0 * 128, 3u);
    fft_ext_fused_dft64_forward(x + 1 * 128, y + 1 * 128, 3u);
    rvv_combine_radix2_forward_128_rvv(y, y, 0);
}

static void fft_ext_dft512_fused_forward(const int16_t *x, int16_t *y)
{
    fft_ext_dft256_fused_forward(x + 0 * 512, y + 0 * 512, 1u);
    fft_ext_dft256_fused_forward(x + 1 * 512, y + 1 * 512, 1u);
    rvv_combine_radix2_forward_512_rvv(y, y, 0);
}

static void fft_ext_dft256_fused_inverse(const int16_t *x,
                                         int16_t *y,
                                         unsigned shift)
{
    for (int child = 0; child < 4; ++child) {
        fft_ext_fused_dft64_inverse(x + child * 128, y + child * 128, 3u);
    }
    rvv_combine_radix4_inverse_256_rvv(y, y, shift);
}

static void fft_ext_dft128_fused_inverse(const int16_t *x, int16_t *y)
{
    fft_ext_fused_dft64_inverse(x + 0 * 128, y + 0 * 128, 3u);
    fft_ext_fused_dft64_inverse(x + 1 * 128, y + 1 * 128, 3u);
    rvv_combine_radix2_inverse_128_rvv(y, y, 0);
}

static void fft_ext_dft512_fused_inverse(const int16_t *x, int16_t *y)
{
    fft_ext_dft256_fused_inverse(x + 0 * 512, y + 0 * 512, 1u);
    fft_ext_dft256_fused_inverse(x + 1 * 512, y + 1 * 512, 1u);
    rvv_combine_radix2_inverse_512_rvv(y, y, 0);
}

static void fft_ext_dft1024_fused_forward(const int16_t *x,
                                          int16_t *y,
                                          unsigned scale)
{
    fft_ext_dft256_fused_forward(x + 0 * 512, y + 0 * 512, 1u);
    fft_ext_dft256_fused_forward(x + 1 * 512, y + 1 * 512, 1u);
    fft_ext_dft256_fused_forward(x + 2 * 512, y + 2 * 512, 1u);
    fft_ext_dft256_fused_forward(x + 3 * 512, y + 3 * 512, 1u);
    rvv_combine_radix4_forward_1024_rvv(y, y, scale ? 1u : 0u);
}

static void fft_ext_dft1024_fused_inverse(const int16_t *x,
                                          int16_t *y,
                                          unsigned scale)
{
    fft_ext_dft256_fused_inverse(x + 0 * 512, y + 0 * 512, 1u);
    fft_ext_dft256_fused_inverse(x + 1 * 512, y + 1 * 512, 1u);
    fft_ext_dft256_fused_inverse(x + 2 * 512, y + 2 * 512, 1u);
    fft_ext_dft256_fused_inverse(x + 3 * 512, y + 3 * 512, 1u);
    rvv_combine_radix4_inverse_1024_rvv(y, y, scale ? 1u : 0u);
}

static void fft_ext_dft2048_fused_forward(const int16_t *x, int16_t *y)
{
    fft_ext_dft1024_fused_forward(x + 0 * 2048, y + 0 * 2048, 1u);
    fft_ext_dft1024_fused_forward(x + 1 * 2048, y + 1 * 2048, 1u);
    rvv_combine_radix2_forward_2048_rvv(y, y, 0);
}

static void fft_ext_dft2048_fused_inverse(const int16_t *x, int16_t *y)
{
    fft_ext_dft1024_fused_inverse(x + 0 * 2048, y + 0 * 2048, 1u);
    fft_ext_dft1024_fused_inverse(x + 1 * 2048, y + 1 * 2048, 1u);
    rvv_combine_radix2_inverse_2048_rvv(y, y, 0);
}

static void fft_ext_dft4096_fused_forward(const int16_t *x, int16_t *y)
{
    fft_ext_dft1024_fused_forward(x + 0 * 2048, y + 0 * 2048, 1u);
    fft_ext_dft1024_fused_forward(x + 1 * 2048, y + 1 * 2048, 1u);
    fft_ext_dft1024_fused_forward(x + 2 * 2048, y + 2 * 2048, 1u);
    fft_ext_dft1024_fused_forward(x + 3 * 2048, y + 3 * 2048, 1u);
    rvv_combine_radix4_forward_4096_rvv(y, y, 0);
}

static void fft_ext_dft4096_fused_inverse(const int16_t *x, int16_t *y)
{
    fft_ext_dft1024_fused_inverse(x + 0 * 2048, y + 0 * 2048, 1u);
    fft_ext_dft1024_fused_inverse(x + 1 * 2048, y + 1 * 2048, 1u);
    fft_ext_dft1024_fused_inverse(x + 2 * 2048, y + 2 * 2048, 1u);
    fft_ext_dft1024_fused_inverse(x + 3 * 2048, y + 3 * 2048, 1u);
    rvv_combine_radix4_inverse_4096_rvv(y, y, 0);
}

#endif
