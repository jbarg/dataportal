/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/



#include "scommands.h"



extern char srbAuth[];
extern char srbHost[];
extern char srbUser[];
extern char mdasDomainName[];
extern char mdasCollectionName[];
extern char inCondition[];

    srbConn *conn;
    int myself;
    
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    mdasC_sql_result_struct  myresult;
    int alive;
    char *rval[MAX_DCS_NUM];
    char dname[MAX_TOKEN];
    
    int D_case = 0;
    int I_case = 0;
   char collName[MAX_TOKEN];
    char dataName[MAX_TOKEN];


void
usage()
{
  fprintf(stderr,"Sannotate -w position annotation dataName\n");
  fprintf(stderr,"Sannotate -u timestamp newAnnotation dataName\n");
  fprintf(stderr,"Sannotate -d timestamp dataName\n");
  fprintf(stderr,"Sannotate -c -w type annotation collectionName\n");
  fprintf(stderr,"Sannotate -c -u timestamp newAnnotation collectionName\n");
  fprintf(stderr,"Sannotate -c -d timestamp collectionName\n");
  fprintf(stderr,"Sannotate  [-R] [-t timestamp] [-a annotation] [-p position] [-U userName@domainName] [-T dataType]  [-Y n] [-L n] dataName|collectionName\n");
  fprintf(stderr,"Sannotate -c [-R] [-t timestamp] [-a annotation] [-p type] [-U userName@domainName] [-Y n] [-L n]  collectionName\n");
}
int
main(int argc, char **argv)
{
    int c;
    int L_val = DEFAULT_ROW_COUNT;
    int forCollection = 0;
    char oper[SMALL_TOKEN], userName[MAX_TOKEN], domainName[MAX_TOKEN];
    char timestamp[MAX_TOKEN],dataType[MAX_TOKEN],position[MAX_TOKEN];
    char annotation[MAX_TOKEN];
    char *tmpPtr;
    int first = 0;
    char tmpColl[MAX_TOKEN];
    strcpy(timestamp,"");
    strcpy(dataType,"");
    strcpy(userName,"");
    strcpy(domainName,"");
    strcpy(position,"");
    strcpy(annotation,"");
    strcpy(oper,"read");

    myself = -1; 
    if (argc == 1) {
      usage();
      exit(1);
    }

    strcpy(inCondition , "");
    I_case = 0;
    D_case = DEFAULT_FIELD_WIDTH;


    while ((c=getopt(argc, argv,"hcwudRPt:p:U:Y:L:T:a:")) != EOF) {
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
      case 'w':
	strcpy(oper,"write");
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
      case 'P':
	break;
      case 'c':
	forCollection = 1;
	break;
      case 't':
	strcpy(timestamp,optarg);
	break;
      case 'p':
	strcpy(position,optarg);
	break;
      case 'a':
	strcpy(annotation,optarg);
	break;
      case 'T':
	strcpy(dataType,optarg);
	break;
      case 'U':
	strcpy(userName,optarg);
	if ((tmpPtr = strstr(userName,"@")) == NULL){
	  printf("Domain of User Missing\n");
	  exit(1);
	}
	*tmpPtr = '\0';
	tmpPtr++;
	strcpy(domainName, tmpPtr);
	break;
      default:
	usage ();
	exit (1);
      }
    }
    
   i = initSrbClientEnv();
    if (i < 0)
      {printf("Sannotate Initialization Error:%i\n",i); exit(1);}
 
 
    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
      fprintf(stderr,"Connection to srbMaster failed.\n");
      fprintf(stderr,"%s",clErrorMessage(conn));
      srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
      clFinish(conn); exit(3);
    }
	  
    if (!strcmp(oper,"read") || !strcmp(oper,"recursive")) {
     if (forCollection == 0) {
      if (!strcmp(oper,"read")) {
	splitpath(argv[argc-1],collName,dataName);
      if (make_like_for_mdas(collName))
	sprintf(qval[DATA_GRP_NAME]," like '%s'",collName);
      else
	sprintf(qval[DATA_GRP_NAME]," = '%s'",collName);
      if (make_like_for_mdas(dataName))
	sprintf(qval[DATA_NAME]," like '%s'",dataName);
      else
	if (strlen(dataName) > 0)
		  sprintf(qval[DATA_NAME]," = '%s'",dataName);
      }
      else {
	sprintf(tmpColl,"%s/%s",argv[argc-1],"dummyData");
	splitpath(tmpColl,collName,dataName);
	make_like_for_mdas(collName);      
	sprintf(qval[DATA_GRP_NAME]," like '%s%%'",collName);

      }
      sprintf(qval[USER_NAME]," = '%s'",srbUser);
      sprintf(qval[DOMAIN_DESC]," = '%s'",mdasDomainName);
      sprintf(qval[DATA_ACCESS_PRIVILEGE]," like '%%''read''%%'");
      selval[DATA_NAME] = 1;
      selval[DATA_GRP_NAME] = 1;
      selval[DATA_ANNOTATION] = 1;
      selval[DATA_ANNOTATION_TIMESTAMP] = 1;
      selval[DATA_ANNOTATION_USERNAME] = 1;
      selval[DATA_ANNOTATION_USERDOMAIN] = 1;
      selval[DATA_ANNOTATION_POSITION] = 1;
      if (strlen(dataType) > 0){
	if (make_like_for_mdas(dataType))
	  sprintf(qval[DATA_TYP_NAME], " like '%s'", dataType);
	else 
	  sprintf(qval[DATA_TYP_NAME], " = '%s'", dataType);
      }
      if (strlen(position) > 0) {
	if (make_like_for_mdas(position))
	  sprintf(qval[DATA_ANNOTATION_POSITION], " like '%s'", position);
	else 
	  sprintf(qval[DATA_ANNOTATION_POSITION], " = '%s'", position);
      }
      if (strlen(annotation) > 0) {
	if (make_like_for_mdas(annotation))
	  sprintf(qval[DATA_ANNOTATION], " like '%s'", annotation);
	else 
	  sprintf(qval[DATA_ANNOTATION], " = '%s'", annotation);
      }
      if (strlen(timestamp) > 0) {
	tmpPtr = timestamp;
	while (*tmpPtr == ' ') tmpPtr++;
	if (isdigit(*tmpPtr) || *tmpPtr == '%' || *tmpPtr == '_') {
	  if (make_like_for_mdas(timestamp))
	    sprintf(qval[DATA_ANNOTATION_TIMESTAMP]," like '%s'",timestamp );
	  else 
	    sprintf(qval[DATA_ANNOTATION_TIMESTAMP]," = '%s'",timestamp );
	}
	else {
	  sprintf(qval[DATA_ANNOTATION_TIMESTAMP], timestamp);
	}
      }
      if (strlen(userName) > 0) {
	if (make_like_for_mdas(userName))
	  sprintf(qval[DATA_ANNOTATION_USERNAME], " like '%s'", userName);
	else 
	  sprintf(qval[DATA_ANNOTATION_USERNAME], " = '%s'", userName);
      }
      if (strlen(domainName) > 0) {
	if (make_like_for_mdas(domainName))
	  sprintf(qval[DATA_ANNOTATION_USERDOMAIN]," like '%s'",domainName );
	else 
	  sprintf(qval[DATA_ANNOTATION_USERDOMAIN], " = '%s'",domainName );
      }

      j = srbGetDataDirInfo(conn, MDAS_CATALOG, qval, selval, 
			    &myresult, L_val);
      if (j < 0)  {
	if (j != MCAT_INQUIRE_ERROR)
	  printf("Sannotate Error for %s: %i\n",argv[argc-1], j);
	else 
	  printf("No Annotations found for %s\n",argv[argc-1]); 
      }
      else {
	first = show_addl_results(conn, qval,selval, 
				  &myresult, D_case,first, L_val);
      }
     }
     else { /* collection reads */
      if (!strcmp(oper,"read")) {
	sprintf(tmpColl,"%s/%s",argv[argc-1],"dummyData");
	splitpath(tmpColl,collName,dataName);
	if (make_like_for_mdas(collName))
	  /*  sprintf(qval[DATA_GRP_NAME]," like '%s'",collName);*/
	  sprintf(qval[ACCESS_COLLECTION_NAME]," like '%s'",collName);
	else
	 /*   sprintf(qval[DATA_GRP_NAME]," = '%s'",collName);*/
	  sprintf(qval[ACCESS_COLLECTION_NAME]," = '%s'",collName);
      }
      else {
	sprintf(tmpColl,"%s/%s",argv[argc-1],"dummyData");
	splitpath(tmpColl,collName,dataName);
	make_like_for_mdas(collName);      
	sprintf(qval[DATA_GRP_NAME]," like '%s%%'",collName);
      }

      sprintf(qval[USER_NAME]," = '%s'",srbUser);
      sprintf(qval[DOMAIN_DESC]," = '%s'",mdasDomainName);
      sprintf(qval[COLLECTION_ACCESS_CONSTRAINT]," like '%%'");
      selval[ACCESS_COLLECTION_NAME] = 1;

      selval[COLL_ANNOTATION] = 1;
      selval[COLL_ANNOTATION_TIMESTAMP] = 1;
      selval[COLL_ANNOTATION_USERNAME] = 1;
      selval[COLL_ANNOTATION_USERDOMAIN] = 1;
      selval[COLL_ANNOTATION_TYPE] = 1;







      if (strlen(position) > 0) {
	if (make_like_for_mdas(position))
	  sprintf(qval[COLL_ANNOTATION_TYPE], " like '%s'", position);
	else 
	  sprintf(qval[COLL_ANNOTATION_TYPE], " = '%s'", position);
      }
      if (strlen(annotation) > 0) {
	if (make_like_for_mdas(annotation))
	  sprintf(qval[COLL_ANNOTATION], " like '%s'", annotation);
	else 
	  sprintf(qval[COLL_ANNOTATION], " = '%s'", annotation);
      }
      if (strlen(timestamp) > 0) {
	tmpPtr = timestamp;
	while (*tmpPtr == ' ') tmpPtr++;
	if (isdigit(*tmpPtr) || *tmpPtr == '%' || *tmpPtr == '_') {
	  if (make_like_for_mdas(timestamp))
	    sprintf(qval[COLL_ANNOTATION_TIMESTAMP]," like '%s'",timestamp );
	  else 
	    sprintf(qval[COLL_ANNOTATION_TIMESTAMP]," = '%s'",timestamp );
	}
	else {
	  sprintf(qval[COLL_ANNOTATION_TIMESTAMP], timestamp);
	}
      }
      if (strlen(userName) > 0) {
	if (make_like_for_mdas(userName))
	  sprintf(qval[COLL_ANNOTATION_USERNAME], " like '%s'", userName);
	else 
	  sprintf(qval[COLL_ANNOTATION_USERNAME], " = '%s'", userName);
      }
      if (strlen(domainName) > 0) {
	if (make_like_for_mdas(domainName))
	  sprintf(qval[COLL_ANNOTATION_USERDOMAIN]," like '%s'",domainName );
	else 
	  sprintf(qval[COLL_ANNOTATION_USERDOMAIN], " = '%s'",domainName );
      }

      j = srbGetDataDirInfo(conn, MDAS_CATALOG, qval, selval, 
			    &myresult, L_val);
      if (j < 0)  {
	if (j != MCAT_INQUIRE_ERROR)
	  printf("Sannotate Error for %s: %i\n",argv[argc-1], j);
	else 
	  printf("No Annotations found for %s\n",argv[argc-1]); 
      }
      else {
	first = show_addl_results(conn, qval,selval, 
				  &myresult, D_case,first, L_val);
      }
     }
    }
    else {
     if (forCollection == 0) {
      splitpath(argv[argc-1],collName,dataName);
      if (!strcmp(oper,"write")) {
	if (optind + 3 != argc) {
	  usage();
	  clFinish(conn); exit(4);
	}
	i = srbModifyDataset(conn, MDAS_CATALOG, 
			     dataName, collName,
			     "","",
			     argv[optind+1],argv[optind], 
			     D_INSERT_ANNOTATIONS);
      }
      else if (!strcmp(oper,"update")) {
	if (optind + 3 != argc) {
	  usage();
	  clFinish(conn); exit(4);
	}
	i = srbModifyDataset(conn, MDAS_CATALOG, 
			     dataName, collName,
			     "","",
			     argv[optind+1],argv[optind], 
			     D_UPDATE_ANNOTATIONS);
      }
      else if (!strcmp(oper,"delete")) {
	if (optind + 2 != argc) {
	  usage();
	  clFinish(conn); exit(5);
	}
	i = srbModifyDataset(conn, MDAS_CATALOG, 
			     dataName, collName,
			     "","",
			     argv[optind],"", 
			     D_DELETE_ANNOTATIONS);
      }
      if (i < 0) {
	fprintf(stderr,"Sannotate: Error:\"%s/%s:%i\"\n", 
		collName,dataName, i);
	srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
      }
     }
     else {
      if (!strcmp(oper,"write")) {
	if (optind + 3 != argc) {
	  usage();
	  clFinish(conn); exit(4);
	}
	sprintf(tmpColl,"%s/%s",argv[argc-1],"dummyData");
	splitpath(tmpColl,collName,dataName);
	i = srbModifyCollect(conn, MDAS_CATALOG, 
			     collName,
			     argv[optind+1],argv[optind],"",
			     C_INSERT_ANNOTATIONS);
      }
      else if (!strcmp(oper,"update")) {
	if (optind + 3 != argc) {
	  usage();
	  clFinish(conn); exit(4);
	}
	i = srbModifyCollect(conn, MDAS_CATALOG, 
			     collName,
			     argv[optind+1],argv[optind], "",
			     C_UPDATE_ANNOTATIONS);
      }
      else if (!strcmp(oper,"delete")) {
	if (optind + 2 != argc) {
	  usage();
	  clFinish(conn); exit(5);
	}
	i = srbModifyCollect(conn, MDAS_CATALOG, 
			     collName,
			     argv[optind],"","",
			     C_DELETE_ANNOTATIONS);
      }
      if (i < 0) {
	fprintf(stderr,"Sannotate: Error:\"%s:%i\"\n", 
		collName, i);
	srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
      }
     }
    }
    clFinish(conn);
    fflush(stdout);
    exit(0);
}


