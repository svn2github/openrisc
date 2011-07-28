#include "../../../Source/portable/GCC/OpenRISC/portmacro.h"
#include "spr_defs.h"

// Dummy or32 except vectors
void buserr_except(void) {
	uart_print_str("buserr_except\n\r");
	while(1) ;
}

void dpf_except(void) {
	uart_print_str("dpf_except\n\r");
	while(1) ;
}

void ipf_except(void) {
	uart_print_str("ipf_except\n\r");
	while(1) ;
}

void align_except(void) {
	uart_print_str("align_except\n\r");
	while(1) ;
}

void illegal_except(void) {
	uart_print_str("illegal_except\n\r");
	while(1) ;
}

void dtlbmiss_except(void) {
	uart_print_str("dtlbmiss_except\n\r");
	while(1) ;
}

void itlbmiss_except(void) {
	uart_print_str("itlbmiss_except\n\r");
	while(1) ;
}

void range_except(void) {
	uart_print_str("range_except\n\r");
	while(1) ;
}

static void syscall_enter_critical(void) {
	unsigned int exception_sr = mfspr(SPR_ESR_BASE);
	exception_sr &= (~SPR_SR_IEE);		// disable all external interrupt
	exception_sr &= (~SPR_SR_TEE);		// disable tick timer interrupt

	mtspr(SPR_ESR_BASE, exception_sr);
}

static void syscall_exit_critical(void) {
	unsigned int exception_sr = mfspr(SPR_ESR_BASE);
	exception_sr |= SPR_SR_IEE;		// enable all external interrupt
	exception_sr |= SPR_SR_TEE;		// enable tick timer interrupt

	mtspr(SPR_ESR_BASE, exception_sr);
}

void syscall_except(int id) {
	if(id == 0x0FCE) {
		syscall_enter_critical();
	} else if(id == 0x0FCF) {
		syscall_exit_critical();
	} else {
		uart_put_int(id);
		uart_print_str(" syscall is not impelmented yet....\n\r");
	}
}


void res1_except(void) {
	uart_print_str("res1_except\n\r");
	while(1) ;
}

void trap_except(void) {
	uart_print_str("trap_except\n\r");
	while(1) ;
}

void res2_except(void) {
	uart_print_str("res2_except\n\r");
	while(1) ;
}

