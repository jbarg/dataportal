/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* elogExtern.h - Definition and prototype for elog.c
 */

#ifndef ELOG_EXTERN_H
#define ELOG_EXTERN_H

#define NOTICE  0       /* random info - no special action */
#define WARN    1      /* Warning error - return to known state */
#define FATAL   2      /* Fatal error - abort process */
#define DEBUG   -2      /* debug message */
#define NOIND   -3      /* debug message, don't indent as far */
#define DEMOM	-4	/* demo message */

extern void elog(int lev, char *fmt, ...);
extern int DebugFileOpen();


#endif 	/* ELOG_EXTERN_H */

