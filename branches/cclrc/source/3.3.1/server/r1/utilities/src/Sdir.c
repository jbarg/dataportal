/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


#include "srbClient.h"
#include "srb_client_errno.h"
 
  
extern char srbAuth[];
extern char srbHost[];
extern char srbUser[];
extern char mdasCollectionName[];
 
 
int
main(int argc, char **argv)
{
    srbConn *conn;
    srbResult *res;
    int i, myself;
    
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    mdasC_sql_result_struct  myresult;
    int j, alive;
    char *rval[MAX_DCS_NUM];
    char dname[MAX_TOKEN];

 
    i = initSrbClientEnv();
    if (i < 0)
      {printf("Error:%i\n",i); exit(1);}
 
 
 
    for (i = 0; i < MAX_DCS_NUM; i++)
      {
        sprintf(qval[i],"");
        selval[i] = 0;
      }   

   switch (atoi(argv[1])) {
       case 1 : sprintf(qval[DATA_NAME]," = '%s'",argv[2]);
                selval[PATH_NAME] = 1;
                selval[PHY_RSRC_NAME] = 1;
                selval[RSRC_ADDR_NETPREFIX] = 1;
                selval[PHY_RSRC_TYP_NAME] = 1;
                break;
       case 2 : sprintf(qval[DATA_NAME]," = '%s'",argv[2]);
                selval[DATA_REPL_ENUM] = 1;
                selval[DATA_GRP_NAME] = 1;
                selval[REPL_TIMESTAMP] = 1;
                selval[SIZE] = 1;
                break;
       case 4 : sprintf(qval[DATA_NAME]," = '%s'",argv[2]);
                selval[PATH_NAME] = 1;
                selval[DATA_REPL_ENUM] = 1;
                selval[USER_NAME] = 1;
                selval[PHY_RSRC_NAME] = 1;
                selval[ACCESS_CONSTRAINT] = 1;
                break;
       case 6 : sprintf(qval[DATA_NAME]," = '%s'",argv[2]);
                selval[PATH_NAME] = 1;
                selval[USER_NAME] = 1;
                selval[DATA_REPL_ENUM] = 1;
                selval[PHY_RSRC_NAME] = 1;
                selval[AUDIT_ACTION_DESC] = 1;
                selval[AUDIT_TIMESTAMP] = 1;
                selval[AUDIT_COMMENTS] = 1;
                break;
       case 7 : sprintf(qval[PHY_RSRC_NAME]," = '%s'",argv[2]);
                selval[DATA_NAME] = 1;
                selval[PATH_NAME] = 1; 
                break;
       case 8 : sprintf(qval[RSRC_NAME]," = '%s'",argv[2]);
                selval[RSRC_ADDR_NETPREFIX] = 1;
		selval[PHY_RSRC_DEFAULT_PATH] = 1;
		selval[RSRC_DEFAULT_PATH] = 1;
                selval[RSRC_TYP_NAME] = 1;
                selval[PHY_RSRC_TYP_NAME] = 1;
                break;
       case 9 : sprintf(qval[USER_NAME]," = '%s'",argv[2]);
                selval[USER_ADDRESS] = 1;
                selval[USER_PHONE] = 1;
                selval[USER_EMAIL] = 1;
                break;
       case 10 : sprintf(qval[USER_NAME]," = '%s'",argv[2]);
                selval[PATH_NAME] = 1;
                selval[DATA_NAME] = 1;
                selval[PHY_RSRC_NAME] = 1;
                selval[ACCESS_CONSTRAINT] = 1;
                break;
       case 11 : sprintf(qval[USER_NAME]," = '%s'",argv[2]);
                selval[DOMAIN_DESC] = 1;
                break;
       case 12 : sprintf(qval[USER_NAME]," = '%s'",argv[2]);
                selval[PATH_NAME] = 1;
                selval[DATA_NAME] = 1;
                selval[PHY_RSRC_NAME] = 1;
                selval[AUDIT_ACTION_DESC] = 1;
                selval[AUDIT_TIMESTAMP] = 1;
                selval[AUDIT_COMMENTS] = 1;
                break;
       case 13 : sprintf(qval[USER_NAME]," = '%s'",argv[2]);
                selval[USER_NAME] = 1;
                selval[USER_GROUP_NAME] = 1;
                break;       
       case 14 : sprintf(qval[USER_NAME]," = '%s'",argv[2]);
                selval[DATA_GRP_NAME] = 1;
                selval[ACCESS_CONSTRAINT] = 1;
 
       case 15 : sprintf(qval[DATA_NAME]," = '%s'",argv[2]);
                 sprintf(qval[PHY_RSRC_NAME]," = '%s'",argv[3]);
                 sprintf(qval[DATA_GRP_NAME]," = '%s'",argv[4]);
                 selval[SIZE] = 1;
                break;       
       case 16 : sprintf(qval[DATA_NAME]," = '%s'",argv[2]);
	         sprintf(qval[PHY_RSRC_NAME]," = '%s'",argv[3]);
	         sprintf(qval[DATA_GRP_NAME]," = '%s'",argv[4]);
		 selval[DATA_ID] = 1;
		 selval[DATA_NAME] = 1;
		 selval[DATA_TYP_NAME] = 1;
		 selval[USER_ADDRESS] = 1;
		 selval[ACCESS_CONSTRAINT] = 1;
		 selval[PATH_NAME] = 1;
		 selval[RSRC_ADDR_NETPREFIX] = 1;
		 selval[REPL_TIMESTAMP] = 1;
		 selval[USER_PHONE] = 1;
		 selval[SIZE] = 1;
		 selval[USER_AUDIT_COMMENTS] = 1;
		 selval[AUDIT_ACTION_DESC] = 1;
		 selval[AUDIT_COMMENTS] = 1;
                break;  
       case 17 : sprintf(qval[DATA_NAME]," = '%s'",argv[2]);
	         sprintf(qval[DATA_GRP_NAME]," = '%s'",argv[3]);
	         sprintf(qval[USER_NAME]," = '%s'",argv[4]);
                selval[PATH_NAME] = 1;
                selval[RSRC_NAME] = 1;
                selval[DATA_NAME] = 1;
		break;
       case 18 : sprintf(qval[DATA_NAME]," = '%s'",argv[2]);
                 sprintf(qval[PHY_RSRC_NAME]," = '%s'",argv[3]);
                 sprintf(qval[DATA_GRP_NAME]," = '%s'",argv[4]);
                 selval[DATA_ID] = 1;
                break;  
 
       case 19 : 
                 selval[DATA_ID] = 1;
                break;  

       default :
                fprintf(stderr,"Unknown Option: %s\n", argv[1]);
                fprintf(stderr,"Use Sdir <option-code> <name>\n");
                exit(1);
                break;
   }

    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Connection to srbMaster failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
        clFinish(conn); exit(3);
    }
 
   i = srbGetDataDirInfo(conn, MDAS_CATALOG, qval, selval, &myresult);
    if (i < 0)
      {
        if (i != CLI_NO_ANSWER)
          fprintf(stderr,"SgetU Error: %i\n", i);
        else fprintf(stderr,"No Answer\n");
        clFinish(conn); exit(4);
      }
 
    show_results(qval, selval, &myresult, 0);
    clFinish(conn);
    exit(0);
}

 
 



