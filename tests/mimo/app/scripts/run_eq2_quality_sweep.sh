#!/usr/bin/env bash
set -euo pipefail

APP="${MIMO_APP_DIR:-$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)}"
QUALITY_TOL=${MIMO_EQ2_QUALITY_TOL:-11}
TIMEOUT_SEC=${TIMEOUT_SEC:-900}
TAG=${1:-final-v1}

if ! [[ "${TAG}" =~ ^[A-Za-z0-9._-]+$ ]]; then
  echo "Invalid tag: ${TAG}" >&2
  exit 2
fi

if pgrep -f "${APP}/../../../XiangShan/build/emu" >/dev/null; then
  echo "Refusing EQ2 quality sweep while official emu is active." >&2
  exit 3
fi

cd "${APP}"
mkdir -p perf_eval/raw/nemu/eq2-quality
summary="${APP}/perf_eval/raw/nemu/eq2-quality/${TAG}-summary.log"
: > "${summary}"

cases=(
  "2 1024 0"
  "4 2048 1"
  "6 4096 2"
)

for kernel in eq_zf_2x2 eq_mmse_2x2; do
  for case_args in "${cases[@]}"; do
    read -r mod_bits noise_q15 cond_profile <<< "${case_args}"
    python3 scripts/analyze_eq2_quality_float.py \
      --kernel "${kernel}" \
      --mod-bits "${mod_bits}" \
      --noise-q15 "${noise_q15}" \
      --cond-profile "${cond_profile}" | tee -a "${summary}"

    for backend in scalar rvv ext; do
      case_tag="${TAG}-${kernel}-${backend}"
      profile_env=(
        MIMO_EQ2_QUALITY_TOL="${QUALITY_TOL}"
        MIMO_VALIDATE_FAIL_PRINT_LIMIT=8
        TIMEOUT_SEC="${TIMEOUT_SEC}"
      )

      case "${backend}/${kernel}" in
        scalar/eq_zf_2x2)
          profile_env+=(
            MIMO_USE_EQ2_ZF_DIRECT_H=1
            MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP=1
            MIMO_USE_SOLVE_STORE_TRUST_I16=0
          )
          ;;
        rvv/eq_zf_2x2)
          profile_env+=(
            MIMO_USE_EQ2_ZF_DIRECT_H=1
            MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP=1
            MIMO_USE_RVV_EQ2_UQ12_SCALE=0
            MIMO_USE_RVV_EQ2_EARLY_RECIP_OVERLAP=0
            MIMO_USE_SOLVE_STORE_TRUST_I16=0
          )
          ;;
        ext/eq_zf_2x2)
          profile_env+=(
            MIMO_USE_EQ2_ZF_DIRECT_H=1
            MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP=0
            MIMO_USE_EXT_EQ2_INVCOEFF=0
            MIMO_USE_EXT_EQ2_M2_BATCH=1
            MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID=1
            MIMO_USE_SOLVE_STORE_TRUST_I16=1
          )
          ;;
        scalar/eq_mmse_2x2)
          profile_env+=(
            MIMO_USE_SCALAR_EQ2_MMSE_FULL_TRUNC_SCALE=1
            MIMO_USE_SOLVE_STORE_TRUST_I16=0
          )
          ;;
        rvv/eq_mmse_2x2)
          profile_env+=(
            MIMO_USE_RVV_EQ2_UQ12_SCALE=0
            MIMO_USE_RVV_EQ2_EARLY_RECIP_OVERLAP=1
            MIMO_USE_RVV_EQ2_MMSE_FULL_TRUNC_SCALE=1
            MIMO_USE_RVV_EQ2_STAGED_NARROW_BUILD=0
            MIMO_USE_SOLVE_STORE_TRUST_I16=0
          )
          ;;
        ext/eq_mmse_2x2)
          profile_env+=(
            MIMO_USE_EXT_EQ2_INVCOEFF=1
            MIMO_USE_EXT_EQ2_M2_BATCH=1
            MIMO_USE_EXT_EQ2_MMSE_ADAPTIVE_Q13=1
            MIMO_USE_SOLVE_STORE_TRUST_I16=0
          )
          ;;
      esac

      env "${profile_env[@]}" \
        bash scripts/run_eq2_quality_case.sh \
        "${backend}" "${kernel}" "${mod_bits}" "${noise_q15}" \
        "${cond_profile}" "${case_tag}"

      case_log="${APP}/perf_eval/raw/nemu/eq2-quality/${case_tag}-${backend}-${kernel}-m${mod_bits}-n${noise_q15}-c${cond_profile}.log"
      grep "MIMO_VALIDATE_\(PASS\|ERROR_STATS\) kernel=${kernel}" "${case_log}" \
        | tail -n 2 >> "${summary}"
      grep "MIMO_EQ2_QUALITY backend=${backend} kernel=${kernel}" "${case_log}" \
        | tail -n 1 >> "${summary}"
    done
  done
done

python3 scripts/check_eq2_quality_summary.py \
  --max-component-error "${QUALITY_TOL}" "${summary}"
echo "MIMO_EQ2_QUALITY_SWEEP_DONE tol=${QUALITY_TOL} summary=${summary}"
