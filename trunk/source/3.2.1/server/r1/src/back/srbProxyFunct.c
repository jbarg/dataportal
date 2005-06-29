 

/* srbObjFunct - Routines to handle Object type SID.
 */

#include "srbProxyFunct.h"
#include "dbTableDefines.h"


int
extractMetadata( int argc, char **argv, int lsock)
{


  char *templateString, *mybuf, *dataValue, *buffer;
  int i,ii,iii,j,jj, bufSize, kk;
  tableStateInfo *stateInfo;
  char *fileBuf, *metaStr;
  char dataTag[MAX_TAG_SIZE];
  char *templateObj, *srbObj, *metaObj;
  char trgObj[MAX_TOKEN],trgColl[MAX_TOKEN];
  char newVal[MAX_TOKEN];
  char minbuf[MAX_TOKEN * 3];
  char *dataValuePtr;
  char tmpChar;
  char tmpData[MAX_MDATA_SIZE+50];
  int curStrLen = 0;

  if (argc != 3 && argc != 4) 
    return (-200);
  templateObj = argv[1];
  srbObj = argv[2];
  if (argc == 4)
    metaObj = argv[3];
  else
    metaObj = argv[2];
  elog (NOTICE, " ARGC=%i\n %s %s %s %s \n", argc,argv[0],templateObj,
	srbObj,metaObj);

    if((stateInfo = malloc(sizeof (tableStateInfo))) == NULL) {
      elog (NOTICE, "extractMetadata: Malloc error");
      return DB_ERR_MALLOC;
    }
    stateInfo->mm_size = 0;
    stateInfo->num_of_cols = 0;
    stateInfo->num_of_rows_read = 0;
    stateInfo->endOfData = 0;
    stateInfo->dataTemplateH = NULL;
    stateInfo->dataTemplateB = NULL;
    stateInfo->dataTemplateT = NULL;
    stateInfo->sql = NULL;
    stateInfo->tableNameList= NULL;
    stateInfo->dataTemplateChoice.choiceCount = 0;
    stateInfo->dataTemplateChoice.roundRobin = 0;
    stateInfo->dataTemplateChoice.choiceCondition = NULL;
    stateInfo->dataTemplateChoice.compiledHeadRE[0] = NULL;
    stateInfo->dataTemplateChoice.compiledTailRE[0] = NULL;
    stateInfo->definedValues.varCount = 0;
    stateInfo->constantValues.varCount = 0;
    stateInfo->storedSQLBuffer = NULL;
    stateInfo->freestoredSQLBufferSize = 0;
    stateInfo->num_of_tables = 0;
    stateInfo->constantValues.varCount = 0;
    stateInfo->definedValues.varCount = 0;
    strcpy(stateInfo->constantValues.preString, SYSTEM_OPEN_TAG);
    strcpy(stateInfo->definedValues.preString, STANDARAD_OPEN_TAG);
    strcpy(stateInfo->sqlType,"");
    strcpy(stateInfo->dataType,"");
    stateInfo->userFuncs.funcCount = 0;
    stateInfo->compSql.sqlCount = 0;
    for (i = 0 ; i < MAX_TABLE_COLS ; i++)
      stateInfo->colName[i] =  NULL;

    i = readSrbObjIntoBufferPtr(templateObj,&templateString);
    if (i <= 0) { 
      elog (NOTICE, "Error reading template Object %s: %i\n",
	    templateObj,i); 
      return(i) ; 
    }
    i = splitStyleProc( templateString, stateInfo);
    free( templateString);
    if (i < 0)  { 
      elog (NOTICE, "Error parsing srbPage.html: %i\n",i); 
      return(i) ;
    }
    i = readSrbObjIntoBufferPtr(metaObj,&fileBuf);
    if (i <= 0) {
      elog(NOTICE, "Error reading srbsrbObject %s: %i\n",
	   metaObj,i);
      return(i) ; 
    }
    mybuf = fileBuf;
    buffer =  fileBuf;
    bufSize = strlen(fileBuf);
    splitbychar(srbObj,trgColl,trgObj, '/');
    if((metaStr = malloc(HUGH_HUGE_STRING )) == NULL) {
      elog (NOTICE, "extractMetadata: Malloc error");
      return DB_ERR_MALLOC;
    }
    curStrLen = 0;
    metaStr[0] = '\0';
    while (1) {
      j = getNextDataFromTEMPLATE(stateInfo, &mybuf, dataTag, &dataValue,
				  bufSize - (mybuf -  buffer));
      if (j != MDAS_SUCCESS ) {
	if (curStrLen > 0) {
	  kk =  _svrModifyDataset(MDAS_CATALOG,trgObj,trgColl,"","",
				  metaStr,"|",D_BULK_INSERT_UDEF_META_DATA_FOR_DATA);
	  if (kk < 0) {
	    free(metaStr);
	    return(kk);
	  }
	}
	free(metaStr);
	if (j == RE_MATCH_NOT_FOUND)
	  return(MDAS_SUCCESS);
	elog(NOTICE, "Error in getNextDataFromTEMPLATE: %i\n",j);
	return(j);
      }
      while (isspace(*dataValue)) dataValue++;
      make_quote_to_two_quote(dataValue,tmpData);
      dataValuePtr = tmpData;
      while ((jj= strlen(dataValuePtr)) > 0) {
	if (jj > MAX_MDATA_SIZE) {
	  tmpChar = *(dataValuePtr+MAX_MDATA_SIZE);
	  *(dataValuePtr+MAX_MDATA_SIZE) = '\0';
	}
	/*** RAJA June 24 2004 changing to bulk load instead of single load ***
	iii = _svrModifyDataset(MDAS_CATALOG, trgObj,trgColl, "","",
				"0",dataTag,
				D_INSERT_USER_DEFINED_STRING_META_DATA);
	
	if (iii < 0) return (iii);
	sprintf(newVal,"1@%i",iii);
	ii = _svrModifyDataset(MDAS_CATALOG, trgObj,trgColl, "","",
			       newVal,dataValuePtr,
			       D_CHANGE_USER_DEFINED_STRING_META_DATA);
	if (ii < 0) return (ii);
	*** RAJA June 24 2004 changing to bulk load instead of single load ***/
	sprintf(minbuf,"%s|%s\n", dataTag,dataValuePtr);
	curStrLen += strlen(minbuf);
	if (curStrLen > HUGH_HUGE_STRING) {
	  kk = _svrModifyDataset(MDAS_CATALOG,trgObj,trgColl,"","",
				 metaStr,"|",D_BULK_INSERT_UDEF_META_DATA_FOR_DATA);
	  if (kk < 0) {
	    free(metaStr);
	    return(kk);
	  }
	  metaStr[0] = '\0';
	  curStrLen = strlen(minbuf);
	}
	strcat(metaStr,minbuf);
	if (jj > MAX_MDATA_SIZE) {
	  *(dataValuePtr+MAX_MDATA_SIZE) = tmpChar;
	  dataValuePtr += MAX_MDATA_SIZE;
	}
	else
	  break;
      }
    }
    return(0);
}
