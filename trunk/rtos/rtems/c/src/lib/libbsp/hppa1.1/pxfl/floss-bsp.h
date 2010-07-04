/* 
 * we need a bsp.h in the rtems tree to keep the src/tests/...
 * happy.
 * But our bsp.h really lives in floss tree.
 *
 * Fake it out by installing this one in rtems
 *
 #  $Id: floss-bsp.h,v 1.2 2001-09-27 11:59:44 chris Exp $
 */

#include <floss/bsp.h>
