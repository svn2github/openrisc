/*
 *  $Id: nmi.c,v 1.2 2001-09-27 11:59:59 chris Exp $
 */

int mach_error_expected = 0;
void nmi_isr(void)
{
	if( mach_error_expected)
	{
	 	mach_error_expected = 0;
	}
	else{
		kkprintf("NMI Interrupt Occured \n");
	}
}
