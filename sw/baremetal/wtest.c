// DDR 写路径测试: 写入地址唯一模式 -> cbo.flush 强制写回 -> 自旋
// 模式: beat i (8字节) = (i<<8) | 0x5A, 唯一可溯源
#define REG 0x81000000UL
#define SZ  (256*1024UL)

void _start(void) {
    volatile unsigned long *p = (volatile unsigned long *)REG;
    unsigned long i, n = SZ/8;
    for (i = 0; i < n; i++)
        p[i] = (i << 8) | 0x5AUL;
    // 强制写回+无效化 (Zicbom cbo.flush, 64B 粒度)
    for (unsigned long a = REG; a < REG+SZ; a += 64)
        asm volatile("cbo.flush (%0)" :: "r"(a) : "memory");
    for (;;)
        ;
}
