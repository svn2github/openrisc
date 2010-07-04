/*-------------------------------------*/
/* faultret.h                          */
/* Last change :  3.11.94              */
/*-------------------------------------*/
/*
 *  $Id: faultret.h,v 1.2 2001-09-27 11:59:59 chris Exp $
 */

#ifndef _FAULTRET_H_
#define _FAULTRET_H_

  /* Return to the point where fault happened.
   * Fault state keeps all registers. 
   */
extern void faultRet(unsigned int * faultState);

#endif
/*-------------*/
/* End of file */
/*-------------*/

