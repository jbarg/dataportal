/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*
 * unixFileDvr.c - Routines to handle Unix type file storage
 */

#include "unixFileDvr.h" 

/* _unixOpen - This is the internal version of open. It is used by both
 * the open and create calls.
 *
 * Input : char *fileName - The path name of the file to be opened.
 *	   int fileFlags - The open flag
 *	   int fileMode - The file mode
 *
 * Output : Returns the file descriptor of the opened file.
 */
 
static int
_unixOpen(char *fileName, int fileFlags, int fileMode)
{
    int tmpfd;
    
    tmpfd = open(fileName,fileFlags,fileMode);
    if (tmpfd == 0) {
        close (tmpfd);
        elog (NOTICE, "_unixOpen - zero descriptor");
	open ("/dev/null", O_RDWR, 0);
        tmpfd = open(fileName,fileFlags,fileMode);
    }
    
    return tmpfd;
}

/* unixOpen - Handles the open call.
 *
 * Input : char *fileName - The file path name to be opened
 *         int fileFlags - The open flag
 *         int fileMode - The file mode
 *
 * Output : Returns the file descriptor of the opened file.
 */

int
 unixOpen(char *fileName, int fileFlags, int fileMode)
{
    int status;

    status = _unixOpen(fileName, fileFlags, fileMode);
    if (status < 0) {
	status = unixErrConv (errno);
        elog (NOTICE, "UNIX open error. errorCode = %d", status);
        return (status);
    } else
        return (status);
}

/* unixCreate - Handles the create call.
 *
 * Input : char *fileName - The file path name to be opened
 *         int fileMode - The file mode
 *
 * Output : Returns the file descriptor of the new file.
 */

int
unixCreate(char *fileName, int fileMode, srb_long_t dummySize)
{
    int status;


    /* allow user to set the mode of the file */
 
    (void) umask((mode_t) 0000);
    status = _unixOpen(fileName, O_RDWR|O_CREAT|O_EXCL, fileMode);
    (void) umask((mode_t) 0077);
 
    if (status < 0) {
        status = unixErrConv (errno);
        elog (NOTICE, "UNIX create error. errorCode = %d", status);
        return (status);
    } else
        return (status);
}

/* unixClose - Handles the close call.
 *
 * Input : int file - The file descriptor to be closed
 *
 * Output : Return status of close
 */

int
unixClose(int file)
{
    int status;

    status = close(file);

    if (file == 0) {
        elog (NOTICE, "UNIX close of fd 0");
        open ("/dev/null", O_RDWR, 0);
    }
    if (status < 0) {
        status = unixErrConv (errno);
        elog (NOTICE, "UNIX close error. errorCode = %d", status);
        return (status);
    } else
        return (status);
}

/* unixRead - Handles the read call.
 *
 * Input : int file - The file descriptor to read
 *	   char *buffer - The input buffer
 *	   int amount - The amount to read
 *
 * Output : Returns to number of bytes read
 */

int
unixRead(int file, char *buffer, int amount)
{
    int	status;


    status = read(file, buffer, amount);

    if (status < 0) {
        status = unixErrConv (errno);
        elog (NOTICE, "UNIX read error. Fd = %d, errorCode = %d", file, status);
        return (status);
    } else
        return (status);
}

/* unixWrite - Handles the write call.
 *
 * Input : int file - The file descriptor to write
 *         char *buffer - The output buffer
 *         int amount - The amount to write
 *
 * Output : Returns to number of bytes written
 */

int
unixWrite(int file, char *buffer, int amount)
{
    int	status;

    status = write(file, buffer, amount);

    if (status < 0) {
        status = unixErrConv (errno);
        elog (NOTICE, "UNIX Write error. errorCode = %d", status);
        return (status);
    } else
        return (status);
}

/* unixSeek - Handles the seek call.
 *
 * Input : int file - The file descriptor to seek
 *         int offset - The position of the next operation
 *         int whence - Same definition as in Unix.
 *              SEEK_SET - pointer is set to the value of the Offset parameter.
 *              SEEK_CUR - pointer is set to its current location plus the
 *                      value of the Offset parameter.
 *              SEEK_END - pointer is set to the size of the file plus the
 *                      value of the Offset parameter.
 *
 * Output : Returns the status of seek
 */

srb_long_t
unixSeek(int file, srb_long_t offset, int whence)
{
    srb_long_t	status;
    srb_long_t seekPos;
    
    status = lseek(file, offset, whence);

    if (status < 0) {
        status = unixErrConv (errno);
        elog (NOTICE, "UNIX seek error. errorCode = %d, fd = %d", file, status);
        return (status);
    } else
        return (status);
}

/* unixSync - Handles the sync call.
 *
 * Input : int file - The file descriptor to sync
 *
 * Output : Returns the status of sync
 */

unixSync(int file)
{
    int	status;
    
    status = fsync(file);

    if (status < 0) {
        status = unixErrConv (errno);
        elog (NOTICE, "UNIX sync error. errorCode = %d", status);
        return (status);
    } else
        return (status);
}

/* unixUnlink - Handles the unlink call.
 *
 * Input : char *filename - The file path name to unlink
 *
 * Output : Returns the status of unlink
 */

int
unixUnlink(char *filename)
{
    int status;
 
    status = unlink(filename);

    if (status < 0) {
        status = unixErrConv (errno);
        elog (NOTICE, "UNIX unlink error. errorCode = %d", status);
        return (status);
    } else
        return (status);
}
 
/* unixStat - Handles the stat call.
 *
 * Input : char *filename - The file path name to stat
 *	   struct stat *statbuf - The place to put the stat output
 *
 * Output : Returns the status of stat
 */

int
unixStat(char *filename, struct stat *statbuf)
{
    int status;
 
    status = stat(filename, statbuf);

    if (status < 0) {
        status = unixErrConv (errno);
	if (DebugLvl > 1)
            elog (NOTICE, "UNIX stat error. errorCode = %d", status); 
        return (status);
    } else
        return (status);
}

int
unixFstat (int fd, struct stat *statbuf)
{
    int status;

    status = fstat(fd, statbuf);

    if (status < 0) {
        status = unixErrConv (errno);
        if (DebugLvl > 1)
            elog (NOTICE, "UNIX fstat error. errorCode = %d", status);
        return (status);
    } else
        return (status);
}

/* unixMkdir - Handles the mkdir call.
 *
 * Input : char *filename - The directory path name to mkdir
 *	   int mode - The mode of the directory
 *
 * Output : Returns the status of mkdir
 */

int
unixMkdir(char *filename, int mode)
{
    int status;
 
    status = mkdir(filename, mode);

    if (status < 0) {
        status = unixErrConv (errno);
        elog (NOTICE, "UNIX mkdir error. errorCode = %d", status);
        return (status);
    } else
        return (status);
}

/* unixChmod - Handles the chmod call.
 *
 * Input : char *filename - The path name to chmod
 *         int mode - The mode of the directory
 *
 * Output : Returns the status of chmod
 */

int
unixChmod (char *filename, int mode)
{
    int status;
 
    status = chmod(filename, mode);

    if (status < 0) {
        status = unixErrConv (errno);
        elog (NOTICE, "UNIX chmod error. errorCode = %d", status);
        return (status);
    } else
        return (status);
}

/* unixRmdir - Handles the rmdir call.
 *
 * Input : char *filename - The path name to rmdir
 *
 * Output : Returns the status of rmdir
 */

int
unixRmdir (char *filename)
{
    int status;
 
    status = rmdir(filename);

    if (status < 0) {
        status = unixErrConv (errno);
        elog (NOTICE, "UNIX rmdir error. errorCode = %d", status);
        return (status);
    } else
        return (status);

}
 
/* unixOpendir - Handles the opendir call.
 *
 * Input : char *dirname - The path name to Opendir
 *	   **sysDepDirP - The address to store the opened *DIR.
 *
 * Output : Returns the status of Opendir - 0 - OK, negative - error.
 */
 
int
unixOpendir (char *dirname, void **sysDepDirP)
{
    int status;
    DIR *dirPtr;
 
    
    dirPtr = opendir (dirname);
    if (dirPtr != NULL) {
	*sysDepDirP = (void *) dirPtr;
	return (0);
    } else {
        status = unixErrConv (errno);
        elog (NOTICE, "UNIX opendir error. errorCode = %d", status);
        return (status);
    }
}

/* unixClosedir - Handles the closedir call.
 *
 * Input : *sysDepDir - The *DIR to close.
 *
 * Output : Returns the status of closedir
 */
 
int
unixClosedir (void *sysDepDir)
{
    int status;

    status = closedir ((DIR *) sysDepDir);   

    if (status < 0) {
        status = unixErrConv (errno);
        elog (NOTICE, "UNIX closedir error. errorCode = %d", status);
        return (status);
    } else
        return (status);
}

/* unixReaddir - Handles the readdir call.
 *
 * Input : *sysDepDir - The *DIR to read.
 *	   struct srbDirent *srbDirent - Address of the output srbDirent.
 *
 * Output : Returns the status of readdir
 */
 
int
unixReaddir (void *sysDepDir, struct srbDirent *srbDirent)
{
    struct dirent *dp;
    int status;

    dp = readdir (sysDepDir);

    if (dp == NULL) {
        status = unixErrConv (errno);
        elog (NOTICE, "UNIX readdir error. errorCode = %d", status);
        return (status);
    } 

#if defined(PORTNAME_linux)
    srbDirent->d_ino = htonl (dp->d_ino);
    srbDirent->d_offset = 0;            /* no equiv entry for alpha */
    srbDirent->d_reclen = htons (dp->d_reclen);
    strcpy (srbDirent->d_name, dp->d_name);
    srbDirent->d_namlen = htons (_D_EXACT_NAMLEN(dp));
#elif defined(PORTNAME_aix)
    srbDirent->d_ino = htonl (dp->d_ino);
    srbDirent->d_offset = htonl (dp->d_offset);
    srbDirent->d_reclen = htons (dp->d_reclen);
    srbDirent->d_namlen = htons (dp->d_namlen);  
    strcpy (srbDirent->d_name, dp->d_name);
#elif defined(PORTNAME_solaris) || defined(PORTNAME_dgux)
    srbDirent->d_ino = htonl (dp->d_ino);
    srbDirent->d_offset = htonl (dp->d_off);
    srbDirent->d_reclen = htons (dp->d_reclen);
    srbDirent->d_namlen = 0;		/* no equiv entry for solaris */
    strcpy (srbDirent->d_name, dp->d_name);
#elif defined(PORTNAME_sunos)
    srbDirent->d_ino = htonl (dp->d_fileno);
    srbDirent->d_offset = htonl (dp->d_off);
    srbDirent->d_reclen = htons (dp->d_reclen);
    strcpy (srbDirent->d_name, dp->d_name);
    srbDirent->d_namlen = htons (dp->d_namlen);  
#elif defined(PORTNAME_alpha)
    srbDirent->d_ino = htonl (dp->d_ino);
    srbDirent->d_offset = 0;		/* no equiv entry for alpha */
    srbDirent->d_reclen = htons (dp->d_reclen);
    strcpy (srbDirent->d_name, dp->d_name);
    srbDirent->d_namlen = htons (dp->d_namlen);  
#elif defined(PORTNAME_c90)
    srbDirent->d_ino = htonl (dp->d_ino);
    srbDirent->d_offset = htonl (dp->d_off);
    srbDirent->d_reclen = htons (dp->d_reclen);
    strcpy (srbDirent->d_name, dp->d_name);
    srbDirent->d_namlen = 0;
#elif defined(PORTNAME_sgi)
    srbDirent->d_ino = htonl (dp->d_ino);
    srbDirent->d_offset = htonl (dp->d_off);
    srbDirent->d_reclen = htons (dp->d_reclen);
    strcpy (srbDirent->d_name, dp->d_name);
    srbDirent->d_namlen = 0;
#else 	/* Use the alpha definition */
    srbDirent->d_ino = htonl (dp->d_ino);
    srbDirent->d_offset = 0;            /* no equiv entry for alpha */
    srbDirent->d_reclen = htons (dp->d_reclen);
    strcpy (srbDirent->d_name, dp->d_name);
    srbDirent->d_namlen = htons (dp->d_namlen);  
#endif
    return (0);
}

srb_long_t 
unixGetFsFreeSpace (char *path, int flag)
{
    int status;
    srb_long_t fssize = INP_ERR_NO_SUPPORT;
#if defined(PORTNAME_solaris)
    struct statvfs statbuf;
#else
    struct statfs statbuf;
#endif
#if defined(PORTNAME_solaris) || defined(PORTNAME_sgi) || defined(PORTNAME_aix) || defined(PORTNAME_linux) || defined(PORTNAME_osx)
#if defined(PORTNAME_solaris)
    status = statvfs (path, &statbuf);
#else
#if defined(PORTNAME_sgi)
    status = statfs (path, &statbuf, sizeof (struct statfs), 0);
#else
    status = statfs (path, &statbuf);
#endif
#endif
    if (status < 0) {
        status = unixErrConv (errno);
        elog (NOTICE, "UNIX statfs error for %s. errorCode = %d", 
	 path, status);
        return (status);
    }
#if defined(PORTNAME_solaris)
    if (statbuf.f_frsize > 0) {
        fssize = statbuf.f_frsize;
    } else {
	fssize = statbuf.f_bsize;
    }
    fssize *= statbuf.f_bavail;
#endif

#if defined(PORTNAME_aix) || defined(PORTNAME_osx) || (PORTNAME_linux)
   fssize = statbuf.f_bavail * statbuf.f_bsize;
#endif
#if defined(PORTNAME_sgi)
    fssize = statbuf.f_bfree * statbuf.f_bsize;
#endif

#endif /* PORTNAME_solaris, PORTNAME_sgi .... */

    return (fssize);
}
