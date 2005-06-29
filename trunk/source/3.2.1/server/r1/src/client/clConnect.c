/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 

/*-------------------------------------------------------------------------
 *
 * clConnect.c--
 *    functions related to setting up a connection to the srbmaster
 *
 *-------------------------------------------------------------------------
 */

#include "clConnect.h"
#ifdef _WIN32
#include "Unix2Nt.h"
#include "startsock.h"
#endif /* WIN32 */

#ifndef _WIN32
#include <netinet/tcp.h>
#endif

void cPipSigHandler ();

#ifdef _WIN32
#define errno WSAGetLastError()
#endif /* WIN32 */

#ifndef _WIN32
void cleanConn ();
#endif

#if defined(PORTNAME_ultrix4) || defined(PORTNAME_next)
/* ultrix and next doesn't have strdup in libc. */

/* strdup() -
 * Input - origStr - the original string.
 * Output - dupedStr
 */
static
char *
strdup(char *origStr)
{
    char *dupedStr;

    dupedStr = strcpy((char *)malloc(strlen(origStr)+1), origStr);
    return dupedStr;
}
#endif

/* 
 * clConnect()  - Establishes a connectin to a srbServer through the srbMaster
 * at the specified host and port.
 *
 * Input : char *srbHost - The host address of srbMaster.
 *           If the input value in NULL, it will use the env variable
 *           "srbHost" if it is defined. If not, it use the hostname of the
 *           client machine.
 *         char *srbPort - The port # of srbMaster. If the input value in NULL,
 *           it will use the env variable "srbPort" if it is defined.
 *           If not, it defaults to the default port 5558).
 *         char *srbAuth - The auth string. It is used to define the password
 *           for MDAS or SEA authentication. For SEA authentication, this is
 *           the password used by the SEA library to decrypt the encrypted
 *           private key stored in the file  ~/.SEAuuuuu@ddddd (where uuuuu
 *           is the user ID and ddddd is the user domain name). This input is
 *           not needed if an unencrypted private key is available in the
 *           /tmp directory (generated using the 'seaauth auto' command).
 *           To provide additional flexibility, a client may also use the
 *           env variable "srbAuth" to specify the password. If the MDAS
 *           authentication scheme, a client may also supply the password in
 *           the ~.srb/.MdasAuth file.  If a client uses more than one method
 *           to specify the password, the vaule given in this function
 *           call will take precedent, the env variable "srbAuth" comes next
 *           and the ~.srb/.MdasAuth file is last.
 *
 * Output - returns a srbConn* which is needed for all subsequent calls.
 * if the status field of the connection returned is CONNECTION_BAD,
 * then some fields may be null'ed out instead of having valid values 
 */
srbConn* 
clConnect(char *srbHost, char* srbPort, char* srbAuth)
{
    srbConn *conn;
    char *tmp;
    int status;
    struct mdasEnvData *myMdasEnvData;


    conn = initSrbConn ();
    
#ifndef _WIN32
    if (UserClass == CLASS_CLI)
        signal (SIGPIPE, (void (*)(int)) cPipSigHandler);
#endif 

    myMdasEnvData = readMdasParam ();   /* read the .MdasEnv File */
    if (myMdasEnvData != NULL && myMdasEnvData->retVal < 0) {
        conn->status = myMdasEnvData->retVal;
        free (myMdasEnvData);
        return (conn);
    }

    if ((status = initUserInfo (conn, myMdasEnvData, srbAuth, NULL, NULL, 
     conn->errorMessage)) < 0) {
	conn->status = status;
	if (myMdasEnvData != NULL)
	    free (myMdasEnvData);
	return (conn);
    }
	
    clSetHost (conn, myMdasEnvData, srbHost, srbPort);

    conn->srbVersion = strdup(SRB_VERSION);

    /* set the authentication scheme */

    status = clSetAuthScheme (conn, myMdasEnvData, NULL);
    if (status < 0) {
	conn->status = status;
	if (myMdasEnvData != NULL)
	    free (myMdasEnvData);
	return (conn);
    }

    clSetAuthOption (conn, status);


    conn->status = connectSvr(conn);
    if (conn->status == SYS_ERR_PORTAL_CONN) {
	fprintf (stderr, "connectSvr failed, try again\n");
	conn->status = connectSvr (conn);
        if (conn->status >= 0) {
	    fprintf (stderr, "connectSvr succeeded the second time\n");
        }
    }

    if (myMdasEnvData != NULL)
        free (myMdasEnvData);
    return conn;
}

/*
 * tiUserConnect()  - Establishes a connection to a srbServer through the 
 * srbMaster at the specified host and port by a TICKET USER. Normal 
 * authentication will be bypassed for a TICKET USER.
 *
 * Input : char *srbHost - The host address of srbMaster.
 *           If the input value in NULL, it will use the env variable
 *           "srbHost" if it is defined. If not, it use the hostname of the
 *           client machine.
 *         char *srbPort - The port # of srbMaster. If the input value in NULL,
 *           it will use the env variable "srbPort" if it is defined.
 *           If not, it defaults to the default port 5558).
 *
 * Output - returns a srbConn* which is needed for all subsequent calls.
 * if the status field of the connection returned is CONNECTION_BAD,
 * then some fields may be null'ed out instead of having valid values
 */
srbConn*
tiUserConnect(char *srbHost, char* srbPort)
{
    srbConn *conn;
    char *userBuf;
    

    /* Set env "srbUser" to TICKET_USER */

    if ((getenv("srbUser") == NULL) || 
      (strcmp (getenv("srbUser"), TICKET_USER) != 0)) {
	userBuf = (char *) malloc (sizeof ("srbUser=") + 
	 sizeof (TICKET_USER) + 1);
	strcpy (userBuf, "srbUser=");
	strcat (userBuf, TICKET_USER);
	putenv (userBuf);

	/* Reset the global user ID */

	/* ProxyUser = ClientUser = NULL; */
    }

    conn = clConnect(srbHost, srbPort, NULL);

    return (conn);
}

/*
 * connectSvr -
 * make a connection to the srbMaster,  
 * Input - srbConn *conn (from clConnect()).
 *
 * Output - returns CLI_CONNECTION_OK if successful. Anything else are failure.
 *
 */
static int
connectSvr(srbConn *conn)
{
    int         status;

    status = initPort (conn);

    if (status < 0) {
	fprintf (stderr, "connectSvr: initPort error. status =%d\n",
	status);
	return (status);
    }

    if (getenv("SRB_SHELL_PORT") != NULL && SrbShellPort == -1) {
#if defined(PORTNAME_aix)
        size_t      laddrlen = sizeof(struct sockaddr);
#else
        int         laddrlen = sizeof(struct sockaddr);
#endif

	SrbShellPort = conn->sock = conn->port->sock = 
	 atoi (getenv("SRB_SHELL_PORT"));

        /* fill in the client address */
        if (getsockname(conn->port->sock, 
	  (struct sockaddr *) &conn->port->laddr,
          &laddrlen) < 0) {
	    fprintf (stderr, "connectSvr: getsockname error. errno = %d\n",
	      errno);
	}    

	if (SrbShellLockFd < 0 && getenv ("mdasEnvFile") != NULL) {
	    SrbShellLockFd = clFileLock ( getenv ("mdasEnvFile"),
	     F_SETLKW, F_WRLCK, 0);
	    if (SrbShellLockFd < 0) {
		return (SrbShellLockFd);
	    }
	}
#ifndef _WIN32
        if (SrbShellPort > 0 && UserClass ==  CLASS_CLI) {
            signal(SIGINT, cleanConn);
            signal(SIGHUP, cleanConn);
            signal(SIGTERM, cleanConn);
        }
#endif
    } else {
	status = connectPort (conn);
       if (status < 0) {
            fprintf (stderr, "connectSvr: connectPort error. status =%d\n",
            status);
            return (status);
        }
    }

    status = postProcPort (conn);

    return (status);
} 

int
initPort (srbConn *conn)
{
    struct hostent *hp;

#if defined(PORTNAME_c90) || defined(PORTNAME_t3e)
    short       status;
#else
    int         status;
#endif
    Port        *port;
    int         portno;
    
   /*
    *
    * Open a connection to srbMaster.
    *
    */
    port = (Port *) malloc(sizeof(Port));
    memset((char *) port, 0, sizeof(Port));

#ifdef _WIN32
    if ((status = startWinsock()) != 0) {
	(void) sprintf(conn->errorMessage,
	   "connectSvr() --  error starting Winsock\n");
	return CLI_ERR_SOCKET;	
    }
#endif /* WIN32 */

    if (!(hp = gethostbyname(conn->srbHost)) || hp->h_addrtype != AF_INET) {
	(void) sprintf(conn->errorMessage,
	   "connectSvr() --  unknown hostname: %s\n", conn->srbHost);
	return CLI_ERR_HOST_NAME;
    }
    memset((char *) &port->raddr, 0, sizeof(port->raddr));
    memmove((char *) &(port->raddr.sin_addr),
    	(char *) hp->h_addr, hp->h_length);
    port->raddr.sin_family = AF_INET;
    portno = atoi(conn->srbPort);
    port->raddr.sin_port = htons((unsigned short)(portno));
    
    conn->sockIn = -1;
    conn->port = port;

    return (0);
}

int 
connectPort (srbConn *conn)
{
    PacketBuf   pacBuf;
    int retBuf;
    int flag;
    int result;
    int svrPortNum;     /* The srbServer's port number */
    StartupInfo startup;
    MsgType	msgtype;
#if defined(PORTNAME_aix)
    size_t      laddrlen = sizeof(struct sockaddr);
#else
    int         laddrlen = sizeof(struct sockaddr);
#endif
#if defined(PORTNAME_c90) || defined(PORTNAME_t3e)
    short       status;
#else
    int         status;
#endif
    int connectCnt = 0;
    Port        *port = conn->port;

    /*
     * Initialize the startup packet.
     *
     */

    initStartupPack (conn, &startup);

    /* connect to the server  */
    if ((port->sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	(void) sprintf(conn->errorMessage,
	   "connectSvr() -- socket() failed: errno=%d\n%s\n", 
	    errno, strerror(errno));
	return CLI_ERR_SOCKET;	
    }

    while (connectCnt < MAX_CONN_SVR_CNT) {
        if ((status = connect(port->sock, (struct sockaddr *)&port->raddr,
	  sizeof(port->raddr))) < 0) {
           connectCnt ++;
            srbThreadSleep (0, 200000);
        } else {
            break;
        }
    }

    if (status < 0) {
	(void) sprintf(conn->errorMessage,
	"connectSvr: Connect() failed to host %s and port %s with Errno=%d.\n",
	 conn->srbHost,conn->srbPort, errno);
#ifdef _WIN32
	closesocket (port->sock);
    cleanWinsock();
#else
	close (port->sock);
#endif /* WIN32 */
	return CLI_ERR_SOCK_CONN;
    }
    

    /* fill in the client address */
    if (getsockname(port->sock, (struct sockaddr *) &port->laddr,
	&laddrlen) < 0) {
#ifdef _WIN32
	closesocket (port->sock);
    cleanWinsock();
#else
	close (port->sock);
#endif /* WIN32 */
	(void) sprintf(conn->errorMessage,
	    "connectSvr() -- getsockname() failed: errno=%d\n%s\n",
	    errno, strerror(errno));
        if (errno > 0)
            return -errno;
        else
            return CLI_ERR_SOCKET;
    }
    
    /* Connection has been opened */
    /** RAJA: added to reduce TCP delay in sending packets: June 30, 2000 ***/
    /** No checking of error for the operation is done ***/
#ifdef TCPNODELAYFLAG
    flag = 1;
    result = setsockopt(port->sock,            /* socket affected */
			IPPROTO_TCP,     /* set option at TCP level */
			TCP_NODELAY,     /* name of option */
			(char *) &flag,  /* the cast is historical
					    cruft */
			sizeof(int));    /* length of option value */
    if (result < 0) 
      fprintf(stderr,"Error: setsockopt:TCP_NODELAY failed to set: %d\n",result);
#endif /* TCPNODELAYFLAG */

    /* send the first msg */
#ifdef _WIN32
	status = send(port->sock, STARTUP_HEADER, strlen (STARTUP_HEADER) + 1,0);
#else
    status = write (port->sock, STARTUP_HEADER, strlen (STARTUP_HEADER) + 1);
#endif
    
    if (status <= 0) {
#ifdef _WIN32
    closesocket(port->sock);
#else
	close (port->sock);
#endif
	sprintf(conn->errorMessage,
	   "connectSvr() --  couldn't send header packet: errno=%d\n%s\n", 
	    errno,strerror(errno));
	return CLI_ERR_SEND_PACKET;
    }

    /* receive status from the server */
#ifdef _WIN32
	status = recv(port->sock, (char *) &retBuf, 4, 0);
#else
    status = read (port->sock, (char *) &retBuf, 4);
#endif /* WIN32 */
    if (status <= 0) {
    	sprintf(conn->errorMessage,
          "connectSvr() --  couldn't recv status1 packet: errno=%d\n%s\n",
            errno,strerror(errno));
	return CLI_ERR_RECV;
    }

#if defined(PORTNAME_c90) || defined(PORTNAME_t3e)
    svrPortNum = sToL (ntohl (VARSIZE (&retBuf)));
#else
    svrPortNum = ntohl (retBuf);
#endif

    if (svrPortNum < 0) {
        sprintf(conn->errorMessage,
          "connectSvr() --  Connection Error from server: status=%d\n",
            svrPortNum);
        return (svrPortNum);
    } else if (svrPortNum > 0) {

        /* close connection to the srbMaster and open connection to srbServer */

#ifdef _WIN32
        closesocket(port->sock);
#else
        close (port->sock);
#endif

        port->sock = portalConnect (svrPortNum, conn->srbHost);

        if (port->sock < 0) {
	    sprintf(conn->errorMessage,
              "connectSvr(): Connect to srbServer: %s, status = %d, errno=%d\n",
                conn->srbHost, port->sock, errno);
            return (port->sock);
        }
    }
    /* If svrPortNum == 0, connect to the same port */


#ifdef PORTNAME_solaris
    /* This tries to get around the problem of JAVA VM automatically set
     * any I/O to O_NONBLOCK.
     */
    flag = fcntl (port->sock, F_GETFL);
    if (flag & O_NONBLOCK)
	fcntl (port->sock, F_SETFL, (flag & (~O_NONBLOCK)));
#endif

    msgtype = STARTUP_MSG;
    startup2PacketBuf(&startup, &pacBuf);
    pacBuf.msgtype = htonl(msgtype);
    status = clPacketSend (port, &pacBuf, sizeof(PacketBuf));

    if (status == STATUS_ERROR) {
#ifdef _WIN32
	closesocket (port->sock);
    cleanWinsock();
#else
        close (port->sock);
#endif /* WIN32 */
        sprintf(conn->errorMessage,
           "connectSvr() --  couldn't send complete packet: errno=%d\n%s\n",
            errno,strerror(errno));
        return CLI_ERR_SEND_PACKET;
    }

    /* receive status from the server */
#ifdef _WIN32
	status = recv(port->sock, (char *) &retBuf, 4, 0);
#else
    status = read (port->sock, (char *) &retBuf, 4);
#endif /* WIN32 */
    if (status <= 0) {
        sprintf(conn->errorMessage,
          "connectSvr() --  couldn't recv status2 packet: errno=%d\n%s\n",
            errno,strerror(errno));
        return CLI_ERR_RECV;
    }

#if defined(PORTNAME_c90) || defined(PORTNAME_t3e)
    status = sToL (ntohl (VARSIZE (&retBuf)));
#else
    status = ntohl (retBuf);
#endif

    if (status != STATUS_OK) {
        sprintf(conn->errorMessage,
          "connectSvr() --  Connection Error from server: status=%d\n",
            status);
        return (status);
    }

    /* Authenticate as required*/

    if ((status = clSendAuth(&startup, conn)) != STATUS_OK) {
      return status;	
    }

    conn->sock = port->sock;  

    return (0);
}

int 
postProcPort (srbConn *conn)
{
    int flag;

    /* Set up the socket */
#ifdef _WIN32
	/* Pfin and Pfout are being replaced by sock */
    /* conn->Pfout = fdopen(port->sock,"w"); */
    /*  conn->Pfin  = fdopen(dup(port->sock), "r"); */
#else
    conn->Pfout = fdopen(conn->sock, "w");
    conn->sockIn = dup(conn->sock);
    if (conn->sockIn < 0) {
        (void) sprintf(conn->errorMessage,
        "connectSvr() -- dup () of %d failed: errno=%d\n%s\n",
        conn->sock, errno, strerror(errno));
        return CLI_ERR_FDOPEN;
    }

    conn->Pfin = fdopen(conn->sockIn, "r");
    if (!conn->Pfout || !conn->Pfin) {
	(void) sprintf(conn->errorMessage,
        "connectSvr() -- fdopen() failed: errno=%d\n%s\n", 
	errno, strerror(errno));
        return CLI_ERR_FDOPEN;	
    }
#endif /* WIN32 */
    
#ifdef PORTNAME_solaris
    /* This tries to get around the problem of JAVA VM automatically set
     * any I/O to O_NONBLOCK.
     */
    flag = fcntl (fileno(conn->Pfin), F_GETFL);
    fcntl (fileno(conn->Pfin), F_SETFL, (flag & (~O_NONBLOCK)));
#endif


    /* initialize the conn buffer */

    conn->commBuf = (comBuf *) malloc (sizeof (comBuf));
    conn->commBuf->inpInx1 = 0;
    conn->commBuf->inpInx2 = 0;
    conn->commBuf->outInx = 0;

    return CLI_CONNECTION_OK;
}

void 
initStartupPack(srbConn *conn, StartupInfo *startup)
{
    /* Fill in the proxy user Info */

    strncpy(startup->proxyUserName,conn->proxyUser->userName,
      sizeof(startup->proxyUserName));
    strncpy(startup->proxyDomainName,conn->proxyUser->domainName,
      sizeof(startup->proxyDomainName));

#ifdef FED_MCAT
    if (conn->proxyUser->mcatZone) {
        strncpy(startup->proxyMcatZone,conn->proxyUser->mcatZone, 
	 sizeof(startup->proxyMcatZone));
    }
    else
        startup->proxyMcatZone[0]='\0';
#endif

    /* fill in the proxyAuth anyway because it may use multiple auth scheme */
     if (conn->proxyUser != NULL && conn->proxyUser->userAuth != NULL) {
         strncpy(startup->proxyAuth,conn->proxyUser->userAuth,
          sizeof(startup->proxyAuth));
    } else {
	startup->proxyAuth[0] = '\0';
    }

    /* Fill in the client user Info */

    if (conn->clientUser == NULL || conn->clientUser->userName == NULL) {     
	/* Use the proxyUser Info */
        strncpy(startup->clientUserName,conn->proxyUser->userName,
          sizeof(startup->clientUserName));
        strncpy(startup->clientDomainName,conn->proxyUser->domainName,
          sizeof(startup->clientDomainName));
#ifdef FED_MCAT
        strncpy(startup->clientMcatZone, startup->proxyMcatZone,
          sizeof(startup->clientMcatZone));
#endif
        startup->clientAuth[0] = '\0';   /* Don't pass the passwd with SEA */
    } else {
        strncpy(startup->clientUserName,conn->clientUser->userName,
          sizeof(startup->clientUserName));
        strncpy(startup->clientDomainName,conn->clientUser->domainName,
          sizeof(startup->clientDomainName));

#ifdef FED_MCAT
        if (conn->clientUser->mcatZone) {
            strncpy(startup->clientMcatZone,conn->clientUser->mcatZone,
             sizeof(startup->clientMcatZone));
        }
        else
            startup->clientMcatZone[0]='\0';
#endif

        startup->clientAuth[0] = '\0';  /* Don't pass the passwd with SEA */
    }


    strncpy(startup->version,conn->srbVersion,sizeof(startup->version));
    if (conn->srbOptions) {
        strncpy(startup->options,conn->srbOptions, sizeof(startup->options));
    }
    else
        startup->options[0]='\0';
    startup->execFile[0]='\0';  /* not used */
}

/*
 * clPacketSend()
 *
 * clPacketSend -- send a single-packet message.
 *
 * Input : Port *port
 *         PacketBuf *buf - The Buffer to send.
 *         PacketLen len - The length of the Packet.
 *
 * Output : STATUS_ERROR if the write fails, STATUS_OK otherwise.
 *
 */
int
clPacketSend(Port *port, PacketBuf *buf, PacketLen len)
{
    PacketLen   totalLen;
    int         addrLen = sizeof(struct sockaddr_in);
    char *tmpBuf;

/*
#if defined(PORTNAME_c90) || defined(PORTNAME_t3e
    buf->msgtype = buf->msgtype | ((buf->len - sizeof (int)) << 32);
    tmpBuf = (char *) buf + sizeof (int);
    totalLen = len - sizeof (int);
#else
*/
    tmpBuf = (char *) buf;
    totalLen = len;
/*
#endif
*/

    len = send(port->sock, (Addr) tmpBuf, totalLen, 0);

    if (len < totalLen) {
        return(STATUS_ERROR);
    }

    return(STATUS_OK);
}

/*
 * clPacketReceive -- receive a packet on a port.
 *
 * Input : Port *port - The Port struct
 *         PacketBuf *buf - Input buffer
 *         char nonBlocking - nonBlocking or Blocking.
 *
 * Output : returns: connection id of the packet sender.
 *
 */
int
clPacketReceive(Port *port,       /* receive port */
              PacketBuf *buf,   /* MAX_PACKET_SIZE-worth of buffer space */
              char nonBlocking) /* NON_BLOCKING or BLOCKING i/o */
{
    PacketLen   max_size = sizeof(PacketBuf);
    PacketLen   cc;             /* character count -- bytes recvd */
    PacketLen   packetLen;      /* remaining packet chars to read */
    Addr        tmp;            /* curr recv buf pointer */
#if defined(PORTNAME_aix)
    size_t      addrLen = sizeof(struct sockaddr_in);
#else
    int         addrLen = sizeof(struct sockaddr_in);
#endif
    int         hdrLen;
    int         flag;
    int         decr;

#if defined(PORTNAME_c90) || defined(PORTNAME_t3e)
    hdrLen = 4;
#else
    hdrLen = sizeof(buf->len);
#endif

    if (nonBlocking == NON_BLOCKING) {
        flag = MSG_PEEK;
        decr = 0;
    } else {
        flag = 0;
        decr = hdrLen;
    }
    /*
     * port->nBytes is zero unless it is interrupted during
     * non-blocking I/O.  The first recvfrom() is to get the hdr
     * information so we know how many bytes to read.
     */
    tmp = ((Addr)buf) + port->nBytes;

    if (port->nBytes >= hdrLen)  {
        packetLen = ntohl(buf->len) - port->nBytes;
    }
     else {
        /* peeking into the incoming message */
#if defined(PORTNAME_c90) || defined(PORTNAME_t3e)
        cc = recvfrom(port->sock, (char *)buf, hdrLen, flag,
#else
        cc = recvfrom(port->sock, (char *)&(buf->len), hdrLen, flag,
#endif
            (struct sockaddr*) &(port->raddr), &addrLen);
        if (cc < hdrLen) {
            if (cc < 0) {
                return(STATUS_ERROR);
            }
            /*
             * cc == 0 means the connection was broken at the other end.
             */
            else if (! cc) {
                return(STATUS_INVALID);

            } else {
                return(STATUS_NOT_DONE);
            }
        } else {
            /*
             * Got the header. now get the length (including header size).
             */
            packetLen = ntohl(buf->len);
           if (packetLen > max_size) {
               port->nBytes = packetLen;
               return(STATUS_BAD_PACKET);
           }
            packetLen -= decr;
            tmp += decr - port->nBytes;
        }
    }

    /*
     * Read the packet.
     */
    while (packetLen) {
        cc = recvfrom(port->sock, tmp, packetLen, 0,
                      (struct sockaddr*) &(port->raddr), &addrLen);
        if (cc < 0)
            return(STATUS_ERROR);
        /*
         * cc == 0 means the connection was broken at the other end.
         */
        else if (! cc)
            return(STATUS_INVALID);

        tmp += cc;
        packetLen -= cc;

        /* if non-blocking, we're done. */
        if (nonBlocking && packetLen) {
            port->nBytes += cc;
            return(STATUS_NOT_DONE);
        }
    }

    port->nBytes = 0;
    return(STATUS_OK);
}

/*
 * freeSrbConn
 *   - free the srbConn data structure 
 *
 */
void 
freeSrbConn(srbConn *conn)
{
    if (conn->srbHost) free(conn->srbHost);
    if (conn->srbVersion) free(conn->srbVersion);
    if (conn->srbOptions) free(conn->srbOptions);
    if (conn->srbPort) free(conn->srbPort);
    if (conn->port) free(conn->port);  /* KJDAVIS - mem leak fix */
    if (conn->commBuf) free(conn->commBuf);
#ifdef foo	
    /* proxyUser and clientUser are normally from global ProxyUser and
     * ClientUser */
    if (conn->proxyUser) {
	if (conn->proxyUser->userName != NULL) 
	    free(conn->proxyUser->userName);
	if (conn->proxyUser->domainName != NULL) 
	    free(conn->proxyUser->domainName);
	if (conn->proxyUser->userAuth != NULL)
	    free(conn->proxyUser->userAuth);
	free(conn->proxyUser);
    }
    if (conn->clientUser) {
        if (conn->clientUser->userName != NULL) 
            free(conn->clientUser->userName);
        if (conn->clientUser->domainName != NULL) 
            free(conn->clientUser->domainName);
        if (conn->clientUser->userAuth != NULL)
            free(conn->clientUser->userAuth);
        free(conn->clientUser);
    }
#endif /* foo */
    free(conn);
}

/*
   closeSrbConn
     - properly close a connection to the backend
*/
static void
closeSrbConn(srbConn *conn)
{
  /*    fputs("X\0", conn->Pfout); 
    fflush(conn->Pfout);  */

    if (SrbShellPort > 0 && SrbShellPort == conn->sock) {
	/* don't disconnect */
	clFlush(conn->sock, conn->commBuf, conn->authSchemeInx);
	if (SrbShellLockFd >= 0 && getenv ("mdasEnvFile") != NULL) {
	    clFileLock ( getenv ("mdasEnvFile"),  F_SETLKW, F_UNLCK, 
	     SrbShellLockFd); 
	    SrbShellLockFd = -1;
	}
	return;
    }

    clPutnchar("X",1,conn->sock, conn->commBuf, conn->authSchemeInx);
    clFlush(conn->sock, conn->commBuf, conn->authSchemeInx);

#ifdef _WIN32
    if(conn->sock >= 0)
      closesocket(conn->sock);
    if(conn->sockIn >= 0)
      closesocket(conn->sockIn);
    cleanWinsock();
#else
    if (conn->Pfout) fclose(conn->Pfout);
    if (conn->Pfin)  fclose(conn->Pfin);
    if (conn->Pfdebug) fclose(conn->Pfdebug);
#ifdef foo
    /* No need to close sock and sockIn since sock is associated with
     * Pfout and sockIn is associated with Pfin
     */
    if (conn->sock >= 0) 
        close (conn->sock);
    if (conn->sockIn >= 0)
        close (conn->sockIn);
#endif
#endif /* WIN32 */
}

/*
   clFinish:
      properly close a connection to the backend
      also frees the srbConn data structure so it shouldn't be re-used 
      after this
*/
void
clFinish(srbConn *conn)
{
  if (conn == NULL)
    return;

  if (conn->status == CLI_CONNECTION_OK)
    closeSrbConn(conn);
  freeSrbConn(conn);
}

/* clReset :
   resets the connection to the backend
   closes the existing connection and makes a new one 
*/
void
clReset(srbConn *conn)
{
    closeSrbConn(conn);
    conn->status = connectSvr(conn);
}

/*
 * startup2PacketBuf()
 *
 * Put the content of a StartupInfo structure into a PacketBuf
 *
 * Input - StartupInfo* sInfo
 */
void
startup2PacketBuf(StartupInfo* sInfo, PacketBuf* pBuf)
{
    char* tmp;

    pBuf->len = htonl(sizeof(PacketBuf));
    pBuf->data[0] = '\0';

    tmp= pBuf->data;

#ifdef FED_MCAT
    strncpy(tmp, sInfo->proxyUserName, sizeof(sInfo->proxyUserName));
    tmp += sizeof(sInfo->proxyUserName);
    strncpy(tmp, sInfo->proxyDomainName, sizeof(sInfo->proxyDomainName));
    tmp += sizeof(sInfo->proxyDomainName);
    strncpy(tmp, sInfo->proxyMcatZone, sizeof(sInfo->proxyMcatZone));
    tmp += sizeof(sInfo->proxyMcatZone);
    strncpy(tmp, sInfo->clientUserName, sizeof(sInfo->clientUserName));
    tmp += sizeof(sInfo->clientUserName);
    strncpy(tmp, sInfo->clientDomainName, sizeof(sInfo->clientDomainName));
    tmp += sizeof(sInfo->clientDomainName);
    strncpy(tmp, sInfo->clientMcatZone, sizeof(sInfo->clientMcatZone));
    tmp += sizeof(sInfo->clientMcatZone);
    strncpy(tmp, sInfo->version, sizeof(sInfo->version));
    tmp += sizeof(sInfo->version);
    strncpy(tmp, sInfo->options, sizeof(sInfo->options));
    tmp += sizeof(sInfo->options);
    strncpy(tmp, sInfo->execFile, sizeof(sInfo->execFile));
#else
    strncpy(tmp, sInfo->proxyUserName, sizeof(sInfo->proxyUserName));
    tmp += sizeof(sInfo->proxyUserName);
    strncpy(tmp, sInfo->proxyDomainName, sizeof(sInfo->proxyDomainName));
    tmp += sizeof(sInfo->proxyDomainName);
    strncpy(tmp, sInfo->clientUserName, sizeof(sInfo->clientUserName));
    tmp += sizeof(sInfo->clientUserName);
    strncpy(tmp, sInfo->clientDomainName, sizeof(sInfo->clientDomainName));
    tmp += sizeof(sInfo->clientDomainName);
    strncpy(tmp, sInfo->options, sizeof(sInfo->options));
    tmp += sizeof(sInfo->options);
    strncpy(tmp, sInfo->execFile, sizeof(sInfo->execFile));
    tmp += sizeof(sInfo->execFile);
    strncpy(tmp, sInfo->version, sizeof(sInfo->version));
#endif
}

/* clHost - Returns the host address of the srbMaster
 *
 * Input - srbConn* conn (from clConnect())
 * Output - The host address
 */

char* 
clHost(srbConn* conn)
{
    return conn->srbHost;
}

/* clPort - Returns the port # of the srbMaster
 *
 * Input - srbConn* conn (from clConnect())
 * Output - the port # 
 */

char*
clPort(srbConn* conn)
{
    return conn->srbPort;
}

/* clStatus - Returns the status of a connection.
 *
 * Input - srbConn* conn (from clConnect())
 * Output - The status
 */

int
clStatus(srbConn* conn)
{
    return conn->status;
}

/* clErrorMessage - Returns the ErrorMessage of a connection. This is the
 * error message returned by the SRB server from the current client call.
 *
 * Input - srbConn* conn (from clConnect())
 * Output - The pointer to ErrorMessage
 */

char* 
clErrorMessage(srbConn* conn)
{
    return conn->errorMessage;
}

/* clTrace - Turn on tracing of a connection.
 *
 * Input - srbConn* conn (from clConnect())
 * Output - None
 */

void
clTrace(srbConn *conn, FILE* debug_port)
{
    if (conn == NULL ||
        conn->status != CLI_CONNECTION_OK) {
        return;
    }
    clUntrace(conn);
    conn->Pfdebug = debug_port;
}

/* clUntrace - Turn off tracing of a connection.
 *
 * Input - srbConn* conn (from clConnect())
 * Output - None
 */

void 
clUntrace(srbConn *conn)
{
    if (conn == NULL ||
      	conn->status != CLI_CONNECTION_OK) {
    	return;
    }
    if (conn->Pfdebug) {
    	fflush(conn->Pfdebug);
    	fclose(conn->Pfdebug);
    	conn->Pfdebug = NULL;
    }
}

/*
 * srbConnect()  - Establishes a connectin to a srbServer through the srbMaster
 * at the specified host and port. This is a new version of clConnect().
 * clConnect() still exists for backward compatibility purpose.
 *
 * Input : char *srbHost - The host address of srbMaster.
 *           If the input value in NULL, it will use the env variable
 *           "srbHost" if it is defined. If not, it use the hostname of the
 *           client machine.
 *         char *srbPort - The port # of srbMaster. If the input value in NULL,
 *           it will use the env variable "srbPort" if it is defined.
 *           If not, it defaults to the default port 5558).
 *         char *srbAuth - The auth string. It is used to define the password
 *           for MDAS or SEA authentication. For SEA authentication, this is
 *           the password used by the SEA library to decrypt the encrypted
 *           private key stored in the file  ~/.SEAuuuuu@ddddd (where uuuuu
 *           is the user ID and ddddd is the user domain name). This input is
 *           not needed if an unencrypted private key is available in the
 *           /tmp directory (generated using the 'seaauth auto' command).
 *           To provide additional flexibility, a client may also use the
 *           env variable "srbAuth" to specify the password. If the MDAS
 *           authentication scheme, a client may also supply the password in
 *           the ~.srb/.MdasAuth file.  If a client uses more than one method
 *           to specify the password, the vaule given in this function
 *           call will take precedent, the env variable "srbAuth" comes next
 *           and the ~.srb/.MdasAuth file is last.
 *         char *userName - The user ID in the SRB env.
 *         char *domainName - The user domain.
 *         char *authScheme - The authentication scheme to use. Valid values
 *            are: "PASSWD_AUTH", "SEA_AUTH", "SEA_ENCRYPT", "GSI_AUTH" and
 *            "GSI_SECURE_COMM" given in clAuthExtern.h.
 *         char *serverDn - the distinguish name of the server user. Valid
 *            only for the "GSI_AUTH" and "GSI_SECURE_COMM".
 *
 * Output - returns a srbConn* which is needed for all subsequent calls.
 * if the status field of the connection returned is CONNECTION_BAD,
 * then some fields may be null'ed out instead of having valid values
 */
srbConn*
srbConnect(char *srbHost, char* srbPort, char* srbAuth, char *userName,
char *domainName, char *authScheme,  char *serverDn)
{
    srbConn *conn;
    char *tmp;
    int status;
    struct mdasEnvData *myMdasEnvData;

    conn = initSrbConn ();
#ifndef _WIN32
    if (UserClass == CLASS_CLI)
        signal (SIGPIPE, (void (*)(int)) cPipSigHandler);
#endif
    /* Reverse the change given below because it is OK for myMdasEnvData ==
     * NULL (no .MdasEnv file). All input would then come from the env
     * variable. The portal group needs this feature.
     */
    myMdasEnvData = readMdasParam ();
    if (myMdasEnvData != NULL && myMdasEnvData->retVal < 0) {
        conn->status = myMdasEnvData->retVal;
	free (myMdasEnvData);
        return (conn);
    }
    /***vvv changed by Raja July 3, 2001 to get from non-Home environments
    myMdasEnvData = readMdasParam ();
    if (myMdasEnvData == NULL) {
        conn->status = CLI_ERR_MALLOC;
        return (conn);
    } else if (myMdasEnvData->retVal < 0) {
	free (myMdasEnvData);
	myMdasEnvData =  NULL;
    }
    ***/
    if ((status = initUserInfo (conn, myMdasEnvData, srbAuth, userName, 
     domainName, conn->errorMessage)) < 0) {
	conn->status = status;
	if (myMdasEnvData != NULL)
	    free (myMdasEnvData);
	return (conn);
    }
	
    clSetHost (conn, myMdasEnvData, srbHost, srbPort);

    conn->srbVersion = strdup(SRB_VERSION);

    /* set the authentication scheme */

    status = clSetAuthScheme (conn, myMdasEnvData, authScheme);
    if (status < 0) {
	conn->status = status;
	if (myMdasEnvData != NULL)
	    free (myMdasEnvData);
	return (conn);
    }

    clSetAuthOption (conn, status);

    conn->status = connectSvr(conn);

    if (myMdasEnvData != NULL)
        free (myMdasEnvData);

    return conn;
}

void cPipSigHandler ()
{
    fprintf (stderr, 
      "Caught broken pipe signal. Connection to server may be down\n");
}

srbConn *
initSrbConn ()
{
    srbConn *conn;

    conn = (srbConn *)malloc(sizeof(srbConn));
    memset (conn, 0, sizeof (srbConn));
#ifdef foo
    conn->Pfout = NULL;
    conn->Pfin = NULL;
    conn->Pfdebug = NULL;
    conn->port = NULL;
    conn->srbHost = NULL;
    conn->srbPort = NULL;
    conn->commBuf = NULL;
    conn->srbOptions = NULL;
    conn->proxyUser = NULL;
    conn->clientUser = NULL;
    conn->srbVersion = NULL;
    conn->errorMessage[0] = '\0';
#endif

    return (conn);
}

int
clSetHost (srbConn *conn, struct mdasEnvData *myMdasEnvData, char *srbHost, 
char *srbPort)
{
    char *tmp;

    if (!srbHost || srbHost[0] == '\0') {
        if (!(tmp = getenv("srbHost"))) {
            if (myMdasEnvData != NULL && myMdasEnvData->host[0] != '\0')
                tmp = myMdasEnvData->host;
            else
                tmp = DefaultHost;
        }
        conn->srbHost = strdup(tmp);
    } else
        conn->srbHost = strdup(srbHost);

    if (!srbPort || srbPort[0] == '\0') {
        if (!(tmp = getenv("srbPort")) || tmp[0] == '\0') {
            if (myMdasEnvData != NULL && myMdasEnvData->port[0] != '\0')
                tmp = myMdasEnvData->port;
            else
                tmp = DefaultPort;
        }
        conn->srbPort = strdup(tmp);
    } else
        conn->srbPort = strdup(srbPort);
    
    return 0;
}

int
srbSetsockopt (int sock)
{
    int winSz;
    struct linger linger;
    int status = 0;
    int one = 1;

    winSz = SOCK_WIN_SIZE;
#if defined(PORTNAME_osx)
 /* these two don't seem to work on Mac OS X, avoid run time error message */
#else
    if (setsockopt (sock, SOL_SOCKET, SO_SNDBUF, &winSz, sizeof (winSz)) < 0)
        status = -1;

    if (setsockopt (sock, SOL_SOCKET, SO_RCVBUF, &winSz, sizeof (winSz)) < 0)
        status = -1;
#endif

    if (setsockopt (sock, IPPROTO_TCP, TCP_NODELAY, &one, sizeof (one)) < 0)
        status = -1;

    linger.l_onoff = linger.l_linger = 0;

#if defined(PORTNAME_solaris)
    if (setsockopt(sock, SOL_SOCKET, SO_LINGER, (char *) &linger ,
     sizeof(linger)) < 0)
	status = -1;
#else
    if (setsockopt(sock, SOL_SOCKET, SO_LINGER, &linger ,sizeof(linger)) < 0)
	status = -1;
#endif
    if (setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (char *) &one, 
     sizeof(one)) < 0)
	status = -1;

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &one,
     sizeof(one)) < 0)
        status = -1;

    return (status);
}

int
clFileLock (char *filePath, int lockCmd, int lockType, int lockFd)
{
    int status;
    int myLockFd;
    flock_t myLock;

    if (lockType != F_UNLCK) {
        myLockFd = open (filePath, O_RDWR, 0644);
        if (myLockFd < 0) {
            fprintf (stderr, "clFileLock: Unable to open lockPath:%s, errno=%d",
             filePath, errno);
            return (myLockFd);
        }
    } else {
        myLockFd = lockFd;
    }

    /* now try to lock the file */
    myLock.l_type = lockType;
    myLock.l_whence = SEEK_SET;
    myLock.l_start = 0;
    myLock.l_len = 0;
    status = fcntl (myLockFd, lockCmd, &myLock);
    if (status < 0) {
        fprintf (stderr, "clFileLock: Unable to lock lockPath:%s, errno=%d\n",
          filePath, errno);
	return (status);
    }
    if (lockType == F_UNLCK) {
        close (myLockFd);
    }
    return (myLockFd);
}

#ifndef _WIN32
void
cleanConn ()
{
    if (UserClass ==  CLASS_CLI && SrbShellPort > 0) {
	close (SrbShellPort);
	kill (getppid (), SIGKILL); 
    }
    exit (1);
}
#endif

