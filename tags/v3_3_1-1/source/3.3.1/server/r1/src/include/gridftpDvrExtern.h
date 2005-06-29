/* 
gridftpDvrExtern.h 

Copyright University of California: San Diego
All Rights Reserved

This file contains Original Code and/or Modifications of Original Code as
defined in and that are subject to the General Atomics Storage Resource
Broker Academic and Research Use  License (the 'License'). This license is
for academic and research use only. You may not use, copy, modify or
distribute SRB for commercial purposes, nor incorporate the SRB into
commercial products without the express written permission of General
Atomics nor may you distribute SRB to any third party.

You may not use this software except in compliance with the License. Please
obtain a copy of the License at http://www.npaci.edu/DICE/SRB/License and
read it before using this file.

The Original Code and all software distributed under the License are
distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
EXPRESS OR IMPLIED, AND GENERAL ATOMICS AND THE SAN DIEGO SUPERCOMPUTER
CENTER HEREBY DISCLAIM ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION,
ANY WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
NON-INFRINGEMENT OF ANY COPYRIGHT, PATENT , TRADEMARK OR OTHER RIGHTS.
GENERAL ATOMICS AND THE SAN DIEGO SUPERCOMPUTER CENTER FURTHER  HAVE NO
OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCMENTS OR
MODIFICATIONS.. Please see the License for the specific language governing
rights and limitations under the License."

Please read the terms of this license carefully. By using the SRB software,
you are accepting and agreeing to the terms of this license with General
Atomics. If you are agreeing to this license on behalf of a company, or
other institution, you represent that you are authorized to bind the
company/institution to such a license. If you do not meet this criterion or
you do not agree to any of the terms of this license, you are not
authorized to use the software.
*/

#ifndef _gridftpDvrExtern_h__
#define _gridftpDvrExtern_h__
#include <sys/stat.h>

/* for read */
extern int gridftpRead(int fd, char *buf, unsigned int max_buf);
extern int gridftpOpen(char *gridftpUrl, int fileFlags, int fileMode);

/* for write */
extern int gridftpCreate(char *gridftpUrl, int fileMode, int dummySize);
extern int gridftpWrite(int fd, char *buffer, unsigned int amount);

extern int gridftpClose(int fd);
extern char *gridftpGetErrMsg(int err_id);

extern int gridftpMkdir(char *url, int mode);
extern int gridftpRmdir(char *url);

extern int gridftpUnlink(char *url);

extern int gridftpStat(char *filename, struct stat *statbuf);
extern int gridftpFstat(int fd,struct stat *statbuf);



/******** error code: should be moved to SRB error code table. ****************/
/*
#define GRIDFTP_OPEN_NULL_URL -6001
#define GRIDFTP_SETJMP_FAILED -6002
#define GRIDFTP_CANNOT_CALLOC -6003
#define GRIDFTP_CANNOT_FIND_NODE -6004
#define GRIDFTP_GETHOSTNAME_FAILED -6005
#define GRIDFTP_GETHOSTBYNAME_FAILED -6006
#define GRIDFTP_NOT_A_WRITE_OPERATION -6007
#define GRIDFTP_CLIENT_GET_ERROR -6008
#define GRIDFTP_READ_DONE_CB_ERR -6009
#define GRIDFTP_CLIENT_PUT_ERROR -6010
#define GRIDFTP_WRITE_DONE_CB_ERR -6011
#define GRIDFTP_CLIENT_MKDIR_ERROR -6012
#define GRIDFTP_MKDIR_DONE_CB_ERR -6013
#define GRIDFTP_CLIENT_RMDIR_ERROR -6014
#define GRIDFTP_RMDIR_DONE_CB_ERR -6015
#define GRIDFTP_CLIENT_UNLINK_ERROR -6016
#define GRIDFTP_UNLINK_DONE_CB_ERR -6017
#define GRIDFTP_CLIENT_EXISTS_ERROR -6018
#define GRIDFTP_FILE_OR_DIR_EXISTS_DONE_CB_ERR -6019
#define GSIFTP_SETJMP_FAILED -6020
#define GRIDFTP_WRITE_DATA_CB_ERR -6021
#define GRIDFTP_READ_DATA_CB_ERR -6022
#define GRIDFTP_WRITE_ERROR -6023
#define GRIDFTP_CLOSE_ERROR -6024
*/

#endif
