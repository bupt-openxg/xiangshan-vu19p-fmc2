#!/usr/bin/env python3
"""Generate deterministic FFT validation inputs and a header-only input overlay."""

from __future__ import annotations

import argparse
import hashlib
import json
import struct
from pathlib import Path


POINTS = (16, 64, 128, 256, 512, 1024, 2048, 4096)
CORPORA = {
    "C0-legacy-pattern-v1",
    "C1-impulse-v1",
    "C2-dc-alternating-v1",
    "C3-prng-q15-v1",
    "C4-near-limit-v1",
}


def c_rem(value: int, divisor: int) -> int:
    """C99 signed remainder for positive divisor."""
    quotient = abs(value) // divisor
    if value < 0:
        quotient = -quotient
    return value - quotient * divisor


def xorshift32(state: int) -> int:
    state &= 0xFFFFFFFF
    state ^= (state << 13) & 0xFFFFFFFF
    state ^= state >> 17
    state ^= (state << 5) & 0xFFFFFFFF
    return state & 0xFFFFFFFF


def c0_input(points: int) -> list[int]:
    if points == 16:
        return [
            5, -8, 16, 2, 29, 13, 28, 25,
            36, 17, 59, 29, 53, 52, 57, 22,
            -1, 13, -7, 23, 12, 32, 18, 41,
            14, 41, 22, 48, 33, 12, 49, 19,
        ]

    values: list[int] = []
    for i in range(points):
        real_part = (
            ((i * 17) % 97)
            + ((i * 11) % 29)
            - ((i * 7) % 31)
            + (i % 9) * 3
            - (i % 5) * 4
            - 48
        )
        imag_part = (
            ((i * 13) % 89)
            - ((i * 19) % 43)
            + (i % 7) * 5
            - (i % 11) * 3
            + ((i // 3) % 17) * 2
            - 40
        )
        values.extend((c_rem(real_part, 64), c_rem(imag_part, 64)))
    return values


def c1_input(points: int) -> list[int]:
    values = [0] * (2 * points)
    values[0] = 16384
    return values


def c2_input(points: int) -> list[int]:
    values: list[int] = []
    for i in range(points):
        real = -8192 if (i & 1) else 8192
        imag = -4096 if ((i >> 1) & 1) else 4096
        values.extend((real, imag))
    return values


def c3_input(points: int, seed: int) -> list[int]:
    state = seed
    values: list[int] = []
    for _ in range(2 * points):
        state = xorshift32(state)
        values.append(((state >> 16) & 0x7FFF) - 16384)
    return values


def c4_input(points: int, seed: int) -> list[int]:
    values: list[int] = []
    del seed
    for i in range(points):
        re_mag = 24576 + ((i * 257) % 2048)
        im_mag = 24576 + ((i * 131) % 2048)
        real = -re_mag if (i & 1) else re_mag
        imag = -im_mag if (i & 2) else im_mag
        values.extend((real, imag))
    return values


def make_values(corpus_id: str, points: int) -> tuple[list[int], int | None]:
    if corpus_id == "C0-legacy-pattern-v1":
        return c0_input(points), None
    if corpus_id == "C1-impulse-v1":
        return c1_input(points), None
    if corpus_id == "C2-dc-alternating-v1":
        return c2_input(points), None
    if corpus_id == "C3-prng-q15-v1":
        seed = 0x20260715
        return c3_input(points, seed), seed
    if corpus_id == "C4-near-limit-v1":
        seed = 0x20260716
        return c4_input(points, seed), seed
    raise ValueError(f"unsupported corpus: {corpus_id}")


def sha256_bytes(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def endpoint_count(values: list[int]) -> int:
    return sum(value in (-32768, 32767) for value in values)


def render_data_header(values: list[int], corpus_id: str, points: int) -> str:
    lines = [
        "#ifndef FFT_EXP_CORPUS_DATA_H",
        "#define FFT_EXP_CORPUS_DATA_H",
        "#include <stdint.h>",
        f'#define FFT_EXP_CORPUS_ID "{corpus_id}"',
        f"#define FFT_EXP_CORPUS_POINTS {points}",
        f"#define FFT_EXP_INPUT_ENDPOINT_COUNT {endpoint_count(values)}",
        f"static const int16_t fft_exp_corpus_data[{len(values)}] = {{",
    ]
    for start in range(0, len(values), 8):
        chunk = ", ".join(str(value) for value in values[start : start + 8])
        lines.append(f"    {chunk},")
    lines.extend(["};", "#endif", ""])
    return "\n".join(lines)


def render_overlay_header() -> str:
    return r'''#ifndef FFT_EXP_TEST_COMMON_OVERLAY_H
#define FFT_EXP_TEST_COMMON_OVERLAY_H

/* Rename the original helpers while importing the unchanged baseline header. */
#define fft_load_test_input fft_load_test_input_default
#define fft_compare_complex_vector fft_compare_complex_vector_default
#include_next "fft_test_common.h"
#undef fft_load_test_input
#undef fft_compare_complex_vector

#include "fft_corpus_data.h"

#ifndef FFT_EXP_FAIL_PRINT_LIMIT
#define FFT_EXP_FAIL_PRINT_LIMIT 8
#endif

static inline int fft_load_test_input(int16_t *dst, int complex_n)
{
    if (complex_n != FFT_EXP_CORPUS_POINTS) {
        return 0;
    }
    for (int i = 0; i < complex_n * 2; ++i) {
        dst[i] = fft_exp_corpus_data[i];
    }
    return 1;
}

static inline uint64_t fft_exp_checksum(const int16_t *data, int complex_n)
{
    uint64_t hash = 1469598103934665603ULL;
    for (int i = 0; i < complex_n * 2; ++i) {
        uint16_t value = (uint16_t)data[i];
        hash ^= (uint8_t)(value & 0xffu);
        hash *= 1099511628211ULL;
        hash ^= (uint8_t)(value >> 8);
        hash *= 1099511628211ULL;
    }
    return hash;
}

static inline uint64_t fft_exp_endpoint_count(const int16_t *data,
                                               int complex_n)
{
    uint64_t count = 0;
    for (int i = 0; i < complex_n * 2; ++i) {
        count += (data[i] == -32768 || data[i] == 32767);
    }
    return count;
}

static inline unsigned int fft_exp_popcount16(uint16_t value)
{
    unsigned int count = 0;
    for (unsigned int bit = 0; bit < 16; ++bit) {
        count += (value >> bit) & 1u;
    }
    return count;
}

static inline int fft_compare_complex_vector(const int16_t *ref,
                                             const int16_t *dut,
                                             int complex_n)
{
    uint64_t mismatching_components = 0;
    uint64_t mismatching_bits = 0;
    uint64_t max_abs_error = 0;
    uint64_t sum_abs_error = 0;
    uint64_t sum_sq_error = 0;
    uint64_t printed = 0;

    for (int i = 0; i < complex_n * 2; ++i) {
        int32_t delta = (int32_t)dut[i] - (int32_t)ref[i];
        uint32_t abs_delta = (uint32_t)(delta < 0 ? -delta : delta);
        if (delta != 0) {
            mismatching_components++;
            mismatching_bits += (uint64_t)fft_exp_popcount16(
                (uint16_t)ref[i] ^ (uint16_t)dut[i]);
            if (printed < FFT_EXP_FAIL_PRINT_LIMIT) {
                printf("FFT_EXP_MISMATCH component=%d ref=%d dut=%d\n",
                       i, ref[i], dut[i]);
                printed++;
            }
        }
        if (abs_delta > max_abs_error) {
            max_abs_error = abs_delta;
        }
        sum_abs_error += abs_delta;
        sum_sq_error += (uint64_t)abs_delta * (uint64_t)abs_delta;
    }

    printf("FFT_EXP_VALIDATION corpus=%s points=%d mismatching_components=%llu "
           "mismatching_bits=%llu max_abs_error=%llu sum_abs_error=%llu "
           "sum_sq_error=%llu ref_checksum=%016llx dut_checksum=%016llx "
           "ref_endpoint_count=%llu dut_endpoint_count=%llu "
           "input_endpoint_count=%u\n",
           FFT_EXP_CORPUS_ID,
           complex_n,
           (unsigned long long)mismatching_components,
           (unsigned long long)mismatching_bits,
           (unsigned long long)max_abs_error,
           (unsigned long long)sum_abs_error,
           (unsigned long long)sum_sq_error,
           (unsigned long long)fft_exp_checksum(ref, complex_n),
           (unsigned long long)fft_exp_checksum(dut, complex_n),
           (unsigned long long)fft_exp_endpoint_count(ref, complex_n),
           (unsigned long long)fft_exp_endpoint_count(dut, complex_n),
           (unsigned int)FFT_EXP_INPUT_ENDPOINT_COUNT);
    return mismatching_components == 0;
}

#endif
'''


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--corpus", required=True, choices=sorted(CORPORA))
    parser.add_argument("--points", required=True, type=int, choices=POINTS)
    parser.add_argument("--output", required=True, type=Path)
    parser.add_argument("--overlay", required=True, type=Path)
    args = parser.parse_args()

    if args.output.exists() or args.overlay.exists():
        raise SystemExit("refusing to overwrite existing corpus or overlay directory")

    values, seed = make_values(args.corpus, args.points)
    raw = b"".join(struct.pack("<h", value) for value in values)
    args.output.mkdir(parents=True)
    args.overlay.mkdir(parents=True)
    (args.output / "input.bin").write_bytes(raw)
    (args.output / "fft_corpus_data.h").write_text(
        render_data_header(values, args.corpus, args.points), encoding="ascii"
    )
    (args.overlay / "fft_corpus_data.h").write_text(
        render_data_header(values, args.corpus, args.points), encoding="ascii"
    )
    (args.overlay / "fft_test_common.h").write_text(
        render_overlay_header(), encoding="ascii"
    )
    metadata = {
        "corpus_id": args.corpus,
        "points": args.points,
        "seed": seed,
        "direction_independent": True,
        "component_count": len(values),
        "input_endpoint_count": endpoint_count(values),
        "input_sha256": sha256_bytes(raw),
        "generator": "scripts/generate_fft_corpus.py",
        "generator_version": "v2-contract-aligned",
    }
    (args.output / "corpus.json").write_text(
        json.dumps(metadata, indent=2, sort_keys=True) + "\n", encoding="ascii"
    )
    print(json.dumps(metadata, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
