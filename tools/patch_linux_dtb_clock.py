#!/usr/bin/env python3
"""
修复官方 kmh-linux 内存镜像的 DTB UART 时钟假设。

背景: 官方镜像内嵌 DTB (OpenSBI embedded, 镜像偏移 0x1e000) 中
serial@310b0000 的 clock-frequency = 50MHz, 而本设计 (fpga_diff, PHINE 板)
AXI_bridge/JTAG-DDR 子系统的实际时钟为 clk5 = 25MHz。
软件按 50MHz 计算分频 -> 实际波特率偏离 2 倍 (OpenSBI 横幅 57600, 内核阶段更乱)。

修复: 将镜像中该 DTB 的 clock-frequency 改为 25,000,000。
     此后 OpenSBI / Linux 内核 / 自制裸机 (分频=14) 均为 ~111.6kbps ≈ 115200 8N1。

用法: python3 tools/patch_linux_dtb_clock.py <data.txt> <输出.txt>
"""
import sys

def decode(data_txt):
    segs = []
    it = iter([l.strip() for l in open(data_txt) if l.strip()])
    while True:
        try: a = int(next(it), 16)
        except StopIteration: break
        try: d = next(it)
        except StopIteration: break
        beats = [bytes.fromhex(d[i:i+16]) for i in range(0, len(d), 16)]
        segs.append((a, b''.join(b[::-1] for b in reversed(beats))))
    img = bytearray(max(a+len(m) for a, m in segs))
    for a, m in segs:
        img[a:a+len(m)] = m
    return img

def encode(img, out_txt, chunk=1024):
    out = []
    for off in range(0, len(img), chunk):
        c = bytes(img[off:off+chunk])
        beats = [c[i:i+8] for i in range(0, len(c), 8)]
        out.append(hex(off))
        out.append(''.join(b[::-1].hex() for b in reversed(beats)))
    open(out_txt, 'w').write('\n'.join(out) + '\n')

def main():
    src, dst = sys.argv[1], sys.argv[2]
    img = decode(src)
    pat = bytes.fromhex('0000000300000004') + (0x1a8).to_bytes(4, 'big') + bytes.fromhex('02faf080')
    idx = bytes(img).find(pat)
    if idx < 0:
        print("未找到 clock-frequency=50MHz 属性, 无需修改"); 
        encode(img, dst); return
    valoff = idx + 12
    img[valoff:valoff+4] = (25000000).to_bytes(4, 'big')
    print(f"已修改 clock-frequency: 50000000 -> 25000000 (镜像偏移 {hex(valoff)})")
    encode(img, dst)
    print(f"输出: {dst}")

if __name__ == '__main__':
    main()
