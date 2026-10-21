#!/usr/bin/env python3
"""Float64 reference for the deterministic EQ2 communication-quality corpus."""

import argparse
import cmath
import math


Q15 = 32768
LAMBDA = 512 / Q15
SITE_COUNT = 1200 * 13
PHASES = (
    (32767, 0),
    (23170, 23170),
    (0, 32767),
    (-23170, 23170),
    (-32767, 0),
    (-23170, -23170),
    (0, -32767),
    (23170, -23170),
)


def u32(value: int) -> int:
    return value & 0xFFFFFFFF


def mix(site: int, stream: int) -> int:
    value = u32(site ^ u32(stream * 0x9E3779B9))
    value = u32(value ^ (value >> 16))
    value = u32(value * 0x7FEB352D)
    value = u32(value ^ (value >> 15))
    value = u32(value * 0x846CA68B)
    return u32(value ^ (value >> 16))


def centered(site: int, a: int, b: int, span: int) -> int:
    value = u32(site)
    value = u32(value ^ u32(a * 0x45D9F3B))
    value = u32(value + u32(b * 0x9E3779B9))
    value = u32(value ^ (value >> 16))
    value = u32(value * 0x7FEB352D)
    value = u32(value ^ (value >> 15))
    signed = (value & 1023) - 512
    return (signed * span) >> 9


def sat_i16(value: int) -> int:
    return max(-32768, min(32767, value))


def q15_round(value: int) -> int:
    return sat_i16((value + (1 << 14)) >> 15)


def q30_round(value: int) -> int:
    return sat_i16((value + (1 << 29)) >> 30)


def cmul_q15(a: tuple[int, int], b: tuple[int, int]) -> tuple[int, int]:
    ar, ai = a
    br, bi = b
    return q15_round(ar * br - ai * bi), q15_round(ar * bi + ai * br)


def qam_shape(mod_bits: int) -> tuple[int, int]:
    if mod_bits == 2:
        return 2, 8192
    if mod_bits == 4:
        return 4, 4096
    if mod_bits == 6:
        return 8, 2048
    raise ValueError("mod_bits must be 2, 4, or 6")


def qam_component(site: int, tx: int, component: int, mod_bits: int) -> int:
    levels, unit = qam_shape(mod_bits)
    index = mix(site, 17 + tx * 2 + component) & (levels - 1)
    return (2 * index + 1 - levels) * unit


def quality_channel(site: int, cond_profile: int) -> list[tuple[int, int]]:
    singular_values = ((19000, 19000), (23396, 15598), (27033, 13517))
    d1, d2 = singular_values[cond_profile]
    ct, st = PHASES[mix(site, 41) & 7]
    cp, sp = PHASES[mix(site, 43) & 7]
    real_h = (
        q30_round(d1 * ct * cp + d2 * st * sp),
        q30_round(d1 * ct * sp - d2 * st * cp),
        q30_round(d1 * st * cp - d2 * ct * sp),
        q30_round(d1 * st * sp + d2 * ct * cp),
    )
    result: list[tuple[int, int]] = []
    for row in range(2):
        phase = PHASES[mix(site, 47 + row) & 7]
        for col in range(2):
            result.append(cmul_q15((real_h[row * 2 + col], 0), phase))
    return result


def trunc_div2(value: int) -> int:
    return value // 2 if value >= 0 else -((-value) // 2)


def quality_noise(site: int, rx: int, component: int, span: int) -> int:
    total = sum(
        centered(site, 61 + rx * 13 + component * 7 + tap, 79 + tap * 11, span)
        for tap in range(4)
    )
    return trunc_div2(total)


def level_index(value: float, mod_bits: int) -> int:
    levels, unit = qam_shape(mod_bits)
    return min(range(levels), key=lambda index: abs(value - (2 * index + 1 - levels) * unit))


def gray(index: int) -> int:
    return index ^ (index >> 1)


def solve_float(
    h_q15: list[tuple[int, int]],
    y_q15: list[tuple[int, int]],
    mmse: bool,
    solver: str,
) -> tuple[complex, complex]:
    h = [complex(r, i) / Q15 for r, i in h_q15]
    y = [complex(r, i) / Q15 for r, i in y_q15]
    h00, h01, h10, h11 = h
    b0 = h00.conjugate() * y[0] + h10.conjugate() * y[1]
    b1 = h01.conjugate() * y[0] + h11.conjugate() * y[1]
    a00 = abs(h00) ** 2 + abs(h10) ** 2 + (LAMBDA if mmse else 0.0)
    a11 = abs(h01) ** 2 + abs(h11) ** 2 + (LAMBDA if mmse else 0.0)
    a01 = h00.conjugate() * h01 + h10.conjugate() * h11
    if mmse and solver in ("diagonal", "neumann1", "gs1"):
        diagonal = b0 / a00, b1 / a11
        if solver == "diagonal":
            return diagonal
        if solver == "gs1":
            return (
                diagonal[0],
                (b1 - a01.conjugate() * diagonal[0]) / a11,
            )
        return (
            (b0 - a01 * diagonal[1]) / a00,
            (b1 - a01.conjugate() * diagonal[0]) / a11,
        )
    determinant = a00 * a11 - abs(a01) ** 2
    return (
        (a11 * b0 - a01 * b1) / determinant,
        (a00 * b1 - a01.conjugate() * b0) / determinant,
    )


def condition_number(h_q15: list[tuple[int, int]]) -> float:
    h = [complex(r, i) / Q15 for r, i in h_q15]
    h00, h01, h10, h11 = h
    a00 = abs(h00) ** 2 + abs(h10) ** 2
    a11 = abs(h01) ** 2 + abs(h11) ** 2
    a01 = h00.conjugate() * h01 + h10.conjugate() * h11
    root = math.sqrt(max(0.0, (a00 - a11) ** 2 + 4.0 * abs(a01) ** 2))
    largest = (a00 + a11 + root) / 2.0
    smallest = (a00 + a11 - root) / 2.0
    return math.sqrt(largest / smallest)


def wilson(errors: int, total: int) -> tuple[float, float]:
    if total == 0:
        return 0.0, 0.0
    z = 1.959963984540054
    p = errors / total
    denominator = 1.0 + z * z / total
    center = (p + z * z / (2.0 * total)) / denominator
    radius = z * math.sqrt(p * (1.0 - p) / total + z * z / (4.0 * total * total)) / denominator
    return max(0.0, center - radius), min(1.0, center + radius)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--kernel", choices=("eq_zf_2x2", "eq_mmse_2x2"), required=True)
    parser.add_argument("--mod-bits", type=int, choices=(2, 4, 6), required=True)
    parser.add_argument("--noise-q15", type=int, required=True)
    parser.add_argument("--cond-profile", type=int, choices=(0, 1, 2), required=True)
    parser.add_argument(
        "--solver",
        choices=("exact", "diagonal", "neumann1", "gs1"),
        default="exact",
    )
    args = parser.parse_args()

    symbol_errors = 0
    bit_errors = 0
    symbols = 0
    bits = 0
    error_power = 0.0
    signal_power = 0.0
    rx_signal_power = 0
    noise_power = 0
    cond_min = math.inf
    cond_max = 0.0
    cond_sum = 0.0
    det_min = 2**32 - 1
    det_max = 0

    for site in range(SITE_COUNT):
        truth = [
            (qam_component(site, tx, 0, args.mod_bits), qam_component(site, tx, 1, args.mod_bits))
            for tx in range(2)
        ]
        h = quality_channel(site, args.cond_profile)
        y: list[tuple[int, int]] = []
        for rx in range(2):
            yr = 0
            yi = 0
            for tx in range(2):
                hr, hi = cmul_q15(h[rx * 2 + tx], truth[tx])
                yr += hr
                yi += hi
            nr = quality_noise(site, rx, 0, args.noise_q15)
            ni = quality_noise(site, rx, 1, args.noise_q15)
            rx_signal_power += yr * yr + yi * yi
            noise_power += nr * nr + ni * ni
            y.append((sat_i16(yr + nr), sat_i16(yi + ni)))

        p0 = cmul_q15(h[0], h[3])
        p1 = cmul_q15(h[1], h[2])
        dr = p0[0] - p1[0]
        di = p0[1] - p1[1]
        det_q15 = (dr * dr + di * di + (1 << 14)) >> 15
        det_min = min(det_min, det_q15)
        det_max = max(det_max, det_q15)

        cond = condition_number(h)
        cond_min = min(cond_min, cond)
        cond_max = max(cond_max, cond)
        cond_sum += cond
        estimate = solve_float(
            h, y, args.kernel == "eq_mmse_2x2", args.solver
        )
        for tx in range(2):
            value_r = estimate[tx].real * Q15
            value_i = estimate[tx].imag * Q15
            true_r, true_i = truth[tx]
            true_ri = level_index(true_r, args.mod_bits)
            true_ii = level_index(true_i, args.mod_bits)
            value_ri = level_index(value_r, args.mod_bits)
            value_ii = level_index(value_i, args.mod_bits)
            symbols += 1
            bits += args.mod_bits
            symbol_errors += int(value_ri != true_ri or value_ii != true_ii)
            bit_errors += (gray(value_ri) ^ gray(true_ri)).bit_count()
            bit_errors += (gray(value_ii) ^ gray(true_ii)).bit_count()
            error_power += (value_r - true_r) ** 2 + (value_i - true_i) ** 2
            signal_power += true_r * true_r + true_i * true_i

    ser_low, ser_high = wilson(symbol_errors, symbols)
    ber_low, ber_high = wilson(bit_errors, bits)
    evm2_ppm = error_power * 1_000_000.0 / signal_power
    snr_db = math.inf if noise_power == 0 else 10.0 * math.log10(rx_signal_power / noise_power)
    print(
        "MIMO_EQ2_FLOAT_QUALITY "
        f"kernel={args.kernel} mod_bits={args.mod_bits} noise_q15={args.noise_q15} "
        f"cond_profile={args.cond_profile} solver={args.solver} "
        f"symbols={symbols} symbol_errors={symbol_errors} "
        f"ser={symbol_errors / symbols:.9g} ser_ci95_low={ser_low:.9g} ser_ci95_high={ser_high:.9g} "
        f"bits={bits} bit_errors={bit_errors} ber={bit_errors / bits:.9g} "
        f"ber_ci95_low={ber_low:.9g} ber_ci95_high={ber_high:.9g} "
        f"evm2_ppm={evm2_ppm:.6f} evm_pct={math.sqrt(evm2_ppm / 100.0):.6f} "
        f"rx_snr_db={snr_db:.6f} cond_min={cond_min:.6f} "
        f"cond_mean={cond_sum / SITE_COUNT:.6f} cond_max={cond_max:.6f} "
        f"zf_det_q15_min={det_min} zf_det_q15_max={det_max}"
    )


if __name__ == "__main__":
    main()
