/**************************************************************************
Copyright ^Å” General Atomics, 1995-1999
All Rights Reserved
 
This file contains Original Code and/or Modifications of Original Code as
defined in and that are subject to the General Atomics Storage Resource
Broker Academic and Research Use  License (the 'License'). This license is
for academic and research use only. You may not use, copy, modify or
distribute SRB for commercial purposes, nor incorporate the SRB into
commercial products without the express written permission of General
Atomics nor may you distribute SRB to any third party.
 
You may not use this software except in compliance with the License. Please
obtain a copy of the License at http://www.npaci.edu/DICE/SRB/License and
read it before using this file.
 
The Original Code and all software distributed under the License are
distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
EXPRESS OR IMPLIED, AND GENERAL ATOMICS AND THE SAN DIEGO SUPERCOMPUTER
CENTER HEREBY DISCLAIM ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION,
ANY WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
NON-INFRINGEMENT OF ANY COPYRIGHT, PATENT , TRADEMARK OR OTHER RIGHTS.
GENERAL ATOMICS AND THE SAN DIEGO SUPERCOMPUTER CENTER FURTHER  HAVE NO
OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCMENTS OR
MODIFICATIONS.. Please see the License for the specific language governing
rights and limitations under the License."
 
Please read the terms of this license carefully. By using the SRB software,
you are accepting and agreeing to the terms of this license with General
Atomics. If you are agreeing to this license on behalf of a company, or
other institution, you represent that you are authorized to bind the
company/institution to such a license. If you do not meet this criterion or
you do not agree to any of the terms of this license, you are not
authorized to use the software.
**************************************************************************/

/* MDriver.c - Routines to handle Miscellaneous Driver requests */

#include "MDriver.h"

/* dummy_rt - A dummy routine to fill in the exfsw table.
 *
 * Input - None
 *
 * Output - None
 */
static int dummy_rt ()
{
    elog (NOTICE, "This extended file storage system is not supported");
    return INP_ERR_NO_SUPPORT;
}

/* void_dummy_rt - A dummy routine to fill in the exfsw table.
 *
 * Input - None
 *
 * Output - None
 */

static void void_dummy_rt ()
{
    elog (NOTICE, "This extended file storage system is not supported");
}

/* long_dummy_rt - A dummy routine to fill in the exfsw table.
 *
 * Input - None
 *
 * Output - None
 */

static srb_long_t long_dummy_rt ()
{
    elog (NOTICE, "This extended file storage system is not supported");
    return INP_ERR_NO_SUPPORT;
}

/* null_rt - A null routine for NULL_FT.
 *
 * Input - None
 *
 * Output - None
 */
static int null_rt ()
{
    return 0;
}

/* void_null_rt - A null routine for NULL_FT.
 *
 * Input - None
 *
 * Output - None
 */

static void void_null_rt ()
{
    return;
}

/* long_null_rt - A null routine for NULL_FT.
 *
 * Input - None
 *
 * Output - None
 */

static srb_long_t long_null_rt ()
{
    return 0;
}

/* null_read - A read routine for NULL_FT.
 *
 * Input - None
 *
 * Output - None
 */

static int null_read (int file, char *buffer, int amount)
{
    return EOF;
}

/* null_write - A write routine for NULL_FT.
 *
 * Input - None
 *
 * Output - None
 */

static int null_write (int file, char *buffer, int amount)
{
    return amount;
}
/* initMDriverDesc - Initialize the gMDriverDesc 
 *   called in srbServerMisc.c
 * Input - None
 *
 * Output - None
 */

void initMDriverDesc()
{
    int i;
 
    for (i = 0; i < MAX_MDRIVER_DESC; i++) {
        gMDriverDesc[i].fd = -1;
        gMDriverDesc[i].remoteFlag = 0;
        gMDriverDesc[i].hostTabPtr = NULL;
        gMDriverDesc[i].driverSpecificInfo = NULL;
    }
}

/* newMDriverDesc - Get a free gMDriverDesc.
 *
 * Input - None
 *
 * Output - Returns the index of a free gMDriverDesc.
 *	    Returns negative - error
 */

int
newMDriverDesc (struct hostElement *hostTabPtr, int remoteFlag, 
		int MDriverType, char * driverSpecificInfo, int fd)
{
    int i;
 
    for (i = 0; i < MAX_MDRIVER_DESC; i++) {
        if (gMDriverDesc[i].fd == -1) {
	    gMDriverDesc[i].fd = fd;
	    gMDriverDesc[i].hostTabPtr = hostTabPtr;
	    gMDriverDesc[i].remoteFlag = remoteFlag;
	    gMDriverDesc[i].MDriverType = MDriverType;
	    gMDriverDesc[i].driverSpecificInfo = driverSpecificInfo;
            return (i);
        }
    }
    elog(NOTICE,"newMDriverDesc: Unable to allocate gMDriverDesc descriptor");
    return SYS_ERR_DESC_OVERFLOW;
}
 
/* delMDriverDesc - Put a gMDriverDesc back to the free pool.
 *
 * Input : int i - The index of the gMDriverDesc to be freed.
 *
 * Output - None.
 */

void
delMDriverDesc (int i) {

    gMDriverDesc[i].fd = -1;
    gMDriverDesc[i].remoteFlag = 0;
    gMDriverDesc[i].driverSpecificInfo = NULL;
    gMDriverDesc[i].hostTabPtr = NULL;
}

/* MDriverOpen () - Open a Misc Driver.
 *
 * Input : int MDriverType - The type of Misc Driver system
 *	   mdasGetInfoOut *dataInfo - Data Info including ressource location. 
 *         Normally, the
 *	   host address where the misc driver is located.
 *	   char *dataPath - The Path to the Driver Object.
 *	   int openFlag - The open flag (Same definition as in Unix)
 *	   int mode - The mode (Same definition as in Unix)
 *	   char *userName - The user name (not used for now).
 *
 * Output : The Index of MDriverDesc for the opened Driver Object.
 */
/*** 
int
MDriverOpen(int MDriverType, struct mdasGetInfoOut *dataInfo, 
struct varlena *dataPath, int openFlag, int mode)
***/
int
MDriverOpen(int MDriverType, struct varlena *rsrcLoc,
struct varlena *dataPath, int openFlag, int mode)
{
    int descInx;
    char  *dataPathBuf, *rsrcLocBuf;


    if (TicketUserFlag && ProxyUser->privUserFlag == 0) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return ILLEGAL_OPR_TICKET_USER;
    }


    dataPathBuf = varToStr (dataPath);
    rsrcLocBuf = varToStr (rsrcLoc);
    
    /*
    descInx = _MDriverOpen(MDriverType, dataInfo, 
	dataPathBuf, openFlag, mode, SRB_SERVER_USER);
    */
    
    descInx = _MDriverOpen(MDriverType, rsrcLocBuf, 
	dataPathBuf, openFlag, mode, SRB_SERVER_USER);
    free (dataPathBuf);
    free (rsrcLocBuf);
    return (descInx);
}
/*
int
_MDriverOpen(int MDriverType, struct mdasGetInfoOut *dataInfo, char *dataPath, int openFlag, int mode, int driverUserFlag)
*/
int
_MDriverOpen(int MDriverType, char *rsrcLoc, char *dataPath, int openFlag, int mode, int driverUserFlag)
{
    int descInx;
    int  retVal;
    int remoteFlag;
    char hostAddr[MAX_TOKEN], dbInstance[MAX_TOKEN];
    struct hostElement *hostTabPtr;
    char *driverUserName;
    char tmpStr[MAX_TOKEN];

    if (mySplitStr (rsrcLoc, hostAddr, dbInstance, ':') == NULL) {
	elog (NOTICE, "MDriverOpen: Illegal resourceLocation format: %s",
        rsrcLoc);
        return INP_ERR_RES_FORMAT;
    }


    /* Check whether the host is remote or local. Note: hostAddr now contains
     * only the host name */
 
#ifdef FED_MCAT
    remoteFlag = chkHostAndZone (hostAddr, &hostTabPtr);
    mySplitStr (rsrcLoc,tmpStr,rsrcLoc, '&');
#else
    remoteFlag = chkHost (hostAddr, NULL, &hostTabPtr);
#endif
     
    if (remoteFlag < 0) {
        elog (NOTICE, "MDriverOpen: Invalid host addr %s", hostAddr);
        return INP_ERR_HOST_ADDR;              /* failure */
    } 
    else if (remoteFlag == REMOTE_HOST) {
      if ((retVal = remoteMDriverOpen (MDriverType, hostTabPtr, rsrcLoc, 
				  dataPath, openFlag, mode)) < 0)
	return retVal;
      descInx = newMDriverDesc (hostTabPtr,REMOTE_HOST, MDriverType,
				NULL, retVal);
    } 
    else if (remoteFlag == LOCAL_HOST) {
      if (MDriverType < 0 || MDriverType > nMdSwEntries) {
	elog (NOTICE, "MDriverOpen: The storSysType %d is not supported", MDriverType);
        return INP_ERR_NO_SUPPORT;
      }
      if (driverUserFlag == SRB_SERVER_USER) 
	driverUserName = ProxyUser->userName;
      else 
	driverUserName = ClientUser->userName;
      if ((descInx = newMDriverDesc (hostTabPtr, LOCAL_HOST, MDriverType,
				     NULL, 0))< 0)
	return descInx;
      /*
      retVal =  (*(mdsw[MDriverType].md_open))(&gMDriverDesc[descInx],
					    dataInfo, dataPath, 
					    openFlag, mode,driverUserName);
      */
      retVal =  (*(mdsw[MDriverType].md_open))(&gMDriverDesc[descInx],
					    rsrcLoc, dataPath, 
					    openFlag, mode,driverUserName);
      
      if (retVal != 0) {
	delMDriverDesc (descInx);
	return retVal;

      }
    }
    else 
      return INP_ERR_RES_FORMAT;
    return descInx;
}

/* MDriverCreate () - Create a Misc Driver.
 *
 * Input : int MDriverType - The type of Misc Driver system
 *	   mdasResInfo *resourceInfo - The resource Information. Noramlly, the
 *	   host address where the misc driver is located.
 *	   char *dataPath - The Path to the Driver Object.
 *	   int mode - The mode (Same definition as in Unix)
 *
 * Output : The Index of MDriverDesc for the createed Driver Object.
 */
/*
int
MDriverCreate(int MDriverType, struct mdasResInfo *resourceInfo, 
struct varlena *dataPath, int mode)
*/
int
MDriverCreate(int MDriverType, struct varlena *rsrcLoc, 
struct varlena *dataPath, int mode)
{
    int descInx;
    char  *dataPathBuf, *rsrcLocBuf;
 
    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }


    dataPathBuf = varToStr (dataPath);
    rsrcLocBuf = varToStr (rsrcLoc);
    descInx = _MDriverCreate(MDriverType, rsrcLocBuf, 
        dataPathBuf, mode, SRB_SERVER_USER);
 

    free (dataPathBuf);

    return (descInx);
}
/*
int
_MDriverCreate(int MDriverType, struct mdasResInfo *resourceInfo, char *dataPath, int mode, int driverUserFlag)
*/
int
_MDriverCreate(int MDriverType, char *rsrcLoc, char *dataPath, int mode, int driverUserFlag)
{
    int descInx;
    int  retVal;
    int remoteFlag;
    char hostAddr[MAX_TOKEN], dbInstance[MAX_TOKEN];
    struct hostElement *hostTabPtr;
    char *driverUserName;
    char tmpStr[MAX_TOKEN];
    
    if (mySplitStr (rsrcLoc, hostAddr, dbInstance, ':') == NULL) {
	elog (NOTICE, "MDriverCreate: Illegal resourceLocation format: %s",
        rsrcLoc);
        return INP_ERR_RES_FORMAT;
    }


    /* Check whether the host is remote or local. Note: hostAddr now contains
     * only the host name */
 
#ifdef FED_MCAT
    remoteFlag = chkHostAndZone (hostAddr, &hostTabPtr);
    mySplitStr (rsrcLoc,tmpStr,rsrcLoc, '&');
#else
    remoteFlag = chkHost (hostAddr, NULL, &hostTabPtr);
#endif

     
    if (remoteFlag < 0) {
        elog (NOTICE, "Invalid host addr %s", hostAddr);
        return INP_ERR_HOST_ADDR;              /* failure */
    } else if (remoteFlag == REMOTE_HOST) {
      /*        
        if ((retVal = remoteMDriverCreate (MDriverType,hostTabPtr,resourceInfo,
					   dataPath, mode)) < 0)
      */
      if ((retVal = remoteMDriverCreate (MDriverType,hostTabPtr, rsrcLoc,
					   dataPath, mode)) < 0)
	return retVal;
	descInx = newMDriverDesc (hostTabPtr,REMOTE_HOST, MDriverType, NULL, retVal);
    } else if (remoteFlag == LOCAL_HOST) {
      if (MDriverType < 0 || MDriverType > nMdSwEntries) {
	elog (NOTICE, "The storSysType %d is not supported", MDriverType);
        return INP_ERR_NO_SUPPORT;
      }
      if (driverUserFlag == SRB_SERVER_USER)
	driverUserName = ProxyUser->userName;
      else
	driverUserName = ClientUser->userName;
      if ((descInx = newMDriverDesc (hostTabPtr, LOCAL_HOST, 
				     MDriverType, NULL, 0))< 0)
	return descInx;
      /*
      retVal =  (*(mdsw[MDriverType].md_create))(&gMDriverDesc[descInx],
					    resourceInfo, dataPath, 
					    mode,driverUserName);
      */
      retVal =  (*(mdsw[MDriverType].md_create))(&gMDriverDesc[descInx],
					    rsrcLoc, dataPath, 
					    mode,driverUserName);

      if (retVal != 0) {
	delMDriverDesc (descInx);
	return retVal;
      }
    }
    else 
      return INP_ERR_RES_FORMAT;
    return descInx;
}


/* MDriverClose () - Close a Misc Driver.
 *
 * Input : int descInx - The Index of MDriverDesc to close
 *
 * Output : Returns 0 - success
 *	           < 0 - failure
 */

int
MDriverClose (int descInx)
{
    srbConn* conn;
    int retval;

    if (descInx < 0 || descInx >= MAX_MDRIVER_DESC ||
    gMDriverDesc[descInx].fd < 0) {
        elog(NOTICE,"MDriverClose: fd (%d) out of range", descInx);
        return INP_ERR_DESC;
    }

    if (gMDriverDesc[descInx].remoteFlag == REMOTE_HOST) {
        conn = getRemoteConn (gMDriverDesc[descInx].hostTabPtr);
        if (conn == NULL)
            return SYS_ERR_REMOTE_CONN;
        retval = srbMDriverClose (conn, gMDriverDesc[descInx].fd);
    } else {
      if (gMDriverDesc[descInx].MDriverType < 0 || 
	  gMDriverDesc[descInx].MDriverType > nMdSwEntries) {
	elog (NOTICE, "The storSysType %d is not supported", 
	      gMDriverDesc[descInx].MDriverType);
        return INP_ERR_NO_SUPPORT;
      }
      retval =  (*(mdsw[gMDriverDesc[descInx].MDriverType].md_close))(&gMDriverDesc[descInx]);
    }
    delMDriverDesc (descInx);
    return retval;
}

/*
 * MDriverRead - Read len bytes of the MDriver into buf.
 *
 * Input - int fd_inx - The MDriver descriptor to read (from MDriverOpen or
 *         MDriverCreate).
 *         char *buf - The input buffer.
 *         int len - The length to read.
 *
 * Output - Returns the number of bytes read..
 *          Returns negative upon failure.
 */
 
struct varlena *MDriverRead(int fd, int len)
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
    totalread = _MDriverRead(fd, VARDATA(retval) + SIZEOF32, len);

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

/* _MDriverRead () - Perform a read operation on a Misc Driver.
 *
 * Input : int descInx - The Index of MDriverDesc to read
 *	   char *buf - The input buffer
 *	   int len - The length to read.
 *
 * Output : Returns the number of bytes read.
 */

int
_MDriverRead (int descInx, char *buf, int len)
{
    srbConn* conn;
    int retval;

    if (descInx < 0 || descInx >= MAX_MDRIVER_DESC ||
    gMDriverDesc[descInx].fd < 0) {
        elog(NOTICE,"MDriverRead: desc (%d) out of range", descInx);
        return INP_ERR_DESC;
    }
    if (gMDriverDesc[descInx].remoteFlag == REMOTE_HOST) {
        conn = getRemoteConn (gMDriverDesc[descInx].hostTabPtr);
        if (conn == NULL)
            return SYS_ERR_REMOTE_CONN;
        return srbMDriverRead (conn, gMDriverDesc[descInx].fd, buf, len);
    } else {
      if (gMDriverDesc[descInx].MDriverType < 0 || 
	  gMDriverDesc[descInx].MDriverType > nMdSwEntries) {
	elog (NOTICE, "The storSysType %d is not supported", 
	      gMDriverDesc[descInx].MDriverType);
        return INP_ERR_NO_SUPPORT;
      }
      retval =  (*(mdsw[gMDriverDesc[descInx].MDriverType].md_read))(&gMDriverDesc[descInx], 
					       buf, len);
      return retval;
    }
}
 
/* MDriverWrite () - Perform a write operation on a Misc Driver.
 *
 * Input : int descInx - The Index of MDriverDesc to write
 *	   struct varlena *buf - The input buffer
 *
 * Output : Returns the number of bytes write.
 */

int
MDriverWrite (int descInx, struct varlena *buf)
{
    int totalwritten;
    int bytestowrite;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    bytestowrite = VARSIZE(buf) - SIZEOF32;
    totalwritten = _MDriverWrite (descInx, VARDATA(buf), bytestowrite);
    return totalwritten;
}

int
_MDriverWrite (int descInx, char *buf, int len)
{
    srbConn* conn;
    int retval;
    if (descInx < 0 || descInx >= MAX_MDRIVER_DESC ||
    gMDriverDesc[descInx].fd < 0) {
        elog(NOTICE,"MDriverWrite: desc (%d) out of range", descInx);
        return INP_ERR_DESC;
    }

    if (gMDriverDesc[descInx].remoteFlag == REMOTE_HOST) {
        conn = getRemoteConn (gMDriverDesc[descInx].hostTabPtr);
        if (conn == NULL)
            return SYS_ERR_REMOTE_CONN;
        return srbMDriverWrite (conn, gMDriverDesc[descInx].fd, buf, len);
    } else {
      if (gMDriverDesc[descInx].MDriverType < 0 || 
	  gMDriverDesc[descInx].MDriverType > nMdSwEntries) {
	elog (NOTICE, "The storSysType %d is not supported", 
	      gMDriverDesc[descInx].MDriverType);
        return INP_ERR_NO_SUPPORT;
      }
      retval =  (*(mdsw[gMDriverDesc[descInx].MDriverType].md_write))(&gMDriverDesc[descInx], 
						buf, len);
      return retval;
    }
}
 
/* MDriverSeek () - Perform a seek operation on a Misc Driver.
 *
 * Input : int descInx - The Index of MDriverDesc to seek
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
MDriverSeek (int descInx, long offset, int whence)
{
    srbConn* conn;
    int retval;
    if (descInx < 0 || descInx >= MAX_MDRIVER_DESC ||
    gMDriverDesc[descInx].fd < 0) {
        elog(NOTICE,"MDriverSeek: desc (%d) out of range", descInx);
        return INP_ERR_DESC;
    }

    if (gMDriverDesc[descInx].remoteFlag == REMOTE_HOST) {
        conn = getRemoteConn (gMDriverDesc[descInx].hostTabPtr);
        if (conn == NULL)
            return SYS_ERR_REMOTE_CONN;
        return srbMDriverSeek (conn, gMDriverDesc[descInx].fd, offset, whence);
    } else {
      if (gMDriverDesc[descInx].MDriverType < 0 || 
	  gMDriverDesc[descInx].MDriverType > nMdSwEntries) {
	elog (NOTICE, "The storSysType %d is not supported", 
	      gMDriverDesc[descInx].MDriverType);
        return INP_ERR_NO_SUPPORT;
      }
      retval =  (*(mdsw[gMDriverDesc[descInx].MDriverType].md_seek))(&gMDriverDesc[descInx], 
					       offset, whence);
      return retval;
    }
}

/* MDriverUnlink () - Unlink a Misc Driver.
 *
 * Input : int MDriverType - The type of Misc Driver system
 *         char *resourceLoc - The resource location. Noramlly, the
 *         host address where the misc driver is located.
 *         char *dataPath - The Path to the Table.
 *
 * Output : The Index of MDriverDesc for the opened Driver Object.
 */

int
MDriverUnlink(int MDriverType, struct varlena  *resourceLoc,
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

    status = _MDriverUnlink(MDriverType, resourceLocBuf,
        dataPathBuf, SRB_SERVER_USER);

    free (resourceLocBuf);
    free (dataPathBuf);

    return (status);
}

int
_MDriverUnlink(int MDriverType, char *resourceLoc, char *dataPath,
int driverUserFlag)
{
    int  retVal;
    int remoteFlag;
    char hostAddr[STRING_LEN1], dbInstance[STRING_LEN1];
    struct hostElement *hostTabPtr;

    if (mySplitStr (resourceLoc, hostAddr, dbInstance, ':') == NULL) {
        elog (NOTICE, "MDriverUnlink: Illegal resourceLocation format: %s",
        resourceLoc);
        return INP_ERR_RES_FORMAT;
    }

    /* Check whether the host is remote or local. Note: hostAddr now contains
     * only the host name */

    remoteFlag = chkHost (hostAddr, NULL, &hostTabPtr);

    if (remoteFlag < 0) {
        elog (NOTICE, "Invalid host addr %s", hostAddr);
        return INP_ERR_HOST_ADDR;              /* failure */
    } else if (remoteFlag == REMOTE_HOST) {
        retVal = remoteMDriverUnlink (MDriverType, hostTabPtr, resourceLoc,
          dataPath);
    } else if (remoteFlag == LOCAL_HOST) {
      if (MDriverType < 0 || MDriverType > nMdSwEntries) {
	elog (NOTICE, "The storSysType %d is not supported", MDriverType);
        return INP_ERR_NO_SUPPORT;
      }
      retVal = (*(mdsw[MDriverType].md_unlink))(resourceLoc, dataPath);
    }
    return retVal;
}

int
MDriverSync (int descInx)
{
    srbConn* conn;
    int retval;
    if (descInx < 0 || descInx >= MAX_MDRIVER_DESC ||
    gMDriverDesc[descInx].fd < 0) {
        elog(NOTICE,"MDriverSync: desc (%d) out of range", descInx);
        return INP_ERR_DESC;
    }

    if (gMDriverDesc[descInx].remoteFlag == REMOTE_HOST) {
        conn = getRemoteConn (gMDriverDesc[descInx].hostTabPtr);
        if (conn == NULL)
            return SYS_ERR_REMOTE_CONN;
        return srbMDriverSync (conn, gMDriverDesc[descInx].fd);
    } else {
      if (gMDriverDesc[descInx].MDriverType < 0 || 
	  gMDriverDesc[descInx].MDriverType > nMdSwEntries) {
	elog (NOTICE, "The storSysType %d is not supported", 
	      gMDriverDesc[descInx].MDriverType);
        return INP_ERR_NO_SUPPORT;
      }
      retval =  (*(mdsw[gMDriverDesc[descInx].MDriverType].md_sync))(&gMDriverDesc[descInx]);
      return retval;
    }
}


extern struct varlena *
MDriverProc (int fd,int outLen,
				    struct varlena *vprocName,
                                    struct varlena *vinBuf)
{
    struct varlena *retval;
    int totalread;
    int status;
    int bytestowrite;
    char *procName;

    retval = (struct varlena *)malloc(VAROUTHDRSZ +  outLen + 1);
    bytestowrite = VARSIZE(vinBuf)  - SIZEOF32;   
    procName = varToStr (vprocName);
    totalread = _MDriverProc (fd, procName, 
                             VARDATA(vinBuf), bytestowrite,
                             VAROUTDATA(retval), outLen);
         
    if (totalread >= 0) {
        status = 0;
    } else {
        status = totalread;
        totalread = 0;
    }
    VARSIZE(retval) = totalread + VAROUTHDRSZ;
    VAROUTSTAT(retval) = htonl (status);

    return(retval);
}


int
_MDriverProc (int descInx,char *procName,
	      char *inBuf, int inLen, 
	      char *outBuf, int outLen )
{
    srbConn* conn;
    int retval;
    if (descInx < 0 || descInx >= MAX_MDRIVER_DESC ||
        gMDriverDesc[descInx].fd < 0) {
        elog(NOTICE,"MDriverWrite: desc (%d) out of range", descInx);
        return INP_ERR_DESC;
    }

    if (gMDriverDesc[descInx].remoteFlag == REMOTE_HOST) {
        conn = getRemoteConn (gMDriverDesc[descInx].hostTabPtr);
        if (conn == NULL)
            return SYS_ERR_REMOTE_CONN;
        return srbMDriverProc (conn, gMDriverDesc[descInx].fd, 
			       procName,inBuf,inLen,outBuf,  outLen);
    } else {
        if (gMDriverDesc[descInx].MDriverType < 0 ||
            gMDriverDesc[descInx].MDriverType > nMdSwEntries) {
            elog (NOTICE, "The storSysType %d is not supported",
                  gMDriverDesc[descInx].MDriverType);
            return INP_ERR_NO_SUPPORT;
        }
        retval =  (*(mdsw[gMDriverDesc[descInx].MDriverType].md_proc))
	      (&gMDriverDesc[descInx],procName,inBuf,inLen,outBuf,  outLen);
        return retval;
    }
}

