MEMORY
        {
        vectors : ORIGIN = 0x00000000, LENGTH = 0x00002000 
        ram     : ORIGIN = 0x00002000, LENGTH = 0x00400000 - 0x00002000
	flash   : ORIGIN = 0xf0000000, LENGTH = 0x04000000
        }

SECTIONS
{
        .vectors :
        {
        *(.crc)
        *(.vectors)
        } > vectors

        .text :
        {
	_text_begin = .;
        *(.text)
	_text_end = .;
        } > ram

	.mytext :
	{
	*(.mytext)
	_fprog_addr = .;
	. += 0x500;
	} > ram

        .data :
	AT ( ADDR (.text) + SIZEOF(.text) + SIZEOF(.mytext))
        {
        *(.data)
        } > ram

        .rodata :
        {
        *(.rodata)
	*(.rodata.*)
        } > ram

        .bss :
        {
        *(.bss)
        } > ram

        .stack :
        {
        *(.stack)
        _src_addr = .;
        } > ram

	. = 0xf0000100;

	.monitor ALIGN(0x40000) :
	{
	*(.monitor)
	} > flash

	. += 0x100000;

	.config ALIGN(0x40000) :
	{
	_cfg_start = .;
	*(.config)
	_cfg_end = .;
	} > flash
}
