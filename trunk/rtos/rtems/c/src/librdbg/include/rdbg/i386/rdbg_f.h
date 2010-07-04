/*
 **************************************************************************
 *
 * Component =   RDBG
 * Module =   rdbg_f.h
 *
 * Synopsis = Machine-dependent header file
 *
 * $Id: rdbg_f.h,v 1.2 2001-09-27 12:02:01 chris Exp $
 *
 **************************************************************************
 */

#ifndef RDBG_F_H
#define RDBG_F_H

#include <rtems.h>
#include <rdbg/remdeb.h>

#define EFLAGS_TF 0x00100

static inline int isRdbgException(Exception_context *ctx)
{
  if (
      ctx->ctx->idtIndex != I386_EXCEPTION_DEBUG &&
      ctx->ctx->idtIndex != I386_EXCEPTION_BREAKPOINT &&
      ctx->ctx->idtIndex != I386_EXCEPTION_ENTER_RDBG
      ) return 0;
  else return 1;
}
static inline int getExcNum(Exception_context *ctx)
{
  return ctx->ctx->idtIndex;
}

extern void connect_rdbg_exception();

#endif



