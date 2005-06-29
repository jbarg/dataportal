/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

#ifdef DB_Oracle
#define ORACLE_DRIVER
#include "srbC_ora_externs.h"


/* The following table should be created before storing Large object :
  create table srbVault(
  obj_id     char(255),
  obj_locator long raw);
 */

/* GLOBALS */
  static int i, n;
  static unsigned char        query[100];
  static char                 queryString[1000]; 
  static char                 oracledblobsidbuffer[MAX_TOKEN];

void  err_report( Cda_Def *cursor, Lda_Def *lda)
{   sword n;
    text msg[512];

    elog(NOTICE, "\n-- ORACLE error--\n");
    elog(NOTICE, "\n");
    n = oerhms(lda, cursor->rc, msg, (sword) sizeof msg);

    elog(NOTICE, "%s\n", msg);

    if (cursor->fc > 0)
        elog(NOTICE, "Processing OCI function %s:\n",
            oci_func_tab[cursor->fc]);


}




void
separateColVal(char *inval, char *columnSet, char *valueSet)
{
  int i,j,k,l,m;
  i = 0; j = 0; k = 0; m = 0;
  l = strlen(inval);
  for (i = 0; i < l ; i++)
    {
      if (inval[i] == '\0') break;
      /*if (inval[i] == ' ' && inval[i-1] != '\\') continue;      */
      else if (inval[i] == '\\' && inval[i-1] != '\\') continue;
      else if (inval[i] == '&' && inval[i-1] != '\\') 
             {valueSet[k] = ','; k++; m = 0;}
      else if (inval[i] == '=' && inval[i-1] != '\\')
             {columnSet[j] = ','; j++; m = 1;}
      else
	if (m == 0) {columnSet[j] = inval[i]; j++;}
        else {valueSet[k] = inval[i]; k++;}
    }
  columnSet[j] = '\0';
  valueSet[k] = ',';  k++;
  valueSet[k] = '\0';
}

convertAnd(char *inval)
{
    char temptab[STRING_LENMAX];
    int i,j;
    i = 0; j = 0;
    strcpy(temptab,inval);
    while(temptab[i] != '\0')
      {
	if (temptab[i] == '&')
	  {
	    inval[j] = ' ';
	    inval[++j] = 'A';
	    inval[++j] = 'N';
	    inval[++j] = 'D';
	    inval[++j] = ' ';
	  }
	else
	  {
	    inval[j] = temptab[i];
	  }
	j++; i++;
      }
	    
}

int mm_init(or_db_state_info *stateInfo, char *fname) {
   off_t off = 0;
   srand(time(NULL));
  
   sprintf(stateInfo->mm_fname, "srb_ora.%s.%d.%d.mmap", 
		   fname, getpid(), rand());

   stateInfo->mm_size = BUF * BUF * 10; /* initial size at 10 mb */

/* This mmap does not work for solaris. For solaris, the mm_fname with length
 * mm_size must already exist before it can be mapped.
 */
#ifdef MM_MAP
   if((stateInfo->mm_fd = 
	   open( stateInfo->mm_fname, O_RDWR | O_CREAT, 0644)) == -1){
	 elog(NOTICE, "srbC_ora: Could not create mmap file %s\n", stateInfo->mm_fname);
	 return ORA_MMAP_CREATE_ERROR;
   }

   stateInfo->mm_ptr = (char*) mmap(NULL, stateInfo->mm_size, 
     (PROT_READ | PROT_WRITE), (MAP_PRIVATE), 
	stateInfo->mm_fd, off);

   if(stateInfo->mm_ptr == MAP_FAILED) {
         elog(NOTICE, "srbC_ora:Mapping Failed\n");
         close(stateInfo->mm_fd);
         unlink(stateInfo->mm_fname);
         return -1;
   }

#else
   stateInfo->mm_ptr = malloc (stateInfo->mm_size);
#endif

   return 1;
}

int mm_delete(or_db_state_info *stateInfo) {
   int res;

#ifdef MM_MAP
 if((res = munmap(stateInfo->mm_ptr, stateInfo->mm_size)) == -1) {
   stateInfo->mm_ptr = NULL;
   elog(NOTICE, "srbC_ora:Error unmapping memmap\n");
   close(stateInfo->mm_fd); unlink(stateInfo->mm_fname);  
   return ORA_MMAP_DELETE_ERROR;
 }

 stateInfo->mm_ptr = NULL;
 close(stateInfo->mm_fd); 
 unlink(stateInfo->mm_fname); 
#else
 free (stateInfo->mm_ptr);
#endif
 return 1;

}












/*
 Open an existing large object
 *
 * Input : char *resourceLocationName - format : host:database:instance.
 *         For Illustra, instance is not used.
 *         char *dataPathName - Format : tablename/outCol/inCol=objectID
 *                          or tabname/outCol/inCol1=val1&...&inColn=valn
 *
 */


int srbC_ora_open(dbLobjDesc *exfd, char *resourceLocationName,
		  char *dataPathName, char *uid, char *pwd)
{
    or_db_state_info *stateInfo;
    char  tmpStr[STRING_LENMAX];
    char instance[STRING_LEN1],hostAddr[STRING_LEN1], database[STRING_LEN1];
	int 	  bufmk = 0;
	text    longbuf[BUF+1];
	ub4     len_longbuf  = BUF;
	ub4     iteration = 0;
	ub4     plsqltable;
	ub1     cont = 1;
	ub1     piece;
	ub2     col_rcode;
	ub2     col_len;
    char   *context = "context pointer"; /* dummy value */
 
    if (mySplitStr(resourceLocationName, hostAddr, tmpStr, ':') == NULL) {
        elog (NOTICE, "srbC_ora_create: Error in resourceLocationName : %s",
        resourceLocationName);
        return INP_ERR_RES_FORMAT;
    }
    if (mySplitStr(tmpStr, database, instance, ':') == NULL) {
        elog (NOTICE, "srbC_ora_create: Error in resourceLocationName : %s",
        resourceLocationName);
        return INP_ERR_RES_FORMAT;
    }

    /* dataPathName must begin with a '/' */

    if (*dataPathName != '/')
        return DBLOBJ_PATH_NAME_ERROR;

    dataPathName ++;            /* skip the '/' */

    /*First allocate an area for state information */
    if (exfd->dbSystemType != ORA_LOBJ)	{	
        elog(NOTICE, "srbC_ORA_open: Bad ORA_LOBJ Type: %i\n", 
             exfd->dbSystemType);  
      return ORA_ERR_OBJ_DESC;
	}
    /* allocate the space for the dbstateinfo */
	if((stateInfo = malloc(sizeof (or_db_state_info))) == NULL) {
	  elog (NOTICE, "srbC_ora_open: Malloc error");
      return ORA_ERR_MALLOC;
	}

    /* init the state info */   
    stateInfo->mm_ptr = NULL;
    stateInfo->dirty = 'F';
	stateInfo->offset = 1;

    /* zero out the Hda */
	memset(&stateInfo->hda, 0, HDA_SIZE);

    if (mySplitStr(dataPathName, stateInfo->tableName,tmpStr,'/') == NULL) 
      {
		elog(NOTICE, "srbC_ORA_open: Illegal dataPathName format: %s",
			  dataPathName);
        free(stateInfo);
		return ORA_LOBJ_PATH_NAME_ERROR;
      }

    if (mySplitStr(tmpStr, stateInfo->outColName,
					stateInfo->inCondition,'/') == NULL) 
		{

			elog(NOTICE, "srbC_ORA_open: Illegal dataPathName format: %s",
					dataPathName);
			free(stateInfo);
			return ORA_LOBJ_PATH_NAME_ERROR;
		}
    convertAnd(stateInfo->inCondition);

 /* ORACLE logon calls */ 

#ifdef ORACLE_CONNECT_TNS
    if (olog(&stateInfo->lda, (ub1*)stateInfo->hda, 
		  (text*) uid, -1, (text *) pwd, -1,
		  (text *) database, -1, OCI_LM_DEF))
   {  
	 elog (NOTICE, "srbC_ORA_open:  Connect Error\n");
	 err_report(&stateInfo->cda, &stateInfo->lda); 
	 free(stateInfo);
	 return ORA_ERR_LOGIN;
   }
#endif /* ORACLE_CONNECT_TNS */
#ifdef ORACLE_CONNECT_ONS
  sprintf(oracledblobsidbuffer,"ORACLE_SID=%s",database);
  putenv(oracledblobsidbuffer);
 if (olog(&stateInfo->lda, (ub1*)stateInfo->hda, 
		  (text*) uid, -1, (text *) pwd, -1,
		  (text *) 0, -1, OCI_LM_DEF))
   {  
	 elog (NOTICE, "srbC_ORA_open:  Connect Error\n");
	 err_report(&stateInfo->cda, &stateInfo->lda); 
	 free(stateInfo);
	 return ORA_ERR_LOGIN;
   }
#endif /* ORACLE_CONNECT_ONS */

 /* turn auto commit off */
 ocof(&stateInfo->lda);

 /* open the cursor */
 if (oopen(&stateInfo->cda, &stateInfo->lda, 
		   (text *) 0, -1, -1, (text *) 0, -1))
   {
	 elog (NOTICE, "srbC_ORA_open: Error opening cursor.\n");
	 err_report(&stateInfo->cda, &stateInfo->lda);
	 ologof(&stateInfo->lda);
	 free(stateInfo);
	 return ORA_ERR_OPEN_CURSOR;
   }


 sprintf(queryString, "select %s from %s where %s",
		  stateInfo->outColName, stateInfo->tableName, 
		  stateInfo->inCondition);


 if (oparse(&stateInfo->cda, (text*)queryString, 
			(sb4) -1, FALSE, (ub4) VERSION_7))
   {
	 elog (NOTICE, "srbC_ORA_open: Error Parsing statement.\n");
	 err_report(&stateInfo->cda, &stateInfo->lda);
	 oclose(&stateInfo->cda); ologof(&stateInfo->lda);
	 free(stateInfo);
   
     return ORA_ERR_SQLSTMT_PARSE;
   }

/* create the memory map to hold the query results */
 
 if(!mm_init(stateInfo, stateInfo->outColName)){
    elog(NOTICE, "srbC_ORA_open: Could not buffer the requested file\n");
    oclose(&stateInfo->cda); ologof(&stateInfo->lda); 
    free(stateInfo);
    return ORA_MMAP_CREATE_ERROR;
 } 

 if (odefinps(&stateInfo->cda, 0, 1, (ub1 *)&context, 
			  (ub4) ((uint)MAX_LONG_SIZE - 1), 
			  SQLT_LNG, 0, (sb2 *)0, (text *)0, 0, 0,
			  (ub2 *)&col_len, (ub2 *)&col_rcode, 0, 0, 0, 0)) {
   err_report(&stateInfo->cda, &stateInfo->lda);
   oclose(&stateInfo->cda); ologof(&stateInfo->lda);
   mm_delete(stateInfo); free(stateInfo);
   return ORA_ERR_PIECEWISE_DEFINE; 
 }
 
 /* Execute the Query Statement */ 
 if (oexec(&stateInfo->cda)) {   
   elog(NOTICE, "srbC_ORA_open:Error in executing SQL stmt\n");    
   err_report(&stateInfo->cda, &stateInfo->lda);
   oclose(&stateInfo->cda); ologof(&stateInfo->lda);
   mm_delete(stateInfo); free(stateInfo);
   return ORA_ERR_OEXEC; 
 }	

 while (cont)            
   {
	 /* do fetch & switch on return value */
	 ofetch(&stateInfo->cda);
	 switch (stateInfo->cda.rc)       
	   { 
	   case 0:  /* write last piece to buffer */
		 memcpy(stateInfo->mm_ptr + bufmk, longbuf, len_longbuf);
		 bufmk += len_longbuf;

		 if(stateInfo->mm_size < bufmk)
		   stateInfo->mm_size = bufmk;
         /* assign the size */
         stateInfo->lobsize = bufmk;
		 cont = 0;
		 break;
		 
	   case OCI_MORE_FETCH_PIECES:   /* ORA-03130 was returned */
		 ogetpi(&stateInfo->cda, &piece, (dvoid **)&context, 
				&iteration, &plsqltable);
		 
		 if (piece!=OCI_FIRST_PIECE) { /* can't write on first fetch */
/* XXXX - take out logging very large message 
		   elog(NOTICE,"srbopen next piece:\n%s\n", longbuf);
*/
		   memcpy(stateInfo->mm_ptr + bufmk, longbuf, len_longbuf); 
		   bufmk += len_longbuf;
           
		   if(stateInfo->mm_size < bufmk)
			 stateInfo->mm_size = bufmk;
		 }
		 osetpi(&stateInfo->cda, piece, longbuf, &len_longbuf);
		 break;

       case OCI_NO_DATA_FOUND:
        elog(NOTICE, "srbC_ORA_open: Object does not exist\n");
		err_report(&stateInfo->cda, &stateInfo->lda);
		oclose(&stateInfo->cda); ologof(&stateInfo->lda);
		mm_delete(stateInfo); free(stateInfo);
        return ORA_ERR_DATA_FETCH;

	   default:
		 /* other value indicates error */
		 err_report(&stateInfo->cda, &stateInfo->lda);
		 mm_delete(stateInfo);
         oclose(&stateInfo->cda); ologof(&stateInfo->lda);
		 free(stateInfo);
		 return ORA_ERR_DATA_FETCH; 
	   }/*endswitch*/
   }/*	endwhile*/
 
 
  exfd->dbSpecificInfo = (char *)stateInfo;    

  return 0;
}



/* SRB CLOSE */ 

int srbC_ora_close(dbLobjDesc  *exfd)
{
  or_db_state_info *stateInfo;
  text updateStr[256];
  char bindvar[256];
  text msg[512];
  ub4    plsqltable;
  ub2   col_rcode;
  const ub4 piecesize = BUF - 1;
  ub4 len_longbuf  = BUF,  iteration;
  text longbuf[BUF];
  ub1 piece;
  ub1 cont  = 1;
  char  *context = "context pointer"; /* dummy value */
  int error = 0;

  if (exfd->dbSpecificInfo == NULL)
	{	elog (NOTICE, "srbC_ora_close: NULL value passed as object\n");
	return -1;
	}


  if (exfd->dbSystemType != ORA_LOBJ)
	{	elog (NOTICE, "srbC_ora_close: Bad ORA_LOBJ Type: %i\n",
				 exfd->dbSystemType);
	return ORA_ERR_OBJ_DESC;
	}

  stateInfo = (or_db_state_info *) exfd->dbSpecificInfo;
  stateInfo->offset = 0; /* go to the beg. of the file */


  if(stateInfo->dirty == 'T') {
	/* A write was performed */
	/* do an insert of the data and commit */

    sprintf(bindvar, ":%s", stateInfo->outColName);

	sprintf((char *) updateStr, "update %s set %s = %s where %s",
			 stateInfo->tableName, stateInfo->outColName,
      		 bindvar, stateInfo->inCondition);

	if (oparse(&stateInfo->cda, (text*)updateStr, 
			   (sb4) -1, FALSE, (ub4) VERSION_7))
	  {
		elog (NOTICE, "srbC_ORA_close: Error Parsing statement.\n");
		err_report(&stateInfo->cda, &stateInfo->lda);
		oclose(&stateInfo->cda); ologof(&stateInfo->lda);
		mm_delete(stateInfo); free(stateInfo);
   
		return ORA_ERR_SQLSTMT_PARSE;
	  }

	/* do the binds and stuff */

	if (obindps(&stateInfo->cda, 0, (text *) bindvar,
			-1 , (ub1 *)context, (ub4) ((uint)MAX_LONG_SIZE - 1) , SQLT_LNG, 
			(sword)0, (sb2 *)0, (ub2 *)0, &col_rcode, 0, 0, 0, 0, 0, 
			(ub4 *)0, (text *)0, 0, 0))
	  {
        elog (NOTICE, "srbC_ORA_close: Error Binding statement.\n");
		err_report(&stateInfo->cda, &stateInfo->lda);
		oclose(&stateInfo->cda); ologof(&stateInfo->lda);
		mm_delete(stateInfo); free(stateInfo);
		return ORA_ERR_DATATYPE_BIND;
	  }

	while (cont)
	  {
  
		oexec(&stateInfo->cda);

		switch (stateInfo->cda.rc)
		  {
		  case 0:  /*  operation is finished  */
            cont = 0;
			break;
			
		  case OCI_MORE_INSERT_PIECES:       /* ORA-03129 was returned  */

			  if(stateInfo->lobsize >= len_longbuf) {
                len_longbuf = len_longbuf;
                stateInfo->lobsize -= len_longbuf;
			  }              
              else {
                len_longbuf = stateInfo->lobsize;
              }

              memcpy(longbuf, stateInfo->mm_ptr + stateInfo->offset, 
					 len_longbuf);

			  stateInfo->offset += len_longbuf;

			  if(len_longbuf < piecesize) {
				/* zero out the end of the longbuf */ 
                longbuf[len_longbuf] = '\0';
			  }

			  ogetpi(&stateInfo->cda, &piece, (dvoid **)&context, &iteration,
					 &plsqltable);

			  if (len_longbuf < piecesize) 
				piece = OCI_LAST_PIECE; 
  			
			  
			  osetpi(&stateInfo->cda, piece, longbuf, &len_longbuf);
			  break;

		  default:
            /* print an error here */ 
			err_report(&stateInfo->cda, &stateInfo->lda);
			return ORA_ERR_PIECEWISE_INSERT;

		  }   /* switch  */

	  } /*  while  */

	/* commit the changes */ 
	ocom(&stateInfo->lda);

  }/*ifdirtyistrue*/

  /* delete the memory map */  
  if(stateInfo->mm_ptr != NULL)
	mm_delete(stateInfo);

   
  if (oclose(&stateInfo->cda))
    {   
	 
	  err_report(&stateInfo->cda, &stateInfo->lda);
	  elog(NOTICE, "srbC_ora_close:Error on closing cursor:\n[%s]\n",
			  msg);
	  error++;
    }

  if (ologof(&stateInfo->lda))
    { 
	  err_report(&stateInfo->cda, &stateInfo->lda);
	  elog(NOTICE, "srbC_ora_close:Error on disconnect:\n[%s]\n",
			  msg);
	  error++;
    }

    if(error) {
	  free(stateInfo);
	  return -1;
    }
    else {
	  free(stateInfo);
	  return 0;
	}
}/* srbclose */



int srbC_ora_read(dbLobjDesc *exfd, char *buffer, int length)
{
      or_db_state_info *stateInfo;
	  if (exfd->dbSpecificInfo == NULL)
		{	
		  elog (NOTICE, 
				   "srbC_ora_read: NULL value passed as object\n");
		  return ORA_ERR_NONEXISTENT_OBJ;
		}

	  if (exfd->dbSystemType != ORA_LOBJ){
		  elog(NOTICE, "srbC_ora_read: Bad ORA_LOBJ Type: %i\n",
	      exfd->dbSystemType);
	  }

	  stateInfo = (or_db_state_info *) exfd->dbSpecificInfo; 

	  if(length < 1 || stateInfo->offset >= stateInfo->lobsize) {
        return -1;

  	  } else {

        if(length > (stateInfo->lobsize - stateInfo->offset) + 1)
/* XXXX - get ride of one extra byte 
		  length = stateInfo->lobsize - stateInfo->offset + 1;
*/
		  length = stateInfo->lobsize - stateInfo->offset;

		memcpy(buffer, stateInfo->mm_ptr + stateInfo->offset -1, length);
		
		stateInfo->offset += length;
	  }

      /* return the stateInfo back to its previous state */ 
	  exfd->dbSpecificInfo = (char *)stateInfo;

      return length;

}/* srb_ora_read */













int srbC_ora_seek(dbLobjDesc *exfd, int offset, int whence)
{
  or_db_state_info *stateInfo;

  if (exfd->dbSpecificInfo == NULL)
	{	
	  elog (NOTICE, 
			   "srbC_ora_seek: NULL value passed as object\n");
	  return ORA_ERR_NONEXISTENT_OBJ;
	}

  if (exfd->dbSystemType != ORA_LOBJ){
	elog(NOTICE, "srbC_ora_seek: Bad ORA_LOBJ Type: %i\n",
			exfd->dbSystemType);
	return ORA_ERR_OBJ_DESC;
  }

  stateInfo = (or_db_state_info *) exfd->dbSpecificInfo; 


  if(offset > 0)
    offset--;


  switch (whence)
	{
	case SEEK_SET:
	  /* The 1 is added because the internal representation
	   * starts with byte 1 when the UNIX implemnetation 
	   * starts with byte 0.
	   */

	  stateInfo->offset = offset + 1;
	  break;

	case SEEK_CUR:
	  stateInfo->offset = stateInfo->offset + offset;
	  break;

	case SEEK_END:
	  stateInfo->offset = stateInfo->lobsize + offset;
	  break;
	}

  if (stateInfo->offset < 1) 
	stateInfo->offset = 1;

  if (stateInfo->offset > stateInfo->lobsize)  
	stateInfo->offset = stateInfo->lobsize - 1;

  /* return the stateinfo to its previous state */ 
  exfd->dbSpecificInfo = (char *)stateInfo;

  /* Substact 1 because the internal representation starts with byte 1. */
  return  stateInfo->offset;
}


/* SRB write */ 
/* if (mm_ptr != NULL){
          seek(0);
   }
   write buffer to mm starting at offset for length bytes
   set dirty to 'T';
*/



int srbC_ora_write(dbLobjDesc *exfd, char *buffer, int length)
{
   or_db_state_info *stateInfo;

  if (exfd->dbSpecificInfo == NULL)
	{	
	  elog (NOTICE, 
			   "srbC_ora_write: NULL value passed as object\n");
	  return ORA_ERR_NONEXISTENT_OBJ;
	}

  if (exfd->dbSystemType != ORA_LOBJ){
	elog(NOTICE, "srbC_ora_write: Bad ORA_LOBJ Type: %i\n",
			exfd->dbSystemType);
       return ORA_ERR_OBJ_DESC;
  }

  if(length < 1)
     return -1;
  
  stateInfo = (or_db_state_info *) exfd->dbSpecificInfo; 
  
  /* write to the memory map */ 
  memcpy(stateInfo->mm_ptr + stateInfo->offset - 1, buffer,length);     

   /* Set the dirty bit to true */ 
  stateInfo->dirty = 'T';

  stateInfo->offset = stateInfo->offset + length;
  stateInfo->lobsize = stateInfo->offset;
  
  stateInfo->mm_ptr[stateInfo->lobsize-1] = '\0';
   

  /* return the stateinfo to its previous state */ 
  exfd->dbSpecificInfo = (char *)stateInfo;

  return length;


} /*srbwrite */

/* think about */

/* srbC_ora_create - Create a ORA large object.
 *
 * Input : char *resourceLocationName - format : host:database:instance.
 *         For Illustra, instance is not used.
 *         char *dataPathName - Format : tablename/outCol/inCondition
 */


int srbC_ora_create(dbLobjDesc *exfd, char *resourceLocationName, 
					char *dataPathName, char *uid, char *pwd)

{
  
  char buffer[] = " "; 
  char bindvar[STRING_LENMAX], tmpStr[STRING_LENMAX];
  char instance[STRING_LEN1],hostAddr[STRING_LEN1], database[STRING_LEN1];
  or_db_state_info *stateInfo;
  char columnSet[STRING_LENMAX], valueSet[STRING_LENMAX];

    if (mySplitStr(resourceLocationName, hostAddr, tmpStr, ':') == NULL) {
        elog (NOTICE, "srbC_ora_create: Error in resourceLocationName : %s",
        resourceLocationName);
        return INP_ERR_RES_FORMAT;
    }
    if (mySplitStr(tmpStr, database, instance, ':') == NULL) {
        elog (NOTICE, "srbC_ora_create: Error in resourceLocationName : %s",
        resourceLocationName);
        return INP_ERR_RES_FORMAT;
    }


    /* dataPathName must begin with a '/' */

    if (*dataPathName != '/')
        return DBLOBJ_PATH_NAME_ERROR;

    dataPathName ++;            /* skip the '/' */


  if (exfd->dbSystemType != ORA_LOBJ){
	elog(NOTICE, "srbC_ora_create: Bad ORA_LOBJ Type: %i\n",
			exfd->dbSystemType);
	return ORA_ERR_OBJ_DESC;
  }


    /* First allocate an area for state information */
  if((stateInfo = malloc(sizeof (or_db_state_info))) == NULL){
     elog(NOTICE, 
			 "srbC_ora_create: not enough memory to perform function\n");
     return ORA_ERR_MALLOC;
   }

  /* init the state info */   
  stateInfo->mm_ptr = NULL;
  stateInfo->dirty = 'F';
  stateInfo->offset = 1;

  /* zero out the Hda */
  memset(&stateInfo->hda, 0, HDA_SIZE);

  if (mySplitStr(dataPathName, stateInfo->tableName,tmpStr,'/') == NULL) 
	{
	  elog(NOTICE, "srbC_ORA_create: Illegal dataPathName format: %s",
			  dataPathName);
	  free(stateInfo);
	  return ORA_LOBJ_PATH_NAME_ERROR;
	}

  if (mySplitStr(tmpStr, stateInfo->outColName,
				  stateInfo->inCondition,'/') == NULL) 
	{
	  elog(NOTICE, "srbC_ORA_create: Illegal dataPathName format: %s",
			  dataPathName);
	  free(stateInfo);
	  return ORA_LOBJ_PATH_NAME_ERROR;
	}

  separateColVal(stateInfo->inCondition,  columnSet, valueSet);
  convertAnd(stateInfo->inCondition);

  /* ORACLE logon calls */ 

#ifdef ORACLE_CONNECT_TNS
 if (olog(&stateInfo->lda, (ub1*)stateInfo->hda, 
		  (text*) uid, -1, (text *) pwd, -1,
		  (text *) database, -1, OCI_LM_DEF))
   {  
	 elog (NOTICE, "srbC_ORA_create:  Connect Error\n");
	 err_report(&stateInfo->cda, &stateInfo->lda); 
	 free(stateInfo);
	 return ORA_ERR_LOGIN;
   }
#endif /* ORACLE_CONNECT_TNS */
#ifdef ORACLE_CONNECT_ONS
  sprintf(oracledblobsidbuffer,"ORACLE_SID=%s",database);
  putenv(oracledblobsidbuffer);
 if (olog(&stateInfo->lda, (ub1*)stateInfo->hda, 
		  (text*) uid, -1, (text *) pwd, -1,
		  (text *) 0, -1, OCI_LM_DEF))
   {  
	 elog (NOTICE, "srbC_ORA_create:  Connect Error\n");
	 err_report(&stateInfo->cda, &stateInfo->lda); 
	 free(stateInfo);
	 return ORA_ERR_LOGIN;
   }
#endif /* ORACLE_CONNECT_ONS */


  /* turn auto commit off */
  ocof(&stateInfo->lda);

  /* open the cursor */
  if (oopen(&stateInfo->cda, &stateInfo->lda, 
			(text *) 0, -1, -1, (text *) 0, -1))
	{
	  elog (NOTICE, "srbC_ORA_create: Error opening cursor.\n");
	  err_report(&stateInfo->cda, &stateInfo->lda);
	  ologof(&stateInfo->lda);
	  free(stateInfo);
	  return ORA_ERR_OPEN_CURSOR;
	}

  sprintf(bindvar, ":%s",stateInfo->outColName);

  sprintf(queryString,
		  "insert into %s ( %s %s) values (%s %s )",
		  stateInfo->tableName, columnSet, stateInfo->outColName, 
		  valueSet, bindvar);
  elog (NOTICE, "stmt: %s\n columnSet:%s\n outColName: %s\n valueSet%s\n", 
		queryString, columnSet, stateInfo->outColName, valueSet );
  if (oparse(&stateInfo->cda, (text*)queryString, 
			 (sb4) -1, FALSE, (ub4) VERSION_7))
	{
	  elog (NOTICE, "srbC_ORA_create: Error Parsing statement.\n");
	  err_report(&stateInfo->cda, &stateInfo->lda);
	  oclose(&stateInfo->cda); ologof(&stateInfo->lda);
	  free(stateInfo);
   
	  return ORA_ERR_SQLSTMT_PARSE;
	}




  if (obndrv(&stateInfo->cda, (text *)bindvar, -1, (text *)buffer, -1, 
			 VARCHAR2_TYPE, -1, (sb2 *) 0, (text *) 0, -1, -1))
	{
	  elog (NOTICE, "srbC_ORA_create: Error Binding statement.\n");
	  err_report(&stateInfo->cda, &stateInfo->lda);
	  oclose(&stateInfo->cda); ologof(&stateInfo->lda);
	  free(stateInfo); 
	  return ORA_ERR_DATATYPE_BIND;
	}


  if (oexec(&stateInfo->cda)) {   
	elog(NOTICE, "srbC_ORA_create:Error in executing SQL stmt\n");    
	err_report(&stateInfo->cda, &stateInfo->lda);
	oclose(&stateInfo->cda); ologof(&stateInfo->lda);
	free(stateInfo);
	return ORA_ERR_OEXEC; 
  }	
 

  if(stateInfo->cda.rpc < 1) {
    elog(NOTICE, "srbC_ORA_create:No rows were created\n"); 
	oclose(&stateInfo->cda); ologof(&stateInfo->lda);
	free(stateInfo);
  }

  /* commit the changes */ 
  ocom(&stateInfo->lda);


 /* init a memory map and all that stuff */ 

  if(!mm_init(stateInfo, stateInfo->outColName)){
	elog(NOTICE, 
			"srbC_ORA_create: Could not create buffer for file creation\n");
	oclose(&stateInfo->cda); ologof(&stateInfo->lda); 
	free(stateInfo);
	return ORA_MMAP_CREATE_ERROR;
  } 

  /* return ptr to state information for this connection */
  exfd->dbSpecificInfo  = (char *)stateInfo; 

  return 0;

} /* srbcreate */ 





















/* delete */

/* srbC_ora_unlink - Delete a ORA large object.
 *
 * Input : char *resourceLocationName - format : host:database:instance.
 *         For Illustra, instance is not used.
 *         char *dataPathName - Format : tablename/objectID
 */

int srbC_ora_unlink(char *resourceLocationName, 
                  char *dataPathName, char *uid, char *pwd)

{
    char tmpStr[STRING_LENMAX];
    char instance[STRING_LEN1],hostAddr[STRING_LEN1], database[STRING_LEN1];
    or_db_state_info *stateInfo;
    char  msg[BUF];

    if (mySplitStr(resourceLocationName, hostAddr, tmpStr, ':') == NULL) {
        elog (NOTICE, "srbC_ora_unlink: Error in resourceLocationName : %s",
        resourceLocationName);
        return INP_ERR_RES_FORMAT;
    }
    if (mySplitStr(tmpStr, database, instance, ':') == NULL) {
        elog (NOTICE, "srbC_ora_unlink: Error in resourceLocationName : %s",
        resourceLocationName);
        return INP_ERR_RES_FORMAT;
    }

    /* dataPathName must begin with a '/' */

    if (*dataPathName != '/')
        return DBLOBJ_PATH_NAME_ERROR;

    dataPathName ++;            /* skip the '/' */

    /* First allocate an area for state information */
    if((stateInfo = malloc(sizeof(or_db_state_info))) == NULL) {
	  elog (NOTICE, "srbC_ora_unlink:Malloc Error");
      return -1;
	}

	if (mySplitStr(dataPathName, stateInfo->tableName,tmpStr,'/') == NULL) 
      {
		elog (NOTICE, "srbC_ora_unlink: Illegal dataPathName format: %s",
			  dataPathName);
        free(stateInfo);
		return -1;
      }
    if (mySplitStr(tmpStr, stateInfo->outColName,
		    stateInfo->inCondition,'/') == NULL) 
      {
		elog (NOTICE, "srbC_ora_unlink: Illegal dataPathName format: %s",
			  dataPathName);
        free(stateInfo);
		return -1;
      }
    convertAnd(stateInfo->inCondition);

    /* zero out the Hda */
        memset(&stateInfo->hda, 0, HDA_SIZE);

#ifdef ORACLE_CONNECT_TNS
 if (olog(&stateInfo->lda, (ub1*)stateInfo->hda, 
		  (text*) uid, -1, (text *) pwd, -1,
		  (text *) database, -1, OCI_LM_DEF))
   {  
	 elog (NOTICE, "srbC_ORA_unlink:  Connect Error\n");
	 err_report(&stateInfo->cda, &stateInfo->lda); 
	 free(stateInfo);
	 return ORA_ERR_LOGIN;
   }
#endif /* ORACLE_CONNECT_TNS */
#ifdef ORACLE_CONNECT_ONS
  sprintf(oracledblobsidbuffer,"ORACLE_SID=%s",database);
  putenv(oracledblobsidbuffer);
 if (olog(&stateInfo->lda, (ub1*)stateInfo->hda, 
		  (text*) uid, -1, (text *) pwd, -1,
		  (text *) 0, -1, OCI_LM_DEF))
   {  
	 elog (NOTICE, "srbC_ORA_unlink:  Connect Error\n");
	 err_report(&stateInfo->cda, &stateInfo->lda); 
	 free(stateInfo);
	 return ORA_ERR_LOGIN;
   }
#endif /* ORACLE_CONNECT_ONS */


	/* turn auto commit off */
	ocof(&stateInfo->lda);

	/* open the cursor */
	if (oopen(&stateInfo->cda, &stateInfo->lda, 
			  (text *) 0, -1, -1, (text *) 0, -1))
	  {
		elog (NOTICE, "srbC_ORA_unlink: Error opening cursor.\n");
		err_report(&stateInfo->cda, &stateInfo->lda);
		ologof(&stateInfo->lda);
		free(stateInfo);
		return ORA_ERR_OPEN_CURSOR;
	  }
	/* create and parse the stmt */ 
	sprintf(queryString, "delete from %s where %s",
			stateInfo->tableName, stateInfo->inCondition);

 if (oparse(&stateInfo->cda, (text*)queryString, 
			(sb4) -1, FALSE, (ub4) VERSION_7))
   {
     elog (NOTICE, "srbC_ORA_unlink: Error Parsing statement.\n");
	 err_report(&stateInfo->cda, &stateInfo->lda);
     ologof(&stateInfo->lda);
     free(stateInfo);
     return ORA_ERR_SQLSTMT_PARSE;
   }

 /* Execute the Delete Statement */ 
 if (oexec(&stateInfo->cda)) { 
   elog (NOTICE, "srbC_ORA_unlink: Error Executing Stmt.\n");      
   err_report(&stateInfo->cda, &stateInfo->lda);
   free(stateInfo);
   return ORA_ERR_OEXEC; 
 }	
 
 /* check the number of rows processed  
 if(stateInfo->cda.rpc < 1) {
    ; 
 }
 */

 /* commit the changes */ 
 ocom(&stateInfo->lda);


 if (oclose(&stateInfo->cda))
   {   
	 elog(NOTICE, "srbC_ora_unlink:Error closing cursor\n");
	 oerhms(&stateInfo->lda, stateInfo->cda.rc, (text *)msg, (sword) sizeof msg);
	 elog(NOTICE, "srbC_ora_unlink:Error on closing cursor:\n[%s]\n",
			 msg);
	 free(stateInfo);
    	 return ORA_ERR_CLOSE_CURSOR; 
   }

 if (ologof(&stateInfo->lda))
   {   oerhms(&stateInfo->lda, stateInfo->lda.rc, (text *)msg, (sword) sizeof msg);
   elog(NOTICE, "srbC_ora_unlink:Error on disconnect:\n[%s]\n",
		   msg);
   free(stateInfo);
   return -1;

   }

  free(stateInfo);
  return 0;
}
#undef ORACLE_DRIVER
#endif 	/* DB_Oracle */

