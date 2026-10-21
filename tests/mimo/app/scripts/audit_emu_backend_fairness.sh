#!/usr/bin/env bash
set -euo pipefail

ROOT="${XS_ENV_ROOT:-$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../../../.." && pwd)}"
APP=${ROOT}/nexus-am/apps/mimo

expected_backends=${MIMO_EMU_FAIRNESS_BACKENDS:-"scalar rvv ext"}
allow_partial=0
entries=()

usage() {
  cat <<'EOF'
Usage:
  scripts/audit_emu_backend_fairness.sh \
    --entry scalar=<log_path>,<manifest_path> \
    --entry rvv=<log_path>,<manifest_path> \
    --entry ext=<log_path>,<manifest_path>

Purpose:
  Read-only audit for a formal scalar/RVV/EXT comparison group.
  It refuses to validate a group that mixes direct-div allowed and no-harddiv
  results, validation tolerance, ROI shape, or division/reciprocal strategy.
  Backend-specific solve/layout choices are reported by default; set
  MIMO_EMU_FAIRNESS_STRICT_STRATEGY=1 when auditing a synchronized
  rebaseline that must use identical implementation flags.

Options:
  --allow-partial    Audit only the provided backends. Formal tables should not use this.
  --entry ...        Backend, log, and snapshot manifest. May be repeated.

Paths may be absolute or relative to the small-letter mimo project root.
EOF
}

if [[ "${1:-}" == "-h" || "${1:-}" == "--help" ]]; then
  usage
  exit 0
fi

while [[ $# -gt 0 ]]; do
  case "$1" in
    --allow-partial)
      allow_partial=1
      shift
      ;;
    --entry)
      entries+=("${2:-}")
      shift 2
      ;;
    *)
      usage >&2
      exit 2
      ;;
  esac
done

if [[ "${#entries[@]}" -eq 0 ]]; then
  usage >&2
  exit 2
fi

resolve_path() {
  local path=$1
  if [[ "${path}" == /* ]]; then
    printf '%s\n' "${path}"
  else
    printf '%s/%s\n' "${APP}" "${path}"
  fi
}

summary_value() {
  local file=$1
  local key=$2
  awk -F '=' -v k="${key}" '$1 == k { print $2; found = 1; exit } END { if (!found) exit 1 }' "${file}"
}

summary_value_or() {
  local file=$1
  local key=$2
  local fallback=$3
  summary_value "${file}" "${key}" || printf '%s\n' "${fallback}"
}

manifest_value() {
  local file=$1
  local key=$2
  awk -F '=' -v k="${key}" '$1 == k { print $2; found = 1; exit } END { if (!found) exit 1 }' "${file}"
}

manifest_value_or() {
  local file=$1
  local key=$2
  local fallback=$3
  manifest_value "${file}" "${key}" || printf '%s\n' "${fallback}"
}

manifest_strategy_value_or() {
  local file=$1
  local key=$2
  local fallback=$3
  if manifest_value "${file}" "${key}"; then
    return 0
  fi

	  case "${key}" in
	    MIMO_EQ2_RECIP_MODE)
	      printf '5\n'
	      ;;
	    MIMO_EQ2_RECIP_NUM_SHIFT)
	      printf '13\n'
	      ;;
	    MIMO_USE_EXT_EQ2_INVCOEFF)
	      printf '1\n'
	      ;;
	    MIMO_USE_EQ2_ZF_DIRECT_H|MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP|MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID|MIMO_USE_EXT_EQ2_MMSE_ADAPTIVE_Q13|MIMO_USE_EQ2_MMSE_DIRECT_H|MIMO_USE_SCALAR_EQ2_FUSED_LAMBDA|MIMO_USE_SCALAR_EQ2_MMSE_FULL_TRUNC_SCALE|MIMO_USE_RVV_EQ2_M1|MIMO_USE_RVV_EQ2_ALWAYS_ADD_LAMBDA|MIMO_USE_RVV_EQ2_SPLIT_BUILD|MIMO_USE_RVV_EQ2_FUSED_NCLIP_BUILD|MIMO_USE_RVV_EQ2_TX_SOA_STORE|MIMO_USE_RVV_EQ2_3M_BUILD|MIMO_USE_RVV_EQ2_ZEXT_PACK|MIMO_USE_RVV_EQ2_RI_SOA_OUTPUT|MIMO_USE_RVV_EQ2_STAGED_NARROW_BUILD|MIMO_USE_RVV_EQ2_IMPLICIT_VXRM|MIMO_USE_RVV_EQ2_UQ12_SCALE|MIMO_USE_RVV_EQ2_EARLY_RECIP_OVERLAP|MIMO_USE_RVV_EQ2_MMSE_FULL_TRUNC_SCALE|MIMO_USE_RVV_EQ2_BLOCK_SOA|MIMO_USE_RVV_EQ2_GROUP_LOAD|MIMO_USE_RVV_EQ2_GROUP_LOAD_M4|MIMO_USE_EXT_EQ2_M2_BATCH|MIMO_USE_EXT_EQ2_RHS_INTERLEAVE)
	      printf '0\n'
	      ;;
	    MIMO_USE_RVV_ZF4_FUSED_BUILD_SOLVE|MIMO_USE_RVV_ZF4_SEED_ACC|MIMO_USE_RVV_MMSE4_EARLY_TAIL_RHS|MIMO_USE_RVV_MMSE4_CONST_VL|MIMO_USE_RVV_MMSE4_SEED_ACC|MIMO_USE_RVV_MMSE4_RI_PACKED_LOAD|MIMO_USE_RVV_MMSE4_BINV_Q13_REUSE|MIMO_USE_RVV_MMSE4_SCHUR_TRUE16B_SOLVE|MIMO_USE_RVV_MMSE4_SCHUR_TRUE16B_SHARED_A16|MIMO_USE_RVV_SCHUR4_SOLVE2_PAIR_M2|MIMO_USE_RVV_SCHUR4_NUM_MAC|MIMO_USE_RVV_SCHUR4_COMPLEX_MAC|MIMO_USE_RVV_SCHUR4_VSSRA_ROUND|MIMO_USE_RVV_SITE4_VSSRA_ROUND|MIMO_USE_RVV_SITE4_SEG2_LOAD|MIMO_USE_RVV_SITE4_RI_SOA|MIMO_USE_RVV_SCHUR4_DET_MAC)
      printf '0\n'
      ;;
    MIMO_USE_EXT_MMSE4_RVV_I32_BINV_REUSE)
      printf '0\n'
      ;;
    MIMO_USE_EXT_MMSE4_RVV_I32_PACKED_PHASE)
      printf '0\n'
      ;;
    MIMO_EXT_MMSE4_PACKED_TOP_EXT)
      printf '1\n'
      ;;
    MIMO_USE_SCALAR4_SCHUR_TOP_Q30_ONCE|MIMO_USE_SCALAR_MMSE4_FUSED_BUILD_SOLVE|MIMO_USE_SCALAR_MMSE4_FUSED_COMPACT_C32)
      printf '0\n'
      ;;
    MIMO_USE_SOLVE4_SCHUR_DET_SCALE|MIMO_USE_SOLVE4_SCHUR_BINV_Q15|MIMO_USE_SOLVE4_SCHUR_Q15_RECIP|MIMO_USE_SOLVE4_SCHUR_PAIR2|MIMO_USE_SOLVE4_SCHUR_FUSED_INNER|MIMO_USE_SOLVE4_SCHUR_FUSED_INNER_MMSE|MIMO_USE_SOLVE4_SCHUR_MMSE_SPECIALIZE|MIMO_USE_SOLVE4_SCHUR_ZF_EXPLICIT|MIMO_USE_SOLVE4_SCHUR_ZF_TOP_RESOLVE|MIMO_USE_SOLVE4_SCHUR_ZF_HANDASM|MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER|MIMO_USE_SOLVE4_SCHUR_ZF_EXT_MACC_INNER|MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_MASK|MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_MODEL|MIMO_CHECK_SOLVE2H_GROUP_LAYOUT|MIMO_USE_EQ4_DIRECT_H|MIMO_EQ4_DIRECT_H_MODE|MIMO_EQ4_DIRECT_H_NODIV|MIMO_USE_SOLVE_Q15_I64_ARSHIFT_ROUND|MIMO_USE_SOLVE_STORE_TRUST_I16|MIMO_USE_SCALAR4_UPPER_ONLY_BUILD|MIMO_USE_RVV_SOLVE4_Q15_I32_MUL|MIMO_USE_RVV_ZF4_SCHUR_RVV_SOLVE|MIMO_USE_RVV_MMSE4_SCHUR_RVV_SOLVE|MIMO_USE_RVV_SCHUR4_VSMUL_SCALE|MIMO_USE_MMSE4_SCHUR_NO_MAG_SCALE|MIMO_USE_MMSE4_SCHUR_SIGNED_TRUNC_SCALE|MIMO_USE_MMSE4_SCHUR_NUM_ARSHIFT_SCALE|MIMO_USE_MMSE4_SCHUR_INNER_Q30_ONCE|MIMO_USE_RVV_MMSE4_SCHUR_INNER_SYM_ROUND|MIMO_USE_RVV_MMSE4_SCHUR_TOP_Q30_ONCE|MIMO_USE_RVV_MMSE4_STAGED_BUILD_SOLVE|MIMO_USE_RVV_ZF4_STAGED_BUILD_SOLVE|MIMO_USE_EXT_MMSE4_SCHUR_FINAL_SIGNED_TRUNC_SCALE|MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK|MIMO_USE_EXT_MMSE4_RVV_I32_VEC_LAMBDA|MIMO_USE_EXT_MMSE4_RVV_I32_LATE_TAIL_LOAD|MIMO_USE_EXT_MMSE4_RVV_I32_SPECIALIZE|MIMO_USE_EXT_MMSE4_RVV_I32_INNER_Q30_ONCE|MIMO_USE_EXT_MMSE4_RVV_I32_TOP_Q30_ONCE|MIMO_USE_EXT_MMSE4_RVV_I32_ASSUME_LAMBDA_NOSAT|MIMO_USE_RVV_ZF4_PACK32_STORE|MIMO_USE_SITE4_HERMITIAN_UPPER_MMSE|MIMO_USE_SITE4_HERMITIAN_UPPER_ROWFUSE|MIMO_USE_SITE4_HERMITIAN_UPPER_ROWPAIR|MIMO_USE_SITE4_ROUND_ONCE_DOT|MIMO_USE_SITE4_BUILD_SOLVE_PIPELINE2|MIMO_USE_EXT_ZF4_RVV_NODIV_FAST_SOLVE|MIMO_USE_EXT_ZF4_SCHUR_RVV_SOLVE|MIMO_USE_EXT_MMSE4_SCHUR_RVV_SOLVE|MIMO_USE_EXT_MMSE4_TRUE16B_LIVE_TAIL_SOLVE|MIMO_USE_EXT_ZF4_RVV_ROUND_ONCE_BUILD|MIMO_USE_EXT_ZF4_TRUE16B_WORK|MIMO_USE_EXT_ZF4_PACK32_WORK|MIMO_USE_EXT_ZF4_VSCMACC_BUILD|MIMO_USE_EXT_ZF4_VSCMACC_FUSED_BUILD|MIMO_USE_EXT_ZF4_INLINE_UPPER_I16_SOLVE|MIMO_USE_EXT_ZF4_LIVE_TAIL_SCHUR|MIMO_USE_EXT_ZF4_TAIL_SCRATCH_REUSE|MIMO_USE_EXT_ZF4_INLINE_HEAD_WORK_REUSE|MIMO_USE_EXT_ZF4_TOP2_MACC_SUM|MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE|MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_BSOLVE|MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_STORE|MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_LOAD|MIMO_EXT_ZF4_FAST_SCALE_BIAS_Q30|MIMO_EXT_ZF4_FAST_REUSE_SCALE|MIMO_USE_EXT_ZF4_UPPER_RVV_SOLVE|MIMO_USE_EXT_ZF4_MIXED_UPDATE_SOLVE|MIMO_DEBUG_EXT_ZF4_RVV_NODIV_COMPARE)
      printf '0\n'
      ;;
    MIMO_EQ4_DIRECT_H_ITERS)
      printf '3\n'
      ;;
    MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT)
      printf '15\n'
      ;;
    MIMO_EXT_ZF4_FAST_CORR_ITERS)
      printf '8\n'
      ;;
    *)
      printf '%s\n' "${fallback}"
      ;;
  esac
}

tmp_dir=$(mktemp -d)
trap 'rm -rf "${tmp_dir}"' EXIT

declare -A log_by_backend
declare -A manifest_by_backend
declare -A summary_by_backend
declare -A seen_backend

failed=0

fail() {
  echo "FAIRNESS_FAIL $*" >&2
  failed=1
}

for entry in "${entries[@]}"; do
  if [[ "${entry}" != *=*,* ]]; then
    fail "invalid_entry=${entry}"
    continue
  fi

  backend=${entry%%=*}
  rest=${entry#*=}
  log_path=${rest%%,*}
  manifest_path=${rest#*,}

  if [[ -z "${backend}" || -z "${log_path}" || -z "${manifest_path}" || "${log_path}" == "${manifest_path}" ]]; then
    fail "invalid_entry=${entry}"
    continue
  fi
  if [[ -n "${seen_backend[${backend}]:-}" ]]; then
    fail "duplicate_backend=${backend}"
    continue
  fi

  log_path=$(resolve_path "${log_path}")
  manifest_path=$(resolve_path "${manifest_path}")

  if [[ ! -f "${log_path}" ]]; then
    fail "missing_log backend=${backend} log=${log_path}"
    continue
  fi
  if [[ ! -f "${manifest_path}" ]]; then
    fail "missing_manifest backend=${backend} manifest=${manifest_path}"
    continue
  fi

  summary_file="${tmp_dir}/${backend}.summary"
  if ! "${APP}/scripts/summarize_emu_results.sh" --log "${log_path}" > "${summary_file}"; then
    fail "summary_failed backend=${backend} log=${log_path}"
    continue
  fi

  seen_backend["${backend}"]=1
  log_by_backend["${backend}"]=${log_path}
  manifest_by_backend["${backend}"]=${manifest_path}
  summary_by_backend["${backend}"]=${summary_file}
done

if [[ "${allow_partial}" != "1" ]]; then
  for backend in ${expected_backends}; do
    if [[ -z "${seen_backend[${backend}]:-}" ]]; then
      fail "missing_required_backend=${backend}"
    fi
  done
fi

checked_backends=()
for backend in ${expected_backends}; do
  if [[ -n "${seen_backend[${backend}]:-}" ]]; then
    checked_backends+=("${backend}")
  fi
done
for backend in "${!seen_backend[@]}"; do
  found=0
  for ordered in "${checked_backends[@]}"; do
    if [[ "${ordered}" == "${backend}" ]]; then
      found=1
      break
    fi
  done
  if [[ "${found}" == "0" ]]; then
    checked_backends+=("${backend}")
  fi
done

for backend in "${checked_backends[@]}"; do
  summary_file=${summary_by_backend[${backend}]}
  manifest_path=${manifest_by_backend[${backend}]}

  runtime_status=$(summary_value_or "${summary_file}" runtime_status missing)
  roi_present=$(summary_value_or "${summary_file}" roi_present 0)
  done_present=$(summary_value_or "${summary_file}" done_present 0)
  done_status=$(summary_value_or "${summary_file}" done_status missing)
  trap_state=$(summary_value_or "${summary_file}" trap missing)
  roi_backend=$(summary_value_or "${summary_file}" roi_backend missing)
  roi_kernel=$(summary_value_or "${summary_file}" roi_kernel missing)
  manifest_backend=$(manifest_value_or "${manifest_path}" backend missing)
  manifest_kernel=$(manifest_value_or "${manifest_path}" kernel missing)

  [[ "${runtime_status}" == "0" ]] || fail "backend=${backend} runtime_status=${runtime_status}"
  [[ "${roi_present}" == "1" ]] || fail "backend=${backend} roi_present=${roi_present}"
  [[ "${done_present}" == "1" ]] || fail "backend=${backend} done_present=${done_present}"
  [[ "${done_status}" == "0" ]] || fail "backend=${backend} done_status=${done_status}"
  [[ "${trap_state}" == "good" ]] || fail "backend=${backend} trap=${trap_state}"
  [[ "${roi_backend}" == "${backend}" ]] || fail "backend=${backend} roi_backend=${roi_backend}"
  [[ "${manifest_backend}" == "${backend}" ]] || fail "backend=${backend} manifest_backend=${manifest_backend}"
  [[ "${roi_kernel}" == "${manifest_kernel}" ]] || fail "backend=${backend} roi_kernel=${roi_kernel} manifest_kernel=${manifest_kernel}"

  if ! (
    cd "${APP}"
    grep -E '^[0-9a-f]{64}[[:space:]]+' "${manifest_path}" | sha256sum -c - >/dev/null
  ); then
    fail "backend=${backend} manifest_sha256=invalid manifest=${manifest_path}"
  fi
done

required_equal_keys=(
  kernel
  warmup
  repeat
  MIMO_VALIDATE_TOL
  MIMO_VALIDATE_ERROR_STATS
  MIMO_EMU_REQUIRE_NO_HARD_DIV
  MIMO_USE_EQ4_DIRECT_H
  MIMO_EQ4_DIRECT_H_MODE
  MIMO_EQ4_DIRECT_H_ITERS
  MIMO_EQ4_DIRECT_H_NODIV
)

division_strategy_keys=(
  MIMO_USE_RECIP_TABLE
  MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE
  MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE_HARDDIV
  MIMO_USE_SOLVE4_HERMITIAN_Q15_ONECORR
  MIMO_SOLVE4_HERMITIAN_Q15_JUMP_ONLY_MASK
  MIMO_USE_SOLVE4_HERMITIAN_Q30_FIVECORR
  MIMO_USE_SOLVE4_HERMITIAN_Q30_TWOJUMP_CORR
  MIMO_USE_SOLVE4_HERMITIAN_Q30_JUMP_ONLY
  MIMO_USE_SOLVE4_HERMITIAN_Q30_ONECORR
  MIMO_USE_SOLVE4_HERMITIAN_Q30_PAIR_CORR
  MIMO_SOLVE4_HERMITIAN_Q30_JUMP_ONLY_MASK
  MIMO_USE_SOLVE4_HERMITIAN_POS_PIVOT_SCALE
  MIMO_USE_SOLVE4_HERMITIAN_TRUST_PIVOT_RANGE
  MIMO_USE_SOLVE4_HERMITIAN_TRUST_PIVOT_NONZERO
  MIMO_USE_SOLVE4_HERMITIAN_FULL_RECIP_TABLE
  MIMO_USE_SOLVE4_HERMITIAN_FLOOR_FULL_RECIP_TABLE
  MIMO_USE_SOLVE4_HERMITIAN_RECIP_STEP16
  MIMO_USE_EXT_ZF4_RVV_NODIV_FAST_SOLVE
  MIMO_EXT_ZF4_FAST_CORR_ITERS
  MIMO_EXT_ZF4_FAST_SCALE_BIAS_Q30
  MIMO_EXT_ZF4_FAST_REUSE_SCALE
  MIMO_USE_SOLVE4_SCHUR_DET_SCALE
  MIMO_USE_SOLVE4_SCHUR_BINV_Q15
  MIMO_USE_SOLVE4_SCHUR_Q15_RECIP
  MIMO_USE_SOLVE4_SCHUR_FUSED_INNER
  MIMO_USE_SOLVE4_SCHUR_FUSED_INNER_MMSE
  MIMO_USE_SOLVE4_SCHUR_MMSE_SPECIALIZE
  MIMO_USE_MMSE4_SCHUR_NO_MAG_SCALE
  MIMO_USE_MMSE4_SCHUR_SIGNED_TRUNC_SCALE
  MIMO_USE_SOLVE4_SCHUR_ZF_EXPLICIT
  MIMO_USE_SOLVE4_SCHUR_ZF_TOP_RESOLVE
  MIMO_USE_SOLVE4_SCHUR_ZF_HANDASM
  MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT
)

eq2_division_strategy_keys=(
  MIMO_EQ2_RECIP_MODE
  MIMO_EQ2_RECIP_NUM_SHIFT
)

eq2_backend_strategy_keys=(
  MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP
  MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID
  MIMO_USE_EQ2_MMSE_DIRECT_H
  MIMO_USE_SCALAR_EQ2_FUSED_LAMBDA
  MIMO_USE_SCALAR_EQ2_MMSE_FULL_TRUNC_SCALE
  MIMO_USE_SOLVE_STORE_TRUST_I16
  MIMO_USE_RVV_EQ2_M1
  MIMO_USE_RVV_EQ2_ALWAYS_ADD_LAMBDA
  MIMO_USE_RVV_EQ2_SPLIT_BUILD
  MIMO_USE_RVV_EQ2_FUSED_NCLIP_BUILD
  MIMO_USE_RVV_EQ2_TX_SOA_STORE
  MIMO_USE_RVV_EQ2_3M_BUILD
  MIMO_USE_RVV_EQ2_ZEXT_PACK
  MIMO_USE_RVV_EQ2_RI_SOA_OUTPUT
  MIMO_USE_RVV_EQ2_STAGED_NARROW_BUILD
  MIMO_USE_RVV_EQ2_IMPLICIT_VXRM
  MIMO_USE_RVV_EQ2_UQ12_SCALE
  MIMO_USE_RVV_EQ2_EARLY_RECIP_OVERLAP
  MIMO_USE_RVV_EQ2_MMSE_FULL_TRUNC_SCALE
  MIMO_USE_RVV_EQ2_BLOCK_SOA
  MIMO_USE_RVV_EQ2_GROUP_LOAD
  MIMO_USE_RVV_EQ2_GROUP_LOAD_M4
  MIMO_USE_EXT_EQ2_INVCOEFF
  MIMO_USE_EXT_EQ2_M2_BATCH
  MIMO_USE_EXT_EQ2_RHS_INTERLEAVE
  MIMO_USE_EXT_EQ2_MMSE_ADAPTIVE_Q13
)

backend_strategy_keys=(
  MIMO_USE_SOLVE4_UNROLL_V2A
  MIMO_USE_SOLVE4_HERMITIAN
  MIMO_USE_SOLVE4_HERMITIAN_LANE_OUTER
  MIMO_USE_SOLVE4_HERMITIAN_HAND_SCHED
  MIMO_USE_SOLVE4_HERMITIAN_FACTOR_CALL
  MIMO_USE_SOLVE4_HERMITIAN_Q15_CALL
  MIMO_USE_SOLVE4_HERMITIAN_GS
  MIMO_SOLVE4_HERMITIAN_GS_ITERS
  MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
  MIMO_USE_SOLVE_BRANCHLIGHT_ROUND
  MIMO_USE_SOLVE_Q15_I64_ARSHIFT_ROUND
  MIMO_USE_SOLVE_STORE_TRUST_I16
  MIMO_USE_SCALAR4_UPPER_ONLY_BUILD
  MIMO_USE_SCALAR4_SCHUR_TOP_Q30_ONCE
  MIMO_USE_SCALAR_MMSE4_FUSED_BUILD_SOLVE
  MIMO_USE_SCALAR_MMSE4_FUSED_COMPACT_C32
  MIMO_USE_SOLVE4_LDLH
  MIMO_USE_SOLVE4_SCHUR
  MIMO_USE_SOLVE4_SCHUR_PAIR2
  MIMO_USE_SOLVE_SKIP_LOWER_UPDATE
  MIMO_USE_RVV_SOLVE4
  MIMO_USE_RVV_SOLVE4_STREAMED
  MIMO_USE_RVV_SOLVE4_VNCLIP_ROUND
  MIMO_USE_RVV_SOLVE4_Q15_I32_MUL
  MIMO_USE_RVV_MMSE4_SCHUR_RVV_SOLVE
  MIMO_USE_MMSE4_SCHUR_INNER_Q30_ONCE
  MIMO_USE_MMSE4_SCHUR_NUM_ARSHIFT_SCALE
  MIMO_USE_RVV_MMSE4_SCHUR_INNER_SYM_ROUND
  MIMO_USE_RVV_MMSE4_SCHUR_TOP_Q30_ONCE
	  MIMO_USE_RVV_MMSE4_STAGED_BUILD_SOLVE
	  MIMO_USE_RVV_ZF4_FUSED_BUILD_SOLVE
	  MIMO_USE_RVV_ZF4_SEED_ACC
	  MIMO_USE_RVV_MMSE4_EARLY_TAIL_RHS
	  MIMO_USE_RVV_MMSE4_SEED_ACC
	  MIMO_USE_RVV_MMSE4_RI_PACKED_LOAD
	  MIMO_USE_RVV_MMSE4_BINV_Q13_REUSE
	  MIMO_USE_RVV_MMSE4_SCHUR_TRUE16B_SOLVE
	  MIMO_USE_RVV_MMSE4_SCHUR_TRUE16B_SHARED_A16
	  MIMO_USE_RVV_SCHUR4_SOLVE2_PAIR_M2
	  MIMO_USE_RVV_SCHUR4_VSMUL_SCALE
  MIMO_USE_RVV_SCHUR4_NUM_MAC
  MIMO_USE_RVV_SCHUR4_COMPLEX_MAC
  MIMO_USE_RVV_SCHUR4_VSSRA_ROUND
	  MIMO_USE_RVV_SITE4_VSSRA_ROUND
	  MIMO_USE_RVV_SITE4_SEG2_LOAD
	  MIMO_USE_RVV_SITE4_RI_SOA
	  MIMO_USE_RVV_SCHUR4_DET_MAC
	  MIMO_USE_RVV_MMSE4_CONST_VL
  MIMO_USE_RVV_ZF4_STAGED_BUILD_SOLVE
  MIMO_USE_RVV_ZF4_PACK32_STORE
  MIMO_USE_RVV_SITE4_MF2
  MIMO_USE_EXT_ZF4_LOCAL_SOLVE
  MIMO_USE_EXT_ZF4_RVV_NODIV_SOLVE
  MIMO_USE_EXT_ZF4_SCHUR_RVV_SOLVE
  MIMO_USE_EXT_MMSE4_SCHUR_RVV_SOLVE
  MIMO_USE_EXT_ZF4_RVV_ROUND_ONCE_BUILD
  MIMO_USE_EXT_ZF4_TRUE16B_WORK
  MIMO_USE_EXT_ZF4_PACK32_WORK
  MIMO_USE_EXT_ZF4_VSCMACC_BUILD
  MIMO_USE_EXT_ZF4_VSCMACC_FUSED_BUILD
  MIMO_USE_EXT_ZF4_INLINE_UPPER_I16_SOLVE
  MIMO_USE_EXT_ZF4_LIVE_TAIL_SCHUR
  MIMO_USE_EXT_ZF4_TAIL_SCRATCH_REUSE
  MIMO_USE_EXT_ZF4_INLINE_HEAD_WORK_REUSE
  MIMO_USE_EXT_ZF4_TOP2_MACC_SUM
  MIMO_USE_EXT_MMSE4_TRUE16B_LIVE_TAIL_SOLVE
  MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE
  MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_BSOLVE
  MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_STORE
  MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_LOAD
  MIMO_USE_EXT_MMSE4_SCHUR_FINAL_SIGNED_TRUNC_SCALE
  MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK
  MIMO_USE_EXT_MMSE4_RVV_I32_VEC_LAMBDA
  MIMO_USE_EXT_MMSE4_RVV_I32_LATE_TAIL_LOAD
  MIMO_USE_EXT_MMSE4_RVV_I32_SPECIALIZE
  MIMO_USE_EXT_MMSE4_RVV_I32_INNER_Q30_ONCE
  MIMO_USE_EXT_MMSE4_RVV_I32_TOP_Q30_ONCE
  MIMO_USE_EXT_MMSE4_RVV_I32_ASSUME_LAMBDA_NOSAT
  MIMO_USE_EXT_MMSE4_RVV_I32_BINV_REUSE
  MIMO_USE_EXT_MMSE4_RVV_I32_PACKED_PHASE
  MIMO_EXT_MMSE4_PACKED_TOP_EXT
  MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER
  MIMO_USE_SOLVE4_SCHUR_ZF_EXT_MACC_INNER
  MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_MASK
  MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_MODEL
  MIMO_CHECK_SOLVE2H_GROUP_LAYOUT
  MIMO_USE_EXT_ZF4_UPPER_RVV_SOLVE
  MIMO_USE_EXT_ZF4_MIXED_UPDATE_SOLVE
  MIMO_DEBUG_EXT_ZF4_RVV_NODIV_COMPARE
  MIMO_USE_SITE4_SOLVE_SOA
  MIMO_USE_SITE4_SOLVE_SOA_UNROLL
  MIMO_USE_SITE4_DIAG_REAL_BUILD
  MIMO_USE_SITE4_HERMITIAN_UPPER_BUILD
  MIMO_USE_SITE4_HERMITIAN_UPPER_MMSE
  MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED
  MIMO_USE_SITE4_HERMITIAN_UPPER_ROWFUSE
  MIMO_USE_SITE4_HERMITIAN_UPPER_ROWPAIR
  MIMO_USE_SITE4_ROUND_ONCE_DOT
  MIMO_USE_SITE4_PRELUDE_HOIST
  MIMO_USE_SITE4_BUILD_SOLVE_PIPELINE2
)

strategy_keys=("${division_strategy_keys[@]}" "${backend_strategy_keys[@]}")

if [[ "${MIMO_EMU_FAIRNESS_ALLOW_DIVISION_STRATEGY_MIX:-0}" == "1" ]]; then
  fairness_keys=("${required_equal_keys[@]}")
else
  fairness_keys=("${required_equal_keys[@]}" "${division_strategy_keys[@]}")
fi

if [[ "${MIMO_EMU_FAIRNESS_STRICT_STRATEGY:-0}" == "1" ]]; then
  fairness_keys+=("${backend_strategy_keys[@]}")
fi

reference_backend=
for backend in "${checked_backends[@]}"; do
  reference_backend=${backend}
  break
done

reference_kernel=
if [[ -n "${reference_backend}" ]]; then
  reference_kernel=$(manifest_value_or "${manifest_by_backend[${reference_backend}]}" kernel missing)
fi

if [[ "${reference_kernel}" == "eq_zf_2x2" || "${reference_kernel}" == "eq_mmse_2x2" ]]; then
  eq2_required_equal_keys=(
    kernel
    warmup
    repeat
    MIMO_VALIDATE_TOL
    MIMO_VALIDATE_ERROR_STATS
    MIMO_EMU_REQUIRE_NO_HARD_DIV
    MIMO_USE_EQ2_ZF_DIRECT_H
  )

  if [[ "${MIMO_EMU_FAIRNESS_ALLOW_DIVISION_STRATEGY_MIX:-0}" == "1" ]]; then
    fairness_keys=("${eq2_required_equal_keys[@]}")
  else
    fairness_keys=("${eq2_required_equal_keys[@]}" "${eq2_division_strategy_keys[@]}")
  fi
  if [[ "${MIMO_EMU_FAIRNESS_STRICT_STRATEGY:-0}" == "1" ]]; then
    fairness_keys+=("${eq2_backend_strategy_keys[@]}")
  fi
  strategy_keys=("${eq2_division_strategy_keys[@]}" "${eq2_backend_strategy_keys[@]}")
fi

if [[ "${reference_kernel}" == "eq_zf_4x4" ]]; then
  zf4_active_fairness_keys=()
  for key in "${fairness_keys[@]}"; do
    case "${key}" in
      MIMO_USE_MMSE4_SCHUR_NO_MAG_SCALE|\
      MIMO_USE_MMSE4_SCHUR_SIGNED_TRUNC_SCALE|\
      MIMO_USE_EXT_MMSE4_SCHUR_FINAL_SIGNED_TRUNC_SCALE|\
      MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK)
        ;;
      *)
        zf4_active_fairness_keys+=("${key}")
        ;;
    esac
  done
  fairness_keys=("${zf4_active_fairness_keys[@]}")
fi

declare -A reference_value_by_key

if [[ -n "${reference_backend}" ]]; then
  reference_manifest=${manifest_by_backend[${reference_backend}]}
  for key in "${fairness_keys[@]}"; do
    reference_value_by_key["${key}"]=$(manifest_strategy_value_or "${reference_manifest}" "${key}" missing)
    if [[ "${reference_value_by_key[${key}]}" == "missing" ]]; then
      fail "key=${key} reference_backend=${reference_backend} reference=missing"
    fi
  done

  for backend in "${checked_backends[@]}"; do
    manifest_path=${manifest_by_backend[${backend}]}
    for key in "${fairness_keys[@]}"; do
      value=$(manifest_strategy_value_or "${manifest_path}" "${key}" missing)
      if [[ "${value}" == "missing" ]]; then
        fail "key=${key} backend=${backend} actual=missing"
        continue
      fi
      if [[ "${value}" != "${reference_value_by_key[${key}]}" ]]; then
        fail "key=${key} reference_backend=${reference_backend} reference=${reference_value_by_key[${key}]} backend=${backend} actual=${value}"
      fi
    done
  done
fi

if [[ "${failed}" -ne 0 ]]; then
  echo "fairness_status=invalid"
  exit 4
fi

require_no_hard_div=${reference_value_by_key[MIMO_EMU_REQUIRE_NO_HARD_DIV]:-missing}
if [[ "${require_no_hard_div}" == "1" ]]; then
  fairness_profile=no-harddiv
elif [[ "${require_no_hard_div}" == "0" ]]; then
  fairness_profile=direct-div-allowed
else
  fairness_profile=unknown
fi

echo "fairness_status=valid"
echo "fairness_profile=${fairness_profile}"
echo "reference_backend=${reference_backend}"
echo "reference_kernel=${reference_kernel}"
printf 'checked_backends='
printf '%s ' "${checked_backends[@]}"
printf '\n'

for key in "${fairness_keys[@]}"; do
  echo "fairness_key ${key}=${reference_value_by_key[${key}]}"
done

for key in "${strategy_keys[@]}"; do
  values=()
  different=0
  first=
  for backend in "${checked_backends[@]}"; do
    manifest_path=${manifest_by_backend[${backend}]}
    value=$(manifest_strategy_value_or "${manifest_path}" "${key}" missing)
    values+=("${backend}:${value}")
    if [[ -z "${first}" ]]; then
      first=${value}
    elif [[ "${value}" != "${first}" ]]; then
      different=1
    fi
  done

  if [[ "${different}" == "1" ]]; then
    printf 'strategy_note %s=different' "${key}"
  else
    printf 'strategy_key %s=%s' "${key}" "${first}"
  fi
  printf ' values='
  printf '%s ' "${values[@]}"
  printf '\n'
done

for backend in "${checked_backends[@]}"; do
  summary_file=${summary_by_backend[${backend}]}
  echo "entry backend=${backend} log=${log_by_backend[${backend}]} manifest=${manifest_by_backend[${backend}]} cycle=$(summary_value_or "${summary_file}" roi_cycle missing) instret=$(summary_value_or "${summary_file}" roi_instret missing) wall_sec=$(summary_value_or "${summary_file}" runtime_elapsed_sec missing)"
done
