/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

#ifdef SRB_MDAS
 
#include "mdasC_db2_externs.h"
#include "srbC_mdas_externs.h"
#include "mdasGlobalsExtern.h"
#include "elogExtern.h"
#include "srbMiscExtern.h"
#ifdef PORTNAME_solaris
#include <strings.h>
#endif


extern void freeSqlResult (mdasC_sql_result_struct *myresult);
void rmGlbFromResult (mdasC_sql_result_struct  *myresult);
extern char * getFromResultStruct(mdasC_sql_result_struct *result,
                              char *tab_name, char *att_name);

int
reload_ext_tab_attr_index(int cat_type, char *schemaValues );
int get_mdas_authorization(char *user_name, char *user_password, char* domain_name)
{
    failure = 0;
    strcpy(reg_action_name,"authorization");
    i =  web_mdas_authorization(user_name,user_password, domain_name );
    if (i != 0) 
      {
        sprintf(errmess,"MDAS Authent API: Failed Password:%s,%s,%s",
		user_password,user_name, domain_name);

	return (USER_AUTHORIZATION_ERROR);
      }
    else 
      {
    return(MDAS_SUCCESS);
      }
}

int get_mdas_sys_authorization(char *user_name, char *user_password, char* domain_name)
{
    failure = 0;
    strcpy(reg_action_name,"authorization");
    i =  web_mdas_sys_authorization(user_name,user_password, domain_name);
    if (i != 0) 
      {
        sprintf(errmess,"MDAS System Authent API:  Failed Password:%s,%s,%s",
		user_password,user_name, domain_name);

	return (SYS_USER_AUTHORIZATION_ERROR);
      }
    else 
      {
	return(MDAS_SUCCESS);
      }
}


int iGetPasswordFromMcat(char *user_name, char *user_password, char *domain_name)
{
  int i;
  char data_value_encoded[MAX_TOKEN];
  failure = 0;
  /* elog(DEBUG,"CALLING IGETPASSWORDFROMMCAT:%s@%s\n",user_name,domain_name);*/
#ifdef FED_MCAT
  sprintf(sqlq, "select t2.priv_key from %sMDAS_CD_USER t1, %sMDAS_AU_AUTH_KEY t2, %sMDAS_AU_DOMN t3, %sMDAS_TD_DOMN t4, %sMDAS_TD_ZONE t5 where t1.user_name = '%s' and t1.user_id = t2.user_id  AND t1.user_id = t3.user_id AND t3.domain_id = t4.domain_id and t4.domain_desc ='%s' and t1.zone_id = t5.zone_id and t5.local_zone_flag = 1",
	  MDASSCHEMENAME, MDASSCHEMENAME,MDASSCHEMENAME, MDASSCHEMENAME,MDASSCHEMENAME,
	  user_name,  domain_name);
#else
  sprintf(sqlq, "select t2.priv_key from %sMDAS_CD_USER t1, %sMDAS_AU_AUTH_KEY t2, %sMDAS_AU_DOMN t3, %sMDAS_TD_DOMN t4 where t1.user_name = '%s' and t1.user_id = t2.user_id  AND t1.user_id = t3.user_id AND t3.domain_id = t4.domain_id and t4.domain_desc ='%s'",
	  MDASSCHEMENAME, MDASSCHEMENAME,MDASSCHEMENAME, MDASSCHEMENAME,
	  user_name,  domain_name);
#endif
  i = get_ctype_value_from_query(data_value_encoded, sqlq);
  if (failure != 0) 
    return (USER_AUTHORIZATION_ERROR);
  obf2Decode(data_value_encoded, user_password, OBF2_EXTRA_KEY);
  return(MDAS_SUCCESS);
}

int web_mdas_authorization(char *user_name, char *user_password, char* domain_name)
{
  int i;
  char data_value_encoded[MAX_TOKEN];
  obf2Encode(user_password, data_value_encoded, MAX_TOKEN, OBF2_EXTRA_KEY);
  /* elog(DEBUG,"CALLING NORMAL_AUTH:%s@%s\n",user_name,domain_name);*/
    failure = 0;
    sprintf(sqlq, "select t1.user_id from %sMDAS_CD_USER t1, %sMDAS_AU_AUTH_KEY t2, %sMDAS_AU_DOMN t3, %sMDAS_TD_DOMN t4 where t1.user_name = '%s' and t1.user_id = t2.user_id AND t2.priv_key = '%s' AND t1.user_id = t3.user_id AND t3.domain_id = t4.domain_id and t4.domain_desc ='%s'",
	    MDASSCHEMENAME, MDASSCHEMENAME,MDASSCHEMENAME, MDASSCHEMENAME,
	    user_name, data_value_encoded, domain_name);
    tabledIsOkay = 1;
    if ((i = get_itype_value_from_query(sqlq)) < 0) {
      sprintf(sqlq, "select t1.user_id from %sMDAS_CD_USER t1, %sMDAS_AU_AUTH_KEY t2, %sMDAS_AU_DOMN t3, %sMDAS_TD_DOMN t4 where t1.user_name = '%s' and t1.user_id = t2.user_id AND t2.priv_key = '%s' AND t1.user_id = t3.user_id AND t3.domain_id = t4.domain_id and t4.domain_desc ='%s'",
	    MDASSCHEMENAME, MDASSCHEMENAME,MDASSCHEMENAME, MDASSCHEMENAME,
	    user_name, user_password, domain_name);
      if ((i = get_itype_value_from_query(sqlq)) < 0) {
	tabledIsOkay = 0;
	return (USER_AUTHORIZATION_ERROR);
      }
    }
    tabledIsOkay = 0;
    curUserId = i;
    return(MDAS_SUCCESS);
}
int isUserSysAdmin(char *user_name, char* domain_name)
{
    int i;
    char localZoneName[MAX_TOKEN];

    failure = 0;    
    strcpy(localZoneName,"");
#ifdef FED_MCAT    
    i = isLocalPrivUser();
    if (i == AUTH_ERR_PROXY_NOPRIV) {
      failure = 0;
      sprintf(query_string, "select zone_id from %s%s where local_zone_flag = 1",
	      MDASSCHEMENAME, "MDAS_TD_ZONE");
      get_ctype_value_from_query(localZoneName,query_string );
      if (failure != 0) 
	return( LOCAL_ZONE_NOT_FOUND);
    }
    else 
      return(i);
#endif
    if (strlen(localZoneName) > 0) {
      sprintf(sqlq, "select t1.user_id from %sMDAS_CD_USER t1, %sMDAS_TD_USER_TYP t3 , %sMDAS_AU_DOMN t5, %sMDAS_TD_DOMN t4 where t1.user_name = '%s'  and t1.zone_id = '%s' and t1.user_typ_id = t3.user_typ_id AND t3.user_typ_name = 'sysadmin' AND t1.user_id = t5.user_id AND t5.domain_id = t4.domain_id and t4.domain_desc ='%s'",
	    MDASSCHEMENAME, MDASSCHEMENAME,
	     MDASSCHEMENAME, MDASSCHEMENAME,
	    user_name, localZoneName, domain_name);
    }
    else {
    sprintf(sqlq, "select t1.user_id from %sMDAS_CD_USER t1, %sMDAS_TD_USER_TYP t3 , %sMDAS_AU_DOMN t5, %sMDAS_TD_DOMN t4 where t1.user_name = '%s'  and t1.user_typ_id = t3.user_typ_id AND t3.user_typ_name = 'sysadmin' AND t1.user_id = t5.user_id AND t5.domain_id = t4.domain_id and t4.domain_desc ='%s'",
	    MDASSCHEMENAME, MDASSCHEMENAME,
	     MDASSCHEMENAME, MDASSCHEMENAME,
	    user_name, domain_name);
    }
    tabledIsOkay = 1;
    if ((i = get_itype_value_from_query(sqlq)) < 0) {
      tabledIsOkay = 0;
      return (SYS_USER_AUTHORIZATION_ERROR);
    }
    tabledIsOkay = 0;
    return(MDAS_SUCCESS);
}

int isUserDomainAdmin(char *user_name, char* domain_name)
{
    int i;
    char localZoneName[MAX_TOKEN];

    failure = 0;    
    strcpy(localZoneName,"");
#ifdef FED_MCAT    
    sprintf(query_string, "select zone_id from %s%s where local_zone_flag = 1",
	      MDASSCHEMENAME, "MDAS_TD_ZONE");
    get_ctype_value_from_query(localZoneName,query_string );
      if (failure != 0) 
	return( LOCAL_ZONE_NOT_FOUND);
#endif
    if (strlen(localZoneName) > 0) {
      sprintf(sqlq, "select t1.user_id from %sMDAS_CD_USER t1, %sMDAS_TD_USER_TYP t3 , %sMDAS_AU_DOMN t5, %sMDAS_TD_DOMN t4 where t1.user_name = '%s'  and t1.zone_id = '%s' and t1.user_typ_id = t3.user_typ_id AND t3.user_typ_name = 'domainadmin' AND t1.user_id = t5.user_id AND t5.domain_id = t4.domain_id and t4.domain_desc ='%s'",
	    MDASSCHEMENAME, MDASSCHEMENAME,
	     MDASSCHEMENAME, MDASSCHEMENAME,
	    user_name, localZoneName, domain_name);
    }
    else {
    sprintf(sqlq, "select t1.user_id from %sMDAS_CD_USER t1, %sMDAS_TD_USER_TYP t3 , %sMDAS_AU_DOMN t5, %sMDAS_TD_DOMN t4 where t1.user_name = '%s'  and t1.user_typ_id = t3.user_typ_id AND t3.user_typ_name = 'domainadmin' AND t1.user_id = t5.user_id AND t5.domain_id = t4.domain_id and t4.domain_desc ='%s'",
	    MDASSCHEMENAME, MDASSCHEMENAME,
	     MDASSCHEMENAME, MDASSCHEMENAME,
	    user_name, domain_name);
    }
    tabledIsOkay = 1;
    if ((i = get_itype_value_from_query(sqlq)) < 0) {
      tabledIsOkay = 0;
      return (SYS_USER_AUTHORIZATION_ERROR);
    }
    tabledIsOkay = 0;
    return(MDAS_SUCCESS);
}

int web_mdas_sys_authorization(char *user_name, char *user_password, char* domain_name)
{
    int i;
    char localZoneName[MAX_TOKEN];
    char data_value_encoded[MAX_TOKEN];

    failure = 0;
    strcpy(localZoneName,"");
#ifdef FED_MCAT    
    i = isLocalPrivUser();
    if (i == AUTH_ERR_PROXY_NOPRIV) {
      failure = 0;
      sprintf(query_string, "select zone_id from %s%s where local_zone_flag = 1",
	      MDASSCHEMENAME, "MDAS_TD_ZONE");
      get_ctype_value_from_query(localZoneName,query_string );
      if (failure != 0) 
	return( LOCAL_ZONE_NOT_FOUND);
    }
    else 
      return(i);
#endif
    obf2Encode(user_password, data_value_encoded, MAX_TOKEN, OBF2_EXTRA_KEY);

    tabledIsOkay = 1;
    if (i = web_auth2(localZoneName, user_name,  data_value_encoded, domain_name) < 0) {
      if (i = web_auth2(localZoneName, user_name,  user_password, domain_name) < 0) {
	tabledIsOkay = 0;
	return (SYS_USER_AUTHORIZATION_ERROR);
      }
    }
    tabledIsOkay = 0;
    /***    curUserId = i; taken out Dec 14,2000 */
    return(MDAS_SUCCESS);
}

/*
 called only by web_mdas_sys_authorization; does the query to MCAT to
 check on the user authentication.
 */
int web_auth2(char *localZoneName, char * user_name, char * user_password, char * domain_name)
{
    int i;
    if (strlen(localZoneName) > 0) {
      elog(DEBUG,"CALLING SYSTEM_AUTH:%s@%s\n",user_name,domain_name);

      sprintf(sqlq, "select t1.user_id from %sMDAS_CD_USER t1, %sMDAS_AU_AUTH_KEY t2, %sMDAS_TD_USER_TYP t3 , %sMDAS_AU_DOMN t5, %sMDAS_TD_DOMN t4 where t1.user_name = '%s' and t1.zone_id = '%s' and t1.user_id = t2 .user_id AND t2.priv_key = '%s' and t1.user_typ_id = t3.user_typ_id AND t3.user_typ_name = 'sysadmin' AND t1.user_id = t5.user_id AND t5.domain_id = t4.domain_id and t4.domain_desc ='%s'",
	    MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	     MDASSCHEMENAME, MDASSCHEMENAME,
	    user_name, localZoneName, user_password, domain_name);
    }
    else {
      elog(DEBUG,"CALLING SYSTEM_AUTH:%s@%s\n",user_name,domain_name);

      sprintf(sqlq, "select t1.user_id from %sMDAS_CD_USER t1, %sMDAS_AU_AUTH_KEY t2, %sMDAS_TD_USER_TYP t3 , %sMDAS_AU_DOMN t5, %sMDAS_TD_DOMN t4 where t1.user_name = '%s' and t1.user_id = t2 .user_id AND t2.priv_key = '%s' and t1.user_typ_id = t3.user_typ_id AND t3.user_typ_name = 'sysadmin' AND t1.user_id = t5.user_id AND t5.domain_id = t4.domain_id and t4.domain_desc ='%s'",
	    MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	     MDASSCHEMENAME, MDASSCHEMENAME,
	    user_name, user_password, domain_name);
    }
    i = get_itype_value_from_query(sqlq);
    return i;
}

int
is_curator_userid(int user_id,  char *collName ) 
{
  int i;
  char collId[MAX_TOKEN + 10];
  char collId2[MAX_TOKEN];
  char collId3[MAX_TOKEN];
  
  get_ctype_value(collId2,"MDAS_TD_DATA_GRP.data_grp_name",collName);
   if (failure != 0) 
     { return (COLLECTION_NOT_IN_CAT);}
  
  sprintf(collId,"%s.0000",collId2);
  i  = NO_ACCS_TO_USER_IN_COLLECTION;
  while (i < 0 && strlen(collId) > 4) {
    splitbychar(collId,collId2,collId3,'.');
    i =  check_permit_in_group("curate%%", collId2, user_id);
    strcpy(collId,collId2);
  }
  return(i);
  
}

int
is_system_userid(int userid)
{
  int i;
  char localZoneName[MAX_TOKEN];

  failure = 0;
    strcpy(localZoneName,"");
#ifdef FED_MCAT    
    i = isLocalPrivUser();
    if (i == AUTH_ERR_PROXY_NOPRIV) {
      failure = 0;
      sprintf(query_string, "select zone_id from %s%s where local_zone_flag = 1",
	      MDASSCHEMENAME, "MDAS_TD_ZONE");
      get_ctype_value_from_query(localZoneName,query_string );
      if (failure != 0) 
	return( LOCAL_ZONE_NOT_FOUND);
    }
    else 
      return(i);
#endif
    if (strlen(localZoneName) > 0) {
      sprintf(sqlq, "select user_id from  %sMDAS_CD_USER t1,  %sMDAS_TD_USER_TYP t2 where t1.user_id = %i   and t1.zone_id = '%s' and t1.user_typ_id = t2.user_typ_id AND t2.user_typ_name = 'sysadmin'",
          MDASSCHEMENAME, MDASSCHEMENAME, userid,localZoneName);
    }
    else {
      sprintf(sqlq, "select user_id from  %sMDAS_CD_USER t1,  %sMDAS_TD_USER_TYP t2 where t1.user_id = %i and t1.user_typ_id = t2.user_typ_id AND t2.user_typ_name = 'sysadmin'",
          MDASSCHEMENAME, MDASSCHEMENAME, userid);
    }
    
 if (check_exists(sqlq) != 0) return (SYS_USER_AUTHORIZATION_ERROR);
    return(MDAS_SUCCESS);
}


int  check_unique_name_in_domain(char *obj_type, char *obj_name, char *domain_id)
{
  
    char tab1[MAX_TOKEN], tab2[MAX_TOKEN], tab3[MAX_TOKEN]; 
    

    if (!strcmp(obj_type,"DATA"))
      {
	strcpy(tab1,"MDAS_AD_DOMN");
	strcpy(tab2,"MDAS_AD_REPL");
	strcpy(tab3, "data");
      }
    else if (!strcmp(obj_type,"METHOD"))
      {
	strcpy(tab1,"MDAS_AM_DOMN");
	strcpy(tab2,"MDAS_AM_REPL");
	strcpy(tab3, "meth");
      }
    else if (!strcmp(obj_type,"RESOURCE"))
      {
	strcpy(tab1,"MDAS_AR_DOMN");
	strcpy(tab2,"MDAS_AR_REPL");
	strcpy(tab3, "rsrc");
      }
    else if (!strcmp(obj_type,"USER"))
      {
	strcpy(tab1,"MDAS_AU_DOMN");
	strcpy(tab2,"MDAS_CD_USER");
	strcpy(tab3, "user");

      }
    else return (DATA_VALUE_TYPE_ERROR);


  sprintf(sqlq, "select * from %s%s t2, %s%s t1, %sMDAS_TD_DOMN t3 where t3.domain_desc = '%s' AND  t3.domain_id = t1.domain_id AND t1.%s_id = t2.%s_id AND t2.%s_name = '%s' ",
	  MDASSCHEMENAME, tab2,  MDASSCHEMENAME, tab1,  MDASSCHEMENAME, 
	  domain_id, tab3, tab3,  tab3,  obj_name);

   if (check_exists(sqlq) == MDAS_SUCCESS) 
     return(MDAS_OBJ_NOT_IN_DOMN);
   else 
     return(MDAS_SUCCESS);
  /***** CEOPT
    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i != 0) return (MDAS_FAILURE);
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = get_row_db2(hstmt);
    if (i == 0) 
      {
        i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
	return (MDAS_OBJ_NOT_IN_DOMN);
      }
    i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
    return(MDAS_SUCCESS);
  ****/
}

int check_user_group (char *group_name)
{
  

   sprintf(sqlq, "select t1.user_id from %sMDAS_CD_USER t1, %sMDAS_AU_GROUP t2 where t1.user_name = '%s' AND t1.user_id = t2.group_user_id and not t2.user_id  = t2.group_user_id", MDASSCHEMENAME, MDASSCHEMENAME, group_name);

   if (check_exists(sqlq) == MDAS_SUCCESS) 
      return(MDAS_SUCCESS);
   else 
     return(USER_GROUP_NOT_IN_CAT);
   /**** CEOPT
    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i != 0) return (MDAS_FAILURE);
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = get_row_db2(hstmt);
    if (i != 0) 
      {
        i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
	return (USER_GROUP_NOT_IN_CAT);
      }
    i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
    return(MDAS_SUCCESS);
   */
}

int  check_unique_user_name(char *user_name)
{

  sprintf(sqlq, "select * from %sMDAS_CD_USER t1 where t1.user_name = '%s'",
	  MDASSCHEMENAME, user_name);

   if (check_exists(sqlq) == MDAS_SUCCESS) 
      return(USER_NOT_UNIQUE_IN_CAT);
   else 
     return(MDAS_SUCCESS);
  /**** CEOPT
    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i != 0) return (MDAS_FAILURE);
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = get_row_db2(hstmt);
    if (i == 0)    {
        i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
	return (USER_NOT_UNIQUE_IN_CAT);
      }
    i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
    return(MDAS_SUCCESS);
  ****/
}
int  check_unique_dname_in_group(char *obj_data_name, char * group_id)
{
  

  sprintf(sqlq, "select * from %sMDAS_AD_REPL t1 where t1.data_name = '%s' AND t1.data_grp_id = '%s' and t1.is_deleted = 0",    MDASSCHEMENAME,
	  obj_data_name, group_id);
  tabledIsOkay = 1;
  if (check_exists(sqlq) == MDAS_SUCCESS) {
    tabledIsOkay = 0;
      return(DATA_NOT_UNIQUE_IN_COLLECTION);
  }
  else {
    tabledIsOkay = 0;
    return(MDAS_SUCCESS);
  }
  /**** CEOPT
    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i != 0) return (MDAS_FAILURE);
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = get_row_db2(hstmt);
    if (i == 0) 
      {
        i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
	return (DATA_NOT_UNIQUE_IN_COLLECTION);
      }
    i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
    return(MDAS_SUCCESS);
  ****/
}


int  check_unique_dname_in_respath(char *obj_data_name, char *obj_path_name, 
			 int resource_id)
{
  

  sprintf(sqlq, "select * from %sMDAS_AD_REPL t1 where t1.path_name = '%s' AND t1.rsrc_id = %i and t1.is_deleted = 0",    MDASSCHEMENAME,
	  obj_path_name, resource_id);
  tabledIsOkay = 1;
  if (check_exists(sqlq) == MDAS_SUCCESS) {
    tabledIsOkay = 0;
    return(DATA_NOT_UNIQUE_IN_PATH);
  }
  else {
    tabledIsOkay = 0;
    return(MDAS_SUCCESS);
  }
  /**** CEOPT
    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i != 0) return (MDAS_FAILURE);
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = get_row_db2(hstmt);
    if (i == 0) 
      {
        i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
	return (DATA_NOT_UNIQUE_IN_PATH);
      }
    i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
    return(MDAS_SUCCESS);
  ****/
}

int check_exists(char *sqlqq)
{

    if (inTabledExistsData(sqlq) == MDAS_SUCCESS) return(MDAS_SUCCESS);

    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlqq);
    if (i != 0) return(i);
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0) return(i);
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0) return(i);
    i = get_row_db2(hstmt);
    /*printf("CE1:%i::%s\n",i,sqlq);*/
    if (i == 0) 
      { /* found something to exist returning MDAS_SUCCESS (0)*/
        i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
	addToTabledExistsData(sqlq);
	return(MDAS_SUCCESS);

      }
    i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
    return (-1);
}

int check_domain_access_permits(char *obj_user_name, char *obj_access_name, 
			    char *obj_data_name, char *obj_domain_name,
			    char *collection_name)
{

    /* is data there */
    sprintf(sqlq, "select distinct t1.data_id from  %sMDAS_AD_REPL t1 where t1.data_name = '%s' and t1.is_deleted = 0", 
	    MDASSCHEMENAME,obj_data_name);
    tabledIsOkay = 1;
    if (check_exists(sqlq) != 0) {
      tabledIsOkay = 0;
      return (DATA_NOT_IN_CAT);
    }
      
    /* data in given collection */
    sprintf(sqlq, "select distinct  t1.data_id from  %sMDAS_AD_REPL t1, %sMDAS_TD_DATA_GRP t2 where  t1.data_name = '%s' and t1.is_deleted = 0 and   t1.data_grp_id = t2.data_grp_id and t2.data_grp_name =  '%s'" ,
	    MDASSCHEMENAME,MDASSCHEMENAME,
	    obj_data_name, collection_name);

    if (check_exists(sqlq) != 0) {
      tabledIsOkay = 0;
      return (DATA_NOT_IN_COLLECTION);
    }


    /* is user in given domain */
    sprintf(sqlq, "select distinct  t1.user_id from  %sMDAS_CD_USER t1, %sMDAS_AU_DOMN t2, %sMDAS_TD_DOMN t3 where  t1.user_name = '%s' and   t1.user_id = t2.user_id and t2.domain_id = t3.domain_id  and t3.domain_desc = '%s'" ,
	    MDASSCHEMENAME,MDASSCHEMENAME,MDASSCHEMENAME,
	    obj_user_name, obj_domain_name);

    if (check_exists(sqlq) != 0) {
      tabledIsOkay = 0;
      return (USER_NOT_IN_DOMN);
    }

    /* does user have given access to data */
  
    sprintf(sqlq, "select distinct  t3.access_id from %sMDAS_CD_USER t1 , %sMDAS_AU_GROUP t2, %sMDAS_AD_ACCS t3,  %sMDAS_AD_REPL t4, %sMDAS_TD_DS_ACCS t5 , %sMDAS_TD_DATA_GRP t6 where  t1.user_name = '%s' and t6.data_grp_name =  '%s' and t4.data_name = '%s' and t4.data_grp_id = t6.data_grp_id and  t1.user_id = t2.user_id and t3.user_id = t2.group_user_id and t5.access_constraint like '%%%s%%' and t4.is_deleted = 0 and t4.data_id = t3.data_id and t3.access_id >= t5.access_id ",
	    MDASSCHEMENAME,MDASSCHEMENAME,MDASSCHEMENAME,MDASSCHEMENAME,
	    MDASSCHEMENAME,MDASSCHEMENAME,
	    obj_user_name, collection_name, obj_data_name,  obj_access_name);

    if (check_exists(sqlq) != 0)  {
      tabledIsOkay = 0;
      return (NO_DATA_ACCS_TO_USER);
    }
    tabledIsOkay = 0;
    return (NO_DATA_ACCS_TO_USER_IN_DOMN);
}

int getDatasetInfoFromTicket(int cat_type, char *in_data_name, char *obj_user_name, 
		      char *obj_access_name, char *obj_domain_name,
		    char *in_collection_name,
		   mdasC_sql_result_struct  **myresult,
		   int numrows)
{
    int cnt, ii;
    mdasC_sql_result_struct* result1 = NULL;
    char                access_name[MAX_TOKEN];
    int  obj_data_id,  obj_user_id;
    char                access_list[MAX_TOKEN];
    char *tempstrptr;
    int accsId, dataId, contId;
    char *tmpDataName, *tmpDataName2, *tmpDataName3;
    char sqlq[HUGE_STRING],sqlqq[HUGE_STRING];
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    char qval2[MAX_DCS_NUM][MAX_TOKEN];
    int i , selval[MAX_DCS_NUM];
    char obj_data_name[MAX_TOKEN];
    char like_collection_name[MAX_TOKEN * 2];
    char collection_name[MAX_TOKEN];
    char ticketCollName[MAX_TOKEN],ticketOwnerName[MAX_TOKEN],ticketOwnerDomain[MAX_TOKEN];
    char ticketDataName[MAX_TOKEN];
    char *tCNPtr;
    int cTktRcrsv;
    strcpy(obj_data_name, in_data_name);
    strcpy(collection_name,in_collection_name);
    failure = 0;
#ifdef DEBUGON
    elog(NOTICE,"Entering getDatasetInfoFromTicket:%i\n",numrows);
#endif /* DEBUGON */

    strcpy(reg_registrar_name,obj_user_name);
    strcpy(reg_obj_name, in_data_name);
    strcpy(reg_obj_code,"MDAS_CD_DATA");
    strcpy(reg_action_name, "data inquiry");
    get_time(timeval);
    strcpy(reg_timeval,timeval);


   reg_obj_id =  obj_data_id;
   reg_obj_repl_enum = 0;
   for (i = 0; i < MAX_DCS_NUM; i++)
     {
       sprintf(qval[i],"");
       selval[i] = 0;
       sprintf(qval2[i],"");
     }
   i = set_qval_data_cond(in_data_name, qval);
   if (i != 0) return(i);
   
   if (strlen(qval[TICKET_C]) > 0)  {
     
     strcpy(qval2[TICKET_C],qval[TICKET_C]);
     sprintf(qval2[TICKET_USER_C], " in ( '%s', '%s')", obj_user_name, TICKET_USER);
     sprintf(qval2[TICKET_USER_DOMAIN_C], " in ( '%s', '%s')",TICKET_USER_DOMAIN,obj_domain_name);
     sprintf(qval2[TICKET_ACC_COUNT_C]," <> 0");
     sprintf(qval2[TICKET_BEGIN_TIME_C] , " < '%s'", timeval);
     sprintf(qval2[TICKET_END_TIME_C] , " > '%s'", timeval);
     selval[DATA_GRP_NAME] = 1;
     selval[TICKET_OWNER_DOMAIN_C] = 1;
     selval[TICKET_OWNER_C] = 1;
     selval[C_TICKET_RECURSIVE] = 1;
     i =  get_data_dir_info(MDAS_CATALOG, qval2, selval, &result1,
			    dcs_tname, dcs_aname, 10);
     if (i != 0)
       return (INVALID_TICKET);
     tCNPtr = (char *)getFromResultStruct(
					  (mdasC_sql_result_struct *) result1,
					  dcs_tname[DATA_GRP_NAME],dcs_aname[DATA_GRP_NAME]);
     if (tCNPtr == NULL)
       return(INVALID_TICKET);
     make_quote_to_two_quote(tCNPtr,ticketCollName);
     tCNPtr = (char *)getFromResultStruct(
					  (mdasC_sql_result_struct *) result1,
					  dcs_tname[TICKET_OWNER_DOMAIN_C],dcs_aname[TICKET_OWNER_DOMAIN_C]);
     if (tCNPtr == NULL)
       return(INVALID_TICKET);
     strcpy(ticketOwnerDomain,tCNPtr);
     tCNPtr = (char *)getFromResultStruct(
					  (mdasC_sql_result_struct *) result1,
					  dcs_tname[TICKET_OWNER_C],dcs_aname[TICKET_OWNER_C]);
     if (tCNPtr == NULL)
       return(INVALID_TICKET);
     strcpy(ticketOwnerName,tCNPtr);
     tCNPtr = (char *)getFromResultStruct(
					  (mdasC_sql_result_struct *) result1,
					  dcs_tname[C_TICKET_RECURSIVE],dcs_aname[C_TICKET_RECURSIVE]);
     if (tCNPtr == NULL)
       return(INVALID_TICKET);
     cTktRcrsv = atoi(tCNPtr);
     freeSqlResult ( result1);

     if (!cTktRcrsv) {
       if (strcmp(collection_name,ticketCollName))
	 return(NO_ACCS_TO_USER_IN_COLLECTION);
     }
     else {
       if (strstr(collection_name,ticketCollName) !=  collection_name)
	 return(NO_ACCS_TO_USER_IN_COLLECTION);
     }
     tmpDataName = in_data_name;
     tmpDataName3 = NULL;
     if (strstr(tmpDataName,"&") != NULL) {
       tmpDataName3 = strdup(qval[DATA_NAME]);
       tmpDataName2 = strstr(tmpDataName3,"'");
       if (tmpDataName2 != NULL) {
	 tmpDataName = tmpDataName2;
	 tmpDataName++;
	 tmpDataName2 = strstr(tmpDataName,"'");
	 if (tmpDataName2 != NULL)
	   *tmpDataName2 = '\0';
       }
     }
     strcpy(ticketDataName,tmpDataName);
     if (tmpDataName3 != NULL)
       free(tmpDataName3);
   }
   else {
     strcpy(qval2[TICKET_D],qval[TICKET_D]);
     sprintf(qval2[TICKET_USER_D], " in ( '%s', '%s')", obj_user_name, TICKET_USER);
     sprintf(qval2[TICKET_USER_DOMAIN_D], " in ( '%s', '%s')",TICKET_USER_DOMAIN,obj_domain_name);
     sprintf(qval2[TICKET_ACC_COUNT_D]," <> 0");
     sprintf(qval2[TICKET_BEGIN_TIME_D] , " < '%s'", timeval);
     sprintf(qval2[TICKET_END_TIME_D] , " > '%s'", timeval);
     selval[TICKET_OWNER_DOMAIN_D] = 1;
     selval[TICKET_OWNER_D] = 1;
     selval[DATA_GRP_NAME] = 1;
     selval[DATA_NAME] = 1;
     i =  get_data_dir_info(MDAS_CATALOG, qval2, selval, &result1,
                         dcs_tname, dcs_aname, 10);
     if (i != 0)
       return (INVALID_TICKET);
     tCNPtr = (char *)getFromResultStruct(
					  (mdasC_sql_result_struct *) result1,
					  dcs_tname[DATA_GRP_NAME],dcs_aname[DATA_GRP_NAME]);
     if (tCNPtr == NULL)
       return(INVALID_TICKET);
     make_quote_to_two_quote(tCNPtr,ticketCollName);
     tCNPtr = (char *)getFromResultStruct(
                                          (mdasC_sql_result_struct *) result1,
                                          dcs_tname[DATA_NAME],dcs_aname[DATA_NAME]);
     if (tCNPtr == NULL)
       return(INVALID_TICKET);
     make_quote_to_two_quote(tCNPtr,ticketDataName);
     tCNPtr = (char *)getFromResultStruct(
                                          (mdasC_sql_result_struct *) result1,
                                          dcs_tname[TICKET_OWNER_DOMAIN_D],dcs_aname[TICKET_OWNER_DOMAIN_D]);
     if (tCNPtr == NULL)
       return(INVALID_TICKET);
     strcpy(ticketOwnerDomain,tCNPtr);
     tCNPtr = (char *)getFromResultStruct(
                                          (mdasC_sql_result_struct *) result1,
                                          dcs_tname[TICKET_OWNER_D],dcs_aname[TICKET_OWNER_D]);
     if (tCNPtr == NULL)
       return(INVALID_TICKET);
     strcpy(ticketOwnerName,tCNPtr);
     freeSqlResult ( result1);
     if (strlen(collection_name) != 0 ) {
       sprintf(like_collection_name,"'%s'",ticketDataName);
       if (strcmp(collection_name,ticketCollName) || strstr(qval[DATA_NAME],like_collection_name) == NULL)
	 return(NO_ACCS_TO_USER_IN_COLLECTION);
     }
     else
       strcpy(collection_name,ticketCollName);
   }

   for (i = 0; i < MAX_DCS_NUM; i++) {
     selval[i] = 0;
   }

   reg_registrar_id = get_user_id_in_domain(ticketOwnerName,ticketOwnerDomain);
   if (reg_registrar_id < 0) 
     return(reg_registrar_id);
   accsId = getAccessId("all");
   if (accsId < 0)
     return(accsId);
   if ((i = getDataIdContIdByName(ticketDataName,collection_name, &dataId, &contId))
	   != MDAS_SUCCESS) {
     return(i);
   }
   if (dataId == contId)
     sprintf(qval[CONTAINER_REPL_ENUM]," = %s%s",
	     MDASSCHEMENAME,"MDAS_ADC_REPL.repl_enum");
   sprintf(qval[DATA_ACCESS_ID]," >= %i",accsId);
   sprintf(qval[USER_ID]," = %i",reg_registrar_id);
   sprintf(qval[DATA_NAME],""); 
   sprintf(qval[TICKET_C],"");
   sprintf(qval[TICKET_D],"");
   sprintf(qval[DATA_ID]," = %i",dataId);
   sprintf(qval[DATA_HIDE], " = 0 ");
   sprintf(qval[DATA_LOCK_NUM], " <= %i",getAccessId("read audit"));
   if (strlen(qval[DATA_VER_NUM]) == 0)
     sprintf(qval[DATA_VER_NUM], " = '0'");
   selval[DATA_TYP_NAME] = 1;
   selval[DATA_REPL_ENUM] = 1;
   selval[DATA_NAME] = 1;
   selval[PATH_NAME] = 1;
   selval[PHY_RSRC_NAME] = 1;
   selval[RSRC_ADDR_NETPREFIX] = 1;
   selval[PHY_RSRC_TYP_NAME] = 1;
   selval[IS_DIRTY] = 1;
   selval[RSRC_CLASS] = 1;
   selval[OFFSET] = 1;
   selval[CONTAINER_NAME] = 1;
   selval[SIZE] = 1;
   selval[DATA_CHECKSUM] = 1;
   i = get_data_dir_info(MDAS_CATALOG, qval, selval, myresult,
			 dcs_tname, dcs_aname, numrows);
   elog(NOTICE, "Ticket SQL: %s",GENERATEDSQL);
   if (i != 0)
     return (i);

   /* incrementing counter */
   if (strlen(qval2[TICKET_C]) > 0)
     {
       sprintf(query_string, "update %sMDAS_AU_TCKT_GRP set access_count = access_count - 1 where ticket_id %s", MDASSCHEMENAME,qval2[TICKET_C]);
       i = ExecSqlDb2(henv, hdbc, query_string);
       if (i != 0) 
	 {
	   return(MDAS_AU_TCKT_GRP_UPDATE_ERROR);
	 }
     }
   else if (strlen(qval2[TICKET_D]) > 0)
     {
       sprintf(query_string, "update %sMDAS_AU_TCKT_DATA set access_count = access_count - 1 where ticket_id %s ", MDASSCHEMENAME,qval2[TICKET_D]);
       i = ExecSqlDb2(henv, hdbc, query_string);
       if (i != 0) 
	 {
	   return(MDAS_AU_TCKT_DATA_UPDATE_ERROR);
	 }
     }
   return (MDAS_SUCCESS);
}

	   
int getDatasetInfo(int cat_type, char *my_data_name, char *obj_user_name, 
		      char *obj_access_name, char *obj_domain_name,
		    char *my_collection_name,
		   mdasC_sql_result_struct  **myresult,
		   int numrows)

{
    int cnt;
    mdasC_sql_result_struct* result1 = NULL;
    char                access_name[MAX_TOKEN];
    int  obj_data_id,  obj_user_id;
    char                access_list[MAX_TOKEN];
    char *tempstrptr;
    char qval[MAX_DCS_NUM][MAX_TOKEN], tmpStr[MAX_TOKEN * 2];
    int i , selval[MAX_DCS_NUM];
    char *tmpDataName, *tmpDataName2, *tmpDataName3;
    char obj_data_name[MAX_TOKEN];
    int accsId, dataId, contId;
    int trashFlag = 0;
    char in_data_name[MAX_TOKEN + 20];
    char collection_name[MAX_TOKEN + 20];
    char *t1;

    t1 = NULL;
    if ( ( t1 =  strstr(my_data_name,"&")) != NULL)
      *t1 = '\0';
    make_quote_to_two_quote(my_data_name,in_data_name);
    if (t1 != NULL) {
      *t1 = '&';
      strcat(in_data_name,t1);
    }
    make_quote_to_two_quote(my_collection_name,collection_name);

    if (strstr(in_data_name, "NONCONTCOLLINFO") == in_data_name) {
	return (getNonContCollectionInfo(cat_type,
					 (char *) in_data_name+15, 
				     obj_user_name, obj_access_name, 
				     obj_domain_name, collection_name,
				     myresult, numrows));
    }

    if (strstr(in_data_name, "&TICKET ") != NULL ||
	strstr(in_data_name, "&TICKET=") != NULL || 
	strstr(in_data_name, " TICKET ") != NULL || 
	strstr(in_data_name, " TICKET=") != NULL)
      {
	return (getDatasetInfoFromTicket(cat_type,in_data_name, 
				     obj_user_name, obj_access_name, 
				     obj_domain_name, collection_name,
				     myresult, numrows));
	
      }
    if (strlen(in_data_name) == 0 || in_data_name[0] == '&') {
      return(getDatasetInfoByMetadata(cat_type,in_data_name, 
				     obj_user_name, obj_access_name, 
				     obj_domain_name, collection_name,
				     myresult, numrows));
    }

#ifdef LOGTIMINGMCAT
    sprintf(tmpStr,"getDatasetInfo: Enter %s/%s ",collection_name,in_data_name);
    logmess_db2(tmpStr);
#endif /* LOGTIMINGMCAT */
    
    i = getTabledGetDatasetInfo(cat_type,in_data_name, 
				obj_user_name, obj_access_name, 
				obj_domain_name,collection_name,
				myresult, numrows);
    if (i == MDAS_SUCCESS) {
#ifdef LOGTIMINGMCAT
    logmess_db2("getDatasetInfo: Exit");
#endif /* LOGTIMINGMCAT */      
      return (i);
    }
#ifdef LOGTIMINGMCAT
    logmess_db2("getDatasetInfo: After getTabledGetDatasetInfo");
#endif /* LOGTIMINGMCAT */

   strcpy(obj_data_name, in_data_name);
   failure = 0;
   strcpy(reg_registrar_name,obj_user_name);
   strcpy(reg_obj_name, in_data_name);
   strcpy(reg_obj_code,"MDAS_CD_DATA");
   strcpy(reg_action_name, "data inquiry");
    
   /**** turned off because of some authorization problem
   if (curUserId != -1) 
     reg_registrar_id = curUserId;
   else 
   ****/
     reg_registrar_id = get_user_id_in_domain(obj_user_name,obj_domain_name);

    for (i = 0; i < MAX_DCS_NUM; i++)
      {
	sprintf(qval[i],"");
	selval[i] = 0;
      }
    if (!strcmp(obj_access_name,"trash")) {
      /* condition added RAJA: Jan 9, 2004 */
      i =  checkTrashSuperUserAccess(obj_user_name,obj_domain_name,
				     collection_name );
      if (i != 0)
	return (ACTION_NOT_ALLOWED);
      trashFlag = 1;
    }
    else {
      accsId = getAccessId(obj_access_name);
      if (accsId < 0) {
#ifdef LOGTIMINGMCAT
	logmess_db2("getDatasetInfo: Exit with error");
#endif /* LOGTIMINGMCAT */
	return(accsId);
      }
    }
    
    i = set_qval_data_cond(in_data_name, qval);
    if (i != 0) {
#ifdef LOGTIMINGMCAT
    logmess_db2("getDatasetInfo: Exit with error");
#endif /* LOGTIMINGMCAT */
      return(i);
    }
    /*** added to get container-datasets MArch 2000***/
    tmpDataName = in_data_name;
    tmpDataName3 = NULL;
    if (strstr(tmpDataName,"&") != NULL) {
      tmpDataName3 = strdup(qval[DATA_NAME]);
      tmpDataName2 = strstr(tmpDataName3,"'");
      if (tmpDataName2 != NULL) {
	tmpDataName = tmpDataName2;
	tmpDataName++;
	tmpDataName2 = strstr(tmpDataName,"'");
	if (tmpDataName2 != NULL)
	  *tmpDataName2 = '\0';
      }
    }
    /**if (isContainerByName(tmpDataName,collection_name) 
       == MDAS_ONE) { **/
    /****if ( inContainerObjectByName(tmpDataName,collection_name)
	 == MDAS_ZERO) {
      sprintf(qval[CONTAINER_REPL_ENUM]," = %s%s",
	      MDASSCHEMENAME,"MDAS_ADC_REPL.repl_enum");
    }
    *** removed Aug 24,2000 and replaced as below for speedup purposes ***/

    if ((i = getDataIdContIdByName(tmpDataName,collection_name, &dataId, &contId)) 
	!= MDAS_SUCCESS) {
#ifdef LOGTIMINGMCAT
      logmess_db2("getDatasetInfo: Exit with error");
#endif /* LOGTIMINGMCAT */
      return(i);
    }
    if (dataId == contId) 
      sprintf(qval[CONTAINER_REPL_ENUM]," = %s%s",
	      MDASSCHEMENAME,"MDAS_ADC_REPL.repl_enum");


    /*** added to get container-datasets MArch 2000***/
    /* done Aug 24,2000 for speedup 
    if (collection_name != NULL && strcmp( collection_name,""))
      sprintf(qval[DATA_GRP_NAME]," = '%s'",
	      collection_name);
    */

    if (trashFlag == 0) {/* condition added RAJA: Jan 9, 2004 */
      sprintf(qval[DATA_ACCESS_ID]," >= %i",accsId);
      sprintf(qval[USER_ID]," = %i",reg_registrar_id);
    }
    sprintf(qval[DATA_NAME],""); /* done Aug 24,2000 for speedup */
    sprintf(qval[DATA_ID]," = %i",dataId);/* done Aug 24,2000 for speedup */

    /* RAJA added January 24, 2003 for checking for locked and hidden files ***/
    sprintf(qval[DATA_HIDE], " = 0 ");
    sprintf(qval[DATA_LOCK_NUM], " <= %i",accsId);
    if (strlen(qval[DATA_VER_NUM]) == 0)
      sprintf(qval[DATA_VER_NUM], " = '0'");

    selval[DATA_TYP_NAME] = 1;
    selval[DATA_REPL_ENUM] = 1;
    selval[DATA_NAME] = 1;
    selval[PATH_NAME] = 1;
    selval[PHY_RSRC_NAME] = 1;
    selval[RSRC_ADDR_NETPREFIX] = 1;
    selval[PHY_RSRC_TYP_NAME] = 1;
    selval[IS_DIRTY] = 1;
    selval[RSRC_CLASS] = 1;
    selval[OFFSET] = 1;
    selval[CONTAINER_NAME] = 1;
    selval[SIZE] = 1;
    selval[DATA_CHECKSUM] = 1;
#ifdef LOGTIMINGMCAT
    logmess_db2("getDatasetInfo: Before Calling get_data_dir_info");
#endif /* LOGTIMINGMCAT */
    if (trashFlag == 0 &&  strstr(in_data_name,"&") == NULL) /* condition added RAJA: Jan 9, 2004  April 9, 2004*/
      getDatasetQuery = 1;  /* Added Aug 24,2000 for speed up */
    
    i = get_data_dir_info(MDAS_CATALOG, qval, selval, myresult,
			  dcs_tname, dcs_aname, numrows);
    getDatasetQuery = 0; /* Added Aug 24,2000 for speed up */
#ifdef LOGTIMINGMCAT
    sprintf(tmpStr,"getDatasetInfo: After return from get_data_dir_info: %i",i);
    logmess_db2(tmpStr);
#endif /* LOGTIMINGMCAT */
    if (i != 0) /*myresult == NULL) */
      {		
	i = check_domain_access_permits(obj_user_name, 
					obj_access_name, tmpDataName, obj_domain_name,
					collection_name);
	if (tmpDataName3 != NULL) free(tmpDataName3);
#ifdef LOGTIMINGMCAT
    logmess_db2("getDatasetInfo: Exit with error");
#endif /* LOGTIMINGMCAT */
	return(i);
      }
    
#ifdef LOGTIMINGMCAT
    logmess_db2("getDatasetInfo: Before calling addToTabledGetDatasetInfo");
#endif /* LOGTIMINGMCAT */
    addToTabledGetDatasetInfo(cat_type,in_data_name, 
			      obj_user_name, obj_access_name, 
			      obj_domain_name,collection_name,
			      *myresult, numrows);
    performAuditDataAccess(obj_access_name,reg_registrar_id,dataId,"","");
#ifdef LOGTIMINGMCAT
    logmess_db2("getDatasetInfo: Exit");
#endif /* LOGTIMINGMCAT */
	return (i);
}

int getDatasetInfoByMetadata(int cat_type, char *my_data_name, char *obj_user_name, 
		      char *obj_access_name, char *obj_domain_name,
		    char *my_collection_name,
		   mdasC_sql_result_struct  **myresult,
		   int numrows)

{
    int cnt;
    mdasC_sql_result_struct* result1 = NULL;
    char                access_name[MAX_TOKEN];
    int  obj_data_id,  obj_user_id;
    char                access_list[MAX_TOKEN];
    char *tempstrptr;
    char qval[MAX_DCS_NUM][MAX_TOKEN], tmpStr[MAX_TOKEN * 2];
    int i , selval[MAX_DCS_NUM];
    char *tmpDataName, *tmpDataName2, *tmpDataName3;
    char obj_data_name[MAX_TOKEN];
    int accsId, dataId, contId;
    int trashFlag = 0;
    char in_data_name[MAX_TOKEN + 20];
    char collection_name[MAX_TOKEN + 20];

    make_quote_to_two_quote(my_data_name,in_data_name);
    make_quote_to_two_quote(my_collection_name,collection_name);
	

#ifdef LOGTIMINGMCAT
    sprintf(tmpStr,"getDatasetInfoByMetadata: Enter %s/%s ",collection_name,in_data_name);
    logmess_db2(tmpStr);
#endif /* LOGTIMINGMCAT */
    
   strcpy(obj_data_name, in_data_name);
   failure = 0;
   strcpy(reg_registrar_name,obj_user_name);
   strcpy(reg_obj_name, in_data_name);
   strcpy(reg_obj_code,"MDAS_CD_DATA");
   strcpy(reg_action_name, "data inquiry");
    
   reg_registrar_id = get_user_id_in_domain(obj_user_name,obj_domain_name);

    for (i = 0; i < MAX_DCS_NUM; i++)
      {
	sprintf(qval[i],"");
	selval[i] = 0;
      }
    accsId = getAccessId(obj_access_name);
    if (accsId < 0) {
#ifdef LOGTIMINGMCAT
      logmess_db2("getDatasetInfoByMetadata: Exit with error");
#endif /* LOGTIMINGMCAT */
      return(accsId);
    }      
    i = set_qval_data_cond(in_data_name, qval);
    if (i != 0) {
#ifdef LOGTIMINGMCAT
    logmess_db2("getDatasetInfoByMetadata: Exit with error");
#endif /* LOGTIMINGMCAT */
      return(i);
    }
    if (collection_name != NULL && strcmp( collection_name,""))
      sprintf(qval[DATA_GRP_NAME]," = '%s'",   collection_name);
    sprintf(qval[DATA_ACCESS_ID]," >= %i",accsId);
    sprintf(qval[USER_ID]," = %i",reg_registrar_id);
    sprintf(qval[DATA_HIDE], " = 0 ");
    sprintf(qval[DATA_LOCK_NUM], " <= %i",accsId);
    if (strlen(qval[DATA_VER_NUM]) == 0)
      sprintf(qval[DATA_VER_NUM], " = '0'");

    selval[DATA_TYP_NAME] = 1;
    selval[DATA_REPL_ENUM] = 1;
    selval[DATA_NAME] = 1;
    selval[PATH_NAME] = 1;
    selval[PHY_RSRC_NAME] = 1;
    selval[RSRC_ADDR_NETPREFIX] = 1;
    selval[PHY_RSRC_TYP_NAME] = 1;
    selval[IS_DIRTY] = 1;
    selval[RSRC_CLASS] = 1;
    selval[OFFSET] = 1;
    selval[CONTAINER_NAME] = 1;
    selval[SIZE] = 1;
    selval[DATA_CHECKSUM] = 1;
#ifdef LOGTIMINGMCAT
    logmess_db2("getDatasetInfoByMetadata: Before Calling get_data_dir_info");
#endif /* LOGTIMINGMCAT */
    
    i = get_data_dir_info(MDAS_CATALOG, qval, selval, myresult,
			  dcs_tname, dcs_aname, numrows);
    getDatasetQuery = 0; /* Added Aug 24,2000 for speed up */
#ifdef LOGTIMINGMCAT
    sprintf(tmpStr,"getDatasetInfoByMetadata: After return from get_data_dir_info: %i",i);
    logmess_db2(tmpStr);
#endif /* LOGTIMINGMCAT */

    /*** logging of this is not done for now ****/    
#ifdef LOGTIMINGMCAT
    logmess_db2("getDatasetInfoByMetadata: Exit");
#endif /* LOGTIMINGMCAT */
    return (i);
}


int getNonContCollectionInfo(int cat_type, char *in_data_name, 
			     char *obj_user_name, 
			     char *obj_access_name, char *obj_domain_name,
			     char *collection_name,
			     mdasC_sql_result_struct  **myresult,
			     int numrows)

{
    int cnt;
    mdasC_sql_result_struct* result1 = NULL;
    char                access_name[MAX_TOKEN];
    int  obj_data_id,  obj_user_id;
    char                access_list[MAX_TOKEN];
    char *tempstrptr;
    char qval[MAX_DCS_NUM][MAX_TOKEN], tmpStr[MAX_TOKEN * 2];
    int i , selval[MAX_DCS_NUM];
    char *tmpDataName, *tmpDataName2, *tmpDataName3;
    char obj_data_name[MAX_TOKEN];
    int accsId, dataId, contId;

#ifdef LOGTIMINGMCAT
    sprintf(tmpStr,"getNonContCollectionInfo: Enter %s/%s ",collection_name,in_data_name);
    logmess_db2(tmpStr);
#endif /* LOGTIMINGMCAT */
    
   strcpy(obj_data_name, in_data_name);
   failure = 0;
   strcpy(reg_registrar_name,obj_user_name);
   strcpy(reg_obj_name, in_data_name);
   strcpy(reg_obj_code,"MDAS_CD_DATA");
   strcpy(reg_action_name, "data inquiry");

     reg_registrar_id = get_user_id_in_domain(obj_user_name,obj_domain_name);

    for (i = 0; i < MAX_DCS_NUM; i++)
      {
	sprintf(qval[i],"");
	selval[i] = 0;
      }
       

    accsId = getAccessId(obj_access_name);
    if (accsId < 0) {
#ifdef LOGTIMINGMCAT
      logmess_db2("getNonContCollectionInfo: Exit with error");
#endif /* LOGTIMINGMCAT */
      return(accsId);
    }
    if (strlen(in_data_name) > 0) {
      i = set_qval_data_cond(in_data_name, qval);
      if (i != 0) {
#ifdef LOGTIMINGMCAT
	logmess_db2("getNonContCollectionInfo: Exit with error");
#endif /* LOGTIMINGMCAT */
	return(i);
      }
    }
    /*** added to get container-datasets MArch 2000***/
    tmpDataName = in_data_name;
    tmpDataName3 = NULL;
    if (strstr(tmpDataName,"&") != NULL) {
      tmpDataName3 = strdup(qval[DATA_NAME]);
      tmpDataName2 = strstr(tmpDataName3,"'");
      if (tmpDataName2 != NULL) {
	tmpDataName = tmpDataName2;
	tmpDataName++;
	tmpDataName2 = strstr(tmpDataName,"'");
	if (tmpDataName2 != NULL)
	  *tmpDataName2 = '\0';
      }
    }

    
    sprintf(qval[DATA_ACCESS_ID]," >= %i",accsId);
    sprintf(qval[USER_ID]," = %i",reg_registrar_id);
    sprintf(qval[DATA_HIDE], " = 0 ");
    sprintf(qval[DATA_LOCK_NUM], " <= %i",accsId);
    if (strlen(qval[DATA_VER_NUM]) == 0)
      sprintf(qval[DATA_VER_NUM], " = '0'");
    /* sprintf(qval[DATA_GRP_NAME]," like '%s' || like '%s%%%%' ", */
    sprintf(qval[DATA_GRP_NAME]," like '%s' ESCAPE '\\' || like '%s/%%' ESCAPE '\\'",

	    collection_name,collection_name);
    sprintf(qval[CONTAINER_NAME]," is NULL || ='' ");
    selval[DATA_TYP_NAME] = 1;
    selval[DATA_REPL_ENUM] = 1;
    selval[DATA_NAME] = 1;
    selval[PATH_NAME] = 1;
    selval[PHY_RSRC_NAME] = 1;
    selval[RSRC_ADDR_NETPREFIX] = 1;
    selval[PHY_RSRC_TYP_NAME] = 1;
    selval[IS_DIRTY] = 1;
    selval[RSRC_CLASS] = 1;
    selval[OFFSET] = 1;
    selval[CONTAINER_NAME] = 1;
    selval[SIZE] = 1;
    selval[DATA_GRP_NAME] = 1;
    selval[DATA_CHECKSUM] = 1;
    
#ifdef LOGTIMINGMCAT
    logmess_db2("getNonContCollectionInfo: Before Calling get_data_dir_info");
#endif /* LOGTIMINGMCAT */

    i = get_data_dir_info(MDAS_CATALOG, qval, selval, myresult,
			  dcs_tname, dcs_aname, numrows);
#ifdef LOGTIMINGMCAT
    sprintf(tmpStr,"getNonContCollectionInfo: After return from get_data_dir_info: %i",i);
    logmess_db2(tmpStr);
#endif /* LOGTIMINGMCAT */
    if (i != 0) /*myresult == NULL) */
      {		
	i = check_domain_access_permits(obj_user_name, 
					obj_access_name, tmpDataName, obj_domain_name,
					collection_name);
	if (tmpDataName3 != NULL) free(tmpDataName3);
#ifdef LOGTIMINGMCAT
    logmess_db2("getNonContCollectionInfo: Exit with error");
#endif /* LOGTIMINGMCAT */
	return(i);
      }
    /***    perform better audit ...
	    performAuditDataAccess(obj_access_name,reg_registrar_id,dataId,"","");   */
#ifdef LOGTIMINGMCAT
    logmess_db2("getNonContCollectionInfo: Exit");
#endif /* LOGTIMINGMCAT */
	return (i);
}

void reset_flag_for_distinct(char qval[][MAX_TOKEN],int selval[])
{
  if (selval[PHY_RSRC_DEFAULT_PATH] > 0 ||
      selval[RSRC_DEFAULT_PATH] > 0 ||
      selval[INTERNAL_STRUCTURED_METADATA] > 0 ||
      selval[RESOURCE_DESCRIPTION ] > 0 ||
      selval[NONDISTINCT] > 0 ||
      (selval[DATA_ANNOTATION] > 0 && selval[DATA_ANNOTATION] != 14) )
    distinct_flag = 0;
  selval[NONDISTINCT] = 0;
}

int
get_more_rows(int cat_type, int cont_index, mdasC_sql_result_struct **myresult,
	     int numrows)
{
  mdasC_sql_result_struct* result1;
  int i;

    if (numrows == 0)
      {
	if (queryCont[cont_index].hstmt == 0 || queryCont[cont_index].id == 0) {
	  queryCont[cont_index].id = 0;
	  return(0);
	}
	i = done_with_result_db2(queryCont[cont_index].hstmt);
	queryCont[cont_index].id = 0;
	return(0);
      }


   failure = 0;
  result1 = (mdasC_sql_result_struct *) 
    malloc(sizeof(mdasC_sql_result_struct));
  if (result1 == NULL) 
    {i = done_with_result_db2(queryCont[cont_index].hstmt);
     return(MEMORY_ALLOCATION_ERROR);
    }
  /* scrub memory - KESTOCK */
  bzero(result1, sizeof(mdasC_sql_result_struct));
  result1->result_count = 0;

  mdasC_inquire_more(  cont_index,
		(mdasC_infoh*)  result1,  status, numrows);
  if (status[2] != MDAS_SUCCESS)
    { 
      i = done_with_result_db2(queryCont[cont_index].hstmt);
      queryCont[cont_index].id = 0;
      /* spiining to free  - KESTOCK */
      for (i = 0; i < result1->result_count ; i++) {
	if (result1->sqlresult[i].values != NULL)
	  free (result1->sqlresult[i].values);
      }
      free(result1);
      *myresult = NULL;
      return(MCAT_INQUIRE_ERROR);
    }
  rmGlbFromResult (result1);
  *myresult = result1;
  return (MDAS_SUCCESS);
}



int get_data_dir_info(int cat_type, char qval[][MAX_TOKEN],
		  int selval[], 
                       mdasC_sql_result_struct **myresult,
		      char tname[][MAX_TOKEN],
		      char aname[][MAX_TOKEN],
		      int numrows)
 
{
 
        mdasC_sql_query_struct* info1;
        mdasC_sql_result_struct* result1;
 

        char *in_dti;
	int i;
 
   failure = 0;

   /*** ADDED MARCH 10, 2005 RAJA TO CHANGE THE EXTENSIBLE MCAT PART IN DCS  ***/
   if (selval[EXTENSIBLE_SCHEMA_NAME] == 1 &&
       selval[EXTENSIBLE_TABLE_NAME] == 1 &&
       selval[EXTENSIBLE_ATTR_NAME] == 1 &&
       selval[EXTENSIBLE_ATTR_OUTSIDE_NAME] == 1 ) {
     if (selval[IX_COLL_NAME_FOR_DATASET] == 1)
       selval[IX_COLL_NAME_FOR_DATASET] = 0;
     else
       reload_ext_tab_attr_index(cat_type,qval[EXTENSIBLE_SCHEMA_NAME]);
   }
   /*** ADDED MARCH 10, 2005 RAJA TO CHANGE THE EXTENSIBLE MCAT PART IN DCS  ***/

        info1 = (mdasC_sql_query_struct *) malloc(sizeof(mdasC_sql_query_struct));
	if (info1 == NULL) return(MEMORY_ALLOCATION_ERROR);
        result1 = (mdasC_sql_result_struct *) malloc(sizeof(mdasC_sql_result_struct));
 	if (result1 == NULL) {
	  free(info1);
	  return(MEMORY_ALLOCATION_ERROR);
	}
	/* scrub memory - KESTOCK */
	bzero(result1, sizeof(mdasC_sql_result_struct));
	result1->result_count = 0;

#ifdef LOGTIMINGMCAT
    logmess_db2("get_data_dir_info: Before Calling set_info_srb_call");
#endif /* LOGTIMINGMCAT */
	reset_flag_for_distinct(qval,selval);
        i = set_info_srb_call(info1, result1, qval,selval, 
			      tname, aname, numrows);
        if (i != 0) {
	  free(info1);
	  free(result1);
	  return(i);
	}

#ifdef LOGTIMINGMCAT
    logmess_db2("get_data_dir_info: Before Calling mdasC_inquire");
#endif /* LOGTIMINGMCAT */
        mdasC_inquire(&serverhndl[0], (mdasC_infoh*) info1 ,
               (mdasC_infoh*)  result1,  status, numrows);

        if (status[2] != MDAS_SUCCESS)
          { 

#ifdef DEBUGON
	    elog(NOTICE, "Error in Inquire Status:%i\n",status[2]);
	    elog(NOTICE, "Failed SQL:%s\n",GENERATEDSQL);
#endif /* DEBUGON */ 
	    free (info1);
      /* spiining to free  - KESTOCK */
	    for (i = 0; i < result1->result_count ; i++) {
	      if (result1->sqlresult[i].values != NULL)
		free (result1->sqlresult[i].values);
	    }
	    free(result1);
            *myresult = NULL;
	     distinct_flag = 1;
            return(MCAT_INQUIRE_ERROR);
          }
	distinct_flag = 1;
	rmGlbFromResult (result1);
	*myresult = result1;
	free(info1);
	return (MDAS_SUCCESS);
}


int getResOnChoice(char *logRes, char *phyRes, char *choice)
{
  int i,j,k, rowcount ;
  int    rnum[MAX_ITYPE_LIST_COUNT], phrnum[MAX_ITYPE_LIST_COUNT];
  char cval[MAX_TABLE_COLS][MAX_TOKEN];

  sprintf(sqlq, "select rsrc_id, phy_rsrc_id from %sMDAS_AR_REPL where rsrc_name = '%s'",
		 MDASSCHEMENAME, logRes);
  i = get_2itype_list_from_query(rnum,phrnum,&rowcount,sqlq);
  if (i != 0 || rowcount == 0) {
    strcpy(GENERATEDSQL,sqlq);
    return (RESOURCE_NOT_IN_CAT);
  }
  if (!strcmp(choice,"RR")) { /* Round Robin */
    j = roundRobinLogRsrc(rnum[0],phrnum,rowcount);
  }
  else if (!strcmp(choice,"RANDOM")) { /* Random */
    k =  random() % rowcount;
    j = phrnum[k];
  }
  else { /* any */
    j = phrnum[0];
  }
    sprintf(sqlq, "select phy_rsrc_name from %sMDAS_AR_PHYSICAL where phy_rsrc_id = %i",
	    MDASSCHEMENAME, j);
    rowcount = 1;
    i = get_row_from_query(cval,&rowcount,sqlq);
    if (i < 0) {
    strcpy(GENERATEDSQL,sqlq);
    return(RESOURCE_NOT_IN_CAT);
    }
    strcpy(phyRes, cval[0]);
    return(MDAS_SUCCESS);
}



int resAttrLookup(int catType, char *resourceName,
		       char *domainName, char **resourceType,
		       char **resourceLoc,
		       char **resourceClass, int **resourceMaxSize,
		      int numrows, int *rowcount)
{
  char qval[MAX_DCS_NUM][MAX_TOKEN];
  int i , selval[MAX_DCS_NUM];
  mdasC_sql_result_struct  *myresult;
  char cval1[MAX_TOKEN], cval2[MAX_TOKEN];
  char *rsrcMaxSize;

  for (i = 0; i < MAX_DCS_NUM; i++)
         {
           sprintf(qval[i],"");
           selval[i] = 0;
         }   
        sprintf(qval[RSRC_NAME]," = '%s'",resourceName);
	selval[12] = 1;
	selval[13] = 1;
	selval[RSRC_CLASS] = 1;
	selval[MAX_OBJ_SIZE] = 1;

   failure = 0;
  i = get_data_dir_info(catType, qval, selval, &myresult, 
			dcs_tname, dcs_aname, numrows);
  if (myresult == NULL) 
    {
    sprintf(sqlq, 
       "select t1.rsrc_name from %sMDAS_AR_REPL t1 where t1.rsrc_name = '%s'",
		 MDASSCHEMENAME, resourceName);
    if (check_exists(sqlq) != 0) return (RESOURCE_NOT_IN_CAT);
    else return (i);
    }

      sprintf(cval1, "%sMDAS_TD_RSRC_TYP",  MDASSCHEMENAME);
      sprintf(cval2, "%sMDAS_TD_LOCN",  MDASSCHEMENAME);
  *resourceType = (char *) get_from_result_struct(
		  (mdasC_sql_result_struct *) myresult, 
		  cval1,"rsrc_typ_name");

  *resourceLoc = (char *)get_from_result_struct(
		  (mdasC_sql_result_struct *) myresult, 
		  cval2,"netprefix");
  sprintf(cval1, "%sMDAS_TD_RSRC_CLASS",  MDASSCHEMENAME);
  *resourceClass = (char *) get_from_result_struct(
		  (mdasC_sql_result_struct *) myresult, 
		  cval1,"rsrc_class_name");

  sprintf(cval1, "%sMDAS_AR_PHYSICAL",  MDASSCHEMENAME);
  rsrcMaxSize =  (char *) get_from_result_struct(
		  (mdasC_sql_result_struct *) myresult, 
		  cval1,"max_obj_size");
  *resourceMaxSize = malloc(sizeof(int) * myresult->row_count);
  for (i = 0; i <myresult->row_count ; i++) {
    *(*resourceMaxSize + i) = atoi(rsrcMaxSize + i*  MAX_DATA_SIZE);
  }
  free(rsrcMaxSize);
  *rowcount = myresult->row_count;

  free(myresult);
  return(MDAS_SUCCESS);

}




int get_dataset_info(int cat_type, char *obj_data_name, char *obj_user, 
		      char *obj_access_name, char *obj_domain_name,
			    char *collection_name,
		   mdasC_sql_result_struct  **myresult,
		      int numrows)
{
  int i;

  tabledIsOkay = 1;
  i = getDatasetInfo(cat_type, obj_data_name, obj_user,
			  obj_access_name,obj_domain_name,
			  collection_name,
			  myresult, numrows);
  tabledIsOkay = 0;
  return(i);

}
		
void rmGlbFromResult (mdasC_sql_result_struct  *myresult) 
{
    int len;
    int i;
    char *ptr;

    if (myresult == NULL)
	return;

    for (i = 0; i < myresult->result_count ; i++){
	len = strlen (myresult->sqlresult[i].tab_name);
	if (len <= 0)
	    continue;
	ptr = malloc (len + 1);
	strcpy (ptr, myresult->sqlresult[i].tab_name);
	myresult->sqlresult[i].tab_name = ptr;
	
	len = strlen (myresult->sqlresult[i].att_name);
	if (len <= 0)
	    continue;
	ptr = malloc (len + 1);
	strcpy (ptr, myresult->sqlresult[i].att_name);
	myresult->sqlresult[i].att_name = ptr;
    }
    return;
}


int 
inTabledExistsData(char *sqlq)
{
   int i;
#ifndef LEANMCAT
	    return(-1);
#else
   if (tabledIsOkay == 0)
     return(-1);
   for (i = 0; i < TABLED_EXISTS_DATA_COUNTER ; i++) {
     if (!strcmp(tabledExistsDataQuery[i],sqlq)) {
       /*
       elog(NOTICE,"TABLED_EXISTS_DATA SUCCESS:%i", i);
       */
       return (MDAS_SUCCESS);
     }
   }
   /*
   elog(NOTICE,"TABLED_EXISTS_DATA FAILURE: %s", sqlq);
   */
   return(-1);
#endif
}

int
addToTabledExistsData(char *sqlq) 
{
  int i;
#ifndef LEANMCAT
    return(MDAS_SUCCESS);
#else
  if (TABLED_EXISTS_DATA_COUNTER  == MAX_TABLED_EXISTS_DATA_COUNTER)
    return(-1);
  tabledExistsDataQuery[TABLED_EXISTS_DATA_COUNTER] = 
    strdup(sqlq);
  TABLED_EXISTS_DATA_COUNTER++;
  /*
  elog(NOTICE,"TABLED_EXISTS_DATA_COUNTER = %i\n",TABLED_EXISTS_DATA_COUNTER);
  */
  return(MDAS_SUCCESS);
#endif
}

char*
getTabledSingleData(char *sqlq)
{
   int i;
#ifndef LEANMCAT
   return(NULL);
#else 
   if (tabledIsOkay == 0)
     return(NULL);
   for (i = 0; i < TABLED_SINGLE_DATA_COUNTER ; i++) {
     if (!strcmp(tabledSingleDataQuery[i],sqlq)) {
       /*
       elog(NOTICE,"TABLED_SINGLE_DATA SUCCESS: %i", i);
       */
       return (tabledSingleDataResult[i]);
     }
   }
   /*
   elog(NOTICE,"TABLED_SINGLE_DATA FAILURE: %s", sqlq);
   */
   return(NULL);
#endif 
}

int
addToTabledSingleData(char *sqlq, unsigned char *data) 
{
  int i;

#ifndef LEANMCAT
    return(MDAS_SUCCESS);
#else  
  if (TABLED_SINGLE_DATA_COUNTER  == MAX_TABLED_SINGLE_DATA_COUNTER)
       return(-1);
  tabledSingleDataQuery[TABLED_SINGLE_DATA_COUNTER] = 
       strdup(sqlq);
  tabledSingleDataResult[TABLED_SINGLE_DATA_COUNTER] = 
       strdup(data);
  TABLED_SINGLE_DATA_COUNTER++;
  /*
  elog(NOTICE,"TABLED_SINGLE_DATA_COUNTER = %i\n",TABLED_SINGLE_DATA_COUNTER);
  */
  return(MDAS_SUCCESS);
#endif 

}




int
copyMdasSqlResultStruct(mdasC_sql_result_struct  *inResult,
			mdasC_sql_result_struct  **outResult)
{
  int i;
  mdasC_sql_result_struct* result1;

  *outResult = NULL;

  if ((result1 = (mdasC_sql_result_struct *) 
       malloc(sizeof(mdasC_sql_result_struct)))  == NULL)
    return(MEMORY_ALLOCATION_ERROR);
  
  for (i = 0; i < inResult->result_count ; i++) {
    if ((result1->sqlresult[i].values = 
	 (char *) malloc( inResult->row_count * MAX_DATA_SIZE)) == NULL) {
      for (i-- ; i < 0 ; i--) 
	free(result1->sqlresult[i].values);
      return(MEMORY_ALLOCATION_ERROR);
    }
    memcpy(result1->sqlresult[i].values, 
	   inResult->sqlresult[i].values, inResult->row_count * MAX_DATA_SIZE);
    result1->sqlresult[i].tab_name = strdup(inResult->sqlresult[i].tab_name);
    result1->sqlresult[i].att_name = strdup(inResult->sqlresult[i].att_name);
  }
  result1->continuation_index = inResult->continuation_index;
  result1->result_count = inResult->result_count;
  result1->row_count = inResult->row_count;
  *outResult = result1;
  return(MDAS_SUCCESS);

}
int
getTabledGetDatasetInfo(int cat_type, char *in_data_name, 
			char *obj_user_name, char *obj_access_name, 
			char *obj_domain_name, char *collection_name,
			mdasC_sql_result_struct  **myresult,  int numrows)
{
  int i;
#ifndef LEANMCAT
   return(-1);
#else 
   if (tabledIsOkay == 0)
     return(-1);
  for (i = 0; i < TABLED_GETDATASETINFO_COUNTER ; i++) {
    if (tabledGetDatasetInfo[i].cat_type ==  cat_type &&
	!strcmp(tabledGetDatasetInfo[i].in_data_name, in_data_name) &&
	!strcmp(tabledGetDatasetInfo[i].obj_user_name, 	obj_user_name) &&
	!strcmp(tabledGetDatasetInfo[i].obj_access_name, obj_access_name) &&
	!strcmp(tabledGetDatasetInfo[i].obj_domain_name, obj_domain_name) &&
	!strcmp(tabledGetDatasetInfo[i].collection_name, collection_name) &&
	tabledGetDatasetInfo[i].myresult->row_count <= numrows)
      {
	if (copyMdasSqlResultStruct(tabledGetDatasetInfo[i].myresult,
				 myresult) == 0)
	  return(MDAS_SUCCESS);
	else
	  return(-1);
      }
  }
  return(-1);
#endif 
}


int 
removeTabledQueries(char *string)
{
  int i,j;

  for (i = 0; i < TABLED_EXISTS_DATA_COUNTER ; i++){
    if (strstr(tabledExistsDataQuery[i],string) != NULL) {
      free(tabledExistsDataQuery[i]);
      TABLED_EXISTS_DATA_COUNTER--;
      if (TABLED_EXISTS_DATA_COUNTER != i ) {
	tabledExistsDataQuery[i] = 
	  tabledExistsDataQuery[TABLED_EXISTS_DATA_COUNTER];
      }
      TABLED_EXISTS_DATA_COUNTER--;
    }
  }
  for (i = 0; i < TABLED_SINGLE_DATA_COUNTER ; i++) {
    if (strstr(tabledSingleDataQuery[i],string) != NULL ||
	strstr(tabledSingleDataResult[i],string) != NULL ){
      free(tabledSingleDataQuery[i]);
      free(tabledSingleDataResult[i]);
      TABLED_SINGLE_DATA_COUNTER--;
      if (TABLED_SINGLE_DATA_COUNTER != i ) {
	tabledSingleDataQuery[i] =
	  tabledSingleDataQuery[TABLED_SINGLE_DATA_COUNTER];
	tabledSingleDataResult[i] =
	  tabledSingleDataResult[TABLED_SINGLE_DATA_COUNTER];
      }
      TABLED_SINGLE_DATA_COUNTER--;
      
    }
  }
  for (i = 0; i < TABLED_GETDATASETINFO_COUNTER ; i++) {
    if (tabledGetDatasetInfo[i].in_data_name && (
	  strstr(tabledGetDatasetInfo[i].in_data_name,string) != NULL ||
	  strstr(tabledGetDatasetInfo[i].obj_user_name,string) != NULL ||
	  strstr(tabledGetDatasetInfo[i].collection_name,string))){
	strcpy(tabledGetDatasetInfo[i].in_data_name,"");
	strcpy(tabledGetDatasetInfo[i].obj_user_name,"");
	strcpy(tabledGetDatasetInfo[i].obj_access_name,"xx");
    }
  }

}

int
addToTabledGetDatasetInfo(int cat_type, char *in_data_name, 
			  char *obj_user_name, char *obj_access_name, 
			  char *obj_domain_name,char *collection_name,
			  mdasC_sql_result_struct  *myresult, int numrows)
{
#ifndef LEANMCAT
    return(MDAS_SUCCESS);
#else  
  if (myresult->continuation_index != -1) 
    return(-1);
  if (TABLED_GETDATASETINFO_COUNTER  == MAX_TABLED_GETDATASETINFO_COUNTER)
    return(-1);
  i = TABLED_GETDATASETINFO_COUNTER;
  if (copyMdasSqlResultStruct(myresult,&tabledGetDatasetInfo[i].myresult)
        == MDAS_SUCCESS) {
    tabledGetDatasetInfo[i].cat_type =  cat_type;
    tabledGetDatasetInfo[i].in_data_name = strdup(in_data_name);
    tabledGetDatasetInfo[i].obj_user_name = strdup(obj_user_name); 
    tabledGetDatasetInfo[i].obj_access_name = strdup(obj_access_name);
    tabledGetDatasetInfo[i].obj_domain_name = strdup(obj_domain_name);
    tabledGetDatasetInfo[i].collection_name = strdup(collection_name);
    TABLED_GETDATASETINFO_COUNTER++;
  }
  return(MDAS_SUCCESS);
#endif
}

int
getAccessId(char *accsName) {
  int i;
  for (i = 0; accsIdList[i] >= 0 ; i++) {
    if (!strcmp(accsNameList[i],accsName))
      break;
  }
  if (accsIdList[i] >= 0)
    return accsIdList[i];
  else
    return ( ACCESS_TYPE_NOT_IN_CAT);
    
}

int
checkTrashSuperUserAccess(char *obj_user_name, char *obj_domain_name,
			  char *collection_name )
{
  char localZoneName[MAX_TOKEN];
  int i;

  i = isUserSysAdmin(obj_user_name,obj_domain_name);
  if (i != MDAS_SUCCESS) 
    return(i);
  /*
  if (strstr(collection_name,DEF_TRASH_COLL) != collection_name) {
    failure = 0;
    sprintf(sqlq, "select zone_id from %s%s where local_zone_flag = 1",
	    MDASSCHEMENAME, "MDAS_TD_ZONE");
    tabledIsOkay = 1;
    get_ctype_value_from_query(localZoneName,sqlq);
    tabledIsOkay = 0;
    if (failure != 0) 
      return(ACTION_NOT_ALLOWED);
    sprintf(sqlq, "/%s%s",localZoneName,DEF_TRASH_COLL);
    if (strstr(collection_name,sqlq) !=collection_name)
      return(ACTION_NOT_ALLOWED);
  }
  */
  return(MDAS_SUCCESS);
  
}
#endif   /*for SRB_MDAS*/
