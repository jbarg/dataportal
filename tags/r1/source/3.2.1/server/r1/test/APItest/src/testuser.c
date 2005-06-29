/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * testuser.c--
 *    test using sys admin API to query and manipulating the MCAT user
 *    information. 
 *
 *-------------------------------------------------------------------------
 */

#include "srbClient.h"

#define BUFSIZE		4096
#define srbAuth		NULL
#define sysUser		"srb"
#define UNIX_AM		0	/* UNIX access method */
#define UTREE_AM	1
#define HPSS_AM		2
#define HOST_ADDR	NULL
#define DOMAIN          "sdsc"
#define OLD_USER_TYPE   "staff"
#define NEW_USER_TYPE	"student"

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
    mdasC_sql_result_struct myresult;
    struct clHostElement *hostQueHead;
    int status;
    char collResult[MAX_ROW][MAX_DATA_SIZE];

 
    if (argc != 5) {
        fprintf(stderr, 
	  "Usage: %s userName userPasswd userGrpName userGrpPasswd\n",
            argv[0]);
        exit(1);
    }

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
	
    status = srbChkMdasSysAuth (conn, sysUser, srbAuth, DOMAIN);

    if (status != 0) {
        fprintf(stderr,"srbChkMdasSysAuth for user: %s failed. status = %d\n", 
	sysUser, status);
        fprintf(stderr,"%s",clErrorMessage(conn));
    }

    status = srbRegisterUser (conn, 0, argv[1], DOMAIN, argv[2], OLD_USER_TYPE,
      "1234 SDSC Lane, S.D. Ca", "619-123-4567", "foo@sdsc.edu");

    if (status != 0) {
        fprintf(stderr,"srbRegisterUser for user: %s failed. status = %d\n", 
	argv[1], status);
        fprintf(stderr,"%s",clErrorMessage(conn));
    }

    status = srbRegisterUserGrp (conn, 0, argv[3], argv[4], OLD_USER_TYPE, 
	"1234 SDSC Lane, S.D. Ca", "619-123-4567", "foo@sdsc.edu");

    if (status != 0) {
        fprintf(stderr,"srbRegisterUserGrp for user: %s failed.status = %d\n", 
	argv[1], status);
        fprintf(stderr,"%s",clErrorMessage(conn));
    }

    status = srbModifyUser (conn, 0, argv[1], NEW_USER_TYPE, U_CHANGE_TYPE);

    if (status != 0) {
        fprintf(stderr,"srbModifyUser for user: %s failed. status = %d\n", 
	argv[1], status);
        fprintf(stderr,"%s",clErrorMessage(conn));
    }

    clFinish(conn);
    exit(0);
}
