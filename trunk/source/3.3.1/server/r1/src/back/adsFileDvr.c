/*
 * adsFileDvr.c - Routines to handle Atlas Datastore storage
 */

/* Changes:
 *
 * 25-Aug-04 BS Change handling of update operation for tapes.  Was causing
 *           old vtape to be overwritten with garbage.  For now, just 
 *           disallow update function, return error.
 */

#include "ctype.h"
#include "string.h"
#include "adsFileDvr.h" 

extern userInfo *ClientUser;

/* _adsOpen - This is the internal version of open. It is used by both
 * the open and create calls.
 *
 * Input : char *fileName - The path name of the file to be opened.
 *         int fileFlags - The open flag
 *         int fileMode - The file mode 
 *
 * Output : Returns the file descriptor of the opened file.
 */
 
static int
_adsOpen(char *tapename, int fileFlags, int fileMode)
{
    int status;
    int ts;                      /* Tape stream descriptor */
    int tapeMode;

    if (DebugLvl > 1)
       elog (NOTICE, "ADS DEBUG[%d]: _adsOpen:: tapename=%s fileFlags=0%o", 
             getpid(), tapename, fileFlags);
  
    status = ADS_SUCCESS;

    tapeMode = taperead;
    if (fileFlags & O_RDONLY) {
        tapeMode = taperead;
    }
    else if (fileFlags & O_WRONLY) {
        tapeMode = tapewrite;  
    }
    else if (fileFlags & O_RDWR) {
       elog(NOTICE, 
            "ADS ERROR[%d] in _adsOpen, O_RDWR (update) not supported by ADS",
            getpid());
        status = ADS_ERROR_ON_OPEN;
    }
    if (DebugLvl > 2)
       elog (NOTICE, "ADS DEBUG[%d]: -- tapeMode=%d", getpid(), tapeMode);

    /* ts = topen(VTPSERVER, tapename, tapeMode, TAPEOWNER, NOSHADOW, 10); */
    /* Timeout on read  (in minutes) is nattempt*30sec/60sec */
    /* nattempt=2880 -> timeout of 1 day                     */
    if (status == ADS_SUCCESS) {
       ts = topen(VTPSERVER, tapename, tapeMode, adsTapeOwner(), NOSHADOW,2880);
       if (ts <= 0) {
          elog(NOTICE, 
               "ADS ERROR[%d] in _adsOpen, topen failed, ts=%d, terrno=%d",
               getpid(), ts, terrno);
          status = ADS_ERROR_IN_VTP_OPEN;
       }
       else {
          if (DebugLvl > 1)
             elog(NOTICE, 
                  "ADS DEBUG[%d]: -- opened tstream, ts=%d tapename=%s", 
                  getpid(), ts, tapename);
       }
    }

    if (status == ADS_SUCCESS) {
       adsInitAdsStreams(ts);

       /* If writing to tape, write tape labels */
       if (tapeMode == tapewrite) {
          if ((status = adsWriteTapeLabels(ts, tapename)) != ADS_SUCCESS) {
             elog (NOTICE, "ADS ERROR[%d] writing tape labels", getpid());
             status = ADS_ERROR_WRITING_TAPE_LABELS;
          }
       }
  
       /* If reading from tape, read tape labels */
       /* if ((tapeMode == taperead) || (tapeMode == tapeupdate)) { */
       if (tapeMode == taperead) {
          if ((status = adsReadTapeLabels(ts, tapename)) != ADS_SUCCESS) {
             elog (NOTICE, "ADS ERROR[%d] reading tape labels", getpid());
             status = ADS_ERROR_READING_TAPE_LABELS;
          }
       }
    }

    /* return ts stream number or error */
    if (status == ADS_SUCCESS)
       status = ts;

    if (DebugLvl > 1)
       elog(NOTICE, "ADS DEBUG[%d]: _adsOpen returns %d for tape=%s", 
            getpid(), status, tapename);
    return(status);
}


/*************************************************************/
/* adsOpen - Handles the open call.
 *
 * Input : char *fileName - The file path name to be opened
 *         int fileFlags - The open flag
 *         int fileMode - The file mode
 *
 * Output : Returns the file descriptor of the opened file.
 */

int
adsOpen(char *fileName, int fileFlags, int fileMode)
{
    int status;
    char tapename[TAPENAMELEN+1];

    if (DebugLvl > 1)
       elog (NOTICE, "ADS DEBUG[%d]: adsOpen: fileName=%s, fileFlags=%d", 
             getpid(), fileName, fileFlags);

    status = ADS_SUCCESS;

    /* Call pathtape server to get tapename from fileName */
    status = adsGetTapename(fileName, tapename);

    if (status == ADS_SUCCESS)
       status = _adsOpen(tapename, fileFlags, fileMode);

    if (status < 0) {
        elog (NOTICE, "ADS ERROR[%d] in open. ", getpid());
        status = ADS_ERROR_ON_OPEN;
    }
    else {
       if (DebugLvl > 1)
          elog(NOTICE, "ADS DEBUG[%d]: adsOpen returns %d tape=%s", 
               getpid(), status, tapename);
    }
    return (status);
}

/*************************************************************/
/* adsCreate - Handles the create call.
 *
 * Input : char *fileName - The file path name to be opened
 *         int fileMode - The file mode
 *
 * Output : Returns the file descriptor of the new file.
 */

int
adsCreate(char *fileName, int fileMode, srb_long_t dummySize)
{
    int status;
    char cmdString[MAXCMD+1];     /* Command to send to pipe */
    char pipeOutput[MAXCMDOUT];   /* Output from piped command */
    int firstNewline;             /* Position of first newline in pipeOutput */
    char tapename[TAPENAMELEN+1]; /* Tape name from pathtape */
    char pool[FLF_LEN_POOLNAME+1]; /* Name of flfsys pool */
    char teststring[MAX_FILENAME+1];
    char *token;
    int gotpool;
    int poolsfound;

    if (DebugLvl > 1)
       elog (NOTICE, "ADS DEBUG[%d]: adsCreate: fileName=%s", 
             getpid(), fileName);

    /* Call pathtape server to create tapename that maps to fileName */
    status = adsCreateTapename(fileName, tapename);

    /* Find pool name part of fileName */
    strcpy(pool, "");
    gotpool = FALSE; poolsfound = FALSE;
    strcpy(teststring, fileName);
    token = strtok(teststring, "/");
    while ((gotpool == FALSE) && (token != NULL)) {
       if (poolsfound == TRUE) {
          strncpy(pool, token, FLF_LEN_POOLNAME);
	  pool[FLF_LEN_POOLNAME] = '\0';
          gotpool = TRUE;
       }
       else if (strncmp(token, POOL_PARENT_DIR, 5) == 0)
          poolsfound = TRUE;
       token = strtok(NULL, "/");
    }
    if (DebugLvl > 1)
       elog (NOTICE, "ADS DEBUG[%d]: adsCreate: pool = %s", getpid(), pool);

    /* Open pipe and issue flfsys command to create pool */
    if (strcmp(pool, "") != 0) {
       sprintf(cmdString, "sysreq flfsys POOL ADD %s %s 2>&1", 
                           adsTapeOwner(), pool);

       if (adsPipeCmd(cmdString, pipeOutput) != ADS_SUCCESS) {
          elog (NOTICE, "ADS ERROR[%d] executing pipe", getpid());
          status = ADS_ERROR_EXECUTING_PIPED_COMMAND;
       }
       elog (NOTICE, "ADS[%d]: %s", getpid(), pipeOutput);
    }

    /* 
      Open pipe and execute flfsys-create command
      NOTE: flfsys output:
         1) if includes "created in pool" - created successfully;
         2) if includes "already exists" - does not need created;
         3) anything else - tapename not created.
    */ 
    if ((status == ADS_SUCCESS)  && (tapename[0] != '?')) {

       /* Open pipe and issue flfsys create command */
       if (strcmp(pool, "") == 0) 
          sprintf(cmdString, "sysreq flfsys CREATE %s %s %s 2>&1", 
                  adsTapeOwner(), tapename, TAPESIZE);
       else
          sprintf(cmdString, "sysreq flfsys CREATE %s %s %s POOL %s 2>&1", 
                  adsTapeOwner(), tapename, TAPESIZE, pool);

       if (adsPipeCmd(cmdString, pipeOutput) != ADS_SUCCESS) {
          elog (NOTICE, "ADS ERROR[%d] executing pipe", getpid());
          status = ADS_ERROR_EXECUTING_PIPED_COMMAND;
       }

       else {

          /* Interpret flfsys output */
          elog (NOTICE, "ADS[%d]: %s", getpid(), pipeOutput);
          if (strstr(pipeOutput, "created in pool") != NULL) {
             status = ADS_SUCCESS;
          }
          else if (strstr(pipeOutput, "already exists") != NULL) {
             status = ADS_SUCCESS;
          }
          else {
             elog (NOTICE, "ADS ERROR[%d] creating %s in flfsys", 
                   getpid(), tapename);
             status = ADS_ERROR_IN_FLFSYS_CREATE;
          }
       }

    }

    /* Open tape stream */
    if (status == ADS_SUCCESS) {
       status = _adsOpen(tapename, O_WRONLY, fileMode);
    }

    return (status);
}

/*************************************************************/
/* adsClose - Handles the close call.
 *
 * Input : int ts - The tape stream descriptor to be closed
 *
 * Output : Return status of close
 */

int
adsClose(int ts)
{
    int status;
    int rc;
    struct RECLIST *rs;

    if (DebugLvl > 1)
       elog (NOTICE, "ADS DEBUG[%d]: adsClose: ts=%d", getpid(), ts);

    status = ADS_SUCCESS;

    rs = adsStreams[ts].rStream;
    if (rs != NULL) {
       if (rs->access == tapewrite) {
          if ( (rc=wreof(rs)) !=0) {
             elog (NOTICE, "ADS ERROR[%d] in adsClose, error writing eof",
                   getpid());
             status = ADS_ERROR_ON_CLOSE;
          }
       }
    }

    if (tclose(ts, ACTION_OK, 0) < TAPE_OK) {
       elog (NOTICE, "ADS ERROR[%d] in close, terrno=%d.", getpid(), terrno);
       status = ADS_ERROR_IN_VTP_CLOSE;
    } 

    if (rs != NULL) {
       if (DebugLvl > 2)
          elog(NOTICE, "ADS DEBUG[%d]: free rs: rs=0x%x", getpid(), rs);
       free(rs);
    }

    return (status);
}

/*************************************************************/
/* adsRead - Handles the read call.
 *
 * Input : int ts - The tape stream descriptor to read
 *	   char *buffer - The input buffer
 *	   int amount - The amount to read
 *
 * Output : Returns to number of bytes read
 */

#define BUFSIZE  32767  
int
adsRead(int ts, char *buffer, int amount)
{
    int	status;
    int bytesRead;
    int nread;
    char *bufPtr;
    int tapeMark;

    if (DebugLvl > 2)
        elog (NOTICE, "ADS DEBUG[%d]: adsRead: ts=%d, amount=%d ", 
              getpid(), ts, amount);

    status = ADS_SUCCESS;
    bytesRead = 0;
    bufPtr = buffer;
    tapeMark = FALSE;

    /* Make calls to tread until the correct number of bytes have 
       been read or an error has been encountered
    */
    while ((bytesRead < amount) && (status == ADS_SUCCESS) && (!tapeMark)) {
       if ((nread = tread(ts, bufPtr, amount)) <= 0 ) { 
          if (nread != TAPE_MARK) {
             elog(NOTICE, "ADS ERROR[%d] in tread, return=%d, terrno=%d", 
                  getpid(), nread, terrno);
             status = ADS_ERROR_IN_TAPE_READ;
          }
          else {
             if (DebugLvl > 3)
                elog(NOTICE, "ADS DEBUG[%d]: -- tread found tape mark", 
                     getpid());
             tapeMark = TRUE;
          }
       }
       else {
          bufPtr += nread;
          bytesRead += nread;
          if (DebugLvl > 2)
             elog(NOTICE, "ADS DEBUG[%d]: -- read %d bytes, total-read=%d", 
                  getpid(), nread, bytesRead);
       }
    }

    /* If found tape mark, backspace to stay positioned at tape mark */
    if (nread == TAPE_MARK) {
      tctl(ts, opbsr, 1);
      if (DebugLvl > 3)
         elog (NOTICE, "ADS DEBUG[%d]: -- back space one block to stay on TM",
               getpid());
    }

    if ((status == ADS_SUCCESS) && (DebugLvl > 2)) {
       elog(NOTICE, "ADS DEBUG[%d] in adsRead, read %d bytes",
          getpid(), bytesRead);
    }

    if (status == ADS_SUCCESS)  {
       status = bytesRead;
       if (DebugLvl > 2) 
          elog(NOTICE, "ADS DEBUG[%d]: -- adsRead returning status=%d",
               getpid(), status);
       if (DebugLvl > 3) 
          fprintf(stderr,"ADS DEBUG[%d]: -- adsRead returning with buffer=\n%s",
             getpid(), buffer);
    }

    return (status);
}

/*************************************************************/
/* adsWrite - Handles the write call.
 *
 * Input : int ts - The tape stream descriptor to write
 *         char *buffer - The output buffer
 *         int amount - The amount to write
 *
 * Output : Returns to number of bytes written
 */

int
adsWrite(int ts, char *buffer, int amount)
{
    int	status;
    int rc;
    int totalWritten;     /* bytes from buffer written */
    int amountRemaining;  /* bytes in buffer remaining to be written */
    char *pBuffer;        /* pointer into buffer for next write */
    int writeSize;        /* size of buffer for next write */

    if (DebugLvl > 2)
        elog (NOTICE, "ADS DEBUG[%d]: adsWrite: ts=%d amount=%d", 
              getpid(), ts, amount);

    /* fprintf(stderr, "ADS STDERR: adsWrite: buffer=%s\n", buffer); */

    status = ADS_SUCCESS;
    rc = 0;
    pBuffer = buffer;
    totalWritten = 0;
    amountRemaining = amount;

    /* Write segments of buffer <= BLOCKSIZE until all of buffer written */
    while ((totalWritten < amount) && (status == ADS_SUCCESS)) {

       /* Determine size of buffer segment to write and set block size */
       amountRemaining = amount - totalWritten;
       if (amountRemaining < BLOCKSIZE) 
          writeSize = amountRemaining;
       else
          writeSize = BLOCKSIZE;
       if ((rc=tsetblk(ts, writeSize)) != TAPE_OK) {
          elog (NOTICE, 
                "ADS ERROR[%d] in write, tsetblk failed, writeSize=%d, rc=%d",
                getpid(), writeSize, rc);
          status = ADS_ERROR_IN_TAPE_WRITE;
       }

       /* Write current buffer segment */
       if (status == ADS_SUCCESS) {
          if ((rc = twrite(ts, pBuffer, writeSize)) != writeSize) {
             elog (NOTICE, "ADS ERROR[%d] in write, bytes written = %d.", 
                   getpid(), rc);
             status = ADS_ERROR_IN_TAPE_WRITE;
          } 
          else {
             /* Move pointer to next buffer segment */
             if ((pBuffer+writeSize) <= (buffer+amount)) {
                pBuffer += writeSize;
             }
             else {
                elog (NOTICE, "ADS ERROR[%d]: internal error in write buffer",
                      getpid());
                status = ADS_ERROR_IN_TAPE_WRITE;
             }
             totalWritten += writeSize;
             if (DebugLvl > 2)
                elog (NOTICE, "ADS DEBUG[%d]: -- adsWrite wrote %d", 
                      getpid(), writeSize);
          }
       } 
    }

    if (status == ADS_SUCCESS) {
       adsStreams[ts].fileWriteSize += (unsigned long) totalWritten;
       status = totalWritten;
    }
    
    if (DebugLvl > 2)
       elog (NOTICE, "ADS DEBUG[%d]: -- ads fileWriteSize set to %lu", 
             getpid(), adsStreams[ts].fileWriteSize);
    if (DebugLvl > 2)
       elog (NOTICE, "ADS DEBUG[%d]: -- adsWrite for ts=%d returns %d", 
             getpid(), ts, status);
    return(status);
}

/*************************************************************/
/* adsSeek - Handles the seek call.
 *
 * Input : int ts - The tape stream descriptor to seek
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
adsSeek(int ts, srb_long_t offset, int whence)
{
    if (DebugLvl > 1)
        elog (NOTICE, "ADS DEBUG[%d]: adsSeek: ts=%d offset=%ld whence=%d", 
              getpid(), ts, offset, whence);

    elog (NOTICE, 
          "ADS ERROR[%d]: The seek operation is not supported for ADS storage type",
          getpid());
    return(ADS_ERROR_FUNC_NOT_SUPPORTED);
}

/*************************************************************/
/* adsSync - Handles the sync call.
 *
 * Input : int ts - The tape stream descriptor to sync
 *
 * Output : Returns the status of sync
 */

adsSync(int ts)
{
    int status;

    if (DebugLvl > 1)
       elog (NOTICE, "ADS DEBUG[%d]: adsSync: ", getpid());

    status = fsync(ts);

    if (status < 0) {
        status = unixErrConv (errno);
        elog (NOTICE, "UNIX sync error. errorCode = %d", status);
    } 
    return (status);
}

/*************************************************************/
/* adsUnlink - Handles the unlink call.
 *
 * Input : char *filename - The file path name to unlink
 *
 * Output : Returns the status of unlink
 */

int
adsUnlink(char *filename)
{
    int status;
    char tapename[TAPENAMELEN+1];
    char cmdString[MAXCMD+1];     /* Command to send to pipe */
    char pipeOutput[MAXCMDOUT];   /* Output from piped command */
    int inUse;                    /* Flfsys thinks file is in-use */
    int count;                    /* Number retries if in-use */

    if (DebugLvl > 1)
       elog (NOTICE, "ADS DEBUG[%d]: adsUnlink: file=%s", getpid(), filename);

    status = ADS_SUCCESS;

    /* Call pathtape server to get tapename from fileName */
    status = adsGetTapename(filename, tapename);

    /* Open pipe and execute flfsys command to destroy file in ADS */
    if (status == ADS_SUCCESS) {

       inUse = TRUE; 
       count = 0;
       while ((inUse == TRUE) && (count < 10)) {
          sprintf(cmdString, "sysreq flfsys DESTROY %s %s 2>&1",
                  adsTapeOwner(), tapename);

          if (adsPipeCmd(cmdString, pipeOutput) != ADS_SUCCESS) {
             elog (NOTICE, "ADS ERROR[%d] executing pipe", getpid());
             status = ADS_ERROR_EXECUTING_PIPED_COMMAND;
          }
          else {
             elog (NOTICE, "ADS[%d]: %s", getpid(), pipeOutput);
          }

          if (status == ADS_SUCCESS) {
             if (strstr(pipeOutput, "destroyed") != NULL) {
                if (DebugLvl > 1)
                   elog (NOTICE, "ADS DEBUG[%d]: destroyed %s in flfsys",
                         getpid(), tapename);
                inUse = FALSE;
             }
             else if (strstr(pipeOutput, "File in use") != NULL) {
                elog (NOTICE, 
                   "ADS[%d]: attempt to destroy file in use; wait and retry",
                    getpid(), tapename);
                inUse = TRUE;
             }
             else {
                elog(NOTICE,
                     "ADS ERROR[%d] flfsys-destroy not completed for %s", 
                      getpid(), filename);
                status = ADS_ERROR_IN_FLFSYS_DESTROY;
                inUse = FALSE;
             }
          }
          count++;
          if (inUse == TRUE)
             sleep(10);
       }
    }

    /* If destroy not completed due to file in use, error exit */
    if (inUse == TRUE) {
       elog(NOTICE,
            "ADS ERROR[%d] flfsys-destroy not completed for %s", 
             getpid(), filename);
       status = ADS_ERROR_IN_FLFSYS_DESTROY;
    }

    /* Open pipe and call pathtape to delete pathname */
    if (status == ADS_SUCCESS) {
       sprintf(cmdString, "sysreq path delete -u %s %s ", 
                           adsTapeOwner(), filename);

       if (adsPipeCmd(cmdString, pipeOutput) != ADS_SUCCESS) {
          elog (NOTICE, "ADS ERROR[%d] executing pipe", getpid());
          status = ADS_ERROR_EXECUTING_PIPED_COMMAND;
       }
       else {
          elog (NOTICE, "ADS[%d]: %s", getpid(), pipeOutput);
       }

       if (status == ADS_SUCCESS) {
          if (strstr(pipeOutput, "Delete") != NULL) {
             status = ADS_SUCCESS;
             if (DebugLvl > 1)
                elog (NOTICE, "ADS DEBUG[%d]: deleted pathtape entry for %s",
                      getpid(), tapename);
          }
          else {
             elog (NOTICE, "ADS ERROR[%d] deleting %s in pathtape",
                   getpid(), filename);
             status = ADS_ERROR_IN_PATHTAPE;
          }
       }

    }


    return (status);
}
 
/*************************************************************/
/* adsStat - Handles the stat call.
 *
 * Input : char *filename - The file path name to stat
 *	   struct stat *statbuf - The place to put the stat output
 *
 * Output : Returns the status of stat
 */

int
adsStat(char *filename, struct stat *statbuf)
{
    int status;
    int ts;        /* tape stream number */
    int i;
    char tapename[TAPENAMELEN+1];

    status = ADS_SUCCESS;
    ts = 0;

    if (DebugLvl > 1)
       elog (NOTICE, "ADS DEBUG[%d]: adsStat: filename=%s", getpid(), filename);

#ifdef NOTHING
    status = stat(filename, statbuf);
    if (status < 0) {
        status = unixErrConv (errno);
        elog (NOTICE, "UNIX stat error. errorCode = %d", status);
    }

    return (status);

#endif
#ifdef NOTHING
    elog (NOTICE, 
          "ADS ERROR[%d]: The Stat operation is not supported for ADS storage type",
          getpid());
    return(ADS_ERROR_FUNC_NOT_SUPPORTED);
#endif
    return(ADS_SUCCESS);
}

/*************************************************************/
int
adsFstat (int ts, struct stat *statbuf)
{
    int status;

    if (DebugLvl > 1)
       elog (NOTICE, "ADS DEBUG[%d]: adsFstat: ts=%d", getpid(), ts);

    /* This is a partial implementation.  The     
       file size is needed to do replicate function */

    if (DebugLvl > 1)
       elog (NOTICE, "ADS DEBUG[%d]: adsFstat: ads fileWriteSize=%lu", 
          getpid(), adsStreams[ts].fileWriteSize);
    statbuf->st_size = adsStreams[ts].fileWriteSize; 

    status = ADS_SUCCESS;
    if (DebugLvl > 1)
       elog (NOTICE, 
             "ADS DEBUG[%d]: adsFstat: returns %d size=%lu", 
              getpid(), status, statbuf->st_size);
    return (status);

}

/*************************************************************/
/* adsMkdir - Handles the mkdir call.
 *
 * Input : char *filename - The directory path name to mkdir
 *	   int mode - The mode of the directory
 *
 * Output : Returns the status of mkdir
 */

int
adsMkdir(char *filename, int mode)
{
    int status;

    if (DebugLvl > 1)
       elog (NOTICE, "ADS DEBUG[%d]: adsMkdir:", getpid());

#ifdef NOTHING
    status = mkdir(filename, mode);

    if (status < 0) {
        status = unixErrConv (errno);
        elog (NOTICE, "UNIX mkdir error. errorCode = %d", status);
    } 
    return (status);
#endif
#ifdef NOTHING
    elog (NOTICE, 
     "ADS ERROR[%d]: The Mkdir operation is not supported for ADS storage type",
          getpid());
    return(ADS_ERROR_FUNC_NOT_SUPPORTED);
#endif

    /* Does nothing, but let it return successfully to avoid vault errors */
    return(ADS_SUCCESS);
}

/*************************************************************/
/* adsChmod - Handles the chmod call.
 *
 * Input : char *filename - The path name to chmod
 *         int mode - The mode of the directory
 *
 * Output : Returns the status of chmod
 */

int
adsChmod (char *filename, int mode)
{
    int status;

    if (DebugLvl > 1)
       elog (NOTICE, "ADS DEBUG[%d]: adsChmod: ", getpid());

#ifdef NOTHING
    status = chmod(filename, mode);

    if (status < 0) {
        status = unixErrConv (errno);
        elog (NOTICE, "UNIX chmod error. errorCode = %d", status);
    } 
    return (status);

#endif
    elog (NOTICE, 
          "ADS ERROR[%d]: The Chmod operation is not supported for ADS storage type",
          getpid());
    return(ADS_ERROR_FUNC_NOT_SUPPORTED);
}

/*************************************************************/
/* adsRmdir - Handles the rmdir call.
 *
 * Input : char *filename - The path name to rmdir
 *
 * Output : Returns the status of rmdir
 */

int
adsRmdir (char *filename)
{
    int status;

    if (DebugLvl > 1)
       elog (NOTICE, "ADS DEBUG[%d]: adsRmdir: ", getpid());

#ifdef NOTHING
    status = rmdir(filename);

    if (status < 0) {
        status = unixErrConv (errno);
        elog (NOTICE, "UNIX rmdir error. errorCode = %d", status);
    } 
    return (status);
#endif
    elog (NOTICE, 
          "ADS ERROR[%d]: The Rmdir operation is not supported for ADS storage type",
          getpid());
    return(ADS_ERROR_FUNC_NOT_SUPPORTED);
}
 
/*************************************************************/
/* adsOpendir - Handles the opendir call.
 *
 * Input : char *dirname - The path name to Opendir
 *	   **sysDepDirP - The address to store the opened *DIR.
 *
 * Output : Returns the status of Opendir - 0 - OK, negative - error.
 */
 
int
adsOpendir (char *dirname, void **sysDepDirP)
{
    int status;
    DIR *dirPtr; 

    if (DebugLvl > 1)
       elog (NOTICE, "ADS DEBUG[%d]: adsOpendir: ", getpid());

#ifdef NOTHING
    dirPtr = opendir (dirname);
    if (dirPtr != NULL) {
        *sysDepDirP = (void *) dirPtr;
        status = 0;
    } else {
        status = unixErrConv (errno);
        elog (NOTICE, "UNIX opendir error. errorCode = %d", status);
    }
    return (status);
#endif
    elog (NOTICE, 
          "ADS ERROR[%d]: The Opendir operation is not supported for ADS storage type",
          getpid());
    return(ADS_ERROR_FUNC_NOT_SUPPORTED);

}

/*************************************************************/
/* adsClosedir - Handles the closedir call.
 *
 * Input : *sysDepDir - The *DIR to close.
 *
 * Output : Returns the status of closedir
 */
 
int
adsClosedir (void *sysDepDir)
{
    int status;

    if (DebugLvl > 1)
       elog (NOTICE, "ADS DEBUG[%d]: adsClosedir: ", getpid());

#ifdef NOTHING
    status = closedir ((DIR *) sysDepDir);

    if (status < 0) {
        status = unixErrConv (errno);
        elog (NOTICE, "UNIX closedir error. errorCode = %d", status);
    } 
    return (status);
#endif
    elog (NOTICE, 
          "ADS ERROR[%d]: The Closedir operation is not supported for ADS storage type",
          getpid());
    return(ADS_ERROR_FUNC_NOT_SUPPORTED);
}

/*************************************************************/
/* adsReaddir - Handles the readdir call.
 *
 * Input : *sysDepDir - The *DIR to read.
 *	   struct srbDirent *srbDirent - Address of the output srbDirent.
 *
 * Output : Returns the status of readdir
 */
 
int
adsReaddir (void *sysDepDir, struct srbDirent *srbDirent)
{
    struct dirent *dp;
    int status;

    if (DebugLvl > 1)
       elog (NOTICE, "ADS DEBUG[%d]: adsReaddir: ", getpid());

    elog (NOTICE, 
          "ADS ERROR[%d]: The Readdir operation is not supported for ADS storage type",
          getpid());
    return(ADS_ERROR_FUNC_NOT_SUPPORTED);

#ifdef NOTHING
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
    srbDirent->d_namlen = 0;            /* no equiv entry for solaris */
    strcpy (srbDirent->d_name, dp->d_name);
#elif defined(PORTNAME_sunos)
    srbDirent->d_ino = htonl (dp->d_fileno);
    srbDirent->d_offset = htonl (dp->d_off);
    srbDirent->d_reclen = htons (dp->d_reclen);
    strcpy (srbDirent->d_name, dp->d_name);
    srbDirent->d_namlen = htons (dp->d_namlen);
#elif defined(PORTNAME_alpha)
    srbDirent->d_ino = htonl (dp->d_ino);
    srbDirent->d_offset = 0;            /* no equiv entry for alpha */
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
#else   /* Use the alpha definition */
    srbDirent->d_ino = htonl (dp->d_ino);
    srbDirent->d_offset = 0;            /* no equiv entry for alpha */
    srbDirent->d_reclen = htons (dp->d_reclen);
    strcpy (srbDirent->d_name, dp->d_name);
    srbDirent->d_namlen = htons (dp->d_namlen);
#endif
    return (0);
#endif

}


/*************************************************************/
/* adsPipeCmd - Sets up pipe to run command and return output.
 *
 * Input : *cmdString - command to run   
 *         *cmdout    - output of command
 *
 * Output : Returns the status of pipe operation
 */
int
adsPipeCmd(char *cmdString, char *cmdout)
{
   FILE *cmdPipe;                 /* Access to pipe command output */
   int i;

   /* Open pipe and execute command */
   cmdPipe = popen(cmdString, "r");
   if (cmdPipe == NULL) {
      elog(NOTICE, "ADS ERROR[%d] in adsPipeCmd", getpid());
      return ADS_ERROR_EXECUTING_PIPED_COMMAND;
   }
   else {
      elog (NOTICE, "ADS[%d]: execute: %s", getpid(), cmdString);
   }

   /* Get output from stdout of command executed */
   i=0;
   do {
      cmdout[i] = getc(cmdPipe);
   } while ((!feof(cmdPipe)) && (i < MAXCMDOUT) && (cmdout[i++] != '\0'));
   cmdout[i] = '\0';

   elog (NOTICE, "ADS[%d]: pipeout: %s", getpid(), cmdout);

   pclose(cmdPipe);
   return ADS_SUCCESS;
}

/*************************************************************/
/* adsGetTapename - Open pipe and call pathtape to get 6-char 
 *                  tapename from full pathname
 *
 * Input : *pathname  - Unix-style pathname
 *         *tapename  - 6-char ADS tapename
 *
 * Output : Returns ADS_SUCCESS if tapename found; otherwise and 
 *          ADS error code
 */
int
adsGetTapename(char *pathname, char *tapename) 
{
    int status;
    char formatString[10];       /* For building format for sscanf */
    char cmdString[MAXCMD+1];    /* Command to send to pipe */
    char pipeOutput[MAXCMDOUT];  /* Output from piped command */
    int firstNewline;            /* Position of first newline in pipeOutput */
    char *s;                     /* Pointer to substring */

    status = ADS_SUCCESS;
    strcpy(tapename, "?");
    sprintf(formatString, "%%%ds", TAPENAMELEN);  

    /*
       Open pipe and call pathtape to get 6-char tapename from full pathname
    */
    sprintf(cmdString, "sysreq path get -u %s %s ", adsTapeOwner(), pathname);
    if (adsPipeCmd(cmdString, pipeOutput) != ADS_SUCCESS) {
       elog (NOTICE, "ADS ERROR[%d] executing pipe", getpid());
       status = ADS_ERROR_EXECUTING_PIPED_COMMAND;
    }
   
    else {
       /*
          Get tapename from pathtape output
          NOTE: this code depends on exact output from pathtape command;
                this could break if pathtape output changes.
       */
       sscanf(pipeOutput, formatString, tapename);

       if (tapename[0] == '?')
          status = ADS_ERROR_IN_PATHTAPE;
    }

    if ((status == ADS_SUCCESS) && (DebugLvl > 1))
       elog (NOTICE, "ADS DEBUG[%d]: -- get tapename found name=%s", 
          getpid(), tapename);
    return(status);
}

/*************************************************************/
/* adsCreateTapename - Open pipe and call pathtape to create 6-char 
 *                     tapename from full pathname
 *
 * Input : *pathname  - Unix-style pathname
 *         *tapename  - 6-char ADS tapename
 *
 * Output : Returns ADS_SUCCESS if tapename found; otherwise ADS error code
 */
int
adsCreateTapename(char *pathname, char *tapename) 
{
    int status;
    char formatString[10];       /* For building format for sscanf */
    char cmdString[MAXCMD+1];    /* Command to send to pipe */
    char pipeOutput[MAXCMDOUT];  /* Output from piped command */
    int firstNewline;            /* Position of first newline in pipeOutput */
    char *s;                     /* Pointer to substring */


    strcpy(tapename, "?");
    sprintf(formatString, "%%%ds", TAPENAMELEN);  

    /*
       Open pipe and call pathtape to create 6-char tapename from full pathname
    */
    status = ADS_SUCCESS;
    sprintf(cmdString, "sysreq path create -u %s %s ", 
                        adsTapeOwner(), pathname);
    if (adsPipeCmd(cmdString, pipeOutput) != ADS_SUCCESS) {
       elog (NOTICE, "ADS ERROR[%d] executing pipe", getpid());
       status = ADS_ERROR_EXECUTING_PIPED_COMMAND;
    }

    /*
       Get tapename from pathtape output
       NOTE: this code depends on exact output from pathtape command;
             this could break if pathtape output changes.
    */

    if (status == ADS_SUCCESS) {

       elog (NOTICE, "ADS[%d]: %s", getpid(), pipeOutput);
       if (strncmp(pipeOutput, "pathtape:", 9) == 0) {

          /* Pathtape is reporting an error; determine what it means */

          if (strstr(pipeOutput, "already exists") != NULL) {
             /* Pathname to create already exists in pathtape; get tapename */
             firstNewline = strcspn(pipeOutput, "\n");
             s = &(pipeOutput[firstNewline]);
             sscanf(s, formatString, tapename);
          }

          else {
             /* Pathtape reports error, other than "already exists" */
             elog (NOTICE, 
                   "ADS ERROR[%d] adsCreateTapename: unable to create path",
                   getpid()); 
             status = ADS_ERROR_IN_PATHTAPE;
          }
       }

       /* No pathtape errors; get tapename from pathtape output */
       else {
          sscanf(pipeOutput, formatString, tapename);
       }
    }

    if ((status == ADS_SUCCESS) && (DebugLvl > 1))
       elog (NOTICE, "ADS DEBUG[%d]: adsCreateTapename created name=%s", 
          getpid(), tapename);
    return(status);
}

/*************************************************************/
/* adsReadTapeLabels -  Read initial labels from tape; includes      
 *                      a VOL label and HDR labels;
 *
 * Input : ts         - tape stream descriptor
 *         *tapename  - 6-char ADS tapename
 *
 * Output : Returns ADS_SUCCESS or ADS error code 
 */
int 
adsReadTapeLabels(int ts, char *tapename)
{
    /* Quick and dirty implementation, assuming standard labeled tapes
       with VOL1, HDR1, and HDR2 labels, each 80 chars 
    */
   
    char buffer[81];
    int nread;
    int status;
    int blen;

    if (DebugLvl > 3)
       elog (NOTICE, "ADS DEBUG[%d]: adsReadTapeLabels:", getpid());
    status = ADS_SUCCESS;

    /* Read VOL1 */
    if ((nread = tread(ts, buffer, 80)) <= 0 ) {
       elog(NOTICE, "ADS ERROR[%d]: -- error reading label VOL1 =%d", 
            getpid(), nread);
       status = ADS_ERROR_READING_TAPE_LABELS;
    }
    else {
       if (DebugLvl > 3) {
          buffer[nread] = '\0';
          rebtoasc(buffer, nread);
          elog(NOTICE,"ADS DEBUG[%d]: -- read label VOL1, nread=%d, label=\n%s",
               getpid(), nread, buffer);
       }
    }

    /* Read HDR1 */
    if ((nread = tread(ts, buffer, 80)) <= 0 ) {
       elog(NOTICE, "ADS ERROR[%d]: -- error reading label HDR1 =%d", 
            getpid(), nread);
       status = ADS_ERROR_READING_TAPE_LABELS;
    }
    else {
       if (DebugLvl > 3) {
          buffer[nread] = '\0';
          rebtoasc(buffer, nread);
          elog(NOTICE,"ADS DEBUG[%d]: -- read label HDR1, nread=%d, label=\n%s",
               getpid(), nread, buffer);
       }
    }

    /* Read HDR2 */
    if ((nread = tread(ts, buffer, 80)) <= 0 ) {
       elog(NOTICE, "ADS ERROR[%d]: -- error reading label HDR2 =%d", 
            getpid(), nread);
       status = ADS_ERROR_READING_TAPE_LABELS;
    }
    else {
       if (DebugLvl > 3) {
          buffer[nread] = '\0';
          rebtoasc(buffer, nread);
          elog(NOTICE,"ADS DEBUG[%d]: -- read label HDR2, nread=%d, label=\n%s",
               getpid(), nread, buffer);
       }
    }

    /* Read tape mark */
    nread=treadblk(ts, buffer, 1, &blen);
    if (DebugLvl > 3)
       elog(NOTICE, "ADS DEBUG[%d]: -- treadblk for TM nread=%d blen=%d", 
            getpid(), nread, blen);

    return (status); 
}

/*************************************************************/
/* adsWriteTapeLabels - Write initial labels to tape; includes      
 *                      a VOL label and HDR labels;
 *
 * Input : ts         - tape stream descriptor
 *         *tapename  - 6-char ADS tapename
 *
 * Output : Returns ADS_SUCCESS or ADS error code 
 */
int 
adsWriteTapeLabels(int ts, char *tapename)
/* FIX - need to pass in tapeMode to set permissions */
{
    struct RECLIST *r;
    int status;

    if (DebugLvl > 3)
       elog (NOTICE, "ADS DEBUG[%d]: adsWriteTapeLabels:", getpid());

    status = ADS_SUCCESS;

    /* Allocate  a record structure */
    if ((r = (struct RECLIST *) malloc(sizeof(struct RECLIST))) == NULL) {
       elog (NOTICE, 
            "ADS ERROR[%d]: -- adsWriteTapeLabels error in malloc for rs",
            getpid());
       status = ADS_ERROR_ALLOCATING_MEMORY;
    }
    else {
       if (DebugLvl > 3)
          elog (NOTICE, "ADS DEBUG[%d]: adsWriteTapeLabels: malloc r = 0x%x", 
                getpid(), r);
    }

    /* Initialize record structure */
    if (status == ADS_SUCCESS) {
       if (adsSetupRecord(r) != TAPE_OK) {
          elog (NOTICE, 
               "ADS ERROR[%d]: adsWriteTapeLabels error in record setup",
                getpid());
          status = ADS_ERROR_WRITING_TAPE_LABELS;
       }
    }

    /* Finish filling record structure fields */
    if (status == ADS_SUCCESS) {
       r->ts=ts;

       r->fmode=filewrite;          /* open tape for write */
       r->access=filewrite;         /* open tape for write */

       strncpy(r->finfo.f_dsn, tapename, TAPENAMELEN);  /* Data set name */
       strncpy(r->tapowner, adsTapeOwner(), LEN_TOWN);  /* Owner */
       strncpy(r->vsn, tapename, LEN_VID);              /* Volume serial name */
       r->vsn[LEN_VID]='\0';
       r->tapowner[LEN_TOWN]='\0';
       r->finfo.f_dsn[MAX_DSN]='\0';
    }

    /* first we have to create a SL label, so lets write VOL1 */
    if (status == ADS_SUCCESS) {
       if (wrvol(r) !=0) {
          /* count("failed to write VOL for %s %s",r->tapowner,r->vsn); */
          elog (NOTICE, "ADS ERROR[%d]: failed to write tape label VOL", 
                getpid());
          status = ADS_ERROR_WRITING_TAPE_LABELS;
       }
    }

    /* now lets write a HDR or two */
    if (status == ADS_SUCCESS) {
       if (wrhdr(r) !=0) {
          /* count("failed to write HDR for %s %s",r->tapowner,r->vsn); */
          elog (NOTICE, "ADS ERROR[%d]: failed to write tape label HDR", 
                getpid());
          status = ADS_ERROR_WRITING_TAPE_LABELS;
       }
    }
    
    if (status == ADS_SUCCESS) {
       adsStreams[ts].rStream = r;
    }
    else {
       if (r != NULL) {
          free(r);
          adsStreams[ts].rStream = NULL;
       }
    }

    return(status);
}

/*************************************************************/
/* adsInitAdsStreams - Initialize adsStreams entry for this tape stream  
 *
 * Input : ts - stream identifier
 *
 * Output : Returns ADS_SUCCESS or ADS error code 
 */
int 
adsInitAdsStreams(int ts)        
{
   int status;

    if (DebugLvl > 1)
       elog (NOTICE, "ADS DEBUG[%d]: adsInitAdsStreams, ts=%d", getpid(), ts);

    status = ADS_SUCCESS;

    if ((ts > 0) && (ts < MAX_TSTREAM)) {
       adsStreams[ts].rStream = NULL;
       adsStreams[ts].fileWriteSize = 0;
    }
    else {
       if (DebugLvl > 1)
          elog (NOTICE, "ADS DEBUG[%d]: adsInitAdsStreams, ts out of range",
                getpid());
       status = ADS_UNKNOWN_ERROR;
    }
    return(status);
}

/*************************************************************/
/* adsSetupRecord    - Fill fields of record structure used by librec   
 *
 * Input : *r - pointer to record structure to fill
 *
 * Output : Returns ADS_SUCCESS or ADS error code 
 */
int 
adsSetupRecord(struct RECLIST *r)
{
    struct FILEINFO *finfo; 
    struct tm *mytm;     /* Current time */
    time_t longtime;     /* Current time in secs since 1Jan70 */


    if (DebugLvl > 3)
       elog (NOTICE, "ADS DEBUG[%d]: adsSetupRecord", getpid());

    /* Setup the default values for the block */
    r->ts            = 0;
    r->rblen         = 0;
    r->inbytesleft   = 0;
    r->wrused        = 0;
    r->out_cur_bsize = RECBIGSIZE;
    r->eot           = FALSE;
    r->weot          = FALSE;
    r->eofdone       = FALSE;
    r->hdrdone       = FALSE;
    r->ineof         = FALSE;
    r->pastsilver    = FALSE;
    r->vsnignore     = FALSE;
    r->silverignore  = FALSE;
    r->filenum       = 1;    /* About to do the first file */
    r->fmode         = 0;
    r->rbuf          = NULL;
    r->access        = 0;
    r->labmode       = sl;
    r->tapowner[0]   = '\0';
    r->vsn[0]        = '\0';

    /* Set up finfo */
    finfo = &r->finfo;
    terrno=TAPE_OK;

    /* Set the expiry date and generation date to today */
    rexpdate(finfo,0);
    longtime=time(NULL);
    mytm=localtime(&longtime);
    finfo->f_exp.year=mytm->tm_year;
    finfo->f_exp.yday=mytm->tm_yday;
    /*count("REXPDATE: End with year=%d day=%d",
                        finfo->f_exp.year, finfo->f_exp.yday);*/


    finfo->f_gen.year=finfo->f_exp.year;
    finfo->f_gen.yday=finfo->f_exp.yday;

    /* Clear the data set name and the system name */
    finfo->f_dsn[0]='\0';
    finfo->f_sys[0]='\0';

    /* Set the volume number and sequence number to 1 */
    finfo->f_vnum=1;
    finfo->f_seq=1;

    /* Set the remaining fields to something useful - we hope */
    finfo->f_fmt         =RECu;
    finfo->f_fmt_force   =FALSE;
    finfo->f_reclen      =RECBIGSIZE;
    finfo->f_reclen_force=FALSE;

    finfo->f_bsize       =RECBIGSIZE;
    finfo->f_bsize_force =FALSE;

    /* Clear a few items */
    finfo->f_bnum=0;
    finfo->f_nbytes=0;
    finfo->f_numpad=0;
    finfo->f_fixpad=FALSE;

    return(TAPE_OK);
}


/*************************************************************/
/* LABEL.C -- Handle the labels on tape */
/*            Taken from globus/atlascopy, which was in turn taken 
/*            from librec
*/

/* WRITEVOL - Write the volume label header */
/* Return a TAPE_xxxx return code.          */
static int writevol(struct RECLIST *r)
{
   char label[81];

   if (DebugLvl > 3)
      elog (NOTICE, "ADS DEBUG[%d]: -- writevol", getpid());

   if (r->labmode==sl) {
      if (DebugLvl > 3)
         elog (NOTICE, "ADS DEBUG[%d]: ---- write Standard Label", getpid());
      /* IBM Standard Label tape */
      /* [#] taken from OS/VS tape labels GC26-3795-3 page 47 */
      sprintf(label,"VOL1%-6.6s%c%10s%10s%10s%-10.10s%29s",
                 r->vsn, 	          /* [3] Volume Serial Number */
                 (char)0x00,              /* [4] Reserved             */
                 "",                      /* [5] VTOC pointer (blank) */
                 "",                      /* [6] Reserved             */
                 "",                      /* [7] Reserved             */
                 r->tapowner,	          /* [8] Owner name code      */
                 ""                       /* [9] Reserved             */
             );
   } else {
      /* ANSI Label tape */
      /* [#] taken from OS/VS tape labels GC26-3795-3 page 85 */
      sprintf(label,"VOL1%-6.6s%c%20s%6s%-14.14s%28s1",
                 r->vsn,        /* [3] Volume Serial Number */
                 ' ',                     /* [4] Accessibility        */
                 "",                      /* [5] Reserved             */
                 "",                      /* [6] Reserved             */
                 r->tapowner,   /* [7] Owner name code      */
                 ""                       /* [8] Reserved             */
             );
   }

   put_label(r,label);
   if (terrno==TAPE_OK || terrno==TAPE_SILVER) {
      tctl(r->ts, opwtm, 1);
      if (DebugLvl > 3)
         elog (NOTICE, "ADS DEBUG[%d]: ---- write tape mark", getpid());
   }
   if (terrno==TAPE_OK || terrno==TAPE_SILVER){
      tctl(r->ts, opbsr, 1);
      if (DebugLvl > 3)
         elog (NOTICE, "ADS DEBUG[%d]: ---- back space one block", getpid());
   }
   if (terrno==TAPE_MARK) 
      terrno=TAPE_OK;

   return(terrno);
}

/* PUT_LABEL - Put a label onto the tape using the correct      */
/* character code (ascii or ebcdic). Input label MUST be ASCII. */
/* Return a TAPE_xxxx return code.                              */
static int put_label(struct RECLIST *r, char *label)
{
   int rc;

   if (DebugLvl > 3)
      elog (NOTICE, "ADS DEBUG[%d]: -- put_label", getpid());

   upcase(label);    /* Ensure it is upper case */

   if (DebugLvl > 3)
      elog (NOTICE, "ADS DEBUG[%d]: ---- writing label=\nADS DEBUG:%s|$",
            getpid(), label);

   if (r->labmode==sl) rasctoeb(label,80);

   /* Write the label */
   if ((rc=twrite(r->ts, label, 80)) != 80) {
      if (terrno==TAPE_SILVER) {
         r->pastsilver=TRUE;
         if (r->silverignore) terrno=TAPE_OK;
      }
   }

   return(terrno);
}

/* WRHDR - Write the tape header depending upon the tape type */
/* Returns a TAPE_xxxx return code. */
int wrhdr(struct RECLIST *r)
{

   if (DebugLvl > 3)
      elog (NOTICE, "ADS DEBUG[%d]: -- wrhdr", getpid());

   terrno=TAPE_OK;

   if (r->labmode==sl || r->labmode==al) {   /* Write the HDR records */
      if (writeskel(r,skelhdr)!=TAPE_OK) return(terrno);
      tctl(r->ts,opwtm,1);  /* Write a tape mark */
   }

   r->hdrdone=TRUE;
   return(terrno);
}

/* WRITESKEL - A skeleton to write EOF or HDR records on SL tapes */
/* Returns a TAPE_xxxx return code.                               */
static int writeskel(struct RECLIST *r, enum SKELTYPE stype)
/*  stype -  Type of skeleton "HDR", "EOF" or "EOV" */
{
   char label[81];
   char *fileid;              /* Name of file for label */
   char *sname;               /* Skeleton name          */
   int  myreclen;             /* Chosen file record len */
   int  myyear1,myyear2; char myind1,myind2;   /* Y2k compliance         */

   if (DebugLvl > 3)
      elog (NOTICE, "ADS DEBUG[%d]: -- writeskel", getpid());

   terrno=TAPE_OK;

   if (tsetblk(r->ts,80)!=TAPE_OK) 
      return(terrno);  /* Set the block size */

   /* Sort out the file name required on the tape */
   if (strlen(r->finfo.f_dsn)!=0) fileid=r->finfo.f_dsn;
   else                           fileid="stdin";

   /* Get the local host name */
   if (gethostname(r->finfo.f_sys,MAX_SYS)!=0) {
      strcpy(r->finfo.f_sys,"*Unknown*");
   }
   r->finfo.f_sys[MAX_SYS]='\0';  /* Just in case */

   /* Sort out the label characters from the type */
   switch (stype) {
      case skelhdr:   sname="HDR";   break;
      case skeleof:   sname="EOF";   break;
      case skeleov:   sname="EOV";   break;
      default:
         /* rmsg("ERROR in WRITESKEL - unknown stype=%d",stype);*/
         return(99);
   }

   /* In the label below, the number in square brackets before each */
   /* field definition comment indicates the field number given in  */
   /* the IBM OS/VS Tape Labels manual (GC26-3795-3) on page 50.    */
   /* For year two thousand compliance we understand that the two characters
      of the year field are to be prefaced with a blank for 19XX and a
      character zero for 20XX                                       */
   if(r->finfo.f_gen.year > 99) /*after 1999*/
   {
     myyear1 = r->finfo.f_gen.year - 100;/*this ceases to work in 2099
                                           so is just a temporary fix*/
     myind1 = '0';
   }
   else
   {
     myyear1 = r->finfo.f_gen.year;
     myind1 = ' ';
   }
 if(r->finfo.f_exp.year > 99) /*after 1999*/
   {
     myyear2 = r->finfo.f_exp.year - 100;/*this ceases to work in 2099
                                           so is just a temporary fix*/
     myind2 = '0';
   }
   else
   {
     myyear2 = r->finfo.f_exp.year;
     myind2 = ' ';
   }
   sprintf(label,
           "%-3s1%-17.17s%-6.6s%04d%04d%-6s%c%02d%03d%c%02d%03d%c%06d%-13.13s%-7s", 
	   sname,                           /* [  1] Label identifier        */
 	   fileid,                          /* [  3] Data set identifier     */
           r->vsn,                          /* [  4] Data set serial number  */
           r->finfo.f_vnum,                 /* [  5] Volume sequence number  */
           r->filenum,                      /* [  6] DS/File sequence number */
           "",                              /* [7/8] Generation information  */
           myind1,
           myyear1,                         /* [  9] Creation date           */
           r->finfo.f_gen.yday,
           myind2,
           myyear2,                         /* [ 10] Expiry date             */
           r->finfo.f_exp.yday,
           (r->labmode==al)?' ':'0',        /* [ 11] Accessibility           */
           r->finfo.f_bnum,                 /* [ 12] Block count             */
           r->finfo.f_sys,                  /* [ 13] System code             */
           "");                             /* [ 14] Reserved                */

   put_label(r,label);
   if (terrno!=TAPE_OK && terrno!=TAPE_SILVER) return(terrno);

   if (r->finfo.f_fmt==RECu) myreclen=0;
   else                      myreclen=r->finfo.f_reclen;

   sprintf(label,"%-3s2%c%05d%05d%-23s%c%-41s",
           sname,                           /* [  1] Label identifier        */
           get_rform(r),                   /* [  3] Record Format F/V/D/U   */
           r->finfo.f_bsize,                /* [  4] Block Length            */
           myreclen,                        /* [  5] Record Length           */
           "",
           get_battr(r),                   /* [ 12] Block Attributes        */
           "");

   put_label(r,label);
   return(terrno);
}

/* GETDATE - Fill in the year and day values from a label date field */
/* Call this with "dbuf" pointing to the start of a label date field */
static void mygetdate(char *dbuf, int *outyear, int *outday)
/* dbuf     - Label date buffer */
/* outyear  - Output year value */
/* outday   - Output day value  */
{
   char buf[6];

   if (DebugLvl > 3)
      elog (NOTICE, "ADS DEBUG[%d]: -- mygetdate", getpid());

   strncpy(buf,dbuf,6);
   buf[5]='\0';

   sscanf(buf,"%2d%3d",outyear,outday);
   /*fprintf(stderr,"GETDATE: Str=%s Date=%d/%d",
                        buf,*outyear,*outday);*/
}

/* GET_RFORM - Return a character that represents the record format. */
/*             We use F (Fixed); V (Variable); U (Undefined) and     */
/*             D (Decimal zoned).                                    */
static char get_rform(struct RECLIST *r)
{
   if (DebugLvl > 3)
      elog (NOTICE, "ADS DEBUG[%d]: -- get_rform", getpid());

   switch (r->finfo.f_fmt) {
      case RECf:
      case RECfb:   return('F');

      case RECv:
      case RECvb:
      case RECvs:
      case RECvbs:  return('V');

      case RECu:    return('U');

      case RECd:
      case RECdb:   return('D');

      case RECcos:  return('C');

      default:
         /*rmsg("Internal Error in GET_RFORM/LABEL.C");*/
         return('*');
   }
}

/* GET_BATTR - Get the block attributes and return as a character. */
/*             We use 'B' (Blocked); 'S' (Spanned); 'R' (both) and */
/*             ' ' space (neither).                                */
static char get_battr(struct RECLIST *r)
{
   if (DebugLvl > 3)
      elog (NOTICE, "ADS DEBUG[%d]: -- get_battr", getpid());

   switch (r->finfo.f_fmt) {
      case RECcos:
      case RECf:
      case RECv:
      case RECu:
      case RECd:   return(' ');   /* One record per block */

      case RECfb:
      case RECvb:
      case RECdb:  return('B');   /* Many records per block */

      case RECvbs: return('R');   /* Blocked and Spanned */

      case RECvs:  return('S');   /* Spanned */

      default:
         /*rmsg("Internal Error in GET_BATTR/LABEL.C");*/
         return('*');
   }
}

/* R_UPCASE - Convert a string to upper case */
static void upcase(char *s)
{
   if (DebugLvl > 3)
      elog (NOTICE, "ADS DEBUG[%d]: -- upcase", getpid());

   for (; *s!='\0'; s++) *s=toupper(*s);
}


/* R_WRVOL - Process the VOL labels on the tape in write mode.   */
/* Return a TAPE_xxxx return code.                               */
/* If the tape is in UPDATE mode, then we check the vol labels   */
/* (if tape is appropriate type) and return. If the tape is in   */
/* WRITE mode, then we write our own VOL labels as this is a     */
/* fast mode of access where the old tape data is not available  */
/* to us under VTP. This is ONLY called is a WRITE/UPDATE mode.  */
int wrvol(struct RECLIST *r)
{
   int i;

   if (DebugLvl > 3) 
      elog (NOTICE, "ADS DEBUG[%d]: -- wrvol, r->access=%d r->labmode=%d", 
            getpid(), r->access, r->labmode);

   terrno=TAPE_OK;
   if (tsetblk(r->ts, 80) != TAPE_OK) {
      elog (NOTICE, "ADS ERROR[%d]: wrvol error calling tsetblk, returned=%d", 
            getpid(), terrno);
      return(terrno);
   }

   switch (r->access) {
      case tapewrite:
         switch (r->labmode) {
            case blp:   case nl:
               return(terrno);        /* No need to do anything */

            case al:    case sl:
               return(writevol(r));  /* Write new VOL labels */

            default:
               elog (NOTICE, "ADS ERROR[%d] in wrvol, unknown labmode=%d",
                     getpid(), r->labmode); 
               return(99);
         }
         break;

      case tapeupdate:
         switch (r->labmode) {
            case blp:
               return(terrno);        /* No need to do anything */

            case al:    case sl:   case nl:   case labany:
               /* Handle case where tape really is empty */
               if (chekvol(r)==TAPE_AFTER) {
                  if (r->labmode==labany) 
                     r->labmode=sl;
                  writevol(r);
               }
               return(terrno);

            default:
               elog (NOTICE, 
                     "ADS ERROR[%d] in wrvol, tapeupdate unknown labmode=%d",
                     getpid(), r->labmode);
               return(99);
         }
         break;

      default:
         elog (NOTICE, "ADS ERROR[%d] in wrvol, unknown access=%d",  
                getpid(), r->access);
         return(99);
   }

   return(terrno);   /* No problem! */
}

/* WREOF - Do the End Of File processing depending upon the tape type */
int wreof(struct RECLIST *r)
{
   int status;

   if (DebugLvl > 3)
      elog (NOTICE, "ADS DEBUG[%d]: -- wreof", getpid());

   status = do_wrend(r, skeleof);
   return(status);
}

/* WREOV - Do the End Of Volume processing */
int wreov(struct RECLIST *r)
{
   if (DebugLvl > 3)
      elog (NOTICE, "ADS DEBUG[%d]: -- wreov", getpid());

   return(do_wrend(r,skeleov));
}


/* do_wrend - Write the end of file/volume labels as required */
static int do_wrend(struct RECLIST *r, enum SKELTYPE stype)
{

   if (DebugLvl > 3)
      elog (NOTICE, "ADS DEBUG[%d]: -- do_wrend, stype=%d", getpid(), stype);

   terrno=TAPE_OK;

   if (r->labmode==sl || r->labmode==al) {
      tctl(r->ts,opwtm,1);
      if (terrno!=TAPE_OK && terrno!=TAPE_SILVER) return(terrno);

      writeskel(r,stype);
      if (terrno!=TAPE_OK && terrno!=TAPE_SILVER) return(terrno);
   }

   tctl(r->ts,opwtm,2);
   if (terrno!=TAPE_OK && terrno!=TAPE_SILVER) return(terrno);

   tctl(r->ts,opbsf,1);
   return(terrno);
}

/* CHEKVOL - Check the label type and that it is correct */
static int chekvol(struct RECLIST *r)
{
   char label[81];        /* Local copy of the label read from tape */
   char myvsn[LEN_VSN+1]; /* Copy of the space-removed VSN          */
   enum LABEL mylab;      /* Label type that we found               */
   int vsnstart,ownstart,ownlen;

   mylab=nl;   /* Start with a good default */
   if (get_label(r,label)) {
      if (strncmp(label,"VOL1",4)==0) {           /* Got an ANSI tape */
         mylab=al;
      } else {
         rebtoasc(label,80);
         if (strncmp(label,"VOL1",4)==0) {        /* Got an EBCDIC tape */
            mylab=sl;
         } else {
            mylab=nl;
         }
      }

   }
   if (terrno!=TAPE_OK) return(terrno);   /* Some error occured */
   if (R_debug) rmsg("CHEKVOL: Found a %s tape.",rlab2char(mylab));
   if (mylab==nl) tctl(r->ts,oprew,1);

   /* Now check what we've got against what the user asked for. */
   /* If they haven't specified a type, then we accept it.      */
   if (r->labmode==labany) {
      r->labmode=mylab;
   } else {
      if (r->labmode!=mylab) {   /* Got wrong type! */
         if (R_debug) rmsg("CHEKVOL: Requested %s tape, got %s.",
                 rlab2char(r->labmode),rlab2char(mylab));
         r->labmode=mylab;  /* So user knows what was there */
         return(terrno=REC_BADLABEL);
      }
   }

   /* An NL tape needs no further work */
   if (mylab==nl) return(terrno);

   if (mylab==al) {   /* ANSII tape */
      vsnstart=AL_VOL1_VSN;
      ownstart=AL_VOL1_OWN;
      ownlen  =AL_OWN_LEN;
   } else {           /* IBM label tape */
      vsnstart=SL_VOL1_VSN;

      /* Due to a bodge with the earlier label writing from the */
      /* very first versions of the tape command, we always put */
      /* the owner field in the AL position - even for SL tapes */
      /* So we now read back both versions.         TK 7/12/93  */
      if (label[AL_VOL1_OWN]!=' ') {
         if (R_debug) rmsg("CHEKVOL: Using OLD AL owner position");
         ownstart=AL_VOL1_OWN;
         ownlen  =AL_OWN_LEN;
      } else {
         ownstart=SL_VOL1_OWN;
         ownlen  =SL_OWN_LEN;
      }
   }

   strncpy(myvsn,&(label[vsnstart]),LEN_VSN);
   myvsn[LEN_VSN]='\0';
   strncpy(r->tapowner,&(label[ownstart]),ownlen);
   r->tapowner[ownlen]='\0';
   despace(r->tapowner);
   despace(myvsn);
   if (R_debug) rmsg("CHEKVOL: De-spaced VSN is >>>%s<<<",myvsn);

   if (strcmp(myvsn,r->vsn)!=0) {
      if (R_debug) rmsg("CHEKVOL: Incorrect VSN mounted (got %s).",
                           myvsn);
      if (r->vsnignore) {
         rmsg("Using tape VSN=%s rather than requested VSN=%s",myvsn,r->vsn);
         strcpy(r->vsn,myvsn);  /* Remember the real VSN */
      } else {
         return(terrno=REC_BADVSN);
      }
   }

   return(TAPE_OK);
}

/* GET_LABEL - Get a label image into a buffer - no translation. */
/* Return TRUE or FALSE.                                         */
static int get_label(struct RECLIST *r, char *label)
{
   char *dptr;         /* Pointer whilst reading data in  */
   int  nread;         /* Control the read in process     */
   int blksize;        /* Size of first block read        */

   if (R_debug) rmsg("GET_LABEL: Called...");
   dptr=label;

   if ((nread=treadblk(r->ts,dptr,80,(unsigned int *)&blksize))<0) {
      if (R_debug) prerror("GET_LABEL");
      return(FALSE);
   }

   if (blksize!=80) {  /* This is not a label */
      if (R_debug) rmsg("GET_LABEL: Not a label (length=%d)",blksize);
      blksize-=nread;
      while (blksize>0) { /* Waste the data already in the pipeline */
         if ((nread=tread(r->ts,dptr,80))<0) {
            if (R_debug) prerror("GET_LABEL");
            return(FALSE);
         }
         blksize-=nread;
      }
      return(FALSE);
   }

   blksize-=nread;
   dptr   +=nread;

   while (blksize>0) {
      if ((nread=tread(r->ts,dptr,blksize))<0) return(FALSE);
      blksize-=nread;
      dptr   +=nread;
      if (R_debug) rmsg("GET_LABEL: %d bytes left of 80.",blksize);
   }

   label[80]='\0';

   return(TRUE);
}

/* DESPACE - Remove trailing and leading spaces from a string */
static void despace(char *str)
{
   char *from, *to;   /* Pointers for work */


   if (*str=='\0') {  /* Empty string */
      return;
   }

   if (*str==' ') {  /* We have leading blanks */
      for (from=str; *from==' '; from++)
         ;

      if (*from=='\0') {  /* Empty string - only spaces! */
         *str='\0';
         return;
      }

      for (to=str; ((*to)=(*from))!='\0'; to++, from++)
         ;
   } else {          /* No leading blanks, get "to" pointing to end */
      for (to=str; *to!='\0'; to++)
         ;
   }

   to--;   /* "to" points to last character in de-pre'd string */

   if (*to==' ') {   /* We have a trailing space here */
      for (; *to==' '; to--)
         ;
      *(++to)='\0';
   }

}

char *adsTapeOwner(void) {
/* Return ADS tape owner name of 8 chars, in the form
      SRBXXXXX
   where XXXXX is either:
      1) the first 5 alphanumeric chars of the SRB domain name, if ADSOWNER=1
      2) the first 5 alphanumeric chars of the SRB user name, if ADSOWNER=2.
   Any other value of ADSOWNERMAP defaults to domain name.
*/
   static char tapeOwner[TAPEOWNER_LEN+1];  
   char *owner;
   char *client;
   int i,k;

   owner = tapeOwner;
   (void) sprintf(owner, "SRB");
   k = strlen("SRB");

   client = ClientUser->domainName;
#if ADSOWNERMAP == 1
   client = ClientUser->domainName;
#elif ADSOWNERMAP == 2
   client = ClientUser->userName;
#endif

   for (i=0; (i < strlen(client)) && (k < TAPEOWNER_LEN); i++) {
      if (isalnum(client[i]))
         owner[k++] = client[i];
   }
   owner[k] = '\0';

   if (DebugLvl > 1)
      elog (NOTICE, "ADS DEBUG[%d]: client=%s, tapeowner=%s", 
         getpid(),client,tapeOwner);
   return(tapeOwner);
}
