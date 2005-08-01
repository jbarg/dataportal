#include <string.h>
#include <stdio.h>
#include "srbPrelMisc.h"

extern srbConn *prelConn;
extern struct srbFd srbFdArray[];
extern int CwdFlag;

/* timing switch */
/* #define INSTRON */

srbConn * connToSrb () 
{
    srbConn *conn;

    if (EnvFilePath[0] == '\0') {      /* Have not been initialized */
	if (initSrbEnv () < 0) {
	    fprintf (stderr,"connToSrb: initSrbEnv error\n");
	    return NULL;
	}
    }

    conn = clConnect(0,0,0);

    if(clStatus(conn) != CLI_CONNECTION_OK) {
	fprintf(stderr,"connToSrb: %s\n",strerror(errno));
	return 0;
    } else {
	return conn;
    }
}

int isSrbPath( char *inPath, char *resource, char *outPath )
{
    int fieldCnt;
    int result;
    srbPathName inpPath;
    char *myPath, tmpBuf[MAX_TOKEN], tmpPtr;;

    if (resource == NULL || outPath == NULL)
	return -1;

    outPath[0] = 0;
    resource[0] = 0;

    if (inPath[0] == '/' && 
     strncmp (inPath, SRB_ROOT, strlen (SRB_ROOT)) != 0) {
	/* a local absolute path */
	strcpy (outPath, inPath);
	return 0;
    }

    inpPath.inpArgv = inPath;

    /* parse the path */
    if (myParseName (&inpPath, resource) < 0) {
        errno = EDQUOT;
        return -1;
    }

    myPath = inpPath.outArgv;

    if (strncmp (myPath, SRB_ROOT, strlen (SRB_ROOT)) == 0) {
	myPath = myPath + strlen (SRB_ROOT) - 1;
	strcpy (outPath, myPath);
        fieldCnt = 1;
    } else if (strcmp (myPath, SRB_ROOT1) == 0) {	/* just /srb */
	outPath[0] = '/';	/* it is the srb root node */
	outPath[1] = '\0';
        fieldCnt = 1;
    } else {
	strcpy (outPath, myPath);
        fieldCnt = 0;
    }

    free (inpPath.outArgv);

    return (fieldCnt);
}

int
extractRes (char *inPath, char *outPath, char *resource)
{
    char *tmpPtr;

    if (splitStringfromEnd (inPath, outPath, resource, '%') != NULL &&
     outPath[0] != '\0') {
	/* take care of the case where resource in defined in
	 * the middle of the path */
	if ((tmpPtr = strstr (resource, "/")) != NULL) {
	    strcat (outPath, tmpPtr);
	    tmpPtr[0] = '\0';
	}
	return 1;
    } else {
        strcpy (outPath, resource);
        resource[0] = '\0';
	return 0;
    }
}

int myParseName (srbPathName *srbPathName, char *condition)
{
    char temp[MAX_OBJ_NAME_LENGTH], temp1[MAX_OBJ_NAME_LENGTH], 
     temp2[MAX_OBJ_NAME_LENGTH], temp3[MAX_OBJ_NAME_LENGTH];
    char tmpInp[8];
    char *myInput;
    int len = 0, dot2cnt;
    char *cptr1, *cptr2;
    char *cwdPtr, *homePtr;
    int tmpLen;

    if (strncmp (srbPathName->inpArgv, SRB_PREFIX, strlen (SRB_PREFIX)) == 0) {
	/* srb prefix */
	/* skip the prefix */
	srbPathName->inpArgv += strlen (SRB_PREFIX);
	/* do not allow just s: because 'ls -R' will cause
         * the content of the directory to look like s:/aaaa.
         * i.e., absolute path.
         */
        if (srbPathName->inpArgv[0] == '\0')
            return -1;
	if (srbPathName->inpArgv[0] != '/') {
	    /* relative path */
	    cwdPtr = getenv (SRB_CWD);
	    if (cwdPtr == NULL) {
		errno = EEXIST;
		return -1;
            }
	    homePtr = getenv (SRB_HOME);
	    if (homePtr == NULL) {
            	/* initialize it now */
            	if (EnvFilePath[0] == '\0') {      /* Have not been initialized */
                    if (initSrbEnv () < 0) {
                    	fprintf (stderr,"mygethome: initSrbEnv error\n");
                    	errno = EEXIST;
                    	return -1;
		    }
                }
            	tmpLen = strlen (mdasCollectionHome) + strlen (SRB_CWD) + 8;
            	homePtr = malloc (tmpLen);
            	sprintf (homePtr, "%s=%s", SRB_HOME, mdasCollectionHome);
            	putenv (homePtr);
            	homePtr = getenv (SRB_HOME);
            }
	}
    } else if (strncmp (srbPathName->inpArgv, LOCAL_PREFIX, 
     strlen (LOCAL_PREFIX)) == 0) {
	/* srb prefix */
	/* skip the prefix */
	srbPathName->inpArgv += strlen (LOCAL_PREFIX);
	/* do not allow just s: because 'ls -R' will cause
         * the content of the directory to look like s:/aaaa.
         * i.e., absolute path.
         */
        if (srbPathName->inpArgv[0] == '\0')
            return -1;
	if (srbPathName->inpArgv[0] != '/') {
	    cwdPtr = getenv (LOCAL_CWD);
	    if (cwdPtr == NULL) {
		errno = EEXIST;
		return -1;
            }
	    homePtr = getenv (LOCAL_HOME);
	    if (homePtr == NULL) {
		errno = EEXIST;
		return -1;
            }
	}
    } else {
    	cwdPtr = _mygetcwd ();
    	if (cwdPtr == NULL) {
	    errno = EEXIST;
	    return -1;
    	}
    	homePtr = mygethome ();
    	if (homePtr == NULL) {
	    errno = EEXIST;
	    return -1;
    	}
    }

    strcpy (temp3, srbPathName->inpArgv);

    if (strncmp (cwdPtr, SRB_ROOT, strlen (SRB_ROOT)) == 0) {
	/* a srb path */
	filterSrbPath (temp3, LOCAL_PREFIX_PATH);
	extractRes (temp3, temp2, condition);
	myInput = temp2;
    } else {
	filterSrbPath (temp3, SRB_PREFIX_PATH);
	myInput = temp3;
    }
	
    srbPathName->outArgv = malloc (MAX_OBJ_NAME_LENGTH);

    if (srbPathName->outArgv == NULL) {
        errno = CLI_ERR_MALLOC;
        fprintf(stderr, "Client malloc error.\n");
        return -1;
    }

    len = strlen(myInput);

    /* Parse the first 3 char to take care of ".", "..", etc */

    if(len == 0) {
        strcpy(srbPathName->outArgv, cwdPtr);
        srbPathName->type = COLLECTION_T;
        return (COLLECTION_T);
    } else if (myInput[0] == '^' ||
      myInput[0] == '~') {
        if (len == 1) {
            strcpy(srbPathName->outArgv, homePtr);
            srbPathName->type = COLLECTION_T;
            return (COLLECTION_T);
        } else if (myInput[1] == '/') {
            sprintf(srbPathName->outArgv, "%s%s",
              homePtr, (char *)((int) myInput + 1));
        } else {
            clSplitbychar(homePtr,temp1, temp,'/');
            sprintf(srbPathName->outArgv, "%s/%s",
             temp1,(char *)((int) myInput + 1));
        }
    } else if (myInput[0] == '.' && len == 1) {
        strcpy(srbPathName->outArgv, cwdPtr);
        srbPathName->type = SPEC_COLL_T;
        return (SPEC_COLL_T);
    } else if (myInput[0] == '.' &&
     myInput[1] == '/') {  /* case for "./" */
        if (len == 2) {
            strcpy(srbPathName->outArgv, cwdPtr);
            srbPathName->type = SPEC_COLL_T;
            return (SPEC_COLL_T);
        } else {
            sprintf(srbPathName->outArgv, "%s%s",
             cwdPtr, (char *)((int) myInput + 1));
        }
    } else if (myInput[0] == '.' &&
     myInput[1] == '.') {  /* case for ".." */
        if (len == 2 || (myInput[2] == '/' && len == 3)) {
            clSplitbychar(cwdPtr, srbPathName->outArgv, temp,'/');
	    if (srbPathName->outArgv[0] == '\0') {
		srbPathName->outArgv[0] = '/';	/* root dir */
		srbPathName->outArgv[1] = '\0';
	    }
            srbPathName->type = SPEC_COLL_T;
            return (SPEC_COLL_T);
        }
        if (myInput[2] == '/') {   /* case for "../" */
            clSplitbychar(cwdPtr,temp1, temp,'/');
            sprintf(srbPathName->outArgv, "%s%s", temp1,
              (char *)((int) myInput + 2));
        } else {
              fprintf(stderr, "inconsistent name: %s\n", myInput);
              return CLI_ERR_COLLNAME;
        }
    } else if (myInput[0] == '/') {
          strcpy(srbPathName->outArgv, myInput);
    } else {
        sprintf(srbPathName->outArgv, "%s/%s",
          cwdPtr, myInput);
    }

    /* Take care of any "./" and "/" in between */

    len = strlen (srbPathName->outArgv);
    cptr1 = srbPathName->outArgv + len - 2;
    while (cptr1 >= srbPathName->outArgv) {
        if (*cptr1 == '/' && *(cptr1 + 1) == '/') {     /* A "//" pattern */
            strcpy (cptr1, (cptr1 + 1));
        } else if (*cptr1 == '.' && *(cptr1 + 1) == '/') {
            if (*(cptr1 - 1) == '/') {  /* we have a "/./" pattern */
                strcpy (cptr1, (cptr1 + 2));    /* just eliminate "./" */
            }
        }
        cptr1 --;
    }

    /* Take care of any "../" in between */

    len = strlen (srbPathName->outArgv) - 3;
    cptr1 = srbPathName->outArgv;
    while (cptr1 <= srbPathName->outArgv + len ) {
        if (*cptr1 == '/' && *(cptr1 + 1) == '.' &&
          *(cptr1 + 2) == '.' && *(cptr1 + 3) == '/') {
            /* we have a "/../" pattern. Skip to the next "/" */
            cptr2 = (cptr1 + 4);        /* point to the start of the tail */
	    len -= 3;
            cptr1 --;   /* skip pass the current "/" */
            while (cptr1 >= srbPathName->outArgv && *cptr1 != '/') {
                cptr1 --;
		len --;
            }
            if (*cptr1 == '/')
                strcpy (cptr1 + 1, cptr2);
            else
               return CLI_ERR_COLLNAME;
        }
        cptr1 ++;
    }
    /* Now handle the last 3 char to take care of "/.", "/..", etc */

    len = strlen (srbPathName->outArgv);
    cptr1 = srbPathName->outArgv + len - 1;

    if (len >= 2 && *cptr1 == '.' && *(cptr1 - 1) == '/') {
        if (len == 2)
            *cptr1 = '\0';
        else
            *(cptr1 - 1) = '\0';
        srbPathName->type = SPEC_COLL_T;
    } else if (len >= 3 && *cptr1 == '.' && *(cptr1 - 1) == '.' &&
      *(cptr1 - 2) == '/') {
        if (len == 3)
            return CLI_ERR_COLLNAME;

        /* we have a "/../" pattern. Skip to the next "/" */
        cptr1 -= 3;   /* skip pass the "/" */
        while (cptr1 >= srbPathName->outArgv && *cptr1 != '/') {
            cptr1 --;
        }
        if (*cptr1 == '/')
            *cptr1 = '\0';
        else
           return CLI_ERR_COLLNAME;
        srbPathName->type = SPEC_COLL_T;
    } else {
        srbPathName->type = DATA_COLL_T;
    }

    /* take care of ending / */
    len = strlen (srbPathName->outArgv) - 1;
    while (len > 1) {
        if (srbPathName->outArgv[len] == '/')
            srbPathName->outArgv[len] = '\0';
        else
            break;
        len --;
    }

    return srbPathName->type;
}

int
filterSrbPath (char *outPath, char *filterStr)
{
int result;
char *tmpPtr;

    result = strlen (outPath) - 1;
    if (result > 1 && outPath[result] == '/')
        outPath[result] = '\0';

    if ((tmpPtr = strstr (outPath, filterStr)) != NULL) {
        /* take out any "/s:" in the input */
        result = (int) tmpPtr - (int) outPath;
        outPath += (result + 1);
        tmpPtr += strlen (filterStr);
        strcpy (outPath, tmpPtr);
	return 1;
    } else {
	return 0;
    }
}

int srbStat (char *path, struct stat *buf, int myType)
{
    int status;
    mdasC_sql_result_struct myresult;
    struct srbStat mySrbStat;

    status = _srbObjStat (prelConn, MDAS_CATALOG, path, &mySrbStat, myType);
    if (status >= 0)
        status = srbToLocalStat (&mySrbStat, buf);

    return status;
}

#ifdef LOCAL_STAT
int srbStatl (char *path, struct stat *buf, int myType)
{
    int i;
    int status;
    mdasC_sql_result_struct myresult;
    char dataName[MAX_TOKEN], collection[MAX_TOKEN];
    int myUid, myGid, moMode;

    splitpath (path, collection, dataName);

    if (myType <= IS_FILE && 
     (status = srbDataStat (dataName, collection, &myresult)) >= 0) {
    	/* interprete the info */
    	char *sizeList, *timeList, *phyResc;

    	sizeList = (char *) getFromResultStruct(
      	 (mdasC_sql_result_struct *) &myresult,
          dcs_tname[SIZE], dcs_aname[SIZE]);
    	timeList = (char *) getFromResultStruct(
      	 (mdasC_sql_result_struct *) &myresult,
          dcs_tname[REPL_TIMESTAMP], dcs_aname[REPL_TIMESTAMP]);
    	phyResc = (char *) getFromResultStruct(
      	 (mdasC_sql_result_struct *) &myresult,
          dcs_tname[PHY_RSRC_NAME], dcs_aname[PHY_RSRC_NAME]);

	parseDataUserMode (&myresult, &myUid, &myGid, &moMode);
	buf->st_uid = myUid;
	buf->st_gid = myGid;
	buf->st_mode = moMode;
	buf->st_mode |= S_IFREG;
	

	buf->st_ino = FAKE_INODE;
#if defined(PORTNAME_solaris)
	buf->st_atim.tv_sec = buf->st_mtim.tv_sec = buf->st_ctim.tv_sec = 
	 (long) srbToLocalTime (timeList);
#else
        buf->st_atime = buf->st_mtime = buf->st_ctime =
         (long) srbToLocalTime (timeList);
#endif
	buf->st_size = atoi (sizeList);
	buf->st_dev = FAKE_DEV;
	buf->st_blksize = MY_BLKSIZE;
	buf->st_blocks = buf->st_size/MY_BLKSIZE + 1;
	 buf->st_nlink = 1;
#if defined(PORTNAME_solaris)
	strncpy (buf->st_fstype, phyResc, _ST_FSTYPSZ - 1);
	buf->st_fstype[_ST_FSTYPSZ - 1] = '\n';
#endif
	clearSqlResult (&myresult);
    } else if (myType != IS_FILE && srbCollStat (path, &myresult) >= 0) {
	/* a directory */
        parseCollUserMode (&myresult, &myUid, &myGid, &moMode);
        buf->st_uid = myUid;
        buf->st_gid = myGid;
        buf->st_mode = moMode;

	buf->st_mode |= S_IFDIR;
	buf->st_dev = FAKE_DEV;
	buf->st_size = MY_BLKSIZE;
	buf->st_blksize = MY_BLKSIZE;
	buf->st_ino = FAKE_INODE;
	buf->st_blocks = 10;
	buf->st_nlink = 3;
#if defined(PORTNAME_solaris)
	strcpy (buf->st_fstype, "srb");
	buf->st_atim.tv_sec = buf->st_mtim.tv_sec = buf->st_ctim.tv_sec = 
	 (long) 999999;
#else
        buf->st_atime = buf->st_mtime = buf->st_ctime =
         (long) 999999;
#endif
	status = 0;
	clearSqlResult (&myresult);
    } else {
        if (status == MCAT_INQUIRE_ERROR) {       /* no answer */
	    errno = ENOENT;
	} else {
	    errno =ENOLINK;
	}
	status = -1;
    }
    return status;
}

int srbStat64l (char *path, struct stat64 *buf, int myType)
{
    int i;
    int status;
    mdasC_sql_result_struct myresult;
    char dataName[MAX_TOKEN], collection[MAX_TOKEN];
    int myUid, myGid, moMode;

    splitpath (path, collection, dataName);


    if (myType <= IS_FILE &&
     (status = srbDataStat (dataName, collection, &myresult)) >= 0) {
    	/* interprete the info */
	char *sizeList, *timeList, *phyResc;

    	sizeList = (char *) getFromResultStruct(
      	 (mdasC_sql_result_struct *) &myresult,
          dcs_tname[SIZE], dcs_aname[SIZE]);
    	timeList = (char *) getFromResultStruct(
      	 (mdasC_sql_result_struct *) &myresult,
          dcs_tname[REPL_TIMESTAMP], dcs_aname[REPL_TIMESTAMP]);
        phyResc = (char *) getFromResultStruct(
         (mdasC_sql_result_struct *) &myresult,
          dcs_tname[PHY_RSRC_NAME], dcs_aname[PHY_RSRC_NAME]);

        parseDataUserMode (&myresult, &myUid, &myGid, &moMode);
        buf->st_uid = myUid;
        buf->st_gid = myGid;
        buf->st_mode = moMode;
        buf->st_mode |= S_IFREG;

	buf->st_ino = FAKE_INODE;
#if defined(PORTNAME_solaris)
	buf->st_atim.tv_sec = buf->st_mtim.tv_sec = buf->st_ctim.tv_sec = 
	 (long) srbToLocalTime (timeList);
#else
        buf->st_atime = buf->st_mtime = buf->st_ctime =
         (long) srbToLocalTime (timeList);
#endif
	buf->st_size = strtoll (sizeList, 0, 0);
	buf->st_dev = FAKE_DEV;
	buf->st_blksize = MY_BLKSIZE;
	buf->st_blocks = buf->st_size/MY_BLKSIZE + 1;
	buf->st_nlink = 1;
#if defined(PORTNAME_solaris)
	strncpy (buf->st_fstype, phyResc, _ST_FSTYPSZ - 1);
	buf->st_fstype[_ST_FSTYPSZ - 1] = '\n';
#endif
	clearSqlResult (&myresult);
    } else if (myType != IS_FILE && srbCollStat (path, &myresult) == 0) {
	/* a directory */
        parseCollUserMode (&myresult, &myUid, &myGid, &moMode);
        buf->st_uid = myUid;
        buf->st_gid = myGid;
        buf->st_mode = moMode;
	buf->st_ino = FAKE_INODE + strlen (path);

        buf->st_mode |= S_IFDIR;
	buf->st_size = MY_BLKSIZE;
	buf->st_blksize = MY_BLKSIZE;
	buf->st_dev = 1;
	buf->st_blocks = 10;
	buf->st_nlink = 3;
#if defined(PORTNAME_solaris)
	strcpy (buf->st_fstype, "srb");
	buf->st_atim.tv_sec = buf->st_mtim.tv_sec = buf->st_ctim.tv_sec =
         (long) 999999;
#else
        buf->st_atime = buf->st_mtime = buf->st_ctime =
         (long) 999999;
#endif
	status = 0;
	clearSqlResult (&myresult);
    } else if (strcmp (path, "/") == 0) { 	/* does not work with root */
        buf->st_uid = FAKE_ROOT_USER;
        buf->st_gid = FAKE_ROOT_USER;
        buf->st_mode = S_IRWXU | S_IFDIR;
        buf->st_mode |= S_IFDIR;
        buf->st_ino = FAKE_INODE;

        buf->st_mode |= S_IFDIR;
        buf->st_size = MY_BLKSIZE;
        buf->st_blksize = MY_BLKSIZE;
        buf->st_dev = 1;
        buf->st_blocks = 10;
        buf->st_nlink = 3;
#if defined(PORTNAME_solaris)
        strcpy (buf->st_fstype, "srb");
        buf->st_atim.tv_sec = buf->st_mtim.tv_sec = buf->st_ctim.tv_sec =
         (long) 999999;
#else
        buf->st_atime = buf->st_mtime = buf->st_ctime =
         (long) 999999;
#endif
        errno = 0;
        status = 0;
    } else {
        if (status == MCAT_INQUIRE_ERROR) {       /* no answer */
	    errno = ENOENT;
	} else {
	    errno =ENOLINK;
	}
	status = -1;
    }
    return status;
}
#endif	/* LOCAL_STAT */

int srbStat64 (char *path, struct stat64 *buf, int myType)
{
    int status;
    mdasC_sql_result_struct myresult;
    struct srbStat mySrbStat;

    status = _srbObjStat (prelConn, MDAS_CATALOG, path, &mySrbStat, myType);

    if (status >= 0)
        status = srbToLocalStat64 (&mySrbStat, buf);

    return status;
}

int srbDataStat (char *dataName, char *collection, 
mdasC_sql_result_struct *myresult)
{
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i;
    int status;
#ifdef INSTRON
    hrtime_t beforeTime;
#endif

    if (dataName == NULL || collection == NULL || myresult == NULL) 
	return -1;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }

    selval[DATA_ID] = 1;
    selval[SIZE] = 1;
    selval[ACCESS_CONSTRAINT] = 1;

    selval[DATA_OWNER] = 1;
    selval[DATA_OWNER_DOMAIN] = 1;
    selval[USER_NAME] = 1;
    selval[DOMAIN_DESC] = 1;
    selval[REPL_TIMESTAMP] = 1;
    selval[PHY_RSRC_NAME] = 1;

    sprintf(qval[DATA_GRP_NAME]," = '%s'", collection);
    sprintf(qval[DATA_NAME]," = '%s'", dataName);

#ifdef INSTRON
    beforeTime =  gethrtime ();
#endif
    status = srbGetDataDirInfo(prelConn, MDAS_CATALOG, qval, selval,
      myresult, DEF_NUM_ROWS);
#ifdef INSTRON
    fprintf (stderr, "srbDataStat: query time = %lld\n", 
     gethrtime () - beforeTime);
#endif

    return (status);
}

int srbCollStat (char *collection, mdasC_sql_result_struct *myresult)
{
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i;
    int status;
#ifdef INSTRON
    hrtime_t beforeTime;
#endif

    if (collection == NULL || myresult == NULL) 
	return -1;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }

    selval[USER_NAME] = 1;
    selval[DOMAIN_DESC] = 1;
    selval[GROUP_ACCESS_CONSTRAINT] = 1;

    sprintf(qval[ACCESS_GROUP_NAME]," = '%s'", collection);

#ifdef INSTRON
    beforeTime =  gethrtime ();
#endif
    status = srbGetDataDirInfo(prelConn, MDAS_CATALOG, qval, selval,
      myresult, DEF_NUM_ROWS);
#ifdef INSTRON
    fprintf (stderr, "srbCollStat: query time = %lld\n",
     gethrtime () - beforeTime);
#endif

    return (status);
}

int
srbGetdents (int fd, void *buf, size_t nbyte, int is64Flag) 
{
    int continueIndex = -1;
    int status = 0;
    int rowCnt = 0;
    char *nameList;
    int loopAgain;

    do {

      loopAgain = 0;
      switch (srbFdArray[fd].isDir) {
      case IS_DIR_1: 
        /* new desc */
	rowCnt = getDataInColl (srbFdArray[fd].path, MAX_RET_CNT,  
	 &nameList, &(srbFdArray[fd].continueIndex));
	if (rowCnt <= 0) {
	    setFdToStage3 (fd);
            loopAgain = 1;        /* try to list subcollection */
            break;
	}
	srbFdArray[fd].gotData = 1;
	if (is64Flag== IS_64_BIT)
	    status = srbResToDirent64 (nameList, rowCnt, fd, 1);
	else
	    status = srbResToDirent (nameList, rowCnt, fd, 1);
	free (nameList);
	if (status <= 0) { 
	    setFdToStage3 (fd);
            loopAgain = 1;        /* try to list subcollection */
            break;
	}

	if (is64Flag== IS_64_BIT)
	    status = outputDirent64 (fd, buf, nbyte);
	else
	    status = outputDirent (fd, buf, nbyte);
	if (status <= 0) {
	    setFdToStage3 (fd);
            loopAgain = 1;        /* try to list subcollection */
            break;
	}
	srbFdArray[fd].isDir = IS_DIR_2;
	break;
      case IS_DIR_2:
	/* Continue listing data */
	
	if (srbFdArray[fd].maxDirLen > srbFdArray[fd].offset) { 	
	    /* still more data */
	    if (is64Flag== IS_64_BIT)
	        status = outputDirent64 (fd, buf, nbyte);
	    else
	        status = outputDirent (fd, buf, nbyte);
	    if (status > 0)
		break;
	}

	/* See if we can get more result */

	if (srbFdArray[fd].continueIndex >= 0) 
	    rowCnt = getMoreData (&srbFdArray[fd].continueIndex, MAX_RET_CNT,  
	     DATA_NAME, &nameList);
	if (rowCnt <= 0) {
	    setFdToStage3 (fd);
            loopAgain = 1;        /* try to list subcollection */
            break;
	} 
	if (is64Flag== IS_64_BIT)
	    status = srbResToDirent64 (nameList, rowCnt, fd, 0);
	else
	    status = srbResToDirent (nameList, rowCnt, fd, 0);
	free (nameList);
	if (status <= 0) { 
	    setFdToStage3 (fd);
            loopAgain = 1;        /* try to list subcollection */
            break;
	}

	if (is64Flag== IS_64_BIT)
	    status = outputDirent64 (fd, buf, nbyte);
	else
	    status = outputDirent (fd, buf, nbyte);
	if (status <= 0) {
	    setFdToStage3 (fd);
            loopAgain = 1;        /* try to list subcollection */
            break;
	}
	break;
      case IS_DIR_3:
	/* done listing data. now try to list collection */

	rowCnt = getCollInColl (srbFdArray[fd].path, MAX_RET_CNT,  
	 &nameList, &(srbFdArray[fd].continueIndex));
	
	if (rowCnt < 0)
	    rowCnt = 0;
	if (rowCnt == 0 && srbFdArray[fd].gotData == 1)  {
	    status = 0;
	    break;
	} 
	if (is64Flag== IS_64_BIT)
	    if (srbFdArray[fd].gotData == 1)
	    	status = srbResToDirent64 (nameList, rowCnt, fd, 0);
	    else
	    	status = srbResToDirent64 (nameList, rowCnt, fd, 1);
	else
	    if (srbFdArray[fd].gotData == 1)
	    	status = srbResToDirent (nameList, rowCnt, fd, 0);
	    else
	    	status = srbResToDirent (nameList, rowCnt, fd, 1);
	free (nameList);
	if (status <= 0) { 
	    break;
	}

	if (is64Flag== IS_64_BIT)
	    status = outputDirent64 (fd, buf, nbyte);
	else
	    status = outputDirent (fd, buf, nbyte);
	if (status <= 0) {
	    break;
	}
	srbFdArray[fd].isDir = IS_DIR_4;
	break;
      case IS_DIR_4:
	/* Continue listing Coll */
	
	if (srbFdArray[fd].maxDirLen > srbFdArray[fd].offset) { 	
	    /* still more data */
	    if (is64Flag== IS_64_BIT)
	        status = outputDirent64 (fd, buf, nbyte);
	    else
	        status = outputDirent (fd, buf, nbyte);
	    if (status > 0)
		break;
	}

	/* See if we can get more result */

	if (srbFdArray[fd].continueIndex >= 0) 
	    rowCnt = getMoreData (&srbFdArray[fd].continueIndex, MAX_RET_CNT,  
	     DATA_GRP_NAME, &nameList);
	if (rowCnt <= 0) {
	    break;
	} 
	if (is64Flag== IS_64_BIT)
	    status = srbResToDirent64 (nameList, rowCnt, fd, 0);
	else
	    status = srbResToDirent (nameList, rowCnt, fd, 0);
	free (nameList);
	if (status <= 0) { 
	    break;
	}

	if (is64Flag== IS_64_BIT)
	    status = outputDirent64 (fd, buf, nbyte);
	else
	    status = outputDirent (fd, buf, nbyte);
	    break;
      default:
        errno = EBADF;
	status = 0;
      }
    } while (loopAgain > 0);
    return status;
}

int
setFdToStage3 (int fd) 
{
    srbFdArray[fd].isDir = IS_DIR_3;
    srbFdArray[i].continueIndex = -1;
    return (0);
}

int
getDataInColl (char *parColl, int maxRow, char **nameList, 
int *continueIndex)
{
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i;
    int status;
    mdasC_sql_result_struct myresult;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }

    selval[DATA_NAME] = 1;
    selval[DATA_ID] = 1;
    sprintf(qval[DATA_GRP_NAME]," = '%s'", parColl);

    status = srbGetDataDirInfo(prelConn, MDAS_CATALOG, qval, selval,
      &myresult, maxRow);

    if (status >= 0 && myresult.row_count > 0) {
	*nameList = (char *) getFromResultStruct (&myresult,
         dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
	*continueIndex = myresult.continuation_index;
    } else {
	*nameList = NULL;
	*continueIndex = -1;
    }
    return (myresult.row_count);
}

int
getCollInColl (char *parColl, int maxRow, char **nameList, int *continueIndex)
{
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i;
    int status;
    mdasC_sql_result_struct myresult;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }

    selval[DATA_GRP_NAME] = 1;
    sprintf(qval[PARENT_COLLECTION_NAME]," = '%s'", parColl);

    status = srbGetDataDirInfo(prelConn, MDAS_CATALOG, qval, selval,
      &myresult, maxRow);

    if (status >= 0 && myresult.row_count > 0) {
	*nameList = (char *) getFromResultStruct (&myresult,
         dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
	*continueIndex = myresult.continuation_index;
    } else {
	*nameList = NULL;
	*continueIndex = -1;
    }
    return (myresult.row_count);
}
    
/* srbResToDirent() - Convert the query result to dirent and return the length 
 * of dirent. 
 */

int 
srbResToDirent (char *nameList, int rowCnt, int fd, int firstFlag)
{
    int i;
    dirent_t *mydirent;
    int mallocLen = 0;
    int direntLen = 0;
    int tmpLen;
    
    /* do some bookkeeping */

    if (srbFdArray[fd].maxDirLen > srbFdArray[fd].offset) {
	fprintf (stderr, 
	 "srbResToDirent: Internal error, maxDirLen > offset\n");
	return 0;
    }

    if (nameList == NULL && rowCnt > 0)
	return 0;

    srbFdArray[fd].accumOffset += srbFdArray[fd].maxDirLen;
    srbFdArray[fd].maxDirLen = srbFdArray[fd].offset = 0;
    if (srbFdArray[fd].dirent != NULL) {
	free (srbFdArray[fd].dirent);
	srbFdArray[fd].dirent = NULL;
    }


    /* estimate the no of bytes to alloc */

    for (i = 0; i < rowCnt; i++) {
	mallocLen += (strlen (&nameList[i*MAX_DATA_SIZE]) + 1) /4 * 4 + 4;
    }

    /* first time around, have to add "." and ".." */
    if (firstFlag == 1) {
	/* add 9 bytes for padding */
	mallocLen += (rowCnt + 2)  * sizeof (dirent_t);
    } else {
    	/* add 9 bytes for padding */
    	mallocLen += rowCnt * (sizeof (dirent_t));
    }

    srbFdArray[fd].dirent = malloc (mallocLen);

    /* pack the dirent */

    mydirent = (dirent_t *) srbFdArray[fd].dirent;

    if (firstFlag == 1) {
	/* hand craft "." and ".." */
	mydirent->d_ino = FAKE_INODE;
        tmpLen = mydirent->d_reclen = sizeof (dirent_t);
        direntLen += tmpLen;
        mydirent->d_off = srbFdArray[fd].accumOffset + direntLen;
        strcpy (mydirent->d_name, ".");
        mydirent = (dirent_t *) ((char *) mydirent + tmpLen);

        mydirent->d_ino = FAKE_INODE;
        tmpLen = mydirent->d_reclen = sizeof (dirent_t);
        direntLen += tmpLen;
        mydirent->d_off = srbFdArray[fd].accumOffset + direntLen;
        strcpy (mydirent->d_name, "..");
        mydirent = (dirent_t *) ((char *) mydirent + tmpLen);
    }

    for (i = 0; i < rowCnt; i++) {
	char lastName[MAX_TOKEN], tmpColl[MAX_TOKEN];

	splitpath (&nameList[i*MAX_DATA_SIZE], tmpColl, lastName);
	/* this take care of "/XYZ" type path */
	if (lastName[0] == '\0' && tmpColl[0] == '/')
	    strcpy (lastName, &tmpColl[1]);
	tmpLen = mydirent->d_reclen = (sizeof (dirent_t) + 
	  strlen (lastName)) / 4 * 4 + 4;
	direntLen += tmpLen;
	/* fake some no for d_ino and d_off */
	mydirent->d_ino = FAKE_INODE;
        mydirent->d_off = srbFdArray[fd].accumOffset + direntLen;
	if (direntLen > mallocLen) {
	    fprintf (stderr, "srbResToDirent: direntLen > mallocLen\n");
	    errno = EINVAL;
	    free (srbFdArray[fd].dirent);
	    srbFdArray[fd].dirent = NULL;
	    return 0;
	}
	strcpy (mydirent->d_name, lastName);
	    
	mydirent = (dirent_t *) ((char *) mydirent + tmpLen);
    }
    srbFdArray[fd].maxDirLen = direntLen;
    srbFdArray[fd].offset = 0;
    return (direntLen);
}
	
int 
srbResToDirent64 (char *nameList, int rowCnt, int fd, int firstFlag)
{
    int i;
    dirent64_t *mydirent;
    int mallocLen = 0;
    int direntLen = 0;
    int tmpLen;
    
    /* do some bookkeeping */

    if (srbFdArray[fd].maxDirLen > srbFdArray[fd].offset) {
	fprintf (stderr, 
	 "srbResToDirent: Internal error, maxDirLen > offset\n");
	return 0;
    }

    if (nameList == NULL && rowCnt > 0)
	return 0;

    srbFdArray[fd].accumOffset += srbFdArray[fd].maxDirLen;
    srbFdArray[fd].maxDirLen = srbFdArray[fd].offset = 0;
    if (srbFdArray[fd].dirent != NULL) {
	free (srbFdArray[fd].dirent);
	srbFdArray[fd].dirent = NULL;
    }


    /* estimate the no fo bytes to alloc */

    for (i = 0; i < rowCnt; i++) {
	mallocLen += (strlen (&nameList[i*MAX_DATA_SIZE]) + 1) /8 * 8 + 8;
    }

    /* first time around, have to add "." and ".." */
    if (firstFlag == 1) {
        /* add 9 bytes for padding */
        mallocLen += (rowCnt + 2)  * sizeof (dirent64_t);
    } else {
        /* add 9 bytes for padding */
        mallocLen += rowCnt * sizeof (dirent64_t);
    }

    srbFdArray[fd].dirent = malloc (mallocLen);

    /* pack the dirent */

    mydirent = (dirent64_t *) srbFdArray[fd].dirent;

    if (firstFlag == 1) {
        /* hand craft "." and ".." */
        mydirent->d_ino = FAKE_INODE;
        tmpLen = mydirent->d_reclen = sizeof (dirent64_t) ;
        direntLen += tmpLen;
        mydirent->d_off = srbFdArray[fd].accumOffset + direntLen;
        strcpy (mydirent->d_name, ".");
        mydirent = (dirent64_t *) ((char *) mydirent + tmpLen);

        mydirent->d_ino = FAKE_INODE;
        tmpLen = mydirent->d_reclen = sizeof (dirent64_t);
        direntLen += tmpLen;
        mydirent->d_off = srbFdArray[fd].accumOffset + direntLen;
        strcpy (mydirent->d_name, "..");
        mydirent = (dirent64_t *) ((char *) mydirent + tmpLen);
    }

    for (i = 0; i < rowCnt; i++) {
	char lastName[MAX_TOKEN], tmpColl[MAX_TOKEN];

	splitpath (&nameList[i*MAX_DATA_SIZE], tmpColl, lastName);
	/* this take care of "/XYZ" type path */
	if (lastName[0] == '\0' && tmpColl[0] == '/')
	    strcpy (lastName, &tmpColl[1]);
	tmpLen = mydirent->d_reclen = (sizeof (dirent64_t) + 
	  strlen (lastName) + 1) / 8 * 8 + 8;
	direntLen += tmpLen;
	/* fake some number for d_ino & d_off */
	mydirent->d_ino = FAKE_INODE;
        mydirent->d_off = srbFdArray[fd].accumOffset + direntLen;
	if (direntLen > mallocLen) {
	    fprintf (stderr, "srbResToDirent64: direntLen > mallocLen\n");
	    errno = EINVAL;
	    free (srbFdArray[fd].dirent);
	    srbFdArray[fd].dirent = NULL;
	    return 0;
	}
	strcpy (mydirent->d_name, lastName);
	    
	mydirent = (dirent64_t *) ((char *) mydirent + tmpLen);
    }
    srbFdArray[fd].maxDirLen = direntLen;
    srbFdArray[fd].offset = 0;
    return (direntLen);
}
	
int
getMoreData (int *continueIndex, int maxCnt, int mcatIndex, char **nameList)
{
    int i;
    int status;
    mdasC_sql_result_struct myresult;

    if (*continueIndex < 0) {
	*nameList = NULL;
	return 0;
    }
    
    status = srbGetMoreRows(prelConn, MDAS_CATALOG, *continueIndex,
      &myresult, maxCnt);

    if (status >= 0 && myresult.row_count > 0) {
	*nameList = (char *) getFromResultStruct (&myresult,
         dcs_tname[mcatIndex], dcs_aname[mcatIndex]);
	*continueIndex = myresult.continuation_index;
    } else {
	*nameList = NULL;
    }
    return (myresult.row_count);
}
    
int 
outputDirent (int fd, dirent_t *buf, int bufLen)
{
    int i;
    dirent_t *direntStart, *mydirent;
    int direntLen = 0;
    int copyLen;
    

    direntLen = srbFdArray[fd].maxDirLen - srbFdArray[fd].offset;

    if (direntLen <= 0) 
	return 0;

    /* pack the dirent */

    mydirent = (dirent_t *) srbFdArray[fd].dirent;
    mydirent = (dirent_t *) ((char *) mydirent + srbFdArray[fd].offset);
    direntStart = mydirent;

    if (bufLen >= direntLen) {
	copyLen = direntLen;
    } else {
	copyLen = 0;
	while (copyLen + mydirent->d_reclen <= direntLen &&
	 copyLen + mydirent->d_reclen <= bufLen) {

	    copyLen += mydirent->d_reclen;
    	    mydirent = (dirent_t *) ((char *) mydirent + 
	     mydirent->d_reclen);
	    if (copyLen == direntLen || copyLen == bufLen)
		break;
	}
    }
    if (copyLen > 0) {
	memcpy ((void *) buf, direntStart, copyLen); 
    }

    if (copyLen >= direntLen) {		/* all done */
        srbFdArray[fd].offset = srbFdArray[fd].maxDirLen;
    } else {
	srbFdArray[fd].offset += copyLen;
    }

    return (copyLen);
}
	
int 
outputDirent64 (int fd, dirent_t *buf, int bufLen)
{
    int i;
    dirent64_t *direntStart, *mydirent;
    int direntLen = 0;
    int copyLen;
    

    direntLen = srbFdArray[fd].maxDirLen - srbFdArray[fd].offset;

    if (direntLen <= 0) 
	return 0;

    /* pack the dirent */

    mydirent = (dirent64_t *) srbFdArray[fd].dirent;
    mydirent = (dirent64_t *) ((char *) mydirent + srbFdArray[fd].offset);
    direntStart = mydirent;

    if (bufLen >= direntLen) {
	copyLen = direntLen;
    } else {
	copyLen = 0;
	while (copyLen + mydirent->d_reclen <= direntLen &&
	 copyLen + mydirent->d_reclen <= bufLen) {

	    copyLen += mydirent->d_reclen;
	     mydirent = (dirent64_t *) ((char *) mydirent +
             mydirent->d_reclen);
	    if (copyLen == direntLen || copyLen == bufLen)
		break;
	}
    }
    if (copyLen > 0) {
	memcpy ((void *) buf, direntStart, copyLen); 
    }

    if (copyLen >= direntLen) {		/* all done */
        srbFdArray[fd].offset = srbFdArray[fd].maxDirLen;
    } else {
	srbFdArray[fd].offset += copyLen;
    }

    return (copyLen);
}

int 
stripSrbPath (char *inPath, char *outPath)
{
    char *tmpStr, *tmpStr1;
    int result = 0;

    tmpStr = inPath;
    while (tmpStr != NULL) {
	strcpy (outPath, tmpStr);
	if ((tmpStr1 = strstr (tmpStr, SRB_PREFIX_PATH)) == NULL)
	    break; 
	/* advance the pointers */
	outPath = outPath + ((int) tmpStr - (int) tmpStr1 + 1);
	tmpStr += strlen (SRB_PREFIX_PATH);
	result = 1;
    }
    return result;
}

int 
srbCollSeekl (int fd, off64_t where, int whence, int is64Flag )
{

    int targPos;
    int result;
    char buf[MY_BLKSIZE];

    if (whence == SEEK_SET) {
	targPos = where;
    } else if (whence == SEEK_CUR) {
	targPos = srbFdArray[fd].accumOffset + srbFdArray[fd].offset +
	 where;
    } else {
	targPos = -1;	/* assume to EOF */
    }

/*
    if (srbFdArray[fd].gotData > 0 &&
*/
    if (targPos >= srbFdArray[fd].accumOffset && targPos <= 
      srbFdArray[fd].accumOffset + srbFdArray[fd].maxDirLen) {
	/* in the same page */
	srbFdArray[fd].offset = targPos - srbFdArray[fd].accumOffset;
	return (targPos);
    } else if (targPos >= 0) {
	if (targPos < srbFdArray[fd].accumOffset) {	/* start over */
	    /* reset some parameter */
	    if (srbFdArray[fd].dirent != NULL) {
		free (srbFdArray[fd].dirent);
		srbFdArray[fd].dirent = NULL;
	    }
	    srbFdArray[fd].maxDirLen = srbFdArray[fd].offset = 
	    srbFdArray[fd].accumOffset = 0;
	    srbFdArray[i].continueIndex = -1;
	    srbFdArray[i].isDir = IS_DIR_1;
	}
	while ((result = srbGetdents (fd, buf, MY_BLKSIZE, is64Flag)) > 0) {
	    if (targPos <= srbFdArray[fd].accumOffset + 
	     srbFdArray[fd].maxDirLen) {
		srbFdArray[fd].offset = targPos - srbFdArray[fd].accumOffset;
		return (targPos);
	    }
	}
	return -1;	/* can't get there */
    } else {	/*	SEEK_END */
	targPos = srbFdArray[fd].accumOffset +  srbFdArray[fd].offset;
	/* seek to the end */
	 while ((result = srbGetdents (fd, buf, MY_BLKSIZE, is64Flag)) > 0) {
	    targPos += result;
	}
	return (targPos);
    }
}
	
#if defined(PORTNAME_linux)

int 
myxstat64_conv (struct kernel_stat *kbuf, struct stat64* buf)
{
#ifdef _HAVE___PAD1
        buf->__pad1 = 0;
#endif
        buf->st_ino = kbuf->st_ino;
        buf->st_mode = kbuf->st_mode;
        buf->st_nlink = kbuf->st_nlink;
        buf->st_uid = kbuf->st_uid;
        buf->st_gid = kbuf->st_gid;
        buf->st_rdev = kbuf->st_rdev;
#ifdef _HAVE___PAD2
        buf->__pad2 = 0;
#endif
        buf->st_size = kbuf->st_size;
        buf->st_blksize = kbuf->st_blksize;
        buf->st_blocks = kbuf->st_blocks;
        buf->st_atime = kbuf->myst_atime;
#ifdef foo_HAVE___UNUSED1
        buf->__unused1 = 0;
#endif
        buf->st_mtime = kbuf->myst_mtime;
#ifdef foo_HAVE___UNUSED2
        buf->__unused2 = 0;
#endif
        buf->st_ctime = kbuf->myst_ctime;
#ifdef foo_HAVE___UNUSED3
        buf->__unused3 = 0;
#endif
	return 0;
}

int 
myxstat_conv (struct kernel_stat *kbuf, struct stat* buf)
{
        /* Convert to current kernel version of `struct stat'.  */
        buf->st_dev = kbuf->st_dev;
#ifdef _HAVE___PAD1
        buf->__pad1 = 0;
#endif
        buf->st_ino = kbuf->st_ino;
        buf->st_mode = kbuf->st_mode;
        buf->st_nlink = kbuf->st_nlink;
        buf->st_uid = kbuf->st_uid;
        buf->st_gid = kbuf->st_gid;
        buf->st_rdev = kbuf->st_rdev;
#ifdef _HAVE___PAD2
        buf->__pad2 = 0;
#endif
        buf->st_size = kbuf->st_size;
        buf->st_blksize = kbuf->st_blksize;
        buf->st_blocks = kbuf->st_blocks;
        buf->st_atime = kbuf->myst_atime;
#ifdef foo_HAVE___UNUSED1
        buf->__unused1 = 0;
#endif
        buf->st_mtime = kbuf->myst_mtime;
#ifdef foo_HAVE___UNUSED1
        buf->__unused1 = 0;
#endif
        buf->st_mtime = kbuf->myst_mtime;
#ifdef foo_HAVE___UNUSED2
        buf->__unused2 = 0;
#endif
        buf->st_ctime = kbuf->myst_ctime;
#ifdef foo_HAVE___UNUSED3
        buf->__unused3 = 0;
#endif
#ifdef foo_HAVE___UNUSED4
        buf->__unused4 = 0;
#endif
#ifdef foo_HAVE___UNUSED5
        buf->__unused5 = 0;
#endif
	return 0;
}

#endif	/* PORTNAME_linux */
