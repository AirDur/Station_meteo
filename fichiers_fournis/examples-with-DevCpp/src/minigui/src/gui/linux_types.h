#ifndef _LINUX_TYPES_H
#define _LINUX_TYPES_H

#ifdef __NOUNIX__

typedef unsigned char		unchar;
#if !defined(__SIMNT__)
typedef unsigned short		ushort;
#endif
typedef unsigned int		uint;
typedef unsigned long		ulong;

#if !defined(__ECOS__) && !defined(__SIMNT__)
typedef unsigned short		u_short;
typedef unsigned int		u_int;
typedef unsigned long		u_long;
#endif

#else

#include <sys/types.h>

#endif

#endif /* _LINUX_TYPES_H */
