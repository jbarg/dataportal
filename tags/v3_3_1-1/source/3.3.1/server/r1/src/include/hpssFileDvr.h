/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* hpssFileDvr.h */

#ifndef HPSS_FILE_DVR_H
#define HPSS_FILE_DVR_H
 
#include <stdio.h>
#include <sys/file.h>
#include <sys/param.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#if defined(PORTNAME_osx)
#include <sys/types.h>
#include <sys/malloc.h>
#else
#include <malloc.h>
#endif
#include <dirent.h>
 
#include <fcntl.h>      /* most O_ */
#ifndef O_RDONLY
#include <sys/file.h>   /* The rest of O_ */
#endif /* O_RDONLY */
#include <unistd.h>     /* for SEEK_ */
 
#ifdef HPSS
/* ------------- HPSS-specific includes ----------- */
#include "u_signed64.h"
#include "hpss_api.h"
#include <sys/time.h>
#ifndef NO_DCE
#include <dce/rpc.h>
#include <dce/keymgmt.h>
#include <dce/sec_login.h>
#include "hpsscomm.h"
#endif
#include "hpss_errno.h"
#include "srbErrno.h"
/* ------------------------------------------------ */
 
#include <sys/types.h>
#include <netinet/in.h>

#include "elogExtern.h"
#include "srbType.h"
#include "srbStat.h"
#include "hpssFileDvrExtern.h"
/* ----- Add by K.Ishikawa 2003-07-02 ----------- */
#include <setjmp.h>
#include <signal.h>
 
typedef unsigned int    Index;
typedef unsigned int        Size;
 
#define df_PrincipalName    "srb"    /* default PrincipalName */
#define df_KeytabName       "/krb5/srb.keytab" /* default _KeytabName */
 
#ifndef NO_DCE
static pthread_t                refreshThread;
 
static sec_login_handle_t       loginContext;     /* Build context here */
static void                     refreshLoginContext( pthread_addr_t unusedArg);
static signed32                 createLoginContext(void);
static char                     *PrincipalName;
static char                     *KeytabName;
#endif
 
#define  DEFAULT_COS             10   /* COS for medium/large files */
#define  HPSS_COS_CONFIG	"hpssCosConfig"  /* The hpss class of service
						  * config file.
						  */
#define DEF_COS_KEY	"default_cos"	/* the keyword for the default cos */
static int HpssDefCos; 

#define LINE_LEN	512
struct hpssCosTable {
    int cos;	/* class of service */
    srb_long_t maxKByteSz;	/* Max size of the class in KBytes */
    struct hpssCosTable *next;
};

static struct hpssCosTable *hpssCosHead;

int _hpssOpen(char *fileName, int fileFlags, int fileMode, srb_long_t size);
int enqueCos (struct hpssCosTable *cosTable);
int convStat32ToStat (struct stat32 *mystat32, struct stat *mystat);
 
extern char *getSrbDataDir();
extern int DebugLvl;

char cpFunctionName[256]="none_set_yet", cpLastFileName[1024]="";
unsigned int    uiSecs=300;
jmp_buf         jenv;

#define MIN_5 5*60
#define MIN_20 20*60
#define MIN_30 30*60

#endif  /* HPSS */
#endif  /* HPSS_FILE_DVR_H */
