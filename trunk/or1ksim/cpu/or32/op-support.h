/* op-support.h -- Definitions of support routines for operations
   Copyright (C) 2005 Gy�rgy `nog' Jeney, nog@sdf.lonestar.org

This file is part of OpenRISC 1000 Architectural Simulator.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. */

void op_support_nop_exit(void);
void op_support_nop_reset(void);
void op_support_nop_printf(void);
void op_support_nop_report(void);
void op_support_nop_report_imm(int imm);
void op_support_analysis(void);
void do_jump(oraddr_t addr);

void upd_reg_from_t(oraddr_t pc, int bound);
