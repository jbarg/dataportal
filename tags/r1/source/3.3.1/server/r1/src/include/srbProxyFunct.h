

/* srbProxyFunct.h
 * Header file for srbProxyFunct.c
 */

#ifndef SRB_PROXY_FUNCT_H
#define SRB_PROXY_FUNCT_H
#ifdef REFACTOR
#include <stdio.h>
#include <stdarg.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#ifndef _WIN32
#include <sys/socket.h>
#endif
#if defined(PORTNAME_osx)
#include <stdlib.h>
#include <limits.h>
#include <sys/time.h>
#endif
#ifndef _WIN32
#include <sys/resource.h>
#include <netinet/in.h>
#include <netdb.h>
#endif
#include <string.h>
#include <errno.h>

#ifdef PORTNAME_solaris
#include <ctype.h>
#endif

#include "srb.h"
#include "elogExtern.h"
#include "srbMiscExtern.h"
#include "srbStat.h"
#include "srbObjFunctExtern.h" 
#endif  /* REFACTOR */
#include "serverAll.h"

#endif	/* SRB_PROXY_FUNCT_H */
