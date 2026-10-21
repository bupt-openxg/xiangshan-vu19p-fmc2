#!/usr/bin/env python3
from __future__ import annotations

import re
from dataclasses import dataclass
from pathlib import Path


VERSIONS = ("oai", "scalar", "rvv", "ext")

REPORT_RE = re.compile(
    r"n(?P<points>[0-9]+)-r(?P<repeat>[0-9]+)-idft(?P<idft>[0-9]+)-(?P<tag>[a-z0-9-]+)\.report\.txt"
)
VALIDATE_RE = re.compile(
    r"n(?P<points>[0-9]+)-r(?P<repeat>[0-9]+)-idft(?P<idft>[0-9]+)-(?P<tag>[a-z0-9-]+)\.(?P<suffix>validate[^/]*)\.txt"
)
ANALYSIS_RE = re.compile(
    r"n(?P<points>[0-9]+)-r(?P<repeat>[0-9]+)-idft(?P<idft>[0-9]+)-(?P<tag>[a-z0-9-]+)\.analysis\.md"
)

DEFAULT_IMPL = {
    "oai": "minimal",
    "scalar": "mainline",
    "rvv": "mainline",
    "ext": "mainline",
}


@dataclass(frozen=True)
class ImplKey:
    version: str
    implementation: str


@dataclass(frozen=True)
class ParsedArtifact:
    path: Path
    points: int | None
    repeat: int | None
    idft: int | None
    scope: str
    version: str | None = None
    implementation: str | None = None


def split_impl_tag(tag: str) -> ImplKey | None:
    if tag in DEFAULT_IMPL:
        return ImplKey(tag, DEFAULT_IMPL[tag])

    for version in VERSIONS:
        prefix = f"{version}-"
        if tag.startswith(prefix):
            implementation = tag[len(prefix):]
            if implementation:
                return ImplKey(version, implementation)

    return None


def classify_vs_tag(tag: str) -> tuple[str, str | None]:
    left, right = tag.split("-vs-", 1)
    left_key = split_impl_tag(left)
    right_key = split_impl_tag(right)

    if left_key and right_key and left_key.version != right_key.version:
        return "point_analysis", None

    if left_key:
        return "version_analysis", left_key.version

    return "point_analysis", None


def classify_artifact(path: Path) -> ParsedArtifact | None:
    if path.name == "pow2-dft-summary.analysis.md":
        return ParsedArtifact(path=path, points=None, repeat=None, idft=None, scope="summary")

    match = REPORT_RE.fullmatch(path.name)
    if match:
        impl = split_impl_tag(match.group("tag"))
        if impl is None:
            return None
        return ParsedArtifact(
            path=path,
            points=int(match.group("points")),
            repeat=int(match.group("repeat")),
            idft=int(match.group("idft")),
            scope="impl_sim",
            version=impl.version,
            implementation=impl.implementation,
        )

    match = VALIDATE_RE.fullmatch(path.name)
    if match:
        impl = split_impl_tag(match.group("tag"))
        if impl is None:
            return None
        return ParsedArtifact(
            path=path,
            points=int(match.group("points")),
            repeat=int(match.group("repeat")),
            idft=int(match.group("idft")),
            scope="impl_sim",
            version=impl.version,
            implementation=impl.implementation,
        )

    match = ANALYSIS_RE.fullmatch(path.name)
    if not match:
        return None

    points = int(match.group("points"))
    repeat = int(match.group("repeat"))
    idft = int(match.group("idft"))
    tag = match.group("tag")

    if tag == "comparison":
        return ParsedArtifact(path=path, points=points, repeat=repeat, idft=idft, scope="point_analysis")

    if "-vs-" in tag:
        scope, version = classify_vs_tag(tag)
        return ParsedArtifact(
            path=path,
            points=points,
            repeat=repeat,
            idft=idft,
            scope=scope,
            version=version,
        )

    impl = split_impl_tag(tag)
    if impl is None:
        return None

    return ParsedArtifact(
        path=path,
        points=points,
        repeat=repeat,
        idft=idft,
        scope="impl_analysis",
        version=impl.version,
        implementation=impl.implementation,
    )


def point_dir(run_dir: Path, points: int) -> Path:
    return run_dir / f"n{points}"


def point_analysis_dir(run_dir: Path, points: int) -> Path:
    return point_dir(run_dir, points)


def version_dir(run_dir: Path, points: int, version: str) -> Path:
    return point_dir(run_dir, points) / version


def version_analysis_dir(run_dir: Path, points: int, version: str) -> Path:
    return version_dir(run_dir, points, version) / "analysis"


def version_sim_dir(run_dir: Path, points: int, version: str) -> Path:
    return version_dir(run_dir, points, version) / "sim"


def impl_analysis_dir(run_dir: Path, points: int, version: str, implementation: str) -> Path:
    return version_analysis_dir(run_dir, points, version) / implementation


def impl_sim_dir(run_dir: Path, points: int, version: str, implementation: str) -> Path:
    return version_sim_dir(run_dir, points, version) / implementation


def target_path_for_artifact(run_dir: Path, artifact: ParsedArtifact) -> Path:
    if artifact.scope == "summary":
        return run_dir / artifact.path.name

    assert artifact.points is not None

    if artifact.scope == "point_analysis":
        return point_analysis_dir(run_dir, artifact.points) / artifact.path.name

    if artifact.scope == "version_analysis":
        assert artifact.version is not None
        return version_dir(run_dir, artifact.points, artifact.version) / artifact.path.name

    if artifact.scope == "impl_analysis":
        assert artifact.version is not None
        assert artifact.implementation is not None
        return impl_analysis_dir(run_dir, artifact.points, artifact.version, artifact.implementation) / artifact.path.name

    if artifact.scope == "impl_sim":
        assert artifact.version is not None
        assert artifact.implementation is not None
        return impl_sim_dir(run_dir, artifact.points, artifact.version, artifact.implementation) / artifact.path.name

    raise ValueError(f"unsupported artifact scope: {artifact.scope}")
