#!/usr/bin/env python3
"""Parse exact-validation metrics emitted by the evidence overlay."""

from __future__ import annotations

import argparse
import hashlib
import json
import math
import re
from datetime import datetime, timezone
from pathlib import Path


METRIC_RE = re.compile(r"FFT_EXP_VALIDATION\s+(.+)")


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def parse_metrics(log: str) -> dict[str, str]:
    matches = METRIC_RE.findall(log)
    if not matches:
        raise ValueError("FFT_EXP_VALIDATION marker not found")
    fields: dict[str, str] = {}
    for token in matches[-1].split():
        if "=" in token:
            key, value = token.split("=", 1)
            fields[key] = value
    return fields


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--log", required=True, type=Path)
    parser.add_argument("--output", required=True, type=Path)
    parser.add_argument("--binary", required=True, type=Path)
    parser.add_argument("--source-manifest", required=True, type=Path)
    parser.add_argument("--corpus-manifest", required=True, type=Path)
    parser.add_argument("--impl", required=True)
    parser.add_argument("--points", required=True, type=int)
    parser.add_argument("--idft", required=True, type=int)
    parser.add_argument("--nemu-exit", required=True, type=int)
    args = parser.parse_args()

    log = args.log.read_text(encoding="utf-8", errors="replace")
    fields = parse_metrics(log)
    numeric = {
        key: int(value, 0)
        for key, value in fields.items()
        if key not in {"corpus", "ref_checksum", "dut_checksum"}
    }
    count = args.points * 2
    sum_abs = numeric.get("sum_abs_error", 0)
    sum_sq = numeric.get("sum_sq_error", 0)
    mismatch = numeric.get("mismatching_components", 1)
    exact = mismatch == 0 and numeric.get("mismatching_bits", 1) == 0
    result = {
        "created_at": datetime.now(timezone.utc).isoformat(),
        "impl": args.impl,
        "points": args.points,
        "idft": args.idft,
        "nemu_exit_code": args.nemu_exit,
        "check_pass_marker": "CHECK: PASS" in log,
        "exact_match": exact,
        "status": "PASS" if exact and "CHECK: PASS" in log and args.nemu_exit == 0 else "FAIL",
        "metrics": {
            **fields,
            "mean_abs_error": sum_abs / count,
            "rms_error": math.sqrt(sum_sq / count),
        },
        "sha256": {
            "log": sha256_file(args.log),
            "binary": sha256_file(args.binary),
            "source_manifest": sha256_file(args.source_manifest),
            "corpus_manifest": sha256_file(args.corpus_manifest),
        },
    }
    args.output.write_text(
        json.dumps(result, indent=2, sort_keys=True) + "\n", encoding="ascii"
    )
    print(json.dumps(result, sort_keys=True))
    return 0 if result["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
