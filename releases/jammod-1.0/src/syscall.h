/*
 * Taken from SucKIT 1.3b; its license file says:
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

/*
 * $Id: stuff.h, syscalls calling stuff
 */

#ifndef _SYSCALL_H
#define _SYSCALL_H

#define	rr(n, x) ,n ((ulong) x)

/* syscall stuff */
#define syscall0(__type, __name)	\
__type __name(void)	\
{					\
	ulong	__res;			\
	__asm__ volatile		\
	("int	$0x80"			\
	: "=a" (__res)			\
	: "0" (__NR_##__name));		\
	return (__type) __res;		\
}

#define syscall1(__type, __name, __t1)	\
	 __type __name(__t1 __a1)	\
{					\
	ulong	__res, d1;		\
	__asm__ volatile		\
	("int	$0x80"			\
	: "=a" (__res), "=&b" (d1)	\
	: "0" (__NR_##__name)		\
	  rr("1", __a1));		\
	return (__type) __res;		\
}

#define syscall2(__type, __name, __t1, __t2)		\
	 __type __name(__t1 __a1, __t2 __a2)	\
{					\
	ulong	__res;			\
	__asm__ volatile		\
	("int	$0x80"			\
	: "=a" (__res)			\
	: "0" (__NR_##__name)		\
	  rr("b", __a1)			\
	  rr("c", __a2));		\
	return (__type) __res;		\
}

#define syscall3(__type, __name, __t1, __t2, __t3)		\
	 __type __name(__t1 __a1, __t2 __a2, __t3 __a3)	\
{					\
	ulong	__res;			\
	__asm__ volatile		\
	("int	$0x80"			\
	: "=a" (__res)			\
	: "0" (__NR_##__name)		\
	  rr("b", __a1)			\
	  rr("c", __a2)			\
	  rr("d", __a3));		\
	return (__type) __res;		\
}

#define syscall4(__type, __name, __t1, __t2, __t3, __t4)			\
	 __type __name(__t1 __a1, __t2 __a2, __t3 __a3, __t4 __a4)	\
{					\
	ulong	__res;			\
	__asm__ volatile		\
	("int	$0x80"			\
	: "=a" (__res)			\
	: "0" (__NR_##__name)		\
	  rr("b", __a1)			\
	  rr("c", __a2)			\
	  rr("d", __a3)			\
	  rr("S", __a4));		\
	return (__type) __res;		\
}

#define syscall5(__type, __name, __t1, __t2, __t3, __t4, __t5)				\
	 __type __name(__t1 __a1, __t2 __a2, __t3 __a3, __t4 __a4, __t5 __a5)	\
{					\
	ulong	__res;			\
	__asm__ volatile		\
	("int	$0x80"			\
	: "=a" (__res)			\
	: "0" (__NR_##__name)		\
	  rr("b", __a1)			\
	  rr("c", __a2)			\
	  rr("d", __a3)			\
	  rr("S", __a4)			\
	  rr("D", __a5));		\
	return (__type) __res;		\
}

#define	__NR_KMALLOC OURSYS
#define	__NR_KFREE OURSYS
#define	__NR_INIT_MODULE OURSYS

static inline syscall2(ulong, KMALLOC, ulong, ulong)
static inline syscall1(int, KFREE, ulong)
static inline syscall0(int, INIT_MODULE)

static inline struct ts * current(void)
{
	struct ts *ts;
	__asm__("andl %%esp,%0; ":"=r" (ts) : "0" (~8191UL));
	return ts;
}

#endif
