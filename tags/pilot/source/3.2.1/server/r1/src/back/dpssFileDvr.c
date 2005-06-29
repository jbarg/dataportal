/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/**** 
  The driver routines were started by  SanKu Jo of LBL and then
  later modified by A. Rajasekar to conform to the driver requirements.
  Many new functionalities were also added  and the routines were made to
  match the server-cleient environment required by SRB.
****/


/*
 * dpssFileDvr.c-- Routines to handle DPSS storage type calls from SRB
 */

#ifdef DPSS

#include "dpssFileDvr.h"

#define DPSSLOG  1 
#define DPSSLOGFILE "/projects/diglib/srb/SRB1_1_6i/data/dpssLog"

/* _dpssSrbOpen - This is the internal version of open. It is used by both
 * the open and create calls.
 *
 * Input : char *fileName - The path name of the file to be opened.
 *         int fileFlags - The open flag
 *         int fileMode - The file mode
 *
 * Output : Returns the file descriptor of the opened file.
 */

/* defined in include/dpssFileDvr.h */
/*
 * /home/users/lblsrb/SRBVault/lblsrb.lbl/xx.1132369762.913107551
 * Destination of unix file system : skjo
 * In MCAT, it should be changed as  x-dpss://domain/filename.*.* : skjo
 */

static int
_dpssSrbOpen(char *fileName, int fileFlags, int fileMode)
{
    static int osRanOut = 0;
    int     tmpfd;
    
    if(dpssInit()!= DPSSFS_OK)
    {
       dpssPerror("dpssInit()");
       elog (NOTICE, "dpssInit Error\n");
       return(-1);
    }

    elog (NOTICE, " ----------------\n_dpssSrbOpen starting %s %d", fileName, fileMode);

/*
 * In dpss_put ang get, fileMode is 0
 * If I use original fileFlags, there is error for read. 
 * Therefore, I checked the fileFlags. If it is 0, it is for read.
 */

    if(fileFlags==O_RDONLY) {
      elog (NOTICE, "DPSSOpen: Opening file for Read Only\n");
       tmpfd = dpssOpen(fileName, O_RDONLY, 0);
    }
    else {
      elog (NOTICE, "DPSSOpen: Opening file for Write or Create Only\n");
       tmpfd = dpssOpen(fileName,  O_WRONLY | O_CREAT, 0);
    }


    if (tmpfd  < 0)
	{ 
    		elog (NOTICE, " _dpssSrbOpen %d", tmpfd);
		dpssPerror("dpssOpen()");
		return(-1);
	}

#ifdef  DPSSLOG
    dpssStartRcvLogging(tmpfd, NL_FILE, DPSSLOGFILE , NULL, 0);
#endif /* DPSSLOG */
     elog (NOTICE, " _dpssSrbOpen Opened file with fd=%d ", tmpfd);
     return tmpfd;
}


/* dpssSrbOpen - Handles the open call.
 *
 * Input : char *fileName - The file path name to be opened
 *         int fileFlags - The open flag
 *         int fileMode - The file mode
 *
 * Output : Returns the file descriptor of the opened file.
 */

int
dpssSrbOpen(char *fileName, int fileFlags, int fileMode)
{
    int status;
    int retVal;

    /** For SRB/MCAT, we have an extra slash at the beginning of the 
	file name. We need to strip it off before submitting it to DPSS 
    **/
    fileName++;

    status = _dpssSrbOpen(fileName, fileFlags, fileMode);

    if (status < 0) {
        retVal = dpssSrbErrConv (status);
        elog (NOTICE, "DPSS open error. ErrorCode = %d", retVal);
        return (retVal);
    } else
        return (status);
}

/* dpssSrbCreate - Handles the create call.
 *
 * Input : char *fileName - The file path name to be opened
 *         int fileMode - The file mode
 *
 * Output : Returns the file descriptor of the new file.
 */

int
dpssSrbCreate(char *fileName, int fileMode, int size)
{
    int status;
    int retVal;

    /** For SRB/MCAT, we have an extra slash at the beginning of the 
	file name. We need to strip it off before submitting it to DPSS 
    **/
    fileName++;

    status = _dpssSrbOpen(fileName,  O_WRONLY | O_CREAT, 0);

    if (status < 0) {
        retVal = dpssSrbErrConv (status);
        elog (NOTICE, "DPSS create error. ErrorCode = %d", retVal);
        return (retVal);
    }
    elog (NOTICE, "dpssSrbCreate: Allocating %i space in DPSS at fileId:%i\n",
	  size, status);
    if(dpssAlloc(status, size) != size) {
        elog (NOTICE, "DPSS Allocation error. ErrorCode ");
	dpssPerror("dpssAlloc()");
	return(-1);
    }
    elog (NOTICE, " dpssSrbCreate: Allocation Successful\n");

        return (status);

}
 
/* dpssSrbClose - Handles the close call.
 *
 * Input : int file - The file descriptor to be closed
 *
 * Output : status of close
 */

int
dpssSrbClose(int fd)
{
    int status;
    int retVal;

    
    elog (NOTICE, " dpssSrbClsoe end %d", fd);

#ifdef  DPSSLOG
    dpssStopRcvLogging(fd);
#endif /* DPSSLOG */

    status = dpssClose(fd);


    if (status != DPSSFS_OK) {
             	dpssPerror("dpssClose()");
        	retVal = dpssSrbErrConv (status);
    		elog (NOTICE, "dpssSrbClose: Error in dpssDriver  status=%d", status);
        	return (retVal);
	}
    else
        return (status);


}

/* dpssSrbRead - Handles the read call.
 *
 * Input : int file - The file descriptor to read
 *         char *buffer - The input buffer
 *         int amount - The amount to read
 *
 * Output : Returns to number of bytes read
 */

int
dpssSrbRead(int dpssfd, char *buffer, int amount)
{
    int	status;
    int retVal;
    int set_id=-1;
    int nread =0;
    int totalnread = 0;
    int nleft;
    int iosize = NBYTES;

    nleft = amount;

    (void) dpssSetNumRetries(dpssfd, 5);

    elog (NOTICE, "DPSSRead: Total bytes to read: %i bytes\n",amount);
    while (nleft > 0) {
      if (nleft < iosize ) {
	elog (NOTICE, "DPSSRead: Starting to read %i bytes\n",nleft);
	 nread = dpssRead(dpssfd, buffer, nleft);
	 if (nread < 0) {
	   dpssPerror("dpssRead()");
	   elog (NOTICE, "HPSS read error. ErrorCode = %d", nread);
	   return(nread);
	 }
	 elog (NOTICE, "DPSSRead: Read %i bytes\n",nread);
	 totalnread += nread;
	 return (totalnread);
      }
      else {
	elog (NOTICE, "DPSSRead: Starting to read %i bytes\n",iosize);
	nread = dpssRead(dpssfd, buffer, iosize);
	if (nread < 0 ) {
	  dpssPerror("dpssRead()");
	  return(nread);
	}
	elog (NOTICE, "DPSSRead: Read %i bytes\n",nread);
	totalnread += nread;
	if (nread < iosize )
	  return (totalnread);
      }
      nleft -= nread;
      buffer += nread;
    }
    return (amount);
}

/* dpssSrbWrite - Handles the write call.
 *
 * Input : int file - The file descriptor to write
 *         char *buffer - The output buffer
 *         int amount - The amount to write
 *
 * Output : Returns to number of bytes written
 */

int
dpssSrbWrite(int dpssfd, char *buffer, int amount)
{
    int nwritten;
    int nleft;
    int iosize = NBYTES;

    nleft = amount;

    elog (NOTICE, "DPSSWrite: Total bytes to write: %i bytes\n",nleft);

    while (nleft > 0) {
      if (nleft < iosize ) {
	elog (NOTICE, "DPSSWrite: Starting to write %i bytes\n",nleft);
	 nwritten = dpssWrite(dpssfd, buffer, nleft);
	 if (nwritten != nleft) {
	   dpssPerror("dpssWrite()");
	   elog (NOTICE, "HPSS write error. ErrorCode = %d", nwritten);
	   if (nwritten < 0) return(nwritten);
	   else return(-1);
	 }
	 elog (NOTICE, "DPSSWrite: Written %i bytes\n",nwritten);
	 return (amount);
      }
      else {
	elog (NOTICE, "DPSSWrite: Starting to write %i bytes\n",iosize);
	nwritten = dpssWrite(dpssfd, buffer, iosize);
	if (nwritten != iosize) {
	  dpssPerror("dpssWrite()");
	  if (nwritten < 0) return(nwritten);
	  else return(-1);
	}
	elog (NOTICE, "DPSSWrite: Written %i bytes\n",nwritten);
      }
      nleft -= nwritten;
      buffer += nwritten;
    }
    return (amount);
}

/* dpssSrbSeek - Handles the seek call.
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

/*
 * int dpssLseek(int fd, unsigned int offset, int whence)
 ** long vs. int
 */

long
dpssSrbSeek(int fd, long offset, int whence)
{
    int	status;
    int retVal;
    
    status = dpssLseek(fd, offset, whence);

    if (status < 0) {
      dpssPerror("dpssLseek()");
      retVal = dpssSrbErrConv (status);
      elog (NOTICE, "DPSS seek error. ErrorCode = %d", retVal);
      return (retVal);
    } else
      return (status);
}

/* dpssSrbSync - Handles the sync call.
 *
 * Input : int file - The file descriptor to sync
 *
 * Output : Returns the status of sync
 */

int
dpssSrbSync(int fd)
{
    return 0;
}

/* dpssSrbUnlink - Handles the unlink call.
 *
 * Input : char *filename - The file path name to unlink
 *
 * Output : Returns the status of unlink
 */

int
dpssSrbUnlink(char *filename)
{

    /** For SRB/MCAT, we have an extra slash at the beginning of the 
	file name. We need to strip it off before submitting it to DPSS 
    **/
    filename++;

  if (dpssUnlink(filename) != DPSSFS_OK)
    {
      dpssPerror("dpssUnlink()");
      elog (NOTICE, "dpssSrbUnlink: Error when removing file '%s'\n",filename);
      return (-1);
    }
}

/* dpssSrbStat - Handles the stat call.
 *
 * Input : char *filename - The file path name to stat
 *         struct stat *statbuf - The place to put the stat output
 *
 * Output : Returns the status of stat
 */

int
dpssSrbStat(char *filename, struct stat *statbuf)
{
    /** For SRB/MCAT, we have an extra slash at the beginning of the 
	file name. We need to strip it off before submitting it to DPSS 
    **/
    filename++;
        return 0;
}

/* dpssSrbMkdir - Handles the mkdir call.
 *
 * Input : char *filename - The directory path name to mkdir
 *         int mode - The mode of the directory
 *
 * Output : Returns the status of mkdir
 */

int
dpssSrbMkdir(char *filename, int mode)
{
    /** For SRB/MCAT, we have an extra slash at the beginning of the 
	file name. We need to strip it off before submitting it to DPSS 
    **/
    filename++;
        return 0;
}
 
/* dpssSrbChmod - Handles the chmod call.
 *
 * Input : char *filename - The path name to chmod
 *         int mode - The mode of the directory
 *
 * Output : Returns the status of chmod
 */

int
dpssSrbChmod (char *filename, int mode)
{
    /** For SRB/MCAT, we have an extra slash at the beginning of the 
	file name. We need to strip it off before submitting it to DPSS 
    **/
    filename++;
        return 0;
}

/* dpssSrbRmdir - Handles the rmdir call.
 *
 * Input : char *filename - The path name to rmdir
 *
 * Output : Returns the status of rmdir
 */

int
dpssSrbRmdir (char *filename)
{
    /** For SRB/MCAT, we have an extra slash at the beginning of the 
	file name. We need to strip it off before submitting it to DPSS 
    **/
    filename++;
        return 0;
}

/* dpssSrbOpendir - Handles the opendir call.
 *
 * Input : char *dirname - The path name to Opendir
 *         **sysDepDirP - The address to store the opened *descriptor.
 *
 * Output : Returns the status of Opendir - 0 - OK, -1 - error.
 */
 
int
dpssSrbOpendir (char *dirname, void **sysDepDirP)
{
    /** For SRB/MCAT, we have an extra slash at the beginning of the 
	file name. We need to strip it off before submitting it to DPSS 
    **/
    dirname++;
        return 0;
}

/* dpssSrbClosedir - Handles the closedir call.
 *
 * Input : *sysDepDir - The *DIR to close.
 *
 * Output : Returns the status of closedir
 */
 
int
dpssSrbClosedir (void *sysDepDir)
{
    /** For SRB/MCAT, we have an extra slash at the beginning of the 
	file name. We need to strip it off before submitting it to DPSS 
    **/
  char *filename;
  
  filename =  (char *) sysDepDir;
    filename++;
        return 0;
}

/* dpssSrbReaddir - Handles the readdir call.
 *
 * Input : *sysDepDir - The *descriptor to read.
 *         struct srbDirent *srbDirent - Address of the output srbDirent.
 *
 * Output : Returns the status of readdir
 */
 
int
dpssSrbReaddir (void *sysDepDir, struct srbDirent *srbDirent)
{
    /** For SRB/MCAT, we have an extra slash at the beginning of the 
	file name. We need to strip it off before submitting it to DPSS 
    **/
  char *filename;
  
  filename =  (char *) sysDepDir;
    filename++;

    return (0);
}
 
int dpssSrbErrConv (int status)
{
    return status;
}

#endif /* DPSS */
