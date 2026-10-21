// =============================================================================
// XVBBP 扩展指令随机激励测试（板级）
//   对 7 条扩展指令做逐 lane 的随机差分：DUT = RTL，参考 = 本文件标量模型。
//   语义来源: YunSuan golden model (vector_integer_mac.cpp / vector_integer_aluF.cpp)
//   覆盖: e16,m1,vl=8；vxrm 0..3；普通随机值 + 周期性边界值（0/±1/±max）
//   规模: 默认 1,000,000 条指令请求；可用 -DRANDOM_CASES=N 覆盖
//   说明: 文档中 10^9 的功能单元级随机测试需要 YunSuan emu（Verilator），
//         不能在板卡上直接运行；本测试是其在 FPGA 上的核级可执行版本。
// =============================================================================
#include <klib.h>
#include <stdint.h>
#include <stddef.h>
#include <riscv_vector.h>

#ifndef RANDOM_CASES
#define RANDOM_CASES 1000000u
#endif

#ifndef read_csr
#define read_csr(csr) ({ unsigned long __v; \
    __asm__ volatile ("csrr %0, " #csr : "=r" (__v)); __v; })
#endif
#ifndef write_csr
#define write_csr(csr, val) do { \
    unsigned long __v = (unsigned long)(val); \
    __asm__ volatile ("csrw " #csr ", %0" :: "r" (__v)); \
} while (0)
#endif

#define MSTATUS_VS_SHIFT 9
#define MSTATUS_VS_MASK (3UL << MSTATUS_VS_SHIFT)
#define MSTATUS_VS_CLEAN (2UL << MSTATUS_VS_SHIFT)

enum { OP_SCMUL, OP_SCMULCJ, OP_SCMACC, OP_SCMACCCJ,
       OP_SCADDCONJ, OP_SCADDRQ, OP_SCADDRTQ, OP_COUNT };

static const char *const k_op_name[OP_COUNT] = {
    "vscmul", "vscmulcj", "vscmacc", "vscmacccj",
    "vscaddconj", "vscaddrq", "vscaddrtq",
};

typedef struct { int16_t r, i; } c16_t;

static inline void enable_vector_state(void)
{
    uintptr_t v = read_csr(mstatus);
    write_csr(mstatus, (v & ~MSTATUS_VS_MASK) | MSTATUS_VS_CLEAN);
}

static inline size_t set_e16m1_vl8(void)
{
    size_t vl;
    __asm__ volatile ("vsetivli %0, 8, e16, m1, ta, ma"
                      : "=r"(vl) : : "memory");
    return vl;
}

static inline void set_vxrm_clear_vxsat(unsigned vxrm)
{
    write_csr(vcsr, (vxrm & 3U) << 1);
}

static inline unsigned read_vxsat(void)
{
    return (unsigned)(read_csr(vcsr) & 1UL);
}

// ---- 7 条指令执行器（固定 vd=v16, vs2=v20, vs1=v24）-------------------------
#define EXEC_BIN(ENC, VS2, VS1) ({ \
    register vint16m1_t ra asm("v20") = (VS2); \
    register vint16m1_t rb asm("v24") = (VS1); \
    register vint16m1_t rd asm("v16"); \
    __asm__ volatile (ENC : "=vr"(rd) : "vr"(ra), "vr"(rb) : "memory"); \
    rd; })

#define EXEC_ACC(ENC, VD, VS2, VS1) ({ \
    register vint16m1_t ra asm("v20") = (VS2); \
    register vint16m1_t rb asm("v24") = (VS1); \
    register vint16m1_t rd asm("v16") = (VD); \
    __asm__ volatile (ENC : "+vr"(rd) : "vr"(ra), "vr"(rb) : "memory"); \
    rd; })

// ---- 标量参考模型 ----------------------------------------------------------
static inline int16_t sat16(int64_t v, int *sat)
{
    if (v > 32767) { *sat = 1; return 32767; }
    if (v < -32768) { *sat = 1; return -32768; }
    return (int16_t)v;
}

static inline int16_t round_shift15(int64_t v, unsigned vxrm, int *sat)
{
    const uint64_t lsb = 1ULL << 15;
    const uint64_t half = lsb >> 1;
    switch (vxrm) {
    case 0: v += (int64_t)half; break;                     // RNU
    case 1:                                                // RNE
        if ((v & half) && ((v & (half - 1)) || (v & lsb))) v += (int64_t)lsb;
        break;
    case 2: break;                                         // RDN
    case 3:                                                // ROD
        if (v & (lsb - 1)) v |= (int64_t)lsb;
        break;
    }
    return sat16(v >> 15, sat);
}

static inline int16_t sat_add16(int16_t a, int16_t b, int *sat)
{
    int32_t s = (int32_t)a + (int32_t)b;
    if (s > 32767) { *sat = 1; return 32767; }
    if (s < -32768) { *sat = 1; return -32768; }
    return (int16_t)s;
}

static inline int16_t sat_sub16(int16_t a, int16_t b, int *sat)
{
    int32_t s = (int32_t)a - (int32_t)b;
    if (s > 32767) { *sat = 1; return 32767; }
    if (s < -32768) { *sat = 1; return -32768; }
    return (int16_t)s;
}

static void ref_exec(int op, unsigned vxrm, const int16_t *vs2,
                     const int16_t *vs1, const int16_t *vd, int16_t *out,
                     unsigned *vxsat)
{
    int sat = 0;
    *vxsat = 0;
    if (op >= OP_SCADDCONJ) {
        for (int lane = 0; lane < 4; ++lane) {
            int16_t a_r = vs2[2 * lane], a_i = vs2[2 * lane + 1];
            int16_t b_r = vs1[2 * lane], b_i = vs1[2 * lane + 1];
            int16_t r, i;
            if (op == OP_SCADDCONJ) {
                r = sat_add16(a_r, b_r, &sat);
                i = sat_sub16(b_i, a_i, &sat);
            } else if (op == OP_SCADDRQ) {
                r = sat_sub16(b_r, a_i, &sat);
                i = sat_add16(b_i, a_r, &sat);
            } else { // RTQ
                r = sat_add16(b_r, a_i, &sat);
                i = sat_sub16(b_i, a_r, &sat);
            }
            out[2 * lane] = r;
            out[2 * lane + 1] = i;
        }
    } else {
        const int conj = (op == OP_SCMULCJ || op == OP_SCMACCCJ);
        const int acc = (op == OP_SCMACC || op == OP_SCMACCCJ);
        int sat_component = 0;
        for (int lane = 0; lane < 4; ++lane) {
            int16_t a_r = vs2[2 * lane], a_i = vs2[2 * lane + 1];
            int16_t b_r = vs1[2 * lane], b_i = vs1[2 * lane + 1];
            int64_t pr, pi;
            if (!conj) {
                pr = (int64_t)a_r * b_r - (int64_t)a_i * b_i;
                pi = (int64_t)a_r * b_i + (int64_t)a_i * b_r;
            } else {
                pr = (int64_t)a_r * b_r + (int64_t)a_i * b_i;
                pi = (int64_t)a_r * b_i - (int64_t)a_i * b_r;
            }
            if (acc) {
                pr += (int64_t)vd[2 * lane] << 15;
                pi += (int64_t)vd[2 * lane + 1] << 15;
            }
            out[2 * lane]     = round_shift15(pr, vxrm, &sat_component);
            out[2 * lane + 1] = round_shift15(pi, vxrm, &sat_component);
        }
        sat = sat_component;
    }
    *vxsat = (unsigned)sat;
}

// ---- 随机源 ----------------------------------------------------------------
static uint32_t g_rng = 0x20261021u;

static inline uint32_t rnd32(void)
{
    uint32_t x = g_rng;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return g_rng = x;
}

static inline int16_t rnd16(void) { return (int16_t)(rnd32() >> 16); }

static const int16_t k_corners[8] = {
    0, 1, -1, 32767, -32768, 16384, -16384, 23170,
};

static void fill_case(uint32_t index, int16_t *vs2, int16_t *vs1, int16_t *vd)
{
    const int corner = (index & 15u) == 0;
    for (int i = 0; i < 8; ++i) {
        if (corner && (rnd32() & 1u)) {
            vs2[i] = k_corners[rnd32() & 7u];
        } else {
            vs2[i] = rnd16();
        }
        if (corner && (rnd32() & 1u)) {
            vs1[i] = k_corners[rnd32() & 7u];
        } else {
            vs1[i] = rnd16();
        }
        vd[i] = (rnd32() & 3u) == 0u ? k_corners[rnd32() & 7u] : rnd16();
    }
}

int main(void)
{
    static int16_t vs2[8] __attribute__((aligned(16)));
    static int16_t vs1[8] __attribute__((aligned(16)));
    static int16_t vd[8]  __attribute__((aligned(16)));
    static int16_t dut[8] __attribute__((aligned(16)));
    static int16_t ref[8] __attribute__((aligned(16)));
    unsigned op_seen = 0;
    unsigned vxrm_seen = 0;

    enable_vector_state();
    if (set_e16m1_vl8() != 8) {
        printf("XVBBP_RANDOM_FAIL setup_vl\n");
        return 1;
    }

    printf("XVBBP_RANDOM_BEGIN seed=0x%08x cases=%lu ops=%d\n",
           (unsigned)g_rng, (unsigned long)RANDOM_CASES, OP_COUNT);

    for (uint32_t index = 0; index < RANDOM_CASES; ++index) {
        const int op = (int)(rnd32() % OP_COUNT);
        const unsigned vxrm = (index & 3u); // 保证 4 种舍入模式都被覆盖
        uint32_t bit;

        op_seen |= 1u << op;
        vxrm_seen |= 1u << vxrm;
        fill_case(index, vs2, vs1, vd);

        const vint16m1_t a = __riscv_vle16_v_i16m1(vs2, 8);
        const vint16m1_t b = __riscv_vle16_v_i16m1(vs1, 8);
        const vint16m1_t c = __riscv_vle16_v_i16m1(vd, 8);

        set_vxrm_clear_vxsat(vxrm);
        vint16m1_t r;
        switch (op) {
        case OP_SCMUL:    r = EXEC_BIN(".word 0xCB4C0857", a, b); break;
        case OP_SCMULCJ:  r = EXEC_BIN(".word 0xCF4C0857", a, b); break;
        case OP_SCMACC:   r = EXEC_ACC(".word 0xD34C0857", c, a, b); break;
        case OP_SCMACCCJ: r = EXEC_ACC(".word 0xDB4C0857", c, a, b); break;
        case OP_SCADDCONJ:r = EXEC_BIN(".word 0xDF4C0857", a, b); break;
        case OP_SCADDRQ:  r = EXEC_BIN(".word 0xE34C0857", a, b); break;
        default:          r = EXEC_BIN(".word 0xE74C0857", a, b); break;
        }
        __riscv_vse16_v_i16m1(dut, r, 8);
        const unsigned dut_sat = read_vxsat();

        unsigned ref_sat;
        ref_exec(op, vxrm, vs2, vs1, vd, ref, &ref_sat);

        for (int lane = 0; lane < 8; ++lane) {
            if (dut[lane] != ref[lane]) {
                printf("XVBBP_RANDOM_FAIL case=%lu op=%s vxrm=%u lane=%d "
                       "dut=%d ref=%d\n",
                       (unsigned long)index, k_op_name[op], vxrm, lane,
                       (int)dut[lane], (int)ref[lane]);
                return 1;
            }
        }
        if (dut_sat != ref_sat) {
            printf("XVBBP_RANDOM_FAIL case=%lu op=%s vxrm=%u vxsat dut=%u ref=%u\n",
                   (unsigned long)index, k_op_name[op], vxrm, dut_sat, ref_sat);
            return 1;
        }

        if ((index & 0x3ffffu) == 0x3ffffu) {
            printf("XVBBP_RANDOM_PROGRESS cases=%lu\n",
                   (unsigned long)(index + 1));
        }
    }

    const unsigned all_ops = (1u << OP_COUNT) - 1u;
    printf("XVBBP_RANDOM_COVERAGE ops=0x%02x/0x%02x vxrm=0x%x/0xf\n",
           op_seen, all_ops, vxrm_seen);
    if (op_seen != all_ops || vxrm_seen != 0xf) {
        printf("XVBBP_RANDOM_FAIL coverage\n");
        return 1;
    }
    printf("XVBBP_RANDOM_PASS cases=%lu ops=0x%02x vxrm=0xf\n",
           (unsigned long)RANDOM_CASES, op_seen);
    return 0;
}
