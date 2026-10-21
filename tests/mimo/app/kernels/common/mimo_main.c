#include <stdint.h>
#include <stddef.h>

#include "klib.h"
#include "mimo_bench.h"
#include "mimo_kernels.h"
#include "mimo_rvv.h"

#ifndef MIMO_VALIDATE
#define MIMO_VALIDATE 1
#endif

#ifndef MIMO_VALIDATE_TOL
#define MIMO_VALIDATE_TOL 2
#endif

#ifndef MIMO_VALIDATE_ERROR_STATS
#define MIMO_VALIDATE_ERROR_STATS 0
#endif

#ifndef MIMO_VALIDATE_FAIL_PRINT_LIMIT
#define MIMO_VALIDATE_FAIL_PRINT_LIMIT 4
#endif

#ifndef MIMO_EQ2_QUALITY_EVAL
#define MIMO_EQ2_QUALITY_EVAL 0
#endif

#ifndef MIMO_EQ2_QUALITY_MOD_BITS
#define MIMO_EQ2_QUALITY_MOD_BITS 2
#endif

#ifndef MIMO_EQ2_QUALITY_NOISE_Q15
#define MIMO_EQ2_QUALITY_NOISE_Q15 0
#endif

#ifndef MIMO_EQ2_QUALITY_COND_PROFILE
#define MIMO_EQ2_QUALITY_COND_PROFILE 0
#endif

#ifndef MIMO_PERF
#define MIMO_PERF 0
#endif

#ifndef MIMO_WARMUP_REPEAT
#define MIMO_WARMUP_REPEAT 1
#endif

#ifndef MIMO_REPEAT
#define MIMO_REPEAT 1
#endif

#ifndef MIMO_BACKEND_NAME
#define MIMO_BACKEND_NAME "unknown"
#endif

#ifndef MIMO_SELECTED_LAYOUT
#define MIMO_SELECTED_LAYOUT MIMO_LAYOUT_ROW
#endif

#ifndef MIMO_USE_RVV_EQ2_TX_SOA_STORE
#define MIMO_USE_RVV_EQ2_TX_SOA_STORE 0
#endif

#ifndef MIMO_USE_RVV_EQ2_RI_SOA_OUTPUT
#define MIMO_USE_RVV_EQ2_RI_SOA_OUTPUT 0
#endif

#ifndef MIMO_USE_RVV_EQ2_BLOCK_SOA
#define MIMO_USE_RVV_EQ2_BLOCK_SOA 0
#endif

#ifndef MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID
#define MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID 0
#endif

#ifndef MIMO_USE_RVV_SITE4_RI_SOA
#define MIMO_USE_RVV_SITE4_RI_SOA 0
#endif

#if MIMO_USE_RVV_SITE4_RI_SOA && MIMO_SELECTED_LAYOUT != MIMO_LAYOUT_SITE4
#error "MIMO_USE_RVV_SITE4_RI_SOA requires the site4 native input layout"
#endif

#ifndef MIMO_KERNEL_RUNTIME_INIT
#define MIMO_KERNEL_RUNTIME_INIT() ((void)0)
#endif

#ifndef MIMO_4X4_EQ_ONLY_LOCK
#define MIMO_4X4_EQ_ONLY_LOCK 1
#endif

#ifndef MIMO_ZF4_ONLY_LOCK
#define MIMO_ZF4_ONLY_LOCK 1
#endif

#define MIMO_KERNEL_EQ_ZF_4X4 1
#define MIMO_KERNEL_EQ_MMSE_4X4 2
#define MIMO_KERNEL_EQ_ZF_2X2 3
#define MIMO_KERNEL_EQ_MMSE_2X2 4

#ifndef MIMO_EQ2_BACKEND_READY
#define MIMO_EQ2_BACKEND_READY 0
#endif

#ifndef MIMO_EQ_ZF_2X2_KERNEL
#define MIMO_EQ_ZF_2X2_KERNEL mimo_eq_zf_2x2_scalar
#endif

#ifndef MIMO_EQ_MMSE_2X2_KERNEL
#define MIMO_EQ_MMSE_2X2_KERNEL mimo_eq_mmse_2x2_scalar
#endif

#ifndef MIMO_EQ_MMSE_4X4_KERNEL
#define MIMO_EQ_MMSE_4X4_KERNEL mimo_eq_mmse_4x4_scalar
#endif

#ifndef MIMO_PERF_KERNEL
#define MIMO_PERF_KERNEL MIMO_KERNEL_EQ_ZF_4X4
#endif

#ifndef MIMO_VALIDATE_KERNEL
#define MIMO_VALIDATE_KERNEL MIMO_KERNEL_EQ_ZF_4X4
#endif

#if MIMO_4X4_EQ_ONLY_LOCK != 1
#error "Legacy MIMO_4X4_EQ_ONLY_LOCK must remain enabled while the 2x2/4x4 EQ scope is active"
#endif
#if MIMO_ZF4_ONLY_LOCK != 1
#error "Legacy MIMO_ZF4_ONLY_LOCK must remain enabled while the 2x2/4x4 EQ scope is active"
#endif

#if MIMO_VALIDATE_KERNEL != MIMO_KERNEL_EQ_ZF_4X4 && \
    MIMO_VALIDATE_KERNEL != MIMO_KERNEL_EQ_MMSE_4X4 && \
    MIMO_VALIDATE_KERNEL != MIMO_KERNEL_EQ_ZF_2X2 && \
    MIMO_VALIDATE_KERNEL != MIMO_KERNEL_EQ_MMSE_2X2
#error "Current hard scope permits only 2x2/4x4 ZF/MMSE validation"
#endif
#if MIMO_PERF_KERNEL != MIMO_KERNEL_EQ_ZF_4X4 && \
    MIMO_PERF_KERNEL != MIMO_KERNEL_EQ_MMSE_4X4 && \
    MIMO_PERF_KERNEL != MIMO_KERNEL_EQ_ZF_2X2 && \
    MIMO_PERF_KERNEL != MIMO_KERNEL_EQ_MMSE_2X2
#error "Current hard scope permits only 2x2/4x4 ZF/MMSE perf"
#endif

#if (MIMO_VALIDATE && \
     (MIMO_VALIDATE_KERNEL == MIMO_KERNEL_EQ_ZF_2X2 || \
      MIMO_VALIDATE_KERNEL == MIMO_KERNEL_EQ_MMSE_2X2)) || \
    (MIMO_PERF && \
     (MIMO_PERF_KERNEL == MIMO_KERNEL_EQ_ZF_2X2 || \
      MIMO_PERF_KERNEL == MIMO_KERNEL_EQ_MMSE_2X2))
#define MIMO_NEED_2 1
#else
#define MIMO_NEED_2 0
#endif

#if (MIMO_VALIDATE && \
     (MIMO_VALIDATE_KERNEL == MIMO_KERNEL_EQ_ZF_4X4 || \
      MIMO_VALIDATE_KERNEL == MIMO_KERNEL_EQ_MMSE_4X4)) || \
    (MIMO_PERF && \
     (MIMO_PERF_KERNEL == MIMO_KERNEL_EQ_ZF_4X4 || \
      MIMO_PERF_KERNEL == MIMO_KERNEL_EQ_MMSE_4X4))
#define MIMO_NEED_4 1
#else
#define MIMO_NEED_4 0
#endif

#if MIMO_NEED_2 && (!MIMO_ENABLE_EQ2 || !MIMO_EQ2_BACKEND_READY)
#error "The selected EQ2 backend is not implemented and cannot be measured"
#endif

#if MIMO_EQ2_QUALITY_EVAL && (!MIMO_VALIDATE || MIMO_PERF || !MIMO_NEED_2)
#error "EQ2 quality evaluation requires EQ2 validation with performance mode disabled"
#endif

#if MIMO_EQ2_QUALITY_EVAL && \
    MIMO_EQ2_QUALITY_MOD_BITS != 2 && \
    MIMO_EQ2_QUALITY_MOD_BITS != 4 && \
    MIMO_EQ2_QUALITY_MOD_BITS != 6
#error "EQ2 quality modulation must use 2, 4, or 6 bits per complex symbol"
#endif

#if MIMO_EQ2_QUALITY_EVAL && \
    (MIMO_EQ2_QUALITY_COND_PROFILE < 0 || MIMO_EQ2_QUALITY_COND_PROFILE > 2)
#error "EQ2 quality condition profile must be 0, 1, or 2"
#endif

#if MIMO_NEED_2 && MIMO_USE_RVV_EQ2_TX_SOA_STORE && \
    MIMO_SELECTED_LAYOUT != MIMO_LAYOUT_EQ2_RI_SOA
#error "EQ2 tx-SoA output requires the RVV RI-SoA backend layout"
#endif

#if MIMO_NEED_2 && MIMO_USE_RVV_EQ2_RI_SOA_OUTPUT && \
    MIMO_SELECTED_LAYOUT != MIMO_LAYOUT_EQ2_RI_SOA
#error "EQ2 RI-SoA output requires the RVV RI-SoA backend layout"
#endif

#if MIMO_USE_RVV_EQ2_RI_SOA_OUTPUT && MIMO_USE_RVV_EQ2_TX_SOA_STORE
#error "EQ2 RI-SoA output and TX-SoA packed output are mutually exclusive"
#endif

typedef void (*mimo_eq2_fn_t)(const mimo_c16_t (*)[MIMO_N2][MIMO_N2],
                              const mimo_c16_t (*)[MIMO_N2],
                              mimo_c16_t (*)[MIMO_N2]);

typedef void (*mimo_eq4_fn_t)(const mimo_c16_t (*)[MIMO_N4][MIMO_N4],
                              const mimo_c16_t (*)[MIMO_N4],
                              mimo_c16_t (*)[MIMO_N4]);

volatile int mimo_status = 0;
volatile int32_t mimo_checksum = 0;

#define MIMO_C16I(re, im) { (int16_t)(re), (int16_t)(im) }
#define MIMO_H2_ROW_SITE_INIT \
    { \
        { MIMO_C16I(19040, 16), MIMO_C16I(384, -192) }, \
        { MIMO_C16I(-288, 160), MIMO_C16I(18976, -24) } \
    }
#define MIMO_Y2_SITE_INIT \
    { MIMO_C16I(4200, -2800), MIMO_C16I(-3600, 2400) }
#define MIMO_H4_ROW_SITE_INIT \
    { \
        { MIMO_C16I(19040, 16), MIMO_C16I(384, -192), MIMO_C16I(-256, 224), MIMO_C16I(192, 128) }, \
        { MIMO_C16I(-288, 160), MIMO_C16I(18976, -24), MIMO_C16I(352, -176), MIMO_C16I(-224, 208) }, \
        { MIMO_C16I(256, -224), MIMO_C16I(-320, 192), MIMO_C16I(19024, 32), MIMO_C16I(288, -160) }, \
        { MIMO_C16I(-192, -128), MIMO_C16I(224, -208), MIMO_C16I(-288, 160), MIMO_C16I(18992, -16) } \
    }
#define MIMO_H4_COL_SITE_INIT \
    { \
        { MIMO_C16I(19040, 16), MIMO_C16I(-288, 160), MIMO_C16I(256, -224), MIMO_C16I(-192, -128) }, \
        { MIMO_C16I(384, -192), MIMO_C16I(18976, -24), MIMO_C16I(-320, 192), MIMO_C16I(224, -208) }, \
        { MIMO_C16I(-256, 224), MIMO_C16I(352, -176), MIMO_C16I(19024, 32), MIMO_C16I(-288, 160) }, \
        { MIMO_C16I(192, 128), MIMO_C16I(-224, 208), MIMO_C16I(288, -160), MIMO_C16I(18992, -16) } \
    }
#define MIMO_Y4_SITE_INIT \
    { MIMO_C16I(4200, -2800), MIMO_C16I(-3600, 2400), MIMO_C16I(3100, 1900), MIMO_C16I(-2500, -1700) }

#if MIMO_NEED_2
#if MIMO_PERF && !MIMO_VALIDATE
#if MIMO_SELECTED_LAYOUT == MIMO_LAYOUT_ROW
static mimo_c16_t h2_row[MIMO_EQ2_SITE_COUNT][MIMO_N2][MIMO_N2] MIMO_ALIGN64 =
    { [0 ... MIMO_EQ2_SITE_COUNT - 1] = MIMO_H2_ROW_SITE_INIT };
static mimo_c16_t y2_data[MIMO_EQ2_SITE_COUNT][MIMO_N2] MIMO_ALIGN64 =
    { [0 ... MIMO_EQ2_SITE_COUNT - 1] = MIMO_Y2_SITE_INIT };
#elif MIMO_SELECTED_LAYOUT == MIMO_LAYOUT_EQ2_PACKED
static mimo_c16_t h2_row[MIMO_EQ2_SITE_COUNT][MIMO_N2][MIMO_N2] MIMO_ALIGN64 =
    { [0 ... MIMO_EQ2_SITE_COUNT - 1] = MIMO_H2_ROW_SITE_INIT };
static mimo_c16_t y2_data[MIMO_EQ2_SITE_COUNT][MIMO_N2] MIMO_ALIGN64 =
    { [0 ... MIMO_EQ2_SITE_COUNT - 1] = MIMO_Y2_SITE_INIT };
static mimo_c16_t h2_packed[MIMO_EQ2_SITE_COUNT][MIMO_N2][MIMO_N2]
    MIMO_ALIGN64;
static mimo_c16_t y2_packed[MIMO_EQ2_SITE_COUNT][MIMO_N2] MIMO_ALIGN64;
#elif MIMO_SELECTED_LAYOUT == MIMO_LAYOUT_EQ2_RI_SOA
#if MIMO_USE_RVV_EQ2_BLOCK_SOA
static int16_t h2_ri_soa[MIMO_EQ2_BLOCK_SOA_BLOCKS]
                            [MIMO_EQ2_H_RI_COMPONENTS]
                            [MIMO_EQ2_BLOCK_SOA_LANES] MIMO_ALIGN64 = {
    [0 ... MIMO_EQ2_BLOCK_SOA_BLOCKS - 1] = {
        [0] = { [0 ... MIMO_EQ2_BLOCK_SOA_LANES - 1] = 19040 },
        [1] = { [0 ... MIMO_EQ2_BLOCK_SOA_LANES - 1] = 16 },
        [2] = { [0 ... MIMO_EQ2_BLOCK_SOA_LANES - 1] = 384 },
        [3] = { [0 ... MIMO_EQ2_BLOCK_SOA_LANES - 1] = -192 },
        [4] = { [0 ... MIMO_EQ2_BLOCK_SOA_LANES - 1] = -288 },
        [5] = { [0 ... MIMO_EQ2_BLOCK_SOA_LANES - 1] = 160 },
        [6] = { [0 ... MIMO_EQ2_BLOCK_SOA_LANES - 1] = 18976 },
        [7] = { [0 ... MIMO_EQ2_BLOCK_SOA_LANES - 1] = -24 },
    },
};
static int16_t y2_ri_soa[MIMO_EQ2_BLOCK_SOA_BLOCKS]
                            [MIMO_EQ2_Y_RI_COMPONENTS]
                            [MIMO_EQ2_BLOCK_SOA_LANES] MIMO_ALIGN64 = {
    [0 ... MIMO_EQ2_BLOCK_SOA_BLOCKS - 1] = {
        [0] = { [0 ... MIMO_EQ2_BLOCK_SOA_LANES - 1] = 4200 },
        [1] = { [0 ... MIMO_EQ2_BLOCK_SOA_LANES - 1] = -2800 },
        [2] = { [0 ... MIMO_EQ2_BLOCK_SOA_LANES - 1] = -3600 },
        [3] = { [0 ... MIMO_EQ2_BLOCK_SOA_LANES - 1] = 2400 },
    },
};
#else
static int16_t h2_ri_soa[MIMO_EQ2_H_RI_COMPONENTS][MIMO_EQ2_SITE_COUNT]
    MIMO_ALIGN64 = {
        { [0 ... MIMO_EQ2_SITE_COUNT - 1] = 19040 },
        { [0 ... MIMO_EQ2_SITE_COUNT - 1] = 16 },
        { [0 ... MIMO_EQ2_SITE_COUNT - 1] = 384 },
        { [0 ... MIMO_EQ2_SITE_COUNT - 1] = -192 },
        { [0 ... MIMO_EQ2_SITE_COUNT - 1] = -288 },
        { [0 ... MIMO_EQ2_SITE_COUNT - 1] = 160 },
        { [0 ... MIMO_EQ2_SITE_COUNT - 1] = 18976 },
        { [0 ... MIMO_EQ2_SITE_COUNT - 1] = -24 },
    };
static int16_t y2_ri_soa[MIMO_EQ2_Y_RI_COMPONENTS][MIMO_EQ2_SITE_COUNT]
    MIMO_ALIGN64 = {
        { [0 ... MIMO_EQ2_SITE_COUNT - 1] = 4200 },
        { [0 ... MIMO_EQ2_SITE_COUNT - 1] = -2800 },
        { [0 ... MIMO_EQ2_SITE_COUNT - 1] = -3600 },
        { [0 ... MIMO_EQ2_SITE_COUNT - 1] = 2400 },
    };
#endif
#else
#error "The selected EQ2 input layout is not implemented"
#endif
#else
static mimo_c16_t h2_row[MIMO_EQ2_SITE_COUNT][MIMO_N2][MIMO_N2] MIMO_ALIGN64;
static mimo_c16_t y2_data[MIMO_EQ2_SITE_COUNT][MIMO_N2] MIMO_ALIGN64;
#if MIMO_SELECTED_LAYOUT == MIMO_LAYOUT_EQ2_RI_SOA
#if MIMO_USE_RVV_EQ2_BLOCK_SOA
static int16_t h2_ri_soa[MIMO_EQ2_BLOCK_SOA_BLOCKS]
                            [MIMO_EQ2_H_RI_COMPONENTS]
                            [MIMO_EQ2_BLOCK_SOA_LANES] MIMO_ALIGN64;
static int16_t y2_ri_soa[MIMO_EQ2_BLOCK_SOA_BLOCKS]
                            [MIMO_EQ2_Y_RI_COMPONENTS]
                            [MIMO_EQ2_BLOCK_SOA_LANES] MIMO_ALIGN64;
#else
static int16_t h2_ri_soa[MIMO_EQ2_H_RI_COMPONENTS][MIMO_EQ2_SITE_COUNT]
    MIMO_ALIGN64;
static int16_t y2_ri_soa[MIMO_EQ2_Y_RI_COMPONENTS][MIMO_EQ2_SITE_COUNT]
    MIMO_ALIGN64;
#endif
#elif MIMO_SELECTED_LAYOUT == MIMO_LAYOUT_EQ2_PACKED
static mimo_c16_t h2_packed[MIMO_EQ2_SITE_COUNT][MIMO_N2][MIMO_N2]
    MIMO_ALIGN64;
static mimo_c16_t y2_packed[MIMO_EQ2_SITE_COUNT][MIMO_N2] MIMO_ALIGN64;
#elif MIMO_SELECTED_LAYOUT != MIMO_LAYOUT_ROW
#error "The selected EQ2 input layout is not implemented"
#endif
#endif
#if MIMO_SELECTED_LAYOUT == MIMO_LAYOUT_ROW
#define MIMO_H2_SELECTED h2_row
#define MIMO_Y2_SELECTED y2_data
#elif MIMO_SELECTED_LAYOUT == MIMO_LAYOUT_EQ2_RI_SOA
#define MIMO_H2_SELECTED \
    ((const mimo_c16_t (*)[MIMO_N2][MIMO_N2])(const void *)h2_ri_soa)
#define MIMO_Y2_SELECTED \
    ((const mimo_c16_t (*)[MIMO_N2])(const void *)y2_ri_soa)
#else
#define MIMO_H2_SELECTED h2_packed
#define MIMO_Y2_SELECTED y2_packed
#endif
#endif

#if MIMO_NEED_4
#if MIMO_PERF && !MIMO_VALIDATE
static mimo_c16_t h4_row[MIMO_EQ4_SITE_COUNT][MIMO_N4][MIMO_N4] MIMO_ALIGN64 =
    { [0 ... MIMO_EQ4_SITE_COUNT - 1] = MIMO_H4_ROW_SITE_INIT };
static mimo_c16_t y4_data[MIMO_EQ4_SITE_COUNT][MIMO_N4] MIMO_ALIGN64 =
    { [0 ... MIMO_EQ4_SITE_COUNT - 1] = MIMO_Y4_SITE_INIT };
#else
static mimo_c16_t h4_row[MIMO_EQ4_SITE_COUNT][MIMO_N4][MIMO_N4] MIMO_ALIGN64;
static mimo_c16_t y4_data[MIMO_EQ4_SITE_COUNT][MIMO_N4] MIMO_ALIGN64;
#endif
#endif

#if MIMO_VALIDATE
#if MIMO_NEED_2
static mimo_c16_t x2_ref[MIMO_EQ2_SITE_COUNT][MIMO_N2] MIMO_ALIGN64;
#if MIMO_EQ2_QUALITY_EVAL
static mimo_c16_t x2_true[MIMO_EQ2_SITE_COUNT][MIMO_N2] MIMO_ALIGN64;
static uint64_t quality_rx_signal_power;
static uint64_t quality_noise_power;
static uint32_t quality_zf_det_q15_min = UINT32_MAX;
static uint32_t quality_zf_det_q15_max;
#endif
#if MIMO_USE_RVV_EQ2_RI_SOA_OUTPUT || MIMO_USE_RVV_EQ2_TX_SOA_STORE || \
    MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID || \
    MIMO_SELECTED_LAYOUT == MIMO_LAYOUT_EQ2_PACKED
static mimo_c16_t x2_canonical[MIMO_EQ2_SITE_COUNT][MIMO_N2] MIMO_ALIGN64;
#endif
#endif
#if MIMO_NEED_4
static mimo_c16_t x4_ref[MIMO_EQ4_SITE_COUNT][MIMO_N4] MIMO_ALIGN64;
#endif
#endif
#if MIMO_NEED_2
static mimo_c16_t x2_out[MIMO_EQ2_SITE_COUNT][MIMO_N2] MIMO_ALIGN64;
#endif
#if MIMO_NEED_4
static mimo_c16_t x4_out[MIMO_EQ4_SITE_COUNT][MIMO_N4] MIMO_ALIGN64;
#endif

#if MIMO_SELECTED_LAYOUT == MIMO_LAYOUT_COL
#if MIMO_NEED_4
#if MIMO_PERF && !MIMO_VALIDATE
static mimo_c16_t h4_col[MIMO_EQ4_SITE_COUNT][MIMO_N4][MIMO_N4] MIMO_ALIGN64 =
    { [0 ... MIMO_EQ4_SITE_COUNT - 1] = MIMO_H4_COL_SITE_INIT };
#else
static mimo_c16_t h4_col[MIMO_EQ4_SITE_COUNT][MIMO_N4][MIMO_N4] MIMO_ALIGN64;
#endif
#endif
#define MIMO_H4_SELECTED h4_col
#define MIMO_Y4_SELECTED y4_data
#elif MIMO_SELECTED_LAYOUT == MIMO_LAYOUT_SITE4
#if MIMO_NEED_4
#if MIMO_PERF && !MIMO_VALIDATE
static __attribute__((unused)) mimo_c16_t
    h4_col[MIMO_EQ4_SITE_COUNT][MIMO_N4][MIMO_N4] MIMO_ALIGN64 =
    { [0 ... MIMO_EQ4_SITE_COUNT - 1] = MIMO_H4_COL_SITE_INIT };
#else
static mimo_c16_t h4_col[MIMO_EQ4_SITE_COUNT][MIMO_N4][MIMO_N4] MIMO_ALIGN64;
#endif
static mimo_c16_t h4_site4[MIMO_EQ4_SITE_COUNT][MIMO_N4][MIMO_N4] MIMO_ALIGN64;
static mimo_c16_t y4_site4[MIMO_EQ4_SITE_COUNT][MIMO_N4] MIMO_ALIGN64;
#endif
#define MIMO_H4_SELECTED h4_site4
#define MIMO_Y4_SELECTED y4_site4
#else
#define MIMO_H4_SELECTED h4_row
#define MIMO_Y4_SELECTED y4_data
#endif

static int16_t centered_value(size_t site, int a, int b, int span)
{
    uint32_t x = (uint32_t)site;
    x ^= (uint32_t)(a * 0x45d9f3bu);
    x += (uint32_t)(b * 0x9e3779b9u);
    x ^= x >> 16;
    x *= 0x7feb352du;
    x ^= x >> 15;
    const int32_t v = (int32_t)(x & 1023u) - 512;
    return (int16_t)((v * span) >> 9);
}

#if MIMO_EQ2_QUALITY_EVAL
typedef struct {
    int16_t c;
    int16_t s;
} quality_phase_q15_t;

static const quality_phase_q15_t quality_phase_table[8] = {
    {32767, 0},
    {23170, 23170},
    {0, 32767},
    {-23170, 23170},
    {-32767, 0},
    {-23170, -23170},
    {0, -32767},
    {23170, -23170},
};

static uint32_t quality_mix(size_t site, unsigned stream)
{
    uint32_t x = (uint32_t)site ^ (stream * 0x9e3779b9u);

    x ^= x >> 16;
    x *= 0x7feb352du;
    x ^= x >> 15;
    x *= 0x846ca68bu;
    x ^= x >> 16;
    return x;
}

static int quality_qam_levels(void)
{
    return 1 << (MIMO_EQ2_QUALITY_MOD_BITS / 2);
}

static int quality_qam_unit(void)
{
#if MIMO_EQ2_QUALITY_MOD_BITS == 2
    return 8192;
#elif MIMO_EQ2_QUALITY_MOD_BITS == 4
    return 4096;
#else
    return 2048;
#endif
}

static int16_t quality_qam_component(size_t site, int tx, int component)
{
    const int levels = quality_qam_levels();
    const int index =
        (int)(quality_mix(site, (unsigned)(17 + tx * 2 + component)) &
              (uint32_t)(levels - 1));

    return (int16_t)((2 * index + 1 - levels) * quality_qam_unit());
}

static int16_t quality_q30_to_i16(int64_t value)
{
    return mimo_sat_i16((int32_t)((value + (1ll << 29)) >> 30));
}

static void quality_channel(size_t site, mimo_c16_t h[MIMO_N2 * MIMO_N2])
{
#if MIMO_EQ2_QUALITY_COND_PROFILE == 0
    const int32_t d1 = 19000;
    const int32_t d2 = 19000;
#elif MIMO_EQ2_QUALITY_COND_PROFILE == 1
    const int32_t d1 = 23396;
    const int32_t d2 = 15598;
#else
    const int32_t d1 = 27033;
    const int32_t d2 = 13517;
#endif
    const quality_phase_q15_t left =
        quality_phase_table[quality_mix(site, 41) & 7u];
    const quality_phase_q15_t right =
        quality_phase_table[quality_mix(site, 43) & 7u];
    const int64_t ct = left.c;
    const int64_t st = left.s;
    const int64_t cp = right.c;
    const int64_t sp = right.s;
    const int16_t real_h[MIMO_N2 * MIMO_N2] = {
        quality_q30_to_i16(d1 * ct * cp + d2 * st * sp),
        quality_q30_to_i16(d1 * ct * sp - d2 * st * cp),
        quality_q30_to_i16(d1 * st * cp - d2 * ct * sp),
        quality_q30_to_i16(d1 * st * sp + d2 * ct * cp),
    };

    for (int row = 0; row < MIMO_N2; ++row) {
        const quality_phase_q15_t row_phase =
            quality_phase_table[quality_mix(site, (unsigned)(47 + row)) & 7u];
        const mimo_c16_t phase = mimo_make_c16(row_phase.c, row_phase.s);

        for (int col = 0; col < MIMO_N2; ++col) {
            h[row * MIMO_N2 + col] = mimo_cmul_q15(
                mimo_make_c16(real_h[row * MIMO_N2 + col], 0), phase);
        }
    }
}

static int16_t quality_noise(size_t site, int rx, int component)
{
    int32_t sum = 0;

    for (int tap = 0; tap < 4; ++tap) {
        sum += centered_value(
            site,
            61 + rx * 13 + component * 7 + tap,
            79 + tap * 11,
            MIMO_EQ2_QUALITY_NOISE_Q15);
    }
    return (int16_t)(sum / 2);
}

static void quality_update_det_range(const mimo_c16_t h[MIMO_N2 * MIMO_N2])
{
    const mimo_c16_t p0 = mimo_cmul_q15(h[0], h[3]);
    const mimo_c16_t p1 = mimo_cmul_q15(h[1], h[2]);
    const int32_t dr = (int32_t)p0.r - p1.r;
    const int32_t di = (int32_t)p0.i - p1.i;
    const uint32_t det_q15 = (uint32_t)(
        ((int64_t)dr * dr + (int64_t)di * di + MIMO_Q15_ROUND) >>
        MIMO_Q15_SHIFT);

    if (det_q15 < quality_zf_det_q15_min) {
        quality_zf_det_q15_min = det_q15;
    }
    if (det_q15 > quality_zf_det_q15_max) {
        quality_zf_det_q15_max = det_q15;
    }
}
#endif

static __attribute__((unused)) mimo_c16_t tx_symbol(size_t site, int tx)
{
    const int16_t r = (int16_t)(5200 + centered_value(site, 23 + tx, tx, 1500));
    const int16_t i = (int16_t)(-3600 + centered_value(site, 31 + tx, tx + 5, 1200));
    return mimo_make_c16(r, i);
}

static __attribute__((unused)) mimo_c16_t channel_value(int n, size_t site, int rx, int tx)
{
    if (rx == tx) {
        const int16_t r = (int16_t)(19000 + centered_value(site, 3 + rx, tx, 350));
        const int16_t i = centered_value(site, 5 + tx, rx, 260);
        return mimo_make_c16(r, i);
    }

    const int16_t r = centered_value(site, 7 + rx, 3 * tx + n, 420);
    const int16_t i = centered_value(site, 11 + tx, 5 * rx + n, 360);
    return mimo_make_c16(r, i);
}

#if !(MIMO_PERF && !MIMO_VALIDATE)
static void fill_one_site(int n,
                          size_t site,
                          mimo_c16_t *h_row,
#if MIMO_SELECTED_LAYOUT == MIMO_LAYOUT_COL || \
    MIMO_SELECTED_LAYOUT == MIMO_LAYOUT_SITE4
                          mimo_c16_t *h_col,
#endif
                          mimo_c16_t *y)
{
    mimo_c16_t x_true[MIMO_MAX_N];

#if MIMO_EQ2_QUALITY_EVAL
    if (n == MIMO_N2) {
        mimo_c16_t quality_h[MIMO_N2 * MIMO_N2];

        quality_channel(site, quality_h);
        quality_update_det_range(quality_h);
        for (int tx = 0; tx < MIMO_N2; ++tx) {
            x_true[tx] = mimo_make_c16(
                quality_qam_component(site, tx, 0),
                quality_qam_component(site, tx, 1));
            x2_true[site][tx] = x_true[tx];
        }

        for (int rx = 0; rx < MIMO_N2; ++rx) {
            int32_t yr = 0;
            int32_t yi = 0;

            for (int tx = 0; tx < MIMO_N2; ++tx) {
                const mimo_c16_t h = quality_h[rx * MIMO_N2 + tx];
                const mimo_c16_t hx = mimo_cmul_q15(h, x_true[tx]);

                h_row[rx * MIMO_N2 + tx] = h;
#if MIMO_SELECTED_LAYOUT == MIMO_LAYOUT_COL || \
    MIMO_SELECTED_LAYOUT == MIMO_LAYOUT_SITE4
                h_col[tx * MIMO_N2 + rx] = h;
#endif
                yr += hx.r;
                yi += hx.i;
            }

            const int16_t nr = quality_noise(site, rx, 0);
            const int16_t ni = quality_noise(site, rx, 1);

            quality_rx_signal_power += (uint64_t)((int64_t)yr * yr) +
                                       (uint64_t)((int64_t)yi * yi);
            quality_noise_power += (uint64_t)((int64_t)nr * nr) +
                                   (uint64_t)((int64_t)ni * ni);
            y[rx] = mimo_make_c16(
                mimo_sat_i16(yr + nr), mimo_sat_i16(yi + ni));
        }
        return;
    }
#endif

    for (int tx = 0; tx < n; ++tx) {
        x_true[tx] = tx_symbol(site, tx);
    }

    for (int rx = 0; rx < n; ++rx) {
        int32_t yr = 0;
        int32_t yi = 0;

        for (int tx = 0; tx < n; ++tx) {
            const mimo_c16_t h = channel_value(n, site, rx, tx);
            const mimo_c16_t hx = mimo_cmul_q15(h, x_true[tx]);

            h_row[rx * n + tx] = h;
#if MIMO_SELECTED_LAYOUT == MIMO_LAYOUT_COL || \
    MIMO_SELECTED_LAYOUT == MIMO_LAYOUT_SITE4
            h_col[tx * n + rx] = h;
#endif
            yr += hx.r;
            yi += hx.i;
        }
        y[rx] = mimo_make_c16(mimo_sat_i16(yr), mimo_sat_i16(yi));
    }
}
#endif

#if MIMO_SELECTED_LAYOUT == MIMO_LAYOUT_SITE4
#if MIMO_NEED_4
#if MIMO_USE_RVV_SITE4_RI_SOA
static void store_site4_ri_soa_lane(mimo_c16_t vec[MIMO_SITE4_LANES],
                                    size_t lane,
                                    mimo_c16_t value)
{
    switch (lane) {
    case 0:
        vec[0].r = value.r;
        vec[2].r = value.i;
        break;
    case 1:
        vec[0].i = value.r;
        vec[2].i = value.i;
        break;
    case 2:
        vec[1].r = value.r;
        vec[3].r = value.i;
        break;
    default:
        vec[1].i = value.r;
        vec[3].i = value.i;
        break;
    }
}
#endif

static void fill_h4_site4_from_row(const mimo_c16_t h_row[MIMO_EQ4_SITE_COUNT][MIMO_N4][MIMO_N4],
                                   const mimo_c16_t y_row[MIMO_EQ4_SITE_COUNT][MIMO_N4])
{
    mimo_c16_t *h_flat = &h4_site4[0][0][0];
    mimo_c16_t *y_flat = &y4_site4[0][0];

    for (size_t site = 0; site < MIMO_EQ4_SITE_COUNT; ++site) {
        const size_t block = site / MIMO_SITE4_LANES;
        const size_t lane = site % MIMO_SITE4_LANES;

        for (int tx = 0; tx < MIMO_N4; ++tx) {
            for (int rx = 0; rx < MIMO_N4; ++rx) {
                const size_t base =
                    (((block * MIMO_N4 + (size_t)tx) * MIMO_N4 + (size_t)rx) *
                     MIMO_SITE4_LANES);
#if MIMO_USE_RVV_SITE4_RI_SOA
                store_site4_ri_soa_lane(
                    &h_flat[base], lane, h_row[site][rx][tx]);
#else
                h_flat[base + lane] = h_row[site][rx][tx];
#endif
            }
        }
        for (int rx = 0; rx < MIMO_N4; ++rx) {
            const size_t base =
                (block * MIMO_N4 + (size_t)rx) * MIMO_SITE4_LANES;
#if MIMO_USE_RVV_SITE4_RI_SOA
            store_site4_ri_soa_lane(&y_flat[base], lane, y_row[site][rx]);
#else
            y_flat[base + lane] = y_row[site][rx];
#endif
        }
    }
}
#endif
#endif

#if MIMO_SELECTED_LAYOUT == MIMO_LAYOUT_EQ2_RI_SOA && MIMO_NEED_2 && \
    !(MIMO_PERF && !MIMO_VALIDATE)
static void fill_h2_ri_soa_from_row(void)
{
    for (size_t site = 0; site < MIMO_EQ2_SITE_COUNT; ++site) {
#if MIMO_USE_RVV_EQ2_BLOCK_SOA
        const size_t block = site / MIMO_EQ2_BLOCK_SOA_LANES;
        const size_t lane = site % MIMO_EQ2_BLOCK_SOA_LANES;

        h2_ri_soa[block][0][lane] = h2_row[site][0][0].r;
        h2_ri_soa[block][1][lane] = h2_row[site][0][0].i;
        h2_ri_soa[block][2][lane] = h2_row[site][0][1].r;
        h2_ri_soa[block][3][lane] = h2_row[site][0][1].i;
        h2_ri_soa[block][4][lane] = h2_row[site][1][0].r;
        h2_ri_soa[block][5][lane] = h2_row[site][1][0].i;
        h2_ri_soa[block][6][lane] = h2_row[site][1][1].r;
        h2_ri_soa[block][7][lane] = h2_row[site][1][1].i;

        y2_ri_soa[block][0][lane] = y2_data[site][0].r;
        y2_ri_soa[block][1][lane] = y2_data[site][0].i;
        y2_ri_soa[block][2][lane] = y2_data[site][1].r;
        y2_ri_soa[block][3][lane] = y2_data[site][1].i;
#else
        h2_ri_soa[0][site] = h2_row[site][0][0].r;
        h2_ri_soa[1][site] = h2_row[site][0][0].i;
        h2_ri_soa[2][site] = h2_row[site][0][1].r;
        h2_ri_soa[3][site] = h2_row[site][0][1].i;
        h2_ri_soa[4][site] = h2_row[site][1][0].r;
        h2_ri_soa[5][site] = h2_row[site][1][0].i;
        h2_ri_soa[6][site] = h2_row[site][1][1].r;
        h2_ri_soa[7][site] = h2_row[site][1][1].i;

        y2_ri_soa[0][site] = y2_data[site][0].r;
        y2_ri_soa[1][site] = y2_data[site][0].i;
        y2_ri_soa[2][site] = y2_data[site][1].r;
        y2_ri_soa[3][site] = y2_data[site][1].i;
#endif
    }
}
#endif

#if MIMO_SELECTED_LAYOUT == MIMO_LAYOUT_EQ2_PACKED && MIMO_NEED_2
static void fill_h2_packed_from_row(void)
{
    mimo_c16_t *const h_flat = &h2_packed[0][0][0];
    mimo_c16_t *const y_flat = &y2_packed[0][0];

    for (size_t site = 0; site < MIMO_EQ2_SITE_COUNT; ++site) {
        const size_t block = site / MIMO_EQ2_PACKED_LANES;
        const size_t lane = site % MIMO_EQ2_PACKED_LANES;

        for (size_t tx = 0; tx < MIMO_N2; ++tx) {
            for (size_t rx = 0; rx < MIMO_N2; ++rx) {
                const size_t base =
                    ((block * MIMO_N2 * MIMO_N2 + tx * MIMO_N2 + rx) *
                     MIMO_EQ2_PACKED_LANES);
                h_flat[base + lane] = h2_row[site][rx][tx];
            }
        }
        for (size_t rx = 0; rx < MIMO_N2; ++rx) {
            const size_t base =
                (block * MIMO_N2 + rx) * MIMO_EQ2_PACKED_LANES;
            y_flat[base + lane] = y2_data[site][rx];
        }
    }
}
#endif

static void fill_inputs(void)
{
#if MIMO_PERF && !MIMO_VALIDATE
#if MIMO_SELECTED_LAYOUT == MIMO_LAYOUT_EQ2_PACKED
#if MIMO_NEED_2
    fill_h2_packed_from_row();
#endif
#endif
#if MIMO_SELECTED_LAYOUT == MIMO_LAYOUT_SITE4
#if MIMO_NEED_4
    fill_h4_site4_from_row(h4_row, y4_data);
#endif
#endif
    return;
#else
#if MIMO_NEED_2
    for (size_t site = 0; site < MIMO_EQ2_SITE_COUNT; ++site) {
        fill_one_site(MIMO_N2, site, &h2_row[site][0][0],
                      &y2_data[site][0]);
    }
#if MIMO_SELECTED_LAYOUT == MIMO_LAYOUT_EQ2_RI_SOA
    fill_h2_ri_soa_from_row();
#elif MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID || \
      MIMO_SELECTED_LAYOUT == MIMO_LAYOUT_EQ2_PACKED
    fill_h2_packed_from_row();
#endif
#endif
#if MIMO_NEED_4
    for (size_t site = 0; site < MIMO_EQ4_SITE_COUNT; ++site) {
        fill_one_site(MIMO_N4, site, &h4_row[site][0][0],
#if MIMO_SELECTED_LAYOUT == MIMO_LAYOUT_COL || \
    MIMO_SELECTED_LAYOUT == MIMO_LAYOUT_SITE4
                      &h4_col[site][0][0],
#endif
                      &y4_data[site][0]);
    }
#endif

#if MIMO_SELECTED_LAYOUT == MIMO_LAYOUT_SITE4
#if MIMO_NEED_4
    fill_h4_site4_from_row(h4_row, y4_data);
#endif
#endif
#endif
}

static __attribute__((unused)) int32_t checksum_c16(const mimo_c16_t *x, size_t count)
{
    uint32_t s = 2166136261u;

    for (size_t i = 0; i < count; ++i) {
        s ^= (uint16_t)x[i].r;
        s *= 16777619u;
        s ^= (uint16_t)x[i].i;
        s *= 16777619u;
    }
    return (int32_t)s;
}

#if MIMO_NEED_2
static int32_t checksum_eq2_output(const mimo_c16_t *x)
{
#if MIMO_USE_RVV_EQ2_RI_SOA_OUTPUT
    const int16_t *const physical = (const int16_t *)(const void *)x;
    uint32_t s = 2166136261u;

    for (size_t site = 0; site < MIMO_EQ2_SITE_COUNT; ++site) {
        for (size_t tx = 0; tx < MIMO_N2; ++tx) {
            const size_t component = tx * 2;

            s ^= (uint16_t)physical[component * MIMO_EQ2_SITE_COUNT + site];
            s *= 16777619u;
            s ^= (uint16_t)physical[(component + 1) * MIMO_EQ2_SITE_COUNT + site];
            s *= 16777619u;
        }
    }
    return (int32_t)s;
#elif MIMO_USE_RVV_EQ2_TX_SOA_STORE
    uint32_t s = 2166136261u;

    for (size_t site = 0; site < MIMO_EQ2_SITE_COUNT; ++site) {
        for (size_t tx = 0; tx < MIMO_N2; ++tx) {
            const mimo_c16_t value = x[tx * MIMO_EQ2_SITE_COUNT + site];

            s ^= (uint16_t)value.r;
            s *= 16777619u;
            s ^= (uint16_t)value.i;
            s *= 16777619u;
        }
    }
    return (int32_t)s;
#elif MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID || \
      MIMO_SELECTED_LAYOUT == MIMO_LAYOUT_EQ2_PACKED
    uint32_t s = 2166136261u;

    for (size_t site = 0; site < MIMO_EQ2_SITE_COUNT; ++site) {
        const size_t block = site / MIMO_EQ2_PACKED_LANES;
        const size_t lane = site % MIMO_EQ2_PACKED_LANES;

        for (size_t tx = 0; tx < MIMO_N2; ++tx) {
            const mimo_c16_t value =
                x[(block * MIMO_N2 + tx) * MIMO_EQ2_PACKED_LANES + lane];

            s ^= (uint16_t)value.r;
            s *= 16777619u;
            s ^= (uint16_t)value.i;
            s *= 16777619u;
        }
    }
    return (int32_t)s;
#else
    return checksum_c16(x, MIMO_EQ2_SITE_COUNT * MIMO_N2);
#endif
}
#endif

#if MIMO_VALIDATE
static inline int iabs32(int x)
{
    return x < 0 ? -x : x;
}

#if MIMO_VALIDATE_ERROR_STATS
typedef struct {
    uint64_t components;
    uint64_t sum_abs;
    uint64_t sum_sq;
    uint64_t over_tol;
    uint64_t over_2;
    uint64_t over_3;
    uint64_t over_4;
    uint64_t over_8;
    uint64_t over_16;
    unsigned max_abs;
} mimo_validate_error_stats_t;

static void validate_stats_update(mimo_validate_error_stats_t *stats,
                                  unsigned err,
                                  unsigned tol)
{
    ++stats->components;
    stats->sum_abs += err;
    stats->sum_sq += (uint64_t)err * err;
    if (err > stats->max_abs) {
        stats->max_abs = err;
    }
    if (err > tol) {
        ++stats->over_tol;
    }
    if (err > 2u) {
        ++stats->over_2;
    }
    if (err > 3u) {
        ++stats->over_3;
    }
    if (err > 4u) {
        ++stats->over_4;
    }
    if (err > 8u) {
        ++stats->over_8;
    }
    if (err > 16u) {
        ++stats->over_16;
    }
}

static void validate_stats_print(const char *name,
                                 const mimo_validate_error_stats_t *stats,
                                 int tol,
                                 int fail)
{
    const uint64_t mean_abs_x1000 =
        stats->components ? (stats->sum_abs * 1000u) / stats->components : 0u;
    const uint64_t mse_x1000 =
        stats->components ? (stats->sum_sq * 1000u) / stats->components : 0u;

    printf("MIMO_VALIDATE_ERROR_STATS kernel=%s tol=%d fail=%d components=%llu "
           "max_abs=%u mean_abs_x1000=%llu mse_x1000=%llu "
           "over_tol=%llu over2=%llu over3=%llu over4=%llu over8=%llu over16=%llu\n",
           name,
           tol,
           fail,
           stats->components,
           stats->max_abs,
           mean_abs_x1000,
           mse_x1000,
           stats->over_tol,
           stats->over_2,
           stats->over_3,
           stats->over_4,
           stats->over_8,
           stats->over_16);
}
#endif

static int compare_c16(const char *name,
                       const mimo_c16_t *ref,
                       const mimo_c16_t *out,
                       size_t count,
                       int tol)
{
    int fail = 0;
#if MIMO_VALIDATE_ERROR_STATS
    mimo_validate_error_stats_t stats = {0};
#endif

    for (size_t i = 0; i < count; ++i) {
        const int dr = iabs32((int)ref[i].r - (int)out[i].r);
        const int di = iabs32((int)ref[i].i - (int)out[i].i);

#if MIMO_VALIDATE_ERROR_STATS
        validate_stats_update(&stats, (unsigned)dr, (unsigned)tol);
        validate_stats_update(&stats, (unsigned)di, (unsigned)tol);
#endif

        if (dr > tol || di > tol) {
            if (fail < MIMO_VALIDATE_FAIL_PRINT_LIMIT) {
                printf("MIMO_VALIDATE_FAIL kernel=%s index=%u ref=(%d,%d) out=(%d,%d) dr=%d di=%d\n",
                       name,
                       (unsigned)i,
                       ref[i].r,
                       ref[i].i,
                       out[i].r,
                       out[i].i,
                       dr,
                       di);
            }
            ++fail;
        }
    }

    if (fail == 0) {
        printf("MIMO_VALIDATE_PASS kernel=%s count=%u tol=%d checksum=%d\n",
               name,
               (unsigned)count,
               tol,
               checksum_c16(out, count));
    } else {
        printf("MIMO_VALIDATE_SUMMARY kernel=%s fail=%d count=%u tol=%d\n",
               name,
               fail,
               (unsigned)count,
               tol);
    }

#if MIMO_VALIDATE_ERROR_STATS
    validate_stats_print(name, &stats, tol, fail);
#endif

    return fail;
}

#if MIMO_NEED_2
static void reference_eq2(int mmse,
                          const mimo_c16_t h[MIMO_EQ2_SITE_COUNT][MIMO_N2][MIMO_N2],
                          const mimo_c16_t y[MIMO_EQ2_SITE_COUNT][MIMO_N2],
                          mimo_c16_t x[MIMO_EQ2_SITE_COUNT][MIMO_N2])
{
    mimo_c32_t a[MIMO_N2 * MIMO_N2];
    mimo_c32_t b[MIMO_N2];

    for (size_t site = 0; site < MIMO_EQ2_SITE_COUNT; ++site) {
        mimo_build_normal_row_q15(MIMO_N2, &h[site][0][0], &y[site][0], a, b);
        mimo_solve_normal_ref_q15(MIMO_N2, a, b, mmse, &x[site][0]);
    }
}

static const mimo_c16_t *canonical_eq2_output(void)
{
#if MIMO_USE_RVV_EQ2_RI_SOA_OUTPUT
    const int16_t *const physical =
        (const int16_t *)(const void *)&x2_out[0][0];

    for (size_t site = 0; site < MIMO_EQ2_SITE_COUNT; ++site) {
        for (size_t tx = 0; tx < MIMO_N2; ++tx) {
            const size_t component = tx * 2;

            x2_canonical[site][tx].r =
                physical[component * MIMO_EQ2_SITE_COUNT + site];
            x2_canonical[site][tx].i =
                physical[(component + 1) * MIMO_EQ2_SITE_COUNT + site];
        }
    }
    return &x2_canonical[0][0];
#elif MIMO_USE_RVV_EQ2_TX_SOA_STORE
    const mimo_c16_t *const physical = &x2_out[0][0];

    for (size_t site = 0; site < MIMO_EQ2_SITE_COUNT; ++site) {
        for (size_t tx = 0; tx < MIMO_N2; ++tx) {
            x2_canonical[site][tx] =
                physical[tx * MIMO_EQ2_SITE_COUNT + site];
        }
    }
    return &x2_canonical[0][0];
#elif MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID || \
      MIMO_SELECTED_LAYOUT == MIMO_LAYOUT_EQ2_PACKED
    const mimo_c16_t *const physical = &x2_out[0][0];

    for (size_t site = 0; site < MIMO_EQ2_SITE_COUNT; ++site) {
        const size_t block = site / MIMO_EQ2_PACKED_LANES;
        const size_t lane = site % MIMO_EQ2_PACKED_LANES;

        for (size_t tx = 0; tx < MIMO_N2; ++tx) {
            x2_canonical[site][tx] =
                physical[(block * MIMO_N2 + tx) * MIMO_EQ2_PACKED_LANES + lane];
        }
    }
    return &x2_canonical[0][0];
#else
    return &x2_out[0][0];
#endif
}

#if MIMO_EQ2_QUALITY_EVAL
typedef struct {
    uint64_t symbols;
    uint64_t symbol_errors;
    uint64_t bits;
    uint64_t bit_errors;
    uint64_t error_power;
    uint64_t signal_power;
} quality_metrics_t;

static unsigned quality_popcount(unsigned value)
{
    unsigned count = 0;

    while (value != 0) {
        count += value & 1u;
        value >>= 1;
    }
    return count;
}

static int quality_level_index(int value)
{
    const int levels = quality_qam_levels();
    const int unit = quality_qam_unit();
    int best = 0;
    int best_distance = INT32_MAX;

    for (int index = 0; index < levels; ++index) {
        const int level = (2 * index + 1 - levels) * unit;
        const int distance = iabs32(value - level);

        if (distance < best_distance) {
            best = index;
            best_distance = distance;
        }
    }
    return best;
}

static uint64_t quality_ratio_ppm(uint64_t numerator, uint64_t denominator)
{
    if (denominator == 0) {
        return 0;
    }
    return (numerator / denominator) * 1000000u +
           ((numerator % denominator) * 1000000u) / denominator;
}

static quality_metrics_t quality_measure(const mimo_c16_t *values)
{
    quality_metrics_t metrics = {0};

    for (size_t site = 0; site < MIMO_EQ2_SITE_COUNT; ++site) {
        for (int tx = 0; tx < MIMO_N2; ++tx) {
            const size_t index = site * MIMO_N2 + (size_t)tx;
            const mimo_c16_t truth = x2_true[site][tx];
            const mimo_c16_t value = values[index];
            const int true_r = quality_level_index(truth.r);
            const int true_i = quality_level_index(truth.i);
            const int value_r = quality_level_index(value.r);
            const int value_i = quality_level_index(value.i);
            const int32_t dr = (int32_t)value.r - truth.r;
            const int32_t di = (int32_t)value.i - truth.i;

            ++metrics.symbols;
            metrics.bits += MIMO_EQ2_QUALITY_MOD_BITS;
            if (value_r != true_r || value_i != true_i) {
                ++metrics.symbol_errors;
            }
            metrics.bit_errors += quality_popcount(
                (unsigned)(true_r ^ (true_r >> 1) ^
                           value_r ^ (value_r >> 1)));
            metrics.bit_errors += quality_popcount(
                (unsigned)(true_i ^ (true_i >> 1) ^
                           value_i ^ (value_i >> 1)));
            metrics.error_power += (uint64_t)((int64_t)dr * dr) +
                                   (uint64_t)((int64_t)di * di);
            metrics.signal_power +=
                (uint64_t)((int64_t)truth.r * truth.r) +
                (uint64_t)((int64_t)truth.i * truth.i);
        }
    }
    return metrics;
}

static void quality_print(const mimo_c16_t *output)
{
    const quality_metrics_t out_metrics = quality_measure(output);
    const quality_metrics_t ref_metrics = quality_measure(&x2_ref[0][0]);
    const mimo_c16_t *const ref_values = &x2_ref[0][0];
    uint64_t decision_mismatches = 0;
    uint64_t output_ref_error_power = 0;

    for (size_t index = 0; index < MIMO_EQ2_SITE_COUNT * MIMO_N2; ++index) {
        const mimo_c16_t out = output[index];
        const mimo_c16_t ref = ref_values[index];
        const int32_t dr = (int32_t)out.r - ref.r;
        const int32_t di = (int32_t)out.i - ref.i;

        if (quality_level_index(out.r) != quality_level_index(ref.r) ||
            quality_level_index(out.i) != quality_level_index(ref.i)) {
            ++decision_mismatches;
        }
        output_ref_error_power += (uint64_t)((int64_t)dr * dr) +
                                  (uint64_t)((int64_t)di * di);
    }

#if MIMO_VALIDATE_KERNEL == MIMO_KERNEL_EQ_ZF_2X2
    const char *const kernel = "eq_zf_2x2";
#else
    const char *const kernel = "eq_mmse_2x2";
#endif
    printf("MIMO_EQ2_QUALITY backend=%s kernel=%s mod_bits=%d noise_q15=%d "
           "cond_profile=%d symbols=%llu symbol_errors=%llu bits=%llu "
           "bit_errors=%llu evm2_ppm=%llu ref_symbol_errors=%llu "
           "ref_bit_errors=%llu ref_evm2_ppm=%llu "
           "decision_mismatch_vs_ref=%llu output_ref_mse_x1000=%llu "
           "noise_to_rx_signal_ppm=%llu zf_det_q15_min=%u zf_det_q15_max=%u\n",
           MIMO_BACKEND_NAME,
           kernel,
           MIMO_EQ2_QUALITY_MOD_BITS,
           MIMO_EQ2_QUALITY_NOISE_Q15,
           MIMO_EQ2_QUALITY_COND_PROFILE,
           out_metrics.symbols,
           out_metrics.symbol_errors,
           out_metrics.bits,
           out_metrics.bit_errors,
           quality_ratio_ppm(out_metrics.error_power, out_metrics.signal_power),
           ref_metrics.symbol_errors,
           ref_metrics.bit_errors,
           quality_ratio_ppm(ref_metrics.error_power, ref_metrics.signal_power),
           decision_mismatches,
           (output_ref_error_power * 1000u) /
               (MIMO_EQ2_SITE_COUNT * MIMO_N2 * 2u),
           quality_ratio_ppm(quality_noise_power, quality_rx_signal_power),
           quality_zf_det_q15_min,
           quality_zf_det_q15_max);
}
#endif
#endif

#if MIMO_NEED_4
static void reference_eq4(int mmse,
                          const mimo_c16_t h[MIMO_EQ4_SITE_COUNT][MIMO_N4][MIMO_N4],
                          const mimo_c16_t y[MIMO_EQ4_SITE_COUNT][MIMO_N4],
                          mimo_c16_t x[MIMO_EQ4_SITE_COUNT][MIMO_N4])
{
    mimo_c32_t a[MIMO_N4 * MIMO_N4];
    mimo_c32_t b[MIMO_N4];

    for (size_t site = 0; site < MIMO_EQ4_SITE_COUNT; ++site) {
        mimo_build_normal_row_q15(MIMO_N4, &h[site][0][0], &y[site][0], a, b);
        mimo_solve_normal_ref_q15(MIMO_N4, a, b, mmse, &x[site][0]);
    }
}
#endif

static void validate_all(void)
{
    int fail = 0;
    const int tol = MIMO_VALIDATE_TOL;

#if MIMO_VALIDATE_KERNEL == MIMO_KERNEL_EQ_ZF_2X2
    reference_eq2(0, h2_row, y2_data, x2_ref);
    MIMO_EQ_ZF_2X2_KERNEL(MIMO_H2_SELECTED, MIMO_Y2_SELECTED, x2_out);
    fail += compare_c16("eq_zf_2x2", &x2_ref[0][0], canonical_eq2_output(),
                        MIMO_EQ2_SITE_COUNT * MIMO_N2, tol);
#elif MIMO_VALIDATE_KERNEL == MIMO_KERNEL_EQ_MMSE_2X2
    reference_eq2(1, h2_row, y2_data, x2_ref);
    MIMO_EQ_MMSE_2X2_KERNEL(MIMO_H2_SELECTED, MIMO_Y2_SELECTED, x2_out);
    fail += compare_c16("eq_mmse_2x2", &x2_ref[0][0], canonical_eq2_output(),
                        MIMO_EQ2_SITE_COUNT * MIMO_N2, tol);
#elif MIMO_VALIDATE_KERNEL == MIMO_KERNEL_EQ_ZF_4X4
    reference_eq4(0, h4_row, y4_data, x4_ref);
    MIMO_EQ_ZF_4X4_KERNEL(MIMO_H4_SELECTED, MIMO_Y4_SELECTED, x4_out);
    fail += compare_c16("eq_zf_4x4", &x4_ref[0][0], &x4_out[0][0],
                        MIMO_EQ4_SITE_COUNT * MIMO_N4, tol);
#elif MIMO_VALIDATE_KERNEL == MIMO_KERNEL_EQ_MMSE_4X4
    reference_eq4(1, h4_row, y4_data, x4_ref);
    MIMO_EQ_MMSE_4X4_KERNEL(MIMO_H4_SELECTED, MIMO_Y4_SELECTED, x4_out);
    fail += compare_c16("eq_mmse_4x4", &x4_ref[0][0], &x4_out[0][0],
                        MIMO_EQ4_SITE_COUNT * MIMO_N4, tol);
#endif
#if MIMO_EQ2_QUALITY_EVAL
    quality_print(canonical_eq2_output());
#endif
    mimo_status = fail;
#if MIMO_VALIDATE_KERNEL == MIMO_KERNEL_EQ_ZF_2X2 || \
    MIMO_VALIDATE_KERNEL == MIMO_KERNEL_EQ_MMSE_2X2
    mimo_checksum = checksum_eq2_output(&x2_out[0][0]);
#else
    mimo_checksum = checksum_c16(&x4_out[0][0], MIMO_EQ4_SITE_COUNT * MIMO_N4);
#endif
}
#endif

#if MIMO_PERF
#if MIMO_NEED_2
static void measure_eq2(const char *name,
                        mimo_eq2_fn_t fn,
                        const mimo_c16_t h[MIMO_EQ2_SITE_COUNT][MIMO_N2][MIMO_N2],
                        const mimo_c16_t y[MIMO_EQ2_SITE_COUNT][MIMO_N2],
                        mimo_c16_t x[MIMO_EQ2_SITE_COUNT][MIMO_N2])
{
    mimo_roi_stats_t stats;

    for (uint32_t r = 0; r < MIMO_WARMUP_REPEAT; ++r) {
        fn(h, y, x);
    }

    const mimo_roi_stamp_t stamp = mimo_roi_begin();
    for (uint32_t r = 0; r < MIMO_REPEAT; ++r) {
        fn(h, y, x);
    }
    mimo_roi_end(&stats, stamp, MIMO_REPEAT);
    mimo_print_roi_summary(name, MIMO_BACKEND_NAME, &stats);
    mimo_checksum ^= checksum_eq2_output(&x[0][0]);
}
#endif

#if MIMO_NEED_4
static void measure_eq4(const char *name,
                        mimo_eq4_fn_t fn,
                        const mimo_c16_t h[MIMO_EQ4_SITE_COUNT][MIMO_N4][MIMO_N4],
                        const mimo_c16_t y[MIMO_EQ4_SITE_COUNT][MIMO_N4],
                        mimo_c16_t x[MIMO_EQ4_SITE_COUNT][MIMO_N4])
{
    mimo_roi_stats_t stats;

    for (uint32_t r = 0; r < MIMO_WARMUP_REPEAT; ++r) {
        fn(h, y, x);
    }

    const mimo_roi_stamp_t stamp = mimo_roi_begin();
    for (uint32_t r = 0; r < MIMO_REPEAT; ++r) {
        fn(h, y, x);
    }
    mimo_roi_end(&stats, stamp, MIMO_REPEAT);
    mimo_print_roi_summary(name, MIMO_BACKEND_NAME, &stats);
    mimo_checksum ^= checksum_c16(&x[0][0], MIMO_EQ4_SITE_COUNT * MIMO_N4);
}
#endif

static void measure_all(void)
{
    mimo_print_steady_state_config(MIMO_WARMUP_REPEAT, MIMO_REPEAT);
#if MIMO_PERF_KERNEL == MIMO_KERNEL_EQ_ZF_2X2
    measure_eq2("eq_zf_2x2", MIMO_EQ_ZF_2X2_KERNEL, MIMO_H2_SELECTED,
                MIMO_Y2_SELECTED, x2_out);
#elif MIMO_PERF_KERNEL == MIMO_KERNEL_EQ_MMSE_2X2
    measure_eq2("eq_mmse_2x2", MIMO_EQ_MMSE_2X2_KERNEL, MIMO_H2_SELECTED,
                MIMO_Y2_SELECTED, x2_out);
#elif MIMO_PERF_KERNEL == MIMO_KERNEL_EQ_ZF_4X4
    measure_eq4("eq_zf_4x4", MIMO_EQ_ZF_4X4_KERNEL, MIMO_H4_SELECTED,
                MIMO_Y4_SELECTED, x4_out);
#elif MIMO_PERF_KERNEL == MIMO_KERNEL_EQ_MMSE_4X4
    measure_eq4("eq_mmse_4x4", MIMO_EQ_MMSE_4X4_KERNEL, MIMO_H4_SELECTED,
                MIMO_Y4_SELECTED, x4_out);
#endif
}
#endif

int main(void)
{
    printf("MIMO_EQ_BENCH backend=%s layout=%s validate_kernel=%d perf_kernel=%d sites2=%u sites4=%u lambda_q15=%d\n",
           MIMO_BACKEND_NAME,
           MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID ? "eq2-packed-adaptive-guard" :
           MIMO_SELECTED_LAYOUT == MIMO_LAYOUT_COL ? "col" :
           MIMO_SELECTED_LAYOUT == MIMO_LAYOUT_SITE4 ? "site4" :
           MIMO_SELECTED_LAYOUT == MIMO_LAYOUT_EQ2_RI_SOA ? "eq2-ri-soa" :
           MIMO_SELECTED_LAYOUT == MIMO_LAYOUT_EQ2_PACKED ? "eq2-packed" :
           "row",
           MIMO_VALIDATE_KERNEL,
           MIMO_PERF_KERNEL,
           (unsigned)MIMO_EQ2_SITE_COUNT,
           (unsigned)MIMO_EQ4_SITE_COUNT,
           MIMO_MMSE_LAMBDA_Q15);

    fill_inputs();
    mimo_init_recip_table();
    MIMO_KERNEL_RUNTIME_INIT();

#if MIMO_VALIDATE
    validate_all();
#endif

#if MIMO_PERF
    measure_all();
#endif

    mimo_print_solve_den_stats();
    printf("MIMO_DONE status=%d checksum=%d\n", mimo_status, mimo_checksum);
    return mimo_status == 0 ? 0 : 1;
}
