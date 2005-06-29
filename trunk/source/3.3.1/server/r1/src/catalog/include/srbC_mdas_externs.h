/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/


#ifndef SRBC_MDAS_EXTERNS_H
#define SRBC_MDAS_EXTERNS_H

/* #include "mdas_errno.h" */
#include "srbErrno.h"

/****************************************************************************
   The following definitions are used in the  retractionType  argument 
   of functions that modify the metadatdata in MDAS catalog. These functions 
   are: modify_dataset_info, modify_collection_info, modify_user_info and
   modify_resource_info. The definitions of these functions can be found in
   mdasPrototypes.h 
****************************************************************************/


#define D_DELETE_ONE             1
#define D_DELETE_DOMN            2
#define D_INSERT_DOMN            3
#define D_CHANGE_SIZE            4           
#define D_CHANGE_TYPE            5    
#define D_CHANGE_GROUP           6
#define D_CHANGE_SCHEMA          7
#define D_INSERT_ACCS            8
#define D_DELETE_ACCS            9
#define D_DELETE_ALIAS          10
#define D_INSERT_ALIAS          11
#define D_DELETE_COMMENTS       12
#define D_INSERT_COMMENTS       13
#define D_INSERT_AUDIT          14
#define D_CHANGE_OWNER          15
#define U_CHANGE_PASSWORD       16
#define U_DELETE                17
#define U_DELETE_DOMN           18
#define U_INSERT_DOMN           19
#define U_DELETE_GROUP          20
#define U_INSERT_GROUP          21
#define U_CHANGE_TYPE           22    
#define U_DELETE_ALIAS          23
#define U_INSERT_ALIAS          24
#define U_CHANGE_VERKEY         25           
#define U_CHANGE_ENCKEY         26 
#define D_INSERT_COLL_ACCS      27
#define D_DELETE_COLL_ACCS      28
#define D_CHANGE_DNAME          29
#define D_CHANGE_COLL_NAME      30
#define D_DELETE_COLL           31
#define D_UPDATE_COMMENTS       32
#define D_APPEND_COMMENTS       33
#define SU_CHANGE_PASSWORD      34
#define D_CHANGE_OFFSET         35
#define D_CHANGE_DIRTY          36
#define U_DELETE_AUTH_MAP       37
#define U_INSERT_AUTH_MAP       38
#define D_DELETE_ANNOTATIONS    39
#define D_INSERT_ANNOTATIONS    40
#define D_UPDATE_ANNOTATIONS    41
#define D_CHANGE_MODIFY_TIMESTAMP    42
#define D_DELETE_CLASS    43
#define D_INSERT_CLASS    44
#define D_DELETE_INDEX_FOR_DATA    45
#define D_INSERT_INDEX_FOR_DATA    46
#define D_DELETE_INDEX_FOR_DATATYPE    47
#define D_INSERT_INDEX_FOR_DATATYPE    48
#define D_DELETE_INDEX_FOR_COLLECTION    49
#define D_INSERT_INDEX_FOR_COLLECTION    50
#define D_DELETE_METHOD_FOR_DATA         51
#define D_INSERT_METHOD_FOR_DATA         52
#define D_DELETE_METHOD_FOR_DATATYPE     53
#define D_INSERT_METHOD_FOR_DATATYPE     54
#define D_DELETE_METHOD_FOR_COLLECTION   55
#define D_INSERT_METHOD_FOR_COLLECTION   56 
#define D_DELETE_STRUCT_METADATA         57
#define D_INSERT_EXTERNAL_STRUCT_METADATA        58  
#define D_INSERT_INTERNAL_STRUCT_METADATA        59
#define D_CHANGE_USER_DEFINED_STRING_META_DATA   60                 
#define D_CHANGE_USER_DEFINED_INTEGER_META_DATA  61  
#define D_INSERT_USER_DEFINED_STRING_META_DATA   62
#define D_DELETE_USER_DEFINED_STRING_META_DATA   63
#define D_INSERT_USER_DEFINED_INTEGER_META_DATA  64
#define D_DELETE_USER_DEFINED_INTEGER_META_DATA  65
#define C_INSERT_CONTAINER_FOR_COLLECTION        66
#define C_DELETE_CONTAINER_FOR_COLLECTION        67
#define C_CHANGE_USER_DEFINED_COLL_STRING_META_DATA   68                 
#define C_CHANGE_USER_DEFINED_COLL_INTEGER_META_DATA  69  
#define C_INSERT_USER_DEFINED_COLL_STRING_META_DATA   70
#define C_INSERT_USER_DEFINED_COLL_INTEGER_META_DATA  71
#define C_DELETE_USER_DEFINED_COLL_STRING_META_DATA   72
#define C_DELETE_USER_DEFINED_COLL_INTEGER_META_DATA  73
#define R_DELETE_ACCS                                 74
#define R_INSERT_ACCS                                 75
#define D_CHANGE_LAST_ACCS_TIME                       76
#define D_DELETE_ATTR                                 77
#define D_INSERT_ATTR                                 78
#define D_MODIFY_ATTR                                 79
#define D_DELETE_MULTI_ATTR                           80
#define D_INSERT_MULTI_ATTR                           81
#define D_MODIFY_MULTI_ATTR                           82
#define D_INSERT_LINK                                 83
#define D_UPDATE_COLL_COMMENTS                        84
#define D_INSERT_LOCK                                 85
#define D_UPGRADE_LOCK                                86
#define D_DELETE_LOCK                                 87
#define D_CHANGE_VERSION                              88
#define D_DELETE_ALL_FROM_CONTAINER                   89
#define D_EXTRACT_TEMPLATE_METADATA                   90
#define C_COPY_META_DATA_FROM_COLL_TO_COLL            91
#define D_COPY_META_DATA_FROM_COLL_TO_DATA            92
#define C_COPY_META_DATA_FROM_DATA_TO_COLL            93
#define D_COPY_META_DATA_FROM_DATA_TO_DATA            94
#define D_COPY_ANNOTATIONS_FROM_DATA_TO_DATA          95
#define D_READ                                        96
#define D_WRITE                                       97
#define D_CREATE                                      98
#define D_REPLICATE                                   99
#define D_MOVE                                       100
#define C_CREATE                                     101
#define D_TICKET                                     102
#define C_TICKET                                     103
#define D_LINK                                       104
#define D_BULKINGEST                                 105
#define D_DELETECONT                                 106
#define D_AUDIT_SUCCESS                              107
#define D_AUDIT_FAILURE                              108
#define D_CONTAINER                                  109
#define C_DELETE_ANNOTATIONS                         110
#define C_INSERT_ANNOTATIONS                         111
#define C_UPDATE_ANNOTATIONS                         112
#define C_COPY_ANNOTATIONS_FROM_COLL_TO_COLL         113
#define D_COMPOUND_CREATE                            114
#define D_DELETE_COMPOUND                            115
#define D_DELETE_SUBCOMPOUND                         116
#define D_COMPOUND_MODIFY_CMPD_PATH_NAME             117
#define D_COMPOUND_MODIFY_MODIFY_TIMESTAMP           118
#define D_COMPOUND_MODIFY_LAST_ACCS_TIME             119
#define D_COMPOUND_MODIFY_SIZE                       120
#define D_COMPOUND_MODIFY_OFFSET                     121
#define D_COMPOUND_MODIFY_IS_DIRTY                   122
#define D_COMPOUND_SET_MODIFY_TIMESTAMP              123
#define D_COMPOUND_SET_LAST_ACCS_TIME                124
#define D_COMPOUND_MODIFY_SIZE_AND_DIRTY             125
#define D_CHANGE_DPATH                               126
#define D_INSERT_INCOLL_ACCS                         127
#define D_INSERT_INCOLL_ACCS_RECUR                   128
#define D_DELETE_INCOLL_ACCS                         129
#define D_DELETE_INCOLL_ACCS_RECUR                   130
#define C_INSERT_LINK                                131
#define D_CHANGE_REPLNUM_VERNUM                      132
#define T_INSERT_TAPE_INFO                           133
#define T_UPDATE_TAPE_INFO                           134
#define T_UPDATE_TAPE_INFO_2                         135
#define T_DELETE_TAPE_INFO                           136
#define D_INSERT_DCHECKSUM                           137
#define D_INSERT_DHIDE                               138
#define D_INSERT_AUDITFLAG                           139
#define D_UPDATE_PIN                                 140
#define D_UPDATE_DEXPIRE_DATE                        141
#define D_UPDATE_DEXPIRE_DATE_2                      142
#define D_UPDATE_DCOMPRESSED                         143
#define D_UPDATE_DENCRYPTED                          144
#define D_COPY_META_DATA_FROM_CONTAINER_TO_NEW_CONTAINER    145
#define D_UPDATE_CONTAINER_FOR_COLLECTION            146
#define D_BULK_INSERT_UDEF_META_DATA_FOR_DATA        147
#define C_BULK_INSERT_UDEF_META_DATA_FOR_COLL        148
#define U_BULK_INSERT_UDEF_META_DATA_FOR_USER        149
#define R_BULK_INSERT_UDEF_META_DATA_FOR_RSRC        150
#define Z_INSERT_NEW_LOCAL_ZONE                      151
#define Z_INSERT_NEW_ALIEN_ZONE                      152
#define Z_MODIFY_ZONE_INFO                           153
#define Z_MODIFY_ZONE_FOR_USER                       154
#define Z_CHANGE_ZONE_NAME                           155
#define Z_MODIFY_ZONE_LOCAL_FLAG                     156
#define Z_MODIFY_ZONE_STATUS                         157
#define C_DELETE_MULTI_ATTR                          158
#define C_INSERT_MULTI_ATTR                          159
#define C_MODIFY_MULTI_ATTR                          160
#define C_DELETE_ATTR                                161
#define C_INSERT_ATTR                                162
#define C_MODIFY_ATTR                                163
#define D_BULK_INSERT_UDEF_META_DATA_FOR_MANY_DATA   164
#define R_CHANGE_OWNER                               165
#define C_CHANGE_MODIFY_TIMESTAMP                    166
#define C_CHANGE_COLL_OWNER                          167
#define D_INSERT_GUID                                168
#define D_DELETE_GUID                                169
#define D_DELETE_FROM_EXTMD_TABLE                    170
#define D_INSERT_INTO_EXTMD_TABLE                    171
#define D_MODIFY_IN_EXTMD_TABLE                      172
#define D_MOVE_DATA_TO_NEWCOLL                       173
#define D_MOVE_DATA_TO_TRASH                         174
#define C_MOVE_COLL_TO_NEWCOLL                       175
#define C_MOVE_COLL_TO_TRASH                         176
#define D_CHANGE_INCOLL_OWNER                        177
#define D_CHANGE_INCOLL_OWNER_RECUR                  178
#define R_CHANGE_USER_DEFINED_STRING_META_DATA       179                 
#define R_INSERT_USER_DEFINED_STRING_META_DATA       180
#define R_DELETE_USER_DEFINED_STRING_META_DATA       181
#define U_INSERT_USER_DEFINED_STRING_META_DATA       182
#define U_DELETE_USER_DEFINED_STRING_META_DATA       183
#define U_CHANGE_USER_DEFINED_STRING_META_DATA       184                
#define R_ADJUST_LATENCY_MAX_IN_MILLISEC             185
#define R_ADJUST_LATENCY_MIN_IN_MILLISEC             186
#define R_ADJUST_BANDWIDTH_IN_MBITSPS                187
#define R_ADJUST_MAXIMUM_CONCURRENCY                 188
#define R_ADJUST_NUM_OF_STRIPES                      189
#define R_ADJUST_NUM_OF_HIERARCHIES                  190               
#define R_ADJUST_CAPACITY_IN_GIGABYTES               191
#define R_ADJUST_DESCRIPTION                         192     
#define R_INSERT_RSRC_USAGE_TOTAL_BY_DATE            193
#define R_INSERT_RSRC_USAGE_AND_QUOTA                194
#define R_DELETE_PHY_RSRC_FROM_LOG_RSRC              195
#define D_DELETE_USER_DEFINED_ATTR_VAL_META_DATA     196
#define R_DELETE_USER_DEFINED_ATTR_VAL_META_DATA     197
#define U_DELETE_USER_DEFINED_ATTR_VAL_META_DATA     198
#define C_DELETE_USER_DEFINED_ATTR_VAL_META_DATA     199
#define R_INSERT_PHY_RSRC_INTO_LOG_RSRC              200
#define R_INSERT_FREE_SPACE                          201
#define R_ADJUST_RSRC_COMMENT         		     202
#define U_CHANGE_INFO                                203
#define U_ADD_GROUP_OWNER                            204
#define U_REMOVE_GROUP_OWNER                         205
#define U_UPDATE_EMAIL                               206
#define U_UPDATE_PHONE                               207
#define U_UPDATE_ADDRESS                             208
#define BULK_PHY_MOVE                                209
#define BULK_PHY_MOVE_INTO_CONTAINER                 210
#define BULK_REGISTER_REPLICATE                      211
#define C_CHANGE_ACL_INHERITANCE_BIT                 212
#define C_CHANGE_ACL_INHERITANCE_BIT_RECUR           213

/* curator actions should have an equivalent in non-curator action!!! */
#define CURATOR_ACTION_TYPE_MIN                      3000
#define D_INSERT_ACCS_BY_CURATOR                     3008
#define D_DELETE_ACCS_BY_CURATOR                     3009
#define D_CHANGE_OWNER_BY_CURATOR                    3015
#define D_INSERT_COLL_ACCS_BY_CURATOR                3027
#define D_DELETE_COLL_ACCS_BY_CURATOR                3028
#define D_INSERT_INCOLL_ACCS_BY_CURATOR              3127
#define D_INSERT_INCOLL_ACCS_RECUR_BY_CURATOR        3128
#define D_DELETE_INCOLL_ACCS_BY_CURATOR              3129
#define D_DELETE_INCOLL_ACCS_RECUR_BY_CURATOR        3130
#define C_CHANGE_COLL_OWNER_BY_CURATOR               3167
#define D_CHANGE_INCOLL_OWNER_BY_CURATOR             3177
#define D_CHANGE_INCOLL_OWNER_RECUR_BY_CURATOR       3178
#define C_CHANGE_ACL_INHERITANCE_BIT_BY_CURATOR      3212
#define C_CHANGE_ACL_INHERITANCE_BIT_RECUR_BY_CURATOR    3213
#define CURATOR_ACTION_TYPE_MAX                      4000



#define SYS_ACTION_TYPE_MIN                          1000
#define D_SU_INSERT_ACCS                             1001
#define D_SU_DELETE_ACCS                             1002
#define C_SU_INSERT_COLL_ACCS                        1003
#define C_SU_DELETE_COLL_ACCS                        1004
#define C_SU_CHANGE_COLL_NAME                        1005
#define D_SU_DELETE_TRASH_ONE                        1006
#define C_SU_DELETE_TRASH_COLL                       1007
#define SYS_ACTION_TYPE_MAX                          2000
/****************************************************************************
   The following definitions are used in the deleteValueInfo function. 
****************************************************************************/
#define DELETE_TYPE_LOCATION 1
#define DELETE_TYPE_USER     2
#define DELETE_TYPE_RESOURCE 3


#define MDAS_CAT_COMMIT        0
#define MDAS_CAT_ROLLBACK      1
#define DATAID_CURRVAL       -100
#define ANNON_USER             0

#define MDAS_CATALOG 0
#define LDAP_CATALOG 1
#define FILE_CATALOG 2

#define MDAS_CONFIG_FILE  "MdasConfig"  /* Name of the MDAS config file */
#define MDAS_EXT_TAB_ATTR_FILE "ExtTabInfo" /* Used for defining the dcs entries for exterded tables */


#endif	/* SRBC_MDAS_EXTERNS_H */
