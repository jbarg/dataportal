/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

#ifndef SRBC_DB2_EXTERNS_H
#define SRBC_DB2_EXTERNS_H

#define STRING_LENMAX 2000

#define STRING_LEN2 32
#define DB2_DEBUG 0

typedef struct {
    int         fd;             /* the file descriptor or lob locator */
    int         henv;           /* db2 environment */
    int         hdbc;           /* db2 connection handle */
    int         lobsize;        /* lob size */
    int         locpos;         /* lob cursorr position */
    char        tableName[STRING_LENMAX];
    char        objectName[STRING_LENMAX];
    char        outColName[STRING_LENMAX];
    char        inCondition[STRING_LENMAX];
}db_state_info;
 

/***************************************************************************
  NAME   : srbC_db2_open
  PURPOSE: Opens a DB2 LOB as a file stream
  INPUT  :  exfd                    - external file descriptor handle
            resourceLocationName    - ipaddress:db-name:instance
            dataPathName            - schema_name.table_name/row-id
	    uid                     - user identifier
	    pwd                     - user password

  OUTPUT : exfd                     - updated with LOB information, 
                                      internal cursor set to 1 (beginning)
  RETURN : 0 for SUCCESS negative for failure
****************************************************************************/
 
int srbC_db2_open(dbLobjDesc   *exfd, 
		   char       *resourceLocationName, 
		   char       *dataPathName, 
		   char       *uid, 
		   char       *pwd);
/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : srbC_db2_close
  PURPOSE: Closes an open DB2 LOB file stream
  INPUT  :  exfd                    - external file descriptor handle
  OUTPUT : 
  RETURN : 0 for SUCCESS negative for failure
****************************************************************************/
int srbC_db2_close(dbLobjDesc  *exfd);
/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : srbC_db2_read
  PURPOSE: Opens a DB2 LOB as a file stream
  INPUT  :  exfd                    - external file descriptor handle
            buffer                  - pre-allocated buffer for data (NOTE: 
	                              buffer size should be at 
	                              least 1 more than 'length')
	    length                  - length of the data to be read
  OUTPUT :  buffer gets filled from db2 lob
  RETURN : On SUCCESS returns actual number of bytes read; 
           negative if FAILURE
****************************************************************************/
int srbC_db2_read(dbLobjDesc *exfd, 
		  char     *buffer, 
		  int       length);
/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : srbC_db2_write
  PURPOSE: Opens a DB2 LOB as a file stream
  INPUT  :  exfd                    - external file descriptor handle
            buffer                  - pre-allocated buffer for data (NOTE: 
	                              buffer size should be at 
	                              least 1 more than 'length')
	    length                  - length of the data to be read
  OUTPUT :  buffer gets written into  db2 lob
  RETURN : On SUCCESS returns actual number of bytes written; 
           negative if FAILURE
****************************************************************************/
int srbC_db2_write(dbLobjDesc *exfd, 
		  char     *buffer, 
		  int       length);
/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : srbC_db2_seek
  PURPOSE: Opens a DB2 LOB as a file stream
  INPUT  :  exfd                    - external file descriptor handle
            offset                  - number (in bytes) to move the cursor 
	    whence                  - base of move (SEEK_SET, SEEK_CUR, 
	                              SEEK_END)
  OUTPUT :  internal cursor gets moved
  RETURN :  returns current cursor location (1 is beginning of buffer)
****************************************************************************/
int srbC_db2_seek(dbLobjDesc *exfd, 
		  int      offset, 
		  int      whence);
/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : srbC_db2_create
  PURPOSE: Creates a DB2 LOB entry in the appropriate table. If table not already
           available, creates a new table.
  INPUT  :  exfd                    - external file descriptor handle
            resourceLocationName    - ipaddress:db-name:instance
            dataPathName            - schema_name.table_name/row-id
	    uid                     - user identifier
	    pwd                     - user password

  OUTPUT : exfd                     - updated with LOB information, 
                                      internal cursor set to 1 (beginning)
  RETURN : 0 for SUCCESS negative for failure
****************************************************************************/
 
int srbC_db2_create(dbLobjDesc   *exfd, 
		   char       *resourceLocationName, 
		   char       *dataPathName, 
		   char       *uid, 
		   char       *pwd);
/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : srbC_db2_unlink
  PURPOSE: Deletes a DB2 LOB entry in the appropriate table. 
           If table not already available, creates a new table.
  INPUT  :  resourceLocationName    - ipaddress:db-name:instance
            dataPathName            - schema_name.table_name/row-id
	    uid                     - user identifier
	    pwd                     - user password

  OUTPUT : exfd                     - updated with LOB information, 
                                      internal cursor set to 1 (beginning)
  RETURN : 0 for SUCCESS negative for failure
****************************************************************************/
 
int srbC_db2_unlink(char          *resourceLocationName, 
                    char          *dataPathName, 
		    char          *uid, 
		    char          *pwd);
/*-------------------------------------------------------------------------*/
#endif	/* SRBC_DB2_EXTERNS_H */
