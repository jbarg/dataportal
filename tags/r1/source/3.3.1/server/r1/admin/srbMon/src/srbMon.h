/*-------------------------------------------------------------------------
 *
 * srbMon.h-- Header file for srbMon.c
 *
 *
 *
 *-------------------------------------------------------------------------
 */

#ifndef SRB_MON_H
#define SRB_MON_H

#include <stdarg.h>
#include <signal.h>
#include "srbClient.h"

extern char *optarg;
extern int optind, opterr, optopt;

#define SRB_HOST_FNAME	"srbHostFile"
#define LINE_SZ		1024	
#define MDAS_ENABLED_FL	"MDAS_ENABLED"
#define LOG_FILENAME	"monLog"
#define REBOOT_LIM	1
#define NUM_ROWS_WANTED 3
#define SLEEP_TIME	600	/* sleep 10 minutes */
#define ALARM_TIME	60	/* connection alarm */
#define RUN_SCRIPT	"runsrb"	/* The name of the startup script */
#define KILL_SCRIPT	"killsrb"	/* The name of the shutdown script */

/* Flags for the runMode */
#define INF_LOOP	0	/* The daemon will loop forever */
#define SINGLE_PASS	1	/* Run through the test one time and exit */

/* The following are error codes for srbMon */

#define ERR_HOST_INP_TOO_SH	-90000
#define ERR_HOST_INP_MDAS_EN	-90001
#define ERR_MALLOC		-90002

/* some globals */

int LogFd;			/* The logfile descriptor */
struct srbHostElm *AlarmHostElm = NULL;

/* struct for the host configuration */

struct srbHostElm {
    char *hostName;  	/* The host name */
    char *portNum;
    char *srbLogin;	/* The login name that runs the srbMaster */ 
    char *runPath;	/* The absolute pathName that start the srbMaster */
    int failCnt;
    struct srbHostElm *next;
};

struct srbHostElm *mcatHostHead, *hostHead;

extern int initHost ();
extern int enqHost (struct srbHostElm **queHead,
  char *hostName, char *portNum, char *srbLogin, char *runPath);
extern char *getDataDir();
extern void monLog( char *fmt, ... );
extern int logFileOpen();
extern void rebootSRB (struct srbHostElm *tmpHostElm, int killOnly);
extern void testSRB (struct srbHostElm *tmpHostElm);
extern void daemonize ();
extern char *parseAddr (char *hostAddr);
#if defined(PORTNAME_linux) || defined(PORTNAME_aix) || defined(PORTNAME_c90) || defined(PORTNAME_solaris)
static void alarmHandler(int);
#else
static void alarmHandler(void);
#endif


#endif	/* SRB_MON_H */
