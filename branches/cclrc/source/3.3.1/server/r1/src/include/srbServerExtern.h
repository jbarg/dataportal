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

/* The state of server initialization - used by the exitSrbServer call */
#define INIT_IN_PROGRESS        0
#define INIT_DONE               1

struct allowedUser {
    char *userName;
    char *domainName;
    struct allowedUser *next;
};

extern int srbServerMain(int argc, char *argv[]);
extern void die();
extern void exitSrbServer(int status);
extern void initUserid();
int setAllowedUser (struct allowedUser **allowedUserHead);
int initSrbServer();
int procStartupMsg (Port *myPort, StartupInfo *sp);



#endif	/* SRB_SERVER_EXTERN_H */
