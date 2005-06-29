 


/*
 * MDriver.h - Header file for exfFunct.c
 */
 
#ifndef M_DRIVER_H
#define M_DRIVER_H


#if defined(PORTNAME_osx)
#include <sys/types.h>
#include <sys/malloc.h>
#else
#include <malloc.h>
#endif
#include <stdio.h>
#include <sys/file.h>
#include <sys/stat.h>
#include "c.h"
#include "srb.h"
#include "distribExtern.h"

#include "srbMiscExtern.h"
#include "exfFunctExtern.h"
#include "MDriverExtern.h"
#include "clStubExtern.h" 

/* PUT ALL DRIVER HANDLER EXTERN INCLUDE FILES HERE */

#include "antelopeOrbMDriverExtern.h"
#include "datascopeMDriverExtern.h"

/* PUT ALL DRIVER HANDLER EXTERN INCLUDE FILES HERE */
#include "elogExtern.h"
#include <fcntl.h>      /* most O_ */
#ifndef O_RDONLY
#include <sys/file.h>   /* The rest of O_ */
#endif /* O_RDONLY */
 
#include <unistd.h>     /* for SEEK_ */



/* exf descriptor */
 
#define MAX_MDRIVER_DESC 256
#define SRB_SERVER_USER         1       /* the srb server user */
#define SRB_CLIENT_USER         0       /* the client user */
 
extern char *srbUserDomain;
extern userInfo *ProxyUser, *ClientUser;
extern struct hostElement *localHostHead;
extern struct hostElement *remoteHostHead;

static int dummy_rt ();
static void void_dummy_rt ();
static srb_long_t long_dummy_rt ();
static int null_rt ();
static void void_null_rt ();
static srb_long_t long_null_rt ();
static int null_write (int file, char *buffer, int amount);
static int null_read (int file, char *buffer, int amount);


MDriverDesc gMDriverDesc[MAX_MDRIVER_DESC];


extern int TicketUserFlag;


typedef struct md_mgr {
    int         (*md_create)();
    int         (*md_open)();
    int        (*md_close)();
    int         (*md_unlink)();
    int         (*md_read)();
    int         (*md_write)();
    srb_long_t  (*md_seek)();
    int         (*md_sync)();
    int         (*md_proc)();
} md_mgr;


static md_mgr mdsw[] = {
#ifdef ANT_ORB_MD  /** ANT_ORB == 0 **/
    { antelopeOrbCreate,  antelopeOrbOpen,  antelopeOrbClose, 
	antelopeOrbUnlink, antelopeOrbRead,    antelopeOrbWrite, 
	antelopeOrbSeek,  antelopeOrbSync,   antelopeOrbProc
    },
#else /* dummy routines */
    { dummy_rt,   dummy_rt, dummy_rt,      dummy_rt, 
      dummy_rt,   dummy_rt, long_dummy_rt, dummy_rt, dummy_rt
    },
#endif /* ANT_ORB_MD */
#ifdef DATASCOPE_MD  /** DATASCOPE == 1 **/
    { datascopeCreate,  datascopeOpen,  datascopeClose, 
	datascopeUnlink, datascopeRead,    datascopeWrite, 
	datascopeSeek,  datascopeSync,   datascopeProc
    }
#else /* dummy routines */
    { dummy_rt,   dummy_rt, dummy_rt,      dummy_rt, 
      dummy_rt,   dummy_rt, long_dummy_rt, dummy_rt, dummy_rt
    }
#endif /* DATASCOPE_MD */
};


int nMdSwEntries = (sizeof(mdsw) / sizeof(struct md_mgr));


#endif	/* M_DRIVER_H */
