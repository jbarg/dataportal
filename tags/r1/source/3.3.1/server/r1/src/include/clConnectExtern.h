/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* clConnectExtern.h - Prototype header file for clConnect.c *
 */

#ifndef CL_CONNECT_EXTERN_H
#define CL_CONNECT_EXTERN_H

#include "srb.h"
#include "dllist.h"
#include "clMiscExtern.h"
#include "mdasC_db2_externs.h"

/*
#define ERROR_MSG_LENGTH 4096
*/

/* Default values for some elements of the struct exf_conn */
 
#define DefaultHost     "localhost"
#define DefaultTty      ""

/* Default MDAS Names */

#define DefMdasDataTypeName "generic"
#define DefMdasResourceName "mda18-unix-sdsc"
#define DefMdasCollectionName "generic"

#define MAX_CONN_SVR_CNT 25
#define CONNECT_TIMEOUT 15	/* 15 sec connect timeout */

/* definition for the server port number return from the srbmaster */

#define NO_NEW_CONNECTION 0

/* srbConn encapsulates a connection to the backend */
typedef struct exf_conn{
  char *srbHost; /* the machine on which the server is running */
  char *srbVersion;  /* version of srb */
  char *srbPort; /* the communication port with the backend */
  char *srbOptions; /* options to start the backend with */
  userInfo *proxyUser;
  userInfo *clientUser;
  char *srbMdasDomain; /* authorization string */
  int status;
  char errorMessage[ERROR_MSG_LENGTH*8];
  /* pipes for be/fe communication */
  FILE *Pfin;
  FILE *Pfout;
  FILE *Pfdebug;
  comBuf *commBuf;
  int sock;
  int sockIn;
  Port *port; /* really a Port* */
  int numThread;
  int authSchemeInx;	/* the auth scheme to use - PASSWD_AUTH_INX, 
			 * SEA_AUTH_INX, SEA_ENCRYPT_INX, 
			 * GSI_AUTH_INX or GSI_SECURE_COMM_INX.
			 */
  char *dn;		/* the server dn on the other side */
} srbConn;

/* srbResult encapsulates the result of a function call */
typedef struct exf_result{
  int resultStatus;
  srbConn* conn;
} srbResult;

#if defined(PORTNAME_sunos)
extern char *sys_errlist[];
#define strerror(A) (sys_errlist[(A)])
#endif /* PORTNAME_sunos */

#ifdef c_plus_plus
extern "C" {
#endif

/*
 * clConnect()  - Establishes a connection to a srbServer through the srbMaster
 * at the specified host and port.
 *
 * Input : char *srbHost - The host address of srbMaster. 
 *	     If the input value in NULL, it will use the env variable 
 *	     "srbHost" if it is defined. If not, it use the hostname of the 
 *	     client machine.
 *         char *srbPort - The port # of srbMaster. If the input value in NULL,
 *	     it will use the env variable "srbPort" if it is defined. 
 *	     If not, it defaults to the default port 5556).
 *         char *srbAuth - The auth string. It is used to define the password 
 *	     for MDAS or SEA authentication. For SEA authentication, this is 
 *	     the password used by the SEA library to decrypt the encrypted 
 *	     private key stored in the file  ~/.SEAuuuuu@ddddd (where uuuuu 
 *	     is the user ID and ddddd is the user domain name). This input is 
 *	     not needed if an unencrypted private key is available in the 
 *	     /tmp directory (generated using the 'seaauth auto' command). 
 *	     To provide additional flexibility, a client may also use the
 *	     env variable "srbAuth" to specify the password. If the MDAS 
 *	     authentication scheme, a client may also supply the password in 
 *	     the ~.srb/.MdasAuth file.  If a client uses more than one method 
 *	     to specify the password, the vaule given in this function 
 *	     call will take precedent, the env variable "srbAuth" comes next 
 *	     and the ~.srb/.MdasAuth file is last.
 *
 * Output - returns a srbConn* which is needed for all subsequent calls.
 * if the status field of the connection returned is CONNECTION_BAD,
 * then some fields may be null'ed out instead of having valid values
 */
extern srbConn* clConnect(char* srbHost, char* srbPort, char* srbAuth);

/*
 * tiUserConnect()  - Establishes a connection to a srbServer through the
 * srbMaster at the specified host and port by a TICKET USER. Normal 
 * authentication will be bypassed for a TICKET USER.
 *
 * Input : char *srbHost - The host address of srbMaster.
 *           If the input value in NULL, it will use the env variable
 *           "srbHost" if it is defined. If not, it use the hostname of the
 *           client machine.
 *         char *srbPort - The port # of srbMaster. If the input value in NULL,
 *           it will use the env variable "srbPort" if it is defined.
 *           If not, it defaults to the default port 5556).
 *
 * Output - returns a srbConn* which is needed for all subsequent calls.
 * if the status field of the connection returned is CONNECTION_BAD,
 * then some fields may be null'ed out instead of having valid values
 */
extern srbConn* tiUserConnect(char *srbHost, char* srbPort);

/* clFinish - Close the current connection and free the srbConn data 
 * structure.
 *
 * Input - srbConn* conn - The connect to close.
 *
 * Output - None.
 */

extern void clFinish(srbConn* conn);

/* clErrorMessage - Returns the ErrorMessage of a connection. This is the
 * error message returned by the SRB server from the current client call.
 *
 * Input - srbConn* conn (from clConnect())
 * Output - The pointer to ErrorMessage
 */

extern char* clErrorMessage(srbConn* conn);


/* clReset - Close the current connection and restablish a new one with 
 * the same parameters 
 *
 * Input - srbConn* conn - The connection to reset.
 *
 * Output - None.
 *
 */

extern void clReset(srbConn* conn);
 
extern char* clHost(srbConn* conn);
extern char* clPort(srbConn* conn);
extern int clStatus(srbConn* conn);
#ifdef foo      /* deplicated */
extern void clTrace(srbConn *conn, FILE* debug_port);
extern void clUntrace(srbConn *conn);
#endif	/* foo */
extern int clPacketSend(Port *port, PacketBuf *buf, PacketLen len);
extern int clPacketReceive(Port *port, PacketBuf *buf, char nonBlocking);
extern void initStartupPack(srbConn *conn, StartupInfo *startup);
extern void startup2PacketBuf(StartupInfo* sInfo, PacketBuf* pBuf);
extern srbConn*
srbConnect(char *srbHost, char* srbPort, char* srbAuth, char *userName,
char *domainName, char *authScheme,  char *serverDn);
extern srbConn *initSrbConn();
extern int
clSetHost (srbConn *conn, struct mdasEnvData *myMdasEnvData, char *srbHost,
char *srbPort);
extern int
srbSetsockopt (int sock);
extern void cPipSigHandler ();
int initPort (srbConn *conn);
int connectPort (srbConn *conn);
int postProcPort (srbConn *conn);

#ifdef c_plus_plus
}
#endif

#endif	/* CL_CONNECT_EXTERN_H */
