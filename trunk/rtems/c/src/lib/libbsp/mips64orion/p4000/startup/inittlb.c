/*
 *  $Id: inittlb.c,v 1.2 2001-09-27 12:00:25 chris Exp $
 */

#include <idtcpu.h>

extern void resettlb( int i );

void init_tlb(void)
{
  int i;

        for (i = 0; i < N_TLB_ENTRIES; i++ )
                resettlb(i);
}

