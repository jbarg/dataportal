#ifdef SRB_MDAS

#include "mdasC_db2_externs.h"
#include "srbC_mdas_externs.h"
#include "mdasGlobalsExtern.h"
#include "elogExtern.h"
#include "oraTableDvr.h"

int
extractTemplateMetadata(char *dataName, char *collName, char *templateName)
{
  char *templateString, *mybuf, *dataValue, *buffer;
  int i,j, bufSize;
  tableStateInfo *stateInfo;
  char *fileBuf, fileName[2 * MAX_TOKEN];  
  char dataTag[MAX_TAG_SIZE];

    if((stateInfo = malloc(sizeof (tableStateInfo))) == NULL) {
      elog (NOTICE, "oraTableOpen: Malloc error");
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

  i = readSrbObjIntoBufferPtr(templateName, &templateString);
  if (i < 0)    return i;
  i = splitStyleProc( templateString, stateInfo);
  free( templateString);
  if (i < 0)    return i;
  sprintf(fileName,"%s/%s",collName,dataName);
  i = readSrbObjIntoBufferPtr(fileName, &fileBuf);
  if (i < 0)    return i;
  mybuf = fileBuf;
  buffer =  fileBuf;
  bufSize = strlen(fileBuf);
  while (1) {
    j = getNextDataFromTEMPLATE(stateInfo, &mybuf, dataTag, &dataValue,
				  bufSize - (mybuf -  buffer));
    if (j != MDAS_SUCCESS )  return(j);
    printf("%s=%s\n",dataTag,dataValue);
  }

  return 0;
}

#endif /* SRB_MDAS*/


