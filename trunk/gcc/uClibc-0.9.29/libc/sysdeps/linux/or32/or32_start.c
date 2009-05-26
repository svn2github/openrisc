/* 
 * uClibc/sysdeps/linux/or32/crt1.c
 * Pull stuff off the stack and get uClibc moving.
 *
 * Copyright (C) 2000,2001 by Erik Andersen <andersen@uclibc.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/*void __uClibc_main(int argc,void *argv,void *envp);*/

void _uClibc_start(unsigned int first_arg)
{
	unsigned int argc;
	char **argv, **envp;
	unsigned long *stack;

	stack = (unsigned long*) first_arg;
	argc = *(stack);
	argv = (char **)(stack + 1);
	envp = (char **)(stack + 1 + argc + 1);

	__uClibc_start_main(argc, argv, envp);
}

extern int weak_function main(int argc, char **argv, char **envp);
void __attribute__ ((__noreturn__))
__uClibc_start_main(int argc, char **argv, char **envp,
		    void (*app_fini)(void), void (*app_init)(void))
{
	__uClibc_main(main, argc, argv, app_init, app_fini, 0, 0);
}
