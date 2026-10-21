#!/usr/bin/env bash
set -euo pipefail

ROOT="${XS_ENV_ROOT:-$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../../../.." && pwd)}"
EMU_ENV="${ROOT:-${XS_ENV_ROOT:-$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../../../.." && pwd)}}"
APP=${ROOT}/nexus-am/apps/mimo

usage() {
  cat <<'EOF'
Usage:
  scripts/run_candidate_gates.sh <tag> [backend ...]

Examples:
  scripts/run_candidate_gates.sh ext4-specialized ext
  scripts/run_candidate_gates.sh rvv4-reduction rvv
  scripts/run_candidate_gates.sh shared-solve scalar rvv ext

Environment:
  TIMEOUT_SEC             NEMU timeout per backend, default 300.
  MIMO_GATE_KERNEL        Kernel for riscv64-xs assembly build, default eq_zf_4x4.
  MIMO_VALIDATE_KERNEL    Correctness kernel. Current scope permits 2x2/4x4 ZF/MMSE.
  MIMO_ZF4_ONLY_LOCK      Legacy compatibility guard; must remain 1.
  MIMO_WARMUP_REPEAT      Perf-build warmup repeat, default 0.
  MIMO_REPEAT             Perf-build repeat, default 1.
  MIMO_GATE_SKIP_NEMU     Set to 1 to skip NEMU correctness.
  MIMO_GATE_SKIP_ASM      Set to 1 to skip riscv64-xs assembly build/copy.
  MIMO_GATE_ALLOW_EMU_RUNNING
                          Set to 1 to allow running while official emu exists.
  MIMO_GATE_ASM_EXPECT    Optional asm expectation gate name:
                            ext-site4-v3
                            site4-rvv-lane-solve-v1
                            site4-rvv-solve-vset-schedule-v1
                            zf4-mode4-microkernel
  MIMO_GATE_ASM_EXPECT_BACKENDS
                          Space-separated backend list for asm expectation,
                          default is all selected backends. This also limits
                          optional asm div-budget checks.
  MIMO_GATE_ASM_SYMBOL    Optional symbol override for asm expectation.
  MIMO_ASM_MAX_DIV_SIGNED
  MIMO_ASM_MAX_DIV_UNSIGNED
  MIMO_ASM_MAX_VDIV_VV    Optional maximum counts passed to
                          check_asm_symbol_counts.sh. These can be used
                          without MIMO_GATE_ASM_EXPECT.
  MIMO_ASM_MAX_SCALAR_LOAD
  MIMO_ASM_MAX_STACK_LOAD
  MIMO_ASM_MAX_STACK_STORE
  MIMO_ASM_MAX_STACK_FRAME_BYTES
                          Optional scalar-load/stack-pressure maximums passed to
                          check_asm_symbol_counts.sh. These can be used
                          without MIMO_GATE_ASM_EXPECT.
	  MIMO_ASM_FORBID_HARD_DIV
	                          Set to 1 to reject any scalar/vector div/rem in
	                          checked hot symbols. For wrapper symbols this also
	                          follows the known ZF4 solve helper list, including
	                          schur4_solve_site4_zf_ext_inner3_q15.
	  MIMO_ASM_FORBID_SEGMENT_LDST
	                          Set to 0 only for historical diagnosis. Defaults to
	                          1 and rejects RVV segment load/store in hot symbols.
  MIMO_USE_RECIP_TABLE
  MIMO_VALIDATE_TOL
  MIMO_VALIDATE_ERROR_STATS
  MIMO_VALIDATE_FAIL_PRINT_LIMIT
  MIMO_USE_SOLVE4_UNROLL_V2A
  MIMO_USE_SOLVE4_HERMITIAN
  MIMO_USE_SOLVE4_HERMITIAN_LANE_OUTER
  MIMO_USE_SOLVE4_HERMITIAN_SOA_STAGE_OUTER
  MIMO_USE_SOLVE4_HERMITIAN_FACTOR_PRECOMPUTE
  MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE
  MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE_HARDDIV
  MIMO_USE_SOLVE4_LDLH
  MIMO_USE_SOLVE4_SCHUR
  MIMO_USE_SOLVE4_SCHUR_PAIR2
  MIMO_USE_SOLVE4_SCHUR_DET_SCALE
  MIMO_USE_SOLVE4_SCHUR_BINV_Q15
  MIMO_USE_SOLVE4_SCHUR_Q15_RECIP
  MIMO_USE_SOLVE4_SCHUR_FUSED_INNER
  MIMO_USE_SOLVE4_SCHUR_FUSED_INNER_MMSE
  MIMO_USE_SOLVE4_SCHUR_MMSE_SPECIALIZE
  MIMO_USE_SOLVE4_SCHUR_ZF_EXPLICIT
  MIMO_USE_SOLVE4_SCHUR_ZF_RELOAD_MICRO
  MIMO_USE_SOLVE4_SCHUR_ZF_TOP_RESOLVE
  MIMO_USE_SOLVE4_SCHUR_ZF_HANDASM
  MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER
  MIMO_USE_SOLVE4_SCHUR_ZF_I32_NUM
  MIMO_USE_SOLVE4_SCHUR_ZF_Q15_NUM_SOLVE
  MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_ONLY
  MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_MASK
  MIMO_USE_SOLVE4_SCHUR_ZF_INLINE_HELPER
  MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_MODEL
  MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_MODEL
  MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_ASM
  MIMO_USE_SOLVE4_SCHUR_ZF_PIVOTSCALE_MODEL
  MIMO_CHECK_SOLVE2H_GROUP_LAYOUT
  MIMO_CHECK_VSOLVE2H2_LAYOUT
  MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT
  MIMO_USE_EQ4_DIRECT_H
  MIMO_EQ4_DIRECT_H_MODE
  MIMO_EQ4_DIRECT_H_ITERS
  MIMO_EQ4_DIRECT_H_NODIV
  MIMO_USE_SOLVE_SKIP_LOWER_UPDATE
  MIMO_USE_RVV_SOLVE4
  MIMO_USE_RVV_SOLVE4_STREAMED
  MIMO_USE_RVV_SOLVE4_VNCLIP_ROUND
  MIMO_USE_RVV_SOLVE4_Q15_I32_MUL
  MIMO_USE_RVV_ZF4_SCHUR_RVV_SOLVE
  MIMO_USE_RVV_MMSE4_SCHUR_RVV_SOLVE
  MIMO_USE_RVV_ZF4_FUSED_BUILD_SOLVE
  MIMO_USE_RVV_ZF4_SEED_ACC
  MIMO_USE_RVV_ZF4_Q15ACC_BUILD
  MIMO_USE_RVV_ZF4_PACK32_STORE
  MIMO_USE_RVV_SITE4_MF2
  MIMO_USE_RVV_EQ2_M1
  MIMO_USE_EQ2_ZF_DIRECT_H
  MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP
  MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID
  MIMO_USE_EQ2_DIRECT_Q13_INV
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
  MIMO_USE_RVV_EQ2_BLOCK_SOA
  MIMO_USE_RVV_EQ2_GROUP_LOAD
  MIMO_USE_RVV_EQ2_GROUP_LOAD_M4
  MIMO_USE_EXT_EQ2_INVCOEFF
  MIMO_USE_EXT_EQ2_M2_BATCH
  MIMO_USE_EXT_EQ2_RHS_INTERLEAVE
  MIMO_USE_MMSE4_SCHUR_NO_MAG_SCALE
  MIMO_USE_MMSE4_SCHUR_SIGNED_TRUNC_SCALE
  MIMO_USE_MMSE4_SCHUR_NUM_ARSHIFT_SCALE
  MIMO_USE_MMSE4_SCHUR_INNER_Q30_ONCE
  MIMO_USE_RVV_MMSE4_SCHUR_INNER_SYM_ROUND
  MIMO_USE_RVV_MMSE4_SCHUR_TOP_Q30_ONCE
  MIMO_USE_RVV_MMSE4_STAGED_BUILD_SOLVE
  MIMO_USE_RVV_MMSE4_EARLY_TAIL_RHS
  MIMO_USE_RVV_MMSE4_SEED_ACC
  MIMO_USE_RVV_SCHUR4_SOLVE2_PAIR_M2
  MIMO_USE_RVV_SCHUR4_VSMUL_SCALE
  MIMO_USE_RVV_SCHUR4_NUM_MAC
  MIMO_USE_RVV_SCHUR4_COMPLEX_MAC
  MIMO_USE_RVV_SCHUR4_VSSRA_ROUND
  MIMO_USE_RVV_SITE4_VSSRA_ROUND
  MIMO_USE_RVV_SITE4_SEG2_LOAD
  MIMO_USE_RVV_SITE4_RI_SOA
  MIMO_USE_RVV_SCHUR4_DET_MAC
  MIMO_USE_EXT_MMSE4_SCHUR_FINAL_SIGNED_TRUNC_SCALE
  MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK
  MIMO_USE_EXT_MMSE4_SCHUR_RVV_SOLVE
  MIMO_USE_EXT_MMSE4_FUSED_BUILD_SOLVE
  MIMO_USE_EXT_MMSE4_NARROW_WORK
  MIMO_USE_EXT_MMSE4_LIVE_BUILD
  MIMO_USE_EXT_MMSE4_STREAM_WORK_SOLVE
  MIMO_USE_EXT_MMSE4_TRUE16B_INLINE_SOLVE
  MIMO_USE_EXT_MMSE4_TRUE16B_LIVE_TAIL_SOLVE
  MIMO_USE_EXT_MMSE4_RVV_I32_VEC_LAMBDA
  MIMO_USE_EXT_MMSE4_RVV_I32_LATE_TAIL_LOAD
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
  MIMO_COLLECT_SOLVE_DEN_STATS
  MIMO_COLLECT_SOLVE_VALUE_STATS
  MIMO_COLLECT_SCHUR_STATS
  MIMO_COLLECT_PIVOT_SCALE_STATS
                          Set to 1 to print pivot-scale correction stats.
	  MIMO_USE_SOLVE4_HERMITIAN_HAND_SCHED
	                          Set to 1 for hand-scheduled Hermitian ZF4 solve.
	  MIMO_USE_SOLVE4_HERMITIAN_FACTOR_CALL
	                          Set to 1 to keep q30 factor division behind a noinline call.
	  MIMO_USE_SOLVE4_HERMITIAN_FACTOR_Q15
	                          Set to 1 for a local Q15-factor Hermitian solve diagnostic.
	  MIMO_USE_SOLVE4_HERMITIAN_Q15_CALL
	                          Set to 1 to keep q15 back-sub division behind a noinline call.
	  MIMO_USE_SOLVE4_HERMITIAN_Q15_ONECORR
	                          Set to 1 for one-step Q15 pivot correction.
	  MIMO_SOLVE4_HERMITIAN_Q15_JUMP_ONLY_MASK
	                          Bit mask for local per-stage Q15 jump-only diagnostics.
	  MIMO_USE_SOLVE4_HERMITIAN_GS
	                          Set to 1 for a fixed-iteration Gauss-Seidel ZF4 solve.
	  MIMO_SOLVE4_HERMITIAN_GS_ITERS
	                          Iteration count for the Gauss-Seidel ZF4 solve, default 3.
	  MIMO_USE_SOLVE4_HERMITIAN_Q30_FIVECORR
	                          Set to 1 for fixed five-step Q30 pivot correction.
	  MIMO_USE_SOLVE4_HERMITIAN_Q30_TWOJUMP_CORR
	                          Set to 1-3 for second-jump plus fixed-step Q30 pivot correction.
	  MIMO_USE_SOLVE4_HERMITIAN_Q30_JUMP_ONLY
	                          Set to 1 to skip final Q30 step correction after residual jump.
	  MIMO_USE_SOLVE4_HERMITIAN_Q30_ONECORR
	                          Set to 1 for one-step Q30 pivot correction.
	  MIMO_USE_SOLVE4_HERMITIAN_Q30_PAIR_CORR
	                          Set to 1 to correct real/imag Q30 factor components in one helper.
	  MIMO_SOLVE4_HERMITIAN_Q30_JUMP_ONLY_MASK
	                          Bit mask for local per-stage Q30 jump-only diagnostics.
	  MIMO_USE_SOLVE4_HERMITIAN_POS_PIVOT_SCALE
	                          Set to 1 to assume Hermitian pivot denominators are positive.
	  MIMO_USE_SOLVE4_HERMITIAN_TRUST_PIVOT_RANGE
	                          Set to 1 to trust denominator stats table range.
	  MIMO_USE_SOLVE4_HERMITIAN_TRUST_PIVOT_NONZERO
	                          Set to 1 to skip Hermitian pivot zero guard.
	  MIMO_USE_SOLVE4_HERMITIAN_FULL_RECIP_TABLE
	                          Set to 1 to replace pivot reciprocal interpolation with an initialized full table.
	  MIMO_USE_SOLVE4_HERMITIAN_FLOOR_FULL_RECIP_TABLE
	                          Set to 1 to floor-correct the initialized full reciprocal table.
	  MIMO_USE_SOLVE4_HERMITIAN_RECIP_STEP16
	                          Set to 1 to use a step-16 Hermitian pivot reciprocal table.
	  MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE
	                          Set to 1 to update Hermitian diagonal real parts only.
	  MIMO_USE_SOLVE4_HERMITIAN_DIAG_APPROX
	                          Set to 1 for diagonal-preconditioned ZF4 approximation.
	  MIMO_USE_SOLVE_BRANCHLIGHT_ROUND
	                          Set to 1 to remove unreachable saturation branches in solve rounding.
	  MIMO_USE_SOLVE_Q15_I64_ARSHIFT_ROUND
	                          Set to 1 to use arithmetic-shift Q15 rounding for i64 solve products.
	  MIMO_USE_SOLVE_STORE_TRUST_I16
	                          Set to 1 to trust solve output range and skip final int16 saturation.
	  MIMO_USE_SCALAR4_UPPER_ONLY_BUILD
	  MIMO_USE_SCALAR4_COMPLEX_MUL_3M
	  MIMO_USE_SCALAR4_SCHUR_TOP_Q30_ONCE
	  MIMO_USE_SCALAR_MMSE4_FUSED_BUILD_SOLVE
	  MIMO_USE_SCALAR_MMSE4_FUSED_COMPACT_C32
	                          Set to 1 for scalar 4x4 Schur upper-only build diagnostic.
	  MIMO_USE_SCALAR_ZF4_Q15ACC_BUILD
	                          Set to 1 for scalar ZF4 Q15 product + int16 saturated build accumulation.
	  MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED
	                          Set to 1 to pack the ZF4 Hermitian upper triangle into 10 SoA slots.
	  MIMO_USE_SITE4_HERMITIAN_UPPER_ROWFUSE
	                          Set to 1 to fuse each upper-build row into one EXT asm block.
	  MIMO_USE_SITE4_HERMITIAN_UPPER_ROWPAIR
	                          Set to 1 to fuse upper-build rows in row pairs.
	  MIMO_USE_SITE4_BUILD_SOLVE_PIPELINE2
	                          Set to 1 for the local ZF4 rowfuse build/solve two-buffer pipeline diagnostic.
	  MIMO_USE_SITE4_HERMITIAN_UPPER_MMSE
	                          Set to 1 to let MMSE4 reuse the SoA Schur Hermitian upper build.
	  MIMO_USE_EXT_ZF4_LOCAL_SOLVE
	                          Set to 1 to inline the EXT ZF4 SoA Hermitian solve in the site4 runner.
		  MIMO_USE_EXT_ZF4_RVV_NODIV_SOLVE
		                          Set to 1 for the EXT ZF4 RVV no-harddiv solve candidate.
		  MIMO_USE_EXT_ZF4_RVV_NODIV_FAST_SOLVE
		                          Set to 1 for the EXT ZF4 RVV fast no-harddiv solve candidate.
		  MIMO_USE_EXT_ZF4_SCHUR_RVV_SOLVE
		                          Set to 1 for the EXT ZF4 RVV Schur q15recip solve candidate.
		  MIMO_USE_EXT_ZF4_RVV_ROUND_ONCE_BUILD
		                          Set to 1 for the EXT ZF4 local RVV round-once build candidate.
			  MIMO_USE_EXT_ZF4_TRUE16B_WORK
			                          Set to 1 for the EXT ZF4 packed Q15 work build/solve candidate.
			  MIMO_USE_SOLVE4_SCHUR_ZF_Q15_NUM_SOLVE
			                          Set to 1 to compute the ZF4 Schur 2x2 solve numerators with Q15 EXT complex multiply.
			  MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_ONLY
			                          Set to 1 to skip numerator rounding before q15recip scale in the ZF4 diagnostic path.
			  MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_MASK
			                          Bit 0 uses arshift in all B-stage solve2 calls, bit 1 uses arshift in S-stage solve2.
			                          Bits 2/3/4 split B-stage as w0/w1/y RHS diagnostics.
			  MIMO_USE_EXT_ZF4_PACK32_WORK
			                          Set to 1 to use continuous packed Q15 stores for the true16b work candidate.
		  MIMO_USE_EXT_ZF4_VSCMACC_BUILD
		                          Set to 1 to use Q15 vscmacc/vscmacccj accumulation for the EXT ZF4 upper build.
		  MIMO_USE_EXT_ZF4_VSCMACC_FUSED_BUILD
		                          Set to 1 to fuse all four VSCMACC upper-build rows into one pass.
		  MIMO_USE_EXT_ZF4_INLINE_UPPER_I16_SOLVE
		                          Set to 1 to inline the current upper-i16 Schur solve in the EXT hot loop.
		  MIMO_USE_EXT_ZF4_LIVE_TAIL_SCHUR
		                          Set to 1 to reuse fused-build live tail vectors in the inline Schur update.
		  MIMO_USE_EXT_ZF4_TAIL_SCRATCH_REUSE
		                          Set to 1 to reuse true16b work tail slots as inline Schur scratch.
		  MIMO_USE_EXT_ZF4_INLINE_HEAD_WORK_REUSE
		                          Set to 1 to reuse true16b work head slots for inline B-stage W/Y temporaries.
		  MIMO_USE_EXT_ZF4_TOP2_MACC_SUM
		                          Set to 1 to accumulate the two top back-substitution products with vscmacc before subtracting once.
		  MIMO_USE_SOLVE4_SCHUR_ZF_EXT_MACC_INNER
		                          Set to 1 to use vscmacccj accumulation in the ZF4 Schur inner update.
		  MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE
		                          Set to 1 to use the RVV i32 Schur solve on packed Q15 true16b work.
		  MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_BSOLVE
		                          Set to 1 to use RVV i32 only for the first 2x2 3-RHS solve on true16b work.
		  MIMO_USE_EXT_ZF4_SCALAR_X23_STORE
		                          Set to 1 to store x2/x3 from the scalar S-stage loop and let EXT top2 store only x0/x1.
		  MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SPLIT_SOLVE
		                          Set to 1 to stage the RVV i32 true16b solve to reduce vector register pressure.
		  MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_STORE
		                          Set to 1 to pack RVV i32 solve outputs into one complex 32-bit strided store.
		  MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_LOAD
		                          Set to 1 to load true16b work as packed complex 32-bit values in the RVV i32 solve.
		  MIMO_USE_EXT_MMSE4_RVV_I32_LATE_TAIL_LOAD
		                          Set to 1 to delay tail loads in the packed RVV i32 solve helper.
		  MIMO_EXT_ZF4_FAST_CORR_ITERS
		                          Correction iterations for the fast no-harddiv solve candidate.
		  MIMO_EXT_ZF4_FAST_SCALE_BIAS_Q30
		                          Final Q30 reciprocal scale bias for the fast no-harddiv solve candidate.
		  MIMO_EXT_ZF4_FAST_REUSE_SCALE
		                          Set to 1 to reuse one reciprocal scale per pivot in the fast no-harddiv solve candidate.
		  MIMO_USE_EXT_ZF4_UPPER_RVV_SOLVE
		                          Set to 1 for the EXT ZF4 upper-only RVV direct-div solve candidate.
		  MIMO_USE_EXT_ZF4_MIXED_UPDATE_SOLVE
		                          Set to 1 for the EXT ZF4 scalar-div/RVV-update mixed solve candidate.
		  MIMO_DEBUG_EXT_ZF4_RVV_NODIV_COMPARE
		                          Set to 1 to print first-block scalar/RVV solve comparison.
	  MIMO_USE_RVV_SOLVE4_STREAMED
	                          Set to 1 for streamed 4-lane RVV solve candidates.

Outputs:
  perf_eval/raw/nemu/nemu-mimo-<backend>-correctness-<tag>.log
  perf_eval/raw/nemu/nemu-mimo-<backend>-<tag>-am-log.txt
  perf_eval/asm/mimo-<backend>-riscv64-xs-<tag>.txt
EOF
}

if [[ "${1:-}" == "-h" || "${1:-}" == "--help" ]]; then
  usage
  exit 0
fi

if [[ $# -lt 1 ]]; then
  usage >&2
  exit 2
fi

tag=$1
shift

if [[ ! "${tag}" =~ ^[A-Za-z0-9._-]+$ ]]; then
  echo "Invalid tag '${tag}'; use only letters, digits, dot, underscore, and dash." >&2
  exit 2
fi

if [[ $# -gt 0 ]]; then
  backends=("$@")
else
  backends=(scalar rvv ext)
fi

for backend in "${backends[@]}"; do
  case "${backend}" in
    scalar|rvv|ext) ;;
    *)
      echo "Unsupported backend '${backend}'; use scalar, rvv, or ext." >&2
      exit 2
      ;;
  esac
done

if [[ "${MIMO_GATE_ALLOW_EMU_RUNNING:-0}" != "1" ]] &&
   pgrep -f "${EMU_ENV}/XiangShan/build/emu" >/dev/null; then
  echo "Refusing gate run: an official emu process is running. Re-run after emu finishes, or set MIMO_GATE_ALLOW_EMU_RUNNING=1." >&2
  exit 3
fi

timeout_sec=${TIMEOUT_SEC:-300}
kernel=${MIMO_GATE_KERNEL:-eq_zf_4x4}
if [[ "${MIMO_4X4_EQ_ONLY_LOCK:-1}" != "1" ]]; then
  echo "Refusing gate run: legacy MIMO_4X4_EQ_ONLY_LOCK must remain enabled while the 2x2/4x4 EQ scope is active." >&2
  exit 2
fi
if [[ "${MIMO_ZF4_ONLY_LOCK:-1}" != "1" ]]; then
  echo "Refusing gate run: legacy MIMO_ZF4_ONLY_LOCK must remain enabled while the 2x2/4x4 EQ scope is active." >&2
  exit 2
fi
if [[ "${MIMO_ALLOW_8X8+x}" == "x" || "${MIMO_USE_SITE4_8X8_BUILD+x}" == "x" ]]; then
  echo "Refusing gate run: 8x8 has been removed from the current execution scope; do not set 8x8 knobs." >&2
  exit 2
fi
if [[ "${MIMO_ALLOW_2X2+x}" == "x" || "${MIMO_USE_SITE2_2X2_BUILD+x}" == "x" ]]; then
  echo "Refusing gate run: 2x2 is not unlocked in the current execution scope; do not set 2x2 knobs before ZF4/MMSE4 signoff." >&2
  exit 2
fi
if [[ "${MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_ASM_STUB:-0}" == "1" ]]; then
  echo "Refusing gate run: vschur2h asm-stub is a linked-assembly-only preflight; use scripts/check_vschur2h_asm_stub.sh and do not run NEMU before ISA/NEMU/RTL support exists." >&2
  exit 2
fi
if [[ "${MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_ASM:-0}" == "1" ]]; then
  echo "Refusing gate run: vsolve2h2 asm path is disabled on the fixed hardware platform; do not run NEMU or official emu with this accident-line config." >&2
  exit 2
fi
eq4_only_lock=1
zf4_only_lock=1
validate_kernel=${MIMO_VALIDATE_KERNEL:-eq_zf_4x4}
warmup=${MIMO_WARMUP_REPEAT:-0}
repeat=${MIMO_REPEAT:-1}
validate_tol=${MIMO_VALIDATE_TOL:-2}
validate_error_stats=${MIMO_VALIDATE_ERROR_STATS:-0}
validate_fail_print_limit=${MIMO_VALIDATE_FAIL_PRINT_LIMIT:-4}
recip_table=${MIMO_USE_RECIP_TABLE:-0}
solve4_unroll=${MIMO_USE_SOLVE4_UNROLL_V2A:-0}
solve4_hermitian=${MIMO_USE_SOLVE4_HERMITIAN:-0}
solve4_hermitian_lane_outer=${MIMO_USE_SOLVE4_HERMITIAN_LANE_OUTER:-0}
solve4_hermitian_soa_stage_outer=${MIMO_USE_SOLVE4_HERMITIAN_SOA_STAGE_OUTER:-0}
solve4_hermitian_factor_precompute=${MIMO_USE_SOLVE4_HERMITIAN_FACTOR_PRECOMPUTE:-0}
solve4_hermitian_pivot_scale=${MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE:-0}
solve4_hermitian_pivot_scale_harddiv=${MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE_HARDDIV:-0}
solve4_hermitian_hand_sched=${MIMO_USE_SOLVE4_HERMITIAN_HAND_SCHED:-0}
solve4_hermitian_factor_call=${MIMO_USE_SOLVE4_HERMITIAN_FACTOR_CALL:-0}
solve4_hermitian_factor_q15=${MIMO_USE_SOLVE4_HERMITIAN_FACTOR_Q15:-0}
solve4_hermitian_q15_call=${MIMO_USE_SOLVE4_HERMITIAN_Q15_CALL:-0}
solve4_hermitian_q15_onecorr=${MIMO_USE_SOLVE4_HERMITIAN_Q15_ONECORR:-0}
solve4_hermitian_q15_jump_only_mask=${MIMO_SOLVE4_HERMITIAN_Q15_JUMP_ONLY_MASK:-0}
solve4_hermitian_gs=${MIMO_USE_SOLVE4_HERMITIAN_GS:-0}
solve4_hermitian_gs_iters=${MIMO_SOLVE4_HERMITIAN_GS_ITERS:-3}
solve4_hermitian_q30_fivecorr=${MIMO_USE_SOLVE4_HERMITIAN_Q30_FIVECORR:-0}
solve4_hermitian_q30_twojump_corr=${MIMO_USE_SOLVE4_HERMITIAN_Q30_TWOJUMP_CORR:-0}
solve4_hermitian_q30_jump_only=${MIMO_USE_SOLVE4_HERMITIAN_Q30_JUMP_ONLY:-0}
solve4_hermitian_q30_onecorr=${MIMO_USE_SOLVE4_HERMITIAN_Q30_ONECORR:-0}
solve4_hermitian_q30_pair_corr=${MIMO_USE_SOLVE4_HERMITIAN_Q30_PAIR_CORR:-0}
solve4_hermitian_q30_jump_only_mask=${MIMO_SOLVE4_HERMITIAN_Q30_JUMP_ONLY_MASK:-0}
solve4_hermitian_pos_pivot_scale=${MIMO_USE_SOLVE4_HERMITIAN_POS_PIVOT_SCALE:-0}
solve4_hermitian_trust_pivot_range=${MIMO_USE_SOLVE4_HERMITIAN_TRUST_PIVOT_RANGE:-0}
solve4_hermitian_trust_pivot_nonzero=${MIMO_USE_SOLVE4_HERMITIAN_TRUST_PIVOT_NONZERO:-0}
solve4_hermitian_full_recip_table=${MIMO_USE_SOLVE4_HERMITIAN_FULL_RECIP_TABLE:-0}
solve4_hermitian_floor_full_recip_table=${MIMO_USE_SOLVE4_HERMITIAN_FLOOR_FULL_RECIP_TABLE:-0}
solve4_hermitian_recip_step16=${MIMO_USE_SOLVE4_HERMITIAN_RECIP_STEP16:-0}
solve4_hermitian_diag_real_update=${MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE:-0}
solve4_hermitian_diag_approx=${MIMO_USE_SOLVE4_HERMITIAN_DIAG_APPROX:-0}
solve_branchlight_round=${MIMO_USE_SOLVE_BRANCHLIGHT_ROUND:-0}
solve_q15_i64_arshift_round=${MIMO_USE_SOLVE_Q15_I64_ARSHIFT_ROUND:-0}
solve_store_trust_i16=${MIMO_USE_SOLVE_STORE_TRUST_I16:-0}
scalar4_upper_only_build=${MIMO_USE_SCALAR4_UPPER_ONLY_BUILD:-0}
scalar4_complex_mul_3m=${MIMO_USE_SCALAR4_COMPLEX_MUL_3M:-0}
scalar4_schur_top_q30_once=${MIMO_USE_SCALAR4_SCHUR_TOP_Q30_ONCE:-0}
scalar_mmse4_fused_build_solve=${MIMO_USE_SCALAR_MMSE4_FUSED_BUILD_SOLVE:-0}
scalar_mmse4_fused_compact_c32=${MIMO_USE_SCALAR_MMSE4_FUSED_COMPACT_C32:-0}
scalar_zf4_q15acc_build=${MIMO_USE_SCALAR_ZF4_Q15ACC_BUILD:-0}
solve4_ldlh=${MIMO_USE_SOLVE4_LDLH:-0}
solve4_schur=${MIMO_USE_SOLVE4_SCHUR:-0}
solve4_schur_pair2=${MIMO_USE_SOLVE4_SCHUR_PAIR2:-0}
solve4_schur_det_scale=${MIMO_USE_SOLVE4_SCHUR_DET_SCALE:-0}
solve4_schur_binv_q15=${MIMO_USE_SOLVE4_SCHUR_BINV_Q15:-0}
solve4_schur_q15_recip=${MIMO_USE_SOLVE4_SCHUR_Q15_RECIP:-0}
solve4_schur_fused_inner=${MIMO_USE_SOLVE4_SCHUR_FUSED_INNER:-0}
solve4_schur_fused_inner_mmse=${MIMO_USE_SOLVE4_SCHUR_FUSED_INNER_MMSE:-0}
solve4_schur_mmse_specialize=${MIMO_USE_SOLVE4_SCHUR_MMSE_SPECIALIZE:-0}
solve4_schur_zf_explicit=${MIMO_USE_SOLVE4_SCHUR_ZF_EXPLICIT:-0}
solve4_schur_zf_reload_micro=${MIMO_USE_SOLVE4_SCHUR_ZF_RELOAD_MICRO:-0}
solve4_schur_zf_top_resolve=${MIMO_USE_SOLVE4_SCHUR_ZF_TOP_RESOLVE:-0}
solve4_schur_zf_handasm=${MIMO_USE_SOLVE4_SCHUR_ZF_HANDASM:-0}
solve4_schur_zf_ext_inner=${MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER:-0}
solve4_schur_zf_i32_num=${MIMO_USE_SOLVE4_SCHUR_ZF_I32_NUM:-0}
solve4_schur_zf_q15_num_solve=${MIMO_USE_SOLVE4_SCHUR_ZF_Q15_NUM_SOLVE:-0}
solve4_schur_zf_num_arshift_only=${MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_ONLY:-0}
solve4_schur_zf_num_arshift_mask=${MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_MASK:-0}
solve4_schur_zf_inline_helper=${MIMO_USE_SOLVE4_SCHUR_ZF_INLINE_HELPER:-0}
solve4_schur_zf_solve2h_model=${MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_MODEL:-0}
solve4_schur_zf_vsolve2h2_model=${MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_MODEL:-0}
solve4_schur_zf_vsolve2h2_asm=${MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_ASM:-0}
solve4_schur_zf_pivotscale_model=${MIMO_USE_SOLVE4_SCHUR_ZF_PIVOTSCALE_MODEL:-0}
check_solve2h_group_layout=${MIMO_CHECK_SOLVE2H_GROUP_LAYOUT:-0}
check_vsolve2h2_layout=${MIMO_CHECK_VSOLVE2H2_LAYOUT:-0}
solve4_schur_q15_recip_num_shift=${MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT:-15}
eq4_direct_h=${MIMO_USE_EQ4_DIRECT_H:-0}
eq4_direct_h_mode=${MIMO_EQ4_DIRECT_H_MODE:-0}
eq4_direct_h_iters=${MIMO_EQ4_DIRECT_H_ITERS:-3}
eq4_direct_h_nodiv=${MIMO_EQ4_DIRECT_H_NODIV:-0}
solve_skip_lower_update=${MIMO_USE_SOLVE_SKIP_LOWER_UPDATE:-0}
rvv_solve4=${MIMO_USE_RVV_SOLVE4:-0}
rvv_solve4_streamed=${MIMO_USE_RVV_SOLVE4_STREAMED:-0}
rvv_solve4_vnclip=${MIMO_USE_RVV_SOLVE4_VNCLIP_ROUND:-0}
rvv_solve4_q15_i32_mul=${MIMO_USE_RVV_SOLVE4_Q15_I32_MUL:-0}
rvv_zf4_schur_rvv_solve=${MIMO_USE_RVV_ZF4_SCHUR_RVV_SOLVE:-0}
rvv_mmse4_schur_rvv_solve=${MIMO_USE_RVV_MMSE4_SCHUR_RVV_SOLVE:-0}
rvv_zf4_fused_build_solve=${MIMO_USE_RVV_ZF4_FUSED_BUILD_SOLVE:-0}
rvv_zf4_staged_build_solve=${MIMO_USE_RVV_ZF4_STAGED_BUILD_SOLVE:-0}
rvv_zf4_seed_acc=${MIMO_USE_RVV_ZF4_SEED_ACC:-0}
rvv_zf4_q15acc_build=${MIMO_USE_RVV_ZF4_Q15ACC_BUILD:-0}
rvv_zf4_pack32_store=${MIMO_USE_RVV_ZF4_PACK32_STORE:-0}
rvv_site4_mf2=${MIMO_USE_RVV_SITE4_MF2:-0}
eq2_zf_direct_h=${MIMO_USE_EQ2_ZF_DIRECT_H:-0}
eq2_zf_guard1_fine_recip=${MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP:-0}
ext_eq2_zf_guard1_hybrid=${MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID:-0}
eq2_direct_q13_inv=${MIMO_USE_EQ2_DIRECT_Q13_INV:-0}
rvv_eq2_m1=${MIMO_USE_RVV_EQ2_M1:-0}
rvv_eq2_always_add_lambda=${MIMO_USE_RVV_EQ2_ALWAYS_ADD_LAMBDA:-0}
rvv_eq2_split_build=${MIMO_USE_RVV_EQ2_SPLIT_BUILD:-0}
rvv_eq2_fused_nclip_build=${MIMO_USE_RVV_EQ2_FUSED_NCLIP_BUILD:-0}
rvv_eq2_tx_soa_store=${MIMO_USE_RVV_EQ2_TX_SOA_STORE:-0}
rvv_eq2_3m_build=${MIMO_USE_RVV_EQ2_3M_BUILD:-0}
rvv_eq2_zext_pack=${MIMO_USE_RVV_EQ2_ZEXT_PACK:-0}
rvv_eq2_ri_soa_output=${MIMO_USE_RVV_EQ2_RI_SOA_OUTPUT:-0}
rvv_eq2_staged_narrow_build=${MIMO_USE_RVV_EQ2_STAGED_NARROW_BUILD:-0}
rvv_eq2_implicit_vxrm=${MIMO_USE_RVV_EQ2_IMPLICIT_VXRM:-0}
rvv_eq2_uq12_scale=${MIMO_USE_RVV_EQ2_UQ12_SCALE:-0}
rvv_eq2_early_recip_overlap=${MIMO_USE_RVV_EQ2_EARLY_RECIP_OVERLAP:-0}
rvv_eq2_block_soa=${MIMO_USE_RVV_EQ2_BLOCK_SOA:-0}
rvv_eq2_group_load=${MIMO_USE_RVV_EQ2_GROUP_LOAD:-0}
rvv_eq2_group_load_m4=${MIMO_USE_RVV_EQ2_GROUP_LOAD_M4:-0}
ext_eq2_invcoeff=${MIMO_USE_EXT_EQ2_INVCOEFF:-1}
ext_eq2_m2_batch=${MIMO_USE_EXT_EQ2_M2_BATCH:-0}
ext_eq2_rhs_interleave=${MIMO_USE_EXT_EQ2_RHS_INTERLEAVE:-0}
mmse4_schur_no_mag_scale=${MIMO_USE_MMSE4_SCHUR_NO_MAG_SCALE:-0}
mmse4_schur_signed_trunc_scale=${MIMO_USE_MMSE4_SCHUR_SIGNED_TRUNC_SCALE:-0}
mmse4_schur_num_arshift_scale=${MIMO_USE_MMSE4_SCHUR_NUM_ARSHIFT_SCALE:-0}
mmse4_schur_inner_q30_once=${MIMO_USE_MMSE4_SCHUR_INNER_Q30_ONCE:-0}
rvv_mmse4_schur_inner_sym_round=${MIMO_USE_RVV_MMSE4_SCHUR_INNER_SYM_ROUND:-0}
rvv_mmse4_schur_top_q30_once=${MIMO_USE_RVV_MMSE4_SCHUR_TOP_Q30_ONCE:-0}
rvv_mmse4_staged_build_solve=${MIMO_USE_RVV_MMSE4_STAGED_BUILD_SOLVE:-0}
rvv_mmse4_early_tail_rhs=${MIMO_USE_RVV_MMSE4_EARLY_TAIL_RHS:-0}
rvv_mmse4_const_vl=${MIMO_USE_RVV_MMSE4_CONST_VL:-0}
rvv_mmse4_seed_acc=${MIMO_USE_RVV_MMSE4_SEED_ACC:-0}
rvv_mmse4_schur_true16b_solve=${MIMO_USE_RVV_MMSE4_SCHUR_TRUE16B_SOLVE:-0}
rvv_mmse4_schur_true16b_shared_a16=${MIMO_USE_RVV_MMSE4_SCHUR_TRUE16B_SHARED_A16:-0}
rvv_schur4_solve2_pair_m2=${MIMO_USE_RVV_SCHUR4_SOLVE2_PAIR_M2:-0}
rvv_schur4_vsmul_scale=${MIMO_USE_RVV_SCHUR4_VSMUL_SCALE:-0}
rvv_schur4_num_mac=${MIMO_USE_RVV_SCHUR4_NUM_MAC:-0}
rvv_schur4_complex_mac=${MIMO_USE_RVV_SCHUR4_COMPLEX_MAC:-0}
rvv_schur4_vssra_round=${MIMO_USE_RVV_SCHUR4_VSSRA_ROUND:-0}
rvv_site4_vssra_round=${MIMO_USE_RVV_SITE4_VSSRA_ROUND:-0}
rvv_site4_seg2_load=${MIMO_USE_RVV_SITE4_SEG2_LOAD:-0}
rvv_site4_ri_soa=${MIMO_USE_RVV_SITE4_RI_SOA:-0}
rvv_schur4_det_mac=${MIMO_USE_RVV_SCHUR4_DET_MAC:-0}
ext_mmse4_schur_final_signed_trunc_scale=${MIMO_USE_EXT_MMSE4_SCHUR_FINAL_SIGNED_TRUNC_SCALE:-0}
ext_mmse4_schur_signed_trunc_mask=${MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK:-0}
ext_mmse4_rvv_i32_vec_lambda=${MIMO_USE_EXT_MMSE4_RVV_I32_VEC_LAMBDA:-0}
ext_mmse4_rvv_i32_late_tail_load=${MIMO_USE_EXT_MMSE4_RVV_I32_LATE_TAIL_LOAD:-0}
ext_zf4_local_solve=${MIMO_USE_EXT_ZF4_LOCAL_SOLVE:-0}
ext_zf4_rvv_nodiv_solve=${MIMO_USE_EXT_ZF4_RVV_NODIV_SOLVE:-0}
ext_zf4_rvv_nodiv_fast_solve=${MIMO_USE_EXT_ZF4_RVV_NODIV_FAST_SOLVE:-0}
ext_zf4_schur_rvv_solve=${MIMO_USE_EXT_ZF4_SCHUR_RVV_SOLVE:-0}
ext_mmse4_schur_rvv_solve=${MIMO_USE_EXT_MMSE4_SCHUR_RVV_SOLVE:-0}
ext_mmse4_fused_build_solve=${MIMO_USE_EXT_MMSE4_FUSED_BUILD_SOLVE:-0}
ext_mmse4_narrow_work=${MIMO_USE_EXT_MMSE4_NARROW_WORK:-0}
ext_mmse4_live_build=${MIMO_USE_EXT_MMSE4_LIVE_BUILD:-0}
ext_mmse4_stream_work_solve=${MIMO_USE_EXT_MMSE4_STREAM_WORK_SOLVE:-0}
ext_mmse4_true16b_inline_solve=${MIMO_USE_EXT_MMSE4_TRUE16B_INLINE_SOLVE:-0}
ext_mmse4_true16b_live_tail_solve=${MIMO_USE_EXT_MMSE4_TRUE16B_LIVE_TAIL_SOLVE:-0}
ext_zf4_rvv_round_once_build=${MIMO_USE_EXT_ZF4_RVV_ROUND_ONCE_BUILD:-0}
ext_zf4_true16b_work=${MIMO_USE_EXT_ZF4_TRUE16B_WORK:-0}
ext_zf4_pack32_work=${MIMO_USE_EXT_ZF4_PACK32_WORK:-0}
ext_zf4_vscmacc_build=${MIMO_USE_EXT_ZF4_VSCMACC_BUILD:-0}
ext_zf4_vscmacc_fused_build=${MIMO_USE_EXT_ZF4_VSCMACC_FUSED_BUILD:-0}
ext_zf4_inline_upper_i16_solve=${MIMO_USE_EXT_ZF4_INLINE_UPPER_I16_SOLVE:-0}
ext_zf4_live_tail_schur=${MIMO_USE_EXT_ZF4_LIVE_TAIL_SCHUR:-0}
ext_zf4_tail_scratch_reuse=${MIMO_USE_EXT_ZF4_TAIL_SCRATCH_REUSE:-0}
ext_zf4_inline_head_work_reuse=${MIMO_USE_EXT_ZF4_INLINE_HEAD_WORK_REUSE:-0}
ext_zf4_top2_macc_sum=${MIMO_USE_EXT_ZF4_TOP2_MACC_SUM:-0}
solve4_schur_zf_ext_macc_inner=${MIMO_USE_SOLVE4_SCHUR_ZF_EXT_MACC_INNER:-0}
ext_zf4_true16b_rvv_i32_solve=${MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE:-0}
ext_zf4_true16b_rvv_i32_bsolve=${MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_BSOLVE:-0}
ext_zf4_scalar_x23_store=${MIMO_USE_EXT_ZF4_SCALAR_X23_STORE:-0}
ext_zf4_true16b_rvv_i32_split_solve=${MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SPLIT_SOLVE:-0}
ext_zf4_true16b_rvv_i32_pack32_store=${MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_STORE:-0}
ext_zf4_true16b_rvv_i32_pack32_load=${MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_LOAD:-0}
ext_zf4_true16b_rvv_i32_preserve_prelude=${MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PRESERVE_PRELUDE:-0}
ext_zf4_fast_corr_iters=${MIMO_EXT_ZF4_FAST_CORR_ITERS:-8}
ext_zf4_fast_scale_bias_q30=${MIMO_EXT_ZF4_FAST_SCALE_BIAS_Q30:-0}
ext_zf4_fast_reuse_scale=${MIMO_EXT_ZF4_FAST_REUSE_SCALE:-0}
ext_zf4_upper_rvv_solve=${MIMO_USE_EXT_ZF4_UPPER_RVV_SOLVE:-0}
ext_zf4_mixed_update_solve=${MIMO_USE_EXT_ZF4_MIXED_UPDATE_SOLVE:-0}
debug_ext_zf4_rvv_nodiv_compare=${MIMO_DEBUG_EXT_ZF4_RVV_NODIV_COMPARE:-0}
site4_solve_soa=${MIMO_USE_SITE4_SOLVE_SOA:-0}
site4_solve_soa_unroll=${MIMO_USE_SITE4_SOLVE_SOA_UNROLL:-0}
site4_diag_real_build=${MIMO_USE_SITE4_DIAG_REAL_BUILD:-0}
site4_hermitian_upper_build=${MIMO_USE_SITE4_HERMITIAN_UPPER_BUILD:-0}
site4_hermitian_upper_mmse=${MIMO_USE_SITE4_HERMITIAN_UPPER_MMSE:-0}
site4_hermitian_upper_packed=${MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED:-0}
site4_hermitian_upper_rowfuse=${MIMO_USE_SITE4_HERMITIAN_UPPER_ROWFUSE:-0}
site4_hermitian_upper_rowpair=${MIMO_USE_SITE4_HERMITIAN_UPPER_ROWPAIR:-0}
site4_round_once_dot=${MIMO_USE_SITE4_ROUND_ONCE_DOT:-0}
site4_prelude_hoist=${MIMO_USE_SITE4_PRELUDE_HOIST:-0}
site4_build_solve_pipeline2=${MIMO_USE_SITE4_BUILD_SOLVE_PIPELINE2:-0}
den_stats=${MIMO_COLLECT_SOLVE_DEN_STATS:-0}
value_stats=${MIMO_COLLECT_SOLVE_VALUE_STATS:-0}
schur_stats=${MIMO_COLLECT_SCHUR_STATS:-0}
pivot_scale_stats=${MIMO_COLLECT_PIVOT_SCALE_STATS:-0}
asm_expect=${MIMO_GATE_ASM_EXPECT:-}
asm_expect_backends=" ${MIMO_GATE_ASM_EXPECT_BACKENDS:-${backends[*]}} "
asm_div_budget_enabled=0
if [[ -n "${MIMO_ASM_MAX_DIV_SIGNED:-}" ||
      -n "${MIMO_ASM_MAX_DIV_UNSIGNED:-}" ||
      -n "${MIMO_ASM_MAX_VDIV_VV:-}" ||
      -n "${MIMO_ASM_MAX_SCALAR_LOAD:-}" ||
      -n "${MIMO_ASM_MAX_STACK_LOAD:-}" ||
      -n "${MIMO_ASM_MAX_STACK_STORE:-}" ||
      -n "${MIMO_ASM_MAX_STACK_FRAME_BYTES:-}" ||
	      "${MIMO_ASM_FORBID_HARD_DIV:-0}" == "1" ||
	      "${MIMO_ASM_FORBID_SEGMENT_LDST:-1}" == "1" ]]; then
  asm_div_budget_enabled=1
fi

case "${kernel}" in
  eq_zf_2x2|eq_mmse_2x2|eq_zf_4x4|eq_mmse_4x4) ;;
  *)
    echo "Refusing gate run: current scope permits only 2x2/4x4 ZF/MMSE kernels." >&2
    exit 2
    ;;
esac

case "${validate_kernel}" in
  eq_zf_2x2|eq_mmse_2x2|eq_zf_4x4|eq_mmse_4x4) ;;
  *)
    echo "Refusing gate run: current scope permits only 2x2/4x4 ZF/MMSE validation." >&2
    exit 2
    ;;
esac

if [[ "${eq2_zf_guard1_fine_recip}" != "0" &&
      "${eq2_zf_guard1_fine_recip}" != "1" ]]; then
  echo "Refusing gate run: MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP must be 0 or 1." >&2
  exit 2
fi
if [[ "${eq2_zf_guard1_fine_recip}" == "1" &&
      ( "${kernel}" != "eq_zf_2x2" ||
        "${validate_kernel}" != "eq_zf_2x2" ||
        "${eq2_zf_direct_h}" != "1" ) ]]; then
  echo "Refusing gate run: guard1 fine reciprocal requires ZF2 direct-H for both performance and validation." >&2
  exit 2
fi
if [[ "${eq2_zf_guard1_fine_recip}" == "1" ]]; then
  for backend in "${backends[@]}"; do
    if [[ "${backend}" == "ext" ]]; then
      echo "Refusing gate run: guard1 fine reciprocal is valid only for scalar/RVV." >&2
      exit 2
    fi
    if [[ "${backend}" == "rvv" &&
          ( "${rvv_eq2_m1}" != "1" ||
            "${rvv_eq2_implicit_vxrm}" != "1" ||
            "${rvv_eq2_uq12_scale}" != "0" ) ]]; then
      echo "Refusing gate run: RVV guard1 requires wide-m1, implicit RNU, and Q30 scale." >&2
      exit 2
    fi
  done
fi
if [[ "${ext_eq2_zf_guard1_hybrid}" != "0" &&
      "${ext_eq2_zf_guard1_hybrid}" != "1" ]]; then
  echo "Refusing gate run: MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID must be 0 or 1." >&2
  exit 2
fi
if [[ "${ext_eq2_zf_guard1_hybrid}" == "1" ]]; then
  if [[ "${kernel}" != "eq_zf_2x2" ||
        "${validate_kernel}" != "eq_zf_2x2" ||
        "${eq2_zf_direct_h}" != "1" ||
        "${eq2_zf_guard1_fine_recip}" != "0" ||
        "${ext_eq2_invcoeff}" != "0" || "${ext_eq2_m2_batch}" != "1" ||
        "${ext_eq2_rhs_interleave}" != "0" ]]; then
    echo "Refusing gate run: EXT ZF2 guard1 hybrid requires direct-H m2 with no scalar/RVV guard1, invcoeff, or RHS interleave." >&2
    exit 2
  fi
  for backend in "${backends[@]}"; do
    if [[ "${backend}" != "ext" ]]; then
      echo "Refusing gate run: EXT ZF2 guard1 hybrid is valid only for the EXT backend." >&2
      exit 2
    fi
  done
fi

if [[ "${rvv_eq2_early_recip_overlap}" != "0" && "${rvv_eq2_early_recip_overlap}" != "1" ]]; then
  echo "Refusing gate run: MIMO_USE_RVV_EQ2_EARLY_RECIP_OVERLAP must be 0 or 1." >&2
  exit 2
fi
if [[ "${rvv_eq2_early_recip_overlap}" == "1" &&
      ( "${validate_kernel}" != "eq_mmse_2x2" || "${rvv_eq2_m1}" != "1" ) ]]; then
  echo "Refusing gate run: early reciprocal overlap requires MMSE2 RVV m1." >&2
  exit 2
fi
if [[ "${rvv_eq2_early_recip_overlap}" == "1" && "${rvv_eq2_uq12_scale}" == "1" ]]; then
  echo "Refusing gate run: early reciprocal overlap and UQ12 scale are mutually exclusive." >&2
  exit 2
fi

refuse_zf4_ext_schur_mode4_missing_hermitian() {
  local selected_backend=$1

  if [[ "${selected_backend}" == "ext" &&
        "${kernel}" == "eq_zf_4x4" &&
        "${solve4_schur}" == "1" &&
        "${solve4_schur_q15_recip}" == "4" &&
        "${solve4_schur_q15_recip_num_shift}" == "13" &&
        "${site4_solve_soa}" == "1" &&
        "${site4_hermitian_upper_build}" == "1" &&
        "${solve4_hermitian}" != "1" ]]; then
    echo "Refusing gate run: EXT eq_zf_4x4 Schur q15recip mode4 upper-SoA requires MIMO_USE_SOLVE4_HERMITIAN=1; otherwise the wrapper falls back to the generic Schur helper instead of the current-best ZF-only helper." >&2
    exit 2
  fi
}

asm_expect_flag=
case "${asm_expect}" in
  "")
    ;;
  ext-site4-v3)
    asm_expect_flag=--expect-ext-site4-v3
    ;;
  site4-rvv-lane-solve-v1)
    asm_expect_flag=--expect-site4-rvv-lane-solve-v1
    ;;
  site4-rvv-solve-vset-schedule-v1)
    asm_expect_flag=--expect-site4-rvv-solve-vset-schedule-v1
    ;;
  zf4-mode4-microkernel)
    asm_expect_flag=--expect-zf4-mode4-microkernel
    ;;
  *)
    echo "Unsupported MIMO_GATE_ASM_EXPECT='${asm_expect}'" >&2
    exit 2
    ;;
esac

auto_asm_symbol() {
  local backend=$1

  case "${kernel}" in
    eq_zf_2x2|eq_mmse_2x2|eq_zf_4x4|eq_mmse_4x4)
      printf 'mimo_%s_%s\n' "${kernel}" "${backend}"
      ;;
    *)
      printf '\n'
      ;;
  esac
}

asm_symbol_exists() {
  local symbol=$1
  local asm_path=$2

  [[ -n "$(resolve_asm_symbol "${symbol}" "${asm_path}")" ]]
}

resolve_asm_symbol() {
  local requested=$1
  local asm_path=$2

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
  ' "${asm_path}"
}

check_nemu_correctness_log() {
  local backend=$1
  local tag=$2
  local log=$3
  local am_log=$4
  case "${validate_kernel}" in
    eq_zf_2x2|eq_mmse_2x2|eq_zf_4x4|eq_mmse_4x4)
      ;;
    *)
      echo "MIMO_GATE_NEMU_LOG_FAILED backend=${backend} tag=${tag} reason=unsupported-validate-kernel kernel=${validate_kernel} log=${log} am_log=${am_log}" >&2
      return 1
      ;;
  esac

  if grep -Eq 'HIT CRITICAL ERROR|BAD TRAP|MIMO_VALIDATE_FAIL|MIMO_DONE status=[^0]' "${log}"; then
    echo "MIMO_GATE_NEMU_LOG_FAILED backend=${backend} tag=${tag} reason=bad-marker log=${log} am_log=${am_log}" >&2
    return 1
  fi

  if ! grep -q 'MIMO_DONE status=0' "${log}"; then
    echo "MIMO_GATE_NEMU_LOG_FAILED backend=${backend} tag=${tag} reason=missing-done log=${log} am_log=${am_log}" >&2
    return 1
  fi

  if ! grep -q "MIMO_VALIDATE_PASS kernel=${validate_kernel} " "${log}"; then
    echo "MIMO_GATE_NEMU_LOG_FAILED backend=${backend} tag=${tag} reason=missing-pass kernel=${validate_kernel} log=${log} am_log=${am_log}" >&2
    return 1
  fi
}

asm_symbol_refs() {
  local caller=$1
  local callee=$2
  local asm_path=$3

  awk -v needle="<${caller}>:" '
    index($0, needle) { in_symbol = 1; next }
    in_symbol && /^[[:xdigit:]]+ <[^.][^>]*>:/ { in_symbol = 0 }
    in_symbol { print }
  ' "${asm_path}" | grep -F -q "<${callee}>"
}

collect_hard_div_symbols() {
  local wrapper=$1
  local asm_path=$2
  local helper helper_symbol caller existing changed found wrapper_symbol
  wrapper_symbol=$(resolve_asm_symbol "${wrapper}" "${asm_path}")
  if [[ -z "${wrapper_symbol}" ]]; then
    wrapper_symbol=${wrapper}
  fi
  local -a symbols=("${wrapper_symbol}")
  local -a helpers=(
    mimo_solve_normal4_site4_hermitian_zf_hand_lane_q15
    c32_factor_q30_pivotscale_call
    herm_pivot_div_to_q30_scaled_call
    c32_div_real_to_q15_pivotscale_call
    mimo_solve_normal4_site4_hermitian_zf_q15
    mimo_solve_normal4_site4_soa_hermitian_gs_q15
	    mimo_solve_normal4_site4_soa_hermitian_zf_q15
	    mimo_ext_zf4_local_solve_soa_q15
	    mimo_ext_zf4_rvv_nodiv_solve_soa_q15
	    mimo_ext_zf4_rvv_nodiv_fast_solve_soa_q15
	    mimo_ext_zf4_schur_rvv_solve_soa_q15
	    mimo_rvv_zf4_schur_rvv_solve_soa_q15
	    mimo_ext_mmse4_schur_rvv_solve_soa_q15
	    mimo_rvv_mmse4_schur_rvv_solve_soa_q15
	    mimo_solve_normal4_site4_upper_i16_schur_zf_q15
	    mimo_solve_normal4_site4_upper_i16_schur_zf_rvv_i32_q15
	    mimo_solve_normal4_site4_upper_i16_schur_zf_rvv_i32_split_q15
	    mimo_solve_normal4_site4_upper_i16_schur_zf_rvv_i32_split_ext_q15
	    mimo_rvv_solve_zf4_upper_hermitian_site4_q15
	    mimo_rvv_solve_zf4_upper_hermitian_site4_mixed_update_q15
    mimo_solve_normal4_site4_ldlh_q15
    mimo_solve_normal4_site4_schur_q15
    mimo_solve_normal4_site4_soa_schur_q15
    mimo_solve_normal4_site4_soa_schur_zf_q15
    schur4_solve_site4_zf_ext_inner_q15
    schur4_solve_site4_zf_ext_inner2_q15
    schur4_solve_site4_zf_ext_inner3_q15
    schur4_solve_lane_zf_reload_micro_q15
    schur4_solve_lane_q15
	    mimo_solve_normal4_site4_q15
    mimo_solve_normal4_site4_soa_q15
    mimo_solve_normal4_site4_soa_unroll_lane_q15
	    mimo_eq_mmse4_scalar_fused_q15
	    mimo_eq2_zf_scalar_fused_q15
	    mimo_eq2_mmse_scalar_fused_q15
	    mimo_eq2_zf_rvv_ri_soa_q15
	    mimo_eq2_mmse_rvv_ri_soa_q15
    mimo_solve_normal4_scalar_q15
    mimo_solve_normal_4_schur_scalar
    mimo_solve_normal_4_exact
    mimo_solve_normal_q15
    mimo_solve_normal_ref_q15
    mimo_div_c32_by_i32
    mimo_div_q15_by_i32
  )

  changed=1
  while [[ "${changed}" == "1" ]]; do
    changed=0
    for helper in "${helpers[@]}"; do
      helper_symbol=$(resolve_asm_symbol "${helper}" "${asm_path}")
      [[ -n "${helper_symbol}" ]] || continue

      found=0
      for existing in "${symbols[@]}"; do
        if [[ "${existing}" == "${helper_symbol}" ]]; then
          found=1
          break
        fi
      done
      [[ "${found}" == "0" ]] || continue

      for caller in "${symbols[@]}"; do
        asm_symbol_refs "${caller}" "${helper_symbol}" "${asm_path}" || continue

        symbols+=("${helper_symbol}")
        changed=1
        break
      done
    done
  done

  if [[ "${MIMO_USE_SOLVE4_HERMITIAN_HAND_SCHED:-0}" == "1" ||
        "${MIMO_USE_SOLVE4_HERMITIAN_FACTOR_CALL:-0}" == "1" ||
        "${MIMO_USE_SOLVE4_HERMITIAN_Q15_CALL:-0}" == "1" ]]; then
    for helper in c32_factor_q30_pivotscale_call \
                  herm_pivot_div_to_q30_scaled_call \
                  c32_div_real_to_q15_pivotscale_call; do
      helper_symbol=$(resolve_asm_symbol "${helper}" "${asm_path}")
      [[ -n "${helper_symbol}" ]] || continue

      found=0
      for existing in "${symbols[@]}"; do
        if [[ "${existing}" == "${helper_symbol}" ]]; then
          found=1
          break
        fi
      done
      [[ "${found}" == "0" ]] || continue

      symbols+=("${helper_symbol}")
    done
  fi

  printf '%s\n' "${symbols[@]}"
}

mkdir -p "${APP}/perf_eval/raw/nemu" "${APP}/perf_eval/asm"

cd "${ROOT}"
source ./env.sh >/dev/null
cd "${APP}"

for backend in "${backends[@]}"; do
  refuse_zf4_ext_schur_mode4_missing_hermitian "${backend}"

  if [[ "${MIMO_GATE_SKIP_NEMU:-0}" != "1" ]]; then
    nemu_log="${APP}/perf_eval/raw/nemu/nemu-mimo-${backend}-correctness-${tag}.log"
    am_log="${APP}/perf_eval/raw/nemu/nemu-mimo-${backend}-${tag}-am-log.txt"

    echo "MIMO_GATE_NEMU_START backend=${backend} tag=${tag} timeout_sec=${timeout_sec} log=${nemu_log}"
    make ARCH=riscv64-xs MIMO_IMPL="${backend}" clean >/dev/null
    set +e
    make ARCH=riscv64-xs MIMO_IMPL="${backend}" \
	      MIMO_VALIDATE=1 \
	      MIMO_VALIDATE_KERNEL="${validate_kernel}" \
	      MIMO_VALIDATE_TOL="${validate_tol}" \
	      MIMO_VALIDATE_ERROR_STATS="${validate_error_stats}" \
	      MIMO_VALIDATE_FAIL_PRINT_LIMIT="${validate_fail_print_limit}" \
	      MIMO_USE_RECIP_TABLE="${recip_table}" \
	      MIMO_USE_SOLVE4_UNROLL_V2A="${solve4_unroll}" \
	      MIMO_USE_SOLVE4_HERMITIAN="${solve4_hermitian}" \
	      MIMO_USE_SOLVE4_HERMITIAN_LANE_OUTER="${solve4_hermitian_lane_outer}" \
	      MIMO_USE_SOLVE4_HERMITIAN_SOA_STAGE_OUTER="${solve4_hermitian_soa_stage_outer}" \
	      MIMO_USE_SOLVE4_HERMITIAN_FACTOR_PRECOMPUTE="${solve4_hermitian_factor_precompute}" \
	      MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE="${solve4_hermitian_pivot_scale}" \
	      MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE_HARDDIV="${solve4_hermitian_pivot_scale_harddiv}" \
	      MIMO_USE_SOLVE4_HERMITIAN_HAND_SCHED="${solve4_hermitian_hand_sched}" \
	      MIMO_USE_SOLVE4_HERMITIAN_FACTOR_CALL="${solve4_hermitian_factor_call}" \
	      MIMO_USE_SOLVE4_HERMITIAN_FACTOR_Q15="${solve4_hermitian_factor_q15}" \
	      MIMO_USE_SOLVE4_HERMITIAN_Q15_CALL="${solve4_hermitian_q15_call}" \
	      MIMO_USE_SOLVE4_HERMITIAN_Q15_ONECORR="${solve4_hermitian_q15_onecorr}" \
	      MIMO_SOLVE4_HERMITIAN_Q15_JUMP_ONLY_MASK="${solve4_hermitian_q15_jump_only_mask}" \
	      MIMO_USE_SOLVE4_HERMITIAN_GS="${solve4_hermitian_gs}" \
	      MIMO_SOLVE4_HERMITIAN_GS_ITERS="${solve4_hermitian_gs_iters}" \
	      MIMO_USE_SOLVE4_HERMITIAN_Q30_FIVECORR="${solve4_hermitian_q30_fivecorr}" \
	      MIMO_USE_SOLVE4_HERMITIAN_Q30_TWOJUMP_CORR="${solve4_hermitian_q30_twojump_corr}" \
	      MIMO_USE_SOLVE4_HERMITIAN_Q30_JUMP_ONLY="${solve4_hermitian_q30_jump_only}" \
	      MIMO_USE_SOLVE4_HERMITIAN_Q30_ONECORR="${solve4_hermitian_q30_onecorr}" \
	      MIMO_USE_SOLVE4_HERMITIAN_Q30_PAIR_CORR="${solve4_hermitian_q30_pair_corr}" \
	      MIMO_SOLVE4_HERMITIAN_Q30_JUMP_ONLY_MASK="${solve4_hermitian_q30_jump_only_mask}" \
	      MIMO_USE_SOLVE4_HERMITIAN_POS_PIVOT_SCALE="${solve4_hermitian_pos_pivot_scale}" \
	      MIMO_USE_SOLVE4_HERMITIAN_TRUST_PIVOT_RANGE="${solve4_hermitian_trust_pivot_range}" \
	      MIMO_USE_SOLVE4_HERMITIAN_TRUST_PIVOT_NONZERO="${solve4_hermitian_trust_pivot_nonzero}" \
	      MIMO_USE_SOLVE4_HERMITIAN_FULL_RECIP_TABLE="${solve4_hermitian_full_recip_table}" \
	      MIMO_USE_SOLVE4_HERMITIAN_FLOOR_FULL_RECIP_TABLE="${solve4_hermitian_floor_full_recip_table}" \
	      MIMO_USE_SOLVE4_HERMITIAN_RECIP_STEP16="${solve4_hermitian_recip_step16}" \
	      MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE="${solve4_hermitian_diag_real_update}" \
	      MIMO_USE_SOLVE4_HERMITIAN_DIAG_APPROX="${solve4_hermitian_diag_approx}" \
	      MIMO_USE_SOLVE_BRANCHLIGHT_ROUND="${solve_branchlight_round}" \
	      MIMO_USE_SOLVE_Q15_I64_ARSHIFT_ROUND="${solve_q15_i64_arshift_round}" \
	      MIMO_USE_SOLVE_STORE_TRUST_I16="${solve_store_trust_i16}" \
	      MIMO_USE_SCALAR4_UPPER_ONLY_BUILD="${scalar4_upper_only_build}" \
	      MIMO_USE_SCALAR4_COMPLEX_MUL_3M="${scalar4_complex_mul_3m}" \
	      MIMO_USE_SCALAR4_SCHUR_TOP_Q30_ONCE="${scalar4_schur_top_q30_once}" \
	      MIMO_USE_SCALAR_MMSE4_FUSED_BUILD_SOLVE="${scalar_mmse4_fused_build_solve}" \
	      MIMO_USE_SCALAR_MMSE4_FUSED_COMPACT_C32="${scalar_mmse4_fused_compact_c32}" \
	      MIMO_USE_SCALAR_ZF4_Q15ACC_BUILD="${scalar_zf4_q15acc_build}" \
	      MIMO_USE_SOLVE4_LDLH="${solve4_ldlh}" \
	      MIMO_USE_SOLVE4_SCHUR="${solve4_schur}" \
	      MIMO_USE_SOLVE4_SCHUR_PAIR2="${solve4_schur_pair2}" \
	      MIMO_USE_SOLVE4_SCHUR_DET_SCALE="${solve4_schur_det_scale}" \
	      MIMO_USE_SOLVE4_SCHUR_BINV_Q15="${solve4_schur_binv_q15}" \
	      MIMO_USE_SOLVE4_SCHUR_Q15_RECIP="${solve4_schur_q15_recip}" \
	      MIMO_USE_SOLVE4_SCHUR_FUSED_INNER="${solve4_schur_fused_inner}" \
	      MIMO_USE_SOLVE4_SCHUR_FUSED_INNER_MMSE="${solve4_schur_fused_inner_mmse}" \
	      MIMO_USE_SOLVE4_SCHUR_MMSE_SPECIALIZE="${solve4_schur_mmse_specialize}" \
	      MIMO_USE_SOLVE4_SCHUR_ZF_EXPLICIT="${solve4_schur_zf_explicit}" \
	      MIMO_USE_SOLVE4_SCHUR_ZF_RELOAD_MICRO="${solve4_schur_zf_reload_micro}" \
	      MIMO_USE_SOLVE4_SCHUR_ZF_TOP_RESOLVE="${solve4_schur_zf_top_resolve}" \
	      MIMO_USE_SOLVE4_SCHUR_ZF_HANDASM="${solve4_schur_zf_handasm}" \
		      MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER="${solve4_schur_zf_ext_inner}" \
		      MIMO_USE_SOLVE4_SCHUR_ZF_I32_NUM="${solve4_schur_zf_i32_num}" \
		      MIMO_USE_SOLVE4_SCHUR_ZF_EXT_MACC_INNER="${solve4_schur_zf_ext_macc_inner}" \
		      MIMO_USE_SOLVE4_SCHUR_ZF_Q15_NUM_SOLVE="${solve4_schur_zf_q15_num_solve}" \
		      MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_ONLY="${solve4_schur_zf_num_arshift_only}" \
		      MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_MASK="${solve4_schur_zf_num_arshift_mask}" \
			      MIMO_USE_SOLVE4_SCHUR_ZF_INLINE_HELPER="${solve4_schur_zf_inline_helper}" \
		      MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_MODEL="${solve4_schur_zf_solve2h_model}" \
		      MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_MODEL="${solve4_schur_zf_vsolve2h2_model}" \
		      MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_ASM="${solve4_schur_zf_vsolve2h2_asm}" \
		      MIMO_USE_SOLVE4_SCHUR_ZF_PIVOTSCALE_MODEL="${solve4_schur_zf_pivotscale_model}" \
		      MIMO_CHECK_SOLVE2H_GROUP_LAYOUT="${check_solve2h_group_layout}" \
		      MIMO_CHECK_VSOLVE2H2_LAYOUT="${check_vsolve2h2_layout}" \
	      MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT="${solve4_schur_q15_recip_num_shift}" \
	      MIMO_USE_EQ4_DIRECT_H="${eq4_direct_h}" \
	      MIMO_EQ4_DIRECT_H_MODE="${eq4_direct_h_mode}" \
	      MIMO_EQ4_DIRECT_H_ITERS="${eq4_direct_h_iters}" \
	      MIMO_EQ4_DIRECT_H_NODIV="${eq4_direct_h_nodiv}" \
	      MIMO_USE_SOLVE_SKIP_LOWER_UPDATE="${solve_skip_lower_update}" \
	      MIMO_USE_RVV_SOLVE4="${rvv_solve4}" \
	      MIMO_USE_RVV_SOLVE4_STREAMED="${rvv_solve4_streamed}" \
	      MIMO_USE_RVV_SOLVE4_VNCLIP_ROUND="${rvv_solve4_vnclip}" \
	      MIMO_USE_RVV_SOLVE4_Q15_I32_MUL="${rvv_solve4_q15_i32_mul}" \
	      MIMO_USE_RVV_ZF4_SCHUR_RVV_SOLVE="${rvv_zf4_schur_rvv_solve}" \
	      MIMO_USE_RVV_MMSE4_SCHUR_RVV_SOLVE="${rvv_mmse4_schur_rvv_solve}" \
	      MIMO_USE_RVV_ZF4_FUSED_BUILD_SOLVE="${rvv_zf4_fused_build_solve}" \
	      MIMO_USE_RVV_ZF4_STAGED_BUILD_SOLVE="${rvv_zf4_staged_build_solve}" \
	      MIMO_USE_RVV_ZF4_SEED_ACC="${rvv_zf4_seed_acc}" \
	      MIMO_USE_RVV_ZF4_Q15ACC_BUILD="${rvv_zf4_q15acc_build}" \
	      MIMO_USE_RVV_ZF4_PACK32_STORE="${rvv_zf4_pack32_store}" \
	      MIMO_USE_RVV_SITE4_MF2="${rvv_site4_mf2}" \
	      MIMO_USE_EQ2_ZF_DIRECT_H="${eq2_zf_direct_h}" \
	      MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP="${eq2_zf_guard1_fine_recip}" \
	      MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID="${ext_eq2_zf_guard1_hybrid}" \
	      MIMO_USE_EQ2_DIRECT_Q13_INV="${eq2_direct_q13_inv}" \
	      MIMO_USE_RVV_EQ2_M1="${rvv_eq2_m1}" \
	      MIMO_USE_RVV_EQ2_ALWAYS_ADD_LAMBDA="${rvv_eq2_always_add_lambda}" \
	      MIMO_USE_RVV_EQ2_SPLIT_BUILD="${rvv_eq2_split_build}" \
	      MIMO_USE_RVV_EQ2_FUSED_NCLIP_BUILD="${rvv_eq2_fused_nclip_build}" \
	      MIMO_USE_RVV_EQ2_TX_SOA_STORE="${rvv_eq2_tx_soa_store}" \
	      MIMO_USE_RVV_EQ2_3M_BUILD="${rvv_eq2_3m_build}" \
	      MIMO_USE_RVV_EQ2_ZEXT_PACK="${rvv_eq2_zext_pack}" \
	      MIMO_USE_RVV_EQ2_RI_SOA_OUTPUT="${rvv_eq2_ri_soa_output}" \
	      MIMO_USE_RVV_EQ2_STAGED_NARROW_BUILD="${rvv_eq2_staged_narrow_build}" \
	      MIMO_USE_RVV_EQ2_IMPLICIT_VXRM="${rvv_eq2_implicit_vxrm}" \
	      MIMO_USE_RVV_EQ2_UQ12_SCALE="${rvv_eq2_uq12_scale}" \
	      MIMO_USE_RVV_EQ2_EARLY_RECIP_OVERLAP="${rvv_eq2_early_recip_overlap}" \
	      MIMO_USE_RVV_EQ2_BLOCK_SOA="${rvv_eq2_block_soa}" \
	      MIMO_USE_RVV_EQ2_GROUP_LOAD="${rvv_eq2_group_load}" \
	      MIMO_USE_RVV_EQ2_GROUP_LOAD_M4="${rvv_eq2_group_load_m4}" \
	      MIMO_USE_EXT_EQ2_INVCOEFF="${ext_eq2_invcoeff}" \
	      MIMO_USE_EXT_EQ2_M2_BATCH="${ext_eq2_m2_batch}" \
	      MIMO_USE_EXT_EQ2_RHS_INTERLEAVE="${ext_eq2_rhs_interleave}" \
	      MIMO_USE_MMSE4_SCHUR_NO_MAG_SCALE="${mmse4_schur_no_mag_scale}" \
	      MIMO_USE_MMSE4_SCHUR_SIGNED_TRUNC_SCALE="${mmse4_schur_signed_trunc_scale}" \
	      MIMO_USE_MMSE4_SCHUR_NUM_ARSHIFT_SCALE="${mmse4_schur_num_arshift_scale}" \
	      MIMO_USE_MMSE4_SCHUR_INNER_Q30_ONCE="${mmse4_schur_inner_q30_once}" \
	      MIMO_USE_RVV_MMSE4_SCHUR_INNER_SYM_ROUND="${rvv_mmse4_schur_inner_sym_round}" \
	      MIMO_USE_RVV_MMSE4_SCHUR_TOP_Q30_ONCE="${rvv_mmse4_schur_top_q30_once}" \
	      MIMO_USE_RVV_MMSE4_STAGED_BUILD_SOLVE="${rvv_mmse4_staged_build_solve}" \
	      MIMO_USE_RVV_MMSE4_EARLY_TAIL_RHS="${rvv_mmse4_early_tail_rhs}" \
	      MIMO_USE_RVV_MMSE4_CONST_VL="${rvv_mmse4_const_vl}" \
	      MIMO_USE_RVV_MMSE4_SEED_ACC="${rvv_mmse4_seed_acc}" \
	      MIMO_USE_RVV_MMSE4_SCHUR_TRUE16B_SOLVE="${rvv_mmse4_schur_true16b_solve}" \
	      MIMO_USE_RVV_MMSE4_SCHUR_TRUE16B_SHARED_A16="${rvv_mmse4_schur_true16b_shared_a16}" \
	      MIMO_USE_RVV_SCHUR4_SOLVE2_PAIR_M2="${rvv_schur4_solve2_pair_m2}" \
	      MIMO_USE_RVV_SCHUR4_VSMUL_SCALE="${rvv_schur4_vsmul_scale}" \
	      MIMO_USE_RVV_SCHUR4_NUM_MAC="${rvv_schur4_num_mac}" \
	      MIMO_USE_RVV_SCHUR4_COMPLEX_MAC="${rvv_schur4_complex_mac}" \
		      MIMO_USE_RVV_SCHUR4_VSSRA_ROUND="${rvv_schur4_vssra_round}" \
		      MIMO_USE_RVV_SITE4_VSSRA_ROUND="${rvv_site4_vssra_round}" \
		      MIMO_USE_RVV_SITE4_SEG2_LOAD="${rvv_site4_seg2_load}" \
		      MIMO_USE_RVV_SITE4_RI_SOA="${rvv_site4_ri_soa}" \
		      MIMO_USE_RVV_SCHUR4_DET_MAC="${rvv_schur4_det_mac}" \
	      MIMO_USE_EXT_MMSE4_SCHUR_FINAL_SIGNED_TRUNC_SCALE="${ext_mmse4_schur_final_signed_trunc_scale}" \
	      MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK="${ext_mmse4_schur_signed_trunc_mask}" \
	      MIMO_USE_EXT_MMSE4_RVV_I32_VEC_LAMBDA="${ext_mmse4_rvv_i32_vec_lambda}" \
	      MIMO_USE_EXT_MMSE4_RVV_I32_LATE_TAIL_LOAD="${ext_mmse4_rvv_i32_late_tail_load}" \
	      MIMO_USE_EXT_ZF4_LOCAL_SOLVE="${ext_zf4_local_solve}" \
		      MIMO_USE_EXT_ZF4_RVV_NODIV_SOLVE="${ext_zf4_rvv_nodiv_solve}" \
		      MIMO_USE_EXT_ZF4_RVV_NODIV_FAST_SOLVE="${ext_zf4_rvv_nodiv_fast_solve}" \
		      MIMO_USE_EXT_ZF4_SCHUR_RVV_SOLVE="${ext_zf4_schur_rvv_solve}" \
		      MIMO_USE_EXT_MMSE4_SCHUR_RVV_SOLVE="${ext_mmse4_schur_rvv_solve}" \
		      MIMO_USE_EXT_MMSE4_FUSED_BUILD_SOLVE="${ext_mmse4_fused_build_solve}" \
		      MIMO_USE_EXT_MMSE4_NARROW_WORK="${ext_mmse4_narrow_work}" \
			      MIMO_USE_EXT_MMSE4_LIVE_BUILD="${ext_mmse4_live_build}" \
			      MIMO_USE_EXT_MMSE4_STREAM_WORK_SOLVE="${ext_mmse4_stream_work_solve}" \
			      MIMO_USE_EXT_MMSE4_TRUE16B_INLINE_SOLVE="${ext_mmse4_true16b_inline_solve}" \
			      MIMO_USE_EXT_MMSE4_TRUE16B_LIVE_TAIL_SOLVE="${ext_mmse4_true16b_live_tail_solve}" \
			      MIMO_USE_EXT_ZF4_RVV_ROUND_ONCE_BUILD="${ext_zf4_rvv_round_once_build}" \
	      MIMO_USE_EXT_ZF4_TRUE16B_WORK="${ext_zf4_true16b_work}" \
	      MIMO_USE_EXT_ZF4_PACK32_WORK="${ext_zf4_pack32_work}" \
	      MIMO_USE_EXT_ZF4_VSCMACC_BUILD="${ext_zf4_vscmacc_build}" \
		      MIMO_USE_EXT_ZF4_VSCMACC_FUSED_BUILD="${ext_zf4_vscmacc_fused_build}" \
		      MIMO_USE_EXT_ZF4_INLINE_UPPER_I16_SOLVE="${ext_zf4_inline_upper_i16_solve}" \
			      MIMO_USE_EXT_ZF4_LIVE_TAIL_SCHUR="${ext_zf4_live_tail_schur}" \
			      MIMO_USE_EXT_ZF4_TAIL_SCRATCH_REUSE="${ext_zf4_tail_scratch_reuse}" \
			      MIMO_USE_EXT_ZF4_INLINE_HEAD_WORK_REUSE="${ext_zf4_inline_head_work_reuse}" \
			      MIMO_USE_EXT_ZF4_TOP2_MACC_SUM="${ext_zf4_top2_macc_sum}" \
			      MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE="${ext_zf4_true16b_rvv_i32_solve}" \
		      MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_BSOLVE="${ext_zf4_true16b_rvv_i32_bsolve}" \
		      MIMO_USE_EXT_ZF4_SCALAR_X23_STORE="${ext_zf4_scalar_x23_store}" \
			      MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SPLIT_SOLVE="${ext_zf4_true16b_rvv_i32_split_solve}" \
			      MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_STORE="${ext_zf4_true16b_rvv_i32_pack32_store}" \
			      MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_LOAD="${ext_zf4_true16b_rvv_i32_pack32_load}" \
			      MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PRESERVE_PRELUDE="${ext_zf4_true16b_rvv_i32_preserve_prelude}" \
		      MIMO_EXT_ZF4_FAST_CORR_ITERS="${ext_zf4_fast_corr_iters}" \
	      MIMO_EXT_ZF4_FAST_SCALE_BIAS_Q30="${ext_zf4_fast_scale_bias_q30}" \
	      MIMO_EXT_ZF4_FAST_REUSE_SCALE="${ext_zf4_fast_reuse_scale}" \
	      MIMO_USE_EXT_ZF4_UPPER_RVV_SOLVE="${ext_zf4_upper_rvv_solve}" \
	      MIMO_USE_EXT_ZF4_MIXED_UPDATE_SOLVE="${ext_zf4_mixed_update_solve}" \
	      MIMO_DEBUG_EXT_ZF4_RVV_NODIV_COMPARE="${debug_ext_zf4_rvv_nodiv_compare}" \
	      MIMO_USE_SITE4_SOLVE_SOA="${site4_solve_soa}" \
	      MIMO_USE_SITE4_SOLVE_SOA_UNROLL="${site4_solve_soa_unroll}" \
	      MIMO_USE_SITE4_DIAG_REAL_BUILD="${site4_diag_real_build}" \
	      MIMO_USE_SITE4_HERMITIAN_UPPER_BUILD="${site4_hermitian_upper_build}" \
	      MIMO_USE_SITE4_HERMITIAN_UPPER_MMSE="${site4_hermitian_upper_mmse}" \
	      MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED="${site4_hermitian_upper_packed}" \
	      MIMO_USE_SITE4_HERMITIAN_UPPER_ROWFUSE="${site4_hermitian_upper_rowfuse}" \
	      MIMO_USE_SITE4_HERMITIAN_UPPER_ROWPAIR="${site4_hermitian_upper_rowpair}" \
	      MIMO_USE_SITE4_ROUND_ONCE_DOT="${site4_round_once_dot}" \
	      MIMO_USE_SITE4_PRELUDE_HOIST="${site4_prelude_hoist}" \
	      MIMO_USE_SITE4_BUILD_SOLVE_PIPELINE2="${site4_build_solve_pipeline2}" \
	      MIMO_COLLECT_SOLVE_DEN_STATS="${den_stats}" \
	      MIMO_COLLECT_SOLVE_VALUE_STATS="${value_stats}" \
	      MIMO_COLLECT_SCHUR_STATS="${schur_stats}" \
	      MIMO_COLLECT_PIVOT_SCALE_STATS="${pivot_scale_stats}" \
	      > "${nemu_log%.log}-build.log" 2>&1
    build_status=$?
    if [[ "${build_status}" -eq 0 ]]; then
      timeout "${timeout_sec}" "${ROOT}/NEMU/build/riscv64-nemu-interpreter" \
        -b "${APP}/build/mimo-${backend}.bin" 2>&1 | tee "${nemu_log}"
      nemu_pipe_status=("${PIPESTATUS[@]}")
      nemu_status=${nemu_pipe_status[0]}
    else
      nemu_status=${build_status}
    fi
    set -e
    cp "${nemu_log}" "${am_log}"
    grep -E 'MIMO_VALIDATE_PASS|MIMO_DONE|HIT GOOD TRAP|BAD TRAP' "${nemu_log}" || true
    if [[ "${nemu_status}" -ne 0 ]]; then
      echo "MIMO_GATE_NEMU_FAILED backend=${backend} tag=${tag} status=${nemu_status} log=${nemu_log} am_log=${am_log}" >&2
      exit "${nemu_status}"
    fi
    if ! check_nemu_correctness_log "${backend}" "${tag}" "${nemu_log}" "${am_log}"; then
      exit 1
    fi
    echo "MIMO_GATE_NEMU_DONE backend=${backend} tag=${tag} log=${nemu_log} am_log=${am_log}"
  fi

  if [[ "${MIMO_GATE_SKIP_ASM:-0}" != "1" ]]; then
    asm_out="${APP}/perf_eval/asm/mimo-${backend}-riscv64-xs-${tag}.txt"

    echo "MIMO_GATE_ASM_BUILD_START backend=${backend} tag=${tag} kernel=${kernel}"
    make ARCH=riscv64-xs MIMO_IMPL="${backend}" clean >/dev/null
    make ARCH=riscv64-xs \
      MIMO_IMPL="${backend}" \
      MIMO_PERF=1 \
      MIMO_VALIDATE=0 \
      MIMO_VALIDATE_TOL="${validate_tol}" \
      MIMO_VALIDATE_ERROR_STATS="${validate_error_stats}" \
      MIMO_VALIDATE_FAIL_PRINT_LIMIT="${validate_fail_print_limit}" \
      MIMO_PERF_KERNEL="${kernel}" \
      MIMO_WARMUP_REPEAT="${warmup}" \
	      MIMO_REPEAT="${repeat}" \
	      MIMO_USE_RECIP_TABLE="${recip_table}" \
	      MIMO_USE_SOLVE4_UNROLL_V2A="${solve4_unroll}" \
	      MIMO_USE_SOLVE4_HERMITIAN="${solve4_hermitian}" \
	      MIMO_USE_SOLVE4_HERMITIAN_LANE_OUTER="${solve4_hermitian_lane_outer}" \
	      MIMO_USE_SOLVE4_HERMITIAN_SOA_STAGE_OUTER="${solve4_hermitian_soa_stage_outer}" \
	      MIMO_USE_SOLVE4_HERMITIAN_FACTOR_PRECOMPUTE="${solve4_hermitian_factor_precompute}" \
	      MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE="${solve4_hermitian_pivot_scale}" \
	      MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE_HARDDIV="${solve4_hermitian_pivot_scale_harddiv}" \
	      MIMO_USE_SOLVE4_HERMITIAN_HAND_SCHED="${solve4_hermitian_hand_sched}" \
	      MIMO_USE_SOLVE4_HERMITIAN_FACTOR_CALL="${solve4_hermitian_factor_call}" \
	      MIMO_USE_SOLVE4_HERMITIAN_FACTOR_Q15="${solve4_hermitian_factor_q15}" \
	      MIMO_USE_SOLVE4_HERMITIAN_Q15_CALL="${solve4_hermitian_q15_call}" \
	      MIMO_USE_SOLVE4_HERMITIAN_Q15_ONECORR="${solve4_hermitian_q15_onecorr}" \
	      MIMO_SOLVE4_HERMITIAN_Q15_JUMP_ONLY_MASK="${solve4_hermitian_q15_jump_only_mask}" \
	      MIMO_USE_SOLVE4_HERMITIAN_GS="${solve4_hermitian_gs}" \
	      MIMO_SOLVE4_HERMITIAN_GS_ITERS="${solve4_hermitian_gs_iters}" \
	      MIMO_USE_SOLVE4_HERMITIAN_Q30_FIVECORR="${solve4_hermitian_q30_fivecorr}" \
	      MIMO_USE_SOLVE4_HERMITIAN_Q30_TWOJUMP_CORR="${solve4_hermitian_q30_twojump_corr}" \
	      MIMO_USE_SOLVE4_HERMITIAN_Q30_JUMP_ONLY="${solve4_hermitian_q30_jump_only}" \
	      MIMO_USE_SOLVE4_HERMITIAN_Q30_ONECORR="${solve4_hermitian_q30_onecorr}" \
	      MIMO_USE_SOLVE4_HERMITIAN_Q30_PAIR_CORR="${solve4_hermitian_q30_pair_corr}" \
	      MIMO_SOLVE4_HERMITIAN_Q30_JUMP_ONLY_MASK="${solve4_hermitian_q30_jump_only_mask}" \
	      MIMO_USE_SOLVE4_HERMITIAN_POS_PIVOT_SCALE="${solve4_hermitian_pos_pivot_scale}" \
	      MIMO_USE_SOLVE4_HERMITIAN_TRUST_PIVOT_RANGE="${solve4_hermitian_trust_pivot_range}" \
	      MIMO_USE_SOLVE4_HERMITIAN_TRUST_PIVOT_NONZERO="${solve4_hermitian_trust_pivot_nonzero}" \
	      MIMO_USE_SOLVE4_HERMITIAN_FULL_RECIP_TABLE="${solve4_hermitian_full_recip_table}" \
	      MIMO_USE_SOLVE4_HERMITIAN_FLOOR_FULL_RECIP_TABLE="${solve4_hermitian_floor_full_recip_table}" \
	      MIMO_USE_SOLVE4_HERMITIAN_RECIP_STEP16="${solve4_hermitian_recip_step16}" \
	      MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE="${solve4_hermitian_diag_real_update}" \
	      MIMO_USE_SOLVE4_HERMITIAN_DIAG_APPROX="${solve4_hermitian_diag_approx}" \
	      MIMO_USE_SOLVE_BRANCHLIGHT_ROUND="${solve_branchlight_round}" \
	      MIMO_USE_SOLVE_Q15_I64_ARSHIFT_ROUND="${solve_q15_i64_arshift_round}" \
	      MIMO_USE_SOLVE_STORE_TRUST_I16="${solve_store_trust_i16}" \
	      MIMO_USE_SCALAR4_UPPER_ONLY_BUILD="${scalar4_upper_only_build}" \
	      MIMO_USE_SCALAR4_COMPLEX_MUL_3M="${scalar4_complex_mul_3m}" \
	      MIMO_USE_SCALAR4_SCHUR_TOP_Q30_ONCE="${scalar4_schur_top_q30_once}" \
	      MIMO_USE_SCALAR_MMSE4_FUSED_BUILD_SOLVE="${scalar_mmse4_fused_build_solve}" \
	      MIMO_USE_SCALAR_MMSE4_FUSED_COMPACT_C32="${scalar_mmse4_fused_compact_c32}" \
	      MIMO_USE_SCALAR_ZF4_Q15ACC_BUILD="${scalar_zf4_q15acc_build}" \
	      MIMO_USE_SOLVE4_LDLH="${solve4_ldlh}" \
	      MIMO_USE_SOLVE4_SCHUR="${solve4_schur}" \
	      MIMO_USE_SOLVE4_SCHUR_PAIR2="${solve4_schur_pair2}" \
	      MIMO_USE_SOLVE4_SCHUR_DET_SCALE="${solve4_schur_det_scale}" \
	      MIMO_USE_SOLVE4_SCHUR_BINV_Q15="${solve4_schur_binv_q15}" \
	      MIMO_USE_SOLVE4_SCHUR_Q15_RECIP="${solve4_schur_q15_recip}" \
	      MIMO_USE_SOLVE4_SCHUR_FUSED_INNER="${solve4_schur_fused_inner}" \
	      MIMO_USE_SOLVE4_SCHUR_FUSED_INNER_MMSE="${solve4_schur_fused_inner_mmse}" \
	      MIMO_USE_SOLVE4_SCHUR_MMSE_SPECIALIZE="${solve4_schur_mmse_specialize}" \
	      MIMO_USE_SOLVE4_SCHUR_ZF_EXPLICIT="${solve4_schur_zf_explicit}" \
	      MIMO_USE_SOLVE4_SCHUR_ZF_RELOAD_MICRO="${solve4_schur_zf_reload_micro}" \
	      MIMO_USE_SOLVE4_SCHUR_ZF_TOP_RESOLVE="${solve4_schur_zf_top_resolve}" \
	      MIMO_USE_SOLVE4_SCHUR_ZF_HANDASM="${solve4_schur_zf_handasm}" \
		      MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER="${solve4_schur_zf_ext_inner}" \
		      MIMO_USE_SOLVE4_SCHUR_ZF_I32_NUM="${solve4_schur_zf_i32_num}" \
		      MIMO_USE_SOLVE4_SCHUR_ZF_EXT_MACC_INNER="${solve4_schur_zf_ext_macc_inner}" \
		      MIMO_USE_SOLVE4_SCHUR_ZF_Q15_NUM_SOLVE="${solve4_schur_zf_q15_num_solve}" \
		      MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_ONLY="${solve4_schur_zf_num_arshift_only}" \
		      MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_MASK="${solve4_schur_zf_num_arshift_mask}" \
			      MIMO_USE_SOLVE4_SCHUR_ZF_INLINE_HELPER="${solve4_schur_zf_inline_helper}" \
		      MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_MODEL="${solve4_schur_zf_solve2h_model}" \
		      MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_MODEL="${solve4_schur_zf_vsolve2h2_model}" \
		      MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_ASM="${solve4_schur_zf_vsolve2h2_asm}" \
		      MIMO_USE_SOLVE4_SCHUR_ZF_PIVOTSCALE_MODEL="${solve4_schur_zf_pivotscale_model}" \
		      MIMO_CHECK_SOLVE2H_GROUP_LAYOUT="${check_solve2h_group_layout}" \
		      MIMO_CHECK_VSOLVE2H2_LAYOUT="${check_vsolve2h2_layout}" \
	      MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT="${solve4_schur_q15_recip_num_shift}" \
	      MIMO_USE_EQ4_DIRECT_H="${eq4_direct_h}" \
	      MIMO_EQ4_DIRECT_H_MODE="${eq4_direct_h_mode}" \
	      MIMO_EQ4_DIRECT_H_ITERS="${eq4_direct_h_iters}" \
	      MIMO_EQ4_DIRECT_H_NODIV="${eq4_direct_h_nodiv}" \
	      MIMO_USE_SOLVE_SKIP_LOWER_UPDATE="${solve_skip_lower_update}" \
	      MIMO_USE_RVV_SOLVE4="${rvv_solve4}" \
	      MIMO_USE_RVV_SOLVE4_STREAMED="${rvv_solve4_streamed}" \
	      MIMO_USE_RVV_SOLVE4_VNCLIP_ROUND="${rvv_solve4_vnclip}" \
	      MIMO_USE_RVV_SOLVE4_Q15_I32_MUL="${rvv_solve4_q15_i32_mul}" \
	      MIMO_USE_RVV_ZF4_SCHUR_RVV_SOLVE="${rvv_zf4_schur_rvv_solve}" \
	      MIMO_USE_RVV_MMSE4_SCHUR_RVV_SOLVE="${rvv_mmse4_schur_rvv_solve}" \
	      MIMO_USE_RVV_ZF4_FUSED_BUILD_SOLVE="${rvv_zf4_fused_build_solve}" \
	      MIMO_USE_RVV_ZF4_STAGED_BUILD_SOLVE="${rvv_zf4_staged_build_solve}" \
	      MIMO_USE_RVV_ZF4_SEED_ACC="${rvv_zf4_seed_acc}" \
	      MIMO_USE_RVV_ZF4_Q15ACC_BUILD="${rvv_zf4_q15acc_build}" \
	      MIMO_USE_RVV_ZF4_PACK32_STORE="${rvv_zf4_pack32_store}" \
	      MIMO_USE_RVV_SITE4_MF2="${rvv_site4_mf2}" \
	      MIMO_USE_EQ2_ZF_DIRECT_H="${eq2_zf_direct_h}" \
	      MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP="${eq2_zf_guard1_fine_recip}" \
	      MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID="${ext_eq2_zf_guard1_hybrid}" \
	      MIMO_USE_EQ2_DIRECT_Q13_INV="${eq2_direct_q13_inv}" \
	      MIMO_USE_RVV_EQ2_M1="${rvv_eq2_m1}" \
	      MIMO_USE_RVV_EQ2_ALWAYS_ADD_LAMBDA="${rvv_eq2_always_add_lambda}" \
	      MIMO_USE_RVV_EQ2_SPLIT_BUILD="${rvv_eq2_split_build}" \
	      MIMO_USE_RVV_EQ2_FUSED_NCLIP_BUILD="${rvv_eq2_fused_nclip_build}" \
	      MIMO_USE_RVV_EQ2_TX_SOA_STORE="${rvv_eq2_tx_soa_store}" \
	      MIMO_USE_RVV_EQ2_3M_BUILD="${rvv_eq2_3m_build}" \
	      MIMO_USE_RVV_EQ2_ZEXT_PACK="${rvv_eq2_zext_pack}" \
	      MIMO_USE_RVV_EQ2_RI_SOA_OUTPUT="${rvv_eq2_ri_soa_output}" \
	      MIMO_USE_RVV_EQ2_STAGED_NARROW_BUILD="${rvv_eq2_staged_narrow_build}" \
	      MIMO_USE_RVV_EQ2_IMPLICIT_VXRM="${rvv_eq2_implicit_vxrm}" \
	      MIMO_USE_RVV_EQ2_UQ12_SCALE="${rvv_eq2_uq12_scale}" \
	      MIMO_USE_RVV_EQ2_EARLY_RECIP_OVERLAP="${rvv_eq2_early_recip_overlap}" \
	      MIMO_USE_RVV_EQ2_BLOCK_SOA="${rvv_eq2_block_soa}" \
	      MIMO_USE_RVV_EQ2_GROUP_LOAD="${rvv_eq2_group_load}" \
	      MIMO_USE_RVV_EQ2_GROUP_LOAD_M4="${rvv_eq2_group_load_m4}" \
	      MIMO_USE_EXT_EQ2_INVCOEFF="${ext_eq2_invcoeff}" \
	      MIMO_USE_EXT_EQ2_M2_BATCH="${ext_eq2_m2_batch}" \
	      MIMO_USE_EXT_EQ2_RHS_INTERLEAVE="${ext_eq2_rhs_interleave}" \
	      MIMO_USE_MMSE4_SCHUR_NO_MAG_SCALE="${mmse4_schur_no_mag_scale}" \
	      MIMO_USE_MMSE4_SCHUR_SIGNED_TRUNC_SCALE="${mmse4_schur_signed_trunc_scale}" \
	      MIMO_USE_MMSE4_SCHUR_NUM_ARSHIFT_SCALE="${mmse4_schur_num_arshift_scale}" \
	      MIMO_USE_MMSE4_SCHUR_INNER_Q30_ONCE="${mmse4_schur_inner_q30_once}" \
	      MIMO_USE_RVV_MMSE4_SCHUR_INNER_SYM_ROUND="${rvv_mmse4_schur_inner_sym_round}" \
	      MIMO_USE_RVV_MMSE4_SCHUR_TOP_Q30_ONCE="${rvv_mmse4_schur_top_q30_once}" \
	      MIMO_USE_RVV_MMSE4_STAGED_BUILD_SOLVE="${rvv_mmse4_staged_build_solve}" \
	      MIMO_USE_RVV_MMSE4_EARLY_TAIL_RHS="${rvv_mmse4_early_tail_rhs}" \
	      MIMO_USE_RVV_MMSE4_CONST_VL="${rvv_mmse4_const_vl}" \
	      MIMO_USE_RVV_MMSE4_SEED_ACC="${rvv_mmse4_seed_acc}" \
	      MIMO_USE_RVV_MMSE4_SCHUR_TRUE16B_SOLVE="${rvv_mmse4_schur_true16b_solve}" \
	      MIMO_USE_RVV_MMSE4_SCHUR_TRUE16B_SHARED_A16="${rvv_mmse4_schur_true16b_shared_a16}" \
	      MIMO_USE_RVV_SCHUR4_SOLVE2_PAIR_M2="${rvv_schur4_solve2_pair_m2}" \
	      MIMO_USE_RVV_SCHUR4_VSMUL_SCALE="${rvv_schur4_vsmul_scale}" \
	      MIMO_USE_RVV_SCHUR4_NUM_MAC="${rvv_schur4_num_mac}" \
	      MIMO_USE_RVV_SCHUR4_COMPLEX_MAC="${rvv_schur4_complex_mac}" \
		      MIMO_USE_RVV_SCHUR4_VSSRA_ROUND="${rvv_schur4_vssra_round}" \
		      MIMO_USE_RVV_SITE4_VSSRA_ROUND="${rvv_site4_vssra_round}" \
		      MIMO_USE_RVV_SITE4_SEG2_LOAD="${rvv_site4_seg2_load}" \
		      MIMO_USE_RVV_SITE4_RI_SOA="${rvv_site4_ri_soa}" \
		      MIMO_USE_RVV_SCHUR4_DET_MAC="${rvv_schur4_det_mac}" \
	      MIMO_USE_EXT_MMSE4_SCHUR_FINAL_SIGNED_TRUNC_SCALE="${ext_mmse4_schur_final_signed_trunc_scale}" \
	      MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK="${ext_mmse4_schur_signed_trunc_mask}" \
	      MIMO_USE_EXT_MMSE4_RVV_I32_VEC_LAMBDA="${ext_mmse4_rvv_i32_vec_lambda}" \
	      MIMO_USE_EXT_MMSE4_RVV_I32_LATE_TAIL_LOAD="${ext_mmse4_rvv_i32_late_tail_load}" \
	      MIMO_USE_EXT_ZF4_LOCAL_SOLVE="${ext_zf4_local_solve}" \
		      MIMO_USE_EXT_ZF4_RVV_NODIV_SOLVE="${ext_zf4_rvv_nodiv_solve}" \
		      MIMO_USE_EXT_ZF4_RVV_NODIV_FAST_SOLVE="${ext_zf4_rvv_nodiv_fast_solve}" \
		      MIMO_USE_EXT_ZF4_SCHUR_RVV_SOLVE="${ext_zf4_schur_rvv_solve}" \
		      MIMO_USE_EXT_MMSE4_SCHUR_RVV_SOLVE="${ext_mmse4_schur_rvv_solve}" \
		      MIMO_USE_EXT_MMSE4_FUSED_BUILD_SOLVE="${ext_mmse4_fused_build_solve}" \
		      MIMO_USE_EXT_MMSE4_NARROW_WORK="${ext_mmse4_narrow_work}" \
			      MIMO_USE_EXT_MMSE4_LIVE_BUILD="${ext_mmse4_live_build}" \
			      MIMO_USE_EXT_MMSE4_STREAM_WORK_SOLVE="${ext_mmse4_stream_work_solve}" \
			      MIMO_USE_EXT_MMSE4_TRUE16B_INLINE_SOLVE="${ext_mmse4_true16b_inline_solve}" \
			      MIMO_USE_EXT_MMSE4_TRUE16B_LIVE_TAIL_SOLVE="${ext_mmse4_true16b_live_tail_solve}" \
			      MIMO_USE_EXT_ZF4_RVV_ROUND_ONCE_BUILD="${ext_zf4_rvv_round_once_build}" \
	      MIMO_USE_EXT_ZF4_TRUE16B_WORK="${ext_zf4_true16b_work}" \
	      MIMO_USE_EXT_ZF4_PACK32_WORK="${ext_zf4_pack32_work}" \
	      MIMO_USE_EXT_ZF4_VSCMACC_BUILD="${ext_zf4_vscmacc_build}" \
		      MIMO_USE_EXT_ZF4_VSCMACC_FUSED_BUILD="${ext_zf4_vscmacc_fused_build}" \
		      MIMO_USE_EXT_ZF4_INLINE_UPPER_I16_SOLVE="${ext_zf4_inline_upper_i16_solve}" \
			      MIMO_USE_EXT_ZF4_LIVE_TAIL_SCHUR="${ext_zf4_live_tail_schur}" \
			      MIMO_USE_EXT_ZF4_TAIL_SCRATCH_REUSE="${ext_zf4_tail_scratch_reuse}" \
			      MIMO_USE_EXT_ZF4_INLINE_HEAD_WORK_REUSE="${ext_zf4_inline_head_work_reuse}" \
			      MIMO_USE_EXT_ZF4_TOP2_MACC_SUM="${ext_zf4_top2_macc_sum}" \
			      MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE="${ext_zf4_true16b_rvv_i32_solve}" \
	      MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_BSOLVE="${ext_zf4_true16b_rvv_i32_bsolve}" \
		      MIMO_USE_EXT_ZF4_SCALAR_X23_STORE="${ext_zf4_scalar_x23_store}" \
			      MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SPLIT_SOLVE="${ext_zf4_true16b_rvv_i32_split_solve}" \
			      MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_STORE="${ext_zf4_true16b_rvv_i32_pack32_store}" \
			      MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_LOAD="${ext_zf4_true16b_rvv_i32_pack32_load}" \
			      MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PRESERVE_PRELUDE="${ext_zf4_true16b_rvv_i32_preserve_prelude}" \
		      MIMO_EXT_ZF4_FAST_CORR_ITERS="${ext_zf4_fast_corr_iters}" \
	      MIMO_EXT_ZF4_FAST_SCALE_BIAS_Q30="${ext_zf4_fast_scale_bias_q30}" \
	      MIMO_EXT_ZF4_FAST_REUSE_SCALE="${ext_zf4_fast_reuse_scale}" \
	      MIMO_USE_EXT_ZF4_UPPER_RVV_SOLVE="${ext_zf4_upper_rvv_solve}" \
	      MIMO_USE_EXT_ZF4_MIXED_UPDATE_SOLVE="${ext_zf4_mixed_update_solve}" \
	      MIMO_DEBUG_EXT_ZF4_RVV_NODIV_COMPARE="${debug_ext_zf4_rvv_nodiv_compare}" \
	      MIMO_USE_SITE4_SOLVE_SOA="${site4_solve_soa}" \
	      MIMO_USE_SITE4_SOLVE_SOA_UNROLL="${site4_solve_soa_unroll}" \
	      MIMO_USE_SITE4_DIAG_REAL_BUILD="${site4_diag_real_build}" \
	      MIMO_USE_SITE4_HERMITIAN_UPPER_BUILD="${site4_hermitian_upper_build}" \
	      MIMO_USE_SITE4_HERMITIAN_UPPER_MMSE="${site4_hermitian_upper_mmse}" \
	      MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED="${site4_hermitian_upper_packed}" \
	      MIMO_USE_SITE4_HERMITIAN_UPPER_ROWFUSE="${site4_hermitian_upper_rowfuse}" \
	      MIMO_USE_SITE4_HERMITIAN_UPPER_ROWPAIR="${site4_hermitian_upper_rowpair}" \
	      MIMO_USE_SITE4_ROUND_ONCE_DOT="${site4_round_once_dot}" \
	      MIMO_USE_SITE4_PRELUDE_HOIST="${site4_prelude_hoist}" \
	      MIMO_USE_SITE4_BUILD_SOLVE_PIPELINE2="${site4_build_solve_pipeline2}" \
	      MIMO_COLLECT_SOLVE_DEN_STATS="${den_stats}" \
	      MIMO_COLLECT_SOLVE_VALUE_STATS="${value_stats}" \
	      MIMO_COLLECT_SCHUR_STATS="${schur_stats}" \
	      MIMO_COLLECT_PIVOT_SCALE_STATS="${pivot_scale_stats}"
    cp "${APP}/build/mimo-${backend}.txt" "${asm_out}"
    echo "MIMO_GATE_ASM_BUILD_DONE backend=${backend} tag=${tag} asm=${asm_out}"

    if [[ ( -n "${asm_expect_flag}" || "${asm_div_budget_enabled}" -eq 1 ) &&
          "${asm_expect_backends}" == *" ${backend} "* ]]; then
      asm_symbol=${MIMO_GATE_ASM_SYMBOL:-$(auto_asm_symbol "${backend}")}
      if [[ -z "${asm_symbol}" ]]; then
        echo "MIMO_GATE_ASM_EXPECT_FAILED backend=${backend} tag=${tag}: cannot infer symbol for kernel=${kernel}; set MIMO_GATE_ASM_SYMBOL" >&2
        exit 2
      fi
      if [[ "${MIMO_ASM_FORBID_HARD_DIV:-0}" == "1" ]]; then
        mapfile -t hard_div_symbols < <(collect_hard_div_symbols "${asm_symbol}" "${asm_out}")
      else
        hard_div_symbols=("${asm_symbol}")
      fi

      for check_symbol in "${hard_div_symbols[@]}"; do
        check_args=(--symbol "${check_symbol}")
        if [[ -n "${asm_expect_flag}" && "${check_symbol}" == "${asm_symbol}" ]]; then
          check_args+=("${asm_expect_flag}")
        fi
        check_args+=("${asm_out}")
        echo "MIMO_GATE_ASM_EXPECT_START backend=${backend} tag=${tag} expect=${asm_expect:-none} symbol=${check_symbol}"
	        MIMO_ASM_FORBID_SEGMENT_LDST="${MIMO_ASM_FORBID_SEGMENT_LDST:-1}" \
	          "${APP}/scripts/check_asm_symbol_counts.sh" "${check_args[@]}"
        echo "MIMO_GATE_ASM_EXPECT_DONE backend=${backend} tag=${tag} expect=${asm_expect:-none} symbol=${check_symbol}"
      done
    fi
	  fi
	done

echo "MIMO_GATE_DONE tag=${tag} backends=${backends[*]}"
