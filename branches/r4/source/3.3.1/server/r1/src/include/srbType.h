/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/* srbType.h - Contains info on typedef for the SRB code.
 */

#ifndef SRB_TYPE_H
#define SRB_TYPE_H

#include <sys/types.h>
#if defined(PORTNAME_linux)
#include <fcntl.h>
#endif

#if defined(PORTNAME_c90)
typedef long srb_long_t;
#endif
#if defined(PORTNAME_sgi)
typedef __int64_t srb_long_t;
#endif
#if defined(PORTNAME_solaris) || defined(PORTNAME_aix) || defined(PORTNAME_linux) || defined(PORTNAME_sunos) || defined(PORTNAME_alpha)
typedef long long srb_long_t;
#endif	/* PORTNAME_solaris */

#if defined(_WIN32)
typedef __int64 srb_long_t;
typedef __int64 u_longlong_t;
typedef unsigned long ulong;
#endif  /* WIN32 */

#if defined(PORTNAME_osx)
typedef int64_t srb_long_t;
#endif

#if defined(PORTNAME_linux) || defined(PORTNAME_alpha) /* True64 Alpha PSC port */
typedef unsigned long long u_longlong_t;
#endif

#if defined(PORTNAME_osx)
typedef u_int64_t u_longlong_t;
typedef u_long ulong;
#endif

#if defined(PORTNAME_sgi)
typedef int64_t u_longlong_t;
#endif

#if defined(PORTNAME_osx)
/* use strtol for now since strtoq does not work */
#define strtoll strtoq
#endif

#if defined(PORTNAME_alpha)  /* True64 Alpha PSC port */ 
/* use strtol for now since strtoq does not work */
#define strtoll strtol
#endif

/* Common Lock command definition - equiv to those defined in fcntl.h */
#define SRB_F_SETLK         6       /* Set file lock */
#define SRB_F_SETLKW        7       /* Set file lock and wait */
#define SRB_F_GETLK         14      /* Get file lock */
#define SRB_F_RDLCK         01      /* Read lock */
#define SRB_F_WRLCK         02      /* Write lock */
#define SRB_F_UNLCK         03      /* Remove lock(s) */

#if defined(PORTNAME_linux) || defined(PORTNAME_alpha) || defined(PORTNAME_osx)
  /* True64 Alpha PSC port */ 
typedef struct flock flock_t;
#endif

#endif	/* SRB_TYPE_H */
