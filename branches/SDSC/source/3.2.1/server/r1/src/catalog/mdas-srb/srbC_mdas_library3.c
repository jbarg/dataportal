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


int
get_max_in_series( char *tokenTable, char *tokenIdAttr)
{
 int r_num;
 int i;
  sprintf(sqlq,"select max(%s) from %s%s", tokenIdAttr,
	  MDASSCHEMENAME,  tokenTable);
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
      failure = METADATA_ACCESS_ERROR;
      return (DATA_NOT_IN_CAT);
    }
  else 
    {
      truncate_end_blanks(data_size[0], data[0]);
      r_num = 1 + atoi( (char *) data[0]);
      i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
      return(r_num);
      }
}

int
insertTokenInfo(char *TokenSwitch, char *newValue, char *parentValue,
		char *registrarName, char *registrarDomain,
		char *registrarPassword)
{

char tokenNameAttr[SMALL_TOKEN];
char tokenIdAttr[SMALL_TOKEN];
char tokenTable[SMALL_TOKEN];
char temp[MAX_TOKEN], temp2[MAX_TOKEN];
char queryString[HUGE_STRING];
char parentId[MAX_TOKEN], newId[MAX_TOKEN];
char *mimeType = NULL;
char *dotExtender = NULL;
int newintId;
int i;
int hierarchical;


/**** RAJA removed  January 24, 2003 as these checks are made in the layer above ****

  i = web_mdas_sys_authorization(registrarName, registrarPassword,
                                  registrarDomain);

   if ( i != 0)    
     {
       return(SYS_USER_AUTHORIZATION_ERROR);
     }
**** RAJA removed  January 24, 2003 as these checks are made in the layer above ****/
   i = isUserSysAdmin(registrarName,  registrarDomain);
   if ( i != 0)           
     return(SYS_USER_AUTHORIZATION_ERROR);

    hierarchical = -1;
    failure = 0;
    strcpy(reg_obj_code,"NOOBJ");

    if  (!strcmp(TokenSwitch,"ResourceType"))
      {strcpy(tokenTable,"MDAS_TD_RSRC_TYP");
       strcpy(tokenIdAttr,"rsrc_typ_id");
       strcpy(tokenNameAttr,"rsrc_typ_name");
       hierarchical = 1;
      }
    else if (!strcmp(TokenSwitch,"DataType"))
      {strcpy(tokenTable,"MDAS_TD_DATA_TYP");
       strcpy(tokenIdAttr,"data_typ_id");
       strcpy(tokenNameAttr,"data_typ_name");
       hierarchical = 1;
       mimeType = strstr(newValue,"|");
       if (mimeType != NULL) {
	 *mimeType = '\0';
	 mimeType++;
	 dotExtender = strstr(mimeType,"|");
	 if (dotExtender != NULL) {
	 *dotExtender = '\0';
	 dotExtender++;
	 }
       }
      }
    else if (!strcmp(TokenSwitch,"UserType"))
      {strcpy(tokenTable,"MDAS_TD_USER_TYP");
       strcpy(tokenIdAttr,"user_typ_id");
       strcpy(tokenNameAttr,"user_typ_name");
       hierarchical = 1;
      }
    else if (!strcmp(TokenSwitch,"Domain"))
      {strcpy(tokenTable,"MDAS_TD_DOMN");
       strcpy(tokenIdAttr,"domain_id");
       strcpy(tokenNameAttr,"domain_desc");
       hierarchical = 1;
      }
    else if (!strcmp(TokenSwitch,"Action"))
      {strcpy(tokenTable,"MDAS_TD_ACTN");
       strcpy(tokenIdAttr,"action_id");
       strcpy(tokenNameAttr,"action_desc");
       hierarchical = 0;
      }
    else if (!strcmp(TokenSwitch,"ResourceClass"))
      {strcpy(tokenTable,"MDAS_TD_RSRC_CLASS");
       strcpy(tokenIdAttr,"rsrc_class_id");
       strcpy(tokenNameAttr,"rsrc_class_name");
       hierarchical = 0;
      }
    else if (!strcmp(TokenSwitch,"AuthScheme"))
      {strcpy(tokenTable,"MDAS_TD_AUTH_SCHM");
       strcpy(tokenIdAttr,"auth_scheme_id");
       strcpy(tokenNameAttr,"auth_scheme");
       hierarchical = 0;
      }    
    else 
      {
#ifdef DEBUGON 
        elog(NOTICE,"Unknown option \n");
#endif /* INSMESSON */ 
	return(TOKEN_IDEN_TYPE_ERROR);
      }
    


    sprintf(temp,"%s.%s",tokenTable,tokenNameAttr);
    get_ctype_value(temp2,temp, newValue);
    if (failure == 0) {return (TOKEN_ALREADY_IN_CAT);}
    failure = 0;

    if (hierarchical == 1)
      {
	if (strlen(parentValue) == 0)
	  return(DATA_VALUE_TYPE_ERROR);
	/*printf("iT1:'%s','%s'\n",temp, parentValue);*/
	get_ctype_value(parentId, temp, parentValue);
	if (failure != 0) {return (TOKEN_NOT_IN_CAT);}
	/*printf("PARNM=%s, PARID=%s\n",parentValue,parentId);*/
	
	i = get_next_in_hierarchy(parentId,newId,tokenTable,tokenIdAttr);
	if (i != 0) 
	  return (i);
	/** fprintf(stderr,"iT2:%i:%s",i, newId);fflush(stderr); **/
        sprintf(sqlq, "insert into %s%s values ('%s','%s')",
		MDASSCHEMENAME, tokenTable, newId, newValue); 
	/** fprintf(stderr,"INS:%s\n",sqlq);fflush(stderr);**/
        res = ExecSqlDb2(henv, hdbc, sqlq); 
        /** fprintf(stderr,"INSR:%i:%s\n",res,sqlq);fflush(stderr);**/

	if (res != 0) {
	  /*** Added July 30, 2001 to avoid hierarchy contention ***/
	  commit_db2_interaction(MDAS_CAT_ROLLBACK);
	  /*** Added July 30, 2001 to avoid hierarchy contention ***/
	  return(METADATA_INSERTION_ERROR);
	}
	else {
	  /*** Added July 30, 2001 to avoid hierarchy contention ***/
	  commit_db2_interaction(MDAS_CAT_COMMIT);
	  /*** Added July 30, 2001 to avoid hierarchy contention ***/
	  if (dotExtender != NULL) {
	    sprintf(query_string, 
		    "insert into %s%s (data_typ_id,data_typ_mime,data_typ_ext) values ('%s','%s','%s')",
		    MDASSCHEMENAME, "MDAS_AT_DATA_TYP_EXT", 
		    newId, mimeType, dotExtender);
	    res = ExecSqlDb2(henv, hdbc, query_string);
	    if (res != 0)  
	      return(MDAS_AT_DATA_TYP_EXT_INSERTION_ERROR);
	  }
	  return(MDAS_SUCCESS);
	}
      }
    else if (hierarchical == 0)
      {
	newintId = get_max_in_series(tokenTable,tokenIdAttr);
	if (newintId < 0)
	  return (newintId);
	/*printf("iT2:%i", newintId);*/
        sprintf(sqlq, "insert into %s%s values (%i,'%s')",
		MDASSCHEMENAME, tokenTable, newintId, newValue); 
	/*printf("INS:%s\n",sqlq);*/
        res = ExecSqlDb2(henv, hdbc, sqlq);
        if (res != 0) 
	  return(METADATA_INSERTION_ERROR);
	else
	  return(MDAS_SUCCESS);
      }
    else
      {
	return(DATA_VALUE_TYPE_ERROR);
      }
}


int
insertDataMimeTypeInfo ( char *dataTypeName, char *mydataTypeMimeString,
			 char *mydataTypeExtenders, char *registrarName,  
			 char *registrarDomain, char *registrarAuth)
{
  int i,j;
  char dataTypeId[MAX_TOKEN];
  char temp[MAX_TOKEN];
  char *dataTypeExtenders;
  char *dataTypeMimeString;
  char emptyStr[4];

   strcpy(emptyStr," ");
   failure = 0;
   if (strlen(mydataTypeMimeString) > 0)
     dataTypeMimeString = mydataTypeMimeString;
   else
     dataTypeMimeString = emptyStr;
   if (strlen(mydataTypeExtenders) > 0)
     dataTypeExtenders = mydataTypeExtenders;
   else
     dataTypeExtenders = emptyStr;
   i = isUserSysAdmin(registrarName,  registrarDomain);
   if ( i != 0)           
     return(SYS_USER_AUTHORIZATION_ERROR);
   sprintf(temp,"%s.%s","MDAS_TD_DATA_TYP","data_typ_name");
   get_ctype_value(dataTypeId, temp, dataTypeName);
   if (failure != 0) 
     return (TOKEN_NOT_IN_CAT);
   sprintf(query_string,
	   "select  data_typ_id from %sMDAS_AT_DATA_TYP_EXT where data_typ_id = '%s' and data_typ_mime = '%s' and data_typ_ext = '%s'",
	   MDASSCHEMENAME, dataTypeId, dataTypeMimeString, dataTypeExtenders);
   if (check_exists(query_string) == 0) 
     {
       failure = MIME_EXT_ALREADY_IN_CAT;
       return (MIME_EXT_ALREADY_IN_CAT);
     }
  get_time(reg_timeval);
  sprintf(query_string, 
         "insert into %s%s (data_typ_id,data_typ_mime,data_typ_ext) values ('%s','%s','%s')",
          MDASSCHEMENAME, "MDAS_AT_DATA_TYP_EXT", 
	  dataTypeId, dataTypeMimeString, dataTypeExtenders);
  res = ExecSqlDb2(henv, hdbc, query_string);
  if (res != 0)  
     return(MDAS_AT_DATA_TYP_EXT_INSERTION_ERROR);
  return(MDAS_SUCCESS);

}
int
insertRsrcAccessInfo ( char *accessList, char *accessPrivilege,
		       char *accessConstraint, char *accessId,
		       char *registrarName,char *registrarDomain,
		       char *registrarPassword )
{
  int i,j;
  
   i = isUserSysAdmin(registrarName,  registrarDomain);
   if ( i != 0)           
     return(SYS_USER_AUTHORIZATION_ERROR);

   sprintf(query_string,
	   "select  access_id from %sMDAS_TD_RSRC_ACCS where access_id = %i",
				 MDASSCHEMENAME, accessId);
   if (check_exists(query_string) == 0) 
     {
       failure = ACCESS_ID_ALREADY_IN_CAT;
       return (ACCESS_ID_ALREADY_IN_CAT);
     }
   sprintf(query_string,
	   "select  access_constraint from %sMDAS_TD_RSRC_ACCS where access_constraint = '%s'",
				 MDASSCHEMENAME, accessConstraint);
   if (check_exists(query_string) == 0) 
     {
       failure = ACCESS_CONSTRAINT_ALREADY_IN_CAT;
       return (ACCESS_CONSTRAINT_ALREADY_IN_CAT);
     }
  get_time(reg_timeval);
  sprintf(query_string, 
         "insert into %s%s (access_id,access_constraint,access_list,access_privilege) values (%i,'%s','%s','%s')",
          MDASSCHEMENAME, "MDAS_TD_RSRC_ACCS", accessId, accessConstraint,
	  accessList, accessPrivilege);
  res = ExecSqlDb2(henv, hdbc, query_string);
  if (res != 0)  
     return(MDAS_TD_RSRC_ACCS_INSERTION_ERROR);
  return(MDAS_SUCCESS);

}

int
insertDataAccessInfo ( char *accessList, char *accessPrivilege,
		       char *accessConstraint, char *accessId,
		       char *registrarName,char *registrarDomain,
		       char *registrarPassword )
{
  int i,j;
  
   i = isUserSysAdmin(registrarName,  registrarDomain);
   if ( i != 0)           
     return(SYS_USER_AUTHORIZATION_ERROR);

   sprintf(query_string,
	   "select  access_id from %sMDAS_TD_DS_ACCS where access_id = %i",
				 MDASSCHEMENAME, accessId);
   if (check_exists(query_string) == 0) 
     {
       failure = ACCESS_ID_ALREADY_IN_CAT;
       return (ACCESS_ID_ALREADY_IN_CAT);
     }
   sprintf(query_string,
	   "select  access_constraint from %sMDAS_TD_DS_ACCS where access_constraint = '%s'",
				 MDASSCHEMENAME, accessConstraint);
   if (check_exists(query_string) == 0) 
     {
       failure = ACCESS_CONSTRAINT_ALREADY_IN_CAT;
       return (ACCESS_CONSTRAINT_ALREADY_IN_CAT);
     }
  get_time(reg_timeval);
  sprintf(query_string, 
         "insert into %s%s (access_id,access_constraint,access_list,access_privilege) values (%i,'%s','%s','%s')",
          MDASSCHEMENAME, "MDAS_TD_DS_ACCS", accessId, accessConstraint,
	  accessList, accessPrivilege);
  res = ExecSqlDb2(henv, hdbc, query_string);
  if (res != 0)  
     return(MDAS_TD_DS_ACCS_INSERTION_ERROR);
  return(MDAS_SUCCESS);

}

int
registerResourceInfo(char *resourceName,char *resourceType,char *resourceLocation,
		   char *registrarName,char *registrarDomainZone,
		   char *registrarPassword, char *indefaultPath,
		     char *resourceClass, int maxObjSizeInResource)
{
  int i;
  failure = 0;
  get_time(reg_timeval);
  if (failure != 0) { return (GET_TIME_ERROR);}
  i = registerResourceInfoInternal(resourceName, resourceType, resourceLocation,
				   registrarName, registrarDomainZone, registrarPassword,
				   indefaultPath, resourceClass, maxObjSizeInResource,
				   reg_timeval, reg_timeval, 
				   registrarName, registrarDomainZone,
				   "");
  return(i);
}

int
registerResourceInfoInternal(char *resourceName,char *resourceType,char *location,
		   char *registrarName,char *registrarDomain,
		   char *registrarPassword, char *indefaultPath,
		     char *resourceClass, int maxObjSizeInResource,
			     char *createTimeVal, char *modifyTimeVal,
			     char *ownerName, char *ownerDomainZone, 
			     char *resourceComments)
{
  char resourceTypeId[MAX_TOKEN];
  char locationId[MAX_TOKEN];
  char temp[MAX_TOKEN], temp2[MAX_TOKEN];
  char queryString[HUGE_STRING];
  int i;
  int  obj_action_id;
  char defaultPath[HUGE_STRING];
  int resourceClassId;
  int ownerId;
  char pubName[MAX_TOKEN];

   uralize(indefaultPath, defaultPath);
   strcpy(reg_registrar_name,registrarName);
   strcpy(reg_obj_name, resourceName);
   strcpy(reg_obj_code,"MDAS_CD_RSRC");
   strcpy(reg_action_name,"create resource");
   reg_obj_repl_enum = 0;
   failure = 0;
/**** RAJA removed  January 24, 2003 as these checks are made in the layer above ****

  i = web_mdas_sys_authorization(registrarName, registrarPassword,
                                  registrarDomain);
   if ( i != 0)    
     {
       sprintf(query_string,"resource registry SYSAPI: Failed Password:%s,%s\n",
          registrarPassword,registrarName);
       return(SYS_USER_AUTHORIZATION_ERROR);
     }
*** RAJA removed  January 24, 2003 as these checks are made in the layer above ****/
   i = isUserSysAdmin(registrarName,  registrarDomain);
   if ( i != 0)           
     return(SYS_USER_AUTHORIZATION_ERROR);

   sprintf(query_string,
	   "select  rsrc_id from %sMDAS_AR_REPL where rsrc_name = '%s'",
				 MDASSCHEMENAME, resourceName);
   if (check_exists(query_string) == 0) 
     {
       failure = RESOURCE_ALREADY_IN_CAT;
       return (RESOURCE_ALREADY_IN_CAT);
     }
 
  reg_registrar_id = get_user_id_in_domain(registrarName, registrarDomain);
  if (reg_registrar_id < 0) { return (USER_NOT_IN_DOMN);}
  ownerId = get_user_id_in_domain(ownerName,ownerDomainZone);
  if (ownerId < 0) { return (USER_NOT_IN_DOMN);}
 
 
  get_ctype_value(resourceTypeId, 
		  "MDAS_TD_RSRC_TYP.rsrc_typ_name", resourceType);
   if (failure != 0) { return (MDAS_TD_RSRC_TYP_ACCESS_ERROR);}
 
  get_ctype_value(locationId, "MDAS_TD_LOCN.locn_desc", location);
   if (failure != 0) { return (MDAS_TD_LOCN_ACCESS_ERROR);}

  resourceClassId  = get_itype_value("MDAS_TD_RSRC_CLASS.rsrc_class_name", 
				resourceClass);
   if (failure != 0) { return (RESOURCE_CLASS_NOT_IN_CAT);}

  reg_obj_id = get_next_counter_value("RSRC_ID");
  if (failure != 0) 
    return (RSRC_ID_COUNTER_ERROR);
  get_time(reg_timeval);
  if (failure != 0) 
    return (GET_TIME_ERROR);

  sprintf(query_string, 
         "insert into %s%s (rsrc_id,rsrc_name,rsrc_typ_id) values (%i,'%s','%s')",
          MDASSCHEMENAME, "MDAS_CD_RSRC", 
          reg_obj_id, resourceName, resourceTypeId);
  /*printf("rR1:%s\n", query_string);*/
  res = ExecSqlDb2(henv, hdbc, query_string);
  if (res != 0)  
    {error_exit("user registry API: CD resource insertion error");
     return(MDAS_CD_RSRC_INSERTION_ERROR);}

#ifdef FED_MCAT
  sprintf(query_string, 
         "insert into %s%s (rsrc_id,repl_enum,rsrc_name,rsrc_typ_id,locn_id,repl_timestamp,owner,phy_rsrc_id, default_path,max_obj_size,rsrc_class_id,rsrc_creation_date,rsrc_modify_date,rsrc_comments) values (%i,%i,'%s','%s','%s','%s',%i,%i,'%s',%i,%i,'%s','%s','%s')",
          MDASSCHEMENAME, "MDAS_AR_REPL", 
          reg_obj_id, reg_obj_repl_enum,
	  resourceName, resourceTypeId,
	  locationId,reg_timeval,ownerId, reg_obj_id,defaultPath,
	  maxObjSizeInResource,resourceClassId,createTimeVal,modifyTimeVal,resourceComments);
#else
  sprintf(query_string, 
         "insert into %s%s (rsrc_id,repl_enum,rsrc_name,rsrc_typ_id,locn_id,repl_timestamp,owner,phy_rsrc_id, default_path,max_obj_size,rsrc_class_id) values (%i,%i,'%s','%s','%s','%s',%i,%i,'%s',%i,%i)",
          MDASSCHEMENAME, "MDAS_AR_REPL", 
          reg_obj_id, reg_obj_repl_enum,
	  resourceName, resourceTypeId,
	  locationId,reg_timeval,ownerId, reg_obj_id,defaultPath,
	  maxObjSizeInResource,resourceClassId);
#endif

  /*printf("rR2:%s\n", query_string);*/
  res = ExecSqlDb2(henv, hdbc, query_string);
  if (res != 0)  
    {error_exit("user registry API: REPL resource insertion error");
     return(MDAS_AR_REPL_INSERTION_ERROR);}
  
  sprintf(query_string, 
         "insert into %s%s (phy_rsrc_id,phy_repl_enum,phy_rsrc_name,rsrc_typ_id,locn_id,phy_repl_timestamp,phy_owner,phy_default_path,max_obj_size,rsrc_class_id) values (%i,%i,'%s','%s','%s','%s',%i,'%s',%i,%i)",
          MDASSCHEMENAME, "MDAS_AR_PHYSICAL", 
          reg_obj_id, reg_obj_repl_enum,
	  resourceName, resourceTypeId,
	  locationId,reg_timeval,ownerId,defaultPath,
	  maxObjSizeInResource,resourceClassId);
  /*printf("rR2:%s\n", query_string);*/
  res = ExecSqlDb2(henv, hdbc, query_string);
  if (res != 0)  
    {error_exit("user registry API: REPL resource insertion error");
     return(MDAS_AR_PHYSICAL_INSERTION_ERROR);}
  sprintf(query_string, 
         "insert into %s%s (rsrc_id,repl_enum,user_id,access_id) values (%i,%i,%i,%i)",
          MDASSCHEMENAME, "MDAS_AR_ACCS", reg_obj_id, reg_obj_repl_enum,reg_registrar_id,MAX_ACCS_NUM);
  res = ExecSqlDb2(henv, hdbc, query_string);
  if (res != 0)  
    {error_exit("user registry API: REPL resource insertion error");
     return(MDAS_AR_ACCS_INSERTION_ERROR);}
  
    strcpy(pubName,"public@npaci");
  i =  modify_resource_info(MDAS_CATALOG , resourceName, 
			    registrarName,  registrarDomain,
			    R_INSERT_ACCS, pubName,"write","","");
  if (i != 0)
    return(i);
  return(MDAS_SUCCESS);
}


int
registerLogicalResourceInfo(char *resourceName,char *resourceType,
		   char *physicalResourceName,
		   char *registrarName,char *registrarDomainZone,
		   char *registrarPassword, char *indefaultPath)
{
   int i;
  failure = 0;
  get_time(reg_timeval);
  if (failure != 0) { return (GET_TIME_ERROR);}
  i = registerLogicalResourceInfoInternal(resourceName, resourceType,
					  physicalResourceName,registrarName,
					  registrarDomainZone,registrarPassword,
					  indefaultPath,
					  "","",-1,
					  reg_timeval, reg_timeval, 
					  registrarName, registrarDomainZone,
					  "");
  return(i);
}
int
registerLogicalResourceInfoInternal(char *resourceName,char *resourceType,
		   char *physicalResourceName,
		   char *registrarName,char *registrarDomain,
		   char *registrarPassword, char *indefaultPath,
				    char *inResourceLocation,
				    char *inResourceClass, int inMaxObjSizeInResource,
				    char *createTimeVal, char *modifyTimeVal,
				    char *ownerName, char *ownerDomainZone, 
				    char *resourceComments)

{
  char resourceTypeId[MAX_TOKEN];
  char temp[MAX_TOKEN], temp2[MAX_TOKEN];
  char queryString[HUGE_STRING];
  int i;
  int  obj_action_id;
  int phy_rsrc_id;
  int ownerId;
  char defaultPath[HUGE_STRING];
  char resourceLocationId[MAX_TOKEN];
  char char_phy_rsrc_id[MAX_TOKEN];
  int resourceClassId, maxObjSizeInResource;
  int rowcount;
  char cval[USABLE_TABLE_COLS][MAX_TOKEN];

   uralize(indefaultPath, defaultPath);
   strcpy(reg_registrar_name,registrarName);
   strcpy(reg_obj_name, resourceName);
   strcpy(reg_obj_code,"MDAS_CD_RSRC");
   strcpy(reg_action_name,"create logical resource");
   reg_obj_repl_enum = 0;
   failure = 0;

/**** RAJA removed  January 24, 2003 as these checks are made in the layer above ****

  i = web_mdas_sys_authorization(registrarName, registrarPassword,
                                  registrarDomain);
   if ( i != 0)    
     {
       sprintf(query_string,"resource registry SYSAPI: Failed Password:%s,%s\n",
          registrarPassword,registrarName);
       return(SYS_USER_AUTHORIZATION_ERROR);
     }
**** RAJA removed  January 24, 2003 as these checks are made in the layer above ****/
   i = isUserSysAdmin(registrarName,  registrarDomain);
   if ( i != 0)           
     return(SYS_USER_AUTHORIZATION_ERROR);

   sprintf(query_string,
	   "select  rsrc_id from %sMDAS_AR_REPL where rsrc_name = '%s'",
				 MDASSCHEMENAME, resourceName);
   if (check_exists(query_string) == 0) 
     {
       failure = LOGICAL_RESOURCE_ALREADY_IN_CAT;
       return (LOGICAL_RESOURCE_ALREADY_IN_CAT);
     }
  reg_registrar_id = get_user_id_in_domain(registrarName,  registrarDomain);
  if (reg_registrar_id < 0) { return (USER_NOT_IN_DOMN);}
  ownerId = get_user_id_in_domain(ownerName,ownerDomainZone);
  if (ownerId < 0) { return (USER_NOT_IN_DOMN);}
  
  get_time(reg_timeval);
  if (failure != 0) { return (GET_TIME_ERROR);}
 

 
  phy_rsrc_id = get_itype_value("MDAS_AR_PHYSICAL.phy_rsrc_name", 
				physicalResourceName);
   if (failure != 0) { return (RESOURCE_NOT_IN_CAT);}

   sprintf(char_phy_rsrc_id,"%i",phy_rsrc_id);
   if (inResourceLocation == NULL || strlen(inResourceLocation) == 0) {
     get_ctype_attr_value(resourceLocationId,
			  "MDAS_AR_PHYSICAL.locn_id",
			  "MDAS_AR_PHYSICAL.phy_rsrc_id", char_phy_rsrc_id);
     if (failure != 0) { return (MDAS_AR_PHYSICAL_ACCESS_ERROR);}
   }
   else {
     get_ctype_value(resourceLocationId, "MDAS_TD_LOCN.locn_desc", inResourceLocation);
     if (failure != 0) { return (MDAS_TD_LOCN_ACCESS_ERROR);}
   }
   /**** Raja changed Aug 13, 2003 so that every logical resource is of type logical 
   if (resourceType ==  NULL || strlen(resourceType) == 0) {
     get_ctype_attr_value(resourceTypeId,
			  "MDAS_AR_PHYSICAL.rsrc_typ_id",
			  "MDAS_AR_PHYSICAL.phy_rsrc_id", char_phy_rsrc_id);
     if (failure != 0) { return (MDAS_AR_PHYSICAL_ACCESS_ERROR);}
   }
   else {
     get_ctype_value(resourceTypeId, 
		     "MDAS_TD_RSRC_TYP.rsrc_typ_name", resourceType);
     if (failure != 0) { return (MDAS_TD_RSRC_TYP_ACCESS_ERROR);}
   }
   ***********/
   get_ctype_value(resourceTypeId, 
		   "MDAS_TD_RSRC_TYP.rsrc_typ_name", "logical");
   if (failure != 0) { return (MDAS_TD_RSRC_TYP_ACCESS_ERROR);}
   
  sprintf(query_string, "select max_obj_size, rsrc_class_id from %s%s where phy_rsrc_id = %i",
	  MDASSCHEMENAME, "MDAS_AR_PHYSICAL", phy_rsrc_id);
  rowcount = 2;
   i = get_row_from_query(cval,&rowcount,query_string);
   if (i < 0 || rowcount == 0) {
     return (MDAS_AR_PHYSICAL_ACCESS_ERROR);
   }
   resourceClassId = atoi((char *) cval[1]);
   maxObjSizeInResource = atoi((char *) cval[0]);
   if (inMaxObjSizeInResource >= 0)
     maxObjSizeInResource = inMaxObjSizeInResource;
     
   if (inResourceClass != NULL && strlen(inResourceClass) > 0) {
     resourceClassId  = get_itype_value("MDAS_TD_RSRC_CLASS.rsrc_class_name", 
					inResourceClass);
     if (failure != 0) { return (RESOURCE_CLASS_NOT_IN_CAT);}
   }

  reg_obj_id = get_next_counter_value("RSRC_ID");
  if (failure != 0) {
    return (RSRC_ID_COUNTER_ERROR);
  }

  sprintf(query_string, 
         "insert into %s%s (rsrc_id,rsrc_name,rsrc_typ_id) values (%i,'%s','%s')",
          MDASSCHEMENAME, "MDAS_CD_RSRC", 
          reg_obj_id, resourceName, resourceTypeId);
  /*printf("rR1:%s\n", query_string);*/
  res = ExecSqlDb2(henv, hdbc, query_string);
  if (res != 0)  
    {error_exit("user registry API: CD resource insertion error");
     return(MDAS_CD_RSRC_INSERTION_ERROR);}
#ifdef FED_MCAT
  sprintf(query_string, 
         "insert into %s%s (rsrc_id,repl_enum,rsrc_name,rsrc_typ_id,locn_id,repl_timestamp,owner,phy_rsrc_id, default_path,max_obj_size,rsrc_class_id,rsrc_creation_date,rsrc_modify_date,rsrc_comments) values (%i,%i,'%s','%s','%s','%s',%i,%i,'%s',%i,%i,'%s','%s','%s')",
          MDASSCHEMENAME, "MDAS_AR_REPL", 
          reg_obj_id, reg_obj_repl_enum,
	  resourceName, resourceTypeId,
	  resourceLocationId,reg_timeval,
	  ownerId, phy_rsrc_id,defaultPath,
	  maxObjSizeInResource,resourceClassId,createTimeVal,modifyTimeVal,resourceComments);
#else
  sprintf(query_string, 
         "insert into %s%s (rsrc_id,repl_enum,rsrc_name,rsrc_typ_id,locn_id,repl_timestamp,owner,phy_rsrc_id, default_path,max_obj_size,rsrc_class_id) values (%i,%i,'%s','%s','%s','%s',%i,%i,'%s',%i,%i)",
          MDASSCHEMENAME, "MDAS_AR_REPL", 
          reg_obj_id, reg_obj_repl_enum,
	  resourceName, resourceTypeId,
	  resourceLocationId,reg_timeval,
	  ownerId, phy_rsrc_id,defaultPath,
	  maxObjSizeInResource,resourceClassId);
#endif
  /*printf("rR2:%s\n", query_string);*/
  res = ExecSqlDb2(henv, hdbc, query_string);
  if (res != 0)  
    {error_exit("user registry API: REPL resource insertion error");
     return(MDAS_AR_REPL_INSERTION_ERROR);}


  return(MDAS_SUCCESS);
}

int get_max_resource_repl_num(int obj_rsrc_id)
{

    int r_num;

  sprintf(sqlq,"select max(REPL_ENUM) from %sMDAS_AR_REPL where rsrc_id = %i",
	  MDASSCHEMENAME,  obj_rsrc_id);
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
        sprintf(errmess,"MDAS registry: Get_max_repl_num: Data missin:%s",sqlq);
	failure = RESOURCE_NOT_IN_CAT;
	return (RESOURCE_NOT_IN_CAT);
      }
    else 
      {
    truncate_end_blanks(data_size[0], data[0]);
    r_num = 1 + atoi( (char *) data[0]);
    i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
    return(r_num);
      }

}


int
registerReplicateResourceInfo(char *physicalResourceName,char *resourceType,
		   char *oldLogicalResourceName,
		   char *registrarName,char *registrarDomainZone,
		   char *registrarPassword, char *indefaultPath)
{
    int i;
  failure = 0;
  get_time(reg_timeval);
  if (failure != 0) { return (GET_TIME_ERROR);}
  i = registerReplicateResourceInfoInternal(physicalResourceName, resourceType,
					  oldLogicalResourceName,registrarName,
					  registrarDomainZone,registrarPassword,
					  indefaultPath,
					  "","",-1,
					  reg_timeval, reg_timeval, 
					  registrarName, registrarDomainZone,
					  "");
  return(i);
 
}
int
registerReplicateResourceInfoInternal(char *physicalResourceName,char *resourceType,
		   char *oldLogicalResourceName,
		   char *registrarName,char *registrarDomain,
		   char *registrarPassword, char *indefaultPath,
				    char *inResourceLocation,
				    char *inResourceClass, int inMaxObjSizeInResource,
				    char *createTimeVal, char *modifyTimeVal,
				    char *ownerName, char *ownerDomainZone, 
				    char *resourceComments)
{
  char resourceTypeId[MAX_TOKEN];
  char temp[MAX_TOKEN], temp2[MAX_TOKEN];
  char queryString[HUGE_STRING];
  int i;
  int  obj_action_id;
  int phy_rsrc_id;
  int ownerId;
  char defaultPath[HUGE_STRING];
  char resourceLocationId[MAX_TOKEN];
  char char_phy_rsrc_id[MAX_TOKEN];
  int resourceClassId, maxObjSizeInResource;
  int rowcount;
  char cval[USABLE_TABLE_COLS][MAX_TOKEN];

   uralize(indefaultPath, defaultPath);
   strcpy(reg_registrar_name,registrarName);
   strcpy(reg_obj_name, oldLogicalResourceName);
   strcpy(reg_obj_code,"MDAS_CD_RSRC");
   strcpy(reg_action_name,"create logical resource");
   reg_obj_repl_enum = 0;
   failure = 0;

/**** RAJA removed  January 24, 2003 as these checks are made in the layer above ****
  i = web_mdas_sys_authorization(registrarName, registrarPassword,
                                  registrarDomain);
   if ( i != 0)    
     {
       sprintf(query_string,"resource registry SYSAPI: Failed Password:%s,%s\n",
          registrarPassword,registrarName);
       return(SYS_USER_AUTHORIZATION_ERROR);
     }
**** RAJA removed  January 24, 2003 as these checks are made in the layer above ****/

   i = isUserSysAdmin(registrarName,  registrarDomain);
   if ( i != 0)           
     return(SYS_USER_AUTHORIZATION_ERROR);



   reg_registrar_id = get_user_id_in_domain(registrarName, 
                                           registrarDomain);
  if (reg_registrar_id < 0) { return (USER_NOT_IN_DOMN);}
  ownerId = get_user_id_in_domain(ownerName,ownerDomainZone);
  if (ownerId < 0) { return (USER_NOT_IN_DOMN);}
  /****** Disable this check as it is too restrictive Sept 30,2003 RAJA *****
   sprintf(query_string,
	   "select  owner from %sMDAS_AR_REPL where rsrc_name = '%s' and owner = %i",
	   MDASSCHEMENAME, oldLogicalResourceName,reg_registrar_id );
   if (check_exists(query_string) != 0) 
     {
       failure = REGISTRATION_PERMITS_INADEQUATE;
       return (REGISTRATION_PERMITS_INADEQUATE);
     }
     ****** Disable this check as it is too restrictive Sept 30,2003 RAJA *****/

  get_time(reg_timeval);
  if (failure != 0) { return (GET_TIME_ERROR);}
 
  phy_rsrc_id = get_itype_value("MDAS_AR_PHYSICAL.phy_rsrc_name", 
				physicalResourceName);
   if (failure != 0) { return (RESOURCE_NOT_IN_CAT);}
   sprintf(query_string,
	   "select  rsrc_id from %sMDAS_AR_REPL where rsrc_name = '%s' and phy_rsrc_id = '%i'",
	   MDASSCHEMENAME, oldLogicalResourceName,phy_rsrc_id);
   if (check_exists(query_string) == 0)   {
     failure = LOGICAL_RESOURCE_ALREADY_IN_CAT;
     return (LOGICAL_RESOURCE_ALREADY_IN_CAT);
   }

   sprintf(char_phy_rsrc_id,"%i",phy_rsrc_id);
   if (inResourceLocation == NULL || strlen(inResourceLocation) == 0) {
     get_ctype_attr_value(resourceLocationId,
			"MDAS_AR_PHYSICAL.locn_id",
			"MDAS_AR_PHYSICAL.phy_rsrc_id", char_phy_rsrc_id);
     if (failure != 0) { return (MDAS_AR_PHYSICAL_ACCESS_ERROR);}
   }
   else {
     get_ctype_value(resourceLocationId, "MDAS_TD_LOCN.locn_desc", inResourceLocation);
     if (failure != 0) { return (MDAS_TD_LOCN_ACCESS_ERROR);}
   }
   /**** Raja changed Aug 13, 2003 so that every logical resource is of type logical 
   if (resourceType ==  NULL || strlen(resourceType) == 0) {
     get_ctype_attr_value(resourceTypeId,
			  "MDAS_AR_PHYSICAL.rsrc_typ_id",
			  "MDAS_AR_PHYSICAL.phy_rsrc_id", char_phy_rsrc_id);
     if (failure != 0) { return (MDAS_AR_PHYSICAL_ACCESS_ERROR);}
   }
   else {
     get_ctype_value(resourceTypeId, 
		     "MDAS_TD_RSRC_TYP.rsrc_typ_name", resourceType);
     if (failure != 0) { return (MDAS_TD_RSRC_TYP_ACCESS_ERROR);}
   }
   ***********/
   get_ctype_value(resourceTypeId, 
		     "MDAS_TD_RSRC_TYP.rsrc_typ_name", "logical");
   if (failure != 0) { return (MDAS_TD_RSRC_TYP_ACCESS_ERROR);}

   sprintf(query_string, "select max_obj_size, rsrc_class_id from %s%s where phy_rsrc_id = %i",
	  MDASSCHEMENAME, "MDAS_AR_PHYSICAL", phy_rsrc_id);
   rowcount = 2;
   i = get_row_from_query(cval,&rowcount,query_string);
   if (i < 0 || rowcount == 0) {
     return (MDAS_AR_PHYSICAL_ACCESS_ERROR);
   }
   resourceClassId = atoi((char *) cval[1]);
   maxObjSizeInResource = atoi((char *) cval[0]);
   if (inMaxObjSizeInResource >= 0)
     maxObjSizeInResource = inMaxObjSizeInResource;
     
   if (inResourceClass != NULL && strlen(inResourceClass) > 0) {
     resourceClassId  = get_itype_value("MDAS_TD_RSRC_CLASS.rsrc_class_name", 
					inResourceClass);
     if (failure != 0) { return (RESOURCE_CLASS_NOT_IN_CAT);}
   }

  reg_obj_id = get_itype_value("MDAS_AR_REPL.rsrc_name", 
				oldLogicalResourceName);
  if (failure != 0) { return (LOGICAL_RESOURCE_NOT_IN_CAT);}

   reg_obj_repl_enum = get_max_resource_repl_num(reg_obj_id);
#ifdef FED_MCAT
  sprintf(query_string, 
         "insert into %s%s (rsrc_id,repl_enum,rsrc_name,rsrc_typ_id,locn_id,repl_timestamp,owner,phy_rsrc_id, default_path,max_obj_size,rsrc_class_id,rsrc_creation_date,rsrc_modify_date,rsrc_comments) values (%i,%i,'%s','%s','%s','%s',%i,%i,'%s',%i,%i,'%s','%s','%s')",
          MDASSCHEMENAME, "MDAS_AR_REPL", 
          reg_obj_id, reg_obj_repl_enum,
	  oldLogicalResourceName, resourceTypeId,
	  resourceLocationId,reg_timeval,ownerId,
	  phy_rsrc_id,defaultPath,
	  maxObjSizeInResource,resourceClassId,createTimeVal,modifyTimeVal,resourceComments);
#else
  sprintf(query_string, 
         "insert into %s%s (rsrc_id,repl_enum,rsrc_name,rsrc_typ_id,locn_id,repl_timestamp,owner,phy_rsrc_id, default_path,max_obj_size,rsrc_class_id) values (%i,%i,'%s','%s','%s','%s',%i,%i,'%s',%i,%i)",
          MDASSCHEMENAME, "MDAS_AR_REPL", 
          reg_obj_id, reg_obj_repl_enum,
	  oldLogicalResourceName, resourceTypeId,
	  resourceLocationId,reg_timeval,ownerId,
	  phy_rsrc_id,defaultPath,
	  maxObjSizeInResource,resourceClassId);
#endif
  /*printf("rR2:%s\n", query_string);*/
  res = ExecSqlDb2(henv, hdbc, query_string);
  if (res != 0)  
    {error_exit("user registry API: REPL resource insertion error");
     return(MDAS_AR_REPL_INSERTION_ERROR);}


  return(MDAS_SUCCESS);
}


int
registerLocationInfo(char *locationName, char *netPrefix, 
		     char *parentLocation,
		     char *serverUserName, char *serverUserDomain,
		     char *registrarName, char *registrarDomain,
		     char *registrarPassword)
{
char temp[MAX_TOKEN], temp2[MAX_TOKEN];
char queryString[HUGE_STRING];
char parentId[MAX_TOKEN], newId[MAX_TOKEN];
int newintId;
int i;
int hierarchical;
int serverUserId;

/**** RAJA removed  January 24, 2003 as these checks are made in the layer above ****
  i = web_mdas_sys_authorization(registrarName, registrarPassword,
                                  registrarDomain);
   if ( i != 0)    
     {
       return(SYS_USER_AUTHORIZATION_ERROR);
     }
**** RAJA removed  January 24, 2003 as these checks are made in the layer above ****/
 i = isUserSysAdmin(registrarName,  registrarDomain);
 if ( i != 0)           
   return(SYS_USER_AUTHORIZATION_ERROR);

    failure = 0;
    strcpy(reg_obj_code,"NOOBJ");


    get_ctype_value(temp2,"MDAS_TD_LOCN.locn_desc", locationName);
    if (failure == 0) {return (TOKEN_ALREADY_IN_CAT);}
    failure = 0;

    get_ctype_value(parentId, "MDAS_TD_LOCN.locn_desc", parentLocation);
	if (failure != 0) {return (TOKEN_NOT_IN_CAT);}
	/*printf("PARNM=%s, PARID=%s\n",parentLocation,parentId);*/
	
	serverUserId = get_user_id_in_domain(serverUserName,
					     serverUserDomain);
	 if (serverUserId < 0 )
	   {  
	     return (USER_NOT_IN_DOMN);
	   }
	i = get_next_in_hierarchy(parentId,newId,"MDAS_TD_LOCN","locn_id");
	if (i != 0) 
	  return (i);
	/*printf("iT2:%i:%s",i, newId);*/
        sprintf(sqlq, "insert into %s%s (locn_id,locn_desc,netprefix,server_user_id) values ('%s','%s','%s',%i)",
		MDASSCHEMENAME, "MDAS_TD_LOCN",newId,locationName,
		netPrefix,serverUserId); 
	/*printf("INS:%s\n",sqlq);*/
        res = ExecSqlDb2(henv, hdbc, sqlq);

	if (res != 0) {
	  /*** Added July 30, 2001 to avoid hierarchy contention ***/
	  commit_db2_interaction(MDAS_CAT_ROLLBACK);
	  /*** Added July 30, 2001 to avoid hierarchy contention ***/
	  return(METADATA_INSERTION_ERROR);
	}
	else {
	  /*** Added July 30, 2001 to avoid hierarchy contention ***/
	  commit_db2_interaction(MDAS_CAT_COMMIT);
	  /*** Added July 30, 2001 to avoid hierarchy contention ***/
	  return(MDAS_SUCCESS);
	}

}

getSRBUserInfo(int catType, mdasC_sql_result_struct  **myresult,
	       int numrows)
{
  char qval[MAX_DCS_NUM][MAX_TOKEN];
  int i , j, alive,  selval[MAX_DCS_NUM];
  /*mdasC_sql_result_struct *myresult;*/
  char *r1, *r2, *r11, *r12;
  char **srbUserList;



  for (i = 0; i < MAX_DCS_NUM; i++)
    {
      sprintf(qval[i],"");
      selval[i] = 0;
    }   
  sprintf(qval[USER_NAME]," not like '%%$deleted%%'");
  sprintf(qval[USER_TYP_NAME], " = 'sysadmin'");
  selval[USER_NAME] = 1;
  selval[DOMAIN_DESC] = 1;
  /* Added ZONE_NAME in the query. MW 3/1/04 */
#ifdef FED_MCAT
  selval[ZONE_NAME] = 1;
#endif
  failure = 0;
  i = get_data_dir_info(catType, qval, selval, myresult, dcs_tname, 
			dcs_aname, numrows);
  if ((myresult == NULL) || (i != 0))
    {
        return (i);
    }

  r1 = (char *) get_from_result_struct(
		  (mdasC_sql_result_struct *) *myresult, 
		  dcs_tname[USER_NAME], dcs_aname[USER_NAME]);
  r2 = (char *) get_from_result_struct(
		  (mdasC_sql_result_struct *) *myresult, 
		  dcs_tname[DOMAIN_DESC], dcs_aname[DOMAIN_DESC]);
  r11 = r1;
  r12 = r2;
  j = 0; 
  alive = 1;
  while (alive && j < (numrows -1) )
    {
      if(j < (*myresult)->row_count)
	{
	  sprintf(r1,"%s@%s", r1, r2);
	  j++;
	}
      else
	{
	  alive = 0;
	}
      r1 += MAX_DATA_SIZE;
      r2 += MAX_DATA_SIZE;
    }
  /**** (*myresult)->result_count = 1;  KESTOCK 01/24/2001 ****/
  /* sprintf(srbUserList[j],"%s", RESWANTED);*/
  /*printf("AA:%i:%i:%i:%i\n",j,r1,r11,(r1 - (j+1)*MAX_DATA_SIZE)); */
  /**** free(r12); KESTOCK 01/24/2001 ***/
  return(MDAS_SUCCESS);

}

void
get_ticket_id(char *obj_ticket_id, int isdata)
{
  int i, j, k, p;

  if (isdata == 1) obj_ticket_id[0] = 'D';
  else   obj_ticket_id[0] = 'C';
  for (i = 1; i < MDAS_TICKET_SIZE ; i++)
    {
      k =  random() % 62;
      if ( k < 10)
	{
	  obj_ticket_id[i] = '0' + k;
	}
      else if ( k < 36 )
 	{
	  obj_ticket_id[i] = 'a' + k-10;
	}
      else if ( k < 62 )
	{
	  obj_ticket_id[i] = 'A' + k-36;
	}
	 
    }
  obj_ticket_id[i] = '\0';
}

int not_good_ticket(char *obj_ticket_id)
{
  sprintf(query_string,
	 "select  ticket_id from %sMDAS_AU_TCKT_DATA where ticket_id = '%s'",
	   MDASSCHEMENAME, obj_ticket_id);
  if (check_exists(query_string) == 0) 
    return 1;
  sprintf(query_string,
	 "select  ticket_id from %sMDAS_AU_TCKT_GRP where ticket_id = '%s'",
	   MDASSCHEMENAME, obj_ticket_id);
  if (check_exists(query_string) == 0) 
    return 1;

  return 0;
}
int
get_user_ids_from_list(char *userSet, int *usercnt, int  newUserId[])
{
  int i,j,k, flag;
  char cv1[MAX_TOKEN], cv2[MAX_TOKEN];
  char uval[MAX_TOKEN], dval[MAX_TOKEN];
  char inval[MAX_TOKEN];

  i = 0;
  flag = 1;
  strcpy(inval, userSet);
  while (strlen(inval) > 0)
    {
      splitbychar(inval, cv1, cv2, '&');
      strcpy(inval, cv1);
      splitbychar(cv2,uval,dval, '@');
      j = get_user_id_in_domain(uval,dval);
      if (j < 0) { return (USER_NOT_IN_DOMN);}
      newUserId[i] = j;
      i++;
    }
  *usercnt = i;
  return 0;
}

int removeTicket(char *ticketId, char *userName, char *domainName)
{
  int i,j;
  
  j = get_user_id_in_domain(userName,domainName);
  if (j < 0) { return (USER_NOT_IN_DOMN);}
  sprintf(atname[0],"ticket_id");
  sprintf(atname[1],"ticket_owner");
  sprintf(atval[0],"'%s'",ticketId);
  sprintf(atval[1],"%i",j);
  sprintf(query_string,
	  "select  ticket_id from %sMDAS_AU_TCKT_DATA where ticket_id = '%s' and ticket_owner = %i",
	   MDASSCHEMENAME, ticketId, j);
  if (check_exists(query_string) != 0)    
    {
      sprintf(query_string,
	      "select  ticket_id from %sMDAS_AU_TCKT_GRP where ticket_id = '%s' and ticket_owner = %i",
	      MDASSCHEMENAME, ticketId, j);
      if (check_exists(query_string) != 0)  return (INVALID_TICKET);
      else
	{
	  i = deleteFromTable(atname, atval,"MDAS_AU_TCKT_GRP",2);
	  if (i != 0) return (MDAS_AU_TCKT_GRP_DELETION_ERROR);
	}
    }
  else
    {
      i = deleteFromTable(atname, atval,"MDAS_AU_TCKT_DATA",2);
      if (i != 0) return (MDAS_AU_TCKT_DATA_DELETION_ERROR);
    }
  removeTabledQueries(ticketId);
  return (MDAS_SUCCESS);
}

int
issueTicket(char *objName, char *collection, char *flag, 
	    char *begTime, char* endTime, int AccessCount,
	    char *userSet, char **myticketId, 
	    char *userName, char *domainName)
{
  int i, j, usercnt, recurflag;
  int    obj_data_id, obj_user_id, obj_access_id;
  int    replication_num;
  char   *ticketId, collectionId[MAX_TOKEN], tmpStr[1000];
  int    newUserId[100];
  char   newcollections[MAX_CTYPE_LIST_COUNT][MAX_TOKEN];
  char   timeval1[MAX_TOKEN],timeval2[MAX_TOKEN];
  int jj, colcount;
  char like_col_name[MAX_TOKEN * 2];
  if (userSet != NULL  && strcmp( userSet,""))
    {
      i = get_user_ids_from_list(userSet, &usercnt, newUserId);
      if (i != 0 ) return (i);
    }
  else
    {
      tabledIsOkay =  1;
      newUserId[0] = get_itype_attr_value(
			"MDAS_CD_USER.user_id",
			"MDAS_CD_USER.user_name", TICKET_USER);
      tabledIsOkay = 0;
      usercnt = 1;
    }
  if (begTime == NULL || strlen(begTime) < 1)
    { begTime = timeval1; get_time(begTime);}
  if (endTime == NULL || strlen(endTime) < 1)
    {endTime = timeval2; strcpy(endTime, "3999-12-31-23.55.55.000000");}
  tabledIsOkay =  1;
  obj_access_id = get_itype_attr_value(
			"MDAS_TD_DS_ACCS.access_id",
			"MDAS_TD_DS_ACCS.access_constraint", "read");
  tabledIsOkay =  0;
  if (obj_access_id < 0 ) return (obj_access_id);

  
  if (objName != NULL  && strcmp( objName,""))
    {
      ticketId =  (char *) malloc((MDAS_TICKET_SIZE+1) * sizeof(char));
      get_ticket_id(ticketId, 1);
      while(not_good_ticket(ticketId))
	get_ticket_id(ticketId,1);

      i =get_data_id_with_collection(objName, "","",
		     userName, collection,  domainName,
		     &obj_data_id, &replication_num, &obj_user_id);
      if (i != 0 ) return (i);
      
      for (j = 0; j < usercnt; j++)
	{
	  sprintf(atname[0],"ticket_id");
	  sprintf(atname[1],"data_id");
	  sprintf(atname[2],"user_id");
	  sprintf(atname[3],"begin_time");
	  sprintf(atname[4],"end_time");
	  sprintf(atname[5],"access_count");
	  sprintf(atname[6],"access_id");
	  sprintf(atname[7],"ticket_owner");
	  sprintf(atval[0],"'%s'",ticketId);
	  sprintf(atval[1],"%i",obj_data_id);
	  sprintf(atval[2],"%i",newUserId[j]);
	  sprintf(atval[3],"'%s'",begTime);
	  sprintf(atval[4],"'%s'",endTime);
	  sprintf(atval[5],"%i",AccessCount);
	  sprintf(atval[6],"%i",obj_access_id);
	  sprintf(atval[7],"%i",obj_user_id);
	  i = insertIntoTable(atname, atval,"MDAS_AU_TCKT_DATA",8);
	  if (i != 0) return (MDAS_AU_TCKT_DATA_INSERTION_ERROR);
	}
      *myticketId = ticketId;
      performAudit(D_TICKET,obj_user_id,obj_data_id,"",ticketId);
      return (MDAS_SUCCESS);
    }
  else
    {
      ticketId =  (char *) malloc((MDAS_TICKET_SIZE+1) * sizeof(char));
      get_ticket_id(ticketId, 0);
      while(not_good_ticket(ticketId))
	get_ticket_id(ticketId,0);
      if (i != 0 ) return (COLLECTION_NOT_IN_CAT);

      obj_user_id = get_user_id_in_domain(userName, domainName);
      if (obj_user_id < 0) { return (USER_NOT_IN_DOMN);}
      


      if (!strcmp(flag, "R")) 
	{
	  strcpy(like_col_name,collection);
	  mcatEscapeWildCards(like_col_name);
#ifdef MCAT_VERSION_10
	  sprintf(tmpStr, "select * from %sMDAS_TD_DATA_GRP t1 where t1.data_grp_name  like '%s%%' and t1.data_grp_name   not like '%%$deleted %%'",
		  MDASSCHEMENAME,  like_col_name);
#else
	  sprintf(tmpStr, "select * from %sMDAS_TD_DATA_GRP t1 where t1.data_grp_name  like '%s%%' and t1.data_grp_name   not like '%%$deleted'",
		  MDASSCHEMENAME,  like_col_name);
#endif
	  i = get_ctype_list_from_query(newcollections, &colcount,
			 tmpStr);
	  if (i != 0 ) return (COLLECTION_NOT_IN_CAT);
	  recurflag = 1;
	}
      else 
	{
	  recurflag = 0;
	  tabledIsOkay =  1;
	  i = get_ctype_value(newcollections[0],
			      "MDAS_TD_DATA_GRP.data_grp_name", collection);
	  tabledIsOkay =  0;
	  colcount =  1;
	}
    for (jj = 0; jj < colcount; jj++)
      {
       if (check_permit_in_group("all%", newcollections[jj], obj_user_id)==0)
	 {
	   for (j = 0; j < usercnt; j++)
	     {
	       sprintf(atname[0],"ticket_id");
	       sprintf(atname[1],"data_grp_id");
	       sprintf(atname[2],"user_id");
	       sprintf(atname[3],"begin_time");
	       sprintf(atname[4],"end_time");
	       sprintf(atname[5],"access_count");
	       sprintf(atname[6],"access_id");
	       sprintf(atname[7],"ticket_owner");
	       sprintf(atname[8],"is_recursive");
	       sprintf(atval[0],"'%s'",ticketId);
	       sprintf(atval[1],"'%s'",newcollections[jj]);
	       sprintf(atval[2],"%i",newUserId[j]);
	       sprintf(atval[3],"'%s'",begTime);
	       sprintf(atval[4],"'%s'",endTime);
	       sprintf(atval[5],"%i",AccessCount);
	       sprintf(atval[6],"%i",obj_access_id);
	       sprintf(atval[7],"%i",obj_user_id);
	       sprintf(atval[8],"%i", recurflag);
	       i = insertIntoTable(atname, atval,"MDAS_AU_TCKT_GRP",9);
	       if (i != 0) return (MDAS_AU_TCKT_GRP_INSERTION_ERROR);
	     }
	 }
       performAudit(C_TICKET,obj_user_id,0,newcollections[jj],ticketId);
      }
      *myticketId = ticketId;
      return (MDAS_SUCCESS);
    }
}

/* delete a Location */
#define ID_LIMIT 30
int
deleteValueLocation(char *delValue)
{
  int i,j;
  char tabNameChkId[ID_LIMIT][SMALL_TOKEN];
  char attrNameChkId[ID_LIMIT][SMALL_TOKEN];
  char extraChkId[ID_LIMIT][MAX_TOKEN];
  char msgChkId[ID_LIMIT][MAX_TOKEN];
  char tabNameDelId[ID_LIMIT][SMALL_TOKEN];
  char attrNameDelId[ID_LIMIT][SMALL_TOKEN];
  char tabNameDelName[ID_LIMIT][SMALL_TOKEN];
  char attrNameDelName[ID_LIMIT][SMALL_TOKEN];
  char userName[MAX_TOKEN], domainName[MAX_TOKEN];
  char *tmpPtr;


  int idType = 0; /* 0=int, 1 = string */
  int chkIdCnt = 0;
  int delNameCnt = 0;
  int idInt;
  char idVal[MAX_TOKEN];
  
  char sqlq[HUGE_STRING];
  
  for (i = 0; i < ID_LIMIT ; i++) {
    extraChkId[i][0] = '\0';
    msgChkId[i][0] = '\0';
  }
  idType = 1;
  strcpy(tabNameChkId[0],"MDAS_AR_REPL");
  strcpy(attrNameChkId[0],"locn_id");
  strcpy(tabNameChkId[1],"MDAS_AR_PHYSICAL");
  strcpy(attrNameChkId[1],"locn_id");
  chkIdCnt = 2;
  strcpy(tabNameDelName[0],"MDAS_TD_LOCN");
  strcpy(attrNameDelName[0],"locn_desc");
  delNameCnt = 1;

  sprintf(sqlq,"%s.%s",tabNameDelName[0],attrNameDelName[0]);
  if (idType == 0) {
    idInt = get_itype_value(sqlq,delValue);
    if (idInt < 0)
      return (NO_VALUE_FOUND);
    sprintf(idVal,"%i",idInt);
  }
  else {
    i = get_ctype_value(idVal, sqlq, delValue);
    if (i < 0) return (NO_VALUE_FOUND);
  }

  for (i = 0; i < chkIdCnt ; i++) {
    sprintf(sqlq, "select %s from %s%s where %s = '%s'",
	    attrNameChkId[i], MDASSCHEMENAME,tabNameChkId[i],
	    attrNameChkId[i], idVal);
    if (strlen(extraChkId[i]) > 0)
      strcat(sqlq,extraChkId[i]);
    if ( check_exists(sqlq) == 0) { /* something exists */ 
      return(ACTION_NOT_ALLOWED);
    }
  }

  for (i = 0; i < delNameCnt; i++) {
    sprintf(sqlq, "delete from %s%s where %s = '%s'",
	    MDASSCHEMENAME,tabNameDelName[i],attrNameDelName[i],delValue);
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) {
      return(res);
    }
  }
  return(MDAS_SUCCESS);
}

#if MYSMCAT
/* delete a User */
int
deleteValueUser(char *delValue)
{
  int i,j;
  char tabNameChkId[ID_LIMIT][SMALL_TOKEN];
  char attrNameChkId[ID_LIMIT][SMALL_TOKEN];
  char extraChkId[ID_LIMIT][MAX_TOKEN];
  char msgChkId[ID_LIMIT][MAX_TOKEN];
  char tabNameDelId[ID_LIMIT][SMALL_TOKEN];
  char attrNameDelId[ID_LIMIT][SMALL_TOKEN];
  char tabNameDelName[ID_LIMIT][SMALL_TOKEN];
  char attrNameDelName[ID_LIMIT][SMALL_TOKEN];
  char userName[MAX_TOKEN], domainName[MAX_TOKEN];
  char LikeuserName[MAX_TOKEN * 2], LikedomainName[MAX_TOKEN * 2];
  char *tmpPtr;

  char mys_buf[MAX_TOKEN];

  int idType = 0; /* 0=int, 1 = string */
  int chkIdCnt = 0;
  int delIdCnt = 0;
  int delNameCnt = 0;
  int idInt;
  char idVal[MAX_TOKEN];
  
  char sqlq[HUGE_STRING];
  char logMsg[MAX_TOKEN*2];

  for (i = 0; i < ID_LIMIT ; i++) {
    extraChkId[i][0] = '\0';
    msgChkId[i][0] = '\0';
  }

      idType = 0;
      strcpy(tabNameChkId[0],"MDAS_TD_LOCN");
      strcpy(attrNameChkId[0],"server_user_id");
      strcpy(tabNameChkId[1],"MDAS_AR_PHYSICAL");
      strcpy(attrNameChkId[1],"phy_owner");
      strcpy(tabNameChkId[2],"MDAS_AR_REPL");
      strcpy(attrNameChkId[2],"owner");
      strcpy(tabNameChkId[3],"MDAS_AD_REPL");
      strcpy(attrNameChkId[3],"data_owner");
      chkIdCnt = 4;
      strcpy(tabNameDelId[0],"MDAS_AU_INFO");
      strcpy(attrNameDelId[0],"user_id");
      strcpy(tabNameDelId[1],"MDAS_AU_GROUP");
      strcpy(attrNameDelId[1],"user_id");
      strcpy(tabNameDelId[2],"MDAS_AU_DOMN");
      strcpy(attrNameDelId[2],"user_id");
      strcpy(tabNameDelId[3],"MDAS_AU_AUTH_KEY");
      strcpy(attrNameDelId[3],"user_id");
      strcpy(tabNameDelId[4],"MDAS_AU_AUTH_MAP");
      strcpy(attrNameDelId[4],"user_id");
      strcpy(tabNameDelId[5],"MDAS_AU_AUDIT");
      strcpy(attrNameDelId[5],"user_id");
      strcpy(tabNameDelId[6],"MDAS_AU_TCKT_GRP");
      strcpy(attrNameDelId[6],"ticket_owner");
      strcpy(tabNameDelId[7],"MDAS_AR_ACCS");
      strcpy(attrNameDelId[7],"user_id");
      strcpy(tabNameDelId[8],"MDAS_AR_AUDIT");
      strcpy(attrNameDelId[8],"user_id");
      strcpy(tabNameDelId[9],"MDAS_AD_COMMENTS");
      strcpy(attrNameDelId[9],"user_id");
      strcpy(tabNameDelId[10],"MDAS_AD_ANNOTATION");
      strcpy(attrNameDelId[10],"user_id");
      strcpy(tabNameDelId[11],"MDAS_AD_ACCS");
      strcpy(attrNameDelId[11],"user_id");
      strcpy(tabNameDelId[12],"MDAS_AD_GRP_ACCS");
      strcpy(attrNameDelId[12],"user_id");
      strcpy(tabNameDelId[13],"MDAS_AD_AUDIT");
      strcpy(attrNameDelId[13],"user_id");
      strcpy(tabNameDelId[14],"MDAS_AU_TCKT_DATA");
      strcpy(attrNameDelId[14],"user_id");
      strcpy(tabNameDelId[15],"MDAS_AU_TCKT_DATA");
      strcpy(attrNameDelId[15],"ticket_owner");
      strcpy(tabNameDelId[16],"MDAS_AU_TCKT_GRP");
      strcpy(attrNameDelId[16],"user_id");
      strcpy(tabNameDelId[17],"MDAS_CD_USER");
      strcpy(attrNameDelId[17],"user_id");
      delIdCnt = 18;
      delNameCnt = 0; 


    strcpy(userName,delValue);
    if ((tmpPtr = strstr(userName,"@")) == NULL)
      return(DATA_VALUE_TYPE_ERROR);
    *tmpPtr = '\0';
    tmpPtr++;
    strcpy(domainName,tmpPtr);
    idInt = get_user_id_in_domain(userName, domainName);
    if (idInt < 0)
      return(idInt);
    sprintf(idVal,"%i",idInt);

  for (i = 0; i < chkIdCnt ; i++) {
    sprintf(sqlq, "select %s from %s%s where %s = %s",
	    attrNameChkId[i], MDASSCHEMENAME,tabNameChkId[i],
	    attrNameChkId[i], idVal);
    if (strlen(extraChkId[i]) > 0)
      strcat(sqlq,extraChkId[i]);
    if ( check_exists(sqlq) == 0) { /* something exists */ 
      sprintf(logMsg,"Error: Value Not Removed. Value exists in %s",
	      tabNameChkId[i]);
      elog(NOTICE, logMsg);
      return(ACTION_NOT_ALLOWED);
    }
  }
  for (i = 0; i < delIdCnt; i++) {
    sprintf(sqlq, "delete from %s%s where %s = %s",
	    MDASSCHEMENAME,tabNameDelId[i],attrNameDelId[i], idVal);
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) {
      sprintf(logMsg,"Error: Unable to delete Value from %s: %i",
	      tabNameDelId[i],res);
      elog(NOTICE, logMsg);
      return(res);
    }
  }
  for (i = 0; i < delNameCnt; i++) {
    sprintf(sqlq, "delete from %s%s where %s = '%s'",
	    MDASSCHEMENAME,tabNameDelName[i],attrNameDelName[i],delValue);
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) {
      sprintf(logMsg,"Error: Unable to delete Value from %s: %i",
	      tabNameDelId[i],res);
      elog(NOTICE, logMsg);
      return(res);
    }
  }
    sprintf(sqlq, "select DATA_GRP_ID from %sMDAS_TD_DATA_GRP where DATA_GRP_NAME = '/home/%s.%s'", MDASSCHEMENAME,userName, domainName);
    res = get_ctype_value_from_query(mys_buf, sqlq);
    if(res != 0)
    {
      sprintf(logMsg,"Error: Unable to perform %s: %i", sqlq,res);
      elog(NOTICE, logMsg);
      return(res);
    }
    sprintf(sqlq, "delete from %sMDAS_AD_GRP_ACCS where DATA_GRP_ID in ('%s')", MDASSCHEMENAME, mys_buf);
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) {
      sprintf(logMsg,"Error: Unable to perform %s: %i", sqlq,res);
      elog(NOTICE, logMsg);
      return(res);
    }

    sprintf(sqlq, "select DATA_GRP_ID from %sMDAS_TD_DATA_GRP where DATA_GRP_NAME like '/home/%s.%s/%%'", MDASSCHEMENAME,LikeuserName, LikedomainName);
    res = get_ctype_value_from_query(mys_buf, sqlq);
    if(res != 0)
    {
      sprintf(logMsg,"Error: Unable to perform %s: %i", sqlq,res);
      elog(NOTICE, logMsg);
      return(res);
    }
    sprintf(sqlq, "delete from %sMDAS_AD_GRP_ACCS where DATA_GRP_ID in ('%s')", MDASSCHEMENAME, mys_buf);
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) {
      sprintf(logMsg,"Error: Unable to perform %s: %i", sqlq,res);
      elog(NOTICE, logMsg);
      return(res);
    }

    sprintf(sqlq, "delete from %sMDAS_TD_DATA_GRP where DATA_GRP_NAME = '/home/%s.%s'",
	    MDASSCHEMENAME,userName, domainName);
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) {
      sprintf(logMsg,"Error: Unable to perform %s: %i", sqlq,res);
      elog(NOTICE, logMsg);
      return(res);
    }
    sprintf(sqlq, "delete from %sMDAS_TD_DATA_GRP where DATA_GRP_NAME like '/home/%s.%s/%%'",
	    MDASSCHEMENAME,LikeuserName, LikedomainName);
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) {
      sprintf(logMsg,"Error: Unable to perform %s: %i", sqlq,res);
      elog(NOTICE, logMsg);
      return(res);
    }


    sprintf(sqlq, "select DATA_GRP_ID from %sMDAS_TD_DATA_GRP where DATA_GRP_NAME = '/container/%s.%s'", MDASSCHEMENAME,userName, domainName);
    res = get_ctype_value_from_query(mys_buf, sqlq);
    if(res != 0)
    {
      sprintf(logMsg,"Error: Unable to perform %s: %i", sqlq,res);
      elog(NOTICE, logMsg);
      return(res);
    }
    sprintf(sqlq, "delete from %sMDAS_AD_GRP_ACCS where DATA_GRP_ID in ('%s')", MDASSCHEMENAME, mys_buf);
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) {
      sprintf(logMsg,"Error: Unable to perform %s: %i", sqlq,res);
      elog(NOTICE, logMsg);
      return(res);
    }
    sprintf(sqlq, "select DATA_GRP_ID from %sMDAS_TD_DATA_GRP where DATA_GRP_NAME like '/container/%s.%s/%%')", MDASSCHEMENAME,LikeuserName, LikedomainName);
    res = get_ctype_value_from_query(mys_buf, sqlq);
    if(res != 0)
    {
      sprintf(logMsg,"Error: Unable to perform %s: %i", sqlq,res);
      elog(NOTICE, logMsg);
      return(res);
    }
    sprintf(sqlq, "delete from %sMDAS_AD_GRP_ACCS where DATA_GRP_ID in ('%s')", MDASSCHEMENAME, mys_buf);
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) {
      sprintf(logMsg,"Error: Unable to perform %s: %i", sqlq,res);
      elog(NOTICE, logMsg);
      return(res);
    }
    sprintf(sqlq, "delete from %sMDAS_TD_DATA_GRP where DATA_GRP_NAME = '/container/%s.%s'",
	    MDASSCHEMENAME,userName, domainName);
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) {
      sprintf(logMsg,"Error: Unable to perform %s: %i", sqlq,res);
      elog(NOTICE, logMsg);
      return(res);
    }
    sprintf(sqlq, "delete from %sMDAS_TD_DATA_GRP where DATA_GRP_NAME like '/container/%s.%s/%%'",
	    MDASSCHEMENAME,LikeuserName, LikedomainName);
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) {
      sprintf(logMsg,"Error: Unable to perform %s: %i", sqlq,res);
      elog(NOTICE, logMsg);
      return(res);
    }
  
  return(MDAS_SUCCESS);
}
#else
/* delete a User */
int
deleteValueUser(char *delValue)
{
  int i,j;
  char tabNameChkId[ID_LIMIT][SMALL_TOKEN];
  char attrNameChkId[ID_LIMIT][SMALL_TOKEN];
  char extraChkId[ID_LIMIT][MAX_TOKEN];
  char msgChkId[ID_LIMIT][MAX_TOKEN];
  char tabNameDelId[ID_LIMIT][SMALL_TOKEN];
  char attrNameDelId[ID_LIMIT][SMALL_TOKEN];
  char tabNameDelName[ID_LIMIT][SMALL_TOKEN];
  char attrNameDelName[ID_LIMIT][SMALL_TOKEN];
  char userName[MAX_TOKEN], domainName[MAX_TOKEN];
  char LikeuserName[MAX_TOKEN * 2], LikedomainName[MAX_TOKEN * 2];
  char *tmpPtr;


  int idType = 0; /* 0=int, 1 = string */
  int chkIdCnt = 0;
  int delIdCnt = 0;
  int delNameCnt = 0;
  int idInt;
  char idVal[MAX_TOKEN];
  
  char sqlq[HUGE_STRING];
  char logMsg[MAX_TOKEN*2];

  for (i = 0; i < ID_LIMIT ; i++) {
    extraChkId[i][0] = '\0';
    msgChkId[i][0] = '\0';
  }

      idType = 0;
      strcpy(tabNameChkId[0],"MDAS_TD_LOCN");
      strcpy(attrNameChkId[0],"server_user_id");
      strcpy(tabNameChkId[1],"MDAS_AR_PHYSICAL");
      strcpy(attrNameChkId[1],"phy_owner");
      strcpy(tabNameChkId[2],"MDAS_AR_REPL");
      strcpy(attrNameChkId[2],"owner");
      strcpy(tabNameChkId[3],"MDAS_AD_REPL");
      strcpy(attrNameChkId[3],"data_owner");
      chkIdCnt = 4;
      strcpy(tabNameDelId[0],"MDAS_AU_INFO");
      strcpy(attrNameDelId[0],"user_id");
      strcpy(tabNameDelId[1],"MDAS_AU_GROUP");
      strcpy(attrNameDelId[1],"user_id");
      strcpy(tabNameDelId[2],"MDAS_AU_DOMN");
      strcpy(attrNameDelId[2],"user_id");
      strcpy(tabNameDelId[3],"MDAS_AU_AUTH_KEY");
      strcpy(attrNameDelId[3],"user_id");
      strcpy(tabNameDelId[4],"MDAS_AU_AUTH_MAP");
      strcpy(attrNameDelId[4],"user_id");
      strcpy(tabNameDelId[5],"MDAS_AU_AUDIT");
      strcpy(attrNameDelId[5],"user_id");
      strcpy(tabNameDelId[6],"MDAS_AU_TCKT_GRP");
      strcpy(attrNameDelId[6],"ticket_owner");
      strcpy(tabNameDelId[7],"MDAS_AR_ACCS");
      strcpy(attrNameDelId[7],"user_id");
      strcpy(tabNameDelId[8],"MDAS_AR_AUDIT");
      strcpy(attrNameDelId[8],"user_id");
      strcpy(tabNameDelId[9],"MDAS_AD_COMMENTS");
      strcpy(attrNameDelId[9],"user_id");
      strcpy(tabNameDelId[10],"MDAS_AD_ANNOTATION");
      strcpy(attrNameDelId[10],"user_id");
      strcpy(tabNameDelId[11],"MDAS_AD_ACCS");
      strcpy(attrNameDelId[11],"user_id");
      strcpy(tabNameDelId[12],"MDAS_AD_GRP_ACCS");
      strcpy(attrNameDelId[12],"user_id");
      strcpy(tabNameDelId[13],"MDAS_AD_AUDIT");
      strcpy(attrNameDelId[13],"user_id");
      strcpy(tabNameDelId[14],"MDAS_AU_TCKT_DATA");
      strcpy(attrNameDelId[14],"user_id");
      strcpy(tabNameDelId[15],"MDAS_AU_TCKT_DATA");
      strcpy(attrNameDelId[15],"ticket_owner");
      strcpy(tabNameDelId[16],"MDAS_AU_TCKT_GRP");
      strcpy(attrNameDelId[16],"user_id");
      strcpy(tabNameDelId[17],"MDAS_CD_USER");
      strcpy(attrNameDelId[17],"user_id");
      delIdCnt = 18;
      delNameCnt = 0; 


    strcpy(userName,delValue);
    if ((tmpPtr = strstr(userName,"@")) == NULL)
      return(DATA_VALUE_TYPE_ERROR);
    *tmpPtr = '\0';
    tmpPtr++;
    strcpy(domainName,tmpPtr);
    idInt = get_user_id_in_domain(userName, domainName);
    if (idInt < 0)
      return(idInt);
    sprintf(idVal,"%i",idInt);

  for (i = 0; i < chkIdCnt ; i++) {
    sprintf(sqlq, "select %s from %s%s where %s = %s",
	    attrNameChkId[i], MDASSCHEMENAME,tabNameChkId[i],
	    attrNameChkId[i], idVal);
    if (strlen(extraChkId[i]) > 0)
      strcat(sqlq,extraChkId[i]);
    if ( check_exists(sqlq) == 0) { /* something exists */ 
      sprintf(logMsg,"Error: Value Not Removed. Value exists in %s",
	      tabNameChkId[i]);
      elog(NOTICE, logMsg);
      return(ACTION_NOT_ALLOWED);
    }
  }
  for (i = 0; i < delIdCnt; i++) {
    sprintf(sqlq, "delete from %s%s where %s = %s",
	    MDASSCHEMENAME,tabNameDelId[i],attrNameDelId[i], idVal);
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) {
      sprintf(logMsg,"Error: Unable to delete Value from %s: %i",
	      tabNameDelId[i],res);
      elog(NOTICE, logMsg);
      return(res);
    }
  }
  for (i = 0; i < delNameCnt; i++) {
    sprintf(sqlq, "delete from %s%s where %s = '%s'",
	    MDASSCHEMENAME,tabNameDelName[i],attrNameDelName[i],delValue);
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) {
      sprintf(logMsg,"Error: Unable to delete Value from %s: %i",
	      tabNameDelId[i],res);
      elog(NOTICE, logMsg);
      return(res);
    }
  }
    sprintf(sqlq, "delete from %sMDAS_AD_GRP_ACCS where DATA_GRP_ID in (select DATA_GRP_ID from %sMDAS_TD_DATA_GRP where DATA_GRP_NAME = '/home/%s.%s')",
	    MDASSCHEMENAME,MDASSCHEMENAME,userName, domainName);
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) {
      sprintf(logMsg,"Error: Unable to perform %s: %i", sqlq,res);
      elog(NOTICE, logMsg);
      return(res);
    }
    sprintf(sqlq, "delete from %sMDAS_AD_GRP_ACCS where DATA_GRP_ID in (select DATA_GRP_ID from %sMDAS_TD_DATA_GRP where DATA_GRP_NAME like '/home/%s.%s/%%')",
	    MDASSCHEMENAME,MDASSCHEMENAME,LikeuserName, LikedomainName);
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) {
      sprintf(logMsg,"Error: Unable to perform %s: %i", sqlq,res);
      elog(NOTICE, logMsg);
      return(res);
    }
    sprintf(sqlq, "delete from %sMDAS_TD_DATA_GRP where DATA_GRP_NAME = '/home/%s.%s'",
	    MDASSCHEMENAME,userName, domainName);
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) {
      sprintf(logMsg,"Error: Unable to perform %s: %i", sqlq,res);
      elog(NOTICE, logMsg);
      return(res);
    }
    sprintf(sqlq, "delete from %sMDAS_TD_DATA_GRP where DATA_GRP_NAME like '/home/%s.%s/%%'",
	    MDASSCHEMENAME,LikeuserName, LikedomainName);
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) {
      sprintf(logMsg,"Error: Unable to perform %s: %i", sqlq,res);
      elog(NOTICE, logMsg);
      return(res);
    }


    sprintf(sqlq, "delete from %sMDAS_AD_GRP_ACCS where DATA_GRP_ID in (select DATA_GRP_ID from %sMDAS_TD_DATA_GRP where DATA_GRP_NAME = '/container/%s.%s')",
	    MDASSCHEMENAME,MDASSCHEMENAME,userName, domainName);
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) {
      sprintf(logMsg,"Error: Unable to perform %s: %i", sqlq,res);
      elog(NOTICE, logMsg);
      return(res);
    }
    sprintf(sqlq, "delete from %sMDAS_AD_GRP_ACCS where DATA_GRP_ID in (select DATA_GRP_ID from %sMDAS_TD_DATA_GRP where DATA_GRP_NAME like '/container/%s.%s/%%')",
	    MDASSCHEMENAME,MDASSCHEMENAME,LikeuserName, LikedomainName);
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) {
      sprintf(logMsg,"Error: Unable to perform %s: %i", sqlq,res);
      elog(NOTICE, logMsg);
      return(res);
    }
    sprintf(sqlq, "delete from %sMDAS_TD_DATA_GRP where DATA_GRP_NAME = '/container/%s.%s'",
	    MDASSCHEMENAME,userName, domainName);
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) {
      sprintf(logMsg,"Error: Unable to perform %s: %i", sqlq,res);
      elog(NOTICE, logMsg);
      return(res);
    }
    sprintf(sqlq, "delete from %sMDAS_TD_DATA_GRP where DATA_GRP_NAME like '/container/%s.%s/%%'",
	    MDASSCHEMENAME,LikeuserName, LikedomainName);
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) {
      sprintf(logMsg,"Error: Unable to perform %s: %i", sqlq,res);
      elog(NOTICE, logMsg);
      return(res);
    }
  
  return(MDAS_SUCCESS);
}
#endif

/* Delete a Physical Resource */
int
deleteValuePhyRes(char *delValue)
{
  int i,j;
  char tabNameChkId[ID_LIMIT][SMALL_TOKEN];
  char attrNameChkId[ID_LIMIT][SMALL_TOKEN];
  char extraChkId[ID_LIMIT][MAX_TOKEN];
  char msgChkId[ID_LIMIT][MAX_TOKEN];
  char tabNameDelId[ID_LIMIT][SMALL_TOKEN];
  char attrNameDelId[ID_LIMIT][SMALL_TOKEN];
  char tabNameDelName[ID_LIMIT][SMALL_TOKEN];
  char attrNameDelName[ID_LIMIT][SMALL_TOKEN];
  char userName[MAX_TOKEN], domainName[MAX_TOKEN];
  char *tmpPtr;


  int idType = 0; /* 0=int, 1 = string */
  int chkIdCnt = 0;
  int delIdCnt = 0;
  int delNameCnt = 0;
  int idInt;
  char idVal[MAX_TOKEN];
  
  char logMsg[MAX_TOKEN*2];

  char sqlq[HUGE_STRING];
  
  for (i = 0; i < ID_LIMIT ; i++) {
    extraChkId[i][0] = '\0';
    msgChkId[i][0] = '\0';
  }

      idType = 0;
      strcpy(tabNameChkId[0],"MDAS_TD_CONTAINER");
      strcpy(attrNameChkId[0],"rsrc_id");
      strcpy(msgChkId[0],"Container in the Resource");
      strcpy(tabNameChkId[1],"MDAS_AD_REPL");
      strcpy(attrNameChkId[1],"rsrc_id");
      strcpy(msgChkId[1],"SRB Data Object in the Resource");
      strcpy(tabNameChkId[2],"MDAS_AR_REPL");
      strcpy(attrNameChkId[2],"phy_rsrc_id");
      strcpy(msgChkId[2],"Logical Resource using the Resource");
      strcpy(extraChkId[2]," and phy_rsrc_id <> rsrc_id");
      chkIdCnt = 3;
      strcpy(tabNameDelId[0],"MDAS_AR_LRSRC_RRBN");
      strcpy(attrNameDelId[0],"phy_rsrc_id");
      strcpy(tabNameDelId[1],"MDAS_AR_ACCS");
      strcpy(attrNameDelId[1],"rsrc_id");
      strcpy(tabNameDelId[2],"MDAS_AR_AUDIT");
      strcpy(attrNameDelId[2],"rsrc_id");
      strcpy(tabNameDelId[3],"MDAS_AR_INFO");
      strcpy(attrNameDelId[3],"rsrc_id");
      strcpy(tabNameDelId[4],"MDAS_AR_DB_RSRC");
      strcpy(attrNameDelId[4],"rsrc_id");
      strcpy(tabNameDelId[5],"MDAS_AR_LRSRC_RRBN");
      strcpy(attrNameDelId[5],"rsrc_id");
      delIdCnt = 6;
      strcpy(tabNameDelName[0],"MDAS_AR_REPL");
      strcpy(attrNameDelName[0],"rsrc_name");
      strcpy(tabNameDelName[1],"MDAS_AR_PHYSICAL");
      strcpy(attrNameDelName[1],"phy_rsrc_name");
      strcpy(tabNameDelName[2],"MDAS_CD_RSRC");
      strcpy(attrNameDelName[2],"rsrc_name");
      delNameCnt = 3;

    sprintf(sqlq,"%s.%s",tabNameDelName[0],attrNameDelName[0]);
    if (idType == 0) {
      idInt = get_itype_value(sqlq,delValue);
      if (idInt < 0)
        return (NO_VALUE_FOUND);
      sprintf(idVal,"%i",idInt);
    }

  for (i = 0; i < chkIdCnt ; i++) {
    sprintf(sqlq, "select %s from %s%s where %s = %s",
	    attrNameChkId[i], MDASSCHEMENAME,tabNameChkId[i],
	    attrNameChkId[i], idVal);
    if (strlen(extraChkId[i]) > 0)
      strcat(sqlq,extraChkId[i]);
    if ( check_exists(sqlq) == 0) { /* something exists */ 
      sprintf(logMsg,"Error: Value Not Removed. Value exists in %s",
	      tabNameChkId[i]);
      elog(NOTICE, logMsg);
      return(ACTION_NOT_ALLOWED);
    }
  }
  for (i = 0; i < delIdCnt; i++) {
    sprintf(sqlq, "delete from %s%s where %s = %s",
	    MDASSCHEMENAME,tabNameDelId[i],attrNameDelId[i], idVal);
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) {
      sprintf(logMsg,"Error: Unable to delete Value from %s: %i\n",
	      tabNameDelId[i],res);
      elog(NOTICE, logMsg);
      return(res);
    }
  }
  for (i = 0; i < delNameCnt; i++) {
    sprintf(sqlq, "delete from %s%s where %s = '%s'",
	    MDASSCHEMENAME,tabNameDelName[i],attrNameDelName[i],delValue);
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) {
      sprintf(logMsg,"Error: Unable to delete Value from %s: %i\n",
	      tabNameDelId[i],res);
      elog(NOTICE, logMsg);
      return(res);
    }
  }
  
  return(MDAS_SUCCESS);
}



int
deleteValueInfo(int valueType, char *deleteValue)
{
  int status;

  if (valueType == DELETE_TYPE_LOCATION) {
    status = deleteValueLocation(deleteValue);
  }
  else if (valueType == DELETE_TYPE_USER) {
    status = deleteValueUser(deleteValue);
  }
  else if (valueType == DELETE_TYPE_RESOURCE) {
    status = deleteValuePhyRes(deleteValue);
  }
  else {
    status = CLI_FUNC_NOT_SUPPORTED;
  }
  return status;
}

int
insertResourceInfo (char *phyRsrcName,char *mlsec_latency_max,char *mlsec_latency_min,
		    char *mbps_bandwidth,char *concurrency_max,char *num_of_hierarchies,
		    char *num_of_stripes,char *megab_capacity,char *description,
		    char *username,char *domainname,char *userPassword)
{
  int i;
  int phy_rsrc_id;

  failure = 0;
  phy_rsrc_id = get_itype_value("MDAS_AR_PHYSICAL.phy_rsrc_name", phyRsrcName);
  if (failure != 0) 
    return (RESOURCE_NOT_IN_CAT);
  sprintf(sqlq, "select rsrc_id from %sMDAS_AR_INFO where rsrc_id = %i and mlsec_latency_max = %s and mlsec_latency_min = %s and mbps_bandwidth = %s and concurrency_max = %s and num_of_hierarchies = %s and  num_of_stripes = %s and megab_capacity = %s and (description = '%s' or description is NULL)",
	  MDASSCHEMENAME,phy_rsrc_id,mlsec_latency_max,mlsec_latency_min,mbps_bandwidth,
	  concurrency_max,num_of_hierarchies,num_of_stripes,megab_capacity,description);
  if (check_exists(sqlq) == 0)   {
     failure = INFO_ALREADY_IN_CAT;
     return (INFO_ALREADY_IN_CAT);
   }
  sprintf(sqlq, "insert into %sMDAS_AR_INFO (rsrc_id,repl_enum, mlsec_latency_max,mlsec_latency_min,mbps_bandwidth,concurrency_max,num_of_hierarchies,num_of_stripes,megab_capacity,description) values (%i,0, %s,%s,%s,%s,%s,%s,%s,'%s')",
	  MDASSCHEMENAME,phy_rsrc_id,mlsec_latency_max,mlsec_latency_min,mbps_bandwidth,
	  concurrency_max,num_of_hierarchies,num_of_stripes,megab_capacity,description);
  i = ExecSqlDb2(henv, hdbc, sqlq);
  if (i != 0)  
    return(MDAS_AR_INFO_INSERTION_ERROR);
  return(MDAS_SUCCESS);
}
#endif /* SRB_MDAS*/
