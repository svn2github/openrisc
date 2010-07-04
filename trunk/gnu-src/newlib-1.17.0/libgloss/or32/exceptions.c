#include <stdio.h>
#include <sys/types.h>

#define STR_EXCEPTION_OCCURED " exception occured.\n"

char *exception_strings[] = {
	"An unknown",             // 0
	"A reset (?!)",           // 1 
	"A Bus Error",            // 2
	"A Data Page Fault",      // 3
	"An Instruction Page Fault", // 4
	"A Tick-Timer",              // 5
	"An Alignment",              // 6
	"An Illegal Instruction",    // 7
	"An External Interrupt",     // 8
	"A D-TLB Miss",              // 9
	"An I-TLB Miss",             // a
	"A Range",                   // b
	"A System Call",             // c
	"A Floating-Point",          // d
	"A Trap",                    // e
	"A \"Reserved\"",            // f
	"A \"Reserved\"",            // 10
	"A \"Reserved\"",            // 11
	"A \"Reserved\"",            // 12
	"A \"Reserved\"",            // 13
	"A \"Reserved\"",            // 14
	"An Implementation Specific \"Reserved\"", // 16
	"An Implementation Specific \"Reserved\"", // 17
	"An Implementation Specific \"Reserved\"", // 18
	"A Custom",                                // 19
	"A Custom",                                // 1a
	"A Custom",                                // 1b
	"A Custom",                                // 1c
	"A Custom",                                // 1d
	"A Custom",                                // 1e
	"A Custom",                                // 1f
};

void default_exception_handler_c(__uint32_t exception_address, __uint32_t epc)
{
	int exception_no = (exception_address >> 8) & 0x1f;
	
	printf("EPC = 0x%.8x\n", exception_address);
	
	// Output initial messaging using low-level functions incase
	// something really bad happened.
	write(stderr,
	      exception_strings[exception_no],
	      strlen(exception_strings[exception_no]));
	
	write(stderr,
	      STR_EXCEPTION_OCCURED,
	      strlen(STR_EXCEPTION_OCCURED));

	// Icing on the cake using fancy functions
	printf("EPC = 0x%.8x\n", epc);

	/* For the simulator this will terminate execution */
	_exit (65535);

	for(;;);
}
