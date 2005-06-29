/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

#include "mdasPrototypes.h"
#include "mcatAdmin.h"

extern mcatContext svrMcatContext[];
main(int argc,
        char* argv[])
{  
  int i, counta, actual_attr_set_size;
  mdas_metadata_attr_entry *attr_info;
  char schemaset[300];
  char *attrstr;
  char *attr_value[300];
  int attr_id[300];
  mdas_metadata_attr_entry *AttrEntries;
  clock_t mmmmcl;
  mcatContextDesc cd;

   strcpy(schemaset,"");

      if (argc <= 1) {
	printf("Usage: %s attrcnt attrnum attrval ... attrnum attrval schemaname ...schemaname\n",
	       argv[0]);
	exit(0);
      }
      for (i= 0; i < atoi(argv[1]); i++) {
	attr_id[i] = atoi(argv[(2*i)+2]);
	attr_value[i] = 
	  (char *)malloc(sizeof(char) * (strlen(argv[(2*i)+3]) + 1));
	strcpy(attr_value[i], argv[(2*i)+3]);
      }
      for (i = (2*atoi(argv[1]))+2; i < argc ; i++) {
       if (i > ((2*atoi(argv[1]))+2)) strcat(schemaset,",");
       strcat(schemaset,"'");
       strcat(schemaset,argv[i]);
       strcat(schemaset,"'");
     }  


   i = open_db2_interaction(MDAS_CATALOG);
   if (i != 0) {
     printf("Opening Error:%i\n",i);
     exit(-1);
   }

   if(strlen(schemaset) > 0) 
     cd = initializeMcatContext(schemaset);
   else
     cd = 0;

   if (cd < 0) {
     printf("Error in Initializing Mcat Contex: %i\n",cd);
     close_db2_interaction(MDAS_CAT_ROLLBACK);
     exit(-1);
   }

   i = insertIntoSchema(cd, 0, 0, attr_id, attr_value, atoi(argv[1]));

   if (i != 0) {
     printf("Error in Inserting into Schema: %i\n",i);
     close_db2_interaction(MDAS_CAT_ROLLBACK);
     exit(-1);
   }

   /*
     printAttrEntrySet(svrMcatContext[cd].attr_info,
		       svrMcatContext[cd].max_attr_entries);
     printGraphPathEntrySet(svrMcatContext[cd].graph_info,
	    svrMcatContext[cd].max_graph_entries);

    i  =GetAttributesfromMcatContext(cd, &counta, &AttrEntries);
   if (i != 0) {
     printf("Error in GetAttributesfromMcatContext: %i\n",i);
     freeMcatContext(cd);
     close_db2_interaction(MDAS_CAT_ROLLBACK);
     exit(-1);
   }

printf("After GetAttributesfromMcatContext:%i:%i\n",cd, counta);
    printAttrEntrySet(AttrEntries, counta);
   */
     freeMcatContext(cd);
     close_db2_interaction(MDAS_CAT_COMMIT);
}
  
      
        
     


