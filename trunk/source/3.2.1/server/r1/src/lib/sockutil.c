/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/**#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <sgtty.h>
#include <netdb.h>
#include <ctype.h>
#include <stdio.h>

#include <errno.h>
#include <arpa/inet.h>
#include <sys/time.h>
**/
#include <stdio.h>
#include <errno.h>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/time.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif


#define LSA (sizeof (struct sockaddr_in))
#define MAX_TOKEN 200


extern int errno;


/*
 *---------------------------------------------------------------------------
 * GetNetName (ia) -- get name of network corresponding to network address ia.
 *---------------------------------------------------------------------------

char *GetNetName (ia)
struct in_addr ia;
{
    struct netent *pne;

    if ((pne = getnetbyaddr (inet_netof (ia))) == NULL)
        return(-31);
    return pne->n_name;
}
Raja: GetNetName removed because of pargument conflicts*/
/*
 *---------------------------------------------------------------------------
 * ConnectUdp (sbHost, port) -- create a udp socket to destination
 * port on sbHost. Connect the socket to the port.
 *---------------------------------------------------------------------------
 */
int ConnectUdp (sbHost, port)
char *sbHost;
int port;
{
    int fd;
    struct sockaddr_in sa;	/* sa = socket address */
    struct hostent *phe;	/* he = host entry */
    struct in_addr na;		/* na = network address */

#ifdef _WIN32
    if (startWinsock() != 0) {
    (void) fprintf(stderr,
        "FATAL: ConnectUdp: startWinsock() failed\n");
    return(-1);
    }
#endif

    if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) < 0){
      return(-1);
    }

    if ((phe = gethostbyname (sbHost)) == NULL){
      return(-2);
    }

    na = *( (struct in_addr *) (phe->h_addr));

    sa.sin_port = htons(port);

    sa.sin_family = AF_INET;
    sa.sin_addr = na;
    if (connect (fd, (struct sockaddr *) &sa, LSA) == -1) 
    {
#ifdef _WIN32
        if (errno == WSAECONNREFUSED)
            return -3;
        else if (errno == WSAENETUNREACH)
            return -4;
        else
            return -5;
#else
        if (errno == ECONNREFUSED)
	  return(-3);
	else if (errno == ENETUNREACH)
	  return(-4);
        else
	return(-5);
#endif
	}
    return fd;
}

/*
 *---------------------------------------------------------------------------
 * FdToRemoteSa (fd) -- return psa of remote socket associated with fd.
 *---------------------------------------------------------------------------
 */
struct sockaddr_in *FdToRemoteSa (fd)
int fd;
{
    static struct sockaddr_in sa;
#if defined(PORTNAME_aix)
    size_t lsaT = LSA;
#else
    int lsaT = LSA;
#endif


    if (getpeername (fd, (struct sockaddr*)&sa, &lsaT) == -1)
      return((struct sockaddr_in *)-6);

    return &sa;
}

/*
 *---------------------------------------------------------------------------
 * FdToLocalSa (fd) -- return psa of local socket associated with fd.
 *---------------------------------------------------------------------------
 */
struct sockaddr_in *FdToLocalSa (fd)
int fd;
{
    static struct sockaddr_in sa;
#if defined(PORTNAME_aix)
    size_t lsaT = LSA;
#else
    int lsaT = LSA;
#endif

    if (getsockname (fd, (struct sockaddr *)&sa, &lsaT) == -1)
    {	/* since getsockname doesn't fill in our internet address... */
	char sb[MAX_TOKEN];
	struct hostent *phe;

	if (gethostname (sb, sizeof (sb)) != 0)
	  return((struct sockaddr_in *)-32);
	if ((phe = gethostbyname (sb)) == NULL)
	    return((struct sockaddr_in *)-33);
	sa.sin_addr = *(struct in_addr *) phe->h_addr;
    }

    return &sa;
}

/*
 *---------------------------------------------------------------------------
 * IaToSb (ia) -- convert internet address to string representation.
 *---------------------------------------------------------------------------
 */



/*
 *---------------------------------------------------------------------------
 * ConnectTcp (sbHost, port) -- open a tcp connection to host sbHost at
 * the port named port. Return a two-way file descriptor to that socket.
 *---------------------------------------------------------------------------
 */
int ConnectTcp (sbHost, port)
char *sbHost;
u_short port;
{
    int fd;
    struct sockaddr_in sa;	/* sa = socket address */
    struct hostent *phe;	/* he = host entry */

#ifdef _WIN32
    if (startWinsock() != 0) {
    (void) fprintf(stderr,
        "FATAL: ConnectTcp: startWinsock() failed\n");
    return(-34);
    }
#endif

    if ((fd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
       return(-34);

    sa.sin_family = AF_INET;

    if ((phe = gethostbyname (sbHost)) == NULL){
      return(-35);
    }

    sa.sin_addr = *( (struct in_addr *) (phe->h_addr));

    sa.sin_port = htons (port);

    if (connect (fd, (struct sockaddr *)&sa, sizeof sa) == -1) 
    {
#ifdef _WIN32
        if (errno == WSAECONNREFUSED)
            return(-36);
        if (errno == WSAECONNREFUSED)
            return(-37);
        else
            return(-38);
#else
        if (errno == ECONNREFUSED)
	    return(-36);
	else if (errno == ENETUNREACH)
	   return(-37);
        else
	  return(-38);
#endif
	}
    return fd;
}


/*
 *---------------------------------------------------------------------------
 * TrytoConnectTcp (sbHost, port) -- try to open a tcp connection to
 * host sbHost at the port named port. Return a two-way file
 * descriptor to that socket.
 *---------------------------------------------------------------------------
 */
int TrytoConnectTcp (sbHost, port)
char *sbHost;
u_short port;
{
    int fd;
    struct sockaddr_in sa;		/* sa = socket address */
    struct hostent *phe;		/* he = host entry */

#ifdef _WIN32
    if (startWinsock() != 0) {
    (void) fprintf(stderr,
        "FATAL: TrytoConnectTcp: startWinsock() failed\n");
    return(-8);
    }
#endif

    if ((fd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
      return(-8);

    sa.sin_family = AF_INET;

    if ((phe = gethostbyname (sbHost)) == NULL){
      return(-9);
    }

    sa.sin_addr = *( (struct in_addr *) (phe->h_addr));

    sa.sin_port = htons (port);

    if (connect (fd, (struct sockaddr *)&sa, sizeof sa) == -1) 
    {
#ifdef _WIN32
        if(errno == WSAECONNREFUSED)
            return -10;
        else if(errno == WSAENETUNREACH)
            return -11;
        else
            return -12;
#else
        if (errno == ECONNREFUSED) {
	    return(-10);
	}
	else if (errno == ENETUNREACH) {
	  return(-11);
	}
        else {
	    return(-12);
	}
#endif
    }
    return fd;
}


/*
 *---------------------------------------------------------------------------
 * ListenTcp () -- create and return an fd that will accept TCP connections.
 *---------------------------------------------------------------------------
 */
ListenTcp (pport)
    u_short *pport;

{
    int fd;
    struct sockaddr_in sa;
#if defined(PORTNAME_aix)
    size_t len;
#else
    int len;
#endif

#ifdef _WIN32
    if (startWinsock() != 0) {
    (void) fprintf(stderr,
        "FATAL: ListeTcp: startWinsock() failed\n");
    return(-13);
    }
#endif

    if ((fd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
      return(-13);
    sa.sin_family = AF_INET;
    sa.sin_port = htons(*pport);
    sa.sin_addr.s_addr = INADDR_ANY; /* accept connection from all */
    if (bind (fd, (struct sockaddr *)&sa, LSA) == -1)
        return(-14);

    len = sizeof (sa);
    if (getsockname(fd, (struct sockaddr *)&sa, &len) < 0) {
      return(-15);

    }
    
    if (listen (fd, SOMAXCONN) == -1)	/* set up for listening */
            return(-15);

    *pport = ntohs(sa.sin_port);
    return fd;
}


/*
 *---------------------------------------------------------------------------
 * ListenTcpFd () -- listen on TCP connection 'fd'
 *---------------------------------------------------------------------------
 */
ListenTcpFd(fd)
     int fd;
{
    if (listen (fd, SOMAXCONN) == -1)	/* set up for listening */
        return(-16);
    return fd;
}



/*
 *---------------------------------------------------------------------------
 * OpenTcp () -- create and return an fd that can later be used for TCP
 *               connections, return the port number
 *---------------------------------------------------------------------------
 */
OpenTcp (pport)
     u_short *pport;
{
    int fd;
    struct sockaddr_in sa;
#if defined(PORTNAME_aix)
    size_t len;
#else
    int len;
#endif

#ifdef _WIN32
    if (startWinsock() != 0) {
    (void) fprintf(stderr,
        "FATAL: OpenTcp: startWinsock() failed\n");
    return(-17);
    }
#endif

    if ((fd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
                return(-17);
    sa.sin_family = AF_INET;
    sa.sin_port = htons(*pport);
    sa.sin_addr.s_addr = INADDR_ANY;	/* accept connection from all */

    if (bind (fd, (struct sockaddr *)&sa, LSA) == -1)
              return(-18);

    len = sizeof (sa);
    if (getsockname(fd, (struct sockaddr *)&sa, &len) < 0) {
      return(-19);
    }
    
    *pport = ntohs(sa.sin_port);
    return fd;
}


/*
 *---------------------------------------------------------------------------
 * AcceptTcp () -- return an fd to accept a new connection
 *---------------------------------------------------------------------------
 */
AcceptTcp (fd)
     int fd;
{
    int fdConn;
    struct sockaddr_in sa;
#if defined(PORTNAME_aix)
    size_t lsa = LSA;
#else
    int lsa = LSA;
#endif
    
    if ((fdConn = accept (fd, (struct sockaddr *)&sa, &lsa)) < 0)
	return(-20);

    return(fdConn);
}


     
/*
 *---------------------------------------------------------------------------
 * ListenUdp (port) -- create and return an fd that will receive
 * a datagram on port named by port
 *---------------------------------------------------------------------------
 */
ListenUdp (port)
     short port;
{
    int fd;
    struct sockaddr_in sa;

#ifdef _WIN32
    if (startWinsock() != 0) {
    (void) fprintf(stderr,
        "FATAL: ListenUdp: startWinsock() failed\n");
    return(-21);
    }
#endif

    if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
        return(-21);

    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = INADDR_ANY;

    if (bind (fd, (struct sockaddr *)&sa, LSA) == -1) {
	return(-22);
	}
    return fd;
}

/*
 *---------------------------------------------------------------------------  
 * Udp2Way(machine, srcport, destport) -- make a UDP connection between
 * the local srcport and the remote destport.
 *--------------------------------------------------------------------------- 
 */

int Udp2Way (sbHost, srcport, destport)
char *sbHost;
int srcport, destport;
{
    int fd;
    struct sockaddr_in sa;	/* sa = socket address */
    struct hostent *phe;	/* he = host entry */
    struct in_addr na;		/* na = network address */

#ifdef _WIN32
    if (startWinsock() != 0) {
    (void) fprintf(stderr,
        "FATAL: Udp2Way: startWinsock() failed\n");
    return(-23);
    }
#endif

    if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
	return(-23);

    sa.sin_family = AF_INET;
    sa.sin_port = htons(srcport);
    sa.sin_addr.s_addr = INADDR_ANY;

    if (bind (fd, (struct sockaddr *)&sa, LSA) == -1) {
	return(-24);
    }
    
    if ((phe = gethostbyname (sbHost)) == NULL){
      return(-25);
    }


    na = *( (struct in_addr *) (phe->h_addr));

    sa.sin_port = htons(destport);

    sa.sin_family = AF_INET;
    sa.sin_addr = na;
    if (connect (fd, (struct sockaddr *)&sa, LSA) == -1) 
    {
#ifdef _WIN32
        if (errno == WSAECONNREFUSED)
            return -26;
        else if (errno == WSAECONNREFUSED)
            return -27;
        else
            return -28;
#else
        if (errno == ECONNREFUSED)
	    return(-26);
	else if (errno == ENETUNREACH)
	    return(-27);
        else
	    return(-28);
#endif
	}
    return fd;
}

/*
 *---------------------------------------------------------------------------  
 * GetUdpPort() -- allocate a local UDP port.
 *--------------------------------------------------------------------------- 
 */

int GetUdpPort()

{
    int fd;
    struct sockaddr_in sa;	/* sa = socket address */

#ifdef _WIN32
    if (startWinsock() != 0) {
    (void) fprintf(stderr,
        "FATAL: GetUdpPort: startWinsock() failed\n");
    return(-29);
    }
#endif

    if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
	return(-29);
    sa.sin_family = AF_INET;
    sa.sin_port = 0;
    sa.sin_addr.s_addr = INADDR_ANY;

    if (bind (fd, (struct sockaddr *)&sa, LSA) == -1) {
	return(-30);
    }
    
    return(fd);
}

