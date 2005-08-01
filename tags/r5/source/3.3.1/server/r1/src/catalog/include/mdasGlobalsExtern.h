/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

#ifndef MDASGLOBALS_EXTERN_H
#define MDASGLOBALS_EXTERN_H

#include "mdasC_db2_externs.h"
#include "srbC_mdas_externs.h"

/*** mapping from db2 structures to mcat structure  ***/
#if defined(MYSMCAT) || defined(DB_MYS)
#define SQLCHAR unsigned char
#define SQLUSMALLINT unsigned short int
#define SQLSMALLINT short int
#define SQLINTEGER long int
#define SQLUINTEGER unsigned long int
#endif
#if defined(SYBMCAT) || defined(DB_Sybase) || defined(ORAMCAT) || defined(DB_Oracle) 
typedef unsigned char SQLCHAR;
typedef unsigned short int SQLUSMALLINT;
typedef short int SQLSMALLINT;
typedef long     int SQLINTEGER;
typedef unsigned long     int SQLUINTEGER;
typedef int SQLRETURN;
#endif

#define DEF_PARENT_COLL "/home"         /* the defuault collection under which
                                         * the user's home collection will be
                                         * made.
                                         */
#define DEF_CONTAINER_COLL  "/container" /* the defuault collection under which
                                          * the user's container collection
                                          * will be made.
                                          */
#define DEF_TRASH_COLL "/trash"         /* the defuault collection under which
                                         * the user's trash collection will be
                                         * made.
                                         */
#define PUBLIC_USER "public"
#define PUBLIC_USER_DOMAIN "npaci"


extern char MDASDBTYPE[];
extern char MDASSCHEMENAME[];
extern char MDASDBNAME[];
extern char MDASINSERTSFILE[];
extern char MDASCATEXECTYPE[];
extern char MDASCATRSRCIC[];
extern char DB2INSTANCE[];
extern char DB2PASSWORD[];
extern char DB2USER[];
extern char DB2LOGFILE[];
extern char DBHOME[];
extern char db2instancebuffer[MAX_TOKEN];
extern char metadatafkrelbuffer[MAX_TOKEN];

extern int TABLED_EXISTS_DATA_COUNTER;
extern int TABLED_SINGLE_DATA_COUNTER;
extern int TABLED_GETDATASETINFO_COUNTER;
extern char *tabledExistsDataQuery[];
extern char *tabledSingleDataQuery[];
extern char *tabledSingleDataResult[];
extern getDatasetInfoStruct tabledGetDatasetInfo[];
extern int tabledIsOkay;
extern int ALLOW_NON_CHECKING_REGISTERY_OF_DATA;
extern int showQuery;

extern int distinct_flag;
extern int best_response_flag;
extern int outer_join_flag;
extern char groupByList[];
extern int   cur_pds_num;
extern FILE *INSMESSFILE;

extern int audit_flag;
extern int auditLevel;

extern char catser[MAX_TOKEN];
extern char cattype[MAX_TOKEN];
extern char user[MAX_TOKEN];
extern char pass[MAX_TOKEN];
extern char instance[MAX_TOKEN];
extern char exec_type[MAX_TOKEN];
extern char rsrc_id[SMALL_TOKEN];
extern char   server[SMALL_TOKEN];
extern char   uid[SMALL_TOKEN];
extern char   pwd[SMALL_TOKEN];
extern char dbhome[MAX_TOKEN];
extern char   GENERATEDSQL[HUGH_HUGE_STRING];
extern long int             henv;
extern long int             hdbc;
extern mdasC_status         status;  
  
extern char atname[SMALL_TOKEN][MAX_TOKEN];
extern char atval[SMALL_TOKEN][MAX_DATA_SIZE];
extern char cval[SMALL_TOKEN][MAX_TOKEN];
extern char chval[MAX_TOKEN];
extern int  ival[SMALL_TOKEN];
extern int intval, intval2, intval3;
extern char timeval[SMALL_TOKEN];
extern char genGuidFlag[MAX_TOKEN];

extern char newglobalerror[HUGE_STRING];
extern char oldglobalerror[HUGE_STRING];

extern char query_string[HUGE_STRING];
extern char query_string2[HUGE_STRING];
extern int res;
extern int failure;

extern mdasC_servh  serverhndl[MAX_SERVERS];
extern mdasC_sql_continuation_struct queryCont[MAX_QUERY_LIMIT];

/**** needed by database drivers DAI ****/
extern char *Db2Home;
extern char *IllusHome;
extern char *OracleHome;
extern char *InformixHome;
extern char *PostgresHome;
extern char *SybaseHome;
extern char *MysqlHome;

extern char Db2HomeStr[];
extern char IllusHomeStr[];
extern char OracleHomeStr[];
extern char InformixHomeStr[];
extern char PostgresHomeStr[];
extern char SybaseHomeStr[];
extern char MysqlHomeStr[];
/**** needed by database drivers DAI ****/

extern char reg_registrar_name[];
extern int  reg_registrar_id;
extern char reg_obj_name[];
extern int  reg_obj_id;
extern int  reg_obj_repl_enum;
extern char reg_obj_code[];
extern char reg_timeval[];
extern char reg_action_name[];

extern    long int   hstmt;
extern    int i,j,k;
extern    char sqlq[HUGE_STRING];
extern    char spsqlq[HUGE_STRING];
extern    char errmess[HUGE_STRING];
extern    char tabname[MAX_TOKEN];
extern    short int            num_of_cols;
extern    unsigned char       *data[MAX_ROW];
extern    long int             data_size[MAX_ROW];

extern char  dcs_tname[MAX_DCS_STRUCTS][MAX_TOKEN];
extern char  dcs_aname[MAX_DCS_STRUCTS][MAX_TOKEN];
extern char  dcs_cname[MAX_DCS_STRUCTS][MAX_TOKEN];
extern int   accsIdList[SMALL_TOKEN];
extern char  accsNameList[SMALL_TOKEN][SMALL_TOKEN];

extern int   curUserId;
extern int   isUserDomainAdminFlag;
extern int   getDatasetQuery;
extern int   specialQueryFlag;
extern char *specialQueryString;
extern char  specialOrderByClause[];
extern char  localZoneForMcat[];

extern char *protectedDataTableList[];
extern char *protectedCollTableList[];
extern char protectedTableValue[][MAX_TOKEN];
extern char  installSRBUser[][MAX_TOKEN];


extern char *getSrbDataDir();
extern FILE *LOGMESSFILE;

#endif	/* MDASGLOBALS_EXTERN_H */
