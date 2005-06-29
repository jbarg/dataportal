/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* srbC_httpc_libraryExtern.h 
 */

#ifndef SRBC_HTTPC_LIBRARY_EXTERN_H
#define SRBC_HTTPC_LIBRARY_EXTERN_H

extern int srbC_httpc_open(char *dataPathName, int fileFlags, int fileMode);
extern int srbC_httpc_close(int fd);
extern int srbC_httpc_read (int fd, char *buffer, int length);
int parseHttpPath (char *dataPathName, char **myHostAddr, char **fullPathName);

#endif	/* SRBC_HTTPC_LIBRARY_EXTERN_H */

