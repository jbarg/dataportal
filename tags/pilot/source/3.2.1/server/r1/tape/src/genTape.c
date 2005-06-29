/* Driver routines for the generic tape - GENERIC_TAPE_DRV */

#include "genTape.h"

/* genInitTape - Initialize a tape. Write the VOL label and two EOF mark.
 * The tape will then be rewind and unloaded. 
 */

int
genInitTape (tapeDev_t *tapeDev)
{
    int retVal;
    vol1Label_t myVol1Lab;
    struct stop myStop;
    
    /* open the tape */

    tapeDev->fd = openRewindTape (tapeDev, O_RDWR);

    if (tapeDev->fd < 0) 
	return tapeDev->fd;

    retVal = setVol1Struct (&myVol1Lab, tapeDev->mountedCart->tapeNum);

    if (retVal < 0) {
	genCloseTape (tapeDev, 0);
	return retVal;
    }

    /* Write the volume label */

    retVal = write (tapeDev->fd, &myVol1Lab, sizeof (vol1Label_t));

    if (retVal != sizeof (vol1Label_t)) {
	elog (NOTICE, 
	  "genInitTape: Error writing VOL1, wrote %d bytes, errno %d",
	   retVal, errno);
	genCloseTape (tapeDev, 0);
        retVal = unixErrConv (errno);
        return retVal;
    }

    /* put two eof mark */

    myStop.st_op = STWEOF;
    myStop.st_count = 2;
    retVal = ioctl(tapeDev->fd, STIOCTOP, &myStop);

    if (retVal < 0) {
        printf (NOTICE, "genInitTape: Error writing EOF mark, errno %d",
         errno);
        genCloseTape (tapeDev, 0);
        retVal = unixErrConv (errno);
        return retVal;
    }
	
    genCloseTape (tapeDev, 0);
    /* Update the position */

    memset (tapeDev->position.absPos.address, 0, ABS_POS_SIZE);
    tapeDev->position.filePos.fileSeqNo = -1;
    tapeDev->position.filePos.offset = 0;

    return (0);
}

/*
 * genOpenTape - Open a tape and read and verify the volume label. 
 * If sucessful, the tape position should be at the end of the vol label 
 * (fileSeqNo =  0, offset = sizeof (vol1Label_t)) 
 */

int
genOpenTape (tapeDev_t *tapeDev, int flag)
{
    int retVal;
   
    /* open the tape */

    if ((retVal = openRewindTape (tapeDev, flag)) < 0)
        return retVal;

    retVal = readVerifyVol1 (tapeDev);

    if (retVal < 0) {
        genCloseTape (tapeDev, 0);
        return retVal;
    }

    /* The tape position now is at the end of the vol label 
     * (fileSeqNo =  0, offset = sizeof (vol1Label_t)) */


    return (0);
}

/* genSeekTape - Seek the tape to position indicated by destPos.
 * Current we only seek to file boundary and we don't seek into the file.
 * Therefore, filePos.offset is meaningless.
 */
 
int
genSeekTape (tapeDev_t *tapeDev, tapePos_t *destPos)
{
    filePos_t *destFilePos, *devFilePos;
    absPos_t  *destAbsPos, *devAbsPos;
    struct stop myStop;
    int retVal;
    int numLeft, numToSeek;

    destFilePos = &destPos->filePos;
    destAbsPos = &destPos->absPos;

    devFilePos = &tapeDev->position.filePos;
    devAbsPos = &tapeDev->position.absPos;

    if (tapeDev->fd < 0) {
	elog (NOTICE, "genSeekTape: Tape device %s not opened, fd = %d",
	    tapeDev->devPath, tapeDev->fd);
	return (TAPE_FILE_NOT_OPENED);
    }

    if (destFilePos->fileSeqNo < 1) {	/* fileSeqNo==0 =>vol label */
	elog (NOTICE,
	 "genSeekTape: Input filePos less than 1, fileSeqNo=%d\n",
	  destFilePos->fileSeqNo);
	return TAPE_FILE_SEQ_NUM_ERROR;
    }

    /* The generic tape does not seek to absolute address. Will have to do 
     * by filePos.
     */

    if (devFilePos->fileSeqNo < 0) {

	/* We are at the beginning of the tape ? Rewind it to be sure. */
 
	myStop.st_op = STREW;
	myStop.st_count = 1;
	retVal = ioctl (tapeDev->fd, STIOCTOP, &myStop);

	if (retVal < 0) {
            elog (NOTICE, "genSeekTape: Unable to rewind %s, errno=%d",
              tapeDev->devPath, errno);
	    retVal = unixErrConv (errno);
            return retVal;
        }

        retVal = readVerifyVol1 (tapeDev);

        if (retVal < 0) {
            return retVal;
        }

        /* The tape position now is at the end of the vol label
         * (fileSeqNo =  0, offset = sizeof (vol1Label_t)) */
    }

    if (destFilePos->fileSeqNo > devFilePos->fileSeqNo) {
	/* seek forward */
	numLeft = destFilePos->fileSeqNo - devFilePos->fileSeqNo;
	while (numLeft > 0) {

	    if (numLeft > MAX_FILE_MARK_TO_SEEK)
		numToSeek = MAX_FILE_MARK_TO_SEEK;
	    else
		numToSeek = numLeft;

            myStop.st_op = STFSF;
            myStop.st_count = numToSeek;
  
            retVal = ioctl (tapeDev->fd, STIOCTOP, &myStop);

            if (retVal < 0) {
                elog (NOTICE,
		  "genSeekTape: Unable to tape seek STFSF %d, errno=%d",
                  numToSeek, errno);
		devFilePos->fileSeqNo = -1;	/* don't know where we are */
		retVal = unixErrConv (errno);
                return retVal;
            }
	    numLeft -= numToSeek;
	}
    } else if (destFilePos->fileSeqNo < devFilePos->fileSeqNo ||
     (destFilePos->fileSeqNo == devFilePos->fileSeqNo &&
      devFilePos->offset > 0)) {
	/* Seek backward. Must back up one extra and then one stop 
	 * forward to position at the beginning of the file mark 
	 */

	numLeft = devFilePos->fileSeqNo - destFilePos->fileSeqNo + 1;
	while (numLeft > 0) {
            if (numLeft > MAX_FILE_MARK_TO_SEEK)
                numToSeek = MAX_FILE_MARK_TO_SEEK;
            else
                numToSeek = numLeft;

            myStop.st_op = STRSF;
            myStop.st_count = numToSeek;
 
            retVal = ioctl (tapeDev->fd, STIOCTOP, &myStop);

            if (retVal < 0) {
                elog (NOTICE, 
		  "genSeekTape: Unable to tape seek STRSF %d, errno=%d",
                  numToSeek, errno);
		devFilePos->fileSeqNo = -1;	/* don't know where we are */
		retVal = unixErrConv (errno);
                return retVal;
            }
            numLeft -= numToSeek;
        }

	/* Now, one step forward */

        myStop.st_op = STFSF;
        myStop.st_count = 1;
 
        retVal = ioctl (tapeDev->fd, STIOCTOP, &myStop);

        if (retVal < 0) {
            elog (NOTICE,
	      "genSeekTape: Unable to tape seek STFSF %d, errno=%d\n",
              1, errno);
            devFilePos->fileSeqNo = -1;     /* don't know where we are */
	    retVal = unixErrConv (errno);
            return retVal;
        }
    }

    /* So far so good. Update the device position */

    devFilePos->fileSeqNo = destFilePos->fileSeqNo;
    devFilePos->offset = 0;

    return (0);
}
	
int
openRewindTape (tapeDev_t *tapeDev, int flag)
{
    struct stop myStop;
    int retVal;
    int openRetry = 0;

    /* open the tape */

    while (openRetry < TAPE_OPEN_RETRY) {
        tapeDev->fd = open (tapeDev->devPath, flag);
        if (tapeDev->fd >= 0)
            break;

        if (errno == ENOENT || errno == ENXIO) {
            elog (NOTICE, "openRewindTape: Device %s does not exist",
             tapeDev->devPath);
	    retVal = unixErrConv (errno);
            return retVal;
        } else if (errno == EACCES || errno == EPERM) {
            elog (NOTICE, 
	      "openRewindTape: No access to device %s, errno = %d",
              tapeDev->devPath, errno);
            retVal = unixErrConv (errno);
            return retVal;
        }

        openRetry ++;
        srbThreadSleep (10, 0);
    }

    if (tapeDev->fd < 0) {
        elog (NOTICE,
	  "openRewindTape: Unable to open %s, errno=%d",
          tapeDev->devPath, errno);
        retVal = unixErrConv (errno);
        return retVal;
    }
    /* rewind the tape */

    myStop.st_op = STREW;
    myStop.st_count = 1;
    retVal = ioctl (tapeDev->fd, STIOCTOP, &myStop);
    if (retVal < 0) {
        genCloseTape (tapeDev, 0);
        elog (NOTICE,
	  "openRewindTape: Unable to rewind %s, errno=%d",
          tapeDev->devPath, errno);
        retVal = unixErrConv (errno);
        return retVal;
    }

    /* set the position */

    memset (tapeDev->position.absPos.address, 0, ABS_POS_SIZE);
    tapeDev->position.filePos.fileSeqNo = 0;
    tapeDev->position.filePos.offset = 0;
    tapeDev->state = ST_TLIB_MOUNTED;
    if (flag & O_WRONLY || flag & O_RDWR)
        tapeDev->state = tapeDev->state | ST_WRITE_MODE | ST_OPENED;
    else if (flag & O_RDWR)
        tapeDev->state = tapeDev->state | ST_WRITE_MODE | ST_READ_MODE |
         ST_OPENED;
    else
        tapeDev->state = tapeDev->state | ST_READ_MODE | ST_OPENED;

    return tapeDev->fd;
}

int
genCloseTape (tapeDev_t *tapeDev, int unloadFlag)
{
    struct stop myStop;
    int retVal;


    /* XXXXXXX take me out ASAP. unloadFlag forced to 0 because don't want
     * to run downstair to re-mount tape  unloadFlag = 0; */
    if (unloadFlag > 0) {
        /* rewind and unload the tape */
        myStop.st_op = STOFFL;
        myStop.st_count = 1;
        retVal = ioctl (tapeDev->fd, STIOCTOP, &myStop);
        if (retVal < 0) {
            elog (NOTICE, "genCloseTape: Unable to unload tape, errno=%d",
              errno);
    	    tapeDev->state = ST_MOUNTED | ST_TLIB_MOUNTED;
            retVal = unixErrConv (errno);
        } else {
            tapeDev->state = ST_UNLOADED | ST_TLIB_MOUNTED;
        }
    } else {
        /* just rewind the tape */
        myStop.st_op = STREW;
        myStop.st_count = 1;
        retVal = ioctl (tapeDev->fd, STIOCTOP, &myStop);
        if (retVal < 0) {
            elog (NOTICE, "genCloseTape: Unable to rewind tape, errno=%d",
              errno);
            retVal = unixErrConv (errno);
        } else {
    	    tapeDev->state = ST_MOUNTED | ST_REWINDED | ST_TLIB_MOUNTED;
        }
    }

    close (tapeDev->fd);
    tapeDev->fd = -1;

    /* set the position */
    memset (tapeDev->position.absPos.address, 0, ABS_POS_SIZE);
    tapeDev->position.filePos.fileSeqNo = -1;
    tapeDev->position.filePos.offset = 0;


    return retVal;
}

int genWriteEOF (tapeDev_t *tapeDev)
{
    struct stop myStop;
    int retVal;

    if (tapeDev->position.filePos.fileSeqNo < 1) {
        /* must be positioned after the first EOF mark */
        elog (NOTICE,
	  "genWriteEOF: Tape positioned before the first EOF mark.");
        retVal = unixErrConv (errno);
        return retVal;
    }

    if ((tapeDev->state & ST_EOT)) {
        elog (NOTICE, "genWriteEOF: Trying to put tape mark beyond EOT.");
        retVal = unixErrConv (errno);
        return retVal;
    }

    myStop.st_op = STWEOF;
    myStop.st_count = 1;
    retVal = ioctl (tapeDev->fd, STIOCTOP, &myStop);

    if (retVal < 0) {
        elog (NOTICE, "genWriteEOF: Unable to write EOF, errno=%d.", errno);
        retVal = unixErrConv (errno);
        return retVal;
    }

    /* Update the file position */

    tapeDev->position.filePos.fileSeqNo ++;
    tapeDev->position.filePos.offset = 0;

    return (retVal);
}

