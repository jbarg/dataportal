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



main(int argc,
        char* argv[])
{  
  int i;
  char qval[MAX_DCS_NUM][MAX_TOKEN];
  int selval[MAX_DCS_NUM];
  mdasC_sql_result_struct  *myresult;
  int j, alive;
  char *rval[MAX_DCS_NUM];
  char fullPathName[MAX_ROW][HUGE_STRING];
  char *defPathName;
  char *phyDefPathName;
  char cval[MAX_TOKEN];


        if ( argc != 8 ) 
	  {
	    printf(
	       "Error: Use test_srb_mdas_get_rsrc <rsrc-name> <user-name> <passwd> <domain-name> <data-name> <collection-name> <path-name>\n");
	    exit(-1);
	  }

	i = open_db2_interaction(MDAS_CATALOG);
	if (i != 0) {
	  printf("Opening Error:%i\n",i);
	  exit(-1);
	}

	printf("opened\n");
	i= get_mdas_authorization(argv[2],argv[3],argv[4]);
	if (i != 0) {
	  printf("Authorization Error:%i\n",i);
	  exit(-1);
	}
	printf("authorized\n");
       for (i = 0; i < MAX_DCS_NUM; i++)
         {
           sprintf(qval[i],"");
           selval[i] = 0;
         }

	sprintf(qval[RSRC_NAME]," = '%s'",argv[1]);
	selval[RSRC_NAME] = 1;
	selval[RSRC_ADDR_NETPREFIX] = 1;
	selval[RSRC_TYP_NAME] = 1;
	selval[RSRC_DEFAULT_PATH] = 1;
	selval[RSRC_REPL_ENUM] = 1;
	selval[PHY_RSRC_DEFAULT_PATH] = 1;
	selval[PHY_RSRC_NAME] = 1;
	selval[PHY_RSRC_TYP_NAME] = 1;

	distinct_flag = 0;
     i = get_data_dir_info(MDAS_CATALOG, qval, selval, &myresult,
                       dcs_tname, dcs_aname,MAX_ROW);
	distinct_flag = 1;

 
     if (myresult == NULL) 
       {
         printf("Main: Missing resource  in catalog:%i\n",i );
         exit(-1);       
       }

  sprintf(cval, "%sMDAS_AR_PHYSICAL",  MDASSCHEMENAME);
  phyDefPathName = (char *) get_from_result_struct(myresult,
		  cval,"phy_default_path");
  if(phyDefPathName == NULL) 
    {
      printf("Main: Error in default Path\n");
      exit(-1);
    }
  sprintf(cval, "%sMDAS_AR_REPL",  MDASSCHEMENAME);
  defPathName = (char *) get_from_result_struct(myresult,
		  cval,"default_path");
  if(defPathName == NULL) 
    {
      printf("Main: Error in default Path\n");
      exit(-1);
    }
    j=0;
    while (j < myresult->row_count)
      {

   i = getFullPathName(defPathName, phyDefPathName, argv[2],argv[4],
		   argv[7],argv[5],argv[6], fullPathName[j]);
		   	
	phyDefPathName += MAX_DATA_SIZE;
	defPathName += MAX_DATA_SIZE;
	j++;
      }
   
   if (i < 0)
       {
         printf("Main: Error in generating Full Path Name:%i\n",i );
         exit(-1);       
       }


     printf("\n\n --------------------------- FULLPATH RESULTS ---------------- \n")
;
    for (i=0; i < MAX_DCS_NUM; i++) {
        if(selval[i] > 0)
             rval[i] = (char *) get_from_result_struct
               (myresult, dcs_tname[i], dcs_aname[i]);
     }
    j = 0;
    alive = 1;
    while (alive && j < myresult ->row_count) {
        for (i=0; i < MAX_DCS_NUM; i++) {
            if(selval[i] > 0) {
                     printf("%s :%s\n",dcs_aname[i],
                       (char *)((int) rval[i]+ j*MAX_DATA_SIZE));
	    }
        }
	/* if (alive) printf("full_path_name :%s\n",fullPathName[j]); */
	if (alive) printf("full_path_name : not shown for sake of diff\n");
        j++;
        printf("----------------------------------------------------------\n");
    }
    printf("Number of rows: %i\n", --j);
    if (j < MAX_ROW) printf("End of Answer\n");
    else printf("Possibly there are more answers\n");
    for (i = 0; i < MAX_DCS_NUM; i++) {
      if(selval[i] > 0)
        free(rval[i]);
 }
   free(myresult);
   close_db2_interaction(MDAS_CAT_COMMIT);
}
  
      
        
     


