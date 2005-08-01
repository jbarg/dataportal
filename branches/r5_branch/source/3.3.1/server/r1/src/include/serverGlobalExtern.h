
/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/



/* serverGlobalExtern.h - Header file for global extern for server codes.
 */

#ifndef SERVER_GLOBAL_EXTERN_H
#define SERVER_GLOBAL_EXTERN_H
/* Ticket user flag */

extern int TicketUserFlag;     /* 1 = a ticket user; 0 = not a ticket user */
extern int DbOpened;

extern userInfo *ClientUser;
extern userInfo *ProxyUser;
extern userInfo *ThisUser;
extern struct mdasEnvData *ThisMdasEnvData;
extern char            *DataDir;
extern struct hostElement *LocalHostHead;
extern struct hostElement *RemoteHostHead;
extern mdasC_sql_result_struct *SrbUserList;
extern int ElogMsgToClient;
#ifdef TAPE_DRIVE
extern tapeLibConfig_t *TapeLibConfigHead;
#endif

extern int SvrInitialized; 	/* SvrInitialized = 1 ==> server has 
				* been initialized */

extern int ServerTimeOut;       /* -1 has not been initialized
                                 * 0 - initialized, but no timeout
                                 * > 0 - the timeout value in seconds
                                 */
extern int MaxMcatTimeoutForServer; /* The max number of McatTimeout before
                                   * the server also timed out and exit.
                                   */
extern int McatTimeoutCount;   	/* The number of times the Mcat connection has
                                 * timed out */

extern int MaxThread;
extern int SizePerThread;

#ifdef FED_MCAT
extern struct hostElement *LocalMcat;
#endif

extern struct hostElement *LastQueryHost;
extern int LastContinuationIndex;

extern struct mdasResInfo *LocalResInfo;

extern int SingleSvrPort;

#endif	/* SERVER_GLOBAL_EXTERN_H */
