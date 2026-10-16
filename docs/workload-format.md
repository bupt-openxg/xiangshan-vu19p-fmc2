# data.txt 工作负载格式（JTAG-to-AXI 写 DDR）

本文档记录 `bin2fpgadata` 生成的 data.txt 与 `jtag_write_ddr.tcl` 加载器的字节级语义
（已用官方 hello-xs 与自编译 hello 两组数据逐字节回读验证）。

## 格式

```
<地址(hex, 任意位数, 需 8 字节对齐)>
<数据串(hex, 长度须为 16 的倍数)>
<地址>
<数据串>
...
```

## 加载器语义（jtag_write_ddr.tcl, 64bit AXI）

- 每 **16 个 hex 字符 = 1 个 64-bit beat**（`beat_bytes = AXI_WIDTH/8`）
- 数据串内的 beat **从最右端开始对应最低地址**（hw_axi `-data` 约定）：
  第 k 组（自左数，共 K 组）写入地址 `line_addr + (K-1-k)*8`
- beat 的 16 位 hex 为**大端表示的 64bit 值**，内存按**小端**存放
  （等价于：`bytes = reverse(hex_decode(group))`）
- 每笔 HW AXI 事务 ≤256 beat（脚本自动分块）

## 验证方法（Python 回读）

```python
mem = bytearray(size)
for addr, data in line_pairs:
    K = len(data)//16
    for k in range(K):
        raw = bytes.fromhex(data[k*16:(k+1)*16])[::-1]
        mem[addr + (K-1-k)*8 : addr + (K-1-k)*8 + 8] = raw
assert mem[:len(bin)] == bin   # 已对 hello-xs.bin 与自编译 hello.bin 验证通过
```

## 生成（自编译程序）

```bash
cd sw/baremetal
make CROSS=riscv64-linux-gnu-      # 需要 tools/env.sh 提供的本地工具链
# 输出 hello.txt (data.txt 格式) -> make write_ddr WORKLOAD=.../hello.txt
```

## 注意事项

- 地址 0 = CPU 物理地址 0x80000000（DDR 窗口，SoC wrapper 内做了 -0x8000_0000 平移）
- 文件末尾会多 1 个 0x0a 字节（工具添加的 EOF 标记），位于程序之后的无害区域
- 官方 `hello-xs.txt`/`microbench-xs-no-uart.txt` 由同一工具生成，格式相同
