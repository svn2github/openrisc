#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/errno.h>

#ifdef __NR_vfork
#define __NR___libc_vfork __NR_vfork
inline	_syscall0(pid_t, __libc_vfork);
inline	_syscall0(pid_t, vfork);
#endif

libc_hidden_proto(vfork)
libc_hidden_weak(vfork)
