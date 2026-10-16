ENTRY(_start)
SECTIONS {
  . = 0x80000000;
  .text : { *(.text.start) *(.text*) }
  .rodata : { *(.rodata*) }
  /DISCARD/ : { *(.note*) *(.eh_frame*) *(.comment) *(.riscv.attributes) }
}
