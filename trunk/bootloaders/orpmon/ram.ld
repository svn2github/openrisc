
/* Seen 3 bad pages, 180-182 (0xb400-0xb6ff), so put text after these pages */

MEMORY
        {
        vectors : ORIGIN = 0x00000000, LENGTH = 0x00002000 
        ram     : ORIGIN = 0x00002000, LENGTH = 0x02000000 - 0x00002000

	/* Uncomment the following to work around bad pages in a flash mem */
	/* 
        ram     : ORIGIN = 0x00001200, LENGTH = 0xB400 - 0x1200
        ram2     : ORIGIN = 0xb700, LENGTH = 0x02000000 - 0xb700
	*/
	/*flash   : ORIGIN = 0xf0000000, LENGTH = 0x04000000*/
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

        .data :
/*	AT ( ADDR (.text) + SIZEOF(.text) + SIZEOF(.mytext))*/
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
        src_addr = .;
        } > ram
/*
	.monitor :
	{
	*(.monitor)
	} > ram
*/

/*
	. += 0x100000;

// This section was in cmds/load.c, but we don't need it -jb
	.config :
	{
	_cfg_start = .;
	*(.config)
	_cfg_end = .;
	} > ram
*/
/*
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
*/
}
