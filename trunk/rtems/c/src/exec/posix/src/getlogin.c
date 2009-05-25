/*
 *  $Id: getlogin.c,v 1.2 2001-09-27 11:59:17 chris Exp $
 */

#include <limits.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/posix/seterr.h>

#ifndef LOGIN_NAME_MAX
#define LOGIN_NAME_MAX 9
#endif

/*PAGE
 *
 *  4.2.4 Get User Name, P1003.1b-1993, p. 87
 *
 *  NOTE:  P1003.1c/D10, p. 49 adds getlogin_r().
 */

static char _POSIX_types_Getlogin_buffer[ LOGIN_NAME_MAX ];

char *getlogin( void )
{
  (void) getlogin_r( _POSIX_types_Getlogin_buffer, LOGIN_NAME_MAX );
  return _POSIX_types_Getlogin_buffer;
}

/*PAGE
 *
 *  4.2.4 Get User Name, P1003.1b-1993, p. 87
 *
 *  NOTE:  P1003.1c/D10, p. 49 adds getlogin_r().
 */

int getlogin_r(
  char   *name,
  size_t  namesize
)
{
  if ( namesize < LOGIN_NAME_MAX )
    return ERANGE;

  strcpy( name, "posixapp" );
  return 0;
}
