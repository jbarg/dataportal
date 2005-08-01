/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

#include "mdasPrototypes.h"
#include "mdasGlobalsExtern.h"
#include "mcatAdmin.h"

  int i;
  time_t mmmmcl;
  char m_user_name[MAX_TOKEN];
  char data_name[MAX_TOKEN];
  char access_name[MAX_TOKEN];
  char domain_name[MAX_TOKEN];
  char user_pasword[MAX_TOKEN];
  char collection_name[MAX_TOKEN];
  char resource_name[MAX_TOKEN];
  mdasC_sql_result_struct  *myresult;
  char cellval[MAX_TOKEN];

  char *resourceType;
  char *resourceLoc;
  char *resourceClass;
  int  *resourceMaxSize;
  char *res_data_name;
  char *res_data_path_name;
  char *res_rsrc_name;
  char *res_rsrc_typ_name;
  char *res_rsrc_locn_name;
  char *res_data_typ_name;




rungetDatasetInfo()
{
	i = getDatasetInfo(MDAS_CATALOG, data_name,m_user_name, access_name, 
			   domain_name, collection_name, &myresult,MAX_ROW);
     if (myresult == NULL || i != 0) 
       {
	 printf("Main: Missing object data  in database: %i\n",i);
	 return;
       }
     printf("\n\n ---------------------------- RESULT ------------------- \n");
   res_data_name = (char *) m_user_name;
   sprintf(cellval,"%sMDAS_ADC_REPL",MDASSCHEMENAME);
   res_data_path_name = (char *) get_from_result_struct(
             (mdasC_sql_result_struct *) myresult, cellval,"path_name");
   sprintf(cellval,"%sMDAS_TD_DATA_TYP",MDASSCHEMENAME);
   res_data_typ_name = (char *)get_from_result_struct(
             (mdasC_sql_result_struct *) myresult, cellval,"data_typ_name");
   sprintf(cellval,"%sMDAS_AR_PHYSICAL",MDASSCHEMENAME);
   res_rsrc_name = (char *)get_from_result_struct(
             (mdasC_sql_result_struct *) myresult, cellval,"phy_rsrc_name");
   sprintf(cellval,"%sMDAS_TD_RSRC_2TYP",MDASSCHEMENAME);
   res_rsrc_typ_name = (char *)get_from_result_struct(
             (mdasC_sql_result_struct *) myresult, cellval,"rsrc_typ_name");
   sprintf(cellval,"%sMDAS_TD_LOCN",MDASSCHEMENAME);
   res_rsrc_locn_name = (char *)get_from_result_struct(
             (mdasC_sql_result_struct *) myresult, cellval,"netprefix");
   /*   sprintf(cellval,"%sMDAS_TD_RSRC_CLASS",MDASSCHEMENAME);
   res_rsrc_class = (char *)get_from_result_struct(
             (mdasC_sql_result_struct *) myresult, cellval,"rsrc_class_name");
   sprintf(cellval,"%sMDAS_AR_REPL",MDASSCHEMENAME);
   res_rsrc_maxsize =  (char *)get_from_result_struct(
             (mdasC_sql_result_struct *) myresult, cellval,"max_obj_size");
   */
   i = 0;
   while (i < myresult->row_count)
     {
       printf("DATA NAME     : %s\n",res_data_name);
       printf("DATA TYPE NAME: %s\n",res_data_typ_name + i*  MAX_DATA_SIZE);
       printf("DATA PATH NAME: %s\n",res_data_path_name + i*  MAX_DATA_SIZE);
       printf("RESOURCE NAME : %s\n",res_rsrc_name + i*  MAX_DATA_SIZE);
       printf("RESOURCE TYPE : %s\n",res_rsrc_typ_name + i*  MAX_DATA_SIZE);
       printf("RESOURCE LOCN : %s\n",res_rsrc_locn_name + i*  MAX_DATA_SIZE);
       i++;
       printf(" ------------------------------------------------------------ \n");
     }

   free(res_data_path_name);
   free(res_rsrc_name);
   free(res_rsrc_typ_name);
   free(res_rsrc_locn_name);
   free(myresult);
}
runresAttrLookup()
{
int rowcount;
        i = resAttrLookup(MDAS_CATALOG, resource_name, domain_name, 
			  &resourceType, &resourceLoc, 
			  &resourceClass, &resourceMaxSize,
			  MAX_ROW, &rowcount);
     if ( i != 0) 
       {
         printf("Main: Missing Resource  in database: %i\n",i);
         return;
       }
     printf("\n\n ---------------------------- RESULT ------------------- \n");
   i = 0;
 
   while (i < rowcount)
    {
       printf("RESOURCE TYPE: %s\n", resourceType + i*  MAX_DATA_SIZE);
       printf("RESOURCE LOCN: %s\n", resourceLoc + i*  MAX_DATA_SIZE);
       printf("RESOURCE CLAS: %s\n", resourceClass + i*  MAX_DATA_SIZE);
       printf("RESOURCE SIZE: %i\n", resourceMaxSize[i]);
       i++;
       printf(" ------------------------------------------------------------ \n");
     }
 
   free(resourceType);
   free(resourceLoc);
   free(resourceClass);
   free(resourceMaxSize);

}
main(int argc,
        char* argv[])
{  

   if ( argc  != 8) {
	  printf("Error: Use test_srb_mdas_all <dataset-name> <user-name> <passwd> <access-name> <domain-name> <collection-name> <resource-name>\n");
	  exit(-1);
	}

	sprintf(data_name,"%s",argv[1]);
        sprintf(m_user_name,"%s",argv[2]);
	sprintf(user_pasword,"%s",argv[3]);
        sprintf(access_name,"%s",argv[4]);
        sprintf(domain_name,"%s",argv[5]);
	sprintf(collection_name,"%s",argv[6]);
	sprintf(resource_name,"%s",argv[7]);

	i = open_db2_interaction(MDAS_CATALOG);
	if (i != 0) {
	  printf("Opening Error:%i\n",i);
	  exit(-1);
	}
	printf("opened\n");

	i= get_mdas_authorization(m_user_name,user_pasword, domain_name);
	if (i != 0) {
	  printf("Authorization Error:%i\n",i);
	  exit(-1);
	}
	printf("authorized\n");

	rungetDatasetInfo();
	runresAttrLookup();
	rungetDatasetInfo();
	runresAttrLookup();
	rungetDatasetInfo();
	runresAttrLookup();
	rungetDatasetInfo();
	runresAttrLookup();

	close_db2_interaction(MDAS_CAT_COMMIT);


}
  
      
        
     


