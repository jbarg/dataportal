/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/*-------------------------------------------------------------------------
 *
 * clChksum.h
 *    
 *    Definitions for network authentication routines
 *
 *-------------------------------------------------------------------------
 */
#ifndef CL_CHKSUM_H
#define	CL_CHKSUM_H

#include <stdio.h>
#include <errno.h>
#include <string.h>
#ifdef _WIN32
  #include "Unix2NT.h"
  #undef _UNICODE
  #define MAX_USERNAME_LENGTH 128
  #include <windows.h>
#else
#include <sys/param.h>  /* for MAX{HOSTNAME,PATH}LEN, NOFILE */
#include <unistd.h>
#endif    /* WIN32 */

#include <fcntl.h>

#include <sys/types.h>
#ifndef _WIN32
#include <pwd.h>
#endif    /* WIN32 */

#include "srb.h"
#include "clChksumExtern.h"
#include "clConnectExtern.h"
#include "mdasGlobalsExtern.h"

#ifndef PARAMS
# if defined PROTOTYPES || (defined __STDC__ && __STDC__)
#  define PARAMS(Args) Args
# else
#  define PARAMS(Args) ()
# endif
#endif

ssize_t safe_read PARAMS ((int desc, void *ptr, size_t len));
ssize_t
safe_read (int desc, void *ptr, size_t len);

#endif /* CL_CHKSUM_H */

