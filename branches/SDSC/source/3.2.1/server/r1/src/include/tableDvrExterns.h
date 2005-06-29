/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
#ifndef TABLE_DVR_EXTERNS_H
#define TABLE_DVR_EXTERNS_H
 

#include "srbC_ora_externs.h"


extern int oraTableOpen(dbTableDesc   *exfd, 
		   char       *resourceLocationName, 
		   char       *sqlSelectQuery,
		   char       *uid, 
		   char       *pwd);

extern int oraTableClose(dbTableDesc  *exfd);

extern int oraTableRead(dbTableDesc *exfd, 
		  char     *buffer, 
		  int       length);
extern int oraTableWrite(dbTableDesc *exfd, 
		  char     *sqlInsertString, 
		  int       length);

extern int oraTableSeek(dbTableDesc *exfd, 
                  int      offset, 
                  int      whence);
extern int oraTableCreate(dbTableDesc   *exfd, 
                   char       *resourceLocationName, 
                   char       *tableProperties,
                   char       *uid, 
                   char       *pwd);
extern int oraTableUnlink(char    *resourceLocationName, 
                    char          *tableName,
                    char          *uid, 
                    char          *pwd);

extern int db2TableOpen(dbTableDesc   *exfd, 
		   char       *resourceLocationName, 
		   char       *sqlSelectQuery,
		   char       *uid, 
		   char       *pwd);

extern int db2TableClose(dbTableDesc  *exfd);

extern int db2TableRead(dbTableDesc *exfd, 
		  char     *buffer, 
		  int       length);
extern int db2TableWrite(dbTableDesc *exfd, 
		  char     *sqlInsertString, 
		  int       length);

extern int db2TableSeek(dbTableDesc *exfd, 
                  int      offset, 
                  int      whence);
extern int db2TableCreate(dbTableDesc   *exfd, 
                   char       *resourceLocationName, 
                   char       *tableProperties,
                   char       *uid, 
                   char       *pwd);
extern int db2TableUnlink(char    *resourceLocationName, 
                    char          *tableName,
                    char          *uid, 
                    char          *pwd);

extern int illusTableOpen(dbTableDesc   *exfd, 
		   char       *resourceLocationName, 
		   char       *sqlSelectQuery,
		   char       *uid, 
		   char       *pwd);

extern int illusTableClose(dbTableDesc  *exfd);

extern int illusTableRead(dbTableDesc *exfd, 
		  char     *buffer, 
		  int       length);
extern int illusTableWrite(dbTableDesc *exfd, 
		  char     *sqlInsertString, 
		  int       length);

extern int illusTableSeek(dbTableDesc *exfd, 
                  int      offset, 
                  int      whence);
extern int illusTableCreate(dbTableDesc   *exfd, 
                   char       *resourceLocationName, 
                   char       *tableProperties,
                   char       *uid, 
                   char       *pwd);
extern int oraTableUnlink(char    *resourceLocationName, 
                    char          *tableName,
                    char          *uid, 
                    char          *pwd);
#endif /*TABLE_DVR_EXTERNS_H*/
