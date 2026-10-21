#!/usr/bin/env python3
"""检查 EQ2 质量矩阵的完整性和三端通信质量包络。"""

import argparse
from collections import defaultdict
from pathlib import Path


BACKENDS = {"scalar", "rvv", "ext"}
EXPECTED_CASES = {
    (kernel, mod_bits, noise_q15, cond_profile)
    for kernel in ("eq_zf_2x2", "eq_mmse_2x2")
    for mod_bits, noise_q15, cond_profile in (
        (2, 1024, 0),
        (4, 2048, 1),
        (6, 4096, 2),
    )
}


def parse_record(line: str) -> tuple[str, dict[str, str]] | None:
    fields = line.split()
    if not fields or not fields[0].startswith("MIMO_"):
        return None
    values: dict[str, str] = {}
    for field in fields[1:]:
        if "=" in field:
            key, value = field.split("=", 1)
            values[key] = value
    return fields[0], values


def case_key(values: dict[str, str]) -> tuple[str, int, int, int]:
    return (
        values["kernel"],
        int(values["mod_bits"]),
        int(values["noise_q15"]),
        int(values["cond_profile"]),
    )


def ratio_delta(lhs: int, rhs: int, total: int) -> float:
    return abs(lhs - rhs) / total


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("summary", type=Path)
    parser.add_argument("--max-component-error", type=int, default=16)
    args = parser.parse_args()
    if args.max_component_error < 0:
        raise SystemExit("max-component-error 必须为非负整数")

    floats: dict[tuple[str, int, int, int], dict[str, str]] = {}
    results: dict[
        tuple[str, int, int, int], dict[str, dict[str, str]]
    ] = defaultdict(dict)
    pass_count = 0
    error_stats_count = 0

    for line in args.summary.read_text(encoding="utf-8").splitlines():
        record = parse_record(line)
        if record is None:
            continue
        kind, values = record
        if kind == "MIMO_EQ2_FLOAT_QUALITY":
            floats[case_key(values)] = values
        elif kind == "MIMO_EQ2_QUALITY":
            results[case_key(values)][values["backend"]] = values
        elif kind == "MIMO_VALIDATE_PASS":
            pass_count += 1
        elif kind == "MIMO_VALIDATE_ERROR_STATS":
            error_stats_count += 1
            if (
                int(values["fail"]) != 0
                or int(values["max_abs"]) > args.max_component_error
            ):
                raise SystemExit(f"质量分量门失败: {line}")

    if set(floats) != EXPECTED_CASES:
        raise SystemExit(
            f"float64 case 不完整: actual={sorted(floats)}"
        )
    if set(results) != EXPECTED_CASES:
        raise SystemExit(
            f"backend case 不完整: actual={sorted(results)}"
        )
    if pass_count != 18 or error_stats_count != 18:
        raise SystemExit(
            f"validation 记录数错误: pass={pass_count} stats={error_stats_count}"
        )

    for key in sorted(EXPECTED_CASES):
        case_results = results[key]
        if set(case_results) != BACKENDS:
            raise SystemExit(
                f"backend 不完整: case={key} actual={sorted(case_results)}"
            )

        reference_metrics = {
            (
                value["ref_symbol_errors"],
                value["ref_bit_errors"],
                value["ref_evm2_ppm"],
            )
            for value in case_results.values()
        }
        if len(reference_metrics) != 1:
            raise SystemExit(
                f"三端 reference 不一致: case={key} values={reference_metrics}"
            )

        reference = next(iter(case_results.values()))
        ref_symbol_errors = int(reference["ref_symbol_errors"])
        ref_bit_errors = int(reference["ref_bit_errors"])
        ref_evm2_ppm = int(reference["ref_evm2_ppm"])
        symbols = int(reference["symbols"])
        bits = int(reference["bits"])

        float_result = floats[key]
        if ratio_delta(
            int(float_result["symbol_errors"]), ref_symbol_errors, symbols
        ) > 0.0002:
            raise SystemExit(f"fixed reference SER 偏离 float64: case={key}")
        if ratio_delta(
            int(float_result["bit_errors"]), ref_bit_errors, bits
        ) > 0.0001:
            raise SystemExit(f"fixed reference BER 偏离 float64: case={key}")
        if abs(float(float_result["evm2_ppm"]) - ref_evm2_ppm) > 100.0:
            raise SystemExit(f"fixed reference EVM 偏离 float64: case={key}")

        for backend, value in sorted(case_results.items()):
            if ratio_delta(
                int(value["symbol_errors"]), ref_symbol_errors, symbols
            ) > 0.0002:
                raise SystemExit(f"SER 包络失败: case={key} backend={backend}")
            if ratio_delta(
                int(value["bit_errors"]), ref_bit_errors, bits
            ) > 0.0001:
                raise SystemExit(f"BER 包络失败: case={key} backend={backend}")
            if abs(int(value["evm2_ppm"]) - ref_evm2_ppm) > 100:
                raise SystemExit(f"EVM 包络失败: case={key} backend={backend}")
            if int(value["decision_mismatch_vs_ref"]) / symbols > 0.002:
                raise SystemExit(
                    f"判决不一致率失败: case={key} backend={backend}"
                )
            if not (
                3400 <= int(value["zf_det_q15_min"])
                <= int(value["zf_det_q15_max"])
                <= 4408
            ):
                raise SystemExit(
                    f"reciprocal 支持域失败: case={key} backend={backend}"
                )

    print(
        "MIMO_EQ2_QUALITY_SUMMARY_PASS "
        f"cases=6 backend_results=18 tol={args.max_component_error} "
        "ser_delta_max=0.0002 ber_delta_max=0.0001 "
        "evm2_delta_ppm_max=100 decision_mismatch_rate_max=0.002"
    )


if __name__ == "__main__":
    main()
