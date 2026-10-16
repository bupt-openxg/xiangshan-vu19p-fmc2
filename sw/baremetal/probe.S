# 双标记探针: M1(0xA5A5A5A5)@+0x10000 = CPU已启动; 打印HELLO后 M2(0x5A5A5A5A)@+0x10004 = UART通路工作
    .section .text
    .globl _start
_start:
    auipc x6, 0x10              # x6 = 运行地址 + 0x10000 (标记区)
    li    x8, 0xA5A5A5A5
    sw    x8, 0(x6)             # M1: CPU ALIVE
    li    x9, 0x310B0000        # UART THR
    addi  x10, x9, 5            # UART LSR
    li    x8, 0x5A5A5A5A        # M2 值
    la    x11, msg
1:  lbu   x12, 0(x11)
    beqz  x12, 2f
3:  lbu   x13, 0(x10)
    andi  x13, x13, 0x20        # THRE?
    beqz  x13, 3b
    sb    x12, 0(x9)
    addi  x11, x11, 1
    j     1b
2:  sw    x8, 4(x6)             # M2: 打印循环完成
    j     .
    .section .rodata
msg:
    .asciz "Hello, XiangShan!\r\n"
