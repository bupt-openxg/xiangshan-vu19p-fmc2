#ifndef MIMO_TYPES_H
#define MIMO_TYPES_H

#include <stddef.h>
#include <stdint.h>

#ifndef MIMO_ENABLE_EQ2
#define MIMO_ENABLE_EQ2 0
#endif

#define MIMO_N_RB 100
#define MIMO_N_SC_PER_RB 12
#define MIMO_N_SC (MIMO_N_RB * MIMO_N_SC_PER_RB)
#define MIMO_N_SYM_SLOT 14
#define MIMO_TOTAL_SITE_COUNT (MIMO_N_SC * MIMO_N_SYM_SLOT)
#define MIMO_EQ2_SITE_COUNT (MIMO_N_SC * 13)
#define MIMO_EQ4_SITE_COUNT (MIMO_N_SC * 13)

#define MIMO_N2 2
#define MIMO_N4 4
#define MIMO_MAX_N 4

#define MIMO_Q15_SHIFT 15
#define MIMO_Q15_ROUND (1 << (MIMO_Q15_SHIFT - 1))
#define MIMO_Q15_SCALE 32768
#define MIMO_MMSE_LAMBDA_Q15 512
#ifndef MIMO_VALIDATE_TOL
#define MIMO_VALIDATE_TOL 2
#endif

#define MIMO_LAYOUT_ROW 0
#define MIMO_LAYOUT_COL 1
#define MIMO_LAYOUT_SITE4 2
#define MIMO_LAYOUT_EQ2_RI_SOA 3
#define MIMO_LAYOUT_EQ2_PACKED 4

#define MIMO_EQ2_H_RI_COMPONENTS 8
#define MIMO_EQ2_Y_RI_COMPONENTS 4
#define MIMO_EQ2_BLOCK_SOA_LANES 8
#define MIMO_EQ2_BLOCK_SOA_BLOCKS \
    (MIMO_EQ2_SITE_COUNT / MIMO_EQ2_BLOCK_SOA_LANES)

#if MIMO_USE_EXT_EQ2_M2_BATCH
#define MIMO_EQ2_PACKED_LANES 8
#else
#define MIMO_EQ2_PACKED_LANES 4
#endif

#define MIMO_SITE4_LANES 4
#define MIMO_SITE4_UPPER4_SLOTS 10

typedef struct {
    int16_t r;
    int16_t i;
} mimo_c16_t;

typedef struct {
    int32_t r;
    int32_t i;
} mimo_c32_t;

typedef struct {
    int32_t a_re[MIMO_N4 * MIMO_N4][MIMO_SITE4_LANES];
    int32_t a_im[MIMO_N4 * MIMO_N4][MIMO_SITE4_LANES];
    int32_t b_re[MIMO_N4][MIMO_SITE4_LANES];
    int32_t b_im[MIMO_N4][MIMO_SITE4_LANES];
} mimo_site4_soa4_t;

typedef struct {
    int32_t a00r[MIMO_SITE4_LANES];
    int32_t a01r[MIMO_SITE4_LANES];
    int32_t a01i[MIMO_SITE4_LANES];
    int32_t a02r[MIMO_SITE4_LANES];
    int32_t a02i[MIMO_SITE4_LANES];
    int32_t a03r[MIMO_SITE4_LANES];
    int32_t a03i[MIMO_SITE4_LANES];
    int32_t a11r[MIMO_SITE4_LANES];
    int32_t a12r[MIMO_SITE4_LANES];
    int32_t a12i[MIMO_SITE4_LANES];
    int32_t a13r[MIMO_SITE4_LANES];
    int32_t a13i[MIMO_SITE4_LANES];
    int32_t a22r[MIMO_SITE4_LANES];
    int32_t a23r[MIMO_SITE4_LANES];
    int32_t a23i[MIMO_SITE4_LANES];
    int32_t a33r[MIMO_SITE4_LANES];
    int32_t b0r[MIMO_SITE4_LANES];
    int32_t b0i[MIMO_SITE4_LANES];
    int32_t b1r[MIMO_SITE4_LANES];
    int32_t b1i[MIMO_SITE4_LANES];
    int32_t b2r[MIMO_SITE4_LANES];
    int32_t b2i[MIMO_SITE4_LANES];
    int32_t b3r[MIMO_SITE4_LANES];
    int32_t b3i[MIMO_SITE4_LANES];
} mimo_site4_upper_rhs_soa4_t;

typedef struct {
    mimo_c16_t a[MIMO_SITE4_UPPER4_SLOTS][MIMO_SITE4_LANES];
    mimo_c16_t b[MIMO_N4][MIMO_SITE4_LANES];
} mimo_site4_upper_i16_t;

#if defined(__GNUC__)
#define MIMO_ALIGN64 __attribute__((aligned(64)))
#define MIMO_NOINLINE __attribute__((noinline))
#define MIMO_ALWAYS_INLINE inline __attribute__((always_inline))
#else
#define MIMO_ALIGN64
#define MIMO_NOINLINE
#define MIMO_ALWAYS_INLINE inline
#endif

#endif
