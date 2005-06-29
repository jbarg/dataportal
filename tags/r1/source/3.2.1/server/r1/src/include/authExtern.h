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
#endif	/* AUTH_EXTERN_H */
