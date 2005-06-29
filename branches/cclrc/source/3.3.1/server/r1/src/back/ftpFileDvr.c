/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 

/*
 * ftpFileDvr.c - Routines to handle FTP type file storage
 */

#include "ftpFileDvr.h" 

#if defined(PORTNAME_linux)
#include <sys/time.h>
#endif 
/* PORTNAME_linux */

/* ftpOpen - Handles the open call.
 *
 * Input : char *fileName - The file path name to be opened
 *         int fileFlags - The open flag
 *         int fileMode - The file mode
 *
 * Output : Returns the file descriptor of the opened file.
 */

int
ftpOpen(char *fileName, int fileFlags, int fileMode)
{
    _ftpAutoConnect();
    if (strlen(fileName)==0) return(_ftpOpenDir(""));
    if (fileFlags == O_RDONLY) return(_ftpOpenForGet(fileName));
    if (fileFlags == O_WRONLY) return(_ftpOpenForPut(fileName));
    elog(NOTICE,"For FTP files, you must open a file for either \
write-only (O_WRONLY) or for read-only (O_RDONLY)");
    return(FTP_ERR_OPEN_FLAGS);
}

/* ftpCreate - Handles the create call.
 *
 * Input : char *fileName - The file path name to be opened
 *         int fileMode - The file mode
 *
 * Output : Returns the file descriptor of the new file.
 */

int
ftpCreate(char *fileName, int fileMode)
{
    _ftpAutoConnect();
    return(_ftpOpenForPut(fileName));
}

/* ftpClose - Handles the close call.
 *
 * Input : int file - The file descriptor to be closed
 *
 * Output : Return status of close
 */

int
ftpClose(int file)
{
    int xfersock;
    int funcVal;

    xfersock = file;

#ifdef _WIN32
    funcVal = closesocket(xfersock);
#else
    funcVal  = close(xfersock);            /* close the socket */
#endif

    if (funcVal < 0) {
        return 0;            /* assume that the error is because it
                                is already closed which we'll assume is
                                because it was a Read which is closed when
                                it completes.  So just return success. 
                                (closes on ftpReads is a noop). */
    }

/*
  Continue with additional processing to close as Write connection.
 */

    funcVal=_ftpReceive();
    if (funcVal != 226) {
        elog(NOTICE,"Error response from FTP Server after closing socket: %s",
             ftpRcvCmdBuf);
         return FTP_ERR_RETURN_VAL; 
    }

    return 0;
}

/* ftpRead - Handles the read call.
 *
 * Input : int file - The file descriptor to read
 *	   char *buffer - The input buffer
 *	   int amount - The amount to read
 *
 * Output : Returns to number of bytes read
 */

int
ftpRead(int file, char *buffer, int amount)
{
    int returnCode, funcVal;
    returnCode = _ftpRead(file, buffer, amount);
    if (returnCode == 0) {   /* transfer completed */
        funcVal=_ftpReceive();
        if (funcVal != 226) {
            elog(NOTICE,"Error response from FTP Server after reading data:%s",
                 ftpRcvCmdBuf);
	    return FTP_ERR_RETURN_VAL; 
	}
#ifdef _WIN32
    closesocket(file);
#else
        close(file);
#endif
    }
    return returnCode;
}

/* ftpWrite - Handles the write call.
 *
 * Input : int file - The file descriptor to write
 *         char *buffer - The output buffer
 *         int amount - The amount to write
 *
 * Output : Returns to number of bytes written
 */

int
ftpWrite(int file, char *buffer, int amount)
{
    int	returnCode;
    returnCode = _ftpWrite(file,buffer, amount);
    return returnCode;
}

/* ftpSeek - Handles the seek call.
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

srb_long_t
ftpSeek(int file, long offset, int whence)
{
    elog(NOTICE,"The seek function is not available for FTP files.\n");
    return INP_ERR_NO_SUPPORT;
}

/* ftpSync - Handles the sync call.
 *
 * Input : int file - The file descriptor to sync
 *
 * Output : Returns the status of sync
 */

ftpSync(int file)
{
    int	returnCode;
    elog(NOTICE,"The sync function is not available for FTP files.\n");
    return INP_ERR_NO_SUPPORT;
}

/* ftpUnlink - Handles the unlink call.
 *
 * Input : char *filename - The file path name to unlink
 *
 * Output : Returns the status of unlink
 */

int
ftpUnlink(char *filename)
{
    int funcVal;
    int status;

    if ((status = _ftpSend("DELE %s\n",filename)) < 0) {
        elog(NOTICE,"Error sending DELE command");
        return status;
    }
    funcVal=_ftpReceive();

    if (funcVal != 250) {
        elog(NOTICE,"Error response from FTP Server after DELE command: %s",
             ftpRcvCmdBuf);
        return FTP_ERR_RETURN_VAL;
    }
    return 0;
}
 
/* ftpStat - Handles the stat call.
 *
 * Input : char *filename - The file path name to stat
 *	   struct stat *statbuf - The place to put the stat output
 *         (made to be just char * since was a problem on the Cray,
 *          it is not really used anyway)
 *
 * Output : Returns the status of stat
 */

int
ftpStat(char *filename, char *statbuf)
{
    elog(NOTICE,"The Stat function is not available for FTP files.\n");
    return INP_ERR_NO_SUPPORT;
}

/* ftpMkdir - Handles the mkdir call.
 *
 * Input : char *filename - The directory path name to mkdir
 *	   int mode - The mode of the directory (unused)
 *
 * Output : Returns the status of mkdir
 */

int
ftpMkdir(char *filename, int mode)
{
    int funcVal;
    int status;

    _ftpAutoConnect();
    if ((status = _ftpSend("MKD %s\n",filename)) < 0) {
        elog(NOTICE,"Error sending MKD command");
        return status;
    }
    funcVal=_ftpReceive();

    if (funcVal != 257) {
        elog(NOTICE,"Error response from FTP Server after MKD command: %s",
             ftpRcvCmdBuf);
        return FTP_ERR_RETURN_VAL;
    }
    return 0;
}

/* ftpChmod - Handles the chmod call.
 *
 * Input : char *filename - The path name to chmod
 *         int mode - The mode of the directory
 *
 * Output : Returns the status of chmod
 */

int
ftpChmod (char *filename, int mode)
{
    elog(NOTICE,"The chmod function is not available for FTP files.\n");
    return INP_ERR_NO_SUPPORT;
}

/* ftpRmdir - Handles the rmdir call.
 *
 * Input : char *filename - The path name to rmdir
 *
 * Output : Returns the status of rmdir
 */

int
ftpRmdir (char *filename)
{
    int funcVal;
    int status;

    if ((status = _ftpSend("RMD %s\n",filename)) < 0) {
        elog(NOTICE,"Error sending RMD command");
        return status;
    }
    funcVal=_ftpReceive();

    if (funcVal != 250) {
        elog(NOTICE,"Error response from FTP Server after RMD command: %s",
             ftpRcvCmdBuf);
        return FTP_ERR_RETURN_VAL;
    }
    return 0;
}
 
/* ftpOpendir - Handles the opendir call.
 *
 * Input : char *dirname - The path name to Opendir
 *	   **sysDepDirP - The address to store the opened *DIR.
 *
 * Output : Returns the status of Opendir - 0 - OK, negative - error.
 */
 
int
ftpOpendir (char *dirname, void **sysDepDirP)
{
    elog(NOTICE,"opendir function is not available for FTP files.\n");
    return INP_ERR_NO_SUPPORT; 

}

/* ftpClosedir - Handles the closedir call.
 *
 * Input : *sysDepDir - The *DIR to close.
 *
 * Output : Returns the status of closedir
 */
 
int
ftpClosedir (void *sysDepDir)
{
    elog(NOTICE,"closedir function is not available for FTP files.\n");
    return INP_ERR_NO_SUPPORT;
}

/* ftpReaddir - Handles the readdir call.
 *
 * Input : *sysDepDir - The *DIR to read.
 *	   struct srbDirent *srbDirent - Address of the output srbDirent.
 *
 * Output : Returns the status of readdir
 */
 
int
ftpReaddir (void *sysDepDir, struct srbDirent *srbDirent)
{
    elog(NOTICE,"readdir function is not available for FTP files.\n");
    return (INP_ERR_NO_SUPPORT);
}

/* elog_test - test version of elog, not used now, 
 *
 * Input : int lev: level of error
 *	   char *fmt: format string
 *	   char *arg1: arg1, first input argument
 *
 * Output : none
 *
 *  Real elog will be used in library,
 *  The real elog uses s variable number of arguments.
 *
 */
int
elog_test(lev, fmt, arg1)
int lev;
char *fmt;
char *arg1;
{
   char buf[2000];
   sprintf(buf,fmt,arg1);
   printf("%s \n",buf);
   return 0;
}

/* _ftpAutoConnect() - Automatically connect to an FTP Server if the
 *  user hasn't done so already (via an ftpSetStorAttri call).  This is
 *  just attempted once.
 *
 * Input : none, uses ftpCmdSock global variable
 * Output : none
 */
int
_ftpAutoConnect()
{
    static int first=1; 
    if (first!=1) return 0;
    if (ftpCmdSock!=0) return 0;
    ftpSetStorAttri("ftp.sdsc.edu","anonymous:schroede@sdsc.edu", "");
    first = 0;
    return 0;
}

/* _ftpOpenForPut(filename) - Open a file to be "Put" (Stored).
 *
 * Input : char *filename - filename to open
 * Output : Returns the status of the open
 */
int
_ftpOpenForPut(filename)
char *filename;
{
    int len, funcVal;
    char command[CMDLEN];
    int status;

    if ((status = _ftpSend("TYPE I\n")) < 0) 
	return status;

    funcVal=_ftpReceive();
    if (funcVal != 200) return FTP_ERR_RETURN_VAL;

    len = strlen(filename);
    if (len > CMDLEN-50) return FTP_ERR_FNAME_2_LONG;

    sprintf(command,"STOR %s\n",filename);

    return(_ftpSetupDataPort(command));
}

/* _ftpOpenForGet(filename) - Open a file for a "Get" (Retrieve).
 *
 * Input : char *filename - filename to open
 * Output : Returns the status of the open
 */
int
_ftpOpenForGet(filename)
char *filename;
{
    int len, funcVal;
    char command[CMDLEN];
    int status;

    if ((status = _ftpSend("TYPE I\n")) < 0) 
	return status;

    funcVal=_ftpReceive();
    if (funcVal != 200) 
	return FTP_ERR_RETURN_VAL;

    len = strlen(filename);
    if (len > CMDLEN-50) 
	return FTP_ERR_FNAME_2_LONG;

    sprintf(command,"RETR %s\n",filename);

    return (_ftpSetupDataPort(command));
}

/* _ftpSetupDataPort(command) - Set up a Data Port for the given command
 *
 * Input : char *command - the command to be sent as part of the setup
 * Output : Returns the socket number or the status of the 
 *          operation (negative value)
 */
int
_ftpSetupDataPort(command)
char *command;
{
    int funcVal;
#ifdef PORTNAME_aix
    size_t len;
#else
    int len; 
#endif
    int i;
    static int datasock=0;
    static int addrbytes[6];

#if defined(PORTNAME_linux)
	fd_set readfds, writefds, exceptfds;
	struct timeval timeout;
#elif defined(_WIN32)
    fd_set readfds, writefds, exceptfds;
    struct tm timeout;
#else
    int readfds, writefds, exceptfds;
    struct tm timeout;
#endif 
/* PORTNAME_linux */

    int nready;
    int xfersock;

    struct sockaddr_in data_addr;
    char *a;
    char *p;
    unsigned char temp;
    int status;

#if defined(PORTNAME_linux)
/* Clear fd_set for later use */
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&exceptfds);
#endif 
/* PORTNAME_linux */

#ifdef _WIN32
    if ((status = startWinsock()) != 0) {
        elog(NOTICE,"error starting up Winsocket: %m");
        return status;
    }
#endif

    datasock = socket(AF_INET, SOCK_STREAM, 0);      /* Create socket */
    if (datasock < 0) {
        elog(NOTICE,"error opening dataport stream socket: %m");
        return datasock;
    }

    data_addr.sin_family = AF_INET;
    data_addr.sin_addr.s_addr = INADDR_ANY;
    data_addr.sin_port = 0;
    funcVal =  bind(datasock, (struct sockaddr *)&data_addr,  
                    sizeof (data_addr));             /* bind it */
    if (funcVal < 0) {
        elog(NOTICE,"bind error opening dataport: %m");
        return funcVal;
    }

    if ((status = listen(datasock,1)) < 0) {             /* listen on it */
        elog(NOTICE,"listen error opening dataport: %m");
        return status;
    }

    len = sizeof (data_addr);
    if ((status = getsockname(datasock, (struct sockaddr *)&data_addr, 
        &len)) < 0) {                            /* get the port number */
        elog(NOTICE,"getsockname error opening dataport: %m");
        return status;
    }

    /* the IP address of our host is available in the control address struct */
    a = (char *)&ftpCntlAddr.sin_addr;
    for (i=0;i<4;i++) {
        addrbytes[i]= ((int)(*a++))&0xff;
    }

    p = (char *)&data_addr.sin_port;
#ifdef CRAY
    p = p + 6;    /* On Cray, must space forward in the word */
#endif
    addrbytes[4]=((int)(*p++))&0xff;
    addrbytes[5]=((int)(*p++))&0xff;

    if ((status = _ftpSend("PORT %d,%d,%d,%d,%d,%d\n",  /* send PORT command */
          addrbytes[0], addrbytes[1], addrbytes[2], addrbytes[3],
          addrbytes[4], addrbytes[5])) < 0) {
        elog(NOTICE,"_ftpSend error opening dataport");
        return status;
    }
    funcVal=_ftpReceive();                          /* get the response */
    if (funcVal != 200) {
        elog(NOTICE,"Error response from FTP server while opening Dataport:%s",
             ftpRcvCmdBuf);
#ifdef _WIN32
    closesocket(datasock);
#else
	close(datasock);
#endif
        return FTP_ERR_RETURN_VAL;
    }

    if ((status = _ftpSend(command)) < 0) {
        elog(NOTICE,"_ftpSend error opening dataport sending: %s",command);
        return status;
    }
 
    funcVal=_ftpReceive();                         /* get the response */
    if (funcVal != 150) 
    {
        elog(NOTICE,"Error response from FTP server while opening dataport:%s",
             ftpRcvCmdBuf);
#ifdef _WIN32
    closesocket(datasock);
#else
	close(datasock);
#endif
        return FTP_ERR_RETURN_VAL;
    }

#ifdef _WIN32
    memset((char *)&timeout,0,sizeof(timeout));
#else
    bzero((char *)&timeout,sizeof(timeout));
#endif

#if defined(PORTNAME_linux)
	timeout.tv_sec = 4;
	timeout.tv_usec = 0;
	FD_SET((1<<datasock), &readfds);
#elif defined(_WIN32)
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);
    FD_ZERO(&readfds);
    FD_SET(datasock,&readfds);
    timeout.tm_sec = 4;
#else
    timeout.tm_sec = 4;   
    readfds = 0 | (1<<datasock);
#endif

    nready = select(datasock+1, &readfds, &writefds,
             &exceptfds, &timeout);         /* wait for a connect to come in */
    if (nready<0) {
        elog(NOTICE,"Response timeout opening dataport");
        return nready;
    }
#if defined(PORTNAME_linux)
	if (!FD_ISSET((1<<datasock), &readfds)) 
#elif defined(_WIN32)
    if(WSAGetLastError() == WSAEINTR)
#else   
    if (readfds==0) 
#endif 
/* PORTNAME_linux */
    {
        elog(NOTICE,"Response timeout opening dataport (no data)");
        return FTP_ERR_RESP_TIMEOUT;
    }

    xfersock = accept(datasock,0,0);              /* accept the connection */
    if ((status = xfersock) < 0) {
        xfersock = 0;
        elog(NOTICE, "accept error opening dataport: %m");
        return status;
    }

#ifdef _WIN32
    closesocket(datasock);
#else
    close(datasock);
#endif

    return xfersock;
}

/* ftpDir(filename,buf,buflen) - do a directory listing; this was
 * used during testing, but is not currently  called.
 *
 * Input : char *filename - null (for directory) or filename to list
 *         int buflen - the maximum number of characters to read.
 * Output : char *buf - contains the text from the LIST operation.
 *          Returns the status of the operation (negative) or the length
 *          of data in the buffer.
 */
ftpDir(filename,buf,buflen)
char *filename;
char *buf;
int buflen;
{
    int funcVal, len, readVal;
    int xfersock;
    char command[CMDLEN];

    _ftpAutoConnect();

    _ftpFlushReceive();  /* get any pending responses */

    len = strlen(filename);
    if (len > CMDLEN-50) {
        elog(NOTICE,"command exceeded allocated space");
        return FTP_ERR_FNAME_2_LONG;
    }

    if (len==0) 
        sprintf(command,"LIST\n");
    else
        sprintf(command,"LIST %s\n",filename);

    funcVal = _ftpSetupDataPort(command);
    if (funcVal < 0 ) return funcVal;
    
    xfersock = funcVal;
    readVal = _ftpRead(xfersock,buf,buflen);

    funcVal=_ftpReceive();                         /* get the response */

    if (funcVal != 226) {
        elog(NOTICE,"Error response from FTP Server after LIST command: %s",
             ftpRcvCmdBuf);
        return FTP_ERR_RETURN_VAL;
    }
#ifdef _WIN32
    closesocket(xfersock);
#else
    close(xfersock);
#endif
    return readVal;
}

/* _ftpOpenDir(dirname) - open a directory for reading (LISTing).
 *
 * Input : char *dirname - null (for directory) or filename to list
 * Output : Returns the status of the operation (negative) or the fd which
 *          is set for reading
 */
_ftpOpenDir(dirname)
char *dirname;
{
    int funcVal, len, readVal;
    char command[CMDLEN];

    _ftpFlushReceive();  /* get any pending responses */

    if (ftpCmdSock == 0) {
        elog(NOTICE,"Not connected to an FTP Server, must call SetStorAttri");
        return FTP_ERR_NOT_CONNECTED;
    }

    len = strlen(dirname);
    if (len > CMDLEN-50) {
        elog(NOTICE,"command exceeded allocated space");
        return FTP_ERR_FNAME_2_LONG;
    }

    if (len==0) 
        sprintf(command,"LIST\n");
    else
        sprintf(command,"LIST %s\n",dirname);

    funcVal = _ftpSetupDataPort(command);
    return funcVal;
}

/* _ftpWrite(xfersock,buf,len) - write data on a socket.
 *
 * Input : int xfersock - socket(fd) to write on
 *         char *buf    - buffer to write
 *         len          - length, in bytes, to write.
 * Output : Returns the status of the operation (negative) or the number
 *          of bytes written.
 */
int
_ftpWrite(xfersock,buf,len)
int xfersock;
char *buf;
int len;
{
    int ix, left;

#if defined(PORTNAME_linux)
    fd_set readfds, writefds, exceptfds;
    struct timeval timeout;
#elif defined(_WIN32)
    fd_set readfds, writefds, exceptfds;
    struct tm timeout;
#else
    int readfds, writefds, exceptfds;
    struct tm timeout;
#endif 
/* PORTNAME_linux */

    int nready;
    int writebytes;

#if defined(PORTNAME_linux)
/* Clear fd_set for later use */
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);
#endif 
/* PORTNAME_linux */

#ifdef _WIN32
    memset((char *)&timeout,0,sizeof(timeout));
#else
    bzero((char *)&timeout,sizeof(timeout));
#endif
    for (ix=0, left=len;left>0;)
    {

#if defined(PORTNAME_linux)
		timeout.tv_sec = 2;
		timeout.tv_usec = 0;
	    FD_ZERO(&readfds);
		FD_ZERO(&exceptfds); 
		FD_SET((1<<xfersock), &writefds);
#elif defined(_WIN32)
        FD_ZERO(&readfds);
        FD_ZERO(&exceptfds);
        FD_ZERO(&writefds);
        FD_SET(xfersock,&writefds);
        timeout.tm_sec = 2;
#else
        timeout.tm_sec = 2;
        readfds = 0;
        exceptfds=0;
        writefds = 0 | (1<<xfersock);
#endif 
/* PORTNAME_linux */

        nready = select(xfersock+1, &readfds, &writefds,
             &exceptfds, &timeout);  /* wait for write space to be available */
        if (nready<0) {
            elog(NOTICE,"select error while writing data in _ftpWrite: %m");
            return nready;
        }

#if defined(PORTNAME_linux)
		if (!FD_ISSET((1<<xfersock), &writefds)) 
#elif defined(_WIN32)
        if(WSAGetLastError() == WSAEINTR)
#else
        if (writefds==0) 
#endif 
/* PORTNAME_linux */
        {
            elog(NOTICE,"timeout while writing data in _ftpWrite");
            return FTP_ERR_RESP_TIMEOUT;
        }

#ifdef _WIN32
        writebytes = send(xfersock, (char *)&buf[ix], left, 0); /* write data */
#else
        writebytes = write(xfersock, (char *)&buf[ix], left); /* write data */
#endif
        if (writebytes < 0) {
            elog(NOTICE,"write error in _ftpWrite: %m");
            return writebytes;
        }

        left -=  writebytes;
        ix += writebytes;
    }
    return ix;
}

/*
 * _ftpRead(xfersock,buf,len) - Read data on an open data input socket
 * Input:  buf -  the buffer to store into
 *         len - maximum character length to store into it
 * Output: buf - contained received data
 *         return value:
 *           < 0 error status
 *           > 0 number of bytes received
 *           = 0 no bytes received, transfer is complete
 */
int
_ftpRead(xfersock,buf,len)
int xfersock;
char *buf;
int len;
{
    int readbytes;
    int ix, left;
#if defined(PORTNAME_linux)
    fd_set readfds, writefds, exceptfds;
    struct timeval timeout;
#elif defined(_WIN32)
    fd_set readfds, writefds, exceptfds;
    struct tm timeout;
#else
    int readfds, writefds, exceptfds;
    struct tm timeout;
#endif 
/* PORTNAME_linux */

    int nready;

#if defined(PORTNAME_linux)
/* Clear fd_set for later use */
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);
#endif 
/* PORTNAME_linux */

#ifdef _WIN32
    memset((char *)&timeout,0,sizeof(timeout));
#else
    bzero((char *)&timeout,sizeof(timeout));
#endif
    for (ix=0, left=len;left>0;)
    {
#if defined(PORTNAME_linux)
        timeout.tv_sec = 2;
        timeout.tv_usec = 0;
        FD_ZERO(&writefds);
        FD_ZERO(&exceptfds);
        FD_SET((1<<xfersock), &readfds);
#elif defined(_WIN32)
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_ZERO(&exceptfds);
        FD_SET(xfersock,&readfds);
        timeout.tm_sec = 2;
#else
        timeout.tm_sec = 2;
        readfds = 0 | (1<<xfersock);
        writefds = 0;
        exceptfds=0;
#endif 
/* PORTNAME_linux */
        nready = select(xfersock+1, &readfds, &writefds,
                &exceptfds, &timeout);     /* wait for some data to come in */

        if (nready<0) {
            elog(NOTICE,"select error while reading data in _ftpRead: %m");
            return nready;
        }

#if defined(PORTNAME_linux)
		if (!FD_ISSET((1<<xfersock), &readfds)) 
#elif defined(_WIN32)
        if(WSAGetLastError() == WSAEINTR)
#else
        if (readfds==0) 
#endif
        {
            elog(NOTICE,"timeout while reading data in _ftpRead");
            return FTP_ERR_RESP_TIMEOUT;
        }

#ifdef _WIN32
        readbytes = recv(xfersock, (char *)&buf[ix], left, 0);
#else
        readbytes = read(xfersock, (char *)&buf[ix], left); /* read the data */
#endif

        if (readbytes==0) return ix;
        if (readbytes < 0) {
            elog(NOTICE,"read error in _ftpRead : %m");
            return readbytes;
        }
        left -=  readbytes;
        ix += readbytes;
    }
    return ix;
}


/* _ftpDisconnect() - Disconnect (logout) from the FTP server
 *
 * Input : none, uses ftpCmdSock global variable
 * Output : none
 */
int
_ftpDisconnect()
{
    int funcVal;
    int status;

    if (ftpCmdSock==0) { return 1;}

    funcVal=_ftpFlushReceive();  /* get any pending responses (eg transfer
                                    complete), if any, and ignore them. */

    if ((status = _ftpSend("quit\n")) < 0) {
        elog(NOTICE,"Error sending 'quit' command");
#ifdef _WIN32
    closesocket(ftpCmdSock);
#else
        close(ftpCmdSock);
#endif
        ftpCmdSock = 0;
        return status;
    }
    funcVal=_ftpReceive();                  /* get the response */

    if (funcVal != 221) {
        elog(NOTICE,"Error response during FTP server during FtpClose; %s",
                   ftpRcvCmdBuf);
#ifdef _WIN32
    closesocket(ftpCmdSock);
#else
        close(ftpCmdSock);
#endif
        ftpCmdSock = 0;
        return FTP_ERR_RETURN_VAL;
    }
#ifdef _WIN32
    closesocket(ftpCmdSock);
#else
    close(ftpCmdSock);
#endif
    ftpCmdSock = 0;
    return 0;
}

/*  ftpSetStorAttri(hostAddr,userAuth,dirPath) - Set Storage-System 
 *    Attributes; Connect to an FTP server and log in, and/or set
 *      current directory, or disconnect.
 * Input : char *hostAddr -  host name followed by optional ":" and port number
 *         char *userAuth -  user name followed by optional ":" and password
 *         char *dirPath  -  optional directory to cd to.
 * Output : return value is error code (if negative)
 *        connection is established to the server
 *
 *    hostAddr or dirPath may be null to leave them as is.
 *    If they are both null tho, do a disconnect from the server
 *    (graceful logout).
 */
int
ftpSetStorAttri(hostAddr,userAuth,dirPath)
char *hostAddr;
char *userAuth;
char *dirPath;
{
#define STRSIZE 100
    char userid[STRSIZE];
    char passwd[STRSIZE];
    char hostname[STRSIZE];
    int funcVal;
    int port;
    int i;
    char *cp1;
    char *cp2;
    int status;

/*
 Parse input hostAddr into hostname string and port number
 */
    port = 21;  /* default FTP control port */
    hostname[0]='\0';
    for (i=0, cp1=hostAddr, cp2=hostname;i<STRSIZE-2 && *cp1 !='\0';i++) {
        if (*cp1 != ':') { *cp2++ = *cp1++; }
        else {
            cp1++;
            port = atoi(cp1);
            break;
        }
    }
    *cp2 = '\0';

    if (hostname[0]!='\0') {
/*
 First disconnect, to clean up any messes (if connected).
 */
         _ftpDisconnect();
/*
 Connect
 */
        if ((status = _ftpConnect(hostname,port)) < 0)  return status;

        funcVal=_ftpReceive();

        if (funcVal != 220) {
            elog(NOTICE,"Error response from FTP server after connecting: %s",
                 ftpRcvCmdBuf);
            return FTP_ERR_RETURN_VAL;
        }

/*
 Parse input userAuth into userid and password
 */
        passwd[0] = '\0';
        for (i=0, cp1=userAuth, cp2=userid;i<STRSIZE-2 && *cp1 !='\0';i++) {
            if (*cp1 != ':') { *cp2++ = *cp1++; }
            else {
                *cp2 = '\0';
                cp2 = passwd;
                cp1++;
                i=0;
            }
        }
        *cp2 = '\0';

/*
  Login
 */

        if ((status = _ftpSend("USER %s\n",userid)) < 0) {
            elog(NOTICE,"Error sending USER id");
            return status;
        }

        funcVal=_ftpReceive();

        if (funcVal != 331 && funcVal != 230) {
            elog(NOTICE,"Error response from FTP server after sending USER:%s",
                 ftpRcvCmdBuf);
            return FTP_ERR_RETURN_VAL;
        }

        if (funcVal == 331)
        {
            if ((status = _ftpSend("PASS %s\n",passwd)) < 0) {
                elog(NOTICE,"Error sending password");
                return status;
            }

            funcVal=_ftpReceive();

            if (funcVal != 230) {
               elog(NOTICE,
                    "Error response from FTP server after sending password:%s",
                    ftpRcvCmdBuf);
                return FTP_ERR_RETURN_VAL;
            }
        }

    } /* end of test for non-null hostname */

    if (dirPath != 0 && dirPath[0]!='\0') {
        if ((status = _ftpSend("CWD %s\n",dirPath)) < 0) {
            elog(NOTICE,"Error sending CWD command");
            return status;
        }
        funcVal=_ftpReceive();

        if (funcVal != 250) {
            elog(NOTICE,"Error response from FTP Server after CWD command: %s",
                 ftpRcvCmdBuf);
            return FTP_ERR_RETURN_VAL;
        }
    }

    if ( hostname[0]=='\0' && ( dirPath==0 || *dirPath=='\0' ) ) {
        return(_ftpDisconnect());
    }
    return 1;
}

/*  _ftpConnect(hostname,port) -  Connect to an FTP server
 * Input : char *hostname - name of host to connect to
 *         int port - port to connect to
 * Output : return value is error code (if negative)
 *        connection is established to the server
 *        global ftpCmdSock is set
 */
int
_ftpConnect(hostname,port)
char *hostname;
int port;
{
    struct sockaddr_in server;
#ifdef _WIN32
    struct hostent *hp;
    u_long nonBlockingFlag = 1;
#else
    struct hostent *hp, *gethostbyname();
#endif
    int len;
    int status;

#ifdef _WIN32
    if ((status = startWinsock()) != 0) {
        elog(NOTICE,"error starting up Winsocket: %m");
    return status;
    }
#endif

    ftpCmdSock = socket(AF_INET, SOCK_STREAM, 0);    /* Create socket */
    if (ftpCmdSock < 0) {
        elog(NOTICE,"opening stream socket: %m");
        return ftpCmdSock;
    }

    server.sin_family = AF_INET;
    hp = gethostbyname(hostname);    /* get remote host address */
    if (hp == 0) {
        elog(NOTICE,"unknown host: %s", hostname);
        return SYS_ERR_HOST_ADDR;
    }
#ifdef _WIN32
    memcpy(&server.sin_addr, hp->h_addr, hp->h_length);
#else
    bcopy(hp->h_addr, &server.sin_addr, hp->h_length);
#endif
    server.sin_port = htons(port);

    if ((status = connect(ftpCmdSock, (struct sockaddr *)&server,
        sizeof(server))) < 0) {  /* connect */
        elog(NOTICE,"connecting stream socket: %m");
        return status;
    }

    /* set up for using 'select' */
#ifdef _WIN32
    if ((status = ioctlsocket(ftpCmdSock,FIONBIO,&nonBlockingFlag)) != 0)
#else
    if ((status = fcntl(ftpCmdSock,F_SETFL,O_NDELAY)) < 0)  
#endif
    {
        elog(NOTICE,"fcntl O_NDELAY: %m");
        return status;
    }

    len = sizeof (ftpCntlAddr);   /* get control addr to have our IP addr */
    if ((status = getsockname(ftpCmdSock, 
      (struct sockaddr *)&ftpCntlAddr, &len)) < 0) {
        elog(NOTICE,"getsockname: %m");
        return status;
    }

    return 1;
}

/* _ftpSend(fmt,arg1,arg2,arg3,arg4,arg5,arg6) - Send a command message
 *
 * Input : char *fmt: format string
 *         char *arg1 thru *arg6 up to six arguments to apply to format 
 *              string
 * Output : return value is error code (if negative)
 *        message is sent on the command socket
 */
int
_ftpSend(fmt,arg1,arg2,arg3,arg4,arg5,arg6)
char *fmt;
char *arg1, *arg2, *arg3, *arg4, *arg5, *arg6; 
{
    int len;

#if defined(PORTNAME_linux)
    fd_set readfds, writefds, exceptfds;
    struct timeval timeout;
#elif defined(_WIN32)
    fd_set readfds, writefds, exceptfds;
    struct tm timeout;
#else
    int readfds, writefds, exceptfds;
    struct tm timeout;
#endif 
/* PORTNAME_linux */

    int nready;
    int writebytes;

    char cmdbuf[2000];

#if defined(PORTNAME_linux)
/* Clear fd_set for later use */
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);
#endif 
/* PORTNAME_linux */

    if (ftpCmdSock == 0) {
        elog(NOTICE,"Not connected to an FTP Server, must call SetStorAttri");
        return FTP_ERR_NOT_CONNECTED;
    }
    sprintf((char *)cmdbuf,fmt,arg1,arg2,arg3,arg4,arg5,arg6);
    len = strlen((char *)cmdbuf);
    if (len<1) return FTP_ERR_CMD_LENGTH;

#ifdef _WIN32
    memset((char *)&timeout,0,sizeof(timeout));
#else
	bzero((char *)&timeout,sizeof(timeout));
#endif
#if defined(PORTNAME_linux)
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	FD_SET((1<<ftpCmdSock), &writefds);
	FD_ZERO(&exceptfds);
	FD_ZERO(&readfds);
#elif defined(_WIN32)
    FD_ZERO(&readfds);
    FD_ZERO(&exceptfds);
    FD_ZERO(&writefds);
    FD_SET(ftpCmdSock,&writefds);
    timeout.tm_sec = 1;
#else    
	timeout.tm_sec = 1;        /* wake up quickly on 1st pass */
	readfds = 0; 
	writefds =  0 | (1<<ftpCmdSock);
	exceptfds=0;
#endif 
/* PORTNAME_linux */


    nready = select(ftpCmdSock+1, &readfds, &writefds, &exceptfds, &timeout);
    if (nready<0) {
        elog(NOTICE,"select error in _ftpSend: %m");
        return nready;
    }


#if defined(PORTNAME_linux)
	if (!FD_ISSET((1<<ftpCmdSock), &writefds)) 
#elif defined(_WIN32)
    if(WSAGetLastError() == WSAEINTR)
#else
	if (writefds==0) 
#endif
    {
        elog(NOTICE,"timeout while writing command data in _ftpSend");
        return FTP_ERR_RESP_TIMEOUT;
    }

#ifdef _WIN32
    writebytes = send(ftpCmdSock, cmdbuf, len, 0);
#else
    writebytes = write(ftpCmdSock, cmdbuf, len);
#endif
    if (writebytes < 0) {
        elog(NOTICE,"write error in _ftpSend: %m");
        return writebytes;
    }

    if (ftpDebugFlag > 1) printf("sent=:%s:\n",cmdbuf);

    return 1;
}

/* _ftpReceive() -  Receive the next response message from a server
 *
 * Input : none, uses ftpCmdSock global variable
 * Output : data in global ftpRcvCmdBuf buffer,
 *          flag - set to 1 if this is the last of the data for this response
 *          return value - message code (if positive) or 
 *             error status (ifnegative)
 */
int
_ftpReceive()
{
    static int flag;
    int funcVal, lastFuncVal;
    lastFuncVal=-9999;
    for (funcVal=1;funcVal>0;)
    {
        funcVal=_ftpReceiveMore(&flag, lastFuncVal);
        if (funcVal>0 || lastFuncVal==-9999) lastFuncVal=funcVal;
        if (flag==1) break;
    }
    return (lastFuncVal);
}

/* _ftpReceiveMore(flag, commandValue) -   Receive initial or subsequent
 *        command data from a server.
 *
 * Input : commandValue - the command we are current receiving (or -9999)
 *         (also global variable)
 * Output : data in global ftpRcvCmdBuf buffer.
 *          flag - set to 1 if this is the last of the data for this response
 */
int
_ftpReceiveMore(flag, commandValue)
int *flag;
int commandValue;  /* the number of the command we are 
                      currently receiving (or -9999) */
{
    int readbytes, i, j;
    char matchstr[10];
#if defined(PORTNAME_linux)
    fd_set readfds, writefds, exceptfds;
    struct timeval timeout;
#elif defined(_WIN32)
    fd_set readfds, writefds, exceptfds;
    struct tm timeout;
#else
    int readfds, writefds, exceptfds;
    struct tm timeout;
#endif 
/* PORTNAME_linux */

    int nready;

#ifdef _WIN32
    memset((char *)&timeout,0,sizeof(timeout));
#else
    bzero((char *)&timeout,sizeof(timeout));
#endif
#if defined(PORTNAME_linux)
	timeout.tv_sec = 4;
	timeout.tv_usec = 0;
	FD_SET((1<<ftpCmdSock), &readfds);
	FD_SET((1<<ftpCmdSock), &exceptfds);
	FD_ZERO(&writefds);
#elif defined(_WIN32)
    FD_ZERO(&readfds);
    FD_ZERO(&exceptfds);
    FD_ZERO(&writefds);
    FD_SET(ftpCmdSock,&readfds);
    FD_SET(ftpCmdSock,&exceptfds);
    timeout.tm_sec = 4;
#else
	timeout.tm_sec = 4;
    readfds = 0 | (1<<ftpCmdSock);
    exceptfds = 0 | (1<<ftpCmdSock);
    writefds  = 0;
#endif 
/* PORTNAME_linux */

    nready = select(ftpCmdSock+1, &readfds, &writefds, &exceptfds, &timeout);
    if (nready<0) {
        elog(NOTICE,"select error in _ftpReceiveMore: %m");
        return nready;
    }

#if defined(PORTNAME_linux)
	if (!FD_ISSET((1<<ftpCmdSock), &readfds)) 
#elif defined(_WIN32)
    if(WSAGetLastError() == WSAEINTR)
#else
    if (readfds==0) 
#endif 
/* PORTNAME_linux */
    {
    	if (ftpDebugFlag>0) 
			printf("Note, timeout occurred in _ftpReceiveMore\n");
        return FTP_ERR_RESP_TIMEOUT;
    }

#ifdef _WIN32
    readbytes = recv(ftpCmdSock, ftpRcvCmdBuf, RCV_BUF_SIZE, 0);
#else
    readbytes = read(ftpCmdSock, ftpRcvCmdBuf, RCV_BUF_SIZE);
#endif
    if (readbytes < 0) {
        elog(NOTICE,"read error in _ftpReceiveMore: %m");
        return readbytes;
    }
    ftpRcvCmdBuf[readbytes]='\0';

    if (ftpDebugFlag > 1) printf("rcved=:%s:\n",ftpRcvCmdBuf);

    *flag=0;
    i = commandValue;
    if (commandValue < 0) {  /* first buffer of possible series */
        i = atoi(ftpRcvCmdBuf);
        if (i>100 && i<999 &&
            ftpRcvCmdBuf[3]==' '  &&
            ftpRcvCmdBuf[readbytes-1] == 0x0a ) *flag=1;  /* known to be all
                                     that is coming in response */
        return i;
    }
    /* look for "nnn " followed by crlf later the buffer to indicate the end */
    sprintf(matchstr,"%3d ",i);
    for (j=0;ftpRcvCmdBuf[j]!='\0' && j<readbytes;j++)   {
        if (ftpRcvCmdBuf[j]  == matchstr[0] &&
            ftpRcvCmdBuf[j+1]== matchstr[1] &&
            ftpRcvCmdBuf[j+2]== matchstr[2] &&
            ftpRcvCmdBuf[j+3]== ' '  &&
            ftpRcvCmdBuf[readbytes-1] == 0x0a )  {
                *flag=1;
                return i;
	  }
    }
    return i;
}

/* _ftpFlushReceive() - Flush the command receive socket; if data is
 *    pending on the command socket, read it (it will be disgarded).
 *
 * Input : none, uses ftpCmdSock global variable
 * Output : data may be stored in the global ftpRcvCmdBuf buffer.
 */
int
_ftpFlushReceive()
{
    int readbytes, i, j;

#if defined(PORTNAME_linux)
    fd_set readfds, writefds, exceptfds;
    struct timeval timeout;
#elif defined(_WIN32)
    fd_set readfds, writefds, exceptfds;
    struct tm timeout;
#else
    int readfds, writefds, exceptfds;
    struct tm timeout;
#endif 
/* PORTNAME_linux */

    int nready;

    if (ftpCmdSock == 0) return 0;

#ifdef _WIN32
    memset((char *)&timeout,0,sizeof(timeout));
#else
    bzero((char *)&timeout,sizeof(timeout));
#endif
#if defined(PORTNAME_linux)
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    FD_SET((1<<ftpCmdSock), &readfds);
    FD_SET((1<<ftpCmdSock), &exceptfds);
    FD_ZERO(&writefds);
#elif defined(_WIN32)
    FD_ZERO(&readfds);
    FD_ZERO(&exceptfds);
    FD_ZERO(&writefds);
    FD_SET(ftpCmdSock,&readfds);
    FD_SET(ftpCmdSock,&exceptfds);
    timeout.tm_sec = 0;
#else
    timeout.tm_sec = 0;
    readfds = 0 | (1<<ftpCmdSock);
    exceptfds = 0 | (1<<ftpCmdSock);
    writefds  = 0;
#endif 
/* PORTNAME_linux */

    nready = select(ftpCmdSock+1, &readfds, &writefds, &exceptfds, &timeout);

#ifdef _WIN32
    if(WSAGetLastError() == WSAEINTR)
#else
    if (nready<0) 
#endif
    {
        return 0;
    }

#ifdef _WIN32
    readbytes = recv(ftpCmdSock, ftpRcvCmdBuf, RCV_BUF_SIZE, 0);
#else
    readbytes = read(ftpCmdSock, ftpRcvCmdBuf, RCV_BUF_SIZE);
#endif
    return 0;
}


/* _ftpDebug(int flag) - set the ftp driver debug print flag; not currently 
 *                       called.
 * Input : int flag 
 * Output : none
 */
ftpDebug(int flag)
{
    ftpDebugFlag = flag;
    return 0;
}
