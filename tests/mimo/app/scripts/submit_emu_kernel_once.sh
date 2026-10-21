#!/usr/bin/env bash
set -euo pipefail

ROOT="${XS_ENV_ROOT:-$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../../../.." && pwd)}"
EMU_ENV="${ROOT:-${XS_ENV_ROOT:-$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../../../.." && pwd)}}"
APP=${ROOT}/nexus-am/apps/mimo

backend=${1:-scalar}
kernel=${2:-eq_zf_4x4}
timeout_sec=${3:-0}
if [[ "${timeout_sec}" != "0" ]]; then
  echo "Refusing to submit official emu: timeout_sec must be 0; long MIMO simulations must run without a kill timeout." >&2
  exit 2
fi
if [[ "${MIMO_4X4_EQ_ONLY_LOCK:-1}" != "1" ]]; then
  echo "Refusing to submit: the legacy scope lock must remain enabled for the current 2x2/4x4 EQ scope." >&2
  exit 2
fi
if [[ "${MIMO_ZF4_ONLY_LOCK:-1}" != "1" ]]; then
  echo "Refusing to submit: the legacy ZF4 lock must remain enabled for compatibility." >&2
  exit 2
fi
if [[ "${MIMO_ALLOW_8X8+x}" == "x" || "${MIMO_USE_SITE4_8X8_BUILD+x}" == "x" ]]; then
  echo "Refusing to submit: 8x8 has been removed from the current execution scope; do not set 8x8 knobs." >&2
  exit 2
fi
if [[ "${MIMO_ALLOW_2X2+x}" == "x" || "${MIMO_USE_SITE2_2X2_BUILD+x}" == "x" ]]; then
  echo "Refusing to submit: MIMO_ALLOW_2X2 and MIMO_USE_SITE2_2X2_BUILD are obsolete; select an eq_*_2x2 kernel directly." >&2
  exit 2
fi
eq4_only_lock=1
zf4_only_lock=1
warmup=${MIMO_WARMUP_REPEAT:-0}
repeat=${MIMO_REPEAT:-1}
validate_tol=${MIMO_VALIDATE_TOL:-2}
validate_error_stats=${MIMO_VALIDATE_ERROR_STATS:-0}
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
solve4_schur_zf_solve2h_model=${MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_MODEL:-0}
solve4_schur_zf_vsolve2h2_model=${MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_MODEL:-0}
solve4_schur_zf_vsolve2h2_asm=${MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_ASM:-0}
solve4_schur_zf_pivotscale_model=${MIMO_USE_SOLVE4_SCHUR_ZF_PIVOTSCALE_MODEL:-0}
solve4_schur_zf_solve2h_asm_stub=${MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_ASM_STUB:-0}
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
eq2_recip_mode=${MIMO_EQ2_RECIP_MODE:-5}
eq2_recip_num_shift=${MIMO_EQ2_RECIP_NUM_SHIFT:-13}
eq2_experimental_profile=${MIMO_EQ2_EXPERIMENTAL_PROFILE:-0}
eq2_zf_direct_h=${MIMO_USE_EQ2_ZF_DIRECT_H:-0}
eq2_zf_guard1_fine_recip=${MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP:-0}
ext_eq2_zf_guard1_hybrid=${MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID:-0}
eq2_mmse_direct_h=${MIMO_USE_EQ2_MMSE_DIRECT_H:-0}
scalar_eq2_fused_lambda=${MIMO_USE_SCALAR_EQ2_FUSED_LAMBDA:-0}
scalar_eq2_mmse_full_trunc_scale=${MIMO_USE_SCALAR_EQ2_MMSE_FULL_TRUNC_SCALE:-0}
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
rvv_eq2_mmse_full_trunc_scale=${MIMO_USE_RVV_EQ2_MMSE_FULL_TRUNC_SCALE:-0}
rvv_eq2_block_soa=${MIMO_USE_RVV_EQ2_BLOCK_SOA:-0}
rvv_eq2_group_load=${MIMO_USE_RVV_EQ2_GROUP_LOAD:-0}
rvv_eq2_group_load_m4=${MIMO_USE_RVV_EQ2_GROUP_LOAD_M4:-0}
ext_eq2_invcoeff=${MIMO_USE_EXT_EQ2_INVCOEFF:-1}
ext_eq2_m2_batch=${MIMO_USE_EXT_EQ2_M2_BATCH:-0}
ext_eq2_rhs_interleave=${MIMO_USE_EXT_EQ2_RHS_INTERLEAVE:-0}
ext_eq2_mmse_adaptive_q13=${MIMO_USE_EXT_EQ2_MMSE_ADAPTIVE_Q13:-0}
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
rvv_mmse4_ri_packed_load=${MIMO_USE_RVV_MMSE4_RI_PACKED_LOAD:-0}
rvv_mmse4_binv_q13_reuse=${MIMO_USE_RVV_MMSE4_BINV_Q13_REUSE:-0}
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

if [[ "${solve4_schur_zf_handasm}" != "0" && \
      "${MIMO_ALLOW_ZF4_HANDASM_OFFICIAL:-0}" != "1" ]]; then
  echo "Refusing to submit official emu: ZF4 handasm stack/address variants are local-gate only after NEMU rejection; set MIMO_ALLOW_ZF4_HANDASM_OFFICIAL=1 only for a new P0 dataflow with documented local evidence." >&2
  exit 2
fi

if [[ "${rvv_site4_seg2_load}" != "0" ]]; then
  echo "Refusing to submit official emu: RVV segment load/store is disabled on this XiangShan configuration after the SEG2 official result regressed cycles by 3.39x." >&2
  exit 2
fi
ext_zf4_rvv_round_once_build=${MIMO_USE_EXT_ZF4_RVV_ROUND_ONCE_BUILD:-0}
ext_zf4_true16b_work=${MIMO_USE_EXT_ZF4_TRUE16B_WORK:-0}
ext_zf4_pack32_work=${MIMO_USE_EXT_ZF4_PACK32_WORK:-0}
ext_zf4_vscmacc_build=${MIMO_USE_EXT_ZF4_VSCMACC_BUILD:-0}
ext_zf4_vscmacc_fused_build=${MIMO_USE_EXT_ZF4_VSCMACC_FUSED_BUILD:-0}
ext_zf4_vscmacc_seed_build=${MIMO_USE_EXT_ZF4_VSCMACC_SEED_BUILD:-0}
ext_zf4_packed_live_build=${MIMO_USE_EXT_ZF4_PACKED_LIVE_BUILD:-0}
ext_zf4_inline_upper_i16_solve=${MIMO_USE_EXT_ZF4_INLINE_UPPER_I16_SOLVE:-0}
ext_zf4_live_tail_schur=${MIMO_USE_EXT_ZF4_LIVE_TAIL_SCHUR:-0}
ext_zf4_tail_scratch_reuse=${MIMO_USE_EXT_ZF4_TAIL_SCRATCH_REUSE:-0}
ext_zf4_inline_head_work_reuse=${MIMO_USE_EXT_ZF4_INLINE_HEAD_WORK_REUSE:-0}
ext_zf4_top2_macc_sum=${MIMO_USE_EXT_ZF4_TOP2_MACC_SUM:-0}
solve4_schur_zf_ext_macc_inner=${MIMO_USE_SOLVE4_SCHUR_ZF_EXT_MACC_INNER:-0}
solve4_schur_zf_num_arshift_mask=${MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_MASK:-0}
ext_zf4_true16b_rvv_i32_solve=${MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE:-0}
ext_zf4_true16b_rvv_i32_bsolve=${MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_BSOLVE:-0}
ext_zf4_true16b_rvv_i32_split_solve=${MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SPLIT_SOLVE:-0}
ext_zf4_true16b_rvv_i32_pack32_store=${MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_STORE:-0}
ext_zf4_true16b_rvv_i32_pack32_load=${MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_LOAD:-0}
ext_mmse4_rvv_i32_late_tail_load=${MIMO_USE_EXT_MMSE4_RVV_I32_LATE_TAIL_LOAD:-0}
ext_mmse4_rvv_i32_specialize=${MIMO_USE_EXT_MMSE4_RVV_I32_SPECIALIZE:-0}
ext_mmse4_rvv_i32_inner_q30_once=${MIMO_USE_EXT_MMSE4_RVV_I32_INNER_Q30_ONCE:-0}
ext_mmse4_rvv_i32_top_q30_once=${MIMO_USE_EXT_MMSE4_RVV_I32_TOP_Q30_ONCE:-0}
ext_mmse4_rvv_i32_assume_lambda_nosat=${MIMO_USE_EXT_MMSE4_RVV_I32_ASSUME_LAMBDA_NOSAT:-0}
ext_mmse4_rvv_i32_binv_reuse=${MIMO_USE_EXT_MMSE4_RVV_I32_BINV_REUSE:-0}
ext_mmse4_rvv_i32_packed_phase=${MIMO_USE_EXT_MMSE4_RVV_I32_PACKED_PHASE:-0}
ext_mmse4_packed_top_ext=${MIMO_EXT_MMSE4_PACKED_TOP_EXT:-1}
ext_mmse4_packed_ssolve_ext=${MIMO_EXT_MMSE4_PACKED_SSOLVE_EXT:-0}
ext_mmse4_packed_live_build=${MIMO_USE_EXT_MMSE4_PACKED_LIVE_BUILD:-0}
ext_mmse4_packed_live_build_unroll4=${MIMO_USE_EXT_MMSE4_PACKED_LIVE_BUILD_UNROLL4:-0}
ext_mmse4_packed_invcoeff_ext=${MIMO_EXT_MMSE4_PACKED_INVCOEFF_EXT:-0}
ext_mmse4_packed_vset_chain=${MIMO_USE_EXT_MMSE4_PACKED_VSET_CHAIN:-0}
ext_mmse4_packed_solve_seed=${MIMO_EXT_MMSE4_PACKED_SOLVE_SEED:-0}
ext_mmse4_packed_det_ext=${MIMO_EXT_MMSE4_PACKED_DET_EXT:-0}
ext_mmse4_packed_fixed_ptrs=${MIMO_USE_EXT_MMSE4_PACKED_FIXED_PTRS:-0}
ext_mmse4_packed_e16_recip=${MIMO_USE_EXT_MMSE4_PACKED_E16_RECIP:-0}
ext_mmse4_packed_e16_lambda=${MIMO_USE_EXT_MMSE4_PACKED_E16_LAMBDA:-0}
ext_mmse4_packed_rx4_m4_load=${MIMO_USE_EXT_MMSE4_PACKED_RX4_M4_LOAD:-0}
ext_mmse4_packed_s_recip_mode=${MIMO_EXT_MMSE4_PACKED_S_RECIP_MODE:-0}
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
require_no_hard_div=${MIMO_EMU_REQUIRE_NO_HARD_DIV:-1}
diagnostic_allow_hard_div=${MIMO_EMU_DIAGNOSTIC_ALLOW_HARD_DIV:-0}
diagnostic_allow_pivot_scale=${MIMO_EMU_DIAGNOSTIC_ALLOW_PIVOT_SCALE:-0}

case "${kernel}" in
  eq_zf_2x2|eq_mmse_2x2|eq_zf_4x4|eq_mmse_4x4) ;;
  *)
    echo "Refusing to submit: current scope permits only 2x2/4x4 ZF/MMSE kernels." >&2
    exit 2
    ;;
esac

if [[ "${eq2_zf_direct_h}" != "0" && "${eq2_zf_direct_h}" != "1" ]]; then
  echo "Refusing to submit: MIMO_USE_EQ2_ZF_DIRECT_H must be 0 or 1." >&2
  exit 2
fi
if [[ "${eq2_zf_direct_h}" == "1" && "${kernel}" != "eq_zf_2x2" ]]; then
  echo "Refusing to submit: EQ2 direct-H is valid only for eq_zf_2x2." >&2
  exit 2
fi
if [[ "${eq2_zf_guard1_fine_recip}" != "0" && "${eq2_zf_guard1_fine_recip}" != "1" ]]; then
  echo "Refusing to submit: MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP must be 0 or 1." >&2
  exit 2
fi
if [[ "${eq2_zf_guard1_fine_recip}" == "1" &&
      ( "${kernel}" != "eq_zf_2x2" || "${eq2_zf_direct_h}" != "1" ||
        "${backend}" == "ext" ) ]]; then
  echo "Refusing to submit: guard1 fine reciprocal requires scalar/RVV ZF2 direct-H." >&2
  exit 2
fi
if [[ "${eq2_zf_guard1_fine_recip}" == "1" && "${backend}" == "rvv" &&
      ( "${rvv_eq2_m1}" != "1" || "${rvv_eq2_implicit_vxrm}" != "1" ||
        "${rvv_eq2_uq12_scale}" != "0" ) ]]; then
  echo "Refusing to submit: RVV guard1 requires wide-m1, implicit RNU, and Q30 scale." >&2
  exit 2
fi
if [[ "${ext_eq2_zf_guard1_hybrid}" != "0" &&
      "${ext_eq2_zf_guard1_hybrid}" != "1" ]]; then
  echo "Refusing to submit: MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID must be 0 or 1." >&2
  exit 2
fi
if [[ "${ext_eq2_zf_guard1_hybrid}" == "1" &&
      ( "${backend}" != "ext" || "${kernel}" != "eq_zf_2x2" ||
        "${eq2_zf_direct_h}" != "1" || "${eq2_zf_guard1_fine_recip}" != "0" ||
        "${ext_eq2_invcoeff}" != "0" || "${ext_eq2_m2_batch}" != "1" ||
        "${ext_eq2_rhs_interleave}" != "0" ) ]]; then
  echo "Refusing to submit: EXT ZF2 guard1 hybrid requires EXT direct-H m2 with no scalar/RVV guard1, invcoeff, or RHS interleave." >&2
  exit 2
fi
if [[ "${ext_eq2_mmse_adaptive_q13}" != "0" &&
      "${ext_eq2_mmse_adaptive_q13}" != "1" ]]; then
  echo "Refusing to submit: MIMO_USE_EXT_EQ2_MMSE_ADAPTIVE_Q13 must be 0 or 1." >&2
  exit 2
fi
if [[ "${ext_eq2_mmse_adaptive_q13}" == "1" &&
      ( "${backend}" != "ext" || "${kernel}" != "eq_mmse_2x2" ||
        "${ext_eq2_invcoeff}" != "1" ||
        "${ext_eq2_rhs_interleave}" != "0" ||
        "${solve_store_trust_i16}" != "0" ) ]]; then
  echo "Refusing to submit: EXT adaptive Q13 requires MMSE2 invcoeff and saturated output." >&2
  exit 2
fi
if [[ "${eq2_mmse_direct_h}" != "0" && "${eq2_mmse_direct_h}" != "1" ]]; then
  echo "Refusing to submit: MIMO_USE_EQ2_MMSE_DIRECT_H must be 0 or 1." >&2
  exit 2
fi
if [[ "${eq2_mmse_direct_h}" == "1" && "${kernel}" != "eq_mmse_2x2" ]]; then
  echo "Refusing to submit: EQ2 MMSE direct-H is valid only for eq_mmse_2x2." >&2
  exit 2
fi
if [[ "${eq2_experimental_profile}" != "0" &&
      "${eq2_experimental_profile}" != "1" ]]; then
  echo "Refusing to submit: MIMO_EQ2_EXPERIMENTAL_PROFILE must be 0 or 1." >&2
  exit 2
fi
if [[ "${eq2_mmse_direct_h}" == "1" && "${backend}" == "ext" &&
      ( "${ext_eq2_invcoeff}" != "1" || "${ext_eq2_m2_batch}" != "1" ||
        "${ext_eq2_rhs_interleave}" != "0" ||
        "${ext_eq2_mmse_adaptive_q13}" != "1" ||
        "${solve_store_trust_i16}" != "0" ) ]]; then
  echo "Refusing to submit: EXT MMSE2 direct-H requires m2 adaptive-Q13 no-harddiv fallback." >&2
  exit 2
fi
if [[ "${scalar_eq2_fused_lambda}" != "0" && "${scalar_eq2_fused_lambda}" != "1" ]]; then
  echo "Refusing to submit: MIMO_USE_SCALAR_EQ2_FUSED_LAMBDA must be 0 or 1." >&2
  exit 2
fi
if [[ "${scalar_eq2_fused_lambda}" == "1" &&
      ( "${backend}" != "scalar" || "${kernel}" != "eq_mmse_2x2" ) ]]; then
  echo "Refusing to submit: scalar EQ2 fused lambda is valid only for scalar eq_mmse_2x2." >&2
  exit 2
fi
if [[ "${scalar_eq2_mmse_full_trunc_scale}" != "0" &&
      "${scalar_eq2_mmse_full_trunc_scale}" != "1" ]]; then
  echo "Refusing to submit: MIMO_USE_SCALAR_EQ2_MMSE_FULL_TRUNC_SCALE must be 0 or 1." >&2
  exit 2
fi
if [[ "${scalar_eq2_mmse_full_trunc_scale}" == "1" &&
      ( "${backend}" != "scalar" || "${kernel}" != "eq_mmse_2x2" ||
        "${scalar_eq2_fused_lambda}" != "1" ) ]]; then
  echo "Refusing to submit: scalar full trunc scale requires scalar MMSE2 with fused lambda." >&2
  exit 2
fi
if [[ "${rvv_eq2_implicit_vxrm}" != "0" && "${rvv_eq2_implicit_vxrm}" != "1" ]]; then
  echo "Refusing to submit: MIMO_USE_RVV_EQ2_IMPLICIT_VXRM must be 0 or 1." >&2
  exit 2
fi
if [[ "${rvv_eq2_uq12_scale}" != "0" && "${rvv_eq2_uq12_scale}" != "1" ]]; then
  echo "Refusing to submit: MIMO_USE_RVV_EQ2_UQ12_SCALE must be 0 or 1." >&2
  exit 2
fi
if [[ "${rvv_eq2_early_recip_overlap}" != "0" && "${rvv_eq2_early_recip_overlap}" != "1" ]]; then
  echo "Refusing to submit: MIMO_USE_RVV_EQ2_EARLY_RECIP_OVERLAP must be 0 or 1." >&2
  exit 2
fi
if [[ "${rvv_eq2_early_recip_overlap}" == "1" &&
      ( "${backend}" != "rvv" || "${kernel}" != "eq_mmse_2x2" ||
        "${rvv_eq2_m1}" != "1" ) ]]; then
  echo "Refusing to submit: early reciprocal overlap requires MMSE2 RVV m1." >&2
  exit 2
fi
if [[ "${rvv_eq2_early_recip_overlap}" == "1" && "${rvv_eq2_uq12_scale}" == "1" ]]; then
  echo "Refusing to submit: early reciprocal overlap and UQ12 scale are mutually exclusive." >&2
  exit 2
fi
if [[ "${rvv_eq2_mmse_full_trunc_scale}" != "0" &&
      "${rvv_eq2_mmse_full_trunc_scale}" != "1" ]]; then
  echo "Refusing to submit: MIMO_USE_RVV_EQ2_MMSE_FULL_TRUNC_SCALE must be 0 or 1." >&2
  exit 2
fi
if [[ "${rvv_eq2_mmse_full_trunc_scale}" == "1" &&
      ( "${backend}" != "rvv" || "${kernel}" != "eq_mmse_2x2" ||
        "${rvv_eq2_m1}" != "1" || "${rvv_eq2_uq12_scale}" != "0" ) ]]; then
  echo "Refusing to submit: RVV full trunc scale requires MMSE2 RVV m1 and the Q30 scale path." >&2
  exit 2
fi
if [[ "${rvv_eq2_implicit_vxrm}" == "1" &&
      ( "${backend}" != "rvv" ||
        ( "${eq2_zf_direct_h}" != "1" &&
          ( "${kernel}" != "eq_mmse_2x2" ||
            "${rvv_eq2_staged_narrow_build}" != "1" ||
            "${rvv_eq2_ri_soa_output}" != "1" ) ) ) ]]; then
  echo "Refusing to submit: implicit-vxrm requires RVV ZF2 direct-H or MMSE2 staged RI-SoA." >&2
  exit 2
fi
if [[ "${rvv_eq2_block_soa}" == "1" &&
      ( "${backend}" != "rvv" || "${rvv_eq2_m1}" != "1" ) ]]; then
  echo "Refusing to submit: block-SoA requires the RVV wide mode." >&2
  exit 2
fi
if [[ "${rvv_eq2_group_load}" == "1" && "${rvv_eq2_block_soa}" != "1" ]]; then
  echo "Refusing to submit: group load requires block-SoA." >&2
  exit 2
fi
if [[ "${rvv_eq2_group_load_m4}" == "1" && "${rvv_eq2_group_load}" != "1" ]]; then
  echo "Refusing to submit: m4 group load requires group load." >&2
  exit 2
fi

if [[ "${solve4_schur_zf_solve2h_model}" == "1" ||
      "${check_solve2h_group_layout}" == "1" ||
      "${solve4_schur_zf_vsolve2h2_model}" == "1" ||
      "${solve4_schur_zf_vsolve2h2_asm}" == "1" ||
      "${solve4_schur_zf_pivotscale_model}" == "1" ||
      "${check_vsolve2h2_layout}" == "1" ]]; then
  echo "Refusing to submit: vschur2h/vsolve2h2/solve2h/pivotscale model, asm, and layout-check configs are local-only or disabled; do not submit them to official emu on the fixed hardware platform." >&2
  exit 2
fi

if [[ "${solve4_schur_zf_solve2h_asm_stub}" == "1" ]]; then
  echo "Refusing to submit: vschur2h asm-stub is linked-assembly-only preflight; use scripts/check_vschur2h_asm_stub.sh and do not submit it to official emu before ISA/NEMU/RTL support exists." >&2
  exit 2
fi

if [[ "${MIMO_ALLOW_DIAGNOSTIC_Q15ACC_OFFICIAL:-0}" != "1" ]]; then
  if [[ "${backend}" == "scalar" && "${scalar_zf4_q15acc_build}" == "1" ]]; then
    echo "Refusing to submit: MIMO_USE_SCALAR_ZF4_Q15ACC_BUILD is currently a local diagnostic path and does not match the EXT VSCMACC upper-i16 checksum/solve contract. Set MIMO_ALLOW_DIAGNOSTIC_Q15ACC_OFFICIAL=1 only for an explicit diagnostic run." >&2
    exit 2
  fi
  if [[ "${backend}" == "rvv" && "${rvv_zf4_q15acc_build}" == "1" ]]; then
    echo "Refusing to submit: MIMO_USE_RVV_ZF4_Q15ACC_BUILD is currently a local diagnostic path and does not match the EXT VSCMACC upper-i16 checksum/solve contract. Set MIMO_ALLOW_DIAGNOSTIC_Q15ACC_OFFICIAL=1 only for an explicit diagnostic run." >&2
    exit 2
  fi
fi

if [[ "${backend}" == "ext" &&
      "${kernel}" == "eq_zf_4x4" &&
      "${solve4_schur}" == "1" &&
      "${solve4_schur_q15_recip}" == "4" &&
      "${solve4_schur_q15_recip_num_shift}" == "13" &&
      "${site4_solve_soa}" == "1" &&
      "${site4_hermitian_upper_build}" == "1" &&
      "${solve4_hermitian}" != "1" ]]; then
  echo "Refusing to submit: EXT eq_zf_4x4 Schur q15recip mode4 upper-SoA requires MIMO_USE_SOLVE4_HERMITIAN=1; otherwise the wrapper falls back to the generic Schur helper instead of the current-best ZF-only helper." >&2
  exit 2
fi

if [[ "${solve4_hermitian_pivot_scale_harddiv}" == "1" &&
      "${require_no_hard_div}" == "1" ]]; then
  echo "Refusing to submit: MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE_HARDDIV=1 uses hot hard-div; set MIMO_EMU_REQUIRE_NO_HARD_DIV=0 for this direct-div allowed candidate." >&2
  exit 2
fi

if [[ "${solve4_schur_det_scale}" == "1" &&
      "${require_no_hard_div}" == "1" ]]; then
  echo "Refusing to submit: MIMO_USE_SOLVE4_SCHUR_DET_SCALE=1 still uses determinant hard-div; set MIMO_EMU_REQUIRE_NO_HARD_DIV=0 for this direct-div allowed candidate." >&2
  exit 2
fi

if [[ "${solve4_schur_binv_q15}" == "1" &&
      "${require_no_hard_div}" == "1" ]]; then
  echo "Refusing to submit: MIMO_USE_SOLVE4_SCHUR_BINV_Q15=1 still uses Schur hard-div; set MIMO_EMU_REQUIRE_NO_HARD_DIV=0 for this direct-div diagnostic." >&2
  exit 2
fi

if [[ "${solve4_hermitian_pivot_scale}" == "1" &&
      "${solve4_hermitian_pivot_scale_harddiv}" != "1" &&
      "${diagnostic_allow_pivot_scale}" != "1" ]]; then
  if [[ "${solve4_hermitian_hand_sched}" != "1" ]] &&
     [[ "${site4_solve_soa}" != "1" ||
        "${site4_hermitian_upper_build}" != "1" ]]; then
    echo "Refusing to submit: standalone MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE=1 is a rejected local diagnostic candidate. Use the hand-scheduled pairwrap or SoA Hermitian path, or set MIMO_EMU_DIAGNOSTIC_ALLOW_PIVOT_SCALE=1 only for an explicit diagnostic run." >&2
    exit 2
  fi
fi

if pgrep -f "${EMU_ENV}/XiangShan/build/emu" >/dev/null; then
  echo "Refusing to submit: an official emu process is already running; concurrent official simulations are prohibited." >&2
  exit 3
fi

job_dir="${APP}/perf_eval/raw/emu/jobs"
mkdir -p "${job_dir}" "${APP}/perf_eval/raw/emu"

submit_id=$(date +%Y%m%d-%H%M%S)
job_id="${submit_id}-${backend}-${kernel}-w${warmup}-r${repeat}"
driver_log="${job_dir}/${job_id}.driver.log"
worker_script="${job_dir}/${job_id}.run_emu_kernel_once.sh"
worker_pidfile="${job_dir}/${job_id}.worker.pid"
jobs_tsv="${job_dir}/emu_offline_jobs.tsv"
submit_iso=$(date -Is)

auto_asm_symbol() {
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

append_unique_symbol() {
  local symbol=$1
  local existing

  for existing in "${hard_div_symbols[@]}"; do
    if [[ "${existing}" == "${symbol}" ]]; then
      return
    fi
  done
  hard_div_symbols+=("${symbol}")
}

run_hard_div_preflight() {
  if [[ "${require_no_hard_div}" != "1" ]]; then
    echo "MIMO_EMU_HARD_DIV_PREFLIGHT_SKIP reason=MIMO_EMU_REQUIRE_NO_HARD_DIV_${require_no_hard_div}"
    return
  fi
  if [[ "${diagnostic_allow_hard_div}" == "1" ]]; then
    echo "MIMO_EMU_HARD_DIV_PREFLIGHT_SKIP reason=diagnostic_allow_hard_div"
    return
  fi

  local wrapper
  wrapper=$(auto_asm_symbol)
  if [[ -z "${wrapper}" ]]; then
    echo "MIMO_EMU_HARD_DIV_PREFLIGHT_SKIP reason=unknown_kernel kernel=${kernel}"
    return
  fi

  local asm_preflight="${APP}/perf_eval/asm/mimo-${backend}-riscv64-xs-${job_id}-preflight-harddiv.txt"
  mkdir -p "${APP}/perf_eval/asm"

  cd "${ROOT}"
  source ./env.sh >/dev/null
  cd "${APP}"

  make ARCH=riscv64-xs MIMO_IMPL="${backend}" clean >/dev/null
  make ARCH=riscv64-xs \
    MIMO_IMPL="${backend}" \
    MIMO_PERF=1 \
    MIMO_VALIDATE=0 \
    MIMO_VALIDATE_TOL="${validate_tol}" \
    MIMO_VALIDATE_ERROR_STATS="${validate_error_stats}" \
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
    MIMO_USE_SOLVE4_SCHUR_ZF_EXPLICIT="${solve4_schur_zf_explicit}" \
    MIMO_USE_SOLVE4_SCHUR_ZF_RELOAD_MICRO="${solve4_schur_zf_reload_micro}" \
    MIMO_USE_SOLVE4_SCHUR_ZF_TOP_RESOLVE="${solve4_schur_zf_top_resolve}" \
    MIMO_USE_SOLVE4_SCHUR_ZF_HANDASM="${solve4_schur_zf_handasm}" \
	    MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER="${solve4_schur_zf_ext_inner}" \
	    MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_MODEL="${solve4_schur_zf_solve2h_model}" \
	    MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_MODEL="${solve4_schur_zf_vsolve2h2_model}" \
	    MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_ASM="${solve4_schur_zf_vsolve2h2_asm}" \
	    MIMO_USE_SOLVE4_SCHUR_ZF_PIVOTSCALE_MODEL="${solve4_schur_zf_pivotscale_model}" \
	    MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_ASM_STUB="${solve4_schur_zf_solve2h_asm_stub}" \
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
    MIMO_EQ2_EXPERIMENTAL_PROFILE="${eq2_experimental_profile}" \
    MIMO_EQ2_RECIP_MODE="${eq2_recip_mode}" \
    MIMO_EQ2_RECIP_NUM_SHIFT="${eq2_recip_num_shift}" \
    MIMO_USE_EQ2_ZF_DIRECT_H="${eq2_zf_direct_h}" \
    MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP="${eq2_zf_guard1_fine_recip}" \
    MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID="${ext_eq2_zf_guard1_hybrid}" \
    MIMO_USE_EQ2_MMSE_DIRECT_H="${eq2_mmse_direct_h}" \
    MIMO_USE_SCALAR_EQ2_FUSED_LAMBDA="${scalar_eq2_fused_lambda}" \
    MIMO_USE_SCALAR_EQ2_MMSE_FULL_TRUNC_SCALE="${scalar_eq2_mmse_full_trunc_scale}" \
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
	    MIMO_USE_RVV_EQ2_MMSE_FULL_TRUNC_SCALE="${rvv_eq2_mmse_full_trunc_scale}" \
	    MIMO_USE_RVV_EQ2_BLOCK_SOA="${rvv_eq2_block_soa}" \
	    MIMO_USE_RVV_EQ2_GROUP_LOAD="${rvv_eq2_group_load}" \
	    MIMO_USE_RVV_EQ2_GROUP_LOAD_M4="${rvv_eq2_group_load_m4}" \
	    MIMO_USE_EXT_EQ2_INVCOEFF="${ext_eq2_invcoeff}" \
	    MIMO_USE_EXT_EQ2_M2_BATCH="${ext_eq2_m2_batch}" \
	    MIMO_USE_EXT_EQ2_RHS_INTERLEAVE="${ext_eq2_rhs_interleave}" \
	    MIMO_USE_EXT_EQ2_MMSE_ADAPTIVE_Q13="${ext_eq2_mmse_adaptive_q13}" \
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
	    MIMO_USE_RVV_MMSE4_RI_PACKED_LOAD="${rvv_mmse4_ri_packed_load}" \
	    MIMO_USE_RVV_MMSE4_BINV_Q13_REUSE="${rvv_mmse4_binv_q13_reuse}" \
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
    MIMO_USE_EXT_ZF4_VSCMACC_SEED_BUILD="${ext_zf4_vscmacc_seed_build}" \
    MIMO_USE_EXT_ZF4_PACKED_LIVE_BUILD="${ext_zf4_packed_live_build}" \
    MIMO_USE_EXT_ZF4_INLINE_UPPER_I16_SOLVE="${ext_zf4_inline_upper_i16_solve}" \
    MIMO_USE_EXT_ZF4_LIVE_TAIL_SCHUR="${ext_zf4_live_tail_schur}" \
    MIMO_USE_EXT_ZF4_TAIL_SCRATCH_REUSE="${ext_zf4_tail_scratch_reuse}" \
    MIMO_USE_EXT_ZF4_INLINE_HEAD_WORK_REUSE="${ext_zf4_inline_head_work_reuse}" \
    MIMO_USE_EXT_ZF4_TOP2_MACC_SUM="${ext_zf4_top2_macc_sum}" \
    MIMO_USE_SOLVE4_SCHUR_ZF_EXT_MACC_INNER="${solve4_schur_zf_ext_macc_inner}" \
    MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_MASK="${solve4_schur_zf_num_arshift_mask}" \
    MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE="${ext_zf4_true16b_rvv_i32_solve}" \
    MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_BSOLVE="${ext_zf4_true16b_rvv_i32_bsolve}" \
    MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SPLIT_SOLVE="${ext_zf4_true16b_rvv_i32_split_solve}" \
    MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_STORE="${ext_zf4_true16b_rvv_i32_pack32_store}" \
    MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_LOAD="${ext_zf4_true16b_rvv_i32_pack32_load}" \
    MIMO_USE_EXT_MMSE4_RVV_I32_LATE_TAIL_LOAD="${ext_mmse4_rvv_i32_late_tail_load}" \
    MIMO_USE_EXT_MMSE4_RVV_I32_SPECIALIZE="${ext_mmse4_rvv_i32_specialize}" \
    MIMO_USE_EXT_MMSE4_RVV_I32_INNER_Q30_ONCE="${ext_mmse4_rvv_i32_inner_q30_once}" \
    MIMO_USE_EXT_MMSE4_RVV_I32_TOP_Q30_ONCE="${ext_mmse4_rvv_i32_top_q30_once}" \
    MIMO_USE_EXT_MMSE4_RVV_I32_ASSUME_LAMBDA_NOSAT="${ext_mmse4_rvv_i32_assume_lambda_nosat}" \
    MIMO_USE_EXT_MMSE4_RVV_I32_BINV_REUSE="${ext_mmse4_rvv_i32_binv_reuse}" \
    MIMO_USE_EXT_MMSE4_RVV_I32_PACKED_PHASE="${ext_mmse4_rvv_i32_packed_phase}" \
    MIMO_EXT_MMSE4_PACKED_TOP_EXT="${ext_mmse4_packed_top_ext}" \
    MIMO_EXT_MMSE4_PACKED_SSOLVE_EXT="${ext_mmse4_packed_ssolve_ext}" \
    MIMO_USE_EXT_MMSE4_PACKED_LIVE_BUILD="${ext_mmse4_packed_live_build}" \
    MIMO_USE_EXT_MMSE4_PACKED_LIVE_BUILD_UNROLL4="${ext_mmse4_packed_live_build_unroll4}" \
    MIMO_EXT_MMSE4_PACKED_INVCOEFF_EXT="${ext_mmse4_packed_invcoeff_ext}" \
    MIMO_USE_EXT_MMSE4_PACKED_VSET_CHAIN="${ext_mmse4_packed_vset_chain}" \
    MIMO_EXT_MMSE4_PACKED_SOLVE_SEED="${ext_mmse4_packed_solve_seed}" \
    MIMO_EXT_MMSE4_PACKED_DET_EXT="${ext_mmse4_packed_det_ext}" \
    MIMO_USE_EXT_MMSE4_PACKED_FIXED_PTRS="${ext_mmse4_packed_fixed_ptrs}" \
    MIMO_USE_EXT_MMSE4_PACKED_E16_RECIP="${ext_mmse4_packed_e16_recip}" \
    MIMO_USE_EXT_MMSE4_PACKED_E16_LAMBDA="${ext_mmse4_packed_e16_lambda}" \
    MIMO_USE_EXT_MMSE4_PACKED_RX4_M4_LOAD="${ext_mmse4_packed_rx4_m4_load}" \
    MIMO_EXT_MMSE4_PACKED_S_RECIP_MODE="${ext_mmse4_packed_s_recip_mode}" \
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
  cp "${APP}/build/mimo-${backend}.txt" "${asm_preflight}"

  hard_div_symbols=()
  local wrapper_symbol
  wrapper_symbol=$(resolve_asm_symbol "${wrapper}" "${asm_preflight}")
  if [[ -z "${wrapper_symbol}" ]]; then
    wrapper_symbol=${wrapper}
  fi
  append_unique_symbol "${wrapper_symbol}"

  # Static fallback symbols may be local and therefore are not always found
  # by the caller-reachability walk. Add the direct-MMSE EXT fallback explicitly.
  if [[ "${backend}" == "ext" && "${kernel}" == "eq_mmse_2x2" &&
        "${eq2_mmse_direct_h}" == "1" ]]; then
    local fallback_symbol
    fallback_symbol=$(resolve_asm_symbol \
      mimo_eq2_ext_mmse_direct_fallback_block "${asm_preflight}")
    if [[ -n "${fallback_symbol}" ]]; then
      append_unique_symbol "${fallback_symbol}"
    fi
  fi

  local helper
  local helper_symbol
  local helpers=(
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
    schur4_solve_lane_q15
    mimo_solve_normal4_site4_q15
    mimo_solve_normal4_site4_soa_q15
    mimo_solve_normal4_site4_soa_unroll_lane_q15
    mimo_eq_mmse4_scalar_fused_q15
    mimo_eq2_zf_scalar_fused_q15
    mimo_eq2_mmse_scalar_fused_q15
    mimo_eq2_zf_rvv_ri_soa_q15
    mimo_eq2_mmse_rvv_ri_soa_q15
    mimo_eq2_ext_mmse_direct_fallback_block
    mimo_solve_normal4_scalar_q15
    mimo_solve_normal_4_schur_scalar
    mimo_solve_normal_4_exact
    mimo_solve_normal_q15
    mimo_div_c32_by_i32
    mimo_div_q15_by_i32
  )

  local changed
  local found
  local existing
  changed=1
  while [[ "${changed}" == "1" ]]; do
    changed=0
    for helper in "${helpers[@]}"; do
      helper_symbol=$(resolve_asm_symbol "${helper}" "${asm_preflight}")
      [[ -n "${helper_symbol}" ]] || continue

      found=0
      for existing in "${hard_div_symbols[@]}"; do
        if [[ "${existing}" == "${helper_symbol}" ]]; then
          found=1
          break
        fi
      done
      [[ "${found}" == "0" ]] || continue

      local caller
      for caller in "${hard_div_symbols[@]}"; do
        if asm_symbol_refs "${caller}" "${helper_symbol}" "${asm_preflight}"; then
          append_unique_symbol "${helper_symbol}"
          changed=1
          break
        fi
      done
    done
  done

  if [[ "${solve4_hermitian_hand_sched}" == "1" ||
        "${solve4_hermitian_factor_call}" == "1" ||
        "${solve4_hermitian_q15_call}" == "1" ]]; then
    for helper in c32_factor_q30_pivotscale_call \
                  herm_pivot_div_to_q30_scaled_call \
                  c32_div_real_to_q15_pivotscale_call; do
      helper_symbol=$(resolve_asm_symbol "${helper}" "${asm_preflight}")
      [[ -n "${helper_symbol}" ]] || continue
      append_unique_symbol "${helper_symbol}"
    done
  fi

  echo "MIMO_EMU_HARD_DIV_PREFLIGHT_START asm=${asm_preflight} symbols=${hard_div_symbols[*]}"
  local symbol
  for symbol in "${hard_div_symbols[@]}"; do
    MIMO_ASM_FORBID_HARD_DIV=1 \
      MIMO_ASM_FORBID_SEGMENT_LDST=1 \
      "${APP}/scripts/check_asm_symbol_counts.sh" --symbol "${symbol}" "${asm_preflight}"
  done
	  if [[ "${backend}" == "ext" &&
	        ( "${kernel}" == "eq_zf_2x2" || "${kernel}" == "eq_mmse_2x2" ) ]]; then
	    local eq2_min_custom_ext=19
	    local eq2_max_stack_frame=0
	    if [[ "${eq2_zf_direct_h}" == "1" ]]; then
	      eq2_min_custom_ext=10
	    fi
	    if [[ "${eq2_mmse_direct_h}" == "1" ]]; then
	      eq2_min_custom_ext=18
	      eq2_max_stack_frame=48
	    fi
	    if [[ "${ext_eq2_zf_guard1_hybrid}" == "1" ]]; then
	      eq2_min_custom_ext=4
	      eq2_max_stack_frame=80
	    fi
	    MIMO_ASM_FORBID_HARD_DIV=1 \
	    MIMO_ASM_FORBID_SEGMENT_LDST=1 \
	    MIMO_ASM_MIN_CUSTOM_EXT_VIMAC="${eq2_min_custom_ext}" \
	    MIMO_ASM_MAX_STACK_FRAME_BYTES="${eq2_max_stack_frame}" \
      "${APP}/scripts/check_asm_symbol_counts.sh" \
        --symbol "${wrapper}" "${asm_preflight}"
  fi
  if [[ "${ext_mmse4_packed_live_build}" == "1" ]]; then
    local packed_live_symbol=mimo_ext_mmse4_packed_live_run
    if [[ "${ext_zf4_packed_live_build}" == "1" ]]; then
      packed_live_symbol=mimo_ext_zf4_packed_live_run
    fi
    local packed_min_custom_ext=58
    if [[ "${ext_mmse4_packed_live_build_unroll4}" == "1" ]]; then
      packed_min_custom_ext=$((packed_min_custom_ext + 28))
    fi
    if [[ "${ext_mmse4_packed_invcoeff_ext}" == "1" ]]; then
      packed_min_custom_ext=$((packed_min_custom_ext + 6))
    fi
    if [[ "${ext_mmse4_packed_det_ext}" == "1" ]]; then
      packed_min_custom_ext=$((packed_min_custom_ext + 4))
    fi
    MIMO_ASM_FORBID_HARD_DIV=1 \
    MIMO_ASM_FORBID_SEGMENT_LDST=1 \
    MIMO_ASM_MIN_CUSTOM_EXT_VIMAC="${packed_min_custom_ext}" \
    MIMO_ASM_MAX_WHOLE_VREG_LOAD=0 \
    MIMO_ASM_MAX_WHOLE_VREG_STORE=0 \
      "${APP}/scripts/check_asm_symbol_counts.sh" \
        --symbol "${packed_live_symbol}" \
        "${asm_preflight}"
    if [[ "${ext_mmse4_packed_rx4_m4_load}" == "1" ]]; then
      MIMO_ASM_MIN_VLE16=5 \
      MIMO_ASM_MAX_VLE16=5 \
      MIMO_ASM_MIN_E16_M4_VSET=1 \
      MIMO_ASM_MAX_E16_M4_VSET=1 \
      MIMO_ASM_MIN_RX4_M4_GROUP_LOAD=5 \
      MIMO_ASM_MAX_RX4_M4_GROUP_LOAD=5 \
        "${APP}/scripts/check_asm_symbol_counts.sh" \
          --symbol "${packed_live_symbol}" \
          "${asm_preflight}"
    fi
  elif [[ "${ext_mmse4_rvv_i32_packed_phase}" == "1" ]]; then
    MIMO_ASM_FORBID_HARD_DIV=1 \
    MIMO_ASM_FORBID_SEGMENT_LDST=1 \
    MIMO_ASM_MIN_CUSTOM_EXT_VIMAC=26 \
      "${APP}/scripts/check_asm_symbol_counts.sh" \
        --symbol mimo_solve_normal4_site4_upper_i16_schur_zf_rvv_i32_q15 \
        "${asm_preflight}"
  fi
  echo "MIMO_EMU_HARD_DIV_PREFLIGHT_PASS asm=${asm_preflight} symbols=${hard_div_symbols[*]}"
}

run_hard_div_preflight

cp "${APP}/scripts/run_emu_kernel_once.sh" "${worker_script}"
chmod +x "${worker_script}"
rm -f "${worker_pidfile}"

if [[ ! -f "${jobs_tsv}" ]]; then
  printf 'job_id\tsubmit_iso\tpid\tbackend\tkernel\twarmup\trepeat\ttimeout_sec\tdriver_log\tworker_script\n' > "${jobs_tsv}"
fi

nohup setsid --fork --wait env \
  MIMO_EMU_OFFLINE_RUN=1 \
  MIMO_EMU_JOB_ID="${job_id}" \
  MIMO_WARMUP_REPEAT="${warmup}" \
  MIMO_REPEAT="${repeat}" \
  MIMO_4X4_EQ_ONLY_LOCK="${eq4_only_lock}" \
  MIMO_ZF4_ONLY_LOCK="${zf4_only_lock}" \
  MIMO_VALIDATE_TOL="${validate_tol}" \
  MIMO_VALIDATE_ERROR_STATS="${validate_error_stats}" \
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
  MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_MODEL="${solve4_schur_zf_solve2h_model}" \
  MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_MODEL="${solve4_schur_zf_vsolve2h2_model}" \
  MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_ASM="${solve4_schur_zf_vsolve2h2_asm}" \
  MIMO_USE_SOLVE4_SCHUR_ZF_PIVOTSCALE_MODEL="${solve4_schur_zf_pivotscale_model}" \
  MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_ASM_STUB="${solve4_schur_zf_solve2h_asm_stub}" \
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
  MIMO_EQ2_EXPERIMENTAL_PROFILE="${eq2_experimental_profile}" \
  MIMO_EQ2_RECIP_MODE="${eq2_recip_mode}" \
  MIMO_EQ2_RECIP_NUM_SHIFT="${eq2_recip_num_shift}" \
  MIMO_USE_EQ2_ZF_DIRECT_H="${eq2_zf_direct_h}" \
  MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP="${eq2_zf_guard1_fine_recip}" \
  MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID="${ext_eq2_zf_guard1_hybrid}" \
  MIMO_USE_EQ2_MMSE_DIRECT_H="${eq2_mmse_direct_h}" \
  MIMO_USE_SCALAR_EQ2_FUSED_LAMBDA="${scalar_eq2_fused_lambda}" \
  MIMO_USE_SCALAR_EQ2_MMSE_FULL_TRUNC_SCALE="${scalar_eq2_mmse_full_trunc_scale}" \
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
	  MIMO_USE_RVV_EQ2_MMSE_FULL_TRUNC_SCALE="${rvv_eq2_mmse_full_trunc_scale}" \
	  MIMO_USE_RVV_EQ2_BLOCK_SOA="${rvv_eq2_block_soa}" \
	  MIMO_USE_RVV_EQ2_GROUP_LOAD="${rvv_eq2_group_load}" \
	  MIMO_USE_RVV_EQ2_GROUP_LOAD_M4="${rvv_eq2_group_load_m4}" \
	  MIMO_USE_EXT_EQ2_INVCOEFF="${ext_eq2_invcoeff}" \
	  MIMO_USE_EXT_EQ2_M2_BATCH="${ext_eq2_m2_batch}" \
	  MIMO_USE_EXT_EQ2_RHS_INTERLEAVE="${ext_eq2_rhs_interleave}" \
	  MIMO_USE_EXT_EQ2_MMSE_ADAPTIVE_Q13="${ext_eq2_mmse_adaptive_q13}" \
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
	  MIMO_USE_RVV_MMSE4_RI_PACKED_LOAD="${rvv_mmse4_ri_packed_load}" \
	  MIMO_USE_RVV_MMSE4_BINV_Q13_REUSE="${rvv_mmse4_binv_q13_reuse}" \
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
  MIMO_USE_EXT_ZF4_VSCMACC_SEED_BUILD="${ext_zf4_vscmacc_seed_build}" \
  MIMO_USE_EXT_ZF4_PACKED_LIVE_BUILD="${ext_zf4_packed_live_build}" \
  MIMO_USE_EXT_ZF4_INLINE_UPPER_I16_SOLVE="${ext_zf4_inline_upper_i16_solve}" \
  MIMO_USE_EXT_ZF4_LIVE_TAIL_SCHUR="${ext_zf4_live_tail_schur}" \
  MIMO_USE_EXT_ZF4_TAIL_SCRATCH_REUSE="${ext_zf4_tail_scratch_reuse}" \
  MIMO_USE_EXT_ZF4_INLINE_HEAD_WORK_REUSE="${ext_zf4_inline_head_work_reuse}" \
  MIMO_USE_EXT_ZF4_TOP2_MACC_SUM="${ext_zf4_top2_macc_sum}" \
  MIMO_USE_SOLVE4_SCHUR_ZF_EXT_MACC_INNER="${solve4_schur_zf_ext_macc_inner}" \
  MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_MASK="${solve4_schur_zf_num_arshift_mask}" \
  MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE="${ext_zf4_true16b_rvv_i32_solve}" \
  MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_BSOLVE="${ext_zf4_true16b_rvv_i32_bsolve}" \
  MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SPLIT_SOLVE="${ext_zf4_true16b_rvv_i32_split_solve}" \
  MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_STORE="${ext_zf4_true16b_rvv_i32_pack32_store}" \
  MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_LOAD="${ext_zf4_true16b_rvv_i32_pack32_load}" \
  MIMO_USE_EXT_MMSE4_RVV_I32_LATE_TAIL_LOAD="${ext_mmse4_rvv_i32_late_tail_load}" \
  MIMO_USE_EXT_MMSE4_RVV_I32_SPECIALIZE="${ext_mmse4_rvv_i32_specialize}" \
  MIMO_USE_EXT_MMSE4_RVV_I32_INNER_Q30_ONCE="${ext_mmse4_rvv_i32_inner_q30_once}" \
  MIMO_USE_EXT_MMSE4_RVV_I32_TOP_Q30_ONCE="${ext_mmse4_rvv_i32_top_q30_once}" \
  MIMO_USE_EXT_MMSE4_RVV_I32_ASSUME_LAMBDA_NOSAT="${ext_mmse4_rvv_i32_assume_lambda_nosat}" \
  MIMO_USE_EXT_MMSE4_RVV_I32_BINV_REUSE="${ext_mmse4_rvv_i32_binv_reuse}" \
  MIMO_USE_EXT_MMSE4_RVV_I32_PACKED_PHASE="${ext_mmse4_rvv_i32_packed_phase}" \
  MIMO_EXT_MMSE4_PACKED_TOP_EXT="${ext_mmse4_packed_top_ext}" \
  MIMO_EXT_MMSE4_PACKED_SSOLVE_EXT="${ext_mmse4_packed_ssolve_ext}" \
  MIMO_USE_EXT_MMSE4_PACKED_LIVE_BUILD="${ext_mmse4_packed_live_build}" \
  MIMO_USE_EXT_MMSE4_PACKED_LIVE_BUILD_UNROLL4="${ext_mmse4_packed_live_build_unroll4}" \
  MIMO_EXT_MMSE4_PACKED_INVCOEFF_EXT="${ext_mmse4_packed_invcoeff_ext}" \
  MIMO_USE_EXT_MMSE4_PACKED_VSET_CHAIN="${ext_mmse4_packed_vset_chain}" \
  MIMO_EXT_MMSE4_PACKED_SOLVE_SEED="${ext_mmse4_packed_solve_seed}" \
  MIMO_EXT_MMSE4_PACKED_DET_EXT="${ext_mmse4_packed_det_ext}" \
  MIMO_USE_EXT_MMSE4_PACKED_FIXED_PTRS="${ext_mmse4_packed_fixed_ptrs}" \
  MIMO_USE_EXT_MMSE4_PACKED_E16_RECIP="${ext_mmse4_packed_e16_recip}" \
  MIMO_USE_EXT_MMSE4_PACKED_E16_LAMBDA="${ext_mmse4_packed_e16_lambda}" \
  MIMO_USE_EXT_MMSE4_PACKED_RX4_M4_LOAD="${ext_mmse4_packed_rx4_m4_load}" \
  MIMO_EXT_MMSE4_PACKED_S_RECIP_MODE="${ext_mmse4_packed_s_recip_mode}" \
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
  MIMO_EMU_REQUIRE_NO_HARD_DIV="${require_no_hard_div}" \
  bash -c 'cd "$1" && exec "$5" "$2" "$3" "$4"' \
  mimo-emu-job "${APP}" "${backend}" "${kernel}" "${timeout_sec}" "${worker_script}" \
  > "${driver_log}" 2>&1 < /dev/null &

launch_pid=$!
pid=

for _ in $(seq 1 100); do
  if [[ -s "${worker_pidfile}" ]]; then
    worker_pid=$(tr -d '[:space:]' < "${worker_pidfile}")
    if [[ "${worker_pid}" =~ ^[0-9]+$ ]] && kill -0 "${worker_pid}" 2>/dev/null; then
      pid="${worker_pid}"
      break
    fi
  fi
  if ! kill -0 "${launch_pid}" 2>/dev/null; then
    break
  fi
  sleep 0.1
done

if [[ -z "${pid}" ]] && kill -0 "${launch_pid}" 2>/dev/null; then
  pid="${launch_pid}"
  echo "MIMO_EMU_JOB_WORKER_PID_PENDING job_id=${job_id} launch_pid=${launch_pid} pidfile=${worker_pidfile}" >&2
fi

if [[ -z "${pid}" ]] || ! kill -0 "${pid}" 2>/dev/null; then
  echo "MIMO_EMU_JOB_LAUNCH_FAILED job_id=${job_id} launch_pid=${launch_pid} worker_pid=${pid:-missing} driver_log=${driver_log}" >&2
  if [[ -s "${driver_log}" ]]; then
    sed -n '1,120p' "${driver_log}" >&2
  else
    echo "MIMO_EMU_JOB_LAUNCH_FAILED_EMPTY_LOG job_id=${job_id}" >&2
  fi
  exit 2
fi

printf '%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n' \
  "${job_id}" "${submit_iso}" "${pid}" "${backend}" "${kernel}" \
  "${warmup}" "${repeat}" "${timeout_sec}" "${driver_log}" "${worker_script}" >> "${jobs_tsv}"

printf 'MIMO_EMU_JOB_SUBMITTED job_id=%s pid=%s backend=%s kernel=%s warmup=%s repeat=%s timeout_sec=%s driver_log=%s worker_script=%s\n' \
  "${job_id}" "${pid}" "${backend}" "${kernel}" "${warmup}" "${repeat}" "${timeout_sec}" "${driver_log}" "${worker_script}"
printf 'MIMO_EMU_JOB_ZF4_RVV_SEED_FLAGS MIMO_USE_RVV_ZF4_SEED_ACC=%s\n' \
  "${rvv_zf4_seed_acc}"
printf 'MIMO_EMU_JOB_PID_TRACKING job_id=%s launch_pid=%s worker_pid=%s worker_pidfile=%s\n' \
  "${job_id}" "${launch_pid}" "${pid}" "${worker_pidfile}"
printf 'MIMO_EMU_JOB_EQ2_PROFILE MIMO_EQ2_EXPERIMENTAL_PROFILE=%s\n' \
  "${eq2_experimental_profile}"
printf 'MIMO_EMU_JOB_EQ2_FLAGS MIMO_EQ2_RECIP_MODE=%s MIMO_EQ2_RECIP_NUM_SHIFT=%s MIMO_USE_EQ2_ZF_DIRECT_H=%s MIMO_USE_EQ2_ZF_GUARD1_FINE_RECIP=%s MIMO_USE_EXT_EQ2_ZF_GUARD1_HYBRID=%s MIMO_USE_EQ2_MMSE_DIRECT_H=%s MIMO_USE_SCALAR_EQ2_FUSED_LAMBDA=%s MIMO_USE_SCALAR_EQ2_MMSE_FULL_TRUNC_SCALE=%s MIMO_USE_RVV_EQ2_M1=%s MIMO_USE_RVV_EQ2_ALWAYS_ADD_LAMBDA=%s MIMO_USE_RVV_EQ2_SPLIT_BUILD=%s MIMO_USE_RVV_EQ2_FUSED_NCLIP_BUILD=%s MIMO_USE_RVV_EQ2_TX_SOA_STORE=%s MIMO_USE_RVV_EQ2_3M_BUILD=%s MIMO_USE_RVV_EQ2_ZEXT_PACK=%s MIMO_USE_RVV_EQ2_RI_SOA_OUTPUT=%s MIMO_USE_RVV_EQ2_STAGED_NARROW_BUILD=%s MIMO_USE_RVV_EQ2_IMPLICIT_VXRM=%s MIMO_USE_RVV_EQ2_UQ12_SCALE=%s MIMO_USE_RVV_EQ2_EARLY_RECIP_OVERLAP=%s MIMO_USE_RVV_EQ2_MMSE_FULL_TRUNC_SCALE=%s MIMO_USE_RVV_EQ2_BLOCK_SOA=%s MIMO_USE_RVV_EQ2_GROUP_LOAD=%s MIMO_USE_RVV_EQ2_GROUP_LOAD_M4=%s MIMO_USE_EXT_EQ2_INVCOEFF=%s MIMO_USE_EXT_EQ2_M2_BATCH=%s MIMO_USE_EXT_EQ2_RHS_INTERLEAVE=%s MIMO_USE_EXT_EQ2_MMSE_ADAPTIVE_Q13=%s\n' \
  "${eq2_recip_mode}" "${eq2_recip_num_shift}" "${eq2_zf_direct_h}" \
  "${eq2_zf_guard1_fine_recip}" "${ext_eq2_zf_guard1_hybrid}" \
  "${eq2_mmse_direct_h}" \
  "${scalar_eq2_fused_lambda}" "${scalar_eq2_mmse_full_trunc_scale}" \
  "${rvv_eq2_m1}" \
  "${rvv_eq2_always_add_lambda}" "${rvv_eq2_split_build}" \
  "${rvv_eq2_fused_nclip_build}" "${rvv_eq2_tx_soa_store}" \
	  "${rvv_eq2_3m_build}" "${rvv_eq2_zext_pack}" "${rvv_eq2_ri_soa_output}" \
	  "${rvv_eq2_staged_narrow_build}" "${rvv_eq2_implicit_vxrm}" \
	  "${rvv_eq2_uq12_scale}" "${rvv_eq2_early_recip_overlap}" \
	  "${rvv_eq2_mmse_full_trunc_scale}" \
	  "${rvv_eq2_block_soa}" "${rvv_eq2_group_load}" "${rvv_eq2_group_load_m4}" \
	  "${ext_eq2_invcoeff}" "${ext_eq2_m2_batch}" "${ext_eq2_rhs_interleave}" \
	  "${ext_eq2_mmse_adaptive_q13}"
printf 'MIMO_EMU_JOB_BUILD_FLAGS MIMO_VALIDATE_TOL=%s MIMO_VALIDATE_ERROR_STATS=%s MIMO_USE_RECIP_TABLE=%s MIMO_USE_SOLVE4_UNROLL_V2A=%s MIMO_USE_SOLVE4_HERMITIAN=%s MIMO_USE_SOLVE4_HERMITIAN_LANE_OUTER=%s MIMO_USE_SOLVE4_HERMITIAN_SOA_STAGE_OUTER=%s MIMO_USE_SOLVE4_HERMITIAN_FACTOR_PRECOMPUTE=%s MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE=%s MIMO_USE_SOLVE4_HERMITIAN_PIVOT_SCALE_HARDDIV=%s MIMO_USE_SOLVE4_HERMITIAN_HAND_SCHED=%s MIMO_USE_SOLVE4_HERMITIAN_FACTOR_CALL=%s MIMO_USE_SOLVE4_HERMITIAN_FACTOR_Q15=%s MIMO_USE_SOLVE4_HERMITIAN_Q15_CALL=%s MIMO_USE_SOLVE4_HERMITIAN_Q15_ONECORR=%s MIMO_SOLVE4_HERMITIAN_Q15_JUMP_ONLY_MASK=%s MIMO_USE_SOLVE4_HERMITIAN_GS=%s MIMO_SOLVE4_HERMITIAN_GS_ITERS=%s MIMO_USE_SOLVE4_HERMITIAN_Q30_FIVECORR=%s MIMO_USE_SOLVE4_HERMITIAN_Q30_TWOJUMP_CORR=%s MIMO_USE_SOLVE4_HERMITIAN_Q30_JUMP_ONLY=%s MIMO_USE_SOLVE4_HERMITIAN_Q30_ONECORR=%s MIMO_USE_SOLVE4_HERMITIAN_Q30_PAIR_CORR=%s MIMO_SOLVE4_HERMITIAN_Q30_JUMP_ONLY_MASK=%s MIMO_USE_SOLVE4_HERMITIAN_POS_PIVOT_SCALE=%s MIMO_USE_SOLVE4_HERMITIAN_TRUST_PIVOT_RANGE=%s MIMO_USE_SOLVE4_HERMITIAN_TRUST_PIVOT_NONZERO=%s MIMO_USE_SOLVE4_HERMITIAN_FULL_RECIP_TABLE=%s MIMO_USE_SOLVE4_HERMITIAN_FLOOR_FULL_RECIP_TABLE=%s MIMO_USE_SOLVE4_HERMITIAN_RECIP_STEP16=%s MIMO_USE_SOLVE4_HERMITIAN_DIAG_REAL_UPDATE=%s MIMO_USE_SOLVE_BRANCHLIGHT_ROUND=%s MIMO_USE_SOLVE4_LDLH=%s MIMO_USE_SOLVE4_SCHUR=%s MIMO_USE_SOLVE4_SCHUR_DET_SCALE=%s MIMO_USE_SOLVE_SKIP_LOWER_UPDATE=%s MIMO_USE_RVV_SOLVE4=%s MIMO_USE_RVV_SOLVE4_STREAMED=%s MIMO_USE_RVV_SOLVE4_VNCLIP_ROUND=%s MIMO_USE_RVV_SOLVE4_Q15_I32_MUL=%s MIMO_USE_RVV_SITE4_MF2=%s MIMO_USE_EXT_ZF4_LOCAL_SOLVE=%s MIMO_USE_EXT_ZF4_RVV_NODIV_SOLVE=%s MIMO_USE_SITE4_SOLVE_SOA=%s MIMO_USE_SITE4_SOLVE_SOA_UNROLL=%s MIMO_USE_SITE4_DIAG_REAL_BUILD=%s MIMO_USE_SITE4_HERMITIAN_UPPER_BUILD=%s MIMO_USE_SITE4_HERMITIAN_UPPER_MMSE=%s MIMO_USE_SITE4_HERMITIAN_UPPER_PACKED=%s MIMO_USE_SITE4_HERMITIAN_UPPER_ROWFUSE=%s MIMO_USE_SITE4_HERMITIAN_UPPER_ROWPAIR=%s MIMO_USE_SITE4_ROUND_ONCE_DOT=%s MIMO_COLLECT_SOLVE_DEN_STATS=%s MIMO_COLLECT_SOLVE_VALUE_STATS=%s MIMO_COLLECT_SCHUR_STATS=%s MIMO_COLLECT_PIVOT_SCALE_STATS=%s MIMO_EMU_REQUIRE_NO_HARD_DIV=%s\n' \
  "${validate_tol}" "${validate_error_stats}" "${recip_table}" "${solve4_unroll}" "${solve4_hermitian}" "${solve4_hermitian_lane_outer}" "${solve4_hermitian_soa_stage_outer}" "${solve4_hermitian_factor_precompute}" "${solve4_hermitian_pivot_scale}" "${solve4_hermitian_pivot_scale_harddiv}" "${solve4_hermitian_hand_sched}" "${solve4_hermitian_factor_call}" "${solve4_hermitian_factor_q15}" "${solve4_hermitian_q15_call}" "${solve4_hermitian_q15_onecorr}" "${solve4_hermitian_q15_jump_only_mask}" "${solve4_hermitian_gs}" "${solve4_hermitian_gs_iters}" "${solve4_hermitian_q30_fivecorr}" "${solve4_hermitian_q30_twojump_corr}" "${solve4_hermitian_q30_jump_only}" "${solve4_hermitian_q30_onecorr}" "${solve4_hermitian_q30_pair_corr}" "${solve4_hermitian_q30_jump_only_mask}" "${solve4_hermitian_pos_pivot_scale}" "${solve4_hermitian_trust_pivot_range}" "${solve4_hermitian_trust_pivot_nonzero}" "${solve4_hermitian_full_recip_table}" "${solve4_hermitian_floor_full_recip_table}" "${solve4_hermitian_recip_step16}" "${solve4_hermitian_diag_real_update}" "${solve_branchlight_round}" "${solve4_ldlh}" "${solve4_schur}" "${solve4_schur_det_scale}" "${solve_skip_lower_update}" "${rvv_solve4}" "${rvv_solve4_streamed}" "${rvv_solve4_vnclip}" "${rvv_solve4_q15_i32_mul}" "${rvv_site4_mf2}" "${ext_zf4_local_solve}" "${ext_zf4_rvv_nodiv_solve}" "${site4_solve_soa}" "${site4_solve_soa_unroll}" "${site4_diag_real_build}" "${site4_hermitian_upper_build}" "${site4_hermitian_upper_mmse}" "${site4_hermitian_upper_packed}" "${site4_hermitian_upper_rowfuse}" "${site4_hermitian_upper_rowpair}" "${site4_round_once_dot}" "${den_stats}" "${value_stats}" "${schur_stats}" "${pivot_scale_stats}" "${require_no_hard_div}"
printf 'MIMO_EMU_JOB_EXTRA_BUILD_FLAGS MIMO_USE_RVV_ZF4_SCHUR_RVV_SOLVE=%s MIMO_USE_RVV_MMSE4_SCHUR_RVV_SOLVE=%s MIMO_USE_RVV_ZF4_FUSED_BUILD_SOLVE=%s MIMO_USE_RVV_ZF4_STAGED_BUILD_SOLVE=%s MIMO_USE_RVV_ZF4_Q15ACC_BUILD=%s MIMO_USE_RVV_ZF4_PACK32_STORE=%s MIMO_USE_MMSE4_SCHUR_NO_MAG_SCALE=%s MIMO_USE_MMSE4_SCHUR_SIGNED_TRUNC_SCALE=%s MIMO_USE_MMSE4_SCHUR_NUM_ARSHIFT_SCALE=%s MIMO_USE_EXT_MMSE4_SCHUR_FINAL_SIGNED_TRUNC_SCALE=%s MIMO_EXT_MMSE4_SCHUR_SIGNED_TRUNC_MASK=%s MIMO_USE_EXT_MMSE4_RVV_I32_VEC_LAMBDA=%s MIMO_USE_EXT_ZF4_RVV_NODIV_FAST_SOLVE=%s MIMO_USE_EXT_ZF4_SCHUR_RVV_SOLVE=%s MIMO_USE_EXT_MMSE4_SCHUR_RVV_SOLVE=%s MIMO_USE_EXT_MMSE4_FUSED_BUILD_SOLVE=%s MIMO_USE_EXT_MMSE4_NARROW_WORK=%s MIMO_USE_EXT_MMSE4_LIVE_BUILD=%s MIMO_USE_EXT_MMSE4_STREAM_WORK_SOLVE=%s MIMO_USE_EXT_MMSE4_TRUE16B_LIVE_TAIL_SOLVE=%s MIMO_USE_EXT_ZF4_RVV_ROUND_ONCE_BUILD=%s MIMO_USE_EXT_ZF4_TRUE16B_WORK=%s MIMO_USE_EXT_ZF4_PACK32_WORK=%s MIMO_USE_EXT_ZF4_VSCMACC_BUILD=%s MIMO_USE_EXT_ZF4_VSCMACC_FUSED_BUILD=%s MIMO_USE_EXT_ZF4_INLINE_UPPER_I16_SOLVE=%s MIMO_USE_EXT_ZF4_LIVE_TAIL_SCHUR=%s MIMO_USE_EXT_ZF4_TAIL_SCRATCH_REUSE=%s MIMO_USE_EXT_ZF4_INLINE_HEAD_WORK_REUSE=%s MIMO_USE_EXT_ZF4_TOP2_MACC_SUM=%s MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SOLVE=%s MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_BSOLVE=%s MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_SPLIT_SOLVE=%s MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_STORE=%s MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PACK32_LOAD=%s MIMO_USE_EXT_MMSE4_RVV_I32_LATE_TAIL_LOAD=%s MIMO_USE_EXT_MMSE4_RVV_I32_SPECIALIZE=%s MIMO_USE_EXT_MMSE4_RVV_I32_INNER_Q30_ONCE=%s MIMO_USE_EXT_MMSE4_RVV_I32_TOP_Q30_ONCE=%s MIMO_USE_EXT_ZF4_TRUE16B_RVV_I32_PRESERVE_PRELUDE=%s MIMO_EXT_ZF4_FAST_CORR_ITERS=%s MIMO_EXT_ZF4_FAST_SCALE_BIAS_Q30=%s MIMO_EXT_ZF4_FAST_REUSE_SCALE=%s MIMO_USE_EXT_ZF4_UPPER_RVV_SOLVE=%s MIMO_USE_EXT_ZF4_MIXED_UPDATE_SOLVE=%s MIMO_USE_SITE4_PRELUDE_HOIST=%s MIMO_USE_SITE4_HERMITIAN_UPPER_ROWFUSE=%s MIMO_USE_SITE4_HERMITIAN_UPPER_ROWPAIR=%s MIMO_USE_SITE4_ROUND_ONCE_DOT=%s MIMO_USE_SITE4_BUILD_SOLVE_PIPELINE2=%s\n' \
  "${rvv_zf4_schur_rvv_solve}" "${rvv_mmse4_schur_rvv_solve}" "${rvv_zf4_fused_build_solve}" "${rvv_zf4_staged_build_solve}" "${rvv_zf4_q15acc_build}" "${rvv_zf4_pack32_store}" "${mmse4_schur_no_mag_scale}" "${mmse4_schur_signed_trunc_scale}" "${mmse4_schur_num_arshift_scale}" "${ext_mmse4_schur_final_signed_trunc_scale}" "${ext_mmse4_schur_signed_trunc_mask}" "${ext_mmse4_rvv_i32_vec_lambda}" "${ext_zf4_rvv_nodiv_fast_solve}" "${ext_zf4_schur_rvv_solve}" "${ext_mmse4_schur_rvv_solve}" "${ext_mmse4_fused_build_solve}" "${ext_mmse4_narrow_work}" "${ext_mmse4_live_build}" "${ext_mmse4_stream_work_solve}" "${ext_mmse4_true16b_live_tail_solve}" "${ext_zf4_rvv_round_once_build}" "${ext_zf4_true16b_work}" "${ext_zf4_pack32_work}" "${ext_zf4_vscmacc_build}" "${ext_zf4_vscmacc_fused_build}" "${ext_zf4_inline_upper_i16_solve}" "${ext_zf4_live_tail_schur}" "${ext_zf4_tail_scratch_reuse}" "${ext_zf4_inline_head_work_reuse}" "${ext_zf4_top2_macc_sum}" "${ext_zf4_true16b_rvv_i32_solve}" "${ext_zf4_true16b_rvv_i32_bsolve}" "${ext_zf4_true16b_rvv_i32_split_solve}" "${ext_zf4_true16b_rvv_i32_pack32_store}" "${ext_zf4_true16b_rvv_i32_pack32_load}" "${ext_mmse4_rvv_i32_late_tail_load}" "${ext_mmse4_rvv_i32_specialize}" "${ext_mmse4_rvv_i32_inner_q30_once}" "${ext_mmse4_rvv_i32_top_q30_once}" "${ext_zf4_true16b_rvv_i32_preserve_prelude}" "${ext_zf4_fast_corr_iters}" "${ext_zf4_fast_scale_bias_q30}" "${ext_zf4_fast_reuse_scale}" "${ext_zf4_upper_rvv_solve}" "${ext_zf4_mixed_update_solve}" "${site4_prelude_hoist}" "${site4_hermitian_upper_rowfuse}" "${site4_hermitian_upper_rowpair}" "${site4_round_once_dot}" "${site4_build_solve_pipeline2}"
printf 'MIMO_EMU_JOB_MMSE4_EXT_LAMBDA_FLAGS MIMO_USE_EXT_MMSE4_RVV_I32_ASSUME_LAMBDA_NOSAT=%s\n' \
  "${ext_mmse4_rvv_i32_assume_lambda_nosat}"
printf 'MIMO_EMU_JOB_MMSE4_EXT_BINV_FLAGS MIMO_USE_EXT_MMSE4_RVV_I32_BINV_REUSE=%s\n' \
  "${ext_mmse4_rvv_i32_binv_reuse}"
printf 'MIMO_EMU_JOB_MMSE4_EXT_PACKED_FLAGS MIMO_USE_EXT_MMSE4_RVV_I32_PACKED_PHASE=%s MIMO_EXT_MMSE4_PACKED_TOP_EXT=%s MIMO_EXT_MMSE4_PACKED_SSOLVE_EXT=%s MIMO_USE_EXT_MMSE4_PACKED_LIVE_BUILD=%s MIMO_USE_EXT_MMSE4_PACKED_LIVE_BUILD_UNROLL4=%s MIMO_EXT_MMSE4_PACKED_INVCOEFF_EXT=%s MIMO_USE_EXT_MMSE4_PACKED_VSET_CHAIN=%s MIMO_EXT_MMSE4_PACKED_SOLVE_SEED=%s MIMO_EXT_MMSE4_PACKED_DET_EXT=%s MIMO_USE_EXT_MMSE4_PACKED_FIXED_PTRS=%s MIMO_USE_EXT_MMSE4_PACKED_E16_RECIP=%s MIMO_USE_EXT_MMSE4_PACKED_E16_LAMBDA=%s MIMO_USE_EXT_MMSE4_PACKED_RX4_M4_LOAD=%s MIMO_USE_EXT_ZF4_VSCMACC_SEED_BUILD=%s\n' \
  "${ext_mmse4_rvv_i32_packed_phase}" "${ext_mmse4_packed_top_ext}" \
  "${ext_mmse4_packed_ssolve_ext}" "${ext_mmse4_packed_live_build}" \
  "${ext_mmse4_packed_live_build_unroll4}" "${ext_mmse4_packed_invcoeff_ext}" \
  "${ext_mmse4_packed_vset_chain}" "${ext_mmse4_packed_solve_seed}" \
  "${ext_mmse4_packed_det_ext}" "${ext_mmse4_packed_fixed_ptrs}" \
  "${ext_mmse4_packed_e16_recip}" "${ext_mmse4_packed_e16_lambda}" \
  "${ext_mmse4_packed_rx4_m4_load}" \
  "${ext_zf4_vscmacc_seed_build}"
printf 'MIMO_EMU_JOB_ZF4_PACKED_FLAGS MIMO_USE_EXT_ZF4_PACKED_LIVE_BUILD=%s\n' \
  "${ext_zf4_packed_live_build}"
printf 'MIMO_EMU_JOB_PACKED_RECIP_FLAGS MIMO_EXT_MMSE4_PACKED_S_RECIP_MODE=%s\n' \
  "${ext_mmse4_packed_s_recip_mode}"
printf 'MIMO_EMU_JOB_ROUND_BUILD_FLAGS MIMO_USE_SOLVE_Q15_I64_ARSHIFT_ROUND=%s MIMO_USE_SOLVE_STORE_TRUST_I16=%s\n' \
  "${solve_q15_i64_arshift_round}" "${solve_store_trust_i16}"
printf 'MIMO_EMU_JOB_SCALAR_BUILD_FLAGS MIMO_USE_SCALAR4_UPPER_ONLY_BUILD=%s MIMO_USE_SCALAR4_COMPLEX_MUL_3M=%s MIMO_USE_SCALAR4_SCHUR_TOP_Q30_ONCE=%s MIMO_USE_SCALAR_MMSE4_FUSED_BUILD_SOLVE=%s MIMO_USE_SCALAR_MMSE4_FUSED_COMPACT_C32=%s MIMO_USE_SCALAR_ZF4_Q15ACC_BUILD=%s\n' \
  "${scalar4_upper_only_build}" "${scalar4_complex_mul_3m}" "${scalar4_schur_top_q30_once}" "${scalar_mmse4_fused_build_solve}" "${scalar_mmse4_fused_compact_c32}" "${scalar_zf4_q15acc_build}"
printf 'MIMO_EMU_JOB_MISC_BUILD_FLAGS MIMO_USE_SOLVE4_SCHUR_PAIR2=%s MIMO_DEBUG_EXT_ZF4_RVV_NODIV_COMPARE=%s\n' \
  "${solve4_schur_pair2}" "${debug_ext_zf4_rvv_nodiv_compare}"
printf 'MIMO_EMU_JOB_MMSE4_FAIR_BUILD_FLAGS MIMO_USE_MMSE4_SCHUR_NUM_ARSHIFT_SCALE=%s MIMO_USE_MMSE4_SCHUR_INNER_Q30_ONCE=%s MIMO_USE_RVV_MMSE4_SCHUR_INNER_SYM_ROUND=%s MIMO_USE_RVV_MMSE4_SCHUR_TOP_Q30_ONCE=%s MIMO_USE_RVV_MMSE4_STAGED_BUILD_SOLVE=%s MIMO_USE_RVV_MMSE4_SEED_ACC=%s MIMO_USE_RVV_MMSE4_RI_PACKED_LOAD=%s MIMO_USE_RVV_SCHUR4_SOLVE2_PAIR_M2=%s MIMO_USE_RVV_SCHUR4_VSMUL_SCALE=%s MIMO_USE_RVV_SCHUR4_NUM_MAC=%s MIMO_USE_RVV_SCHUR4_COMPLEX_MAC=%s\n' \
	  "${mmse4_schur_num_arshift_scale}" "${mmse4_schur_inner_q30_once}" "${rvv_mmse4_schur_inner_sym_round}" "${rvv_mmse4_schur_top_q30_once}" "${rvv_mmse4_staged_build_solve}" "${rvv_mmse4_seed_acc}" "${rvv_mmse4_ri_packed_load}" "${rvv_schur4_solve2_pair_m2}" "${rvv_schur4_vsmul_scale}" "${rvv_schur4_num_mac}" "${rvv_schur4_complex_mac}"
printf 'MIMO_EMU_JOB_MMSE4_RVV_AUX_FLAGS MIMO_USE_RVV_MMSE4_EARLY_TAIL_RHS=%s MIMO_USE_RVV_MMSE4_CONST_VL=%s MIMO_USE_RVV_MMSE4_SCHUR_TRUE16B_SOLVE=%s MIMO_USE_RVV_MMSE4_SCHUR_TRUE16B_SHARED_A16=%s\n' \
	  "${rvv_mmse4_early_tail_rhs}" "${rvv_mmse4_const_vl}" "${rvv_mmse4_schur_true16b_solve}" "${rvv_mmse4_schur_true16b_shared_a16}"
printf 'MIMO_EMU_JOB_MMSE4_RVV_Q13_FLAGS MIMO_USE_RVV_MMSE4_BINV_Q13_REUSE=%s\n' \
  "${rvv_mmse4_binv_q13_reuse}"
printf 'MIMO_EMU_JOB_RVV_ARITH_FLAGS MIMO_USE_RVV_SCHUR4_VSSRA_ROUND=%s MIMO_USE_RVV_SITE4_VSSRA_ROUND=%s MIMO_USE_RVV_SITE4_SEG2_LOAD=%s MIMO_USE_RVV_SITE4_RI_SOA=%s MIMO_USE_RVV_SCHUR4_DET_MAC=%s\n' \
  "${rvv_schur4_vssra_round}" "${rvv_site4_vssra_round}" "${rvv_site4_seg2_load}" "${rvv_site4_ri_soa}" "${rvv_schur4_det_mac}"
printf 'MIMO_EMU_JOB_SCHUR_BUILD_FLAGS MIMO_USE_SOLVE4_SCHUR_BINV_Q15=%s MIMO_USE_SOLVE4_SCHUR_Q15_RECIP=%s MIMO_USE_SOLVE4_SCHUR_FUSED_INNER=%s MIMO_USE_SOLVE4_SCHUR_FUSED_INNER_MMSE=%s MIMO_USE_SOLVE4_SCHUR_MMSE_SPECIALIZE=%s MIMO_USE_SOLVE4_SCHUR_ZF_EXPLICIT=%s MIMO_USE_SOLVE4_SCHUR_ZF_RELOAD_MICRO=%s MIMO_USE_SOLVE4_SCHUR_ZF_TOP_RESOLVE=%s MIMO_USE_SOLVE4_SCHUR_ZF_HANDASM=%s MIMO_USE_SOLVE4_SCHUR_ZF_EXT_INNER=%s MIMO_USE_SOLVE4_SCHUR_ZF_EXT_MACC_INNER=%s MIMO_USE_SOLVE4_SCHUR_ZF_NUM_ARSHIFT_MASK=%s MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_MODEL=%s MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_MODEL=%s MIMO_USE_SOLVE4_SCHUR_ZF_VSOLVE2H2_ASM=%s MIMO_USE_SOLVE4_SCHUR_ZF_SOLVE2H_ASM_STUB=%s MIMO_CHECK_SOLVE2H_GROUP_LAYOUT=%s MIMO_CHECK_VSOLVE2H2_LAYOUT=%s MIMO_SOLVE4_SCHUR_Q15_RECIP_NUM_SHIFT=%s MIMO_USE_EQ4_DIRECT_H=%s MIMO_EQ4_DIRECT_H_MODE=%s MIMO_EQ4_DIRECT_H_ITERS=%s MIMO_EQ4_DIRECT_H_NODIV=%s\n' \
  "${solve4_schur_binv_q15}" "${solve4_schur_q15_recip}" "${solve4_schur_fused_inner}" "${solve4_schur_fused_inner_mmse}" "${solve4_schur_mmse_specialize}" "${solve4_schur_zf_explicit}" "${solve4_schur_zf_reload_micro}" "${solve4_schur_zf_top_resolve}" "${solve4_schur_zf_handasm}" "${solve4_schur_zf_ext_inner}" "${solve4_schur_zf_ext_macc_inner}" "${solve4_schur_zf_num_arshift_mask}" "${solve4_schur_zf_solve2h_model}" "${solve4_schur_zf_vsolve2h2_model}" "${solve4_schur_zf_vsolve2h2_asm}" "${solve4_schur_zf_solve2h_asm_stub}" "${check_solve2h_group_layout}" "${check_vsolve2h2_layout}" "${solve4_schur_q15_recip_num_shift}" "${eq4_direct_h}" "${eq4_direct_h_mode}" "${eq4_direct_h_iters}" "${eq4_direct_h_nodiv}"
