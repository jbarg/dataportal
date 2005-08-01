
/* svrConn.h - 
 */

#ifndef SVR_CONN_H
#define SVR_CONN_H
#include <stdio.h>
#include <sys/types.h>


#define ERROR_MSG_LENGTH 4096
typedef struct svrConn{
  char *srbHost; 
  char *srbVersion; 
  char *srbPort;
  char *srbOptions; 
  char *srbMdasDomain;
  int status;
  char errorMessage[ERROR_MSG_LENGTH*8];
  FILE *Pfin;
  FILE *Pfout;
  FILE *Pfdebug;
  int sock; 
  int asyncNotifyWaiting;
  int authSchemeInx;	
} svrConn;

#endif	/* SVR_CONN_H */
