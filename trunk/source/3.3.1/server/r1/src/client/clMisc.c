/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 

/*-------------------------------------------------------------------------
 * clMisc.c  
 * Miscellaneous client functions, mostly for handling socket I/O between the
 * client and the SRB..
 *
 * Reworked 10/97 to replace FILE stream I/O with a little buffering and
 * socket I/O (via seaRead and seaWrite).  This is needed for SEA encryption.
 * New routines are the buf* family (bufgetc, bufputc, bufputs, bufread, 
 * bufungetc, bufflush) replacing getc, putc, etc.
 *
 *-------------------------------------------------------------------------
 */

#include "clMisc.h"

#ifdef _WIN32
#include <windows.h>
#include "Unix2Nt.h"
#include <nt_extra.h>
#include <mmsystem.h>
#endif

#ifdef _WIN32
int win_connect_timeout;
MMRESULT win_connect_timer_id;
#endif

#ifndef _WIN32
#include <setjmp.h>
jmp_buf Jcenv;

void
connectTimeout (int sig)
{
    alarm (0);
    longjmp (Jcenv, 1);
}
#endif  /* _WIN32 */

int
bufgetc(int fd, comBuf *commBuf, int authSchemeInx)
{
    int len;
    if (commBuf->inpInx1 < commBuf->inpInx2) {
      return(commBuf->inbuf[commBuf->inpInx1++]);
    }
    commBuf->inpInx1 = 0;

/* When SEA encryption is enabled, the minimum that seaRead can read
   at a time is 8 characters (an encryption block).  We could read more
   but then more may have to be moved in the bufread function. */
    len = 8;
    commBuf->inpInx2 = srbRead(fd,(char *) commBuf->inbuf,len, authSchemeInx); 
    if (commBuf->inpInx2 <= 0) {
	fprintf (stderr, "PID: %d Client socket read error. errno=%d\n", 
	 getpid (), errno);
	/* close (fd); */
    	return(-1);
    }
    return(commBuf->inbuf[commBuf->inpInx1++]);
}

int 
bufread(int fd, char *buf, int len, comBuf *commBuf, int authSchemeInx)
{
    int i;
    int nrcved;
    int toread;

    if (commBuf->inpInx1 < commBuf->inpInx2) {
        for (i=commBuf->inpInx1;i<commBuf->inpInx2 && i<len ;i++) 
	 *buf++ = commBuf->inbuf[i];
        nrcved = i-commBuf->inpInx1;
        commBuf->inpInx1 = i;
    }
    else {
        nrcved=0;
    }
    toread = len - nrcved;
    do {
        i = srbRead (fd,buf,toread, authSchemeInx);
        if (i>0) {
            buf += i;
	    toread -= i;
	    nrcved += i;
	}
    } while (nrcved<len && i>0);  /* must loop to attempt to fill buffer 
                                   like steam I/O does */
    if (i <= 0) {
	fprintf (stderr, "PID: %d. Client socket read error. errno=%d\n", 
	 getpid (), errno);
	/* close (fd); */
    }
    if (i<0) nrcved=i;
    return(nrcved);
}


int
bufungetc(char c, int fd, comBuf *commBuf)
{
    commBuf->inpInx1--;
    commBuf->inbuf[commBuf->inpInx1]=c;
    return 0;
}

int 
bufputc(char c, int fd, comBuf *commBuf, int authSchemeInx)
{
    if (commBuf->outInx==COMM_BUF_SIZE) {
        if (bufflush(fd, commBuf, authSchemeInx)) return(-1);
    }
    commBuf->outbuf[commBuf->outInx++]=c;
    return 0;
}

int 
bufputs(char *s, int fd, comBuf *commBuf, int authSchemeInx)
{
   while (commBuf->outInx < COMM_BUF_SIZE && *s) {
        commBuf->outbuf[commBuf->outInx++]=*s++;
        if (commBuf->outInx==COMM_BUF_SIZE) {
            if (bufflush(fd, commBuf, authSchemeInx)) return(-1);
	}
    }
    return (0);    
}

int 
bufflush(int fd, comBuf *commBuf, int authSchemeInx)
{
    int i;
    if (commBuf->outInx > 0) {
        i = srbWrite(fd,commBuf->outbuf,commBuf->outInx, authSchemeInx);
	if (i <= 0) 
	    fprintf (stderr, "PID: %d. Client socket write error. errno=%d\n", 
	      getpid (), errno);
        if (i != commBuf->outInx) return(-1);
    }
    commBuf->outInx = 0;
    return 0;
}


/* clGetc: Get a character from stream fin. If stream debug is set, also 
 * echo the character in stream debug.
 * Input - fd - the socket.
 * Output - The byte read in integer.  */

int
clGetc(int fd, comBuf *commBuf, int authSchemeInx)
{
    int c;

    c = bufgetc(fd, commBuf, authSchemeInx);
    return c;
}

/* clPutnchar: Send a string of exactly len length into socket fd. 
 * Input : char* s - The string to output.
 *	   int len - The length to output.
 *	   int fd -  The output socket.
 *
 * Output - returns 1 if there was an error, 0 otherwise.
 */

int
clPutnchar(char* s, int len, int fd, comBuf *commBuf, int authSchemeInx)
{
    int status;

    if (fd == 0)
	return 1;

    if (s == NULL)
	s = "";

    if (len <= CHAR_CUTOFF_LEN) {
    	while (len--) {
            status = bufputc(*s,fd, commBuf, authSchemeInx);
            s++;

            if (status == EOF)
            	return 1;
        }
    } else {
      /* new call to write */
        bufflush(fd, commBuf, authSchemeInx);
  	if ((status = srbWrite(fd, s, len, authSchemeInx)) != len) {
        if (status <= 0)
            fprintf (stderr, "PID: %d. Client socket write error. errno=%d\n", 
	     getpid (), errno);

            return 1;
    	}
    	s += len;
    }
    return 0;
}

/* clGetnchar: Get a string of len length from socket fd
 * Input - char* s - The place to put the string.
 *	   int fd - The input socket.
 *
 * Output - Return value - 0 - no error, 1 - error. 
 *	   
 */

int
clGetnchar(char* s, int len, int f, comBuf *commBuf, int authSchemeInx)
{
    int c;

    if (f == 0)
    	return 1;
  
    if (len <= 0)
	return 1;

    if (len <= CHAR_CUTOFF_LEN) {
    	while (len-- && (c = bufgetc(f, commBuf, authSchemeInx)) != EOF)
      	    *s++ = c;
    } else {  
      /* new call to read */
    	if (bufread(f, s, len, commBuf, authSchemeInx) != len) {
      	    return 1;
    	}
    	s += len;
    }

    return 0;
}

/* clGets: Get a string of up to length len from socket fd
 *
 * Input - char* s - The place to put the string.
 *         int  fd - The input socket.
 *
 * Output - Return value - 0 - no error, 1 - error.
 */

int
clGets(char* s, int len, int fd, comBuf *commBuf, int authSchemeInx)
{
    int c;

    if (fd == 0)
    	return 1;
  
    while (len-- && (c = bufgetc(fd, commBuf, authSchemeInx)) != EOF && c)
    	*s++ = c;

    *s = '\0';

    return 0;
}


/* clPutInt () - Send an integer of up to 4 bytes to the output socket.
 * doing this one byte at at time. 
 * This insures that machines with different ENDIAN can talk to each other.
 * Input - int i - The integer to send.
 *	   int bytes - No. of bytes to send (<= 4)
 *
 * Output - Returns 0 if successful, 1 otherwise
 */

int
clPutInt(int i, int bytes, int fd, comBuf *commBuf, int authSchemeInx)
{
    int status;

    if (bytes > SIZEOF32)
	bytes = SIZEOF32;

    while (bytes--) {
	status = bufputc(i & 0xff, fd, commBuf, authSchemeInx);
	i >>= 8;
	if (status == EOF) {
	    return 1;
	}
    }
    return 0;
}

/* clPutLong () - Send an 8 bytes integer of up to 4 bytes to the output socket.
 * doing this one byte at at time. 
 * This insures that machines with different ENDIAN can talk to each other.
 * Input - srb_long_t *i - The pointer to the integer to send. NOTE that
 *	   the input is a pointer rather than value.
 *	   int bytes - No. of bytes to send (<= 4)
 *
 * Output - Returns 0 if successful, 1 otherwise
 */

int
clPutLong(srb_long_t *i, int bytes, int fd, comBuf *commBuf, int authSchemeInx)
{
    int status;
    srb_long_t tmpLong;

    tmpLong = *i;

    if (bytes > 2*SIZEOF32)
	bytes = 2*SIZEOF32;

    while (bytes--) {
	status = bufputc(tmpLong & 0xff, fd, commBuf, authSchemeInx);
	tmpLong >>= 8;
	if (status == EOF) {
	    return 1;
	}
    }
    return 0;
}

#if defined(PORTNAME_c90)
/* crGetInt - Get and reconstruct an integer one byte at a time when the 
 * data is sent as a character string.
 * Input - int* i - The place to put the integer.
 *         int bytes - No. of bytes to receive (<= 4)
 *
 * Output - Returns 0 if successful, 1 otherwise
*/
int
crGetInt(int* result, int bytes, int fd, comBuf *commBuf, int authSchemeInx)
{
    int c;
    int n;
    int origBytes;

    if (fd == NULL)
    	return 1;

    if (bytes > 8)
	return 1;
 
   origBytes = bytes;

    n = 0;
    while (bytes && (c = bufgetc(fd, commBuf, authSchemeInx)) != EOF) {
      	n |= (c & 0xff) << 8 * (bytes - 1);
      	bytes--;
    }

    if (bytes != 0)
    	return 1;

    if (origBytes <= 4)
	n = sToL (n);

    *result = n;

    return 0;
}

#endif	/* PORTNAME_c90 */

/*  - Get and reconstruct an integer one byte at a time.
 * This insures that machines with different ENDIAN can talk to each other
 * Input - int* i - The place to put the integer.
 *         int bytes - No. of bytes to receive (<= 4)
 *
 * Output - Returns 0 if successful, 1 otherwise
 */
int
clGetInt(int* result, int bytes, int fd, comBuf *commBuf, int authSchemeInx)
{
    int c;
    int p;
    int n;
#if defined(PORTNAME_c90)
    int origBytes = bytes;
#endif

    if (fd == 0)
    	return 1;
  
    p = 0;
    n = 0;
    while (bytes && (c = bufgetc(fd, commBuf, authSchemeInx)) != EOF) {
      	n |= (c & 0xff) << p;
      	p += 8;
      	bytes--;
    }

    if (bytes != 0)
    	return 1;

#if defined(PORTNAME_c90)
    if (origBytes <= 4)
	n = sToL (n);
#endif

    *result = n;

    return 0;
}

/*  - Get and reconstruct a long long integer one byte at a time.
 * This insures that machines with different ENDIAN can talk to each other
 * Input - int* i - The place to put the integer.
 *         int bytes - No. of bytes to receive (<= 8)
 *
 * Output - Returns 0 if successful, 1 otherwise
 */
int
clGetLong (srb_long_t* result, int bytes, int fd, comBuf *commBuf, 
int authSchemeInx)
{
    srb_long_t c;
    int p;
    srb_long_t n;

    if (fd == 0)
    	return 1;
  
    p = 0;
    n = 0;
    while (bytes && (c = bufgetc(fd, commBuf, authSchemeInx)) != EOF) {
      	n |= (c & 0xff) << p;
      	p += 8;
      	bytes--;
    }

    if (bytes != 0)
    	return 1;

    *result = n;

    return 0;
}

/* clPuts: Send a NULL terminated string to socket fd.
 * Input : char* s - The string to output.
 *         int fd  - The output socket.
 *
 * Output - returns 1 if there was an error, 0 otherwise.
 */

int
clPuts(char* s, int fd, comBuf *commBuf, int authSchemeInx)
{
    if (fd == 0)
    	return 1;
  
    if (s == NULL)
        s = "";

    if (bufputs(s,fd, commBuf, authSchemeInx) == EOF)
    	return 1;

    bufputc('\0',fd, commBuf, authSchemeInx);
    bufflush(fd, commBuf, authSchemeInx);

    return 0;
}

/* clFlush: Flush the socket fd.
 *         int fd - The socket.
 *
 * Output - Returns 0
 *                 
 */

int
clFlush(int fd, comBuf *commBuf, int authSchemeInx)
{
    if (fd)
	bufflush(fd, commBuf, authSchemeInx);
    return 0;
}

/*
 * getNextStr - Get one string out of the socket. A string is Defined as
 * the next string of non-whitespace . After we get the string, continue
 * reading until EOF, end of line or the next string.
 *
 * Input : int fd - The file stream
 *         char *buf - The buffer to put the stream
 *         int bufsz - The buffer size
 *
 * Output : Returns EOF - EOF reached
 *                  \n - reached a \n
 *                  0 - All other cases.
 */

int
getNextStr(FILE *fp, char *buf, int bufsz)
{
    int c;
    char *eb = buf+(bufsz-1);

    /* Discard inital whitespace */
    while (isspace(c = getc(fp))) ;

    /* EOF before any string, returns EOF */
    if (c == EOF) return -1;

    do {
        if (buf < eb) *buf++ = c;
        c = getc(fp);
    } while (!isspace(c) && c != EOF);
    *buf = '\0';

    /* Discard trailing tabs and spaces */
    while (c == ' ' || c == '\t') c = getc(fp);

    /* Put back the char that was non-whitespace */
    (void) ungetc(c, fp);

    return (c == '\n' ? '\n' : 0);
}

int clUnixErrConv (int status)
{

    if (status < UNIX_ERR_START && status > UNIX_ERR_END) {
        return (UNIX_UNKNOWN_ERR);
    }

    return (UNIX_ERR_PREFIX - status);
}

int
parseContainerName (char *inContainerName, char *outContainerName,
char *collectionName, char *userName, char *domainName, char *mcatName)
{
    char tmpStr[MAX_TOKEN];

    if (inContainerName[0] == '/') {    /* full path */
        if (splitStrByLastChar (inContainerName, collectionName,
         outContainerName, '/') == NULL || inContainerName[0] != '/') {
            outContainerName[0] = collectionName[0] = 0;
            return (OBJ_ERR_BAD_CONTAINER_NAME);
        } else {
            return (0);
        }
    }

#ifdef FED_MCAT
    if (mcatName != NULL && strlen (mcatName) > 0) {
        if (splitStrByLastChar (inContainerName, tmpStr, outContainerName, '/')
	 == NULL) {            /* simple form */
            strcpy (outContainerName, inContainerName);
            sprintf (collectionName, "/%s%s/%s.%s", mcatName, CONTAINER_HOME,
             userName, domainName);
        } else {
            sprintf (collectionName, "/%s%s/%s", mcatName, CONTAINER_HOME, 
	     tmpStr);
        }
    } else {
        if (splitStrByLastChar (inContainerName, tmpStr, outContainerName, '/') 
         == NULL) {            /* simple form */
            strcpy (outContainerName, inContainerName);
            sprintf (collectionName, "%s/%s.%s", CONTAINER_HOME,
             userName, domainName);
        } else {
            sprintf (collectionName, "%s/%s", CONTAINER_HOME, tmpStr);
        }
    }
#else
    if (splitStrByLastChar (inContainerName, tmpStr, outContainerName, '/') ==
     NULL) {            /* simple form */
        strcpy (outContainerName, inContainerName);
        sprintf (collectionName, "%s/%s.%s", CONTAINER_HOME,
         userName, domainName);
    } else {
        sprintf (collectionName, "%s/%s", CONTAINER_HOME, tmpStr);
    }
#endif

    return (0);
}

/* myIsContainer - given the ojbID and collection, check whether it is a
 * container obj. It is a container if objID/collection == containerName.
 */
int
myIsContainer (char *objID, char *collection, char *containerName)
{
    char fullContName[MAX_TOKEN];

    sprintf (fullContName, "%s/%s", collection, objID);
    if (strcmp (containerName, fullContName) != 0)
        return 0;       /* not a container */
    else
        return 1;       /* a container */
}

char *
splitStrByLastChar (char *st1, char *st2, char *st3, char key)
{
    int inLen;
    int tmpLen;

    inLen = tmpLen = strlen (st1);
    if (inLen <= 0)
        return NULL;

    while (tmpLen >= 0 && st1[tmpLen] != '/')
            tmpLen --;

    if (tmpLen < 0)
        return NULL;

    if (tmpLen == 0)
        st2[0] = '\0';
    else {
        strncpy (st2, st1, tmpLen);
        st2[tmpLen] = '\0';
    }

    strcpy (st3, &st1[tmpLen + 1]);
    return &st1[tmpLen + 1];
}

#if defined(PORTNAME_c90)
/* sToL - routine to convert 32 bit int to 64 bit.
 */

int sToL (int shortInt)
{
    int mask = 0xffffffff;
    int short1;
    int result;

    if (shortInt < 0)
	return (shortInt);

    short1 = shortInt & mask;

    if ((short1 & 0x80000000) > 0) {
        result = (short1 - mask - 1);
    } else {
        result = short1;
    }
    return (result);
}
#endif

int
srbRead (int fd, char *myInbuf,int len, int authSchemeInx)
{
    int status;
    int intrCnt = 0;

  while (intrCnt < 100) {
    switch (authSchemeInx) {
          case PASSWD_AUTH_INX:
#ifdef _WIN32
			status = recv(fd, myInbuf, len, 0);
#else
            status = read (fd, myInbuf, len);
#endif /* WIN32 */
            break;
          case SEA_AUTH_INX:
          case SEA_ENCRYPT_INX:
#ifdef SEA_AUTH
            status = seaRead (fd, myInbuf, len);
#else
            status = CLI_AUTH_SCHEME_NOT_SUPPORTED;
#endif
            break;
        case GSI_AUTH_INX:
#ifdef _WIN32
			status = recv(fd, myInbuf, len, 0);
#else
            status = read (fd, myInbuf, len);
#endif /* WIN32 */
            break;
        case GSI_SECURE_COMM_INX:
#ifdef GSI_AUTH
            status = aid_read (fd, myInbuf, len);
#else
            status = CLI_AUTH_SCHEME_NOT_SUPPORTED;
#endif
            break;
          default:
#ifdef _WIN32
			status = recv(fd, myInbuf, len, 0);
#else
            status = read (fd, myInbuf, len);
#endif /* WIN32 */
    }
    if (status <= 0) {
	if (errno == EINTR) {
	    intrCnt++;
	    continue;
	}
    }
    break;
  }	    
  return (status);
}

int
srbWrite (int fd, char *outbuf,int len, int authSchemeInx)
{
    int status;

    switch (authSchemeInx) {
          case PASSWD_AUTH_INX:
#ifdef _WIN32
			status = send(fd, outbuf, len, 0);
#else
            status = write (fd, outbuf, len);
#endif /* WIN32 */
            break;
          case SEA_AUTH_INX:
          case SEA_ENCRYPT_INX:
#ifdef SEA_AUTH
            status = seaWrite (fd, outbuf, len);
#else
            status = CLI_AUTH_SCHEME_NOT_SUPPORTED;
#endif
            break;
        case GSI_AUTH_INX:
#ifdef _WIN32
			status = send(fd, outbuf, len, 0);
#else
            status = write (fd, outbuf, len);
#endif /* WIN32 */
            break;
        case GSI_SECURE_COMM_INX:
#ifdef GSI_AUTH
            status = aid_write (fd, outbuf, len);
#else
            status = CLI_AUTH_SCHEME_NOT_SUPPORTED;
#endif
            break;
          default:
#ifdef _WIN32
			status = send(fd, outbuf, len, 0);
#else
            status = write (fd, outbuf, len);
#endif /* WIN32 */
    }
    return (status);
}

/* getNextLine - Read the next line. Discard comment (start with '#'
 */

int
getNextLine(FILE *fp, char *buf, int bufsz)
{
    int c;
    int len = 0;
    char *cptr = buf;

    while ((c = getc(fp)) != EOF) {
	/* Skip comment */
	if (c == '#') {
	    while ((c = getc (fp)) != '\n');
	    if (len > 0)
		break;
	    else
		continue;
	}
	if (c == '\n')
	    break; 
	*cptr ++ = c;
	len ++;

	/* over flow buf ? */
	if (len >= bufsz - 1) {
	    break;
	}

    }
    if (c == EOF && len == 0)
	return EOF;
    else {
        *cptr ++ = '\0';
        len ++;
	return len;
    }
}

/* parseAddr - Parse the address with format host:port.
 * Input : char *hostAddr
 *
 * Output : Return the portNum if exists. Otherwise, returns DefaultPort.
 */

char *
parseAddr (char *hostAddr)
{
    char *portNum = NULL;
    char *cptr;

    cptr =  hostAddr;

    if (hostAddr == NULL)
        return NULL;

    while (*cptr != '\0') {
        if (*cptr == ':') {
            *cptr = '\0';
            cptr ++;
            if (*cptr != '\0') {
                portNum = cptr;
            }
            break;
        }
        cptr ++;
    }

    /* Some sanity check of portNum */

    if (portNum != NULL) {
        while (*cptr != '\0') {
            if (*cptr == ':') {
                *cptr = '\0';
                break;
            }
            if (!isdigit (*cptr)) {     /* Not a digit */
                portNum = NULL;
                break;
            }
            cptr ++;
        }
    }

    return (portNum);
}

int 
portalCreate () {
    struct sockaddr_in sin;
    int lsock;
    int curPortNum;
    int tryCnt = 0;
    int commPortNumStart = 0;
    int commPortNumCount = 0;

    if ((lsock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return CLI_ERR_SOCKET;
    } else if (lsock == 0) {
        if ((lsock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            return CLI_ERR_SOCKET;
        }
    }

    if (srbSetsockopt (lsock) < 0) {
        fprintf(stderr,"error setting socket Option, errno = %d\n", errno);
    }

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;

#ifdef COMM_PORT_NUM_START
    commPortNumStart = COMM_PORT_NUM_START;
#ifdef COMM_PORT_NUM_COUNT
    commPortNumCount = COMM_PORT_NUM_COUNT;
#endif
#endif

    if (getenv (COMM_PORT_NUM_START_KETWORD) != NULL) {
	commPortNumStart = atoi (getenv (COMM_PORT_NUM_START_KETWORD));
    }
    if (getenv (COMM_PORT_NUM_COUNT_KETWORD) != NULL) { 
	commPortNumCount = atoi (getenv (COMM_PORT_NUM_COUNT_KETWORD));
    }

    if (commPortNumStart > 0) {
        /* try to bind to a port number that is in range */

	if (commPortNumCount <= 0) {
	    commPortNumCount = DEF_COMM_PORT_NUM_COUNT;
	}

        if (RandomSeed == 0) {
            srandom ((unsigned int) time(0) / getpid());
            RandomSeed = 1;
        }

        curPortNum = commPortNumStart + random() % commPortNumCount;

        while (1) {

	    if (curPortNum >= commPortNumStart + commPortNumCount)
	        curPortNum = commPortNumStart;

            sin.sin_port = htons (curPortNum);

            /* bind the addr */

            if (bind(lsock, (struct sockaddr *)&sin, sizeof sin) >= 0) {
                break;
            }
#ifndef _WIN32
	    if (errno != EADDRINUSE && errno != EADDRNOTAVAIL) {
                return CLI_ERR_SOCKET;
	    }
#endif

	    if (tryCnt >= commPortNumCount)
	        return CLI_ERR_SOCKET;

	    curPortNum ++;
	    tryCnt ++;
        }
    } else {
        sin.sin_port = 0;

        /* bind the addr */

        if (bind(lsock, (struct sockaddr *)&sin, sizeof sin) < 0) {
	    return CLI_ERR_SOCKET;
        } 
    }
#ifdef TCPNODELAYFLAG
    /** RAJA: added to reduce TCP delay in sending packets: Aug 22, 2001 ***/
    /** No checking of error for the operation is done ***/
    flag = 1;
    result = setsockopt(lsock,            /* socket affected */
			IPPROTO_TCP,     /* set option at TCP level */
			TCP_NODELAY,     /* name of option */
			(char *) &flag,  /* the cast is historical
					    cruft */
			sizeof(int));    /* length of option value */
    if (result != 0) {
      fprintf(stderr,"Error: streamServerPort: setsockopt:TCP_NODELAY failed to set: %d\n",result);
    }
#endif /* TCPNODELAYFLAG */
    return (lsock);


}

int
portalConnect (int port, char *hostName)
{
    struct hostent *hostEnt;
    struct sockaddr_in sin;
    int lsock;
    int clPort, cookie, nbytes;
    int status;
#if defined(PORTNAME_sgi) || defined(PORTNAME_solaris) ||  defined(PORTNAME_linux)
    /* Added by David McNab of NASA to address reentrant gethostbyname bug. */
    struct hostent myHostEnt;
    char workingbuf[MAX_TOKEN];
    int errnop;
#endif

    sin.sin_family = AF_INET;
/*
Take out linux for now. does not work on some linux versions - NULL hostEnt
and connect failed.
#if defined(PORTNAME_sgi) || defined(PORTNAME_solaris) || defined(PORTNAME_linux)
*/
#if defined(PORTNAME_sgi) || defined(PORTNAME_solaris)
#if defined(PORTNAME_sgi) || defined(PORTNAME_solaris)
    if ((hostEnt = gethostbyname_r (hostName, &myHostEnt, workingbuf, MAX_TOKEN,
      &errnop)) == NULL) {
#else
    hostEnt = NULL;
    if ((status = gethostbyname_r (hostName, &myHostEnt, workingbuf, MAX_TOKEN,
      &hostEnt, &errnop)) < 0) {
#endif          /* defined(PORTNAME_sgi) || defined(PORTNAME_solaris) */
#else
    if ((hostEnt = gethostbyname (hostName)) == NULL) {
#endif
        return (OBJ_ERR_SOCKET);
    }

#ifdef _WIN32
	memcpy(&sin.sin_addr,hostEnt->h_addr_list[0],
     hostEnt->h_length);
#else
    bcopy(hostEnt->h_addr_list[0], (caddr_t)&sin.sin_addr,
     hostEnt->h_length);
#endif /* WIN32 */

    lsock = portalConnectWithRaddr (port, &sin);

    return (lsock);

#ifdef foo	/* done in portalConnectWithRaddr */
    clPort = getPortFromPortInp (port);
    cookie = getCookieFromPortInp (port);

    sin.sin_port = htons ((unsigned short) (clPort));


    lsock = socket (AF_INET, SOCK_STREAM, 0);
    if (lsock < 0) {
        return (OBJ_ERR_SOCKET);
    }

    status = connectWithTimeout (lsock, (struct sockaddr *) &sin);

    if (status < 0)
        return (SYS_ERR_PORTAL_CONN);

    srbSetsockopt (lsock);
    if (cookie != 0) {
 	cookie = htonl (cookie);
#ifdef _WIN32
         nbytes = send(lsock, (char *)&cookie, sizeof (cookie), 0);
#else
         nbytes = write (lsock, &cookie, sizeof (cookie));
#endif

	if (nbytes != sizeof (cookie)) {
#ifdef _WIN32
        closesocket(lsock);
#else
	    close (lsock);
#endif
	    lsock = SYS_ERR_PORTAL_COOKIE_SEND;
	}
    }
    return (lsock);
#endif	/* foo */
}

int
portalConnectWithRaddr (int port, struct sockaddr_in  *raddr)
{
    int lsock;
    int clPort, cookie, nbytes;
    int status;


    clPort = getPortFromPortInp (port);
    cookie = getCookieFromPortInp (port);

    raddr->sin_port = htons((unsigned short)(clPort));

    lsock = socket (AF_INET, SOCK_STREAM, 0);
    /* XXXXX for some reason, we could get sock == 0 for server */
    if (lsock == 0) {
        lsock = socket(AF_INET, SOCK_STREAM, 0);
    }

    if (lsock <= 0) {
	fprintf (stderr,
         "portalConnectWithRaddr() - socket() failed: sock=%d, errno=%d\n%s\n",
           lsock, errno, strerror(errno));
        return OBJ_ERR_SOCKET;
    }

    status = connectWithTimeout (lsock, (struct sockaddr *) raddr);

    if (status < 0) {
#ifdef _WIN32
        closesocket (lsock);
#else
        close (lsock);
#endif /* WIN32 */
        if (UserClass == CLASS_SER) {       
            return SVR_TO_SVR_CONNECT_ERROR;
        } else {
            return CLI_ERR_SOCK_CONN;
        }
    }

    srbSetsockopt (lsock);
    if (cookie != 0) {
 	cookie = htonl (cookie);
#ifdef _WIN32
         nbytes = send(lsock, (char *)&cookie, sizeof (cookie), 0);
#else
         nbytes = write (lsock, &cookie, sizeof (cookie));
#endif

	if (nbytes != sizeof (cookie)) {
#ifdef _WIN32
            closesocket(lsock);
#else
	    close (lsock);
#endif
	    lsock = SYS_ERR_PORTAL_COOKIE_SEND;
	}
    }
    return (lsock);
}

int 
getPortNum (int lsock)
{
#if defined(PORTNAME_aix)
    size_t length;
#else
    int length;
#endif
    struct sockaddr_in sin;

    length = sizeof (sin);
    if (getsockname (lsock, (struct sockaddr *) &sin, &length)) {
        return CLI_ERR_SOCKET;
    }
    return (ntohs (sin.sin_port));
}

int
parseCollUserMode (mdasC_sql_result_struct *myresult, userInfo *proxyUser, 
userInfo *clientUser, int *outUserId, int *outGid, int *outMode)
{
    char *accessList, *origAccessList;
    char *domainList, *origDomainList, *userList, *origUserList;
    int i;
    char *myuser, *mydomain;
#ifndef _WIN32
    struct passwd * pw;
#endif

    if (myresult == NULL || myresult->row_count <= 0) {
        *outUserId = NOT_LOCAL_USER;
        *outGid = NOT_LOCAL_USER;
        *outMode = 0 | S_IRUSR | S_IWUSR;
        return -1;
    }

    *outUserId = -1;
    *outGid = -1;
    *outMode = 0;

    accessList = origAccessList = (char *) getFromResultStruct(
     (mdasC_sql_result_struct *) myresult,
      dcs_tname[GROUP_ACCESS_CONSTRAINT], dcs_aname[GROUP_ACCESS_CONSTRAINT]);

    userList = origUserList = (char *) getFromResultStruct(
     (mdasC_sql_result_struct *) myresult,
      dcs_tname[USER_NAME], dcs_aname[USER_NAME]);

    domainList = origDomainList = (char *) getFromResultStruct(
     (mdasC_sql_result_struct *) myresult,
      dcs_tname[DOMAIN_DESC], dcs_aname[DOMAIN_DESC]);

    if (clientUser != NULL) {
        myuser = clientUser->userName;
        mydomain = clientUser->domainName;
    } else if (proxyUser != NULL) {
        myuser = proxyUser->userName;
        mydomain = proxyUser->domainName;
    } else {
        myuser = NULL;
        mydomain = NULL;
    }

    if (myuser != NULL) {
        /* find the owner */
        for (i = 0; i < myresult->row_count; i++) {
            if (strcmp (userList, myuser) == 0 &&
             strcmp (domainList, mydomain) == 0) 
			{
#ifndef _WIN32
            	pw = getpwnam (userList);
            	if (pw != NULL) {
                    *outUserId = pw->pw_uid;
                    *outGid = pw->pw_gid;
				} else {
        	    *outUserId = NOT_LOCAL_USER;
        	    *outGid = NOT_LOCAL_USER;
				}
#endif /* WIN32 */

                if (strcmp (accessList, "read") == 0) {
                    *outMode = *outMode | S_IRUSR;
                } else if (strcmp (accessList, "write") == 0 ||
                 strcmp (accessList, "all") == 0) {
                     *outMode = *outMode | S_IRWXU;
                }
                break;
            }
            accessList += MAX_DATA_SIZE;
            userList += MAX_DATA_SIZE;
            domainList += MAX_DATA_SIZE;
        }
    }
    if (*outUserId < 0) {
        /* no match, just use any */
        accessList = origAccessList;
        userList = origUserList;
        domainList = origDomainList;

        for (i = 0; i < myresult->row_count; i++) {
#ifndef _WIN32
            pw = getpwnam (userList);
            if (pw != NULL) {
                *outUserId = pw->pw_uid;
                *outGid = pw->pw_gid;
                if (strcmp (accessList, "read") == 0) {
                    *outMode = *outMode | S_IRUSR;
                } else if (strcmp (accessList, "write") == 0 ||
                 strcmp (accessList, "all") == 0) {
                     *outMode = *outMode | S_IRWXU;
                }
                break;
            }
#endif /* WIN32 */
            accessList += MAX_DATA_SIZE;
            userList += MAX_DATA_SIZE;
            domainList += MAX_DATA_SIZE;
        }
    }
    if (*outUserId < 0) {
        *outUserId = NOT_LOCAL_USER;
        *outGid = NOT_LOCAL_USER;
        if (strcmp (origAccessList, "read") == 0) {
            *outMode = *outMode | S_IRUSR;
        } else if (strcmp (origAccessList, "write") == 0 ||
         strcmp (origAccessList, "all") == 0) {
            *outMode = *outMode | S_IRWXU;
        }
    }
    /* process the world access */

    accessList = origAccessList;
    userList = origUserList;

    for (i = 0; i < myresult->row_count; i++) {
        if (strcmp (userList, PUBLIC_USER) == 0) {
            if (strcmp (accessList, "read") == 0) {
                *outMode = *outMode | S_IROTH | S_IRGRP | S_IRUSR;
            } else if (strcmp (accessList, "write") == 0 ||
             strcmp (accessList, "all") == 0) {
                *outMode = *outMode | S_IRWXO | S_IRWXG | S_IRWXU;
            }
            break;
        }
        userList += MAX_DATA_SIZE;
        accessList += MAX_DATA_SIZE;
    }
    return 0;
}

int
parseDataUserMode (mdasC_sql_result_struct *myresult, userInfo *proxyUser,
userInfo *clientUser, int *outUserId, int *outGid, int *outMode)
{
    char *ownerList, *accessList, *origOwnerList, *origAccessList,
     *ownDomainList, *origOwnDomainList;
    char *domainList, *origDomainList, *userList, *origUserList;
    int i;
    char *myuser, *mydomain;
#ifndef _WIN32
    struct passwd * pw;
#endif

    if (myresult == NULL || myresult->row_count <= 0) {
        *outUserId = NOT_LOCAL_USER;
        *outGid = NOT_LOCAL_USER;
        *outMode = 0 | S_IRUSR | S_IWUSR;
        return -1;
    }

    *outUserId = -1;
    *outGid = -1;
    *outMode = 0;

    ownerList = origOwnerList = (char *) getFromResultStruct(
     (mdasC_sql_result_struct *) myresult,
      dcs_tname[DATA_OWNER], dcs_aname[DATA_OWNER]);

    ownDomainList = origOwnDomainList = (char *) getFromResultStruct(
     (mdasC_sql_result_struct *) myresult,
      dcs_tname[DATA_OWNER_DOMAIN], dcs_aname[DATA_OWNER_DOMAIN]);

    accessList = origAccessList = (char *) getFromResultStruct(
     (mdasC_sql_result_struct *) myresult,
      dcs_tname[ACCESS_CONSTRAINT], dcs_aname[ACCESS_CONSTRAINT]);

    userList = origUserList = (char *) getFromResultStruct(
     (mdasC_sql_result_struct *) myresult,
      dcs_tname[USER_NAME], dcs_aname[USER_NAME]);

    domainList = origDomainList = (char *) getFromResultStruct(
     (mdasC_sql_result_struct *) myresult,
      dcs_tname[DOMAIN_DESC], dcs_aname[DOMAIN_DESC]);

    if (clientUser != NULL) {
        myuser = clientUser->userName;
        mydomain = clientUser->domainName;
    } else if (proxyUser != NULL) {
        myuser = proxyUser->userName;
        mydomain = proxyUser->domainName;
    } else {
        myuser = NULL;
        mydomain = NULL;
    }

    if (myuser != NULL) {
        /* find the owner */
        for (i = 0; i < myresult->row_count; i++) {
            if (strcmp (userList, myuser) == 0 &&
             strcmp (domainList, mydomain) == 0) {
                if (strcmp (ownerList, myuser) == 0 &&
                 strcmp (ownDomainList, mydomain) == 0) {
#ifndef _WIN32
                    pw = getpwnam (userList);
                    if (pw != NULL) {
                    	*outUserId = pw->pw_uid;
                    	*outGid = pw->pw_gid;
		    } else {
                    	*outUserId = NOT_LOCAL_USER;
                    	*outGid = NOT_LOCAL_USER;
                    }

                    if (strcmp (accessList, "read") == 0) {
                        *outMode = *outMode | S_IRUSR;
                    } else if (strcmp (accessList, "write") == 0 ||
                        strcmp (accessList, "all") == 0) {
                            *outMode = *outMode | S_IRWXU;
                    }
                    break;
#endif /* WIN32 */
                } else {
                    /* clientUser have access but is not owner--group access */
                    if (strcmp (accessList, "read") == 0) {
                        *outMode = *outMode | S_IRGRP;
                    } else if (strcmp (accessList, "write") == 0 ||
                        strcmp (accessList, "all") == 0) {
                            *outMode = *outMode | S_IRWXG;
                    }
                }
            }
            ownerList += MAX_DATA_SIZE;
            ownDomainList += MAX_DATA_SIZE;
            accessList += MAX_DATA_SIZE;
            userList += MAX_DATA_SIZE;
            domainList += MAX_DATA_SIZE;
        }
    }
    if (*outUserId < 0) {
        /* no match, just use any */
        ownerList = origOwnerList;
        ownDomainList = origOwnDomainList;
        accessList = origAccessList;
        userList = origUserList;
        domainList = origDomainList;

        for (i = 0; i < myresult->row_count; i++) {
#ifndef _WIN32
            pw = getpwnam (ownerList);
            if (pw != NULL) {
                *outUserId = pw->pw_uid;
                *outGid = pw->pw_gid;
                if (strcmp (accessList, "read") == 0) {
                    *outMode = *outMode | S_IRUSR;
                } else if (strcmp (accessList, "write") == 0 ||
                 strcmp (accessList, "all") == 0) {
                     *outMode = *outMode | S_IRWXU;
                }
                break;
            }
#endif /* WIN32 */
            ownerList += MAX_DATA_SIZE;
            ownDomainList += MAX_DATA_SIZE;
            accessList += MAX_DATA_SIZE;
            userList += MAX_DATA_SIZE;
            domainList += MAX_DATA_SIZE;
        }
    }
    if (*outUserId < 0) {
        *outUserId = NOT_LOCAL_USER;
        *outGid = NOT_LOCAL_USER;
        if (strcmp (origAccessList, "read") == 0) {
            *outMode = *outMode | S_IRUSR;
        } else if (strcmp (origAccessList, "write") == 0 ||
         strcmp (origAccessList, "all") == 0) {
            *outMode = *outMode | S_IRWXU;
        }
    }
    /* process the world access */

    accessList = origAccessList;
    userList = origUserList;

    for (i = 0; i < myresult->row_count; i++) {
        if (strcmp (userList, PUBLIC_USER) == 0) {
            if (strcmp (accessList, "read") == 0) {
                *outMode = *outMode | S_IROTH | S_IRGRP | S_IRUSR;
            } else if (strcmp (accessList, "write") == 0 ||
             strcmp (accessList, "all") == 0) {
                *outMode = *outMode | S_IRWXO | S_IRWXG | S_IRWXU;
            }
            break;
        }
        userList += MAX_DATA_SIZE;
        accessList += MAX_DATA_SIZE;
    }
    return 0;
}

/* checkCollPerm - check whether the client User has the permission 
 * to perform operation on a collection.
 *
 * Input:
 * mdasC_sql_result_struct *myresult - result from a svrCollStat query.
 * int mode - valid modes are: O_RDONLY, O_WRONLY, O_RDWR, O_WR_REPL.
 */
 
int
checkCollPerm (mdasC_sql_result_struct *myresult, userInfo *proxyUser,
userInfo *clientUser, int mode)
{
    char *accessList, *domainList, *userList;
    int i;
    char *myuser, *mydomain;

    if (myresult == NULL || myresult->row_count <= 0) {
        return SRB_EPERM;
    }

    accessList = (char *) getFromResultStruct(
     (mdasC_sql_result_struct *) myresult,
      dcs_tname[GROUP_ACCESS_CONSTRAINT], dcs_aname[GROUP_ACCESS_CONSTRAINT]);

    userList = (char *) getFromResultStruct(
     (mdasC_sql_result_struct *) myresult,
      dcs_tname[USER_NAME], dcs_aname[USER_NAME]);

    domainList = (char *) getFromResultStruct(
     (mdasC_sql_result_struct *) myresult,
      dcs_tname[DOMAIN_DESC], dcs_aname[DOMAIN_DESC]);

    if (clientUser != NULL) {
        myuser = clientUser->userName;
        mydomain = clientUser->domainName;
    } else if (proxyUser != NULL) {
        myuser = proxyUser->userName;
        mydomain = proxyUser->domainName;
    } else {
	return SRB_EPERM;
    }

    for (i = 0; i < myresult->row_count; i++) {
        if ((strcmp (userList, myuser) == 0 && 
	 strcmp (domainList, mydomain) == 0) || 
	  strcmp (userList, PUBLIC_USER) == 0) {
	    if (mode == O_RDONLY) {
		return 0;
            } else if (strcmp (accessList, "read") != 0) {
		return 0; 
	    }
	}
        accessList += MAX_DATA_SIZE;
        userList += MAX_DATA_SIZE;
        domainList += MAX_DATA_SIZE;
    }

    return 0;
}

time_t
srbToLocalTime (char *srbTime)
{
    int status;
    struct tm myTm;
    time_t myTime;

    if (srbTime == NULL)
        return 0;
    status = sscanf (srbTime, "%d-%d-%d-%d.%d.%d",
     &myTm.tm_year, &myTm.tm_mon, &myTm.tm_mday, &myTm.tm_hour, &myTm.tm_min,
      &myTm.tm_sec);

    if (status != 6)
        return -1;

    myTm.tm_year -= 1900;       /* yr since 1900 */
    myTm.tm_mon -= 1;           /* [0,11] */
    myTm.tm_isdst = 1;

    myTime = mktime (&myTm);

    return (myTime);
}

#if defined(PORTNAME_alpha)  /* True64 Alpha PSC port */ /* fake it to make loader happy */
    char *get_home_env() 
      { 
#ifdef _WIN32 
        return getenv("HOMEDRIVE"); 
#else 
        return ((char *)getenv ("HOME")); 
#endif 
      } 
#endif


int 
srbToLocalStat (struct srbStat *mySrbStat, struct stat *statbuf)
{
    if (mySrbStat == NULL || statbuf == NULL)
	return OBJ_ERR_BAD_STAT_INP;

    statbuf->st_dev = mySrbStat->st_dev;
    statbuf->st_ino = mySrbStat->st_ino;
    statbuf->st_mode = mySrbStat->st_mode;
    statbuf->st_nlink = mySrbStat->st_nlink;
    statbuf->st_uid = mySrbStat->st_uid;
    statbuf->st_gid = mySrbStat->st_gid;
    statbuf->st_rdev = mySrbStat->st_rdev;
    statbuf->st_size = mySrbStat->st_size;
    statbuf->st_atime = mySrbStat->st_atim;
    statbuf->st_mtime = mySrbStat->st_mtim;
    statbuf->st_ctime = mySrbStat->st_ctim;

#ifndef _WIN32 
    statbuf->st_blksize = mySrbStat->st_blksize;
    statbuf->st_blocks = mySrbStat->st_blocks;
#endif /* WIN32 */

#if defined(PORTNAME_aix) || defined(PORTNAME_alpha) || defined(PORTNAME_sunos) || defined(PORTNAME_c90)
    statbuf->st_spare1 = mySrbStat->st_spare1;
    statbuf->st_spare2 = mySrbStat->st_spare2;
    statbuf->st_spare3 = mySrbStat->st_spare3;
#endif  /* PORTNAME_aix */
#ifdef PORTNAME_aix
    statbuf->st_vfstype = mySrbStat->st_vfstype;
    statbuf->st_vfs = mySrbStat->st_vfs;
    statbuf->st_type = mySrbStat->st_type;
    statbuf->st_access = mySrbStat->st_Access;
    statbuf->st_flag = mySrbStat->st_flag;
#endif  /* PORTNAME_aix */
#if defined(PORTNAME_aix) || defined(PORTNAME_alpha)
    statbuf->st_gen =  mySrbStat->st_gen;
#endif  /* defined(PORTNAME_aix) || defined(PORTNAME_alpha) */
#if defined(PORTNAME_solaris)
    strncpy (statbuf->st_fstype, mySrbStat->st_fstype, _ST_FSTYPSZ - 1);
    statbuf->st_fstype[_ST_FSTYPSZ - 1] = '\n';
#endif  /* PORTNAME_solaris */

    return 0;
}

#if !defined(PORTNAME_c90)
int 
srbToLocalStat64 (struct srbStat *mySrbStat, struct stat64 *statbuf)
{
    if (mySrbStat == NULL || statbuf == NULL)
	return OBJ_ERR_BAD_STAT_INP;

    statbuf->st_dev = mySrbStat->st_dev;
    statbuf->st_ino = mySrbStat->st_ino;
    statbuf->st_mode = mySrbStat->st_mode;
    statbuf->st_nlink = mySrbStat->st_nlink;
    statbuf->st_uid = mySrbStat->st_uid;
    statbuf->st_gid = mySrbStat->st_gid;
    statbuf->st_rdev = mySrbStat->st_rdev;
    statbuf->st_size = mySrbStat->st_size;
    statbuf->st_atime = mySrbStat->st_atim;
    statbuf->st_mtime = mySrbStat->st_mtim;
    statbuf->st_ctime = mySrbStat->st_ctim;

#ifndef _WIN32
    statbuf->st_blksize = mySrbStat->st_blksize;
    statbuf->st_blocks = mySrbStat->st_blocks;
#endif

#if defined(PORTNAME_aix) || defined(PORTNAME_alpha) || defined(PORTNAME_sunos) || defined(PORTNAME_c90)
    statbuf->st_spare1 = mySrbStat->st_spare1;
    statbuf->st_spare2 = mySrbStat->st_spare2;
    statbuf->st_spare3 = mySrbStat->st_spare3;
#endif  /* PORTNAME_aix */
#ifdef PORTNAME_aix
    statbuf->st_vfstype = mySrbStat->st_vfstype;
    statbuf->st_vfs = mySrbStat->st_vfs;
    statbuf->st_type = mySrbStat->st_type;
    statbuf->st_access = mySrbStat->st_Access;
    statbuf->st_flag = mySrbStat->st_flag;
#endif  /* PORTNAME_aix */
#if defined(PORTNAME_aix) || defined(PORTNAME_alpha)
    statbuf->st_gen =  mySrbStat->st_gen;
#endif  /* defined(PORTNAME_aix) || defined(PORTNAME_alpha) */
#if defined(PORTNAME_solaris)
    strncpy (statbuf->st_fstype, mySrbStat->st_fstype, _ST_FSTYPSZ - 1);
    statbuf->st_fstype[_ST_FSTYPSZ - 1] = '\n';
#endif  /* defined(PORTNAME_solaris) */

    return 0;
}
#endif	/* PORTNAME_c90 */

int 
htonDirent (dirent_t *mydirent, int len)
{
    dirent_t *tmpdirent;
    int tmplen, savlen;

    if (mydirent == NULL)
	return 0;

    tmpdirent = mydirent;
    tmplen = 0;
    
    while (tmplen < len) {
	savlen = tmpdirent->d_reclen;
	tmpdirent->d_reclen = htons (tmpdirent->d_reclen);
	tmpdirent->d_ino = (ino_t) htonl (tmpdirent->d_ino);
	tmpdirent->d_off = (off_t) htonl (tmpdirent->d_off);
	tmplen += savlen;
	tmpdirent = (dirent_t *) ((char *) tmpdirent + savlen);
    }
    return (0);
}

int
htonDirent64 (dirent64_t *mydirent, int len)
{
    dirent64_t *tmpdirent;
    int tmplen, savlen;

    if (mydirent == NULL)
        return 0;

    tmpdirent = mydirent;
    tmplen = 0;
   
    while (tmplen < len) {
        savlen = tmpdirent->d_reclen;
	if (tmpdirent->d_reclen <= 0)
	    break;
        tmpdirent->d_reclen = htons (tmpdirent->d_reclen);
#if defined(PORTNAME_c90)
	tmpdirent->d_ino = htonl (tmpdirent->d_ino);
	tmpdirent->d_off = htonl (tmpdirent->d_off);
#else
        srbHtonll ((u_longlong_t *) &tmpdirent->d_ino);
        srbHtonll ((u_longlong_t *) &tmpdirent->d_off);
#endif
        tmplen += savlen;
	tmpdirent = (dirent64_t *) ((char *) tmpdirent + savlen);

    }
    return (0);
}

int 
ntohDirent (dirent_t *mydirent, int len)
{
    dirent_t *tmpdirent;
    int tmplen;

    if (mydirent == NULL || len <= 0)
	return 0;

    tmpdirent = mydirent;
    tmplen = 0;
    
    while (tmplen < len) {
	tmpdirent->d_reclen = ntohs (tmpdirent->d_reclen);
	if (tmpdirent->d_reclen <= 0)
	    break;
	tmpdirent->d_ino = (ino_t) ntohl (tmpdirent->d_ino);
	tmpdirent->d_off = (off_t) ntohl (tmpdirent->d_off);
	tmplen += tmpdirent->d_reclen;
	tmpdirent = (dirent_t *) ((char *) tmpdirent + tmpdirent->d_reclen);
    }
    return (0);
}

int
ntohDirent64 (dirent64_t *mydirent, int len)
{
    dirent64_t *tmpdirent;
    int tmplen;

    if (mydirent == NULL)
        return 0;

    tmpdirent = mydirent;
    tmplen = 0;
   
    while (tmplen < len) {
        tmpdirent->d_reclen = ntohs (tmpdirent->d_reclen);
	if (tmpdirent->d_reclen <= 0)
	    break;
#if defined(PORTNAME_c90)
	tmpdirent->d_ino = ntohl (tmpdirent->d_ino);
	tmpdirent->d_off = ntohl (tmpdirent->d_off);
#else
        srbNtohll ((u_longlong_t *)&tmpdirent->d_ino);
        srbNtohll ((u_longlong_t *)&tmpdirent->d_off);
#endif
        tmplen += tmpdirent->d_reclen;
	tmpdirent = (dirent64_t *) ((char *) tmpdirent + tmpdirent->d_reclen);
    }
    return (0);
}

#if !defined(PORTNAME_c90)
#ifndef srbNtohll
int
srbHtonll (u_longlong_t *mylonglong)
{
    int *tmpIntPtr, *myIntPtr1, *myIntPtr2;
    u_longlong_t tmplonglong;

    if (mylonglong == NULL)
        return 0;

    if (ntohl(1) == 1)
        return 0;

    tmplonglong = *mylonglong;
    tmpIntPtr = (int *) &tmplonglong;
    myIntPtr1 = (int *) mylonglong;
    myIntPtr2 = myIntPtr1 + 1;
    *myIntPtr2 = htonl (*tmpIntPtr);
    tmpIntPtr += 1;
    *myIntPtr1 = htonl (*tmpIntPtr);

    return 0;
}

int
srbNtohll (u_longlong_t *mylonglong)
{
    int *tmpIntPtr, *myIntPtr1, *myIntPtr2;
    u_longlong_t tmplonglong;

    if (mylonglong == NULL)
        return 0;

    if (ntohl(1) == 1)
	return 0;

    tmplonglong = *mylonglong;
    tmpIntPtr = (int *) &tmplonglong;
    myIntPtr1 = (int *) mylonglong;
    myIntPtr2 = myIntPtr1 + 1;
    *myIntPtr2 = ntohl (*tmpIntPtr);
    tmpIntPtr += 1;
    *myIntPtr1 = ntohl (*tmpIntPtr);

    return 0;
}
#endif	/* srbNtohll */
#endif	/* PORTNAME_c90 */

#ifdef LOCK_OBJ
int srbToLocalLockCmd (int srbCmd)
{
    if (srbCmd == SRB_F_SETLK)
        return (F_SETLK);
    else if (srbCmd == SRB_F_SETLKW)
        return (F_SETLKW);
    else if (srbCmd == SRB_F_GETLK)
        return (F_GETLK);
    else if (srbCmd == SRB_F_RDLCK)
        return (F_RDLCK);
    else if (srbCmd == SRB_F_WRLCK)
        return (F_WRLCK);
    else if (srbCmd == SRB_F_UNLCK)
        return (F_UNLCK);
    else
        return (OBJ_ERR_LOCK_CMD);
}

int localToSrbLockCmd (int localCmd)
{
    if (localCmd == F_SETLK)
        return (SRB_F_SETLK);
    else if (localCmd == F_SETLKW)
        return (SRB_F_SETLKW);
    else if (localCmd == F_GETLK)
        return (SRB_F_GETLK);
    else if (localCmd == F_RDLCK)
        return (SRB_F_RDLCK);
    else if (localCmd == F_WRLCK)
        return (SRB_F_WRLCK);
    else if (localCmd == F_UNLCK)
        return (SRB_F_UNLCK);
    else
        return (OBJ_ERR_LOCK_CMD);
}
#endif

#ifdef PARA_OPR
void 
portalManager (struct portalInput *myInput)
{
   struct sockaddr_in rsin;
#if defined(PORTNAME_aix) || defined(PORTNAME_sgi)
   size_t addrlen;
#else
   int addrlen;
#endif
#ifdef PORTAL_TIMING
   int firstTime = 0;
#endif
   int nbytes;
   int myCookie;
   int status;


   for (;;) {
      int relaySocketFd;

      addrlen = sizeof(rsin);
      while ((relaySocketFd =
              accept(myInput->sock, (struct sockaddr *) & rsin,
#if defined(PORTNAME_osx) || defined(PORTNAME_sgi)
                     (int *) & addrlen)) < 0) {
#else
                     (size_t *) & addrlen)) < 0) {
#endif
         if ((errno != EINTR) && (errno != EAGAIN)) {
#ifdef PORTAL_DEBUG
            perror("accept");
#endif
            break;
         }
      }                         /* end while */
      if (relaySocketFd < 0)
         break;

      if (myInput->cookie != 0) {
         /* read the first 4 bytes for the cookie */
#ifdef _WIN32
         nbytes = recv(relaySocketFd,&myCookie,sizeof(myCookie),0);
#else
         nbytes = read (relaySocketFd, &myCookie,
#endif
           sizeof (myCookie));
	 myCookie = ntohl (myCookie);
	 if (nbytes != sizeof (myCookie) || myCookie != myInput->cookie) {
	    fprintf (stderr, 
             "portalManager:cookie err, bytes read=%d,cookie=%d,inCookie=%d for %s\n",
              nbytes, myInput->cookie, myCookie, myInput->locFilePath);
#ifdef _WIN32
        closesocket(relaySocketFd);
#else
	    close (relaySocketFd);
#endif
	    continue;
	 }
      }

       pthread_mutex_lock(&myInput->condMutex);
       myInput->tranInp[myInput->threadInx].relaySocketFd = relaySocketFd;
       myInput->tranInp[myInput->threadInx].threadInx = myInput->threadInx;
       myInput->tranInp[myInput->threadInx].portInput = myInput;
      
#ifdef PORTAL_DEBUG
        printf("Connection accepted on socket %d from %s\n",
                relaySocketFd, inet_ntoa (rsin.sin_addr));
#endif

#ifdef PORTAL_TIMING
      if (firstTime == 0) {
	(void) gettimeofday(&myInput->startTime, (struct timezone *)0);
        firstTime = 1;
      }
#endif

      myInput->numThreads ++;
      pthread_mutex_unlock(&myInput->condMutex);
      srbSetsockopt (relaySocketFd);

      /*
       * Spawn a thread to handle this transfer request
       */
      status = pthread_create(&myInput->threadId[myInput->threadInx], 
       pthread_attr_default,
       (void*(*)(void*)) myInput->transferHandler, 
       (void *) &myInput->tranInp[myInput->threadInx]);
      if (status < 0) {
         printf ("portalManager: pthread_create failed. errno = %d\n",
	  errno);
      } else {
	 if (status < 0) 
            printf ("portalManager: pthread_detach failed. errno = %d\n",
             errno);
         myInput->threadInx++;
      }
   }                            /* end for */
}

void *
portalTransfer (struct transferInput *tranInp)
{
   struct relayHeader myHeader;
   char *buf = NULL;
   int bufLen = 0;
   int nbytes, toread;
   char *bufptr;
   struct portalInput *myPortalInput;
   int localFd = -1;
   srb_long_t curOffset = 0;
   int status = 1;

    myPortalInput = (struct portalInput *) tranInp->portInput;
#ifdef PORTAL_DEBUG
    printf ("portalTransfer: threadInx %d for %s\n", tranInp->threadInx,
    myPortalInput->locFilePath);
    fflush (stdout);
#endif

   while (status > 0) {
      /* read the header */

#ifdef _WIN32
      nbytes = recv(tranInp->relaySocketFd,(void *) &myHeader,sizeof (myHeader),0);
#else
      nbytes = read (tranInp->relaySocketFd, (void *) &myHeader, 
        sizeof (myHeader));
#endif
#ifdef PORTAL_DEBUG
     if (nbytes != sizeof (myHeader))
	printf (
	 "portalTransfer:header read- read %d bytes, expect %d\n", nbytes, 
	 sizeof (myHeader));
#endif
      if (nbytes < 0) {
         status = unixErrConv (errno);
         break;
      } else if (nbytes == 0) {
         status = 0;
         break;
      }
      myHeader.operation = ntohl (myHeader.operation);
      srbNtohll ((u_longlong_t *)&myHeader.offset);
      myHeader.length = ntohl (myHeader.length);
#ifdef PORTAL_DEBUG
      printf 
       ("portalTransfer: recieve packet: oper=%d, offset=%lld, length=%d\n",
        myHeader.operation, myHeader.offset, myHeader.length);
      fflush (stdout);
#endif


      if (myHeader.offset < 0) {
         printf ("portalTranser: requested offset %lld less than zero\n",
	  myHeader.offset);
      }

      switch (myHeader.operation) {
       case READ_OPR:
       case GET_OPR:
         if (localFd < 0) {
            localFd = open (myPortalInput->locFilePath, O_RDWR | O_CREAT, 
             0644);
            if (localFd < 0) {
               sprintf (myPortalInput->errorMessage,
                "portalTransfer: cannot open %s, errno = %d\n",
                  myPortalInput->locFilePath, errno);
               status = unixErrConv (errno);
               break;
            }
         }

         if (myHeader.length > bufLen) {
            if (bufLen > 0)
               free (buf);
            buf = (char *) malloc (myHeader.length);
            bufLen = myHeader.length;
         }
         toread = myHeader.length;
         bufptr = buf;
         do {
#ifdef _WIN32
            nbytes = recv(tranInp->relaySocketFd, bufptr, toread, 0);
#else
            nbytes = read (tranInp->relaySocketFd, bufptr, toread);
#endif
            if (nbytes > 0) {
               toread -= nbytes;
               bufptr += nbytes;
            }
         } while (toread > 0 && nbytes > 0);

         if (toread > 0) {
            sprintf (myPortalInput->errorMessage, 
             "portalTransfer: %d bytes read at offset %lld does not equal to header %d\n",
              myHeader.length - toread, myHeader.offset, myHeader.length);
            status = unixErrConv (errno);
            break;
         }

         if (curOffset != myHeader.offset) {
             curOffset = lseek (localFd, myHeader.offset, SEEK_SET);
            if (curOffset != myHeader.offset) {
               sprintf (myPortalInput->errorMessage,
                "portalTransfer: lseek of %s to %lld error, errno = %d\n",
                  myPortalInput->locFilePath, myHeader.offset, errno);
               status = unixErrConv (errno);
               break;
            }
         }

         nbytes = write (localFd, buf, myHeader.length);
         if (nbytes != myHeader.length) {
            sprintf (myPortalInput->errorMessage,
             "portalTransfer: %d bytes written at offset %lld does not equal to header %d\n",
              nbytes, myHeader.offset, myHeader.length);
            status = unixErrConv (errno);
            break;
         }

	 curOffset += nbytes;
         pthread_mutex_lock(&myPortalInput->varMutex);
         myPortalInput->bytesMoved += myHeader.length;
         pthread_mutex_unlock(&myPortalInput->varMutex);
         break;
       case WRITE_OPR:
       case PUT_OPR:
         if (localFd < 0) {
            localFd = open (myPortalInput->locFilePath, O_RDONLY, 0644);
	    if (localFd < 0) {
               sprintf (myPortalInput->errorMessage, 
                "portalTransfer: cannot open %s, errno = %d\n",
                  myPortalInput->locFilePath, errno);
               status = unixErrConv (errno);
               break;
            }
         }

         if (myHeader.offset >= myPortalInput->size) {
            /* we reached the end */
            status = 0;
            break;
         }

         if (curOffset != myHeader.offset) {
             curOffset = lseek (localFd, myHeader.offset, SEEK_SET);
            if (curOffset != myHeader.offset) {
               sprintf (myPortalInput->errorMessage,
                "portalTransfer: lseek of %s to %lld error, errno = %d\n",
                  myPortalInput->locFilePath, myHeader.offset, errno);
               status = unixErrConv (errno);
               break;
            }
         }

         if (myHeader.length > bufLen) {
            if (bufLen > 0)
               free (buf);
            buf = (char *) malloc (myHeader.length);
            bufLen = myHeader.length;
         }
         toread = read (localFd, buf, myHeader.length);
         if (toread <= 0) {
            if (toread < 0) {
               sprintf (myPortalInput->errorMessage,
                "portalTransfer: read of %s to %lld error, errno = %d\n",
                  myPortalInput->locFilePath, myHeader.offset, errno);
            }
            status = unixErrConv (errno);
            break;
         }

	 curOffset += toread;
#ifdef _WIN32
         nbytes = send(tranInp->relaySocketFd,buf, toread,0);
#else
         nbytes = write (tranInp->relaySocketFd, buf, toread);
#endif
         if (nbytes != toread) {
            sprintf (myPortalInput->errorMessage,
             "portalTransfer: %d bytes written at offset %lld does not equal to header %d\n",
              nbytes, myHeader.offset, toread);
            status = unixErrConv (errno);
            break;
         }
         pthread_mutex_lock(&myPortalInput->varMutex);
         myPortalInput->bytesMoved += nbytes;
         pthread_mutex_unlock(&myPortalInput->varMutex);
#ifdef DEBUG
         printf ("Thread %d wrote %d at offset %d\n",
          tranInp->relaySocketFd, nbytes, myHeader.offset);
#endif
         break;
       case DONE_OPR:
       default:
         /* signal to the main thread if everyone are done */
         status = 0;
         break;
      }
   }
   /* signal to the main thread if everyone are done */
   pthread_mutex_lock (&myPortalInput->condMutex);
   myPortalInput->numThreads --;
   if (status < 0)
      myPortalInput->status = status;
   pthread_mutex_unlock (&myPortalInput->condMutex);
#ifdef _WIN32
   closesocket(tranInp->relaySocketFd);
#else
   close (tranInp->relaySocketFd);
#endif
   if (localFd >= 0)
      close (localFd);
   if (buf != NULL)
      free (buf);

   return 0;
}

#endif /* PARA_OPR */

int
readHeader (int fd, struct relayHeader *myHeader)
{
    int status;

    status = readPartialHeader (fd, myHeader, 0);
    return (status);
}

int
readPartialHeader (int fd, struct relayHeader *myHeader, int offset)
{
    int nbytes, status;
    int toRead;
    char *tmpPtr;

    toRead = sizeof (struct relayHeader) - offset;
    tmpPtr = (char *) myHeader + offset;

    while (toRead > 0) {
#ifdef _WIN32
        nbytes = recv(fd,(void *) tmpPtr, toRead, 0);
#else
        nbytes = read (fd, (void *) tmpPtr, toRead);
#endif
	if (nbytes <= 0)
	    break;
	toRead -= nbytes;
	tmpPtr += nbytes;
    }

    if (toRead != 0) {
        fprintf (stderr,
         "readHeader:header read- read %d bytes, expect %d\n", 
	 sizeof (struct relayHeader) - toRead, sizeof (struct relayHeader));
         status = unixErrConv (errno);
         return (status);
     }
     myHeader->operation = ntohl (myHeader->operation);
     srbNtohll ((u_longlong_t *)&myHeader->offset);
     myHeader->length = ntohl (myHeader->length);
     return (0);
}

int unixErrConv (int status)
{

    if (status < UNIX_ERR_START && status > UNIX_ERR_END) {
        printf ("unixErrConv: Unknown UNIX error %d\n", status);
        return (UNIX_UNKNOWN_ERR);
    }

    return (UNIX_ERR_PREFIX - status);
}

int
srbThreadSleep (int sec, int microSec)
{
    struct timeval sleepTime;

    sleepTime.tv_sec = sec;
    sleepTime.tv_usec = microSec;

    select (0, NULL, NULL, NULL, &sleepTime);

    return 0;
}

/* getrandomArray - With the input arraySz, generate an array of random unique
 * number with value less than arraySz. The output is put in randomArray.
 */

int
getrandomArray (int arraySz, int **randomArray)
{
    int i, j, k;
    int *myArray;
    int myNum;

    if (arraySz < 0) {
	*randomArray = NULL;
	return -1;
    }

    myArray = (int *) malloc (arraySz * sizeof (int));

    for (i = 0; i < arraySz; i++) {
        myArray[i] = -1;
    }

    for (i = arraySz ; i > 0; i --) {
    /* XXXX on solaris, the first random number is always odd. 
     * we are in trouble if there are just 2 resources.
     * myNum = random() % i; */

        myNum = (random() >> 4) % i;

        k = 0;
        for (j = 0; j < myNum + 1 ; j ++) {
            while (myArray[j + k] != -1) {
                k++;
            }
        }
        myArray[myNum + k] = i - 1;
    }

    *randomArray = myArray;

    return (0);
}

#if defined(__STDC__) || defined(__cplusplus)
void tvsub(struct timeval *tdiff,
          struct timeval *t1,
          struct timeval *t0)
#else  /* not __STDC__ or __cplusplus */
void tvsub(tdiff, t1, t0)
        struct timeval *tdiff;
        struct timeval *t1;
        struct timeval *t0;
#endif /* __STDC__ or __cplusplus */
{
    tdiff->tv_sec = t1->tv_sec - t0->tv_sec;
    tdiff->tv_usec = t1->tv_usec - t0->tv_usec;
    if (tdiff->tv_usec < 0) {
        tdiff->tv_sec--;
        tdiff->tv_usec += 1000000;
    } /* end_if */
} /* end_function tvsub() */

srb_long_t
addrToSRBLong (char *myAddr)
{

#ifdef ADDR_64BIT
    srb_long_t tmpLong;
    tmpLong = (srb_long_t) myAddr;
    return (tmpLong);
#else
    int tmpInt;

    tmpInt = (int) myAddr;
    return ((srb_long_t) tmpInt);
#endif
}

/* genPortInp - Given the port number which should be a 16 bit integer,
 * compose an integer with a random number in the upper 16 bits and the
 * port number in the lower 16 bits
 */
int 
genPortInp (int clPort)
{
    int portInp;

    if (clPort < 0 || clPort >= (1 << 17)) {
	fprintf (stderr, "composePortInput: Input clPort %d is out of range\n",
         clPort);
	return -1;
    }
 
    if (RandomSeed == 0) {
	srandom ((unsigned int) time(0) / getpid());
	RandomSeed = 1;
    }

    portInp = ((random () << 16) & 0x7fff0000) | clPort;
     
    return (portInp);
}

int
getPortFromPortInp (int portInp)
{
    int clPort;

    clPort = portInp & 0xffff;

    return clPort;
}

int 
getCookieFromPortInp (int portInp)
{
    int cookie;

    cookie = (portInp & 0x7fff0000) >> 16;

    return cookie;
}

/* setMcatNameInQval - This is a kluge that uses an empty field in the
 * qval array and use it to pass the mcatName to the query- queryDataInfo()
 * on the server and srbGetDataDirInfo on the client and return the index
 * of qval used.
 */
int
setMcatNameInQval (char* mcatName, char qval[][MAX_TOKEN])
{
    /* start with index 1 */
    for (i = 1; i < MAX_DCS_NUM; i++) {
 	if (qval[i][0] == '\0') {
    	    sprintf(qval[i], mcatName);
	    return (i);
	}
    }
    return (-1);
}

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

/* svrSendStatus - send the status to client */
int
svrSendStatus (int sock, int status)
{
    int retVal, retBuf;
#if defined(PORTNAME_c90)
    VARSIZE (&retBuf) = htonl (status);
#else
    retBuf = htonl (status);
#endif

    retVal = send (sock, (char *) (char *) &retBuf, 4, 0);
    return (retVal);
}

/* svrReceiveStatus - receive status */
int
svrReceiveStatus (int sock)
{
    int retBuf, status;

#ifdef _WIN32
    status = recv(sock, (char *) &retBuf, 4, 0);
#else
    status = read (sock, (char *) &retBuf, 4);
#endif
    if (status <= 0) {
       fprintf (stderr,
        "svrReceiveStatus: couldn't recv status packet: errno=%d, %s\n",
        errno,strerror(errno));
       return CLI_ERR_RECV;
    }

#if defined(PORTNAME_c90) || defined(PORTNAME_t3e)
    status = sToL (ntohl (VARSIZE (&retBuf)));
#else
    status = ntohl (retBuf);
#endif

    return (status);
}

#ifdef _WIN32
void CALLBACK win_connect_timeout_cb(
  UINT uID, UINT uMsg, DWORD dwUser,
  DWORD dw1, DWORD dw2)
{
   win_connect_timeout = 1;
}
#endif

int
connectWithTimeout (int lsock, struct sockaddr *sin)
{
    int connectCnt = 0;
    int status;

#ifdef _WIN32
    /* A Windows console app has very limited timeout functionality.
     * An pseudo timeout is implemented.
     */
    status = 0;
    win_connect_timeout = 0;
    win_connect_timer_id = timeSetEvent(CONNECT_TIMEOUT*1000, 0, win_connect_timeout_cb, 0, TIME_ONESHOT);

    while ((connectCnt < MAX_CONN_SVR_CNT) && (!win_connect_timeout))
    {
        if ((status = connect (lsock, sin, sizeof (struct sockaddr)))
         < 0) {
            connectCnt ++;
            srbThreadSleep (0, 200000);
        } else {
            break;
        }
    }

    if(win_connect_timeout)
    {
        fprintf(stderr,"portalConnect: connect msg timed out for pid %d\n", getpid ());
        status = SYS_ERR_PORTAL_CONN;
    }
    else
    {
        timeKillEvent(win_connect_timer_id);
    }

#else
    signal(SIGALRM, connectTimeout);
    if (setjmp(Jcenv) == 0) {
        alarm (CONNECT_TIMEOUT);
        while (connectCnt < MAX_CONN_SVR_CNT) {
            if ((status = connect (lsock, sin, sizeof (struct sockaddr))) < 0) {
                connectCnt ++;
                srbThreadSleep (0, 200000);
            } else {
                break;
            }
        }
        alarm (0);
    } else {
        fprintf(stderr,
         "portalConnect: connect msg timed out for pid %d\n", getpid ());
        status = SYS_ERR_PORTAL_CONN;
    }

#endif
    return (status);
}

/* clGetallChar - try to get all char in the input buf */

int
clGetAllChar(char* s, comBuf *commBuf, int f, int authSchemeInx, int maxLen)
{
    int c;
    int len, myLen;

    len = commBuf->inpInx2 - commBuf->inpInx1;
    if (len <= 0) {
	return (0);
    } else if (len > maxLen) {
	len = maxLen;
    }
    myLen = len;
    while (len-- && (c = bufgetc(f, commBuf, authSchemeInx)) != EOF)
        *s++ = c;

    return myLen;
}

int
getUserByDn (char *userDn, char **outBuf)
{
    srbConn* conn;
    int retval;

    conn = srbConnect (NULL, NULL, NULL, TICKET_USER, TICKET_USER_DOMAIN, 
     NULL, NULL);

    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf (stderr, "Remote connect failed: %s",clErrorMessage(conn));
        return clStatus(conn);
    }

    retval = srbGetUserByDn (conn, 0, 0, userDn, outBuf);

    clFinish (conn);

    return (retval);
}
    
int
readDataFromPortal (int srb_fd, char *buf, int maxLen)
{
    int status;
    int toread, nbytes;
    struct relayHeader myHeader;
    char *bufptr;

    status = readHeader (srb_fd, &myHeader);
    if (status < 0) {
#ifdef _WIN32
        closesocket(srb_fd);
#else
        close (srb_fd);
#endif
        return (status);
    }

    if (myHeader.operation == DONE_OPR)
        return (0);

    toread = myHeader.length;

    if (toread > maxLen) {
        fprintf (stderr,
         "readDataFromPortal: len from header %d is large than buf size %d\n",
         toread, maxLen);
        return -1;
    }

    bufptr = buf;
    do {
#ifdef _WIN32
        nbytes = recv(srb_fd, bufptr, toread, 0);
#else
        nbytes = read (srb_fd, bufptr, toread);
#endif
        if (nbytes > 0) {
            toread -= nbytes;
            bufptr += nbytes;
        }
    } while (toread > 0 && nbytes > 0);


    if (toread != 0)
        return -1;
    return (myHeader.length);
}

