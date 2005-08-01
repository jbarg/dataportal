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
extern char mdasCollectionName[];
extern char inCondition[];
extern char mdasDomainName[];



int readExtendedTableAttributeIndex(srbConn *conn, int catType, char *schemaStringList, char *zoneName, char *udsmdArray[])
{
  char qval[MAX_DCS_NUM][MAX_TOKEN];
  int i , j, k, selval[MAX_DCS_NUM];
  mdasC_sql_result_struct *myresult;
  char *aval, *tval, *cval, *sval;
  char tmpStr[MAX_TOKEN];
  myresult  = malloc (sizeof (mdasC_sql_result_struct));
  for (i = 0; i < MAX_DCS_NUM; i++)
    {
           sprintf(qval[i],"");
           selval[i] = 0;
    }
  selval[EXTENSIBLE_SCHEMA_NAME] = 1;
  selval[EXTENSIBLE_TABLE_NAME] = 1;
  selval[EXTENSIBLE_ATTR_NAME] = 1;
  selval[EXTENSIBLE_ATTR_OUTSIDE_NAME] = 1;
  if (schemaStringList != NULL && strlen(schemaStringList) > 0) {
    sprintf(qval[EXTENSIBLE_SCHEMA_NAME]," in (%s) ",schemaStringList);
  }
  failure = 0;
  if (zoneName != NULL && strlen(zoneName) > 0)
    i = srbGetDataDirInfoWithZone(conn, catType,zoneName, qval, selval, myresult, MAX_EXT_TABLE_INDEX - MIN_EXT_TABLE_INDEX);
  else
    i = srbGetDataDirInfo(conn, catType, qval, selval, myresult, MAX_EXT_TABLE_INDEX - MIN_EXT_TABLE_INDEX);
  if (i < 0 || myresult == NULL )
    return(i);
  sval = (char *) getFromResultStruct(
                  (mdasC_sql_result_struct *) myresult,
                  dcs_tname[EXTENSIBLE_SCHEMA_NAME],dcs_aname[EXTENSIBLE_SCHEMA_NAME]);
  tval = (char *) getFromResultStruct(
                  (mdasC_sql_result_struct *) myresult,
                  dcs_tname[EXTENSIBLE_TABLE_NAME],dcs_aname[EXTENSIBLE_TABLE_NAME]);
  aval = (char *) getFromResultStruct(
                  (mdasC_sql_result_struct *) myresult,
                  dcs_tname[EXTENSIBLE_ATTR_NAME],dcs_aname[EXTENSIBLE_ATTR_NAME]);
  cval = (char *) getFromResultStruct(
                  (mdasC_sql_result_struct *) myresult,
                  dcs_tname[EXTENSIBLE_ATTR_OUTSIDE_NAME],dcs_aname[EXTENSIBLE_ATTR_OUTSIDE_NAME]);
  j = MIN_EXT_TABLE_INDEX;
  k = 0;
  udsmdArray[k] = NULL;
  for (i = 0; i <myresult->row_count ; i++) {
    if (!strcmp(tval,"UDSMD")) {
      if (strlen(sval) > 0)
	sprintf(tmpStr,"%s:%s.%s",aval,sval,cval);
      else
	sprintf(tmpStr,"%s:%s",aval,cval);
      udsmdArray[k++] = strdup(tmpStr);
      udsmdArray[k] = NULL;
    }
    else {
      if (strlen(sval) > 0)
	sprintf(dcs_cname[j],"%s.%s",sval,cval);
      else
	sprintf(dcs_cname[j],"%s",cval);
      strcpy(dcs_tname[j],tval);
      strcpy(dcs_aname[j],aval);
      j++;
    }
    sval += MAX_DATA_SIZE;
    tval += MAX_DATA_SIZE;
    aval += MAX_DATA_SIZE;
    cval += MAX_DATA_SIZE;
  }
  sval = (char *) getFromResultStruct(
                  (mdasC_sql_result_struct *) myresult,
                  dcs_tname[EXTENSIBLE_SCHEMA_NAME],dcs_aname[EXTENSIBLE_SCHEMA_NAME]);
  tval = (char *) getFromResultStruct(
                  (mdasC_sql_result_struct *) myresult,
                  dcs_tname[EXTENSIBLE_TABLE_NAME],dcs_aname[EXTENSIBLE_TABLE_NAME]);
  aval = (char *) getFromResultStruct(
                  (mdasC_sql_result_struct *) myresult,
                  dcs_tname[EXTENSIBLE_ATTR_NAME],dcs_aname[EXTENSIBLE_ATTR_NAME]);
  cval = (char *) getFromResultStruct(
                  (mdasC_sql_result_struct *) myresult,
                  dcs_tname[EXTENSIBLE_ATTR_OUTSIDE_NAME],dcs_aname[EXTENSIBLE_ATTR_OUTSIDE_NAME]);
  free (tval);
  free (aval);
  free (cval);
  free (sval);
  free(myresult);
  return(0);

}

void
usage()
{
  fprintf(stderr,"Usage: Squery [-z zone] [-N NameSpace ...] -S McatAttrName  [-S McatAttrName ] ... McatAttrName Op McatAttrValue [ McatAttrName Op McatAttrValue ] ... \n");
  fprintf(stderr,"Usage: Squery [-z zone] -H|-E|-U [-L]  [-N NameSpace ...] [-B AttrBeginsWithString]  [-C AttrContainsString]\n");
}
int
main(int argc, char **argv)
{
    srbConn *conn;
    int i,c,j, sel;
    int L_val = DEFAULT_ROW_COUNT;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    mdasC_sql_result_struct  myresult;
    int D_case = DEFAULT_FIELD_WIDTH;
    int H_Value = 0;
    int L_Value = 0;
    int E_Value = 0;
    int U_Value = 0;
    char zoneName[MAX_TOKEN]; 
    char tmpStr[MAX_TOKEN];
    char bStr[MAX_TOKEN];
    char mStr[MAX_TOKEN];
    char *selectAttrList[MAX_TOKEN];
    char schemaListString[MAX_TOKEN *3];
    int dj,cj,rj,uj;
    char *udsmdArray[MAX_UDSMD_NUMBER];

    if (argc == 1) {
      usage ();
      exit(1);
    }
    else if (!strcmp(argv[1],"-h")) {
      usage ();
      exit(1);
    }
    strcpy(zoneName,"");
    strcpy(bStr,"");
    strcpy(mStr,"");
    strcpy(schemaListString,"");
    j = 0;
    sel = 0;
    while ((c= getopt(argc, argv,"z:B:C:HEULS:N:h")) != EOF) {
      switch (c) {
      case 'h':
	usage ();
	clFinish(conn);
	exit(1);
	break; 
      case 'U':
	U_Value = 1;
	break;
      case 'E':
	E_Value = 1;
	break;
      case 'H':
	H_Value = 1;
	break;
      case 'L':
        L_Value = 1;
        break;
       case  'B':
	strcpy(bStr,optarg);
        break;
      case  'C':
        strcpy(mStr,optarg);
        break;
      case  'z':
	strcpy(zoneName,optarg);
	break;
      case  'N':
	if (strlen(schemaListString) == 0)
	  sprintf(schemaListString, "( '%s'",optarg);
	else 
	  sprintf(schemaListString,"%s , '%s'",schemaListString, optarg);
	break;
      case  'S':
	selectAttrList[sel++] = strdup(optarg);
	break;
      default:
	usage ();
	clFinish(conn);
	exit(1);
      }
    }
    if (strlen(schemaListString) > 0)
      strcat(schemaListString, " )");

    i = initSrbClientEnv();
    if (i < 0)
      {printf("Squery Initialization Error:%i\n",i); exit(1);}

    conn = srbConnect (srbHost, NULL, srbAuth, NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
      fprintf(stderr,"Connection to srbMaster failed.\n");
      fprintf(stderr,"%s",clErrorMessage(conn));
      srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
      clFinish(conn); exit(3);
    }

    readExtendedTableAttributeIndex(conn, MDAS_CATALOG,schemaListString, zoneName, udsmdArray);

    if (H_Value == 1 || E_Value == 1 || U_Value == 1) {
      i = 0;
      j = 0;
      if (E_Value == 1) {
	i = MIN_EXT_TABLE_INDEX;
	j = MAX_EXT_TABLE_INDEX;
      }
      if (H_Value == 1) {
	i = 0;
	j = MAX_DCS_NUM;
      }
      
      for ( ; i < j; i++) {
	if (strstr(dcs_cname[i],"USEREXTRA") != NULL)
	  continue;
	if (U_Value== 1) {
	  if (strcmp(dcs_tname[i],"UDSMD"))
	    continue;
	}
	if (E_Value == 0 && i >= MIN_EXT_TABLE_INDEX && i <= MAX_EXT_TABLE_INDEX)
	  continue;
	k = 1;
	if (strlen(bStr) > 0) {
	  if (strstr(dcs_cname[i],bStr) != dcs_cname[i])
	    k = 0;
	}
	if (strlen(mStr) > 0) {
	  if (strstr(dcs_cname[i],mStr) == NULL)
	    k = 0;
	}
	if (k == 1) {
	  if (L_Value == 0)
	    printf("%s\n",dcs_cname[i]);
	  else 
	    printf("%i:%s -> %s.%s\n",i,dcs_cname[i],dcs_tname[i],dcs_aname[i]);
	}
      }
      if (U_Value == 1) {
	i = 0;
	while (udsmdArray[i] != NULL) {
	  if (L_Value == 0)
	    printf("%s\n",(char *) &udsmdArray[i][5]);
	  else {
	    udsmdArray[i][4] = '\0';
	    printf("%i:%s->UDSMD.%s\n",i,(char *) &udsmdArray[i][5], udsmdArray[i]);
	    udsmdArray[i][4] = ':';
	  }
	  i++;
	}
      }
      
      clFinish(conn);
      exit(0);
    }


    if (sel <= 0) {
      printf("Some Select AttrName is  expected\n");
      usage ();
      clFinish(conn);
      exit(1);
    }


    for (i = 0; i < MAX_DCS_NUM; i++)    {
      sprintf(qval[i],"");
      selval[i] = 0;
    }


    dj = 0;
    cj = 0;
    rj = 0;
    uj = 0;
    for (i = optind; i < argc; i = i+3) {
      if (i +2 >= argc) {
	printf("Error in number of arguments expected for each condition\n");
	usage();
	clFinish(conn); exit(3);
      }
      j = getIndexForConstName(argv[i]);
      if (j < 0) {
	j = 0;
	while (udsmdArray[j] != NULL) {

	  if (strcmp(argv[i],&udsmdArray[j][5]) ) {
	    j++;
	    continue;
	  }
	  strcpy(dcs_tname[DC_CONTRIBUTOR_WEB],"UDSMD");
	  udsmdArray[j][4] = '\0';
	  strcpy(dcs_aname[DC_CONTRIBUTOR_WEB],udsmdArray[j]);
	  strcpy(dcs_cname[DC_CONTRIBUTOR_WEB],&udsmdArray[j][5]);
	  udsmdArray[j][4] = ':';
	  j = DC_CONTRIBUTOR_WEB;
	  break;
	}
	if (j != DC_CONTRIBUTOR_WEB)
	  j = -1;
      }
      if (j < 0 ) {
	printf("Error in Condition Attribute Name. Unknown Attribute:%s.\n",argv[i]);
	clFinish(conn); exit(3);
      }

      if (!strcmp(dcs_tname[j],"UDSMD")){
	
	if (!strcmp(dcs_aname[j],"DATA")) {
	  if (dj >= 5) {
	    fprintf(stderr,"Too many User-defined Attributes for Data. Only 5 supported per query. Ignoring this attribute for condition: %s %s %s.\n",argv[i], argv[i+1],argv[i+2]); 
	    continue;
	  }
	  strcpy(tmpStr, argv[i+2]);
	  make_like_for_mdas(tmpStr);
	  if (dj == 0) {
	    sprintf(qval[UDSMD0]," = '%s'",argv[i]);
	    sprintf(qval[UDSMD1]," %s '%s'",argv[i+1],tmpStr);
	  }
	  else {
	    sprintf(qval[UDSMD0_1 + dj-1]," = '%s'",argv[i]);
	    sprintf(qval[UDSMD1_1 + dj-1]," %s '%s'",argv[i+1],tmpStr);
	  }
	  dj++;
	}
	else if ( !strcmp(dcs_aname[j],"COLL")) {
	  if (cj >= 5) {
	    fprintf(stderr,"Too many User-defined Attributes for Collection. Only 5 supported per query. Ignoring this attribute for condition: %s %s %s.\n",argv[i], argv[i+1],argv[i+2]);
	    continue;
	  }
	  strcpy(tmpStr, argv[i+2]);
	  make_like_for_mdas(tmpStr);
	  if (cj == 0) {
	    sprintf(qval[UDSMD_COLL0]," = '%s'",argv[i]);
	    sprintf(qval[UDSMD_COLL1]," %s '%s'",argv[i+1],tmpStr);
	  }
	  else {
	    sprintf(qval[UDSMD_COLL0_1 + cj-1]," = '%s'",argv[i]);
	    sprintf(qval[UDSMD_COLL1_1 + cj-1]," %s '%s'",argv[i+1],tmpStr);
	  }
	  cj++;
	}
	else if ( !strcmp(dcs_aname[j],"RSRC")) {
	  if (rj >= 4) {
	    fprintf(stderr,"Too many User-defined Attributes for Resource. Only 4 supported per query. Ignoring this attribute for condition: %s %s %s.\n",argv[i], argv[i+1],argv[i+2]);
	    continue;
	  }
	  strcpy(tmpStr, argv[i+2]);
	  make_like_for_mdas(tmpStr);
	  if (rj == 0) {
	    sprintf(qval[UDSMD_RSRC0]," = '%s'",argv[i]);
	    sprintf(qval[UDSMD_RSRC1]," %s '%s'",argv[i+1],tmpStr);
	  }
	  else {
	    sprintf(qval[UDSMD_RSRC0_1 + rj-1]," = '%s'",argv[i]);
	    sprintf(qval[UDSMD_RSRC1_1 + rj-1]," %s '%s'",argv[i+1],tmpStr);
	  }
	  rj++;	
	}
	else if ( !strcmp(dcs_aname[j],"USER")) {
	  if (uj >= 4) {
	    fprintf(stderr,"Too many User-defined Attributes for Users. Only 4 supported per query. Ignoring this attribute for condition: %s %s %s.\n",argv[i], argv[i+1],argv[i+2]);
	    continue;
	  }
	  strcpy(tmpStr, argv[i+2]);
	  make_like_for_mdas(tmpStr);
	  if (uj == 0) {
	    sprintf(qval[UDSMD_USER0]," = '%s'",argv[i]);
	    sprintf(qval[UDSMD_USER1]," %s '%s'",argv[i+1],tmpStr);
	  }
	  else {
	    sprintf(qval[UDSMD_USER0_1 + uj-1]," = '%s'",argv[i]);
	    sprintf(qval[UDSMD_USER1_1 + uj-1]," %s '%s'",argv[i+1],tmpStr);
	  }
	  uj++;
	  
	}
      }
      else {
	if (isAlphaString(argv[i+2])) {
	  strcpy(tmpStr, argv[i+2]);
	  make_like_for_mdas(tmpStr);
	  sprintf(qval[j]," %s '%s'",argv[i+1],tmpStr);
	}
	else
	sprintf(qval[j]," %s %s",argv[i+1],argv[i+2]);
      }
    }

    for (i = 0; i < sel ; i++) {
      k = getIndexForConstName(selectAttrList[i]); 
      if (k < 0) {
	k = 0;
        while (udsmdArray[k] != NULL) {
          if (strcmp(selectAttrList[i],&udsmdArray[k][5]) ) {
	    k++;
            continue;
	  }
          strcpy(dcs_tname[DC_CONTRIBUTOR_WEB],"UDSMD");
          udsmdArray[k][4] = '\0';
          strcpy(dcs_aname[DC_CONTRIBUTOR_WEB],udsmdArray[k]);
          strcpy(dcs_cname[DC_CONTRIBUTOR_WEB],&udsmdArray[k][5]);
          udsmdArray[k][4] = ':';
	  k = DC_CONTRIBUTOR_WEB;
          break;
        }
        if (k != DC_CONTRIBUTOR_WEB)
          k = -1;
      }

      if (k < 0) {
	printf("Error in Selected Attribute Name. Unknown Attribute:%s.\n",selectAttrList[i]);
	usage ();
	clFinish(conn);
	exit(1);
      }
      if (!strcmp(dcs_tname[k],"UDSMD") ) {
	if (!strcmp(dcs_aname[k],"DATA")) {
	  sprintf(tmpStr," = '%s'",selectAttrList[i]);
	  if (!strcmp(qval[UDSMD0],tmpStr))
	    selval[UDSMD1] = 1;
	  else if (!strcmp(qval[UDSMD0_1],tmpStr))
            selval[UDSMD1_1] = 1;
          else if (!strcmp(qval[UDSMD0_2],tmpStr))
            selval[UDSMD1_2] = 1;
	  else if (!strcmp(qval[UDSMD0_3],tmpStr))
            selval[UDSMD1_3] = 1;
          else if (!strcmp(qval[UDSMD0_4],tmpStr))
            selval[UDSMD1_4] = 1;
	  else {
	    if (dj >= 5) {
            fprintf(stderr,"Too many User-defined Attributes for Data. Only 5 supported per query. Ignoring this attribute for selection:%s.\n",
		    selectAttrList[i]);
            continue;
	    }
	    else if (dj == 0) {
	      strcat(qval[UDSMD0],tmpStr);
	      selval[UDSMD1] = 1;
	    }
	    else {
	      strcat(qval[UDSMD0_1 + dj-1],tmpStr);
	      selval[UDSMD1_1 + dj-1] = 1;
	    }
	    dj++;
	  }
	}
	if (!strcmp(dcs_aname[k],"COLL")) {
          sprintf(tmpStr," = '%s'",selectAttrList[i]);
          if (!strcmp(qval[UDSMD_COLL0],tmpStr))
            selval[UDSMD_COLL1] = 1;
          else if (!strcmp(qval[UDSMD_COLL0_1],tmpStr))
            selval[UDSMD_COLL1_1] = 1;
          else if (!strcmp(qval[UDSMD_COLL0_2],tmpStr))
            selval[UDSMD_COLL1_2] = 1;
          else if (!strcmp(qval[UDSMD_COLL0_3],tmpStr))
            selval[UDSMD_COLL1_3] = 1;
          else if (!strcmp(qval[UDSMD_COLL0_4],tmpStr))
            selval[UDSMD_COLL1_4] = 1;
          else {
            if (cj >= 5) {
            fprintf(stderr,"Too many User-defined Attributes for Data. Only 5 supported per query. Ignoring this attribute for selection:%s.\n",
                    selectAttrList[i]);
            continue;
            }
            else if (cj == 0) {
              strcat(qval[UDSMD_COLL0],tmpStr);
              selval[UDSMD_COLL1] = 1;
            }
            else {
              strcat(qval[UDSMD_COLL0_1 + cj-1],tmpStr);
              selval[UDSMD_COLL1_1 + cj-1] = 1;
            }
            cj++;
          }
        }
	if (!strcmp(dcs_aname[k],"RSRC")) {
          sprintf(tmpStr," = '%s'",selectAttrList[i]);
          if (!strcmp(qval[UDSMD_RSRC0],tmpStr))
            selval[UDSMD_RSRC1] = 1;
          else if (!strcmp(qval[UDSMD_RSRC0_1],tmpStr))
            selval[UDSMD_RSRC1_1] = 1;
          else if (!strcmp(qval[UDSMD_RSRC0_2],tmpStr))
            selval[UDSMD_RSRC1_2] = 1;
          else if (!strcmp(qval[UDSMD_RSRC0_3],tmpStr))
            selval[UDSMD_RSRC1_3] = 1;
          else {
            if (rj >= 4) {
            fprintf(stderr,"Too many User-defined Attributes for Data. Only 5 supported per query. Ignoring this attribute for selection:%s.\n",
                    selectAttrList[i]);
            continue;
            }
            else if (rj == 0) {
              strcat(qval[UDSMD_RSRC0],tmpStr);
              selval[UDSMD_RSRC1] = 1;
            }
            else {
              strcat(qval[UDSMD_RSRC0_1 + rj-1],tmpStr);
              selval[UDSMD_RSRC1_1 + rj-1] = 1;
            }
            rj++;
          }
        }
	if  (!strcmp(dcs_aname[k],"USER")) {
          sprintf(tmpStr," = '%s'",selectAttrList[i]);
          if (!strcmp(qval[UDSMD_USER0],tmpStr))
            selval[UDSMD_USER1] = 1;
          else if (!strcmp(qval[UDSMD_USER0_1],tmpStr))
            selval[UDSMD_USER1_1] = 1;
          else if (!strcmp(qval[UDSMD_USER0_2],tmpStr))
            selval[UDSMD_USER1_2] = 1;
          else if (!strcmp(qval[UDSMD_USER0_3],tmpStr))
            selval[UDSMD_USER1_3] = 1;
          else {
            if (uj >= 4) {
            fprintf(stderr,"Too many User-defined Attributes for Data. Only 5 supported per query. Ignoring this attribute for selection:%s.\n",
                    selectAttrList[i]);
            continue;
            }
            else if (uj == 0) {
              strcat(qval[UDSMD_USER0],tmpStr);
              selval[UDSMD_USER1] = 1;
            }
            else {
              strcat(qval[UDSMD_USER0_1 + uj-1],tmpStr);
              selval[UDSMD_USER1_1 + uj-1] = 1;
            }
            uj++;
          }
        }
      }
      else {
	selval[k] = 1;
      }
    }    

    
    i = queryAndPrintAcrossZones(conn, MDAS_CATALOG, zoneName, qval, selval,&myresult,L_val);
    if (i < 0) {
      if (i != CLI_NO_ANSWER || i != MCAT_INQUIRE_ERROR) {
          fprintf(stderr,"Squery Error: %i\n", i);
      } else {
          fprintf(stderr,"No Answer\n");
          srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
      }
      clFinish(conn); exit(4);
    }
    clFinish(conn);
    exit(0);
}

