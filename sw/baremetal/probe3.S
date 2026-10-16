# UART 数据通路探针: 不轮询, 直接写. 前 8 字节用 sb(字节写), 后 8 字节用 sw(字写)
    .section .text
    .globl _start
_start:
    li   x9, 0x310B0000
    # 'A'..'H' 字节写
    li   x8, 0x41
    sb   x8, 0(x9)
    li   x8, 0x42
    sb   x8, 0(x9)
    li   x8, 0x43
    sb   x8, 0(x9)
    li   x8, 0x44
    sb   x8, 0(x9)
    li   x8, 0x45
    sb   x8, 0(x9)
    li   x8, 0x46
    sb   x8, 0(x9)
    li   x8, 0x47
    sb   x8, 0(x9)
    li   x8, 0x48
    sb   x8, 0(x9)
    # '0'..'7' 字写
    li   x8, 0x30
    sw   x8, 0(x9)
    li   x8, 0x31
    sw   x8, 0(x9)
    li   x8, 0x32
    sw   x8, 0(x9)
    li   x8, 0x33
    sw   x8, 0(x9)
    li   x8, 0x34
    sw   x8, 0(x9)
    li   x8, 0x35
    sw   x8, 0(x9)
    li   x8, 0x36
    sw   x8, 0(x9)
    li   x8, 0x37
    sw   x8, 0(x9)
    j    .
