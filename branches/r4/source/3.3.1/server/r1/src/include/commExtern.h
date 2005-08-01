/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/*-------------------------------------------------------------------------
 *
 * commExtern.h-- Prototypes for comm.c
 *    
 *
 *
 *-------------------------------------------------------------------------
 */
#ifndef COMM_EXTERN_H
#define	COMM_EXTERN_H


#include "clConnectExtern.h"
#include "srbMiscExtern.h"
#include "mdasGlobalsExtern.h"

#define HOST_CONFIG    "hostConfig"   /* The host config file */
#define MCAT_HOST      "mcatHost"     /* The config file that contain the MCAT 
				       * host file */
#ifdef FED_MCAT
#define FED_MCAT_HOST  "fedMcatHost"  /* The config file federated MCAT
				       * host file */
#endif
#define RS_BEGIN_READ      0          /* The first line in the HOST_CONFIG
                                       * is the localhost */
#define RS_GOT_LOCALHOST   1          /* The remaining are remote hosts */

#define MDAS_ENABLED_FLAG       "MDAS_ENABLED"

/* defineition for the mode input in svrGetAuthScheme () */
#define CLIENT_MODE	0	/* Server to client */
#define SERVER_MODE	1	/* Server to server */

/* Min value for server timeout in seconds */
#define   MIN_SERVER_TIMEOUT_TIME       7200

/* struct for the host configuration */

struct hostElement {
    struct nameElement *alias;  /* Link list of valid name for this host */
    char *portNum;
    srbConn* conn;
    int mdasEnabled;            /* Indicate if the host is mdasEnabled.
                                 * 0 - no, 1 local MCAT,  2 - remote MCAT*/
    struct serverAuthInfo *serverAuthInfo;  /* The serverAuthInfo. For now,
					     * only avail for MCAT enabled
					     * host */
    struct hostElement *zoneHostTab;
    struct hostElement *next;
};

/* definition for mdasEnabled in struct hostElement */
#define NOT_MCAT_ENABLED	0
#define MCAT_ENABLED_LOCAL	1
#define MCAT_ENABLED_REMOTE	2

struct nameElement {
    char *hostName;
    struct nameElement *next;
};

/* This is the Resource Info struct for each Physical resource belonging
 * to a Logical Resource.
 */

struct mdasResInfo {            /* output struct */
    char *phyResType;           /* The resource type */
    char *phyResLoc;            /* The resource Location */
    char *phyResName;           /* Physical resource name */
    char *phyDirPathName;       /* Default dir name for physical resource */
    char *logDirPathName;       /* Default dir name for lodical resource */
    char *phyResClass;          /* Physical resource class */
    srb_long_t  phyResMaxObjSize; /* Max object size */
    srb_long_t freeSpace;       /* free space in bytes */
    time_t freeSpaceTime;	/* time when freeSpace was checked */
    int flag;                   /* LOCAL_HOST ? */
    struct mdasResInfo *compoundLink;
    struct mdasResInfo *next;
};

#define RES_FREE_SPACE_CHK_TIME	 1800	/* how often we should check 
				         * freeSpace. every 1800 sec */

extern struct srbObjTypeMap srbObjTypeEntry[];

#ifdef _WIN32
#include <winsock.h>
#else
extern char *inet_ntoa ();
#endif /* WIN32 */

/* extern FILE *Pfout, *Pfin; */
extern int CLAsyncNotifyWaiting;

extern int newCommGetc(svrComm_t *myComm, int authSchemeInx);
extern int commGetc(svrComm_t *myComm, int authSchemeInx);
extern void commInit(svrComm_t *myComm, int fd);
extern int commGetport();
extern void commClose(svrComm_t *myComm);
extern void commFlush(svrComm_t *myComm, int authSchemeInx);
extern int commGetstr(svrComm_t *myComm, char *s, int maxlen, 
int authSchemeInx);
extern int commGetnchar(svrComm_t *myComm, char *s, int off, int maxlen, 
int authSchemeInx);
extern int commGetint(svrComm_t *myComm, int b, int authSchemeInx);
extern srb_long_t *commGetlong(svrComm_t *myComm, int b, int authSchemeInx);
extern int commFlushInp (svrComm_t *myComm, int authSchemeInx);
extern int newCommRead(svrComm_t *myComm, char *buf, int len, 
int authSchemeInx);
extern void commPutstr(svrComm_t *myComm, char *s, int authSchemeInx);
extern void commPutnchar(svrComm_t *myComm, char *s, int n, int authSchemeInx);
extern void commPutint(svrComm_t *myComm, int i, int b, int authSchemeInx);
extern int newFlush(svrComm_t *myComm, int authSchemeInx);
extern int newCommPutc(svrComm_t *myComm, char c, int authSchemeInx);
extern int newCommPuts(svrComm_t *myComm, char *s, int authSchemeInx);
extern void commPutstr(svrComm_t *myComm, char *s, int authSchemeInx);
extern int commGetinaddr(struct sockaddr_in *sin, 
char *host, int port);
extern void
commPutlong(svrComm_t *myComm, srb_long_t *inpLong, int b, int authSchemeInx);
extern int streamServerPort(char *hostName, short portName, int *fdP);
extern int streamConnection(int server_fd, Port *port);
extern void streamClose(int sock);
extern void srbDebug(char *target, char *msg);
extern void commTrace();
extern void commUntrace();
extern int packetReceive(Port *port, char nonBlocking);
extern int packetSend(Port *port, PacketLen len);
extern int initSrbHost();
extern int initHostWithMCat ();
extern mdasC_sql_result_struct *queryResInfo ();
extern struct hostElement *enqueHost (char *hostName, char *portNum,
struct hostElement **queHead);
extern void enqueName (char *name, struct hostElement *hostElement);
extern int remoteConnect (struct hostElement  *hostTabPtr);
extern void remoteDisconnect (struct hostElement  *hostTabPtr);
extern void remoteClose (struct hostElement  *hostTabPtr);
extern int getMdasEnabledHost(char *mcatName, 
struct hostElement **myHhostElement);
extern srbConn* getRemoteConn (struct hostElement  *hostTabPtr);
extern int queryDataInfo (char *mcatName, int selval[], char qval[][MAX_TOKEN], 
mdasC_sql_result_struct **myresult, int rowsWanted);
extern int
genQueryDataInfo (int cat_type, char *myMcatZone, genQuery_t *myGenQuery,
mdasC_sql_result_struct **myresult, int rowsWanted);
extern char *
getMcatHostHintByQval (char qval[][MAX_TOKEN]);
extern srbConn *svrConnect(char *srbHost, char* srbPort, char* srbAuth,
int proxyUserFlag, struct hostElement *hostTabPtr);
extern int svrConnectSvr (srbConn *conn);
extern int svrSendAuth(srbConn *conn);
extern int svrSendGsiAuth (srbConn *conn);
extern int svrSetAuthScheme (srbConn *conn, struct mdasEnvData *myMdasEnvData,
struct hostElement  *hostTabPtr);
extern int svrGetAuthScheme (int mode);
extern char readCommand(svrComm_t *myComm, char *inBuf);
extern char *
getLocalHostNetAddr ();
extern int
getHostAuthInxFromHostElement (char *hostName);
extern char *
getDnFromHostElement (char *hostName);
extern int
_svrGetMoreRows (int catalog, int contDesc, mdasC_sql_result_struct **myresult,
int rowsWanted);
extern int
checkSameMcat (char *path1, char *path2);
extern int
remoteTiConnect (struct hostElement  *hostTabPtr);
extern int
getZoneFlag (char *srcMcatZoneHint, char *destMcatZoneHint);
#ifdef FED_MCAT
extern mdasC_sql_result_struct * getMcatZoneInfo ();
#endif
extern int
dupUserInfo (userInfo *destUserInfo, userInfo *srcUserInfo);
extern char *
getMcatHostHintByGenQuery (genQuery_t *myGenQuery);
extern int
setZoneForHostElement (struct hostElement *myHElement, char *myMcatName);

#endif	/* COMM_EXTERN_H */

