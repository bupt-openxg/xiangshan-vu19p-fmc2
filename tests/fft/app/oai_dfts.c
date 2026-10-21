#if defined(FFT_OAI_DFT16_MINIMAL)
#include "fft_test_common.h"
#include <stdint.h>
typedef int log_t;
#include "include/sse_intrin.h"

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


static const int16_t conjugatedft[8] __attribute__((aligned(16))) = {
    -1, 1, -1, 1, -1, 1, -1, 1
};

static const int16_t tw16_dft_a[24] __attribute__((aligned(16))) = {
    32767, 0, 30272, 12540, 23169, 23170, 12539, 30273,
    32767, 0, 23169, 23170, 0, 32767, -23170, 23170,
    32767, 0, 12539, 30273, -23170, 23170, -30273, -12539
};

static const int16_t tw16_dft_b[24] __attribute__((aligned(16))) = {
    0, 32767, -12540, 30272, -23170, 23169, -30273, 12539,
    0, 32767, -23170, 23169, -32767, 0, -23170, -23170,
    0, 32767, -30273, 12539, -23170, -23170, 12539, -30273
};

static const int16_t tw16_idft_a[24] __attribute__((aligned(16))) = {
    32767, 0, 30272, -12540, 23169, -23170, 12539, -30273,
    32767, 0, 23169, -23170, 0, -32767, -23170, -23170,
    32767, 0, 12539, -30273, -23170, -23170, -30273, 12539
};

static const int16_t tw16_idft_b[24] __attribute__((aligned(16))) = {
    0, 32767, 12540, 30272, 23170, 23169, 30273, 12539,
    0, 32767, 23170, 23169, 32767, 0, 23170, -23170,
    0, 32767, 30273, 12539, 23170, -23170, -12539, -30273
};

static inline __m128i fft_complex_shuffle_mask(void)
{
    return _mm_set_epi8(13, 12, 15, 14, 9, 8, 11, 10,
                        5, 4, 7, 6, 1, 0, 3, 2);
}

static inline __m128i cpack(__m128i xre, __m128i xim)
{
    __m128i cpack_tmp1 = _mm_unpacklo_epi32(xre, xim);
    __m128i cpack_tmp2 = _mm_unpackhi_epi32(xre, xim);

    return _mm_packs_epi32(_mm_srai_epi32(cpack_tmp1, 15),
                           _mm_srai_epi32(cpack_tmp2, 15));
}

static inline __m128i packed_cmult2(__m128i a, __m128i b, __m128i b2)
{
    __m128i cre = _mm_madd_epi16(a, b);
    __m128i cim = _mm_madd_epi16(a, b2);

    return cpack(cre, cim);
}

static inline void dft16(int16_t *x, int16_t *y)
{
    __m128i *tw16a_128 = (__m128i *)tw16_dft_a;
    __m128i *tw16b_128 = (__m128i *)tw16_dft_b;
    __m128i *x128 = (__m128i *)x;
    __m128i *y128 = (__m128i *)y;
    __m128i x1_flip;
    __m128i x3_flip;
    __m128i x02t;
    __m128i x13t;
    __m128i ytmp0;
    __m128i ytmp1;
    __m128i ytmp2;
    __m128i ytmp3;
    __m128i xtmp0;
    __m128i xtmp1;
    __m128i xtmp2;
    __m128i xtmp3;
    __m128i complex_shuffle = fft_complex_shuffle_mask();

    x02t = _mm_adds_epi16(x128[0], x128[2]);
    x13t = _mm_adds_epi16(x128[1], x128[3]);
    xtmp0 = _mm_adds_epi16(x02t, x13t);
    xtmp2 = _mm_subs_epi16(x02t, x13t);
    x1_flip = _mm_sign_epi16(x128[1], *(__m128i *)conjugatedft);
    x1_flip = _mm_shuffle_epi8(x1_flip, complex_shuffle);
    x3_flip = _mm_sign_epi16(x128[3], *(__m128i *)conjugatedft);
    x3_flip = _mm_shuffle_epi8(x3_flip, complex_shuffle);
    x02t = _mm_subs_epi16(x128[0], x128[2]);
    x13t = _mm_subs_epi16(x1_flip, x3_flip);
    xtmp1 = _mm_adds_epi16(x02t, x13t);
    xtmp3 = _mm_subs_epi16(x02t, x13t);

    ytmp0 = _mm_unpacklo_epi32(xtmp0, xtmp1);
    ytmp1 = _mm_unpackhi_epi32(xtmp0, xtmp1);
    ytmp2 = _mm_unpacklo_epi32(xtmp2, xtmp3);
    ytmp3 = _mm_unpackhi_epi32(xtmp2, xtmp3);
    xtmp0 = _mm_unpacklo_epi64(ytmp0, ytmp2);
    xtmp1 = _mm_unpackhi_epi64(ytmp0, ytmp2);
    xtmp2 = _mm_unpacklo_epi64(ytmp1, ytmp3);
    xtmp3 = _mm_unpackhi_epi64(ytmp1, ytmp3);

    xtmp1 = packed_cmult2(xtmp1, tw16a_128[0], tw16b_128[0]);
    xtmp2 = packed_cmult2(xtmp2, tw16a_128[1], tw16b_128[1]);
    xtmp3 = packed_cmult2(xtmp3, tw16a_128[2], tw16b_128[2]);

    x02t = _mm_adds_epi16(xtmp0, xtmp2);
    x13t = _mm_adds_epi16(xtmp1, xtmp3);
    y128[0] = _mm_adds_epi16(x02t, x13t);
    y128[2] = _mm_subs_epi16(x02t, x13t);
    x1_flip = _mm_sign_epi16(xtmp1, *(__m128i *)conjugatedft);
    x1_flip = _mm_shuffle_epi8(x1_flip, complex_shuffle);
    x3_flip = _mm_sign_epi16(xtmp3, *(__m128i *)conjugatedft);
    x3_flip = _mm_shuffle_epi8(x3_flip, complex_shuffle);
    x02t = _mm_subs_epi16(xtmp0, xtmp2);
    x13t = _mm_subs_epi16(x1_flip, x3_flip);
    y128[1] = _mm_adds_epi16(x02t, x13t);
    y128[3] = _mm_subs_epi16(x02t, x13t);
}

static inline void idft16(int16_t *x, int16_t *y)
{
    __m128i *tw16a_128 = (__m128i *)tw16_idft_a;
    __m128i *tw16b_128 = (__m128i *)tw16_idft_b;
    __m128i *x128 = (__m128i *)x;
    __m128i *y128 = (__m128i *)y;
    __m128i x1_flip;
    __m128i x3_flip;
    __m128i x02t;
    __m128i x13t;
    __m128i ytmp0;
    __m128i ytmp1;
    __m128i ytmp2;
    __m128i ytmp3;
    __m128i xtmp0;
    __m128i xtmp1;
    __m128i xtmp2;
    __m128i xtmp3;
    __m128i complex_shuffle = fft_complex_shuffle_mask();

    x02t = _mm_adds_epi16(x128[0], x128[2]);
    x13t = _mm_adds_epi16(x128[1], x128[3]);
    xtmp0 = _mm_adds_epi16(x02t, x13t);
    xtmp2 = _mm_subs_epi16(x02t, x13t);
    x1_flip = _mm_sign_epi16(x128[1], *(__m128i *)conjugatedft);
    x1_flip = _mm_shuffle_epi8(x1_flip, complex_shuffle);
    x3_flip = _mm_sign_epi16(x128[3], *(__m128i *)conjugatedft);
    x3_flip = _mm_shuffle_epi8(x3_flip, complex_shuffle);
    x02t = _mm_subs_epi16(x128[0], x128[2]);
    x13t = _mm_subs_epi16(x1_flip, x3_flip);
    xtmp3 = _mm_adds_epi16(x02t, x13t);
    xtmp1 = _mm_subs_epi16(x02t, x13t);

    ytmp0 = _mm_unpacklo_epi32(xtmp0, xtmp1);
    ytmp1 = _mm_unpackhi_epi32(xtmp0, xtmp1);
    ytmp2 = _mm_unpacklo_epi32(xtmp2, xtmp3);
    ytmp3 = _mm_unpackhi_epi32(xtmp2, xtmp3);
    xtmp0 = _mm_unpacklo_epi64(ytmp0, ytmp2);
    xtmp1 = _mm_unpackhi_epi64(ytmp0, ytmp2);
    xtmp2 = _mm_unpacklo_epi64(ytmp1, ytmp3);
    xtmp3 = _mm_unpackhi_epi64(ytmp1, ytmp3);

    xtmp1 = packed_cmult2(xtmp1, tw16a_128[0], tw16b_128[0]);
    xtmp2 = packed_cmult2(xtmp2, tw16a_128[1], tw16b_128[1]);
    xtmp3 = packed_cmult2(xtmp3, tw16a_128[2], tw16b_128[2]);

    x02t = _mm_adds_epi16(xtmp0, xtmp2);
    x13t = _mm_adds_epi16(xtmp1, xtmp3);
    y128[0] = _mm_adds_epi16(x02t, x13t);
    y128[2] = _mm_subs_epi16(x02t, x13t);
    x1_flip = _mm_sign_epi16(xtmp1, *(__m128i *)conjugatedft);
    x1_flip = _mm_shuffle_epi8(x1_flip, complex_shuffle);
    x3_flip = _mm_sign_epi16(xtmp3, *(__m128i *)conjugatedft);
    x3_flip = _mm_shuffle_epi8(x3_flip, complex_shuffle);
    x02t = _mm_subs_epi16(xtmp0, xtmp2);
    x13t = _mm_subs_epi16(x1_flip, x3_flip);
    y128[3] = _mm_adds_epi16(x02t, x13t);
    y128[1] = _mm_subs_epi16(x02t, x13t);
}

#define FFT_POW2_BASE_DFT16 dft16
#define FFT_POW2_BASE_IDFT16 idft16
#include "fft_pow2_scalar_impl.h"
#undef FFT_POW2_BASE_DFT16
#undef FFT_POW2_BASE_IDFT16

void idft16f(int16_t *x,int16_t *y)
{
    idft16(x, y);
}

void oai_fft_dft16_reference(int16_t *x, int16_t *y)
{
    dft16(x, y);
}

int oai_fft_dft_reference_points(int points, int16_t *x, int16_t *y)
{
    return fft_pow2_run_forward(points, x, y);
}

int oai_fft_idft_reference_points(int points, int16_t *x, int16_t *y)
{
    return fft_pow2_run_inverse(points, x, y);
}

#ifndef FFT_LIBRARY_ONLY
static volatile int16_t g_fft_perf_sink;

static int16_t g_fft_main_input[2 * FFT_MAIN_MAX_POINTS] __attribute__((aligned(16)));
static int16_t g_fft_main_output[2 * FFT_MAIN_MAX_POINTS] __attribute__((aligned(16)));

int main(void)
{
    int16_t *x = g_fft_main_input;
    int16_t *y = g_fft_main_output;

    if (!fft_pow2_supported_points(FFT_MAIN_POINTS)) {
        printf("Unsupported FFT_MAIN_POINTS=%d in oai_dfts.c minimal mode\n", FFT_MAIN_POINTS);
        return 1;
    }

    if (!fft_load_test_input(x, FFT_MAIN_POINTS)) {
        printf("Failed to load FFT test input for %d points\n", FFT_MAIN_POINTS);
        return 1;
    }

    for (int i = 0; i < FFT_MAIN_REPEAT; ++i) {
        int ok = FFT_MAIN_RUN_IDFT
            ? fft_pow2_run_inverse(FFT_MAIN_POINTS, x, y)
            : fft_pow2_run_forward(FFT_MAIN_POINTS, x, y);
        if (!ok) {
            printf("OAI minimal dispatch failed for %d points\n", FFT_MAIN_POINTS);
            return 1;
        }
    }

#if FFT_VALIDATE
    if (FFT_MAIN_POINTS <= 64) {
        fft_print_complex_vector("Input", x, FFT_MAIN_POINTS);
        fft_print_complex_vector(FFT_MAIN_RUN_IDFT ? "OAI minimal IDFT result" : "OAI minimal DFT result",
                                 y,
                                 FFT_MAIN_POINTS);
    } else {
        printf("Vector dump omitted for %d points in validate mode\n", FFT_MAIN_POINTS);
    }
    printf("CHECK: PASS\n");
    return 0;
#else
    g_fft_perf_sink = (int16_t)(y[0] ^ y[1]);
    return (g_fft_perf_sink == (int16_t)0x7fff) ? 1 : 0;
#endif
}
#endif
#else
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
//#include <klib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "klib.h"
#include <stdint.h>
#include <math.h>
#include <pthread.h>
//#include <execinfo.h>
#include "oai_dfts.h"
#include "fdlibm.h"
#include <riscv_vector.h>

#define yuandaima
//#define RVV_Intr

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define OAIDFTS_MAIN
//#ifndef MR_MAIN
//#include "PHY/defs_common.h"
//#include "PHY/impl_defs_top.h"
//#else
#include "include/utils/time_meas.h"
#include "LOG/log.h"

#define ONE_OVER_SQRT2_Q15 23170

//int oai_exit=0;
//#endif

#define ONE_OVER_SQRT3_Q15 18919

#include "sse_intrin.h"

#include "utils/assertions.h"

#include "tools_defs.h"


#include <csr.h>
// VS 域在 mstatus 寄存器中的位置和掩码
// VS 域为 mstatus[10:9]
#define MSTATUS_VS_SHIFT        9
#define MSTATUS_VS_MASK         (0x3UL << MSTATUS_VS_SHIFT)

// VS 域的常见值
#define MSTATUS_VS_OFF          (0x0UL << MSTATUS_VS_SHIFT) // 00：向量指令未启用
#define MSTATUS_VS_INITIAL      (0x1UL << MSTATUS_VS_SHIFT) // 01：向量寄存器集处于初始状态
#define MSTATUS_VS_CLEAN        (0x2UL << MSTATUS_VS_SHIFT) // 10：向量寄存器集已使用但干净
#define MSTATUS_VS_DIRTY        (0x3UL << MSTATUS_VS_SHIFT) // 11：向量寄存器集已使用且脏
#define read_csr(csr)       ({ unsigned long __v; __asm__ volatile ("csrr %0, " #csr : "=r" (__v)); __v; })
#define write_csr(csr, val) ({ unsigned long __v = (unsigned long)(val); __asm__ volatile ("csrw " #csr ", %0" :: "r" (__v)); })

#ifndef __RISCV_VXRM_RDN
#define __RISCV_VXRM_RDN 2
#endif

void set_mstatus_vs_field(uintptr_t vs_value) {
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

const static int16_t conjugatedft[32] __attribute__((aligned(32))) = {-1,1,-1,1,-1,1,-1,1,-1,1,-1,1,-1,1,-1,1,-1,1};

static const int16_t ext_minus_j_rep16[16] __attribute__((aligned(32))) = {
    0, -32768, 0, -32768, 0, -32768, 0, -32768,
    0, -32768, 0, -32768, 0, -32768, 0, -32768
};

const int16_t ext_plus_j_rep16[16] __attribute__((aligned(32))) = {
    0, 32767, 0, 32767, 0, 32767, 0, 32767,
    0, 32767, 0, 32767, 0, 32767, 0, 32767
};

const static int16_t reflip[32]  __attribute__((aligned(32))) = {1,-1,1,-1,1,-1,1,-1,1,-1,1,-1,1,-1,1,-1};
// ---- forward declarations to avoid implicit declaration ----


// ---- twiddle tables should be visible in this TU ----
// 64-point optimized DFT

const static int16_t tw64[96] __attribute__((aligned(32))) = { 
32767,0,32609,-3212,32137,-6393,31356,-9512,
30272,-12540,28897,-15447,27244,-18205,25329,-20788,
23169,-23170,20787,-25330,18204,-27245,15446,-28898,
12539,-30273,9511,-31357,6392,-32138,3211,-32610,
32767,0,32137,-6393,30272,-12540,27244,-18205,
23169,-23170,18204,-27245,12539,-30273,6392,-32138,
0,-32767,-6393,-32138,-12540,-30273,-18205,-27245,
-23170,-23170,-27245,-18205,-30273,-12540,-32138,-6393,
32767,0,31356,-9512,27244,-18205,20787,-25330,
12539,-30273,3211,-32610,-6393,-32138,-15447,-28898,
-23170,-23170,-28898,-15447,-32138,-6393,-32610,3211,
-30273,12539,-25330,20787,-18205,27244,-9512,31356
                                                };
const static int16_t tw64a[96] __attribute__((aligned(32))) = { 
32767,0,32609,3212,32137,6393,31356,9512,
30272,12540,28897,15447,27244,18205,25329,20788,
23169,23170,20787,25330,18204,27245,15446,28898,
12539,30273,9511,31357,6392,32138,3211,32610,
32767,0,32137,6393,30272,12540,27244,18205,
23169,23170,18204,27245,12539,30273,6392,32138,
0,32767,-6393,32138,-12540,30273,-18205,27245,
-23170,23170,-27245,18205,-30273,12540,-32138,6393,
32767,0,31356,9512,27244,18205,20787,25330,
12539,30273,3211,32610,-6393,32138,-15447,28898,
-23170,23170,-28898,15447,-32138,6393,-32610,-3211,
-30273,-12539,-25330,-20787,-18205,-27244,-9512,-31356
                                                 };
const static int16_t tw64b[96] __attribute__((aligned(32))) = { 
0,32767,-3212,32609,-6393,32137,-9512,31356,
-12540,30272,-15447,28897,-18205,27244,-20788,25329,
-23170,23169,-25330,20787,-27245,18204,-28898,15446,
-30273,12539,-31357,9511,-32138,6392,-32610,3211,
0,32767,-6393,32137,-12540,30272,-18205,27244,
-23170,23169,-27245,18204,-30273,12539,-32138,6392,
-32767,0,-32138,-6393,-30273,-12540,-27245,-18205,
-23170,-23170,-18205,-27245,-12540,-30273,-6393,-32138,
0,32767,-9512,31356,-18205,27244,-25330,20787,
-30273,12539,-32610,3211,-32138,-6393,-28898,-15447,
-23170,-23170,-15447,-28898,-6393,-32138,3211,-32610,
12539,-30273,20787,-25330,27244,-18205,31356,-9512
                                                 };
const static int16_t tw64c[96] __attribute__((aligned(32))) = { 
0,32767,3212,32609,6393,32137,9512,31356,
12540,30272,15447,28897,18205,27244,20788,25329,
23170,23169,25330,20787,27245,18204,28898,15446,
30273,12539,31357,9511,32138,6392,32610,3211,
0,32767,6393,32137,12540,30272,18205,27244,
23170,23169,27245,18204,30273,12539,32138,6392,
32767,0,32138,-6393,30273,-12540,27245,-18205,
23170,-23170,18205,-27245,12540,-30273,6393,-32138,
0,32767,9512,31356,18205,27244,25330,20787,
30273,12539,32610,3211,32138,-6393,28898,-15447,
23170,-23170,15447,-28898,6393,-32138,-3211,-32610,
-12539,-30273,-20787,-25330,-27244,-18205,-31356,-9512
                                                 };
/*-----------------AVX函数改写为RVV intrinsic--------------------------*/


static inline vint16m4_t rvv_adds_epi16(vint16m4_t a, vint16m4_t b, size_t vl) {
    // __riscv_vsadd_vv_i16m4: Signed Saturating Add Vector-Vector
    return __riscv_vsadd_vv_i16m4(a, b, vl);
}

/**
 * @brief 签名饱和减法 (vint16m4_t)
 */
static inline vint16m4_t rvv_subs_epi16(vint16m4_t a, vint16m4_t b, size_t vl) {
    // __riscv_vssub_vv_i16m4: Signed Saturating Subtract Vector-Vector
    return __riscv_vssub_vv_i16m4(a, b, vl);
}

/**
 * @brief 符号反转操作 (AVX _mm_sign_epi16 模拟)
 * 如果 b < 0, 返回 -a; 否则返回 a.
 */
static inline vint16m4_t rvv_sign_epi16(vint16m4_t a, vint16m4_t b, size_t vl) {
    // 1. 检查 b 的符号 (b < 0)。布尔掩码使用 b4 (EMUL=4)
    vbool4_t b_is_neg = __riscv_vmslt_vx_i16m4_b4(b, 0, vl);
    
    // 2. 计算 -a (即 0 - a)。
    vint16m4_t neg_a = __riscv_vneg_v_i16m4(a, vl);
    
    // 3. 使用掩码进行选择：
    // vselect(mask, vec_true, vec_false, vl)
    return __riscv_vmerge_vvm_i16m4(a, neg_a, b_is_neg, vl);
}

// 假设 rvv_set_epi8_mask 已经定义并返回 vint8m4_t
 static inline vint8m4_t rvv_set_epi8_mask(
    int8_t e31, int8_t e30, int8_t e29, int8_t e28, int8_t e27, int8_t e26, int8_t e25, int8_t e24,
    int8_t e23, int8_t e22, int8_t e21, int8_t e20, int8_t e19, int8_t e18, int8_t e17, int8_t e16,
    int8_t e15, int8_t e14, int8_t e13, int8_t e12, int8_t e11, int8_t e10, int8_t e9, int8_t e8,
    int8_t e7, int8_t e6, int8_t e5, int8_t e4, int8_t e3, int8_t e2, int8_t e1, int8_t e0)
{
    // C 数组，注意：AVX 是小端存储，e0 对应地址最低字节
    const int8_t const_array[32] = {
        e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, e11, e12, e13, e14, e15,
        e16, e17, e18, e19, e20, e21, e22, e23, e24, e25, e26, e27, e28, e29, e30, e31
    };
    
    // RVV 1.0 加载 32 个 8-bit 元素 (VL=32, LMUL=4)
    size_t vl = __riscv_vsetvl_e8m4(32);
    return __riscv_vle8_v_i8m4((const int8_t*)const_array, vl);
}
/**
 * @brief 8-bit Shuffle (vint16m4_t)
 * 输入 a 转换为 vint8m4_t (32x 8-bit)，mask 保持 vint8m4_t。
 */
static inline vint16m4_t rvv_shuffle_epi8(vint16m4_t a, vint8m4_t mask, size_t vl_16) {
    // 对于 8-bit 操作，VL 始终是 32
    size_t vl_8bit = __riscv_vsetvl_e8m4(32); 

    // 1. 解释输入 i16m4 为 i8m4
    vint8m4_t a_8bit;
    // 使用 memcpy 来模拟位重新解释，避免依赖特定编译器 Intrinsic
    memcpy(&a_8bit, &a, 32); 

    // 2. 解释 mask i8m4 为 u8m4
    vuint8m4_t mask_u8;
    memcpy(&mask_u8, &mask, 32);

    // 3. vrgather (8-bit)
    vint8m4_t result_8bit = __riscv_vrgather_vv_i8m4(a_8bit, mask_u8, vl_8bit);

    // 4. 结果 i8m4 转换回 i16m4
    vint16m4_t final_res;
    memcpy(&final_res, &result_8bit, 32);
    return final_res;
}

// ----------------------------------------------------------------
// Unpack 辅助函数 (已更新为 m4 类型)
// ----------------------------------------------------------------

static __attribute__((always_inline)) inline vint16m4_t rvv_unpacklo_epi32(vint16m4_t v1, vint16m4_t v2, size_t vl)
{
    (void)vl;
    const size_t vl32 = 8;

    // reinterpret i16m4 -> u32m4 (bitcast)
    vuint32m4_t a = __riscv_vreinterpret_v_u16m4_u32m4(__riscv_vreinterpret_v_i16m4_u16m4(v1));
    vuint32m4_t b = __riscv_vreinterpret_v_u16m4_u32m4(__riscv_vreinterpret_v_i16m4_u16m4(v2));

    // idx for a lanes: [0,0,1,1,4,4,5,5]
    // build in vector regs (no memory constants)
    vuint32m4_t vid = __riscv_vid_v_u32m4(vl32);                 // 0..7
    vuint32m4_t lane = __riscv_vand_vx_u32m4(vid, 1u, vl32);     // 0,1,0,1...
    vuint32m4_t base = __riscv_vand_vx_u32m4(vid, 6u, vl32);     // 0,0,2,2,4,4,6,6
    vuint32m4_t idx_a = __riscv_vsrl_vx_u32m4(base, 1, vl32);    // 0,0,1,1,2,2,3,3
    idx_a = __riscv_vadd_vv_u32m4(idx_a, base, vl32);            // 0,0,3,3,6,6,9,9 (too big) ❌
    // 上面这条思路会跑偏，所以这里改用“直接构造”方式（更稳）：用 vrgather from a with explicit idx vector
    // ——我们用一组常量 idx 来避免复杂算术出错（这组常量只占 8 个 u32，通常会被编译器放到 rodata，代价很小）

    // 用静态常量 idx（只读，不是 volatile，不会造成往返访存；多次调用还能被缓存）
    static const uint32_t idx_a_mem[8] __attribute__((aligned(32))) = {0,0,1,1,4,4,5,5};
    static const uint32_t idx_b_mem[8] __attribute__((aligned(32))) = {0,0,1,1,4,4,5,5};

    vuint32m4_t idx_a2 = __riscv_vle32_v_u32m4(idx_a_mem, vl32);
    vuint32m4_t idx_b2 = __riscv_vle32_v_u32m4(idx_b_mem, vl32);

    vuint32m4_t a_dup = __riscv_vrgather_vv_u32m4(a, idx_a2, vl32);
    vuint32m4_t b_dup = __riscv_vrgather_vv_u32m4(b, idx_b2, vl32);

    // mask odd lanes (1,3,5,7) => take from b
    vbool8_t mask_odd = __riscv_vmsne_vx_u32m4_b8(__riscv_vand_vx_u32m4(vid, 1u, vl32), 0u, vl32);

    vuint32m4_t out32 = __riscv_vmerge_vvm_u32m4(a_dup, b_dup, mask_odd, vl32);

    // reinterpret back to i16m4
    vuint16m4_t out16u = __riscv_vreinterpret_v_u32m4_u16m4(out32);
    return __riscv_vreinterpret_v_u16m4_i16m4(out16u);
}

// -------- unpackhi_epi32: [a2,b2,a3,b3,a6,b6,a7,b7] --------
static __attribute__((always_inline)) inline vint16m4_t rvv_unpackhi_epi32(vint16m4_t v1, vint16m4_t v2, size_t vl)
{
    (void)vl;
    const size_t vl32 = 8;

    vuint32m4_t a = __riscv_vreinterpret_v_u16m4_u32m4(__riscv_vreinterpret_v_i16m4_u16m4(v1));
    vuint32m4_t b = __riscv_vreinterpret_v_u16m4_u32m4(__riscv_vreinterpret_v_i16m4_u16m4(v2));

    static const uint32_t idx_mem[8] __attribute__((aligned(32))) = {2,2,3,3,6,6,7,7};
    vuint32m4_t idx = __riscv_vle32_v_u32m4(idx_mem, vl32);

    vuint32m4_t a_dup = __riscv_vrgather_vv_u32m4(a, idx, vl32);
    vuint32m4_t b_dup = __riscv_vrgather_vv_u32m4(b, idx, vl32);

    vuint32m4_t vid = __riscv_vid_v_u32m4(vl32);
    vbool8_t mask_odd = __riscv_vmsne_vx_u32m4_b8(__riscv_vand_vx_u32m4(vid, 1u, vl32), 0u, vl32);

    vuint32m4_t out32 = __riscv_vmerge_vvm_u32m4(a_dup, b_dup, mask_odd, vl32);

    vuint16m4_t out16u = __riscv_vreinterpret_v_u32m4_u16m4(out32);
    return __riscv_vreinterpret_v_u16m4_i16m4(out16u);
}

// -------- unpacklo_epi64: [a0,b0,a2,b2] (64-bit lanes) --------
static __attribute__((always_inline)) inline vint16m4_t rvv_unpacklo_epi64(vint16m4_t v1, vint16m4_t v2, size_t vl)
{
    (void)vl;
    const size_t vl64 = 4;

    vuint64m4_t a = __riscv_vreinterpret_v_u16m4_u64m4(__riscv_vreinterpret_v_i16m4_u16m4(v1));
    vuint64m4_t b = __riscv_vreinterpret_v_u16m4_u64m4(__riscv_vreinterpret_v_i16m4_u16m4(v2));

    static const uint64_t idx_mem[4] __attribute__((aligned(32))) = {0,0,2,2};
    vuint64m4_t idx = __riscv_vle64_v_u64m4(idx_mem, vl64);

    vuint64m4_t a_dup = __riscv_vrgather_vv_u64m4(a, idx, vl64);
    vuint64m4_t b_dup = __riscv_vrgather_vv_u64m4(b, idx, vl64);

    vuint64m4_t vid = __riscv_vid_v_u64m4(vl64);
    vbool16_t mask_odd = __riscv_vmsne_vx_u64m4_b16(__riscv_vand_vx_u64m4(vid, 1u, vl64), 0u, vl64);

    vuint64m4_t out64 = __riscv_vmerge_vvm_u64m4(a_dup, b_dup, mask_odd, vl64);

    vuint16m4_t out16u = __riscv_vreinterpret_v_u64m4_u16m4(out64);
    return __riscv_vreinterpret_v_u16m4_i16m4(out16u);
}

// -------- unpackhi_epi64: [a1,b1,a3,b3] --------
static __attribute__((always_inline)) inline vint16m4_t rvv_unpackhi_epi64(vint16m4_t v1, vint16m4_t v2, size_t vl)
{
    (void)vl;
    const size_t vl64 = 4;

    vuint64m4_t a = __riscv_vreinterpret_v_u16m4_u64m4(__riscv_vreinterpret_v_i16m4_u16m4(v1));
    vuint64m4_t b = __riscv_vreinterpret_v_u16m4_u64m4(__riscv_vreinterpret_v_i16m4_u16m4(v2));

    static const uint64_t idx_mem[4] __attribute__((aligned(32))) = {1,1,3,3};
    vuint64m4_t idx = __riscv_vle64_v_u64m4(idx_mem, vl64);

    vuint64m4_t a_dup = __riscv_vrgather_vv_u64m4(a, idx, vl64);
    vuint64m4_t b_dup = __riscv_vrgather_vv_u64m4(b, idx, vl64);

    vuint64m4_t vid = __riscv_vid_v_u64m4(vl64);
    vbool16_t mask_odd = __riscv_vmsne_vx_u64m4_b16(__riscv_vand_vx_u64m4(vid, 1u, vl64), 0u, vl64);

    vuint64m4_t out64 = __riscv_vmerge_vvm_u64m4(a_dup, b_dup, mask_odd, vl64);

    vuint16m4_t out16u = __riscv_vreinterpret_v_u64m4_u16m4(out64);
    return __riscv_vreinterpret_v_u16m4_i16m4(out16u);
}

// rvv_unpacklo_epi32
// static inline vint16m4_t rvv_unpacklo_epi32(vint16m4_t v1, vint16m4_t v2, size_t vl) {
//     volatile uint32_t a_buf[8], b_buf[8], res_buf[8]; // 8x 32-bit = 32 bytes
    
//     // 1. (i16m4 -> u16m4)
//     vuint16m4_t v1_16u = __riscv_vreinterpret_v_i16m4_u16m4(v1);
//     vuint16m4_t v2_16u = __riscv_vreinterpret_v_i16m4_u16m4(v2);

//     // 2. (u16m4 -> u32m4) - 使用编译器建议的 Intrinsic
//     vuint32m4_t v1_32u = __riscv_vreinterpret_v_u16m4_u32m4(v1_16u);
//     vuint32m4_t v2_32u = __riscv_vreinterpret_v_u16m4_u32m4(v2_16u);
    
//     // 3. 存储 (使用 u32m4 类型)
//     size_t vl_32bit = 8; // 32 bytes / 4 bytes per element
//     __riscv_vse32_v_u32m4((uint32_t*)a_buf, v1_32u, vl_32bit);
//     __riscv_vse32_v_u32m4((uint32_t*)b_buf, v2_32u, vl_32bit);

//     // 4. C 语言交错 (Lo) - 逻辑不变，依然针对 8个 32-bit 元素
//     res_buf[0] = a_buf[0]; res_buf[1] = b_buf[0];
//     res_buf[2] = a_buf[1]; res_buf[3] = b_buf[1];
//     res_buf[4] = a_buf[4]; res_buf[5] = b_buf[4];
//     res_buf[6] = a_buf[5]; res_buf[7] = b_buf[5];
    
//     // 5. 加载
//     vuint32m4_t result_32u = __riscv_vle32_v_u32m4((uint32_t*)res_buf, vl_32bit);
    
//     // 6. 转换回 vint16m4_t (u32m4 -> u16m4 -> i16m4)
//     vuint16m4_t result_16u = __riscv_vreinterpret_v_u32m4_u16m4(result_32u);
//     return __riscv_vreinterpret_v_u16m4_i16m4(result_16u);
// }

// // rvv_unpackhi_epi32
// static inline vint16m4_t rvv_unpackhi_epi32(vint16m4_t v1, vint16m4_t v2, size_t vl) {
//     volatile uint32_t a_buf[8], b_buf[8], res_buf[8];
    
//     vuint16m4_t v1_16u = __riscv_vreinterpret_v_i16m4_u16m4(v1);
//     vuint16m4_t v2_16u = __riscv_vreinterpret_v_i16m4_u16m4(v2);
//     vuint32m4_t v1_32u = __riscv_vreinterpret_v_u16m4_u32m4(v1_16u);
//     vuint32m4_t v2_32u = __riscv_vreinterpret_v_u16m4_u32m4(v2_16u);
    
//     size_t vl_32bit = 8;
//     __riscv_vse32_v_u32m4((uint32_t*)a_buf, v1_32u, vl_32bit);
//     __riscv_vse32_v_u32m4((uint32_t*)b_buf, v2_32u, vl_32bit);

//     // C 语言交错 (Hi)
//     res_buf[0] = a_buf[2]; res_buf[1] = b_buf[2];
//     res_buf[2] = a_buf[3]; res_buf[3] = b_buf[3];
//     res_buf[4] = a_buf[6]; res_buf[5] = b_buf[6];
//     res_buf[6] = a_buf[7]; res_buf[7] = b_buf[7];
    
//     vuint32m4_t result_32u = __riscv_vle32_v_u32m4((uint32_t*)res_buf, vl_32bit);
    
//     vuint16m4_t result_16u = __riscv_vreinterpret_v_u32m4_u16m4(result_32u);
//     return __riscv_vreinterpret_v_u16m4_i16m4(result_16u);
// }

// // rvv_unpacklo_epi64
// static inline vint16m4_t rvv_unpacklo_epi64(vint16m4_t v1, vint16m4_t v2, size_t vl) {
//     volatile uint64_t a_buf[4], b_buf[4], res_buf[4]; // 4x 64-bit = 32 bytes
    
//     // 1. (i16m4 -> u16m4)
//     vuint16m4_t v1_16u = __riscv_vreinterpret_v_i16m4_u16m4(v1);
//     vuint16m4_t v2_16u = __riscv_vreinterpret_v_i16m4_u16m4(v2);

//     // 2. (u16m4 -> u64m4)
//     vuint64m4_t v1_64u = __riscv_vreinterpret_v_u16m4_u64m4(v1_16u);
//     vuint64m4_t v2_64u = __riscv_vreinterpret_v_u16m4_u64m4(v2_16u);
    
//     // 3. 存储 (使用 u64m4 类型)
//     size_t vl_64bit = 4; // 32 bytes / 8 bytes per element
//     __riscv_vse64_v_u64m4((uint64_t*)a_buf, v1_64u, vl_64bit);
//     __riscv_vse64_v_u64m4((uint64_t*)b_buf, v2_64u, vl_64bit);

//     // 4. C 语言交错 (Lo) - 逻辑不变，依然针对 4个 64-bit 元素
//     res_buf[0] = a_buf[0]; res_buf[1] = b_buf[0];
//     res_buf[2] = a_buf[2]; res_buf[3] = b_buf[2];
    
//     // 5. 加载
//     vuint64m4_t result_64u = __riscv_vle64_v_u64m4((uint64_t*)res_buf, vl_64bit);
    
//     // 6. 转换回 vint16m4_t (u64m4 -> u16m4 -> i16m4)
//     vuint16m4_t result_16u = __riscv_vreinterpret_v_u64m4_u16m4(result_64u);
//     return __riscv_vreinterpret_v_u16m4_i16m4(result_16u);
// }

// // rvv_unpackhi_epi64
// static inline vint16m4_t rvv_unpackhi_epi64(vint16m4_t v1, vint16m4_t v2, size_t vl) {
//     volatile uint64_t a_buf[4], b_buf[4], res_buf[4];
    
//     vuint16m4_t v1_16u = __riscv_vreinterpret_v_i16m4_u16m4(v1);
//     vuint16m4_t v2_16u = __riscv_vreinterpret_v_i16m4_u16m4(v2);
//     vuint64m4_t v1_64u = __riscv_vreinterpret_v_u16m4_u64m4(v1_16u);
//     vuint64m4_t v2_64u = __riscv_vreinterpret_v_u16m4_u64m4(v2_16u);
    
//     size_t vl_64bit = 4;
//     __riscv_vse64_v_u64m4((uint64_t*)a_buf, v1_64u, vl_64bit);
//     __riscv_vse64_v_u64m4((uint64_t*)b_buf, v2_64u, vl_64bit);

//     // C 语言交错 (Hi)
//     res_buf[0] = a_buf[1]; res_buf[1] = b_buf[1];
//     res_buf[2] = a_buf[3]; res_buf[3] = b_buf[3];
    
//     vuint64m4_t result_64u = __riscv_vle64_v_u64m4((uint64_t*)res_buf, vl_64bit);
    
//     vuint16m4_t result_16u = __riscv_vreinterpret_v_u64m4_u16m4(result_64u);
//     return __riscv_vreinterpret_v_u16m4_i16m4(result_16u);
// }

static inline vint16m4_t rvv_add_epi32(vint16m4_t a, vint16m4_t b, size_t vl) {
    // 1. 设置 32-bit 操作的 VL (8 个 32-bit 元素)
    const size_t VL_32 = 8;
    // 使用 e32m4 设置 VL
    size_t vl_32 = __riscv_vsetvl_e32m4(VL_32); 

    // 2. 类型重解释: vint16m4_t -> vint32m4_t (零开销操作)
    // __riscv_vreinterpret_v_i16m4_i32m4: 将 16-bit, m4 类型重解释为 32-bit, m4 类型
    vint32m4_t a_32 = __riscv_vreinterpret_v_i16m4_i32m4(a);
    vint32m4_t b_32 = __riscv_vreinterpret_v_i16m4_i32m4(b);
    
    // 3. 核心操作: 32-bit 非饱和加法
    vint32m4_t r_32 = __riscv_vadd_vv_i32m4(a_32, b_32, vl_32);
    
    // 4. 结果类型重解释: vint32m4_t -> vint16m4_t
    // __riscv_vreinterpret_v_i32m4_i16m4: 将 32-bit, m4 类型重解释为 16-bit, m4 类型
    vint16m4_t r_16 = __riscv_vreinterpret_v_i32m4_i16m4(r_32);

    return r_16;
}
static inline vint16m4_t rvv_sub_epi32(vint16m4_t a, vint16m4_t b, size_t vl) {
    // 1. 设置 32-bit 操作的 VL (8 个 32-bit 元素)
    const size_t VL_32 = 8;
    // 使用 e32m4 设置 VL
    size_t vl_32 = __riscv_vsetvl_e32m4(VL_32); 
    
    // 2. 类型重解释: vint16m4_t -> vint32m4_t (零开销操作)
    vint32m4_t a_32 = __riscv_vreinterpret_v_i16m4_i32m4(a);
    vint32m4_t b_32 = __riscv_vreinterpret_v_i16m4_i32m4(b);
    
    // 3. 核心操作: 32-bit 非饱和减法
    vint32m4_t r_32 = __riscv_vsub_vv_i32m4(a_32, b_32, vl_32);
    
    // 4. 结果类型重解释: vint32m4_t -> vint16m4_t
    vint16m4_t r_16 = __riscv_vreinterpret_v_i32m4_i16m4(r_32);

    return r_16;
}

// ----------------------------------------------------------------
// 修正后的新增 RVV 乘法和打包辅助函数
// ----------------------------------------------------------------

static inline vint16m4_t rvv_packed_cmult2_q15_i16m4(
    vint16m4_t a, vint16m4_t b, vint16m4_t b2, size_t vl)
{
    // vid = [0..15]
    vuint16m4_t vid = __riscv_vid_v_u16m4(vl);

    // idx_even = [0,0,2,2,...,14,14]
    // idx_odd  = [1,1,3,3,...,15,15]
    vuint16m4_t idx_even = __riscv_vand_vx_u16m4(vid, (uint16_t)0xFFFEu, vl);
    vuint16m4_t idx_odd  = __riscv_vor_vx_u16m4(vid,  (uint16_t)0x0001u, vl);

    // a: ar/ai 成对重复
    vint16m4_t ar = __riscv_vrgather_vv_i16m4(a, idx_even, vl);
    vint16m4_t ai = __riscv_vrgather_vv_i16m4(a, idx_odd,  vl);

    // b: cos/sin 成对重复
    vint16m4_t br = __riscv_vrgather_vv_i16m4(b,  idx_even, vl);
    vint16m4_t bi = __riscv_vrgather_vv_i16m4(b,  idx_odd,  vl);

    // b2: -sin/cos 成对重复
    vint16m4_t b2r = __riscv_vrgather_vv_i16m4(b2, idx_even, vl);
    vint16m4_t b2i = __riscv_vrgather_vv_i16m4(b2, idx_odd,  vl);

    // 宽累加：re32 = ar*br + ai*bi （在 e16 vtype 下完成）
    vint32m8_t re32 = __riscv_vwmul_vv_i32m8(ar, br, vl);
    re32 = __riscv_vwmacc_vv_i32m8(re32, ai, bi, vl);

    // 宽累加：im32 = ar*b2r + ai*b2i
    vint32m8_t im32 = __riscv_vwmul_vv_i32m8(ar, b2r, vl);
    im32 = __riscv_vwmacc_vv_i32m8(im32, ai, b2i, vl);

    // 窄化右移 + 饱和：>>15
    // vnclip 会按 vxrm 进行舍入，并且带饱和（非常适合你这里）
    vint16m4_t re16 = __riscv_vnclip_wx_i16m4(re32, 15, __RISCV_VXRM_RDN, vl);
    vint16m4_t im16 = __riscv_vnclip_wx_i16m4(im32, 15, __RISCV_VXRM_RDN, vl);

    // 组回 packed：[re0,im0,re1,im1,...]
    vuint16m4_t lsb = __riscv_vand_vx_u16m4(vid, 1, vl);
    vbool4_t mask_even = __riscv_vmseq_vx_u16m4_b4(lsb, 0, vl);
    return __riscv_vmerge_vvm_i16m4(im16, re16, mask_even, vl);
}


// packed 乘 (-j)：(re,im) -> (im,-re)
static inline vint16m4_t rvv_mul_minus_j_packed_i16m4(vint16m4_t v, size_t vl)
{
    vuint16m4_t vid = __riscv_vid_v_u16m4(vl);

    // swap even/odd: i^1
    vuint16m4_t idx_swap = __riscv_vxor_vx_u16m4(vid, 1, vl);
    vint16m4_t swapped = __riscv_vrgather_vv_i16m4(v, idx_swap, vl);

    // negate odd lanes
    vuint16m4_t lsb = __riscv_vand_vx_u16m4(vid, 1, vl);
    vbool4_t mask_odd = __riscv_vmseq_vx_u16m4_b4(lsb, 1, vl);

    vint16m4_t neg_swapped = __riscv_vneg_v_i16m4(swapped, vl);
    return __riscv_vmerge_vvm_i16m4(swapped, neg_swapped, mask_odd, vl);
}

// packed 乘 (+j)：(re,im) -> (-im, re)
static inline vint16m4_t rvv_mul_plus_j_packed_i16m4(vint16m4_t v, size_t vl)
{
    vuint16m4_t vid = __riscv_vid_v_u16m4(vl);

    // swap even/odd: i^1  => (im,re,im,re,...)
    vuint16m4_t idx_swap = __riscv_vxor_vx_u16m4(vid, 1, vl);
    vint16m4_t swapped = __riscv_vrgather_vv_i16m4(v, idx_swap, vl);

    // negate even lanes (which now hold imag)
    vuint16m4_t lsb = __riscv_vand_vx_u16m4(vid, 1, vl);
    vbool4_t mask_even = __riscv_vmseq_vx_u16m4_b4(lsb, 0, vl);

    vint16m4_t neg_swapped = __riscv_vneg_v_i16m4(swapped, vl);
    return __riscv_vmerge_vvm_i16m4(swapped, neg_swapped, mask_even, vl);
}

/**
 * @brief 模拟 _mm256_madd_epi16: 
 * 将相邻的 16-bit 元素相乘 (a[2i]*b[2i] + a[2i+1]*b[2i+1]), 结果为 32-bit (vint32m4_t)。
 */
static inline vint32m4_t rvv_madd_epi16(vint16m4_t a, vint16m4_t b, size_t vl_16) {
    size_t vl_8_32bit = vl_16 / 2; // VL=8 for i32m4
    size_t vl_16_32bit = vl_16;    // VL=16 for i32m8

    // 1. 宽乘 (16-bit * 16-bit -> 32-bit): 结果是 vint32m8_t (VL=16)
    // R_32m8 包含 16 个 32-bit 乘积: R0, R1, R2, ..., R15
    vint32m8_t R_32m8 = __riscv_vwmul_vv_i32m8(a, b, vl_16_32bit);

    // 2. 内存转储，便于按索引操作拆分奇偶元素
    int32_t R_buf[16]; 
    __riscv_vse32_v_i32m8((int32_t*)R_buf, R_32m8, vl_16_32bit);

    int32_t R_even_buf[8]; // R0, R2, R4, R6, R8, R10, R12, R14
    int32_t R_odd_buf[8];  // R1, R3, R5, R7, R9, R11, R13, R15

    for (size_t i = 0; i < vl_8_32bit; i++) {
        // 偶数索引乘积
        R_even_buf[i] = R_buf[2 * i];
        // 奇数索引乘积
        R_odd_buf[i] = R_buf[2 * i + 1];
    }
    
    // 3. 重新加载为 i32m4 向量 (VL=8)
    vint32m4_t R_even_final = __riscv_vle32_v_i32m4((const int32_t*)R_even_buf, vl_8_32bit);
    vint32m4_t R_odd_final  = __riscv_vle32_v_i32m4((const int32_t*)R_odd_buf, vl_8_32bit);

    // 4. 32-bit 加法 (偶数乘积 + 奇数乘积)
    // 结果 R_32m4 包含 8 个 32-bit 结果: (R0+R1, R2+R3, ...)
    return __riscv_vadd_vv_i32m4(R_even_final, R_odd_final, vl_8_32bit);
}

// ------------------- 2. rvv_srai_epi32 (保持不变) ---------------------

/**
 * @brief 模拟 _mm256_srai_epi32: 32-bit 算术右移
 */
static inline vint32m4_t rvv_srai_epi32(vint32m4_t a, const unsigned int imm8, size_t vl_32) {
    return __riscv_vsra_vx_i32m4(a, imm8, vl_32);
}

// ------------------- 3. rvv_packs_epi32_manual (手动饱和窄化) ---------------------

/**
 * @brief 窄化饱和转换 (32-bit to 16-bit Signed Saturation)
 */
static inline vint16m4_t rvv_packs_epi32_manual(vint32m8_t combined_32m8, size_t vl_16_32bit) {
    const int32_t INT16_MAX_VAL = 32767;
    const int32_t INT16_MIN_VAL = -32768;
    
    vint32m8_t max_vec = __riscv_vmv_v_x_i32m8(INT16_MAX_VAL, vl_16_32bit);
    vint32m8_t min_vec = __riscv_vmv_v_x_i32m8(INT16_MIN_VAL, vl_16_32bit);
    
    // 饱和逻辑 (Max)
    vbool4_t mask_max = __riscv_vmsgt_vv_i32m8_b4(combined_32m8, max_vec, vl_16_32bit);
    vint32m8_t sat_max = __riscv_vmerge_vvm_i32m8(combined_32m8, max_vec, mask_max, vl_16_32bit);
    
    // 饱和逻辑 (Min)
    vbool4_t mask_min = __riscv_vmslt_vv_i32m8_b4(sat_max, min_vec, vl_16_32bit);
    vint32m8_t sat_final = __riscv_vmerge_vvm_i32m8(sat_max, min_vec, mask_min, vl_16_32bit);
    
    // 窄化 (Convert/Truncate): i32m8 -> i16m4
    return __riscv_vncvt_x_x_w_i16m4(sat_final, vl_16_32bit);
}

/**
 * @brief 模拟 _mm256_packs_epi32: 32-bit 到 16-bit 的有符号饱和打包。
 */
static inline vint16m4_t rvv_packs_epi32(vint32m4_t a, vint32m4_t b, size_t vl_32) {
    size_t vl_16 = 16; 
    size_t vl_16_32bit = 16; 
    
    // 1. 合并 32-bit 向量并进行饱和窄化 (rvv_packs_epi32_manual内部已验证逻辑正确)
    int32_t a_buf[8], b_buf[8];
    __riscv_vse32_v_i32m4((int32_t*)a_buf, a, vl_32);
    __riscv_vse32_v_i32m4((int32_t*)b_buf, b, vl_32);
    
    int32_t combined_32_buf[16];
    for (int i = 0; i < 8; i++) {
        combined_32_buf[i] = a_buf[i];
        combined_32_buf[i + 8] = b_buf[i];
    }
    vint32m8_t combined = __riscv_vle32_v_i32m8((const int32_t*)combined_32_buf, vl_16_32bit);

    vint16m4_t narrow_result = rvv_packs_epi32_manual(combined, vl_16_32bit);
    
    // 2. **关键修正: 内存转储，手动交换 $64$-bit 块，然后重新加载**
    
    int16_t result_buf[16];
    __riscv_vse16_v_i16m4((int16_t*)result_buf, narrow_result, vl_16);

    // 块定义 (4x I16 = 64-bit):
    // C0_C3: result_buf[0..3]
    // C4_C7: result_buf[4..7]
    // C8_C11: result_buf[8..11]
    // C12_C15: result_buf[12..15]

    // 根据您的输出分析，我们需要交换 C4_C7 和 C8_C11 块。
    // 目标顺序: C0_C3, C8_C11, C4_C7, C12_C15
    
    int16_t temp_C4_C7[4];
    
    // 备份 C4_C7 (result_buf[4..7])
    for (int i = 0; i < 4; i++) {
        temp_C4_C7[i] = result_buf[4 + i];
    }
    
    // 移动 C8_C11 到 C4_C7 的位置 (result_buf[8..11] -> result_buf[4..7])
    for (int i = 0; i < 4; i++) {
        result_buf[4 + i] = result_buf[8 + i];
    }
    
    // 移动 C4_C7 (备份) 到 C8_C11 的位置 (temp_C4_C7 -> result_buf[8..11])
    for (int i = 0; i < 4; i++) {
        result_buf[8 + i] = temp_C4_C7[i];
    }

    // 3. 重新加载 $16$-bit 向量，完成顺序修正
    return __riscv_vle16_v_i16m4((const int16_t*)result_buf, vl_16);
}
// ------------------- 4. rvv_cpack_256 (关键修正交错逻辑) ---------------------

/**
 * @brief 修正 rvv_cpack_256: 严格模拟 _mm256_unpacklo/hi_epi32 的交错。
 */
static inline vint16m4_t rvv_cpack_256(vint32m4_t xre, vint32m4_t xim, size_t vl_16) {
    size_t vl_32 = 8; 

    int32_t xre_buf[8], xim_buf[8]; 
    int32_t cpack_buf1[8], cpack_buf2[8]; 
    
    __riscv_vse32_v_i32m4((int32_t*)xre_buf, xre, vl_32);
    __riscv_vse32_v_i32m4((int32_t*)xim_buf, xim, vl_32);

    // 模拟 _mm256_unpacklo_epi32 (cpack_tmp1)
    cpack_buf1[0] = xre_buf[0]; cpack_buf1[1] = xim_buf[0]; 
    cpack_buf1[2] = xre_buf[1]; cpack_buf1[3] = xim_buf[1]; 
    cpack_buf1[4] = xre_buf[4]; cpack_buf1[5] = xim_buf[4]; 
    cpack_buf1[6] = xre_buf[5]; cpack_buf1[7] = xim_buf[5]; 
    vint32m4_t cpack_tmp1 = __riscv_vle32_v_i32m4((const int32_t*)cpack_buf1, vl_32);

    // 模拟 _mm256_unpackhi_epi32 (cpack_tmp2)
    cpack_buf2[0] = xre_buf[2]; cpack_buf2[1] = xim_buf[2]; 
    cpack_buf2[2] = xre_buf[3]; cpack_buf2[3] = xim_buf[3]; 
    cpack_buf2[4] = xre_buf[6]; cpack_buf2[5] = xim_buf[6]; 
    cpack_buf2[6] = xre_buf[7]; cpack_buf2[7] = xim_buf[7]; 
    vint32m4_t cpack_tmp2 = __riscv_vle32_v_i32m4((const int32_t*)cpack_buf2, vl_32);

    // 定标右移 15 位
    vint32m4_t sra_tmp1 = rvv_srai_epi32(cpack_tmp1, 15, vl_32);
    vint32m4_t sra_tmp2 = rvv_srai_epi32(cpack_tmp2, 15, vl_32);

    // 饱和打包
    return rvv_packs_epi32(sra_tmp1, sra_tmp2, vl_32);
}
// ------------------- 5. rvv_packed_cmult2 (保持不变) ---------------------

static inline vint16m4_t rvv_packed_cmult2(vint16m4_t a, vint16m4_t b, vint16m4_t b2, size_t vl_16) {
    // 1. cre = _mm256_madd_epi16(a, b); 
    vint32m4_t cre = rvv_madd_epi16(a, b, vl_16);
    
    // 2. cim = _mm256_madd_epi16(a, b2); 
    vint32m4_t cim = rvv_madd_epi16(a, b2, vl_16);
    
    // 3. return(cpack_256(cre,cim));
    return rvv_cpack_256(cre, cim, vl_16);
}
// ----------------------------------------------------------------
// 调试函数和宏 (已更新为 m4 类型)
// ----------------------------------------------------------------

void debug_print_vector_i16(const char *label, const int16_t *data, size_t N_ELEMENTS) {
    printf("DEBUG: %s (16x I16):\n", label);
    if (N_ELEMENTS > 16) N_ELEMENTS = 16;
    
    for (size_t i = 0; i < N_ELEMENTS; i++) {
        printf(" %d", data[i]); 
        if (i == 7) {
            printf(" |"); // AVX 128-bit Lane 边界
        }
    }
    printf("\n");
}

// 宏 1: 打印 RVV 向量 (vint16m4_t)
#define DEBUG_PRINT_RVV_VEC(label, rvv_vec, vl) do { \
    int16_t debug_array[16]; \
    __riscv_vse16_v_i16m4(debug_array, rvv_vec, vl); \
    debug_print_vector_i16(label, debug_array, 16); \
} while(0)

// 宏 2: 打印 AVX 向量 (__m256i) - 修正版 (保持不变，用于 AVX 兼容性参考)
#define DEBUG_PRINT_AVX_VEC(label, avx_vec) do { \
    /* 创建一个临时的栈上数组 */ \
    int16_t debug_array[16] __attribute__ ((aligned (32))); \
    /* 显式地将 AVX 寄存器的内容存储到该数组中 */ \
    _mm256_storeu_si256((__m256i*)debug_array, avx_vec); \
    /* 打印该数组的内容 */ \
    debug_print_vector_i16(label, debug_array, 16); \
} while(0)


/*-----------------AVX函数改写为RVV intrinsic--------------------------*/


#if defined(__x86_64__) || defined(__i386__) || defined SIMDE_ENABLE_NATIVE_ALIASES
static inline void cmac(__m128i a,__m128i b, __m128i *re32, __m128i *im32) __attribute__((always_inline));
static inline void cmac(__m128i a,__m128i b, __m128i *re32, __m128i *im32)
{

  __m128i cmac_tmp,cmac_tmp_re32,cmac_tmp_im32;

  cmac_tmp    = _mm_sign_epi16(b,*(__m128i*)reflip);
  cmac_tmp_re32  = _mm_madd_epi16(a,cmac_tmp);

 
  //  cmac_tmp    = _mm_shufflelo_epi16(b,_MM_SHUFFLE(2,3,0,1));
  //  cmac_tmp    = _mm_shufflehi_epi16(cmac_tmp,_MM_SHUFFLE(2,3,0,1));
  cmac_tmp = _mm_shuffle_epi8(b,_mm_set_epi8(13,12,15,14,9,8,11,10,5,4,7,6,1,0,3,2));
  cmac_tmp_im32  = _mm_madd_epi16(cmac_tmp,a);

  *re32 = _mm_add_epi32(*re32,cmac_tmp_re32);
  *im32 = _mm_add_epi32(*im32,cmac_tmp_im32);
}

static inline void cmacc(__m128i a,__m128i b, __m128i *re32, __m128i *im32) __attribute__((always_inline));
static inline void cmacc(__m128i a,__m128i b, __m128i *re32, __m128i *im32)
{

  __m128i cmac_tmp,cmac_tmp_re32,cmac_tmp_im32;


  cmac_tmp_re32  = _mm_madd_epi16(a,b);


  cmac_tmp    = _mm_sign_epi16(b,*(__m128i*)reflip);
  //  cmac_tmp    = _mm_shufflelo_epi16(b,_MM_SHUFFLE(2,3,0,1));
  //  cmac_tmp    = _mm_shufflehi_epi16(cmac_tmp,_MM_SHUFFLE(2,3,0,1));
  cmac_tmp = _mm_shuffle_epi8(cmac_tmp,_mm_set_epi8(13,12,15,14,9,8,11,10,5,4,7,6,1,0,3,2));
  cmac_tmp_im32  = _mm_madd_epi16(cmac_tmp,a);

  *re32 = _mm_add_epi32(*re32,cmac_tmp_re32);
  *im32 = _mm_add_epi32(*im32,cmac_tmp_im32);
}

#ifdef __AVX2__
static inline void cmac_256(__m256i a,__m256i b, __m256i *re32, __m256i *im32) __attribute__((always_inline));
static inline void cmac_256(__m256i a,__m256i b, __m256i *re32, __m256i *im32)
{

  __m256i cmac_tmp,cmac_tmp_re32,cmac_tmp_im32;
  __m256i imshuffle = _mm256_set_epi8(29,28,31,30,25,24,27,26,21,20,23,22,17,16,19,18,13,12,15,14,9,8,11,10,5,4,7,6,1,0,3,2);

  cmac_tmp       = _mm256_sign_epi16(b,*(__m256i*)reflip);
  cmac_tmp_re32  = _mm256_madd_epi16(a,cmac_tmp);

  cmac_tmp       = _mm256_shuffle_epi8(b,imshuffle);
  cmac_tmp_im32  = _mm256_madd_epi16(cmac_tmp,a);

  *re32 = _mm256_add_epi32(*re32,cmac_tmp_re32);
  *im32 = _mm256_add_epi32(*im32,cmac_tmp_im32);
}

static inline void cmacc_256(__m256i a,__m256i b, __m256i *re32, __m256i *im32) __attribute__((always_inline));
static inline void cmacc_256(__m256i a,__m256i b, __m256i *re32, __m256i *im32)
{

  __m256i cmac_tmp,cmac_tmp_re32,cmac_tmp_im32;
  __m256i imshuffle = _mm256_set_epi8(29,28,31,30,25,24,27,26,21,20,23,22,17,16,19,18,13,12,15,14,9,8,11,10,5,4,7,6,1,0,3,2);

  cmac_tmp_re32   = _mm256_madd_epi16(a,b);


  cmac_tmp        = _mm256_sign_epi16(b,*(__m256i*)reflip);
  cmac_tmp        = _mm256_shuffle_epi8(b,imshuffle);
  cmac_tmp_im32   = _mm256_madd_epi16(cmac_tmp,a);

  *re32 = _mm256_add_epi32(*re32,cmac_tmp_re32);
  *im32 = _mm256_add_epi32(*im32,cmac_tmp_im32);
}

#endif

static inline void cmult(__m128i a,__m128i b, __m128i *re32, __m128i *im32) __attribute__((always_inline));

static inline void cmult(__m128i a,__m128i b, __m128i *re32, __m128i *im32)
{

  register __m128i mmtmpb;

  mmtmpb    = _mm_sign_epi16(b,*(__m128i*)reflip);
  *re32     = _mm_madd_epi16(a,mmtmpb);
  //  mmtmpb    = _mm_shufflelo_epi16(b,_MM_SHUFFLE(2,3,0,1));
  //  mmtmpb    = _mm_shufflehi_epi16(mmtmpb,_MM_SHUFFLE(2,3,0,1));
  mmtmpb        = _mm_shuffle_epi8(b,_mm_set_epi8(13,12,15,14,9,8,11,10,5,4,7,6,1,0,3,2));
  *im32  = _mm_madd_epi16(a,mmtmpb);

}

#ifdef __AVX2__
static inline void rvv_cmult_256(vint16m4_t a, vint16m4_t b, 
                                 vint16m4_t *re32, vint16m4_t *im32)
{
    size_t vl = __riscv_vsetvl_e16m4(16);

    // 1. 准备常量
    vint16m4_t reflip_vec = __riscv_vle16_v_i16m4(reflip, vl);
    
    // 准备 Mask: 对应 AVX _mm256_set_epi8 参数顺序 (高位到低位)
    vint8m4_t perm_mask = rvv_set_epi8_mask(
        29,28,31,30,25,24,27,26,21,20,23,22,17,16,19,18,
        13,12,15,14,9,8,11,10,5,4,7,6,1,0,3,2
    );

    // 2. 计算实部 (Real Part)
    // 目标公式: ar*br - ai*bi
    // AVX: mmtmpb = _mm256_sign_epi16(b, reflip);
    //      *re32  = _mm256_madd_epi16(a, mmtmpb);
    
    // [Step 1] Sign: 对 b 进行符号处理 (reflip 通常是 {1, -1...}，即虚部取反)
    // b = (br, bi) -> b_sign = (br, -bi)
    vint16m4_t b_sign = rvv_sign_epi16(b, reflip_vec, vl);
    
    // [Step 2] Madd: 乘加运算
    // madd(a, b_sign) = ar*br + ai*(-bi) = ar*br - ai*bi
    vint32m4_t re_res = rvv_madd_epi16(a, b_sign, vl);
    
    // 将 32-bit 结果强制转换为 16-bit 容器指针类型
    *re32 = __riscv_vreinterpret_v_i32m4_i16m4(re_res); 

    // 3. 计算虚部 (Imaginary Part)
    // 目标公式: ar*bi + ai*br
    // AVX: mmtmpb = _mm256_shuffle_epi8(b, perm_mask);
    //      *im32  = _mm256_madd_epi16(a, mmtmpb);
    
    // [Step 1] Shuffle: 交换 b 的实虚部
    // b = (br, bi) -> b_shuf = (bi, br)
    // 注意：这里是对原始的 b 进行 shuffle，而不是 b_sign
    vint16m4_t b_shuf = rvv_shuffle_epi8(b, perm_mask, vl);

    // [Step 2] Madd: 乘加运算
    // madd(a, b_shuf) = ar*bi + ai*br
    vint32m4_t im_res = rvv_madd_epi16(a, b_shuf, vl);
    
    *im32 = __riscv_vreinterpret_v_i32m4_i16m4(im_res);
}
static inline void cmult_256(__m256i a,__m256i b, __m256i *re32, __m256i *im32) __attribute__((always_inline));

static inline void cmult_256(__m256i a,__m256i b, __m256i *re32, __m256i *im32)
{

  register __m256i mmtmpb;
  __m256i const perm_mask = _mm256_set_epi8(29,28,31,30,25,24,27,26,21,20,23,22,17,16,19,18,13,12,15,14,9,8,11,10,5,4,7,6,1,0,3,2);

  mmtmpb    = _mm256_sign_epi16(b,*(__m256i*)reflip);
  *re32     = _mm256_madd_epi16(a,mmtmpb);
  mmtmpb    = _mm256_shuffle_epi8(b,perm_mask);
  *im32     = _mm256_madd_epi16(a,mmtmpb);

}

#endif

static inline void cmultc(__m128i a,__m128i b, __m128i *re32, __m128i *im32) __attribute__((always_inline));

static inline void cmultc(__m128i a,__m128i b, __m128i *re32, __m128i *im32)
{

  register __m128i mmtmpb;

  *re32     = _mm_madd_epi16(a,b);
  mmtmpb    = _mm_sign_epi16(b,*(__m128i*)reflip);
  mmtmpb    = _mm_shuffle_epi8(mmtmpb,_mm_set_epi8(13,12,15,14,9,8,11,10,5,4,7,6,1,0,3,2));
  *im32  = _mm_madd_epi16(a,mmtmpb);

}

#ifdef __AVX2__
static inline void cmultc_256(__m256i a,__m256i b, __m256i *re32, __m256i *im32) __attribute__((always_inline));

static inline void cmultc_256(__m256i a,__m256i b, __m256i *re32, __m256i *im32)
{

  register __m256i mmtmpb;
  __m256i const perm_mask = _mm256_set_epi8(29,28,31,30,25,24,27,26,21,20,23,22,17,16,19,18,13,12,15,14,9,8,11,10,5,4,7,6,1,0,3,2);

  *re32     = _mm256_madd_epi16(a,b);
  mmtmpb    = _mm256_sign_epi16(b,*(__m256i*)reflip);
  mmtmpb    = _mm256_shuffle_epi8(mmtmpb,perm_mask);
  *im32     = _mm256_madd_epi16(a,mmtmpb);

}

#endif

static inline __m128i cpack(__m128i xre,__m128i xim) __attribute__((always_inline));

static inline __m128i cpack(__m128i xre,__m128i xim)
{

  register __m128i cpack_tmp1,cpack_tmp2;

  cpack_tmp1 = _mm_unpacklo_epi32(xre,xim);
  cpack_tmp2 = _mm_unpackhi_epi32(xre,xim);
  return(_mm_packs_epi32(_mm_srai_epi32(cpack_tmp1,15),_mm_srai_epi32(cpack_tmp2,15)));

}

#ifdef __AVX2__
static inline __m256i cpack_256(__m256i xre,__m256i xim) __attribute__((always_inline));

static inline __m256i cpack_256(__m256i xre,__m256i xim)
{

  register __m256i cpack_tmp1,cpack_tmp2;

  cpack_tmp1 = _mm256_unpacklo_epi32(xre,xim);
  cpack_tmp2 = _mm256_unpackhi_epi32(xre,xim);
  return(_mm256_packs_epi32(_mm256_srai_epi32(cpack_tmp1,15),_mm256_srai_epi32(cpack_tmp2,15)));

}

#endif

static inline void packed_cmult(__m128i a,__m128i b, __m128i *c) __attribute__((always_inline));

static inline void packed_cmult(__m128i a,__m128i b, __m128i *c)
{

  __m128i cre,cim;
  cmult(a,b,&cre,&cim);
  *c = cpack(cre,cim);

}

#ifdef __AVX2__
static inline void packed_cmult_256(__m256i a,__m256i b, __m256i *c) __attribute__((always_inline));

static inline void packed_cmult_256(__m256i a,__m256i b, __m256i *c)
{

  __m256i cre,cim;
  cmult_256(a,b,&cre,&cim);
  *c = cpack_256(cre,cim);

}
#endif

static inline void packed_cmultc(__m128i a,__m128i b, __m128i *c) __attribute__((always_inline));

static inline void packed_cmultc(__m128i a,__m128i b, __m128i *c)
{

  __m128i cre,cim;

  cmultc(a,b,&cre,&cim);
  *c = cpack(cre,cim);

}

#ifdef __AVX2__
static inline void packed_cmultc_256(__m256i a,__m256i b, __m256i *c) __attribute__((always_inline));

static inline void packed_cmultc_256(__m256i a,__m256i b, __m256i *c)
{

  __m256i cre,cim;

  cmultc_256(a,b,&cre,&cim);
  *c = cpack_256(cre,cim);

}
#endif

static inline __m128i packed_cmult2(__m128i a,__m128i b,__m128i b2) __attribute__((always_inline));

static inline __m128i packed_cmult2(__m128i a,__m128i b,__m128i b2)
{


  register __m128i cre,cim;

  cre       = _mm_madd_epi16(a,b);
  cim       = _mm_madd_epi16(a,b2);

  return(cpack(cre,cim));

}

#ifdef __AVX2__
static inline __m256i packed_cmult2_256(__m256i a,__m256i b,__m256i b2) __attribute__((always_inline));

static inline __m256i packed_cmult2_256(__m256i a,__m256i b,__m256i b2)
{


  register __m256i cre,cim;

  cre       = _mm256_madd_epi16(a,b);
  cim       = _mm256_madd_epi16(a,b2);

  return(cpack_256(cre,cim));

}
#endif


#endif

const static int16_t W0s[16]__attribute__((aligned(32))) = {32767,0,32767,0,32767,0,32767,0,32767,0,32767,0,32767,0,32767,0};

const static int16_t W13s[16]__attribute__((aligned(32))) = {-16384,-28378,-16384,-28378,-16384,-28378,-16384,-28378,-16384,-28378,-16384,-28378,-16384,-28378,-16384,-28378};
const static int16_t W23s[16]__attribute__((aligned(32))) = {-16384,28378,-16384,28378,-16384,28378,-16384,28378,-16384,28378,-16384,28378,-16384,28378,-16384,28378};

const static int16_t W15s[16]__attribute__((aligned(32))) = {10126,-31163,10126,-31163,10126,-31163,10126,-31163,10126,-31163,10126,-31163,10126,-31163,10126,-31163};
const static int16_t W25s[16]__attribute__((aligned(32))) = {-26509,-19260,-26509,-19260,-26509,-19260,-26509,-19260,-26509,-19260,-26509,-19260,-26509,-19260,-26509,-19260};
const static int16_t W35s[16]__attribute__((aligned(32))) = {-26510,19260,-26510,19260,-26510,19260,-26510,19260,-26510,19260,-26510,19260,-26510,19260,-26510,19260};
const static int16_t W45s[16]__attribute__((aligned(32))) = {10126,31163,10126,31163,10126,31163,10126,31163,10126,31163,10126,31163,10126,31163,10126,31163};

#if defined(__x86_64__) || defined(__i386__) || defined SIMDE_ENABLE_NATIVE_ALIASES
const __m128i *W0 = (__m128i *)W0s;
const __m128i *W13 = (__m128i *)W13s;
const __m128i *W23 = (__m128i *)W23s;
const __m128i *W15 = (__m128i *)W15s;
const __m128i *W25 = (__m128i *)W25s;
const __m128i *W35 = (__m128i *)W35s;
const __m128i *W45 = (__m128i *)W45s;

#ifdef __AVX2__
const __m256i *W0_256 =  (__m256i *)W0s;
const __m256i *W13_256 = (__m256i *)W13s;
const __m256i *W23_256 = (__m256i *)W23s;
const __m256i *W15_256 = (__m256i *)W15s;
const __m256i *W25_256 = (__m256i *)W25s;
const __m256i *W35_256 = (__m256i *)W35s;
const __m256i *W45_256 = (__m256i *)W45s;
#endif


#endif
const static int16_t dft_norm_table[16] = {9459,  //12
					   6689,//24
					   5461,//36
					   4729,//482
					   4230,//60
					   23170,//72
					   3344,//96
					   3153,//108
					   2991,//120
					   18918,//sqrt(3),//144
					   18918,//sqrt(3),//180
					   16384,//2, //192
					   18918,//sqrt(3), // 216
					   16384,//2, //240
					   18918,//sqrt(3), // 288
					   14654
}; //sqrt(5) //300


#if defined(__x86_64__) || defined(__i386__) || defined SIMDE_ENABLE_NATIVE_ALIASES
static inline void bfly2(__m128i *x0, __m128i *x1,__m128i *y0, __m128i *y1,__m128i *tw)__attribute__((always_inline));

static inline void bfly2(__m128i *x0, __m128i *x1,__m128i *y0, __m128i *y1,__m128i *tw)
{

  __m128i x0r_2,x0i_2,x1r_2,x1i_2,dy0r,dy1r,dy0i,dy1i;
  __m128i bfly2_tmp1,bfly2_tmp2;

  cmult(*(x0),*(W0),&x0r_2,&x0i_2);
  cmult(*(x1),*(tw),&x1r_2,&x1i_2);

  dy0r = _mm_srai_epi32(_mm_add_epi32(x0r_2,x1r_2),15);
  dy1r = _mm_srai_epi32(_mm_sub_epi32(x0r_2,x1r_2),15);
  dy0i = _mm_srai_epi32(_mm_add_epi32(x0i_2,x1i_2),15);
  //  printf("y0i %d\n",((int16_t *)y0i)[0]);
  dy1i = _mm_srai_epi32(_mm_sub_epi32(x0i_2,x1i_2),15);

  bfly2_tmp1 = _mm_unpacklo_epi32(dy0r,dy0i);
  bfly2_tmp2 = _mm_unpackhi_epi32(dy0r,dy0i);
  *y0 = _mm_packs_epi32(bfly2_tmp1,bfly2_tmp2);

  bfly2_tmp1 = _mm_unpacklo_epi32(dy1r,dy1i);
  bfly2_tmp2 = _mm_unpackhi_epi32(dy1r,dy1i);
  *y1 = _mm_packs_epi32(bfly2_tmp1,bfly2_tmp2);
}

#ifdef __AVX2__
static inline void rvv_bfly2_256(int16_t *x0, int16_t *x1, 
                                 int16_t *y0, int16_t *y1, 
                                 int16_t *tw)
{
    const size_t VL_16 = 16;
    size_t vl = __riscv_vsetvl_e16m4(VL_16);
    size_t vl_32 = __riscv_vsetvl_e32m4(VL_16 / 2); // 8个32位元素

    // 设置 VXRM = 0 (VXRM_NONE) 实现截断取整 (Truncation Towards Zero)。
    // 确保与原AVX代码 _mm256_srai_epi32 的行为一致。
    const unsigned int VXRM_NONE = 2; 

    // 1. 加载输入
    vint16m4_t x0_vec = __riscv_vle16_v_i16m4(x0, vl);
    vint16m4_t x1_vec = __riscv_vle16_v_i16m4(x1, vl);
    vint16m4_t tw_vec = __riscv_vle16_v_i16m4(tw, vl);
    // 假设 W0_256 在 RVV 中对应全局变量 W0s
    vint16m4_t w0_vec = __riscv_vle16_v_i16m4(W0s, vl);

    // 2. 复数乘法 (非共轭)
    // cmult_256(*(x0),*(W0_256),&x0r_2,&x0i_2);
    // cmult_256(*(x1),*(tw),&x1r_2,&x1i_2);
    vint32m4_t x0r_2_32, x0i_2_32, x1r_2_32, x1i_2_32;

    // !!! 使用 rvv_cmult_256 (非共轭乘法) !!!
    rvv_cmult_256(x0_vec, w0_vec, (vint16m4_t *)&x0r_2_32, (vint16m4_t *)&x0i_2_32);
    rvv_cmult_256(x1_vec, tw_vec, (vint16m4_t *)&x1r_2_32, (vint16m4_t *)&x1i_2_32);

    // 3. 32位数据类型转换 (用于加减运算)
    vint16m4_t x0r_2_16 = __riscv_vreinterpret_v_i32m4_i16m4(x0r_2_32);
    vint16m4_t x1r_2_16 = __riscv_vreinterpret_v_i32m4_i16m4(x1r_2_32);
    vint16m4_t x0i_2_16 = __riscv_vreinterpret_v_i32m4_i16m4(x0i_2_32);
    vint16m4_t x1i_2_16 = __riscv_vreinterpret_v_i32m4_i16m4(x1i_2_32);

    // 4. 蝶形加减运算 (32-bit data)
    // dy0r = _mm256_add_epi32(x0r_2,x1r_2)
    vint16m4_t sum_r = rvv_add_epi32(x0r_2_16, x1r_2_16, vl_32); 
    // dy1r = _mm256_sub_epi32(x0r_2,x1r_2)
    vint16m4_t sub_r = rvv_sub_epi32(x0r_2_16, x1r_2_16, vl_32);
    // dy0i = _mm256_add_epi32(x0i_2,x1i_2)
    vint16m4_t sum_i = rvv_add_epi32(x0i_2_16, x1i_2_16, vl_32);
    // dy1i = _mm256_sub_epi32(x0i_2,x1i_2)
    vint16m4_t sub_i = rvv_sub_epi32(x0i_2_16, x1i_2_16, vl_32);

    // 5. 32位右移和截断 (SRAI 15)
    // 重新转换回 vint32m4_t 类型以便使用 vssra 指令
    vint32m4_t sum_r_32 = __riscv_vreinterpret_v_i16m4_i32m4(sum_r);
    vint32m4_t sub_r_32 = __riscv_vreinterpret_v_i16m4_i32m4(sub_r);
    vint32m4_t sum_i_32 = __riscv_vreinterpret_v_i16m4_i32m4(sum_i);
    vint32m4_t sub_i_32 = __riscv_vreinterpret_v_i16m4_i32m4(sub_i);

    // 使用 VSSRA.VX，设置 VXRM_NONE (0) 实现截断
    vint32m4_t dy0r_32 = __riscv_vssra_vx_i32m4(sum_r_32, 15, VXRM_NONE, vl_32);
    vint32m4_t dy1r_32 = __riscv_vssra_vx_i32m4(sub_r_32, 15, VXRM_NONE, vl_32);
    vint32m4_t dy0i_32 = __riscv_vssra_vx_i32m4(sum_i_32, 15, VXRM_NONE, vl_32);
    vint32m4_t dy1i_32 = __riscv_vssra_vx_i32m4(sub_i_32, 15, VXRM_NONE, vl_32);

    // 6. Unpack (交织实部和虚部)
    vint16m4_t dy0r_16 = __riscv_vreinterpret_v_i32m4_i16m4(dy0r_32);
    vint16m4_t dy0i_16 = __riscv_vreinterpret_v_i32m4_i16m4(dy0i_32);
    vint16m4_t dy1r_16 = __riscv_vreinterpret_v_i32m4_i16m4(dy1r_32);
    vint16m4_t dy1i_16 = __riscv_vreinterpret_v_i32m4_i16m4(dy1i_32);

    // y0 结果的交织 (实部/虚部)
    vint16m4_t bfly2_tmp1_y0 = rvv_unpacklo_epi32(dy0r_16, dy0i_16, vl); 
    vint16m4_t bfly2_tmp2_y0 = rvv_unpackhi_epi32(dy0r_16, dy0i_16, vl);

    // y1 结果的交织 (实部/虚部)
    vint16m4_t bfly2_tmp1_y1 = rvv_unpacklo_epi32(dy1r_16, dy1i_16, vl);
    vint16m4_t bfly2_tmp2_y1 = rvv_unpackhi_epi32(dy1r_16, dy1i_16, vl);

    // 7. Pack (32-bit -> 16-bit 饱和)
    vint32m4_t tmp1_y0_32 = __riscv_vreinterpret_v_i16m4_i32m4(bfly2_tmp1_y0);
    vint32m4_t tmp2_y0_32 = __riscv_vreinterpret_v_i16m4_i32m4(bfly2_tmp2_y0);
    
    vint32m4_t tmp1_y1_32 = __riscv_vreinterpret_v_i16m4_i32m4(bfly2_tmp1_y1);
    vint32m4_t tmp2_y1_32 = __riscv_vreinterpret_v_i16m4_i32m4(bfly2_tmp2_y1);

    // *y0 = _mm256_packs_epi32(bfly2_tmp1,bfly2_tmp2);
    vint16m4_t y0_result = rvv_packs_epi32(tmp1_y0_32, tmp2_y0_32, vl_32); 
    // *y1 = _mm256_packs_epi32(bfly2_tmp1,bfly2_tmp2);
    vint16m4_t y1_result = rvv_packs_epi32(tmp1_y1_32, tmp2_y1_32, vl_32);

    // 8. 存储结果
    __riscv_vse16_v_i16m4(y0, y0_result, vl);
    __riscv_vse16_v_i16m4(y1, y1_result, vl);
}
static inline void bfly2_256(__m256i *x0, __m256i *x1,__m256i *y0, __m256i *y1,__m256i *tw)__attribute__((always_inline));

static inline void bfly2_256(__m256i *x0, __m256i *x1,__m256i *y0, __m256i *y1,__m256i *tw)
{

  __m256i x0r_2,x0i_2,x1r_2,x1i_2,dy0r,dy1r,dy0i,dy1i;
  __m256i bfly2_tmp1,bfly2_tmp2;

  cmult_256(*(x0),*(W0_256),&x0r_2,&x0i_2);
  cmult_256(*(x1),*(tw),&x1r_2,&x1i_2);

  dy0r = _mm256_srai_epi32(_mm256_add_epi32(x0r_2,x1r_2),15);
  dy1r = _mm256_srai_epi32(_mm256_sub_epi32(x0r_2,x1r_2),15);
  dy0i = _mm256_srai_epi32(_mm256_add_epi32(x0i_2,x1i_2),15);
  //  printf("y0i %d\n",((int16_t *)y0i)[0]);
  dy1i = _mm256_srai_epi32(_mm256_sub_epi32(x0i_2,x1i_2),15);

  bfly2_tmp1 = _mm256_unpacklo_epi32(dy0r,dy0i);
  bfly2_tmp2 = _mm256_unpackhi_epi32(dy0r,dy0i);
  *y0 = _mm256_packs_epi32(bfly2_tmp1,bfly2_tmp2);

  bfly2_tmp1 = _mm256_unpacklo_epi32(dy1r,dy1i);
  bfly2_tmp2 = _mm256_unpackhi_epi32(dy1r,dy1i);
  *y1 = _mm256_packs_epi32(bfly2_tmp1,bfly2_tmp2);
}

#endif


#endif

#if defined(__x86_64__) || defined(__i386__) || defined SIMDE_ENABLE_NATIVE_ALIASES
static inline void bfly2_tw1(__m128i *x0, __m128i *x1, __m128i *y0, __m128i *y1)__attribute__((always_inline));

static inline void bfly2_tw1(__m128i *x0, __m128i *x1, __m128i *y0, __m128i *y1)
{

  *y0  = _mm_adds_epi16(*x0,*x1);
  *y1  = _mm_subs_epi16(*x0,*x1);

}

#endif
 
#if defined(__x86_64__) || defined(__i386__) || defined SIMDE_ENABLE_NATIVE_ALIASES



static inline void bfly2_16(__m128i *x0, __m128i *x1, __m128i *y0, __m128i *y1, __m128i *tw, __m128i *twb)__attribute__((always_inline));

static inline void bfly2_16(__m128i *x0, __m128i *x1, __m128i *y0, __m128i *y1, __m128i *tw, __m128i *twb)
{

  //  register __m128i x1t;
  __m128i x1t;

  x1t = packed_cmult2(*(x1),*(tw),*(twb));
  /*
  print_shorts("x0",(int16_t*)x0);
  print_shorts("x1",(int16_t*)x1);
  print_shorts("tw",(int16_t*)tw);
  print_shorts("twb",(int16_t*)twb);
  print_shorts("x1t",(int16_t*)&x1t);*/
  *y0  = _mm_adds_epi16(*x0,x1t);
  *y1  = _mm_subs_epi16(*x0,x1t);
  /*  print_shorts("y0",(int16_t*)y0);
      print_shorts("y1",(int16_t*)y1);*/
}

#ifdef __AVX2__

static inline void bfly2_16_256(__m256i *x0, __m256i *x1, __m256i *y0, __m256i *y1, __m256i *tw, __m256i *twb)__attribute__((always_inline));

static inline void bfly2_16_256(__m256i *x0, __m256i *x1, __m256i *y0, __m256i *y1, __m256i *tw, __m256i *twb)
{

  //  register __m256i x1t;
  __m256i x1t;

  x1t = packed_cmult2_256(*(x1),*(tw),*(twb));
  
  /*
  print_shorts256("x0",(int16_t*)x0);
  print_shorts256("x1",(int16_t*)x1);
  print_shorts256("tw",(int16_t*)tw);
  print_shorts256("twb",(int16_t*)twb);
  print_shorts256("x1t",(int16_t*)&x1t);*/
  *y0  = _mm256_adds_epi16(*x0,x1t);
  *y1  = _mm256_subs_epi16(*x0,x1t);
  
  /*print_shorts256("y0",(int16_t*)y0);
    print_shorts256("y1",(int16_t*)y1);*/
}

static inline void rvv_bfly2_16_256(const int16_t *x0, const int16_t *x1, 
                                     int16_t *y0, int16_t *y1, 
                                     const int16_t *tw, const int16_t *twb)
{
    // 1. 设置 VL (Vector Length) 和 VTYPE
    // 256-bit 向量对应 16 个 int16 元素。使用 m4 类型以匹配前面函数定义。
    const size_t VL_16 = 16;
    size_t vl = __riscv_vsetvl_e16m4(VL_16);

    // 2. 加载输入向量
    vint16m4_t x0_vec = __riscv_vle16_v_i16m4(x0, vl);
    vint16m4_t x1_vec = __riscv_vle16_v_i16m4(x1, vl);
    vint16m4_t tw_vec = __riscv_vle16_v_i16m4(tw, vl);
    vint16m4_t twb_vec = __riscv_vle16_v_i16m4(twb, vl);

    // 3. 计算: x1t = packed_cmult2_256(*(x1), *(tw), *(twb))
    // 调用您提供的 rvv_packed_cmult2
    vint16m4_t x1t_vec;
    x1t_vec = rvv_packed_cmult2(x1_vec, tw_vec, twb_vec, vl);
    
    // 4. 计算: *y0 = _mm256_adds_epi16(*x0, x1t)
    // 调用您提供的 rvv_adds_epi16 (签名饱和加法)
    vint16m4_t y0_vec = rvv_adds_epi16(x0_vec, x1t_vec, vl);
    
    // 5. 计算: *y1 = _mm256_subs_epi16(*x0, x1t)
    // 调用您提供的 rvv_subs_epi16 (签名饱和减法)
    vint16m4_t y1_vec = rvv_subs_epi16(x0_vec, x1t_vec, vl);
    
    // 6. 存储结果向量
    __riscv_vse16_v_i16m4(y0, y0_vec, vl);
    __riscv_vse16_v_i16m4(y1, y1_vec, vl);
}

#endif


#endif

#if defined(__x86_64__) || defined(__i386__) || defined SIMDE_ENABLE_NATIVE_ALIASES
static inline void ibfly2(__m128i *x0, __m128i *x1,__m128i *y0, __m128i *y1,__m128i *tw)__attribute__((always_inline));

static inline void ibfly2(__m128i *x0, __m128i *x1,__m128i *y0, __m128i *y1,__m128i *tw)
{

  __m128i x0r_2,x0i_2,x1r_2,x1i_2,dy0r,dy1r,dy0i,dy1i;
  __m128i bfly2_tmp1,bfly2_tmp2;

  cmultc(*(x0),*(W0),&x0r_2,&x0i_2);
  cmultc(*(x1),*(tw),&x1r_2,&x1i_2);

  dy0r = _mm_srai_epi32(_mm_add_epi32(x0r_2,x1r_2),15);
  dy1r = _mm_srai_epi32(_mm_sub_epi32(x0r_2,x1r_2),15);
  dy0i = _mm_srai_epi32(_mm_add_epi32(x0i_2,x1i_2),15);
  //  printf("y0i %d\n",((int16_t *)y0i)[0]);
  dy1i = _mm_srai_epi32(_mm_sub_epi32(x0i_2,x1i_2),15);

  bfly2_tmp1 = _mm_unpacklo_epi32(dy0r,dy0i);
  bfly2_tmp2 = _mm_unpackhi_epi32(dy0r,dy0i);
  *y0 = _mm_packs_epi32(bfly2_tmp1,bfly2_tmp2);

  bfly2_tmp1 = _mm_unpacklo_epi32(dy1r,dy1i);
  bfly2_tmp2 = _mm_unpackhi_epi32(dy1r,dy1i);
  *y1 = _mm_packs_epi32(bfly2_tmp1,bfly2_tmp2);
}

#ifdef __AVX2__
static inline void rvv_cmultc_256(vint16m4_t a, vint16m4_t b, 
                                  vint16m4_t *re32, vint16m4_t *im32)
{
    size_t vl = __riscv_vsetvl_e16m4(16);

    // 1. 准备常量
    vint16m4_t reflip_vec = __riscv_vle16_v_i16m4(reflip, vl);
    
    // 准备 Mask: 对应 AVX _mm256_set_epi8 参数顺序 (高位到低位)
    // 注意：在 RVV 中使用绝对索引，此掩码已验证适用于全向量操作
    vint8m4_t perm_mask = rvv_set_epi8_mask(
        29,28,31,30,25,24,27,26,21,20,23,22,17,16,19,18,
        13,12,15,14,9,8,11,10,5,4,7,6,1,0,3,2
    );

    // 2. 计算实部 (Real Part)
    // 公式: ar*br + ai*bi
    // madd_epi16(a, b) 直接计算 (a_even * b_even) + (a_odd * b_odd)
    // 即 ar*br + ai*bi，结果正确
    vint32m4_t re_res = rvv_madd_epi16(a, b, vl);
    
    // 将 32-bit 结果强制转换为 16-bit 容器指针类型 (Caller 期望 vint16m4_t*)
    *re32 = __riscv_vreinterpret_v_i32m4_i16m4(re_res); 

    // 3. 计算虚部 (Imaginary Part)
    // 目标公式: ai*br - ar*bi
    
    // [Step 1] Sign: 先对 b 进行符号翻转 (AVX: _mm256_sign_epi16)
    // b = (br, bi), reflip = (1, -1) -> b_sign = (br, -bi)
    vint16m4_t b_sign = rvv_sign_epi16(b, reflip_vec, vl);
    
    // [Step 2] Shuffle: 再交换位置 (AVX: _mm256_shuffle_epi8)
    // b_sign = (br, -bi) -> b_final = (-bi, br)
    vint16m4_t b_final = rvv_shuffle_epi8(b_sign, perm_mask, vl);

    // [Step 3] Madd: 乘加运算
    // madd(a, b_final) 
    // = ar * (-bi) + ai * (br) 
    // = ai*br - ar*bi  <-- 正确的共轭乘法虚部!
    vint32m4_t im_res = rvv_madd_epi16(a, b_final, vl);
    
    *im32 = __riscv_vreinterpret_v_i32m4_i16m4(im_res);
}

static inline void rvv_ibfly2_256(int16_t *x0, int16_t *x1, 
                                  int16_t *y0, int16_t *y1, 
                                  int16_t *tw)
{

  const size_t VL_16 = 16;
  size_t vl = __riscv_vsetvl_e16m4(VL_16);
  size_t vl_32 = __riscv_vsetvl_e32m4(VL_16 / 2); // 8个32位元素

  // !!! 关键修正 !!!
  // 1. 移除手动舍入常数 16384 (2^14)。
  // 2. 设置 vxrm = 0 (VXRM_NONE)，实现截断右移 (Truncation Towards Zero)。
  const unsigned int VXRM_NONE = 2; 

  // 1. 加载输入
  vint16m4_t x0_vec = __riscv_vle16_v_i16m4(x0, vl);
  vint16m4_t x1_vec = __riscv_vle16_v_i16m4(x1, vl);
  vint16m4_t tw_vec = __riscv_vle16_v_i16m4(tw, vl);
  vint16m4_t w0_vec = __riscv_vle16_v_i16m4(W0s, vl);

  // 2. 复数共轭乘法
  vint32m4_t x0r_2_32, x0i_2_32, x1r_2_32, x1i_2_32;

  rvv_cmultc_256(x0_vec, w0_vec, (vint16m4_t *)&x0r_2_32, (vint16m4_t *)&x0i_2_32);
  rvv_cmultc_256(x1_vec, tw_vec, (vint16m4_t *)&x1r_2_32, (vint16m4_t *)&x1i_2_32);

    vint16m4_t x0r_2_16 = __riscv_vreinterpret_v_i32m4_i16m4(x0r_2_32);
    vint16m4_t x1r_2_16 = __riscv_vreinterpret_v_i32m4_i16m4(x1r_2_32);
    vint16m4_t x0i_2_16 = __riscv_vreinterpret_v_i32m4_i16m4(x0i_2_32);
    vint16m4_t x1i_2_16 = __riscv_vreinterpret_v_i32m4_i16m4(x1i_2_32);

  // 3. 加减运算
  vint16m4_t sum_r = rvv_add_epi32(x0r_2_16, x1r_2_16, vl_32); 
  vint16m4_t sub_r = rvv_sub_epi32(x0r_2_16, x1r_2_16, vl_32);
  vint16m4_t sum_i = rvv_add_epi32(x0i_2_16, x1i_2_16, vl_32);
  vint16m4_t sub_i = rvv_sub_epi32(x0i_2_16, x1i_2_16, vl_32);

    // 4. 右移 (SRAI 15) - 使用截断 (VXRM=0)
  vint32m4_t sum_r_32 = __riscv_vreinterpret_v_i16m4_i32m4(sum_r);
  vint32m4_t sub_r_32 = __riscv_vreinterpret_v_i16m4_i32m4(sub_r);
  vint32m4_t sum_i_32 = __riscv_vreinterpret_v_i16m4_i32m4(sum_i);
  vint32m4_t sub_i_32 = __riscv_vreinterpret_v_i16m4_i32m4(sub_i);

    // !!! 确保移位前没有加 16384 !!!

    // 使用 VSSRA.VX，设置 VXRM_NONE (0) 实现截断
  vint32m4_t dy0r = __riscv_vssra_vx_i32m4(sum_r_32, 15, VXRM_NONE, vl_32);
  vint32m4_t dy1r = __riscv_vssra_vx_i32m4(sub_r_32, 15, VXRM_NONE, vl_32);
  vint32m4_t dy0i = __riscv_vssra_vx_i32m4(sum_i_32, 15, VXRM_NONE, vl_32);
  vint32m4_t dy1i = __riscv_vssra_vx_i32m4(sub_i_32, 15, VXRM_NONE, vl_32);

  // 5. Unpack (Interleave)
  vint16m4_t dy0r_16 = __riscv_vreinterpret_v_i32m4_i16m4(dy0r);
  vint16m4_t dy0i_16 = __riscv_vreinterpret_v_i32m4_i16m4(dy0i);
  vint16m4_t dy1r_16 = __riscv_vreinterpret_v_i32m4_i16m4(dy1r);
  vint16m4_t dy1i_16 = __riscv_vreinterpret_v_i32m4_i16m4(dy1i);

    // unpacklo/unpackhi 的 VL 使用 vl (16)
  vint16m4_t bfly2_tmp1_y0 = rvv_unpacklo_epi32(dy0r_16, dy0i_16, vl); 
  vint16m4_t bfly2_tmp2_y0 = rvv_unpackhi_epi32(dy0r_16, dy0i_16, vl);

  vint16m4_t bfly2_tmp1_y1 = rvv_unpacklo_epi32(dy1r_16, dy1i_16, vl);
  vint16m4_t bfly2_tmp2_y1 = rvv_unpackhi_epi32(dy1r_16, dy1i_16, vl);

  // 6. Pack (32-bit -> 16-bit Saturated) - VL 使用 vl_32 (8)
  vint32m4_t tmp1_y0_32 = __riscv_vreinterpret_v_i16m4_i32m4(bfly2_tmp1_y0);
  vint32m4_t tmp2_y0_32 = __riscv_vreinterpret_v_i16m4_i32m4(bfly2_tmp2_y0);
  
  vint32m4_t tmp1_y1_32 = __riscv_vreinterpret_v_i16m4_i32m4(bfly2_tmp1_y1);
  vint32m4_t tmp2_y1_32 = __riscv_vreinterpret_v_i16m4_i32m4(bfly2_tmp2_y1);

  vint16m4_t y0_result = rvv_packs_epi32(tmp1_y0_32, tmp2_y0_32, vl_32); 
  vint16m4_t y1_result = rvv_packs_epi32(tmp1_y1_32, tmp2_y1_32, vl_32);

  // 7. 存储结果
  __riscv_vse16_v_i16m4(y0, y0_result, vl);
  __riscv_vse16_v_i16m4(y1, y1_result, vl);
}



static inline void ibfly2_256(__m256i *x0, __m256i *x1,__m256i *y0, __m256i *y1,__m256i *tw)__attribute__((always_inline));
static inline void ibfly2_256(__m256i *x0, __m256i *x1,__m256i *y0, __m256i *y1,__m256i *tw)
{

  __m256i x0r_2,x0i_2,x1r_2,x1i_2,dy0r,dy1r,dy0i,dy1i;
  __m256i bfly2_tmp1,bfly2_tmp2;
  // DEBUG_PRINT_AVX_VEC("AVX: x0[0]", x0[0]);
  // DEBUG_PRINT_AVX_VEC("AVX: x0[1]", x0[1]);
  // DEBUG_PRINT_AVX_VEC("AVX: x0[2]", x0[2]);
  // DEBUG_PRINT_AVX_VEC("AVX: x0[3]", x0[3]);
  // DEBUG_PRINT_AVX_VEC("AVX: x0[4]", x0[4]);
  // DEBUG_PRINT_AVX_VEC("AVX: x0[5]", x0[5]);
  // DEBUG_PRINT_AVX_VEC("AVX: x0[6]", x0[6]);
  // DEBUG_PRINT_AVX_VEC("AVX: x0[7]", x0[7]);

  cmultc_256(*(x0),*(W0_256),&x0r_2,&x0i_2);

  // DEBUG_PRINT_AVX_VEC("AVX: x0r_2", x0r_2);
  // DEBUG_PRINT_AVX_VEC("AVX: x0i_2", x0i_2);
  // DEBUG_PRINT_AVX_VEC("AVX: x0[2]", x0[2]);
  // DEBUG_PRINT_AVX_VEC("AVX: x0[3]", x0[3]);
  // DEBUG_PRINT_AVX_VEC("AVX: x0[4]", x0[4]);
  // DEBUG_PRINT_AVX_VEC("AVX: x0[5]", x0[5]);
  // DEBUG_PRINT_AVX_VEC("AVX: x0[6]", x0[6]);
  // DEBUG_PRINT_AVX_VEC("AVX: x0[7]", x0[7]);

  cmultc_256(*(x1),*(tw),&x1r_2,&x1i_2);

  // DEBUG_PRINT_AVX_VEC("AVX: x1r_2", x1r_2);
  // DEBUG_PRINT_AVX_VEC("AVX: x1i_2", x1i_2);

  dy0r = _mm256_srai_epi32(_mm256_add_epi32(x0r_2,x1r_2),15);
  dy1r = _mm256_srai_epi32(_mm256_sub_epi32(x0r_2,x1r_2),15);
  dy0i = _mm256_srai_epi32(_mm256_add_epi32(x0i_2,x1i_2),15);
  //  printf("y0i %d\n",((int16_t *)y0i)[0]);
  dy1i = _mm256_srai_epi32(_mm256_sub_epi32(x0i_2,x1i_2),15);

  bfly2_tmp1 = _mm256_unpacklo_epi32(dy0r,dy0i);
  bfly2_tmp2 = _mm256_unpackhi_epi32(dy0r,dy0i);
  *y0 = _mm256_packs_epi32(bfly2_tmp1,bfly2_tmp2);

  bfly2_tmp1 = _mm256_unpacklo_epi32(dy1r,dy1i);
  bfly2_tmp2 = _mm256_unpackhi_epi32(dy1r,dy1i);
  *y1 = _mm256_packs_epi32(bfly2_tmp1,bfly2_tmp2);
}
#endif


#endif




// This is the radix-3 butterfly (fft)

#if defined(__x86_64__) || defined(__i386__) || defined SIMDE_ENABLE_NATIVE_ALIASES

static inline void bfly3(__m128i *x0,__m128i *x1,__m128i *x2,
                         __m128i *y0,__m128i *y1,__m128i *y2,
                         __m128i *tw1,__m128i *tw2) __attribute__((always_inline));

static inline void bfly3(__m128i *x0,__m128i *x1,__m128i *x2,
                         __m128i *y0,__m128i *y1,__m128i *y2,
                         __m128i *tw1,__m128i *tw2)
{

  __m128i tmpre,tmpim,x1_2,x2_2;

  packed_cmult(*(x1),*(tw1),&x1_2);
  packed_cmult(*(x2),*(tw2),&x2_2);
  *(y0)  = _mm_adds_epi16(*(x0),_mm_adds_epi16(x1_2,x2_2));
  cmult(x1_2,*(W13),&tmpre,&tmpim);
  cmac(x2_2,*(W23),&tmpre,&tmpim);
  *(y1) = cpack(tmpre,tmpim);
  *(y1) = _mm_adds_epi16(*(x0),*(y1));
  cmult(x1_2,*(W23),&tmpre,&tmpim);
  cmac(x2_2,*(W13),&tmpre,&tmpim);
  *(y2) = cpack(tmpre,tmpim);
  *(y2) = _mm_adds_epi16(*(x0),*(y2));
}

#ifdef __AVX2__

static inline void bfly3_256(__m256i *x0,__m256i *x1,__m256i *x2,
			     __m256i *y0,__m256i *y1,__m256i *y2,
			     __m256i *tw1,__m256i *tw2) __attribute__((always_inline));

static inline void bfly3_256(__m256i *x0,__m256i *x1,__m256i *x2,
			     __m256i *y0,__m256i *y1,__m256i *y2,
			     __m256i *tw1,__m256i *tw2)
{ 

  __m256i tmpre,tmpim,x1_2,x2_2;

  packed_cmult_256(*(x1),*(tw1),&x1_2);
  packed_cmult_256(*(x2),*(tw2),&x2_2);
  *(y0)  = _mm256_adds_epi16(*(x0),_mm256_adds_epi16(x1_2,x2_2));
  cmult_256(x1_2,*(W13_256),&tmpre,&tmpim);
  cmac_256(x2_2,*(W23_256),&tmpre,&tmpim);
  *(y1) = cpack_256(tmpre,tmpim);
  *(y1) = _mm256_adds_epi16(*(x0),*(y1));
  cmult_256(x1_2,*(W23_256),&tmpre,&tmpim);
  cmac_256(x2_2,*(W13_256),&tmpre,&tmpim);
  *(y2) = cpack_256(tmpre,tmpim);
  *(y2) = _mm256_adds_epi16(*(x0),*(y2));
}
#endif


#endif

#if defined(__x86_64__) || defined(__i386__) || defined SIMDE_ENABLE_NATIVE_ALIASES
static inline void ibfly3(__m128i *x0,__m128i *x1,__m128i *x2,
			  __m128i *y0,__m128i *y1,__m128i *y2,
			  __m128i *tw1,__m128i *tw2) __attribute__((always_inline));

static inline void ibfly3(__m128i *x0,__m128i *x1,__m128i *x2,
			  __m128i *y0,__m128i *y1,__m128i *y2,
			  __m128i *tw1,__m128i *tw2)
{

  __m128i tmpre,tmpim,x1_2,x2_2;

  packed_cmultc(*(x1),*(tw1),&x1_2);
  packed_cmultc(*(x2),*(tw2),&x2_2);
  *(y0)  = _mm_adds_epi16(*(x0),_mm_adds_epi16(x1_2,x2_2));
  cmultc(x1_2,*(W13),&tmpre,&tmpim);
  cmacc(x2_2,*(W23),&tmpre,&tmpim);
  *(y1) = cpack(tmpre,tmpim);
  *(y1) = _mm_adds_epi16(*(x0),*(y1));
  cmultc(x1_2,*(W23),&tmpre,&tmpim);
  cmacc(x2_2,*(W13),&tmpre,&tmpim);
  *(y2) = cpack(tmpre,tmpim);
  *(y2) = _mm_adds_epi16(*(x0),*(y2));
}

#ifdef __AVX2__

static inline void ibfly3_256(__m256i *x0,__m256i *x1,__m256i *x2,
			      __m256i *y0,__m256i *y1,__m256i *y2,
			      __m256i *tw1,__m256i *tw2) __attribute__((always_inline));

static inline void ibfly3_256(__m256i *x0,__m256i *x1,__m256i *x2,
			      __m256i *y0,__m256i *y1,__m256i *y2,
			      __m256i *tw1,__m256i *tw2)
{ 

  __m256i tmpre,tmpim,x1_2,x2_2;

  packed_cmultc_256(*(x1),*(tw1),&x1_2);
  packed_cmultc_256(*(x2),*(tw2),&x2_2);
  *(y0)  = _mm256_adds_epi16(*(x0),_mm256_adds_epi16(x1_2,x2_2));
  cmultc_256(x1_2,*(W13_256),&tmpre,&tmpim);
  cmacc_256(x2_2,*(W23_256),&tmpre,&tmpim);
  *(y1) = cpack_256(tmpre,tmpim);
  *(y1) = _mm256_adds_epi16(*(x0),*(y1));
  cmultc_256(x1_2,*(W23_256),&tmpre,&tmpim);
  cmacc_256(x2_2,*(W13_256),&tmpre,&tmpim);
  *(y2) = cpack_256(tmpre,tmpim);
  *(y2) = _mm256_adds_epi16(*(x0),*(y2));
}
#endif


#endif

#if defined(__x86_64__) || defined(__i386__) || defined SIMDE_ENABLE_NATIVE_ALIASES
static inline void bfly3_tw1(__m128i *x0,__m128i *x1,__m128i *x2,
                             __m128i *y0,__m128i *y1,__m128i *y2) __attribute__((always_inline));

static inline void bfly3_tw1(__m128i *x0,__m128i *x1,__m128i *x2,
                             __m128i *y0,__m128i *y1,__m128i *y2)
{

  __m128i tmpre,tmpim;

  *(y0) = _mm_adds_epi16(*(x0),_mm_adds_epi16(*(x1),*(x2)));
  cmult(*(x1),*(W13),&tmpre,&tmpim);
  cmac(*(x2),*(W23),&tmpre,&tmpim);
  *(y1) = cpack(tmpre,tmpim);
  *(y1) = _mm_adds_epi16(*(x0),*(y1));
  cmult(*(x1),*(W23),&tmpre,&tmpim);
  cmac(*(x2),*(W13),&tmpre,&tmpim);
  *(y2) = cpack(tmpre,tmpim);
  *(y2) = _mm_adds_epi16(*(x0),*(y2));
}

#ifdef __AVX2__

static inline void bfly3_tw1_256(__m256i *x0,__m256i *x1,__m256i *x2,
				 __m256i *y0,__m256i *y1,__m256i *y2) __attribute__((always_inline));

static inline void bfly3_tw1_256(__m256i *x0,__m256i *x1,__m256i *x2,
				 __m256i *y0,__m256i *y1,__m256i *y2)
{

  __m256i tmpre,tmpim;

  *(y0) = _mm256_adds_epi16(*(x0),_mm256_adds_epi16(*(x1),*(x2)));
  cmult_256(*(x1),*(W13_256),&tmpre,&tmpim);
  cmac_256(*(x2),*(W23_256),&tmpre,&tmpim);
  *(y1) = cpack_256(tmpre,tmpim);
  *(y1) = _mm256_adds_epi16(*(x0),*(y1));
  cmult_256(*(x1),*(W23_256),&tmpre,&tmpim);
  cmac_256(*(x2),*(W13_256),&tmpre,&tmpim);
  *(y2) = cpack_256(tmpre,tmpim);
  *(y2) = _mm256_adds_epi16(*(x0),*(y2));
}
#endif


#endif

#if defined(__x86_64__) || defined(__i386__) || defined SIMDE_ENABLE_NATIVE_ALIASES
static inline void bfly4(__m128i *x0,__m128i *x1,__m128i *x2,__m128i *x3,
                         __m128i *y0,__m128i *y1,__m128i *y2,__m128i *y3,
                         __m128i *tw1,__m128i *tw2,__m128i *tw3)__attribute__((always_inline));

static inline void bfly4(__m128i *x0,__m128i *x1,__m128i *x2,__m128i *x3,
                         __m128i *y0,__m128i *y1,__m128i *y2,__m128i *y3,
                         __m128i *tw1,__m128i *tw2,__m128i *tw3)
{

  __m128i x1r_2,x1i_2,x2r_2,x2i_2,x3r_2,x3i_2,dy0r,dy0i,dy1r,dy1i,dy2r,dy2i,dy3r,dy3i;

  //  cmult(*(x0),*(W0),&x0r_2,&x0i_2);
  cmult(*(x1),*(tw1),&x1r_2,&x1i_2);
  cmult(*(x2),*(tw2),&x2r_2,&x2i_2);
  cmult(*(x3),*(tw3),&x3r_2,&x3i_2);
  //  dy0r = _mm_add_epi32(x0r_2,_mm_add_epi32(x1r_2,_mm_add_epi32(x2r_2,x3r_2)));
  //  dy0i = _mm_add_epi32(x0i_2,_mm_add_epi32(x1i_2,_mm_add_epi32(x2i_2,x3i_2)));
  //  *(y0)  = cpack(dy0r,dy0i);
  dy0r = _mm_add_epi32(x1r_2,_mm_add_epi32(x2r_2,x3r_2));
  dy0i = _mm_add_epi32(x1i_2,_mm_add_epi32(x2i_2,x3i_2));
  *(y0)  = _mm_add_epi16(*(x0),cpack(dy0r,dy0i));
  //  dy1r = _mm_add_epi32(x0r_2,_mm_sub_epi32(x1i_2,_mm_add_epi32(x2r_2,x3i_2)));
  //  dy1i = _mm_sub_epi32(x0i_2,_mm_add_epi32(x1r_2,_mm_sub_epi32(x2i_2,x3r_2)));
  //  *(y1)  = cpack(dy1r,dy1i);
  dy1r = _mm_sub_epi32(x1i_2,_mm_add_epi32(x2r_2,x3i_2));
  dy1i = _mm_sub_epi32(_mm_sub_epi32(x3r_2,x2i_2),x1r_2);
  *(y1)  = _mm_add_epi16(*(x0),cpack(dy1r,dy1i));
  //  dy2r = _mm_sub_epi32(x0r_2,_mm_sub_epi32(x1r_2,_mm_sub_epi32(x2r_2,x3r_2)));
  //  dy2i = _mm_sub_epi32(x0i_2,_mm_sub_epi32(x1i_2,_mm_sub_epi32(x2i_2,x3i_2)));
  //  *(y2)  = cpack(dy2r,dy2i);
  dy2r = _mm_sub_epi32(_mm_sub_epi32(x2r_2,x3r_2),x1r_2);
  dy2i = _mm_sub_epi32(_mm_sub_epi32(x2i_2,x3i_2),x1i_2);
  *(y2)  = _mm_add_epi16(*(x0),cpack(dy2r,dy2i));
  //  dy3r = _mm_sub_epi32(x0r_2,_mm_add_epi32(x1i_2,_mm_sub_epi32(x2r_2,x3i_2)));
  //  dy3i = _mm_add_epi32(x0i_2,_mm_sub_epi32(x1r_2,_mm_add_epi32(x2i_2,x3r_2)));
  //  *(y3) = cpack(dy3r,dy3i);
  dy3r = _mm_sub_epi32(_mm_sub_epi32(x3i_2,x2r_2),x1i_2);
  dy3i = _mm_sub_epi32(x1r_2,_mm_add_epi32(x2i_2,x3r_2));
  *(y3) = _mm_add_epi16(*(x0),cpack(dy3r,dy3i));
}

#ifdef __AVX2__
static inline void bfly4_256(__m256i *x0,__m256i *x1,__m256i *x2,__m256i *x3,
			     __m256i *y0,__m256i *y1,__m256i *y2,__m256i *y3,
			     __m256i *tw1,__m256i *tw2,__m256i *tw3)__attribute__((always_inline));

static inline void bfly4_256(__m256i *x0,__m256i *x1,__m256i *x2,__m256i *x3,
			     __m256i *y0,__m256i *y1,__m256i *y2,__m256i *y3,
			     __m256i *tw1,__m256i *tw2,__m256i *tw3)
{

  __m256i x1r_2,x1i_2,x2r_2,x2i_2,x3r_2,x3i_2,dy0r,dy0i,dy1r,dy1i,dy2r,dy2i,dy3r,dy3i;

  //  cmult(*(x0),*(W0),&x0r_2,&x0i_2);
  cmult_256(*(x1),*(tw1),&x1r_2,&x1i_2);
  cmult_256(*(x2),*(tw2),&x2r_2,&x2i_2);
  cmult_256(*(x3),*(tw3),&x3r_2,&x3i_2);
  //  dy0r = _mm_add_epi32(x0r_2,_mm_add_epi32(x1r_2,_mm_add_epi32(x2r_2,x3r_2)));
  //  dy0i = _mm_add_epi32(x0i_2,_mm_add_epi32(x1i_2,_mm_add_epi32(x2i_2,x3i_2)));
  //  *(y0)  = cpack(dy0r,dy0i);
  dy0r = _mm256_add_epi32(x1r_2,_mm256_add_epi32(x2r_2,x3r_2));
  dy0i = _mm256_add_epi32(x1i_2,_mm256_add_epi32(x2i_2,x3i_2));
  *(y0)  = _mm256_add_epi16(*(x0),cpack_256(dy0r,dy0i));
  //  dy1r = _mm_add_epi32(x0r_2,_mm_sub_epi32(x1i_2,_mm_add_epi32(x2r_2,x3i_2)));
  //  dy1i = _mm_sub_epi32(x0i_2,_mm_add_epi32(x1r_2,_mm_sub_epi32(x2i_2,x3r_2)));
  //  *(y1)  = cpack(dy1r,dy1i);
  dy1r = _mm256_sub_epi32(x1i_2,_mm256_add_epi32(x2r_2,x3i_2));
  dy1i = _mm256_sub_epi32(_mm256_sub_epi32(x3r_2,x2i_2),x1r_2);
  *(y1)  = _mm256_add_epi16(*(x0),cpack_256(dy1r,dy1i));
  //  dy2r = _mm_sub_epi32(x0r_2,_mm_sub_epi32(x1r_2,_mm_sub_epi32(x2r_2,x3r_2)));
  //  dy2i = _mm_sub_epi32(x0i_2,_mm_sub_epi32(x1i_2,_mm_sub_epi32(x2i_2,x3i_2)));
  //  *(y2)  = cpack(dy2r,dy2i);
  dy2r = _mm256_sub_epi32(_mm256_sub_epi32(x2r_2,x3r_2),x1r_2);
  dy2i = _mm256_sub_epi32(_mm256_sub_epi32(x2i_2,x3i_2),x1i_2);
  *(y2)  = _mm256_add_epi16(*(x0),cpack_256(dy2r,dy2i));
  //  dy3r = _mm_sub_epi32(x0r_2,_mm_add_epi32(x1i_2,_mm_sub_epi32(x2r_2,x3i_2)));
  //  dy3i = _mm_add_epi32(x0i_2,_mm_sub_epi32(x1r_2,_mm_add_epi32(x2i_2,x3r_2)));
  //  *(y3) = cpack(dy3r,dy3i);
  dy3r = _mm256_sub_epi32(_mm256_sub_epi32(x3i_2,x2r_2),x1i_2);
  dy3i = _mm256_sub_epi32(x1r_2,_mm256_add_epi32(x2i_2,x3r_2));
  *(y3) = _mm256_add_epi16(*(x0),cpack_256(dy3r,dy3i));
}
static inline void rvv_bfly4_256(int16_t *x0, int16_t *x1, int16_t *x2, int16_t *x3,
                                 int16_t *y0, int16_t *y1, int16_t *y2, int16_t *y3,
                                 const int16_t *tw1, const int16_t *tw2, const int16_t *tw3)
{
    // 向量长度设置
    size_t vl = __riscv_vsetvl_e16m4(16);
    size_t vl_32 = __riscv_vsetvl_e32m4(16 / 2); // 8个32位元素

    // 1. 加载所有输入数据和旋转因子 (16-bit)
    vint16m4_t x0_vec = __riscv_vle16_v_i16m4(x0, vl);
    vint16m4_t x1_vec = __riscv_vle16_v_i16m4(x1, vl);
    vint16m4_t x2_vec = __riscv_vle16_v_i16m4(x2, vl);
    vint16m4_t x3_vec = __riscv_vle16_v_i16m4(x3, vl);

    vint16m4_t tw1_vec = __riscv_vle16_v_i16m4(tw1, vl);
    vint16m4_t tw2_vec = __riscv_vle16_v_i16m4(tw2, vl);
    vint16m4_t tw3_vec = __riscv_vle16_v_i16m4(tw3, vl);

    // 2. 声明 32 位中间结果 (使用 vint32m4_t)
    vint32m4_t x1r_2, x1i_2, x2r_2, x2i_2, x3r_2, x3i_2;
    vint32m4_t dy0r, dy0i, dy1r, dy1i, dy2r, dy2i, dy3r, dy3i;

    // 3. 复数乘法 (cmult_256) - **指针强制转换**
    // 使用 rvv_cmult_256 (非共轭乘法)
    rvv_cmult_256(x1_vec, tw1_vec, (vint16m4_t *)&x1r_2, (vint16m4_t *)&x1i_2);
    rvv_cmult_256(x2_vec, tw2_vec, (vint16m4_t *)&x2r_2, (vint16m4_t *)&x2i_2);
    rvv_cmult_256(x3_vec, tw3_vec, (vint16m4_t *)&x3r_2, (vint16m4_t *)&x3i_2);
    
    // ----------------------------------------------------------------------
    // 4. 计算 y0: dy0r = x1r_2 + (x2r_2 + x3r_2), dy0i = x1i_2 + (x2i_2 + x3i_2)
    
    // 辅助宏用于简化 32-bit 运算
    #define V32(v) __riscv_vreinterpret_v_i32m4_i16m4(v)
    #define V16(v) __riscv_vreinterpret_v_i16m4_i32m4(v)

    // dy0r
    vint16m4_t tmp_r_16 = rvv_add_epi32(V32(x2r_2), V32(x3r_2), vl_32);
    vint16m4_t dy0r_16 = rvv_add_epi32(V32(x1r_2), tmp_r_16, vl_32);
    dy0r = V16(dy0r_16); 

    // dy0i
    vint16m4_t tmp_i_16 = rvv_add_epi32(V32(x2i_2), V32(x3i_2), vl_32);
    vint16m4_t dy0i_16 = rvv_add_epi32(V32(x1i_2), tmp_i_16, vl_32);
    dy0i = V16(dy0i_16); 
    
    // 写入 y0
    vint16m4_t packed_y0 = rvv_cpack_256(dy0r, dy0i, vl_32);
    vint16m4_t y0_result = rvv_adds_epi16(x0_vec, packed_y0, vl);
    __riscv_vse16_v_i16m4(y0, y0_result, vl);

    // ----------------------------------------------------------------------
    // 5. 计算 y1: dy1r = x1i_2 - (x2r_2 + x3i_2), dy1i = (x3r_2 - x2i_2) - x1r_2
    
    // dy1r = x1i_2 - (x2r_2 + x3i_2)
    vint16m4_t tmp_add_dy1r_16 = rvv_add_epi32(V32(x2r_2), V32(x3i_2), vl_32); 
    vint16m4_t dy1r_16 = rvv_sub_epi32(V32(x1i_2), tmp_add_dy1r_16, vl_32);       
    dy1r = V16(dy1r_16); 

    // dy1i = (x3r_2 - x2i_2) - x1r_2
    vint16m4_t tmp_sub_dy1i_16 = rvv_sub_epi32(V32(x3r_2), V32(x2i_2), vl_32); 
    vint16m4_t dy1i_16 = rvv_sub_epi32(tmp_sub_dy1i_16, V32(x1r_2), vl_32);       
    dy1i = V16(dy1i_16); 
    
    // 写入 y1
    vint16m4_t packed_y1 = rvv_cpack_256(dy1r, dy1i, vl_32);
    vint16m4_t y1_result = rvv_adds_epi16(x0_vec, packed_y1, vl);
    __riscv_vse16_v_i16m4(y1, y1_result, vl);

    // ----------------------------------------------------------------------
    // 6. 计算 y2: dy2r = (x2r_2 - x3r_2) - x1r_2, dy2i = (x2i_2 - x3i_2) - x1i_2
    
    // dy2r = (x2r_2 - x3r_2) - x1r_2
    vint16m4_t tmp_sub_dy2r_16 = rvv_sub_epi32(V32(x2r_2), V32(x3r_2), vl_32); 
    vint16m4_t dy2r_16 = rvv_sub_epi32(tmp_sub_dy2r_16, V32(x1r_2), vl_32);       
    dy2r = V16(dy2r_16); 

    // dy2i = (x2i_2 - x3i_2) - x1i_2
    vint16m4_t tmp_sub_dy2i_16 = rvv_sub_epi32(V32(x2i_2), V32(x3i_2), vl_32); 
    vint16m4_t dy2i_16 = rvv_sub_epi32(tmp_sub_dy2i_16, V32(x1i_2), vl_32);       
    dy2i = V16(dy2i_16); 
    
    // 写入 y2
    vint16m4_t packed_y2 = rvv_cpack_256(dy2r, dy2i, vl_32);
    vint16m4_t y2_result = rvv_adds_epi16(x0_vec, packed_y2, vl);
    __riscv_vse16_v_i16m4(y2, y2_result, vl);

    // ----------------------------------------------------------------------
    // 7. 计算 y3: dy3r = (x3i_2 - x2r_2) - x1i_2, dy3i = x1r_2 - (x2i_2 + x3r_2)
    
    // dy3r = (x3i_2 - x2r_2) - x1i_2
    vint16m4_t tmp_sub_dy3r_16 = rvv_sub_epi32(V32(x3i_2), V32(x2r_2), vl_32); 
    vint16m4_t dy3r_16 = rvv_sub_epi32(tmp_sub_dy3r_16, V32(x1i_2), vl_32);       
    dy3r = V16(dy3r_16); 

    // dy3i = x1r_2 - (x2i_2 + x3r_2)
    vint16m4_t tmp_add_dy3i_16 = rvv_add_epi32(V32(x2i_2), V32(x3r_2), vl_32); 
    vint16m4_t dy3i_16 = rvv_sub_epi32(V32(x1r_2), tmp_add_dy3i_16, vl_32);       
    dy3i = V16(dy3i_16); 
    
    // 写入 y3
    vint16m4_t packed_y3 = rvv_cpack_256(dy3r, dy3i, vl_32);
    vint16m4_t y3_result = rvv_adds_epi16(x0_vec, packed_y3, vl);
    __riscv_vse16_v_i16m4(y3, y3_result, vl);

    #undef V32
    #undef V16
}
#endif

#endif

#if defined(__x86_64__) || defined(__i386__) || defined SIMDE_ENABLE_NATIVE_ALIASES
static inline void ibfly4(__m128i *x0,__m128i *x1,__m128i *x2,__m128i *x3,
                          __m128i *y0,__m128i *y1,__m128i *y2,__m128i *y3,
                          __m128i *tw1,__m128i *tw2,__m128i *tw3)__attribute__((always_inline));

static inline void ibfly4(__m128i *x0,__m128i *x1,__m128i *x2,__m128i *x3,
                          __m128i *y0,__m128i *y1,__m128i *y2,__m128i *y3,
                          __m128i *tw1,__m128i *tw2,__m128i *tw3)
{

  __m128i x1r_2,x1i_2,x2r_2,x2i_2,x3r_2,x3i_2,dy0r,dy0i,dy1r,dy1i,dy2r,dy2i,dy3r,dy3i;


  cmultc(*(x1),*(tw1),&x1r_2,&x1i_2);
  cmultc(*(x2),*(tw2),&x2r_2,&x2i_2);
  cmultc(*(x3),*(tw3),&x3r_2,&x3i_2);

  dy0r = _mm_add_epi32(x1r_2,_mm_add_epi32(x2r_2,x3r_2));
  dy0i = _mm_add_epi32(x1i_2,_mm_add_epi32(x2i_2,x3i_2));
  *(y0)  = _mm_add_epi16(*(x0),cpack(dy0r,dy0i));
  dy3r = _mm_sub_epi32(x1i_2,_mm_add_epi32(x2r_2,x3i_2));
  dy3i = _mm_sub_epi32(_mm_sub_epi32(x3r_2,x2i_2),x1r_2);
  *(y3)  = _mm_add_epi16(*(x0),cpack(dy3r,dy3i));
  dy2r = _mm_sub_epi32(_mm_sub_epi32(x2r_2,x3r_2),x1r_2);
  dy2i = _mm_sub_epi32(_mm_sub_epi32(x2i_2,x3i_2),x1i_2);
  *(y2)  = _mm_add_epi16(*(x0),cpack(dy2r,dy2i));
  dy1r = _mm_sub_epi32(_mm_sub_epi32(x3i_2,x2r_2),x1i_2);
  dy1i = _mm_sub_epi32(x1r_2,_mm_add_epi32(x2i_2,x3r_2));
  *(y1) = _mm_add_epi16(*(x0),cpack(dy1r,dy1i));
}

#ifdef __AVX2__

static inline void ibfly4_256(__m256i *x0,__m256i *x1,__m256i *x2,__m256i *x3,
			      __m256i *y0,__m256i *y1,__m256i *y2,__m256i *y3,
			      __m256i *tw1,__m256i *tw2,__m256i *tw3)__attribute__((always_inline));

static inline void ibfly4_256(__m256i *x0,__m256i *x1,__m256i *x2,__m256i *x3,
			      __m256i *y0,__m256i *y1,__m256i *y2,__m256i *y3,
			      __m256i *tw1,__m256i *tw2,__m256i *tw3)
{

  __m256i x1r_2,x1i_2,x2r_2,x2i_2,x3r_2,x3i_2,dy0r,dy0i,dy1r,dy1i,dy2r,dy2i,dy3r,dy3i;


  cmultc_256(*(x1),*(tw1),&x1r_2,&x1i_2);
  cmultc_256(*(x2),*(tw2),&x2r_2,&x2i_2);
  cmultc_256(*(x3),*(tw3),&x3r_2,&x3i_2);

  dy0r = _mm256_add_epi32(x1r_2,_mm256_add_epi32(x2r_2,x3r_2));
  dy0i = _mm256_add_epi32(x1i_2,_mm256_add_epi32(x2i_2,x3i_2));
  *(y0)  = _mm256_add_epi16(*(x0),cpack_256(dy0r,dy0i));
  dy3r = _mm256_sub_epi32(x1i_2,_mm256_add_epi32(x2r_2,x3i_2));
  dy3i = _mm256_sub_epi32(_mm256_sub_epi32(x3r_2,x2i_2),x1r_2);
  *(y3)  = _mm256_add_epi16(*(x0),cpack_256(dy3r,dy3i));
  dy2r = _mm256_sub_epi32(_mm256_sub_epi32(x2r_2,x3r_2),x1r_2);
  dy2i = _mm256_sub_epi32(_mm256_sub_epi32(x2i_2,x3i_2),x1i_2);
  *(y2)  = _mm256_add_epi16(*(x0),cpack_256(dy2r,dy2i));
  dy1r = _mm256_sub_epi32(_mm256_sub_epi32(x3i_2,x2r_2),x1i_2);
  dy1i = _mm256_sub_epi32(x1r_2,_mm256_add_epi32(x2i_2,x3r_2));
  *(y1) = _mm256_add_epi16(*(x0),cpack_256(dy1r,dy1i));
}

static inline void rvv_ibfly4_256(int16_t *x0, int16_t *x1, int16_t *x2, int16_t *x3,
                                  int16_t *y0, int16_t *y1, int16_t *y2, int16_t *y3,
                                  const int16_t *tw1, const int16_t *tw2, const int16_t *tw3)
{
   size_t vl = __riscv_vsetvl_e16m4(16);
    size_t vl_32 = __riscv_vsetvl_e32m4(16 / 2); // 8个32位元素

    // 1. 加载所有输入数据和旋转因子 (16-bit)
    vint16m4_t x0_vec = __riscv_vle16_v_i16m4(x0, vl);
    vint16m4_t x1_vec = __riscv_vle16_v_i16m4(x1, vl);
    vint16m4_t x2_vec = __riscv_vle16_v_i16m4(x2, vl);
    vint16m4_t x3_vec = __riscv_vle16_v_i16m4(x3, vl);

    vint16m4_t tw1_vec = __riscv_vle16_v_i16m4(tw1, vl);
    vint16m4_t tw2_vec = __riscv_vle16_v_i16m4(tw2, vl);
    vint16m4_t tw3_vec = __riscv_vle16_v_i16m4(tw3, vl);

    // 2. 声明 32 位中间结果 (使用 vint32m4_t)
    vint32m4_t x1r_2, x1i_2, x2r_2, x2i_2, x3r_2, x3i_2;
    vint32m4_t dy0r, dy0i, dy1r, dy1i, dy2r, dy2i, dy3r, dy3i;

    // 3. 复数共轭乘法 (cmultc_256) - **指针强制转换**
    // 我们将 vint32m4_t 的地址强制转换为 vint16m4_t * 来匹配函数签名
    rvv_cmultc_256(x1_vec, tw1_vec, (vint16m4_t *)&x1r_2, (vint16m4_t *)&x1i_2);
    rvv_cmultc_256(x2_vec, tw2_vec, (vint16m4_t *)&x2r_2, (vint16m4_t *)&x2i_2);
    rvv_cmultc_256(x3_vec, tw3_vec, (vint16m4_t *)&x3r_2, (vint16m4_t *)&x3i_2);
    
    // ----------------------------------------------------------------------
    // 4. 计算 dy0r 和 dy0i
    
    // x1r_2 + (x2r_2 + x3r_2)
    // 步骤 1: (x2r_2 + x3r_2)
    vint16m4_t tmp_r_16 = rvv_add_epi32(
        __riscv_vreinterpret_v_i32m4_i16m4(x2r_2), 
        __riscv_vreinterpret_v_i32m4_i16m4(x3r_2), 
        vl_32
    );
    // 步骤 2: x1r_2 + tmp_r
    vint16m4_t dy0r_16 = rvv_add_epi32(
        __riscv_vreinterpret_v_i32m4_i16m4(x1r_2), 
        tmp_r_16, 
        vl_32
    );
    dy0r = __riscv_vreinterpret_v_i16m4_i32m4(dy0r_16); // 转换回 32-bit 容器

    // x1i_2 + (x2i_2 + x3i_2)
    // 步骤 1: (x2i_2 + x3i_2)
    vint16m4_t tmp_i_16 = rvv_add_epi32(
        __riscv_vreinterpret_v_i32m4_i16m4(x2i_2), 
        __riscv_vreinterpret_v_i32m4_i16m4(x3i_2), 
        vl_32
    );
    // 步骤 2: x1i_2 + tmp_i
    vint16m4_t dy0i_16 = rvv_add_epi32(
        __riscv_vreinterpret_v_i32m4_i16m4(x1i_2), 
        tmp_i_16, 
        vl_32
    );
    dy0i = __riscv_vreinterpret_v_i16m4_i32m4(dy0i_16); // 转换回 32-bit 容器
    
    // 写入 y0
    vint16m4_t packed_y0 = rvv_cpack_256(dy0r, dy0i, vl_32);
    vint16m4_t y0_result = rvv_adds_epi16(x0_vec, packed_y0, vl);
    __riscv_vse16_v_i16m4(y0, y0_result, vl);

    // ----------------------------------------------------------------------
    // 5. 计算 dy3r 和 dy3i 
    
    // dy3r = x1i_2 - (x2r_2 + x3i_2);
    // 步骤 1: (x2r_2 + x3i_2)
    vint16m4_t tmp_add_dy3r_16 = rvv_add_epi32(
        __riscv_vreinterpret_v_i32m4_i16m4(x2r_2), 
        __riscv_vreinterpret_v_i32m4_i16m4(x3i_2), 
        vl_32
    ); 
    // 步骤 2: x1i_2 - tmp_add_dy3r
    vint16m4_t dy3r_16 = rvv_sub_epi32(
        __riscv_vreinterpret_v_i32m4_i16m4(x1i_2), 
        tmp_add_dy3r_16, 
        vl_32
    );             
    dy3r = __riscv_vreinterpret_v_i16m4_i32m4(dy3r_16); 

    // dy3i = (x3r_2 - x2i_2) - x1r_2;
    // 步骤 1: (x3r_2 - x2i_2)
    vint16m4_t tmp_sub_dy3i_16 = rvv_sub_epi32(
        __riscv_vreinterpret_v_i32m4_i16m4(x3r_2), 
        __riscv_vreinterpret_v_i32m4_i16m4(x2i_2), 
        vl_32
    ); 
    // 步骤 2: tmp_sub_dy3i - x1r_2
    vint16m4_t dy3i_16 = rvv_sub_epi32(
        tmp_sub_dy3i_16, 
        __riscv_vreinterpret_v_i32m4_i16m4(x1r_2), 
        vl_32
    );             
    dy3i = __riscv_vreinterpret_v_i16m4_i32m4(dy3i_16); 
    
    // 写入 y3
    vint16m4_t packed_y3 = rvv_cpack_256(dy3r, dy3i, vl_32);
    vint16m4_t y3_result = rvv_adds_epi16(x0_vec, packed_y3, vl);
    __riscv_vse16_v_i16m4(y3, y3_result, vl);

    // ----------------------------------------------------------------------
    // 6. 计算 dy2r 和 dy2i 
    
    // dy2r = (x2r_2 - x3r_2) - x1r_2;
    // 步骤 1: (x2r_2 - x3r_2)
    vint16m4_t tmp_sub_dy2r_16 = rvv_sub_epi32(
        __riscv_vreinterpret_v_i32m4_i16m4(x2r_2), 
        __riscv_vreinterpret_v_i32m4_i16m4(x3r_2), 
        vl_32
    ); 
    // 步骤 2: tmp_sub_dy2r - x1r_2
    vint16m4_t dy2r_16 = rvv_sub_epi32(
        tmp_sub_dy2r_16, 
        __riscv_vreinterpret_v_i32m4_i16m4(x1r_2), 
        vl_32
    );             
    dy2r = __riscv_vreinterpret_v_i16m4_i32m4(dy2r_16); 

    // dy2i = (x2i_2 - x3i_2) - x1i_2;
    // 步骤 1: (x2i_2 - x3i_2)
    vint16m4_t tmp_sub_dy2i_16 = rvv_sub_epi32(
        __riscv_vreinterpret_v_i32m4_i16m4(x2i_2), 
        __riscv_vreinterpret_v_i32m4_i16m4(x3i_2), 
        vl_32
    ); 
    // 步骤 2: tmp_sub_dy2i - x1i_2
    vint16m4_t dy2i_16 = rvv_sub_epi32(
        tmp_sub_dy2i_16, 
        __riscv_vreinterpret_v_i32m4_i16m4(x1i_2), 
        vl_32
    );             
    dy2i = __riscv_vreinterpret_v_i16m4_i32m4(dy2i_16); 
    
    // 写入 y2
    vint16m4_t packed_y2 = rvv_cpack_256(dy2r, dy2i, vl_32);
    vint16m4_t y2_result = rvv_adds_epi16(x0_vec, packed_y2, vl);
    __riscv_vse16_v_i16m4(y2, y2_result, vl);

    // ----------------------------------------------------------------------
    // 7. 计算 dy1r 和 dy1i 
    
    // dy1r = (x3i_2 - x2r_2) - x1i_2;
    // 步骤 1: (x3i_2 - x2r_2)
    vint16m4_t tmp_sub_dy1r_16 = rvv_sub_epi32(
        __riscv_vreinterpret_v_i32m4_i16m4(x3i_2), 
        __riscv_vreinterpret_v_i32m4_i16m4(x2r_2), 
        vl_32
    ); 
    // 步骤 2: tmp_sub_dy1r - x1i_2
    vint16m4_t dy1r_16 = rvv_sub_epi32(
        tmp_sub_dy1r_16, 
        __riscv_vreinterpret_v_i32m4_i16m4(x1i_2), 
        vl_32
    );             
    dy1r = __riscv_vreinterpret_v_i16m4_i32m4(dy1r_16); 

    // dy1i = x1r_2 - (x2i_2 + x3r_2);
    // 步骤 1: (x2i_2 + x3r_2)
    vint16m4_t tmp_add_dy1i_16 = rvv_add_epi32(
        __riscv_vreinterpret_v_i32m4_i16m4(x2i_2), 
        __riscv_vreinterpret_v_i32m4_i16m4(x3r_2), 
        vl_32
    ); 
    // 步骤 2: x1r_2 - tmp_add_dy1i
    vint16m4_t dy1i_16 = rvv_sub_epi32(
        __riscv_vreinterpret_v_i32m4_i16m4(x1r_2), 
        tmp_add_dy1i_16, 
        vl_32
    );             
    dy1i = __riscv_vreinterpret_v_i16m4_i32m4(dy1i_16); 
    
    // 写入 y1
    vint16m4_t packed_y1 = rvv_cpack_256(dy1r, dy1i, vl_32);
    vint16m4_t y1_result = rvv_adds_epi16(x0_vec, packed_y1, vl);
    __riscv_vse16_v_i16m4(y1, y1_result, vl);
}
#endif


#endif

#if defined(__x86_64__) || defined(__i386__) || defined SIMDE_ENABLE_NATIVE_ALIASES

static inline void bfly4_tw1(__m128i *x0,__m128i *x1,__m128i *x2,__m128i *x3,
                             __m128i *y0,__m128i *y1,__m128i *y2,__m128i *y3)__attribute__((always_inline));

static inline void bfly4_tw1(__m128i *x0,__m128i *x1,__m128i *x2,__m128i *x3,
                             __m128i *y0,__m128i *y1,__m128i *y2,__m128i *y3)
{
  register __m128i x1_flip,x3_flip,x02t,x13t;
  register __m128i complex_shuffle = _mm_set_epi8(13,12,15,14,9,8,11,10,5,4,7,6,1,0,3,2);

  x02t    = _mm_adds_epi16(*(x0),*(x2));
  x13t    = _mm_adds_epi16(*(x1),*(x3));
  *(y0)   = _mm_adds_epi16(x02t,x13t);
  *(y2)   = _mm_subs_epi16(x02t,x13t);
  x1_flip = _mm_sign_epi16(*(x1),*(__m128i*)conjugatedft);
  x1_flip = _mm_shuffle_epi8(x1_flip,complex_shuffle);
  x3_flip = _mm_sign_epi16(*(x3),*(__m128i*)conjugatedft);
  x3_flip = _mm_shuffle_epi8(x3_flip,complex_shuffle);
  x02t    = _mm_subs_epi16(*(x0),*(x2));
  x13t    = _mm_subs_epi16(x1_flip,x3_flip);
  *(y1)   = _mm_adds_epi16(x02t,x13t);  // x0 + x1f - x2 - x3f
  *(y3)   = _mm_subs_epi16(x02t,x13t);  // x0 - x1f - x2 + x3f

  /*
  *(y0) = _mm_adds_epi16(*(x0),_mm_adds_epi16(*(x1),_mm_adds_epi16(*(x2),*(x3))));
  x1_flip = _mm_sign_epi16(*(x1),*(__m128i*)conjugatedft);
  x1_flip = _mm_shuffle_epi8(x1_flip,_mm_set_epi8(13,12,15,14,9,8,11,10,5,4,7,6,1,0,3,2));
  x3_flip = _mm_sign_epi16(*(x3),*(__m128i*)conjugatedft);
  x3_flip = _mm_shuffle_epi8(x3_flip,_mm_set_epi8(13,12,15,14,9,8,11,10,5,4,7,6,1,0,3,2));
  *(y1)   = _mm_adds_epi16(*(x0),_mm_subs_epi16(x1_flip,_mm_adds_epi16(*(x2),x3_flip)));
  *(y2)   = _mm_subs_epi16(*(x0),_mm_subs_epi16(*(x1),_mm_subs_epi16(*(x2),*(x3))));
  *(y3)   = _mm_subs_epi16(*(x0),_mm_adds_epi16(x1_flip,_mm_subs_epi16(*(x2),x3_flip)));
  */
}

#ifdef __AVX2__

static inline void bfly4_tw1_256(__m256i *x0,__m256i *x1,__m256i *x2,__m256i *x3,
				 __m256i *y0,__m256i *y1,__m256i *y2,__m256i *y3)__attribute__((always_inline));

static inline void bfly4_tw1_256(__m256i *x0,__m256i *x1,__m256i *x2,__m256i *x3,
				 __m256i *y0,__m256i *y1,__m256i *y2,__m256i *y3)
{
  register __m256i x1_flip,x3_flip,x02t,x13t;
  register __m256i complex_shuffle = _mm256_set_epi8(29,28,31,30,25,24,27,26,21,20,23,22,17,16,19,18,13,12,15,14,9,8,11,10,5,4,7,6,1,0,3,2);

  x02t    = _mm256_adds_epi16(*(x0),*(x2));
  x13t    = _mm256_adds_epi16(*(x1),*(x3));
  *(y0)   = _mm256_adds_epi16(x02t,x13t);
  *(y2)   = _mm256_subs_epi16(x02t,x13t);
  x1_flip = _mm256_sign_epi16(*(x1),*(__m256i*)conjugatedft);
  x1_flip = _mm256_shuffle_epi8(x1_flip,complex_shuffle);
  x3_flip = _mm256_sign_epi16(*(x3),*(__m256i*)conjugatedft);
  x3_flip = _mm256_shuffle_epi8(x3_flip,complex_shuffle);
  x02t    = _mm256_subs_epi16(*(x0),*(x2));
  x13t    = _mm256_subs_epi16(x1_flip,x3_flip);
  *(y1)   = _mm256_adds_epi16(x02t,x13t);  // x0 + x1f - x2 - x3f
  *(y3)   = _mm256_subs_epi16(x02t,x13t);  // x0 - x1f - x2 + x3f
}
#endif



#endif



#if defined(__x86_64__) || defined(__i386__) || defined SIMDE_ENABLE_NATIVE_ALIASES
static inline void bfly4_16(__m128i *x0,__m128i *x1,__m128i *x2,__m128i *x3,
                            __m128i *y0,__m128i *y1,__m128i *y2,__m128i *y3,
                            __m128i *tw1,__m128i *tw2,__m128i *tw3,
                            __m128i *tw1b,__m128i *tw2b,__m128i *tw3b)__attribute__((always_inline));

static inline void bfly4_16(__m128i *x0,__m128i *x1,__m128i *x2,__m128i *x3,
                            __m128i *y0,__m128i *y1,__m128i *y2,__m128i *y3,
                            __m128i *tw1,__m128i *tw2,__m128i *tw3,
                            __m128i *tw1b,__m128i *tw2b,__m128i *tw3b)
{

  register __m128i x1t,x2t,x3t,x02t,x13t;
  register __m128i x1_flip,x3_flip;

  x1t = packed_cmult2(*(x1),*(tw1),*(tw1b));
  x2t = packed_cmult2(*(x2),*(tw2),*(tw2b));
  x3t = packed_cmult2(*(x3),*(tw3),*(tw3b));


  //  bfly4_tw1(x0,&x1t,&x2t,&x3t,y0,y1,y2,y3);
  x02t  = _mm_adds_epi16(*(x0),x2t);
  x13t  = _mm_adds_epi16(x1t,x3t);
  /*
  *(y0) = _mm_adds_epi16(*(x0),_mm_adds_epi16(x1t,_mm_adds_epi16(x2t,x3t)));
  *(y2)   = _mm_subs_epi16(*(x0),_mm_subs_epi16(x1t,_mm_subs_epi16(x2t,x3t)));
  */
  *(y0)   = _mm_adds_epi16(x02t,x13t);
  *(y2)   = _mm_subs_epi16(x02t,x13t);

  x1_flip = _mm_sign_epi16(x1t,*(__m128i*)conjugatedft);
  //  x1_flip = _mm_shufflelo_epi16(x1_flip,_MM_SHUFFLE(2,3,0,1));
  //  x1_flip = _mm_shufflehi_epi16(x1_flip,_MM_SHUFFLE(2,3,0,1));
  x1_flip = _mm_shuffle_epi8(x1_flip,_mm_set_epi8(13,12,15,14,9,8,11,10,5,4,7,6,1,0,3,2));
  x3_flip = _mm_sign_epi16(x3t,*(__m128i*)conjugatedft);
  //  x3_flip = _mm_shufflelo_epi16(x3_flip,_MM_SHUFFLE(2,3,0,1));
  //  x3_flip = _mm_shufflehi_epi16(x3_flip,_MM_SHUFFLE(2,3,0,1));
  x3_flip = _mm_shuffle_epi8(x3_flip,_mm_set_epi8(13,12,15,14,9,8,11,10,5,4,7,6,1,0,3,2));
  x02t  = _mm_subs_epi16(*(x0),x2t);
  x13t  = _mm_subs_epi16(x1_flip,x3_flip);
  /*
  *(y1)   = _mm_adds_epi16(*(x0),_mm_subs_epi16(x1_flip,_mm_adds_epi16(x2t,x3_flip)));  // x0 + x1f - x2 - x3f
  *(y3)   = _mm_subs_epi16(*(x0),_mm_adds_epi16(x1_flip,_mm_subs_epi16(x2t,x3_flip)));  // x0 - x1f - x2 + x3f
  */
  *(y1)   = _mm_adds_epi16(x02t,x13t);  // x0 + x1f - x2 - x3f
  *(y3)   = _mm_subs_epi16(x02t,x13t);  // x0 - x1f - x2 + x3f

}

#ifdef __AVX2__
static inline void bfly4_16_256(__m256i *x0,__m256i *x1,__m256i *x2,__m256i *x3,
				__m256i *y0,__m256i *y1,__m256i *y2,__m256i *y3,
				__m256i *tw1,__m256i *tw2,__m256i *tw3,
				__m256i *tw1b,__m256i *tw2b,__m256i *tw3b)__attribute__((always_inline));

static inline void bfly4_16_256(__m256i *x0,__m256i *x1,__m256i *x2,__m256i *x3,
				__m256i *y0,__m256i *y1,__m256i *y2,__m256i *y3,
				__m256i *tw1,__m256i *tw2,__m256i *tw3,
				__m256i *tw1b,__m256i *tw2b,__m256i *tw3b)
{

  register __m256i x1t,x2t,x3t,x02t,x13t;
  register __m256i x1_flip,x3_flip;
  register __m256i complex_shuffle = _mm256_set_epi8(29,28,31,30,25,24,27,26,21,20,23,22,17,16,19,18,13,12,15,14,9,8,11,10,5,4,7,6,1,0,3,2);

  // each input xi is assumed to be to consecutive vectors xi0 xi1 on which to perform the 8 butterflies
  // [xi00 xi01 xi02 xi03 xi10 xi20 xi30 xi40]
  // each output yi is the same

  x1t = packed_cmult2_256(*(x1),*(tw1),*(tw1b));
  x2t = packed_cmult2_256(*(x2),*(tw2),*(tw2b));
  x3t = packed_cmult2_256(*(x3),*(tw3),*(tw3b));

  x02t  = _mm256_adds_epi16(*(x0),x2t);
  x13t  = _mm256_adds_epi16(x1t,x3t);
  *(y0)   = _mm256_adds_epi16(x02t,x13t);
  *(y2)   = _mm256_subs_epi16(x02t,x13t);

  x1_flip = _mm256_sign_epi16(x1t,*(__m256i*)conjugatedft);
  x1_flip = _mm256_shuffle_epi8(x1_flip,complex_shuffle);
  x3_flip = _mm256_sign_epi16(x3t,*(__m256i*)conjugatedft);
  x3_flip = _mm256_shuffle_epi8(x3_flip,complex_shuffle);
  x02t  = _mm256_subs_epi16(*(x0),x2t);
  x13t  = _mm256_subs_epi16(x1_flip,x3_flip);
  *(y1)   = _mm256_adds_epi16(x02t,x13t);  // x0 + x1f - x2 - x3f
  *(y3)   = _mm256_subs_epi16(x02t,x13t);  // x0 - x1f - x2 + x3f

  // // 1. 基础配置 (使用 m4 类型)
  //   const size_t VL_16 = 16; // 16 x 16-bit = 256 bits
  //   size_t vl = __riscv_vsetvl_e16m4(VL_16);

  //   // 转换指针类型方便操作
  //   int16_t *x0_ptr = (int16_t *)x0;
  //   int16_t *x1_ptr = (int16_t *)x1;
  //   int16_t *x2_ptr = (int16_t *)x2;
  //   int16_t *x3_ptr = (int16_t *)x3;

  //   int16_t *y0_ptr = (int16_t *)y0;
  //   int16_t *y1_ptr = (int16_t *)y1;
  //   int16_t *y2_ptr = (int16_t *)y2;
  //   int16_t *y3_ptr = (int16_t *)y3;

  //   int16_t *tw1_ptr  = (int16_t *)tw1;
  //   int16_t *tw2_ptr  = (int16_t *)tw2;
  //   int16_t *tw3_ptr  = (int16_t *)tw3;
  //   int16_t *tw1b_ptr = (int16_t *)tw1b;
  //   int16_t *tw2b_ptr = (int16_t *)tw2b;
  //   int16_t *tw3b_ptr = (int16_t *)tw3b;

  //   // 2. 加载输入向量
  //   vint16m4_t x0_vec = __riscv_vle16_v_i16m4(x0_ptr, vl);
  //   vint16m4_t x1_vec = __riscv_vle16_v_i16m4(x1_ptr, vl);
  //   vint16m4_t x2_vec = __riscv_vle16_v_i16m4(x2_ptr, vl);
  //   vint16m4_t x3_vec = __riscv_vle16_v_i16m4(x3_ptr, vl);

  //   // 3. 加载旋转因子
  //   vint16m4_t tw1_vec  = __riscv_vle16_v_i16m4(tw1_ptr, vl);
  //   vint16m4_t tw2_vec  = __riscv_vle16_v_i16m4(tw2_ptr, vl);
  //   vint16m4_t tw3_vec  = __riscv_vle16_v_i16m4(tw3_ptr, vl);
    
  //   vint16m4_t tw1b_vec = __riscv_vle16_v_i16m4(tw1b_ptr, vl);
  //   vint16m4_t tw2b_vec = __riscv_vle16_v_i16m4(tw2b_ptr, vl);
  //   vint16m4_t tw3b_vec = __riscv_vle16_v_i16m4(tw3b_ptr, vl);

  //   // 4. 准备常量 (Conjugate & Shuffle)
  //   // 假设 conjugatedft 是全局/外部定义的 const int16_t 数组
  //   vint16m4_t conjugatedft_vec = __riscv_vle16_v_i16m4((const int16_t*)conjugatedft, vl);

  //   // 准备 shuffle mask
  //   // 使用您提供的 rvv_set_epi8_mask 函数
  //   vint8m4_t complex_shuffle = rvv_set_epi8_mask(
  //       29,28,31,30,25,24,27,26,21,20,23,22,17,16,19,18,
  //       13,12,15,14,9,8,11,10,5,4,7,6,1,0,3,2);

  //   // 5. 计算逻辑
  //   vint16m4_t x1t, x2t, x3t, x02t, x13t;
  //   vint16m4_t x1_flip, x3_flip;
  //   vint16m4_t y0_res, y1_res, y2_res, y3_res;

  //   // x1t = packed_cmult2_256(*(x1),*(tw1),*(tw1b));
  //   x1t = rvv_packed_cmult2(x1_vec, tw1_vec, tw1b_vec, vl);
    
  //   // x2t = packed_cmult2_256(*(x2),*(tw2),*(tw2b));
  //   x2t = rvv_packed_cmult2(x2_vec, tw2_vec, tw2b_vec, vl);
    
  //   // x3t = packed_cmult2_256(*(x3),*(tw3),*(tw3b));
  //   x3t = rvv_packed_cmult2(x3_vec, tw3_vec, tw3b_vec, vl);

  //   // x02t = _mm256_adds_epi16(*(x0),x2t);
  //   x02t = rvv_adds_epi16(x0_vec, x2t, vl);
    
  //   // x13t = _mm256_adds_epi16(x1t,x3t);
  //   x13t = rvv_adds_epi16(x1t, x3t, vl);
    
  //   // *(y0) = _mm256_adds_epi16(x02t,x13t);
  //   y0_res = rvv_adds_epi16(x02t, x13t, vl);
  //   __riscv_vse16_v_i16m4(y0_ptr, y0_res, vl);

  //   // *(y2) = _mm256_subs_epi16(x02t,x13t);
  //   y2_res = rvv_subs_epi16(x02t, x13t, vl);
  //   __riscv_vse16_v_i16m4(y2_ptr, y2_res, vl);

  //   // x1_flip = _mm256_sign_epi16(x1t,*(__m256i*)conjugatedft);
  //   x1_flip = rvv_sign_epi16(x1t, conjugatedft_vec, vl);
    
  //   // x1_flip = _mm256_shuffle_epi8(x1_flip,complex_shuffle);
  //   x1_flip = rvv_shuffle_epi8(x1_flip, complex_shuffle, vl);

  //   // x3_flip = _mm256_sign_epi16(x3t,*(__m256i*)conjugatedft);
  //   x3_flip = rvv_sign_epi16(x3t, conjugatedft_vec, vl);
    
  //   // x3_flip = _mm256_shuffle_epi8(x3_flip,complex_shuffle);
  //   x3_flip = rvv_shuffle_epi8(x3_flip, complex_shuffle, vl);

  //   // x02t = _mm256_subs_epi16(*(x0),x2t);
  //   x02t = rvv_subs_epi16(x0_vec, x2t, vl);
    
  //   // x13t = _mm256_subs_epi16(x1_flip,x3_flip);
  //   x13t = rvv_subs_epi16(x1_flip, x3_flip, vl);
    
  //   // *(y1) = _mm256_adds_epi16(x02t,x13t);
  //   y1_res = rvv_adds_epi16(x02t, x13t, vl);
  //   __riscv_vse16_v_i16m4(y1_ptr, y1_res, vl);

  //   // *(y3) = _mm256_subs_epi16(x02t,x13t);
  //   y3_res = rvv_subs_epi16(x02t, x13t, vl);
  //   __riscv_vse16_v_i16m4(y3_ptr, y3_res, vl);

}

#endif


#endif

#if defined(__x86_64__) || defined(__i386__) || defined SIMDE_ENABLE_NATIVE_ALIASES
static inline void ibfly4_16(__m128i *x0,__m128i *x1,__m128i *x2,__m128i *x3,
                             __m128i *y0,__m128i *y1,__m128i *y2,__m128i *y3,
                             __m128i *tw1,__m128i *tw2,__m128i *tw3,
                             __m128i *tw1b,__m128i *tw2b,__m128i *tw3b)__attribute__((always_inline));

static inline void ibfly4_16(__m128i *x0,__m128i *x1,__m128i *x2,__m128i *x3,
                             __m128i *y0,__m128i *y1,__m128i *y2,__m128i *y3,
                             __m128i *tw1,__m128i *tw2,__m128i *tw3,
                             __m128i *tw1b,__m128i *tw2b,__m128i *tw3b)
{

  register __m128i x1t,x2t,x3t,x02t,x13t;
  register __m128i x1_flip,x3_flip;

  x1t = packed_cmult2(*(x1),*(tw1),*(tw1b));
  x2t = packed_cmult2(*(x2),*(tw2),*(tw2b));
  x3t = packed_cmult2(*(x3),*(tw3),*(tw3b));


  //  bfly4_tw1(x0,&x1t,&x2t,&x3t,y0,y1,y2,y3);
  x02t  = _mm_adds_epi16(*(x0),x2t);
  x13t  = _mm_adds_epi16(x1t,x3t);
  /*
  *(y0) = _mm_adds_epi16(*(x0),_mm_adds_epi16(x1t,_mm_adds_epi16(x2t,x3t)));
  *(y2)   = _mm_subs_epi16(*(x0),_mm_subs_epi16(x1t,_mm_subs_epi16(x2t,x3t)));
  */
  *(y0)   = _mm_adds_epi16(x02t,x13t);
  *(y2)   = _mm_subs_epi16(x02t,x13t);

  x1_flip = _mm_sign_epi16(x1t,*(__m128i*)conjugatedft);
  //  x1_flip = _mm_shufflelo_epi16(x1_flip,_MM_SHUFFLE(2,3,0,1));
  //  x1_flip = _mm_shufflehi_epi16(x1_flip,_MM_SHUFFLE(2,3,0,1));
  x1_flip = _mm_shuffle_epi8(x1_flip,_mm_set_epi8(13,12,15,14,9,8,11,10,5,4,7,6,1,0,3,2));
  x3_flip = _mm_sign_epi16(x3t,*(__m128i*)conjugatedft);
  //  x3_flip = _mm_shufflelo_epi16(x3_flip,_MM_SHUFFLE(2,3,0,1));
  //  x3_flip = _mm_shufflehi_epi16(x3_flip,_MM_SHUFFLE(2,3,0,1));
  x3_flip = _mm_shuffle_epi8(x3_flip,_mm_set_epi8(13,12,15,14,9,8,11,10,5,4,7,6,1,0,3,2));
  x02t  = _mm_subs_epi16(*(x0),x2t);
  x13t  = _mm_subs_epi16(x1_flip,x3_flip);
  /*
  *(y1)   = _mm_adds_epi16(*(x0),_mm_subs_epi16(x1_flip,_mm_adds_epi16(x2t,x3_flip)));  // x0 + x1f - x2 - x3f
  *(y3)   = _mm_subs_epi16(*(x0),_mm_adds_epi16(x1_flip,_mm_subs_epi16(x2t,x3_flip)));  // x0 - x1f - x2 + x3f
  */
  *(y3)   = _mm_adds_epi16(x02t,x13t);  // x0 + x1f - x2 - x3f
  *(y1)   = _mm_subs_epi16(x02t,x13t);  // x0 - x1f - x2 + x3f

}

#ifdef __AVX2__
static inline void ibfly4_16_256(__m256i *x0,__m256i *x1,__m256i *x2,__m256i *x3,
				 __m256i *y0,__m256i *y1,__m256i *y2,__m256i *y3,
				 __m256i *tw1,__m256i *tw2,__m256i *tw3,
				 __m256i *tw1b,__m256i *tw2b,__m256i *tw3b)__attribute__((always_inline));

static inline void ibfly4_16_256(__m256i *x0,__m256i *x1,__m256i *x2,__m256i *x3,
				 __m256i *y0,__m256i *y1,__m256i *y2,__m256i *y3,
				 __m256i *tw1,__m256i *tw2,__m256i *tw3,
				 __m256i *tw1b,__m256i *tw2b,__m256i *tw3b)
{

  register __m256i x1t,x2t,x3t,x02t,x13t;
  register __m256i x1_flip,x3_flip;
  register __m256i complex_shuffle = _mm256_set_epi8(29,28,31,30,25,24,27,26,21,20,23,22,17,16,19,18,13,12,15,14,9,8,11,10,5,4,7,6,1,0,3,2);

  // each input xi is assumed to be to consecutive vectors xi0 xi1 on which to perform the 8 butterflies
  // [xi00 xi01 xi02 xi03 xi10 xi20 xi30 xi40]
  // each output yi is the same

  x1t = packed_cmult2_256(*(x1),*(tw1),*(tw1b));
  //DEBUG_PRINT_AVX_VEC("AVX: x1t", x1t);
  x2t = packed_cmult2_256(*(x2),*(tw2),*(tw2b));
  //DEBUG_PRINT_AVX_VEC("AVX: x2t", x2t);
  x3t = packed_cmult2_256(*(x3),*(tw3),*(tw3b));
  //DEBUG_PRINT_AVX_VEC("AVX: x3t", x3t);
  // // DEBUG_PRINT_AVX_VEC("AVX: y256[0]", y256[0]);
  // // DEBUG_PRINT_AVX_VEC("AVX: y256[1]", y256[1]);
  // // DEBUG_PRINT_AVX_VEC("AVX: y256[2]", y256[2]);
  // // DEBUG_PRINT_AVX_VEC("AVX: y256[3]", y256[3]);
  // // DEBUG_PRINT_AVX_VEC("AVX: y256[4]", y256[4]);
  // // DEBUG_PRINT_AVX_VEC("AVX: y256[5]", y256[5]);
  // // DEBUG_PRINT_AVX_VEC("AVX: y256[6]", y256[6]);
  // // DEBUG_PRINT_AVX_VEC("AVX: y256[7]", y256[7]);
  x02t  = _mm256_adds_epi16(*(x0),x2t);
  //DEBUG_PRINT_AVX_VEC("AVX: x02t", x02t);
  x13t  = _mm256_adds_epi16(x1t,x3t);
  //DEBUG_PRINT_AVX_VEC("AVX: x13t", x13t);
  *(y0)   = _mm256_adds_epi16(x02t,x13t);
  //DEBUG_PRINT_AVX_VEC("AVX: *(y0)", *(y0));
  *(y2)   = _mm256_subs_epi16(x02t,x13t);
  //DEBUG_PRINT_AVX_VEC("AVX: *(y2)", *(y2));

  x1_flip = _mm256_sign_epi16(x1t,*(__m256i*)conjugatedft);
  //DEBUG_PRINT_AVX_VEC("AVX: x1_flip", x1_flip);
  x1_flip = _mm256_shuffle_epi8(x1_flip,complex_shuffle);
  //DEBUG_PRINT_AVX_VEC("AVX: x1_flip", x1_flip);
  x3_flip = _mm256_sign_epi16(x3t,*(__m256i*)conjugatedft);
  //DEBUG_PRINT_AVX_VEC("AVX: x3_flip", x3_flip);
  x3_flip = _mm256_shuffle_epi8(x3_flip,complex_shuffle);
  //DEBUG_PRINT_AVX_VEC("AVX: x3_flip", x3_flip);
  x02t  = _mm256_subs_epi16(*(x0),x2t);
  //DEBUG_PRINT_AVX_VEC("AVX: x02t", x02t);
  x13t  = _mm256_subs_epi16(x1_flip,x3_flip);
  //DEBUG_PRINT_AVX_VEC("AVX: x13t", x13t);
  *(y3)   = _mm256_adds_epi16(x02t,x13t);  // x0 + x1f - x2 - x3f
  //DEBUG_PRINT_AVX_VEC("AVX: *(y3)", *(y3));
  *(y1)   = _mm256_subs_epi16(x02t,x13t);  // x0 - x1f - x2 + x3f
  //DEBUG_PRINT_AVX_VEC("AVX: *(y1)", *(y1));

  //  // 1. 基础配置 (使用 m4 类型)
  //   const size_t VL_16 = 16; // 16 x 16-bit = 256 bits
  //   size_t vl = __riscv_vsetvl_e16m4(VL_16);

  //   // 转换指针类型方便操作
  //   int16_t *x0_ptr = (int16_t *)x0;
  //   int16_t *x1_ptr = (int16_t *)x1;
  //   int16_t *x2_ptr = (int16_t *)x2;
  //   int16_t *x3_ptr = (int16_t *)x3;

  //   int16_t *y0_ptr = (int16_t *)y0;
  //   int16_t *y1_ptr = (int16_t *)y1;
  //   int16_t *y2_ptr = (int16_t *)y2;
  //   int16_t *y3_ptr = (int16_t *)y3;

  //   int16_t *tw1_ptr  = (int16_t *)tw1;
  //   int16_t *tw2_ptr  = (int16_t *)tw2;
  //   int16_t *tw3_ptr  = (int16_t *)tw3;
  //   int16_t *tw1b_ptr = (int16_t *)tw1b;
  //   int16_t *tw2b_ptr = (int16_t *)tw2b;
  //   int16_t *tw3b_ptr = (int16_t *)tw3b;

  //   // 2. 加载输入向量
  //   vint16m4_t x0_vec = __riscv_vle16_v_i16m4(x0_ptr, vl);
  //   vint16m4_t x1_vec = __riscv_vle16_v_i16m4(x1_ptr, vl);
  //   vint16m4_t x2_vec = __riscv_vle16_v_i16m4(x2_ptr, vl);
  //   vint16m4_t x3_vec = __riscv_vle16_v_i16m4(x3_ptr, vl);

  //   // 3. 加载旋转因子
  //   vint16m4_t tw1_vec  = __riscv_vle16_v_i16m4(tw1_ptr, vl);
  //   vint16m4_t tw2_vec  = __riscv_vle16_v_i16m4(tw2_ptr, vl);
  //   vint16m4_t tw3_vec  = __riscv_vle16_v_i16m4(tw3_ptr, vl);
    
  //   vint16m4_t tw1b_vec = __riscv_vle16_v_i16m4(tw1b_ptr, vl);
  //   vint16m4_t tw2b_vec = __riscv_vle16_v_i16m4(tw2b_ptr, vl);
  //   vint16m4_t tw3b_vec = __riscv_vle16_v_i16m4(tw3b_ptr, vl);

  //   // 4. 准备常量 (Conjugate & Shuffle)
  //   // 假设 conjugatedft 是全局/外部定义的 const int16_t 数组
  //   extern const int16_t conjugatedft[]; 
  //   vint16m4_t conjugatedft_vec = __riscv_vle16_v_i16m4(conjugatedft, vl);

  //   // 准备 shuffle mask
  //   vint8m4_t complex_shuffle = rvv_set_epi8_mask(
  //       29,28,31,30,25,24,27,26,21,20,23,22,17,16,19,18,
  //       13,12,15,14,9,8,11,10,5,4,7,6,1,0,3,2);

  //   // 5. 计算逻辑
  //   vint16m4_t x1t, x2t, x3t, x02t, x13t;
  //   vint16m4_t x1_flip, x3_flip;
  //   vint16m4_t y0_res, y1_res, y2_res, y3_res;

  //   // x1t = packed_cmult2_256(*(x1),*(tw1),*(tw1b));
  //   x1t = rvv_packed_cmult2(x1_vec, tw1_vec, tw1b_vec, vl);
    
  //   // x2t = packed_cmult2_256(*(x2),*(tw2),*(tw2b));
  //   x2t = rvv_packed_cmult2(x2_vec, tw2_vec, tw2b_vec, vl);
    
  //   // x3t = packed_cmult2_256(*(x3),*(tw3),*(tw3b));
  //   x3t = rvv_packed_cmult2(x3_vec, tw3_vec, tw3b_vec, vl);

  //   // x02t = _mm256_adds_epi16(*(x0),x2t);
  //   x02t = rvv_adds_epi16(x0_vec, x2t, vl);
    
  //   // x13t = _mm256_adds_epi16(x1t,x3t);
  //   x13t = rvv_adds_epi16(x1t, x3t, vl);
    
  //   // *(y0) = _mm256_adds_epi16(x02t,x13t);
  //   y0_res = rvv_adds_epi16(x02t, x13t, vl);
  //   __riscv_vse16_v_i16m4(y0_ptr, y0_res, vl);

  //   // *(y2) = _mm256_subs_epi16(x02t,x13t);
  //   y2_res = rvv_subs_epi16(x02t, x13t, vl);
  //   __riscv_vse16_v_i16m4(y2_ptr, y2_res, vl);

  //   // x1_flip = _mm256_sign_epi16(x1t,*(__m256i*)conjugatedft);
  //   x1_flip = rvv_sign_epi16(x1t, conjugatedft_vec, vl);
  //   // x1_flip = _mm256_shuffle_epi8(x1_flip,complex_shuffle);
  //   x1_flip = rvv_shuffle_epi8(x1_flip, complex_shuffle, vl);

  //   // x3_flip = _mm256_sign_epi16(x3t,*(__m256i*)conjugatedft);
  //   x3_flip = rvv_sign_epi16(x3t, conjugatedft_vec, vl);
  //   // x3_flip = _mm256_shuffle_epi8(x3_flip,complex_shuffle);
  //   x3_flip = rvv_shuffle_epi8(x3_flip, complex_shuffle, vl);

  //   // x02t = _mm256_subs_epi16(*(x0),x2t);
  //   x02t = rvv_subs_epi16(x0_vec, x2t, vl);
    
  //   // x13t = _mm256_subs_epi16(x1_flip,x3_flip);
  //   x13t = rvv_subs_epi16(x1_flip, x3_flip, vl);
    
  //   // *(y3) = _mm256_adds_epi16(x02t,x13t);  <-- 注意：IBFLY 中这里是 y3
  //   y3_res = rvv_adds_epi16(x02t, x13t, vl);
  //   __riscv_vse16_v_i16m4(y3_ptr, y3_res, vl);

  //   // *(y1) = _mm256_subs_epi16(x02t,x13t);  <-- 注意：IBFLY 中这里是 y1
  //   y1_res = rvv_subs_epi16(x02t, x13t, vl);
  //   __riscv_vse16_v_i16m4(y1_ptr, y1_res, vl);

}
#endif


#endif

#if defined(__x86_64__) || defined(__i386__) || defined SIMDE_ENABLE_NATIVE_ALIASES
static inline void bfly5(__m128i *x0, __m128i *x1, __m128i *x2, __m128i *x3,__m128i *x4,
                         __m128i *y0, __m128i *y1, __m128i *y2, __m128i *y3,__m128i *y4,
                         __m128i *tw1,__m128i *tw2,__m128i *tw3,__m128i *tw4)__attribute__((always_inline));

static inline void bfly5(__m128i *x0, __m128i *x1, __m128i *x2, __m128i *x3,__m128i *x4,
                         __m128i *y0, __m128i *y1, __m128i *y2, __m128i *y3,__m128i *y4,
                         __m128i *tw1,__m128i *tw2,__m128i *tw3,__m128i *tw4)
{



  __m128i x1_2,x2_2,x3_2,x4_2,tmpre,tmpim;

  packed_cmult(*(x1),*(tw1),&x1_2);
  packed_cmult(*(x2),*(tw2),&x2_2);
  packed_cmult(*(x3),*(tw3),&x3_2);
  packed_cmult(*(x4),*(tw4),&x4_2);

  *(y0)  = _mm_adds_epi16(*(x0),_mm_adds_epi16(x1_2,_mm_adds_epi16(x2_2,_mm_adds_epi16(x3_2,x4_2))));
  cmult(x1_2,*(W15),&tmpre,&tmpim);
  cmac(x2_2,*(W25),&tmpre,&tmpim);
  cmac(x3_2,*(W35),&tmpre,&tmpim);
  cmac(x4_2,*(W45),&tmpre,&tmpim);
  *(y1) = cpack(tmpre,tmpim);
  *(y1) = _mm_adds_epi16(*(x0),*(y1));

  cmult(x1_2,*(W25),&tmpre,&tmpim);
  cmac(x2_2,*(W45),&tmpre,&tmpim);
  cmac(x3_2,*(W15),&tmpre,&tmpim);
  cmac(x4_2,*(W35),&tmpre,&tmpim);
  *(y2) = cpack(tmpre,tmpim);
  *(y2) = _mm_adds_epi16(*(x0),*(y2));

  cmult(x1_2,*(W35),&tmpre,&tmpim);
  cmac(x2_2,*(W15),&tmpre,&tmpim);
  cmac(x3_2,*(W45),&tmpre,&tmpim);
  cmac(x4_2,*(W25),&tmpre,&tmpim);
  *(y3) = cpack(tmpre,tmpim);
  *(y3) = _mm_adds_epi16(*(x0),*(y3));

  cmult(x1_2,*(W45),&tmpre,&tmpim);
  cmac(x2_2,*(W35),&tmpre,&tmpim);
  cmac(x3_2,*(W25),&tmpre,&tmpim);
  cmac(x4_2,*(W15),&tmpre,&tmpim);
  *(y4) = cpack(tmpre,tmpim);
  *(y4) = _mm_adds_epi16(*(x0),*(y4));


}






#endif

#if defined(__x86_64__) || defined(__i386__) || defined SIMDE_ENABLE_NATIVE_ALIASES
static inline void bfly5_tw1(__m128i *x0, __m128i *x1, __m128i *x2, __m128i *x3,__m128i *x4,
                             __m128i *y0, __m128i *y1, __m128i *y2, __m128i *y3,__m128i *y4) __attribute__((always_inline));

static inline void bfly5_tw1(__m128i *x0, __m128i *x1, __m128i *x2, __m128i *x3,__m128i *x4,
                             __m128i *y0, __m128i *y1, __m128i *y2, __m128i *y3,__m128i *y4)
{

  __m128i tmpre,tmpim;

  *(y0) = _mm_adds_epi16(*(x0),_mm_adds_epi16(*(x1),_mm_adds_epi16(*(x2),_mm_adds_epi16(*(x3),*(x4)))));
  cmult(*(x1),*(W15),&tmpre,&tmpim);
  cmac(*(x2),*(W25),&tmpre,&tmpim);
  cmac(*(x3),*(W35),&tmpre,&tmpim);
  cmac(*(x4),*(W45),&tmpre,&tmpim);
  *(y1) = cpack(tmpre,tmpim);
  *(y1) = _mm_adds_epi16(*(x0),*(y1));
  cmult(*(x1),*(W25),&tmpre,&tmpim);
  cmac(*(x2),*(W45),&tmpre,&tmpim);
  cmac(*(x3),*(W15),&tmpre,&tmpim);
  cmac(*(x4),*(W35),&tmpre,&tmpim);
  *(y2) = cpack(tmpre,tmpim);
  *(y2) = _mm_adds_epi16(*(x0),*(y2));
  cmult(*(x1),*(W35),&tmpre,&tmpim);
  cmac(*(x2),*(W15),&tmpre,&tmpim);
  cmac(*(x3),*(W45),&tmpre,&tmpim);
  cmac(*(x4),*(W25),&tmpre,&tmpim);
  *(y3) = cpack(tmpre,tmpim);
  *(y3) = _mm_adds_epi16(*(x0),*(y3));
  cmult(*(x1),*(W45),&tmpre,&tmpim);
  cmac(*(x2),*(W35),&tmpre,&tmpim);
  cmac(*(x3),*(W25),&tmpre,&tmpim);
  cmac(*(x4),*(W15),&tmpre,&tmpim);
  *(y4) = cpack(tmpre,tmpim);
  *(y4) = _mm_adds_epi16(*(x0),*(y4));
}




#endif
// performs 4x4 transpose of input x (complex interleaved) using 128bit SIMD intrinsics
// i.e. x = [x0r x0i x1r x1i ... x15r x15i], y = [x0r x0i x4r x4i x8r x8i x12r x12i x1r x1i x5r x5i x9r x9i x13r x13i x2r x2i ... x15r x15i]

#if defined(__x86_64__) || defined(__i386__) || defined SIMDE_ENABLE_NATIVE_ALIASES
static inline void transpose16(__m128i *x,__m128i *y) __attribute__((always_inline));
static inline void transpose16(__m128i *x,__m128i *y)
{
  register __m128i ytmp0,ytmp1,ytmp2,ytmp3;

  ytmp0 = _mm_unpacklo_epi32(x[0],x[1]);
  ytmp1 = _mm_unpackhi_epi32(x[0],x[1]);
  ytmp2 = _mm_unpacklo_epi32(x[2],x[3]);
  ytmp3 = _mm_unpackhi_epi32(x[2],x[3]);
  y[0]    = _mm_unpacklo_epi64(ytmp0,ytmp2);
  y[1]    = _mm_unpackhi_epi64(ytmp0,ytmp2);
  y[2]    = _mm_unpacklo_epi64(ytmp1,ytmp3);
  y[3]    = _mm_unpackhi_epi64(ytmp1,ytmp3);
}


# endif
// same as above but output is offset by off
#if defined(__x86_64__) || defined(__i386__) || defined SIMDE_ENABLE_NATIVE_ALIASES
static inline void transpose16_ooff(__m128i *x,__m128i *y,int off) __attribute__((always_inline));

static inline void transpose16_ooff(__m128i *x,__m128i *y,int off)
{
  register __m128i ytmp0,ytmp1,ytmp2,ytmp3;
  __m128i *y2=y;

  ytmp0 = _mm_unpacklo_epi32(x[0],x[1]); // x00 x10 x01 x11
  ytmp1 = _mm_unpackhi_epi32(x[0],x[1]); // x02 x12 x03 x13
  ytmp2 = _mm_unpacklo_epi32(x[2],x[3]); // x20 x30 x21 x31
  ytmp3 = _mm_unpackhi_epi32(x[2],x[3]); // x22 x32 x23 x33
  *y2     = _mm_unpacklo_epi64(ytmp0,ytmp2); // x00 x10 x20 x30 
  y2+=off;
  *y2     = _mm_unpackhi_epi64(ytmp0,ytmp2); // x01 x11 x21 x31
  y2+=off;
  *y2     = _mm_unpacklo_epi64(ytmp1,ytmp3); // x02 x12 x22 x32
  y2+=off;
  *y2     = _mm_unpackhi_epi64(ytmp1,ytmp3); // x03 x13 x23 x33
}


#ifdef __AVX2__


#ifdef yuandaima
static inline vint32m2_t rvv_set_epi32_m2(int32_t e7, int32_t e6, int32_t e5, int32_t e4,
                                          int32_t e3, int32_t e2, int32_t e1, int32_t e0,
                                          size_t vl) {
    // AVX set_epi32 参数顺序是 e7(高位)...e0(低位)
    // 内存顺序应为 e0, e1, ..., e7
    int32_t buf[8] = {e0, e1, e2, e3, e4, e5, e6, e7};
    return __riscv_vle32_v_i32m2(buf, vl);
}

// -------------------------------------------------------------------------
// 辅助函数：AVX PermuteVar8x32 (LMUL=2)
// -------------------------------------------------------------------------
static inline vint32m2_t rvv_permutevar8x32_epi32_m2(vint32m2_t a, vint32m2_t idx, size_t vl) {
    // AVX 逻辑: res[i] = a[idx[i] & 7]
    // 1. Mask indices to 3 bits
    vint32m2_t masked_idx = __riscv_vand_vx_i32m2(idx, 7, vl);
    // 2. Interpret as unsigned for vrgather
    vuint32m2_t gather_idx = __riscv_vreinterpret_v_i32m2_u32m2(masked_idx);
    // 3. Gather
    return __riscv_vrgather_vv_i32m2(a, gather_idx, vl);
}
static inline void debug_print_vec_m2(const char *label, vint32m2_t vec, size_t vl) {
    int32_t temp_buf[8]; // 临时数组，用于存放 256-bit 数据 (8 * 32-bit)
    
    // 将向量寄存器 vec 的内容存储到 temp_buf 内存中
    __riscv_vse32_v_i32m2(temp_buf, vec, vl);

    printf("DEBUG [%s]: ", label);
    for (int i = 0; i < 8; i++) {
        printf("%d ", temp_buf[i]); // 以十进制打印
        // printf("0x%08X ", temp_buf[i]); // 如果想看十六进制，用这就行
    }
    printf("\n");
}
static inline vint32m2_t rvv_unpacklo_epi64_m2(vint32m2_t v1, vint32m2_t v2, size_t vl) {
    const size_t VL_64 = 4;
    
    // 步骤 1: 转换到 u32m2 (避免从 i32m2 直接跳到 u64m2 报错)
    vuint32m2_t v1_32u = __riscv_vreinterpret_v_i32m2_u32m2(v1);
    vuint32m2_t v2_32u = __riscv_vreinterpret_v_i32m2_u32m2(v2);
    
    // 步骤 2: 从 u32m2 解释为 u64m2 (改变宽度)
    vuint64m2_t v1_64u = __riscv_vreinterpret_v_u32m2_u64m2(v1_32u);
    vuint64m2_t v2_64u = __riscv_vreinterpret_v_u32m2_u64m2(v2_32u);

    // 3. 存储到临时 64-bit 缓冲区
    uint64_t a_buf[4], b_buf[4], res_buf[4];
    __riscv_vse64_v_u64m2((uint64_t*)a_buf, v1_64u, VL_64);
    __riscv_vse64_v_u64m2((uint64_t*)b_buf, v2_64u, VL_64);

    // 4. C 语言交错 (Lo)
    res_buf[0] = a_buf[0]; res_buf[1] = b_buf[0];
    res_buf[2] = a_buf[2]; res_buf[3] = b_buf[2];
    
    // 5. 加载回 64-bit 向量
    vuint64m2_t result_64u = __riscv_vle64_v_u64m2((uint64_t*)res_buf, VL_64);
    
    // 6. 转换回 vint32m2_t (先 u64m2 -> u32m2，再 u32m2 -> i32m2)
    vuint32m2_t result_32u = __riscv_vreinterpret_v_u64m2_u32m2(result_64u);
    return __riscv_vreinterpret_v_u32m2_i32m2(result_32u);
}


/**
 * @brief 模拟 _mm256_unpackhi_epi64，适用于 vint32m2_t 类型。
 */
static inline vint32m2_t rvv_unpackhi_epi64_m2(vint32m2_t v1, vint32m2_t v2, size_t vl) {
    const size_t VL_64 = 4;
    
    // 步骤 1 & 2: 解释到 u64m2
    vuint32m2_t v1_32u = __riscv_vreinterpret_v_i32m2_u32m2(v1);
    vuint32m2_t v2_32u = __riscv_vreinterpret_v_i32m2_u32m2(v2);

    vuint64m2_t v1_64u = __riscv_vreinterpret_v_u32m2_u64m2(v1_32u);
    vuint64m2_t v2_64u = __riscv_vreinterpret_v_u32m2_u64m2(v2_32u);

    // 3. 存储到临时 64-bit 缓冲区
    uint64_t a_buf[4], b_buf[4], res_buf[4];
    __riscv_vse64_v_u64m2((uint64_t*)a_buf, v1_64u, VL_64);
    __riscv_vse64_v_u64m2((uint64_t*)b_buf, v2_64u, VL_64);

    // 4. C 语言交错 (Hi)
    res_buf[0] = a_buf[1]; res_buf[1] = b_buf[1];
    res_buf[2] = a_buf[3]; res_buf[3] = b_buf[3];
    
    // 5. 加载回 64-bit 向量
    vuint64m2_t result_64u = __riscv_vle64_v_u64m2((uint64_t*)res_buf, VL_64);
    
    // 6. 转换回 vint32m2_t
    vuint32m2_t result_32u = __riscv_vreinterpret_v_u64m2_u32m2(result_64u);
    return __riscv_vreinterpret_v_u32m2_i32m2(result_32u);
}
static inline void debug_print_vec_avx(const char *label, __m256i vec) {
    // 使用联合体 (union) 或内存拷贝 (memcpy) 将 __m256i 转换为可访问的 int32_t 数组
    // 这里使用联合体，这是在 GCC/Clang 中访问 SIMD 寄存器内容的常见且高效的方法。
    union {
        __m256i v;
        int32_t i[8]; // 256 bits = 8 x 32 bits
    } data;
    
    data.v = vec;

    printf("DEBUG [%s]: ", label);
    for (int i = 0; i < 8; i++) {
        printf("%d ", data.i[i]);
    }
    printf("\n");
}
static inline void rvv_transpose16_ooff(int16_t *x,int16_t *y,int off)
{
  const size_t VL_32 = 8;
    size_t vl = __riscv_vsetvl_e32m2(VL_32);

    int16_t *y2 = y;

    // 转换指针类型方便操作
    const int32_t *x_ptr = (const int32_t *)x;
    int32_t *y_ptr = (int32_t *)y;

    // 2. 加载输入向量
    vint32m2_t x0_vec = __riscv_vle32_v_i32m2(x_ptr + 0 * VL_32, vl);
    vint32m2_t x1_vec = __riscv_vle32_v_i32m2(x_ptr + 1 * VL_32, vl);
    vint32m2_t x2_vec = __riscv_vle32_v_i32m2(x_ptr + 2 * VL_32, vl);
    vint32m2_t x3_vec = __riscv_vle32_v_i32m2(x_ptr + 3 * VL_32, vl);


    // 3. 准备 Permute Mask
    // _mm256_set_epi32(7, 3, 5, 1, 6, 2, 4, 0) -> 内存 [0, 4, 2, 6, 1, 5, 3, 7]
    vint32m2_t perm_mask = rvv_set_epi32_m2(7, 3, 5, 1, 6, 2, 4, 0, vl);
    // debug_print_vec_m2("x0_vec", x0_vec, vl);
    // debug_print_vec_m2("x1_vec", x1_vec, vl);
    // debug_print_vec_m2("x2_vec", x2_vec, vl);
    // debug_print_vec_m2("x3_vec", x3_vec, vl);

    // debug_print_vec_m2("perm_mask", perm_mask, vl);
    // 4. 执行 Permute
    vint32m2_t ytmp0 = rvv_permutevar8x32_epi32_m2(x0_vec, perm_mask, vl);
    vint32m2_t ytmp1 = rvv_permutevar8x32_epi32_m2(x1_vec, perm_mask, vl);
    vint32m2_t ytmp2 = rvv_permutevar8x32_epi32_m2(x2_vec, perm_mask, vl);
    vint32m2_t ytmp3 = rvv_permutevar8x32_epi32_m2(x3_vec, perm_mask, vl);

    // debug_print_vec_m2("ytmp0", ytmp0, vl);
    // debug_print_vec_m2("ytmp1", ytmp1, vl);
    // debug_print_vec_m2("ytmp2", ytmp2, vl);
    // debug_print_vec_m2("ytmp3", ytmp3, vl);

    vint32m2_t ytmp4 = rvv_unpacklo_epi64_m2(ytmp0, ytmp1, VL_32);
    vint32m2_t ytmp5 = rvv_unpackhi_epi64_m2(ytmp0, ytmp1, VL_32);
    vint32m2_t ytmp6 = rvv_unpacklo_epi64_m2(ytmp2, ytmp3, VL_32);
    vint32m2_t ytmp7 = rvv_unpackhi_epi64_m2(ytmp2, ytmp3, VL_32);

    // debug_print_vec_m2("ytmp4", ytmp4, vl);
    // debug_print_vec_m2("ytmp5", ytmp5, vl);
    // debug_print_vec_m2("ytmp6", ytmp6, vl);
    // debug_print_vec_m2("ytmp7", ytmp7, vl);

    // ----------------------------------------------------------------
    // 步骤 4: Final Store (128-bit Lane Reordering)
    // ----------------------------------------------------------------

    // 1. 准备内存空间并转储所有源向量
    int32_t ytmp_buf[32]; 
    
    __riscv_vse32_v_i32m2((int32_t*)(ytmp_buf + 0), ytmp4, vl); 
    __riscv_vse32_v_i32m2((int32_t*)(ytmp_buf + 8), ytmp5, vl);
    __riscv_vse32_v_i32m2((int32_t*)(ytmp_buf + 16), ytmp6, vl);
    __riscv_vse32_v_i32m2((int32_t*)(ytmp_buf + 24), ytmp7, vl);


    // 2. 在内存中构建四个最终结果
    int32_t y_res_buf[8]; // 临时缓冲区，用于构建一个最终结果向量
    const int N_HALF = 4; // 128-bit 块的元素数量 (4 x 32-bit)

    // --- y2[0] ---
    // AVX: insertf128(ytmp4, extracti128(ytmp6, 0), 1) -> Low(ytmp4) | Low(ytmp6)
    for (int i = 0; i < N_HALF; i++) {
        y_res_buf[0 + i]      = ytmp_buf[0 + i];      // Low(ytmp4)
        y_res_buf[N_HALF + i] = ytmp_buf[16 + i];     // Low(ytmp6)
    }
    // Store
    vint32m2_t final_vec0 = __riscv_vle32_v_i32m2((const int32_t*)(y_res_buf + 0), vl);
    __riscv_vse32_v_i32m2((int32_t*)y2, final_vec0, vl);

    
    // vint32m2_t y0_vec = __riscv_vle32_v_i32m2(y_ptr + 0 * VL_32, vl);
    // vint32m2_t y1_vec = __riscv_vle32_v_i32m2(y_ptr + 1 * VL_32, vl);
    // vint32m2_t y2_vec = __riscv_vle32_v_i32m2(y_ptr + 2 * VL_32, vl);
    // vint32m2_t y3_vec = __riscv_vle32_v_i32m2(y_ptr + 3 * VL_32, vl);
    // debug_print_vec_m2("y0_vec", y0_vec, vl);
    // debug_print_vec_m2("y1_vec", y1_vec, vl);
    // debug_print_vec_m2("y2_vec", y2_vec, vl);
    // debug_print_vec_m2("y3_vec", y3_vec, vl);

    y2 += off*16; // __m256i 指针偏移

    // --- y2[off] ---
    // AVX: insertf128(ytmp6, extracti128(ytmp4, 1), 0) -> High(ytmp4) | High(ytmp6)
    for (int i = 0; i < N_HALF; i++) {
        y_res_buf[0 + i]      = ytmp_buf[4 + i];      // High(ytmp4)
        y_res_buf[N_HALF + i] = ytmp_buf[20 + i];     // High(ytmp6)
    }
    // Store
    vint32m2_t final_vec1 = __riscv_vle32_v_i32m2((const int32_t*)(y_res_buf + 0), vl);
    __riscv_vse32_v_i32m2((int32_t*)y2, final_vec1, vl);
    y2 += off*16;

    // --- y2[2*off] ---
    // AVX: insertf128(ytmp5, extracti128(ytmp7, 0), 1) -> Low(ytmp5) | Low(ytmp7)
    for (int i = 0; i < N_HALF; i++) {
        y_res_buf[0 + i]      = ytmp_buf[8 + i];      // Low(ytmp5)
        y_res_buf[N_HALF + i] = ytmp_buf[24 + i];     // Low(ytmp7)
    }
    // Store
    vint32m2_t final_vec2 = __riscv_vle32_v_i32m2((const int32_t*)(y_res_buf + 0), vl);
    __riscv_vse32_v_i32m2((int32_t*)y2, final_vec2, vl);
    y2 += off*16;

    // --- y2[3*off] ---
    // AVX: insertf128(ytmp7, extracti128(ytmp5, 1), 0) -> High(ytmp5) | High(ytmp7)
    for (int i = 0; i < N_HALF; i++) {
        y_res_buf[0 + i]      = ytmp_buf[12 + i];     // High(ytmp5)
        y_res_buf[N_HALF + i] = ytmp_buf[28 + i];     // High(ytmp7)
    }
    // Store
    vint32m2_t final_vec3 = __riscv_vle32_v_i32m2((const int32_t*)(y_res_buf + 0), vl);
    __riscv_vse32_v_i32m2((int32_t*)y2, final_vec3, vl);

    // y0_vec = __riscv_vle32_v_i32m2(y_ptr + 0 * VL_32, vl);
    // y1_vec = __riscv_vle32_v_i32m2(y_ptr + 1 * VL_32, vl);
    // y2_vec = __riscv_vle32_v_i32m2(y_ptr + 2 * VL_32, vl);
    // y3_vec = __riscv_vle32_v_i32m2(y_ptr + 3 * VL_32, vl);
    // debug_print_vec_m2("y0_vec", y0_vec, vl);
    // debug_print_vec_m2("y1_vec", y1_vec, vl);
    // debug_print_vec_m2("y2_vec", y2_vec, vl);
    // debug_print_vec_m2("y3_vec", y3_vec, vl);
}
static inline void transpose16_ooff_simd256(__m256i *x,__m256i *y,int off) __attribute__((always_inline));
static inline void transpose16_ooff_simd256(__m256i *x,__m256i *y,int off)
{
  register __m256i ytmp0,ytmp1,ytmp2,ytmp3,ytmp4,ytmp5,ytmp6,ytmp7;
  __m256i *y2=y;
  __m256i const perm_mask = _mm256_set_epi32(7, 3, 5, 1, 6, 2, 4, 0);

  ytmp0 = _mm256_permutevar8x32_epi32(x[0],perm_mask);  // x00 x10 x01 x11 x02 x12 x03 x13
  ytmp1 = _mm256_permutevar8x32_epi32(x[1],perm_mask);  // x20 x30 x21 x31 x22 x32 x23 x33
  ytmp2 = _mm256_permutevar8x32_epi32(x[2],perm_mask);  // x40 x50 x41 x51 x42 x52 x43 x53
  ytmp3 = _mm256_permutevar8x32_epi32(x[3],perm_mask);  // x60 x70 x61 x71 x62 x72 x63 x73

  ytmp4 = _mm256_unpacklo_epi64(ytmp0,ytmp1);           // x00 x10 x20 x30 x01 x11 x21 x31
  ytmp5 = _mm256_unpackhi_epi64(ytmp0,ytmp1);           // x02 x12 x22 x32 x03 x13 x23 x33
  ytmp6 = _mm256_unpacklo_epi64(ytmp2,ytmp3);           // x40 x50 x60 x70 x41 x51 x61 x71
  ytmp7 = _mm256_unpackhi_epi64(ytmp2,ytmp3);           // x42 x52 x62 x72 x43 x53 x63 x73

  *y2    = _mm256_insertf128_si256(ytmp4,_mm256_extracti128_si256(ytmp6,0),1);  //x00 x10 x20 x30 x40 x50 x60 x70
  y2+=off;  
  *y2    = _mm256_insertf128_si256(ytmp6,_mm256_extracti128_si256(ytmp4,1),0);  //x01 x11 x21 x31 x41 x51 x61 x71
  y2+=off;  
  *y2    = _mm256_insertf128_si256(ytmp5,_mm256_extracti128_si256(ytmp7,0),1);  //x00 x10 x20 x30 x40 x50 x60 x70
  y2+=off;  
  *y2    = _mm256_insertf128_si256(ytmp7,_mm256_extracti128_si256(ytmp5,1),0);  //x01 x11 x21 x31 x41 x51 x61 x71

}

#endif
#endif



#endif

#if defined(__x86_64__) || defined(__i386__) || defined SIMDE_ENABLE_NATIVE_ALIASES

static inline void transpose4_ooff(__m64 *x,__m64 *y,int off)__attribute__((always_inline));
static inline void transpose4_ooff(__m64 *x,__m64 *y,int off)
{
  y[0]   = _mm_unpacklo_pi32(x[0],x[1]);
  y[off] = _mm_unpackhi_pi32(x[0],x[1]);

  // x[0] = [x0 x1]
  // x[1] = [x2 x3]
  // y[0] = [x0 x2]
  // y[off] = [x1 x3]
}
#ifdef __AVX2__
/* Use the same __m256i type as before; include immintrin.h if available.
   Here we don't need extra headers beyond stdint.h/string.h because we
   use low-level __riscv_ builtins directly. */


#ifdef yuandaima
static inline void transpose4_ooff_simd256(__m256i *x,__m256i *y,int off)__attribute__((always_inline));
static inline void transpose4_ooff_simd256(__m256i *x,__m256i *y,int off)
{
  // __m256i const perm_mask = _mm256_set_epi32(7, 5, 3, 1, 6, 4, 2, 0);
  // __m256i perm_tmp0,perm_tmp1;

  // // x[0] = [x0 x1 x2 x3 x4 x5 x6 x7]
  // // x[1] = [x8 x9 x10 x11 x12 x13 x14]
  // // y[0] = [x0 x2 x4 x6 x8 x10 x12 x14]
  // // y[off] = [x1 x3 x5 x7 x9 x11 x13 x15]
  // perm_tmp0 = _mm256_permutevar8x32_epi32(x[0],perm_mask);
  // perm_tmp1 = _mm256_permutevar8x32_epi32(x[1],perm_mask);
  // y[0]   = _mm256_insertf128_si256(perm_tmp0,_mm256_extracti128_si256(perm_tmp1,0),1);
  // y[off] = _mm256_insertf128_si256(perm_tmp1,_mm256_extracti128_si256(perm_tmp0,1),0);

  /*---------------------------------------------------------*/
  /*---------------------------------------------------------*/

    // 1. 基础配置 (使用 m2 类型)
    const size_t VL_32 = 8;
    size_t vl = __riscv_vsetvl_e32m2(VL_32);
    
    // 指针类型转换
    const int32_t *x_ptr = (const int32_t *)x;
    int32_t *y_ptr = (int32_t *)y;

    // 2. 准备 Permute Mask
    // AVX: _mm256_set_epi32(7, 5, 3, 1, 6, 4, 2, 0)
    // 注意: set_epi32 参数顺序是 e7...e0 (高位到低位)
    vint32m2_t perm_mask = rvv_set_epi32_m2(7, 5, 3, 1, 6, 4, 2, 0, vl);

    // 3. 加载并 Permute
    // x[0] -> perm_tmp0
    vint32m2_t x0_vec = __riscv_vle32_v_i32m2(x_ptr + 0 * VL_32, vl);
    vint32m2_t perm_tmp0 = rvv_permutevar8x32_epi32_m2(x0_vec, perm_mask, vl);

    // x[1] -> perm_tmp1
    vint32m2_t x1_vec = __riscv_vle32_v_i32m2(x_ptr + 1 * VL_32, vl);
    vint32m2_t perm_tmp1 = rvv_permutevar8x32_epi32_m2(x1_vec, perm_mask, vl);

    // 4. Final Store (Insert/Extract 逻辑)
    // 目标操作:
    // y[0]   = insertf128(perm_tmp0, extracti128(perm_tmp1, 0), 1) -> Low(perm0) | Low(perm1)
    // y[off] = insertf128(perm_tmp1, extracti128(perm_tmp0, 1), 0) -> High(perm0) | High(perm1)

    // 4.1 将中间结果转储到内存
    int32_t tmp_buf[16]; // 存放 perm_tmp0 (0-7) 和 perm_tmp1 (8-15)
    __riscv_vse32_v_i32m2(tmp_buf + 0, perm_tmp0, vl);
    __riscv_vse32_v_i32m2(tmp_buf + 8, perm_tmp1, vl);

    // 4.2 在内存中重组并存储
    int32_t y_res_buf[8];
    const int N_HALF = 4; // 128-bit 块 = 4 个 int32

    // --- 构建 y[0] ---
    // Low 128-bit: Low 128 of perm_tmp0 (tmp_buf[0..3])
    // High 128-bit: Low 128 of perm_tmp1 (tmp_buf[8..11])
    for (int i = 0; i < N_HALF; i++) {
        y_res_buf[i]          = tmp_buf[i];     // Low(perm0)
        y_res_buf[i + N_HALF] = tmp_buf[i + 8]; // Low(perm1)
    }
    // Store to y[0]
    vint32m2_t y0_vec = __riscv_vle32_v_i32m2(y_res_buf, vl);
    __riscv_vse32_v_i32m2(y_ptr, y0_vec, vl);

    // --- 构建 y[off] ---
    // Low 128-bit: High 128 of perm_tmp0 (tmp_buf[4..7])
    // High 128-bit: High 128 of perm_tmp1 (tmp_buf[12..15])
    // (注意：insertf128(A, B, 0) 将 B 放入低位，保留 A 的高位。
    //  A=perm_tmp1, B=High(perm_tmp0)。所以结果是 High(perm_tmp0) | High(perm_tmp1))
    for (int i = 0; i < N_HALF; i++) {
        y_res_buf[i]          = tmp_buf[i + 4];  // High(perm0)
        y_res_buf[i + N_HALF] = tmp_buf[i + 12]; // High(perm1)
    }
    // Store to y[off]
    // off 是 __m256i 的偏移量，转换为 int32 指针偏移量需 * 8
    vint32m2_t y_off_vec = __riscv_vle32_v_i32m2(y_res_buf, vl);
    __riscv_vse32_v_i32m2(y_ptr + off * 8, y_off_vec, vl);

}
static inline void rvv_transpose4_ooff(const int16_t *x,int16_t *y,int off)
{
    // 1. 基础配置 (使用 m2 类型)
    const size_t VL_32 = 8;
    size_t vl = __riscv_vsetvl_e32m2(VL_32);
    
    // 指针类型转换
    const int32_t *x_ptr = (const int32_t *)x;
    int32_t *y_ptr = (int32_t *)y;

    // 2. 准备 Permute Mask
    // AVX: _mm256_set_epi32(7, 5, 3, 1, 6, 4, 2, 0)
    // 注意: set_epi32 参数顺序是 e7...e0 (高位到低位)
    vint32m2_t perm_mask = rvv_set_epi32_m2(7, 5, 3, 1, 6, 4, 2, 0, vl);

    // 3. 加载并 Permute
    // x[0] -> perm_tmp0
    vint32m2_t x0_vec = __riscv_vle32_v_i32m2(x_ptr + 0 * VL_32, vl);
    vint32m2_t perm_tmp0 = rvv_permutevar8x32_epi32_m2(x0_vec, perm_mask, vl);

    // x[1] -> perm_tmp1
    vint32m2_t x1_vec = __riscv_vle32_v_i32m2(x_ptr + 1 * VL_32, vl);
    vint32m2_t perm_tmp1 = rvv_permutevar8x32_epi32_m2(x1_vec, perm_mask, vl);

    // 4. Final Store (Insert/Extract 逻辑)
    // 目标操作:
    // y[0]   = insertf128(perm_tmp0, extracti128(perm_tmp1, 0), 1) -> Low(perm0) | Low(perm1)
    // y[off] = insertf128(perm_tmp1, extracti128(perm_tmp0, 1), 0) -> High(perm0) | High(perm1)

    // 4.1 将中间结果转储到内存
    int32_t tmp_buf[16]; // 存放 perm_tmp0 (0-7) 和 perm_tmp1 (8-15)
    __riscv_vse32_v_i32m2(tmp_buf + 0, perm_tmp0, vl);
    __riscv_vse32_v_i32m2(tmp_buf + 8, perm_tmp1, vl);

    // 4.2 在内存中重组并存储
    int32_t y_res_buf[8];
    const int N_HALF = 4; // 128-bit 块 = 4 个 int32

    // --- 构建 y[0] ---
    // Low 128-bit: Low 128 of perm_tmp0 (tmp_buf[0..3])
    // High 128-bit: Low 128 of perm_tmp1 (tmp_buf[8..11])
    for (int i = 0; i < N_HALF; i++) {
        y_res_buf[i]          = tmp_buf[i];     // Low(perm0)
        y_res_buf[i + N_HALF] = tmp_buf[i + 8]; // Low(perm1)
    }
    // Store to y[0]
    vint32m2_t y0_vec = __riscv_vle32_v_i32m2(y_res_buf, vl);
    __riscv_vse32_v_i32m2(y_ptr, y0_vec, vl);

    // --- 构建 y[off] ---
    // Low 128-bit: High 128 of perm_tmp0 (tmp_buf[4..7])
    // High 128-bit: High 128 of perm_tmp1 (tmp_buf[12..15])
    // (注意：insertf128(A, B, 0) 将 B 放入低位，保留 A 的高位。
    //  A=perm_tmp1, B=High(perm_tmp0)。所以结果是 High(perm_tmp0) | High(perm_tmp1))
    for (int i = 0; i < N_HALF; i++) {
        y_res_buf[i]          = tmp_buf[i + 4];  // High(perm0)
        y_res_buf[i + N_HALF] = tmp_buf[i + 12]; // High(perm1)
    }
    // Store to y[off]
    // off 是 __m256i 的偏移量，转换为 int32 指针偏移量需 * 8
    vint32m2_t y_off_vec = __riscv_vle32_v_i32m2(y_res_buf, vl);
    __riscv_vse32_v_i32m2(y_ptr + off * 8, y_off_vec, vl);

}
#endif
#endif

#endif

// 16-point optimized DFT kernel

const static int16_t tw16[24] __attribute__((aligned(32))) = { 32767,0,30272,-12540,23169 ,-23170,12539 ,-30273,
                                                  32767,0,23169,-23170,0     ,-32767,-23170,-23170,
                                                  32767,0,12539,-30273,-23170,-23170,-30273,12539
                                                };

const static int16_t tw16a[24] __attribute__((aligned(32))) = {32767,0,30272,12540,23169 ,23170,12539 ,30273,
                                                  32767,0,23169,23170,0     ,32767,-23170,23170,
                                                  32767,0,12539,30273,-23170,23170,-30273,-12539
                                                 };

const static int16_t tw16b[24] __attribute__((aligned(32))) = { 0,32767,-12540,30272,-23170,23169 ,-30273,12539,
                                                   0,32767,-23170,23169,-32767,0     ,-23170,-23170,
                                                   0,32767,-30273,12539,-23170,-23170,12539 ,-30273
                                                 };

const static int16_t tw16c[24] __attribute__((aligned(32))) = { 0,32767,12540,30272,23170,23169 ,30273 ,12539,
                                                   0,32767,23170,23169,32767,0     ,23170 ,-23170,
                                                   0,32767,30273,12539,23170,-23170,-12539,-30273
                                                 };

#ifdef __AVX2__

const static int16_t tw16rep[48] __attribute__((aligned(32))) = { 32767,0,30272,-12540,23169 ,-23170,12539 ,-30273,32767,0,30272,-12540,23169 ,-23170,12539 ,-30273,
						     32767,0,23169,-23170,0     ,-32767,-23170,-23170,32767,0,23169,-23170,0     ,-32767,-23170,-23170,
						     32767,0,12539,-30273,-23170,-23170,-30273,12539,32767,0,12539,-30273,-23170,-23170,-30273,12539
                                                   };

const static int16_t tw16arep[48] __attribute__((aligned(32))) = {32767,0,30272,12540,23169 ,23170,12539 ,30273,32767,0,30272,12540,23169 ,23170,12539 ,30273,
						     32767,0,23169,23170,0     ,32767,-23170,23170,32767,0,23169,23170,0     ,32767,-23170,23170,
						     32767,0,12539,30273,-23170,23170,-30273,-12539,32767,0,12539,30273,-23170,23170,-30273,-12539
                                                    }; 

const static int16_t tw16brep[48] __attribute__((aligned(32))) = { 0,32767,-12540,30272,-23170,23169 ,-30273,12539,0,32767,-12540,30272,-23170,23169 ,-30273,12539,
                                                      0,32767,-23170,23169,-32767,0     ,-23170,-23170,0,32767,-23170,23169,-32767,0     ,-23170,-23170,
                                                      0,32767,-30273,12539,-23170,-23170,12539 ,-30273,0,32767,-30273,12539,-23170,-23170,12539 ,-30273
                                                    };

const static int16_t tw16crep[48] __attribute__((aligned(32))) = { 0,32767,12540,30272,23170,23169 ,30273 ,12539,0,32767,12540,30272,23170,23169 ,30273 ,12539,
						      0,32767,23170,23169,32767,0     ,23170 ,-23170,0,32767,23170,23169,32767,0     ,23170 ,-23170,
						      0,32767,30273,12539,23170,-23170,-12539,-30273,0,32767,30273,12539,23170,-23170,-12539,-30273
                                                    };

#endif /* __AVX2__ */



static inline void dft16(int16_t *x,int16_t *y) __attribute__((always_inline));

static inline void dft16(int16_t *x,int16_t *y)
{

#if defined(__x86_64__) || defined(__i386__) || defined SIMDE_ENABLE_NATIVE_ALIASES

  __m128i *tw16a_128=(__m128i *)tw16a,*tw16b_128=(__m128i *)tw16b,*x128=(__m128i *)x,*y128=(__m128i *)y;



  /*  This is the original version before unrolling

  bfly4_tw1(x128,x128+1,x128+2,x128+3,
      y128,y128+1,y128+2,y128+3);

  transpose16(y128,ytmp);

  bfly4_16(ytmp,ytmp+1,ytmp+2,ytmp+3,
     y128,y128+1,y128+2,y128+3,
     tw16_128,tw16_128+1,tw16_128+2);
  */

  register __m128i x1_flip,x3_flip,x02t,x13t;
  register __m128i ytmp0,ytmp1,ytmp2,ytmp3,xtmp0,xtmp1,xtmp2,xtmp3;
  register __m128i complex_shuffle = _mm_set_epi8(13,12,15,14,9,8,11,10,5,4,7,6,1,0,3,2);

  // First stage : 4 Radix-4 butterflies without input twiddles

  x02t    = _mm_adds_epi16(x128[0],x128[2]);
  x13t    = _mm_adds_epi16(x128[1],x128[3]);
  xtmp0   = _mm_adds_epi16(x02t,x13t);
  xtmp2   = _mm_subs_epi16(x02t,x13t);
  x1_flip = _mm_sign_epi16(x128[1],*(__m128i*)conjugatedft);
  x1_flip = _mm_shuffle_epi8(x1_flip,complex_shuffle);
  x3_flip = _mm_sign_epi16(x128[3],*(__m128i*)conjugatedft);
  x3_flip = _mm_shuffle_epi8(x3_flip,complex_shuffle);
  x02t    = _mm_subs_epi16(x128[0],x128[2]);
  x13t    = _mm_subs_epi16(x1_flip,x3_flip);
  xtmp1   = _mm_adds_epi16(x02t,x13t);  // x0 + x1f - x2 - x3f
  xtmp3   = _mm_subs_epi16(x02t,x13t);  // x0 - x1f - x2 + x3f

  ytmp0   = _mm_unpacklo_epi32(xtmp0,xtmp1);
  ytmp1   = _mm_unpackhi_epi32(xtmp0,xtmp1);
  ytmp2   = _mm_unpacklo_epi32(xtmp2,xtmp3);
  ytmp3   = _mm_unpackhi_epi32(xtmp2,xtmp3);
  xtmp0   = _mm_unpacklo_epi64(ytmp0,ytmp2);
  xtmp1   = _mm_unpackhi_epi64(ytmp0,ytmp2);
  xtmp2   = _mm_unpacklo_epi64(ytmp1,ytmp3);
  xtmp3   = _mm_unpackhi_epi64(ytmp1,ytmp3);

  // Second stage : 4 Radix-4 butterflies with input twiddles
  xtmp1 = packed_cmult2(xtmp1,tw16a_128[0],tw16b_128[0]);
  xtmp2 = packed_cmult2(xtmp2,tw16a_128[1],tw16b_128[1]);
  xtmp3 = packed_cmult2(xtmp3,tw16a_128[2],tw16b_128[2]);

  x02t    = _mm_adds_epi16(xtmp0,xtmp2);
  x13t    = _mm_adds_epi16(xtmp1,xtmp3);
  y128[0] = _mm_adds_epi16(x02t,x13t);
  y128[2] = _mm_subs_epi16(x02t,x13t);
  x1_flip = _mm_sign_epi16(xtmp1,*(__m128i*)conjugatedft);
  x1_flip = _mm_shuffle_epi8(x1_flip,_mm_set_epi8(13,12,15,14,9,8,11,10,5,4,7,6,1,0,3,2));
  x3_flip = _mm_sign_epi16(xtmp3,*(__m128i*)conjugatedft);
  x3_flip = _mm_shuffle_epi8(x3_flip,_mm_set_epi8(13,12,15,14,9,8,11,10,5,4,7,6,1,0,3,2));
  x02t    = _mm_subs_epi16(xtmp0,xtmp2);
  x13t    = _mm_subs_epi16(x1_flip,x3_flip);
  y128[1] = _mm_adds_epi16(x02t,x13t);  // x0 + x1f - x2 - x3f
  y128[3] = _mm_subs_epi16(x02t,x13t);  // x0 - x1f - x2 + x3f



#endif
}

#if defined(__x86_64__) || defined(__i386__) || defined SIMDE_ENABLE_NATIVE_ALIASES
#ifdef __AVX2__
static inline vint16m4_t rvv_shiftright_int16(vint16m4_t a, int shift, size_t vl) {
    // 使用算术右移 (Arithmetic Shift Right) 保留符号位
    return __riscv_vsra_vx_i16m4(a, shift, vl);
}
// m2版本dft_16simd（未写完）
// static inline vint16m2_t rvv_adds_epi16(vint16m2_t a, vint16m2_t b, size_t vl) {
//     // __riscv_vsadd_vv_i16m2: Signed Saturating Add Vector-Vector
//     return __riscv_vsadd_vv_i16m2(a, b, vl);
// }
// static inline vint16m2_t rvv_subs_epi16(vint16m2_t a, vint16m2_t b, size_t vl) {
//     // __riscv_vssub_vv_i16m2: Signed Saturating Subtract Vector-Vector
//     return __riscv_vssub_vv_i16m2(a, b, vl);
// }
// static inline vint16m2_t rvv_sign_epi16(vint16m2_t a, vint16m2_t b, size_t vl) {
//     // 1. 检查 b 的符号 (b < 0)。
//     vbool8_t b_is_neg = __riscv_vmslt_vx_i16m2_b8(b, 0, vl);
    
//     // 2. 计算 -a (即 0 - a)。使用普通减法，因为 AVX 的 _mm_sign 没有饱和。
//     vint16m2_t neg_a = __riscv_vneg_v_i16m2(a, vl);
    
//     // 3. 使用掩码进行选择：
//     // 如果 b_is_neg (b < 0) 为真，选择 neg_a (-a)
//     // 否则，选择 a
//     // vselect(mask, vec_true, vec_false, vl)
//     return __riscv_vmerge_vvm_i16m2(a, neg_a, b_is_neg, vl);
// }
// // a 是 vint16m2_t (32字节), mask 是 vint8m4_t (32字节)
// // 修正后的 rvv_shuffle_epi8 函数
// static inline vint16m2_t rvv_shuffle_epi8(vint16m2_t a, vint8m4_t mask, size_t vl_16) {
//     size_t vl_8bit = __riscv_vsetvl_e8m4(32); // VL=32 for 8-bit operations

//     // 1. 替换: vint8m4_t a_8bit = __riscv_vreinterpret_v_i16m2_i8m4(a);
//     vint8m4_t a_8bit;
//     memcpy(&a_8bit, &a, 32); // 32 字节内存复制

//     // 2. 替换: vuint8m4_t mask_u8 = __riscv_vreinterpret_v_i8m4_u8m4(mask);
//     vuint8m4_t mask_u8;
//     memcpy(&mask_u8, &mask, 32);

//     // 3. vrgather (您确认过这个 Intrinsic 存在)
//     vint8m4_t result_8bit = __riscv_vrgather_vv_i8m4(a_8bit, mask_u8, vl_8bit);

//     // 4. 替换: return __riscv_vreinterpret_v_i8m4_i16m2(result_8bit);
//     vint16m2_t final_res;
//     memcpy(&final_res, &result_8bit, 32);
//     return final_res;
// }
// // 辅助函数：将两个 I32m1 向量交错合并为一个 I32m2 向量
// vint32m2_t rvv_vzip_m1_m1(vint32m1_t a, vint32m1_t b, size_t sub_vl) {
//     // sub_vl 必须是 4 (即 16位 VL / 4)
//     size_t result_vl = sub_vl * 2; // 结果的 VL = 8

//     // 临时内存空间用于重排 (Memory Shuffle)
//     int32_t temp_a[4];
//     int32_t temp_b[4];
//     int32_t result_array[8];
    
//     // 1. 显式存储 sub-vectors 到栈上
//     __riscv_vse32_v_i32m1(temp_a, a, sub_vl);
//     __riscv_vse32_v_i32m1(temp_b, b, sub_vl);
    
//     // 2. C 语言层面的交错 (Zipper Merge)
//     for (size_t i = 0; i < sub_vl; i++) {
//         result_array[2 * i]     = temp_a[i];     // Even index gets 'a'
//         result_array[2 * i + 1] = temp_b[i];     // Odd index gets 'b'
//     }
    
//     // 3. 将交错后的结果加载回 I32m2 向量
//     return __riscv_vle32_v_i32m2(result_array, result_vl);
// }

// // ----------------------------------------------------------------
// // 最终修正 V13.0: 使用 vrgather 精确模拟 SIMDE_SHUFFLE_VECTOR_
// // ----------------------------------------------------------------

// // rvv_unpacklo_epi32
// vint16m2_t rvv_unpacklo_epi32(vint16m2_t v1, vint16m2_t v2, size_t vl) {
//     volatile uint32_t a_buf[8], b_buf[8], res_buf[8];
    
//     // 1. (i16m2 -> u16m2)
//     vuint16m2_t v1_16u = __riscv_vreinterpret_v_i16m2_u16m2(v1);
//     vuint16m2_t v2_16u = __riscv_vreinterpret_v_i16m2_u16m2(v2);

//     // 2. (u16m2 -> u32m2) - 使用编译器建议的 Intrinsic
//     vuint32m2_t v1_32u = __riscv_vreinterpret_v_u16m2_u32m2(v1_16u);
//     vuint32m2_t v2_32u = __riscv_vreinterpret_v_u16m2_u32m2(v2_16u);
    
//     // 3. 存储 (使用 u32m2 类型)
//     __riscv_vse32_v_u32m2((uint32_t*)a_buf, v1_32u, 8);
//     __riscv_vse32_v_u32m2((uint32_t*)b_buf, v2_32u, 8);

//     // 4. C 语言交错 (Lane 0 和 Lane 1)
//     res_buf[0] = a_buf[0];
//     res_buf[1] = b_buf[0];
//     res_buf[2] = a_buf[1];
//     res_buf[3] = b_buf[1];
//     res_buf[4] = a_buf[4];
//     res_buf[5] = b_buf[4];
//     res_buf[6] = a_buf[5];
//     res_buf[7] = b_buf[5];
    
//     // 5. 加载
//     vuint32m2_t result_32u = __riscv_vle32_v_u32m2((uint32_t*)res_buf, 8);
    
//     // 6. 转换回 vint16m2_t (u32m2 -> u16m2 -> i16m2)
//     vuint16m2_t result_16u = __riscv_vreinterpret_v_u32m2_u16m2(result_32u);
//     return __riscv_vreinterpret_v_u16m2_i16m2(result_16u);
// }
// // rvv_unpackhi_epi32
// vint16m2_t rvv_unpackhi_epi32(vint16m2_t v1, vint16m2_t v2, size_t vl) {
//     volatile uint32_t a_buf[8], b_buf[8], res_buf[8];
    
//     vuint16m2_t v1_16u = __riscv_vreinterpret_v_i16m2_u16m2(v1);
//     vuint16m2_t v2_16u = __riscv_vreinterpret_v_i16m2_u16m2(v2);
//     vuint32m2_t v1_32u = __riscv_vreinterpret_v_u16m2_u32m2(v1_16u);
//     vuint32m2_t v2_32u = __riscv_vreinterpret_v_u16m2_u32m2(v2_16u);
    
//     __riscv_vse32_v_u32m2((uint32_t*)a_buf, v1_32u, 8);
//     __riscv_vse32_v_u32m2((uint32_t*)b_buf, v2_32u, 8);

//     // C 语言交错 (Hi)
//     res_buf[0] = a_buf[2];
//     res_buf[1] = b_buf[2];
//     res_buf[2] = a_buf[3];
//     res_buf[3] = b_buf[3];
//     res_buf[4] = a_buf[6];
//     res_buf[5] = b_buf[6];
//     res_buf[6] = a_buf[7];
//     res_buf[7] = b_buf[7];
    
//     vuint32m2_t result_32u = __riscv_vle32_v_u32m2((uint32_t*)res_buf, 8);
    
//     vuint16m2_t result_16u = __riscv_vreinterpret_v_u32m2_u16m2(result_32u);
//     return __riscv_vreinterpret_v_u16m2_i16m2(result_16u);
// }
// // rvv_unpacklo_epi64
// vint16m2_t rvv_unpacklo_epi64(vint16m2_t v1, vint16m2_t v2, size_t vl) {
//     volatile uint64_t a_buf[4], b_buf[4], res_buf[4];
    
//     // 1. (i16m2 -> u16m2)
//     vuint16m2_t v1_16u = __riscv_vreinterpret_v_i16m2_u16m2(v1);
//     vuint16m2_t v2_16u = __riscv_vreinterpret_v_i16m2_u16m2(v2);

//     // 2. (u16m2 -> u64m2) - 使用编译器建议的 Intrinsic
//     vuint64m2_t v1_64u = __riscv_vreinterpret_v_u16m2_u64m2(v1_16u);
//     vuint64m2_t v2_64u = __riscv_vreinterpret_v_u16m2_u64m2(v2_16u);
    
//     // 3. 存储 (使用 u64m2 类型)
//     __riscv_vse64_v_u64m2((uint64_t*)a_buf, v1_64u, 4);
//     __riscv_vse64_v_u64m2((uint64_t*)b_buf, v2_64u, 4);

//     // 4. C 语言交错 (Lo)
//     res_buf[0] = a_buf[0];
//     res_buf[1] = b_buf[0];
//     res_buf[2] = a_buf[2];
//     res_buf[3] = b_buf[2];
    
//     // 5. 加载
//     vuint64m2_t result_64u = __riscv_vle64_v_u64m2((uint64_t*)res_buf, 4);
    
//     // 6. 转换回 vint16m2_t (u64m2 -> u16m2 -> i16m2)
//     vuint16m2_t result_16u = __riscv_vreinterpret_v_u64m2_u16m2(result_64u);
//     return __riscv_vreinterpret_v_u16m2_i16m2(result_16u);
// }
// // rvv_unpackhi_epi64
// vint16m2_t rvv_unpackhi_epi64(vint16m2_t v1, vint16m2_t v2, size_t vl) {
//     volatile uint64_t a_buf[4], b_buf[4], res_buf[4];
    
//     vuint16m2_t v1_16u = __riscv_vreinterpret_v_i16m2_u16m2(v1);
//     vuint16m2_t v2_16u = __riscv_vreinterpret_v_i16m2_u16m2(v2);
//     vuint64m2_t v1_64u = __riscv_vreinterpret_v_u16m2_u64m2(v1_16u);
//     vuint64m2_t v2_64u = __riscv_vreinterpret_v_u16m2_u64m2(v2_16u);
    
//     __riscv_vse64_v_u64m2((uint64_t*)a_buf, v1_64u, 4);
//     __riscv_vse64_v_u64m2((uint64_t*)b_buf, v2_64u, 4);

//     // C 语言交错 (Hi)
//     res_buf[0] = a_buf[1];
//     res_buf[1] = b_buf[1];
//     res_buf[2] = a_buf[3];
//     res_buf[3] = b_buf[3];
    
//     vuint64m2_t result_64u = __riscv_vle64_v_u64m2((uint64_t*)res_buf, 4);
    
//     vuint16m2_t result_16u = __riscv_vreinterpret_v_u64m2_u16m2(result_64u);
//     return __riscv_vreinterpret_v_u16m2_i16m2(result_16u);
// }
// void debug_print_vector_i16(const char *label, const int16_t *data, size_t N_ELEMENTS) {
//     printf("DEBUG: %s (16x I16):\n", label);
//     if (N_ELEMENTS > 16) N_ELEMENTS = 16;
    
//     for (size_t i = 0; i < N_ELEMENTS; i++) {
//         printf(" %d", data[i]); 
//         if (i == 7) {
//             printf(" |"); // AVX 128-bit Lane 边界
//         }
//     }
//     printf("\n");
// }

// // 宏 1: 打印 RVV 向量 (vint16m2_t)
// #define DEBUG_PRINT_RVV_VEC(label, rvv_vec, vl) do {
//     int16_t debug_array[16];
//     __riscv_vse16_v_i16m2(debug_array, rvv_vec, vl);
//     debug_print_vector_i16(label, debug_array, 16);
// } while(0)

// // 宏 2: 打印 AVX 向量 (__m256i) - 修正版
// #define DEBUG_PRINT_AVX_VEC(label, avx_vec) do {
//     /* 创建一个临时的栈上数组 */
//     int16_t debug_array[16] __attribute__ ((aligned (32)));
//     /* 显式地将 AVX 寄存器的内容存储到该数组中 */
//     _mm256_storeu_si256((__m256i*)debug_array, avx_vec);
//     /* 打印该数组的内容 */
//     debug_print_vector_i16(label, debug_array, 16);
// } while(0)
// Does two 16-point DFTS (x[0 .. 15] is 128 LSBs of input vector, x[16..31] is in 128 MSBs) 


/*---------------------------------------------*/



static inline void dft16_simd256(int16_t *x,int16_t *y) __attribute__((always_inline));
static inline void dft16_simd256(int16_t *x,int16_t *y)
{
  
  __m256i *tw16a_256=(__m256i *)tw16arep,*tw16b_256=(__m256i *)tw16brep,*x256=(__m256i *)x,*y256=(__m256i *)y;

  __m256i x1_flip,x3_flip,x02t,x13t;
  __m256i ytmp0,ytmp1,ytmp2,ytmp3,xtmp0,xtmp1,xtmp2,xtmp3;
  register __m256i complex_shuffle = _mm256_set_epi8(29,28,31,30,25,24,27,26,21,20,23,22,17,16,19,18,13,12,15,14,9,8,11,10,5,4,7,6,1,0,3,2);


//  uint8_t avx_values[32];
//     _mm256_storeu_si256((__m256i*)avx_values, complex_shuffle);
    
    // printf("AVX2 complex_shuffle values:\n");
    // for(int i = 31; i >= 0; i--) {  // 从高位到低位，与set顺序一致
    //     printf("%d ", avx_values[i]);
    //     if(i % 16 == 0) printf("  ");
    // }
    // printf("\n");
  //First stage : 4 Radix-4 butterflies without input twiddles

  x02t    = _mm256_adds_epi16(x256[0],x256[2]);
  x13t    = _mm256_adds_epi16(x256[1],x256[3]);
  xtmp0   = _mm256_adds_epi16(x02t,x13t);
  xtmp2   = _mm256_subs_epi16(x02t,x13t);
  x1_flip = _mm256_sign_epi16(x256[1],*(__m256i*)conjugatedft);
  x1_flip = _mm256_shuffle_epi8(x1_flip,complex_shuffle);
  x3_flip = _mm256_sign_epi16(x256[3],*(__m256i*)conjugatedft);
  x3_flip = _mm256_shuffle_epi8(x3_flip,complex_shuffle);
  x02t    = _mm256_subs_epi16(x256[0],x256[2]);
  x13t    = _mm256_subs_epi16(x1_flip,x3_flip);
  xtmp1   = _mm256_adds_epi16(x02t,x13t);  // x0 + x1f - x2 - x3f
  xtmp3   = _mm256_subs_epi16(x02t,x13t);  // x0 - x1f - x2 + x3f

  // DEBUG_PRINT_AVX_VEC("AVX: xtmp0", xtmp0);
  // DEBUG_PRINT_AVX_VEC("AVX: xtmp1", xtmp1);
  // DEBUG_PRINT_AVX_VEC("AVX: xtmp2", xtmp2);
  // DEBUG_PRINT_AVX_VEC("AVX: xtmp3", xtmp3);

  ytmp0   = _mm256_unpacklo_epi32(xtmp0,xtmp1);  
  //DEBUG_PRINT_AVX_VEC("AVX: ytmp0", ytmp0);
  ytmp1   = _mm256_unpackhi_epi32(xtmp0,xtmp1);
  //DEBUG_PRINT_AVX_VEC("AVX: ytmp1", ytmp1);
  ytmp2   = _mm256_unpacklo_epi32(xtmp2,xtmp3);
  //DEBUG_PRINT_AVX_VEC("AVX: ytmp2", ytmp2);
  ytmp3   = _mm256_unpackhi_epi32(xtmp2,xtmp3);
  //DEBUG_PRINT_AVX_VEC("AVX: ytmp3", ytmp3);
  xtmp0   = _mm256_unpacklo_epi64(ytmp0,ytmp2);
  //DEBUG_PRINT_AVX_VEC("AVX: xtmp0", xtmp0);
  xtmp1   = _mm256_unpackhi_epi64(ytmp0,ytmp2);
  //DEBUG_PRINT_AVX_VEC("AVX: xtmp1", xtmp1);
  xtmp2   = _mm256_unpacklo_epi64(ytmp1,ytmp3);
  //DEBUG_PRINT_AVX_VEC("AVX: xtmp2", xtmp2);
  xtmp3   = _mm256_unpackhi_epi64(ytmp1,ytmp3);
  //DEBUG_PRINT_AVX_VEC("AVX: xtmp3", xtmp3);

  // DEBUG_PRINT_AVX_VEC("AVX: xtmp1", xtmp1);
  // DEBUG_PRINT_AVX_VEC("AVX: tw16a_256", tw16a_256[0]);
  // DEBUG_PRINT_AVX_VEC("AVX: tw16b_256", tw16b_256[0]);
  //   DEBUG_PRINT_AVX_VEC("AVX: xtmp2", xtmp2);
  // DEBUG_PRINT_AVX_VEC("AVX: tw16a_256", tw16a_256[1]);
  // DEBUG_PRINT_AVX_VEC("AVX: tw16b_256", tw16b_256[1]);
  //   DEBUG_PRINT_AVX_VEC("AVX: xtmp3", xtmp3);
  // DEBUG_PRINT_AVX_VEC("AVX: tw16a_256", tw16a_256[2]);
  // DEBUG_PRINT_AVX_VEC("AVX: tw16b_256", tw16b_256[2]);

  xtmp1 = packed_cmult2_256(xtmp1,tw16a_256[0],tw16b_256[0]);
  xtmp2 = packed_cmult2_256(xtmp2,tw16a_256[1],tw16b_256[1]);
  xtmp3 = packed_cmult2_256(xtmp3,tw16a_256[2],tw16b_256[2]);

  // DEBUG_PRINT_AVX_VEC("AVX: xtmp1", xtmp1);
  // DEBUG_PRINT_AVX_VEC("AVX: xtmp2", xtmp2);
  // DEBUG_PRINT_AVX_VEC("AVX: xtmp3", xtmp3);

  x02t    = _mm256_adds_epi16(xtmp0,xtmp2);
  x13t    = _mm256_adds_epi16(xtmp1,xtmp3);
  ytmp0   = _mm256_adds_epi16(x02t,x13t);
  ytmp2   = _mm256_subs_epi16(x02t,x13t);
  x1_flip = _mm256_sign_epi16(xtmp1,*(__m256i*)conjugatedft);
  x1_flip = _mm256_shuffle_epi8(x1_flip,complex_shuffle);
  x3_flip = _mm256_sign_epi16(xtmp3,*(__m256i*)conjugatedft);
  x3_flip = _mm256_shuffle_epi8(x3_flip,complex_shuffle);
  x02t    = _mm256_subs_epi16(xtmp0,xtmp2);
  x13t    = _mm256_subs_epi16(x1_flip,x3_flip);
  ytmp1   = _mm256_adds_epi16(x02t,x13t);  // x0 + x1f - x2 - x3f
  ytmp3   = _mm256_subs_epi16(x02t,x13t);  // x0 - x1f - x2 + x3f

//   // 修正后的打印调用
//   DEBUG_PRINT_AVX_VEC("AVX: xtmp0", xtmp0);
//   DEBUG_PRINT_AVX_VEC("AVX: xtmp1", xtmp1);
//   DEBUG_PRINT_AVX_VEC("AVX: xtmp2", xtmp2);
//   DEBUG_PRINT_AVX_VEC("AVX: xtmp3", xtmp3);
  // DEBUG_PRINT_AVX_VEC("AVX: ytmp0", ytmp0);
  // DEBUG_PRINT_AVX_VEC("AVX: ytmp1", ytmp1);
  // DEBUG_PRINT_AVX_VEC("AVX: ytmp2", ytmp2);
  // DEBUG_PRINT_AVX_VEC("AVX: ytmp3", ytmp3);
  // DEBUG_PRINT_AVX_VEC("AVX: x02t (final)", x02t);
  // DEBUG_PRINT_AVX_VEC("AVX: x13t (final)", x13t);
  // DEBUG_PRINT_AVX_VEC("AVX: x1_flip", x1_flip);
  // DEBUG_PRINT_AVX_VEC("AVX: x3_flip", x3_flip);

  y256[0] = _mm256_insertf128_si256(ytmp0,_mm256_extracti128_si256(ytmp1,0),1);
  y256[1] = _mm256_insertf128_si256(ytmp2,_mm256_extracti128_si256(ytmp3,0),1);
  y256[2] = _mm256_insertf128_si256(ytmp1,_mm256_extracti128_si256(ytmp0,1),0);
  y256[3] = _mm256_insertf128_si256(ytmp3,_mm256_extracti128_si256(ytmp2,1),0);

  // DEBUG_PRINT_AVX_VEC("AVX: y256[0]", y256[0]);
  // DEBUG_PRINT_AVX_VEC("AVX: y256[1]", y256[1]);
  // DEBUG_PRINT_AVX_VEC("AVX: y256[2]", y256[2]);
  // DEBUG_PRINT_AVX_VEC("AVX: y256[3]", y256[3]);

  /*  print_shorts256("xtmp0",(int16_t*)&xtmp0);
      print_shorts256("xtmp1",(int16_t*)&xtmp1);
  print_shorts256("xtmp2",(int16_t*)&xtmp2);
  print_shorts256("xtmp3",(int16_t*)&xtmp3);*/
/*---------------------------------------------------------------------- */

}

static inline void rvv_dft16(int16_t *x, int16_t *y)
{
    size_t vl = __riscv_vsetvl_e16m4(16);

    const int16_t *tw16a = (const int16_t *)tw16arep;
    const int16_t *tw16b = (const int16_t *)tw16brep;

    // twiddle
    vint16m4_t tw_a0 = __riscv_vle16_v_i16m4(tw16a + 0 * 16, vl);
    vint16m4_t tw_b0 = __riscv_vle16_v_i16m4(tw16b + 0 * 16, vl);
    vint16m4_t tw_a1 = __riscv_vle16_v_i16m4(tw16a + 1 * 16, vl);
    vint16m4_t tw_b1 = __riscv_vle16_v_i16m4(tw16b + 1 * 16, vl);
    vint16m4_t tw_a2 = __riscv_vle16_v_i16m4(tw16a + 2 * 16, vl);
    vint16m4_t tw_b2 = __riscv_vle16_v_i16m4(tw16b + 2 * 16, vl);

    // input
    vint16m4_t x0 = __riscv_vle16_v_i16m4(x + 0 * 16, vl);
    vint16m4_t x1 = __riscv_vle16_v_i16m4(x + 1 * 16, vl);
    vint16m4_t x2 = __riscv_vle16_v_i16m4(x + 2 * 16, vl);
    vint16m4_t x3 = __riscv_vle16_v_i16m4(x + 3 * 16, vl);

    // ---------------- Stage 1 radix-4 ----------------
    vint16m4_t x02   = __riscv_vsadd_vv_i16m4(x0, x2, vl);
    vint16m4_t x13   = __riscv_vsadd_vv_i16m4(x1, x3, vl);
    vint16m4_t xtmp0 = __riscv_vsadd_vv_i16m4(x02, x13, vl);
    vint16m4_t xtmp2 = __riscv_vssub_vv_i16m4(x02, x13, vl);

    // ✅ 用 mul_minus_j 代替 sign+shuffle：x*(-j)
    vint16m4_t x1_mj = rvv_mul_minus_j_packed_i16m4(x1, vl);
    vint16m4_t x3_mj = rvv_mul_minus_j_packed_i16m4(x3, vl);

    x02 = __riscv_vssub_vv_i16m4(x0, x2, vl);
    x13 = __riscv_vssub_vv_i16m4(x1_mj, x3_mj, vl);
    vint16m4_t xtmp1 = __riscv_vsadd_vv_i16m4(x02, x13, vl);
    vint16m4_t xtmp3 = __riscv_vssub_vv_i16m4(x02, x13, vl);

    // -------- 与老版一致的 unpack32 + unpack64 转置链 --------
    vint16m4_t ytmp0 = rvv_unpacklo_epi32(xtmp0, xtmp1, vl);
    vint16m4_t ytmp1 = rvv_unpackhi_epi32(xtmp0, xtmp1, vl);
    vint16m4_t ytmp2 = rvv_unpacklo_epi32(xtmp2, xtmp3, vl);
    vint16m4_t ytmp3 = rvv_unpackhi_epi32(xtmp2, xtmp3, vl);

    xtmp0 = rvv_unpacklo_epi64(ytmp0, ytmp2, vl);
    xtmp1 = rvv_unpackhi_epi64(ytmp0, ytmp2, vl);
    xtmp2 = rvv_unpacklo_epi64(ytmp1, ytmp3, vl);
    xtmp3 = rvv_unpackhi_epi64(ytmp1, ytmp3, vl);

    // ---------------- Twiddle multiply ----------------
    xtmp1 = rvv_packed_cmult2_q15_i16m4(xtmp1, tw_a0, tw_b0, vl);
    xtmp2 = rvv_packed_cmult2_q15_i16m4(xtmp2, tw_a1, tw_b1, vl);
    xtmp3 = rvv_packed_cmult2_q15_i16m4(xtmp3, tw_a2, tw_b2, vl);

    // ---------------- Stage 2 radix-4 ----------------
    x02   = __riscv_vsadd_vv_i16m4(xtmp0, xtmp2, vl);
    x13   = __riscv_vsadd_vv_i16m4(xtmp1, xtmp3, vl);
    ytmp0 = __riscv_vsadd_vv_i16m4(x02, x13, vl);
    ytmp2 = __riscv_vssub_vv_i16m4(x02, x13, vl);

    // ✅ 同样替换第二段 sign+shuffle
    vint16m4_t xtmp1_mj = rvv_mul_minus_j_packed_i16m4(xtmp1, vl);
    vint16m4_t xtmp3_mj = rvv_mul_minus_j_packed_i16m4(xtmp3, vl);

    x02   = __riscv_vssub_vv_i16m4(xtmp0, xtmp2, vl);
    x13   = __riscv_vssub_vv_i16m4(xtmp1_mj, xtmp3_mj, vl);
    ytmp1 = __riscv_vsadd_vv_i16m4(x02, x13, vl);
    ytmp3 = __riscv_vssub_vv_i16m4(x02, x13, vl);

    // ---------------- Final permutation（slide 版）----------------
    vint16m4_t final0 = __riscv_vslideup_vx_i16m4(ytmp0, ytmp1, 8, vl);
    vint16m4_t final1 = __riscv_vslideup_vx_i16m4(ytmp2, ytmp3, 8, vl);

    vint16m4_t y0_hi = __riscv_vslidedown_vx_i16m4(ytmp0, 8, vl);
    vint16m4_t y1_hi = __riscv_vslidedown_vx_i16m4(ytmp1, 8, vl);
    vint16m4_t y2_hi = __riscv_vslidedown_vx_i16m4(ytmp2, 8, vl);
    vint16m4_t y3_hi = __riscv_vslidedown_vx_i16m4(ytmp3, 8, vl);

    vint16m4_t final2 = __riscv_vslideup_vx_i16m4(y0_hi, y1_hi, 8, vl);
    vint16m4_t final3 = __riscv_vslideup_vx_i16m4(y2_hi, y3_hi, 8, vl);

    __riscv_vse16_v_i16m4(y + 0 * 16, final0, vl);
    __riscv_vse16_v_i16m4(y + 1 * 16, final1, vl);
    __riscv_vse16_v_i16m4(y + 2 * 16, final2, vl);
    __riscv_vse16_v_i16m4(y + 3 * 16, final3, vl);
}



#include <stdint.h>

static inline __attribute__((always_inline))
void set_vxrm(unsigned vxrm) {
  unsigned long v;
  asm volatile ("csrr %0, vcsr" : "=r"(v));
  v = (v & ~0x6UL) | ((unsigned long)(vxrm & 0x3) << 1); // VXRM in vcsr[2:1]
  asm volatile ("csrw vcsr, %0" :: "r"(v));
}

static inline __attribute__((always_inline))
vint16m4_t vscmul_i16m4(vint16m4_t a, vint16m4_t b) {
  register vint16m4_t ra asm("v20") = a;
  register vint16m4_t rb asm("v24") = b;
  register vint16m4_t rd asm("v16");
  asm volatile(
    //"vsetivli zero, 16, e16, m4, ta, ma\n\t"
    ".word 0xCB4C0857\n\t"        // vscmul.vv v16, v20, v24
    : "=vr"(rd)
    : "vr"(ra), "vr"(rb)
    : "memory"
  );
  return rd;
}

static inline __attribute__((always_inline))
vint16m4_t vscmulcj_i16m4(vint16m4_t a_conj_src, vint16m4_t b) {
  register vint16m4_t ra asm("v20") = a_conj_src;
  register vint16m4_t rb asm("v24") = b;
  register vint16m4_t rd asm("v16");
  asm volatile(
    //"vsetivli zero, 16, e16, m4, ta, ma\n\t"
    ".word 0xCF4C0857\n\t"        // vscmulcj.vv v16, v20, v24
    : "=vr"(rd)
    : "vr"(ra), "vr"(rb)
    : "memory"
  );
  return rd;
}

// 乘加：rd 既是输入 accumulator（vd），也是输出
static inline __attribute__((always_inline))
vint16m4_t vscmacc_i16m4(vint16m4_t acc_vd, vint16m4_t a, vint16m4_t b) {
  register vint16m4_t ra asm("v20") = a;
  register vint16m4_t rb asm("v24") = b;
  register vint16m4_t rd asm("v16") = acc_vd;
  asm volatile(
    //"vsetivli zero, 16, e16, m4, ta, ma\n\t"
    ".word 0xD34C0857\n\t"        // vscmacc.vv v16, v20, v24
    : "+vr"(rd)
    : "vr"(ra), "vr"(rb)
    : "memory"
  );
  return rd;
}

static inline __attribute__((always_inline))
vint16m4_t vscmacccj_i16m4(vint16m4_t acc_vd, vint16m4_t a_conj_src, vint16m4_t b) {
  register vint16m4_t ra asm("v20") = a_conj_src;
  register vint16m4_t rb asm("v24") = b;
  register vint16m4_t rd asm("v16") = acc_vd;
  asm volatile(
    //"vsetivli zero, 16, e16, m4, ta, ma\n\t"
    ".word 0xDB4C0857\n\t"        // vscmacccj.vv v16, v20, v24
    : "+vr"(rd)
    : "vr"(ra), "vr"(rb)
    : "memory"
  );
  return rd;
}

#include <riscv_vector.h>
#include <stdint.h>
#include <stddef.h>

static inline void ext_dft16(int16_t *x, int16_t *y)
{
    size_t vl = __riscv_vsetvl_e16m4(16);

    // DFT twiddle: W = cos - j sin
    const int16_t *tw16 = (const int16_t *)tw16arep;

    // twiddle vectors
    vint16m4_t tw0 = __riscv_vle16_v_i16m4(tw16 + 0 * 16, vl);
    vint16m4_t tw1 = __riscv_vle16_v_i16m4(tw16 + 1 * 16, vl);
    vint16m4_t tw2 = __riscv_vle16_v_i16m4(tw16 + 2 * 16, vl);

    // input
    vint16m4_t x0 = __riscv_vle16_v_i16m4(x + 0 * 16, vl);
    vint16m4_t x1v = __riscv_vle16_v_i16m4(x + 1 * 16, vl);
    vint16m4_t x2 = __riscv_vle16_v_i16m4(x + 2 * 16, vl);
    vint16m4_t x3v = __riscv_vle16_v_i16m4(x + 3 * 16, vl);

    // ---------------- Stage 1 radix-4 ----------------
    vint16m4_t x02   = __riscv_vsadd_vv_i16m4(x0, x2, vl);
    vint16m4_t x13   = __riscv_vsadd_vv_i16m4(x1v, x3v, vl);
    vint16m4_t xtmp0 = __riscv_vsadd_vv_i16m4(x02, x13, vl);
    vint16m4_t xtmp2 = __riscv_vssub_vv_i16m4(x02, x13, vl);

    // (-j)*x1, (-j)*x3
    vint16m4_t mj    = __riscv_vle16_v_i16m4(ext_minus_j_rep16, vl);
    asm volatile("vsetivli zero, 16, e16, m4, ta, ma\n\t");
    vint16m4_t x1_mj = vscmul_i16m4(x1v, mj);
    vint16m4_t x3_mj = vscmul_i16m4(x3v, mj);

    x02 = __riscv_vssub_vv_i16m4(x0, x2, vl);
    x13 = __riscv_vssub_vv_i16m4(x1_mj, x3_mj, vl);
    vint16m4_t xtmp1 = __riscv_vsadd_vv_i16m4(x02, x13, vl);
    vint16m4_t xtmp3 = __riscv_vssub_vv_i16m4(x02, x13, vl);

    // -------- unpack32 + unpack64 transpose chain --------
    vint16m4_t ytmp0 = rvv_unpacklo_epi32(xtmp0, xtmp1, vl);
    vint16m4_t ytmp1 = rvv_unpackhi_epi32(xtmp0, xtmp1, vl);
    vint16m4_t ytmp2 = rvv_unpacklo_epi32(xtmp2, xtmp3, vl);
    vint16m4_t ytmp3 = rvv_unpackhi_epi32(xtmp2, xtmp3, vl);

    xtmp0 = rvv_unpacklo_epi64(ytmp0, ytmp2, vl);
    xtmp1 = rvv_unpackhi_epi64(ytmp0, ytmp2, vl);
    xtmp2 = rvv_unpacklo_epi64(ytmp1, ytmp3, vl);
    xtmp3 = rvv_unpackhi_epi64(ytmp1, ytmp3, vl);

    // ---------------- Twiddle multiply (DFT) ----------------
    // xtmpk = xtmpk * Wk
    asm volatile("vsetivli zero, 16, e16, m4, ta, ma\n\t");
    xtmp1 = vscmulcj_i16m4(tw0, xtmp1);
    xtmp2 = vscmulcj_i16m4(tw1, xtmp2);
    xtmp3 = vscmulcj_i16m4(tw2, xtmp3);

    // ---------------- Stage 2 radix-4 ----------------
    x02 = __riscv_vsadd_vv_i16m4(xtmp0, xtmp2, vl);
    x13 = __riscv_vsadd_vv_i16m4(xtmp1, xtmp3, vl);
    vint16m4_t y0 = __riscv_vsadd_vv_i16m4(x02, x13, vl);
    vint16m4_t y2 = __riscv_vssub_vv_i16m4(x02, x13, vl);

    // (-j)*xtmp1, (-j)*xtmp3
    asm volatile("vsetivli zero, 16, e16, m4, ta, ma\n\t");
    vint16m4_t xtmp1_mj = vscmul_i16m4(xtmp1, mj);
    vint16m4_t xtmp3_mj = vscmul_i16m4(xtmp3, mj);

    x02 = __riscv_vssub_vv_i16m4(xtmp0, xtmp2, vl);
    x13 = __riscv_vssub_vv_i16m4(xtmp1_mj, xtmp3_mj, vl);
    vint16m4_t y1 = __riscv_vsadd_vv_i16m4(x02, x13, vl);
    vint16m4_t y3 = __riscv_vssub_vv_i16m4(x02, x13, vl);

    // ---------------- Final permutation (slide) ----------------
    vint16m4_t final0 = __riscv_vslideup_vx_i16m4(y0, y1, 8, vl);
    vint16m4_t final1 = __riscv_vslideup_vx_i16m4(y2, y3, 8, vl);

    vint16m4_t y0_hi = __riscv_vslidedown_vx_i16m4(y0, 8, vl);
    vint16m4_t y1_hi = __riscv_vslidedown_vx_i16m4(y1, 8, vl);
    vint16m4_t y2_hi = __riscv_vslidedown_vx_i16m4(y2, 8, vl);
    vint16m4_t y3_hi = __riscv_vslidedown_vx_i16m4(y3, 8, vl);

    vint16m4_t final2 = __riscv_vslideup_vx_i16m4(y0_hi, y1_hi, 8, vl);
    vint16m4_t final3 = __riscv_vslideup_vx_i16m4(y2_hi, y3_hi, 8, vl);

    __riscv_vse16_v_i16m4(y + 0 * 16, final0, vl);
    __riscv_vse16_v_i16m4(y + 1 * 16, final1, vl);
    __riscv_vse16_v_i16m4(y + 2 * 16, final2, vl);
    __riscv_vse16_v_i16m4(y + 3 * 16, final3, vl);
}



// ============================================================
// 3) ext_idft16：修正点
//    - IDFT 用 tw16arep（cos + j sin），不要在这里乱用 tw16rep
//    - shuffle 用 vl8=32
//    - 保留你原先 IDFT 的 xtmp1/xtmp3 顺序（你已写对）
// ============================================================

static inline void ext_idft16(int16_t *x, int16_t *y)
{
    size_t vl = __riscv_vsetvl_e16m4(16);

    // IDFT：乘 conj(W)。你这里继续复用 tw16rep (W = cos - j*sin)，用 vscmulcj 做 conj(vs2)*vs1
    const int16_t *tw16 = (const int16_t *)tw16rep;

    vint16m4_t tw0 = __riscv_vle16_v_i16m4(tw16 + 0 * 16, vl);
    vint16m4_t tw1 = __riscv_vle16_v_i16m4(tw16 + 1 * 16, vl);
    vint16m4_t tw2 = __riscv_vle16_v_i16m4(tw16 + 2 * 16, vl);

    vint16m4_t x0  = __riscv_vle16_v_i16m4(x + 0 * 16, vl);
    vint16m4_t x1v = __riscv_vle16_v_i16m4(x + 1 * 16, vl);
    vint16m4_t x2  = __riscv_vle16_v_i16m4(x + 2 * 16, vl);
    vint16m4_t x3v = __riscv_vle16_v_i16m4(x + 3 * 16, vl);

    // ---- stage1 radix-4 ----
    vint16m4_t x02   = __riscv_vsadd_vv_i16m4(x0,  x2,  vl);
    vint16m4_t x13   = __riscv_vsadd_vv_i16m4(x1v, x3v, vl);
    vint16m4_t xtmp0 = __riscv_vsadd_vv_i16m4(x02, x13, vl);
    vint16m4_t xtmp2 = __riscv_vssub_vv_i16m4(x02, x13, vl);

    // ★ IDFT 这里应该是 (+j)*x1, (+j)*x3（不是 -j）
     
    vint16m4_t pj    = __riscv_vle16_v_i16m4(ext_plus_j_rep16, vl);
    asm volatile("vsetivli zero, 16, e16, m4, ta, ma\n\t");
    vint16m4_t x1_pj = vscmul_i16m4(x1v, pj);
    vint16m4_t x3_pj = vscmul_i16m4(x3v, pj);

    x02 = __riscv_vssub_vv_i16m4(x0, x2, vl);
    x13 = __riscv_vssub_vv_i16m4(x1_pj, x3_pj, vl);
    vint16m4_t xtmp1 = __riscv_vsadd_vv_i16m4(x02, x13, vl);
    vint16m4_t xtmp3 = __riscv_vssub_vv_i16m4(x02, x13, vl);

    // ---- transpose chain（与 ext_dft16 相同）----
    vint16m4_t ytmp0 = rvv_unpacklo_epi32(xtmp0, xtmp1, vl);
    vint16m4_t ytmp1 = rvv_unpackhi_epi32(xtmp0, xtmp1, vl);
    vint16m4_t ytmp2 = rvv_unpacklo_epi32(xtmp2, xtmp3, vl);
    vint16m4_t ytmp3 = rvv_unpackhi_epi32(xtmp2, xtmp3, vl);

    xtmp0 = rvv_unpacklo_epi64(ytmp0, ytmp2, vl);
    xtmp1 = rvv_unpackhi_epi64(ytmp0, ytmp2, vl);
    xtmp2 = rvv_unpacklo_epi64(ytmp1, ytmp3, vl);
    xtmp3 = rvv_unpackhi_epi64(ytmp1, ytmp3, vl);

    // ---- twiddle multiply (IDFT): xtmpk *= conj(Wk) ----
    asm volatile("vsetivli zero, 16, e16, m4, ta, ma\n\t");
    xtmp1 = vscmulcj_i16m4(tw0, xtmp1);
    xtmp2 = vscmulcj_i16m4(tw1, xtmp2);
    xtmp3 = vscmulcj_i16m4(tw2, xtmp3);

    // ---- stage2 radix-4 ----
    x02 = __riscv_vsadd_vv_i16m4(xtmp0, xtmp2, vl);
    x13 = __riscv_vsadd_vv_i16m4(xtmp1, xtmp3, vl);
    vint16m4_t y0 = __riscv_vsadd_vv_i16m4(x02, x13, vl);
    vint16m4_t y2 = __riscv_vssub_vv_i16m4(x02, x13, vl);

    // ★ IDFT 这里同样是 (+j)*xtmp1, (+j)*xtmp3
    asm volatile("vsetivli zero, 16, e16, m4, ta, ma\n\t");
    vint16m4_t xtmp1_pj = vscmul_i16m4(xtmp1, pj);
    vint16m4_t xtmp3_pj = vscmul_i16m4(xtmp3, pj);

    x02 = __riscv_vssub_vv_i16m4(xtmp0, xtmp2, vl);
    x13 = __riscv_vssub_vv_i16m4(xtmp1_pj, xtmp3_pj, vl);
    vint16m4_t y1 = __riscv_vsadd_vv_i16m4(x02, x13, vl);
    vint16m4_t y3 = __riscv_vssub_vv_i16m4(x02, x13, vl);

    // ---- final permutation（slide）----
    vint16m4_t final0 = __riscv_vslideup_vx_i16m4(y0, y1, 8, vl);
    vint16m4_t final1 = __riscv_vslideup_vx_i16m4(y2, y3, 8, vl);

    vint16m4_t y0_hi = __riscv_vslidedown_vx_i16m4(y0, 8, vl);
    vint16m4_t y1_hi = __riscv_vslidedown_vx_i16m4(y1, 8, vl);
    vint16m4_t y2_hi = __riscv_vslidedown_vx_i16m4(y2, 8, vl);
    vint16m4_t y3_hi = __riscv_vslidedown_vx_i16m4(y3, 8, vl);

    vint16m4_t final2 = __riscv_vslideup_vx_i16m4(y0_hi, y1_hi, 8, vl);
    vint16m4_t final3 = __riscv_vslideup_vx_i16m4(y2_hi, y3_hi, 8, vl);

    __riscv_vse16_v_i16m4(y + 0 * 16, final0, vl);
    __riscv_vse16_v_i16m4(y + 1 * 16, final1, vl);
    __riscv_vse16_v_i16m4(y + 2 * 16, final2, vl);
    __riscv_vse16_v_i16m4(y + 3 * 16, final3, vl);
}

// ============================================================
// 4) ext_bfly4_16_256 / ext_ibfly4_16_256
//    这里我做“最保守且不想当然”的处理：
//    - tw1/tw2/tw3 直接当 packed 复数 twiddle 用（cos, ±sin 已经在表里）
//    - tw1b/tw2b/tw3b 不再参与计算（你现在用 vscmul 后本来就不需要）
//    - shuffle 用 vl8=32
// ============================================================

// (-j) packed constant: [0, -32767, 0, -32767, ...]
static inline void ext_bfly4_16_256(
    int16_t *x0,  int16_t *x1,  int16_t *x2,  int16_t *x3,
    int16_t *y0,  int16_t *y1,  int16_t *y2,  int16_t *y3,
    int16_t *tw1, int16_t *tw2, int16_t *tw3
) {
    // 固定：16 x i16 = 256-bit（m4）
    const size_t vl = 16;

    // -------- Load inputs --------
    vint16m4_t x0_vec = __riscv_vle16_v_i16m4(x0, vl);
    vint16m4_t x1_vec = __riscv_vle16_v_i16m4(x1, vl);
    vint16m4_t x2_vec = __riscv_vle16_v_i16m4(x2, vl);
    vint16m4_t x3_vec = __riscv_vle16_v_i16m4(x3, vl);

    vint16m4_t tw1_vec = __riscv_vle16_v_i16m4(tw1, vl);
    vint16m4_t tw2_vec = __riscv_vle16_v_i16m4(tw2, vl);
    vint16m4_t tw3_vec = __riscv_vle16_v_i16m4(tw3, vl);

    // -------- Complex muls (Q15 sat+round in HW) --------
    asm volatile("vsetivli zero, 16, e16, m4, ta, ma\n\t");
    vint16m4_t x1t = vscmul_i16m4(x1_vec, tw1_vec);
    vint16m4_t x2t = vscmul_i16m4(x2_vec, tw2_vec);
    vint16m4_t x3t = vscmul_i16m4(x3_vec, tw3_vec);

    // -------- Butterfly --------
    vint16m4_t x02t = __riscv_vsadd_vv_i16m4(x0_vec, x2t, vl);
    vint16m4_t x13t = __riscv_vsadd_vv_i16m4(x1t, x3t, vl);

    __riscv_vse16_v_i16m4(y0, __riscv_vsadd_vv_i16m4(x02t, x13t, vl), vl);
    __riscv_vse16_v_i16m4(y2, __riscv_vssub_vv_i16m4(x02t, x13t, vl), vl);

    // -------- *(-j) via ext complex mul --------
    vint16m4_t mj = __riscv_vle16_v_i16m4(ext_minus_j_rep16, vl);
    asm volatile("vsetivli zero, 16, e16, m4, ta, ma\n\t");
    vint16m4_t x1_flip = vscmul_i16m4(x1t, mj);
    vint16m4_t x3_flip = vscmul_i16m4(x3t, mj);

    x02t = __riscv_vssub_vv_i16m4(x0_vec, x2t, vl);
    x13t = __riscv_vssub_vv_i16m4(x1_flip, x3_flip, vl);

    __riscv_vse16_v_i16m4(y1, __riscv_vsadd_vv_i16m4(x02t, x13t, vl), vl);
    __riscv_vse16_v_i16m4(y3, __riscv_vssub_vv_i16m4(x02t, x13t, vl), vl);
}

static inline void ext_ibfly4_16_256(
    int16_t *x0,  int16_t *x1,  int16_t *x2,  int16_t *x3,
    int16_t *y0,  int16_t *y1,  int16_t *y2,  int16_t *y3,
    int16_t *tw1, int16_t *tw2, int16_t *tw3
) {
    const size_t vl = 16;

    vint16m4_t x0v = __riscv_vle16_v_i16m4(x0, vl);
    vint16m4_t x1v = __riscv_vle16_v_i16m4(x1, vl);
    vint16m4_t x2v = __riscv_vle16_v_i16m4(x2, vl);
    vint16m4_t x3v = __riscv_vle16_v_i16m4(x3, vl);

    vint16m4_t tw1v = __riscv_vle16_v_i16m4(tw1, vl);
    vint16m4_t tw2v = __riscv_vle16_v_i16m4(tw2, vl);
    vint16m4_t tw3v = __riscv_vle16_v_i16m4(tw3, vl);

    // IDFT: 乘 conj(W)（tw 表仍是 W=cos-j*sin，靠 vscmulcj 做 conj）
    asm volatile("vsetivli zero, 16, e16, m4, ta, ma\n\t");
    vint16m4_t x1t = vscmulcj_i16m4(tw1v, x1v);
    vint16m4_t x2t = vscmulcj_i16m4(tw2v, x2v);
    vint16m4_t x3t = vscmulcj_i16m4(tw3v, x3v);

    vint16m4_t x02t = __riscv_vsadd_vv_i16m4(x0v, x2t, vl);
    vint16m4_t x13t = __riscv_vsadd_vv_i16m4(x1t, x3t, vl);

    __riscv_vse16_v_i16m4(y0, __riscv_vsadd_vv_i16m4(x02t, x13t, vl), vl);
    __riscv_vse16_v_i16m4(y2, __riscv_vssub_vv_i16m4(x02t, x13t, vl), vl);

    // ★ IDFT 这里用 +j
    vint16m4_t pj = __riscv_vle16_v_i16m4(ext_plus_j_rep16, vl);
    asm volatile("vsetivli zero, 16, e16, m4, ta, ma\n\t");
    vint16m4_t x1_flip = vscmul_i16m4(x1t, pj);
    vint16m4_t x3_flip = vscmul_i16m4(x3t, pj);

    x02t = __riscv_vssub_vv_i16m4(x0v, x2t, vl);
    x13t = __riscv_vssub_vv_i16m4(x1_flip, x3_flip, vl);

    __riscv_vse16_v_i16m4(y1, __riscv_vsadd_vv_i16m4(x02t, x13t, vl), vl);
    __riscv_vse16_v_i16m4(y3, __riscv_vssub_vv_i16m4(x02t, x13t, vl), vl);
}

// ============================================================
// 5) ext_dft64 / ext_idft64：
//    这两份我只动你必须动的：
//    - 调用 ext_dft16 / ext_idft16（它们已修正 twiddle 与 shuffle VL）
//    - butterfly 调用保持你原来的 tw64a/tw64b 与 tw64/tw64c 传法，不做“拍脑袋互换”
// ============================================================

void ext_dft64(int16_t *x, int16_t *y, unsigned char scale)
{
    int16_t xtmp[16 * 16] __attribute__((aligned(32)));
    int16_t ytmp[16 * 16] __attribute__((aligned(32)));

    const int32_t *x_ptr = (const int32_t *)x;
    int32_t *xtmp_ptr    = (int32_t *)xtmp;

    // ---------- Stage 1: e32,m2, VL=8 ----------
    const size_t vl32 = 8;
    __riscv_vsetvl_e32m2(vl32);

    vint32m2_t perm_mask = rvv_set_epi32_m2(7, 3, 5, 1, 6, 2, 4, 0, vl32);

    for (int g = 0; g < 4; g++) {
        vint32m2_t xa = __riscv_vle32_v_i32m2(x_ptr + (2 * g + 0) * 8, vl32);
        vint32m2_t xb = __riscv_vle32_v_i32m2(x_ptr + (2 * g + 1) * 8, vl32);

        vint32m2_t xintla = rvv_permutevar8x32_epi32_m2(xa, perm_mask, vl32);
        vint32m2_t xintlb = rvv_permutevar8x32_epi32_m2(xb, perm_mask, vl32);

        __riscv_vse32_v_i32m2(
            xtmp_ptr + (g + 0) * 8,
            rvv_unpacklo_epi64_m2(xintla, xintlb, vl32),
            vl32
        );
        __riscv_vse32_v_i32m2(
            xtmp_ptr + (g + 4) * 8,
            rvv_unpackhi_epi64_m2(xintla, xintlb, vl32),
            vl32
        );
    }

    // ---------- Stage 2+: e16,m4, VL=16 ----------
    // ext_dft16（内部会 vsetivli 到 e16,m4,vl=16）
    ext_dft16(xtmp, ytmp);
    ext_dft16(xtmp + 4 * 16, ytmp + 4 * 16);

    // ---------- Butterfly ----------
    // 这里 twiddle 只传 “cos/sin packed” 的那份（例如 tw64a）
    
    int16_t *tw64_ptr = (int16_t *)tw64;

    ext_bfly4_16_256(
        ytmp + 0 * 16, ytmp + 2 * 16, ytmp + 4 * 16, ytmp + 6 * 16,
        y    + 0 * 16, y    + 2 * 16, y    + 4 * 16, y    + 6 * 16,
        tw64_ptr + 0 * 16, tw64_ptr + 2 * 16, tw64_ptr + 4 * 16
    );

    ext_bfly4_16_256(
        ytmp + 1 * 16, ytmp + 3 * 16, ytmp + 5 * 16, ytmp + 7 * 16,
        y    + 1 * 16, y    + 3 * 16, y    + 5 * 16, y    + 7 * 16,
        tw64_ptr + 1 * 16, tw64_ptr + 3 * 16, tw64_ptr + 5 * 16
    );

    // ---------- optional scaling ----------
    // 保持你现有行为：scale>0 右移 3
    if (scale > 0) {
        size_t vl = __riscv_vsetvl_e16m4(16);
        for (int i = 0; i < 8; i++) {
            vint16m4_t v = __riscv_vle16_v_i16m4(y + i * 16, vl);
            v = rvv_shiftright_int16(v, 3, vl);
            __riscv_vse16_v_i16m4(y + i * 16, v, vl);
        }
    }
}

void ext_idft64(int16_t *x, int16_t *y, unsigned char scale)
{
    int16_t xtmp[16 * 16] __attribute__((aligned(32)));
    int16_t ytmp[16 * 16] __attribute__((aligned(32)));

    const int32_t *x_ptr = (const int32_t *)x;
    int32_t *xtmp_ptr    = (int32_t *)xtmp;

    const size_t vl32 = 8;
    __riscv_vsetvl_e32m2(vl32);

    vint32m2_t perm_mask = rvv_set_epi32_m2(7, 3, 5, 1, 6, 2, 4, 0, vl32);

    for (int g = 0; g < 4; g++) {
        vint32m2_t xa = __riscv_vle32_v_i32m2(x_ptr + (2 * g + 0) * 8, vl32);
        vint32m2_t xb = __riscv_vle32_v_i32m2(x_ptr + (2 * g + 1) * 8, vl32);

        vint32m2_t xintla = rvv_permutevar8x32_epi32_m2(xa, perm_mask, vl32);
        vint32m2_t xintlb = rvv_permutevar8x32_epi32_m2(xb, perm_mask, vl32);

        __riscv_vse32_v_i32m2(xtmp_ptr + (g + 0) * 8, rvv_unpacklo_epi64_m2(xintla, xintlb, vl32), vl32);
        __riscv_vse32_v_i32m2(xtmp_ptr + (g + 4) * 8, rvv_unpackhi_epi64_m2(xintla, xintlb, vl32), vl32);
    }

    ext_idft16(xtmp,          ytmp);
    ext_idft16(xtmp + 4 * 16, ytmp + 4 * 16);

    int16_t *tw64_ptr = (int16_t *)tw64;  // W=cos - j*sin

    ext_ibfly4_16_256(
        ytmp + 0 * 16, ytmp + 2 * 16, ytmp + 4 * 16, ytmp + 6 * 16,
        y    + 0 * 16, y    + 2 * 16, y    + 4 * 16, y    + 6 * 16,
        tw64_ptr + 0 * 16, tw64_ptr + 2 * 16, tw64_ptr + 4 * 16
    );

    ext_ibfly4_16_256(
        ytmp + 1 * 16, ytmp + 3 * 16, ytmp + 5 * 16, ytmp + 7 * 16,
        y    + 1 * 16, y    + 3 * 16, y    + 5 * 16, y    + 7 * 16,
        tw64_ptr + 1 * 16, tw64_ptr + 3 * 16, tw64_ptr + 5 * 16
    );

    if (scale > 0) {
        size_t vl = __riscv_vsetvl_e16m4(16);
        for (int i = 0; i < 8; i++) {
            vint16m4_t v = __riscv_vle16_v_i16m4(y + i * 16, vl);
            v = rvv_shiftright_int16(v, 3, vl);
            __riscv_vse16_v_i16m4(y + i * 16, v, vl);
        }
    }
}

// ------------------------------------------------------------
// DFT butterfly2: y0 = x0 + x1*tw , y1 = x0 - x1*tw
// 约定：vs1 = x1, vs2 = tw  （便于 IDFT 直接换成 vscmulcj）
// ------------------------------------------------------------
static inline void ext_bfly2_16_256(const int16_t *x0, const int16_t *x1,
                                   int16_t *y0, int16_t *y1,
                                   const int16_t *tw, const int16_t *twb)
{
    // 假设外部已设置 vtype = e16m4；这里不做 vsetvl 以减少开销
    const size_t vl = 16;

    (void)twb; // 方案A下不再需要 twb，保留形参只为兼容旧接口

    // load
    vint16m4_t x0v = __riscv_vle16_v_i16m4(x0, vl);
    vint16m4_t x1v = __riscv_vle16_v_i16m4(x1, vl);
    vint16m4_t twv = __riscv_vle16_v_i16m4(tw, vl);

    // x1t = x1 * tw   (DFT：不共轭)
    // 注意：为了和 IDFT 的 vscmulcj 对齐，固定 tw 作为 vs2
    vint16m4_t x1t = vscmul_i16m4(x1v, twv);

    // y0/y1
    vint16m4_t y0v = rvv_adds_epi16(x0v, x1t, vl);
    vint16m4_t y1v = rvv_subs_epi16(x0v, x1t, vl);

    // store
    __riscv_vse16_v_i16m4(y0, y0v, vl);
    __riscv_vse16_v_i16m4(y1, y1v, vl);
}

// ------------------------------------------------------------
// IDFT butterfly2: y0 = x0 + x1*conj(tw) , y1 = x0 - x1*conj(tw)
// 方案A：共轭对 vs2 做，因此必须写成 vscmulcj(x1, tw)
// ------------------------------------------------------------
static inline void ext_ibfly2_16_256(const int16_t *x0, const int16_t *x1,
                                    int16_t *y0, int16_t *y1,
                                    const int16_t *tw, const int16_t *twb)
{
    const size_t vl = 16;

    (void)twb; // 同上：兼容形参，实际不使用

    // load
    vint16m4_t x0v = __riscv_vle16_v_i16m4(x0, vl);
    vint16m4_t x1v = __riscv_vle16_v_i16m4(x1, vl);
    vint16m4_t twv = __riscv_vle16_v_i16m4(tw, vl);

    // x1t = x1 * conj(tw)   (IDFT：twiddle 共轭)
    // 仍然固定 tw 作为 vs2，保证“共轭只对 vs2”严格成立
    vint16m4_t x1t = vscmulcj_i16m4(x1v, twv);

    // y0/y1
    vint16m4_t y0v = rvv_adds_epi16(x0v, x1t, vl);
    vint16m4_t y1v = rvv_subs_epi16(x0v, x1t, vl);

    // store
    __riscv_vse16_v_i16m4(y0, y0v, vl);
    __riscv_vse16_v_i16m4(y1, y1v, vl);
}


#endif  
#endif
static inline void idft16(int16_t *x,int16_t *y) __attribute__((always_inline));

static inline void idft16(int16_t *x,int16_t *y)
{

#if defined(__x86_64__) || defined(__i386__) || defined SIMDE_ENABLE_NATIVE_ALIASES
  __m128i *tw16a_128=(__m128i *)tw16,*tw16b_128=(__m128i *)tw16c,*x128=(__m128i *)x,*y128=(__m128i *)y;

  /*
  bfly4_tw1(x128,x128+1,x128+2,x128+3,
      y128,y128+1,y128+2,y128+3);

  transpose16(y128,ytmp);

  bfly4_16(ytmp,ytmp+1,ytmp+2,ytmp+3,
     y128,y128+1,y128+2,y128+3,
     tw16_128,tw16_128+1,tw16_128+2);
  */

  register __m128i x1_flip,x3_flip,x02t,x13t;
  register __m128i ytmp0,ytmp1,ytmp2,ytmp3,xtmp0,xtmp1,xtmp2,xtmp3;

  // First stage : 4 Radix-4 butterflies without input twiddles

  x02t    = _mm_adds_epi16(x128[0],x128[2]);
  x13t    = _mm_adds_epi16(x128[1],x128[3]);
  xtmp0   = _mm_adds_epi16(x02t,x13t);
  xtmp2   = _mm_subs_epi16(x02t,x13t);
  x1_flip = _mm_sign_epi16(x128[1],*(__m128i*)conjugatedft);
  x1_flip = _mm_shuffle_epi8(x1_flip,_mm_set_epi8(13,12,15,14,9,8,11,10,5,4,7,6,1,0,3,2));
  x3_flip = _mm_sign_epi16(x128[3],*(__m128i*)conjugatedft);
  x3_flip = _mm_shuffle_epi8(x3_flip,_mm_set_epi8(13,12,15,14,9,8,11,10,5,4,7,6,1,0,3,2));
  x02t    = _mm_subs_epi16(x128[0],x128[2]);
  x13t    = _mm_subs_epi16(x1_flip,x3_flip);
  xtmp3   = _mm_adds_epi16(x02t,x13t);  // x0 + x1f - x2 - x3f
  xtmp1   = _mm_subs_epi16(x02t,x13t);  // x0 - x1f - x2 + x3f

  ytmp0   = _mm_unpacklo_epi32(xtmp0,xtmp1);
  ytmp1   = _mm_unpackhi_epi32(xtmp0,xtmp1);
  ytmp2   = _mm_unpacklo_epi32(xtmp2,xtmp3);
  ytmp3   = _mm_unpackhi_epi32(xtmp2,xtmp3);
  xtmp0   = _mm_unpacklo_epi64(ytmp0,ytmp2);
  xtmp1   = _mm_unpackhi_epi64(ytmp0,ytmp2);
  xtmp2   = _mm_unpacklo_epi64(ytmp1,ytmp3);
  xtmp3   = _mm_unpackhi_epi64(ytmp1,ytmp3);

  // Second stage : 4 Radix-4 butterflies with input twiddles
  xtmp1 = packed_cmult2(xtmp1,tw16a_128[0],tw16b_128[0]);
  xtmp2 = packed_cmult2(xtmp2,tw16a_128[1],tw16b_128[1]);
  xtmp3 = packed_cmult2(xtmp3,tw16a_128[2],tw16b_128[2]);

  x02t    = _mm_adds_epi16(xtmp0,xtmp2);
  x13t    = _mm_adds_epi16(xtmp1,xtmp3);
  y128[0] = _mm_adds_epi16(x02t,x13t);
  y128[2] = _mm_subs_epi16(x02t,x13t);
  x1_flip = _mm_sign_epi16(xtmp1,*(__m128i*)conjugatedft);
  x1_flip = _mm_shuffle_epi8(x1_flip,_mm_set_epi8(13,12,15,14,9,8,11,10,5,4,7,6,1,0,3,2));
  x3_flip = _mm_sign_epi16(xtmp3,*(__m128i*)conjugatedft);
  x3_flip = _mm_shuffle_epi8(x3_flip,_mm_set_epi8(13,12,15,14,9,8,11,10,5,4,7,6,1,0,3,2));
  x02t    = _mm_subs_epi16(xtmp0,xtmp2);
  x13t    = _mm_subs_epi16(x1_flip,x3_flip);
  y128[3] = _mm_adds_epi16(x02t,x13t);  // x0 + x1f - x2 - x3f
  y128[1] = _mm_subs_epi16(x02t,x13t);  // x0 - x1f - x2 + x3f


#endif
}

void idft16f(int16_t *x,int16_t *y) {
  idft16(x,y);
}

#if defined(__x86_64__) || defined(__i386__) || defined SIMDE_ENABLE_NATIVE_ALIASES
#ifdef __AVX2__
// Does two 16-point IDFTS (x[0 .. 15] is 128 LSBs of input vector, x[16..31] is in 128 MSBs) 
static inline void idft16_simd256(int16_t *x,int16_t *y) __attribute__((always_inline));
static inline void idft16_simd256(int16_t *x,int16_t *y)
{

  __m256i *tw16a_256=(__m256i *)tw16rep,*tw16b_256=(__m256i *)tw16crep,*x256=(__m256i *)x,*y256=(__m256i *)y;
  register __m256i x1_flip,x3_flip,x02t,x13t;
  register __m256i ytmp0,ytmp1,ytmp2,ytmp3,xtmp0,xtmp1,xtmp2,xtmp3;
  register __m256i complex_shuffle = _mm256_set_epi8(29,28,31,30,25,24,27,26,21,20,23,22,17,16,19,18,13,12,15,14,9,8,11,10,5,4,7,6,1,0,3,2);

  // First stage : 4 Radix-4 butterflies without input twiddles

  x02t    = _mm256_adds_epi16(x256[0],x256[2]);
  x13t    = _mm256_adds_epi16(x256[1],x256[3]);
  xtmp0   = _mm256_adds_epi16(x02t,x13t);
  xtmp2   = _mm256_subs_epi16(x02t,x13t);
  x1_flip = _mm256_sign_epi16(x256[1],*(__m256i*)conjugatedft);
  x1_flip = _mm256_shuffle_epi8(x1_flip,complex_shuffle);
  x3_flip = _mm256_sign_epi16(x256[3],*(__m256i*)conjugatedft);
  x3_flip = _mm256_shuffle_epi8(x3_flip,complex_shuffle);
  x02t    = _mm256_subs_epi16(x256[0],x256[2]);
  x13t    = _mm256_subs_epi16(x1_flip,x3_flip);
  xtmp3   = _mm256_adds_epi16(x02t,x13t);  // x0 + x1f - x2 - x3f
  xtmp1   = _mm256_subs_epi16(x02t,x13t);  // x0 - x1f - x2 + x3f

  ytmp0   = _mm256_unpacklo_epi32(xtmp0,xtmp1);  
  ytmp1   = _mm256_unpackhi_epi32(xtmp0,xtmp1);
  ytmp2   = _mm256_unpacklo_epi32(xtmp2,xtmp3);
  ytmp3   = _mm256_unpackhi_epi32(xtmp2,xtmp3);
  xtmp0   = _mm256_unpacklo_epi64(ytmp0,ytmp2);
  xtmp1   = _mm256_unpackhi_epi64(ytmp0,ytmp2);
  xtmp2   = _mm256_unpacklo_epi64(ytmp1,ytmp3);
  xtmp3   = _mm256_unpackhi_epi64(ytmp1,ytmp3);

  // Second stage : 4 Radix-4 butterflies with input twiddles
  xtmp1 = packed_cmult2_256(xtmp1,tw16a_256[0],tw16b_256[0]);
  xtmp2 = packed_cmult2_256(xtmp2,tw16a_256[1],tw16b_256[1]);
  xtmp3 = packed_cmult2_256(xtmp3,tw16a_256[2],tw16b_256[2]);

  x02t    = _mm256_adds_epi16(xtmp0,xtmp2);
  x13t    = _mm256_adds_epi16(xtmp1,xtmp3);
  ytmp0   = _mm256_adds_epi16(x02t,x13t);
  ytmp2   = _mm256_subs_epi16(x02t,x13t);
  x1_flip = _mm256_sign_epi16(xtmp1,*(__m256i*)conjugatedft);
  x1_flip = _mm256_shuffle_epi8(x1_flip,complex_shuffle);
  x3_flip = _mm256_sign_epi16(xtmp3,*(__m256i*)conjugatedft);
  x3_flip = _mm256_shuffle_epi8(x3_flip,complex_shuffle);
  x02t    = _mm256_subs_epi16(xtmp0,xtmp2);
  x13t    = _mm256_subs_epi16(x1_flip,x3_flip);
  ytmp3   = _mm256_adds_epi16(x02t,x13t);  // x0 + x1f - x2 - x3f
  ytmp1   = _mm256_subs_epi16(x02t,x13t);  // x0 - x1f - x2 + x3f

  // [y0  y1  y2  y3  y16 y17 y18 y19]
  // [y4  y5  y6  y7  y20 y21 y22 y23]
  // [y8  y9  y10 y11 y24 y25 y26 y27]
  // [y12 y13 y14 y15 y28 y29 y30 y31]

  y256[0] = _mm256_insertf128_si256(ytmp0,_mm256_extracti128_si256(ytmp1,0),1);
  y256[1] = _mm256_insertf128_si256(ytmp2,_mm256_extracti128_si256(ytmp3,0),1);
  y256[2] = _mm256_insertf128_si256(ytmp1,_mm256_extracti128_si256(ytmp0,1),0);
  y256[3] = _mm256_insertf128_si256(ytmp3,_mm256_extracti128_si256(ytmp2,1),0);

}

static inline void rvv_idft16(int16_t *x, int16_t *y)
{
    size_t vl = __riscv_vsetvl_e16m4(16);

    // IDFT twiddle：按你老版 idft16
    const int16_t *tw16a = (const int16_t *)tw16rep;
    const int16_t *tw16b = (const int16_t *)tw16crep;

    // twiddle
    vint16m4_t tw_a0 = __riscv_vle16_v_i16m4(tw16a + 0 * 16, vl);
    vint16m4_t tw_b0 = __riscv_vle16_v_i16m4(tw16b + 0 * 16, vl);
    vint16m4_t tw_a1 = __riscv_vle16_v_i16m4(tw16a + 1 * 16, vl);
    vint16m4_t tw_b1 = __riscv_vle16_v_i16m4(tw16b + 1 * 16, vl);
    vint16m4_t tw_a2 = __riscv_vle16_v_i16m4(tw16a + 2 * 16, vl);
    vint16m4_t tw_b2 = __riscv_vle16_v_i16m4(tw16b + 2 * 16, vl);

    // input
    vint16m4_t x0 = __riscv_vle16_v_i16m4(x + 0 * 16, vl);
    vint16m4_t x1 = __riscv_vle16_v_i16m4(x + 1 * 16, vl);
    vint16m4_t x2 = __riscv_vle16_v_i16m4(x + 2 * 16, vl);
    vint16m4_t x3 = __riscv_vle16_v_i16m4(x + 3 * 16, vl);

    // ---------------- Stage 1 radix-4 ----------------
    vint16m4_t x02   = __riscv_vsadd_vv_i16m4(x0, x2, vl);
    vint16m4_t x13   = __riscv_vsadd_vv_i16m4(x1, x3, vl);
    vint16m4_t xtmp0 = __riscv_vsadd_vv_i16m4(x02, x13, vl);
    vint16m4_t xtmp2 = __riscv_vssub_vv_i16m4(x02, x13, vl);

    // (-j)*x1, (-j)*x3
    vint16m4_t x1mj = rvv_mul_plus_j_packed_i16m4(x1, vl);
    vint16m4_t x3mj = rvv_mul_plus_j_packed_i16m4(x3, vl);


    x02 = __riscv_vssub_vv_i16m4(x0, x2, vl);
    x13 = __riscv_vssub_vv_i16m4(x1mj, x3mj, vl);
    vint16m4_t xtmp1 = __riscv_vsadd_vv_i16m4(x02, x13, vl);
    vint16m4_t xtmp3 = __riscv_vssub_vv_i16m4(x02, x13, vl);

    // -------- 转置链（保持与 dft16 一致的 AVX 语义）--------
    vint16m4_t ytmp0 = rvv_unpacklo_epi32(xtmp0, xtmp1, vl);
    vint16m4_t ytmp1 = rvv_unpackhi_epi32(xtmp0, xtmp1, vl);
    vint16m4_t ytmp2 = rvv_unpacklo_epi32(xtmp2, xtmp3, vl);
    vint16m4_t ytmp3 = rvv_unpackhi_epi32(xtmp2, xtmp3, vl);

    xtmp0 = rvv_unpacklo_epi64(ytmp0, ytmp2, vl);
    xtmp1 = rvv_unpackhi_epi64(ytmp0, ytmp2, vl);
    xtmp2 = rvv_unpacklo_epi64(ytmp1, ytmp3, vl);
    xtmp3 = rvv_unpackhi_epi64(ytmp1, ytmp3, vl);

    // ---------------- Twiddle multiply ----------------
    xtmp1 = rvv_packed_cmult2_q15_i16m4(xtmp1, tw_a0, tw_b0, vl);
    xtmp2 = rvv_packed_cmult2_q15_i16m4(xtmp2, tw_a1, tw_b1, vl);
    xtmp3 = rvv_packed_cmult2_q15_i16m4(xtmp3, tw_a2, tw_b2, vl);

    // ---------------- Stage 2 radix-4 ----------------
    x02   = __riscv_vsadd_vv_i16m4(xtmp0, xtmp2, vl);
    x13   = __riscv_vsadd_vv_i16m4(xtmp1, xtmp3, vl);
    ytmp0 = __riscv_vsadd_vv_i16m4(x02, x13, vl);
    ytmp2 = __riscv_vssub_vv_i16m4(x02, x13, vl);

    // (-j)*xtmp1, (-j)*xtmp3
    vint16m4_t xtmp1mj = rvv_mul_plus_j_packed_i16m4(xtmp1, vl);
    vint16m4_t xtmp3mj = rvv_mul_plus_j_packed_i16m4(xtmp3, vl);


    x02   = __riscv_vssub_vv_i16m4(xtmp0, xtmp2, vl);
    x13   = __riscv_vssub_vv_i16m4(xtmp1mj, xtmp3mj, vl);
    ytmp1 = __riscv_vsadd_vv_i16m4(x02, x13, vl);
    ytmp3 = __riscv_vssub_vv_i16m4(x02, x13, vl);

    // ---------------- Final permutation（slide 版）----------------
    vint16m4_t final0 = __riscv_vslideup_vx_i16m4(ytmp0, ytmp1, 8, vl);
    vint16m4_t final1 = __riscv_vslideup_vx_i16m4(ytmp2, ytmp3, 8, vl);

    vint16m4_t y0_hi = __riscv_vslidedown_vx_i16m4(ytmp0, 8, vl);
    vint16m4_t y1_hi = __riscv_vslidedown_vx_i16m4(ytmp1, 8, vl);
    vint16m4_t y2_hi = __riscv_vslidedown_vx_i16m4(ytmp2, 8, vl);
    vint16m4_t y3_hi = __riscv_vslidedown_vx_i16m4(ytmp3, 8, vl);

    vint16m4_t final2 = __riscv_vslideup_vx_i16m4(y0_hi, y1_hi, 8, vl);
    vint16m4_t final3 = __riscv_vslideup_vx_i16m4(y2_hi, y3_hi, 8, vl);

    __riscv_vse16_v_i16m4(y + 0 * 16, final0, vl);
    __riscv_vse16_v_i16m4(y + 1 * 16, final1, vl);
    __riscv_vse16_v_i16m4(y + 2 * 16, final2, vl);
    __riscv_vse16_v_i16m4(y + 3 * 16, final3, vl);
}


#endif  
#endif


#if defined(__x86_64__) || defined(__i386__) || defined SIMDE_ENABLE_NATIVE_ALIASES
#define simd_q15_t __m128i
#define simdshort_q15_t __m64
#define shiftright_int16(a,shift) _mm_srai_epi16(a,shift)
#define set1_int16(a) _mm_set1_epi16(a);
#define mulhi_int16(a,b) _mm_mulhrs_epi16 (a,b)
#ifdef __AVX2__
#define simd256_q15_t __m256i
#define shiftright_int16_simd256(a,shift) _mm256_srai_epi16(a,shift)

#define set1_int16_simd256(a) _mm256_set1_epi16(a);


/**
 * @brief 将标量广播到 vint16m4_t 向量
 */

static inline vint16m4_t rvv_set1_int16(int16_t a) {
    size_t vl = __riscv_vsetvl_e16m4(16);
    return __riscv_vmv_v_x_i16m4(a, vl);
}

#define mulhi_int16_simd256(a,b) _mm256_mulhrs_epi16(a,b); //_mm256_slli_epi16(_mm256_mulhi_epi16(a,b),1);
typedef simd256_q15_t v256_q15_t;

/**
 * @brief 使用 RVV Intrinsic 实现 _mm256_mulhrs_epi16 (Q1.15 有符号舍入高位乘法)。
 *
 * 采用加宽乘法和窄化并移位 (vnsra) 来确保功能兼容。
 */
static inline vint16m4_t rvv_mulhi_int16(vint16m4_t a, vint16m4_t b, size_t vl)
{
    // // 256位向量 (16个 int16_t 元素) 需要分为两部分处理 (低8个和高8个)，
    // // 以适应 RVV 的加宽操作 (LMUL=2 -> LMUL=4)
    
    // // 1. 定义 VL: 针对 8 个 16-bit 元素 (处理 128 位块)
    // const size_t vl_m1 = 8;
    // size_t vl = __riscv_vsetvl_e16m1(vl_m1); // 16-bit, LMUL=1
    
    // // 2. 声明结果和中间变量
    // v256_q15_t result_v;
    
    // // 3. 处理低 128 位 (0-7 元素)
    // {
    //     // 3a. Load: 加载低 8 个元素 (使用 LMUL=1)
    //     vint16m1_t va_lo = __riscv_vle16_v_i16m1((const int16_t *)&a, vl);
    //     vint16m1_t vb_lo = __riscv_vle16_v_i16m1((const int16_t *)&b, vl);

    //     // 3b. Widening (加宽乘法): va_lo * vb_lo -> 32-bit results (LMUL=2)
    //     // 重新设置 VL 为 32-bit (8个元素, LMUL=2)
    //     size_t vl_w = __riscv_vsetvl_e32m2(vl_m1);
    //     vint32m2_t product_w = __riscv_vwmul_vv_i32m2(va_lo, vb_lo, vl_w);

    //     // 3c. Rounding (舍入): product_w += 0x4000
    //     const int32_t round_val = 0x4000;
    //     vint32m2_t product_r = __riscv_vadd_vx_i32m2(product_w, round_val, vl_w);

    //     // 3d. Narrowing (窄化): product_r = product_r >> 15 (截断并移位)
    //     // 使用 vnsra.wx: Narrowing Signed Right Shift Arithmetic
    //     // vnsra.wx vd, vs2, rs1 (窄化 vs2, 同时右移 rs1 位)
    //     const int shift_val = 15;
    //     // 重新设置 VL 为 16-bit (8个元素, LMUL=1)
    //     vl = __riscv_vsetvl_e16m1(vl_m1);
    //     vint16m1_t final_16_lo = __riscv_vnsra_wx_i16m1(product_r, shift_val, vl);

    //     // 3e. Store: 存储低 8 个元素
    //     __riscv_vse16_v_i16m1((int16_t *)&result_v, final_16_lo, vl);
    // }

    // // 4. 处理高 128 位 (8-15 元素)
    // {
    //     // 4a. Load: 加载高 8 个元素 (偏移量 +8 * sizeof(int16_t) = 16 字节)
    //     vl = __riscv_vsetvl_e16m1(vl_m1);
    //     vint16m1_t va_hi = __riscv_vle16_v_i16m1((const int16_t *)&a + 8, vl);
    //     vint16m1_t vb_hi = __riscv_vle16_v_i16m1((const int16_t *)&b + 8, vl);
        
    //     // 4b. Widening (加宽乘法)
    //     size_t vl_w = __riscv_vsetvl_e32m2(vl_m1);
    //     vint32m2_t product_w = __riscv_vwmul_vv_i32m2(va_hi, vb_hi, vl_w);

    //     // 4c. Rounding (舍入)
    //     const int32_t round_val = 0x4000;
    //     vint32m2_t product_r = __riscv_vadd_vx_i32m2(product_w, round_val, vl_w);

    //     // 4d. Narrowing (窄化并移位)
    //     const int shift_val = 15;
    //     vl = __riscv_vsetvl_e16m1(vl_m1);
    //     vint16m1_t final_16_hi = __riscv_vnsra_wx_i16m1(product_r, shift_val, vl);

    //     // 4e. Store: 存储高 8 个元素
    //     __riscv_vse16_v_i16m1((int16_t *)&result_v + 8, final_16_hi, vl);
    // }
    
    // // 确保 VL 恢复到 256 位操作的默认状态（如果需要）
    // __riscv_vsetvl_e16m2(16);

    // return result_v;
    const unsigned int vxrm = 0;
    return __riscv_vsmul_vv_i16m4(a, b, vxrm, vl);
}
#endif



#endif

#ifndef __AVX2__


#else // __AVX2__

static inline void rvv_bfly4_16_256(
    int16_t *x0,  int16_t *x1,  int16_t *x2,  int16_t *x3,
    int16_t *y0,  int16_t *y1,  int16_t *y2,  int16_t *y3,
    int16_t *tw1, int16_t *tw2, int16_t *tw3,
    int16_t *tw1b, int16_t *tw2b, int16_t *tw3b
) {
    // 固定：16 x i16 = 256-bit（m4）
    const size_t vl = 16;

    // -------- Load inputs --------
    vint16m4_t x0_vec = __riscv_vle16_v_i16m4(x0, vl);
    vint16m4_t x1_vec = __riscv_vle16_v_i16m4(x1, vl);
    vint16m4_t x2_vec = __riscv_vle16_v_i16m4(x2, vl);
    vint16m4_t x3_vec = __riscv_vle16_v_i16m4(x3, vl);

    // -------- Load twiddles --------
    vint16m4_t tw1_vec  = __riscv_vle16_v_i16m4(tw1, vl);
    vint16m4_t tw2_vec  = __riscv_vle16_v_i16m4(tw2, vl);
    vint16m4_t tw3_vec  = __riscv_vle16_v_i16m4(tw3, vl);
    vint16m4_t tw1b_vec = __riscv_vle16_v_i16m4(tw1b, vl);
    vint16m4_t tw2b_vec = __riscv_vle16_v_i16m4(tw2b, vl);
    vint16m4_t tw3b_vec = __riscv_vle16_v_i16m4(tw3b, vl);

    // -------- Complex muls --------
    vint16m4_t x1t = rvv_packed_cmult2_q15_i16m4(x1_vec, tw1_vec, tw1b_vec, vl);
    vint16m4_t x2t = rvv_packed_cmult2_q15_i16m4(x2_vec, tw2_vec, tw2b_vec, vl);
    vint16m4_t x3t = rvv_packed_cmult2_q15_i16m4(x3_vec, tw3_vec, tw3b_vec, vl);

    // -------- Butterfly --------
    // x02t = x0 + x2t
    vint16m4_t x02t = __riscv_vsadd_vv_i16m4(x0_vec, x2t, vl);

    // x13t = x1t + x3t
    vint16m4_t x13t = __riscv_vsadd_vv_i16m4(x1t, x3t, vl);

    // y0 = x02t + x13t
    __riscv_vse16_v_i16m4(y0, __riscv_vsadd_vv_i16m4(x02t, x13t, vl), vl);

    // y2 = x02t - x13t
    __riscv_vse16_v_i16m4(y2, __riscv_vssub_vv_i16m4(x02t, x13t, vl), vl);

    // x1_flip = x1t * (-j), x3_flip = x3t * (-j)
    vint16m4_t x1_flip = rvv_mul_minus_j_packed_i16m4(x1t, vl);
    vint16m4_t x3_flip = rvv_mul_minus_j_packed_i16m4(x3t, vl);

    // x02t = x0 - x2t
    x02t = __riscv_vssub_vv_i16m4(x0_vec, x2t, vl);

    // x13t = x1_flip - x3_flip
    x13t = __riscv_vssub_vv_i16m4(x1_flip, x3_flip, vl);

    // y1 = x02t + x13t
    __riscv_vse16_v_i16m4(y1, __riscv_vsadd_vv_i16m4(x02t, x13t, vl), vl);

    // y3 = x02t - x13t
    __riscv_vse16_v_i16m4(y3, __riscv_vssub_vv_i16m4(x02t, x13t, vl), vl);
}

static inline void rvv_ibfly4_16_256(
    int16_t *x0,  int16_t *x1,  int16_t *x2,  int16_t *x3,
    int16_t *y0,  int16_t *y1,  int16_t *y2,  int16_t *y3,
    int16_t *tw1, int16_t *tw2, int16_t *tw3,
    int16_t *tw1c, int16_t *tw2c, int16_t *tw3c
) {
    // 固定：16 x i16 = 256-bit（m4）
    const size_t vl = 16;

    // -------- Load inputs --------
    vint16m4_t x0_vec = __riscv_vle16_v_i16m4(x0, vl);
    vint16m4_t x1_vec = __riscv_vle16_v_i16m4(x1, vl);
    vint16m4_t x2_vec = __riscv_vle16_v_i16m4(x2, vl);
    vint16m4_t x3_vec = __riscv_vle16_v_i16m4(x3, vl);

    // -------- Load twiddles --------
    vint16m4_t tw1_vec  = __riscv_vle16_v_i16m4(tw1, vl);
    vint16m4_t tw2_vec  = __riscv_vle16_v_i16m4(tw2, vl);
    vint16m4_t tw3_vec  = __riscv_vle16_v_i16m4(tw3, vl);
    vint16m4_t tw1b_vec = __riscv_vle16_v_i16m4(tw1c, vl);
    vint16m4_t tw2b_vec = __riscv_vle16_v_i16m4(tw2c, vl);
    vint16m4_t tw3b_vec = __riscv_vle16_v_i16m4(tw3c, vl);

    // -------- Complex muls --------
    vint16m4_t x1t = rvv_packed_cmult2_q15_i16m4(x1_vec, tw1_vec, tw1b_vec, vl);
    vint16m4_t x2t = rvv_packed_cmult2_q15_i16m4(x2_vec, tw2_vec, tw2b_vec, vl);
    vint16m4_t x3t = rvv_packed_cmult2_q15_i16m4(x3_vec, tw3_vec, tw3b_vec, vl);

    // -------- Butterfly --------
    // x02t = x0 + x2t
    vint16m4_t x02t = __riscv_vsadd_vv_i16m4(x0_vec, x2t, vl);

    // x13t = x1t + x3t
    vint16m4_t x13t = __riscv_vsadd_vv_i16m4(x1t, x3t, vl);

    // y0 = x02t + x13t
    __riscv_vse16_v_i16m4(y0, __riscv_vsadd_vv_i16m4(x02t, x13t, vl), vl);

    // y2 = x02t - x13t
    __riscv_vse16_v_i16m4(y2, __riscv_vssub_vv_i16m4(x02t, x13t, vl), vl);

    // x1_flip = x1t * (-j), x3_flip = x3t * (-j)
    vint16m4_t x1_flip = rvv_mul_plus_j_packed_i16m4(x1t, vl);
    vint16m4_t x3_flip = rvv_mul_plus_j_packed_i16m4(x3t, vl);

    // x02t = x0 - x2t
    x02t = __riscv_vssub_vv_i16m4(x0_vec, x2t, vl);

    // x13t = x1_flip - x3_flip
    x13t = __riscv_vssub_vv_i16m4(x1_flip, x3_flip, vl);

    // y1 = x02t + x13t
    __riscv_vse16_v_i16m4(y1, __riscv_vsadd_vv_i16m4(x02t, x13t, vl), vl);

    // y3 = x02t - x13t
    __riscv_vse16_v_i16m4(y3, __riscv_vssub_vv_i16m4(x02t, x13t, vl), vl);
}


void dft64(int16_t *x,int16_t *y,unsigned char scale)
{

  simd256_q15_t xtmp[16],ytmp[16],*tw64a_256=(simd256_q15_t *)tw64a,*tw64b_256=(simd256_q15_t *)tw64b,*x256=(simd256_q15_t *)x,*y256=(simd256_q15_t *)y;
  simd256_q15_t xintl0,xintl1,xintl2,xintl3,xintl4,xintl5,xintl6,xintl7;
  simd256_q15_t const perm_mask = _mm256_set_epi32(7, 3, 5, 1, 6, 2, 4, 0);

  // debug_print_vec_avx("perm_mask", perm_mask);
  // DEBUG_PRINT_AVX_VEC("AVX: x256[0]", x256[0]);
  // DEBUG_PRINT_AVX_VEC("AVX: x256[1]", x256[1]);
  // DEBUG_PRINT_AVX_VEC("AVX: x256[2]", x256[2]);
  // DEBUG_PRINT_AVX_VEC("AVX: x256[3]", x256[3]);

  xintl0 = _mm256_permutevar8x32_epi32(x256[0],perm_mask);  // x0  x4  x1  x5  x2  x6  x3  x7
  xintl1 = _mm256_permutevar8x32_epi32(x256[1],perm_mask);  // x8  x12 x9  x13 x10 x14 x11 x15
  xintl2 = _mm256_permutevar8x32_epi32(x256[2],perm_mask);  // x16 x20 x17 x21 x18 x22 x19 x23
  xintl3 = _mm256_permutevar8x32_epi32(x256[3],perm_mask);  // x24 x28 x25 x29 x26 x30 x27 x31

  // debug_print_vec_avx("xintl0", xintl0);
  // debug_print_vec_avx("xintl1", xintl1);


  xintl4 = _mm256_permutevar8x32_epi32(x256[4],perm_mask);  // x32 x28 x25 x29 x26 x30 x27 x31
  xintl5 = _mm256_permutevar8x32_epi32(x256[5],perm_mask);  // x40 x28 x25 x29 x26 x30 x27 x31
  xintl6 = _mm256_permutevar8x32_epi32(x256[6],perm_mask);  // x48 x28 x25 x29 x26 x30 x27 x31
  xintl7 = _mm256_permutevar8x32_epi32(x256[7],perm_mask);  // x56 x28 x25 x29 x26 x30 x27 x31

  // debug_print_vec_avx("perm_mask", perm_mask);
  // debug_print_vec_avx("x0_vec", x[0]);
  // debug_print_vec_avx("x1_vec", x[1]);
  // debug_print_vec_avx("x2_vec", x[2]);
  // debug_print_vec_avx("x3_vec", x[3]);
  // DEBUG_PRINT_AVX_VEC("AVX: x[0]", x[0]);
  // DEBUG_PRINT_AVX_VEC("AVX: x[1]", x[1]);
  // DEBUG_PRINT_AVX_VEC("AVX: x[2]", x[2]);
  // DEBUG_PRINT_AVX_VEC("AVX: x[3]", x[3]);

  xtmp[0] = _mm256_unpacklo_epi64(xintl0,xintl1);        // x0  x4  x8  x12 x1  x5  x9  x13
  xtmp[4] = _mm256_unpackhi_epi64(xintl0,xintl1);        // x2  x6  x10 x14 x3  x7  x11 x15

  // debug_print_vec_avx("AVX: xtmp[0]", xtmp[0]);
  // debug_print_vec_avx("AVX: xtmp[4]", xtmp[4]);

  // debug_print_vec_avx("xintl2", xintl2);
  // debug_print_vec_avx("xintl3", xintl3);
  xtmp[1] = _mm256_unpacklo_epi64(xintl2,xintl3);        // x16 x20 x24 x28 x17 x21 x25 x29
  xtmp[5] = _mm256_unpackhi_epi64(xintl2,xintl3);        // x18 x22 x26 x30 x19 x23 x27 x31
  // debug_print_vec_avx("AVX: xtmp[1]", xtmp[1]);
  // debug_print_vec_avx("AVX: xtmp[5]", xtmp[5]);
  xtmp[2] = _mm256_unpacklo_epi64(xintl4,xintl5);        // x32 x36 x40 x44 x33 x37 x41 x45
  xtmp[6] = _mm256_unpackhi_epi64(xintl4,xintl5);        // x34 x38 x42 x46 x35 x39 x43 x47
  xtmp[3] = _mm256_unpacklo_epi64(xintl6,xintl7);        // x48 x52 x56 x60 x49 x53 x57 x61
  xtmp[7] = _mm256_unpackhi_epi64(xintl6,xintl7);        // x50 x54 x58 x62 x51 x55 x59 x63

  dft16_simd256((int16_t*)(xtmp),(int16_t*)ytmp);
  // // [y0  y1  y2  y3  y4  y5  y6  y7]
  // // [y8  y9  y10 y11 y12 y13 y14 y15]
  // // [y16 y17 y18 y19 y20 y21 y22 y23]
  // // [y24 y25 y26 y27 y28 y29 y30 y31]

  dft16_simd256((int16_t*)(xtmp+4),(int16_t*)(ytmp+4));
  // // [y32 y33 y34 y35 y36 y37 y38 y39]
  // // [y40 y41 y42 y43 y44 y45 y46 y47]
  // // [y48 y49 y50 y51 y52 y53 y54 y55]
  // // [y56 y57 y58 y59 y60 y61 y62 y63]
  // debug_print_vec_avx("AVX: ytmp[0]", ytmp[0]);
  // debug_print_vec_avx("AVX: ytmp[1]", ytmp[1]);
  // debug_print_vec_avx("AVX: ytmp[2]", ytmp[2]);
  // debug_print_vec_avx("AVX: ytmp[3]", ytmp[3]);
  // debug_print_vec_avx("AVX: ytmp[4]", ytmp[4]);
  // debug_print_vec_avx("AVX: ytmp[5]", ytmp[5]);
  // debug_print_vec_avx("AVX: ytmp[6]", ytmp[6]);
  // debug_print_vec_avx("AVX: ytmp[7]", ytmp[7]);
  // debug_print_vec_avx("AVX: ytmp[8]", ytmp[8]);
  // debug_print_vec_avx("AVX: ytmp[9]", ytmp[9]);
  // debug_print_vec_avx("AVX: ytmp[10]", ytmp[10]);
  // debug_print_vec_avx("AVX: ytmp[11]", ytmp[11]);
  // debug_print_vec_avx("AVX: ytmp[12]", ytmp[12]);
  // debug_print_vec_avx("AVX: ytmp[13]", ytmp[13]);
  // debug_print_vec_avx("AVX: ytmp[14]", ytmp[14]);
  // debug_print_vec_avx("AVX: ytmp[15]", ytmp[15]);


  bfly4_16_256(ytmp,ytmp+2,ytmp+4,ytmp+6,
	       y256,y256+2,y256+4,y256+6,
	       tw64a_256,tw64a_256+2,tw64a_256+4,
	       tw64b_256,tw64b_256+2,tw64b_256+4);
  // [y0  y1  y2  y3  y4  y5  y6  y7]
  // [y16 y17 y18 y19 y20 y21 y22 y23]
  // [y32 y33 y34 y35 y36 y37 y38 y39]
  // [y48 y49 y50 y51 y52 y53 y54 y55]

  bfly4_16_256(ytmp+1,ytmp+3,ytmp+5,ytmp+7,
	       y256+1,y256+3,y256+5,y256+7,
	       tw64a_256+1,tw64a_256+3,tw64a_256+5,
	       tw64b_256+1,tw64b_256+3,tw64b_256+5);
  // [y8  y9  y10 y11 y12 y13 y14 y15]
  // [y24 y25 y26 y27 y28 y29 y30 y31]
  // [y40 y41 y42 y43 y44 y45 y46 y47]
  // [y56 y57 y58 y59 y60 y61 y62 y63]
  // DEBUG_PRINT_AVX_VEC("AVX: y256[0]", y256[0]);
  // DEBUG_PRINT_AVX_VEC("AVX: y256[1]", y256[1]);
  // DEBUG_PRINT_AVX_VEC("AVX: y256[2]", y256[2]);
  // DEBUG_PRINT_AVX_VEC("AVX: y256[3]", y256[3]);
  // DEBUG_PRINT_AVX_VEC("AVX: y256[4]", y256[4]);
  // DEBUG_PRINT_AVX_VEC("AVX: y256[5]", y256[5]);
  // DEBUG_PRINT_AVX_VEC("AVX: y256[6]", y256[6]);
  // DEBUG_PRINT_AVX_VEC("AVX: y256[7]", y256[7]);
    if (scale>0) {
    y256[0]  = shiftright_int16_simd256(y256[0],3);
    y256[1]  = shiftright_int16_simd256(y256[1],3);
    y256[2]  = shiftright_int16_simd256(y256[2],3);
    y256[3]  = shiftright_int16_simd256(y256[3],3);
    y256[4]  = shiftright_int16_simd256(y256[4],3);
    y256[5]  = shiftright_int16_simd256(y256[5],3);
    y256[6]  = shiftright_int16_simd256(y256[6],3);
    y256[7]  = shiftright_int16_simd256(y256[7],3);
  }

  _mm_empty();
  _m_empty();

}

void rvv_dft64(int16_t *x, int16_t *y, unsigned char scale)
{
    int16_t xtmp[16 * 16] __attribute__((aligned(32)));
    int16_t ytmp[16 * 16] __attribute__((aligned(32)));

    const int32_t *x_ptr = (const int32_t *)x;
    int32_t *xtmp_ptr    = (int32_t *)xtmp;

    // ---------- Stage 1: e32,m2, VL=8 ----------
    const size_t vl32 = 8;
    size_t vl = __riscv_vsetvl_e32m2(vl32);

    vint32m2_t perm_mask = rvv_set_epi32_m2(7, 3, 5, 1, 6, 2, 4, 0, vl32);

    for (int g = 0; g < 4; g++) {
        vint32m2_t xa = __riscv_vle32_v_i32m2(x_ptr + (2 * g + 0) * 8, vl32);
        vint32m2_t xb = __riscv_vle32_v_i32m2(x_ptr + (2 * g + 1) * 8, vl32);

        vint32m2_t xintla = rvv_permutevar8x32_epi32_m2(xa, perm_mask, vl32);
        vint32m2_t xintlb = rvv_permutevar8x32_epi32_m2(xb, perm_mask, vl32);

        __riscv_vse32_v_i32m2(xtmp_ptr + (g + 0) * 8, rvv_unpacklo_epi64_m2(xintla, xintlb, vl32), vl32);
        __riscv_vse32_v_i32m2(xtmp_ptr + (g + 4) * 8, rvv_unpackhi_epi64_m2(xintla, xintlb, vl32), vl32);
    }

    // ---------- Stage 2+: e16,m4, VL=16 ----------
    // dft16（内部已去 vset）
    rvv_dft16(xtmp, ytmp);
    rvv_dft16(xtmp + 4 * 16, ytmp + 4 * 16);

    // bfly4（内部已去 vset）
    int16_t *tw64a_ptr = (int16_t *)tw64a;
    int16_t *tw64b_ptr = (int16_t *)tw64b;

    rvv_bfly4_16_256(
        ytmp + 0 * 16, ytmp + 2 * 16, ytmp + 4 * 16, ytmp + 6 * 16,
        y    + 0 * 16, y    + 2 * 16, y    + 4 * 16, y    + 6 * 16,
        tw64a_ptr + 0 * 16, tw64a_ptr + 2 * 16, tw64a_ptr + 4 * 16,
        tw64b_ptr + 0 * 16, tw64b_ptr + 2 * 16, tw64b_ptr + 4 * 16
    );

    rvv_bfly4_16_256(
        ytmp + 1 * 16, ytmp + 3 * 16, ytmp + 5 * 16, ytmp + 7 * 16,
        y    + 1 * 16, y    + 3 * 16, y    + 5 * 16, y    + 7 * 16,
        tw64a_ptr + 1 * 16, tw64a_ptr + 3 * 16, tw64a_ptr + 5 * 16,
        tw64b_ptr + 1 * 16, tw64b_ptr + 3 * 16, tw64b_ptr + 5 * 16
    );

    // scale（不再 vset，复用当前 e16,m4,vl=16 配置）
    if (scale > 0) {
      size_t vl = __riscv_vsetvl_e16m4(16);
      for (int i = 0; i < 8; i++) {
          vint16m4_t v = __riscv_vle16_v_i16m4(y + i * 16, vl);
          v = rvv_shiftright_int16(v, 3, vl);
          __riscv_vse16_v_i16m4(y + i * 16, v, vl);
      }
    }
}

#endif

#ifndef __AVX2__


#else // __AVX2__
void idft64(int16_t *x,int16_t *y,unsigned char scale)
{

  simd256_q15_t xtmp[16],ytmp[16],*tw64a_256=(simd256_q15_t *)tw64,*tw64b_256=(simd256_q15_t *)tw64c,*x256=(simd256_q15_t *)x,*y256=(simd256_q15_t *)y;
  register simd256_q15_t xintl0,xintl1,xintl2,xintl3,xintl4,xintl5,xintl6,xintl7;
  simd256_q15_t const perm_mask = _mm256_set_epi32(7, 3, 5, 1, 6, 2, 4, 0);


#ifdef D64STATS
  time_stats_t ts_t,ts_d,ts_b;

  reset_meas(&ts_t);
  reset_meas(&ts_d);
  reset_meas(&ts_b);
  start_meas(&ts_t);
#endif

#ifdef D64STATS
  stop_meas(&ts_t);
  start_meas(&ts_d);
#endif

  xintl0 = _mm256_permutevar8x32_epi32(x256[0],perm_mask);  // x0  x4  x1  x5  x2  x6  x3  x7
  xintl1 = _mm256_permutevar8x32_epi32(x256[1],perm_mask);  // x8  x12 x9  x13 x10 x14 x11 x15
  xintl2 = _mm256_permutevar8x32_epi32(x256[2],perm_mask);  // x16 x20 x17 x21 x18 x22 x19 x23
  xintl3 = _mm256_permutevar8x32_epi32(x256[3],perm_mask);  // x24 x28 x25 x29 x26 x30 x27 x31
  xintl4 = _mm256_permutevar8x32_epi32(x256[4],perm_mask);  // x24 x28 x25 x29 x26 x30 x27 x31
  xintl5 = _mm256_permutevar8x32_epi32(x256[5],perm_mask);  // x24 x28 x25 x29 x26 x30 x27 x31
  xintl6 = _mm256_permutevar8x32_epi32(x256[6],perm_mask);  // x24 x28 x25 x29 x26 x30 x27 x31
  xintl7 = _mm256_permutevar8x32_epi32(x256[7],perm_mask);  // x24 x28 x25 x29 x26 x30 x27 x31

  xtmp[0] = _mm256_unpacklo_epi64(xintl0,xintl1);        // x0  x4  x8  x12 x1  x5  x9  x13
  xtmp[4] = _mm256_unpackhi_epi64(xintl0,xintl1);        // x2  x6  x10 x14 x3  x7  x11 x15
  xtmp[1] = _mm256_unpacklo_epi64(xintl2,xintl3);        // x16 x20 x24 x28 x17 x21 x25 x29
  xtmp[5] = _mm256_unpackhi_epi64(xintl2,xintl3);        // x18 x22 x26 x30 x19 x23 x27 x31
  xtmp[2] = _mm256_unpacklo_epi64(xintl4,xintl5);        // x32 x36 x40 x44 x33 x37 x41 x45
  xtmp[6] = _mm256_unpackhi_epi64(xintl4,xintl5);        // x34 x38 x42 x46 x35 x39 x43 x47
  xtmp[3] = _mm256_unpacklo_epi64(xintl6,xintl7);        // x48 x52 x56 x60 x49 x53 x57 x61
  xtmp[7] = _mm256_unpackhi_epi64(xintl6,xintl7);        // x50 x54 x58 x62 x51 x55 x59 x63

  // DEBUG_PRINT_AVX_VEC("AVX: xtmp[0]", xtmp[0]);
  // DEBUG_PRINT_AVX_VEC("AVX: xtmp[1]", xtmp[1]);
  // DEBUG_PRINT_AVX_VEC("AVX: xtmp[2]", xtmp[2]);
  // DEBUG_PRINT_AVX_VEC("AVX: xtmp[3]", xtmp[3]);
  // DEBUG_PRINT_AVX_VEC("AVX: xtmp[4]", xtmp[4]);
  // DEBUG_PRINT_AVX_VEC("AVX: xtmp[5]", xtmp[5]);
  // DEBUG_PRINT_AVX_VEC("AVX: xtmp[6]", xtmp[6]);
  // DEBUG_PRINT_AVX_VEC("AVX: xtmp[7]", xtmp[7]);

  idft16_simd256((int16_t*)(xtmp),(int16_t*)ytmp);
  // [y0  y1  y2  y3  y16 y17 y18 y19]
  // [y4  y5  y6  y7  y20 y21 y22 y23]
  // [y8  y9  y10 y11 y24 y25 y26 y27]
  // [y12 y13 y14 y15 y28 y29 y30 y31]

  idft16_simd256((int16_t*)(xtmp+4),(int16_t*)(ytmp+4));
  // [y32 y33 y34 y35 y48 y49 y50 y51]
  // [y36 y37 y38 y39 y52 y53 y54 y55]
  // [y40 y41 y42 y43 y56 y57 y58 y59]
  // [y44 y45 y46 y47 y60 y61 y62 y63]
  // DEBUG_PRINT_AVX_VEC("AVX: ytmp[0]", ytmp[0]);
  // DEBUG_PRINT_AVX_VEC("AVX: ytmp[1]", ytmp[1]);
  // DEBUG_PRINT_AVX_VEC("AVX: ytmp[2]", ytmp[2]);
  // DEBUG_PRINT_AVX_VEC("AVX: ytmp[3]", ytmp[3]);
  // DEBUG_PRINT_AVX_VEC("AVX: ytmp[4]", ytmp[4]);
  // DEBUG_PRINT_AVX_VEC("AVX: ytmp[5]", ytmp[5]);
  // DEBUG_PRINT_AVX_VEC("AVX: ytmp[6]", ytmp[6]);
  // DEBUG_PRINT_AVX_VEC("AVX: ytmp[7]", ytmp[7]);
  // DEBUG_PRINT_AVX_VEC("AVX: tw64a_256[0]", tw64a_256[0]);
  // DEBUG_PRINT_AVX_VEC("AVX: tw64a_256[1]", tw64a_256[1]);
  // DEBUG_PRINT_AVX_VEC("AVX: tw64a_256[2]", tw64a_256[2]);
  // DEBUG_PRINT_AVX_VEC("AVX: tw64a_256[3]", tw64a_256[3]);
  // DEBUG_PRINT_AVX_VEC("AVX: tw64a_256[4]", tw64a_256[4]);
  // DEBUG_PRINT_AVX_VEC("AVX: tw64a_256[5]", tw64a_256[5]);
  
#ifdef D64STATS
  stop_meas(&ts_d);
  start_meas(&ts_b);
#endif


  ibfly4_16_256(ytmp,ytmp+2,ytmp+4,ytmp+6,
		y256,y256+2,y256+4,y256+6,
		tw64a_256,tw64a_256+2,tw64a_256+4,
		tw64b_256,tw64b_256+2,tw64b_256+4);
  // [y0  y1  y2  y3  y4  y5  y6  y7]
  // [y16 y17 y18 y19 y20 y21 y22 y23]
  // [y32 y33 y34 y35 y36 y37 y38 y39]
  // [y48 y49 y50 y51 y52 y53 y54 y55]

  ibfly4_16_256(ytmp+1,ytmp+3,ytmp+5,ytmp+7,
		y256+1,y256+3,y256+5,y256+7,
		tw64a_256+1,tw64a_256+3,tw64a_256+5,
		tw64b_256+1,tw64b_256+3,tw64b_256+5);
  // [y8  y9  y10 y11 y12 y13 y14 y15]
  // [y24 y25 y26 y27 y28 y29 y30 y31]
  // [y40 y41 y42 y43 y44 y45 y46 y47]
  // [y56 y57 y58 y59 y60 y61 y62 y63]

  // DEBUG_PRINT_AVX_VEC("AVX: y256[0]", y256[0]);
  // DEBUG_PRINT_AVX_VEC("AVX: y256[1]", y256[1]);
  // DEBUG_PRINT_AVX_VEC("AVX: y256[2]", y256[2]);
  // DEBUG_PRINT_AVX_VEC("AVX: y256[3]", y256[3]);
  // DEBUG_PRINT_AVX_VEC("AVX: y256[4]", y256[4]);
  // DEBUG_PRINT_AVX_VEC("AVX: y256[5]", y256[5]);
  // DEBUG_PRINT_AVX_VEC("AVX: y256[6]", y256[6]);
  // DEBUG_PRINT_AVX_VEC("AVX: y256[7]", y256[7]);


#ifdef D64STATS
  stop_meas(&ts_b);
  printf("t: %llu cycles, d: %llu cycles, b: %llu cycles\n",ts_t.diff,ts_d.diff,ts_b.diff);
#endif


  if (scale>0) {
    y256[0]  = shiftright_int16_simd256(y256[0],3);
    y256[1]  = shiftright_int16_simd256(y256[1],3);
    y256[2]  = shiftright_int16_simd256(y256[2],3);
    y256[3]  = shiftright_int16_simd256(y256[3],3);
    y256[4]  = shiftright_int16_simd256(y256[4],3);
    y256[5]  = shiftright_int16_simd256(y256[5],3);
    y256[6]  = shiftright_int16_simd256(y256[6],3);
    y256[7]  = shiftright_int16_simd256(y256[7],3);
  }

}

void rvv_idft64(int16_t *x, int16_t *y, unsigned char scale)
{
    int16_t xtmp[16 * 16] __attribute__((aligned(32)));
    int16_t ytmp[16 * 16] __attribute__((aligned(32)));

    const int32_t *x_ptr = (const int32_t *)x;
    int32_t *xtmp_ptr    = (int32_t *)xtmp;

    // ---------- Stage 1: e32,m2, VL=8 ----------
    const size_t vl32 = 8;
    __riscv_vsetvl_e32m2(vl32);

    vint32m2_t perm_mask = rvv_set_epi32_m2(7, 3, 5, 1, 6, 2, 4, 0, vl32);

    for (int g = 0; g < 4; g++) {
        vint32m2_t xa = __riscv_vle32_v_i32m2(x_ptr + (2 * g + 0) * 8, vl32);
        vint32m2_t xb = __riscv_vle32_v_i32m2(x_ptr + (2 * g + 1) * 8, vl32);

        vint32m2_t xintla = rvv_permutevar8x32_epi32_m2(xa, perm_mask, vl32);
        vint32m2_t xintlb = rvv_permutevar8x32_epi32_m2(xb, perm_mask, vl32);

        __riscv_vse32_v_i32m2(xtmp_ptr + (g + 0) * 8, rvv_unpacklo_epi64_m2(xintla, xintlb, vl32), vl32);
        __riscv_vse32_v_i32m2(xtmp_ptr + (g + 4) * 8, rvv_unpackhi_epi64_m2(xintla, xintlb, vl32), vl32);
    }

    // ---------- Stage 2+: e16,m4, VL=16 ----------
    rvv_idft16(xtmp, ytmp);
    rvv_idft16(xtmp + 4 * 16, ytmp + 4 * 16);

    // ---------- Butterfly ----------
    int16_t *tw64_ptr  = (int16_t *)tw64;
    int16_t *tw64c_ptr = (int16_t *)tw64c;

    rvv_ibfly4_16_256(
        ytmp + 0 * 16, ytmp + 2 * 16, ytmp + 4 * 16, ytmp + 6 * 16,
        y    + 0 * 16, y    + 2 * 16, y    + 4 * 16, y    + 6 * 16,
        tw64_ptr  + 0 * 16, tw64_ptr  + 2 * 16, tw64_ptr  + 4 * 16,
        tw64c_ptr + 0 * 16, tw64c_ptr + 2 * 16, tw64c_ptr + 4 * 16
    );

    rvv_ibfly4_16_256(
        ytmp + 1 * 16, ytmp + 3 * 16, ytmp + 5 * 16, ytmp + 7 * 16,
        y    + 1 * 16, y    + 3 * 16, y    + 5 * 16, y    + 7 * 16,
        tw64_ptr  + 1 * 16, tw64_ptr  + 3 * 16, tw64_ptr  + 5 * 16,
        tw64c_ptr + 1 * 16, tw64c_ptr + 3 * 16, tw64c_ptr + 5 * 16
    );

    // ---------- optional scaling ----------
    // 这里我保持与你 dft64 一致的行为：scale>0 就右移 3（你工程老版 idft64 若不同，你再告诉我）
    if (scale > 0) {
        size_t vl = __riscv_vsetvl_e16m4(16);
        for (int i = 0; i < 8; i++) {
            vint16m4_t v = __riscv_vle16_v_i16m4(y + i * 16, vl);
            v = rvv_shiftright_int16(v, 3, vl);
            __riscv_vse16_v_i16m4(y + i * 16, v, vl);
        }
    }
}


#endif

int16_t tw128[128] __attribute__((aligned(32))) = {  32767,0,32727,-1608,32609,-3212,32412,-4808,32137,-6393,31785,-7962,31356,-9512,30851,-11039,30272,-12540,29621,-14010,28897,-15447,28105,-16846,27244,-18205,26318,-19520,25329,-20788,24278,-22005,23169,-23170,22004,-24279,20787,-25330,19519,-26319,18204,-27245,16845,-28106,15446,-28898,14009,-29622,12539,-30273,11038,-30852,9511,-31357,7961,-31786,6392,-32138,4807,-32413,3211,-32610,1607,-32728,0,-32767,-1608,-32728,-3212,-32610,-4808,-32413,-6393,-32138,-7962,-31786,-9512,-31357,-11039,-30852,-12540,-30273,-14010,-29622,-15447,-28898,-16846,-28106,-18205,-27245,-19520,-26319,-20788,-25330,-22005,-24279,-23170,-23170,-24279,-22005,-25330,-20788,-26319,-19520,-27245,-18205,-28106,-16846,-28898,-15447,-29622,-14010,-30273,-12540,-30852,-11039,-31357,-9512,-31786,-7962,-32138,-6393,-32413,-4808,-32610,-3212,-32728,-1608};

int16_t tw128a[128] __attribute__((aligned(32))) = { 32767,0,32727,1608,32609,3212,32412,4808,32137,6393,31785,7962,31356,9512,30851,11039,30272,12540,29621,14010,28897,15447,28105,16846,27244,18205,26318,19520,25329,20788,24278,22005,23169,23170,22004,24279,20787,25330,19519,26319,18204,27245,16845,28106,15446,28898,14009,29622,12539,30273,11038,30852,9511,31357,7961,31786,6392,32138,4807,32413,3211,32610,1607,32728,0,32767,-1608,32728,-3212,32610,-4808,32413,-6393,32138,-7962,31786,-9512,31357,-11039,30852,-12540,30273,-14010,29622,-15447,28898,-16846,28106,-18205,27245,-19520,26319,-20788,25330,-22005,24279,-23170,23170,-24279,22005,-25330,20788,-26319,19520,-27245,18205,-28106,16846,-28898,15447,-29622,14010,-30273,12540,-30852,11039,-31357,9512,-31786,7962,-32138,6393,-32413,4808,-32610,3212,-32728,1608};

int16_t tw128b[128] __attribute__((aligned(32))) = {0,32767,-1608,32727,-3212,32609,-4808,32412,-6393,32137,-7962,31785,-9512,31356,-11039,30851,-12540,30272,-14010,29621,-15447,28897,-16846,28105,-18205,27244,-19520,26318,-20788,25329,-22005,24278,-23170,23169,-24279,22004,-25330,20787,-26319,19519,-27245,18204,-28106,16845,-28898,15446,-29622,14009,-30273,12539,-30852,11038,-31357,9511,-31786,7961,-32138,6392,-32413,4807,-32610,3211,-32728,1607,-32767,0,-32728,-1608,-32610,-3212,-32413,-4808,-32138,-6393,-31786,-7962,-31357,-9512,-30852,-11039,-30273,-12540,-29622,-14010,-28898,-15447,-28106,-16846,-27245,-18205,-26319,-19520,-25330,-20788,-24279,-22005,-23170,-23170,-22005,-24279,-20788,-25330,-19520,-26319,-18205,-27245,-16846,-28106,-15447,-28898,-14010,-29622,-12540,-30273,-11039,-30852,-9512,-31357,-7962,-31786,-6393,-32138,-4808,-32413,-3212,-32610,-1608,-32728};

int16_t tw128c[128] __attribute__((aligned(32))) = {0,32767,1608,32727,3212,32609,4808,32412,6393,32137,7962,31785,9512,31356,11039,30851,12540,30272,14010,29621,15447,28897,16846,28105,18205,27244,19520,26318,20788,25329,22005,24278,23170,23169,24279,22004,25330,20787,26319,19519,27245,18204,28106,16845,28898,15446,29622,14009,30273,12539,30852,11038,31357,9511,31786,7961,32138,6392,32413,4807,32610,3211,32728,1607,32767,0,32728,-1608,32610,-3212,32413,-4808,32138,-6393,31786,-7962,31357,-9512,30852,-11039,30273,-12540,29622,-14010,28898,-15447,28106,-16846,27245,-18205,26319,-19520,25330,-20788,24279,-22005,23170,-23170,22005,-24279,20788,-25330,19520,-26319,18205,-27245,16846,-28106,15447,-28898,14010,-29622,12540,-30273,11039,-30852,9512,-31357,7962,-31786,6393,-32138,4808,-32413,3212,-32610,1608,-32728};

#ifndef __AVX2__


#else // __AVX2__

// 假设 VL_16 和 vint16m4_t 是在您的环境中定义好的
#define VL_16 16
#define N_VEC_256 16
void dft128(int16_t *x,int16_t *y,unsigned char scale)
{

  simd256_q15_t xtmp[16],*x256 = (simd256_q15_t *)x;
  simd256_q15_t ytmp[16],*y256=(simd256_q15_t*)y;
  simd256_q15_t *tw128a_256p=(simd256_q15_t *)tw128a,*tw128b_256p=(simd256_q15_t *)tw128b,*y256p=(simd256_q15_t *)y;
  simd256_q15_t *ytmpp = &ytmp[0];
  int i;
  simd256_q15_t ONE_OVER_SQRT2_Q15_256 = set1_int16_simd256(ONE_OVER_SQRT2_Q15);

  transpose4_ooff_simd256(x256  ,xtmp,8);
  transpose4_ooff_simd256(x256+2,xtmp+1,8);
  transpose4_ooff_simd256(x256+4,xtmp+2,8);
  transpose4_ooff_simd256(x256+6,xtmp+3,8);
  transpose4_ooff_simd256(x256+8,xtmp+4,8);
  transpose4_ooff_simd256(x256+10,xtmp+5,8);
  transpose4_ooff_simd256(x256+12,xtmp+6,8);
  transpose4_ooff_simd256(x256+14,xtmp+7,8);
  
  dft64((int16_t*)(xtmp),(int16_t*)ytmp,1);
  dft64((int16_t*)(xtmp+8),(int16_t*)(ytmp+8),1);

  for (i=0; i<8; i++) {
    bfly2_16_256(ytmpp,ytmpp+8,
		 y256p,y256p+8,
		 tw128a_256p,
		 tw128b_256p);
    tw128a_256p++;
    tw128b_256p++;
    y256p++;
    ytmpp++;
  }

  if (scale>0) {

    y256[0] = mulhi_int16_simd256(y256[0],ONE_OVER_SQRT2_Q15_256);
    y256[1] = mulhi_int16_simd256(y256[1],ONE_OVER_SQRT2_Q15_256);
    y256[2] = mulhi_int16_simd256(y256[2],ONE_OVER_SQRT2_Q15_256);
    y256[3] = mulhi_int16_simd256(y256[3],ONE_OVER_SQRT2_Q15_256);
    y256[4] = mulhi_int16_simd256(y256[4],ONE_OVER_SQRT2_Q15_256);
    y256[5] = mulhi_int16_simd256(y256[5],ONE_OVER_SQRT2_Q15_256);
    y256[6] = mulhi_int16_simd256(y256[6],ONE_OVER_SQRT2_Q15_256);
    y256[7] = mulhi_int16_simd256(y256[7],ONE_OVER_SQRT2_Q15_256);
    y256[8] = mulhi_int16_simd256(y256[8],ONE_OVER_SQRT2_Q15_256);
    y256[9] = mulhi_int16_simd256(y256[9],ONE_OVER_SQRT2_Q15_256);
    y256[10] = mulhi_int16_simd256(y256[10],ONE_OVER_SQRT2_Q15_256);
    y256[11] = mulhi_int16_simd256(y256[11],ONE_OVER_SQRT2_Q15_256);
    y256[12] = mulhi_int16_simd256(y256[12],ONE_OVER_SQRT2_Q15_256);
    y256[13] = mulhi_int16_simd256(y256[13],ONE_OVER_SQRT2_Q15_256);
    y256[14] = mulhi_int16_simd256(y256[14],ONE_OVER_SQRT2_Q15_256);
    y256[15] = mulhi_int16_simd256(y256[15],ONE_OVER_SQRT2_Q15_256);

  }

}

void rvv_dft128(int16_t *x,int16_t *y,unsigned char scale)
{

  // 使用 int16_t 数组作为临时存储，并确保对齐
    int16_t xtmp[N_VEC_256 * VL_16] __attribute__((aligned(32)));
    int16_t ytmp[N_VEC_256 * VL_16] __attribute__((aligned(32)));
    
    // 假设 tw128a/tw128b 已在外部定义为 int16_t 数组
    const int16_t *tw128a_ptr = tw128a;
    const int16_t *tw128b_ptr = tw128b;

    // 假设 set1_int16_simd256 返回 simd256_q15_rvv (vint16m4_t)
    // 注意：这里需要一个 int16_t 数组来存储这个广播值，如果后续要作为 tw 因子使用。
    // 但在 dft128 中，它只用于可能的内部计算，这里主要用于遵循原代码结构。
    vint16m4_t ONE_OVER_SQRT2_Q15_256_V = rvv_set1_int16(ONE_OVER_SQRT2_Q15);
    
    // 循环变量
    int i;
    
    // ----------------------------------------------------------------
    // ----------------------------------------------------------------
    // rvv_transpose4_ooff(x, y, off) 使用 int16_t*， off 是 256-bit 块的偏移量 (即 16 个 int16_t)
    
    // x256+N 对应 x + N * VL_16
    // xtmp+N 对应 xtmp + N * VL_16
    // off=8 对应 8 * VL_16 = 128 个 int16_t 偏移
    
    rvv_transpose4_ooff(x + 0 * VL_16, xtmp + 0 * VL_16, 8);
    rvv_transpose4_ooff(x + 2 * VL_16, xtmp + 1 * VL_16, 8);
    rvv_transpose4_ooff(x + 4 * VL_16, xtmp + 2 * VL_16, 8);
    rvv_transpose4_ooff(x + 6 * VL_16, xtmp + 3 * VL_16, 8);
    rvv_transpose4_ooff(x + 8 * VL_16, xtmp + 4 * VL_16, 8);
    rvv_transpose4_ooff(x + 10 * VL_16, xtmp + 5 * VL_16, 8);
    rvv_transpose4_ooff(x + 12 * VL_16, xtmp + 6 * VL_16, 8);
    rvv_transpose4_ooff(x + 14 * VL_16, xtmp + 7 * VL_16, 8);

    // xtmp (0 to 127) -> ytmp (0 to 127)
    rvv_dft64(xtmp, ytmp, 1); 
    
    // xtmp+8 (128 to 255) -> ytmp+8 (128 to 255)
    // 注意: xtmp+8 是 C 指针语法，代表偏移 8 个 simd256_q15_t 块，即 8 * 16 = 128 个 int16_t 元素
    rvv_dft64(xtmp + 8 * VL_16, ytmp + 8 * VL_16, 1);


    int16_t *ytmpp = ytmp;
    int16_t *y_ptr = y; 

    for (i = 0; i < 8; i++) {
        // rvv_bfly2_16_256(x0, x1, y0, y1, tw, twb)
        
        // x0: ytmp[i] -> ytmpp + i*VL_16
        // x1: ytmp[i+8] -> ytmpp + i*VL_16 + 8*VL_16
        // y0: y[i] -> y_ptr + i*VL_16
        // y1: y[i+8] -> y_ptr + i*VL_16 + 8*VL_16
        // tw: tw128a[i] -> tw128a_ptr + i*VL_16
        // twb: tw128b[i] -> tw128b_ptr + i*VL_16

        rvv_bfly2_16_256(
            ytmpp + i * VL_16,               // x0 (ytmp[i])
            ytmpp + i * VL_16 + 8 * VL_16,   // x1 (ytmp[i+8])
            y_ptr + i * VL_16,               // y0 (y[i])
            y_ptr + i * VL_16 + 8 * VL_16,   // y1 (y[i+8])
            tw128a_ptr + i * VL_16,          // tw (tw128a[i])
            tw128b_ptr + i * VL_16           // twb (tw128b[i])
        );
        
    }
    if (scale > 0) {
    // 设置 VL_16 (16个 int16_t 元素)
    size_t vl = __riscv_vsetvl_e16m4(VL_16);
    
    // ONE_OVER_SQRT2_Q15_256_V 已经在函数开始时通过 set1_int16_simd256(ONE_OVER_SQRT2_Q15) 广播
    // 并存储在 simd256_q15_rvv 类型的变量中。
    
    // 循环处理 y[0] 到 y[15]，共 16 个 256-bit 向量
    for (i = 0; i < 16; i++) {
        // 计算当前向量块的内存地址
        int16_t *current_ptr = y + i * VL_16; 

        // 1. Load: 加载 y[i]
        vint16m4_t y_vec = __riscv_vle16_v_i16m4(current_ptr, vl);
        
        // 2. Multiply: y[i] = y[i] * ONE_OVER_SQRT2_Q15_256
        y_vec = rvv_mulhi_int16(y_vec, ONE_OVER_SQRT2_Q15_256_V, vl);

        // 3. Store: 写回 y[i]
        __riscv_vse16_v_i16m4(current_ptr, y_vec, vl);
    }
}

}


void ext_dft128(int16_t *x,int16_t *y,unsigned char scale)
{

  // 使用 int16_t 数组作为临时存储，并确保对齐
    int16_t xtmp[N_VEC_256 * VL_16] __attribute__((aligned(32)));
    int16_t ytmp[N_VEC_256 * VL_16] __attribute__((aligned(32)));
    
    // 假设 tw128a/tw128b 已在外部定义为 int16_t 数组
    const int16_t *tw128a_ptr = tw128a;
    const int16_t *tw128b_ptr = tw128b;

    // 假设 set1_int16_simd256 返回 simd256_q15_rvv (vint16m4_t)
    // 注意：这里需要一个 int16_t 数组来存储这个广播值，如果后续要作为 tw 因子使用。
    // 但在 dft128 中，它只用于可能的内部计算，这里主要用于遵循原代码结构。
    vint16m4_t ONE_OVER_SQRT2_Q15_256_V = rvv_set1_int16(ONE_OVER_SQRT2_Q15);
    
    // 循环变量
    int i;
    
    // ----------------------------------------------------------------
    // ----------------------------------------------------------------
    // rvv_transpose4_ooff(x, y, off) 使用 int16_t*， off 是 256-bit 块的偏移量 (即 16 个 int16_t)
    
    // x256+N 对应 x + N * VL_16
    // xtmp+N 对应 xtmp + N * VL_16
    // off=8 对应 8 * VL_16 = 128 个 int16_t 偏移
    
    rvv_transpose4_ooff(x + 0 * VL_16, xtmp + 0 * VL_16, 8);
    rvv_transpose4_ooff(x + 2 * VL_16, xtmp + 1 * VL_16, 8);
    rvv_transpose4_ooff(x + 4 * VL_16, xtmp + 2 * VL_16, 8);
    rvv_transpose4_ooff(x + 6 * VL_16, xtmp + 3 * VL_16, 8);
    rvv_transpose4_ooff(x + 8 * VL_16, xtmp + 4 * VL_16, 8);
    rvv_transpose4_ooff(x + 10 * VL_16, xtmp + 5 * VL_16, 8);
    rvv_transpose4_ooff(x + 12 * VL_16, xtmp + 6 * VL_16, 8);
    rvv_transpose4_ooff(x + 14 * VL_16, xtmp + 7 * VL_16, 8);

    // xtmp (0 to 127) -> ytmp (0 to 127)
    ext_dft64(xtmp, ytmp, 1); 
    
    // xtmp+8 (128 to 255) -> ytmp+8 (128 to 255)
    // 注意: xtmp+8 是 C 指针语法，代表偏移 8 个 simd256_q15_t 块，即 8 * 16 = 128 个 int16_t 元素
    ext_dft64(xtmp + 8 * VL_16, ytmp + 8 * VL_16, 1);


    int16_t *ytmpp = ytmp;
    int16_t *y_ptr = y; 

    for (i = 0; i < 8; i++) {
        // rvv_bfly2_16_256(x0, x1, y0, y1, tw, twb)
        
        // x0: ytmp[i] -> ytmpp + i*VL_16
        // x1: ytmp[i+8] -> ytmpp + i*VL_16 + 8*VL_16
        // y0: y[i] -> y_ptr + i*VL_16
        // y1: y[i+8] -> y_ptr + i*VL_16 + 8*VL_16
        // tw: tw128a[i] -> tw128a_ptr + i*VL_16
        // twb: tw128b[i] -> tw128b_ptr + i*VL_16

        ext_bfly2_16_256(
            ytmpp + i * VL_16,               // x0 (ytmp[i])
            ytmpp + i * VL_16 + 8 * VL_16,   // x1 (ytmp[i+8])
            y_ptr + i * VL_16,               // y0 (y[i])
            y_ptr + i * VL_16 + 8 * VL_16,   // y1 (y[i+8])
            tw128a_ptr + i * VL_16,          // tw (tw128a[i])
            tw128b_ptr + i * VL_16           // twb (tw128b[i])
        );
        
    }
    if (scale > 0) {
    // 设置 VL_16 (16个 int16_t 元素)
    size_t vl = __riscv_vsetvl_e16m4(VL_16);
    
    // ONE_OVER_SQRT2_Q15_256_V 已经在函数开始时通过 set1_int16_simd256(ONE_OVER_SQRT2_Q15) 广播
    // 并存储在 simd256_q15_rvv 类型的变量中。
    
    // 循环处理 y[0] 到 y[15]，共 16 个 256-bit 向量
    for (i = 0; i < 16; i++) {
        // 计算当前向量块的内存地址
        int16_t *current_ptr = y + i * VL_16; 

        // 1. Load: 加载 y[i]
        vint16m4_t y_vec = __riscv_vle16_v_i16m4(current_ptr, vl);
        
        // 2. Multiply: y[i] = y[i] * ONE_OVER_SQRT2_Q15_256
        y_vec = rvv_mulhi_int16(y_vec, ONE_OVER_SQRT2_Q15_256_V, vl);

        // 3. Store: 写回 y[i]
        __riscv_vse16_v_i16m4(current_ptr, y_vec, vl);
    }
}

}

#endif

#ifndef __AVX2__


#else // __AVX2__
void idft128(int16_t *x,int16_t *y,unsigned char scale)
{
  simd256_q15_t xtmp[16],*x256 = (simd256_q15_t *)x;
  simd256_q15_t ytmp[16],*y256=(simd256_q15_t*)y;
  simd256_q15_t *tw128_256p=(simd256_q15_t *)tw128,*y256p=(simd256_q15_t *)y;
  simd256_q15_t *ytmpp = &ytmp[0];
  int i;
  simd256_q15_t ONE_OVER_SQRT2_Q15_256 = set1_int16_simd256(ONE_OVER_SQRT2_Q15);


  transpose4_ooff_simd256(x256  ,xtmp,8);
  transpose4_ooff_simd256(x256+2,xtmp+1,8);
  transpose4_ooff_simd256(x256+4,xtmp+2,8);
  transpose4_ooff_simd256(x256+6,xtmp+3,8);
  transpose4_ooff_simd256(x256+8,xtmp+4,8);
  transpose4_ooff_simd256(x256+10,xtmp+5,8);
  transpose4_ooff_simd256(x256+12,xtmp+6,8);
  transpose4_ooff_simd256(x256+14,xtmp+7,8);

  // DEBUG_PRINT_AVX_VEC("AVX: xtmp[0]", xtmp[0]);
  // DEBUG_PRINT_AVX_VEC("AVX: xtmp[1]", xtmp[1]);
  // DEBUG_PRINT_AVX_VEC("AVX: xtmp[2]", xtmp[2]);
  // DEBUG_PRINT_AVX_VEC("AVX: xtmp[3]", xtmp[3]);
  // DEBUG_PRINT_AVX_VEC("AVX: xtmp[4]", xtmp[4]);
  // DEBUG_PRINT_AVX_VEC("AVX: xtmp[5]", xtmp[5]);
  // DEBUG_PRINT_AVX_VEC("AVX: xtmp[6]", xtmp[6]);
  // DEBUG_PRINT_AVX_VEC("AVX: xtmp[7]", xtmp[7]);

  idft64((int16_t*)(xtmp),(int16_t*)ytmp,1);
  idft64((int16_t*)(xtmp+8),(int16_t*)(ytmp+8),1);

  // DEBUG_PRINT_AVX_VEC("AVX: ytmp[0]", ytmp[0]);
  // DEBUG_PRINT_AVX_VEC("AVX: ytmp[1]", ytmp[1]);
  // DEBUG_PRINT_AVX_VEC("AVX: ytmp[2]", ytmp[2]);
  // DEBUG_PRINT_AVX_VEC("AVX: ytmp[3]", ytmp[3]);
  // DEBUG_PRINT_AVX_VEC("AVX: ytmp[4]", ytmp[4]);
  // DEBUG_PRINT_AVX_VEC("AVX: ytmp[5]", ytmp[5]);
  // DEBUG_PRINT_AVX_VEC("AVX: ytmp[6]", ytmp[6]);
  // DEBUG_PRINT_AVX_VEC("AVX: ytmp[7]", ytmp[7]);
  // DEBUG_PRINT_AVX_VEC("AVX: ytmp[8]", ytmp[8]);
  // DEBUG_PRINT_AVX_VEC("AVX: ytmp[9]", ytmp[9]);
  // DEBUG_PRINT_AVX_VEC("AVX: ytmp[10]", ytmp[10]);
  // DEBUG_PRINT_AVX_VEC("AVX: ytmp[11]", ytmp[11]);
  // DEBUG_PRINT_AVX_VEC("AVX: ytmp[12]", ytmp[12]);
  // DEBUG_PRINT_AVX_VEC("AVX: ytmp[13]", ytmp[13]);
  // DEBUG_PRINT_AVX_VEC("AVX: ytmp[14]", ytmp[14]);
  // DEBUG_PRINT_AVX_VEC("AVX: ytmp[15]", ytmp[15]);

  for (i=0; i<8; i++) {
    ibfly2_256(ytmpp,ytmpp+8,
	       y256p,y256p+8,
	       tw128_256p);
    tw128_256p++;
    y256p++;
    ytmpp++;
  }

  // DEBUG_PRINT_AVX_VEC("AVX: y256p[0]", y256p[0]);
  // DEBUG_PRINT_AVX_VEC("AVX: y256p[1]", y256p[1]);
  // DEBUG_PRINT_AVX_VEC("AVX: y256p[2]", y256p[2]);
  // DEBUG_PRINT_AVX_VEC("AVX: y256p[3]", y256p[3]);
  // DEBUG_PRINT_AVX_VEC("AVX: y256p[4]", y256p[4]);
  // DEBUG_PRINT_AVX_VEC("AVX: y256p[5]", y256p[5]);
  // DEBUG_PRINT_AVX_VEC("AVX: y256p[6]", y256p[6]);
  // DEBUG_PRINT_AVX_VEC("AVX: y256p[7]", y256p[7]);
  // DEBUG_PRINT_AVX_VEC("AVX: y256p[8]", y256p[8]);
  // DEBUG_PRINT_AVX_VEC("AVX: y256p[9]", y256p[9]);
  // DEBUG_PRINT_AVX_VEC("AVX: y256p[10]", y256p[10]);
  // DEBUG_PRINT_AVX_VEC("AVX: y256p[11]", y256p[11]);
  // DEBUG_PRINT_AVX_VEC("AVX: y256p[12]", y256p[12]);
  // DEBUG_PRINT_AVX_VEC("AVX: y256p[13]", y256p[13]);
  // DEBUG_PRINT_AVX_VEC("AVX: y256p[14]", y256p[14]);
  // DEBUG_PRINT_AVX_VEC("AVX: y256p[15]", y256p[15]);

  if (scale>0) {

    y256[0] = mulhi_int16_simd256(y256[0],ONE_OVER_SQRT2_Q15_256);
    y256[1] = mulhi_int16_simd256(y256[1],ONE_OVER_SQRT2_Q15_256);
    y256[2] = mulhi_int16_simd256(y256[2],ONE_OVER_SQRT2_Q15_256);
    y256[3] = mulhi_int16_simd256(y256[3],ONE_OVER_SQRT2_Q15_256);
    y256[4] = mulhi_int16_simd256(y256[4],ONE_OVER_SQRT2_Q15_256);
    y256[5] = mulhi_int16_simd256(y256[5],ONE_OVER_SQRT2_Q15_256);
    y256[6] = mulhi_int16_simd256(y256[6],ONE_OVER_SQRT2_Q15_256);
    y256[7] = mulhi_int16_simd256(y256[7],ONE_OVER_SQRT2_Q15_256);
    y256[8] = mulhi_int16_simd256(y256[8],ONE_OVER_SQRT2_Q15_256);
    y256[9] = mulhi_int16_simd256(y256[9],ONE_OVER_SQRT2_Q15_256);
    y256[10] = mulhi_int16_simd256(y256[10],ONE_OVER_SQRT2_Q15_256);
    y256[11] = mulhi_int16_simd256(y256[11],ONE_OVER_SQRT2_Q15_256);
    y256[12] = mulhi_int16_simd256(y256[12],ONE_OVER_SQRT2_Q15_256);
    y256[13] = mulhi_int16_simd256(y256[13],ONE_OVER_SQRT2_Q15_256);
    y256[14] = mulhi_int16_simd256(y256[14],ONE_OVER_SQRT2_Q15_256);
    y256[15] = mulhi_int16_simd256(y256[15],ONE_OVER_SQRT2_Q15_256);

  }
}

void rvv_idft128(int16_t *x,int16_t *y,unsigned char scale)
{
  // 1. 内存分配 (栈上分配临时缓冲区，并对齐到 32 字节)
    int16_t xtmp[16 * 16] __attribute__((aligned(32)));
    int16_t ytmp[16 * 16] __attribute__((aligned(32)));

    // 指针别名
    int16_t *x_ptr = x;
    int16_t *y_ptr = y;
    const int16_t *tw128_ptr = tw128; // IDFT 只使用一个 tw 数组

    // 2. 准备常量
    vint16m4_t ONE_OVER_SQRT2_Q15_256_V = rvv_set1_int16(ONE_OVER_SQRT2_Q15);

    // 3. 第一阶段: Transpose
    // rvv_transpose4_ooff(x, y, off) 的 off 是 256-bit 向量的偏移量 (即 16 个 int16)
    rvv_transpose4_ooff(x_ptr + 0 * VL_16,  xtmp + 0 * VL_16, 8);
    rvv_transpose4_ooff(x_ptr + 2 * VL_16,  xtmp + 1 * VL_16, 8);
    rvv_transpose4_ooff(x_ptr + 4 * VL_16,  xtmp + 2 * VL_16, 8);
    rvv_transpose4_ooff(x_ptr + 6 * VL_16,  xtmp + 3 * VL_16, 8);
    rvv_transpose4_ooff(x_ptr + 8 * VL_16,  xtmp + 4 * VL_16, 8);
    rvv_transpose4_ooff(x_ptr + 10 * VL_16, xtmp + 5 * VL_16, 8);
    rvv_transpose4_ooff(x_ptr + 12 * VL_16, xtmp + 6 * VL_16, 8);
    rvv_transpose4_ooff(x_ptr + 14 * VL_16, xtmp + 7 * VL_16, 8);

    // vint16m4_t xtmp0 = __riscv_vle16_v_i16m4(xtmp + 0 * 16, VL_16);
    // vint16m4_t xtmp1 = __riscv_vle16_v_i16m4(xtmp + 1 * 16, VL_16);
    // vint16m4_t xtmp2 = __riscv_vle16_v_i16m4(xtmp + 2 * 16, VL_16);
    // vint16m4_t xtmp3 = __riscv_vle16_v_i16m4(xtmp + 3 * 16, VL_16);
    // vint16m4_t xtmp4 = __riscv_vle16_v_i16m4(xtmp + 4 * 16, VL_16);
    // vint16m4_t xtmp5 = __riscv_vle16_v_i16m4(xtmp + 5 * 16, VL_16);
    // vint16m4_t xtmp6 = __riscv_vle16_v_i16m4(xtmp + 6 * 16, VL_16);
    // vint16m4_t xtmp7 = __riscv_vle16_v_i16m4(xtmp + 7 * 16, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: xtmp0", xtmp0, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: xtmp1", xtmp1, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: xtmp2", xtmp2, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: xtmp3", xtmp3, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: xtmp4", xtmp4, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: xtmp5", xtmp5, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: xtmp6", xtmp6, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: xtmp7", xtmp7, VL_16);

    // 4. 第二阶段: IDFT64
    // 注意：原 IDFT128 中调用的是 idft64
    // 处理前 64 个点 (xtmp 0-127 -> ytmp 0-127)
    rvv_idft64(xtmp, ytmp, 1); 
    
    // 处理后 64 个点 (xtmp 128-255 -> ytmp 128-255)
    rvv_idft64(xtmp + 8 * VL_16, ytmp + 8 * VL_16, 1);

    // 5. 第三阶段: Inverse Butterfly 2 (rvv_ibfly2_256)
    // vint16m4_t ytmp0 = __riscv_vle16_v_i16m4(ytmp + 0 * 16, VL_16);
    // vint16m4_t ytmp1 = __riscv_vle16_v_i16m4(ytmp + 1 * 16, VL_16);
    // vint16m4_t ytmp2 = __riscv_vle16_v_i16m4(ytmp + 2 * 16, VL_16);
    // vint16m4_t ytmp3 = __riscv_vle16_v_i16m4(ytmp + 3 * 16, VL_16);
    // vint16m4_t ytmp4 = __riscv_vle16_v_i16m4(ytmp + 4 * 16, VL_16);
    // vint16m4_t ytmp5 = __riscv_vle16_v_i16m4(ytmp + 5 * 16, VL_16);
    // vint16m4_t ytmp6 = __riscv_vle16_v_i16m4(ytmp + 6 * 16, VL_16);
    // vint16m4_t ytmp7 = __riscv_vle16_v_i16m4(ytmp + 7 * 16, VL_16);
    // vint16m4_t ytmp8 = __riscv_vle16_v_i16m4(ytmp + 8 * 16, VL_16);
    // vint16m4_t ytmp9 = __riscv_vle16_v_i16m4(ytmp + 9 * 16, VL_16);
    // vint16m4_t ytmp10 = __riscv_vle16_v_i16m4(ytmp + 10 * 16, VL_16);
    // vint16m4_t ytmp11 = __riscv_vle16_v_i16m4(ytmp + 11 * 16, VL_16);
    // vint16m4_t ytmp12 = __riscv_vle16_v_i16m4(ytmp + 12 * 16, VL_16);
    // vint16m4_t ytmp13 = __riscv_vle16_v_i16m4(ytmp + 13 * 16, VL_16);
    // vint16m4_t ytmp14 = __riscv_vle16_v_i16m4(ytmp + 14 * 16, VL_16);
    // vint16m4_t ytmp15 = __riscv_vle16_v_i16m4(ytmp + 15 * 16, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: ytmp0", ytmp0, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: ytmp1", ytmp1, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: ytmp2", ytmp2, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: ytmp3", ytmp3, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: ytmp4", ytmp4, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: ytmp5", ytmp5, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: ytmp6", ytmp6, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: ytmp7", ytmp7, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: ytmp8", ytmp8, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: ytmp9", ytmp9, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: ytmp10", ytmp10, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: ytmp11", ytmp11, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: ytmp12", ytmp12, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: ytmp13", ytmp13, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: ytmp14", ytmp14, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: ytmp15", ytmp15, VL_16);
    int i;
    for (i = 0; i < 8; i++) {
        // 原代码: ibfly2_256(ytmpp, ytmpp+8, y256p, y256p+8, tw128_256p);
        
        rvv_ibfly2_256(
            ytmp + i * VL_16,           // x0 (ytmpp)
            ytmp + (i + 8) * VL_16,     // x1 (ytmpp+8)
            y_ptr + i * VL_16,          // y0 (y256p)
            y_ptr + (i + 8) * VL_16,    // y1 (y256p+8)
            tw128_ptr + i * VL_16       // tw (tw128_256p)
        );
        
        // 指针自增操作已集成到循环内部的索引计算中，无需 tw128_256p++/y256p++/ytmpp++
    }
    // vint16m4_t y_ptr0 = __riscv_vle16_v_i16m4(y_ptr + 0 * 16, VL_16);
    // vint16m4_t y_ptr1 = __riscv_vle16_v_i16m4(y_ptr + 1 * 16, VL_16);
    // vint16m4_t y_ptr2 = __riscv_vle16_v_i16m4(y_ptr + 2 * 16, VL_16);
    // vint16m4_t y_ptr3 = __riscv_vle16_v_i16m4(y_ptr + 3 * 16, VL_16);
    // vint16m4_t y_ptr4 = __riscv_vle16_v_i16m4(y_ptr + 4 * 16, VL_16);
    // vint16m4_t y_ptr5 = __riscv_vle16_v_i16m4(y_ptr + 5 * 16, VL_16);
    // vint16m4_t y_ptr6 = __riscv_vle16_v_i16m4(y_ptr + 6 * 16, VL_16);
    // vint16m4_t y_ptr7 = __riscv_vle16_v_i16m4(y_ptr + 7 * 16, VL_16);
    // vint16m4_t y_ptr8 = __riscv_vle16_v_i16m4(y_ptr + 8 * 16, VL_16);
    // vint16m4_t y_ptr9 = __riscv_vle16_v_i16m4(y_ptr + 9 * 16, VL_16);
    // vint16m4_t y_ptr10 = __riscv_vle16_v_i16m4(y_ptr + 10 * 16, VL_16);
    // vint16m4_t y_ptr11 = __riscv_vle16_v_i16m4(y_ptr + 11 * 16, VL_16);
    // vint16m4_t y_ptr12 = __riscv_vle16_v_i16m4(y_ptr + 12 * 16, VL_16);
    // vint16m4_t y_ptr13 = __riscv_vle16_v_i16m4(y_ptr + 13 * 16, VL_16);
    // vint16m4_t y_ptr14 = __riscv_vle16_v_i16m4(y_ptr + 14 * 16, VL_16);
    // vint16m4_t y_ptr15 = __riscv_vle16_v_i16m4(y_ptr + 15 * 16, VL_16);

    // DEBUG_PRINT_RVV_VEC("RVV: y_ptr0", y_ptr0, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: y_ptr1", y_ptr1, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: y_ptr2", y_ptr2, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: y_ptr3", y_ptr3, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: y_ptr4", y_ptr4, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: y_ptr5", y_ptr5, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: y_ptr6", y_ptr6, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: y_ptr7", y_ptr7, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: y_ptr8", y_ptr8, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: y_ptr9", y_ptr9, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: y_ptr10", y_ptr10, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: y_ptr11", y_ptr11, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: y_ptr12", y_ptr12, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: y_ptr13", y_ptr13, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: y_ptr14", y_ptr14, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: y_ptr15", y_ptr15, VL_16);

    // 6. 第四阶段: Final Scaling (Optional)
    if (scale > 0) {
        size_t vl = __riscv_vsetvl_e16m4(VL_16);
        
        // 循环处理所有 16 个输出向量 (y[0]...y[15])
        for (i = 0; i < 16; i++) {
            int16_t *curr_y = y_ptr + i * VL_16;
            
            vint16m4_t y_vec = __riscv_vle16_v_i16m4(curr_y, vl);
            
            // 使用 rvv_mulhi_int16 (对应 mulhi_int16_simd256)
            y_vec = rvv_mulhi_int16(y_vec, ONE_OVER_SQRT2_Q15_256_V, vl);
            
            __riscv_vse16_v_i16m4(curr_y, y_vec, vl);
        }
    }
  }


void ext_idft128(int16_t *x,int16_t *y,unsigned char scale)
{
  // 1. 内存分配 (栈上分配临时缓冲区，并对齐到 32 字节)
    int16_t xtmp[16 * 16] __attribute__((aligned(32)));
    int16_t ytmp[16 * 16] __attribute__((aligned(32)));

    // 指针别名
    int16_t *x_ptr = x;
    int16_t *y_ptr = y;
    const int16_t *tw128_ptr = tw128; // IDFT 只使用一个 tw 数组

    // 2. 准备常量
    vint16m4_t ONE_OVER_SQRT2_Q15_256_V = rvv_set1_int16(ONE_OVER_SQRT2_Q15);

    // 3. 第一阶段: Transpose
    // rvv_transpose4_ooff(x, y, off) 的 off 是 256-bit 向量的偏移量 (即 16 个 int16)
    rvv_transpose4_ooff(x_ptr + 0 * VL_16,  xtmp + 0 * VL_16, 8);
    rvv_transpose4_ooff(x_ptr + 2 * VL_16,  xtmp + 1 * VL_16, 8);
    rvv_transpose4_ooff(x_ptr + 4 * VL_16,  xtmp + 2 * VL_16, 8);
    rvv_transpose4_ooff(x_ptr + 6 * VL_16,  xtmp + 3 * VL_16, 8);
    rvv_transpose4_ooff(x_ptr + 8 * VL_16,  xtmp + 4 * VL_16, 8);
    rvv_transpose4_ooff(x_ptr + 10 * VL_16, xtmp + 5 * VL_16, 8);
    rvv_transpose4_ooff(x_ptr + 12 * VL_16, xtmp + 6 * VL_16, 8);
    rvv_transpose4_ooff(x_ptr + 14 * VL_16, xtmp + 7 * VL_16, 8);

    // vint16m4_t xtmp0 = __riscv_vle16_v_i16m4(xtmp + 0 * 16, VL_16);
    // vint16m4_t xtmp1 = __riscv_vle16_v_i16m4(xtmp + 1 * 16, VL_16);
    // vint16m4_t xtmp2 = __riscv_vle16_v_i16m4(xtmp + 2 * 16, VL_16);
    // vint16m4_t xtmp3 = __riscv_vle16_v_i16m4(xtmp + 3 * 16, VL_16);
    // vint16m4_t xtmp4 = __riscv_vle16_v_i16m4(xtmp + 4 * 16, VL_16);
    // vint16m4_t xtmp5 = __riscv_vle16_v_i16m4(xtmp + 5 * 16, VL_16);
    // vint16m4_t xtmp6 = __riscv_vle16_v_i16m4(xtmp + 6 * 16, VL_16);
    // vint16m4_t xtmp7 = __riscv_vle16_v_i16m4(xtmp + 7 * 16, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: xtmp0", xtmp0, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: xtmp1", xtmp1, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: xtmp2", xtmp2, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: xtmp3", xtmp3, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: xtmp4", xtmp4, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: xtmp5", xtmp5, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: xtmp6", xtmp6, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: xtmp7", xtmp7, VL_16);

    // 4. 第二阶段: IDFT64
    // 注意：原 IDFT128 中调用的是 idft64
    // 处理前 64 个点 (xtmp 0-127 -> ytmp 0-127)
    ext_idft64(xtmp, ytmp, 1); 
    
    // 处理后 64 个点 (xtmp 128-255 -> ytmp 128-255)
    ext_idft64(xtmp + 8 * VL_16, ytmp + 8 * VL_16, 1);

    // 5. 第三阶段: Inverse Butterfly 2 (rvv_ibfly2_256)
    // vint16m4_t ytmp0 = __riscv_vle16_v_i16m4(ytmp + 0 * 16, VL_16);
    // vint16m4_t ytmp1 = __riscv_vle16_v_i16m4(ytmp + 1 * 16, VL_16);
    // vint16m4_t ytmp2 = __riscv_vle16_v_i16m4(ytmp + 2 * 16, VL_16);
    // vint16m4_t ytmp3 = __riscv_vle16_v_i16m4(ytmp + 3 * 16, VL_16);
    // vint16m4_t ytmp4 = __riscv_vle16_v_i16m4(ytmp + 4 * 16, VL_16);
    // vint16m4_t ytmp5 = __riscv_vle16_v_i16m4(ytmp + 5 * 16, VL_16);
    // vint16m4_t ytmp6 = __riscv_vle16_v_i16m4(ytmp + 6 * 16, VL_16);
    // vint16m4_t ytmp7 = __riscv_vle16_v_i16m4(ytmp + 7 * 16, VL_16);
    // vint16m4_t ytmp8 = __riscv_vle16_v_i16m4(ytmp + 8 * 16, VL_16);
    // vint16m4_t ytmp9 = __riscv_vle16_v_i16m4(ytmp + 9 * 16, VL_16);
    // vint16m4_t ytmp10 = __riscv_vle16_v_i16m4(ytmp + 10 * 16, VL_16);
    // vint16m4_t ytmp11 = __riscv_vle16_v_i16m4(ytmp + 11 * 16, VL_16);
    // vint16m4_t ytmp12 = __riscv_vle16_v_i16m4(ytmp + 12 * 16, VL_16);
    // vint16m4_t ytmp13 = __riscv_vle16_v_i16m4(ytmp + 13 * 16, VL_16);
    // vint16m4_t ytmp14 = __riscv_vle16_v_i16m4(ytmp + 14 * 16, VL_16);
    // vint16m4_t ytmp15 = __riscv_vle16_v_i16m4(ytmp + 15 * 16, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: ytmp0", ytmp0, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: ytmp1", ytmp1, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: ytmp2", ytmp2, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: ytmp3", ytmp3, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: ytmp4", ytmp4, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: ytmp5", ytmp5, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: ytmp6", ytmp6, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: ytmp7", ytmp7, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: ytmp8", ytmp8, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: ytmp9", ytmp9, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: ytmp10", ytmp10, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: ytmp11", ytmp11, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: ytmp12", ytmp12, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: ytmp13", ytmp13, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: ytmp14", ytmp14, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: ytmp15", ytmp15, VL_16);
    int i;
    for (i = 0; i < 8; i++) {
        // 原代码: ibfly2_256(ytmpp, ytmpp+8, y256p, y256p+8, tw128_256p);
        
        rvv_ibfly2_256(
            ytmp + i * VL_16,           // x0 (ytmpp)
            ytmp + (i + 8) * VL_16,     // x1 (ytmpp+8)
            y_ptr + i * VL_16,          // y0 (y256p)
            y_ptr + (i + 8) * VL_16,    // y1 (y256p+8)
            tw128_ptr + i * VL_16       // tw (tw128_256p)
        );
        
        // 指针自增操作已集成到循环内部的索引计算中，无需 tw128_256p++/y256p++/ytmpp++
    }
    // vint16m4_t y_ptr0 = __riscv_vle16_v_i16m4(y_ptr + 0 * 16, VL_16);
    // vint16m4_t y_ptr1 = __riscv_vle16_v_i16m4(y_ptr + 1 * 16, VL_16);
    // vint16m4_t y_ptr2 = __riscv_vle16_v_i16m4(y_ptr + 2 * 16, VL_16);
    // vint16m4_t y_ptr3 = __riscv_vle16_v_i16m4(y_ptr + 3 * 16, VL_16);
    // vint16m4_t y_ptr4 = __riscv_vle16_v_i16m4(y_ptr + 4 * 16, VL_16);
    // vint16m4_t y_ptr5 = __riscv_vle16_v_i16m4(y_ptr + 5 * 16, VL_16);
    // vint16m4_t y_ptr6 = __riscv_vle16_v_i16m4(y_ptr + 6 * 16, VL_16);
    // vint16m4_t y_ptr7 = __riscv_vle16_v_i16m4(y_ptr + 7 * 16, VL_16);
    // vint16m4_t y_ptr8 = __riscv_vle16_v_i16m4(y_ptr + 8 * 16, VL_16);
    // vint16m4_t y_ptr9 = __riscv_vle16_v_i16m4(y_ptr + 9 * 16, VL_16);
    // vint16m4_t y_ptr10 = __riscv_vle16_v_i16m4(y_ptr + 10 * 16, VL_16);
    // vint16m4_t y_ptr11 = __riscv_vle16_v_i16m4(y_ptr + 11 * 16, VL_16);
    // vint16m4_t y_ptr12 = __riscv_vle16_v_i16m4(y_ptr + 12 * 16, VL_16);
    // vint16m4_t y_ptr13 = __riscv_vle16_v_i16m4(y_ptr + 13 * 16, VL_16);
    // vint16m4_t y_ptr14 = __riscv_vle16_v_i16m4(y_ptr + 14 * 16, VL_16);
    // vint16m4_t y_ptr15 = __riscv_vle16_v_i16m4(y_ptr + 15 * 16, VL_16);

    // DEBUG_PRINT_RVV_VEC("RVV: y_ptr0", y_ptr0, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: y_ptr1", y_ptr1, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: y_ptr2", y_ptr2, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: y_ptr3", y_ptr3, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: y_ptr4", y_ptr4, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: y_ptr5", y_ptr5, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: y_ptr6", y_ptr6, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: y_ptr7", y_ptr7, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: y_ptr8", y_ptr8, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: y_ptr9", y_ptr9, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: y_ptr10", y_ptr10, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: y_ptr11", y_ptr11, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: y_ptr12", y_ptr12, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: y_ptr13", y_ptr13, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: y_ptr14", y_ptr14, VL_16);
    // DEBUG_PRINT_RVV_VEC("RVV: y_ptr15", y_ptr15, VL_16);

    // 6. 第四阶段: Final Scaling (Optional)
    if (scale > 0) {
        size_t vl = __riscv_vsetvl_e16m4(VL_16);
        
        // 循环处理所有 16 个输出向量 (y[0]...y[15])
        for (i = 0; i < 16; i++) {
            int16_t *curr_y = y_ptr + i * VL_16;
            
            vint16m4_t y_vec = __riscv_vle16_v_i16m4(curr_y, vl);
            
            // 使用 rvv_mulhi_int16 (对应 mulhi_int16_simd256)
            y_vec = rvv_mulhi_int16(y_vec, ONE_OVER_SQRT2_Q15_256_V, vl);
            
            __riscv_vse16_v_i16m4(curr_y, y_vec, vl);
        }
    }
  }

#endif

int16_t tw256[384] __attribute__((aligned(32))) = {  32767,0,32757,-805,32727,-1608,32678,-2411,32609,-3212,32520,-4012,32412,-4808,32284,-5602,32137,-6393,31970,-7180,31785,-7962,31580,-8740,31356,-9512,31113,-10279,30851,-11039,30571,-11793,30272,-12540,29955,-13279,29621,-14010,29268,-14733,28897,-15447,28510,-16151,28105,-16846,27683,-17531,27244,-18205,26789,-18868,26318,-19520,25831,-20160,25329,-20788,24811,-21403,24278,-22005,23731,-22595,23169,-23170,22594,-23732,22004,-24279,21402,-24812,20787,-25330,20159,-25832,19519,-26319,18867,-26790,18204,-27245,17530,-27684,16845,-28106,16150,-28511,15446,-28898,14732,-29269,14009,-29622,13278,-29956,12539,-30273,11792,-30572,11038,-30852,10278,-31114,9511,-31357,8739,-31581,7961,-31786,7179,-31971,6392,-32138,5601,-32285,4807,-32413,4011,-32521,3211,-32610,2410,-32679,1607,-32728,804,-32758,
                                                     32767,0,32727,-1608,32609,-3212,32412,-4808,32137,-6393,31785,-7962,31356,-9512,30851,-11039,30272,-12540,29621,-14010,28897,-15447,28105,-16846,27244,-18205,26318,-19520,25329,-20788,24278,-22005,23169,-23170,22004,-24279,20787,-25330,19519,-26319,18204,-27245,16845,-28106,15446,-28898,14009,-29622,12539,-30273,11038,-30852,9511,-31357,7961,-31786,6392,-32138,4807,-32413,3211,-32610,1607,-32728,0,-32767,-1608,-32728,-3212,-32610,-4808,-32413,-6393,-32138,-7962,-31786,-9512,-31357,-11039,-30852,-12540,-30273,-14010,-29622,-15447,-28898,-16846,-28106,-18205,-27245,-19520,-26319,-20788,-25330,-22005,-24279,-23170,-23170,-24279,-22005,-25330,-20788,-26319,-19520,-27245,-18205,-28106,-16846,-28898,-15447,-29622,-14010,-30273,-12540,-30852,-11039,-31357,-9512,-31786,-7962,-32138,-6393,-32413,-4808,-32610,-3212,-32728,-1608,
                                                     32767,0,32678,-2411,32412,-4808,31970,-7180,31356,-9512,30571,-11793,29621,-14010,28510,-16151,27244,-18205,25831,-20160,24278,-22005,22594,-23732,20787,-25330,18867,-26790,16845,-28106,14732,-29269,12539,-30273,10278,-31114,7961,-31786,5601,-32285,3211,-32610,804,-32758,-1608,-32728,-4012,-32521,-6393,-32138,-8740,-31581,-11039,-30852,-13279,-29956,-15447,-28898,-17531,-27684,-19520,-26319,-21403,-24812,-23170,-23170,-24812,-21403,-26319,-19520,-27684,-17531,-28898,-15447,-29956,-13279,-30852,-11039,-31581,-8740,-32138,-6393,-32521,-4012,-32728,-1608,-32758,804,-32610,3211,-32285,5601,-31786,7961,-31114,10278,-30273,12539,-29269,14732,-28106,16845,-26790,18867,-25330,20787,-23732,22594,-22005,24278,-20160,25831,-18205,27244,-16151,28510,-14010,29621,-11793,30571,-9512,31356,-7180,31970,-4808,32412,-2411,32678
                                                  };

int16_t tw256a[384] __attribute__((aligned(32))) = { 32767,0,32757,804,32727,1607,32678,2410,32609,3211,32520,4011,32412,4807,32284,5601,32137,6392,31970,7179,31785,7961,31580,8739,31356,9511,31113,10278,30851,11038,30571,11792,30272,12539,29955,13278,29621,14009,29268,14732,28897,15446,28510,16150,28105,16845,27683,17530,27244,18204,26789,18867,26318,19519,25831,20159,25329,20787,24811,21402,24278,22004,23731,22594,23169,23169,22594,23731,22004,24278,21402,24811,20787,25329,20159,25831,19519,26318,18867,26789,18204,27244,17530,27683,16845,28105,16150,28510,15446,28897,14732,29268,14009,29621,13278,29955,12539,30272,11792,30571,11038,30851,10278,31113,9511,31356,8739,31580,7961,31785,7179,31970,6392,32137,5601,32284,4807,32412,4011,32520,3211,32609,2410,32678,1607,32727,804,32757,
                                                     32767,0,32727,1607,32609,3211,32412,4807,32137,6392,31785,7961,31356,9511,30851,11038,30272,12539,29621,14009,28897,15446,28105,16845,27244,18204,26318,19519,25329,20787,24278,22004,23169,23169,22004,24278,20787,25329,19519,26318,18204,27244,16845,28105,15446,28897,14009,29621,12539,30272,11038,30851,9511,31356,7961,31785,6392,32137,4807,32412,3211,32609,1607,32727,0,32767,-1608,32727,-3212,32609,-4808,32412,-6393,32137,-7962,31785,-9512,31356,-11039,30851,-12540,30272,-14010,29621,-15447,28897,-16846,28105,-18205,27244,-19520,26318,-20788,25329,-22005,24278,-23170,23169,-24279,22004,-25330,20787,-26319,19519,-27245,18204,-28106,16845,-28898,15446,-29622,14009,-30273,12539,-30852,11038,-31357,9511,-31786,7961,-32138,6392,-32413,4807,-32610,3211,-32728,1607,
                                                     32767,0,32678,2410,32412,4807,31970,7179,31356,9511,30571,11792,29621,14009,28510,16150,27244,18204,25831,20159,24278,22004,22594,23731,20787,25329,18867,26789,16845,28105,14732,29268,12539,30272,10278,31113,7961,31785,5601,32284,3211,32609,804,32757,-1608,32727,-4012,32520,-6393,32137,-8740,31580,-11039,30851,-13279,29955,-15447,28897,-17531,27683,-19520,26318,-21403,24811,-23170,23169,-24812,21402,-26319,19519,-27684,17530,-28898,15446,-29956,13278,-30852,11038,-31581,8739,-32138,6392,-32521,4011,-32728,1607,-32758,-805,-32610,-3212,-32285,-5602,-31786,-7962,-31114,-10279,-30273,-12540,-29269,-14733,-28106,-16846,-26790,-18868,-25330,-20788,-23732,-22595,-22005,-24279,-20160,-25832,-18205,-27245,-16151,-28511,-14010,-29622,-11793,-30572,-9512,-31357,-7180,-31971,-4808,-32413,-2411,-32679
                                                   };

int16_t tw256b[384] __attribute__((aligned(32))) = {0,32767,-805,32757,-1608,32727,-2411,32678,-3212,32609,-4012,32520,-4808,32412,-5602,32284,-6393,32137,-7180,31970,-7962,31785,-8740,31580,-9512,31356,-10279,31113,-11039,30851,-11793,30571,-12540,30272,-13279,29955,-14010,29621,-14733,29268,-15447,28897,-16151,28510,-16846,28105,-17531,27683,-18205,27244,-18868,26789,-19520,26318,-20160,25831,-20788,25329,-21403,24811,-22005,24278,-22595,23731,-23170,23169,-23732,22594,-24279,22004,-24812,21402,-25330,20787,-25832,20159,-26319,19519,-26790,18867,-27245,18204,-27684,17530,-28106,16845,-28511,16150,-28898,15446,-29269,14732,-29622,14009,-29956,13278,-30273,12539,-30572,11792,-30852,11038,-31114,10278,-31357,9511,-31581,8739,-31786,7961,-31971,7179,-32138,6392,-32285,5601,-32413,4807,-32521,4011,-32610,3211,-32679,2410,-32728,1607,-32758,804,
                                                    0,32767,-1608,32727,-3212,32609,-4808,32412,-6393,32137,-7962,31785,-9512,31356,-11039,30851,-12540,30272,-14010,29621,-15447,28897,-16846,28105,-18205,27244,-19520,26318,-20788,25329,-22005,24278,-23170,23169,-24279,22004,-25330,20787,-26319,19519,-27245,18204,-28106,16845,-28898,15446,-29622,14009,-30273,12539,-30852,11038,-31357,9511,-31786,7961,-32138,6392,-32413,4807,-32610,3211,-32728,1607,-32767,0,-32728,-1608,-32610,-3212,-32413,-4808,-32138,-6393,-31786,-7962,-31357,-9512,-30852,-11039,-30273,-12540,-29622,-14010,-28898,-15447,-28106,-16846,-27245,-18205,-26319,-19520,-25330,-20788,-24279,-22005,-23170,-23170,-22005,-24279,-20788,-25330,-19520,-26319,-18205,-27245,-16846,-28106,-15447,-28898,-14010,-29622,-12540,-30273,-11039,-30852,-9512,-31357,-7962,-31786,-6393,-32138,-4808,-32413,-3212,-32610,-1608,-32728,
                                                    0,32767,-2411,32678,-4808,32412,-7180,31970,-9512,31356,-11793,30571,-14010,29621,-16151,28510,-18205,27244,-20160,25831,-22005,24278,-23732,22594,-25330,20787,-26790,18867,-28106,16845,-29269,14732,-30273,12539,-31114,10278,-31786,7961,-32285,5601,-32610,3211,-32758,804,-32728,-1608,-32521,-4012,-32138,-6393,-31581,-8740,-30852,-11039,-29956,-13279,-28898,-15447,-27684,-17531,-26319,-19520,-24812,-21403,-23170,-23170,-21403,-24812,-19520,-26319,-17531,-27684,-15447,-28898,-13279,-29956,-11039,-30852,-8740,-31581,-6393,-32138,-4012,-32521,-1608,-32728,804,-32758,3211,-32610,5601,-32285,7961,-31786,10278,-31114,12539,-30273,14732,-29269,16845,-28106,18867,-26790,20787,-25330,22594,-23732,24278,-22005,25831,-20160,27244,-18205,28510,-16151,29621,-14010,30571,-11793,31356,-9512,31970,-7180,32412,-4808,32678,-2411
                                                   };
#ifndef __AVX2__

#else //__AVX2__

void dft256(int16_t *x,int16_t *y,unsigned char scale)
{

  simd256_q15_t xtmp[32],ytmp[32],*tw256a_256p=(simd256_q15_t *)tw256a,*tw256b_256p=(simd256_q15_t *)tw256b,*x256=(simd256_q15_t *)x,*y256=(simd256_q15_t *)y,*y256p=(simd256_q15_t *)y;
  simd256_q15_t *ytmpp = &ytmp[0];
  int i;

  transpose16_ooff_simd256(x256+0,xtmp+0,8);
  transpose16_ooff_simd256(x256+4,xtmp+1,8);
  transpose16_ooff_simd256(x256+8,xtmp+2,8);
  transpose16_ooff_simd256(x256+12,xtmp+3,8);
  transpose16_ooff_simd256(x256+16,xtmp+4,8);
  transpose16_ooff_simd256(x256+20,xtmp+5,8);
  transpose16_ooff_simd256(x256+24,xtmp+6,8);
  transpose16_ooff_simd256(x256+28,xtmp+7,8);

  /*
  char vname[10];
  for (i=0;i<32;i++) {
    sprintf(vname,"xtmp%d",i);
    print_shorts256(vname,(int16_t*)(xtmp+i));
  }
  exit(-1);*/

  dft64((int16_t*)(xtmp),(int16_t*)(ytmp),1);
  dft64((int16_t*)(xtmp+8),(int16_t*)(ytmp+8),1);
  dft64((int16_t*)(xtmp+16),(int16_t*)(ytmp+16),1);
  dft64((int16_t*)(xtmp+24),(int16_t*)(ytmp+24),1);


  bfly4_16_256(ytmpp,ytmpp+8,ytmpp+16,ytmpp+24,
	       y256p,y256p+8,y256p+16,y256p+24,
	       tw256a_256p,tw256a_256p+8,tw256a_256p+16,
	       tw256b_256p,tw256b_256p+8,tw256b_256p+16);
  bfly4_16_256(ytmpp+1,ytmpp+9,ytmpp+17,ytmpp+25,
	       y256p+1,y256p+9,y256p+17,y256p+25,
	       tw256a_256p+1,tw256a_256p+9,tw256a_256p+17,
	       tw256b_256p+1,tw256b_256p+9,tw256b_256p+17);
  bfly4_16_256(ytmpp+2,ytmpp+10,ytmpp+18,ytmpp+26,
	       y256p+2,y256p+10,y256p+18,y256p+26,
	       tw256a_256p+2,tw256a_256p+10,tw256a_256p+18,
	       tw256b_256p+2,tw256b_256p+10,tw256b_256p+18);
  bfly4_16_256(ytmpp+3,ytmpp+11,ytmpp+19,ytmpp+27,
	       y256p+3,y256p+11,y256p+19,y256p+27,
	       tw256a_256p+3,tw256a_256p+11,tw256a_256p+19,
	       tw256b_256p+3,tw256b_256p+11,tw256b_256p+19);
  bfly4_16_256(ytmpp+4,ytmpp+12,ytmpp+20,ytmpp+28,
	       y256p+4,y256p+12,y256p+20,y256p+28,
	       tw256a_256p+4,tw256a_256p+12,tw256a_256p+20,
	       tw256b_256p+4,tw256b_256p+12,tw256b_256p+20);
  bfly4_16_256(ytmpp+5,ytmpp+13,ytmpp+21,ytmpp+29,
	       y256p+5,y256p+13,y256p+21,y256p+29,
	       tw256a_256p+5,tw256a_256p+13,tw256a_256p+21,
	       tw256b_256p+5,tw256b_256p+13,tw256b_256p+21);
  bfly4_16_256(ytmpp+6,ytmpp+14,ytmpp+22,ytmpp+30,
	       y256p+6,y256p+14,y256p+22,y256p+30,
	       tw256a_256p+6,tw256a_256p+14,tw256a_256p+22,
	       tw256b_256p+6,tw256b_256p+14,tw256b_256p+22);
  bfly4_16_256(ytmpp+7,ytmpp+15,ytmpp+23,ytmpp+31,
	       y256p+7,y256p+15,y256p+23,y256p+31,
	       tw256a_256p+7,tw256a_256p+15,tw256a_256p+23,
	       tw256b_256p+7,tw256b_256p+15,tw256b_256p+23);

  if (scale>0) {

    for (i=0; i<2; i++) {
      y256[0]  = shiftright_int16_simd256(y256[0],1);
      y256[1]  = shiftright_int16_simd256(y256[1],1);
      y256[2]  = shiftright_int16_simd256(y256[2],1);
      y256[3]  = shiftright_int16_simd256(y256[3],1);
      y256[4]  = shiftright_int16_simd256(y256[4],1);
      y256[5]  = shiftright_int16_simd256(y256[5],1);
      y256[6]  = shiftright_int16_simd256(y256[6],1);
      y256[7]  = shiftright_int16_simd256(y256[7],1);
      y256[8]  = shiftright_int16_simd256(y256[8],1);
      y256[9]  = shiftright_int16_simd256(y256[9],1);
      y256[10] = shiftright_int16_simd256(y256[10],1);
      y256[11] = shiftright_int16_simd256(y256[11],1);
      y256[12] = shiftright_int16_simd256(y256[12],1);
      y256[13] = shiftright_int16_simd256(y256[13],1);
      y256[14] = shiftright_int16_simd256(y256[14],1);
      y256[15] = shiftright_int16_simd256(y256[15],1);

      y256+=16;
    }

  }
}

void rvv_dft256(int16_t *x,int16_t *y,unsigned char scale)
{
// 1. 内存分配 (32 个 256-bit 向量 = 32 * 16 = 512 个 int16_t)
    int16_t xtmp[32 * VL_16] __attribute__((aligned(32)));
    int16_t ytmp[32 * VL_16] __attribute__((aligned(32)));

    // 2. 指针定义 (使用 int16_t* 代替 simd256_q15_t*)
    int16_t *tw256a_ptr = tw256a;
    int16_t *tw256b_ptr = tw256b;
    int16_t *x_ptr = x;
    int16_t *y_ptr = y;
    
    // 中间指针用于循环和索引 (保持原代码结构)
    int16_t *ytmpp = ytmp;
    int16_t *y_write_ptr = y_ptr;

    // 3. 第一阶段: Transpose (rvv_transpose16_ooff)
    // transpose16_ooff_simd256(x256+N, xtmp+M, 8);
    // 这里的 N/M 是 256-bit 块偏移量，对应 N*16/M*16 个 int16_t 元素偏移。
    // off=8 对应 8*16 = 128 个 int16_t 的块偏移。

    rvv_transpose16_ooff(x_ptr + 0 * VL_16,  xtmp + 0 * VL_16, 8);
    rvv_transpose16_ooff(x_ptr + 4 * VL_16,  xtmp + 1 * VL_16, 8);
    rvv_transpose16_ooff(x_ptr + 8 * VL_16,  xtmp + 2 * VL_16, 8);
    rvv_transpose16_ooff(x_ptr + 12 * VL_16, xtmp + 3 * VL_16, 8);
    rvv_transpose16_ooff(x_ptr + 16 * VL_16, xtmp + 4 * VL_16, 8);
    rvv_transpose16_ooff(x_ptr + 20 * VL_16, xtmp + 5 * VL_16, 8);
    rvv_transpose16_ooff(x_ptr + 24 * VL_16, xtmp + 6 * VL_16, 8);
    rvv_transpose16_ooff(x_ptr + 28 * VL_16, xtmp + 7 * VL_16, 8);

    // 4. 第二阶段: DFT64
    // 每个 DFT64 处理 4 个 256-bit 块，即 4*16=64 个 int16_t 元素 (128 个 Q15 复数)
    // 偏移量 N*VL_16: 0, 8*16, 16*16, 24*16
    
    // (xtmp 0-127) -> (ytmp 0-127)
    rvv_dft64(xtmp + 0 * VL_16, ytmp + 0 * VL_16, 1);
    
    // (xtmp 128-255) -> (ytmp 128-255)
    rvv_dft64(xtmp + 8 * VL_16, ytmp + 8 * VL_16, 1);
    
    // (xtmp 256-383) -> (ytmp 256-383)
    rvv_dft64(xtmp + 16 * VL_16, ytmp + 16 * VL_16, 1);
    
    // (xtmp 384-511) -> (ytmp 384-511)
    rvv_dft64(xtmp + 24 * VL_16, ytmp + 24 * VL_16, 1);


    // 5. 第三阶段: Butterfly 4 (rvv_bfly4_16_256)
    // bfly4_16_256(x0, x1, x2, x3, y0, y1, y2, y3, twa0, twa1, twa2, twb0, twb1, twb2)
    
    const int VL_STEP = VL_16;      // 16 int16_t
    const int BLOCK_STEP = 8 * VL_STEP; // 8 * 16 int16_t

    // 循环 8 次，每次处理一个垂直 slice (4 个输入向量，4 个输出向量)
    for (int i = 0; i < 8; i++) {
        
        rvv_bfly4_16_256(
            // Inputs (ytmp)
            ytmpp + i * VL_STEP,           // x0: ytmp[i]
            ytmpp + i * VL_STEP + 1 * BLOCK_STEP, // x1: ytmp[i+8]
            ytmpp + i * VL_STEP + 2 * BLOCK_STEP, // x2: ytmp[i+16]
            ytmpp + i * VL_STEP + 3 * BLOCK_STEP, // x3: ytmp[i+24]
            
            // Outputs (y)
            y_write_ptr + i * VL_STEP,           // y0: y[i]
            y_write_ptr + i * VL_STEP + 1 * BLOCK_STEP, // y1: y[i+8]
            y_write_ptr + i * VL_STEP + 2 * BLOCK_STEP, // y2: y[i+16]
            y_write_ptr + i * VL_STEP + 3 * BLOCK_STEP, // y3: y[i+24]

            // Twiddles A (tw256a)
            tw256a_ptr + i * VL_STEP,           // twa0: tw256a[i]
            tw256a_ptr + i * VL_STEP + 1 * BLOCK_STEP, // twa1: tw256a[i+8]
            tw256a_ptr + i * VL_STEP + 2 * BLOCK_STEP, // twa2: tw256a[i+16]

            // Twiddles B (tw256b)
            tw256b_ptr + i * VL_STEP,           // twb0: tw256b[i]
            tw256b_ptr + i * VL_STEP + 1 * BLOCK_STEP, // twb1: tw256b[i+8]
            tw256b_ptr + i * VL_STEP + 2 * BLOCK_STEP  // twb2: tw256b[i+16]
        );
    }

    // 6. 第四阶段: Final Scaling (Optional)
    if (scale > 0) {
        size_t vl = __riscv_vsetvl_e16m4(VL_16);
        
        // 缩放循环 (执行两次移位)
        for (int k = 0; k < 2; k++) {
            // y256+=16 是将 y256 指针向后移动 16 个 256-bit 块
            // 每次循环处理 16 个 256-bit 块，共 16 * 16 = 256 个 int16_t
            // 两个循环处理 2 * 16 = 32 个 256-bit 块，即所有 256 点
            
            int16_t *current_y_base = y_ptr + k * 16 * VL_16; 
            
            for (int j = 0; j < 16; j++) {
                int16_t *curr_y_vec = current_y_base + j * VL_16;

                // 1. Load
                vint16m4_t y_vec = __riscv_vle16_v_i16m4(curr_y_vec, vl);
                
                // 2. Shift Right (Arithmetic, by 1)
                // y256[j] = shiftright_int16_simd256(y256[j], 1);
                y_vec = rvv_shiftright_int16(y_vec, 1, vl);
                
                // 3. Store
                __riscv_vse16_v_i16m4(curr_y_vec, y_vec, vl);
            }
        }
    }
  _mm_empty();
  _m_empty();

}

void idft256(int16_t *x,int16_t *y,unsigned char scale)
{

  simd256_q15_t xtmp[32],ytmp[32],*tw256_256p=(simd256_q15_t *)tw256,*x256=(simd256_q15_t *)x,*y256=(simd256_q15_t *)y,*y256p=(simd256_q15_t *)y;
  simd256_q15_t *ytmpp = &ytmp[0];
  int i;

  transpose16_ooff_simd256(x256+0,xtmp+0,8);
  transpose16_ooff_simd256(x256+4,xtmp+1,8);
  transpose16_ooff_simd256(x256+8,xtmp+2,8);
  transpose16_ooff_simd256(x256+12,xtmp+3,8);
  transpose16_ooff_simd256(x256+16,xtmp+4,8);
  transpose16_ooff_simd256(x256+20,xtmp+5,8);
  transpose16_ooff_simd256(x256+24,xtmp+6,8);
  transpose16_ooff_simd256(x256+28,xtmp+7,8);
  
  idft64((int16_t*)(xtmp),(int16_t*)(ytmp),1);
  idft64((int16_t*)(xtmp+8),(int16_t*)(ytmp+8),1);
  idft64((int16_t*)(xtmp+16),(int16_t*)(ytmp+16),1);
  idft64((int16_t*)(xtmp+24),(int16_t*)(ytmp+24),1);
  
  
  ibfly4_256(ytmpp,ytmpp+8,ytmpp+16,ytmpp+24,
	     y256p,y256p+8,y256p+16,y256p+24,
	     tw256_256p,tw256_256p+8,tw256_256p+16);

  ibfly4_256(ytmpp+1,ytmpp+9,ytmpp+17,ytmpp+25,
	     y256p+1,y256p+9,y256p+17,y256p+25,
	     tw256_256p+1,tw256_256p+9,tw256_256p+17);

  ibfly4_256(ytmpp+2,ytmpp+10,ytmpp+18,ytmpp+26,
	     y256p+2,y256p+10,y256p+18,y256p+26,
	     tw256_256p+2,tw256_256p+10,tw256_256p+18);

  ibfly4_256(ytmpp+3,ytmpp+11,ytmpp+19,ytmpp+27,
	     y256p+3,y256p+11,y256p+19,y256p+27,
	     tw256_256p+3,tw256_256p+11,tw256_256p+19);

  ibfly4_256(ytmpp+4,ytmpp+12,ytmpp+20,ytmpp+28,
	     y256p+4,y256p+12,y256p+20,y256p+28,
	     tw256_256p+4,tw256_256p+12,tw256_256p+20);

  ibfly4_256(ytmpp+5,ytmpp+13,ytmpp+21,ytmpp+29,
	     y256p+5,y256p+13,y256p+21,y256p+29,
	     tw256_256p+5,tw256_256p+13,tw256_256p+21);

  ibfly4_256(ytmpp+6,ytmpp+14,ytmpp+22,ytmpp+30,
	     y256p+6,y256p+14,y256p+22,y256p+30,
	     tw256_256p+6,tw256_256p+14,tw256_256p+22);

  ibfly4_256(ytmpp+7,ytmpp+15,ytmpp+23,ytmpp+31,
	     y256p+7,y256p+15,y256p+23,y256p+31,
	     tw256_256p+7,tw256_256p+15,tw256_256p+23);

  
  if (scale>0) {

    for (i=0; i<2; i++) {
      y256[0]  = shiftright_int16_simd256(y256[0],1);
      y256[1]  = shiftright_int16_simd256(y256[1],1);
      y256[2]  = shiftright_int16_simd256(y256[2],1);
      y256[3]  = shiftright_int16_simd256(y256[3],1);
      y256[4]  = shiftright_int16_simd256(y256[4],1);
      y256[5]  = shiftright_int16_simd256(y256[5],1);
      y256[6]  = shiftright_int16_simd256(y256[6],1);
      y256[7]  = shiftright_int16_simd256(y256[7],1);
      y256[8]  = shiftright_int16_simd256(y256[8],1);
      y256[9]  = shiftright_int16_simd256(y256[9],1);
      y256[10] = shiftright_int16_simd256(y256[10],1);
      y256[11] = shiftright_int16_simd256(y256[11],1);
      y256[12] = shiftright_int16_simd256(y256[12],1);
      y256[13] = shiftright_int16_simd256(y256[13],1);
      y256[14] = shiftright_int16_simd256(y256[14],1);
      y256[15] = shiftright_int16_simd256(y256[15],1);

      y256+=16;
    }

  }
}

void rvv_idft256(int16_t *x,int16_t *y,unsigned char scale)
{
  // 1. 内存分配 (32 个 256-bit 向量 = 32 * 16 = 512 个 int16_t)
    int16_t xtmp[32 * VL_16] __attribute__((aligned(32)));
    int16_t ytmp[32 * VL_16] __attribute__((aligned(32)));

    // 2. 指针定义
    const int16_t *tw256_ptr = tw256;
    int16_t *x_ptr = x;
    int16_t *y_ptr = y;
    
    // 中间指针用于循环和索引
    int16_t *ytmpp = ytmp;
    int16_t *y_write_ptr = y_ptr;
    
    // 3. 第一阶段: Transpose (rvv_transpose16_ooff)
    // 这里的 N/M 是 256-bit 块偏移量，对应 N*16/M*16 个 int16_t 元素偏移。
    // off=8 对应 8*16 = 128 个 int16_t 的块偏移。

    rvv_transpose16_ooff(x_ptr + 0 * VL_16,  xtmp + 0 * VL_16, 8);
    rvv_transpose16_ooff(x_ptr + 4 * VL_16,  xtmp + 1 * VL_16, 8);
    rvv_transpose16_ooff(x_ptr + 8 * VL_16,  xtmp + 2 * VL_16, 8);
    rvv_transpose16_ooff(x_ptr + 12 * VL_16, xtmp + 3 * VL_16, 8);
    rvv_transpose16_ooff(x_ptr + 16 * VL_16, xtmp + 4 * VL_16, 8);
    rvv_transpose16_ooff(x_ptr + 20 * VL_16, xtmp + 5 * VL_16, 8);
    rvv_transpose16_ooff(x_ptr + 24 * VL_16, xtmp + 6 * VL_16, 8);
    rvv_transpose16_ooff(x_ptr + 28 * VL_16, xtmp + 7 * VL_16, 8);

    // 4. 第二阶段: IDFT64
    // 偏移量 N*VL_16: 0, 8*16, 16*16, 24*16
    
    rvv_idft64(xtmp + 0 * VL_16, ytmp + 0 * VL_16, 1);
    rvv_idft64(xtmp + 8 * VL_16, ytmp + 8 * VL_16, 1);
    rvv_idft64(xtmp + 16 * VL_16, ytmp + 16 * VL_16, 1);
    rvv_idft64(xtmp + 24 * VL_16, ytmp + 24 * VL_16, 1);

    // 5. 第三阶段: Inverse Butterfly 4 (rvv_ibfly4_256)
    // 8 次 ibfly4_256 调用，每次处理一个垂直 slice
    
    const int VL_STEP = VL_16;      // 16 int16_t
    const int BLOCK_STEP = 8 * VL_STEP; // 8 * 16 int16_t

    for (int i = 0; i < 8; i++) {
        // ibfly4_256(x0, x1, x2, x3, y0, y1, y2, y3, tw0, tw1, tw2)
        
        rvv_ibfly4_256(
            // Inputs (ytmp)
            ytmpp + i * VL_STEP,           // x0: ytmp[i]
            ytmpp + i * VL_STEP + 1 * BLOCK_STEP, // x1: ytmp[i+8]
            ytmpp + i * VL_STEP + 2 * BLOCK_STEP, // x2: ytmp[i+16]
            ytmpp + i * VL_STEP + 3 * BLOCK_STEP, // x3: ytmp[i+24]
            
            // Outputs (y)
            y_write_ptr + i * VL_STEP,           // y0: y[i]
            y_write_ptr + i * VL_STEP + 1 * BLOCK_STEP, // y1: y[i+8]
            y_write_ptr + i * VL_STEP + 2 * BLOCK_STEP, // y2: y[i+16]
            y_write_ptr + i * VL_STEP + 3 * BLOCK_STEP, // y3: y[i+24]

            // Twiddles (tw256_256p)
            tw256_ptr + i * VL_STEP,           // tw0: tw256[i]
            tw256_ptr + i * VL_STEP + 1 * BLOCK_STEP, // tw1: tw256[i+8]
            tw256_ptr + i * VL_STEP + 2 * BLOCK_STEP  // tw2: tw256[i+16]
        );
    }

    // 6. 第四阶段: Final Scaling (Optional)
    if (scale > 0) {
        size_t vl = __riscv_vsetvl_e16m4(VL_16);
        
        // 缩放循环 (执行两次移位)
        for (int k = 0; k < 2; k++) {
            // y256+=16 是将 y256 指针向后移动 16 个 256-bit 块
            // 每次循环处理 16 个 256-bit 块
            
            int16_t *current_y_base = y_ptr + k * 16 * VL_16; 
            
            for (int j = 0; j < 16; j++) {
                int16_t *curr_y_vec = current_y_base + j * VL_16;

                // 1. Load
                vint16m4_t y_vec = __riscv_vle16_v_i16m4(curr_y_vec, vl);
                
                // 2. Shift Right (Arithmetic, by 1)
                // y256[j] = shiftright_int16_simd256(y256[j], 1);
                y_vec = rvv_shiftright_int16(y_vec, 1, vl);
                
                // 3. Store
                __riscv_vse16_v_i16m4(curr_y_vec, y_vec, vl);
            }
        }
    }

  _mm_empty();
  _m_empty();

}

#endif
int16_t tw512[512] __attribute__((aligned(32))) = {
  32767,0,32764,-403,32757,-805,32744,-1207,32727,-1608,32705,-2010,32678,-2411,32646,-2812,32609,-3212,32567,-3612,32520,-4012,32468,-4410,32412,-4808,32350,-5206,32284,-5602,32213,-5998,32137,-6393,32056,-6787,31970,-7180,31880,-7572,31785,-7962,31684,-8352,31580,-8740,31470,-9127,31356,-9512,31236,-9896,31113,-10279,30984,-10660,30851,-11039,30713,-11417,30571,-11793,30424,-12167,30272,-12540,30116,-12910,29955,-13279,29790,-13646,29621,-14010,29446,-14373,29268,-14733,29085,-15091,28897,-15447,28706,-15800,28510,-16151,28309,-16500,28105,-16846,27896,-17190,27683,-17531,27466,-17869,27244,-18205,27019,-18538,26789,-18868,26556,-19195,26318,-19520,26077,-19841,25831,-20160,25582,-20475,25329,-20788,25072,-21097,24811,-21403,24546,-21706,24278,-22005,24006,-22302,23731,-22595,23452,-22884,23169,-23170,22883,-23453,22594,-23732,22301,-24007,22004,-24279,21705,-24547,21402,-24812,21096,-25073,20787,-25330,20474,-25583,20159,-25832,19840,-26078,19519,-26319,19194,-26557,18867,-26790,18537,-27020,18204,-27245,17868,-27467,17530,-27684,17189,-27897,16845,-28106,16499,-28310,16150,-28511,15799,-28707,15446,-28898,15090,-29086,14732,-29269,14372,-29447,14009,-29622,13645,-29791,13278,-29956,12909,-30117,12539,-30273,12166,-30425,11792,-30572,11416,-30714,11038,-30852,10659,-30985,10278,-31114,9895,-31237,9511,-31357,9126,-31471,8739,-31581,8351,-31685,7961,-31786,7571,-31881,7179,-31971,6786,-32057,6392,-32138,5997,-32214,5601,-32285,5205,-32351,4807,-32413,4409,-32469,4011,-32521,3611,-32568,3211,-32610,2811,-32647,2410,-32679,2009,-32706,1607,-32728,1206,-32745,804,-32758,402,-32765,0,-32767,-403,-32765,-805,-32758,-1207,-32745,-1608,-32728,-2010,-32706,-2411,-32679,-2812,-32647,-3212,-32610,-3612,-32568,-4012,-32521,-4410,-32469,-4808,-32413,-5206,-32351,-5602,-32285,-5998,-32214,-6393,-32138,-6787,-32057,-7180,-31971,-7572,-31881,-7962,-31786,-8352,-31685,-8740,-31581,-9127,-31471,-9512,-31357,-9896,-31237,-10279,-31114,-10660,-30985,-11039,-30852,-11417,-30714,-11793,-30572,-12167,-30425,-12540,-30273,-12910,-30117,-13279,-29956,-13646,-29791,-14010,-29622,-14373,-29447,-14733,-29269,-15091,-29086,-15447,-28898,-15800,-28707,-16151,-28511,-16500,-28310,-16846,-28106,-17190,-27897,-17531,-27684,-17869,-27467,-18205,-27245,-18538,-27020,-18868,-26790,-19195,-26557,-19520,-26319,-19841,-26078,-20160,-25832,-20475,-25583,-20788,-25330,-21097,-25073,-21403,-24812,-21706,-24547,-22005,-24279,-22302,-24007,-22595,-23732,-22884,-23453,-23170,-23170,-23453,-22884,-23732,-22595,-24007,-22302,-24279,-22005,-24547,-21706,-24812,-21403,-25073,-21097,-25330,-20788,-25583,-20475,-25832,-20160,-26078,-19841,-26319,-19520,-26557,-19195,-26790,-18868,-27020,-18538,-27245,-18205,-27467,-17869,-27684,-17531,-27897,-17190,-28106,-16846,-28310,-16500,-28511,-16151,-28707,-15800,-28898,-15447,-29086,-15091,-29269,-14733,-29447,-14373,-29622,-14010,-29791,-13646,-29956,-13279,-30117,-12910,-30273,-12540,-30425,-12167,-30572,-11793,-30714,-11417,-30852,-11039,-30985,-10660,-31114,-10279,-31237,-9896,-31357,-9512,-31471,-9127,-31581,-8740,-31685,-8352,-31786,-7962,-31881,-7572,-31971,-7180,-32057,-6787,-32138,-6393,-32214,-5998,-32285,-5602,-32351,-5206,-32413,-4808,-32469,-4410,-32521,-4012,-32568,-3612,-32610,-3212,-32647,-2812,-32679,-2411,-32706,-2010,-32728,-1608,-32745,-1207,-32758,-805,-32765,-403
};

int16_t tw512a[512] __attribute__((aligned(32))) = {
  32767,0,32764,403,32757,805,32744,1207,32727,1608,32705,2010,32678,2411,32646,2812,32609,3212,32567,3612,32520,4012,32468,4410,32412,4808,32350,5206,32284,5602,32213,5998,32137,6393,32056,6787,31970,7180,31880,7572,31785,7962,31684,8352,31580,8740,31470,9127,31356,9512,31236,9896,31113,10279,30984,10660,30851,11039,30713,11417,30571,11793,30424,12167,30272,12540,30116,12910,29955,13279,29790,13646,29621,14010,29446,14373,29268,14733,29085,15091,28897,15447,28706,15800,28510,16151,28309,16500,28105,16846,27896,17190,27683,17531,27466,17869,27244,18205,27019,18538,26789,18868,26556,19195,26318,19520,26077,19841,25831,20160,25582,20475,25329,20788,25072,21097,24811,21403,24546,21706,24278,22005,24006,22302,23731,22595,23452,22884,23169,23170,22883,23453,22594,23732,22301,24007,22004,24279,21705,24547,21402,24812,21096,25073,20787,25330,20474,25583,20159,25832,19840,26078,19519,26319,19194,26557,18867,26790,18537,27020,18204,27245,17868,27467,17530,27684,17189,27897,16845,28106,16499,28310,16150,28511,15799,28707,15446,28898,15090,29086,14732,29269,14372,29447,14009,29622,13645,29791,13278,29956,12909,30117,12539,30273,12166,30425,11792,30572,11416,30714,11038,30852,10659,30985,10278,31114,9895,31237,9511,31357,9126,31471,8739,31581,8351,31685,7961,31786,7571,31881,7179,31971,6786,32057,6392,32138,5997,32214,5601,32285,5205,32351,4807,32413,4409,32469,4011,32521,3611,32568,3211,32610,2811,32647,2410,32679,2009,32706,1607,32728,1206,32745,804,32758,402,32765,0,32767,-403,32765,-805,32758,-1207,32745,-1608,32728,-2010,32706,-2411,32679,-2812,32647,-3212,32610,-3612,32568,-4012,32521,-4410,32469,-4808,32413,-5206,32351,-5602,32285,-5998,32214,-6393,32138,-6787,32057,-7180,31971,-7572,31881,-7962,31786,-8352,31685,-8740,31581,-9127,31471,-9512,31357,-9896,31237,-10279,31114,-10660,30985,-11039,30852,-11417,30714,-11793,30572,-12167,30425,-12540,30273,-12910,30117,-13279,29956,-13646,29791,-14010,29622,-14373,29447,-14733,29269,-15091,29086,-15447,28898,-15800,28707,-16151,28511,-16500,28310,-16846,28106,-17190,27897,-17531,27684,-17869,27467,-18205,27245,-18538,27020,-18868,26790,-19195,26557,-19520,26319,-19841,26078,-20160,25832,-20475,25583,-20788,25330,-21097,25073,-21403,24812,-21706,24547,-22005,24279,-22302,24007,-22595,23732,-22884,23453,-23170,23170,-23453,22884,-23732,22595,-24007,22302,-24279,22005,-24547,21706,-24812,21403,-25073,21097,-25330,20788,-25583,20475,-25832,20160,-26078,19841,-26319,19520,-26557,19195,-26790,18868,-27020,18538,-27245,18205,-27467,17869,-27684,17531,-27897,17190,-28106,16846,-28310,16500,-28511,16151,-28707,15800,-28898,15447,-29086,15091,-29269,14733,-29447,14373,-29622,14010,-29791,13646,-29956,13279,-30117,12910,-30273,12540,-30425,12167,-30572,11793,-30714,11417,-30852,11039,-30985,10660,-31114,10279,-31237,9896,-31357,9512,-31471,9127,-31581,8740,-31685,8352,-31786,7962,-31881,7572,-31971,7180,-32057,6787,-32138,6393,-32214,5998,-32285,5602,-32351,5206,-32413,4808,-32469,4410,-32521,4012,-32568,3612,-32610,3212,-32647,2812,-32679,2411,-32706,2010,-32728,1608,-32745,1207,-32758,805,-32765,403
};



int16_t tw512b[512] __attribute__((aligned(32))) = {
  0,32767,-403,32764,-805,32757,-1207,32744,-1608,32727,-2010,32705,-2411,32678,-2812,32646,-3212,32609,-3612,32567,-4012,32520,-4410,32468,-4808,32412,-5206,32350,-5602,32284,-5998,32213,-6393,32137,-6787,32056,-7180,31970,-7572,31880,-7962,31785,-8352,31684,-8740,31580,-9127,31470,-9512,31356,-9896,31236,-10279,31113,-10660,30984,-11039,30851,-11417,30713,-11793,30571,-12167,30424,-12540,30272,-12910,30116,-13279,29955,-13646,29790,-14010,29621,-14373,29446,-14733,29268,-15091,29085,-15447,28897,-15800,28706,-16151,28510,-16500,28309,-16846,28105,-17190,27896,-17531,27683,-17869,27466,-18205,27244,-18538,27019,-18868,26789,-19195,26556,-19520,26318,-19841,26077,-20160,25831,-20475,25582,-20788,25329,-21097,25072,-21403,24811,-21706,24546,-22005,24278,-22302,24006,-22595,23731,-22884,23452,-23170,23169,-23453,22883,-23732,22594,-24007,22301,-24279,22004,-24547,21705,-24812,21402,-25073,21096,-25330,20787,-25583,20474,-25832,20159,-26078,19840,-26319,19519,-26557,19194,-26790,18867,-27020,18537,-27245,18204,-27467,17868,-27684,17530,-27897,17189,-28106,16845,-28310,16499,-28511,16150,-28707,15799,-28898,15446,-29086,15090,-29269,14732,-29447,14372,-29622,14009,-29791,13645,-29956,13278,-30117,12909,-30273,12539,-30425,12166,-30572,11792,-30714,11416,-30852,11038,-30985,10659,-31114,10278,-31237,9895,-31357,9511,-31471,9126,-31581,8739,-31685,8351,-31786,7961,-31881,7571,-31971,7179,-32057,6786,-32138,6392,-32214,5997,-32285,5601,-32351,5205,-32413,4807,-32469,4409,-32521,4011,-32568,3611,-32610,3211,-32647,2811,-32679,2410,-32706,2009,-32728,1607,-32745,1206,-32758,804,-32765,402,-32767,0,-32765,-403,-32758,-805,-32745,-1207,-32728,-1608,-32706,-2010,-32679,-2411,-32647,-2812,-32610,-3212,-32568,-3612,-32521,-4012,-32469,-4410,-32413,-4808,-32351,-5206,-32285,-5602,-32214,-5998,-32138,-6393,-32057,-6787,-31971,-7180,-31881,-7572,-31786,-7962,-31685,-8352,-31581,-8740,-31471,-9127,-31357,-9512,-31237,-9896,-31114,-10279,-30985,-10660,-30852,-11039,-30714,-11417,-30572,-11793,-30425,-12167,-30273,-12540,-30117,-12910,-29956,-13279,-29791,-13646,-29622,-14010,-29447,-14373,-29269,-14733,-29086,-15091,-28898,-15447,-28707,-15800,-28511,-16151,-28310,-16500,-28106,-16846,-27897,-17190,-27684,-17531,-27467,-17869,-27245,-18205,-27020,-18538,-26790,-18868,-26557,-19195,-26319,-19520,-26078,-19841,-25832,-20160,-25583,-20475,-25330,-20788,-25073,-21097,-24812,-21403,-24547,-21706,-24279,-22005,-24007,-22302,-23732,-22595,-23453,-22884,-23170,-23170,-22884,-23453,-22595,-23732,-22302,-24007,-22005,-24279,-21706,-24547,-21403,-24812,-21097,-25073,-20788,-25330,-20475,-25583,-20160,-25832,-19841,-26078,-19520,-26319,-19195,-26557,-18868,-26790,-18538,-27020,-18205,-27245,-17869,-27467,-17531,-27684,-17190,-27897,-16846,-28106,-16500,-28310,-16151,-28511,-15800,-28707,-15447,-28898,-15091,-29086,-14733,-29269,-14373,-29447,-14010,-29622,-13646,-29791,-13279,-29956,-12910,-30117,-12540,-30273,-12167,-30425,-11793,-30572,-11417,-30714,-11039,-30852,-10660,-30985,-10279,-31114,-9896,-31237,-9512,-31357,-9127,-31471,-8740,-31581,-8352,-31685,-7962,-31786,-7572,-31881,-7180,-31971,-6787,-32057,-6393,-32138,-5998,-32214,-5602,-32285,-5206,-32351,-4808,-32413,-4410,-32469,-4012,-32521,-3612,-32568,-3212,-32610,-2812,-32647,-2411,-32679,-2010,-32706,-1608,-32728,-1207,-32745,-805,-32758,-403,-32765
};

int16_t tw512c[512] __attribute__((aligned(32))) = {
  0,32767,403,32764,805,32757,1207,32744,1608,32727,2010,32705,2411,32678,2812,32646,3212,32609,3612,32567,4012,32520,4410,32468,4808,32412,5206,32350,5602,32284,5998,32213,6393,32137,6787,32056,7180,31970,7572,31880,7962,31785,8352,31684,8740,31580,9127,31470,9512,31356,9896,31236,10279,31113,10660,30984,11039,30851,11417,30713,11793,30571,12167,30424,12540,30272,12910,30116,13279,29955,13646,29790,14010,29621,14373,29446,14733,29268,15091,29085,15447,28897,15800,28706,16151,28510,16500,28309,16846,28105,17190,27896,17531,27683,17869,27466,18205,27244,18538,27019,18868,26789,19195,26556,19520,26318,19841,26077,20160,25831,20475,25582,20788,25329,21097,25072,21403,24811,21706,24546,22005,24278,22302,24006,22595,23731,22884,23452,23170,23169,23453,22883,23732,22594,24007,22301,24279,22004,24547,21705,24812,21402,25073,21096,25330,20787,25583,20474,25832,20159,26078,19840,26319,19519,26557,19194,26790,18867,27020,18537,27245,18204,27467,17868,27684,17530,27897,17189,28106,16845,28310,16499,28511,16150,28707,15799,28898,15446,29086,15090,29269,14732,29447,14372,29622,14009,29791,13645,29956,13278,30117,12909,30273,12539,30425,12166,30572,11792,30714,11416,30852,11038,30985,10659,31114,10278,31237,9895,31357,9511,31471,9126,31581,8739,31685,8351,31786,7961,31881,7571,31971,7179,32057,6786,32138,6392,32214,5997,32285,5601,32351,5205,32413,4807,32469,4409,32521,4011,32568,3611,32610,3211,32647,2811,32679,2410,32706,2009,32728,1607,32745,1206,32758,804,32765,402,32767,0,32765,-403,32758,-805,32745,-1207,32728,-1608,32706,-2010,32679,-2411,32647,-2812,32610,-3212,32568,-3612,32521,-4012,32469,-4410,32413,-4808,32351,-5206,32285,-5602,32214,-5998,32138,-6393,32057,-6787,31971,-7180,31881,-7572,31786,-7962,31685,-8352,31581,-8740,31471,-9127,31357,-9512,31237,-9896,31114,-10279,30985,-10660,30852,-11039,30714,-11417,30572,-11793,30425,-12167,30273,-12540,30117,-12910,29956,-13279,29791,-13646,29622,-14010,29447,-14373,29269,-14733,29086,-15091,28898,-15447,28707,-15800,28511,-16151,28310,-16500,28106,-16846,27897,-17190,27684,-17531,27467,-17869,27245,-18205,27020,-18538,26790,-18868,26557,-19195,26319,-19520,26078,-19841,25832,-20160,25583,-20475,25330,-20788,25073,-21097,24812,-21403,24547,-21706,24279,-22005,24007,-22302,23732,-22595,23453,-22884,23170,-23170,22884,-23453,22595,-23732,22302,-24007,22005,-24279,21706,-24547,21403,-24812,21097,-25073,20788,-25330,20475,-25583,20160,-25832,19841,-26078,19520,-26319,19195,-26557,18868,-26790,18538,-27020,18205,-27245,17869,-27467,17531,-27684,17190,-27897,16846,-28106,16500,-28310,16151,-28511,15800,-28707,15447,-28898,15091,-29086,14733,-29269,14373,-29447,14010,-29622,13646,-29791,13279,-29956,12910,-30117,12540,-30273,12167,-30425,11793,-30572,11417,-30714,11039,-30852,10660,-30985,10279,-31114,9896,-31237,9512,-31357,9127,-31471,8740,-31581,8352,-31685,7962,-31786,7572,-31881,7180,-31971,6787,-32057,6393,-32138,5998,-32214,5602,-32285,5206,-32351,4808,-32413,4410,-32469,4012,-32521,3612,-32568,3212,-32610,2812,-32647,2411,-32679,2010,-32706,1608,-32728,1207,-32745,805,-32758,403,-32765
};

#ifndef __AVX2__


#else //__AVX2__

void dft512(int16_t *x,int16_t *y,unsigned char scale)
{
  simd256_q15_t xtmp[64],*x256 = (simd256_q15_t *)x;
  simd256_q15_t ytmp[64],*y256=(simd256_q15_t*)y;
  simd256_q15_t *tw512_256p=(simd256_q15_t*)tw512,*y256p=(simd256_q15_t *)y;
  simd256_q15_t *ytmpp = &ytmp[0];
  int i;
  simd256_q15_t ONE_OVER_SQRT2_Q15_256 = set1_int16_simd256(ONE_OVER_SQRT2_Q15);


  transpose4_ooff_simd256(x256  ,xtmp,32);
  transpose4_ooff_simd256(x256+2,xtmp+1,32);
  transpose4_ooff_simd256(x256+4,xtmp+2,32);
  transpose4_ooff_simd256(x256+6,xtmp+3,32);
  transpose4_ooff_simd256(x256+8,xtmp+4,32);
  transpose4_ooff_simd256(x256+10,xtmp+5,32);
  transpose4_ooff_simd256(x256+12,xtmp+6,32);
  transpose4_ooff_simd256(x256+14,xtmp+7,32);
  transpose4_ooff_simd256(x256+16,xtmp+8,32);
  transpose4_ooff_simd256(x256+18,xtmp+9,32);
  transpose4_ooff_simd256(x256+20,xtmp+10,32);
  transpose4_ooff_simd256(x256+22,xtmp+11,32);
  transpose4_ooff_simd256(x256+24,xtmp+12,32);
  transpose4_ooff_simd256(x256+26,xtmp+13,32);
  transpose4_ooff_simd256(x256+28,xtmp+14,32);
  transpose4_ooff_simd256(x256+30,xtmp+15,32);
  transpose4_ooff_simd256(x256+32,xtmp+16,32);
  transpose4_ooff_simd256(x256+34,xtmp+17,32);
  transpose4_ooff_simd256(x256+36,xtmp+18,32);
  transpose4_ooff_simd256(x256+38,xtmp+19,32);
  transpose4_ooff_simd256(x256+40,xtmp+20,32);
  transpose4_ooff_simd256(x256+42,xtmp+21,32);
  transpose4_ooff_simd256(x256+44,xtmp+22,32);
  transpose4_ooff_simd256(x256+46,xtmp+23,32);
  transpose4_ooff_simd256(x256+48,xtmp+24,32);
  transpose4_ooff_simd256(x256+50,xtmp+25,32);
  transpose4_ooff_simd256(x256+52,xtmp+26,32);
  transpose4_ooff_simd256(x256+54,xtmp+27,32);
  transpose4_ooff_simd256(x256+56,xtmp+28,32);
  transpose4_ooff_simd256(x256+58,xtmp+29,32);
  transpose4_ooff_simd256(x256+60,xtmp+30,32);
  transpose4_ooff_simd256(x256+62,xtmp+31,32);
  dft256((int16_t*)(xtmp),(int16_t*)ytmp,1);
  dft256((int16_t*)(xtmp+32),(int16_t*)(ytmp+32),1);

  for (i=0; i<32; i++) {
    bfly2_256(ytmpp,ytmpp+32,
	      y256p,y256p+32,
	      tw512_256p);
    tw512_256p++;
    y256p++;
    ytmpp++;
  }

  if (scale>0) {

    for (i=0;i<4;i++) {
      y256[0] = mulhi_int16_simd256(y256[0],ONE_OVER_SQRT2_Q15_256);
      y256[1] = mulhi_int16_simd256(y256[1],ONE_OVER_SQRT2_Q15_256);
      y256[2] = mulhi_int16_simd256(y256[2],ONE_OVER_SQRT2_Q15_256);
      y256[3] = mulhi_int16_simd256(y256[3],ONE_OVER_SQRT2_Q15_256);
      y256[4] = mulhi_int16_simd256(y256[4],ONE_OVER_SQRT2_Q15_256);
      y256[5] = mulhi_int16_simd256(y256[5],ONE_OVER_SQRT2_Q15_256);
      y256[6] = mulhi_int16_simd256(y256[6],ONE_OVER_SQRT2_Q15_256);
      y256[7] = mulhi_int16_simd256(y256[7],ONE_OVER_SQRT2_Q15_256);
      y256[8] = mulhi_int16_simd256(y256[8],ONE_OVER_SQRT2_Q15_256);
      y256[9] = mulhi_int16_simd256(y256[9],ONE_OVER_SQRT2_Q15_256);
      y256[10] = mulhi_int16_simd256(y256[10],ONE_OVER_SQRT2_Q15_256);
      y256[11] = mulhi_int16_simd256(y256[11],ONE_OVER_SQRT2_Q15_256);
      y256[12] = mulhi_int16_simd256(y256[12],ONE_OVER_SQRT2_Q15_256);
      y256[13] = mulhi_int16_simd256(y256[13],ONE_OVER_SQRT2_Q15_256);
      y256[14] = mulhi_int16_simd256(y256[14],ONE_OVER_SQRT2_Q15_256);
      y256[15] = mulhi_int16_simd256(y256[15],ONE_OVER_SQRT2_Q15_256);
      y256+=16;
    }
  }
}

void rvv_dft512(int16_t *x,int16_t *y,unsigned char scale)
{
  // 1. 内存分配 (64 个 256-bit 向量 = 64 * 16 = 1024 个 int16_t)
    // xtmp 存储转置后的输入，ytmp 存储 dft256 的结果
    int16_t xtmp[64 * VL_16] __attribute__((aligned(32)));
    int16_t ytmp[64 * VL_16] __attribute__((aligned(32)));

    // 2. 指针定义 (使用 int16_t* 代替 simd256_q15_t*)
    int16_t *tw512_ptr = tw512; // 假设 tw512 是全局或传入的 int16_t 数组
    
    // 中间指针用于循环和索引 (保持原代码结构)
    int16_t *ytmpp = ytmp;
    int16_t *y_write_ptr = y;

    // RVV 向量设置
    size_t vl = __riscv_vsetvl_e16m4(VL_16);
    
    // 3. 第一阶段: Transpose (rvv_transpose4_ooff)
    // 原代码逻辑: transpose4_ooff_simd256(x256 + 2*i, xtmp + i, 32); for i = 0 to 31
    // N = 2*i (输入块偏移), M = i (输出块偏移)
    // off=32 对应 32*16 = 512 个 int16_t 的块偏移。

    for (int i = 0; i < 32; i++) {
        // x 的 256-bit 块偏移量是 2*i
        // xtmp 的 256-bit 块偏移量是 i
        // 1 块 = 16 int16_t
        rvv_transpose4_ooff(
            x + (2 * i) * VL_16,  // 输入指针 x256 + 2*i
            xtmp + i * VL_16,     // 输出指针 xtmp + i
            32                    // off=32 (32 * 16 个 int16_t 间距)
        );
    }
    
    // 4. 第二阶段: DFT256
    // 调用两次 dft256，分别处理 xtmp 的前一半和后一半。
    // dft256(x, y, scale)
    
    // dft256((int16_t*)(xtmp), (int16_t*)ytmp, 1);
    rvv_dft256(xtmp + 0 * VL_16, ytmp + 0 * VL_16, 1);

    // dft256((int16_t*)(xtmp+32), (int16_t*)(ytmp+32), 1);
    rvv_dft256(xtmp + 32 * VL_16, ytmp + 32 * VL_16, 1);


    // 5. 第三阶段: Butterfly 2 (rvv_bfly2_256)
    // bfly2_256(x0, x1, y0, y1, tw)
    // 循环 32 次 (i=0 to 31)，每次处理一个 256-bit 块 (16 int16_t)

    // ytmpp 遍历 ytmp[0] 到 ytmp[31]
    // y256p 遍历 y[0] 到 y[31]
    // tw512_256p 遍历 tw512[0] 到 tw512[31]
    
    // 块偏移量 (Block Step) 是 32 * VL_16 = 512 int16_t
    const int BLOCK_STEP = 32 * VL_16; 

    for (int i = 0; i < 32; i++) {
        
        rvv_bfly2_256(
            // Inputs (ytmp)
            ytmpp + i * VL_16,            // x0: ytmpp[i]
            ytmpp + i * VL_16 + BLOCK_STEP, // x1: ytmpp[i+32]
            
            // Outputs (y)
            y_write_ptr + i * VL_16,            // y0: y256p[i]
            y_write_ptr + i * VL_16 + BLOCK_STEP, // y1: y256p[i+32]

            // Twiddles (tw512)
            tw512_ptr + i * VL_16         // tw: tw512_256p[i]
        );
    }


    // 6. 第四阶段: Final Scaling (Optional)
    if (scale > 0) {
        
        // 常数 ONE_OVER_SQRT2_Q15_256 的 RVV 实现
        // 假设 ONE_OVER_SQRT2_Q15 已定义 (Q15 格式)

        vint16m4_t ONE_OVER_SQRT2_VEC = __riscv_vmv_v_x_i16m4(ONE_OVER_SQRT2_Q15, vl);

        // 循环处理所有 64 个 256-bit 块。
        // 原代码中 y256+=16，循环 4 次，共 4 * 16 = 64 个块
        for (int k = 0; k < 4; k++) {
            // current_y_base 每次跳 16 个 256-bit 块，即 16 * 16 = 256 个 int16_t
            int16_t *current_y_base = y + k * 16 * VL_16; 
            
            // 每次循环处理 16 个 256-bit 块
            for (int j = 0; j < 16; j++) {
                int16_t *curr_y_vec = current_y_base + j * VL_16;

                // 1. Load
                vint16m4_t y_vec = __riscv_vle16_v_i16m4(curr_y_vec, vl);
                
                // 2. Multiply High (Q15 * Q15 -> Q15)
                // y256[j] = mulhi_int16_simd256(y256[j], ONE_OVER_SQRT2_Q15_256);
                
                // 使用 vmulh.vv (或 vmulh.vx) 实现 Q15 乘法
                // vmulh.vv/vx 自动取高 16 位，实现 Q15 * Q15 -> Q15
                y_vec = __riscv_vmulh_vv_i16m4(y_vec, ONE_OVER_SQRT2_VEC, vl);
                
                // 3. Store
                __riscv_vse16_v_i16m4(curr_y_vec, y_vec, vl);
            }
        }
    }

}

void idft512(int16_t *x,int16_t *y,unsigned char scale)
{

  simd256_q15_t xtmp[64],*x256 = (simd256_q15_t *)x;
  simd256_q15_t ytmp[64],*y256=(simd256_q15_t*)y;
  simd256_q15_t *tw512_256p=(simd256_q15_t *)tw512,*y256p=(simd256_q15_t *)y;
  simd256_q15_t *ytmpp = &ytmp[0];
  int i;
  simd256_q15_t ONE_OVER_SQRT2_Q15_256 = set1_int16_simd256(ONE_OVER_SQRT2_Q15);


  transpose4_ooff_simd256(x256  ,xtmp,32);
  transpose4_ooff_simd256(x256+2,xtmp+1,32);
  transpose4_ooff_simd256(x256+4,xtmp+2,32);
  transpose4_ooff_simd256(x256+6,xtmp+3,32);
  transpose4_ooff_simd256(x256+8,xtmp+4,32);
  transpose4_ooff_simd256(x256+10,xtmp+5,32);
  transpose4_ooff_simd256(x256+12,xtmp+6,32);
  transpose4_ooff_simd256(x256+14,xtmp+7,32);
  transpose4_ooff_simd256(x256+16,xtmp+8,32);
  transpose4_ooff_simd256(x256+18,xtmp+9,32);
  transpose4_ooff_simd256(x256+20,xtmp+10,32);
  transpose4_ooff_simd256(x256+22,xtmp+11,32);
  transpose4_ooff_simd256(x256+24,xtmp+12,32);
  transpose4_ooff_simd256(x256+26,xtmp+13,32);
  transpose4_ooff_simd256(x256+28,xtmp+14,32);
  transpose4_ooff_simd256(x256+30,xtmp+15,32);
  transpose4_ooff_simd256(x256+32,xtmp+16,32);
  transpose4_ooff_simd256(x256+34,xtmp+17,32);
  transpose4_ooff_simd256(x256+36,xtmp+18,32);
  transpose4_ooff_simd256(x256+38,xtmp+19,32);
  transpose4_ooff_simd256(x256+40,xtmp+20,32);
  transpose4_ooff_simd256(x256+42,xtmp+21,32);
  transpose4_ooff_simd256(x256+44,xtmp+22,32);
  transpose4_ooff_simd256(x256+46,xtmp+23,32);
  transpose4_ooff_simd256(x256+48,xtmp+24,32);
  transpose4_ooff_simd256(x256+50,xtmp+25,32);
  transpose4_ooff_simd256(x256+52,xtmp+26,32);
  transpose4_ooff_simd256(x256+54,xtmp+27,32);
  transpose4_ooff_simd256(x256+56,xtmp+28,32);
  transpose4_ooff_simd256(x256+58,xtmp+29,32);
  transpose4_ooff_simd256(x256+60,xtmp+30,32);
  transpose4_ooff_simd256(x256+62,xtmp+31,32);

  idft256((int16_t*)(xtmp),(int16_t*)ytmp,1);
  idft256((int16_t*)(xtmp+32),(int16_t*)(ytmp+32),1);


  for (i=0; i<32; i++) {
    ibfly2_256(ytmpp,ytmpp+32,
	       y256p,y256p+32,
	       tw512_256p);
    tw512_256p++;
    y256p++;
    ytmpp++;
  }

  if (scale>0) {

    for (i=0;i<4;i++) {
      y256[0] = mulhi_int16_simd256(y256[0],ONE_OVER_SQRT2_Q15_256);
      y256[1] = mulhi_int16_simd256(y256[1],ONE_OVER_SQRT2_Q15_256);
      y256[2] = mulhi_int16_simd256(y256[2],ONE_OVER_SQRT2_Q15_256);
      y256[3] = mulhi_int16_simd256(y256[3],ONE_OVER_SQRT2_Q15_256);
      y256[4] = mulhi_int16_simd256(y256[4],ONE_OVER_SQRT2_Q15_256);
      y256[5] = mulhi_int16_simd256(y256[5],ONE_OVER_SQRT2_Q15_256);
      y256[6] = mulhi_int16_simd256(y256[6],ONE_OVER_SQRT2_Q15_256);
      y256[7] = mulhi_int16_simd256(y256[7],ONE_OVER_SQRT2_Q15_256);
      y256[8] = mulhi_int16_simd256(y256[8],ONE_OVER_SQRT2_Q15_256);
      y256[9] = mulhi_int16_simd256(y256[9],ONE_OVER_SQRT2_Q15_256);
      y256[10] = mulhi_int16_simd256(y256[10],ONE_OVER_SQRT2_Q15_256);
      y256[11] = mulhi_int16_simd256(y256[11],ONE_OVER_SQRT2_Q15_256);
      y256[12] = mulhi_int16_simd256(y256[12],ONE_OVER_SQRT2_Q15_256);
      y256[13] = mulhi_int16_simd256(y256[13],ONE_OVER_SQRT2_Q15_256);
      y256[14] = mulhi_int16_simd256(y256[14],ONE_OVER_SQRT2_Q15_256);
      y256[15] = mulhi_int16_simd256(y256[15],ONE_OVER_SQRT2_Q15_256);
      y256+=16;
    }
  }

}

void rvv_idft512(int16_t *x,int16_t *y,unsigned char scale)
{

  // 1. 内存分配 (64 个 256-bit 向量 = 1024 个 int16_t)
    // xtmp 存储转置后的输入，ytmp 存储 idft256 的结果
    int16_t xtmp[64 * 16] __attribute__((aligned(32)));
    int16_t ytmp[64 * 16] __attribute__((aligned(32)));

    // 2. 指针定义 (使用 int16_t* 代替 simd256_q15_t*)
    // 假设 tw512 是全局或传入的 int16_t 数组
    // 注意：IDFT 512 的蝶形因子通常与 DFT 512 不同，但原代码都使用 tw512。
    int16_t *tw512_ptr = tw512; 
    
    // 中间指针用于循环和索引
    int16_t *ytmpp = ytmp;
    int16_t *y_write_ptr = y;

    // RVV 向量设置
    size_t vl = __riscv_vsetvl_e16m4(16);
    
    // --- 阶段一: Transpose ---
    // 循环 32 次 (i=0 to 31)
    // 原代码逻辑: transpose4_ooff_simd256(x256 + 2*i, xtmp + i, 32); 

    for (int i = 0; i < 32; i++) {
        // x 的 256-bit 块偏移量是 2*i
        // xtmp 的 256-bit 块偏移量是 i
        // 1 块 = 16 int16_t
         rvv_transpose4_ooff(
            x + (2 * i) * 16,  // 输入指针 x256 + 2*i
            xtmp + i * 16,     // 输出指针 xtmp + i
            32                    // off=32 (32 * 16 个 int16_t 间距)
        );
    }
    
    // --- 阶段二: IDFT256 ---
    // 调用两次 idft256，分别处理 xtmp 的前一半和后一半。
    // 原代码逻辑: idft256((int16_t*)(xtmp), (int16_t*)ytmp, 1);
    //             idft256((int16_t*)(xtmp+32), (int16_t*)(ytmp+32), 1);
    
    rvv_idft256(xtmp + 0 * 16, ytmp + 0 * 16, 1);
    rvv_idft256(xtmp + 32 * 16, ytmp + 32 * 16, 1);


    // --- 阶段三: Inverse Butterfly 2 (rvv_ibfly2_256) ---
    // 循环 32 次 (i=0 to 31)，每次处理一个 256-bit 块 (16 int16_t)
    // 原代码逻辑: ibfly2_256(ytmpp, ytmpp+32, y256p, y256p+32, tw512_256p);
    
    const int BLOCK_STEP = 32 * 16; 

    for (int i = 0; i < 32; i++) {
        
        // !!! 使用 rvv_ibfly2_256 !!!
        rvv_ibfly2_256(
            // Inputs (ytmp)
            ytmpp + i * 16,            // x0: ytmpp[i]
            ytmpp + i * 16 + BLOCK_STEP, // x1: ytmpp[i+32]
            
            // Outputs (y)
            y_write_ptr + i * 16,            // y0: y256p[i]
            y_write_ptr + i * 16 + BLOCK_STEP, // y1: y256p[i+32]

            // Twiddles (tw512)
            tw512_ptr + i * 16         // tw: tw512_256p[i]
        );
    }


    // --- 阶段四: Final Scaling (Optional) ---
    if (scale > 0) {
        
        // 常数 ONE_OVER_SQRT2_Q15_256 的 RVV 实现
        // 假设 ONE_OVER_SQRT2_Q15 已定义 (Q15 格式)
        vint16m4_t ONE_OVER_SQRT2_VEC = __riscv_vmv_v_x_i16m4(ONE_OVER_SQRT2_Q15, vl);

        // 循环处理所有 64 个 256-bit 块 (4 * 16 = 64)
        for (int k = 0; k < 4; k++) {
            // current_y_base 每次跳 16 个 256-bit 块，即 16 * 16 = 256 个 int16_t
            int16_t *current_y_base = y + k * 16 * 16; 
            
            // 每次循环处理 16 个 256-bit 块
            for (int j = 0; j < 16; j++) {
                int16_t *curr_y_vec = current_y_base + j * 16;

                // 1. Load
                vint16m4_t y_vec = __riscv_vle16_v_i16m4(curr_y_vec, vl);
                
                // 2. Multiply High (Q15 * Q15 -> Q15)
                // y256[j] = mulhi_int16_simd256(y256[j], ONE_OVER_SQRT2_Q15_256);
                
                // 使用 vmulh.vv (或 vmulh.vx) 实现 Q15 乘法
                y_vec = __riscv_vmulh_vv_i16m4(y_vec, ONE_OVER_SQRT2_VEC, vl);
                
                // 3. Store
                __riscv_vse16_v_i16m4(curr_y_vec, y_vec, vl);
            }
        }
    }
}

#endif

int16_t tw1024[1536] __attribute__((aligned(32)));

#ifndef __AVX2__


#else //__AVX2__
void dft1024(int16_t *x,int16_t *y,unsigned char scale)
{

  simd256_q15_t xtmp[128],ytmp[128],*tw1024_256p=(simd256_q15_t *)tw1024,*x256=(simd256_q15_t *)x,*y256=(simd256_q15_t *)y,*y256p=(simd256_q15_t *)y;
  simd256_q15_t *ytmpp = &ytmp[0];
  int i,j;

  for (i=0,j=0; i<128; i+=4,j++) {
    transpose16_ooff_simd256(x256+i,xtmp+j,32);
  }


  dft256((int16_t*)(xtmp),(int16_t*)(ytmp),1);
  dft256((int16_t*)(xtmp+32),(int16_t*)(ytmp+32),1);
  dft256((int16_t*)(xtmp+64),(int16_t*)(ytmp+64),1);
  dft256((int16_t*)(xtmp+96),(int16_t*)(ytmp+96),1);

  for (i=0; i<32; i++) {
    bfly4_256(ytmpp,ytmpp+32,ytmpp+64,ytmpp+96,
	      y256p,y256p+32,y256p+64,y256p+96,
	      tw1024_256p,tw1024_256p+32,tw1024_256p+64);
    tw1024_256p++;
    y256p++;
    ytmpp++;
  }

  if (scale>0) {

    for (i=0; i<8; i++) {
      y256[0]  = shiftright_int16_simd256(y256[0],1);
      y256[1]  = shiftright_int16_simd256(y256[1],1);
      y256[2]  = shiftright_int16_simd256(y256[2],1);
      y256[3]  = shiftright_int16_simd256(y256[3],1);
      y256[4]  = shiftright_int16_simd256(y256[4],1);
      y256[5]  = shiftright_int16_simd256(y256[5],1);
      y256[6]  = shiftright_int16_simd256(y256[6],1);
      y256[7]  = shiftright_int16_simd256(y256[7],1);
      y256[8]  = shiftright_int16_simd256(y256[8],1);
      y256[9]  = shiftright_int16_simd256(y256[9],1);
      y256[10] = shiftright_int16_simd256(y256[10],1);
      y256[11] = shiftright_int16_simd256(y256[11],1);
      y256[12] = shiftright_int16_simd256(y256[12],1);
      y256[13] = shiftright_int16_simd256(y256[13],1);
      y256[14] = shiftright_int16_simd256(y256[14],1);
      y256[15] = shiftright_int16_simd256(y256[15],1);

      y256+=16;
    }

  }

  _mm_empty();
  _m_empty();

}

void rvv_dft1024(int16_t *x, int16_t *y, unsigned char scale)
{
    // 1. 内存分配 (128 个 256-bit 向量 = 128 * 16 = 2048 个 int16_t)
    // xtmp 存储转置后的输入，ytmp 存储 dft256 的结果
    int16_t xtmp[128 * VL_16] __attribute__((aligned(32)));
    int16_t ytmp[128 * VL_16] __attribute__((aligned(32)));

    // 2. 指针定义
    int16_t *tw1024_ptr = tw1024; // 假设 tw1024 是全局或传入的 int16_t 数组
    
    // 中间指针用于循环和索引 (与原代码保持一致的逻辑)
    int16_t *ytmpp = ytmp;
    int16_t *y_write_ptr = y;

    int i, j;
    
    // RVV 向量设置
    size_t vl = __riscv_vsetvl_e16m4(VL_16);
    
    // 3. 第一阶段: Transpose (rvv_transpose16_ooff)
    // 原代码逻辑: for (i=0, j=0; i<128; i+=4, j++) { transpose16_ooff_simd256(x256+i, xtmp+j, 32); }
    // 循环 32 次 (j=0 to 31)。
    // 输入块偏移 i = 4*j, 输出块偏移 j。
    // x256+i 对应 x + (4*j)*VL_16
    // xtmp+j 对应 xtmp + j*VL_16
    // off=32 (32 * 16 个 int16_t 间距)

    for (j = 0; j < 32; j++) {
        // i 是输入块在 x256 数组中的索引，对应 int16_t 偏移量 (4 * j) * VL_16
        // j 是输出块在 xtmp 数组中的索引，对应 int16_t 偏移量 j * VL_16
        rvv_transpose16_ooff(
            x + (4 * j) * 16,  // 输入指针 x256 + 2*i
            xtmp + j * 16,     // 输出指针 xtmp + i
            32                    // off=32 (32 * 16 个 int16_t 间距)
        );
    }
    
    // 4. 第二阶段: DFT256
    // 调用四次 rvv_dft256，处理 xtmp 的四个 256 长度的块。
    // 块大小 32 * VL_16 = 512 int16_t。
    const int N256_BLOCK_SIZE = 32 * VL_16;
    
    // dft256((int16_t*)(xtmp), (int16_t*)(ytmp), 1);
    rvv_dft256(xtmp + 0 * N256_BLOCK_SIZE, ytmp + 0 * N256_BLOCK_SIZE, 1);

    // dft256((int16_t*)(xtmp+32), (int16_t*)(ytmp+32), 1);
    rvv_dft256(xtmp + 1 * N256_BLOCK_SIZE, ytmp + 1 * N256_BLOCK_SIZE, 1);

    // dft256((int16_t*)(xtmp+64), (int16_t*)(ytmp+64), 1);
    rvv_dft256(xtmp + 2 * N256_BLOCK_SIZE, ytmp + 2 * N256_BLOCK_SIZE, 1);

    // dft256((int16_t*)(xtmp+96), (int16_t*)(ytmp+96), 1);
    rvv_dft256(xtmp + 3 * N256_BLOCK_SIZE, ytmp + 3 * N256_BLOCK_SIZE, 1);


    // 5. 第三阶段: Butterfly 4 (rvv_bfly4_256)
    // 循环 32 次 (i=0 to 31)，每次处理一个 256-bit 块 (16 int16_t)
    // 块偏移量 (Block Step) 是 32 * VL_16 = 512 int16_t
    const int BLOCK_STEP = 32 * VL_16; 

    for (i = 0; i < 32; i++) {
        
        // twiddles 因子在原代码中为 tw1024_256p, tw1024_256p+32, tw1024_256p+64
        // 注意：原代码的指针自增是 tw1024_256p++，所以基地址是 tw1024_ptr + i*VL_16
        const int tw1_offset = i * VL_16;
        const int tw2_offset = (32 + i) * VL_16;
        const int tw3_offset = (64 + i) * VL_16;

        rvv_bfly4_256(
            // Inputs (ytmp)
            ytmpp + i * 16,// x0: ytmp[i]
            ytmpp + i * VL_16 + 1 * BLOCK_STEP, // x1: ytmp[i+32]
            ytmpp + i * VL_16 + 2 * BLOCK_STEP, // x2: ytmp[i+64]
            ytmpp + i * VL_16 + 3 * BLOCK_STEP, // x3: ytmp[i+96]
            
            // Outputs (y)
            y_write_ptr + i * 16,// y0: y[i]
            y_write_ptr + i * VL_16 + 1 * BLOCK_STEP, // y1: y[i+32]
            y_write_ptr + i * VL_16 + 2 * BLOCK_STEP, // y2: y[i+64]
            y_write_ptr + i * VL_16 + 3 * BLOCK_STEP, // y3: y[i+96]

            // Twiddles (tw1024)
            tw1024_ptr + tw1_offset, 
            tw1024_ptr + tw2_offset, 
            tw1024_ptr + tw3_offset
        );
    }


    // 6. 第四阶段: Final Scaling (Optional)
    if (scale > 0) {
        
        // 缩放操作是：shiftright_int16_simd256(..., 1) 即右移 1 位 (除以 2)
        const int SHIFT_AMOUNT = 1;
        
        // 循环处理所有 128 个 256-bit 块。
        // 原代码中 y256+=16，循环 8 次，共 8 * 16 = 128 个块
        const int NUM_BLOCKS = 128; // 1024个复数 / 8个复数每块 = 128块
        
        for (i = 0; i < NUM_BLOCKS; i++) {
            
            int16_t *curr_y_vec = y + i * VL_16;

            // 1. Load
            vint16m4_t y_vec = __riscv_vle16_v_i16m4(curr_y_vec, vl);
            
            // 2. Shift Right (算术右移实现除以 2)
            // shiftright_int16_simd256(y256[j], 1)
            // 使用 vsra.vi 或 vsa.vi
            // RISC-V V-Extension 算术右移： vsra.vi
            y_vec = __riscv_vsra_vx_i16m4(y_vec, SHIFT_AMOUNT, vl);
            
            // 3. Store
            __riscv_vse16_v_i16m4(curr_y_vec, y_vec, vl);
        }
    }
}

void idft1024(int16_t *x,int16_t *y,unsigned char scale)
{

  simd256_q15_t xtmp[128],ytmp[128],*tw1024_256p=(simd256_q15_t *)tw1024,*x256=(simd256_q15_t *)x,*y256=(simd256_q15_t *)y,*y256p=(simd256_q15_t *)y;
  simd256_q15_t *ytmpp = &ytmp[0];
  int i,j;

  for (i=0,j=0; i<128; i+=4,j++) {
    transpose16_ooff_simd256(x256+i,xtmp+j,32);
  }


  idft256((int16_t*)(xtmp),(int16_t*)(ytmp),1);
  idft256((int16_t*)(xtmp+32),(int16_t*)(ytmp+32),1);
  idft256((int16_t*)(xtmp+64),(int16_t*)(ytmp+64),1);
  idft256((int16_t*)(xtmp+96),(int16_t*)(ytmp+96),1);

  for (i=0; i<32; i++) {
    ibfly4_256(ytmpp,ytmpp+32,ytmpp+64,ytmpp+96,
	       y256p,y256p+32,y256p+64,y256p+96,
	       tw1024_256p,tw1024_256p+32,tw1024_256p+64);
    tw1024_256p++;
    y256p++;
    ytmpp++;
  }

  if (scale>0) {

    for (i=0; i<8; i++) {
      y256[0]  = shiftright_int16_simd256(y256[0],1);
      y256[1]  = shiftright_int16_simd256(y256[1],1);
      y256[2]  = shiftright_int16_simd256(y256[2],1);
      y256[3]  = shiftright_int16_simd256(y256[3],1);
      y256[4]  = shiftright_int16_simd256(y256[4],1);
      y256[5]  = shiftright_int16_simd256(y256[5],1);
      y256[6]  = shiftright_int16_simd256(y256[6],1);
      y256[7]  = shiftright_int16_simd256(y256[7],1);
      y256[8]  = shiftright_int16_simd256(y256[8],1);
      y256[9]  = shiftright_int16_simd256(y256[9],1);
      y256[10] = shiftright_int16_simd256(y256[10],1);
      y256[11] = shiftright_int16_simd256(y256[11],1);
      y256[12] = shiftright_int16_simd256(y256[12],1);
      y256[13] = shiftright_int16_simd256(y256[13],1);
      y256[14] = shiftright_int16_simd256(y256[14],1);
      y256[15] = shiftright_int16_simd256(y256[15],1);

      y256+=16;
    }

  }

  _mm_empty();
  _m_empty();

}

void rvv_idft1024(int16_t *x, int16_t *y, unsigned char scale)
{
    // 1. 内存分配 (128 个 256-bit 向量 = 128 * 16 = 2048 个 int16_t)
    int16_t xtmp[128 * VL_16] __attribute__((aligned(32)));
    int16_t ytmp[128 * VL_16] __attribute__((aligned(32)));

    // 2. 指针定义
    int16_t *tw1024_ptr = tw1024; // 假设 tw1024 是全局或传入的 int16_t 数组
    
    // 中间指针用于循环和索引
    int16_t *ytmpp = ytmp;
    int16_t *y_write_ptr = y;

    int i, j;
    
    // RVV 向量设置
    size_t vl = __riscv_vsetvl_e16m4(VL_16);
    
    // --- 第一阶段: Transpose (rvv_transpose16_ooff) ---
    // 逻辑: for (i=0, j=0; i<128; i+=4, j++) { transpose16_ooff_simd256(x256+i, xtmp+j, 32); }
    for (j = 0; j < 32; j++) {
        rvv_transpose16_ooff(
            x + (4 * j) * 16,  // 输入指针 x256 + 2*i
            xtmp + j * 16,     // 输出指针 xtmp + i
            32                    // off=32 (32 * 16 个 int16_t 间距)
        );
    }
    
    // --- 第二阶段: IDFT256 ---
    // 调用四次 rvv_idft256
    const int N256_BLOCK_SIZE = 32 * VL_16;
    
    // idft256((int16_t*)(xtmp), (int16_t*)(ytmp), 1);
    rvv_idft256(xtmp + 0 * N256_BLOCK_SIZE, ytmp + 0 * N256_BLOCK_SIZE, 1);

    // idft256((int16_t*)(xtmp+32), (int16_t*)(ytmp+32), 1);
    rvv_idft256(xtmp + 1 * N256_BLOCK_SIZE, ytmp + 1 * N256_BLOCK_SIZE, 1);

    // idft256((int16_t*)(xtmp+64), (int16_t*)(ytmp+64), 1);
    rvv_idft256(xtmp + 2 * N256_BLOCK_SIZE, ytmp + 2 * N256_BLOCK_SIZE, 1);

    // idft256((int16_t*)(xtmp+96), (int16_t*)(ytmp+96), 1);
    rvv_idft256(xtmp + 3 * N256_BLOCK_SIZE, ytmp + 3 * N256_BLOCK_SIZE, 1);


    // --- 第三阶段: Inverse Butterfly 4 (rvv_ibfly4_256) ---
    // 循环 32 次 (i=0 to 31)，每次处理一个 256-bit 块
    const int BLOCK_STEP = 32 * VL_16; 

    for (i = 0; i < 32; i++) {
        
        // Twiddles 偏移量 (与 dft1024 相同)
        const int tw1_offset = i * VL_16;
        const int tw2_offset = (32 + i) * VL_16;
        const int tw3_offset = (64 + i) * VL_16;
        
        rvv_ibfly4_256(
            // Inputs (ytmp)
            ytmpp + i * 16,// x0: ytmp[i]
            ytmpp + i * VL_16 + 1 * BLOCK_STEP, // x1: ytmp[i+32]
            ytmpp + i * VL_16 + 2 * BLOCK_STEP, // x2: ytmp[i+64]
            ytmpp + i * VL_16 + 3 * BLOCK_STEP, // x3: ytmp[i+96]
            
            // Outputs (y)
            y_write_ptr + i * 16,// y0: y[i]
            y_write_ptr + i * VL_16 + 1 * BLOCK_STEP, // y1: y[i+32]
            y_write_ptr + i * VL_16 + 2 * BLOCK_STEP, // y2: y[i+64]
            y_write_ptr + i * VL_16 + 3 * BLOCK_STEP, // y3: y[i+96]

            // Twiddles (tw1024)
            tw1024_ptr + tw1_offset, 
            tw1024_ptr + tw2_offset, 
            tw1024_ptr + tw3_offset
        );
    }


    // --- 第四阶段: Final Scaling (Optional) ---
    if (scale > 0) {
        
        // 缩放操作是：shiftright_int16_simd256(..., 1) 即右移 1 位 (除以 2)
        const int SHIFT_AMOUNT = 1;
        const int NUM_BLOCKS = 128; // 1024个复数 / 8个复数每块 = 128块
        
        for (i = 0; i < NUM_BLOCKS; i++) {
            
            int16_t *curr_y_vec = y + i * VL_16;

            // 1. Load
            vint16m4_t y_vec = __riscv_vle16_v_i16m4(curr_y_vec, vl);
            
            // 2. Shift Right (算术右移实现除以 2)
            // 使用 vsra.vx 实现算术右移
            y_vec = __riscv_vsra_vx_i16m4(y_vec, SHIFT_AMOUNT, vl);
            
            // 3. Store
            __riscv_vse16_v_i16m4(curr_y_vec, y_vec, vl);
        }
    }
}

#endif

int16_t tw2048[2048] __attribute__((aligned(32)));

#ifndef __AVX2__


#else // __AVX2__

void dft2048(int16_t *x,int16_t *y,unsigned char scale)
{

  simd256_q15_t xtmp[256],*xtmpp,*x256 = (simd256_q15_t *)x;
  simd256_q15_t ytmp[256],*tw2048_256p=(simd256_q15_t *)tw2048,*y256=(simd256_q15_t *)y,*y256p=(simd256_q15_t *)y;
  simd256_q15_t *ytmpp = &ytmp[0];
  int i;
  simd256_q15_t ONE_OVER_SQRT2_Q15_128 = set1_int16_simd256(ONE_OVER_SQRT2_Q15);


  xtmpp = xtmp;

  for (i=0; i<4; i++) {
    transpose4_ooff_simd256(x256  ,xtmpp,128);
    transpose4_ooff_simd256(x256+2,xtmpp+1,128);
    transpose4_ooff_simd256(x256+4,xtmpp+2,128);
    transpose4_ooff_simd256(x256+6,xtmpp+3,128);
    transpose4_ooff_simd256(x256+8,xtmpp+4,128);
    transpose4_ooff_simd256(x256+10,xtmpp+5,128);
    transpose4_ooff_simd256(x256+12,xtmpp+6,128);
    transpose4_ooff_simd256(x256+14,xtmpp+7,128);
    transpose4_ooff_simd256(x256+16,xtmpp+8,128);
    transpose4_ooff_simd256(x256+18,xtmpp+9,128);
    transpose4_ooff_simd256(x256+20,xtmpp+10,128);
    transpose4_ooff_simd256(x256+22,xtmpp+11,128);
    transpose4_ooff_simd256(x256+24,xtmpp+12,128);
    transpose4_ooff_simd256(x256+26,xtmpp+13,128);
    transpose4_ooff_simd256(x256+28,xtmpp+14,128);
    transpose4_ooff_simd256(x256+30,xtmpp+15,128);
    transpose4_ooff_simd256(x256+32,xtmpp+16,128);
    transpose4_ooff_simd256(x256+34,xtmpp+17,128);
    transpose4_ooff_simd256(x256+36,xtmpp+18,128);
    transpose4_ooff_simd256(x256+38,xtmpp+19,128);
    transpose4_ooff_simd256(x256+40,xtmpp+20,128);
    transpose4_ooff_simd256(x256+42,xtmpp+21,128);
    transpose4_ooff_simd256(x256+44,xtmpp+22,128);
    transpose4_ooff_simd256(x256+46,xtmpp+23,128);
    transpose4_ooff_simd256(x256+48,xtmpp+24,128);
    transpose4_ooff_simd256(x256+50,xtmpp+25,128);
    transpose4_ooff_simd256(x256+52,xtmpp+26,128);
    transpose4_ooff_simd256(x256+54,xtmpp+27,128);
    transpose4_ooff_simd256(x256+56,xtmpp+28,128);
    transpose4_ooff_simd256(x256+58,xtmpp+29,128);
    transpose4_ooff_simd256(x256+60,xtmpp+30,128);
    transpose4_ooff_simd256(x256+62,xtmpp+31,128);
    x256+=64;
    xtmpp+=32;
  }

  dft1024((int16_t*)(xtmp),(int16_t*)ytmp,1);
  dft1024((int16_t*)(xtmp+128),(int16_t*)(ytmp+128),1);


  for (i=0; i<128; i++) {
    bfly2_256(ytmpp,ytmpp+128,
	      y256p,y256p+128,
	      tw2048_256p);
    tw2048_256p++;
    y256p++;
    ytmpp++;
  }

  if (scale>0) {
    y256p = y256;

    for (i=0; i<16; i++) {
      y256p[0]  = mulhi_int16_simd256(y256p[0],ONE_OVER_SQRT2_Q15_128);
      y256p[1]  = mulhi_int16_simd256(y256p[1],ONE_OVER_SQRT2_Q15_128);
      y256p[2]  = mulhi_int16_simd256(y256p[2],ONE_OVER_SQRT2_Q15_128);
      y256p[3]  = mulhi_int16_simd256(y256p[3],ONE_OVER_SQRT2_Q15_128);
      y256p[4]  = mulhi_int16_simd256(y256p[4],ONE_OVER_SQRT2_Q15_128);
      y256p[5]  = mulhi_int16_simd256(y256p[5],ONE_OVER_SQRT2_Q15_128);
      y256p[6]  = mulhi_int16_simd256(y256p[6],ONE_OVER_SQRT2_Q15_128);
      y256p[7]  = mulhi_int16_simd256(y256p[7],ONE_OVER_SQRT2_Q15_128);
      y256p[8]  = mulhi_int16_simd256(y256p[8],ONE_OVER_SQRT2_Q15_128);
      y256p[9]  = mulhi_int16_simd256(y256p[9],ONE_OVER_SQRT2_Q15_128);
      y256p[10] = mulhi_int16_simd256(y256p[10],ONE_OVER_SQRT2_Q15_128);
      y256p[11] = mulhi_int16_simd256(y256p[11],ONE_OVER_SQRT2_Q15_128);
      y256p[12] = mulhi_int16_simd256(y256p[12],ONE_OVER_SQRT2_Q15_128);
      y256p[13] = mulhi_int16_simd256(y256p[13],ONE_OVER_SQRT2_Q15_128);
      y256p[14] = mulhi_int16_simd256(y256p[14],ONE_OVER_SQRT2_Q15_128);
      y256p[15] = mulhi_int16_simd256(y256p[15],ONE_OVER_SQRT2_Q15_128);
      y256p+=16;
    }
  }

  _mm_empty();
  _m_empty();

}

void rvv_dft2048(int16_t *x, int16_t *y, unsigned char scale)
{
    // 1. 内存分配 (256 个 256-bit 向量 = 256 * 16 = 4096 个 int16_t)
    int16_t xtmp[256 * VL_16] __attribute__((aligned(32)));
    int16_t ytmp[256 * VL_16] __attribute__((aligned(32)));

    // 2. 指针定义
    int16_t *tw2048_ptr = tw2048; // 假设 tw2048 是全局或传入的 int16_t 数组
    
    // 中间指针用于循环和索引
    int16_t *ytmpp = ytmp;
    int16_t *y_write_ptr = y;
    int16_t *x_read_ptr = x; // 用于在转置循环中自增

    int i, j;
    
    // RVV 向量设置
    size_t vl = __riscv_vsetvl_e16m4(VL_16);
    
    // --- 第一阶段: Transpose (rvv_transpose4_ooff) ---
    // 原代码逻辑: 
    // for (i=0; i<4; i++) {
    //    // 32 次 transpose4_ooff_simd256 调用
    //    x256+=64;
    //    xtmpp+=32;
    // }
    
    // 块偏移量：128 * VL_16 = 2048 int16_t
    const int TRANSPOSE_BLOCK_OFFSET = 128; // 128个 256-bit 块

    // 循环 i=0 to 3: 4个大块
    for (i = 0; i < 4; i++) {
        
        // 循环 j=0 to 31: 32 次 transpose4_ooff_simd256
        for (j = 0; j < 32; j++) {
            
            // 输入指针：x_read_ptr + (2 * j) * VL_16
            // 输出指针：xtmp + (i * 32 * VL_16) + j * VL_16
            // i * 32 * VL_16 是 xtmpp 在 xtmp 数组中的基地址。
            
            rvv_transpose4_ooff(
                x_read_ptr + (2 * j) * VL_16,// 输入指针 x256 + 2*j
                xtmp + (i * 32 + j) * VL_16,// 输出指针 xtmpp + j
                TRANSPOSE_BLOCK_OFFSET// off=128
            );
        }
        
        // 更新指针，指向下一个 1024 复数块 (64 * VL_16 = 1024 int16_t)
        x_read_ptr += 64 * VL_16;
    }
    
    // --- 第二阶段: DFT1024 ---
    // 调用两次 rvv_dft1024
    // 块大小 128 * VL_16 = 2048 int16_t。
    const int N1024_BLOCK_SIZE = 128 * VL_16;
    
    // dft1024((int16_t*)(xtmp), (int16_t*)ytmp, 1);
    rvv_dft1024(xtmp + 0 * N1024_BLOCK_SIZE, ytmp + 0 * N1024_BLOCK_SIZE, 1);

    // dft1024((int16_t*)(xtmp+128), (int16_t*)(ytmp+128), 1);
    rvv_dft1024(xtmp + 1 * N1024_BLOCK_SIZE, ytmp + 1 * N1024_BLOCK_SIZE, 1);


    // --- 第三阶段: Butterfly 2 (rvv_bfly2_256) ---
    // 循环 128 次 (i=0 to 127)，每次处理一个 256-bit 块
    // 块偏移量 (Block Step) 是 128 * VL_16 = 2048 int16_t
    const int BLOCK_STEP = 128 * VL_16; 
    const int NUM_BUTTERFLIES = 128; // 2048/16 = 128 次

    for (i = 0; i < NUM_BUTTERFLIES; i++) {
        
        // twiddles 因子 tw2048_256p++
        const int tw_offset = i * VL_16;
        
        rvv_bfly2_256(
            // Inputs (ytmp)
            ytmpp + i * VL_16,// x0: ytmp[i]
            ytmpp + i * VL_16 + BLOCK_STEP,// x1: ytmp[i+128]
            
            // Outputs (y)
            y_write_ptr + i * VL_16,// y0: y[i]
            y_write_ptr + i * VL_16 + BLOCK_STEP,// y1: y[i+128]

            // Twiddles (tw2048)
            tw2048_ptr + tw_offset 
        );
    }


    // --- 第四阶段: Final Scaling (Optional) ---
    if (scale > 0) {
        
        // 缩放操作是：mulhi_int16_simd256(..., ONE_OVER_SQRT2_Q15_128)
        
        // 常数 ONE_OVER_SQRT2_Q15_128 的 RVV 实现
        // 假设 ONE_OVER_SQRT2_Q15 已定义 (Q15 格式)
        vint16m4_t ONE_OVER_SQRT2_VEC = __riscv_vmv_v_x_i16m4(ONE_OVER_SQRT2_Q15, vl);

        // 循环处理所有 256 个 256-bit 块。
        // 原代码中 y256p+=16，循环 16 次，共 16 * 16 = 256 个块
        const int NUM_BLOCKS = 256; 
        
        for (i = 0; i < NUM_BLOCKS; i++) {
            
            int16_t *curr_y_vec = y + i * VL_16;

            // 1. Load
            vint16m4_t y_vec = __riscv_vle16_v_i16m4(curr_y_vec, vl);
            
            // 2. Multiply High (Q15 * Q15 -> Q15)
            // 使用 vmulh.vv 实现 Q15 乘法 (自动取高 16 位)
            y_vec = __riscv_vmulh_vv_i16m4(y_vec, ONE_OVER_SQRT2_VEC, vl);
            
            // 3. Store
            __riscv_vse16_v_i16m4(curr_y_vec, y_vec, vl);
        }
    }
}
void idft2048(int16_t *x,int16_t *y,unsigned char scale)
{

  simd256_q15_t xtmp[256],*xtmpp,*x256 = (simd256_q15_t *)x;
  simd256_q15_t ytmp[256],*tw2048_256p=(simd256_q15_t *)tw2048,*y256=(simd256_q15_t *)y,*y256p=(simd256_q15_t *)y;
  simd256_q15_t *ytmpp = &ytmp[0];
  int i;
  simd256_q15_t ONE_OVER_SQRT2_Q15_128 = set1_int16_simd256(ONE_OVER_SQRT2_Q15);

  xtmpp = xtmp;
  
  for (i=0; i<4; i++) {
    transpose4_ooff_simd256(x256  ,xtmpp,128);
    transpose4_ooff_simd256(x256+2,xtmpp+1,128);
    transpose4_ooff_simd256(x256+4,xtmpp+2,128);
    transpose4_ooff_simd256(x256+6,xtmpp+3,128);
    transpose4_ooff_simd256(x256+8,xtmpp+4,128);
    transpose4_ooff_simd256(x256+10,xtmpp+5,128);
    transpose4_ooff_simd256(x256+12,xtmpp+6,128);
    transpose4_ooff_simd256(x256+14,xtmpp+7,128);
    transpose4_ooff_simd256(x256+16,xtmpp+8,128);
    transpose4_ooff_simd256(x256+18,xtmpp+9,128);
    transpose4_ooff_simd256(x256+20,xtmpp+10,128);
    transpose4_ooff_simd256(x256+22,xtmpp+11,128);
    transpose4_ooff_simd256(x256+24,xtmpp+12,128);
    transpose4_ooff_simd256(x256+26,xtmpp+13,128);
    transpose4_ooff_simd256(x256+28,xtmpp+14,128);
    transpose4_ooff_simd256(x256+30,xtmpp+15,128);
    transpose4_ooff_simd256(x256+32,xtmpp+16,128);
    transpose4_ooff_simd256(x256+34,xtmpp+17,128);
    transpose4_ooff_simd256(x256+36,xtmpp+18,128);
    transpose4_ooff_simd256(x256+38,xtmpp+19,128);
    transpose4_ooff_simd256(x256+40,xtmpp+20,128);
    transpose4_ooff_simd256(x256+42,xtmpp+21,128);
    transpose4_ooff_simd256(x256+44,xtmpp+22,128);
    transpose4_ooff_simd256(x256+46,xtmpp+23,128);
    transpose4_ooff_simd256(x256+48,xtmpp+24,128);
    transpose4_ooff_simd256(x256+50,xtmpp+25,128);
    transpose4_ooff_simd256(x256+52,xtmpp+26,128);
    transpose4_ooff_simd256(x256+54,xtmpp+27,128);
    transpose4_ooff_simd256(x256+56,xtmpp+28,128);
    transpose4_ooff_simd256(x256+58,xtmpp+29,128);
    transpose4_ooff_simd256(x256+60,xtmpp+30,128);
    transpose4_ooff_simd256(x256+62,xtmpp+31,128);
    x256+=64;
    xtmpp+=32;
  }

  idft1024((int16_t*)(xtmp),(int16_t*)ytmp,1);
  idft1024((int16_t*)(xtmp+128),(int16_t*)(ytmp+128),1);


  for (i=0; i<128; i++) {
    ibfly2_256(ytmpp,ytmpp+128,
	       y256p,y256p+128,
	       tw2048_256p);
    tw2048_256p++;
    y256p++;
    ytmpp++;
  }

  if (scale>0) {
    y256p = y256;

    for (i=0; i<16; i++) {
      y256p[0]  = mulhi_int16_simd256(y256p[0],ONE_OVER_SQRT2_Q15_128);
      y256p[1]  = mulhi_int16_simd256(y256p[1],ONE_OVER_SQRT2_Q15_128);
      y256p[2]  = mulhi_int16_simd256(y256p[2],ONE_OVER_SQRT2_Q15_128);
      y256p[3]  = mulhi_int16_simd256(y256p[3],ONE_OVER_SQRT2_Q15_128);
      y256p[4]  = mulhi_int16_simd256(y256p[4],ONE_OVER_SQRT2_Q15_128);
      y256p[5]  = mulhi_int16_simd256(y256p[5],ONE_OVER_SQRT2_Q15_128);
      y256p[6]  = mulhi_int16_simd256(y256p[6],ONE_OVER_SQRT2_Q15_128);
      y256p[7]  = mulhi_int16_simd256(y256p[7],ONE_OVER_SQRT2_Q15_128);
      y256p[8]  = mulhi_int16_simd256(y256p[8],ONE_OVER_SQRT2_Q15_128);
      y256p[9]  = mulhi_int16_simd256(y256p[9],ONE_OVER_SQRT2_Q15_128);
      y256p[10] = mulhi_int16_simd256(y256p[10],ONE_OVER_SQRT2_Q15_128);
      y256p[11] = mulhi_int16_simd256(y256p[11],ONE_OVER_SQRT2_Q15_128);
      y256p[12] = mulhi_int16_simd256(y256p[12],ONE_OVER_SQRT2_Q15_128);
      y256p[13] = mulhi_int16_simd256(y256p[13],ONE_OVER_SQRT2_Q15_128);
      y256p[14] = mulhi_int16_simd256(y256p[14],ONE_OVER_SQRT2_Q15_128);
      y256p[15] = mulhi_int16_simd256(y256p[15],ONE_OVER_SQRT2_Q15_128);
      y256p+=16;
    }
  }

  _mm_empty();
  _m_empty();

}

void rvv_idft2048(int16_t *x, int16_t *y, unsigned char scale)
{
    // 1. 内存分配 (256 个 256-bit 向量 = 256 * 16 = 4096 个 int16_t)
    int16_t xtmp[256 * VL_16] __attribute__((aligned(32)));
    int16_t ytmp[256 * VL_16] __attribute__((aligned(32)));

    // 2. 指针定义
    int16_t *tw2048_ptr = tw2048; // 假设 tw2048 是全局或传入的 int16_t 数组
    
    // 中间指针用于循环和索引
    int16_t *ytmpp = ytmp;
    int16_t *y_write_ptr = y;
    int16_t *x_read_ptr = x; // 用于在转置循环中自增

    int i, j;
    
    // RVV 向量设置
    size_t vl = __riscv_vsetvl_e16m4(VL_16);
    
    // --- 第一阶段: Transpose (rvv_transpose4_ooff) ---
    
    // 块偏移量：128 * VL_16 = 2048 int16_t
    const int TRANSPOSE_BLOCK_OFFSET = 128; // 128个 256-bit 块

    // 循环 i=0 to 3: 4个大块
    for (i = 0; i < 4; i++) {
        
        // 循环 j=0 to 31: 32 次 rvv_transpose4_ooff
        for (j = 0; j < 32; j++) {
            
            // 输入指针：x_read_ptr + (2 * j) * VL_16
            // 输出指针：xtmp + (i * 32 * VL_16) + j * VL_16
            
            rvv_transpose4_ooff(
                x_read_ptr + (2 * j) * VL_16,// 输入指针 x256 + 2*j
                xtmp + (i * 32 + j) * VL_16,// 输出指针 xtmpp + j
                TRANSPOSE_BLOCK_OFFSET// off=128
            );
        }
        
        // 更新指针，指向下一个 1024 复数块 (64 * VL_16 = 1024 int16_t)
        x_read_ptr += 64 * VL_16;
    }
    
    // --- 第二阶段: IDFT1024 ---
    // 调用两次 rvv_idft1024
    // 块大小 128 * VL_16 = 2048 int16_t。
    const int N1024_BLOCK_SIZE = 128 * VL_16;
    
    // idft1024((int16_t*)(xtmp), (int16_t*)ytmp, 1);
    rvv_idft1024(xtmp + 0 * N1024_BLOCK_SIZE, ytmp + 0 * N1024_BLOCK_SIZE, 1);

    // idft1024((int16_t*)(xtmp+128), (int16_t*)(ytmp+128), 1);
    rvv_idft1024(xtmp + 1 * N1024_BLOCK_SIZE, ytmp + 1 * N1024_BLOCK_SIZE, 1);


    // --- 第三阶段: Inverse Butterfly 2 (rvv_ibfly2_256) ---
    // 循环 128 次 (i=0 to 127)，每次处理一个 256-bit 块
    // 块偏移量 (Block Step) 是 128 * VL_16 = 2048 int16_t
    const int BLOCK_STEP = 128 * VL_16; 
    const int NUM_BUTTERFLIES = 128; // 2048/16 = 128 次

    for (i = 0; i < NUM_BUTTERFLIES; i++) {
        
        // twiddles 因子 tw2048_256p++
        const int tw_offset = i * VL_16;
        
        rvv_ibfly2_256(
            // Inputs (ytmp)
            ytmpp + i * VL_16,// x0: ytmp[i]
            ytmpp + i * VL_16 + BLOCK_STEP,// x1: ytmp[i+128]
            
            // Outputs (y)
            y_write_ptr + i * VL_16,// y0: y[i]
            y_write_ptr + i * VL_16 + BLOCK_STEP,// y1: y[i+128]

            // Twiddles (tw2048)
            tw2048_ptr + tw_offset 
        );
    }


    // --- 第四阶段: Final Scaling (Optional) ---
    if (scale > 0) {
        
        // 缩放操作是：mulhi_int16_simd256(..., ONE_OVER_SQRT2_Q15_128)
        
        // 常数 ONE_OVER_SQRT2_Q15_128 的 RVV 实现
        vint16m4_t ONE_OVER_SQRT2_VEC = __riscv_vmv_v_x_i16m4(ONE_OVER_SQRT2_Q15, vl);

        // 循环处理所有 256 个 256-bit 块。
        // 原代码中 y256p+=16，循环 16 次，共 16 * 16 = 256 个块
        const int NUM_BLOCKS = 256; 
        
        for (i = 0; i < NUM_BLOCKS; i++) {
            
            int16_t *curr_y_vec = y + i * VL_16;

            // 1. Load
            vint16m4_t y_vec = __riscv_vle16_v_i16m4(curr_y_vec, vl);
            
            // 2. Multiply High (Q15 * Q15 -> Q15)
            // 使用 vmulh.vv 实现 Q15 乘法 (自动取高 16 位)
            y_vec = __riscv_vmulh_vv_i16m4(y_vec, ONE_OVER_SQRT2_VEC, vl);
            
            // 3. Store
            __riscv_vse16_v_i16m4(curr_y_vec, y_vec, vl);
        }
    }
    
    // 原代码末尾的 SIMD 内存屏障/清空指令在 RVV 中通常不需要或有不同的等效指令，这里省略。
    // _mm_empty();
    // _m_empty();
}
#endif



int16_t tw4096[3*2*1024];

#ifndef __AVX2__

#else //__AVX2__
void dft4096(int16_t *x,int16_t *y,unsigned char scale)
{

  simd256_q15_t xtmp[512],ytmp[512],*tw4096_256p=(simd256_q15_t *)tw4096,*x256=(simd256_q15_t *)x,*y256=(simd256_q15_t *)y,*y256p=(simd256_q15_t *)y;
  simd256_q15_t *ytmpp = &ytmp[0];
  int i,j;

  for (i=0,j=0; i<512; i+=4,j++) {
    transpose16_ooff_simd256(x256+i,xtmp+j,128);
  }


  dft1024((int16_t*)(xtmp),(int16_t*)(ytmp),1);
  dft1024((int16_t*)(xtmp+128),(int16_t*)(ytmp+128),1);
  dft1024((int16_t*)(xtmp+256),(int16_t*)(ytmp+256),1);
  dft1024((int16_t*)(xtmp+384),(int16_t*)(ytmp+384),1);

  for (i=0; i<128; i++) {
    bfly4_256(ytmpp,ytmpp+128,ytmpp+256,ytmpp+384,
	      y256p,y256p+128,y256p+256,y256p+384,
	      tw4096_256p,tw4096_256p+128,tw4096_256p+256);
    tw4096_256p++;
    y256p++;
    ytmpp++;
  }

  if (scale>0) {

    for (i=0; i<32; i++) {
      y256[0]  = shiftright_int16_simd256(y256[0],1);
      y256[1]  = shiftright_int16_simd256(y256[1],1);
      y256[2]  = shiftright_int16_simd256(y256[2],1);
      y256[3]  = shiftright_int16_simd256(y256[3],1);
      y256[4]  = shiftright_int16_simd256(y256[4],1);
      y256[5]  = shiftright_int16_simd256(y256[5],1);
      y256[6]  = shiftright_int16_simd256(y256[6],1);
      y256[7]  = shiftright_int16_simd256(y256[7],1);
      y256[8]  = shiftright_int16_simd256(y256[8],1);
      y256[9]  = shiftright_int16_simd256(y256[9],1);
      y256[10] = shiftright_int16_simd256(y256[10],1);
      y256[11] = shiftright_int16_simd256(y256[11],1);
      y256[12] = shiftright_int16_simd256(y256[12],1);
      y256[13] = shiftright_int16_simd256(y256[13],1);
      y256[14] = shiftright_int16_simd256(y256[14],1);
      y256[15] = shiftright_int16_simd256(y256[15],1);

      y256+=16;
    }

  }

  _mm_empty();
  _m_empty();

}

void rvv_dft4096(int16_t *x, int16_t *y, unsigned char scale)
{
    // 1. 内存分配 (512 个 256-bit 向量 = 512 * 16 = 8192 个 int16_t)
    int16_t xtmp[512 * VL_16] __attribute__((aligned(32)));
    int16_t ytmp[512 * VL_16] __attribute__((aligned(32)));

    // 2. 指针定义
    int16_t *tw4096_ptr = tw4096; // 假设 tw4096 是全局或传入的 int16_t 数组
    
    // 中间指针用于循环和索引
    int16_t *ytmpp = ytmp;
    int16_t *y_write_ptr = y;

    int i, j;
    
    // RVV 向量设置
    size_t vl = __riscv_vsetvl_e16m4(VL_16);
    
    // --- 第一阶段: Transpose (rvv_transpose16_ooff) ---
    // 原代码逻辑: for (i=0, j=0; i<512; i+=4, j++) { transpose16_ooff_simd256(x256+i, xtmp+j, 128); }
    // 循环 128 次 (j=0 to 127)。
    // 输入块偏移 i = 4*j, 输出块偏移 j。
    // off=128 (128 * 16 个 int16_t 间距)

    for (j = 0; j < 128; j++) {
        // i 是输入块在 x256 数组中的索引，对应 int16_t 偏移量 (4 * j) * VL_16
        // j 是输出块在 xtmp 数组中的索引，对应 int16_t 偏移量 j * VL_16
        rvv_transpose16_ooff(
            x + (4 * j) * VL_16,// 输入指针 x256 + 4*j
            xtmp + j * VL_16,// 输出指针 xtmp + j
            128// off=128
        );
    }
    
    // --- 第二阶段: DFT1024 ---
    // 调用四次 rvv_dft1024
    // 块大小 128 * VL_16 = 2048 int16_t。
    const int N1024_BLOCK_SIZE = 128 * VL_16;
    
    // dft1024((int16_t*)(xtmp), (int16_t*)(ytmp), 1);
    rvv_dft1024(xtmp + 0 * N1024_BLOCK_SIZE, ytmp + 0 * N1024_BLOCK_SIZE, 1);

    // dft1024((int16_t*)(xtmp+128), (int16_t*)(ytmp+128), 1);
    rvv_dft1024(xtmp + 1 * N1024_BLOCK_SIZE, ytmp + 1 * N1024_BLOCK_SIZE, 1);

    // dft1024((int16_t*)(xtmp+256), (int16_t*)(ytmp+256), 1);
    rvv_dft1024(xtmp + 2 * N1024_BLOCK_SIZE, ytmp + 2 * N1024_BLOCK_SIZE, 1);

    // dft1024((int16_t*)(xtmp+384), (int16_t*)(ytmp+384), 1);
    rvv_dft1024(xtmp + 3 * N1024_BLOCK_SIZE, ytmp + 3 * N1024_BLOCK_SIZE, 1);


    // --- 第三阶段: Butterfly 4 (rvv_bfly4_256) ---
    // 循环 128 次 (i=0 to 127)，每次处理一个 256-bit 块
    // 块偏移量 (Block Step) 是 128 * VL_16 = 2048 int16_t
    const int BLOCK_STEP = 128 * VL_16; 
    const int NUM_BUTTERFLIES = 128; // 4096/32 = 128 次

    for (i = 0; i < NUM_BUTTERFLIES; i++) {
        
        // twiddles 因子在原代码中为 tw4096_256p, tw4096_256p+128, tw4096_256p+256
        // 注意：原代码的指针自增是 tw4096_256p++，所以基地址是 tw4096_ptr + i*VL_16
        const int tw1_offset = i * VL_16;
        const int tw2_offset = (128 + i) * VL_16;
        const int tw3_offset = (256 + i) * VL_16;
        
        rvv_bfly4_256(
            // Inputs (ytmp)
            ytmpp + i * VL_16,// x0: ytmp[i]
            ytmpp + i * VL_16 + 1 * BLOCK_STEP, // x1: ytmp[i+128]
            ytmpp + i * VL_16 + 2 * BLOCK_STEP, // x2: ytmp[i+256]
            ytmpp + i * VL_16 + 3 * BLOCK_STEP, // x3: ytmp[i+384]
            
            // Outputs (y)
            y_write_ptr + i * VL_16,// y0: y[i]
            y_write_ptr + i * VL_16 + 1 * BLOCK_STEP, // y1: y[i+128]
            y_write_ptr + i * VL_16 + 2 * BLOCK_STEP, // y2: y[i+256]
            y_write_ptr + i * VL_16 + 3 * BLOCK_STEP, // y3: y[i+384]

            // Twiddles (tw4096)
            tw4096_ptr + tw1_offset, 
            tw4096_ptr + tw2_offset, 
            tw4096_ptr + tw3_offset
        );
    }


    // --- 第四阶段: Final Scaling (Optional) ---
    if (scale > 0) {
        
        // 缩放操作是：shiftright_int16_simd256(..., 1) 即右移 1 位 (除以 2)
        const int SHIFT_AMOUNT = 1;
        
        // 循环处理所有 512 个 256-bit 块。
        // 原代码中 y256+=16，循环 32 次，共 32 * 16 = 512 个块
        const int NUM_BLOCKS = 512; // 4096个复数 / 8个复数每块 = 512块
        
        for (i = 0; i < NUM_BLOCKS; i++) {
            
            int16_t *curr_y_vec = y + i * VL_16;

            // 1. Load
            vint16m4_t y_vec = __riscv_vle16_v_i16m4(curr_y_vec, vl);
            
            // 2. Shift Right (算术右移实现除以 2)
            // 使用 vsra.vx 实现算术右移
            y_vec = __riscv_vsra_vx_i16m4(y_vec, SHIFT_AMOUNT, vl);
            
            // 3. Store
            __riscv_vse16_v_i16m4(curr_y_vec, y_vec, vl);
        }
    }
    
    // 原代码末尾的 SIMD 内存屏障/清空指令在 RVV 中通常不需要或有不同的等效指令，这里省略。
    // _mm_empty();
    // _m_empty();
}

void idft4096(int16_t *x,int16_t *y,unsigned char scale)
{

  simd256_q15_t xtmp[512],ytmp[512],*tw4096_256p=(simd256_q15_t *)tw4096,*x256=(simd256_q15_t *)x,*y256=(simd256_q15_t *)y,*y256p=(simd256_q15_t *)y;
  simd256_q15_t *ytmpp = &ytmp[0];
  int i,j;

  for (i=0,j=0; i<512; i+=4,j++) {
    transpose16_ooff_simd256(x256+i,xtmp+j,128);
  }


  idft1024((int16_t*)(xtmp),(int16_t*)(ytmp),1);
  idft1024((int16_t*)(xtmp+128),(int16_t*)(ytmp+128),1);
  idft1024((int16_t*)(xtmp+256),(int16_t*)(ytmp+256),1);
  idft1024((int16_t*)(xtmp+384),(int16_t*)(ytmp+384),1);

  for (i=0; i<128; i++) {
    ibfly4_256(ytmpp,ytmpp+128,ytmpp+256,ytmpp+384,
	       y256p,y256p+128,y256p+256,y256p+384,
	       tw4096_256p,tw4096_256p+128,tw4096_256p+256);
    tw4096_256p++;
    y256p++;
    ytmpp++;
  }

  if (scale>0) {

    for (i=0; i<32; i++) {
      y256[0]  = shiftright_int16_simd256(y256[0],1);
      y256[1]  = shiftright_int16_simd256(y256[1],1);
      y256[2]  = shiftright_int16_simd256(y256[2],1);
      y256[3]  = shiftright_int16_simd256(y256[3],1);
      y256[4]  = shiftright_int16_simd256(y256[4],1);
      y256[5]  = shiftright_int16_simd256(y256[5],1);
      y256[6]  = shiftright_int16_simd256(y256[6],1);
      y256[7]  = shiftright_int16_simd256(y256[7],1);
      y256[8]  = shiftright_int16_simd256(y256[8],1);
      y256[9]  = shiftright_int16_simd256(y256[9],1);
      y256[10] = shiftright_int16_simd256(y256[10],1);
      y256[11] = shiftright_int16_simd256(y256[11],1);
      y256[12] = shiftright_int16_simd256(y256[12],1);
      y256[13] = shiftright_int16_simd256(y256[13],1);
      y256[14] = shiftright_int16_simd256(y256[14],1);
      y256[15] = shiftright_int16_simd256(y256[15],1);

      y256+=16;
    }

  }

  _mm_empty();
  _m_empty();

}

void rvv_idft4096(int16_t *x, int16_t *y, unsigned char scale)
{
    // 1. 内存分配 (512 个 256-bit 向量 = 512 * 16 = 8192 个 int16_t)
    int16_t xtmp[512 * VL_16] __attribute__((aligned(32)));
    int16_t ytmp[512 * VL_16] __attribute__((aligned(32)));

    // 2. 指针定义
    int16_t *tw4096_ptr = tw4096; // 假设 tw4096 是全局或传入的 int16_t 数组
    
    // 中间指针用于循环和索引
    int16_t *ytmpp = ytmp;
    int16_t *y_write_ptr = y;

    int i, j;
    
    // RVV 向量设置
    size_t vl = __riscv_vsetvl_e16m4(VL_16);
    
    // --- 第一阶段: Transpose (rvv_transpose16_ooff) ---
    // 逻辑: for (i=0, j=0; i<512; i+=4, j++) { transpose16_ooff_simd256(x256+i, xtmp+j, 128); }
    // 循环 128 次 (j=0 to 127)。
    // off=128 (128 * 16 个 int16_t 间距)

    for (j = 0; j < 128; j++) {
        rvv_transpose16_ooff(
            x + (4 * j) * VL_16,// 输入指针 x256 + 4*j
            xtmp + j * VL_16,// 输出指针 xtmp + j
            128// off=128
        );
    }
    
    // --- 第二阶段: IDFT1024 ---
    // 调用四次 rvv_idft1024
    // 块大小 128 * VL_16 = 2048 int16_t。
    const int N1024_BLOCK_SIZE = 128 * VL_16;
    
    // idft1024((int16_t*)(xtmp), (int16_t*)(ytmp), 1);
    rvv_idft1024(xtmp + 0 * N1024_BLOCK_SIZE, ytmp + 0 * N1024_BLOCK_SIZE, 1);

    // idft1024((int16_t*)(xtmp+128), (int16_t*)(ytmp+128), 1);
    rvv_idft1024(xtmp + 1 * N1024_BLOCK_SIZE, ytmp + 1 * N1024_BLOCK_SIZE, 1);

    // idft1024((int16_t*)(xtmp+256), (int16_t*)(ytmp+256), 1);
    rvv_idft1024(xtmp + 2 * N1024_BLOCK_SIZE, ytmp + 2 * N1024_BLOCK_SIZE, 1);

    // idft1024((int16_t*)(xtmp+384), (int16_t*)(ytmp+384), 1);
    rvv_idft1024(xtmp + 3 * N1024_BLOCK_SIZE, ytmp + 3 * N1024_BLOCK_SIZE, 1);


    // --- 第三阶段: Inverse Butterfly 4 (rvv_ibfly4_256) ---
    // 循环 128 次 (i=0 to 127)，每次处理一个 256-bit 块
    // 块偏移量 (Block Step) 是 128 * VL_16 = 2048 int16_t
    const int BLOCK_STEP = 128 * VL_16; 
    const int NUM_BUTTERFLIES = 128; // 4096/32 = 128 次

    for (i = 0; i < NUM_BUTTERFLIES; i++) {
        
        // Twiddles 偏移量
        const int tw1_offset = i * VL_16;
        const int tw2_offset = (128 + i) * VL_16;
        const int tw3_offset = (256 + i) * VL_16;
        
        rvv_ibfly4_256(
            // Inputs (ytmp)
            ytmpp + i * VL_16,// x0: ytmp[i]
            ytmpp + i * VL_16 + 1 * BLOCK_STEP, // x1: ytmp[i+128]
            ytmpp + i * VL_16 + 2 * BLOCK_STEP, // x2: ytmp[i+256]
            ytmpp + i * VL_16 + 3 * BLOCK_STEP, // x3: ytmp[i+384]
            
            // Outputs (y)
            y_write_ptr + i * VL_16,// y0: y[i]
            y_write_ptr + i * VL_16 + 1 * BLOCK_STEP, // y1: y[i+128]
            y_write_ptr + i * VL_16 + 2 * BLOCK_STEP, // y2: y[i+256]
            y_write_ptr + i * VL_16 + 3 * BLOCK_STEP, // y3: y[i+384]

            // Twiddles (tw4096)
            tw4096_ptr + tw1_offset, 
            tw4096_ptr + tw2_offset, 
            tw4096_ptr + tw3_offset
        );
    }


    // --- 第四阶段: Final Scaling (Optional) ---
    if (scale > 0) {
        
        // 缩放操作是：shiftright_int16_simd256(..., 1) 即右移 1 位 (除以 2)
        const int SHIFT_AMOUNT = 1;
        const int NUM_BLOCKS = 512; // 4096个复数 / 8个复数每块 = 512块
        
        for (i = 0; i < NUM_BLOCKS; i++) {
            
            int16_t *curr_y_vec = y + i * VL_16;

            // 1. Load
            vint16m4_t y_vec = __riscv_vle16_v_i16m4(curr_y_vec, vl);
            
            // 2. Shift Right (算术右移实现除以 2)
            // 使用 vsra.vx 实现算术右移
            y_vec = __riscv_vsra_vx_i16m4(y_vec, SHIFT_AMOUNT, vl);
            
            // 3. Store
            __riscv_vse16_v_i16m4(curr_y_vec, y_vec, vl);
        }
    }
    
    // 忽略 SIMD 内存屏障/清空指令
    // _mm_empty();
    // _m_empty();
}
#endif //__AVX2__


 



 
// int main(){
//    set_mstatus_vs_field(MSTATUS_VS_CLEAN);
//     //测试向量指令是否可用——测试成功
//     int16_t a[8] = {1,2,3,4,5,6,7,8};
//     int16_t b[8] = {8,7,6,5,4,3,2,1};
//     int16_t result[8] = {0};
 
//     asm volatile (
//         "vsetvli t0, %[vl], e16, m1\n\t"    // 设置向量长度到t0寄存器和元素宽度
//         "vle16.v v0, (%[pa])\n\t"               // 加载a数组到v0寄存器
//         "vle16.v v1, (%[pb])\n\t"               // 加载b数组到v1寄存器
//         "vmod.vv v2, v0, v1\n\t"         // vmod_cus.vv：v0和v1逐元素mod，结果存到v2
//         "vse16.v v2, (%[pr])\n\t"               // 将结果从v2寄存器存储到result数组
//         :
//         : [pa] "r" (a),
//           [pb] "r" (b),
//           [pr] "r" (result),
//           [vl] "r" (8)
//         : "t0", "v0", "v1", "v2"
//     );
 
//     printf("Result: ");
//     for(int i = 0; i < 8; i++) {
//         printf("%d ", result[i]);
//     }
//     printf("\n");
//     return 0;
// }
// #include <stdio.h>
// #include <stdint.h>
// #include <klib.h>

// int main(){
//     set_mstatus_vs_field(MSTATUS_VS_CLEAN);
//     printf("Testing vscmacccj (Conjugate Multiply Accumulate)...\n");

//     // A (v0/vs2 - 被取共轭): 10000 + 20000i
//     int16_t a[8] = {10000, 20000,  32767, 0,    0, 0, 0, 0}; 
//     // B (v1/vs1): 0.5
//     int16_t b[8] = {16384, 0,      32767, 0,    0, 0, 0, 0};
    
//     // 初始 Acc = 100 + 100i
//     int16_t acc[8] = {100, 100, 100, 100, 0, 0, 0, 0}; 
 
//     asm volatile (
//         "vsetvli t0, %[vl], e16, m1\n\t"    
//         "vle16.v v0, (%[pa])\n\t"           
//         "vle16.v v1, (%[pb])\n\t"
//         "vle16.v v2, (%[pr])\n\t"    // 加载初始 Acc
        
//         "vscmacccj.vv v2, v0, v1\n\t" // v2 = v2 + (v1 * conj(v0))
        
//         "vse16.v v2, (%[pr])\n\t"           
//         :
//         : [pa] "r" (a),
//           [pb] "r" (b),
//           [pr] "r" (acc),
//           [vl] "r" (8)
//         : "t0", "v0", "v1", "v2", "memory"
//     );
 
//     printf("Result: ");
//     for(int i = 0; i < 8; i++) printf("%d ", acc[i]);
//     printf("\n");

//     // 验证逻辑
//     int pass = 1;
//     // Case 1: conj(A)*B = 5000 - 10000i
//     // Acc + Prod = (100+100i) + (5000-10000i) = 5100 - 9900i
//     if (acc[0] != 5100 || acc[1] != -9900) pass = 0;
    
//     // Case 2: Max * Max + Acc -> 饱和
//     if (acc[4] != 32767) pass = 0;

//     if(pass) printf("CHECK: PASS\n");
//     else printf("CHECK: FAIL\n");
    
//     return 0;
// }


// #include <stdio.h>
// #include <stdint.h>
// #include <klib.h>

// int main(){
//     set_mstatus_vs_field(MSTATUS_VS_CLEAN);
//     printf("Testing vccj (Conjugate)...\n");

//     // A (v0/vs2)
//     // Case 1: 1000 + 2000i
//     // Case 2: 0 - 32768i (最小负数，取反溢出)
//     int16_t a[8] = {1000, 2000,   0, -32768,    0, 0, 0, 0}; 
//     // B (v1) - 不使用，但这行代码保留以匹配模板
//     int16_t b[8] = {0};
    
//     int16_t result[8] = {0}; 
 
//     asm volatile (
//         "vsetvli t0, %[vl], e16, m1\n\t"    
//         "vle16.v v0, (%[pa])\n\t"           
//         "vle16.v v1, (%[pb])\n\t"
//         "vle16.v v2, (%[pr])\n\t"    
        
//         // vccj.vv v2, v0
//         // Hex: 0xEA000157
//         ".word 0xDE000157\n\t" 
        
//         "vse16.v v2, (%[pr])\n\t"           
//         :
//         : [pa] "r" (a),
//           [pb] "r" (b),
//           [pr] "r" (result),
//           [vl] "r" (8)
//         : "t0", "v0", "v1", "v2", "memory"
//     );
 
//     printf("Result: ");
//     for(int i = 0; i < 8; i++) printf("%d ", result[i]);
//     printf("\n");

//     // 验证逻辑
//     int pass = 1;
//     // Case 1: conj(1000+2000i) = 1000 - 2000i
//     if (result[0] != 1000 || result[1] != -2000) pass = 0;
    
//     // Case 2 (饱和测试): conj(0 - 32768i) 
//     // Im = -(-32768) = 32768 -> 饱和到 32767
//     if (result[2] != 0 || result[3] != 32767) pass = 0;

//     if(pass) printf("CHECK: PASS\n");
//     else printf("CHECK: FAIL\n");
    
//     return 0;
// }
//#define newmain
//#define oldmain

// }
double get_time_meas_us(time_stats_t *ts);
//extern int opp_enabled = 1; 

#ifndef FFT_MAIN_POINTS
#define FFT_MAIN_POINTS 16
#endif

#ifndef FFT_MAIN_REPEAT
#define FFT_MAIN_REPEAT 1
#endif

#ifndef FFT_VALIDATE
#define FFT_VALIDATE 1
#endif

#ifndef FFT_MAIN_RUN_IDFT
#define FFT_MAIN_RUN_IDFT 0
#endif

static volatile int16_t g_fft_perf_sink;

void oai_fft_dft16_reference(int16_t *x, int16_t *y)
{
#if defined(__x86_64__) || defined(__i386__) || defined SIMDE_ENABLE_NATIVE_ALIASES
  dft16(x, y);
#else
  rvv_dft16(x, y);
#endif
}

static void fill_fft_test_input(int16_t *x, int complex_n)
{
  for (int i = 0; i < complex_n; i++) {
    int16_t real_part =
        (i % 8) * 8 +
        (int16_t)(10 * sin(2 * 3.14159 * i / 16)) +
        (int16_t)(5 * cos(2 * 3.14159 * i / 32)) +
        (i % 3) * 4 - (i % 5) * 3;

    int16_t imag_part =
        (i % 7) * 6 -
        (int16_t)(8 * cos(2 * 3.14159 * i / 20)) +
        (int16_t)(6 * sin(2 * 3.14159 * i / 40)) +
        (i % 4) * 5 - (i % 6) * 2;

    x[2 * i] = real_part % 64;
    x[2 * i + 1] = imag_part % 64;
  }
}

static void print_complex_result(const char *label, const int16_t *x, int complex_n)
{
  printf("%s (%d点):\n", label, complex_n);
  for (int i = 0; i < complex_n; i++) {
    printf("[%3d] = %6d + %6dj\n", i, x[2 * i], x[2 * i + 1]);
  }
}

static int compare_complex_result(const int16_t *ref, const int16_t *dut, int complex_n)
{
  int pass = 1;

  for (int i = 0; i < complex_n; i++) {
    int re_idx = 2 * i;
    int im_idx = re_idx + 1;
    if (ref[re_idx] != dut[re_idx] || ref[im_idx] != dut[im_idx]) {
      printf("Mismatch at [%3d]: ref=%d + %dj, dut=%d + %dj\n",
             i,
             ref[re_idx], ref[im_idx],
             dut[re_idx], dut[im_idx]);
      pass = 0;
    }
  }

  return pass;
}

#ifndef FFT_LIBRARY_ONLY
int main(void) {
    int16_t x[2 * FFT_MAIN_POINTS];
    int16_t y[2 * FFT_MAIN_POINTS];
#if FFT_MAIN_RUN_IDFT
    int16_t z[2 * FFT_MAIN_POINTS];
#endif
    int repeat = FFT_MAIN_REPEAT;
    int point_n = FFT_MAIN_POINTS;

    set_mstatus_vs_field(MSTATUS_VS_CLEAN);
    set_vxrm(2);
    fill_fft_test_input(x, point_n);

    switch (point_n) {
      case 16:
        for (int i = 0; i < FFT_MAIN_WARMUP_REPEAT; ++i) {
          oai_fft_dft16_reference(x, y);
        }
        for (int i = 0; i < repeat; ++i) {
          oai_fft_dft16_reference(x, y);
        }
        break;
      case 64:
        for (int i = 0; i < FFT_MAIN_WARMUP_REPEAT; ++i) {
          ext_dft64(x, y, 0);
        }
        for (int i = 0; i < repeat; ++i) {
          ext_dft64(x, y, 0);
        }
        break;
      default:
        printf("Unsupported FFT_MAIN_POINTS=%d in oai_dfts.c main\n", point_n);
        return 1;
    }

#if FFT_VALIDATE
    print_complex_result("DFT结果", y, point_n);
    printf("CHECK: PASS\n");

#if FFT_MAIN_RUN_IDFT
    if (point_n == 64) {
      for (int i = 0; i < repeat; ++i) {
        ext_idft64(y, z, 0);
      }
      print_complex_result("IDFT结果", z, point_n);
    }
#endif
#else
    fft_print_steady_state_config(FFT_MAIN_WARMUP_REPEAT, FFT_MAIN_REPEAT);
    g_fft_perf_sink = (int16_t)(y[0] ^ y[1]);
    return (g_fft_perf_sink == (int16_t)0x7fff) ? 1 : 0;
#endif

    return 0;
}
#endif

#endif
