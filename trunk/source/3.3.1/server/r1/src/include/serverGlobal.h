/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/



/* serverGlobal.h - Header file for global variables for server codes.
 */

#ifndef SERVER_GLOBAL_H
#define SERVER_GLOBAL_H

/* Ticket user flag */

int TicketUserFlag;     /* 1 = a ticket user; 0 = not a ticket user */
int DbOpened = 0;

userInfo *ClientUser = NULL;
userInfo *ProxyUser = NULL;
userInfo *ThisUser = NULL;
struct mdasEnvData *ThisMdasEnvData = NULL;
char            *DataDir = (char *) NULL;
struct hostElement *LocalHostHead = NULL;
struct hostElement *RemoteHostHead = NULL;
mdasC_sql_result_struct *SrbUserList = NULL;
int ElogMsgToClient = 1;
#ifdef TAPE_DRIVE
tapeLibConfig_t *TapeLibConfigHead = NULL;
#endif

int SvrInitialized = 0; /* SvrInitialized = 1 ==> server has been initialized */


int ServerTimeOut = -1;         /* -1 has not been initialized
                                 * 0 - initialized, but no timeout
                                 * > 0 - the timeout value in seconds
                                 */
int MaxMcatTimeoutForServer = -1; /* The max number of McatTimeout before
                                   * the server also timed out and exit.
                                   */
int McatTimeoutCount = 0;       /* The number of times the Mcat connection has
                                 * timed out */

int MaxThread = DEF_MAX_THREAD;
int SizePerThread = DEF_SIZE_PER_THREAD;

#ifdef FED_MCAT
struct hostElement *LocalMcat = NULL;
#endif

/* This keep track of the last query that where continuation_index >= 0 */
struct hostElement *LastQueryHost = NULL;
int LastContinuationIndex = -1;

struct mdasResInfo *LocalResInfo = NULL;

int SingleSvrPort = 0;	/* Whether server is running using one server port */

#endif	/* SERVER_GLOBAL_H */
