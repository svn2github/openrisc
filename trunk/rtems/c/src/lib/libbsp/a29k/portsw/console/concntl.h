/*
 *  $Id: concntl.h,v 1.2 2001-09-27 11:59:36 chris Exp $
 */

typedef enum
{
        CON_KBHIT,
        CON_GET_RAW_BYTE,
        CON_SEND_RAW_BYTE
} console_ioctl_t;

typedef struct
{
        console_ioctl_t ioctl_type;
   unsigned32 param;
} console_ioctl_request_t;
