/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
#ifndef ORA_DVR_LIB_H
#define ORA_DVR_LIB_H

#ifdef PORTNAME_solaris
#include <ctype.h>
#endif

#include "srbMiscExtern.h"
#include "elogExtern.h"

#define MAXDATATYPELISTSIZE 9

static short int oraDataTypeCode[] = {
  0,             1,        2,       8,     11,     12,    23,      24,    
  96,     105      };
static char *oraDataTypeName[] = {
  "UNKNOWNTYPE", "VARCHAR","NUMBER","CLOB","ROWID","DATE","BINARY","BLOB",
  "CHAR", "MLSLABEL"};

int open_env_oraTab(long int *inhenv);
void  oraTableErrorReport( Cda_Def *cursor, Lda_Def *lda);
int connect_oraTab(long int inhenv,
                long int *inhdbc,
                SQLCHAR *databasename,
                SQLCHAR *user,
                SQLCHAR *password);
int close_env_oraTab(long int inhenv);
int
getColumnInfoFororaTable(long int  henv, long int  hdbc,
                      char *inTableName,
                      int *numOfColumns,
                      char *colName[],
                      char coltype[MAX_TABLE_COLS][SMALL_TOKEN],
                      long int displaysize[],
                      short int nullable[],
                      unsigned long int precision[],
                      short int   scale[]);
int disconnect_oraTab(long int inhenv,
                   long int inhdbc,
                   SQLUSMALLINT transstate);
int
getColTypeFromColNameoraTab(long int  henv, long int  hdbc,
                      char *colType, char *colName, char *inTableName);
int exec_no_res_sql_oraTab(long int inhenv,
                        long int inhdbc,
                        SQLCHAR *query);
int exec_sql_oraTab(long int inhenv,
                 long int inhdbc,
                 long int *inhstmt,
                 SQLCHAR *query);
int get_no_of_columns_oraTab(long int inhstmt,
                        SQLSMALLINT *num_of_cols);
int bind_default_row_oraTab(long int inhstmt,
                  SQLCHAR *data[],
                  SQLINTEGER data_size[],
                  SQLSMALLINT *num_of_cols);
int get_column_info_oraTab(long int inhstmt,
                        SQLSMALLINT  column_number,
                        SQLINTEGER *displaysize,
                        SQLCHAR      colname[],
                        SQLSMALLINT *coltype,
                        SQLSMALLINT buf_size,
                        SQLSMALLINT *colnamelen,
                        SQLSMALLINT *nullable,
                        SQLUINTEGER *precision,
                        SQLSMALLINT *scale
                        );
int done_with_default_result_oraTab(long int inhstmt,
                                  SQLCHAR *data[],
                                  SQLINTEGER data_size[],
                                  SQLSMALLINT num_of_cols);
void
getDataTypeFromMap_oraTab(char *typeName, short int   typeId);
int get_row_oraTab(long int inhstmt);

#endif /* ORA_DVR_LIB_H */
