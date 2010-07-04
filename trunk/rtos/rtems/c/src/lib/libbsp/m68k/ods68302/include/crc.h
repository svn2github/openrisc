/*****************************************************************************/
/*
  $Id: crc.h,v 1.2 2001-09-27 12:00:21 chris Exp $

   CRC 16 Calculate Interface

*/
/*****************************************************************************/

#ifndef _CRC_H_
#define _CRC_H_

 /* ----
      F U N C T I O N S
  */
#if __cplusplus
extern "C" 
{
#endif

rtems_unsigned16 calc_crc(void *data, rtems_unsigned32 count);

#if __cplusplus
}
#endif
#endif
