/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

#include "mdasC_db2_externs.h"
#include "mdasGlobalsExtern.h"
#include "mdasPrototypes.h"

#include "srbC_mdas_externs.h"
#include "mcatAdminExtern.h"

#define ID_LIMIT 30
/***
#define ExecSqlDb2 testExecSqlDb2
#define modify_collection_info   testmodify_collection_info
#define check_exists testcheck_exists
***/
int
changeSingleMcatValue(int valueType, char *newValue, char *oldValue,
		  char *regUserName, char *regDomainName, char *regUserAuth)
{
  int i,j;
  char tabName[MAX_TOKEN], tabName2[MAX_TOKEN];
  char attrName[MAX_TOKEN], attrName2[MAX_TOKEN];
  char sqlq[HUGE_STRING];
  char userName[MAX_TOKEN], domainName[MAX_TOKEN];
  char *tmpPtr;
  char oldCollName[MAX_TOKEN], newCollName[MAX_TOKEN];
  int idInt;
  char idChar[MAX_TOKEN];
  strcpy(tabName2,"");
  switch(valueType)
    {
    case PHYSICAL_RESOURCE_NAME:
      strcpy(tabName,"MDAS_AR_REPL");
      strcpy(attrName,"rsrc_name");
      strcpy(tabName2,"MDAS_AR_PHYSICAL");
      strcpy(attrName2,"phy_rsrc_name");
      break;
    case LOGICAL_RESOURCE_NAME:
      strcpy(tabName,"MDAS_AR_REPL");
      strcpy(attrName,"rsrc_name");
      break;
    case USER_NAME:
      strcpy(tabName,"MDAS_CD_USER");
      strcpy(attrName,"user_name");
      break;
    case USER_DOMAIN:
      strcpy(tabName,"MDAS_AU_DOMN");
      strcpy(attrName,"domain_id");
      break;
    case TYPE_OF_USER:
      strcpy(tabName,"MDAS_CD_USER");
      strcpy(attrName,"user_typ_id");
      break;
    case USER_GROUP_NAME:
      strcpy(tabName,"MDAS_CD_USER");
      strcpy(attrName,"user_name");
      break;
    case DOMAIN_NAME:
      strcpy(tabName,"MDAS_TD_DOMN");
      strcpy(attrName,"domain_desc");
      break;
    case USER_TYPE:
      strcpy(tabName,"MDAS_TD_USER_TYP");
      strcpy(attrName,"user_typ_name");
      break;
    case RESOURCE_TYPE:
      strcpy(tabName,"MDAS_TD_RSRC_TYP");
      strcpy(attrName,"rsrc_typ_name");
      break;
    case DATA_TYPE:
      strcpy(tabName,"MDAS_TD_DATA_TYP");
      strcpy(attrName,"data_typ_name");
      break;
    case ACTION_NAME:
      strcpy(tabName,"MDAS_TD_ACTN");
      strcpy(attrName,"action_desc");
      break;
    case RESOURCE_CLASS:
      strcpy(tabName,"MDAS_TD_RSRC_CLASS");
      strcpy(attrName,"rsrc_class_name");
      break;
    case RESOURCE_LOCATION_NAME:
      strcpy(tabName,"MDAS_TD_LOCN");
      strcpy(attrName,"locn_desc");
      break;
    case RESOURCE_LOCATION_NETADDRESS:
      strcpy(tabName,"MDAS_TD_LOCN");
      strcpy(attrName,"netprefix");
      break;
    default:
      return (TOKEN_IDEN_TYPE_ERROR);
      break;
    }
  if (valueType != USER_DOMAIN && 
      valueType != USER_NAME && 
      valueType != USER_GROUP_NAME && 
      valueType != TYPE_OF_USER) {
    sprintf(sqlq, "update %s%s set %s = '%s' where %s = '%s'",
	    MDASSCHEMENAME, tabName, attrName, newValue,attrName, oldValue); 
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0)
      return(METADATA_INSERTION_ERROR);
    if (strlen(tabName2) == 0)
      return(MDAS_SUCCESS);
    sprintf(sqlq, "update %s%s set %s = '%s' where %s = '%s'",
	 MDASSCHEMENAME, tabName2, attrName2, newValue,attrName2, oldValue); 
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0)
      return(METADATA_INSERTION_ERROR);
    else
      return(MDAS_SUCCESS);
  }
  else { /* changing user domain or name  or type */
    if (valueType == USER_GROUP_NAME) 
      sprintf(userName,"%s@groups",oldValue);
    else
      strcpy(userName,oldValue);
    if ((tmpPtr = strstr(userName,"@")) == NULL)
      return(DATA_VALUE_TYPE_ERROR);
    *tmpPtr = '\0';
    tmpPtr++;
    strcpy(domainName,tmpPtr);
    idInt = get_user_id_in_domain(userName, domainName);
    if (idInt < 0)
      return(idInt);
    if (valueType == USER_NAME) {
      sprintf(sqlq, "update %s%s set %s = '%s' where user_id = %i",
	MDASSCHEMENAME, tabName, attrName, newValue, idInt); 
      res = ExecSqlDb2(henv, hdbc, sqlq);
      if (res != 0)
	return(METADATA_INSERTION_ERROR);
      sprintf(oldCollName,"%s/%s.%s", DEF_PARENT_COLL, userName, domainName);
      sprintf(newCollName,"%s/%s.%s", DEF_PARENT_COLL, newValue, domainName);
      res = modify_collection_info(0, regUserName, oldCollName,
			       newCollName,"","",
			       D_CHANGE_COLL_NAME, regDomainName);
      if (res != 0)
	return(res);
      sprintf(oldCollName,"%s/%s.%s", DEF_CONTAINER_COLL,userName, domainName);
      sprintf(newCollName,"%s/%s.%s", DEF_CONTAINER_COLL,newValue, domainName);
      res = modify_collection_info(0, regUserName, oldCollName,
			       newCollName,"","",
			       D_CHANGE_COLL_NAME, regDomainName);
      if (res != 0)
	return(res);
      else
	return(MDAS_SUCCESS);
    }    
    else if ( valueType == USER_DOMAIN ) {
      failure = 0;
      get_ctype_value(idChar, "MDAS_TD_DOMN.domain_desc", newValue);
      if (failure != 0) 
	return (MDAS_TD_DOMN_ACCESS_ERROR);
      sprintf(sqlq, "update %s%s set %s = '%s' where user_id = %i",
	MDASSCHEMENAME, tabName, attrName, idChar, idInt); 
      res = ExecSqlDb2(henv, hdbc, sqlq);
      if (res != 0)
	return(METADATA_INSERTION_ERROR);
      sprintf(oldCollName,"%s/%s.%s", DEF_PARENT_COLL, userName, domainName);
      sprintf(newCollName,"%s/%s.%s", DEF_PARENT_COLL, userName, newValue);
      res = modify_collection_info(0, regUserName, oldCollName,
			       newCollName,"","",
			       D_CHANGE_COLL_NAME, regDomainName);
      if (res != 0)
	return(res);
      sprintf(oldCollName,"%s/%s.%s", DEF_CONTAINER_COLL,userName, domainName);
      sprintf(newCollName,"%s/%s.%s", DEF_CONTAINER_COLL,userName, newValue);
      res = modify_collection_info(0, regUserName, oldCollName,
			       newCollName,"","",
			       D_CHANGE_COLL_NAME, regDomainName);
      if (res != 0)
	return(res);
      else
	return(MDAS_SUCCESS);
    }
    else { /* valueType == TYPE_OF_USER */
      failure = 0;
      get_ctype_value(idChar, "MDAS_TD_USER_TYP.user_typ_name", newValue);
      if (failure != 0) 
	return (MDAS_TD_DOMN_ACCESS_ERROR);
      sprintf(sqlq, "update %s%s set %s = '%s' where user_id = %i",
	      MDASSCHEMENAME, tabName, attrName, idChar, idInt); 
      res = ExecSqlDb2(henv, hdbc, sqlq);
      if (res != 0)
	return(METADATA_INSERTION_ERROR);



      else
	return(MDAS_SUCCESS);
    }
  }
}

int
deleteSingleMcatValue(int valueType, char *delValue, 
		  char *regUserName, char *regDomainName, char *regUserAuth)
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
  
  char sqlq[HUGE_STRING];
  
  for (i = 0; i < ID_LIMIT ; i++) {
    extraChkId[i][0] = '\0';
    msgChkId[i][0] = '\0';
  }
  switch(valueType)
    {
    case PHYSICAL_RESOURCE_NAME:
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
      /*** strcpy(tabNameDelId[0],"MDAS_AR_COMMENTS");
      strcpy(attrNameDelId[0],"rsrc_id"); ****/
      strcpy(tabNameDelId[0],"MDAS_AR_ACCS");
      strcpy(attrNameDelId[0],"rsrc_id");
      strcpy(tabNameDelId[1],"MDAS_AR_AUDIT");
      strcpy(attrNameDelId[1],"rsrc_id");
      strcpy(tabNameDelId[2],"MDAS_AR_INFO");
      strcpy(attrNameDelId[2],"rsrc_id");
      strcpy(tabNameDelId[3],"MDAS_AR_DB_RSRC");
      strcpy(attrNameDelId[3],"rsrc_id");
      strcpy(tabNameDelId[4],"MDAS_AR_LRSRC_RRBN");
      strcpy(attrNameDelId[4],"rsrc_id");
      strcpy(tabNameDelId[5],"MDAS_AR_LRSRC_RRBN");
      strcpy(attrNameDelId[5],"phy_rsrc_id");
      delIdCnt = 6;
      strcpy(tabNameDelName[0],"MDAS_AR_REPL");
      strcpy(attrNameDelName[0],"rsrc_name");
      strcpy(tabNameDelName[1],"MDAS_AR_PHYSICAL");
      strcpy(attrNameDelName[1],"phy_rsrc_name");
      strcpy(tabNameDelName[2],"MDAS_CD_RSRC");
      strcpy(attrNameDelName[2],"rsrc_name");
      delNameCnt = 3;
      break;
    case LOGICAL_RESOURCE_NAME:
      idType = 0;
      strcpy(tabNameChkId[0],"MDAS_TD_CONTAINER");
      strcpy(attrNameChkId[0],"rsrc_id");
      strcpy(msgChkId[0],"Container in the Resource");
      strcpy(tabNameChkId[1],"MDAS_AD_REPL");
      strcpy(attrNameChkId[1],"rsrc_id");
      strcpy(msgChkId[1],"SRB Data Object in the Resource");
      chkIdCnt = 2;
      /*** strcpy(tabNameDelId[0],"MDAS_AR_COMMENTS");
      strcpy(attrNameDelId[0],"rsrc_id");  ***/
      strcpy(tabNameDelId[0],"MDAS_AR_ACCS");
      strcpy(attrNameDelId[0],"rsrc_id");
      strcpy(tabNameDelId[1],"MDAS_AR_AUDIT");
      strcpy(attrNameDelId[1],"rsrc_id");
      strcpy(tabNameDelId[2],"MDAS_AR_INFO");
      strcpy(attrNameDelId[2],"rsrc_id");
      strcpy(tabNameDelId[3],"MDAS_AR_DB_RSRC");
      strcpy(attrNameDelId[3],"rsrc_id");
      strcpy(tabNameDelId[4],"MDAS_AR_LRSRC_RRBN");
      strcpy(attrNameDelId[4],"rsrc_id");
      delIdCnt = 5;
      strcpy(tabNameDelName[0],"MDAS_AR_REPL");
      strcpy(attrNameDelName[0],"rsrc_name");
      strcpy(tabNameDelName[1],"MDAS_CD_RSRC");
      strcpy(attrNameDelName[1],"rsrc_name");
      delNameCnt = 2;
      break;
    case USER_NAME:
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
      /*** strcpy(tabNameDelId[6],"MDAS_AR_COMMENTS");
      strcpy(attrNameDelId[6],"user_id"); ****/
      strcpy(tabNameDelId[6],"MDAS_AR_ACCS");
      strcpy(attrNameDelId[6],"user_id");
      strcpy(tabNameDelId[7],"MDAS_AR_AUDIT");
      strcpy(attrNameDelId[7],"user_id");
      strcpy(tabNameDelId[8],"MDAS_AD_COMMENTS");
      strcpy(attrNameDelId[8],"user_id");
      strcpy(tabNameDelId[9],"MDAS_AD_ANNOTATION");
      strcpy(attrNameDelId[9],"user_id");
      strcpy(tabNameDelId[10],"MDAS_AD_ACCS");
      strcpy(attrNameDelId[10],"user_id");
      strcpy(tabNameDelId[11],"MDAS_AD_GRP_ACCS");
      strcpy(attrNameDelId[11],"user_id");
      strcpy(tabNameDelId[12],"MDAS_AD_AUDIT");
      strcpy(attrNameDelId[12],"user_id");
      strcpy(tabNameDelId[13],"MDAS_AU_TCKT_DATA");
      strcpy(attrNameDelId[13],"user_id");
      strcpy(tabNameDelId[14],"MDAS_AU_TCKT_DATA");
      strcpy(attrNameDelId[14],"ticket_owner");
      strcpy(tabNameDelId[15],"MDAS_AU_TCKT_GRP");
      strcpy(attrNameDelId[15],"user_id");
      strcpy(tabNameDelId[16],"MDAS_AU_TCKT_GRP");
      strcpy(attrNameDelId[16],"ticket_owner");
      strcpy(tabNameDelId[17],"MDAS_TD_DATA_GRP");
      strcpy(attrNameDelId[17],"coll_owner");
      strcpy(tabNameDelId[18],"MDAS_CD_USER");
      strcpy(attrNameDelId[18],"user_id");
      delIdCnt = 19;
      delNameCnt = 0; 
      break;
    case USER_GROUP_NAME:
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
      /*** strcpy(tabNameDelId[6],"MDAS_AR_COMMENTS");
      strcpy(attrNameDelId[6],"user_id"); ****/
      strcpy(tabNameDelId[6],"MDAS_AR_ACCS");
      strcpy(attrNameDelId[6],"user_id");
      strcpy(tabNameDelId[7],"MDAS_AR_AUDIT");
      strcpy(attrNameDelId[7],"user_id");
      strcpy(tabNameDelId[8],"MDAS_AD_COMMENTS");
      strcpy(attrNameDelId[8],"user_id");
      strcpy(tabNameDelId[9],"MDAS_AD_ANNOTATION");
      strcpy(attrNameDelId[9],"user_id");
      strcpy(tabNameDelId[10],"MDAS_AD_ACCS");
      strcpy(attrNameDelId[10],"user_id");
      strcpy(tabNameDelId[11],"MDAS_GRP_ACCS");
      strcpy(attrNameDelId[11],"user_id");
      strcpy(tabNameDelId[12],"MDAS_AD_AUDIT");
      strcpy(attrNameDelId[12],"user_id");
      strcpy(tabNameDelId[13],"MDAS_AU_TCKT_DATA");
      strcpy(attrNameDelId[13],"user_id");
      strcpy(tabNameDelId[14],"MDAS_AU_TCKT_DATA");
      strcpy(attrNameDelId[14],"ticket_owner");
      strcpy(tabNameDelId[15],"MDAS_AU_TCKT_GRP");
      strcpy(attrNameDelId[15],"user_id");
      strcpy(tabNameDelId[16],"MDAS_AU_TCKT_GRP");
      strcpy(attrNameDelId[16],"ticket_owner");
      strcpy(tabNameDelId[17],"MDAS_AU_GROUP");
      strcpy(attrNameDelId[17],"group_user_id");
      strcpy(tabNameDelId[18],"MDAS_TD_DATA_GRP");
      strcpy(attrNameDelId[18],"coll_owner");
      strcpy(tabNameDelId[19],"MDAS_CD_USER");
      strcpy(attrNameDelId[19],"user_id");
      delIdCnt = 20;
      delNameCnt = 0; 
      break;
    case DOMAIN_NAME:
      idType = 1;
      strcpy(tabNameChkId[0],"MDAS_AU_DOMN");
      strcpy(attrNameChkId[0],"domain_id");
      chkIdCnt = 1;
      delIdCnt = 0;
      strcpy(tabNameDelName[0],"MDAS_TD_DOMN");
      strcpy(attrNameDelName[0],"domain_desc");
      delNameCnt = 1;
      break;
    case USER_TYPE:
      idType = 1;
      strcpy(tabNameChkId[0],"MDAS_CD_USER");
      strcpy(attrNameChkId[0],"user_typ_id");
      chkIdCnt = 1;
      delIdCnt = 0;
      strcpy(tabNameDelName[0],"MDAS_TD_USER_TYP");
      strcpy(attrNameDelName[0],"user_typ_name");
      delNameCnt = 1;
      break;
    case RESOURCE_TYPE:
      idType = 1;
      strcpy(tabNameChkId[0],"MDAS_CD_RSRC");
      strcpy(attrNameChkId[0],"rsrc_typ_id");
      strcpy(tabNameChkId[1],"MDAS_AR_PHYSICAL");
      strcpy(attrNameChkId[1],"rsrc_typ_id");
      strcpy(tabNameChkId[2],"MDAS_AR_REPL");
      strcpy(attrNameChkId[2],"rsrc_typ_id");
      strcpy(tabNameChkId[3],"MDAS_AR_DB_RSRC");
      strcpy(attrNameChkId[3],"rsrc_typ_id");
      chkIdCnt = 4;
      delIdCnt = 0;
      strcpy(tabNameDelName[0],"MDAS_TD_RSRC_TYP");
      strcpy(attrNameDelName[0],"rsrc_typ_name");
      delNameCnt = 1;
      break;
    case DATA_TYPE:
      idType = 1;
      strcpy(tabNameChkId[0],"MDAS_CD_DATA");
      strcpy(attrNameChkId[0],"data_typ_id");
      strcpy(tabNameChkId[1],"MDAS_AD_REPL");
      strcpy(attrNameChkId[1],"data_typ_id");
      strcpy(tabNameChkId[2],"MDAS_AD_COLLCONT");
      strcpy(attrNameChkId[2],"data_typ_id");
      chkIdCnt = 3;
      delIdCnt = 0;
      strcpy(tabNameDelName[0],"MDAS_TD_DATA_TYP");
      strcpy(attrNameDelName[0],"data_typ_name");
      delNameCnt = 1;
      break;
    case ACTION_NAME:
      idType = 0;
      strcpy(tabNameChkId[0],"MDAS_AU_AUDIT");
      strcpy(attrNameChkId[0],"action_id");
      strcpy(tabNameChkId[1],"MDAS_AR_AUDIT");
      strcpy(attrNameChkId[1],"action_id");
      strcpy(tabNameChkId[2],"MDAS_AD_AUDIT");
      strcpy(attrNameChkId[2],"action_id");
      chkIdCnt = 3;
      delIdCnt = 0;
      strcpy(tabNameDelName[0],"MDAS_TD_ACTN");
      strcpy(attrNameDelName[0],"action_desc");
      delNameCnt = 1;
      break;
    case RESOURCE_CLASS:
      idType = 0;
      strcpy(tabNameChkId[0],"MDAS_AR_REPL");
      strcpy(attrNameChkId[0],"rsrc_class_id");
      strcpy(tabNameChkId[1],"MDAS_AR_PHYSICAL");
      strcpy(attrNameChkId[1],"rsrc_class_id");
      chkIdCnt = 2;
      delIdCnt = 0;
      strcpy(tabNameDelName[0],"MDAS_TD_RSRC_CLASS");
      strcpy(attrNameDelName[0],"rsrc_class_name");
      delNameCnt = 1;
      break;
    case RESOURCE_LOCATION_NAME:
      idType = 1;
      strcpy(tabNameChkId[0],"MDAS_AR_REPL");
      strcpy(attrNameChkId[0],"locn_id");
      strcpy(tabNameChkId[1],"MDAS_AR_PHYSICAL");
      strcpy(attrNameChkId[1],"locn_id");
      chkIdCnt = 2;
      delIdCnt = 0;
      strcpy(tabNameDelName[0],"MDAS_TD_LOCN");
      strcpy(attrNameDelName[0],"locn_desc");
      delNameCnt = 1;
      break;
      
    default:
      return (TOKEN_IDEN_TYPE_ERROR);
      break;
    }
 
  if (valueType == USER_NAME) {
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
  }
  else if (valueType == USER_GROUP_NAME) {
    sprintf(userName,"%s@groups",delValue);
    if ((tmpPtr = strstr(userName,"@")) == NULL)
      return(DATA_VALUE_TYPE_ERROR);
    *tmpPtr = '\0';
    tmpPtr++;
    strcpy(domainName,tmpPtr);
    idInt = get_user_id_in_domain(userName, domainName);
    if (idInt < 0)
      return(idInt);
    sprintf(idVal,"%i",idInt);
  }
  else {
    sprintf(sqlq,"%s.%s",tabNameDelName[0],attrNameDelName[0]);
    if (idType == 0) {
      idInt = get_itype_value(sqlq,delValue);
      if (idInt < 0)
	return (NO_VALUE_FOUND);
      sprintf(idVal,"%i",idInt);
    }
    else {
      failure = 0;
      get_ctype_value(userName, sqlq,delValue);
      if (failure != 0)
	return (NO_VALUE_FOUND);
      sprintf(idVal,"'%s'",userName);
    }
  }


  for (i = 0; i < chkIdCnt ; i++) {
    sprintf(sqlq, "select %s from %s%s where %s = %s",
	    attrNameChkId[i], MDASSCHEMENAME,tabNameChkId[i],
	    attrNameChkId[i], idVal);
    if (strlen(extraChkId[i]) > 0)
      strcat(sqlq,extraChkId[i]);
    if ( check_exists(sqlq) == 0) { /* something exists */ 
      fprintf(stderr,"Error: Value Not Removed. Value exists in %s\n",
	      tabNameChkId[i]);
      return(ACTION_NOT_ALLOWED);
    }
  }
  for (i = 0; i < delIdCnt; i++) {
    sprintf(sqlq, "delete from %s%s where %s = %s",
	    MDASSCHEMENAME,tabNameDelId[i],attrNameDelId[i], idVal);
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) {
      fprintf(stderr,"Error: Unable to delete Value from %s: %i\n",
	      tabNameDelId[i],res);
      return(res);
    }
  }
  for (i = 0; i < delNameCnt; i++) {
    sprintf(sqlq, "delete from %s%s where %s = '%s'",
	    MDASSCHEMENAME,tabNameDelName[i],attrNameDelName[i],delValue);
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) {
      fprintf(stderr,"Error: Unable to delete Value from %s: %i\n",
	      tabNameDelId[i],res);
      return(res);
    }
  }
  if (valueType == USER_NAME || valueType == USER_GROUP_NAME) {
    sprintf(sqlq, "delete from %sMDAS_AD_GRP_ACCS where data_grp_id in (select data_grp_id  from   %sMDAS_TD_DATA_GRP where data_grp_name like '%%/home/%s.%s' or data_grp_name like '%%/container/%s.%s' or data_grp_name like '%%/home/%s.%s/%%' or data_grp_name like '%%/container/%s.%s/%%')",MDASSCHEMENAME,MDASSCHEMENAME,userName,domainName,userName,domainName,userName,domainName,userName,domainName);
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) {
      fprintf(stderr,"Error: Unable to delete Value from %s for %s@%s: %i\n",
	      "MDAS_TD_DATA_GRP",userName,domainName, res);
      return(res);
    }
    sprintf(sqlq, "delete from %sMDAS_AD_COLLMDATA where data_grp_id in (select data_grp_id  from   %sMDAS_TD_DATA_GRP where data_grp_name like '%%/home/%s.%s' or data_grp_name like '%%/container/%s.%s' or data_grp_name like '%%/home/%s.%s/%%' or data_grp_name like '%%/container/%s.%s/%%')",MDASSCHEMENAME,MDASSCHEMENAME,userName,domainName,userName,domainName,userName,domainName,userName,domainName);
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) {
      fprintf(stderr,"Error: Unable to delete Value from %s for %s@%s: %i\n",
	      "MDAS_TD_DATA_GRP",userName,domainName, res);
      return(res);
    }
    sprintf(sqlq, "delete from %sMDAS_AD_COLLCONT where data_grp_id in (select data_grp_id  from   %sMDAS_TD_DATA_GRP where data_grp_name like '%%/home/%s.%s' or data_grp_name like '%%/container/%s.%s' or data_grp_name like '%%/home/%s.%s/%%' or data_grp_name like '%%/container/%s.%s/%%')",MDASSCHEMENAME,MDASSCHEMENAME,userName,domainName,userName,domainName,userName,domainName,userName,domainName);
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) {
      fprintf(stderr,"Error: Unable to delete Value from %s for %s@%s: %i\n",
	      "MDAS_TD_DATA_GRP",userName,domainName, res);
      return(res);
    }
    sprintf(sqlq, "delete from %sMDAS_TD_DATA_GRP where data_grp_name like '%%/home/%s.%s' or data_grp_name like '%%/container/%s.%s' or data_grp_name like '%%/home/%s.%s/%%' or data_grp_name like '%%/container/%s.%s/%%'",MDASSCHEMENAME,userName,domainName,userName,domainName,userName,domainName,userName,domainName);
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) {
      fprintf(stderr,"Error: Unable to delete Value from %s for %s@%s: %i\n",
	      "MDAS_TD_DATA_GRP",userName,domainName, res);
      return(res);
    }
  }
  return(MDAS_SUCCESS);
}

int
testExecSqlDb2(void *henv, void *hdbc, char *sqlq)
{
  printf("SQLQ: %s\n",sqlq);
  return(0);
}

int
testmodify_collection_info(int A, char *regUserName, char *oldCollName,
			   char *newCollName, char *B, char *C,
			   int Flag, char *regDomainName)
{
  printf("ModCollInfo: %s to %s\n",oldCollName,newCollName);
  return(0);

}

int
testcheck_exists(char *sqlq) 
{
  printf("SQLQ: %s\n",sqlq);
  return(-1);
}

