/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * testtick.c--
 *    Test ticket cals
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
    printf ("Doing a srbListCollect of:%s\n\n", COLLECTION);
    collResult = malloc (sizeof (mdasC_sql_result_struct));

    if ((status = srbListCollect (conn, 0, COLLECTION, "R", 
      collResult, ROWS_WANTED)) < 0) {
	fprintf(stderr, "can't srbListCollect, status = %d\n", status);
	exit_nicely(conn);
    }

    printf ("\nResults of srbListCollect:\n");

    printSqlResult (collResult);

    clearSqlResult (collResult);  /* clear the content of collResult */

    while (collResult->continuation_index >= 0) {
        if ((status = srbGetMoreRows(conn, 0, 
	  collResult->continuation_index, collResult, ROWS_WANTED)) < 0)
            break;

        /* Dump the result */

        printSqlResult (collResult);
        clearSqlResult (collResult);      /* clear the content of myresult */
    }


    printf ("--------------------------------------------------------------\n");
    printf ("Doing a srbIssueTicket for objID %s\n\n", argv[1]);

    if ((status = srbIssueTicket (conn, argv[1], COLLECTION, COLLFLAG, 
      BEGINTIME, ENDTIME, ACCESSCNT,  ticketUser, &ticket)) < 0) {
        fprintf(stderr, "can't srbIssueTicket obj\"%s\", %d\n", 
	  argv[1], status);
        exit_nicely(conn);
    }
 
    /* Dump the ticket */

    printf ("ticket = %s\n", ticket);

    printf ("--------------------------------------------------------------\n");


    printf ("Logging in as : %s and opening obj : %s \n", ticketUser, argv[1]);
    if (ticketUser[0] == '\0') {
    	conn1 = tiUserConnect (HOST_ADDR, NULL);
    } else {
    	conn1 = clConnect (HOST_ADDR, NULL, srbAuth);
    }
    if ((status = clStatus(conn1)) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Connection to srbMaster failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
    	srbRemoveTicket (conn, ticket);
        exit_nicely(conn);
    }

    in_fd = srbObjOpenWithTicket (conn1, argv[1], O_RDONLY, COLLECTION, ticket);
    if (in_fd < 0)  {   /* error */
       fprintf(stderr, "can't open obj\"%s\"\n", 
	argv[1]);
    	srbRemoveTicket (conn, ticket);
	exit_nicely(conn);
    }

    i = 0;
    printf ("Reading obj : %s\n", argv[1]);
    while ((nbytes = srbObjRead (conn1, in_fd, buf, BUFSIZE)) > 0) {
	i += nbytes;
	printf ("Content of the object: \n%s\n", buf);
    }

    if (nbytes < 0) {
        fprintf(stderr, "Error reading, status = %d\n", nbytes);
        fprintf(stderr,"%s",clErrorMessage(conn));
    }
	
    printf ("\n\nBytes read: %d\n", i);
    printf ("----------------------------------------------------------\n");
    srbObjClose (conn1, in_fd);
    clFinish(conn1);

    printf ("Doing a srbRemoveTicket.\n");

    status = srbRemoveTicket (conn, ticket);

    if (status < 0) {
        fprintf(stderr, "srbRemoveTicket error. status = %d\n", status);
        fprintf(stderr,"%s",clErrorMessage(conn));
    }
    
    clFinish(conn);
    exit(0);
}
