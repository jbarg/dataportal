/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 

#include <mi.h> 
#include "dbLobj.h" 
#include "srbMiscExtern.h" 
/* #include "mdas_errno.h" */

/* 
 These are 'driver' routines used by the SRB to provide file i/o
 access (open, read, write, seek, close, create, delete) to a large
 object in an Illustra database.  Under development.  Wayne Schroeder 

 "obj_id" is the field name in the table that is used to identify the
 object.  "obj_locator" is the Illustra Handle (a 'random' string) used to
 access the object.  These field names are imbedded in some strings in 
 this source (but could be easily changed).  This table display shows 
 how they look:

 sage 83% msql
 Connected to schroede on connection 0
 
 * create table srbVault (obj_id text, obj_locator large_object);

 * select * from srbVault;
 ------------------------------
 |obj_id       |obj_locator    |
 ------------------------------
 |test1        |I0107817598377|
 ------------------------------
 one row selected
  
 * 

 In Illustra (as in DB2), writes are not allowed to existing large objects. 
 To work around this, we copy the large object to an external object, write
 to that, and then replace it in the database.  The copy is done on the
 first write and the replace is done in the close.

 For a create, however, Illustra does allow direct writing to the large
 object until that connection is closed.  So in this case, we don't
 need to make the external object.  

 Since Illustra large objects are write only though, users will get an
 error if they create, write, and then try to read.

 The obj_id field is contrained to be unique so if a user tries to
 create an object with an existing obj_id, they get an error.

 For completeness, I added the delete routine which just deletes the row.

 An illustra 'vacuum' command might be appropriate after large objects
 are updated or deleted, but I have not included it.  This would remove
 historical data and non-referenced large objects (which will accumulate
 with each large object update), but can be run only by the owner of the
 table or DBA.

 Error returns are negative, different values for different errors.
 See the code for the meaning of each.

 compile execute line:
 cc -I/misc/illustra/h -o a.out illustraDB.c -ldl -lsocket 
     -lnsl -L/misc/illustra/lib -lmi 

*/


#define ILLUS_LOBJ_ID_COL_NAME "obj_id"
#define ILLUS_LOBJ_COL_NAME "obj_locator"
#define STRING_LENMAX 2000

typedef struct {
    MI_CONNECTION  *conn;
    MI_LODESC largeObjDesc;
    char LOHandle[32];
    MI_LODESC externObjDesc;
    char externLOHandle[32];
    MI_LODESC currentObjDesc;  /* this is either the external obj or the
                                  regular large object, whichever is being
                                  currently used */
    int writeFlag;   /* set to 0 if only opened or read;
                set to 1 if an external object has been created (written to);
                set to 2 if this is a created object as so written directly  */
    char tableName[STRING_LEN1];
    char objectName[STRING_LEN1];
    char outColName[STRING_LENMAX];
    char inCondition[STRING_LENMAX];
} illus_state_info;

illus_state_info *illusSateInfo;

void
separateColVal(char *inval, char *columnSet, char *valueSet)
{
  int i,j,k,l,m;
  i = 0; j = 0; k = 0; m = 0;
  l = strlen(inval);
  for (i = 0; i < l ; i++)
    {
      if (inval[i] == '\0') break;
      if (inval[i] == ' ' && inval[i-1] != '\\') continue;      
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
    int i,j,l;
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
	    

/*
 * Open an existing large object
 *
 * Input : char *resourceLocationName - format : host:database:instance.
 *	   For Illustra, instance is not used.
 *         char *dataPathName - Format : tablename/outCol/inCol=objectID
 *                          or tabname/outCol/inCol1=val1&...&inColn=valn
 */

int illustraOpen(dbLobjDesc *exfd, char *resourceLocationName, 
                   char *dataPathName, char *user, char *pwd)
{
    MI_CONNECTION_INFO cinfo;
    MI_DATABASE_INFO dbinfo;
    mi_integer milog;
    char cmd[STRING_LEN1];

    int error;
    MI_ROW         *row;
    int            collen;
    char           *colval;
    int  result, rows;
    char instance[STRING_LEN1],hostAddr[STRING_LEN1];
    char database[STRING_LEN1], tmpStr[STRING_LEN1];

    /* dataPathName must begin with a '/' */

    if (*dataPathName != '/')
        return DBLOBJ_PATH_NAME_ERROR;

    dataPathName ++;            /* skip the '/' */

    /* allocate an area for state information */
    illusSateInfo = malloc(sizeof (illus_state_info));

    if (mySplitStr(resourceLocationName, hostAddr, tmpStr, ':') == NULL) {
	elog (NOTICE, "illustraOpen: Error in resourceLocationName : %s",
	resourceLocationName);
	return INP_ERR_RES_FORMAT;
    }
    if (mySplitStr(tmpStr, database, instance, ':') == NULL) {
        elog (NOTICE, "illustraOpen: Error in resourceLocationName : %s",
        resourceLocationName);
        return INP_ERR_RES_FORMAT;
    }

    /*mySplitStr(dataPathName, illusSateInfo->tableName, 
                illusSateInfo->objectName,'/');*/

    if (mySplitStr(dataPathName, illusSateInfo->tableName,tmpStr,'/') == NULL) 
      {
	elog (NOTICE, "srbC_illustra_open: Illegal dataPathName format: %s",
	      dataPathName);
        free(illusSateInfo);return(DBLOBJ_PATH_NAME_ERROR);
      }
    if (mySplitStr(tmpStr, illusSateInfo->outColName,
		    illusSateInfo->inCondition,'/') == NULL) 
      {
	elog (NOTICE, "srbC_illustra_open: Illegal dataPathName format: %s",
	      dataPathName);
        free(illusSateInfo);return(DBLOBJ_PATH_NAME_ERROR);
      }
    convertAnd(illusSateInfo->inCondition);

    /* assign values to the connection info */
    cinfo.server_name = hostAddr;
    cinfo.server_host = hostAddr;
    cinfo.server_port = 0;
    cinfo.language = (char *)0;
    cinfo.encoding = (char *)0;

    illusSateInfo->conn = mi_server_connect( &cinfo );

    dbinfo.database_name = database;
    if (user == NULL || user[0] == '\0')
	dbinfo.user_name = ThisUser->userName;
    else 
    	dbinfo.user_name = user;
    dbinfo.password = (char *)pwd;     /* for testing */

    milog = mi_login(illusSateInfo->conn,&dbinfo);;
    if (milog == MI_ERROR) {
        return ILLUS_ERR_LOGIN;
    }
    sprintf(cmd,"select %s from %s where %s; ",
	    illusSateInfo->outColName, illusSateInfo->tableName, 
	    illusSateInfo->inCondition);
    if (mi_exec(illusSateInfo->conn, cmd, 0) == MI_ERROR)  {
        return ILLUS_ERR_MI_EXEC;
    }

    /* get query results */
    rows = 0;
    while ((result = mi_get_result(illusSateInfo->conn)) != MI_NO_MORE_RESULTS)  {
        switch(result)  {
        case MI_ERROR:
            return ILLUS_ERR_RESULT;
        case MI_ROWS:
            row = mi_next_row(illusSateInfo->conn, &error);
            if (row == NULL) {
                return ILLUS_ERR_NEXT_ROW;
            }
            if (mi_value(row, 0, &colval, &collen) != MI_NORMAL_VALUE)
            {
                return ILLUS_ERR_VALUE;
            }
            break;
        case MI_DDL:
            return ILLUS_ERR_MI_DML;
        case MI_DML:
	     rows = mi_result_row_count(illusSateInfo->conn);
            break;
        default:
            return ILLUS_ERR__RESULT_TYPE;
        }
    }
    if (rows != 1) return ILLUS_ERR_NUM_ROW;

    /* make sure query finished */
    if (mi_query_finish(illusSateInfo->conn) == MI_ERROR)  {
        return ILLUS_ERR_FINISH;
    }

    /* open the object */
    strcpy(illusSateInfo->LOHandle,colval);
    illusSateInfo->largeObjDesc = mi_large_object_open(illusSateInfo->conn,
                           illusSateInfo->LOHandle, MI_LO_RDONLY);
    if (illusSateInfo->largeObjDesc == MI_ERROR) return ILLUS_ERR_OBJECT_TYPE;

    illusSateInfo->currentObjDesc = illusSateInfo->largeObjDesc; 

    illusSateInfo->writeFlag = 0;   /* has not been written to yet */

    exfd->dbSpecificInfo 
       = (char *)illusSateInfo;     /* return ptr to state information
                                   for this connection */

    exfd->fd = 1;              /* just for tests in layers above, not used */
    return 0;
}

/*
 read from the object
 */
int illustraRead(dbLobjDesc *exfd, char *buffer, int length)
{
    /* set the pointer to the state information for this connection & obj */
    illusSateInfo = (illus_state_info *) exfd->dbSpecificInfo; 


    return(mi_large_object_read(illusSateInfo->conn,
            illusSateInfo->currentObjDesc, buffer, length));
    /* returns length or read on success and -1 on error */
}

/*
 write to the object
 */
int illustraWrite(dbLobjDesc *exfd, char *buffer, int length)
{
    char *LOH;
    int wval;
    int pos;

    /* set the pointer to the state information for this connection & obj */
    illusSateInfo = (illus_state_info *) exfd->dbSpecificInfo; 

    /* if this is the first write on an this (non-created) Obj, we first have
       to make it an external object and open that */
    if (illusSateInfo->writeFlag == 0)
    {
        pos = mi_large_object_seek(illusSateInfo->conn, illusSateInfo->largeObjDesc,
                   0, MI_LO_SEEK_CUR);  /* save current position for new obj*/
        if (pos < 0) return ILLUS_ERR_SEEK;

        LOH = mi_large_object_copy(illusSateInfo->conn, illusSateInfo->LOHandle,
              "extern1", MI_LO_EXTERNAL_FILE,0777,MI_LO_DEFAULT_SMGR,0);
        if (LOH==0) return ILLUS_ERR_COPY;
        strcpy(illusSateInfo->externLOHandle,LOH);
  
        illusSateInfo->externObjDesc = 
            mi_large_object_open(illusSateInfo->conn, 
                                 illusSateInfo->externLOHandle, MI_LO_RDWR);

        if (illusSateInfo->externObjDesc == MI_ERROR) {
            return ILLUS_ERR_OBJ_DESC;
        }

        pos = mi_large_object_seek(illusSateInfo->conn, illusSateInfo->externObjDesc,
                           pos, MI_LO_SEEK_CUR);  /* set current position */
        if (pos < 0) return ILLUS_ERR_SEEK;

        illusSateInfo->writeFlag=1;
        
        illusSateInfo->currentObjDesc = illusSateInfo->externObjDesc;
    }

    wval = mi_large_object_write(illusSateInfo->conn,
/* gcc complained to many arguement
         illusSateInfo->currentObjDesc,  buffer, length, 0, MI_LO_SEEK_CUR);
*/
         illusSateInfo->currentObjDesc,  buffer, length);
  
    return wval;
}       

/*
 close the object and connection 
 */
int illustraClose(dbLobjDesc  *exfd)
{
    char *LOH;
    char cmd[STRING_LEN1];
    int  result;
    int  rows;

    /* set the pointer to the state information for this connection & obj */
    illusSateInfo = (illus_state_info *) exfd->dbSpecificInfo; 

    mi_large_object_close(illusSateInfo->conn, 
                     illusSateInfo->largeObjDesc); /* (func always returns 0) */

    /* If we have written to the object, then we need to move the extern
       object that we have updated into the DB as a large object and 
       update the table that refers to it with the new handle (idstring). */

    if (illusSateInfo->writeFlag == 1 ) {

        mi_large_object_close(illusSateInfo->conn, 
                              illusSateInfo->currentObjDesc); 

        LOH = mi_large_object_copy(illusSateInfo->conn, illusSateInfo->externLOHandle,
/*
              "", MI_LO_INTERNAL_LO,0,MI_LO_DEFAULT_SMGR,0,0);
*/
              "", MI_LO_INTERNAL_LO,0,MI_LO_DEFAULT_SMGR,0);
        if (LOH==0) return ILLUS_ERR_COPY;

	/*        sprintf(cmd,"update %s set %s='%s' where %s = '%s';",
            illusSateInfo->tableName, ILLUS_LOBJ_COL_NAME, 
		LOH, ILLUS_LOBJ_ID_COL_NAME, illusSateInfo->objectName); */
	sprintf(cmd,"update %s set %s='%s' where %s;",
		illusSateInfo->tableName, 
		illusSateInfo->outColName, LOH,
		illusSateInfo->inCondition);

        if (mi_exec(illusSateInfo->conn, cmd, 0) == MI_ERROR)  {
            fprintf(stderr, "cannot send: %s\n", cmd);
            return ILLUS_ERR_MI_EXEC;
        }

        /* get exec results */
	rows=0;
        while ((result = mi_get_result(illusSateInfo->conn))
                != MI_NO_MORE_RESULTS)  {
            switch(result)  {
                case MI_ERROR:
                     return ILLUS_ERR_RESULT;
                case MI_ROWS:
                     return ILLUS_ERR__RESULT_TYPE;
                case MI_DDL:
                     break;
                case MI_DML:
		     rows = mi_result_row_count(illusSateInfo->conn);
                     break;
                default:
                    return ILLUS_ERR__RESULT_TYPE;
            }
        }
	if (rows != 1) return ILLUS_ERR_NUM_ROW;
    }

    free(illusSateInfo);

    return 0;
}

/*
 seek to a position in the object
 */
int illustraSeek(dbLobjDesc *exfd, int offset, int whence)
{
    int mi_seek;

    /* set the pointer to the state information for this connection & obj */
    illusSateInfo = (illus_state_info *) exfd->dbSpecificInfo; 

    mi_seek = mi_large_object_seek(illusSateInfo->conn, illusSateInfo->currentObjDesc,
            offset, whence);

    return (mi_seek);
}

/*
 create a new object and open it for writing
 *
 * Input : char *resourceLocationName - format : host:database:instance.
 *         For Illustra, instance is not used.
 *         char *dataPathName - Format : tablename/objectID
 *
 */

int illustraCreate(dbLobjDesc *exfd, char *resourceLocationName, 
                  char *dataPathName, char *user, char *pwd)
{
    MI_CONNECTION_INFO cinfo;
    MI_DATABASE_INFO dbinfo;
    mi_integer milog;
    char cmd[STRING_LEN1];
    char *LOH;

    int  result, rows;
    char           instance[STRING_LEN1],hostAddr[STRING_LEN1];
    char database[STRING_LEN1], tmpStr[STRING_LEN1];
    char columnSet[STRING_LENMAX], valueSet[STRING_LENMAX];

    /* dataPathName must begin with a '/' */

    if (*dataPathName != '/')
        return DBLOBJ_PATH_NAME_ERROR;

    dataPathName ++;            /* skip the '/' */

    /* allocate an area for state information */
    illusSateInfo = malloc(sizeof (illus_state_info));

    if (mySplitStr(resourceLocationName, hostAddr, tmpStr, ':') == NULL) {
        elog (NOTICE, "illustraOpen: Error in resourceLocationName : %s",
        resourceLocationName);
        return INP_ERR_RES_FORMAT;
    }
    if (mySplitStr(tmpStr, database, instance, ':') == NULL) {
        elog (NOTICE, "illustraOpen: Error in resourceLocationName : %s",
        resourceLocationName);
        return INP_ERR_RES_FORMAT;
    }

    /*mySplitStr(dataPathName, illusSateInfo->tableName, 
                illusSateInfo->objectName,'/');*/
   if (mySplitStr(dataPathName, illusSateInfo->tableName,tmpStr,'/') == NULL) 
      {
	elog (NOTICE, "srbC_illustra_open: Illegal dataPathName format: %s",
	      dataPathName);
        free(illusSateInfo);return(DBLOBJ_PATH_NAME_ERROR);
      }
    if (mySplitStr(tmpStr, illusSateInfo->outColName,
		    illusSateInfo->inCondition,'/') == NULL) 
      {
	elog (NOTICE, "srbC_illustra_open: Illegal dataPathName format: %s",
	      dataPathName);
        free(illusSateInfo);return(DBLOBJ_PATH_NAME_ERROR);
      }
    separateColVal(illusSateInfo->inCondition,  columnSet, valueSet);
    convertAnd(illusSateInfo->inCondition);


    /* assign values to the connection info */
    cinfo.server_name = hostAddr;
    cinfo.server_host = hostAddr;
    cinfo.server_port = 0;
    cinfo.language = (char *)0;
    cinfo.encoding = (char *)0;

    illusSateInfo->conn = mi_server_connect( &cinfo );

    dbinfo.database_name = database;
    if (user == NULL || user[0] == '\0')
        dbinfo.user_name = ThisUser->userName;
    else
        dbinfo.user_name = user;
    dbinfo.password = (char *) pwd;     /* for testing */

    milog = mi_login(illusSateInfo->conn,&dbinfo);;
    if (milog == MI_ERROR) {
        return ILLUS_ERR_LOGIN;
    }

    /* create the object */
    LOH = mi_large_object_create(illusSateInfo->conn,0,
               MI_LO_INTERNAL_LO,0,MI_LO_DEFAULT_SMGR,MI_LO_DEFAULT_SMGR);
    if (LOH==0) return ILLUS_ERR_CREATE;
    strcpy(illusSateInfo->LOHandle,LOH);

    /* open it */
    illusSateInfo->largeObjDesc = mi_large_object_open(illusSateInfo->conn,
                           illusSateInfo->LOHandle, MI_LO_WRONLY);
    if (illusSateInfo->largeObjDesc == MI_ERROR) return ILLUS_ERR_OPEN;

    illusSateInfo->currentObjDesc = illusSateInfo->largeObjDesc; 

    /* insert it into the table */
    /* sprintf(cmd,"insert into %s (%s, %s) values ('%s', '%s');",
        illusSateInfo->tableName,ILLUS_LOBJ_ID_COL_NAME,
	    ILLUS_LOBJ_COL_NAME, illusSateInfo->objectName, 
	    illusSateInfo->LOHandle); */
	sprintf(cmd,"insert into %s ( %s %s) values (%s '%s');",
		illusSateInfo->tableName, 
		columnSet, illusSateInfo->outColName, valueSet,
		illusSateInfo->LOHandle);
printf("CMD:%s\n",cmd);
    if (mi_exec(illusSateInfo->conn, cmd, 0) == MI_ERROR)  {
        return ILLUS_ERR_MI_EXEC;
    }

    /* get exec results */
    rows = 0;
    while ((result = mi_get_result(illusSateInfo->conn))
            != MI_NO_MORE_RESULTS)  {
        switch(result)  {
            case MI_ERROR:
                return ILLUS_ERR_RESULT;
            case MI_ROWS:
            case MI_DDL:
            case MI_DML:
	        rows = mi_result_row_count(illusSateInfo->conn);
                break;
            default:
                return ILLUS_ERR__RESULT_TYPE;
        }
    }
    if (rows != 1) return ILLUS_ERR_NUM_ROW;

    /* make sure query finished */
    if (mi_query_finish(illusSateInfo->conn) == MI_ERROR)  {
        return ILLUS_ERR_FINISH;
    }

    illusSateInfo->writeFlag = 2;   /* can be written directly */

    exfd->dbSpecificInfo 
       = (char *)illusSateInfo;     /* return ptr to state information
                                   for this connection */

    exfd->fd = 1;              /* just for tests in layers above, not used */

    return 0;
}

/*
 delete an object
 */
int illustraDelete(char *resourceLocationName, 
                  char *dataPathName, char *user, char *pwd)
{
    MI_CONNECTION  *conn;
    char tableName[STRING_LEN1];
    char objectName[STRING_LEN1];
    char outColName[STRING_LENMAX];
    char inCondition[STRING_LENMAX];

    MI_CONNECTION_INFO cinfo;
    MI_DATABASE_INFO dbinfo;
    mi_integer milog;
    char cmd[STRING_LEN1];
    int  result, rows;
    char           instance[STRING_LEN1],hostAddr[STRING_LEN1];
    char database[STRING_LEN1], tmpStr[STRING_LEN1];

    /* dataPathName must begin with a '/' */

    if (*dataPathName != '/')
        return DBLOBJ_PATH_NAME_ERROR;

    dataPathName ++;            /* skip the '/' */

    if (mySplitStr(resourceLocationName, hostAddr, tmpStr, ':') == NULL) {
        elog (NOTICE, "illustraDelete: Error in resourceLocationName : %s",
        resourceLocationName);
        return INP_ERR_RES_FORMAT;
    }
    if (mySplitStr(tmpStr, database, instance, ':') == NULL) {
        elog (NOTICE, "illustraDelete: Error in resourceLocationName : %s",
        resourceLocationName);
        return INP_ERR_RES_FORMAT;
    }

    /*mySplitStr(dataPathName, tableName, 
                objectName,'/'); */
   if (mySplitStr(dataPathName, tableName,tmpStr,'/') == NULL) 
      {
	elog (NOTICE, "srbC_illustra_open: Illegal dataPathName format: %s",
	      dataPathName);
        return(DBLOBJ_PATH_NAME_ERROR);
      }
    if (mySplitStr(tmpStr, outColName,
		    inCondition,'/') == NULL) 
      {
	elog (NOTICE, "srbC_illustra_open: Illegal dataPathName format: %s",
	      dataPathName);
	return(DBLOBJ_PATH_NAME_ERROR);
      }
    convertAnd(inCondition);



    /* assign values to the connection info */
    cinfo.server_name = hostAddr;
    cinfo.server_host = hostAddr;
    cinfo.server_port = 0;
    cinfo.language = (char *)0;
    cinfo.encoding = (char *)0;

    conn = mi_server_connect( &cinfo );

    dbinfo.database_name = database;
    dbinfo.user_name = user;
    dbinfo.password = (char *) pwd;     /* for testing */

    milog = mi_login(conn,&dbinfo);;
    if (milog == MI_ERROR) {
        return ILLUS_ERR_LOGIN;
    }

    /* remove it from the table */
    /*sprintf(cmd,"delete from %s where %s = '%s';",
                tableName, ILLUS_LOBJ_ID_COL_NAME, objectName);*/
   sprintf(cmd,"delete from %s where %s ;",
	    tableName, inCondition);


    if (mi_exec(conn, cmd, 0) == MI_ERROR)  {
        return ILLUS_ERR_MI_EXEC;
    }

    /* get exec results */
    rows = 0;
    while ((result = mi_get_result(conn))
            != MI_NO_MORE_RESULTS)  {
        switch(result)  {
            case MI_ERROR:
                return ILLUS_ERR_RESULT;
            case MI_ROWS:
            case MI_DDL:
            case MI_DML:
	        rows = mi_result_row_count(conn);
                break;
            default:
                return ILLUS_ERR__RESULT_TYPE;
        }
    }
    if (rows != 1) return ILLUS_ERR_NUM_ROW;

    /* make sure query finished */
    if (mi_query_finish(conn) == MI_ERROR)  {
        return ILLUS_ERR_FINISH;
    }

    return 0;
}

#ifdef TESTING
/*
 for testing

cc -DTESTING=1 -DDBLOBJ_PATH_NAME_ERROR=1234 -I/misc/illustra/h -I/projects/mdas/srb/SRB/src/include -I/projects/mdas/srb/SRB/src/catalog/include -o a.out ../../MCAT/solbin/globals.o illustraDB.c -ldl -lsocket -lnsl -L/misc/illustra/lib -lmi

 */
main(int argc, char *argv[])
{
    dbLobjDesc lObj;
    int i;
    char mydatapath[STRING_LENMAX];
    char myresource[STRING_LENMAX];
    char user[20];
    char passwd[20];

    char buf[50];
    if (argc != 2 ){
        printf("usage : a.out [1 2 3]\n");
        exit(1);
    }
    /*create table svaulttest(A text not null,B text not null,C text not null,D text not null,E large_object, unique (A,B,C,D));*/
    strcpy(myresource,"sage:sekar:0");
    strcpy(mydatapath,"svaulttest/E/A='1' & B='1' & C='1' & D='1'");
    strcpy(user,"sekar");
    strcpy(passwd,"alpha99");
    i=atoi(argv[1]);
    if (i==1)
    {
        i=illustraOpen(&lObj, myresource, mydatapath,user,passwd);

        printf("open=%d\n",i);
        if (i>=0) {
            i = illustraRead(&lObj,buf,10);
            printf("readval=%d\n",i);
            if (i>0 && i<11) {
                buf[i]='\0';
                printf("buf=%s\n",buf);
            }

            i = illustraClose(&lObj);
	    printf("close = %d\n",i);
        }
    }
    if (i==2)
    {
        i=illustraOpen(&lObj, myresource, mydatapath,user,passwd);
        printf("open=%d\n",i);
        if (i>=0) {

            i = illustraSeek(&lObj,3,SEEK_SET);
            printf("seekval=%d\n",i);
        
            for (i=0;i<10;i++) {buf[i]='E';}
            i = illustraWrite(&lObj,buf,5);
            printf("writevalue=%d\n",i);

            i = illustraClose(&lObj);
	    printf("close = %d\n",i);
        }
    }
    if (i==3)
    {
        i=illustraCreate(&lObj, myresource, mydatapath,user,passwd);
        printf("create=%d\n",i);
        if (i>=0) {

            for (i=0;i<10;i++) {buf[i]='F';}
            i = illustraWrite(&lObj,buf,10);
            printf("writevalue=%d\n",i);

            i = illustraClose(&lObj);
	    printf("close = %d\n",i);
        }
    }
    if (i==4)
    {
        i=illustraCreate(&lObj,  myresource, mydatapath,user,passwd);
        printf("create=%d\n",i);
        if (i>=0) {

            for (i=0;i<10;i++) {buf[i]='G';}
            i = illustraWrite(&lObj,buf,10);
            printf("writevalue=%d\n",i);

            i = illustraSeek(&lObj,0,SEEK_SET);
            printf("seekval=%d\n",i);

            /* this read fails because created objs are write only */
            i = illustraRead(&lObj,buf,10);
            printf("readval=%d\n",i);
            if (i>0 && i<11) {
                buf[i]='\0';
                printf("buf=%s\n",buf);
            }

            i = illustraClose(&lObj);
	    printf("close = %d\n",i);
        }
    }
    if (i==5)
    {
        i=illustraDelete( myresource, mydatapath,user,passwd);
        printf("delete=%d\n",i);
    }
}
#endif
