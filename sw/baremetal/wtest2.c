// 写路径测试 v2: 进度标记 + CPU 自校验 + flush 前后对比
#define REG  0x81000000UL
#define SZ   (256*1024UL)
#define MARK 0x80000200UL

void _start(void) {
    volatile unsigned long *p = (volatile unsigned long *)REG;
    volatile unsigned long *m = (volatile unsigned long *)MARK;
    unsigned long n = SZ/8, i, err;

    m[0] = 0x1111;                      // 阶段1: 开始
    for (i = 0; i < n; i++) p[i] = (i << 8) | 0x5A;
    m[1] = 0x2222;                      // 阶段2: fill 完成

    err = 0;
    for (i = 0; i < n; i++) if (p[i] != ((i << 8) | 0x5A)) err++;
    m[2] = 0x3333; m[3] = err;          // 阶段3: cache 视角回读错误数

    for (unsigned long a = REG; a < REG+SZ; a += 64)
        asm volatile("cbo.flush (%0)" :: "r"(a) : "memory");
    m[4] = 0x4444;                      // 阶段4: flush 完成

    err = 0;
    for (i = 0; i < n; i++) if (p[i] != ((i << 8) | 0x5A)) err++;
    m[5] = err;                         // 阶段5: flush 后回读错误数
    m[6] = 0x5555;                      // 程序完成
    for (;;) ;
}
