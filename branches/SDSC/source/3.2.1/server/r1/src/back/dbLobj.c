/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/* dbLobj.c - Routines to handle DataBase Large Obj request */

#include "dbLobj.h"

#if defined(PORTNAME_linux)
/* Soothe the compiler */
char *mySplitStr(char *st1, char *st2, char *st3, char key);
#endif


/* initDbLObjDesc - Initialize the gDbLobjDesc 
 *
 * Input - None
 *
 * Output - None
 */

void initDbLObjDesc()
{
    int i;
 
    for (i = 0; i < MAX_DB_LOBJ_DESC; i++) {
        gDbLobjDesc[i].fd = -1;
        gDbLobjDesc[i].remoteFlag = 0;
        gDbLobjDesc[i].hostTabPtr = NULL;
        gDbLobjDesc[i].dbSpecificInfo = NULL;
    }
}

/* newDbLObjDesc - Get a free gDbLobjDesc.
 *
 * Input - None
 *
 * Output - Returns the index of a free gDbLobjDesc.
 *	    Returns negative - error
 */

static int
newDbLObjDesc (struct hostElement *hostTabPtr, int remoteFlag, 
int dbSystemType, int fd)
{
    int i;
 
    for (i = 0; i < MAX_DB_LOBJ_DESC; i++) {
        if (gDbLobjDesc[i].fd == -1) {
	    gDbLobjDesc[i].fd = fd;
	    gDbLobjDesc[i].hostTabPtr = hostTabPtr;
	    gDbLobjDesc[i].remoteFlag = remoteFlag;
	    gDbLobjDesc[i].dbSystemType = dbSystemType;
            return (i);
        }
    }
    elog(NOTICE,"newDbLObjDesc: Unable to allocate gDbLobjDesc descriptor");
    return SYS_ERR_DESC_OVERFLOW;
}
 
/* delDbLObjDesc - Put a gDbLobjDesc back to the free pool.
 *
 * Input : int i - The index of the gDbLobjDesc to be freed.
 *
 * Output - None.
 */

static void
delDbLObjDesc (int i) {

    gDbLobjDesc[i].fd = -1;
    gDbLobjDesc[i].remoteFlag = 0;
    gDbLobjDesc[i].dbSpecificInfo = NULL;
    gDbLobjDesc[i].hostTabPtr = NULL;
}

/* dbLobjOpen () - Open a DB LOBJ.
 *
 * Input : int dbSystemType - The type of DataBase system
 *	   char *resourceLoc - The resource location. Noramlly, the
 *	   host address where the dataBase is located.
 *	   char *dataPath - The Path to the Lobj.
 *	   int openFlag - The open flag (Same definition as in Unix)
 *	   int mode - The mode (Same definition as in Unix)
 * 	   (For DB2/Illustra type LOBJ, openFlag and mode are not used.) 
 *	   char *userName - The user name (not used for now).
 *
 * Output : The Index of DbLObjDesc for the opened Lobj.
 */

int
dbLobjOpen(int dbSystemType, struct varlena  *resourceLoc, 
struct varlena *dataPath, int openFlag, int mode)
{
    int descInx;
    char *resourceLocBuf, *dataPathBuf, *tempStr;

    if (TicketUserFlag && ProxyUser->privUserFlag == 0) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    resourceLocBuf = varToStr (resourceLoc);
    dataPathBuf = varToStr (dataPath);

    /* Convert to full path.
     */

    dataPathBuf = dbLobjPath2FullPath (dataPathBuf);

    /* Use SRB_SERVER_USER for now. */

    descInx = _dbLobjOpen(dbSystemType, resourceLocBuf, 
	dataPathBuf, openFlag, mode, SRB_SERVER_USER);

    free (resourceLocBuf);
    free (dataPathBuf);

    return (descInx);
}

int
_dbLobjOpen(int dbSystemType, char *resourceLoc, char *dataPath, 
int openFlag, int mode, int dbUserFlag)
{
    int descInx;
    int  retVal;
    int remoteFlag;
    char hostAddr[STRING_LEN1], tmpStr[STRING_LEN1];
    struct hostElement *hostTabPtr;
    char *dbUserName;
   
    if (mySplitStr (resourceLoc, hostAddr, tmpStr, ':') == NULL) {
	elog (NOTICE, "dbLobjOpen: Illegal resourceLocation format: %s",
        resourceLoc);
        return INP_ERR_RES_FORMAT;
    }
  
    /* Check whether the host is remote or local. Note: hostAddr now contains
     * only the host name */
 
#ifdef FED_MCAT
    remoteFlag = chkHostAndZone (hostAddr, &hostTabPtr);
#else
    remoteFlag = chkHost (hostAddr, NULL, &hostTabPtr);
#endif

     
    if (remoteFlag < 0) {
        elog (NOTICE, "Invalid host addr %s", hostAddr);
        return INP_ERR_HOST_ADDR;              /* failure */
    } else if (remoteFlag == REMOTE_HOST) {
#ifdef DEMO
        elog (DEMOM, 
	  "_dbLobjOpen: %s is a remote host. Will do a remoteDbLobjOpen",
            hostAddr);
#endif
        retVal = remoteDbLobjOpen (dbSystemType, hostTabPtr, resourceLoc, 
	  dataPath, openFlag, mode);
	if (retVal < 0)
	    return retVal;
	descInx = newDbLObjDesc (hostTabPtr, REMOTE_HOST, dbSystemType, retVal);
	if (descInx < 0)
	    return descInx;
	
    } else if (remoteFlag == LOCAL_HOST) {

#ifdef DEMO
        elog (DEMOM, "_dbLobjOpen: %s is a local host. Will Open Lobj here.",
            hostAddr);
        elog (DEMOM, "_dbLobjOpen: DB type= %d, Path Name = %s",
            dbSystemType, dataPath);
#endif
/* XXXX - not used for now
	if (dbUserFlag == SRB_SERVER_USER) {
	    dbUserName = ThisUser->userName;
	} else { 
	    dbUserName = ThisUser->userName;
	}
*/

    	switch (dbSystemType) {
#ifdef DB_DB2
          case DB2_LOBJ:
    	    descInx = newDbLObjDesc (hostTabPtr, LOCAL_HOST, dbSystemType, 0);
	    if (descInx < 0) return descInx;

            if (Db2User == NULL) {   /* read from config file */
                Db2User = readLobjConfig (DB2_USER_KEYWD);
                Db2Passwd = readLobjConfig (DB2_PASSWD_KEYWD);
            }
    	    retVal = srbC_db2_open (&gDbLobjDesc[descInx], 
	    resourceLoc, dataPath, Db2User, Db2Passwd);
    	    if (retVal != 0) { 	/* open failed */
                elog(NOTICE,"cannot open DB2 LOBJ. resLoc=%s, dataPath=%s, status=%d", 
	        resourceLoc, dataPath, retVal);
                return retVal;
	    }
	    break;
#endif

#ifdef DB_Oracle
          case ORA_LOBJ:
            descInx = newDbLObjDesc (hostTabPtr, LOCAL_HOST, dbSystemType, 0);
            if (descInx < 0) return descInx;
	    if (OracleUser == NULL) {	/* read from config file */
		OracleUser = readLobjConfig (ORACLE_USER_KEYWD);
		OraclePasswd = readLobjConfig (ORACLE_PASSWD_KEYWD);
	    }
            retVal = srbC_ora_open (&gDbLobjDesc[descInx],
            resourceLoc, dataPath, OracleUser, OraclePasswd);
            if (retVal != 0) {  /* open failed */
                elog(NOTICE,
		  "cannot open ORA LOBJ. resLoc=%s, dataPath=%s, status=%d",
                    resourceLoc, dataPath, retVal);
                return retVal;
            }
            break;
#endif

#ifdef DB_Illustra
          case ILLUSTRA_LOBJ:
            descInx = newDbLObjDesc (hostTabPtr, LOCAL_HOST, dbSystemType, 0);
	    if (descInx < 0) return descInx;

            if (IllusUser == NULL) {   /* read from config file */            
                IllusUser = readLobjConfig (ILLUS_USER_KEYWD);
                IllusPasswd = readLobjConfig (ILLUS_PASSWD_KEYWD);
            }
	    retVal = illustraOpen (&gDbLobjDesc[descInx], 
   	             resourceLoc, dataPath, IllusUser, IllusPasswd);
    	    if (retVal != 0) { 	/* open failed */
                elog(NOTICE,
		"cannot open Illustra LOBJ resLoc=%s,dataPath=%s, userName=%s", 
	        resourceLoc, dataPath, dbUserName);
                return retVal;
	    }
	    break;
#endif
          default:                /* shouldn't get here */
            elog(NOTICE, "dbLobjOpen: error in dbSystemType mapping, %d",
	    dbSystemType);
	    return INP_ERR_DB_TYPE;
	}
    }
    return descInx;
}

/* dbLobjCreate () - Create a DB LOBJ.
 *
 * Input : int dbSystemType - The type of DataBase system
 *	   char *resourceLoc - The resource location. Noramlly, the
 *	   host address where the dataBase is located.
 *	   char *dataPath - The Path to the Lobj.
 *	   int mode - The mode (Same definition as in Unix)
 * 	   (For DB2/Illustra type LOBJ, mode is not used.) 
 *
 * Output : The Index of DbLObjDesc for the createed Lobj.
 */

int
dbLobjCreate(int dbSystemType, struct varlena *resourceLoc, 
struct varlena *dataPath, int mode)
{
    int descInx;
    char *resourceLocBuf, *dataPathBuf;
 
    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    resourceLocBuf = varToStr (resourceLoc);
    dataPathBuf = varToStr (dataPath);

    /* Convert to full path.
     */

    dataPathBuf = dbLobjPath2FullPath (dataPathBuf);

    descInx = _dbLobjCreate(dbSystemType, resourceLocBuf, 
        dataPathBuf, mode, SRB_SERVER_USER);
 
    free (resourceLocBuf);
    free (dataPathBuf);

    return (descInx);
}

int
_dbLobjCreate(int dbSystemType, char *resourceLoc, char *dataPath, 
int mode, int dbUserFlag)
{
    int descInx;
    int  retVal;
    int remoteFlag;
    char hostAddr[STRING_LEN1], tmpStr[STRING_LEN1];
    struct hostElement *hostTabPtr;
    char *dbUserName;
   
    if (mySplitStr (resourceLoc, hostAddr, tmpStr, ':') == NULL) {
	elog (NOTICE, "dbLobjCreate: Illegal resourceLocation format: %s",
        resourceLoc);
        return INP_ERR_RES_FORMAT;
    }
  
    /* Check whether the host is remote or local. Note: hostAddr now contains
     * only the host name */
 
#ifdef FED_MCAT
    remoteFlag = chkHostAndZone (hostAddr, &hostTabPtr);
#else
    remoteFlag = chkHost (hostAddr, NULL, &hostTabPtr);
#endif

     
    if (remoteFlag < 0) {
        elog (NOTICE, "Invalid host addr %s", hostAddr);
        return INP_ERR_HOST_ADDR;              /* failure */
    } else if (remoteFlag == REMOTE_HOST) {
        retVal = remoteDbLobjCreate (dbSystemType, hostTabPtr, resourceLoc, 
	  dataPath, mode);
	if (retVal < 0)
	    return retVal;
	descInx = newDbLObjDesc (hostTabPtr, REMOTE_HOST, dbSystemType, retVal);
	if (descInx < 0)
	    return descInx;
    } else if (remoteFlag == LOCAL_HOST) {
	/* XXXXX not used for now
        if (dbUserFlag == SRB_SERVER_USER)
            dbUserName = ThisUser->userName;
        else
            dbUserName = ClientUser->userName;
	*/

    	switch (dbSystemType) {
#ifdef DB_DB2
          case DB2_LOBJ:
    	    descInx = newDbLObjDesc (hostTabPtr, LOCAL_HOST, dbSystemType, 0);
	    if (descInx < 0) return descInx;

            if (Db2User == NULL) {   /* read from config file */
                Db2User = readLobjConfig (DB2_USER_KEYWD);
                Db2Passwd = readLobjConfig (DB2_PASSWD_KEYWD);
            }
    	    retVal = srbC_db2_create (&gDbLobjDesc[descInx], 
	    resourceLoc, dataPath, Db2User, Db2Passwd);
    	    if (retVal != 0) { 	/* create failed */
                elog(NOTICE,
		  "cannot create DB2 LOBJ. resLoc=%s dataPath=%s status=%d", 
	            resourceLoc, dataPath, retVal);
                return retVal;
	    }
	    break;
#endif

#ifdef DB_Oracle
          case ORA_LOBJ:
            descInx = newDbLObjDesc (hostTabPtr, LOCAL_HOST, dbSystemType, 0);
            if (descInx < 0) return descInx;

            if (OracleUser == NULL) {   /* read from config file */
                OracleUser = readLobjConfig (ORACLE_USER_KEYWD);
                OraclePasswd = readLobjConfig (ORACLE_PASSWD_KEYWD);
            }
            retVal = srbC_ora_create (&gDbLobjDesc[descInx],
            resourceLoc, dataPath, OracleUser, OraclePasswd);
            if (retVal != 0) {  /* create failed */
                elog(NOTICE,
                  "cannot create ORA LOBJ. resLoc=%s dataPath=%s status=%d",
                    resourceLoc, dataPath, retVal);
                return retVal;
            }
            break;
#endif

#ifdef DB_Illustra
          case ILLUSTRA_LOBJ:
            descInx = newDbLObjDesc (hostTabPtr, LOCAL_HOST, dbSystemType, 0);
	    if (descInx < 0) return descInx;

            if (IllusUser == NULL) {   /* read from config file */
                IllusUser = readLobjConfig (ILLUS_USER_KEYWD);
                IllusPasswd = readLobjConfig (ILLUS_PASSWD_KEYWD);
            }
	    retVal = illustraCreate (&gDbLobjDesc[descInx], 
   	             resourceLoc, dataPath, IllusUser, IllusPasswd);
    	    if (retVal != 0) { 	/* create failed */
                elog(NOTICE,"cannot create Illustra LOBJ resLoc=%s,dataPath=%s",
	        resourceLoc, dataPath);
                return retVal;
	    }
	    break;
#endif
          default:                /* shouldn't get here */
            elog(NOTICE, "dbLobjCreate: error in dbSystemType mapping, %d",
	    dbSystemType);
	    return INP_ERR_DB_TYPE;
	}
    }
    return descInx;
}

/* dbLobjClose () - Close a DB LOBJ.
 *
 * Input : int descInx - The Index of DbLObjDesc to close
 *
 * Output : Returns 0 - success
 *	           < 0 - failure
 */

int
dbLobjClose (int descInx)
{
    srbConn* conn;
    int retval;

    if (descInx < 0 || descInx >= MAX_DB_LOBJ_DESC ||
    gDbLobjDesc[descInx].fd < 0) {
        elog(NOTICE,"dbLobjClose: fd (%d) out of range", descInx);
        return INP_ERR_DESC;
    }

    if (gDbLobjDesc[descInx].remoteFlag == REMOTE_HOST) {
        conn = getRemoteConn (gDbLobjDesc[descInx].hostTabPtr);
        if (conn == NULL)
            return SYS_ERR_REMOTE_CONN;
        retval = srbDbLobjClose (conn, gDbLobjDesc[descInx].fd);
    } else {
    	switch (gDbLobjDesc[descInx].dbSystemType) {
#ifdef DB_DB2
          case DB2_LOBJ:
            retval = srbC_db2_close (&gDbLobjDesc[descInx]);
            break;
#endif

#ifdef DB_Oracle
          case ORA_LOBJ:
            retval = srbC_ora_close (&gDbLobjDesc[descInx]);
            break;
#endif

#ifdef DB_Illustra
          case ILLUSTRA_LOBJ:
            retval = illustraClose (&gDbLobjDesc[descInx]);
            break;
#endif
          default:                /* shouldn't get here */
            elog(NOTICE, "dbLobjClose: error in dbSystemType mapping, %d",
	    gDbLobjDesc[descInx].dbSystemType);
	    retval = INP_ERR_DB_TYPE;
            break;
	}
    }
    delDbLObjDesc (descInx);
    return retval;
}

/*
 * dbLobjRead - Read len bytes of the dbLobj into buf.
 *
 * Input - int fd_inx - The dbLobj descriptor to read (from dbLobjOpen or
 *         dbLobjCreate).
 *         char *buf - The input buffer.
 *         int len - The length to read.
 *
 * Output - Returns the number of bytes read..
 *          Returns negative upon failure.
 */
 
struct varlena *
dbLobjRead(int fd, int len)
{
    struct varlena *retval;
    int totalread = 0;
#if defined(PORTNAME_c90)
    struct int32Array *retInt;
#else
    int *retInt;
#endif
    int status;

    retval = (struct varlena *)malloc(SIZEOF32 * 2 + len);
    totalread = _dbLobjRead(fd, VARDATA(retval) + SIZEOF32, len);

    if (totalread >= 0) {
        status = 0;
    } else {
        status = totalread;
        totalread = 0;
    }
    VARSIZE(retval) = totalread + SIZEOF32 * 2;
#if defined(PORTNAME_c90)
    retInt = (struct int32Array *) retval;
    retInt->myInt1 = htonl (status);
#else
    retInt = (int *) VARDATA (retval);
    *retInt = htonl (status);
#endif
 
    return retval;
}

/* _dbLobjRead () - Perform a read operation on a DB Lobj.
 *
 * Input : int descInx - The Index of DbLObjDesc to read
 *	   char *buf - The input buffer
 *	   int len - The length to read.
 *
 * Output : Returns the number of bytes read.
 */

int
_dbLobjRead (int descInx, char *buf, int len)
{
    srbConn* conn;

    if (descInx < 0 || descInx >= MAX_DB_LOBJ_DESC ||
    gDbLobjDesc[descInx].fd < 0) {
        elog(NOTICE,"dbLobjRead: desc (%d) out of range", descInx);
        return INP_ERR_DESC;
    }

    if (gDbLobjDesc[descInx].remoteFlag == REMOTE_HOST) {
#ifdef DEMO
        elog (DEMOM, "_dbLobjRead: DB on remote host. Will do a remote read");
#endif
        conn = getRemoteConn (gDbLobjDesc[descInx].hostTabPtr);
        if (conn == NULL)
            return SYS_ERR_REMOTE_CONN;
        return srbDbLobjRead (conn, gDbLobjDesc[descInx].fd, buf, len);
    } else {

#ifdef DEMO
        elog (DEMOM, "_dbLobjRead: DB on local host. Will do a local read");
#endif
    	switch (gDbLobjDesc[descInx].dbSystemType) {
#ifdef DB_DB2
          case DB2_LOBJ:
            return (srbC_db2_read (&gDbLobjDesc[descInx], buf, len));
            break;
#endif

#ifdef DB_Oracle
          case ORA_LOBJ:
            return (srbC_ora_read (&gDbLobjDesc[descInx], buf, len));
            break;
#endif

#ifdef DB_Illustra
          case ILLUSTRA_LOBJ:
            return (illustraRead (&gDbLobjDesc[descInx], buf, len));
            break;
#endif
          default:                /* shouldn't get here */
            elog(NOTICE, "dbLobjRead: error in dbSystemType mapping, %d",
            gDbLobjDesc[descInx].dbSystemType);
	    return INP_ERR_DB_TYPE;
	}
    }
}
 
/* dbLobjWrite () - Perform a write operation on a DB Lobj.
 *
 * Input : int descInx - The Index of DbLObjDesc to write
 *	   struct varlena *buf - The input buffer
 *
 * Output : Returns the number of bytes write.
 */

int
dbLobjWrite (int descInx, struct varlena *buf)
{
    int totalwritten;
    int bytestowrite;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    bytestowrite = VARSIZE(buf) - SIZEOF32;
    totalwritten = _dbLobjWrite (descInx, VARDATA(buf), bytestowrite);
    return totalwritten;
}

int
_dbLobjWrite (int descInx, char *buf, int len)
{
    srbConn* conn;
    int status;

    if (descInx < 0 || descInx >= MAX_DB_LOBJ_DESC ||
    gDbLobjDesc[descInx].fd < 0) {
        elog(NOTICE,"dbLobjWrite: desc (%d) out of range", descInx);
        return INP_ERR_DESC;
    }

    if (gDbLobjDesc[descInx].remoteFlag == REMOTE_HOST) {
#ifdef DEMO
        elog (DEMOM, 
	  "dbLobjWrite: DB on remote host. Will do a remote write");
#endif
        conn = getRemoteConn (gDbLobjDesc[descInx].hostTabPtr);
        if (conn == NULL)
            return SYS_ERR_REMOTE_CONN;
        return srbDbLobjWrite (conn, gDbLobjDesc[descInx].fd, buf, len);
    } else {
#ifdef DEMO
        elog (DEMOM, "dbLobjWrite: DB on local host. Will do a local write");
#endif
    	switch (gDbLobjDesc[descInx].dbSystemType) {
#ifdef DB_DB2
          case DB2_LOBJ:
            status = srbC_db2_write (&gDbLobjDesc[descInx], buf, len);
	    if (status < 0)
	        elog (NOTICE, "dbLobjWrite: DB2 dbLobjWrite ERROR. status = %d",
	          status);
	    return (status);
            break;
#endif

#ifdef DB_Oracle
          case ORA_LOBJ:
            status = srbC_ora_write (&gDbLobjDesc[descInx], buf, len);
            if (status < 0)
                elog (NOTICE, "dbLobjWrite: ORA dbLobjWrite ERROR. status = %d",
                  status);
            return (status);
            break;
#endif

#ifdef DB_Illustra
          case ILLUSTRA_LOBJ:
            status = illustraWrite (&gDbLobjDesc[descInx], buf, len);
	    if (status < 0)
                elog (NOTICE, "dbLobjWrite: illustraWrite ERROR. status = %d",
                  status);
	    return (status);
            break;
#endif
          default:                /* shouldn't get here */
            elog(NOTICE, "dbLobjWrite: error in dbSystemType mapping, %d",
            gDbLobjDesc[descInx].dbSystemType);
	    return INP_ERR_DB_TYPE;
	}
    }
}
 
/* dbLobjSeek () - Perform a seek operation on a DB Lobj.
 *
 * Input : int descInx - The Index of DbLObjDesc to seek
 *         int offset - The position of the next operation
 *         int whence - Same definition as in Unix.
 *              SEEK_SET - pointer is set to the value of the Offset parameter.
 *              SEEK_CUR - pointer is set to its current location plus the
 *                      value of the Offset parameter.
 *              SEEK_END - pointer is set to the size of the file plus the
 *                      value of the Offset parameter.
 *
 * Output : Returns the number of bytes read.
 */

int
dbLobjSeek (int descInx, long offset, int whence)
{
    srbConn* conn;

    if (descInx < 0 || descInx >= MAX_DB_LOBJ_DESC ||
    gDbLobjDesc[descInx].fd < 0) {
        elog(NOTICE,"dbLobjSeek: desc (%d) out of range", descInx);
        return INP_ERR_DESC;
    }

    if (gDbLobjDesc[descInx].remoteFlag == REMOTE_HOST) {
        conn = getRemoteConn (gDbLobjDesc[descInx].hostTabPtr);
        if (conn == NULL)
            return SYS_ERR_REMOTE_CONN;
        return srbDbLobjSeek (conn, gDbLobjDesc[descInx].fd, offset, whence);
    } else {

    	switch (gDbLobjDesc[descInx].dbSystemType) {
#ifdef DB_DB2
          case DB2_LOBJ:
            return (srbC_db2_seek (&gDbLobjDesc[descInx], offset, whence));
            break;
#endif
#ifdef DB_Oracle
          case ORA_LOBJ:
            return (srbC_ora_seek (&gDbLobjDesc[descInx], offset, whence));
            break;
#endif
#ifdef DB_Illustra
          case ILLUSTRA_LOBJ:
            return (illustraSeek (&gDbLobjDesc[descInx], offset, whence));
            break;
#endif
          default:                /* shouldn't get here */
            elog(NOTICE, "dbLobjSeek: error in dbSystemType mapping, %d",
            gDbLobjDesc[descInx].dbSystemType);
	    return INP_ERR_DB_TYPE;
	}
    }
}

/*
 * There is a  delete for Illustra.
 */

/* dbLobjUnlink () - Unlink a DB LOBJ.
 *
 * Input : int dbSystemType - The type of DataBase system
 *         char *resourceLoc - The resource location. Noramlly, the
 *         host address where the dataBase is located.
 *         char *dataPath - The Path to the Lobj.
 *
 * Output : The Index of DbLObjDesc for the opened Lobj.
 */

int
dbLobjUnlink(int dbSystemType, struct varlena  *resourceLoc,
struct varlena *dataPath)
{
    int status;
    char *resourceLocBuf, *dataPathBuf;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    resourceLocBuf = varToStr (resourceLoc);
    dataPathBuf = varToStr (dataPath);

    /* Convert to full path.
     */

    dataPathBuf = dbLobjPath2FullPath (dataPathBuf);

    /* Use SRB_SERVER_USER for now. */

    status = _dbLobjUnlink(dbSystemType, resourceLocBuf,
        dataPathBuf, SRB_SERVER_USER);

    free (resourceLocBuf);
    free (dataPathBuf);

    return (status);
}

int
_dbLobjUnlink(int dbSystemType, char *resourceLoc, char *dataPath,
int dbUserFlag)
{
    int  retVal;
    int remoteFlag;
    char hostAddr[STRING_LEN1], tmpStr[STRING_LEN1];
    struct hostElement *hostTabPtr;
    char *dbUserName;

    if (mySplitStr (resourceLoc, hostAddr, tmpStr, ':') == NULL) {
        elog (NOTICE, "dbLobjUnlink: Illegal resourceLocation format: %s",
        resourceLoc);
        return INP_ERR_RES_FORMAT;
    }

    /* Check whether the host is remote or local. Note: hostAddr now contains
     * only the host name */

#ifdef FED_MCAT
    remoteFlag = chkHostAndZone (hostAddr, &hostTabPtr);
#else
    remoteFlag = chkHost (hostAddr, NULL, &hostTabPtr);
#endif


    if (remoteFlag < 0) {
        elog (NOTICE, "Invalid host addr %s", hostAddr);
        return INP_ERR_HOST_ADDR;              /* failure */
    } else if (remoteFlag == REMOTE_HOST) {
#ifdef DEMO
        elog (DEMOM,
          "_dbLobjUnlink: %s is a remote host. Will do a remoteDbLobjUnlink",
            hostAddr);
#endif
        retVal = remoteDbLobjUnlink (dbSystemType, hostTabPtr, resourceLoc,
          dataPath);

    } else if (remoteFlag == LOCAL_HOST) {

#ifdef DEMO
        elog (DEMOM, "_dbLobjUnlink: %s is a local host. Will Unlink Lobj here.",
            hostAddr);
        elog (DEMOM, "_dbLobjUnlink: DB type= %d, Path Name = %s",
            dbSystemType, dataPath);
#endif
	/* XXXX not used for now
        if (dbUserFlag == SRB_SERVER_USER)
            dbUserName = ThisUser->userName;
        else
            dbUserName = ClientUser->userName;
	*/

        switch (dbSystemType) {
#ifdef DB_DB2
          case DB2_LOBJ:
            if (Db2User == NULL) {   /* read from config file */
                Db2User = readLobjConfig (DB2_USER_KEYWD);
                Db2Passwd = readLobjConfig (DB2_PASSWD_KEYWD);
            }
            retVal = srbC_db2_unlink ( resourceLoc, dataPath, 
	      Db2User, Db2Passwd);
            if (retVal != 0) { 
                elog(NOTICE,
		"cannot unlink DB2 LOBJ. resLoc=%s, dataPath=%s, status=%d",
                resourceLoc, dataPath, retVal);
                return retVal;
            }
            break;
#endif

#ifdef DB_Oracle
          case ORA_LOBJ:
            if (OracleUser == NULL) {   /* read from config file */
                OracleUser = readLobjConfig (ORACLE_USER_KEYWD);
                OraclePasswd = readLobjConfig (ORACLE_PASSWD_KEYWD);
            }
            retVal = srbC_ora_unlink ( resourceLoc, dataPath, 
	      OracleUser, OraclePasswd);
            if (retVal != 0) {
                elog(NOTICE,
                "cannot unlink ORA LOBJ. resLoc=%s, dataPath=%s, status=%d",
                resourceLoc, dataPath, retVal);
                return retVal;
            }
            break;
#endif

#ifdef DB_Illustra
          case ILLUSTRA_LOBJ:
            if (IllusUser == NULL) {   /* read from config file */
                IllusUser = readLobjConfig (ILLUS_USER_KEYWD);
                IllusPasswd = readLobjConfig (ILLUS_PASSWD_KEYWD);
            }
            retVal = illustraDelete (resourceLoc, dataPath, IllusUser, 
	      IllusPasswd);
            if (retVal != 0) {  /* open failed */
                elog(NOTICE,
                "cannot delete Illustra LOBJ resLoc=%s,dataPath=%s, userName=%s",
                resourceLoc, dataPath, IllusUser);
                return retVal;
            }
            break;
#endif
          default:                /* shouldn't get here */
            elog(NOTICE, "_dbLobjUnlink: error in dbSystemType mapping, %d",
            dbSystemType);
            return INP_ERR_DB_TYPE;
        }
    }
    return retVal;
}

/* dbLobjPath2FullPath () - Convert relative LOBJ path to the default full path.
 * i.e., see if the input dataPath is in the tableName/outColName/inCondition
 * format. If not, use the following dataPath:
 * DEF_DBLOBJ_TABLENAME/DEF_LOBJ_COL_NAME/DEF_LOBJ_ID_COL_NAME=objectName
 */


char *
dbLobjPath2FullPath (char *dataPath)
{
    char *tempStr;

    /* see if the dataPath is in the for tableName/objectName.
     * If not, use the DEF_DBLOBJ_TABLENAME/objectName
     */

        if (checkString (dataPath, '/') != 0) {
            tempStr = (char *) malloc (strlen (dataPath) +
              strlen (DEF_LOBJ_COL_NAME) +
              strlen (DEF_LOBJ_ID_COL_NAME) +
              strlen (DEF_DBLOBJ_TABLENAME) + 6);
            strcpy (tempStr, DEF_DBLOBJ_TABLENAME);
            strcat (tempStr, "/");
            strcat (tempStr, DEF_LOBJ_COL_NAME);
            strcat (tempStr, "/");
            strcat (tempStr, DEF_LOBJ_ID_COL_NAME);
            strcat (tempStr, "='");
            strcat (tempStr, dataPath);
            strcat (tempStr, "'");
            free (dataPath);
            return (tempStr);
        } else {
            return (dataPath);
        }
}

char *
readLobjConfig (char *keyWd)
{
    FILE *file;
    char buf[MAXEXFPATH];
    char buf1[MAXEXFPATH];
    char *retVal;
    int i;

    strcpy(buf, getSrbDataDir());
    strcat(buf, "/");
    strcat(buf, LOBJ_CONFIG);

    file = fopen(buf, "r");

    buf[0] = '\0';
    buf1[0] = '\0';

    if (file) {

        /* get the string */

        while (fscanf(file, "%s '%[^']'", buf, buf1) != EOF) {

            if (!strcmp(buf, keyWd)) {
                if (strlen (buf1) > 0) {
                    retVal = malloc (strlen (buf1) + 1);
                    strcpy (retVal, buf1);
                    fclose (file);
                    return (retVal);
                }
            }
        }
	fclose (file);
    }
    return (NULL);
}



size_t
_dbLobjWriteSize(int descInx)
{
    srbConn* conn;
    int retval;
    db_state_info *stateInfo;
    stateInfo = (db_state_info *) gDbLobjDesc[descInx].dbSpecificInfo; 
    
    return (SYS_ERR_NO_SUPPORT); /* something wrong here :-) */
#ifdef foo
    if (descInx < 0 || descInx >= MAX_DB_LOBJ_DESC ||
    gDbLobjDesc[descInx].fd < 0) {
        elog(NOTICE,"_dbLobjWriteSize: fd (%d) out of range", descInx);
        return INP_ERR_DESC;
    }
    return stateInfo->lobsize;
#endif
}

