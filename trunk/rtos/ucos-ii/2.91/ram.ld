MEMORY
        {
        vectors : ORIGIN = 0x00000000, LENGTH = 0x00002000 
        ram     : ORIGIN = 0x00002000, LENGTH = 0x00200000 - 0x00002000
        }
 
SECTIONS
{
        .vectors :
        {
		*(.vectors)
        } > vectors

	.text : {
		_stext = .;
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
	} > ram

        .stack :
        {
        *(.stack)
        _stack_addr = .;
        } > ram
}
