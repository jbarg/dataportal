/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/*
 * TableSidMisc.c - Misc routines for the srbServer and srbMaster TableSID.
 */

#ifdef REFACTOR
#if defined(PORTNAME_solaris)
#include <ctype.h>
#endif

#include "dbTableDefines.h"
#include "dbTableExtern.h"
#include "srbObjFunctExtern.h"
#endif  /* REFACTOR */
#include "serverAll.h"

char *emptyStr = "";

extern char *skiptochar( char *inStr, char skipChar);
extern char *evaluateArithmeticStringExpression(char *inExpr);



char *
getPosOfChar(char *inStr, char qC)
{
  int inQuote;
  int inDblQuote;

  if (*inStr == '\0') return(NULL);
  inQuote = 0;
  inDblQuote = 0;
  
  while (*inStr != '\0') {
    if (inQuote) {
      if (*inStr == '\'' && *(inStr+1) !=  '\'') inQuote = 0;
      else if (*inStr == '\'' && *(inStr+1) ==  '\'') inStr++;
    }
    else if (inDblQuote) {
      if (*inStr == '\"' && *(inStr+1) !=  '\"') inDblQuote = 0;
      else if (*inStr == '\"' && *(inStr+1) ==  '\"') inStr++;
    }
    else {
      if (*inStr == '\'') inQuote = 1;
      else if (*inStr == '\"') inDblQuote = 1;
      else if (*inStr == qC)
	return(inStr);
    }
    inStr++;
  }
  return(inStr);
}

char *
getEndOfValue(char *inStr) 
{
  return(getPosOfChar(inStr, ','));
}

char *
getEndOfStatement(char *inStr)
{
  return(getPosOfChar(inStr, ';'));
}

char *replaceBlankByUnderscore(char *inStr, char *outStr)
{
  int i;

  for (i = 0; i <= strlen(inStr) ; i++) {
    if(isspace(inStr[i]))
      outStr[i] = '_';
    else
      outStr[i] = inStr[i];
  }
  return(outStr);
}

int
convCtrlCharsToBlanks(char *inStr)
{
  int inQuote;
  int inDblQuote;
  inQuote = 0;
  inDblQuote = 0;
  while (*inStr != '\0') {
    if (inQuote) {
      if (*inStr == '\'' && *(inStr+1) !=  '\'') inQuote = 0;
      else if (*inStr == '\'' && *(inStr+1) ==  '\'') inStr++;
    }
    else if (inDblQuote) {
      if (*inStr == '\"' && *(inStr+1) !=  '\"') inDblQuote = 0;
      else if (*inStr == '\"' && *(inStr+1) ==  '\"') inStr++;
    }
    else {
      if (*inStr == '\'') inQuote = 1;
      else if (*inStr == '\"') inDblQuote = 1;
      else if (isspace(*inStr)) *inStr = ' ';
    }
    inStr++;
  }
  return (0);
}

char *
skiptoelse(char * inStr)
{
  int depth;
  char *tmpPtr, *tmpPtr2, *tmpPtr3, *tmpPtr4;

  tmpPtr4 =  inStr;
  
  while (tmpPtr4) {
    if ((tmpPtr = strstr(tmpPtr4,"<TLELSE>")) == NULL) 
      return(NULL);
    if (((tmpPtr2 = strstr(tmpPtr4,"<TLIF>")) == NULL) ||
	(tmpPtr  < tmpPtr2))
      return (tmpPtr);
    depth = 1;
    tmpPtr2 += strlen("<TLIF>");
    while (depth > 0) {
      if ((tmpPtr = strstr(tmpPtr2,"</TLIF>")) == NULL) 
	return(strstr(tmpPtr2,"<TLELSE>"));
      if (((tmpPtr3 = strstr(tmpPtr2,"<TLIF>")) == NULL) ||
	  (tmpPtr  < tmpPtr3)) {
	depth--;
	tmpPtr2 = tmpPtr+strlen("</TLIF>");
      }
      else {
	depth++;
	tmpPtr2 = tmpPtr3+strlen("<TLIF>");
      }
    }
    tmpPtr4 = tmpPtr2;
  }
  return (0);
}

char *
skiptoforend(char * inStr)
{
  int depth;
  char *tmpPtr, *tmpPtr2, *tmpPtr3, *tmpPtr4;

  tmpPtr4 =  inStr;
  
  while (tmpPtr4) {
    if ((tmpPtr = strstr(tmpPtr4,"</TLFOR>")) == NULL) 
      return(NULL);
    if (((tmpPtr2 = strstr(tmpPtr4,"<TLFOR>")) == NULL) ||
	(tmpPtr  < tmpPtr2))
      return (tmpPtr);
    depth = 1;
    tmpPtr2 += strlen("<TLFOR>");
    while (depth > 0) {
      if ((tmpPtr = strstr(tmpPtr2,"</TLFOR>")) == NULL) 
	return(NULL);
      if (((tmpPtr3 = strstr(tmpPtr2,"<TLFOR>")) == NULL) ||
	  (tmpPtr  < tmpPtr3)) {
	depth--;
	tmpPtr2 = tmpPtr+strlen("</TLFOR>");
      }
      else {
	depth++;
	tmpPtr2 = tmpPtr3+strlen("<TLFOR>");
      }
    }
    tmpPtr4 = tmpPtr2;
  }
  return (0);
}

char *
skiptoendif(char * inStr)
{
  int depth;
  char *tmpPtr, *tmpPtr2, *tmpPtr3, *tmpPtr4;

  tmpPtr4 =  inStr;
  
  while (tmpPtr4) {
    if ((tmpPtr = strstr(tmpPtr4,"</TLIF>")) == NULL) 
      return(NULL);
    if (((tmpPtr2 = strstr(tmpPtr4,"<TLIF>")) == NULL) ||
	(tmpPtr  < tmpPtr2))
      return (tmpPtr);
    depth = 1;
    tmpPtr2 += strlen("<TLIF>");
    while (depth > 0) {
      if ((tmpPtr = strstr(tmpPtr2,"</TLIF>")) == NULL) 
	return(NULL);
      if (((tmpPtr3 = strstr(tmpPtr2,"<TLIF>")) == NULL) ||
	  (tmpPtr  < tmpPtr3)) {
	depth--;
	tmpPtr2 = tmpPtr+strlen("</TLIF>");
      }
      else {
	depth++;
	tmpPtr2 = tmpPtr3+strlen("<TLIF>");
      }
    }
    tmpPtr4 = tmpPtr2;
  }
  return (0);
}
int
fillInIVarValue(char *bodyStr,char *fillStr,
		char IVarName[][VAR_NAME_SIZE], 
		char IVarVal[][VAR_VAL_SIZE], int IVarCnt)
{
  char *tmpPtr;
  char *tmpPtr2;
  int i;
  char tmpStr[HUGH_HUGE_STRING];

  if (IVarCnt == 0) {
    strcpy(fillStr,bodyStr);
    return(0);
  }
  tmpPtr = bodyStr;
  for (i = 0 ; i < IVarCnt ; i++) {
    fillStr[0] = '\0';
    while( (tmpPtr2 = strstr(tmpPtr,IVarName[i])) !=NULL) {
      *tmpPtr2 = '\0';
      strcat(fillStr,tmpPtr);
      *tmpPtr2 = IVarName[i][0];
      sprintf (&fillStr[strlen(fillStr)],"%s",IVarVal[i]);
      tmpPtr = tmpPtr2 + strlen(IVarName[i]);
    }
    strcat(fillStr,tmpPtr);
    strcpy(tmpStr,fillStr);
    tmpPtr = tmpStr;
  }
  return(0);
}

int
processIncludeObjectsInFilledStyleSheet(char *buffer)
{
  int i;
  char *bufPtr, *tmpPtr, *tmpPtr2;
  char fillStr[HUGH_HUGE_STRING];
  char *inBuffer;

  while ((tmpPtr = strstr(buffer,"<TLINCLUDEOBJ>")) != NULL) {
    bufPtr =  buffer;
    fillStr[0] = '\0';
    while ( (tmpPtr = strstr(bufPtr,"<TLINCLUDEOBJ>")) != NULL) {
      *tmpPtr = '\0';
      tmpPtr += strlen("<TLINCLUDEOBJ>");
      strcat(fillStr,bufPtr);
      if ((tmpPtr2 = strstr(tmpPtr,"</TLINCLUDEOBJ>")) == NULL) return(DATA_TAG_ERROR);
      *tmpPtr2 = '\0';
      tmpPtr2 += strlen("</TLINCLUDEOBJ>");
      i = readSrbObjIntoBufferPtr(tmpPtr, &inBuffer);
      if (i < 0) return(i);
      if (i >=0) {
	strcat(fillStr,inBuffer);
	free(inBuffer);
      }
      bufPtr = tmpPtr2;
    }
    if (strlen(fillStr) > 0) {
      strcpy(buffer,fillStr);
      strcat(buffer,bufPtr);
    }
  }
  return(MDAS_SUCCESS);
}

int
processEvalExpressionsInFilledStyleSheet(char *buffer)
{
  char *bufPtr, *tmpPtr, *tmpPtr2;
  char fillStr[HUGH_HUGE_STRING];
  char *evalPtr;

  while ((tmpPtr = strstr(buffer,"<TLEVAL>")) != NULL) {
    bufPtr =  buffer;
    fillStr[0] = '\0';
    while ( (tmpPtr = strstr(bufPtr,"<TLEVAL>")) != NULL) {
      *tmpPtr = '\0';
      tmpPtr += strlen("<TLEVAL>");
      strcat(fillStr,bufPtr);
      if ((tmpPtr2 = strstr(tmpPtr,"</TLEVAL>")) == NULL) return(DATA_TAG_ERROR);
      *tmpPtr2 = '\0';
      evalPtr = (char *) evaluateArithmeticStringExpression(tmpPtr);
      tmpPtr2 += strlen("</TLEVAL>");
      strcat(fillStr,evalPtr);
      free(evalPtr);
      bufPtr = tmpPtr2;
    }
    if (strlen(fillStr) > 0) {
      strcpy(buffer,fillStr);
      strcat(buffer,bufPtr);
    }
  }
  return(MDAS_SUCCESS);
}


int
processFuncClassInBuffer(tableStateInfo *stateInfo, 
			 char *inBuffer, char **retPtr)
{
  int i,cnt, changeFlag, localChangeFlag;
  char *funPtr, *bufPtr, *tmpPtr, *tmpPtr3, *tmpPtr4 , *tmpPtr5;
  char *funcName;
  char *funcArg;
  char *funcParam;
  char buffer[HUGH_HUGE_STRING];
  char buffer2[HUGH_HUGE_STRING];
  char assignStr[HUGE_STRING];
  bufPtr = inBuffer;
  changeFlag = 0;
  strcpy(buffer,"");
  while (1) {
    localChangeFlag = 0;
    while ((funPtr = strstr(bufPtr,"<TLFUNCCALL>")) != NULL) {
      if ((tmpPtr3 = strstr(funPtr,"</TLFUNCCALL>")) == NULL) 
	return(DATA_TAG_ERROR);
      changeFlag = 1;
      localChangeFlag = 1;
      *tmpPtr3 = '\0';
      tmpPtr = funPtr;
      funPtr = '\0';
      tmpPtr += strlen("<TLFUNCCALL>");
      if ((tmpPtr4 = strstr(tmpPtr,"("))  == NULL) {
	funcName = tmpPtr;
	funcArg = emptyStr;
	trimsides(funcName);
      }
      else {
	if ((tmpPtr5 = strstr(tmpPtr4,")"))  == NULL) 
	  return(DATA_TAG_ERROR);
	*tmpPtr4 ='\0';
	*tmpPtr5 ='\0';
	funcName = tmpPtr;
	funcArg = tmpPtr4 + 1;
	trimsides(funcName);
	trimsides(funcArg);
      }
      for (i = 0; i < stateInfo->userFuncs.funcCount ; i++) {
	if (!strcmp(stateInfo->userFuncs.funcName[i], funcName))
	  break;
      }
      if (i == stateInfo->userFuncs.funcCount)
	return (TEMPLATE_FUNCTION_NOT_FOUND);
      funcParam = stateInfo->userFuncs.funcArgList[i];
      if (!strcmp(funcArg,"") || !strcmp(funcParam,"") )
	strcpy(assignStr,"");
      else {
	  strcat(buffer, "<TLASSIGN>");
	  cnt = 0;
	  while ((tmpPtr4 = skiptochar(funcArg,',')) != NULL) {
	    *tmpPtr4 = '\0';
	    if ((tmpPtr5  = skiptochar(funcParam,',')) == NULL)
	      return(FUNCTION_CALL_PARAM_ERROR);
	    *tmpPtr5 = '\0';
	    trimsides(funcArg);
	    trimsides(funcParam);
	    if (cnt >= 1) strcat(buffer, ",");
	    strcat(buffer, funcParam);
	    strcat(buffer, "=");
	    strcat(buffer, funcArg);
	    funcArg = tmpPtr4 + 1;
	    funcParam = tmpPtr5 + 1;
	    cnt++;
	  }
	  if (skiptochar(funcParam,',') != NULL)
	     return(FUNCTION_CALL_PARAM_ERROR);
	  trimsides(funcArg);
	  trimsides(funcParam);
	  if (cnt >= 1) strcat(buffer, ",");
	  strcat(buffer, funcParam);
	  strcat(buffer, "=");
	  strcat(buffer, funcArg);
      }
      strcat(buffer, bufPtr);
      strcat(buffer, assignStr);
      bufPtr = tmpPtr3 + strlen("</TLFUNCCALL>");
    }
    if (localChangeFlag) {
      strcpy(buffer2, buffer);
      strcat(buffer2, bufPtr);
      bufPtr = buffer2;
    }
    else {
      break;
    }
  }
  if (changeFlag) {
    *retPtr = strdup(bufPtr);
    if (*retPtr == NULL) return (MEMORY_ALLOCATION_ERROR);
  }
  return(changeFlag);
}


int
processForInBuffer(tableStateInfo *stateInfo, char *inBuffer, char **retPtr)
{
  /***stateInfo->num_of_cols, stateInfo->colName,dimen,
      stateInfo->dataTemplateH, ***/

  int whFlag, changeFlag;
  char *bufPtr, *tmpPtr, *tmpPtr2, *tmpPtr3;
  char fillStr[HUGH_HUGE_STRING];
  char *fillStrPtr;
  char condStr[HUGE_STRING], bodyStr[HUGE_STRING];
  char buffer[HUGH_HUGE_STRING];
  char IVarName[MAX_VAR_COUNT][VAR_NAME_SIZE];
  char IVarVal[MAX_VAR_COUNT][VAR_VAL_SIZE];
  int IVarCnt;

  bufPtr = inBuffer;
  strcpy(buffer,bufPtr);
  whFlag = 1;
  changeFlag = 0;
  while (whFlag && (tmpPtr = strstr(buffer,"<TLFOR>")) != NULL) {
    whFlag = 0;
    bufPtr =  buffer;
    fillStr[0] = '\0';
    while ( (tmpPtr = strstr(bufPtr,"<TLFOR>")) != NULL) {
      *tmpPtr = '\0';
      strcat(fillStr,bufPtr);
      *tmpPtr = '<';
      tmpPtr += strlen("<TLFOR>");
      if ((tmpPtr2 = strstr(tmpPtr,"<TLFORBODY>")) == NULL) return(DATA_TAG_ERROR);
      *tmpPtr2 = '\0';
      if (evaluableCond(tmpPtr)) {
	whFlag = 1;
	changeFlag = 1;
	strcpy(condStr,tmpPtr);
	*tmpPtr2 = '<';
	tmpPtr2 += strlen("<TLFORBODY>");
	if ((tmpPtr3 = skiptoforend(tmpPtr2)) == NULL) return(DATA_TAG_ERROR);
	*tmpPtr3 = '\0';
	strcpy(bodyStr,tmpPtr2);
	*tmpPtr3 = '<';
	bufPtr = tmpPtr3 +strlen("</TLFOR>");
	while (evaluateForCondition(condStr,IVarName, IVarVal, &IVarCnt)== 1) {
	  fillStrPtr = fillStr +  + strlen(fillStr);
	  fillInIVarValue(bodyStr,fillStrPtr,IVarName, IVarVal,IVarCnt);
	}
      }
      else {
	strcat(fillStr,tmpPtr - strlen("<TLFOR>"));
	*tmpPtr2 = '<';
	bufPtr = tmpPtr2;
      }
    }
    if (strlen(fillStr) > 0) {
      strcpy(buffer,fillStr);
      strcat(buffer,bufPtr);
    }
  }
  if (changeFlag) {
    *retPtr = strdup(buffer);
    if (*retPtr == NULL) return (MEMORY_ALLOCATION_ERROR);
  }
  return(changeFlag);
}

int evalAndStoreAssignValues(tableStateInfo *stateInfo, char *inAssStr)
{
  int i,j;
  char *tmpPtr2;
  char *tmpPtr3;
  char *assStr, *retStr, *evalPtr;

  assStr = inAssStr;
  while ((tmpPtr3 = skiptochar(assStr,',')) != NULL) {
    *tmpPtr3 = '\0';
    if ((tmpPtr2  = skiptochar(assStr,'=')) == NULL)
      return(EXPRESSION_SYNTAX_ERROR);
    *tmpPtr2 = '\0';
    tmpPtr2++;
    trimsides(assStr);
    trimsides(tmpPtr2);
    i = processAllVarsInBuffer (stateInfo, tmpPtr2, &retStr);
    if (i < 0) return(i);
    evalPtr = evaluateArithmeticStringExpression(retStr);
    if (evalPtr == NULL) return(EXPRESSION_SYNTAX_ERROR);
    j = updateVarList(&(stateInfo->definedValues), assStr, evalPtr, "UNKNOWN");
    if (j < 0) return(j);
    free(evalPtr);
    if (i == 1) free(retStr);
    assStr = tmpPtr3+1;
  }
  if ((tmpPtr2  = skiptochar(assStr,'=')) == NULL)
      return(EXPRESSION_SYNTAX_ERROR);
  *tmpPtr2 = '\0';
  tmpPtr2++;
  trimsides(assStr);
  trimsides(tmpPtr2);
  i = processAllVarsInBuffer (stateInfo, tmpPtr2, &retStr);
  if (i < 0) return(i);
  evalPtr = evaluateArithmeticStringExpression(retStr);
  j = updateVarList(&(stateInfo->definedValues), assStr, evalPtr, "UNKNOWN");
  if (j < 0) return(j);
  free(evalPtr);
  if (i == 1) free(retStr);
  return(MDAS_SUCCESS);

}

int 
processAssignmentBlocksInBuffer(tableStateInfo *stateInfo, 
				    char *givenForm)
{
  
  char *tmpPtr, *tmpPtr2, *tmpPtr3;

  tmpPtr3 = givenForm;
  while (1) {
    if ((tmpPtr = strstr(tmpPtr3, "<TLASSIGN>")) == NULL)
      break; 
    tmpPtr += strlen("<TLASSIGN>");
    if ((tmpPtr2 = strstr(tmpPtr, "</TLASSIGN>")) == NULL)
      return (DATA_TAG_ERROR);
    *tmpPtr2 = '\0';
    i = evalAndStoreAssignValues(stateInfo, tmpPtr);
    if (i < 0) return(i);
    tmpPtr2 += strlen("</TLASSIGN>");
    tmpPtr -= strlen("<TLASSIGN>");
    memcpy(tmpPtr, tmpPtr2, strlen(tmpPtr2)+1);
    tmpPtr3 = tmpPtr;
  }
  return(MDAS_SUCCESS);
}

int processVarsListInBuffer (varsList *listName, 
			     char *givenForm, char **retForm) 
{
  /***
      %%%NC - NumOfColumns
      %%%NT - NumOfTables
      %%%ND - NumOfUserDefinedValues
      %%%QQ - Query
      %%%TL - TableNameList
      %%%DT - dataType
      %%%QT - queryType
      %%%TF - TemplateFile
  ***/

  int i, changeFlag, localChangeFlag;
  char *tmpPtr, *tmpPtr2;
  char inForm[MAX_TEMPLATE_SIZE];
  char outForm[MAX_TEMPLATE_SIZE];
  char constName[MAX_VARNAME_SIZE];

  changeFlag = 0;
  if (strstr(givenForm, listName->preString) == NULL)
    return (changeFlag);
  localChangeFlag = 0;
  memcpy(inForm,givenForm, strlen(givenForm)+1);
  tmpPtr = inForm;
  strcpy(outForm,"");

  for (i = 0; i < listName->varCount ; i++) {
    sprintf(constName, "%s%s:",listName->preString, listName->varName[i]);
    while( (tmpPtr2 = strstr(tmpPtr,constName)) !=NULL) {
      *tmpPtr2 = '\0';
      strcat(outForm,tmpPtr);
      *tmpPtr2 = listName->preString[0];
      sprintf (&outForm[strlen(outForm)],"%s",listName->varValue[i]);
      tmpPtr = tmpPtr2 + strlen(constName);
      changeFlag = 1;
      localChangeFlag = 1;
    }
    if (localChangeFlag) {
      strcat(outForm,tmpPtr);
      memcpy(inForm,outForm,strlen(outForm) + 1);
      localChangeFlag = 0;
      tmpPtr = inForm;
      strcpy(outForm,"");
    }
  }
  if (changeFlag) {
    *retForm = strdup(tmpPtr);
    if (*retForm == NULL) return (MEMORY_ALLOCATION_ERROR);
  }
  return(changeFlag);
}



int processAllVarsInBuffer (tableStateInfo *stateInfo, char *givenForm,
			    char **retForm)
{

  char *bufPtr;
  int i; 
  int changed = 0;
  char *inStr;
  
  inStr = givenForm;
  if ((i = processVarsListInBuffer(&(stateInfo->constantValues),
				   inStr, &bufPtr)) == 1) {
    inStr = bufPtr;
    changed = 1;
  }
  else if (i < 0) return(i);
  if ((i = processVarsListInBuffer(&(stateInfo->definedValues),
				   inStr, &bufPtr)) == 1) {
    if (changed) free(inStr);
    inStr = bufPtr;
  }
  else if (i < 0) return(i);
  *retForm = inStr;
  return(changed);
}


int processVarsListInStyleSheet(tableStateInfo *stateInfo, varsList *listName)
{

  char *bufPtr;
  int i; 
  
  if ((i = processVarsListInBuffer(listName, stateInfo->dataTemplateH, &bufPtr)) == 1) {
    free(stateInfo->dataTemplateH);
    stateInfo->dataTemplateH = bufPtr;
  }
  else if (i < 0) return(i);

  if ((i = processVarsListInBuffer(listName, stateInfo->dataTemplateT, &bufPtr)) == 1) {
  free(stateInfo->dataTemplateT);
  stateInfo->dataTemplateT = bufPtr;
  }
  else if (i < 0) return(i);
  if ((i = processVarsListInBuffer(listName, stateInfo->dataTemplateB, &bufPtr)) == 1) {
  free(stateInfo->dataTemplateB);
  stateInfo->dataTemplateB = bufPtr;
  }
  else if (i < 0) return(i);
  return(MDAS_SUCCESS);
}



int
processForInStyleSheet(tableStateInfo *stateInfo)
{

  char *bufPtr;
  int i; 
  
  if ((i = processForInBuffer(stateInfo, stateInfo->dataTemplateH, &bufPtr)) == 1) {
    free(stateInfo->dataTemplateH);
    stateInfo->dataTemplateH = bufPtr;
  }
  else if (i < 0) return(i);
  if ((i = processForInBuffer(stateInfo, stateInfo->dataTemplateT, &bufPtr)) == 1) {
  free(stateInfo->dataTemplateT);
  stateInfo->dataTemplateT = bufPtr;
  }
  else if (i < 0) return(i);
  if ((i = processForInBuffer(stateInfo, stateInfo->dataTemplateB, &bufPtr)) == 1) {
  free(stateInfo->dataTemplateB);
  stateInfo->dataTemplateB = bufPtr;
  }
  else if (i < 0) return(i);
  return(MDAS_SUCCESS);

}


int
processIfConditionsInFilledStyleSheet(char *buffer)
{
  int i;
  char *bufPtr, *tmpPtr, *tmpPtr2, *tmpPtr3, *tmpPtr4 ;
  char fillStr[HUGH_HUGE_STRING];

  

  while ((tmpPtr = strstr(buffer,"<TLIF>")) != NULL) {
    bufPtr =  buffer;
    fillStr[0] = '\0';
    while ( (tmpPtr = strstr(bufPtr,"<TLIF>")) != NULL) {
      *tmpPtr = '\0';
      tmpPtr += strlen("<TLIF>");
      strcat(fillStr,bufPtr);
      if ((tmpPtr2 = strstr(tmpPtr,"<TLTHEN>")) == NULL) return(DATA_TAG_ERROR);
      *tmpPtr2 = '\0';
      i = evaluateLogicalExpression(tmpPtr);
      tmpPtr2 += strlen("<TLTHEN>");
      if ((tmpPtr3 = skiptoelse(tmpPtr2)) == NULL) {
	if ((tmpPtr4 = skiptoendif(tmpPtr2)) == NULL) return(DATA_TAG_ERROR);
	*tmpPtr4 = '\0';
	if (i == 1) strcat(fillStr,tmpPtr2);
	bufPtr = tmpPtr4 +strlen("</TLIF>");
      }
      else {
	*tmpPtr3 = '\0';
	tmpPtr3 += strlen("<TLELSE>");
	if ((tmpPtr4 = skiptoendif(tmpPtr3)) == NULL) return(DATA_TAG_ERROR);
	*tmpPtr4 = '\0';
	if (i == 1) 
	  strcat(fillStr,tmpPtr2);
	else if (i == 0) 
	  strcat(fillStr,tmpPtr3);
	bufPtr = tmpPtr4 +strlen("</TLIF>");
      }

    }
    if (strlen(fillStr) > 0) {
      strcpy(buffer,fillStr);
      strcat(buffer,bufPtr);
    }
  }
  return(MDAS_SUCCESS);

}
int
readSrbObjIntoBufferPtr(char *objName, char **bufPtr)
{

  char *dataName;
  char *collectionName;
  int i,j, nbytes, fd;
  char *buf[MAX_NUM_SVR_OBJ_READ_BUFFERS] ;
  char *storPtr;

  dataName =  malloc (strlen (objName) + 1);
  collectionName =  malloc (strlen (objName) + 1);
  if (dataName == NULL || collectionName == NULL)
    return(MEMORY_ALLOCATION_ERROR);
  splitbychar(objName,collectionName,dataName, '/');

  fd = _svrObjOpen(dataName, O_RDONLY, collectionName);
  free(dataName);
  free(collectionName);
  if (fd < 0) return (SRB_OBJ_OPEN_ERROR);
  i = 0;
  buf[i] =  malloc (SVR_OBJ_READ_BUFFER_SIZE);
  if (buf[i] == NULL)
    return(MEMORY_ALLOCATION_ERROR);
  while ((nbytes = _svrObjRead (fd , buf[i], SVR_OBJ_READ_BUFFER_SIZE)) >= 0) {
    if (nbytes < SVR_OBJ_READ_BUFFER_SIZE) break;
    if (i < (MAX_NUM_SVR_OBJ_READ_BUFFERS - 1 )) {
      i++;
      buf[i] =  malloc (SVR_OBJ_READ_BUFFER_SIZE);
      if (buf[i] == NULL)
	return(MEMORY_ALLOCATION_ERROR);
     
    }
    else {
      for (j = 0; j <= i ; j++) {
	free(buf[j]);
      }
      return(SVR_OBJ_READ_BUFFER_OVERFLOW);
    }
  }
  if (nbytes < 0) {
    for (j = 0; j <= i ; j++) {
      free(buf[j]);
    }
    return(nbytes);
  }
  storPtr = malloc ((SVR_OBJ_READ_BUFFER_SIZE * i) + nbytes);
  if (storPtr == NULL) return (MEMORY_ALLOCATION_ERROR);
  *bufPtr = storPtr;
  for (j = 0; j < i ; j++) {
    memcpy(storPtr, buf[j],SVR_OBJ_READ_BUFFER_SIZE);
    storPtr += SVR_OBJ_READ_BUFFER_SIZE;
    free(buf[j]);
  }
  memcpy(storPtr, buf[i],nbytes);
  free(buf[i]);
  return ((SVR_OBJ_READ_BUFFER_SIZE * i) + nbytes);

}



char *
escape_for_browser(char *instr)
{
  char *tmpS;
  int i,j,l;

  l =  strlen(instr);
  tmpS = (char *) malloc(sizeof(char) * l * 2);
  if (tmpS == NULL)
    {
     return(NULL);
    }
  j= 0;
  for (i = 0; i < l; i++){
    if (instr[i] == '"') {
      sprintf(&tmpS[j],"&quot;");
      j += 6;
    }
    else if (instr[i] == '&') {
      sprintf(&tmpS[j],"&amp;");
      j += 5;
    }
    else if (instr[i] == '<') {
      sprintf(&tmpS[j],"&lt;");
      j += 4;
    }
    else if (instr[i] == '>') {
      sprintf(&tmpS[j],"&gt;");
      j += 4;
    }
    else {
      tmpS[j++] = instr[i];
    }
  }
  tmpS[j] ='\0';
  return(tmpS);
}
char *
escape_url(char *instr)
{
  char *tmpS;
  int i,j,l;

  l =  strlen(instr);
  tmpS = (char *) malloc(sizeof(char) * l * 2);
  if (tmpS == NULL)    {
    return(NULL);
  }
  j= 0;
  for (i = 0; i < l; i++){
    if (instr[i] == ' ' ||
	instr[i] == '<' ||
	instr[i] == '>' ||
	instr[i] == '%' ||
	instr[i] == '&' ||
	instr[i] == '@' ||
	instr[i] == '\?' ||
	instr[i] == '\'' ||
	instr[i] == '\"' 
	) { 
      tmpS[j++] = '%';
      sprintf(&tmpS[j],"%X", instr[i]);
      j += 2;
    }
    else {
      tmpS[j++] = instr[i];
    }
  }
  tmpS[j] ='\0';
  return(tmpS);
}


int
mapRowIntoStyleSheet(tableStateInfo *stateInfo, char *inbuffer)
{
  int i,j,ii;
  char *rowValues[MAX_TABLE_COLS];
  char *lastValues[MAX_TABLE_COLS];
  char *dimen[MAX_TABLE_COLS];
  char *buffer;
  char emptyStr[2];
  char *bufPtr, *bodyBuffer;

  emptyStr[0] = '\0';
  buffer = inbuffer;
  buffer[0] = '\0';
  
  /*
  if (stateInfo->endOfData != 1 && stateInfo->num_of_rows_read == 1) {
  */
  if ((stateInfo->endOfData == 0 && stateInfo->num_of_rows_read == 1) ||
      (stateInfo->endOfData == 1 && stateInfo->num_of_rows_read == 0) ) {

    if ((i = processVarsListInBuffer(&(stateInfo->constantValues), 
				     stateInfo->dataTemplateH, &bufPtr)) == 1) {
      free(stateInfo->dataTemplateH);
      stateInfo->dataTemplateH = bufPtr;
    }
    else if (i < 0)      return(i);

    i = processAssignmentBlocksInBuffer(stateInfo, stateInfo->dataTemplateH);
    if (i < 0)      return(i);
    
    if ((i = processVarsListInBuffer(&(stateInfo->definedValues), 
				     stateInfo->dataTemplateH, &bufPtr)) == 1) {
      free(stateInfo->dataTemplateH);
      stateInfo->dataTemplateH = bufPtr;
    }
    else if (i < 0)      return(i);
    if ((i = processForInBuffer(stateInfo, stateInfo->dataTemplateH, &bufPtr)) == 1) {
      free(stateInfo->dataTemplateH);
      stateInfo->dataTemplateH = bufPtr;
    }
    else if (i < 0)      return(i);

    i = subStyleVarsWithValuesforHead(stateInfo->num_of_cols, 
	stateInfo->colName,dimen,
	stateInfo->dataTemplateH,buffer, 0, 0, NULL, NULL);
    if (i < 0)      return(i);
    buffer = buffer + strlen(buffer);
  }

  if (stateInfo->endOfData != 1) {
    for (j =  0; j < stateInfo->num_of_cols ; j++) {
      rowValues[j] = escape_for_browser(stateInfo->data[j]);
      if (rowValues[j] == NULL) return(MEMORY_ALLOCATION_ERROR);
      dimen[j] = emptyStr;
      if (stateInfo->lastData[j] == NULL) 
	lastValues[j] = emptyStr;
      else
	lastValues[j] = stateInfo->lastData[j];
    }
    bodyBuffer = strdup(stateInfo->dataTemplateB);
    if ((i = processVarsListInBuffer(&(stateInfo->constantValues),
				     bodyBuffer,&bufPtr)) == 1) {
      free(bodyBuffer);
      bodyBuffer = bufPtr;
    }
    else if (i < 0)   { free(bodyBuffer);   return(i);}

    i = processAssignmentBlocksInBuffer(stateInfo, bodyBuffer);
    if (i < 0)      { free(bodyBuffer);   return(i);}

    if ((i = processVarsListInBuffer(&(stateInfo->definedValues),
				     bodyBuffer,&bufPtr)) == 1) {
      free(bodyBuffer);
      bodyBuffer = bufPtr;
    }
    else if (i < 0)   { free(bodyBuffer);   return(i);}
    if ((i = processForInBuffer(stateInfo, bodyBuffer, &bufPtr)) == 1) {
      free(bodyBuffer);
      bodyBuffer = bufPtr;
    }
    else if (i < 0)   { free(bodyBuffer);   return(i);}
    i = subStyleVarsWithValuesforBody(stateInfo->num_of_cols, 
				  stateInfo->num_of_rows_read,
				  rowValues,
				  lastValues,dimen,
				  stateInfo->colName,
				  bodyBuffer,buffer);

    if (i < 0)      { free(bodyBuffer);   return(i);}
    for (ii =  0; ii < stateInfo->num_of_cols ; ii++) {
      if (stateInfo->lastData[ii] == NULL) {
	stateInfo->lastData[ii] = strdup(rowValues[ii]);
      }
      else if (strcmp(stateInfo->lastData[ii],rowValues[ii])) {
	free(stateInfo->lastData[ii]);
	stateInfo->lastData[ii] = strdup(rowValues[ii]);
      }
    }
    for (j =  0; j < stateInfo->num_of_cols ; j++) {
      free(rowValues[j]); 
    }
    free(bodyBuffer); 
    
  }
  else {
    if ((i = processVarsListInBuffer(&(stateInfo->constantValues), 
				     stateInfo->dataTemplateT, &bufPtr)) == 1) {
      free(stateInfo->dataTemplateT);
      stateInfo->dataTemplateT = bufPtr;
    }
    else if (i < 0)      return(i);

    i = processAssignmentBlocksInBuffer(stateInfo, stateInfo->dataTemplateT);
    if (i < 0)      return(i);

    if ((i = processVarsListInBuffer(&(stateInfo->definedValues), 
				     stateInfo->dataTemplateT, &bufPtr)) == 1) {
      free(stateInfo->dataTemplateT);
      stateInfo->dataTemplateT = bufPtr;
    }
    else if (i < 0)      return(i);
    if ((i = processForInBuffer(stateInfo, stateInfo->dataTemplateT, &bufPtr)) == 1) {
      free(stateInfo->dataTemplateT);
      stateInfo->dataTemplateT = bufPtr;
    }
    else if (i < 0)      return(i);
    i = subStyleVarsWithValuesforHead(stateInfo->num_of_cols, 
		  stateInfo->colName,dimen,
		  stateInfo->dataTemplateT,buffer, 0, 0, NULL, NULL);
    if (i < 0)      return(i);

  }
  
  i = processEvalExpressionsInFilledStyleSheet(inbuffer);
  if (i < 0)      return(i);
  i = processIfConditionsInFilledStyleSheet(inbuffer);
  if (i < 0)      return(i);
  i = processIncludeObjectsInFilledStyleSheet(inbuffer);
  if (i < 0)      return(i);
  return(0);
}

int
splitStyleProc(char *inForm, tableStateInfo *stateInfo)
{

  char *tmpPtr, *tmpPtr2, *tmpPtr3, *tmpPtr4, *tmpPtr5;
  char *formPtr;
  int  i; 
  if ( (tmpPtr = strstr(inForm,"<TLHEAD>")) != NULL) {
    if ((tmpPtr2 = strstr(tmpPtr,"</TLHEAD>")) == NULL) 
      return(DATA_TAG_ERROR);
    *tmpPtr2 ='\0';
    tmpPtr += strlen("<TLHEAD>");
    if ((stateInfo->dataTemplateH = malloc(strlen(tmpPtr)+1)) == NULL)
      return MEMORY_ALLOCATION_ERROR;
    strcpy(stateInfo->dataTemplateH,tmpPtr);
    *tmpPtr2 ='<';
  }
  if ( (tmpPtr = strstr(inForm,"<TLBODY>")) != NULL) {
    if ((tmpPtr2 = strstr(tmpPtr,"</TLBODY>")) == NULL) 
      return(DATA_TAG_ERROR);
    *tmpPtr2 ='\0';
    tmpPtr += strlen("<TLBODY>");
    if ((stateInfo->dataTemplateB = malloc(strlen(tmpPtr)+1)) == NULL)
      return MEMORY_ALLOCATION_ERROR;
    strcpy(stateInfo->dataTemplateB,tmpPtr);
    *tmpPtr2 ='<';
  }
  if ( (tmpPtr = strstr(inForm,"<TLTAIL>")) != NULL) {
    if ((tmpPtr2 = strstr(tmpPtr,"</TLTAIL>")) == NULL) 
      return(DATA_TAG_ERROR);
    *tmpPtr2 ='\0';
    tmpPtr += strlen("<TLTAIL>");
    if ((stateInfo->dataTemplateT = malloc(strlen(tmpPtr)+1)) == NULL)
      return MEMORY_ALLOCATION_ERROR;
    strcpy(stateInfo->dataTemplateT,tmpPtr);
    *tmpPtr2 ='<';
  }
  if ( (tmpPtr = strstr(inForm,"<TLRULES>")) != NULL) {
    if ((tmpPtr2 = strstr(tmpPtr,"</TLRULES>")) == NULL) 
      return(DATA_TAG_ERROR);
    *tmpPtr2 ='\0';
    i = 0;
    formPtr = tmpPtr + strlen("<TLRULES>");

    if ( (tmpPtr = strstr(formPtr,"<TLQUERY>")) != NULL) {
      if ((tmpPtr3 = strstr(tmpPtr,"</TLQUERY>")) == NULL) 
	return(DATA_TAG_ERROR);
      *tmpPtr3 ='\0';
      tmpPtr += strlen("<TLQUERY>");
      if (stateInfo->sql == NULL || strlen(stateInfo->sql) == 0) {
	if (stateInfo->sql != NULL) 
	  free(stateInfo->sql);
	if ((stateInfo->sql = malloc(strlen(tmpPtr)+1))  == NULL)
	  return MEMORY_ALLOCATION_ERROR;
	strcpy(stateInfo->sql,tmpPtr);
	updateVarList(&(stateInfo->constantValues), "QQ",stateInfo->sql, "CHAR");
      }
      *tmpPtr3 ='<';
      formPtr = tmpPtr3 + strlen("</TLQUERY>");
    }
    if ( (tmpPtr = strstr(formPtr,"<TLRULECOND>")) != NULL) {
      if ((tmpPtr3 = strstr(tmpPtr,"</TLRULECOND>")) == NULL) 
	return(DATA_TAG_ERROR);
      *tmpPtr3 ='\0';
      tmpPtr += strlen("<TLRULECOND>");
      if ((stateInfo->dataTemplateChoice.choiceCondition = 
	   malloc(strlen(tmpPtr)+1))  == NULL)
	return MEMORY_ALLOCATION_ERROR;
      strcpy(stateInfo->dataTemplateChoice.choiceCondition,tmpPtr);
      *tmpPtr3 ='<';
      formPtr = tmpPtr3 + strlen("</TLRULECOND>");
    }

    while ((tmpPtr = strstr(formPtr,"<TLRULEHEAD>")) != NULL) {
      if ((tmpPtr3 = strstr(tmpPtr,"</TLRULEHEAD>")) == NULL) 
	return(DATA_TAG_ERROR);
      *tmpPtr3 ='\0';
      tmpPtr += strlen("<TLRULEHEAD>");
      if ((tmpPtr4 = strstr(tmpPtr,"<TLRULEPRESTRINGCOND>")) != NULL) {
	*tmpPtr4 ='\0';
	tmpPtr4 += strlen("<TLRULEPRESTRINGCOND>");
	if ((stateInfo->dataTemplateChoice.choicePreStringCond[i] 
	     = malloc(strlen(tmpPtr4)+1)) == NULL)
	  return MEMORY_ALLOCATION_ERROR;
	strcpy(stateInfo->dataTemplateChoice.choicePreStringCond[i],tmpPtr4);
	trimsides(stateInfo->dataTemplateChoice.choicePreStringCond[i]);
	if ((stateInfo->dataTemplateChoice.choiceHead[i] 
	    = malloc(strlen(tmpPtr)+1)) == NULL)
	  return MEMORY_ALLOCATION_ERROR;
	strcpy(stateInfo->dataTemplateChoice.choiceHead[i],tmpPtr);
	*tmpPtr4 ='<';
      }
      else {
	stateInfo->dataTemplateChoice.choicePreStringCond[i] =  NULL;
	if ((stateInfo->dataTemplateChoice.choiceHead[i] 
	     = malloc(strlen(tmpPtr)+1)) == NULL)
	  return MEMORY_ALLOCATION_ERROR;
	strcpy(stateInfo->dataTemplateChoice.choiceHead[i],tmpPtr);
      }
      trimsides(stateInfo->dataTemplateChoice.choiceHead[i]);
      *tmpPtr3 ='<';
      formPtr = tmpPtr3 + strlen("</TLRULEHEAD>");
      if ((tmpPtr = strstr(formPtr,"<TLRULETAIL>")) == NULL) 
	return(DATA_TAG_ERROR);
      *tmpPtr ='\0';
      if ((stateInfo->dataTemplateChoice.choiceBody[i] 
	    = malloc(strlen(formPtr)+1)) == NULL)
	return MEMORY_ALLOCATION_ERROR;
      strcpy(stateInfo->dataTemplateChoice.choiceBody[i],formPtr);
      trimsides(stateInfo->dataTemplateChoice.choiceBody[i]);
      *tmpPtr ='<';
      if ((tmpPtr3 = strstr(tmpPtr,"</TLRULETAIL>")) == NULL) 
	return(DATA_TAG_ERROR);
      *tmpPtr3 ='\0';
      tmpPtr += strlen("<TLRULETAIL>");
      if ((stateInfo->dataTemplateChoice.choiceTail[i] 
	    = malloc(strlen(tmpPtr)+1)) == NULL)
	return MEMORY_ALLOCATION_ERROR;
      strcpy(stateInfo->dataTemplateChoice.choiceTail[i],tmpPtr);
      trimsides(stateInfo->dataTemplateChoice.choiceTail[i]);
      *tmpPtr3 ='<';
      formPtr = tmpPtr3 + strlen("</TLRULEHEAD>");
      stateInfo->dataTemplateChoice.compiledHeadRE[i] = NULL;
      stateInfo->dataTemplateChoice.compiledTailRE[i] = NULL;
      i++;
    }
    stateInfo->dataTemplateChoice.choiceCount = i;
    *tmpPtr2 ='<';
  }
  if ( (tmpPtr = strstr(inForm,"<TLFUNCLIST>")) != NULL) {
    if ((tmpPtr2 = strstr(tmpPtr,"</TLFUNCLIST>")) == NULL) 
      return(DATA_TAG_ERROR);
    *tmpPtr2 ='\0';
    i = 0;
    formPtr = tmpPtr + strlen("<TLFUNCLIST>");
    while ((tmpPtr = strstr(formPtr,"<TLFUNC>")) != NULL) {
      if ((tmpPtr3 = strstr(tmpPtr,"<TLFUNCBODY>")) == NULL) 
	return(DATA_TAG_ERROR);
      *tmpPtr3 ='\0';
      tmpPtr += strlen("<TLFUNC>");
      if ((tmpPtr4 = strstr(tmpPtr,"("))  == NULL) {
	stateInfo->userFuncs.funcName[i] = strdup(tmpPtr);
	stateInfo->userFuncs.funcArgList[i] = strdup("");
	if ((stateInfo->userFuncs.funcName[i] == NULL) ||
	    (stateInfo->userFuncs.funcArgList[i] == NULL))
	  return MEMORY_ALLOCATION_ERROR;
	trimsides(stateInfo->userFuncs.funcName[i]);
      }
      else {
	if ((tmpPtr5 = strstr(tmpPtr4,")"))  == NULL) 
	  return(DATA_TAG_ERROR);
	*tmpPtr4 ='\0';
	*tmpPtr5 ='\0';
	stateInfo->userFuncs.funcName[i] = strdup(tmpPtr);
	stateInfo->userFuncs.funcArgList[i] = strdup(tmpPtr4 + 1);
	if ((stateInfo->userFuncs.funcName[i] == NULL) ||
	    (stateInfo->userFuncs.funcArgList[i] == NULL))
	  return MEMORY_ALLOCATION_ERROR;
	trimsides(stateInfo->userFuncs.funcName[i]);
	trimsides(stateInfo->userFuncs.funcArgList[i]);
	*tmpPtr4 ='(';
	*tmpPtr5 =')';
      }
      *tmpPtr3 ='<';
      formPtr = tmpPtr3 + strlen("<TLFUNCBODY>");
      if ((tmpPtr = strstr(formPtr,"</TLFUNC>")) == NULL) 
	return(DATA_TAG_ERROR);
      *tmpPtr ='\0';
      stateInfo->userFuncs.funcBody[i]  = strdup(formPtr);
      if (stateInfo->userFuncs.funcBody[i] == NULL)
	return MEMORY_ALLOCATION_ERROR;
      trimsides(stateInfo->userFuncs.funcBody[i]);
      *tmpPtr ='<';
      formPtr = tmpPtr + strlen("</TLFUNC>");
      i++;
    }
    stateInfo->userFuncs.funcCount = i;
    *tmpPtr2 ='<';
  }



  return(MDAS_SUCCESS);
}

int
subStyleVarsWithValuesforHead(int numOfColumns, 
			      char *nameentries[], 
			      char *Dimen[],
			      char *inForm,
			      char *outForm,
			      int   numOfTables,
			      int   numOfUserDefValues,
			      char *userQuery,
			      char *tableNameList)
{
  int i;
  char varName[MAX_TOKEN];
  char tempForm[HUGE_STRING];
  char *tmpPtr;
  char *tmpPtr2;

  /*  
      $$$ - data
      +++ - data inside form values (to be escaped)
      ### - dimensions
      @@@ - column names
      %%%RN - RowNumber

  */

  strcpy( tempForm ,inForm);

  for (i = 0; i < numOfColumns; i++) {
    sprintf(varName,"###%i:",i);
    tmpPtr = tempForm;
    strcpy(outForm,"");
    while( (tmpPtr2 = strstr(tmpPtr,varName)) !=NULL) {
      *tmpPtr2 = '\0';
      strcat(outForm,tmpPtr);
      *tmpPtr2 = '#';
      strcat(outForm,Dimen[i]);
      tmpPtr = tmpPtr2 + strlen(varName);
      if (i > 9) tmpPtr++;
    }
    strcat(outForm,tmpPtr);
    strcpy(tempForm,outForm);
  }
  for (i = 0; i < numOfColumns; i++) {
    sprintf(varName,"@@@%i:",i);
    tmpPtr = tempForm;
    strcpy(outForm,"");
    while( (tmpPtr2 = strstr(tmpPtr,varName)) !=NULL) {
      *tmpPtr2 = '\0';
      strcat(outForm,tmpPtr);
      *tmpPtr2 = '@';
      strcat(outForm,nameentries[i]);
      tmpPtr = tmpPtr2 + strlen(varName);
      if (i > 9) tmpPtr++;
    }
    strcat(outForm,tmpPtr);
    strcpy(tempForm,outForm);
  }

  for (i = 0; i < numOfColumns; i++) {
    sprintf(varName,"###%s:",nameentries[i]);
    tmpPtr = tempForm;
    strcpy(outForm,"");
    while( (tmpPtr2 = strstr(tmpPtr,varName)) !=NULL) {
      *tmpPtr2 = '\0';
      strcat(outForm,tmpPtr);
      *tmpPtr2 = '#';
      strcat(outForm,Dimen[i]);
      tmpPtr = tmpPtr2 + strlen(varName);
      if (i > 9) tmpPtr++;
    }
    strcat(outForm,tmpPtr);
    strcpy(tempForm,outForm);
  }
  for (i = 0; i < numOfColumns; i++) {
    sprintf(varName,"@@@%s:",nameentries[i]);
    tmpPtr = tempForm;
    strcpy(outForm,"");
    while( (tmpPtr2 = strstr(tmpPtr,varName)) !=NULL) {
      *tmpPtr2 = '\0';
      strcat(outForm,tmpPtr);
      *tmpPtr2 = '@';
      strcat(outForm,nameentries[i]);
      tmpPtr = tmpPtr2 + strlen(varName);
      if (i > 9) tmpPtr++;
    }
    strcat(outForm,tmpPtr);
    strcpy(tempForm,outForm);
  }
 return(MDAS_SUCCESS);

}

int
subStyleVarsWithValuesforBody(int   numOfColumns, 
			      int   rowNumber,
			      char *varValues[], 
			      char *lastValues[], 
			      char *Dimen[],
			      char *nameentries[],
			      char *inForm, 
			      char *outForm)
{
  int i;
  char varName[MAX_TOKEN];
  char tempForm[HUGH_HUGE_STRING];
  char *tmpPtr;
  char *tmpPtr2;
  char *escPtr;
  /*  
      $$$ - data
      +++ - data inside form values (to be escaped)
      ### - dimensions
      @@@ - column names
      !!! - oldData
      %%%RN - RowNumber
      
  */

  strcpy( tempForm ,inForm);
  for (i = 0; i < numOfColumns; i++) {
    sprintf(varName,"$$$%i:",i);
    tmpPtr = tempForm;
    strcpy(outForm,"");
    while( (tmpPtr2 = strstr(tmpPtr,varName)) !=NULL) {
      *tmpPtr2 = '\0';
      strcat(outForm,tmpPtr);
      *tmpPtr2 = '$';
      strcat(outForm,varValues[i]);
      tmpPtr = tmpPtr2 + strlen(varName);
    }
    strcat(outForm,tmpPtr);
    strcpy(tempForm,outForm);
  }
  for (i = 0; i < numOfColumns; i++) {
    sprintf(varName,"+++%i:",i);
    tmpPtr = tempForm;
    strcpy(outForm,"");
    while( (tmpPtr2 = strstr(tmpPtr,varName)) !=NULL) {
      *tmpPtr2 = '\0';
      strcat(outForm,tmpPtr);
      *tmpPtr2 = '+';
      escPtr = escape_url(varValues[i]);
      if (escPtr == NULL) return(MEMORY_ALLOCATION_ERROR);
      strcat(outForm,escPtr);
      free(escPtr);
      tmpPtr = tmpPtr2 + strlen(varName);

    }
    strcat(outForm,tmpPtr);
    strcpy(tempForm,outForm);
  }
  for (i = 0; i < numOfColumns; i++) {
    sprintf(varName,"###%i:",i);
    tmpPtr = tempForm;
    strcpy(outForm,"");
    while( (tmpPtr2 = strstr(tmpPtr,varName)) !=NULL) {
      *tmpPtr2 = '\0';
      strcat(outForm,tmpPtr);
      *tmpPtr2 = '#';
      strcat(outForm,Dimen[i]);
      tmpPtr = tmpPtr2 + strlen(varName);

    }
    strcat(outForm,tmpPtr);
    strcpy(tempForm,outForm);
  }
  for (i = 0; i < numOfColumns; i++) {
    sprintf(varName,"@@@%i:",i);
    tmpPtr = tempForm;
    strcpy(outForm,"");
    while( (tmpPtr2 = strstr(tmpPtr,varName)) !=NULL) {
      *tmpPtr2 = '\0';
      strcat(outForm,tmpPtr);
      *tmpPtr2 = '@';
      strcat(outForm,nameentries[i]);
      tmpPtr = tmpPtr2 + strlen(varName);

    }
    strcat(outForm,tmpPtr);
    strcpy(tempForm,outForm);
  }
  for (i = 0; i < numOfColumns; i++) {
    sprintf(varName,"!!!%i:",i);
    tmpPtr = tempForm;
    strcpy(outForm,"");
    while( (tmpPtr2 = strstr(tmpPtr,varName)) !=NULL) {
      *tmpPtr2 = '\0';
      strcat(outForm,tmpPtr);
      *tmpPtr2 = '!';
      strcat(outForm,lastValues[i]);
      tmpPtr = tmpPtr2 + strlen(varName);

    }
    strcat(outForm,tmpPtr);
    strcpy(tempForm,outForm);
  }



  for (i = 0; i < numOfColumns; i++) {
    sprintf(varName,"$$$%s:",nameentries[i]);
    tmpPtr = tempForm;
    strcpy(outForm,"");
    while( (tmpPtr2 = strstr(tmpPtr,varName)) !=NULL) {
      *tmpPtr2 = '\0';
      strcat(outForm,tmpPtr);
      *tmpPtr2 = '$';
      strcat(outForm,varValues[i]);
      tmpPtr = tmpPtr2 + strlen(varName);

    }
    strcat(outForm,tmpPtr);
    strcpy(tempForm,outForm);
  }
  for (i = 0; i < numOfColumns; i++) {
    sprintf(varName,"+++%s:",nameentries[i]);
    tmpPtr = tempForm;
    strcpy(outForm,"");
    while( (tmpPtr2 = strstr(tmpPtr,varName)) !=NULL) {
      *tmpPtr2 = '\0';
      strcat(outForm,tmpPtr);
      *tmpPtr2 = '+';
      escPtr = escape_url(varValues[i]);
      if (escPtr == NULL) return(MEMORY_ALLOCATION_ERROR);
      strcat(outForm,escPtr);
      free(escPtr);
      tmpPtr = tmpPtr2 + strlen(varName);

    }
    strcat(outForm,tmpPtr);
    strcpy(tempForm,outForm);
  }
  for (i = 0; i < numOfColumns; i++) {
    sprintf(varName,"###%s:",nameentries[i]);
    tmpPtr = tempForm;
    strcpy(outForm,"");
    while( (tmpPtr2 = strstr(tmpPtr,varName)) !=NULL) {
      *tmpPtr2 = '\0';
      strcat(outForm,tmpPtr);
      *tmpPtr2 = '#';
      strcat(outForm,Dimen[i]);
      tmpPtr = tmpPtr2 + strlen(varName);

    }
    strcat(outForm,tmpPtr);
    strcpy(tempForm,outForm);
  }
  for (i = 0; i < numOfColumns; i++) {
    sprintf(varName,"@@@%s:",nameentries[i]);
    tmpPtr = tempForm;
    strcpy(outForm,"");
    while( (tmpPtr2 = strstr(tmpPtr,varName)) !=NULL) {
      *tmpPtr2 = '\0';
      strcat(outForm,tmpPtr);
      *tmpPtr2 = '@';
      strcat(outForm,nameentries[i]);
      tmpPtr = tmpPtr2 + strlen(varName);

    }
    strcat(outForm,tmpPtr);
    strcpy(tempForm,outForm);
  }
  for (i = 0; i < numOfColumns; i++) {
    sprintf(varName,"!!!%s:",nameentries[i]);
    tmpPtr = tempForm;
    strcpy(outForm,"");
    while( (tmpPtr2 = strstr(tmpPtr,varName)) !=NULL) {
      *tmpPtr2 = '\0';
      strcat(outForm,tmpPtr);
      *tmpPtr2 = '!';
      strcat(outForm,lastValues[i]);
      tmpPtr = tmpPtr2 + strlen(varName);

    }
    strcat(outForm,tmpPtr);
    strcpy(tempForm,outForm);
  }

  tmpPtr = tempForm;
  strcpy(outForm,"");
  while( (tmpPtr2 = strstr(tmpPtr,"%%%RN:")) !=NULL) {
    *tmpPtr2 = '\0';
    strcat(outForm,tmpPtr);
    *tmpPtr2 = '%';
    sprintf (&outForm[strlen(outForm)],"%i",rowNumber);
    tmpPtr = tmpPtr2 + strlen("%%%RN:");
  }
  strcat(outForm,tmpPtr);
  strcpy(tempForm,outForm);
 return(MDAS_SUCCESS);


}



int
getNextDataFromTEMPLATE(tableStateInfo *stateInfo,
			char **buf, char *dataTag, 
			char **dataValue, int length)
{
  char *mybuf, *tmpPtr;
  int i,j, k, matchFlag;
  char tmpChar;
  char *startPtr, *endPtr, *bestStartPtr, *bestEndPtr;
  char *startTailPtr, *endTailPtr;
  int bestChoice;

  mybuf = *buf;
  bestStartPtr = NULL;
  bestEndPtr = NULL;

  if (stateInfo->dataTemplateChoice.compiledHeadRE[0] == NULL) {
    for (i = 0; i < stateInfo->dataTemplateChoice.choiceCount ; i++) {
      j = compileRegExp(&stateInfo->dataTemplateChoice.compiledHeadRE[i],
			stateInfo->dataTemplateChoice.choiceHead[i]);
      if (j != 0) 
	return(j);
      j = compileRegExp(&stateInfo->dataTemplateChoice.compiledTailRE[i],
			stateInfo->dataTemplateChoice.choiceTail[i]);
      if (j != 0) 
	return(j);
    }
  }
  if ((stateInfo->dataTemplateChoice.choiceCondition == NULL) ||
      (strstr(stateInfo->dataTemplateChoice.choiceCondition,"match=") == NULL) ||
      (strstr(stateInfo->dataTemplateChoice.choiceCondition, "match=FIRSTRULE")
       != NULL)) {
    matchFlag = 0;
  }
  else if((strstr(stateInfo->dataTemplateChoice.choiceCondition, "match=NEAREST")
	   != NULL)) {
    matchFlag = 1;
  }
  else if((strstr(stateInfo->dataTemplateChoice.choiceCondition, "match=ROUNDROBIN")
	   != NULL)) {
    matchFlag = 2;
  }
  
  if ( matchFlag == 0 ) {
    for (i = 0; i < stateInfo->dataTemplateChoice.choiceCount ; i++) {
      j = matchCompRegExp(stateInfo->dataTemplateChoice.compiledHeadRE[i], 
		      mybuf, &startPtr, &endPtr);
      if (j == RE_EVALUATION_ERROR) return(j);
      if (j == MDAS_SUCCESS) {
	if (stateInfo->dataTemplateChoice.choicePreStringCond[i] != NULL) {
	  tmpChar = *startPtr;
	  *startPtr =  '\0';
	  k = evaluateLogicalExpression(
	       stateInfo->dataTemplateChoice.choicePreStringCond[i]);
	  *startPtr = tmpChar;
	  if (k  == 1) {
	    bestStartPtr = startPtr;
	    bestEndPtr = endPtr;
	    bestChoice = i;
	    break;
	  }
	}
	else {
	  bestStartPtr = startPtr;
	  bestEndPtr = endPtr;
	  bestChoice = i;
	  break;
	}
      }
    }
  }
  else if ( matchFlag == 1 ) {
    for (i = 0; i < stateInfo->dataTemplateChoice.choiceCount ; i++) {
      j = matchCompRegExp(stateInfo->dataTemplateChoice.compiledHeadRE[i], 
		      mybuf, &startPtr, &endPtr);
      if (j == RE_EVALUATION_ERROR) return(j);
      if (j == MDAS_SUCCESS && (bestStartPtr==NULL || bestStartPtr > startPtr)) {
	if (stateInfo->dataTemplateChoice.choicePreStringCond[i] != NULL) {
	  tmpChar = *startPtr;
	  *startPtr =  '\0';
	  k = evaluateLogicalExpression(
	       stateInfo->dataTemplateChoice.choicePreStringCond[i]);
	  *startPtr = tmpChar;
	  if (k  == 1) {
	    bestStartPtr = startPtr;
	    bestEndPtr = endPtr;
	    bestChoice = i;
	  }
	}
	else {
	  bestStartPtr = startPtr;
	  bestEndPtr = endPtr;
	  bestChoice = i;
	}
      }
    }
  }
  else {
        for (i = stateInfo->dataTemplateChoice.roundRobin; 
	     i < stateInfo->dataTemplateChoice.choiceCount ; i++) {
      j = matchCompRegExp(stateInfo->dataTemplateChoice.compiledHeadRE[i], 
		      mybuf, &startPtr, &endPtr);
      if (j == RE_EVALUATION_ERROR) return(j);
      if (j == MDAS_SUCCESS) {
	if (stateInfo->dataTemplateChoice.choicePreStringCond[i] != NULL) {
	  tmpChar = *startPtr;
	  *startPtr =  '\0';
	  k = evaluateLogicalExpression(
	       stateInfo->dataTemplateChoice.choicePreStringCond[i]);
	  *startPtr = tmpChar;
	  if (k  == 1) {
	    bestStartPtr = startPtr;
	    bestEndPtr = endPtr;
	    bestChoice = i;
	    stateInfo->dataTemplateChoice.roundRobin = 
	      (i + 1) % stateInfo->dataTemplateChoice.choiceCount;	
	    break;
	  }
	}
	else {
	  bestStartPtr = startPtr;
	  bestEndPtr = endPtr;
	  bestChoice = i;
	  stateInfo->dataTemplateChoice.roundRobin = 
	      (i + 1) % stateInfo->dataTemplateChoice.choiceCount;	
	  break;
	}
      }
    }
  }
  if (bestStartPtr == NULL || bestEndPtr > (mybuf + length -1))
      return (RE_MATCH_NOT_FOUND);
  if (strcmp(stateInfo->dataTemplateChoice.choiceTail[bestChoice],"EOF$")) {
    j = matchCompRegExp(stateInfo->dataTemplateChoice.compiledTailRE[bestChoice],
			bestEndPtr, &startTailPtr, &endTailPtr);
    if (j == RE_EVALUATION_ERROR) return(j);
    if (j != MDAS_SUCCESS)     return(RE_MATCH_NOT_FOUND);
  }
  else {
    startTailPtr = bestEndPtr;
    while (*startTailPtr != '\n' &&
	   *startTailPtr != '\r' &&
	   *startTailPtr != '\f' &&
	   *startTailPtr != '\0' )
      startTailPtr++;
  }
    
  if (strstr(stateInfo->dataTemplateChoice.choiceBody[bestChoice],"|") 
      == NULL ) {
    tmpChar = *startTailPtr;
    *startTailPtr = '\0';
    *dataValue = strdup(bestEndPtr);
    if (*dataValue == NULL) return(MEMORY_ALLOCATION_ERROR);
    *startTailPtr = tmpChar;
    strcpy(dataTag,stateInfo->dataTemplateChoice.choiceBody[bestChoice]);
    *buf = startTailPtr;
  }
  else {
    tmpChar = *startTailPtr;
    *startTailPtr = '\0';
    *dataValue = malloc(strlen(bestEndPtr) + strlen(stateInfo->dataTemplateChoice.choiceBody[bestChoice])+1);
    if (*dataValue == NULL) return(MEMORY_ALLOCATION_ERROR);
    strcpy(*dataValue,stateInfo->dataTemplateChoice.choiceBody[bestChoice]);
    tmpPtr = strstr(*dataValue,"|");
    *tmpPtr ='\0';
    tmpPtr++;
    strcpy(dataTag,tmpPtr);
    strcat(*dataValue,bestEndPtr);
    tmpPtr = strstr(dataTag,"|");
    if (tmpPtr != NULL) {
      *tmpPtr ='\0';
      tmpPtr++;
      strcat(*dataValue,tmpPtr);
    }
    *startTailPtr = tmpChar;
    /*strcpy(dataTag,stateInfo->dataTemplateChoice.choiceBody[bestChoice]);*/
    *buf = startTailPtr;
  }
  return (MDAS_SUCCESS);
}

int
updateVarList(varsList *listName, char *varName, char *varValue, char *varType)
{
  int i;

  for (i = 0; i < listName->varCount ; i++) {
    if (!strcmp(listName->varName[i], varName)) {
      free(listName->varValue[i]);
      listName->varValue[i] =  strdup(varValue);
      return(MDAS_SUCCESS);
    }
  }

  return(addToVarList(listName,varName,varValue,varType));
}

int
addToVarList(varsList *listName, char *varName, char *varValue, char *varType)
{
    
  listName->varName[listName->varCount] =  strdup(varName);
  if (listName->varName[listName->varCount] == NULL) return (MEMORY_ALLOCATION_ERROR);
  listName->varType[listName->varCount] =  strdup(varType);
  if (listName->varType[listName->varCount] == NULL) return (MEMORY_ALLOCATION_ERROR);
  listName->varValue[listName->varCount] =  strdup(varValue);
  if (listName->varValue[listName->varCount] == NULL) return (MEMORY_ALLOCATION_ERROR);
  listName->varCount++;
  return(MDAS_SUCCESS);
}

int
extractOptionsDBDvr(tableStateInfo *stateInfo,
			char **indataPathName)
{

  char *dataPathName, *nextDataPathName;
  char *tmpPtr2, *tmpPtr3, *templateString;
  char tmpStr[SMALL_TOKEN];

  dataPathName = *indataPathName;
  
  while (1) {
    while (isspace(*dataPathName)) dataPathName++;
    if (strncmp(dataPathName,"<TEMPLATETYPE>", 14) == 0) {
      dataPathName = dataPathName + strlen("<TEMPLATETYPE>");
      if ((nextDataPathName = strstr(dataPathName,"</TEMPLATETYPE>")) == NULL) 
	return (INPUT_PROPERTY_ERROR);
      *nextDataPathName = '\0';
      nextDataPathName += strlen("</TEMPLATETYPE>");
      
      if (strncasecmp(dataPathName,"XMLREL", 6) == 0) {
	strcpy(stateInfo->dataType, "XMLREL");
	for (i=0; i < 6 ; i++) 
	  dataPathName[i] = ' ';
      }
      else if (strncasecmp(dataPathName,"XMLTREE", 7) == 0) {
	strcpy(stateInfo->dataType, "XMLTREE");
	for (i=0; i < 7 ; i++) 
	  dataPathName[i] = ' ';
      }
      else if (strncasecmp(dataPathName,"HTMLREL", 7) == 0) {
	strcpy(stateInfo->dataType, "HTMLREL");
	for (i=0; i < 7 ; i++) 
	  dataPathName[i] = ' ';
      }
      else if (strncasecmp(dataPathName,"HTMLTREE", 8) == 0) {
	strcpy(stateInfo->dataType, "HTMLTREE");
	for (i=0; i < 8 ; i++) 
	  dataPathName[i] = ' ';
      }
      else if (strncasecmp(dataPathName,"HTMLNEST", 8) == 0) {
	strcpy(stateInfo->dataType, "HTMLNEST");
	for (i=0; i < 8 ; i++) 
	  dataPathName[i] = ' ';
      }
      else if (strncasecmp(dataPathName,"HEAD", 4) == 0) {
	strcpy(stateInfo->dataType, "HEAD");
	for (i=0; i < 4 ; i++) 
	  dataPathName[i] = ' ';
      }
      else if (strncasecmp(dataPathName,"PACK", 4) == 0) {
	strcpy(stateInfo->dataType, "PACK");
	for (i=0; i < 4 ; i++) 
	  dataPathName[i] = ' ';
      }
      else if (strncasecmp(dataPathName,"SQL", 3) == 0) {
	strcpy(stateInfo->dataType, "SQL");
	for (i=0; i < 3 ; i++) 
	  dataPathName[i] = ' ';
      }
      else if (strncasecmp(dataPathName,"VALUES", 6) == 0) {
	strcpy(stateInfo->dataType, "VALUES");
	for (i=0; i < 6 ; i++) 
	  dataPathName[i] = ' ';
      }
      else if (strncasecmp(dataPathName,"TEMPLATE", 8) == 0) {
	strcpy(stateInfo->dataType, "TEMPLATE");
	for (i=0; i < 8 ; i++) 
	  dataPathName[i] = ' ';
	while (isspace(*dataPathName)) dataPathName++;
	if ((i = readSrbObjIntoBufferPtr(dataPathName, &templateString)) < 0) {
	  elog (NOTICE, "extractOptionsDBDvr: Error in Reading SRB Object  %s: %i",
		dataPathName,i);
	  return(i);
	}
	j = splitStyleProc( templateString, stateInfo);
	i = addToVarList(&(stateInfo->constantValues), "TF",dataPathName, "CHAR");
	free( templateString);
	if (j != 0) {
	  elog (NOTICE, "extractOptionsDBDvr: Error in Parsing Template  %s: %i",
		dataPathName,j);
	  return(j);
	}
	if (i < 0) return(i);
	dataPathName = tmpPtr3;
      }
      else {
	strcpy(stateInfo->dataType, "SQL");
      }
      i = addToVarList(&(stateInfo->constantValues), "DT",stateInfo->dataType, "CHAR");
      if (i < 0) return(i);
      if (strcmp(stateInfo->dataType,"TEMPLATE") ) {
	i = addToVarList(&(stateInfo->constantValues), "TF",stateInfo->dataType, "CHAR");
	if (i < 0) return(i);
      }
    }
    else if (strncmp(dataPathName,"<TEMPLATEPARAM>", strlen("<TEMPLATEPARAM>")) == 0) {
      dataPathName = dataPathName + strlen("<TEMPLATEPARAM>");
      if ((nextDataPathName = strstr(dataPathName,"</TEMPLATEPARAM>")) == NULL) 
	return (INPUT_PROPERTY_ERROR);
      *nextDataPathName = '\0';
      nextDataPathName += strlen("</TEMPLATEPARAM>");
      while ((tmpPtr3 = (char *)skiptochar(dataPathName,',')) != NULL) {
	*tmpPtr3 = '\0';
	if ((tmpPtr2  = (char *)skiptochar(dataPathName,'=')) == NULL)
	  return(INPUT_PROPERTY_ERROR);
	*tmpPtr2 = '\0';
	tmpPtr2++;
	trimsides(dataPathName);
	trimsides(tmpPtr2);
	i = addToVarList(&(stateInfo->definedValues), 
			 dataPathName,tmpPtr2, "CHAR");
	if (i < 0) return(i);
	dataPathName = tmpPtr3+1;
      }
      if ((tmpPtr2  = (char *)skiptochar(dataPathName,'=')) == NULL)
	return(INPUT_PROPERTY_ERROR);
      *tmpPtr2 = '\0';
      tmpPtr2++;
      trimsides(dataPathName);
      trimsides(tmpPtr2);
      i = addToVarList(&(stateInfo->definedValues), 
		       dataPathName,tmpPtr2, "CHAR");
      if (i < 0) return(i);
      sprintf(tmpStr,"%i",stateInfo->definedValues.varCount);
      i = addToVarList(&(stateInfo->constantValues),
			 "ND",tmpStr, "NUM");
	if (i < 0) return(i);
    }
    else { break;}
    dataPathName = nextDataPathName;
  }

  *indataPathName = dataPathName;
  return(MDAS_SUCCESS);

}
int
getMultiInsertInformation(tableStateInfo *stateInfo, char *inString)
{

  char *tmpPtr, *tmpPtr2;
  int i;
  tmpPtr = inString;
  while (isspace(*tmpPtr)) tmpPtr++;

  if (strstr(tmpPtr,"insert") != tmpPtr &&
	  strstr(tmpPtr,"INSERT") != tmpPtr) {
    tmpPtr2 =  strchr(tmpPtr, ' ');
    *tmpPtr2 = '\0';
    stateInfo->compSql.dataTagOpen = tmpPtr;
    tmpPtr2++;
    while (isspace(*tmpPtr2)) tmpPtr2++;
    tmpPtr =  strchr(tmpPtr2, ' ');
    *tmpPtr = '\0';
    stateInfo->compSql.dataTagClose = tmpPtr2;
    tmpPtr++;
    while (isspace(*tmpPtr)) tmpPtr++;
  }
  else {
  stateInfo->compSql.dataTagOpen = STANDARAD_OPEN_TAG;
  stateInfo->compSql.dataTagClose = STANDARAD_CLOSE_TAG;
  }

  
  i = captureStmtsIntoSqlList(&(stateInfo->compSql), tmpPtr);
  if (i < 0) return(i);
  for (i = 0; i < stateInfo->compSql.sqlCount ; i++) {
    stateInfo->compSql.tagusage[i][0]  = TAG_BAD;
    j = extractTags(&(stateInfo->compSql), i, 0, 
		    stateInfo->compSql.dataTagOpen ,
		    stateInfo->compSql.dataTagClose,
		    stateInfo->compSql.sql[i]);
    if (j < 0) return(j);
  }
  return(MDAS_SUCCESS);
  
  
}


int
appendAllUnConsumedSqlIntoAllocatedBuffer(sqlList *sl,  char **sqlString, 
				    int *bufSize)
{
  int i, j, k, ii;
  int tagFlag = 0;
  for (i = 0 ; i < sl->sqlCount ; i++) {
    for ( j = 0  ; j < MAX_SQL_TAGS_COUNT ; j++) {
      if (sl->tagusage[i][j] == TAG_BAD)
	break;
      if (sl->tagusage[i][j] == TAG_FULL) {
	if ((ii = appendSqlFromSqlListIntoAllocatedBuffer(
                         sl, i, sqlString,bufSize)) < 0)
	  return(ii);
	tagFlag = 1;
	for ( k = 0  ; k < MAX_SQL_TAGS_COUNT ; k++) {
	  if (sl->tagusage[i][k] == TAG_BAD)
	    break;
	  sl->tagusage[i][k] = TAG_CONSUMED;
	}
	break;
      }
    }
  }
  return(tagFlag);	  
}
int
appendSqlFromSqlListIntoAllocatedBuffer(sqlList *sl, 
				    int i, char **sqlString, 
				    int *bufSize)
{
  char *sql;
  int n,j,k, m;

  sql = *sqlString;
  n = 0 ;
  for ( j = 0  ; j < MAX_SQL_TAGS_COUNT ; j++) {
    if (sl->tagusage[i][j] == TAG_BAD)
      break;
    if (sl->dataValue[i][j] == NULL) return (MISSING_DATA_ERROR);
    n += strlen(sl->preString[i][j]);
    n += strlen(sl->dataValue[i][j]);
  }
  n += strlen(sl->postString[i]);
  k =  *bufSize  ;
  if (n > k) {    /** reallocate space **/
    while  (n > k) 
      k += SVR_OBJ_READ_BUFFER_SIZE ;
    m = k +  strlen(*sqlString);
    if ((sql =  (char *) malloc(m)) == NULL)
      return(MEMORY_ALLOCATION_ERROR);
    *bufSize = k;
    memcpy(sql, *sqlString, (strlen(*sqlString) + 1));
  }
  for ( j = 0  ; j < MAX_SQL_TAGS_COUNT ; j++) {
    if (sl->tagusage[i][j] == TAG_BAD)
      break;
    strcat(sql,sl->preString[i][j]);
    strcat(sql,sl->dataValue[i][j]);
  }
  strcat(sql,sl->postString[i]);
  strcat(sql,";");
  *sqlString = sql;
  *bufSize = *bufSize - n;
  return(MDAS_SUCCESS);
}
int
captureStmtsIntoSqlList(sqlList *sl, char *inString)
{
  char *tmpPtr, *tmpPtr2;

  tmpPtr = inString;
  i = 0;
  trimsides(tmpPtr);
  while ((tmpPtr2 = getEndOfStatement(tmpPtr)) != NULL) {
    *tmpPtr2 = '\0';
    sl->sql[sl->sqlCount] = tmpPtr;
    sl->sqlCount++;
    tmpPtr = tmpPtr2 + 1;
  }
  return(MDAS_SUCCESS);
}
int 
extractTags(sqlList *sl, int stmtNum , int beginIndex, 
	    char *openTag, char *closeTag, char *inString)
{
  char *tmpPtr, *tmpPtr2, *tmpPtr3;
  int i,j;


  tmpPtr = inString;
  i = strlen(openTag);
  j = strlen(closeTag);
  sl->tagusage[stmtNum][beginIndex] = TAG_BAD;
  while (  (tmpPtr2 = strstr(tmpPtr, openTag)) != NULL) {
    if ((tmpPtr3 = strstr(tmpPtr2, closeTag)) == NULL)
      return(DATA_TAG_ERROR);
    *tmpPtr2 = '\0';
    tmpPtr2 += i;
    *tmpPtr3 = '\0';
    sl->tags[stmtNum][beginIndex] = tmpPtr2;
    sl->preString[stmtNum][beginIndex] = tmpPtr; 
    sl->tagusage[stmtNum][beginIndex] = TAG_CONSUMED;
    beginIndex++;
    sl->tagusage[stmtNum][beginIndex] = TAG_BAD;
    tmpPtr = tmpPtr3 + j;
  }
  sl->postString[stmtNum] = tmpPtr;
  return(MDAS_SUCCESS);

}

int
getNumOfColsFromTags(tableStateInfo *stateInfo)
{
   int i,j,k, m;

   m =  0;
   for (i = 0 ; i < stateInfo->compSql.sqlCount ; i++) {
    for ( j = 0  ; j < MAX_SQL_TAGS_COUNT ; j++) {
      if (stateInfo->compSql.tagusage[i][j] == TAG_BAD)
	break;
      for (k = 0 ; k < m ; k++) 
	if (!strcmp(stateInfo->xmlcolName[k], stateInfo->compSql.tags[i][j])) 
	  break;
      if (k == m) {
	stateInfo->xmlcolName[m] = strdup(stateInfo->compSql.tags[i][j]);
	m++;
      }
    }
   }
   return(m); 
 
}

int
reverseExecution(tableStateInfo *stateInfo)
{
  if (!strcmp(stateInfo->dataType, "XMLREL"))
    return(FALSE);
  else if (!strcmp(stateInfo->dataType, "XMLTREE"))
    return(FALSE);
  else if (!strcmp(stateInfo->dataType, "HTMLREL"))
    return(FALSE);
  else if (!strcmp(stateInfo->dataType, "HTMLTREE"))
    return(FALSE);
  else if (!strcmp(stateInfo->dataType, "HTMLNEST"))
    return(TRUE);
  else if (!strcmp(stateInfo->dataType, "TEMPLATE"))
    return(TRUE);
  else if (!strcmp(stateInfo->dataType, "SQL"))
    return(FALSE);
  else if (!strcmp(stateInfo->dataType, "PACK"))
    return(FALSE);
  else if (!strcmp(stateInfo->dataType, "VALUES"))
    return(FALSE);
  else
    return(FALSE);
}
