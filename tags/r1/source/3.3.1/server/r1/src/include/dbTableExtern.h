/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/*-------------------------------------------------------------------------
 *
 * dbTable.h - Header file for dbTable.c
 *
 *
 *-------------------------------------------------------------------------
 */
 
#ifndef DB_TABLE_EXTERN_H
#define DB_TABLE_EXTERN_H

#include "distribExtern.h"

/* which userName should be used to open the DB Table */

#define SRB_SERVER_USER		1	/* the srb server user */
#define SRB_CLIENT_USER		0	/* the client user */

#define DEF_DBTABLE_TABLENAME	"/srbVault"  /* the default table name for
					     * DB Table 
					     */
#define DEF_TABLE_ID_COL_NAME    "obj_id"    /* The default column name of the
					     * TABLE ID specified by the client.
					     */
#define DEF_TABLE_COL_NAME       "obj_locator" /* The dafault column name of the
					       * of the TABLE handle generated
					       * by the DB.
					       */


typedef struct {
    struct hostElement  *hostTabPtr;    /* Pointer to the host table */
    int         remoteFlag;     /* host location, 0 - local, 1 - remote */
    int         dbSystemType;      /* access method */
    int         fd;                /* the file descriptor or lob locator*/
    char        *dbSpecificInfo;   /* ptr to DB specific state information */
} dbTableDesc;

/*
 * prototypes for functions in dbTable.c.
 */
 
extern void initDbTableDesc();
extern int dbTableOpen(int dbSystemType, struct varlena *resourceLoc, 
struct varlena *dataPath, int openFlag, int mode);
extern int _dbTableOpen(int dbSystemType, char *resourceLoc, char *dataPath,
int openFlag, int mode, int dbUserFlag);
extern int dbTableClose (int descInx);
extern struct varlena *dbTableRead(int fd, int len);
extern int _dbTableRead (int descInx, char *buf, int len);
extern int dbTableSeek (int descInx, long offset, int whence);
extern int dbTableCreate(int dbSystemType, struct varlena *resourceLoc, 
struct varlena *dataPath, int mode);
extern int _dbTableCreate(int dbSystemType, char *resourceLoc, char *dataPath,
int mode, int dbUserFlag);
extern int
dbTableUnlink(int dbSystemType, struct varlena  *resourceLoc,
struct varlena *dataPath);
extern int
_dbTableUnlink(int dbSystemType, char *resourceLoc, char *dataPath,
int dbUserFlag);
extern int dbTableWrite (int descInx, struct varlena *buf);
extern int _dbTableWrite (int descInx, char *buf, int len);
extern char *dbTablePath2FullPath (char *dataPath);


#endif	/* DB_TABLE_EXTERN_H */

