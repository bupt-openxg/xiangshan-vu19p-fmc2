#!/usr/bin/env python3
"""四张 UG DDR 表交叉验证 + xls 球点合并 -> pinmaps/fmc2_ddr4_pins.csv
用法: build_fmc2_pinmap.py <ug.txt> <xls> <out.csv>
"""
import os
import re
import sys
from collections import defaultdict, Counter

# 本仓库 tools/sysroot 内的 python3-xlrd (由 flows/01 安装)
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                "sysroot", "usr", "lib", "python3", "dist-packages"))
import xlrd

# UG 四张表的行范围 (1-based, 含头不含 6.1.3)
TABLES = {
    "FMC2": (15204, 15675),
    "FMC4": (15676, 16156),
    "FMC7": (16157, 16630),
    "FMC9": (16631, 17104),
}

PAT = re.compile(
    r"(?:^|\s)(\d{1,3})?\s*"
    r"(DDR4_DIMM_[A-Za-z0-9_]+)\s+"
    r"(FMC[0-9]+_[A-Za-z0-9_]+)\s*"
    r"([A-Z]{1,2}\d{1,2})\b"
)

def parse_table(lines, slot):
    pairs = {}
    text = "".join(lines)
    for m in PAT.finditer(text):
        _, sig, fmcpin, ball = m.groups()
        if not fmcpin.startswith(slot + "_"):
            continue  # 避免跨表串行
        fmcpin = fmcpin.replace(slot + "_", "", 1)
        pairs.setdefault(sig, []).append((fmcpin, ball))
    return pairs

def main():
    ug_path, xls_path, out_path = sys.argv[1:4]
    lines = open(ug_path).readlines()
    votes = defaultdict(Counter)          # sig -> Counter(fmc_pin_local)
    slots = {}
    for slot, (a, b) in TABLES.items():
        slots[slot] = parse_table(lines[a - 1:b], slot)
        for sig, lst in slots[slot].items():
            for fmcpin, _ in lst:
                votes[sig][fmcpin] += 1

    # xls FMC2: signal name -> ball
    wb = xlrd.open_workbook(xls_path)
    sh = wb.sheet_by_name("FMC2")
    xls_ball = {}
    for r in range(sh.nrows):
        sig = sh.cell_value(r, 3)
        ball = sh.cell_value(r, 5)
        if isinstance(sig, str) and sig.startswith("FMC2_") and isinstance(ball, str) and re.fullmatch(r"[A-Z]{1,2}\d{1,2}", ball):
            xls_ball[sig.replace("FMC2_", "", 1)] = ball

    rows = []
    problems = []
    for sig in sorted(votes, key=lambda s: (re.sub(r"\d+$", "", s), int(re.findall(r"\d+$", s)[0]) if re.findall(r"\d+$", s) else 0)):
        c = votes[sig]
        fmcpin, n = c.most_common(1)[0]
        if n < 3:
            problems.append(f"低置信 {sig}: votes={dict(c)}")
        ball = xls_ball.get(fmcpin)
        if not ball:
            problems.append(f"xls 缺球点 {sig} -> {fmcpin}")
            ball = "MISSING"
        rows.append((sig, fmcpin, ball))

    with open(out_path, "w") as f:
        f.write("ddr_signal,fmc_pin,fpga_ball\n")
        for sig, fmcpin, ball in rows:
            f.write(f"{sig},FMC2_{fmcpin},{ball}\n")

    print(f"[ok] {len(rows)} signals -> {out_path}")
    if problems:
        print("[problems]")
        for p in problems:
            print("  ", p)
    # 一致性检查: 各表对同信号的 fmc pin 分歧
    for slot, sp in slots.items():
        for sig, lst in sp.items():
            pins = {p for p, _ in lst}
            if len(pins) > 1:
                print(f"[diverge] {slot} {sig}: {pins}")

if __name__ == "__main__":
    main()
