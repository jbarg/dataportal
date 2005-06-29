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

void
usage()
{
  printf("Usage (create new schema):test_srb_mdas_createschema 0 <schemaname> <subject> <comments> <constraints> <similarto>\n");
  printf("Usage (insert new  token): test_srb_mdas_createschema 1 <schemaname> <token-subject> <token-constraints> <attr-name1> <attr-data-type1> <attr-type1> <default-value1> <comments1> <presentation1> [<attr-name2> <attr-data-type2> <attr-type2> <default-value2> <comments2> <presentation2> ...]\n");
  printf("Usage (insert new attrib): test_srb_mdas_createschema 2 <schemaname> <clump-subject> <clump-constraints> <attr-name> <attr-data-type> <attr-type> <default-value> <comments> <presentation> <same-as> <primary-key> [<attr-name> <attr-data-type> <attr-type> <default-value> <comments> <presentation> <same-as> <primary-key> ... ] \n");
  exit(0);
}

main(int argc,
        char* argv[])
{  
  int i,k;
  clock_t mmmmcl;
  mcatContextDesc cd;
  char empStr[10];
  char *schemaName; 
  int attrCount = 0;
  char *attrExtrName[MAX_TABLE_COLS];
  char *attrDataType[MAX_TABLE_COLS];
  char *attrIdenType[MAX_TABLE_COLS];
  char *defaultVal[MAX_TABLE_COLS];
  char *attrComments[MAX_TABLE_COLS];
  char *attrPresentation[MAX_TABLE_COLS];
  char *attrSameas[MAX_TABLE_COLS];
  int   primaryKey[MAX_TABLE_COLS];
  char *subjects;
  char *constraints;
  

  if (argc <= 6) usage();
  strcpy(empStr,"");
  switch (atoi(argv[1])) {
     case 0:  
            if (argc != 7) usage();
	    printf("Opening MCAT interaction\n"); 
	    i = open_db2_interaction(MDAS_CATALOG);
	    if (i != 0) {
	      printf("Opening Error:%i\n",i);
	      exit(-1);
	    }
	    i = createNewSchema(argv[2], argv[3], argv[4],argv[5], argv[6]);
	    if (i != 0) printf ("Schema Insertion Error:%i\n", i);
	    else printf ("Schema Insertion Successful\n");
	    break;
     case 1:  
            if (((argc - 5) % 6) != 0) usage();
	    printf("Opening MCAT interaction\n"); 
	    i = open_db2_interaction(MDAS_CATALOG);
	    if (i != 0) {
	      printf("Opening Error:%i\n",i);
	      exit(-1);
	    }
	    schemaName = argv[2]; 
	    subjects = argv[3];
	    constraints = argv[4];
	    k =  5;
	    while (k < argc) {
	      attrExtrName[attrCount] = argv[k++];
	      attrDataType[attrCount] = argv[k++];
	      attrIdenType[attrCount] = argv[k++];
	      defaultVal[attrCount] = argv[k++];
	      attrComments[attrCount] = argv[k++];
	      attrPresentation[attrCount] = argv[k++];
	      attrCount++;	      
	    }
	    i = addTokenAttrInSchemaByUser(schemaName, attrCount, 
			      attrExtrName, attrDataType,
			      attrIdenType, defaultVal,
			      attrComments, attrPresentation,
			      subjects,	constraints, "sekar","sdsc");
	      if (i != 0) printf ("Token Insertion Error:%i\n", i);
	      else printf ("Token Insertion Successful\n");
	      break;
    case 2:  
            if (((argc - 5) % 8) != 0) usage();
	    printf("Opening MCAT interaction\n"); 
	    i = open_db2_interaction(MDAS_CATALOG);
	    if (i != 0) {
	      printf("Opening Error:%i\n",i);
	      exit(-1);
	    }
	    schemaName = argv[2]; 
	    subjects = argv[3];
	    constraints = argv[4];
	    k =  5;
	    while (k < argc) {
	      attrExtrName[attrCount] = argv[k++];
	      attrDataType[attrCount] = argv[k++];
	      attrIdenType[attrCount] = argv[k++];
	      defaultVal[attrCount] = argv[k++];
	      attrComments[attrCount] = argv[k++];
	      attrPresentation[attrCount] = argv[k++];
	      attrSameas[attrCount] = argv[k++];
	      primaryKey[attrCount] = atoi(argv[k++]);
	      attrCount++;	      
	    }


	    i = addAttrClusterInSchemaByUser(schemaName, attrExtrName,attrCount, 
			      attrExtrName, attrDataType,
			      attrIdenType, defaultVal,
			      attrComments, attrPresentation,
			      attrSameas,primaryKey,
			      subjects,	constraints,"sekar","sdsc");
	      if (i != 0) printf ("Token Insertion Error:%i\n", i);
	      else printf ("Token Insertion Successful\n");
	      break;
    default: 
            i = -1;
            printf("Unknown Option: %i\n", atoi(argv[1]));
	    exit(-1);
	    break;
   }
   
   if (i != MDAS_SUCCESS) 
     close_db2_interaction(MDAS_CAT_ROLLBACK);
   else
     close_db2_interaction(MDAS_CAT_COMMIT);
}
  
      
        
     


