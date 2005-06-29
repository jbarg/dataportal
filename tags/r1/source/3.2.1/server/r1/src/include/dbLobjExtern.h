/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/*-------------------------------------------------------------------------
 *
 * dbLobj.h - Header file for dbLobj.c
 *
 *
 *-------------------------------------------------------------------------
 */
 
#ifndef DB_LOBJ_EXTERN_H
#define DB_LOBJ_EXTERN_H

#include "distribExtern.h"

/* which userName should be used to open the DB Lobj */

#define SRB_SERVER_USER		1	/* the srb server user */
#define SRB_CLIENT_USER		0	/* the client user */

#define DEF_DBLOBJ_TABLENAME	"/srbVault"  /* the default table name for
					     * DB Lobj 
					     */
#define DEF_LOBJ_ID_COL_NAME    "obj_id"    /* The default column name of the
					     * LOBJ ID specified by the client.
					     */
#define DEF_LOBJ_COL_NAME       "obj_locator" /* The dafault column name of the
					       * of the LOBJ handle generated
					       * by the DB.
					       */


typedef struct {
    struct hostElement  *hostTabPtr;    /* Pointer to the host table */
    int         remoteFlag;     /* host location, 0 - local, 1 - remote */
    int         dbSystemType;      /* access method */
    int         fd;                /* the file descriptor or lob locator*/
    char        *dbSpecificInfo;   /* ptr to DB specific state information */
} dbLobjDesc;

/*
 * prototypes for functions in dbLobj.c.
 */
 
extern void initDbLObjDesc();
extern int dbLobjOpen(int dbSystemType, struct varlena *resourceLoc, 
struct varlena *dataPath, int openFlag, int mode);
extern int _dbLobjOpen(int dbSystemType, char *resourceLoc, char *dataPath,
int openFlag, int mode, int dbUserFlag);
extern int dbLobjClose (int descInx);
extern struct varlena *dbLobjRead(int fd, int len);
extern int _dbLobjRead (int descInx, char *buf, int len);
extern int dbLobjSeek (int descInx, long offset, int whence);
extern int dbLobjCreate(int dbSystemType, struct varlena *resourceLoc, 
struct varlena *dataPath, int mode);
extern int _dbLobjCreate(int dbSystemType, char *resourceLoc, char *dataPath,
int mode, int dbUserFlag);
extern int
dbLobjUnlink(int dbSystemType, struct varlena  *resourceLoc,
struct varlena *dataPath);
extern int
_dbLobjUnlink(int dbSystemType, char *resourceLoc, char *dataPath,
int dbUserFlag);
extern int dbLobjWrite (int descInx, struct varlena *buf);
extern int _dbLobjWrite (int descInx, char *buf, int len);
extern char *dbLobjPath2FullPath (char *dataPath);


#endif	/* DB_LOBJ_EXTERN_H */

