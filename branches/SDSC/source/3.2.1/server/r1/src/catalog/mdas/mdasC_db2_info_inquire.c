/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/* mdasC driver function */



/*
#include "COPYRIGHT.h"
#include "mdasC.h"
*/
#include "mdasC_db2_externs.h"
#include "table_names_to_sql_externs.h"
#include "elogExtern.h"
#include "mdasGlobalsExtern.h"
#include "srb.h"
extern userInfo *ClientUser;


extern  char metadatafkrelbuffer[];
extern mdasC_sql_continuation_struct queryCont[];
int  fkrelgraph_initialized = 0;
int  predefined_GDI_initialized = 0;
char predefined_GDI_select_template[]="SELECT DISTINCT $$$.MDAS_ADC_REPL.repl_enum , $$$.MDAS_AD_REPL.data_name , $$$.MDAS_TD_DATA_TYP.data_typ_name , $$$.MDAS_ADC_REPL.path_name , $$$.MDAS_TD_LOCN.netprefix , $$$.MDAS_AD_REPL.data_size , $$$.MDAS_AR_PHYSICAL.phy_rsrc_name , $$$.MDAS_TD_RSRC_2TYP.rsrc_typ_name , $$$.MDAS_AD_REPL.is_dirty , $$$.MDAS_TD_CONTAINER.container_name , $$$.MDAS_TD_RSRC_CLASS.rsrc_class_name , $$$.MDAS_AD_REPL.offset,  $$$.MDAS_AD_REPL.dchecksum FROM $$$.MDAS_ADC_REPL , $$$.MDAS_AD_REPL , $$$.MDAS_TD_DATA_TYP , $$$.MDAS_TD_LOCN , $$$.MDAS_AR_PHYSICAL , $$$.MDAS_TD_RSRC_2TYP , $$$.MDAS_TD_CONTAINER , $$$.MDAS_TD_RSRC_CLASS , $$$.MDAS_TD_DS_ACCS , $$$.MDAS_AD_ACCS , $$$.MDAS_CD_USER , $$$.MDAS_AU_GROUP WHERE ";
#ifdef ORAMCAT
char predefined_GDI_join_conditions_template[]= "AND $$$.MDAS_AD_REPL.data_typ_id = $$$.MDAS_TD_DATA_TYP.data_typ_id AND $$$.MDAS_ADC_REPL.rsrc_id = $$$.MDAS_AR_PHYSICAL.phy_rsrc_id AND $$$.MDAS_AD_ACCS.data_id = $$$.MDAS_ADC_REPL.data_id AND $$$.MDAS_AD_ACCS.user_id = $$$.MDAS_AU_GROUP.group_user_id AND $$$.MDAS_AR_PHYSICAL.rsrc_class_id = $$$.MDAS_TD_RSRC_CLASS.rsrc_class_id AND $$$.MDAS_AR_PHYSICAL.locn_id = $$$.MDAS_TD_LOCN.locn_id AND $$$.MDAS_AD_ACCS.access_id = $$$.MDAS_TD_DS_ACCS.access_id AND $$$.MDAS_AU_GROUP.user_id = $$$.MDAS_CD_USER.user_id AND $$$.MDAS_AR_PHYSICAL.rsrc_typ_id = $$$.MDAS_TD_RSRC_2TYP.rsrc_typ_id AND $$$.MDAS_ADC_REPL.data_id = $$$.MDAS_TD_CONTAINER.container_id(+) AND $$$.MDAS_AD_REPL.container_id = $$$.MDAS_ADC_REPL.data_id AND $$$.MDAS_AD_REPL.version_num = $$$.MDAS_ADC_REPL.version_num ORDER BY $$$.MDAS_AD_REPL.data_name";
#else
char predefined_GDI_join_conditions_template[]= "AND $$$.MDAS_AD_REPL.data_typ_id = $$$.MDAS_TD_DATA_TYP.data_typ_id AND $$$.MDAS_ADC_REPL.rsrc_id = $$$.MDAS_AR_PHYSICAL.phy_rsrc_id AND $$$.MDAS_AD_ACCS.data_id = $$$.MDAS_ADC_REPL.data_id AND $$$.MDAS_AD_ACCS.user_id = $$$.MDAS_AU_GROUP.group_user_id AND $$$.MDAS_AR_PHYSICAL.rsrc_class_id = $$$.MDAS_TD_RSRC_CLASS.rsrc_class_id AND $$$.MDAS_AR_PHYSICAL.locn_id = $$$.MDAS_TD_LOCN.locn_id AND $$$.MDAS_AD_ACCS.access_id = $$$.MDAS_TD_DS_ACCS.access_id AND $$$.MDAS_AU_GROUP.user_id = $$$.MDAS_CD_USER.user_id AND $$$.MDAS_AR_PHYSICAL.rsrc_typ_id = $$$.MDAS_TD_RSRC_2TYP.rsrc_typ_id AND $$$.MDAS_ADC_REPL.container_td_id = $$$.MDAS_TD_CONTAINER.container_id AND $$$.MDAS_AD_REPL.container_id = $$$.MDAS_ADC_REPL.data_id AND MDAS_ADC_REPL.version_num = MDAS_AD_REPL.version_num ORDER BY $$$.MDAS_AD_REPL.data_name";
#endif /* ORAMCAT */

char predefined_GDI_join_conditions[HUGE_STRING];
char predefined_GDI_select[HUGE_STRING];
#define DBQGEN 1


int mdasC_make_sql(mdasC_sql_query_struct *queryinfo,
		    char *sqlq)
{
  char in_table_name[MAX_DCS_STRUCTS][MAX_TOKEN];
  int given = 0;
  int i,j,flag,jj;
  char select_clause[HUGE_STRING] = "";
  char other_cond[HUGE_STRING] = "";
  char metadata_fkrel_file[MAX_TOKEN];
  char condexpr[HUGE_STRING],givenCond[HUGE_STRING];
  char *tmpCond, *tmpCond1;
  char tempStr[MAX_TOKEN];
  char tempStr2[MAX_TOKEN];
#ifdef MDATAACCS
  setAccessControlForMetaData(queryinfo);
#endif /* MDATAACCS */
  for (i = 0; i <  queryinfo->select_count ; i++){

     flag = 0;
     for (j = 0; j < given ; j++){
       if (!strcmp(in_table_name[j],queryinfo->sqlselect[i].tab_name))
	 flag = 1;
     }
     if (flag == 0){
       strcpy(in_table_name[given], queryinfo->sqlselect[i].tab_name);
       given++;
     }
     if (i > 0) sprintf(&select_clause[strlen(select_clause)]," , ");

     sprintf(&select_clause[strlen(select_clause)],"%s",
	     queryinfo->sqlselect[i].aggr_val);
  }
  for (i = 0; i <  queryinfo->condition_count ; i++){
    if (strstr(queryinfo->sqlwhere[i].tab_name,"MDAS_TD_ORDERBY") != NULL) {
      /*      elog(NOTICE, "Order By Input:%s\n",queryinfo->sqlwhere[i].att_value);*/
      makeOrderByString(queryinfo->sqlwhere[i].att_value);
      /*      elog(NOTICE, "Order By Output:%s\n",specialOrderByClause); */
      continue;
    }
    if (strstr(queryinfo->sqlwhere[i].tab_name,"MDAS_TD_UDF") != NULL) {
      strcpy(givenCond,queryinfo->sqlwhere[i].att_value);
      strcpy(condexpr ,"");
      jj = instantiateUDF(condexpr ,givenCond, in_table_name, given);
      if (jj < 0) return(jj);
      if (strlen(other_cond) > 0)
	strcat(other_cond," AND ");
      else
	strcat(other_cond," WHERE ");
      strcat(other_cond, condexpr);
      continue;
    }
     flag = 0;
     for (j = 0; j < given ; j++){
       if (!strcmp(in_table_name[j],queryinfo->sqlwhere[i].tab_name))
	 flag = 1;
     }
     if (flag == 0){
       strcpy(in_table_name[given], queryinfo->sqlwhere[i].tab_name);
       given++;
     }
/*
     if (i > 0) sprintf(&other_cond[strlen(other_cond)]," AND ");
     sprintf(&other_cond[strlen(other_cond)],"%s.%s %s",
	     queryinfo->sqlwhere[i].tab_name,
	     queryinfo->sqlwhere[i].att_name,
	     queryinfo->sqlwhere[i].att_value);
*/
     strcpy(givenCond,queryinfo->sqlwhere[i].att_value);
     strcpy(condexpr ,"");
     tmpCond = givenCond;
     if (strstr(tmpCond,"||") != NULL) {
       while( (tmpCond1 = strstr(tmpCond,"||")) != NULL) {
	 tmpCond1[0] =  '\0';
	 if (strlen(condexpr) > 0) 
	   sprintf(&condexpr[strlen(condexpr)]," OR ");
	 else strcpy(condexpr ," ( ");
	 sprintf(&condexpr[strlen(condexpr)],"%s.%s %s",
		 queryinfo->sqlwhere[i].tab_name,
		 queryinfo->sqlwhere[i].att_name,
		 tmpCond);
	 tmpCond = (char *) &tmpCond1[2];
       }
       sprintf(&condexpr[strlen(condexpr)]," OR %s.%s %s ) ",
	       queryinfo->sqlwhere[i].tab_name,
	       queryinfo->sqlwhere[i].att_name,
	       tmpCond);     
       if (i > 0) sprintf(&other_cond[strlen(other_cond)]," AND ");
       strcat(other_cond, condexpr);
     }
     else {
       while( (tmpCond1 = strstr(tmpCond,"&&")) != NULL) {
	 tmpCond1[0] =  '\0';
	 if (strlen(condexpr) > 0) 
	   sprintf(&condexpr[strlen(condexpr)]," AND ");
	 sprintf(&condexpr[strlen(condexpr)],"%s.%s %s",
		 queryinfo->sqlwhere[i].tab_name,
		 queryinfo->sqlwhere[i].att_name,
		 tmpCond);
	 tmpCond = (char *) &tmpCond1[2];
       }
       if (strlen(condexpr) > 0) 
	 sprintf(&condexpr[strlen(condexpr)]," AND ");
       sprintf(&condexpr[strlen(condexpr)],"%s.%s %s",
	       queryinfo->sqlwhere[i].tab_name,
	       queryinfo->sqlwhere[i].att_name,
	       tmpCond);     
       if (i > 0) sprintf(&other_cond[strlen(other_cond)]," AND ");
       strcat(other_cond, condexpr);
     }
  }
  /*  strcpy(metadata_fkrel_file, getenv("METADATA_FKREL")); */
  if (fkrelgraph_initialized == 0) {
    if (initialize_graph(metadatafkrelbuffer) < 0) 
      {
#ifdef DEBUGON
	elog(NOTICE, "ERROR: Unble to initialize graph\n");
#endif /* DEBUGON */ 
	return -1;
      }
  }
  convert_to_sql(given,  in_table_name, other_cond, select_clause, sqlq);
  strcat(sqlq,groupByList);


  /*******    RAJA ADDED Sep 27, 2004 REMOVE LATER *****/
  substituteInPlace(sqlq,"MDAS_ADCONT_REPL.data_size",
		         "MDAS_ADC_REPL.data_size   ");
  sprintf(tempStr,", %s%s",MDASSCHEMENAME,"MDAS_ADCONT_REPL");
  for (i = 0; i < strlen(tempStr); i++)
    tempStr2[i] = ' ';
  tempStr2[i] ='\0';
  substituteInPlace(sqlq,tempStr,tempStr2);
  sprintf(tempStr,"%s%s%s%s",MDASSCHEMENAME,"MDAS_ADCONT_REPL.data_id = ",
	  MDASSCHEMENAME,"MDAS_TD_CONTAINER.container_id  AND ");
  for (i = 0; i < strlen(tempStr); i++)
    tempStr2[i] = ' ';
  tempStr2[i] ='\0';
  substituteInPlace(sqlq,tempStr,tempStr2);
  /*******    RAJA ADDED Sep 27, 2004 REMOVE LATER *****/


  strcpy(GENERATEDSQL,sqlq);
#ifdef DEBUGON
  elog(NOTICE, "<P> \n INQUIRE SQL:%s\n",sqlq);
#endif /* DEBUGON */ 
  return 0;

}
void mdasC_db2_info_inquire(
        mdasC_servh* servh ,
        mdasC_infoh* info ,
        mdasC_infoh* result ,
        mdasC_status status, 
	int numrows
        )
{

    long int             hstmt;
    int i,ii, j,k, cnt;
    char sqlq[HUGH_HUGE_STRING];
    MDAS_DB2_SERVER_HNDL *db2srvh; 
    short int            num_of_cols;
    unsigned char       *data[MAX_TOKEN];
    long int             data_size[MAX_TOKEN];
    mdasC_sql_result_struct *newres;

        status[0] = 0 ;
        status[1] = 1 ;
        status[2] = MDAS_SUCCESS ;
        status[3] = MDAS_db2_INFO_INQUIRE ;

    db2srvh = (MDAS_DB2_SERVER_HNDL *) servh->conn;     

#ifdef LOGTIMINGMCAT
    logmess_db2("mdasC_db2_info_inquire: Before Calling mdasC_make_sql");
#endif /* LOGTIMINGMCAT */
    if (specialQueryFlag == 1) {
      i = 0;
      strcpy(sqlq, specialQueryString);
    }
    else if (getDatasetQuery == 1)
      i = mdasC_make_sql_using_predefined_dataset_query(
		 (mdasC_sql_query_struct *) info ,sqlq);
    else
      i = mdasC_make_sql((mdasC_sql_query_struct *) info ,sqlq);
    if (i < 0) 
      {status[0]=-10;status[2]=MDAS_FAILURE;  
#ifdef DEBUGON
    elog(NOTICE,"mdasC_make_sql failed:%i\n",i); 
#endif /* DEBUGON */ 
    return;}

#ifdef LOGTIMINGMCAT
    logmess_db2("mdasC_db2_info_inquire: Before Calling exec_sql_db2");
#endif /* LOGTIMINGMCAT */
    i = exec_sql_db2(db2srvh->henv, db2srvh->hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i < 0) 
      {status[0]=-2;status[2]=MDAS_FAILURE;    
#ifdef DEBUGON
     elog(NOTICE,"exec_sql_db2 failed:%i\n",i);  
#endif /* DEBUGON */ 
    return;}
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i < 0) 
      {status[0]=-3;status[2]=MDAS_FAILURE;    
#ifdef DEBUGON
      elog(NOTICE,"get_no_of_columns_db2 failed:%i\n",i); 
#endif /* DEBUGON */ 
     return;}
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i < 0) 
      {status[0]=-4;status[2]=MDAS_FAILURE;    
#ifdef DEBUGON
      elog(NOTICE,"bind_default_row_db2 failed:%i\n",i); 
#endif /* DEBUGON */ 
     return;}
    i = get_row_db2(hstmt);
    if (i < 0) 
      {status[0]=-5;status[2]=MDAS_FAILURE;     
       done_with_default_result_db2(
	       hstmt,data,data_size, num_of_cols);

#ifdef DEBUGON
      elog(NOTICE,"get_row_db2 failed:%i\n",i);
#endif /* DEBUGON */ 
      return;}
    newres = (mdasC_sql_result_struct *) result;
    k = 0;
    cnt = 0;
/*** added May 4,2001 to accomodate hidden select elements used by 
     metadata access control ***/
    num_of_cols = newres->result_count;
/*** added May 4,2001 to accomodate hidden select elements used by 
     metadata access control ***/

    while (i == 0 && cnt < (numrows-1) )
      {
	for (j=0; j < num_of_cols;j++){
	      truncate_end_blanks(data_size[j], data[j]);
 	      if (data[j] == NULL)
		data_size[j] = 0;
 	      else
		data_size[j] = strlen((char *) data[j]); /**KESTOCK-01/24/2001**/
              sprintf((char *)
		 (((int) (newres->sqlresult[j].values)) + (k * MAX_DATA_SIZE)),
		      "%s", data[j]);
	      strcpy((char *)data[j],"");
	}
	for (j=0; j < num_of_cols;j++) {
	  if (data[j] != NULL) {
	    strcpy((char *)data[j],"");
	    data_size[j] = 0;
	  }
	}
	i = get_row_db2(hstmt);
	k++;
	cnt++;
      }
    if (i == 0)
      {
	for (j=0; j < num_of_cols;j++){
	      truncate_end_blanks(data_size[j], data[j]);
 	      if (data[j] == NULL)
		data_size[j] = 0;
 	      else
		data_size[j] = strlen((char *) data[j]); /**KESTOCK-01/24/2001**/
              sprintf((char *)
		 (((int) (newres->sqlresult[j].values)) + (k * MAX_DATA_SIZE)),
		      "%s", data[j]);
	}
	k++;
	cnt++;
      }

    /*
    for (j=0; j < num_of_cols;j++){
      sprintf((char *)
	      (((int) (newres->sqlresult[j].values)) + (k * MAX_DATA_SIZE)),
		      "%s", RESWANTED);
    }
    */
    
    if (cnt >= numrows)
      {
	for (i =  0; i < num_of_cols; i++){
	  free(data[i]);
	}
	for (i = 0; i < MAX_QUERY_LIMIT; i++)
	  {
	    if (queryCont[i].id == 0) break;
	  }
	queryCont[i].id = 1;
	queryCont[i].hstmt =  hstmt;
	queryCont[i].colcount = num_of_cols;
	for (ii = 0; ii < num_of_cols; ii++){
	  queryCont[i].sqlselect[ii].tab_name=newres->sqlresult[ii].tab_name;
	  queryCont[i].sqlselect[ii].att_name=newres->sqlresult[ii].att_name;
	}
	newres->continuation_index = i;
      }
    else
      {
	i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
	newres->continuation_index = -1;
      }
    newres->result_count = num_of_cols;
    newres->row_count = cnt;

}

void mdasC_db2_info_inquire_more(int cidx,
				 mdasC_infoh* result ,  
				 mdasC_status status,
				 int numrows
				 )
{
    int i,j,k, cnt;
    long int hstmt;
    short int            num_of_cols;
    unsigned char       *data[MAX_TOKEN];
    long int             data_size[MAX_TOKEN];
    mdasC_sql_result_struct *newres;


    status[0] = 0 ;
    status[1] = 1 ;
    status[2] = MDAS_SUCCESS ;
    status[3] = MDAS_db2_INFO_INQUIRE ;
    num_of_cols = queryCont[cidx].colcount;
    hstmt = queryCont[cidx].hstmt;

    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i < 0) {queryCont[cidx].id = 0; /* done_with_result_db2(hstmt); RAJA remd Aug19, 2003 */
                status[0]=-4;status[2]=MDAS_FAILURE; return;}
    i = get_row_db2(hstmt);
    if (i < 0) {queryCont[cidx].id = 0;
               /* done_with_default_result_db2(
				       hstmt,data,data_size, num_of_cols);
		 RAJA remd Aug19, 2003 */
                status[0]=-5;status[2]=MDAS_FAILURE; return;}
    newres = (mdasC_sql_result_struct *) result;
    for (k = 0; k <num_of_cols ; k++)
      {
	newres->sqlresult[k].tab_name = queryCont[cidx].sqlselect[k].tab_name;
	newres->sqlresult[k].att_name = queryCont[cidx].sqlselect[k].att_name;
	newres->sqlresult[k].values  = (char *)  
	    malloc(numrows * MAX_DATA_SIZE * sizeof(char));
      }
    k = 0;
    cnt = 0;

    while (i == 0 && cnt < (numrows-1) )
      {
	for (j=0; j < num_of_cols;j++){
	  truncate_end_blanks(data_size[j], data[j]);
	  if (data[j] == NULL)
	    data_size[j] = 0;
	  else
	    data_size[j] = strlen((char *) data[j]); /**KESTOCK-01/24/2001**/ 
	  sprintf((char *)
		 (((int) (newres->sqlresult[j].values)) + (k * MAX_DATA_SIZE)),
		      "%s", data[j]);
	  strcpy((char *)data[j],"");
	}
	for (j=0; j < num_of_cols;j++) {
	  if (data[j] != NULL) {
	    strcpy((char *)data[j],"");
	    data_size[j] = 0;
	  }
	}
	i = get_row_db2(hstmt);
	k++;
	cnt++;
      }
    if (i == 0)
      {
	for (j=0; j < num_of_cols;j++){
	      truncate_end_blanks(data_size[j], data[j]);
 	      if (data[j] == NULL)
		data_size[j] = 0;
 	      else
		data_size[j] = strlen((char *) data[j]); /**KESTOCK-01/24/2001**/
              sprintf((char *)
		 (((int) (newres->sqlresult[j].values)) + (k * MAX_DATA_SIZE)),
		      "%s", data[j]);
	}
	k++;
	cnt++;
      }

    /*
      for (j=0; j < num_of_cols;j++){
      sprintf((char *)
	      (((int) (newres->sqlresult[j].values)) + (k * MAX_DATA_SIZE)),
		      "%s", RESWANTED);
      }
    */
    

    if (cnt >= numrows)
      {
	for (i =  0; i < num_of_cols; i++){
	  free(data[i]);
	}
	newres->continuation_index = (int) cidx;
      }
    else
      {
	i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
	newres->continuation_index = -1;
	queryCont[cidx].id = 0;
      }
    newres->result_count = queryCont[cidx].colcount;
    newres->row_count = cnt;

}



int mdasC_make_sql_using_predefined_dataset_query(
	     mdasC_sql_query_struct *queryinfo,
	     char *sqlq)
{
  char other_cond[HUGE_STRING] = "";
  int i;
  char *tmpStr1,*tmpStr2,*tmpStr3;

  if (predefined_GDI_initialized ==  0) {
    tmpStr1 =  predefined_GDI_select_template;
    predefined_GDI_select[0] ='\0';
    while ((tmpStr2 = strstr(tmpStr1,"$$$.")) != NULL) {
       *tmpStr2 = '\0';
       strcat(predefined_GDI_select, tmpStr1);
       strcat(predefined_GDI_select, MDASSCHEMENAME);
       tmpStr1 = tmpStr2 + 4;
    }
    strcat(predefined_GDI_select, tmpStr1);

    tmpStr1 =  predefined_GDI_join_conditions_template;
    predefined_GDI_join_conditions[0] ='\0';
    while ((tmpStr2 = strstr(tmpStr1,"$$$.")) != NULL) {
       *tmpStr2 = '\0';
       strcat(predefined_GDI_join_conditions, tmpStr1);
       strcat(predefined_GDI_join_conditions, MDASSCHEMENAME);
       tmpStr1 = tmpStr2 + 4;
    }
    strcat(predefined_GDI_join_conditions, tmpStr1);
    predefined_GDI_initialized = 1;
  }

  for (i = 0; i <  queryinfo->condition_count ; i++){
    if (i > 0) 
      sprintf(&other_cond[strlen(other_cond)]," AND ");
    sprintf(&other_cond[strlen(other_cond)],"%s.%s %s",
	    queryinfo->sqlwhere[i].tab_name,
	    queryinfo->sqlwhere[i].att_name,
	    queryinfo->sqlwhere[i].att_value);
  }
  sprintf(sqlq, "%s %s %s", 
	    predefined_GDI_select, 
	    other_cond,
	    predefined_GDI_join_conditions); 
  getDatasetQuery = 0;
  return 0;
}

#ifdef MDATAACCS
int
setAccessControlForMetaData(mdasC_sql_query_struct *queryinfo)
{
  int i,j,k, accsId;
  int flagC = 0;
  int flagD = 0;
  char tmpStr[MAX_TOKEN];
  int ownFlag = 0;
  int dhideFlag = 0;

  if (isLocalPrivUser() == MDAS_SUCCESS)
    return (MDAS_SUCCESS);
  for (i = 0; i <  queryinfo->condition_count ; i++){
    if (strstr(queryinfo->sqlwhere[i].tab_name,dcs_tname[DATA_OWNER]) != NULL 
	&&
	strstr(queryinfo->sqlwhere[i].att_name,dcs_aname[DATA_OWNER]) != NULL
	&&
       queryinfo->sqlwhere[i].att_value[0] == '=' )
      ownFlag = 1;
    if (strstr(queryinfo->sqlwhere[i].tab_name,dcs_tname[DATA_HIDE]) != NULL 
	&&
	strstr(queryinfo->sqlwhere[i].att_name,dcs_aname[DATA_HIDE]) != NULL)
      dhideFlag = 1;
  }
  for (i = 0; i <  queryinfo->condition_count ; i++){
    if (strstr(queryinfo->sqlwhere[i].tab_name,"MDAS_AU_TCKT_DATA") != NULL ||
	strstr(queryinfo->sqlwhere[i].tab_name,"MDAS_AU_TCKT_GRP") != NULL)
      return(0);
  }
  for (i = 0; i <  queryinfo->select_count ; i++){
    for (j = 0; strcmp(protectedDataTableList[j],"null") ; j++){
       if (strstr(queryinfo->sqlselect[i].tab_name,protectedDataTableList[j]
		   )!=NULL) {
	 flagD = 1; 
	 break;
       }
    }
    if (flagD == 1)  break;
  }

  if (flagD == 0) {
    for (i = 0; i <  queryinfo->select_count ; i++){
      for (j = 0; strcmp(protectedCollTableList[j],"null") ; j++){
	if (strstr(queryinfo->sqlselect[i].tab_name,protectedCollTableList[j]
		   )!=NULL) {
	  flagC = 1; 
	  break;
	}
      }
      if (flagC == 1)  break;
    }
  }
  if (flagD == 0) {
    for (i = 0; i <  queryinfo->condition_count ; i++){
      for (j = 0; strcmp(protectedDataTableList[j],"null") ; j++){
	if (strstr(queryinfo->sqlwhere[i].tab_name,protectedDataTableList[j]
		    )!=NULL) {
	  flagD = 1; 
	  break;
	}
      }
      if (flagD == 1)  break;
    }
  }

  if (flagC == 0 && flagD == 0) {
    for (i = 0; i <  queryinfo->condition_count ; i++){
      for (j = 0; strcmp(protectedCollTableList[j],"null") ; j++){
	if (strstr(queryinfo->sqlwhere[i].tab_name,protectedCollTableList[j]
		    )!=NULL) {
	  flagC = 1; 
	  break;
	}
      }
      if (flagC == 1)  break;
    }
  }

  j = queryinfo->select_count;
  k = queryinfo->condition_count;
  if (flagD == 1) {
    /*** RAJA removed November 3, 2003 to reduce extra answers ****
      for (i = 0; i <  queryinfo->select_count ; i++){
	if (!strcmp(queryinfo->sqlselect[i].tab_name,dcs_tname[DATA_ID]) &&
	    !strcmp(queryinfo->sqlselect[i].att_name,dcs_aname[DATA_ID]))
	  break;
      }
      if (i >= queryinfo->select_count) {
	queryinfo->sqlselect[j].tab_name = (char *) dcs_tname[DATA_ID];
	queryinfo->sqlselect[j].att_name = (char *) dcs_aname[DATA_ID];
	sprintf(queryinfo->sqlselect[j].aggr_val,
		"%s.%s ",dcs_tname[DATA_ID],dcs_aname[DATA_ID]);
	j++;
	if (strlen(groupByList) > 0) {
	  sprintf(tmpStr,", %s.%s ", dcs_tname[DATA_ID],dcs_aname[DATA_ID]);
	  strcat(groupByList,tmpStr );
	}
      }
    *** RAJA removed November 3, 2003 to reduce extra answers ****/
    queryinfo->sqlwhere[k].tab_name = (char *) dcs_tname[DATA_ACCS_USER_ID];
    queryinfo->sqlwhere[k].att_name = (char *) dcs_aname[DATA_ACCS_USER_ID];
    if (curUserId == -1)
      generateCurUserId();
    sprintf(protectedTableValue[0], " = %i",  curUserId );
    queryinfo->sqlwhere[k].att_value = protectedTableValue[0];
    k++;
    queryinfo->sqlwhere[k].tab_name = (char *) dcs_tname[DATA_ACCS_ID];
    queryinfo->sqlwhere[k].att_name = (char *) dcs_aname[DATA_ACCS_ID];
    accsId = getAccessId("read");
    sprintf(protectedTableValue[1], "  >= %i",accsId);
    queryinfo->sqlwhere[k].att_value = protectedTableValue[1];
    k++;
    if (dhideFlag == 0 || ownFlag == 0) {
      queryinfo->sqlwhere[k].tab_name = (char *) dcs_tname[DATA_HIDE];
      queryinfo->sqlwhere[k].att_name = (char *) dcs_aname[DATA_HIDE];
      sprintf(protectedTableValue[5], " = 0 ");
      queryinfo->sqlwhere[k].att_value = protectedTableValue[5];
      k++;
    }
    /** need to increase protectedTableValue beyond 5 in mdasC_db2_externs.h  **/
  }
  if (flagC == 1) {
     /*** RAJA removed November 3, 2003 to reduce extra answers ****
      for (i = 0; i <  queryinfo->select_count ; i++){
	if (!strcmp(queryinfo->sqlselect[i].tab_name,dcs_tname[COLLECTION_NAME]) &&
	    !strcmp(queryinfo->sqlselect[i].att_name,dcs_aname[COLLECTION_NAME]))
	  break;
      }
      if (i >= queryinfo->select_count) {
	queryinfo->sqlselect[j].tab_name = (char *) dcs_tname[COLLECTION_NAME];
	queryinfo->sqlselect[j].att_name = (char *) dcs_aname[COLLECTION_NAME];
	sprintf(queryinfo->sqlselect[j].aggr_val,
		"%s.%s ",dcs_tname[COLLECTION_NAME],dcs_aname[COLLECTION_NAME]);
	j++;
	if (strlen(groupByList) > 0) {
	  sprintf(tmpStr,", %s.%s ", dcs_tname[COLLECTION_NAME],dcs_aname[COLLECTION_NAME]);
	  strcat(groupByList,tmpStr );
	}
      }
    *** RAJA removed November 3, 2003 to reduce extra answers ****/
    queryinfo->sqlwhere[k].tab_name = (char *) dcs_tname[COLL_ACCS_USER_ID];
    queryinfo->sqlwhere[k].att_name = (char *) dcs_aname[COLL_ACCS_USER_ID];
    if (curUserId == -1)
      generateCurUserId();
    sprintf(protectedTableValue[3], " = %i",  curUserId );
    queryinfo->sqlwhere[k].att_value = protectedTableValue[3];
    k++;
    queryinfo->sqlwhere[k].tab_name = (char *) dcs_tname[COLL_ACCS_ID];
    queryinfo->sqlwhere[k].att_name = (char *) dcs_aname[COLL_ACCS_ID];
    accsId = getAccessId("read");
    sprintf(protectedTableValue[4], "  >= %i",accsId);
    queryinfo->sqlwhere[k].att_value = protectedTableValue[4];
    k++;
    /** need to increase protectedTableValue beyond 5 in mdas_db2_externs.h  **/
  }
  queryinfo->select_count = j;
  queryinfo->condition_count = k;
  return(0);

}

int
generateCurUserId()
{
 
  int i;
  
  failure = 0;
  sprintf(sqlq, "select t1.user_id from %sMDAS_CD_USER t1, %sMDAS_AU_DOMN t3, %sMDAS_TD_DOMN t4 where t1.user_name = '%s' AND t1.user_id = t3.user_id AND t3.domain_id = t4.domain_id and t4.domain_desc ='%s'",
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  ClientUser->userName,  ClientUser->domainName);
  if ((i = get_itype_value_from_query(sqlq)) < 0) {
    return (USER_AUTHORIZATION_ERROR);
  }
  curUserId = i;
  return(MDAS_SUCCESS);
}

#endif /* MDATAACCS */

int
makeOrderByString(char *obList)
{
  int i,j,inx;
  char *tmpPtr1, *tmpPtr2, *tmpPtr3;
  char tmpBuf[MAX_TOKEN];
  i  = 0;
  tmpPtr1 = obList;
  while ((tmpPtr2 = strchr(tmpPtr1,',')) != NULL) {
    *tmpPtr2 = '\0';
    strcpy(tmpBuf,tmpPtr1);
    trimsides(tmpBuf);
    inx = getDCSIndex(tmpBuf);
    if (inx < 0) {
      elog(NOTICE,"Bad Order By Value %s: %i\n",tmpBuf,inx);
     return (inx);
    }
    if (i == 0) {
        strcpy(specialOrderByClause," ORDER BY ");
	i =  1;
    }
    else
      strcat(specialOrderByClause,", ");
    strcat(specialOrderByClause , dcs_tname[inx]);
    strcat(specialOrderByClause , ".");
    strcat(specialOrderByClause , dcs_aname[inx]);
    *tmpPtr2 = ',';
    tmpPtr1 =  tmpPtr2 + 1;
  }
  strcpy(tmpBuf,tmpPtr1);
  trimsides(tmpBuf);
  inx = getDCSIndex(tmpBuf);
  if (inx < 0)
    return (inx);
  if (i == 0) {
    strcpy(specialOrderByClause," ORDER BY ");
    i =  1;
  }
  else
    strcat(specialOrderByClause,", ");
  strcat(specialOrderByClause , dcs_tname[inx]);
  strcat(specialOrderByClause , ".");
  strcat(specialOrderByClause , dcs_aname[inx]);
  return(0);
}
int
instantiateUDF(char *condexpr ,char *givenCond,
	       char in_table_name[][MAX_TOKEN], int given)
{
  int i, j, inx;
  char *tmpPtr1, *tmpPtr2, *tmpPtr3;
  tmpPtr1 = givenCond;
  while ((tmpPtr2 = strstr(tmpPtr1,"SRBATTR[")) != NULL) {
    *tmpPtr2 = '\0';
    tmpPtr2++;
    strcat (condexpr ,tmpPtr1);
    if ((tmpPtr3 = strstr(tmpPtr2,"]")) == NULL)
      return(ERROR_IN_QUERY_FORMATION);
    *tmpPtr3 = '\0';
    tmpPtr1 = tmpPtr3 + 1;
    inx = getDCSIndex((char *)(tmpPtr2 + 7));
    if (inx < 0)
     return (inx);
    strcat(condexpr , dcs_tname[inx]);
    strcat(condexpr , ".");
    strcat(condexpr , dcs_aname[inx]);

     for (j = 0; j < given ; j++){
       if (!strcmp(in_table_name[j],dcs_tname[inx]))
	 break;
     }
     if (j < given ){
       strcpy(in_table_name[given], dcs_tname[inx]);
       given++;
     }

  }
  strcat(condexpr ,tmpPtr1);
  return(0);
}

int 
getDCSIndex( char *cname)
{
  int i;

  for (i = 0; i < MAX_DCS_NUM; i++) {
    if (!strcmp(dcs_cname[i],cname))
      return(i);
  }
  if (!strcmp(cname,"COLLECTION_NAME")) return(COLLECTION_NAME);
  if (!strcmp(cname,"GROUP_ACCESS_CONSTRAINT")) return(GROUP_ACCESS_CONSTRAINT);
  if (!strcmp(cname,"ACCESS_GROUP_NAME")) return(ACCESS_GROUP_NAME);
  if (!strcmp(cname,"UDSMD0_0")) return(UDSMD0_0);
  if (!strcmp(cname,"UDSMD1_0")) return(UDSMD1_0);
  if (!strcmp(cname,"UDSMD_RSRC0_0")) return(UDSMD_RSRC0_0);
  if (!strcmp(cname,"UDSMD_RSRC1_0")) return(UDSMD_RSRC1_0);
  if (!strcmp(cname,"UDSMD_USER0_0")) return(UDSMD_USER0_0);
  if (!strcmp(cname,"UDSMD_USER1_0")) return(UDSMD_USER1_0);
  if (!strcmp(cname,"UDSMD_COLL0_0")) return(UDSMD_COLL0_0);
  if (!strcmp(cname,"UDSMD_COLL1_0")) return(UDSMD_COLL1_0);
  return(UNKNOWN_ATTRIBUTE_NAME);
}

int
substituteInPlace(char *inString, char *key, char *value)
{
  int i,j,k;
  char *p, *q;

  k = strlen(key);
  if (k != strlen(value))
    return(-1);
  q = inString;
  while ((p = strstr(q,key)) != NULL) {
    memcpy(p,value,k);
    q = p +  k;
  }
  return(0);
}
