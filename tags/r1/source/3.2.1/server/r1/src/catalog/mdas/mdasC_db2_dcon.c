/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

#include "mdasC_db2_externs.h"

void mdasC_db2_dcon(mdasC_servh insrvh, mdasC_status status)
{
      int i;
     MDAS_DB2_SERVER_HNDL *db2srvh; 

    status[0] = 0 ;
    status[1] = 1 ;
    status[2] = MDAS_SUCCESS;
    status[3] = MDAS_db2_DCON;

    db2srvh = (MDAS_DB2_SERVER_HNDL *) insrvh.conn;     

    i =  disconnect_db2( db2srvh->henv, db2srvh->hdbc, TRANSCOMMIT);
    if (i == -1) {status[0]=-1;status[2]=MDAS_ERR_SERVER; return;}
    if (i == -2) {status[0]=-1;status[2]=MDAS_ERR_SERVER; return;}
    if (i == -3) {status[0]=-1;status[2]=MDAS_ERR_MEMORY; return;}
    i =  close_env_db2(db2srvh->henv);
    if (i == -1) {status[0]=-1;status[2]=MDAS_ERR_MEMORY; return;}
    free(db2srvh);

}

