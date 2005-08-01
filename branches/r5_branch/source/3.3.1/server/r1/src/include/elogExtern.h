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

#ifdef MALLOC_TESTING
# Include the following code to log each malloc and free.
# This can be useful in testing/debugging of memory allocation problems.
#define MYMALLOC 1
#define malloc(x) mymalloc(__FILE__, __LINE__ , x)
extern void* mymalloc(char* file, int line, int x); 
#define free(x) myfree(__FILE__, __LINE__ , x)
extern void myfree(char* file, int line, void *x); 
#endif

#define elog elogSRB

#define NOTICE  0       /* random info - no special action */
#define WARN    1      /* Warning error - return to known state */
#define FATAL   2      /* Fatal error - abort process */
#define DEBUG   -2      /* debug message */
#define NOIND   -3      /* debug message, don't indent as far */
#define DEMOM	-4	/* demo message */

extern void elog(int lev, char *fmt, ...);
extern int debugFileOpen();

#endif 	/* ELOG_EXTERN_H */

