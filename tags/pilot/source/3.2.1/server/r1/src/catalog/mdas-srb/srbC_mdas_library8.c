






































#ifdef SRB_MDAS
#include "svrConn.h"
#include "mdasC_db2_externs.h"
#include "srbC_mdas_externs.h"
#include "mdasGlobalsExtern.h"
#include "elogExtern.h"
#include "srbType.h"
#include "mdasPrototypes.h"


int
getRandomString(char *rString, int length)
{
  int i,k;
  for (i =0 ;  i < (length - 1); i++) {
      k =  random() % 62;
      if ( k < 10)
	  rString[i] = '0' + k;
      else if ( k < 36 )
	  rString[i] = 'a' + k-10;
      else if ( k < 62 )
	  rString[i] = 'A' + k-36;
    }
  rString[length] = '\0';
}

int
registerCompoundResourceInfo(char *compoundResourceName,
		   char *physicalResourceName,
		   char *registrarName,char *registrarDomain,
		   char *registrarPassword)
{
  int i;
  int cmpnd_rsrc_id, phy_rsrc_id;
  
   strcpy(reg_registrar_name,registrarName);
   strcpy(reg_obj_name, compoundResourceName);
   strcpy(reg_obj_code,"MDAS_CD_RSRC");
   strcpy(reg_action_name,"create compound resource");
   reg_obj_repl_enum = 0;
   failure = 0;
/**** removed RAJA  Feb 18,2003 
  i = web_mdas_sys_authorization(registrarName, registrarPassword,
                                  registrarDomain);
   if ( i != 0) {
     sprintf(query_string,"resource registry SYSAPI:Failed Password:%s,%s\n",
	     registrarPassword,registrarName);
     return(SYS_USER_AUTHORIZATION_ERROR);
   }
 ********/
   i = isUserSysAdmin(registrarName,  registrarDomain);
   if ( i != 0)
     return(SYS_USER_AUTHORIZATION_ERROR);
   sprintf(query_string,
	   "select  rsrc_id from %sMDAS_AR_REPL A, %sMDAS_TD_RSRC_TYP  B where A.rsrc_name = '%s' and B.rsrc_typ_name like '%%compound%%' and A.rsrc_typ_id = B.rsrc_typ_id",
	    MDASSCHEMENAME, MDASSCHEMENAME, compoundResourceName);
   cmpnd_rsrc_id = get_itype_value_from_query(query_string) ;
   if (cmpnd_rsrc_id < 0) {
     elog(NOTICE,"Failure to find compund resource: %s\n",query_string);
     failure = COMPOUND_RESOURCE_NOT_IN_CAT;
     return (COMPOUND_RESOURCE_NOT_IN_CAT);
   }
   reg_registrar_id = get_user_id_in_domain(registrarName, 
                                           registrarDomain);

  if (reg_registrar_id < 0) { return (USER_NOT_IN_DOMN);}
  get_time(reg_timeval);
  if (failure != 0) { return (GET_TIME_ERROR);}
 
  phy_rsrc_id = get_itype_value("MDAS_AR_PHYSICAL.phy_rsrc_name", 
				physicalResourceName);
   if (failure != 0) { return (PHYSICAL_RESOURCE_NOT_IN_CAT);}

  sprintf(query_string, 
    "insert into %s%s (cmpnd_rsrc_id,phy_rsrc_id,user_id) values (%i,%i,%i)",
          MDASSCHEMENAME, "MDAS_AR_CMPND_RSRC", 
          cmpnd_rsrc_id, phy_rsrc_id, reg_registrar_id);
  res = ExecSqlDb2(henv, hdbc, query_string);
  if (res != 0) {
    elog (NOTICE, "registry API: Compond resource insertion error:%s", query_string);
    return(MDAS_AR_CMPND_RSRC_INSERTION_ERROR);
  }
  return(MDAS_SUCCESS);
}


int 
isCompoundObjectByName(svrConn  *conn,char *comObjName, 
		       char *comObjCollName)
{
   
  sprintf(sqlq, "select data_id from %sMDAS_AD_REPL A, %sMDAS_TD_DATA_GRP B, %sMDAS_AR_REPL C, %sMDAS_TD_RSRC_TYP  D where A.data_name = '%s' and B.data_grp_name = '%s' and A.data_grp_id = B.data_grp_id and A.rsrc_id =C.rsrc_id and C.rsrc_typ_id = D.rsrc_typ_id and D.rsrc_typ_name like '%%compound%%'",
	  MDASSCHEMENAME,MDASSCHEMENAME,MDASSCHEMENAME,MDASSCHEMENAME,
	  comObjName,comObjCollName);
  tabledIsOkay = 1;
  if(check_exists(sqlq) == 0) 
	  {
	    tabledIsOkay = 0;
	    return (MDAS_ONE);
	  }
  tabledIsOkay = 0;
  return(MDAS_ZERO);
}


int 
registerInternalCompoundObject (
       svrConn  *conn,
       char *objName, 
       char *objCollName, 
       int   objReplNum,
       int   objSegNum,
       char *resourceName,
       char *dataPathName, 
       srb_long_t dataSize,
       srb_long_t offset,
       int   myIntReplNum,
       int   myIntSegNum,
       char *userName, 
       char *domainName)
{
  int i;
  int userId, objId, parRsrcId, childRsrcId, intReplNum, intSegNum;
  char tmpStr2[MAX_TOKEN];
  /* create table MDAS_AD_COMPOUND(
	data_id integer not null,
	repl_enum integer  not null,
	seg_num	integer DEFAULT -1,
	user_id integer,
	int_repl_num integer  not null,
	int_seg_num integer DEFAULT -1,
	cmpd_path_name  varchar(699),
	cmpd_rsrc_id   integer,
	repl_timestamp VARCHAR(32)  not null ,	 
	modify_timestamp varchar(32),
	last_accs_time varchar(32),
	data_size integer,				     
	offset     integer,
	is_dirty   integer
	);
  */
  strcpy(reg_registrar_name,userName);
  strcpy(reg_obj_name, objName);
  strcpy(reg_obj_code,"MDAS_AD_COMPOUND");
  strcpy(reg_action_name,"create dataset");
  failure = 0;

  
  userId = get_user_id_in_domain(userName, domainName);
  if (userId < 0)  
    return (USER_NOT_IN_DOMN);
  i = getDataIdRsrcIdFromObj(objCollName, objName,objReplNum,objSegNum,
			     &objId, &parRsrcId);
  if (i < 0) 
    return (i);
  childRsrcId = getChildCompoundRsrcId(parRsrcId,resourceName);
  if (childRsrcId < 0) 
    return(ERROR_IN_COMPOUND_RESOURCE_NAME);
  
  get_time(cval[5]);
  if (failure != 0) { return (GET_TIME_ERROR);}

  reg_registrar_id = userId;
  reg_obj_repl_enum = objReplNum;
  strcpy(reg_timeval,cval[5]);
  reg_obj_id =  objId;
  make_quote_to_two_quote(dataPathName,dataPathName);

  /*
    intReplNum NEW_INT_REPL_NUM   means generate next replica number
    intSegNum  NO_INT_SEGMENTATION    means non-segmented object
    intSegNum  NEW_INT_SEG_NUM  means generate  the next segment number
  */
  if (myIntReplNum == NEW_INT_REPL_NUM) {
    intReplNum = getMaxCompoundReplNum(objId, objReplNum,objSegNum);
    if (intReplNum < 0) return intReplNum;
  }
  else 
    intReplNum = myIntReplNum;
  if ( intReplNum < 0) return intReplNum;

  if (myIntSegNum == NEW_INT_SEG_NUM) {
    intSegNum = getMaxCompoundSegNum(objId, objReplNum, objSegNum,intReplNum);
    if (intSegNum < 0) intSegNum = 0;
  }
  else
    intSegNum = myIntSegNum;
  if (intSegNum < NO_INT_SEGMENTATION) return intSegNum;

  i = checkIfCompoundObjectExists(objId, objReplNum,objSegNum,intReplNum,intSegNum);
  if (i < 0)
    return (i);
  
  sprintf(query_string, 
	  "insert into %s%s (data_id,repl_enum,seg_num,user_id,int_repl_num,int_seg_num,cmpd_path_name,cmpd_rsrc_id,repl_timestamp,modify_timestamp,last_accs_time,data_size,offset,is_dirty) values (%i,%i,%i,%i,%i,%i,'%s',%i,'%s','%s','%s',%lld,%lld,%i)",
	  MDASSCHEMENAME, 
	  "MDAS_AD_COMPOUND",  
	  objId, objReplNum, objSegNum, userId, intReplNum, intSegNum,
	  dataPathName,childRsrcId,cval[5],cval[5],cval[5],
	  dataSize,offset,0);

  i = ExecSqlDb2(henv, hdbc, query_string);
  if (i != 0) 
    {error_exit("data registry API: REPL data insertion error"); 
    return(MDAS_AD_COMPOUND_INSERTION_ERROR);}
  sprintf(tmpStr2,"%s/%s::%i:%::%i:%i",objCollName,objName,
	  objReplNum, objSegNum, intReplNum, intSegNum);
  performAudit(D_COMPOUND_CREATE,ival[4],ival[0],"",tmpStr2);
  return(intReplNum);
}

     

int
getInternalReplicaInfo(
       svrConn  *conn,
       char *objName, 
       char *objCollName,
       int   objReplNum,
       int   objSegNum,
       mdasC_sql_result_struct **myresult, 
       int rowsWanted)
{

    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int i , selval[MAX_DCS_NUM];

    for (i = 0; i < MAX_DCS_NUM; i++)  {
      sprintf(qval[i],"");
      selval[i] = 0;
    }

    sprintf(qval[DATA_NAME]," = '%s'",objName);
    sprintf(qval[DATA_GRP_NAME]," = '%s'",objCollName);
    sprintf(qval[DATA_REPL_ENUM]," = %i",objReplNum);
    sprintf(qval[DATA_SEG_NUM]," = %i",objSegNum);

    selval[DATA_ID] = 1;
    selval[DATA_REPL_ENUM] = 1;
    selval[DATA_SEG_NUM] = 1;
    selval[INT_REPL_ENUM] = 1;
    selval[INT_SEG_NUM] = 1;
    selval[INT_PATH_NAME] = 1;
    selval[INT_RSRC_NAME] = 1;
    selval[INT_RSRC_ADDR_NETPREFIX] = 1;
    selval[INT_RSRC_TYP_NAME] = 1;
    selval[INT_IS_DIRTY] = 1;
    selval[INT_RSRC_CLASS] = 1;
    selval[INT_OFFSET] = 1;
    selval[INT_SIZE] = 1;

    i = get_data_dir_info(MDAS_CATALOG, qval, selval, myresult,
			  dcs_tname, dcs_aname, rowsWanted);

    return (i);

}

int
getInternalReplicaInfoGivenPathOfOne(
       svrConn  *conn,
       char *pathName,
       mdasC_sql_result_struct **myresult, 
       int rowsWanted)
{

    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int i , selval[MAX_DCS_NUM];

    for (i = 0; i < MAX_DCS_NUM; i++)  {
      sprintf(qval[i],"");
      selval[i] = 0;
    }

    
    sprintf(qval[INT_PATH_NAME_FOR_REPL]," = '%s'",pathName);

    selval[DATA_ID] = 1;
    selval[DATA_REPL_ENUM] = 1;
    selval[DATA_GRP_NAME] = 1;
    selval[PHY_RSRC_NAME] = 1;
    selval[RSRC_ADDR_NETPREFIX] = 1;    
    selval[DATA_SEG_NUM] = 1;
    selval[INT_REPL_ENUM] = 1;
    selval[INT_SEG_NUM] = 1;
    selval[INT_PATH_NAME] = 1;
    selval[INT_RSRC_NAME] = 1;
    selval[INT_RSRC_ADDR_NETPREFIX] = 1;
    selval[INT_RSRC_TYP_NAME] = 1;
    selval[INT_IS_DIRTY] = 1;
    selval[INT_RSRC_CLASS] = 1;
    selval[INT_OFFSET] = 1;
    selval[INT_SIZE] = 1;

    i = get_data_dir_info(MDAS_CATALOG, qval, selval, myresult,
			  dcs_tname, dcs_aname, rowsWanted);

    return (i);

}

int
removeCompoundObject(
       svrConn  *conn,
       char *objName, 
       char *objCollName,
       int   objReplNum,
       int   objSegNum,
       char *objUserName,
       char *domainName)
{
  int    i, objDataId, objUserId;
  char tmpStr[MAX_TOKEN];
  
  /* if repl_enum < 0 and/or seg_num < 0 then all repls and/or
       segments will be removed */

  objDataId = getDataId(objCollName,objName);
  if (objDataId < 0 ) 
    return (objDataId);

  /* first check if there are any internal objects still there */
   sprintf(sqlq,"select data_id from %sMDAS_AD_COMPOUND where data_id = %i ",
	   MDASSCHEMENAME,objDataId);
   if (objReplNum >= 0 )
     sprintf(&sqlq[strlen(sqlq)], " and repl_enum= %i ", objReplNum);
   if (objSegNum >= 0 )
     sprintf(&sqlq[strlen(sqlq)], " and seg_num = %i ",objSegNum); 
   if(check_exists(sqlq) == 0) 
      return(COMPOUND_OBJECT_NOT_EMPTY);

   objUserId = get_user_id_in_domain(objUserName,domainName);
   if (objUserId < 0)
     return(USER_NOT_IN_DOMN);
   i = deleteDataInfoFromMCAT(objName,objUserId,objDataId,
			      objReplNum,objSegNum);
   if (i < 0)   
     return(i);


  sprintf(tmpStr," objReplNum=%i; objSegNum=%i;",
	  objReplNum,objSegNum);
   performAudit(D_DELETE_COMPOUND, objUserId, objDataId,"","");

  return(MDAS_SUCCESS);
}

int
removeInternalCompoundObject(
       svrConn  *conn,
       char *objName, 
       char *objCollName,
       int   objReplNum,
       int   objSegNum,
       int   intReplNum,       
       int   intSegNum,       
       char *objUserName,
       char *domainName)
{
  int j;
  int accsId, objDataId, objUserId; 
  char tmpStr[MAX_TOKEN];
  /* check for permission */

  accsId = getAccessId("all audit");
  objDataId = getDataId(objCollName,objName);
  if (objDataId < 0 ) 
    return (objDataId);
  objUserId = get_user_id_in_domain(objUserName,domainName);
  if (objUserId < 0)
    return(USER_NOT_IN_DOMN);
  
  /* check for privilege */
  sprintf(sqlq, "select data_id from %sMDAS_AD_ACCS  where data_id = %i and repl_enum =  %i and user_id = %i and access_id >= %i", 
	  MDASSCHEMENAME, objDataId, objReplNum, objUserId, accsId);
  if (check_exists(sqlq) != MDAS_SUCCESS)
    return (USER_HAS_INSUFFICIENT_PRIVILEGE);

  sprintf(atname[0],"data_id");
  sprintf(atname[1],"repl_enum");
  sprintf(atname[2],"seg_num");
  sprintf(atname[3],"int_repl_num");
  sprintf(atname[4],"int_seg_num");
  sprintf(atval[0],"%i",objDataId);
  sprintf(atval[1],"%i",objReplNum);
  sprintf(atval[2],"%i",objSegNum);
  sprintf(atval[3],"%i",intReplNum);
  sprintf(atval[4],"%i",intSegNum);
  if (intReplNum >= 0) {
    if (intSegNum >= 0) j = 5;
    else j = 4;
  }
  else {
    if (intSegNum < 0) j = 3;
    else {
      j = 4;
      sprintf(atname[3],"int_seg_num");
      sprintf(atval[3],"%i",intSegNum);
    }
  }
  i = deleteFromTable(atname, atval,"MDAS_AD_COMPOUND",j);
  if (i != 0) 
    return (MDAS_AD_COMPOUND_DELETION_ERROR);
  sprintf(tmpStr," objReplNum=%i; objSegNum=%i; intReplNum=%i; intSegNum=%i;",
	  objReplNum,objSegNum,intReplNum,intSegNum);
  performAudit(D_DELETE_SUBCOMPOUND, objUserId, objDataId,"",tmpStr);
  sprintf(tmpStr,"%i",objDataId);
  removeTabledQueries(objName);
  removeTabledQueries(tmpStr);

  return(MDAS_SUCCESS);  
   
}

int modifyInternalCompoundObject(
       svrConn  *conn,
       char *objName, 
       char *objCollName,
       int   objReplNum,
       int   objSegNum,
       int   intReplNum,       
       int   intSegNum,       
       char *userName,
       char *domainName,
       char *data_value_1, 
       char *data_value_2,
       char *data_value_3,
       char *data_value_4,
       int retraction_type)
{
  /* no checks are made for seeing whether the modifications can be done by
     the user !!!!! */
  int i,j;
  int objDataId, objUserId; 
  char tmpStr[MAX_TOKEN];
  /* check for permission */

  objDataId = getDataId(objCollName,objName);
  if (objDataId < 0 ) 
    return (objDataId);
  objUserId = get_user_id_in_domain(userName,domainName);
  if (objUserId < 0)
    return(USER_NOT_IN_DOMN);
  get_time(timeval);

  if (intSegNum < 0) {
    if (intReplNum < 0)
      j = 4;
    else
      j = 5;
  }
  else {
    if (intReplNum < 0) {
      sprintf(atname[4],"int_seg_num");
      sprintf(atval[4],"%i",intSegNum);
      j = 5;
    }
    else
      j = 6;
  }
  
  if (retraction_type == D_COMPOUND_MODIFY_SIZE) {
    sprintf(atname[0],"data_size");
    sprintf(atname[1],"modify_timestamp");
    sprintf(atname[2],"last_accs_time");
    sprintf(atname[3],"data_id");
    sprintf(atname[4],"repl_enum");
    sprintf(atname[5],"seg_num");
    sprintf(atname[6],"int_repl_num");
    sprintf(atname[7],"int_seg_num");
    sprintf(atval[0],"%s",data_value_1);
    sprintf(atval[1],"'%s'",timeval);
    sprintf(atval[2],"'%s'",timeval);
    sprintf(atval[3],"%i",objDataId);
    sprintf(atval[4],"%i",objReplNum);
    sprintf(atval[5],"%i",objSegNum);
    sprintf(atval[6],"%i",intReplNum);
    sprintf(atval[7],"%i",intSegNum);
    j = j + 2;
    i = changeAttrValueSet(atname, atval,"MDAS_ad_compound", j, 3);
    if (i != 0) return (MDAS_AD_COMPOUND_UPDATE_ERROR);

  }
  else if (retraction_type == D_COMPOUND_MODIFY_SIZE_AND_DIRTY) {
    sprintf(atname[0],"data_size");
    sprintf(atname[1],"modify_timestamp");
    sprintf(atname[2],"last_accs_time");
    sprintf(atname[3],"is_dirty");
    sprintf(atname[4],"data_id");
    sprintf(atname[5],"repl_enum");
    sprintf(atname[6],"seg_num");
    sprintf(atname[7],"int_repl_num");
    sprintf(atname[8],"int_seg_num");
    sprintf(atval[0],"%s",data_value_1);
    sprintf(atval[1],"'%s'",timeval);
    sprintf(atval[2],"'%s'",timeval);
    sprintf(atval[3],"%s",data_value_2);
    sprintf(atval[4],"%i",objDataId);
    sprintf(atval[5],"%i",objReplNum);
    sprintf(atval[6],"%i",objSegNum);
    sprintf(atval[7],"%i",intReplNum);
    sprintf(atval[8],"%i",intSegNum);
    j = j + 3;
    i = changeAttrValueSet(atname, atval,"MDAS_ad_compound", j, 4);
    if (i != 0) return (MDAS_AD_COMPOUND_UPDATE_ERROR);

  }
  else {
    sprintf(atname[1],"data_id");
    sprintf(atname[2],"repl_enum");
    sprintf(atname[3],"seg_num");
    sprintf(atname[4],"int_repl_num");
    sprintf(atname[5],"int_seg_num");
    sprintf(atval[1],"%i",objDataId);
    sprintf(atval[2],"%i",objReplNum);
    sprintf(atval[3],"%i",objSegNum);
    sprintf(atval[4],"%i",intReplNum);
    sprintf(atval[5],"%i",intSegNum);
    
    switch (retraction_type)
      {
      case D_COMPOUND_MODIFY_CMPD_PATH_NAME:
	sprintf(atname[0],"cmpd_path_name");
	sprintf(atval[0],"'%s'",data_value_1);
	break;
      case D_COMPOUND_MODIFY_MODIFY_TIMESTAMP:
	sprintf(atname[0],"modify_timestamp");
	sprintf(atval[0],"'%s'",data_value_1);
	break;
      case D_COMPOUND_MODIFY_LAST_ACCS_TIME:
	sprintf(atname[0],"last_accs_time");
	sprintf(atval[0],"'%s'",data_value_1);
	break;
      case D_COMPOUND_MODIFY_OFFSET:
	sprintf(atname[0],"offset");
	sprintf(atval[0],"%s",data_value_1);
	break;
      case D_COMPOUND_MODIFY_IS_DIRTY:
	sprintf(atname[0],"is_dirty");
	sprintf(atval[0],"%s",data_value_1);
	break;
      case D_COMPOUND_SET_MODIFY_TIMESTAMP:
	sprintf(atname[0],"modify_timestamp");
	sprintf(atval[0],"'%s'",timeval);
	break;
      case D_COMPOUND_SET_LAST_ACCS_TIME:
	sprintf(atname[0],"last_accs_time");
	sprintf(atval[0],"'%s'",timeval);
	break;
      default:
	return (MDAS_CATALOG_MODIFY_TYPE_ERROR);
	break;
      }
    i = changeAttrValue(atname, atval,"MDAS_AD_COMPOUND",j);
    if (i != 0) return (MDAS_AD_COMPOUND_UPDATE_ERROR);
  }
  sprintf(tmpStr," objReplNum=%i; objSegNum=%i; intReplNum=%i; intSegNum=%i;",
	  objReplNum,objSegNum,intReplNum,intSegNum);
  performAudit(retraction_type,objUserId,objDataId,"",tmpStr);
  removeTabledQueries(objName);
  sprintf(tmpStr,"%i",objDataId);
  removeTabledQueries(tmpStr);
  return(MDAS_SUCCESS);
}


int
registerDataSegmentInfo(       svrConn  *conn,
       char *objName, 
       char *objCollName,
       int   objSegNum,
       char *resourceName,
       char *dataPathName, 
       srb_long_t dataSize,
       srb_long_t offset,
       char *dataTypeName,
       char *userName, 
       char *domainName)
{
}

int
getMaxCompoundReplNum(int objId, int objReplNum, int objSegNum)
{
  int r_num;
  
  sprintf(sqlq,"select max(int_repl_num) from %sMDAS_AD_COMPOUND where data_id = %i and repl_enum = %i and seg_num= %i",
	  MDASSCHEMENAME,  objId, objReplNum,objSegNum);
    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i != 0) return (MDAS_FAILURE);
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = get_row_db2(hstmt);
    if (i != 0) {
      i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
      failure = DATA_NOT_IN_CAT;
      return (DATA_NOT_IN_CAT);
    }
    else  {
      truncate_end_blanks(data_size[0], data[0]);
      if (strlen((char *)data[0]) == 0)
	r_num = 0;
      else
	r_num = 1 + atoi( (char *) data[0]);
      i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
      return(r_num);
    }
}

int
getMaxCompoundSegNum(int objId, int objReplNum, int objSegNum, int intReplNum)
{
  int r_num;
  
  sprintf(sqlq,"select max(int_seg_num) from %sMDAS_AD_COMPOUND where data_id = %i and repl_enum = %i and seg_num= %i and int_repl_num = %i",
	  MDASSCHEMENAME,  objId, objReplNum,objSegNum,intReplNum);
    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i != 0) return (MDAS_FAILURE);
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = get_row_db2(hstmt);
    if (i != 0) {
      i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
      failure = DATA_NOT_IN_CAT;
      return (DATA_NOT_IN_CAT);
    }
    else  {
      truncate_end_blanks(data_size[0], data[0]);
      if (strlen((char *)data[0]) == 0)
	r_num = 0;
      else
	r_num = 1 + atoi( (char *) data[0]);
      i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
      return(r_num);
    }
}

int
checkIfCompoundObjectExists(int objId, int objReplNum, int objSegNum, 
			    int intReplNum, int intSegNum)
{
  /* returns MDAS_SUCCESS if it DOES NOT EXIST */

    sprintf(sqlq, "select data_id from %sMDAS_AD_COMPOUND where data_id = %i and repl_enum = %i and seg_num= %i and int_repl_num = %i and int_seg_num= %i",
	    MDASSCHEMENAME,  objId, objReplNum, objSegNum,
	    intReplNum, intSegNum);
   if (check_exists(sqlq) == MDAS_SUCCESS) 
      return(INTERNAL_COMPOUND_OBJECT_IS_NOT_UNIQUE);
   else 
     return(MDAS_SUCCESS);


}

int
getDataIdRsrcIdFromObj(char *objCollName, char *objName,
		       int objReplNum,int objSegNum,
		       int *objId, int *rsrcId)
{
    int i,dnum[MAX_ITYPE_LIST_COUNT], cnum[MAX_ITYPE_LIST_COUNT];
    int rowcount;
  
    sprintf(sqlq, "select  t1.data_id, t1.rsrc_id from %sMDAS_AD_REPL t1 , %sMDAS_TD_DATA_GRP t2 where t1.data_name = '%s' and t1.repl_enum = %i and t1. seg_num = %i and t2.data_grp_name = '%s' and t1.data_grp_id = t2.data_grp_id ",
	    MDASSCHEMENAME, MDASSCHEMENAME, 
	    objName, objReplNum, objSegNum,
	    objCollName);
  i = get_2itype_list_from_query(dnum,cnum,&rowcount,sqlq);
  if (i != 0 || rowcount == 0) {
    return (DATA_NOT_IN_CAT);
  }
  *objId = dnum[0];
  *rsrcId = cnum[0];
  return(MDAS_SUCCESS);  
}

int
getChildCompoundRsrcId(parRsrcId,resourceName)
{
  /* given parentCompoundResource Identifier and child Resource Name
     gets the child resource Id */
  int i;
  
  sprintf(sqlq, "select t1.phy_rsrc_id from %sMDAS_AR_CMPND_RSRC t1, %sMDAS_AR_PHYSICAL t2 where t1.cmpnd_rsrc_id = %i and t1.phy_rsrc_id = t2.phy_rsrc_id and t2.phy_rsrc_name = '%s'",
	  MDASSCHEMENAME, MDASSCHEMENAME, 
	  parRsrcId,resourceName);
  i =  get_itype_value_from_query(sqlq);
  return(i);
  
}


int
deleteDataInfoFromMCAT(char *objDataName, int objUserId,int objDataId, 
		       int objReplNum,int objSegNum)
{
  int j;
  int inSuffPriv = 0;
  int accsId ;
  char tmpStr[MAX_TOKEN];
  /* check for permission */
  accsId = getAccessId("all audit");
   if (objReplNum >= 0 ) {
     sprintf(sqlq, "select data_id from %sMDAS_AD_ACCS  where data_id = %i and repl_enum =  %i and user_id = %i and access_id >= %i", 
	     MDASSCHEMENAME, objDataId, objReplNum, objUserId, accsId);
     if (check_exists(sqlq) != MDAS_SUCCESS) {
       /**  RAJA removed April 6, 2004 to allow superuser to remove it 
       return (USER_HAS_INSUFFICIENT_PRIVILEGE);
       RAJA removed April 6, 2004 to allow superuser to remove it */
       i = isLocalPrivUser();
       if (i != 0)
	 return (ACTION_NOT_ALLOWED);
     }
   }
   else if ( isLocalPrivUser()) { /* removing all data */
     inSuffPriv = 0;
   }
   else     { /* removing all data */
     sprintf(sqlq, "select data_id from %sMDAS_AD_ACCS  where data_id = %i and user_id = %i and access_id >= %i", 
	     MDASSCHEMENAME, objDataId, objUserId, accsId);
     if (check_exists(sqlq) != MDAS_SUCCESS)
       return (USER_HAS_INSUFFICIENT_PRIVILEGE);
     sprintf(sqlq, "select data_id from %sMDAS_AD_ACCS  where data_id = %i and user_id = %i and access_id < %i", 
	     MDASSCHEMENAME, objDataId, objUserId, accsId);
     if (check_exists(sqlq) == MDAS_SUCCESS)
       inSuffPriv = 1; /* there exists at least one replica for which
			  the user has insufficient privilege */
   }
 
  if (objSegNum >= 0) {
    sprintf(atname[0],"data_id");
    sprintf(atname[1],"seg_num");
    sprintf(atname[2],"repl_enum");
    sprintf(atval[0],"%i",objDataId);
    sprintf(atval[1],"%i",objSegNum);
    sprintf(atval[2],"%i",objReplNum);
    if (inSuffPriv == 0) {
      if (objReplNum >= 0 ) j = 3;
      else  j = 2;
      i = deleteFromTable(atname, atval,"MDAS_AD_REPL",j);
      if (i != 0) 
	return (MDAS_AD_REPL_DELETION_ERROR);
    }
    else {
      i = deleteFromTableWithAllDataAccessCheck(atname, atval,"MDAS_AD_REPL",2,accsId,objUserId);
      if (i != 0) 
	return (MDAS_AD_REPL_DELETION_ERROR);
    }
  }
  else {/* seg_num < 0 */
    sprintf(atname[0],"data_id");
    sprintf(atname[1],"repl_enum");
    sprintf(atval[0],"%i",objDataId);
    sprintf(atval[1],"%i",objReplNum);
    if (inSuffPriv == 1) {
      i = deleteFromTableWithAllDataAccessCheck(atname, atval,"MDAS_AD_REPL",1, accsId,objUserId);
      if (i != 0) 
	return (MDAS_AD_REPL_DELETION_ERROR);
      i = deleteFromTableWithAllDataAccessCheck(atname, atval,"MDAS_AD_ACCS",1, accsId,objUserId);
      if (i != 0) 
	return (MDAS_AD_ACCS_DELETION_ERROR);
      
    }
    else {/*inSuffPriv == 0 */ 
      if (objReplNum >= 0 )
	sprintf(sqlq, "select data_id from %sMDAS_AD_REPL   where data_id = %i and repl_enum <> %i", MDASSCHEMENAME, objDataId,objReplNum);
      if (check_exists(sqlq) != MDAS_SUCCESS)
	objReplNum = -1;

      if (objReplNum >= 0 ) j = 2;
      else    j = 1;
      failure = 0;
      i = deleteFromTable(atname, atval,"MDAS_AD_ACCS",j);
      i =deleteFromTable(atname,atval,"MDAS_AD_AUDIT",j);
      /*      i =deleteFromTable(atname,atval,"MDAS_AD_COMMENTS",j);
	      depercated ****/
      i =deleteFromTable(atname,atval,"MDAS_AD_AUDIT",j);
      i=deleteFromTable(atname,atval,"MDAS_AD_ANNOTATION",j);
      i=deleteFromTable(atname,atval,"MDAS_AD_MISC1",j);
      if (objReplNum < 0){
	i=deleteFromTable(atname,atval,"MDAS_AU_TCKT_DATA",1);
	i =deleteFromTable(atname,atval,"MDAS_AD_MDATA",1);
#ifndef LEANMCAT
	i=deleteFromTable(atname,atval,"MDAS_AD_CLASS",1);
	sprintf(atname[0],"strblngsto_data_id");
	i=deleteFromTable(atname,atval,"MDAS_AD_STRCT_BLOB",1);
	sprintf(atname[0],"INDEXED_DATA_ID");
	i=deleteFromTable(atname,atval,"MDAS_AD_DATA_INDEX",1);
	sprintf(atname[0],"DATA_ID_OF_INDEX");
	i=deleteFromTable(atname,atval,"MDAS_AD_DATA_INDEX",1);
	sprintf(atname[0]," METHFOR_DATA_ID");
	i=deleteFromTable(atname,atval,"MDAS_AD_DATA_METH",1);
	sprintf(atname[0]," DATA_ID_OF_METH");	 
	i=deleteFromTable(atname,atval,"MDAS_AD_DATA_METH",1);
	sprintf(atname[0]," DATA_ID_OF_METH");	 
	i=deleteFromTable(atname,atval,"MDAS_AD_DTYP_METH",1);
	sprintf(atname[0],"DATA_ID_OF_INDEX");
	i=deleteFromTable(atname,atval,"MDAS_AD_COLL_INDEX",1);
	sprintf(atname[0],"DATA_ID_OF_METH");
	i=deleteFromTable(atname,atval,"MDAS_AD_COLL_METH",1);
#endif /* LEANMCAT */
	sprintf(atname[0],"data_id");
      }
      i = deleteFromTable(atname, atval,"MDAS_AD_REPL",j);
      if (i != 0) 
	return (MDAS_AD_REPL_DELETION_ERROR);
    }
  }
  sprintf(tmpStr,"%i",objDataId);
  removeTabledQueries(tmpStr);
  removeTabledQueries(objDataName);
  return(MDAS_SUCCESS);
  
  
}

int
modify_zone_info(int   cat_type, 
		 char *regUserName,
		 char *regUserDomain,
                 char *zone_name,
                 char *data_value_1, 
                 char *data_value_2,
                 char *data_value_3,
                 char *data_value_4,
                 char *data_value_5,
                 int   action_type)
{
  int i,j,ii;
  char *ptr1, *ptr2, *ptr3;
  int uId, adminId;
  char locnId[MAX_TOKEN];
  char chuser[MAX_TOKEN];
  char chdomain[MAX_TOKEN];
  char timeStamp[MAX_TOKEN];
  char newStr[MAX_TOKEN + 3];
  char myNewZoneNameColl[MAX_TOKEN + 3];
  char myZoneNameColl[MAX_TOKEN + 3];
  failure = 0;

  get_time(timeStamp);
  adminId = get_user_id_in_domain(regUserName,regUserDomain);
  if (adminId < 0) { return (MCAT_USER_NOT_IN_DOMN);}

  switch (action_type)
    {
    case Z_INSERT_NEW_LOCAL_ZONE:
      sprintf(sqlq,"select zone_id from %sMDAS_TD_ZONE where zone_id = '%s'",
	      MDASSCHEMENAME, zone_name);
      if (check_exists(sqlq) == MDAS_SUCCESS)
	return (ZONE_NAME_ALREADY_EXISTS);
      sprintf(sqlq,"select zone_id from %sMDAS_TD_ZONE where local_zone_flag = 1",
	      MDASSCHEMENAME);
      if (check_exists(sqlq) == MDAS_SUCCESS)
	return (LOCAL_ZONE_ALREADY_EXISTS);
      tabledIsOkay = 1;
      splitbychar(data_value_3, chuser, chdomain,'@');
      uId = get_user_id_in_domain(chuser,  chdomain);
      tabledIsOkay = 0;
      if ( uId < 0) 
	return (USER_NOT_IN_DOMN);
      tabledIsOkay = 1;
      get_ctype_value(locnId, "MDAS_TD_LOCN.locn_desc", data_value_1);
      tabledIsOkay = 0;
      if (failure != 0) { return (MDAS_TD_LOCN_ACCESS_ERROR);}
      sprintf(sqlq,"ZoneName=%s, ZoneAdmin=%s,ZoneLocation=%s,ZonePort=%s,ZoneContact=%s,ZoneComment=%s",
	  zone_name, data_value_3,data_value_1,data_value_2,data_value_4,
	  data_value_5);
      performAudit(action_type,adminId,0,"",sqlq);
      sprintf(sqlq,"insert into %sMDAS_TD_ZONE (zone_id,local_zone_flag,locn_id,port_number,user_id,zone_status,zone_create_date,zone_modify_date,zone_contact,zone_comments) values ('%s',1,'%s',%i,%i,1,'%s','%s','%s','%s')",
	       MDASSCHEMENAME, zone_name, locnId, atoi(data_value_2), uId, 
	       timeStamp, timeStamp,data_value_4,data_value_5);
      i = ExecSqlDb2(henv, hdbc, sqlq);
      if (i != 0) 		      
	return(MDAS_TD_ZONE_INSERTION_ERROR);
      i = make_new_collection(cat_type, "/", zone_name,regUserName,regUserDomain);
      if (i != 0)	{
	commit_db2_interaction(MDAS_CAT_ROLLBACK);
	return(i);
      }
      sprintf(newStr,"/%s",zone_name);
      i = make_new_collection(cat_type, newStr, DEF_PARENT_COLL,regUserName,regUserDomain);
      if (i != 0)	{
	commit_db2_interaction(MDAS_CAT_ROLLBACK);
	return(i);
      }
      i = make_new_collection(cat_type, newStr, DEF_CONTAINER_COLL,regUserName,regUserDomain);
      if (i != 0)	{
	commit_db2_interaction(MDAS_CAT_ROLLBACK);
	return(i);
      }
      i = make_new_collection(cat_type, newStr, DEF_TRASH_COLL,regUserName,regUserDomain);
      if (i != 0)	{
	commit_db2_interaction(MDAS_CAT_ROLLBACK);
	return(i);
      }
      sprintf(newStr,"/%s%s",zone_name,DEF_TRASH_COLL);
      i = modify_collection_info(0, regUserName, newStr,
			       PUBLIC_USER, PUBLIC_USER_DOMAIN, "write",
			       D_INSERT_COLL_ACCS, regUserDomain);
      if (i != 0)  {
	commit_db2_interaction(MDAS_CAT_ROLLBACK);
	return(i);
      }
      i = make_new_collection(cat_type,newStr,DEF_PARENT_COLL,regUserName,regUserDomain);
      if (i != 0)	{
	commit_db2_interaction(MDAS_CAT_ROLLBACK);
	return(i);
      }
      i = make_new_collection(cat_type, newStr, DEF_CONTAINER_COLL,regUserName,regUserDomain);
      if (i != 0)	{
	commit_db2_interaction(MDAS_CAT_ROLLBACK);
	return(i);
      }
      sprintf(newStr,"/%s%s%s",zone_name,DEF_TRASH_COLL,DEF_PARENT_COLL);
      i = modify_collection_info(0, regUserName, newStr,
			       PUBLIC_USER, PUBLIC_USER_DOMAIN, "write",
			       D_INSERT_COLL_ACCS, regUserDomain);
      if (i != 0)  {
	commit_db2_interaction(MDAS_CAT_ROLLBACK);
	return(i);
      }
      sprintf(newStr,"/%s%s%s",zone_name,DEF_TRASH_COLL,DEF_CONTAINER_COLL);
      i = modify_collection_info(0, regUserName, newStr,
			       PUBLIC_USER, PUBLIC_USER_DOMAIN, "write",
			       D_INSERT_COLL_ACCS, regUserDomain);
      if (i != 0)  {
	commit_db2_interaction(MDAS_CAT_ROLLBACK);
	return(i);
      }
      
      break;
    case Z_INSERT_NEW_ALIEN_ZONE:
      sprintf(sqlq,"select zone_id from %sMDAS_TD_ZONE where zone_id = '%s'",
	      MDASSCHEMENAME, zone_name);
      if (check_exists(sqlq) == MDAS_SUCCESS)
	return (ZONE_NAME_ALREADY_EXISTS);
      tabledIsOkay = 1;
      splitbychar(data_value_3, chuser, chdomain,'@');
      uId = get_user_id_in_domain(chuser,  chdomain);
      tabledIsOkay = 0;
      if ( uId < 0) 
	return (USER_NOT_IN_DOMN);
      tabledIsOkay = 1;
      get_ctype_value(locnId, "MDAS_TD_LOCN.locn_desc", data_value_1);
      tabledIsOkay = 0;
      if (failure != 0) { return (MDAS_TD_LOCN_ACCESS_ERROR);}
      sprintf(sqlq,"ZoneName=%s, ZoneAdmin=%s,ZoneLocation=%s,ZonePort=%s,ZoneContact=%s,ZoneComment=%s",
	  zone_name, data_value_3,data_value_1,data_value_2,data_value_4,
	  data_value_5);
      performAudit(action_type,adminId,0,"",sqlq);
      sprintf(sqlq,"insert into %sMDAS_TD_ZONE (zone_id,local_zone_flag,locn_id,port_number,user_id,zone_status,zone_create_date,zone_modify_date,zone_contact,zone_comments) values ('%s',0,'%s',%i,%i,1,'%s','%s','%s','%s')",
	       MDASSCHEMENAME, zone_name, locnId, atoi(data_value_2), uId, 
	       timeStamp, timeStamp,data_value_4,data_value_5);
      i = ExecSqlDb2(henv, hdbc, sqlq);
      if (i != 0) 		      
	return(MDAS_TD_ZONE_INSERTION_ERROR);
      i = make_new_collection(cat_type, "/", zone_name,regUserName,regUserDomain);
      if (i != 0)	{
	commit_db2_interaction(MDAS_CAT_ROLLBACK);
	return(i);
      }
      sprintf(newStr,"/%s",zone_name);
      i = make_new_collection(cat_type, newStr, DEF_PARENT_COLL,regUserName,regUserDomain);
      if (i != 0)	{
	commit_db2_interaction(MDAS_CAT_ROLLBACK);
	return(i);
      }
      i = make_new_collection(cat_type, newStr, DEF_CONTAINER_COLL,regUserName,regUserDomain);
      if (i != 0)	{
	commit_db2_interaction(MDAS_CAT_ROLLBACK);
	return(i);
      }
      i = make_new_collection(cat_type, newStr, DEF_TRASH_COLL,regUserName,regUserDomain);
      if (i != 0)	{
	commit_db2_interaction(MDAS_CAT_ROLLBACK);
	return(i);
      }
      sprintf(newStr,"/%s%s",zone_name,DEF_TRASH_COLL);
      i = modify_collection_info(0, regUserName, newStr,
			       PUBLIC_USER, PUBLIC_USER_DOMAIN, "write",
			       D_INSERT_COLL_ACCS, regUserDomain);
    if (i != 0)  {
	commit_db2_interaction(MDAS_CAT_ROLLBACK);
	return(i);
      }
      i = make_new_collection(cat_type,newStr,DEF_PARENT_COLL,regUserName,regUserDomain);
      if (i != 0)	{
	commit_db2_interaction(MDAS_CAT_ROLLBACK);
	return(i);
      }
      i = make_new_collection(cat_type, newStr, DEF_CONTAINER_COLL,regUserName,regUserDomain);
      if (i != 0)	{
	commit_db2_interaction(MDAS_CAT_ROLLBACK);
	return(i);
      }
      sprintf(newStr,"/%s%s%s",zone_name,DEF_TRASH_COLL,DEF_PARENT_COLL);
      i = modify_collection_info(0, regUserName, newStr,
			       PUBLIC_USER, PUBLIC_USER_DOMAIN, "write",
			       D_INSERT_COLL_ACCS, regUserDomain);
      if (i != 0)  {
	commit_db2_interaction(MDAS_CAT_ROLLBACK);
	return(i);
      }
      sprintf(newStr,"/%s%s%s",zone_name,DEF_TRASH_COLL,DEF_CONTAINER_COLL);
      i = modify_collection_info(0, regUserName, newStr,
			       PUBLIC_USER, PUBLIC_USER_DOMAIN, "write",
			       D_INSERT_COLL_ACCS, regUserDomain);
      if (i != 0)  {
	commit_db2_interaction(MDAS_CAT_ROLLBACK);
	return(i);
      }

      break;
    case Z_MODIFY_ZONE_INFO:
      sprintf(sqlq,"select zone_id from %sMDAS_TD_ZONE where zone_id = '%s'",
	      MDASSCHEMENAME, zone_name);
      if (check_exists(sqlq) != MDAS_SUCCESS)
	return (ZONE_NAME_NOT_IN_CAT);
      j = 0;
      sprintf(sqlq,"ZoneName=%s", zone_name);
      if (strlen(data_value_1) > 0) {
	tabledIsOkay = 1;
	get_ctype_value(locnId, "MDAS_TD_LOCN.locn_desc", data_value_1);
	tabledIsOkay = 0;
	if (failure != 0) { return (MDAS_TD_LOCN_ACCESS_ERROR);}
	sprintf(atname[j],"locn_id");
	sprintf(atval[j],"'%s'",locnId);
	j++;
	strcat(sqlq,"ZoneLocation=");
	strcat(sqlq,data_value_1);
      }
      if (strlen(data_value_2) > 0) {
	sprintf(atname[j],"port_number");
	sprintf(atval[j],"%i",atoi(data_value_2));
	j++;
	strcat(sqlq,"ZonePort=");
	strcat(sqlq,data_value_2);
      }
      if (strlen(data_value_3) > 0) {
	tabledIsOkay = 1;
	splitbychar(data_value_3, chuser, chdomain,'@');
	uId = get_user_id_in_domain(chuser,  chdomain);
	tabledIsOkay = 0;
	if ( uId < 0) 
	  return (USER_NOT_IN_DOMN);
	sprintf(atname[j],"user_id");
	sprintf(atval[j],"%i",uId);
	j++;
	strcat(sqlq,"ZoneAdmin");
	strcat(sqlq,data_value_3);
      }
      if (strlen(data_value_4) > 0) {
	sprintf(atname[j],"zone_contact");
	sprintf(atval[j],"'%s'",data_value_4);
	j++;
	strcat(sqlq,"ZoneContact");
	strcat(sqlq,data_value_4);
      }
      if (strlen(data_value_5) > 0) {
	sprintf(atname[j],"zone_comments");
	sprintf(atval[j],"'%s'",data_value_5);
	j++;
	strcat(sqlq,"ZoneComment");
	strcat(sqlq,data_value_5);
      }
      performAudit(action_type,adminId,0,"",sqlq);
      sprintf(atname[j],"zone_modify_date");
      sprintf(atval[j],"'%s'",timeStamp);
      j++;
      sprintf(atname[j],"zone_id");
      sprintf(atval[j],"'%s'",zone_name);
      i = changeAttrValueSet(atname,atval,"MDAS_TD_ZONE",j+1,j);
      if (i != 0) return (MDAS_TD_ZONE_UPDATE_ERROR);
      break;
    case Z_MODIFY_ZONE_FOR_USER:
      sprintf(sqlq,"select zone_id from %sMDAS_TD_ZONE where zone_id = '%s'",
	      MDASSCHEMENAME, zone_name);
      if (check_exists(sqlq) != MDAS_SUCCESS)
	return (ZONE_NAME_NOT_IN_CAT);
      tabledIsOkay = 1;
      uId = get_user_id_in_domain(data_value_1,data_value_2);
      tabledIsOkay = 0;
      if ( uId < 0) 
	  return (USER_NOT_IN_DOMN);
      sprintf(atname[0],"zone_id");
      sprintf(atname[1],"user_id");
      sprintf(atval[0],"'%s'",zone_name);
      sprintf(atval[1],"%i",uId);
      sprintf(sqlq,"ZoneName=%s, User=%s@%s",zone_name, 
	      data_value_1,data_value_2);
      performAudit(action_type,adminId,0,"",sqlq);
      i = changeAttrValue(atname, atval,"MDAS_CD_USER", 2);
      if (i != 0) return (MDAS_CD_USER_UPDATE_ERROR);
      
      break;
    case Z_CHANGE_ZONE_NAME:
      sprintf(sqlq,"select zone_id from %sMDAS_TD_ZONE where zone_id = '%s'",
	      MDASSCHEMENAME, zone_name);
      if (check_exists(sqlq) != MDAS_SUCCESS)
	return (ZONE_NAME_NOT_IN_CAT);
      sprintf(sqlq,"select zone_id from %sMDAS_TD_ZONE where zone_id = '%s'",
	      MDASSCHEMENAME, data_value_1);
      if (check_exists(sqlq) == MDAS_SUCCESS)
	return (ZONE_NAME_ALREADY_EXISTS);
      sprintf(atname[0],"zone_id");
      sprintf(atname[1],"zone_modify_date");
      sprintf(atname[2],"zone_id");
      sprintf(atval[0],"'%s'",data_value_1);
      sprintf(atval[1],"'%s'",timeStamp);
      sprintf(atval[2],"'%s'",zone_name);
      sprintf(sqlq,"ZoneName=%s, NewZone=%s",zone_name, data_value_1);
      performAudit(action_type,adminId,0,"",sqlq);
      sprintf(sqlq,"ZoneName=%s, OldZone=%s",data_value_1,zone_name);
      performAudit(action_type,adminId,0,"",sqlq);
      i = changeAttrValueSet(atname, atval,"MDAS_TD_ZONE", 3,2);
      if (i != 0) return (MDAS_TD_ZONE_UPDATE_ERROR);
      sprintf(atname[1],"zone_id");
      sprintf(atval[1],"'%s'",zone_name);
      i = changeAttrValue(atname, atval,"MDAS_CD_USER", 2);
      if (i != 0) return (MDAS_CD_USER_UPDATE_ERROR);
      sprintf(myZoneNameColl,"/%s",zone_name);
      sprintf(myNewZoneNameColl,"/%s",data_value_1);
      /*** change all values for collections to this new zone ***/
      i = modify_collection_info(cat_type, regUserName,myZoneNameColl,
				 myNewZoneNameColl,"","",
			       D_CHANGE_COLL_NAME, regUserDomain);
      if (i != 0) 
	return (i);
      break;
    case Z_MODIFY_ZONE_LOCAL_FLAG:
      j= atoi(data_value_1);
      if (j == 1) {
	sprintf(sqlq,"select zone_id from %sMDAS_TD_ZONE where local_zone_flag = 1",
		MDASSCHEMENAME);
	if (check_exists(sqlq) == MDAS_SUCCESS)
	  return (LOCAL_ZONE_ALREADY_EXISTS);
      }
      
      sprintf(atname[0],"local_zone_flag");
      sprintf(atname[1],"zone_modify_date");
      sprintf(atname[2],"zone_id");
      sprintf(atval[0],"%i",j);
      sprintf(atval[1],"'%s'",timeStamp);
      sprintf(atval[2],"'%s'",zone_name);
      j++;
      sprintf(sqlq,"ZoneName=%s, ZoneFlag=%s",zone_name, data_value_1);
      performAudit(action_type,adminId,0,"",sqlq);
      i = changeAttrValueSet(atname, atval,"MDAS_TD_ZONE", 3,2);
      if (i != 0) return (MDAS_TD_ZONE_UPDATE_ERROR);
      break;
    case Z_MODIFY_ZONE_STATUS:
      sprintf(atname[0],"zone_status");
      sprintf(atname[1],"zone_modify_date");
      sprintf(atname[2],"zone_id");
      sprintf(atval[0],"%i",atoi(data_value_1));
      sprintf(atval[1],"'%s'",timeStamp);
      sprintf(atval[2],"'%s'",zone_name);
      sprintf(sqlq,"ZoneName=%s, ZoneStatus=%s",zone_name, data_value_1);
      performAudit(action_type,adminId,0,"",sqlq);
      i = changeAttrValueSet(atname, atval,"MDAS_TD_ZONE", 3,2);
      if (i != 0) return (MDAS_TD_ZONE_UPDATE_ERROR);
      break;
    default:
      return (MDAS_CATALOG_MODIFY_TYPE_ERROR);
      break;
    }
  return(MDAS_SUCCESS);
}

int
get_bulk_query_answers (int   cat_type, 
			char *queryInfo,  
			mdasC_sql_result_struct **myresult,
			int rowsWanted,
			char *userName, 
			char *domainName)
{
  int i,j;
  char *tmpPtr, *tmpPtr2;
  char qval[MAX_DCS_NUM][MAX_TOKEN];
  int  selval[MAX_DCS_NUM];
  int dirInfoQueryType = 0;  
  for (i = 0; i < MAX_DCS_NUM; i++)  {
    sprintf(qval[i],"");
    selval[i] = 0;
  }   
  if (strstr(queryInfo,"GET_USER_TYPE") == queryInfo) {
    selval[USER_TYP_NAME] = 1;
    selval[PARENT_USER_TYPE] = 1;
#ifdef MCAT_VERSION_10
    sprintf(spsqlq,"select t1.user_typ_name,  t2.user_typ_name from %sMDAS_TD_USER_TYP t1, %sMDAS_TD_USER_TYP t2  where  length(rtrim(t1.user_typ_id)) = length(rtrim(t2.user_typ_id)) + 5  and rtrim(t2.user_typ_id) = substr(t1.user_typ_id,1,length(rtrim(t2.user_typ_id)))", MDASSCHEMENAME, MDASSCHEMENAME);
#else
    sprintf(spsqlq,"select t1.user_typ_name,  t2.user_typ_name from %sMDAS_TD_USER_TYP t1, %sMDAS_TD_USER_TYP t2  where  length(t1.user_typ_id) = length(t2.user_typ_id) + 5  and t2.user_typ_id = substr(t1.user_typ_id,1,length(t2.user_typ_id))", MDASSCHEMENAME, MDASSCHEMENAME);
#endif

    dirInfoQueryType = 0;

  }
  else if (strstr(queryInfo,"GET_USER_DOMAIN") == queryInfo) {
    selval[DOMAIN_DESC] = 1;
    selval[PARENT_DOMAIN_DESC] = 1;
#ifdef MCAT_VERSION_10
    sprintf(spsqlq,"select t1.domain_desc,  t2.domain_desc from %sMDAS_TD_DOMN t1, %sMDAS_TD_DOMN t2  where  length(rtrim(t1.domain_id)) = length(rtrim(t2.domain_id)) + 5  and rtrim(t2.domain_id) = substr(t1.domain_id,1,length(rtrim(t2.domain_id)))", MDASSCHEMENAME, MDASSCHEMENAME);
#else
    sprintf(spsqlq,"select t1.domain_desc,  t2.domain_desc from %sMDAS_TD_DOMN t1, %sMDAS_TD_DOMN t2  where  length(t1.domain_id) = length(t2.domain_id) + 5  and t2.domain_id = substr(t1.domain_id,1,length(t2.domain_id))", MDASSCHEMENAME, MDASSCHEMENAME);
#endif
    dirInfoQueryType = 0;
  }
  else if (strstr(queryInfo,"GET_USER_AUTH_SCHEME") == queryInfo) {
    selval[USER_AUTH_SCHEME] = 1;
    dirInfoQueryType = 1;
  }
  else if (strstr(queryInfo,"GET_LOCATION_INFO") == queryInfo) {

    selval[SERVER_LOCATION] = 1;
    selval[SERVER_NETPREFIX] = 1;
    selval[USER_NAME] = 1;
    selval[DOMAIN_DESC] = 1;
    selval[ZONE_NAME] = 1;
    selval[PARENT_SERVER_LOCATION] = 1; 

#ifdef MCAT_VERSION_10
    sprintf(spsqlq,"select t3.user_name, t5.domain_desc, t1.locn_desc, t1.netprefix, t3.zone_id, t2.locn_desc from %sMDAS_TD_LOCN t1, %sMDAS_TD_LOCN t2, %sMDAS_CD_USER t3, %sMDAS_AU_DOMN t4, %sMDAS_TD_DOMN t5 where t1.server_user_id = t3.user_id and t3.user_id = t4.user_id and t4.domain_id = t5.domain_id and length(rtrim(t1.locn_id)) = length(rtrim(t2.locn_id)) + 5 AND rtrim(t2.locn_id) = substr(t1.locn_id,1,length(rtrim(t2.locn_id)))", 
	    MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME);
#else
    sprintf(spsqlq,"select t3.user_name, t5.domain_desc, t1.locn_desc, t1.netprefix, t3.zone_id, t2.locn_desc from %sMDAS_TD_LOCN t1, %sMDAS_TD_LOCN t2, %sMDAS_CD_USER t3, %sMDAS_AU_DOMN t4, %sMDAS_TD_DOMN t5 where t1.server_user_id = t3.user_id and t3.user_id = t4.user_id and t4.domain_id = t5.domain_id and length(t1.locn_id) = length(t2.locn_id) + 5 AND t2.locn_id = substr(t1.locn_id,1,length(t2.locn_id))", 
	    MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME);
#endif
    dirInfoQueryType = 0;

    
  }
  else if (strstr(queryInfo,"GET_RESOURCE_TYPE") == queryInfo) {
    selval[RSRC_TYP_NAME] = 1;
    selval[PARENT_RSRC_TYPE] = 1;
#ifdef MCAT_VERSION_10
    sprintf(spsqlq,"select t1.rsrc_typ_name,  t2.rsrc_typ_name from %sMDAS_TD_RSRC_TYP t1, %sMDAS_TD_RSRC_TYP t2  where  length(rtrim(t1.rsrc_typ_id)) = length(rtrim(t2.rsrc_typ_id)) + 5  and rtrim(t2.rsrc_typ_id) = substr(t1.rsrc_typ_id,1,length(rtrim(t2.rsrc_typ_id)))", MDASSCHEMENAME, MDASSCHEMENAME);
#else
    sprintf(spsqlq,"select t1.rsrc_typ_name,  t2.rsrc_typ_name from %sMDAS_TD_RSRC_TYP t1, %sMDAS_TD_RSRC_TYP t2  where  length(t1.rsrc_typ_id) = length(t2.rsrc_typ_id) + 5  and t2.rsrc_typ_id = substr(t1.rsrc_typ_id,1,length(t2.rsrc_typ_id))", MDASSCHEMENAME, MDASSCHEMENAME);
#endif
    dirInfoQueryType = 0;
  }
  else if (strstr(queryInfo,"GET_RESOURCE_CLASS") == queryInfo) {
    selval[RSRC_CLASS] = 1;
    dirInfoQueryType = 1;
  }
  else if (strstr(queryInfo,"GET_RESOURCE_ACCESS_CONSTRAINT") == queryInfo) {
    selval[RSRC_ACCESS_LIST] = 1;
    selval[RSRC_ACCESS_PRIVILEGE] = 1;
    selval[RSRC_ACCESS_CONSTRAINT] = 1;
    selval[RSRC_ACCESS_ID] = 1;
    dirInfoQueryType = 1;
    
  }
  else if (strstr(queryInfo,"GET_DATA_TYPE") == queryInfo) {

    selval[DATA_TYP_NAME] = 1; 
    selval[DATA_TYPE_MIME_STRING] = 1;
    selval[DATA_TYPE_EXTENDERS] = 1;
    selval[PARENT_DATA_TYPE] = 1;

#ifdef MCAT_VERSION_10
    sprintf(spsqlq,"select t1.data_typ_name, t3.data_typ_mime, t3.data_typ_ext, t2.data_typ_name from %sMDAS_TD_DATA_TYP t1, %sMDAS_TD_DATA_TYP t2, %sMDAS_AT_DATA_TYP_EXT  t3 where t1.data_typ_id = t3.data_typ_id and   length(rtrim(t1.data_typ_id)) = length(rtrim(t2.data_typ_id)) + 5  and rtrim(t2.data_typ_id) = substr(t1.data_typ_id,1,length(rtrim(t2.data_typ_id)))", MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME);
#else
    sprintf(spsqlq,"select t1.data_typ_name, t3.data_typ_mime, t3.data_typ_ext, t2.data_typ_name from %sMDAS_TD_DATA_TYP t1, %sMDAS_TD_DATA_TYP t2, %sMDAS_AT_DATA_TYP_EXT  t3 where t1.data_typ_id = t3.data_typ_id and   length(t1.data_typ_id) = length(t2.data_typ_id) + 5  and t2.data_typ_id = substr(t1.data_typ_id,1,length(t2.data_typ_id))", MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME);
#endif
    dirInfoQueryType = 0;


  }
  else if (strstr(queryInfo,"GET_DATA_ACCESS_CONSTRAINT") == queryInfo) {
    selval[DATA_ACCESS_LIST] = 1;
    selval[DATA_ACCESS_PRIVILEGE] = 1;
    selval[ACCESS_CONSTRAINT] = 1;
    selval[DATA_ACCESS_ID] = 1;
    dirInfoQueryType = 1;
    
  }
  else  if (strstr(queryInfo,"GET_CHANGED_ZONE_INFO") == queryInfo) {
    if ((tmpPtr = strstr(queryInfo,"|")) == NULL)
      return (BAD_QUERY_INFO_FORNAT);
      sprintf(qval[ZONE_MODIFY_DATE], " >= '%s'",tmpPtr + 1);
      *tmpPtr = '|';
      selval[ZONE_NAME] = 1;
      selval[ZONE_LOCALITY] = 1;
      selval[ZONE_LOCN_DESC] = 1;
      selval[ZONE_PORT_NUM] = 1;
      selval[ZONE_ADMIN_AUTH_SCHEME_NAME] = 1;
      selval[ZONE_ADMIN_DISTIN_NAME] = 1;
      selval[ZONE_STATUS] = 1;
      selval[ZONE_CREATE_DATE] = 1;
      selval[ZONE_MODIFY_DATE] = 1;
      selval[ZONE_COMMENTS] = 1;
      selval[ZONE_CONTACT] = 1;
      selval[ZONE_ADMIN_NAME] = 1;
      selval[ZONE_ADMIN_DOMAIN_NAME] = 1;
      dirInfoQueryType = 1;
  }
  else if (strstr(queryInfo,"GET_CHANGED_USER_INFO") == queryInfo) {
    if ((tmpPtr = strstr(queryInfo,"|")) == NULL)
      return (BAD_QUERY_INFO_FORNAT);
    sprintf(qval[USER_MODIFY_DATE], " >= '%s'",tmpPtr + 1);
    *tmpPtr = '|';
    selval[USER_NAME] = 1;
    selval[DOMAIN_DESC] = 1;
    selval[ZONE_NAME] = 1;
    selval[USER_TYP_NAME] = 1;
    selval[USER_ADDRESS] = 1;
    selval[USER_PHONE] = 1;
    selval[USER_EMAIL] = 1;
    selval[USER_CREATE_DATE] = 1;
    selval[USER_MODIFY_DATE] = 1;
    dirInfoQueryType = 1;
  }
  else if (strstr(queryInfo,"GET_CHANGED_USER_AUTHMAP") == queryInfo) {
    if ((tmpPtr = strstr(queryInfo,"|")) == NULL)
      return (BAD_QUERY_INFO_FORNAT);
    sprintf(qval[USER_MODIFY_DATE], " >= '%s'",tmpPtr + 1);
    *tmpPtr = '|';
    selval[USER_NAME] = 1;
    selval[DOMAIN_DESC] = 1;
    selval[ZONE_NAME] = 1;
    selval[USER_DISTIN_NAME] = 1;
    selval[USER_AUTH_SCHEME] = 1;
    dirInfoQueryType = 1;
    
  }
  else if (strstr(queryInfo,"GET_CHANGED_USER_GROUPS") == queryInfo) {
    if ((tmpPtr = strstr(queryInfo,"|")) == NULL)
      return (BAD_QUERY_INFO_FORNAT);
    sprintf(qval[USER_GROUP_MODIFY_DATE], " >= '%s'",tmpPtr + 1);
    *tmpPtr = '|';
    selval[USER_GROUP_NAME] = 1;
    selval[USER_GROUP_ZONE_NAME] = 1;
    selval[USER_GROUP_DOMAIN_DESC] = 1;
    selval[USER_NAME] = 1;
    selval[DOMAIN_DESC] = 1;
    selval[ZONE_NAME] = 1;
    dirInfoQueryType = 1;
    
  }
  else if (strstr(queryInfo,"GET_CHANGED_USER_UDEF_METADATA") == queryInfo) {
    if ((tmpPtr = strstr(queryInfo,"|")) == NULL)
      return (BAD_QUERY_INFO_FORNAT);
    sprintf(qval[USER_UDEF_MDATA_MODIFY_DATE], " >= '%s'",tmpPtr + 1);
    *tmpPtr = '|';
    dirInfoQueryType = 1;
    selval[USER_NAME] = 1;
    selval[DOMAIN_DESC] = 1;
    selval[ZONE_NAME] = 1;
    selval[METADATA_NUM_USER] = 1;
    selval[UDSMD_USER0] = 1;
    selval[UDSMD_USER1] = 1;
    selval[UDSMD_USER2] = 1;
    selval[UDSMD_USER3] = 1;
    selval[UDSMD_USER4] = 1;
    selval[UDSMD_USER5] = 1;
    selval[UDSMD_USER6] = 1;
    selval[UDSMD_USER7] = 1;
    selval[UDSMD_USER8] = 1;
    selval[UDSMD_USER9] = 1;
    selval[UDIMD_USER0] = 1;
    selval[UDIMD_USER1] = 1;
    selval[USER_UDEF_MDATA_MODIFY_DATE] = 1;
    selval[USER_UDEF_MDATA_CREATE_DATE] = 1;
  }
  else if (strstr(queryInfo,"GET_CHANGED_PHYSICAL_RESOURCE_CORE_INFO")==queryInfo ||
	   strstr(queryInfo,"GET_CHANGED_LOGICAL_RESOURCE_CORE_INFO")==queryInfo ){
    if ((tmpPtr = strstr(queryInfo,"|")) == NULL)
      return (BAD_QUERY_INFO_FORNAT);
    sprintf(qval[RSRC_MODIFY_DATE], " >= '%s'",tmpPtr + 1);
    *tmpPtr = '|';
    dirInfoQueryType = 1;
    selval[RSRC_NAME] = 1;
    selval[RSRC_TYP_NAME] = 1;
    selval[LOCATION_NAME] = 1;
    selval[RSRC_DEFAULT_PATH ] = 1;
    selval[RSRC_REPL_ENUM] = 1;
    selval[RSRC_COMMENTS] = 1;
    selval[RSRC_CREATE_DATE] = 1;
    selval[RSRC_MODIFY_DATE] = 1;
    selval[PHY_RSRC_NAME] = 1;
    selval[RSRC_CLASS] = 1;
    selval[RSRC_MAX_OBJ_SIZE] = 1;
    selval[RSRC_OWNER_NAME] = 1;
    selval[RSRC_OWNER_DOMAIN] = 1; 
    selval[RSRC_OWNER_ZONE] = 1; 
  }
  else if (strstr(queryInfo,"GET_CHANGED_RESOURCE_OTHER_INFO") ==queryInfo) {
    if ((tmpPtr = strstr(queryInfo,"|")) == NULL)
      return (BAD_QUERY_INFO_FORNAT);
    sprintf(qval[RSRC_MODIFY_DATE], " >= '%s'",tmpPtr + 1);
    *tmpPtr = '|';
    dirInfoQueryType = 1;
    selval[PHY_RSRC_NAME] = 1;    
    selval[RSRC_MLSEC_LATENCY_MAX] = 1;
    selval[RSRC_MLSEC_LATENCY_MIN] = 1;
    selval[RSRC_MBPS_BANDWIDTH] = 1;
    selval[RSRC_CONCURRENCY_MAX] = 1;
    selval[RSRC_NUM_OF_HIERARCHIES] = 1;
    selval[RSRC_NUM_OF_STRIPES] = 1;
    selval[RSRC_MEGAB_CAPACITY] = 1;
  }
  else if (strstr(queryInfo,"GET_CHANGED_RESOURCE_UDEF_METADATA") ==queryInfo){
    if ((tmpPtr = strstr(queryInfo,"|")) == NULL)
      return (BAD_QUERY_INFO_FORNAT);
    sprintf(qval[RSRC_UDEF_MDATA_MODIFY_DATE], " >= '%s'",tmpPtr + 1);
    *tmpPtr = '|';
    dirInfoQueryType = 1;
    selval[RSRC_NAME] = 1;
    selval[METADATA_NUM_RSRC] = 1;
    selval[UDSMD_RSRC0] = 1;
    selval[UDSMD_RSRC1] = 1;
    selval[UDSMD_RSRC2] = 1;
    selval[UDSMD_RSRC3] = 1;
    selval[UDSMD_RSRC4] = 1;
    selval[UDSMD_RSRC5] = 1;
    selval[UDSMD_RSRC6] = 1;
    selval[UDSMD_RSRC7] = 1;
    selval[UDSMD_RSRC8] = 1;
    selval[UDSMD_RSRC9] = 1;
    selval[UDIMD_RSRC0] = 1;
    selval[UDIMD_RSRC1] = 1;
    selval[RSRC_UDEF_MDATA_MODIFY_DATE] = 1;
    selval[RSRC_UDEF_MDATA_CREATE_DATE] = 1;
    
  }
  else if (strstr(queryInfo,"GET_CHANGED_RESOURCE_ACCESS") == queryInfo) {
    if ((tmpPtr = strstr(queryInfo,"|")) == NULL)
      return (BAD_QUERY_INFO_FORNAT);
    sprintf(qval[RSRC_MODIFY_DATE], " >= '%s'",tmpPtr + 1);
    *tmpPtr = '|';
    dirInfoQueryType = 1;
    selval[RSRC_NAME] = 1;
    selval[RSRC_ACCESS_CONSTRAINT] = 1;
    selval[RSRC_ACCS_GRPUSER_NAME] = 1;
    selval[RSRC_ACCS_GRPUSER_DOMAIN] = 1;
    selval[RSRC_ACCS_GRPUSER_ZONE] = 1;
  }
  else if (strstr(queryInfo,"GET_CHANGED_COLL_CORE_INFO")==queryInfo){
    dirInfoQueryType = 1;
    if ((tmpPtr = strstr(queryInfo,"|")) == NULL)
      return (BAD_QUERY_INFO_FORNAT);
    sprintf(qval[COLL_MODIFY_TIMESTAMP], " >= '%s'",tmpPtr + 1);
    *tmpPtr = '|';
    selval[COLLECTION_NAME] = 1;
    selval[PARENT_COLLECTION_NAME] = 1;
    selval[COLL_OWNER_NAME] = 1;
    selval[COLL_OWNER_DOMAIN] = 1;
    selval[COLL_OWNER_ZONE] = 1;
    selval[COLL_CREATE_TIMESTAMP] = 1;
    selval[COLL_MODIFY_TIMESTAMP] = 1;
    selval[COLL_COMMENTS] = 1;
    sprintf(specialOrderByClause," ORDER BY %s%s.%s",
	    MDASSCHEMENAME, "MDAS_TD_DATA_GRP","coll_mod_timestamp");
  }
  else if (strstr(queryInfo,"GET_CHANGED_DATA_CORE_INFO")==queryInfo){
    if ((tmpPtr = strstr(queryInfo,"|")) == NULL)
      return (BAD_QUERY_INFO_FORNAT);
    sprintf(qval[REPL_TIMESTAMP], " >= '%s'",tmpPtr + 1);
    *tmpPtr = '|';
    selval[DATA_NAME] = 1;
    selval[CONTAINER_REPL_ENUM] = 1;
    selval[DATA_TYP_NAME] = 1;
    selval[PATH_NAME] = 1;
    selval[RSRC_NAME] = 1;
    selval[REPL_TIMESTAMP] = 1;
    selval[COLLECTION_NAME] = 1;
    selval[SIZE] = 1;
    selval[DATA_IS_DELETED] = 1;
    selval[DATA_OWNER] = 1;
    selval[DATA_OWNER_DOMAIN] = 1;
    selval[DATA_OWNER_ZONE] = 1;
    selval[IS_DIRTY] = 1;
    selval[OFFSET] = 1;
    selval[DATA_CONT_NAME] = 1;
    selval[COLLECTION_CONT_NAME] = 1;
    selval[DATA_CREATE_TIMESTAMP] = 1;
    selval[DATA_LAST_ACCESS_TIMESTAMP] = 1;
    selval[DATA_LOCK_NUM] = 1;
    selval[DATA_LOCK_OWNER_NAME] = 1;
    selval[DATA_LOCK_OWNER_DOMAIN] = 1;
    selval[DATA_LOCK_OWNER_ZONE] = 1;
    selval[DATA_VER_NUM] = 1;
    selval[CONTAINER_NAME] = 1;
    selval[DATA_COMMENTS] = 1;
    selval[DATA_LOCK_EXPIRY] = 1;
    selval[DATA_CHECKSUM] = 1;
    selval[DATA_HIDE] = 1;
    selval[DATA_AUDITFLAG] = 1;
    selval[DATA_SEG_NUM] = 1;
    sprintf(specialOrderByClause," ORDER BY %s%s.%s",
	    MDASSCHEMENAME, "MDAS_AD_REPL","modify_timestamp");
    dirInfoQueryType = 1;
    
  }
  else if (strstr(queryInfo,"GET_CONTAINER_INFO") == queryInfo) {
    dirInfoQueryType = 1;
    selval[CONTAINER_NAME] = 1;
    selval[CONTAINER_MAX_SIZE] = 1;
    selval[CONTAINER_LOG_RSRC_NAME] = 1;
  }
  else if (strstr(queryInfo,"GET_CHANGED_DATA_ACCESS") == queryInfo) {
    if ((tmpPtr = strstr(queryInfo,"|")) == NULL)
      return (BAD_QUERY_INFO_FORNAT);
    sprintf(qval[REPL_TIMESTAMP], " >= '%s'",tmpPtr + 1);
    *tmpPtr = '|';
    dirInfoQueryType = 1;
    selval[DATA_CONT_NAME] = 1;
    selval[DATA_REPL_ENUM] = 1;
    selval[COLLECTION_CONT_NAME] = 1;
    selval[DATA_ACCS_GRPUSER_NAME] = 1;
    selval[DATA_ACCS_GRPUSER_DOMAIN] = 1;
    selval[DATA_ACCS_GRPUSER_ZONE] = 1;
    selval[ACCESS_CONSTRAINT] = 1;
  }
  else if (strstr(queryInfo,"GET_CHANGED_COLL_ACCESS") == queryInfo) {
    if ((tmpPtr = strstr(queryInfo,"|")) == NULL)
      return (BAD_QUERY_INFO_FORNAT);
    sprintf(qval[COLL_MODIFY_TIMESTAMP], " >= '%s'",tmpPtr + 1);
    *tmpPtr = '|';
    dirInfoQueryType = 1;
    selval[COLLECTION_NAME] = 1;
    selval[COLL_ACCS_GRPUSER_NAME] = 1;
    selval[COLL_ACCS_GRPUSER_DOMAIN] = 1;
    selval[COLL_ACCS_GRPUSER_ZONE] = 1;
    selval[COLL_ACCS_ACCESS_CONSTRAINT] = 1;
    
  }
  else if (strstr(queryInfo,"GET_CHANGED_COLLCONT_INFO") == queryInfo) {
    dirInfoQueryType = 1;
    if ((tmpPtr = strstr(queryInfo,"|")) == NULL)
      return (BAD_QUERY_INFO_FORNAT);
    sprintf(qval[COLL_MODIFY_TIMESTAMP], " >= '%s'",tmpPtr + 1);
    *tmpPtr = '|';
    dirInfoQueryType = 1;
    selval[COLLECTION_NAME] = 1;
    selval[CONTAINER_FOR_COLLECTION] = 1;
    selval[DATA_TYPE_FOR_CONTAINER_FOR_COLLECTION] = 1;

  }
  else if (strstr(queryInfo,"GET_CHANGED_DATA_UDEFMETA_INFO")==queryInfo){
   if ((tmpPtr = strstr(queryInfo,"|")) == NULL)
      return (BAD_QUERY_INFO_FORNAT);
    sprintf(qval[DATA_UDEF_MDATA_MODIFY_DATE], " >= '%s'",tmpPtr + 1);
    *tmpPtr = '|';
    dirInfoQueryType = 1;
    selval[DATA_NAME] = 1;
    selval[COLLECTION_NAME] = 1;
    selval[METADATA_NUM] = 1;
    selval[UDSMD0] = 1;
    selval[UDSMD1] = 1;
    selval[UDSMD2] = 1;
    selval[UDSMD3] = 1;
    selval[UDSMD4] = 1;
    selval[UDSMD5] = 1;
    selval[UDSMD6] = 1;
    selval[UDSMD7] = 1;
    selval[UDSMD8] = 1;
    selval[UDSMD9] = 1;
    selval[UDIMD0] = 1;
    selval[UDIMD1] = 1;
    selval[DATA_UDEF_MDATA_MODIFY_DATE] = 1;    
    selval[DATA_UDEF_MDATA_CREATE_DATE] = 1;    
  }
  else if (strstr(queryInfo,"GET_CHANGED_COLL_UDEFMETA_INFO")==queryInfo){
   if ((tmpPtr = strstr(queryInfo,"|")) == NULL)
      return (BAD_QUERY_INFO_FORNAT);
    sprintf(qval[COLL_UDEF_MDATA_MODIFY_DATE], " >= '%s'",tmpPtr + 1);
    *tmpPtr = '|';
    dirInfoQueryType = 1;
    selval[COLLECTION_NAME] = 1;
    selval[METADATA_NUM_COLL] = 1;
    selval[UDSMD_COLL0] = 1;
    selval[UDSMD_COLL1] = 1;
    selval[UDSMD_COLL2] = 1;
    selval[UDSMD_COLL3] = 1;
    selval[UDSMD_COLL4] = 1;
    selval[UDSMD_COLL5] = 1;
    selval[UDSMD_COLL6] = 1;
    selval[UDSMD_COLL7] = 1;
    selval[UDSMD_COLL8] = 1;
    selval[UDSMD_COLL9] = 1;
    selval[UDIMD_COLL0] = 1;
    selval[UDIMD_COLL1] = 1;
    selval[COLL_UDEF_MDATA_MODIFY_DATE] = 1;    
    selval[COLL_UDEF_MDATA_CREATE_DATE] = 1;    
  }
  else if (strstr(queryInfo,"GET_CHANGED_DATA_ANNOTATION_INFO")==queryInfo){
   if ((tmpPtr = strstr(queryInfo,"|")) == NULL)
      return (BAD_QUERY_INFO_FORNAT);
    sprintf(qval[DATA_ANNOTATION_TIMESTAMP], " >= '%s'",tmpPtr + 1);
    *tmpPtr = '|';
    selval[DATA_NAME] = 1;
    selval[COLLECTION_NAME] = 1;
    selval[DATA_ANNOTATION_USERNAME] = 1;    
    selval[DATA_ANNOTATION_USERDOMAIN] = 1;    
    selval[DATA_ANNOTATION_USER_ZONE] = 1;   
    selval[DATA_ANNOTATION] = 1;    
    selval[DATA_ANNOTATION_TIMESTAMP] = 1;    
    selval[DATA_ANNOTATION_POSITION] = 1;    
   dirInfoQueryType = 1;
    
  }
  else if (strstr(queryInfo,"GET_CHANGED_COLL_ANNOTATION_INFO") == queryInfo) {
   if ((tmpPtr = strstr(queryInfo,"|")) == NULL)
      return (BAD_QUERY_INFO_FORNAT);
    sprintf(qval[COLL_ANNOTATION_TIMESTAMP], " >= '%s'",tmpPtr + 1);
    *tmpPtr = '|';
    selval[COLLECTION_NAME] = 1;
    selval[COLL_ANNOTATION_USERNAME] = 1;    
    selval[COLL_ANNOTATION_USERDOMAIN] = 1;    
    selval[COLL_ANNOTATION_USER_ZONE] = 1;   
    selval[COLL_ANNOTATION] = 1;    
    selval[COLL_ANNOTATION_TIMESTAMP] = 1;    
    selval[COLL_ANNOTATION_TYPE] = 1;    
    dirInfoQueryType = 1;
    
  }
  else if (strstr(queryInfo,"GET_CHANGED_OTHERDATA_INFO") == queryInfo) {
    dirInfoQueryType = 1;
    selval[DATA_NAME] = 1;
    selval[COLLECTION_NAME] = 1;
    selval[CONTAINER_REPL_ENUM] = 1;
    selval[DATA_PIN_VAL] = 1;
    selval[DATA_PIN_OWNER_NAME] = 1;
    selval[DATA_PIN_OWNER_DOMAIN] = 1;
    selval[DATA_PIN_OWNER_ZONE] = 1;
    selval[DATA_PIN_EXPIRY] = 1;
    selval[DATA_EXPIRY_DATE] = 1;
    selval[DATA_IS_COMPRESSED] = 1;
    selval[DATA_IS_ENCRYPTED] = 1;
    selval[DATA_EXPIRE_DATE_2] = 1;
  }
  else if (strstr(queryInfo,"GET_CHANGED_DATA_GUID_INFO") == queryInfo) {
   if ((tmpPtr = strstr(queryInfo,"|")) == NULL)
      return (BAD_QUERY_INFO_FORNAT);
    sprintf(qval[GUID_TIME_STAMP], " >= '%s'",tmpPtr + 1);
    *tmpPtr = '|';
    selval[DATA_NAME] = 1;
    selval[COLLECTION_NAME] = 1;
    selval[GUID] = 1;
    selval[GUID_FLAG] = 1;
    selval[GUID_TIME_STAMP] = 1;
    dirInfoQueryType = 1;
  }
  else {
    return(FUNCTION_NOT_SUPPORTED);
  }
  if (dirInfoQueryType == 1) {
    i = get_data_dir_info(cat_type, qval, selval, myresult,
			  dcs_tname, dcs_aname, rowsWanted);
    strcpy(specialOrderByClause,"");
    if (i < 0)
      return(i);
  }
  else if (dirInfoQueryType == 0) {
    specialQueryFlag = 1;
    specialQueryString = spsqlq;
    i = get_data_dir_info(cat_type, qval, selval, myresult,
			  dcs_tname, dcs_aname, rowsWanted);
    specialQueryFlag = 0;
    specialQueryString = NULL;
    strcpy(specialOrderByClause,"");
    if (i < 0)
      return(i);
  }
  else if (strstr(queryInfo,"GETFORUSER_EXTENSIBLE_SCHEMA_INFO") == queryInfo) {
    return(FUNCTION_NOT_SUPPORTED);
  }
  else {
    return(FUNCTION_NOT_SUPPORTED);
  }
  return(MDAS_SUCCESS);
}

int
ingest_bulk_metadata(int   cat_type, 
		     char *ingestInfo,  
		     mdasC_sql_result_struct *myresult,
		     char *userName, 
		     char *domainName)
{
  int i,j,ii;
  int cnt = 0;
  char *tmpPtr, *tmpPtr2;
  char *rval[MAX_DCS_NUM];
  char sval[3][MAX_TOKEN];
  char *zone;
  char *actionInfo;

  actionInfo = ingestInfo;

  if (strstr(actionInfo,"PUT_USER_TYPE") == actionInfo) {
    rval[0] =  (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
	dcs_tname[USER_TYP_NAME], dcs_aname[USER_TYP_NAME]);
    if (rval[0] == NULL) 
      return (USER_TYP_NAME_NOT_FOUND);
    rval[1] =  (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
	dcs_tname[PARENT_USER_TYPE], dcs_aname[PARENT_USER_TYPE]);
    if (rval[1] == NULL) 
      return (PARENT_USER_TYPE_NOT_FOUND);
    for (i=0; i < myresult->row_count; i++) {
      ii = insertTokenInfo ("UserType",rval[0],rval[1],userName,domainName,"");
      if (ii == MDAS_SUCCESS)
	cnt++;
      else if (ii != TOKEN_ALREADY_IN_CAT) {
	elog(NOTICE, "ingest_bulk_metadata error for action %s: %i\n",actionInfo, ii);
	elog(NOTICE, "Bad Row No: %i Row=%s\n",i+1,rval[0]);
	return(ii);
      }
      rval[0] += MAX_DATA_SIZE;
      rval[1] += MAX_DATA_SIZE;
      
    }
    return(cnt);
  }
  else if (strstr(actionInfo,"PUT_USER_DOMAIN") == actionInfo) {
    rval[0] =  (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
	dcs_tname[DOMAIN_DESC], dcs_aname[DOMAIN_DESC]);
    if (rval[0] == NULL) 
      return (DOMAIN_DESC_NOT_FOUND);
    rval[1] =  (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
	dcs_tname[PARENT_DOMAIN_DESC], dcs_aname[PARENT_DOMAIN_DESC]);
    if (rval[1] == NULL) 
      return (PARENT_DOMAIN_DESC_NOT_FOUND);
    for (i=0; i < myresult->row_count; i++) {
      ii = insertTokenInfo ("Domain",rval[0],rval[1],userName,domainName,"");
      if (ii == MDAS_SUCCESS) {
	getRandomString(sval[0],10);
	ii = register_user_group(cat_type, rval[0],
			sval[0],
			"group",
			"", "", 
			"",
		        userName, "",domainName);
	 if (ii == MDAS_SUCCESS)
	   cnt++;
	 else {
	   elog(NOTICE, "ingest_bulk_metadata error for action when doing register_user_group %s: %i\n",actionInfo, ii);
	   elog(NOTICE, "Bad Row No: %i Row=%s\n",i+1,rval[0]);
	   return(ii);
	 }
      }
      else if (ii != TOKEN_ALREADY_IN_CAT) {
	elog(NOTICE, "ingest_bulk_metadata error for action %s: %i\n",actionInfo, ii);
	elog(NOTICE, "Bad Row No: %i Row=%s\n",i+1,rval[0]);
	return(ii);
      }
      rval[0] += MAX_DATA_SIZE;
      rval[1] += MAX_DATA_SIZE;
      
    }
    return(cnt);
  }
  else if (strstr(actionInfo,"PUT_USER_AUTH_SCHEME") == actionInfo) {
    rval[0] =  (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
	dcs_tname[USER_AUTH_SCHEME], dcs_aname[USER_AUTH_SCHEME]);
    if (rval[0] == NULL) 
      return (USER_AUTH_SCHEME_NOT_FOUND);
    for (i=0; i < myresult->row_count; i++) {
      ii = insertTokenInfo ("AuthScheme",rval[0],"",userName,domainName,"");
      if (ii == MDAS_SUCCESS)
	cnt++;
      else if (ii != TOKEN_ALREADY_IN_CAT) {
	elog(NOTICE, "ingest_bulk_metadata error for action %s: %i\n",actionInfo, ii);
	elog(NOTICE, "Bad Row No: %i Row=%s\n",i+1,rval[0]);
	return(ii);
      }
      rval[0] += MAX_DATA_SIZE;
      
    }
    return(cnt);
  }
  else if (strstr(actionInfo,"PUT_LOCATION_INFO") == actionInfo) {

    rval[0] =  (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[SERVER_LOCATION], dcs_aname[SERVER_LOCATION]);
    if (rval[0] == NULL) 
      return (SERVER_LOCATION_NOT_FOUND);
    rval[1] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[SERVER_NETPREFIX], dcs_aname[SERVER_NETPREFIX]);
    if (rval[1] == NULL) 
      return (SERVER_NETPREFIX_NOT_FOUND);
    rval[2] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[USER_NAME], dcs_aname[USER_NAME]);
    if (rval[2] == NULL) 
      return (USER_NAME_NOT_FOUND);
    rval[3] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DOMAIN_DESC], dcs_aname[DOMAIN_DESC]);
    if (rval[3] == NULL) 
      return (DOMAIN_DESC_NOT_FOUND);
    rval[4] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[ZONE_NAME], dcs_aname[ZONE_NAME]);
    if (rval[4] == NULL) 
      return (ZONE_NAME_NOT_FOUND);
    rval[5] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[PARENT_SERVER_LOCATION], dcs_aname[PARENT_SERVER_LOCATION]);
    if (rval[5] == NULL) 
      return (PARENT_SERVER_LOCATION_NOT_FOUND);

    for (i=0; i < myresult->row_count; i++) {
      ii = registerLocationInfo(rval[0],rval[1], 
		     rval[5],
		     rval[2], rval[3],
		     userName,domainName,"");
      if (ii == MDAS_SUCCESS)
	cnt++;
      else if (ii != TOKEN_ALREADY_IN_CAT) {
	elog(NOTICE, "ingest_bulk_metadata error for action %s: %i\n",actionInfo, ii);
	elog(NOTICE, "Bad Row No: %i Row=%s|%s|%s|%s|%s|%s\n",i+1,rval[0],rval[1],
	     rval[2],rval[3],rval[4],rval[5]);
	return(ii);
      }
      rval[0] += MAX_DATA_SIZE;
      rval[1] += MAX_DATA_SIZE;
      rval[2] += MAX_DATA_SIZE;
      rval[3] += MAX_DATA_SIZE;
      rval[4] += MAX_DATA_SIZE;
      rval[5] += MAX_DATA_SIZE;
      
    }
    return(cnt);
  }
  else if (strstr(actionInfo,"PUT_RESOURCE_TYPE") == actionInfo) {
    rval[0] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_TYP_NAME], dcs_aname[RSRC_TYP_NAME]);
    if (rval[0] == NULL) 
      return (RSRC_TYP_NAME_NOT_FOUND);
    rval[1] =  (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
	dcs_tname[PARENT_RSRC_TYPE], dcs_aname[PARENT_RSRC_TYPE]);
    if (rval[1] == NULL) 
      return (PARENT_RSRC_TYPE_NOT_FOUND);
    for (i=0; i < myresult->row_count; i++) {
      ii = insertTokenInfo ("ResourceType",rval[0],rval[1],userName,domainName,"");
      if (ii == MDAS_SUCCESS)
	cnt++;
      else if (ii != TOKEN_ALREADY_IN_CAT) {
	elog(NOTICE, "ingest_bulk_metadata error for action %s: %i\n",actionInfo, ii);
	elog(NOTICE, "Bad Row No: %i Row=%s\n",i+1,rval[0]);
	return(ii);
      }
      rval[0] += MAX_DATA_SIZE;
      rval[1] += MAX_DATA_SIZE;
      
    }
    return(cnt);
  }
  else if (strstr(actionInfo,"PUT_RESOURCE_CLASS") == actionInfo) {
    rval[0] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_CLASS], dcs_aname[RSRC_CLASS]);
    if (rval[0] == NULL) 
      return (RSRC_CLASS_NOT_FOUND);
    for (i=0; i < myresult->row_count; i++) {
      ii = insertTokenInfo ("ResourceClass",rval[0],"",userName,domainName,"");
      if (ii == MDAS_SUCCESS)
	cnt++;
      else if (ii != TOKEN_ALREADY_IN_CAT) {
	elog(NOTICE, "ingest_bulk_metadata error for action %s: %i\n",actionInfo, ii);
	elog(NOTICE, "Bad Row No: %i Row=%s\n",i+1,rval[0]);
	return(ii);
      }
      rval[0] += MAX_DATA_SIZE;
      
    }
    return(cnt);

  }
  else if (strstr(actionInfo,"PUT_RESOURCE_ACCESS_CONSTRAINT") == actionInfo) {
    rval[0] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_ACCESS_LIST], dcs_aname[RSRC_ACCESS_LIST]);
    if (rval[0] == NULL) 
      return (RSRC_ACCESS_LIST_NOT_FOUND);
    rval[1] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_ACCESS_PRIVILEGE], dcs_aname[RSRC_ACCESS_PRIVILEGE]);
    if (rval[1] == NULL) 
      return (RSRC_ACCESS_PRIVILEGE_NOT_FOUND);
    rval[2] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_ACCESS_CONSTRAINT], dcs_aname[RSRC_ACCESS_CONSTRAINT]);
    if (rval[2] == NULL) 
      return (RSRC_ACCESS_CONSTRAINT_NOT_FOUND);
    rval[3] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_ACCESS_ID], dcs_aname[RSRC_ACCESS_ID]);
    if (rval[3] == NULL) 
      return (RSRC_ACCESS_ID_NOT_FOUND);
    for (i=0; i < myresult->row_count; i++) {
      ii = insertRsrcAccessInfo (rval[0],rval[1],rval[2],rval[3],userName,domainName,"");
      if (ii == MDAS_SUCCESS)
	cnt++;
      else if (ii != ACCESS_ID_ALREADY_IN_CAT && ii != ACCESS_CONSTRAINT_ALREADY_IN_CAT) {
	elog(NOTICE, "ingest_bulk_metadata error for action %s: %i\n",actionInfo, ii);
	elog(NOTICE, "Bad Row No: %i Row=%s|%s|%s|%s\n",i+1,rval[0],rval[1],rval[2],rval[3]);
	return(ii);
      }
      rval[0] += MAX_DATA_SIZE;
      rval[1] += MAX_DATA_SIZE;
      rval[2] += MAX_DATA_SIZE;
      rval[3] += MAX_DATA_SIZE;

    }
    return(cnt);
  }
  else if (strstr(actionInfo,"PUT_DATA_TYPE") == actionInfo) {
    rval[0] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_TYP_NAME], dcs_aname[DATA_TYP_NAME]);
    if (rval[1] == NULL) 
      return (DATA_TYP_NAME_NOT_FOUND);
    rval[1] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_TYPE_MIME_STRING], dcs_aname[DATA_TYPE_MIME_STRING]);
    if (rval[1] == NULL) 
      return (DATA_TYPE_MIME_STRING_NOT_FOUND);
    rval[2] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_TYPE_EXTENDERS], dcs_aname[DATA_TYPE_EXTENDERS]);
    if (rval[2] == NULL) 
      return (DATA_TYPE_EXTENDERS_NOT_FOUND);
    rval[3] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[PARENT_DATA_TYPE], dcs_aname[PARENT_DATA_TYPE]);
    if (rval[3] == NULL) 
      return (PARENT_DATA_TYPE_NOT_FOUND);
    for (i=0; i < myresult->row_count; i++) {
	elog(NOTICE, "Row No: %i Row=%s|%s|%s|%s\n",i,rval[0],rval[1],rval[2],rval[3]);
      ii = insertTokenInfo ("DataType",rval[0],rval[3],userName,domainName,"");
      if (ii != MDAS_SUCCESS && ii != TOKEN_ALREADY_IN_CAT) {
	elog(NOTICE, "ingest_bulk_metadata error when ingesting new data type for action %s: %i\n",actionInfo, ii);
	elog(NOTICE, "Bad Row No: %i Row=%s|%s|%s|%s\n",i,rval[0],rval[1],rval[2],rval[3]);
	return(ii);
      }
      ii = insertDataMimeTypeInfo(rval[0],rval[1],rval[2],userName,domainName,"");
      if (ii == MDAS_SUCCESS)
	cnt++;
      else if (ii != MIME_EXT_ALREADY_IN_CAT) {
	elog(NOTICE, "ingest_bulk_metadata error  when ingesting new data mime and extensions for action %s: %i\n",actionInfo, ii);
	elog(NOTICE, "Bad Row No: %i Row=%s|%s|%s|%s\n",i+1,rval[0],rval[1],rval[2],rval[3]);
	return(ii);
      }
      rval[0] += MAX_DATA_SIZE;
      rval[1] += MAX_DATA_SIZE;
      rval[2] += MAX_DATA_SIZE;
      rval[3] += MAX_DATA_SIZE;
      
    }
    return(cnt);
  }
  else if (strstr(actionInfo,"PUT_DATA_ACCESS_CONSTRAINT") == actionInfo) {
    rval[0] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_ACCESS_LIST], dcs_aname[DATA_ACCESS_LIST]);
    if (rval[0] == NULL) 
      return (DATA_ACCESS_LIST_NOT_FOUND);
    rval[1] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_ACCESS_PRIVILEGE], dcs_aname[DATA_ACCESS_PRIVILEGE]);
    if (rval[1] == NULL) 
      return (DATA_ACCESS_PRIVILEGE_NOT_FOUND);
    rval[2] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[ACCESS_CONSTRAINT], dcs_aname[ACCESS_CONSTRAINT]);
    if (rval[2] == NULL) 
      return (ACCESS_CONSTRAINT_NOT_FOUND);
    rval[3] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_ACCESS_ID], dcs_aname[DATA_ACCESS_ID]);
    if (rval[3] == NULL) 
      return (DATA_ACCESS_ID_NOT_FOUND);
    
    for (i=0; i < myresult->row_count; i++) {
      ii = insertDataAccessInfo (rval[0],rval[1],rval[2],rval[3],userName,domainName,"");
      if (ii == MDAS_SUCCESS)
	cnt++;
      else if (ii != ACCESS_ID_ALREADY_IN_CAT && ii != ACCESS_CONSTRAINT_ALREADY_IN_CAT) {
	elog(NOTICE, "ingest_bulk_metadata error for action %s: %i\n",actionInfo, ii);
	elog(NOTICE, "Bad Row No: %i Row=%s|%s|%s|%s\n",i+1,rval[0],rval[1],rval[2],rval[3]);
	return(ii);
      }
      rval[0] += MAX_DATA_SIZE;
      rval[1] += MAX_DATA_SIZE;
      rval[2] += MAX_DATA_SIZE;
      rval[3] += MAX_DATA_SIZE;
      
    }
    return(cnt);
  }
  else  if (strstr(actionInfo,"PUT_CHANGED_ZONE_INFO") == actionInfo) {
    rval[0] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[ZONE_NAME], dcs_aname[ZONE_NAME]);
    if (rval[0] == NULL) 
      return (ZONE_NAME_NOT_FOUND);
    rval[1] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[ZONE_LOCALITY], dcs_aname[ZONE_LOCALITY]);
    if (rval[1] == NULL) 
      return (ZONE_LOCALITY_NOT_FOUND);
    rval[2] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[ZONE_LOCN_DESC], dcs_aname[ZONE_LOCN_DESC]);
    if (rval[2] == NULL) 
      return (ZONE_LOCN_DESC_NOT_FOUND);
    rval[3] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[ZONE_PORT_NUM], dcs_aname[ZONE_PORT_NUM]);
    if (rval[3] == NULL) 
      return (ZONE_PORT_NUM_NOT_FOUND);
    rval[4] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[ZONE_ADMIN_AUTH_SCHEME_NAME], dcs_aname[ZONE_ADMIN_AUTH_SCHEME_NAME]);
    if (rval[4] == NULL) 
      return (ZONE_ADMIN_AUTH_SCHEME_NAME_NOT_FOUND);
    rval[5] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[ZONE_ADMIN_DISTIN_NAME], dcs_aname[ZONE_ADMIN_DISTIN_NAME]);
    if (rval[5] == NULL) 
      return (ZONE_ADMIN_DISTIN_NAME_NOT_FOUND);
    rval[6] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[ZONE_STATUS], dcs_aname[ZONE_STATUS]);
    if (rval[6] == NULL) 
      return (ZONE_STATUS_NOT_FOUND);
    rval[7] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[ZONE_CREATE_DATE], dcs_aname[ZONE_CREATE_DATE]);
    if (rval[7] == NULL) 
      return (ZONE_CREATE_DATE_NOT_FOUND);
    rval[8] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[ZONE_MODIFY_DATE], dcs_aname[ZONE_MODIFY_DATE]);
    if (rval[8] == NULL) 
      return (ZONE_MODIFY_DATE_NOT_FOUND);
    rval[9] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[ZONE_COMMENTS], dcs_aname[ZONE_COMMENTS]);
    if (rval[9] == NULL) 
      return (ZONE_COMMENTS_NOT_FOUND);
    rval[10] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[ZONE_CONTACT], dcs_aname[ZONE_CONTACT]);
    if (rval[10] == NULL) 
      return (ZONE_CONTACT_NOT_FOUND);
    rval[11] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[ZONE_ADMIN_NAME], dcs_aname[ZONE_ADMIN_NAME]);
    if (rval[11] == NULL) 
      return (ZONE_ADMIN_NAME_NOT_FOUND);
    rval[12] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[ZONE_ADMIN_DOMAIN_NAME], dcs_aname[ZONE_ADMIN_DOMAIN_NAME]);
    if (rval[12] == NULL) 
      return (ZONE_ADMIN_DOMAIN_NAME_NOT_FOUND);


    for (i=0; i < myresult->row_count; i++) {
      sprintf(sval[0],"%s@%s",rval[11],rval[12]);
      ii = modify_zone_info(cat_type,userName,domainName,rval[0],rval[2],rval[3],sval[0],
			   rval[10],rval[9],Z_INSERT_NEW_ALIEN_ZONE);
      if (ii == MDAS_SUCCESS)
	cnt++;
      else if (ii != ZONE_NAME_ALREADY_EXISTS) {
	elog(NOTICE, "ingest_bulk_metadata error for action %s: %i\n",actionInfo, ii);
	elog(NOTICE, "Bad Row No: %i Row=%s\n",i+1,rval[0]);
	return(ii);
      }
      else {
	ii = modify_zone_info(cat_type,userName,domainName,rval[0],rval[6],"","","","",
			      Z_MODIFY_ZONE_STATUS);
	if (ii != MDAS_SUCCESS) {
	  elog(NOTICE, "ingest_bulk_metadata error for Z_MODIFY_ZONE_STATUS when action %s   and  bad status %s,: %i\n",
	       actionInfo,rval[6], ii);
	  elog(NOTICE, "Bad Row No: %i Row=%s\n",i+1,rval[0]);
	  return(ii);
	}
	ii  = modify_zone_info(cat_type,userName,domainName,rval[0],rval[2],
			       rval[3],sval[0],  rval[10],rval[9],
			      Z_MODIFY_ZONE_INFO);
	if (ii != MDAS_SUCCESS) {
	  elog(NOTICE, "ingest_bulk_metadata error for Z_MODIFY_ZONE_INFO when action %s   and  bad status %s,: %i\n",
	       actionInfo,rval[6], ii);
	  elog(NOTICE, "Bad Row No: %i Row=%s\n",i+1,rval[0]);
	  return(ii);
	}
      }

      
      rval[0] += MAX_DATA_SIZE;
      rval[1] += MAX_DATA_SIZE;
      rval[2] += MAX_DATA_SIZE;
      rval[3] += MAX_DATA_SIZE;
      rval[4] += MAX_DATA_SIZE;
      rval[5] += MAX_DATA_SIZE;
      rval[6] += MAX_DATA_SIZE;
      rval[7] += MAX_DATA_SIZE;
      rval[8] += MAX_DATA_SIZE;
      rval[9] += MAX_DATA_SIZE;
      rval[10] += MAX_DATA_SIZE;
      rval[11] += MAX_DATA_SIZE;
      rval[12] += MAX_DATA_SIZE;
      
    }
    return(cnt);
  }
  else if (strstr(actionInfo,"PUT_CHANGED_USER_INFO") == actionInfo) {
    rval[0] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[USER_NAME], dcs_aname[USER_NAME]);
    if (rval[0] == NULL) 
      return (USER_NAME_NOT_FOUND);
    rval[1] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DOMAIN_DESC], dcs_aname[DOMAIN_DESC]);
    if (rval[1] == NULL) 
      return (DOMAIN_DESC_NOT_FOUND);
    rval[2] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[ZONE_NAME], dcs_aname[ZONE_NAME]);
    if (rval[2] == NULL) 
      return (ZONE_NAME_NOT_FOUND);
    rval[3] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[USER_TYP_NAME], dcs_aname[USER_TYP_NAME]);
    if (rval[3] == NULL) 
      return (USER_TYP_NAME_NOT_FOUND);
    rval[4] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[USER_ADDRESS], dcs_aname[USER_ADDRESS]);
    if (rval[4] == NULL) 
      return (USER_ADDRESS_NOT_FOUND);
    rval[5] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[USER_PHONE], dcs_aname[USER_PHONE]);
    if (rval[5] == NULL) 
      return (USER_PHONE_NOT_FOUND);
    rval[6] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[USER_EMAIL], dcs_aname[USER_EMAIL]);
    if (rval[6] == NULL) 
      return (USER_EMAIL_NOT_FOUND);
    rval[7] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[USER_CREATE_DATE], dcs_aname[USER_CREATE_DATE]);
    if (rval[7] == NULL) 
      return (USER_CREATE_DATE_NOT_FOUND);
    rval[8] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[USER_MODIFY_DATE], dcs_aname[USER_MODIFY_DATE]);
    if (rval[8] == NULL) 
      return (USER_MODIFY_DATE_NOT_FOUND);
    for (i=0; i < myresult->row_count; i++) {
      getRandomString(sval[0],21);
      sprintf(sval[1],"%s|%s", rval[1], rval[2]);
      ii = register_user_info(cat_type, rval[0],
		       sval[0], sval[1],rval[3],rval[4],rval[5],rval[6],
		       userName,"",domainName);

      if (ii == MDAS_SUCCESS)
	cnt++;
      else if (ii != USER_NOT_UNIQUE_IN_DOMN) {
	elog(NOTICE, "ingest_bulk_metadata error for action %s: %i\n",actionInfo, ii);
	elog(NOTICE, "Bad Row No: %i Row=%s %s %s %s %s %s %s\n",i+1,rval[0],rval[1],rval[2],
	     rval[3],rval[4],rval[5],rval[6]);
	return(ii);
      }
      rval[0] += MAX_DATA_SIZE;
      rval[1] += MAX_DATA_SIZE;
      rval[2] += MAX_DATA_SIZE;
      rval[3] += MAX_DATA_SIZE;
      rval[4] += MAX_DATA_SIZE;
      rval[5] += MAX_DATA_SIZE;
      rval[6] += MAX_DATA_SIZE;
      rval[7] += MAX_DATA_SIZE;
      rval[8] += MAX_DATA_SIZE;
      
    }
    return(cnt);
  }
  else if (strstr(actionInfo,"PUT_CHANGED_USER_AUTHMAP") == actionInfo) {
    rval[0] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[USER_NAME], dcs_aname[USER_NAME]);
    if (rval[0] == NULL) 
      return (USER_NAME_NOT_FOUND);
    rval[1] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DOMAIN_DESC], dcs_aname[DOMAIN_DESC]);
    if (rval[1] == NULL) 
      return (DOMAIN_DESC_NOT_FOUND);
    rval[2] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[ZONE_NAME], dcs_aname[ZONE_NAME]);
    if (rval[2] == NULL) 
      return (ZONE_NAME_NOT_FOUND);
    rval[3] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[USER_DISTIN_NAME], dcs_aname[USER_DISTIN_NAME]);
    if (rval[3] == NULL) 
      return (USER_DISTIN_NAME_NOT_FOUND);
    rval[4] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[USER_AUTH_SCHEME], dcs_aname[USER_AUTH_SCHEME]);
    if (rval[4] == NULL) 
      return (USER_AUTH_SCHEME_NOT_FOUND);
    for (i=0; i < myresult->row_count; i++) {
      sprintf(sval[0],"%s@%s|%s",rval[0],rval[1],rval[2]);
      sprintf(sval[1],"%s:%s",rval[4],rval[3]);
      ii = modify_user_info(cat_type, userName,
		       sval[0],sval[1],
		       U_INSERT_AUTH_MAP, "", domainName);
      if (ii == MDAS_SUCCESS)
	cnt++;
      else if (ii != USER_AUTH_SCHEME_ALREADY_IN_CAT) {
	elog(NOTICE, "ingest_bulk_metadata error for action %s: %i\n",actionInfo, ii);
	elog(NOTICE, "Bad Row No: %i Row=%s\n",i+1,rval[0]);
	return(ii);
      }
      rval[0] += MAX_DATA_SIZE;
      rval[1] += MAX_DATA_SIZE;
      rval[2] += MAX_DATA_SIZE;
      rval[3] += MAX_DATA_SIZE;
      rval[4] += MAX_DATA_SIZE;
      
    }
    return(cnt);
    
  }
  else if (strstr(actionInfo,"PUT_CHANGED_USER_GROUPS") == actionInfo) {
    rval[0] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[USER_GROUP_NAME], dcs_aname[USER_GROUP_NAME]);
    if (rval[0] == NULL) 
      return (USER_GROUP_NAME_NOT_FOUND);
    rval[1] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[USER_GROUP_ZONE_NAME], dcs_aname[USER_GROUP_ZONE_NAME]);
    if (rval[1] == NULL) 
      return (USER_GROUP_ZONE_NAME_NOT_FOUND);
    rval[2] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[USER_GROUP_DOMAIN_DESC], dcs_aname[USER_GROUP_DOMAIN_DESC]);
    if (rval[2] == NULL) 
      return (USER_GROUP_DOMAIN_DESC_NOT_FOUND);
    rval[3] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[USER_NAME], dcs_aname[USER_NAME]);
    if (rval[3] == NULL) 
      return (USER_NAME_NOT_FOUND);
    rval[4] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DOMAIN_DESC], dcs_aname[DOMAIN_DESC]);
    if (rval[4] == NULL) 
      return (DOMAIN_DESC_NOT_FOUND);
    rval[5] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[ZONE_NAME], dcs_aname[ZONE_NAME]);
    if (rval[5] == NULL) 
      return (ZONE_NAME_NOT_FOUND);
    
    for (i=0; i < myresult->row_count; i++) {
      sprintf(sval[0],"%s@%s|%s",rval[3],rval[4],rval[5]);
      sprintf(sval[1],"%s|%s", rval[0],rval[1]);
      ii = modify_user_info(cat_type, userName,
		       sval[0],sval[1],
		       U_INSERT_GROUP, "", domainName);

      if (ii == MDAS_SUCCESS)
	cnt++;
      else if (ii != USER_IN_GROUP_ALREADY_IN_CAT) {
	elog(NOTICE, "ingest_bulk_metadata error for action %s: %i\n",actionInfo, ii);
	elog(NOTICE, "Bad Row No: %i Row=%s %s %s %s %s %s\n",i+1,rval[0],rval[1],
	     rval[2],rval[3],rval[4],rval[5]);
	return(ii);
      }
      rval[0] += MAX_DATA_SIZE;
      rval[1] += MAX_DATA_SIZE;
      rval[2] += MAX_DATA_SIZE;
      rval[3] += MAX_DATA_SIZE;
      rval[4] += MAX_DATA_SIZE;
      rval[5] += MAX_DATA_SIZE;

      
    }
    return(cnt);
  }
  else if (strstr(actionInfo,"PUT_CHANGED_USER_UDEF_METADATA") == actionInfo) {
    rval[0] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[USER_NAME], dcs_aname[USER_NAME]);
    if (rval[0] == NULL) 
      return (USER_NAME_NOT_FOUND);
    rval[1] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DOMAIN_DESC], dcs_aname[DOMAIN_DESC]);
    if (rval[1] == NULL) 
      return (DOMAIN_DESC_NOT_FOUND);
    rval[2] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[ZONE_NAME], dcs_aname[ZONE_NAME]);
    if (rval[2] == NULL) 
      return (ZONE_NAME_NOT_FOUND);
    rval[3] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD_USER0], dcs_aname[UDSMD_USER0]);
    if (rval[3] == NULL) 
      return (UDSMD_USER0_NOT_FOUND);
    rval[4] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD_USER1], dcs_aname[UDSMD_USER1]);
    if (rval[4] == NULL) 
      return (UDSMD_USER1_NOT_FOUND);
    rval[5] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD_USER2], dcs_aname[UDSMD_USER2]);
    if (rval[5] == NULL) 
      return (UDSMD_USER2_NOT_FOUND);
    rval[6] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD_USER3], dcs_aname[UDSMD_USER3]);
    if (rval[6] == NULL) 
      return (UDSMD_USER3_NOT_FOUND);
    rval[7] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD_USER4], dcs_aname[UDSMD_USER4]);
    if (rval[7] == NULL) 
      return (UDSMD_USER4_NOT_FOUND);
    rval[8] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD_USER5], dcs_aname[UDSMD_USER5]);
    if (rval[8] == NULL) 
      return (UDSMD_USER5_NOT_FOUND);
    rval[9] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD_USER6], dcs_aname[UDSMD_USER6]);
    if (rval[9] == NULL) 
      return (UDSMD_USER6_NOT_FOUND);
    rval[10] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD_USER7], dcs_aname[UDSMD_USER7]);
    if (rval[10] == NULL) 
      return (UDSMD_USER7_NOT_FOUND);
    rval[11] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD_USER8], dcs_aname[UDSMD_USER8]);
    if (rval[11] == NULL) 
      return (UDSMD_USER8_NOT_FOUND);
    rval[12] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD_USER9], dcs_aname[UDSMD_USER9]);
    if (rval[12] == NULL) 
      return (UDSMD_USER9_NOT_FOUND);
    rval[13] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDIMD_USER0], dcs_aname[UDIMD_USER0]);
    if (rval[13] == NULL) 
      return (UDIMD_USER0_NOT_FOUND);
    rval[14] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDIMD_USER1], dcs_aname[UDIMD_USER1]);
    if (rval[14] == NULL) 
      return (UDIMD_USER1_NOT_FOUND);
    rval[15] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[USER_UDEF_MDATA_MODIFY_DATE], dcs_aname[USER_UDEF_MDATA_MODIFY_DATE]);
    if (rval[15] == NULL) 
      return (USER_UDEF_MDATA_MODIFY_DATE_NOT_FOUND);
    rval[16] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[USER_UDEF_MDATA_CREATE_DATE], dcs_aname[USER_UDEF_MDATA_CREATE_DATE]);
    if (rval[16] == NULL) 
      return (USER_UDEF_MDATA_CREATE_DATE_NOT_FOUND);
    rval[17] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[METADATA_NUM_USER], dcs_aname[METADATA_NUM_USER]);
    if (rval[17] == NULL) 
      return (METADATA_NUM_USER_NOT_FOUND);
    for (i=0; i < myresult->row_count; i++) {
      sprintf(sval[0],"%s@%s|%s",rval[0],rval[1],rval[2]);
      ii = insertUserDefMetadataTable("User",sval[0],rval, 3,17);
      if (ii == MDAS_SUCCESS)
        cnt++;
      else if (ii != METADATA_ALREADY_IN_CAT) {
        elog(NOTICE, "ingest_bulk_metadata error for action %s: %i\n",actionInfo, ii);
        elog(NOTICE, "Bad Row No: %i Row=%s\n",i+1,rval[0]);
        return(ii);
      }

      rval[0] += MAX_DATA_SIZE;
      rval[1] += MAX_DATA_SIZE;
      rval[2] += MAX_DATA_SIZE;
      rval[3] += MAX_DATA_SIZE;
      rval[4] += MAX_DATA_SIZE;
      rval[5] += MAX_DATA_SIZE;
      rval[6] += MAX_DATA_SIZE;
      rval[7] += MAX_DATA_SIZE;
      rval[8] += MAX_DATA_SIZE;
      rval[9] += MAX_DATA_SIZE;
      rval[10] += MAX_DATA_SIZE;
      rval[11] += MAX_DATA_SIZE;
      rval[12] += MAX_DATA_SIZE;
      rval[13] += MAX_DATA_SIZE;
      rval[14] += MAX_DATA_SIZE;
      rval[15] += MAX_DATA_SIZE;
      rval[16] += MAX_DATA_SIZE;
      rval[17] += MAX_DATA_SIZE;
      
    }
    return(cnt);
  }
  else if (strstr(actionInfo,"PUT_CHANGED_PHYSICAL_RESOURCE_CORE_INFO")==actionInfo){
    rval[0] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_NAME], dcs_aname[RSRC_NAME]);
    if (rval[0] == NULL) 
      return (RSRC_NAME_NOT_FOUND);
    rval[1] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_TYP_NAME], dcs_aname[RSRC_TYP_NAME]);
    if (rval[1] == NULL) 
      return (RSRC_TYP_NAME_NOT_FOUND);
    rval[2] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[LOCATION_NAME], dcs_aname[LOCATION_NAME]);
    if (rval[2] == NULL) 
      return (LOCATION_NAME_NOT_FOUND);
    rval[3] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_DEFAULT_PATH], dcs_aname[RSRC_DEFAULT_PATH]);
    if (rval[3] == NULL) 
      return (RSRC_DEFAULT_PATH_NOT_FOUND);
    rval[4] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_REPL_ENUM], dcs_aname[RSRC_REPL_ENUM]);
    if (rval[4] == NULL) 
      return (RSRC_REPL_ENUM_NOT_FOUND);
    rval[5] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_COMMENTS], dcs_aname[RSRC_COMMENTS]);
    if (rval[5] == NULL) 
      return (RSRC_COMMENTS_NOT_FOUND);
    rval[6] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_CREATE_DATE], dcs_aname[RSRC_CREATE_DATE]);
    if (rval[6] == NULL) 
      return (RSRC_CREATE_DATE_NOT_FOUND);
    rval[7] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_MODIFY_DATE], dcs_aname[RSRC_MODIFY_DATE]);
    if (rval[7] == NULL) 
      return (RSRC_MODIFY_DATE_NOT_FOUND);
    rval[8] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[PHY_RSRC_NAME], dcs_aname[PHY_RSRC_NAME]);
    if (rval[8] == NULL) 
      return (PHY_RSRC_NAME_NOT_FOUND);
    rval[9] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_CLASS], dcs_aname[RSRC_CLASS]);
    if (rval[9] == NULL) 
      return (RSRC_CLASS_NOT_FOUND);
    rval[10] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_MAX_OBJ_SIZE], dcs_aname[RSRC_MAX_OBJ_SIZE]);
    if (rval[10] == NULL) 
      return (RSRC_MAX_OBJ_SIZE_NOT_FOUND);
    rval[11] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_OWNER_NAME], dcs_aname[RSRC_OWNER_NAME]);
    if (rval[11] == NULL) 
      return (RSRC_OWNER_NAME_NOT_FOUND);
    rval[12] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_OWNER_DOMAIN], dcs_aname[RSRC_OWNER_DOMAIN]);
    if (rval[12] == NULL) 
      return (RSRC_OWNER_DOMAIN_NOT_FOUND);
    rval[13] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_OWNER_ZONE], dcs_aname[RSRC_OWNER_ZONE]);
    if (rval[13] == NULL) 
      return (RSRC_OWNER_ZONE_NOT_FOUND);
    
    for (i=0; i < myresult->row_count; i++) {
      sprintf(sval[0],"%s|%s",rval[12],rval[13]);
      if (!strcmp(rval[0], rval[8])) {
	ii = registerResourceInfoInternal(rval[0],rval[1],rval[2],
		   userName,domainName,"", rval[3],
		     rval[9], atoi(rval[10]),rval[6],rval[7],rval[11],sval[0],rval[5]);
	  }
      else { /* add to logical */
	ii = LOGICAL_RESOURCE_ALREADY_IN_CAT;
      }
      if (ii == MDAS_SUCCESS)
	cnt++;
      else if (ii != LOGICAL_RESOURCE_ALREADY_IN_CAT &&
	       ii != RESOURCE_ALREADY_IN_CAT ) {
	elog(NOTICE, "ingest_bulk_metadata error for action %s: %i\n",actionInfo, ii);
	elog(NOTICE, "Bad Row No: %i Row=%s %s %s ... %s ...\n",i+1,rval[0],rval[1],rval[2],rval[8]);
	return(ii);
      }
      rval[0] += MAX_DATA_SIZE;
      rval[1] += MAX_DATA_SIZE;
      rval[2] += MAX_DATA_SIZE;
      rval[3] += MAX_DATA_SIZE;
      rval[4] += MAX_DATA_SIZE;
      rval[5] += MAX_DATA_SIZE;
      rval[6] += MAX_DATA_SIZE;
      rval[7] += MAX_DATA_SIZE;
      rval[8] += MAX_DATA_SIZE;
      rval[9] += MAX_DATA_SIZE;
      rval[10] += MAX_DATA_SIZE;
      rval[11] += MAX_DATA_SIZE;
      rval[12] += MAX_DATA_SIZE;
      rval[13] += MAX_DATA_SIZE;
      
    }
    return(cnt);
  }
  else if (strstr(actionInfo,"PUT_CHANGED_LOGICAL_RESOURCE_CORE_INFO")==actionInfo){
    rval[0] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_NAME], dcs_aname[RSRC_NAME]);
    if (rval[0] == NULL) 
      return (RSRC_NAME_NOT_FOUND);
    rval[1] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_TYP_NAME], dcs_aname[RSRC_TYP_NAME]);
    if (rval[1] == NULL) 
      return (RSRC_TYP_NAME_NOT_FOUND);
    rval[2] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[LOCATION_NAME], dcs_aname[LOCATION_NAME]);
    if (rval[2] == NULL) 
      return (LOCATION_NAME_NOT_FOUND);
    rval[3] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_DEFAULT_PATH], dcs_aname[RSRC_DEFAULT_PATH]);
    if (rval[3] == NULL) 
      return (RSRC_DEFAULT_PATH_NOT_FOUND);
    rval[4] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_REPL_ENUM], dcs_aname[RSRC_REPL_ENUM]);
    if (rval[4] == NULL) 
      return (RSRC_REPL_ENUM_NOT_FOUND);
    rval[5] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_COMMENTS], dcs_aname[RSRC_COMMENTS]);
    if (rval[5] == NULL) 
      return (RSRC_COMMENTS_NOT_FOUND);
    rval[6] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_CREATE_DATE], dcs_aname[RSRC_CREATE_DATE]);
    if (rval[6] == NULL) 
      return (RSRC_CREATE_DATE_NOT_FOUND);
    rval[7] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_MODIFY_DATE], dcs_aname[RSRC_MODIFY_DATE]);
    if (rval[7] == NULL) 
      return (RSRC_MODIFY_DATE_NOT_FOUND);
    rval[8] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[PHY_RSRC_NAME], dcs_aname[PHY_RSRC_NAME]);
    if (rval[8] == NULL) 
      return (PHY_RSRC_NAME_NOT_FOUND);
    rval[9] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_CLASS], dcs_aname[RSRC_CLASS]);
    if (rval[9] == NULL) 
      return (RSRC_CLASS_NOT_FOUND);
    rval[10] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_MAX_OBJ_SIZE], dcs_aname[RSRC_MAX_OBJ_SIZE]);
    if (rval[10] == NULL) 
      return (RSRC_MAX_OBJ_SIZE_NOT_FOUND);
    rval[11] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_OWNER_NAME], dcs_aname[RSRC_OWNER_NAME]);
    if (rval[11] == NULL) 
      return (RSRC_OWNER_NAME_NOT_FOUND);
    rval[12] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_OWNER_DOMAIN], dcs_aname[RSRC_OWNER_DOMAIN]);
    if (rval[12] == NULL) 
      return (RSRC_OWNER_DOMAIN_NOT_FOUND);
    rval[13] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_OWNER_ZONE], dcs_aname[RSRC_OWNER_ZONE]);
    if (rval[13] == NULL) 
      return (RSRC_OWNER_ZONE_NOT_FOUND);
    
    for (i=0; i < myresult->row_count; i++) {
      sprintf(sval[0],"%s|%s",rval[12],rval[13]);
      if (!strcmp(rval[0], rval[8])) {
	ii = LOGICAL_RESOURCE_ALREADY_IN_CAT;
      }
      else { /* add to logical */
	sprintf(query_string,
		"select  rsrc_id from %sMDAS_AR_REPL where rsrc_name = '%s'",
		MDASSCHEMENAME, rval[0]);
	if (check_exists(query_string) != 0) {
	  ii = registerLogicalResourceInfoInternal(rval[0],rval[1],rval[8],
		    userName,domainName,"", rval[3],rval[2],	  
                    rval[9],atoi(rval[10]),rval[6],rval[7],rval[11],sval[0],rval[5]);
	}
	else {
	  ii =  registerReplicateResourceInfoInternal(rval[8],rval[1],rval[0],
		    userName,domainName,"", rval[3],rval[2],	  
                    rval[9],atoi(rval[10]),rval[6],rval[7],rval[11],sval[0],rval[5]);
	}
      }
      if (ii == MDAS_SUCCESS)
	cnt++;
      else if (ii != LOGICAL_RESOURCE_ALREADY_IN_CAT &&
	       ii != RESOURCE_ALREADY_IN_CAT ) {
	elog(NOTICE, "ingest_bulk_metadata error for action %s: %i\n",actionInfo, ii);
	elog(NOTICE, "Bad Row No: %i Row=%s %s %s ... %s ...\n",i+1,rval[0],rval[1],rval[2],rval[8]);
	return(ii);
      }
      rval[0] += MAX_DATA_SIZE;
      rval[1] += MAX_DATA_SIZE;
      rval[2] += MAX_DATA_SIZE;
      rval[3] += MAX_DATA_SIZE;
      rval[4] += MAX_DATA_SIZE;
      rval[5] += MAX_DATA_SIZE;
      rval[6] += MAX_DATA_SIZE;
      rval[7] += MAX_DATA_SIZE;
      rval[8] += MAX_DATA_SIZE;
      rval[9] += MAX_DATA_SIZE;
      rval[10] += MAX_DATA_SIZE;
      rval[11] += MAX_DATA_SIZE;
      rval[12] += MAX_DATA_SIZE;
      rval[13] += MAX_DATA_SIZE;
      
    }
    return(cnt);
  }
  else if (strstr(actionInfo,"PUT_CHANGED_RESOURCE_OTHER_INFO") ==actionInfo) {
    rval[0] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[PHY_RSRC_NAME], dcs_aname[PHY_RSRC_NAME]);
    if (rval[0] == NULL) 
      return (PHY_RSRC_NAME_NOT_FOUND);
    rval[1] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_MLSEC_LATENCY_MAX], dcs_aname[RSRC_MLSEC_LATENCY_MAX]);
    if (rval[1] == NULL) 
      return (RSRC_MLSEC_LATENCY_MAX_NOT_FOUND);
    rval[2] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_MLSEC_LATENCY_MIN], dcs_aname[RSRC_MLSEC_LATENCY_MIN]);
    if (rval[2] == NULL) 
      return (RSRC_MLSEC_LATENCY_MIN_NOT_FOUND);
    rval[3] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_MBPS_BANDWIDTH], dcs_aname[RSRC_MBPS_BANDWIDTH]);
    if (rval[3] == NULL) 
      return (RSRC_MBPS_BANDWIDTH_NOT_FOUND);
    rval[4] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_CONCURRENCY_MAX], dcs_aname[RSRC_CONCURRENCY_MAX]);
    if (rval[4] == NULL) 
      return (RSRC_CONCURRENCY_MAX_NOT_FOUND);
    rval[5] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_NUM_OF_HIERARCHIES], dcs_aname[RSRC_NUM_OF_HIERARCHIES]);
    if (rval[5] == NULL) 
      return (RSRC_NUM_OF_HIERARCHIES_NOT_FOUND);
    rval[6] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_NUM_OF_STRIPES], dcs_aname[RSRC_NUM_OF_STRIPES]);
    if (rval[6] == NULL) 
      return (RSRC_NUM_OF_STRIPES_NOT_FOUND);
    rval[7] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_MEGAB_CAPACITY], dcs_aname[RSRC_MEGAB_CAPACITY]);
    if (rval[7] == NULL) 
      return (RSRC_MEGAB_CAPACITY_NOT_FOUND);
    for (i=0; i < myresult->row_count; i++) {
      ii = insertResourceInfo (rval[0],rval[1],rval[2],rval[3],rval[4],rval[5],rval[6],
			      rval[7],"",userName,domainName,"");
      if (ii == MDAS_SUCCESS)
	cnt++;
      else if (ii != INFO_ALREADY_IN_CAT) {
	elog(NOTICE, "ingest_bulk_metadata error for action %s: %i\n",actionInfo, ii);
	elog(NOTICE, "Bad Row No: %i Row=%s\n",i+1,rval[0]);
	return(ii);
      }
      rval[0] += MAX_DATA_SIZE;
      rval[1] += MAX_DATA_SIZE;
      rval[2] += MAX_DATA_SIZE;
      rval[3] += MAX_DATA_SIZE;
      rval[4] += MAX_DATA_SIZE;
      rval[5] += MAX_DATA_SIZE;
      rval[6] += MAX_DATA_SIZE;
      rval[7] += MAX_DATA_SIZE;
      
    }
    return(cnt);


  }
  else if (strstr(actionInfo,"PUT_CHANGED_RESOURCE_UDEF_METADATA") ==actionInfo){

    rval[0] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_NAME], dcs_aname[RSRC_NAME]);
    if (rval[0] == NULL) 
      return (RSRC_NAME_NOT_FOUND);
    rval[3] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD_RSRC0], dcs_aname[UDSMD_RSRC0]);
    if (rval[3] == NULL) 
      return (UDSMD_RSRC0_NOT_FOUND);
    rval[4] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD_RSRC1], dcs_aname[UDSMD_RSRC1]);
    if (rval[4] == NULL) 
      return (UDSMD_RSRC1_NOT_FOUND);
    rval[5] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD_RSRC2], dcs_aname[UDSMD_RSRC2]);
    if (rval[5] == NULL) 
      return (UDSMD_RSRC2_NOT_FOUND);
    rval[6] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD_RSRC3], dcs_aname[UDSMD_RSRC3]);
    if (rval[6] == NULL) 
      return (UDSMD_RSRC3_NOT_FOUND);
    rval[7] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD_RSRC4], dcs_aname[UDSMD_RSRC4]);
    if (rval[7] == NULL) 
      return (UDSMD_RSRC4_NOT_FOUND);
    rval[8] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD_RSRC5], dcs_aname[UDSMD_RSRC5]);
    if (rval[8] == NULL) 
      return (UDSMD_RSRC5_NOT_FOUND);
    rval[9] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD_RSRC6], dcs_aname[UDSMD_RSRC6]);
    if (rval[9] == NULL) 
      return (UDSMD_RSRC6_NOT_FOUND);
    rval[10] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD_RSRC7], dcs_aname[UDSMD_RSRC7]);
    if (rval[10] == NULL) 
      return (UDSMD_RSRC7_NOT_FOUND);
    rval[11] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD_RSRC8], dcs_aname[UDSMD_RSRC8]);
    if (rval[11] == NULL) 
      return (UDSMD_RSRC8_NOT_FOUND);
    rval[12] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD_RSRC9], dcs_aname[UDSMD_RSRC9]);
    if (rval[12] == NULL) 
      return (UDSMD_RSRC9_NOT_FOUND);
    rval[13] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDIMD_RSRC0], dcs_aname[UDIMD_RSRC0]);
    if (rval[13] == NULL) 
      return (UDIMD_RSRC0_NOT_FOUND);
    rval[14] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDIMD_RSRC1], dcs_aname[UDIMD_RSRC1]);
    if (rval[14] == NULL) 
      return (UDIMD_RSRC1_NOT_FOUND);
    rval[15] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_UDEF_MDATA_MODIFY_DATE], dcs_aname[RSRC_UDEF_MDATA_MODIFY_DATE]);
    if (rval[15] == NULL) 
      return (RSRC_UDEF_MDATA_MODIFY_DATE_NOT_FOUND);
    rval[16] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_UDEF_MDATA_CREATE_DATE], dcs_aname[RSRC_UDEF_MDATA_CREATE_DATE]);
    if (rval[16] == NULL) 
      return (RSRC_UDEF_MDATA_CREATE_DATE_NOT_FOUND);
    rval[17] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[METADATA_NUM_RSRC], dcs_aname[METADATA_NUM_RSRC]);
    if (rval[17] == NULL) 
      return (METADATA_NUM_RSRC_NOT_FOUND);
    for (i=0; i < myresult->row_count; i++) {
      ii = insertUserDefMetadataTable("Resource",rval[0],rval, 3,17);
      if (ii == MDAS_SUCCESS)
        cnt++;
      else if (ii != METADATA_ALREADY_IN_CAT) {
        elog(NOTICE, "ingest_bulk_metadata error for action %s: %i\n",actionInfo, ii);
        elog(NOTICE, "Bad Row No: %i Row=%s\n",i+1,rval[0]);
        return(ii);
      }

      rval[0] += MAX_DATA_SIZE;
      rval[1] += MAX_DATA_SIZE;
      rval[2] += MAX_DATA_SIZE;
      rval[3] += MAX_DATA_SIZE;
      rval[4] += MAX_DATA_SIZE;
      rval[5] += MAX_DATA_SIZE;
      rval[6] += MAX_DATA_SIZE;
      rval[7] += MAX_DATA_SIZE;
      rval[8] += MAX_DATA_SIZE;
      rval[9] += MAX_DATA_SIZE;
      rval[10] += MAX_DATA_SIZE;
      rval[11] += MAX_DATA_SIZE;
      rval[12] += MAX_DATA_SIZE;
      rval[13] += MAX_DATA_SIZE;
      rval[14] += MAX_DATA_SIZE;
      rval[15] += MAX_DATA_SIZE;
      rval[16] += MAX_DATA_SIZE;
      rval[17] += MAX_DATA_SIZE;
    }
    return(cnt);
  }
  else if (strstr(actionInfo,"PUT_CHANGED_RESOURCE_ACCESS") == actionInfo) {
    rval[0] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_NAME], dcs_aname[RSRC_NAME]);
    if (rval[0] == NULL) 
      return (RSRC_NAME_NOT_FOUND);
    rval[1] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_ACCESS_CONSTRAINT], dcs_aname[RSRC_ACCESS_CONSTRAINT]);
    if (rval[1] == NULL) 
      return (RSRC_ACCESS_CONSTRAINT_NOT_FOUND);
    rval[2] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_ACCS_GRPUSER_NAME], dcs_aname[RSRC_ACCS_GRPUSER_NAME]);
    if (rval[2] == NULL) 
      return (RSRC_ACCS_GRPUSER_NAME_NOT_FOUND);
    rval[3] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_ACCS_GRPUSER_DOMAIN], dcs_aname[RSRC_ACCS_GRPUSER_DOMAIN]);
    if (rval[3] == NULL) 
      return (RSRC_ACCS_GRPUSER_DOMAIN_NOT_FOUND);
    rval[4] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_ACCS_GRPUSER_ZONE], dcs_aname[RSRC_ACCS_GRPUSER_ZONE]);
    if (rval[4] == NULL) 
      return (RSRC_ACCS_GRPUSER_ZONE_NOT_FOUND);

    for (i=0; i < myresult->row_count; i++) {
      sprintf(sval[0],"%s@%s|%s",rval[2],rval[3],rval[4]);
      ii = modify_resource_info (cat_type,rval[0],userName,domainName,R_INSERT_ACCS,
				sval[0],rval[1],"","");
      if (ii == MDAS_SUCCESS)
	cnt++;
      else  {
	elog(NOTICE, "ingest_bulk_metadata error for action %s: %i\n",actionInfo, ii);
	elog(NOTICE, "Bad Row No: %i Row=%s\n",i+1,rval[0]);
	return(ii);
      }
      rval[0] += MAX_DATA_SIZE;
      rval[1] += MAX_DATA_SIZE;
      rval[2] += MAX_DATA_SIZE;
      rval[3] += MAX_DATA_SIZE;
      rval[4] += MAX_DATA_SIZE;
      
    }
    return(cnt);
  }
  else if (strstr(actionInfo,"PUT_CHANGED_COLL_CORE_INFO")==actionInfo){
    rval[0] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[COLLECTION_NAME], dcs_aname[COLLECTION_NAME]);
    if (rval[0] == NULL) 
      return (COLLECTION_NAME_NOT_FOUND);
    rval[1] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[PARENT_COLLECTION_NAME], dcs_aname[PARENT_COLLECTION_NAME]);
    if (rval[1] == NULL) 
      return (PARENT_COLLECTION_NAME_NOT_FOUND);
    rval[2] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[COLL_OWNER_NAME], dcs_aname[COLL_OWNER_NAME]);
    if (rval[2] == NULL) 
      return (COLL_OWNER_NAME_NOT_FOUND);
    rval[3] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[COLL_OWNER_DOMAIN], dcs_aname[COLL_OWNER_DOMAIN]);
    if (rval[3] == NULL) 
      return (COLL_OWNER_DOMAIN_NOT_FOUND);
    rval[4] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[COLL_OWNER_ZONE], dcs_aname[COLL_OWNER_ZONE]);
    if (rval[4] == NULL) 
      return (COLL_OWNER_ZONE_NOT_FOUND);
    rval[5] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[COLL_CREATE_TIMESTAMP],dcs_aname[COLL_CREATE_TIMESTAMP]);
    if (rval[5] == NULL) 
      return (COLL_CREATE_TIMESTAMP_NOT_FOUND);
    rval[6] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[COLL_MODIFY_TIMESTAMP],dcs_aname[COLL_MODIFY_TIMESTAMP]);
    if (rval[6] == NULL) 
      return (COLL_MODIFY_TIMESTAMP_NOT_FOUND);
    rval[7] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[COLL_COMMENTS], dcs_aname[COLL_COMMENTS]);
    if (rval[7] == NULL) 
      return (COLL_COMMENTS_NOT_FOUND);
    for (i=0; i < myresult->row_count; i++) {
      zoneChangeCollName(rval[0]);
      zoneChangeCollName(rval[1]);
      ii = ibmCollCore(cat_type,rval,userName,domainName);
      if (ii == MDAS_SUCCESS)
        cnt++;
      else if (ii != DATA_SUBCOLLECTION_NOT_UNIQUE) {
        elog(NOTICE, "ingest_bulk_metadata error for action %s: %i\n",actionInfo, ii);
        elog(NOTICE, "Bad Row No: %i Row=%s\n",i+1,rval[0]);
        return(ii);
      }
      rval[0] += MAX_DATA_SIZE;
      rval[1] += MAX_DATA_SIZE;
      rval[2] += MAX_DATA_SIZE;
      rval[3] += MAX_DATA_SIZE;
      rval[4] += MAX_DATA_SIZE;
      rval[5] += MAX_DATA_SIZE;
      rval[6] += MAX_DATA_SIZE;
      rval[7] += MAX_DATA_SIZE;

    }
    return(cnt);

   
  }
  else if (strstr(actionInfo,"PUT_CHANGED_DATA_CORE_INFO")==actionInfo){
    rval[0] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
    if (rval[0] == NULL) 
      return (DATA_NAME_NOT_FOUND);
    rval[1] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[CONTAINER_REPL_ENUM], dcs_aname[CONTAINER_REPL_ENUM]);
    if (rval[1] == NULL) 
      return (CONTAINER_REPL_ENUM_NOT_FOUND);
    rval[2] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_TYP_NAME], dcs_aname[DATA_TYP_NAME]);
    if (rval[2] == NULL) 
      return (DATA_TYP_NAME_NOT_FOUND);
    rval[3] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[PATH_NAME], dcs_aname[PATH_NAME]);
    if (rval[3] == NULL) 
      return (PATH_NAME_NOT_FOUND);
    rval[4] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_NAME], dcs_aname[RSRC_NAME]);
    if (rval[4] == NULL) 
      return (RSRC_NAME_NOT_FOUND);
    rval[5] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[REPL_TIMESTAMP], dcs_aname[REPL_TIMESTAMP]);
    if (rval[5] == NULL) 
      return (REPL_TIMESTAMP_NOT_FOUND);
    rval[6] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[COLLECTION_NAME], dcs_aname[COLLECTION_NAME]);
    if (rval[6] == NULL) 
      return (COLLECTION_NAME_NOT_FOUND);
    rval[7] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[SIZE], dcs_aname[SIZE]);
    if (rval[7] == NULL) 
      return (SIZE_NOT_FOUND);
    rval[8] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_IS_DELETED], dcs_aname[DATA_IS_DELETED]);
    if (rval[8] == NULL) 
      return (DATA_IS_DELETED_NOT_FOUND);
    rval[9] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_OWNER], dcs_aname[DATA_OWNER]);
    if (rval[9] == NULL) 
      return (DATA_OWNER_NOT_FOUND);
    rval[10] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_OWNER_DOMAIN], dcs_aname[DATA_OWNER_DOMAIN]);
    if (rval[10] == NULL) 
      return (DATA_OWNER_DOMAIN_NOT_FOUND);
    rval[11] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_OWNER_ZONE], dcs_aname[DATA_OWNER_ZONE]);
    if (rval[11] == NULL) 
      return (DATA_OWNER_ZONE_NOT_FOUND);
    rval[12] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[IS_DIRTY], dcs_aname[IS_DIRTY]);
    if (rval[12] == NULL) 
      return (IS_DIRTY_NOT_FOUND);
    rval[13] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[OFFSET], dcs_aname[OFFSET]);
    if (rval[13] == NULL) 
      return (OFFSET_NOT_FOUND);
    rval[14] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_CONT_NAME], dcs_aname[DATA_CONT_NAME]);
    if (rval[14] == NULL) 
      return (DATA_CONT_NAME_NOT_FOUND);
    rval[15] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[COLLECTION_CONT_NAME], dcs_aname[COLLECTION_CONT_NAME]);
    if (rval[15] == NULL) 
      return (COLLECTION_CONT_NAME_NOT_FOUND);
    rval[16] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_CREATE_TIMESTAMP],dcs_aname[DATA_CREATE_TIMESTAMP]);
    if (rval[16] == NULL) 
      return (DATA_CREATE_TIMESTAMP_NOT_FOUND);
    rval[17] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_LAST_ACCESS_TIMESTAMP], dcs_aname[DATA_LAST_ACCESS_TIMESTAMP]);
    if (rval[17] == NULL) 
      return (DATA_LAST_ACCESS_TIMESTAMP_NOT_FOUND);
    rval[18] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_LOCK_NUM], dcs_aname[DATA_LOCK_NUM]);
    if (rval[18] == NULL) 
      return (DATA_LOCK_NUM_NOT_FOUND);
    rval[19] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_LOCK_OWNER_NAME], dcs_aname[DATA_LOCK_OWNER_NAME]);
    if (rval[19] == NULL) 
      return (DATA_LOCK_OWNER_NAME_NOT_FOUND);
    rval[20] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_LOCK_OWNER_DOMAIN], dcs_aname[DATA_LOCK_OWNER_DOMAIN]);
    if (rval[20] == NULL) 
      return (DATA_LOCK_OWNER_DOMAIN_NOT_FOUND);
    rval[21] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_LOCK_OWNER_ZONE], dcs_aname[DATA_LOCK_OWNER_ZONE]);
    if (rval[21] == NULL) 
      return (DATA_LOCK_OWNER_ZONE_NOT_FOUND);
    rval[22] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_VER_NUM], dcs_aname[DATA_VER_NUM]);
    if (rval[22] == NULL) 
      return (DATA_VER_NUM_NOT_FOUND);
    rval[23] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[CONTAINER_NAME], dcs_aname[CONTAINER_NAME]);
    if (rval[23] == NULL) 
      return (CONTAINER_NAME_NOT_FOUND);
    rval[24] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_COMMENTS], dcs_aname[DATA_COMMENTS]);
    if (rval[24] == NULL) 
      return (DATA_COMMENTS_NOT_FOUND);
    rval[25] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_LOCK_EXPIRY], dcs_aname[DATA_LOCK_EXPIRY]);
    if (rval[25] == NULL) 
      return (DATA_LOCK_EXPIRY_NOT_FOUND);
    rval[26] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_CHECKSUM], dcs_aname[DATA_CHECKSUM]);
    if (rval[26] == NULL) 
      return (DATA_CHECKSUM_NOT_FOUND);
    rval[27] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_HIDE], dcs_aname[DATA_HIDE]);
    if (rval[27] == NULL) 
      return (DATA_HIDE_NOT_FOUND);
    rval[28] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_AUDITFLAG], dcs_aname[DATA_AUDITFLAG]);
    if (rval[28] == NULL) 
      return (DATA_AUDITFLAG_NOT_FOUND);
    rval[29] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_SEG_NUM], dcs_aname[DATA_SEG_NUM]);
    if (rval[29] == NULL) 
      return (DATA_SEG_NUM_NOT_FOUND);

    for (i=0; i < myresult->row_count; i++) {
      zoneChangeCollName(rval[6]);
      zoneChangeCollName(rval[15]);
      ii = ibmDataCore(cat_type,rval,userName,domainName);
      if (ii == MDAS_SUCCESS)
        cnt++;
      else if (ii != DATA_NOT_UNIQUE_IN_COLLECTION) {
        elog(NOTICE, "ingest_bulk_metadata error for action %s: %i\n",actionInfo, ii);
        elog(NOTICE, "Bad Row No: %i Row=%s|%s||||%s\n",i+1,rval[0],rval[1],rval[6]);
        return(ii);
      }
      rval[0] += MAX_DATA_SIZE;
      rval[1] += MAX_DATA_SIZE;
      rval[2] += MAX_DATA_SIZE;
      rval[3] += MAX_DATA_SIZE;
      rval[4] += MAX_DATA_SIZE;
      rval[5] += MAX_DATA_SIZE;
      rval[6] += MAX_DATA_SIZE;
      rval[7] += MAX_DATA_SIZE;
      rval[8] += MAX_DATA_SIZE;
      rval[9] += MAX_DATA_SIZE;
      rval[10] += MAX_DATA_SIZE;
      rval[11] += MAX_DATA_SIZE;
      rval[12] += MAX_DATA_SIZE;
      rval[13] += MAX_DATA_SIZE;
      rval[14] += MAX_DATA_SIZE;
      rval[15] += MAX_DATA_SIZE;
      rval[16] += MAX_DATA_SIZE;
      rval[17] += MAX_DATA_SIZE;
      rval[18] += MAX_DATA_SIZE;
      rval[19] += MAX_DATA_SIZE;
      rval[20] += MAX_DATA_SIZE;
      rval[21] += MAX_DATA_SIZE;
      rval[22] += MAX_DATA_SIZE;
      rval[23] += MAX_DATA_SIZE;
      rval[24] += MAX_DATA_SIZE;
      rval[25] += MAX_DATA_SIZE;
      rval[26] += MAX_DATA_SIZE;
      rval[27] += MAX_DATA_SIZE;
      rval[28] += MAX_DATA_SIZE;
      rval[29] += MAX_DATA_SIZE;
    }
    return(cnt);
  }
  else if (strstr(actionInfo,"PUT_CONTAINER_INFO") == actionInfo) {
    rval[0] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[CONTAINER_NAME], dcs_aname[CONTAINER_NAME]);
    if (rval[0] == NULL) 
      return (CONTAINER_NAME_NOT_FOUND);
    rval[1] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[CONTAINER_MAX_SIZE], dcs_aname[CONTAINER_MAX_SIZE]);
    if (rval[1] == NULL) 
      return (CONTAINER_MAX_SIZE_NOT_FOUND);
    rval[2] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[CONTAINER_LOG_RSRC_NAME], dcs_aname[CONTAINER_LOG_RSRC_NAME]);
    if (rval[2] == NULL) 
      return (CONTAINER_LOG_RSRC_NAME_NOT_FOUND);

    for (i=0; i < myresult->row_count; i++) {
      zoneChangeCollName(rval[0]);
      ii = ibmContainerCore(cat_type,rval,userName,domainName);
      if (ii == MDAS_SUCCESS)
        cnt++;
      else if (ii != ITEM_ALREADY_IN_CAT) {
        elog(NOTICE, "ingest_bulk_metadata error for action %s: %i\n",actionInfo, ii);
        elog(NOTICE, "Bad Row No: %i Row=%s\n",i+1,rval[0]);
        return(ii);
      }
      rval[0] += MAX_DATA_SIZE;
      rval[1] += MAX_DATA_SIZE;
      rval[2] += MAX_DATA_SIZE;
    }
    return(cnt);

   
  }
  else if (strstr(actionInfo,"PUT_CHANGED_DATA_ACCESS")==actionInfo){
    rval[0] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_CONT_NAME], dcs_aname[DATA_CONT_NAME]);
    if (rval[0] == NULL) 
      return (DATA_CONT_NAME_NOT_FOUND);
    rval[1] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_REPL_ENUM], dcs_aname[DATA_REPL_ENUM]);
    if (rval[1] == NULL) 
      return (DATA_REPL_ENUM_NOT_FOUND);
    rval[2] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[COLLECTION_CONT_NAME], dcs_aname[COLLECTION_CONT_NAME]);
    if (rval[2] == NULL) 
      return (COLLECTION_NAME_NOT_FOUND);
    rval[3] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_ACCS_GRPUSER_NAME], dcs_aname[DATA_ACCS_GRPUSER_NAME]);
    if (rval[3] == NULL) 
      return (DATA_ACCS_GRPUSER_NAME_NOT_FOUND);
    rval[4] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_ACCS_GRPUSER_DOMAIN], dcs_aname[DATA_ACCS_GRPUSER_DOMAIN]);
    if (rval[4] == NULL) 
      return (DATA_ACCS_GRPUSER_DOMAIN_NOT_FOUND);
    rval[5] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_ACCS_GRPUSER_ZONE], dcs_aname[DATA_ACCS_GRPUSER_ZONE]);
    if (rval[5] == NULL) 
      return (DATA_ACCS_GRPUSER_ZONE_NOT_FOUND);
    rval[6] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[ACCESS_CONSTRAINT], dcs_aname[ACCESS_CONSTRAINT]);
    if (rval[6] == NULL) 
      return (ACCESS_CONSTRAINT_NOT_FOUND);

    for (i=0; i < myresult->row_count; i++) {
      zoneChangeCollName(rval[2]);
      sprintf(sval[0],"%s&COPY=%i",rval[0],atoi(rval[1]));
      sprintf(sval[1],"%s@%s|%s",rval[3],rval[4],rval[5]);

      ii = modify_dataset_info(cat_type,sval[0], rval[2],"","",
			       sval[1],rval[6],
			       D_SU_INSERT_ACCS,userName,domainName);
      if (ii == MDAS_SUCCESS)
        cnt++;
      else {
        elog(NOTICE, "ingest_bulk_metadata error for action %s: %i\n",actionInfo, ii);
        elog(NOTICE, "Bad Row No: %i Row=%s|%s|%s\n",i+1,rval[0],rval[1],rval[2]);
        return(ii);
      }
      rval[0] += MAX_DATA_SIZE;
      rval[1] += MAX_DATA_SIZE;
      rval[2] += MAX_DATA_SIZE;
      rval[3] += MAX_DATA_SIZE;
      rval[4] += MAX_DATA_SIZE;
      rval[5] += MAX_DATA_SIZE;
      rval[6] += MAX_DATA_SIZE;
    }
    return(cnt);

    
  }
  else if (strstr(actionInfo,"PUT_CHANGED_COLL_ACCESS")==actionInfo){
    rval[0] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[COLLECTION_NAME], dcs_aname[COLLECTION_NAME]);
    if (rval[0] == NULL) 
      return (COLLECTION_NAME_NOT_FOUND);
    rval[1] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[COLL_ACCS_GRPUSER_NAME], dcs_aname[COLL_ACCS_GRPUSER_NAME]);
    if (rval[1] == NULL) 
      return (COLL_ACCS_GRPUSER_NAME_NOT_FOUND);
    rval[2] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[COLL_ACCS_GRPUSER_DOMAIN], dcs_aname[COLL_ACCS_GRPUSER_DOMAIN]);
    if (rval[2] == NULL) 
      return (COLL_ACCS_GRPUSER_DOMAIN_NOT_FOUND);
    rval[3] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[COLL_ACCS_GRPUSER_ZONE], dcs_aname[COLL_ACCS_GRPUSER_ZONE]);
    if (rval[3] == NULL) 
      return (COLL_ACCS_GRPUSER_ZONE_NOT_FOUND);
    rval[4] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[COLL_ACCS_ACCESS_CONSTRAINT], dcs_aname[COLL_ACCS_ACCESS_CONSTRAINT]);
    if (rval[4] == NULL) 
      return (COLL_ACCS_ACCESS_CONSTRAINT_NOT_FOUND);

    for (i=0; i < myresult->row_count; i++) {
      zoneChangeCollName(rval[0]);
      sprintf(sval[1],"%s|%s",rval[2],rval[3]);
      ii = modify_collection_info(cat_type,userName,  rval[0],
				  rval[1],sval[1],rval[4],
			       C_SU_INSERT_COLL_ACCS,domainName);
      if (ii == MDAS_SUCCESS)
        cnt++;
      else {
        elog(NOTICE, "ingest_bulk_metadata error for action %s: %i\n",actionInfo, ii);
        elog(NOTICE, "Bad Row No: %i Row=%s\n",i+1,rval[0]);
        return(ii);
      }
      rval[0] += MAX_DATA_SIZE;
      rval[1] += MAX_DATA_SIZE;
      rval[2] += MAX_DATA_SIZE;
      rval[3] += MAX_DATA_SIZE;
      rval[4] += MAX_DATA_SIZE;
    }
    return(cnt);

    
  }
  else if (strstr(actionInfo,"PUT_CHANGED_COLLCONT_INFO")==actionInfo){
    rval[0] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[COLLECTION_NAME], dcs_aname[COLLECTION_NAME]);
    if (rval[0] == NULL) 
      return (COLLECTION_NAME_NOT_FOUND);
    rval[1] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[CONTAINER_FOR_COLLECTION], dcs_aname[CONTAINER_FOR_COLLECTION]);
    if (rval[1] == NULL) 
      return (CONTAINER_FOR_COLLECTION_NOT_FOUND);
    rval[2] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_TYPE_FOR_CONTAINER_FOR_COLLECTION], dcs_aname[DATA_TYPE_FOR_CONTAINER_FOR_COLLECTION]);
    if (rval[2] == NULL) 
      return (DATA_TYPE_FOR_CONTAINER_FOR_COLLECTION_NOT_FOUND);

    for (i=0; i < myresult->row_count; i++) {
      zoneChangeCollName(rval[0]);
      zoneChangeCollName(rval[1]);
      ii = ibmContColl(cat_type,rval,userName,domainName);
      if (ii == MDAS_SUCCESS)
        cnt++;
      else if (ii != ITEM_ALREADY_IN_CAT) {
        elog(NOTICE, "ingest_bulk_metadata error for action %s: %i\n",actionInfo, ii);
        elog(NOTICE, "Bad Row No: %i Row=%s|%s|%s\n",i+1,rval[0],rval[1],rval[2]);
        return(ii);
      }
      rval[0] += MAX_DATA_SIZE;
      rval[1] += MAX_DATA_SIZE;
      rval[2] += MAX_DATA_SIZE;
    }
    return(cnt);

    
  }
  else if (strstr(actionInfo,"PUT_CHANGED_DATA_UDEFMETA_INFO")==actionInfo){

    rval[0] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
    if (rval[0] == NULL) 
      return (DATA_NAME_NOT_FOUND);
    rval[1] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[COLLECTION_NAME], dcs_aname[COLLECTION_NAME]);
    if (rval[1] == NULL) 
      return (COLLECTION_NAME_NOT_FOUND);
    rval[3] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD0], dcs_aname[UDSMD0]);
    if (rval[3] == NULL) 
      return (UDSMD0_NOT_FOUND);
    rval[4] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD1], dcs_aname[UDSMD1]);
    if (rval[4] == NULL) 
      return (UDSMD1_NOT_FOUND);
    rval[5] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD2], dcs_aname[UDSMD2]);
    if (rval[5] == NULL) 
      return (UDSMD2_NOT_FOUND);
    rval[6] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD3], dcs_aname[UDSMD3]);
    if (rval[6] == NULL) 
      return (UDSMD3_NOT_FOUND);
    rval[7] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD4], dcs_aname[UDSMD4]);
    if (rval[7] == NULL) 
      return (UDSMD4_NOT_FOUND);
    rval[8] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD5], dcs_aname[UDSMD5]);
    if (rval[8] == NULL) 
      return (UDSMD5_NOT_FOUND);
    rval[9] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD6], dcs_aname[UDSMD6]);
    if (rval[9] == NULL) 
      return (UDSMD6_NOT_FOUND);
    rval[10] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD7], dcs_aname[UDSMD7]);
    if (rval[10] == NULL) 
      return (UDSMD7_NOT_FOUND);
    rval[11] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD8], dcs_aname[UDSMD8]);
    if (rval[11] == NULL) 
      return (UDSMD8_NOT_FOUND);
    rval[12] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD9], dcs_aname[UDSMD9]);
    if (rval[12] == NULL) 
      return (UDSMD9_NOT_FOUND);
    rval[13] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDIMD0], dcs_aname[UDIMD0]);
    if (rval[13] == NULL) 
      return (UDIMD0_NOT_FOUND);
    rval[14] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDIMD1], dcs_aname[UDIMD1]);
    if (rval[14] == NULL) 
      return (UDIMD1_NOT_FOUND);
    rval[15] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_UDEF_MDATA_MODIFY_DATE], dcs_aname[DATA_UDEF_MDATA_MODIFY_DATE]);
    if (rval[15] == NULL) 
      return (DATA_UDEF_MDATA_MODIFY_DATE_NOT_FOUND);
    rval[16] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_UDEF_MDATA_CREATE_DATE], dcs_aname[DATA_UDEF_MDATA_CREATE_DATE]);
    if (rval[16] == NULL) 
      return (DATA_UDEF_MDATA_CREATE_DATE_NOT_FOUND);
    rval[17] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[METADATA_NUM], dcs_aname[METADATA_NUM]);
    if (rval[17] == NULL) 
      return (METADATA_NUM_NOT_FOUND);
    for (i=0; i < myresult->row_count; i++) {
      zoneChangeCollName(rval[1]);
      sprintf(sval[0],"%s/%s",rval[1],rval[0]);
      ii = insertUserDefMetadataTable("Data",sval[0],rval, 3,17);
      if (ii == MDAS_SUCCESS)
        cnt++;
      else if (ii != METADATA_ALREADY_IN_CAT) {
        elog(NOTICE, "ingest_bulk_metadata error for action %s: %i\n",actionInfo, ii);
        elog(NOTICE, "Bad Row No: %i Row=%s\n",i+1,rval[0]);
        return(ii);
      }

      rval[0] += MAX_DATA_SIZE;
      rval[1] += MAX_DATA_SIZE;
      rval[2] += MAX_DATA_SIZE;
      rval[3] += MAX_DATA_SIZE;
      rval[4] += MAX_DATA_SIZE;
      rval[5] += MAX_DATA_SIZE;
      rval[6] += MAX_DATA_SIZE;
      rval[7] += MAX_DATA_SIZE;
      rval[8] += MAX_DATA_SIZE;
      rval[9] += MAX_DATA_SIZE;
      rval[10] += MAX_DATA_SIZE;
      rval[11] += MAX_DATA_SIZE;
      rval[12] += MAX_DATA_SIZE;
      rval[13] += MAX_DATA_SIZE;
      rval[14] += MAX_DATA_SIZE;
      rval[15] += MAX_DATA_SIZE;
      rval[16] += MAX_DATA_SIZE;
      rval[17] += MAX_DATA_SIZE;
    }
    return(cnt);
  }
  else if (strstr(actionInfo,"PUT_CHANGED_COLL_UDEFMETA_INFO")==actionInfo){

    rval[0] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[COLLECTION_NAME], dcs_aname[COLLECTION_NAME]);
    if (rval[0] == NULL) 
      return (COLLECTION_NAME_NOT_FOUND);
    rval[3] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD_COLL0], dcs_aname[UDSMD_COLL0]);
    if (rval[3] == NULL) 
      return (UDSMD_COLL0_NOT_FOUND);
    rval[4] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD_COLL1], dcs_aname[UDSMD_COLL1]);
    if (rval[4] == NULL) 
      return (UDSMD_COLL1_NOT_FOUND);
    rval[5] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD_COLL2], dcs_aname[UDSMD_COLL2]);
    if (rval[5] == NULL) 
      return (UDSMD_COLL2_NOT_FOUND);
    rval[6] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD_COLL3], dcs_aname[UDSMD_COLL3]);
    if (rval[6] == NULL) 
      return (UDSMD_COLL3_NOT_FOUND);
    rval[7] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD_COLL4], dcs_aname[UDSMD_COLL4]);
    if (rval[7] == NULL) 
      return (UDSMD_COLL4_NOT_FOUND);
    rval[8] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD_COLL5], dcs_aname[UDSMD_COLL5]);
    if (rval[8] == NULL) 
      return (UDSMD_COLL5_NOT_FOUND);
    rval[9] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD_COLL6], dcs_aname[UDSMD_COLL6]);
    if (rval[9] == NULL) 
      return (UDSMD_COLL6_NOT_FOUND);
    rval[10] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD_COLL7], dcs_aname[UDSMD_COLL7]);
    if (rval[10] == NULL) 
      return (UDSMD_COLL7_NOT_FOUND);
    rval[11] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD_COLL8], dcs_aname[UDSMD_COLL8]);
    if (rval[11] == NULL) 
      return (UDSMD_COLL8_NOT_FOUND);
    rval[12] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDSMD_COLL9], dcs_aname[UDSMD_COLL9]);
    if (rval[12] == NULL) 
      return (UDSMD_COLL9_NOT_FOUND);
    rval[13] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDIMD_COLL0], dcs_aname[UDIMD_COLL0]);
    if (rval[13] == NULL) 
      return (UDIMD_COLL0_NOT_FOUND);
    rval[14] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[UDIMD_COLL1], dcs_aname[UDIMD_COLL1]);
    if (rval[14] == NULL) 
      return (UDIMD_COLL1_NOT_FOUND);
    rval[15] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[COLL_UDEF_MDATA_MODIFY_DATE], dcs_aname[COLL_UDEF_MDATA_MODIFY_DATE]);
    if (rval[15] == NULL) 
      return (COLL_UDEF_MDATA_MODIFY_DATE_NOT_FOUND);
    rval[16] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[COLL_UDEF_MDATA_CREATE_DATE], dcs_aname[COLL_UDEF_MDATA_CREATE_DATE]);
    if (rval[16] == NULL) 
      return (COLL_UDEF_MDATA_CREATE_DATE_NOT_FOUND);
    rval[17] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[METADATA_NUM_COLL], dcs_aname[METADATA_NUM_COLL]);
    if (rval[17] == NULL) 
      return (METADATA_NUM_COLL_NOT_FOUND);
    for (i=0; i < myresult->row_count; i++) {
      zoneChangeCollName(rval[0]);
      ii = insertUserDefMetadataTable("Collection",rval[0],rval, 3,17);
      if (ii == MDAS_SUCCESS)
        cnt++;
      else if (ii != METADATA_ALREADY_IN_CAT) {
        elog(NOTICE, "ingest_bulk_metadata error for action %s: %i\n",actionInfo, ii);
        elog(NOTICE, "Bad Row No: %i Row=%s\n",i+1,rval[0]);
        return(ii);
      }

      rval[0] += MAX_DATA_SIZE;
      rval[1] += MAX_DATA_SIZE;
      rval[2] += MAX_DATA_SIZE;
      rval[3] += MAX_DATA_SIZE;
      rval[4] += MAX_DATA_SIZE;
      rval[5] += MAX_DATA_SIZE;
      rval[6] += MAX_DATA_SIZE;
      rval[7] += MAX_DATA_SIZE;
      rval[8] += MAX_DATA_SIZE;
      rval[9] += MAX_DATA_SIZE;
      rval[10] += MAX_DATA_SIZE;
      rval[11] += MAX_DATA_SIZE;
      rval[12] += MAX_DATA_SIZE;
      rval[13] += MAX_DATA_SIZE;
      rval[14] += MAX_DATA_SIZE;
      rval[15] += MAX_DATA_SIZE;
      rval[16] += MAX_DATA_SIZE;
      rval[17] += MAX_DATA_SIZE;
    }
    return(cnt);
  }
  else if (strstr(actionInfo,"PUT_CHANGED_DATA_ANNOTATION_INFO")==actionInfo){
    rval[0] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
    if (rval[0] == NULL) 
      return (DATA_NAME_NOT_FOUND);
    rval[1] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[COLLECTION_NAME], dcs_aname[COLLECTION_NAME]);
    if (rval[1] == NULL) 
      return (COLLECTION_NAME_NOT_FOUND);
    rval[2] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_ANNOTATION_USERNAME], dcs_aname[DATA_ANNOTATION_USERNAME]);
    if (rval[2] == NULL) 
      return (DATA_ANNOTATION_USERNAME_NOT_FOUND);
    rval[3] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_ANNOTATION_USERDOMAIN], dcs_aname[DATA_ANNOTATION_USERDOMAIN]);
    if (rval[3] == NULL) 
      return (DATA_ANNOTATION_USERDOMAIN_NOT_FOUND);
    rval[4] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_ANNOTATION_USER_ZONE], dcs_aname[DATA_ANNOTATION_USER_ZONE]);
    if (rval[4] == NULL) 
      return (DATA_ANNOTATION_USER_ZONE_NOT_FOUND);
    rval[5] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_ANNOTATION], dcs_aname[DATA_ANNOTATION]);
    if (rval[5] == NULL) 
      return (DATA_ANNOTATION_NOT_FOUND);
    rval[6] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_ANNOTATION_TIMESTAMP], dcs_aname[DATA_ANNOTATION_TIMESTAMP]);
    if (rval[6] == NULL) 
      return (DATA_ANNOTATION_TIMESTAMP_NOT_FOUND);
    rval[7] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_ANNOTATION_POSITION], dcs_aname[DATA_ANNOTATION_POSITION]);
    if (rval[7] == NULL) 
      return (DATA_ANNOTATION_POSITION_NOT_FOUND);
    rval[8] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[CONTAINER_REPL_ENUM], dcs_aname[CONTAINER_REPL_ENUM]);
    if (rval[8] == NULL) 
      return (CONTAINER_REPL_ENUM_NOT_FOUND);

    for (i=0; i < myresult->row_count; i++) {
      zoneChangeCollName(rval[1]);
      ii = ibmAnnotation(cat_type,"Data",rval,userName,domainName);
      if (ii == MDAS_SUCCESS)
        cnt++;
      else if (ii != ITEM_ALREADY_IN_CAT) {
        elog(NOTICE, "ingest_bulk_metadata error for action %s: %i\n",actionInfo, ii);
        elog(NOTICE, "Bad Row No: %i Row=%s\n",i+1,rval[0]);
        return(ii);
      }
      rval[0] += MAX_DATA_SIZE;
      rval[1] += MAX_DATA_SIZE;
      rval[2] += MAX_DATA_SIZE;
      rval[3] += MAX_DATA_SIZE;
      rval[4] += MAX_DATA_SIZE;
      rval[5] += MAX_DATA_SIZE;
      rval[6] += MAX_DATA_SIZE;
      rval[7] += MAX_DATA_SIZE;
      rval[8] += MAX_DATA_SIZE;
    }
    return(cnt);

    
  }
  else if (strstr(actionInfo,"PUT_CHANGED_COLL_ANNOTATION_INFO")==actionInfo){
    rval[0] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[COLLECTION_NAME], dcs_aname[COLLECTION_NAME]);
    if (rval[0] == NULL) 
      return (COLLECTION_NAME_NOT_FOUND);
    rval[1] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[COLL_ANNOTATION_USERNAME], dcs_aname[COLL_ANNOTATION_USERNAME]);
    if (rval[1] == NULL) 
      return (COLL_ANNOTATION_USERNAME_NOT_FOUND);
    rval[2] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[COLL_ANNOTATION_USERDOMAIN], dcs_aname[COLL_ANNOTATION_USERDOMAIN]);
    if (rval[2] == NULL) 
      return (COLL_ANNOTATION_USERDOMAIN_NOT_FOUND);
    rval[3] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[COLL_ANNOTATION_USER_ZONE], dcs_aname[COLL_ANNOTATION_USER_ZONE]);
    if (rval[3] == NULL) 
      return (COLL_ANNOTATION_USER_ZONE_NOT_FOUND);
    rval[4] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[COLL_ANNOTATION], dcs_aname[COLL_ANNOTATION]);
    if (rval[4] == NULL) 
      return (COLL_ANNOTATION_NOT_FOUND);
    rval[5] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[COLL_ANNOTATION_TIMESTAMP], dcs_aname[COLL_ANNOTATION_TIMESTAMP]);
    if (rval[5] == NULL) 
      return (COLL_ANNOTATION_TIMESTAMP_NOT_FOUND);
    rval[6] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[COLL_ANNOTATION_TYPE], dcs_aname[COLL_ANNOTATION_TYPE]);
    if (rval[6] == NULL) 
      return (COLL_ANNOTATION_TYPE_NOT_FOUND);

    for (i=0; i < myresult->row_count; i++) {
      zoneChangeCollName(rval[0]);
      ii = ibmAnnotation(cat_type,"Collection",rval,userName,domainName);
      if (ii == MDAS_SUCCESS)
        cnt++;
      else if (ii != ITEM_ALREADY_IN_CAT) {
        elog(NOTICE, "ingest_bulk_metadata error for action %s: %i\n",actionInfo, ii);
        elog(NOTICE, "Bad Row No: %i Row=%s\n",i+1,rval[0]);
        return(ii);
      }
      rval[0] += MAX_DATA_SIZE;
      rval[1] += MAX_DATA_SIZE;
      rval[2] += MAX_DATA_SIZE;
      rval[3] += MAX_DATA_SIZE;
      rval[4] += MAX_DATA_SIZE;
      rval[5] += MAX_DATA_SIZE;
      rval[6] += MAX_DATA_SIZE;
    }
    return(cnt);

    
  }
  else if (strstr(actionInfo,"PUT_CHANGED_OTHERDATA_INFO")==actionInfo){
    rval[0] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
    if (rval[0] == NULL) 
      return (DATA_NAME_NOT_FOUND);
    rval[1] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[COLLECTION_NAME], dcs_aname[COLLECTION_NAME]);
    if (rval[1] == NULL) 
      return (COLLECTION_NAME_NOT_FOUND);
    rval[2] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[CONTAINER_REPL_ENUM], dcs_aname[CONTAINER_REPL_ENUM]);
    if (rval[2] == NULL) 
      return (CONTAINER_REPL_ENUM_NOT_FOUND);
    rval[3] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_PIN_VAL], dcs_aname[DATA_PIN_VAL]);
    if (rval[3] == NULL) 
      return (DATA_PIN_VAL_NOT_FOUND);
    rval[4] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_PIN_OWNER_NAME], dcs_aname[DATA_PIN_OWNER_NAME]);
    if (rval[4] == NULL) 
      return (DATA_PIN_OWNER_NAME_NOT_FOUND);
    rval[5] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_PIN_OWNER_DOMAIN], dcs_aname[DATA_PIN_OWNER_DOMAIN]);
    if (rval[5] == NULL) 
      return (DATA_PIN_OWNER_DOMAIN_NOT_FOUND);
    rval[6] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_PIN_OWNER_ZONE], dcs_aname[DATA_PIN_OWNER_ZONE]);
    if (rval[6] == NULL) 
      return (DATA_PIN_OWNER_ZONE_NOT_FOUND);
    rval[7] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_PIN_EXPIRY], dcs_aname[DATA_PIN_EXPIRY]);
    if (rval[7] == NULL) 
      return (DATA_PIN_EXPIRY_NOT_FOUND);
    rval[8] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_EXPIRY_DATE], dcs_aname[DATA_EXPIRY_DATE]);
    if (rval[8] == NULL) 
      return (DATA_EXPIRY_DATE_NOT_FOUND);
    rval[9] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_IS_COMPRESSED], dcs_aname[DATA_IS_COMPRESSED]);
    if (rval[9] == NULL) 
      return (DATA_IS_COMPRESSED_NOT_FOUND);
    rval[10] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_IS_ENCRYPTED], dcs_aname[DATA_IS_ENCRYPTED]);
    if (rval[10] == NULL) 
      return (DATA_IS_ENCRYPTED_NOT_FOUND);
    rval[11] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_EXPIRE_DATE_2], dcs_aname[DATA_EXPIRE_DATE_2]);
    if (rval[11] == NULL) 
      return (DATA_EXPIRE_DATE_2_NOT_FOUND);    
    for (i=0; i < myresult->row_count; i++) {
      zoneChangeCollName(rval[1]);
      ii = ibmDataMisc1(cat_type,rval,userName,domainName);
      if (ii == MDAS_SUCCESS)
        cnt++;
      else if (ii != ITEM_ALREADY_IN_CAT) {
        elog(NOTICE, "ingest_bulk_metadata error for action %s: %i\n",actionInfo, ii);
        elog(NOTICE, "Bad Row No: %i Row=%s\n",i+1,rval[0]);
        return(ii);
      }
      rval[0] += MAX_DATA_SIZE;
      rval[1] += MAX_DATA_SIZE;
      rval[2] += MAX_DATA_SIZE;
      rval[3] += MAX_DATA_SIZE;
      rval[4] += MAX_DATA_SIZE;
      rval[5] += MAX_DATA_SIZE;
      rval[6] += MAX_DATA_SIZE;
      rval[7] += MAX_DATA_SIZE;
      rval[8] += MAX_DATA_SIZE;
      rval[9] += MAX_DATA_SIZE;
      rval[10] += MAX_DATA_SIZE;
      rval[11] += MAX_DATA_SIZE;
    }
    return(cnt);

    
  }
  else if (strstr(actionInfo,"PUT_CHANGED_DATA_GUID_INFO")==actionInfo){
    rval[0] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
    if (rval[0] == NULL) 
      return (DATA_NAME_NOT_FOUND);
    rval[1] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[COLLECTION_NAME], dcs_aname[COLLECTION_NAME]);
    if (rval[1] == NULL) 
      return (COLLECTION_NAME_NOT_FOUND);
    rval[2] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[GUID], dcs_aname[GUID]);
    if (rval[2] == NULL) 
      return (GUID_NOT_FOUND);
    rval[3] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[GUID_FLAG], dcs_aname[GUID_FLAG]);
    if (rval[3] == NULL) 
      return (GUID_FLAG_NOT_FOUND);
    rval[4] = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[GUID_TIME_STAMP], dcs_aname[GUID_TIME_STAMP]);
    if (rval[4] == NULL) 
      return (GUID_TIME_STAMP_NOT_FOUND);
    for (i=0; i < myresult->row_count; i++) {
      zoneChangeCollName(rval[1]);
      ii = ibmDataGuid(cat_type,rval,userName,domainName);
      if (ii == MDAS_SUCCESS)
        cnt++;
      else if (ii != ITEM_ALREADY_IN_CAT) {
        elog(NOTICE, "ingest_bulk_metadata error for action %s: %i\n",actionInfo, ii);
        elog(NOTICE, "Bad Row No: %i Row=%s %s %s\n",i+1,rval[0],rval[1],rval[2]);
        return(ii);
      }
      rval[0] += MAX_DATA_SIZE;
      rval[1] += MAX_DATA_SIZE;
      rval[2] += MAX_DATA_SIZE;
      rval[3] += MAX_DATA_SIZE;
      rval[4] += MAX_DATA_SIZE;
    }
    return(cnt);
  }
  else {
    return(FUNCTION_NOT_SUPPORTED);
  }


}
	
int
insertUserDefMetadataTable(char *ObjectType, char *inObjectInfo, 
			     char *rval[], int startInx, int endInx)

{

  int i,j,ii;
  char objInfo[MAX_TOKEN + 3];
  char objInfo2[MAX_TOKEN];
  char *domainZoneName;
  int objId;
  char *metadataString;
  if (!strcmp(ObjectType,"User")) {
    strcpy(objInfo,inObjectInfo);
    if ((domainZoneName = strstr(objInfo,"@")) == NULL)
      return (DOMAIN_NOT_IN_CAT);
    *domainZoneName = '\0';
    objId = get_user_id_in_domain(objInfo, domainZoneName+1);
    j = 0;
    for (i = startInx  ; i <=endInx; i++)
      j += strlen(rval[i]);
    metadataString = malloc (j+3000);
    i = startInx;
    if (strlen(rval[i+10]) == 0)
      strcpy(rval[i+10],"0");
    if (strlen(rval[i+11]) == 0)
      strcpy(rval[i+11],"0");
    sprintf(metadataString,"select METADATANUM from %sMDAS_AU_MDATA where user_id = %i and (USERDEF_METASTR0 = '%s' OR USERDEF_METASTR0 is NULL) and (USERDEF_METASTR1 = '%s' OR USERDEF_METASTR1 is NULL)  and (USERDEF_METASTR2 = '%s' OR USERDEF_METASTR2 is NULL)  and (USERDEF_METASTR3 = '%s' OR USERDEF_METASTR3 is NULL) and (USERDEF_METASTR4 = '%s'  OR USERDEF_METASTR4 is NULL) and (USERDEF_METASTR5 = '%s'  OR USERDEF_METASTR5 is NULL) and (USERDEF_METASTR6 = '%s' OR USERDEF_METASTR6 is NULL)  and (USERDEF_METASTR7 = '%s'  OR USERDEF_METASTR7 is NULL) and (USERDEF_METASTR8 = '%s'  OR USERDEF_METASTR8 is NULL) and (USERDEF_METASTR9 = '%s'  OR USERDEF_METASTR9 is NULL) and (USERDEF_METAINT0 = %i  OR USERDEF_METAINT0 is NULL) and (USERDEF_METAINT1 = %i OR USERDEF_METAINT1 is NULL)", 
	    MDASSCHEMENAME, objId, rval[i], rval[i+1], rval[i+2], rval[i+3], rval[i+4], rval[i+5], rval[i+6], rval[i+7], rval[i+8], rval[i+9], atoi(rval[i+10]), atoi(rval[i+11]));
    if (check_exists(metadataString) == MDAS_SUCCESS)
	return (METADATA_ALREADY_IN_CAT);
    sprintf(metadataString,"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",rval[i], rval[i+1], rval[i+2], rval[i+3], rval[i+4], rval[i+5], rval[i+6], rval[i+7], rval[i+8], rval[i+9], rval[i+10], rval[i+11]);
    ii = bulkInsertUdefMetadataForUser(objId, metadataString, "|", rval[15]);
    free(metadataString);
    return(ii);
  }
  else if(!strcmp(ObjectType,"Resource")) {
    objId = get_itype_value("MDAS_CD_RSRC.rsrc_name", inObjectInfo);
    j = 0;
    for (i = startInx  ; i <=endInx; i++)
      j += strlen(rval[i]);
    metadataString = malloc (j+3000);
    i = startInx;
    if (strlen(rval[i+10]) == 0)
      strcpy(rval[i+10],"0");
    if (strlen(rval[i+11]) == 0)
      strcpy(rval[i+11],"0");
    sprintf(metadataString,"select METADATANUM from %sMDAS_AR_MDATA where rsrc_id = %i and (USERDEF_METASTR0 = '%s' OR USERDEF_METASTR0 is NULL) and (USERDEF_METASTR1 = '%s' OR USERDEF_METASTR1 is NULL)  and (USERDEF_METASTR2 = '%s' OR USERDEF_METASTR2 is NULL)  and (USERDEF_METASTR3 = '%s' OR USERDEF_METASTR3 is NULL) and (USERDEF_METASTR4 = '%s'  OR USERDEF_METASTR4 is NULL) and (USERDEF_METASTR5 = '%s'  OR USERDEF_METASTR5 is NULL) and (USERDEF_METASTR6 = '%s' OR USERDEF_METASTR6 is NULL)  and (USERDEF_METASTR7 = '%s'  OR USERDEF_METASTR7 is NULL) and (USERDEF_METASTR8 = '%s'  OR USERDEF_METASTR8 is NULL) and (USERDEF_METASTR9 = '%s'  OR USERDEF_METASTR9 is NULL) and (USERDEF_METAINT0 = %i  OR USERDEF_METAINT0 is NULL) and (USERDEF_METAINT1 = %i OR USERDEF_METAINT1 is NULL) ", 
	    MDASSCHEMENAME, objId, rval[i], rval[i+1], rval[i+2], rval[i+3], rval[i+4], rval[i+5], rval[i+6], rval[i+7], rval[i+8], rval[i+9], atoi(rval[i+10]), atoi(rval[i+11]));
    if (check_exists(metadataString) == MDAS_SUCCESS)
	return (METADATA_ALREADY_IN_CAT);
    sprintf(metadataString,"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",rval[i], rval[i+1], rval[i+2], rval[i+3], rval[i+4], rval[i+5], rval[i+6], rval[i+7], rval[i+8], rval[i+9], rval[i+10], rval[i+11]);
    ii = bulkInsertUdefMetadataForResource(objId, metadataString, "|", rval[15]);
    free(metadataString);
    return(ii);
  }
  else if(!strcmp(ObjectType,"Data")) {
    splitbychar(inObjectInfo, objInfo,objInfo2,'/');
    objId = getDataId(objInfo,objInfo2);
    j = 0;
    for (i = startInx  ; i <=endInx; i++)
      j += strlen(rval[i]);
    metadataString = malloc (j+3000);
    i = startInx;
    if (strlen(rval[i+10]) == 0)
      strcpy(rval[i+10],"0");
    if (strlen(rval[i+11]) == 0)
      strcpy(rval[i+11],"0");
    sprintf(metadataString,"select METADATANUM from %sMDAS_AD_MDATA where data_id = %i and (USERDEF_METASTR0 = '%s' OR USERDEF_METASTR0 is NULL) and (USERDEF_METASTR1 = '%s' OR USERDEF_METASTR1 is NULL)  and (USERDEF_METASTR2 = '%s' OR USERDEF_METASTR2 is NULL)  and (USERDEF_METASTR3 = '%s' OR USERDEF_METASTR3 is NULL) and (USERDEF_METASTR4 = '%s'  OR USERDEF_METASTR4 is NULL) and (USERDEF_METASTR5 = '%s'  OR USERDEF_METASTR5 is NULL) and (USERDEF_METASTR6 = '%s' OR USERDEF_METASTR6 is NULL)  and (USERDEF_METASTR7 = '%s'  OR USERDEF_METASTR7 is NULL) and (USERDEF_METASTR8 = '%s'  OR USERDEF_METASTR8 is NULL) and (USERDEF_METASTR9 = '%s'  OR USERDEF_METASTR9 is NULL) and (USERDEF_METAINT0 = %i  OR USERDEF_METAINT0 is NULL) and (USERDEF_METAINT1 = %i OR USERDEF_METAINT1 is NULL)  ", 
	    MDASSCHEMENAME, objId, rval[i], rval[i+1], rval[i+2], rval[i+3], rval[i+4], rval[i+5], rval[i+6], rval[i+7], rval[i+8], rval[i+9], atoi(rval[i+10]), atoi(rval[i+11]));
    if (check_exists(metadataString) == MDAS_SUCCESS)
	return (METADATA_ALREADY_IN_CAT);
    sprintf(metadataString,"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",rval[i], rval[i+1], rval[i+2], rval[i+3], rval[i+4], rval[i+5], rval[i+6], rval[i+7], rval[i+8], rval[i+9], rval[i+10],rval[i+11]);
    ii = bulkInsertUdefMetadataForData(objId, metadataString, "|", rval[15]);
    free(metadataString);
    return(ii);
  }
  else if(!strcmp(ObjectType,"Collection")) {
    get_ctype_value(objInfo, "MDAS_TD_DATA_GRP.data_grp_name",inObjectInfo);
    j = 0;
    for (i = startInx  ; i <=endInx; i++)
      j += strlen(rval[i]);
    metadataString = malloc (j+3000);
    i = startInx;
    if (strlen(rval[i+10]) == 0)
      strcpy(rval[i+10],"0");
    if (strlen(rval[i+11]) == 0)
      strcpy(rval[i+11],"0");
    sprintf(metadataString,"select METADATANUM from %sMDAS_AD_COLLMDATA where data_grp_id = '%s' and (USERDEF_METASTR0 = '%s' OR USERDEF_METASTR0 is NULL) and (USERDEF_METASTR1 = '%s' OR USERDEF_METASTR1 is NULL)  and (USERDEF_METASTR2 = '%s' OR USERDEF_METASTR2 is NULL)  and (USERDEF_METASTR3 = '%s' OR USERDEF_METASTR3 is NULL)  and (USERDEF_METASTR4 = '%s'  OR USERDEF_METASTR4 is NULL) and (USERDEF_METASTR5 = '%s'  OR USERDEF_METASTR5 is NULL) and (USERDEF_METASTR6 = '%s' OR USERDEF_METASTR6 is NULL)  and (USERDEF_METASTR7 = '%s'  OR USERDEF_METASTR7 is NULL) and (USERDEF_METASTR8 = '%s'  OR USERDEF_METASTR8 is NULL) and (USERDEF_METASTR9 = '%s'  OR USERDEF_METASTR9 is NULL) and (USERDEF_METAINT0 = %i  OR USERDEF_METAINT0 is NULL) and (USERDEF_METAINT1 = %i OR USERDEF_METAINT1 is NULL)  ", 
	    MDASSCHEMENAME, objInfo, rval[i], rval[i+1], rval[i+2], rval[i+3], rval[i+4], rval[i+5], rval[i+6], rval[i+7], rval[i+8], rval[i+9], atoi(rval[i+10]), atoi(rval[i+11]));
    if (check_exists(metadataString) == MDAS_SUCCESS)
	return (METADATA_ALREADY_IN_CAT);
    sprintf(metadataString,"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",rval[i], rval[i+1], rval[i+2], rval[i+3], rval[i+4], rval[i+5], rval[i+6], rval[i+7], rval[i+8], rval[i+9], rval[i+10],rval[i+11]);
    ii = bulkInsertUdefMetadataForColl(objInfo, metadataString, "|", rval[15]);
    free(metadataString);
    return(ii);
  }
  else
    return (UNKNOWN_FUNCTION_MAP_ID);
  
}

int 
ibmCollCore(int cat_type,
	    char *rval[],
	    char *userName,
	    char *domainName)
{
  int i;
  char tmpCollName[MAX_TOKEN];
  char remCollName[MAX_TOKEN];
  char sval[MAX_TOKEN * 2];
  if (!strcmp(rval[5],rval[6])) { /* new collection */
    splitbychar(rval[0], tmpCollName, remCollName,'/');    
    sprintf(sval,"%s|%s",rval[3],rval[4]);
    i = make_new_collection_path(cat_type, tmpCollName, remCollName,rval[2],sval);
    if (i < 0) 
      return(i);
    if (strcmp(rval[1],tmpCollName)) {
      sprintf(sqlq,"update %sMDAS_TD_DATA_GRP set parent_grp_name = '%s', coll_cr_timestamp = '%s',coll_comments = '%s',  coll_mod_timestamp = '%s' where data_grp_name ='%s' and parent_grp_name ='%s'",MDASSCHEMENAME, rval[1],rval[5], rval[7],rval[6], rval[0],tmpCollName);
    }
    else {
      sprintf(sqlq,"update %sMDAS_TD_DATA_GRP set coll_cr_timestamp = '%s',coll_comments = '%s',  coll_mod_timestamp = '%s' where data_grp_name ='%s' and parent_grp_name ='%s'",
	      MDASSCHEMENAME, rval[5], rval[7],rval[6], rval[0],tmpCollName);
    }
   i = ExecSqlDb2(henv, hdbc, sqlq);
   if (i < 0)
     return(MDAS_TD_DATA_GRP_UPDATE_ERROR);
  }
  else { /* old collection */
    splitbychar(rval[0], tmpCollName, remCollName,'/');    
    sprintf(sqlq,"update %sMDAS_TD_DATA_GRP set coll_cr_timestamp = '%s',coll_comments = '%s',  coll_mod_timestamp = '%s' where data_grp_name ='%s' and parent_grp_name ='%s'",
	      MDASSCHEMENAME, rval[5], rval[7],rval[6], rval[0],tmpCollName);
    i = ExecSqlDb2(henv, hdbc, sqlq);
    if (i < 0)
      return(MDAS_TD_DATA_GRP_UPDATE_ERROR);
  }
  return(MDAS_SUCCESS);
}



int 
ibmDataCore(int cat_type,
	    char *rval[],
	    char *userName,
	    char *domainName)
{
  int i, repl_enum;
  char sval[MAX_TOKEN * 2];
  char sval2[MAX_TOKEN * 2];

  sprintf(sval,"%s|%s",rval[10],rval[11]);
  if (!strcmp(rval[5],rval[16])) { /* new data */
    if (!strcmp(rval[1],"0")) { /* first one */
      i =  register_dataset_info(cat_type,rval[0],rval[9],"all",
			       sval,rval[2],rval[3],
			       rval[4],rval[6],atol(rval[7]));
      if (i < 0)
	return(i);
      repl_enum = 0;
    }
    else if (!strcmp(rval[1],"-1")){ /* incontainerobject */
      sprintf(sval2,"%s/%s",rval[15],rval[14]);
      i =  register_incontainer_dataset (cat_type,rval[0],rval[6],rval[9],sval,sval2,rval[2],
					 atol(rval[7]),atol(rval[13]));
      if (i < 0)
	return(i);
      repl_enum = -1;

    }
    else { /*replicate */
      i = copy_dataset_with_repl_enum (cat_type,rval[0],rval[6],"","",rval[4],rval[3],
				       rval[9],sval,-1);
      if (i < 0)
	return(i);				       
      repl_enum = i;
    }
    /* change dates and comments and such ***/
    failure = 0; 
    tabledIsOkay = 1;
    get_ctype_value(cval[13], "MDAS_TD_DATA_GRP.data_grp_name",rval[6]);
    if (failure != 0) 
      return(COLLECTION_NOT_IN_CAT);		    
    tabledIsOkay = 0;
    if (atoi(rval[1]) > 0) {
      tabledIsOkay = 1;
      get_ctype_value(cval[2], "MDAS_TD_DATA_TYP.data_typ_name", rval[2]);
      if (failure != 0) 
	return(DATA_TYPE_NOT_IN_CAT);
      tabledIsOkay = 0;
      sprintf(sqlq,"update %sMDAS_AD_REPL set repl_timestamp = '%s',d_comment = '%s', modify_timestamp = '%s', seg_num = %s, data_typ_id='%s' , data_size = %s, dchecksum = '%s', version_num = '%s' where data_grp_id ='%s' and data_name ='%s' and repl_enum = %i",
	      MDASSCHEMENAME, rval[16], rval[24],rval[5],rval[29],cval[2],
	      rval[7],rval[26],rval[22],
	      cval[13],rval[0],repl_enum);
    }
    else {
      sprintf(sqlq,"update %sMDAS_AD_REPL set repl_timestamp = '%s',d_comment = '%s', modify_timestamp = '%s', seg_num = %s, dchecksum = '%s', version_num = '%s' where data_grp_id ='%s' and data_name ='%s' and repl_enum = %i",
	      MDASSCHEMENAME, rval[16], rval[24],rval[5],rval[29],rval[26],rval[22],
	      cval[13],rval[0],repl_enum);
    }
    i = ExecSqlDb2(henv, hdbc, sqlq);
    if (i < 0)
      return(MDAS_AD_REPL_UPDATE_ERROR);
  }
  else {/* old data */
    failure = 0; 
    tabledIsOkay = 1;
    get_ctype_value(cval[13], "MDAS_TD_DATA_GRP.data_grp_name",rval[6]);
    if (failure != 0) 
      return(COLLECTION_NOT_IN_CAT);		    
    get_ctype_value(cval[2], "MDAS_TD_DATA_TYP.data_typ_name", rval[2]);
    if (failure != 0) 
      return(DATA_TYPE_NOT_IN_CAT);
    ival[4] = get_user_id_in_domain(rval[9],sval);
    if (ival[4] < 0) { return (DATA_OWNER_DOMAIN_NOT_FOUND);}
    ival[18] = get_itype_value("MDAS_CD_RSRC.rsrc_name", rval[4]); 
    if (failure != 0) { return (RESOURCE_NOT_IN_CAT);}
    tabledIsOkay = 0;
    if (atoi(rval[18]) == 0) {
      sprintf(sqlq,"update %sMDAS_AD_REPL set repl_timestamp = '%s',d_comment = '%s', modify_timestamp = '%s', seg_num = %s, data_typ_id='%s' , data_size = %s,path_name = '%s',rsrc_id = %i,is_deleted = %s,data_owner = %i,is_dirty = %s,offset = %s,last_accs_time = '%s', version_num = '%s',dchecksum = '%s', dhide = %s, auditflag = %s where data_grp_id ='%s' and data_name ='%s' and repl_enum = %i",
	      MDASSCHEMENAME, rval[16], rval[24],rval[5],rval[29],cval[2], rval[7], rval[3],
	      ival[18],rval[8],ival[4],rval[12], rval[13], rval[17], rval[22],rval[26],rval[27],
	      rval[28],
	      cval[13],rval[0],repl_enum);
    }
    else {
      sprintf(sval2,"%s|%s",rval[20],rval[21]);
      tabledIsOkay = 1;
      ival[5] = get_user_id_in_domain(rval[19],sval2);
      if (ival[5] < 0) { return (DATA_LOCK_OWNER_DOMAIN_NOT_FOUND);}
      tabledIsOkay = 0;
      sprintf(sqlq,"update %sMDAS_AD_REPL set repl_timestamp = '%s',d_comment = '%s', modify_timestamp = '%s', seg_num = %s, data_typ_id='%s' , data_size = %s,path_name = '%s',rsrc_id = %i,is_deleted = %s,data_owner = %i,is_dirty = %s,offset = %s,last_accs_time = '%s', version_num = '%s',dchecksum = '%s', dhide = %s, auditflag = %s,datalock = %s,lockowner = %i,dlockexpiry = '%s' where data_grp_id ='%s' and data_name ='%s' and repl_enum = %i",
	      MDASSCHEMENAME, rval[16], rval[24],rval[5],rval[29],cval[2], rval[7], rval[3],
	      ival[18],rval[8],ival[4],rval[12], rval[13], rval[17], rval[22],rval[26],rval[27],
	      rval[28],rval[18],ival[5],rval[25],
	      cval[13],rval[0],repl_enum);
    }
    i = ExecSqlDb2(henv, hdbc, sqlq);
    if (i < 0)
      return(MDAS_AD_REPL_UPDATE_ERROR);
  }
  return(MDAS_SUCCESS);
}

int 
ibmContainerCore(int cat_type,
		 char *rval[],
		 char *userName,
		 char *domainName)
{
  int i,j;

  failure = 0; 
  tabledIsOkay = 1;
  ival[18] = get_itype_value("MDAS_CD_RSRC.rsrc_name", rval[2]); 
  if (failure != 0) { return (RESOURCE_NOT_IN_CAT);}
  tabledIsOkay = 0;
  sprintf(sqlq,"select container_id from %sMDAS_TD_CONTAINER where container_name='%s'",
	 MDASSCHEMENAME,rval[0] );
  if (check_exists(sqlq) != 0) {
    splitbychar( rval[0], cval[13],cval[14],'/');
    j  = getDataId(cval[13],cval[14]);
    sprintf(sqlq,"insert into %sMDAS_TD_CONTAINER (container_id,container_name,container_max_size,rsrc_id) values (%i,'%s',%s,%i)",
	    MDASSCHEMENAME,j,rval[0],rval[1], ival[18]);
  }
  else {
    sprintf(sqlq, "update %sMDAS_TD_CONTAINER set container_max_size = %s ,rsrc_id = %i where container_name='%s'",
	    MDASSCHEMENAME,rval[1], ival[18],rval[0]);
  }
  
  i = ExecSqlDb2(henv, hdbc, sqlq);
  if (i < 0)
    return(MDAS_TD_CONTAINER_INSERTION_ERROR);
  return(MDAS_SUCCESS);

}

int 
ibmAnnotation(int cat_type,
	      char *objType,
	      char *rval[],
	      char *userName,
	      char *domainName)
{
  int i,j;
  char sval2[MAX_TOKEN * 2];

  failure = 0;
  if (!strcmp(objType,"Data")) {
    j  = getDataId( rval[1], rval[0]);
    sprintf(sqlq,"select data_id from %sMDAS_AD_ANNOTATION where data_id = %i and repl_enum = %s anno_date='%s'",
	    MDASSCHEMENAME,j,rval[8], rval[6]);
    if (check_exists(sqlq) != 0) {
      sprintf(sval2,"%s|%s",rval[3],rval[4]);
      failure = 0;
      tabledIsOkay = 1;
      ival[5] = get_user_id_in_domain(rval[2],sval2);
      if (ival[5] < 0) { return (USER_NAME_NOT_FOUND);}
      tabledIsOkay = 0;
      sprintf(sqlq,"insert into %sMDAS_AD_ANNOTATION (data_id,repl_enum,user_id,anno_date,annotations,anno_position) values (%i,%s,%i,'%s','%s','%s') ",
	      MDASSCHEMENAME,j,rval[8],ival[5],rval[6],rval[5],rval[7]);
    }
    else {
      sprintf(sqlq,"update  %sMDAS_AD_ANNOTATION  set annotations='%s', anno_position='%s' where data_id = %i and repl_enum = %s anno_date='%s'",
	    MDASSCHEMENAME, rval[5], rval[7], j, rval[8], rval[6]);
    }
    i = ExecSqlDb2(henv, hdbc, sqlq);
    if (i < 0)
      return(MDAS_AD_ANNOTATION_INSERTION_ERROR);

  }
  else if (!strcmp(objType,"Collection")) {
    failure = 0; 
    tabledIsOkay = 1;
    get_ctype_value(cval[13], "MDAS_TD_DATA_GRP.data_grp_name",rval[0]);
    if (failure != 0) 
      return(COLLECTION_NOT_IN_CAT);		    
    tabledIsOkay = 0;

    sprintf(sqlq,"select data_id from %sMDAS_AC_ANNOTATION where data_grp_id = '%s' anno_date='%s'",
	    MDASSCHEMENAME,cval[13], rval[5]);
    if (check_exists(sqlq) != 0) {
      sprintf(sval2,"%s|%s",rval[2],rval[3]);
      failure = 0;
      tabledIsOkay = 1;
      ival[5] = get_user_id_in_domain(rval[1],sval2);
      if (ival[5] < 0) { return (USER_NAME_NOT_FOUND);}
      tabledIsOkay = 0;
      sprintf(sqlq,"insert into %sMDAS_AC_ANNOTATION (data_grp_id,user_id,anno_date,annotations,anno_type) values (%s,%i,'%s','%s','%s') ",
	      MDASSCHEMENAME,cval[13],ival[5],rval[5],rval[4],rval[6]);
    }
    else {
      sprintf(sqlq,"update  %sMDAS_AC_ANNOTATION  set annotations='%s', anno_type='%s' where data_grp_id = %i and repl_enum = %s anno_date='%s'",
	    MDASSCHEMENAME, rval[4], rval[6], cval[13], rval[5]);
    }
    i = ExecSqlDb2(henv, hdbc, sqlq);
    if (i < 0)
      return(MDAS_AC_ANNOTATION_INSERTION_ERROR);

  }
  else {
    return(FUNCTION_NOT_SUPPORTED);
  }
  return(MDAS_SUCCESS);

}

int 
ibmContColl(int cat_type,
	    char *rval[],
	    char *userName,
	    char *domainName)
{
  return(FUNCTION_NOT_SUPPORTED);
}

int 
ibmDataMisc1(int cat_type,
	     char *rval[],
	     char *userName,
	     char *domainName)
{
  return(FUNCTION_NOT_SUPPORTED);

}

int zoneChangeCollName(char *collName)
{
  char *tmpPtr;
  char newCollName[MAX_TOKEN * 2];
  char localZoneName[MAX_TOKEN];
  int i;

  if ((strstr(collName,"/home") == collName ) ||
      (strstr(collName,"/container") == collName ) ||
      (strstr(collName,"/style") == collName ) ||
      (strstr(collName,"/trash") == collName ) )
   
    return(MDAS_SUCCESS);
  
  if (strlen(localZoneForMcat) == 0) {
    failure = 0;
    sprintf(query_string, "select zone_id from %s%s where local_zone_flag = 1",
	    MDASSCHEMENAME, "MDAS_TD_ZONE");
    get_ctype_value_from_query(localZoneForMcat,query_string );
    if (failure != 0) 
      return( LOCAL_ZONE_NOT_FOUND);
  }
  tmpPtr = strstr((char *) (collName + 1), "/");
  if (tmpPtr != NULL) {
    sprintf(newCollName,"/%s%s",localZoneForMcat,tmpPtr);
    strcpy(collName,newCollName);
  }
  return(MDAS_SUCCESS);
}
int 
ibmDataGuid(int cat_type,
	      char *rval[],
	      char *userName,
	      char *domainName)
{
  int i,j;
  char sval2[MAX_TOKEN * 2];

  failure = 0;
  j  = getDataId( rval[1], rval[0]);
  sprintf(sqlq,"select data_id from %sMDAS_AD_GUID where data_id = %i ",
	    MDASSCHEMENAME,j);
    if (check_exists(sqlq) != 0) {
      sprintf(sqlq,"insert into %sMDAS_AD_GUID (data_id,guid,guid_flag, guid_time_stamp) values (%i,'%s',%s,'%s') ",
	      MDASSCHEMENAME,j,rval[2],rval[3],rval[4]);
    }
    else {
      sprintf(sqlq,"update  %sMDAS_AD_GUID  set guid='%s', guid_flag=%s, guid_time_stamp='%s' where data_id = %i",
	    MDASSCHEMENAME, rval[2],rval[3],rval[4], j);
    }
    i = ExecSqlDb2(henv, hdbc, sqlq);
    if (i < 0)
      return(MDAS_AD_ANNOTATION_INSERTION_ERROR);
  return(MDAS_SUCCESS);
}

#endif /* SRB_MDAS*/
