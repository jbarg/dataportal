/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/* XXXXXX - testing only */
#define TEST_GFTP_PREFIX "gsiftp://srbbrick7.sdsc.edu:8888"

/*
 * exfSw.c-- Routines for storage system switch.
 */

#include "exfSw.h"

/* dummy_rt - A dummy routine to fill in the exfsw table.
 *
 * Input - None
 *
 * Output - None
 */
static int dummy_rt ()
{
    elog (NOTICE, "This extended file storage system is not supported");
    return INP_ERR_NO_SUPPORT;
}

/* void_dummy_rt - A dummy routine to fill in the exfsw table.
 *
 * Input - None
 *
 * Output - None
 */

static void void_dummy_rt ()
{
    elog (NOTICE, "This extended file storage system is not supported");
}

/* long_dummy_rt - A dummy routine to fill in the exfsw table.
 *
 * Input - None
 *
 * Output - None
 */

static srb_long_t long_dummy_rt ()
{
    elog (NOTICE, "This extended file storage system is not supported");
    return INP_ERR_NO_SUPPORT;
}

/* null_rt - A null routine for NULL_FT.
 *
 * Input - None
 *
 * Output - None
 */
static int null_rt ()
{
    return 0;
}

/* void_null_rt - A null routine for NULL_FT.
 *
 * Input - None
 *
 * Output - None
 */

static void void_null_rt ()
{
    return;
}

/* long_null_rt - A null routine for NULL_FT.
 *
 * Input - None
 *
 * Output - None
 */

static srb_long_t long_null_rt ()
{
    return 0;
}

/* null_read - A read routine for NULL_FT.
 *
 * Input - None
 *
 * Output - None
 */

static int null_read (int file, char *buffer, int amount)
{
    return EOF;
}

/* null_write - A write routine for NULL_FT.
 *
 * Input - None
 *
 * Output - None
 */

static int null_write (int file, char *buffer, int amount)
{
    return amount;
}

/*
 *  exfcreate() -- The storage system switch for Create
 *
 * Input : int which - which storage system
 *              0 = Unix, 1 = UniTree, 2 = HPSS.
 *	   char *where - The hostname of the storage system
 *	   char *filename - the file path name
 *	   srb_long_t fileMode - The mode of the file created.
 *
 * Output : The created file descriptor
 */

int
exfcreate(int which, char *where, char *filename, int fileMode, srb_long_t size)
{
    int fd;



#ifdef GRID_FTP
    char myPath[MAX_TOKEN*2];
    if (which == GRIDFTP_FT) {
	sprintf (myPath, "%s::::%s", where, filename);
        filename = myPath;
    }
#endif

    if (which < 0 || which >= nExfswEntries) {
	elog (NOTICE, "The storSysType %d is not supported", which);
	return INP_ERR_NO_SUPPORT;
    }
    fd = (*(exfsw[which].exf_create))(filename, fileMode, size);

    return (fd);
}

/*
 *  exfopen() -- The storage system switch for Open
 *
 * Input : int which - which storage system
 *              0 = Unix, 1 = UniTree, 2 = HPSS.
 *         char *where - The hostname of the storage system
 *         char *filename - the file path name
 *	   int fileFlags - The open flags
 *         int fileMode - The mode of the file created.
 *
 * Output : The opened file descriptor
 */

int
exfopen(int which, char *where, char *filename, int fileFlags,int fileMode)
{
    int fd;

#ifdef GRID_FTP
    char myPath[MAX_TOKEN*2];
    if (which == GRIDFTP_FT) {
	sprintf (myPath, "%s::::%s", where, filename);
        filename = myPath;
    }
#endif 

    if (which < 0 || which >= nExfswEntries) {
        elog (NOTICE, "The storSysType %d is not supported", which);
        return INP_ERR_NO_SUPPORT;
    }

    fd = (*(exfsw[which].exf_open))(filename, fileFlags, fileMode);

    return (fd);
}

/*
 *  exfunlink() -- The storage system switch for Unlink
 *
 * Input : int which - which storage system
 *              0 = Unix, 1 = UniTree, 2 = HPSS.
 *         char *where - The hostname of the storage system
 *         char *filename - the file path name
 *
 * Output : Returns the returned value from the Unlink call.
 */

int
exfunlink(int which, char *where, char *filename)
{
    int status;

#ifdef GRID_FTP
    char myPath[MAX_TOKEN*2];
    if (which == GRIDFTP_FT) {
	sprintf (myPath, "%s::::%s", where, filename);
        filename = myPath;
    }
#endif 

    if (which < 0 || which >= nExfswEntries) {
        elog (NOTICE, "The storSysType %d is not supported", which);
        return INP_ERR_NO_SUPPORT;
    }

    status = (*(exfsw[which].exf_unlink))(filename);
    return (status);    
}

/*
 *  exfclose() -- The storage system switch for Close
 *
 * Input : int which - which storage system
 *              0 = Unix, 1 = UniTree, 2 = HPSS.
 *         char *where - The hostname of the storage system
 *         int file - the file descriptor of the file to close
 *
 * Output : Returns the status of close call.
 */

int
exfclose(int which, char *where, int file)
{
    int status;

    if (which < 0 || which >= nExfswEntries) {
        elog (NOTICE, "The storSysType %d is not supported", which);
        return INP_ERR_NO_SUPPORT;
    }

    status = (*(exfsw[which].exf_close))(file);

    return (status);
}

/*
 *  exfread() -- The storage system switch for Read
 *
 * Input : int which - which storage system
 *              0 = Unix, 1 = UniTree, 2 = HPSS.
 *         char *where - The hostname of the storage system
 *         int file - the file descriptor of the file to read
 *	   char *buffer - The output buffer
 *	   int length - The length in bypts to read
 *
 * Output : Returns the number of bytes read.
 */

int
exfread(int which, char *where, int file, char *buffer, int length)
{
    int status;

    if (which < 0 || which >= nExfswEntries) {
        elog (NOTICE, "The storSysType %d is not supported", which);
        return INP_ERR_NO_SUPPORT;
    }

    status = (*(exfsw[which].exf_read))(file, buffer, length);

    return (status);
}

/*
 *  exfwrite() -- The storage system switch for Write
 *
 * Input : int which - which storage system
 *              0 = Unix, 1 = UniTree, 2 = HPSS.
 *         char *where - The hostname of the storage system
 *         int file - the file descriptor of the file to write
 *         char *buffer - The input buffer
 *         int length - The length in bypts to write
 *
 * Output : Returns the number of bytes written.
 */

int
exfwrite(int which, char *where, int file, char *buffer, int length)
{
    int status;

    if (which < 0 || which >= nExfswEntries) {
        elog (NOTICE, "The storSysType %d is not supported", which);
        return INP_ERR_NO_SUPPORT;
    }

    status = (*(exfsw[which].exf_write))(file, buffer, length);

    return (status);
}

/*
 *  exfsync() -- The storage system switch for Sync
 *
 * Input : int which - which storage system
 *              0 = Unix, 1 = UniTree, 2 = HPSS.
 *         char *where - The hostname of the storage system
 *         int file - the file descriptor of the file to sync
 *
 * Output : Returns the status of the sync operation.
 */

int
exfsync(int which, char *where, int file)
{
    int status;

    if (which < 0 || which >= nExfswEntries) {
        elog (NOTICE, "The storSysType %d is not supported", which);
        return INP_ERR_NO_SUPPORT;
    }

    status = (*(exfsw[which].exf_sync))(file);

    return (status);
}

/*
 *  exfseek() -- The storage system switch for Seek
 *
 * Input : int which - which storage system
 *              0 = Unix, 1 = UniTree, 2 = HPSS.
 *         char *where - The hostname of the storage system
 *         int file - the file descriptor of the file to seek
 *         srb_long_t offset - The position of the next operation
 *         int whence - Same definition as in Unix.
 *              SEEK_SET - pointer is set to the value of the Offset parameter.
 *              SEEK_CUR - pointer is set to its current location plus the
 *                      value of the Offset parameter.
 *              SEEK_END - pointer is set to the size of the file plus the
 *                      value of the Offset parameter.
 *
 * Output : Returns the status of the seek operation.
 */

srb_long_t
exfseek(int which, char *where, int file, srb_long_t offset, int whence)
{
    srb_long_t status;

    if (which < 0 || which >= nExfswEntries) {
        elog (NOTICE, "The storSysType %d is not supported", which);
        return INP_ERR_NO_SUPPORT;
    }

    status = (*(exfsw[which].exf_seek))(file, offset, whence);

    return (status);
}

/*
 *  exfstat() -- The storage system switch for stat
 *
 * Input : int which - which storage system
 *              0 = Unix, 1 = UniTree, 2 = HPSS.
 *         char *where - The hostname of the storage system
 *         char *filename - the file name to stat
 *	   char *statbuf - The place to put the stat result
 *
 * Output : Returns the status of the stat operation.
 */

int
exfstat(int which, char *where, char *filename, char *statbuf)
{
    int status;

    char myPath[MAX_TOKEN*2];
    if (which == GRIDFTP_FT) {
        sprintf (myPath, "%s::::%s", where, filename);
	filename = myPath;
    }

    if (which < 0 || which >= nExfswEntries) {
        elog (NOTICE, "The storSysType %d is not supported", which);
        return INP_ERR_NO_SUPPORT;
    }

    status = (*(exfsw[which].exf_stat))(filename, statbuf);

    return status;
}

/*
 *  exffstat() -- The storage system switch for fstat
 *
 * Input : int which - which storage system
 *              0 = Unix, 1 = UniTree, 2 = HPSS.
 *         char *where - The hostname of the storage system
 *         int fd - the fd of an opened file 
 *	   char *statbuf - The place to put the stat result
 *
 * Output : Returns the status of the stat operation.
 */

int
exffstat(int which, char *where, int fd, char *statbuf)
{
    int status;

    if (which < 0 || which >= nExfswEntries) {
        elog (NOTICE, "The storSysType %d is not supported", which);
        return INP_ERR_NO_SUPPORT;
    }

    status = (*(exfsw[which].exf_fstat))(fd, statbuf);

    return status;
}

/*
 *  exfmkdir() -- The storage system switch for Mkdir
 *
 * Input : int which - which storage system
 *              0 = Unix, 1 = UniTree, 2 = HPSS.
 *         char *where - The hostname of the storage system
 *         char *filename - the directory name to make
 *	   int mode - The mode of the created directory
 *
 * Output : Returns the status of the mkdir operation.
 */

int
exfmkdir(int which, char *where, char *filename, int mode)
{
    int status;
 
    char myPath[MAX_TOKEN*2];
    if (which == GRIDFTP_FT) {
        sprintf (myPath, "%s::::%s", where, filename);
        filename = myPath;
    }


    if (which < 0 || which >= nExfswEntries) {
        elog (NOTICE, "The storSysType %d is not supported", which);
        return INP_ERR_NO_SUPPORT;
    }

    status = (*(exfsw[which].exf_mkdir))(filename, mode);

    return status;
}

/*
 *  exfchmod() -- The storage system switch for Chmod
 *
 * Input : int which - which storage system
 *              0 = Unix, 1 = UniTree, 2 = HPSS.
 *         char *where - The hostname of the storage system
 *         char *filename - the file name to chmod
 *         int mode - The new mode 
 *
 * Output : Returns the status of the chmod operation.
 */

int
exfchmod(int which, char *where, char *filename, int mode)
{
    int status;
 
    char myPath[MAX_TOKEN*2];
    if (which == GRIDFTP_FT) {
        sprintf (myPath, "%s::::%s", where, filename);
        filename = myPath;
    }

    if (which < 0 || which >= nExfswEntries) {
        elog (NOTICE, "The storSysType %d is not supported", which);
        return INP_ERR_NO_SUPPORT;
    }

    status = (*(exfsw[which].exf_chmod))(filename, mode);

    return status;
}

/*
 *  exfrmdir() -- The storage system switch for Rmdir
 *
 * Input : int which - which storage system
 *              0 = Unix, 1 = UniTree, 2 = HPSS.
 *         char *where - The hostname of the storage system
 *         char *filename - the directory name to remove
 *         int mode - The new mode 
 *
 * Output : Returns the status of the rmdir operation.
 */

int
exfrmdir(int which, char *where, char *filename)
{
    int status;
 
    char myPath[MAX_TOKEN*2];
    if (which == GRIDFTP_FT) {
        sprintf (myPath, "%s::::%s", where, filename);
        filename = myPath;
    }

    if (which < 0 || which >= nExfswEntries) {
        elog (NOTICE, "The storSysType %d is not supported", which);
        return INP_ERR_NO_SUPPORT;
    }

    status = (*(exfsw[which].exf_rmdir))(filename);

    return status;
}

/*
 *  exfopendir() -- Open a directory
 *
 * Input : int which - which storage system
 *              0 = Unix, 1 = UniTree, 2 = HPSS.
 *         char *where - The hostname of the storage system
 *         char *dirname - the directory name to open
 *         void **sysDepDirP - the address of the system dependent 
 *		dirent pointer.
 *
 * Output : Returns the status of the operation.
 */

int 
exfopendir(int which, char *where, char *dirname, void **sysDepDirP)
{
    int status;
 
    if (which < 0 || which >= nExfswEntries) {
        elog (NOTICE, "The storSysType %d is not supported", which);
        return INP_ERR_NO_SUPPORT;
    }

    status = (*(exfsw[which].exf_opendir))(dirname, sysDepDirP);

    return status;
}

/*
 *  exfclosedir() -- Open a directory
 *
 * Input : int which - which storage system
 *              0 = Unix, 1 = UniTree, 2 = HPSS.
 *         char *where - The hostname of the storage system
 *         void *sysDepDirP - the the system dependent dirent pointer.
 *
 * Output : Returns the status of the operation.
 */

int 
exfclosedir(int which, char *where, void *sysDepDir)
{
    int status;
 
    if (which < 0 || which >= nExfswEntries) {
        elog (NOTICE, "The storSysType %d is not supported", which);
        return INP_ERR_NO_SUPPORT;
    }

    status = (*(exfsw[which].exf_closedir))(sysDepDir);

    return (status);
}

/*
 *  exfreaddir() -- Read a directory
 *
 * Input : int which - which storage system
 *              0 = Unix, 1 = UniTree, 2 = HPSS.
 *         char *where - The hostname of the storage system
 *         void *sysDepDirP - the the system dependent dirent pointer.
 *         srbDirent *srbDirent - the place to put the output dirent.
 *
 * Output : Returns the status of the operation.
 */
 
int 
exfreaddir(int which, char *where, void *sysDepDir,
struct srbDirent *srbDirent)

{
    int status;

    if (which < 0 || which >= nExfswEntries) {
        elog (NOTICE, "The storSysType %d is not supported", which);
        return INP_ERR_NO_SUPPORT;
    }

    status = (*(exfsw[which].exf_readdir)) (sysDepDir, srbDirent);

    return status;
}

/*
 *  exfsetstorattri() -- The storage system switch for SetStorAttri
 *
 * Input : int which - which storage system
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP.
 *         char *host - the host string
 *         char *user - the user string
 *         char *dir  - the directory
 *
 * Output : Returns the status of the SetStorAttri operation.
 */

int
exfsetstorattri(int which, char *host, char *user, char *dir)
{
    int status;
 
    if (which < 0 || which >= nExfswEntries) {
        elog (NOTICE, "The storSysType %d is not supported", which);
        return INP_ERR_NO_SUPPORT;
    }

     if ((status = (*(exfsw[which].exf_setstorattri))(host, user, dir)) < 0) {
        if (status == -1 && errno > 0)
           status = -errno;
        elog (NOTICE, "cannot setstorattri %s, status=%d",
              host, status);
    }
    return status;
}

int
exfmigrate(int which, char *where, int fd, int srcLevel, int flags)
{
    int status;

    if (which < 0 || which >= nExfswEntries) {
        elog (NOTICE, "The storSysType %d is not supported", which);
        return INP_ERR_NO_SUPPORT;
    }

    status = (*(exfsw[which].exf_migrate))(fd, srcLevel, flags);

    return status;
}

int
exfstage(int which, char *where, int fd, int offset, int len, 
int srcLevel, int flags)
{
    int status;

    if (which < 0 || which >= nExfswEntries) {
        elog (NOTICE, "The storSysType %d is not supported", which);
        return INP_ERR_NO_SUPPORT;
    }

    status = (*(exfsw[which].exf_stage))(fd, offset, len, srcLevel, flags);

    return status;
}

int
exffstage(int which, char *where, char *pathName, int flags)
{
    int status;

    if (which < 0 || which >= nExfswEntries) {
        elog (NOTICE, "The storSysType %d is not supported", which);
        return INP_ERR_NO_SUPPORT;
    }

    status = (*(exfsw[which].exf_fstage))(pathName, flags);

    return status;
}

int
exfpurge(int which, char *where, int fd, int offset, int len, 
int srcLevel, int flags)
{
    int status;

    if (which < 0 || which >= nExfswEntries) {
        elog (NOTICE, "The storSysType %d is not supported", which);
        return INP_ERR_NO_SUPPORT;
    }

    status = (*(exfsw[which].exf_purge))(fd, offset, len, srcLevel, flags);

    return status;
}

srb_long_t
exfgetFsFreeSpace (int which, char *where, char *path, int flag)
{
    srb_long_t status;

    if (which < 0 || which >= nExfswEntries) {
        elog (NOTICE, "The storSysType %d is not supported", which);
        return INP_ERR_NO_SUPPORT;
    }

    status = (*(exfsw[which].exf_getFsFreeSpace))(path, flag);

    return status;
}

/*
 *  exfchown() -- The storage system switch for Chmod
 *
 * Input : int which - which storage system
 *              0 = Unix, 1 = UniTree, 2 = HPSS.
 *         char *where - The hostname of the storage system
 *         char *filename - the file name to chown
 *         int mode - The new mode 
 *
 * Output : Returns the status of the chown operation.
 */

int
exfchown(int which, char *where, char *filename, char *fileUser, 
char *fileGroup, int chownFlag)
{
    int status;
 
    char myPath[MAX_TOKEN*2];
    if (which == GRIDFTP_FT) {
	return 0;
/*	no support for now
        sprintf (myPath, "%s::::%s", where, filename);
        filename = myPath;
*/
    }

    if (which < 0 || which >= nExfswEntries) {
        elog (NOTICE, "The storSysType %d is not supported", which);
        return INP_ERR_NO_SUPPORT;
    }

    status = (*(exfsw[which].exf_chown))(filename, fileUser, fileGroup,
     chownFlag);

    return status;
}

