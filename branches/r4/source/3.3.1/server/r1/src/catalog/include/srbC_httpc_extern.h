/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

#ifndef SRBC_HTTPC_EXTERN_H
#define SRBC_HTTPC_EXTERN_H

typedef struct {
  int         sockFd;      /* socket descriptor */
}sock_info;

 
typedef struct {
    char        *hostAddr;     /* The host address of the ext file*/
    char        *filename;      /* the file pathname */
    int          storSysType;       /* access method */
    int          fd;             /* the file descriptor */
    void        *storSysDepend; /* actually sock_info */
}exfDesc;

/***************************************************************************
  NAME   : srbC_httpc_open
  PURPOSE: Opens a HTTP FILE as a file stream
  INPUT  :  exfd                    - external file descriptor handle
            resourceLocationName  - host-addr:port
            dataPathName          - path name of file
  OUTPUT : exfd                     - updated with FILE information, 
  RETURN : 0 for SUCCESS negative for failure
****************************************************************************/
 
int srbC_httpc_open(exfDesc   *exfd);
/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : srbC_httpc_close
  PURPOSE: Closes an open HTTP FILE file stream
  INPUT  :  exfd                    - external file descriptor handle
  OUTPUT : 
  RETURN : 0 for SUCCESS negative for failure
****************************************************************************/
int srbC_httpc_close(exfDesc  *exfd);
/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : srbC_httpc_read
  PURPOSE: Opens a HTTP FILE as a file stream
  INPUT  :  exfd                    - external file descriptor handle
            buffer                  - pre-allocated buffer for data 
	    length                  - length of the data to be read
  OUTPUT :  buffer gets filled from http file
  RETURN : On SUCCESS returns actual number of bytes read; 
           negative if FAILURE
****************************************************************************/
int srbC_httpc_read(exfDesc *exfd, 
		  char     *buffer, 
		  int       length);
/*-------------------------------------------------------------------------*/

#endif	/* SRBC_HTTPC_EXTERN_H */
