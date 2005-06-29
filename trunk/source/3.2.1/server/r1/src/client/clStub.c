/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 

/*-------------------------------------------------------------------------
 *
 * clStub.c--
 *    Client function interface
 *
 *-------------------------------------------------------------------------
 */

#include "clStub.h"

/*
 * srbFileOpen - Opens an extended file
 *
 * Input - srbConn* conn - From srbConnect ().
 *	   int storSysType - Storage system type. 
 *	   	0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP, 4 = HTTP.
 *	   char *hostAddr - The Host address of the SRB.
 *	   char *filename - The file Path name to open. Must be an
 *          absolute path.
 *	   int flags - same definition as in unix.
 *	   int mode - same definition as in unix.
 * Output - Returns the file descriptor for use in later srbFile* calls.
 * 	    Returns a negative value upon failure.
 */

int
srbFileOpen(srbConn* conn, int storSysType, char *hostAddr, char *filename, 
int flags, int mode)
{
    int status;

    status = _srbFileOpen(conn, storSysType, hostAddr, filename, flags, mode,
    DO_CHKDIR);

    return (status);
}

int
_srbFileOpen(srbConn* conn, int storSysType, char *hostAddr, 
char *filename, int flags, int mode, int chdirFlag)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;
 
    clOut.retval = (char *) &clOutVal;

    argv[0] = storSysType;
    argv[1] = addrToSRBLong (hostAddr);
    argv[2] = addrToSRBLong (filename);
    argv[3] = flags;
    argv[4] = mode;
    argv[5] = chdirFlag;
 
    retval = clCall (conn, F_E_OPEN, argv, &clOut);
    return retval;
}

/*
 * srbFileCreate Create an extended file.
 * Input - srbConn* conn - From srbConnect ().
 *         int storSysType - Storage system type. 
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP (not supported for
 *		HTTP).
 *         char *hostAddr - The Host address of the SRB.
 *         char *filename - The file Path name to create. Must be an
 *          absolute path.
 *              Storage Vault.
 *         int mode - same definition as in unix.
 *	   int size - File size. Only valid for HPSS. -1 => don't know and
 *	   The default COS will be used.
 * Output - Returns the file descriptor for use in later srbFile* calls.
 *          Returns a negative value upon failure.
 */

int
srbFileCreate(srbConn* conn, int storSysType, char *hostAddr, char *filename, 
int mode, srb_long_t size)
{
    int status;

    status = _srbFileCreate(conn, storSysType, hostAddr, filename,
      mode, DO_CHKDIR, size);

    return (status);
}

int
_srbFileCreate(srbConn* conn, int storSysType, char *hostAddr, char *filename, 
int mode, int chdirFlag, srb_long_t size)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;
 

    clOut.retval = (char *) &clOutVal;

    argv[0] = storSysType;
    argv[1] = addrToSRBLong (hostAddr);
    argv[2] = addrToSRBLong (filename);
    argv[3] = mode;
    argv[4] = chdirFlag;
    argv[5] = addrToSRBLong ((char *) &size);
 
    retval = clCall (conn, F_E_CREATE, argv, &clOut);
    return retval;
}

/*
 * srbFileUnlink - Unlink an extended file
 * Input - srbConn* conn - From srbConnect ().
 *         int storSysType - Storage system type. 
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP (not supported for
 *              HTTP).
 *         char *hostAddr - The Host address of the SRB.
 *         char *filename - The file Path name to unlink. Must be an
 *          absolute path.
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */

int
srbFileUnlink(srbConn* conn, int storSysType, char *hostAddr, char *filename) 
{
    return (_srbFileUnlink(conn, storSysType, hostAddr, filename,
      DO_CHKDIR));
}

int
_srbFileUnlink(srbConn* conn, int storSysType, char *hostAddr, 
char *filename, int chdirFlag) 
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;
 
    clOut.retval = (char *) &clOutVal;

    argv[0] = storSysType;
    argv[1] = addrToSRBLong (hostAddr);
    argv[2] = addrToSRBLong (filename);
    argv[3] = chdirFlag;
 
    retval = clCall (conn, F_E_UNLINK, argv, &clOut);
    return retval;
}

/*
 * srbFileClose - close an extended file
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int fd - The file descriptor to close (from srbFileOpen or 
 *	   srbFileCreate).
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */

int
srbFileClose(srbConn* conn, int fd) 
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;
 
    argv[0] = fd;
 
    retval = clCall (conn, F_E_CLOSE, argv, &clOut);
    return retval;
}

/*
 * srbFileRead - Read len bytes of the extended file into buf
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int fd - The file descriptor to read (from srbFileOpen or 
 *	   srbFileCreate).
 *	   char *buf - The input buffer.
 *	   int len - The length to read.
 *
 * Output - Returns the number of bytes read..
 *          Returns a negative value upon failure.
 *
 * The CALLER must have allocated enough space to hold the result returned
 */
 
int
srbFileRead(srbConn *conn, int fd, char *buf, int len)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;
 
    clOut.retval = buf;

    argv[0] = fd;
 
    argv[1] = len;
 
    retval = clCall (conn, F_E_READ, argv, &clOut);
    return retval;
}

/*
 * srbFileWrite - Write len bytes of buf into the extended file fd
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int fd - The file descriptor to write (from srbFileOpen or 
 *	   srbFileCreate).
 *         char *buf - The output buffer.
 *         int len - The length to write.
 *
 * Output - Returns the number of bytes written..
 *          Returns a negative value upon failure.
 * 
 * Note : We cannot use the generic routine clCall() because the input
 * byte stream is not a char string.
 *
 */
int
srbFileWrite(srbConn *conn, int fd, char *buf, int len)
{
    clArgBlock argv[2];
    srbResult *res;
    int resultLen;
    int retval;
 
    if (len <= 0)
        return 0;
 
    argv[0].isint = 1;
    argv[0].len = 4;
    argv[0].u.integer = fd;
 
    argv[1].isint = 0;
    argv[1].len = len;
    argv[1].u.ptr = (char*)buf;
 
    res = clFunct(conn, F_E_WRITE,(char *) &retval,&resultLen,1,argv,2);
    if (clResultStatus(res) == CLI_COMMAND_OK) {
        clClear(res);
        return retval;
    } else {
        retval = clResultStatus(res);
        clClear(res);
        return retval;
    }
}

/*
 * srbFileSeek - Change the current read or write location on an extended file
 * currently.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int fd - The file descriptor to seek (from srbFileOpen or 
 * 	   srbFileCreate).
 *	   int offset - The position of the next operation
 *	   int whence - Same definition as in Unix.
 *       	SEEK_SET - pointer is set to the value of the Offset parameter.
 *       	SEEK_CUR - pointer is set to its current location plus the 
 *			value of the Offset parameter.
 *       	SEEK_END - pointer is set to the size of the file plus the 
 *			value of the Offset parameter.
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 *
 */
 
srb_long_t
srbFileSeek(srbConn *conn, int fd, srb_long_t offset, int whence)
{
    clArgBlock argv[3];
    srbResult *res;
    int resultLen;
    srb_long_t retval;

    argv[0].isint = 1;
    argv[0].len = 4;
    argv[0].u.integer = fd;

    argv[1].isint = 1;
    argv[1].len = 2*SIZEOF32;
    argv[1].u.ptr = (char*) &offset;

    argv[2].isint = 1;
    argv[2].len = 4;
    argv[2].u.integer = whence;

    res = clFunct(conn, F_E_SEEK,(char *) &retval,&resultLen,1,argv,3);
    if (clResultStatus(res) == CLI_COMMAND_OK) {
        clClear(res);
        return retval;
    } else {
        retval = clResultStatus(res);
        clClear(res);
        return retval;
    }
}

/*
 * srbFileSync - Sync an extended file
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int fd - The file descriptor to sync (from srbFileOpen or 
 *	   srbFileCreate).
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 *
 */
int
srbFileSync(srbConn* conn, int fd)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;
 
    argv[0] = fd;
 
    retval = clCall (conn, F_E_SYNC, argv, &clOut);
    return retval;
}

/*
 * srbFileStat - Stat an extended file
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int storSysType - Storage system type. 
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP (not supported for
 *              HTTP).
 *         char *hostAddr - The Host address of the SRB.
 *         char *filename - The file Path name to stat. Must be an
 *          absolute path.
 *	   stat *statbuf - The place to put the stat result.
 *
 * Output - stat result in statbuf.
 *	    Returns 0 upon success.
 *          Returns a negative value upon failure.
 */
int
srbFileStat(srbConn* conn, int storSysType, char *hostAddr, char *filename, 
struct srbStat *statbuf) 
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) statbuf;
 
    argv[0] = storSysType;
    argv[1] = addrToSRBLong (hostAddr);
    argv[2] = addrToSRBLong (filename);

    retval = clCall (conn, F_E_STAT, argv, &clOut);

    if (retval >= 0) {
	/* For c90, pat it to 64 bit boundary */
	if (retval != retval / sizeof (int) * sizeof (int))
	    retval = retval / sizeof (int) * sizeof (int) + 
	      sizeof (int);
	if (retval < sizeof (struct srbStat))
	    return CLI_ERR_RETURN_LEN;
	else {
	    /* Convert the elements of the srbStat struct. */
	    ntohStat (statbuf);
            return 0;
        }
    } else {
        return retval;
    }
}

/*
 * srbFileFstat - Fstat an extended file
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int storSysType - Storage system type.
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP (not supported for
 *              HTTP).
 *         char *hostAddr - The Host address of the SRB.
 *         int fd_inx - The srb file descriptor fstat.
 *         stat *statbuf - The place to put the stat result.
 *
 * Output - stat result in statbuf.
 *          Returns 0 upon success.
 *          Returns a negative value upon failure.
 */
int
srbFileFstat(srbConn* conn, int storSysType, char *hostAddr, int fd_inx,
struct srbStat *statbuf)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) statbuf;

    argv[0] = storSysType;
    argv[1] = addrToSRBLong (hostAddr);
    argv[2] = fd_inx;

    retval = clCall (conn, F_E_FSTAT, argv, &clOut);

    if (retval >= 0) {
        /* For c90, pat it to 64 bit boundary */
        if (retval != retval / sizeof (int) * sizeof (int))
            retval = retval / sizeof (int) * sizeof (int) +
              sizeof (int);
        if (retval < sizeof (struct srbStat))
            return CLI_ERR_RETURN_LEN;
        else {
            /* Convert the elements of the srbStat struct. */
            ntohStat (statbuf);
            return 0;
        }
    } else {
        return retval;
    }
}

/*
 * srbFileChksum - checksum an extended file
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int fd_inx - The exf file descriptor fstat.
 *         int flags - not used
 *         unsigned long *ichksum - The output chksum.
 *	   srb_long_t myOffset - the offset and size to checksum
 *	   srb_long_t mySize - for whole file chksum, myOffset=mySize=0
 *
 * Output - stat result in statbuf.
 *          Returns 0 upon success.
 *          Returns a negative value upon failure.
 */
int
srbFileChksum (srbConn* conn, int fd_inx,
int flags, unsigned long *ichksum, srb_long_t myOffset, srb_long_t mySize)
{
    clArgBlock argv[4];
    srbResult *res;
    int resultLen;
    char retval[CHKSUM_STR_LEN + 1];
    int status;

    argv[0].isint = 1;
    argv[0].len = SIZEOF32;
    argv[0].u.integer = fd_inx;

    argv[1].isint = 1;
    argv[1].len = SIZEOF32;
    argv[1].u.integer = flags;

    argv[2].isint = 1;
    argv[2].len = 2*SIZEOF32;
    argv[2].u.ptr = (char*) &myOffset;

    argv[3].isint = 1;
    argv[3].len = 2*SIZEOF32;
    argv[3].u.ptr = (char*) &mySize;

    res = clFunct(conn, F_E_CHKSUM,(char *) &retval,&resultLen,0,argv,4);
    if ((status = clResultStatus(res)) != CLI_COMMAND_OK) {
        clClear(res);
        return status;
    }
    *ichksum = atoi (retval);
    return (0);
}

int
ntohStat (struct srbStat *statbuf)
{
    if (statbuf == NULL)
	return (-1);

#if defined(PORTNAME_c90)
    statbuf->st_size = (srb_size_t) htonl (statbuf->st_size);
#else
    {
	srbNtohll ((u_longlong_t *) &statbuf->st_size);
    }
#endif
    statbuf->st_dev = (srb_dev_t) ntohl (statbuf->st_dev);
    statbuf->st_ino = (srb_ino_t) ntohl (statbuf->st_ino);
    statbuf->st_mode = (srb_mode_t) ntohl (statbuf->st_mode);
#ifdef _WIN32
        statbuf->st_nlink = (srb_size_t) ntohs ((u_short)statbuf->st_nlink);
#else
    statbuf->st_nlink = (short) ntohs (statbuf->st_nlink);
#endif
    statbuf->st_uid = (srb_uid_t) ntohl (statbuf->st_uid);
    statbuf->st_gid = (srb_gid_t) ntohl (statbuf->st_gid);
    statbuf->st_rdev = (srb_dev_t) ntohl (statbuf->st_rdev);
    statbuf->st_atim = (srb_time_t) ntohl (statbuf->st_atim);
    statbuf->st_mtim = (srb_time_t) ntohl (statbuf->st_mtim);
    statbuf->st_ctim = (srb_time_t) ntohl (statbuf->st_ctim);
    statbuf->st_blksize = (srb_size_t) ntohl (statbuf->st_blksize);
    statbuf->st_blocks = (srb_size_t) ntohl (statbuf->st_blocks);
    statbuf->st_spare1 = (int) ntohl (statbuf->st_spare1);
    statbuf->st_spare2 = (int) ntohl (statbuf->st_spare2);
    statbuf->st_spare3 = (int) ntohl (statbuf->st_spare3);
    statbuf->st_vfstype = (int) ntohl (statbuf->st_vfstype);
    statbuf->st_vfs = (srb_size_t) ntohl (statbuf->st_vfs);
    statbuf->st_type = (srb_size_t) ntohl (statbuf->st_type);
#ifdef PORTNAME_aix
    statbuf->AIX_Reserved1 = (srb_uid_t) ntohl (statbuf->AIX_Reserved1);
    statbuf->AIX_Reserved2 = (srb_gid_t) ntohl (statbuf->AIX_Reserved2);
    statbuf->st_Access = (srb_ushort_t) ntohl (statbuf->st_Access);
#else
    statbuf->Reserved1 = (srb_uid_t) ntohl (statbuf->Reserved1);
    statbuf->Reserved2 = (srb_gid_t) ntohl (statbuf->Reserved2);
    statbuf->st_access = (srb_ushort_t) ntohl (statbuf->st_access);
#endif
#ifdef _WIN32
        statbuf->st_flag = (srb_ushort_t) ntohs ((u_short)statbuf->st_flag);
#else
    statbuf->st_flag = (srb_ushort_t) ntohs (statbuf->st_flag);
#endif /* WIN32 */
    statbuf->st_gen = (srb_size_t) ntohl (statbuf->st_gen);
	    
    return 0;
}

/*
 * srbFileMkdir - create a new directory
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int storSysType - Storage system type. 
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP (not supported for
 *              HTTP).
 *         char *hostAddr - The Host address of the SRB.
 *         char *filename - The Path name of the new directory. Must be an
 *          absolute path.
 *	   int mode - Same definition as in Unix.
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */
int
srbFileMkdir(srbConn* conn, int storSysType, char *hostAddr, char *filename, 
int mode) 
{
    return (_srbFileMkdir(conn, storSysType, hostAddr, filename,
      mode, DO_CHKDIR));
}

int
_srbFileMkdir(srbConn* conn, int storSysType, char *hostAddr, char *filename, 
int mode, int chdirFlag) 
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;
 
    argv[0] = storSysType;
    argv[1] = addrToSRBLong (hostAddr);
    argv[2] = addrToSRBLong (filename);
    argv[3] = mode;
    argv[4] = chdirFlag;
 
    retval = clCall (conn, F_E_MKDIR, argv, &clOut);
    return retval;
}

/*
 * srbFileChmod - Change the mode of a file or directory.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int storSysType - Storage system type. 
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP (not supported for
 *              HTTP).
 *         char *hostAddr - The Host address of the SRB.
 *         char *filename - The Path name of the file or directory. 
 *	    Must be an absolute path.
 *         int mode - Same definition as in Unix.
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 *
 */
int
srbFileChmod(srbConn* conn, int storSysType, char *hostAddr, char *filename, 
int mode) 
{
    return (_srbFileChmod (conn, storSysType, hostAddr, filename,
      mode, DO_CHKDIR));
}

int
_srbFileChmod(srbConn* conn, int storSysType, char *hostAddr, char *filename, 
int mode, int chdirFlag) 
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;
 
    clOut.retval = (char *) &clOutVal;

    argv[0] = storSysType;
    argv[1] = addrToSRBLong (hostAddr);
    argv[2] = addrToSRBLong (filename);
    argv[3] = mode;
    argv[4] = chdirFlag;
 
    retval = clCall (conn, F_E_CHMOD, argv, &clOut);
    return retval;
}
 
/*
 * srbFileRmdir - Remove a directory.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int storSysType - Storage system type. 
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP (not supported for
 *              HTTP).
 *         char *hostAddr - The Host address of the SRB.
 *         char *filename - The Path name of the directory to remove.
 *	    Must be an absolute path.
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */
int
srbFileRmdir(srbConn* conn, int storSysType, char *hostAddr, char *filename) 
{
    return (_srbFileRmdir(conn, storSysType, hostAddr, filename,
      DO_CHKDIR));
}
 
int
_srbFileRmdir(srbConn* conn, int storSysType, char *hostAddr, 
char *filename, int chdirFlag) 
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = storSysType;
    argv[1] = addrToSRBLong (hostAddr);
    argv[2] = addrToSRBLong (filename);
    argv[3] = chdirFlag;
 
    retval = clCall (conn, F_E_RMDIR, argv, &clOut);
    return retval;
}

/*
 * srbSetStorAttri - Set Attributes for a Storage System; establish connection.
 *   Currently only defined for FTP files, connects to an FTP Server and/or
 *   sets the current directory. *
 * Input - srbConn* conn - From srbConnect ().
 *         int storSysType - Storage system type. 
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP (not supported for
 *              HTTP).
 *         struct varlena *hostAddr - host address for the storage system,
 *                for FTP this is host address and an optional :port.
 *         struct varlena *userAuth - user authorization string, for FTP
 *                this is username to login as and optional :password.
 *         struct varlena *dirPath - directory to change to (cd)
 *                (dirPath or hostAddr may be null to leave unchanged)
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 *
 */

int
srbSetStorAttri(srbConn* conn, int storSysType, char *hostAddr,
char *userAuth, char *dirPath) 
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;
 
    clOut.retval = (char *) &clOutVal;
    argv[0] = storSysType;
    argv[1] = addrToSRBLong (hostAddr);
    argv[2] = addrToSRBLong (userAuth);
    argv[3] = addrToSRBLong (dirPath);

    retval = clCall (conn, F_E_SETSTORATTRI, argv, &clOut);
    return retval;
}

/*
 * srbObjOpen - Open a SRB object 
 *
 * Input - srbConn* conn - From srbConnect ().
 *	   char *objID - The SRB object ID to open. The objID is obtained
 *		through registration with MDAS. One or more conditions
 *		can be appended to the objID. Each condition must be 
 *		preceded by the character '&'. Currently, only one condition
 *		is supported. i.e., COPY=n (where n = replica number beginning
 *		with 0). e.g. foo&COPY=1 specifies opening replica number
 *		1 of data object "foo". 
 *         int oflag - Unix type open flag. O_CREAT is not supported.
 *	   char *collectionName - The name of the collection this objID
 *	 	belongs.
 *
 *
 * Output - Returns the object descriptor. 
 * 	    Returns a negative value upon failure.
 */

int
srbObjOpen(srbConn* conn, char *objID,
int oflag, char *collectionName)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;
 
    argv[0] = addrToSRBLong (objID);
    argv[1] = oflag;
    argv[2] = addrToSRBLong (collectionName);

    retval = clCall (conn, F_SRBO_OPEN, argv, &clOut);
    return retval;
}

/*
 * srbObjOpenWithTicket - Open a SRB object using a ticket.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *objID - The SRB object ID to open. The objID is obtained
 *              through registration with MDAS. objID may be NULL. In this
 *		case, SRB will open the object associated with the ticket.
 *              One or more conditions can be appended to the objID.
 *              Each condition must be preceded by the character '&'.
 *              Currently, only one condition is supported. i.e., COPY=n
 *              (where n = replica number beginning with 0).
 *              e.g. foo&COPY=1 specifies opening replica number
 *              1 of data object "foo".
 *         int oflag - Unix type open flag. O_CREAT is not supported.
 *         char *collectionName - The name of the collection this objID
 *              belongs.
 *	   char *ticket - The ticket for the object.
 *
 *
 * Output - Returns the object descriptor.
 *          Returns a negative value upon failure.
 */

int
srbObjOpenWithTicket (srbConn* conn, char *objID,
int oflag, char *collectionName, char *ticket)
{
    int fd;
    char objBuf[MAX_TOKEN];

    /* Compose the objID */

    if (objID == NULL)
	objID = "";
    strcpy (objBuf, objID);
    strcat (objBuf, "&TICKET='");
    strcat (objBuf, ticket);
    strcat (objBuf, "'");

    fd = srbObjOpen(conn, objBuf, oflag, collectionName);

    return (fd);
}

/*
 * srbObjCreate - Create a SRB object
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *         char *objID - The SRB object ID to create. The objID is a
 *              user defined name to be registered with MDAS. This 
 *		ID will be used for subsequent reference of the data
 *	        object.
 *              One or more conditions can be appended to the objID.
 *              Each condition must be preceded by the character '&'.
 *              Currently, two conditions are supported: 
 *		1) COPIES=MMM where MMM may be:
 *		   a) an integer n which means n replica should be created.
 *		   The "resourceName" input is the logical resource in which 
 *		   this object is to be stored. This logical resource must 
 *		   consist of at least n physical resources. e.g. foo&COPIES=2 
 *		   specifies the creation of two replica of data object "foo".
 *		   b) the keyword RR which means a single copy should be 
 *		   created in one of the physical resources belonging to 
 *		   the input logical resource ("resourceName") chosen in a 
 *		   Round-Robin fashion. e.g. foo&COPIES=RR.
 *		   c) the keyword RANDOM produces similar effect as the 
 *		   RR keyword. The only difference is the selection algorithm 
 *		   is random rather than Round-Robin. e.g. foo&COPIES=RANDOM.
 *		2) CONTAINER=containerName. This keyword specifies the
 *		object is to be placed in the given container. The
 *		container must have already been created using the 
 *		srbContainerCreate() call.
 *	   char *dataTypeName - Data type. e.g. "generic"
 *	   char *resourceName - The storage resource name. This may be a
 *		the name of a single resource or a resource group 
 *		(or logical resource) consisting of two or more physical 
 *		resources. 
 *		e.g. "mda18-unix-sdsc"
 *	   char *collectionName - The collection name. 
 *	   char *pathName - The file/DB path of the data.
 *         srb_long_t dataSize - File size. Used by HPSS to determine COS. 
 *	   -1 => don't know and the default COS will be used.
 *
 *
 * Output - Returns the object descriptor.
 *          Returns a negative value upon failure.
 */

int
srbObjCreate(srbConn* conn, int catType, char *objID, 
char *dataTypeName, char *resourceName, char *collectionName, 
char *pathName, srb_long_t dataSize)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    if (objID[0] == '\0')
	return (INP_ERR_FILENAME);
	
    clOut.retval = (char *) &clOutVal;

    argv[0] = catType;
    argv[1] = addrToSRBLong (objID);
    if (dataTypeName != NULL) {
        argv[2] = addrToSRBLong (dataTypeName);
    } else {
        if (!(argv[2] = addrToSRBLong (getenv("mdasDataTypeName")))) {
            argv[2] = addrToSRBLong (DefMdasDataTypeName);
        }
    }

    if (resourceName != NULL) {
        argv[3] = addrToSRBLong (resourceName);
    } else {
        if (!(argv[3] = addrToSRBLong (getenv("mdasResourceName")))) {
            argv[3] = addrToSRBLong (DefMdasResourceName);
        }
    }

    if (collectionName != NULL) {
        argv[4] = addrToSRBLong (collectionName);
    } else {
        if (!(argv[4] = addrToSRBLong (getenv("mdasCollectionName")))) {
            argv[4] = addrToSRBLong (DefMdasCollectionName);
        }
    }
    argv[5] = addrToSRBLong (pathName);

    argv[6] = addrToSRBLong ((char *) &dataSize);

    retval = clCall (conn, F_SRBO_CREATE, argv, &clOut);
    return retval;
}

/*
 * srbObjClose - Close an opened object
 *
 * Input - srbConn* conn - From srbConnect ().
 *	   int desc - The object descriptor (from the srbObjOpen call).
 *
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */

int
srbObjClose (srbConn* conn, int desc)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;
 
    clOut.retval = (char *) &clOutVal;

    argv[0] = desc;
 
    retval = clCall (conn, F_SRBO_CLOSE, argv, &clOut);
    return retval;
}

/*
 * srbObjUnlink - Unlink an SRB object
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *objID - The SRB object ID to unlink. The objID is obtained
 *              through registration with MDAS.
 *         char *collectionName - The name of the collection this objID
 *              belongs.
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 *
 */

int
srbObjUnlink (srbConn* conn, char *objID,
char *collectionName)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = addrToSRBLong (objID);
    argv[1] = addrToSRBLong (collectionName);

    retval = clCall (conn, F_SRBO_UNLINK, argv, &clOut);
    return retval;
}

/*
 * srbObjRead - Read len bytes of the SRB object into buf
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int desc - The object descriptor (from the srbObjOpen call) to read.
 *	   char *buf - The input buffer.
 *	   int len - The number of bytes to read.
 *
 * Output - Returns the length of bytes read.
 *          Returns a negative value upon failure.
 * the CALLER must have allocated enough space to hold the result returned
 */
 
int
srbObjRead(srbConn *conn, int desc, char *buf, int len)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) buf;
 
    argv[0] = desc;
    argv[1] = len;
 
    retval = clCall (conn, F_SRBO_READ, argv, &clOut);
    return retval;
}
 
/*
 * srbObjWrite - Write len bytes of buf into the Object fd
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int desc - The Object descriptor to write (from srbObjOpen or
 *         svrObjCreate).
 *         char *buf - The output buffer.
 *         int len - The length to write.
 *
 * Output - Returns the number of bytes written..
 *          Returns a negative value upon failure.
 *
 * Note : We cannot use the generic routine clCall() because the input
 * byte stream is not a char string.
 */
int
srbObjWrite(srbConn *conn, int desc, char *buf, int len)
{
    clArgBlock argv[2];
    srbResult *res;
    int resultLen;
    int retval;
 
    if (len <= 0)
        return 0;
 
    argv[0].isint = 1;
    argv[0].len = 4;
    argv[0].u.integer = desc;
 
    argv[1].isint = 0;
    argv[1].len = len;
    argv[1].u.ptr = (char*)buf;
 
    res = clFunct(conn, F_SRBO_WRITE,(char *) &retval,&resultLen,1,argv,2);
    if (clResultStatus(res) == CLI_COMMAND_OK) {
        clClear(res);
        return retval;
    } else {
        retval = clResultStatus(res);
        clClear(res);
        return retval;
    }
}

/*
 * srbObjSeek - Change the current read or write location on an srb obj file
 * currently.
 * Input - srbConn* conn - From srbConnect ().
 *         int desc - The object descriptor (from the srbObjOpen call) to seek.
 *         srb_long_t offset - The position of the next operation
 *         int whence - Same definition as in Unix.
 *              SEEK_SET - pointer is set to the value of the Offset parameter.
 *              SEEK_CUR - pointer is set to its current location plus the
 *                      value of the Offset parameter.
 *              SEEK_END - pointer is set to the size of the file plus the
 *                      value of the Offset parameter.
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */
 
srb_long_t
srbObjSeek(srbConn *conn, int desc, srb_long_t offset, int whence)
{
    clArgBlock argv[3];
    srbResult *res;
    int resultLen;
    srb_long_t retval;

    argv[0].isint = 1;
    argv[0].len = 4;
    argv[0].u.integer = desc;

    argv[1].isint = 1;
    argv[1].len = 2*SIZEOF32;
    argv[1].u.ptr = (char*) &offset;

    argv[2].isint = 1;
    argv[2].len = 4;
    argv[2].u.integer = whence;

    res = clFunct(conn, F_SRBO_SEEK,(char *) &retval,&resultLen,1,argv,3);
    if (clResultStatus(res) == CLI_COMMAND_OK) {
        clClear(res);
        return retval;
    } else {
        retval = clResultStatus(res);
        clClear(res);
        return retval;
    }
}

/*
 * srbObjSync - Sync an opened object (call fsync for UNIX FS)
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int desc - The object descriptor (from the srbObjOpen call).
 *
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */

int
srbObjSync (srbConn* conn, int desc) 
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = desc;

    retval = clCall (conn, F_SRBO_SYNC, argv, &clOut);
    return retval;
}


/*
 * _srbObjStat - Stat a srb Path.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - The catalog type.
 *         char *path - the SRB path.
 *	   int myType - file or dir state : IS_UNKNOWN, IS_FILE, IS_DIR_1,
 *			IS_DIR_2, IS_DIR_3, IS_DIR_4.
 *         stat *statbuf - The place to put the stat result.
 *
 * Output - stat result in statbuf.
 *          Returns 0 upon success.
 *          Returns a negative value upon failure.
 */
int
_srbObjStat(srbConn* conn, int catType, char *path, 
struct srbStat *statbuf, int myType)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) statbuf;

    argv[0] = catType;
    argv[1] = addrToSRBLong (path);
    argv[2] = myType;

    retval = clCall (conn, F_SRBO_STAT, argv, &clOut);

    if (retval >= 0) {
        /* For c90, pat it to 64 bit boundary */
        if (retval != retval / sizeof (int) * sizeof (int))
            retval = retval / sizeof (int) * sizeof (int) +
              sizeof (int);
        if (retval < sizeof (struct srbStat))
            return CLI_ERR_RETURN_LEN;
        else {
            /* Convert the elements of the srbStat struct. */
            ntohStat (statbuf);
            return 0;
        }
    } else {
        return retval;
    }
}

/*
 * srbObjStat - Stat a srb Path - The result is placed in statbuf which
 * is of type (strust stat) of the local fs.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - The catalog type.
 *         char *path - the SRB path.
 *         stat *statbuf - The place to put the stat result.
 *
 * Output - stat result in statbuf.
 *          Returns 0 upon success.
 *          Returns a negative value upon failure.
 */
int
srbObjStat(srbConn* conn, int catType, char *path, struct stat *statbuf)
{
    struct srbStat mySrbStat;
    int status;

    status = _srbObjStat (conn, catType, path, &mySrbStat, IS_UNKNOWN);
    if (status >= 0)
	status = srbToLocalStat (&mySrbStat, statbuf);

    return status;
}
   
/*
 * srbObjStat64 - Stat a srb Path - The result is placed in statbuf which
 * is of type (strust stat) of the local fs.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - The catalog type.
 *         char *path - the SRB path.
 *         stat *statbuf - The place to put the stat result.
 *
 * Output - stat result in statbuf.
 *          Returns 0 upon success.
 *          Returns a negative value upon failure.
 */
#if !defined(PORTNAME_c90)
int
srbObjStat64 (srbConn* conn, int catType, char *path, struct stat64 *statbuf)
{
    struct srbStat mySrbStat;
    int status;

    status = _srbObjStat (conn, catType, path, &mySrbStat, IS_UNKNOWN);
    if (status >= 0)
	status = srbToLocalStat64 (&mySrbStat, statbuf);

    return status;
}
#endif	/* PORTNAME_c90 */
   
/*
 * srbObjGetdents - The SRB equivalent of the getdents call.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - The catalog type.
 *         int fd - the opened fd.
 *         dirent_t *buf - The place to put the dirents result.
 *	   size_t nbyte - The max number of bytes to output.
 *
 * Output - dirents result in buf.
 *          Returns 0 upon success.
 *          Returns a negative value upon failure.
 */
int
srbObjGetdents (srbConn* conn, int catType, int fd, dirent_t *buf, 
size_t nbyte)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) buf;

    argv[0] = catType;
    argv[1] = fd;
    argv[2] = nbyte;

    retval = clCall (conn, F_SRBO_GET_DENTS, argv, &clOut);

    if (retval >= 0) {
        /* For c90, pat it to 64 bit boundary */
        if (retval != retval / sizeof (int) * sizeof (int)) {
            retval = retval / sizeof (int) * sizeof (int) +
              sizeof (int);
        } else {
            /* Convert the elements of the dirent_t. */
	    if (retval > 0)
                ntohDirent (buf, retval);
        }
    }
    return retval;
}

/*
 * srbObjGetdents64 - The SRB equivalent of the getdents64 call.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - The catalog type.
 *         int fd - the opened fd.
 *         dirent_t *buf - The place to put the dirents result.
 *	   size_t nbyte - The max number of bytes to output.
 *
 * Output - dirents result in buf.
 *          Returns 0 upon success.
 *          Returns a negative value upon failure.
 */
int
srbObjGetdents64 (srbConn* conn, int catType, int fd, dirent64_t *buf, 
size_t nbyte)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) buf;

    argv[0] = catType;
    argv[1] = fd;
    argv[2] = nbyte;

    retval = clCall (conn, F_SRBO_GET_DENTS64, argv, &clOut);

    if (retval >= 0) {
        /* For c90, pat it to 64 bit boundary */
        if (retval != retval / sizeof (int) * sizeof (int)) {
            retval = retval / sizeof (int) * sizeof (int) +
              sizeof (int);
        } else {
            /* Convert the elements of the dirent_t. */
	    if (retval > 0)
                ntohDirent64 (buf, retval);
        }
    }
    return retval;
}

/*
 * srbCollSeek - Seek into a collection. A collection must have been
 *		 opened using srbObjOpen.
 * Input - srbConn* conn - From srbConnect ().
 *         int desc - The object descriptor (from the srbObjOpen call) to seek.
 *         int offset - The position of the next operation
 *         int whence - Same definition as in Unix.
 *              SEEK_SET - pointer is set to the value of the Offset parameter.
 *              SEEK_CUR - pointer is set to its current location plus the
 *                      value of the Offset parameter.
 *              SEEK_END - pointer is set to the size of the file plus the
 *                      value of the Offset parameter.
 *	  int is64Flag - valid vaiue: IS_64_BIT or IS_32_BIT.
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */
 
int
srbCollSeek(srbConn *conn, int desc, int offset, int whence, int is64Flag)
{
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;
    int retval;

    clOut.retval = (char *) &clOutVal;

    argv[0] = desc;
    argv[1] = offset;
    argv[2] = whence;
    argv[3] = is64Flag;


    retval = clCall (conn, F_SRBC_SEEK, argv, &clOut);
    return retval;
}

/*
 * srbObjProxyOpr - Proxy Operation request.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         ProxyOprId operation -The type of proxy operation. Valid operations:
 *              OPR_COPY - Copy from the object descriptor given in sourceInp
 *              to the object descriptor given in destInp.
 *              If successful, the number of bypes copied is returned. -ive
 *              values means failure.
 *
 *	   int inputInt1 - intput integer 1.
 *	   int inputInt2 - intput integer 2.
 *	   int inputInt3 - intput integer 3.
 *	   int inputInt4 - intput integer 4.
 *         char *inputStr1 - Input String 1.
 *         char *inputStr2 - Input String 2.
 *         char *inputStr3 - Input String 3.
 *         char *inputStr4 - Input String 4.
 *         char *inputStr5 - Input String 5.
 *         byteStream *inputBStrm1 - Input Byte stream 1.
 *         byteStream *inputBStrm2 - Input Byte stream 2.
 *
 *
 * Output - Returns 0 or positive value upon success.
 *          Returns a negative value upon failure.
 *	    outBuf - any byte stream output.
 */

int
srbObjProxyOpr (srbConn *conn, ProxyOprId operation, 
int inputInt1, int inputInt2, int inputInt3, int inputInt4,
char *inputStr1, char *inputStr2, char *inputStr3, char *inputStr4, 
char *inputStr5, byteStream *inputBStrm1, byteStream *inputBStrm2,
char *outBuf)
{
    srbResult *res;
    clArgBlock argv[MAXFMGRARGS];
    int status;
    int resultLen;

    argv[0].isint = 1;
    argv[0].len = 4;
    argv[0].u.integer = operation;

    argv[1].isint = 1;
    argv[1].len = 4;
    argv[1].u.integer = inputInt1;

    argv[2].isint = 1;
    argv[2].len = 4;
    argv[2].u.integer = inputInt2;

    argv[3].isint = 1;
    argv[3].len = 4;
    argv[3].u.integer = inputInt3;

    argv[4].isint = 1;
    argv[4].len = 4;
    argv[4].u.integer = inputInt4;

    argv[5].isint = 0;
    if (inputStr1 == NULL)
        inputStr1 = "";
    argv[5].len = strlen (inputStr1) + 1;
    argv[5].u.ptr = inputStr1;

    argv[6].isint = 0;
    if (inputStr2 == NULL)
        inputStr2 = "";
    argv[6].len = strlen (inputStr2) + 1;
    argv[6].u.ptr = inputStr2;

    argv[7].isint = 0;
    if (inputStr3 == NULL)
        inputStr3 = "";
    argv[7].len = strlen (inputStr3) + 1;
    argv[7].u.ptr = inputStr3;

    argv[8].isint = 0;
    if (inputStr4 == NULL)
        inputStr4 = "";
    argv[8].len = strlen (inputStr4) + 1;
    argv[8].u.ptr = inputStr4;

    argv[9].isint = 0;
    if (inputStr5 == NULL)
	inputStr5 = "";
    argv[9].len = strlen (inputStr5) + 1;
    argv[9].u.ptr = inputStr5;

    argv[10].isint = 0;
    if (inputBStrm1 == NULL) {
        argv[10].len = 1;
        argv[10].u.ptr = "";
    } else {    
        argv[10].len = inputBStrm1->len;
        argv[10].u.ptr = inputBStrm1->bytes;
    }

    argv[11].isint = 0;
    if (inputBStrm2 == NULL) {
        argv[11].len = 1;
        argv[11].u.ptr = "";
    } else {    
        argv[11].len = inputBStrm2->len;
        argv[11].u.ptr = inputBStrm2->bytes;
    }

    res = clFunct(conn, F_SRBO_PROXY_OPR, outBuf,&resultLen,0,argv,MAXFMGRARGS);
    if ((status = clResultStatus(res)) != CLI_COMMAND_OK) {
        clClear(res);
        return status;
    }

    clClear(res);

    return status;
}


/*
 * srbObjReplicate - Make a copy of an SRB object
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *         char *objID - The SRB object ID to unlink. The objID is obtained
 *              through registration with MDAS.
 *         char *collectionName - The name of the collection this objID
 *              belongs.
 *         char *origPathName - The file/DB path of the original copy.
 *         char *newResourceName - The storage resource name of the new copy.
 *              e.g. "mda18-unix-sdsc"
 *         char *newPathName - The file/DB path of the new copy.
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 *
 */

int
srbObjReplicate (srbConn* conn, int catType, char *objID, char *collectionName, 
char *newResourceName, char *newPathName)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = catType;
    argv[1] = addrToSRBLong (objID);
    argv[2] = addrToSRBLong (collectionName);
    argv[3] = addrToSRBLong (newResourceName);
    argv[4] = addrToSRBLong (newPathName);

    retval = clCall (conn, F_SRBO_REPLICATE, argv, &clOut);
    return retval;
}

/*
 * srbObjMove - Move a copy of an SRB object to a new location
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *         char *objID - The SRB object ID to unlink. The objID is obtained
 *              through registration with MDAS.
 *         char *collectionName - The name of the collection this objID
 *              belongs.
 *         char *newResourceName - The storage resource name of the new copy.
 *              e.g. "mda18-unix-sdsc"
 *         char *newPathName - The file/DB path of the new copy.
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 *
 */

int
srbObjMove (srbConn* conn, int catType, char *objID, char *collectionName,
char *newResourceName, char *newPathName)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = catType;
    argv[1] = addrToSRBLong (objID);
    argv[2] = addrToSRBLong (collectionName);
    argv[3] = addrToSRBLong (newResourceName);
    argv[4] = addrToSRBLong (newPathName);

    retval = clCall (conn, F_SRBO_MOVE, argv, &clOut);
    return retval;
}

/*
 * srbGetDatasetInfo - Get Info on a SRB data object. 
 *
 * Input : srbConn* conn,
 *         int catType - The catalog type - 0 = MDAS_CATALOG
 *	   char *objID,
 *	   char *collectionName
 *         mdasC_sql_result_struct *myresult - A pointer to a user supplied 
 *          mdasC_sql_result_struct.
 * 	   int rowsWanted - number of rows of result wanted.
 *	   char *mode
 * Output:
 *	   A mdasC_sql_result_struct *myresult
 *
 * Return value :
 *      0 - success; myresult->continuation_index >= 0, ==> more results from
 *          the query. Use srbGetMoreRows() to retrieve more rows.
 *	a negative value - failure
 */

int
srbGetDatasetInfo(srbConn* conn, int catType, char *objID, 
char *collectionName, mdasC_sql_result_struct *myresult, int rowsWanted,
char *mode)
{
    srbResult *res;
    char *buf;
    int buflen;
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;
    mdasC_sql_result_struct *tmpResult;

 
    argv[0] = catType;
    argv[1] = addrToSRBLong (objID);
    argv[2] = addrToSRBLong (collectionName);
    argv[3] = rowsWanted;
    argv[4] = addrToSRBLong (mode);

    if ((buflen = getSqlResultLen (DATASET_INFO_COL_CNT, rowsWanted)) <= 0)
	return CLI_ERR_MALLOC;

    buf = malloc (buflen);
    if (buf == NULL)
        return CLI_ERR_MALLOC;


    clOut.retval = (char *) buf;

    retval = clCall (conn, F_SRBO_GET_LOID_INFO, argv, &clOut);

    if (retval < 0) {
        free (buf);
	return retval;
    }

#ifdef foo
    strToSqlResult (buf, myresult);
#endif
    retval = unpackMsg (buf, (char **) &tmpResult,
     sqlResultStruct_PF, NULL);

    free (buf);

    if (retval < 0) {
        return (retval);
    }

    *myresult = *tmpResult;
    free (tmpResult);

    if (myresult->result_count > 0)
        return 0;       /* success */
    else
        return CLI_ERR_RETURN_LEN;
}

/*
 * srbGetDataDirInfo - Get metadata info by querying the MDAS catalog.
 * The server uses the input qval[][] and selval[] array to compose
 * and execute SQL queries and returns the query result in myresult.
 * The selval[] array specifies a list of attrbutes to retrieve and
 * qval[][] specifies a lists of "=" predicates to search.
 * Both selval[] and qval[][] must be arrays of size MAX_DCS_NUM and
 * are indexed by values given in mdasC_db2_externs.h under the heading
 * DCS-ATTRIBUTE-INDEX DEFINES. 
 *
 * For the selval[] array, setting an element of the array to 1 means that 
 * the attribute associated with this element is to be retrieved. e.g., 
 * selval[USER_NAME] = 1; means the "user_name" attribute is to be retrieved.
 *
 * The qval[][] array specifies the "=" predicates to search. e.g., 
 * sprintf(qval[DATA_NAME],"'%s'", "unixFileObj1"); means that the search
 * condition includes the term (data_name = "unixFileObj1"). 
 *
 * An example of srbGetDataDirInfo:
 *
 *  mdasC_sql_result_struct myresult;
 *  char qval[MAX_DCS_NUM][MAX_TOKEN];
 *  int selval[MAX_DCS_NUM];
 *
 * for (i = 0; i < MAX_DCS_NUM; i++) {
 *      selval[i] = 0;
 *      sprintf(qval[i],"");
 *  }
 *  sprintf(qval[DATA_NAME],"'%s'",argv[2]);
 *  selval[PATH_NAME] = 1;
 *  selval[RSRC_NAME] = 1;
 *	.
 *	.
 *  if (srbGetDataDirInfo(conn, MDAS_CATALOG, qval, selval, &myresult) < 0) {
 *      fprintf(stderr, "can't srbGetDataDirInfo \n");
 *      exit_nicely(conn);
 *  }
 *
 *
 *  
 * Input : srbConn* conn,
 *	   int catType - The catalog type - 0 = MDAS_CATALOG
 *	   char qval[][MAX_TOKEN] - A point to a user supplied
 *	     char qval[MAX_DCS_NUM][MAX_TOKEN] array. 
 *	   int *selval -  A pointer to a user supplied selval[MAX_DCS_NUM]
 *	     array.
 *         int rowsWanted - number of rows of result wanted.
 *	   
 * Output:
 *         A mdasC_sql_result_struct *myresult
 *
 * Return value :
 *      0 - success; myresult->continuation_index >= 0, ==> more results from
 *          the query. Use srbGetMoreRows() to retrieve more rows.
 *      a negative value - failure
 *
 * Note : We cannot use the generic routine clCall() because the input
 * byte stream is not a char string.
 */

#define CONV_TO_GEN_QUERY       1 

int
srbGetDataDirInfo(srbConn* conn, int catType, char qval[][MAX_TOKEN],
int *selval, mdasC_sql_result_struct *myresult, int rowsWanted)
{
#ifdef CONV_TO_GEN_QUERY
    genQuery_t myGenQuery;
    int i;
    int status;
    char *tmpPtr;

    qvalToGenQuery (qval, selval, &myGenQuery);

    status = srbGenQuery (conn, catType, NULL, &myGenQuery, myresult,
     rowsWanted);

    clearGenQuery (&myGenQuery);

    return (status);

#else	/* CONV_TO_GEN_QUERY */

    srbResult *res;
    int resultLen;
    clArgBlock argv[4];
    char *buf;
    int buflen;
    int i;
    int status;
#ifdef PORTNAME_c90
    /* Have to convert 64 bit integer to 32 bit integer */
    struct int32Array mySelval[MAX_DCS_NUM / 2 + 1]; 
    int loopCnt;
#else
    int mySelval[MAX_DCS_NUM];
#endif
    int selCnt = 0;

    argv[0].isint = 1;
    argv[0].len = 4;
    argv[0].u.integer = catType;

    argv[1].isint = 0;
    argv[1].len = MAX_DCS_NUM * MAX_TOKEN;
    argv[1].u.ptr = (char*) qval;
 
#ifdef PORTNAME_c90
    if ((MAX_DCS_NUM / 2 * 2) != MAX_DCS_NUM) {		/* odd */
	loopCnt = MAX_DCS_NUM / 2 + 1;
    } else {
	loopCnt = MAX_DCS_NUM / 2;
    }
    for (i = 0; i < loopCnt; i++) {
	mySelval[i].myInt0 = htonl (selval[2 * i]);
	mySelval[i].myInt1 = htonl (selval[2 * i + 1]);
	if (selval[2 * i] != 0)
	    selCnt ++;
	if (selval[2 * i + 1] != 0)
	    selCnt ++;
    }
	
#else
    for (i = 0; i < MAX_DCS_NUM; i++) {
        mySelval[i] = htonl (selval[i]);
	if (selval[i] != 0)
	    selCnt ++;
    }
#endif
    argv[2].isint = 0;
    argv[2].len = MAX_DCS_NUM * 4;
    argv[2].u.ptr = (char*) mySelval;

    argv[3].isint = 1;
    argv[3].len = 4;
    argv[3].u.integer = rowsWanted;

    if ((buflen = getSqlResultLen (selCnt, rowsWanted)) <= 0)
        return CLI_ERR_MALLOC;

    buf = malloc (buflen);
    if (buf == NULL)
	return CLI_ERR_MALLOC;

    res = clFunct(conn, F_SRBO_GET_DATADIR_INFO, buf,&resultLen,0,argv,4);
    if ((status = clResultStatus(res)) != CLI_COMMAND_OK) {
	myresult->result_count = 0;
        myresult->row_count = 0;
        myresult->continuation_index = -1;
	free (buf);
	clClear(res);
	return status;
    }

    clClear(res);

    if (resultLen == 0) {
	myresult->result_count = 0;
	myresult->row_count = 0;
	myresult->continuation_index = -1;
	return CLI_NO_ANSWER;
    }

    strToSqlResult (buf, myresult);

    free (buf);

    if (myresult->result_count > 0)
        return 0;       /* success */
    else
        return CLI_NO_ANSWER;
#endif  /* CONV_TO_GEN_QUERY */
}

/* This is the same as srbGetDataDirInfo except with an additional 
 * char *myMcatZone input */

int
srbGetDataDirInfoWithZone(srbConn* conn, int catType, char *myMcatZone,
char qval[][MAX_TOKEN],
int *selval, mdasC_sql_result_struct *myresult, int rowsWanted)
{
#ifdef CONV_TO_GEN_QUERY
    genQuery_t myGenQuery;
    int i;
    int status;
    char *tmpPtr;

    qvalToGenQuery (qval, selval, &myGenQuery);

    status = srbGenQuery (conn, catType, myMcatZone, &myGenQuery, myresult,
     rowsWanted);

    clearGenQuery (&myGenQuery);

    return (status);

#else   /* CONV_TO_GEN_QUERY */
    int status;

#ifdef FED_MCAT
    if (myMcatZone != NULL && 
     strlen (qval[DATA_GRP_NAME]) == 0 &&
     strlen (qval[PARENT_COLLECTION_NAME]) == 0 &&
     strlen (qval[CONTAINER_NAME]) == 0) {
        /* XXXXX this is a hack to pass on the mcatName. Need to change the API
         */
        sprintf (qval[DATA_GRP_NAME], "MCAT_NAME=%s", myMcatZone);
    }
#endif
    status = srbGetDataDirInfo (conn, catType, qval, selval, myresult, 
     rowsWanted);
    return (status);
#endif	/* CONV_TO_GEN_QUERY */
}

/*
 * srbGenQuery - This is a more compact form of srbGetDataDirInfo.
 * Instead of using fixed array of selval and qval, it uses the
 * genQuery struct.
 *
 *
 * Input : srbConn* conn,
 *         int catType - The catalog type - 0 = MDAS_CATALOG
 *         char *myMcatZone - The MCAT zone for this query. Only valid
 *          for version 3.x.x and above. For version 2.x.x, this should be NULL.
 *          genQuery_t *myGenQuery - The input query in th form of genQuery_t
 *           instead of selval and qval of srbGetDataDirInfo.
 *         mdasC_sql_result_struct *myresult - The output query result.
 *         int rowsWanted - number of rows of result wanted.
 *
 * Output:
 *         A mdasC_sql_result_struct *myresult
 *
 * Return value :
 *      0 - success; myresult->continuation_index >= 0, ==> more results from
 *          the query. Use srbGetMoreRows() to retrieve more rows.
 *      a negative value - failure
 *
 * Note : We cannot use the generic routine clCall() because the input
 * byte stream is not a char string.
 */

int
srbGenQuery (srbConn* conn, int catType, char *myMcatZone,
genQuery_t *myGenQuery, mdasC_sql_result_struct *myresult, int rowsWanted)
{
    srbResult *res;
    int resultLen;
    clArgBlock argv[4];
    struct varlena *packedResult;
    byteStream myBS;
    char *buf;
    int buflen;
    int i;
    int status;
    mdasC_sql_result_struct *tmpResult;

    argv[0].isint = 1;
    argv[0].len = 4;
    argv[0].u.integer = catType;

    argv[1].isint = 0;
    if (myMcatZone == NULL)
        myMcatZone = "";
    argv[1].len = strlen (myMcatZone) + 1;
    argv[1].u.ptr = (char*) myMcatZone;

    status = packMsg ((char *) myGenQuery, &packedResult, genQuery_PF,
     NULL);

   if (status < 0) {
        fprintf (stderr, "srbGenQuery: packMsg error, status = %d\n", status);
        return status;
    }

    myBS.len = VARSIZE (packedResult) - VARHDRSZ;
    myBS.bytes = VARDATA(packedResult);

    argv[2].isint = 0;
    argv[2].len = myBS.len;
    argv[2].u.ptr = (char *) myBS.bytes;

    argv[3].isint = 1;
    argv[3].len = 4;
    argv[3].u.integer = rowsWanted;

    if ((buflen = getSqlResultLen (myGenQuery->selInxCnt, rowsWanted)) <= 0)
        return CLI_ERR_MALLOC;

    buf = malloc (buflen);
    if (buf == NULL)
        return CLI_ERR_MALLOC;

    res = clFunct(conn, F_GEN_QUERY, buf,&resultLen,0,argv,4);
    if ((status = clResultStatus(res)) != CLI_COMMAND_OK) {
        myresult->result_count = 0;
        myresult->row_count = 0;
        myresult->continuation_index = -1;
        free (buf);
        clClear(res);
        return status;
    }

    clClear(res);

    if (resultLen == 0) {
        myresult->result_count = 0;
        myresult->row_count = 0;
        myresult->continuation_index = -1;
        return CLI_NO_ANSWER;
    }

#ifdef foo
    strToSqlResult (buf, myresult);
#else
    status = unpackMsg (buf, (char **) &tmpResult,
     sqlResultStruct_PF, NULL);

    if (status < 0) {
        free (buf);
        return (status);
    }
    *myresult = *tmpResult;
    free (tmpResult);
#endif

    free (buf);

    if (myresult->result_count > 0)
        return 0;       /* success */
    else
        return CLI_NO_ANSWER;
}

/* strToSqlResult - convert the sql results from SRB to mdasC_sql_result_struct.
 * 
 * Input - char *strFromSrb - The byte stream returned from the SRB.
 * 	   mdasC_sql_result_struct *myresult - The place to put the output.
 *
 * Output - 0 - success
 *          a negative value - Failure
 */

int
strToSqlResult (char *strFromSrb, mdasC_sql_result_struct *myresult)
{
    char *ptr;
    INT32 i;
    int resLen;
#if defined(PORTNAME_c90)
    struct int32Array *iptr;
#endif

    ptr = strFromSrb;
#if defined(PORTNAME_c90)
    iptr = (struct int32Array *) ptr;
    myresult->result_count = sToL (ntohl (iptr->myInt0));
    myresult->row_count = sToL (ntohl (iptr->myInt1));
    ptr += SIZEOF32;
    ptr += SIZEOF32;
    iptr = (struct int32Array *) ptr;
    myresult->continuation_index = sToL (ntohl (iptr->myInt0));
    ptr += SIZEOF32;
#else
    myresult->result_count = ntohl (*(INT32 *) ptr);
    ptr += SIZEOF32;
    myresult->row_count = ntohl (*(INT32 *) ptr);
    ptr += SIZEOF32;
    myresult->continuation_index = ntohl (*(INT32 *) ptr);
    ptr += SIZEOF32;
#endif


    for (i = 0; i < myresult->result_count ; i++){
 
        /* Add one for the NULL at end of str */

        resLen = strlen (ptr) + 1;
	myresult->sqlresult[i].tab_name = (char *) malloc (resLen);
	strcpy (myresult->sqlresult[i].tab_name, ptr);
	ptr += resLen;
        resLen = strlen (ptr) + 1;
	myresult->sqlresult[i].att_name = (char *) malloc (resLen);
	strcpy (myresult->sqlresult[i].att_name, ptr);
	ptr += resLen;
        resLen = strlen (ptr) + 1;
	if (myresult->row_count <= MAX_ROW)
	    myresult->sqlresult[i].values = (char *) malloc 
	      (MAX_ROW * MAX_DATA_SIZE);
	else
            myresult->sqlresult[i].values = (char *) malloc 
              (myresult->row_count * MAX_DATA_SIZE);

	memcpy (myresult->sqlresult[i].values, ptr, 
	  myresult->row_count * MAX_DATA_SIZE);
	ptr += myresult->row_count * MAX_DATA_SIZE;
    }

    if (myresult->result_count > 0) {
        return 0;       /* success */
    } else {
	myresult->row_count = 0;
	myresult->continuation_index = -1;
        return CLI_ERR_RETURN_LEN;
    }
}

/*
 * srbRegisterDataset - Register a SRB dataset
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *         char *objID - The SRB object ID to create. The objID is obtained
 *              through registration with MDAS.
 *         char *dataTypeName - Data type. e.g. "generic"
 *         char *resourceName - The storage resource name.
 *              e.g. "mda18-unix-sdsc"
 *         char *collectionName - The collection name.
 *         char *pathName - The file/DB path of the data.
 *         srb_long_t dataSize - The size of the dataset if known. 0 = unknown.
 *
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

int
srbRegisterDataset (srbConn* conn, int catType, char *objID, 
char *dataTypeName, char *resourceName, char *collectionName,
char *pathName, srb_long_t dataSize)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = catType;
    argv[1] = addrToSRBLong (objID);
    if (dataTypeName != NULL) {
        argv[2] = addrToSRBLong (dataTypeName);
    } else {
        if (!(argv[2] = addrToSRBLong (getenv("mdasDataTypeName")))) {
            argv[2] = addrToSRBLong (DefMdasDataTypeName);
        }
    }

    if (resourceName != NULL) {
        argv[3] = addrToSRBLong (resourceName);
    } else {
        if (!(argv[3] = addrToSRBLong (getenv("mdasResourceName")))) {
            argv[3] = addrToSRBLong (DefMdasResourceName);
        }
    }

    if (collectionName != NULL) {
        argv[4] = addrToSRBLong (collectionName);
    } else {
        if (!(argv[4] = addrToSRBLong (getenv("mdasCollectionName")))) {
            argv[4] = addrToSRBLong (DefMdasCollectionName);
        }
    }
    argv[5] = addrToSRBLong (pathName);
    argv[6] = addrToSRBLong ((char *) &dataSize);

    retval = clCall (conn, F_SRBO_REGISTER_DATASET, argv, &clOut);
    return retval;
}

/*
 * srbRegDatasetInternal - Server version of register a SRB dataset. Only
 * a privileged user api.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *         char *objID - The SRB object ID to create. The objID is obtained
 *              through registration with MDAS.
 *	   char *userName - owner of dataset.
 *	   char *access - access mode.
 *	   char *domainName - domain of owner
 *         char *dataTypeName - Data type. e.g. "generic"
 *         char *resourceName - The storage resource name.
 *              e.g. "mda18-unix-sdsc"
 *         char *collectionName - The collection name.
 *         char *pathName - The file/DB path of the data.
 *         srb_long_t dataSize - The size of the dataset if known. 0 = unknown.
 *	   int rmObjTypeInx - the objTypeInx of the dataset. If this index
 *	         is >= 0, unlink the object if the registration failed.
 *	   char *rmObjResLoc - If rmObjTypeInx >= 0, the Resource location
 *               of the object.
 *
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

int 
srbRegDatasetInternal (srbConn* conn, int catType, char *objID,
char *userName, char *access, char *domainName,
char *dataTypeName, char *resourceName, char *collectionName, 
char *pathName, srb_long_t dataSize,
int rmObjTypeInx, char *rmObjResLoc)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = catType;
    argv[1] = addrToSRBLong (objID);
    argv[2] = addrToSRBLong (userName);
    argv[3] = addrToSRBLong (access);
    argv[4] = addrToSRBLong (domainName);
    if (dataTypeName != NULL) {
        argv[5] = addrToSRBLong (dataTypeName);
    } else {
        if (!(argv[5] = addrToSRBLong (getenv("mdasDataTypeName")))) {
            argv[5] = addrToSRBLong (DefMdasDataTypeName);
        }
    }

    if (resourceName != NULL) {
        argv[6] = addrToSRBLong (resourceName);
    } else {
        if (!(argv[6] = addrToSRBLong (getenv("mdasResourceName")))) {
            argv[6] = addrToSRBLong (DefMdasResourceName);
        }
    }

    if (collectionName != NULL) {
        argv[7] = addrToSRBLong (collectionName);
    } else {
        if (!(argv[7] = addrToSRBLong (getenv("mdasCollectionName")))) {
            argv[7] = addrToSRBLong (DefMdasCollectionName);
        }
    }
    argv[8] = addrToSRBLong (pathName);
    argv[9] = addrToSRBLong ((char *) &dataSize);
    argv[10] = rmObjTypeInx;
    argv[11] = addrToSRBLong (rmObjResLoc);

    retval = clCall (conn, F_SRBO_REG_DATASET_INT, argv, &clOut);
    return retval;
}

/*
 * srbUnregisterDataset - Unregister an SRB object
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *objID - The SRB object ID to unlink. The objID is obtained
 *              through registration with MDAS.
 *         char *collectionName - The name of the collection this objID
 *              belongs.
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 *
 */

int
srbUnregisterDataset (srbConn* conn, char *objID,
char *collectionName)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = addrToSRBLong (objID);
    argv[1] = addrToSRBLong (collectionName);

    retval = clCall (conn, F_UNREGISTER_DATASET, argv, &clOut);
    return retval;
}

/*
 * srbRegisterReplica - Register a SRB Replica
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *         char *objID - The SRB object ID to create. The objID is obtained
 *              through registration with MDAS.
 *         char *collectionName - The collection name.
 *         char *origPathName - The orighnal file/DB path of the data.
 *              (This entry can be NULL).
 *         char *newResourceName - The new storage resource name.
 *              e.g. "mda18-unix-sdsc"
 *         char *newPathName - The new file/DB path of the data.
 *         char *userName - The new file/DB path of the data.
 *         struct varlena *domainName - The domain name to which the object
 *         belongs. Valid domains - "sdsc".
 *
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

int
srbRegisterReplica (srbConn* conn, int catType, char *objID, 
char *collectionName, char *origResourceName, char *origPathName,
char *newResourceName, char *newPathName,
char *userName, char *domainName)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = catType;
    argv[1] = addrToSRBLong (objID);

    if (collectionName != NULL) {
        argv[2] = addrToSRBLong (collectionName);
    } else {
        if (!(argv[2] = addrToSRBLong (getenv("mdasCollectionName")))) {
            argv[2] = addrToSRBLong (DefMdasCollectionName);
        }
    }

    argv[3] = addrToSRBLong (origResourceName);
    argv[4] = addrToSRBLong (origPathName);
    argv[5] = addrToSRBLong (newResourceName);
    argv[6] = addrToSRBLong (newPathName);
    argv[7] = addrToSRBLong (userName);
    argv[8] = addrToSRBLong (domainName);

    retval = clCall (conn, F_REGISTER_REPLICA, argv, &clOut);
    return retval;
}

/*
 * srbModifyDataset - Modify a SRB dataset
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *         char *objID - The SRB object ID to modify. The objID must
 *              already have been registered with the MDAS catalog.
 *         char *collectionName - The name of the collection this objID
 *              belongs.
 *         char *resourceName - The storage resource name.
 *              e.g. "mda18-unix-sdsc"
 *         char *pathName - The file/DB path of the data.
 *	   char *dataValue1 - Input value 1.
 *	   char *dataValue2 - Input value 2.
 *	   int retractionType - The type of retraction. See srbC_mdas_externs.h
 *				for the retractionType definition.
 *
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

int
srbModifyDataset (srbConn* conn, int catType, char *objID,
char *collectionName, char *resourceName, char *pathName, char *dataValue1,
char *dataValue2, int retractionType)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = catType;
    argv[1] = addrToSRBLong (objID);
    argv[2] = addrToSRBLong (collectionName);
    if (resourceName != NULL) {
        argv[3] = addrToSRBLong (resourceName);
    } else {
        if (!(argv[3] = addrToSRBLong (getenv("mdasResourceName")))) {
            argv[3] = addrToSRBLong (DefMdasResourceName);
        }
    }

    argv[4] = addrToSRBLong (pathName);
    argv[5] = addrToSRBLong (dataValue1);
    argv[6] = addrToSRBLong (dataValue2);
    argv[7] = retractionType;

    retval = clCall (conn, F_SRBO_MODIFY_DATASET, argv, &clOut);
    return retval;
}

/*
 * srbCreateCollect - Create a SRB collection
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *         char *parentCollect- The parent collection in which to create
 *		the new collection.
 *         char *newCollect - The name of the collection to create.
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

int
srbCreateCollect (srbConn* conn, int catType,
char *parentCollect, char *newCollect)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    if (newCollect[0] == '\0')
	return INP_ERR_FILENAME;

    clOut.retval = (char *) &clOutVal;

    argv[0] = catType;
    argv[1] = addrToSRBLong (parentCollect);
    argv[2] = addrToSRBLong (newCollect);

    retval = clCall (conn, F_CREATE_COLLECTION, argv, &clOut);
    return retval;
}

/*
 * srbListCollect - List a SRB collection
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *         char *collectionName- The collection to list.
 *         char *flag - "C" - non-recursive. "R" - recursive
 *	   mdasC_sql_result_struct *listResult - The address points to the 
 *	     result.  A pointer to a user supplied mdasC_sql_result_struct.
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

int
srbListCollect (srbConn* conn, int catType,
char *collectionName, char *flag, mdasC_sql_result_struct *listResult,
int rowsWanted)
{
    char *buf;
    int buflen;
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;


    argv[0] = catType;
    argv[1] = addrToSRBLong (collectionName);
    argv[2] = addrToSRBLong (flag);
    argv[3] = rowsWanted;

    if ((buflen = getSqlResultLen (1, rowsWanted)) <= 0)
        return CLI_ERR_MALLOC;

    buf = malloc (buflen);
    if (buf == NULL)
        return CLI_ERR_MALLOC;

    clOut.retval = buf;

    retval = clCall (conn, F_LIST_COLLECTION, argv, &clOut);

    if (retval < 0) {
	free (buf);
        return retval;
    }
    strToSqlResult (buf, listResult);

    free (buf);

    if (listResult->result_count > 0)
        return 0;       /* success */
    else
        return CLI_ERR_RETURN_LEN;
}

/*
 * srbModifyCollect - Modify a SRB collection
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *         char *collectionName - The name of the collection to modify.
 *	   char *dataValue1 - Input value 1.
 *	   char *dataValue2 - Input value 2.
 *	   char *dataValue3 - Input value 3.
 *	   int retractionType - The type of retraction. See srbC_mdas_externs.h
 *				for the retractionType definition.
 *
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

int
srbModifyCollect (srbConn* conn, int catType,
char *collectionName, char *dataValue1, char *dataValue2, 
char *dataValue3, int retractionType)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = catType;
    argv[1] = addrToSRBLong (collectionName);
    argv[2] = addrToSRBLong (dataValue1);
    argv[3] = addrToSRBLong (dataValue2);
    argv[4] = addrToSRBLong (dataValue3);
    argv[5] = retractionType;

    retval = clCall (conn, F_MODIFY_COLLECTION, argv, &clOut);
    return retval;
}

/*
 * srbChkMdasAuth - Authenticate a userName/passwd.
 *
 * Input - srbConn* conn - From srbConnect ().
 *	   char *userName   and
 *	   char *srbAuth - The userName/passwd pair to authenticate.
 *         char *mdasDomain - The MDAS Domain.
 *
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

int
srbChkMdasAuth (srbConn* conn, char *userName, char *srbAuth, char *mdasDomain)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = addrToSRBLong (userName);
    argv[1] = addrToSRBLong (srbAuth);
    argv[2] = addrToSRBLong (mdasDomain);

    retval = clCall (conn, F_CHK_MDAS_AUTH, argv, &clOut);
    return retval;
}

/*
 * srbChkEncryptAuth - Authenticate a userName/passwd.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *userName   and
 *         char *mdasDomain - The MDAS Domain.
 *	   int iLen1 - The length of the Challenge/Scrambled buffer
 *	   int sessionFlag - create a session ? (for fed MCAT only)
 *	       0 - don't create a session, 1 - create a session. 
 *	   char *cpChallengeMessageBuf - The challenge msg buf
 *	   char *cpScrambledMsgFromClient - The Scrambled response
 *
 *
 * Output - Returns 0 (success for sessionFlag = 0), or a positive session
 *	    number (sessionFlag = 1).
 *          Returns negative - failure.
 */

int
srbChkEncryptAuth (srbConn* conn, char *userName, char *mdasDomain,
                   int iLen1, int sessionFlag,
                   char *cpChallengeMessageBuf,
                   char *cpScrambledMsgFromClient)
{
  int retval;
  clArgBlock argv[MAXFMGRARGS];
  ClOut clOut;
  int clOutVal;
  int resultLen;
  INT32 i32Len1, i32Len2;
  
  srbResult *res;
  
  clOut.retval = (char *) &clOutVal;
  
  argv[0].isint = 0;
  argv[0].len = strlen(userName)+1;
  argv[0].u.ptr = (char*) userName;
  
  argv[1].isint = 0;
  argv[1].len = strlen(mdasDomain)+1;
  argv[1].u.ptr =  (char*) mdasDomain;
  
  i32Len1 = iLen1;
  argv[2].isint = 1;
  argv[2].len =  SIZEOF32; /* because of T90 */
  argv[2].u.integer = i32Len1;
  
  i32Len2 = sessionFlag;
  argv[3].isint = 1;
  argv[3].len =  SIZEOF32; /* because of T90 */
  argv[3].u.integer = i32Len2;
  
  argv[4].isint = 0;
  argv[4].len = strlen(cpChallengeMessageBuf)+1;
  argv[4].u.ptr = (char *) cpChallengeMessageBuf;
  
  argv[5].isint = 0;
  argv[5].len = strlen(cpScrambledMsgFromClient)+1;
  argv[5].u.ptr = (char *) cpScrambledMsgFromClient;
  
  res = clFunct(conn, F_CHK_ENCRYPT1_AUTH, (char *) &retval, &resultLen, 1, argv, 6);
  if (clResultStatus(res) == CLI_COMMAND_OK) {
    clClear(res);
    return retval;
  } else {
    retval = clResultStatus(res);
    clClear(res);
    return retval;
  }
}

/*
 * srbObjLock - Lock a SRB object. It is a priviledged call for
 * now.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *objID - The SRB object ID to lock. The objID is obtained
 *              through registration with MDAS.
 *         char *collectionName - The collection name.
 *         lockCmd - F_SETLKW (see fnctl).
 *         lockType - F_RDLCK, F_WRLCK or F_UNLCK.
 *         lockFd - The fd of the lock. Valid only for F_UNLCK.
 *
 * Output : Returns the Object descriptor - success, negative  - failure
 */
#ifdef LOCK_OBJ
int
srbObjLock(srbConn* conn, char *objID, char *collectionName,
int lockCmd, int lockType, int lockFd)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = addrToSRBLong (objID);
    argv[1] = addrToSRBLong (collectionName);
    argv[2] = localToSrbLockCmd (lockCmd);
    argv[3] = localToSrbLockCmd (lockType);
    argv[4] = lockFd;

    retval = clCall (conn, F_SRBO_LOCK, argv, &clOut);
    return retval;
}
#endif

/*
 * srbChkMdasSysAuth - Authenticate a userName/passwd for sys admin access.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *userName   and
 *         char *srbAuth - The userName/passwd pair to authenticate.
 *         char *mdasDomain - The MDAS Domain.
 *
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

int
srbChkMdasSysAuth (srbConn* conn, char *userName, char *srbAuth, 
char *mdasDomain)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = addrToSRBLong (userName);
    argv[1] = addrToSRBLong (srbAuth);
    argv[2] = addrToSRBLong (mdasDomain);

    retval = clCall (conn, F_CHK_MDAS_SYS_AUTH, argv, &clOut);
    return retval;
}

/*
 * srbRegisterUserGrp - Register a user group
 *
 * Input - srbConn* conn - From srbConnect ().
 *	   int catType - catalog type. e,g., MDAS_CATALOG.
 *	   char *userGrpName - The name of the user group to register.
 *	   char *userGrpPasswd - The user group passwd.
 *         char *userGrpType - The user group type. Currently, at SDSC valid
 *              userType are:
 *              "staff", "sdsc staff", "sdsc staff scientist", 
 *              "sdsc senior staff scientist", "pto staff", "ucsd staff"
 *              "student", "sdsc student", "uva student", "project",
 *              "umd student", "public", "sysadmin", " deleted"
 *	   char* userGrpAddress - The mailing address of the user group.
 *	   char* userGrpPhone - The phone number of the user group.
 *	   char* userGrpEmail - The Email address of the user group.
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

int
srbRegisterUserGrp (srbConn* conn, int catType, char *userGrpName,
                        char *userGrpPasswd,
                        char *userGrpType,
                        char* userGrpAddress, char* userGrpPhone,
                        char* userGrpEmail)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = catType;
    argv[1] = addrToSRBLong (userGrpName);
    argv[2] = addrToSRBLong (userGrpPasswd);
    argv[3] = addrToSRBLong (userGrpType);
    argv[4] = addrToSRBLong (userGrpAddress);
    argv[5] = addrToSRBLong (userGrpPhone);
    argv[6] = addrToSRBLong (userGrpEmail);

    retval = clCall (conn, F_REGISTER_USER_GROUP, argv, &clOut);
    return retval;
}

/*
 * srbRegisterUser - Register a user. 
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *         char *userName - The name of the user to register.
 *         char *userDomain - The domain of the user to register.
 *         char *userPasswd - The user passwd.
 *	   char *userType - The user type. Currently, at SDSC valid userType 
 *	        are:
 *		"staff", "sdsc staff", "sdsc staff scientist", 
 *		"sdsc senior staff scientist", "pto staff", "ucsd staff"
 *		"student", "sdsc student", "uva student", "project",
 *		"umd student", "public", "sysadmin", " deleted"
 *         char* userAddress - The mailing address of the user.
 *         char* userPhone - The phone number of the user.
 *         char* userEmail - The Email address of the user.
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

int
srbRegisterUser (srbConn* conn, int catType, char *userName, char *userDomain,
                        char *userPasswd,
                        char *userType,
                        char* userAddress, char* userPhone,
                        char* userEmail)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = catType;
    argv[1] = addrToSRBLong (userName);
    argv[2] = addrToSRBLong (userDomain);
    argv[3] = addrToSRBLong (userPasswd);
    argv[4] = addrToSRBLong (userType);
    argv[5] = addrToSRBLong (userAddress);
    argv[6] = addrToSRBLong (userPhone);
    argv[7] = addrToSRBLong (userEmail);

    retval = clCall (conn, F_REGISTER_USER, argv, &clOut);
    return retval;
}

/*
 * srbModifyUser - Modify a user info. 
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *
 *	   char *dataValue1, char *dataValue2 and int retractionType - 
 *	   They are used to specify the user attributes to modify. A normal
 *	   user may use it to modify his/her own passwd and a limited
 *	   set of attributes. A user with MDAS sys admin privilege can
 *	   also use these input values to modify other user's attributes. 
 *
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

int
srbModifyUser (srbConn* conn, int catType, char *dataValue1,
char *dataValue2, int retractionType)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = catType;
    argv[1] = addrToSRBLong (dataValue1);
    argv[2] = addrToSRBLong (dataValue2);
    argv[3] = retractionType;

    retval = clCall (conn, F_MODIFY_USER, argv, &clOut);
    return retval;
}

/*
 * srbSetAuditTrail - Setting and Unsetting Audit Trail.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int set_value - The Audit Trail value to set.
 *		AUDIT_TRAIL_OFF - turn on audit trail.
 *		AUDIT_TRAIL_ON - turn on audit trail.
 *		GET_AUDIT_TRAIL_SETTING - return the cuurent audit trail
 *		  setting without modifying the setting.
 *
 * Output - Returns the currently audit trail setting (after processing
 *	    the latest change request).
 */

int
srbSetAuditTrail (srbConn* conn, int set_value)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = set_value;

    retval = clCall (conn, F_SET_AUDIT_TRAIL, argv, &clOut);
    return retval;
}

/*
 * srbObjAudit - Make an audit entry for an object
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *	   char *userName - The userName.
 *	   char *objID - The objID of the object to audit.
 *         char *collectionName - The name of the collection this objID
 *              belongs.
 *	   char *dataPath - The path name of the object.
 *	   char *resourceName - the resource name of the object.
 *	   char *accessMode - The access mode ("read", "write", "all");
 *	   char *comment - comments to be included.
 *	   int success - Indication whether the operation was successful.
 *	     0 = failure, 1 - success.
 *	   char *domainName - The domain name of the user.
 *
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

int
srbObjAudit (srbConn* conn, int catType,
char *userName, char *objID,
char *collectionName,  char *dataPath,  char *resourceName,
char *accessMode,  char *comment, int success,  char *domainName)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = catType;
    argv[1] = addrToSRBLong (userName);
    argv[2] = addrToSRBLong (objID);
    argv[3] = addrToSRBLong (collectionName);
    argv[4] = addrToSRBLong (dataPath);
    argv[5] = addrToSRBLong (resourceName);
    argv[6] = addrToSRBLong (accessMode);
    argv[7] = addrToSRBLong (comment);
    argv[8] = success;
    argv[9] = addrToSRBLong (domainName);

    retval = clCall (conn, F_SRBO_AUDIT, argv, &clOut);
    return retval;
}

/*
 * srbVaultInfo()
 * Get Info on the SRB storage vault. 
 *
 * Input : srbConn* conn
 *	
 * Output: A link list of struct vaultQueElement with the queue head pointed 
 * to by vaultQueHead. 
 *
 * Return value :
 *	struct vaultQueElement *vaultQueHead - pointer to the
 *	head of the link list
 *	
 */
struct vaultQueElement *
srbVaultInfo(srbConn* conn)
{
    int resultLen;
    srbResult *res;
    char buf[200 * MAX_TOKEN];
    INT32 resCount, i;
    clArgBlock *argv;
    int resLen;
    char *cptr;
#if defined(PORTNAME_c90)
    struct int32Array *iptr;
#else
    INT32 *iptr;
#endif
    int vaultCount;
    struct vaultQueElement *tmpElement, *oldElement;
    struct vaultQueElement *vaultQueHead = NULL;
    int status;
 
    argv = NULL;
    res = clFunct(conn, F_E_VAULT_INFO, buf,&resultLen,0,argv,0);
    if ((status = clResultStatus(res)) != CLI_COMMAND_OK) {
	errno = status;
        return NULL;
    }

    clClear(res);

    vaultQueHead = NULL;

    /* Now parse the result. The format is: int vaultCount;
     * int storSysType[vaultCount]; char *vaultPath[vaultCount]
     */

#if defined(PORTNAME_c90)
    /* Get the first 32 bit */
    vaultCount = sToL (ntohl (VARSIZE (buf)));
#else
    vaultCount = ntohl (*(INT32 *) buf);
#endif

    /* Location of vaultPath */

    cptr = buf + (vaultCount + 1) * SIZEOF32;

    /* Location of storSysType */

#if defined(PORTNAME_c90)
    /* c90 is 64 bit word addressible. Don't have to increase
     * word address to access the last 32 bit. What a pain !!
     */
    iptr = (struct int32Array *) buf;
#else
    iptr = (INT32 *) (buf + sizeof (INT32));
#endif

    for (i = 0; i < vaultCount; i++){
 	tmpElement = (struct vaultQueElement *)
	    malloc (sizeof (struct vaultQueElement));
#if defined(PORTNAME_c90)
	/* i + 1 because the array starts the second 32 bit */
	tmpElement->storSysType = sToL (ntohl (ARRYAY32 (iptr, (i + 1))));
#else
	tmpElement->storSysType = ntohl (iptr[i]);
#endif
	tmpElement->vaultPath = (char *) malloc (strlen (cptr) + 1);
	strcpy (tmpElement->vaultPath, cptr);
	cptr += (strlen (cptr) + 1);
	if (i == 0) {
	    vaultQueHead = tmpElement;
	} else {
	    oldElement->next = tmpElement;
	}
	oldElement = tmpElement;
    }
    tmpElement->next = NULL;	/* Bottom of the link list */
    return vaultQueHead;
}

/*
 * srbGetSvrVersion - Get the Server version
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char **outbuf - Pointer to point of output buffer.
 *
 * Output - Returns the status.
 *          Returns a negative value upon failure.
 */

int
srbGetSvrVersion(srbConn *conn, char *outbuf)
{
    int retval, i;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;
    char *buf;

    buf = malloc (STRING_LEN1);
    *buf='\0';

    clOut.retval = (char *) buf;

    retval = clCall (conn, F_GET_SVR_VER, argv, &clOut);

    buf[STRING_LEN1-1]='\0'; /* make sure is null terminated */
    /* isalpha(), isascii(), isprint() */
    if((strlen(buf)+2) > MAX_TOKEN) {
      /* GJK path 2001.02.06 kill non ASCII */
      for(i=0; i < (MAX_TOKEN-1); i++) {
	if(isascii(isprint(buf[i]) != 0) && (isprint(buf[i]) != 0)) {
	  outbuf[i]=buf[i];
	  outbuf[i+1]='\0';
	}
      }
      outbuf[MAX_TOKEN-1]='\0';
    }
    else
      strcpy(outbuf, buf);
    free(buf);

    if (retval > 0)
	retval = 0;
    return retval;
}

/*
 * srbFreeVaultInfo () - Free memory taken by the vaultQueElement link list.
 *
 * Input - vaultQueElement *vaultQueHead
 *
 * Output - None.
 */

void
srbFreeVaultInfo(struct vaultQueElement *vaultQueHead)
{
    struct vaultQueElement *tmpElement, *oldElement;
 
    tmpElement = vaultQueHead;
    while (tmpElement != NULL) {
	free (tmpElement->vaultPath);
	oldElement = tmpElement;
	tmpElement = tmpElement->next;
	free (oldElement);
    }
}

/*
 * srbPrintVaultInfo () - Print out the VaultInfo link list.
 *
 * Input - vaultQueElement *vaultQueHead
 *
 * Output - Printed output.
 */
 
void
srbPrintVaultInfo(struct vaultQueElement *vaultQueHead)
{
    struct vaultQueElement *tmpElement;
 
    tmpElement = vaultQueHead;
    while (tmpElement != NULL) {
	printf ("storSysType = %d, vaultPath = %s \n", 
	    tmpElement->storSysType,
	tmpElement->vaultPath);
        tmpElement = tmpElement->next;
    }
}

/*
 * srbOpendir - Opens a directory 
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int storSysType - Storage system type.
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP (not supported for
 *              HTTP).
 *         char *hostAddr - The Host address of the SRB.
 *         char *dirname - The dir Path name to open. Must be an
 *          absolute path.
 * Output - Returns the file descriptor for use in later srb* calls.
 *          Returns a negative value upon failure.
 */
 
int
srbOpendir(srbConn* conn, int storSysType, char *hostAddr, char *dirname)
{
    return (_srbOpendir (conn, storSysType, hostAddr, dirname,
      DO_CHKDIR));
}

int
_srbOpendir(srbConn* conn, int storSysType, char *hostAddr, 
char *dirname, int chdirFlag)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;
 
    argv[0] = storSysType;
    argv[1] = addrToSRBLong (hostAddr);
    argv[2] = addrToSRBLong (dirname);
    argv[3] = chdirFlag;
 
    retval = clCall (conn, F_E_OPENDIR, argv, &clOut);
    return retval;
}

/*
 * srbClosedir - close an opened directory
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int fd - The file descriptor to close (from srbOpendir) 
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */
 
int
srbClosedir(srbConn* conn, int fd)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;
 
    argv[0] = fd;
 
    retval = clCall (conn, F_E_CLOSEDIR, argv, &clOut);
    return retval;
}

/*
 * srbReaddir - Stat a directory entry 
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int fd - The file descriptor to read (from srbOpendir) 
 *
 * Output - struct srbDirent *srbDirent - The dirent read.
 */

struct srbDirent *
srbReaddir(srbConn* conn, int dirDesc)
{
    struct srbDirent *srbDirent;
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;
 
    argv[0] = dirDesc;
 
    srbDirent = (struct srbDirent *) malloc (sizeof (struct srbDirent));
    
    clOut.retval = (char *) srbDirent;

    retval = clCall (conn, F_E_READDIR, argv, &clOut);

    if (retval >= 0) {
        if (retval < sizeof (struct srbDirent)) {
	    free (srbDirent);
	    errno = CLI_ERR_RETURN_LEN;
            return NULL;
        } else {
	    /* Convert the elemens of the srbDirent struct */
	    srbDirent->d_offset = ntohl (srbDirent->d_offset);
	    srbDirent->d_ino = ntohl (srbDirent->d_ino);
	    srbDirent->d_reclen = ntohs (srbDirent->d_reclen);
	    srbDirent->d_namlen = ntohs (srbDirent->d_namlen);
            return srbDirent;
	}
    } else {
	errno = retval;
        return NULL;
    }
}

#ifdef HPSS_PRIV        /* required HPSS root user priv */
/*
 * srbFileMigrate - Migrate an extended file
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int fd - The file descriptor to migrate (from srbFileOpen or
 *         srbFileCreate).
 *	   int srcLevel - The storage hierarchy from which the data is to be
 *	   migrated.
 *	   int flags - BFS_MIGRATE_ALL - migrate the entire file.
 *	   
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */

int
srbFileMigrate(srbConn* conn, int fd, int srcLevel, int flags)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = fd;
    argv[1] = srcLevel;
    argv[2] = flags;

    retval = clCall (conn, F_E_MIGRATE, argv, &clOut);
    return retval;
}
#endif          /* HPSS_PRIV */

/*
 * srbFileStage - Stage an extended file
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int fd - The file descriptor to stage (from srbFileOpen or
 *         srbFileCreate).
 *	   int offset - The offset of the startof the data to stage.
 *	   int len - The length to stage.
 *         int storLevel - The storage hierarchy to which the data is to be
 *         staged.
 *         int flags - BFS_STAGE_ALL - stage the entire file.
		       BFS_ASYNC_CALL - return after initial stage.
 *
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */

int
srbFileStage(srbConn* conn, int fd, int offset, int len, 
 int storLevel, int flags)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = fd;
    argv[1] = offset;
    argv[2] = len;
    argv[3] = storLevel;
    argv[4] = flags;

    retval = clCall (conn, F_E_STAGE, argv, &clOut);
    return retval;
}

#ifdef HPSS_PRIV        /* required HPSS root user priv */
/*
 * srbFilePurge - Purge an extended file
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int fd - The file descriptor to purge.g (from srbFileOpen or
 *         srbFileCreate).
 *         int offset - The offset of the startof the data to purge.g.
 *         int len - The length to purge.g.
 *         int storLevel - The storage hierarchy from which the data is to be
 *         purged.
 *         int flags - BFS_PURGE_ALL - purge.g the entire file.
 *
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */

int
srbFilePurge(srbConn* conn, int fd, int offset, int len,
 int storLevel, int flags)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = fd;
    argv[1] = offset;
    argv[2] = len;
    argv[3] = storLevel;
    argv[4] = flags;

    retval = clCall (conn, F_E_PURGE, argv, &clOut);
    return retval;
}
#endif          /* HPSS_PRIV */

/*
 * srbDbLobjOpen - Opens a DB Large Object. 
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int storSysType - Storage system type.
 *              0 = DB2, 1 = Illustra.
 *         char *resourceLoc - format : host:database:instance.
 *         char *dataPath - Format : tablename/objectID. If only the objectID
 *           is given (without the "/"), the tablename will default to
 *           "srbVault".
 *         int flags - same definition as in unix.
 *         int mode - Not curently used. 
 * Output - Returns the file descriptor for use in later srbDbLobj* calls.
 *          Returns a negative value upon failure.
 */
 
int
srbDbLobjOpen(srbConn* conn, int storSysType, char *resourceLoc, char *dataPath,
int flags, int mode)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;
 
    argv[0] = storSysType;
    argv[1] = addrToSRBLong (resourceLoc);
    argv[2] = addrToSRBLong (dataPath);
    argv[3] = flags;
    argv[4] = mode;
 
    retval = clCall (conn, F_DB_LOBJ_OPEN, argv, &clOut);
    return retval;
}

/*
 * srbDbLobjCreate Create a DB Large Object.
 * Input - srbConn* conn - From srbConnect ().
 *         int storSysType - Storage system type.
 *              0 = DB2, 1 = Illustra.
 *         char *resourceLoc - format : host:database:instance.
 *         char *dataPath - Format : tablename/objectID. If only the objectID
 *           is given (without the "/"), the tablename will default to
 *           "srbVault".
 *         int mode - Not curently used.
 * Output - Returns the file descriptor for use in later srbDbLobj* calls.
 *          Returns a negative value upon failure.
 */
 
int
srbDbLobjCreate(srbConn* conn, int storSysType, char *hostAddr, char *filename,
int mode)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;
 
    argv[0] = storSysType;
    argv[1] = addrToSRBLong (hostAddr);
    argv[2] = addrToSRBLong (filename);
    argv[3] = mode;
 
    retval = clCall (conn, F_DB_LOBJ_CREATE, argv, &clOut);
    return retval;
}

/*
 * srbDbLobjClose - close a DB Large Object.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int fd - The dbLobj descriptor to close (from srbDbLobjClose or
 *         srbDbLobjCreate).
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */
 
int
srbDbLobjClose(srbConn* conn, int fd)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;
 
    argv[0] = fd;
 
    retval = clCall (conn, F_DB_LOBJ_CLOSE, argv, &clOut);
    return retval;
}

/*
 * srbDbLobjRead - Read len bytes of the DB large object into buf
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int fd - The object descriptor (from the srbDbLobjOpen call) to read.
 *         char *buf - The input buffer.
 *         int len - The number of bytes to read.
 *
 * Output - Returns the length of bytes read.
 *          Returns a negative value upon failure.
 * the CALLER must have allocated enough space to hold the result returned
 */
 
int
srbDbLobjRead(srbConn *conn, int fd, char *buf, int len)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) buf;
 
    argv[0] = fd;
    argv[1] = len;
 
    retval = clCall (conn, F_DB_LOBJ_READ, argv, &clOut);
    return retval;
}

/*
 * srbDbLobjWrite - Write len bytes of buf into the dbLobj fd
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int fd - The dbLobj descriptor to write (from srbDbLobjOpen or
 *         srbDbLobjCreate).
 *         char *buf - The output buffer.
 *         int len - The length to write.
 *
 * Output - Returns the number of bytes written..
 *          Returns a negative value upon failure.
 *
 * Note : We cannot use the generic routine clCall() because the input
 * byte stream is not a char string.
 */
int
srbDbLobjWrite(srbConn *conn, int fd, char *buf, int len)
{
    clArgBlock argv[2];
    srbResult *res;
    int resultLen;
    int retval;
 
    if (len <= 0)
        return 0;
 
    argv[0].isint = 1;
    argv[0].len = 4;
    argv[0].u.integer = fd;
 
    argv[1].isint = 0;
    argv[1].len = len;
    argv[1].u.ptr = (char*)buf;
 
    res = clFunct(conn, F_DB_LOBJ_WRITE,(char *) &retval,&resultLen,1,argv,2);
    if (clResultStatus(res) == CLI_COMMAND_OK) {
        clClear(res);
        return retval;
    } else {
        retval = clResultStatus(res);
        clClear(res);
        return retval;
    }
}

/*
 * srbDbLobjSeek - Change the current read or write location of an dbLobj 
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int fd - The dbLobj descriptor (from the srbDbLobjOpen call) to seek.
 *         int offset - The position of the next operation
 *         int whence - Same definition as in Unix.
 *              SEEK_SET - pointer is set to the value of the Offset parameter.
 *              SEEK_CUR - pointer is set to its current location plus the
 *                      value of the Offset parameter.
 *              SEEK_END - pointer is set to the size of the file plus the
 *                      value of the Offset parameter.
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */
 
int
srbDbLobjSeek(srbConn *conn, int fd, int offset, int whence)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;
 
    argv[0] = fd;
    argv[1] = offset;
    argv[2] = whence;
 
    retval = clCall (conn, F_DB_LOBJ_SEEK, argv, &clOut);
    return retval;
}

/*
 * srbDbLobjUnlink - Unlinks a DB large object.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int storSysType - Storage system type.
 *              0 = DB2, 1 = Illustra.
 *         char *resourceLoc - format : host:database:instance.
 *         char *dataPath - Format : tablename/objectID
 *          char *userName - the user name.
 * Output - Returns 0 - success.
 *          Returns a negative value upon failure.
 */

int
srbDbLobjUnlink(srbConn* conn, int storSysType, char *resourceLoc, 
char *dataPath)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = storSysType;
    argv[1] = addrToSRBLong (resourceLoc);
    argv[2] = addrToSRBLong (dataPath);

    retval = clCall (conn, F_DB_LOBJ_UNLINK, argv, &clOut);
    return retval;
}


/*
 * srbDbTableOpen - Opens a DB Large Object. 
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int storSysType - Storage system type.
 *              0 = DB2, 1 = Illustra.
 *         char *resourceLoc - format : host:database:instance.
 *         char *dataPath - Format : tablename/objectID. If only the objectID
 *           is given (without the "/"), the tablename will default to
 *           "srbVault".
 *         int flags - same definition as in unix.
 *         int mode - Not curently used. 
 * Output - Returns the file descriptor for use in later srbDbTable* calls.
 *          Returns a negative value upon failure.
 */
 
int
srbDbTableOpen(srbConn* conn, int storSysType, char *resourceLoc, char *dataPath,
int flags, int mode)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;
 
    argv[0] = storSysType;
    argv[1] = addrToSRBLong (resourceLoc);
    argv[2] = addrToSRBLong (dataPath);
    argv[3] = flags;
    argv[4] = mode;
 
    retval = clCall (conn, F_DB_TABLE_OPEN, argv, &clOut);
    return retval;
}

/*
 * srbDbTableCreate Create a DB Large Object.
 * Input - srbConn* conn - From srbConnect ().
 *         int storSysType - Storage system type.
 *              0 = DB2, 1 = Illustra.
 *         char *resourceLoc - format : host:database:instance.
 *         char *dataPath - Format : tablename/objectID. If only the objectID
 *           is given (without the "/"), the tablename will default to
 *           "srbVault".
 *         int mode - Not curently used.
 * Output - Returns the file descriptor for use in later srbDbTable* calls.
 *          Returns a negative value upon failure.
 */
 
int
srbDbTableCreate(srbConn* conn, int storSysType, char *hostAddr, char *filename,
int mode)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;
 
    argv[0] = storSysType;
    argv[1] = addrToSRBLong (hostAddr);
    argv[2] = addrToSRBLong (filename);
    argv[3] = mode;
 
    retval = clCall (conn, F_DB_TABLE_CREATE, argv, &clOut);
    return retval;
}

/*
 * srbDbTableClose - close a DB Large Object.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int fd - The dbTable descriptor to close (from srbDbTableClose or
 *         srbDbTableCreate).
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */
 
int
srbDbTableClose(srbConn* conn, int fd)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;
 
    argv[0] = fd;
 
    retval = clCall (conn, F_DB_TABLE_CLOSE, argv, &clOut);
    return retval;
}

/*
 * srbDbTableRead - Read len bytes of the DB large object into buf
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int fd - The object descriptor (from the srbDbTableOpen call) to read.
 *         char *buf - The input buffer.
 *         int len - The number of bytes to read.
 *
 * Output - Returns the length of bytes read.
 *          Returns a negative value upon failure.
 * the CALLER must have allocated enough space to hold the result returned
 */
 
int
srbDbTableRead(srbConn *conn, int fd, char *buf, int len)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) buf;
 
    argv[0] = fd;
    argv[1] = len;
 
    retval = clCall (conn, F_DB_TABLE_READ, argv, &clOut);
    return retval;
}

/*
 * srbDbTableWrite - Write len bytes of buf into the dbTable fd
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int fd - The dbTable descriptor to write (from srbDbTableOpen or
 *         srbDbTableCreate).
 *         char *buf - The output buffer.
 *         int len - The length to write.
 *
 * Output - Returns the number of bytes written..
 *          Returns a negative value upon failure.
 *
 * Note : We cannot use the generic routine clCall() because the input
 * byte stream is not a char string.
 */
int
srbDbTableWrite(srbConn *conn, int fd, char *buf, int len)
{
    clArgBlock argv[2];
    srbResult *res;
    int resultLen;
    int retval;
 
    if (len <= 0)
        return 0;
 
    argv[0].isint = 1;
    argv[0].len = 4;
    argv[0].u.integer = fd;
 
    argv[1].isint = 0;
    argv[1].len = len;
    argv[1].u.ptr = (char*)buf;
 
    res = clFunct(conn, F_DB_TABLE_WRITE,(char *) &retval,&resultLen,1,argv,2);
    if (clResultStatus(res) == CLI_COMMAND_OK) {
        clClear(res);
        return retval;
    } else {
        retval = clResultStatus(res);
        clClear(res);
        return retval;
    }
}

/*
 * srbDbTableSeek - Change the current read or write location of an dbTable 
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int fd - The dbTable descriptor (from the srbDbTableOpen call) to seek.
 *         int offset - The position of the next operation
 *         int whence - Same definition as in Unix.
 *              SEEK_SET - pointer is set to the value of the Offset parameter.
 *              SEEK_CUR - pointer is set to its current location plus the
 *                      value of the Offset parameter.
 *              SEEK_END - pointer is set to the size of the file plus the
 *                      value of the Offset parameter.
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */
 
int
srbDbTableSeek(srbConn *conn, int fd, int offset, int whence)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;
 
    argv[0] = fd;
    argv[1] = offset;
    argv[2] = whence;
 
    retval = clCall (conn, F_DB_TABLE_SEEK, argv, &clOut);
    return retval;
}

/*
 * srbDbTableUnlink - Unlinks a DB large object.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int storSysType - Storage system type.
 *              0 = DB2, 1 = Illustra.
 *         char *resourceLoc - format : host:database:instance.
 *         char *dataPath - Format : tablename/objectID
 *          char *userName - the user name.
 * Output - Returns 0 - success.
 *          Returns a negative value upon failure.
 */

int
srbDbTableUnlink(srbConn* conn, int storSysType, char *resourceLoc, 
char *dataPath)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = storSysType;
    argv[1] = addrToSRBLong (resourceLoc);
    argv[2] = addrToSRBLong (dataPath);

    retval = clCall (conn, F_DB_TABLE_UNLINK, argv, &clOut);
    return retval;
}

/*
 * srbHostConfig()
 * Get the storage vault information on a SRB host.
 *
 * Input : srbConn* conn
 *
 * Output: A link list of struct clHostElement with the queue head pointed
 * to by hostQueHead. 
 *
 * Return value :
 *      struct clHostElement *hostQueHead - pointer to the
 *      head of the link list
 *
 */
struct clHostElement *
srbHostConfig(srbConn* conn)
{
    int resultLen;
    srbResult *res;
    char buf[100 * sizeof(struct clHostElement)];
    INT32 resCount, i;
    clArgBlock *argv;
    int resLen;
    char *cptr;
#if defined(PORTNAME_c90)
    struct int32Array *iptr;
#else
    INT32 *iptr;
#endif
    int hostCount;
    struct clHostElement *tmpElement, *oldElement;
    struct clHostElement *hostQueHead = NULL;
 
    argv = NULL;
    res = clFunct(conn, F_GET_HOST_CONFIG, buf,&resultLen,0,argv,0);
    if (clResultStatus(res) != CLI_COMMAND_OK) {
        errno = clResultStatus(res);
        clClear(res);
        return NULL;
    }
 
    clClear(res);
 
    hostQueHead = NULL;
 
    /* Now parse the result. The format is: int hostCount;
     * int storSysType[hostCount]; char *hostPath[hostCount]
     */
 
#if defined(PORTNAME_c90)
    /* Get the first 32 bit */
    hostCount = sToL (ntohl (VARSIZE (buf)));
#else
    hostCount = ntohl (*(INT32 *) buf);
#endif
 
    /* Location of hostPath */
 
    cptr = buf + SIZEOF32;
 
    /* Location of storSysType */
 
    for (i = 0; i < hostCount; i++){
        tmpElement = (struct clHostElement *)
            malloc (sizeof (struct clHostElement));

	/* copy the hostName */
        tmpElement->hostName = (char *) malloc (strlen (cptr) + 1);
        strcpy (tmpElement->hostName, cptr);
        cptr += (strlen (cptr) + 1);
	/* Then the portNum */
	tmpElement->portNum = (char *) malloc (strlen (cptr) + 1);
        strcpy (tmpElement->portNum, cptr);
        cptr += (strlen (cptr) + 1);

	/* Finnally, mdasEnabled */
	tmpElement->mdasEnabled = atoi (cptr);
	cptr += (strlen (cptr) + 1);

        if (i == 0) {
            hostQueHead = tmpElement;
        } else {
            oldElement->next = tmpElement;
        }
        oldElement = tmpElement;
    }
    tmpElement->next = NULL;    /* Bottom of the link list */
    return hostQueHead;
}

/*
 * srbPrintHostInfo () - Print out the VaultInfo link list.
 *
 * Input - vaultQueElement *vaultQueHead
 *
 * Output - Printed output.
 */
 
void
srbPrintHostInfo(struct clHostElement *vaultQueHead)
{
    struct clHostElement *tmpElement;
 
    tmpElement = vaultQueHead;
    while (tmpElement != NULL) {
        printf ("hostName = %s, portNum = %s mdasEnabled = %d\n",
            tmpElement->hostName, tmpElement->portNum,
	    tmpElement->mdasEnabled);
        tmpElement = tmpElement->next;
    }
}

/*
 * srbFreeHostInfo () - Free memory taken by the clHostElement link list.
 *
 * Input - clHostElement *hostQueHead
 *
 * Output - None.
 */
 
void
srbFreeHostInfo(struct clHostElement *hostQueHead)
{
    struct clHostElement *tmpElement, *oldElement;
 
    tmpElement = hostQueHead;
    while (tmpElement != NULL) {
        free (tmpElement->hostName);
        free (tmpElement->portNum);
        oldElement = tmpElement;
        tmpElement = tmpElement->next;
        free (oldElement);
    }
}

/*
 * srbGetPrivUsers - Read the privileged users list and put it in
 * a user supplied mdasC_sql_result_struct. 
 *
 * Input - srbConn* conn - From srbConnect ().
 * 	   int catalog - The catalog type. e.g., MDAS_CATALOG
 *	   mdasC_sql_result_struct *srbUserList - A pointer to a user supplied 
 *	    mdasC_sql_result_struct.
 *	   int rowsWanted - The number of rows to be returned.
 *
 * Output -
 *      0 - success; myresult->continuation_index >= 0, ==> more results from
 *          the query. Use srbGetMoreRows() to retrieve more rows.
 *      Returns a negative value upon failure.
 */
 
int
srbGetPrivUsers(srbConn *conn, int catalog, 
mdasC_sql_result_struct *srbUserList, int rowsWanted)
{
    char *buf;
    int buflen;
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    argv[0] = catalog;
    argv[1] = rowsWanted;

    if ((buflen = getSqlResultLen (3, rowsWanted)) <= 0)
        return CLI_ERR_MALLOC;

    buf = malloc (buflen);
    if (buf == NULL)
        return CLI_ERR_MALLOC;

    clOut.retval = (char *) buf;

    retval = clCall (conn, F_GET_PRIV_USERS, argv, &clOut);

    if (retval < 0) {
        free (buf);
        return retval;
    }
    strToSqlResult (buf, srbUserList);

    free (buf);

    if (srbUserList->result_count > 0)
        return 0;       /* success */
    else
        return CLI_ERR_RETURN_LEN;
}

/*
 * srbGetMoreRows - Get more rows of result from a srbGetDatasetInfo,
 * srbGetDataDirInfo, srbListCollect or srbGetPrivUsers calls and put
 * the results in a user supplied mdasC_sql_result_struct. 
 *
 * Input - srbConn* conn - From srbConnect ().
 * 	   int catalog - The catalog type. e.g., MDAS_CATALOG
 *	   mdasC_sql_result_struct *srbUserList - A pointer to a user supplied 
 *	   mdasC_sql_result_struct.
 *	   int contDesc - The continuation descriptor. This is a non negative
 *	   integer returned from a srbGetDatasetInfo, srbGetDataDirInfo, 
 *	   srbListCollect or srbGetPrivUsers call as 
 *	   myresult->continuation_index.
 *         int rowsWanted - The number of rows to be returned.
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */
 
int
srbGetMoreRows(srbConn *conn, int catalog, 
int contDesc, mdasC_sql_result_struct *myresult, int rowsWanted)
{
#ifdef CONV_TO_GEN_QUERY
    int status;

    status = srbGenGetMoreRows (conn, catalog, contDesc, myresult, rowsWanted);

    return (status);

#else   /* CONV_TO_GEN_QUERY */
    char *buf;
    int buflen;
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;
 
    argv[0] = catalog;
    argv[1] = contDesc;
    argv[2] = rowsWanted;

    if ((buflen = getSqlResultLen (MAX_DCS_NUM, rowsWanted)) <= 0)
        return CLI_ERR_MALLOC;

    buf = malloc (buflen);
    if (buf == NULL)
        return CLI_ERR_MALLOC;

 clOut.retval = (char *) buf;

    retval = clCall (conn, F_GET_MORE_ROWS, argv, &clOut);

    if (retval < 0 || rowsWanted == 0 ) {
        free (buf);
        return retval;
    }

    strToSqlResult (buf, myresult);

    free (buf);

    if (myresult->result_count > 0)
        return 0;       /* success */
    else
        return CLI_ERR_RETURN_LEN;
#endif  /* CONV_TO_GEN_QUERY */
}

/*
 * srbGenGetMoreRows -This is a more compact form of srbGetMoreRows.
 * The result is packed with packMsg.
 * Get more rows of result from a srbGetDatasetInfo,
 * srbGetDataDirInfo, srbListCollect or srbGetPrivUsers calls and put
 * the results in a user supplied mdasC_sql_result_struct. 
 *
 * Input - srbConn* conn - From srbConnect ().
 * 	   int catalog - The catalog type. e.g., MDAS_CATALOG
 *	   mdasC_sql_result_struct *srbUserList - A pointer to a user supplied 
 *	   mdasC_sql_result_struct.
 *	   int contDesc - The continuation descriptor. This is a non negative
 *	   integer returned from a srbGetDatasetInfo, srbGetDataDirInfo, 
 *	   srbListCollect or srbGetPrivUsers call as 
 *	   myresult->continuation_index.
 *         int rowsWanted - The number of rows to be returned.
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */
 
int
srbGenGetMoreRows(srbConn *conn, int catalog, 
int contDesc, mdasC_sql_result_struct *myresult, int rowsWanted)
{
    char *buf;
    int buflen;
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;
    int status;
    mdasC_sql_result_struct *tmpResult;
 
    argv[0] = catalog;
    argv[1] = contDesc;
    argv[2] = rowsWanted;

    if ((buflen = getSqlResultLen (MAX_DCS_NUM, rowsWanted)) <= 0)
        return CLI_ERR_MALLOC;

    buf = malloc (buflen);
    if (buf == NULL)
        return CLI_ERR_MALLOC;

 clOut.retval = (char *) buf;

    retval = clCall (conn, F_GEN_GET_MORE_ROWS, argv, &clOut);

    if (retval < 0 || rowsWanted == 0 ) {
        free (buf);
        return retval;
    }

#ifdef foo
    strToSqlResult (buf, myresult);
#else
    status = unpackMsg (buf, (char **) &tmpResult,
     sqlResultStruct_PF, NULL);

    if (status < 0) {
        free (buf);
        return (status);
    }
    *myresult = *tmpResult;
    free (tmpResult);
#endif

    free (buf);

    if (myresult->result_count > 0)
        return 0;       /* success */
    else
        return CLI_ERR_RETURN_LEN;
}

/*
 * srbIssueTicket - Issue a ticket.
 *
 * Input - srbConn* conn - From srbConnect ().
 * 	   char *objID - The object ID
 *         char *collectionName - The collection name
 *         char *collectionFlag - The collect flag if vCollectionName
 *              is non NULL. "R" - the ticket is for all dataset and
 *              sub-collection recursively. "D" - the ticket is for the
 *              datasets directly beneath the colloection.
 *         char *beginTime - The beginning time when the ticket
 *              becomes effective. A NULL means no time limit.
 *         char *endTime - The ending time of the ticket.
 *         int accessCnt - The number of time the ticket can be used to
 *              access the dataset.
 *         char *ticketUser - The user/userGroup that will use the
 *              ticket. Multiply users can be specified with the following 
 *		format:
 *                      user1@domain1&user2@domain2 ....
 *		If it is NULL, => all users.
 *
 *	   char **ticket - The address to put the output ticket.
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

int
srbIssueTicket (srbConn* conn, char *objID, char *collectionName, 
char *collectionFlag, char *beginTime, char *endTime, int accessCnt,
char *ticketUser, char **ticket)
{
    char buf[MAXEXFPATH];
    int buflen;
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    argv[0] = addrToSRBLong (objID);
    argv[1] = addrToSRBLong (collectionName);
    argv[2] = addrToSRBLong (collectionFlag);
    argv[3] = addrToSRBLong (beginTime);
    argv[4] = addrToSRBLong (endTime);
    argv[5] = accessCnt;
    argv[6] = addrToSRBLong (ticketUser);

    clOut.retval = (char *) buf;

    retval = clCall (conn, F_ISSUE_TICKET, argv, &clOut);

    if (retval < 0) {
        return retval;
    }

    *ticket = malloc (strlen (buf) + 1);
    strcpy (*ticket, buf);

    return (0);
}

/* srbRemoveTicket - Cancel a ticket.
 *
 *
 * Input : char *ticket - The ticket to remove.
 *
 * Output : Returns 0 - success, negative  - failure
 */

int
srbRemoveTicket (srbConn* conn, char *ticket)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = addrToSRBLong (ticket);

    retval = clCall (conn, F_REMOVE_TICKET, argv, &clOut);
    return retval;
}

/* srbRemoveTicketWithZone - Cancel a ticket.
 *
 *
 * Input : char *ticket - The ticket to remove.
 *
 * Output : Returns 0 - success, negative  - failure
 */

int
srbRemoveTicketWithZone (srbConn* conn, char *mcatZone, char *ticket)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = addrToSRBLong (mcatZone);
    argv[1] = addrToSRBLong (ticket);

    retval = clCall (conn, F_REMOVE_TICKET_WITH_ZONE, argv, &clOut);
    return retval;
}


/* getSqlResultLen - Return the length in bytes the query result
 *
 * Input - int colCnt - The number of columns expected.
 *	   int rowCnt - The number of rows expected.
 *
 * Output - The length
 */

int
getSqlResultLen (int colCnt, int rowCnt)
{
    int resultLen;

    /* XXXXX this is a hack to prevent the length getting too long */
    if ((colCnt * rowCnt) > 4000) {
	resultLen = MAX_SQL_RESULT_LEN;
    } else {
        resultLen = sizeof (mdasC_sql_result_struct) + 
          ((colCnt * rowCnt) + 2) * MAX_DATA_SIZE; /* 2 more for tab_name and
						    * att_name */
    }

    return (resultLen);
}

/* freeSqlResult - Free a mdasC_sql_result_struct and any memory associated
 * with it. This will free the content of myresult as well as the
 * myresult struct itself.
 *
 * Input : mdasC_sql_result_struct *myresult - The mdasC_sql_result_struct
 * to be freed.
 *
 * Output - None
 */

void
freeSqlResult (mdasC_sql_result_struct *myresult)
{
    int i;

    if (myresult == NULL) 
        return;

    /* KESTOCK - mem leak fix */
    /** if (myresult->row_count <= 0 || myresult->result_count <= 0)
     **   return;
     **/

    clearSqlResult (myresult);
    free (myresult);
}

/* clearSqlResult - Clear the content of myresult. Myresult is not freed
 * itself.
 *
 * Input : mdasC_sql_result_struct *myresult - The mdasC_sql_result_struct
 * to be cleared.
 *
 * Output - None
 */

void
clearSqlResult (mdasC_sql_result_struct *myresult)
{
    int i;

    for (i = 0; i < myresult->result_count ; i++){
        if (myresult->sqlresult[i].tab_name != NULL) 
            free (myresult->sqlresult[i].tab_name);
        if (myresult->sqlresult[i].att_name != NULL) 
            free (myresult->sqlresult[i].att_name);
        if (myresult->sqlresult[i].values != NULL && myresult->row_count > 0)
            free (myresult->sqlresult[i].values);
        myresult->sqlresult[i].values = myresult->sqlresult[i].tab_name =
         myresult->sqlresult[i].att_name = NULL;
    }
    myresult->result_count = 0;
    myresult->row_count = 0;
}

/* printSqlResult - Print the content of myresult.
 *
 * Input : mdasC_sql_result_struct *myresult - The mdasC_sql_result_struct
 * to be printed.
 *
 * Output - None
 */

void
printSqlResult (mdasC_sql_result_struct *myresult)
{
    int i, j;
    char *ptr;

    if (myresult == NULL)
	return;
    if (myresult->row_count == 0)
	return;

    for (i = 0; i < myresult->result_count ; i++){
        printf ("tab_name=%s, ",myresult->sqlresult[i].tab_name);
        printf ("att_name=%s\n",myresult->sqlresult[i].att_name);
	ptr = myresult->sqlresult[i].values;
	for (j = 0; j < myresult->row_count; j++) {
            printf ("%4i    values=%s\n",j, ptr);
	    ptr += MAX_DATA_SIZE;
	}
    }
}

/* clFunctionLookup - Using the input clFunction ID, lookup a clFunction in the
 * clFunctBuiltin[] table.
 *
 * Input : int funcid - The clFunction ID to lookup
 *
 * Output : A pointer to the FunctCall struct with matching funcid.
 */

FunctCall *clFunctionLookup(int funcid)
{
    int i;
    for (i=0;i<nBuiltinFunct;i++) {
        if (clFunctBuiltin[i].funcid == funcid)
                return(&clFunctBuiltin[i]);
    }
    return(NULL);
}

/* clCall - Set up the stub argument and may the function call.
 * For integer type return, an integer status is returned.
 * For byte stream type return, a byte count is returned it the call
 * is successful. Otherwise, a negative error code is returned.
 *
 * NOTE : This routine only works when all byte stream type input are
 * char string (terminated by NULL).
 */

int
clCall (srbConn *conn, int funcid, srb_long_t *inpArgv, ClOut *clOut)
{ 
    clArgBlock argv[MAXFMGRARGS];
    int i;
    FunctCall *functPtr;
    srbResult *res;
    int retType;
    int tmpInt;

    functPtr = clFunctionLookup (funcid);

    for (i = 0; i < functPtr->nargs; i++) {
	if (functPtr->arglen[i] == SIZEOF32) {
	    argv[i].isint = 1;
	    argv[i].len = SIZEOF32;
	    argv[i].u.integer = inpArgv[i];
	} else if (functPtr->arglen[i] == 2*SIZEOF32) {    /* 64 bit int */
            argv[i].isint = 1;
            argv[i].len = 2*SIZEOF32;
#ifdef ADDR_64BIT
            argv[i].u.ptr = (char*) inpArgv[i];
#else
            tmpInt = inpArgv[i];
            argv[i].u.ptr = (char*) tmpInt;
#endif
	} else {
	    argv[i].isint = 0;
#ifdef ADDR_64BIT
	    if ((char *) inpArgv[i] == NULL)
		inpArgv[i] = (srb_long_t) "";
	    argv[i].len = strlen ((char *) inpArgv[i]) + 1;
	    argv[i].u.ptr = (char*) inpArgv[i];
#else
            tmpInt = inpArgv[i];
            if ((char *) tmpInt == NULL)
                tmpInt = (int) "";
            argv[i].u.ptr = (char*) tmpInt;
            argv[i].len = strlen ((char *) tmpInt) + 1;
#endif
	}
    }

    if (functPtr->retlen == SIZEOF32)
	retType = 1;
    else
	retType = 0;
	
    res = clFunct(conn, funcid, clOut->retval, &clOut->resultLen,
      retType ,argv, functPtr->nargs);

    if (retType == 1) {		/* integer type return */
    	if (clResultStatus(res) != CLI_COMMAND_OK) {
            *(int *) clOut->retval = clResultStatus(res);
    	}
        clClear(res);
    	return (*(int *) clOut->retval);
    } else {		/* byte stream type return */
    	if (clResultStatus(res) == CLI_COMMAND_OK) {
    	} else {
            clOut->resultLen = clResultStatus(res);
	}
        clClear(res);
        return clOut->resultLen;
    }
}

/*
 * srbContainerCreate - Create a container
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *         char *containName - The name of the container to be created.
 *         char *containerType - Data type of the container. e.g. "generic"
 *         char *resourceName - The storage resource name. This should be is a
 *	    logical resource (resource group) consisting of two physical
 *	    resources, a TEMPORARY_RES_CL and a PERMANENT_RES_CL class.
 *         srb_long_t containerSize - The size of the container to be created.
 *
 *
 * Output - Returns 0 ==> success.
 *          Returns a negative value upon failure.
 */

int
srbContainerCreate (srbConn *conn, int catType, char *containerName,
char *containerType, char *resourceName, srb_long_t containerSize)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = catType;
    argv[1] = addrToSRBLong (containerName);
    if (containerType != NULL) {
        argv[2] = addrToSRBLong (containerType);
    } else {
        if (!(argv[2] = addrToSRBLong (getenv("mdasDataTypeName")))) {
            argv[2] = addrToSRBLong (DefMdasDataTypeName);
        }
    }

    if (resourceName != NULL) {
        argv[3] = addrToSRBLong (resourceName);
    } else {
        if (!(argv[3] = addrToSRBLong (getenv("mdasResourceName")))) {
            argv[3] = addrToSRBLong (DefMdasResourceName);
        }
    }
    argv[4] = addrToSRBLong ((char *) &containerSize);

    retval = clCall (conn, F_CONTAINER_CREATE, argv, &clOut);
    return retval;
}

/*
 * srbRegisterContainer - Register a container to MCAT.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *         char *containerName - The name of the container to be registered..
 *         char *resourceName - The storage resource name.
 *              e.g. "mda18-unix-sdsc"
 *         srb_long_t containerSize - The size of the container.
 *
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

int
srbRegisterContainer (srbConn *conn, int catType, char *containerName,
 char *resourceName, srb_long_t containerSize)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = catType;
    argv[1] = addrToSRBLong (containerName);

    if (resourceName != NULL) {
        argv[2] = addrToSRBLong (resourceName);
    } else {
        if (!(argv[2] = addrToSRBLong (getenv("mdasResourceName")))) {
            argv[2] = addrToSRBLong (DefMdasResourceName);
        }
    }

    argv[3] = addrToSRBLong ((char *) &containerSize);

    retval = clCall (conn, F_REGISTER_CONTAINER, argv, &clOut);
    return retval;
}

/*
 * srbRegisterInContDataset - Register a inContainer dataset
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *         char *objID - The SRB object ID to create. The objID is obtained
 *              through registration with MDAS.
 *         char *collectionName - The collection name.
 *         char *containerName - The collection name.
 *         char *dataTypeName - Data type. e.g. "generic"
 *         srb_long_t dataSize - The size of the dataset if known. 0 = unknown.
 *         srb_long_t baseOffset - The offset of the dataset if known. 
 *				   0 = unknown.
 *
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

int srbRegisterInContDataset (srbConn *conn, int catType, char *objID,
 char *collectionName, char *containerName, char *dataTypeName,
srb_long_t dataSize, srb_long_t baseOffset)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = catType;
    argv[1] = addrToSRBLong (objID);
    if (collectionName != NULL) {
        argv[2] = addrToSRBLong (collectionName);
    } else {
        if (!(argv[2] = addrToSRBLong (getenv("mdasCollectionName")))) {
            argv[2] = addrToSRBLong (DefMdasCollectionName);
        }
    }

    argv[3] = addrToSRBLong (containerName);

    if (dataTypeName != NULL) {
        argv[4] = addrToSRBLong (dataTypeName);
    } else {
        if (!(argv[4] = addrToSRBLong (getenv("mdasDataTypeName")))) {
            argv[4] = addrToSRBLong (DefMdasDataTypeName);
        }
    }

    argv[5] = addrToSRBLong ((char *) &dataSize);
    argv[6] = addrToSRBLong ((char *) &baseOffset);

    retval = clCall (conn, F_REGISTER_IN_CONTAINER, argv, &clOut);
    return retval;
}

/*
 * srbGetContainerInfo - Get Info on a SRB container.
 *
 * Input : srbConn* conn,
 *         int catType - The catalog type - 0 = MDAS_CATALOG
 *         char *containerName - The name of the container.
 *         mdasC_sql_result_struct *myresult - A pointer to a user supplied
 *          mdasC_sql_result_struct.
 *         int rowsWanted - number of rows of result wanted.
 * Output:
 *         A mdasC_sql_result_struct *myresult
 *
 * Return value :
 *      0 - success; myresult->continuation_index >= 0, ==> more results from
 *          the query. Use srbGetMoreRows() to retrieve more rows.
 *      a negative value - failure
 */

int
srbGetContainerInfo (srbConn *conn, int catType, char *containerName,
 mdasC_sql_result_struct *myresult, int rowsWanted)
{
    srbResult *res;
    char *buf;
    int buflen;
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;


    argv[0] = catType;
    argv[1] = addrToSRBLong (containerName);
    argv[2] = rowsWanted;

    if ((buflen = getSqlResultLen (DATASET_INFO_COL_CNT, rowsWanted)) <= 0)
        return CLI_ERR_MALLOC;

    buf = malloc (buflen);
    if (buf == NULL)
        return CLI_ERR_MALLOC;

    clOut.retval = (char *) buf;

    retval = clCall (conn, F_GET_CONTAINER_INFO, argv, &clOut);

    if (retval < 0) {
        free (buf);
        return retval;
    }

    strToSqlResult (buf, myresult);

    free (buf);

    if (myresult->result_count > 0)
        return 0;       /* success */
    else
        return CLI_ERR_RETURN_LEN;
}

/*
 * srbGetResOnChoice - Given the Logigical resource name and the inputFlag,
 * return a physical resource name.
 *
 * Input - srbConn* conn - From srbConnect ().
 * 	   int catType - catalog type - 0 - MCAT
 *         char *logResName - The logical resource name.
 *	   char *phyResName - The output physical resource.
 *         char *inputFlag - The Input flag, valid inputs are:
 *          "RR" - Round Robin.
 *          "RANDOM" - randomly selecting a physical resource from the
 *                     input logical resource.
 * Output - The Physical resource string.
 * 	    Returns 0 if successful..
 *          Returns a negative value upon failure.
 * the CALLER must have allocated enough space to hold the result returned
 */
 
int
srbGetResOnChoice(srbConn *conn, int catType, char *logResName, 
char *phyResName, char *inputFlag)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) phyResName;
 
    argv[0] = catType;
    argv[1] = addrToSRBLong (logResName);
    argv[2] = addrToSRBLong (inputFlag);
 
    retval = clCall (conn, F_GET_RESC_ON_CHOICE, argv, &clOut);
    return retval;
}
 
/*
 * srbRmContainer - remove a container
 *
 * Input - srbConn* conn - From srbConnect ().
 * 	   int catType - catalog type - 0 - MCAT
 *         char *containerName - The name of the container.
 *	   int deleteFlag - D_DELETE_ONE - delete the container by the 
 *	   owner. D_SU_DELETE_TRASH_ONE - remove the container from
 *	   the trash bin by admin user.
 *	
 *
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */
 
int
srbRmContainer(srbConn *conn, int catType, char *containerName, int deleteFlag) 
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = catType;
    argv[1] = addrToSRBLong (containerName);
    argv[2] = deleteFlag;
 
    retval = clCall (conn, F_REMOVE_CONTAINER, argv, &clOut);

    return retval;
}
 
/*
 * srbSyncContainer - Sync a container - Sync the permanant replica with the
 * temporary replica.
 *
 * Input - srbConn* conn - From srbConnect ().
 * 	   int catType - catalog type - 0 - MCAT
 *         char *containerName - The name of the container.
 *         int syncFlag - valid values are:
 *              PURGE_FLAG - purge the cache copies after sync is done.
 *              PRIMARY_FLAG - Synchronize to the primary archival resource
 *              only. The default is to synchronize all archival resources.
 *
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */
 
int
srbSyncContainer (srbConn *conn, int catType, char *containerName, 
int syncFlag) 
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = catType;
    argv[1] = addrToSRBLong ( containerName);
    argv[2] = syncFlag;
 
    retval = clCall (conn, F_SYNC_CONTAINER, argv, &clOut);

    return retval;
}
 
/*
 * srbReplContainer - Replicate a container.
 *
 * Input - srbConn* conn - From srbConnect ().
 * 	   int catType - catalog type - 0 - MCAT
 *         char *containerName - The name of the container.
 *         char *newResourceName - The resource for the replica.
 *
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */
 
int
srbReplContainer (srbConn *conn, int catType, char *containerName, 
char *newResourceName) 
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = catType;
    argv[1] = addrToSRBLong (containerName);
    argv[2] = addrToSRBLong (newResourceName);
 
    retval = clCall (conn, F_REPL_CONTAINER, argv, &clOut);

    return retval;
}
 
/*
 * srbBulkRegister - Register object in bulk. This call can perform
 * bulk registration for either all normal objects and or all
 * inContainer objects.
 *
 * Input - srbConn* conn - From srbConnect ().
 * 	   int catType - catalog type - 0 - MCAT
 *         char *containerName/Resource - For normal objects, this input 
 *	   specifies the Resource and for inContainer objects, it is the 
 *	   name of the container.
 *         mdasC_sql_result_struct  *myresult - The mdasC_sql_result_struct
 *	   that contains the objects to be registered.
 * myresult->sqlresult[0].values   should contain dataNameList
 * myresult->sqlresult[1].values   should contain collectionNameList
 * myresult->sqlresult[2].values   should contain dataSizeList (in ascii)
 *                                  (I will perform atol)
 * If all inContainer objects:
 * myresult->sqlresult[3].values   should contain offSetList (in ascii)
 *
 * If all normal objects:
 * myresult->sqlresult[3].values   should contain dataType
 * myresult->sqlresult[4].values   should contain physical pathName 
 *
 * 
 * myresult->row_count             should contain the number of datsets to
 *                                      be registered.
 *
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */
 
int
srbBulkRegister (srbConn *conn, int catType, char *containerName, 
mdasC_sql_result_struct *myresult) 
{
    int retval;
    srbResult *res;
    clArgBlock argv[4];
    struct varlena *packedResult;
    byteStream myBS;
    int resultLen;


    retval = packMsg ((char *) myresult, &packedResult, sqlResultStruct_PF,
     NULL);

    if (retval < 0) {
        fprintf (stderr, "packMsg error, status = %d\n", retval);
	return retval;
    }

    myBS.len = VARSIZE (packedResult) - VARHDRSZ;
    myBS.bytes = VARDATA(packedResult);

    argv[0].isint = 1;
    argv[0].len = 4;
    argv[0].u.integer = catType;

    argv[1].isint = 0;
    if (containerName == NULL)
        containerName = "";
    argv[1].len = strlen (containerName) + 1;
    argv[1].u.ptr = containerName;

    argv[2].isint = 0;
    argv[2].len = myBS.len;
    argv[2].u.ptr = myBS.bytes;

    res = clFunct (conn, F_BULK_REGISTER, (char *) &retval, &resultLen, 1, 
     argv, 3);

    free (packedResult);
    if (clResultStatus(res) != CLI_COMMAND_OK) {
        retval = clResultStatus(res);
    }
    clClear(res);
    return retval;
}
 
/*
 * srbExecCommand - Proxy Operation that exec a command.
 *
 * Input - srbConn* conn - From srbConnect ().
 *	   char *command - The command to run.
 *	   char *commandArgv - The command argument string
 *	   char *proxyAddr - The host address where this proxy operation
 *	                     should be performed. NULL => the server for the
 *			     current connect.
 *         char *filename - The file Path name to open. Must be an
 *         absolute path.
 *	   int portalFlag - The portal flag (see definition in srb.h).
 *			    Valid flags are - PORTAL_OFF, PORTAL_ON,
 *			    PORTAL_STD_IN_OUT.
 *			     
 *
 *
 * Output - Returns 0 or positive value upon success.
 *          Returns a negative value upon failure.
 *          outBuf - any byte stream output.
 */

int
srbExecCommand (srbConn *conn, char *command,
char *commandArgv, char *proxyAddr, char *filename, int portalFlag)
{
    struct sockaddr_in sin;
    int lport = 0;
    int resolvePath = 1;
    char *laddr = NULL;
    int lsock;
    int status;
#ifdef PORTNAME_aix
    size_t length;
#else
    int length;
#endif
    char sb[MAX_TOKEN];
    struct hostent *hostEnt;
    int newSock;
    struct in_addr myIn;

    if (portalFlag > 0) {
	if ((lsock = portalCreate ()) < 0) {
            (void) sprintf(conn->errorMessage,
             "srbExecCommand() -- portalCreate() failed: errno=%d\n%s\n",
              errno, strerror(errno));
            return lsock;
	}
	
	length = sizeof (sin);
	if (getsockname (lsock, (struct sockaddr *) &sin, &length)) {
            (void) sprintf(conn->errorMessage,
             "srbExecCommand() -- getsockname() failed: errno=%d\n%s\n",
              errno, strerror(errno));
            return CLI_ERR_SOCKET;
        }
	lport = ntohs (sin.sin_port);
        laddr = inet_ntoa (conn->port->laddr.sin_addr);

        listen (lsock, 1);	/* accept just one connect */
    }
    status = srbObjProxyOpr (conn, OPR_COMMAND, 
     portalFlag, lport, resolvePath, 0,
      command, commandArgv, proxyAddr, laddr, filename, NULL, NULL,
       NULL);
    if (status < 0) {
#ifdef _WIN32
    closesocket(lsock);
#else
	close (lsock);
#endif
	return status;
    }

    if (portalFlag > 0) {
	newSock = accept (lsock, 0, 0);
	if (newSock < 0) {
	    (void) sprintf(conn->errorMessage,
             "srbExecCommand() -- accept() failed: errno=%d\n%s\n",
              errno, strerror(errno));
            return CLI_ERR_SOCKET;
        }
#ifdef _WIN32
    closesocket(lsock);
#else
	close (lsock);
#endif
	status = newSock;
    }
    return (status);
}

int
srbGetPortalNum ()
{
    char *tmpStr;

    tmpStr = getenv (PORTAL_ENV);
    if (tmpStr != NULL) {
	return (atoi (tmpStr));
    } else {
	return (-1);
    }
}

/*
 * srbSyncData - Sync a data object - Sync the permanant replica with the
 * temporary replica.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type - 0 - MCAT
 *         char *objID - The SRB object ID to Sync.
 *         char *collectionName - The name of the collection this objID
 *              belongs.
 *         char *resource - The resource for the object to sync to.
 *         A NULL or empty string means synchronize existing copies.
 *
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */

int
srbSyncData (srbConn *conn, int catType, char *objID, char *collectionName,
char *resource)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = catType;
    argv[1] = addrToSRBLong (objID);
    argv[2] = addrToSRBLong (collectionName);
    argv[3] = addrToSRBLong (resource);

    retval = clCall (conn, F_SRBO_SYNC_DATA, argv, &clOut);

    return retval;
}

/*
 * srbBackupData - Backup a data object -  Make a replica to the backup
 * resource. Skip it if a good copy already exist.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type - 0 - MCAT
 *         char *objID - The SRB object ID to Sync.
 *         char *collectionName - The name of the collection this objID
 *              belongs.
 *         char *backupResource - The backup resource 
 *	   int flag - not used.
 *
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */

int
srbBackupData (srbConn *conn, int catType, char *objID, char *collectionName,
char *backupResource, int flag)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = catType;
    argv[1] = addrToSRBLong (objID);
    argv[2] = addrToSRBLong (collectionName);
    argv[3] = addrToSRBLong (backupResource);
    argv[4] = flag;

    retval = clCall (conn, F_BACKUP_DATA, argv, &clOut);

    return retval;
}

/*
 * srbContainerOpen - Open a container for ingestion.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type - 0 - MCAT
 *         char *containerName - The name of the container.
 *         int openFlag - O_RDWR, O_WRONLY or O_RDONLY.
 *
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */

int
srbContainerOpen (srbConn *conn, int catType, char *containerName,
int openFlag)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = catType;
    argv[1] = addrToSRBLong (containerName);
    argv[2] = openFlag;

    retval = clCall (conn, F_CONTAINER_OPEN, argv, &clOut);

    return retval;
}

/*
 * srbContainerClose - Close an opened a container.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type - 0 - MCAT
 *         int confFd - The fd returned from srbContainerOpen ().
 *
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */

int
srbContainerClose (srbConn *conn, int confFd)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = confFd;

    retval = clCall (conn, F_CONTAINER_CLOSE, argv, &clOut);

    return retval;
}

/*
 * srbExecFunction - Proxy Operation that exec a function.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *function - The function to run.
 *         char *functionArgv - The function argument string
 *         char *proxyAddr - The host address where this proxy operation
 *                           should be performed. NULL => the server for the
 *                           current connect.
 *         int portalFlag - The portal flag (see definition in srb.h).
 *                          Valid flags are - PORTAL_OFF, PORTAL_ON,
 *                          PORTAL_STD_IN_OUT.
 *
 *
 *
 * Output - Returns 0 or positive value upon success.
 *          Returns a negative value upon failure.
 *          outBuf - any byte stream output.
 */

int
srbExecFunction (srbConn *conn, char *function,
char *functionArgv, char *proxyAddr, int portalFlag)
{    struct sockaddr_in sin;
    int lport = 0;
    char *laddr = NULL;
    int lsock;
    int status;
#ifdef PORTNAME_aix
    size_t length;
#else
    int length;
#endif
    char sb[MAX_TOKEN];
    struct hostent *hostEnt;
    int newSock;

    if (portalFlag > 0) {
        if ((lsock = portalCreate ()) < 0) {
            (void) sprintf(conn->errorMessage,
             "srbExecFunction() -- portalCreate() failed: errno=%d\n%s\n",
              errno, strerror(errno));
            return lsock;
        }

        length = sizeof (sin);
        if (getsockname (lsock, (struct sockaddr *) &sin, &length)) {
            (void) sprintf(conn->errorMessage,
             "srbExecFunction() -- getsockname() failed: errno=%d\n%s\n",
              errno, strerror(errno));
            return CLI_ERR_SOCKET;
        }
        lport = ntohs (sin.sin_port);
        laddr = inet_ntoa (conn->port->laddr.sin_addr);
        listen (lsock, 1);      /* accept just one connect */
    }
    status = srbObjProxyOpr (conn, OPR_FUNCTION,
     portalFlag, lport, 0, 0,
      function, functionArgv, proxyAddr, laddr, NULL, NULL, NULL,
       NULL);
    if (status < 0) {
#ifdef _WIN32
        closesocket(lsock);
#else
        close (lsock);
#endif
        return status;
    }

    if (portalFlag > 0) {
        newSock = accept (lsock, 0, 0);
        if (newSock < 0) {
            (void) sprintf(conn->errorMessage,
             "srbExecFunction() -- accept() failed: errno=%d\n%s\n",
              errno, strerror(errno));
            return CLI_ERR_SOCKET;
        }
#ifdef _WIN32
        closesocket(lsock);
#else
        close (lsock);
#endif
        status = newSock;
    }
    return (status);
}

/*
 * _srbDataCopy - Copy a dataset. This is a priviledged call used by
 * the server only to move files around.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         destObjInx, destPath, destResLoc - the triplet defining the
 *         destination dataset location.
 *         srcObjInx, srcPath, srcResLoc - the triplet defining the
 *         source dataset location.
 *         size - the size of the dataset to be copied. -1 means don't
 *         know.
 *
 *
 * Output - Returns the number of bytes copied.
 *          Returns a negative value upon failure.
 */

srb_long_t
_srbDataCopy(srbConn* conn, int destObjInx, char *destPath, char *destResLoc,
int srcObjInx, char *srcPath, char *srcResLoc, srb_long_t size, int zoneFlag)
{
    clArgBlock argv[8];
    srbResult *res;
    int resultLen;
    srb_long_t retval;

    argv[0].isint = 1;
    argv[0].len = 4;
    argv[0].u.integer = destObjInx;

    argv[1].isint = 0;
    if (destPath == NULL)
        destPath = "";
    argv[1].len = strlen (destPath) + 1;
    argv[1].u.ptr = (char*) destPath;

    argv[2].isint = 0;
    if (destResLoc == NULL)
        destResLoc = "";
    argv[2].len = strlen (destResLoc) + 1;
    argv[2].u.ptr = (char*) destResLoc;

    argv[3].isint = 1;
    argv[3].len = 4;
    argv[3].u.integer = srcObjInx;

    argv[4].isint = 0;
    if (srcPath == NULL)
        srcPath = "";
    argv[4].len = strlen (srcPath) + 1;
    argv[4].u.ptr = (char*) srcPath;

    argv[5].isint = 0;
    if (srcResLoc == NULL)
        srcResLoc = "";
    argv[5].len = strlen (srcResLoc) + 1;
    argv[5].u.ptr = (char*) srcResLoc;

    argv[6].isint = 1;
    argv[6].len = 2*SIZEOF32;
    argv[6].u.ptr = (char*) &size;

    argv[7].isint = 1;
    argv[7].len = 4;
    argv[7].u.integer = zoneFlag;

    res = clFunct(conn, F_DATA_COPY,(char *) &retval,&resultLen,1,argv,8);
    if (clResultStatus(res) == CLI_COMMAND_OK) {
        clClear(res);
        return retval;
    } else {
        retval = clResultStatus(res);
        clClear(res);
        return retval;
    }
}

/*
 * srbObjCopy - Copy a dataset. This is a client call as oppose to
 *_srbDataCopy which is only used by the server.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *srcObjID - The source object.
 *         char *srcCollection - The source collection.
 *         char *destObjID - The destination object.
 *         char *destCollection - The destination collection.
 *         char *destResource - The resource to put the destination object.
 *
 * Output - Returns the number of bytes copied.
 *          Returns a negative value upon failure.
 */

srb_long_t
srbObjCopy(srbConn* conn, char *srcObjID, char *srcCollection,
char *destObjID, char *destCollection, char *destResource)
{
    clArgBlock argv[5];
    srbResult *res;
    int resultLen;
    srb_long_t retval;

    argv[0].isint = 0;
    if (srcObjID == NULL)
        srcObjID = "";
    argv[0].len = strlen (srcObjID) + 1;
    argv[0].u.ptr = (char*) srcObjID;

    argv[1].isint = 0;
    if (srcCollection == NULL)
        srcCollection = "";
    argv[1].len = strlen (srcCollection) + 1;
    argv[1].u.ptr = (char*) srcCollection;

    argv[2].isint = 0;
    if (destObjID == NULL)
        destObjID = "";
    argv[2].len = strlen (destObjID) + 1;
    argv[2].u.ptr = (char*) destObjID;

    argv[3].isint = 0;
    if (destCollection == NULL)
        destCollection = "";
    argv[3].len = strlen (destCollection) + 1;
    argv[3].u.ptr = (char*) destCollection;

    argv[4].isint = 0;
    if (destResource == NULL)
        destResource = "";
    argv[4].len = strlen (destResource) + 1;
    argv[4].u.ptr = (char*) destResource;

    res = clFunct(conn, F_SRBO_COPY,(char *) &retval,&resultLen,1,argv,5);
    if (clResultStatus(res) == CLI_COMMAND_OK) {
        clClear(res);
        return retval;
    } else {
        retval = clResultStatus(res);
        clClear(res);
        return retval;
    }
}

/*
 * _srbDataPut - Copy a dataset from local space to SRB space. This is
 * only used by the SRB server.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int destObjInx - The object type index.
 *         e.g., objType = srbObjTypeEntry[destObjInx].objType.
 *         char *destPath - The destination path.
 *         char *destResLoc - The destination resource.
 *         char *clHostAddr - The client (source) portal network address.
 *         int clPort - The client portal port number.
 *         srb_long_t size - The size of the file. Must be non-negative.
 *
 * Output - Returns the number of bytes copied.
 *          Returns a negative value upon failure.
 */

srb_long_t
_srbDataPut(srbConn* conn, int destObjInx, char *destPath, char *destResLoc,
char *clHostAddr, int clPort, srb_long_t size)
{
    clArgBlock argv[6];
    srbResult *res;
    int resultLen;
    srb_long_t retval;

    argv[0].isint = 1;
    argv[0].len = 4;
    argv[0].u.integer = destObjInx;

    argv[1].isint = 0;
    if (destPath == NULL)
        destPath = "";
    argv[1].len = strlen (destPath) + 1;
    argv[1].u.ptr = (char*) destPath;

    argv[2].isint = 0;
    if (destResLoc == NULL)
        destResLoc = "";
    argv[2].len = strlen (destResLoc) + 1;
    argv[2].u.ptr = (char*) destResLoc;

    argv[3].isint = 0;
    if (clHostAddr == NULL)
        clHostAddr = "";
    argv[3].len = strlen (clHostAddr) + 1;
    argv[3].u.ptr = (char*) clHostAddr;

    argv[4].isint = 1;
    argv[4].len = 4;
    argv[4].u.integer = clPort;

    argv[5].isint = 1;
    argv[5].len = 2*SIZEOF32;
    argv[5].u.ptr = (char*) &size;

    res = clFunct(conn, F_DATA_PUT,(char *) &retval,&resultLen,1,argv,6);
    if (clResultStatus(res) == CLI_COMMAND_OK) {
        clClear(res);
        return retval;
    } else {
        retval = clResultStatus(res);
        clClear(res);
        return retval;
    }
}

/*
 * srbObjPut - Copy a dataset from local space to SRB space. This is
 * only used by the SRB client vs _srbDataPut which is used only by the
 * SRB server.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *destObjID - The destination objID.
 *         char *destCollection - The destination collwction.
 *         char *destResLoc - The destination resource.
 *         char *dataType - The data type.
 *         char *destPath - The destination path name.
 *         char *locFilePath - The local fullPath name.
 *         srb_long_t size - The size of the file. negative means don't know.
 *         int forceFlag - over write flag
 *
 * Output - Returns the number of bytes copied.
 *          Returns a negative value upon failure.
 */

#ifdef PARA_OPR
srb_long_t
srbObjPut(srbConn* conn, char *destObjID, char *destCollection,
char *destResLoc, char *dataType, char *destPath, char *locFilePath,
srb_long_t size, int forceFlag)
{
    clArgBlock argv[9];
    srbResult *res;
    int resultLen;
    srb_long_t retval;

    int clPort;
    char *clHostAddr;
    int lsock;
#ifdef PORTNAME_aix
    size_t length;
#else
    int length;
#endif
    struct sockaddr_in sin;
    srb_long_t status;
    struct portalInput myInput;
    pthread_t portalManagerThread;
    char sb[MAX_TOKEN];
    struct hostent *hostEnt;
    struct in_addr myIn;
    struct relayHeader myHeader;
    int mySock;

    if (size < 0) {
        struct stat statbuf;

        status = stat (locFilePath, &statbuf);
        if (status != 0) {
            (void) sprintf (conn->errorMessage,
             "srbObjPut() -- stat() failed: errno=%d\n%s\n",
              errno, strerror(errno));
            return status;
        }

        size = statbuf.st_size;
    }

    if ((lsock = portalCreate ()) < 0) {
        (void) sprintf (conn->errorMessage,
         "srbObjPut() -- portalCreate() failed: errno=%d\n%s\n",
          errno, strerror(errno));
        return lsock;
    }

    listen(lsock, SOMAXCONN);

    length = sizeof (sin);
    if (getsockname (lsock, (struct sockaddr *) &sin, &length)) {
        (void) sprintf(conn->errorMessage,
         "srbExecCommand() -- getsockname() failed: errno=%d\n%s\n",
          errno, strerror(errno));
        return CLI_ERR_SOCKET;
    }
    clPort = ntohs (sin.sin_port);
    clPort = genPortInp (clPort);
    clHostAddr = inet_ntoa (conn->port->laddr.sin_addr);

    myInput.sock = lsock;
    myInput.cookie = getCookieFromPortInp (clPort);
    myInput.locFilePath = locFilePath;
    myInput.operation = PUT_OPR;
    myInput.transferHandler = (void *) portalTransfer;
    myInput.size = size;
    myInput.numThreads = 0;
    myInput.threadInx = 0;
    myInput.bytesMoved = 0;
    myInput.status = 0;
    myInput.errorMessage = conn->errorMessage;
    pthread_mutex_init(&myInput.varMutex, pthread_mutexattr_default);
    pthread_mutex_init(&myInput.condMutex, pthread_mutexattr_default);
    pthread_cond_init (&myInput.cond, pthread_condattr_default);



   status = pthread_create(&portalManagerThread, pthread_attr_default,
                  (void *(*)(void *)) portalManager,
                  (void *) &myInput);

    if (status < 0) {
        (void) sprintf (conn->errorMessage,
         "srbObjPut() -- pthread_create () failed: errno=%d\n%s\n",
          errno, strerror(errno));
        return SYS_ERR_PTHREAD_CREATE;
    }

    argv[0].isint = 0;
    if (destObjID == NULL)
        destObjID = "";
    argv[0].len = strlen (destObjID) + 1;
    argv[0].u.ptr = (char*) destObjID;

    argv[1].isint = 0;
    if (destCollection == NULL)
        destCollection = "";
    argv[1].len = strlen (destCollection) + 1;
    argv[1].u.ptr = (char*) destCollection;

    argv[2].isint = 0;
    if (destResLoc == NULL)
        destResLoc = "";
    argv[2].len = strlen (destResLoc) + 1;
    argv[2].u.ptr = (char*) destResLoc;

    argv[3].isint = 0;
    if (clHostAddr == NULL)
        clHostAddr = "";
    argv[3].len = strlen (clHostAddr) + 1;
    argv[3].u.ptr = (char*) clHostAddr;

    argv[4].isint = 1;
    argv[4].len = 4;
    argv[4].u.integer = clPort;

    argv[5].isint = 0;
    if (dataType == NULL)
        dataType = "";
    argv[5].len = strlen (dataType) + 1;
    argv[5].u.ptr = (char*) dataType;

    argv[6].isint = 0;
    if (destPath == NULL)
        destPath = "";
    argv[6].len = strlen (destPath) + 1;
    argv[6].u.ptr = (char*) destPath;

    argv[7].isint = 1;
    argv[7].len = 2*SIZEOF32;
    argv[7].u.ptr = (char*) &size;

    argv[8].isint = 1;
    argv[8].len = 4;
    argv[8].u.integer = forceFlag;

    res = clFunct(conn, F_SRBO_PUT,(char *) &retval,&resultLen,1,argv,9);
    if (clResultStatus(res) == CLI_COMMAND_OK) {
        clClear(res);
    } else {
        retval = clResultStatus(res);
        clClear(res);
    }

    if (retval < 0) {
        pthread_cancel (portalManagerThread);
#ifdef _WIN32
        closesocket(lsock);
#else
        close (lsock);
#endif
        return retval;
    }

    pthread_cancel (portalManagerThread);
#ifdef _WIN32
    closesocket(lsock);
#else
    close (lsock);
#endif

    for (i = 0; i < myInput.threadInx; i++) {
        pthread_join (myInput.threadId[i], NULL);
    }
    (void) pthread_join(portalManagerThread, NULL);

    conn->numThread = myInput.threadInx;

#ifdef PORTAL_TIMING
    if (myInput.bytesMoved > 0) {
        float fSec;
        struct timeval diffTime;

       (void) gettimeofday(&myInput.endTime, (struct timezone *)0);
       (void)tvsub(&diffTime, &myInput.endTime, &myInput.startTime);
       fSec = (float)diffTime.tv_sec +
        ((float)diffTime.tv_usec / 1000000.);
        printf ("time to transfer %f Mb = %f sec, %f Mb/sec\n",
         (float) myInput.bytesMoved / 1000000., fSec,
          (float) myInput.bytesMoved/ 1000000. / fSec);
    }
#endif  /* PORTAL_TIMING */

    /* if myInput.bytesMoved != retval, must be multiple of retval (discovered
     * not true for HPSS mover transfer. Different mover threads could
     * request for the same offset and data) */
    if (retval > 0 && 
     (retval > myInput.bytesMoved || ((myInput.bytesMoved % retval) != 0) &&
      retval != size)) {
        if (myInput.status < 0)
            return (myInput.status);
        else
            return (OBJ_ERR_COPY_LEN);
    } else if (retval == 0 && retval != myInput.bytesMoved) {
        if (myInput.status < 0)
            return (myInput.status);
        else
            return (OBJ_ERR_COPY_LEN);
    }

    return retval;
}

/*
 * srbObjPutC - The client initiated version of srbObjPut. Copy a dataset 
 * from local space to SRB space. This is only used by the SRB client vs 
 * _srbDataPut which is used only by the SRB server.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *destObjID - The destination objID.
 *         char *destCollection - The destination collwction.
 *         char *destResLoc - The destination resource.
 *         char *dataType - The data type.
 *         char *destPath - The destination path name.
 *         char *locFilePath - The local fullPath name.
 *         srb_long_t size - The size of the file. negative means don't know.
 *         int forceFlag - over write flag
 *	   int numThreads - number of threads
 *
 * Output - Returns the number of bytes copied.
 *          Returns a negative value upon failure.
 */

srb_long_t
srbObjPutC (srbConn* conn, char *destObjID, char *destCollection,
char *destResLoc, char *dataType, char *destPath, char *locFilePath,
srb_long_t size, int forceFlag, int numThreads)
{
    clArgBlock argv[8];
    srbResult *res;
    int resultLen;
    srb_long_t retval;

    int clPort;
    char *clHostAddr;
    int lsock;
#ifdef PORTNAME_aix
    size_t length;
#else
    int length;
#endif
    struct sockaddr_in sin;
    int status;
    struct portalInput myInput;
    char sb[MAX_TOKEN];
    struct hostent *hostEnt;
    struct in_addr myIn;
    struct firstHeader myHeader;
    int mySock;
    char mybuf[10*MAX_TOKEN];
    portList_t *myPortList;
    int newSock;

    if (size < 0) {
        struct stat statbuf;

        status = stat (locFilePath, &statbuf);
        if (status != 0) {
            (void) sprintf (conn->errorMessage,
             "srbObjPut() -- stat() failed: errno=%d\n%s\n",
              errno, strerror(errno));
            return status;
        }

        size = statbuf.st_size;
    }

    argv[0].isint = 0;
    if (destObjID == NULL)
        destObjID = "";
    argv[0].len = strlen (destObjID) + 1;
    argv[0].u.ptr = (char*) destObjID;

    argv[1].isint = 0;
    if (destCollection == NULL)
        destCollection = "";
    argv[1].len = strlen (destCollection) + 1;
    argv[1].u.ptr = (char*) destCollection;

    argv[2].isint = 0;
    if (destResLoc == NULL)
        destResLoc = "";
    argv[2].len = strlen (destResLoc) + 1;
    argv[2].u.ptr = (char*) destResLoc;

    argv[3].isint = 0;
    if (dataType == NULL)
        dataType = "";
    argv[3].len = strlen (dataType) + 1;
    argv[3].u.ptr = (char*) dataType;

    argv[4].isint = 0;
    if (destPath == NULL)
        destPath = "";
    argv[4].len = strlen (destPath) + 1;
    argv[4].u.ptr = (char*) destPath;

    argv[5].isint = 1;
    argv[5].len = 2*SIZEOF32;
    argv[5].u.ptr = (char*) &size;

    argv[6].isint = 1;
    argv[6].len = 4;
    argv[6].u.integer = forceFlag;

    argv[7].isint = 1;
    argv[7].len = 4;
    argv[7].u.integer = numThreads;

    res = clFunct(conn, F_SRBO_PUT_C,(char *) mybuf,&resultLen,0,argv,8);

    status = clResultStatus(res);
    clClear(res);

    if (status < 0) {
	retval = status;
        return retval;
    }

    while (status == NEW_PORTLIST) {
        status = unpackMsg (mybuf, (char **) &myPortList, portList_t_PF, NULL);
	if (status < 0) {
	    fprintf (stderr, 
	     "srbObjPutC: Error unpacking portList_t_PF, status=%d\n",
	     status);
	    retval = status;
            return retval;
        }

        newSock = portalConnect (myPortList->portNum[0], myPortList->hostAddr);
        if (newSock < 0) {
            (void) sprintf(conn->errorMessage,
             "srbObjGetC() -- portalConnect() to %s failed: errno=%d\n%s\n",
              myPortList->hostAddr, errno, strerror(errno));

            res = clReadServerReturn (conn, mybuf, &resultLen, 0);

            status = clResultStatus(res);
	    free (myPortList);
            clClear(res);
            continue;
        }

        /* we could cast the relayHeader structure because relayHeader and
         * firstHeader has the same struct */
        status = readHeader (newSock, (struct relayHeader *) &myHeader);
        if (status < 0) {
#ifdef _WIN32
            closesocket(newSock);
#else
            close (newSock);
#endif
            res = clReadServerReturn (conn, mybuf, &resultLen, 0);

            status = clResultStatus(res);
	    free (myPortList);
            clClear(res);
            continue;
        }

#ifdef foo
    myInput.sock = lsock;
    myInput.cookie = getCookieFromPortInp (clPort);
#endif
        myInput.locFilePath = locFilePath;
        myInput.operation = PUT_OPR;
        myInput.transferHandler = (void *) portalTransfer;
        myInput.size = size;
        myInput.numThreads = 0;
        myInput.threadInx = 0;
        myInput.bytesMoved = 0;
        myInput.status = 0;
        myInput.errorMessage = conn->errorMessage;
        pthread_mutex_init(&myInput.varMutex, pthread_mutexattr_default);
        pthread_mutex_init(&myInput.condMutex, pthread_mutexattr_default);
        pthread_cond_init (&myInput.cond, pthread_condattr_default);

        for (i = 0; i < myHeader.numThreads; i++) {
            if (i > 0) {
                newSock = portalConnect (myPortList->portNum[0],
                 myPortList->hostAddr);
                if (newSock < 0) {
                    (void) sprintf(conn->errorMessage,
                     "srbObjGetC()--portalConnect() to %s failed:errno=%d\n%s\n"
,
                      myPortList->hostAddr, errno, strerror(errno));
                    break;
                }
            }
            myInput.tranInp[i].relaySocketFd = newSock;
            myInput.tranInp[i].threadInx = i;
            myInput.tranInp[i].portInput = &myInput;
            status = pthread_create(&myInput.threadId[i], pthread_attr_default,
            (void*(*)(void*)) myInput.transferHandler,
             (void *)&myInput.tranInp[i]);
            if (status < 0) {
                fprintf (stderr,
                 "srbObjGetC: pthread_create failed. errno = %d\n", errno);
            }
        }
	free (myPortList);
        res = clReadServerReturn (conn, mybuf, &resultLen, 0);

        if ((status = clResultStatus(res)) < 0) {
            for (i = 0; i < myHeader.numThreads; i++) {
                pthread_cancel (myInput.threadId[i]);
            }
            clClear(res);
            return status;
        }

        for (i = 0; i < myHeader.numThreads; i++) {
            pthread_join (myInput.threadId[i], NULL);
        }
        conn->numThread = myHeader.numThreads;

        /* read the extra status */

    }

    if (myHeader.size != myInput.bytesMoved) {
        if (myInput.status < 0)
            return (myInput.status);
        else
            return (OBJ_ERR_COPY_LEN);
    }
    if (myHeader.size == 0) {
        int myFd;
        /* make a 0 length file */
        myFd = open (locFilePath, O_RDWR | O_CREAT, 0644);
        close (myFd);
    }
    return (myHeader.size);

#ifdef foo
    /* if myInput.bytesMoved != retval, must be multiple of retval (discovered
     * not true for HPSS mover transfer. Different mover threads could
     * request for the same offset and data) */
    if (retval > 0 && 
     (retval > myInput.bytesMoved || ((myInput.bytesMoved % retval) != 0) &&
      retval != size)) {
        if (myInput.status < 0)
            return (myInput.status);
        else
            return (OBJ_ERR_COPY_LEN);
    } else if (retval == 0 && retval != myInput.bytesMoved) {
        if (myInput.status < 0)
            return (myInput.status);
        else
            return (OBJ_ERR_COPY_LEN);
    }

    return retval;
#endif
}

/*
 * _srbDataPutC - The client driven version of _srbDataPut. Put a dataset
 * from SRB space to local space. This is only used by the SRB server.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int destTypeInx - The object type index.
 *         e.g., objType = srbObjTypeEntry[destObjInx].objType.
 *         char *destPath - The source path.
 *         char *destResLoc - The source resource.
 *         srb_long_t size - The size of the object.
 *         int flag - must have CL_INIT_CONN_FLAG on
 *         int numThreads - input number of threads
 *         portList_t **myPortList - the output portList.
 *
 * Output - Returns the number of bytes copied.
 *          Returns a negative value upon failure.
 */

srb_long_t
_srbDataPutC(srbConn* conn, int destTypeInx, char *destPath, char *destResLoc,
srb_long_t size, int flag, int numThreads, portList_t **myPortList)
{
    clArgBlock argv[6];
    srbResult *res;
    int resultLen;
    char retval[10*MAX_TOKEN];
    int status;

    argv[0].isint = 1;
    argv[0].len = 4;
    argv[0].u.integer = destTypeInx;

    argv[1].isint = 0;
    if (destPath == NULL)
        destPath = "";
    argv[1].len = strlen (destPath) + 1;
    argv[1].u.ptr = (char*) destPath;

    argv[2].isint = 0;
    if (destResLoc == NULL)
        destResLoc = "";
    argv[2].len = strlen (destResLoc) + 1;
    argv[2].u.ptr = (char*) destResLoc;

    argv[3].isint = 1;
    argv[3].len = 2*SIZEOF32;
    argv[3].u.ptr = (char*) &size;

    argv[4].isint = 1;
    argv[4].len = 4;
    argv[4].u.integer = flag;

    argv[5].isint = 1;
    argv[5].len = 4;
    argv[5].u.integer = numThreads;


    res = clFunct(conn, F_DATA_PUT_C,(char *) &retval,&resultLen,0,argv,6);
    if ((status = clResultStatus(res)) < 0) {
        clClear(res);
        return status;
    }

    status = unpackMsg (retval, (char **) myPortList, portList_t_PF, NULL);

    if (status < 0) {
        return status;
    }

    return status;
}

/*
 * srbObjGet - Copy a dataset from SRB space to local space. This is
 * only used by the SRB client vs _srbDataGet which is used only by the
 * SRB server.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *srcObjID - The source objID.
 *         char *srcCollection - The source collwction.
 *         char *locFilePath - The local fullPath name.
 *
 * Output - Returns the number of bytes copied.
 *          Returns a negative value upon failure.
 */

srb_long_t
srbObjGet (srbConn* conn, char *srcObjID, char *srcCollection,
char *locFilePath)
{
    clArgBlock argv[4];
    srbResult *res;
    int resultLen;
    srb_long_t retval;

    int clPort;
    char *clHostAddr;
    int lsock;
#ifdef PORTNAME_aix
    size_t length;
#else
    int length;
#endif
    struct sockaddr_in sin;
    srb_long_t status;
    struct portalInput myInput;
    pthread_t portalManagerThread;
    char sb[MAX_TOKEN];
    struct hostent *hostEnt;
    struct relayHeader myHeader;
    int mySock;
    int retryCnt = 0;


    if ((lsock = portalCreate ()) < 0) {
        (void) sprintf (conn->errorMessage,
         "srbObjPut() -- portalCreate() failed: errno=%d\n%s\n",
          errno, strerror(errno));
        return lsock;
    }

    listen(lsock, SOMAXCONN);

    length = sizeof (sin);
    if (getsockname (lsock, (struct sockaddr *) &sin, &length)) {
        (void) sprintf(conn->errorMessage,
         "srbExecCommand() -- getsockname() failed: errno=%d\n%s\n",
          errno, strerror(errno));
        return CLI_ERR_SOCKET;
    }
    clPort = ntohs (sin.sin_port);
    clPort = genPortInp (clPort);
    clHostAddr = inet_ntoa (conn->port->laddr.sin_addr);
    myInput.sock = lsock;
    myInput.cookie = getCookieFromPortInp (clPort);
    myInput.locFilePath = locFilePath;
    myInput.operation = GET_OPR;
    myInput.transferHandler = (void *) portalTransfer;
    myInput.size = -1;
    myInput.numThreads = 0;
    myInput.threadInx = 0;
    myInput.bytesMoved = 0;
    myInput.status = 0;
    myInput.errorMessage = conn->errorMessage;
    pthread_mutex_init(&myInput.varMutex, pthread_mutexattr_default);
    pthread_mutex_init(&myInput.condMutex, pthread_mutexattr_default);
    pthread_cond_init (&myInput.cond, pthread_condattr_default);



   status = pthread_create(&portalManagerThread, pthread_attr_default,
                  (void *(*)(void *)) portalManager,
                  (void *) &myInput);

    if (status < 0) {
        (void) sprintf (conn->errorMessage,
         "srbObjPut() -- pthread_create () failed: errno=%d\n%s\n",
          errno, strerror(errno));
        return status;
    }

    argv[0].isint = 0;
    if (srcObjID == NULL)
        srcObjID = "";
    argv[0].len = strlen (srcObjID) + 1;
    argv[0].u.ptr = (char*) srcObjID;

    argv[1].isint = 0;
    if (srcCollection == NULL)
        srcCollection = "";
    argv[1].len = strlen (srcCollection) + 1;
    argv[1].u.ptr = (char*) srcCollection;

    argv[2].isint = 0;
    if (clHostAddr == NULL)
        clHostAddr = "";
    argv[2].len = strlen (clHostAddr) + 1;
    argv[2].u.ptr = (char*) clHostAddr;

    argv[3].isint = 1;
    argv[3].len = 4;
    argv[3].u.integer = clPort;

    res = clFunct(conn, F_SRBO_GET,(char *) &retval,&resultLen,1,argv,4);
    if (clResultStatus(res) == CLI_COMMAND_OK) {
        clClear(res);
    } else {
        retval = clResultStatus(res);
        clClear(res);
    }

    if (retval < 0) {
        pthread_cancel (portalManagerThread);
#ifdef _WIN32
        closesocket(lsock);
#else
        close (lsock);
#endif
        return retval;
    }

    /* this loop prevent a race condition  for very small file transfer
     * the server may send a the short too quickly 
     */
    while (myInput.threadInx <= 0 && retryCnt <= MAX_OPEN_RETRY) {
        srbThreadSleep (0, 100000);
        retryCnt ++;
    }

    for (i = 0; i < myInput.threadInx; i++) {
        pthread_join (myInput.threadId[i], NULL);
    }
    pthread_cancel (portalManagerThread);
#ifdef _WIN32
    closesocket(lsock);
#else
    close (lsock);
#endif
    (void) pthread_join(portalManagerThread, NULL);

    conn->numThread = myInput.threadInx;

#ifdef PORTAL_TIMING
    if (myInput.bytesMoved > 0) {
        float fSec;
        struct timeval diffTime;

       (void) gettimeofday(&myInput.endTime, (struct timezone *)0);
       (void)tvsub(&diffTime, &myInput.endTime, &myInput.startTime);
       fSec = (float)diffTime.tv_sec +
        ((float)diffTime.tv_usec / 1000000.);
        printf ("time to transfer %f Mb = %f sec, %f Mb/sec\n",
         (float) myInput.bytesMoved / 1000000., fSec,
          (float) myInput.bytesMoved/ 1000000. / fSec);
    }
#endif  /* PORTAL_TIMING */

    if (retval >= 0 && retval != myInput.bytesMoved) {
        if (myInput.status < 0)
            return (myInput.status);
        else
            return (OBJ_ERR_COPY_LEN);
    }
    if (retval == 0) {
        /* make a 0 length file */
        lsock = open (locFilePath, O_RDWR | O_CREAT, 0644);
        close (lsock);
    }
    return retval;
}

/*
 * srbObjGetC - The client initiated version of srbObjGet. Copy a dataset 
 * from SRB space to local space. This is only used by the SRB client 
 * vs _srbDataGet which is used only by the SRB server.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *srcObjID - The source objID.
 *         char *srcCollection - The source collwction.
 *         char *locFilePath - The local fullPath name.
 *	   int flag - not used currently
 *	   int numThreads - number of threads
 *
 * Output - Returns the number of bytes copied.
 *          Returns a negative value upon failure.
 */

srb_long_t
srbObjGetC (srbConn* conn, char *srcObjID, char *srcCollection,
char *locFilePath, int flag, int numThreads)
{
    int status;
#ifdef PORTNAME_aix
    size_t length;
#else
    int length;
#endif
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    char mybuf[10*MAX_TOKEN];
    char *buf;
    portList_t *myPortList;
    struct firstHeader myHeader;
    int newSock;
    struct portalInput myInput;
    int actualResultLen;
    srbResult *res;

    clOut.retval = (char *) mybuf;

    argv[0] = addrToSRBLong (srcObjID);
    argv[1] = addrToSRBLong (srcCollection);
    argv[2] = flag;
    argv[3] = numThreads;

    status = clCall (conn, F_SRBO_GET_C, argv, &clOut);

    if (status < 0) {
        return status;
    }

    while (status == NEW_PORTLIST) {
        status = unpackMsg (mybuf, (char **) &myPortList, portList_t_PF, NULL);
        if (status < 0) {
            fprintf (stderr,
             "srbObjGetC: Error unpacking portList_t_PF, status=%d\n",
             status);
            return status;
        }

        newSock = portalConnect (myPortList->portNum[0], myPortList->hostAddr);
        if (newSock < 0) {
            (void) sprintf(conn->errorMessage,
             "srbObjGetC() -- portalConnect() to %s failed: errno=%d\n%s\n",
              myPortList->hostAddr, errno, strerror(errno));

            res = clReadServerReturn (conn, mybuf, &actualResultLen, 0);

            status = clResultStatus(res);
            free (myPortList);
            clClear(res);
            continue;
	}

        /* we could cast the relayHeader structure because relayHeader and
         * firstHeader has the same struct */
        status = readHeader (newSock, (struct relayHeader *) &myHeader);
        if (status < 0) {
#ifdef _WIN32
            closesocket(newSock);
#else
            close (newSock);
#endif
            res = clReadServerReturn (conn, mybuf, &actualResultLen, 0);

            status = clResultStatus(res);
            free (myPortList);
            clClear(res);
            continue;
        }

        myInput.locFilePath = locFilePath;
        myInput.operation = GET_OPR;
        myInput.transferHandler = (void *) portalTransfer;
        myInput.size = -1;
        myInput.numThreads = 0;
        myInput.threadInx = 0;
        myInput.bytesMoved = 0;
        myInput.status = 0;
        myInput.errorMessage = conn->errorMessage;
        pthread_mutex_init(&myInput.varMutex, pthread_mutexattr_default);
        pthread_mutex_init(&myInput.condMutex, pthread_mutexattr_default);
        pthread_cond_init (&myInput.cond, pthread_condattr_default);

        for (i = 0; i < myHeader.numThreads; i++) {
    	    if (i > 0) {
	        newSock = portalConnect (myPortList->portNum[0], 
	         myPortList->hostAddr);
                if (newSock < 0) {
                    (void) sprintf(conn->errorMessage,
                     "srbObjGetC()--portalConnect() to %s failed:errno=%d\n%s\n",
                      myPortList->hostAddr, errno, strerror(errno));
		    break;
	        }
	    }
            myInput.tranInp[i].relaySocketFd = newSock;
            myInput.tranInp[i].threadInx = i;
            myInput.tranInp[i].portInput = &myInput;
            status = pthread_create(&myInput.threadId[i], pthread_attr_default,
            (void*(*)(void*)) myInput.transferHandler, 
	     (void *)&myInput.tranInp[i]);
            if (status < 0) {
                fprintf (stderr, 
		 "srbObjGetC: pthread_create failed. errno = %d\n", errno);
            }
        }
	free (myPortList);
        res = clReadServerReturn (conn, mybuf, &actualResultLen, 0);

        if ((status = clResultStatus(res)) < 0) {
            for (i = 0; i < myHeader.numThreads; i++) {
                pthread_cancel (myInput.threadId[i]);
            }
            clClear(res);
            return status;
        }

        for (i = 0; i < myHeader.numThreads; i++) {
            pthread_join (myInput.threadId[i], NULL);
        }
        conn->numThread = myHeader.numThreads;

        /* read the extra status */

#ifdef foo	/* moved up */
        res = clReadServerReturn (conn, mybuf, &actualResultLen, 0);

        if ((status = clResultStatus(res)) < 0) {
            clClear(res);
            return status;
        }
#endif
    }

    if (myHeader.size != myInput.bytesMoved) {
        if (myInput.status < 0)
            return (myInput.status);
        else
            return (OBJ_ERR_COPY_LEN);
    }
    if (myHeader.size == 0) {
	int myFd;
        /* make a 0 length file */
        myFd = open (locFilePath, O_RDWR | O_CREAT, 0644);
        close (myFd);
    }
    return (myHeader.size);
}
#endif /* PARA_OPR */

/*
 * _srbDataGet - Get a dataset from SRB space to local space. This is
 * only used by the SRB server.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int srcTypeInx - The object type index.
 *         e.g., objType = srbObjTypeEntry[destObjInx].objType.
 *         char *srcPath - The source path.
 *         char *srcResLoc - The source resource.
 *         char *clHostAddr - The client (source) portal network address.
 *         int clPort - The client portal port number.
 *         srb_long_t size - The size of the object.
 *
 * Output - Returns the number of bytes copied.
 *          Returns a negative value upon failure.
 */

srb_long_t
_srbDataGet(srbConn* conn, int srcTypeInx, char *srcPath, char *srcResLoc,
char *clHostAddr, int clPort, srb_long_t size)
{
    clArgBlock argv[6];
    srbResult *res;
    int resultLen;
    srb_long_t retval;

    argv[0].isint = 1;
    argv[0].len = 4;
    argv[0].u.integer = srcTypeInx;

    argv[1].isint = 0;
    if (srcPath == NULL)
        srcPath = "";
    argv[1].len = strlen (srcPath) + 1;
    argv[1].u.ptr = (char*) srcPath;

    argv[2].isint = 0;
    if (srcResLoc == NULL)
        srcResLoc = "";
    argv[2].len = strlen (srcResLoc) + 1;
    argv[2].u.ptr = (char*) srcResLoc;

    argv[3].isint = 0;
    if (clHostAddr == NULL)
        clHostAddr = "";
    argv[3].len = strlen (clHostAddr) + 1;
    argv[3].u.ptr = (char*) clHostAddr;

    argv[4].isint = 1;
    argv[4].len = 4;
    argv[4].u.integer = clPort;

    argv[5].isint = 1;
    argv[5].len = 2*SIZEOF32;
    argv[5].u.ptr = (char*) &size;

    res = clFunct(conn, F_DATA_GET,(char *) &retval,&resultLen,1,argv,6);
    if (clResultStatus(res) == CLI_COMMAND_OK) {
        clClear(res);
        return retval;
    } else {
        retval = clResultStatus(res);
        clClear(res);
        return retval;
    }
}

/*
 * _srbDataGetC - The client driven version of _srbDataGet. Get a dataset 
 * from SRB space to local space. This is only used by the SRB server.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int srcTypeInx - The object type index.
 *         e.g., objType = srbObjTypeEntry[destObjInx].objType.
 *         char *srcPath - The source path.
 *         char *srcResLoc - The source resource.
 *         srb_long_t size - The size of the object.
 *	   int flag - must have CL_INIT_CONN_FLAG on
 *	   int numThreads - input number of threads
 *	   portList_t **myPortList - the output portList.
 *
 * Output - Returns the number of bytes copied.
 *          Returns a negative value upon failure.
 */

srb_long_t
_srbDataGetC(srbConn* conn, int srcTypeInx, char *srcPath, char *srcResLoc,
srb_long_t size, int flag, int numThreads, portList_t **myPortList)
{
    clArgBlock argv[6];
    srbResult *res;
    int resultLen;
    char retval[10*MAX_TOKEN];
    int status;

    argv[0].isint = 1;
    argv[0].len = 4;
    argv[0].u.integer = srcTypeInx;

    argv[1].isint = 0;
    if (srcPath == NULL)
        srcPath = "";
    argv[1].len = strlen (srcPath) + 1;
    argv[1].u.ptr = (char*) srcPath;

    argv[2].isint = 0;
    if (srcResLoc == NULL)
        srcResLoc = "";
    argv[2].len = strlen (srcResLoc) + 1;
    argv[2].u.ptr = (char*) srcResLoc;

    argv[3].isint = 1;
    argv[3].len = 2*SIZEOF32;
    argv[3].u.ptr = (char*) &size;

    argv[4].isint = 1;
    argv[4].len = 4;
    argv[4].u.integer = flag;

    argv[5].isint = 1;
    argv[5].len = 4;
    argv[5].u.integer = numThreads;


    res = clFunct(conn, F_DATA_GET_C,(char *) &retval,&resultLen,0,argv,6);
    if ((status = clResultStatus(res)) < 0) {
        clClear(res);
        return status;
    }

    status = unpackMsg (retval, (char **) myPortList, portList_t_PF, NULL);

    if (status < 0) {
        return status;
    }

    return status;
}

/*
 * srbTapelibMntCart - Mount a tape Cart.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *tapeNum - The tape num (char string) to mount.
 *         int priority - The priority of the mount operation.
 *
 *
 * Output - Returns status of mount. If successful, status == 0. The device
 *          path of the device where the tape cart is mounted is returned in
 *          *outDevPath.
 *          Returns a negative status upon failure.
 */

int
srbTapelibMntCart(srbConn* conn, char *tapeNum, int priority,
char **outDevPath)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;
    char buf[MAX_TOKEN];
    int resultLen;

    clOut.retval = (char *) &clOutVal;

    argv[0] = addrToSRBLong (tapeNum);
    argv[1] = getpid ();
    argv[2] = priority;

    retval = clCall (conn, F_TAPELIB_MNT_CART, argv, &clOut);

    if (retval < 0) {
        *outDevPath = NULL;
    } else {
        /* get the devPath */
        retval = getByteStreamFromSvr (conn, outDevPath, &resultLen);
    }

    return retval;
}

/*
 * srbTapelibDismntCart - Dismount a tape Cart.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *tapeNum - The tape num (char string) to mount.
 *
 *
 * Output - status of dismount.  Returns a negative value upon failure.
 */

int
srbTapelibDismntCart(srbConn* conn, char *tapeNum)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = addrToSRBLong (tapeNum);
    argv[1] = getpid ();

    retval = clCall (conn, F_TAPELIB_DISMNT_CART, argv, &clOut);

    return retval;
}

/*
 * srbGetTapeCartPri - Get the priorities for each cartType depending on the
 * availability of tape drives.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         tapeCartPri_t **outTapeCartPri - The output cartType arrary
 *         specifies the cartType sorted in decending order. i.e.
 *         cartType[0] is the cartType with the highest priority.
 *
 * Output - status of srbGetTapeCartPri.  A negative value means failure.
 *
 */

int
srbGetTapeCartPri (srbConn* conn, tapeCartPri_t **outTapeCartPri)
{
    clArgBlock argv[9];
    srbResult *res;
    int resultLen;
    char *buf;
    int status, retVal;

    buf = malloc (HUGE_STRING);

    argv[0].isint = 1;
    argv[0].len = 4;
    argv[0].u.ptr = (char*) NULL;

    *outTapeCartPri = NULL;

    res = clFunct(conn, F_TAPELIB_GET_CART_PRI, buf,&resultLen,0,argv,0);
    if ((status = clResultStatus(res)) != CLI_COMMAND_OK) {
        free (buf);
        clClear(res);
        return status;
    }

    clClear(res);

    if (resultLen == 0) {
        free (buf);
        return CLI_NO_ANSWER;
    }

    retVal = unpackMsg (buf, (char **) outTapeCartPri,
     tapeCartPri_PF, NULL);

    free (buf);

    if (retVal < 0) {
        fprintf (stderr,
         "srbGetTapeCartPri: unpackMsg() error, status = %d",
          retVal);
        return (retVal);
    }

    return (0);
}

/*
 * srbDumpFileList - Dump a list of files to tape. This is a priviledged
 * function.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         tapeLibInx - The tape library index to dump the files to.
 *         fileList - list of files to dump. - files to be dumped are
 *           contained in the fileList->fileName[numFile][MAX_DATA_SIZE]
 *         array. Note that the fileName is the physical path name of the
 *         cache copy.
 *         purgeFlag - 1 - purge the cache copy when done dumping.
 *                     0 - no purge.
 *
 *
 * Output - status of the dump.  Returns a negative value upon failure.
 */

int
srbDumpFileList (srbConn *conn, int tapeLibInx, fileList_t *fileList,
int purgeFlag)
{
    int retval;
    srbResult *res;
    clArgBlock argv[3];
    struct varlena *packedResult;
    byteStream myBS;
    int resultLen;


    retval = packMsg ((char *) fileList, &packedResult, fileList_t_PF,
     NULL);

    if (retval < 0) {
        fprintf (stderr, "packMsg error, status = %d\n", retval);
        return retval;
    }

    myBS.len = VARSIZE (packedResult) - VARHDRSZ;
    myBS.bytes = VARDATA(packedResult);

    argv[0].isint = 1;
    argv[0].len = 4;
    argv[0].u.integer = tapeLibInx;

    argv[1].isint = 0;
    argv[1].len = myBS.len;
    argv[1].u.ptr = (char *) myBS.bytes;

    argv[2].isint = 1;
    argv[2].len = 4;
    argv[2].u.integer = purgeFlag;

    res = clFunct (conn, F_CMP_DUMP_FILE_LIST, (char *) &retval, &resultLen, 1,
     argv, 3);

    free (packedResult);
    if (clResultStatus(res) != CLI_COMMAND_OK) {
        retval = clResultStatus(res);
    }
    clClear(res);
    return retval;
}

/*
 * srbStageCompObj - Stage a compound Object to cache.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *objID - The name Of the compound obj
 *         char *collectionName - The collection
 *         char *resourceLoc - The resc location of the compound obj.
 *         char *resourceName - The resource of the compound obj.
 *         srb_long_t offset - The offset to be staged
 *         srb_long_t size - The size to be staged.
 *         int replNum - The replication number of the compound obj.
 *         int segNum - The segment number of the compound obj.
 *         int stageFlag - if stageFlag <= 0, just create a zero length
 *             file in cache, register and  close it.
 *         struct mdasInfoOut **outCacheInfoOut - The output struct that
 *             contains info for the cache copy.
 *
 * Output - status of the stage.  Returns a negative value upon failure.
 */


int
srbStageCompObj (srbConn* conn, char *objID, char *collectionName,
char *resourceLoc, char *resourceName,
srb_long_t offset, srb_long_t size,
int replNum, int segNum, int stageFlag,
struct mdasInfoOut **outCacheInfoOut)
{
    clArgBlock argv[9];
    srbResult *res;
    int resultLen;
    char *buf;
    int status, retVal;

    buf = malloc (HUGE_STRING);

    argv[0].isint = 0;
    if (objID == NULL)
        objID = "";
    argv[0].len = strlen (objID) + 1;
    argv[0].u.ptr = (char *) objID;

    argv[1].isint = 0;
    if (collectionName == NULL)
        collectionName = "";
    argv[1].len = strlen (collectionName) + 1;
    argv[1].u.ptr = (char *) collectionName;

    argv[2].isint = 0;
    if (resourceLoc == NULL)
        resourceLoc = "";
    argv[2].len = strlen (resourceLoc) + 1;
    argv[2].u.ptr = (char *) resourceLoc;

    argv[3].isint = 0;
    if (resourceName == NULL)
        resourceName = "";
    argv[3].len = strlen (resourceName) + 1;
    argv[3].u.ptr = (char *) resourceName;

    argv[4].isint = 1;
    argv[4].len = 2*SIZEOF32;
    argv[4].u.ptr = (char*) &offset;

    argv[5].isint = 1;
    argv[5].len = 2*SIZEOF32;
    argv[5].u.ptr = (char*) &size;

    argv[6].isint = 1;
    argv[6].len = 4;
    argv[6].u.integer = replNum;

    argv[7].isint = 1;
    argv[7].len = 4;
    argv[7].u.integer = segNum;

    argv[8].isint = 1;
    argv[8].len = 4;
    argv[8].u.integer = stageFlag;

    *outCacheInfoOut = NULL;
    res = clFunct(conn, F_CMP_STAGE_COMP_OBJ, buf,&resultLen,0,argv,9);
    if ((status = clResultStatus(res)) != CLI_COMMAND_OK) {
        free (buf);
        clClear(res);
        return status;
    }

    clClear(res);

    if (resultLen == 0) {
        free (buf);
        return CLI_NO_ANSWER;
    }

    retVal = unpackMsg (buf, (char **) outCacheInfoOut,
     mdasInfoOut_PF, NULL);

    if (retVal < 0) {
        fprintf (stderr,
         "srbStageCompObj: unpackMsg() of srbStageCompObj error, status = %d",
          retVal);
        free (buf);
        return (retVal);
    }

    (*outCacheInfoOut)->next = NULL;
    (*outCacheInfoOut)->compoundLink = NULL;
    (*outCacheInfoOut)->tapeFileInfo = NULL;

    free (buf);

    return (0);
}

/*
 * srbRegInternalCompObj - register an internal compound obj..
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *objID - The name of the compound obj
 *         char *collectionName - The collection
 *         int objReplNum - The replication number of the compound obj
 *         int objSegNum - The segment number of the compound obj
 *         char *intObjRescName - The resource where the internal compound obj
 *           is located .
 *         char *dataPathName - The physical path of the int comp obj.
 *         srb_long_t dataSize - The size of the int comp obj.
 *         srb_long_t offset - The offset of the int comp obj.
 *         int inpIntReplNum - The replication number of the int compound obj.
 *         int intSegNum - The segment number of the int compound obj.
 *         int objTypeInx - In case of failure, if objTypeInx >=0, the
 *           int comp obj will be removed. It is the object type index
 *           used to do the unlinking.
 *         char *phyResLoc - valid only when objTypeInx >=0. This is the
 *           resouce location used to do the unlinking.
 *
 * Output - status of the registration.  Returns a negative value upon failure.
 */

int
srbRegInternalCompObj (srbConn* conn, char *objName, char *objCollName,
int objReplNum, int objSegNum, char *intObjRescName, char *dataPathName,
srb_long_t dataSize,
srb_long_t offset, int inpIntReplNum, int intSegNum,
int objTypeInx, char *phyResLoc)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = addrToSRBLong (objName);
    argv[1] = addrToSRBLong (objCollName);
    argv[2] = objReplNum;
    argv[3] = objSegNum;
    argv[4] = addrToSRBLong (intObjRescName);
    argv[5] = addrToSRBLong (dataPathName);
    argv[6] = addrToSRBLong ((char *) &dataSize);
    argv[7] = addrToSRBLong ((char *) &offset);
    argv[8] = inpIntReplNum;
    argv[9] = intSegNum;
    argv[10] = objTypeInx;
    argv[11] = addrToSRBLong (phyResLoc);

    retval = clCall (conn, F_CMP_REG_INT_COMP_OBJ, argv, &clOut);

    return retval;
}

/*
 * srbRmIntCompObj - unregister an internal compound obj..
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *objID - The name of the compound obj
 *         char *collectionName - The collection
 *         int objReplNum - The replication number of the compound obj
 *         int objSegNum - The segment number of the compound obj
 *         int inpIntReplNum - The replication number of the int compound obj.
 *         int intSegNum - The segment number of the int compound obj.
 *
 * Output - status of the unregistration. Returns a negative value upon failure.
 */

int
srbRmIntCompObj (srbConn* conn, char *objName, char *objCollName,
int objReplNum, int objSegNum,
int inpIntReplNum, int intSegNum)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = addrToSRBLong (objName);
    argv[1] = addrToSRBLong (objCollName);
    argv[2] = objReplNum;
    argv[3] = objSegNum;
    argv[4] = inpIntReplNum;
    argv[5] = intSegNum;

    retval = clCall (conn, F_CMP_RM_INT_COMP_OBJ, argv, &clOut);

    return retval;
}

/*
 * srbRmCompObj - unregister a compound obj..
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *objID - The name of the compound obj
 *         char *collectionName - The collection
 *         int objReplNum - The replication number of the compound obj
 *         int objSegNum - The segment number of the compound obj
 *
 * Output - status of the unregistration. Returns a negative value upon failure.
 */

int
srbRmCompObj (srbConn* conn, char *objName, char *objCollName,
int objReplNum, int objSegNum)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = addrToSRBLong (objName);
    argv[1] = addrToSRBLong (objCollName);
    argv[2] = objReplNum;
    argv[3] = objSegNum;

    retval = clCall (conn, F_CMP_RM_COMP_OBJ, argv, &clOut);

    return retval;
}
/*
 * srbModInternalCompObj - Modify a SRB internal comp obj
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *         char *objID - The SRB object ID to modify. The objID must
 *              already have been registered with the MDAS catalog.
 *         int objReplNum - The replication number of the compound obj
 *         int objSegNum - The segment number of the compound obj
 *         int inpIntReplNum - The replication number of the int compound obj.
 *         int intSegNum - The segment number of the int compound obj.
 *         char *dataValue1 - Input value 1.
 *         char *dataValue2 - Input value 2.
 *         char *dataValue3 - Input value 3.
 *         char *dataValue4 - Input value 4.
 *         int retractionType - The type of retraction. See srbC_mdas_externs.h
 *                              for the retractionType definition.
 *
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

int
srbModInternalCompObj (srbConn* conn, char *objID, char *collectionName,
int   objReplNum, int objSegNum,
int inpIntReplNum, int intSegNum,
char *data_value_1, char *data_value_2,
char *data_value_3, char *data_value_4, int retraction_type)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = addrToSRBLong (objID);
    argv[1] = addrToSRBLong (collectionName);
    argv[2] = objReplNum;
    argv[3] = objSegNum;
    argv[4] = inpIntReplNum;
    argv[5] = intSegNum;
    argv[6] = addrToSRBLong (data_value_1);
    argv[7] = addrToSRBLong (data_value_2);
    argv[8] = addrToSRBLong (data_value_3);
    argv[9] = addrToSRBLong (data_value_4);
    argv[10] = retraction_type;

    retval = clCall (conn, F_CMP_MOD_INT_COMP_OBJ, argv, &clOut);

    return retval;
}

/*
 * srbModifyRescInfo - Modify/create/delete a SRB resource
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *         char *resourceName - The storage resource name.
 *              e.g. "mda18-unix-sdsc"
 *         int retractionType - The type of retraction. See srbC_mdas_externs.h
 *                              for the retractionType definition.
 *                              For tapes, valid values are T_INSERT_TAPE_INFO,
 *                              T_UPDATE_TAPE_INFO, T_UPDATE_TAPE_INFO_2,
 *                              T_DELETE_TAPE_INFO.
 *         char *dataValue1 - Input value 1.
 *         char *dataValue2 - Input value 2.
 *         char *dataValue3 - Input value 3.
 *         char *dataValue4 - Input value 4.
 *
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

int
srbModifyRescInfo (srbConn* conn, int catType, char *resourceName,
int retractionType,
char *dataValue1, char *dataValue2, char *dataValue3, char *dataValue4)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = catType;
    argv[1] = addrToSRBLong (resourceName);
    argv[2] = retractionType;
    argv[3] = addrToSRBLong (dataValue1);
    argv[4] = addrToSRBLong (dataValue2);
    argv[5] = addrToSRBLong (dataValue3);
    argv[6] = addrToSRBLong (dataValue4);

    retval = clCall (conn, F_MOD_RESC_INFO, argv, &clOut);
    return retval;
}

/*
 * srbRegisterLocation - register location information
 *
 * Input - srbConn* conn - From srbConnect ().
 *	   char *locName - The location name.
 *	   char *fullAddr - Full Address.
 *         char *parentLoc - Parent location
 *	   char *serverUser - Server User
 *         char *serverUserDomain - Server User Domain
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */

int
srbRegisterLocation(srbConn* conn,
	 char *locName,
	 char *fullAddr,
         char *parentLoc,
         char *serverUser,
         char *serverUserDomain)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;
 
    clOut.retval = (char *) &clOutVal;

    argv[0] = addrToSRBLong (locName);
    argv[1] = addrToSRBLong (fullAddr);
    argv[2] = addrToSRBLong (parentLoc);
    argv[3] = addrToSRBLong (serverUser);
    argv[4] = addrToSRBLong (serverUserDomain);
 
    retval = clCall (conn, F_REGISTER_LOCATION, argv, &clOut);
    return retval;
}

/*
 * srbIngestToken - Ingest Token
 *
 * Input - srbConn* conn - From srbConnect ().
 *	   char *typeName - The type name.
 *	   char *newValue - The new value.
 *         char *parentValue - Parent value.
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */

int
srbIngestToken(srbConn* conn,
	 char *typeName,
	 char *newValue,
         char *parentValue)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;
 
    clOut.retval = (char *) &clOutVal;

    argv[0] = addrToSRBLong (typeName);
    argv[1] = addrToSRBLong (newValue);
    argv[2] = addrToSRBLong (parentValue);
 
    retval = clCall (conn, F_INGEST_TOKEN, argv, &clOut);
    return retval;
}

/*
 * srbRegisterResource - Register Resource
 *
 * Input - srbConn* conn - From srbConnect ().
 *	   char *rescName - The resource name.
 *	   char *rescType - Resource type.
 *         char *location - Location.
 *         char *phyPath -  Physical Path.
 *         char *class   -  class.
 *         int size      -  size.
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */

int
srbRegisterResource(srbConn* conn,
		    char *rescName,
		    char *rescType,
		    char *location,
		    char *phyPath,
		    char *class,
		    int size)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = addrToSRBLong (rescName);
    argv[1] = addrToSRBLong (rescType);
    argv[2] = addrToSRBLong (location);
    argv[3] = addrToSRBLong (phyPath);
    argv[4] = addrToSRBLong (class);
    argv[5] = size;
 
    retval = clCall (conn, F_REGISTER_RESOURCE, argv, &clOut);
    return retval;
}

/*
 * srbRegisterLogicalResource - Register Logical Resource
 *
 * Input - srbConn* conn - From srbConnect ().
 *	   char *rescName - The resource name.
 *	   char *rescType - Resource type.
 *         char *phyResc -  Physical resource.
 *         char *phyPath -  Physical path.
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */

int
srbRegisterLogicalResource(srbConn* conn,
		    char *rescName,
		    char *rescType,
		    char *phyResc,
		    char *phyPath)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;
 
    clOut.retval = (char *) &clOutVal;

    argv[0] = addrToSRBLong (rescName);
    argv[1] = addrToSRBLong (rescType);
    argv[2] = addrToSRBLong (phyResc);
    argv[3] = addrToSRBLong (phyPath);
 
    retval = clCall (conn, F_REGISTER_LOGICAL_RESOURCE, argv, &clOut);
    return retval;
}

/*
 * srbRegisterReplicateResourceInfo
 *
 * Input - srbConn* conn - From srbConnect ().
 *	   char *physicalRescName - The physical resource name.
 *	   char *rescType - Resource type.
 *         char *oldLogicalRescName - old logical resource name.
 *         char *indefaultPath -  Indefault Path.
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */

int
srbRegisterReplicateResourceInfo( srbConn* conn,
		    char *physicalRescName,
		    char *rescType,
		    char *oldLogicalRescName,
		    char *indefaultPath)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;
 
    clOut.retval = (char *) &clOutVal;

    argv[0] = addrToSRBLong (physicalRescName);
    argv[1] = addrToSRBLong (rescType);
    argv[2] = addrToSRBLong (oldLogicalRescName);
    argv[3] = addrToSRBLong (indefaultPath);
 
    retval = clCall (conn, F_REGISTER_REPLICATE_RESOURCE_INFO, argv, &clOut);
    return retval;
}

/*
 * srbDeleteValue - Delete a Value; a single MCAT entry
 *
 * Input - srbConn* conn - From srbConnect ().
 *	   int valueType - the value (token) type.
 *	   char *deleteValue - The value (name) that is being deleted.
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */

int
srbDeleteValue(srbConn* conn,
	       int valueType,
	       char *deleteValue)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;
 
    clOut.retval = (char *) &clOutVal;

    argv[0] = valueType;
    argv[1] = addrToSRBLong (deleteValue);
 
    retval = clCall (conn, F_DELETE_VALUE, argv, &clOut);
    return retval;
}
#ifdef FED_MCAT
/*
 * srbGetMcatZone - Get the mcatName of this user.
 *
 * Input - srbConn* conn - From srbConnect ().
 *	   char *userName - The input userName.
 *	   char *domainName - The input domainName.
 *	   char *mcatName - The output mcatname. It should be preallocated
 *		with length MAX_TOKEN  
 *
 * Output - Returns 0 if success.
 *          Returns a negative value upon failure.
 * the CALLER must have allocated enough space to hold the result returned
 */
 
int
srbGetMcatZone(srbConn *conn, char *userName, char *domainName, char *mcatName)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) mcatName;
 
    argv[0] = addrToSRBLong (userName);
    argv[1] = addrToSRBLong (domainName);
 
    retval = clCall (conn, F_GET_MCAT_NAME, argv, &clOut);
    return retval;
}
#endif	/* FED_MCAT */ 

/*
 * srbSetupSessionPublicKey - Get the MCAT-enabled server's public key
 *    in preparation for transferring encryptioned information.
 *    Also see the sscSetupSessionPublicKey library routine.
 *
 * Input - srbConn* conn - From srbConnect ().
 *
 *
 * Output - Returns positive value upon success (strlen of publicKey)
 *          Returns a NULL or negative value for some failures.
 *          publicKey - a string representation of the current public key,
 *               If server-side secure communication is not supported, 
 *               publicKey will instead contain an error message string.
 */
extern int
srbSetupSessionPublicKey (srbConn *conn, char *publicKey)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;
 
    clOut.retval = (char *) publicKey;

    argv[0] = 0;
 
    retval = clCall (conn, F_SETUP_SESSION_PUBLIC_KEY, argv, &clOut);
    return retval;
}

/*
 * srbSetupSession - set up a session (for encryption) with the
 *     MCAT-enabled server
 *    Also see the sscSetupSession library routine.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char* sessionKey - a string representation of the session key
 *                            from sscSetupSession (encrypted in the 
 *                            public key)
 *
 * Output - Returns 0 upon success.
 *          Returns a negative value upon failure.
 */
extern int
srbSetupSession (srbConn *conn, char *sessionKey)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;
 
    clOut.retval = (char *) &clOutVal;

    argv[0] = addrToSRBLong (sessionKey);
 
    retval = clCall (conn, F_SETUP_SESSION, argv, &clOut);
    return retval;
}

/*
 * srbBulkLoad - Bulk load a set of data. All the actual data should
 * already be contained in bloadFullPath, but in concatenated form
 * just as in the case of a container.
 *
 * Input - srbConn* conn - From srbConnect ().
 * 	   int catType - catalog type - 0 - MCAT
 *         char *bloadFullPath - The name of the container.
 *         mdasC_sql_result_struct  *myresult - The mdasC_sql_result_struct
 *	   that contains the objects to be registered.
 * myresult->sqlresult[0].values   should contain dataNameList
 * myresult->sqlresult[1].values   should contain collectionNameList
 * myresult->sqlresult[2].values   should contain dataSizeList (in ascii)
 *                                  (I will perform atol)
 * myresult->sqlresult[3].values   should contain offSetList (in ascii)
 * myresult->row_count             should contain the number of datsets to
 *                                      be registered.
 * myresult->continuation_index    0 - no chksum; K_FLAG - verify and register
 *				   chksum.
 *
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */
 
int
srbBulkLoad (srbConn *conn, int catType, char *bloadFullPath, 
mdasC_sql_result_struct *myresult) 
{
    int retval;
    srbResult *res;
    clArgBlock argv[3];
    struct varlena *packedResult;
    byteStream myBS;
    int resultLen;


    retval = packMsg ((char *) myresult, &packedResult, sqlResultStruct_PF,
     NULL);

    if (retval < 0) {
        fprintf (stderr, "packMsg error, status = %d\n", retval);
	return retval;
    }

    myBS.len = VARSIZE (packedResult) - VARHDRSZ;
    myBS.bytes = VARDATA(packedResult);

    argv[0].isint = 1;
    argv[0].len = 4;
    argv[0].u.integer = catType;

    argv[1].isint = 0;
    if (bloadFullPath == NULL)
        bloadFullPath = "";
    argv[1].len = strlen (bloadFullPath) + 1;
    argv[1].u.ptr = bloadFullPath;

    argv[2].isint = 0;
    argv[2].len = myBS.len;
    argv[2].u.ptr = myBS.bytes;

    res = clFunct (conn, F_BULK_LOAD, (char *) &retval, &resultLen, 1, 
     argv, 3);

    free (packedResult);
    if (clResultStatus(res) != CLI_COMMAND_OK) {
        retval = clResultStatus(res);
    }
    clClear(res);
    return retval;
}
 
/*
 * srbBulkUnload - Bulk unload of non-container files in a collection
 * recursively.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int flag - flag value for future use.
 *         char *collection - The collection to unload
 *         mdasC_sql_result_struct *myresult - A list of files to be
 *          unloaded.
 *
 * Output - In addition to myresult, returns 0 or positive value
 *          representing the socket fd from which the data will come.
 *
 */

int
srbBulkUnload (srbConn *conn, int catType, int flag,
char *collection, mdasC_sql_result_struct **myresult)
{
    struct sockaddr_in sin;
    int lport = 0;
    char *laddr = NULL;
    int lsock;
    int status;
#ifdef PORTNAME_aix
    size_t length;
#else
    int length;
#endif
    char sb[MAX_TOKEN];
    struct hostent *hostEnt;
    int newSock;
    struct in_addr myIn;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;
    int myCookie;
    int cookie, nbytes;
    struct relayHeader myHeader;
    char *bufptr;
    int toread;
    char *buf;


    if ((lsock = portalCreate ()) < 0) {
        (void) sprintf(conn->errorMessage,
         "srbBulkUnload() -- portalCreate() failed: errno=%d\n%s\n",
          errno, strerror(errno));
        return lsock;
    }

    length = sizeof (sin);
    if (getsockname (lsock, (struct sockaddr *) &sin, &length)) {
        (void) sprintf(conn->errorMessage,
         "srbBulkUnload() -- getsockname() failed: errno=%d\n%s\n",
          errno, strerror(errno));
        return CLI_ERR_SOCKET;
    }
    lport = ntohs (sin.sin_port);
    lport = genPortInp (lport);
    laddr = inet_ntoa (conn->port->laddr.sin_addr);

    listen (lsock, 1);  /* accept just one connect */

    clOut.retval = (char *) &clOutVal;

    argv[0] = catType;
    argv[1] = flag;
    argv[2] = addrToSRBLong (laddr);
    argv[3] = lport;
    argv[4] = addrToSRBLong (collection);

    status = clCall (conn, F_BULK_UNLOAD, argv, &clOut);

    if (status < 0) {
#ifdef _WIN32
    closesocket(lsock);
#else
        close (lsock);
#endif
        return status;
    }

    newSock = accept (lsock, 0, 0);
    if (newSock < 0) {
        (void) sprintf(conn->errorMessage,
         "srbBulkUnload() -- accept() failed: errno=%d\n%s\n",
          errno, strerror(errno));
        return CLI_ERR_SOCKET;
    }
#ifdef _WIN32
    nbytes = recv(newSock,&myCookie,sizeof(myCookie),0);
#else
    nbytes = read (newSock, &myCookie,sizeof (myCookie));
#endif
    cookie = getCookieFromPortInp (lport);
    myCookie = ntohl (myCookie);
    if (nbytes != sizeof (myCookie) || myCookie != cookie) {
        fprintf (stderr,
         "srbBulkUnload: cookie err, bytes read=%d,cookie=%d,inCookie=%d\n",
          nbytes, cookie, myCookie);
#ifdef _WIN32
        closesocket(newSock);
#else
        close (newSock);
#endif
#ifdef _WIN32
        closesocket(lsock);
#else
        close (lsock);
#endif
        return -1;
    }

#ifdef _WIN32
    closesocket(lsock);
#else
    close (lsock);
#endif
    status = getResultForBunload (newSock, myresult);

#ifdef foo      /* done in getResultForBunload */
    status = readHeader (newSock, &myHeader);
    if (status < 0) {
#ifdef _WIN32
        closesocket(newSock);
#else
        close (newSock);
#endif
        return (status);
    }
    toread = myHeader.length;
    buf = (char *) malloc (toread);
    bufptr = buf;
    do {
#ifdef _WIN32
        nbytes = recv(newSock, bufptr, toread, 0);
#else
        nbytes = read (newSock, bufptr, toread);
#endif
        if (nbytes > 0) {
           toread -= nbytes;
           bufptr += nbytes;
        }
    } while (toread > 0 && nbytes > 0);

    if (toread > 0) {
      sprintf (conn->errorMessage,
       "srbBulkUnload: %d bytes read does not equal to header %d\n",
        myHeader.length - toread, myHeader.length);
      status = unixErrConv (errno);
#ifdef _WIN32
        closesocket(newSock);
#else
        close (newSock);
#endif
        free (buf);
        return (status);
   }
    status = unpackMsg (buf, (char **) myresult,
     sqlResultStruct_PF, NULL);

    free (buf);
#endif
    if (status < 0) {
#ifdef _WIN32
        closesocket(newSock);
#else
        close (newSock);
#endif
        return (status);
    }
    return (newSock);
}

int getResultForBunload (int newSock, mdasC_sql_result_struct **myresult)
{
    int status;
    struct relayHeader myHeader;
    char *bufptr;
    int toread;
    int nbytes;
    char *buf;

    status = readHeader (newSock, &myHeader);
    if (status < 0) {
#ifdef _WIN32
        closesocket(newSock);
#else
        close (newSock);
#endif
        return (status);
    }
    toread = myHeader.length;

    if (toread <= 0)
        return (MCAT_INQUIRE_ERROR);

    buf = (char *) malloc (toread);
    bufptr = buf;
    do {
#ifdef _WIN32
        nbytes = recv(newSock, bufptr, toread, 0);
#else
        nbytes = read (newSock, bufptr, toread);
#endif
        if (nbytes > 0) {
           toread -= nbytes;
           bufptr += nbytes;
        }
    } while (toread > 0 && nbytes > 0);

    if (toread > 0) {
      fprintf (stderr,
       "srbBulkUnload: %d bytes read does not equal to header %d\n",
        myHeader.length - toread, myHeader.length);
      status = unixErrConv (errno);
#ifdef _WIN32
        closesocket(newSock);
#else
        close (newSock);
#endif
        free (buf);
        return (status);
   }
    status = unpackMsg (buf, (char **) myresult,
     sqlResultStruct_PF, NULL);

    free (buf);
    return (status);
}

/*
 * _srbBulkUnload - Internal version of Bulk unload of non-container files
 * used by server-server call.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int flag - flag value for future use.
 *         char *clHostAddr - The host address of client portal.
 *         int clPort - The port number of client portal.
 *         char *collection - The collection to unload
 *
 * Output - In addition to myresult, returns 0 or positive value
 *          representing the socket fd from which the data will come.
 *
 */

int
_srbBulkUnload (srbConn *conn, int catType, int flag,
char *clHostAddr, int clPort, char *collection)
{
    int status;
#ifdef PORTNAME_aix
    size_t length;
#else
    int length;
#endif
    struct in_addr myIn;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;


    clOut.retval = (char *) &clOutVal;

    argv[0] = catType;
    argv[1] = flag;
    argv[2] = addrToSRBLong (clHostAddr);
    argv[3] = clPort;
    argv[4] = addrToSRBLong (collection);

    status = clCall (conn, F_BULK_UNLOAD, argv, &clOut);

    return status;
}

/*
 * srbBulkUnloadC - The client initiated socket conection version of 
 * Bulk unload of non-container files in a collection
 * recursively.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int flag - flag value for future use.
 *         char *collection - The collection to unload
 *         mdasC_sql_result_struct *myresult - A list of files to be
 *          unloaded.
 *
 * Output - In addition to myresult, returns 0 or positive value
 *          representing the socket fd from which the data will come.
 *
 */

int
srbBulkUnloadC (srbConn *conn, int catType, int flag,
char *collection, mdasC_sql_result_struct **myresult)
{
    int status;
#ifdef PORTNAME_aix
    size_t length;
#else
    int length;
#endif
    int newSock;
    int nbytes;
    struct relayHeader myHeader;
    char *bufptr;
    int toread;
    char *buf;
    portList_t *myPortList;
    

    status = _srbBulkUnloadC (conn, catType, flag, collection, &myPortList);

    if (status < 0) {
        return status;
    }

    newSock = portalConnect (myPortList->portNum[0], myPortList->hostAddr);
    if (newSock < 0) {
        (void) sprintf(conn->errorMessage,
         "srbBulkUnload() -- accept() failed: errno=%d\n%s\n",
          errno, strerror(errno));
        return CLI_ERR_SOCKET;
    }

    status = readHeader (newSock, &myHeader);
    if (status < 0) {
#ifdef _WIN32
        closesocket(newSock);
#else
        close (newSock);
#endif
        return (status);
    }
    toread = myHeader.length;
    buf = (char *) malloc (toread);
    bufptr = buf;
    do {
#ifdef _WIN32
        nbytes = recv(newSock, bufptr, toread, 0);
#else
        nbytes = read (newSock, bufptr, toread);
#endif
        if (nbytes > 0) {
           toread -= nbytes;
           bufptr += nbytes;
        }
    } while (toread > 0 && nbytes > 0);

    if (toread > 0) {
      sprintf (conn->errorMessage,
       "srbBulkUnload: %d bytes read does not equal to header %d\n",
        myHeader.length - toread, myHeader.length);
      status = unixErrConv (errno);
#ifdef _WIN32
        closesocket(newSock);
#else
        close (newSock);
#endif
        free (buf);
        return (status);
   }
    status = unpackMsg (buf, (char **) myresult,
     sqlResultStruct_PF, NULL);

    free (buf);
    if (status < 0) {
#ifdef _WIN32
        closesocket(newSock);
#else
        close (newSock);
#endif
        return (status);
    }
    return (newSock);
}

int
_srbBulkUnloadC (srbConn *conn, int catType, int flag,
char *collection, portList_t **myPortList)
{
    int status;
#ifdef PORTNAME_aix
    size_t length;
#else
    int length;
#endif
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    char mybuf[20 * MAX_TOKEN];
    char *buf;

    clOut.retval = (char *) mybuf;

    argv[0] = catType;
    argv[1] = flag;
    argv[2] = addrToSRBLong (collection);

    status = clCall (conn, F_BULK_UNLOAD_C, argv, &clOut);

    if (status < 0) {
        return status;
    }

    status = unpackMsg (mybuf, (char **) myPortList, portList_t_PF, NULL);

    if (status < 0) {
        return status;
    }

    return status;
}

/*
 * srbModifyZone - Modify and Insert SRB zone and zone information in the
 *  Metadata Catalog. Information about the operation  performed is 
 *  also logged in the audit  trail (if turned on). This is a
 *  privileged function and should be called only by a 
 *  srbAdmin user.
 *
 *
 * Input - srbConn* conn    - From clConnect ().
 *         int catType      - catalog type. e,g., MDAS_CATALOG.
 *         char *zone_name  - name of the zone
 *         char *dataValue1 - Input value 1.
 *         char *dataValue2 - Input value 2.
 *         char *dataValue3 - Input value 3.
 *         char *dataValue4 - Input value 4.
 *         char *dataValue5 - Input value 5.
 *         int  actionType  - The type of action. performed
 *                  values supported are:
 *
 *                 INSERT_NEW_LOCAL_ZONE
 *                     dv1 = locn_desc
 *                     dv2 = port_number
 *                     dv3 = username@domain   of remote MCAT admin
 *                     dv4 = zone_contact
 *                     dv5 = zone_comment
 *                 INSERT_NEW_ALIEN_ZONE
 *                     dv1-5 = same as for INSERT_NEW_LOCAL_ZONE
 *                 MODIFY_ZONE_INFO
 *                     dv1-5 = same as for INSERT_NEW_LOCAL_ZONE
 *                           empty string implies no change.
 *                 MODIFY_ZONE_FOR_USER
 *                     dv1 = user_name
 *                     dv2 = domain_name
 *                 CHANGE_ZONE_NAME
 *                     dv1 = new name
 *                 MODIFY_ZONE_LOCAL_FLAG
 *                     dv1 = new value (integer)
 *                 MODIFY_ZONE_STATUS
 *                     dv1 = new value (integer)
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

int srbModifyZone (srbConn* conn, int catType,
char *zoneName, char *dataValue1, char *dataValue2,
char *dataValue3, char *dataValue4, char *dataValue5, int actionType)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = catType;
    argv[1] = addrToSRBLong (zoneName);
    argv[2] = addrToSRBLong (dataValue1);
    argv[3] = addrToSRBLong (dataValue2);
    argv[4] = addrToSRBLong (dataValue3);
    argv[5] = addrToSRBLong (dataValue4);
    argv[6] = addrToSRBLong (dataValue5);
    argv[7] = actionType;

    retval = clCall (conn, F_MODIFY_ZONE, argv, &clOut);
    return retval;
}




/*
 * srbBulkQueryAnswer -  Get answers for canned queries from MCAT.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type - 0 - MCAT
 *         char *queryInfo - query information all info needed to
 *               perform the query is in this string. The semantics for
 *               the string depends upon the operation.
 *         int rowsWanted - number of rows of result wanted.
 *
 * Output:
 *         A mdasC_sql_result_struct *myresult
 *
 * Return value :
 *      0 - success; myresult->continuation_index >= 0, ==> more results from
 *          the query. Use srbGetMoreRows() to retrieve more rows.
 *      a negative value - failure
 */
extern int
srbBulkQueryAnswer (srbConn *conn, int catType, char *queryInfo,
mdasC_sql_result_struct *myresult, int rowsWanted)

{
    char *buf;
    int buflen;
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;
 
    argv[0] = catType;
    argv[1] = addrToSRBLong (queryInfo);
    argv[2] = rowsWanted;

    if ((buflen = getSqlResultLen (MAX_DCS_NUM, rowsWanted)) <= 0)
        return CLI_ERR_MALLOC;

    buf = malloc (buflen);
    if (buf == NULL)
        return CLI_ERR_MALLOC;

    clOut.retval = (char *) buf;

    retval = clCall (conn, F_BULK_QUERY_ANSWER, argv, &clOut);

    if (retval < 0 || rowsWanted == 0 ) {
        free (buf);
        return retval;
    }

    strToSqlResult (buf, myresult);

    free (buf);

    if (myresult->result_count > 0)
        return 0;       /* success */
    else
        return CLI_ERR_RETURN_LEN;
}

/*
 * srbBulkMcatIngest - Bulk ingestion of a set of metadata in to the 
 * SRB-MCAT system. 
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type - 0 - MCAT
 *         char *ingestInfo - information about what is done. All info 
 *               needed to perform the ingestion is in this string. The 
 *               semantics for the string depends upon the operation.
 *         mdasC_sql_result_struct  *myresult - The mdasC_sql_result_struct
 *         that contains the objects to be ingested.
 *
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */
 
int
srbBulkMcatIngest (srbConn *conn, int catType, char *ingestInfo, 
mdasC_sql_result_struct *myresult) 
{
    int retval;
    srbResult *res;
    clArgBlock argv[3];
    struct varlena *packedResult;
    byteStream myBS;
    int resultLen;


    retval = packMsg ((char *) myresult, &packedResult, sqlResultStruct_PF,
     NULL);

    if (retval < 0) {
        fprintf (stderr, "srbBulkMcatIngest: packMsg error, status = %d\n", retval);
	return retval;
    }

    myBS.len = VARSIZE (packedResult) - VARHDRSZ;
    myBS.bytes = VARDATA(packedResult);

    argv[0].isint = 1;
    argv[0].len = 4;
    argv[0].u.integer = catType;

    argv[1].isint = 0;
    if (ingestInfo == NULL)
        ingestInfo = "";
    argv[1].len = strlen (ingestInfo) + 1;
    argv[1].u.ptr = ingestInfo;

    argv[2].isint = 0;
    argv[2].len = myBS.len;
    argv[2].u.ptr = myBS.bytes;

    res = clFunct (conn, F_BULK_MCAT_INGEST, (char *) &retval, &resultLen, 1, 
     argv, 3);

    free (packedResult);
    if (clResultStatus(res) != CLI_COMMAND_OK) {
        retval = clResultStatus(res);
    }
    clClear(res);
    return retval;
}

int
qvalToGenQuery (char qval[][MAX_TOKEN], int *selval, genQuery_t *myGenQuery)
{
    int *selInx;
    int *selVal;
    int *qvalInx;
    int i;
    char *tmpPtr;
    int selInxCnt = 0;
    int qvalCnt = 0;


    selInx = (int *) malloc (MAX_DCS_NUM * sizeof (int));
    selVal = (int *) malloc (MAX_DCS_NUM * sizeof (int));
    qvalInx = (int *) malloc (MAX_DCS_NUM * sizeof (int));

    memset (myGenQuery, 0, sizeof (genQuery_t));
    memset (selInx, 0, sizeof (MAX_DCS_NUM * sizeof (int)));
    memset (selVal, 0, sizeof (MAX_DCS_NUM * sizeof (int)));
    memset (qvalInx, 0, sizeof (MAX_DCS_NUM * sizeof (int)));


    myGenQuery->selInx = selInx;
    myGenQuery->selVal = selVal;
    myGenQuery->qvalInx = qvalInx;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        if (selval[i] != 0) {
            myGenQuery->selInx[selInxCnt] = i;
            myGenQuery->selVal[selInxCnt] = selval[i];
            selInxCnt++;
        }

        if (strlen (qval[i]) > 0) {
            myGenQuery->qvalInx[qvalCnt] = i;
            qvalCnt++;
        }
    }

    if (qvalCnt > 0) {
        myGenQuery->qval = malloc (qvalCnt * MAX_TOKEN);
        memset (myGenQuery->qval, 0, qvalCnt * MAX_TOKEN);
    } else {
        myGenQuery->qval = malloc (4);
    }

    tmpPtr = (char *) myGenQuery->qval;
    for (i = 0; i < qvalCnt; i++) {
        strcpy (tmpPtr, qval[myGenQuery->qvalInx[i]]);
        tmpPtr += MAX_TOKEN;
    }
    myGenQuery->selInxCnt = selInxCnt;
    myGenQuery->qvalCnt = qvalCnt;

    return (0);
}

int
clearGenQuery (genQuery_t *myGenQuery) 
{
    if (myGenQuery->selInx != NULL && myGenQuery->selInxCnt > 0)
        free (myGenQuery->selInx);
    if (myGenQuery->selVal != NULL && myGenQuery->selInxCnt > 0)
        free (myGenQuery->selVal);
    if (myGenQuery->qvalInx != NULL && myGenQuery->qvalCnt > 0)
        free (myGenQuery->qvalInx);
    if (myGenQuery->qval != NULL && myGenQuery->qvalCnt > 0)
        free (myGenQuery->qval);

    return (0);
}

/*
 * srbObjChksum - Checksum a SRB data file. By default, if the chksum
 * already already exists, do nothing and return the cjksum value.
 * If the chksum does not exist, compute and reigister it.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *objID - The data name.
 *         char *collectionName - The collection name.
 *         int chksumFlag -  valid flags are :
 *	     l_FLAG - list the registered chksum value.
 *	     c_FLAG - compute chksum, but don't register
 *	     f_FLAG - force compute and register of chksum even if one
 *	      already exist.
 *	   char *inpChksum - No used.
 *
 * Output - 0 - success, CHKSUM_REG - a new chksum has been registered,
 * Returns a negative value upon failure.
 * the CALLER must have allocated enough space in char * chksum to hold 
 * the result returned
 */

int
srbObjChksum (srbConn *conn, char *objID, char *collectionName, int chksumFlag,
char *inpChksum, char *chksum)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) chksum;

    argv[0] = addrToSRBLong (objID);
    argv[1] = addrToSRBLong (collectionName);
    argv[2] = chksumFlag;
    argv[3] = addrToSRBLong (inpChksum);

    retval = clCall (conn, F_SRBO_CHKSUM, argv, &clOut);
    return retval;
}

/*
 * srbModifyUserNonPriv - Modify and Insert SRB user information in the
 *  Metadata Catalog. Information about the operation  performed is 
 *  also logged in the audit  trail (if turned on). This is a 
 *  non-privileged function.
 *
 * Input - srbConn* conn    - From clConnect ().
 *         int catType      - catalog type. e,g., MDAS_CATALOG.
 *         char *userNameDomain  - name@domain of the user
 *         char *dataValue1 - Input value 1.
 *         char *dataValue2 - Input value 2.
 *         char *dataValue3 - Input value 3.
 *         char *dataValue4 - Input value 4.
 *         char *dataValue5 - Input value 5.
 *         int  actionType  - The type of action. performed
 *                  values supported are:
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

int srbModifyUserNonPriv (srbConn* conn, int catType,
char *userNameDomain, char *dataValue1, char *dataValue2,
char *dataValue3, char *dataValue4, char *dataValue5, int actionType)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = catType;
    argv[1] = addrToSRBLong (userNameDomain);
    argv[2] = addrToSRBLong (dataValue1);
    argv[3] = addrToSRBLong (dataValue2);
    argv[4] = addrToSRBLong (dataValue3);
    argv[5] = addrToSRBLong (dataValue4);
    argv[6] = addrToSRBLong (dataValue5);
    argv[7] = actionType;

    retval = clCall (conn, F_MODIFY_USER_NP, argv, &clOut);
    return retval;
}





/*
 * srbModifyResource - Modify and Insert SRB resource information in the
 *  Metadata Catalog. Information about the operation  performed is 
 *  also logged in the audit  trail (if turned on). This is a
 *  privileged function and should be called only by a 
 *  srbAdmin user.
 *
 *
 * Input - srbConn* conn    - From clConnect ().
 *         int catType      - catalog type. e,g., MDAS_CATALOG.
 *         char *resource_name  - name of the resource
 *         char *dataValue1 - Input value 1.
 *         char *dataValue2 - Input value 2.
 *         char *dataValue3 - Input value 3.
 *         char *dataValue4 - Input value 4.
 *         int  actionType  - The type of action. performed
 *                  values supported are:
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

int srbModifyResource (srbConn* conn, int catType,
char *resourceName, char *dataValue1, char *dataValue2,
char *dataValue3, char *dataValue4,  int actionType)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = catType;
    argv[1] = addrToSRBLong (resourceName);
    argv[2] = addrToSRBLong (dataValue1);
    argv[3] = addrToSRBLong (dataValue2);
    argv[4] = addrToSRBLong (dataValue3);
    argv[5] = addrToSRBLong (dataValue4);
    argv[6] = actionType;

    retval = clCall (conn, F_MODIFY_RESOURCE, argv, &clOut);
    return retval;
}

/*
 * srbGetFsFreeSpace - get the FS free space in bytes of the FS where the
 * input path is a file or directory.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int storSysType - Storage system type.
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP, 4 = HTTP.
 *         char *hostAddr - The Host address of the SRB.
 *         char *path - A file or directory path in the FS
 *         int flag - not used
 *
 * Output - Returns the free space in bytes upon success.
 *          Returns a negative value upon failure.
 *
 */

srb_long_t
srbGetFsFreeSpace (srbConn *conn, int storSysType, char *hostAddr, char *path,
int flag)

{
    clArgBlock argv[4];
    srbResult *res;
    int resultLen;
    srb_long_t retval;

    argv[0].isint = 1;
    argv[0].len = 4;
    argv[0].u.integer = storSysType;

    argv[1].isint = 0;
    if (hostAddr == NULL) {
        argv[1].len = 1;
        argv[1].u.ptr = "";
    } else {
        argv[1].len = strlen (hostAddr) + 1;
        argv[1].u.ptr = hostAddr;
    }

    argv[2].isint = 0;
    if (path == NULL) {
        argv[2].len = 1;
        argv[2].u.ptr = "";
    } else {
        argv[2].len = strlen (path) + 1;
        argv[2].u.ptr = path;
    }

    argv[3].isint = 1;
    argv[3].len = 4;
    argv[3].u.integer = flag;

    res = clFunct(conn, F_E_GET_FS_FREESPACE,(char *) &retval,&resultLen,1,
     argv,4);
    if (clResultStatus(res) == CLI_COMMAND_OK) {
        clClear(res);
        return retval;
    } else {
        retval = clResultStatus(res);
        clClear(res);
        return retval;
    }
}

/*
 * srbBulkMove - Bulk physical move
 *
 * Input - srbConn* conn    - From clConnect ().
 *         int catType      - catalog type. e,g., MDAS_CATALOG.
 *         char *collectionName  - The collection
 *	   int flag - not used
 *         char *srcResource - the source resources, multiple resources can be 
 *	     specified with resource1|resource2|...If srcResource is specified,
 *	     only files stored in these resources will be moved. 
 *	   char *destResource - The target resource to move to.
 *	   char *containerName - The container to move to. If containerName
 *	     is specified, the destResource will be ignored. 
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

int srbBulkMove (srbConn* conn, int catType, char *collectionName, int flag,
char *srcResource, char *destResource, char *containerName)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = catType;
    argv[1] = addrToSRBLong (collectionName);
    argv[2] = flag;
    argv[3] = addrToSRBLong (srcResource);
    argv[4] = addrToSRBLong (destResource);
    argv[5] = addrToSRBLong (containerName);

    retval = clCall (conn, F_BULK_MOVE, argv, &clOut);
    return retval;
}


/*
 * srbModifyForBulkMove - Used for Third-party Physical Copying/Moving of Files in Bulk
 *                       to modify the MCAT information
 *                       this call is to be used in conjunction with the physical move
 *                       call which makes the actual move
 * Input - srbConn* conn - From srbConnect ().
 *         catType - catalog type. e,g., MDAS_CATALOG.
 *         operCode - code for the operation to be performed
 *                  supported code:
 *                                    BULK_PHY_MOVE
 *                                    BULK_PHY_MOVE_INTO_CONTAINER
 *         operString - Additional Information useful for performing the operation
 *                  for BULK_PHY_MOVE   this holds  the ResourceName
 *                  for BULK_PHY_MOVE_INTO_CONTAINER   this holds  the ContainerName
 *         inDataInfo  mdasC_sql_result_struct  that holds information to be modified
 *                  for BULK_PHY_MOVE information required are:
 *                          dataName,collName,replNum,versionString
 *                          and the pathName  in the given ResourceName
 *                  for BULK_PHY_MOVE_INTO_CONTAINER information required are:
 *                          dataName,collName,replNum,versionString
 *                          and dataSize,offSet  in the given ContainerName
 *
 * Output - Returns 0
 *          Returns a negative value upon failure.
 *
 * the caller has to clean up the mdasC_sql_result_struct
 */

int srbModifyForBulkMove( srbConn *conn, int catType, 
			  int operCode,  char *operString,
			  mdasC_sql_result_struct *inDataInfo)
{
  int retval;
  srbResult *res;
  clArgBlock argv[3];
  struct varlena *packedResult;
  byteStream myBS;
  int resultLen;


  retval = packMsg ((char *) inDataInfo, &packedResult, sqlResultStruct_PF,
		    NULL);
  if (retval < 0) {
    fprintf (stderr, "packMsg error, status = %d\n", retval);
    return retval;
  }
  myBS.len = VARSIZE (packedResult) - VARHDRSZ;
  myBS.bytes = VARDATA(packedResult);

  argv[0].isint = 1;
  argv[0].len = 4;
  argv[0].u.integer = catType;
    
  argv[1].isint = 1;
  argv[1].len = 4;
  argv[1].u.integer = operCode;

  argv[2].isint = 0;
  argv[2].len = strlen (operString) + 1;
  argv[2].u.ptr = operString;

  argv[3].isint = 0;
  argv[3].len = myBS.len;
  argv[3].u.ptr = myBS.bytes;

  res = clFunct (conn, F_MODIFY_FOR_BULK_MOVE, (char *) &retval, &resultLen, 1,
		 argv, 4);
  free (packedResult);
  if (clResultStatus(res) != CLI_COMMAND_OK) {
    retval = clResultStatus(res);
  }
  clClear(res);
  return retval;
}

/*
 * srbBulkUnloadForMove - Bulk unload of non-container files in a collection
 * recursively for phymove.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int flag - flag value for future use.
 *         char *collection - The collection to unload
 *         char *srcResource - The source resource to unload
 *         char *targResource - The target resource to unload
 *         mdasC_sql_result_struct *myresult - A list of files to be
 *          unloaded.
 *
 * Output - In addition to myresult, returns 0 or positive value
 *          representing the socket fd from which the data will come.
 *
 */

int
srbBulkUnloadForMove (srbConn *conn, int catType, int flag,
char *collection, char *srcResource, char *targResource,
mdasC_sql_result_struct **myresult)
{
    struct sockaddr_in sin;
    int lport = 0;
    char *laddr = NULL;
    int lsock;
    int status;
#ifdef PORTNAME_aix
    size_t length;
#else
    int length;
#endif
    char sb[MAX_TOKEN];
    struct hostent *hostEnt;
    int newSock;
    struct in_addr myIn;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;
    int myCookie;
    int cookie, nbytes;
    struct relayHeader myHeader;
    char *bufptr;
    int toread;
    char *buf;


    if ((lsock = portalCreate ()) < 0) {
        (void) sprintf(conn->errorMessage,
         "srbBulkUnloadForMove() -- portalCreate() failed: errno=%d\n%s\n",
          errno, strerror(errno));
        return lsock;
    }

    length = sizeof (sin);
    if (getsockname (lsock, (struct sockaddr *) &sin, &length)) {
        (void) sprintf(conn->errorMessage,
         "srbBulkUnloadForMove() -- getsockname() failed: errno=%d\n%s\n",
          errno, strerror(errno));
        return CLI_ERR_SOCKET;
    }
    lport = ntohs (sin.sin_port);
    lport = genPortInp (lport);
    laddr = inet_ntoa (conn->port->laddr.sin_addr);

    listen (lsock, 1);  /* accept just one connect */

    clOut.retval = (char *) &clOutVal;

    argv[0] = catType;
    argv[1] = flag;
    argv[2] = addrToSRBLong (laddr);
    argv[3] = lport;
    argv[4] = addrToSRBLong (collection);
    argv[5] = addrToSRBLong (srcResource);
    argv[6] = addrToSRBLong (targResource);

    status = clCall (conn, F_BULK_UNLOAD_FOR_MOVE, argv, &clOut);

    if (status < 0) {
#ifdef _WIN32
    closesocket(lsock);
#else
        close (lsock);
#endif
        return status;
    }

    newSock = accept (lsock, 0, 0);
    if (newSock < 0) {
        (void) sprintf(conn->errorMessage,
         "srbBulkUnloadForMove() -- accept() failed: errno=%d\n%s\n",
          errno, strerror(errno));
        return CLI_ERR_SOCKET;
    }
#ifdef _WIN32
    nbytes = recv(newSock,&myCookie,sizeof(myCookie),0);
#else
    nbytes = read (newSock, &myCookie,sizeof (myCookie));
#endif
    cookie = getCookieFromPortInp (lport);
    myCookie = ntohl (myCookie);
    if (nbytes != sizeof (myCookie) || myCookie != cookie) {
        fprintf (stderr,
         "srbBulkUnloadForMove: cookie err, bytes read=%d,cookie=%d,inCookie=%d\n",
          nbytes, cookie, myCookie);
#ifdef _WIN32
        closesocket(newSock);
#else
        close (newSock);
#endif
#ifdef _WIN32
        closesocket(lsock);
#else
        close (lsock);
#endif
        return -1;
    }

#ifdef _WIN32
    closesocket(lsock);
#else
    close (lsock);
#endif
    status = getResultForBunload (newSock, myresult);

#ifdef foo      /* done in getResultForBunload */
    status = readHeader (newSock, &myHeader);
    if (status < 0) {
#ifdef _WIN32
        closesocket(newSock);
#else
        close (newSock);
#endif
        return (status);
    }
    toread = myHeader.length;
    buf = (char *) malloc (toread);
    bufptr = buf;
    do {
#ifdef _WIN32
        nbytes = recv(newSock, bufptr, toread, 0);
#else
        nbytes = read (newSock, bufptr, toread);
#endif
        if (nbytes > 0) {
           toread -= nbytes;
           bufptr += nbytes;
        }
    } while (toread > 0 && nbytes > 0);

    if (toread > 0) {
      sprintf (conn->errorMessage,
       "srbBulkUnloadForMove: %d bytes read does not equal to header %d\n",
        myHeader.length - toread, myHeader.length);
      status = unixErrConv (errno);
#ifdef _WIN32
        closesocket(newSock);
#else
        close (newSock);
#endif
        free (buf);
        return (status);
   }
    status = unpackMsg (buf, (char **) myresult,
     sqlResultStruct_PF, NULL);

    free (buf);
#endif
    if (status < 0) {
#ifdef _WIN32
        closesocket(newSock);
#else
        close (newSock);
#endif
        return (status);
    }
    return (newSock);
}

/*
 * srbRegPhyMove - Register a phymove of a SRB dataset
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *         char *objID - The SRB object ID to create. The objID is obtained
 *              through registration with MDAS.
 *         char *collectionName - The collection name.
 *         char *curResourceName - the resource of the current copy to be moved
 *         char *curDataPath - The phy path of the current copy 
 *         char *newResourceName - The resource of the new copy.
 *         char * newFullPathName- The phy path of the new copy.
 *
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

int
srbRegPhyMove (srbConn* conn, int catType, char *objID, 
char *collectionName, char *curResourceName, char *curDataPath,
char *newResourceName, char *newFullPathName)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = catType;
    argv[1] = addrToSRBLong (objID);
    argv[2] = addrToSRBLong (collectionName);
    argv[3] = addrToSRBLong (curResourceName);
    argv[4] = addrToSRBLong (curDataPath);
    argv[5] = addrToSRBLong (newResourceName);
    argv[6] = addrToSRBLong (newFullPathName);

    retval = clCall (conn, F_REG_PHY_MOVE, argv, &clOut);
    return retval;
}

/*
 * srbBulkCopy - Bulk copy
 *
 * Input - srbConn* conn    - From clConnect ().
 *         int catType      - catalog type. e,g., MDAS_CATALOG.
 *         char *collectionName  - The collection
 *         int flag - not used
 *         char *destResource - The target resource to move to.
 *         char *destCollection - The destination collection
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

int srbBulkCopy (srbConn* conn, int catType, char *collectionName, int flag,
char *destResource, char *destCollection)
{
    int retval;
    srb_long_t argv[MAXFMGRARGS];
    ClOut clOut;
    int clOutVal;

    clOut.retval = (char *) &clOutVal;

    argv[0] = catType;
    argv[1] = addrToSRBLong (collectionName);
    argv[2] = flag;
    argv[3] = addrToSRBLong (destResource);
    argv[4] = addrToSRBLong (destCollection);

    retval = clCall (conn, F_BULK_COPY, argv, &clOut);
    return retval;
}

