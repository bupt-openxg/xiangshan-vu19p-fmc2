#!/usr/bin/env python3
"""Parse one XiangShan EMU FFT report into a reproducible JSON record."""

from __future__ import annotations

import argparse
import hashlib
import json
import re
from datetime import datetime, timezone
from pathlib import Path


ANSI_RE = re.compile(r"\x1b\[[0-9;]*m")
ROI_RE = re.compile(r"FFT_ROI cycle=(\d+) instret=(\d+) repeat=(\d+)")
HPM_RE = re.compile(
    r"FFT_ROI_HPM group=([A-Za-z0-9_-]+) event=([A-Za-z0-9_.-]+) value=(\d+)"
)
SUMMARY_RE = re.compile(
    r"Core-0 instrCnt = ([0-9,]+), cycleCnt = ([0-9,]+), IPC = ([0-9.]+)"
)


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--report", required=True, type=Path)
    parser.add_argument("--output", required=True, type=Path)
    parser.add_argument("--binary", required=True, type=Path)
    parser.add_argument("--source-manifest", required=True, type=Path)
    parser.add_argument("--corpus-manifest", required=True, type=Path)
    parser.add_argument("--impl", required=True)
    parser.add_argument("--points", required=True, type=int)
    parser.add_argument("--idft", required=True, type=int)
    parser.add_argument("--corpus", required=True)
    parser.add_argument("--warmup", required=True, type=int)
    parser.add_argument("--repeat", required=True, type=int)
    parser.add_argument("--emu-exit", required=True, type=int)
    args = parser.parse_args()

    text = ANSI_RE.sub("", args.report.read_text(encoding="utf-8", errors="replace"))
    roi_matches = list(ROI_RE.finditer(text))
    hpm = {
        f"{match.group(1)}.{match.group(2)}": int(match.group(3))
        for match in HPM_RE.finditer(text)
    }
    summary_matches = list(SUMMARY_RE.finditer(text))
    if not roi_matches:
        raise SystemExit("FFT_ROI marker not found")
    roi = roi_matches[-1]
    roi_cycle = int(roi.group(1))
    roi_instret = int(roi.group(2))
    roi_repeat = int(roi.group(3))
    whole = None
    if summary_matches:
        match = summary_matches[-1]
        whole = {
            "instret": int(match.group(1).replace(",", "")),
            "cycle": int(match.group(2).replace(",", "")),
            "ipc": float(match.group(3)),
        }
    good_trap = "HIT GOOD TRAP" in text
    result = {
        "created_at": datetime.now(timezone.utc).isoformat(),
        "impl": args.impl,
        "points": args.points,
        "idft": args.idft,
        "direction": "IDFT" if args.idft else "DFT",
        "corpus": args.corpus,
        "warmup": args.warmup,
        "repeat_requested": args.repeat,
        "emu_exit_code": args.emu_exit,
        "good_trap": good_trap,
        "roi": {
            "cycle": roi_cycle,
            "instret": roi_instret,
            "repeat": roi_repeat,
            "cycle_per_repeat": roi_cycle / roi_repeat if roi_repeat else None,
            "instret_per_repeat": roi_instret / roi_repeat if roi_repeat else None,
            "hpm": hpm,
        },
        "whole_program": whole,
        "status": "PASS" if args.emu_exit == 0 and good_trap else "FAIL",
        "sha256": {
            "report": sha256_file(args.report),
            "binary": sha256_file(args.binary),
            "source_manifest": sha256_file(args.source_manifest),
            "corpus_manifest": sha256_file(args.corpus_manifest),
        },
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    if args.output.exists():
        raise SystemExit(f"refusing to overwrite parser output: {args.output}")
    args.output.write_text(json.dumps(result, indent=2, sort_keys=True) + "\n", encoding="ascii")
    print(json.dumps(result, sort_keys=True))
    return 0 if result["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
