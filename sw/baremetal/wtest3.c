// DDR 写路径测试 v3: UART 播报各阶段结果
// 阶段: 1 fill 256KB -> 2 cache回读 -> 3 cbo.flush -> 4 回读 -> 5 大填充(驱逐) -> 6 回读 -> 7
#define REG  0x81000000UL
#define SZ   (256UL*1024)
#define BIG  0x84000000UL
#define BGSZ (48UL*1024*1024)

static void uputc(char c) {
    volatile unsigned int *lsr = (volatile unsigned int *)0x310B0014UL;
    volatile unsigned int *thr = (volatile unsigned int *)0x310B0000UL;
    while (!(*lsr & 0x20)) ;
    *thr = (unsigned int)(unsigned char)c;
}
static void puts_(const char *s) { while (*s) uputc(*s++); }
static void puthex(unsigned long v) {
    static const char h[] = "0123456789abcdef";
    for (int i = 60; i >= 0; i -= 4) uputc(h[(v >> i) & 0xF]);
    uputc(' ');
}

void w3main(void) {
    unsigned long n = SZ/8, i, err;
    volatile unsigned long *p = (volatile unsigned long *)REG;

    // UART 分频 (115200 @ 25MHz)
    *(volatile unsigned int *)0x310B000CUL = 0x83;
    *(volatile unsigned int *)0x310B0000UL = 14;
    *(volatile unsigned int *)0x310B0004UL = 0;
    *(volatile unsigned int *)0x310B000CUL = 0x03;
    puts_("\r\nW3\r\n");

    puts_("1\r\n");
    for (i = 0; i < n; i++) p[i] = (i << 8) | 0x5A;

    puts_("2 err_cache=");
    err = 0;
    for (i = 0; i < n; i++) if (p[i] != ((i << 8) | 0x5A)) err++;
    puthex(err); puts_("\r\n");

    puts_("3\r\n");
    for (unsigned long a = REG; a < REG+SZ; a += 64)
        asm volatile("cbo.flush (%0)" :: "r"(a) : "memory");

    puts_("4 err_flush=");
    err = 0;
    for (i = 0; i < n; i++) if (p[i] != ((i << 8) | 0x5A)) err++;
    puthex(err); puts_("\r\n");

    puts_("5\r\n");
    {
        volatile unsigned long *q = (volatile unsigned long *)BIG;
        for (i = 0; i < BGSZ/8; i++) q[i] = 0xDEADBEEF00000000UL | i;
    }

    puts_("6 err_evict=");
    err = 0;
    for (i = 0; i < n; i++) if (p[i] != ((i << 8) | 0x5A)) err++;
    puthex(err); puts_("\r\n");

    puts_("7\r\n");
    for (;;) ;
}
