#!/usr/bin/env python3
"""Audit the fixed-point OAI/reference dataflow without changing the DUT."""

from __future__ import annotations

import argparse
import json
import re
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable

from generate_fft_corpus import make_values


I16_MIN = -32768
I16_MAX = 32767
I32_MIN = -(1 << 31)
I32_MAX = (1 << 31) - 1
SQRT2_Q15 = 23170
POINTS = (16, 64, 128, 256, 512, 1024, 2048, 4096)


@dataclass
class AuditStats:
    clamp_calls: int = 0
    saturation_count: int = 0
    saturation_high: int = 0
    saturation_low: int = 0
    add_calls: int = 0
    sub_calls: int = 0
    narrow_calls: int = 0
    mul_q15_calls: int = 0
    wide_overflow_count: int = 0
    mulhrs_calls: int = 0
    mulhrs_overflow_count: int = 0

    def clamp(self, value: int) -> int:
        self.clamp_calls += 1
        if value > I16_MAX:
            self.saturation_count += 1
            self.saturation_high += 1
            return I16_MAX
        if value < I16_MIN:
            self.saturation_count += 1
            self.saturation_low += 1
            return I16_MIN
        return value

    def summary(self) -> dict[str, int | float]:
        opportunities = self.clamp_calls + self.mulhrs_calls
        return {
            "clamp_calls": self.clamp_calls,
            "saturation_count": self.saturation_count,
            "saturation_high": self.saturation_high,
            "saturation_low": self.saturation_low,
            "saturation_rate": (
                self.saturation_count / self.clamp_calls
                if self.clamp_calls
                else 0.0
            ),
            "saturating_opportunities": self.clamp_calls,
            "add_calls": self.add_calls,
            "sub_calls": self.sub_calls,
            "narrow_calls": self.narrow_calls,
            "mul_q15_calls": self.mul_q15_calls,
            "wide_overflow_count": self.wide_overflow_count,
            "mulhrs_calls": self.mulhrs_calls,
            "mulhrs_overflow_count": self.mulhrs_overflow_count,
            "overflow_count": self.wide_overflow_count
            + self.mulhrs_overflow_count,
            "all_boundary_opportunities": opportunities,
        }


Complex = tuple[int, int]


def signed_i16(value: int) -> int:
    value &= 0xFFFF
    return value - 0x10000 if value & 0x8000 else value


def neg_wrap(value: int) -> int:
    return signed_i16((~(value & 0xFFFF) + 1) & 0xFFFF)


def add_sat(a: int, b: int, stats: AuditStats) -> int:
    stats.add_calls += 1
    return stats.clamp(a + b)


def sub_sat(a: int, b: int, stats: AuditStats) -> int:
    stats.sub_calls += 1
    return stats.clamp(a - b)


def add_cplx(a: Complex, b: Complex, stats: AuditStats) -> Complex:
    return add_sat(a[0], b[0], stats), add_sat(a[1], b[1], stats)


def sub_cplx(a: Complex, b: Complex, stats: AuditStats) -> Complex:
    return sub_sat(a[0], b[0], stats), sub_sat(a[1], b[1], stats)


def mul_minus_j(value: Complex) -> Complex:
    return value[1], neg_wrap(value[0])


def narrow_q15(value: int, stats: AuditStats) -> int:
    stats.narrow_calls += 1
    if value >= 0:
        shifted = value >> 15
    else:
        shifted = -(((-value) + 32767) >> 15)
    return stats.clamp(shifted)


def mul_q15(a: Complex, tw_re: int, tw_im: int, stats: AuditStats) -> Complex:
    stats.mul_q15_calls += 1
    re_wide = a[0] * tw_re + a[1] * tw_im
    im_wide = a[1] * tw_re - a[0] * tw_im
    if not I32_MIN <= re_wide <= I32_MAX:
        stats.wide_overflow_count += 1
    if not I32_MIN <= im_wide <= I32_MAX:
        stats.wide_overflow_count += 1
    return narrow_q15(re_wide, stats), narrow_q15(im_wide, stats)


def mulhrs(value: int, coeff: int, stats: AuditStats) -> int:
    stats.mulhrs_calls += 1
    shifted = (value * coeff + 0x4000) >> 15
    if shifted > I16_MAX:
        stats.mulhrs_overflow_count += 1
    elif shifted < I16_MIN:
        stats.mulhrs_overflow_count += 1
    return signed_i16(shifted)


def parse_c_array(path: Path, name: str) -> list[int]:
    source = path.read_text(encoding="ascii")
    pattern = re.compile(
        rf"static const int16_t\s+{re.escape(name)}\s*\[[^]]+\][^=]*=\s*\{{(.*?)\}};",
        re.DOTALL,
    )
    match = pattern.search(source)
    if not match:
        raise ValueError(f"array not found: {name} in {path}")
    return [int(token) for token in re.findall(r"-?\d+", match.group(1))]


def load_twiddles(app_dir: Path) -> dict[str, list[int]]:
    table_path = app_dir / "include/fft_pow2_twiddles_precomputed.h"
    scalar_path = app_dir / "dft_scalar.c"
    names = ["fftp_tw64", "fftp_tw128", "fftp_tw256", "fftp_tw512",
             "fftp_tw1024", "fftp_tw2048", "fftp_tw4096"]
    tables = {name: parse_c_array(table_path, name) for name in names}
    tables["tw16_dft"] = parse_c_array(scalar_path, "tw16_dft")
    tables["tw16_idft"] = parse_c_array(scalar_path, "tw16_idft")
    return tables


def transpose4x4(rows: list[list[Complex]]) -> None:
    for i, j in ((1, 0), (2, 0), (3, 0), (2, 1), (3, 1), (3, 2)):
        rows[i][j], rows[j][i] = rows[j][i], rows[i][j]


def dft16(values: list[Complex], inverse: bool, stats: AuditStats,
          tables: dict[str, list[int]]) -> list[Complex]:
    tw = tables["tw16_idft" if inverse else "tw16_dft"]
    rows = [[values[i + 4 * row] for i in range(4)] for row in range(4)]
    t0: list[Complex] = []
    t1: list[Complex] = []
    t2: list[Complex] = []
    t3: list[Complex] = []
    for i in range(4):
        x0, x1, x2, x3 = (rows[row][i] for row in range(4))
        x02 = add_cplx(x0, x2, stats)
        x13 = add_cplx(x1, x3, stats)
        t0.append(add_cplx(x02, x13, stats))
        t2.append(sub_cplx(x02, x13, stats))
        x02 = sub_cplx(x0, x2, stats)
        x13 = sub_cplx(mul_minus_j(x1), mul_minus_j(x3), stats)
        if inverse:
            t3.append(add_cplx(x02, x13, stats))
            t1.append(sub_cplx(x02, x13, stats))
        else:
            t1.append(add_cplx(x02, x13, stats))
            t3.append(sub_cplx(x02, x13, stats))

    rows = [t0, t1, t2, t3]
    transpose4x4(rows)
    for i in range(4):
        rows[1][i] = mul_q15(rows[1][i], tw[2 * i], tw[2 * i + 1], stats)
        rows[2][i] = mul_q15(rows[2][i], tw[8 + 2 * i], tw[9 + 2 * i], stats)
        rows[3][i] = mul_q15(rows[3][i], tw[16 + 2 * i], tw[17 + 2 * i], stats)

    output = [(0, 0)] * 16
    for i in range(4):
        x02 = add_cplx(rows[0][i], rows[2][i], stats)
        x13 = add_cplx(rows[1][i], rows[3][i], stats)
        y0 = add_cplx(x02, x13, stats)
        y2 = sub_cplx(x02, x13, stats)
        x02 = sub_cplx(rows[0][i], rows[2][i], stats)
        x13 = sub_cplx(mul_minus_j(rows[1][i]), mul_minus_j(rows[3][i]), stats)
        if inverse:
            y3 = add_cplx(x02, x13, stats)
            y1 = sub_cplx(x02, x13, stats)
        else:
            y1 = add_cplx(x02, x13, stats)
            y3 = sub_cplx(x02, x13, stats)
        output[i + 0 * 4] = y0
        output[i + 1 * 4] = y1
        output[i + 2 * 4] = y2
        output[i + 3 * 4] = y3
    return output


def split_radix4(values: list[Complex]) -> list[Complex]:
    child = len(values) // 4
    return [values[4 * i + r] for r in range(4) for i in range(child)]


def layout_to_leaf16(values: list[Complex], offset: int = 0,
                     stride: int = 1, points: int | None = None) -> list[Complex]:
    if points is None:
        points = len(values)
    if points == 16:
        return [values[offset + i * stride] for i in range(16)]
    radix = 2 if points in (128, 512, 2048) else 4
    child = points // radix
    output: list[Complex] = []
    for r in range(radix):
        output.extend(layout_to_leaf16(values, offset + r * stride,
                                       stride * radix, child))
    assert len(output) == points
    return output


def combine_radix2(values: list[Complex], child: int, inverse: bool,
                   tw: list[int], stats: AuditStats) -> list[Complex]:
    output = list(values)
    for k in range(child):
        a = values[k]
        b = values[child + k]
        tw_im = neg_wrap(tw[2 * k + 1]) if inverse else tw[2 * k + 1]
        bt = mul_q15(b, tw[2 * k], tw_im, stats)
        output[k] = add_cplx(a, bt, stats)
        output[child + k] = sub_cplx(a, bt, stats)
    return output


def combine_radix4(values: list[Complex], child: int, inverse: bool,
                   tw: list[int], stats: AuditStats) -> list[Complex]:
    output = list(values)
    for k in range(child):
        base = 6 * k
        x0 = values[k]
        x1 = values[child + k]
        x2 = values[2 * child + k]
        x3 = values[3 * child + k]
        x1t = mul_q15(x1, tw[base], neg_wrap(tw[base + 1]) if inverse else tw[base + 1], stats)
        x2t = mul_q15(x2, tw[base + 2], neg_wrap(tw[base + 3]) if inverse else tw[base + 3], stats)
        x3t = mul_q15(x3, tw[base + 4], neg_wrap(tw[base + 5]) if inverse else tw[base + 5], stats)
        x02 = add_cplx(x0, x2t, stats)
        x13 = add_cplx(x1t, x3t, stats)
        y0 = add_cplx(x02, x13, stats)
        y2 = sub_cplx(x02, x13, stats)
        x02d = sub_cplx(x0, x2t, stats)
        x13d = sub_cplx(mul_minus_j(x1t), mul_minus_j(x3t), stats)
        if inverse:
            y3 = add_cplx(x02d, x13d, stats)
            y1 = sub_cplx(x02d, x13d, stats)
        else:
            y1 = add_cplx(x02d, x13d, stats)
            y3 = sub_cplx(x02d, x13d, stats)
        output[k] = y0
        output[child + k] = y1
        output[2 * child + k] = y2
        output[3 * child + k] = y3
    return output


def scale_shift(values: list[Complex], shift: int) -> list[Complex]:
    return [(re >> shift, im >> shift) for re, im in values]


def scale_mulhrs(values: list[Complex], stats: AuditStats) -> list[Complex]:
    return [(mulhrs(re, SQRT2_Q15, stats), mulhrs(im, SQRT2_Q15, stats))
            for re, im in values]


def dft64_from_leaf(values: list[Complex], inverse: bool,
                    stats: AuditStats, tables: dict[str, list[int]]) -> list[Complex]:
    child = 16
    tmp = []
    for r in range(4):
        tmp.extend(dft16(values[r * 16:(r + 1) * 16], inverse, stats, tables))
    return combine_radix4(tmp, child, inverse, tables["fftp_tw64"], stats)


def dft64(values: list[Complex], inverse: bool, stats: AuditStats,
          tables: dict[str, list[int]]) -> list[Complex]:
    return dft64_from_leaf(split_radix4(values), inverse, stats, tables)


def from_leaf(values: list[Complex], points: int, inverse: bool,
              scale: bool, stats: AuditStats,
              tables: dict[str, list[int]]) -> list[Complex]:
    if points == 64:
        output = dft64_from_leaf(values, inverse, stats, tables)
    elif points == 128:
        output = []
        for r in range(2):
            output.extend(from_leaf(values[r * 64:(r + 1) * 64], 64, inverse, True, stats, tables))
        output = combine_radix2(output, 64, inverse, tables["fftp_tw128"], stats)
    elif points == 256:
        output = []
        for r in range(4):
            output.extend(from_leaf(values[r * 64:(r + 1) * 64], 64, inverse, True, stats, tables))
        output = combine_radix4(output, 64, inverse, tables["fftp_tw256"], stats)
    elif points == 512:
        output = []
        for r in range(2):
            output.extend(from_leaf(values[r * 256:(r + 1) * 256], 256, inverse, True, stats, tables))
        output = combine_radix2(output, 256, inverse, tables["fftp_tw512"], stats)
    elif points == 1024:
        output = []
        for r in range(4):
            output.extend(from_leaf(values[r * 256:(r + 1) * 256], 256, inverse, True, stats, tables))
        output = combine_radix4(output, 256, inverse, tables["fftp_tw1024"], stats)
    elif points == 2048:
        output = []
        for r in range(2):
            output.extend(from_leaf(values[r * 1024:(r + 1) * 1024], 1024, inverse, True, stats, tables))
        output = combine_radix2(output, 1024, inverse, tables["fftp_tw2048"], stats)
    elif points == 4096:
        output = []
        for r in range(4):
            output.extend(from_leaf(values[r * 1024:(r + 1) * 1024], 1024, inverse, True, stats, tables))
        output = combine_radix4(output, 1024, inverse, tables["fftp_tw4096"], stats)
    else:
        raise ValueError(f"unsupported from-leaf points: {points}")
    if scale:
        if points == 64:
            output = scale_shift(output, 3)
        elif points in (128, 512, 1024, 2048):
            output = scale_mulhrs(output, stats)
        elif points in (256, 4096):
            output = scale_shift(output, 1)
    return output


def run_reference(values: list[Complex], points: int, inverse: bool,
                  tables: dict[str, list[int]]) -> tuple[list[Complex], AuditStats]:
    stats = AuditStats()
    if points == 16:
        return dft16(values, inverse, stats, tables), stats
    if points == 64:
        return dft64(values, inverse, stats, tables), stats
    laid_out = layout_to_leaf16(values)
    return from_leaf(laid_out, points, inverse, False, stats, tables), stats


def checksum(values: Iterable[Complex]) -> str:
    h = 1469598103934665603
    for re, im in values:
        for value in (re, im):
            raw = value & 0xFFFF
            for byte in (raw & 0xFF, raw >> 8):
                h ^= byte
                h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return f"{h:016x}"


def load_result_keys(*roots: Path) -> dict[tuple[int, int, str], dict]:
    records: dict[tuple[int, int, str], dict] = {}
    for root in roots:
        for path in root.glob("runs/*/correctness/result.json"):
            data = json.loads(path.read_text(encoding="ascii"))
            key = (data["points"], data["idft"], data["metrics"]["corpus"])
            records.setdefault(key, data)
        for path in root.glob("replay-*/correctness/result.json"):
            data = json.loads(path.read_text(encoding="ascii"))
            key = (data["points"], data["idft"], data["metrics"]["corpus"])
            records.setdefault(key, data)
    return records


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--app-dir", type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument("--matrix-root", action="append", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()

    app_dir = args.app_dir.resolve()
    evidence_root = (app_dir / "evidence").resolve()
    output = args.output.resolve()
    if not str(output).startswith(str(evidence_root) + "/"):
        raise SystemExit(f"refusing output outside evidence: {output}")
    if output.exists():
        raise SystemExit(f"refusing to overwrite audit output: {output}")

    tables = load_twiddles(app_dir)
    records = load_result_keys(*(root.resolve() for root in args.matrix_root))
    workloads = []
    failures = []
    for points, idft, corpus in sorted(records):
        raw_values, seed = make_values(corpus, points)
        values = list(zip(raw_values[::2], raw_values[1::2]))
        result, stats = run_reference(values, points, bool(idft), tables)
        model_checksum = checksum(result)
        reference_checksums = set()
        for root in args.matrix_root:
            for path in root.glob("runs/*/correctness/result.json"):
                data = json.loads(path.read_text(encoding="ascii"))
                if (data["points"], data["idft"], data["metrics"]["corpus"]) == (points, idft, corpus):
                    reference_checksums.add(data["metrics"]["ref_checksum"])
            for path in root.glob("replay-*/correctness/result.json"):
                data = json.loads(path.read_text(encoding="ascii"))
                if (data["points"], data["idft"], data["metrics"]["corpus"]) == (points, idft, corpus):
                    reference_checksums.add(data["metrics"]["ref_checksum"])
        exact = len(reference_checksums) == 1 and model_checksum in reference_checksums
        record = {
            "points": points,
            "idft": idft,
            "direction": "IDFT" if idft else "DFT",
            "corpus": corpus,
            "seed": seed,
            "model_checksum": model_checksum,
            "reference_checksums": sorted(reference_checksums),
            "model_matches_oai_checksum": exact,
            "stats": stats.summary(),
        }
        workloads.append(record)
        if not exact:
            failures.append(record)

    report = {
        "audit": "scalar fixed-point reference dataflow",
        "contract": "OAI/reference checksum cross-check; saturation counts are reference-operation counts, not DUT hardware flags",
        "workload_count": len(workloads),
        "checksum_crosscheck_pass": len(failures) == 0,
        "failures": failures,
        "workloads": workloads,
    }
    output.parent.mkdir(parents=True)
    output.write_text(json.dumps(report, indent=2, sort_keys=True) + "\n", encoding="ascii")
    print(json.dumps({"output": str(output), "workload_count": len(workloads), "failures": len(failures)}, sort_keys=True))
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
