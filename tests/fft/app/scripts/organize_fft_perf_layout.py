#!/usr/bin/env python3
import argparse
import re
import shutil
from dataclasses import dataclass
from pathlib import Path


KNOWN_SUFFIXES = (
    ".analysis.md",
    ".report.txt",
    ".validate-nemu.txt",
    ".validate.txt",
    ".perf-nemu.txt",
    ".nemu.txt",
)
POINT_PREFIX_RE = re.compile(r"n(?P<points>[0-9]+)-r(?P<repeat>[0-9]+)-idft(?P<idft>[01])-(?P<label>.+)")
POINT_FROM_TASK_RE = re.compile(r"(?:^|-)dft(?P<points>[0-9]+)(?:-|$)")
VERSIONS = {"oai", "scalar", "rvv"}


@dataclass
class TaskContext:
    task_dir: Path
    default_points: int | None
    has_scalar_mainline: bool
    has_scalar_feopt: bool


def split_stem_suffix(name: str) -> tuple[str | None, str | None]:
    for suffix in KNOWN_SUFFIXES:
        if name.endswith(suffix):
            return name[: -len(suffix)], suffix
    return None, None


def normalize_version_impl(label: str, ctx: TaskContext) -> tuple[str | None, str | None]:
    if label == "oai":
        return "oai", "minimal"
    if label == "scalar":
        if ctx.has_scalar_mainline or ctx.has_scalar_feopt:
            return "scalar", "baseline"
        return "scalar", "mainline"
    if label == "rvv":
        return "rvv", "mainline"

    if "-" not in label:
        return None, None

    version, impl = label.split("-", 1)
    if version not in VERSIONS:
        return None, None
    return version, impl


def classify_destination(path: Path, ctx: TaskContext) -> Path | None:
    stem, suffix = split_stem_suffix(path.name)
    if stem is None or suffix is None:
        return None

    if path.name == "pow2-dft-summary.analysis.md":
        return ctx.task_dir / path.name

    points: int | None = None
    label = stem

    match = POINT_PREFIX_RE.fullmatch(stem)
    if match:
        points = int(match.group("points"))
        label = match.group("label")
    else:
        if ctx.default_points is None:
            return None
        points = ctx.default_points
        if "." in stem:
            label = stem.split(".", 1)[0]

    point_dir = ctx.task_dir / f"n{points}"

    if label == "comparison":
        return point_dir / path.name

    if "-vs-" in label:
        version = label.split("-", 1)[0]
        if version in VERSIONS:
            return point_dir / version / path.name
        return point_dir / path.name

    version, impl = normalize_version_impl(label, ctx)
    if version is None or impl is None:
        return None

    kind_dir = "analysis" if suffix == ".analysis.md" else "sim"
    return point_dir / version / kind_dir / impl / path.name


def build_task_context(task_dir: Path) -> TaskContext:
    default_points = None
    match = POINT_FROM_TASK_RE.search(task_dir.name)
    if match:
        default_points = int(match.group("points"))

    labels: set[str] = set()
    for source_name in ("sim", "analysis", "results"):
        source_dir = task_dir / source_name
        if not source_dir.is_dir():
            continue
        for file_path in source_dir.rglob("*"):
            if not file_path.is_file():
                continue
            stem, _ = split_stem_suffix(file_path.name)
            if stem is None:
                continue
            label = stem
            match = POINT_PREFIX_RE.fullmatch(stem)
            if match:
                label = match.group("label")
            elif "." in stem:
                label = stem.split(".", 1)[0]
            labels.add(label)

    return TaskContext(
        task_dir=task_dir,
        default_points=default_points,
        has_scalar_mainline="scalar-mainline" in labels,
        has_scalar_feopt="scalar-feopt" in labels,
    )


def move_file(src: Path, dst: Path, dry_run: bool) -> None:
    if src == dst:
        return
    if dst.exists():
        raise FileExistsError(f"destination already exists: {dst}")
    print(f"{src} -> {dst}")
    if dry_run:
        return
    dst.parent.mkdir(parents=True, exist_ok=True)
    shutil.move(str(src), str(dst))


def cleanup_empty_dirs(task_dir: Path, dry_run: bool) -> None:
    for source_name in ("sim", "analysis", "results"):
        source_dir = task_dir / source_name
        if not source_dir.exists():
            continue
        for subdir in sorted((p for p in source_dir.rglob("*") if p.is_dir()), reverse=True):
            if any(subdir.iterdir()):
                continue
            print(f"remove empty dir {subdir}")
            if not dry_run:
                subdir.rmdir()
        if source_dir.is_dir() and not any(source_dir.iterdir()):
            print(f"remove empty dir {source_dir}")
            if not dry_run:
                source_dir.rmdir()


def organize_task(task_dir: Path, dry_run: bool) -> None:
    ctx = build_task_context(task_dir)
    for source_name in ("sim", "analysis", "results"):
        source_dir = task_dir / source_name
        if not source_dir.is_dir():
            continue
        for file_path in sorted(p for p in source_dir.rglob("*") if p.is_file()):
            dst = classify_destination(file_path, ctx)
            if dst is None:
                continue
            move_file(file_path, dst, dry_run)
    cleanup_empty_dirs(task_dir, dry_run)


def iter_task_dirs(root: Path) -> list[Path]:
    if any((root / name).is_dir() for name in ("sim", "analysis", "results")):
        return [root]
    return sorted(p for p in root.iterdir() if p.is_dir())


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--root-dir",
        required=True,
        help="legacy perf folder or perf_eval root; outputs follow the shared n<points>/<version> layout",
    )
    parser.add_argument("--dry-run", action="store_true")
    args = parser.parse_args()

    root = Path(args.root_dir)
    for task_dir in iter_task_dirs(root):
        organize_task(task_dir, args.dry_run)


if __name__ == "__main__":
    main()
