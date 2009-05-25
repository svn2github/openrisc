/*-------------------------------------*/
/* ihandler.h                          */
/* Last change : 12.10.94              */
/*-------------------------------------*/
/*
 *  $Id: ihandler.h,v 1.2 2001-09-27 11:59:59 chris Exp $
 */

#ifndef _IHANDLER_H_
#define _IHANDLER_H_

  /* Interrupt Handler for NMI.
   */
extern void nmiHandler(void);
  /* Interrupt Handlers for Dedicated Interrupts.
   */
extern void intr5Handler(void);
extern void intr6Handler(void);   

#endif
/*-------------*/
/* End of file */
/*-------------*/

