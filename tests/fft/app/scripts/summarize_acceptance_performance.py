#!/usr/bin/env python3
"""Pair RVV/extension FFT cycles and enforce the acceptance threshold."""

from __future__ import annotations

import argparse
import csv
import json
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--matrix", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--threshold", type=float, default=30.0)
    parser.add_argument(
        "--expected-configurations",
        type=int,
        default=14,
        help="number of point/direction pairs expected in the input matrix",
    )
    args = parser.parse_args()

    cycles: dict[tuple[int, int], dict[str, float]] = {}
    with args.matrix.open(encoding="utf-8", newline="") as stream:
        for row in csv.DictReader(stream, delimiter="\t"):
            if row["status"] != "PASS":
                raise SystemExit(f"FFT配置未通过: {row}")
            key = (int(row["points"]), int(row["idft"]))
            result_path = Path(row["child_root"]) / "official" / "result.json"
            result = json.loads(result_path.read_text(encoding="utf-8"))
            repeat = int(result["roi"]["repeat"])
            total_cycle = int(result["roi"]["cycle"])
            if repeat <= 0:
                raise SystemExit(f"FFT配置repeat非法: {row}")
            cycles.setdefault(key, {})[row["impl"]] = total_cycle / repeat

    failures = 0
    args.output.parent.mkdir(parents=True, exist_ok=True)
    with args.output.open("w", encoding="utf-8", newline="") as stream:
        writer = csv.writer(stream, delimiter="\t", lineterminator="\n")
        writer.writerow(
            [
                "points",
                "transform",
                "rvv_cycle",
                "rvv_extension_cycle",
                "reduction_percent",
                "status",
            ]
        )
        for (points, idft), values in sorted(cycles.items()):
            if set(values) != {"dft_rvv", "dft_ext"}:
                raise SystemExit(
                    f"FFT配置缺少RVV或扩展结果: points={points} idft={idft}"
                )
            rvv = values["dft_rvv"]
            ext = values["dft_ext"]
            reduction = (rvv - ext) * 100.0 / rvv
            status = "PASS" if reduction >= args.threshold else "FAIL"
            failures += status == "FAIL"
            writer.writerow(
                [
                    points,
                    "IDFT" if idft else "DFT",
                    f"{rvv:.3f}".rstrip("0").rstrip("."),
                    f"{ext:.3f}".rstrip("0").rstrip("."),
                    f"{reduction:.2f}",
                    status,
                ]
            )

    if len(cycles) != args.expected_configurations:
        raise SystemExit(
            "FFT性能矩阵数量错误: "
            f"expected={args.expected_configurations} actual={len(cycles)}"
        )
    if failures:
        raise SystemExit(f"FFT性能阈值失败: {failures}项低于{args.threshold:.2f}%")
    print(
        "FFT_PERFORMANCE_SUMMARY_PASS "
        f"cases={len(cycles)} threshold={args.threshold:.2f} output={args.output}"
    )


if __name__ == "__main__":
    main()
