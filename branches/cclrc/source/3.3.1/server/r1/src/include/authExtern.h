/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/*-------------------------------------------------------------------------
 *
 * authExtern.h--
 *    Definitions and prototypes for auth.c
 *
 *
 *-------------------------------------------------------------------------
 */
#ifndef AUTH_EXTERN_H
#define AUTH_EXTERN_H

#include "srbMiscExtern.h"
#include "clConnectExtern.h"
#include "packStruct.h"
#include "clStubIntern.h"
#include "distribExtern.h"

extern int svrCheckAuth (int sock, char *usreName, char *domainName, 
char *mcatZone);
extern int
chkMdasAuth (int sock, char *userName, char *mdasDomain);
extern int
chkSeaAuth (int sock, char *userName, char *mdasDomain);
extern int
enableSeaEncrypt (int sock, char *userName, char *mdasDomain);
extern int chkEncryptAuth (int sock, char *userName, char *mdasDomain,
char *mcatZone);
extern int
chkGsiDelegAuth (int sock, char *userName, char *mdasDomain);
extern int
chkGsiAuth (int sock, char *userName, char *mdasDomain);
extern int
getAuthDnFromMCAT (char *userName, char *domainName, char *authScheme,
char **mcatDnOutput);
extern int
compareGSIDnByMultiKeywd (char *dn1, char *dn2, char *keywd1, char *keywd2);
int
getAuthSchemeFromMCAT (char *userName, char *domainName,
mdasC_sql_result_struct  **outresult);
int 
getUserAuthSchemeCnt (char *userName, char *domainName);
#endif	/* AUTH_EXTERN_H */
