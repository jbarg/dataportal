/* general routines for the tape driver */

#include "srbTapeMisc.h"

int
setVol1Struct (vol1Label_t *myVol1Lab, char *tapeNum)
{
    if (strlen (tapeNum) > TAPE_NUM_LEN) {
	elog (NOTICE, "setVol1Struct: Length of tapeNum %s greater than %d",
	 tapeNum, TAPE_NUM_LEN);
	return TAPE_NUM_ERROR;
    }

    memset(myVol1Lab, 0, sizeof(vol1Label_t)); 

    sprintf (myVol1Lab->labelID, "VOL");
    sprintf (myVol1Lab->labelNo, "1");
    sprintf (myVol1Lab->tapeNum, "%-s", tapeNum);
    sprintf (myVol1Lab->accessibility, " ");
    sprintf (myVol1Lab->ownerID, "SDSC_SRB");
    sprintf (myVol1Lab->labelStandardLevel, "3");

    return (0);
}

int
setHdr1Struct (hdr1Label_t *myHdr1Label, char *tapeNum, char *fileID,  
int fileSegmNo, int fileSeqNo)
{
    int myLen;
    int retVal;

    if (strlen (tapeNum) > TAPE_NUM_LEN) {
        elog (NOTICE, "setHdr1Struct: Length of tapeNum %s greater than %d",
         tapeNum, TAPE_NUM_LEN);
        return TAPE_NUM_ERROR;
    }

    if ((myLen = strlen (fileID)) > FILE_ID_LEN) {
	fileID += (myLen - FILE_ID_LEN + 1);
    }

    if (fileSegmNo > MAX_FILE_SEGM_NO || fileSegmNo < 0) {
        elog (NOTICE, 
	  "setHdr1Struct: FileSegmNo %d greater than %d or negative",
           fileSegmNo, MAX_FILE_SEGM_NO);
        return TAPE_FILE_SEG_NUM_ERROR;
    }

    if (fileSeqNo > MAX_FILE_SEQ_NO || fileSeqNo < 0) {
        elog (NOTICE,
	  "setHdr1Struct: FileSeqNo %d greater than %d or negative",
          fileSeqNo, MAX_FILE_SEQ_NO);
        return TAPE_FILE_SEQ_NUM_ERROR;
    }

    memset (myHdr1Label, 0, sizeof(hdr1Label_t));

    sprintf(myHdr1Label->labelID, "HDR");
    sprintf(myHdr1Label->labelNo, "1");
    sprintf(myHdr1Label->fileName,"%-s", fileID);
    sprintf(myHdr1Label->fileSegmNo, "%06d", fileSegmNo);
    sprintf(myHdr1Label->fileSeqNo, "%08d", fileSeqNo);
    sprintf(myHdr1Label->genNo, "0001");
    sprintf(myHdr1Label->versNo, "00");
    if ((retVal = setTapeDate (myHdr1Label->creationDate)) < 0)
	return retVal;

    sprintf(myHdr1Label->expirationDate,TAPE_EXPIR_DATE);
    myHdr1Label->accessibility[0] = ' ';
    sprintf(myHdr1Label->blockCount, "000000");
    sprintf(myHdr1Label->systemCode,"SRB");
    sprintf(myHdr1Label->tapeNum,"%-s", tapeNum);

    return 0;
}

int
setHdr2Struct (hdr2Label_t *myHdr2Label, int blockSz)
{
    if (blockSz > MAX_BLOCK_SZ || blockSz < 0) {
        elog (NOTICE,
	  "setHdr2Struct: blockSz %d greater than %d or negative",
         blockSz, MAX_BLOCK_SZ);
        return TAPE_BLOCK_SZ_ERROR;
    }

    memset (myHdr2Label, 0, sizeof(hdr2Label_t));

    sprintf(myHdr2Label->labelID, "HDR");
    sprintf(myHdr2Label->labelNo, "2");
    sprintf(myHdr2Label->recordFormat, "F");
    sprintf(myHdr2Label->blockSz, "%010d", blockSz);
    sprintf(myHdr2Label->bufferOffset, "00");

    return(0);
}

/* setTapeDate - set the 6 char date. Format = cyyddd, where 
 *	c = century since 1900
 *	yy = year in the century
 *	ddd = days in the year
 *
 */
 
 
int 
setTapeDate (char *myDate)
{
    time_t          myTime;
    struct tm       *myTm;
    char* tmpPtr;
    int retVal;

    myTime = time (0);
    myTm = localtime (&myTime);
    
    if(myTm == NULL) {
        elog (NOTICE, "setTapeDate: Problem calling localtime, errno=%d",
         errno);
        retVal = unixErrConv (errno);
        return retVal;
    }

    /* tm_year = year since 1900 */

    tmpPtr = myDate;
    tmpPtr[0] = '0' + ((myTm->tm_year - 100) / 100);
    tmpPtr += 1;
    sprintf(tmpPtr, "%02d",myTm->tm_year % 100);
    tmpPtr += 2;
    sprintf(tmpPtr,"%03d",myTm->tm_yday + 1); /* have to add 1 since tm_yday
						 * is (0, 365) */

    return (0);
}
						
int readVerifyVol1 (tapeDev_t *tapeDev)
{
    int retVal;
    vol1Label_t myVol1Lab;

    memset (tapeDev->position.absPos.address, 0, ABS_POS_SIZE);

    retVal = read(tapeDev->fd, (char *) &myVol1Lab, sizeof(vol1Label_t));

    if (retVal != sizeof(vol1Label_t)) {
	/* Could not read the label. Either the lable is not there or
	 * the label has less than 80 char 
	 */
	elog (NOTICE, 
	 "readVerifyVol1: Unable to read VOL1, read %d of %d, errno=%d",
	    retVal, sizeof (vol1Label_t), errno);

	tapeDev->position.filePos.fileSeqNo = -1;
	tapeDev->position.filePos.offset = 0;
        retVal = unixErrConv (errno);
        return retVal;
    }

    /* Verify the header */

    
    if (memcmp(myVol1Lab.labelID, "VOL", 3) != 0 ||
     myVol1Lab.labelNo[0] != '1') {
        elog (NOTICE,
	  "readVerifyHdr1: The vol label does not start with VOL1");
        tapeDev->position.filePos.fileSeqNo = -1;
        tapeDev->position.filePos.offset = 0;
        return TAPE_VOL_LABEL_ERROR;
    }
     
    if (strncmp (tapeDev->mountedCart->tapeNum, myVol1Lab.tapeNum, 
     TAPE_NUM_LEN) != 0) {
	elog (NOTICE,
	  "readVerifyHdr1: Wrong tape mounted: requested=%s,mounted=%s",
	 tapeDev->mountedCart->tapeNum, myVol1Lab.tapeNum);
        tapeDev->position.filePos.fileSeqNo = -1;
        tapeDev->position.filePos.offset = 0;
        return TAPE_VOL_LABEL_ERROR;
    }

    /* The tape position now is at the end of the vol label (file 0) */

    tapeDev->position.filePos.fileSeqNo = 0;
    tapeDev->position.filePos.offset = sizeof (vol1Label_t);

    return (0);
}

int
readVerifyHdr (tapeDev_t *tapeDev)
{
    int retVal;
    hdr1Label_t myHdr1Label;
    hdr2Label_t myHdr2Label;
    char tmpStr[FILE_ID_LEN];

    retVal = read(tapeDev->fd, (char *) &myHdr1Label, sizeof(hdr1Label_t));

    if (retVal != sizeof(hdr1Label_t)) {
        /* Could not read the HDR1. Either the header is not there or
         * the header has less than 80 char
         */
	if (retVal > 0 || (tapeDev->state & ST_WRITE_MODE) == 0)
            elog (NOTICE, 
	     "readVerifyHdr: Unable to read HDR1, read %d of %d, errno=%d",
              retVal, sizeof (vol1Label_t), errno);
	if (retVal == 0)
	    tapeDev->state |= ST_EOT;
        return TAPE_HEADER_LABEL_ERROR;
    }

    /* Verify HDR1 */

    if (memcmp(myHdr1Label.labelID, "HDR", 3) != 0 ||
     myHdr1Label.labelNo[0] != '1') {
        elog (NOTICE,
	   "readVerifyHdr: The HDR label does not start with HDR1");
        return TAPE_HEADER_LABEL_ERROR;
    }

    /* don't compare fileSegmNo yet. We are not currently implementing
     * segmented file yet */

    /* Check the fileSegmNo */

    memcpy (tmpStr, myHdr1Label.fileSeqNo, FILE_SEQ_LEN);
    tmpStr[FILE_SEQ_LEN] = '\0';

    if (atoi (tmpStr) != tapeDev->position.filePos.fileSeqNo) {
        elog (NOTICE,
	  "readVerifyHdr: fileSeqNo %s on tape does not match %d",
	 tmpStr, tapeDev->position.filePos.fileSeqNo);
        return TAPE_HEADER_LABEL_ERROR;
    }

    if (strncmp (myHdr1Label.tapeNum, tapeDev->mountedCart->tapeNum, 
     TAPE_NUM_LEN) != 0) {
        elog (NOTICE, "readVerifyHdr: tapeNum %s on tape does not match %s",
         myHdr1Label.tapeNum, tapeDev->mountedCart->tapeNum);
        return TAPE_HEADER_LABEL_ERROR;
    }

   /* HDR1 seems to be OK. Now check HDR2 */

    retVal = read (tapeDev->fd, (char *) &myHdr2Label, sizeof(hdr2Label_t));

    if (retVal != sizeof(hdr2Label_t)) {
        /* Could not read the HDR2. Either the header is not there or
         * the header has less than 80 char
         */
        elog (NOTICE,
	  "readVerifyHdr: Unable to read HDR2, read %d of %d, errno=%d",
            retVal, sizeof (hdr2Label_t), errno);
        return TAPE_HEADER_LABEL_ERROR;
    }

    /* Verify HDR2 */

    if (memcmp(myHdr2Label.labelID, "HDR", 3) != 0 ||
     myHdr2Label.labelNo[0] != '2') {
        elog (NOTICE, 
	  "readVerifyHdr: The HDR label does not start with HDR2");
        return TAPE_HEADER_LABEL_ERROR;
    }

    /* Well, we are done */

    tapeDev->state |= ST_IO_READY;
    tapeDev->position.filePos.offset = DATA_BEGIN_OFFSET;

    return (0);
}

int
writeHeader (tapeDev_t *tapeDev,  char *fileID)
{
    hdr1Label_t myHdr1Label;
    hdr2Label_t myHdr2Label;
    int retVal;

    /* check if it is opened for write */

    if ((tapeDev->state & ST_WRITE_MODE) == 0) {
	elog (NOTICE, "writeHeader: tapeDev not opened for write");
	return TAPE_FILE_NOT_OPENED;
    }

    if (tapeDev->position.filePos.offset != 0) {
        elog (NOTICE,
	  "writeHeader: Trying to write HDR in the middle of a file");
        return TAPE_FILE_POSITION_ERROR;
    }

    retVal = setHdr1Struct (&myHdr1Label, tapeDev->mountedCart->tapeNum, 
     fileID, 0, tapeDev->position.filePos.fileSeqNo); 

    if (retVal < 0)
	return retVal;

   /* Write the HDR1 label */

    retVal = write (tapeDev->fd, &myHdr1Label, sizeof (hdr1Label_t));

    if (retVal != sizeof (hdr1Label_t)) {
        elog (NOTICE,
	  "writeHeader: Error writing HDR1, wrote %d bytes, errno %d",
         retVal, errno);
        retVal = unixErrConv (errno);
        return retVal;
    }

    retVal = setHdr2Struct (&myHdr2Label, tapeDev->blockSz);

    if (retVal < 0)
        return retVal;

   /* Write the HRD2 label */

    retVal = write (tapeDev->fd, &myHdr2Label, sizeof (hdr2Label_t));

    if (retVal != sizeof (hdr2Label_t)) {
        elog (NOTICE,
	 "writeHeader: Error writing HDR2, wrote %d bytes, errno %d",
         retVal, errno);
        retVal = unixErrConv (errno);
        return retVal;
    }

    /* Well, we are done */

    tapeDev->state |= ST_IO_READY;
    tapeDev->state &= ~(ST_EOT);
    tapeDev->position.filePos.offset = sizeof (hdr1Label_t) +
     sizeof (hdr2Label_t);

    return (0);
}

int
srbOpenTapeForRead (tapeDev_t *tapeDev, tapePos_t *destPos)
{
    int retVal;

    /* open the tape */

    if (genOpenTape (tapeDev, O_RDONLY) < 0)
        return tapeDev->fd;

    retVal = swSeekTape (tapeDev->drvInx, tapeDev, destPos);

    if (retVal < 0) {
        swCloseTape (tapeDev->drvInx, tapeDev, 0);
        return retVal;
    }

    /* Read and verify the headers */

    retVal = readVerifyHdr (tapeDev);

    if (retVal < 0) {
        swCloseTape (tapeDev->drvInx, tapeDev, 0);
        return retVal;
    }

    return (0);
}

int
srbOpenTapeForWrite (tapeDev_t *tapeDev, tapePos_t *destPos, char *fileID)
{
    int retVal;

    /* open the tape */

    if (swOpenTape (tapeDev->drvInx, tapeDev, O_RDWR) < 0)
        return tapeDev->fd;

    retVal = swSeekTape (tapeDev->drvInx, tapeDev, destPos);

    if (retVal < 0) {
        swCloseTape (tapeDev->drvInx, tapeDev, 0);
        return retVal;
    }

#ifdef foo
    /* Read and verify the headers */

    retVal = readVerifyHdr (tapeDev);

    if ((tapeDev->state & ST_EOT) == 0) {
        printf ("genOpenTapeForWrite: Try to write in the middle of tape\n");
        genCloseTape (tapeDev, 1);
        return -1;
    }
#endif /* foo */

    retVal = writeHeader (tapeDev, fileID);

    if (retVal < 0) {
        swCloseTape (tapeDev->drvInx, tapeDev, 0);
        return retVal;
    }

    return retVal;
}

int
srbReadTape (tapeDev_t *tapeDev, char *buf, int length)
{
    int retVal;

    if ((tapeDev->state & ST_IO_READY) == 0 || tapeDev->fd < 0 ||
     tapeDev->position.filePos.offset < DATA_BEGIN_OFFSET) {
        elog (NOTICE,
	  "srbReadTape: Tape drive %s is not ready to read tape %s.",
          tapeDev->devPath, tapeDev->mountedCart->tapeNum);
        retVal = unixErrConv (errno);
        return retVal;
    }

    if (tapeDev->position.filePos.fileSeqNo < 1) {
        /* must be positioned after the first EOF mark */
        elog (NOTICE,
	  "genReadTape: Tape positioned before the first EOF mark.");
        retVal = unixErrConv (errno);
        return retVal;
    }

    retVal = read(tapeDev->fd, buf, length);

    if (retVal > 0) {
        tapeDev->position.filePos.offset += retVal;
    } else if (retVal == 0) {   /* read EOF */
#if defined(PORTNAME_aix)
        /* The fileSeqNo does not get advanced with solaris */
        tapeDev->position.filePos.fileSeqNo ++;
        tapeDev->position.filePos.offset = 0;
#endif
    } else {	/* error */
        elog (NOTICE,
          "genReadTape: Tape read error. errno = %d", errno);
        retVal = unixErrConv (errno);
    }

    return (retVal);
}

int
srbWriteTape (tapeDev_t *tapeDev, char *buf, int length)
{
    int retVal;

    if ((tapeDev->state & ST_IO_READY) == 0 || tapeDev->fd < 0 ||
     (tapeDev->state & ST_WRITE_MODE) == 0 ||
      tapeDev->position.filePos.offset < DATA_BEGIN_OFFSET) {
        elog (NOTICE,
	  "srbWriteTape: Tape drive %s is not ready to write tape %s.",
          tapeDev->devPath, tapeDev->mountedCart->tapeNum);
        return (TAPE_FILE_POSITION_ERROR);
    }

    if (tapeDev->position.filePos.fileSeqNo < 1) {
        /* must be positioned after the first EOF mark */
        elog (NOTICE,
	  "genReadTape: Tape positioned before the first EOF mark.");
        return (TAPE_FILE_SEQ_NUM_ERROR);
    }

    retVal = write (tapeDev->fd, buf, length);

    if (retVal > 0) {
        tapeDev->position.filePos.offset += retVal;
    }

    return (retVal);
}

int initDrvByDevPath (char *devPath, tapeDev_t *tapeDev)
{
    int i;
    tapeDrvConfig_t *tmpTapeDrvConfig;

#ifdef foo
    for (i = 0; i < NumTapeDrv; i++) {
	if (strcmp (devPath, TapeDrvConfig[i].devPath) == 0) {
	    return (TapeDrvConfig[i].drvInx);
	}
    }
#endif	/* foo */
    tmpTapeDrvConfig = TapeDrvConfig;
    while (tmpTapeDrvConfig != NULL) {
	if (strcmp (devPath, tmpTapeDrvConfig->devPath) == 0) {
            tapeDev->drvInx = tmpTapeDrvConfig->drvInx;
            tapeDev->drvTypeInx = tmpTapeDrvConfig->cartType;
	    strcpy (tapeDev->address, tmpTapeDrvConfig->address);
	    strcpy (tapeDev->devPath, devPath);
            return (0);
	}
	tmpTapeDrvConfig = tmpTapeDrvConfig->next;
    }

    return (TAPE_DEVICE_PATH_ERROR);
}

int
initTapeLibConfig ()
{
   FILE *file;
    char buf[MAX_TOKEN];
    char *conFile;
    int i;
    tapeLibConfig_t *tmpConfig;

    TapeLibConfigHead = NULL;

    /* put together the full pathname for the config file */

    conFile = (char *) malloc((strlen(getSrbDataDir()) +
        strlen(TAPELIB_CONFIG)+2)*sizeof(char));

    strcpy(conFile, getSrbDataDir());
    strcat(conFile, "/");
    strcat(conFile, TAPELIB_CONFIG);

    file = fopen(conFile, "r");

    if (file == NULL) {
        elog (NOTICE,
         "initTapeLibConfig: Cannot open tapeLib config file: %s.",
          TAPELIB_CONFIG);
        free (conFile);
        return (TAPE_TAPELIB_CONFIG_ERROR);
    }

    free (conFile);

    while ((i = getNextStr(file, buf, sizeof(buf))) != EOF) {

        /* Read until end of line then next line */

        if (buf[0] == '#') {
            while (i == 0)
                i = getNextStr(file, buf, sizeof(buf));
            continue;
        }

        if (i == '\n') {
            elog (NOTICE,
             "initTapeLibConfig: %s must have 3 entries per line. Got %s",
             TAPELIB_CONFIG, buf);
	    fclose (file);
            return TAPE_TAPELIB_CONFIG_ERROR;
        }

        tmpConfig = (tapeLibConfig_t *) malloc (sizeof(tapeLibConfig_t));

        if (tmpConfig == NULL) {
            elog (NOTICE, 
	      "initTapeLibConfig: malloc error. Errno = %d", errno);
	    fclose (file);
            return (TAPE_TAPELIB_CONFIG_ERROR);
        }

        tmpConfig->inx = atoi (buf);

        /* read the next input */

        i = getNextStr(file, buf, sizeof(buf));

        if (buf[0] == '#') {
            elog (NOTICE, 
             "initTapeLibConfig: %s must have 3 entries per line. Got %s",
             TAPELIB_CONFIG, buf);
	    free (tmpConfig);
            return TAPE_TAPELIB_CONFIG_ERROR;
        }

        strcpy (tmpConfig->host, buf);

        /* read the last input */

        i = getNextStr(file, buf, sizeof(buf));

        if (buf[0] == '#') {
            elog (NOTICE, 
             "initTapeLibConfig: %s must have 3 entries per line. Got %s",
             TAPELIB_CONFIG, buf);
	    free (tmpConfig);
            return TAPE_TAPELIB_CONFIG_ERROR;
        }

        strcpy (tmpConfig->port, buf);

	/* queue it */

	if (TapeLibConfigHead == NULL) {
	    TapeLibConfigHead = tmpConfig;
	    tmpConfig->next = NULL;
	} else {
	    tmpConfig->next = TapeLibConfigHead->next;
	    TapeLibConfigHead = tmpConfig;
	}
	    
        /* discard the rest */

        while (i == 0)
            i = getNextStr(file, buf, sizeof(buf));
    }
    fclose (file);
    return (0);
}

tapeLibConfig_t *
matchTapeLibByInx (int inx)
{
tapeLibConfig_t *tmpConfig;

    tmpConfig = TapeLibConfigHead;

    while (tmpConfig != NULL) {
	if (inx == tmpConfig->inx)
	    return (tmpConfig);
	tmpConfig = tmpConfig->next;
    }

    return (NULL);
}

/* initTapeBuf - malloc a tape buf when needed.
 * drvInx - Index into the TapeDrvConfig[] table.
 */

int
initTapeBuf (int cartType, int curBufLen, char **buf)
{
    int bufLen;
    int i;

    if (cartType < 0 || cartType >= NumTapeMediaChar) {
	elog (NOTICE, "initTapeBuf: Input cartType %d out of range", cartType);
	*buf = NULL;
	return (TAPE_DEVICE_INX_ERROR);
    }

    bufLen = getBlkSizeByCartTypeInx (cartType);
	
    if (bufLen < 0) {
	elog (NOTICE, "initTapeBuf: No match for cartType %d", cartType);
	return (-1);
    }

    if (*buf == NULL) {
        *buf = malloc (bufLen);
    } else {
        if (bufLen != curBufLen) {
	    free (buf);
            *buf = malloc (bufLen);
        } else {
	    bufLen = curBufLen;
	}
    }
    return (bufLen);
}

int
initTapeDevConfig ()
{
    int i;
    FILE *file;
    int retVal;
    char devPath[MAX_TOKEN];
    char address[MAX_TOKEN];
    char cartTypeName[MAX_TOKEN];
    char *conFile;
    tapeDrvConfig_t *myTapeDrvConfig;
    int cartType;
    int drvInx;
    int numTapeDrv;

    conFile = (char *) malloc((strlen(getSrbDataDir()) +
        strlen(TAPE_DEV_CONFIG)+2)*sizeof(char));

    strcpy(conFile, getSrbDataDir());
    strcat(conFile, "/");
    strcat(conFile, TAPE_DEV_CONFIG);

    file = fopen(conFile, "r");

    if (file == NULL) {
        elog (NOTICE,
          "initTapeDev: Open of %s error, errno = %d\n", conFile, errno);
        free (conFile);
        return (-1);
    }
    free (conFile);

    numTapeDrv = 0;
    TapeDrvConfig = NULL;

    while ((i = getNextStr(file, devPath, sizeof(devPath))) != EOF) {
        if (i != 0)
            continue;

        /* Read until end of line then next line */

        if (devPath[0] == '#')
        {
            while (i == 0)
                i = getNextStr(file, devPath, sizeof(devPath));
            continue;
        }

        if (strlen (devPath) >= TAPE_DEVNAME_LEN) {
            elog (NOTICE, "initTapeDev: devPath %s too long", devPath);
            return -1;
        }

        i = getNextStr(file, address, sizeof (address));
        if (i != 0) {
            elog (NOTICE,
              "initTapeDev: line starts with %s too short", devPath);
            return -1;
        }

        if (strlen (address) >= DRIVE_ADDR_LEN) {
            elog (NOTICE, "initTapeDev: address %s too long", address);
            return -1;
        }

        i = getNextStr(file, cartTypeName, sizeof (cartTypeName));

        cartType = getCartTypeInxByCartName (cartTypeName);
        if (cartType < 0) {
            elog (NOTICE, "initTapeDev: getCartTypeInxByCartName error for %s",
              cartTypeName);
            return -1;
        }

        drvInx = getDrvInxByCartName (cartTypeName);
        if (drvInx < 0) {
            elog (NOTICE, "initTapeDev: getDrvInxByCartName error for %s",
              cartTypeName);
            return -1;
        }

        myTapeDrvConfig = malloc (sizeof (tapeDrvConfig_t));

        strcpy (myTapeDrvConfig->devPath, devPath);
        strcpy (myTapeDrvConfig->address, address);
        myTapeDrvConfig->drvInx = drvInx;
        myTapeDrvConfig->cartType = cartType;

        /* Queue it */

        if (TapeDrvConfig == NULL) {
            myTapeDrvConfig->next = NULL;
        } else {
            myTapeDrvConfig->next = (void *) TapeDrvConfig;
        }
        TapeDrvConfig = myTapeDrvConfig;

        numTapeDrv ++;

        while (i == 0) {        /* read to end of line */
            i = getNextStr(file, devPath, sizeof(devPath));
        }

    }

    if (numTapeDrv == 0) {
        elog (NOTICE, "initTapeDev: No tape device configured");
        return 0;
    }
    return (0);
}

int
getCartTypeInxByCartName (char *cartTypeName)
{
    int i;

    for (i = 0; i < NumTapeMediaChar; i++) {
        if (strcmp (cartTypeName, TapeMediaCharTable[i].name) == 0) {
	    return (TapeMediaCharTable[i].type);
        }
    }
    return (-1);
}

char *
getCartNameByCartTypeInx (int cartTypeInx)
{
    int i;

    for (i = 0; i < NumTapeMediaChar; i++) {
	if (cartTypeInx == TapeMediaCharTable[i].type) {
            return (TapeMediaCharTable[i].name);
        }
    }
    return (NULL);
}

int
getDrvInxByCartName (char *cartTypeName)
{
    int i;

    for (i = 0; i < NumTapeMediaChar; i++) {
        if (strcmp (cartTypeName, TapeMediaCharTable[i].name) == 0) {
            return (TapeMediaCharTable[i].dvrInx);
        }
    }
    return (-1);
}

int
getBlkSizeByCartTypeInx (int cartType)
{
    int i;

    for (i = 0; i < NumTapeMediaChar; i++) {
        if (cartType == TapeMediaCharTable[i].type) {
            return (TapeMediaCharTable[i].blkSize);
        }
    }
    return (-1);
}

