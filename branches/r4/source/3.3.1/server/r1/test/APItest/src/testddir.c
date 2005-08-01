/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * testddir.c--
 *    test the srbGetDataDirInfo call 
 *
 *-------------------------------------------------------------------------
 */

#include "srbClient.h"

#define srbAuth		NULL
#define HOST_ADDR	NULL

#define COLLECTION	"/srbtest"
#define ROWS_WANTED     2

static char *
get_from_result_struct(mdasC_sql_result_struct *result,
char *tab_name, char *att_name)
{
  int i;
  for (i = 0; i <  result->result_count ; i++){
    /* XXXXX - strstr is used because the schema name has been appended to
     * the beginning of sqlresult[i].tab_name.
     */
    if(strstr(result->sqlresult[i].tab_name,tab_name) &&
       !strcmp(result->sqlresult[i].att_name,att_name))
      {return ((char *) result->sqlresult[i].values);}
  }
  return (NULL);
}

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
    mdasC_sql_result_struct *myresult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int selval[MAX_DCS_NUM];
    int i;
    char m_user_name[100];
    char data_name[100];
    char access_name[100];
    char domain_name[100];

    char *res_data_name;
    char *res_data_path_name;
    char *res_rsrc_name;
    char *res_rsrc_typ_name;
    char *res_rsrc_locn_name;
    int j, alive;
    char *rval[MAX_DCS_NUM];
    int status;
 
    if (argc != 3) {
        fprintf(stderr, 
	  "Usage: %s <option-code> <name>\n",
             argv[0]);
        exit(1);
    }

    for (i = 0; i < MAX_DCS_NUM; i++) {
	selval[i] = 0;
	sprintf(qval[i],"");
    }

/*
 0       "data_id",
 1       "repl_enum",
 2"data_name",
 3"alias_name",
 4"data_typ_name",
 5"schema_name",
 6      "is_partitioned",
 7"user_name",
 8"access_constraint",
 9"domain_desc",
10"path_name",
11"rsrc_name",
12"netprefix",
13"rsrc_typ_name",
14"repl_timestamp",
15       "data_grp_name",
16       "is_encrypted",
17      "is_dynamic",
18"size",
19       "permanence" ,
20       "default_flags",
21"comments",
22  "action_desc",
23  "time_stamp",
24  "comments"
Given 2:(dataname)
 set  1 == 2 1 10 11 12 13 (resource)
 set  2 == 2 4 5 14 18 21 (data param)      rem 4
 set  3 == 2 3 7  (dataname aliases)
 set  4 == 2 10 12 7 8  (access constraint)
 set  5 == 2 9 (domain)
 set  6 == 2 10 12 7 22 23 24 (audit trail)
Given 11:(resourcename)
 set  7 == 2 10 11 12 (data)
 set  8 == 11 12 13 (resource param)
 set  9 == 11 9 (domain)
Given 7:(user)
 set 10 == 2 10 12 7 8  (data access constraint)
 set 11 == 2 3 7  (data name aliases)
 set 12 == 2 10 12 7 22 23 24 (audit trail)
*/

   switch (atoi(argv[1])) {
       case 1 : sprintf(qval[DATA_NAME]," = '%s'",argv[2]);
		selval[DATA_REPL_ENUM] = 1;
		selval[PATH_NAME] = 1;
		selval[RSRC_NAME] = 1;
		selval[RSRC_ADDR_NETPREFIX] = 1;
		selval[RSRC_TYP_NAME] = 1;
                break;
       case 2 : sprintf(qval[DATA_NAME]," = '%s'",argv[2]);
/*
		selval[SCHEMA_NAME] = 1;
*/
		selval[REPL_TIMESTAMP] = 1;
		selval[SIZE] = 1;
                break;
/*
       case 3 : sprintf(qval[DATA_NAME],"'%s'",argv[2]);
		selval[DATA_ALIAS_NAME] = 1;
		selval[USER_NAME] = 1;
                break;
*/
       case 4 : sprintf(qval[DATA_NAME]," = '%s'",argv[2]);
		selval[PATH_NAME] = 1;
		selval[USER_NAME] = 1;
		selval[RSRC_ADDR_NETPREFIX] = 1;
		selval[ACCESS_CONSTRAINT] = 1;
                break;
/*
       case 5 : sprintf(qval[DATA_NAME],"'%s'",argv[2]);
		selval[DOMAIN_DESC] = 1;
                break;
*/
       case 6 : sprintf(qval[DATA_NAME]," = '%s'",argv[2]);
		selval[PATH_NAME] = 1;
		selval[USER_NAME] = 1;
		selval[RSRC_ADDR_NETPREFIX] = 1;
		selval[AUDIT_ACTION_DESC] = 1;
		selval[AUDIT_TIMESTAMP] = 1;
		selval[AUDIT_COMMENTS] = 1;
                break;
       case 7 : sprintf(qval[RSRC_NAME]," = '%s'",argv[2]);
		selval[DATA_NAME] = 1;
		selval[PATH_NAME] = 1;
		selval[RSRC_ADDR_NETPREFIX] = 1;
                break;
       case 8 : sprintf(qval[RSRC_NAME]," = '%s'",argv[2]);
		selval[RSRC_ADDR_NETPREFIX] = 1;
		selval[RSRC_TYP_NAME] = 1;
                break;
/*
       case 9 : sprintf(qval[RSRC_NAME]," = '%s'",argv[2]);
		selval[DOMAIN_DESC] = 1;
                break;
*/
       case 10 : sprintf(qval[USER_NAME]," = '%s'",argv[2]);
		selval[PATH_NAME] = 1;
		selval[DATA_NAME] = 1;
		selval[RSRC_ADDR_NETPREFIX] = 1;
		selval[ACCESS_CONSTRAINT] = 1;
                break;
/*
       case 11 : sprintf(qval[USER_NAME]," = '%s'",argv[2]);
                sprintf(qval[DATA_NAME],"'%s'",argv[2]);
		selval[DATA_ALIAS_NAME] = 1;
                break;
*/
       case 12 : sprintf(qval[USER_NAME]," = '%s'",argv[2]);
		selval[PATH_NAME] = 1;
		selval[DATA_NAME] = 1;
		selval[RSRC_ADDR_NETPREFIX] = 1;
		selval[AUDIT_ACTION_DESC] = 1;
		selval[AUDIT_TIMESTAMP] = 1;
		selval[AUDIT_COMMENTS] = 1;
                break;
       default :
                fprintf(stderr,"Unknown Option: %s\n", argv[1]);
                fprintf(stderr,"Use Sls <option-code> <name>\n");
                break;
   }

    /*
     * set up the connection
     */
    conn = clConnect (HOST_ADDR, NULL, srbAuth);

    /* check to see that the backend connection was successfully made */
    if (clStatus(conn) != CLI_CONNECTION_OK) {
	fprintf(stderr,"Connection to exfMaster failed.\n");
	fprintf(stderr,"%s",clErrorMessage(conn));
	exit_nicely(conn);
    }
	
    myresult = malloc (sizeof (mdasC_sql_result_struct));
    if ((status = srbGetDataDirInfo(conn, 0, qval, selval, myresult, 
      ROWS_WANTED)) < 0) {
        fprintf(stderr, "can't srbGetDataDirInfo. status = %d.\n", status);
        exit_nicely(conn);
    }
 
    /* Dump the result */

    printSqlResult (myresult);

    clearSqlResult (myresult);  /* clear the content of myresult */

    while (myresult->continuation_index >= 0) {
        if ((status = srbGetMoreRows(conn, 0,
          myresult->continuation_index, myresult, ROWS_WANTED)) < 0)
            break;

        /* Dump the result */

        printSqlResult (myresult);
        clearSqlResult (myresult);      /* clear the content of myresult */
    }

    freeSqlResult (myresult);


    clFinish(conn);
    exit(0);
}
