#ifndef PORTAL_H
#define PORTAL_H

#ifdef PARA_OPR
#include <pthread.h>
#endif
#include "srbType.h"
#ifdef HPSS
#include "hpss_api.h"
#include "u_signed64.h"
#include "mvr_protocol.h"
#ifndef NO_DCE
#include "tools.h"
#endif
#else
#define u_signed64 srb_long_t
#endif	/* HPSS */

#ifdef PARA_OPR
#if defined(PORTNAME_aix)
#ifndef _AIX_PTHREADS_D7
#define pthread_mutexattr_default NULL
#define pthread_condattr_default NULL
#define pthread_attr_default NULL
#endif	/* _AIX_PTHREADS_D7 */
#else	/* PORTNAME_aix */
#define pthread_mutexattr_default NULL
#define pthread_condattr_default NULL
#define pthread_attr_default NULL
#endif	/* PORTNAME_aix */
#endif	/* PARA_OPR */

#define SMALL_FILE_SIZE	(10 * 1048576)
/* XXXX for testing
#define SIZE_PER_THREAD	(1000000)
#define SMALL_FILE_SIZE	(20000)
 */

#define MAX_THREAD	16
#define DEF_MAX_THREAD  4
#define MAX_THREAD_ENV  "MaxThread"	/* the MaxThread env string */

#define DEF_SIZE_PER_THREAD	(30 * 1048576)
#define MIN_SIZE_PER_THREAD	(1048576)
#define SIZE_PER_THREAD_ENV	"SizePerThread"

/* The type of operation for relayHeader and firstHeader operation */
#define DONE_OPR    0
#define READ_OPR    1
#define WRITE_OPR   2
#define PUT_OPR     3
#define GET_OPR     4
#define FIRST_HEADER 9
#define DEL_FILES_IN_RES  99999
#define SHUTDOWN_OPR     9999

/* definition for the zoneFlag for the srbDataCopy Call */
#define ALL_LOCAL_ZONE		0	/* both src and dest are local zone */ 
#define ALL_SAME_FOREIGN_ZONE	1	/* both src and dest are foreign zone.
					 * but they are from the same zone */
#define ALL_DIFF_FOREIGN_ZONE	2	/* both src and dest are foreign zone */
#define SRC_LOCAL_ZONE		3	/* only src is in local zone */
#define DEST_LOCAL_ZONE		4	/* only dest is in local zone */


/*
#define PORTAL_DEBUG    1
#define PORTAL_TIMING   1
*/

#if defined(PORTNAME_linux)
#include <sys/timex.h>
#endif /* PORTNAME_linux */
#include <sys/stat.h>

#define nz(x)   ((x) == 0.0 ? 1.0 : (x))

#define LOOP_BACK_ADDR	"127.0.0"	/* loop back addr starts with 27.0.0 */
#define LOCALHOST	"localhost"	/* local host host name */
/* The relay header */
struct relayHeader {
   int operation;
   int length;
   srb_long_t offset;
};

/* The first header */
struct firstHeader {
   int operation;
   int numThreads;
   srb_long_t size;
};

struct transferInput {
    int relaySocketFd;
    int threadInx;
    void *portInput;
};

struct portalInput {
    int sock;
    int cookie;
    int locTypeInx;
    char *locFilePath;
    char *locResLoc;
    void *transferHandler;
    srb_long_t size;
    int operation;
    int numThreads;
    srb_long_t bytesMoved;
    int status;
    char *errorMessage;
    struct timeval startTime;
    struct timeval endTime;
#ifdef PARA_OPR
    pthread_mutex_t varMutex;
    pthread_mutex_t condMutex;
    pthread_cond_t cond;
   pthread_t       threadId[MAX_THREAD * 2];
   struct transferInput tranInp[MAX_THREAD * 2];
   int threadInx;
#endif /* PARA_OPR */
};

/* Server side struct */

/* Define program default values */
#define DEFAULT_MAX_CONNECTIONS 32
/* Define maximum values for sanity checks */
#define MAX_BUFFER_SIZE (8*1048576)

#ifdef PARA_OPR
/*
 * Define a structure of information to track for each Mover transfer
 * connection/thread
 */
typedef struct {
   int             active;      /* Whether thread/connection is active */
   pthread_t       threadId;    /* Id of the transfer thread */
   int             controlSocketFd;     /* Control socket descriptor */
}               connection_info_t;
#endif /* PARA_OPR */

typedef struct {
   struct hostent *hostEntry;
   char            hostname[128];
   unsigned long   ipAddr;
}               tcphost_t;

#ifdef PARA_OPR
struct session {
   u_signed64   fileSize;
   srb_long_t   srbFileSize;
   int          requestId;      /* HPSS request id */
   int          transferStatus; /* Overall status of the data transfer
                                 * (HPSS_E_NOERROR if ok) */
   int          controlSocket;      /* Central Mover connection socket */
   tcphost_t      *hostList;
   pthread_mutex_t paraOprMutex;
   connection_info_t *paraConn; /* Array of connection thread info */
   int		connCnt;	/* the number of connections */
   u_signed64   totalBytesMoved; /* Actual bytes sent from Movers */
   int          firstTimeOpen; /* First time the file is created */
};

/* The double buffer scheme sturct */
struct dBuffer {
   int curInx;
   char *buf[2];
   int status[2];
   int length[2];
   srb_long_t offset[2];
   int fileFd;
   int operation;
   srb_long_t curOffet; /* The current offset of the fileFd */
   pthread_mutex_t *condMutex;
   pthread_cond_t  *myCondition;
   struct moverInput *myMoverInput;
};

struct moverInput {
   int index;   /* ParaConn index */
   char *path;  /* the triplet specify the file for reading or writing. */
   char *resLoc;
   char *clHostAddr;    /* the addr/port double for PUT_OPR and GET_OPR */
   int clPort;
   int systemType;
   int operation;       /* READ_OPR or WRITE_OPR */
   struct session *mySession;
};

#define MAX_OPEN_RETRY	10

struct paraPutGetInput {
    int srbTypeInx; 
    char *srbPath;
    char *srbResLoc;
    char *clHostAddr;
    int clPort;
    srb_long_t offset;
    srb_long_t length;
    srb_long_t status;
    int threadInx;
    int flag;		/* the to indicated whether it is client driven */
    int portalFd;		/* only relevant when it is client driven */
    pthread_mutex_t *myLock;    
};

/* struct dBuffer.status definition */
#define DBUF_NO_DATA    0
#define DBUF_HAVE_DATA  1
#define DBUF_ALL_DONE   2

#endif /* PARA_OPR */
#endif  	/* PORTAL_H */
