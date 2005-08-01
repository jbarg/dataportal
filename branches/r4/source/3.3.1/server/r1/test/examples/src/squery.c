/* squery.c - replicate a srb file
 * int
 * srbGetDataDirInfo(srbConn* conn, int catType, char qval[][MAX_TOKEN],
 * int *selval, mdasC_sql_result_struct *myresult, int rowsWanted)
 *
 * srbGetDataDirInfo - Get metadata info by querying the MDAS catalog.
 * The server uses the input qval[][] and selval[] array to compose
 * and execute SQL queries and returns the query result in myresult.
 * The selval[] array specifies a list of attrbutes to retrieve and
 * qval[][] specifies a lists of predicates to search.
 * Both selval[] and qval[][] must be arrays of size MAX_DCS_NUM and
 * are indexed by values given in mdasC_db2_externs.h under the heading
 * DCS-ATTRIBUTE-INDEX DEFINES.
 *
 * For the selval[] array, setting an element of the array to 1 means that
 * the attribute associated with this element is to be retrieved. e.g.,
 * selval[USER_NAME] = 1; means the "user_name" attribute is to be retrieved.
 * The qval[][] array specifies the predicates of the query.
 */

#include <stdio.h>
#include <sys/file.h>
#include <sys/stat.h>
#if defined(PORTNAME_c90) || defined(PORTNAME_solaris)
#include <sys/fcntl.h> 
#endif
#include <unistd.h>

#include "srbClient.h"

#define HOST       	NULL
#define PORT            NULL
#define SRB_AUTH        NULL	/* For 'PASSWD_AUTH', 'ENCRYPT1' */
#define USER            NULL
#define DOMAIN          NULL
#define AUTH_SCHEME     NULL	/* 'PASSWD_AUTH', 'ENCRYPT1', 'GSI_AUTH' */
#define SERVER_DN       NULL

#define DATATYPE "ascii text"
#define RESOURCE "unix-sdsc"

#define ROWS_WANTED 200

#define NAME_LEN 256
int
main(int argc, char **argv)
{
    srbConn *conn;
    srbResult *res;
    int retVal; 
    int c;
    mdasC_sql_result_struct myresult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int selval[MAX_DCS_NUM];
    int i;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s collection\n",argv[0]);
        exit(1);
    }

    conn = srbConnect (HOST, PORT, SRB_AUTH, USER, DOMAIN, AUTH_SCHEME, 
      SERVER_DN);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Connection to srbMaster failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
        srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        clFinish (conn);
        exit (1);
    }

    printf ("Connected to srbServer successfully\n");

    /* Initialize the selval and qval array. */
    for (i = 0; i < MAX_DCS_NUM; i++) {
        selval[i] = 0;
        sprintf(qval[i],"");
    }

    /* set up the query: Given the Collection, select the path name, 
     * resource name, the network address and the resource type.
     */

    sprintf(qval[DATA_GRP_NAME]," = '%s'",argv[1]);

    selval[DATA_NAME] = 1;
    selval[SIZE] = 1;
    selval[PHY_RSRC_NAME] = 1;
    selval[DATA_OWNER] = 1;
    selval[REPL_TIMESTAMP] = 1;

    /* Send the query request */

    if ((retVal = srbGetDataDirInfo(conn, MDAS_CATALOG, qval, selval,
     &myresult, ROWS_WANTED)) < 0) {
        fprintf(stderr, "can't srbGetDataDirInfo. status = %d.\n", retVal);
        clFinish (conn);
	exit (1);
    }

    /* Dump the result */

    printSqlResult (&myresult);
    clearSqlResult (&myresult); /* clear the content of myresult */

    /* Print any additional rows */

    while (myresult.continuation_index >= 0) {
        if ((retVal = srbGetMoreRows (conn, MDAS_CATALOG,
          myresult.continuation_index, &myresult, ROWS_WANTED)) < 0)
            break;

        /* Dump the result */
        printSqlResult (&myresult);
        clearSqlResult (&myresult); 
    }

    clFinish(conn);
    exit (0);
}
