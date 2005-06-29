/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 

/*
 * c.h- Some basic definitions
 */
#ifndef	C_H
#define C_H

/*
 * true, false, TRUE, FALSE
 */
#ifndef false
#define false	((char) 0)
#endif
#ifndef true
#define true	((char) 1)
#endif
/* XXXXX - Taken out because conflict with curses.h
typedef char	bool;
*/

#ifndef TRUE
#define TRUE	1
#endif /* TRUE */

#ifndef FALSE
#define FALSE	0
#endif /* FALSE */

#ifndef	NULL
#define NULL	0
#endif	/* !defined(NULL) */

#include <stddef.h>
#include <stdlib.h>

#if defined(PORTNAME_c90)
typedef short	INT32;			/* >= 32 bits */
typedef unsigned short	UINT32;		/* >= 32 bits */
#else
typedef int	INT32;			/* >= 32 bits */
typedef unsigned int	UINT32;		/* >= 32 bits */
#endif

#define Assert(condition)

/*
 * Max - Return the maximum of two numbers.
 */
#define Max(x, y)	((x) > (y) ? (x) : (y))

/*
 * Min - Return the minimum of two numbers.
 */
#define Min(x, y)	((x) < (y) ? (x) : (y))

/* Abs - Return the absolute value of the argument.
 */
#define Abs(x)		((x) >= 0 ? (x) : -(x))

#if defined(PORTNAME_hpux) 
#include "port/hpux/fixade.h"		/* for 8.07 unaligned access fixup */
#endif /* PORTNAME_hpux */

#if defined(PORTNAME_sunos)
#define	memmove(d, s, l)	bcopy(s, d, l)
#endif

#define	NULL_DEV	"/dev/null"

#endif	/* C_H */
