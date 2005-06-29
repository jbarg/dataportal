/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


#ifndef SRBC_ORA_EXTERNS_H
#define SRBC_ORA_EXTERNS_H


#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include  <unistd.h>

#include <oratypes.h>
#include <ociapr.h>
#include <ocidfn.h>
#include <ocimisc.h>

#include "dbLobjExtern.h"
#include "srbMiscExtern.h"
/* #include "mdas_errno.h" */
#include "srbErrno.h"

/* A compile time error occurs if the file "srb.h" is
 included here because in oratypes.h a "text" type
 is an unsigned char, in the srb.h text is a struct 
 varlena. I have worked around this by using ifndef s
 around the ORA_LOBJ #define constant 
*/ 
#ifndef ORA_LOBJ
#define ORA_LOBJ   5       /* Oracle LOBJ */
#endif

#define NOTICE  0 
#define HTTPC 4
/* #define MAX_TOKEN 200removed by raja March 8,2000 included from elsewhere*/


#define STRING_LEN2 32
#define STRING_LENMAX 2000

 
#define  VERSION_7   2
#define MAX_LONG_SIZE   2147483648

#define  OCI_MORE_INSERT_PIECES  3129
#define OCI_MORE_FETCH_PIECES 3130
#define OCI_NO_DATA_FOUND 1403

#ifndef BUF
#define BUF 1024 
#endif 

#ifndef MAX_BINDS
#define MAX_BINDS 12
#endif

#ifndef MAX_SQL_IDENTIFIER
#define MAX_SQL_IDENTIFIER  31
#endif

typedef struct {
  /* Is the cursor needed in this data struct ???? */ 
  ub4   hda[HDA_SIZE/sizeof(ub4)]; /* oracle host data area */
  Cda_Def cda; /* oracle cursor data area */
  Lda_Def lda; /* oracle logon data area */

  char mm_fname[256];
  int mm_fd;
  size_t mm_size;
  char *mm_ptr;
  char dirty;
  size_t offset;
  size_t lobsize;
  char	tableName[STRING_LENMAX];
  char  outColName[STRING_LENMAX];
  char  inCondition[STRING_LENMAX]; 

  char *query_string;

}or_db_state_info;


 

/***************************************************************************
  NAME   : srbC_ora_open
  PURPOSE: Opens a ORA LOB as a file stream
  INPUT  :  exfd                    - external file descriptor handle
            resourceLocationName    - ipaddress:db-name:instance
            dataPathName            - schema_name.table_name/outCol/inCol=objectID
                      or tabname/outCol/inCol1=val1&...&inColn=valn
	    uid                     - user identifier
	    pwd                     - user password

  OUTPUT : exfd                     - updated with LOB information, 
                                      internal cursor set to 1 (beginning)
  RETURN : 0 for SUCCESS negative for failure
****************************************************************************/
 
int srbC_ora_open(dbLobjDesc   *exfd, 
		   char       *resourceLocationName, 
		   char       *dataPathName, 
		   char       *uid, 
		   char       *pwd);
/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : srbC_ora_close
  PURPOSE: Closes an open ORA LOB file stream
  INPUT  :  exfd                    - external file descriptor handle
  OUTPUT : 
  RETURN : 0 for SUCCESS negative for failure
****************************************************************************/
int srbC_ora_close(dbLobjDesc  *exfd);
/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : srbC_ora_read
  PURPOSE: Opens a ORA LOB as a file stream
  INPUT  :  exfd                    - external file descriptor handle
            buffer                  - pre-allocated buffer for data (NOTE: 
	                              buffer size should be at 
	                              least 1 more than 'length')
	    length                  - length of the data to be read
  OUTPUT :  buffer gets filled from ora lob
  RETURN : On SUCCESS returns actual number of bytes read; 
           negative if FAILURE
****************************************************************************/
int srbC_ora_read(dbLobjDesc *exfd, 
		  char     *buffer, 
		  int       length);
/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : srbC_ora_write
  PURPOSE: Opens a ORA LOB as a file stream
  INPUT  :  exfd                    - external file descriptor handle
            buffer                  - pre-allocated buffer for data (NOTE: 
	                              buffer size should be at 
	                              least 1 more than 'length')
	    length                  - length of the data to be read
  OUTPUT :  buffer gets written into  ora lob
  RETURN : On SUCCESS returns actual number of bytes written; 
           negative if FAILURE
****************************************************************************/
int srbC_ora_write(dbLobjDesc *exfd, 
		  char     *buffer, 
		  int       length);
/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : srbC_ora_seek
  PURPOSE: Opens a ORA LOB as a file stream
  INPUT  :  exfd                    - external file descriptor handle
            offset                  - number (in bytes) to move the cursor 
	    whence                  - base of move (SEEK_SET, SEEK_CUR, 
	                              SEEK_END)
  OUTPUT :  internal cursor gets moved
  RETURN :  returns current cursor location (1 is beginning of buffer)
****************************************************************************/
int srbC_ora_seek(dbLobjDesc *exfd, 
		  int      offset, 
		  int      whence);
/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : srbC_ora_create
  PURPOSE: Creates a ORA LOB entry in the appropriate table. If table not already
           available, creates a new table.
  INPUT  :  exfd                    - external file descriptor handle
            resourceLocationName    - ipaddress:db-name:instance
            dataPathName            - schema_name.table_name/outCol/inCol=objectID
                      or tabname/outCol/inCol1=val1&...&inColn=valn
	    uid                     - user identifier
	    pwd                     - user password

  OUTPUT : exfd                     - updated with LOB information, 
                                      internal cursor set to 1 (beginning)
  RETURN : 0 for SUCCESS negative for failure
****************************************************************************/
 
int srbC_ora_create(dbLobjDesc   *exfd, 
		   char       *resourceLocationName, 
		   char       *dataPathName, 
		   char       *uid, 
		   char       *pwd);
/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : srbC_ora_unlink
  PURPOSE: Deletes a ORA LOB entry in the appropriate table. 
           If table not already available, creates a new table.
  INPUT  :  resourceLocationName    - ipaddress:db-name:instance
            dataPathName            - schema_name.table_name/outCol/inCol=objectID
                      or tabname/outCol/inCol1=val1&...&inColn=valn
	    uid                     - user identifier
	    pwd                     - user password

  OUTPUT : exfd                     - updated with LOB information, 
                                      internal cursor set to 1 (beginning)
  RETURN : 0 for SUCCESS negative for failure
****************************************************************************/
 
int srbC_ora_unlink(char          *resourceLocationName, 
                    char          *dataPathName, 
		    char          *uid, 
		    char          *pwd);
/*-------------------------------------------------------------------------*/
#endif	/* SRBC_ORA_EXTERNS_H */
