/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

#ifndef  MDAS_PROTOTYPES_H
#define  MDAS_PROTOTYPES_H

#include "srbType.h"
#include "mdasC_db2_externs.h"
#include "svrConn.h"

/***************************************************************************
  NAME   : get_dataset_info
  PURPOSE: To ping the Metadata Catalog for information about an 
           object-dataset. Call  get_more_rows at least once
  INPUT  :  cat_type         - catalog type (ex. MDAS_CATALOG)
            data_name        - name of the dataset (or object)
            obj_user         - user name
	    access_name      - access permission (eg. read, write, append, 
	                       execute)
	    domain_name      - security domain name (eg. legion, sdsc)
	                       user and dataset should belong to this domain
	    collection_name  - collection to which the dataset belongs
	    myresult         - holder for pointer to result structure that 
	                       returns the results.
	    rows_required    - maximum number of result rows required
	                       for this call
			       (see also get_more_rows)
  OUTPUT : myresult          - updated with object information, 
  RETURN : 0 for SUCCESS negative for failure
****************************************************************************/

extern int get_dataset_info(int                      cat_type,
			    char                      *data_name, 
			    char                      *obj_user, 
			    char                      *access_name, 
			    char                      *domain_name,
			    char                      *collection_name,
			    mdasC_sql_result_struct  **myresult,
			    int                       rows_required);
 
 

/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : get_more_rows
  PURPOSE: To get additional rows after a get_* call
           Call get_more_rows with rows_required = 0 for ending
	   the query.
  INPUT  :  cat_type         - catalog type (ex. MDAS_CATALOG)
            result_descr     - index into array  maintaining the 
	                       query continuation information
			       use myresult->continuation_index from any
			       get_* call (including most recent get_more_rows
			       call).
	    myresult         - holder for pointer to result structure that 
	                       returns the results.
	    rows_required    - maximum number of result rows required
	                       for this call
			       if zero then no rows are returned and
			       the query structures are closed.
			       (see also get_more_rows)
  OUTPUT : myresult          - updated with object information, 
  RETURN : 0 for SUCCESS negative for failure
****************************************************************************/

extern int get_more_rows(int                        cat_type,
			 int                        result_descr,
			 mdasC_sql_result_struct  **myresult,
			 int                        rows_required);
 
 

/*-------------------------------------------------------------------------*/

/***************************************************************************
  NAME   :  register_dataset_info
  PURPOSE: To register an object-dataset into a Metadata Catalog. Information 
	   about the operation is  also logged in the audit  trail.
  INPUT  :  cat_type         - catalog type (ex. MDAS_CATALOG)
            data_name        - name of the dataset (or object)
            data_user         - user name
	    access_name      - access permission (eg. read, write, append, 
	                       execute)
	    domain_name      - security domain name (eg. legion, sdsc)
	                       user and dataset should belong to this domain 
	    data_type_name    - object type name (eg. legion-object, html, etc)
	    data_path_name    - full path name of the object if a file
	                       table-name/object-id if a DB LOB
	    resource_name    - name of the storage resource.
	    collection_name  - collection to which the object belongs 
	                       (eg.'adl')
	    data_size	     - size of the object
  OUTPUT : none
  RETURN : 0 for SUCCESS negative for failure
****************************************************************************/

extern int register_dataset_info(int                 cat_type,
		   char                      *data_name, 
		   char                      *data_user, 
		   char                      *access_name, 
		   char                      *domain_name,
                   char                      *data_type_name,
		   char                      *data_path_name,   
		   char                      *resource_name,
                   char                      *collection_name,
		   srb_long_t                data_size);

/*-------------------------------------------------------------------------*/

extern int register_container_info(int catType,
                        char *containerName,
                        char *userName,
                        char *domainName,
                        char *resourceName,
                        srb_long_t containerMaxSize);

extern int register_incontainer_dataset (int catType,
                              char *dataName,
                              char *collectionName,
                              char *userName,
                              char *domainName,
                              char *containerName,
                              char *dataTypeName,
                              srb_long_t dataSize,
                              srb_long_t offset);

/***************************************************************************
  NAME   :  modify_dataset_info
  PURPOSE: To modify, add or delete metadata information about an
           object-dataset into a Metadata Catalog. Information 
	   about the operation (except D_INSERT_AUDIT) performed is 
	   also logged in the audit  trail.
  INPUT  :  cat_type         - catalog type (ex. MDAS_CATALOG)
            data_name        - name of the dataset (or object)
	    collection_name  - collection to which the dataset belongs
	    data_path_name    - full path name of the object if a file
	                       table-name/object-id if a DB LOB
	    resource_name    - name of the storage resource
	    data_value_1, data_value_2
	                     - data used for addition/deletion 
	                       or modification
	    retraction_type  - type of operation to be performed.
	    data_user_name    - name of the user
	    user_domain_name  - domain to which the user belongs
	
           (below, we give the semantics for the data_values for the 
	      supported retraction_type (they are comma separted)
	                       	  D_DELETE_ONE      
				      not used,not used
	                     	  D_DELETE_DOMN 
				      domain_desc, not used
				  D_INSERT_DOMN 
				      domain_desc, not used
				  D_CHANGE_SIZE 
				      size, not used
				  D_CHANGE_TYPE 
				      data_typ_name, not used
				  D_CHANGE_GROUP
				      collection_name, not used
				  D_INSERT_ACCS 
				      userName@Domain, access_constraint
				  D_DELETE_ACCS  
				      userName@Domain, access_constraint
				  D_UPDATE_COMMENTS 
				      comment, not used
	                       	  D_BULK_INSERT_UDEF_META_DATA_FOR_DATA
				     metadataString,separator
				  D_BULK_INSERT_UDEF_META_DATA_FOR_MANY_DATA
				     metadataString,separator
	                       	  D_DELETE_ALL_FROM_CONTAINER
				     not used,not used
	                       	  D_CHANGE_OFFSET
				     offset,not_used
	                       	  D_CHANGE_DIRTY
				     data_value_1,not_used
	                       	  D_CHANGE_REPLNUM_VERNUM
				     oldReplNum:newReplNum, oldVerNum:newVerNum
				     use -1 for new values for using internal
				         generated number
	                       	  D_INSERT_LOCK
				     accessName, not_used
				     use read, write  or null for accessName
				     read= readLock not even read is allowed
				     write= writeLock write not allowed
				     null= remove lock.
	                       	  D_INSERT_DCHECKSUM
				     checksum, not_used
	                       	  D_INSERT_DHIDE
				     hidevalue, not_used
				     hidevalue=0 for not hiding
	                       	  D_INSERT_AUDITFLAG
				     auditflag, not_used
	                       	  D_UPDATE_PIN
				     pinNumber, pinExpiryDate
				     pinNumber = 0 for no pin
				     pinExpiryDate = YYYY-MM-DD-HH.MM.SS
	                       	  D_UPDATE_DEXPIRE_DATE
				     expiryDate, not_used
				     expiryDate = YYYY-MM-DD-HH.MM.SS
	                       	  D_UPDATE_DEXPIRE_DATE_2
				     expiryDate, not_used
				     expiryDate = YYYY-MM-DD-HH.MM.SS
	                       	  D_UPDATE_DCOMPRESSED
				     compressionInfo, not_used
				     compressionInfo is a string
	                       	  D_UPDATE_DENCRYPTED
				     encryptionInfo, not_used
				     encryptionInfo is a string
	                       	  D_INSERT_LINK
				     newCollName, newDataName
				     links the data to this srbObjectPath
	                       	  D_CHANGE_DNAME
				     newDataName, unused
	                       	  D_DELETE_ANNOTATIONS
				     annotationDate, not_used
	                       	  D_INSERT_ANNOTATIONS
				     annotation, annotationPosition
	                       	  D_UPDATE_ANNOTATIONS
				     newAnn[@@@newPosition, newAnnotationDate
	                       	  D_CHANGE_OWNER
				     newOwnerName, newOwnerDomain[|Zone]
	                       	  D_CHANGE_SIZE
				     newSize, not_used
				     also changed modifyDate
	                       	  D_CHANGE_DPATH
				     newPathName, not_used 
				     (be careful how you use this!)
	                       	  D_CHANGE_MODIFY_TIMESTAMP
				     newTimeStamp, not_used
				     newTimeStamp=YYYY-MM-DD-HH.MM.SS
	                       	  D_CHANGE_LAST_ACCS_TIME
				     newAccessTime, not_used
				     newAccessTime=YYYY-MM-DD-HH.MM.SS
	                       	  D_DELETE_CLASS
				     classTypeString,classNameString
				     (user-defined)
	                       	  D_INSERT_CLASS
				     classTypeString,classNameString
				     (user-defined)
	                       	  D_DELETE_INDEX_FOR_DATA
				     srbObjName, not_used
				     srbObjName contains the index values
				     used by the data_name
	                       	  D_INSERT_INDEX_FOR_DATA
				     srbObjName, not_used
				     srbObjName contains the index values
				     used by the data_name
				     srbObjName should already exist and user 
				     should have all permission on it.
	                       	  D_DELETE_INDEX_FOR_DATATYPE
				     dataTypeName,not_used
				     dataType for which the data_name cane be
				     used as index
	                       	  D_INSERT_INDEX_FOR_DATATYPE
				     dataTypeName,not_used
				     dataType for which the data_name can be
				     used as index
	                       	  D_DELETE_INDEX_FOR_COLLECTION
				     collectionName,not_used
				     Collection for which the data_name can be
				     used as index
	                       	  D_INSERT_INDEX_FOR_COLLECTION
				     collectionName,not_used
				     Collection for whose data thedata_name
				     can be used as index
	                       	  D_DELETE_METHOD_FOR_DATA
				     srbObjName, not_used
				     srbObjName contains the method applicable
				     to  the data_name
	                       	  D_INSERT_METHOD_FOR_DATA
				     srbObjName, not_used
				     srbObjName contains the method applicable
				     to  the data_name
				     srbObjName should already exist.
	                       	  D_DELETE_METHOD_FOR_DATATYPE
				     dataTypeName,not_used
				     dataType for which the data_name cane be
				     used as a method
	                       	  D_INSERT_METHOD_FOR_DATATYPE
				     dataTypeName, not_used
				     dataType for which the data_name cane be
				     used as a method
	                       	  D_DELETE_METHOD_FOR_COLLECTION
				     collectionName, not_used
				     collection for whose data  the data_name 
				     can be used as a method
	                       	  D_INSERT_METHOD_FOR_COLLECTION
				     collectionName, not_used
				     collection for whose data  the data_name 
				     can be used as a method
	                       	  D_CHANGE_USER_DEFINED_STRING_META_DATA
				     colmnNumber@metadataIndex, newValue
				     for the data_name and the metadata row
				     whose index number = metadataIndex, 
				     the column is given the newValue.
	                       	  D_INSERT_USER_DEFINED_STRING_META_DATA
				     colmnNumber,newValue
				     for the  data_name  a new metadata row
				     is inserted and the columnNum is
				     given the newValue. The return Value
				     is the metadataIndex of the row and can 
				     be  used in performing
				     D_CHANGE_USER_DEFINED_STRING_META_DATA
	                       	  D_DELETE_USER_DEFINED_STRING_META_DATA
				     metadataIndex (or -1) ,not_used
				     deletes the metadata row for data_name
				     whose  metadataIndex is given
				     if -1 is given then all metadata rows
				     are deleted for the data_name
	                       	  D_CHANGE_USER_DEFINED_INTEGER_META_DATA
				     same as for STRING
	                       	  D_INSERT_USER_DEFINED_INTEGER_META_DATA
				     same as for STRING
	                       	  D_DELETE_USER_DEFINED_INTEGER_META_DATA
				     same as for STRING
	                       	  D_COPY_ANNOTATIONS_FROM_DATA_TO_DATA
				     sourceColName, sourceDataName
				     annotation copied from source to data_name
	                       	  D_UPDATE_CONTAINER_FOR_COLLECTION
				     oldContainerName, not_used
				     if the oldContainerNameis used as a 
				     container forsome collection(s) then 
				     the data_name    will replace it.
				     (used internally by SRB server)
	                       	  D_COPY_META_DATA_FROM_CONTAINER_TO_NEW_CONTAINER
				     newContainerCollName, newContainerDataName
				     copies access control (and some other 
				     metadata) from data_name to the new one.
				     (used internally by SRB server)
	                       	  D_COPY_META_DATA_FROM_DATA_TO_DATA
				     sourceCollName, sourceDataName
				     copies user-defined metadata from 
				     source to data_name.
	                       	  D_COPY_META_DATA_FROM_COLL_TO_DATA
				     sourceCollName, not_used
				     copies user-defined metadata from 
				     source to data_name.
	                       	  D_DELETE_STRUCT_METADATA
	                       	  D_INSERT_EXTERNAL_STRUCT_METADATA
	                       	  D_INSERT_INTERNAL_STRUCT_METADATA
	                       	  D_DELETE_ATTR
	                       	  D_INSERT_ATTR
	                       	  D_MODIFY_ATTR
	                       	  D_DELETE_MULTI_ATTR
	                       	  D_INSERT_MULTI_ATTR
	                       	  D_MODIFY_MULTI_ATTR

  OUTPUT : none
  RETURN : 0 for SUCCESS negative for failure
           for D_INSERT_USER_DEFINED_STRING_META_DATA or
	       D_INSERT_USER_DEFINED_INTEGER_META_DATA, 
	       the metadataIndexNum is  returned on success.
****************************************************************************/


extern int modify_dataset_info(int                   cat_type,
			       char                      *data_name, 
			       char                      *collection_name,
			       char                      *data_path_name,   
			       char                      *resource_name,
			       char                      *data_value_1,
			       char                      *data_value_2,
			       int                        retraction_type,
			       char                      *data_user_name,
			       char                      *user_domain_name);


/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   :  modify_extensible_metadata
  PURPOSE: To modify, add or delete metadata information into an extensible
           metadata table in the  Metadata Catalog. 
  INPUT  :  cat_type         - catalog type (ex. MDAS_CATALOG)
            data_name        - name of the dataset (or object)
	    collection_name  - collection to which the dataset belongs
	    data_value_1,data_value_2,data_value_3,data_value_4,data_value_5
	                     - data used for addition/deletion 
	                       or modification
	    retraction_type  - type of operation to be performed.
	    data_user_name    - name of the user
	    user_domain_name  - domain to which the user belongs
	
           (below, we give the semantics for the data_values for the 
	      supported retraction_type (they are comma separted)
	      D_DELETE_FROM_EXTMD_TABLE
	                    data_1 contains table name
                            data_2 contains one or more attribute names in that table
                            separated by | (only one set)
                            data_3 contains the comparison-values corresponding to
                            those attributes.  data_3 can have many rows/statements 
			    separated by new_line. each will be deleted.
                            Attribute names: data_id,user_id,  and collection_id
                            are keywords and will be substituted. Use 0 as place-holder
	      D_INSERT_INTO_EXTMD_TABLE
	                    data_1 contains table name
                            data_2 contains one or more attribute in that table
                            data_3 contains the values corresponding to
                            those attributes.data_3 can have many rows/statements.
			    Separate rows by new_line. New rows are inserted.
                            Attribute names data_id,user_id,time_val, and collection_id
                            are keywords and will be substituted.  Use 0 as place-holder
              D_MODIFY_IN_EXTMD_TABLE
			    data_1 contains table name
                            data_2 contains one or more attribute in that table and
                            data_3 contains the values corresponding to
                            those attributes.these are new values to be inserted
                            data_4 contains one or more attribute in that table and
                            data_5 contains the comparison-values corresponding to
                            data_4 attributes. These are used to chose the rows.
			    data_5 and data_3 can have more than one row separated by 
			    newline. they should match in number of rows.
                            Separate rows by new_line.
                            Attribute names data_id,user_id,time_val, and collection_id
                            are keywords and will be substituted.  Use 0 as place-holder


  OUTPUT : none
  RETURN : 0 for SUCCESS negative for failure

****************************************************************************/


extern int  modify_extensible_metadata(int cat_type, char *data_name,
                        char *collection_name,
                        char *data_value_1,char *data_value_2,
                        char *data_value_3,char *data_value_4, char *data_value_5,
                        int retraction_type, char *user_name,
                        char * domain_name);

/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   :  copy_dataset
  PURPOSE: To record the copy of an object-dataset inthe a Metadata Catalog.
           Information about the operation is  also logged in the audit trail.
  INPUT  :  cat_type             - catalog type (ex. MDAS_CATALOG)
            data_name            - name of the dataset (or object)
	    collection_name      - collection to which the dataset belongs
	    old_resource_name    - name of the storage resource in which the
	                           object resided previously.
	    old_data_path_name    - full path name of the object in
	                           old_resource_name
	    new_resource_name    - name of the storage resource to which the
	                           object has been currently  copiedd 
	    new_data_path_name    - full path name of the object in
	                           new_resource_name
            obj_user_name        - user name
	    user_domain_name  - domain to which the user belongs
  OUTPUT : none
  RETURN : 0 for SUCCESS negative for failure
****************************************************************************/

extern int copy_dataset(int                          cat_type,
			char                      *data_name, 
			char                      *collection_name,
			char                      *old_resource_name,
			char                      *old_data_path_name,   
			char                      *new_resource_name,
			char                      *new_data_path_name, 
			char                      *data_user_name,
			char                      *user_domain_name);

/*-------------------------------------------------------------------------*/

/***************************************************************************
  NAME   :  move_dataset
  PURPOSE: To record the move of an object-dataset inthe a Metadata Catalog.
           Information about the operation is  also logged in the audit trail.
  INPUT  :  cat_type             - catalog type (ex. MDAS_CATALOG)
            data_name            - name of the dataset (or object)
	    collection_name      - collection to which the dataset belongs
	    old_resource_name    - name of the storage resource in which the
	                           object resided previously.
	    old_data_path_name    - full path name of the object in
	                           old_resource_name
	    new_resource_name    - name of the storage resource to which the
	                           object is currently  moved 
	    new_data_path_name    - full path name of the object in
	                           new_resource_name
            data_user_name        - user name
	    user_domain_name  - domain to which the user belongs
  OUTPUT : none
  RETURN : 0 for SUCCESS negative for failure
****************************************************************************/

extern int move_dataset(int                          cat_type,
			char                      *data_name, 
			char                      *collection_name,
			char                      *old_resource_name,
			char                      *old_data_path_name,   
			char                      *new_resource_name,
			char                      *new_data_path_name, 
			char                      *data_user_name,
			char                      *user_domain_name);
/*-------------------------------------------------------------------------*/


extern int auditDatasetAccess(int cat_type, char *user_name,
                  char *data_name, char *collection_name,
		  char *data_path_name,
                  char *resource_name, char *data_access_name, 
                  char *comments, int success, char *user_domain_name);
 

extern int get_mdas_authorization(char *user_name, 
				 char *user_password, 
				 char* domain_name);

extern int get_mdas_sys_authorization(char *user_name, 
				      char *user_password, 
				      char* domain_name);

extern int make_new_collection(int cat_type, char *parent_col_name, 
                    char *new_col_name,
                    char *user_name, char *user_domain);

extern int register_user_group(int cat_type, char *group_name, 
                        char *group_password,
                        char *group_type_name, 
                        char* group_address, char* group_phone, 
                        char* group_email,
                        char *registrar_name, char * registrar_password,
                        char *registrar_domain_name);
 
extern int register_user_info(int cat_type, char *user_name, 
		       char *user_password, char *user_domain_name, 
		       char *user_type_name, 
		       char* user_address, char* user_phone, char* user_email,
		        char *registrar_name, char *registrar_password,
		       char *registrar_domain_name);


 
extern int get_data_dir_info(int cat_type, char qval[][MAX_TOKEN],
                             int selval[], 
                             mdasC_sql_result_struct **myresult,
                             char tname[][MAX_TOKEN],
                             char aname[][MAX_TOKEN],
			     int  rows_required);
 
 
extern int get_collections(int cat_type, 
			   char * col_name,
			   char *flag,
			   mdasC_sql_result_struct **myresult,
			   int  rows_required);
 
extern int resAttrLookup(int catType, char *resourceName,
			 char *domainName, char **resourceType,
			 char **resourceLoc, 
			 char **resourceClass, int **resourceMaxSize,
			 int  rows_required, int *row_count);
 

extern int modify_collection_info(int cat_type, char *obj_user_name,
			   char *group_name, 
			 char *data_value_1, char *data_value_2,
			   char *datavalue_3,
			 int retraction_type, char *user_domain_name);

extern int modify_user_info(int cat_type, char *obj_registrar,
                         char *data_value_1, char *data_value_2,
                         int retraction_type, char *registrar_password,
                            char *registrar_domain_name);

extern int commit_db2_interaction(int  transaction_end_code);

extern int close_db2_interaction(int transaction_end_code);

extern int open_db2_interaction(int cat_type);

extern void rmGlbFromResult (mdasC_sql_result_struct  *myresult);

extern int removeTicket(char *ticketId, char *userName, char *domainName);

extern int
issueTicket(char *objName, char *collection, char *flag, 
	    char *begTime, char* endTime, int AccessCount,
	    char *userSet, char **ticketId, 
	    char *userName, char *domainName);

	    
/***************************************************************************
  NAME   : registerCompoundResourceInfo
  PURPOSE: To register physical resources (PRs) for a Compound Resource
           Can be used as many times as needed for a CR
	   The CR should be of compound resource type.
	   The CR and PRs are regietered using registerResourceInfo
  INPUT  :  compoundResourceName - name of the CR
            physicalResourceName - name of the Physical Resource
				   that is being added as a componet 
				   of the CR
            registrarName        - name of registering SRB administrator
	    registrarDomain      - domain of the SRB administrator
	    registrarPassword    - password of the SRB administrator
  OUTPUT : none
  RETURN : 0 for SUCCESS negative for failure
****************************************************************************/
extern int
registerCompoundResourceInfo(
	char *compoundResourceName,
        char *physicalResourceName,
	char *registrarName,
	char *registrarDomain,
	char *registrarPassword);


/***************************************************************************
  NAME   : isCompoundObjectByName
  PURPOSE: To check whether a given DGB object (or a segment of a given object
	   is in a Compound Resource.
  INPUT  :  conn         - connection Information
	    objName      - name of the DGB object
	    objCollName  - name of the collection of the DGB object
  OUTPUT : none
  RETURN : 0 for SUCCESS negative for failure
****************************************************************************/
extern int 
isCompoundObjectByName(
       svrConn  *conn,
       char *objName, 
       char *objCollName
);


/***************************************************************************
  NAME   : registerInternalCompoundObject
  PURPOSE: To register an internal  replica of a virtual DGB Object that 
           is a compound object stored in a Compound Resource.

	   Note that the compound object (the virtual DGB object) by itself
	   is stored using register_dataset_info
	   intReplNum  >= 0 means use that number
	   intReplNum NEW_INT_REPL_NUM   means generate next replica number
	   intSegNum  NO_INT_SEGMENTATION    means non-segmented object
	   intSegNum  NEW_INT_SEG_NUM  means generate  the next segment number
	   This API should not be visible at the client-level.
  INPUT  :  conn           - connection Information
	    objName        - name of the DGB object
	    objCollName    - name of the collection of the DGB object
	    objReplNum     - replica number of the DGB object
	    objSegNum      - segment number of the DGB object
	    resourceName   - resource where the internal replica
			     of the DGB object is stored.
	    dataPathName   - path/tapeinfo where the internal replica
			     is stored
	    dataSize       - size of the internal replica
	    offset	   - offset of the internal replica
	    intReplNum     - replica number of the internal replica
	    intSegNum      - segment number of the internal replica
	    userName	   - name of the registering user
	    domainName	   - domain of the registering user
  OUTPUT : none
  RETURN : intReplNum  or  negative for failure
****************************************************************************/
#define NEW_INT_REPL_NUM        -1      /* if intReplNum == NEW_INT_REPL_NUM,
                                         * MCAT will generate a new intReplNum.
                                         * For intReplNum >= 0, MCAT will
                                         * just use it as is
                                         */
#define NEW_INT_SEG_NUM         -2      /* if intSegNum == NEW_INT_SEG_NUM,
                                         * MCAT will generate a new intSegNum.
                                         */
#define NO_INT_SEGMENTATION     -1      /* if intSegNum == NO_INT_SEGMENTATION,
                                         * MCAT will store it directly which
                                         * means no segmentation.
                                         */

extern int 
registerInternalCompoundObject (
       svrConn  *conn,
       char *objName, 
       char *objCollName, 
       int   objReplNum,
       int   objSegNum,
       char *resourceName,
       char *dataPathName, 
       srb_long_t dataSize,
       srb_long_t offset,
       int   intReplNum,
       int   intSegNum,
       char *userName, 
       char *domainName);
     

/***************************************************************************
  NAME   : getInternalReplicaInfo
  PURPOSE: To get information about the internal replicas of a compound object.
	   This API should not be visible at the client-level.
  INPUT  :  conn           - connection Information
	    objName        - name of the DGB object
	    objCollName    - name of the collection of the DGB object
	    objReplNum     - replica number of the DGB object
	    objSegNum      - segment number of the DGB object
	    myresult       - holder for pointer to result structure that 
                               returns the results.
            rows_required    - maximum number of result rows required
                               for this call
                               if zero then no rows are returned and
                               the query structures are closed.
                               (see also get_more_rows)
  OUTPUT : myresult          - updated with compound object information, 
  RETURN : 0 for SUCCESS negative for failure
****************************************************************************/
extern int
getInternalReplicaInfo(
       svrConn  *conn,
       char *objName, 
       char *objCollName,
       int   objReplNum,
       int   objSegNum,
       mdasC_sql_result_struct **myresult, 
       int rowsWanted);



/***************************************************************************
  NAME   : getInternalReplicaInfoGivenPathOfOne
  PURPOSE: To get information about the internal replicas of a compound object
	   given a path name of one of the internal replicas.
	   This API should not be visible at the client-level.
  INPUT  :  conn           - connection Information
	    pathName        - pathname of a internal replica
	    myresult       - holder for pointer to result structure that 
                               returns the results.
            rows_required    - maximum number of result rows required
                               for this call
                               if zero then no rows are returned and
                               the query structures are closed.
                               (see also get_more_rows)
  OUTPUT : myresult          - updated with compound object information, 
  RETURN : 0 for SUCCESS negative for failure
****************************************************************************/
extern int
getInternalReplicaInfoGivenPathOfOne(
       svrConn  *conn,
       char *pathName,
       mdasC_sql_result_struct **myresult, 
       int rowsWanted);

/***************************************************************************
  NAME   : removeCompoundObject
  PURPOSE: To delete a GDB object DGB Object that 
           is a compound object stored in a Compound Resource.
           This API checks to see if all the real internal replicas have
	   been deleted. See also removeInternalReplicaOfCompoundObject
	   This API should not be visible at the client-level and should
	   be called by server when athe object being removed is a 
	   compound object.
	   If objReplNum < 0 then all replicas are removed 
	   If objSegNum  < 0 then all segments are removed
  INPUT  :  conn           - connection Information
	    objName        - name of the DGB object
	    objCollName    - name of the collection of the DGB object
	    objReplNum     - replica number of the DGB object
	    objSegNum      - segment number of the DGB object
	    userName	   - name of the deleting user
	    domainName	   - domain of the deleting user

  OUTPUT : none
  RETURN : 0 for SUCCESS negative for failure
****************************************************************************/
extern int
removeCompoundObject(
       svrConn  *conn,
       char *objName, 
       char *objCollName,
       int   objReplNum,
       int   objSegNum,
       char *obj_user_name,
       char *domain_name);

/***************************************************************************
  NAME   : removeInternalCompoundObject
  PURPOSE: To delete an internal replica of a compound object.
	   See also removeCompoundObject
	   This API should not be visible at the client-level.
  INPUT  :  conn           - connection Information
	    objName        - name of the DGB object
	    objCollName    - name of the collection of the DGB object
	    objReplNum     - replica number of the DGB object
	    objSegNum      - segment number of the DGB object
            intReplNum     - replica number of the internal replica
            intSegNum      - segment number of the internal replica
	    userName	   - name of the deleting user
	    domainName	   - domain of the deleting user

  OUTPUT : none
  RETURN : 0 for SUCCESS negative for failure
****************************************************************************/
extern int
removeInternalCompoundObject(
       svrConn  *conn,
       char *objName, 
       char *objCollName,
       int   objReplNum,
       int   objSegNum,
       int   intReplNum,       
       int   intSegNum,       
       char *obj_user_name,
       char *domain_name);

/***************************************************************************
  NAME   : modifyInternalCompoundObject
  PURPOSE: To modify information about an internal replica of a 
           compound object.
	   This API should not be visible at the client-level.
  INPUT  :  conn           - connection Information
	    objName        - name of the DGB object
	    objCollName    - name of the collection of the DGB object
	    objReplNum     - replica number of the DGB object
	    objSegNum      - segment number of the DGB object
            intReplNum     - replica number of the internal replica
            intSegNum      - segment number of the internal replica
	    userName	   - name of the deleting user
	    domainName	   - domain of the deleting user
	    data_value_1   - info to be used for modification
	    data_value_2   - info to be used for modification
	    data_value_3   - info to be used for modification
	    data_value_4   - info to be used for modification
	    retraction_type - modification to be performed.

  OUTPUT : none
  RETURN : 0 for SUCCESS negative for failure
****************************************************************************/
extern int
modifyInternalCompoundObject(
       svrConn  *conn,
       char *objName, 
       char *objCollName,
       int   objReplNum,
       int   objSegNum,
       int   intReplNum,       
       int   intSegNum,       
       char *userName,
       char *domainName,
       char *data_value_1, 
       char *data_value_2,
       char *data_value_3,
       char *data_value_4,
       int retraction_type);



/***************************************************************************
  NAME   :  registerDataSegmentInfo
  PURPOSE: To register the segment of a DGB object. The first
	   segment (segNum=0) should be registered first followed
	   by other segments in any order. Note that register_dataset_info
	   should be used for registering non-segmented objects.
  INPUT  :  conn           - connection Information
	    objName        - name of the DGB object
	    objCollName    - name of the collection of the DGB object
	    objSegNum      - segment number of the DGB object
	    resourceName   - resource where the  DGB object is stored.
	    dataPathName   - path/tapeinfo of the DGB object
	    dataSize       - size of the DGB object
	    offset	   - offset of the DGB object
	    dataTypeName   - data type of the DGB object
	    userName	   - name of the registering user
	    domainName	   - domain of the registering user
  OUTPUT : none
  RETURN : 0 for SUCCESS negative for failure
****************************************************************************/
extern int
registerDataSegmentInfo(
       svrConn  *conn,
       char *objName, 
       char *objCollName,
       int   objSegNum,
       char *resourceName,
       char *dataPathName, 
       srb_long_t dataSize,
       srb_long_t offset,
       char *dataTypeName,
       char *userName, 
       char *domainName);

char *get_from_result_struct(mdasC_sql_result_struct *result,
                             char *tab_name, char *att_name);


/***************************************************************************
  NAME   : modify_zone_info
  PURPOSE: To modify, add or delete zone and zone information in the
           Metadata Catalog. Information about the operation  performed is 
           also logged in the audit  trail (if turned on). This is a
           privileged function and should be called only by a 
           srbAdmin user.
  INPUT  :  cat_type         - catalog type (ex. MDAS_CATALOG)
            regUserName      - registering user name
            regUserDomain    - registering user domain
            zone_name        - name of the zone
            data_value_1     - data used for addition/deletion/modification
            data_value_2     - data used for addition/deletion/modification
            data_value_3     - data used for addition/deletion/modification
            data_value_4     - data used for addition/deletion/modification
            data_value_5     - data used for addition/deletion/modification
                               the semantics of these values will be based 
                               on the action_type.
            action_type      - the type of action to be performed
                             values supported are:

                             Z_INSERT_NEW_LOCAL_ZONE
                                  dv1 = locn_desc
                                  dv2 = port_number
                                  dv3 = username@domain   of remote MCAT admin
                                  dv4 = zone_contact
                                  dv5 = zone_comment
                             Z_INSERT_NEW_ALIEN_ZONE
                                  dv1-5 = same as for INSERT_NEW_LOCAL_ZONE
                             Z_MODIFY_ZONE_INFO
                                  dv1-5 = same as for INSERT_NEW_LOCAL_ZONE
                                      empty string implies no change.
                             Z_MODIFY_ZONE_FOR_USER
                                  dv1 = user_name
                                  dv2 = domain_name
                             Z_CHANGE_ZONE_NAME
                                  dv1 = new name
                             Z_MODIFY_ZONE_LOCAL_FLAG
                                  dv1 = new value (integer)
                             Z_MODIFY_ZONE_STATUS
                                  dv1 = new value (integer)
  OUTPUT : none
  RETURN : 0 for SUCCESS negative for failure
****************************************************************************/
extern int modify_zone_info(int   cat_type, 
                 char *regUserName,
                 char *regUserDomain,
                 char *zone_name,
                 char *data_value_1, 
                 char *data_value_2,
                 char *data_value_3,
                 char *data_value_4,
                 char *data_value_5,
                 int   action_type);


extern int get_bulk_query_answers (int   cat_type, 
			char *queryInfo,  
			mdasC_sql_result_struct **myresult,
			int rowsWanted,
			char *userName, 
			char *domainName);
extern int ingest_bulk_metadata(int   cat_type, 
		     char *ingestInfo,  
		     mdasC_sql_result_struct *myresult,
		     char *userName, 
		     char *domainName);
int
getSRBUserInfo(int catType, mdasC_sql_result_struct  **myresult,
               int numrows);
int 
iGetPasswordFromMcat(char *user_name, char *user_password, char *domain_name);

int
remove_quotes(char *msg, char *ermsg);
int
performAudit(int actId, int userId, int dataId, char *collName,char* comments);
int setConfigInfo();
int get_user_id_in_domain(char *user_name, char *in_user_domain);
int getResourceIdWithAccessToUserId(char *accessPermit,
                                    char *resourceName ,
                                    int user_id);
int check_permit_in_group(char *obj_access_name, char *group_id,
                          int user_id);
int
insertGuidBySRB(int data_id);
int check_permit_in_dataid(char *obj_access_name, int data_id, int user_id);
int
check_permit_in_dataname(char *obj_access_name, char *collName, char *dataName, int user_id);
int
bulkInsertUdefMetadataForManyData(char *userName, char *domainName,
                              char *metadataString,
                              char *separator, char *modDate);
int
getDataIdWithOwnerId(char *collName, char *dataName, int ownerId);
int
bulkInsertUdefMetadataForDataName(char *dataName, char * collName,
                                  char *metadataString,
                                  char *separator, char *modDate);

int
bulkInsertUdefMetadataForData(int obj_data_id,
                              char *metadataString,
                              char *separator, char *modDate);
char*
getTabledSingleData(char *sqlq);
int
addToTabledSingleData(char *sqlq, unsigned char *data);
void get_time(char tval[]);
int check_exists(char *sqlqq);
void splitbychar(char *inname, char *collname, char *dataname, char spchar);
int  check_unique_dname_in_group(char *obj_data_name, char * group_id);
int  check_unique_dname_in_respath(char *obj_data_name, char *obj_path_name,
                         int resource_id);
int getDataId(char *collName, char *dataName);
int is_system_userid(int userid);
int
is_curator_userid(int user_id,  char *collName );
int
getAccessId(char *accsName);
int getGuidBySRB(char *result);
int
makeDataLink(char *srcCollName, char *srcDataName, int trgDataId, int userId);
int make_new_collection_path(int cat_type, char *parent_col_name,
                        char *col_name_new,
                    char *user_name, char *user_domain);
int
removeTabledQueries(char *string);
int
bulkInsertUdefMetadataForColl(char *data_group_id,
                              char *metadataString,
                              char *separator, char *modDate);
int
insertMultiOwner(int registrarId, char *data_group_id, int userId, char *acType);
int
insertMultiAccs(int registrarId, char *data_group_id, int userId, int accessId,
char *acType);
int
deleteMultiAccs(int registrarId, char *data_group_id, int userId, int accessId
, char *acType);
int mcatEscapeWildCards(char *dname);
int
makeCollLink(char *srcParCollName, char *srcChCollName,
             char * trgCollId, int userId);
int  check_unique_name_in_domain(char *obj_type, char *obj_name, char *domain_id);
int
bulkInsertUdefMetadataForUser(int obj_user_id,
                              char *metadataString,
                              char *separator, char *modDate);
int
bulkInsertUdefMetadataForResource(int rsrc_id,
                              char *metadataString,
                              char *separator, char *modDate);
int
registerLogicalResourceInfo(char *resourceName,char *resourceType,
                   char *physicalResourceName,
                   char *registrarName,char *registrarDomainZone,
                   char *registrarPassword, char *indefaultPath);
int
registerReplicateResourceInfo(char *physicalResourceName,char *resourceType,
                   char *oldLogicalResourceName,
                   char *registrarName,char *registrarDomainZone,
                   char *registrarPassword, char *indefaultPath);
int copy_dataset_with_repl_enum(int cat_type, char *obj_data_name,
                 char *collection_name, char *old_resource_name,
                 char *old_obj_path_name, char *new_obj_resource_name,
                 char *new_obj_path_name, char *obj_user_name,
                 char *domain_name, int given_repl_enum);
void
makelikeforvarchar(char *str);
int isUserSysAdmin(char *user_name, char* domain_name);
int isUserDomainAdmin(char *user_name, char* domain_name); 

int
isInstallSRBUser(char *userName);
int make_new_collection_by_id(int cat_type, char *parent_col_name,
                        char *col_name_new_in, int user_id);
int
close_db2_interaction_internal(long int henv,
                  long int hdbc, int transaction_end_code);
int open_db2_interaction_internal(long int *henv,
                  long int *hdbc);
int
getAttrValFromBulkLoadMetadataString(char *beginPtr,
                                     char *separator,
                                     char atval[][MAX_DATA_SIZE],
                                     int atvalStartIndex,
                                     char **endPtr);

void
makelikeforchar(char **str);
int
get_itype_value_from_query(char *sqlq);
int get_itype_value(char name[], char val[]);
int ExecSqlDb2(long int   henv, long int hdbc, char *str);
int
getDataIdRsrcIdFromObj(char *objCollName, char *objName,
                       int objReplNum,int objSegNum,
                       int *objId, int *rsrcId);
int
getChildCompoundRsrcId(int parRsrcId, char *resourceName);
int
getMaxCompoundReplNum(int objId, int objReplNum, int objSegNum);
int
getMaxCompoundSegNum(int objId, int objReplNum, int objSegNum, int intReplNum);
int
checkIfCompoundObjectExists(int objId, int objReplNum, int objSegNum,
                            int intReplNum, int intSegNum);
void error_exit(char *msg);
int
deleteDataInfoFromMCAT(char *objDataName, int objUserId,int objDataId,
                       int objReplNum,int objSegNum);
int deleteFromTable(char atname[][MAX_TOKEN],char atval[][MAX_DATA_SIZE], char *tabname, int atcount);
int changeAttrValueSet(char atname[][MAX_TOKEN],char atval[][MAX_DATA_SIZE], char *tabname, int atcount, int setCount);
int changeAttrValue(char atname[][MAX_TOKEN],char atval[][MAX_DATA_SIZE], char *tabname, int atcount);
int
get_2itype_list_from_query(int cval[], int cval2[], int *colcount,
                      char *sqlq);
int
deleteFromTableWithAllDataAccessCheck(char atname[][MAX_TOKEN],char atval[][MAX_DATA_SIZE], char *tabname, int atcount, int accsId, int userId);
int
insertTokenInfo(char *TokenSwitch, char *newValue, char *parentValue,
                char *registrarName, char *registrarDomain,
                char *registrarPassword);
int
registerLocationInfo(char *locationName, char *netPrefix,
                     char *parentLocation,
                     char *serverUserName, char *serverUserDomain,
                     char *registrarName, char *registrarDomain,
                     char *registrarPassword);
int
insertRsrcAccessInfo ( char *accessList, char *accessPrivilege,
                       char *accessConstraint, char *accessId,
                       char *registrarName,char *registrarDomain,
                       char *registrarPassword );
int
insertDataMimeTypeInfo ( char *dataTypeName, char *mydataTypeMimeString,
                         char *mydataTypeExtenders, char *registrarName,
                         char *registrarDomain, char *registrarAuth);
int
insertDataAccessInfo ( char *accessList, char *accessPrivilege,
                       char *accessConstraint, char *accessId,
                       char *registrarName,char *registrarDomain,
                       char *registrarPassword );
int
insertUserDefMetadataTable(char *ObjectType, char *inObjectInfo,
                             char *rval[], int startInx, int endInx);
int
registerResourceInfoInternal(char *resourceName,char *resourceType,
char *location,
                   char *registrarName,char *registrarDomain,
                   char *registrarPassword, char *indefaultPath,
                     char *resourceClass, int maxObjSizeInResource,
                             char *createTimeVal, char *modifyTimeVal,
                             char *ownerName, char *ownerDomainZone,
                             char *resourceComments);
int
registerLogicalResourceInfoInternal(char *resourceName,char *resourceType,
                   char *physicalResourceName,
                   char *registrarName,char *registrarDomain,
                   char *registrarPassword, char *indefaultPath,
                   char *inResourceLocation,
                   char *inResourceClass, int inMaxObjSizeInResource,
                                    char *createTimeVal, char *modifyTimeVal,
                                    char *ownerName, char *ownerDomainZone,
                                    char *resourceComments);
int
registerReplicateResourceInfoInternal(char *physicalResourceName,
		   char *resourceType,
                   char *oldLogicalResourceName,
                   char *registrarName,char *registrarDomain,
                   char *registrarPassword, char *indefaultPath,
                   char *inResourceLocation,
                   char *inResourceClass, int inMaxObjSizeInResource,
                                    char *createTimeVal, char *modifyTimeVal,
                                    char *ownerName, char *ownerDomainZone,
                                    char *resourceComments);
int
insertResourceInfo (char *phyRsrcName,char *mlsec_latency_max,
		    char *mlsec_latency_min,
                    char *mbps_bandwidth,char *concurrency_max,
		    char *num_of_hierarchies,
                    char *num_of_stripes,char *megab_capacity,char *description,
                    char *username,char *domainname,char *userPassword);
int
modify_resource_info(int cat_type, char *rsrc_name,
                     char *obj_user_name,char *user_domain_name,
                     int retraction_type,
                     char *data_value_1, char *data_value_2,
                     char *data_value_3, char *data_value_4);
int zoneChangeCollName(char *collName);
int
ibmCollCore(int cat_type,
            char *rval[],
            char *userName,
            char *domainName);
int
ibmDataCore(int cat_type,
            char *rval[],
            char *userName,
            char *domainName);
int
ibmContainerCore(int cat_type,
                 char *rval[],
                 char *userName,
                 char *domainName);
int
ibmContColl(int cat_type,
            char *rval[],
            char *userName,
            char *domainName);
int
ibmAnnotation(int cat_type,
              char *objType,
              char *rval[],
              char *userName,
              char *domainName);
int
ibmDataMisc1(int cat_type,
             char *rval[],
             char *userName,
             char *domainName);
int
ibmDataGuid(int cat_type,
              char *rval[],
              char *userName,
              char *domainName);
int get_next_free_server_handle();
void mdasC_db2_connect(mdasC_infoh* user,
        mdasC_infoh* ticket ,
        mdasC_infoh* server ,
        void* comm ,
        mdasC_servh *servh ,
        mdasC_status status);
void mdasC_db2_dcon(mdasC_servh insrvh, mdasC_status status);
void
mdasC_inquire(mdasC_servh* servh ,
               mdasC_infoh* info ,
               mdasC_infoh* result ,
               mdasC_status status, int numrows);
void mdasC_inquire_more(long int hstmt,
                   mdasC_infoh* result ,
                   mdasC_status status,
                   int numrows
                   );
void mdasC_db2_info_inquire(
        mdasC_servh* servh ,
        mdasC_infoh* info ,
        mdasC_infoh* result ,
        mdasC_status status,
        int numrows
        );
void mdasC_db2_info_inquire_more(int cidx,
                                 mdasC_infoh* result ,
                                 mdasC_status status,
                                 int numrows
                                 );

void mdasC_exec(mdasC_servh* servh ,
               mdasC_infoh* data_info ,
               mdasC_infoh* method_info ,
               mdasC_infoh* param_info,
               mdasC_infoh* result ,
               mdasC_status status);
void
set_status_error(mdasC_status status);
int web_mdas_authorization(char *user_name, char *user_password, char* domain_name);
int web_mdas_sys_authorization(char *user_name, char *user_password, char* domain_name);
int
get_ctype_value_from_query(char *cval, char *sqlq);
int
get_itype_value_from_query(char *sqlq);
int get_ctype_value(char cval[], char name[], char val[]);
int
inTabledExistsData(char *sqlq);
int
addToTabledExistsData(char *sqlq);
int set_qval_data_cond(char *in_data_name, char qval[][MAX_TOKEN]);
int getNonContCollectionInfo(int cat_type, char *in_data_name,
                             char *obj_user_name,
                             char *obj_access_name, char *obj_domain_name,
                             char *collection_name,
                             mdasC_sql_result_struct  **myresult,
                             int numrows);
int getDatasetInfoByMetadata(int cat_type, char *in_data_name, 
		char *obj_user_name,
                char *obj_access_name, char *obj_domain_name,
                char *collection_name,
                mdasC_sql_result_struct  **myresult,
                int numrows);
int
getTabledGetDatasetInfo(int cat_type, char *in_data_name,
                        char *obj_user_name, char *obj_access_name,
                        char *obj_domain_name, char *collection_name,
                        mdasC_sql_result_struct  **myresult,  int numrows);
int
checkTrashSuperUserAccess(char *obj_user_name, char *obj_domain_name,
                          char *collection_name );
int
getDataIdContIdByName(char *dataName, char *collName , int *dataId, int *contId);
int
addToTabledGetDatasetInfo(int cat_type, char *in_data_name,
                          char *obj_user_name, char *obj_access_name,
                          char *obj_domain_name,char *collection_name,
                          mdasC_sql_result_struct  *myresult, int numrows);
int
performAuditDataAccess(char *actName, int userId, int dataId,
                       char *collName,char* comments);
void reset_flag_for_distinct(char qval[][MAX_TOKEN],int selval[]);
int
get_more_rows(int cat_type, int cont_index, mdasC_sql_result_struct **myresult,
             int numrows);
int set_info_srb_call(mdasC_sql_query_struct *queryinfo,
              mdasC_sql_result_struct *resultinfo,
              char qval[][MAX_TOKEN],
              int selval[],
              char tname[][MAX_TOKEN],
              char aname[][MAX_TOKEN], int numrows);
int
roundRobinLogRsrc(int rsrcNum, int phyRsrcNum[],int count);
int
get_row_from_query(char cval[][MAX_TOKEN], int *colcount,
                      char *sqlq);
int  get_next_in_hierarchy(char *parent_col_id, char *new_col_id,
                         char *tab_name, char* attr_name);
void uralize(char data[], char datastring[]);
int get_next_counter_value(char name[]);
void get_ctype_attr_value(char cval[], char cname[], char name[], char val[]);
int get_itype_attr_value( char cname[], char name[], char val[]);
int get_data_id_with_collection(char *obj_data_name,
                                char *obj_path_name, char *resource_name,
                                char *obj_user_name,  char *collection_name,
                                char *domain_name,
                                int *obj_data_id,  int *replication_num,
                                int *obj_user_id);

int insertIntoTable(char atname[][MAX_TOKEN],char atval[][MAX_DATA_SIZE], char *tabname, int atcount);
int
get_ctype_list_from_query(char cval[][MAX_TOKEN], int *colcount,
                      char *sqlq);
int
getDataInfo(int dataId, char atval[][MAX_DATA_SIZE]);
int
isContainerByName(char *containerName, char *collectionName);
int
registerNormalDatasetInBulk (int catType,
                                  char *userName,
                                  char *domainName,
                                  char *rsrcName,
                                  mdasC_sql_result_struct *inDataInfo,
                                  int noCheckFlag);
int
isContainer(int dataId);
int
makeOrderByString(char *obList);
int
instantiateUDF(char *condexpr ,char *givenCond,
               char in_table_name[][MAX_TOKEN], int given);
int
substituteInPlace(char *inString, char *key, char *value);
int mdasC_make_sql_using_predefined_dataset_query(
             mdasC_sql_query_struct *queryinfo,
             char *sqlq);
int
getDCSIndex( char *cname);
void print_graph(FILE *fd);
void initialize_flags_in_graph();
void make_sql(char *qs,char *other_conditions,char *select_clause);
void convert_to_sql(int n, char table_names[][MAX_TOKEN], 
		     char *other_conditions ,
                     char *select_clause, char *query_string);
int initMdasEnviron();
void splitAttrOpCond(char *inname, char *aname, char *aop, char *acond);
int make_like_for_mdas_sr(char *dname);
int getFullPathName(char *defPathName,  char *phyDefPathName,
                    char *userName, char *domainName,
                    char *givenPathName, char *dataName,
                    char *collectionName,
                    char fullPathName[HUGE_STRING]);
int getResOnChoice(char *logRes, char *phyRes, char *choice);
int
get_container_info (int catType,
                    char *containerName,
                    char *userName,
                    char *domainName,
                    mdasC_sql_result_struct **myresult,
                    int rowsWanted);
int
removeContainer(int cat_type, char *containerName, char *obj_user_name,
                              char *domain_name);
int
registerIncontainerDatasetInBulk (int catType,
                                  char *userName,
                                  char *domainName,
                                  char *containerName,
                                  mdasC_sql_result_struct *inDataInfo);
int
registerResourceInfo(char *resourceName,char *resourceType,
		   char *resourceLocation,
                   char *registrarName,char *registrarDomainZone,
                   char *registrarPassword, char *indefaultPath,
                     char *resourceClass, int maxObjSizeInResource);
int
deleteValueInfo(int valueType, char *deleteValue, char *userName, char* userDomain);
int
checkIfUserAllowedBulkQuery(char *queryInfo);
int
modify_user_info_nonpriv(int   cat_type,
                 char *regUserName,
                 char *regUserDomain,
                 char *user_name_domain,
                 char *data_value_1,
                 char *data_value_2,
                 char *data_value_3,
                 char *data_value_4,
                 char *data_value_5,
                 int   retraction_type);

int
readSrbObjIntoBufferPtr(char *objName, char **bufPtr);
int
evaluableCond(char *condition);
#ifdef foo
int
evaluateForCondition(char *condition,
                     char IVarName[][VAR_NAME_SIZE],
                     char IVarVal[][VAR_VAL_SIZE],
                     int *InIVarCnt );
int processAllVarsInBuffer (tableStateInfo *stateInfo, char *givenForm,
                            char **retForm);
int
updateVarList(varsList *listName, char *varName, char *varValue, char *varType);
#endif
int
evaluateLogicalExpression( char* e);
int
readSrbObjIntoBufferPtr(char *objName, char **bufPtr);
int
evaluableCond(char *condition);
int
evaluateLogicalExpression( char* e);
int
subStyleVarsWithValuesforHead(int numOfColumns,
                              char *nameentries[],
                              char *Dimen[],
                              char *inForm,
                              char *outForm,
                              int   numOfTables,
                              int   numOfUserDefValues,
                              char *userQuery,
                              char *tableNameList);
int
subStyleVarsWithValuesforBody(int   numOfColumns,
                              int   rowNumber,
                              char *varValues[],
                              char *lastValues[],
                              char *Dimen[],
                              char *nameentries[],
                              char *inForm,
                              char *outForm);
int
compileRegExp( char **compiledRE,  char *inregExp);
int
matchCompRegExp( char *compiledRE, char *inStr,
             char **startPtr, char **endPtr);
int stringParse();
extern void obf2Encode(char *in, char *out, int max_out, int extra);

extern int check_permit_in_dataid_by_access_id(int data_id, int user_id, int access_id);

extern int
inContainerObjectById(int dataId);

extern int
movePhysicalIntoContainerInBulk(int catType,
                 char *userName,
                 char *domainName,
                 char *containerName,
                 mdasC_sql_result_struct *inDataInfo);
extern int
movePhysicalInBulk(int catType,
                 char *userName,
                 char *domainName,
                 char *rsrcName,
                 mdasC_sql_result_struct *inDataInfo);

extern int
modify_for_bulk_move(int catType, int operType,
                 char *userName,  char *domainName,
                 char *operString,
                 mdasC_sql_result_struct *inDataInfo);


extern int get_max_repl_num(int obj_data_id);
extern int
inheritACLWithSticky( char *parentCollId,
                      int dataId,
                      int replnum,
                      char *collId,
		      int userId,
                      char *flagStr);

extern int
registerReplicateInBulk(int catType,
                 char *userName,
                 char *domainName,
                 char *rsrcName,
                 mdasC_sql_result_struct *inDataInfo);

extern int
get_data_repl_info_with_data_id_repl_num(int data_id,
                                         int rnum,
                                         char * verNum,
                                         char atval[][MAX_DATA_SIZE]);
#endif	/* MDAS_PROTOTYPES_H */
