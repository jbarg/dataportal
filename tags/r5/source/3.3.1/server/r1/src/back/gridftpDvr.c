/*
 * gridftpDvr.c
 */
#include "gridftpDvr.h"

#ifdef foo
#define _DEBUG 1
#define GFTP_TIMING
#endif


/******** error code: should be moved to SRB error code table. *******
#define GRIDFTP_OPEN_NULL_URL -6001
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
#define GRIDFTP_CLIENT_SIZE_ERROR -6020
#define GRIDFTP_SIZE_DONE_CB_ERR -6021
#define GRIDFTP_OPERATION_NOT_SUPPORT -6022
#define GRIDFTP_NEED_NEW_PROXY_CERTIFICATE -6023
#define GRIDFTP_NO_USER_CERT_ERR -6024
#define GRIDFTP_NO_FREE_GFTP_CONN_ERR -6025
#define GRIDFTP_GFTP_CONN_OUT_OF_RANG_ERR -6026
#define GRIDFTP_BAD_INPUT_URL_ERR -6027
*/

/*****************************************************************************/
static int gftpConnect(char *urlPrefix, char *gftpSvrDn)
{
    int i;
    int gconnInx;
    int startInx;
    int matchUrl;
    gss_cred_id_t *myCredential;
    OM_uint32 gss_status;
    char *myGftpSvrDn;

    if(First_time_grid_ftp_call == 1) {
        globus_module_activate(GLOBUS_FTP_CLIENT_MODULE);
        for (i = 0; i < MAX_GFTP_CONN_INX; i++) {
	    memset (&GftpConn[i], 0, sizeof (GSI_FTP_FILE));
	}
        First_time_grid_ftp_call = 0;
    }

    if (VaultpathStatus == PATH_IN_VAULT) {
	startInx = SVR_CONN_INX;
    } else {
	startInx = CLI_CONN_INX;
    }

    /* pick a GftpConn */

    if (GftpConn[startInx].initFlag == GCONN_CONNECTED_NOT_USED && 
     strcmp (urlPrefix, GftpConn[startInx].urlPrefix) == 0) {
	gconnInx = startInx;
	matchUrl = 1;
    } else if (GftpConn[startInx + 1].initFlag == GCONN_CONNECTED_NOT_USED &&
     strcmp (urlPrefix, GftpConn[startInx + 1].urlPrefix) == 0) {
        gconnInx = startInx + 1;
	matchUrl = 1;
    } else if (GftpConn[startInx].initFlag == GCONN_UNCONNECTED) {
	gconnInx = startInx;
    } else if (GftpConn[startInx + 1].initFlag == GCONN_UNCONNECTED) {
        gconnInx = startInx + 1;
    } else if (GftpConn[startInx].initFlag == GCONN_CONNECTED_NOT_USED) {
        gconnInx = startInx;
	matchUrl = 0;
    } else if (GftpConn[startInx + 1].initFlag == GCONN_CONNECTED_NOT_USED) {
        gconnInx = startInx + 1;
	matchUrl = 0;
    } else {
	elog (NOTICE, "gftpConnect: No free GftpConn");
	return GRIDFTP_NO_FREE_GFTP_CONN_ERR;
    }

    if (GftpConn[gconnInx].initFlag == GCONN_CONNECTED_NOT_USED) {
	if (matchUrl == 0) {
	    /* a different URL */
            globus_ftp_client_operationattr_destroy(
	     &GftpConn[gconnInx].attr);
            globus_ftp_client_handle_destroy(
	      &GftpConn[gconnInx].handle);
            globus_ftp_client_handleattr_destroy(
	      &GftpConn[gconnInx].handle_attr);
	} else {
	    GftpConn[gconnInx].initFlag = GCONN_UNCONNECTED_INUSE;
	    return (gconnInx);
	}
    } else {
        /* unconnected */
        globus_mutex_init(&GftpConn[gconnInx].lock, GLOBUS_NULL);
        globus_cond_init(&GftpConn[gconnInx].cond, GLOBUS_NULL);
    }
    strcpy (GftpConn[gconnInx].urlPrefix, urlPrefix); 
    globus_ftp_client_handleattr_init(&(GftpConn[gconnInx].handle_attr));
    /* Set caching to create persistent connections */
    globus_ftp_client_handleattr_set_cache_all(
      &(GftpConn[gconnInx].handle_attr), GLOBUS_TRUE);
    globus_ftp_client_operationattr_init(&(GftpConn[gconnInx].attr));
    globus_ftp_client_handle_init(&(GftpConn[gconnInx].handle), 
      &(GftpConn[gconnInx].handle_attr));

    if (gftpSvrDn == NULL || strlen (gftpSvrDn) <= 0) {
	char *ptr; 
	ptr = getenv (GFTP_SERVER_DN_KEYWD);
	if (ptr == NULL) {
            elog (NOTICE, "gftpConnect: Dn of GFTP server not defined");
	    myGftpSvrDn = NULL;
	} else {
	    strcpy (GftpConn[gconnInx].gftpSvrDn, ptr);
	    myGftpSvrDn = GftpConn[gconnInx].gftpSvrDn;
	}
    } else {
        strcpy (GftpConn[gconnInx].gftpSvrDn, gftpSvrDn);
        myGftpSvrDn = GftpConn[gconnInx].gftpSvrDn;
    }
  
    if (VaultpathStatus == PATH_IN_VAULT) {
	/* use the server Credential */
	 if (globus_gss_assist_acquire_cred (&gss_status, GSS_C_INITIATE,
          &GftpConn[gconnInx].credential) != GSS_S_COMPLETE) {
             elog (NOTICE,
             "gftpConnect:globus_gss_assist_acquire_cred err gss_status = %d",
	       gss_status);
            return GRIDFTP_NO_USER_CERT_ERR;
	}
        globus_ftp_client_operationattr_set_authorization(
         &(GftpConn[gconnInx].attr),
         GftpConn[gconnInx].credential,
         NULL, NULL, NULL, myGftpSvrDn);

    } else {
	/* Use user's proxy certificate */
	if (ClientCertInx < 0) {
             elog (NOTICE,
             "gftpConnect: no proxy cert for %s@%s",
               ClientUser->userName, ClientUser->domainName);
	    return GRIDFTP_NO_USER_CERT_ERR;
	}
        globus_ftp_client_operationattr_set_authorization(
         &(GftpConn[gconnInx].attr),
         delegated_cred_gsi[ClientCertInx],
         NULL, NULL, NULL, myGftpSvrDn);
    }
    GftpConn[gconnInx].dcau.mode = GLOBUS_FTP_CONTROL_DCAU_NONE;

    globus_ftp_client_operationattr_set_dcau(&GftpConn[gconnInx].attr,
     &GftpConn[gconnInx].dcau);

    GftpConn[gconnInx].initFlag = GCONN_UNCONNECTED_INUSE;
    return (gconnInx);
}

/******************************************************************************
 return < 0  --> failed 
        = 0  --> OK
 ******************************************************************************/
static int parseGftpUrl(char *inUrl, char *outUrl, char *outUrlPrefix,
char *outDn)
{
    char *startPtr, *dnPtr, *breakPtr, *tmpPtr1;
    int len;

    breakPtr = strstr (inUrl, "::::");

    if (breakPtr == NULL) {
         elog (NOTICE,
          "parseGftpUrl: Bad input URL %s, missing ::::", inUrl);
         return GRIDFTP_BAD_INPUT_URL_ERR;
    }

    /* skip the zone and the SRB server address */

    startPtr = strstr (inUrl, ":");
    if (startPtr == NULL) {
         elog (NOTICE,
          "parseGftpUrl: Bad input URL %s, missing :", inUrl);
         return GRIDFTP_BAD_INPUT_URL_ERR;
    } else {
	startPtr++;
    }

    /* find the next : */

    tmpPtr1 = strstr (startPtr, ":");

    if (tmpPtr1 < breakPtr) {
	/* skip the : */
	/* gridFTP port number if present */
	if (isdigit (*(tmpPtr1 + 1)) != 0) {
	    tmpPtr1 ++;
	    tmpPtr1 = strstr (tmpPtr1, ":");
	}
    } 
    len = tmpPtr1 - startPtr;

    /* copy the prefix */
    strncpy (outUrlPrefix, startPtr, len);
    outUrlPrefix[len] = '\0';

    if (tmpPtr1 < breakPtr) {
	tmpPtr1 ++;
	if (strncmp (tmpPtr1, "NULL", 4) != 0 &&
	  strncmp (tmpPtr1, "null", 4) != 0) {
	    dnPtr = tmpPtr1;
	    tmpPtr1 = strstr (dnPtr, ":");
	    if (tmpPtr1 != NULL) {
		len = tmpPtr1 - dnPtr;
		strncpy (outDn, dnPtr, len);
		outDn[len] = '\0';
	    } else {
		*outDn = '\0';
	    }
	} else {
	    *outDn = '\0';
	}
    } else {
	*outDn = '\0';
    }

    /* skip :::: */
    breakPtr += 4;

    sprintf (outUrl, "gsiftp://%s%s", outUrlPrefix, breakPtr); 

    return 0;
}

static int findGsiFtpFileByHandle(globus_ftp_client_handle_t handle)
{
    int i;

    for (i = 0; i < MAX_GFTP_CONN_INX; i++) {
        if(GftpConn[i].handle == handle)
            return i;
   }
   return -1;
}

static int  gridftp_closefile (int gconnInx)
{
    GSI_FTP_FILE *fp;

#ifdef _DEBUG
    fprintf(stderr,"gridftp_closefile()\n");
#endif

    if (gconnInx < 0 || gconnInx >= MAX_GFTP_CONN_INX) {
        elog (NOTICE,
         "gridftp_closefile: gconnInx %d out of range", gconnInx);
        return GRIDFTP_GFTP_CONN_OUT_OF_RANG_ERR;
    }

    fp = &GftpConn[gconnInx];

    fp->initFlag = GCONN_CONNECTED_NOT_USED;
    fp->handle_is_open = fp->read_file_flag = fp->mybufsize = fp->offset = 0; 
    fp->mybuf = NULL;
    
    return 0;
}

static int CredentialsExpired(char *msg)
{
   if(strstr(msg,"no proxy credentials"))
     return 1;

   return 0;
}

static void save_err_msgg(globus_object_t* err)
{
   char * tmpstr;

   tmpstr = globus_object_printable_to_string(err);
   if(tmpstr != NULL)
   {
      if(strlen(tmpstr) > 1023)
        strncpy(gsiftpErrMsg,tmpstr,1023);
      else
        strcpy(gsiftpErrMsg,tmpstr);
      globus_libc_free(tmpstr);
      fprintf(stderr,"gsiftpErrMsg=<%s>\n",gsiftpErrMsg); 
   }
}

/* don't forget to free the string returned by this function */
char *gridftpGetErrMsg(int err_id)
{
   char *msg;

   msg = NULL;
   switch(err_id)
   {
      case GRIDFTP_CLIENT_GET_ERROR:
      case GRIDFTP_READ_DONE_CB_ERR:
      case GRIDFTP_CLIENT_MKDIR_ERROR:
      case GRIDFTP_MKDIR_DONE_CB_ERR:
      case GRIDFTP_CLIENT_RMDIR_ERROR:
      case GRIDFTP_RMDIR_DONE_CB_ERR:
      case GRIDFTP_CLIENT_UNLINK_ERROR:
      case GRIDFTP_UNLINK_DONE_CB_ERR:
      case GRIDFTP_CLIENT_EXISTS_ERROR:
      case GRIDFTP_FILE_OR_DIR_EXISTS_DONE_CB_ERR:
      case GRIDFTP_CLIENT_SIZE_ERROR:
      case GRIDFTP_SIZE_DONE_CB_ERR:
      case GRIDFTP_NEED_NEW_PROXY_CERTIFICATE:
      case GRIDFTP_CLIENT_PUT_ERROR:
      case GRIDFTP_WRITE_DONE_CB_ERR:
        msg = strdup(gsiftpErrMsg);
        break;
      case GRIDFTP_OPEN_NULL_URL:
        msg = strdup("The input gsiftp URL string is NULL.");
        break;
      case GRIDFTP_CANNOT_CALLOC:
        msg = strdup("calloc() function failed.");
        break;
      case GRIDFTP_CANNOT_FIND_NODE:
        msg = strdup("failed to find node in linked list.");
        break;
      case GRIDFTP_GETHOSTNAME_FAILED:
        msg = strdup("gethostname() failed.");
        break;
      case GRIDFTP_GETHOSTBYNAME_FAILED:
        msg = strdup("gethostbyname() failed");
        break;
      case GRIDFTP_NOT_A_WRITE_OPERATION:
        msg = strdup("not a write operation.");
        break;
      case GRIDFTP_OPERATION_NOT_SUPPORT:
        msg = strdup("The operation is not supportted by gridftp driver functions.");
        break;
   }

   return msg;
}

/* assume that is open for read. Bing July 19, 2002. */
int gridftpOpen(char *gridftpUrl, int fileFlags, int fileMode)
{
    globus_result_t result;
    GSI_FTP_FILE *fp;
    char actual_url[MAX_TOKEN * 2], myDn[MAX_TOKEN], urlPrefix[MAX_TOKEN];
    int t;
    gss_cred_id_t      credential;  /* user credential          */
    OM_uint32 gss_status;
    int gconnInx;

#ifdef _DEBUG
    fprintf(stderr,"gridftpOpen()\n");
#endif

    if(gridftpUrl == NULL)
      return GRIDFTP_OPEN_NULL_URL;

    t = parseGftpUrl(gridftpUrl, actual_url, urlPrefix, myDn);
    if(t < 0)
      return t;

    gconnInx = gftpConnect(urlPrefix, myDn);
    if (gconnInx < 0) {
	return gconnInx;
    }

    fp = &GftpConn[gconnInx];

    done = 0;
    result = globus_ftp_client_get(&(fp->handle),
                                   actual_url,
                                   &(fp->attr), 
                                   GLOBUS_NULL, /* restart marker */
                                   gridftp_read_done_cb,
                                   0);          /* no user data for the cb */

    if(result != GLOBUS_SUCCESS) {
        strcpy(gsiftpErrMsg,globus_object_printable_to_string(
	 globus_error_get(result)));

        globus_mutex_unlock(&fp->lock);

        gridftp_closefile(gconnInx);

        if(CredentialsExpired(gsiftpErrMsg))
          return GRIDFTP_NEED_NEW_PROXY_CERTIFICATE;

        return GRIDFTP_CLIENT_GET_ERROR;
    }

    fp->handle_is_open = GRIDFTP_HANDLE_OPEN_FOR_READ;
    fp->read_file_flag = 0;

    return (gconnInx);
}

int gridftpRead(int gconnInx, char *buf, unsigned int max_buf)
{
    int bufsize;
    GSI_FTP_FILE *fp;

#ifdef _DEBUG
    fprintf(stderr,"gridftpRead()\n");
#endif

    if (gconnInx < 0 || gconnInx >= MAX_GFTP_CONN_INX) {
        elog (NOTICE,
         "gridftp_closefile: gconnInx %d out of range", gconnInx);
        return GRIDFTP_GFTP_CONN_OUT_OF_RANG_ERR;
    }

    fp = &GftpConn[gconnInx];

    if(fp->read_file_flag == 1)
    {
       catch_done_cb(fp);
       gridftp_closefile (gconnInx);
       return 0;
    }
    else if(fp->read_file_flag > 0)
    {
       gridftp_closefile(gconnInx);
       return 0;
    }

    fp->mybuf = buf;
    done = bufsize = 0;
    if (globus_ftp_client_register_read(
	    &(fp->handle),
	    buf,
	    max_buf,
	    gridftp_read_data_cb, 0)) {
	fprintf (stderr, "globus_ftp_client_register_read error\n");
    }

    gsiftpError = 0;
    globus_mutex_lock(&fp->lock);
    while(!done)
    {
	globus_cond_wait(&fp->cond, &fp->lock);
    }
    done = 0;

    if(gsiftpError)
    {
        gridftp_closefile(gconnInx);
        globus_mutex_unlock(&fp->lock);
        return gsiftpError;
    }

    if((fp->mybuf != NULL) && (fp->mybufsize > 0))
    {
       bufsize = fp->mybufsize;
#ifdef foo
       memcpy(buf,fp->mybuf,fp->mybufsize);
       free(fp->mybuf);
#endif
    }
    fp->mybuf = NULL;
    fp->mybufsize = 0;

    globus_mutex_unlock(&fp->lock); 

    return bufsize;
}

static void gridftp_read_data_cb(
    void *					user_arg,
    globus_ftp_client_handle_t *		handle,
    globus_object_t *				err,
    globus_byte_t *				buffer,
    globus_size_t				length,
    globus_off_t				offset,
    globus_bool_t				eof)
{
   GSI_FTP_FILE *fp;
   int gconnInx;

#ifdef _DEBUG
   fprintf(stderr,"gridftp_read_data_cb()\n"); 
#endif

   gconnInx = findGsiFtpFileByHandle(*handle);

   if(gconnInx >= 0) {
      fp = &GftpConn[gconnInx];
/*
      fp->mybuf = NULL;
      fp->mybufsize = 0;
*/
      if(length > 0) {
         fp->mybufsize = (int)length;
#ifdef foo
         fp->mybuf = (char *)calloc(length,sizeof(char));
         memcpy(fp->mybuf,buffer,length);
#endif
      } else {
         fp->mybufsize = 0;
      }

      if(eof)
         fp->read_file_flag |= 1;

      globus_mutex_lock(&fp->lock);
      done = GLOBUS_TRUE;
      globus_cond_signal(&fp->cond);
      globus_mutex_unlock(&fp->lock);
   } else {
      elog (NOTICE, "gridftp_read_data_cb: no match handle");
      return;
   }
}

/*************************************************************************
 * done callback; passed to globus_ftp_client_get
 * this is how you are notified that the control channel protocol exchange
 * is finished.
 *************************************************************************/
void gridftp_read_done_cb(
	void *					user_arg,
	globus_ftp_client_handle_t *		handle,
	globus_object_t *			err)
{
   GSI_FTP_FILE *fp;
   int gconnInx;

#ifdef _DEBUG
   fprintf(stderr,"read_done_cb()\n");
#endif

   gconnInx = findGsiFtpFileByHandle(*handle);
   if(gconnInx >= 0) {
      fp = &GftpConn[gconnInx];
      fp->read_file_flag |= 2;
   } else {
      elog (NOTICE, "gridftp_read_done_cb: no match handle");
      return;
   }

   if(err) {
      /* fprintf (stderr, "%s", globus_error_print_friendly (err)); */
      save_err_msgg(err);
      if(CredentialsExpired(gsiftpErrMsg))
        gsiftpError = GRIDFTP_NEED_NEW_PROXY_CERTIFICATE;
      else
        gsiftpError = GRIDFTP_READ_DONE_CB_ERR;
   }

   if(gconnInx >= 0) {
      globus_mutex_lock(&fp->lock);
      done = GLOBUS_TRUE;
      globus_cond_signal(&fp->cond);
      globus_mutex_unlock(&fp->lock);
   }
}

static int catch_done_cb(GSI_FTP_FILE *fp)
{

#ifdef _DEBUG
   fprintf(stderr,"catch_done_cb()\n");
#endif

   /* done = 0;  XXXXX turn it back on */
   gsiftpError = 0;
   globus_mutex_lock(&fp->lock);
   while(!done)
   {
      globus_cond_wait(&fp->cond, &fp->lock);
   }
   globus_mutex_unlock(&fp->lock);

   return gsiftpError;
}

int gridftpClose(int gconnInx)
{
   GSI_FTP_FILE *fp;
   char buffer[100];

#ifdef _DEBUG
   fprintf(stderr,"gridftpClose()\n"); 
#endif

   if (gconnInx < 0 || gconnInx >= MAX_GFTP_CONN_INX) {
        elog (NOTICE,
         "gridftpClose: gconnInx %d out of range", gconnInx);
        return GRIDFTP_GFTP_CONN_OUT_OF_RANG_ERR;
   }

   fp = &GftpConn[gconnInx];

   if(fp == NULL)
     return 0;

   if(fp->handle_is_open == GRIDFTP_HANDLE_OPEN_FOR_CREATE)
   {
      done = 0;
      globus_ftp_client_register_write(
            &(fp->handle),
            buffer,
            0,
            fp->offset,
            1,
            gridftp_write_data_cb,
            NULL);
      (void)catch_done_cb(fp);
   }

   return gridftp_closefile(gconnInx);
}


/***************************************************************************
 Write Section
 ***************************************************************************/
int gridftpCreate(char *gridftpUrl, int fileMode, int dummySize)
{
   globus_result_t result;
   GSI_FTP_FILE *fp;
   char actual_url[MAX_TOKEN * 2], myDn[MAX_TOKEN], urlPrefix[MAX_TOKEN];
   int t;
   OM_uint32 gss_status;
   int gconnInx;

#ifdef _DEBUG
   fprintf(stderr,"gridftpCreate()\n"); 
#endif

   if(gridftpUrl == NULL)
     return GRIDFTP_OPEN_NULL_URL;

   t = parseGftpUrl(gridftpUrl, actual_url, urlPrefix, myDn);
   if(t < 0)
     return t;

    gconnInx = gftpConnect(urlPrefix, myDn);
    if (gconnInx < 0) {
        return gconnInx;
    }

    fp = &GftpConn[gconnInx];

   result = globus_ftp_client_put(&(fp->handle),
                                   actual_url,
                                   &(fp->attr),
                                   GLOBUS_NULL,
                                   gridftp_write_done_cb,
                                   0);

   if(result != GLOBUS_SUCCESS)
   {
      strcpy(gsiftpErrMsg,globus_object_printable_to_string(globus_error_get(result)));

      globus_mutex_unlock(&fp->lock);

      gridftp_closefile(gconnInx);

      if(CredentialsExpired(gsiftpErrMsg))
        return GRIDFTP_NEED_NEW_PROXY_CERTIFICATE;

      return GRIDFTP_CLIENT_PUT_ERROR;
   }

   fp->handle_is_open = GRIDFTP_HANDLE_OPEN_FOR_CREATE;

   return (gconnInx);  
}

static void gridftp_write_data_cb(void *        user_arg,
    globus_ftp_client_handle_t *                handle,
    globus_object_t *                           err,
    globus_byte_t *                             buffer,
    globus_size_t                               length,
    globus_off_t                                offset,
    globus_bool_t                               eof)
{
   GSI_FTP_FILE *fp;
   int gconnInx;

#ifdef _DEBUG
    fprintf(stderr,"gridftp_write_data_cb()\n"); 
#endif

   gconnInx = findGsiFtpFileByHandle(*handle);

   if(gconnInx >= 0) {
      fp = &GftpConn[gconnInx];
      fp->offset = fp->offset + (long)length;

      if(!eof) {
         globus_mutex_lock(&fp->lock);
         done = GLOBUS_TRUE;
         globus_cond_signal(&fp->cond);
         globus_mutex_unlock(&fp->lock);
      }
   } else {
      elog (NOTICE, "gridftp_write_data_cb: no match handle");
   }
}

int gridftpWrite(int gconnInx, char *buffer, unsigned int amount)
{
   GSI_FTP_FILE *fp;

#ifdef _DEBUG
   fprintf(stderr,"gridftpWrite()\n"); 
#endif

   if (gconnInx < 0 || gconnInx >= MAX_GFTP_CONN_INX) {
        elog (NOTICE,
         "gridftpWrite: gconnInx %d out of range", gconnInx);
        return GRIDFTP_GFTP_CONN_OUT_OF_RANG_ERR;
   }

   fp = &GftpConn[gconnInx];

   globus_ftp_client_register_write(
            &(fp->handle),
            buffer,
            amount,
            (globus_off_t)(fp->offset),
            0,
            gridftp_write_data_cb,
            NULL);

   done = 0;     
   gsiftpError = 0;
   globus_mutex_lock(&fp->lock);
   while(!done)
   {
      globus_cond_wait(&fp->cond, &fp->lock);
   }
   globus_mutex_unlock(&fp->lock);

   if(gsiftpError < 0)
   {
      return gsiftpError;
   }

   return amount;
}

long gridftpGetWriteSize(int gconnInx)
{
   GSI_FTP_FILE *fp;

#ifdef _DEBUG
   fprintf(stderr,"gridftpGetWriteSize()\n");
#endif


   if (gconnInx < 0 || gconnInx >= MAX_GFTP_CONN_INX) {
        elog (NOTICE,
         "gridftp_closefile: gconnInx %d out of range", gconnInx);
        return GRIDFTP_GFTP_CONN_OUT_OF_RANG_ERR;
   }

   fp = &GftpConn[gconnInx];


   if(fp->handle_is_open != GRIDFTP_HANDLE_OPEN_FOR_CREATE)
   {
      return GRIDFTP_NOT_A_WRITE_OPERATION;
   }

   return fp->offset;
}

void gridftp_write_done_cb(void*               user_arg,
        globus_ftp_client_handle_t*            handle,
        globus_object_t*                       err)
{
   GSI_FTP_FILE *fp;
   int gconnInx;

#ifdef _DEBUG
   fprintf(stderr,"gridftp_write_done_cb()\n");
#endif

   gconnInx = findGsiFtpFileByHandle(*handle);

   if (gconnInx < 0) {
      elog (NOTICE, "gridftp_write_done_cb: no match handle");
      return;
   }

   fp = &GftpConn[gconnInx];

   if(err) {
      save_err_msgg(err);
      if(CredentialsExpired(gsiftpErrMsg))
        gsiftpError = GRIDFTP_NEED_NEW_PROXY_CERTIFICATE;
      else
        gsiftpError = GRIDFTP_WRITE_DONE_CB_ERR;
   }

   globus_mutex_lock(&fp->lock);
   done = GLOBUS_TRUE;
   globus_cond_signal(&fp->cond);
   globus_mutex_unlock(&fp->lock);
}

/****************************************************************************/
static void gridftp_rmdir_done_cb(void*        user_arg,
        globus_ftp_client_handle_t*            handle,
        globus_object_t*                       err)
{
   GSI_FTP_FILE *fp;
   int gconnInx;

#ifdef _DEBUG
   fprintf(stderr,"gridftp_rmdir_done_cb()\n");
#endif
   gconnInx = findGsiFtpFileByHandle(*handle);

   if (gconnInx < 0) {
      elog (NOTICE, "gridftp_rmdir_done_cb: no match handle");
      return;
   }

   fp = &GftpConn[gconnInx];

   if(err)
   {
      save_err_msgg(err);
      if(CredentialsExpired(gsiftpErrMsg))
        gsiftpError = GRIDFTP_NEED_NEW_PROXY_CERTIFICATE;
      else
        gsiftpError = GRIDFTP_RMDIR_DONE_CB_ERR;
   }

   globus_mutex_lock(&fp->lock);
   done = GLOBUS_TRUE;
   globus_cond_signal(&fp->cond);
   globus_mutex_unlock(&fp->lock);
}

int gridftpRmdir(char *url)
{
   char actual_url[MAX_TOKEN * 2], myDn[MAX_TOKEN], urlPrefix[MAX_TOKEN];
   globus_result_t result;
   GSI_FTP_FILE *fp;
   int t;
   OM_uint32 gss_status;
   int gconnInx;

#ifdef _DEBUG
   fprintf(stderr,"gridftpRmdir()\n");
#endif

   t = parseGftpUrl(url, actual_url, urlPrefix, myDn);
   if(t < 0)
     return t;

   gconnInx = gftpConnect(urlPrefix, myDn);
   if (gconnInx < 0) {
        return gconnInx;
   }

   fp = &GftpConn[gconnInx];


   globus_ftp_client_handleattr_init(&(fp->handle_attr));
   globus_ftp_client_operationattr_init(&(fp->attr));
   globus_ftp_client_handle_init(&(fp->handle),  GLOBUS_NULL);

   done = 0;
   result = globus_ftp_client_rmdir(&(fp->handle),
                                   actual_url,
                                   &(fp->attr),
                                   gridftp_rmdir_done_cb,
                                   0);
   if(result != GLOBUS_SUCCESS)
   {
      strcpy(gsiftpErrMsg,globus_object_printable_to_string(globus_error_get(result)));

      gridftp_closefile(gconnInx);

      if(CredentialsExpired(gsiftpErrMsg))
        return GRIDFTP_NEED_NEW_PROXY_CERTIFICATE;

      return GRIDFTP_CLIENT_RMDIR_ERROR;
   }

   fp->handle_is_open = GRIDFTP_HANDLE_OPEN_FOR_RMDIR;

   t = catch_done_cb(fp);
   gridftp_closefile(gconnInx);

   return t;
}

static void gridftp_mkdir_done_cb(void*        user_arg,
        globus_ftp_client_handle_t*            handle,
        globus_object_t*                       err)
{
   GSI_FTP_FILE *fp;
   int gconnInx;

#ifdef _DEBUG
   fprintf(stderr,"gridftp_mkdir_done_cb()\n");
#endif

   gconnInx = findGsiFtpFileByHandle(*handle);

   if(gconnInx < 0) {
      elog (NOTICE, "gridftp_mkdir_done_cb: no match handle");
      return;
   }

   fp = &GftpConn[gconnInx];

   if(err)
   {
      save_err_msgg(err);
      if(CredentialsExpired(gsiftpErrMsg))
        gsiftpError = GRIDFTP_NEED_NEW_PROXY_CERTIFICATE;
      else
        gsiftpError = GRIDFTP_MKDIR_DONE_CB_ERR;
   }

   globus_mutex_lock(&fp->lock);
   done = GLOBUS_TRUE;
   globus_cond_signal(&fp->cond);
   globus_mutex_unlock(&fp->lock);
}

int gridftpMkdir(char *url, int mode)
{
   char actual_url[MAX_TOKEN * 2], myDn[MAX_TOKEN], urlPrefix[MAX_TOKEN];
   globus_result_t result;
   GSI_FTP_FILE *fp;
   int t;
   OM_uint32 gss_status;
   int gconnInx;

#ifdef _DEBUG
   fprintf(stderr,"gridftpMkdir()\n");
#endif

   t = parseGftpUrl(url, actual_url, urlPrefix, myDn);
   if(t < 0)
     return t;

   gconnInx = gftpConnect(urlPrefix, myDn);
   if (gconnInx < 0) {
        return gconnInx;
   }

   fp = &GftpConn[gconnInx];

   done = 0;
   result = globus_ftp_client_mkdir(&(fp->handle),
                                   actual_url,
                                   &(fp->attr),	/* changed from GLOBUS_NULL */
                                   gridftp_mkdir_done_cb,
                                   0);
   if(result != GLOBUS_SUCCESS) {
      strcpy (gsiftpErrMsg,globus_object_printable_to_string(
       globus_error_get(result)));

      gridftp_closefile(gconnInx);

      if(CredentialsExpired(gsiftpErrMsg))
        return GRIDFTP_NEED_NEW_PROXY_CERTIFICATE;

      return GRIDFTP_CLIENT_MKDIR_ERROR;
   }

   fp->handle_is_open = GRIDFTP_HANDLE_OPEN_FOR_MKDIR;

   t = catch_done_cb(fp);
   gridftp_closefile(gconnInx);  

   return t;
}

static void gridftp_unlink_done_cb(void*       user_arg,
        globus_ftp_client_handle_t*            handle,
        globus_object_t*                       err)
{
   GSI_FTP_FILE *fp;
   int gconnInx;

#ifdef _DEBUG2
   fprintf(stderr,"gridftp_unlink_done_cb()\n");
#endif

   gconnInx = findGsiFtpFileByHandle(*handle);

   if (gconnInx < 0) {
      elog (NOTICE, "gridftp_unlink_done_cb: no match handle");
      return;
   }

   fp = &GftpConn[gconnInx];

   if(err)
   {
      save_err_msgg(err);
      if(CredentialsExpired(gsiftpErrMsg))
        gsiftpError = GRIDFTP_NEED_NEW_PROXY_CERTIFICATE;
      else
        gsiftpError = GRIDFTP_UNLINK_DONE_CB_ERR;
   }

   globus_mutex_lock(&fp->lock);
   done = GLOBUS_TRUE;
   globus_cond_signal(&fp->cond);
   globus_mutex_unlock(&fp->lock);
}

int gridftpUnlink(char *url)
{
   char actual_url[MAX_TOKEN * 2], myDn[MAX_TOKEN], urlPrefix[MAX_TOKEN];
   globus_result_t result;
   GSI_FTP_FILE *fp;
   int t;
   OM_uint32 gss_status;
   int gconnInx;


#ifdef _DEBUG
   fprintf(stderr,"gridftpUnlink()\n");
#endif

   t = parseGftpUrl(url, actual_url, urlPrefix, myDn);
   if(t < 0)
     return t;

    gconnInx = gftpConnect(urlPrefix, myDn);
    if (gconnInx < 0) {
        return gconnInx;
    }

    fp = &GftpConn[gconnInx];

   done = 0;
   result = globus_ftp_client_delete(&(fp->handle),
                                   actual_url,
                                   &(fp->attr),
                                   gridftp_unlink_done_cb,
                                   0);
   if(result != GLOBUS_SUCCESS) {
      strcpy (gsiftpErrMsg,globus_object_printable_to_string(
       globus_error_get(result)));

      gridftp_closefile(gconnInx);

      if(CredentialsExpired(gsiftpErrMsg))
        return GRIDFTP_NEED_NEW_PROXY_CERTIFICATE;

      return GRIDFTP_CLIENT_UNLINK_ERROR;
   }

   fp->handle_is_open = GRIDFTP_HANDLE_OPEN_FOR_UNLINK;

   t = catch_done_cb(fp);
   gridftp_closefile(gconnInx);  

   return t;
}

static void file_or_dir_exists_done_cb(void* user_arg,
        globus_ftp_client_handle_t*          handle,
        globus_object_t*                     err)
{
   GSI_FTP_FILE *fp;
   int gconnInx;

#ifdef _DEBUG
   fprintf(stderr,"file_or_dir_exists_done_cb()\n");
#endif

   gconnInx = findGsiFtpFileByHandle(*handle);

   if (gconnInx < 0) {
      elog (NOTICE, "file_or_dir_exists_done_cb: no match handle");
      return;
   }

   fp = &GftpConn[gconnInx];

   if(err)
   {
      save_err_msgg(err);
      if(CredentialsExpired(gsiftpErrMsg))
        gsiftpError = GRIDFTP_NEED_NEW_PROXY_CERTIFICATE;
      else
        gsiftpError = GRIDFTP_FILE_OR_DIR_EXISTS_DONE_CB_ERR;
   }

   globus_mutex_lock(&fp->lock);
   done = GLOBUS_TRUE;
   globus_cond_signal(&fp->cond);
   globus_mutex_unlock(&fp->lock);
}

/* return 0 --> exists, -1 does not exist */
static int file_or_dir_exists(char *url)
{
   char actual_url[MAX_TOKEN * 2], myDn[MAX_TOKEN], urlPrefix[MAX_TOKEN];
   globus_result_t result;
   GSI_FTP_FILE *fp;
   int t;
   OM_uint32 gss_status;
   int gconnInx;

#ifdef _DEBUG2
   fprintf(stderr,"file_or_dir_exists()\n");
#endif

   t = parseGftpUrl(url, actual_url, urlPrefix, myDn);
   if(t < 0)
     return t;

   gconnInx = gftpConnect(urlPrefix, myDn);
   if (gconnInx < 0) {
        return gconnInx;
   }

   fp = &GftpConn[gconnInx];

   done = 0;
   result = globus_ftp_client_exists(&(fp->handle),
				actual_url,
				&(fp->attr),
				file_or_dir_exists_done_cb,NULL);

   if(result != GLOBUS_SUCCESS) {
      strcpy (gsiftpErrMsg,globus_object_printable_to_string(
       globus_error_get(result)));

      gridftp_closefile(gconnInx);

      if(CredentialsExpired(gsiftpErrMsg))
        return GRIDFTP_NEED_NEW_PROXY_CERTIFICATE;

      return GRIDFTP_CLIENT_EXISTS_ERROR;
   }

   fp->handle_is_open = GRIDFTP_HANDLE_OPEN_FOR_FILE_OR_DIR_EXISTS;

   t = catch_done_cb(fp);
   gridftp_closefile(gconnInx);

   return t;
}

static void gridftp_size_done_cb(void*       user_arg,
        globus_ftp_client_handle_t*          handle,
        globus_object_t*                     err)
{
   GSI_FTP_FILE *fp;
   int gconnInx;

#ifdef _DEBUG2
   fprintf(stderr,"gridftp_size_done_cb()\n");
#endif

   gconnInx = findGsiFtpFileByHandle(*handle);

   if (gconnInx < 0) {
      elog (NOTICE, "gridftp_size_done_cb: no match handle");
      return;
   }

   fp = &GftpConn[gconnInx];

   if(err)
   {
      save_err_msgg(err);
      if(CredentialsExpired(gsiftpErrMsg))
        gsiftpError = GRIDFTP_NEED_NEW_PROXY_CERTIFICATE;
      else
        gsiftpError = GRIDFTP_SIZE_DONE_CB_ERR;
   }

   globus_mutex_lock(&fp->lock);
   done = GLOBUS_TRUE;
   globus_cond_signal(&fp-.cond);
   globus_mutex_unlock(&fp->lock);
}

int gridftpFileSize(char *url, long *fsize)
{
   char actual_url[MAX_TOKEN * 2], myDn[MAX_TOKEN], urlPrefix[MAX_TOKEN];
   globus_result_t result;
   globus_off_t file_size;
   GSI_FTP_FILE *fp;
   int t;
   OM_uint32 gss_status;
   int gconnInx;


#ifdef _DEBUG
   fprintf(stderr,"gridftpFileSize()\n");
#endif

   t = parseGftpUrl(url, actual_url, urlPrefix, myDn);
   if(t < 0)
     return t;

   gconnInx = gftpConnect(urlPrefix, myDn);
   if (gconnInx < 0) {
        return gconnInx;
   }

   fp = &GftpConn[gconnInx];

   done = 0;
   result = globus_ftp_client_size(&(fp->handle),
                                actual_url,
                                &(fp->attr),
				&file_size,
                                gridftp_size_done_cb,NULL);

   if(result != GLOBUS_SUCCESS) {
      strcpy (gsiftpErrMsg,globus_object_printable_to_string(
       globus_error_get(result)));
      gridftp_closefile(gconnInx);

      if(CredentialsExpired(gsiftpErrMsg))
        return GRIDFTP_NEED_NEW_PROXY_CERTIFICATE;

      return GRIDFTP_CLIENT_SIZE_ERROR;
   }

   fp->handle_is_open = GRIDFTP_HANDLE_OPEN_FOR_SIZE;

   t = catch_done_cb(fp);
   gridftp_closefile(gconnInx);

   if(t < 0)
     return t;

   *fsize = (long)file_size;
   return 0;
}

int gridftpStat(char *url, struct stat *statbuf)
{
   int t;
   long tl;

#ifdef GFTP_TIMING
    struct timeval diffTime, startTime, endTime;

   (void) gettimeofday(&startTime, (struct timezone *)0);
#endif

#ifdef _DEBUG
   fprintf(stderr,"gridftpStat()\n");
#endif

   t = file_or_dir_exists(url);

#ifdef GFTP_TIMING
   (void) gettimeofday(&endTime, (struct timezone *)0);
   (void) tvsub (&diffTime, &endTime, &startTime);
   fprintf (stderr, "file_or_dir_exists time = %d sec %d usec\n",
    diffTime.tv_sec, diffTime.tv_usec); 
   startTime.tv_sec = endTime.tv_sec;
   startTime.tv_usec = endTime.tv_usec;
#endif

   if(t < 0)
     return t;

   t = gridftpFileSize(url,&tl);

#ifdef GFTP_TIMING
   (void) gettimeofday(&endTime, (struct timezone *)0);
   (void) tvsub (&diffTime, &endTime, &startTime);
   fprintf (stderr, "gridftpFileSize time = %d sec %d usec\n",
    diffTime.tv_sec, diffTime.tv_usec);
   startTime.tv_sec = endTime.tv_sec;
   startTime.tv_usec = endTime.tv_usec;
#endif

   if(t < 0)
   { 
       /* file or directory exist. */
      if(strstr(gsiftpErrMsg,"not a plain file") != NULL)
      {
         /* it is a directory. */
         return 0;
      }

      return t;
   }

   statbuf->st_size = (off_t)tl;

#ifdef _DEBUG
    fprintf(stderr,"gridftp Stat passed.\n");
#endif

   return 0;
}

int gridftpFstat(int gconnInx,struct stat *statbuf)
{
   GSI_FTP_FILE *fp;

#ifdef _DEBUG
   fprintf(stderr,"gridftpFstat()\n");
#endif

   if (gconnInx < 0 || gconnInx >= MAX_GFTP_CONN_INX) {
        elog (NOTICE,
         "gridftpFstat: gconnInx %d out of range", gconnInx);
        return GRIDFTP_GFTP_CONN_OUT_OF_RANG_ERR;
   }

   fp = &GftpConn[gconnInx];

   if(fp->handle_is_open != GRIDFTP_HANDLE_OPEN_FOR_CREATE)
   {
      return GRIDFTP_OPERATION_NOT_SUPPORT;
   }

   statbuf->st_size = (off_t)(fp->offset);
   return 0;
}

void close_grid_ftp_driver_connection()
{
   globus_module_deactivate_all();
}
