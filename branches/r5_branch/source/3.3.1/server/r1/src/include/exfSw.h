/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* exfSw.h - Header file for exfSw.c
 */

#ifndef EXF_SW_H
#define EXF_SW_H

#ifdef REFACTOR
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include "srb.h"
#include "exfSwExtern.h"
#include "elogExtern.h"
#include "unixFileDvrExtern.h"
#include "utFileDvrExtern.h"
#include "hpssFileDvrExtern.h"
#include "dpssFileDvrExtern.h"
#include "ftpFileDvrExtern.h"
#include "srbC_httpc_libraryExtern.h"
#ifdef _WIN32
#include "ntFileDvrExtern.h"
#endif
#ifdef ADS
#include "adsFileDvrExtern.h"
#endif

#ifdef GRID_FTP
#include "gridftpDvr.h"
#endif
#endif  /* REFACTOR */
#include "serverAll.h"

#ifdef _WIN32
#define unixCreate  ntCreate
#define unixOpen    ntOpen
#define unixClose   ntClose
#define unixUnlink  ntUnlink
#define unixRead    ntRead
#define unixWrite   ntWrite
#define unixSeek    ntSeek
#define unixSync    ntSync
#define unixStat    ntStat
#define unixFstat   ntFstat
#define unixMkdir   ntMkdir
#define unixChmod   ntChmod
#define unixRmdir   ntRmdir
#define unixOpendir ntOpendir
#define unixClosedir    ntClosedir
#define unixReaddir ntReaddir
#endif
 
typedef struct exf_mgr {
    int         (*exf_create)();
    int         (*exf_open)();
    int        (*exf_close)();
    int         (*exf_unlink)();
    int         (*exf_read)();
    int         (*exf_write)();
    srb_long_t  (*exf_seek)();
    int         (*exf_sync)();
    int         (*exf_stat)();
    int         (*exf_fstat)();
    int         (*exf_mkdir)();
    int         (*exf_chmod)();
    int         (*exf_rmdir)();
    int         (*exf_opendir)();
    int        (*exf_closedir)();
    int         (*exf_readdir)();
    int         (*exf_setstorattri)();
    int         (*exf_migrate)();
    int         (*exf_stage)();
    int         (*exf_purge)();
    srb_long_t  (*exf_getFsFreeSpace)();
    int         (*exf_chown)();
    int         (*exf_fstage)();
} exf_mgr;

static int dummy_rt ();
static void void_dummy_rt ();
static srb_long_t long_dummy_rt ();
static int null_rt ();
static void void_null_rt ();
static srb_long_t long_null_rt ();
static int null_write (int file, char *buffer, int amount);
static int null_read (int file, char *buffer, int amount);



#ifdef _WIN32
#define unixCreate  ntCreate
#define unixOpen    ntOpen
#define unixClose   ntClose
#define unixUnlink  ntUnlink
#define unixRead    ntRead
#define unixWrite   ntWrite
#define unixSeek    ntSeek
#define unixSync    ntSync
#define unixStat    ntStat
#define unixFstat   ntFstat
#define unixMkdir   ntMkdir
#define unixChmod   ntChmod
#define unixRmdir   ntRmdir
#define unixOpendir ntOpendir
#define unixClosedir    ntClosedir
#define unixReaddir ntReaddir
#define unixGetFsFreeSpace long_dummy_rt
#define unixChown dummy_rt
#endif



static exf_mgr exfsw[] = {
 
    /* UNIX */
    { unixCreate,  unixOpen, unixClose,
      unixUnlink, unixRead, unixWrite, unixSeek, unixSync, unixStat, unixFstat,
      unixMkdir, unixChmod, unixRmdir, unixOpendir, unixClosedir, unixReaddir,
      dummy_rt , dummy_rt, dummy_rt, dummy_rt, unixGetFsFreeSpace, unixChown,
#ifdef SAMFS_STAGE
      samfsFStage}
#else
      dummy_rt} 
#endif
#ifdef UTREE
    /* UniTree */
    , { utCreate, utOpen, utClose,
      utUnlink, utRead, utWrite, utSeek,
      utSync, utStat, utFstat, utMkdir, utChmod,
      utRmdir, utOpendir, utClosedir, utReaddir, 
      dummy_rt, dummy_rt, dummy_rt, dummy_rt, long_dummy_rt, dummy_rt, dummy_rt}
#else
     /* dummy routines */
    , { dummy_rt, dummy_rt, dummy_rt,  dummy_rt, dummy_rt,
        dummy_rt, long_dummy_rt, dummy_rt, dummy_rt, dummy_rt, dummy_rt,
        dummy_rt , dummy_rt, dummy_rt, dummy_rt, dummy_rt,
	dummy_rt, dummy_rt, dummy_rt, dummy_rt, long_dummy_rt, dummy_rt, 
	dummy_rt}
#endif /* UTREE */
#ifdef HPSS
    /* HPSS */
    , { hpssCreate, hpssOpen, hpssClose,
      hpssUnlink, hpssRead, hpssWrite, hpssSeek,
      hpssSync, hpssStat, hpssFstat, hpssMkdir, hpssChmod,
      hpssRmdir, hpssOpendir, hpssClosedir, hpssReaddir,
      dummy_rt, hpssMigrate, hpssStage, hpssPurge, long_dummy_rt, dummy_rt, 
	dummy_rt}
#else
     /* dummy routines */
    , { dummy_rt, dummy_rt, dummy_rt, dummy_rt, dummy_rt,
        dummy_rt, long_dummy_rt, dummy_rt, dummy_rt, dummy_rt, dummy_rt,
        dummy_rt , dummy_rt, dummy_rt, dummy_rt, dummy_rt,
        dummy_rt, dummy_rt, dummy_rt, dummy_rt, long_dummy_rt, dummy_rt, 
	dummy_rt}
#endif /* HPSS */
#ifdef FTP
    /* FTP */
    , { ftpCreate, ftpOpen, ftpClose,
      ftpUnlink, ftpRead, ftpWrite, ftpSeek,
      ftpSync, ftpStat, dummy_rt, ftpMkdir, ftpChmod,
      ftpRmdir, ftpOpendir, ftpClosedir, ftpReaddir,
      ftpSetStorAttri, dummy_rt, dummy_rt, dummy_rt, long_dummy_rt, dummy_rt, 
	dummy_rt}
#else
     /* dummy routines */
    , { dummy_rt, dummy_rt, dummy_rt,  dummy_rt, dummy_rt,
        dummy_rt, long_dummy_rt, dummy_rt, dummy_rt, dummy_rt, dummy_rt,
        dummy_rt , dummy_rt, dummy_rt, dummy_rt, dummy_rt,
        dummy_rt, dummy_rt, dummy_rt, dummy_rt, long_dummy_rt, dummy_rt, 
	dummy_rt}
#endif /* FTP */
#ifdef HTTP
    /* HTTP */
    , { dummy_rt, srbC_httpc_open, srbC_httpc_close, dummy_rt, srbC_httpc_read, 
        dummy_rt, long_dummy_rt, dummy_rt, dummy_rt, dummy_rt, dummy_rt,
        dummy_rt , dummy_rt, dummy_rt, dummy_rt, dummy_rt,
        dummy_rt, dummy_rt, dummy_rt, dummy_rt, long_dummy_rt, dummy_rt, 
	dummy_rt}
#else
     /* dummy routines */
    , { dummy_rt, dummy_rt, dummy_rt,  dummy_rt, dummy_rt,
        dummy_rt, long_dummy_rt, dummy_rt, dummy_rt, dummy_rt, dummy_rt,
        dummy_rt , dummy_rt, dummy_rt, dummy_rt, dummy_rt,
        dummy_rt, dummy_rt, dummy_rt, dummy_rt, long_dummy_rt, dummy_rt, 
	dummy_rt}
#endif /* HTTP */
     /* NULL_FT routines */
    , { null_rt, null_rt, null_rt,  null_rt, null_read,
        null_write, long_null_rt, null_rt, null_rt, null_rt, null_rt,
        null_rt , null_rt, null_rt, null_rt, null_rt,
        null_rt, null_rt, null_rt, null_rt, long_null_rt, dummy_rt, dummy_rt}
#ifdef DPSS
    /* DPSS */
    , { dpssSrbCreate, dpssSrbOpen, dpssSrbClose,
	dpssSrbUnlink, dpssSrbRead, dpssSrbWrite, dpssSrbSeek,
	dpssSrbSync, dpssSrbStat, dummy_rt, dpssSrbMkdir, dpssSrbChmod,
	dpssSrbRmdir, dpssSrbOpendir, dpssSrbClosedir, dpssSrbReaddir,
	dummy_rt, dummy_rt, dummy_rt, dummy_rt, long_dummy_rt, dummy_rt, 
	dummy_rt}
#else
    /* dummy routines */
    , { dummy_rt, dummy_rt, dummy_rt,  dummy_rt, dummy_rt,
        dummy_rt, long_dummy_rt, dummy_rt, dummy_rt, dummy_rt, dummy_rt,
        dummy_rt , dummy_rt, dummy_rt, dummy_rt, dummy_rt,
	dummy_rt, dummy_rt, dummy_rt, dummy_rt, long_dummy_rt, dummy_rt, 
	dummy_rt}
#endif /* DPSS */

#ifdef NTFS
    ,{ ntCreate,  ntOpen, ntClose,
      ntUnlink, ntRead, ntWrite, ntSeek, ntSync, ntStat, ntFstat,
      ntMkdir, ntChmod, ntRmdir, ntOpendir, ntClosedir, ntReaddir,
      dummy_rt , dummy_rt, dummy_rt, dummy_rt, long_dummy_rt, dummy_rt, 
	dummy_rt}
#else
    , { dummy_rt, dummy_rt, dummy_rt,  dummy_rt, dummy_rt,
        dummy_rt, long_dummy_rt, dummy_rt, dummy_rt, dummy_rt, dummy_rt,
        dummy_rt , dummy_rt, dummy_rt, dummy_rt, dummy_rt,
    dummy_rt, dummy_rt, dummy_rt, dummy_rt, long_dummy_rt, dummy_rt, dummy_rt}
#endif
#ifdef ADS
    /* ADS */
    , { adsCreate, adsOpen, adsClose,
      adsUnlink, adsRead, adsWrite, adsSeek,
      adsSync, adsStat, adsFstat, adsMkdir, adsChmod,
      adsRmdir, adsOpendir, adsClosedir, adsReaddir,
      /* dummy_rt, dummy_rt, dummy_rt, dummy_rt, */
      dummy_rt, dummy_rt, dummy_rt, dummy_rt, long_dummy_rt, dummy_rt, dummy_rt}
#else
     /* dummy routines */
    , { dummy_rt, dummy_rt, dummy_rt,  dummy_rt, dummy_rt,
        dummy_rt, long_dummy_rt, dummy_rt, dummy_rt, dummy_rt, dummy_rt,
        dummy_rt , dummy_rt, dummy_rt, dummy_rt, dummy_rt,
        dummy_rt, dummy_rt, dummy_rt, dummy_rt, long_dummy_rt, dummy_rt, 
	dummy_rt}
#endif /* ADS */
#ifdef CMSTORE	/* place holder for CMSTORE */
     /* dummy routines */
    , { dummy_rt, dummy_rt, dummy_rt,  dummy_rt, dummy_rt,
        dummy_rt, long_dummy_rt, dummy_rt, dummy_rt, dummy_rt, dummy_rt,
        dummy_rt , dummy_rt, dummy_rt, dummy_rt, dummy_rt,
        dummy_rt, dummy_rt, dummy_rt, dummy_rt, long_dummy_rt, dummy_rt, 
	dummy_rt}
#else
     /* dummy routines */
    , { dummy_rt, dummy_rt, dummy_rt,  dummy_rt, dummy_rt,
        dummy_rt, long_dummy_rt, dummy_rt, dummy_rt, dummy_rt, dummy_rt,
        dummy_rt , dummy_rt, dummy_rt, dummy_rt, dummy_rt,
        dummy_rt, dummy_rt, dummy_rt, dummy_rt, long_dummy_rt, dummy_rt, 
	dummy_rt}
#endif /* CMSTORE */
#ifdef DCACHE  /* place holder for fnal/DESY dCache file type */
     /* dummy routines */
    , { dummy_rt, dummy_rt, dummy_rt,  dummy_rt, dummy_rt,
        dummy_rt, long_dummy_rt, dummy_rt, dummy_rt, dummy_rt, dummy_rt,
        dummy_rt , dummy_rt, dummy_rt, dummy_rt, dummy_rt,
        dummy_rt, dummy_rt, dummy_rt, dummy_rt, long_dummy_rt, dummy_rt, 
	dummy_rt}
#else
     /* dummy routines */
    , { dummy_rt, dummy_rt, dummy_rt,  dummy_rt, dummy_rt,
        dummy_rt, long_dummy_rt, dummy_rt, dummy_rt, dummy_rt, dummy_rt,
        dummy_rt , dummy_rt, dummy_rt, dummy_rt, dummy_rt,
        dummy_rt, dummy_rt, dummy_rt, dummy_rt, long_dummy_rt, dummy_rt, 
	dummy_rt}
#endif /* dCache */
#ifdef GRID_FTP
    , { gridftpCreate, gridftpOpen, gridftpClose, gridftpUnlink, gridftpRead, 
	gridftpWrite, long_dummy_rt, dummy_rt, gridftpStat, gridftpFstat, 
	gridftpMkdir, dummy_rt, gridftpRmdir, dummy_rt, dummy_rt, dummy_rt,
        dummy_rt, dummy_rt, dummy_rt, dummy_rt, long_dummy_rt, dummy_rt, 
	dummy_rt}
#else
     /* dummy routines */
    , { dummy_rt, dummy_rt, dummy_rt,  dummy_rt, dummy_rt,
        dummy_rt, long_dummy_rt, dummy_rt, dummy_rt, dummy_rt, dummy_rt,
        dummy_rt , dummy_rt, dummy_rt, dummy_rt, dummy_rt,
        dummy_rt, dummy_rt, dummy_rt, dummy_rt, long_dummy_rt, dummy_rt, 
	dummy_rt}
#endif
};
 
/* Number of sw tables */

int nExfswEntries = 
  (sizeof(exfsw) / sizeof(struct exf_mgr));

#endif	/* EXF_SW_H */
