MEMORY
{
	sram (rwx) : ORIGIN = 0x10000000, LENGTH = 4k
	flash (rx) : ORIGIN = 0x00000000, LENGTH = 32k
}

SECTIONS {
	. = ORIGIN(flash);
	.text :
	{
		KEEP(*(.vectors))
		*(.text*)
		*(.rodata*)
	} >flash
	.data :
	{
		*(.data*)
		*(.bss*)
	} >sram AT >flash
}

ENTRY(Reset_Handler)


_sram_size = LENGTH(sram);
_sram_base = ORIGIN(sram);
_end_stack = (_sram_base + _sram_size);