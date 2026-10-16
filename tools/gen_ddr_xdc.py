#!/usr/bin/env python3
"""由 hw/pinmaps/fmc2_ddr4_pins.csv 生成 PHINE 板 DDR4 MIG 引脚约束 ddr.xdc
用法: gen_ddr_xdc.py <pins.csv> <out.xdc>
覆盖 DDR0_* 顶层端口 (64bit 单 rank MIG), 跳过 ECC/rank1/parity/alert 未用引脚。
"""
import csv
import re
import sys

# DDR4_DIMM_* -> DDR0_* 映射 (单 rank 64bit, MIG 无 parity/alert)
def map_signal(sig):
    n = sig.replace("DDR4_DIMM_", "")
    m = re.fullmatch(r"DQ(\d+)", n)
    if m and int(m.group(1)) < 64:
        return f"DDR0_DQ[{m.group(1)}]"
    m = re.fullmatch(r"DQ(\d+)", n)
    if m:  # 64-71 ECC lane -> 不用
        return None
    m = re.fullmatch(r"DQS_P(\d)", n)
    if m and int(m.group(1)) < 8:
        return f"DDR0_DQS_T[{m.group(1)}]"
    m = re.fullmatch(r"DQS_N(\d)", n)
    if m and int(m.group(1)) < 8:
        return f"DDR0_DQS_C[{m.group(1)}]"
    m = re.fullmatch(r"DM(\d)", n)
    if m and int(m.group(1)) < 8:
        return f"DDR0_DM[{m.group(1)}]"
    m = re.fullmatch(r"A(\d+)", n)
    if m:
        return f"DDR0_A[{m.group(1)}]"
    m = re.fullmatch(r"BA(\d)", n)
    if m:
        return f"DDR0_BA[{m.group(1)}]"
    m = re.fullmatch(r"BG(\d)", n)
    if m:
        return f"DDR0_BG[{m.group(1)}]"
    m = re.fullmatch(r"CK_P(\d)", n)
    if m and m.group(1) == "0":
        return "DDR0_CK_T[0]"
    m = re.fullmatch(r"CK_N(\d)", n)
    if m and m.group(1) == "0":
        return "DDR0_CK_C[0]"
    m = re.fullmatch(r"CKE(\d)", n)
    if m and m.group(1) == "0":
        return "DDR0_CKE[0]"
    m = re.fullmatch(r"ODT(\d)", n)
    if m and m.group(1) == "0":
        return "DDR0_ODT[0]"
    if n == "CS0_N":
        return "DDR0_CS_N[0]"
    if n == "ACT_N":
        return "DDR0_ACT_N"
    if n == "RST_B":
        return "DDR0_RESET_N"
    # 未使用: DQ64-71, DQS8, DM8, CK1, CKE1, CS1_N, ODT1, EVENT_N, PARITY, ALERT_N
    return None

def main():
    pins_csv, out_xdc = sys.argv[1:3]
    rows = list(csv.DictReader(open(pins_csv)))
    emitted = {}
    for r in rows:
        port = map_signal(r["ddr_signal"])
        if port:
            emitted[port] = r["fpga_ball"]
    # ai1 ddr.xdc 的端口全集(用于核对): 116 个
    expect = (
        [f"DDR0_A[{i}]" for i in range(17)]
        + [f"DDR0_BA[{i}]" for i in range(2)]
        + [f"DDR0_BG[{i}]" for i in range(2)]
        + ["DDR0_ACT_N"]
        + ["DDR0_CK_T[0]", "DDR0_CK_C[0]"]
        + ["DDR0_CKE[0]", "DDR0_CS_N[0]", "DDR0_ODT[0]", "DDR0_RESET_N"]
        + [f"DDR0_DM[{i}]" for i in range(8)]
        + [f"DDR0_DQ[{i}]" for i in range(64)]
        + [f"DDR0_DQS_T[{i}]" for i in range(8)]
        + [f"DDR0_DQS_C[{i}]" for i in range(8)]
    )
    missing = [p for p in expect if p not in emitted]
    extra = [p for p in emitted if p not in expect]
    if missing or extra:
        print("ERROR: 端口集合不符  missing=%s extra=%s" % (missing, extra))
        sys.exit(1)

    with open(out_xdc, "w") as f:
        f.write("# PHINE PD-VU19P-2-LSI SOLO v3/FMC2 + UV-FMCH-PDDR4DME DDR4 引脚约束\n")
        f.write("# 由 tools/gen_ddr_xdc.py 从 pinmaps/fmc2_ddr4_pins.csv 生成 (UG §6.1.2.1 + BSP 引脚 xls 交叉验证)\n")
        f.write("# MIG: SODIMM 64bit 单 rank 800MT/s; ECC/rank1/parity/alert 引脚未连接\n\n")
        for p in expect:
            f.write(f"set_property PACKAGE_PIN {emitted[p]} [get_ports {{{p}}}]\n")
    print(f"[ok] {len(expect)} pins -> {out_xdc}")

if __name__ == "__main__":
    main()
