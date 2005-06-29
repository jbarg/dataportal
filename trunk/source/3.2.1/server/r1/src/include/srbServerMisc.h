/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* srbServerMisc.h - Header file for srbServerMisc.c
 */

#ifndef SRB_SERVER_MISC_H
#define SRB_SERVER_MISC_H

#include <string.h>
#ifndef _WIN32
#include <sys/param.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#ifndef _WIN32
#include <sys/file.h>
#endif
#include <stdio.h>
#include "srb.h"
#ifndef _WIN32
#include <grp.h>
#include <pwd.h>
#endif
#include <signal.h>

#include "c.h"
#include "elogExtern.h"
#include "vault.h"
#include "clAuthExtern.h"
#include "clConnectExtern.h"
#include "commExtern.h"
#include "srbStat.h"
#ifdef TAPE_DRIVE
#include "srbTapeMiscExtern.h"
#endif
#include "srbObjFunctExtern.h"
#include "srbServerMiscExtern.h"
#if defined(HPSS) && defined(NO_DCE)
#include "hpss_api_auth.h"	/* PORTNAME_linux has this ? */
#define HPSS_NODCE_AUTH_FILE    "hpssNodceAuth"
#endif
/* ----- Add by K.Ishikawa 2003-07-02 ----------- */
#include <setjmp.h>
#include <signal.h>

#define COMMAND_DIR	"bin/commands"	/* The directroy with the proxy
					 * command executables located */
#define ALL_NAME "all"                  /* Name used in config file for      *
                                         * lines that apply to all databases */
#define MAX_LINES 255                    /* Maximum number of config lines    *
                                          * that can apply to one database    */
#define HOST_BASE_CONF_FILE "hostAuthConfig"      /* Name of the host based
                                                   * config file */

#define MAX_CONF_TOKEN 80               /* Maximum size of one token in the
                                         * configuration file                */

#define LOCK_DIR	"lockDir"	/* the lock directory */
/*
 * Programs receiving an authentication request use this structure to
 * decide which authentication service types are currently permitted.
 */

struct conf_line {                      /* Info about config file line */
  u_long adr, mask;
};

char *HostBasedConFile = (char *) NULL;
/* The mode of file created in the storage vault */
#define DEF_STOR_VAULT_MODE	0600
#define STOR_VAULT_MODE_KEYWD	"STOR_VAULT_MODE"

int StorVaultMode = DEF_STOR_VAULT_MODE;


extern char            *DataDir;


extern userInfo *ClientUser, *ProxyUser, *ThisUser;
extern int TicketUserFlag;
extern struct hostElement *RemoteHostHead;
extern struct hostElement *LocalHostHead;
/* A list of priviledged SRB user */
mdasC_sql_result_struct *srbUserList = NULL;
extern struct srbObjDesc srbObjDesc[];
extern int RandomSeed;

extern int MaxThread;
extern int SizePerThread;
extern struct hostElement *LocalMcat;

#if defined(HPSS) && defined(NO_DCE)
int readHpssNodceAuth (char *nodceUser, char *nodceAuth);
#endif

#endif 	/* SRB_SERVER_MISC_H */
