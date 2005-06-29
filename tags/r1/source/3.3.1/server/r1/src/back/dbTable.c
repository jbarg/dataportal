/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/* dbTable.c - Routines to handle DataBase Large Obj request */

#include "dbTable.h"

/* initDbTableDesc - Initialize the gDbTableDesc 
 *
 * Input - None
 *
 * Output - None
 */

void initDbTableDesc()
{
    int i;
 
    for (i = 0; i < MAX_DB_TABLE_DESC; i++) {
        gDbTableDesc[i].fd = -1;
        gDbTableDesc[i].remoteFlag = 0;
        gDbTableDesc[i].hostTabPtr = NULL;
        gDbTableDesc[i].dbSpecificInfo = NULL;
    }
}

/* newDbTableDesc - Get a free gDbTableDesc.
 *
 * Input - None
 *
 * Output - Returns the index of a free gDbTableDesc.
 *	    Returns negative - error
 */

static int
newDbTableDesc (struct hostElement *hostTabPtr, int remoteFlag, 
int dbSystemType, int fd)
{
    int i;
 
    for (i = 0; i < MAX_DB_TABLE_DESC; i++) {
        if (gDbTableDesc[i].fd == -1) {
	    gDbTableDesc[i].fd = fd;
	    gDbTableDesc[i].hostTabPtr = hostTabPtr;
	    gDbTableDesc[i].remoteFlag = remoteFlag;
	    gDbTableDesc[i].dbSystemType = dbSystemType;
            return (i);
        }
    }
    elog(NOTICE,"newDbTableDesc: Unable to allocate gDbTableDesc descriptor");
    return SYS_ERR_DESC_OVERFLOW;
}
 
/* delDbTableDesc - Put a gDbTableDesc back to the free pool.
 *
 * Input : int i - The index of the gDbTableDesc to be freed.
 *
 * Output - None.
 */

static void
delDbTableDesc (int i) {

    gDbTableDesc[i].fd = -1;
    gDbTableDesc[i].remoteFlag = 0;
    gDbTableDesc[i].dbSpecificInfo = NULL;
    gDbTableDesc[i].hostTabPtr = NULL;
}

/* dbTableOpen () - Open a DB TABLE.
 *
 * Input : int dbSystemType - The type of DataBase system
 *	   char *resourceLoc - The resource location. Noramlly, the
 *	   host address where the dataBase is located.
 *	   char *dataPath - The Path to the Table.
 *	   int openFlag - The open flag (Same definition as in Unix)
 *	   int mode - The mode (Same definition as in Unix)
 * 	   (For DB2/Illustra type TABLE, openFlag and mode are not used.) 
 *	   char *userName - The user name (not used for now).
 *
 * Output : The Index of DbTableDesc for the opened Table.
 */

int
dbTableOpen(int dbSystemType, struct varlena  *resourceLoc, 
struct varlena *dataPath, int openFlag, int mode)
{
    int descInx;
    char *resourceLocBuf, *dataPathBuf;

    if (TicketUserFlag && ProxyUser->privUserFlag == 0) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    resourceLocBuf = varToStr (resourceLoc);
    dataPathBuf = varToStr (dataPath);

    /* Convert to full path.
     */

    dataPathBuf = dbTablePath2FullPath (dataPathBuf);

    /* Use SRB_SERVER_USER for now. */

    descInx = _dbTableOpen(dbSystemType, resourceLocBuf, 
	dataPathBuf, openFlag, mode, SRB_SERVER_USER);

    free (resourceLocBuf);
    free (dataPathBuf);

    return (descInx);
}

int
_dbTableOpen(int dbSystemType, char *resourceLoc, char *dataPath, 
int openFlag, int mode, int dbUserFlag)
{
    int descInx;
    int  retVal;
    int remoteFlag;
    char hostAddr[STRING_LEN1], dbInstance[STRING_LEN1];
    struct hostElement *hostTabPtr;
    char tmpStr[MAX_TOKEN];

    if (mySplitStr (resourceLoc, hostAddr, dbInstance, ':') == NULL) {
	elog (NOTICE, "dbTableOpen: Illegal resourceLocation format: %s",
        resourceLoc);
        return INP_ERR_RES_FORMAT;
    }
  
    /* Check whether the host is remote or local. Note: hostAddr now contains
     * only the host name */
 
#ifdef FED_MCAT
    remoteFlag = chkHostAndZone (hostAddr, &hostTabPtr);
    mySplitStr (resourceLoc,tmpStr,resourceLoc, '&');
#else
    remoteFlag = chkHost (hostAddr, NULL, &hostTabPtr);
#endif
     
    if (remoteFlag < 0) {
        elog (NOTICE, "Invalid host addr %s", hostAddr);
        return INP_ERR_HOST_ADDR;              /* failure */
    } else if (remoteFlag == REMOTE_HOST) {
#ifdef DEMO
        elog (DEMOM, 
	  "_dbTableOpen: %s is a remote host. Will do a remoteDbTableOpen",
            hostAddr);
#endif
        retVal = remoteDbTableOpen (dbSystemType, hostTabPtr, resourceLoc, 
	  dataPath, openFlag, mode);
	if (retVal < 0)
	    return retVal;
	descInx = newDbTableDesc (hostTabPtr, REMOTE_HOST, dbSystemType, retVal);
	if (descInx < 0)
	    return descInx;
	
    } else if (remoteFlag == LOCAL_HOST) {

#ifdef DEMO
        elog (DEMOM, "_dbTableOpen: %s is a local host. Will Open Table here.",
            hostAddr);
        elog (DEMOM, "_dbTableOpen: DB type= %d, Path Name = %s",
            dbSystemType, dataPath);
#endif
/* XXXX - not used for now
	if (dbUserFlag == SRB_SERVER_USER) {
	    dbUserName = ThisUser->userName;
	} else { 
	    dbUserName = ClientUser->userName;
	}
*/

    	switch (dbSystemType) {
#ifdef DB_DB2
          case DB2_TABLE:
    	    descInx = newDbTableDesc (hostTabPtr, LOCAL_HOST, dbSystemType, 0);
	    if (descInx < 0) return descInx;
            if (Db2User == NULL) {  /*  read from config file */
                Db2User = readTableConfig (dbInstance, DB2_USER_KEYWD);
                Db2Passwd = readTableConfig (dbInstance, DB2_PASSWD_KEYWD);
                Db2Home = readTableConfig (dbInstance, DB2_HOME_KEYWD);
            }
    	    retVal = db2TableOpen (&gDbTableDesc[descInx], 
	    resourceLoc, dataPath, Db2User, Db2Passwd);
    	    if (retVal != 0) { 	/* open failed */
                elog(NOTICE,"cannot open DB2 TABLE. resLoc=%s, dataPath=%s, status=%d", 
	        resourceLoc, dataPath, retVal);
                return retVal;
	    }
	    break;
#endif

#ifdef DB_Informix
          case IFMX_TABLE:
    	    descInx = newDbTableDesc (hostTabPtr, LOCAL_HOST, dbSystemType, 0);
	    if (descInx < 0) return descInx;
            if (InformixUser == NULL) {  /*  read from config file */
                InformixUser = readTableConfig (dbInstance, INFORMIX_USER_KEYWD);
                InformixPasswd = readTableConfig (dbInstance, INFORMIX_PASSWD_KEYWD);
		InformixHome = readTableConfig (dbInstance, INFORMIX_HOME_KEYWD);
            }
    	    retVal = ifmxTableOpen (&gDbTableDesc[descInx], 
	    resourceLoc, dataPath, InformixUser, InformixPasswd);
    	    if (retVal != 0) { 	/* open failed */
                elog(NOTICE,"cannot open Informix TABLE. resLoc=%s, dataPath=%s, status=%d", 
	        resourceLoc, dataPath, retVal);
                return retVal;
	    }
	    break;
#endif	    

#ifdef DB_Sybase
          case SYB_TABLE:
    	    descInx = newDbTableDesc (hostTabPtr, LOCAL_HOST, dbSystemType, 0);
	    if (descInx < 0) return descInx;
            if (SybaseUser == NULL) {  /*  read from config file */
                SybaseUser = readTableConfig (dbInstance, SYBASE_USER_KEYWD);
                SybasePasswd = readTableConfig (dbInstance, SYBASE_PASSWD_KEYWD);
		SybaseHome = readTableConfig (dbInstance, SYBASE_HOME_KEYWD);
            }
    	    retVal = sybTableOpen (&gDbTableDesc[descInx], 
	    resourceLoc, dataPath, SybaseUser, SybasePasswd);
    	    if (retVal != 0) { 	/* open failed */
                elog(NOTICE,"cannot open Sybase TABLE. resLoc=%s, dataPath=%s, status=%d", 
	        resourceLoc, dataPath, retVal);
                return retVal;
	    }
	    break;
#endif	    

#ifdef DB_Postgres
          case PSG_TABLE:
    	    descInx = newDbTableDesc (hostTabPtr, LOCAL_HOST, dbSystemType, 0);
	    if (descInx < 0) return descInx;
            if (PostgresUser == NULL) {  /*  read from config file */
                PostgresUser = readTableConfig (dbInstance, POSTGRES_USER_KEYWD);
                PostgresPasswd = readTableConfig (dbInstance, POSTGRES_PASSWD_KEYWD);
		PostgresHome = readTableConfig (dbInstance, POSTGRES_HOME_KEYWD);
            }
    	    retVal = psgTableOpen (&gDbTableDesc[descInx], 
	    resourceLoc, dataPath, PostgresUser, PostgresPasswd);
    	    if (retVal != 0) { 	/* open failed */
                elog(NOTICE,"cannot open Postgres TABLE. resLoc=%s, dataPath=%s, status=%d", 
	        resourceLoc, dataPath, retVal);
                return retVal;
	    }
	    break;
#endif	    

#ifdef DB_Mysql
          case MYS_TABLE:
    	    descInx = newDbTableDesc (hostTabPtr, LOCAL_HOST, dbSystemType, 0);
	    if (descInx < 0) return descInx;
            if (MysqlUser == NULL) {  /*  read from config file */
                MysqlUser = readTableConfig (dbInstance, MYSQL_USER_KEYWD);
                MysqlPasswd = readTableConfig (dbInstance, MYSQL_PASSWD_KEYWD);
		MysqlHome = readTableConfig (dbInstance, MYSQL_HOME_KEYWD);
            }
    	    retVal = mysTableOpen (&gDbTableDesc[descInx], 
	    resourceLoc, dataPath, MysqlUser, MysqlPasswd);
    	    if (retVal != 0) { 	/* open failed */
                elog(NOTICE,"cannot open Mysql TABLE. resLoc=%s, dataPath=%s, status=%d", 
	        resourceLoc, dataPath, retVal);
                return retVal;
	    }
	    break;
#endif	    

#ifdef DB_Oracle
          case ORA_TABLE:
            descInx = newDbTableDesc (hostTabPtr, LOCAL_HOST, dbSystemType, 0);
            if (descInx < 0) return descInx;
	    if (OracleUser == NULL) {	/* read from config file */
		OracleUser = readTableConfig (dbInstance, ORACLE_USER_KEYWD);
		OraclePasswd = readTableConfig (dbInstance, ORACLE_PASSWD_KEYWD);
		OracleHome = readTableConfig (dbInstance, ORACLE_HOME_KEYWD);
	    }
            retVal = oraTableOpen (&gDbTableDesc[descInx],
            resourceLoc, dataPath, OracleUser, OraclePasswd);
            if (retVal != 0) {  /* open failed */
                elog(NOTICE,
		  "cannot open ORA TABLE. resLoc=%s, dataPath=%s, status=%d",
                    resourceLoc, dataPath, retVal);
                return retVal;
            }
            break;
#endif

#ifdef DB_Illustra
          case ILLUSTRA_TABLE:
            descInx = newDbTableDesc (hostTabPtr, LOCAL_HOST, dbSystemType, 0);
	    if (descInx < 0) return descInx;
            if (IllusUser == NULL) {   /* read from config file */            
                IllusUser = readTableConfig (dbInstance, ILLUS_USER_KEYWD);
                IllusPasswd = readTableConfig (dbInstance, ILLUS_PASSWD_KEYWD);
		IllusHome = readTableConfig (dbInstance, ILLUS_HOME_KEYWD);
            }
	    retVal = illusTableOpen (&gDbTableDesc[descInx], 
   	             resourceLoc, dataPath, IllusUser, IllusPasswd);
    	    if (retVal != 0) { 	/* open failed */
                elog(NOTICE,
		"cannot open Illustra TABLE resLoc=%s,dataPath=%s", 
	        resourceLoc, dataPath);
                return retVal;
	    }
	    break;
#endif
          default:                /* shouldn't get here */
            elog(NOTICE, "dbTableOpen: error in dbSystemType mapping, %d",
	    dbSystemType);
	    return INP_ERR_DB_TYPE;
	}
    }
    return descInx;
}

/* dbTableCreate () - Create a DB TABLE.
 *
 * Input : int dbSystemType - The type of DataBase system
 *	   char *resourceLoc - The resource location. Noramlly, the
 *	   host address where the dataBase is located.
 *	   char *dataPath - The Path to the Table.
 *	   int mode - The mode (Same definition as in Unix)
 * 	   (For DB2/Illustra type TABLE, mode is not used.) 
 *
 * Output : The Index of DbTableDesc for the createed Table.
 */

int
dbTableCreate(int dbSystemType, struct varlena *resourceLoc, 
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

    dataPathBuf = dbTablePath2FullPath (dataPathBuf);

    descInx = _dbTableCreate(dbSystemType, resourceLocBuf, 
        dataPathBuf, mode, SRB_SERVER_USER);
 
    free (resourceLocBuf);
    free (dataPathBuf);

    return (descInx);
}

int
_dbTableCreate(int dbSystemType, char *resourceLoc, char *dataPath, 
int mode, int dbUserFlag)
{
    int descInx;
    int  retVal;
    int remoteFlag;
    char hostAddr[STRING_LEN1], dbInstance[STRING_LEN1];
    struct hostElement *hostTabPtr;
   
    if (mySplitStr (resourceLoc, hostAddr, dbInstance, ':') == NULL) {
	elog (NOTICE, "dbTableCreate: Illegal resourceLocation format: %s",
        resourceLoc);
        return INP_ERR_RES_FORMAT;
    }
  
    /* Check whether the host is remote or local. Note: hostAddr now contains
     * only the host name */
 
#ifdef FED_MCAT
    remoteFlag = chkHostAndZone (hostAddr, &hostTabPtr);
    mySplitStr (resourceLoc,resourceLoc,resourceLoc, '&');
#else
    remoteFlag = chkHost (hostAddr, NULL, &hostTabPtr);
#endif

     
    if (remoteFlag < 0) {
        elog (NOTICE, "Invalid host addr %s", hostAddr);
        return INP_ERR_HOST_ADDR;              /* failure */
    } else if (remoteFlag == REMOTE_HOST) {
        retVal = remoteDbTableCreate (dbSystemType, hostTabPtr, resourceLoc, 
	  dataPath, mode);
	if (retVal < 0)
	    return retVal;
	descInx = newDbTableDesc (hostTabPtr, REMOTE_HOST, dbSystemType, retVal);
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
          case DB2_TABLE:
    	    descInx = newDbTableDesc (hostTabPtr, LOCAL_HOST, dbSystemType, 0);
	    if (descInx < 0) return descInx;
            if (Db2User == NULL) {   /* read from config file */
                Db2User = readTableConfig (dbInstance, DB2_USER_KEYWD);
                Db2Passwd = readTableConfig (dbInstance, DB2_PASSWD_KEYWD);
		Db2Home = readTableConfig (dbInstance, DB2_HOME_KEYWD);
            }
    	    retVal = db2TableCreate (&gDbTableDesc[descInx], 
	    resourceLoc, dataPath, Db2User, Db2Passwd);
    	    if (retVal != 0) { 	/* create failed */
                elog(NOTICE,
		  "cannot create DB2 TABLE. resLoc=%s dataPath=%s status=%d", 
	            resourceLoc, dataPath, retVal);
                return retVal;
	    }
	    break;
#endif

#ifdef DB_Informix
          case IFMX_TABLE:
    	    descInx = newDbTableDesc (hostTabPtr, LOCAL_HOST, dbSystemType, 0);
	    if (descInx < 0) return descInx;
            if (InformixUser == NULL) {   /* read from config file */
                InformixUser = readTableConfig (dbInstance, INFORMIX_USER_KEYWD);
                InformixPasswd = readTableConfig (dbInstance, INFORMIX_PASSWD_KEYWD);
		InformixHome = readTableConfig (dbInstance, INFORMIX_HOME_KEYWD);
            }
    	    retVal = ifmxTableCreate (&gDbTableDesc[descInx], 
	    resourceLoc, dataPath, InformixUser, InformixPasswd);
    	    if (retVal != 0) { 	/* create failed */
                elog(NOTICE,
		  "cannot create Informix TABLE. resLoc=%s dataPath=%s status=%d", 
	            resourceLoc, dataPath, retVal);
                return retVal;
	    }
	    break;
#endif

#ifdef DB_Postgres
          case PSG_TABLE:
    	    descInx = newDbTableDesc (hostTabPtr, LOCAL_HOST, dbSystemType, 0);
	    if (descInx < 0) return descInx;
            if (PostgresUser == NULL) {   /* read from config file */
                PostgresUser = readTableConfig (dbInstance, POSTGRES_USER_KEYWD);
                PostgresPasswd = readTableConfig (dbInstance, POSTGRES_PASSWD_KEYWD);
		PostgresHome = readTableConfig (dbInstance, POSTGRES_HOME_KEYWD);
            }
    	    retVal = psgTableCreate (&gDbTableDesc[descInx], 
	    resourceLoc, dataPath, PostgresUser, PostgresPasswd);
    	    if (retVal != 0) { 	/* create failed */
                elog(NOTICE,
		  "cannot create Postgres TABLE. resLoc=%s dataPath=%s status=%d", 
	            resourceLoc, dataPath, retVal);
                return retVal;
	    }
	    break;
#endif

#ifdef DB_Sybase
          case SYB_TABLE:
    	    descInx = newDbTableDesc (hostTabPtr, LOCAL_HOST, dbSystemType, 0);
	    if (descInx < 0) return descInx;
            if (SybaseUser == NULL) {   /* read from config file */
                SybaseUser = readTableConfig (dbInstance, SYBASE_USER_KEYWD);
                SybasePasswd = readTableConfig (dbInstance, SYBASE_PASSWD_KEYWD);
		SybaseHome = readTableConfig (dbInstance, SYBASE_HOME_KEYWD);
            }
    	    retVal = sybTableCreate (&gDbTableDesc[descInx], 
	    resourceLoc, dataPath, SybaseUser, SybasePasswd);
    	    if (retVal != 0) { 	/* create failed */
                elog(NOTICE,
		  "cannot create Sybase TABLE. resLoc=%s dataPath=%s status=%d", 
	            resourceLoc, dataPath, retVal);
                return retVal;
	    }
	    break;
#endif

#ifdef DB_Mysql
          case MYS_TABLE:
    	    descInx = newDbTableDesc (hostTabPtr, LOCAL_HOST, dbSystemType, 0);
	    if (descInx < 0) return descInx;
            if (MysqlUser == NULL) {   /* read from config file */
                MysqlUser = readTableConfig (dbInstance, MYSQL_USER_KEYWD);
                MysqlPasswd = readTableConfig (dbInstance, MYSQL_PASSWD_KEYWD);
		MysqlHome = readTableConfig (dbInstance, MYSQL_HOME_KEYWD);
            }
    	    retVal = mysTableCreate (&gDbTableDesc[descInx], 
	    resourceLoc, dataPath, MysqlUser, MysqlPasswd);
    	    if (retVal != 0) { 	/* create failed */
                elog(NOTICE,
		  "cannot create Mysql TABLE. resLoc=%s dataPath=%s status=%d", 
	            resourceLoc, dataPath, retVal);
                return retVal;
	    }
	    break;
#endif

#ifdef DB_Oracle
          case ORA_TABLE:
            descInx = newDbTableDesc (hostTabPtr, LOCAL_HOST, dbSystemType, 0);
            if (descInx < 0) return descInx;
            if (OracleUser == NULL) {   /* read from config file */
                OracleUser = readTableConfig (dbInstance, ORACLE_USER_KEYWD);
                OraclePasswd = readTableConfig (dbInstance, ORACLE_PASSWD_KEYWD);
		OracleHome = readTableConfig (dbInstance, ORACLE_HOME_KEYWD);
            }
            retVal = oraTableCreate (&gDbTableDesc[descInx],
            resourceLoc, dataPath, OracleUser, OraclePasswd);
            if (retVal != 0) {  /* create failed */
                elog(NOTICE,
                  "cannot create ORA TABLE. resLoc=%s dataPath=%s status=%d",
                    resourceLoc, dataPath, retVal);
                return retVal;
            }
            break;
#endif

#ifdef DB_Illustra
          case ILLUSTRA_TABLE:
            descInx = newDbTableDesc (hostTabPtr, LOCAL_HOST, dbSystemType, 0);
	    if (descInx < 0) return descInx;
            if (IllusUser == NULL) {   /* read from config file */
                IllusUser = readTableConfig (dbInstance, ILLUS_USER_KEYWD);
                IllusPasswd = readTableConfig (dbInstance, ILLUS_PASSWD_KEYWD);
		IllusHome = readTableConfig (dbInstance, ILLUS_HOME_KEYWD);
            }
	    retVal = illusTableCreate (&gDbTableDesc[descInx], 
   	             resourceLoc, dataPath, IllusUser, IllusPasswd);
    	    if (retVal != 0) { 	/* create failed */
                elog(NOTICE,"cannot create Illustra TABLE resLoc=%s,dataPath=%s",
	        resourceLoc, dataPath);
                return retVal;
	    }
	    break;
#endif
          default:                /* shouldn't get here */
            elog(NOTICE, "dbTableCreate: error in dbSystemType mapping, %d",
	    dbSystemType);
	    return INP_ERR_DB_TYPE;
	}
    }
    return descInx;
}

/* dbTableClose () - Close a DB TABLE.
 *
 * Input : int descInx - The Index of DbTableDesc to close
 *
 * Output : Returns 0 - success
 *	           < 0 - failure
 */

int
dbTableClose (int descInx)
{
    srbConn* conn;
    int retval;

    if (descInx < 0 || descInx >= MAX_DB_TABLE_DESC ||
    gDbTableDesc[descInx].fd < 0) {
        elog(NOTICE,"dbTableClose: fd (%d) out of range", descInx);
        return INP_ERR_DESC;
    }

    if (gDbTableDesc[descInx].remoteFlag == REMOTE_HOST) {
        conn = getRemoteConn (gDbTableDesc[descInx].hostTabPtr);
        if (conn == NULL)
            return SYS_ERR_REMOTE_CONN;
        retval = srbDbTableClose (conn, gDbTableDesc[descInx].fd);
    } else {
    	switch (gDbTableDesc[descInx].dbSystemType) {
#ifdef DB_DB2
          case DB2_TABLE:
            retval = db2TableClose (&gDbTableDesc[descInx]);
            break;
#endif

#ifdef DB_Informix
          case IFMX_TABLE:
            retval = ifmxTableClose (&gDbTableDesc[descInx]);
            break;
#endif

#ifdef DB_Sybase
          case SYB_TABLE:
            retval = sybTableClose (&gDbTableDesc[descInx]);
            break;
#endif

#ifdef DB_Postgres
          case PSG_TABLE:
            retval = psgTableClose (&gDbTableDesc[descInx]);
            break;
#endif

#ifdef DB_Mysql
          case MYS_TABLE:
            retval = mysTableClose (&gDbTableDesc[descInx]);
            break;
#endif

#ifdef DB_Oracle
          case ORA_TABLE:
            retval = oraTableClose (&gDbTableDesc[descInx]);
            break;
#endif

#ifdef DB_Illustra
          case ILLUSTRA_TABLE:
            retval = illusTableClose (&gDbTableDesc[descInx]);
            break;
#endif
          default:                /* shouldn't get here */
            elog(NOTICE, "dbTableClose: error in dbSystemType mapping, %d",
	    gDbTableDesc[descInx].dbSystemType);
	    retval = INP_ERR_DB_TYPE;
            break;
	}
    }
    delDbTableDesc (descInx);
    return retval;
}

/*
 * dbTableRead - Read len bytes of the dbTable into buf.
 *
 * Input - int fd_inx - The dbTable descriptor to read (from dbTableOpen or
 *         dbTableCreate).
 *         char *buf - The input buffer.
 *         int len - The length to read.
 *
 * Output - Returns the number of bytes read..
 *          Returns negative upon failure.
 */
 
struct varlena *
dbTableRead(int fd, int len)
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
    totalread = _dbTableRead(fd, VARDATA(retval) + SIZEOF32, len);

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

/* _dbTableRead () - Perform a read operation on a DB Table.
 *
 * Input : int descInx - The Index of DbTableDesc to read
 *	   char *buf - The input buffer
 *	   int len - The length to read.
 *
 * Output : Returns the number of bytes read.
 */

int
_dbTableRead (int descInx, char *buf, int len)
{
    srbConn* conn;

    if (descInx < 0 || descInx >= MAX_DB_TABLE_DESC ||
    gDbTableDesc[descInx].fd < 0) {
        elog(NOTICE,"dbTableRead: desc (%d) out of range", descInx);
        return INP_ERR_DESC;
    }

    if (gDbTableDesc[descInx].remoteFlag == REMOTE_HOST) {
#ifdef DEMO
        elog (DEMOM, "_dbTableRead: DB on remote host. Will do a remote read");
#endif
        conn = getRemoteConn (gDbTableDesc[descInx].hostTabPtr);
        if (conn == NULL)
            return SYS_ERR_REMOTE_CONN;
        return srbDbTableRead (conn, gDbTableDesc[descInx].fd, buf, len);
    } else {

#ifdef DEMO
        elog (DEMOM, "_dbTableRead: DB on local host. Will do a local read");
#endif
    	switch (gDbTableDesc[descInx].dbSystemType) {
#ifdef DB_DB2
          case DB2_TABLE:
            return (db2TableRead (&gDbTableDesc[descInx], buf, len));
#endif

#ifdef DB_Informix
          case IFMX_TABLE:
            return (ifmxTableRead (&gDbTableDesc[descInx], buf, len));
#endif

#ifdef DB_Postgres
          case PSG_TABLE:
            return (psgTableRead (&gDbTableDesc[descInx], buf, len));
#endif

#ifdef DB_Mysql
          case MYS_TABLE:
            return (mysTableRead (&gDbTableDesc[descInx], buf, len));
#endif

#ifdef DB_Sybase
          case SYB_TABLE:
            return (sybTableRead (&gDbTableDesc[descInx], buf, len));
#endif

#ifdef DB_Oracle
          case ORA_TABLE:
            return (oraTableRead (&gDbTableDesc[descInx], buf, len));
#endif

#ifdef DB_Illustra
          case ILLUSTRA_TABLE:
            return (illusTableRead (&gDbTableDesc[descInx], buf, len));
#endif
          default:                /* shouldn't get here */
            elog(NOTICE, "dbTableRead: error in dbSystemType mapping, %d",
            gDbTableDesc[descInx].dbSystemType);
	    return INP_ERR_DB_TYPE;
	}
    }
}
 
/* dbTableWrite () - Perform a write operation on a DB Table.
 *
 * Input : int descInx - The Index of DbTableDesc to write
 *	   struct varlena *buf - The input buffer
 *
 * Output : Returns the number of bytes write.
 */

int
dbTableWrite (int descInx, struct varlena *buf)
{
    int totalwritten;
    int bytestowrite;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    bytestowrite = VARSIZE(buf) - SIZEOF32;
    totalwritten = _dbTableWrite (descInx, VARDATA(buf), bytestowrite);
    return totalwritten;
}

int
_dbTableWrite (int descInx, char *buf, int len)
{
    srbConn* conn;
    int status;

    if (descInx < 0 || descInx >= MAX_DB_TABLE_DESC ||
    gDbTableDesc[descInx].fd < 0) {
        elog(NOTICE,"dbTableWrite: desc (%d) out of range", descInx);
        return INP_ERR_DESC;
    }

    if (gDbTableDesc[descInx].remoteFlag == REMOTE_HOST) {
#ifdef DEMO
        elog (DEMOM, 
	  "dbTableWrite: DB on remote host. Will do a remote write");
#endif
        conn = getRemoteConn (gDbTableDesc[descInx].hostTabPtr);
        if (conn == NULL)
            return SYS_ERR_REMOTE_CONN;
        return srbDbTableWrite (conn, gDbTableDesc[descInx].fd, buf, len);
    } else {
#ifdef DEMO
        elog (DEMOM, "dbTableWrite: DB on local host. Will do a local write");
#endif
    	switch (gDbTableDesc[descInx].dbSystemType) {
#ifdef DB_DB2
          case DB2_TABLE:
            status = db2TableWrite (&gDbTableDesc[descInx], buf, len);
	    if (status < 0)
	        elog (NOTICE, "dbTableWrite: DB2 dbTableWrite ERROR. status = %d",
	          status);
            break;
#endif

#ifdef DB_Informix
          case IFMX_TABLE:
            status = ifmxTableWrite (&gDbTableDesc[descInx], buf, len);
	    if (status < 0)
	        elog (NOTICE, "dbTableWrite: Informix dbTableWrite ERROR. status = %d",
	          status);
            break;
#endif
	    
#ifdef DB_Postgres
          case PSG_TABLE:
            status = psgTableWrite (&gDbTableDesc[descInx], buf, len);
	    if (status < 0)
	        elog (NOTICE, "dbTableWrite: Postgres dbTableWrite ERROR. status = %d",
	          status);
            break;
#endif
	    
#ifdef DB_Mysql
          case MYS_TABLE:
            status = mysTableWrite (&gDbTableDesc[descInx], buf, len);
	    if (status < 0)
	        elog (NOTICE, "dbTableWrite: Mysql dbTableWrite ERROR. status = %d",
	          status);
            break;
#endif
	    
#ifdef DB_Sybase
          case SYB_TABLE:
            status = sybTableWrite (&gDbTableDesc[descInx], buf, len);
	    if (status < 0)
	        elog (NOTICE, "dbTableWrite: Sybase dbTableWrite ERROR. status = %d",
	          status);
            break;
#endif
	    
#ifdef DB_Oracle
          case ORA_TABLE:
            status = oraTableWrite (&gDbTableDesc[descInx], buf, len);
            if (status < 0)
                elog (NOTICE, "dbTableWrite: ORA dbTableWrite ERROR. status = %d",
                  status);
            break;
#endif

#ifdef DB_Illustra
          case ILLUSTRA_TABLE:
            status = illusTableWrite (&gDbTableDesc[descInx], buf, len);
	    if (status < 0)
                elog (NOTICE, "dbTableWrite: illusTableWrite ERROR. status = %d",
                  status);
	    break;
#endif
          default:                /* shouldn't get here */
            elog(NOTICE, "dbTableWrite: error in dbSystemType mapping, %d",
            gDbTableDesc[descInx].dbSystemType);
	    status = INP_ERR_DB_TYPE;
	}
	return (status);
    }
}
 
/* dbTableSeek () - Perform a seek operation on a DB Table.
 *
 * Input : int descInx - The Index of DbTableDesc to seek
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
dbTableSeek (int descInx, long offset, int whence)
{
    srbConn* conn;

    if (descInx < 0 || descInx >= MAX_DB_TABLE_DESC ||
    gDbTableDesc[descInx].fd < 0) {
        elog(NOTICE,"dbTableSeek: desc (%d) out of range", descInx);
        return INP_ERR_DESC;
    }

    if (gDbTableDesc[descInx].remoteFlag == REMOTE_HOST) {
        conn = getRemoteConn (gDbTableDesc[descInx].hostTabPtr);
        if (conn == NULL)
            return SYS_ERR_REMOTE_CONN;
        return srbDbTableSeek (conn, gDbTableDesc[descInx].fd, offset, whence);
    } else {

    	switch (gDbTableDesc[descInx].dbSystemType) {
#ifdef DB_DB2
          case DB2_TABLE:
            return (db2TableSeek (&gDbTableDesc[descInx], offset, whence));
#endif
#ifdef DB_Informix
          case IFMX_TABLE:
            return (ifmxTableSeek (&gDbTableDesc[descInx], offset, whence));
#endif
#ifdef DB_Postgres
          case PSG_TABLE:
            return (psgTableSeek (&gDbTableDesc[descInx], offset, whence));
#endif
#ifdef DB_Mysql
          case MYS_TABLE:
            return (mysTableSeek (&gDbTableDesc[descInx], offset, whence));
#endif
#ifdef DB_Sybase
          case SYB_TABLE:
            return (sybTableSeek (&gDbTableDesc[descInx], offset, whence));
#endif
#ifdef DB_Oracle
          case ORA_TABLE:
            return (oraTableSeek (&gDbTableDesc[descInx], offset, whence));
#endif
#ifdef DB_Illustra
          case ILLUSTRA_TABLE:
            return (illusTableSeek (&gDbTableDesc[descInx], offset, whence));
#endif
          default:                /* shouldn't get here */
            elog(NOTICE, "dbTableSeek: error in dbSystemType mapping, %d",
            gDbTableDesc[descInx].dbSystemType);
	    return INP_ERR_DB_TYPE;
	}
    }
}

/*
 * There is a  delete for Illustra.
 */

/* dbTableUnlink () - Unlink a DB TABLE.
 *
 * Input : int dbSystemType - The type of DataBase system
 *         char *resourceLoc - The resource location. Noramlly, the
 *         host address where the dataBase is located.
 *         char *dataPath - The Path to the Table.
 *
 * Output : The Index of DbTableDesc for the opened Table.
 */

int
dbTableUnlink(int dbSystemType, struct varlena  *resourceLoc,
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

    dataPathBuf = dbTablePath2FullPath (dataPathBuf);

    /* Use SRB_SERVER_USER for now. */

    status = _dbTableUnlink(dbSystemType, resourceLocBuf,
        dataPathBuf, SRB_SERVER_USER);

    free (resourceLocBuf);
    free (dataPathBuf);

    return (status);
}

int
_dbTableUnlink(int dbSystemType, char *resourceLoc, char *dataPath,
int dbUserFlag)
{
    int  retVal;
    int remoteFlag;
    char hostAddr[STRING_LEN1], dbInstance[STRING_LEN1];
    struct hostElement *hostTabPtr;

    if (mySplitStr (resourceLoc, hostAddr, dbInstance, ':') == NULL) {
        elog (NOTICE, "dbTableUnlink: Illegal resourceLocation format: %s",
        resourceLoc);
        return INP_ERR_RES_FORMAT;
    }

    /* Check whether the host is remote or local. Note: hostAddr now contains
     * only the host name */

#ifdef FED_MCAT
    remoteFlag = chkHostAndZone (hostAddr, &hostTabPtr);
    mySplitStr (resourceLoc,resourceLoc,resourceLoc, '&');
#else
    remoteFlag = chkHost (hostAddr, NULL, &hostTabPtr);
#endif

    if (remoteFlag < 0) {
        elog (NOTICE, "Invalid host addr %s", hostAddr);
        return INP_ERR_HOST_ADDR;              /* failure */
    } else if (remoteFlag == REMOTE_HOST) {
#ifdef DEMO
        elog (DEMOM,
          "_dbTableUnlink: %s is a remote host. Will do a remoteDbTableUnlink",
            hostAddr);
#endif
        retVal = remoteDbTableUnlink (dbSystemType, hostTabPtr, resourceLoc,
          dataPath);

    } else if (remoteFlag == LOCAL_HOST) {

#ifdef DEMO
        elog (DEMOM, "_dbTableUnlink: %s is a local host. Will Unlink Table here.",
            hostAddr);
        elog (DEMOM, "_dbTableUnlink: DB type= %d, Path Name = %s",
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
          case DB2_TABLE:
            if (Db2User == NULL) {   /* read from config file */
                Db2User = readTableConfig (dbInstance, DB2_USER_KEYWD);
                Db2Passwd = readTableConfig (dbInstance, DB2_PASSWD_KEYWD);
		Db2Home = readTableConfig (dbInstance, DB2_HOME_KEYWD);
            }
            retVal = db2TableUnlink ( resourceLoc, dataPath, 
	      Db2User, Db2Passwd);
            if (retVal != 0) { 
                elog(NOTICE,
		"cannot unlink DB2 TABLE. resLoc=%s, dataPath=%s, status=%d",
                resourceLoc, dataPath, retVal);
                return retVal;
            }
            break;
#endif

#ifdef DB_Informix
          case IFMX_TABLE:
            if (InformixUser == NULL) {   /* read from config file */
                InformixUser = readTableConfig (dbInstance, INFORMIX_USER_KEYWD);
                InformixPasswd = readTableConfig (dbInstance, INFORMIX_PASSWD_KEYWD);
		InformixHome = readTableConfig (dbInstance, INFORMIX_HOME_KEYWD);
            }
            retVal = ifmxTableUnlink ( resourceLoc, dataPath, 
	      InformixUser, InformixPasswd);
            if (retVal != 0) { 
                elog(NOTICE,
		"cannot unlink Informix TABLE. resLoc=%s, dataPath=%s, status=%d",
                resourceLoc, dataPath, retVal);
                return retVal;
            }
            break;
#endif

#ifdef DB_Postgres
          case PSG_TABLE:
            if (PostgresUser == NULL) {   /* read from config file */
                PostgresUser = readTableConfig (dbInstance, POSTGRES_USER_KEYWD);
                PostgresPasswd = readTableConfig (dbInstance, POSTGRES_PASSWD_KEYWD);
		PostgresHome = readTableConfig (dbInstance, POSTGRES_HOME_KEYWD);
            }
            retVal = psgTableUnlink ( resourceLoc, dataPath, 
	      PostgresUser, PostgresPasswd);
            if (retVal != 0) { 
                elog(NOTICE,
		"cannot unlink Postgres TABLE. resLoc=%s, dataPath=%s, status=%d",
                resourceLoc, dataPath, retVal);
                return retVal;
            }
            break;
#endif

#ifdef DB_Mysql
          case MYS_TABLE:
            if (MysqlUser == NULL) {   /* read from config file */
                MysqlUser = readTableConfig (dbInstance, MYSQL_USER_KEYWD);
                MysqlPasswd = readTableConfig (dbInstance, MYSQL_PASSWD_KEYWD);
		MysqlHome = readTableConfig (dbInstance, MYSQL_HOME_KEYWD);
            }
            retVal = mysTableUnlink ( resourceLoc, dataPath, 
	      MysqlUser, MysqlPasswd);
            if (retVal != 0) { 
                elog(NOTICE,
		"cannot unlink Mysql TABLE. resLoc=%s, dataPath=%s, status=%d",
                resourceLoc, dataPath, retVal);
                return retVal;
            }
            break;
#endif

#ifdef DB_Sybase
          case SYB_TABLE:
            if (SybaseUser == NULL) {   /* read from config file */
                SybaseUser = readTableConfig (dbInstance, SYBASE_USER_KEYWD);
                SybasePasswd = readTableConfig (dbInstance, SYBASE_PASSWD_KEYWD);
		SybaseHome = readTableConfig (dbInstance, SYBASE_HOME_KEYWD);
            }
            retVal = sybTableUnlink ( resourceLoc, dataPath, 
	      SybaseUser, SybasePasswd);
            if (retVal != 0) { 
                elog(NOTICE,
		"cannot unlink Sybase TABLE. resLoc=%s, dataPath=%s, status=%d",
                resourceLoc, dataPath, retVal);
                return retVal;
            }
            break;
#endif

#ifdef DB_Oracle
          case ORA_TABLE:
            if (OracleUser == NULL) {   /* read from config file */
                OracleUser = readTableConfig (dbInstance, ORACLE_USER_KEYWD);
                OraclePasswd = readTableConfig (dbInstance, ORACLE_PASSWD_KEYWD);
		OracleHome = readTableConfig (dbInstance, ORACLE_HOME_KEYWD);
            }
            retVal = oraTableUnlink ( resourceLoc, dataPath, 
	      OracleUser, OraclePasswd);
            if (retVal != 0) {
                elog(NOTICE,
                "cannot unlink ORA TABLE. resLoc=%s, dataPath=%s, status=%d",
                resourceLoc, dataPath, retVal);
                return retVal;
            }
            break;
#endif

#ifdef DB_Illustra
          case ILLUSTRA_TABLE:
            if (IllusUser == NULL) {   /* read from config file */
                IllusUser = readTableConfig (dbInstance, ILLUS_USER_KEYWD);
                IllusPasswd = readTableConfig (dbInstance, ILLUS_PASSWD_KEYWD);
		IllusHome = readTableConfig (dbInstance, ILLUS_HOME_KEYWD);
            }
            retVal = illusTableUnlink (resourceLoc, dataPath, IllusUser, 
	      IllusPasswd);
            if (retVal != 0) {  /* open failed */
                elog(NOTICE,
                "cannot delete Illustra TABLE resLoc=%s,dataPath=%s, userName=%s",
                resourceLoc, dataPath, IllusUser);
                return retVal;
            }
            break;
#endif
          default:                /* shouldn't get here */
            elog(NOTICE, "_dbTableUnlink: error in dbSystemType mapping, %d",
            dbSystemType);
            return INP_ERR_DB_TYPE;
        }
    }
    return retVal;
}

/* dbTablePath2FullPath () - Convert relative TABLE path to the default full path.
 * i.e., see if the input dataPath is in the tableName/outColName/inCondition
 * format. If not, use the following dataPath:
 * DEF_DBTABLE_TABLENAME/DEF_TABLE_COL_NAME/DEF_TABLE_ID_COL_NAME=objectName
 */


char *
dbTablePath2FullPath (char *dataPath)
{
    /* see if the dataPath is in the for tableName/objectName.
     * If not, use the DEF_DBTABLE_TABLENAME/objectName
     */
    /****
        if (checkString (dataPath, '/') != 0) {
            tempStr = (char *) malloc (strlen (dataPath) +
              strlen (DEF_TABLE_COL_NAME) +
              strlen (DEF_TABLE_ID_COL_NAME) +
              strlen (DEF_DBTABLE_TABLENAME) + 6);
            strcpy (tempStr, DEF_DBTABLE_TABLENAME);
            strcat (tempStr, "/");
            strcat (tempStr, DEF_TABLE_COL_NAME);
            strcat (tempStr, "/");
            strcat (tempStr, DEF_TABLE_ID_COL_NAME);
            strcat (tempStr, "='");
            strcat (tempStr, dataPath);
            strcat (tempStr, "'");
            free (dataPath);
            return (tempStr);
        } else {
            return (dataPath);
        }
    ***/
    return (dataPath);
}

char *
readTableConfig ( char *dbInstance, char *keyWd)
{
    FILE *file;
    char buf[MAXEXFPATH];
    char buf1[MAXEXFPATH];
    char buf2[MAXEXFPATH];
    char *retVal;

    strcpy(buf, getSrbDataDir());
    strcat(buf, "/");
    strcat(buf, TABLE_CONFIG);

    file = fopen(buf, "r");

    buf[0] = '\0';
    buf1[0] = '\0';
    buf2[0] = '\0';

    if (file) {

        /* get the string */

        while (fscanf(file, "%s  '%[^']' '%[^']'", buf, buf2, buf1) != EOF) {

            if (!strcmp(buf, keyWd) && !strcmp(buf2, dbInstance)) {
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
