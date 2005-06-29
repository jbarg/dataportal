/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 



/* clAuthExtern.h - Prototypes for the clAuth.c module
 */

#ifndef CL_AUTH_EXTERN_H
#define CL_AUTH_EXTERN_H

#include <errno.h>
#include "srb.h"
#include "clConnectExtern.h"

#ifdef _WIN32
#define CL_MDAS_ENV_FILENAME	"MdasEnv"
#define CL_MDAS_AUTH_FILENAME	"MdasAuth"
#define CL_MDAS_AUTH_FILE       "srb/MdasAuth"
#define CL_MDAS_ENV_FILE        "srb/MdasEnv"
#else
#define CL_MDAS_AUTH_FILE       ".srb/.MdasAuth"
#define CL_MDAS_ENV_FILE        ".srb/.MdasEnv"
#endif

/* The following 3 keywords are set using env variable only */
#define CLIENT_USER_KEYWORD	"clientUserName"  
#define CLIENT_DOMAIN_KEYWORD	"clientDomainName"  
#define CLIENT_MCATZONE_KEYWORD "clientMcatZone"  
#define PROXY_MCATZONE_KEYWORD "proxyMcatZone"  
#define CLIENT_AUTH_KEYWORD	"clientAuth"  
#define CLIENT_PRIV_KEYWORD	"clientPrivFlag"  /* only used by server */ 

/* DOMAIN_KEYWORD and USER_KEYWORD defines the proxyUser's user/domain
 * pair. If CLIIENT_USER_KEYWORD and CLIIENT_DOMAIN_KEYWORD are not
 * defined, they also define the clientUser's user/domain pair */
#define DOMAIN_KEYWORD          "mdasDomainName"  /* DomainName keyword in
                                                   * .MdasEnv file. */
#define DOMAIN_HOME_KEYWORD     "mdasDomainHome"  /* DomainHome keyword in
                                                   * .MdasEnv file. */
#define USER_KEYWORD            "srbUser"         /* user keyword in
                                                   * .MdasEnv file. */
#define DEF_COLL_KEYWORD        "mdasCollectionHome" /* mdasCollectionHome 
						   * keyword in
                                                   * .MdasEnv file. */
#define DEF_RESC_KEYWORD        "defaultResource" /* defaultResource 
						   * keyword in
                                                   * .MdasEnv file. */
#define HOST_KEYWORD            "srbHost"         /* host keyword in
                                                   * .MdasEnv file. */

#define PORT_KEYWORD            "srbPort"         /* port number keyword in
                                                   * .MdasEnv file. */
#define MCAT_ZONE_KEYWORD       "mcatZone"         /* mcat zone keyword in
                                                   * .MdasEnv file. */

/* the Authentication keyword for specifying the Authentication scheme. Used
 * as env variable name and in the .MdasEnv file.
 */
#define AUTH_KEYWD	"AUTH_SCHEME"	
#define AUTH_SERVER_KEYWD "AUTH_SERVER"
#define AUTH_SERVER_DN_KEYWD "SERVER_DN"		/* The server's Gsi Dn */

/* Authentication keyword and index */
#define PASSWD_AUTH_KEYWD	"PASSWD_AUTH"
#define PASSWD_AUTH_INX		0
#define SEA_AUTH_KEYWD		"SEA_AUTH"	/* SEA authentication */
#define SEA_AUTH_INX		1
#define SEA_ENCRYPT_KEYWD	"SEA_ENCRYPT"	/* SEA auth and encryption */
#define SEA_ENCRYPT_INX		2
#define GSI_AUTH_KEYWD		"GSI_AUTH"	/* GSI auth */
#define GSI_AUTH_INX		3
#define GSI_SECURE_COMM_KEYWD	"GSI_SECURE_COMM"	/* GSI secure comm */
#define GSI_SECURE_COMM_INX	4
#define AUTH_ENCRYPT1_KEYWD	"ENCRYPT1"	/* encryption of the challenge random data block using the SRB password*/
#define AUTH_ENCRYPT1_INX       5
#define GSI_DELEGATE_KEYWD		"GSI_DELEGATE"	/* GSI auth */
#define GSI_DELEGATE_INX	6


#ifdef GSI_AUTH
/* definition for the aid_setup_creds() call */
/* The client_flag */
#define GSI_SERVER_FLAG		0	/* It is a server */
#define GSI_CLIENT_FLAG		1	/* It is a client */
/* The type_flag */
#define GSI_TYPE_FLAG 		2 	/* do GUI auth */
/* The delegation flag */
#define NO_DELEG_FLAG			0
#define DO_DELEG_FLAG			1
#endif	/* GSI_AUTH */

/* The default Auth service */
#define DEFAULT_CLIENT_AUTHSVC  AUTH_ENCRYPT1_KEYWD

#ifdef c_plus_plus
extern "C"
{
#endif

struct serverAuthInfo {
	struct authsvc *authsvc;
	char *mcatName;		/* the name of this MCAT if it is a 
				 * MCAT enabled host */
	char *hostAddr;
	char *serverDn;
	struct serverAuthInfo *next;
};

struct authsvc {
    char        keyWord[40];    /* Auth scheme key word */
    int         index;          /* startup packet header type */
};

extern int clSendAuth (StartupInfo *startup, srbConn *conn);
extern int clSendSeaAuth (StartupInfo *startup, srbConn *conn);
extern int clSendMdasAuth (StartupInfo *startup, srbConn *conn);
extern int clAuthKeyWdToInx(char *name);
extern char *clGetUserName(struct mdasEnvData *myMdasEnvData, 
char* clErrorMsg);
extern char *clGetDomainName(struct mdasEnvData *myMdasEnvData,
char* clErrorMsg);
extern char *clGetMcatZone(struct mdasEnvData *myMdasEnvData,
char* clErrorMsg);
extern char *clGetAuth(char* clErrorMsg);
extern int initUserInfo (srbConn *conn, struct mdasEnvData *myMdasEnvData,
char* srbAuth, char *userName, char *domainName, char* clErrorMsg);
extern char *readMdasAuth();
extern struct mdasEnvData *readMdasParam ();
extern int clSetAuthScheme (srbConn *conn, struct mdasEnvData *myMdasEnvData,
char *authScheme);
extern int clSetAuthOption (srbConn *conn, int authSchemeInx);
extern int clRecvStatus (int sock, srbConn *conn);
extern char *clGetServerGsiDn ();
extern char *clGetServerDn (srbConn *conn, char *hostAddr, int authInx);
extern int fillServerAuthInfo (char *hostAddr, char *authKeyWd, char *serverDn,
struct serverAuthInfo *serverAuthInfo);
extern int getMcatServerAuthInfo (srbConn *conn);
extern char * getFromResultStruct(mdasC_sql_result_struct *result,
                              char *tab_name, char *att_name);
extern char *
clAuthInxToKeyWd (int authIndex);
extern int
clSendEncryptAuth (StartupInfo *startup, srbConn *conn);
extern int
svrCliSvrEncryptAuth (int svrSock, int cliSock);
extern int
initCliSvrAuthLink (int cliSock);
extern int
clSendGsiDelegAuth (StartupInfo *startup, srbConn *conn);
extern int
svrSendGsiDelegAuth (int delegateCertInx, srbConn *conn);


#ifdef c_plus_plus
}
#endif

#endif	/* CL_AUTH_EXTERN_H */

