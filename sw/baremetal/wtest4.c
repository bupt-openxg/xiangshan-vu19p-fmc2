// wtest4: (1) 读 UART 复位默认寄存器 (2) 设置 115200 分频 (3) CPU 复制运行时 DTB 到 0x82000000 (4) flush
#define UART 0x310B0000UL
#define SRC  0x80200000UL
#define DST  0x82000000UL
#define SZ   (24UL*1024)

static volatile unsigned int *lsr = (volatile unsigned int *)(UART+0x14);
static volatile unsigned int *thr = (volatile unsigned int *)(UART+0x00);
static volatile unsigned int *lcr = (volatile unsigned int *)(UART+0x0C);

static void uputc(char c) { while (!(*lsr & 0x20)) ; *thr = (unsigned int)(unsigned char)c; }
static void puts_(const char *s) { while (*s) uputc(*s++); }
static const char HEXD[] = "0123456789abcdef";
static void puthex(unsigned long v) {
    for (int i = 60; i >= 0; i -= 4) uputc(HEXD[(v >> i) & 0xF]);
    uputc(' ');
}

void bmain(void) {
    unsigned long def_lcr, def_dll, def_dlm, def_ier, def_fcr;

    // 1) 读复位默认值 (DLAB 切换)
    def_lcr = *lcr & 0xFF;
    *lcr = (unsigned int)(def_lcr | 0x80);
    def_dll = *thr & 0xFF;
    def_dlm = *(volatile unsigned int *)(UART+0x04) & 0xFF;
    *lcr = (unsigned int)(def_lcr & ~0x80);
    def_ier = *(volatile unsigned int *)(UART+0x04) & 0xFF;
    def_fcr = *(volatile unsigned int *)(UART+0x08) & 0xFF;

    // 2) 设置 115200 (30MHz, 分频 16)
    *lcr = 0x83; *thr = 16; *(volatile unsigned int *)(UART+0x04) = 0; *lcr = 0x03;
    *lcr = 0x03;  // 8N1

    // 3) 打印默认寄存器
    puts_("W4\r\nLCR="); puthex(def_lcr);
    puts_(" DLL="); puthex(def_dll);
    puts_(" DLM="); puthex(def_dlm);
    puts_(" IER="); puthex(def_ier);
    puts_(" FCR="); puthex(def_fcr);
    puts_("\r\n");

    // 4) 复制运行时 DTB (CPU 视角: 经缓存读! )
    volatile unsigned char *s = (volatile unsigned char *)SRC;
    volatile unsigned char *d = (volatile unsigned char *)DST;
    for (unsigned long i = 0; i < SZ; i++) d[i] = s[i];
    puts_("COPY DONE\r\n");

    // 5) flush 目标区
    for (unsigned long a = DST; a < DST+SZ; a += 64)
        asm volatile("cbo.flush (%0)" :: "r"(a) : "memory");
    puts_("FLUSH DONE\r\n");

    // 6) 再读 DST 前 4 字节的魔数 (应 0xd00dfeed)
    puthex(((unsigned long)d[0]<<24)|((unsigned long)d[1]<<16)|((unsigned long)d[2]<<8)|d[3]);
    puts_("\r\nDONE\r\n");
    for (;;) ;
}
