/*
 *  $Id: semaphore.c,v 1.2 2001-09-27 11:59:17 chris Exp $
 */

#include <stdarg.h>

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <limits.h>

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/posix/semaphore.h>
#include <rtems/posix/time.h>
#include <rtems/posix/seterr.h>

#ifndef _POSIX_PATH_MAX
#define _POSIX_PATH_MAX 255
#endif

/*PAGE
 *
 *  _POSIX_Semaphore_Manager_initialization
 *
 *  This routine initializes all semaphore manager related data structures.
 *
 *  Input parameters:
 *    maximum_semaphores - maximum configured semaphores
 *
 *  Output parameters:  NONE
 */
 
void _POSIX_Semaphore_Manager_initialization(
  unsigned32 maximum_semaphores
)
{
  _Objects_Initialize_information(
    &_POSIX_Semaphore_Information,
    OBJECTS_POSIX_SEMAPHORES,
    TRUE,
    maximum_semaphores,
    sizeof( POSIX_Semaphore_Control ),
    TRUE,
    _POSIX_PATH_MAX,
    FALSE
  );
}
