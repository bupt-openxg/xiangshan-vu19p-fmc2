# ROM 驻留探针: 先打印(验证CPU执行+串口), 后写DDR标记(验证CPU→DDR通路)
    .section .text
    .globl _start
_start:
    li    x9, 0x310B0000        # UART THR
    addi  x10, x9, 5            # UART LSR
    la    x11, msg1
1:  lbu   x12, 0(x11)
    beqz  x12, 2f
3:  lbu   x13, 0(x10)
    andi  x13, x13, 0x20        # THRE
    beqz  x13, 3b
    sb    x12, 0(x9)
    addi  x11, x11, 1
    j     1b
2:  li    x6, 0x80010000        # 标记区
    li    x8, 0xA5A5A5A5
    sw    x8, 0(x6)             # M1: CPU执行到此处
    la    x11, msg2
4:  lbu   x12, 0(x11)
    beqz  x12, 5f
6:  lbu   x13, 0(x10)
    andi  x13, x13, 0x20
    beqz  x13, 6b
    sb    x12, 0(x9)
    addi  x11, x11, 1
    j     4b
5:  li    x8, 0x5A5A5A5A
    sw    x8, 4(x6)             # M2: 两段打印完成
    j     .
    .section .rodata
msg1: .asciz "P\r\n"
msg2: .asciz "Hello, XiangShan!\r\n"
