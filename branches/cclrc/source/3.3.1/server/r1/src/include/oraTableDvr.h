/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
#ifndef ORA_TABLE_DVR_H
#define ORA_TABLE_DVR_H
 
#ifdef PORTNAME_solaris
#include <ctype.h>
#endif

#define ORACLE_DRIVER
#include "srbC_ora_externs.h"
#include "dbTable.h"



/***************************************************************************
  NAME   : oraTableOpen
  PURPOSE: Opens a ORA Table as a file stream
  INPUT  :  exfd                    - external file descriptor handle
            resourceLocationName    - ipaddress:db-name:instance
            sqlSelectQuery          - SQL select query
	    uid                     - user identifier
	    pwd                     - user password

  OUTPUT : exfd                     - updated with Descriptor information, 
                                      internal cursor set to 1 (beginning)
  RETURN : 0 for SUCCESS negative for failure
****************************************************************************/
 
int oraTableOpen(dbTableDesc   *exfd, 
		   char       *resourceLocationName, 
		   char       *sqlSelectQuery,
		   char       *uid, 
		   char       *pwd);
/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : oraTableCreate
  PURPOSE: Creates an ORA Table 
  INPUT  :  exfd                    - external file descriptor handle
            resourceLocationName    - ipaddress:db-name:instance
            tableProperties         - table and its attributes and conditions
	    uid                     - user identifier
	    pwd                     - user password

  OUTPUT : exfd                     - updated with Descriptor information, 
                                      internal cursor set to 1 (beginning)
  RETURN : 0 for SUCCESS negative for failure
****************************************************************************/
 
int oraTableCreate(dbTableDesc   *exfd, 
		   char       *resourceLocationName, 
		   char       *tableProperties,
		   char       *uid, 
		   char       *pwd);
/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : oraTableClose
  PURPOSE: Closes an open Oracle Table file stream
  INPUT  :  exfd                    - external file descriptor handle
  OUTPUT : 
  RETURN : 0 for SUCCESS negative for failure
****************************************************************************/
int oraTableClose(dbTableDesc  *exfd);
/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : oraTableRead
  PURPOSE: Reads from an open Oracle Table file stream
  INPUT  :  exfd                    - external file descriptor handle
            buffer                  - pre-allocated buffer for data (NOTE: 
	                              buffer size should be at 
	                              least 1 more than 'length')
				      data is stored in buffer concatenated
				      row-wise. Each data field is ended with
				      a null ('\0') character.
	    length                  - length of the buffer 
  OUTPUT :  buffer gets filled from Oracle Table
  RETURN : On SUCCESS returns actual number of bytes read; 
           Note that when filling a buffer, the function fills it to length.
	   This may have a data filed being chopped into two. The second
	   part of the field (and the rest of the row) will be returned when
	   the read is issued again.
           negative if FAILURE
****************************************************************************/
int oraTableRead(dbTableDesc *exfd, 
		  char     *buffer, 
		  int       length);
/*-------------------------------------------------------------------------*/

/***************************************************************************
  NAME   : oraTableWrite
  PURPOSE: Writes from an open Oracle Table file stream
  INPUT  :  exfd                    - external file descriptor handle
            sqlInsertString         - pre-allocated buffer for data (NOTE: 
	                              buffer size should be at 
	                              least 1 more than 'length')
				      data is stored in buffer concatenated
				      row-wise. Each data field is ended with
				      a null ('\0') character.
	    length                  - length of the buffer 
  OUTPUT :  buffer gets filled from Oracle Table
  RETURN : On SUCCESS returns actual number of bytes write; 
           Note that when filling a buffer, the function fills it to length.
	   This may have a data filed being chopped into two. The second
	   part of the field (and the rest of the row) will be returned when
	   the write is issued again.
           negative if FAILURE
****************************************************************************/
int oraTableWrite(dbTableDesc *exfd, 
		  char     *sqlInsertString,
		  int       length);
/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : oraTableSeek
  PURPOSE: Seeks into an open Oracle Table file stream
  INPUT  :  exfd                    - external file descriptor handle
            offset                  - number (in bytes) to move the cursor 
            whence                  - base of move (SEEK_SET, SEEK_CUR, 
                                      SEEK_END)
  OUTPUT :  internal cursor gets moved
  RETURN :  returns current cursor location (1 is beginning of buffer)
****************************************************************************/
int oraTableSeek(dbTableDesc *exfd, 
                  int      offset, 
                  int      whence);
/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : oraTableUnlink
  PURPOSE: Drops a given Oracle Table
  INPUT  :  resourceLocationName    - ipaddress:db-name:instance
            tableName               - Name of table to be dropped
            uid                     - user identifier
            pwd                     - user password
 
  OUTPUT : exfd                     - updated with LOB information, 
                                      internal cursor set to 1 (beginning)
  RETURN : 0 for SUCCESS negative for failure
****************************************************************************/
int oraTableUnlink(char    *resourceLocationName, 
                    char          *tableName,
                    char          *uid, 
                    char          *pwd);
/*-------------------------------------------------------------------------*/
int oraTableReadXMLREL(dbTableDesc *exfd, char *buffer, int length);
int
oraTableReadXMLTREE(dbTableDesc *exfd, char *buffer, int length);
int
oraTableReadHTMLREL(dbTableDesc *exfd, char *buffer, int length);
int
oraTableReadHTMLTREE(dbTableDesc *exfd, char *buffer, int length);
int
oraTableReadHTMLNEST(dbTableDesc *exfd, char *buffer, int length);
int
oraTableReadTEMPLATE(dbTableDesc *exfd, char *buffer, int length);
int
oraTableReadPACK(dbTableDesc *exfd, char *buffer, int length);
int
oraTableWriteXMLREL(dbTableDesc *exfd, char *buffer, int length);
int
oraTableWriteXMLTREE(dbTableDesc *exfd, char *inbuffer, int length);
int
oraTableWriteHTMLREL(dbTableDesc *exfd, char *buffer, int length);
int
oraTableWriteHTMLTREE(dbTableDesc *exfd, char *buffer, int length);
int
oraTableWriteHTMLNEST(dbTableDesc *exfd, char *buffer, int length);
int
oraTableWriteTEMPLATE(dbTableDesc *exfd, char *inbuffer, int length);
int
oraTableWriteSQL(dbTableDesc *exfd, char *sqlInsertString, int length);
int
oraTableWritePACK(dbTableDesc *exfd, char *buffer, int length);
int
oraTableWriteVALUES(dbTableDesc *exfd, char *sqlInsertString, int length);


#endif /* ORA_TABLE_DVR_H */
