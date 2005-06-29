/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
/* dpssFileDvr.h */

#ifndef DPSS_FILE_DVR_H
#define DPSS_FILE_DVR_H
 
#ifdef DPSS

/* ------------- DPSS-specific includes ----------- */

/* dpss_put -  simple program to copy a file from local disk to the DPSS */

#include <unistd.h>
#include <stdlib.h>
/***#include "issfs/iss_fs.h" raja ***/
#include "dpssfs/dpssfs.h"

extern char *pname, *hname;     /* declared in utils.c */
extern int Debug;               /* for DPSS library debugging */

#define NBYTES 4 * 1024 * 1024  /* number of bytes to send to the DPSS at a
                                 * time */
#define dpssURL "x-dpss://hpgw.lbl.gov/test"
/* ------------------------------------------------ */
 
#include <sys/types.h>
#include <netinet/in.h>

#include "elogExtern.h"
#include "dpssFileDvrExtern.h"
 
typedef unsigned int    Index;
typedef unsigned int        Size;
 
#define df_PrincipalName    "srb"    /* default PrincipalName */
#define df_KeytabName       "/krb5/srb.keytab" /* default _KeytabName */
 

 
/* in hpss
static pthread_t                refreshThread;
static sec_login_handle_t       loginContext;     
static void                     refreshLoginContext( pthread_addr_t unusedArg);
static signed32                 createLoginContext(void);
static char                     *PrincipalName;
static char                     *KeytabName;
*/
 
#define LINE_LEN	512

static int _dpssSrbOpen(char *fileName, int fileFlags, int fileMode);
/*
 * Because dpssOpen is defined by dpss itself, we can't use dpssOpen in the driver.
 * So, we added Srb between dpss and Open ; dpssSrbOpen
 * Same at include/dpssFileDvrExtern.h,  and ./include/exfSw.h and back/dpssFileDvr.c
 */
 
extern int DebugLvl;

#endif  /* DPSS */
#endif  /* DPSS_FILE_DVR_H */
