/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/


/* #include "COPYRIGHT.h"
   #include "mdasC_db2.h"
*/
#include "mdasC_db2_externs.h"
/* #include "mdasGlobalsExtern.h"*/

extern char DB2LOGFILE[];

void mdasC_db2_connect(mdasC_infoh* user,
        mdasC_infoh* ticket ,
        mdasC_infoh* server ,
        void* comm ,
        mdasC_servh *servh ,
        mdasC_status status)
{
    long int             henv;
    long int             hdbc;
    int i;
    MDAS_DB2_SERVER_HNDL *tmp_srvh;
    char querystr[MAX_TOKEN];

    status[0] = 0 ;
    status[1] = 1 ;
    status[2] = MDAS_SUCCESS;
    status[3] = MDAS_db2_CONN;
#ifdef DEBUGON 
    if (LOGMESSFILE == NULL)
    LOGMESSFILE = fopen(DB2LOGFILE,"a");
#endif /* DEBUGON */ 

    i = open_env_db2(&henv);
    if (i != 0)  {status[0]=-1;status[2]=MDAS_ERR_SERVH; return;}
    i = connect_db2( henv, &hdbc, (unsigned char*) server->info, 
		     /*		     NULL,NULL); */
		      (unsigned char*) user->info, 
		      (unsigned char*) ticket->info); 
    if (i == -1) {status[0]=-1;status[2]=MDAS_ERR_MEMORY; return;}
    if (i == -2) {status[0]=-1;status[2]=MDAS_ERR_MEMORY; return;}
    if (i == -3) {status[0]=-1;status[2]=MDAS_ERR_TICKET; return;}
    sprintf(querystr, "set current query optimization = %i", MDAS_DB2QUERYOPTLEVEL);
    i = exec_no_res_sql_db2( henv,  hdbc, (unsigned char*) querystr);

    tmp_srvh = (MDAS_DB2_SERVER_HNDL *) malloc( 360);
    if (tmp_srvh == NULL) 
      {status[0]=-1;status[2]=MDAS_ERR_MEMORY; return;}
    strcpy(tmp_srvh->user, user->info);
    strcpy(tmp_srvh->server,server->info);
    tmp_srvh->valid = 1;
    tmp_srvh->henv  = henv;
    tmp_srvh->hdbc  = hdbc;
    servh->conn = tmp_srvh;

    return;
}

