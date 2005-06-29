/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/*
 *
 *    comm.c--
 *    Server Communication functions between the client and the SRB
 *
 *  Modified from FILE stream I/O to sockets for libSEA.
 *
 */

#include "comm.h"
#ifdef _WIN32
#include "Unix2Nt.h"
#include "SrbNtUtil.h"
#include "startsock.h"
#include "clMiscExtern.h"
#include "distrib.h"
#endif

#ifdef TCPNODELAYFLAG
#include <netinet/tcp.h>
#endif /* TCPNODELAYFLAG */

/* #define CONNECT_TIMING	1 */

int
newCommGetc(svrComm_t *myComm, int authSchemeInx)
{
    int len;
    if (myComm->inpInx1 < myComm->inpInx2) {
      return(myComm->inBuf[myComm->inpInx1++]);
    }
    myComm->inpInx1 = 0;

/* When SEA encryption is enabled, the minimum that seaRead can read
   at a time is 8 characters (an encryption block).  We could read more
   but then more may have to be moved in the newCommread function. */
    len = 8;
    errno = 0;

    myComm->inpInx2 = srbRead (myComm->commFd, (char *) myComm->inBuf,
     len, authSchemeInx);
    if (myComm->inpInx2 <= 0 && errno != 0 && errno != 2) {
        fprintf (stderr, 
	 "PID: %d. Server socket read error. errno=%d\n", getpid (),
          errno);
    }

    if (myComm->inpInx2 <= 0) return(-1);
    return(myComm->inBuf[myComm->inpInx1++]);
}

/* Empty the input pipe */

int commFlushInp (svrComm_t *myComm, int authSchemeInx)
{
    int len;
    int status = 0;
    int zeroCnt = 0;

    myComm->inpInx1 = 0;
    myComm->inpInx2 = 0;

/* When SEA encryption is enabled, the minimum that seaRead can read
 * at a time is 8 characters (an encryption block).  We could read more
 * but then more may have to be moved in the newCommread function. */
    len = 8;
    while (status >= 0 && zeroCnt < 10) {
    	status = srbRead(myComm->commFd, (char *) myComm->inBuf,len, authSchemeInx);
	/* fix a problem that is get into an inf loop with status = 0 */
	if (status == 0)
	    zeroCnt++;
    }
    return 0;
}

int 
newCommRead(svrComm_t *myComm, char *buf, int len, int authSchemeInx)
{
    int i, nrcved, toread;
    if (myComm->inpInx1 < myComm->inpInx2) {
        for (i=myComm->inpInx1;i<myComm->inpInx2 && i<len ;i++) *buf++ = myComm->inBuf[i];
        nrcved = i-myComm->inpInx1;
        myComm->inpInx1 = i;
    }
    else {
        nrcved=0;
    } 

    toread = len - nrcved;
    errno = 0;
    do {
        i = srbRead(myComm->commFd,buf,toread, authSchemeInx);
    	if (i <= 0 && errno != 0 && errno != 2) {
            fprintf (stderr, 
             "PID: %d. Server socket read error. errno=%d\n", getpid (),
              errno);
    	}   

        if (i>0) {
            buf += i;
	    toread -= i;
	    nrcved += i;
	}
    } while (nrcved<len && i>0);  /* must loop to attempt to fill buffer 
                                   like steam I/O does */
    if (i<0) nrcved=i;
    return(nrcved);
}

int
newCommPutc(svrComm_t *myComm, char c, int authSchemeInx)
{
    if (myComm->outInx==COMM_BUF_SIZE) {
        if (newFlush(myComm, authSchemeInx)) return(-1);
    }
    myComm->outBuf[myComm->outInx++]=c;
    return 0;
}


int
newCommPuts(svrComm_t *myComm, char *s, int authSchemeInx)
{
    while (myComm->outInx < COMM_BUF_SIZE && *s) {
        myComm->outBuf[myComm->outInx++]=*s++;
        if (myComm->outInx==COMM_BUF_SIZE) {
            if (newFlush(myComm, authSchemeInx)) return(-1);
	}
    }
    return (0);
}

int
newFlush(svrComm_t *myComm, int authSchemeInx)
{
    int i;
    if (!myComm->commFd) return 0;
    if (myComm->outInx > 0) {
        i = srbWrite(myComm->commFd,myComm->outBuf,myComm->outInx, authSchemeInx);
    	if (i <= 0)
            fprintf (stderr, "PID: %d. Server socket Write error. errno=%d\n", 
	     getpid (), errno);
        if (i != myComm->outInx) return(-1);
    }
    myComm->outInx = 0;
    return 0;
}

/*
 * commInit - Open the Pfin and Pfout streams
 *
 * Input - int fd - The file desc to fdopen.
 *
 * Output - None.
 */
void
commInit(svrComm_t *myComm, int fd)
{
    SrbServerClientSock = fd;
    myComm->commFd = fd;  
    myComm->inpInx1 = 0;
    myComm->inpInx2 = 0;
    myComm->outInx = 0;
    myComm->delegateCertInx = -1;
    if (ProxyUser != NULL) 
	ProxyUser->myComm = myComm;

#ifdef SEA_AUTH
   /* XXXX - take it out */
   /* seaSetShowNet(-1); call seaSetShowNet to display sea messages on stdout */
#endif

    if (getenv("LIBCL_DEBUG")) {
	Pfdebug = stderr;
    }else {
	Pfdebug = NULL;
    }
}

/*
 * commGetc() - get a character from the input stream,
 *
 * Output : A byte read
 */
int
commGetc(svrComm_t *myComm, int authSchemeInx)
{
  int c;

  c = newCommGetc(myComm, authSchemeInx);
  if (Pfdebug && c != EOF)
    putc(c,Pfdebug);
  return c;
}

/*
 * commGetport - return the srbPort setting
 *
 * Input - None
 *
 * Output - The port number of the srbMaster.
 *
 */
int
commGetport()
{
    char *envport = getenv("srbPort");
    
    if (envport)
	return(atoi(envport));
    return(atoi(DefaultPort));
}

/*
 * commClose - close input / output connections
 *
 * Input - None
 *
 * Output - None
 */
void
commClose(svrComm_t *myComm)
{
    if (myComm->commFd) {
#ifdef SEA_AUTH
	seaClose(myComm->commFd);
#else
#ifdef _WIN32 
    closesocket(myComm->commFd);
#else
	close(myComm->commFd);
#endif
#endif
	/* myComm->commFd = 0; */
    }
}

/* 
 * commFlush - flush the output stream
 *
 * Input - None
 * Output - None
 *
 */
void
commFlush(svrComm_t *myComm, int authSchemeInx)
{
    newFlush(myComm, authSchemeInx);
}

/*
 * commGetstr - get a null terminated string from connection
 *
 * Input : char *s - where to put the string read
 *	   int maxlen - Max length to read.
 *
 * Output : Return EOF or !EOF
 */
int
commGetstr(svrComm_t *myComm, char *s, int maxlen, int authSchemeInx)
{
    int	c;
    
    if (myComm->commFd == 0) {
	return(EOF);
    }
    
    while (maxlen-- && (c = commGetc(myComm, authSchemeInx)) != EOF && c)
	*s++ = c;
    *s = '\0';
    
    /*
     *     If EOF reached let caller know.
     *
     */
    if (c == EOF)
	return(EOF);
    return(!EOF);
}

/*
 * commGetnchar - get n characters from connection
 *
 * Input : char *s - where to put the string read
 *	   int off - The offest from s to put the string
 *	   int maxlen - Max length to read
 *
 * Output - Returns EOF or !EOF
 */
int
commGetnchar(svrComm_t *myComm, char *s, int off, int maxlen, int authSchemeInx)
{
    int	c;
    
    if (myComm->commFd == 0) {
	return(EOF);
    }
    
    s += off;
    if (maxlen <= CHAR_CUTOFF_LEN) {

    	while (maxlen-- && (c = commGetc(myComm, authSchemeInx)) != EOF)
            *s++ = c;
   
    	/*
    	 *     If EOF reached let caller know
    	 *
    	 */
    	if (c == EOF)
            return(EOF);
    } 
    else {
        if (newCommRead(myComm, s,maxlen, authSchemeInx) <= 0) {
            return(EOF);
    	}
	s += maxlen;
    }
    return(!EOF);
}

/*
 * commGetint - Get an integer from connection. Receive a byte at a time 
 * to take care of different ENDIAN representations.
 *
 * Input : int b - size in bytes of the integer to read.
 *
 * Output : The integer read.
 */
int
commGetint(svrComm_t *myComm, int b, int authSchemeInx)
{
    int	n, c, p;
    
    if (myComm->commFd == 0) {
	return(EOF);
    }
    
    n = p = 0;
    while (b-- && (c = commGetc(myComm, authSchemeInx)) != EOF && p < 32) {
	n |= (c & 0xff) << p;
	p += 8;
    }
    
    return(n);
}

/*
 * commGetlong - Get an 8 byte integer from connection. Receive a byte at a 
 * time to take care of different ENDIAN representations.
 *
 * Input : int b - size in bytes of the integer to read.
 *
 * Output : The integer read.
 */
srb_long_t *
commGetlong(svrComm_t *myComm, int b, int authSchemeInx)
{
    int	p;
    srb_long_t c, *tmpLong;
    
    if (myComm->commFd == 0) {
	return(NULL);
    }
    
    
    tmpLong = (srb_long_t *) malloc (2*SIZEOF32);
    if (tmpLong == NULL) {
        return(NULL);
    }


    p = 0;
    *tmpLong = 0;

    while (b-- && (c = commGetc(myComm, authSchemeInx)) != EOF && p < 64) {
	*tmpLong |= (c & 0xff) << p;
	p += 8;
    }
    
    return(tmpLong);
}

/*
 * commPutstr - send a null terminated string to connection
 *
 * Input : char *s - pointer to the string to send
 *
 * Output - None.
 */
void
commPutstr(svrComm_t *myComm, char *s, int authSchemeInx)
{
    int status;
    
    if (myComm->commFd) {
	status = newCommPuts(myComm, s, authSchemeInx);
	if (status == EOF) {
	    (void) fprintf(stderr,
	   	"FATAL: commPutstr: Puts() failed: errno=%d\n", errno);
	}
	status = newCommPutc(myComm, '\0', authSchemeInx);
	if (status == EOF) {
	    (void) fprintf(stderr,
	   	"FATAL: commPutstr: Putc() failed: errno=%d\n", errno);
	}
    }
}

/*
 * commPutnchar - send n characters to connection
 *
 * Input : char *s - pointer to the string to send.
 *	   int n - The number of char to send.
 *
 * Output - None
 */
void
commPutnchar(svrComm_t *myComm, char *s, int n, int authSchemeInx)
{
    int status;
    
    if (n <= 0)
        return;
    if (myComm->commFd) {
	if (n <= CHAR_CUTOFF_LEN) {
            while (n--) {
            	status = newCommPutc(myComm, *s++, authSchemeInx);
            	if (status == EOF) {
                    fprintf (stderr,
                      "PID: %d. newCommPutc() failed: errno=%d\n", 
			getpid (), errno);
		    break;
            	}
            }
	} else {
            newFlush(myComm, authSchemeInx);
    	    if ((status = srbWrite(myComm->commFd, s, n, authSchemeInx)) <= 0) {
            	    fprintf (stderr, 
		     "PID: %d. Server socket Write error. errno=%d\n", 
		      getpid (), errno);
    	    }
    	}
    }
}

/*
 * commPutint - send an integer to connection. The integer s sent in bytes
 * to account for different ENDIAN representations.
 *
 * Input : int i - The integer to send
 *	   int b - Size (in byte) of the integer
 *
 */
void
commPutint(svrComm_t *myComm, int i, int b, int authSchemeInx)
{
    int status;
    
    if (b > 4)
	b = 4;
    
    if (myComm->commFd) {
	while (b--) {
	    status = newCommPutc(myComm, i & 0xff, authSchemeInx);
	    i >>= 8;
	    if (status == EOF) {
		(void) fprintf(stderr,
		    "FATAL: commPutint: newCommPutc failed: errno=%d\n", errno);
	    }
	}
    }
}

/*
 * commPutlong - send a long long to connection. The integer s sent in bytes
 * to account for different ENDIAN representations.
 *
 * Input : int i - The integer to send
 *	   int b - Size (in byte) of the integer
 *
 */
void
commPutlong(svrComm_t *myComm, srb_long_t *inpLong, int b, int authSchemeInx)
{
    int status;
    srb_long_t i;
    
    i = *inpLong;

    if (b > 2*SIZEOF32)
	b = 2*SIZEOF32;
    
    if (myComm->commFd) {
	while (b--) {
	    status = newCommPutc(myComm, i & 0xff, authSchemeInx);
	    i >>= 8;
	    if (status == EOF) {
		(void) fprintf(stderr,
		    "FATAL: commPutlong: newCommPutc failed: errno=%d\n",errno);
	    }
	}
    }
}

/*
 * commGetinaddr - initialize address from host and port number 
 *
 * Input : struct sockaddr_in *sin - The resulting sockaddr_in.
 *	   char *host - The host name or address.
 *	   int port - The port number
 *
 * Output - Returns 0 - success, 1 - failure.
 *
 */
int
commGetinaddr(struct sockaddr_in *sin,
	     char *host,
	     int port)
{
    struct hostent	*hs;
    
    memset((char *) sin, 0, sizeof(*sin));
    
    if (host) {
	if (*host >= '0' && *host <= '9')
	    sin->sin_addr.s_addr = inet_addr(host);
	else {
	    if (!(hs = gethostbyname(host))) {
		perror(host);
		return(1);
	    }
	    if (hs->h_addrtype != AF_INET) {
		(void) fprintf(stderr,
		    "FATAL: commGetinaddr: %s not on Internet\n", host);
		return(1);
	    }
	    memmove((char *) &sin->sin_addr,
		    hs->h_addr,
		    hs->h_length);
	}
    }
    sin->sin_family = AF_INET;
    sin->sin_port = htons(port);
    return(0);
}

/*
 * streamServerPort -- open a sock stream "listening" port.
 *
 * This initializes the srbMaster's connection
 *	accepting port.  
 *
 * Input : char *hostName - The host name
 *	   short portName - The port number
 *	   int *fdP - Location to put the resulting fd.
 *
 * Output : Returns STATUS_OK or STATUS_ERROR
 */
int
streamServerPort(char *hostName, short portName, int *fdP)
{
    struct sockaddr_in	sin;
    int			fd;
    struct linger linger;
    int flag;
    int result;

#ifdef _WIN32
    if (startWinsock() != 0) {
    (void) fprintf(stderr,
        "FATAL: streamServerPort: startWinsock() failed\n");
    return(STATUS_ERROR);
    }
#endif
    
    if (! hostName)
	hostName = "localhost";
    
    memset((char *)&sin, 0, sizeof sin);
    
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	(void) fprintf(stderr,
	    "FATAL: streamServerPort: socket() failed: errno=%d\n", errno);
	return(STATUS_ERROR);
    }
    
    linger.l_onoff = linger.l_linger = 0;
#if defined(PORTNAME_solaris)
    setsockopt(fd, SOL_SOCKET, SO_LINGER, (char *) &linger ,sizeof(linger));
#elif defined(_WIN32)
    setsockopt(fd, SOL_SOCKET, SO_LINGER, (char*)&linger ,sizeof(linger));
#else
    setsockopt(fd, SOL_SOCKET, SO_LINGER, &linger ,sizeof(linger));
#endif

#ifdef TCPNODELAYFLAG
    flag = 1;
    result = setsockopt(fd,            /* socket affected */
			IPPROTO_TCP,     /* set option at TCP level */
			TCP_NODELAY,     /* name of option */
			(char *) &flag,  /* the cast is historical
					    cruft */
			sizeof(int));    /* length of option value */
    if (result < 0) 
      fprintf(stderr,"Error: streamServerPort: setsockopt:TCP_NODELAY failed to set: %d\n",result);
#endif /* TCPNODELAYFLAG */

    if (srbSetsockopt (fd) < 0) {
        fprintf(stderr,"error setting socket Option, errno = %d\n", errno);
    }

    sin.sin_family = AF_INET;
    sin.sin_port = htons(portName);
    
    if (bind(fd, (struct sockaddr *)&sin, sizeof sin) < 0) {
	(void) fprintf(stderr,
	     "FATAL: streamServerPort: bind() failed: errno=%d\n", errno);
	(void) fprintf(stderr, 
	    "\tIs another srbMaster already running on that port?\n");
	(void) fprintf(stderr, 
	    "\tIf not, wait a few seconds and retry.\n");
	return(STATUS_ERROR);
    }
    
#if defined(PORTNAME_solaris) || defined(PORTNAME_linux)
    listen(fd, 100);
#else
    listen(fd, SOMAXCONN);
#endif
    
    *fdP = fd;
    return(STATUS_OK);
}

/*
 * streamConnection -- create a new connection with client 
 *
 * Input : int server_fd - the server fd
 *	   Port *port - The port struct (for both input and output)
 * 
 * Output : Returns STATUS_OK or STATUS_ERROR
 */
int
streamConnection(int server_fd, Port *port)
{
#if defined(PORTNAME_aix)
    size_t addrlen;
#else
    int	addrlen;
#endif
    
    /* accept connection (and fill in the client (remote) address) */
    addrlen = sizeof(struct sockaddr_in);
    if ((port->sock = accept(server_fd,
    	(struct sockaddr *) &port->raddr, &addrlen)) < 0) {
	elog(NOTICE, "srbMaster: streamConnection: accept: %m");
	return(STATUS_ERROR);
    }

    /* fill in the server (local) address */
    addrlen = sizeof(struct sockaddr_in);
    if (getsockname(port->sock, (struct sockaddr *) &port->laddr, 
	&addrlen) < 0) {
	elog(NOTICE, "srbMaster: streamConnection: getsockname: %m");
	return(STATUS_ERROR);
    }
    
    
    return(STATUS_OK);
}

/* 
 * streamClose -- close a client/svrServer connection
 * 
 * Input : int sock - The socket to close
 *
 * Output - None
 */
void
streamClose(int sock)
{
#ifdef _WIN32
    (void)closesocket(sock);
#else
    (void) close(sock); 
#endif
}

/* 
 * srbDebug - Output a debug msg to debug_port.
 * 
 * Input : char *target - The format string (e.g. "%s").
 *	   char *msg - The debug message
 *
 * Output - None
 */
void
srbDebug(char *target, char *msg)
{
    if (!target)
        return;
   
    if (commTracep) {
        /*
         * if nothing else was suggested default to stdout
         */
        if (!debug_port)
            debug_port = stdout;
        fprintf(debug_port, target, msg);
        fprintf(debug_port, "\n");
    }
}

/*
 * commTrace - Turns on debug tracing
 * 
 * Input - None
 *
 * Output - None
 *
 */
void
commTrace()
{
    commTracep = 1;
}
 
/*
 * commUntrace - Turns off debug tracing
 * 
 * Input - None
 *
 * Output - None
 *
 */

void
commUntrace()
{
    commTracep = 0;
}

/*
 * packetSend()
 *
 * packetSend -- send a single-packet message.
 *
 * Input : Port *port
 *         PacketBuf *buf - The Buffer to send.
 *         PacketLen len - The length of the Packet.
 *
 * Output : STATUS_ERROR if the write fails, STATUS_OK otherwise.
 *
 */
int
packetSend(Port *port, PacketLen len)
{
    PacketLen   totalLen;
    int         addrLen = sizeof(struct sockaddr_in);
    char *tmpBuf;
    PacketBuf *buf = &port->buf;
   
/*
#if defined(PORTNAME_c90)
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
   
    len = sendto(port->sock, (Addr) tmpBuf, totalLen, 0,
        (struct sockaddr *)&(port->raddr), addrLen);

    if (len < totalLen) {
        return(STATUS_ERROR);
    }

    return(STATUS_OK);
}

/*
 * packetReceive -- receive a packet on a port.
 *
 * Input : Port *port - The Port struct
 *	   PacketBuf *buf - Input buffer
 *	   char nonBlocking - nonBlocking or Blocking.
 *
 * Output : returns: connection id of the packet sender.
 *
 */
int
packetReceive(Port *port,       /* receive port */
              char nonBlocking) /* NON_BLOCKING or BLOCKING i/o */
{
    PacketLen   max_size = sizeof(PacketBuf);
    PacketLen   cc;             /* character count -- bytes recvd */
    PacketLen   packetLen;      /* remaining packet chars to read */
    Addr        tmp;            /* curr recv buf pointer */
#if defined(PORTNAME_aix)
    size_t         addrLen = sizeof(struct sockaddr_in);
#else
    int         addrLen = sizeof(struct sockaddr_in);
#endif
    int         hdrLen;
    int         flag;
    int         decr;
    PacketBuf *buf = &port->buf;
 
#if defined(PORTNAME_c90)
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
#if defined(PORTNAME_c90)
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

/* initSrbHost - Initial the distributed SRB hosts. i.e., read the
 * MCAT_HOST file and initialize the distHost link list.
 *
 * Input - None
 *
 * Output - None
 */

int
initSrbHost ()
{
    FILE *file;
    char buf[MAX_LINE_SZ];
    char *portNum;
    char *conFile;
    int i;
    char *myHost, *myPort;
    struct hostElement *tmpHElement, *tmpHElement1;
    int remoteFlag;
    struct nameElement *tmpNElement;
    char *hostName;
    struct hostent *hostEnt;
    struct in_addr in;
    char *tmpName, *tmpName1, *tmpHost;
    int status;

    if (LocalHostHead != NULL)
	return (0);

    /* Initialize the local host */

    myHost = malloc (MAX_TOKEN);
    if (myHost == NULL) {
        elog (NOTICE, "initSrbHost: malloc error");
        return UNIX_ENOMEM;
    }

    if ((status = gethostname (myHost, MAX_TOKEN)) != 0) {
        elog (NOTICE, "initSrbHost: gethostname error");
        return status;
    }

#ifdef STK_SERVER
    myPort = getenv("stkPort");
#else
    myPort = getenv("srbPort");
#endif

    tmpHElement = enqueHost (myHost, myPort, &LocalHostHead);
    /* Enqueue all the address too */
    if ((hostEnt = gethostbyname (myHost)) == NULL) {
        elog (NOTICE, "initSrbHost: gethostbyname error. errno = %d", errno);
        strcpy (myHost, "localhost");
        if ((hostEnt = gethostbyname (myHost)) == NULL) {
            elog (NOTICE,
             "initSrbHost: gethostbyname error. errno = %d", errno);
            return (unixErrConv (errno));
        }
    }
    tmpName = malloc (MAX_TOKEN);
    strcpy (tmpName, "localhost");
    enqueName (tmpName, tmpHElement);
    if (strcasecmp (myHost, hostEnt->h_name) != 0) {
        tmpName = malloc (strlen (hostEnt->h_name) + 1);
        strcpy (tmpName, hostEnt->h_name);
        enqueName (tmpName, tmpHElement);
    }
#ifdef foo      /* do this at the end so that it is last in queue */
    i = 0;
    while (1) {
        if (hostEnt->h_addr_list[i] != NULL) {
#ifdef _WIN32
            memmove (&in, hostEnt->h_addr_list[i], hostEnt->h_length);
#else
            bcopy (hostEnt->h_addr_list[i], &in, hostEnt->h_length);
#endif
            tmpName1 = inet_ntoa (in);
            tmpName = malloc (strlen (tmpName1) + 1);
            strcpy (tmpName, tmpName1);
            enqueName (tmpName, tmpHElement);
            i++;
        } else {
            break;
        }
    }
#endif /* foo */


    /* put together the full pathname for the config file */

    conFile = (char *) malloc((strlen(getSrbDataDir()) +
        strlen(HOST_CONFIG)+2)*sizeof(char));

    strcpy(conFile, getSrbDataDir());
    strcat(conFile, "/");
    strcat(conFile, HOST_CONFIG);

    file = fopen(conFile, "r");

    if (file == NULL) {
	if (DebugLvl > 1)
            fprintf(stderr, "Unable to open HOST_CONFIG file %s\n", conFile);
        free (conFile);
	return (0);
    }

    free (conFile);

    /* read the file */

    while ((i = getNextStr(file, buf, sizeof(buf))) != EOF) {

        /* Read until end of line then next line */

        if (buf[0] == '#')
        {
            while (i == 0)
                i = getNextStr(file, buf, sizeof(buf));
            continue;
        }

	
        portNum = parseAddr (buf);

	remoteFlag = chkHost (buf, NULL, &tmpHElement);
	if (remoteFlag < 0) {	/* no match */
	    /* assume a remote host */
	    myHost = strdup (buf);
	    tmpHElement = enqueHost (myHost, portNum, &RemoteHostHead);
	}

	if (portNum != NULL) {		/* change port number */
	    tmpHElement->portNum = malloc (strlen (portNum) + 1);
	    strcpy (tmpHElement->portNum, portNum);
	}

        while (i == 0) {	/* read to end of line */
            i = getNextStr(file, buf, sizeof(buf));
            hostName = (char *) malloc(strlen (buf) + 1);
            strcpy (hostName, buf);
            parseAddr (hostName);
	    remoteFlag = chkHost (hostName, NULL, &tmpHElement1);
	    if (remoteFlag < 0) {	/* no match */
            	enqueName (hostName, tmpHElement);
	    } else if (remoteFlag == LOCAL_HOST) {
		if (tmpHElement1 == tmpHElement) {	/* both are local */
		    free (hostName);
		} else {	/* inconsistency */
		    /* Move all remote name to local. Don't have to 
		     * remove from remote queue because LocalHostHead is
		     * checked first.
		     */
		    tmpNElement = tmpHElement->alias;
		    while (tmpNElement != NULL) {
			enqueName (tmpNElement->hostName, tmpHElement1);
			tmpNElement = tmpNElement->next;
		    }
		}
	    } else {	/* remote host */
		free (hostName);
	    }
        }
    }

    tmpHElement = LocalHostHead;
    i = 0;
    while (1) {
        if (hostEnt->h_addr_list[i] != NULL) {
#ifdef _WIN32
            memmove (&in, hostEnt->h_addr_list[i], hostEnt->h_length);
#else
            bcopy (hostEnt->h_addr_list[i], &in, hostEnt->h_length);
#endif
            tmpName1 = inet_ntoa (in);
            tmpName = malloc (strlen (tmpName1) + 1);
            strcpy (tmpName, tmpName1);
            enqueName (tmpName, tmpHElement);
            i++;
        } else {
            break;
        }
    }

    if (initHostWithMCat () < 0) {
        fprintf (stderr, "initHostWithMCat failed\n");
        return -1;
    }

    return 0;
}

/* initHostWithMCat - Initial the distributed SRB hosts data structure 
 * using MCAT.
 *
 * Input - None
 *
 * Output - None
 */

int
initHostWithMCat ()
{
    FILE *file;
    char buf[MAX_LINE_SZ];
    char *mcatName = NULL;
    char *mcatHost = NULL;
    char *mcatPort, *myMcatHost;
    char *mcatAuthScheme = NULL;
    char *mcatDn = NULL;
    int readState = RS_BEGIN_READ;
    char *hostName, *localHostName;
    char *conFile;
    int cnt, i, j;
    struct hostElement *tmpHElement;
    char *myHost, *myPort;
    struct in_addr in;
    struct hostent *hostEnt;
    mdasC_sql_result_struct  *myresult = NULL;
    char *phyResType;
    char *phyResLoc;
    char *phyResName;
    char *phyDirPathName;
    char *logDirPathName;
    char *rescZone;
    char *phyResClass;
    char *phyResMaxObjSize;
    char *freeSpace;
    char *freeSpaceTime;
    char *tmpName, *tmpName1, *tmpHost;
    int tinx;
    char *cptr, *lastSl, *tmpPath;
    int remoteFlag;
    int status;
    char *tmp = NULL;
    struct mdasResInfo *tmpResInfo, *prevResInfo;
    struct hostElement  *hostTabPtr;

    /* Read the MCAT_HOST file for the MCAT enabled host name */

    conFile = (char *) malloc((strlen(getSrbDataDir()) +
        strlen(MCAT_HOST)+2)*sizeof(char));

    strcpy(conFile, getSrbDataDir());
    strcat(conFile, "/");
#if defined(FED_MCAT) && defined(FED_MCAT_FILE_CONFIG)
    strcat(conFile, FED_MCAT_HOST);
#else
    strcat(conFile, MCAT_HOST);
#endif

    file = fopen(conFile, "r");

    if (file == NULL) {
        fprintf(stderr, "Unable to open MCAT_HOST file %s\n", conFile);
        free (conFile);
        return -1;
    }

    free (conFile);

    /* read the file */

#ifdef FED_MCAT 
#ifdef FED_MCAT_FILE_CONFIG 
    cnt = 0;
    while ((i = getNextStr(file, buf, sizeof(buf))) != EOF) {
        /* Read until end of line then next line */

        if (buf[0] == '#')
        {
            while (i == 0)
                i = getNextStr(file, buf, sizeof(buf));
            continue;
        }

	mcatName = strdup (buf);

	i = getNextStr(file, buf, sizeof(buf));

	if (i < 0) {
	    /* must have at least 2 values per line */
            fprintf (stderr,
	     "%s config error. %s entry must have at least 2 values\n",
	     conFile, mcatName);
            fclose (file);
            return (-1);
	}

	mcatHost = strdup (buf);

	cnt ++;	/* increase host cnt */
        i = getNextStr(file, buf, sizeof(buf));

        if (i < 0) {
	    fillMcatHostInfo (mcatName, mcatHost, NULL, NULL, NULL, cnt);
	    continue;
	}
	mcatAuthScheme = strdup (buf);

        i = getNextLine (file, buf, sizeof(buf));

        if (i < 0) {
            fillMcatHostInfo (mcatName, mcatHost, mcatAuthScheme, NULL, NULL, 
	     cnt);
            continue;
        }

	mcatDn = strdup (buf);

	fillMcatHostInfo (mcatName, mcatHost, mcatAuthScheme, mcatDn, NULL, 
	 cnt);
    }
#else	/* FED_MCAT_FILE_CONFIG */
    while ((i = getNextLine(file, buf, sizeof(buf))) != EOF) {
        if (strlen (buf) <= 0)
            continue;
	if (mcatHost == NULL) {
	    mcatHost = strdup (buf);
	} else if (mcatAuthScheme == NULL) {
	    mcatAuthScheme = strdup (buf);
	} else if (mcatDn == NULL) {
	    mcatDn = strdup (buf);
	}
    }
    mcatName = strdup ("defaultMcat");
    fillMcatHostInfo (mcatName, mcatHost, mcatAuthScheme, mcatDn, NULL, 1);

    /* query the mcatZone */
    /* Now get the resource info from MCAT */

    myresult = getMcatZoneInfo ();
    if (myresult == NULL)
        fprintf (stderr, "initHostWithMCat - No result from getMcatZoneInfo\n");

    while (myresult != NULL) {
      int continueIndex;
      char *myMcatZone, *myMcatHost, *myMcatAuthScheme, *myMcatDn,
	*myLocality, *myPortNum;

      myMcatZone = (char *)getFromResultStruct(
                (mdasC_sql_result_struct *) myresult,
                  dcs_tname[ZONE_NAME], dcs_aname[ZONE_NAME]);

      myMcatHost = (char *)getFromResultStruct(
                (mdasC_sql_result_struct *) myresult,
                  dcs_tname[ZONE_NETPREFIX], dcs_aname[ZONE_NETPREFIX]);

      myMcatAuthScheme = (char *)getFromResultStruct(
                (mdasC_sql_result_struct *) myresult,
                  dcs_tname[ZONE_ADMIN_AUTH_SCHEME_NAME], 
		  dcs_aname[ZONE_ADMIN_AUTH_SCHEME_NAME]);

      myMcatDn = (char *)getFromResultStruct(
                (mdasC_sql_result_struct *) myresult,
                  dcs_tname[ZONE_ADMIN_DISTIN_NAME], 
                  dcs_aname[ZONE_ADMIN_DISTIN_NAME]);

      myLocality = (char *)getFromResultStruct(
                (mdasC_sql_result_struct *) myresult,
                  dcs_tname[ZONE_LOCALITY],
                  dcs_aname[ZONE_LOCALITY]);

      myPortNum = (char *)getFromResultStruct(
                (mdasC_sql_result_struct *) myresult,
                  dcs_tname[ZONE_PORT_NUM],
                  dcs_aname[ZONE_PORT_NUM]);

      for (i = 0; i < myresult->row_count; i++) {
        /* Process the host address */
	if (atoi (&myLocality[i*MAX_DATA_SIZE]) == 1) {
            fillMcatHostInfo (strdup (&myMcatZone[i*MAX_DATA_SIZE]), 
	     strdup (&myMcatHost[i*MAX_DATA_SIZE]), 
	     strdup (&myMcatAuthScheme[i*MAX_DATA_SIZE]), 
	     strdup (&myMcatDn[i*MAX_DATA_SIZE]), 
	     strdup (&myPortNum[i*MAX_DATA_SIZE]), 1);
	} else {
            fillMcatHostInfo (strdup (&myMcatZone[i*MAX_DATA_SIZE]), 
	     strdup (&myMcatHost[i*MAX_DATA_SIZE]), 
	     strdup (&myMcatAuthScheme[i*MAX_DATA_SIZE]), 
	     strdup (&myMcatDn[i*MAX_DATA_SIZE]), 
	     strdup (&myPortNum[i*MAX_DATA_SIZE]), 2);
	}
      }

      continueIndex = myresult->continuation_index;
      freeSqlResult (myresult);

      if (continueIndex >= 0) {
        int remoteFlag;
        struct hostElement  *hostTabPtr;

        remoteFlag = getMdasEnabledHost (NULL, &hostTabPtr);

        if (remoteFlag < 0) {
            status = remoteFlag;
        } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
            status = get_more_rows (MDAS_CATALOG, continueIndex, &myresult,
             DEF_NUM_ROWS);
#endif
        } else {
            status = remoteGetMoreRows (MDAS_CATALOG, continueIndex,
             &myresult, DEF_NUM_ROWS, hostTabPtr);
        }
      } else {
          break;        /* done */
      }
    }

#endif	/* FED_MCAT_FILE_CONFIG */
#else	/* FED_MCAT */
    while ((i = getNextLine(file, buf, sizeof(buf))) != EOF) {
        if (strlen (buf) <= 0)
            continue;
        if (mcatHost == NULL) {
            mcatHost = strdup (buf);
        } else if (mcatAuthScheme == NULL) {
            mcatAuthScheme = strdup (buf);
        } else if (mcatDn == NULL) {
            mcatDn = strdup (buf);
        }
    }
    mcatName = strdup ("defaultMcat");
    fillMcatHostInfo (mcatName, mcatHost, mcatAuthScheme, mcatDn, NULL, 1);
#endif

    fclose (file);

    /* Now get the resource info from MCAT */

    myresult = queryResInfo ();

    while (myresult != NULL) {
      int continueIndex;

      phyResType = (char *)getFromResultStruct(
                (mdasC_sql_result_struct *) myresult,
                  dcs_tname[PHY_RSRC_TYP_NAME], dcs_aname[PHY_RSRC_TYP_NAME]);
      phyResLoc = (char *)getFromResultStruct(
                (mdasC_sql_result_struct *) myresult,
                  dcs_tname[RSRC_ADDR_NETPREFIX],
                    dcs_aname[RSRC_ADDR_NETPREFIX]);
      phyResName = (char *)getFromResultStruct(
                (mdasC_sql_result_struct *) myresult,
                  dcs_tname[PHY_RSRC_NAME], dcs_aname[PHY_RSRC_NAME]);
      phyDirPathName = (char *)getFromResultStruct(
                (mdasC_sql_result_struct *) myresult,
                  dcs_tname[PHY_RSRC_DEFAULT_PATH],
                    dcs_aname[PHY_RSRC_DEFAULT_PATH]);
      logDirPathName = (char *)getFromResultStruct(
                (mdasC_sql_result_struct *) myresult,
                  dcs_tname[RSRC_DEFAULT_PATH], dcs_aname[RSRC_DEFAULT_PATH]);
      rescZone = (char *)getFromResultStruct(
                (mdasC_sql_result_struct *) myresult,
                  dcs_tname[RSRC_OWNER_ZONE], dcs_aname[RSRC_OWNER_ZONE]);
      phyResClass = (char *)get_from_result_struct(
                (mdasC_sql_result_struct *) myresult,
                  dcs_tname[RSRC_CLASS], dcs_aname[RSRC_CLASS]);
      phyResMaxObjSize = (char *)get_from_result_struct(
                (mdasC_sql_result_struct *) myresult,
                  dcs_tname[MAX_OBJ_SIZE], dcs_aname[MAX_OBJ_SIZE]);
      freeSpace = (char *)get_from_result_struct(
                (mdasC_sql_result_struct *) myresult,
                  dcs_tname[FREE_SPACE],
                  dcs_aname[FREE_SPACE]);
      freeSpaceTime = (char *)get_from_result_struct(
                (mdasC_sql_result_struct *) myresult,
                  dcs_tname[FS_TIMESTAMP],
                  dcs_aname[FS_TIMESTAMP]);

      for (i = 0; i < myresult->row_count; i++) {
	struct hostElement *myMcatHostElement;

	/* queue the resource */

        tmpResInfo = (struct mdasResInfo *)
          malloc (sizeof (struct mdasResInfo));
        memset (tmpResInfo, 0, sizeof (struct mdasResInfo));

        if (i == 0) {
            LocalResInfo = tmpResInfo;
        } else  {
            prevResInfo->next = tmpResInfo;
        }
        prevResInfo = tmpResInfo;

        tmpResInfo->phyResType = strdup (&phyResType[i*MAX_DATA_SIZE]);
        /* tmpResInfo->phyResLoc = strdup (&phyResLoc[i*MAX_DATA_SIZE]); */
#ifdef FED_MCAT
        tmpResInfo->phyResLoc = getAddrWithZone (&phyResLoc[i*MAX_DATA_SIZE],
         NULL);
#else
        tmpResInfo->phyResLoc =  strdup (&phyResLoc[i*MAX_DATA_SIZE]);
#endif
        tmpResInfo->phyResName = strdup (&phyResName[i*MAX_DATA_SIZE]);
        tmpResInfo->phyDirPathName = strdup (&phyDirPathName[i*MAX_DATA_SIZE]);
        tmpResInfo->logDirPathName = strdup (&logDirPathName[i*MAX_DATA_SIZE]);
        tmpResInfo->phyResClass = strdup (&phyResClass[i*MAX_DATA_SIZE]);
        tmpResInfo->phyResMaxObjSize = strtoll (&phyResClass[i*MAX_DATA_SIZE],
         0, 0);
	tmpResInfo->freeSpace = atoi (&freeSpace[i*MAX_DATA_SIZE]);
	if (tmpResInfo->freeSpace > 0) {
	    tmpResInfo->freeSpace *= MBYTES;
	}
	tmpResInfo->freeSpaceTime = atoi (&freeSpaceTime[i*MAX_DATA_SIZE]);

	/* Process the host address */
	tmpHost = malloc
          (strlen (&phyResLoc[i*MAX_DATA_SIZE]) + 1);
	strcpy (tmpHost, &phyResLoc[i*MAX_DATA_SIZE]);

	if (DebugLvl > 1) {
	    if (tmpHost)
		fprintf (stderr, "initHostWithMCat: host from DATABASE: %s\n", 
		 tmpHost);
	}

	myPort = parseAddr (tmpHost);
	/* dwwalte - check env variable srbPort */
	if (myPort == NULL) {
	    if ((tmp = getenv ("srbPort")) != NULL) {
		myPort = strdup (tmp);
	    }
	}

        if (DebugLvl > 1) {
            if (myPort)
                fprintf (stderr, "initHostWithMCat: port from DATABASE: %s\n", 
                 myPort);
        }

	remoteFlag = getMdasEnabledHost (&rescZone[i*MAX_DATA_SIZE], 
	 &myMcatHostElement);

	if (remoteFlag < 0) {
	    fprintf (stderr, 
	     "initHostWithMCat: No match zone for %s, Host %s\n",
                 &rescZone[i*MAX_DATA_SIZE], tmpHost);
	    myMcatHostElement = LocalMcat;
        } else {
	    if (myMcatHostElement->portNum != NULL)
	        myPort = strdup (myMcatHostElement->portNum);
	}

	remoteFlag = chkHost (tmpHost, myPort, &tmpHElement);

        if (remoteFlag < 0)
            tmpResInfo->flag = REMOTE_HOST;
        else
            tmpResInfo->flag = remoteFlag;

	if (remoteFlag < 0) {	/* A new host */

	    tmpHElement = enqueHost (tmpHost, myPort, &RemoteHostHead);

#ifdef _WIN32
        /* dont need to call 'gethostbyname' in Windows for efficiency. */
        continue;
#endif

    	    /* Enqueue all the address too */
    	    if ((hostEnt = gethostbyname (tmpHost)) == NULL) {
		if (DebugLvl > 1)
        	    elog (NOTICE, "initSrbHost: gethostbyname of %s error", 
		      tmpHost);
        	continue;
    	    }
    	    if (strcasecmp (tmpHost, hostEnt->h_name) != 0) {
        	tmpName = malloc (strlen (hostEnt->h_name) + 1);
        	strcpy (tmpName, hostEnt->h_name);
        	enqueName (tmpName, tmpHElement);
    	    }

    	    j = 0;
    	    while (1) {
        	if (hostEnt->h_addr_list[j] != NULL) {
#ifdef _WIN32
                    memmove(&in, hostEnt->h_addr_list[j], hostEnt->h_length);
#else
            	    bcopy (hostEnt->h_addr_list[j], &in, hostEnt->h_length);
#endif
	    	    tmpName1 = inet_ntoa (in);
	    	    tmpName = malloc (strlen (tmpName1) + 1);
	    	    strcpy (tmpName, tmpName1);
                    enqueName (tmpName, tmpHElement);
            	    j++;
        	} else {
            	    break;
        	}
    	    }

	} else if (remoteFlag == LOCAL_HOST) {	/* local host */
	    if (tmpHost)
		free (tmpHost);	/* KESTOCK - mem leak fix */
	    /* Process only the file system type */
            tinx = objTypeLookUp (&phyResType[i*MAX_DATA_SIZE]);
	    if (tinx < 0)
		continue;
            if (srbObjTypeEntry[tinx].objType != FILE_TYPE)
            	continue;

	    /* Configure the storage vault. Use the same logic as in 
	     * getFullPathName */
	    if (logDirPathName[i*MAX_DATA_SIZE] == '/') {
		tmpPath = 
		  malloc (strlen (&logDirPathName[i*MAX_DATA_SIZE]) + 1);
		strcpy (tmpPath, &logDirPathName[i*MAX_DATA_SIZE]);
	    } else if (logDirPathName[i*MAX_DATA_SIZE] == '\\' && 
	      logDirPathName[i*MAX_DATA_SIZE + 1] == '/') {
		tmpPath = 
		  malloc (strlen (&logDirPathName[i*MAX_DATA_SIZE + 1]) +
		    strlen (&phyDirPathName[i*MAX_DATA_SIZE]) + 1);
		strcpy (tmpPath, &phyDirPathName[i*MAX_DATA_SIZE]);
		strcat (tmpPath, &logDirPathName[i*MAX_DATA_SIZE + 1]);
	    } else {
		tmpPath = 
		  malloc (strlen (&phyDirPathName[i*MAX_DATA_SIZE]) + 1);
		strcpy (tmpPath, &phyDirPathName[i*MAX_DATA_SIZE]);
	    }

	    /* Vault path ends with the first '?' */

	    cptr = lastSl = tmpPath;

    	    while (*cptr != '\0') {
            	if (*cptr == '/') {
		    lastSl = cptr;
	    	} else if (*cptr == '?') {
		    *lastSl = '\0';
		    break;
		}
		cptr ++;
	    }
	    status = enqueVault (srbObjTypeEntry[tinx].systemType, tmpPath);
	    if (status < 0)
		return status;
	} else {	/* a remote host that already been queued */
            if (tmpHost)
		free (tmpHost);	/* KESTOCK - mem leak fix */
	}

	/* status = setZoneForHostElement 
	 (tmpHElement, &rescZone[i*MAX_DATA_SIZE]); */

	if (tmpHElement->zoneHostTab == NULL)
	    tmpHElement->zoneHostTab = myMcatHostElement;
      }		/* for loop */

      continueIndex = myresult->continuation_index;
      freeSqlResult (myresult);

      if (continueIndex >= 0) {
        int remoteFlag;
        struct hostElement  *hostTabPtr;

        remoteFlag = getMdasEnabledHost (NULL, &hostTabPtr);

        if (remoteFlag < 0) {
            status = remoteFlag;
        } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
            status = get_more_rows (MDAS_CATALOG, continueIndex, &myresult,
             DEF_NUM_ROWS);
#endif
        } else {
            status = remoteGetMoreRows (MDAS_CATALOG, continueIndex,
             &myresult, DEF_NUM_ROWS, hostTabPtr);
        }
      } else {
	  break;	/* done */
      }
    }	/* while loop */
#ifdef FED_MCAT
    /* Make sure LocalMcat->mdasEnabled is 1 since ther may be 
     * conflicting configuration on locality
     */
    LocalMcat->mdasEnabled = 1;
#endif

    return 0;
}

int
fillMcatHostInfo (char *mcatName, char *mcatHost, char *mcatAuthScheme, 
char *mcatDn, char *myPortNum, int cnt)
{
    struct serverAuthInfo *myServerAuthInfo;
    struct hostElement *tmpHElement;
    char *mcatPort;
    struct in_addr in;
    struct hostent *hostEnt;
    char *tmpName, *tmpName1, *tmpHost;
    char *tmp = NULL;
    int status;

#ifdef FED_MCAT
     /* cnt == 1 ==> local MCAT */
    if (cnt ==  1 && LocalMcat != NULL && LocalMcat->serverAuthInfo != NULL &&
     strcmp (LocalMcat->serverAuthInfo->mcatName, "defaultMcat") == 0) {
	/* overwrite mcatName from the mcatHost file */
	free (LocalMcat->serverAuthInfo->mcatName);
	LocalMcat->serverAuthInfo->mcatName = strdup (mcatName);
    }
#endif

    if (mcatHost == NULL || mcatHost[0] == '\0') {
        fprintf (stderr, "initHostWithMCat: mcatHost not defined for zone %s\n",
         mcatName);
        return (SYS_ERR_NO_MCAT_HOST);
    }
    if (mcatDn == NULL)
        mcatDn = strdup ("");

    /* have to strip out unneed port stuff */

    mcatPort = parseAddr (mcatHost);

    if (cnt == 1) {	/* local MCAT */
	/* env srbPort overrides */
        if ((tmp = getenv ("srbPort")) != NULL) {
            mcatPort = strdup (tmp);
#ifdef foo	/* don't depend on config in MCAT */ 
	} else if (myPortNum != NULL && atoi (myPortNum) > 0) {
            mcatPort = strdup (myPortNum);
#endif
        } else {
            mcatPort = strdup (DefaultPort);
        }
    } else {
	if (myPortNum != NULL && atoi (myPortNum) > 0) {
	    mcatPort = strdup (myPortNum);
	} else if ((tmp = getenv ("srbPort")) != NULL) {
            mcatPort = strdup (tmp);
        } else {
            mcatPort = strdup (DefaultPort);
        }
    }

    myServerAuthInfo = (struct serverAuthInfo *) malloc
     (sizeof (struct serverAuthInfo));

    status = fillServerAuthInfo (mcatHost, mcatAuthScheme,
     mcatDn, myServerAuthInfo);
    myServerAuthInfo->mcatName = mcatName;

    /* KESTOCK - mem leak patch */
    if (mcatAuthScheme != NULL)
        free (mcatAuthScheme);

    free (mcatDn);

    if (status < 0) {
        /* free (myServerAuthInfo); */
        fprintf (stderr, "initHostWithMCat: fillServerAuthInfo error\n");
        /* return (SYS_ERR_NO_MCAT_HOST); */
    }

    /* Is localhost MCAT enabled ? */

    if (chkHost (mcatHost, mcatPort, &tmpHElement) >= 0) {
        /* KESTOCK - mem leak fix */
        free (mcatHost);
    } else {
        tmpHElement = enqueHost (mcatHost, mcatPort, &RemoteHostHead);
        tmpHElement->mdasEnabled = 1;

        /* Enqueue all the address too */
        if ((hostEnt = gethostbyname (mcatHost)) == NULL) {
            elog (NOTICE,
              "initSrbHost: gethostbyname of mcatHost %s error,errno = %d", 
	      mcatHost, errno);
	    tmpHElement->mdasEnabled = 0;  /* because of some missing struct */
            return (unixErrConv (errno));
        }
        if (strcasecmp (mcatHost, hostEnt->h_name) != 0) {
            tmpName = malloc (strlen (hostEnt->h_name) + 1);
            strcpy (tmpName, hostEnt->h_name);
            enqueName (tmpName, tmpHElement);
        }

        i = 0;

        while (1) {
            if (hostEnt->h_addr_list[i] != NULL) {
#ifdef _WIN32
                memmove(&in, hostEnt->h_addr_list[i], hostEnt->h_length);
#else
                bcopy (hostEnt->h_addr_list[i], &in, hostEnt->h_length);
#endif
                tmpName1 = inet_ntoa (in);
                tmpName = malloc (strlen (tmpName1) + 1);
                strcpy (tmpName, tmpName1);
                enqueName (tmpName, tmpHElement);
                i++;
            } else {
                break;
            }
        }
    }
    /* point to itself */

    tmpHElement->zoneHostTab = tmpHElement;

    if (tmpHElement->serverAuthInfo != NULL) {
	if (strcmp (tmpHElement->serverAuthInfo->mcatName, 
	 myServerAuthInfo->mcatName) != 0)
	    elog (NOTICE, 
	     "WARNING WARNING fillMcatWithInfo: mcat %s and %s have the same host and port. This must be resolved before the system can function properly",
	     tmpHElement->serverAuthInfo->mcatName, myServerAuthInfo->mcatName);
    } else {
        tmpHElement->serverAuthInfo = myServerAuthInfo;
    }
    if (cnt == 1) {	/* the first extry ==> local MCAT */
        tmpHElement->mdasEnabled = MCAT_ENABLED_LOCAL;
#ifdef FED_MCAT
	LocalMcat = tmpHElement;
#endif
    } else {
#ifdef FED_MCAT
	if (tmpHElement != LocalMcat)
            tmpHElement->mdasEnabled = MCAT_ENABLED_REMOTE;
#endif
    }

    return (0);
}

mdasC_sql_result_struct *
queryResInfo ()
{
    int i;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int selval[MAX_DCS_NUM];
    mdasC_sql_result_struct  *myresult = NULL;
    int status;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }

    selval[PHY_RSRC_TYP_NAME] = 1;
    selval[RSRC_ADDR_NETPREFIX] = 1;
    selval[PHY_RSRC_NAME] = 1;
    selval[PHY_RSRC_DEFAULT_PATH] = 1;
    selval[RSRC_DEFAULT_PATH] = 1;
    selval[RSRC_OWNER_ZONE] = 1;
    selval[RSRC_CLASS] = 1;
    selval[MAX_OBJ_SIZE] = 1;
    selval[FREE_SPACE] = 1;
    selval[FS_TIMESTAMP] = 1;

    status = queryDataInfo (NULL, selval, (char (*) [MAX_TOKEN]) qval, 
     &myresult, 10 * DEF_NUM_ROWS);

    if (status < 0)
        return NULL;

    return myresult;
}


/*
 * enqueHost - Enque a hostName entry to the end of the host queue pointed to
 *             by queHead.
 *
 * Input :     char *hostName - The host to be queued. The format of
 *             the hostName string is hostName[:portNumber].
 *             struct hostElement *queHead - The queue head of the queue.
 *
 * Output : Returns the point to the (struct hostElement) of the host.
 */

struct hostElement *
enqueHost (char *hostName, char *portNum, struct hostElement **queHead)
{
    struct hostElement *tmpHElement, *tmpHElement1;

    tmpHElement = (struct hostElement *) malloc
        (sizeof (struct hostElement));
    if (tmpHElement == NULL) {
        elog (WARN, "enqueHost: Unable of malloc tmpHElement\n");
    }

    memset (tmpHElement, 0, sizeof (struct hostElement));
    tmpHElement->portNum = portNum;
    if (tmpHElement->portNum == NULL)
        tmpHElement->portNum = DefaultPort;

    tmpHElement->next = NULL;
    tmpHElement->conn = NULL;
    tmpHElement->mdasEnabled = 0;

    /* Queue the Name */

    enqueName (hostName, tmpHElement);

    if (*queHead == NULL) {
        *queHead = tmpHElement;
        return tmpHElement;
    }

    /* append tmpHElement to the end */

    tmpHElement1 = *queHead;
    while (tmpHElement1->next != NULL) {
        tmpHElement1 = tmpHElement1->next;
    }
    tmpHElement1->next = tmpHElement;
    return tmpHElement;
}

/*
 * enqueName - Enque a name entry to the end of the alias queue in the
 *             hostElement struct.
 *
 * Input :     char *name - The name to be queued.
 *             struct nameElement *hostElement - The hostElement.
 *
 * Output : None
 */

void enqueName (char *name, struct hostElement *hostElement)
{
    struct nameElement *tmpNElement, *tmpNElement1;

    tmpNElement = (struct nameElement *) malloc
        (sizeof (struct nameElement));
    if (tmpNElement == NULL) {
        elog (WARN, "enqueName: Unable of malloc nameElement\n");
    }

    tmpNElement->hostName = name;
    tmpNElement->next = NULL;

    if (hostElement->alias == NULL) {
        hostElement->alias = tmpNElement;
        return;
    }

    /* append tmpNElement to the end */

    tmpNElement1 = hostElement->alias;
    while (tmpNElement1->next != NULL) {
        tmpNElement1 = tmpNElement1->next;
    }
    tmpNElement1->next = tmpNElement;
}

int
remoteConnect (struct hostElement  *hostTabPtr)
{
    srbConn* conn;

    if (hostTabPtr->conn != NULL)       /* Already been opened */
        return 0;

    /* Already set to same as client at initialization */
    /* clConnectOpt(SEA_AUTHENT);  for now, do SEA authentication */

    conn = svrConnect (hostTabPtr->alias->hostName, 
     hostTabPtr->zoneHostTab->portNum, NULL, 0, hostTabPtr);

    if (clStatus(conn) != CLI_CONNECTION_OK) {
        elog (NOTICE,"Remote connect failed: %s",clErrorMessage(conn));
        /* return -1; */
	if (clStatus(conn) < START_OF_ERR_CODE)
	    /* valid code */
            return clStatus(conn);
	else
	    /* generic code */
	    return (SYS_ERR_REMOTE_CONN);
    }

    hostTabPtr->conn = conn;
    return 0;
}

void
remoteDisconnect (struct hostElement  *hostTabPtr)
{

    if (hostTabPtr == NULL)
        return;

    if (hostTabPtr->conn == NULL)       /* Already been disconnected */
        return;

    clFinish (hostTabPtr->conn);

    hostTabPtr->conn = NULL;
}

int
getMdasEnabledHost(char *mcatNameHint, struct hostElement **myHhostElement)
{
    struct hostElement  *tmpHElement;
    struct hostElement  *localHElement = NULL;
    int remoteFlag = OBJ_ERR_MDAS_HOST;
    char *mcatName;

    mcatName = getmcatNamefromHint (mcatNameHint);

#ifdef FED_MCAT
    if (mcatName == NULL || 
     strcmp (LocalMcat->serverAuthInfo->mcatName, mcatName) == 0) {
	*myHhostElement = LocalMcat;
	if (*myHhostElement == LocalHostHead)
	    return LOCAL_HOST;
	else
	    return REMOTE_HOST;
    }
#endif

    tmpHElement = LocalHostHead;

    while (tmpHElement != NULL) {
        if (tmpHElement->mdasEnabled == MCAT_ENABLED_LOCAL) {
#ifdef FED_MCAT
            localHElement = tmpHElement;
            remoteFlag = LOCAL_HOST;
#else
            *myHhostElement = tmpHElement;
            return LOCAL_HOST;
#endif
        }
        if (tmpHElement->mdasEnabled  > 0 && (mcatName == NULL ||
         strcmp (tmpHElement->serverAuthInfo->mcatName, mcatName) == 0)) {
            /* Can talk to mdas */
            *myHhostElement = tmpHElement;
            if (mcatName != NULL)
                free (mcatName);
            return LOCAL_HOST;
        }
        tmpHElement = tmpHElement->next;
    }

    tmpHElement = RemoteHostHead;

    while (tmpHElement != NULL) {
	if (tmpHElement->mdasEnabled == MCAT_ENABLED_LOCAL) {
#ifdef FED_MCAT
	    if (localHElement == NULL) {
	        localHElement = tmpHElement;
	        remoteFlag = REMOTE_HOST;
	    }
#else
            *myHhostElement = tmpHElement;
            return REMOTE_HOST;
#endif
	}
        if (tmpHElement->mdasEnabled > 0 && (mcatName == NULL ||
	 (tmpHElement->serverAuthInfo != NULL &&
	  strcmp (tmpHElement->serverAuthInfo->mcatName, mcatName) == 0))) {
            /* Can talk to mdas */
	    *myHhostElement = tmpHElement;
	    if (mcatName != NULL)
		free (mcatName);
            return REMOTE_HOST;
        }
        tmpHElement = tmpHElement->next;
    }

    /* no match, assume it is local */
    *myHhostElement = localHElement;
    if (mcatName != NULL)
        free (mcatName);
    return remoteFlag;
}

#ifdef foo	/* now in clMisc.c */
char *
getmcatNamefromHint(char *mcatNameHint)
{
    char *ptr1, *ptr2;
    int c;
    char *mcatName;

#ifdef FED_MCAT
    if (mcatNameHint == NULL) {
        mcatName = NULL;
    } else if ((ptr1 = strstr (mcatNameHint, "/")) != NULL) {
        /* a path name. just take the first entry */
        mcatName = malloc (strlen (mcatNameHint));
        memset (mcatName, 0, strlen (mcatNameHint));
        ptr1 = ptr1 + 1;        /* skip over '/' */
        ptr2 = mcatName;
        while ((c = *ptr1) != '\0') {
            if (c == '/' || c == 39 || c == ' ') {      /* 39 is ' */
                break;
            }
            *ptr2 = c;
            ptr1 ++;
            ptr2 ++;
        }
    } else {
        mcatName = strdup (mcatNameHint);
    }
#else
    mcatName = NULL;
#endif
    return (mcatName);
}
#endif	/* foo */

srbConn*
getRemoteConn (struct hostElement  *hostTabPtr)
{
    if (hostTabPtr == NULL) {
        elog(NOTICE,"getRemoteConn: The input hostTabPtr is NULL");
        return NULL;
    } else if (hostTabPtr->conn == NULL) {
        elog(NOTICE,"getRemoteConn: No connection to remote host %s",
          hostTabPtr->alias->hostName);
        return NULL;
    } else {
        return (hostTabPtr->conn);
    }
}

void
remoteClose (struct hostElement  *hostTabPtr)
{
    srbConn* conn;

    if (hostTabPtr->conn == NULL)      
        return;

    clFinish(hostTabPtr->conn);
    hostTabPtr->conn = NULL;
}

int
queryDataInfo (char *mcatName, int selval[], char qval[][MAX_TOKEN],
mdasC_sql_result_struct **myresult, int rowsWanted)
{
    int status;
    char *mcatHostHint;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    if (mcatName != NULL)
	mcatHostHint = mcatName;
    else
        mcatHostHint = getMcatHostHintByQval (qval);

    remoteFlag = getMdasEnabledHost (mcatHostHint, &hostTabPtr);

    if (remoteFlag < 0) {
	status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        status = get_data_dir_info
          (MDAS_CATALOG, qval, selval, myresult, dcs_tname, dcs_aname,
            rowsWanted);
        if (status < 0 && status != MCAT_INQUIRE_ERROR) {
            elog(NOTICE,"mdasGetInfo: get_data_dir_info error. status = %d",
            status);
            return (status);
        }
#endif
    } else {
        if ((status = remoteConnect (hostTabPtr)) < 0) {
            elog(NOTICE,"mdasGetInfo: remoteConnect error. status =%d",
              status);
            return status;
        }

        /* Do a client call. Have to supply a mdasC_sql_result_struct. */

        *myresult = (mdasC_sql_result_struct  *)
          malloc (sizeof (mdasC_sql_result_struct));

        if (*myresult == NULL) {
            elog(NOTICE,"queryDataInfo: malloc error");
            return SYS_ERR_MALLOC;
        }

        if ((status = srbGetDataDirInfo(hostTabPtr->conn, MDAS_CATALOG,
         (char (*) [MAX_TOKEN]) qval, selval, *myresult, rowsWanted)) < 0) {
	    if (status != MCAT_INQUIRE_ERROR) 
            	elog(NOTICE, "srbGetDataDirInfo failed: %s",
                 clErrorMessage(hostTabPtr->conn));
            freeSqlResult (*myresult);
            return status;
        }
    }
    if (status >= 0 && (*myresult)->continuation_index >= 0) {
	LastQueryHost = hostTabPtr;
	LastContinuationIndex = (*myresult)->continuation_index;
    }
    return status;
}

int
genQueryDataInfo (int cat_type, char *myMcatZone, genQuery_t *myGenQuery,
mdasC_sql_result_struct **myresult, int rowsWanted)
{
    int status;

    char *mcatHostHint;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    if (myMcatZone != NULL && strlen (myMcatZone) > 0)
        mcatHostHint = myMcatZone;
    else
        mcatHostHint = getMcatHostHintByGenQuery (myGenQuery);

    remoteFlag = getMdasEnabledHost (mcatHostHint, &hostTabPtr);

    if (remoteFlag < 0) {
        status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        char *qval, *tmpPtr;
        int selval[MAX_DCS_NUM];
        int i, qvalInx;

        qval = malloc (MAX_DCS_NUM * MAX_TOKEN);

        for (i = 0; i < MAX_DCS_NUM; i++) {
            selval[i] = 0;
            sprintf(&qval[i * MAX_TOKEN],"");
        }

        for (i = 0; i < myGenQuery->selInxCnt; i++) {
            selval[myGenQuery->selInx[i]] = myGenQuery->selVal[i];
        }

        tmpPtr = (char *) myGenQuery->qval;
        for (i = 0; i < myGenQuery->qvalCnt; i++) {
            qvalInx = myGenQuery->qvalInx[i];
            strcpy (&qval[qvalInx * MAX_TOKEN], tmpPtr);
            tmpPtr += MAX_TOKEN;
        }

        status = get_data_dir_info
          (cat_type, (char (*) [MAX_TOKEN]) qval, selval, myresult,
           dcs_tname, dcs_aname, rowsWanted);
        free (qval);
        if (status < 0 && status != MCAT_INQUIRE_ERROR) {
            elog(NOTICE,"genQueryDataInfo: get_data_dir_info error.status = %d",
            status);
            return (status);
        }
#endif
    } else {
        if ((status = remoteConnect (hostTabPtr)) < 0) {
            elog(NOTICE,"genQueryDataInfo: remoteConnect error. status =%d",
              status);
            return status;
        }

        /* Do a client call. Have to supply a mdasC_sql_result_struct. */

        *myresult = (mdasC_sql_result_struct  *)
          malloc (sizeof (mdasC_sql_result_struct));

        if (*myresult == NULL) {
            elog(NOTICE,"genQueryDataInfo: malloc error");
            return SYS_ERR_MALLOC;
        }

        if ((status = srbGenQuery (hostTabPtr->conn, cat_type,
         myMcatZone, myGenQuery, *myresult, rowsWanted)) < 0) {
            if (status != MCAT_INQUIRE_ERROR)
                elog(NOTICE, "genQueryDataInfo failed: %s",
                 clErrorMessage(hostTabPtr->conn));
            return status;
        }
    }
    if (status >= 0 && (*myresult)->continuation_index >= 0) {
        LastQueryHost = hostTabPtr;
        LastContinuationIndex = (*myresult)->continuation_index;
    }
    return status;
}

int
_svrGetMoreRows (int catalog, int contDesc, mdasC_sql_result_struct **myresult,
int rowsWanted)
{
    int status;
    char *mcatHostHint;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    if (LastQueryHost != NULL && LastContinuationIndex == contDesc) {
	mcatHostHint = LastQueryHost->serverAuthInfo->mcatName;
    } else {
	mcatHostHint = NULL;
    }
    remoteFlag = getMdasEnabledHost (mcatHostHint, &hostTabPtr);

    if (remoteFlag < 0) {
        status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        status = get_more_rows (catalog, contDesc, myresult, rowsWanted);
#endif
    } else {
        status = remoteGetMoreRows (catalog, contDesc, myresult, rowsWanted,
        hostTabPtr);
    }
    return (status);

}

char *
getMcatHostHintByQval (char qval[][MAX_TOKEN])
{
    if (strlen (qval[DATA_GRP_NAME]) != 0) {
	/* XXXXX this is a hack for passing the mcatName */
	int len = strlen ("MCAT_NAME=");
	if (strncmp ("MCAT_NAME=", qval[DATA_GRP_NAME], len)  == 0) {
	    qval[DATA_GRP_NAME][0] = '\0';
	    return (&qval[DATA_GRP_NAME][len]);
	} else {
	    return (qval[DATA_GRP_NAME]);
	}
    } else if (strlen (qval[PARENT_COLLECTION_NAME]) != 0) {
	return (qval[PARENT_COLLECTION_NAME]);
    } else if (strlen (qval[CONTAINER_NAME]) != 0) {
	return (qval[CONTAINER_NAME]);
    } else {
        return (NULL);
    }
}

/*
 * svrConnect()  - SRB Server to Server connect. This is almost the same as the
 *                 clConnect () call.
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
 *	   int proxyUserFlag - If set to 1, force cliientUser the same as
 *	     proxyuser
 *
 * Output - returns a srbConn* which is needed for all subsequent calls.
 * if the status field of the connection returned is CONNECTION_BAD,
 * then some fields may be null'ed out instead of having valid values
 */
srbConn*
svrConnect(char *srbHost, char* srbPort, char* srbAuth, int proxyUserFlag,
struct hostElement *hostTabPtr)
{
    srbConn *conn;
    char *tmp;
    int status;

    conn = initSrbConn ();

#ifndef _WIN32
    if (UserClass == CLASS_CLI)
        signal (SIGPIPE, cPipSigHandler);
#endif

    /* Fill in the ProxyUser and ClientUser struct */

    if (ThisMdasEnvData == NULL) {
        ThisMdasEnvData = readMdasParam ();   /* read the .MdasEnv File */

        if (ThisMdasEnvData == NULL) {
            conn->status = CLI_ENV_FILE_ERR;
            return (conn);
        } else if (ThisMdasEnvData->retVal < 0) {
            conn->status = ThisMdasEnvData->retVal;
            free (ThisMdasEnvData);
	    ThisMdasEnvData = NULL;
            return (conn);
        }
    }

#ifdef foo
    if (ThisUser == NULL) {
    	if ((status = initUserInfo (conn, ThisMdasEnvData, srbAuth, NULL, NULL, 
     	 conn->errorMessage)) < 0) {
            conn->status = status;
            return (conn);
	}
	ThisUser = (userInfo *) malloc (sizeof (userInfo));
	dupUserInfo (ThisUser, conn->proxyUser);
	if (ThisMdasEnvData->authScheme != NULL) {
	    struct authsvc *myAuthsvc;
	    ThisUser->authSchemeInx = 
	      matchAuthByKeyWd (ThisMdasEnvData->authScheme, &myAuthsvc);
	}
#endif
    if (ClientUser == NULL) {
        if ((status = initUserInfo (conn, ThisMdasEnvData, srbAuth, NULL, NULL,
         conn->errorMessage)) < 0) {
            conn->status = status;
            return (conn);
        }
	ClientUser = (userInfo *) malloc (sizeof (userInfo));
#ifdef FED_MCAT
	ClientUser = (userInfo *) malloc (sizeof (userInfo));
	if (conn->clientUser != NULL) {
	    dupUserInfo (ClientUser, conn->clientUser);
	} else {
	    memset (ClientUser, 0, sizeof (userInfo));
	}
#else
	if (conn->clientUser != NULL) {
            ClientUser = (userInfo *) malloc (sizeof (userInfo));
            dupUserInfo (ClientUser, conn->clientUser);
	} else {
	    ClientUser = NULL;
	}
#endif
	/* This is needed for Building the MCAT utility */
	if (ThisUser == NULL) {
            ThisUser = (userInfo *) malloc (sizeof (userInfo));
            dupUserInfo (ThisUser, conn->proxyUser);
            if (ThisMdasEnvData->authScheme != NULL) {
                struct authsvc *myAuthsvc;
                ThisUser->authSchemeInx =
                  matchAuthByKeyWd (ThisMdasEnvData->authScheme, 
		 &myAuthsvc);
            }
	}
    } else {
    	conn->proxyUser = (userInfo *) malloc (sizeof (userInfo));
	dupUserInfo (conn->proxyUser, ThisUser);

	if (proxyUserFlag > 0) {
	    /* force clientUser same as proxyUser */
            conn->clientUser = (userInfo *) malloc (sizeof (userInfo));
	    dupUserInfo (conn->clientUser, ThisUser);
	} else {
	    if (ClientUser != NULL) {
    	        conn->clientUser = (userInfo *) malloc (sizeof (userInfo));
		dupUserInfo (conn->clientUser, ClientUser);
	    } else {
	        conn->clientUser = NULL;
	    }
	}
    }

    clSetHost (conn, ThisMdasEnvData, srbHost, srbPort);

    conn->srbVersion = strdup(SRB_VERSION);

    /* set the authentication scheme */

    status = svrSetAuthScheme (conn, ThisMdasEnvData, hostTabPtr);
    if (status < 0) {
        conn->status = status;
        return (conn);
    }

#ifdef foo
    if (ClientUser->authSchemeInx != GSI_DELEGATE_INX) {
	/* if it is GSI_DELEGATE, leave it alone */
        /* make correction authSchemeInx if we are sending to McatHost */

        if ((i = getHostAuthInxFromHostElement (conn->srbHost)) >= 0) 
	status = i;  
    }
    clSetAuthOption (conn, conn->authSchemeInx);
#endif

    conn->status = svrConnectSvr(conn);
    if (conn->status == SYS_ERR_PORTAL_CONN) {
        fprintf (stderr, "svrConnectSvr failed, try again\n");
        conn->status = svrConnectSvr (conn);
        if (conn->status >= 0) {
            fprintf (stderr, "svrConnectSvr succeeded the second time\n");
        }
    }

    return conn;
}

/*
 * svrConnectSvr -
 * Server make a connection to another srbMaster. This is almost the same
 * as connectSvr.
 * Input - srbConn *conn (from clConnect()).
 *
 * Output - returns CLI_CONNECTION_OK if successful. Anything else are failure.
 *
 */
int
svrConnectSvr (srbConn *conn)
{
    struct hostent *hp;

    StartupInfo startup;
    PacketBuf   pacBuf;
#if defined(PORTNAME_c90) || defined(PORTNAME_t3e)
    short       status;
#else
    int         status;
#endif
    MsgType     msgtype;
#if defined(PORTNAME_aix)
    size_t      laddrlen = sizeof(struct sockaddr);
#else
    int         laddrlen = sizeof(struct sockaddr);
#endif
    Port        *port = conn->port;
    int         portno;
    int retBuf;
    int flag;

    int svrPortNum;	/* The srbServer's port number */
    int connectCnt = 0;

#ifdef CONNECT_TIMING
    int initConnTime = time (0);
    elog (NOTICE, "svrConnectSvr: %d at beginning of svrConnectSvr", 
      getpid ()); 
#endif

    /*
     * Initialize the startup packet.
     *
     */

    initStartupPack (conn, &startup);

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
#endif

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

    /* connect to the server  */
    port->sock = socket(AF_INET, SOCK_STREAM, 0);
    /* XXXXX for some reason, we could get sock == 0 */
    if (port->sock == 0) {
	port->sock = socket(AF_INET, SOCK_STREAM, 0);
    }
	
    if (port->sock <= 0) {
        (void) sprintf(conn->errorMessage,
           "connectSvr() -- socket() failed: errno=%d\n%s\n",
            errno, strerror(errno));
        return CLI_ERR_SOCKET;
    }

#ifdef CONNECT_TIMING
    elog (NOTICE, "svrConnectSvr: %d before connect() call ", 
     getpid ());
#endif

    while (connectCnt < MAX_CONN_SVR_CNT) {
        if ((status = connect(port->sock, (struct sockaddr *)&port->raddr,
          sizeof(port->raddr))) < 0) {
           connectCnt ++;
            srbThreadSleep (0, 200000);
        } else {
            break;
        }
    }
#ifdef CONNECT_TIMING
    elog (NOTICE, 
     "svrConnectSvr: %d after connect() call. connectCnt=%d, status=%d",
     getpid (), connectCnt, status);
#endif

    if (status < 0) {
        fprintf(stderr,"connect connect to '%s' on port '%s', errno = %d\n", 
	 conn->srbHost,conn->srbPort, errno);
        (void) sprintf(conn->errorMessage,
        "connectSvr() failed: Is the srbMaster running at '%s' on port '%s'?\n",
        conn->srbHost,conn->srbPort);
#ifdef _WIN32
        closesocket(port->sock);
#else
        close (port->sock);
#endif
        return SVR_TO_SVR_CONNECT_ERROR;
    }

    /* fill in the client address */
    if (getsockname(port->sock, (struct sockaddr *) &port->laddr,
        &laddrlen) < 0) {
#ifdef _WIN32
        closesocket(port->sock);
#else
        close (port->sock);
#endif
        (void) sprintf(conn->errorMessage,
            "connectSvr() -- getsockname() failed: errno=%d\n%s\n",
            errno, strerror(errno));
        return SVR_TO_SVR_CONNECT_ERROR;
    }

#ifdef TCPNODELAYFLAG
    flag = 1;
    status = setsockopt(port->sock,            /* socket affected */
			IPPROTO_TCP,     /* set option at TCP level */
			TCP_NODELAY,     /* name of option */
			(char *) &flag,  /* the cast is historical
					    cruft */
			sizeof(flag));    /* length of option value */
#endif /* TCPNODELAYFLAG */
    /* Connection has been opened */

#ifdef _WIN32
    status = send(port->sock, STARTUP_HEADER, strlen (STARTUP_HEADER) + 1, 0);
#else
    status = write (port->sock, STARTUP_HEADER, strlen (STARTUP_HEADER) + 1);
#endif

#ifdef CONNECT_TIMING
    elog (NOTICE, "svrConnectSvr: %d after writing STARTUP_HEADER",
     getpid ());
#endif

    if (status <= 0) {
#ifdef _WIN32
        closesocket(port->sock);
#else
        close (port->sock);
#endif
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
#endif
    if (status <= 0) {
        sprintf(conn->errorMessage,
          "connectSvr() --  couldn't recv status packet: errno=%d\n%s\n",
            errno,strerror(errno));
        return CLI_ERR_RECV;
    }

#ifdef CONNECT_TIMING
    elog (NOTICE, 
     "svrConnectSvr: %d after reading STARTUP_HEADER reply, status = %d",
     getpid (), status);
#endif

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

#ifdef CONNECT_TIMING
        elog (NOTICE,
         "svrConnectSvr: %d after portalConnect, status = %d",
         getpid (), port->sock);
#endif
        if (port->sock < 0) {
            sprintf(conn->errorMessage,
              "connectSvr(): Connect to srbServer: %s, status = %d, errno=%d\n",
                conn->srbHost, port->sock, errno);
            return (port->sock);
        }
    } 
    /* If svrPortNum == 0, connect to the same port */

    conn->port = port;

#ifdef PORTNAME_solaris
    /* This tries to get around the problem of JAVA VM automatically set
     * any I/O to O_NONBLOCK.
     */
    flag = fcntl (port->sock, F_GETFL);
    fcntl (port->sock, F_SETFL, (flag & (~O_NONBLOCK)));
#endif

    msgtype = STARTUP_MSG;
    startup2PacketBuf(&startup, &pacBuf);
    pacBuf.msgtype = htonl(msgtype);
    status = clPacketSend (port, &pacBuf, sizeof(PacketBuf));

#ifdef CONNECT_TIMING
        elog (NOTICE,
         "svrConnectSvr: %d after clPacketSend, status = %d",
         getpid (), status);
#endif

    if (status == STATUS_ERROR) {
#ifdef _WIN32
        closesocket(port->sock);
#else
        close (port->sock);
#endif
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
#endif

#ifdef CONNECT_TIMING
        elog (NOTICE,
         "svrConnectSvr: %d after reply to clPacketSend, status = %d",
         getpid (), status);
#endif

    if (status <= 0) {
        sprintf(conn->errorMessage,
          "connectSvr() --  couldn't recv status packet: errno=%d\n%s\n",
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

    if ((status = svrSendAuth(&startup, conn)) != STATUS_OK) {
      return status;
    }

#ifdef CONNECT_TIMING
        elog (NOTICE,
         "svrConnectSvr: %d after svrSendAuth, status = %d connect time = %d",
         getpid (), status, time (0) - initConnTime);
#endif

    /* Set up the socket */
#ifdef _WIN32
    /* fout and fin are not used in NT. */
#else
    conn->Pfout = fdopen(port->sock, "w");
    conn->Pfin = fdopen(dup(port->sock), "r");
    if (!conn->Pfout || !conn->Pfin) {
        (void) sprintf(conn->errorMessage,
        "connectSvr() -- fdopen() failed: errno=%d\n%s\n",
        errno, strerror(errno));
        return CLI_ERR_FDOPEN;
    }
#endif

#ifdef PORTNAME_solaris
    /* This tries to get around the problem of JAVA VM automatically set
     * any I/O to O_NONBLOCK.
     */
    flag = fcntl (fileno(conn->Pfin), F_GETFL);
    fcntl (fileno(conn->Pfin), F_SETFL, (flag & (~O_NONBLOCK)));
#endif

    conn->sock = port->sock;

    /* initialize the conn buffer */

    conn->commBuf = (comBuf *) malloc (sizeof (comBuf));
    conn->commBuf->inpInx1 = 0;
    conn->commBuf->inpInx2 = 0;
    conn->commBuf->outInx = 0;

    return CLI_CONNECTION_OK;
}

/*
 * svrSendAuth -- Server send the authentication info to the another server
 * Very similar to clSendAuth.
 */
int
svrSendAuth(StartupInfo *startup, srbConn *conn)
{
    int status;
    int i;

    if (strcmp (startup->proxyUserName, TICKET_USER) == 0 ) {
        status = 0;
    } else {

#ifdef foo 
    	int authIndex;

	/* If the the connection is for a Mcat enabled server, use
	 * the auth info from McatServerAuthInfo */
	if (isMcatHost (conn->srbHost) > 0) {
	    authIndex = McatServerAuthInfo->authsvc->index;
        } else {
	    authIndex = DefServerAuthInfo->authsvc->index;
	}
	conn->authSchemeInx = authIndex;
#endif

        switch (conn->authSchemeInx) {
	case PASSWD_AUTH_INX:
	  status = clSendMdasAuth(startup, conn);
	  break;
	case SEA_AUTH_INX:
	case SEA_ENCRYPT_INX:
	  status = clSendSeaAuth (startup, conn);
	  break;
	case GSI_AUTH_INX:
	case GSI_SECURE_COMM_INX:
	  status = svrSendGsiAuth (startup, conn);
	  break;
	case AUTH_ENCRYPT1_INX:
	  status = clSendEncryptAuth(startup, conn);
	  break;
      case GSI_DELEGATE_INX:
	if (ProxyUser == NULL || ProxyUser->myComm == NULL ||
         ProxyUser->myComm->delegateCertInx < 0) {
            fprintf (stderr,
             "svrSendAuth: ProxyUser->myComm->commFd not initialized %d\n");
            return (AUTH_ERR_GSI_AUTH);
        }
        status = svrSendGsiDelegAuth (ProxyUser->myComm->delegateCertInx, conn);
        break;
	default:
	  status = CLI_AUTH_SCHEME_NOT_SUPPORTED;
	  break;
        }
    }
    if (status != CLIENT_AUTH_REQUIRED) 
	return(status);
	
    i = 0;
    while (status == CLIENT_AUTH_REQUIRED) {
	if (i > 0)
	    break;
	i ++;
	
        if (ProxyUser == NULL || ProxyUser->myComm == NULL) {
            svrSendStatus (conn->port->sock, CLIENT_CROSS_ZONE_AUTH_FAILED);
            status = CLIENT_CROSS_ZONE_AUTH_FAILED;
        } else if (ProxyUser->myComm->delegateCertInx >= 0) {
            svrSendStatus (conn->port->sock, GSI_DELEGATE_INX);
            status = svrSendGsiDelegAuth (ProxyUser->myComm->delegateCertInx, 
	     conn);
        } else {
	    int cliSock, svrSock;

	    cliSock = ProxyUser->myComm->commFd;
	    svrSock = conn->port->sock;

	    status = initCliSvrAuthLink (cliSock); 
	    if (status == AUTH_ENCRYPT1_INX) {
	        /* try encrypt1 */
                svrSendStatus (svrSock, AUTH_ENCRYPT1_INX);
	        status = svrCliSvrEncryptAuth (svrSock, cliSock); 
	    } else if (status == GSI_DELEGATE_INX) {
		status = chkGsiDelegAuth (cliSock, 
		 ProxyUser->userName, ProxyUser->domainName);
		if (status >= 0) {
		    /* loop through again to do svrSendGsiDelegAuth */
		    status = CLIENT_AUTH_REQUIRED;
		}
	    } else {
		fprintf (stderr,
		 "svrSendAuth: Auth inx %d from client not supported\n",
		 status);
		if (status >= 0)
		    status = CLIENT_CROSS_ZONE_AUTH_FAILED;
	    }
        }
    }

    return(status);
}


int
svrSendGsiAuth (StartupInfo *startup, srbConn *conn)
{
#ifdef GSI_AUTH
    Port        *port = conn->port;
    int status;
    char *srbServerDn;

    /* set up the GSI auth */

    if (aid_setup_creds (NULL, GSI_CLIENT_FLAG, GSI_TYPE_FLAG) != 0) {
        return (AUTH_ERR_GSI_SETUP);
    }

    /* Auth with the SRB server */

#ifdef foo
    if ((srbServerDn = svrGetServerDn (conn, conn->srbHost, GSI_AUTH_INX))
     == NULL) {
        return (AUTH_ERR_NO_GSI_SERVER_DN);
    }
#endif
    if ((srbServerDn = conn->dn) == NULL) {
        return (AUTH_ERR_NO_GSI_SERVER_DN);
    }

    if (aid_establish_context_clientside (port->sock, srbServerDn,
     NO_DELEG_FLAG, GSI_TYPE_FLAG) != 0) {
        return (AUTH_ERR_GSI_AUTH);
    }

    status = clRecvStatus (port->sock, conn);

    return (status);

#else
    return AUTH_ERR_GSI_NO_SUPPORT;
#endif
}

#ifdef foo	/* moves to clAuth.c */
int
svrSendGsiDelegAuth (StartupInfo *startup, srbConn *conn)
{
#ifdef GSI_AUTH
    Port        *port = conn->port;
    int status;
    char srbServerDn[MAX_TOKEN];

    /* set up the GSI auth */

    if (aid_setup_creds (NULL, GSI_CLIENT_FLAG, GSI_TYPE_FLAG) != 0) {
        return (AUTH_ERR_GSI_SETUP);
    }

#ifdef foo
    /* Auth with the SRB server */

    if ((srbServerDn = clGetServerDn (conn, conn->srbHost, GSI_AUTH_INX))
     == NULL) {
        return (AUTH_ERR_NO_GSI_SERVER_DN);
    }
#endif

    if (ProxyUser == NULL || ProxyUser->myComm == NULL ||
     ProxyUser->myComm->delegateCertInx < 0) {
        fprintf (stderr, 
	 "svrSendGsiDelegAuth: ProxyUser->myComm->commFd not initialized %d\n");
        return (AUTH_ERR_GSI_AUTH);
    }

    if (aid_establish_context_using_client_delegate (port->sock,
     NULL, srbServerDn, MAX_TOKEN, ProxyUser->myComm->commFd,
     DO_DELEG_FLAG) != 0) {
        return (AUTH_ERR_GSI_AUTH);
    }

    status = clRecvStatus (port->sock, conn);
    return (status);

#else
    return AUTH_ERR_GSI_NO_SUPPORT;
#endif
}
#endif	/* foo */

char *
svrGetServerDn (srbConn *conn, char *hostAddr, int authInx)
{
    struct serverAuthInfo *tmpServerAuthInfo;
    int status;
    char *serverDn;

#ifdef foo
    if (isMcatHost (hostAddr) > 0) {	/* connect to a MCAT host */
	return (McatServerAuthInfo->serverDn);
    }
#endif
    serverDn = getDnFromHostElement (hostAddr);

    if (serverDn != NULL)
	return (serverDn);

    if (DefServerAuthInfo == NULL)      /* DefServerAuthInfo must be init */
        return NULL;

    if (authInx == DefServerAuthInfo->authsvc->index &&
     strcasecmp (hostAddr, DefServerAuthInfo->hostAddr) == 0)
        return (DefServerAuthInfo->serverDn);

    if (ServerAuthInfo == NULL) {
        status = svrGetMcatServerAuthInfo (conn);
    }

    if (status < 0)
        return (NULL);

    tmpServerAuthInfo = ServerAuthInfo;

    while (tmpServerAuthInfo != NULL) {
        if (authInx == tmpServerAuthInfo->authsvc->index &&
         strcasecmp (hostAddr, tmpServerAuthInfo->hostAddr) == 0)
            return (tmpServerAuthInfo->serverDn);

        tmpServerAuthInfo = tmpServerAuthInfo->next;
    }
    return (NULL);
}

int
svrGetMcatServerAuthInfo (srbConn *conn)
{
   int i;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int selval[MAX_DCS_NUM];
    mdasC_sql_result_struct  *myresult;
    int status;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }

    selval[SERVER_NETPREFIX] = 1;
    selval[USER_AUTH_SCHEME] = 1;
    selval[USER_DISTIN_NAME] = 1;

    if ((status = queryDataInfo (NULL, selval, qval, &myresult, 
     DEF_NUM_ROWS) < 0)) {
        return status;
    }

    if (myresult->row_count <= 0)
        return (MCAT_INQUIRE_ERROR);

    status = procMcatServerAuthInfo (myresult);
    free (myresult);

    return status;
}

/* isMcatHost - return 1 if the input hostName is a Mcat-enabled host.
 * 		return 0 if not.
 */

int
isMcatHost (char *hostName)
{
    struct hostElement  *mcatHostElement;
    int remoteFlag;

#ifdef foo
    mcatHostElement = getMdasEnabledHost ();

    if (mcatHostElement == NULL)
	return 0;

    tmpNElement = mcatHostElement->alias;

    while (tmpNElement != NULL) {

        if (strcasecmp (tmpNElement->hostName, hostName) == 0) {
	    return 1;
        } else {
            tmpNElement = tmpNElement->next;
        }
    }
    return 0;
#endif
    remoteFlag = chkHost (hostName, NULL, &mcatHostElement);
    if (remoteFlag < 0) {
        elog (NOTICE, "isMcatHost: Invalid hostName%s", hostName);
	return INP_ERR_HOST_ADDR;
    } else {
	return (mcatHostElement->mdasEnabled);
    }
}

int
getHostAuthInxFromHostElement (char *hostName)
{
    struct hostElement  *myHostElement;
    int remoteFlag;

    remoteFlag = chkHost (hostName, NULL, &myHostElement);
    if (remoteFlag < 0)
	return INP_ERR_HOST_ADDR;

    if (myHostElement->serverAuthInfo != NULL) {
	return (myHostElement->serverAuthInfo->authsvc->index);
    } else {
	return (INP_ERR_HOST_ADDR);
    }
}

char *
getDnFromHostElement (char *hostName)
{
    struct hostElement  *myHostElement;
    int remoteFlag;

    remoteFlag = chkHost (hostName, NULL, &myHostElement);
    if (remoteFlag < 0)
        return NULL;

    if (myHostElement->serverAuthInfo != NULL) {
        return (myHostElement->serverAuthInfo->serverDn);
    } else {
        return (NULL);
    }
}


/* svrSetAuthScheme - Set the default auth scheme (DefServerAuthInfo) and
 * conn->authSchemeInx for this connection */
/*  XXXXXX - don't use DefServerAuthInfo on server */

int
svrSetAuthScheme (srbConn *conn, struct mdasEnvData *myMdasEnvData, 
struct hostElement  *hostTabPtr)
{
    char *tmpAuthKeyWd = NULL;
    char *tmpAuthDn;
    char *tmpHostAddr;
    struct authsvc *myAuthsvc;
    int status = 0;

#ifdef foo
    if (ProxyUser != NULL && ProxyUser->authSchemeInx == GSI_DELEGATE_INX) {
	conn->authSchemeInx = GSI_DELEGATE_INX;
	return GSI_DELEGATE_INX;
    }

    /* If no input authScheme and DefServerAuthInfo already been 
     * set during the last connect, just use DefServerAuthInfo 
     */

    if (DefServerAuthInfo != NULL && authScheme == NULL) {
        /* DefServerAuthInfo has been set and no authScheme */
        conn->authSchemeInx = DefServerAuthInfo->authsvc->index;
        /* free(tmpAuthKeyWd); */
        return DefServerAuthInfo->authsvc->index;
    }

    if ((tmpAuthKeyWd = getenv (AUTH_KEYWD)) != NULL) {
        /* use the env value to override build in value */
	conn->authSchemeInx = matchAuthByKeyWd (tmpAuthKeyWd, &myAuthsvc);
    } else if (authScheme != NULL) {
        /* use the input value */
        tmpAuthKeyWd = authScheme;
	conn->authSchemeInx = matchAuthByKeyWd (tmpAuthKeyWd, &myAuthsvc);
    } else if (myMdasEnvData != NULL &&
        myMdasEnvData->authScheme[0] != '\0') {
        /* use the value in .MdasEnv file */
        tmpAuthKeyWd = myMdasEnvData->authScheme;
	conn->authSchemeInx = matchAuthByKeyWd (tmpAuthKeyWd, &myAuthsvc);
#endif
    if (hostTabPtr != NULL && hostTabPtr->zoneHostTab != NULL &&
	hostTabPtr->zoneHostTab->serverAuthInfo != NULL) {
	conn->authSchemeInx = 
	 hostTabPtr->zoneHostTab->serverAuthInfo->authsvc->index;
    } else if (ThisUser != NULL && ThisUser->authSchemeInx >= 0) {   
        /* authSchemeInx was set to override */
        conn->authSchemeInx = ThisUser->authSchemeInx;
    } else {
       	/* use the default value */
       	tmpAuthKeyWd = DEFAULT_CLIENT_AUTHSVC;
	conn->authSchemeInx = matchAuthByKeyWd (tmpAuthKeyWd, &myAuthsvc);
    }

    conn->srbOptions = malloc (20);
    sprintf (conn->srbOptions, "%-d", conn->authSchemeInx);

#ifdef GSI_AUTH
    if (hostTabPtr->zoneHostTab->serverAuthInfo->serverDn != NULL) {
            conn->dn = strdup (
             hostTabPtr->zoneHostTab->serverAuthInfo->serverDn);
    } else {
        conn->dn = strdup ("");
    }
#else
    if (conn->authSchemeInx == GSI_AUTH_INX || 
     conn->authSchemeInx == GSI_SECURE_COMM_INX ||
     conn->authSchemeInx == GSI_DELEGATE_INX) {
        fprintf (stderr,
         "fillServerAuthInfo - GSI not built into SRB for authScheme %d \n",
          conn->authSchemeInx);
	tmpAuthKeyWd = DEFAULT_CLIENT_AUTHSVC;
        conn->authSchemeInx = matchAuthByKeyWd (tmpAuthKeyWd, &myAuthsvc);
    }
    conn->dn = strdup ("");
#endif
#ifdef foo
    if (DefServerAuthInfo == NULL) {
        /* don't use the host in conn->srbHost for the Default. Use the
         * srbHost, AuthDn pair either through env or the .MdasEnv file.
         */
        if ((tmpAuthDn = getenv (AUTH_SERVER_DN_KEYWD)) == NULL) {
            if (myMdasEnvData != NULL)
                tmpAuthDn = myMdasEnvData->authServerDn;
	    else
		tmpAuthDn = "";
        }

        if ((tmpHostAddr = getenv ("srbHost")) == NULL) {
            if (myMdasEnvData != NULL && myMdasEnvData->host[0] != '\0')
                tmpHostAddr = myMdasEnvData->host;
	    else
		tmpHostAddr = conn->srbHost;
        }

        DefServerAuthInfo = (struct serverAuthInfo *) malloc
         (sizeof (struct serverAuthInfo));
        status = fillServerAuthInfo (tmpHostAddr, tmpAuthKeyWd,
         tmpAuthDn, DefServerAuthInfo);
        if (status < 0) {
            free (DefServerAuthInfo);
            DefServerAuthInfo = NULL;
        }
    }
#endif

    return (status);
}

int
svrGetAuthScheme (int mode)
{
    int authSchemeInx;

    if (mode == SERVER_MODE) {
        if (DefServerAuthInfo != NULL) {
	    authSchemeInx = DefServerAuthInfo->authsvc->index;
        } else if (ThisUser != NULL) {
	    authSchemeInx = ThisUser->authSchemeInx;
        } else {
	    authSchemeInx = -1;
        }
    } else {
        if (ProxyUser != NULL) {
            authSchemeInx = ProxyUser->authSchemeInx;
        } else if (ThisUser != NULL && ThisUser->authSchemeInx >= 0) {
	    authSchemeInx = ThisUser->authSchemeInx;
#ifdef foo
	} else if (DefServerAuthInfo != NULL) {
            authSchemeInx = DefServerAuthInfo->authsvc->index;
#endif
        } else {
	    struct authsvc *myAuthsvc;
	    char *tmpAuthKeyWd;

	    tmpAuthKeyWd = DEFAULT_CLIENT_AUTHSVC;
	    authSchemeInx = matchAuthByKeyWd (tmpAuthKeyWd, &myAuthsvc);
        }
    }
    return (authSchemeInx);
}

/*
 *  readCommand() - Read a string from the client the the socket connection.
 *
 * Input : char *inBuf - Location to put the input command.
 *
 * Output : Returns a character ('F' or 'X') indicating the type of
 *          message received.
 *
 */

char
readCommand(svrComm_t *myComm, char *inBuf)
{
    char qtype[2];
    char result;
    int eof_count;
    int authSchemeInx;

    authSchemeInx = svrGetAuthScheme (CLIENT_MODE);
  
    /*
     *  Get input from the client
     */
    (void) strcpy(qtype, "?");
    eof_count = 0;
    if (commGetnchar(myComm, qtype,0,1, authSchemeInx) == EOF) {

        /* XXX - for some reason, commGetnchar() gets a EOF the first time
         * it enters dbx in AIX.
         */
        /*  No longer true (I think) with new comm routines 10/97, so
            just exit */
        if (eof_count <= 50) {
            eof_count++;
	} else {
	    result = 'X';
	    return (result);
	}
    }

    switch(*qtype) {
    case 'F':
        commGetstr(myComm, inBuf, MAX_PARSE_BUFFER, authSchemeInx);
        result = 'F';
        break;

        /*
         *  'X':  client is exiting
         */
    case 'X':
        result = 'X';
        break;

    default:
	if (*qtype != '?')
            elog(NOTICE, "Socket command type %s unknown", qtype);
	result = 'E';
        break;
    }
    return result;
}

char *
getLocalHostNetAddr ()
{
    struct nameElement *tmpAlias;

    if (LocalHostHead == NULL)
	return NULL;

    tmpAlias = LocalHostHead->alias;
    while (tmpAlias != NULL) {
	if (strcmp (tmpAlias->hostName, LOCALHOST) == 0) {
	    tmpAlias = tmpAlias->next;   /* screen out "localhost" */
	    continue;
	} else if (strncmp (LOOP_BACK_ADDR, tmpAlias->hostName, 
	 strlen (LOOP_BACK_ADDR)) == 0) {
            tmpAlias = tmpAlias->next;   /* screen out loopback addr */
            continue;
	} else {
	    char *tmpPtr;
	    int dotCnt;

	    /* a real network addr must have at least 2 '.' */

	    tmpPtr = tmpAlias->hostName;
	    dotCnt = 0;

	    while (*tmpPtr != '\0') {
		if (*tmpPtr == '.') 
		    dotCnt ++;
		tmpPtr ++;
	    }

	    if (dotCnt >= 2)
		return (tmpAlias->hostName);

            tmpAlias = tmpAlias->next;   /* screen out loopback addr */
            continue;
	}
    }
    return (NULL);
}
 
/* checkSameMcat - Check whether the input 2 path are from the same MCAT.
 * Retrun a 0 if they are. Otherwise a -ive is returned.
 */

int
checkSameMcat (char *path1, char *path2)
{

#ifdef FED_MCAT
    int remoteFlag;
    struct hostElement  *hostTabPtr1, *hostTabPtr2;
    char *mcat1, *mcat2;

    mcat1 = getmcatNamefromHint (path1); 
    mcat2 = getmcatNamefromHint (path2);

    if (mcat1 == NULL) {
	if (mcat2 != NULL) {
	    free (mcat2);
	    return (OBJ_ERR_OBJS_FROM_DIFF_MCAT);
	} else {
	    return 0;
	}
    } else if (mcat2 == NULL) {
        if (mcat1 != NULL) {
            free (mcat1);
            return (OBJ_ERR_OBJS_FROM_DIFF_MCAT);
        } else {
            return 0;
	}
    } else if (strcmp (mcat1, mcat2) == 0) {
	return 0;
    }

    /* mcat1 and mcat2 are different. It could be OK if they don't contain
     * prefixes
     */

    remoteFlag = getMdasEnabledHost (mcat1, &hostTabPtr1);

    if (remoteFlag < 0) {
	free (mcat1);
	free (mcat2);
	return remoteFlag;
    }

    remoteFlag = getMdasEnabledHost (path2, &hostTabPtr2);

    if (remoteFlag < 0) {
	free (mcat1);
	free (mcat2);
        return remoteFlag;
    }

    /* inContainerObj and container much be from the same MCAT */

    if (hostTabPtr1 != hostTabPtr2) {
        return OBJ_ERR_OBJS_FROM_DIFF_MCAT;
    } else {
	if (hostTabPtr1->serverAuthInfo != NULL && 
	 strcmp (hostTabPtr1->serverAuthInfo->mcatName, mcat1) == 0) {
	    /* path1 has a prefix */
	    free (mcat1);
            free (mcat2);
	    return OBJ_ERR_OBJS_FROM_DIFF_MCAT;
	} 
	if (hostTabPtr2->serverAuthInfo != NULL &&
	 strcmp (hostTabPtr2->serverAuthInfo->mcatName, mcat2) == 0) {
	    /* path2 has a prefix */
	    free (mcat1);
            free (mcat2);
	    return OBJ_ERR_OBJS_FROM_DIFF_MCAT;
	} 
        free (mcat1);
        free (mcat2);
	return 0;
    }
#else
    return 0;
#endif  /* FED_MCAT */
}

int
remoteTiConnect (struct hostElement  *hostTabPtr)
{
    srbConn* conn;

    if (hostTabPtr->conn != NULL)       /* Already been opened */
        return 0;

    /* Already set to same as client at initialization */
    /* clConnectOpt(SEA_AUTHENT);  for now, do SEA authentication */

    conn = srbConnect (hostTabPtr->alias->hostName, hostTabPtr->portNum, NULL,
      TICKET_USER, TICKET_USER_DOMAIN, NULL, NULL);

    if (clStatus(conn) != CLI_CONNECTION_OK) {
        elog (NOTICE,"Remote connect failed: %s",clErrorMessage(conn));
        /* return -1; */
	if (clStatus(conn) < START_OF_ERR_CODE)
	    /* valid code */
            return clStatus(conn);
	else
	    /* generic code */
	    return (SYS_ERR_REMOTE_CONN);
    }

    hostTabPtr->conn = conn;
    return 0;
}

int
getZoneFlag (char *srcMcatZoneHint, char *destMcatZoneHint)
{
#ifdef FED_MCAT
    int remoteFlag;
    struct hostElement  *srcHostTabPtr, *destHostTabPtr;
    int srcLocalInx, destLocalInx;

    remoteFlag = getMdasEnabledHost (srcMcatZoneHint, &srcHostTabPtr);
    if (remoteFlag < 0) {
        elog (NOTICE,
         "getZoneFlag: getMdasEnabledHost error for %s",
         srcMcatZoneHint);
        return (remoteFlag);
    }

   remoteFlag = getMdasEnabledHost (destMcatZoneHint, &destHostTabPtr);
    if (remoteFlag < 0) {
        elog (NOTICE,
         "getZoneFlag: getMdasEnabledHost error for %s",
         destMcatZoneHint);
        return (remoteFlag);
    }

    if (srcHostTabPtr == LocalMcat)
        srcLocalInx = 1;
    else
        srcLocalInx = 0;

    if ( destHostTabPtr == LocalMcat)
        destLocalInx = 1;
    else
        destLocalInx = 0;

    if (srcLocalInx == 1 && destLocalInx == 1)
        return ALL_LOCAL_ZONE;
    else if (srcLocalInx == 1 && destLocalInx == 0)
        return SRC_LOCAL_ZONE;
    else if (srcLocalInx == 0 && destLocalInx == 1)
        return DEST_LOCAL_ZONE;
    else if (srcHostTabPtr == destHostTabPtr)
        return ALL_SAME_FOREIGN_ZONE;
    else
        return ALL_DIFF_FOREIGN_ZONE;
#else
    return (ALL_LOCAL_ZONE);
#endif
}

#ifdef FED_MCAT
mdasC_sql_result_struct *
getMcatZoneInfo ()
{
    int i;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int selval[MAX_DCS_NUM];
    mdasC_sql_result_struct  *myresult = NULL;
    int status;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }

    selval[ZONE_NAME] = 1;
    selval[ZONE_NETPREFIX] = 1;
    selval[ZONE_ADMIN_AUTH_SCHEME_NAME] = 1;
    selval[ZONE_ADMIN_DISTIN_NAME] = 1;
    selval[ZONE_LOCALITY] = 1;
    selval[ZONE_PORT_NUM] = 1;

    status = queryDataInfo (NULL, selval, (char (*) [MAX_TOKEN]) qval,
     &myresult, 10 * DEF_NUM_ROWS);

    if (status < 0)
        return NULL;

    return myresult;
}

#endif	/* FED_MCAT */

int
dupUserInfo (userInfo *destUserInfo, userInfo *srcUserInfo)
{
    if (destUserInfo != NULL) {
	memset (destUserInfo, 0, sizeof (userInfo));
    } else {
	elog (NOTICE, "dupUserInfo: NULL input for destUserInfo");
        return 0;
    }

    if (srcUserInfo == NULL) {
        elog (NOTICE, "dupUserInfo: NULL input for srcUserInfo");
        return 0;
    }

    if (srcUserInfo->userName != NULL)
        destUserInfo->userName = strdup (srcUserInfo->userName);
    else
        destUserInfo->userName = NULL;
    if (srcUserInfo->domainName != NULL)
        destUserInfo->domainName = strdup (srcUserInfo->domainName);
    else
        destUserInfo->domainName = NULL;
    if (srcUserInfo->userAuth != NULL)
        destUserInfo->userAuth = strdup (srcUserInfo->userAuth);
    else
        destUserInfo->userAuth = NULL;

    if (srcUserInfo->mcatZone != NULL)
        destUserInfo->mcatZone = strdup (srcUserInfo->mcatZone);
    else
        destUserInfo->mcatZone = NULL;

    destUserInfo->privUserFlag = srcUserInfo->privUserFlag;
    destUserInfo->authSchemeInx = srcUserInfo->authSchemeInx;
    destUserInfo->userId = srcUserInfo->userId;

    return (0);
}

char *
getMcatHostHintByGenQuery (genQuery_t *myGenQuery)
{
    int data_grp_name_inx = -1;
    int parent_collection_inx = -1;
    int container_name_inx = -1;
    int i;
    char *qval;
    char *tmpPtr;

    qval = (char *) myGenQuery->qval;

    for (i = 0; i < myGenQuery->qvalCnt; i++) {
	int qvalInx;
	
        qvalInx = myGenQuery->qvalInx[i];
	if (qvalInx == DATA_GRP_NAME) {
            /* XXXXX this is a hack for passing the mcatName */
            int len = strlen ("MCAT_NAME=");

	    tmpPtr = qval + i * MAX_TOKEN;
            if (strncmp ("MCAT_NAME=", tmpPtr, len)  == 0) {
                *tmpPtr = '\0';
		tmpPtr += len;
                return (tmpPtr);
            } else {
                return (tmpPtr);
            }
	} else if (qvalInx == PARENT_COLLECTION_NAME) {
	    tmpPtr = qval + i * MAX_TOKEN;
	    return (tmpPtr);
        } else if (qvalInx == CONTAINER_NAME) {
            tmpPtr = qval + i * MAX_TOKEN;
            return (tmpPtr);
        }
    }
    return (NULL);
}

int
setZoneForHostElement (struct hostElement *myHElement, char *myMcatName)
{
    struct hostElement *tmpHElement;

    if (myHElement->zoneHostTab != NULL)
	return 0;

    tmpHElement = LocalHostHead;

    while (tmpHElement != NULL) {
	if (tmpHElement->mdasEnabled > 0 && 
	  tmpHElement->serverAuthInfo != NULL) {
	    if (strcmp (tmpHElement->serverAuthInfo->mcatName, myMcatName) 
	     == 0) {
		myHElement->zoneHostTab = tmpHElement;
		return 0;
	    }
	}
	tmpHElement = tmpHElement->next;
    }

    tmpHElement = RemoteHostHead;

    while (tmpHElement != NULL) {
        if (tmpHElement->mdasEnabled > 0 && 
          tmpHElement->serverAuthInfo != NULL) {
            if (strcmp (tmpHElement->serverAuthInfo->mcatName, myMcatName) 
             == 0) {
                myHElement->serverAuthInfo = tmpHElement->serverAuthInfo;
                return 0;
            }
        }
        tmpHElement = tmpHElement->next;
    }

    fprintf (stderr, 
     "setZoneForHostElement: Unable of match a MCAT for zone %s and host %s\n",
      myMcatName, myHElement->alias->hostName);
    
    return (ZONE_NAME_NOT_FOUND);
}

#ifdef foo
int 
parseFreespace (char *inpStr, srb_long_t *freeSpace, time_t *freeSpaceTime)
{
    int iFreeSpace;

    if (freeSpace == NULL || freeSpaceTime == NULL) {
	if (freeSpace != NULL) {
	    *freeSpace = RES_NO_SPACE_DATA;
	}
	return (RES_NO_SPACE_DATA);
    }

    if (inpStr == NULL) {
	*freeSpace = RES_NO_SPACE_DATA;
	*freeSpaceTime = 0;
	return (RES_NO_SPACE_DATA);
    }

    if (strstr (inpStr, "&") == NULL) {
        *freeSpace = RES_NO_SPACE_DATA;
        *freeSpaceTime = 0;
        return (RES_NO_SPACE_DATA);
    }

    if (sscanf (inpStr, "%d&%d", &iFreeSpace, freeSpaceTime) != 2) {
        *freeSpace = RES_NO_SPACE_DATA;
        *freeSpaceTime = 0;
        return (RES_NO_SPACE_DATA);
    }

    *freeSpace = iFreeSpace * MBYTES;

    return (iFreeSpace);
} 
#endif
