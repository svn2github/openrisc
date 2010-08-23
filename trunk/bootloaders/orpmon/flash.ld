MEMORY
        {
        flash   : ORIGIN = 0xf0000000, LENGTH = 0x04000000
        vectors : ORIGIN = 0x00000000, LENGTH = 0x00002000 
        ram     : ORIGIN = 0x00002000, LENGTH = 0x04000000 - 0x00002000
        }
 
SECTIONS
{
        .reset :
        {
        *(.crc)
        *(.reset)
        } > flash

        .text ALIGN(0x04):
        {
        *(.text)
        } > flash

        .rodata :
        {
        *(.rodata)
	*(.rodata.*)
        } > flash

	.monitor ALIGN(0x40000) :
	{
	*(.monitor)
	} > flash

	. += 0x100000;

	.config ALIGN(0x40000) :
	{
	_config_end = .;
	*(.config)
	} > flash

        .dummy ALIGN(0x40000):
        {
        _src_beg = .;
        } > flash

        .vectors :
        AT ( ADDR (.dummy) )
        {
        _vec_start = .;
        *(.vectors)
        _vec_end = .;
        } > vectors

        .data :
        AT ( ADDR (.dummy) + SIZEOF (.vectors) )
        {
        _dst_beg = .;
        *(.data)
        _dst_end = .;
        } > ram

        .bss :
        {
        *(.bss)
        } > ram

        .stack :
        {
        *(.stack)
        } > ram

	.mytext :
	{
	_fprog_addr = .;
	*(.mytext)
	. += 0x500;
	src_addr = .;
	} > ram
}
