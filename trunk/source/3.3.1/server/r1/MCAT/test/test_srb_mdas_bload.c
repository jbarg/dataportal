#include "mdasPrototypes.h"
#include "mcatAdmin.h"
#define MAX_LINES 2020
main(int argc,
        char* argv[])
{  
  int i, j;
  mdasC_sql_result_struct inDataInfo;
  char buf[MAX_TOKEN];
  FILE *fd;

  if ( argc  != 6 ) {
    printf("Error: Use test_srb_mdas_bload  <load-file> <container-name>  <user-name> <domain-name> <password>\n");
    printf("The format of the <load-file> is as follows:\n");
    printf("LINE: dataName collectionName dataSize offSet\n");
    printf("Number of Lines not to exceed: %d\n",MAX_LINES);
    exit(-1);
  }
  
  if ((fd =  fopen( argv[1] , "r")) == NULL) {
    printf("Unable to Open Input Metadata File\n");
    exit(-1);
  }
  j = 0;
  inDataInfo.sqlresult[0].values = (char *)  
	    malloc (MAX_LINES * MAX_DATA_SIZE * sizeof(char));
  inDataInfo.sqlresult[1].values = (char *)  
	    malloc (MAX_LINES * MAX_DATA_SIZE * sizeof(char));
  inDataInfo.sqlresult[2].values = (char *)  
	    malloc (MAX_LINES * MAX_DATA_SIZE * sizeof(char));
  inDataInfo.sqlresult[3].values = (char *)  
	    malloc (MAX_LINES * MAX_DATA_SIZE * sizeof(char));
  while ((i = getNextStr(fd, buf, sizeof(buf))) != EOF  && j < MAX_LINES) {
    strcpy((char *)
	   (((int) (inDataInfo.sqlresult[0].values)) + (j * MAX_DATA_SIZE)),
	    buf);
    getNextStr(fd, buf, sizeof(buf));
    strcpy((char *)
	   (((int) (inDataInfo.sqlresult[1].values)) + (j * MAX_DATA_SIZE)),
	    buf);
    getNextStr(fd, buf, sizeof(buf));
    strcpy((char *)
	   (((int) (inDataInfo.sqlresult[2].values)) + (j * MAX_DATA_SIZE)),
	    buf);
    getNextStr(fd, buf, sizeof(buf));
    strcpy((char *)
	   (((int) (inDataInfo.sqlresult[3].values)) + (j * MAX_DATA_SIZE)),
	    buf);
    j++;
  }
  fclose(fd);
  inDataInfo.row_count = j;
  /*
printf("%s %s %s %s\n",
       (char *)(((int)(inDataInfo.sqlresult[0].values))+((j-1)*MAX_DATA_SIZE)),
       (char *)(((int)(inDataInfo.sqlresult[1].values))+((j-1)*MAX_DATA_SIZE)),
       (char *)(((int)(inDataInfo.sqlresult[2].values))+((j-1)*MAX_DATA_SIZE)),
       (char *)(((int)(inDataInfo.sqlresult[3].values))+((j-1)*MAX_DATA_SIZE)));
  */
  i = open_db2_interaction(MDAS_CATALOG);
  if (i != 0) {
    printf("Opening Error:%i\n",i);
    exit(-1);
  }
  printf("opened\n");
  i= get_mdas_authorization(argv[3],argv[5], argv[4]);
  if (i != 0) {
    printf("Authorization Error:%i\n",i);
    exit(-1);
  }
  printf("authorized\n");
  
  i = 0;


  printf("REGISTRATION STARTED...\n");
  i= registerIncontainerDatasetInBulk(MDAS_CATALOG, argv[3],argv[4],
				      argv[2], &inDataInfo);
  if (i != 0) {
      printf("ERROR: %i\n",i);
      close_db2_interaction(1);
  }
  else {
    printf("DONE\n");
    close_db2_interaction(MDAS_CAT_COMMIT);
  }
  
}
  
      
        
     


