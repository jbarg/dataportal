/********************************************************************************************
 * pysrb_util.c
 * The module provides the a layer between Python glue and SRB's C APIs. 
 * Bing Zhu
 * San Diego Supercomputer Center, University of California: San Diego
 * October 17, 2002
 ********************************************************************************************/
#include "pysrb_util.h"

extern char srbUser[];
/*extern userInfo *proxyUser,*clientUser; */

#define MAX_CONN 40
#define MAX_QUERIES 100

static srbConn *g_conn[MAX_CONN]={NULL};
static mdasC_sql_result_struct g_data_result[MAX_CONN];
static int g_data_flag_val[MAX_CONN];
static int g_data_cat_type[MAX_CONN];
static mdasC_sql_result_struct g_coll_result[MAX_CONN];
static int g_coll_flag_val[MAX_CONN];
static int g_coll_cat_type[MAX_CONN];


/*
 * auth_scheme --> GSI_AUTH, ENCRYPT1, PASSWD_AUTH
 */
int MySrbConnect(char *srb_host, char *srb_port,
		char *srb_domain, char *auth_scheme,
 		char *user_name, char *srb_passwd, char *svr_dn)         
{
   char  buff[1024];
   int cid;

   cid = -1;
   for(i=0;i<MAX_CONN;i++)
   {
      if(g_conn[i] == NULL)
      {
         cid = i+1;
         goto MySrbConnect_GET_ID;
      }
   }

   if(cid < 0)   /* run out of connection IDs. */
     return SYS_ERR_DESC_OVERFLOW;

MySrbConnect_GET_ID:
   
   sprintf(buff,"AUTH_SCHEME=%s",auth_scheme);
   putenv(buff);
   strcpy(srbUser,user_name);

   g_conn[cid-1] = srbConnect(srb_host,srb_port,srb_passwd,user_name,srb_domain,auth_scheme,svr_dn);

   if (clStatus(g_conn[cid-1]) != CLI_CONNECTION_OK)
   {
      return clStatus(g_conn[cid-1]);
   }

   return cid;
}

void MySrbDisconnect(int conn_id)
{
   int t;

   t = conn_id -1;
   if(g_conn[t] != NULL)
   {
      clFinish(g_conn[t]);
      g_conn[t] = NULL;

/*
      if(proxyUser != NULL)
                free(proxyUser);

      proxyUser = NULL;
*/
   }
}

int MySrbOpen(int conn_id,char *coll_name,char *obj_id, int oflag)
{
   int t;

   t = conn_id - 1;

   if(g_conn[t] == NULL)
     return -1;

   return srbObjOpen(g_conn[t],obj_id,oflag,coll_name);
}

int MySrbRead(int conn_id, int fd, char *buf, int buf_size)
{
   int t;

   t = conn_id - 1;

   if(g_conn[t] == NULL)
     return -1;

   return srbObjRead(g_conn[t],fd,buf,buf_size);
}

int MySrbClose(int conn_id, int fd)
{
  int t;

   t = conn_id - 1;

   if(g_conn[t] == NULL)
     return -1;

  return srbObjClose(g_conn[t],fd);
}

int MySrbCreate(int conn_id,int catType, char *objID,
      char *dataTypeName, char *resourceName, char *collectionName,
      char *pathName, srb_long_t dataSize)
{
   int t;

   t = conn_id - 1;

   if(g_conn[t] == NULL)
     return -1;

   return srbObjCreate(g_conn[t],catType,objID,dataTypeName,resourceName,collectionName,pathName,dataSize);
}

int MySrbWrite(int conn_id,int fd,char *buf,int len)
{
   int t;

   t = conn_id - 1;

   if(g_conn[t] == NULL)
     return -1;

   return srbObjWrite(g_conn[t],fd,buf,len);
}

int MySrbSeek(int conn_id,int fd, int offset, int whence)
{
   int t;
   srb_long_t long_offset;

   t = conn_id - 1;

   if(g_conn[t] == NULL)
     return -1;

   long_offset = (srb_long_t)offset;

   return srbObjSeek(g_conn[t],fd,offset,whence);
}

int MySrbDeleteObj(int conn_id,char *objname,int copy_num,char *par_coll)
/* copy_num = -1 means delete all copies */
{
   int t;
   int delStatus;
   char temp[2048];

   t = conn_id - 1;

   if(g_conn[t] == NULL)
     return -1;

   if(copy_num == -1)
   {
      delStatus = srbObjUnlink(g_conn[t],objname,par_coll);
   }
   else
   {
      sprintf(temp, "%s&COPY=%i",objname, copy_num);
      delStatus = srbObjUnlink (g_conn[t], temp, par_coll);
   }

   return delStatus;
}

int MySrbMkColl(int conn_id,int cat_type,char *par_coll,char *new_coll)
{
   int t;

   t = conn_id - 1;

   if(g_conn[t] == NULL)
     return -1;

   return srbCreateCollect(g_conn[t],cat_type,par_coll,new_coll);
}

int MySrbRmColl(int conn_id,int cat_type,int recursive,char *coll)
{
   mdasC_sql_result_struct myresult;
   int t;

   t = conn_id - 1;

   if(g_conn[t] == NULL)
     return -1;


   if(recursive)
   {
      myresult.result_count = 1;
      myresult.row_count = 1;
      myresult.sqlresult[0].tab_name = dcs_tname[DATA_GRP_NAME];
      myresult.sqlresult[0].att_name = dcs_aname[DATA_GRP_NAME];
      myresult.sqlresult[0].values = (mdasC_handle)  coll;
      return rmColl(g_conn[t],cat_type,R_FLAG,&myresult,-1);
   }

   return srbModifyCollect(g_conn[t], MDAS_CATALOG, coll,
                         "", "", "", D_DELETE_COLL);
}

/********************************************************************
                         MCAT operation
 ********************************************************************/

/* The data_idx
   0  -> Dataset name
   1  -> Datset's Parent Collection Name
   2  -> Dataset Size
   3  -> Data Type
   4  -> Owner
   5  -> Time Stamp
   6  -> Replica Index. 0 means the original copy. 1 means second copy...
   7  -> Resource Name
 */

/* The function returns an integer to indicate its query ID. */
int MySrbGetCObjs(int conn_id, int cat_type, int flag_val, char *coll_name)
{
   int t;
   int query_status;
   
   t = conn_id - 1;
   
   if(g_conn[t] == NULL)
     return -1;  

   clearSqlResult(&(g_data_result[t]));

   query_status = queryDataInColl(g_conn[t],MDAS_CATALOG,coll_name,
			flag_val,MAX_GET_RESULTS, &(g_data_result[t]));

   if(query_status < 0)
   {
      if(query_status == MCAT_INQUIRE_ERROR)
        return 0;

      return query_status;
   }

   g_data_flag_val[t] = flag_val;
   g_data_cat_type[t] = MDAS_CATALOG;

   return g_data_result[t].row_count;
}

char *MySrbCObjMetadata(int conn_id,int metadata_idx,int row_idx)
{
   int t;
   char *tmpList;
   int actual_meta_idx;

   t = conn_id - 1;

   if(g_conn[t] == NULL)
     return NULL;

   if(((g_data_flag_val[t]&l_FLAG)==0)&&(metadata_idx > 0))
   {
      return NULL;
   }

   switch(metadata_idx)
   {
      case 0:
        actual_meta_idx = DATA_NAME;
        break;
      case 1:
        actual_meta_idx = DATA_GRP_NAME;
        break;
      case 2:
        actual_meta_idx = SIZE;
        break;
      case 3:
        actual_meta_idx = DATA_TYP_NAME;
        break;
      case 4:
        actual_meta_idx = DATA_OWNER;
        break;
      case 5:
        actual_meta_idx = REPL_TIMESTAMP;
        break;
      case 6:
        actual_meta_idx = DATA_REPL_ENUM;
        break;
      case 7:
        actual_meta_idx = PHY_RSRC_NAME;
        break;
   }

   tmpList = (char *)getFromResultStruct((mdasC_sql_result_struct *)&(g_data_result[t]),
                          dcs_tname[actual_meta_idx], dcs_aname[actual_meta_idx]);
   tmpList = tmpList + row_idx*MAX_DATA_SIZE;
   return strdup(tmpList);
}

int MySrbGetMoreCObjs(int conn_id)
{
   int t;
   char *tmpList;
   int actual_meta_idx;
   int morerow_status;

   t = conn_id - 1;

   if(g_conn[t] == NULL)
     return -1;

   if(g_data_result[t].continuation_index < 0)
     return 0;

   clearSqlResult(&(g_data_result[t]));  
   morerow_status = srbGetMoreRows(g_conn[t],g_data_cat_type[t],
			g_data_result[t].continuation_index,
			&(g_data_result[t]),MAX_GET_RESULTS);

   if(morerow_status < 0)
   {
      return 0;
   }

   return g_data_result[t].row_count;
}

int MySrbGetSubcolls(int conn_id,int cat_type,char *coll)
{
   int t;
   int queryStatus;

   t = conn_id - 1;

   if(g_conn[t] == NULL)
     return -1;

   clearSqlResult(&(g_coll_result[t]));

   queryStatus = querySubCollInColl(g_conn[t],cat_type,coll,0,MAX_GET_RESULTS,&(g_coll_result[t]));

   if(queryStatus < 0)
   {
      if(queryStatus == MCAT_INQUIRE_ERROR)
        return 0;

      return queryStatus;
   }

   g_coll_cat_type[t] = cat_type;
   return g_coll_result[t].row_count;
}

char *MySrbSubcollNames(int conn_id, int row_idx)
{
   char *tmpList;
   int t;

   t = conn_id - 1;

   if(g_conn[t] == NULL)
     return NULL;

   tmpList = (char *)getFromResultStruct((mdasC_sql_result_struct *)&(g_coll_result[t]),
                          dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
   tmpList = tmpList + row_idx*MAX_DATA_SIZE;

   return strdup(tmpList);
}

int MySrbMoreSubcolls(int conn_id)
{
   int t;
   int morerowStatus;

   t = conn_id - 1;

   if(g_conn[t] == NULL)
     return -1;


   if(g_coll_result[t].continuation_index < 0)
     return 0;

   clearSqlResult(&(g_coll_result[t]));
   morerowStatus = srbGetMoreRows(g_conn[t], g_coll_cat_type[t], 
         g_coll_result[t].continuation_index, &(g_coll_result[t]),MAX_GET_RESULTS);

   if(morerowStatus < 0)
     return 0;
  
   return g_coll_result[t].row_count;
}
