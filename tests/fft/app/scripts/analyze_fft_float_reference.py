#!/usr/bin/env python3
"""Compare the bit-exact fixed-point reference with an ideal binary64 FFT."""

from __future__ import annotations

import argparse
import json
import math
from pathlib import Path

from analyze_fft_reference_saturation import (
    checksum,
    load_result_keys,
    load_twiddles,
    run_reference,
)
from generate_fft_corpus import make_values


POINTS = (16, 64, 128, 256, 512, 1024, 2048, 4096)
CORPORA = (
    "C0-legacy-pattern-v1",
    "C1-impulse-v1",
    "C2-dc-alternating-v1",
    "C3-prng-q15-v1",
    "C4-near-limit-v1",
)
SQRT2_Q15 = 23170


SCALE_SCHEDULES: dict[int, dict[str, object]] = {
    16: {"factor": 1.0, "stages": []},
    64: {"factor": 1.0, "stages": []},
    128: {"factor": 1.0 / 8.0, "stages": ["child64 arithmetic >>3"]},
    256: {"factor": 1.0 / 8.0, "stages": ["child64 arithmetic >>3"]},
    512: {
        "factor": 1.0 / 16.0,
        "stages": ["child64 arithmetic >>3", "child256 arithmetic >>1"],
    },
    1024: {
        "factor": 1.0 / 16.0,
        "stages": ["child64 arithmetic >>3", "child256 arithmetic >>1"],
    },
    2048: {
        "factor": (1.0 / 16.0) * (SQRT2_Q15 / 32768.0),
        "stages": [
            "child64 arithmetic >>3",
            "child256 arithmetic >>1",
            "child1024 mulhrs(23170)",
        ],
    },
    4096: {
        "factor": (1.0 / 16.0) * (SQRT2_Q15 / 32768.0),
        "stages": [
            "child64 arithmetic >>3",
            "child256 arithmetic >>1",
            "child1024 mulhrs(23170)",
        ],
    },
}


def fft_binary64(values: list[complex], inverse: bool) -> list[complex]:
    """Unnormalized radix-2 DFT/IDFT using Python binary64 components."""
    size = len(values)
    if size < 1 or size & (size - 1):
        raise ValueError(f"FFT size must be a positive power of two: {size}")

    output = list(values)
    reverse = 0
    for index in range(1, size):
        bit = size >> 1
        while reverse & bit:
            reverse ^= bit
            bit >>= 1
        reverse ^= bit
        if index < reverse:
            output[index], output[reverse] = output[reverse], output[index]

    stage = 2
    sign = 1.0 if inverse else -1.0
    while stage <= size:
        angle = sign * 2.0 * math.pi / stage
        root = complex(math.cos(angle), math.sin(angle))
        half = stage >> 1
        for start in range(0, size, stage):
            twiddle = 1.0 + 0.0j
            for offset in range(half):
                even = output[start + offset]
                odd = output[start + offset + half] * twiddle
                output[start + offset] = even + odd
                output[start + offset + half] = even - odd
                twiddle *= root
        stage <<= 1
    return output


def direct_dft_binary64(values: list[complex], inverse: bool) -> list[complex]:
    """Small-size O(N^2) cross-check independent of the radix-2 schedule."""
    size = len(values)
    sign = 1.0 if inverse else -1.0
    output: list[complex] = []
    for freq in range(size):
        total = 0.0 + 0.0j
        for sample, value in enumerate(values):
            angle = sign * 2.0 * math.pi * freq * sample / size
            total += value * complex(math.cos(angle), math.sin(angle))
        output.append(total)
    return output


def finite_ratio(numerator: float, denominator: float) -> float | None:
    return numerator / denominator if denominator else None


def db20(value: float | None) -> float | None:
    if value is None:
        return None
    if value == 0.0:
        return None
    return 20.0 * math.log10(value)


def db10(value: float | None) -> float | None:
    if value is None:
        return None
    if value == 0.0:
        return None
    return 10.0 * math.log10(value)


def workload_metrics(
    raw_values: list[int],
    fixed_values: list[tuple[int, int]],
    points: int,
    inverse: bool,
) -> dict[str, object]:
    inputs = [complex(re, im) for re, im in zip(raw_values[::2], raw_values[1::2])]
    ideal_unscaled = fft_binary64(inputs, inverse)
    scale = float(SCALE_SCHEDULES[points]["factor"])
    ideal = [value * scale for value in ideal_unscaled]
    fixed = [complex(re, im) for re, im in fixed_values]

    component_abs_errors: list[float] = []
    complex_error_energy = 0.0
    ideal_energy = 0.0
    fixed_energy = 0.0
    ideal_out_of_i16_components = 0
    for expected, actual in zip(ideal, fixed):
        error = actual - expected
        component_abs_errors.extend((abs(error.real), abs(error.imag)))
        complex_error_energy += error.real * error.real + error.imag * error.imag
        ideal_energy += expected.real * expected.real + expected.imag * expected.imag
        fixed_energy += actual.real * actual.real + actual.imag * actual.imag
        ideal_out_of_i16_components += int(
            expected.real < -32768.0 or expected.real > 32767.0
        )
        ideal_out_of_i16_components += int(
            expected.imag < -32768.0 or expected.imag > 32767.0
        )

    component_sq_error = sum(value * value for value in component_abs_errors)
    component_count = len(component_abs_errors)
    input_energy = sum(value.real * value.real + value.imag * value.imag for value in inputs)
    parseval_expected_energy = points * input_energy * scale * scale
    evm_squared = finite_ratio(complex_error_energy, ideal_energy)
    evm_rms = math.sqrt(evm_squared) if evm_squared is not None else None
    energy_ratio = finite_ratio(fixed_energy, ideal_energy)

    direct_crosscheck_max_abs: float | None = None
    if points <= 64:
        direct = direct_dft_binary64(inputs, inverse)
        direct_crosscheck_max_abs = max(
            abs(radix2 - exact) for radix2, exact in zip(ideal_unscaled, direct)
        )

    return {
        "nominal_scale_factor": scale,
        "scale_stages": SCALE_SCHEDULES[points]["stages"],
        "component_count": component_count,
        "component_max_abs_error": max(component_abs_errors, default=0.0),
        "component_mean_abs_error": (
            sum(component_abs_errors) / component_count if component_count else 0.0
        ),
        "component_rms_error": (
            math.sqrt(component_sq_error / component_count) if component_count else 0.0
        ),
        "complex_error_energy": complex_error_energy,
        "evm_squared": evm_squared,
        "evm_rms": evm_rms,
        "evm_db": db20(evm_rms),
        "input_energy": input_energy,
        "ideal_scaled_energy": ideal_energy,
        "fixed_energy": fixed_energy,
        "energy_ratio_fixed_to_ideal": energy_ratio,
        "energy_ratio_db": db10(energy_ratio),
        "parseval_expected_scaled_energy": parseval_expected_energy,
        "float_parseval_relative_delta": finite_ratio(
            ideal_energy - parseval_expected_energy, parseval_expected_energy
        ),
        "fixed_parseval_relative_delta": finite_ratio(
            fixed_energy - parseval_expected_energy, parseval_expected_energy
        ),
        "ideal_out_of_i16_components": ideal_out_of_i16_components,
        "fixed_endpoint_components": sum(
            value in (-32768, 32767) for pair in fixed_values for value in pair
        ),
        "float_radix2_direct_max_complex_delta": direct_crosscheck_max_abs,
    }


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--app-dir", type=Path, default=Path(__file__).resolve().parents[1]
    )
    parser.add_argument("--matrix-root", action="append", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()

    app_dir = args.app_dir.resolve()
    evidence_root = (app_dir / "evidence").resolve()
    output = args.output.resolve()
    if not str(output).startswith(str(evidence_root) + "/"):
        raise SystemExit(f"refusing output outside evidence: {output}")
    if output.exists():
        raise SystemExit(f"refusing to overwrite float reference report: {output}")

    roots = [root.resolve() for root in args.matrix_root]
    records = load_result_keys(*roots)
    expected_keys = {
        (points, inverse, corpus)
        for points in POINTS
        for inverse in (0, 1)
        for corpus in CORPORA
    }
    missing = sorted(expected_keys - set(records))
    unexpected = sorted(set(records) - expected_keys)
    if missing or unexpected:
        raise SystemExit(
            f"incomplete correctness records: missing={len(missing)} "
            f"unexpected={len(unexpected)}"
        )

    tables = load_twiddles(app_dir)
    workloads: list[dict[str, object]] = []
    checksum_failures: list[dict[str, object]] = []
    for points, inverse, corpus in sorted(expected_keys):
        raw_values, seed = make_values(corpus, points)
        values = list(zip(raw_values[::2], raw_values[1::2]))
        fixed_values, _ = run_reference(values, points, bool(inverse), tables)
        fixed_checksum = checksum(fixed_values)
        oai_checksum = records[(points, inverse, corpus)]["metrics"]["ref_checksum"]
        checksum_match = fixed_checksum == oai_checksum
        record = {
            "points": points,
            "idft": inverse,
            "direction": "IDFT" if inverse else "DFT",
            "corpus": corpus,
            "seed": seed,
            "fixed_model_checksum": fixed_checksum,
            "oai_reference_checksum": oai_checksum,
            "fixed_model_matches_oai": checksum_match,
            "metrics": workload_metrics(
                raw_values, fixed_values, points, bool(inverse)
            ),
        }
        workloads.append(record)
        if not checksum_match:
            checksum_failures.append(record)

    direct_deltas = [
        record["metrics"]["float_radix2_direct_max_complex_delta"]
        for record in workloads
        if record["metrics"]["float_radix2_direct_max_complex_delta"] is not None
    ]
    max_direct_delta = max(direct_deltas, default=0.0)
    direct_crosscheck_pass = max_direct_delta <= 1.0e-6
    worst_component = max(
        workloads,
        key=lambda item: item["metrics"]["component_max_abs_error"],
    )
    worst_evm = max(
        workloads,
        key=lambda item: item["metrics"]["evm_rms"] or 0.0,
    )

    report = {
        "audit": "binary64 ideal DFT/IDFT versus bit-exact fixed-point reference",
        "contract": (
            "diagnostic only; OAI/reference remains the sole bit-exact correctness gate"
        ),
        "float_implementation": (
            "Python complex with binary64 real/imag components; unnormalized radix-2 FFT"
        ),
        "scale_schedule_source": (
            "include/fft_pow2_scalar_impl.h; nominal linear factors use exact Q15 "
            "coefficient 23170/32768 where applicable"
        ),
        "scale_schedules": SCALE_SCHEDULES,
        "workload_count": len(workloads),
        "fixed_model_oai_crosscheck_pass": not checksum_failures,
        "fixed_model_oai_failures": checksum_failures,
        "float_radix2_direct_crosscheck": {
            "scope": "N=16 and N=64, all corpora and both directions",
            "threshold": 1.0e-6,
            "max_complex_delta": max_direct_delta,
            "pass": direct_crosscheck_pass,
        },
        "summary": {
            "workloads_with_ideal_component_outside_i16": sum(
                record["metrics"]["ideal_out_of_i16_components"] > 0
                for record in workloads
            ),
            "max_abs_float_parseval_relative_delta": max(
                abs(record["metrics"]["float_parseval_relative_delta"] or 0.0)
                for record in workloads
            ),
            "worst_component_error": {
                "points": worst_component["points"],
                "direction": worst_component["direction"],
                "corpus": worst_component["corpus"],
                "value": worst_component["metrics"]["component_max_abs_error"],
            },
            "worst_evm": {
                "points": worst_evm["points"],
                "direction": worst_evm["direction"],
                "corpus": worst_evm["corpus"],
                "value": worst_evm["metrics"]["evm_rms"],
                "db": worst_evm["metrics"]["evm_db"],
            },
        },
        "workloads": workloads,
    }
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(
        json.dumps(report, indent=2, sort_keys=True) + "\n", encoding="ascii"
    )
    print(
        json.dumps(
            {
                "output": str(output),
                "workloads": len(workloads),
                "oai_crosscheck_failures": len(checksum_failures),
                "float_direct_crosscheck_pass": direct_crosscheck_pass,
            },
            sort_keys=True,
        )
    )
    return 0 if not checksum_failures and direct_crosscheck_pass else 1


if __name__ == "__main__":
    raise SystemExit(main())
