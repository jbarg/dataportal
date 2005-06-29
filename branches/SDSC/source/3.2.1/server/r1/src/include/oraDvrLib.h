/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
#ifndef ORA_DVR_LIB_H
#define ORA_DVR_LIB_H


#define MAXDATATYPELISTSIZE 9

static short int oraDataTypeCode[] = {
  0,             1,        2,       8,     11,     12,    23,      24,    
  96,     105      };
static char *oraDataTypeName[] = {
  "UNKNOWNTYPE", "VARCHAR","NUMBER","CLOB","ROWID","DATE","BINARY","BLOB",
  "CHAR", "MLSLABEL"};

#endif /* ORA_DVR_LIB_H */
