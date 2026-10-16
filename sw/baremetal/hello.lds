OUTPUT_ARCH(riscv)
ENTRY(_start)
SECTIONS
{
    . = 0x80000000;
    .text : { *(.text*) }

    . = 0x80000400;
    .data : { *(.data*) *(.rodata*) *(.sdata*) }
    .bss  : { *(.bss*) *(.sbss*) *(COMMON) }
}
