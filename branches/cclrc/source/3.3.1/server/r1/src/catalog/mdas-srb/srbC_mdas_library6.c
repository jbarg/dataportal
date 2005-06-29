/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

#ifdef SRB_MDAS

#include "mdasGlobalsExtern.h"
#include "elogExtern.h"
#include "srbMiscExtern.h"
#include "mdasPrototypes.h"
#include "srb.h"

#ifdef PSQMCAT
#define nextDataId "nextval('DataId')"
#define currDataId "currval('DataId')"
#endif /* PSQMCAT */
#ifdef ORAMCAT
#define nextDataId "DataId.NextVal"
#define currDataId "DataId.CurrVal"
#endif /* ORAMCAT */


int getDataId(char *collName, char *dataName)
{

 int i;
  if (collName == NULL || strlen(collName) == 0) {
    sprintf(sqlq, "select t1.data_id from %sMDAS_AD_REPL t1 where t1.data_name = '%s'  AND t1.is_deleted = 0  ", 
	  MDASSCHEMENAME,  dataName);
  }
  else {
    sprintf(sqlq, "select t1.data_id from %sMDAS_AD_REPL t1 , %sMDAS_TD_DATA_GRP t2 where t1.data_name = '%s'  AND t1.is_deleted = 0 AND t2.data_grp_name = '%s' and t2.data_grp_id = t1.data_grp_id ", 
	  MDASSCHEMENAME, MDASSCHEMENAME, dataName,collName);
  }
  tabledIsOkay = 1;
  i = get_itype_value_from_query(sqlq);
  tabledIsOkay = 0;
  if (i < 0)
    return(DATA_NOT_IN_COLLECTION);
  else 
    return(i);
}
int  check_unique_container(char *contName)
{
  

  sprintf(sqlq, "select * from %sMDAS_TD_CONTAINER t1 where t1.container_name = '%s'",    MDASSCHEMENAME,contName);
  tabledIsOkay = 1;
  if (check_exists(sqlq) == 0) 
    {
      tabledIsOkay = 0;
      failure = CONTAINER_NOT_UNIQUE;
      return (CONTAINER_NOT_UNIQUE);
    }
  tabledIsOkay = 0;
  return(MDAS_SUCCESS);
}

int check_permit_in_container(char *obj_access_name, int data_id,
			  int user_id)
{
   char access_name[MAX_TOKEN];  


     if (strstr(obj_access_name,"read") != NULL ||
       strstr(obj_access_name,"execute") != NULL ) strcpy( access_name,"read");
   else if (strstr(obj_access_name,"write") != NULL) 
     strcpy(access_name,"write");
   else strcpy(access_name,obj_access_name);

     
     /*** June 15, 2001 
     sprintf(sqlq, "select distinct t2.access_constraint from %sMDAS_AD_ACCS t1, %sMDAS_TD_DS_ACCS t2 where  t1.data_id = %i  AND t1.user_id = %i AND  t2.access_constraint like '%%%s%%' and t1.access_id >= t2.access_id",
	     MDASSCHEMENAME, MDASSCHEMENAME,
	     data_id, user_id, access_name);
     ***/
     sprintf(sqlq, "select distinct t2.access_constraint from %sMDAS_AD_ACCS t1, %sMDAS_TD_DS_ACCS t2, %sMDAS_AU_GROUP t3 where  t1.data_id = %i  AND t3.user_id = %i AND t3.group_user_id = t1.user_id AND  t2.access_constraint like '%%%s%%' and t1.access_id >= t2.access_id",
	     MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	     data_id, user_id, access_name);
     /*printf("CPG1:%i:%s\n",check_exists(sqlq),sqlq);*/
     tabledIsOkay = 1;
	if (check_exists(sqlq) != 0) 
	  {
	    tabledIsOkay = 0;
	    failure = NO_ACCS_TO_USER_IN_CONTAINER;
	    return (NO_ACCS_TO_USER_IN_CONTAINER);
	  }
	tabledIsOkay = 0;
	return(MDAS_SUCCESS);

}

int
check_type_compatibility_in_container(int contId, char *dataTypeId)
{     

  char cval[3][MAX_TOKEN];
  int i,rowcount;

  sprintf(sqlq, "select distinct t2.data_typ_id from %sMDAS_AD_REPL t1, %sMDAS_TD_DATA_TYP t2 where  t1.data_id = %i  AND t1.data_typ_id = t2.data_typ_id",
	     MDASSCHEMENAME, MDASSCHEMENAME,contId);
  tabledIsOkay = 1;
  i =  get_ctype_value_from_query((char *) cval, sqlq);
  tabledIsOkay = 0;
  if (i < 0) 
    return(i);
  if (!strcmp(cval[0],"0000"))  /* generic */
    return(MDAS_SUCCESS);
  else if ((char *)strstr(dataTypeId,cval[0]) == (char *)dataTypeId)
    return(MDAS_SUCCESS);
  else 
    return(DATA_TYPE_COMPATIBILITY_FAILURE);
}

/**** OPT
int
check_type_compatibility_in_container(int contId, char *dataTypeId)
{     

  char cval[USABLE_TABLE_COLS][MAX_TOKEN];
  int i,rowcount;

     sprintf(sqlq, "select distinct t2.data_typ_id, t2.data_typ_name from %sMDAS_AD_REPL t1, %sMDAS_TD_DATA_TYP t2 where  t1.data_id = %i  AND t1.data_typ_id = t2.data_typ_id",
	     MDASSCHEMENAME, MDASSCHEMENAME,contId);

     rowcount = 2;
    i = get_row_from_query(cval,&rowcount,sqlq);
    if (i < 0 || rowcount == 0) 
      return(i);

    if (!strcmp(cval[1],"generic")) 
      return(MDAS_SUCCESS);
    else if ((char *)strstr(dataTypeName,cval[0]) == (char *)dataTypeName)
      return(MDAS_SUCCESS);
    else 
      return(DATA_TYPE_COMPATIBILITY_FAILURE);
}
****/

int 
register_container_info(int catType, 
			char *containerName, 
			char *userName, 
			char *domainName, 
			char *resourceName, 
			srb_long_t containerMaxSize) 
{
  int  obj_action_id;
  int obj_id, repl_num, user_id, rsrc_id;
  char contName[MAX_TOKEN];
  char contColl[MAX_TOKEN];
  char timeStr[MAX_TOKEN];
   strcpy(reg_registrar_name,userName);
   strcpy(reg_obj_name, containerName);
   strcpy(reg_obj_code,"MDAS_CD_DATA");
   strcpy(reg_action_name,"create container");
   failure = 0;

   splitbychar(containerName, contColl,contName,'/');

   i =get_data_id_with_collection(contName,"","",userName,
		   contColl,domainName,
		   &obj_id, &repl_num, &user_id);
   if (i < 0 ) return (i);

  tabledIsOkay = 1;
   rsrc_id = get_itype_value("MDAS_CD_RSRC.rsrc_name", resourceName); 
  tabledIsOkay = 0;
   if (rsrc_id < 0) { return (RESOURCE_NOT_IN_CAT);}

   i = check_unique_container(containerName);
   if (i < 0 ) return (i);
   sprintf(query_string, 
     "insert into %s%s (CONTAINER_ID,CONTAINER_NAME,CONTAINER_MAX_SIZE,RSRC_ID) values (%i,'%s',%lld,%i)",
	  MDASSCHEMENAME, 
	  "MDAS_TD_CONTAINER", obj_id, containerName,containerMaxSize, rsrc_id );
   res = ExecSqlDb2(henv, hdbc, query_string);
   if (res != 0)  
    {        
      strcpy(GENERATEDSQL,query_string);
      sprintf(errmess,"container registry API: TD Containerinsertion error:%s:%s:%s",userName,containerName, resourceName);
      error_exit(errmess);
      return(MDAS_TD_CONTAINER_INSERTION_ERROR);
    }
    /**** added to make container_td_id of mdas_ad_repl to
        be the same as the container_id ****/
    
   sprintf(query_string, 
      "update %s%s set container_td_id = %i where data_id = %i",
         MDASSCHEMENAME, 
         "MDAS_AD_REPL", obj_id, obj_id);
    res = ExecSqlDb2(henv, hdbc, query_string);
    if (res != 0)  
     {        
       strcpy(GENERATEDSQL,query_string);
       sprintf(errmess,"container registry API: TD Containerinsertion error:%s:%s:%s",userName,containerName, resourceName);
       error_exit(errmess);
       return(MDAS_AD_REPL_UPDATE_ERROR);
     }
 

    performAudit(D_CONTAINER, user_id,obj_id,"","");
  return(MDAS_SUCCESS);
}

int
makeCollLink(char *srcParCollName, char *srcChCollName, 
	     char * trgCollId, int userId)
{
  int i,j;
  
  i = make_new_collection_by_id(0,srcParCollName,srcChCollName,userId);
  if (i !=0)  
    return (i);
  sprintf(atname[0],"data_grp_id");
  sprintf(atname[1],"coll_link");
  sprintf(atname[2],"data_grp_name");
  sprintf(atval[0],"'%s'",trgCollId);
  sprintf(atval[2],"'%s/%s'",srcParCollName,srcChCollName);
  for (j=1,i= -3; i < 0 && j < MAX_ALLOWED_COLL_LINKS ; j++) {
    sprintf(atval[1],"%i",j);
    i = changeAttrValueSet(atname, atval,"MDAS_TD_DATA_GRP",3,2);
  }
  if (j == MAX_ALLOWED_COLL_LINKS)
    elog(NOTICE,"Maximum Links (%i) to Collection Reached for %s:%i",
	 MAX_ALLOWED_COLL_LINKS,trgCollId,i);
  if (i < 0)
    return(MDAS_TD_DATA_GRP_UPDATE_ERROR);
  sprintf(query_string,"%s/%s LINKED TO %s",srcParCollName, srcChCollName,atval[0]);
  performAudit(D_LINK, userId,0,trgCollId,query_string);
  return(MDAS_SUCCESS);
}


int
makeDataLink(char *srcCollName, char *srcDataName, int trgDataId, int userId)
{
  int i, rsrc_id;
  srb_long_t containerSize;
  char containerName[HUGE_STRING];
   i =getDataInfo(trgDataId, atval);
   if (i < 0)   return (i);
  tabledIsOkay = 1;
  get_ctype_value(chval, "MDAS_TD_DATA_GRP.data_grp_name", srcCollName);
  if (failure != 0)
    return (COLLECTION_NOT_IN_CAT);
  i = check_permit_in_group("write",chval,userId);
  if (i !=0)  
    return (i);
  i = check_unique_dname_in_group(srcDataName,chval);
  if (i != 0)  
    return (DATA_NOT_UNIQUE_IN_COLLECTION);
  rsrc_id = get_itype_value("MDAS_CD_RSRC.rsrc_name", "null");
  if (i != 0)  
     return (i);
  tabledIsOkay = 0;
  get_time(cval[5]);
  if (failure != 0) { return (GET_TIME_ERROR);}

  sprintf(atval[15],"THIS IS A SLINK");


#if defined(ORAMCAT) || defined(PSQMCAT)
  sprintf(query_string, 
        "insert into %s%s (data_id,repl_enum,data_name,data_typ_id,path_name,rsrc_id,repl_timestamp,data_grp_id,data_size,is_deleted,data_owner,is_dirty,offset,container_id,modify_timestamp,last_accs_time,container_td_id,d_comment,datalock,lockowner,version_num,auditflag, seg_num, dlockexpiry, dchecksum, dhide) values (%s,-1,'%s','%s','%s',%i,'%s','%s',%s,0,%i,%s,%s,%s,'%s','%s',%s,'%s',%s,%s,'%s',%s,%s,'%s','%s',%s)",
	  MDASSCHEMENAME, 
	  "MDAS_AD_REPL",  nextDataId, srcDataName,
	  atval[3],
	  " ", rsrc_id,
	  cval[5], chval,
	  atval[8], userId,atval[11],atval[12],atval[13],cval[5],cval[5],
	  atval[14],atval[15],atval[16],atval[17],atval[18],atval[19],
	  atval[20],
	  atval[21],atval[22],atval[23]);

  ival[0] = DATAID_CURRVAL;
#else
  ival[0] = get_next_counter_value("DATA_ID");
  if (failure != 0) {
    return (DATA_ID_COUNTER_ERROR);
  }
  sprintf(query_string, 
        "insert into %s%s (data_id,repl_enum,data_name,data_typ_id,path_name,rsrc_id,repl_timestamp,data_grp_id,data_size,is_deleted,data_owner,is_dirty,offset,container_id,modify_timestamp,last_accs_time,container_td_id,d_comment,datalock,lockowner,version_num,auditflag, seg_num, dlockexpiry, dchecksum, dhide) values (%i,-1,'%s','%s','%s',%i,'%s','%s',%s,0,%i,%s,%s,%s,'%s','%s',%s,'%s',%s,%s,'%s',%s,%s,'%s','%s',%s)",
	  MDASSCHEMENAME, 
	  "MDAS_AD_REPL", ival[0],  srcDataName,
	  atval[3],
	  " ", rsrc_id,
	  cval[5], chval,
	  atval[8], userId,atval[11],atval[12],atval[13],cval[5],cval[5],
	  atval[14],atval[15],atval[16],atval[17],atval[18],atval[19],
	  atval[20],
	  atval[21],atval[22],atval[23]);
#endif /* ORAMCAT */

  i = ExecSqlDb2(henv, hdbc, query_string);
  if (i != 0)  {
    strcpy(GENERATEDSQL,query_string);
    return(MDAS_AD_REPL_INSERTION_ERROR);
  }
   
  /****** RAJA removed Dec 3, 2003 and on Srm check to see if it s linked object
      by checking the CONTAINER_ID  and DATA_ID *****
  sprintf(query_string, 
     "select data_grp_name from %sMDAS_TD_DATA_GRP where data_grp_id ='%s'",
	  MDASSCHEMENAME,atval[7]);
  get_ctype_value_from_query(cval[13],query_string);
  sprintf(containerName,"%s/%s",cval[13],atval[2]);
  
  if ( check_unique_container(containerName) == MDAS_SUCCESS) {
      containerSize = -1;
    sprintf(query_string, 
	   "insert into %s%s (CONTAINER_ID,CONTAINER_NAME,CONTAINER_MAX_SIZE,RSRC_ID) values (%s,'%s',%lld,%s)", MDASSCHEMENAME,  "MDAS_TD_CONTAINER", 
	   atval[13], containerName, containerSize, atval[5] );
   res = ExecSqlDb2(henv, hdbc, query_string);
   if (res != 0)  {        
     strcpy(GENERATEDSQL,query_string);
     return(MDAS_TD_CONTAINER_INSERTION_ERROR);
   }
  }
    RAJA removed Dec 3, 2003 and replaced with above *****/

#if defined(ORAMCAT) || defined(PSQMCAT)
    sprintf(query_string,
            "insert into %s%s (data_id,metadatanum) values (%s,-1)",MDASSCHEMENAME,"MDAS_AD_MDATA", currDataId);
#else
    sprintf(query_string,
            "insert into %s%s (data_id,metadatanum) values (%i,-1)",MDASSCHEMENAME,"MDAS_AD_MDATA",ival[0]);
#endif /* ORAMCAT */
    res = ExecSqlDb2(henv, hdbc, query_string);
    if (res != 0)
      return(MDAS_AD_MDATA_INSERTION_ERROR);

  sprintf(query_string,"%s/%s LINKED TO %s",srcCollName, srcDataName,atval[0]);
  performAudit(D_LINK, userId,ival[0],"",query_string);
  return(MDAS_SUCCESS);
}


int 
registerIncontainerDatasetInBulk (int catType, 
				  char *userName,
				  char *domainName,
				  char *containerName,
				  mdasC_sql_result_struct *inDataInfo)
{

  int  i,ii, obj_action_id;
  int contId;
  char contName[MAX_TOKEN];
  char contColl[MAX_TOKEN];
  char genDataType[MAX_TOKEN];
  char parCollName[MAX_TOKEN], newCollName[MAX_TOKEN];
  int user_id, rsrc_id, new_data_id;
  int newCollFlag = 0;
  char *dataName, *collName, *dataSize, *offSet, *oldColl;
  srb_long_t idataSize;
  srb_long_t ioffSet;
  char *checkSumStr;
  char *commentStr;
  char emptyStr[] = "";
  char tmpDataName[MAX_TOKEN];
  char *createTimeStamp, *modifyTimeStamp;
  char cTS[SMALL_TOKEN], mTS[SMALL_TOKEN];

  splitbychar(containerName, contColl,contName,'/');
  if (isContainerByName(contName,contColl) == MDAS_ZERO) {
    return(registerNormalDatasetInBulk (catType,userName,
					domainName, containerName,
					/* XXXX added 1 noCheckFlag 9/28/04 */
					inDataInfo, 1));
  }
  strcpy(reg_registrar_name,userName);
  strcpy(reg_obj_code,"MDAS_CD_DATA");
  strcpy(reg_action_name,"create dataset");
  failure = 0;
  genDataType[0] = '\0';
  ival[0] = DATAID_CURRVAL;
  tabledIsOkay = 1;
  user_id = get_user_id_in_domain(userName, domainName);
  if (user_id < 0) { return (USER_NOT_IN_DOMN);}
  rsrc_id = get_itype_value("MDAS_CD_RSRC.rsrc_name", "null");
  if (failure != 0) { return (RESOURCE_NOT_IN_CAT);}
  get_ctype_value(genDataType, "MDAS_TD_DATA_TYP.data_typ_name", "generic");
  if (failure != 0) return(i);
  contId = getDataId(contColl,contName);
  if (contId < 0 ) {
    elog(NOTICE,"registerIncontainerDatasetInBulk: Unable to find Container: %s/%s: Error :%i",contColl,contName,contId);
    elog(NOTICE,"registerIncontainerDatasetInBulk: First Value in BulkLoad: %s|%s|%s|%s",
	 inDataInfo->sqlresult[0].values,inDataInfo->sqlresult[1].values,
	 inDataInfo->sqlresult[2].values,inDataInfo->sqlresult[3].values);
    return (contId);
  }
  i = check_permit_in_container("write", contId,user_id); /*min write permit*/
  if (i != 0 ) {
    elog(NOTICE,"registerIncontainerDatasetInBulk: Write Permission in Container is not allowed: %s/%s. Error = %i",contColl,contName,i);
    return (i);
  }
  tabledIsOkay = 0;
  failure = 0;
  get_time(cval[5]);
  if (failure != 0) { return (GET_TIME_ERROR);}

  reg_registrar_id = user_id;
  reg_obj_repl_enum = 0;
  strcpy(reg_timeval,cval[5]);

  /* XXXXX changed by MW to take out hardwired index
  dataName = (char *) inDataInfo->sqlresult[0].values;
  collName = (char *) inDataInfo->sqlresult[1].values;
  dataSize = (char *) inDataInfo->sqlresult[2].values;
  offSet   = (char *) inDataInfo->sqlresult[3].values;
  XXXXXX */
  dataName = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) inDataInfo,
            dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
  collName = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) inDataInfo,
            dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
  dataSize = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) inDataInfo,
            dcs_tname[SIZE], dcs_aname[SIZE]);
  offSet = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) inDataInfo,
            dcs_tname[OFFSET], dcs_aname[OFFSET]);
  checkSumStr = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) inDataInfo,
            dcs_tname[DATA_CHECKSUM], dcs_aname[DATA_CHECKSUM]);
  commentStr = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) inDataInfo,
            dcs_tname[DATA_COMMENTS], dcs_aname[DATA_COMMENTS]);
  createTimeStamp  = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) inDataInfo,
            dcs_tname[DATA_CREATE_TIMESTAMP], dcs_aname[DATA_CREATE_TIMESTAMP]);
  modifyTimeStamp  = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) inDataInfo,
            dcs_tname[REPL_TIMESTAMP], dcs_aname[REPL_TIMESTAMP]);
  if (checkSumStr == NULL)
    checkSumStr = emptyStr;
  if (commentStr == NULL)
    commentStr = emptyStr;
  oldColl = genDataType;

  for (ii = 0; ii < inDataInfo->row_count ; ii++) { /*** begin the loop ***/
    make_quote_to_two_quote(dataName,tmpDataName);
    strcpy(dataName,tmpDataName);
    make_quote_to_two_quote(collName,tmpDataName);
    strcpy(collName,tmpDataName);
    if (ii == 0 || strcmp(oldColl,collName) ) { /* is coll diff  */
      tabledIsOkay = 1;
      get_ctype_value(cval[13],
		      "MDAS_TD_DATA_GRP.data_grp_name",collName);
      tabledIsOkay = 0;
      if (failure != 0) {/* it is a new collection */
	splitbychar(collName, parCollName, newCollName,'/');
	i =  make_new_collection_path( catType, parCollName, 
			newCollName, userName, domainName);
	if (i < 0) { 
	  error_exit("Error in Creating Collection");
	  elog(NOTICE,"registerIncontainerDatasetInBulk: Error in Creating Collection: %s. Error = %i",collName,i);
	  
	  return (i);
	}
	get_ctype_value(cval[13],
		      "MDAS_TD_DATA_GRP.data_grp_name",collName);
	if (i < 0) { 
	  error_exit("Error in Getting Collection Id");
  elog(NOTICE,"registerIncontainerDatasetInBulk: Error in Getting Collection Id for: %s. Error = %i",collName,i);
	  return (i);
	}
	newCollFlag = 1;
      }
      else {
	tabledIsOkay = 1;
	newCollFlag = 0;
	i = check_permit_in_group("write",  cval[13], user_id);
	tabledIsOkay = 0;
	if (i !=0)  { 
	  error_exit("Error in Checking Permit in Collection");
	  elog(NOTICE,"registerIncontainerDatasetInBulk: Error in Checking Permission in Collection : %s. Error = %i",collName,i);
	  return (i);
	}
      }
      oldColl = collName;
    }
    /*****
    if (!newCollFlag) {
      tabledIsOkay = 1;
      i = check_unique_dname_in_group(dataName,  cval[13]);
      tabledIsOkay = 0;
      if (i != 0)   { 
	error_exit("Error in Registering InContainer Dataset");
	elog(NOTICE,"registerIncontainerDatasetInBulk: Data Name NOt Unique in Collection : %s for data: %s. Error = %i",collName,dataName,i);
	return (DATA_NOT_UNIQUE_IN_COLLECTION);  
      }
    }
    ******/
    idataSize = strtoll (dataSize, 0, 0);
    ioffSet = atol(offSet);
    if (createTimeStamp == NULL)
      strcpy(cTS,cval[5]);
    else
      strcpy(cTS,createTimeStamp);
    if (modifyTimeStamp == NULL)
      strcpy(mTS,cval[5]);
    else
      strcpy(mTS,modifyTimeStamp);

#if defined(ORAMCAT) || defined(PSQMCAT)
    sprintf(query_string, 
	    "insert into %s%s (data_id,repl_enum,data_name,data_typ_id,path_name,rsrc_id,repl_timestamp,data_grp_id,data_size,is_deleted,data_owner,is_dirty,offset,container_id,modify_timestamp,last_accs_time,datalock,lockowner,version_num,container_td_id,dchecksum,d_comment) values (%s,-1,'%s','%s','%s',%i,'%s','%s',%lld,0,%i,%i,%lld,%i,'%s','%s',0,0,'0',%i,'%s','%s')",
	    MDASSCHEMENAME, "MDAS_AD_REPL", nextDataId,  dataName, genDataType," ", rsrc_id,
	    cTS, cval[13],idataSize, user_id,0,ioffSet,contId,
	    mTS,cval[5],contId, checkSumStr,commentStr);
#else
  ival[0] = get_next_counter_value("DATA_ID");
  if (failure != 0) {
    return (DATA_ID_COUNTER_ERROR);
  }
    sprintf(query_string, 
	    "insert into %s%s (data_id,repl_enum,data_name,data_typ_id,path_name,rsrc_id,repl_timestamp,data_grp_id,data_size,is_deleted,data_owner,is_dirty,offset,container_id,modify_timestamp,last_accs_time,datalock,lockowner,version_num,container_td_id,dchecksum,d_comment) values (%i,-1,'%s','%s','%s',%i,'%s','%s',%lld,0,%i,%i,%lld,%i,'%s','%s',0,0,'0',%i,'%s','%s')",
	    MDASSCHEMENAME, "MDAS_AD_REPL", ival[0],  dataName, genDataType," ", rsrc_id,
	    cTS, cval[13],idataSize, user_id,0,ioffSet,contId,
	    mTS,cval[5],contId, checkSumStr,commentStr);
#endif /* ORAMCAT */

    i = ExecSqlDb2(henv, hdbc, query_string);
    if (i != 0) {
      /** RAJA June 24,2004 added to move the file to a new name if it already exists ***/
      i =  getDataId(collName,  dataName);
      if (i >= 0)   {
	/* moving the file to a new name */
	sprintf(tmpDataName,"%s.inContBulk.%s",dataName,cval[5]);
	sprintf(sqlq, "update %s%s set data_name='%s' where data_id = %i",
		MDASSCHEMENAME, "MDAS_AD_REPL", tmpDataName, i);
	i = ExecSqlDb2(henv, hdbc, sqlq);
	if (i != 0) {
	  strcpy(GENERATEDSQL,query_string);
	  error_exit("Error in Registering InContainer Dataset");
	  elog(NOTICE,"registerIncontainerDatasetInBulk: Error in registering:SQL=%s",query_string);
	  return(MDAS_AD_REPL_INSERTION_ERROR);
	}
	i = ExecSqlDb2(henv, hdbc, query_string);
      }
      if ( i != 0)  {
      strcpy(GENERATEDSQL,query_string);
      error_exit("Error in Registering InContainer Dataset");
      elog(NOTICE,"registerIncontainerDatasetInBulk: Error in registering:SQL=%s",query_string);
      return(MDAS_AD_REPL_INSERTION_ERROR);
      }
      /** RAJA June 24,2004 added to move the file to a new name if it already exists ***/
    }
    res = insertGuidBySRB(ival[0]);
    if (res != 0) 
      return(res);

#if defined(ORAMCAT) || defined(PSQMCAT)
    sprintf(query_string,
	    "insert into %s%s (data_id,metadatanum) values (%s,-1)",MDASSCHEMENAME,"MDAS_AD_MDATA", currDataId);
#else
    sprintf(query_string,
	    "insert into %s%s (data_id,metadatanum) values (%i,-1)",MDASSCHEMENAME,"MDAS_AD_MDATA",ival[0]);
#endif /* ORAMCAT */
    res = ExecSqlDb2(henv, hdbc, query_string);
    if (res != 0)
      return(MDAS_AD_MDATA_INSERTION_ERROR);
    performAudit(D_CREATE,user_id,ival[0],"","registerIncontainerDatasetInBulk");    
    dataName += MAX_DATA_SIZE;
    collName += MAX_DATA_SIZE;
    dataSize += MAX_DATA_SIZE;
    offSet   += MAX_DATA_SIZE;
    if (checkSumStr != emptyStr)
      checkSumStr += MAX_DATA_SIZE;
    if (commentStr != emptyStr)
      commentStr += MAX_DATA_SIZE;
    if (createTimeStamp != NULL)
      createTimeStamp  += MAX_DATA_SIZE;
    if (modifyTimeStamp != NULL)
      modifyTimeStamp += MAX_DATA_SIZE;
  }
    performAudit(D_BULKINGEST,user_id,contId,"",cval[5]);

    return(MDAS_SUCCESS);
}

int 
registerNormalDatasetInBulk (int catType, 
				  char *userName,
				  char *domainName,
				  char *rsrcName,
				  mdasC_sql_result_struct *inDataInfo,
				  int noCheckFlag)
{

  int  i,ii, obj_action_id ;
  int contId;
  char contName[MAX_TOKEN];
  char contColl[MAX_TOKEN];
  char genDataType[MAX_TOKEN], myDataType[MAX_TOKEN];
  char parCollName[MAX_TOKEN], newCollName[MAX_TOKEN];
  int user_id, rsrc_id, new_data_id, accessId;
  int newCollFlag = 0;
  char *dataName, *collName, *dataSize, *dataType, *oldColl, *pathName;
  char *dtPtr;
  srb_long_t idataSize;
  srb_long_t ioffSet;
  char *checkSumStr;
  char *commentStr;
  char emptyStr[] = "";
  char tmpDataName[MAX_TOKEN];
  char *createTimeStamp, *modifyTimeStamp;
  char cTS[SMALL_TOKEN], mTS[SMALL_TOKEN];

  if (noCheckFlag == 0 && ALLOW_NON_CHECKING_REGISTERY_OF_DATA == 0) {
      elog(NOTICE,
       "noCheckFlag = %i.  ALLOW_NON_CHECKING_REGISTERY_OF_DATA = %i",
       noCheckFlag,ALLOW_NON_CHECKING_REGISTERY_OF_DATA);
      return(ACTION_NOT_ALLOWED);
  }

  strcpy(reg_registrar_name,userName);
  strcpy(reg_obj_code,"MDAS_CD_DATA");
  strcpy(reg_action_name,"create dataset");
  failure = 0;
  genDataType[0] = '\0';
  ival[0] = DATAID_CURRVAL;
  tabledIsOkay = 1;
  user_id = get_user_id_in_domain(userName, domainName);
  if (user_id < 0) { return (USER_NOT_IN_DOMN);}
  get_ctype_value(genDataType, "MDAS_TD_DATA_TYP.data_typ_name", "generic");
  if (failure != 0) return(failure);
  rsrc_id = getResourceIdWithAccessToUserId("write", rsrcName, user_id);
    if (rsrc_id < 0)  return (rsrc_id);
  accessId  = get_itype_value("MDAS_TD_DS_ACCS.access_constraint",
			    "all");
  if (failure != 0) { return (ACCESS_TYPE_NOT_IN_CAT);}

  tabledIsOkay = 0;
  get_time(cval[5]);
  if (failure != 0) { return (GET_TIME_ERROR);}

  reg_registrar_id = user_id;
  reg_obj_repl_enum = 0;
  strcpy(reg_timeval,cval[5]);

  /* XXXXX changed by MW to take out hardwired index
  dataName = (char *) inDataInfo->sqlresult[0].values;
  collName = (char *) inDataInfo->sqlresult[1].values;
  dataSize = (char *) inDataInfo->sqlresult[2].values;
  dataType   = (char *) inDataInfo->sqlresult[3].values;
  pathName  = (char *) inDataInfo->sqlresult[4].values;
   XXXXXXX */
  dataName = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) inDataInfo,
            dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
  collName = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) inDataInfo,
            dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
  dataSize = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) inDataInfo,
            dcs_tname[SIZE], dcs_aname[SIZE]);
  dataType = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) inDataInfo,
            dcs_tname[DATA_TYP_NAME], dcs_aname[DATA_TYP_NAME]);
  pathName = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) inDataInfo,
            dcs_tname[PATH_NAME], dcs_aname[PATH_NAME]);
  checkSumStr = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) inDataInfo,
            dcs_tname[DATA_CHECKSUM], dcs_aname[DATA_CHECKSUM]);
  commentStr = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) inDataInfo,
            dcs_tname[DATA_COMMENTS], dcs_aname[DATA_COMMENTS]);
  createTimeStamp  = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) inDataInfo,
            dcs_tname[DATA_CREATE_TIMESTAMP], dcs_aname[DATA_CREATE_TIMESTAMP]);
  modifyTimeStamp  = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) inDataInfo,
            dcs_tname[REPL_TIMESTAMP], dcs_aname[REPL_TIMESTAMP]);

  if (checkSumStr == NULL)
    checkSumStr = emptyStr;
  if (commentStr == NULL)
    commentStr = emptyStr;

  oldColl = genDataType;

  for (ii = 0; ii < inDataInfo->row_count ; ii++) { /*** begin the loop ***/
    make_quote_to_two_quote(pathName,tmpDataName);
    strcpy(pathName,tmpDataName);
    make_quote_to_two_quote(dataName,tmpDataName);
    strcpy(dataName,tmpDataName);
    make_quote_to_two_quote(collName,tmpDataName);
    strcpy(collName,tmpDataName);
    make_quote_to_two_quote(commentStr,tmpDataName);
    strcpy(commentStr,tmpDataName);
    if (ii == 0 || strcmp(oldColl,collName) ) { /* is coll diff  */
      tabledIsOkay = 1;
      get_ctype_value(cval[13],
		      "MDAS_TD_DATA_GRP.data_grp_name",collName);
      tabledIsOkay = 0;
      if (failure != 0) {/* it is a new collection */
	splitbychar(collName, parCollName, newCollName,'/');
	i =  make_new_collection_path( catType, parCollName, 
			newCollName, userName, domainName);
	if (i < 0) { 
	  error_exit("Error in Creating Collection");
	  elog(NOTICE,"registerNormalDatasetInBulk: Error in Creating Collection: %s. Error = %i",collName,i);
	  return (i);
	}
	get_ctype_value(cval[13],
		      "MDAS_TD_DATA_GRP.data_grp_name",collName);
	if (i < 0) { 
	  error_exit("Error in Getting Collection Id");
	  elog(NOTICE,"registerNormaDatasetInBulk: Error in Getting Collection Id for: %s. Error = %i",collName,i);
	  return (i);
	}
	newCollFlag = 1;
      }
      else {
	tabledIsOkay = 1;
	newCollFlag = 0;
	i = check_permit_in_group("write",  cval[13], user_id);
	tabledIsOkay = 0;
	if (i !=0)  { 
	  error_exit("Error in Checking Permit in Collection");
	  elog(NOTICE,"registerNormalDatasetInBulk: Error in Checking Permission in Collection : %s. Error = %i",collName,i);
	  return (i);
	}
      }
      oldColl = collName;
    }
    idataSize = strtoll (dataSize, 0, 0);
    if (strlen(dataType) > 0 ) {
      get_ctype_value(myDataType, "MDAS_TD_DATA_TYP.data_typ_name",dataType);
      if (failure != 0) 
	dtPtr = genDataType;
      else
	dtPtr = myDataType;
    }
    else {
      dtPtr = genDataType;
    }
    failure = 0;

    if (createTimeStamp == NULL)
      strcpy(cTS,cval[5]);
    else
      strcpy(cTS,createTimeStamp);
    if (modifyTimeStamp == NULL)
      strcpy(mTS,cval[5]);
    else
      strcpy(mTS,modifyTimeStamp);

#if defined(ORAMCAT) || defined(PSQMCAT)
    sprintf(query_string, 
	    "insert into %s%s (data_id,repl_enum,data_name,data_typ_id,path_name,rsrc_id,repl_timestamp,data_grp_id,data_size,is_deleted,data_owner,is_dirty,offset,container_id,modify_timestamp,last_accs_time,datalock,lockowner,version_num,container_td_id,dchecksum,d_comment) values (%s,0,'%s','%s','%s',%i,'%s','%s',%lld,%i,%i,%i,0,%s,'%s','%s',0,0,'0',0,'%s','%s')",
	    MDASSCHEMENAME, "MDAS_AD_REPL",nextDataId,   dataName, dtPtr,pathName, rsrc_id,
	    cTS, cval[13],idataSize, 0, user_id,0,currDataId,
	    mTS,cval[5], checkSumStr,commentStr);
#else
  ival[0] = get_next_counter_value("DATA_ID");
  if (failure != 0) {
    return (DATA_ID_COUNTER_ERROR);
  }
    sprintf(query_string, 
	    "insert into %s%s (data_id,repl_enum,data_name,data_typ_id,path_name,rsrc_id,repl_timestamp,data_grp_id,data_size,is_deleted,data_owner,is_dirty,offset,container_id,modify_timestamp,last_accs_time,datalock,lockowner,version_num,container_td_id,dchecksum,d_comment) values (%i,-1,'%s','%s','%s',%i,'%s','%s',%lld,%i,%i,%i,0,%i,'%s','%s',0,0,'0',0,'%s','%s')",
	    MDASSCHEMENAME, "MDAS_AD_REPL", ival[0],  dataName, 
	    dtPtr,pathName, rsrc_id,
	    cTS, cval[13],idataSize, 0, user_id,0,ival[0],
	    mTS,cval[5], checkSumStr,commentStr);
#endif /* ORAMCAT */

    i = ExecSqlDb2(henv, hdbc, query_string);
    if (i != 0) {
      /** RAJA June 24,2004 added to move the file to a new name if it already exists ***/
      i =  getDataId(collName,  dataName);
      if (i >= 0)   {
	/* moving the file to a new name */
	sprintf(tmpDataName,"%s.normalBulk.%s",dataName,cval[5]);
	sprintf(sqlq, "update %s%s set data_name='%s' where data_id = %i",
		MDASSCHEMENAME, "MDAS_AD_REPL", tmpDataName, i);
	i = ExecSqlDb2(henv, hdbc, sqlq);
	if (i != 0) {
	  strcpy(GENERATEDSQL,query_string);
	  error_exit("Error in Registering InContainer Dataset");
	  elog(NOTICE,"registerNormalDatasetInBulk: Error in registering:SQL=%s",query_string);
	  return(MDAS_AD_REPL_INSERTION_ERROR);
	}
	i = ExecSqlDb2(henv, hdbc, query_string);
      }
      if ( i != 0)  {
	strcpy(GENERATEDSQL,query_string);
	elog(NOTICE,"registerNormalDatasetInBulk: Error in registering:SQL=%s",query_string);
	return(MDAS_AD_REPL_INSERTION_ERROR);
      }
      /** RAJA June 24,2004 added to move the file to a new name if it already exists ***/
    }
#if defined(ORAMCAT) || defined(PSQMCAT)
  sprintf(query_string, 
	  "insert into %s%s (data_id,repl_enum,user_id,access_id) values (%s,0,%i,%i)",MDASSCHEMENAME, 
	  "MDAS_AD_ACCS", currDataId,user_id,accessId);
#else
  sprintf(query_string, 
	  "insert into %s%s (data_id,repl_enum,user_id,access_id) values (%i,0,%i,%i)",MDASSCHEMENAME, 
	  "MDAS_AD_ACCS", ival[0], user_id,accessId);
#endif /* ORAMCAT */
  i = ExecSqlDb2(henv, hdbc, query_string);
  if (i != 0) {
    elog(NOTICE,"registerNormalDatasetInBulk: Error in inserting Access Control:SQL=%s",query_string);
    return(MDAS_AD_ACCS_INSERTION_ERROR);
  }
    res = insertGuidBySRB(ival[0]);
    if (res != 0) 
      return(res);
    res = inheritACLWithSticky( cval[13],ival[0],0, currDataId,user_id, "single-data-register");
    if (res != 0)
      return(res);
#if defined(ORAMCAT) || defined(PSQMCAT)
    sprintf(query_string,
            "insert into %s%s (data_id,metadatanum) values (%s,-1)",MDASSCHEMENAME,"MDAS_AD_MDATA", currDataId);
#else
    sprintf(query_string,
            "insert into %s%s (data_id,metadatanum) values (%i,-1)",MDASSCHEMENAME,"MDAS_AD_MDATA",ival[0]);
#endif /* ORAMCAT */
res = ExecSqlDb2(henv, hdbc, query_string);
    if (res != 0)
      return(MDAS_AD_MDATA_INSERTION_ERROR);
    performAudit(D_CREATE,user_id,ival[0],"","registerNormalDatasetInBulk");
    dataName += MAX_DATA_SIZE;
    collName += MAX_DATA_SIZE;
    dataSize += MAX_DATA_SIZE;
    dataType += MAX_DATA_SIZE;
    pathName += MAX_DATA_SIZE;
    if (checkSumStr != emptyStr)
      checkSumStr += MAX_DATA_SIZE;
    if (commentStr != emptyStr)
      commentStr += MAX_DATA_SIZE;
    if (createTimeStamp != NULL)
      createTimeStamp  += MAX_DATA_SIZE;
    if (modifyTimeStamp != NULL)
      modifyTimeStamp += MAX_DATA_SIZE;
  }
    performAudit(D_BULKINGEST,user_id,contId,"",cval[5]);

    return(MDAS_SUCCESS);
}


int 
register_incontainer_dataset (int catType, 
			      char *dataName, 
			      char *collectionName, 
			      char *userName, 
			      char *domainName, 
			      char *containerName, 
			      char *dataTypeName, 
			      srb_long_t dataSize,
			      srb_long_t offset)
{

  int  i, obj_action_id;
  int contId;
  char contName[MAX_TOKEN];
  char contColl[MAX_TOKEN];
  int user_id, rsrc_id, new_data_id;

   strcpy(reg_registrar_name,userName);
   strcpy(reg_obj_name, dataName);
   strcpy(reg_obj_code,"MDAS_CD_DATA");
   strcpy(reg_action_name,"create dataset");
    failure = 0;


   user_id = get_user_id_in_domain(userName, domainName);
     if (user_id < 0) { return (USER_NOT_IN_DOMN);}
  tabledIsOkay = 1;
  rsrc_id = get_itype_value("MDAS_CD_RSRC.rsrc_name", "null");
  tabledIsOkay = 0;
  if (failure != 0) { return (RESOURCE_NOT_IN_CAT);}
  tabledIsOkay = 1;
  get_ctype_value(cval[2], "MDAS_TD_DATA_TYP.data_typ_name", dataTypeName);
  tabledIsOkay = 0;
  if (failure != 0) 
    { 
      failure = 0; 
      tabledIsOkay = 1;
      i = get_ctype_value(cval[2], "MDAS_TD_DATA_TYP.data_typ_name", 
			  "generic");
      tabledIsOkay = 0;
      if (failure != 0) return(i);
    }
   
   splitbychar(containerName, contColl,contName,'/');

   contId = getDataId(contColl,contName);
   if (contId < 0 ) return (contId);

   i = check_permit_in_container("write", contId,user_id); /*min write permit*/
   if (i != 0 ) return (i);

   i = check_type_compatibility_in_container(contId,cval[2]);
   if (i != 0 ) return (i);
  

  get_time(cval[5]);
  if (failure != 0) { return (GET_TIME_ERROR);}
  tabledIsOkay = 1;
  get_ctype_value(cval[13], "MDAS_TD_DATA_GRP.data_grp_name",collectionName);
  tabledIsOkay = 0;
  if (failure != 0) { return (COLLECTION_NOT_IN_CAT);} 


  reg_registrar_id = user_id;
  reg_obj_repl_enum = 0;
  strcpy(reg_timeval,cval[5]);
   
  i = check_unique_dname_in_group(dataName,  cval[13]);
  if (i != 0)   { return (DATA_NOT_UNIQUE_IN_COLLECTION);  }

  
  i = check_permit_in_group("write",  cval[13], user_id);
  if (i !=0)  { return (i);}


  new_data_id = 0;
  reg_obj_id =  new_data_id;

#if defined(ORAMCAT) || defined(PSQMCAT)
  sprintf(query_string, 
        "insert into %s%s (data_id,repl_enum,data_name,data_typ_id,path_name,rsrc_id,repl_timestamp,data_grp_id,data_size,is_deleted,data_owner,is_dirty,offset,container_id,modify_timestamp,last_accs_time,datalock,lockowner,version_num,container_td_id) values (%s,-1,'%s','%s','%s',%i,'%s','%s',%lld,0,%i,%i,%lld,%i,'%s','%s',0,0,'0',%i)",
	  MDASSCHEMENAME, 
	  "MDAS_AD_REPL", nextDataId, dataName,
	  cval[2],
	  " ", rsrc_id,
	  cval[5], cval[13],
	  dataSize, user_id,0,offset,contId,cval[5],cval[5],contId);
  ival[0] = DATAID_CURRVAL;
#else
  ival[0] = get_next_counter_value("DATA_ID");
  if (failure != 0) {
    return (DATA_ID_COUNTER_ERROR);
  }
  sprintf(query_string, 
        "insert into %s%s (data_id,repl_enum,data_name,data_typ_id,path_name,rsrc_id,repl_timestamp,data_grp_id,data_size,is_deleted,data_owner,is_dirty,offset,container_id,modify_timestamp,last_accs_time,datalock,lockowner,version_num,container_td_id) values (%i,-1,'%s','%s','%s',%i,'%s','%s',%lld,0,%i,%i,%lld,%i,'%s','%s',0,0,'0',%i)",
	  MDASSCHEMENAME, 
	  "MDAS_AD_REPL", ival[0], dataName,
	  cval[2],
	  " ", rsrc_id,
	  cval[5], cval[13],
	  dataSize, user_id,0,offset,contId,cval[5],cval[5],contId);
#endif /* ORAMCAT */

  i = ExecSqlDb2(henv, hdbc, query_string);
  if (i != 0) 
    {
      strcpy(GENERATEDSQL,query_string);
      error_exit("data registry API: REPL data insertion error"); 
      return(MDAS_AD_REPL_INSERTION_ERROR);
    }
  res = insertGuidBySRB(ival[0]);
  if (res != 0) 
    return(res);
#if defined(ORAMCAT) || defined(PSQMCAT)
    sprintf(query_string,
            "insert into %s%s (data_id,metadatanum) values (%s,-1)",MDASSCHEMENAME,"MDAS_AD_MDATA", currDataId);
#else
    sprintf(query_string,
            "insert into %s%s (data_id,metadatanum) values (%i,-1)",MDASSCHEMENAME,"MDAS_AD_MDATA",ival[0]);
#endif /* ORAMCAT */
    res = ExecSqlDb2(henv, hdbc, query_string);
    if (res != 0)
      return(MDAS_AD_MDATA_INSERTION_ERROR);
  sprintf(query_string," in Conatiner:%s",containerName);
  performAudit(D_CREATE,user_id,ival[0],"",query_string);
  
  return(MDAS_SUCCESS);
}

int
get_container_info (int catType, 
		    char *containerName, 
		    char *userName,
		    char *domainName, 
		    mdasC_sql_result_struct **myresult, 
		    int rowsWanted)
{
  /* Returned value -
     container_max_size, container_size IS_DIRTY 
        (CONTAINER_MAX_SIZE, CONTAIN_SIZE,IS_DIRTY)
     For each physical resource:
     PHY_RSRC_TYP_NAME, RSRC_ADDR_NETPREFIX, PHY_RSRC_NAME,
     PHY_RSRC_DEFAULT_PATH,  RSRC_DEFAULT_PATH, RSRC_CLASS 
  */

  char qval[MAX_DCS_NUM][MAX_TOKEN];
  int i , selval[MAX_DCS_NUM];
  char contName[MAX_TOKEN];
  char contColl[MAX_TOKEN];


  for (i = 0; i < cur_pds_num; i++) {
    sprintf(qval[i],"");
    selval[i] = 0;
  }   

  splitbychar(containerName, contColl,contName,'/');
  
  /*
  sprintf(qval[DATA_NAME]," = '%s'",contName);
  sprintf(qval[DATA_GRP_NAME]," = '%s'",contColl);
  sprintf(qval[CONTAINER_REPL_ENUM]," = %s%s",
	    MDASSCHEMENAME,"MDAS_ADC_REPL.repl_enum");  */

  sprintf(qval[CONTAINER_NAME]," = '%s'",containerName);
  selval[CONTAINER_MAX_SIZE] = 1;
  selval[CONTAINER_RSRC_NAME] = 1;
  selval[CONTAINER_LOG_RSRC_NAME] = 1;
  selval[CONTAINER_RSRC_CLASS] = 1;
  /*	Not needed 
  selval[IS_DIRTY] = 1;
  selval[SIZE] = 1;
  selval[PHY_RSRC_TYP_NAME] = 1;
  selval[RSRC_DEFAULT_PATH] = 1;
  selval[PHY_RSRC_DEFAULT_PATH] = 1;
  selval[RSRC_ADDR_NETPREFIX] = 1;
   */
  
  return(get_data_dir_info(catType, qval, selval, myresult, 
			dcs_tname, dcs_aname, rowsWanted));

}


int
isContainerEmpty(int dataId) 
{
  sprintf(sqlq, "select data_id from %sMDAS_AD_REPL where container_id = %i and data_id <> %i",
	  MDASSCHEMENAME, dataId, dataId );

  if(check_exists(sqlq) == 0) 
	  {
	    return (MDAS_ZERO);
	  }
  return(MDAS_ONE);
}
int
removeContainer(int cat_type, char *containerName, char *obj_user_name,
			      char *domain_name)
{
    int    i, obj_data_id, obj_user_id, obj_action_id;
  int    replication_num, copy_repl_num;
  char obj_group_name[MAX_TOKEN];
  int    all_replicas;
  char obj_path_name[MAX_TOKEN];
  char chuser[MAX_TOKEN];
  char chdomain[MAX_TOKEN];
  int    repllist[MAX_TOKEN];
  int colcount,jj;
  int given_repl_enum;
  char *tmpStr;
  int inContainerObject = 0;
  char contName[MAX_TOKEN];
  char contColl[MAX_TOKEN];
  int contId;
    failure = 0;
   all_replicas = 0;
   given_repl_enum = -1;
   if ((tmpStr = strstr(containerName,"&COPY=")) != NULL)  {
     *tmpStr ='\0';
     given_repl_enum= atoi((char *) &tmpStr[6]);
   }
   splitbychar(containerName, contColl,contName,'/');

   obj_data_id = getDataId(contColl,contName);
   if (obj_data_id < 0 ) return (obj_data_id);

  if (isContainer(obj_data_id) == MDAS_ZERO) 
    return(CONTAINER_NOT_IN_CAT);

  if (given_repl_enum >= 0) {
    sprintf(sqlq, "select repl_enum from %sMDAS_AD_REPL where container_id = %i and data_id =  %i and repl_enum <> %i",
	    MDASSCHEMENAME, obj_data_id, obj_data_id,given_repl_enum );
    if(check_exists(sqlq) != 0) 
      return(CONTAINER_NOT_EMPTY);
  }
  else {
    if (isContainerEmpty(obj_data_id) == MDAS_ZERO) 
      return(CONTAINER_NOT_EMPTY);
  }

   strcpy(reg_registrar_name,obj_user_name);
   strcpy(reg_obj_name, contName);
   strcpy(reg_obj_code,"MDAS_CD_DATA");
   strcpy(reg_action_name,"delete dataset");
   i =get_data_id_with_collection(contName, "","",
		   obj_user_name, contColl,  domain_name,
		   &obj_data_id, &replication_num, &obj_user_id);

 if (i < 0 )
   {  
     /*  RAJA removed April 6, 2004 to allow superuser to remove it 
     return (i);
     RAJA removed April 6, 2004 to allow superuser to remove it */
     i =  checkTrashSuperUserAccess(obj_user_name,domain_name,
				     contColl );
      if (i != 0)
	return (ACTION_NOT_ALLOWED);
      obj_user_id = get_user_id_in_domain(obj_user_name, domain_name);
  }  
 get_time(timeval);


   if (given_repl_enum >= 0) {
     replication_num =given_repl_enum;
     all_replicas =  0;
   }
   else {
     replication_num = 0;
     all_replicas =  1;
   }

#ifdef COMPOUNDNOTDONE

   if (isCompoundObjectById(obj_data_id) == MDAS_ONE) {
      i =removeCompoundObject(NULL, contName,contColl,
				   given_repl_enum, -1, 
				   obj_user_name, domain_name);
      if (i < 0)
	return(i);
   }
#endif /* COMPOUNDNOTDONE */

   reg_registrar_id = obj_user_id;
   reg_obj_id =  obj_data_id;
   reg_obj_repl_enum = replication_num;
   strcpy(reg_timeval,timeval);

   sprintf(atname[0],"data_id");
   sprintf(atname[1],"repl_enum");
   sprintf(atval[0],"%i",obj_data_id);
   sprintf(atval[1],"%i",replication_num);
   if (all_replicas) j = 1; else j = 2;
   i = deleteFromTable(atname, atval,"MDAS_AD_ACCS",j);
   i =deleteFromTable(atname,atval,"MDAS_AD_AUDIT",j);
   if (all_replicas){
     i=deleteFromTable(atname,atval,"MDAS_AU_TCKT_DATA",1);
   }
   i = deleteFromTable(atname, atval,"MDAS_AD_REPL",j);
   if (i != 0) 
     return (MDAS_AD_REPL_DELETION_ERROR);
   if (all_replicas){
     sprintf(atname[0],"container_id");
     sprintf(atval[0],"%i",obj_data_id);
     i=deleteFromTable(atname,atval,"MDAS_AD_COLLCONT",1);
     i=deleteFromTable(atname,atval,"MDAS_TD_CONTAINER",1);
   }
   if (i != 0) 
     return (MDAS_AD_REPL_DELETION_ERROR);

   sprintf(chdomain,"%i",obj_data_id);
   removeTabledQueries(chdomain);
   removeTabledQueries(contName);
   performAudit(D_DELETECONT,obj_user_id,obj_data_id,"","");
   return(MDAS_SUCCESS);
}

int
modify_for_bulk_move(int catType, int operType,
		 char *userName,  char *domainName,
		 char *operString,
		 mdasC_sql_result_struct *inDataInfo)
{
  int  i;

  switch(operType) {
  case BULK_PHY_MOVE:
    i = movePhysicalInBulk(catType, userName, domainName,
                           operString, inDataInfo);
    break;
  case BULK_PHY_MOVE_INTO_CONTAINER:
    i = movePhysicalIntoContainerInBulk(catType, userName, domainName,
					operString, inDataInfo);
    break;
  case BULK_REGISTER_REPLICATE:
    i = registerReplicateInBulk(catType, userName, domainName,
				operString, inDataInfo);
    break;
  default:
    i = FUNCTION_NOT_SUPPORTED;
    break;
  }
  return(i);
}

int
movePhysicalInBulk(int catType,
                 char *userName,
                 char *domainName,
                 char *rsrcName,
                 mdasC_sql_result_struct *inDataInfo)
{
  int  i,ii;
  int sysAdminFlag = 0;
  int user_id, rsrc_id, data_id, rnum;
  char *dataName, *collName,  *pathName,  *replNum, *verNum;
  char newPathName[MAX_TOKEN + 20];
  char newCollName[MAX_TOKEN + 20];
  char newDataName[MAX_TOKEN + 20];
  int accsId;

  failure = 0;
  tabledIsOkay = 1;
  user_id = get_user_id_in_domain(userName, domainName);
  if (user_id < 0) { return (USER_NOT_IN_DOMN);}
  rsrc_id = getResourceIdWithAccessToUserId("write", rsrcName, user_id);
  if (rsrc_id < 0)  
    return (rsrc_id);

  if (is_system_userid(user_id) == 0)
    sysAdminFlag = 1;
  failure = 0;
  if (sysAdminFlag != 1) {
    accsId = getAccessId("write");
    if (accsId < 0)
      return(accsId );
  }

  tabledIsOkay = 0;
  get_time(cval[5]);
  if (failure != 0) { return (GET_TIME_ERROR);}


  dataName = (char *) get_from_result_struct(
	     (mdasC_sql_result_struct *) inDataInfo,
	     dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
  collName = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) inDataInfo,
            dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
  pathName = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) inDataInfo,
            dcs_tname[PATH_NAME], dcs_aname[PATH_NAME]);
  replNum = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) inDataInfo,
            dcs_tname[DATA_REPL_ENUM], dcs_aname[DATA_REPL_ENUM]);
  verNum = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) inDataInfo,
            dcs_tname[DATA_VER_NUM], dcs_aname[DATA_VER_NUM]);

  for (ii = 0; ii < inDataInfo->row_count ; ii++) { /*** begin the loop ***/
    rnum = atoi(replNum);
    make_quote_to_two_quote(pathName,newPathName);
    make_quote_to_two_quote(dataName,newDataName);
    make_quote_to_two_quote(collName,newCollName);
    data_id = getDataId(newCollName,newDataName);
    if (data_id < 0)  
      return (DATA_NAME_NOT_FOUND);
    if (sysAdminFlag != 1) {
      i = check_permit_in_dataid_by_access_id(data_id,user_id,accsId);
      if (i != MDAS_SUCCESS)
	return(i);
      if (inContainerObjectById(data_id) == MDAS_SUCCESS)
	return(ACTN_NOT_ALLOWED_ON_IN_CONTAINER_OR_LINKED_OBJECT);
      i = check_unique_dname_in_respath(newDataName,newPathName,rsrc_id);
      if (i != 0)
	return (DATA_NOT_UNIQUE_IN_PATH);
    }
    sprintf(atname[0],"path_name");
    sprintf(atname[1],"rsrc_id");
    sprintf(atname[2],"data_id");
    sprintf(atname[3],"repl_enum");
    sprintf(atname[4],"version_num");
    sprintf(atval[0],"'%s'",newPathName);
    sprintf(atval[1],"%i",rsrc_id);
    sprintf(atval[2],"%i",data_id);
    sprintf(atval[3],"%i",rnum);
    if (strlen(verNum) > 0) {
      sprintf(atval[4],"'%s'",verNum);
      i = changeAttrValueSet(atname, atval,"MDAS_AD_REPL",5,2);
    }
    else
      i = changeAttrValueSet(atname, atval,"MDAS_AD_REPL",4,2);
    if (i !=0) return (MDAS_AD_REPL_UPDATE_ERROR);

    performAudit(D_MOVE,user_id,data_id,"","");
    dataName += MAX_DATA_SIZE;
    collName += MAX_DATA_SIZE;
    pathName += MAX_DATA_SIZE;
    replNum += MAX_DATA_SIZE;
    verNum += MAX_DATA_SIZE;
  }
  return(MDAS_SUCCESS);
}

int
movePhysicalIntoContainerInBulk(int catType,
                 char *userName,
                 char *domainName,
                 char *containerName,
                 mdasC_sql_result_struct *inDataInfo)
{
  int  i,ii;
  int sysAdminFlag = 0;
  int user_id, contId, data_id, rnum;
  char *dataName, *collName,  *dataSize, *offSet, *replNum, *verNum;
  char newCollName[MAX_TOKEN + 20];
  char newDataName[MAX_TOKEN + 20];
  int accsId, rsrc_id;
  char contName[MAX_TOKEN];
  char contColl[MAX_TOKEN];
  srb_long_t idataSize;
  srb_long_t ioffSet;

  failure = 0;
  tabledIsOkay = 1;

  splitbychar(containerName, contColl,contName,'/');
  contId = getDataId(contColl,contName);
  if (contId < 0 ) 
    return(contId);
  user_id = get_user_id_in_domain(userName, domainName);
  if (user_id < 0) { return (USER_NOT_IN_DOMN);}
  rsrc_id = get_itype_value("MDAS_CD_RSRC.rsrc_name", "null");
  if (failure != 0) { return (RESOURCE_NOT_IN_CAT);}
  if (is_system_userid(user_id) == 0)
    sysAdminFlag = 1;
  failure = 0;
  if (sysAdminFlag != 1) {
    accsId = getAccessId("write");
    if (accsId < 0)
      return(accsId );
    i = check_permit_in_container("write", contId,user_id); /*min write permit*/
    if (i < 0)
      return (i);
  }
  tabledIsOkay = 0;
  get_time(cval[5]);
  if (failure != 0) { return (GET_TIME_ERROR);}


  dataName = (char *) get_from_result_struct(
	     (mdasC_sql_result_struct *) inDataInfo,
	     dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
  collName = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) inDataInfo,
            dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
  dataSize = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) inDataInfo,
            dcs_tname[SIZE], dcs_aname[SIZE]);
  offSet = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) inDataInfo,
            dcs_tname[OFFSET], dcs_aname[OFFSET]);
  replNum = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) inDataInfo,
            dcs_tname[DATA_REPL_ENUM], dcs_aname[DATA_REPL_ENUM]);
  verNum = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) inDataInfo,
            dcs_tname[DATA_VER_NUM], dcs_aname[DATA_VER_NUM]);

  for (ii = 0; ii < inDataInfo->row_count ; ii++) { /*** begin the loop ***/
    rnum = atoi(replNum);
    idataSize = strtoll (dataSize, 0, 0);
    ioffSet = atol(offSet);
    make_quote_to_two_quote(dataName,newDataName);
    make_quote_to_two_quote(collName,newCollName);
    data_id = getDataId(newCollName,newDataName);
    if (data_id < 0)  
      return (DATA_NAME_NOT_FOUND);
    if (sysAdminFlag != 1) {
      i = check_permit_in_dataid_by_access_id(data_id,user_id,accsId);
      if (i != MDAS_SUCCESS)
	return(i);
      if (inContainerObjectById(data_id) == MDAS_SUCCESS)
	return(ACTN_NOT_ALLOWED_ON_IN_CONTAINER_OR_LINKED_OBJECT);
    }
    sprintf(atname[0],"path_name");
    sprintf(atname[1],"rsrc_id");
    sprintf(atname[2],"data_size");
    sprintf(atname[3],"offset");
    sprintf(atname[4],"container_id");
    sprintf(atname[5],"container_td_id");
    sprintf(atname[6],"repl_enum");
    sprintf(atname[7],"data_id");
    sprintf(atname[8],"repl_enum");
    sprintf(atname[9],"version_num");
    sprintf(atval[0],"' '");
    sprintf(atval[1],"%i",rsrc_id);
    sprintf(atval[2],"%lld",idataSize);
    sprintf(atval[3],"%lld",ioffSet);
    sprintf(atval[4],"%i",contId);
    sprintf(atval[5],"%i",contId);
    sprintf(atval[6],"-1");
    sprintf(atval[7],"%i",data_id);
    sprintf(atval[8],"%i",rnum);
    if (strlen(verNum) > 0) {
      sprintf(atval[9],"'%s'",verNum);
      i = changeAttrValueSet(atname, atval,"MDAS_AD_REPL",10,7);
    }
    else
      i = changeAttrValueSet(atname, atval,"MDAS_AD_REPL",9,7);
    if (i !=0) return (MDAS_AD_REPL_UPDATE_ERROR);

    performAudit(D_MOVE,user_id,data_id,"","");
    dataName += MAX_DATA_SIZE;
    collName += MAX_DATA_SIZE;
    dataSize += MAX_DATA_SIZE;
    offSet += MAX_DATA_SIZE;
    replNum += MAX_DATA_SIZE;
    verNum += MAX_DATA_SIZE;
  }
  return(MDAS_SUCCESS);

}


int
registerReplicateInBulk(int catType,
                 char *userName,
                 char *domainName,
                 char *rsrcName,
                 mdasC_sql_result_struct *inDataInfo)
{
  int  i,ii;
  int sysAdminFlag = 0;
  int user_id, rsrc_id, data_id, rnum;
  char *dataName, *collName,  *pathName,  *replNum, *verNum;
  char newPathName[MAX_TOKEN + 20];
  char newCollName[MAX_TOKEN + 20];
  char newDataName[MAX_TOKEN + 20];
  int accsId;
  int replication_num;
  failure = 0;
  tabledIsOkay = 1;
  user_id = get_user_id_in_domain(userName, domainName);
  if (user_id < 0) { return (USER_NOT_IN_DOMN);}
  rsrc_id = getResourceIdWithAccessToUserId("write", rsrcName, user_id);
  if (rsrc_id < 0)  
    return (rsrc_id);

  if (is_system_userid(user_id) == 0)
    sysAdminFlag = 1;
  failure = 0;
  if (sysAdminFlag != 1) {
    accsId = getAccessId("write");
    if (accsId < 0)
      return(accsId );
  }

  tabledIsOkay = 0;
  get_time(cval[5]);
  if (failure != 0) { return (GET_TIME_ERROR);}


  dataName = (char *) get_from_result_struct(
	     (mdasC_sql_result_struct *) inDataInfo,
	     dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
  collName = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) inDataInfo,
            dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
  pathName = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) inDataInfo,
            dcs_tname[PATH_NAME], dcs_aname[PATH_NAME]);
  replNum = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) inDataInfo,
            dcs_tname[DATA_REPL_ENUM], dcs_aname[DATA_REPL_ENUM]);
  verNum = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) inDataInfo,
            dcs_tname[DATA_VER_NUM], dcs_aname[DATA_VER_NUM]);

  for (ii = 0; ii < inDataInfo->row_count ; ii++) { /*** begin the loop ***/
    rnum = atoi(replNum);
    make_quote_to_two_quote(pathName,newPathName);
    make_quote_to_two_quote(dataName,newDataName);
    make_quote_to_two_quote(collName,newCollName);
    data_id = getDataId(newCollName,newDataName);
    if (data_id < 0)  
      return (DATA_NAME_NOT_FOUND);
    if (sysAdminFlag != 1) {
      i = check_permit_in_dataid_by_access_id(data_id,user_id,accsId);
      if (i != MDAS_SUCCESS)
	return(i);
      if (inContainerObjectById(data_id) == MDAS_SUCCESS)
	return(ACTN_NOT_ALLOWED_ON_IN_CONTAINER_OR_LINKED_OBJECT);
      i = check_unique_dname_in_respath(newDataName,newPathName,rsrc_id);
      if (i != 0)
	return (DATA_NOT_UNIQUE_IN_PATH);
    }
    replication_num = get_max_repl_num(data_id);
    if ( replication_num < 0)
      return (NO_DATA_REPLICA_IN_CAT);
    i = get_data_repl_info_with_data_id_repl_num(data_id,rnum, verNum, atval);
    if (i < 0)
      return(i);


    sprintf(atname[0],"data_id");
    sprintf(atname[1],"repl_enum");
    sprintf(atname[2],"data_name");
    sprintf(atname[3],"data_typ_id");
    sprintf(atname[4],"path_name");
    sprintf(atname[5],"rsrc_id");
    sprintf(atname[6],"repl_timestamp");
    sprintf(atname[7],"data_grp_id");
    sprintf(atname[8],"data_size");
    sprintf(atname[9],"is_deleted");
    sprintf(atname[10],"data_owner");
    sprintf(atname[11],"is_dirty");
    sprintf(atname[12],"offset");
    sprintf(atname[13],"container_id");
    sprintf(atname[14],"datalock");
    sprintf(atname[15],"lockowner");
    sprintf(atname[16],"version_num");
    sprintf(atname[17],"container_td_id");
    sprintf(atname[18],"d_comment");
    sprintf(atname[19],"modify_timestamp");
    sprintf(atname[20],"last_accs_time");
    sprintf(atname[21],"auditflag");
    sprintf(atname[22],"seg_num");
    sprintf(atname[23],"dlockexpiry");
    sprintf(atname[24],"dchecksum");
    sprintf(atname[25],"dhide");
    
    sprintf(atval[1],"%i",replication_num);
    sprintf(atval[4],"'%s'",newPathName);
    sprintf(atval[5],"%i",rsrc_id);
    sprintf(atval[6],"'%s'",timeval);
    sprintf(atval[19],"'%s'",timeval);
    sprintf(atval[20],"'%s'",timeval);
    sprintf(atval[39],"'%s'",atval[2]);
    strcpy(atval[2],atval[39]);
    sprintf(atval[39],"'%s'",atval[3]);
    strcpy(atval[3],atval[39]);
    sprintf(atval[39],"'%s'",atval[7]);
    strcpy(atval[7],atval[39]);
    sprintf(atval[39],"'%s'",atval[16]);
    strcpy(atval[16],atval[39]);
    sprintf(atval[39],"'%s'",atval[18]);
    strcpy(atval[18],atval[39]);
    sprintf(atval[39],"'%s'",atval[23]);
    strcpy(atval[23],atval[39]);
    sprintf(atval[39],"'%s'",atval[24]);
    strcpy(atval[24],atval[39]);
    
    
    i = insertIntoTable(atname,atval,"MDAS_AD_REPL",26);
    if (i !=0) return (MDAS_AD_REPL_INSERTION_ERROR);

    sprintf(sqlq, "insert into %sMDAS_AD_ACCS (data_id, repl_enum, user_id,access_id) select %s, %i,  user_id, access_id from %sMDAS_AD_ACCS where data_id = %s and repl_enum = %i ",
          MDASSCHEMENAME,atval[0],replication_num,  MDASSCHEMENAME,
          atval[0],rnum);
    i = ExecSqlDb2(henv, hdbc, sqlq);
    if (i != 0)
      return(MDAS_AD_ACCS_INSERTION_ERROR);


    performAudit(D_REPLICATE, user_id,data_id,"","");
    dataName += MAX_DATA_SIZE;
    collName += MAX_DATA_SIZE;
    pathName += MAX_DATA_SIZE;
    replNum += MAX_DATA_SIZE;
    verNum += MAX_DATA_SIZE;
  }
  return(MDAS_SUCCESS);
  
}


#endif   /*for SRB_MDAS*/

