

/* srbProxyFunct.h
 * Header file for srbProxyFunct.c
 */

#ifndef SRB_PROXY_FUNCT_H
#define SRB_PROXY_FUNCT_H

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

#include "elogExtern.h" 
 

#endif	/* SRB_PROXY_FUNCT_H */
