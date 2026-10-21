#include <klib.h>
#include <stddef.h>
#include <stdint.h>
#include <riscv_vector.h>

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

typedef enum {
  VSCMUL,
  VSCMULCJ,
  VSCMACC,
  VSCMACCCJ,
  VSCADDCONJ,
  VSCADDRQ,
  VSCADDRTQ
} ext_op_t;

typedef struct {
  const char *name;
  ext_op_t op;
  unsigned vxrm;
  int16_t vs2[8];
  int16_t vs1[8];
  int16_t vd[8];
  int16_t expected[8];
  unsigned expected_vxsat;
} ext_test_case_t;

typedef struct {
  const char *name;
  ext_op_t op;
  unsigned vxrm;
  int16_t vs2[16];
  int16_t vs1[16];
  int16_t vd[16];
  int16_t expected[16];
  unsigned expected_vxsat;
} ext_m2_test_case_t;

#include "ext_test_vectors.h"

static inline void enable_vector_state(void)
{
  uintptr_t value = read_csr(mstatus);
  value = (value & ~MSTATUS_VS_MASK) | MSTATUS_VS_CLEAN;
  write_csr(mstatus, value);
}

static inline size_t set_e16m1_vl8(void)
{
  size_t actual_vl;
  __asm__ volatile (
      "vsetivli %0, 8, e16, m1, ta, ma"
      : "=r"(actual_vl)
      :
      : "memory");
  return actual_vl;
}

static inline size_t set_e16m2_vl16(void)
{
  size_t actual_vl;
  __asm__ volatile (
      "vsetivli %0, 16, e16, m2, ta, ma"
      : "=r"(actual_vl)
      :
      : "memory");
  return actual_vl;
}

static inline void set_vxrm_clear_vxsat(unsigned vxrm)
{
  write_csr(vcsr, (vxrm & 3U) << 1);
}

static inline void clear_vxsat(void)
{
  write_csr(vcsr, read_csr(vcsr) & ~1UL);
}

static inline unsigned read_vxsat(void)
{
  return (unsigned)(read_csr(vcsr) & 1UL);
}

static inline __attribute__((always_inline))
vint16m1_t exec_vscmul(vint16m1_t vs2, vint16m1_t vs1)
{
  register vint16m1_t ra asm("v20") = vs2;
  register vint16m1_t rb asm("v24") = vs1;
  register vint16m1_t rd asm("v16");
  __asm__ volatile (".word 0xCB4C0857"
                    : "=vr"(rd) : "vr"(ra), "vr"(rb) : "memory");
  return rd;
}

static inline __attribute__((always_inline))
vint16m1_t exec_vscmulcj(vint16m1_t vs2, vint16m1_t vs1)
{
  register vint16m1_t ra asm("v20") = vs2;
  register vint16m1_t rb asm("v24") = vs1;
  register vint16m1_t rd asm("v16");
  __asm__ volatile (".word 0xCF4C0857"
                    : "=vr"(rd) : "vr"(ra), "vr"(rb) : "memory");
  return rd;
}

static inline __attribute__((always_inline))
vint16m1_t exec_vscmacc(vint16m1_t vd, vint16m1_t vs2, vint16m1_t vs1)
{
  register vint16m1_t ra asm("v20") = vs2;
  register vint16m1_t rb asm("v24") = vs1;
  register vint16m1_t rd asm("v16") = vd;
  __asm__ volatile (".word 0xD34C0857"
                    : "+vr"(rd) : "vr"(ra), "vr"(rb) : "memory");
  return rd;
}

static inline __attribute__((always_inline))
vint16m1_t exec_vscmacccj(vint16m1_t vd, vint16m1_t vs2, vint16m1_t vs1)
{
  register vint16m1_t ra asm("v20") = vs2;
  register vint16m1_t rb asm("v24") = vs1;
  register vint16m1_t rd asm("v16") = vd;
  __asm__ volatile (".word 0xDB4C0857"
                    : "+vr"(rd) : "vr"(ra), "vr"(rb) : "memory");
  return rd;
}

static inline __attribute__((always_inline))
vint16m1_t exec_vscaddconj(vint16m1_t vs2, vint16m1_t vs1)
{
  register vint16m1_t ra asm("v20") = vs2;
  register vint16m1_t rb asm("v24") = vs1;
  register vint16m1_t rd asm("v16");
  __asm__ volatile (".word 0xDF4C0857"
                    : "=vr"(rd) : "vr"(ra), "vr"(rb) : "memory");
  return rd;
}

static inline __attribute__((always_inline))
vint16m1_t exec_vscaddrq(vint16m1_t vs2, vint16m1_t vs1)
{
  register vint16m1_t ra asm("v20") = vs2;
  register vint16m1_t rb asm("v24") = vs1;
  register vint16m1_t rd asm("v16");
  __asm__ volatile (".word 0xE34C0857"
                    : "=vr"(rd) : "vr"(ra), "vr"(rb) : "memory");
  return rd;
}

static inline __attribute__((always_inline))
vint16m1_t exec_vscaddrtq(vint16m1_t vs2, vint16m1_t vs1)
{
  register vint16m1_t ra asm("v20") = vs2;
  register vint16m1_t rb asm("v24") = vs1;
  register vint16m1_t rd asm("v16");
  __asm__ volatile (".word 0xE74C0857"
                    : "=vr"(rd) : "vr"(ra), "vr"(rb) : "memory");
  return rd;
}

#define DEFINE_M2_BINARY_EXECUTOR(name, encoding)                         \
static inline __attribute__((always_inline))                             \
vint16m2_t name(vint16m2_t vs2, vint16m2_t vs1)                         \
{                                                                        \
  register vint16m2_t ra asm("v20") = vs2;                              \
  register vint16m2_t rb asm("v24") = vs1;                              \
  register vint16m2_t rd asm("v16");                                    \
  __asm__ volatile (encoding : "=vr"(rd) : "vr"(ra), "vr"(rb) : "memory"); \
  return rd;                                                             \
}

#define DEFINE_M2_ACC_EXECUTOR(name, encoding)                           \
static inline __attribute__((always_inline))                             \
vint16m2_t name(vint16m2_t vd, vint16m2_t vs2, vint16m2_t vs1)          \
{                                                                        \
  register vint16m2_t ra asm("v20") = vs2;                              \
  register vint16m2_t rb asm("v24") = vs1;                              \
  register vint16m2_t rd asm("v16") = vd;                               \
  __asm__ volatile (encoding : "+vr"(rd) : "vr"(ra), "vr"(rb) : "memory"); \
  return rd;                                                             \
}

DEFINE_M2_BINARY_EXECUTOR(exec_vscmul_m2, ".word 0xCB4C0857")
DEFINE_M2_BINARY_EXECUTOR(exec_vscmulcj_m2, ".word 0xCF4C0857")
DEFINE_M2_ACC_EXECUTOR(exec_vscmacc_m2, ".word 0xD34C0857")
DEFINE_M2_ACC_EXECUTOR(exec_vscmacccj_m2, ".word 0xDB4C0857")
DEFINE_M2_BINARY_EXECUTOR(exec_vscaddconj_m2, ".word 0xDF4C0857")
DEFINE_M2_BINARY_EXECUTOR(exec_vscaddrq_m2, ".word 0xE34C0857")
DEFINE_M2_BINARY_EXECUTOR(exec_vscaddrtq_m2, ".word 0xE74C0857")

static vint16m1_t execute_instruction(const ext_test_case_t *test_case,
                                      size_t vl)
{
  vint16m1_t vs2 = __riscv_vle16_v_i16m1(test_case->vs2, vl);
  vint16m1_t vs1 = __riscv_vle16_v_i16m1(test_case->vs1, vl);
  switch (test_case->op) {
    case VSCMUL:
      return exec_vscmul(vs2, vs1);
    case VSCMULCJ:
      return exec_vscmulcj(vs2, vs1);
    case VSCMACC:
      return exec_vscmacc(__riscv_vle16_v_i16m1(test_case->vd, vl), vs2, vs1);
    case VSCMACCCJ:
      return exec_vscmacccj(__riscv_vle16_v_i16m1(test_case->vd, vl), vs2, vs1);
    case VSCADDCONJ:
      return exec_vscaddconj(vs2, vs1);
    case VSCADDRQ:
      return exec_vscaddrq(vs2, vs1);
    case VSCADDRTQ:
      return exec_vscaddrtq(vs2, vs1);
  }
  return __riscv_vmv_v_x_i16m1(0, vl);
}

static vint16m2_t execute_instruction_m2(const ext_m2_test_case_t *test_case,
                                         size_t vl)
{
  vint16m2_t vs2 = __riscv_vle16_v_i16m2(test_case->vs2, vl);
  vint16m2_t vs1 = __riscv_vle16_v_i16m2(test_case->vs1, vl);
  switch (test_case->op) {
    case VSCMUL:
      return exec_vscmul_m2(vs2, vs1);
    case VSCMULCJ:
      return exec_vscmulcj_m2(vs2, vs1);
    case VSCMACC:
      return exec_vscmacc_m2(__riscv_vle16_v_i16m2(test_case->vd, vl), vs2, vs1);
    case VSCMACCCJ:
      return exec_vscmacccj_m2(__riscv_vle16_v_i16m2(test_case->vd, vl), vs2, vs1);
    case VSCADDCONJ:
      return exec_vscaddconj_m2(vs2, vs1);
    case VSCADDRQ:
      return exec_vscaddrq_m2(vs2, vs1);
    case VSCADDRTQ:
      return exec_vscaddrtq_m2(vs2, vs1);
  }
  return __riscv_vmv_v_x_i16m2(0, vl);
}

static void print_vector(const char *label, const int16_t *vector, int lanes)
{
  printf("  %-8s [", label);
  for (int lane = 0; lane < lanes; ++lane) {
    printf("(%d,%d)%s", (int)vector[2 * lane], (int)vector[2 * lane + 1],
           lane == lanes - 1 ? "" : ", ");
  }
  printf("]\n");
}

static uint32_t pack_complex(const int16_t *vector, int lane)
{
  return (uint32_t)(uint16_t)vector[2 * lane] |
         ((uint32_t)(uint16_t)vector[2 * lane + 1] << 16);
}

static void print_compact_result(const ext_test_case_t *test_case,
                                 const int16_t *actual,
                                 unsigned actual_vxsat,
                                 int pass)
{
  printf("[%s] E=%08x,%08x,%08x,%08x A=%08x,%08x,%08x,%08x "
         "sat=%u/%u %s\n",
         test_case->name,
         pack_complex(test_case->expected, 0),
         pack_complex(test_case->expected, 1),
         pack_complex(test_case->expected, 2),
         pack_complex(test_case->expected, 3),
         pack_complex(actual, 0), pack_complex(actual, 1),
         pack_complex(actual, 2), pack_complex(actual, 3),
         test_case->expected_vxsat, actual_vxsat,
         pass ? "PASS" : "FAIL");
}

static int run_case(const ext_test_case_t *test_case, int print_values)
{
  int16_t actual[8] __attribute__((aligned(16)));
  int pass = 1;
  const size_t vl = set_e16m1_vl8();
  if (vl != 8) {
    printf("[%s] invalid VL: expected=8 actual=%lu\n",
           test_case->name, (unsigned long)vl);
    return 0;
  }

  set_vxrm_clear_vxsat(test_case->vxrm);
  __riscv_vse16_v_i16m1(actual, execute_instruction(test_case, vl), vl);
  const unsigned actual_vxsat = read_vxsat();

  for (int element = 0; element < 8; ++element) {
    if (actual[element] != test_case->expected[element]) {
      pass = 0;
    }
  }
  if (actual_vxsat != test_case->expected_vxsat) {
    pass = 0;
  }

  if (print_values || !pass) {
    print_compact_result(test_case, actual, actual_vxsat, pass);
  }
  if (!pass) {
    print_vector("vs2", test_case->vs2, 4);
    print_vector("vs1", test_case->vs1, 4);
    if (test_case->op == VSCMACC || test_case->op == VSCMACCCJ) {
      print_vector("vd-in", test_case->vd, 4);
    }
  }
  return pass;
}

static int run_m2_case(const ext_m2_test_case_t *test_case)
{
  int16_t actual[16] __attribute__((aligned(32)));
  int pass = 1;
  const size_t vl = set_e16m2_vl16();
  if (vl != 16) {
    printf("[%s] invalid VL: expected=16 actual=%lu\n",
           test_case->name, (unsigned long)vl);
    return 0;
  }

  set_vxrm_clear_vxsat(test_case->vxrm);
  __riscv_vse16_v_i16m2(actual, execute_instruction_m2(test_case, vl), vl);
  const unsigned actual_vxsat = read_vxsat();
  for (int element = 0; element < 16; ++element) {
    if (actual[element] != test_case->expected[element]) {
      pass = 0;
    }
  }
  if (actual_vxsat != test_case->expected_vxsat) {
    pass = 0;
  }

  if (!pass) {
    printf("[%s] sat=%u/%u FAIL\n", test_case->name,
           test_case->expected_vxsat, actual_vxsat);
    print_vector("vs2", test_case->vs2, 8);
    print_vector("vs1", test_case->vs1, 8);
    if (test_case->op == VSCMACC || test_case->op == VSCMACCCJ) {
      print_vector("vd-in", test_case->vd, 8);
    }
    print_vector("expect", test_case->expected, 8);
    print_vector("actual", actual, 8);
  }
  return pass;
}

static const ext_test_case_t *find_case(const char *name)
{
  for (size_t index = 0; index < EXT_TEST_CASE_COUNT; ++index) {
    const char *lhs = kExtTestCases[index].name;
    const char *rhs = name;
    while (*lhs && *lhs == *rhs) {
      ++lhs;
      ++rhs;
    }
    if (*lhs == '\0' && *rhs == '\0') {
      return &kExtTestCases[index];
    }
  }
  return NULL;
}

static int run_vxsat_sticky_case(void)
{
  const ext_test_case_t *saturating = find_case("vscmul_saturation_rnu");
  const ext_test_case_t *normal = find_case("vscmul_normal_rnu");
  int16_t ignored[8] __attribute__((aligned(16)));
  if (saturating == NULL || normal == NULL || set_e16m1_vl8() != 8) {
    printf("[vxsat_sticky] setup FAIL\n");
    return 0;
  }

  set_vxrm_clear_vxsat(0);
  __riscv_vse16_v_i16m1(ignored, execute_instruction(saturating, 8), 8);
  const unsigned after_saturation = read_vxsat();
  __riscv_vse16_v_i16m1(ignored, execute_instruction(normal, 8), 8);
  const unsigned after_normal = read_vxsat();
  clear_vxsat();
  const unsigned after_clear = read_vxsat();
  const int pass = after_saturation == 1 && after_normal == 1 && after_clear == 0;

  printf("[vxsat_sticky] after_sat=%u after_non_sat=%u after_clear=%u\n",
         after_saturation, after_normal, after_clear);
  printf("  result   %s\n", pass ? "PASS" : "FAIL");
  return pass;
}

int main(void)
{
  size_t passed = 0;
  enable_vector_state();

  printf("KMHv2 EXT instruction fixed-vector test\n");
  printf("cases: %lu m1 + %lu m2 arithmetic + 1 vxsat state sequence\n",
         (unsigned long)EXT_TEST_CASE_COUNT,
         (unsigned long)EXT_M2_TEST_CASE_COUNT);

  for (size_t index = 0; index < EXT_TEST_CASE_COUNT; ++index) {
    const int first_case_for_instruction =
        index == 0 || kExtTestCases[index].op != kExtTestCases[index - 1].op;
    passed += (size_t)run_case(&kExtTestCases[index],
                               first_case_for_instruction);
  }
  size_t m2_passed = 0;
  for (size_t index = 0; index < EXT_M2_TEST_CASE_COUNT; ++index) {
    m2_passed += (size_t)run_m2_case(&kExtM2TestCases[index]);
  }
  passed += m2_passed;
  printf("[e16,m2,vl=16] %lu/%lu %s\n",
         (unsigned long)m2_passed,
         (unsigned long)EXT_M2_TEST_CASE_COUNT,
         m2_passed == EXT_M2_TEST_CASE_COUNT ? "PASS" : "FAIL");

  const int sticky_pass = run_vxsat_sticky_case();
  const size_t total = EXT_TEST_CASE_COUNT + EXT_M2_TEST_CASE_COUNT + 1;
  passed += (size_t)sticky_pass;

  printf("SUMMARY: %lu/%lu cases passed\n",
         (unsigned long)passed, (unsigned long)total);
  printf("EXT instruction fixed-vector test: %s\n",
         passed == total ? "PASS" : "FAIL");
  return passed == total ? 0 : 1;
}
