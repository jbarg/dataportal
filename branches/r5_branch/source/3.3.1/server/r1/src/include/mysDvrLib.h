/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
#ifndef MYS_DVR_LIB_H
#define MYS_DVR_LIB_H

#define MAXDATATYPELISTSIZE 11

static short int mysDataTypeCode[] = {
SQL_UNKNOWN_TYPE,
SQL_CHAR         ,
SQL_NUMERIC      ,
SQL_DECIMAL      ,
SQL_INTEGER      ,
SQL_SMALLINT     ,
SQL_FLOAT        ,
SQL_REAL         ,
SQL_DOUBLE       ,
SQL_DATETIME     ,
SQL_VARCHAR      
};
static char *mysDataTypeName[] = {
"UNKNOWN_TYPE",
"CHAR"       ,
"NUMERIC"      ,
"DECIMAL"      ,
"INTEGER"      ,
"SMALLINT"     ,
"FLOAT"        ,
"REAL"         ,
"DOUBLE"       ,
"DATETIME"     ,
"VARCHAR"      
};

result_info* result_info_mys_head;


#endif /* MYS_DVR_LIB_H */
