# hello2: 自设 UART 分频 (115200 @ 30MHz -> divisor=16) 的裸机 Hello
    .section .text
    .globl _start
_start:
    li   x9, 0x310B0000
    # UART 初始化: LCR=0x83 (DLAB=1,8N1) -> DLL=16, DLM=0 -> LCR=0x03
    li   x8, 0x83
    sw   x8, 12(x9)
    li   x8, 16
    sw   x8, 0(x9)
    sw   x0, 4(x9)
    li   x8, 0x03
    sw   x8, 12(x9)
    # 先发一个 'X' 便于定位
    li   x8, 0x58
    sb   x8, 0(x9)
    # 打印长消息 (含轮询)
    la   x11, msg
1:  lbu  x12, 0(x11)
    beqz x12, 2f
3:  lw   x13, 20(x9)        # LSR @ +0x14
    andi x13, x13, 0x20     # THRE
    beqz x13, 3b
    sb   x12, 0(x9)
    addi x11, x11, 1
    j    1b
2:  j .
    .section .rodata
msg:
    .asciz "\r\nHello, XiangShan! MinimalConfig on PHINE PD-VU19P, 115200 8N1\r\n"
