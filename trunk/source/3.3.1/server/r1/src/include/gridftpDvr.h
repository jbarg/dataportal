/* 
gridftpDvr.h 

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

#ifndef _gridftpDvr_h__
#define _gridftpDvr_h__
#include "globus_gass_transfer.h"
#include "globus_ftp_client.h"
#include "globus_io.h"
#include <string.h>
#include "srbMiscExtern.h"
#include "elogExtern.h"
#include "gridftpDvrExtern.h"

/* #define GFTP_SERVER_DN       "/DC=org/DC=doegrids/OU=People/CN=John Bresnahan 413404/CN=246991291" */
/* #define GFTP_SERVER_DN "/C=US/O=NPACI/OU=SDSC/CN=Storage Resource Broker/USERID=srb" */

extern int ClientCertInx;
extern gss_cred_id_t delegated_cred_gsi[];
extern int VaultpathStatus;
extern userInfo *ClientUser, *ProxyUser, *ThisUser;

static int gftpConnect(char *urlPrefix, char *gftpSvrDn);
static int parseGftpUrl(char *inUrl, char *outUrl, char *outUrlPrefix,
char *outDn);
static int findGsiFtpFileByHandle(globus_ftp_client_handle_t handle);
static int  gridftp_closefile (int gconnInx);
extern void gridftp_read_done_cb();
static void gridftp_read_data_cb();
extern void gridftp_write_done_cb();
static void gridftp_write_data_cb();
static void gridftp_mkdir_done_cb();
static void gridftp_rmdir_done_cb();
static void file_or_dir_exists_done_cb();
static void gridftp_size_done_cb();
static int catch_done_cb();

/* a handle open flag is initially 0. */
#define GRIDFTP_HANDLE_OPEN_FOR_READ   1
#define GRIDFTP_HANDLE_OPEN_FOR_CREATE 2
#define GRIDFTP_HANDLE_OPEN_FOR_MKDIR  3
#define GRIDFTP_HANDLE_OPEN_FOR_RMDIR  4
#define GRIDFTP_HANDLE_OPEN_FOR_UNLINK 5
#define GRIDFTP_HANDLE_OPEN_FOR_FILE_OR_DIR_EXISTS 6
#define GRIDFTP_HANDLE_OPEN_FOR_SIZE 7

/*************************************************************************
 data structure for multiple files
 *************************************************************************/
typedef struct __gsiftp_files__
{
   int initFlag;
   globus_mutex_t      lock;           /* locking mutex for callbacks         */
   globus_cond_t       cond;           /* condition variable for callbacks    */
   globus_ftp_client_handle_t handle;
   globus_ftp_client_operationattr_t attr;
   globus_ftp_client_handleattr_t handle_attr;
   globus_ftp_control_dcau_t dcau;
   gss_cred_id_t credential;  	       /* user credential          */
   int handle_is_open;
   int read_file_flag;   /* 1 --> eof.   2 --> done_cb() called. 3 --> both */
   int mybufsize;
   char *mybuf;
   long offset;		/* for write */
   char urlPrefix[MAX_TOKEN];
   char gftpSvrDn[MAX_TOKEN];
}GSI_FTP_FILE;

/* definition for initFlag */
#define GCONN_UNCONNECTED		0
#define GCONN_CONNECTED_NOT_USED	1
#define GCONN_UNCONNECTED_INUSE		2

#define MAX_GFTP_CONN_INX	4
GSI_FTP_FILE GftpConn[MAX_GFTP_CONN_INX]; /* 0,1 conn with server credential,   
				           * 2,3 client credential 
				 	   */
#define SVR_CONN_INX	0
#define CLI_CONN_INX	2

#define GFTP_SERVER_DN_KEYWD	"GftpServerDn"
static int gsiftpError = 0;
static char gsiftpErrMsg[1024];


/**************************** local variables  *********************************/
int done;

static int First_time_grid_ftp_call = 1;


#endif
