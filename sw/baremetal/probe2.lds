SECTIONS {
  . = 0x10000000;
  .text : { *(.text*) }
  .rodata : { *(.rodata*) }
}
