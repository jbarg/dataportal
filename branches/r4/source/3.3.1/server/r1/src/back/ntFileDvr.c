/* Copyright   1999   The Regents of the University of California
 * All Rights Reserved
 *
 * Permission to use, copy, modify and distribute any part of this Storage
 * Resource Broker (SRB) software package, for educational, research and
 * non-profit purposes, without fee, and without a written agreement is
 * hereby granted, provided that the above copyright notice, this paragraph
 * and the following three paragraphs appear in all copies.
 * Those desiring to incorporate this SRB software into commercial products
 * or use for commercial purposes should contact the Technology Transfer
 * Office, University of California, San Diego, 9500 Gilman Drive, La Jolla,
 * CA 92093-0910, Ph: (619) 534-5815, FAX: (619) 534-7345.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING
 * LOST PROFITS, ARISING OUT OF THE USE OF THIS SRB SOFTWARE, EVEN IF THE
 * UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE SRB SOFTWARE PROVIDED HEREIN IS ON AN "AS IS" BASIS, AND THE UNIVERSITY
 * OF CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 * ENHANCEMENTS, OR MODIFICATIONS.  THE UNIVERSITY OF CALIFORNIA MAKES NO
 * REPRESENTATIONS AND EXTENDS NO WARRANTIES OF ANY KIND, EITHER IMPLIED OR
 * EXPRESS, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, OR THAT THE USE OF THE
 * SRB SOFTWARE WILL NOT INFRINGE ANY PATENT, TRADEMARK OR OTHER RIGHTS.
 */

/*
 * ntFileDvr.c - Routines to handle Unix type file storage
 */

#include "ntFileDvr.h"
#include "stdlib.h"
#include "stdio.h"
#include "srbMiscExtern.h"
#include "SrbNtUtil.h"
#include <io.h>
#include <direct.h>
#include "srbDirent.h"


extern int DebugLvl;

#if 0
static void _copyFilename(char *to, char *from)
{
	if(strlen(from) <= 3)
	{
		strcpy(to,from);
		return;
	}

	if((from[0] == '/')&&(from[2] == ':'))  /* the path has form /D:/tete/... */
	{
		strcpy(to,&(from[1]));
		return;
	}

	if((from[0] == '\\')&&(from[2] == ':'))
	{
		strcpy(to,&(from[1]));
		return;
	}

	strcpy(to,from);
}
#endif

/* _ntOpen - This is the internal version of open. It is used by both
 * the open and create calls.
 *
 * Input : char *fileName - The path name of the file to be opened.
 *	   int fileFlags - The open flag
 *	   int fileMode - The file mode
 *
 * Output : Returns the file descriptor of the opened file.
 */
 
static int _ntOpen(char *fileName, int fileFlags, int fileMode)
{
    int tmpfd;
	char ntfp[1024];

	SrbPathToNtPath(ntfp,fileName);

    tmpfd = _open(ntfp,fileFlags,fileMode);
    
    return tmpfd;
}

/* ntOpen - Handles the open call.
 *
 * Input : char *fileName - The file path name to be opened
 *         int fileFlags - The open flag
 *         int fileMode - The file mode
 *
 * Output : Returns the file descriptor of the opened file.
 */

int ntOpen(char *fileName, int fileFlags, int fileMode)
{
    int status;
	int NewFileFlag;

	/* always open file in binary mode in NT case. */
	NewFileFlag = fileFlags | _O_BINARY;


    status = _ntOpen(fileName, NewFileFlag, fileMode);
    if (status < 0) 
	{
		status = unixErrConv (errno);
        elog (NOTICE, "NT open error. errorCode = %d", status);
        return (status);
    } 
	
    return (status);
}

/* ntCreate - Handles the create call.
 *
 * Input : char *fileName - The file path name to be opened
 *         int fileMode - The file mode
 *
 * Output : Returns the file descriptor of the new file.
 */

int ntCreate(char *fileName, int fileMode, srb_long_t dummySize)
{
    int status;
	char ntfp[1024];

	SrbPathToNtPath(ntfp,fileName);

    status = _open(ntfp, _O_RDWR|_O_CREAT|_O_EXCL|_O_BINARY, _S_IREAD|_S_IWRITE); 
 
    if (status < 0) 
	{
        status = unixErrConv (errno);
        elog (NOTICE, "NT create error. errorCode = %d", status);
        return (status);
    } 
	
    return (status);
}

/* ntClose - Handles the close call.
 *
 * Input : int file - The file descriptor to be closed
 *
 * Output : Return status of close
 */

int ntClose(int file)
{
    int status;

    status = _close(file);

    if (status < 0) 
	{
        status = unixErrConv (errno);
        elog (NOTICE, "NT close error. errorCode = %d", status);
        return (status);
    }

    return (status);
}

/* ntRead - Handles the read call.
 *
 * Input : int file - The file descriptor to read
 *	   char *buffer - The input buffer
 *	   int amount - The amount to read
 *
 * Output : Returns to number of bytes read
 */

int ntRead(int file, char *buffer, int amount)
{
    int	status;

    status = _read(file, buffer, amount);

    if (status < 0) 
	{
        status = unixErrConv (errno);
        elog (NOTICE, "NT read error. errorCode = %d", status);
        return (status);
    }

    return (status);
}

/* ntWrite - Handles the write call.
 *
 * Input : int file - The file descriptor to write
 *         char *buffer - The output buffer
 *         int amount - The amount to write
 *
 * Output : Returns to number of bytes written
 */

int ntWrite(int file, char *buffer, int amount)
{
    int	status;

    status = _write(file, buffer, amount);

    if (status < 0) 
	{
        status = unixErrConv (errno);
        elog (NOTICE, "NT Write error. errorCode = %d", status);
        return (status);
    } 

    return (status);
}

/* ntSeek - Handles the seek call.
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

srb_long_t ntSeek(int file, srb_long_t offset, int whence)
{
    int	status;
    
    status = _lseek(file, offset, whence);

    if (status < 0) 
	{
        status = unixErrConv (errno);
        elog (NOTICE, "UNIX seek error. errorCode = %d", status);
        return (status);
    }
    
	return (status);
}

/* ntSync - Handles the sync call.
 *
 * Input : int file - The file descriptor to sync
 *
 * Output : Returns the status of sync
 */

int ntSync(int file)
{
    int	status;
   
    status = FlushFileBuffers((HANDLE) file);

    if (status != 0) 
	{
        status = unixErrConv (errno);
        elog (NOTICE, "NT sync error. errorCode = %d", status);
        return (status);
    }

    return (status);
}

/* ntUnlink - Handles the unlink call.
 *
 * Input : char *filename - The file path name to unlink
 *
 * Output : Returns the status of unlink
 */

int ntUnlink(char *filename)
{
    int status;
	char ntfp[1024];

	SrbPathToNtPath(ntfp,filename);
 
	status = _unlink(ntfp);

    if (status < 0) 
	{
        status = unixErrConv (errno);
        elog (NOTICE, "NT unlink error. errorCode = %d", status);
        return (status);
    }
        
	return (status);
}
 
/* ntStat - Handles the stat call.
 *
 * Input : char *filename - The file path name to stat
 *	   struct stat *statbuf - The place to put the stat output
 *
 * Output : Returns the status of stat
 */

int ntStat(char *filename, struct stat *statbuf)
{
    int status;
	char ntfp[1024];

	SrbPathToNtPath(ntfp,filename);

    status = stat(ntfp, statbuf);

    if (status < 0) 
	{
        status = unixErrConv (errno);
		if (DebugLvl > 1)
            elog (NOTICE, "NT stat error. errorCode = %d", status); 
        return (status);
    }
        
	return (status);
}

int ntFstat(int fd, struct stat *statbuf)
{
    int status;

    status = _fstat(fd, statbuf);

    if (status < 0) {
        status = unixErrConv (errno);
        if (DebugLvl > 1)
            elog (NOTICE, "NT fstat error. errorCode = %d", status);
        return (status);
    } else
        return (status);
}

/* ntMkdir - Handles the mkdir call.
 *
 * Input : char *filename - The directory path name to mkdir
 *	   int mode - The mode of the directory
 *
 * Output : Returns the status of mkdir
 */

int ntMkdir(char *filename, int mode)
{
    int status;
	char ntfp[1024];

	SrbPathToNtPath(ntfp,filename);
 
	status = _mkdir(ntfp);

    if (status < 0) 
	{
        status = unixErrConv (errno);
        elog (NOTICE, "NT mkdir error. errorCode = %d", status);
        return (status);
    }
        
	return (status);
}

/* ntChmod - Handles the chmod call.
 *
 * Input : char *filename - The path name to chmod
 *         int mode - The mode of the directory
 *
 * Output : Returns the status of chmod
 */

int ntChmod (char *filename, int mode)
{
    int status;
	char ntfp[1024];

	SrbPathToNtPath(ntfp,filename);
 
	status = _chmod(ntfp, mode);

    if (status < 0) 
	{
        status = unixErrConv (errno);
        elog (NOTICE, "NT chmod error. errorCode = %d", status);
        return (status);
    }
        
	return (status);
}

/* ntRmdir - Handles the rmdir call.
 *
 * Input : char *filename - The path name to rmdir
 *
 * Output : Returns the status of rmdir
 */

int ntRmdir (char *filename)
{
    int status;
	char ntfp[1024];

	SrbPathToNtPath(ntfp,filename);
 
	status = _rmdir(ntfp);

    if (status < 0) 
	{
        status = unixErrConv (errno);
        elog (NOTICE, "NT rmdir error. errorCode = %d", status);
        return (status);
    } 
        
	return (status);
}
 
/* ntOpendir - Handles the opendir call.
 *
 * Input : char *dirname - The path name to Opendir
 *	   **sysDepDirP - The address to store the opened *DIR.
 *
 * Output : Returns the status of Opendir - 0 - OK, negative - error.
 */
 
int ntOpendir (char *dirname, void **sysDepDirP)
{
    int status;
    DIR *dirPtr;

	char ntfp[1024];

	SrbPathToNtPath(ntfp,dirname);
 
    dirPtr = opendir (ntfp);
    if (dirPtr != NULL) 
	{
		*sysDepDirP = (void *) dirPtr;
		return (0);
    } 
	else 
	{
        status = unixErrConv (errno);
        elog (NOTICE, "NT opendir error. errorCode = %d", status);
        return (status);
    }

	return 0;
}

/* ntClosedir - Handles the closedir call.
 *
 * Input : *sysDepDir - The *DIR to close.
 *
 * Output : Returns the status of closedir
 */
 
int ntClosedir (void *sysDepDir)
{
	closedir((DIR *)sysDepDir); 
	return 0;
}

/* ntReaddir - Handles the readdir call.
 *
 * Input : *sysDepDir - The *DIR to read.
 *	   struct srbDirent *srbDirent - Address of the output srbDirent.
 *
 * Output : Returns the status of readdir
 */
 
int ntReaddir (void *sysDepDir, struct srbDirent *srbDirent)
{
    struct dirent *dp;
    int status;

    dp = readdir (sysDepDir);

    if (dp == NULL) 
	{
        status = unixErrConv (errno);
        elog (NOTICE, "NT readdir error. errorCode = %d", status);
        return (status);
    } 

    srbDirent->d_ino = 0;               /* no equiv entry for nt */
    srbDirent->d_offset = 0; 			/* no equiv entry for nt */
    srbDirent->d_reclen = 0; 
    strcpy (srbDirent->d_name, dp->d_name);
    srbDirent->d_namlen = 0; 

    return (0);
}

