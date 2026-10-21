#!/usr/bin/env bash
set -euo pipefail

ROOT="${XS_ENV_ROOT:-$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../../../.." && pwd)}"
APP=${ROOT}/nexus-am/apps/mimo

usage() {
  cat <<'EOF'
Usage:
  scripts/check_asm_symbol_counts.sh [--symbol <name>] [--expect-ext-site4-v3] [--expect-site4-rvv-lane-solve-v1] [--expect-site4-rvv-solve-vset-schedule-v1] [--expect-zf4-mode4-microkernel] <asm_path>

Paths may be absolute or relative to the small-letter mimo project root.

The script extracts one linked-assembly symbol and prints static instruction
counts used by the MIMO EQ optimization gates. With --expect-ext-site4-v3 it
also enforces the strict EXT site4 v3 prelude-hoist gate.
With --expect-site4-rvv-lane-solve-v1 it enforces the static gate for the
4-lane RVV solve candidate.
With --expect-site4-rvv-solve-vset-schedule-v1 it additionally enforces the
next vtype-scheduling gate: no shared solve call, exact 4x4 lane-solve
instruction shape, and sharply lower vsetvli/static size.
With --expect-zf4-mode4-microkernel it enforces the local preflight for the
eq_zf_4x4 EXT Schur q15recip mode4 hand-asm/micro-kernel: no hard div/rem, no
out-of-line helper calls, and stack pressure clearly below the current helper.

Optional environment budgets:
  MIMO_ASM_MAX_DIV_SIGNED    fail if signed scalar div count is above this value
  MIMO_ASM_MAX_DIV_UNSIGNED  fail if unsigned scalar div count is above this value
  MIMO_ASM_MAX_VDIV_VV       fail if vector vdiv.vv count is above this value
  MIMO_ASM_MAX_SCALAR_LOAD   fail if scalar load count is above this value
  MIMO_ASM_MAX_STACK_LOAD    fail if stack load count is above this value
  MIMO_ASM_MAX_STACK_STORE   fail if stack store count is above this value
  MIMO_ASM_MAX_STACK_FRAME_BYTES
                              fail if parsed stack frame size is above this value
  MIMO_ASM_MAX_WHOLE_VREG_LOAD
                              fail if whole-vector-register load count is above this value
  MIMO_ASM_MAX_WHOLE_VREG_STORE
                              fail if whole-vector-register store count is above this value
  MIMO_ASM_MAX_V0_REFS        fail if v0 reference count is above this value
  MIMO_ASM_MAX_V28_REFS       fail if v28 reference count is above this value
  MIMO_ASM_MAX_V30_REFS       fail if v30 reference count is above this value
	  MIMO_ASM_MIN_VLE16 / MIMO_ASM_MAX_VLE16
	                              bound unit-stride e16 load count
	  MIMO_ASM_MIN_E16_M4_VSET / MIMO_ASM_MAX_E16_M4_VSET
	                              bound e16,m4 vset count
	  MIMO_ASM_MIN_RX4_M4_GROUP_LOAD / MIMO_ASM_MAX_RX4_M4_GROUP_LOAD
	                              bound v0/v4/v8/v12/v16 e16 load count
	  MIMO_ASM_MIN_CUSTOM_EXT_VIMAC
	                              fail if variable-register EXT complex mul/MAC count is below this value
	  MIMO_ASM_FORBID_HARD_DIV   fail if any scalar/vector div/rem instruction is present
	  MIMO_ASM_FORBID_SEGMENT_LDST
	                              fail if any RVV segment load/store instruction is present
  MIMO_ASM_ZF4_MODE4_MICRO_MAX_STACK_LOAD
                              max stack loads for --expect-zf4-mode4-microkernel,
                              default 20
  MIMO_ASM_ZF4_MODE4_MICRO_MAX_SCALAR_LOAD
                              max scalar loads for --expect-zf4-mode4-microkernel,
                              default 96
  MIMO_ASM_ZF4_MODE4_MICRO_MAX_STACK_STORE
                              max stack stores for --expect-zf4-mode4-microkernel,
                              default 16
  MIMO_ASM_ZF4_MODE4_MICRO_MAX_STACK_FRAME_BYTES
                              max stack frame for --expect-zf4-mode4-microkernel,
                              default 160
EOF
}

symbol=mimo_eq_zf_4x4_ext
expect_ext_site4_v3=0
expect_site4_rvv_lane_solve_v1=0
expect_site4_rvv_solve_vset_schedule_v1=0
expect_zf4_mode4_microkernel=0
asm_path=

while [[ $# -gt 0 ]]; do
  case "$1" in
    -h|--help)
      usage
      exit 0
      ;;
    --symbol)
      symbol=${2:-}
      shift 2
      ;;
    --expect-ext-site4-v3)
      expect_ext_site4_v3=1
      shift
      ;;
    --expect-site4-rvv-lane-solve-v1)
      expect_site4_rvv_lane_solve_v1=1
      shift
      ;;
    --expect-site4-rvv-solve-vset-schedule-v1)
      expect_site4_rvv_solve_vset_schedule_v1=1
      shift
      ;;
    --expect-zf4-mode4-microkernel)
      expect_zf4_mode4_microkernel=1
      shift
      ;;
    *)
      if [[ -n "${asm_path}" ]]; then
        usage >&2
        exit 2
      fi
      asm_path=$1
      shift
      ;;
  esac
done

if [[ -z "${asm_path}" || -z "${symbol}" ]]; then
  usage >&2
  exit 2
fi

if [[ "${asm_path}" != /* ]]; then
  asm_path="${APP}/${asm_path}"
fi

if [[ ! -f "${asm_path}" ]]; then
  echo "Missing assembly: ${asm_path}" >&2
  exit 1
fi

resolve_asm_symbol() {
  local requested=$1
  local asm_file=$2

  awk -v requested="${requested}" '
    /^[[:xdigit:]]+ <[^>]+>:/ {
      name = $0
      sub(/^[[:xdigit:]]+ </, "", name)
      sub(/>:.*/, "", name)
      if (name == requested) {
        print name
        exit
      }
      if (first == "" && index(name, requested ".") == 1) {
        first = name
      }
    }
    END {
      if (first != "") {
        print first
      }
    }
  ' "${asm_file}"
}

resolved_symbol=$(resolve_asm_symbol "${symbol}" "${asm_path}")
if [[ -n "${resolved_symbol}" ]]; then
  symbol=${resolved_symbol}
fi

tmp_symbol=$(mktemp)
trap 'rm -f "${tmp_symbol}"' EXIT

awk -v needle="<${symbol}>:" '
  index($0, needle) { in_symbol = 1; next }
  in_symbol && /^[[:xdigit:]]+ <[^.][^>]*>:/ { in_symbol = 0 }
  in_symbol { print }
' "${asm_path}" > "${tmp_symbol}"

lines=$(wc -l < "${tmp_symbol}" | tr -d ' ')
if [[ "${lines}" -eq 0 ]]; then
  echo "Symbol not found: ${symbol} in ${asm_path}" >&2
  exit 3
fi

count_regex() {
  local pattern=$1
  local tmp_count
  tmp_count=$(mktemp)
  if command -v rg >/dev/null 2>&1; then
    rg -o -- "${pattern}" "${tmp_symbol}" > "${tmp_count}" || true
  else
    grep -Eo -- "${pattern}" "${tmp_symbol}" > "${tmp_count}" || true
  fi
  wc -l < "${tmp_count}" | tr -d ' '
  rm -f "${tmp_count}"
}

count_lines() {
  local pattern=$1
  local count
  if command -v rg >/dev/null 2>&1; then
    count=$(rg -c -- "${pattern}" "${tmp_symbol}" || true)
  else
    count=$(grep -Ec -- "${pattern}" "${tmp_symbol}" || true)
  fi
  if [[ -z "${count}" ]]; then
    count=0
  fi
  printf '%s\n' "${count}"
}

count_vset_transition() {
  local from=$1
  local to=$2
  awk -v from="${from}" -v to="${to}" '
    function vtype(line,   m) {
      if (match(line, /e(16|32|64),m(f2|1|2)/, m)) {
        return m[0]
      }
      return ""
    }
    /vsetivli|vsetvli/ {
      cur = vtype($0)
      if (prev == from && cur == to) {
        cnt++
      }
      if (cur != "") {
        prev = cur
      }
    }
    END { print cnt + 0 }
  ' "${tmp_symbol}"
}

vsetivli=$(count_regex 'vsetivli')
vsetvli=$(count_regex '\bvsetvli\b')
vid=$(count_regex 'vid\.v')
vsll=$(count_regex 'vsll\.vi')
vadd=$(count_regex 'vadd\.vi')
vmask=$(count_regex 'vms(leu|ltu)\.vi')
custom=$(count_regex '0xcf4c0857')
custom_vscmulcj_word=$(count_regex '\bcf4c[0-9a-f]{4}\b')
custom_vscmacc_word=$(count_regex '\bd34c[0-9a-f]{4}\b')
custom_vscmacccj_word=$(count_regex '\bdb4c[0-9a-f]{4}\b')
custom_vscmul_any_word=$(count_regex '\bc[ab][0-9a-f]{4}(57|d7)\b')
custom_vscmulcj_any_word=$(count_regex '\bc[ef][0-9a-f]{4}(57|d7)\b')
custom_vscmacc_any_word=$(count_regex '\bd[23][0-9a-f]{4}(57|d7)\b')
custom_vscmacccj_any_word=$(count_regex '\bd[ab][0-9a-f]{4}(57|d7)\b')
custom_ext_vimac_any_word=$((custom_vscmul_any_word + custom_vscmulcj_any_word + custom_vscmacc_any_word + custom_vscmacccj_any_word))
masked_vsse32=$(count_lines 'vsse32\.v.*v0\.t')
vle32=$(count_regex 'vle32\.v')
vlse32=$(count_regex 'vlse32\.v')
vse32=$(count_regex 'vse32\.v')
vsse32=$(count_regex 'vsse32\.v')
vse16=$(count_regex 'vse16\.v')
vle16=$(count_regex 'vle16\.v')
vset_e16_m4=$(count_lines '\bvset(ivli|vli)\b.*e16,m4')
rx4_m4_group_load=$(count_lines 'vle16\.v[[:space:]]+v(0|4|8|12|16),')
lh=$(count_regex '\blh\b')
jal=$(count_regex '\bjal\b')
jalr=$(count_regex '\bjalr\b')
vrgather=$(count_regex 'vrgather\.vv')
vwadd=$(count_regex 'vwadd\.wv')
vwsub=$(count_regex 'vwsub\.wv')
vdiv_vv=$(count_regex 'vdiv\.vv')
vdiv_vx=$(count_regex 'vdiv\.vx')
vdivu_vv=$(count_regex 'vdivu\.vv')
vdivu_vx=$(count_regex 'vdivu\.vx')
vrem_vv=$(count_regex 'vrem\.vv')
vrem_vx=$(count_regex 'vrem\.vx')
vremu_vv=$(count_regex 'vremu\.vv')
vremu_vx=$(count_regex 'vremu\.vx')
vfdiv=$(count_regex 'vfdiv\.')
vmul_vv=$(count_regex '\bvmul\.vv')
vwmul_vv=$(count_regex 'vwmul\.vv')
vnclip_wi=$(count_regex 'vnclip\.wi')
vsse16=$(count_regex 'vsse16\.v')
segment_ldst=$(count_regex '\bv[ls][a-z]*seg[2-8]e(8|16|32|64)(ff)?\.v')
div_signed=$(count_regex '\bdivw?\b')
div_unsigned=$(count_regex '\bdivuw?\b')
rem_signed=$(count_regex '\bremw?\b')
rem_unsigned=$(count_regex '\bremuw?\b')
conditional_branch=$(count_regex '\b(bnez|beqz|bltz|bgez|blez|bgtz|b(eq|ne|lt|ge|ltu|geu)z?)\b')
scalar_load=$(count_regex '\b(lb|lbu|lh|lhu|lw|lwu|ld)\b')
scalar_store=$(count_regex '\b(sb|sh|sw|sd)\b')
stack_load=$(count_lines '\b(lb|lbu|lh|lhu|lw|lwu|ld)\b.*\([[:space:]]*sp\)')
stack_store=$(count_lines '\b(sb|sh|sw|sd)\b.*\([[:space:]]*sp\)')
whole_vreg_load=$(count_regex '\bvl[0-9]+re(8|16|32|64)\.v\b')
whole_vreg_store=$(count_regex '\bvs[0-9]+r\.v\b')
v0_refs=$(count_regex '\bv0(\.t)?\b')
v28_refs=$(count_regex '\bv28\b')
v30_refs=$(count_regex '\bv30\b')
stack_frame_bytes=$(awk '
  /[[:space:]]addi[[:space:]]+sp,sp,-[0-9]+/ {
    frame = $0
    sub(/^.*addi[[:space:]]+sp,sp,-/, "", frame)
    sub(/[^0-9].*$/, "", frame)
    print frame
    found = 1
    exit
  }
  END {
    if (!found) {
      print 0
    }
  }
' "${tmp_symbol}")
solve4_call=$(count_regex 'mimo_solve_normal4_site4_q15')
build_normal_col_call=$(count_regex 'mimo_build_normal_col_q15')
solve_normal_call=$(count_regex 'mimo_solve_normal_q15')
vset_e64_to_e32=$(count_vset_transition 'e64,m2' 'e32,m1')
vset_e32_to_e64=$(count_vset_transition 'e32,m1' 'e64,m2')
vset_e64_to_e16=$(count_vset_transition 'e64,m2' 'e16,mf2')
vset_e16_to_e64=$(count_vset_transition 'e16,mf2' 'e64,m2')

printf 'asm=%s\n' "${asm_path}"
printf 'symbol=%s\n' "${symbol}"
printf 'lines=%s\n' "${lines}"
printf 'vsetivli=%s\n' "${vsetivli}"
printf 'vsetvli=%s\n' "${vsetvli}"
printf 'vid_v=%s\n' "${vid}"
printf 'vsll_vi=%s\n' "${vsll}"
printf 'vadd_vi=%s\n' "${vadd}"
printf 'vmsleu_or_vmsltu_vi=%s\n' "${vmask}"
printf 'custom_0xcf4c0857=%s\n' "${custom}"
printf 'custom_vscmulcj_word=%s\n' "${custom_vscmulcj_word}"
printf 'custom_vscmacc_word=%s\n' "${custom_vscmacc_word}"
printf 'custom_vscmacccj_word=%s\n' "${custom_vscmacccj_word}"
printf 'custom_vscmul_any_word=%s\n' "${custom_vscmul_any_word}"
printf 'custom_vscmulcj_any_word=%s\n' "${custom_vscmulcj_any_word}"
printf 'custom_vscmacc_any_word=%s\n' "${custom_vscmacc_any_word}"
printf 'custom_vscmacccj_any_word=%s\n' "${custom_vscmacccj_any_word}"
printf 'custom_ext_vimac_any_word=%s\n' "${custom_ext_vimac_any_word}"
printf 'masked_vsse32_v0_t=%s\n' "${masked_vsse32}"
printf 'vle32_v=%s\n' "${vle32}"
printf 'vlse32_v=%s\n' "${vlse32}"
printf 'vse32_v=%s\n' "${vse32}"
printf 'vsse32_v=%s\n' "${vsse32}"
printf 'vse16_v=%s\n' "${vse16}"
printf 'vle16_v=%s\n' "${vle16}"
printf 'vset_e16_m4=%s\n' "${vset_e16_m4}"
printf 'rx4_m4_group_load=%s\n' "${rx4_m4_group_load}"
printf 'lh=%s\n' "${lh}"
printf 'jal=%s\n' "${jal}"
printf 'jalr=%s\n' "${jalr}"
printf 'vrgather_vv=%s\n' "${vrgather}"
printf 'vwadd_wv=%s\n' "${vwadd}"
printf 'vwsub_wv=%s\n' "${vwsub}"
printf 'vdiv_vv=%s\n' "${vdiv_vv}"
printf 'vdiv_vx=%s\n' "${vdiv_vx}"
printf 'vdivu_vv=%s\n' "${vdivu_vv}"
printf 'vdivu_vx=%s\n' "${vdivu_vx}"
printf 'vrem_vv=%s\n' "${vrem_vv}"
printf 'vrem_vx=%s\n' "${vrem_vx}"
printf 'vremu_vv=%s\n' "${vremu_vv}"
printf 'vremu_vx=%s\n' "${vremu_vx}"
printf 'vfdiv=%s\n' "${vfdiv}"
printf 'vmul_vv=%s\n' "${vmul_vv}"
printf 'vwmul_vv=%s\n' "${vwmul_vv}"
printf 'vnclip_wi=%s\n' "${vnclip_wi}"
printf 'vsse16_v=%s\n' "${vsse16}"
printf 'segment_load_store=%s\n' "${segment_ldst}"
printf 'div_signed=%s\n' "${div_signed}"
printf 'div_unsigned=%s\n' "${div_unsigned}"
printf 'rem_signed=%s\n' "${rem_signed}"
printf 'rem_unsigned=%s\n' "${rem_unsigned}"
printf 'conditional_branch=%s\n' "${conditional_branch}"
printf 'scalar_load=%s\n' "${scalar_load}"
printf 'scalar_store=%s\n' "${scalar_store}"
printf 'stack_load=%s\n' "${stack_load}"
printf 'stack_store=%s\n' "${stack_store}"
printf 'whole_vreg_load=%s\n' "${whole_vreg_load}"
printf 'whole_vreg_store=%s\n' "${whole_vreg_store}"
printf 'v0_refs=%s\n' "${v0_refs}"
printf 'v28_refs=%s\n' "${v28_refs}"
printf 'v30_refs=%s\n' "${v30_refs}"
printf 'stack_frame_bytes=%s\n' "${stack_frame_bytes}"
printf 'mimo_solve_normal4_site4_q15_refs=%s\n' "${solve4_call}"
printf 'mimo_build_normal_col_q15_refs=%s\n' "${build_normal_col_call}"
printf 'mimo_solve_normal_q15_refs=%s\n' "${solve_normal_call}"
printf 'vset_transition_e64m2_to_e32m1=%s\n' "${vset_e64_to_e32}"
printf 'vset_transition_e32m1_to_e64m2=%s\n' "${vset_e32_to_e64}"
printf 'vset_transition_e64m2_to_e16mf2=%s\n' "${vset_e64_to_e16}"
printf 'vset_transition_e16mf2_to_e64m2=%s\n' "${vset_e16_to_e64}"

budget_failed=0
expect_max_env() {
  local env_name=$1
  local name=$2
  local actual=$3
  local maximum=${!env_name:-}

  if [[ -z "${maximum}" ]]; then
    return
  fi
  if [[ ! "${maximum}" =~ ^[0-9]+$ ]]; then
    echo "Invalid ${env_name}='${maximum}'; expected non-negative integer." >&2
    exit 2
  fi
  if (( actual > maximum )); then
    echo "EXPECT_FAIL ${name}: expected<=${maximum} actual=${actual}" >&2
    budget_failed=1
  fi
}

expect_max_env MIMO_ASM_MAX_DIV_SIGNED div_signed "${div_signed}"
expect_max_env MIMO_ASM_MAX_DIV_UNSIGNED div_unsigned "${div_unsigned}"
expect_max_env MIMO_ASM_MAX_VDIV_VV vdiv_vv "${vdiv_vv}"
expect_max_env MIMO_ASM_MAX_SCALAR_LOAD scalar_load "${scalar_load}"
expect_max_env MIMO_ASM_MAX_STACK_LOAD stack_load "${stack_load}"
expect_max_env MIMO_ASM_MAX_STACK_STORE stack_store "${stack_store}"
expect_max_env MIMO_ASM_MAX_STACK_FRAME_BYTES stack_frame_bytes "${stack_frame_bytes}"
expect_max_env MIMO_ASM_MAX_WHOLE_VREG_LOAD whole_vreg_load "${whole_vreg_load}"
expect_max_env MIMO_ASM_MAX_WHOLE_VREG_STORE whole_vreg_store "${whole_vreg_store}"
expect_max_env MIMO_ASM_MAX_V0_REFS v0_refs "${v0_refs}"
expect_max_env MIMO_ASM_MAX_V28_REFS v28_refs "${v28_refs}"
expect_max_env MIMO_ASM_MAX_V30_REFS v30_refs "${v30_refs}"
expect_max_env MIMO_ASM_MAX_VLE16 vle16_v "${vle16}"
expect_max_env MIMO_ASM_MAX_E16_M4_VSET vset_e16_m4 "${vset_e16_m4}"
expect_max_env MIMO_ASM_MAX_RX4_M4_GROUP_LOAD rx4_m4_group_load "${rx4_m4_group_load}"

expect_min_env() {
  local env_name=$1
  local name=$2
  local actual=$3
  local minimum=${!env_name:-}

  if [[ -z "${minimum}" ]]; then
    return
  fi
  if [[ ! "${minimum}" =~ ^[0-9]+$ ]]; then
    echo "Invalid ${env_name}='${minimum}'; expected non-negative integer." >&2
    exit 2
  fi
  if (( actual < minimum )); then
    echo "EXPECT_FAIL ${name}: expected>=${minimum} actual=${actual}" >&2
    budget_failed=1
  fi
}

expect_min_env MIMO_ASM_MIN_CUSTOM_EXT_VIMAC custom_ext_vimac_any_word "${custom_ext_vimac_any_word}"
expect_min_env MIMO_ASM_MIN_VLE16 vle16_v "${vle16}"
expect_min_env MIMO_ASM_MIN_E16_M4_VSET vset_e16_m4 "${vset_e16_m4}"
expect_min_env MIMO_ASM_MIN_RX4_M4_GROUP_LOAD rx4_m4_group_load "${rx4_m4_group_load}"

if [[ "${budget_failed}" -ne 0 ]]; then
  exit 8
fi

if [[ "${MIMO_ASM_FORBID_HARD_DIV:-0}" == "1" ]]; then
  hard_div_total=$((div_signed + div_unsigned + rem_signed + rem_unsigned + vdiv_vv + vdiv_vx + vdivu_vv + vdivu_vx + vrem_vv + vrem_vx + vremu_vv + vremu_vx + vfdiv))
  if (( hard_div_total > 0 )); then
    echo "EXPECT_FAIL hard_div_forbidden: scalar_div=${div_signed}/${div_unsigned} scalar_rem=${rem_signed}/${rem_unsigned} vector_div_vv_vx=${vdiv_vv}/${vdiv_vx}/${vdivu_vv}/${vdivu_vx} vector_rem_vv_vx=${vrem_vv}/${vrem_vx}/${vremu_vv}/${vremu_vx} vfdiv=${vfdiv}" >&2
    exit 8
  fi
  echo "EXPECT_PASS hard-div-forbidden"
fi

if [[ "${MIMO_ASM_FORBID_SEGMENT_LDST:-0}" == "1" ]]; then
  if (( segment_ldst > 0 )); then
    echo "EXPECT_FAIL segment_load_store_forbidden: actual=${segment_ldst}" >&2
    exit 8
  fi
  echo "EXPECT_PASS segment-load-store-forbidden"
fi

if [[ "${expect_ext_site4_v3}" -eq 1 ]]; then
  failed=0
  expect_eq() {
    local name=$1
    local actual=$2
    local expected=$3
    if [[ "${actual}" != "${expected}" ]]; then
      echo "EXPECT_FAIL ${name}: expected=${expected} actual=${actual}" >&2
      failed=1
    fi
  }

  expect_eq vsetivli "${vsetivli}" 1
  expect_eq vsetvli "${vsetvli}" 0
  expect_eq vid_v "${vid}" 1
  expect_eq vsll_vi "${vsll}" 1
  expect_eq vadd_vi "${vadd}" 1
  expect_eq vmsleu_or_vmsltu_vi "${vmask}" 1
  expect_eq custom_0xcf4c0857 "${custom}" 3
  expect_eq masked_vsse32_v0_t "${masked_vsse32}" 8
  expect_eq vse16_v "${vse16}" 0
  expect_eq lh "${lh}" 0

  if grep -q '<mimo_ext_dot4_sites_store_conj_q15>:' "${asm_path}"; then
    echo "EXPECT_FAIL helper_symbol: mimo_ext_dot4_sites_store_conj_q15 is present" >&2
    failed=1
  fi

  if [[ "${failed}" -ne 0 ]]; then
    exit 4
  fi
  echo "EXPECT_PASS ext-site4-v3"
fi

if [[ "${expect_site4_rvv_lane_solve_v1}" -eq 1 ]]; then
  failed=0
  expect_zero() {
    local name=$1
    local actual=$2
    if [[ "${actual}" != "0" ]]; then
      echo "EXPECT_FAIL ${name}: expected=0 actual=${actual}" >&2
      failed=1
    fi
  }
  expect_ge() {
    local name=$1
    local actual=$2
    local minimum=$3
    if (( actual < minimum )); then
      echo "EXPECT_FAIL ${name}: expected>=${minimum} actual=${actual}" >&2
      failed=1
    fi
  }

  expect_zero mimo_solve_normal4_site4_q15_refs "${solve4_call}"
  expect_zero jal "${jal}"
  expect_zero jalr "${jalr}"
  expect_ge vdiv_vv "${vdiv_vv}" 1
  expect_ge vwmul_vv "${vwmul_vv}" 1
  expect_ge vnclip_wi "${vnclip_wi}" 1
  expect_ge vsse16_v "${vsse16}" 1

  if [[ "${failed}" -ne 0 ]]; then
    exit 5
  fi
  echo "EXPECT_PASS site4-rvv-lane-solve-v1"
fi

if [[ "${expect_site4_rvv_solve_vset_schedule_v1}" -eq 1 ]]; then
  failed=0
  expect_zero() {
    local name=$1
    local actual=$2
    if [[ "${actual}" != "0" ]]; then
      echo "EXPECT_FAIL ${name}: expected=0 actual=${actual}" >&2
      failed=1
    fi
  }
  expect_eq() {
    local name=$1
    local actual=$2
    local expected=$3
    if [[ "${actual}" != "${expected}" ]]; then
      echo "EXPECT_FAIL ${name}: expected=${expected} actual=${actual}" >&2
      failed=1
    fi
  }
  expect_le() {
    local name=$1
    local actual=$2
    local maximum=$3
    if (( actual > maximum )); then
      echo "EXPECT_FAIL ${name}: expected<=${maximum} actual=${actual}" >&2
      failed=1
    fi
  }

  expect_zero mimo_solve_normal4_site4_q15_refs "${solve4_call}"
  expect_zero jal "${jal}"
  expect_zero jalr "${jalr}"
  expect_zero div_signed "${div_signed}"
  expect_zero div_unsigned "${div_unsigned}"
  expect_eq vdiv_vv "${vdiv_vv}" 20
  expect_eq vnclip_wi "${vnclip_wi}" 8
  expect_le vsetvli "${vsetvli}" 80
  expect_le lines "${lines}" 1100
  expect_le vlse32_v "${vlse32}" 4
  expect_le vsse32_v "${vsse32}" 4
  expect_le vset_transition_e64m2_to_e32m1 "${vset_e64_to_e32}" 40
  expect_le vset_transition_e32m1_to_e64m2 "${vset_e32_to_e64}" 40

  if [[ "${failed}" -ne 0 ]]; then
    exit 6
  fi
  echo "EXPECT_PASS site4-rvv-solve-vset-schedule-v1"
fi

if [[ "${expect_zf4_mode4_microkernel}" -eq 1 ]]; then
  failed=0
  max_scalar_load=${MIMO_ASM_ZF4_MODE4_MICRO_MAX_SCALAR_LOAD:-96}
  max_stack_load=${MIMO_ASM_ZF4_MODE4_MICRO_MAX_STACK_LOAD:-20}
  max_stack_store=${MIMO_ASM_ZF4_MODE4_MICRO_MAX_STACK_STORE:-16}
  max_stack_frame=${MIMO_ASM_ZF4_MODE4_MICRO_MAX_STACK_FRAME_BYTES:-160}

  expect_zero() {
    local name=$1
    local actual=$2
    if [[ "${actual}" != "0" ]]; then
      echo "EXPECT_FAIL ${name}: expected=0 actual=${actual}" >&2
      failed=1
    fi
  }
  expect_le() {
    local name=$1
    local actual=$2
    local maximum=$3
    if [[ ! "${maximum}" =~ ^[0-9]+$ ]]; then
      echo "Invalid zf4-mode4-microkernel maximum for ${name}: ${maximum}" >&2
      exit 2
    fi
    if (( actual > maximum )); then
      echo "EXPECT_FAIL ${name}: expected<=${maximum} actual=${actual}" >&2
      failed=1
    fi
  }

  expect_zero jal "${jal}"
  expect_zero jalr "${jalr}"
  expect_zero div_signed "${div_signed}"
  expect_zero div_unsigned "${div_unsigned}"
  expect_zero rem_signed "${rem_signed}"
  expect_zero rem_unsigned "${rem_unsigned}"
  expect_zero vdiv_vv "${vdiv_vv}"
  expect_zero vdiv_vx "${vdiv_vx}"
  expect_zero vdivu_vv "${vdivu_vv}"
  expect_zero vdivu_vx "${vdivu_vx}"
  expect_zero vrem_vv "${vrem_vv}"
  expect_zero vrem_vx "${vrem_vx}"
  expect_zero vremu_vv "${vremu_vv}"
  expect_zero vremu_vx "${vremu_vx}"
  expect_zero vfdiv "${vfdiv}"
  expect_le scalar_load "${scalar_load}" "${max_scalar_load}"
  expect_le stack_load "${stack_load}" "${max_stack_load}"
  expect_le stack_store "${stack_store}" "${max_stack_store}"
  expect_le stack_frame_bytes "${stack_frame_bytes}" "${max_stack_frame}"

  if [[ "${failed}" -ne 0 ]]; then
    exit 9
  fi
  echo "EXPECT_PASS zf4-mode4-microkernel"
fi
