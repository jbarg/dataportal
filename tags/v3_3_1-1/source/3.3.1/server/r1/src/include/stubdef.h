/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* stubDef.h - Definitions of function call stub
 */

#ifndef STUB_DEF_H
#define STUB_DEF_H
#define F_E_CREATE 1000
#define F_E_OPEN 1001
#define F_E_CLOSE 1002
#define F_E_UNLINK 1003
#define F_E_READ 1004
#define F_E_WRITE 1005
#define F_E_SEEK 1006
#define F_E_SYNC 1007
#define F_E_STAT 1008
#define F_E_MKDIR 1009
#define F_E_CHMOD 1010
#define F_E_RMDIR 1011
#define F_E_OPENDIR 1012
#define F_E_READDIR 1013
#define F_E_CLOSEDIR 1014
#define F_E_SETSTORATTRI 1015
#define F_E_MIGRATE 1016
#define F_E_STAGE 1017
#define F_E_PURGE 1018
#define F_E_FSTAT 1019
#define F_E_CHKSUM 1020
#define F_E_GET_FS_FREESPACE 1021
#define F_E_FSTAGE 1022
#define F_E_LOCK_RANGE 1023
#define F_E_CHOWN 1024
 
#define F_E_VAULT_INFO  1050
#define F_GET_SVR_VER  1051
 
/* SRB_MDAS type calls */
 
#define F_SRBO_CREATE 2000
#define F_SRBO_OPEN 2001
#define F_SRBO_CLOSE 2002
#define F_SRBO_UNLINK 2003
#define F_SRBO_READ 2004
#define F_SRBO_WRITE 2005
#define F_SRBO_SEEK 2006
#define F_SRBO_SYNC 2007
#define F_SRBO_STAT 2008
#define F_SRBO_REPLICATE 2009
#define F_SRBO_MOVE 2010
#define F_SRBO_PROXY_OPR 2011
#define F_SRBO_GET_DENTS 2012
#define F_SRBO_GET_DENTS64 2013
#define F_SRBC_SEEK 2014
#define F_SRBO_CHKSUM 2015
#define F_SRBO_LOCK_RANGE 2016
#define F_SRBO_PROC 2017

#define F_SRBO_GET_LOID_INFO    2100
#define F_SRBO_GET_DATADIR_INFO 2101
#define F_SRBO_REGISTER_DATASET 2102
#define F_SRBO_MODIFY_DATASET   2103
#define F_CHK_MDAS_AUTH   	2104
#define F_CREATE_COLLECTION   	2105
#define F_LIST_COLLECTION   	2106
#define F_MODIFY_COLLECTION   	2107
#define F_CHK_MDAS_SYS_AUTH   	2108
#define F_REGISTER_USER_GROUP  	2109
#define F_REGISTER_USER   	2110
#define F_MODIFY_USER   	2111
#define F_SET_AUDIT_TRAIL   	2112
#define F_SRBO_AUDIT   		2113
#define F_REGISTER_REPLICA   	2114
#define F_GET_PRIV_USERS   	2115
#define F_GET_MORE_ROWS   	2116
#define F_ISSUE_TICKET   	2117
#define F_REMOVE_TICKET   	2118
#define F_UNREGISTER_DATASET   	2119
#define F_CONTAINER_CREATE   	2120
#define F_REGISTER_CONTAINER   	2121
#define F_REGISTER_IN_CONTAINER	2122
#define F_GET_CONTAINER_INFO   	2123
#define F_GET_RESC_ON_CHOICE   	2124
#define F_REMOVE_CONTAINER   	2125
#define F_SYNC_CONTAINER   	2126
#define F_REPL_CONTAINER   	2127
#define F_CHK_ENCRYPT1_AUTH     2128
#define F_SRBO_LOCK     	2129
#define F_CONTAINER_OPEN     	2130
#define F_CONTAINER_CLOSE     	2131
#define F_DATA_COPY     	2132
#define F_SRBO_COPY    		2133
#define F_DATA_PUT    		2134
#define F_SRBO_PUT    		2135
#define F_DATA_GET    		2136
#define F_SRBO_GET    		2137
#define F_BULK_REGISTER		2138
#define F_SRBO_SYNC_DATA	2139
#define F_MOD_RESC_INFO		2140
#define F_SRBO_REG_DATASET_INT  2141
#define F_REGISTER_LOCATION                 2142
#define F_INGEST_TOKEN                      2143
#define F_REGISTER_RESOURCE                 2144
#define F_REGISTER_LOGICAL_RESOURCE         2145
#define F_REGISTER_REPLICATE_RESOURCE_INFO  2146
#define F_DELETE_VALUE                      2147
#define F_SETUP_SESSION            2148
#define F_SETUP_SESSION_PUBLIC_KEY 2149
#define F_BULK_LOAD 		2150
#define F_GET_MCAT_NAME                     2151
#define F_MODIFY_ZONE   	2152
#define F_BULK_QUERY_ANSWER   	2153
#define F_BULK_MCAT_INGEST	2154
#define F_GEN_QUERY             2155
#define F_BULK_UNLOAD           2156
#define F_GEN_GET_MORE_ROWS   	2157
#define F_BULK_UNLOAD_C         2158
#define F_BACKUP_DATA           2159
#define F_REMOVE_TICKET_WITH_ZONE   	2160
#define F_MODIFY_USER_NP   	2161
#define F_MODIFY_RESOURCE   	2162
#define F_SRBO_GET_C   		2163
#define F_DATA_GET_C    	2164
#define F_SRBO_PUT_C   		2165
#define F_DATA_PUT_C    	2166
#define F_SRBO_FSTAGE    	2167
#define F_MODIFY_EXT_META_DATA 2168
#define F_GET_USER_BY_DN	2169
#define F_MODIFY_FOR_BULK_MOVE  2170
#define F_BULK_MOVE             2171
#define F_BULK_UNLOAD_FOR_MOVE  2172
#define F_REG_PHY_MOVE          2173
#define F_BULK_COPY             2174
#define F_MOVE_COLL             2175

#define F_GET_HOST_CONFIG    	2200
 
#define F_DB_LOBJ_CREATE 3000
#define F_DB_LOBJ_OPEN 3001
#define F_DB_LOBJ_CLOSE 3002
#define F_DB_LOBJ_READ 3004
#define F_DB_LOBJ_WRITE 3005
#define F_DB_LOBJ_SEEK 3006
#define F_DB_LOBJ_UNLINK 3007


#define F_DB_TABLE_CREATE 4000
#define F_DB_TABLE_OPEN 4001
#define F_DB_TABLE_CLOSE 4002
#define F_DB_TABLE_READ 4004
#define F_DB_TABLE_WRITE 4005
#define F_DB_TABLE_SEEK 4006
#define F_DB_TABLE_UNLINK 4007

/* TapeLib functions */
#define F_TAPELIB_MNT_CART 5000
#define F_TAPELIB_DISMNT_CART 5001
#define F_TAPELIB_GET_CART_PRI 5002

/* compound obj functions */
#define F_CMP_DUMP_FILE_LIST 5100
#define F_CMP_STAGE_COMP_OBJ 5101
#define F_CMP_REG_INT_COMP_OBJ 5102
#define F_CMP_RM_INT_COMP_OBJ 5103
#define F_CMP_RM_COMP_OBJ 5104
#define F_CMP_MOD_INT_COMP_OBJ 5105

#define F_MDRIVER_CREATE 6000
#define F_MDRIVER_OPEN 6001
#define F_MDRIVER_CLOSE 6002
#define F_MDRIVER_READ 6004
#define F_MDRIVER_WRITE 6005
#define F_MDRIVER_SEEK 6006
#define F_MDRIVER_UNLINK 6007
#define F_MDRIVER_SYNC 6008
#define F_MDRIVER_PROC 6009

#define F_DUMMY 999999
/*	Not Yet 
#define F_DB_LOBJ_UNLINK 3003
#define F_DB_TABLE_UNLINK 4003
*/

#endif	/* STUB_DEF_H */
