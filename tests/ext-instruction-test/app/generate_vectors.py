#!/usr/bin/env python3
"""Generate deterministic fixed-vector expectations for the KMHv2 EXT test."""

from dataclasses import dataclass
from pathlib import Path


OPS = {
    "VSCMUL": (False, False),
    "VSCMULCJ": (True, False),
    "VSCMACC": (False, True),
    "VSCMACCCJ": (True, True),
}

ZERO = (0,) * 8

NORMAL_VS2 = (16384, 8192, -16384, 8192, 32767, 0, 0, -16384)
NORMAL_VS1 = (8192, -16384, 24576, -8192, 0, 32767, -16384, 0)
NORMAL_ACC = (1000, -2000, -3000, 4000, 327, -512, -100, 200)

IDENTITY_VS2 = (0, 0, 32767, 0, -32768, 0, 0, 32767)
IDENTITY_VS1 = (12345, -23456, 32767, 0, 32767, 0, 0, -32768)
IDENTITY_ACC = (1234, -2345, -3456, 4567, 3276, -4096, 0, 200)

# Products create +0.5, +1.5, +0.25 and -0.5 Q15 LSB boundaries.
ROUND_VS2 = (128, 0, 192, 0, 64, 0, -128, 0)
ROUND_VS1 = (128, 0, 256, 0, 128, 0, 128, 0)

SAT_VS2 = (-32768, 0, 32767, 32767, 32767, 32767, -32768, -32768)
SAT_VS1 = (-32768, 0, -32768, 32767, 32767, -32768, -32768, -32768)
SAT_ACC = (32767, 32767, -32768, -32768, 32767, -32768, -32768, 32767)

ADD_NORMAL_VS2 = (1000, 2000, -3000, 4000, 12345, -23456, -16000, 0)
ADD_NORMAL_VS1 = (500, -600, 7000, -8000, -2345, 3456, 1, 16000)
ADD_AXIS_VS2 = (0, 0, 8192, 0, 0, -8192, -8192, 0)
ADD_AXIS_VS1 = (1234, -5678, 0, 8192, -8192, 0, 0, -8192)
ADD_SAT_VS2 = (32767, 32767, -32768, -32768, 32767, -32768, -32768, 32767)
ADD_SAT_VS1 = (1, -32768, -1, 32767, 32767, 32767, -32768, -32768)


@dataclass(frozen=True)
class Case:
    name: str
    op: str
    vxrm: int
    vs2: tuple[int, ...]
    vs1: tuple[int, ...]
    vd: tuple[int, ...]
    expected: tuple[int, ...]
    vxsat: int


def round_shift_q15(value: int, vxrm: int) -> int:
    lsb = 1 << 15
    half = lsb >> 1
    uvalue = value & ((1 << 128) - 1)
    if vxrm == 0:  # RNU
        value += half
    elif vxrm == 1:  # RNE
        if (uvalue & half) and ((uvalue & (half - 1)) or (uvalue & lsb)):
            value += lsb
    elif vxrm == 2:  # RDN
        pass
    elif vxrm == 3:  # ROD
        if uvalue & (lsb - 1):
            value |= lsb
    else:
        raise ValueError(vxrm)
    return value >> 15


def sat16(value: int) -> tuple[int, int]:
    if value > 32767:
        return 32767, 1
    if value < -32768:
        return -32768, 1
    return value, 0


def calc_complex(op: str, vxrm: int, vs2, vs1, vd) -> tuple[tuple[int, ...], int]:
    conj_vs2, with_acc = OPS[op]
    result = []
    saturated = 0
    for lane in range(len(vs2) // 2):
        ar, ai = vs2[2 * lane], vs2[2 * lane + 1]
        br, bi = vs1[2 * lane], vs1[2 * lane + 1]
        if conj_vs2:
            real = br * ar + bi * ai
            imag = -br * ai + bi * ar
        else:
            real = br * ar - bi * ai
            imag = br * ai + bi * ar
        if with_acc:
            real += vd[2 * lane] << 15
            imag += vd[2 * lane + 1] << 15
        real, sat_r = sat16(round_shift_q15(real, vxrm))
        imag, sat_i = sat16(round_shift_q15(imag, vxrm))
        saturated |= sat_r | sat_i
        result.extend((real, imag))
    return tuple(result), saturated


def calc_add(op: str, vs2, vs1) -> tuple[tuple[int, ...], int]:
    result = []
    saturated = 0
    for lane in range(len(vs2) // 2):
        ar, ai = vs2[2 * lane], vs2[2 * lane + 1]
        br, bi = vs1[2 * lane], vs1[2 * lane + 1]
        if op == "VSCADDCONJ":
            values = (ar + br, bi - ai)
        elif op == "VSCADDRQ":
            values = (br - ai, ar + bi)
        elif op == "VSCADDRTQ":
            values = (ai + br, bi - ar)
        else:
            raise ValueError(op)
        for value in values:
            narrowed, sat = sat16(value)
            saturated |= sat
            result.append(narrowed)
    return tuple(result), saturated


def make_cases() -> list[Case]:
    cases = []
    for op in OPS:
        acc = NORMAL_ACC if OPS[op][1] else ZERO
        for label, vs2, vs1, vd in (
            ("normal", NORMAL_VS2, NORMAL_VS1, acc),
            ("zero_near_identity", IDENTITY_VS2, IDENTITY_VS1,
             IDENTITY_ACC if OPS[op][1] else ZERO),
        ):
            expected, sat = calc_complex(op, 0, vs2, vs1, vd)
            cases.append(Case(f"{op.lower()}_{label}_rnu", op, 0,
                              vs2, vs1, vd, expected, sat))
        for vxrm, rm_name in enumerate(("rnu", "rne", "rdn", "rod")):
            expected, sat = calc_complex(op, vxrm, ROUND_VS2, ROUND_VS1, ZERO)
            cases.append(Case(f"{op.lower()}_round_{rm_name}", op, vxrm,
                              ROUND_VS2, ROUND_VS1, ZERO, expected, sat))
        sat_acc = SAT_ACC if OPS[op][1] else ZERO
        expected, sat = calc_complex(op, 0, SAT_VS2, SAT_VS1, sat_acc)
        cases.append(Case(f"{op.lower()}_saturation_rnu", op, 0,
                          SAT_VS2, SAT_VS1, sat_acc, expected, sat))

    for op in ("VSCADDCONJ", "VSCADDRQ", "VSCADDRTQ"):
        for label, vs2, vs1 in (
            ("normal", ADD_NORMAL_VS2, ADD_NORMAL_VS1),
            ("zero_axis", ADD_AXIS_VS2, ADD_AXIS_VS1),
            ("saturation", ADD_SAT_VS2, ADD_SAT_VS1),
        ):
            expected, sat = calc_add(op, vs2, vs1)
            cases.append(Case(f"{op.lower()}_{label}", op, 0,
                              vs2, vs1, ZERO, expected, sat))
    return cases


def make_m2_cases() -> list[Case]:
    """Exercise both 128-bit uops of an e16,m2,vl=16 instruction."""
    cases = []
    for op in OPS:
        with_acc = OPS[op][1]
        normal_vd = NORMAL_ACC if with_acc else ZERO
        saturation_vd = SAT_ACC if with_acc else ZERO
        for label, vs2, vs1, vd in (
            ("normal_saturation", NORMAL_VS2 + SAT_VS2,
             NORMAL_VS1 + SAT_VS1, normal_vd + saturation_vd),
            ("saturation_normal", SAT_VS2 + NORMAL_VS2,
             SAT_VS1 + NORMAL_VS1, saturation_vd + normal_vd),
        ):
            expected, sat = calc_complex(op, 0, vs2, vs1, vd)
            cases.append(Case(f"{op.lower()}_m2_{label}", op, 0,
                              vs2, vs1, vd, expected, sat))

    for op in ("VSCADDCONJ", "VSCADDRQ", "VSCADDRTQ"):
        for label, vs2, vs1 in (
            ("normal_saturation", ADD_NORMAL_VS2 + ADD_SAT_VS2,
             ADD_NORMAL_VS1 + ADD_SAT_VS1),
            ("saturation_normal", ADD_SAT_VS2 + ADD_NORMAL_VS2,
             ADD_SAT_VS1 + ADD_NORMAL_VS1),
        ):
            expected, sat = calc_add(op, vs2, vs1)
            cases.append(Case(f"{op.lower()}_m2_{label}", op, 0,
                              vs2, vs1, ZERO + ZERO, expected, sat))
    return cases


def c_array(values) -> str:
    return "{ " + ", ".join(str(v) for v in values) + " }"


def write_header(path: Path) -> None:
    cases = make_cases()
    m2_cases = make_m2_cases()
    lines = [
        "/* Generated by generate_vectors.py; do not edit by hand. */",
        "#ifndef EXT_TEST_VECTORS_H",
        "#define EXT_TEST_VECTORS_H",
        "",
        "static const ext_test_case_t kExtTestCases[] = {",
    ]
    for case in cases:
        lines.extend((
            "  {",
            f'    "{case.name}", {case.op}, {case.vxrm},',
            f"    {c_array(case.vs2)},",
            f"    {c_array(case.vs1)},",
            f"    {c_array(case.vd)},",
            f"    {c_array(case.expected)}, {case.vxsat}",
            "  },",
        ))
    lines.extend((
        "};",
        "",
        "#define EXT_TEST_CASE_COUNT \\",
        "  (sizeof(kExtTestCases) / sizeof(kExtTestCases[0]))",
        "",
        "static const ext_m2_test_case_t kExtM2TestCases[] = {",
    ))
    for case in m2_cases:
        lines.extend((
            "  {",
            f'    "{case.name}", {case.op}, {case.vxrm},',
            f"    {c_array(case.vs2)},",
            f"    {c_array(case.vs1)},",
            f"    {c_array(case.vd)},",
            f"    {c_array(case.expected)}, {case.vxsat}",
            "  },",
        ))
    lines.extend((
        "};",
        "",
        "#define EXT_M2_TEST_CASE_COUNT \\",
        "  (sizeof(kExtM2TestCases) / sizeof(kExtM2TestCases[0]))",
        "",
        "#endif",
        "",
    ))
    path.write_text("\n".join(lines), encoding="ascii", newline="\n")
    print(f"generated {len(cases)} m1 cases and {len(m2_cases)} m2 cases -> {path}")


if __name__ == "__main__":
    write_header(Path(__file__).with_name("ext_test_vectors.h"))
