---
name: baremetal-riscv-hello-first-light
description: 在新 RISC-V SoC/FPGA 上点亮第一个裸机 UART Hello，或排查"能编译、能加载、但串口无输出"时使用：入口与栈、build-id、objcopy 段过滤、静态链接、16550 分频、bin2fpgadata 负载格式、可复现比对。触发词：裸机、hello、串口无输出、objcopy、build-id、静态链接、分频、data.txt、bin2fpgadata。
---

# 裸机 first-light 五个坑（本仓库全部踩过，配方逐字节可复现）

参考实现：`sw/baremetal/`（`hello.c` / `hello2.S` / `wtest3.c` / `start.S` + `Makefile`），详细说明见 `sw/README.md`。以下每条都对应一次"上板无输出"的真实故障。

1. **入口必须先设栈**。ROM/上位机跳过来时 `sp` 是垃圾值，C 函数序言 `sd ra,8(sp)` 立即写非法地址 → 存储异常 → `mtvec=0` → 双陷阱（**无任何输出**）。做法：汇编启动桩，`.text.start` 段内 `li sp,<安全栈顶>; j bmain`；链接脚本 `ENTRY(_start)` 且 `*(.text.start)` 放最前。
2. **无 libc 程序必须 `-static`**。默认动态链接会给程序塞入 `.interp/.dynamic/.got` 等**可加载段**，混进镜像；`-fno-PIE` 不能替代 `-static`。
3. **build-id 的位置随链接脚本而变**：`.note.gnu.build-id` 可能尾随 `.text`（无害，本仓库 hello 配方即如此）也可能顶掉入口地址（表现为"执行的不是入口"）。换任何 lds 后都要检查产物段表（`objcopy -h`）与入口地址；不确定就 `-Wl,--build-id=none`。
4. **objcopy 段过滤按程序定，不能套模板**：`-j .text -j .rodata` 会丢掉放在 `.data`（非 const 数组字符串）或 `.srodata`（小只读数据）里的常量；不过滤则可能把尾随 `.note` 带进镜像。判断流程：`readelf -S` 看常量在哪个段 → 决定过滤与否（本仓库三个程序三种配方的对照见 `sw/baremetal/Makefile` 头注释）。
5. **16550 分频必须程序自设**：复位默认分频 162 = **9600 波特**（上板实测 DLL=0xA2）。`LCR=0x83 → DLL=14, DLM=0 → LCR=0x03`（25MHz/16/14 ≈ 111.6kbps ≈ 115200），输出前等待 LSR.THRE。

## 配套技巧

- **入口名别名**：启动桩固定 `j bmain` 时，C 侧用别的入口名（如 `w3main`）可通过 `-Dbmain=w3main` 对接，避免复制启动桩。
- **负载格式**：`.bin → .txt` 一律用 `tools/bin/bin2fpgadata`（每 8 字节 beat 小端十六进制、最高地址在最左）；**不可手写**（字节序写反=乱码程序，行对齐不对=烧写报错）。写 DDR 即"把 .txt 灌到 0x80000000"。
- **可复现性对照法**：与已验证产物 `cmp` 比对是最高效的回归手段。注意 gcc 临时目标文件名（`ccXXXXXX.o`）会随机进 `.elf` 符号表——**比 `.bin/.txt`（真正加载的镜像）才是确定性的**；本仓库三程序 9 个产物中 7 个连 `.elf` 都逐字节一致。
- 启动成功与"程序跑飞"的分界往往就在上面 5 条；先逐条排除，再怀疑 SoC/DDR 本身。
