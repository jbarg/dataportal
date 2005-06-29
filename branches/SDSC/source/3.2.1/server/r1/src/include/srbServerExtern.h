/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* srbServerExtern.h - Definitions and prototypes for srbServer.c
 */
#ifndef SRB_SERVER_EXTERN_H
#define SRB_SERVER_EXTERN_H

#include "vault.h"

#define MAINTENENCE_CONFIG_FILE	"srb.allow"

struct allowedUser {
    char *userName;
    char *domainName;
    struct allowedUser *next;
};

extern int srbServerMain(int argc, char *argv[]);
extern void die();
extern void exitSrbServer(int status);
extern void initUserid();


#endif	/* SRB_SERVER_EXTERN_H */
