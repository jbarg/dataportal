/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 

/*
 * utFileDvr.c - Routines to handle UNITREE storage type calls
 */

#include "utFileDvr.h"

#ifdef UTREE

void snd_illus_clt (clt_msg)
char *clt_msg;
{
    elog (NOTICE, clt_msg);
}

/* _utOpen - This is the internal version of open. It is used by both
 * the open and create calls.
 *
 * Input : char *fileName - The path name of the file to be opened.
 *         int fileFlags - The open flag
 *         int fileMode - The file mode
 *
 * Output : Returns the file descriptor of the opened file.
 */

static int
_utOpen(char *fileName, int fileFlags, int fileMode)
{
    int tmpfd;
   
    tmpfd = open(fileName,fileFlags,fileMode);
   
    if (tmpfd < 0) {
        return -1;
    }
    return tmpfd;
}

/* utOpen - Handles the open call.
 *
 * Input : char *fileName - The file path name to be opened
 *         int fileFlags - The open flag
 *         int fileMode - The file mode
 *
 * Output : Returns the file descriptor of the opened file.
 */

int
utOpen(char *fileName, int fileFlags, int fileMode)
{
    return(_utOpen(fileName, fileFlags, fileMode));
}

/* utCreate - Handles the create call.
 *
 * Input : char *fileName - The file path name to be opened
 *         int fileMode - The file mode
 *
 * Output : Returns the file descriptor of the new file.
 */

int
utCreate(char *fileName, int fileMode, int dummySize)
{
    return(_utOpen(fileName, O_RDWR|O_CREAT|O_EXCL, fileMode));
}
 
/* utClose - Handles the close call.
 *
 * Input : int file - The file descriptor to be closed
 *
 * Output : Returns status of close
 */

int
utClose(int file)
{
    
    return (close(file));
}

/* utRead - Handles the read call.
 *
 * Input : int file - The file descriptor to read
 *         char *buffer - The input buffer
 *         int amount - The amount to read
 *
 * Output : Returns to number of bytes read
 */

int
utRead(int file, char *buffer, int amount)
{
    int	returnCode;
    
    returnCode = read(file, buffer, amount);
    return returnCode;
}

/* utWrite - Handles the write call.
 *
 * Input : int file - The file descriptor to write
 *         char *buffer - The output buffer
 *         int amount - The amount to write
 *
 * Output : Returns to number of bytes written
 */

int
utWrite(int file, char *buffer, int amount)
{
    int	returnCode;

    returnCode = write(file, buffer, amount);
    return returnCode;
}

/* utSeek - Handles the seek call.
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

off_t
utSeek(int file, long offset, int whence)
{
    int returnCode;
 
    returnCode = lseek(file, offset, whence);
    return returnCode;
}

/* utSync - Handles the sync call.
 *
 * Input : int file - The file descriptor to sync
 *
 * Output : Returns the status of sync
 */

int
utSync(int file)
{
    int	returnCode;
    
    returnCode = fsync(file);
    return returnCode;
}

/* utUnlink - Handles the unlink call.
 *
 * Input : char *filename - The file path name to unlink
 *
 * Output : Returns the status of unlink
 */

int
utUnlink(char *filename)
{
    int retval;

    retval = unlink(filename);
    return(retval);
}

/* utStat - Handles the stat call.
 *
 * Input : char *filename - The file path name to stat
 *         struct stat *statbuf - The place to put the stat output
 *
 * Output : Returns the status of stat
 */

int
utStat(char *filename, struct stat *statbuf)
{
    int retval;

    retval = nslstat(filename, statbuf);
    return(retval);
}

/* utFstat - Handles the fstat call.
 *
 * Input : int *fd - The file desc to stat
 *         struct stat *statbuf - The place to put the stat output
 *
 * Output : Returns the status of stat
 */

int
utFstat(int fd, struct stat *statbuf)
{
    int retval;

    retval = nslfstat(fd, statbuf);
    return(retval);
}

/* utMkdir - Handles the mkdir call.
 *
 * Input : char *filename - The directory path name to mkdir
 *         int mode - The mode of the directory
 *
 * Output : Returns the status of mkdir
 */

int
utMkdir(char *filename, int mode)
{
    int retval;
 
    retval = mkdir(filename, mode);
    return(retval);
}
 
/* utChmod - Handles the chmod call.
 *
 * Input : char *filename - The path name to chmod
 *         int mode - The mode of the directory
 *
 * Output : Returns the status of chmod
 */

int
utChmod (char *filename, int mode)
{
    int retval;
 
    retval = chmod(filename, mode);
    return(retval);
}

/* utRmdir - Handles the rmdir call.
 *
 * Input : char *filename - The path name to rmdir
 *
 * Output : Returns the status of rmdir
 */

int
utRmdir (char *filename)
{
    int retval;
 
    retval = rmdir(filename);
    return(retval);
}

/* utOpendir - Handles the opendir call.
 *
 * Input : char *dirname - The path name to Opendir
 *         **sysDepDirP - The address to store the opened *DIR.
 *
 * Output : Returns the status of Opendir - 0 - OK, -1 - error.
 */
 
int
utOpendir (char *dirname, void **sysDepDirP)
{
    int retval;
    DIR *dirPtr;
 
    dirPtr = opendir (dirname);
    if (dirPtr != NULL) {
        *sysDepDirP = (void *) dirPtr;
        return (0);
    } else {
        return (-1);
    }
}

/* utClosedir - Handles the closedir call.
 *
 * Input : *sysDepDir - The *DIR to close.
 *
 * Output : Returns the status of closedir
 */
 
int
utClosedir (void *sysDepDir)
{
    closedir ((DIR *) sysDepDir);
    return (0);
}
 
/* utReaddir - Handles the readdir call.
 *
 * Input : *sysDepDir - The *DIR to read.
 *         struct srbDirent *srbDirent - Address of the output srbDirent.
 *
 * Output : Returns the status of readdir
 */
 
int
utReaddir (void *sysDepDir, struct srbDirent *srbDirent)
{
    struct dirent *dp;
 
    dp = readdir (sysDepDir);
 
    if (dp == NULL)
        return (-1);
 
    srbDirent->d_ino = htonl (dp->d_ino);
    srbDirent->d_offset = htonl (dp->d_offset);
    srbDirent->d_reclen = htons (dp->d_reclen);
    srbDirent->d_namlen = htons (dp->d_namlen);
    strcpy (srbDirent->d_name, dp->d_name);
    return (0);
}

#endif /* UTREE */
