/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* srbMiscExtern.h - Definition and prototypes for misc.c
 */

#ifndef SRB_MISC_EXTERN_H
#define SRB_MISC_EXTERN_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "portal.h"
#include "commExtern.h"

#define STRING_LEN1 512		/* Used by the dbLobj drivers */

#ifdef FED_MCAT
/* definition for privUserFlag */
#define LOCAL_PRIV_USER         1       /* A local priv user */
#define FOREIGN_PRIV_USER       2       /* A foreign priv user */
#endif

/* The following maps the MDAS resourceType to storSysType -
 * (UNIX, UTREE, HPSS, FTP or HTTP) or DB LOBJ - (DB2, Illustra, ...).
 */

#define NUM_ENV_ENTRY   10

/*
 A key used to scramble passwords in MCAT; an arbitrary, but fixed, value
 */
#define OBF2_EXTRA_KEY 78

/* Object type definition */

#define FILE_TYPE       1       /* File type */
#define DB_LOBJ_TYPE    2       /* DB LOBJ */
#define DB_TABLE_TYPE   3       /* DB table */
#define COMPOUND_TYPE   4       /* compound */
#define TAPE_TYPE   	5       /* tape  type */
#define MDRIVER_TYPE    6       /* DMisc Driver */

#define NO_CHKDIR       0       /* Don't call chkdir (). */
#define DO_CHKDIR       1       /* call chdir () before proceeding */
#define RD_CHK          2
#define WRT_CHK         3
#define CHK_FILE_PATH   4	/* Only check the file path */
#define CHK_VAULT_PATH  5	/* Check whether the path is in a vault path */
#define CHK_AND_MK_VAULT  6	/* This is for grifFTP only. Check whether 
				 * the path is in a vault path */

/* Returned value of CHK_AND_MK_VAULT. set the global value of VaultpathStatus
 * to : */

#define PATH_NOT_IN_VAULT 	0
#define PATH_IN_VAULT 		1

#define COMPOUND_RESC	"compound"
#define TAPE_RESC	"tape"

struct srbObjTypeMap    {
    char* resourceType;         /* MDAS resource type */
    int objType;                /* FILE_TYPE or DB_LOBJ_TYPE */
    int systemType;             /*  For FILE_TYPE, it means the file system
                                 * types:
                                 *  Unix, UniTree,HPSS, or FTP. For
                                 * DB_LOBJ_TYPE, it is the database
                                 * types: DB2, Oracle
                                 */
    int chdirFlag;		/* For FILE_TYPE only. When creating a 
				 * file, should the permission of the
				 * directory where the file is created
				 * be checked ? NO_CHKDIR ==> no, 
				 * DO_CHKDIR ==> yes
				 */
};

/* resource class definition */

#define GENERIC_RES_CL          "generic"
#define ARCHIVAL_RES_CL         "archival"
#define PERMANENT_RES_CL        "permanent"
#define CACHE_RES_CL            "cache"
#define VOLATILE_RES_CL         "volatile"
#define TEMPORARY_RES_CL        "temporary"
#define PRI_ARCHIVAL_RES_CL     "primary archival"
#define PRI_CACHE_RES_CL        "primary cache"
#define PRI_PERMANENT_RES_CL    "primary permanent"

typedef struct {
    char *keyWard;      /* the keyword */
    int permanent;      /* permanent flag. 0 = no, 1= yes */
    int primary;        /* primary flag. 0 = no, 1= yes */
} resClass;

extern int getUserId(char *userName);
extern char *getSrbDataDir();
extern int findServerExec(char *backend, char *argv0);
extern int validateExec(char *path);
extern char *splitString(char *st1, char *st2, char *st3, char key);
extern int getCondStrLen (char *st1);
extern int checkString(char *st1, char key);
extern char *varToStr (struct varlena *varBuf);
extern int objTypeLookUp (char *resourceType);
extern int enqueVault (int storSysType, char *vaultPath);
extern int pathExist (char *path);
extern char *
mySplitStr(char *st1, char *st2, char *st3, char key);
extern int
mySplitPath (char *path, char *collection, char *dataName);
extern void
portalManager (struct portalInput *myInput);
extern int
chkSrbUser (char *namebuf);
extern int initSrbUserList ();
/* deplicated
extern int initProxyUserInfo (char* errorMsg, int authSchemeInx);
*/

extern void
srbDaemonize(char *logfile);
/*
extern struct varlena *
genIntRet (int status);
*/

#ifdef _WIN32
int ntSetSrbDataDir(char *inDataDir);
void ntSetSrbDefaultPort();
#endif

char *
getLocalMcatName ();
int
initThisUserInfo ();
int
make_quote_to_two_quote(char *new_obj_path_name,char *my_new_obj_path_name);
int
isLocalPrivUser();
int
trimsides(char *str);
int
lower (char *str);
int upper (char *str);
int getPortNum (int lsock);
int
chkHostBasedAuth(struct sockaddr_in *addr, StartupInfo *sp);
extern void obf2Encode(char *in, char *out, int max_out, int extra);
extern int obf2Decode(char *in, char *out, int extra);
extern int web_auth2(char *localZoneName, char * user_name, char * user_password, char * domain_name);
#endif	/* SRB_MISC_EXTERN_H */

