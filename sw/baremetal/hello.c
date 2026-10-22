// hello.c — 香山 FPGA 裸机 Hello World
// 目标: PHINE PD-VU19P 板 + XiangShan MinimalConfig, 经 UART0 (16550 @ 0x310B0000) 输出
// 运行方式: 由 JTAG-to-AXI 加载到 DDR 偏移 0 (CPU 物理地址 0x80000000), bootrom 跳转执行
//
// 编译 (任一 riscv64 toolchain):
//   riscv64-linux-gnu-gcc -T hello.lds -nostartfiles -nostdlib \
//     -mcmodel=medany -march=rv64imac -mabi=lp64 -fno-PIE -O2 \
//     -Wl,--defsym=MEM_START=0x80000000 -o hello.elf hello.c
//   riscv64-linux-gnu-objcopy -O binary hello.elf hello.bin
//   bin2fpgadata -i hello.bin -o hello.txt
//
#define UART_BASE 0x310B0000UL
// 注意: 本设计的 16550 为 32 位数据宽度 -> 寄存器 4 字节步长 (LSR 在 +0x14, 非字节流 +0x5)
#define UART_DLL  (*(volatile unsigned int *)(UART_BASE + 0x0))  // 分频低 (DLAB=1)
#define UART_DLM  (*(volatile unsigned int *)(UART_BASE + 0x4))  // 分频高 (DLAB=1)
#define UART_THR  (*(volatile unsigned int *)(UART_BASE + 0x0))  // 发送保持寄存器 (DLAB=0)
#define UART_LCR  (*(volatile unsigned int *)(UART_BASE + 0xC))  // 线路控制
#define UART_LSR  (*(volatile unsigned int *)(UART_BASE + 0x14)) // 线路状态: bit5=THRE, bit6=TEMT

// 关键: 复位后 uart16550 的默认分频约为 38400 波特, 必须自行设置为 115200
// (30MHz/(16*16) = 117.2kbps, 误差 1.7%)
static void uart_init(void) {
    UART_LCR = 0x83;    // DLAB=1, 8N1
    UART_DLL = 16;
    UART_DLM = 0;
    UART_LCR = 0x03;    // DLAB=0, 8N1
}

static void uart_putc(char c) {
    while (!(UART_LSR & 0x20))
        ;
    UART_THR = (unsigned int)(unsigned char)c;
}

static void uart_puts(const char *s) {
    while (*s)
        uart_putc(*s++);
}

void _start(void) {
    uart_init();
    uart_puts("Hello, XiangShan! (MinimalConfig on PHINE PD-VU19P, 115200 8N1)\r\n");
    for (;;)
        ;
}
