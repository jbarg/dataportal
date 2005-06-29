/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* srbC_httpc_library.h - Header for for srbC_httpc_library.c
 */

#ifndef SRBC_HTTPC_LIBRARY_H
#define SRBC_HTTPC_LIBRARY_H

#include <stdlib.h>
#include <sys/types.h>
#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "c.h"
#include "srb.h"
#include "elogExtern.h"
#include "srbMiscExtern.h"
#include "srbC_httpc_libraryExtern.h"

#define URL_HEADER	"://"
#define HTTP_HEADER	"http://"
#define DEF_HTTP_PORT	80	/* default http port */

#endif  /* SRBC_HTTPC_LIBRARY_H */
