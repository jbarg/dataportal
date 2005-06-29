/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
#ifndef SYB_DVR_LIB_H
#define SYB_DVR_LIB_H

#define MAXDATATYPELISTSIZE 11

static short int sybDataTypeCode[] = {
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
static char *sybDataTypeName[] = {
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


#endif /* SYB_DVR_LIB_H */
