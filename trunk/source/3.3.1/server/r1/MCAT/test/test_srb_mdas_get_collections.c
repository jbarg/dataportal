/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

#include "mdasPrototypes.h"
#include "mcatAdmin.h"



mdasC_sql_result_struct  *myresult;
char *grp_name;
int i, numrows, cont_index;

main(int argc,
        char* argv[])
{  

  time_t mmmmcl;
   



	if ( argc  != 6 && argc  != 7) {
	  printf("Error: Use test_srb_mdas_get_collections <flag> <group-name> <user-name> <passwd>  <domain-name> <rows-required>\n");
	  exit(-1);
	}

	i = open_db2_interaction(MDAS_CATALOG);
	if (i != 0) {
	  printf("Opening Error:%i\n",i);
	  exit(-1);
	}
	printf("opened\n");
	i= get_mdas_authorization(argv[3], argv[4], argv[5]);
	if (i != 0) {
	  printf("Authorization Error:%i\n",i);
	  exit(-1);
	}
	printf("authorized\n");
	if (argc == 6) numrows = 100;
	else numrows =  atoi(argv[6]);
	i = get_collections(MDAS_CATALOG, argv[2], argv[1], &myresult,numrows);
     if ( i != 0) 
       {
	 printf("Main: Missing collection  in database: %i\n",i);
	 free(myresult);
	 exit (-1);
       }

	 printf("COLCNT:%i\n",myresult->result_count);
	 printf("ROWCNT:%i\n",myresult->row_count);
	 printf("STMTHN:%i\n",myresult->continuation_index);
     print_results();
     cont_index = myresult->continuation_index; 
     while (cont_index >= 0)
       {
	 i = get_more_rows(MDAS_CATALOG,cont_index, &myresult, numrows);
	 if (myresult == NULL || i != 0) 
	   {
	     if (i != 0)
	       {
		 
	       	 printf("COLCNT:%i:%i\n",myresult->result_count,i);
		 printf("ROWCNT:%i\n",myresult->row_count);
		 printf("STMTHN:%i\n",myresult->continuation_index);
		 free(myresult);
	       }

		  close_db2_interaction(MDAS_CAT_COMMIT);
		  exit (-1);
	   }
	 printf("COLCNT:%i\n",myresult->result_count);
	 printf("ROWCNT:%i\n",myresult->row_count);
	 printf("STMTHN:%i\n",myresult->continuation_index);
	 print_results();
	 cont_index = myresult->continuation_index; 
       }

   free(myresult);
   close_db2_interaction(MDAS_CAT_COMMIT);


}

print_results()
{
     grp_name = myresult->sqlresult[0].values;

     printf("\n\n ---------------------------- RESULT ------------------- \n");
     /*printf("%s.%s\n",myresult->sqlresult[0].tab_name,
       myresult->sqlresult[0].att_name);*/

   i = 0;
   while ( i < myresult->row_count)
     {
       printf(" %s\n",  
	      grp_name + i*  MAX_DATA_SIZE);

       i++;

     }
printf("\n\n ------------------------------------------------------- \n");

   free(grp_name);
}  
      
        
     


