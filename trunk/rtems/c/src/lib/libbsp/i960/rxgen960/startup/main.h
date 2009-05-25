/*-------------------------------------*/
/* main.h                              */
/* Last change :  2. 2.95              */
/*-------------------------------------*/
/*
 *  $Id: main.h,v 1.2 2001-09-27 11:59:59 chris Exp $
 */

#ifndef _MAIN_H_
#define _MAIN_H_

  /* ROM monitor main function(s). 
   * Gets control from rommon.s.
   */
extern void romMain(void);
extern void romFaultMain(void);
extern void romTestMain(void);

extern char * mainRevisionStr(void);
extern char * mainBuildStr(void);

extern unsigned int bootAddr[];

#endif
/*-------------*/
/* End of file */
/*-------------*/

