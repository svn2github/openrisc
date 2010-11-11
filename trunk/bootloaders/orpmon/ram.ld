

MEMORY
        {
        vectors : ORIGIN = 0x00000000, LENGTH = 0x00002000 
	/* Use all RAM */
	/* Put all sections into ram */
	
        ram     : ORIGIN = 0x00002000, LENGTH = 0x02000000 - 0x00002000
	
	/* Uncomment the following to work around bad pages in a flash mem */
	/* Put all sections except data, rodata into ram2 */
	/* On ORSoC dev board devices, for pages 132-134 (256Byte pages) */
	/*
        ram     : ORIGIN = 0x00001200, LENGTH = 0x8400 - 0x1200
        ram2     : ORIGIN = 0x8700, LENGTH = 0x02000000 - 0x8700
	*/
	/* Uncomment the following to work around bad pages in a flash mem */
	/* Put all sections except data, rodata into ram2 */
	/* On ORSoC dev board devices, for pages 180-182 (256Byte pages) */
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
/*        } > ram2*/

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
/*        } > ram2 */

        .stack :
        {
        *(.stack)
        _src_addr = .;
	} > ram
/*        } > ram2 */
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
