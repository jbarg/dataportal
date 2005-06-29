
/* serverAll.h - The header file all srb server modules. It is not to be used
 * in any client codes such as those in the "client" directory and
 * functionHandler.h
 *
 *
 *-------------------------------------------------------------------------
 */
#ifndef SERVER_ALL_H
#define SERVER_ALL_H

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <time.h>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <unistd.h>             /* for ttyname() */
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <grp.h>
#include <pwd.h>
#include <sys/param.h>  /* for MAX{HOSTNAME,PATH}LEN, NOFILE */
#include <setjmp.h>
#endif

#if defined(PORTNAME_solaris)
#include <strings.h>
#include <arpa/inet.h>
#endif

#if defined(PORTNAME_osx)
#include <sys/malloc.h>
#else
#include <malloc.h>
#endif
#ifndef O_RDONLY
#ifndef _WIN32
#include <sys/file.h>   /* The rest of O_ */
#endif
#endif /* O_RDONLY */

#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>

#ifdef PORTNAME_linux
#ifndef SOMAXCONN
#define SOMAXCONN 5             /* from Linux listen(2) man page */
#endif /* SOMAXCONN */
#endif /* PORTNAME_linux */

#include "c.h"
#include "srb.h"
#include "vault.h"
#include "elogExtern.h"
#include "srbMiscExtern.h"
#include "srbStat.h"
#include "srbServerExtern.h"
#include "functionHandlerExtern.h"
#include "exfFunctExtern.h"
#include "dbLobjExtern.h"
#include "dbTableDefines.h"
#ifdef SRB_MDAS
#include "dbTableExtern.h"
#include "srbC_db2_externs.h"
#include "srbC_ora_externs.h"
#endif
#include "distribExtern.h"
#ifdef TAPE_DRIVE
#include "srbTapeMiscExtern.h"
#endif
#include "commExtern.h"
#include "authExtern.h"
#include "srbServerSfoExtern.h"
#include "compoundObjExtern.h"
#ifdef SRB_MDAS
#include "mdasC_db2_externs.h"
#endif
#include "srbC_mdas_externs.h"
#include "mdasPrototypes.h"
#include "srbDirent.h"
#include "exfSwExtern.h"
#include "srbServerMiscExtern.h"
#include "clMiscExtern.h"
#include "clAuthExtern.h"
#include "clStubExtern.h"
#include "clStubIntern.h"
#include "objMiscIoFunctExtern.h"
#include "objParaIoFunctExtern.h"
#include "objMcatAdminFunctExtern.h"
#include "objMcatOprFunctExtern.h"
#include "objSeqIoFunctExtern.h"
#include "objContainerFunctExtern.h"
#include "objBulkFunctExtern.h"
#include "unixFileDvrExtern.h"
#include "utFileDvrExtern.h"
#include "hpssFileDvrExtern.h"
#include "dpssFileDvrExtern.h"
#include "ftpFileDvrExtern.h"
#include "srbC_httpc_libraryExtern.h"
#include "MDriverExtern.h"
/* Raja Jan 25,2005  - removed as causes trouble with elog in antelope system 
#include "antelopeOrbMDriverExtern.h"
#include "datascopeMDriverExtern.h"
Raja Jan 25,2005  - removed as causes trouble with elog in antelope system */
#include "srbSecureComm.h"
#if defined(HPSS) && defined(NO_DCE)
#include "hpss_api_auth.h"      /* PORTNAME_linux has this ? */
#define HPSS_NODCE_AUTH_FILE    "hpssNodceAuth"
#endif
/* ----- Add by K.Ishikawa 2003-07-02 ----------- */

#ifdef ADR_PROXY
#include "adrProxyOprExtern.h"
#ifndef TEST_ADR_PROXY
#include "sfoDCIndex.h"
#include "sfoDCFilter.h"
#include "llaccess.h"
#endif
#endif

#ifdef _WIN32
#include "ntFileDvrExtern.h"
#endif
#ifdef ADS
#include "adsFileDvrExtern.h"
#endif

#ifdef GRID_FTP
#include "gridftpDvr.h"
#endif

#endif	/* SERVER_ALL_H */
