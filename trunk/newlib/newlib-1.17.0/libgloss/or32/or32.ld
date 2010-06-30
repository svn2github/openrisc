ENTRY(_start)
OUTPUT_ARCH(or32)
GROUP(-lor32 -lc -lgcc)
SEARCH_DIR(.)
__DYNAMIC  =  0;

MEMORY
{
	vectors : ORIGIN = 0, LENGTH = 0x2000
	ram     : ORIGIN = 0x2000, LENGTH = (16M - 0x2000)
}

/*
 * Allocate the stack to be at the top of memory, since the stack
 * grows down.
 */
PROVIDE (_stack = 8M - 4);
PROVIDE (__stack = 8M - 4);

SECTIONS
{	
	.vectors :
	{
		*(.vectors)
	} > vectors
                              
	.text : {
		*(.text)
		_etext  =  .;
		__CTOR_LIST__ = .;
		LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2)
		*(.ctors)
		LONG(0)
		__CTOR_END__ = .;
		__DTOR_LIST__ = .;
		LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2)
		*(.dtors)
		LONG(0)
		__DTOR_END__ = .;
		*(.lit)
		*(.shdata)
		_endtext = .;
	}  > ram

	.rodata : {
		*(.rodata);
		*(.rodata.*)
	} > ram
	
	.shbss :
	{
		*(.shbss)
	} > ram
	
	.talias :
	{ 
	}  > ram
	
	.data : {
		sdata  =  .;
		_sdata  =  .;
		*(.data)
		edata  =  .;
		_edata  =  .;
	} > ram
	
	.bss SIZEOF(.data) + ADDR(.data) :
	{
		sbss = . ;
		_sbss = . ;
		__bss_start = ALIGN(0x8);
		___bss_start = ALIGN(0x8);
		*(.bss)
		*(COMMON)
		end = ALIGN(0x8);
		_end = ALIGN(0x8);
		__end = ALIGN(0x8);
		ebss = .;
		_ebss = .;
	}
	
	.stab  0 (NOLOAD) : 
	{
		[ .stab ]
	}
	
	.stabstr  0 (NOLOAD) :
	{
		[ .stabstr ]
	}
}
