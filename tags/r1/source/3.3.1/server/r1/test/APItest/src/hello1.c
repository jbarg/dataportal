/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/*-------------------------------------------------------------------------
 *
 * hello1.c--
 * This is an example of a proxy program that is exec by the srbServer
 * through the srbExecCommand() call from client. PORTAL_ENV contains 
 * the descriptor of the socket connected to the client.
 *
 *-------------------------------------------------------------------------
 */

#include <stdio.h>
#include <sys/file.h>
#include <sys/stat.h>
#ifdef PORTNAME_solaris
#include <fcntl.h>
#endif

#include "srbClient.h"
#define BUF_SZ	4096

#define CONNECT_SRB 1

void exit_nicely(srbConn* conn)
{
  clFinish(conn);
  exit(0);
}

int
main(int argc, char **argv)
{
    srbConn *conn;
    int i;
    int portal, status, portal1;
    char *portalEnv;
    char buf [BUF_SZ];
    
    portalEnv = getenv (PORTAL_ENV);
    if (portalEnv == NULL)
	exit (1);
    else 
        portal =atoi (portalEnv);

    sprintf (buf, "Hello world from command HELLO !!! My portal fd is %d, clientUserName=%s\n", portal, getenv (USER_KEYWORD));

    write (portal, buf, strlen (buf));

#ifdef CONNECT_SRB
    conn = srbConnect (NULL, NULL, NULL, 
     NULL, NULL, NULL, NULL);
    /* conn = svrConnect (NULL, NULL, NULL);  */
    if (clStatus(conn) != CLI_CONNECTION_OK) {
	sprintf (buf, "Connection to srbMaster failed.\n");
	write (portal, buf, strlen (buf));
        exit_nicely(conn);
    }

    sprintf (buf, "Successfully connected to SRB.\n");
    write (portal, buf, strlen (buf));

    portal1 = srbExecCommand (conn, "penv", NULL, NULL, NULL,
     PORTAL_STD_IN_OUT);

    while ((status = read (portal1, buf, sizeof (buf))) > 0) {
        write (portal, buf, strlen (buf));
    }


    clFinish(conn);
#endif


    exit(0);
}

