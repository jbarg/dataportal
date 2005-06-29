/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* clStubExtern.h
 *
 */

#ifndef CL_STUB_EXTERN_H
#define CL_STUB_EXTERN_H

#ifndef _WIN32
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include "srbStat.h"
#include "srbDirent.h"
#include "mdasC_db2_externs.h"
#include "srbC_mdas_externs.h"
#include "functionHandlerExtern.h"
#include "clChksumExtern.h"
#include "packStruct.h"

/* Definition for Audit trail setting */

#define AUDIT_TRAIL_OFF                0       /* turn off auditing */

#define AUDIT_TRAIL_ON                 1       /* turn on auditing */

#define GET_AUDIT_TRAIL_SETTING        2   /* return the audit_trail setting */

#define BL_BUFSIZE         1024*1024*8  /* bulk load buf size */

/* status for srbObjGetC and srbdagetGetC */
#define NEW_PORTLIST 888888

/* The flag definition for Scommand */

#define C_FLAG          0x1             /* collection only or ls acl for Sls */
#define d_FLAG          0x2             /* dataName only */
#define g_FLAG          0x2             /* export fileGroup */
#define P_FLAG          0x4             /* prompt before changing */
#define R_FLAG          0x8             /* recursive */
#define l_FLAG          0x10            /* The print long flag */
#define f_FLAG          0x20            /* The force flag */
#define F_FLAG          0x20            /* The force flag */
#define a_FLAG          0x40            /* The all replica flag or admin */
#define c_FLAG          0x80            /* The container flag */
#define v_FLAG          0x100           /* The verbose flag for Sput and Sget */
#define m_FLAG          0x200           /* multi-thread Sput and Sget */
#define s_FLAG          0x400           /* single-thread Sput and Sget */
#define retry_FLAG      0x800           /* persistant (retry on error) Sput and
Sget */
#define M_FLAG  	0x1000           /* client initiated Sget/Sput */
#define t_FLAG         0x2000           /* Sbunload tar-like output */
#define V_FLAG         0x4000           /* The verbose flag for Sput and Sget pl
us progress bar */
#define z_FLAG         0x8000           /* The MCAT zone flag */
#define b_FLAG         0x10000           /* bulk load/unload flag */
#define e_FLAG         0x20000           /* exit on error flag */
#define A_FLAG         0x40000           /* all replica if a is used for 
					  * admin */
#define B_FLAG         0x80000           /* Backup */
#define k_FLAG         0x100000           /* register ckecksum */
#define K_FLAG         0x200000           /* register and verify ckecksum */
#define y_FLAG         0x400000           /* for util internal use. used to
					   * list all data under a collection
					   * including those in subcollection */
#define Y_FLAG         0x800000           /* for util internal use. used to
					   * query chksum value */
#define N_FLAG         0x1000000          /* number of threads for Sput/Sget */
#define CL_INIT_CONN_FLAG 0x20000000	/* client init connection */
#define u_FLAG         0x40000000       /* For the rmColl call. If this
                                         * Flag is on, will unregister instead
                                         * of unlink
                                         */
#define S_FLAG         0x80000000       /* For Srync - don't use chksum value.
					 * use size instead */
#define unreg_FLAG      u_FLAG

#ifdef PORTNAME_linux
#define BUF_SIZE        (1 * 1024 * 1024)   /* 1 Mb */
#else
#if defined(PORTNAME_osx)
#define BUF_SIZE        (256 * 1024)        /* 0.25 Mb */
#else
#define BUF_SIZE        (2 * 1024 *1024)    /* 2 Mb */
#endif
#endif

/* XXXX - end of section */

struct clHostElement {
    char *hostName; 
    char *portNum;
    int mdasEnabled;            /* Indicate if the host is mdasEnabled.
				 * 0 - no, 1 yes */
    struct clHostElement *next;
};

/* special value for freeSpace */
#define RES_NO_SPACE_DATA        0
#define RES_RESOURCE_RESTART    -1
#define RES_NO_SPACE            -2
#define RES_RESOURCE_DOWN       -3
#define RES_RESOURCE_DELETED    -4

/* XXXXXX - The following are prototypes of Low Level Extended File API.
 *
 * XXXXXX
 */

/*
 * srbFileOpen - Opens an extended file
 *
 * Input - srbConn* conn - From clConnect ().
 *         int storSysType - Storage system type. 
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP, 4 = HTTP.
 *         char *hostAddr - The Host address of the SRB.
 *         char *filename - The file Path name to open. Must be an 
 *	    absolute path.
 *         int flags - same definition as in unix.
 *         int mode - same definition as in unix.
 * Output - Returns the file descriptor for use in later srbFile* calls.
 *          Returns a negative value upon failure.
 */

#ifdef c_plus_plus
extern "C" {
#endif

extern int srbFileOpen(srbConn* conn, int storSysType, char *hostAddr, 
char *filename, int flags, int mode);

/*
 * srbFileCreate Create an extended file.
 * Input - srbConn* conn - From clConnect ().
 *         int storSysType - Storage system type. 
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP (not supported for
 *              HTTP).
 *         char *hostAddr - The Host address of the SRB.
 *         char *filename - The file Path name to create. Must be an
 *          absolute path.
 *         int mode - same definition as in unix.
 *         int size - File size. Only valid for HPSS. -1 => don't know and
 *         The default COS will be used.
 * Output - Returns the file descriptor for use in later srbFile* calls.
 *          Returns a negative value upon failure.
 */

extern int srbFileCreate(srbConn* conn, int storSysType, char *hostAddr, 
char *filename, int mode, srb_long_t size);

/*
 * srbFileUnlink - Unlink an extended file
 * Input - srbConn* conn - From clConnect ().
 *         int storSysType - Storage system type. 
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP (not supported for
 *              HTTP).
 *         char *hostAddr - The Host address of the SRB.
 *         char *filename - The file Path name to unlink. Must be an
 *          absolute path.
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */

extern int srbFileUnlink(srbConn* conn, int storSysType, char *hostAddr, 
char *filename);

/*
 * srbFileClose - close an extended file
 *
 * Input - srbConn* conn - From clConnect ().
 *         int fd - The file descriptor to close (from srbFileOpen or 
 *	   srbFileCreate).
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */

extern int srbFileClose(srbConn* conn, int fd);

/*
 * srbFileRead - Read len bytes of the extended file into buf
 *
 * Input - srbConn* conn - From clConnect ().
 *         int fd - The file descriptor to read (from srbFileOpen or 
 *	   srbFileCreate).
 *         char *buf - The input buffer.
 *         int len - The length to read.
 *
 * Output - Returns the number of bytes read..
 *          Returns a negative value upon failure.
 *
 * The CALLER must have allocated enough space to hold the result returned
 */

extern int srbFileRead(srbConn *conn, int fd, char *buf, int len);

/*
 * srbFileWrite - Write len bytes of buf into the extended file fd
 *
 * Input - srbConn* conn - From clConnect ().
 *         int fd - The file descriptor to write (from srbFileOpen or 
 *	   srbFileCreate).
 *         char *buf - The output buffer.
 *         int len - The length to write.
 *
 * Output - Returns the number of bytes written..
 *          Returns a negative value upon failure.
 *
 */

extern int srbFileWrite(srbConn *conn, int fd, char *buf, int len);

/*
 * srbFileSeek - Change the current read or write location on an extended file
 *
 * Input - srbConn* conn - From clConnect ().
 *         int fd - The file descriptor to seek (from srbFileOpen or 
 *	   srbFileCreate).
 *         int offset - The position of the next operation
 *         int whence - Same definition as in Unix.
 *              SEEK_SET - pointer is set to the value of the Offset parameter.
 *              SEEK_CUR - pointer is set to its current location plus the
 *                      value of the Offset parameter.
 *              SEEK_END - pointer is set to the size of the file plus the
 *                      value of the Offset parameter.
 *
 * Output - Returns the resulting pointer location, measured in bytes from 
 *		the beginning of the file upon success.
 *          Returns a negative value upon failure.
 *
 */

extern srb_long_t srbFileSeek(srbConn *conn, int fd, srb_long_t offset, 
int whence);

/*
 * srbFileSync - Sync an extended file
 *
 * Input - srbConn* conn - From clConnect ().
 *         int fd - The file descriptor to sync (from srbFileOpen or 
 *	   srbFileCreate).
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 *
 */

extern int srbFileSync(srbConn* conn, int fd);

/*
 * srbFileStat - Stat an extended file
 *
 * Input - srbConn* conn - From clConnect ().
 *         int storSysType - Storage system type. 
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP (not supported for
 *              HTTP).
 *         char *hostAddr - The Host address of the SRB.
 *         char *filename - The file Path name to stat. Must be an
 *          absolute path.
 *         struct srbStat *statbuf - The place to put the stat result.
 *
 * Output - stat result in statbuf.
 *          Returns 0 upon success.
 *          Returns a negative value upon failure.
 */

extern int srbFileStat(srbConn* conn, int storSysType, char *hostAddr, 
char *filename, struct srbStat *statbuf);

/*
 * srbFileFstat - Fstat an extended file
 *
 * Input - srbConn* conn - From clConnect ().
 *         int storSysType - Storage system type.
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP (not supported for
 *              HTTP).
 *         char *hostAddr - The Host address of the SRB.
 *         int fd_inx - The srb file descriptor fstat.
 *         stat *statbuf - The place to put the stat result.
 *
 * Output - stat result in statbuf.
 *          Returns 0 upon success.
 *          Returns a negative value upon failure.
 */
extern int
srbFileFstat(srbConn* conn, int storSysType, char *hostAddr, int fd_inx,
struct srbStat *statbuf);

/*
 * srbFileChksum - checksum an extended file
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int fd_inx - The exf file descriptor fstat.
 *         int flags - not used
 *         unsigned long *ichksum - The output chksum.
 *         srb_long_t myOffset - the offset and size to checksum
 *         srb_long_t mySize - for whole file chksum, myOffset=mySize=0
 *
 * Output - stat result in statbuf.
 *          Returns 0 upon success.
 *          Returns a negative value upon failure.
 */
extern int
srbFileChksum (srbConn* conn, int fd_inx,
int flags, unsigned long *ichksum, srb_long_t myOffset, srb_long_t mySize);

extern int ntohStat (struct srbStat *statbuf);

/*
 * srbFileMkdir - create a new directory
 *
 * Input - srbConn* conn - From clConnect ().
 *         int storSysType - Storage system type. 
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP (not supported for
 *              HTTP).
 *         char *hostAddr - The Host address of the SRB.
 *         char *filename - The Path name of the new directory. Must be an
 *          absolute path.
 *         int mode - Same definition as in Unix.
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */

extern int srbFileMkdir(srbConn* conn, int storSysType, char *hostAddr, 
char *filename, int mode);

/*
 * srbFileChmod - Change the mode of a file or directory.
 *
 * Input - srbConn* conn - From clConnect ().
 *         int storSysType - Storage system type. 
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP (not supported for
 *              HTTP).
 *         char *hostAddr - The Host address of the SRB.
 *         char *filename - The Path name of the file or directory.
 *	     Must be an absolute path.
 *         int mode - Same definition as in Unix.
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 *
 */

extern int srbFileChmod(srbConn* conn, int storSysType, char *hostAddr, 
char *filename, int mode);

/*
 * srbFileRmdir - Remove a directory.
 *
 * Input - srbConn* conn - From clConnect ().
 *         int storSysType - Storage system type. 
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP (not supported for
 *              HTTP).
 *         char *hostAddr - The Host address of the SRB.
 *         char *filename - The Path name of the directory to remove.
 *	    Must be an absolute path.
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */

extern int srbFileRmdir(srbConn* conn, int storSysType, char *hostAddr, 
char *filename);

/*
 * srbSetStorAttri - Set Attributes for a Storage System; establish connection.
 *   Currently only defined for FTP files, connects to an FTP Server and/or
 *   sets the current directory.
 *
 * Input - srbConn* conn - From clConnect ().
 *         int storSysType - Storage system type. 
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP (not supported for
 *              HTTP).
 *              Currently only defined for FTP.
 *         char *hostAddr -  The FTP Server address followed by
 *              optional ":" and port, eg "ftp.sdsc.edu:21"
 *         char *userAuth - The User id and optional password, 
 *              eg "anonymous:schroede@sdsc.edu"
 *         char *dirPath - directory to change to (cd)
 *              (dirPath or vaultAddr may be null to leave unchanged,
 *               if both are null the SRB will disconnect from the
 *               ftp server (gracefully logout))
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 *
 */

extern int srbSetStorAttri(srbConn* conn, int storSysType, 
char *hostAddr, char *userAuth, char *dirPath);

/*
 * srbOpendir - Opens a directory
 *
 * Input - srbConn* conn - From clConnect ().
 *         int storSysType - Storage system type.
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP.
 *         char *hostAddr - The Host address of the SRB.
 *         char *dirname - The dir Path name to open. Must be an
 *          absolute path.
 * Output - Returns the file descriptor for use in later srb* calls.
 *          Returns a negative value upon failure.
 */

extern int srbOpendir(srbConn* conn, int storSysType,
char *hostAddr, char *dirname);

/*
 * srbClosedir - close an opened directory
 *
 * Input - srbConn* conn - From clConnect ().
 *         int fd - The file descriptor to close (from srbOpendir)
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */

extern int srbClosedir(srbConn* conn, int fd);

/*
 * srbReaddir - Stat a directory entry
 *
 * Input - srbConn* conn - From clConnect ().
 *         int fd - The file descriptor to read (from srbOpendir)
 *
 * Output - struct srbDirent *srbDirent - The dirent read.
 */

extern struct srbDirent *srbReaddir(srbConn* conn, int dirDesc);

/*
 * srbFileMigrate - Migrate an extended file
 *
 * Input - srbConn* conn - From clConnect ().
 *         int fd - The file descriptor to migrate (from srbFileOpen or
 *         srbFileCreate).
 *         int srcLevel - The storage hierarchy from which the data is to be
 *         migrated.
 *         int flags - BFS_MIGRATE_ALL - migrate the entire file.
 *        
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */

extern int
srbFileMigrate(srbConn* conn, int fd, int srcLevel, int flags);

/*
 * srbFileStage - Stage an extended file
 *
 * Input - srbConn* conn - From clConnect ().
 *         int fd - The file descriptor to stage (from srbFileOpen or
 *         srbFileCreate).
 *         int offset - The offset of the startof the data to stage.
 *         int len - The length to stage.
 *         int storLevel - The storage hierarchy to which the data is to be
 *         staged.
 *         int flags - BFS_STAGE_ALL - stage the entire file.
                       BFS_ASYNC_CALL - return after initial stage.
 *
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */

extern int
srbFileStage(srbConn* conn, int fd, int offset, int len,
 int storLevel, int flags);

/*
 * srbFilePurge - Purge an extended file
 *
 * Input - srbConn* conn - From clConnect ().
 *         int fd - The file descriptor to purge.g (from srbFileOpen or
 *         srbFileCreate).
 *         int offset - The offset of the startof the data to purge.g.
 *         int len - The length to purge.g.
 *         int storLevel - The storage hierarchy from which the data is to be
 *         purged.
 *         int flags - BFS_PURGE_ALL - purge.g the entire file.
 *
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */

extern int
srbFilePurge(srbConn* conn, int fd, int offset, int len,
 int storLevel, int flags);

/* XXXXXX - The following are prototypes of Lower level DB Large Object API.
 *
 * XXXXXX
 */

/*
 * srbDbLobjOpen - Opens a DB Large Object.
 *
 * Input - srbConn* conn - From clConnect ().
 *         int storSysType - Storage system type.
 *              0 = DB2, 1 = Illustra.
 *         char *resourceLoc - format : host:database:instance.
 *         char *dataPath - Format : /tablename/objectID. If only the objectID
 *           is given , the tablename will default to "/srbVault".
 *         int flags - same definition as in unix.
 *         int mode - Not curently used.
 * Output - Returns the file descriptor for use in later srbDbLobj* calls.
 *          Returns a negative value upon failure.
 */

extern int srbDbLobjOpen(srbConn* conn, int storSysType,
char *resourceLoc, char *dataPath, int flags, int mode);

/*
 * srbDbLobjCreate Create a DB Large Object.
 * Input - srbConn* conn - From clConnect ().
 *         int storSysType - Storage system type.
 *              0 = DB2, 1 = Illustra.
 *         char *resourceLoc - format : host:database:instance.
 *         char *dataPath - Format : /tablename/objectID. If only the objectID
 *           is given , the tablename will default to "/srbVault".
 *         int mode - Not curently used.
 * Output - Returns the file descriptor for use in later srbDbLobj* calls.
 *          Returns a negative value upon failure.
 */

extern int srbDbLobjCreate(srbConn* conn, int storSysType,
char *hostAddr, char *filename, int mode);

/*
 * srbDbLobjClose - close a DB Large Object.
 *
 * Input - srbConn* conn - From clConnect ().
 *         int fd - The dbLobj descriptor to close (from srbDbLobjClose or
 *         srbDbLobjCreate).
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */

extern int srbDbLobjClose(srbConn* conn, int fd);

/*
 * srbDbLobjRead - Read len bytes of the DB large object into buf
 *
 * Input - srbConn* conn - From clConnect ().
 *         int fd - The object descriptor (from the srbDbLobjOpen call) to read.
 *         char *buf - The input buffer.
 *         int len - The number of bytes to read.
 *
 * Output - Returns the length of bytes read.
 *          Returns a negative value upon failure.
 * the CALLER must have allocated enough space to hold the result returned
 */

extern int srbDbLobjRead(srbConn *conn, int fd, char *buf, int len);

/*
 * srbDbLobjWrite - Write len bytes of buf into the dbLobj fd
 *
 * Input - srbConn* conn - From clConnect ().
 *         int fd - The dbLobj descriptor to write (from srbDbLobjOpen or
 *         srbDbLobjCreate).
 *         char *buf - The output buffer.
 *         int len - The length to write.
 *
 * Output - Returns the number of bytes written..
 *          Returns a negative value upon failure.
 *
 */

extern int srbDbLobjWrite(srbConn *conn, int fd, char *buf, int len);

/*
 * srbDbLobjSeek - Change the current read or write location on of dbLobj
 *
 * Input - srbConn* conn - From clConnect ().
 *         int fd - The dbLobj descriptor (from the srbDbLobjOpen call) to seek.
 *         int offset - The position of the next operation
 *         int whence - Same definition as in Unix.
 *              SEEK_SET - pointer is set to the value of the Offset parameter.
 *              SEEK_CUR - pointer is set to its current location plus the
 *                      value of the Offset parameter.
 *              SEEK_END - pointer is set to the size of the file plus the
 *                      value of the Offset parameter.
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */

extern int srbDbLobjSeek(srbConn *conn, int fd, int offset, int whence);

/*
 * srbDbLobjUnlink - Unlink a DB large object.
 *
 * Input - srbConn* conn - From clConnect ().
 *         int storSysType - Storage system type.
 *              0 = DB2, 1 = Illustra.
 *         char *resourceLoc - format : host:database:instance.
 *         char *dataPath - Format : tablename/objectID
 * Output - Returns 0 - success.
 *          Returns a negative value upon failure.
 */

extern int
srbDbLobjUnlink(srbConn* conn, int storSysType, char *resourceLoc,
char *dataPath);

/* XXXXXX - The following are prototypes of Lower level DB Table Object API.
 *
 * XXXXXX
 */



/*
 * srbDbTableOpen - Opens a DB Table Object.
 *
 * Input - srbConn* conn - From clConnect ().
 *         int storSysType - Storage system type.
 *              0 = DB2, 1 = Illustra.
 *         char *resourceLoc - format : host:database:instance.
 *         char *dataPath - Format : /tablename/objectID. If only the objectID
 *           is given , the tablename will default to "/srbVault".
 *         int flags - same definition as in unix.
 *         int mode - Not curently used.
 * Output - Returns the file descriptor for use in later srbDbTable* calls.
 *          Returns a negative value upon failure.
 */

extern int srbDbTableOpen(srbConn* conn, int storSysType,
char *resourceLoc, char *dataPath, int flags, int mode);

/*
 * srbDbTableCreate Create a DB Table Object.
 * Input - srbConn* conn - From clConnect ().
 *         int storSysType - Storage system type.
 *              0 = DB2, 1 = Illustra.
 *         char *resourceLoc - format : host:database:instance.
 *         char *dataPath - Format : /tablename/objectID. If only the objectID
 *           is given , the tablename will default to "/srbVault".
 *         int mode - Not curently used.
 * Output - Returns the file descriptor for use in later srbDbTable* calls.
 *          Returns a negative value upon failure.
 */

extern int srbDbTableCreate(srbConn* conn, int storSysType,
char *hostAddr, char *filename, int mode);

/*
 * srbDbTableClose - close a DB Table Object.
 *
 * Input - srbConn* conn - From clConnect ().
 *         int fd - The dbTable descriptor to close (from srbDbTableClose or
 *         srbDbTableCreate).
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */

extern int srbDbTableClose(srbConn* conn, int fd);

/*
 * srbDbTableRead - Read len bytes of the DB table object into buf
 *
 * Input - srbConn* conn - From clConnect ().
 *         int fd - The object descriptor (from the srbDbTableOpen call) to read.
 *         char *buf - The input buffer.
 *         int len - The number of bytes to read.
 *
 * Output - Returns the length of bytes read.
 *          Returns a negative value upon failure.
 * the CALLER must have allocated enough space to hold the result returned
 */

extern int srbDbTableRead(srbConn *conn, int fd, char *buf, int len);

/*
 * srbDbTableWrite - Write len bytes of buf into the dbTable fd
 *
 * Input - srbConn* conn - From clConnect ().
 *         int fd - The dbTable descriptor to write (from srbDbTableOpen or
 *         srbDbTableCreate).
 *         char *buf - The output buffer.
 *         int len - The length to write.
 *
 * Output - Returns the number of bytes written..
 *          Returns a negative value upon failure.
 *
 */

extern int srbDbTableWrite(srbConn *conn, int fd, char *buf, int len);

/*
 * srbDbTableSeek - Change the current read or write location on of dbTable
 *
 * Input - srbConn* conn - From clConnect ().
 *         int fd - The dbTable descriptor (from the srbDbTableOpen call) to seek.
 *         int offset - The position of the next operation
 *         int whence - Same definition as in Unix.
 *              SEEK_SET - pointer is set to the value of the Offset parameter.
 *              SEEK_CUR - pointer is set to its current location plus the
 *                      value of the Offset parameter.
 *              SEEK_END - pointer is set to the size of the file plus the
 *                      value of the Offset parameter.
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */

extern int srbDbTableSeek(srbConn *conn, int fd, int offset, int whence);

/*
 * srbDbTableUnlink - Unlink a DB table object.
 *
 * Input - srbConn* conn - From clConnect ().
 *         int storSysType - Storage system type.
 *              0 = DB2, 1 = Illustra.
 *         char *resourceLoc - format : host:database:instance.
 *         char *dataPath - Format : tablename/objectID
 * Output - Returns 0 - success.
 *          Returns a negative value upon failure.
 */

extern int
srbDbTableUnlink(srbConn* conn, int storSysType, char *resourceLoc,
char *dataPath);




/* XXXXXX - The following are prototypes of High Level Object API.
 *
 * XXXXXX
 */

/*
 * srbObjOpen - Open a SRB object
 *
 * Input - srbConn* conn - From clConnect ().
 *         char *objID - The SRB object ID to open. The objID is must have 
 *		been registered with MMCAT. One or more conditions
 *              can be appended to the objID. Each condition must be  
 *              preceded by the character '&'. Currently, only one condition
 *              is supported. i.e., COPY=n (where n = replica number beginning
 *              with 0). e.g. foo&COPY=1 specifies opening replica number
 *              1 of data object "foo".
 *         int oflag - Unix type open flag. O_CREAT is not supported.
 *         char *collectionName - The name of the collection this objID
 *              belongs.
 *
 * Output - Returns the object descriptor.
 *          Returns a negative value upon failure.
 */

extern int srbObjOpen(srbConn* conn, char *objID, int oflag,
char *collectionName);

/*
 * srbObjOpenWithTicket - Open a SRB object using a ticket.
 *
 * Input - srbConn* conn - From clConnect ().
 *         char *objID - The SRB object ID to open. The objID is must have 
 *		been registered registered with MMCAT. objID may be NULL. 
 *		In this case, SRB will open the object associated with the 
 *		ticket. One or more conditions can be appended to the objID. 
 *		Each condition must be preceded by the character '&'. 
 *		Currently, only one condition is supported. i.e., COPY=n 
 *		(where n = replica number beginning with 0). 
 *		e.g. foo&COPY=1 specifies opening replica number
 *              1 of data object "foo".
 *         int oflag - Unix type open flag. O_CREAT is not supported.
 *         char *collectionName - The name of the collection this objID
 *              belongs.
 *         char *ticket - The ticket for the object.
 *
 *
 * Output - Returns the object descriptor.
 *          Returns a negative value upon failure.
 */

extern int
srbObjOpenWithTicket (srbConn* conn, char *objID,
int oflag, char *collectionName, char *ticket);

/*
 * srbObjCreate - Create a SRB object
 *
 * Input - srbConn* conn - From clConnect ().
 *	   int catType - catalog type. e,g., MDAS_CATALOG.
 *         char *objID - The SRB object ID to create. The objID is a
 *              user defined name to be registered with MDAS. This 
 *              ID will be used for subsequent reference of the data
 *              object.
 *              One or more conditions can be appended to the objID.
 *              Each condition must be preceded by the character '&'.
 *              Currently, two conditions are supported: 
 *              1) COPIES=MMM where MMM may be:
 *                 a) an integer n which means n replica should be created.
 *                 The "resourceName" input is the logical resource in which 
 *                 this object is to be stored. This logical resource must 
 *                 consist of at least n physical resources. e.g. foo&COPIES=2
 *                 specifies the creation of two replica of data object "foo".
 *                 b) the keyword RR which means a single copy should be 
 *                 created in one of the physical resources belonging to 
 *                 the input logical resource ("resourceName") chosen in a 
 *                 Round-Robin fashion. e.g. foo&COPIES=RR.
 *                 c) the keyword RANDOM produces similar effect as the 
 *                 RR keyword. The only difference is the selection algorithm 
 *                 is random rather than Round-Robin. e.g. foo&COPIES=RANDOM.
 *              2) CONTAINER=containerName. This keyword specifies the
 *              object is to be placed in the given container. The
 *              container must have already been created using the 
 *              srbContainerCreate() call.
 *         char *dataTypeName - Data type. e.g. "generic"
 *         char *resourceName - The storage resource name. This may be a
 *              the name of a single resource or a resource group 
 *              (or logical resource) consisting of two or more physical 
 *              resources.
 *              e.g. "mda18-unix-sdsc"
 *         char *collectionName - The collection name.
 *         char *pathName - The file/DB path of the data.
 *         srb_long_t dataSize - File size. Used by HPSS to determine COS. 
 *         -1 => don't know and the default COS will be used.
 *
 *
 * Output - Returns the object descriptor.
 *          Returns a negative value upon failure.
 */

extern int
srbObjCreate(srbConn* conn, int catType, char *objID,
char *dataTypeName, char *resourceName, char *collectionName, 
char *pathName, srb_long_t dataSize);

/*
 * srbObjClose - Close an opened object
 *
 * Input - srbConn* conn - From clConnect ().
 *         int desc - The object descriptor (from the srbObjOpen call).
 *
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */

extern int srbObjClose (srbConn* conn, int desc);

/*
 * srbObjUnlink - Unlink an SRB object
 *
 * Input - srbConn* conn - From clConnect ().
 *         char *objID - The SRB object ID to unlink. The objID is obtained
 *              through registration with MDAS.
 *         char *collectionName - The name of the collection this objID
 *              belongs.
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 *
 */

extern int srbObjUnlink (srbConn* conn, char *objID,
char *collectionName);

/*
 * srbObjRead - Read len bytes of the SRB object into buf
 *
 * Input - srbConn* conn - From clConnect ().
 *         int desc - The object descriptor (from the srbObjOpen call) to read.
 *         char *buf - The input buffer.
 *         int len - The number of bytes to read.
 *
 * Output - Returns the length of bytes read.
 *          Returns a negative value upon failure.
 * the CALLER must have allocated enough space to hold the result returned
 */

extern int srbObjRead(srbConn *conn, int desc, char *buf, int len);

/*
 * srbObjSeek - Change the current read or write location on an SRB object.
 * Input - srbConn* conn - From clConnect ().
 *         int desc - The object descriptor (from the srbObjOpen call) to seek.
 *         int offset - The position of the next operation
 *         int whence - Same definition as in Unix.
 *              SEEK_SET - pointer is set to the value of the Offset parameter.
 *              SEEK_CUR - pointer is set to its current location plus the
 *                      value of the Offset parameter.
 *              SEEK_END - pointer is set to the size of the file plus the
 *                      value of the Offset parameter.
 *
 * Output - Returns the resulting offset,  as  measured  in  bytes from the
 *  	    beginning of the file.
 *          Returns a negative value upon failure.
 */

extern srb_long_t srbObjSeek(srbConn *conn, int desc, 
srb_long_t offset, int whence);

/*
 * srbObjSync - Sync an opened object (call fsync for UNIX FS)
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int desc - The object descriptor (from the srbObjOpen call).
 *
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */

extern int
srbObjSync (srbConn* conn, int desc);

/*
 * srbObjStat - Stat a srb Path - The result is placed in statbuf which
 * is of type (strust stat) of the local fs.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - The catalog type.
 *         char *path - the SRB path.
 *         stat *statbuf - The place to put the stat result.
 *
 * Output - stat result in statbuf.
 *          Returns 0 upon success.
 *          Returns a negative value upon failure.
 */
extern int
srbObjStat(srbConn* conn, int catType, char *path, struct stat *statbuf);

/*
 * srbObjStat64 - Stat a srb Path - The result is placed in statbuf which
 * is of type (strust stat) of the local fs.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - The catalog type.
 *         char *path - the SRB path.
 *         stat *statbuf - The place to put the stat result.
 *
 * Output - stat result in statbuf.
 *          Returns 0 upon success.
 *          Returns a negative value upon failure.
 */
extern int
srbObjStat64 (srbConn* conn, int catType, char *path, struct stat64 *statbuf);

/*
 * srbObjGetdents - The SRB equivalent of the getdents call.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - The catalog type.
 *         int fd - the opened fd.
 *         dirent_t *buf - The place to put the dirents result.
 *         size_t nbyte - The max number of bytes to output.
 *
 * Output - dirents result in buf.
 *          Returns 0 upon success.
 *          Returns a negative value upon failure.
 */
extern int
srbObjGetdents (srbConn* conn, int catType, int fd, dirent_t *buf,
size_t nbyte);

/*
 * srbObjGetdents64 - The SRB equivalent of the getdents64 call.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - The catalog type.
 *         int fd - the opened fd.
 *         dirent_t *buf - The place to put the dirents result.
 *         size_t nbyte - The max number of bytes to output.
 *
 * Output - dirents result in buf.
 *          Returns 0 upon success.
 *          Returns a negative value upon failure.
 */
extern int
srbObjGetdents64 (srbConn* conn, int catType, int fd, dirent64_t *buf,
size_t nbyte);
extern int
srbCollSeek(srbConn *conn, int desc, int offset, int whence, int is64Flag);

/*
 See src/client/clStub.c for a description
*/
extern int
srbObjLockRange(srbConn* conn, int fd, srb_long_t offset, 
                int size, int mode);

/*
 * srbObjProxyOpr - Proxy Operation request.
 *
 * Input - srbConn* conn - From clConnect ().
 *         ProxyOprId operation -The type of proxy operation. Valid operations:
 *              OPR_COPY - Copy from the object descriptor given in sourceInp
 *              to the object descriptor given in destInp.
 *              If successful, the number of bypes copied is returned. -ive
 *              values means failure.
 *
 *         int inputInt1 - intput integer 1.
 *         int inputInt2 - intput integer 2.
 *         int inputInt3 - intput integer 3.
 *         int inputInt4 - intput integer 4.
 *         char *inputStr1 - Input String 1.
 *         char *inputStr2 - Input String 2.
 *         char *inputStr3 - Input String 3.
 *         char *inputStr4 - Input String 4.
 *         char *inputStr5 - Input String 5.
 *         byteStream *inputBStrm1 - Input Byte stream 1.
 *         byteStream *inputBStrm2 - Input Byte stream 2.
 *
 *
 * Output - Returns 0 or positive value upon success.
 *          Returns a negative value upon failure.
 *          outBuf - any byte stream output.
 */

extern int
srbObjProxyOpr (srbConn *conn, ProxyOprId operation,
int inputInt1, int inputInt2, int inputInt3, int inputInt4,
char *inputStr1, char *inputStr2, char *inputStr3, char *inputStr4,
char *inputStr5, byteStream *inputBStrm1, byteStream *inputBStrm2,
char *outBuf);

/*
 * srbObjReplicate - Make a copy of an SRB object
 *
 * Input - srbConn* conn - From clConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *         char *objID - The SRB object ID to replicate.
 *         char *collectionName - The name of the collection this objID
 *              belongs.
 *         char *newResourceName - The storage resource name of the new copy.
 *              e.g. "mda18-unix-sdsc"
 *         char *newPathName - The file/DB path of the new copy.
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 *
 */

extern int
srbObjReplicate (srbConn* conn, int catType, char *objID, char *collectionName,
char *newResourceName, char *newPathName);

/*
 * srbObjMove - Move a copy of an SRB object to a new location
 *
 * Input - srbConn* conn - From clConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *         char *objID - The SRB object ID to unlink. The objID is obtained
 *              through registration with MDAS.
 *         char *collectionName - The name of the collection this objID
 *              belongs.
 *         char *srcResource - The source resource to move - not used
 *         char *newResourceName - The storage resource name of the new copy.
 *              e.g. "mda18-unix-sdsc"
 *         char *newPathName - The file/DB path of the new copy.
 *         char *container - the container to move into 
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 *
 */

extern int
srbObjMove (srbConn* conn, int catType, char *objID, char *collectionName,
char *srcResource, char *newResourceName, char *newPathName, char *container);

/*
 * srbObjWrite - Write len bytes of buf into the Object fd
 *
 * Input - srbConn* conn - From clConnect ().
 *         int desc - The Object descriptor to write (from svrObjOpen or
 *         svrObjCreate).
 *         char *buf - The output buffer.
 *         int len - The length to write.
 *
 * Output - Returns the number of bytes written..
 *          Returns a negative value upon failure.
 *
 */

extern int srbObjWrite(srbConn *conn, int desc, char *buf, int len);
 
/*
 * srbCreateCollect - Create a SRB collection
 *
 * Input - srbConn* conn - From clConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *         char *parentCollect- The parent collection in which to create
 *              the new collection.
 *         char *newCollect - The name of the collection to create.
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

extern int srbCreateCollect (srbConn* conn, int catType, char *parentCollect,
char *newCollect);

/*
 * srbListCollect - List a SRB collection
 *
 * Input - srbConn* conn - From clConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *         char *collectionName- The collection to list.
 *         char *flag - "C" - non-recursive. "R" - recursive
 *         mdasC_sql_result_struct *listResult - The address points to the
 *         result. A pointer to a user supplied mdasC_sql_result_struct.
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

extern int srbListCollect (srbConn* conn, int catType,
char *collectionName, char *flag, mdasC_sql_result_struct *listResult,
int rowsWanted);

/*
 * srbModifyCollect - Modify a SRB collection
 *
 * Input - srbConn* conn - From clConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *         char *collectionName - The name of the collection this objID
 *              belongs.
 *         char *dataValue1 - Input value 1.
 *         char *dataValue2 - Input value 2.
 *         char *dataValue3 - Input value 3.
 *         int retractionType - The type of retraction. See srbC_mdas_externs.h
 *                              for the retractionType definition.
 *
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

extern int srbModifyCollect (srbConn* conn, int catType,
char *collectionName, char *dataValue1, char *dataValue2,
char *dataValue3, int retractionType);

/*
 * srbIssueTicket - Issue a ticket.
 *
 * Input - srbConn* conn - From clConnect ().
 *         char *objID - The object ID
 *         char *collectionName - The collection name
 *         char *collectionFlag - The collect flag if vCollectionName
 *              is non NULL. "R" - the ticket is for all dataset and
 *              sub-collection recursively. "D" - the ticket is for the
 *              datasets directly beneath the colloection.
 *         char *beginTime - The beginning time when the ticket
 *              becomes effective. A NULL means no time limit.
 *         char *endTime - The ending time of the ticket.
 *         int accessCnt - The number of time the ticket can be used to
 *              access the dataset.
 *         char *ticketUser - The user/userGroup that will use the
 *              ticket. Multiply users can be specified with the following
 *              format:
 *                      user1@domain1&user2@domain2 ....
 *              If it is NULL, => all users.
 *
 *         char **ticket - The address to put the output ticket.
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

extern int
srbIssueTicket (srbConn* conn, char *objID, char *collectionName,
char *collectionFlag, char *beginTime, char *endTime, int accessCnt,
char *ticketUser, char **ticket);

/* srbRemoveTicket - Cancel a ticket.
 *
 *
 * Input : char *ticket - The ticket to remove.
 *
 * Output : Returns 0 - success, negative  - failure
 */

extern int
srbRemoveTicket (srbConn* conn, char *ticket);

/* srbRemoveTicketWithZone - Cancel a ticket.
 *
 *
 * Input : char *ticket - The ticket to remove.
 *
 * Output : Returns 0 - success, negative  - failure
 */

extern int
srbRemoveTicketWithZone (srbConn* conn, char *mcatZone, char *ticket);


/* XXXXXX - The following are prototypes of API that interact with MCAT.
 *
 * XXXXXX
 */

/*
 * srbGetDatasetInfo - Get Info on a SRB data object.
 *
 * Input : srbConn* conn,
 *         int catType - The catalog type - 0 = MDAS_CATALOG
 *         char *objID,
 *         char *collectionName
 *	   mdasC_sql_result_struct *myresult - A pointer to a user supplied 
 *	    mdasC_sql_result_struct.
 *         int rowsWanted - number of rows of result wanted.
 *	   char *mode
 * Output:
 *         A mdasC_sql_result_struct *myresult
 *
 * Return value :
 *      0 - success; myresult->continuation_index >= 0, ==> more results from
 *	    the query. Use srbGetMoreRows() to retrieve more rows. 
 *      a negative value - failure
 */

extern int srbGetDatasetInfo(srbConn* conn, int catType, char *objID,
char *collectionName, mdasC_sql_result_struct *myresult, int rowsWanted,
char *mode);

/*
 * srbGetDataDirInfo - Get metadata info by querying the MDAS catalog.
 * The server uses the input qval[][] and selval[] array to compose
 * and execute SQL queries and returns the query result in myresult.
 * The selval[] array specifies a list of attrbutes to retrieve and
 * qval[][] specifies a lists of "=" predicates to search.
 * Both selval[] and qval[][] must be arrays of size MAX_DCS_NUM and
 * are indexed by values given in mdasC_db2_externs.h under the heading
 * DCS-ATTRIBUTE-INDEX DEFINES.
 *
 * For the selval[] array, setting an element of the array to 1 means that
 * the attribute associated with this element is to be retrieved. e.g.,
 * selval[USER_NAME] = 1; means the "user_name" attribute is to be retrieved.
 *
 * The qval[][] array specifies the "=" predicates to search. e.g.,
 * sprintf(qval[DATA_NAME],"'%s'", "unixFileObj1"); means that the search
 * condition includes the term (data_name = "unixFileObj1").
 *
 * An example of srbGetDataDirInfo:
 *
 *  mdasC_sql_result_struct myresult;
 *  char qval[MAX_DCS_NUM][MAX_TOKEN];
 *  int selval[MAX_DCS_NUM];
 *
 * for (i = 0; i < MAX_DCS_NUM; i++) {
 *      selval[i] = 0;
 *      sprintf(qval[i],"");
 *  }
 *  sprintf(qval[DATA_NAME],"'%s'",argv[2]);
 *  selval[PATH_NAME] = 1;
 *  selval[RSRC_NAME] = 1;
 *      .
 *      .
 *  if (srbGetDataDirInfo(conn, MDAS_CATALOG, qval, selval, &myresult) < 0) {
 *      fprintf(stderr, "can't srbGetDataDirInfo \n");
 *      exit_nicely(conn);
 *  }
 *
 *
 * 
 * Input : srbConn* conn,
 *         int catType - The catalog type - 0 = MDAS_CATALOG
 *         char qval[][MAX_TOKEN] - A point to a user supplied
 *           char qval[MAX_DCS_NUM][MAX_TOKEN] array.
 *         int *selval -  A pointer to a user supplied selval[MAX_DCS_NUM]
 *           array.
 *         int rowsWanted - number of rows of result wanted.
 *
 * Output:
 *         A mdasC_sql_result_struct *myresult
 *
 * Return value :
 *      0 - success; myresult->continuation_index >= 0, ==> more results from
 *          the query. Use srbGetMoreRows() to retrieve more rows.
 *      a negative value - failure
 */

extern int
srbGetDataDirInfo(srbConn* conn, int catType, char qval[][MAX_TOKEN],
int *selval, mdasC_sql_result_struct *myresult, int rowsWanted);

/* This is the same as srbGetDataDirInfo except with an additional
 * char *myMcatZone input */

extern int
srbGetDataDirInfoWithZone(srbConn* conn, int catType, char *myMcatZone,
char qval[][MAX_TOKEN],
int *selval, mdasC_sql_result_struct *myresult, int rowsWanted);

/*
 * srbGenQuery - This is a more compact form of srbGetDataDirInfo.
 * Instead of using fixed array of selval and qval, it uses the
 * genQuery struct.
 *
 *
 * Input : srbConn* conn,
 *         int catType - The catalog type - 0 = MDAS_CATALOG
 *         char *myMcatZone - The MCAT zone for this query. Only valid
 *          for version 3.x.x and above. For version 2.x.x, this should be NULL.
 *          genQuery_t *myGenQuery - The input query in th form of genQuery_t
 *           instead of selval and qval of srbGetDataDirInfo.
 *         mdasC_sql_result_struct *myresult - The output query result.
 *         int rowsWanted - number of rows of result wanted.
 *
 * Output:
 *         A mdasC_sql_result_struct *myresult
 *
 * Return value :
 *      0 - success; myresult->continuation_index >= 0, ==> more results from
 *          the query. Use srbGetMoreRows() to retrieve more rows.
 *      a negative value - failure
 *
 * Note : We cannot use the generic routine clCall() because the input
 * byte stream is not a char string.
 */

extern int
srbGenQuery (srbConn* conn, int catType, char *myMcatZone,
genQuery_t *myGenQuery, mdasC_sql_result_struct *myresult, int rowsWanted);

extern int srbGetDataDirInfo(srbConn* conn, int catType, char qval[][MAX_TOKEN],
int *selval, mdasC_sql_result_struct *myresult, int rowsWanted);

/* strToSqlResult - convert the sql results from SRB to mdasC_sql_result_struct.
 *
 * Input - char *strFromSrb - The byte stream returned from the SRB.
 *         mdasC_sql_result_struct *myresult - The place to put the output.
 *
 * Output - 0 - success
 *          a negative value - Failure
 */

extern int
strToSqlResult (char *strFromSrb, mdasC_sql_result_struct *myresult);

/*
 * srbUnregisterDataset - Unregister an SRB object
 *
 * Input - srbConn* conn - From clConnect ().
 *         char *objID - The SRB object ID to unlink. The objID is obtained
 *              through registration with MDAS.
 *         char *collectionName - The name of the collection this objID
 *              belongs.
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 *
 */

extern int
srbUnregisterDataset (srbConn* conn, char *objID,
char *collectionName);

/*
 * srbSetAuditTrail - Setting and Unsetting Audit Trail.
 *
 * Input - srbConn* conn - From clConnect ().
 *         int set_value - The Audit Trail value to set.
 *              AUDIT_TRAIL_OFF - turn on audit trail.
 *              AUDIT_TRAIL_ON - turn on audit trail.
 *              GET_AUDIT_TRAIL_SETTING - return the cuurent audit trail
 *                setting without modifying the setting.
 *
 * Output - Returns the currently audit trail setting (after processing
 *          the latest change request).
 */

extern int
srbSetAuditTrail (srbConn* conn, int set_value);

/*
 * srbModifyDataset - Modify a SRB dataset
 *
 * Input - srbConn* conn - From clConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *         char *objID - The SRB object ID to modify. The objID must
 *              already have been registered with the MDAS catalog.
 *         char *collectionName - The name of the collection this objID
 *              belongs.
 *         char *resourceName - The storage resource name.
 *              e.g. "mda18-unix-sdsc"
 *         char *pathName - The file/DB path of the data.
 *
 *         char *dataValue1 - Input value 1. See the prototype for the
 *              modify_dataset_info() function in
 *              catalog/include/mdasPrototypes.h for the definition of
 *              dataValue1, dataValue2 and retractionType.
 *
 *         char *dataValue2 - Input value 2.
 *         int retractionType - The type of retraction. See srbC_mdas_externs.h
 *                              for the retractionType definition.
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

extern int srbModifyDataset (srbConn* conn, int catType, char *objID,
char *collectionName, char *resourceName, char *pathName, char *dataValue1,
char *dataValue2, int retractionType);

/*
 * srbChkMdasAuth - Authenticate a userName/passwd.
 *
 * Input - srbConn* conn - From clConnect ().
 *         char *userName   and
 *         char *srbAuth - The userName/passwd pair to authenticate.
 *         char *mdasDomain - The MDAS Domaine.
 *
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

extern int
srbChkMdasAuth (srbConn* conn, char *userName, char *srbAuth, char *mdasDomain);

/*
 * srbChkMdasSysAuth - Authenticate a userName/passwd for sys admin access.
 *
 * Input - srbConn* conn - From clConnect ().
 *         char *userName   and
 *         char *srbAuth - The userName/passwd pair to authenticate.
 *         char *mdasDomain - The MDAS Domain.
 *
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

extern int
srbChkMdasSysAuth (srbConn* conn, char *userName, char *srbAuth,
char *mdasDomain);

/*
 * srbRegisterUserGrp - Register a user group
 *
 * Input - srbConn* conn - From clConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *         char *userGrpName - The name of the user group to register.
 *         char *userGrpPasswd - The user group passwd.
 *         char *userGrpType - The user group type. Currently, at SDSC valid 
 *		userType are:
 *              "staff", "sdsc staff", "sdsc staff scientist", 
 *              "sdsc senior staff scientist", "pto staff", "ucsd staff"
 *              "student", "sdsc student", "uva student", "project",
 *              "umd student", "public", "sysadmin", " deleted"
 *         char* userGrpAddress - The mailing address of the user group.
 *         char* userGrpPhone - The phone number of the user group.
 *         char* userGrpEmail - The Email address of the user group.
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

extern int
srbRegisterUserGrp (srbConn* conn, int catType, char *userGrpName,
                        char *userGrpPasswd,
                        char *userGrpType,
                        char* userGrpAddress, char* userGrpPhone,
                        char* userGrpEmail);

/*
 * srbRegisterUser - Register a user.
 *
 * Input - srbConn* conn - From clConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *         char *userName - The name of the user to register.
 *         char *userDomain - The domain of the user to register.
 *         char *userPasswd - The user passwd.
 *         char *userType - The user type. Currently, at SDSC valid userType
 *              are:
 *              "staff", "sdsc staff", "sdsc staff scientist",
 *              "sdsc senior staff scientist", "pto staff", "ucsd staff"
 *              "student", "sdsc student", "uva student", "project",
 *              "umd student", "public", "sysadmin", " deleted"
 *         char* userAddress - The mailing address of the user.
 *         char* userPhone - The phone number of the user.
 *         char* userEmail - The Email address of the user.
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

extern int
srbRegisterUser (srbConn* conn, int catType, char *userName,
			char *userDomain,
                        char *userPasswd,
                        char *userType,
                        char* userAddress, char* userPhone,
                        char* userEmail);

/*
 * srbModifyUser - Modify a user info.
 *
 * Input - srbConn* conn - From clConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *
 *         char *dataValue1, char *dataValue2 and int retractionType -
 *         They are used to specify the user attributes to modify. A normal
 *         user may use it to modify his/her own passwd and a limited
 *         set of attributes. A user with MDAS sys admin privilege can
 *         also use these input values to modify other user's attributes.
 *
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

extern int
srbModifyUser (srbConn* conn, int catType, char *dataValue1,
char *dataValue2, int retractionType);


/*
 * srbModifyExtMetaData - Modify an extensible metadata table in MCAT. 
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *
 *	   char *dataValue1 ...  char *dataValue5 and int retractionType - 
 *	   They are used to specify the table and rows attributes to modify/add/delete.
           (below, we give the semantics for the data_values for the 
	      supported retraction_type (they are comma separted)
	      D_DELETE_FROM_EXTMD_TABLE
	                    data_1 contains table name
                            data_2 contains one or more attribute names in that table
                            separated by | (only one set)
                            data_3 contains the comparison-values corresponding to
                            those attributes.  data_3 can have many rows/statements 
			    separated by new_line. each will be deleted.
                            Attribute names: data_id,user_id,  and collection_id
                            are keywords and will be substituted. Use 0 as place-holder
	      D_INSERT_INTO_EXTMD_TABLE
	                    data_1 contains table name
                            data_2 contains one or more attribute in that table
                            data_3 contains the values corresponding to
                            those attributes.data_3 can have many rows/statements.
			    Separate rows by new_line. New rows are inserted.
                            Attribute names data_id,user_id,time_val, and collection_id
                            are keywords and will be substituted.  Use 0 as place-holder
              D_MODIFY_IN_EXTMD_TABLE
			    data_1 contains table name
                            data_2 contains one or more attribute in that table and
                            data_3 contains the values corresponding to
                            those attributes.these are new values to be inserted
                            data_4 contains one or more attribute in that table and
                            data_5 contains the comparison-values corresponding to
                            data_4 attributes. These are used to chose the rows.
			    data_5 and data_3 can have more than one row separated by 
			    newline. they should match in number of rows.
                            Separate rows by new_line.
                            Attribute names data_id,user_id,time_val, and collection_id
                            are keywords and will be substituted.  Use 0 as place-holder

 *
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

extern int
srbModifyExtMetaData (srbConn* conn, int catType, char *dataName, char *collName,
		      char *dataValue1, char *dataValue2, char *dataValue3, 
		      char *dataValue4, char *dataValue5, 
		      int retractionType);


/*
 * srbGetPrivUsers - Read the privileged users list and put it in
 * a user supplied char srbUserList[MAX_TOKEN][MAX_TOKEN].
 *
 * Input - srbConn* conn - From clConnect ().
 *         int catalog - The catalog type. e.g., MDAS_CATALOG
 *         mdasC_sql_result_struct *srbUserList - A pointer to a user supplied
 *          mdasC_sql_result_struct.
 *         int rowsWanted - The number of rows to be returned.
 *
 * Output -
 *      0 - success; myresult->continuation_index >= 0, ==> more results from
 *          the query. Use srbGetMoreRows() to retrieve more rows.
 *      Returns a negative value upon failure.
 */
extern int srbGetPrivUsers(srbConn *conn, int catalog,
mdasC_sql_result_struct *myresult, int rowsWanted);

/*
 * srbGetMoreRows - Get more rows of result from a srbGetDatasetInfo,
 * srbGetDataDirInfo, srbListCollect or srbGetPrivUsers calls and put
 * the results in a user supplied mdasC_sql_result_struct.
 *
 * Input - srbConn* conn - From clConnect ().
 *         int catalog - The catalog type. e.g., MDAS_CATALOG
 *         mdasC_sql_result_struct *myresult - A pointer to a user supplied
 *         mdasC_sql_result_struct.
 *         int contDesc - The continuation descriptor. This is a non negative
 *         integer returned from a srbGetDatasetInfo, srbGetDataDirInfo,
 *         srbListCollect or srbGetPrivUsers call as
 *         myresult->continuation_index.
 *         int rowsWanted - The number of rows to be returned.
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */

extern int
srbGetMoreRows(srbConn *conn, int catalog,
int contDesc, mdasC_sql_result_struct *myresult, int rowsWanted);

/*
 * srbGenGetMoreRows -This is a more compact form of srbGetMoreRows.
 * The result is packed with packMsg.
 * Get more rows of result from a srbGetDatasetInfo,
 * srbGetDataDirInfo, srbListCollect or srbGetPrivUsers calls and put
 * the results in a user supplied mdasC_sql_result_struct.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catalog - The catalog type. e.g., MDAS_CATALOG
 *         mdasC_sql_result_struct *srbUserList - A pointer to a user supplied
 *         mdasC_sql_result_struct.
 *         int contDesc - The continuation descriptor. This is a non negative
 *         integer returned from a srbGetDatasetInfo, srbGetDataDirInfo,
 *         srbListCollect or srbGetPrivUsers call as
 *         myresult->continuation_index.
 *         int rowsWanted - The number of rows to be returned.
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */

extern int
srbGenGetMoreRows(srbConn *conn, int catalog,
int contDesc, mdasC_sql_result_struct *myresult, int rowsWanted);

extern void
freeSqlResult (mdasC_sql_result_struct *myresult);

/* clearSqlResult - Clear the content of myresult. Myresult is not freed
 * itself.
 *
 * Input : mdasC_sql_result_struct *myresult - The mdasC_sql_result_struct
 * to be cleared.
 *
 * Output - None
 */

extern void
clearSqlResult (mdasC_sql_result_struct *myresult);

/* printSqlResult - Print the content of myresult.
 *
 * Input : mdasC_sql_result_struct *myresult - The mdasC_sql_result_struct
 * to be printed.
 *
 * Output - None
 */

extern void
printSqlResult (mdasC_sql_result_struct *myresult);



/* XXXXXX - The following are prototypes of Miscellaneous SRB API.
 *
 * XXXXXX
 */

/*
 * srbVaultInfo()
 * Get Info on the SRB storage vault.
 *
 * Input : srbConn* conn
 *
 * Output: A link list of struct vaultQueElement with the queue head pointed
 * to by vaultQueHead.
 *
 * Return value :
 *      struct vaultQueElement *vaultQueHead - pointer to the
 *      head of the link list
 */

extern struct vaultQueElement *srbVaultInfo(srbConn* conn);

/*
 * srbFreeVaultInfo () - Free memory taken by the vaultQueElement link list.
 *
 * Input - vaultQueElement *vaultQueHead
 *
 * Output - None.
 */

extern void srbFreeVaultInfo(struct vaultQueElement *vaultQueHead);

/*
 * srbPrintVaultInfo () - Print out the VaultInfo link list.
 *
 * Input - vaultQueElement *vaultQueHead
 *
 * Output - Printed output.
 */

extern void srbPrintVaultInfo(struct vaultQueElement *vaultQueHead);

/*
 * srbHostConfig()
 * Get the storage vault information on a SRB host.
 *
 * Input : srbConn* conn
 *
 * Output: A link list of struct clHostElement with the queue head pointed
 * to by hostQueHead. 
 *
 * Return value :
 *      struct clHostElement *hostQueHead - pointer to the
 *      head of the link list
 *
 */
extern struct clHostElement *srbHostConfig(srbConn* conn);

/*
 * srbPrintHostInfo () - Print out the VaultInfo link list.
 *
 * Input - vaultQueElement *vaultQueHead
 *
 * Output - Printed output.
 */
 
extern void srbPrintHostInfo(struct clHostElement *vaultQueHead);

/*
 * srbFreeHostInfo () - Free memory taken by the clHostElement link list.
 *
 * Input - clHostElement *hostQueHead
 *
 * Output - None.
 */
 
extern void srbFreeHostInfo(struct clHostElement *hostQueHead);

/*------------------------------------------------------------------------------
 void srb_perror(fd, error_id, error_mnenomic, flags)

  SYNOPSIS
     #include <srbErrno.h>

     void srb_perror(fd, error_id, error_mnenomic, flags);

  DESCRIPTION

     srb_error() emits an SRB error message corresponding to the SRB
     error referred to by <error_id> or <error_mnenomic> (if non-null) .
        if both <error_id> and <error_mnenomic> are passed, <error_mnenomic>
        will take precedence.
         The message is emitted to the file
     descriptor referred to by <fd>. If <fd> is invalid, the
     error will be emitted to the stderr (fd 2).
     Optional behavior of srb_perror is controlled by the <flags>
     parameter. <flags> is constructed by ORing the following values:

     SRB_LONG_MSG       Emit the SRB error long message.
     SRB_RCMD_ACTION    Emit the SRB recommended action.

     The format of the message emitted to <fd> is as follows:

     srb error mnenomic: srb short message
     [ srb long message ]
     [ srb recommended action ]

  INPUT PARAMETERS

     fd        Refers to a file description to which the message should
               be emitted.  If fd is invalid, the message will be emitted
               to stderr.


     error_id  Refers to a valid SRB error id from the SRB error message
               table contained within srb_error.h.  If the error id
               is invalid, the error SRB_NO_ERROR will be emitted.

     flags     Contains the OR'ed value of the flags described above.


------------------------------------------------------------------------------*/
void srb_perror(int fd, int error_id, char *error_mnenomic, int flags);

/*
 * srbContainerCreate - Create a container
 *
 * Input - srbConn* conn - From clConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *         char *containName - The name of the container to be created.
 *         char *containerType - Data type of the container. e.g. "generic"
 *         char *resourceName - The storage resource name. This should be is a
 *          logical resource (resource group) consisting of two physical
 *          resources, a TEMPORARY_RES_CL and a PERMANENT_RES_CL class.
 *         srb_long_t containerSize - The size of the container to be created.
 *
 *
 * Output - Returns 0 ==> success.
 *          Returns a negative value upon failure.
 */
extern int
srbContainerCreate (srbConn *conn, int catType, char *containerName,
char *containerType, char *resourceName, srb_long_t containerSize);

/*
 * srbGetContainerInfo - Get Info on a SRB container.
 *
 * Input : srbConn* conn,
 *         int catType - The catalog type - 0 = MDAS_CATALOG
 *         char *containerName - The name of the container.
 *         mdasC_sql_result_struct *myresult - A pointer to a user supplied
 *          mdasC_sql_result_struct.
 *         int rowsWanted - number of rows of result wanted.
 * Output:
 *         A mdasC_sql_result_struct *myresult
 *
 * Return value :
 *      0 - success; myresult->continuation_index >= 0, ==> more results from
 *          the query. Use srbGetMoreRows() to retrieve more rows.
 *      a negative value - failure
 */

extern int
srbGetContainerInfo (srbConn *conn, int catType, char *containerName,
 mdasC_sql_result_struct *myresult, int rowsWanted);

/*
 * srbRmContainer - remove a container
 *
 * Input - srbConn* conn - From clConnect ().
 *         int catType - catalog type - 0 - MCAT
 *         char *containerName - The name of the container.
 *         int deleteFlag - D_DELETE_ONE - delete the container by the 
 *         owner. D_SU_DELETE_TRASH_ONE - remove the container from
 *         the trash bin by admin user.
 *
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */

extern int
srbRmContainer(srbConn *conn, int catType, char *containerName, 
int deleteFlag);

/*
 * srbSyncContainer - Sync a container - Sync the permanant replica with the
 * temporary replica.
 *
 * Input - srbConn* conn - From clConnect ().
 *         int catType - catalog type - 0 - MCAT
 *         char *containerName - The name of the container.
 *	   int syncFlag - valid values are: 
 *	   	PURGE_FLAG - purge the cache copies after sync is done.
 *		PRIMARY_FLAG - Synchronize to the primary archival resource 
 *		only. The default is to synchronize all archival resources.
 *	   
 *
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */
extern int
srbSyncContainer (srbConn *conn, int catType, char *containerName,
int syncFlag);

/*
 * srbSyncData - Sync a data object - Sync the permanant replica with the
 * temporary replica.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type - 0 - MCAT
 *         char *objID - The SRB object ID to Sync.
 *         char *collectionName - The name of the collection this objID
 *              belongs.
 *         char *resource - The resource for the object to sync to.
 *         A NULL or empty string means synchronize existing copies.
 *
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */

extern int
srbSyncData (srbConn *conn, int catType, char *objID, char *collectionName,
char *resource);
/*
 * srbReplContainer - Replicate a container.
 *
 * Input - srbConn* conn - From clConnect ().
 *         int catType - catalog type - 0 - MCAT
 *         char *containerName - The name of the container.
 *         char *newResourceName - The resource for the replica.
 *
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */

extern int
srbReplContainer (srbConn *conn, int catType, char *containerName,
char *newResourceName);

/*
 * srbContainerOpen - Open a container for ingestion.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type - 0 - MCAT
 *         char *containerName - The name of the container.
 *         int openFlag - O_RDWR, O_WRONLY or O_RDONLY.
 *
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */

extern int
srbContainerOpen (srbConn *conn, int catType, char *containerName, 
int openFlag);

/*
 * srbContainerClose - Close an opened a container.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type - 0 - MCAT
 *         int confFd - The fd returned from srbContainerOpen ().
 *
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */

extern int
srbContainerClose (srbConn *conn, int confFd);

/*
 * srbObjCopy - Copy a dataset. This is a client call as oppose to
 *_srbDataCopy which is only used by the server.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *srcObjID - The source object.
 *         char *srcCollection - The source collection.
 *         char *destObjID - The destination object.
 *         char *destCollection - The destination collection.
 *         char *destResource - The resource to put the destination object.
 *
 * Output - Returns the number of bytes copied.
 *          Returns a negative value upon failure.
 */

extern srb_long_t
srbObjCopy(srbConn* conn, char *srcObjID, char *srcCollection,
char *destObjID, char *destCollection, char *destResource);

/*
 * srbObjPut - Copy a dataset from local space to SRB space. This is
 * only used by the SRB client vs _srbDataPut which is used only by the
 * SRB server.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *destObjID - The destination objID.
 *         char *destCollection - The destination collwction.
 *         char *destResLoc - The destination resource.
 *         char *dataType - The data type.
 *         char *destPath - The destination path name.
 *         char *locFilePath - The local fullPath name.
 *         srb_long_t size - The size of the file. negative means don't know.
 *	   int forceFlag - over write flag
 *
 * Output - Returns the number of bytes copied.
 *          Returns a negative value upon failure.
 */
extern srb_long_t
srbObjPut(srbConn* conn, char *destObjID, char *destCollection,
char *destResLoc, char *dataType, char *destPath, char *locFilePath, 
srb_long_t size, int forceFlag);

/*
 * srbBulkRegister - Register inContainer object in bulk.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type - 0 - MCAT
 *         char *containerName/Resource - For normal objects, this input 
 *         specifies the Resource and for inContainer objects, it is the 
 *         name of the container.
 *         mdasC_sql_result_struct  *myresult - The mdasC_sql_result_struct
 *         that contains the objects to be registered.
 * myresult->sqlresult[0].values   should contain dataNameList
 * myresult->sqlresult[1].values   should contain collectionNameList
 * myresult->sqlresult[2].values   should contain dataSizeList (in ascii)
 *                                  (I will perform atol)
 * If all inContainer objects:
 * myresult->sqlresult[3].values   should contain offSetList (in ascii)
 *
 * If all normal objects:
 * myresult->sqlresult[3].values   should contain dataType
 * myresult->sqlresult[4].values   should contain physical pathName
 * myresult->row_count             should contain the number of datsets to
 *                                      be registered.
 *
 *         int noCheckFlag - 1 - check file permission before register
 *
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */

extern int
srbBulkRegister (srbConn *conn, int catType, char *containerName,
mdasC_sql_result_struct *myresult, int noCheckFlag);

/*
 * srbObjGet - Copy a dataset from SRB space to local space. This is
 * only used by the SRB client vs _srbDataGet which is used only by the
 * SRB server.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *srcObjID - The source objID.
 *         char *srcCollection - The source collwction.
 *         char *locFilePath - The local fullPath name.
 *
 * Output - Returns the number of bytes copied.
 *          Returns a negative value upon failure.
 */

extern srb_long_t
srbObjGet (srbConn* conn, char *srcObjID, char *srcCollection,
char *locFilePath);

/*
 * srbObjGetC - The client initiated version of srbObjGet. Copy a dataset
 * from SRB space to local space. This is only used by the SRB client
 * vs _srbDataGet which is used only by the SRB server.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *srcObjID - The source objID.
 *         char *srcCollection - The source collwction.
 *         char *locFilePath - The local fullPath name.
 *         int flag - not used currently
 *         int numThreads - number of threads
 *
 * Output - Returns the number of bytes copied.
 *          Returns a negative value upon failure.
 */

extern srb_long_t
srbObjGetC (srbConn* conn, char *srcObjID, char *srcCollection,
char *locFilePath, int flag, int numThreads);

/*
 * srbExecFunction - Proxy Operation that exec a function.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *function - The function to run.
 *         char *functionArgv - The function argument string
 *         char *proxyAddr - The host address where this proxy operation
 *                           should be performed. NULL => the server for the
 *                           current connect.
 *         int portalFlag - The portal flag (see definition in srb.h).
 *                          Valid flags are - PORTAL_OFF, PORTAL_ON,
 *                          PORTAL_STD_IN_OUT.
 *
 *
 *
 * Output - Returns 0 or positive value upon success.
 *          Returns a negative value upon failure.
 *          outBuf - any byte stream output. 
 */

extern int
srbExecFunction (srbConn *conn, char *function,
char *functionArgv, char *proxyAddr, int portalFlag);

/*
 * srbSetupSessionPublicKey - Get the MCAT-enabled server's public key
 *    in preparation for transferring encryptioned information.
 *    Also see the sscSetupSessionPublicKey library routine.
 *
 * Input - srbConn* conn - From srbConnect ().
 *
 *
 * Output - Returns positive value upon success (strlen of publicKey)
 *          Returns a NULL or negative value for some failures.
 *          publicKey - a string representation of the current public key,
 *               If server-side secure communication is not supported, 
 *               publicKey will instead contain an error message string.
 */
extern int
srbSetupSessionPublicKey (srbConn *conn, char *publicKey);

/*
 * srbSetupSession - set up a session (for encryption) with the
 *     MCAT-enabled server
 *    Also see the sscSetupSession library routine.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char* sessionKey - a string representation of the session key
 *                            from sscSetupSession (encrypted in the 
 *                            public key)
 *
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */
extern int
srbSetupSession (srbConn *conn, char *sessionKey);

/*
 * srbBulkLoad - Bulk load a set of data. All the actual data should
 * already be contained in bloadFullPath, but in concatenated form
 * just as in the case of a container.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type - 0 - MCAT
 *         char *bloadFullPath - The name of the container.
 *         mdasC_sql_result_struct  *myresult - The mdasC_sql_result_struct
 *         that contains the objects to be registered.
 * myresult->sqlresult[0].values   should contain dataNameList
 * myresult->sqlresult[1].values   should contain collectionNameList
 * myresult->sqlresult[2].values   should contain dataSizeList (in ascii)
 *                                  (I will perform atol)
 * myresult->sqlresult[3].values   should contain offSetList (in ascii)
 * myresult->row_count             should contain the number of datsets to
 *                                      be registered.
 * myresult->continuation_index    0 - no chksum; K_FLAG - verify and register
 *                                 chksum.
 *
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */
extern int
srbBulkLoad (srbConn *conn, int catType, char *bloadFullPath,
mdasC_sql_result_struct *myresult);

/*
 * srbModifyZone - Modify and Insert SRB zone and zone information in the
 *  Metadata Catalog. Information about the operation  performed is 
 *  also logged in the audit  trail (if turned on). This is a
 *  privileged function and should be called only by a 
 *  srbAdmin user.
 *
 *
 * Input - srbConn* conn    - From clConnect ().
 *         int catType      - catalog type. e,g., MDAS_CATALOG.
 *         char *zone_name  - name of the zone
 *         char *dataValue1 - Input value 1.
 *         char *dataValue2 - Input value 2.
 *         char *dataValue3 - Input value 3.
 *         char *dataValue4 - Input value 4.
 *         char *dataValue5 - Input value 5.
 *         int  actionType  - The type of action. performed
 *                  values supported are:
 *
 *                 INSERT_NEW_LOCAL_ZONE
 *                     dv1 = locn_desc
 *                     dv2 = port_number
 *                     dv3 = username@domain   of remote MCAT admin
 *                     dv4 = zone_contact
 *                     dv5 = zone_comment
 *                 INSERT_NEW_ALIEN_ZONE
 *                     dv1-5 = same as for INSERT_NEW_LOCAL_ZONE
 *                 MODIFY_ZONE_INFO
 *                     dv1-5 = same as for INSERT_NEW_LOCAL_ZONE
 *                           empty string implies no change.
 *                 MODIFY_ZONE_FOR_USER
 *                     dv1 = user_name
 *                     dv2 = domain_name
 *                 CHANGE_ZONE_NAME
 *                     dv1 = new name
 *                 MODIFY_ZONE_LOCAL_FLAG
 *                     dv1 = new value (integer)
 *                 MODIFY_ZONE_STATUS
 *                     dv1 = new value (integer)
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

extern int srbModifyZone (srbConn* conn, int catType,
char *zoneName, char *dataValue1, char *dataValue2,
char *dataValue3, char *dataValue4, char *dataValue5, int actionType);



/*
 * srbBulkQueryAnswer -  Get answers for canned queries from MCAT.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type - 0 - MCAT
 *         char *queryInfo - query information all info needed to
 *               perform the query is in this string. The semantics for
 *               the string depends upon the operation.
 *         int rowsWanted - number of rows of result wanted.
 *
 * Output:
 *         A mdasC_sql_result_struct *myresult
 *
 * Return value :
 *      0 - success; myresult->continuation_index >= 0, ==> more results from
 *          the query. Use srbGetMoreRows() to retrieve more rows.
 *      a negative value - failure
 */
extern int
srbBulkQueryAnswer (srbConn *conn, int catType, char *queryInfo,
mdasC_sql_result_struct *myresult, int rowsWanted);


/*
 * srbBulkMcatIngest - Bulk ingestion of a set of metadata in to the 
 * SRB-MCAT system. 
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type - 0 - MCAT
 *         char *ingestInfo - information about what is done. All info 
 *               needed to perform the ingestion is in this string. The 
 *               semantics for the string depends upon the operation.
 *         mdasC_sql_result_struct  *myresult - The mdasC_sql_result_struct
 *         that contains the objects to be ingested.
 *
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */
extern int
srbBulkMcatIngest (srbConn *conn, int catType, char *ingestInfo,
mdasC_sql_result_struct *myresult);

/*
 * srbBulkUnload - Bulk unload of non-container files in a collection
 * recursively.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int flag - flag value for future use.
 *         char *collection - The collection to unload
 *         mdasC_sql_result_struct *myresult - A list of files to be
 *          unloaded.
 *
 * Output - In addition to myresult, returns 0 or positive value
 *          representing the socket fd from which the data will come.
 *
 */

extern int
srbBulkUnload (srbConn *conn, int catType, int flag,
char *collection, mdasC_sql_result_struct **myresult);

/*
 * srbObjChksum - Checksum a SRB data file. By default, if the chksum
 * already already exists, do nothing and return the cjksum value.
 * If the chksum does not exist, compute and reigister it.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *objID - The data name.
 *         char *collectionName - The collection name.
 *         int chksumFlag -  valid flags are :
 *           l_FLAG - list the registered chksum value.
 *           c_FLAG - compute chksum, but don't register
 *           f_FLAG - force compute and register of chksum even if one
 *            already exist.
 *         char *inpChksum - No used.
 *
 * Output - 0 - success, CHKSUM_REG - a new chksum has been registered,
 * Returns a negative value upon failure.
 * the CALLER must have allocated enough space in char * chksum to hold 
 * the result returned
 */

extern int
srbObjChksum (srbConn *conn, char *objID, char *collectionName, int chksumFlag,
char *inpChksum, char *chksum);

#ifdef FED_MCAT
/*
 * srbGetMcatZone - Get the mcatName of this user.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *userName - The input userName.
 *         char *domainName - The input domainName.
 *         char *mcatName - The output mcatname. It should be preallocated
 *              with length MAX_TOKEN
 *
 * Output - Returns 0 if success.
 *          Returns a negative value upon failure.
 * the CALLER must have allocated enough space to hold the result returned
 */

extern int
srbGetMcatZone(srbConn *conn, char *userName, char *domainName, char *mcatName);
#endif


/*
 * srbModifyUserNonPriv - Modify and Insert SRB user information in the
 *  Metadata Catalog. Information about the operation  performed is 
 *  also logged in the audit  trail (if turned on). 
 *
 * Input - srbConn* conn    - From clConnect ().
 *         int catType      - catalog type. e,g., MDAS_CATALOG.
 *         char *userNameDomain  - name@domain of the user
 *         char *dataValue1 - Input value 1.
 *         char *dataValue2 - Input value 2.
 *         char *dataValue3 - Input value 3.
 *         char *dataValue4 - Input value 4.
 *         char *dataValue5 - Input value 5.
 *         int  actionType  - The type of action. performed
 *                  values supported are:
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

extern int srbModifyUserNonPriv (srbConn* conn, int catType,
char *userNameDomain, char *dataValue1, char *dataValue2,
char *dataValue3, char *dataValue4, char *dataValue5, int actionType);


/*
 * srbModifyResource - Modify and Insert SRB resource information in the
 *  Metadata Catalog. Information about the operation  performed is 
 *  also logged in the audit  trail (if turned on). This is a
 *  privileged function and should be called only by a 
 *  srbAdmin user.
 *
 * Input - srbConn* conn    - From clConnect ().
 *         int catType      - catalog type. e,g., MDAS_CATALOG.
 *         char *resourceName  - name of the resiurce
 *         char *dataValue1 - Input value 1.
 *         char *dataValue2 - Input value 2.
 *         char *dataValue3 - Input value 3.
 *         char *dataValue4 - Input value 4.
 *         int  actionType  - The type of action. performed
 *                  values supported are:
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

extern int srbModifyResource (srbConn* conn, int catType,
char *resourceName, char *dataValue1, char *dataValue2,
char *dataValue3, char *dataValue4,  int actionType);

/*
 * srbObjPutC - The client initiated version of srbObjPut. Copy a dataset
 * from local space to SRB space. This is only used by the SRB client vs
 * _srbDataPut which is used only by the SRB server.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *destObjID - The destination objID.
 *         char *destCollection - The destination collwction.
 *         char *destResLoc - The destination resource.
 *         char *dataType - The data type.
 *         char *destPath - The destination path name.
 *         char *locFilePath - The local fullPath name.
 *         srb_long_t size - The size of the file. negative means don't know.
 *         int forceFlag - over write flag
 *         int numThreads - number of threads
 *
 * Output - Returns the number of bytes copied.
 *          Returns a negative value upon failure.
 */

srb_long_t
srbObjPutC (srbConn* conn, char *destObjID, char *destCollection,
char *destResLoc, char *dataType, char *destPath, char *locFilePath,
srb_long_t size, int forceFlag, int numThreads);

/*
 * srbGetFsFreeSpace - get the FS free space in bytes of the FS where the
 * input path is a file or directory.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int storSysType - Storage system type.
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP, 4 = HTTP.
 *         char *hostAddr - The Host address of the SRB.
 *         char *path - A file or directory path in the FS
 *         int flag - not used
 *
 * Output - Returns the free space in bytes upon success.
 *          Returns a negative value upon failure.
 *
 */

srb_long_t
srbGetFsFreeSpace (srbConn *conn, int storSysType, char *hostAddr, char *path,
int flag);

/*
 * srbObjFStage - Stage a SRB object. Only applicable to samfs resource.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *         char *objID - The SRB object ID to stage. The objID is obtained
 *              through registration with MDAS.
 *         char *collectionName - The collection of the objID.
 *         char *resource - stage the copy in this resource.
 *         int flag - not used currently.
 *
 * Output - Returns 0 upon success, a negative value upon failure.
 */

int
srbObjFStage(srbConn* conn, int catType, char *objID, char *collectionName,
char *resource, int flag);

/*
 * srbFileFStage - FStages an extended file
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int storSysType - Storage system type.
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP, 4 = HTTP.
 *              currently only Unix type for samfs is implemented.
 *         char *hostAddr - The Host address of the SRB.
 *         char *filename - The file Path name to stage. Must be an
 *          absolute path.
 *         int flags - same definition as in unix.
 * Output - Returns the file descriptor for use in later srbFile* calls.
 *          Returns a negative value upon failure.
 */

int
srbFileFStage(srbConn* conn, int storSysType, char *hostAddr, char *filename,
int flags);

/*
 See src/client/clStub.c for a description
*/
int
srbFileLockRange(srbConn* conn, int fd, srb_long_t offset, 
                int size, int mode);

/*
 * srbRegisterLocation - register location information
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *locName - The location name.
 *         char *fullAddr - Full Address.
 *         char *parentLoc - Parent location
 *         char *serverUser - Server User
 *         char *serverUserDomain - Server User Domain
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */

int
srbRegisterLocation(srbConn* conn,
         char *locName,
         char *fullAddr,
         char *parentLoc,
         char *serverUser,
         char *serverUserDomain);
/*
 * srbIngestToken - Ingest Token
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *typeName - The type name.
 *         char *newValue - The new value.
 *         char *parentValue - Parent value.
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */

int
srbIngestToken(srbConn* conn,
         char *typeName,
         char *newValue,
         char *parentValue);
/*
 * srbRegisterResource - Register Resource
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *rescName - The resource name.
 *         char *rescType - Resource type.
 *         char *location - Location.
 *         char *phyPath -  Physical Path.
 *         char *class   -  class.
 *         int size      -  size.
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */

int
srbRegisterResource(srbConn* conn,
                    char *rescName,
                    char *rescType,
                    char *location,
                    char *phyPath,
                    char *class,
                    int size);
/*
 * srbRegisterLogicalResource - Register Logical Resource
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *rescName - The resource name.
 *         char *rescType - Resource type.
 *         char *phyResc -  Physical resource.
 *         char *phyPath -  Physical path.
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */

int
srbRegisterLogicalResource(srbConn* conn,
                    char *rescName,
                    char *rescType,
                    char *phyResc,
                    char *phyPath);
/*
 * srbRegisterReplicateResourceInfo
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *physicalRescName - The physical resource name.
 *         char *rescType - Resource type.
 *         char *oldLogicalRescName - old logical resource name.
 *         char *indefaultPath -  Indefault Path.
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */

int
srbRegisterReplicateResourceInfo( srbConn* conn,
                    char *physicalRescName,
                    char *rescType,
                    char *oldLogicalRescName,
                    char *indefaultPath);
/*
 * srbDeleteValue - Delete a Value; a single MCAT entry
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int valueType - the value (token) type.
 *         char *deleteValue - The value (name) that is being deleted.
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */

int
srbDeleteValue(srbConn* conn,
               int valueType,
               char *deleteValue);

/* getSqlResultLen - Return the length in bytes the query result
 *
 * Input - int colCnt - The number of columns expected.
 *         int rowCnt - The number of rows expected.
 *
 * Output - The length
 */

int
getSqlResultLen (int colCnt, int rowCnt);

/*
 * _srbDataPutC - The client driven version of _srbDataPut. Put a dataset
 * from SRB space to local space. This is only used by the SRB server.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int destTypeInx - The object type index.
 *         e.g., objType = srbObjTypeEntry[destObjInx].objType.
 *         char *destPath - The source path.
 *         char *destResLoc - The source resource.
 *         srb_long_t size - The size of the object.
 *         int flag - must have CL_INIT_CONN_FLAG on
 *         int numThreads - input number of threads
 *         portList_t **myPortList - the output portList.
 *
 * Output - Returns the number of bytes copied.
 *          Returns a negative value upon failure.
 */

srb_long_t
_srbDataPutC(srbConn* conn, int destTypeInx, char *destPath, char *destResLoc,
srb_long_t size, int flag, int numThreads, portList_t **myPortList);

/*
 * srbObjProc - Apply a Procedure on an opened SRB object
 *
 * Input - srbConn* conn - From clConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *         int desc - The object descriptor (from the srbObjOpen call).
 *         char *procName - procedure string with arguments.
 *                each driver knows how to interpret this. 
 *         char *inBuf - The input buffer to be input to proc.
 *         int inBufLen - The number of bytes to write.
 *         char *outBuf - The output buffer to be output by proc.
 *         int outBufLen - The number of bytes to read.
 *
 * Output - Returns the length of bytes in outBuf.
 *          Returns a negative value upon failure.
 * the CALLER must have allocated enough space to hold the result returned
 */

extern int
srbObjProc(srbConn *conn, int desc, char *procName, char *inBuf, int inBufLen, char *outBuf, int outBufLen );



/*
 * srbMDriverOpen - Opens a Mediator Driver Object.
 *
 * Input - srbConn* conn - From clConnect ().
 *         int storSysType - Storage system type.
 *              0 = Antelope Orb, 1 = Datascope.
 *         char *resourceLoc - format : host:port:NULL.
 *         char *dataPath - Format : pathname.
 *         int flags - same definition as in unix.
 *         int mode - Not curently used.
 * Output - Returns the file descriptor for use in later srbMDriver* calls.
 *          Returns a negative value upon failure.
 */

extern int srbMDriverOpen(srbConn* conn, int storSysType,
char *resourceLoc, char *dataPath, int flags, int mode);

/*
 * srbMDriverCreate Create a Mediator Driver Object.
 * Input - srbConn* conn - From clConnect ().
 *         int storSysType - Storage system type.
 *              0 = Antelope Orb, 1 = Datascope.
 *         char *resourceLoc - format : host:port:NULL.
 *         char *dataPath - Format : pathname.
 *         int mode - Not curently used.
 * Output - Returns the file descriptor for use in later srbMDriver* calls.
 *          Returns a negative value upon failure.
 */

extern int srbMDriverCreate(srbConn* conn, int storSysType,
char *hostAddr, char *filename, int mode);


/*
 * srbMDriverUnlink - Unlink a Mediator Driver  object.
 *
 * Input - srbConn* conn - From clConnect ().
 *         int storSysType - Storage system type.
 *              0 = Antelope Orb, 1 = Datascope.
 *         char *resourceLoc - format : host:database:instance.
 *         char *dataPath - Format : tablename/objectID
 * Output - Returns 0 - success.
 *          Returns a negative value upon failure.
 */

extern int
srbMDriverUnlink(srbConn* conn, int storSysType, char *resourceLoc,
char *dataPath);



/*
 * srbBackupData - Backup a data object -  Make a replica to the backup
 * resource. Skip it if a good copy already exist.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type - 0 - MCAT
 *         char *objID - The SRB object ID to Sync.
 *         char *collectionName - The name of the collection this objID
 *              belongs.
 *         char *backupResource - The backup resource
 *         int flag - not used.
 *
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */

int
srbBackupData (srbConn *conn, int catType, char *objID, char *collectionName,
char *backupResource, int flag);
/*
 * srbGetSvrVersion - Get the Server version
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char **outbuf - Pointer to point of output buffer.
 *
 * Output - Returns the status.
 *          Returns a negative value upon failure.
 */

int
srbGetSvrVersion(srbConn *conn, char *outbuf);

/*
 * srbExecCommand - Proxy Operation that exec a command.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *command - The command to run.
 *         char *commandArgv - The command argument string
 *         char *proxyAddr - The host address where this proxy operation
 *                           should be performed. NULL => the server for the
 *                           current connect.
 *         char *filename - The file Path name to open. Must be an
 *         absolute path.
 *         int portalFlag - The portal flag (see definition in srb.h).
 *                          Valid flags are - PORTAL_OFF, PORTAL_ON,
 *                          PORTAL_STD_IN_OUT.
 *
 *
 *
 * Output - Returns 0 or positive value upon success.
 *          Returns a negative value upon failure.
 *          outBuf - any byte stream output.
 */

int
srbExecCommand (srbConn *conn, char *command,
char *commandArgv, char *proxyAddr, char *filename, int portalFlag);

/*
 * srbBulkUnloadC - The client initiated socket conection version of
 * Bulk unload of non-container files in a collection
 * recursively.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int flag - flag value for future use.
 *         char *collection - The collection to unload
 *         mdasC_sql_result_struct *myresult - A list of files to be
 *          unloaded.
 *
 * Output - In addition to myresult, returns 0 or positive value
 *          representing the socket fd from which the data will come.
 *
 */

int
srbBulkUnloadC (srbConn *conn, int catType, int flag,
char *collection, mdasC_sql_result_struct **myresult);


/*
 * srbMDriverOpen - Opens a MDriver Object. 
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int storSysType - Driver system type.
 *         char *resourceLoc - format : host:database:instance.
 *         char *dataPath - Format : 
 *         int flags - same definition as in unix.
 *         int mode - Not curently used. 
 * Output - Returns the file descriptor for use in later srbMDriver* calls.
 *          Returns a negative value upon failure.
 */

int
srbMDriverOpen(srbConn* conn, int storSysType, char *resourceLoc, char *dataPath,
int flags, int mode);


/*
 * srbMDriverCreate Create a MDriver Object.
 * Input - srbConn* conn - From srbConnect ().
 *         int storSysType - Driver system type.
 *         char *resourceLoc - format : host:database:instance.
 *         char *dataPath - data path
 *         int mode - Not curently used.
 * Output - Returns the file descriptor for use in later srbMDriver* calls.
 *          Returns a negative value upon failure.
 */
 
int
srbMDriverCreate(srbConn* conn, int storSysType, char *hostAddr, char *filename,
int mode);

/*
 * srbMDriverClose - close a MDriver Object.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int fd - The MDriver descriptor to close (from srbMDriverClose or
 *         srbMDriverCreate).
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */
int
srbMDriverClose(srbConn* conn, int fd);

/*
 * srbMDriverRead - Read len bytes of the MDriver object into buf
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int fd - The object descriptor (from the srbMDriverOpen call) to read.
 *         char *buf - The input buffer.
 *         int len - The number of bytes to read.
 *
 * Output - Returns the length of bytes read.
 *          Returns a negative value upon failure.
 * the CALLER must have allocated enough space to hold the result returned
 */
 
int
srbMDriverRead(srbConn *conn, int fd, char *buf, int len);


/*
 * srbMDriverWrite - Write len bytes of buf into the MDriver fd
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int fd - The MDriver descriptor to write (from srbMDriverOpen or
 *         srbMDriverCreate).
 *         char *buf - The output buffer.
 *         int len - The length to write.
 *
 * Output - Returns the number of bytes written..
 *          Returns a negative value upon failure.
 *
 * Note : We cannot use the generic routine clCall() because the input
 * byte stream is not a char string.
 */
int
srbMDriverWrite(srbConn *conn, int fd, char *buf, int len);

/*
 * srbMDriverSeek - Change the current read or write location of an MDriver 
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int fd - The MDriver descriptor (from the srbMDriverOpen call) to seek.
 *         int offset - The position of the next operation
 *         int whence - Same definition as in Unix.
 *              SEEK_SET - pointer is set to the value of the Offset parameter.
 *              SEEK_CUR - pointer is set to its current location plus the
 *                      value of the Offset parameter.
 *              SEEK_END - pointer is set to the size of the file plus the
 *                      value of the Offset parameter.
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */
 
int
srbMDriverSeek(srbConn *conn, int fd, int offset, int whence);


/*
 * srbMDriverUnlink - Unlinks a MDriver object.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int storSysType - Driver system type.
 *         char *resourceLoc - format : host:database:instance.
 *         char *dataPath -
 *          char *userName - the user name.
 * Output - Returns 0 - success.
 *          Returns a negative value upon failure.
 */

int
srbMDriverUnlink(srbConn* conn, int storSysType, char *resourceLoc, 
char *dataPath);



/*
 * srbMDriverSync - Change the current read or write location of an MDriver 
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int fd - The MDriver descriptor (from the srbMDriverOpen call) to sync.
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */
int
srbMDriverSync(srbConn *conn, int fd);

/*
 * srbMDriverProc - Apply the procedure/function to the opened 'file' 
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int fd -   The MDriver descriptor (from the srbMDriverOpen call) to 
 *                    which the the proc is applied
 *         char* procedure - the procedure call to be made underneath 
 *                    normally the arguments are made into strings, possibly
 *                    xml-ized and attached after the procedure name. 
 *                    The procedure name and arguments are separated by |
 *                    if some other separator is being used, it should be the
 *                    first character in procName. The procedure is assumed to
 *                    start with alphabet.
 *                    Example: trloadchan|1000|2000|FRD|HLZ
 *         char *inBuf - Input Buffer. can be used to send data of interest
 *         int inBufLen - Length of inBuf buffer
 *         char *outBuf - Output Buffer. can be used to receive data from the 
 *                    procedure.
 *         int outBufLen - Length of the Output Buffer (allocated before call)
 * Output - Return depends upon the procedure.
 *                    Normally a negative number indicates error and 
 *                    a positive number indicates the size of the info in outBuf.
 */
int
srbMDriverProc (srbConn *conn, int fd, char *procName,
		char *inBuf, int inBufLen,
		char *outBuf, int outBufLen);
/*
 * _srbFileChown - Change the mode of a file or directory.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int storSysType - Storage system type.
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP (not supported for
 *              HTTP).
 *         char *hostAddr - The Host address of the SRB.
 *         char *filename - The Path name of the file or directory.
 *          Must be an absolute path.
 *         char *fileUser - not used
 *         char *fileGroup - the group to change to.
 *         int chownFlag - not used

 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 *
 */

int
_srbFileChown(srbConn* conn, int storSysType, char *hostAddr, char *filename,
char *fileUser, char *fileGroup, int chownFlag);

/*
 * seqObjGet - Copy a dataset from SRB space to local space sequentially. 
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *srcObjID - The source objID.
 *         char *srcCollection - The source collwction.
 *         char *locFilePath - The local fullPath name.
 *
 * Output - Returns the number of bytes copied.
 *          Returns a negative value upon failure.
 */

srb_long_t
seqObjGet (srbConn* conn, char *srcObjID, char *srcCollection,
char *locFilePath);

/*
 * seqObjPut - Copy a dataset from local space to SRB space sequentially.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *destObjID - The destination objID.
 *         char *destCollection - The destination collwction.
 *         char *destResLoc - The destination resource.
 *         char *dataType - The data type.
 *         char *destPath - The destination path name.
 *         char *locFilePath - The local fullPath name.
 *         srb_long_t size - The size of the file. negative means don't know.
 *         int forceFlag - over write flag
 *
 * Output - Returns the number of bytes copied.
 *          Returns a negative value upon failure.
 */

srb_long_t
seqObjPut (srbConn* conn, char *destObjID, char *destCollection,
char *destResLoc, char *dataType, char *destPath, char *locFilePath,
srb_long_t size, int forceFlag);

/*
 * srbGetUserByDn - Get the zone:user@domain given the user's dn
 *
 * Input - srbConn* conn - From srbConnect ().
 *
 *         catType - catalog type. e,g., MDAS_CATALOG.
 *         flag - not used
 *         userDn - The Dn of the user
 *         outBuf - output for zone:user@domain
 *
 * Output - Returns 0
 *          Returns a negative value upon failure.
 *
 * The CALLER must have allocated enough space to hold the result returned
 */

int
srbGetUserByDn (srbConn *conn, int catType, int flag, char *userDn,
char **outBuf);

  /*
   * srbModifyForBulkMove - Used for Third-party Physical Copying/Moving of Files in Bulk
   *                       to modify the MCAT information
   *                       this call is to be used in conjunction with the physical move
   *                       call which makes the actual move
   * Input - srbConn* conn - From srbConnect ().
   *         catType - catalog type. e,g., MDAS_CATALOG.
   *         operCode - code for the operation to be performed 
   *                  supported code:
   *                                    BULK_PHY_MOVE
   *                                    BULK_PHY_MOVE_INTO_CONTAINER
   *         operString - Additional Information useful for performing the operation
   *                  for BULK_PHY_MOVE   this holds  the ResourceName
   *                  for BULK_PHY_MOVE_INTO_CONTAINER   this holds  the ContainerName
   *         inDataInfo  mdasC_sql_result_struct  that holds information to be modified
   *                  for BULK_PHY_MOVE information required are:
   *                          dataName,collName,replNum,versionString
   *                          and the pathName  in the given ResourceName
   *                  for BULK_PHY_MOVE_INTO_CONTAINER information required are:
   *                          dataName,collName,replNum,versionString
   *                          and dataSize,offSet  in the given ContainerName
   *
   * Output - Returns 0
   *          Returns a negative value upon failure.
   *
   * the caller has to clean up the mdasC_sql_result_struct
   */

int
srbModifyForBulkMove(srbConn *conn, int catType, 
		    int operCode,  char *operString,
		    mdasC_sql_result_struct *inDataInfo);
int getResultForBunload (int newSock, mdasC_sql_result_struct **myresult);

/*
 * srbBulkMove - Bulk physical move
 *
 * Input - srbConn* conn    - From clConnect ().
 *         int catType      - catalog type. e,g., MDAS_CATALOG.
 *         char *collectionName  - The collection
 *         int flag - not used
 *         char *srcResource - the source resources, multiple resources can be
 *           specified with resource1|resource2|...If srcResource is specified,
 *           only files stored in these resources will be moved.
 *         char *destResource - The target resource to move to.
 *         char *containerName - The container to move to. If containerName
 *           is specified, the destResource will be ignored.
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

int srbBulkMove (srbConn* conn, int catType, char *collectionName, int flag,
char *srcResource, char *destResource, char *containerName);
/*
 * srbBulkUnloadForMove - Bulk unload of non-container files in a collection
 * recursively for phymove.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int flag - flag value for future use.
 *         char *collection - The collection to unload
 *         char *srcResource - The source resource to unload
 *         char *targResource - The target resource to unload
 *         mdasC_sql_result_struct *myresult - A list of files to be
 *          unloaded.
 *
 * Output - In addition to myresult, returns 0 or positive value
 *          representing the socket fd from which the data will come.
 *
 */

int
srbBulkUnloadForMove (srbConn *conn, int catType, int flag,
char *collection, char *srcResource, char *targResource,
mdasC_sql_result_struct **myresult);
/*
 * srbRegPhyMove - Register a phymove of a SRB dataset
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *         char *objID - The SRB object ID to create. The objID is obtained
 *              through registration with MDAS.
 *         char *collectionName - The collection name.
 *         char *curResourceName - the resource of the current copy to be moved
 *         char *curDataPath - The phy path of the current copy
 *         char *newResourceName - The resource of the new copy.
 *         char * newFullPathName- The phy path of the new copy.
 *
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

int
srbRegPhyMove (srbConn* conn, int catType, char *objID,
char *collectionName, char *curResourceName, char *curDataPath,
char *newResourceName, char *newFullPathName);

/*
 * srbBulkCopy - Bulk copy
 *
 * Input - srbConn* conn    - From clConnect ().
 *         int catType      - catalog type. e,g., MDAS_CATALOG.
 *         char *collectionName  - The collection
 *         int flag - not used
 *         char *destResource - The target resource to move to.
 *         char *destCollection - The destination collection
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

int srbBulkCopy (srbConn* conn, int catType, char *collectionName, int flag,
char *destResource, char *destCollection);

#ifdef c_plus_plus
}
#endif

#endif 	/* CL_STUB_EXTERN_H */

