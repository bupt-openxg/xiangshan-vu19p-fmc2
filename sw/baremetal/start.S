# 裸机启动桩: 设置栈指针后进入 C 主程序 (ROM 跳转过来时 sp 为垃圾值, 必须先设置!)
    .section .text.start
    .globl _start
_start:
    li   sp, 0x81000000        # 栈顶 (DDR 安全区)
    j    bmain
