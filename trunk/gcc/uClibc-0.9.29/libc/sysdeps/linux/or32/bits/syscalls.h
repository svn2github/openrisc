#include <asm/unistd.h>

#ifndef _SYSCALL_H
#error "Never use <bits/syscall.h> directly; include <sys/syscall.h> instead."
#endif

extern int errno;
void *__dso_handle;

/* Include the __NR_<name> definitions. */
#include <bits/sysnum.h>

#undef __syscall_return
#define __syscall_return(type) \
	return (__sc_ret > (unsigned long)-4096  ? errno = -__sc_ret, __sc_ret = -1 : 0), \
	       (type) __sc_ret

#undef __syscall_clobbers
#define __syscall_clobbers

#undef _syscall0
#define _syscall0(type,name) \
type name(void) \
{ \
register long __sc_ret __asm__ ("r11") = __NR_##name; \
__asm__ __volatile__ ("l.sys 	1" : "=r" (__sc_ret): "r" (__sc_ret)); \
__asm__ __volatile__ ("l.nop"); \
__syscall_return(type); \
}

#undef _syscall1
#define _syscall1(type,name,atype,a) \
type name(atype a) \
{ \
register long __sc_ret __asm__ ("r11") = __NR_##name; \
register long __a __asm__ ("r3") = (long)(a); \
__asm__ __volatile__ ("l.sys 	 1" \
			: "=r" (__sc_ret) \
			: "r" (__sc_ret), "r" (__a)); \
__asm__ __volatile__ ("l.nop"); \
__syscall_return(type); \
}

#undef _syscall2
#define _syscall2(type,name,atype,a,btype,b) \
type name(atype a,btype b) \
{ \
register long __sc_ret __asm__ ("r11") = __NR_##name; \
register long __a __asm__ ("r3") = (long)(a); \
register long __b __asm__ ("r4") = (long)(b); \
__asm__ __volatile__ ("l.sys 	 1" \
			: "=r" (__sc_ret) \
			: "r" (__sc_ret), "r" (__a), "r" (__b)); \
__asm__ __volatile__ ("l.nop"); \
__syscall_return(type); \
}

#undef _syscall3
#define _syscall3(type,name,atype,a,btype,b,ctype,c) \
type name(atype a,btype b,ctype c) \
{ \
register long __sc_ret __asm__ ("r11") = __NR_##name; \
register long __a __asm__ ("r3") = (long)(a); \
register long __b __asm__ ("r4") = (long)(b); \
register long __c __asm__ ("r5") = (long)(c); \
__asm__ __volatile__ ("l.sys 	 1" \
			: "=r" (__sc_ret) \
			: "r" (__sc_ret), "r" (__a), "r" (__b), "r" (__c)); \
__asm__ __volatile__ ("l.nop"); \
__syscall_return(type); \
}

#undef _syscall4
#define _syscall4(type,name,atype,a,btype,b,ctype,c,dtype,d) \
type name (atype a, btype b, ctype c, dtype d) \
{ \
register long __sc_ret __asm__ ("r11") = __NR_##name; \
register long __a __asm__ ("r3") = (long)(a); \
register long __b __asm__ ("r4") = (long)(b); \
register long __c __asm__ ("r5") = (long)(c); \
register long __d __asm__ ("r6") = (long)(d); \
__asm__ __volatile__ ("l.sys 	 1" \
			: "=r" (__sc_ret) \
			: "r" (__sc_ret), "r" (__a), "r" (__b), "r" (__c), "r" (__d)); \
__asm__ __volatile__ ("l.nop"); \
__syscall_return(type); \
}

#undef _syscall5
#define _syscall5(type,name,atype,a,btype,b,ctype,c,dtype,d,etype,e) \
type name (atype a,btype b,ctype c,dtype d,etype e) \
{ \
register long __sc_ret __asm__ ("r11") = __NR_##name; \
register long __a __asm__ ("r3") = (long)(a); \
register long __b __asm__ ("r4") = (long)(b); \
register long __c __asm__ ("r5") = (long)(c); \
register long __d __asm__ ("r6") = (long)(d); \
register long __e __asm__ ("r7") = (long)(e); \
__asm__ __volatile__ ("l.sys 	 1" \
			: "=r" (__sc_ret) \
			: "r" (__sc_ret), "r" (__a), "r" (__b), "r" (__c), "r" (__d), "r" (__e)); \
__asm__ __volatile__ ("l.nop"); \
__syscall_return(type); \
}
