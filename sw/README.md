# 软件负载（sw/）

面向本板香山核（MinimalConfig, UART16550 @ `0x310B0000`, DDR @ `0x80000000`）的可运行负载。

```
sw/baremetal/   裸机程序源码与构建 (hello / hello2 / wtest3 等诊断程序)
sw/linux/       官方 Linux 内存镜像的获取与时钟修正说明 (README + fetch_image.sh)
```

## 1. 裸机程序（sw/baremetal）

构建（需先 `flows/01_setup_toolchain.sh` 装好 riscv64 工具链）：

```bash
source tools/env.sh
make -C sw/baremetal            # 产出 hello.txt / hello2.txt / wtest3.txt
```

| 程序 | 作用 | 关键点 |
|---|---|---|
| `hello.c → hello.txt` | 主负载：打印 `Hello, XiangShan!` 后自旋 | 自带 UART 分频初始化 |
| `hello2.S → hello2.txt` | 纯汇编版 Hello（分频初始化示范）| 无 C 运行时/栈依赖 |
| `wtest3.c → wtest3.txt` | DDR 写路径自检（fill + cbo.flush + 驱逐，UART 播报错误数）| 需 `rv64imac_zicbom` |
| `probe*.S` / `wtest*.c` / `wtest.c` | 调试探针/写路径实验（保留作参考）| 见源码头部注释 |

写入与运行见 `docs/run-guide.md`（`flows/06_run_hello.sh` 一键完成）。

### 四个必读的坑（本工程已全部踩过）

1. **入口必须先设栈**：ROM 跳转过来时 `sp` 是垃圾值，C 函数序言 `sd ra,8(sp)`
   会写非法地址 → 存储异常 → `mtvec=0` → 双陷阱（critical error，无任何输出）。
   故 C 程序使用 `start.S` 启动桩：`.text.start` 段内 `li sp,0x81000000; j bmain`，
   链接脚本 `ENTRY(_start)` 且 `*(.text.start)` 放最前。`start.S` 与 C 文件的入口名
   靠编译期宏对接（如 `wtest3` 用 `-Dbmain=w3main`，避免再写一份启动桩）。
2. **无 libc 程序要静态链接**：默认动态链接会给程序注入 `.interp/.dynamic/.got`
   等**可加载段**，objcopy 出的镜像会混入这些垃圾（且入口地址可能被 `.note.gnu.build-id`
   等前置段顶掉，ROM 固定跳 `0x80000000` 就落空）。诊断程序统一加 `-static`，
   wtest 系列另加 `-Wl,--build-id=none` 保险。注意 `hello` 的官方配方**不带**这两个开关
   （其 `.note` 恰好落在 `.text` 之后，无害）——见下条。
3. **镜像抽取的段过滤要按程序定**：objcopy 用"无过滤"（`-O binary`）还是
   `-j .text -j .rodata` 过滤，三个程序各不相同，抽错就是"能编译但上板无输出"：
   - `hello`：字符串常量在 **`.data`**，必须无过滤；
   - `hello2`：`.note.gnu.build-id` 尾随 `.rodata`，必须过滤；
   - `wtest3`：只有 `.text/.rodata` 两个可加载段，无过滤（`-static` 已保证没有别的段）。
   这些配方不是猜的：`make` 产物与**上板实测通过的原产物逐字节一致**
   （本仓库实测 9 个产物中 7 个连 `.elf` 都完全相同；另 2 个 `.elf` 仅差 gcc 临时
   目标文件名 `ccXXXXXX.o`——随机、无功能影响，`.bin/.txt` 均逐字节相同）。
4. **UART 分频必须程序自设**：16550 复位默认分频 = 162（**9600 波特**，实测读 DLL=0xA2）。
   程序必须写 `LCR=0x83 → DLL=14, DLM=0 → LCR=0x03`（25MHz/16/14 ≈ 111.6k ≈ 115200）。

## 2. Linux 镜像（sw/linux）

见 [sw/linux/README.md](linux/README.md)：官方直链、sha256、以及**为什么必须**先跑
`tools/patch_linux_dtb_clock.py` 生成 25MHz 时钟修正版镜像。

## 3. 负载文本格式（data.txt）

`make write_ddr`/`flows/07` 写入 DDR 的负载是 `.txt`（hex 文本）格式，由
`tools/bin/bin2fpgadata` 从 `.bin` 生成。**切不可手写**：换算约定为
"每 8 字节 beat 以小端十六进制表示、按地址从高到低排列"（最右 = 最低地址），
写反会得到字节序颠倒的乱码程序。格式细节见 `docs/workload-format.md`。
