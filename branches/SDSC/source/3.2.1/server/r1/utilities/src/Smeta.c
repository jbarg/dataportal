/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/



#include "scommands.h"

#define NUMMETADATA MAX_ADREPL_STRING_METADATA + MAX_ADREPL_INTEGER_METADATA

extern char srbAuth[];
extern char srbHost[];
extern char srbUser[];
extern char mdasDomainName[];
extern char mdasCollectionName[];
extern char inCondition[];

    srbConn *conn;
    int i, myself;
    
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    mdasC_sql_result_struct  myresult;
    int j, alive;
    char *rval[MAX_DCS_NUM];
    char dname[MAX_TOKEN];
    
    int D_case = 0;
    int I_case = 0;
   char collName[MAX_TOKEN];
    char dataName[MAX_TOKEN];
    int c_value = 0;


void
usage()
{
  fprintf(stderr,"Smeta -i  -I metaAttrNameEqValue  [ [-I metaAttrNameEqValue] ... ]  dataName\n");
  fprintf(stderr,"Smeta -u metadataIndex metaAttrNameEqValue dataName\n");
  fprintf(stderr,"Smeta -d metadataIndex dataName\n");
  fprintf(stderr,"Smeta -c -i  -I metaAttrNameEqValue  [ [-I metaAttrNameEqValue] ... ]  collectionName\n");
  fprintf(stderr,"Smeta -c -u metadataIndex metaAttrNameEqValue collectionName\n");
  fprintf(stderr,"Smeta -c -d metadataIndex collectionName\n");
  fprintf(stderr,"Smeta [-R] [-I metaAttrNameOrCondition [-I metaAttrNameOrCondition] ... ] [-Y n] [-L n] [-T dataType]  dataName|collectionName \n");
  fprintf(stderr,"Smeta -c [-R] [-I metaAttrNameOrCondition [-I metaAttrNameOrCondition] ... ] [-Y n] [-L n] [-T dataType]  [collectionName] \n");
}

int
stripEqandQuote(char *attrVal)
{
  int i,j;
  for (i = 0; i < strlen(attrVal) ; i++) {
    if (attrVal[i] != ' ' && attrVal[i] != '=') 
      break;
  }
  for (; i < strlen(attrVal) ; i++) {
    if (attrVal[i] != ' ' && attrVal[i] != '\'')
      break;
  }
  for (j = strlen(attrVal) - 1; j > 0 ; j--) {
    if (attrVal[j] != ' ' && attrVal[j] != '\'')
      break;
  }
  attrVal[j+1] =  '\0';
  memmove(attrVal, &(attrVal[i]),strlen(attrVal) + 1);
  return(0);
}
int  getUMDId( char *inStr , char *attrVal) 
{
  int j,i;
  char *tmpStrPtr;
  char mindex[SMALL_TOKEN];



  if(strstr(inStr,"UDSMD_COLL") == inStr) {
    tmpStrPtr = inStr;
    tmpStrPtr += 10;
    j = 0;
    while (isdigit(*tmpStrPtr)) {
      mindex[j] = *tmpStrPtr;
      j++;
      tmpStrPtr++;
    }
    mindex[j] = '\0';
    j  = atoi(mindex);
    if  (j < 0 || j >= MAX_COLL_STRING_METADATA) {
      return(-1);
    }
    strcpy(attrVal, tmpStrPtr);
    j = j + 2000;
  }
  else if(strstr(inStr,"UDIMD_COLL") == inStr) {
    tmpStrPtr = inStr;
    tmpStrPtr +=10;
    j = 0;
    while (isdigit(*tmpStrPtr)) {
      mindex[j] = *tmpStrPtr;
      j++;
      tmpStrPtr++;
    }
    mindex[j] = '\0';
    j  = atoi(mindex);
    if  (j < 0 || j >= MAX_COLL_INTEGER_METADATA) {
      return (-1);
    }
    strcpy(attrVal, tmpStrPtr);
    j =  j + 3000;
  }
  else if(strstr(inStr,"UDSMD") == inStr) {
    tmpStrPtr = inStr;
    tmpStrPtr += 5;
    j = 0;
    while (isdigit(*tmpStrPtr)) {
      mindex[j] = *tmpStrPtr;
      j++;
      tmpStrPtr++;
    }
    mindex[j] = '\0';
    j  = atoi(mindex);
    if  (j < 0 || j >= MAX_ADREPL_STRING_METADATA) {
      return(-1);
    }
    strcpy(attrVal, tmpStrPtr);
  }
  else if(strstr(inStr,"UDIMD") == inStr) {
    tmpStrPtr = inStr;
    tmpStrPtr +=5;
    j = 0;
    while (isdigit(*tmpStrPtr)) {
      mindex[j] = *tmpStrPtr;
      j++;
      tmpStrPtr++;
    }
    mindex[j] = '\0';
    j  = atoi(mindex);
    if  (j < 0 || j >= MAX_ADREPL_INTEGER_METADATA) {
      return (-1);
    }
    strcpy(attrVal, tmpStrPtr);
    j =  j + 1000;
  }
  else {
    return(-1);
  }
  return(j);
}

int
main(int argc, char **argv)
{
    int   argval;
    int c;
    int L_val = DEFAULT_ROW_COUNT;
    int mval = 1;
    char oper[SMALL_TOKEN], userName[MAX_TOKEN], domainName[MAX_TOKEN];
    char timestamp[MAX_TOKEN],dataType[MAX_TOKEN],position[MAX_TOKEN];
    int private = 0;
    int operFlag;
    char *tmpPtr;
    int first = 0;
    int nextNum = 0;
    char tmpColl[MAX_TOKEN];
    char *metaAttr[MAX_TOKEN];
    char attrVal[MAX_TOKEN];
    char newVal[MAX_TOKEN];
    int metaIndex;
    int i, j, k;
    char temp[MAX_TOKEN], temp1[MAX_TOKEN];


    strcpy(timestamp,"");
    strcpy(dataType,"");
    strcpy(userName,"");
    strcpy(domainName,"");
    strcpy(position,"");
    strcpy(oper,"read");




    myself = -1; 
    if (argc == 1) {
      usage();
      exit(1);
    }

    strcpy(inCondition , "");
    I_case = 0;
    D_case = DEFAULT_FIELD_WIDTH;
    c_value = 0;

    while ((c=getopt(argc, argv,"hciudRPt:p:U:Y:L:T:I:s:")) != EOF) {
      switch (c) {
      case 'h':
	usage ();
	exit (0);
      case 'L':
	L_val = atoi(optarg);
	break;
      case 'Y':
	D_case = atoi(optarg);
	break;
      case 'i':
	strcpy(oper,"insert");
	break;
      case 'c':
	c_value = 1;
	break;
      case 'u':
	strcpy(oper,"update");
	break;
      case 'd':
	strcpy(oper,"delete");
	break;
      case 'R':
	strcpy(oper,"recursive");
	break;
      case 'I':
	metaAttr[nextNum] = optarg;
	nextNum++;
	break;
      case 'T':
	strcpy(dataType,optarg);
	break;
      default:
	usage ();
	exit (1);
      }
    }
    
   i = initSrbClientEnv();
    if (i < 0)
      {printf("Smeta Initialization Error:%i\n",i); exit(1);}
 
 
    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
      fprintf(stderr,"Connection to srbMaster failed.\n");
      fprintf(stderr,"%s",clErrorMessage(conn));
      srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
      clFinish(conn); exit(3);
    }
	  
    if (!strcmp(oper,"read") || !strcmp(oper,"recursive")) {
      if (optind != argc) {
	if (!strcmp(oper,"read")) {
	  if (c_value) {
	    sprintf(temp,"%s/alpha",argv[argc-1]);
	    splitpath(temp,collName, temp1);
	    strcpy(dataName,"");
	  }
	  else {
	    splitpath(argv[argc-1],collName,dataName, '/');
	  }
	  if (make_like_for_mdas(collName))
#ifdef MCAT_VERSION_10
	    sprintf(qval[DATA_GRP_NAME]," like '%s %%'",collName);
#else
	    sprintf(qval[DATA_GRP_NAME]," like '%s'",collName);
#endif
	  else
	    sprintf(qval[DATA_GRP_NAME]," = '%s'",collName);
	  if (make_like_for_mdas(dataName))
#ifdef MCAT_VERSION_10
	    sprintf(qval[DATA_NAME]," like '%s %%'",dataName);
#else
	    sprintf(qval[DATA_NAME]," like '%s'",dataName);
#endif
	  else
	    if (strlen(dataName) > 0)
	      sprintf(qval[DATA_NAME]," = '%s'",dataName);
	}
	else {
	  sprintf(tmpColl,"%s/%s",argv[argc-1],"dummyData");
	  splitpath(tmpColl,collName,dataName, '/');
	  make_like_for_mdas(collName);      
	  sprintf(qval[DATA_GRP_NAME]," like '%s'",collName);
	}
      }
      for (i  = 0 ; i < nextNum;   i++ ) {
	j = getUMDId(metaAttr[i], attrVal);

	if (j < 0) {
	  fprintf(stderr,"Smeta: Metadata Attribute Name Error:%s\n",
		  metaAttr[i]);
	  clFinish(conn); exit(4);
	}
	else if (j < 1000) {
	  strcpy(qval[UDSMD0 + j], attrVal);
	  make_like_for_mdas(qval[UDSMD0 + j]);
	  selval[UDSMD0 + j] =  1;
	  selval[METADATA_NUM] = 1;
	}
	else if (j < 2000) {
	  j = j - 1000;
	  strcpy(qval[UDIMD0 + j], attrVal);
	  make_like_for_mdas(qval[UDIMD0 + j]);
	  selval[UDIMD0 + j] =  1;
	  selval[METADATA_NUM] = 1;
	}
	else if (j < 3000) {
	  j = j - 2000;
	  strcpy(qval[UDSMD_COLL0 + j], attrVal);
	  make_like_for_mdas(qval[UDSMD_COLL0 + j]);
	  selval[UDSMD_COLL0 + j] =  1;
	  selval[METADATA_NUM_COLL] = 1;
	}
	else { /* j > 3000  */
	  j = j - 3000;
	  strcpy(qval[UDIMD_COLL0 + j], attrVal);
	  selval[UDIMD_COLL0 + j] =  1;
	  selval[METADATA_NUM_COLL] = 1;
	}
      }
      if (nextNum == 0) {
        if (c_value) {
	  for ( j = 0; j < MAX_COLL_STRING_METADATA ; j++) 
	  selval[UDSMD_COLL0 + j] =  1;
	  for ( j = 0; j < MAX_COLL_INTEGER_METADATA ; j++) 
	  selval[UDIMD_COLL0 + j] =  1;
	  selval[METADATA_NUM_COLL] = 1;
        }
	else {
	for ( j = 0; j < MAX_ADREPL_STRING_METADATA ; j++) 
	  selval[UDSMD0 + j] =  1;
	for ( j = 0; j < MAX_ADREPL_INTEGER_METADATA ; j++) 
	  selval[UDIMD0 + j] =  1;
	  selval[METADATA_NUM] = 1;
	}
      }
      if (!c_value) {
	sprintf(qval[USER_NAME]," = '%s'",srbUser);
	sprintf(qval[DOMAIN_DESC]," = '%s'",mdasDomainName);
	sprintf(qval[DATA_ACCESS_PRIVILEGE]," like '%%''read''%%'");
      }
      if (!c_value)
	selval[DATA_NAME] = 1;
      selval[DATA_GRP_NAME] = 1;

      
      if (strlen(dataType) > 0){
	if (make_like_for_mdas(dataType))
	  sprintf(qval[DATA_TYP_NAME], " like '%s'", dataType);
	else 
	  sprintf(qval[DATA_TYP_NAME], " = '%s'", dataType);
      }

      j = srbGetDataDirInfo(conn, MDAS_CATALOG, qval, selval, 
			    &myresult, L_val);
      if (j < 0)  {
	if (argc == optind) {
	  if (j != MCAT_INQUIRE_ERROR)
	    printf("Smeta Error: %i\n", j);
	  else 
	    printf("No Metadata found\n"); 
	}
	else {
	  if (j != MCAT_INQUIRE_ERROR)
	    printf("Smeta Error for %s: %i\n",argv[argc-1], j);
	  else 
	    printf("No Metadata found for %s\n",argv[argc-1]); 
	}
      }
      else {
	first = show_addl_results(conn, qval,selval, 
				  &myresult, D_case,first, L_val);
      }
    }
    else {
      if (c_value) { /* collection  metadata update */
	sprintf(temp,"%s/alpha",argv[argc-1]);
	splitpath(temp,collName, temp1);
	strcpy(dataName,"");
	if (!strcmp(oper,"insert")) {
	  if (optind + 1 != argc) {
	    usage();
	    clFinish(conn); exit(5);
	  }
	  metaIndex = -1;
	  for (k  = 0 ; k < nextNum;   k++ ) {
	    j = getUMDId(metaAttr[k], attrVal);
	    if (j < 2000 || j >= 4000) {
	      fprintf(stderr,"Smeta: Metadata Attribute Name Error:%s\n",
		      metaAttr[k]);
	      clFinish(conn); exit(6);
	    }
	    else if (j < 3000 ) {
	      j = j - 2000;
	      stripEqandQuote(attrVal);
	      if (metaIndex >= 0) {
		sprintf(newVal,"%i@%i",j,metaIndex);
		i = srbModifyCollect(conn, MDAS_CATALOG, 
				     collName,
				     newVal,attrVal,"",
				     C_CHANGE_USER_DEFINED_COLL_STRING_META_DATA);
		if (i < 0) break;
	      }
	      else {
		sprintf(newVal,"%i",j);
		i = srbModifyCollect(conn, MDAS_CATALOG, 
				     collName,
				     newVal,attrVal,"",
				     C_INSERT_USER_DEFINED_COLL_STRING_META_DATA);
		
		if (i < 0) break;
		metaIndex = i;
	      }
	    }
	    else {
	      j = j - 3000;
	      stripEqandQuote(attrVal);
	      if (metaIndex >= 0) {
		sprintf(newVal,"%i@%i",j,metaIndex);
		i = srbModifyCollect(conn, MDAS_CATALOG, 
				     collName,
				     newVal,attrVal,"",
				     C_CHANGE_USER_DEFINED_COLL_INTEGER_META_DATA);
		if (i < 0) break;
	      }
	      else {
		sprintf(newVal,"%i",j);
		i = srbModifyCollect(conn, MDAS_CATALOG, 
				     collName,
				     newVal,attrVal,"",
				     C_INSERT_USER_DEFINED_COLL_INTEGER_META_DATA);
		if (i < 0) break;
		metaIndex = i;
	      }
	    }
	  }
	}
	else if (!strcmp(oper,"update")) {
	  if (optind + 3 != argc) {
	    usage();
	    clFinish(conn); exit(7);
	  }
	  j = getUMDId(argv[optind+1], attrVal);
	  if(j < 2000 || j >= 4000) {
	    fprintf(stderr,"Smeta: Metadata Attribute Name Error:%s\n",
		    metaAttr[optind+1]);
	    clFinish(conn); exit(8);
	  }
	  else if (j < 3000 ) {
	    j = j - 2000;
	    stripEqandQuote(attrVal);
	    sprintf(newVal,"%i@%i",j,atoi(argv[optind]));
	    i = srbModifyCollect(conn, MDAS_CATALOG, 
				 collName,
				 newVal,attrVal, "",
				 C_CHANGE_USER_DEFINED_COLL_STRING_META_DATA);
	  }
	  else {
	    stripEqandQuote(attrVal);
	    j = j - 3000;
	    sprintf(newVal,"%i@%i",j,atoi(argv[optind]));
	    i = srbModifyCollect(conn, MDAS_CATALOG, 
				 collName,
				 newVal,attrVal,"",
				 C_CHANGE_USER_DEFINED_COLL_INTEGER_META_DATA);
	  }
	}
	else if (!strcmp(oper,"delete")) {
	  if (optind + 2 != argc) {
	    usage();
	    clFinish(conn); exit(9);	
	  }
	  i = srbModifyCollect(conn, MDAS_CATALOG, 
			       collName,
			       argv[optind],"", "",
			       C_DELETE_USER_DEFINED_COLL_INTEGER_META_DATA);
	}
	if (i < 0) {
	  fprintf(stderr,"Smeta: Error:\"%s/%s:%i\"\n", 
		  collName,dataName, i);
	  srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	}
      }
      else {  /* data metadata update */
	splitpath(argv[argc-1],collName,dataName, '/');
	if (!strcmp(oper,"insert")) {
	  if (optind + 1 != argc) {
	    usage();
	    clFinish(conn); exit(10);
	  }
	  metaIndex = -1;
	  for (k  = 0 ; k < nextNum;   k++ ) {
	    j = getUMDId(metaAttr[k], attrVal);
	    if (j < 0) {
	      fprintf(stderr,"Smeta: Metadata Attribute Name Error:%s\n",
		      metaAttr[k]);
	      clFinish(conn); exit(11);	     
	    }
	    else if (j < 1000) {
	      stripEqandQuote(attrVal);
	      if (metaIndex >= 0) {
		sprintf(newVal,"%i@%i",j,metaIndex);
		i = srbModifyDataset(conn, MDAS_CATALOG, 
				     dataName, collName,
				     "","",
				     newVal,attrVal,
				     D_CHANGE_USER_DEFINED_STRING_META_DATA);
		if (i < 0) break;
	      }
	      else {
		sprintf(newVal,"%i",j);
		i = srbModifyDataset(conn, MDAS_CATALOG, 
				     dataName, collName,
				     "","",
				     newVal,attrVal,
				     D_INSERT_USER_DEFINED_STRING_META_DATA);
		
		if (i < 0) break;
		metaIndex = i;
	      }
	    }
	    else {
	      stripEqandQuote(attrVal);
	      j = j - 1000;
	      if (metaIndex >= 0) {
		sprintf(newVal,"%i@%i",j,metaIndex);
		i = srbModifyDataset(conn, MDAS_CATALOG, 
				     dataName, collName,
				     "","",
				     newVal,attrVal,
				     D_CHANGE_USER_DEFINED_INTEGER_META_DATA);
		if (i < 0) break;
	      }
	      else {
		sprintf(newVal,"%i",j);
		i = srbModifyDataset(conn, MDAS_CATALOG, 
				     dataName, collName,
				     "","",
				     newVal,attrVal,
				     D_INSERT_USER_DEFINED_INTEGER_META_DATA);
		if (i < 0) break;
		metaIndex = i;
	      }
	    }
	  }
	}
	else if (!strcmp(oper,"update")) {
	  if (optind + 3 != argc) {
	    usage();
	    clFinish(conn); exit(12);	  
	  }
	  j = getUMDId(argv[optind+1], attrVal);
	  if(j < 0) {
	    fprintf(stderr,"Smeta: Metadata Attribute Name Error:%s\n",
		    metaAttr[optind+1]);
	    clFinish(conn); exit(13);
	  }
	  else if (j < 1000 ) {
	    stripEqandQuote(attrVal);
	    sprintf(newVal,"%i@%i",j,atoi(argv[optind]));
	    i = srbModifyDataset(conn, MDAS_CATALOG, 
				 dataName, collName,
				 "","",
				 newVal,attrVal, 
				 D_CHANGE_USER_DEFINED_STRING_META_DATA);
	  }
	  else {
	    stripEqandQuote(attrVal);
	    j = j - 1000;
	    sprintf(newVal,"%i@%i",j,atoi(argv[optind]));
	    i = srbModifyDataset(conn, MDAS_CATALOG, 
				 dataName, collName,
				 "","",
				 newVal,attrVal,
				 D_CHANGE_USER_DEFINED_INTEGER_META_DATA);
	  }
	}
	else if (!strcmp(oper,"delete")) {
	  if (optind + 2 != argc) {
	    usage();
	    clFinish(conn); exit(14);
	  }
	  i = srbModifyDataset(conn, MDAS_CATALOG, 
			       dataName, collName,
			       "","",
			       argv[optind],"", 
			       D_DELETE_USER_DEFINED_INTEGER_META_DATA);
	}
	if (i < 0) {
	  fprintf(stderr,"Smeta: Error:\"%s/%s:%i\"\n", 
		  collName,dataName, i);
	  srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	}
      }
    }
    clFinish(conn);
    fflush(stdout);
    exit(0);
}


