# 裸机程序构建（香山 FPGA, UART @ 0x310B0000, 加载地址 0x80000000）
#
# 工具链: riscv64-linux-gnu-（本仓库 tools/sysroot 内, 由 flows/01_setup_toolchain.sh 安装;
#         先 `source tools/env.sh`）; 也可自行 apt install gcc-riscv64-linux-gnu
#
# 用法:
#   make              -> hello + hello2 + wtest3（三个写入负载）
#   make hello.txt    -> 主负载: 初始化 UART 分频并打印 Hello（供 flows/06 写入 DDR）
#   make hello2.txt   -> 纯汇编版 Hello（分频初始化示范, 不依赖 C 运行时/栈）
#   make wtest3.txt   -> DDR 写路径/复位链自检（UART 播报各阶段错误数）
#   make clean
#
# 说明: 本 Makefile 的配方与"上板实测通过"的原产物保持逐字节一致（gcc 13.3.0 / binutils 2.42）,
#       三处非显然但关键的选择（改动前请对照 sw/README.md 的坑位记录）:
#   1. hello/wtest3 用"无过滤" objcopy: hello 的字符串常量在 .data 段, 只抽 .text/.rodata
#      会把字符串丢掉; wtest3 只有 .text/.rodata 两个可加载段, 无过滤即精确镜像。
#   2. hello2 反而必须过滤段(-j .text -j .rodata): 它的 .note.gnu.build-id 尾随在 .rodata
#      之后, 不过滤则镜像尾部会多出 build-id。
#   3. wtest3 必须 -static: 默认动态链接会给无 libc 程序塞入 .interp/.dynamic/.got 等
#      可加载段, 混进镜像把 DDR 负载搞坏; 入口用 -Dbmain=w3main 别名（start.S 跳 bmain,
#      wtest3.c 定义 w3main, 两者靠编译期宏对接）。
#
# 其余诊断源码 (probe*.S / wtest2/4.c) 的构建方式与对应 target 相同,
# 仅需替换 lds 与 -march（含 cbo.flush 的用 rv64imac_zicbom）。详见 sw/README.md。
#
CROSS    ?= riscv64-linux-gnu-
CC       := $(CROSS)gcc
OBJCOPY  := $(CROSS)objcopy
BIN2FPGADATA ?= ../../tools/bin/bin2fpgadata

CFLAGS_COMMON := -nostartfiles -nostdlib -mcmodel=medany -mabi=lp64 -fno-PIE -O2

all: hello.txt hello2.txt wtest3.txt

# ---- hello: 主负载 (C, 含 UART 分频初始化 115200@25MHz) ----
hello.elf: hello.c hello.lds
	$(CC) -T hello.lds $(CFLAGS_COMMON) -march=rv64imac \
	      -Wl,--defsym=MEM_START=0x80000000 -o $@ hello.c
hello.bin: hello.elf
	$(OBJCOPY) -O binary $< $@
hello.txt: hello.bin
	$(BIN2FPGADATA) -i hello.bin -o hello.txt
	@echo "[hello] hello.txt 就绪 ($(shell stat -c %s hello.bin) 字节 bin)"

# ---- hello2: 纯汇编版 (含分频初始化, 无栈依赖) ----
hello2.elf: hello2.S hello2.lds
	$(CC) -T hello2.lds $(CFLAGS_COMMON) -march=rv64imac -o $@ hello2.S
hello2.bin: hello2.elf
	$(OBJCOPY) -O binary -j .text -j .rodata $< $@
hello2.txt: hello2.bin
	$(BIN2FPGADATA) -i hello2.bin -o hello2.txt

# ---- wtest3: DDR 写路径自检 (fill + cbo.flush + 大填充驱逐, UART 播报错误数) ----
wtest3.elf: start.S wtest3.c wtest.lds
	$(CC) -T wtest.lds -static $(CFLAGS_COMMON) -march=rv64imac_zicbom \
	      -Wl,--build-id=none -Dbmain=w3main -o $@ start.S wtest3.c
wtest3.bin: wtest3.elf
	$(OBJCOPY) -O binary $< $@
wtest3.txt: wtest3.bin
	$(BIN2FPGADATA) -i wtest3.bin -o wtest3.txt

clean:
	rm -f *.elf *.o *.bin *.txt

.PHONY: all clean
