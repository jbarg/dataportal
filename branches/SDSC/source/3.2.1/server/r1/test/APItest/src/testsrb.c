/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * testsrb.c--
 *    test a mixed bag of Logical level Object API.
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
#define ROWS_WANTED	2

void 
exit_nicely(srbConn* conn)
{
  clFinish(conn);
  exit(1);
}

int
main(int argc, char **argv)
{
    srbConn *conn;
    srbResult *res;
    int i, nbytes, tmp, in_fd, out_fd;
    char buf[BUFSIZE];
    struct stat *statbuf;
    int acc_method, num_of_loop, loop_cnt;
    mdasC_sql_result_struct *myresult;
    struct clHostElement *hostQueHead;
    int status;
    mdasC_sql_result_struct *collResult;

 
    if (argc != 2 && argc != 3) {
        fprintf(stderr, "Usage: %s objID [num_of_loop]\n",
                argv[0]);
        exit(1);
    }

    if (argc == 3)
        num_of_loop = atoi (argv[2]);
    else
        num_of_loop = 1;

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
	
    /* test audit trail */

    printf ("Setting Audit Trail to AUDIT_TRAIL_ON\n\n");

    srbSetAuditTrail (conn, AUDIT_TRAIL_ON);

    printf ("Audit Trail has been set to %d\n\n", 
      srbSetAuditTrail (conn,GET_AUDIT_TRAIL_SETTING));

    printf ("Doing a srbHostConfig\n\n");

    hostQueHead = srbHostConfig(conn);

    if (hostQueHead == NULL) {
        fprintf(stderr,"srbHostConfig() call  failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
    }

    srbPrintHostInfo (hostQueHead);
    srbFreeHostInfo (hostQueHead);

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
    printf ("Doing a srbGetDatasetInfo of objID %s\n\n", argv[1]);

    myresult = malloc (sizeof (mdasC_sql_result_struct));
    if ((status = srbGetDatasetInfo(conn, 0, argv[1], COLLECTION,
        myresult, ROWS_WANTED, "read")) < 0) {
        fprintf(stderr, "can't srbGetDatasetInfo obj\"%s\", %d\n", 
	  argv[1], status);
        exit_nicely(conn);
    }
 
    /* Dump the result */

    printSqlResult (myresult);

    clearSqlResult (myresult);	/* clear the content of myresult */

    while (myresult->continuation_index >= 0) {
        if ((status = srbGetMoreRows(conn, 0, 
	  myresult->continuation_index, myresult, ROWS_WANTED)) < 0)
	    break;

        /* Dump the result */

        printSqlResult (myresult);
    	clearSqlResult (myresult);	/* clear the content of myresult */
    }

    freeSqlResult (myresult);

    printf ("--------------------------------------------------------------\n");

    for (loop_cnt = 0; loop_cnt < num_of_loop; loop_cnt++) {

	printf ("Opening obj : %s\n", argv[1]);
    	in_fd = srbObjOpen (conn, argv[1], O_RDONLY, COLLECTION);
    	if (in_fd < 0)  {   /* error */
            fprintf(stderr, "can't open obj\"%s\"\n", 
	    argv[1]);
	    exit_nicely(conn);
    	}

    	i = 0;
	printf ("Reading obj : %s\n", argv[1]);
    	while ((nbytes = srbObjRead (conn, in_fd, buf, BUFSIZE)) > 0) {
	    i += nbytes;
	    printf ("Content of the object: \n%s\n", buf);
    	}

    	if (nbytes < 0) {
            fprintf(stderr, "Error reading, status = %d\n", nbytes);
            fprintf(stderr,"%s",clErrorMessage(conn));
    	}
	
	printf ("\n\nBytes read: %d\n", i);
        printf ("----------------------------------------------------------\n");
    	srbObjClose (conn, in_fd);
    }
    clFinish(conn);
    exit(0);
}
