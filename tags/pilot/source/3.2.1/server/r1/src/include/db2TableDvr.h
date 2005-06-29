/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
#ifndef DB2_TABLE_DVR_H
#define DB2_TABLE_DVR_H

#include "dbTable.h"
#define STRING_LENMAX 2000
#include "srbMiscExtern.h"




/***************************************************************************
  NAME   : db2TableOpen
  PURPOSE: Opens a DB2 Table as a file stream
  INPUT  :  exfd                    - external file descriptor handle
            resourceLocationName    - ipaddress:db-name:instance
            sqlSelectQuery          - SQL select query
	    uid                     - user identifier
	    pwd                     - user password

  OUTPUT : exfd                     - updated with Descriptor information, 
                                      internal cursor set to 1 (beginning)
  RETURN : 0 for SUCCESS negative for failure
****************************************************************************/
 
int db2TableOpen(dbTableDesc   *exfd, 
		   char       *resourceLocationName, 
		   char       *sqlSelectQuery,
		   char       *uid, 
		   char       *pwd);
/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : db2TableCreate
  PURPOSE: Creates an DB2 Table 
  INPUT  :  exfd                    - external file descriptor handle
            resourceLocationName    - ipaddress:db-name:instance
            tableProperties         - table and its attributes and conditions
	    uid                     - user identifier
	    pwd                     - user password

  OUTPUT : exfd                     - updated with Descriptor information, 
                                      internal cursor set to 1 (beginning)
  RETURN : 0 for SUCCESS negative for failure
****************************************************************************/
 
int db2TableCreate(dbTableDesc   *exfd, 
		   char       *resourceLocationName, 
		   char       *tableProperties,
		   char       *uid, 
		   char       *pwd);
/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : db2TableClose
  PURPOSE: Closes an open Db2 Table file stream
  INPUT  :  exfd                    - external file descriptor handle
  OUTPUT : 
  RETURN : 0 for SUCCESS negative for failure
****************************************************************************/
int db2TableClose(dbTableDesc  *exfd);
/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : db2TableRead
  PURPOSE: Reads from an open Db2 Table file stream
  INPUT  :  exfd                    - external file descriptor handle
            buffer                  - pre-allocated buffer for data (NOTE: 
	                              buffer size should be at 
	                              least 1 more than 'length')
				      data is stored in buffer concatenated
				      row-wise. Each data field is ended with
				      a null ('\0') character.
	    length                  - length of the buffer 
  OUTPUT :  buffer gets filled from Db2 Table
  RETURN : On SUCCESS returns actual number of bytes read; 
           Note that when filling a buffer, the function fills it to length.
	   This may have a data filed being chopped into two. The second
	   part of the field (and the rest of the row) will be returned when
	   the read is issued again.
           negative if FAILURE
****************************************************************************/
int db2TableRead(dbTableDesc *exfd, 
		  char     *buffer, 
		  int       length);
/*-------------------------------------------------------------------------*/

/***************************************************************************
  NAME   : db2TableWrite
  PURPOSE: Writes from an open Db2 Table file stream
  INPUT  :  exfd                    - external file descriptor handle
            sqlInsertString         - pre-allocated buffer for data (NOTE: 
	                              buffer size should be at 
	                              least 1 more than 'length')
				      data is stored in buffer concatenated
				      row-wise. Each data field is ended with
				      a null ('\0') character.
	    length                  - length of the buffer 
  OUTPUT :  buffer gets filled from Db2 Table
  RETURN : On SUCCESS returns actual number of bytes write; 
           Note that when filling a buffer, the function fills it to length.
	   This may have a data filed being chopped into two. The second
	   part of the field (and the rest of the row) will be returned when
	   the write is issued again.
           negative if FAILURE
****************************************************************************/
int db2TableWrite(dbTableDesc *exfd, 
		  char     *sqlInsertString,
		  int       length);
/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : db2TableSeek
  PURPOSE: Seeks into an open Db2 Table file stream
  INPUT  :  exfd                    - external file descriptor handle
            offset                  - number (in bytes) to move the cursor 
            whence                  - base of move (SEEK_SET, SEEK_CUR, 
                                      SEEK_END)
  OUTPUT :  internal cursor gets moved
  RETURN :  returns current cursor location (1 is beginning of buffer)
****************************************************************************/
int db2TableSeek(dbTableDesc *exfd, 
                  int      offset, 
                  int      whence);
/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : db2TableUnlink
  PURPOSE: Drops a given Db2 Table
  INPUT  :  resourceLocationName    - ipaddress:db-name:instance
            tableName               - Name of table to be dropped
            uid                     - user identifier
            pwd                     - user password
 
  OUTPUT : exfd                     - updated with LOB information, 
                                      internal cursor set to 1 (beginning)
  RETURN : 0 for SUCCESS negative for failure
****************************************************************************/
int db2TableUnlink(char    *resourceLocationName, 
                    char          *tableName,
                    char          *uid, 
                    char          *pwd);
/*-------------------------------------------------------------------------*/
#endif /* DB2_TABLE_DVR_H */
