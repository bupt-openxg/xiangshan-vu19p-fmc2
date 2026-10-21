# 软件负载（sw/）

面向本板香山核（MinimalConfig，UART16550 @ `0x310B0000`，DDR @ `0x80000000`）
的裸机程序。BUPT FFT/MIMO 负载不放在这里，构建时从
`third_party/riscv-xvbbp/software/nexus-am/apps` 安装（见 `tests/README.md`）。

```
sw/baremetal/   裸机 Hello / DDR 写路径自检 / 诊断探针源码
```

## 构建

```bash
source tools/env.sh
make -C sw/baremetal
```

| 程序 | 作用 |
|---|---|
| `hello.c → hello.txt` | 打印 `Hello, XiangShan!`，自带 UART 分频初始化 |
| `hello2.S → hello2.txt` | 纯汇编 Hello，无 C 运行时/栈依赖 |
| `wtest3.c → wtest3.txt` | DDR 写路径自检（fill + cbo.flush + 驱逐） |
| `probe*.S` / `wtest*.c` | 调试探针/写路径实验，保留作参考 |

## 四个坑

1. C 程序入口先设栈（`start.S`：`li sp,0x81000000`），否则 `sd ra,8(sp)` 双陷阱。
2. 无 libc 程序静态链接，避免 `.interp/.dynamic` 混入镜像。
3. objcopy 段过滤按程序定：hello 不过滤，hello2 过滤 `.note`，wtest3 不过滤。
4. UART 16550 必须程序自设分频：`LCR=0x83 → DLL=14, DLM=0 → LCR=0x03`。

写入与运行：`tests/common/board_run.sh` 或 `flows/06_run_hello.sh`。
负载文本由 `tools/bin/bin2fpgadata` 生成。
