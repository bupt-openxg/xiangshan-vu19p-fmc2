#!/usr/bin/env python3
"""从 UG 文本提取 DDR4 子卡引脚映射表 -> pinmaps/*.csv
用法: parse_ddr_pinmap.py <ug.txt> <start_line> <end_line> <out.csv> <FMCx>
解析列: No. | DDR pin name | FMC pin name | FPGA pin number
兼容换行被 pdftotext 打散的行 (无编号行 / 名字与球点分行)。
"""
import re
import sys

def parse(ug_path, start, end, fmc):
    rows = []
    with open(ug_path) as f:
        lines = f.readlines()[start - 1:end]
    text = "".join(lines)
    # 主模式: [No] DDR名 FMC名 球点
    pat = re.compile(
        r"(?:^|\s)(\d{1,3})?\s*"
        r"(DDR4_DIMM_[A-Za-z0-9_]+)\s+"
        r"((?:" + fmc + r"_[A-Za-z0-9_]+)?)\s*"
        r"([A-Z]{1,2}\d{1,2})\b"
    )
    for m in pat.finditer(text):
        no, sig, fmcpin, ball = m.groups()
        rows.append((no or "", sig, fmcpin or "", ball))
    # 去重 (同信号多行合并)
    seen = {}
    order = []
    for no, sig, fmcpin, ball in rows:
        if sig not in seen:
            seen[sig] = [fmcpin, ball]
            order.append(sig)
        else:
            if not seen[sig][0]:
                seen[sig][0] = fmcpin
    return [(sig, *seen[sig]) for sig in order]

def main():
    ug, start, end, out, fmc = sys.argv[1:6]
    rows = parse(ug, int(start), int(end), fmc)
    with open(out, "w") as f:
        f.write("ddr_signal,fmc_pin,fpga_ball\n")
        for sig, fmcpin, ball in rows:
            f.write(f"{sig},{fmcpin},{ball}\n")
    # 统计校验
    from collections import Counter
    kinds = Counter()
    for sig, _, _ in rows:
        name = sig.replace("DDR4_DIMM_", "")
        base = re.sub(r"\d+$", "", name)
        kinds[base] += 1
    print(f"[ok] {len(rows)} rows -> {out}")
    print("[kinds]", dict(sorted(kinds.items())))
    # 找 DQ 缺失 / 重复
    dq = [s for s, _, _ in rows if re.fullmatch(r"DDR4_DIMM_DQ\d+", s)]
    dqn = sorted(int(s.replace("DDR4_DIMM_DQ", "")) for s in dq)
    missing = sorted(set(range(64)) - set(dqn))
    dup = sorted({n for n in dqn if dqn.count(n) > 1})
    if missing:
        print("[warn] 缺失 DQ:", missing)
    if dup:
        print("[warn] 重复 DQ:", dup)

if __name__ == "__main__":
    main()
