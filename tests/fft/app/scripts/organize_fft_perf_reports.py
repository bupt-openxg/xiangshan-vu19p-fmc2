#!/usr/bin/env python3
from __future__ import annotations

import argparse
from pathlib import Path

from fft_perf_layout import classify_artifact, target_path_for_artifact


def remove_empty_dirs(root: Path) -> None:
    for path in sorted((p for p in root.rglob("*") if p.is_dir()), key=lambda p: len(p.parts), reverse=True):
        if any(path.iterdir()):
            continue
        path.rmdir()


def organize_run_dir(run_dir: Path) -> int:
    files = sorted(p for p in run_dir.rglob("*") if p.is_file())
    moved = 0

    for path in files:
        artifact = classify_artifact(path)
        if artifact is None:
            continue

        target = target_path_for_artifact(run_dir, artifact)
        if path == target:
            continue

        target.parent.mkdir(parents=True, exist_ok=True)
        if target.exists():
            raise FileExistsError(f"target already exists: {target}")
        path.rename(target)
        moved += 1

    remove_empty_dirs(run_dir)
    return moved


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--run-dir",
        required=True,
        help="perf_eval root or any subtree that already follows the n<points>/<version> layout",
    )
    args = parser.parse_args()

    run_dir = Path(args.run_dir)
    moved = organize_run_dir(run_dir)
    print(f"organized {moved} file(s) under {run_dir}")


if __name__ == "__main__":
    main()
