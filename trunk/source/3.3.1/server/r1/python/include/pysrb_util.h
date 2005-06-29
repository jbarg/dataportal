/* pysrb_util.h */
#ifndef __pysrb_util_h__
#define __pysrb_util_h__

#include "mdasGlobalsExtern.h"
#include "clAuthExtern.h"
#include "clConnectExtern.h"
#include "scommands.h"

int MySrbConnect(char *srb_host, char *srb_port,
                char *srb_domain, char *auth_scheme,
                char *user_name, char *srb_passwd, char *svr_dn);

void MySrbDisconnect(int conn_id);

int MySrbOpen(int conn_id,char *coll_name,char *obj_id, int oflag);
int MySrbRead(int conn_id, int fd, char *buf, int buf_size);
int MySrbClose(int conn_id, int fd);
int MySrbCreate(int conn_id,int catType, char *objID,
      char *dataTypeName, char *resourceName, char *collectionName,
      char *pathName, srb_long_t dataSize);
int MySrbWrite(int conn_id,int fd,char *buf,int len);
int MySrbSeek(int conn_id,int fd, int offset, int whence);
int MySrbDeleteObj(int conn_id,char *objname,int copy_num,char *par_coll);
int MySrbMkColl(int conn_id,int cat_type,char *par_coll,char *new_coll);
int MySrbRmColl(int conn_id,int cat_type,int recursive,char *coll);
int MySrbGetCObjs(int conn_id, int cat_type, int flag_val, char *coll_name);
char *MySrbCObjMetadata(int conn_id,int metadata_idx,int row_idx);
int MySrbGetMoreCObjs(int conn_id);
int MySrbGetSubcolls(int conn_id,int cat_type,char *coll);
char *MySrbSubcollNames(int conn_id, int row_idx);
int MySrbMoreSubcolls(int conn_id);


#endif
