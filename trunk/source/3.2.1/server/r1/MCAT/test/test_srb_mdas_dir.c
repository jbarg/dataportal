/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

#include "mdasC_db2_externs.h"
#include "mdasGlobalsExtern.h"
#include "mdasPrototypes.h"
#include "mcatAdmin.h"

mdasC_sql_result_struct  *myresult;
int j, alive;
char *rval[MAX_DCS_NUM];
char qval[MAX_DCS_NUM][MAX_TOKEN];
int selval[MAX_DCS_NUM];
int i;

main(int argc,
        char* argv[])
{  

char *access_list;
int numrows,cont_index;


#ifdef DIAG
   printf("-------------------------------- DIAGNOSTICS ----------------\n");
#endif
       for (i = 0; i < MAX_DCS_NUM; i++)
	 {
	   sprintf(qval[i],"");
	   selval[i] = 0;
	 }
       i = open_db2_interaction(MDAS_CATALOG);
        if (i != 0) {
          printf("Opening Error:%i\n",i);
          exit(-1);
        }

	numrows = atoi(argv[argc-1]);
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
	        selval[DATA_REPL_ENUM] = 1;
                selval[PATH_NAME] = 1;
                selval[USER_NAME] = 1;
                selval[PHY_RSRC_NAME] = 1;
                selval[ACCESS_CONSTRAINT] = 1;
                break;
       case 6 : sprintf(qval[DATA_NAME]," = '%s'",argv[2]);
        	selval[DATA_REPL_ENUM] = 1;
                selval[PATH_NAME] = 1;
                selval[USER_NAME] = 1;
                selval[PHY_RSRC_NAME] = 1;
                selval[AUDIT_ACTION_DESC] = 1;
                selval[AUDIT_TIMESTAMP] = 1;
                selval[AUDIT_COMMENTS] = 1;
                break;
       case 7 : sprintf(qval[RSRC_NAME]," = '%s'",argv[2]);
                selval[DATA_NAME] = 1;
                selval[PATH_NAME] = 1;
                break;
       case 8 : sprintf(qval[RSRC_NAME]," = '%s'",argv[2]);
                selval[RSRC_ADDR_NETPREFIX] = 1;
                selval[RSRC_TYP_NAME] = 1;
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
       case 17 :
	         sprintf(qval[USER_NAME], " = '%s'",argv[2]);
	         selval[USER_NAME] = 1;
		 selval[ACCESS_GROUP_NAME] = 1;
		 selval[GROUP_ACCESS_CONSTRAINT] = 1;

			break;	   
       case 18 : sprintf(qval[DATA_NAME]," = '%s'",argv[2]);
	         sprintf(qval[PHY_RSRC_NAME]," = '%s'",argv[3]);
	         sprintf(qval[DATA_GRP_NAME]," = '%s'",argv[4]);
		 selval[DATA_ID] = 1;
		break;	

       case 19 : 
		 selval[DATA_ID] = 1;
		break;	
       case 20 : 
	         sprintf(qval[ACCESS_CONSTRAINT]," = '%s'",argv[5]);
		 selval[DATA_ACCESS_LIST] = 1;
		 i = get_data_dir_info(MDAS_CATALOG, qval, selval, &myresult,
		       dcs_tname, dcs_aname, 100);
		 if (myresult == NULL) 
		   {
		     printf("Main: Access constraint not in catalog:%i\n",i );
		     close_db2_interaction(MDAS_CAT_COMMIT);
		     exit(-1);	 
		   }
		 access_list = (char *) get_from_result_struct
		   (myresult, dcs_tname[DATA_ACCESS_LIST], 
		    dcs_aname[DATA_ACCESS_LIST]);
		 selval[DATA_ACCESS_LIST] = 0;
	         sprintf(qval[USER_NAME], " = '%s'",argv[2]);
	         sprintf(qval[DATA_NAME], " = '%s'",argv[3]);
	         sprintf(qval[DATA_GRP_NAME]," like '%%%s%%'",argv[4]);
		 sprintf(qval[ACCESS_CONSTRAINT]," in (%s)",access_list);
		 sprintf(qval[DOMAIN_DESC]," = '%s'",argv[6]);
		 selval[DATA_TYP_NAME] = 1;
		 selval[PATH_NAME] = 1;
		 selval[RSRC_NAME] = 1;
		 selval[RSRC_ADDR_NETPREFIX] = 1;
		 selval[RSRC_TYP_NAME] = 1;

		break;	
        default :
                fprintf(stderr,"Unknown Option: %s\n", argv[1]);
                fprintf(stderr,"Use test_srb_mdas_dir <option-code> <name>\n");
		close_db2_interaction(MDAS_CAT_COMMIT);
		exit(-1);
                break;
   }

     i = get_data_dir_info(MDAS_CATALOG, qval, selval, &myresult,
		       dcs_tname, dcs_aname, numrows);
     if (myresult == NULL) 
       {
	 printf("SQL:%s\n",GENERATEDSQL);
	 printf("Main: Missing object data  in database:%i\n",i );
	 free(myresult);
	 close_db2_interaction(MDAS_CAT_COMMIT);
	 exit(-1);	 
       }
     printf("COLCNT:%i\n",myresult->result_count);
     printf("ROWCNT:%i\n",myresult->row_count);
     printf("STMTHN:%i\n",myresult->continuation_index);
     print_results();

     cont_index = myresult->continuation_index; 
     /*     while (cont_index >= 0)*/
     printf("SQL:%s\n",GENERATEDSQL);
      if (cont_index >= 0)
       {
	 i = get_more_rows(MDAS_CATALOG, cont_index, &myresult, numrows);
	 if (myresult == NULL || i != 0) 
	   {
	     free(myresult);
	     close_db2_interaction(MDAS_CAT_COMMIT);
	     exit (-1);
	   }
	 printf("COLCNT:%i\n",myresult->result_count);
	 printf("ROWCNT:%i\n",myresult->row_count);
	 printf("STMTHN:%i\n",myresult->continuation_index);
	 cont_index = myresult->continuation_index; 
	 print_results();

       }

close_db2_interaction(MDAS_CAT_COMMIT);

}

print_results()
{
     printf("\n\n -------------------------------- RESULT -------------------- \n");
    for (i=0; i < MAX_DCS_NUM; i++) {
        if(selval[i] > 0)
             rval[i] = (char *) get_from_result_struct
               (myresult, dcs_tname[i], dcs_aname[i]);
     }
    j = 0;
    alive = 1;
    while (alive && j < MAX_ROW) {
        for (i=0; i < MAX_DCS_NUM; i++) {
            if(selval[i] > 0) {
                if(j < myresult->row_count)
                     printf("%s :%s\n",dcs_aname[i],
                       (char *)((int) rval[i]+ j*MAX_DATA_SIZE));
                else {
                    alive = 0; 
                    break;
                }
             }
        }
        j++;
        printf("----------------------------------------------------------\n");
    }
    printf("Number of rows: %i\n", --j);
    if (!alive) printf("End of Answer\n");
    else printf("Possibly there are more answers\n");
    for (i = 0; i < MAX_DCS_NUM; i++) {
      if(selval[i] > 0)
        free(rval[i]);
    }
   free(myresult);

}  
      
        
     


