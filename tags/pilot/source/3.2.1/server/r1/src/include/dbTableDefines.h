/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/*-------------------------------------------------------------------------
 *
 * dbTableDefines.h 
 *
 *
 *-------------------------------------------------------------------------
 */

#ifndef DB_TABLE_DEFINES_H
#define DB_TABLE_DEFINES_H

#if defined(PORTNAME_osx)
#include <sys/types.h>
#include <sys/malloc.h>
#else
#include <malloc.h>
#endif
#include <stdio.h>
#include "c.h"
#include "srb.h"
#include <fcntl.h>      /* for most O_ */
#ifndef O_RDONLY
#include <sys/file.h>   /* for the rest O_ */
#endif /* O_RDONLY */
#ifndef _WIN32
#include <unistd.h>    
#endif

#include "elogExtern.h"
#include "dbTableExtern.h"
#include "srbMiscExtern.h"
#include "exfFunctExtern.h"
 
#define MAX_DB_TABLE_DESC 256
#define SVR_OBJ_READ_BUFFER_SIZE 8192
#define MAX_NUM_SVR_OBJ_READ_BUFFERS 8192


#define VAR_NAME_SIZE    50
#define VAR_VAL_SIZE    250
#define MAX_VAR_COUNT    30

#define MAX_TAG_SIZE  8192
#define MAX_CHOICE_COUNT 1024
#define MAX_TEMPLATE_SIZE HUGH_HUGE_STRING
#define MAX_VARIABLE_COUNT 1024
#define MAX_VARNAME_SIZE   256
#define MAX_FUNCTION_COUNT 64
#define MAX_COMPOSITE_SQL_COUNT 64
#define MAX_SQL_TAGS_COUNT 256

#define TABLE_CONFIG	"TableConfig"	/* the table config file */


#define TAG_FULL        1
#define TAG_CONSUMED    0
#define TAG_BAD         999

#define SYSTEM_OPEN_TAG        "%%%"
#define STANDARAD_OPEN_TAG     "***"
#define STANDARAD_CLOSE_TAG    ":"

#define DB2_PASSWD_KEYWD	"Db2Passwd"
#define DB2_USER_KEYWD		"Db2User"
#define ILLUS_PASSWD_KEYWD	"IllusPasswd"
#define ILLUS_USER_KEYWD	"IllusUser"
#define ORACLE_PASSWD_KEYWD	"OraclePasswd"
#define ORACLE_USER_KEYWD	"OracleUser"
#define INFORMIX_USER_KEYWD     "InformixUser"
#define INFORMIX_PASSWD_KEYWD   "InformixPasswd"
#define SYBASE_USER_KEYWD       "SybaseUser"
#define SYBASE_PASSWD_KEYWD     "SybasePasswd"
#define POSTGRES_USER_KEYWD     "PostgresUser"
#define POSTGRES_PASSWD_KEYWD   "PostgresPasswd"
#define MYSQL_USER_KEYWD        "MysqlUser"
#define MYSQL_PASSWD_KEYWD      "MysqlPasswd"

typedef struct {
  int   choiceCount;
  int   roundRobin;
  char *choiceCondition;
  char *choicePreStringCond[MAX_CHOICE_COUNT];
  char *choiceHead[MAX_CHOICE_COUNT];
  char *choiceBody[MAX_CHOICE_COUNT];
  char *choiceTail[MAX_CHOICE_COUNT];
  char *compiledHeadRE[MAX_CHOICE_COUNT];
  char *compiledTailRE[MAX_CHOICE_COUNT];
} styleChoice;

typedef struct {
  int   varCount;
  char  preString[MAX_VARNAME_SIZE];
  char *varName[MAX_VARIABLE_COUNT];
  char *varType[MAX_VARIABLE_COUNT];
  char *varValue[MAX_VARIABLE_COUNT];
}
varsList;

typedef struct {
  int   funcCount;
  char *funcName[MAX_FUNCTION_COUNT];
  char *funcArgList[MAX_FUNCTION_COUNT];
  char *funcBody[MAX_FUNCTION_COUNT];
}
funcsList;

typedef struct {
  int sqlCount;
  char *sql[MAX_COMPOSITE_SQL_COUNT];
  char *dataTagOpen;
  char *dataTagClose;
  char *tags[MAX_COMPOSITE_SQL_COUNT][MAX_SQL_TAGS_COUNT];
  char *dataValue[MAX_COMPOSITE_SQL_COUNT][MAX_SQL_TAGS_COUNT];
  int  tagusage[MAX_COMPOSITE_SQL_COUNT][MAX_SQL_TAGS_COUNT];
  char *preString[MAX_COMPOSITE_SQL_COUNT][MAX_SQL_TAGS_COUNT];
  char *postString[MAX_COMPOSITE_SQL_COUNT];
}
sqlList;

typedef struct {
  int         genericPtr;        /* use if needed for stuff not covered here */
  int         statement;         /* statement cursor */
  int         environ;           /* db environment */
  int         connect;           /* db connection handle */
  size_t mm_size;                /* sizeof rest of row that was not sent */
  char *mm_ptr;                  /* stores rest of row that was not sent */
  int  num_of_cols;
  char *colName[MAX_TABLE_COLS];
  char *data[MAX_TABLE_COLS];
  char *xmlcolName[MAX_TABLE_COLS];
  char *xmlcolNameOpen[MAX_TABLE_COLS];
  char *xmlcolNameClose[MAX_TABLE_COLS];
  long int             displaysize[MAX_TABLE_COLS];
  char                 coltype[MAX_TABLE_COLS][SMALL_TOKEN];
  short int            nullable[MAX_TABLE_COLS];
  unsigned long int    precision[MAX_TABLE_COLS];
  short int            scale[MAX_TABLE_COLS];
  int  data_size[MAX_TABLE_COLS];
  int  num_of_rows_read;
  char dataType[MAX_TOKEN];
  char sqlType[MAX_TOKEN];
  char *tableNameList;
  int  endOfData;
  char *lastData[MAX_TABLE_COLS];
  char *dataTemplateH;
  char *dataTemplateB;
  char *dataTemplateT;
  styleChoice dataTemplateChoice;
  char *sql;
  char *storedSQLBuffer;
  int  freestoredSQLBufferSize;
  int num_of_tables;
  varsList constantValues;
  varsList definedValues;
  funcsList userFuncs;
  sqlList compSql;
}tableStateInfo;

#endif	/* DB_TABLE_DEFINES_H */
