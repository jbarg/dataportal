/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/*-------------------------------------------------------------------------
 *
 * clAuth.h
 *    
 *    Definitions for network authentication routines
 *
 *-------------------------------------------------------------------------
 */
#ifndef CL_AUTH_H
#define	CL_AUTH_H

#include <stdio.h>
#include <string.h>
#ifdef _WIN32
  #undef _UNICODE
  #define MAX_USERNAME_LENGTH 128
  #include <windows.h>
#else
#include <sys/param.h>  /* for MAX{HOSTNAME,PATH}LEN, NOFILE */
#include <unistd.h>
#endif    /* WIN32 */

#include <sys/types.h>
#ifndef _WIN32
#include <pwd.h>
#endif    /* WIN32 */

#include "srb.h"
#include "clAuthExtern.h"
#include "clConnectExtern.h"
#include "clStubExtern.h"
#include "mdasGlobalsExtern.h"
#include "obfExtern.h"
 
#define AUTH_OPT_LEN	20	/* the strlen for auth option -a */

/* The default Auth scheme */

struct serverAuthInfo *DefServerAuthInfo = NULL;
/* ServerAuthInfo contain a link list of serverAuthInfo from MCAT */
struct serverAuthInfo *ServerAuthInfo = NULL;

static struct authsvc authsvcs[] = {
    {PASSWD_AUTH_KEYWD, PASSWD_AUTH_INX},
    {SEA_AUTH_KEYWD, SEA_AUTH_INX},
    {SEA_ENCRYPT_KEYWD, SEA_ENCRYPT_INX},
    {GSI_AUTH_KEYWD, GSI_AUTH_INX},
    {GSI_SECURE_COMM_KEYWD, GSI_SECURE_COMM_INX},
    {AUTH_ENCRYPT1_KEYWD, AUTH_ENCRYPT1_INX},
    {GSI_DELEGATE_KEYWD, GSI_DELEGATE_INX}
};

static int n_authsvcs = sizeof(authsvcs) / sizeof(struct authsvc);

/* struct mdasEnvData *MdasEnvData = NULL; */

/* SrbServerMode - This is a global variable that indicate whether the 
 * process is running in server mode. 0 ==> client mode. 1 ==> server mode */
int SrbServerMode = 0;  /* assume it is in client mode */
int SrbServerClientSock = -1;	/* this is used with SrbServerMode for
				 * server to relay ENCRYPT1 msg */
int ClientCertInx = -1;

extern int aid_setup_creds(char *service_name, int client_flag, int type_flag);
extern int aid_establish_context_clientside(int fd, char *service_name,
int deleg_flag, int type_flag);
extern int aid_establish_context_using_client_delegate(int fd, 
						char *server_dn_in,
                                                char *server_dn_out,
                                                int max_len_server_dn_out,
                                                int received_delegation_fd,
                                                int deleg_flag);
#endif /* CL_AUTH_H */

