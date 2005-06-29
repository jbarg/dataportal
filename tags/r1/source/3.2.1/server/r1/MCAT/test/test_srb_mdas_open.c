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
  int i, actual_attr_set_size;
  mdas_metadata_attr_entry *attr_info;
  char schemaset[300];
  char *attrstr;
  clock_t mmmmcl;
  mcatContextDesc cd;

   strcpy(schemaset,"");
   i = open_db2_interaction(MDAS_CATALOG);
   if (i != 0) {
     printf("Opening Error:%i\n",i);
     exit(-1);
   }
   printf("Opened\n"); fflush(stdout);

   if (argc > 1) {
     for (i= 1; i < argc; i++) {
       if (i > 1) strcat(schemaset,",");
       strcat(schemaset,"'");
       strcat(schemaset,argv[i]);
       strcat(schemaset,"'");
     }  
     cd = initializeMcatContext(schemaset);
   }
     printAttrEntrySet(svrMcatContext[cd].attr_info,
		       svrMcatContext[cd].max_attr_entries);
     printGraphPathEntrySet(svrMcatContext[cd].graph_info,
	    svrMcatContext[cd].max_graph_entries);

     freeMcatContext(cd);
     

/*
   i = get_mkrelations();
   if (i != 0) {
     printf("Error when getting mkrelations:%i\n",i);
     exit(-1);
   }
   printf("get_mkrelations Done\n"); fflush(stdout);
   printf("MK RELATIONS:\n");
   print_mkrelations();


   if (argc == 1) strcpy(schemaset,"'mcatcore'");
   else 
     for (i= 1; i < argc; i++) {
       if (i > 1) strcat(schemaset,",");
       strcat(schemaset,"'");
       strcat(schemaset,argv[i]);
       strcat(schemaset,"'");
     }  
     
     i = allocAttrEntrySet(attr_info,  200);
     if (i != 0) {
     printf("Error in allocAttrEntrySet:%i\n",i);
     exit(-1);
     }
     i = syncAttrEntrySet(schemaset, 200 
		 &actual_attr_set_size,attr_info);
     if (i != 0) {
     printf("Error in syncAttrEntrySet:%i\n",i);
     exit(-1);
     }
     printf("get_mdas_attributes_metadata Done\n"); fflush(stdout);
     printf("MDAS ATTRIBUTES METADATA:\n");
     printAttrEntrySet(attr_info,actual_attr_set_size);
     i = freeAttrEntrySet(attr_info,  actual_attr_set_size);
     if (i != 0) {
     printf("Error in freeAttrEntrySet:%i\n",i);
     exit(-1);
     } 

   if (argc == 1) strcpy(schemaset,"'mcatcore'");
   else 
     for (i= 1; i < argc; i++) {
       if (i > 1) strcat(schemaset,",");
       strcat(schemaset,"'");
       strcat(schemaset,argv[i]);
       strcat(schemaset,"'");
     }  
   mmmmcl = clock();
   printf("%i\n",mmmmcl);
   i = get_fkrelgraph( schemaset);
   mmmmcl = clock();
   printf("%i\n",mmmmcl);
   if (i != 0) {
     printf("Error when getting mdas fkrelations graph:%i\n",i);
     exit(-1);
   }
   printf("MDAS MKREL GRAPH:\n");
   print_fkrelgraph();
   mmmmcl = clock();
   printf("%i\n",mmmmcl);
*/
}
  
      
        
     


