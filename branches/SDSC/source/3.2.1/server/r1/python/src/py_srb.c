/********************************************************************************************
 * py_srb.c
 * The module provides the Python glue for SRB's C APIs.
 * Bing Zhu
 * San Diego Supercomputer Center, University of California: San Diego
 * October 17, 2002
 ********************************************************************************************/
#include "Python.h"
#include "pysrb_util.h"

#define py_MAX_DATA_SIZE  20000

static PyObject *py_connect(PyObject *self, PyObject *args)
{
   char *a1,*a2,*a3,*a4,*a5,*a6,*a7;
   int conn_id;

   PyArg_ParseTuple(args,"sssssss",&a1,&a2,&a3,&a4,&a5,&a6,&a7);

   conn_id = MySrbConnect(a1,a2,a3,a4,a5,a6,a7);

   return Py_BuildValue("i",conn_id);
}

static PyObject *py_disconnect(PyObject *self, PyObject *args)
{
   int conn_id;
   PyArg_ParseTuple(args,"i",&conn_id);
   MySrbDisconnect(conn_id);

   return Py_None;
}

static PyObject *py_obj_open(PyObject *self, PyObject *args)
{
   int conn_id;
   char *coll_name,*obj_id;
   int oflag;
   int fd;

   PyArg_ParseTuple(args,"issi",&conn_id,&coll_name,&obj_id,&oflag);

   fd = MySrbOpen(conn_id,coll_name,obj_id,oflag);

   return Py_BuildValue("i",fd);
}

static PyObject *py_obj_read(PyObject *self, PyObject *args)
{
   int conn_id,fd,buf_size;
   char buf[py_MAX_DATA_SIZE+1];
   int read_size;

   PyArg_ParseTuple(args,"iii",&conn_id,&fd,&buf_size);

   if(buf_size > py_MAX_DATA_SIZE)
     buf_size = py_MAX_DATA_SIZE;

   read_size = MySrbRead(conn_id,fd,buf,buf_size);
   if(read_size > 0)
     buf[read_size] = '\0';
   else
   {
      buf[0] = '\0';
      read_size = 0;
   }

   return PyString_FromStringAndSize(buf, read_size);
}

static PyObject *py_obj_close(PyObject *self, PyObject *args)
{
   int conn_id,fd;
   int close_status;

   PyArg_ParseTuple(args,"ii",&conn_id,&fd);
   close_status = MySrbClose(conn_id,fd); 
   return Py_BuildValue("i",close_status);
}

static PyObject *py_obj_create(PyObject *self, PyObject *args)
{
   int conn_id;
   int creat_status;
   int cat_type;
   char *obj_id,*data_type_name,*resc_name,*coll_name,*path_name;
   int data_size;

   PyArg_ParseTuple(args,"iisssssi",
       &conn_id,&cat_type,&obj_id,&data_type_name,
       &resc_name,&coll_name,&path_name,&data_size);  

   creat_status = MySrbCreate(conn_id,cat_type,obj_id,data_type_name,
                      resc_name,coll_name,path_name,data_size);

   return Py_BuildValue("i",creat_status);
}

static PyObject *py_obj_write(PyObject *self, PyObject *args)
{
   int conn_id,fd,len;
   char *buf;
   int write_status;

   PyArg_ParseTuple(args,"iisi",&conn_id,&fd,&buf,&len);

   write_status =  MySrbWrite(conn_id,fd,buf,len);

   return Py_BuildValue("i",write_status);
}

/*   whence = 0 --> SEEK_SET
            = 1 --> SEEK_CUR
            = 2 --> SEEK_END
 */
static PyObject *py_obj_seek(PyObject *self, PyObject *args)
{
   int conn_id,fd,offset,whence;
   int seek_status;

   PyArg_ParseTuple(args,"iiii",&conn_id,&fd,&offset,&whence);

   seek_status =  MySrbSeek(conn_id,fd,offset,whence);

   return Py_BuildValue("i",seek_status);
}

static PyObject *py_obj_delete(PyObject *self, PyObject *args)
{
   int conn_id;
   char *objname,*coll;
   int copy_num;
   int rmStatus;

   PyArg_ParseTuple(args,"isis",&conn_id,&objname,&copy_num,&coll);

   rmStatus = MySrbDeleteObj(conn_id,objname,copy_num,coll);

   return Py_BuildValue("i",rmStatus);
}

static PyObject *py_mk_collection(PyObject *self, PyObject *args)
{
   int conn_id;
   char *par_coll,*new_coll_name;
   int cat_type;
   int mkStatus;

   PyArg_ParseTuple(args,"iiss",&conn_id,&cat_type,&par_coll,&new_coll_name);

   mkStatus = MySrbMkColl(conn_id,cat_type,par_coll,new_coll_name);

   return Py_BuildValue("i",mkStatus);
}

static PyObject *py_rm_collection(PyObject *self, PyObject *args)
{
   int conn_id, cat_type;
   char *coll;
   int recursive;  /* 1 or 0 ==> yes or no  */
   int rmStatus;

   recursive = 0;

   PyArg_ParseTuple(args,"iiis",&conn_id,&cat_type,&recursive,&coll);

   rmStatus = MySrbRmColl(conn_id,cat_type,recursive,coll);

   return Py_BuildValue("i",rmStatus);
}

static PyObject *py_get_objs_in_coll(PyObject *self, PyObject *args)
{
   int conn_id;
   int cat_type,flag_val;
   char *coll;
   int queryStatus;

   PyArg_ParseTuple(args,"iiis",&conn_id,&cat_type,&flag_val,&coll);

   queryStatus = MySrbGetCObjs(conn_id,cat_type,flag_val,coll);

   return Py_BuildValue("i",queryStatus);
}

static PyObject *py_get_obj_metadata(PyObject *self, PyObject *args)
{
   int conn_id;
   int meta_idx,row_id;
   int len;
   char *metastr;

   PyArg_ParseTuple(args,"iii",&conn_id,&meta_idx,&row_id);

   metastr = MySrbCObjMetadata(conn_id,meta_idx,row_id);

   len = 0;
   if(metastr != NULL)
     len = strlen(metastr);

   return PyString_FromStringAndSize(metastr,len);
}

static PyObject *py_more_objs_in_coll(PyObject *self, PyObject *args)
{
   int conn_id;
   int moreStatus;

   PyArg_ParseTuple(args,"i",&conn_id);

   moreStatus = MySrbGetMoreCObjs(conn_id);

   return Py_BuildValue("i",moreStatus);    
}

static PyObject *py_get_subcolls(PyObject *self, PyObject *args)
{
   int conn_id; 
   int cat_type;
   int subcollStatus;
   char *coll;

   PyArg_ParseTuple(args,"iis",&conn_id,&cat_type,&coll);

   subcollStatus = MySrbGetSubcolls(conn_id,cat_type,coll);

   return Py_BuildValue("i",subcollStatus);
}

static PyObject *py_get_subcoll_name(PyObject *self, PyObject *args)
{
   int conn_id;
   int row_idx;
   int subnameStatus;
   char *cname;
   int len;

   PyArg_ParseTuple(args,"ii",&conn_id,&row_idx);

   cname = MySrbSubcollNames(conn_id,row_idx);

   len =0;
   if(cname != NULL)
     len = strlen(cname);

   return PyString_FromStringAndSize(cname,len);
}

static PyObject *py_more_subcolls(PyObject *self, PyObject *args)
{
   int conn_id;
   int morecollStatus;

   PyArg_ParseTuple(args,"i",&conn_id);

   morecollStatus = MySrbMoreSubcolls(conn_id);

   return Py_BuildValue("i",morecollStatus);
}

static PyMethodDef srb_methods[] =
{
   {"connect",py_connect,METH_VARARGS,"connect"},
   {"disconnect",py_disconnect,METH_VARARGS,"disconnect"},
   {"obj_open",py_obj_open,METH_VARARGS,"obj_open"},
   {"obj_read",py_obj_read,METH_VARARGS,"obj_read"},
   {"obj_close",py_obj_close,METH_VARARGS,"obj_close"},
   {"obj_create",py_obj_create,METH_VARARGS,"obj_create"},
   {"obj_write",py_obj_write,METH_VARARGS,"obj_write"},
   {"obj_seek",py_obj_seek,METH_VARARGS,"obj_seek"},
   {"obj_delete",py_obj_delete,METH_VARARGS,"obj_delete"},
   {"mk_collection",py_mk_collection,METH_VARARGS,"mk_collection"},
   {"rm_collection",py_rm_collection,METH_VARARGS,"rm_collection"},
   {"get_objs_in_coll",py_get_objs_in_coll,METH_VARARGS,"get_objs_in_coll"},
   {"get_obj_metadata",py_get_obj_metadata,METH_VARARGS,"get_obj_metadata"},
   {"more_objs_in_coll",py_more_objs_in_coll,METH_VARARGS,"more_objs_in_coll"},
   {"get_subcolls",py_get_subcolls,METH_VARARGS,"get_subcolls"},
   {"get_subcoll_name",py_get_subcoll_name,METH_VARARGS,"get_subcoll_name"},
   {"more_subcolls",py_more_subcolls,METH_VARARGS,"more_subcolls"},
   {NULL,NULL,0,NULL}
};

void initsrb()
{
   (void)Py_InitModule("srb",srb_methods);
}
