/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * testcoll.c--
 *    test using collection
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
#define ROWS_WANTED     300

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
    int status;
    mdasC_sql_result_struct *collResult;
    char *newCollection, *parentCollection;
    char *tmpCollection;

 
    if (argc != 3 && argc != 2) {
        fprintf(stderr, "Usage: %s newCollection [parentCollection]\n",
                argv[0]);
        exit(1);
    }

    newCollection = argv[1];

    if (argc == 2) 
	parentCollection = COLLECTION;
    else
	parentCollection = argv[2];

    /*
     * set up the connection
     */
    conn = clConnect (HOST_ADDR, NULL, srbAuth);

    /* check to see that the backend connection was successfully made */
    if ((status = clStatus(conn)) != CLI_CONNECTION_OK) {
	fprintf(stderr,"Connection to exfMaster failed.\n");
	fprintf(stderr,"%s",clErrorMessage(conn));
	exit_nicely(conn);
    }
	

    printf ("--------------------------------------------------------------\n");
    printf ("Doing a srbListCollect of parrent:%s\n\n", parentCollection);
    collResult = malloc (sizeof (mdasC_sql_result_struct));

    if ((status = srbListCollect (conn, 0, parentCollection, "R",
      collResult, ROWS_WANTED)) < 0) {
        fprintf(stderr, "can't srbListCollect, status = %d\n", status);
        exit_nicely(conn);
    }

    printf ("\nResults of srbListCollect:\n");

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

    /* Doing a srbCreateCollect */

    printf ("--------------------------------------------------------------\n");
    printf ("Doing a srbCreateCollect of :%s, parentCollection:%s\n\n", 
      newCollection, parentCollection);
    if ((status = srbCreateCollect (conn, MDAS_CATALOG, parentCollection,
      newCollection)) < 0) {
        fprintf(stderr, "can't srbCreateCollect, status = %d\n", status);
        exit_nicely(conn);
    }

    printf ("--------------------------------------------------------------\n");
    printf ("Doing a srbListCollect of parrent:%s\n\n", parentCollection);
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

    /* Modify the new Collection */

    printf ("--------------------------------------------------------------\n");
    tmpCollection = malloc (strlen (parentCollection) + 
      strlen (newCollection) + 2);
    strcpy (tmpCollection, parentCollection);
    strcat (tmpCollection, "/");
    strcat (tmpCollection, newCollection);
    printf ("Modify the newCollection:%s using srbModifyCollect\n\n",
      tmpCollection);
    if ((status = srbModifyCollect (conn, MDAS_CATALOG, tmpCollection,
      "mwan", "sdsc", "all", D_INSERT_COLL_ACCS)) < 0) {
        fprintf(stderr, "can't srbModifyCollect, status = %d\n", status);
        exit_nicely(conn);
    }

    clFinish(conn);
    exit(0);
}
