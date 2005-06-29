

































#include "mdasPrototypes.h"
#include "mcatAdmin.h"

int usage()
{
          printf("Error: Use test_srb_mdas_compound_obj <opr_typ> <user-name> <passwd> <domain-name> [ <param1> ... ] \n");
	  printf("Opr_typ=1 Opr=isCompoundObjectByName Param=comObjName,comObjCollName\n");
	  printf("Opr_typ=2 Opr=registerInternalCompoundObject Param=objName,objCollName,objReplNum,objSegNum,resourceName,dataPathName,dataSize,offset,intReplNum,intSegNum,userName,domainName\n");
	  printf("Opr_typ=3 Opr= getInternalReplicaInfo Param=objName,objCollName,objReplNum,objSegNum,rowsWanted\n");
	  printf("Opr_typ=4 Opr= getInternalReplicaInfoGivenPathOfOne Param=pathName,rowsWanted\n");
	  printf("Opr_typ=5 Opr= removeCompoundObject Param=objName,objCollName,objReplNum,objSegNum,userName,domainName\n");
	  printf("Opr_typ=6 Opr= removeInternalCompoundObject Param=objName,objCollName,objReplNum,objSegNum,intReplNum,intSegNum,userName,domainName\n");
	  printf("Opr_typ=7 Opr= modifyInternalCompoundObject Param=objName,objCollName,objReplNum,objSegNum,intReplNum,intSegNum,userName,domainName,data_value_1,data_value_2,data_value_3,data_value_4,retraction_type\n");

}


main(int argc,
        char* argv[])
{  
     int i;
     svrConn *conn;
     mdasC_sql_result_struct  *myResult;
       if ( argc  < 6) {
	 usage();
	 exit(-1);
       }

       i = open_db2_interaction(MDAS_CATALOG);
        if (i != 0) {
          printf("Opening Error:%i\n",i);
          exit(-1);
        }
        printf("opened\n");
        i= get_mdas_authorization(argv[2], argv[3], argv[4]);
        if (i != 0) {
          printf("Authorization Error:%i\n",i);
          exit(-1);
        }
        printf("authorized\nPerforming Operation %s ...",argv[1]);
        switch(atoi(argv[1])) {
	case 1:
	  if (argc != 7) {usage();exit(-1);}
	  i = isCompoundObjectByName(conn,argv[5],argv[6]);
	  break;
	case 2:
	  if (argc != 17) {usage();exit(-1);}
	  i = registerInternalCompoundObject(conn,argv[5],argv[6],atoi(argv[7]),atoi(argv[8]),argv[9],argv[10],strtoll(argv[11], (char **)NULL, 10),strtoll(argv[12], (char **)NULL, 10),atoi(argv[13]),atoi(argv[14]),argv[15],argv[16]);
	  break;
	case 3:
	  if (argc != 10) {usage();exit(-1);}
	  i = getInternalReplicaInfo(conn,argv[5],argv[6],atoi(argv[7]),atoi(argv[8]),&myResult, atoi(argv[9]));
	  break;
	case 4:
	  if (argc != 7) {usage();exit(-1);}
	  i = getInternalReplicaInfoGivenPathOfOne(conn,argv[5],&myResult,atoi(argv[6]));
	  break;
	case 5:
	  if (argc != 11) {usage();exit(-1);}
	  i = removeCompoundObject(conn,argv[5],argv[6],atoi(argv[7]),atoi(argv[8]),argv[9],argv[10]);
	  break;
	case 6:
	  if (argc != 13) {usage();exit(-1);}
	  i = removeInternalCompoundObject(conn,argv[5],argv[6],atoi(argv[7]),atoi(argv[8]),atoi(argv[9]),atoi(argv[10]),argv[11],argv[12]);
	  break;
	case 7:
	  if (argc != 18) {usage();exit(-1);}
	  i = modifyInternalCompoundObject(conn,argv[5],argv[6],atoi(argv[7]),atoi(argv[8]),atoi(argv[9]),atoi(argv[10]),argv[11],argv[12],argv[13],argv[14],argv[15],argv[16],atoi(argv[17]));
	  break;
	default:
	  printf("Unknown Option\n");
	  usage();
	  exit(-1);
	  break;
	}
	printf("\n Retval: %i ... \n", i);
	printf("done\n ");
	if (atoi(argv[1]) == 3 ||atoi(argv[1]) ==   4) 
	  print_results(myResult);
	  
	close_db2_interaction(MDAS_CAT_COMMIT);
	exit(0);
}


  
      
        
     


print_results(mdasC_sql_result_struct  *myResult)
{
  int i,j;
  char *jchar;

     printf("\n\n -------------------------- RESULT -------------------- \n");
     for (i = 0; i < myResult->result_count ; i++) {
       jchar = myResult->sqlresult[i].values;
       printf("%i:%s.%s::::\n",i,myResult->sqlresult[i].tab_name,
		myResult->sqlresult[i].att_name);
       for (j =  0; j < myResult->row_count ; j++) {
         printf("%i:%s\n",j,(char *)((int) jchar + j*MAX_DATA_SIZE));
       }
     }
}
      
        
