/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * issuetick.c--
 *    test issuing a ticket for an object. 
 *
 *-------------------------------------------------------------------------
 */

#include "srbClient.h"

#define BUFSIZE		4096
#define srbAuth		NULL
#define UNIX_AM		0	/* UNIX access method */
#define UTREE_AM	1
#define HPSS_AM		2
#define HOST_ADDR	NULL
#define DATATYPE        "ascii text"
#define RESOURCE        "unix-sdsc"
#define COLLECTION      "/srbtest"
#define COLLFLAG        "D"
#define BEGINTIME 	NULL
#define ENDTIME		NULL
#define ACCESSCNT	1
#define ROWS_WANTED     2


void 
exit_nicely(srbConn* conn)
{
  clFinish(conn);
  exit(1);
}

int
main(int argc, char **argv)
{
    srbConn *conn, *conn1;
    srbResult *res;
    int i, nbytes, tmp, in_fd, out_fd;
    char buf[BUFSIZE];
    struct stat *statbuf;
    int acc_method, num_of_loop, loop_cnt;
    int status;
    char *ticket;
    char *ticketUser;
    mdasC_sql_result_struct *collResult;

 
    if (argc != 2 && argc != 3) {
        fprintf(stderr, "Usage: %s objID [ticketUser]\n",
                argv[0]);
        exit(1);
    }

    if (argc == 2) {
	ticketUser = "";
    } else {
	ticketUser = argv[2];
    }
	
    /*
     * set up the connection
     */
    conn = clConnect (HOST_ADDR, NULL, srbAuth);

    /* check to see that the backend connection was successfully made */
    if ((status = clStatus(conn)) != CLI_CONNECTION_OK) {
	fprintf(stderr,"Connection to srbMaster failed.\n");
	fprintf(stderr,"%s",clErrorMessage(conn));
	exit_nicely(conn);
    }
	

    printf ("--------------------------------------------------------------\n");
    printf ("Doing a srbIssueTicket for objID %s\n\n", argv[1]);

    if (argv[1][0] == '/') {	/* a collection */
    	if ((status = srbIssueTicket (conn, NULL, argv[1], COLLFLAG,
      	    BEGINTIME, ENDTIME, ACCESSCNT,  ticketUser, &ticket)) < 0) {
              fprintf(stderr, "can't srbIssueTicket obj\"%s\", %d\n",
                argv[1], status);
            exit_nicely(conn);
	}
    } else {
    	if ((status = srbIssueTicket (conn, argv[1], COLLECTION, COLLFLAG, 
          BEGINTIME, ENDTIME, ACCESSCNT,  ticketUser, &ticket)) < 0) {
            fprintf(stderr, "can't srbIssueTicket obj\"%s\", %d\n", 
	      argv[1], status);
            exit_nicely(conn);
	}
    }
 
    /* Dump the ticket */

    printf ("ticket = %s\n", ticket);

    clFinish(conn);
    exit(0);
}
