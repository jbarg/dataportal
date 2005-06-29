#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <limits.h>

#include "scommands.h"
#include "datascope.h"

/* extern variables for SRB enviroment reading */
extern char srbUser[];
extern char srbAuth[];
extern char srbHost[];
extern char mdasDomainName[];
extern char mdasCollectionName[];
extern char mdasCollectionHome[];
extern char inCondition[];

srbConn *conn;
char temp_buff[1000];

/* some compilers don't support __func__, some others don't support __FUNCTION__ */
#ifndef __func__
#ifdef __FUNCTION__
#define __func__  __FUNCTION__
#else
#define __func__  "unknown"
#endif
#endif

/* some compilers don't support INT_MAX */
#ifndef INT_MAX
#define INT_MAX 2147483647
#endif

/* inline functions */
#ifndef FREEIF
#define FREEIF(pi)		{ if (pi) { free((void *)pi); (pi)=0; }}
#endif

#ifndef STRNCPY
#define STRNCPY(dest, src, n)	{ strncpy(dest,src,n); dest[n-1]=0; }
#endif

#ifndef MALLOC_SAFE
#define MALLOC_SAFE(dest,size) { dest=malloc(size); if(NULL==dest) {fprintf(stderr,"Error: out of memory at %s:%s\n",__FILE__,__LINE__); exit(-1);} else memset(dest,0,size); }
#endif

#ifndef STRDUP_SAFE
#define STRDUP_SAFE(newstr,oldstr) { newstr=strdup(oldstr); if(NULL==newstr) {fprintf(stderr,"Error: out of memory at %s:%s\n",__FILE__,__LINE__); exit(-1);} }
#endif

#ifndef STRNDUP_SAFE
#define STRNDUP_SAFE(newstr,oldstr,size) { MALLOC_SAFE(newstr,size+sizeof(char)*2); STRNCPY(newstr, oldstr, size+sizeof(char)); }
#endif

#ifndef DIE
#define DIE(str) do {fprintf(stderr, "ERROR:\"%s:%u\" %s: ",__FILE__,__LINE__, __func__ ); fprintf(stderr, str); exit(-1);} while(0)
#endif

#ifndef  WARNING
#define WARNING(str) do { fprintf(stderr, "WARNING:\"%s:%u\" %s: ",__FILE__,__LINE__, __func__ ); fprintf(stderr, str); } while(0)
#endif

#ifndef  DEBUG
#define DEBUG(str) do { fprintf(stderr, "DEBUG:\"%s:%u\" %s: ",__FILE__,__LINE__, __func__ ); fprintf(stderr, str); } while(0)
#endif


/*
#ifndef DIE
#define DIE(fmt, args...) do{ fprintf(stderr, "ERROR:\"%s:%u\" %s: ",__FILE__,__LINE__, __func__ ); fprintf(stderr, fmt , ## args); exit(-1); }while(0)
#endif

#ifndef  WARNING
#define  WARNING(fmt, args...) { fprintf(stderr, "WARNING:\"%s:%u\" %s: ",__FILE__,__LINE__, __func__ ); fprintf(stderr, fmt , ## args); } 
#endif

#ifndef  DEBUG
#define  DEBUG(fmt, args...) { fprintf(stderr, "DEBUG:\"%s:%u\" %s: ",__FILE__,__LINE__, __func__ ); fprintf(stderr, fmt , ## args); }
#endif
*/

/* constants specific for this function */
#define  MAX_PROC_ARGS_FOR_DS 100
#define  MAX_DBPTR_STRLEN 100   /* max strlen of dbptr */
#define  DBTABLE_PREFIX_DELIMITER "___" /* delimiter for database table name and prefix   */
                                        /* for eg., if table name is wfdisc in datascope  */
                                        /* and prefix is sdsc, then the acctaul table name*/
                                        /* in database (sql based) will be: sdsc___wfdisc */

typedef struct DSptr
{
    int database,
        table,
	      field,
	      record;
} DSptr;

typedef struct DSSchemaField
{
    char *name;
    DSptr dsptr;
    int  type;
    char format[100];
    int  size;
}              DSSchemaField;

typedef struct DSSchemaTable
{
    char *name;
    DSptr dsptr;
    int numrecord;
    int numfield;
    DSSchemaField *fields;
}              DSSchemaTable;

typedef struct DSSchemaDatabase
{
    char *name_prefix;    /* table name prefix */
    char *field_prefix;   /* field name prefix */
    DSptr dsptr;
    int numtable;
    DSSchemaTable *tables;
}              DSSchemaDatabase;

int strTokenize(const char *instring, char delim, char ***outstring);
void dsPtr2str(DSptr* datascopedbPtr,  char *outBuf);
int str2dsPtr(char *inBuf, DSptr* datascopedbPtr);
int DSopen(srbConn *srb_conn, char *datascope_object_path);
DSSchemaDatabase* readDSSchema (srbConn *srb_conn, int srb_obj_fd, char *name_prefix, char *field_prefix);
void freeDSSchemaDatabase(DSSchemaDatabase *ds);
char *genNamePrefix(void);
void dumpDSSchemaField2SQL(DSSchemaField *field, char *field_prefix, FILE *fp);
void dumpDSSchema2SQL(DSSchemaDatabase *ds_db, int drop_table_needed, FILE *fp);
void usage (char *prog);
 
/** 
 * split a string by delim and put into a newly allocated string array
 * 
 * @param instring input string
 * @param delim delimiter
 * @param outstring output, (pointer to) an array of strings.
 * @return number of token.
 */
int 
strTokenize(const char *instring, char delim, char ***outstring)
{
  int i=0, num_tok=0, tok_len;
  char *start, *end;
  
  /* count how many tokens are there */
  start=(char *)instring;
  while(NULL!=(end=strchr(start, delim))) 
  {
    num_tok++;
    start=end+sizeof(delim);
  }
  num_tok++;
  
  if(0>=num_tok) return 0;
  
  /* allocate memory to store array of char * */
  MALLOC_SAFE(*outstring, num_tok*sizeof(void *));
  
  /* allocate and assign each char * within the array */
  start=(char *)instring;
  while(end=strchr(start, delim))
  {
    tok_len=end-start;
    STRNDUP_SAFE((*outstring)[i],start,tok_len);
    start=end+sizeof(delim);
    i++;
  }
  
  /* get the last token */
  STRDUP_SAFE((*outstring)[i],start);
  
  return num_tok;
}

void
dsPtr2str(DSptr* datascopedbPtr,  char *outBuf)
{
    sprintf(outBuf, "%i|%i|%i|%i", 
            datascopedbPtr->database,
            datascopedbPtr->table,
            datascopedbPtr->field,
            datascopedbPtr->record);
}

int
str2dsPtr(char *inBuf, DSptr* datascopedbPtr) 
{
    int i,numarg;
    char **argv;
    numarg=strTokenize(inBuf,'|',&argv);
    if (numarg < 4) {
        fprintf(stderr, "Error: wrong number of arguments in dsptr. %d\n",numarg);
        exit(-1);
    }
    
    datascopedbPtr->database = atoi(argv[0]);
    datascopedbPtr->table =  atoi(argv[1]);
    datascopedbPtr->field =  atoi(argv[2]);
    datascopedbPtr->record =  atoi(argv[3]);
    for(i=0;i<numarg;i++)
    {
      FREEIF(argv[i]);
    }
    FREEIF(argv);
    return(0);
}

/** 
 * open a datascope object (SRB)
 * 
 * @param srb_conn srb connection
 * @param datascope_object_path path of the object
 * @return file descriptor.
 */
int 
DSopen(srbConn *srb_conn, char *datascope_object_path)
{
  char collname[MAX_TOKEN];
  char dataname[MAX_OBJ_NAME_LENGTH];
  srbPathName srcNameArray[1];
  int srb_obj_fd;
  
  srcNameArray[0].inpArgv=datascope_object_path;
  
  if (convNameArrayToSQL (srcNameArray, 1) < 0)
  {
    fprintf(stderr,"Error: Incorrect datascope object path!\n");
    clFinish(conn); 
    exit(3);
  }
    
  splitpath(srcNameArray[0].outArgv, collname, dataname);
  
  printf("srcNameArray[0].inpArgv=%s, nameArray[0].type=%d\n",
    srcNameArray[0].outArgv,srcNameArray[0].type);
  printf("collname=%s, dataname=%s\n",collname,dataname);
  
  srb_obj_fd = srbObjOpen (srb_conn, dataname,  O_RDONLY, collname);
  if (srb_obj_fd < 0)  {   /* error */
      fprintf(stderr, "Error: Cannot Open Object. srb_obj_fd=%d\n", srb_obj_fd);
      srb_perror (2, srb_obj_fd, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
      clFinish(srb_conn);
      exit(-1);
  }
  
  return srb_obj_fd;
}    

/** 
 * read the datascope object, and convert into table structure (DSSchemaTable)
 * 
 * @param srb_conn srb connection
 * @param srb_obj_fd path descriptor
 * @param name_prefix prefix for all tables.
 * @return database structure, must be freed by caller!
 */  
DSSchemaDatabase *
readDSSchema (srbConn *srb_conn, int srb_obj_fd, char *name_prefix, char *field_prefix)
{
    char inStr[HUGE_STRING], smallbuf[HUGE_STRING];
    int i,j, status, num_table;
    char **tablenames, **fieldnames, 
         dsptr_str_database[MAX_PROC_ARGS_FOR_DS]={0},
         dsptr_str_table[MAX_PROC_ARGS_FOR_DS]={0},
         dsptr_str_field[MAX_PROC_ARGS_FOR_DS]={0};
    DSSchemaDatabase *database;     
    DSSchemaTable *tables;
    DSptr dsptr_database;
    
    /* get db/dsptr that's passed in by user, and fixed it to dsptr           */
    /* pointing to THE database, but not any table or field... to make        */
    /* ds/srb driver happy. so it's something like 0|-501|-501|-501           */
    memset(smallbuf,0,sizeof(smallbuf));
    sprintf(inStr, "get_dbptr");
    status = srbObjProc(srb_conn,srb_obj_fd,inStr,"",0,smallbuf,sizeof(smallbuf));
    if (status<0)
    {
        fprintf(stderr, "Error: srbObjProc failed.0 ret_val=%d\n",status);
        exit(-1);
    }
    if (0!=str2dsPtr(smallbuf,&dsptr_database))
    {
      fprintf(stderr, "Error: Invalid datascope pointer!\n");
      exit (-1);
    }
    if (dbALL!=dsptr_database.table) dsptr_database.table=dbALL;
    if (dbALL!=dsptr_database.field) dsptr_database.field=dbALL;
    if (dbALL!=dsptr_database.record) dsptr_database.record=dbALL;
    dsPtr2str(&dsptr_database, dsptr_str_database);
    
    /* get all tables' names within db */
    memset(smallbuf,0,sizeof(smallbuf));
    sprintf(inStr, "dbquery|%d",dbSCHEMA_TABLES);
    status = srbObjProc(srb_conn,srb_obj_fd,inStr,"",0,smallbuf,sizeof(smallbuf));
    if (status<0)
    {
        fprintf(stderr, "Error: srbObjProc failed.1 ret_val=%d\n",status);
        exit(-1);
    }
    
    /* parse and assign ds table names to tablenames */
    num_table=strTokenize(smallbuf,'|',&tablenames);
    if (num_table<=0)
    {
      fprintf(stderr, "Error: no related datascope table found \n");
      exit(-1);
    }
    
    /* allocate array to hold all table objects */
    MALLOC_SAFE(tables,num_table*sizeof(*tables));
    
    /* assign each table's tablename, dsptr and fields*/ 
    for (i=0;i<num_table;i++)
    {
      /* tablenames */
      STRDUP_SAFE(tables[i].name,tablenames[i]);
      
      /* table dsptr */ 
      memset(smallbuf,0,sizeof(smallbuf));
      sprintf(inStr, "dbopen_table|%s|0",tables[i].name);
      status = srbObjProc(srb_conn,srb_obj_fd,inStr,dsptr_str_database,
        sizeof(dsptr_str_database),smallbuf,sizeof(smallbuf));
      if (status<0)
      {
          fprintf(stderr, "Error: srbObjProc failed.2.%d ret_val=%d\n",i,status);
          exit(-1);
      }
      if(0!=str2dsPtr(smallbuf,&(tables[i].dsptr)))
      {
        fprintf(stderr, "Error: Invalid datascope pointer!\n");
        exit (-1);
      }
      dsPtr2str(&(tables[i].dsptr),dsptr_str_table);
      
      /* numrecord */
      memset(smallbuf,0,sizeof(smallbuf));
      sprintf(inStr, "dbquery|%d",dbRECORD_COUNT);
      status = srbObjProc(srb_conn,srb_obj_fd,inStr,dsptr_str_table,
        sizeof(dsptr_str_table),smallbuf,sizeof(smallbuf));
      if (status<0)
      {
          fprintf(stderr, "Error: srbObjProc failed.3.%d ret_val=%d \n",
            i,status);
          exit(-1);
      }
      tables[i].numrecord=atoi(smallbuf);
      
      /* fields */
      memset(smallbuf,0,sizeof(smallbuf));
      sprintf(inStr, "dbquery|%d",dbTABLE_FIELDS);
      status = srbObjProc(srb_conn,srb_obj_fd,inStr,dsptr_str_table,
        sizeof(dsptr_str_table),smallbuf,sizeof(smallbuf));
      if (status<0)
      {
          fprintf(stderr, "Error: srbObjProc failed.4.%d ret_val=%d, "
                          "dsptr_str_database=%s, dsptr_str_table=%s\n",
            i,status,dsptr_str_database,dsptr_str_table);
          exit(-1);
      }
      
      tables[i].numfield=strTokenize(smallbuf,'|',&fieldnames);
      MALLOC_SAFE(tables[i].fields, tables[i].numfield*sizeof(*tables[i].fields));
      
      for (j=0;j<tables[i].numfield;j++)
      {
        /* field names, it must not contain '|', because srb-ds uses as a delimiter */
        STRDUP_SAFE(tables[i].fields[j].name,fieldnames[j]);
        if (NULL!=strchr(tables[i].fields[j].name,'|'))
        {
          sprintf(temp_buff,"illegal character '|' in fieldname!\n");
          DIE(temp_buff);
        }
        
        /* field dsptr */
        memset(smallbuf,0,sizeof(smallbuf));
        sprintf(inStr, "dblookup||%s|%s|",tables[i].name,tables[i].fields[j].name);
        status = srbObjProc(srb_conn,srb_obj_fd,inStr,dsptr_str_database,
          sizeof(dsptr_str_database),smallbuf,sizeof(smallbuf));
        if (status<0)
        {
            fprintf(stderr, "Error: srbObjProc failed.5.%d.%d ret_val=%d\n",i,j,status);
            exit(-1);
        }
        if(0!=str2dsPtr(smallbuf,&(tables[i].fields[j].dsptr)))
        {
          fprintf(stderr, "Error: Invalid datascope pointer!\n");
          exit (-1);
        }
        dsPtr2str(&(tables[i].fields[j].dsptr),dsptr_str_field);
        
        /* field type */
        memset(smallbuf,0,sizeof(smallbuf));
        sprintf(inStr, "dbquery|%d",dbFIELD_TYPE);
        status = srbObjProc(srb_conn,srb_obj_fd,inStr,dsptr_str_field,
          sizeof(dsptr_str_field),smallbuf,sizeof(smallbuf));
        if (status<0)
        {
            fprintf(stderr, "Error: srbObjProc failed.6.%d.%d ret_val=%d ",i,j,status);
            fprintf(stderr, "dsptr_str_field=%s\n",dsptr_str_field);
            exit(-1);
        }
        tables[i].fields[j].type=atoi(smallbuf);
        
        /* field size */
        memset(smallbuf,0,sizeof(smallbuf));
        sprintf(inStr, "dbquery|%d",dbFIELD_SIZE);
        status = srbObjProc(srb_conn,srb_obj_fd,inStr,dsptr_str_field,
          sizeof(dsptr_str_field),smallbuf,sizeof(smallbuf));
        if (status<0)
        {
            fprintf(stderr, "Error: srbObjProc failed.7.%d.%d ret_val=%d ",i,j,status);
            fprintf(stderr, "dsptr_str_field=%s\n",dsptr_str_field);
            exit(-1);
        }
        tables[i].fields[j].size=atoi(smallbuf);
        
        /* field format */
        memset(smallbuf,0,sizeof(smallbuf));
        sprintf(inStr, "dbquery|%d",dbFIELD_FORMAT);
        status = srbObjProc(srb_conn,srb_obj_fd,inStr,dsptr_str_field,
          sizeof(dsptr_str_field),smallbuf,sizeof(smallbuf));
        if (status<0)
        {
            fprintf(stderr, "Error: srbObjProc failed.7.%d.%d ret_val=%d ",i,j,status);
            fprintf(stderr, "dsptr_str_field=%s\n",dsptr_str_field);
            exit(-1);
        }
        STRNCPY(tables[i].fields[j].format,smallbuf,sizeof(tables[i].fields[j].format));
      }
      
      /* free fieldnames */
      for (j=0;j<tables[i].numfield;j++)
      {
        FREEIF(fieldnames[j]);
      }
      FREEIF(fieldnames);
    }
    
    /* free tablenames */
    for (j=0;j<num_table;j++)
    {
      FREEIF(tablenames[j]);
    }
    FREEIF(tablenames);
    
    /* assign returning values */
    MALLOC_SAFE(database,sizeof(*database));
    STRDUP_SAFE(database->name_prefix, name_prefix);
    STRDUP_SAFE(database->field_prefix, field_prefix);
    database->dsptr=dsptr_database;
    database->numtable=num_table;
    database->tables=tables;
    
    return database;
}

/** 
 * free a DSSchemaDatabase (dynamically allocated)
 * 
 * @param ds DSSchemaDatabase struct pointer
 * @return none
 */  
void
freeDSSchemaDatabase(DSSchemaDatabase *ds)
{
  int i,j, numtable,numfield;
  
  if (NULL==ds) return;
  for(i=0;i<ds->numtable;i++)
  {
    for(j=0;j<ds->tables[i].numfield;j++)
    {
      FREEIF(ds->tables[i].fields[j].name);
      
    }
    FREEIF(ds->tables[i].fields);
    
    FREEIF(ds->tables[i].name);
    
  }
  FREEIF(ds->tables);
  FREEIF(ds->name_prefix);
  FREEIF(ds->field_prefix);
  FREEIF(ds);
}

/** 
 * Generate a prefix for database tables:
 * "srbusername+'AT'+srbdomainName+yyyymmddhhmmss"
 * 
 * @param srb_conn srb connection
 * @return name prefix string. must be freed by caller!
 */  
char * 
genNamePrefix()
{
  char *userDomainSeperator="AT", *name_prefix;
  struct tm tm;
  time_t timep;
  
  /* get current GMT time */
  time(&timep);
  gmtime_r(&timep, &tm);
  /* allocate space for returned string. note that 14 is length for yyyymmddhhmmss */
  MALLOC_SAFE(name_prefix,
    strlen(srbUser)+strlen(userDomainSeperator)+strlen(mdasDomainName)+14+sizeof('\0'));
  
  sprintf(name_prefix,"%s%s%s%.4d%.2d%.2d%.2d%.2d%.2d", 
    srbUser,userDomainSeperator,mdasDomainName,
    tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);
  
  return name_prefix;
}

/** 
 * translate datascope field schema to ANSI SQL schema
 * 
 * @param field DSSchemaField field
 * @param field_prefix prefix for field names
 * @param fp output file pointer
 * @return none
 */
void
dumpDSSchemaField2SQL(DSSchemaField *field, char *field_prefix, FILE *fp)
{
  fprintf(fp,"%s%s ",field_prefix,field->name);
  switch (field->type)
  {
    case dbBOOLEAN:
    case dbINTEGER:
    case dbYEARDAY:
      fprintf(fp,"INTEGER");
      break;  
    case dbREAL:
    case dbTIME:
      fprintf(fp,"REAL");
      break;
    case dbSTRING:  
    case dbLINK:  
      fprintf(fp,"VARCHAR (%d)",field->size);
      break;
    default:
      sprintf(temp_buff,"wrong datatype: %d!\n",field->type); 
      DIE(temp_buff);
  }
  
}

/** 
 * dump datascope schema to ANSI SQL schema
 * 
 * @param ds_db DSSchemaDatabase database
 * @param drop_table_needed if "drop table xxx" is needed
 * @param fp output file pointer
 * @return none
 */
void 
dumpDSSchema2SQL(DSSchemaDatabase *ds_db, int drop_table_needed, FILE *fp)
{
  int i,j;
  for (i=0; i<ds_db->numtable; i++)
  {
    if (drop_table_needed)
    {
      fprintf(fp,"DROP TABLE %s%s; \n",
        ds_db->name_prefix,ds_db->tables[i].name);
    }
    fprintf(fp,"CREATE TABLE %s%s\n",
      ds_db->name_prefix,ds_db->tables[i].name);
    fprintf(fp,"(\n");
    for (j=0; j<ds_db->tables[i].numfield; j++)
    {
      fprintf(fp,"  ");
      dumpDSSchemaField2SQL(&(ds_db->tables[i].fields[j]),ds_db->field_prefix,fp);
      if (j!=ds_db->tables[i].numfield-1)
        fprintf(fp,",");
      fprintf(fp,"\n");
    }
    fprintf(fp,");\n");
  }
}

/** 
 * dbgetv returns things like dbREAL:40.0000000, this function strips off the "dbREAL:" part
 * 
 * @param in input string
 * @return same string, but starting after ':'
 */
char *
stripDbgetvResult(char *in)
{
  char *ret;
  ret=strchr(in,':');
  if (NULL==ret)
    return in;
  else
    return ret+1;
}


/** 
 * dump all datascope data to ANSI SQL 
 *
 * @param srb_conn srb connection
 * @param srb_obj_fd path descriptor 
 * @param ds_db DSSchemaDatabase database
 * @_max_row_dump max number of rwos to be dumped, if -1, then no limit
 * @param fp output file pointer
 * @return none
 */
void
dumpDSData2SQL(srbConn *srb_conn, int srb_obj_fd, DSSchemaDatabase *ds_db, int _max_row_dump, FILE *fp)
{
  int i,j,k,m,status, numresult;
  char inStr[HUGE_STRING], smallbuf[HUGE_STRING];
  DSptr dsptr_row;
  char dsptr_str_row[MAX_PROC_ARGS_FOR_DS]={0}, **dbgetv_results;
  
  for (i=0; i<ds_db->numtable; i++)
  {
    if (ds_db->tables[i].numrecord<=0) continue;
    
    memset(smallbuf,0,sizeof(smallbuf));
    sprintf(inStr, "dbgetv|%s",ds_db->tables[i].name);
    for (j=0; j<ds_db->tables[i].numfield; j++)
    {
      sprintf(inStr,"%s|%s",inStr,ds_db->tables[i].fields[j].name);
    }
    if (strlen(inStr)>=sizeof(inStr))
    {
      DIE("inStr overflowed!\n");
    }
    
    for(k=0; k<_max_row_dump&&k<ds_db->tables[i].numrecord; k++)
    {
      dsptr_row=ds_db->tables[i].dsptr;
      dsptr_row.record=k;
      dsPtr2str(&dsptr_row,dsptr_str_row);
      
      status = srbObjProc(srb_conn,srb_obj_fd,inStr,dsptr_str_row,
        sizeof(dsptr_str_row),smallbuf,sizeof(smallbuf));
      if (status<0)
      {
        DIE("srbObjProc failed. \n");
      }
      numresult=strTokenize(smallbuf,'|',&dbgetv_results);
      if (numresult!=ds_db->tables[i].numfield)
      {
        sprintf(temp_buff,"number of result:%d from dbgetv does match number of fields:%d\n",
          numresult,ds_db->tables[i].numfield);
        DIE(temp_buff);
      }
      
      fprintf(fp,"INSERT INTO %s%s VALUES (",
        ds_db->name_prefix,ds_db->tables[i].name);
      for(m=0; m<ds_db->tables[i].numfield; m++)
      {
        if (strstr(dbgetv_results[m], "dbSTRING"))
          fprintf(fp,"'%s'",stripDbgetvResult(dbgetv_results[m]));
        else
          fprintf(fp,"%s",stripDbgetvResult(dbgetv_results[m]));
        
        if (m!=ds_db->tables[i].numfield-1)
          fprintf(fp,", ");
        FREEIF(dbgetv_results[m]);
      }
      FREEIF(dbgetv_results);
      fprintf(fp,");\n");
    }  
  }   
}  

void
usage (char *prog)
{
    fprintf(stderr,"Usage  :%s [-sq] [-h] [-o output_file] "
                   "[-n max_num_row_dump] [-t table_prefix] "
                   "[-f field_prefix] srbObj(datascope_descriptor) \n",
      prog);
}


int
main(int argc, char **argv)
{
  srbConn *srb_conn;
  int c, srb_obj_fd, drop_table_needed=0, max_row_dump=INT_MAX, quit_statement_needed=0;
  char *usr_SRB_obj_DS, *name_prefix, *field_prefix;
  FILE *outfp=stdout;
  DSSchemaDatabase *ds_db=NULL;

  STRDUP_SAFE(name_prefix,"\0");
  STRDUP_SAFE(field_prefix,"\0");
  while ((c=getopt(argc, argv,"shf:n:o:t:q")) != EOF)
  {
    switch (c)
    {
      case 's':
        drop_table_needed=1;
        break;
      case 'h':
        usage (argv[0]);
        exit (0);
      case 'o':
        if ((outfp = fopen(optarg, "w")) == NULL)
        {
          fprintf(stderr, "Error: Cannot open %s. Using standard out instead\n", optarg);
          outfp=stdout;
        }
        break;
      case 'n':
        max_row_dump=atoi(optarg);
        break;  
      case 't':
        STRDUP_SAFE(name_prefix,optarg);
        break;
      case 'f':
        STRDUP_SAFE(field_prefix,optarg);
        break;  
      case 'q':
        quit_statement_needed=1;
        break;    
      default:
        usage (argv[0]);
        exit (1);
    }
  }
  
  /* srbObj name or full path is required */
  if (argc <= optind)
  {
    usage (argv[0]);
    exit (1);
  }
  usr_SRB_obj_DS=argv[optind];
  
  
  /* if (argc <= optind) usr_SRB_obj_DS="/home/siosrb.sio/Datascope Anza/datascopeAnzaWithNoTable"; */
  
  i = initSrbClientEnv();
  if (i < 0)
  {printf("Sds2db: Initialization Error:%i\n",i); exit(-1);}

  conn = srbConnect (srbHost, NULL, srbAuth, NULL, NULL, NULL, NULL);
  if (clStatus(conn) != CLI_CONNECTION_OK)
  {
    fprintf(stderr,"Sds2db: Connection to srbMaster failed.\n");
    fprintf(stderr,"Sds2db: %s\n",clErrorMessage(conn));
    srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
    clFinish(conn); exit(3);
  }
  
  srb_obj_fd=DSopen(conn, usr_SRB_obj_DS);
  ds_db=readDSSchema (conn, srb_obj_fd, name_prefix, field_prefix);
  dumpDSSchema2SQL(ds_db,drop_table_needed,outfp);
  dumpDSData2SQL(conn, srb_obj_fd, ds_db, max_row_dump, outfp);
  
  clFinish (conn);
  FREEIF(name_prefix);
  FREEIF(field_prefix);
  freeDSSchemaDatabase(ds_db);
  printf("End of Program \n");
  return 0;


}
