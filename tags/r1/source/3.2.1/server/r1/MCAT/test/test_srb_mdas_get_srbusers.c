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
char *srbusers_name;
int i, numrows, cont_index;


main(int argc,
        char* argv[])
{  
  int i;

        if ( argc != 1 ) 
	  {
	    printf(
	       "Error: Use test_srb_mdas_get_srbuser\n");
	    exit(-1);
	  }

	i = open_db2_interaction(MDAS_CATALOG);
	if (i != 0) {
	  printf("Opening Error:%i\n",i);
	  exit(-1);
	}

	printf("opened\n");

     i = getSRBUserInfo(MDAS_CATALOG, &myresult, 100);

     if (i < 0)
       {
         printf("Main: Missing srbusers  in catalog:%i\n",i );
         free(myresult);
	 exit(-1);       
       }

	 printf("COLCNT:%i\n",myresult->result_count);
	 printf("ROWCNT:%i\n",myresult->row_count);
	 printf("STMTHN:%i\n",myresult->continuation_index);
     print_results();
     cont_index = myresult->continuation_index; 
     while (cont_index >= 0)
       {
	 i = get_more_rows(MDAS_CATALOG,cont_index, &myresult, 100);
	 if (myresult == NULL || i != 0) 
	   {
	     printf("Main: Missing object data  in database: %i\n",i);
	     free(myresult);
		  close_db2_interaction(MDAS_CAT_COMMIT);
		  exit (-1);
	   }
	 printf("COLCNT:%i\n",myresult->result_count);
	 printf("ROWCNT:%i\n",myresult->row_count);
	 printf("STMTHN:%i\n",myresult->continuation_index);
	 print_results();
	 cont_index = myresult->continuation_index; 
       }

    close_db2_interaction(MDAS_CAT_COMMIT);
}

print_results()
{
   i = 0;
   srbusers_name = myresult->sqlresult[0].values;
    while (i < myresult->row_count)
      {
       printf(" %s\n",  
	      srbusers_name + i*  MAX_DATA_SIZE);
	i++;
      }
    printf("%i srb users\n", i);
   free(srbusers_name);
   free(myresult);
}  
      
        
     


