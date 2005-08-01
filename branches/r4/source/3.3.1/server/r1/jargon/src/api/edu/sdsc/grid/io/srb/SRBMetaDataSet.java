//
//	Copyright (c) 2003  San Diego Supercomputer Center (SDSC),
//	University of California, San Diego (UCSD), San Diego, CA, USA.
//
//	Users and possessors of this source code are hereby granted a
//	nonexclusive, royalty-free copyright and design patent license
//	to use this code in individual software.  License is not granted
//	for commercial resale, in whole or in part, without prior written
//	permission from SDSC/UCSD.  This source is provided "AS IS"
//	without express or implied warranty of any kind.
//
//
//  FILE
//	SRBMetaDataSet.java	-  edu.sdsc.grid.io.srb.SRBMetaDataSet
//
//  CLASS HIERARCHY
//	java.lang.Object
//	    |
//	    +-.MetaDataSet
//					|
//					+-.SRBMetaDataSet
//
//  PRINCIPAL AUTHOR
//	Lucas Gilbert, SDSC/UCSD
//
//
package edu.sdsc.grid.io.srb;



import edu.sdsc.grid.io.*;

import java.util.HashMap;



/**
 * SRBMetaDataSet registers all the SRB metadata fields. The fields are
 * divided among various groups to make listing and using the metadata fields
 * more convienent.
 *<P>
 * This class is basically a listing of all the queriable SRB metadata
 * fieldnames. A few hundred static final Strings which equal some more
 * readable name. These strings are then used when forming a MetaDataCondition
 * or MetaDataSelect object.
 *
 * @author	Lucas Gilbert, San Diego Supercomputer Center
 */
/*
	When adding new metadata fields. Four items need to be created for each
	field	added.

	DON'T FORGET THESE. If you forget them, it could take a long time to
	figure out the problem or even notice it. Let me know if think of a
	way to make this easier to troubleshoot.

	1) The static string name which equals a short but readable name.
		- If you forget this, the compiler will fail.

	2) The addition of that name and its related srb ID integer to
			the generalNameToSRBID.
		- If you forget this, it can only be caught at runtime.
			A NullPointerException will occur when the field is used as a select
			in a query.
				"Programming error, was this metadata value: "+fieldName+
				", given an integer value in SRBMetaDataSet.generalNameToSRBID?"
			- see also getSRBID( String fieldName )

	3) The addition to srbNameToGeneralName of the return string sent by
			the srb and the name used in 1).
		- If you forget this, it can only be caught at runtime.
			A NullPointerException will occur when the field is used as a select
			in a query.
				"This srb return value: "+srbTableName+" : "+srbAttributeName+
				", is not supported in this version."
			- see also getGeneralMetaData( String srbName )

	4) The addition of a MetaDataField( name, description, value type ) to the
			group hashmap. An appropriate group for this field must also be chosen.
		- If you forget this, it can only be caught at runtime.
			A NullPointerException, where the fieldName will not be found
			to correspond with any field object, will occur when the field is
			used as a condition or a select in a query.
				"field cannot be null."
			- see also MetaDataSet.getField( String fieldName ) and
			- the MetaDataCondition and MetaDataSelect constructors.
*/
//TODO make the fieldNames actually equal "some more readable name"
//TODO move as many as possible to parent interfaces
public final class SRBMetaDataSet extends MetaDataSet
	implements DublinCoreMetaData, UserMetaData
{
//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------
	/**
	 * The SRB expects a integer, this hashmap is used to convert
	 * the general String name into that integer.
	 */
	private static HashMap generalNameToSRBID = new HashMap(300);



	//Special attributes
	/** return the query values sorted according to another attribute */
	final static String ORDERBY = "order by";

	/** nondistinct*/
	final static String NONDISTINCT = "nondistinct";



	//MetaDataGroup Names
	/**
	 * Group name for metadata relating to Directory/Collection attributes.
	 */
	public final static String GROUP_DIRECTORY = "Directory";

	/**
	 * Group name for metadata relating to Data attributes.
	 */
	public final static String GROUP_DATA = "Data";

	/**
	 * Group name for metadata relating to User attributes.
	 */
	public final static String GROUP_USER = "User";

	/**
	 * Group name for metadata relating to Physical resource attributes.
	 */
	public final static String GROUP_PHYSICAL_RESOURCE = "Physical resource";

	/**
	 * Group name for metadata relating to Logical resource attributes.
	 */
	public final static String GROUP_LOGICAL_RESOURCE = "Logical resource";

	/**
	 * Group name for metadata relating to Server attributes.
	 */
	public final static String GROUP_SERVER = "Server";

	/**
	 * Group name for metadata relating to User group attributes.
	 */
	public final static String GROUP_USER_GROUP = "User group";

	/**
	 * Group name for metadata relating to Authentication attributes.
	 */
	public final static String GROUP_AUTHENTICATION = "Authentication";

	/**
	 * Group name for metadata relating to Authorization attributes.
	 */
	public final static String GROUP_AUTHORIZATION = "Authorization";

	/**
	 * Group name for metadata relating to Audit attributes.
	 */
	public final static String GROUP_AUDIT = "Audit";

	/**
	 * Group name for metadata relating to Ticket attributes.
	 */
	public final static String GROUP_TICKET = "Ticket";

	/**
	 * Group name for metadata relating to Container attributes.
	 */
	public final static String GROUP_CONTAINER = "Container";

	/**
	 * Group name for metadata relating to Annotations attributes.
	 */
	public final static String GROUP_ANNOTATIONS = "Annotations";

	/**
	 * Group name for metadata relating to Compound resource attributes.
	 */
	public final static String GROUP_COMPOUND_RESOURCE = "Compound resource";

	/**
	 * Group name for metadata relating to Dublin Core attributes.
	 */
	public final static String GROUP_DUBLIN_CORE = "Dublin Core";

	/**
	 * Group name for metadata relating to Zone attributes.
	 */
	public final static String GROUP_ZONE = "Zone";

	/**
	 * Group name for metadata relating to Definable metadata attributes.
	 */
	public final static String GROUP_DEFINABLE_METADATA = "Definable Metadata";

	/**
	 * Group name for metadata relating to User-defined metadata attributes
	 * These are the GROUP_DEFINABLE_METADATA attributes using a
	 * non-MetaDataTable access.
	 */
	final static String GROUP_UDMD = "User-defined metadata";

	/**
	 * Group name for metadata relating to Index attributes.
	 */
	public final static String GROUP_INDEX = "Index";

	/**
	 * Group name for metadata relating to Structured Metadata attributes.
	 */
	public final static String GROUP_STRUCTURED_METADATA = "Structured Metadata";

	/**
	 * Group name for metadata relating to Method attributes.
	 */
	public final static String GROUP_METHOD = "Method";

	/**
	 * Group name for metadata relating to GUID attributes.
	 */
	public final static String GROUP_GUID = "GUID";

	/**
	 * Group name for metadata relating to Undefined attributes.
	 */
	public final static String GROUP_UNDEFINED = "Undefined";








	//When updating for new SRB versions, and
	//adding metadata attribute pairs, need to change
	//SRBFileSystem.TOTAL_METADATA_ATTRIBUTES
	//to match, 1 + last DCS-ATTRIBUTE-INDEX
	//see also c client, catalog/include/mdasC_db2_externs.h
	//and SRBCommands class.


	/** internal data id */
	final static String FILE_ID = "file id";

	/** replica copy number */
	public final static String FILE_REPLICATION_ENUM = "file replication enum";

	/** data name */
//	public final static String FILE_NAME = "file name";

	/** name of user group */
	public final static String USER_GROUP_NAME = "user group name";

	/** data type */
	public final static String FILE_TYPE_NAME = "file type name";

	/** user type */
	public final static String USER_TYPE_NAME = "user type name";

	/** user address */
//	public final static String USER_ADDRESS = "user address";

	/** user name */
//	public final static String USER_NAME = "user name";

	/** access restriction on  data */
//	public final static String ACCESS_CONSTRAINT = "access constraint";

	/** user domain name */
	public final static String USER_DOMAIN = "user domain";

	/** physical path name of data object */
	public final static String PATH_NAME = "path name";

	/** name of logical resource */
	public final static String RESOURCE_NAME = "resource name";

	/** net address of resource */
	public final static String RESOURCE_ADDRESS_NETPREFIX = "resource address netprefix";

	/** logical resource type */
	public final static String RESOURCE_TYPE_NAME = "resource type name";

	/** data modification time stamp */
	public final static String MODIFICATION_DATE = "modification date";

	/** collection name */
//	public final static String DIRECTORY_NAME = "directory name";

	/** user phone number */
//	public final static String USER_PHONE = "user phone";

	/** user email */
//	public final static String USER_EMAIL = "user email";

	/** size of data */
//	public final static String SIZE = "size";

	/** audit on user time stamp */
	public final static String USER_AUDIT_TIME_STAMP = "user audit time stamp";

	/** audit on user comments */
	public final static String USER_AUDIT_COMMENTS = "user audit comments";

	/** comments on data */
	public final static String FILE_COMMENTS = "file comments";

	/** audited action  on data */
	public final static String AUDIT_ACTION_DESCRIPTION = "audit action description";

	/** audit time stamp for data */
	public final static String AUDIT_TIMESTAMP = "audit timestamp";

	/** audit comments  for data */
	public final static String AUDIT_COMMENTS = "audit comments";

	/** access on collection in ACCESS_DIRECTORY_NAME */
	public final static String DIRECTORY_ACCESS_CONSTRAINT = "directory access constraint";

	/** collection name for access in DIRECTORY_ACCESS_CONSTRAINT  */
	public final static String ACCESS_DIRECTORY_NAME = "access directory name";

	/** default path in logical resource */
	public final static String RESOURCE_DEFAULT_PATH = "resource default path";

	/** default path in physical resource */
	public final static String PHYSICAL_RESOURCE_DEFAULT_PATH = "physical resource default path";

	/** physical resource name */
	public final static String PHYSICAL_RESOURCE_NAME = "physical resource name";

	/** physical resource type */
	public final static String PHYSICAL_RESOURCE_TYPE_NAME = "physical resource type name";

	/** index of physical rsrc in logical rsrc*/
	public final static String RESOURCE_REPLICATION_ENUM = "resource replication enum";

	/** access list for data */
	public final static String FILE_ACCESS_LIST = "file access list";

	/** access list for resource */
	public final static String RESOURCE_ACCESS_LIST = "resource access list";

	/** data liveness */
	public final static String FILE_IS_DELETED = "file is deleted";

	/** data creator name */
//	public final static String OWNER = "owner";

	/** identifier for ticket given for data*/
	public final static String TICKET_D = "ticket data";

	/** data ticket validity start time */
	public final static String TICKET_BEGIN_TIME_D = "ticket begin time data";

	/** data ticket validity end  time */
	public final static String TICKET_END_TIME_D = "ticket end time data";

	/** valid number of opens allowed on data ticket */
	public final static String TICKET_ACC_COUNT_D = "ticket acc count data";

	/** valid access allowed on data ticket  currently readonly  */
	public final static String TICKET_ACC_LIST_D = "ticket acc list data";

	/** data ticket creator */
	public final static String TICKET_OWNER_D = "ticket owner data";

	/** allowed ticket user or user group */
	public final static String TICKET_USER_D = "ticket user data";

	/** identifier for ticket given for collection and sub collections*/
	public final static String TICKET_C = "ticket collection";

	/** collection ticket validity start time*/
	public final static String TICKET_BEGIN_TIME_C = "ticket begin time collection";

	/** collection ticket validity end time*/
	public final static String TICKET_END_TIME_C = "ticket end time collection";

	/** valid number of opens allowed on data in collection */
	public final static String TICKET_ACC_COUNT_C = "ticket acc count collection";

	/** valid access allowed on data in collection   currently readonly  */
	public final static String TICKET_ACC_LIST_C = "ticket acc list collection";

	/** collection ticket creator */
	public final static String TICKET_OWNER_C = "ticket owner collection";

	/** allowed collection ticket user */
	public final static String TICKET_USER_C = "ticket user collection";

	/** collection ticket user domain */
	public final static String TICKET_USER_DOMAIN_C = "ticket user domain collection";

	/** data ticket user domain */
	public final static String TICKET_USER_DOMAIN_D = "ticket user domain data";

	/** collection ticket creator domain */
	public final static String TICKET_OWNER_DOMAIN_C = "ticket owner domain collection";

	/** data ticket creator domain */
	public final static String TICKET_OWNER_DOMAIN_D = "ticket owner domain data";

	/** name of parent collection in DIRECTORY_NAME  */
	public final static String PARENT_DIRECTORY_NAME = "parent directory name";

	/** location  of resource  name */
	public final static String LOCATION_NAME = "location name";

	/**
	 * data has been changed compared to other copies
	 *  currently used only for containered data
	 */
	public final static String IS_DIRTY = "is dirty";

	/** maximum size of container */
	public final static String CONTAINER_MAX_SIZE = "container max size";

	/** name of container */
	public final static String CONTAINER_NAME = "container name";

	/** classification of resource */
	public final static String RESOURCE_CLASS = "resource class";

	/** maximum size of data object allowed in resource  not enforced by MCAT  */
	public final static String MAX_OBJ_SIZE = "max obj size";

	/** position of data in container */
	public final static String OFFSET = "offset";

	/** name of physical resource of container */
	public final static String CONTAINER_RESOURCE_NAME = "container resource name";

	/** class of physical resource of container */
	public final static String CONTAINER_RESOURCE_CLASS = "container resource class";

	/** current size of container */
	public final static String CONTAINER_SIZE = "container size";

	/** logical resource associated with a  container */
	public final static String CONTAINER_LOG_RESOURCE_NAME = "container log resource name";

	/** domain of data creator */
	public final static String OWNER_DOMAIN = "owner domain";

	/** distinguished name of user  used by authentication systems   */
	public final static String USER_DISTINGUISHED_NAME = "user distinguished name";

	/** user authentication scheme in USER_DISTINGUISHED_NAME  */
	public final static String USER_AUTHENTICATION_SCHEME = "user authentication scheme";

	/** location of SRB server */
	public final static String SERVER_LOCATION = "server location";

	/** net address of SRB server */
	public final static String SERVER_NETPREFIX = "server netprefix";

	/** container copy number */
	public final static String CONTAINER_REPLICATION_ENUM = "container replication enum";

	/** name of annotator */
	public final static String FILE_ANNOTATION_USERNAME = "file annotation username";

	/** domain of annotator */
	public final static String FILE_ANNOTATION_USERDOMAIN = "file annotation userdomain";

	/** annotation on data */
	public final static String FILE_ANNOTATION = "file annotation";

	/** time of annotation */
	public final static String FILE_ANNOTATION_TIMESTAMP = "file annotation timestamp";

	/** data creation time stamp */
//	public final static String CREATION_DATE = "creation date";

	/** time stamp for comments on data */
	public final static String FILE_COMMENTS_TIMESTAMP = "file comments timestamp";

	/** some user-defined location for the annotation */
	public final static String FILE_ANNOTATION_POSITION = "file annotation position";

	/** access privilege on data */
	public final static String FILE_ACCESS_PRIVILEGE = "file access privilege";

	/** physical resource estimated latency  max  */
	public final static String RESOURCE_MAX_LATENCY = "resource max latency";

	/** physical resource estimated latency  min  */
	public final static String RESOURCE_MIN_LATENCY = "resource min latency";

	/** physical resource estimated bandwidth */
	public final static String RESOURCE_BANDWIDTH = "resource bandwidth";

	/** physical resource maximum concurrent requests */
	public final static String RESOURCE_MAX_CONCURRENCY = "resource max concurrency";

	/** number of hierarchies in the physical resource */
	public final static String RESOURCE_NUM_OF_HIERARCHIES = "resource num of hierarchies";

	/** number of striping of data in the physical resource */
	public final static String RESOURCE_NUM_OF_STRIPES = "resource num of stripes";

	/** capacity of  the physical resource */
	public final static String RESOURCE_CAPACITY = "resource capacity";

	/** comments on the resource */
	public final static String RESOURCE_DESCRIPTION = "resource description";

	/** classifcation name for data different from FILE_TYPE_NAME */
	public final static String FILE_CLASS_NAME = "file class name";

	/** classification type */
	public final static String FILE_CLASS_TYPE = "file class type";

	/** type of user-inserted structured metadata for data in STRUCTURED_METADATA_FILE_NAME  or  INTERNAL_STRUCTURED_METADATA  */
	public final static String STRUCTURED_METADATA_TYPE = "structured metadata type";

	/** comments on the structured metadata in STRUCTURED_METADATA_FILE_NAME  or  INTERNAL_STRUCTURED_METADATA  */
	public final static String STRUCTURED_METADATA_COMMENTS = "structured metadata comments";

	/**
	 * data name of user-inserted structured metadata stored as
	 * another data object inside SRB see also  STRUCTURED_METADATA_DIRECTORY_NAME
	 */
	public final static String STRUCTURED_METADATA_FILE_NAME = "structured metadata file name";

	/** user-inserted structured metadata stored as a string inside MCAT */
	public final static String INTERNAL_STRUCTURED_METADATA = "internal structured metadata";

	/** data name of index on data */
	public final static String INDEX_NAME_FOR_FILE = "index name for file";

	/** data name of index on data type */
	public final static String INDEX_NAME_FOR_DATATYPE = "index name for datatype";

	/** data name of index on collection */
	public final static String INDEX_NAME_FOR_DIRECTORY = "index name for directory";

	/** data name of method on data */
	public final static String METHOD_NAME_FOR_FILE = "method name for file";

	/** data name of method on data type */
	public final static String METHOD_NAME_FOR_DATATYPE = "method name for datatype";

	/** data name of method on collection */
	public final static String METHOD_NAME_FOR_DIRECTORY = "method name for directory";

	/** collection name of index on data  in FILE_TYPE_NAME  */
	public final static String IX_DIRECTORY_NAME_FOR_FILE = "ix directory name for file";

	/** collection name of index on data type  in INDEX_NAME_FOR_DATATYPE  */
	public final static String IX_DIRECTORY_NAME_FOR_DATATYPE = "ix directory name for datatype";

	/** collection name of index on collection   in INDEX_NAME_FOR_DIRECTORY  */
	public final static String IX_DIRECTORY_NAME_FOR_DIRECTORY = "ix directory name for directory";

	/** collection name of method on data   in METHOD_NAME_FOR_FILE  */
	public final static String METHOD_DIRECTORY_NAME_FOR_FILE = "method directory name for file";

	/** collection name of method on data type  in METHOD_NAME_FOR_DATATYPE  */
	public final static String METHOD_DIRECTORY_NAME_FOR_DATATYPE = "method directory name for datatype";

	/** collection name of method on collection  in METHOD_NAME_FOR_DIRECTORY  */
	public final static String METHOD_DIRECTORY_NAME_FOR_DIRECTORY = "method directory name for directory";

	/** index type for  INDEX_NAME_FOR_FILE  */
	public final static String IX_DATATYPE_FOR_FILE = "ix datatype for file";

	/** index type for  INDEX_NAME_FOR_DATATYPE  */
	public final static String IX_DATATYPE_FOR_DATATYPE = "ix datatype for datatype";

	/** index type for  INDEX_NAME_FOR_DIRECTORY  */
	public final static String IX_DATATYPE_FOR_DIRECTORY = "ix datatype for directory";

	/** method type for  METHOD_NAME_FOR_FILE  */
	public final static String METHOD_DATATYPE_FOR_FILE = "method datatype for file";

	/** method type for  METHOD_NAME_FOR_DATATYPE  */
	public final static String METHOD_DATATYPE_FOR_DATATYPE = "method datatype for datatype";

	/** method type for  METHOD_NAME_FOR_DIRECTORY  */
	public final static String METHOD_DATATYPE_FOR_DIRECTORY = "method datatype for directory";

	/**
	 * collection name of user-inserted structured metadata stored as
	 * another data object inside SRB see also  STRUCTURED_METADATA_FILE_NAME
	 */
	public final static String STRUCTURED_METADATA_DIRECTORY_NAME = "structured metadata directory name";

	/** path name of index in  INDEX_NAME_FOR_FILE */
	public final static String IX_LOCATION_FOR_FILE = "ix location for file";

	/** path name of index in  INDEX_NAME_FOR_DATATYPE */
	public final static String IX_LOCATION_FOR_DATATYPE = "ix location for datatype";

	/** path name of index in  INDEX_NAME_FOR_DIRECTORY */
	public final static String IX_LOCATION_FOR_DIRECTORY = "ix location for directory";


//definable file
	public final static String DEFINABLE_METADATA_FOR_FILES = "definable metadata for files";

	/** metadata num */
	public final static String METADATA_NUM = "metadata number";

	/** user-defined string metadata  */
	final static String DEFINABLE_METADATA_FOR_FILES0 = "definable metadata file0";
	/** user-defined string metadata  */
	final static String DEFINABLE_METADATA_FOR_FILES1 = "definable metadata file1";
	/** user-defined string metadata  */
	final static String DEFINABLE_METADATA_FOR_FILES2 = "definable metadata file2";
	/** user-defined string metadata  */
	final static String DEFINABLE_METADATA_FOR_FILES3 = "definable metadata file3";
	/** user-defined string metadata  */
	final static String DEFINABLE_METADATA_FOR_FILES4 = "definable metadata file4";
	/** user-defined string metadata  */
	final static String DEFINABLE_METADATA_FOR_FILES5 = "definable metadata file5";
	/** user-defined string metadata  */
	final static String DEFINABLE_METADATA_FOR_FILES6 = "definable metadata file6";
	/** user-defined string metadata  */
	final static String DEFINABLE_METADATA_FOR_FILES7 = "definable metadata file7";
	/** user-defined string metadata  */
	final static String DEFINABLE_METADATA_FOR_FILES8 = "definable metadata file8";
	/** user-defined string metadata  */
	final static String DEFINABLE_METADATA_FOR_FILES9 = "definable metadata file9";
	/** user-defined string metadata  */
	final static String INTEGER_DEFINABLE_METADATA0 = "integer definable metadata file0";
	/** user-defined string metadata  */
	final static String INTEGER_DEFINABLE_METADATA1 = "integer definable metadata file1";



	/** user-defined string metadata  */
	final static String DEFINABLE_METADATA_FOR_FILES0_0 = DEFINABLE_METADATA_FOR_FILES+"0_0";
	/** user-defined string metadata  */
	final static String DEFINABLE_METADATA_FOR_FILES0_1 = DEFINABLE_METADATA_FOR_FILES+"0_1";
	/** user-defined string metadata  */
	final static String DEFINABLE_METADATA_FOR_FILES0_2 = DEFINABLE_METADATA_FOR_FILES+"0_2";
	/** user-defined string metadata  */
	final static String DEFINABLE_METADATA_FOR_FILES0_3 = DEFINABLE_METADATA_FOR_FILES+"0_3";
	/** user-defined string metadata  */
	final static String DEFINABLE_METADATA_FOR_FILES0_4 = DEFINABLE_METADATA_FOR_FILES+"0_4";
	/** user-defined string metadata  */
	final static String DEFINABLE_METADATA_FOR_FILES1_0 = DEFINABLE_METADATA_FOR_FILES+"1_0";
	/** user-defined string metadata  */
	final static String DEFINABLE_METADATA_FOR_FILES1_1 = DEFINABLE_METADATA_FOR_FILES+"1_1";
	/** user-defined string metadata  */
	final static String DEFINABLE_METADATA_FOR_FILES1_2 = DEFINABLE_METADATA_FOR_FILES+"1_2";
	/** user-defined string metadata  */
	final static String DEFINABLE_METADATA_FOR_FILES1_3 = DEFINABLE_METADATA_FOR_FILES+"1_3";
	/** user-defined string metadata  */
	final static String DEFINABLE_METADATA_FOR_FILES1_4 = DEFINABLE_METADATA_FOR_FILES+"1_4";
//end definable


	/** email of data creator */
	public final static String OWNER_EMAIL = "owner email";

	/** audit user */
	public final static String AUDIT_USER = "audit user";

	/** audit user domain */
	public final static String AUDIT_USER_DOMAIN = "audit user domain";

	/** default container  for collection */
	public final static String CONTAINER_FOR_DIRECTORY = "container for directory";

//definable dir
	public final static String DEFINABLE_METADATA_FOR_DIRECTORIES = "definable metadata for directories";

	/** user-defined string metadata  for collection*/
	public final static String METADATA_NUM_DIRECTORY = "metadata number directory";

	/** user-defined string metadata  */
	final static String DEFINABLE_METADATA_FOR_DIRECTORIES0 = "definable metadata directory0";
	/** user-defined string metadata  */
	final static String DEFINABLE_METADATA_FOR_DIRECTORIES1 = "definable metadata directory1";
	/** user-defined string metadata  */
	final static String DEFINABLE_METADATA_FOR_DIRECTORIES2 = "definable metadata directory2";
	/** user-defined string metadata  */
	final static String DEFINABLE_METADATA_FOR_DIRECTORIES3 = "definable metadata directory3";
	/** user-defined string metadata  */
	final static String DEFINABLE_METADATA_FOR_DIRECTORIES4 = "definable metadata directory4";
	/** user-defined string metadata  */
	final static String DEFINABLE_METADATA_FOR_DIRECTORIES5 = "definable metadata directory5";
	/** user-defined string metadata  */
	final static String DEFINABLE_METADATA_FOR_DIRECTORIES6 = "definable metadata directory6";
	/** user-defined string metadata  */
	final static String DEFINABLE_METADATA_FOR_DIRECTORIES7 = "definable metadata directory7";
	/** user-defined string metadata  */
	final static String DEFINABLE_METADATA_FOR_DIRECTORIES8 = "definable metadata directory8";
	/** user-defined string metadata  */
	final static String DEFINABLE_METADATA_FOR_DIRECTORIES9 = "definable metadata directory9";
	/** user-defined string metadata  */
	final static String INTEGER_DEFINABLE_METADATA_FOR_DIRECTORIES0 = "integer definable metadata directory0";
	/** user-defined string metadata  */
	final static String INTEGER_DEFINABLE_METADATA_FOR_DIRECTORIES1 = "integer definable metadata directory1";


	/** user-defined string metadata for collections  */
	final static String DEFINABLE_METADATA_FOR_DIRECTORIES0_0 = DEFINABLE_METADATA_FOR_DIRECTORIES+"0_0";
	/** user-defined string metadata for collections  */
	final static String DEFINABLE_METADATA_FOR_DIRECTORIES0_1 = DEFINABLE_METADATA_FOR_DIRECTORIES+"0_1";
	/** user-defined string metadata for collections  */
	final static String DEFINABLE_METADATA_FOR_DIRECTORIES0_2 = DEFINABLE_METADATA_FOR_DIRECTORIES+"0_2";
	/** user-defined string metadata for collections  */
	final static String DEFINABLE_METADATA_FOR_DIRECTORIES0_3 = DEFINABLE_METADATA_FOR_DIRECTORIES+"0_3";
	/** user-defined string metadata for collections  */
	final static String DEFINABLE_METADATA_FOR_DIRECTORIES0_4 = DEFINABLE_METADATA_FOR_DIRECTORIES+"0_4";
	/** user-defined string metadata for collections  */
	final static String DEFINABLE_METADATA_FOR_DIRECTORIES1_0 = DEFINABLE_METADATA_FOR_DIRECTORIES+"1_0";
	/** user-defined string metadata for collections  */
	final static String DEFINABLE_METADATA_FOR_DIRECTORIES1_1 = DEFINABLE_METADATA_FOR_DIRECTORIES+"1_1";
	/** user-defined string metadata for collections  */
	final static String DEFINABLE_METADATA_FOR_DIRECTORIES1_2 = DEFINABLE_METADATA_FOR_DIRECTORIES+"1_2";
	/** user-defined string metadata for collections  */
	final static String DEFINABLE_METADATA_FOR_DIRECTORIES1_3 = DEFINABLE_METADATA_FOR_DIRECTORIES+"1_3";
	/** user-defined string metadata for collections  */
	final static String DEFINABLE_METADATA_FOR_DIRECTORIES1_4 = DEFINABLE_METADATA_FOR_DIRECTORIES+"1_4";

//end definable dir


	/** MCAT's internal Id for Access Type */
	public final static String FILE_ACCESS_ID = "file access id";

	/** MCAT's internal Id for the User */
	final static String USER_ID = "user id";

	/** data last access time stamp */
	public final static String FILE_LAST_ACCESS_TIMESTAMP = "file last access timestamp";

	/** collection owner name */
	public final static String DIRECTORY_OWNER_NAME = "collection owner name";

	/** collection creation timestamp */
	public final static String DIRECTORY_CREATE_TIMESTAMP = "collection creation timestamp";

	/** collection comments */
	public final static String DIRECTORY_COMMENTS = "collection comments";



	/** directory access id */
	public final static String COLL_ACCS_ID = "directory access id";

	/** directory access user id */
	public final static String COLL_ACCS_USER_ID = "directory access user id";

	/** file access id */
	public final static String DATA_ACCS_ID = "file access id"; //TODO overlap with FILE_ACCESS_ID?

	/** file access user id */
	public final static String DATA_ACCS_USER_ID = "file access user id";



	/** recursive flag for C_TICKET*/
	public final static String TICKET_RECURSIVE = "c ticket recursive";


	/** tape number*/
	public final static String TAPE_NUMBER = "tape number";

	/** tape owner*/
	public final static String TAPE_OWNER = "tape owner";

	/** TAPE_TYPE_VAL*/
	public final static String TAPE_TYPE_VAL = "tape type val";

	/** tape libinx*/
	public final static String TAPE_LIBINX = "tape libinx";

	/** tape fullflag*/
	public final static String TAPE_FULLFLAG = "tape fullflag";

	/** tape current file seqnum*/
	public final static String TAPE_CURRENT_FILE_SEQNUM = "tape current file seqnum";

	/** tape current abs position*/
	public final static String TAPE_CURRENT_ABS_POSITION = "tape current abs position";

	/** tape bytes written*/
	public final static String TAPE_BYTES_WRITTEN = "tape bytes written";

	/** dataset version number */
	public final static String FILE_VER_NUM = "file ver num";

	/** dataset lock */
	public final static String FILE_LOCK_NUM = "file lock num";


	public final static String FILE_LOCK_DESCRIPTION = "file lock description";

	/** dataset lock owner name*/
	public final static String FILE_LOCK_OWNER_NAME = "file lock owner name";

	/** dataset lock owner domain */
	public final static String FILE_LOCK_OWNER_DOMAIN = "file lock owner domain";

	/** collection owner domain */
	public final static String DIRECTORY_OWNER_DOMAIN = "directory owner domain";

	/** collection for ticket */
	public final static String TICKET_DIRECTORY_NAME = "ticket directory name";

	/** ticket collction timestamp */
	public final static String TICKET_DIRECTORY_CREATE_TIMESTAMP = "ticket directory create timestamp";

	/** ticket collction comments */
	public final static String TICKET_DIRECTORY_COMMENTS = "ticket directory comments";

	/** name of annotator */
	public final static String DIRECTORY_ANNOTATION_USERNAME = "directory annotation username";

	/** domain of annotator */
	public final static String DIRECTORY_ANNOTATION_USERDOMAIN = "directory annotation userdomain";

	/** annotation on data */
	public final static String DIRECTORY_ANNOTATION = "directory annotation";

	/** time of annotation */
	public final static String DIRECTORY_ANNOTATION_TIMESTAMP = "directory annotation timestamp";

	/** some user-defined type for the annotation */
	public final static String DIRECTORY_ANNOTATION_TYPE = "directory annotation type";

	/** internal collection id */
	public final static String FILE_GROUP_ID = "file group id";

	/** setting it more than 0 hides the data */
	public final static String FILE_HIDE = "file hide";

	/** check sum for data, 400 character string */
	public final static String FILE_CHECKSUM = "file checksum";

	/** flag to see whether the data needs to be audited */
	public final static String FILE_AUDITFLAG = "file auditflag";

	/** lock expory dtimestamp */
	public final static String FILE_LOCK_EXPIRY = "file lock expiry";

	/** data replica pinned from being moved if set greater than 0 */
	public final static String FILE_PIN_VAL = "file pin val";

	/** owner of the pin */
	public final static String FILE_PIN_OWNER_NAME = "file pin owner name";

	/** domain of the pin owner */
	public final static String FILE_PIN_OWNER_DOMAIN = "file pin owner domain";

	/** expiry timestamp for the pin */
	public final static String FILE_PIN_EXPIRY = "file pin expiry";

	/** expiry timestamp for the data replica itself  */
	public final static String FILE_EXPIRY_DATE = "file expiry date";

	/** if data is compressed tell here how? */
	public final static String FILE_IS_COMPRESSED = "file is compressed";

	/** if data is encrypted tell here how? not the password */
	public final static String FILE_IS_ENCRYPTED = "file is encrypted";

	/** another expiry time_stamp for data as per george req */
	public final static String FILE_EXPIRE_DATE_2 = "file expire date 2";


	/** new audit action description */
	public final static String NEW_AUDIT_ACTION_DESCRIPTION = "new audit action description";


	/** new audit timestamp */
	public final static String NEW_AUDIT_TIMESTAMP = "new audit timestamp";


	/** new audit comments */
	public final static String NEW_AUDIT_COMMENTS = "new audit comments";


	/** definable metadata for resources */
	public final static String DEFINABLE_METADATA_FOR_RESOURCES = "definable metadata for resources";

	public final static String METADATA_NUM_RESOURCE = "metadata number for resources";

	final static String DEFINABLE_METADATA_FOR_RESOURCES0 = "definable metadata resource0";
	final static String DEFINABLE_METADATA_FOR_RESOURCES1 = "definable metadata resource1";
	final static String DEFINABLE_METADATA_FOR_RESOURCES2 = "definable metadata resource2";
	final static String DEFINABLE_METADATA_FOR_RESOURCES3 = "definable metadata resource3";
	final static String DEFINABLE_METADATA_FOR_RESOURCES4 = "definable metadata resource4";
	final static String DEFINABLE_METADATA_FOR_RESOURCES5 = "definable metadata resource5";
	final static String DEFINABLE_METADATA_FOR_RESOURCES6 = "definable metadata resource6";
	final static String DEFINABLE_METADATA_FOR_RESOURCES7 = "definable metadata resource7";
	final static String DEFINABLE_METADATA_FOR_RESOURCES8 = "definable metadata resource8";
	final static String DEFINABLE_METADATA_FOR_RESOURCES9 = "definable metadata resource9";
	final static String DEFINABLE_INTEGER_METADATA_RESOURCE0 = "definable integer metadata resource0";


	/** definable resource metadata 0_0*/
	final static String DEFINABLE_METADATA_FOR_RESOURCES0_0 = DEFINABLE_METADATA_FOR_RESOURCES+"0_0";
	/** definable resource metadata 0_1*/
	final static String DEFINABLE_METADATA_FOR_RESOURCES0_1 = DEFINABLE_METADATA_FOR_RESOURCES+"0_1";
	/** definable resource metadata 0_2*/
	final static String DEFINABLE_METADATA_FOR_RESOURCES0_2 = DEFINABLE_METADATA_FOR_RESOURCES+"0_2";
	/** definable resource metadata 0_3*/
	final static String DEFINABLE_METADATA_FOR_RESOURCES0_3 = DEFINABLE_METADATA_FOR_RESOURCES+"0_3";
	/** definable resource metadata 1_1*/
	final static String DEFINABLE_METADATA_FOR_RESOURCES1_0 = DEFINABLE_METADATA_FOR_RESOURCES+"1_0";
	/** definable resource metadata 1_1*/
	final static String DEFINABLE_METADATA_FOR_RESOURCES1_1 = DEFINABLE_METADATA_FOR_RESOURCES+"1_1";
	/** definable resource metadata 1-2*/
	final static String DEFINABLE_METADATA_FOR_RESOURCES1_2 = DEFINABLE_METADATA_FOR_RESOURCES+"1_2";
	/** definable resource metadata 1_3*/
	final static String DEFINABLE_METADATA_FOR_RESOURCES1_3 = DEFINABLE_METADATA_FOR_RESOURCES+"1_3";

//end resource


	/** compound resource name */
	public final static String COMPOUND_RESOURCE_NAME = "compound resource name";


	/** file seg num */
	public final static String FILE_SEG_NUM = "file seg num";


	/** int replication enum */
	public final static String INT_REPLICATION_ENUM = "int replication enum";


	/** int seg num */
	public final static String INT_SEG_NUM = "int seg num";


	/** int path name */
	public final static String INT_PATH_NAME = "int path name";


	/** int resource name */
	public final static String INT_RESOURCE_NAME = "int resource name";


	/** int resource address netprefix */
	public final static String INT_RESOURCE_ADDRESS_NETPREFIX = "int resource address netprefix";


	/** int resource type name */
	public final static String INT_RESOURCE_TYPE_NAME = "int resource type name";


	/** int is dirty */
	public final static String INT_IS_DIRTY = "int is dirty";


	/** int resource class */
	public final static String INT_RESOURCE_CLASS = "int resource class";


	/** int offset */
	public final static String INT_OFFSET = "int offset";


	/** int size */
	public final static String INT_SIZE = "int size";


	/** comp obj user name */
	public final static String COMP_OBJ_USER_NAME = "comp obj user name";


	/** comp obj user domain */
	public final static String COMP_OBJ_USER_DOMAIN = "comp obj user domain";


	/** int path name for repls */
	public final static String INT_PATH_NAME_FOR_REPL = "int path name for repl";


	/*** fake ****/
	public final static String DIRECTORY_LINK_NUM = "directory link num";

	/*** fake ****/
	public final static String REAL_DIRECTORY_NAME = "real directory name";

	/*** fake ****/
	public final static String FILE_CONTAINER_NAME = "file container name";

	/*** fake ****/
	public final static String DIRECTORY_CONTAINER_NAME = "directory container name";

	/** definable user metadata */
	public final static String DEFINABLE_METADATA_FOR_USERS = "definable metadata for users";

	public final static String METADATA_NUM_USER = "metadata number for users";

	/** definable user metadata string 0*/
	final static String DEFINABLE_METADATA_FOR_USERS0 = "definable metadata user0";
	/** definable user metadata string 1 */
	final static String DEFINABLE_METADATA_FOR_USERS1 = "definable metadata user1";
	/** definable user metadata string 2 */
	final static String DEFINABLE_METADATA_FOR_USERS2 = "definable metadata user2";
	/** definable user metadata string 3 */
	final static String DEFINABLE_METADATA_FOR_USERS3 = "definable metadata user3";
	/** definable user metadata string 4 */
	final static String DEFINABLE_METADATA_FOR_USERS4 = "definable metadata user4";
	/** definable user metadata string 5 */
	final static String DEFINABLE_METADATA_FOR_USERS5 = "definable metadata user5";
	/** definable user metadata string 6 */
	final static String DEFINABLE_METADATA_FOR_USERS6 = "definable metadata user6";
	/** definable user metadata string 7 */
	final static String DEFINABLE_METADATA_FOR_USERS7 = "definable metadata user7";
	/** definable user metadata string 8 */
	final static String DEFINABLE_METADATA_FOR_USERS8 = "definable metadata user8";
	/** definable user metadata string 9 */
	final static String DEFINABLE_METADATA_FOR_USERS9 = "definable metadata user9";

	/** definable user metadata */
	final static String UDIMD_USER0 = "definable user integer metadata 0";


	/** definable user metadata 0_0*/
	final static String DEFINABLE_METADATA_FOR_USERS0_0 = DEFINABLE_METADATA_FOR_USERS+"0_0";
	/** definable user metadata 0_1*/
	final static String DEFINABLE_METADATA_FOR_USERS0_1 = DEFINABLE_METADATA_FOR_USERS+"0_1";
	/** definable user metadata 0_2*/
	final static String DEFINABLE_METADATA_FOR_USERS0_2 = DEFINABLE_METADATA_FOR_USERS+"0_2";
	/** definable user metadata 0_3*/
	final static String DEFINABLE_METADATA_FOR_USERS0_3 = DEFINABLE_METADATA_FOR_USERS+"0_3";
	/** definable user metadata 1_0*/
	final static String DEFINABLE_METADATA_FOR_USERS1_0 = DEFINABLE_METADATA_FOR_USERS+"1_0";
	/** definable user metadata 1_1*/
	final static String DEFINABLE_METADATA_FOR_USERS1_1 = DEFINABLE_METADATA_FOR_USERS+"1_1";
	/** definable user metadata 1_2*/
	final static String DEFINABLE_METADATA_FOR_USERS1_2 = DEFINABLE_METADATA_FOR_USERS+"1_2";
	/** definable user metadata 1_3*/
	final static String DEFINABLE_METADATA_FOR_USERS1_3 = DEFINABLE_METADATA_FOR_USERS+"1_3";
//end definable


	/** definable function metadata 01*/
	final static String USERDEFFUNC01 = "definable function metadata 01";

	/** definable function metadata 02*/
	final static String USERDEFFUNC02 = "definable function metadata 02";

	/** definable function metadata 03*/
	final static String USERDEFFUNC03 = "definable function metadata 03";

	/** definable function metadata 04*/
	final static String USERDEFFUNC04 = "definable function metadata 04";

	/** definable function metadata 05*/
	final static String USERDEFFUNC05 = "definable function metadata 05";


	/** Name of MCAT Zone (char*)*/
	public final static String ZONE_NAME = "zone name";

	/** set to 1 for local zone, 0 otherwise*/
	public final static String ZONE_LOCALITY = "zone locality";

	/** address where (remote) MCAT is */
	public final static String ZONE_NETPREFIX = "zone net prefix";

	/** port num to reach (remote) MCAT*/
	public final static String ZONE_PORT_NUM = "zone port number";

	/** admin's auth scheme of rem MCAT*/
	public final static String ZONE_ADMIN_AUTH_SCHEME_NAME = "zone admin's auth scheme";

	/** DN str  of rem MCAT */
	public final static String ZONE_ADMIN_DISTIN_NAME = "zone admin distinguish name";

	/** 1 for a valid zone */
	public final static String ZONE_STATUS = "zone status";

	/** when the zone was created */
	public final static String ZONE_CREATE_DATE = "zone creation date";

	/** when the zone was modified */
	public final static String ZONE_MODIFY_DATE = "zone modification date";

	/** any comments about the zone */
	public final static String ZONE_COMMENTS = "zone comments";

	/** who is in charge of the zone */
	public final static String ZONE_CONTACT = "zone contact";

	/** zone admin's username */
	public final static String ZONE_ADMIN_NAME = "zone admin's username";

	/** zone admin's user domain name */
	public final static String ZONE_ADMIN_DOMAIN_NAME = "zone admin's user domain name";

	/** resource access privilege */
	public final static String RSRC_ACCESS_PRIVILEGE = "resource access privilege";

	/** resource access constraint name*/
	public final static String RSRC_ACCESS_CONSTRAINT = "resource access constraint name";

	/**change MAX_USER_INTEGER_METADATA*/
	public final static String UDIMD_USER1 = "change MAX_USER_INTEGER_METADATA";

	/**change MAX_RSRC_INTEGER_METADATA*/
	public final static String UDIMD_RSRC1 = "change MAX_RSRC_INTEGER_METADATA";

	/** group user info modify date */
	public final static String USER_GROUP_MODIFY_DATE = "group user info modify date";

	/** group user zone name */
	public final static String USER_GROUP_ZONE_NAME = "group user zone name";

	/** group user domain name */
	public final static String USER_GROUP_DOMAIN_DESC = "group user domain name";

	/** user-def metadata for data mod */
	public final static String DATA_UDEF_MDATA_MODIFY_DATE = "user-def metadata for data mod";

	/** user-def metadata for coll mod */
	public final static String COLL_UDEF_MDATA_MODIFY_DATE = "user-def metadata for coll mod";

	/** user-def metadata for user mod */
	public final static String USER_UDEF_MDATA_MODIFY_DATE = "user-def metadata for user mod";

	/** user-def metadata for rsrc mod */
	public final static String RSRC_UDEF_MDATA_MODIFY_DATE = "user-def metadata for rsrc mod";

	/** mime string for data type */
	public final static String DATA_TYPE_MIME_STRING = "mime string for data type";

	/** extender list for data type */
	public final static String DATA_TYPE_EXTENDERS = "extender list for data type";

	/** comments on log resource */
	public final static String RSRC_COMMENTS = "comments on log resource";

	/** log rsource creation timestamp */
	public final static String RSRC_CREATE_DATE = "log rsource creation timestamp";

	/** log resource modify timestamp */
	public final static String RSRC_MODIFY_DATE = "log resource modify timestamp";

	/** user creation timestamp */
	public final static String USER_CREATE_DATE = "user creation timestamp";

	/** user modify timestamp */
	public final static String USER_MODIFY_DATE = "user modify timestamp";

	/** log resource max obj size */
	public final static String RSRC_MAX_OBJ_SIZE = "log resource max obj size";

	/** owner of the log resource */
	public final static String RSRC_OWNER_NAME = "owner of the log resource";

	/** domain of the log resource */
	public final static String RSRC_OWNER_DOMAIN = "domain of the log resource";

	/** max latency of rsrc in millisec*/
	public final static String RSRC_MLSEC_LATENCY_MAX = "max latency of rsrc in millisec";

	/** min latency of rsrc in millisec*/
	public final static String RSRC_MLSEC_LATENCY_MIN = "min latency of rsrc in millisec";

	/** bandwidth of rsrc in mega bps */
	public final static String RSRC_MBPS_BANDWIDTH = "bandwidth of rsrc in mega bps";

	/** max concurrency allowed in rsrc*/
	public final static String RSRC_CONCURRENCY_MAX = "max concurrency allowed in rsrc";

	/** num of staging levels in rsrc */
	public final static String RSRC_NUM_OF_HIERARCHIES = "num of staging levels in rsrc";

	/** num of striping done in rsrc */
	public final static String RSRC_NUM_OF_STRIPES = "num of striping done in rsrc";

	/** capacity of rsrc in megabytes */
	public final static String RSRC_MEGAB_CAPACITY = "capacity of rsrc in megabytes";

	/** user name used for rsrc access */
	public final static String RSRC_ACCS_USER_NAME = "user name used for rsrc access";

	/** user domn used for rsrc access */
	public final static String RSRC_ACCS_USER_DOMAIN = "user domn used for rsrc access";

	/** user zone used for rsrc access */
	public final static String RSRC_ACCS_USER_ZONE = "user zone used for rsrc access";

	/** zone for resource owner */
	public final static String RSRC_OWNER_ZONE = "zone for resource owner";

	/** zone for data owner */
	public final static String DATA_OWNER_ZONE = "zone for data owner";

	/** zone for data ticket owner */
	public final static String TICKET_OWNER_ZONE_D = "zone for data ticket owner";

	/** zone for data ticket user */
	public final static String TICKET_USER_ZONE_D = "zone for data ticket user";

	/** zone for coll ticket owner */
	public final static String TICKET_OWNER_ZONE_C = "zone for coll ticket owner";

	/** zone for coll ticket user */
	public final static String TICKET_USER_ZONE_C = "zone for coll ticket user";

	/** zone for data annotator */
	public final static String DATA_ANNOTATION_USER_ZONE = "zone for data annotator";

	/** zone for audited suer */
	public final static String AUDIT_USER_ZONE = "zone for audited suer";

	/** zone for collection owner */
	public final static String COLL_OWNER_ZONE = "zone for collection owner";

	/** zone for data lock owner */
	public final static String DATA_LOCK_OWNER_ZONE = "zone for data lock owner";

	/** zone for coll annotator */
	public final static String COLL_ANNOTATION_USER_ZONE = "zone for coll annotator";

	/** zone for data pin owner  */
	public final static String DATA_PIN_OWNER_ZONE = "zone for data pin owner ";

	/** zone for composite obj owner */
	public final static String COMP_OBJ_USER_ZONE = "zone for composite obj owner";

	/** parent of a location */
	public final static String PARENT_SERVER_LOCATION = "parent of a location";

	/** access id number for rsrc access */
	public final static String RSRC_ACCESS_ID = "access id number for rsrc access";

	/** parent of a data type */
	public final static String PARENT_DATA_TYPE = "parent of a data type";

	/** location desc for zone */
	public final static String ZONE_LOCN_DESC = "location desc for zone";

	/** parent of a domain */
	public final static String PARENT_DOMAIN_DESC = "parent of a domain";

	/** parent of a user type */
	public final static String PARENT_USER_TYPE = "parent of a user type";

	/** parent of a rsrc type  */
	public final static String PARENT_RSRC_TYPE = "parent of a rsrc type ";

	/** group user who has access to rsrc*/
	public final static String RSRC_ACCS_GRPUSER_NAME = "group user who has access to rsrc";

	/** rsrc group user domain */
	public final static String RSRC_ACCS_GRPUSER_DOMAIN = "rsrc group user domain";

	/** rsrc group user zone */
	public final static String RSRC_ACCS_GRPUSER_ZONE = "rsrc group user zone";

	/** collection modify_timestamp */
	public final static String COLL_MODIFY_TIMESTAMP = "collection modify_timestamp";

	/** group user who has access to data*/
	public final static String DATA_ACCS_GRPUSER_NAME = "group user who has access to data";

	/** data group user domain */
	public final static String DATA_ACCS_GRPUSER_DOMAIN = "data group user domain";

	/** data group user zone */
	public final static String DATA_ACCS_GRPUSER_ZONE = "data group user zone";

	/** group user who has access to coll*/
	public final static String COLL_ACCS_GRPUSER_NAME = "group user who has access to coll";

	/** coll group user domain */
	public final static String COLL_ACCS_GRPUSER_DOMAIN = "coll group user domain";

	/** coll group user zone */
	public final static String COLL_ACCS_GRPUSER_ZONE = "coll group user zone";

	/** coll name for finding coll access*/
	public final static String COLL_ACCS_COLLECTION_NAME = "coll name for finding coll access";

	/** accs cons for finding coll access*/
	public final static String COLL_ACCS_ACCESS_CONSTRAINT = "accs cons for finding coll access";

	/** data type for container associated with collection */
	public final static String DATA_TYPE_FOR_CONTAINER_FOR_COLLECTION = "data type for container associated with collection";

	/** DATA_UDEF_MDATA_CREATE_DATE */
	public final static String DATA_UDEF_MDATA_CREATE_DATE = "DATA_UDEF_MDATA_CREATE_DATE";

	/** COLL_UDEF_MDATA_CREATE_DATE */
	public final static String COLL_UDEF_MDATA_CREATE_DATE = "COLL_UDEF_MDATA_CREATE_DATE";

	/** RSRC_UDEF_MDATA_CREATE_DATE */
	public final static String RSRC_UDEF_MDATA_CREATE_DATE = "RSRC_UDEF_MDATA_CREATE_DATE";

	/** USER_UDEF_MDATA_CREATE_DATE */
	public final static String USER_UDEF_MDATA_CREATE_DATE = "USER_UDEF_MDATA_CREATE_DATE";

	/**
	 * container id - deprecated, not for external use.
	 * Use CONTAINER_NAME instead.
	 * @deprecate Use CONTAINER_NAME
	 */
	public final static String CONTAINER_ID = "container id";

	/** guid */
	public final static String GUID = "guid";

	/** guid flag */
	public final static String GUID_FLAG = "guid flag";

	/** guid timestamp */
	public final static String GUID_TIME_STAMP = "guid timestamp";


	//SRB3.1
	/** data_id of deleted data object */
	public final static String DELETE_FILE_ID = "delete "+FILE_ID;

	/** repl_enum of deleted data object */
	public final static String DELETE_FILE_REPLICATION_ENUM = "delete "+FILE_REPLICATION_ENUM;

	/** data_name of deleted data object */
	public final static String DELETE_FILE_NAME = "delete "+FILE_NAME;

	/** timestamp of deleted data object */
	public final static String DELETE_MODIFICATION_DATE = "delete "+MODIFICATION_DATE;

	/** collection_name of deleted data object */
	public final static String DELETE_DIRECTORY_NAME = "delete "+DIRECTORY_NAME;



	/*
	 * From srb C client, srbC_mdas_externs.h
	 *
	 * The following definitions are used in the "retractionType" argument
	 * of functions that modify the metadata in MDAS catalog. These functions
	 * are: modify_dataset_info, modify_collection_info, modify_user_info and
	 * modify_resource_info. The definitions of those functions can be found in
	 * mdasPrototypes.h
	 */
//I'd rather these were private but SRBAdmin is using them
	public final static int D_DELETE_ONE            =  1;
	public final static int D_DELETE_DOMN           =  2;
	public final static int D_INSERT_DOMN           =  3;
	public final static int D_CHANGE_SIZE           =  4;
	public final static int D_CHANGE_TYPE           =  5;
	public final static int D_CHANGE_GROUP          =  6;
	public final static int D_CHANGE_SCHEMA         =  7;
	public final static int D_INSERT_ACCS           =  8;
	public final static int D_DELETE_ACCS           =  9;
	public final static int D_DELETE_ALIAS          =  10;
	public final static int D_INSERT_ALIAS          =  11;
	public final static int D_DELETE_COMMENTS       =  12;
	public final static int D_INSERT_COMMENTS       =  13;
	public final static int D_INSERT_AUDIT          =  14;
	public final static int D_CHANGE_OWNER          =  15;
	public final static int U_CHANGE_PASSWORD       =  16;
	public final static int U_DELETE                =  17;
	public final static int U_DELETE_DOMN           =  18;
	public final static int U_INSERT_DOMN           =  19;
	public final static int U_DELETE_GROUP          =  20;
	public final static int U_INSERT_GROUP          =  21;
	public final static int U_CHANGE_TYPE           =  22;
	public final static int U_DELETE_ALIAS          =  23;
	public final static int U_INSERT_ALIAS          =  24;
	public final static int U_CHANGE_VERKEY         =  25;
	public final static int U_CHANGE_ENCKEY         =  26;
	public final static int D_INSERT_COLL_ACCS      =  27;
	public final static int D_DELETE_COLL_ACCS      =  28;
	public final static int D_CHANGE_DNAME          =  29;
	public final static int D_CHANGE_COLL_NAME      =  30;
	public final static int D_DELETE_COLL           =  31;
	public final static int D_UPDATE_COMMENTS       =  32;
	public final static int D_APPEND_COMMENTS       =  33;
	public final static int SU_CHANGE_PASSWORD      =  34;
	public final static int D_CHANGE_OFFSET         =  35;
	public final static int D_CHANGE_DIRTY          =  36;
	public final static int U_DELETE_AUTH_MAP       =  37;
	public final static int U_INSERT_AUTH_MAP       =  38;
	public final static int D_DELETE_ANNOTATIONS             =  39;
	public final static int D_INSERT_ANNOTATIONS             =  40;
	public final static int D_UPDATE_ANNOTATIONS             =  41;
	public final static int D_CHANGE_MODIFY_TIMESTAMP        =  42;
	public final static int D_DELETE_CLASS                   =  43;
	public final static int D_INSERT_CLASS                   =  44;
	public final static int D_DELETE_INDEX_FOR_DATA          =  45;
	public final static int D_INSERT_INDEX_FOR_DATA          =  46;
	public final static int D_DELETE_INDEX_FOR_DATATYPE      =  47;
	public final static int D_INSERT_INDEX_FOR_DATATYPE      =  48;
	public final static int D_DELETE_INDEX_FOR_COLLECTION    =  49;
	public final static int D_INSERT_INDEX_FOR_COLLECTION    =  50;
	public final static int D_DELETE_METHOD_FOR_DATA         =  51;
	public final static int D_INSERT_METHOD_FOR_DATA         =  52;
	public final static int D_DELETE_METHOD_FOR_DATATYPE     =  53;
	public final static int D_INSERT_METHOD_FOR_DATATYPE     =  54;
	public final static int D_DELETE_METHOD_FOR_COLLECTION   =  55;
	public final static int D_INSERT_METHOD_FOR_COLLECTION   =  56;
	public final static int D_DELETE_STRUCT_METADATA         =  57;
	public final static int D_INSERT_EXTERNAL_STRUCT_METADATA        =  58;
	public final static int D_INSERT_INTERNAL_STRUCT_METADATA        =  59;
	public final static int D_CHANGE_USER_DEFINED_STRING_META_DATA   =  60;
	public final static int D_CHANGE_USER_DEFINED_INTEGER_META_DATA  =  61;
	public final static int D_INSERT_USER_DEFINED_STRING_META_DATA   =  62;
	public final static int D_DELETE_USER_DEFINED_STRING_META_DATA   =  63;
	public final static int D_INSERT_USER_DEFINED_INTEGER_META_DATA  =  64;
	public final static int D_DELETE_USER_DEFINED_INTEGER_META_DATA  =  65;
	public final static int C_INSERT_CONTAINER_FOR_COLLECTION        =  66;
	public final static int C_DELETE_CONTAINER_FOR_COLLECTION        =  67;
	public final static int C_CHANGE_USER_DEFINED_COLL_STRING_META_DATA   =  68;
	public final static int C_CHANGE_USER_DEFINED_COLL_INTEGER_META_DATA  =  69;
	public final static int C_INSERT_USER_DEFINED_COLL_STRING_META_DATA   =  70;
	public final static int C_INSERT_USER_DEFINED_COLL_INTEGER_META_DATA  =  71;
	public final static int C_DELETE_USER_DEFINED_COLL_STRING_META_DATA   =  72;
	public final static int C_DELETE_USER_DEFINED_COLL_INTEGER_META_DATA  =  73;
	public final static int R_DELETE_ACCS                                 =  74;
	public final static int R_INSERT_ACCS                                 =  75;
	public final static int D_CHANGE_LAST_ACCS_TIME                       =  76;
	public final static int D_DELETE_ATTR                                 =  77;
	public final static int D_INSERT_ATTR                                 =  78;
	public final static int D_MODIFY_ATTR                                 =  79;
	public final static int D_DELETE_MULTI_ATTR                           =  80;
	public final static int D_INSERT_MULTI_ATTR                           =  81;
	public final static int D_MODIFY_MULTI_ATTR                           =  82;
	public final static int D_INSERT_LINK                                 =  83;
	public final static int D_UPDATE_COLL_COMMENTS                        =  84;
	public final static int D_INSERT_LOCK                                 =  85;
	public final static int D_UPGRADE_LOCK                                =  86;
	public final static int D_DELETE_LOCK                                 =  87;
	public final static int D_CHANGE_VERSION                              =  88;
	public final static int D_DELETE_ALL_FROM_CONTAINER                   =  89;
	public final static int D_EXTRACT_TEMPLATE_METADATA                   =  90;
	public final static int C_COPY_META_DATA_FROM_COLL_TO_COLL            =  91;
	public final static int D_COPY_META_DATA_FROM_COLL_TO_DATA            =  92;
	public final static int C_COPY_META_DATA_FROM_DATA_TO_COLL            =  93;
	public final static int D_COPY_META_DATA_FROM_DATA_TO_DATA            =  94;
	public final static int D_COPY_ANNOTATIONS_FROM_DATA_TO_DATA          =  95;
	public final static int D_READ                                        =  96;
	public final static int D_WRITE                                       =  97;
	public final static int D_CREATE                                      =  98;
	public final static int D_REPLICATE                                   =  99;
	public final static int D_MOVE                                        =  100;
	public final static int C_CREATE                                      =  101;
	public final static int D_TICKET                                      =  102;
	public final static int C_TICKET                                      =  103;
	public final static int D_LINK                                        =  104;
	public final static int D_BULKINGEST                                  =  105;
	public final static int D_DELETECONT                                  =  106;
	public final static int D_AUDIT_SUCCESS                               =  107;
	public final static int D_AUDIT_FAILURE                               =  108;
	public final static int D_CONTAINER                                   =  109;
	public final static int C_DELETE_ANNOTATIONS                          =  110;
	public final static int C_INSERT_ANNOTATIONS                          =  111;
	public final static int C_UPDATE_ANNOTATIONS                          =  112;
	public final static int C_COPY_ANNOTATIONS_FROM_COLL_TO_COLL          =  113;
	public final static int D_COMPOUND_CREATE                             =  114;
	public final static int D_DELETE_COMPOUND                             =  115;
	public final static int D_DELETE_SUBCOMPOUND                          =  116;
	public final static int D_COMPOUND_MODIFY_CMPD_PATH_NAME              =  117;
	public final static int D_COMPOUND_MODIFY_MODIFY_TIMESTAMP            =  118;
	public final static int D_COMPOUND_MODIFY_LAST_ACCS_TIME              =  119;
	public final static int D_COMPOUND_MODIFY_SIZE                        =  120;
	public final static int D_COMPOUND_MODIFY_OFFSET                      =  121;
	public final static int D_COMPOUND_MODIFY_IS_DIRTY                    =  122;
	public final static int D_COMPOUND_SET_MODIFY_TIMESTAMP               =  123;
	public final static int D_COMPOUND_SET_LAST_ACCS_TIME                 =  124;
	public final static int D_COMPOUND_MODIFY_SIZE_AND_DIRTY              =  125;
	public final static int D_CHANGE_DPATH                                =  126;
	public final static int D_INSERT_INCOLL_ACCS                          =  127;
	public final static int D_INSERT_INCOLL_ACCS_RECUR                    =  128;
	public final static int D_DELETE_INCOLL_ACCS                          =  129;
	public final static int D_DELETE_INCOLL_ACCS_RECUR                    =  130;
	public final static int C_INSERT_LINK                                 =  131;
	public final static int D_CHANGE_REPLNUM_VERNUM                       =  132;
	public final static int T_INSERT_TAPE_INFO                            =  133;
	public final static int T_UPDATE_TAPE_INFO                            =  134;
	public final static int T_UPDATE_TAPE_INFO_2                          =  135;
	public final static int T_DELETE_TAPE_INFO                            =  136;
	public final static int D_INSERT_DCHECKSUM                            =  137;
	public final static int D_INSERT_DHIDE                                =  138;
	public final static int D_INSERT_AUDITFLAG                            =  139;
	public final static int D_UPDATE_PIN                                  =  140;
	public final static int D_UPDATE_DEXPIRE_DATE                         =  141;
	public final static int D_UPDATE_DEXPIRE_DATE_2                       =  142;
	public final static int D_UPDATE_DCOMPRESSED                          =  143;
	public final static int D_UPDATE_DENCRYPTED                           =  144;
	public final static int D_COPY_META_DATA_FROM_CONTAINER_TO_NEW_CONTAINER  =	145;
	public final static int D_UPDATE_CONTAINER_FOR_COLLECTION             =  146;
	public final static int D_BULK_INSERT_UDEF_META_DATA_FOR_DATA         =  147;
	public final static int C_BULK_INSERT_UDEF_META_DATA_FOR_COLL         =  148;
	public final static int U_BULK_INSERT_UDEF_META_DATA_FOR_USER         =  149;
	public final static int R_BULK_INSERT_UDEF_META_DATA_FOR_RSRC         =  150;
	public final static int Z_INSERT_NEW_LOCAL_ZONE        							  =  151;
	public final static int Z_INSERT_NEW_ALIEN_ZONE      							 	  =  152;
	public final static int Z_MODIFY_ZONE_INFO      							 			  =  153;
	public final static int Z_MODIFY_ZONE_FOR_USER      				  			  =  154;
	public final static int Z_CHANGE_ZONE_NAME      							 			  =  155;
	public final static int Z_MODIFY_ZONE_LOCAL_FLAG      							  =  156;
	public final static int Z_MODIFY_ZONE_STATUS     		 								  =  157;
	public final static int C_DELETE_MULTI_ATTR      										  =  158;
	public final static int C_INSERT_MULTI_ATTR      										  =  159;
	public final static int C_MODIFY_MULTI_ATTR           							  =  160;
	public final static int C_DELETE_ATTR      													  =  161;
	public final static int C_INSERT_ATTR      							 						  =  162;
	public final static int C_MODIFY_ATTR      													  =  163;
	public final static int D_BULK_INSERT_UDEF_META_DATA_FOR_MANY_DATA	  =  164;
	public final static int R_CHANGE_OWNER                                =  165;
	public final static int C_CHANGE_MODIFY_TIMESTAMP                     =  166;
	public final static int C_CHANGE_COLL_OWNER                           =  167;
	public final static int D_INSERT_GUID                                 =  168;
	public final static int D_DELETE_GUID                                 =  169;
	public final static int D_DELETE_FROM_EXTMD_TABLE                     =  170;
	public final static int D_INSERT_INTO_EXTMD_TABLE                     =  171;
	public final static int D_MODIFY_IN_EXTMD_TABLE                       =  172;
	public final static int D_MOVE_DATA_TO_NEWCOLL                        =  173;
	public final static int D_MOVE_DATA_TO_TRASH                          =  174;
	public final static int C_MOVE_COLL_TO_NEWCOLL                        =  175;
	public final static int C_MOVE_COLL_TO_TRASH                          =  176;
	public final static int D_CHANGE_INCOLL_OWNER                         =  177;
	public final static int D_CHANGE_INCOLL_OWNER_RECUR                   =  178;
	public final static int R_CHANGE_USER_DEFINED_STRING_META_DATA        =  179;
	public final static int R_INSERT_USER_DEFINED_STRING_META_DATA        =  180;
	public final static int R_DELETE_USER_DEFINED_STRING_META_DATA        =  181;
	public final static int U_INSERT_USER_DEFINED_STRING_META_DATA        =  182;
	public final static int U_DELETE_USER_DEFINED_STRING_META_DATA        =  183;
	public final static int U_CHANGE_USER_DEFINED_STRING_META_DATA        =  184;
	public final static int R_ADJUST_LATENCY_MAX_IN_MILLISEC              =  185;
	public final static int R_ADJUST_LATENCY_MIN_IN_MILLISEC              =  186;
	public final static int R_ADJUST_BANDWIDTH_IN_MBITSPS                 =  187;
	public final static int R_ADJUST_MAXIMUM_CONCURRENCY                  =  188;
	public final static int R_ADJUST_NUM_OF_STRIPES                       =  189;
	public final static int R_ADJUST_NUM_OF_HIERARCHIES                   =  190;
	public final static int R_ADJUST_CAPACITY_IN_GIGABYTES                =  191;
	public final static int R_ADJUST_DESCRIPTION                          =  192;
	public final static int R_INSERT_RSRC_USAGE_TOTAL_BY_DATE             =  193;
	public final static int R_INSERT_RSRC_USAGE_AND_QUOTA                 =  194;
	public final static int R_DELETE_PHY_RSRC_FROM_LOG_RSRC               =  195;
	public final static int D_DELETE_USER_DEFINED_ATTR_VAL_META_DATA      =  196;
	public final static int R_DELETE_USER_DEFINED_ATTR_VAL_META_DATA      =  197;
	public final static int U_DELETE_USER_DEFINED_ATTR_VAL_META_DATA      =  198;
	public final static int C_DELETE_USER_DEFINED_ATTR_VAL_META_DATA      =  199;
	public final static int R_INSERT_PHY_RSRC_INTO_LOG_RSRC               =  200;
	public final static int R_INSERT_FREE_SPACE                           =  201;
	public final static int R_ADJUST_RSRC_COMMENT         		            =  202;
	public final static int U_CHANGE_INFO                                 =  203;
	public final static int U_ADD_GROUP_OWNER                             =  204;
	public final static int U_REMOVE_GROUP_OWNER                          =  205;
	public final static int U_UPDATE_EMAIL                                =  206;
	public final static int U_UPDATE_PHONE                                =  207;
	public final static int U_UPDATE_ADDRESS                              =  208;

	final static int SYS_ACTION_TYPE_MIN                          =  1000;
	final static int D_SU_INSERT_ACCS                             =  1001;
	final static int D_SU_DELETE_ACCS                             =  1002;
	final static int C_SU_INSERT_COLL_ACCS                        =  1003;
	final static int C_SU_DELETE_COLL_ACCS                        =  1004;
	final static int C_SU_CHANGE_COLL_NAME                        =  1005;
	final static int D_SU_DELETE_TRASH_ONE                        =  1006;
	final static int C_SU_DELETE_TRASH_COLL                       =  1007;
	final static int SYS_ACTION_TYPE_MAX                          =  2000;



	/**
	 * The tableName, attributeNames, and catalogNames arrays are a consequence
	 * of the SRB return value. A tableName and attributeName will be returned
	 * by the SRB for every catalogName. No single tableName or attributeName
	 * is unique, so the combination of the two is used to find the catalogName.
	 * The catalogName corresponds to one of the public static final metadata
	 * names, e.g. FILE_NAME, COMPOUND_RESOURCE_NAME, USER_EMAIL.
	 */
//TODO make these three faster searchable
	private static String tableNames[] = {
		"MDAS_AD_REPL",
		"MDAS_ADC_REPL",
		"MDAS_AD_REPL",
		"MDAS_CD_PAR_USER",
		"MDAS_TD_DATA_TYP",
		"MDAS_TD_USER_TYP"  ,
		"MDAS_AU_INFO",
		"MDAS_CD_USER",
		"MDAS_TD_DS_ACCS",
		"MDAS_TD_DOMN",
		"MDAS_ADC_REPL",               // 10
		"MDAS_AR_REPL",
		"MDAS_TD_LOCN",
		"MDAS_TD_RSRC_TYP",
		"MDAS_AD_REPL",
		"MDAS_TD_DATA_GRP",
		"MDAS_AU_INFO",
		"MDAS_AU_INFO",
		"MDAS_AD_REPL",
		"MDAS_AU_AUDIT" ,
		"MDAS_AU_AUDIT",             // 20
		"MDAS_AD_REPL" ,
		"MDAS_TD_ACTN"  ,
		"MDAS_AD_AUDIT" ,
		"MDAS_AD_AUDIT",
		"MDAS_TD_GRP_ACCS",
		"MDAS_TD_DATA_2GRP",
		"MDAS_AR_REPL",
		"MDAS_AR_PHYSICAL",
		"MDAS_AR_PHYSICAL",
		"MDAS_TD_RSRC_2TYP",         // 30
		"MDAS_AR_REPL",
		"MDAS_TD_DS_ACCS",
		"MDAS_TD_RSRC_ACCS",
		"MDAS_AD_REPL",
		"MDAS_CD_OWNER_USER",
		"MDAS_AU_TCKT_DATA",
		"MDAS_AU_TCKT_DATA",
		"MDAS_AU_TCKT_DATA",
		"MDAS_AU_TCKT_DATA",
		"MDAS_TD_DSTKT_ACCS",        // 40
		"MDAS_CD_DTKTOWNER",
		"MDAS_CD_DTKTUSER",
		"MDAS_AU_TCKT_GRP",
		"MDAS_AU_TCKT_GRP",
		"MDAS_AU_TCKT_GRP",
		"MDAS_AU_TCKT_GRP",
		"MDAS_TD_GRTKT_ACCS",
		"MDAS_CD_CTKTOWNER",
		"MDAS_CD_CTKTUSER",
		"MDAS_TD_CTKTU_DOMN",       // 50
		"MDAS_TD_DTKTU_DOMN",
		"MDAS_TD_CTKTO_DOMN",
		"MDAS_TD_DTKTO_DOMN",
		"MDAS_TD_DATA_GRP",
		"MDAS_TD_LOCN",
		"MDAS_AD_REPL",
		"MDAS_TD_CONTAINER",
		"MDAS_TD_CONTAINER",
		"MDAS_TD_RSRC_CLASS",
		"MDAS_AR_PHYSICAL",            // 60
		"MDAS_AD_REPL",
		"MDAS_AR_1PHYSICAL",
		"MDAS_TD_1RSRC_CLASS",
		"MDAS_ADCONT_REPL",
		"MDAS_AR_1REPL",
		"MDAS_TD_OWNER_DOMN",
		"MDAS_AU_AUTH_MAP",
		"MDAS_TD_AUTH_SCHM",
		"MDAS_TD_SRVR_LOCN",
		"MDAS_TD_SRVR_LOCN",       //  70
		"MDAS_AD_REPL",
		"MDAS_CD_ANNO_USER",
		"MDAS_TD_ANNO_DOMN",
		"MDAS_AD_ANNOTATION",
		"MDAS_AD_ANNOTATION",
		"MDAS_AD_REPL",
		"MDAS_AD_COMMENTS",
		"MDAS_AD_ANNOTATION",
		"MDAS_TD_DS_ACCS",
		"MDAS_AR_INFO",            //  80
		"MDAS_AR_INFO",
		"MDAS_AR_INFO",
		"MDAS_AR_INFO",
		"MDAS_AR_INFO",
		"MDAS_AR_INFO",
		"MDAS_AR_INFO",
		"MDAS_AR_INFO",
		"MDAS_AD_CLASS",
		"MDAS_AD_CLASS",
		"MDAS_AD_STRCT_BLOB",     //   90
		"MDAS_AD_STRCT_BLOB",
		"MDAS_ADSTRUCT_REPL",
		"MDAS_AD_STRCT_BLOB",
		"MDAS_ADIXDS_REPL",
		"MDAS_ADIXDTP_REPL",
		"MDAS_ADIXCOLL_REPL",
		"MDAS_ADMTHDS_REPL",
		"MDAS_ADMTHDTP_REPL",
		"MDAS_ADMTHCOL_REPL",
		"MDAS_TD_DGRP_IXDS",     //   100
		"MDAS_TD_DGRP_IXDTP",
		"MDAS_TD_DGRP_IXCOL",
		"MDAS_TD_DGRP_MTHDS",
		"MDAS_TD_DGRP_MTDTP",
		"MDAS_TD_DGRP_MTCOL",
		"MDAS_TD_DTYP_IXDS",
		"MDAS_TD_DTYP_IXDTP",
		"MDAS_TD_DTYP_IXCOL",
		"MDAS_TD_DTYP_MTHDS",
		"MDAS_TD_DTYP_MTDTP",   //  110
		"MDAS_TD_DTYP_MTCOL",
		"MDAS_TD_DGRP_STRUC",
		"MDAS_ADIXDS_REPL",
		"MDAS_ADIXDTP_REPL",
		"MDAS_ADIXCOLL_REPL",
		"MDAS_AD_MDATA",
		"MDAS_AD_MDATA",
		"MDAS_AD_MDATA",
		"MDAS_AD_MDATA",
		"MDAS_AD_MDATA",        // 120
		"MDAS_AD_MDATA",
		"MDAS_AD_MDATA",
		"MDAS_AD_MDATA",
		"MDAS_AD_MDATA",
		"MDAS_AD_MDATA",
		"MDAS_AD_MDATA",
		"MDAS_AD_MDATA",
		"MDAS_AD_MDATA",
		"MDAS_AU_OWNER_INFO",
		"MDAS_CD_DAUDT_USER",     // 130
		"MDAS_TD_DAUDT_DOMN",
		"MDAS_TD_COLLCONT",
		"MDAS_AD_COLLMDATA",
		"MDAS_AD_COLLMDATA",
		"MDAS_AD_COLLMDATA",
		"MDAS_AD_COLLMDATA",
		"MDAS_AD_COLLMDATA",
		"MDAS_AD_COLLMDATA",
		"MDAS_AD_COLLMDATA",
		"MDAS_AD_COLLMDATA",     // 140
		"MDAS_AD_COLLMDATA",
		"MDAS_AD_COLLMDATA",
		"MDAS_AD_COLLMDATA",
		"MDAS_AD_COLLMDATA",
		"MDAS_AD_COLLMDATA",
		"MDAS_TD_DS_ACCS",
		"MDAS_CD_USER",
		"MDAS_AD_REPL",
		"MDAS_CD_COOWN_USER",
		"MDAS_TD_DATA_GRP",           // 150
		"MDAS_TD_DATA_GRP",
		"DC_DATA",
		"DC_COLLECTION",
		"DC_TD_CONTR_TYPE",
		"DC_TD_SUBJ_CLASS",
		"DC_TD_DESCR_TYPE",
		"DC_TD_TYPE",
		"DC_TD_SOURCE_TYPE",
		"DC_TD_LANGUAGE",
		"DC_TD_RELATION_TYP",    // 160
		"DC_TD_COVERAGE",
		"DC_TD_RIGHTS",
		"DC_TITLE",
		"DC_AUTHCONTRIB",
		"DC_SUBJECT",
		"DC_DESCRIPTION",
		"DC_PUBLISHER",
		"DC_SOURCE",
		"DC_RELATION",
		"MDAS_AD_DCRELREPL",    // 170
		"MDAS_TD_DCRELGRP",
		"DC_COVERAGE",
		"DC_RIGHTS",
		"DC_PUBLISHER",
		"DC_AUTHCONTRIB",
		"DC_AUTHCONTRIB",
		"DC_AUTHCONTRIB",
		"DC_AUTHCONTRIB",
		"DC_AUTHCONTRIB",
		"MDAS_AD_GRP_2ACCS",      // 180
		"MDAS_TD_IDGRPUSER2",
		"MDAS_AD_ACCS",
		"MDAS_TD_IDGRPUSER1",
		"MDAS_AD_1MDATA",
		"MDAS_AD_2MDATA",
		"MDAS_AD_3MDATA",
		"MDAS_AD_4MDATA",
		"MDAS_AD_1MDATA",
		"MDAS_AD_2MDATA",
		"MDAS_AD_3MDATA",         // 190
		"MDAS_AD_4MDATA",
		"MDAS_AD_1COLLMDATA",
		"MDAS_AD_2COLLMDATA",
		"MDAS_AD_3COLLMDATA",
		"MDAS_AD_4COLLMDATA",
		"MDAS_AD_1COLLMDATA",
		"MDAS_AD_2COLLMDATA",
		"MDAS_AD_3COLLMDATA",
		"MDAS_AD_4COLLMDATA",
		"MDAS_AU_TCKT_GRP",                 // 200
		"MDAS_AR_1MDATA",
		"MDAS_AR_2MDATA",
		"MDAS_AR_3MDATA",
		"MDAS_AR_1MDATA",
		"MDAS_AR_2MDATA",
		"MDAS_AR_3MDATA",
		"MDAS_AU_1MDATA",
		"MDAS_AU_2MDATA",
		"MDAS_AU_3MDATA",
		"MDAS_AU_1MDATA",       // 210
		"MDAS_AU_2MDATA",
		"MDAS_AU_3MDATA",
		"MDAS_AR_TAPE_INFO",
		"MDAS_AR_TAPE_INFO",
		"MDAS_AR_TAPE_INFO",
		"MDAS_AR_TAPE_INFO",
		"MDAS_AR_TAPE_INFO",
		"MDAS_AR_TAPE_INFO",
		"MDAS_AR_TAPE_INFO",
		"MDAS_AR_TAPE_INFO",        // 220
		"MDAS_AD_REPL",
		"MDAS_AD_REPL",
		"MDAS_TD_LCK_ACCS",
		"MDAS_CD_DLOWN_USER",
		"MDAS_TD_DLOWN_DOMN",
		"MDAS_TD_COOWN_DOMN",
		"MDAS_TD_DATA_3GRP",
		"MDAS_TD_DATA_3GRP",
		"MDAS_TD_DATA_3GRP",
		"MDAS_CD_CANNO_USER",    // 230
		"MDAS_TD_CANNO_DOMN",
		"MDAS_AC_ANNOTATION",
		"MDAS_AC_ANNOTATION",
		"MDAS_AC_ANNOTATION",
		"MDAS_TD_DATA_GRP",
		"MDAS_AD_REPL",
		"MDAS_AD_REPL",
		"MDAS_AD_REPL",
		"MDAS_AD_REPL",
		"MDAS_AD_MISC1",        // 240
		"MDAS_CD_PIN_USER",
		"MDAS_TD_PIN_DOMN",
		"MDAS_AD_MISC1",
		"MDAS_AD_MISC1",
		"MDAS_AD_MISC1",
		"MDAS_AD_MISC1",
		"MDAS_AD_MISC1",
		"MDAS_AUDIT_DESC",
		"MDAS_AUDIT",
		"MDAS_AUDIT",        // 250
		"MDAS_AR_MDATA",
		"MDAS_AR_MDATA",
		"MDAS_AR_MDATA",
		"MDAS_AR_MDATA",
		"MDAS_AR_MDATA",
		"MDAS_AR_MDATA",
		"MDAS_AR_MDATA",
		"MDAS_AR_MDATA",
		"MDAS_AR_MDATA",
		"MDAS_AR_MDATA",        // 260
		"MDAS_AR_MDATA",
		"MDAS_AR_MDATA",
		"MDAS_AU_MDATA",
		"MDAS_AR_2PHYSICAL",
		"MDAS_ADC_REPL",
		"MDAS_AD_COMPOUND",
		"MDAS_AD_COMPOUND",
		"MDAS_AD_COMPOUND",
		"MDAS_AR_3PHYSICAL",
		"MDAS_TD_INT_LOCN",      // 270
		"MDAS_TD_RSRC_3TYP",
		"MDAS_AD_COMPOUND",
		"MDAS_TD_2RSRC_CLASS",
		"MDAS_AD_COMPOUND",
		"MDAS_AD_COMPOUND",
		"MDAS_CD_CANNO_USER",
		"MDAS_TD_CANNO_DOMN",
		"MDAS_AD_1COMPOUND",
		"MDAS_TD_DATA_GRP",
		"MDAS_TD_DATA_4GRP",     // 280
		"MDAS_ADC_REPL",
		"MDAS_TD_DATA_5GRP",
		"MDAS_AU_MDATA",
		"MDAS_AU_MDATA",
		"MDAS_AU_MDATA",
		"MDAS_AU_MDATA",
		"MDAS_AU_MDATA",
		"MDAS_AU_MDATA",
		"MDAS_AU_MDATA",
		"MDAS_AU_MDATA",           // 290
		"MDAS_AU_MDATA",
		"MDAS_AU_MDATA",
		"MDAS_AU_MDATA",
		"NONDISTINCT",
		"MDAS_TD_UDF",
		"MDAS_TD_UDF",
		"MDAS_TD_UDF",
		"MDAS_TD_UDF",
		"MDAS_TD_UDF",
		"MDAS_TD_ZONE",                 // 300
		"MDAS_TD_ZONE",
		"MDAS_TD_ZN_LOCN",
		"MDAS_TD_ZONE",
		"MDAS_TD_ZN_ATHSCHM",
		"MDAS_AU_ZN_ATHMAP",
		"MDAS_TD_ZONE",
		"MDAS_TD_ZONE",
		"MDAS_TD_ZONE",
		"MDAS_TD_ZONE",
		"MDAS_TD_ZONE",                 // 310
		"MDAS_CD_ZN_USER",
		"MDAS_TD_ZN_DOMN",
		"MDAS_TD_RSRC_ACCS",
		"MDAS_TD_RSRC_ACCS",
		"MDAS_AU_MDATA",
		"MDAS_AR_MDATA",
		"MDAS_CD_PAR_USER",
		"MDAS_CD_PAR_USER",
		"MDAS_TD_PAR_DOMN",
		"MDAS_AD_MDATA",                // 320
		"MDAS_AD_COLLMDATA",
		"MDAS_AU_MDATA",
		"MDAS_AR_MDATA",
		"MDAS_AT_DATA_TYP_EXT",
		"MDAS_AT_DATA_TYP_EXT",
		"MDAS_AR_REPL",
		"MDAS_AR_REPL",
		"MDAS_AR_REPL",
		"MDAS_CD_USER",
		"MDAS_CD_USER",             // 330
		"MDAS_AR_REPL",
		"MDAS_CD_RS_USER",
		"MDAS_TD_RS_DOMN",
		"MDAS_AR_INFO",
		"MDAS_AR_INFO",
		"MDAS_AR_INFO",
		"MDAS_AR_INFO",
		"MDAS_AR_INFO",
		"MDAS_AR_INFO",
		"MDAS_AR_INFO",             // 340
		"MDAS_CD_RSAC_USER",
		"MDAS_TD_RSAC_DOMN",
		"MDAS_CD_RSAC_USER",
		"MDAS_CD_RS_USER",
		"MDAS_CD_OWNER_USER",
		"MDAS_CD_DTKTOWNER",
		"MDAS_CD_DTKTUSER",
		"MDAS_CD_CTKTOWNER",
		"MDAS_CD_CTKTUSER",
		"MDAS_CD_ANNO_USER",             // 350
		"MDAS_CD_DAUDT_USER",
		"MDAS_CD_COOWN_USER",
		"MDAS_CD_DLOWN_USER",
		"MDAS_CD_CANNO_USER",
		"MDAS_CD_PIN_USER",
		"MDAS_CD_CANNO_USER",
		"MDAS_TD_PAR_LOCN",
		"MDAS_TD_RSRC_ACCS",
		"MDAS_TD_DTYP_PARENT",
		"MDAS_TD_ZN_LOCN",             // 360
		"MDAS_TD_PARDOMN",
		"MDAS_TD_UTYP_PARENT",
		"MDAS_TD_RSRC_PARENT",
		"MDAS_CD_RAGRP_USER",
		"MDAS_TD_RAGRP_DOMN",
		"MDAS_CD_RAGRP_USER",
		"MDAS_TD_DATA_GRP",
		"MDAS_CD_DAGRP_USER",
		"MDAS_TD_DAGRP_DOMN",
		"MDAS_CD_DAGRP_USER",             // 370
		"MDAS_CD_CAGRP_USER",
		"MDAS_TD_CAGRP_DOMN",
		"MDAS_CD_CAGRP_USER",
		"MDAS_TD_CADATA_GRP",
		"MDAS_TD_CAGRP_ACCS",
		"MDAS_TD_CCDATA_TYP",
		"MDAS_AD_MDATA",
		"MDAS_AD_COLLMDATA",
		"MDAS_AR_MDATA",
		"MDAS_AU_MDATA",        // 380
		"MDAS_AD_REPL",
		"MDAS_AD_GUID",
		"MDAS_AD_GUID",
		"MDAS_AD_GUID",
		"MDAS_TD_ORDERBY",
	};


	/**
	 * The tableName, attributeNames, and catalogNames arrays are a consequence
	 * of the SRB return value. A tableName and attributeName will be returned
	 * by the SRB for every catalogName. No single tableName or attributeName
	 * is unique, so the combination of the two is used to find the catalogName.
	 * The catalogName corresponds to one of the public static final metadata
	 * names, e.g. FILE_NAME, COMPOUND_RESOURCE_NAME, USER_EMAIL.
	 */
	private static String attributeNames[] = {
		"data_id",
		"repl_enum",
		"data_name",
		"user_name",
		"data_typ_name",
		"user_typ_name",
		"user_address",
		"user_name",
		"access_constraint",
		"domain_desc",
		"path_name",              // 10
		"rsrc_name",
		"netprefix",
		"rsrc_typ_name",
		"modify_timestamp",
		"data_grp_name",
		"user_phone",
		"user_email",
		"data_size",
		"time_stamp" ,
		"comments",               // 20
		"d_comment",
		"action_desc",
		"time_stamp",
		"comments",
		"access_constraint",
		"data_grp_name",
		"default_path",
		"phy_default_path",
		"phy_rsrc_name",
		"rsrc_typ_name",        // 30
		"repl_enum",
		"access_list",
		"access_list",
		"is_deleted",
		"user_name",
		"ticket_id",
		"begin_time",
		"end_time",
		"access_count",
		"access_constraint",      // 40
		"user_name",
		"user_name",
		"ticket_id",
		"begin_time",
		"end_time",
		"access_count",
		"access_constraint",
		"user_name",
		"user_name",
		"domain_desc",             // 50
		"domain_desc",
		"domain_desc",
		"domain_desc",
		"parent_grp_name",
		"locn_desc",
		"is_dirty",
		"container_max_size",
		"container_name",
		"rsrc_class_name",
		"max_obj_size",  //  60
		"offset",
		"phy_rsrc_name",
		"rsrc_class_name",
		"data_size",
		"rsrc_name",
		"domain_desc",
		"distin_name",
		"auth_scheme",
		"locn_desc",
		"netprefix",     //  70
		"repl_enum",
		"user_name",
		"domain_desc",
		"annotations",
		"anno_date",
		"repl_timestamp",
		"com_date",
		"anno_position",
		"access_privilege",
		"mlsec_latency_max",   //  80
		"mlsec_latency_min",
		"mbps_bandwidth",
		"concurrency_max",
		"num_of_hierarchies",
		"num_of_stripes",
		"megab_capacity",
		"description",
		"class_name",
		"class_type",
		"structure_type",         //  90
		"structure_comments",
		"data_name",
		"internal_structure",
		"data_name",
		"data_name",
		"data_name",
		"data_name",
		"data_name",
		"data_name",
		"data_grp_name",        // 100
		"data_grp_name",
		"data_grp_name",
		"data_grp_name",
		"data_grp_name",
		"data_grp_name",
		"data_typ_name",
		"data_typ_name",
		"data_typ_name",
		"data_typ_name",
		"data_typ_name",       //  110
		"data_typ_name",
		"data_grp_name",
		"path_name",
		"path_name",
		"path_name",
		"metadatanum",
		"userdef_metastr0",
		"userdef_metastr1",
		"userdef_metastr2",
		"userdef_metastr3",    //  120
		"userdef_metastr4",
		"userdef_metastr5",
		"userdef_metastr6",
		"userdef_metastr7",
		"userdef_metastr8",
		"userdef_metastr9",
		"userdef_metaint0",
		"userdef_metaint1",
		"user_email",
		"user_name",          //  130
		"domain_desc",
		"container_name",
		"metadatanum",
		"userdef_metastr0",
		"userdef_metastr1",
		"userdef_metastr2",
		"userdef_metastr3",
		"userdef_metastr4",
		"userdef_metastr5",
		"userdef_metastr6",   //  140
		"userdef_metastr7",
		"userdef_metastr8",
		"userdef_metastr9",
		"userdef_metaint0",
		"userdef_metaint1",
		"access_id",
		"user_id",
		"last_accs_time",
		"user_name",
		"coll_cr_timestamp",             //  150
		"coll_comments",
		"data_name",
		"data_grp_name",
		"dc_contr_type_name",
		"dc_subj_class_name",
		"dc_desc_type_name",
		"dc_type_name",
		"source_type_name",
		"language_name",
		"relation_type_name",   //  160
		"coverage_type_name",
		"rights_type_name",
		"dc_title",
		"dc_contr_name",
		"dc_subject_name",
		"dc_desc",
		"dc_pub_name",
		"source_desc",
		"related_data_desc",
		"data_name",              //  170
		"data_grp_name",
		"coverage_desc",
		"rights_data_desc",
		"dc_pub_addr",
		"dc_contr_address",
		"dc_contr_email",
		"dc_contr_phone",
		"dc_contr_web",
		"dc_contr_corp_name",
		"access_id",           //  180
		"user_id",
		"access_id",
		"user_id",
		"userdef_metastr0",
		"userdef_metastr0",
		"userdef_metastr0",
		"userdef_metastr0",
		"userdef_metastr1",
		"userdef_metastr1",
		"userdef_metastr1",    // 190
		"userdef_metastr1",
		"userdef_metastr0",
		"userdef_metastr0",
		"userdef_metastr0",
		"userdef_metastr0",
		"userdef_metastr1",
		"userdef_metastr1",
		"userdef_metastr1",
		"userdef_metastr1",
		"is_recursive",             //  200
		"userdef_metastr0",
		"userdef_metastr0",
		"userdef_metastr0",
		"userdef_metastr1",
		"userdef_metastr1",
		"userdef_metastr1",
		"userdef_metastr0",
		"userdef_metastr0",
		"userdef_metastr0",
		"userdef_metastr1",        // 210
		"userdef_metastr1",
		"userdef_metastr1",
		"tapeNumber",
		"tapeOwner",
		"tapeType",
		"tapeLibInx",
		"fullFlag",
		"currentFileSeqNum",
		"currentAbsPosition",
		"bytesWritten",             //  220
		"version_num",
		"datalock",
		"access_constraint",
		"user_name",
		"domain_desc",
		"domain_desc",
		"data_grp_name",
		"coll_cr_timestamp",
		"coll_comments",
		"user_name",      //  230
		"domain_desc",
		"annotations",
		"anno_date",
		"anno_type",
		"data_grp_id",
		"dhide",
		"dchecksum",
		"auditflag",
		"dlockexpiry",
		"dpin",             //  240
		"user_name",
		"domain_desc",
		"dpinexpiry",
		"dexpire_date",
		"dcompressed",
		"dencrypted",
		"dexpire_date_2",
		"actionDesc",
		"aud_timestamp",
		"aud_comments",          //  250
		"userdef_metastr0",
		"userdef_metastr1",
		"userdef_metastr2",
		"userdef_metastr3",
		"userdef_metastr4",
		"userdef_metastr5",
		"userdef_metastr6",
		"userdef_metastr7",
		"userdef_metastr8",
		"userdef_metastr9",       //  260
		"userdef_metaint1",
		"metadatanum",
		"metadatanum",
		"phy_rsrc_name",
		"seg_num",
		"int_repl_num",
		"int_seg_num",
		"cmpd_path_name",
		"phy_rsrc_name",
		"netprefix",             // 270
		"rsrc_typ_name",
		"is_dirty",
		"rsrc_class_name",
		"offset",
		"data_size",
		"user_name",
		"domain_desc",
		"cmpd_path_name",
		"coll_link",
		"data_grp_name",     // 280
		"data_name",
		"data_grp_name",
		"userdef_metastr0",
		"userdef_metastr1",
		"userdef_metastr2",
		"userdef_metastr3",
		"userdef_metastr4",
		"userdef_metastr5",
		"userdef_metastr6",
		"userdef_metastr7",         // 290
		"userdef_metastr8",
		"userdef_metastr9",
		"userdef_metaint0",
		"nondistinct",
		"udf",
		"udf",
		"udf",
		"udf",
		"udf",
		"zone_id",                 // 300
		"local_zone_flag",
		"netprefix",
		"port_number",
		"auth_scheme",
		"distin_name",
		"zone_status",
		"zone_create_date",
		"zone_modify_date",
		"zone_comments",
		"zone_contact",           // 310
		"user_name",
		"domain_desc",
		"access_privilege",
		"access_constraint",
		"userdef_metaint1",
		"userdef_metaint1",
		"user_modify_date",
		"zone_id",
		"domain_desc",
		"userdef_modif_date",                 // 320
		"userdef_modif_date",
		"userdef_modif_date",
		"userdef_modif_date",
		"data_typ_mime",
		"data_typ_ext",
		"rsrc_comments",
		"rsrc_creation_date",
		"rsrc_modify_date",
		"user_creation_date",
		"user_modify_date",                 // 330
		"max_obj_size",
		"user_name",
		"domain_desc",
		"mlsec_latency_max",
		"mlsec_latency_min",
		"mbps_bandwidth",
		"concurrency_max",
		"num_of_hierarchies",
		"num_of_stripes",
		"megab_capacity",                 // 340
		"user_name",
		"domain_desc",
		"zone_id",
		"zone_id",
		"zone_id",
		"zone_id",
		"zone_id",
		"zone_id",
		"zone_id",
		"zone_id",                 // 350
		"zone_id",
		"zone_id",
		"zone_id",
		"zone_id",
		"zone_id",
		"zone_id",
		"locn_desc",
		"access_id",
		"data_typ_name",
		"locn_desc",                 // 360
		"domain_desc",
		"user_typ_name",
		"rsrc_typ_name",
		"user_name",
		"domain_desc",
		"zone_id",
		"coll_mod_timestamp",
		"user_name",
		"domain_desc",
		"zone_id",                // 370
		"user_name",
		"domain_desc",
		"zone_id",
		"data_grp_name",
		"access_constraint",
		"data_typ_name",
		"userdef_creat_date",
		"userdef_creat_date",
		"userdef_creat_date",
		"userdef_creat_date",                 // 380
		"container_id",
		"guid",
		"guid_flag",
		"guid_time_stamp",
		"orderby",
	};


	/**
	 * The tableName, attributeNames, and catalogNames arrays are a consequence
	 * of the SRB return value. A tableName and attributeName will be returned
	 * by the SRB for every catalogName. No single tableName or attributeName
	 * is unique, so the combination of the two is used to find the catalogName.
	 * The catalogName corresponds to one of the public static final metadata
	 * names, e.g. FILE_NAME, COMPOUND_RESOURCE_NAME, USER_EMAIL.
	 */
	private static String catalogNames[] = {
		FILE_ID,
		FILE_REPLICATION_ENUM,
		FILE_NAME,
		USER_GROUP_NAME,
		FILE_TYPE_NAME,
		USER_TYPE_NAME,
		USER_ADDRESS,
		USER_NAME,
		ACCESS_CONSTRAINT,
		USER_DOMAIN,
		PATH_NAME,				//	10
		RESOURCE_NAME,
		RESOURCE_ADDRESS_NETPREFIX,
		RESOURCE_TYPE_NAME,
		MODIFICATION_DATE,
		DIRECTORY_NAME,
		USER_PHONE,
		USER_EMAIL,
		SIZE,
		USER_AUDIT_TIME_STAMP,
		USER_AUDIT_COMMENTS,				//	20
		FILE_COMMENTS,
		AUDIT_ACTION_DESCRIPTION,
		AUDIT_TIMESTAMP,
		AUDIT_COMMENTS,
		DIRECTORY_ACCESS_CONSTRAINT,
		ACCESS_DIRECTORY_NAME,
		RESOURCE_DEFAULT_PATH,
		PHYSICAL_RESOURCE_DEFAULT_PATH,
		PHYSICAL_RESOURCE_NAME,
		PHYSICAL_RESOURCE_TYPE_NAME,				//	30
		RESOURCE_REPLICATION_ENUM,
		FILE_ACCESS_LIST,
		RESOURCE_ACCESS_LIST,
		FILE_IS_DELETED,
		OWNER,
		TICKET_D,
		TICKET_BEGIN_TIME_D,
		TICKET_END_TIME_D,
		TICKET_ACC_COUNT_D,
		TICKET_ACC_LIST_D,				//	40
		TICKET_OWNER_D,
		TICKET_USER_D,
		TICKET_C,
		TICKET_BEGIN_TIME_C,
		TICKET_END_TIME_C,
		TICKET_ACC_COUNT_C,
		TICKET_ACC_LIST_C,
		TICKET_OWNER_C,
		TICKET_USER_C,
		TICKET_USER_DOMAIN_C,				//	50
		TICKET_USER_DOMAIN_D,
		TICKET_OWNER_DOMAIN_C,
		TICKET_OWNER_DOMAIN_D,
		PARENT_DIRECTORY_NAME,
		LOCATION_NAME,
		IS_DIRTY,
		CONTAINER_MAX_SIZE,
		CONTAINER_NAME,
		RESOURCE_CLASS,
		MAX_OBJ_SIZE,				//	60
		OFFSET,
		CONTAINER_RESOURCE_NAME,
		CONTAINER_RESOURCE_CLASS,
		CONTAINER_SIZE,
		CONTAINER_LOG_RESOURCE_NAME,
		OWNER_DOMAIN,
		USER_DISTINGUISHED_NAME,
		USER_AUTHENTICATION_SCHEME,
		SERVER_LOCATION,
		SERVER_NETPREFIX,				//	70
		CONTAINER_REPLICATION_ENUM,
		FILE_ANNOTATION_USERNAME,
		FILE_ANNOTATION_USERDOMAIN,
		FILE_ANNOTATION,
		FILE_ANNOTATION_TIMESTAMP,
		CREATION_DATE,
		FILE_COMMENTS_TIMESTAMP,
		FILE_ANNOTATION_POSITION,
		FILE_ACCESS_PRIVILEGE,
		RESOURCE_MAX_LATENCY,				//	80
		RESOURCE_MIN_LATENCY,
		RESOURCE_BANDWIDTH,
		RESOURCE_MAX_CONCURRENCY,
		RESOURCE_NUM_OF_HIERARCHIES,
		RESOURCE_NUM_OF_STRIPES,
		RESOURCE_CAPACITY,
		RESOURCE_DESCRIPTION,
		FILE_CLASS_NAME,
		FILE_CLASS_TYPE,
		STRUCTURED_METADATA_TYPE,				//	90
		STRUCTURED_METADATA_COMMENTS,
		STRUCTURED_METADATA_FILE_NAME,
		INTERNAL_STRUCTURED_METADATA,
		INDEX_NAME_FOR_FILE,
		INDEX_NAME_FOR_DATATYPE,
		INDEX_NAME_FOR_DIRECTORY,
		METHOD_NAME_FOR_FILE,
		METHOD_NAME_FOR_DATATYPE,
		METHOD_NAME_FOR_DIRECTORY,
		IX_DIRECTORY_NAME_FOR_FILE,				//	100
		IX_DIRECTORY_NAME_FOR_DATATYPE,
		IX_DIRECTORY_NAME_FOR_DIRECTORY,
		METHOD_DIRECTORY_NAME_FOR_FILE,
		METHOD_DIRECTORY_NAME_FOR_DATATYPE,
		METHOD_DIRECTORY_NAME_FOR_DIRECTORY,
		IX_DATATYPE_FOR_FILE,
		IX_DATATYPE_FOR_DATATYPE,
		IX_DATATYPE_FOR_DIRECTORY,
		METHOD_DATATYPE_FOR_FILE,
		METHOD_DATATYPE_FOR_DATATYPE,				//	110
		METHOD_DATATYPE_FOR_DIRECTORY,
		STRUCTURED_METADATA_DIRECTORY_NAME,
		IX_LOCATION_FOR_FILE,
		IX_LOCATION_FOR_DATATYPE,
		IX_LOCATION_FOR_DIRECTORY,
		METADATA_NUM,
		DEFINABLE_METADATA_FOR_FILES0,
		DEFINABLE_METADATA_FOR_FILES1,
		DEFINABLE_METADATA_FOR_FILES,
		DEFINABLE_METADATA_FOR_FILES,				//	120
		DEFINABLE_METADATA_FOR_FILES,
		DEFINABLE_METADATA_FOR_FILES,
		DEFINABLE_METADATA_FOR_FILES,
		DEFINABLE_METADATA_FOR_FILES,
		DEFINABLE_METADATA_FOR_FILES,
		DEFINABLE_METADATA_FOR_FILES,
		INTEGER_DEFINABLE_METADATA0,
		INTEGER_DEFINABLE_METADATA1,
		OWNER_EMAIL,
		AUDIT_USER,				//	130
		AUDIT_USER_DOMAIN,
		CONTAINER_FOR_DIRECTORY,
		METADATA_NUM_DIRECTORY,
		DEFINABLE_METADATA_FOR_DIRECTORIES0,
		DEFINABLE_METADATA_FOR_DIRECTORIES1,
		DEFINABLE_METADATA_FOR_DIRECTORIES,
		DEFINABLE_METADATA_FOR_DIRECTORIES,
		DEFINABLE_METADATA_FOR_DIRECTORIES,
		DEFINABLE_METADATA_FOR_DIRECTORIES,
		DEFINABLE_METADATA_FOR_DIRECTORIES,				//	140
		DEFINABLE_METADATA_FOR_DIRECTORIES,
		DEFINABLE_METADATA_FOR_DIRECTORIES,
		DEFINABLE_METADATA_FOR_DIRECTORIES,
		INTEGER_DEFINABLE_METADATA_FOR_DIRECTORIES0,
		INTEGER_DEFINABLE_METADATA_FOR_DIRECTORIES1,
		FILE_ACCESS_ID,
		USER_ID,
		FILE_LAST_ACCESS_TIMESTAMP,
		DIRECTORY_OWNER_NAME,
		DIRECTORY_CREATE_TIMESTAMP,				//	150
		DIRECTORY_COMMENTS,
		DC_DATA_NAME,
		DC_COLLECTION,
		DC_CONTRIBUTOR_TYPE,
		DC_SUBJECT_CLASS,
		DC_DESCRIPTION_TYPE,
		DC_TYPE,
		DC_SOURCE_TYPE,
		DC_LANGUAGE,
		DC_RELATION_TYPE,				//	160
		DC_COVERAGE_TYPE,
		DC_RIGHTS_TYPE,
		DC_TITLE,
		DC_CONTRIBUTOR_NAME,
		DC_SUBJECT_NAME,
		DC_DESCRIPTION,
		DC_PUBLISHER,
		DC_SOURCE,
		DC_RELATED_DATA_DESCRIPTION,
		DC_RELATED_DATA,				//	170
		DC_RELATED_DIRECTORY,
		DC_COVERAGE,
		DC_RIGHTS,
		DC_PUBLISHER_ADDR,
		DC_CONTRIBUTOR_ADDR,
		DC_CONTRIBUTOR_EMAIL,
		DC_CONTRIBUTOR_PHONE,
		DC_CONTRIBUTOR_WEB,
		DC_CONTRIBUTOR_AFFILIATION,
		COLL_ACCS_ID,				//	180
		COLL_ACCS_USER_ID,
		DATA_ACCS_ID,
		DATA_ACCS_USER_ID,
		DEFINABLE_METADATA_FOR_FILES0_1,
		DEFINABLE_METADATA_FOR_FILES0_2,
		DEFINABLE_METADATA_FOR_FILES0_3,
		DEFINABLE_METADATA_FOR_FILES0_4,
		DEFINABLE_METADATA_FOR_FILES1_1,
		DEFINABLE_METADATA_FOR_FILES1_2,
		DEFINABLE_METADATA_FOR_FILES1_3,				//	190
		DEFINABLE_METADATA_FOR_FILES1_4,
		DEFINABLE_METADATA_FOR_DIRECTORIES0_1,
		DEFINABLE_METADATA_FOR_DIRECTORIES0_2,
		DEFINABLE_METADATA_FOR_DIRECTORIES0_3,
		DEFINABLE_METADATA_FOR_DIRECTORIES0_4,
		DEFINABLE_METADATA_FOR_DIRECTORIES1_1,
		DEFINABLE_METADATA_FOR_DIRECTORIES1_2,
		DEFINABLE_METADATA_FOR_DIRECTORIES1_3,
		DEFINABLE_METADATA_FOR_DIRECTORIES1_4,
		TICKET_RECURSIVE,				//	200
		DEFINABLE_METADATA_FOR_RESOURCES0_1,
		DEFINABLE_METADATA_FOR_RESOURCES0_2,
		DEFINABLE_METADATA_FOR_RESOURCES0_3,
		DEFINABLE_METADATA_FOR_RESOURCES1_1,
		DEFINABLE_METADATA_FOR_RESOURCES1_2,
		DEFINABLE_METADATA_FOR_RESOURCES1_3,
		DEFINABLE_METADATA_FOR_USERS0_1,
		DEFINABLE_METADATA_FOR_USERS0_2,
		DEFINABLE_METADATA_FOR_USERS0_3,
		DEFINABLE_METADATA_FOR_USERS1_1,				//	210
		DEFINABLE_METADATA_FOR_USERS1_2,
		DEFINABLE_METADATA_FOR_USERS1_3,
		TAPE_NUMBER,
		TAPE_OWNER,
		TAPE_TYPE_VAL,
		TAPE_LIBINX,
		TAPE_FULLFLAG,
		TAPE_CURRENT_FILE_SEQNUM,
		TAPE_CURRENT_ABS_POSITION,
		TAPE_BYTES_WRITTEN,				//	220
		FILE_VER_NUM,
		FILE_LOCK_NUM,
		FILE_LOCK_DESCRIPTION,
		FILE_LOCK_OWNER_NAME,
		FILE_LOCK_OWNER_DOMAIN,
		DIRECTORY_OWNER_DOMAIN,
		TICKET_DIRECTORY_NAME,
		TICKET_DIRECTORY_CREATE_TIMESTAMP,
		TICKET_DIRECTORY_COMMENTS,
		DIRECTORY_ANNOTATION_USERNAME,				//	230
		DIRECTORY_ANNOTATION_USERDOMAIN,
		DIRECTORY_ANNOTATION,
		DIRECTORY_ANNOTATION_TIMESTAMP,
		DIRECTORY_ANNOTATION_TYPE,
		FILE_GROUP_ID,
		FILE_HIDE,
		FILE_CHECKSUM,
		FILE_AUDITFLAG,
		FILE_LOCK_EXPIRY,
		FILE_PIN_VAL,				//	240
		FILE_PIN_OWNER_NAME,
		FILE_PIN_OWNER_DOMAIN,
		FILE_PIN_EXPIRY,
		FILE_EXPIRY_DATE,
		FILE_IS_COMPRESSED,
		FILE_IS_ENCRYPTED,
		FILE_EXPIRE_DATE_2,
		NEW_AUDIT_ACTION_DESCRIPTION,
		NEW_AUDIT_TIMESTAMP,
		NEW_AUDIT_COMMENTS,				//	250
		DEFINABLE_METADATA_FOR_RESOURCES0,
		DEFINABLE_METADATA_FOR_RESOURCES1,
		DEFINABLE_METADATA_FOR_RESOURCES2,
		DEFINABLE_METADATA_FOR_RESOURCES3,
		DEFINABLE_METADATA_FOR_RESOURCES4,
		DEFINABLE_METADATA_FOR_RESOURCES5,
		DEFINABLE_METADATA_FOR_RESOURCES6,
		DEFINABLE_METADATA_FOR_RESOURCES7,
		DEFINABLE_METADATA_FOR_RESOURCES8,
		DEFINABLE_METADATA_FOR_RESOURCES9,				//	260
		DEFINABLE_INTEGER_METADATA_RESOURCE0,
		METADATA_NUM_RESOURCE,
		METADATA_NUM_USER,
		COMPOUND_RESOURCE_NAME,
		FILE_SEG_NUM,
		INT_REPLICATION_ENUM,
		INT_SEG_NUM,
		INT_PATH_NAME,
		INT_RESOURCE_NAME,
		INT_RESOURCE_ADDRESS_NETPREFIX,				//	270
		INT_RESOURCE_TYPE_NAME,
		INT_IS_DIRTY,
		INT_RESOURCE_CLASS,
		INT_OFFSET,
		INT_SIZE,
		COMP_OBJ_USER_NAME,
		COMP_OBJ_USER_DOMAIN,
		INT_PATH_NAME_FOR_REPL,
		DIRECTORY_LINK_NUM,				//**	MDAS_TD_DATA_GRP,	**
		REAL_DIRECTORY_NAME,				//	280
		FILE_CONTAINER_NAME,
		DIRECTORY_CONTAINER_NAME,
		DEFINABLE_METADATA_FOR_USERS0,
		DEFINABLE_METADATA_FOR_USERS1,
		DEFINABLE_METADATA_FOR_USERS2,
		DEFINABLE_METADATA_FOR_USERS3,
		DEFINABLE_METADATA_FOR_USERS4,
		DEFINABLE_METADATA_FOR_USERS5,
		DEFINABLE_METADATA_FOR_USERS6,
		DEFINABLE_METADATA_FOR_USERS7,				//	290
		DEFINABLE_METADATA_FOR_USERS8,
		DEFINABLE_METADATA_FOR_USERS9,
		UDIMD_USER0,
		NONDISTINCT,
		USERDEFFUNC01,
		USERDEFFUNC02,
		USERDEFFUNC03,
		USERDEFFUNC04,
		USERDEFFUNC05,
		ZONE_NAME,							//	300
		ZONE_LOCALITY,
		ZONE_NETPREFIX,
		ZONE_PORT_NUM,
		ZONE_ADMIN_AUTH_SCHEME_NAME,
		ZONE_ADMIN_DISTIN_NAME,
		ZONE_STATUS,
		ZONE_CREATE_DATE,
		ZONE_MODIFY_DATE,
		ZONE_COMMENTS,
		ZONE_CONTACT,							//	310
		ZONE_ADMIN_NAME,
		ZONE_ADMIN_DOMAIN_NAME,
		RSRC_ACCESS_PRIVILEGE,
		RSRC_ACCESS_CONSTRAINT,
		UDIMD_USER1,
		UDIMD_RSRC1,
		USER_GROUP_MODIFY_DATE,
		USER_GROUP_ZONE_NAME,
		USER_GROUP_DOMAIN_DESC,
		DATA_UDEF_MDATA_MODIFY_DATE,							//	320
		COLL_UDEF_MDATA_MODIFY_DATE,
		USER_UDEF_MDATA_MODIFY_DATE,
		RSRC_UDEF_MDATA_MODIFY_DATE,
		DATA_TYPE_MIME_STRING,
		DATA_TYPE_EXTENDERS,
		RSRC_COMMENTS,
		RSRC_CREATE_DATE,
		RSRC_MODIFY_DATE,
		USER_CREATE_DATE,
		USER_MODIFY_DATE,							//	330
		RSRC_MAX_OBJ_SIZE,
		RSRC_OWNER_NAME,
		RSRC_OWNER_DOMAIN,
		RSRC_MLSEC_LATENCY_MAX,
		RSRC_MLSEC_LATENCY_MIN,
		RSRC_MBPS_BANDWIDTH,
		RSRC_CONCURRENCY_MAX,
		RSRC_NUM_OF_HIERARCHIES,
		RSRC_NUM_OF_STRIPES,
		RSRC_MEGAB_CAPACITY,							//	340
		RSRC_ACCS_USER_NAME,
		RSRC_ACCS_USER_DOMAIN,
		RSRC_ACCS_USER_ZONE,
		RSRC_OWNER_ZONE,
		DATA_OWNER_ZONE,
		TICKET_OWNER_ZONE_D,
		TICKET_USER_ZONE_D,
		TICKET_OWNER_ZONE_C,
		TICKET_USER_ZONE_C,
		DATA_ANNOTATION_USER_ZONE,							//	350
		AUDIT_USER_ZONE,
		COLL_OWNER_ZONE,
		DATA_LOCK_OWNER_ZONE,
		COLL_ANNOTATION_USER_ZONE,
		DATA_PIN_OWNER_ZONE,
		COMP_OBJ_USER_ZONE,
		PARENT_SERVER_LOCATION,
		RSRC_ACCESS_ID,
		PARENT_DATA_TYPE,
		ZONE_LOCN_DESC,							//	360
		PARENT_DOMAIN_DESC,
		PARENT_USER_TYPE,
		PARENT_RSRC_TYPE,
		RSRC_ACCS_GRPUSER_NAME,
		RSRC_ACCS_GRPUSER_DOMAIN,
		RSRC_ACCS_GRPUSER_ZONE,
		COLL_MODIFY_TIMESTAMP,
		DATA_ACCS_GRPUSER_NAME,
		DATA_ACCS_GRPUSER_DOMAIN,
		DATA_ACCS_GRPUSER_ZONE,							//	370
		COLL_ACCS_GRPUSER_NAME,
		COLL_ACCS_GRPUSER_DOMAIN,
		COLL_ACCS_GRPUSER_ZONE,
		COLL_ACCS_COLLECTION_NAME,
		COLL_ACCS_ACCESS_CONSTRAINT,
		DATA_TYPE_FOR_CONTAINER_FOR_COLLECTION,
		DATA_UDEF_MDATA_CREATE_DATE,
		COLL_UDEF_MDATA_CREATE_DATE,
		RSRC_UDEF_MDATA_CREATE_DATE,
		USER_UDEF_MDATA_CREATE_DATE,							//	380
		CONTAINER_ID,
		GUID,
		GUID_FLAG,
		GUID_TIME_STAMP,
		ORDERBY,
		DELETE_FILE_ID,
		DELETE_FILE_REPLICATION_ENUM,
		DELETE_FILE_NAME,
		DELETE_MODIFICATION_DATE,
		DELETE_DIRECTORY_NAME,
	};


	/**
	 * The unmodified SRB metadata attribute names, as string values.
	 * Known uses, ORDERBY, see also SRBMetaDataCommands.srbGenQuery
	 */
	private static String srbCatalogNames[] = {
		"DATA_ID",
		"DATA_REPL_ENUM",
		"DATA_NAME",
		"USER_GROUP_NAME",
		"DATA_TYP_NAME",
		"USER_TYP_NAME",
		"USER_ADDRESS",
		"USER_NAME",
		"ACCESS_CONSTRAINT",
		"DOMAIN_DESC",
		"PATH_NAME",
		"RSRC_NAME",
		"RSRC_ADDR_NETPREFIX",
		"RSRC_TYP_NAME",
		"REPL_TIMESTAMP",
		"DATA_GRP_NAME",
		"USER_PHONE",
		"USER_EMAIL",
		"SIZE",
		"USER_AUDIT_TIME_STAMP",
		"USER_AUDIT_COMMENTS",
		"DATA_COMMENTS",
		"AUDIT_ACTION_DESC",
		"AUDIT_TIMESTAMP",
		"AUDIT_COMMENTS",
		"COLLECTION_ACCESS_CONSTRAINT",
		"ACCESS_COLLECTION_NAME",
		"RSRC_DEFAULT_PATH",
		"PHY_RSRC_DEFAULT_PATH",
		"PHY_RSRC_NAME",
		"PHY_RSRC_TYP_NAME",
		"RSRC_REPL_ENUM",
		"DATA_ACCESS_LIST",
		"RSRC_ACCESS_LIST",
		"DATA_IS_DELETED",
		"DATA_OWNER",
		"TICKET_D",
		"TICKET_BEGIN_TIME_D",
		"TICKET_END_TIME_D",
		"TICKET_ACC_COUNT_D",
		"TICKET_ACC_LIST_D",
		"TICKET_OWNER_D",
		"TICKET_USER_D",
		"TICKET_C",
		"TICKET_BEGIN_TIME_C",
		"TICKET_END_TIME_C",
		"TICKET_ACC_COUNT_C",
		"TICKET_ACC_LIST_C",
		"TICKET_OWNER_C",
		"TICKET_USER_C",
		"TICKET_USER_DOMAIN_C",
		"TICKET_USER_DOMAIN_D",
		"TICKET_OWNER_DOMAIN_C",
		"TICKET_OWNER_DOMAIN_D",
		"PARENT_COLLECTION_NAME",
		"LOCATION_NAME",
		"IS_DIRTY",
		"CONTAINER_MAX_SIZE",
		"CONTAINER_NAME",
		"RSRC_CLASS",
		"MAX_OBJ_SIZE",
		"OFFSET",
		"CONTAINER_RSRC_NAME",
		"CONTAINER_RSRC_CLASS",
		"CONTAINER_SIZE",
		"CONTAINER_LOG_RSRC_NAME",
		"DATA_OWNER_DOMAIN",
		"USER_DISTIN_NAME",
		"USER_AUTH_SCHEME",
		"SERVER_LOCATION",
		"SERVER_NETPREFIX",
		"CONTAINER_REPL_ENUM",
		"DATA_ANNOTATION_USERNAME",
		"DATA_ANNOTATION_USERDOMAIN",
		"DATA_ANNOTATION",
		"DATA_ANNOTATION_TIMESTAMP",
		"DATA_CREATE_TIMESTAMP",
		"DATA_COMMENTS_TIMESTAMP",
		"DATA_ANNOTATION_POSITION",
		"DATA_ACCESS_PRIVILEGE",
		"RESOURCE_MAX_LATENCY",
		"RESOURCE_MIN_LATENCY",
		"RESOURCE_BANDWIDTH",
		"RESOURCE_MAX_CONCURRENCY",
		"RESOURCE_NUM_OF_HIERARCHIES",
		"RESOURCE_NUM_OF_STRIPES",
		"RESOURCE_CAPACITY",
		"RESOURCE_DESCRIPTION",
		"DATA_CLASS_NAME",
		"DATA_CLASS_TYPE",
		"STRUCTURED_METADATA_TYPE",
		"STRUCTURED_METADATA_COMMENTS",
		"STRUCTURED_METADATA_DATA_NAME",
		"INTERNAL_STRUCTURED_METADATA",
		"INDEX_NAME_FOR_DATASET",
		"INDEX_NAME_FOR_DATATYPE",
		"INDEX_NAME_FOR_COLLECTION",
		"METHOD_NAME_FOR_DATASET",
		"METHOD_NAME_FOR_DATATYPE",
		"METHOD_NAME_FOR_COLLECTION",
		"IX_COLL_NAME_FOR_DATASET",
		"IX_COLLNAME_FOR_DATATYPE",
		"IX_COLLNAME_FOR_COLLECTION",
		"MTH_COLLNAME_FOR_DATASET",
		"MTH_COLLNAME_FOR_DATATYPE",
		"MTH_COLLNAME_FOR_COLLECTION",
		"IX_DATATYPE_FOR_DATASET",
		"IX_DATATYPE_FOR_DATATYPE",
		"IX_DATATYPE_FOR_COLLECTION",
		"MTH_DATATYPE_FOR_DATASET",
		"MTH_DATATYPE_FOR_DATATYPE",
		"MTH_DATATYPE_FOR_COLLECTION",
		"STRUCTURED_METADATA_COLLNAME",
		"IX_LOCATION_FOR_DATASET",
		"IX_LOCATION_FOR_DATATYPE",
		"IX_LOCATION_FOR_COLLECTION",
		"METADATA_NUM",
		"UDSMD0",
		"UDSMD1",
		"UDSMD2",
		"UDSMD3",
		"UDSMD4",
		"UDSMD5",
		"UDSMD6",
		"UDSMD7",
		"UDSMD8",
		"UDSMD9",
		"UDIMD0",
		"UDIMD1",
		"DATA_OWNER_EMAIL",
		"AUDIT_USER",
		"AUDIT_USER_DOMAIN",
		"CONTAINER_FOR_COLLECTION",
		"METADATA_NUM_COLL",
		"UDSMD_COLL0",
		"UDSMD_COLL1",
		"UDSMD_COLL2",
		"UDSMD_COLL3",
		"UDSMD_COLL4",
		"UDSMD_COLL5",
		"UDSMD_COLL6",
		"UDSMD_COLL7",
		"UDSMD_COLL8",
		"UDSMD_COLL9",
		"UDIMD_COLL0",
		"UDIMD_COLL1",
		"DATA_ACCESS_ID",
		"USER_ID",
		"DATA_LAST_ACCESS_TIMESTAMP",
		"COLL_OWNER_NAME",
		"COLL_CREATE_TIMESTAMP",
		"COLL_COMMENTS",
		"DC_DATA_NAME",
		"DC_COLLECTION",
		"DC_CONTRIBUTOR_TYPE",
		"DC_SUBJECT_CLASS",
		"DC_DESCRIPTION_TYPE",
		"DC_TYPE",
		"DC_SOURCE_TYPE",
		"DC_LANGUAGE",
		"DC_RELATION_TYPE",
		"DC_COVERAGE_TYPE",
		"DC_RIGHTS_TYPE",
		"DC_TITLE",
		"DC_CONTRIBUTOR_NAME",
		"DC_SUBJECT_NAME",
		"DC_DESCRIPTION",
		"DC_PUBLISHER",
		"DC_SOURCE",
		"DC_RELATED_DATA_DESCR",
		"DC_RELATED_DATA",
		"DC_RELATED_COLL",
		"DC_COVERAGE",
		"DC_RIGHTS",
		"DC_PUBLISHER_ADDR",
		"DC_CONTRIBUTOR_ADDR",
		"DC_CONTRIBUTOR_EMAIL",
		"DC_CONTRIBUTOR_PHONE",
		"DC_CONTRIBUTOR_WEB",
		"DC_CONTRIBUTOR_CORPNAME",
		"COLL_ACCS_ID",
		"COLL_ACCS_USER_ID",
		"DATA_ACCS_ID",
		"DATA_ACCS_USER_ID",
		"UDSMD0_1",
		"UDSMD0_2",
		"UDSMD0_3",
		"UDSMD0_4",
		"UDSMD1_1",
		"UDSMD1_2",
		"UDSMD1_3",
		"UDSMD1_4",
		"UDSMD_COLL0_1",
		"UDSMD_COLL0_2",
		"UDSMD_COLL0_3",
		"UDSMD_COLL0_4",
		"UDSMD_COLL1_1",
		"UDSMD_COLL1_2",
		"UDSMD_COLL1_3",
		"UDSMD_COLL1_4",
		"C_TICKET_RECURSIVE",
		"UDSMD_RSRC1_0",
		"UDSMD_RSRC1_1",
		"UDSMD_RSRC1_2",
		"UDSMD_RSRC2_0",
		"UDSMD_RSRC2_1",
		"UDSMD_RSRC2_2",
		"UDSMD_USER1_0",
		"UDSMD_USER1_1",
		"UDSMD_USER1_2",
		"UDSMD_USER2_0",
		"UDSMD_USER2_1",
		"UDSMD_USER2_2",
		"TAPE_NUMBER",
		"TAPE_OWNER",
		"TAPE_TYPE_VAL",
		"TAPE_LIBINX",
		"TAPE_FULLFLAG",
		"TAPE_CURRENT_FILE_SEQNUM",
		"TAPE_CURRENT_ABS_POSITION",
		"TAPE_BYTES_WRITTEN",
		"DATA_VER_NUM",
		"DATA_LOCK_NUM",
		"DATA_LOCK_DESC",
		"DATA_LOCK_OWNER_NAME",
		"DATA_LOCK_OWNER_DOMAIN",
		"COLL_OWNER_DOMAIN",
		"C_TICKET_COLL_NAME",
		"C_TICKET_COLL_CREATE_TIMESTAMP",
		"C_TICKET_COLL_COMMENTS",
		"COLL_ANNOTATION_USERNAME",
		"COLL_ANNOTATION_USERDOMAIN",
		"COLL_ANNOTATION",
		"COLL_ANNOTATION_TIMESTAMP",
		"COLL_ANNOTATION_TYPE",
		"DATA_GRP_ID",
		"DATA_HIDE",
		"DATA_CHECKSUM",
		"DATA_AUDITFLAG",
		"DATA_LOCK_EXPIRY",
		"DATA_PIN_VAL",
		"DATA_PIN_OWNER_NAME",
		"DATA_PIN_OWNER_DOMAIN",
		"DATA_PIN_EXPIRY",
		"DATA_EXPIRY_DATE",
		"DATA_IS_COMPRESSED",
		"DATA_IS_ENCRYPTED",
		"DATA_EXPIRE_DATE_2",
		"NEW_AUDIT_ACTION_DESC",
		"NEW_AUDIT_TIMESTAMP",
		"NEW_AUDIT_COMMENTS",
		"UDSMD_RSRC0",
		"UDSMD_RSRC1",
		"UDSMD_RSRC2",
		"UDSMD_RSRC3",
		"UDSMD_RSRC4",
		"UDSMD_RSRC5",
		"UDSMD_RSRC6",
		"UDSMD_RSRC7",
		"UDSMD_RSRC8",
		"UDSMD_RSRC9",
		"UDIMD_RSRC0",
		"METADATA_NUM_RSRC",
		"METADATA_NUM_USER",
		"COMPOUND_RSRC_NAME",
		"DATA_SEG_NUM",
		"INT_REPL_ENUM",
		"INT_SEG_NUM",
		"INT_PATH_NAME",
		"INT_RSRC_NAME",
		"INT_RSRC_ADDR_NETPREFIX",
		"INT_RSRC_TYP_NAME",
		"INT_IS_DIRTY",
		"INT_RSRC_CLASS",
		"INT_OFFSET",
		"INT_SIZE",
		"COMP_OBJ_USER_NAME",
		"COMP_OBJ_USER_DOMAIN",
		"INT_PATH_NAME_FOR_REPL",
		"COLLECTION_LINK_NUM",
		"REAL_COLLECTION_NAME",
		"DATA_CONT_NAME",
		"COLLECTION_CONT_NAME",
		"UDSMD_USER0",
		"UDSMD_USER1",
		"UDSMD_USER2",
		"UDSMD_USER3",
		"UDSMD_USER4",
		"UDSMD_USER5",
		"UDSMD_USER6",
		"UDSMD_USER7",
		"UDSMD_USER8",
		"UDSMD_USER9",
		"UDIMD_USER0",
		"NONDISTINCT",
		"USERDEFFUNC01",
		"USERDEFFUNC02",
		"USERDEFFUNC03",
		"USERDEFFUNC04",
		"USERDEFFUNC05",
		"ZONE_NAME",
		"ZONE_LOCALITY",
		"ZONE_NETPREFIX",
		"ZONE_PORT_NUM",
		"ZONE_ADMIN_AUTH_SCHEME_NAME",
		"ZONE_ADMIN_DISTIN_NAME",
		"ZONE_STATUS",
		"ZONE_CREATE_DATE",
		"ZONE_MODIFY_DATE",
		"ZONE_COMMENTS",
		"ZONE_CONTACT",
		"ZONE_ADMIN_NAME",
		"ZONE_ADMIN_DOMAIN_NAME",
		"RSRC_ACCESS_PRIVILEGE",
		"RSRC_ACCESS_CONSTRAINT",
		"UDIMD_USER1",
		"UDIMD_RSRC1",
		"USER_GROUP_MODIFY_DATE",
		"USER_GROUP_ZONE_NAME",
		"USER_GROUP_DOMAIN_DESC",
		"DATA_UDEF_MDATA_MODIFY_DATE",
		"COLl_UDEF_MDATA_MODIFY_DATE",
		"USER_UDEF_MDATA_MODIFY_DATE",
		"RSRC_UDEF_MDATA_MODIFY_DATE",
		"DATA_TYPE_MIME_STRING",
		"DATA_TYPE_EXTENDERS",
		"RSRC_COMMENTS",
		"RSRC_CREATE_DATE",
		"RSRC_MODIFY_DATE",
		"USER_CREATE_DATE",
		"USER_MODIFY_DATE",
		"RSRC_MAX_OBJ_SIZE",
		"RSRC_OWNER_NAME",
		"RSRC_OWNER_DOMAIN",
		"RSRC_MLSEC_LATENCY_MAX",
		"RSRC_MLSEC_LATENCY_MIN",
		"RSRC_MBPS_BANDWIDTH",
		"RSRC_CONCURRENCY_MAX",
		"RSRC_NUM_OF_HIERARCHIES",
		"RSRC_NUM_OF_STRIPES",
		"RSRC_MEGAB_CAPACITY",
		"RSRC_ACCS_USER_NAME",
		"RSRC_ACCS_USER_DOMAIN",
		"RSRC_ACCS_USER_ZONE",
		"RSRC_OWNER_ZONE",
		"DATA_OWNER_ZONE",
		"TICKET_OWNER_ZONE_D",
		"TICKET_USER_ZONE_D",
		"TICKET_OWNER_ZONE_C",
		"TICKET_USER_ZONE_C",
		"DATA_ANNOTATION_USER_ZONE",
		"AUDIT_USER_ZONE",
		"COLL_OWNER_ZONE",
		"DATA_LOCK_OWNER_ZONE",
		"COLL_ANNOTATION_USER_ZONE",
		"DATA_PIN_OWNER_ZONE",
		"COMP_OBJ_USER_ZONE",
		"PARENT_SERVER_LOCATION",
		"RSRC_ACCESS_ID",
		"PARENT_DATA_TYPE",
		"ZONE_LOCN_DESC",
		"PARENT_DOMAIN_DESC",
		"PARENT_USER_TYPE",
		"PARENT_RSRC_TYPE",
		"RSRC_ACCS_GRPUSER_NAME",
		"RSRC_ACCS_GRPUSER_DOMAIN",
		"RSRC_ACCS_GRPUSER_ZONE",
		"COLL_MODIFY_TIMESTAMP",
		"DATA_ACCS_GRPUSER_NAME",
		"DATA_ACCS_GRPUSER_DOMAIN",
		"DATA_ACCS_GRPUSER_ZONE",
		"COLL_ACCS_GRPUSER_NAME",
		"COLL_ACCS_GRPUSER_DOMAIN",
		"COLL_ACCS_GRPUSER_ZONE",
		"COLL_ACCS_COLLECTION_NAME",
		"COLL_ACCS_ACCESS_CONSTRAINT",
		"DATA_TYPE_FOR_CONTAINER_FOR_COLLECTION",
		"DATA_UDEF_MDATA_CREATE_DATE",
		"COLL_UDEF_MDATA_CREATE_DATE",
		"RSRC_UDEF_MDATA_CREATE_DATE",
		"USER_UDEF_MDATA_CREATE_DATE",
		"CONTAINER_ID",
		"GUID",
		"GUID_FLAG",
		"GUID_TIME_STAMP",
		"ORDERBY",
		"DEL_DATA_ID",
		"DEL_DATA_REPL_ENUM",
		"DEL_DATA_NAME",
		"DEL_REPL_TIMESTAMP",
		"DEL_COLLECTION_NAME",
		"DEL_COLLECTION_ID",
		"DEL_DATA_COMMENT"
	};



//----------------------------------------------------------------------
// Static
//----------------------------------------------------------------------
	static {
		//*******************************************************************
		// API metadata field names related to the integers sent to SRB for metadata
		//*******************************************************************
		/** internal data id */
		generalNameToSRBID.put( FILE_ID, new Integer( 0 ));

		/** replica copy number */
		generalNameToSRBID.put( FILE_REPLICATION_ENUM, new Integer( 1 ));

		/** data name */
		generalNameToSRBID.put( FILE_NAME, new Integer( 2 ));

		/** name of user group */
		generalNameToSRBID.put( USER_GROUP_NAME, new Integer( 3 ));

		/** data type */
		generalNameToSRBID.put( FILE_TYPE_NAME, new Integer( 4 ));

		/** user type */
		generalNameToSRBID.put( USER_TYPE_NAME, new Integer( 5 ));

		/** user address */
		generalNameToSRBID.put( USER_ADDRESS, new Integer( 6 ));

		/** user name */
		generalNameToSRBID.put( USER_NAME, new Integer( 7 ));

		/** access restriction on  data */
		generalNameToSRBID.put( ACCESS_CONSTRAINT, new Integer( 8 ));

		/** user domain name */
		generalNameToSRBID.put( USER_DOMAIN, new Integer( 9 ));

		/** physical path name of data object */
		generalNameToSRBID.put( PATH_NAME, new Integer( 10 ));

		/** name of logical resource */
		generalNameToSRBID.put( RESOURCE_NAME, new Integer( 11 ));

		/** net address of resource */
		generalNameToSRBID.put( RESOURCE_ADDRESS_NETPREFIX, new Integer( 12 ));

		/** logical resource type */
		generalNameToSRBID.put( RESOURCE_TYPE_NAME, new Integer( 13 ));

		/** data modification time stamp */
		generalNameToSRBID.put( MODIFICATION_DATE, new Integer( 14 ));

		/** collection name */
		generalNameToSRBID.put( DIRECTORY_NAME, new Integer( 15 ));

		/** user phone number */
		generalNameToSRBID.put( USER_PHONE, new Integer( 16 ));

		/** user email */
		generalNameToSRBID.put( USER_EMAIL, new Integer( 17 ));

		/** size of data */
		generalNameToSRBID.put( SIZE, new Integer( 18 ));

		/** audit on user time stamp */
		generalNameToSRBID.put( USER_AUDIT_TIME_STAMP, new Integer( 19 ));

		/** audit on user comments */
		generalNameToSRBID.put( USER_AUDIT_COMMENTS, new Integer( 20 ));

		/** comments on data */
		generalNameToSRBID.put( FILE_COMMENTS, new Integer( 21 ));

		/** audited action  on data */
		generalNameToSRBID.put( AUDIT_ACTION_DESCRIPTION, new Integer( 22 ));

		/** audit time stamp for data */
		generalNameToSRBID.put( AUDIT_TIMESTAMP, new Integer( 23 ));

		/** audit comments  for data */
		generalNameToSRBID.put( AUDIT_COMMENTS, new Integer( 24 ));

		/** access on collection (26)*/
		generalNameToSRBID.put( DIRECTORY_ACCESS_CONSTRAINT, new Integer( 25 ));

		/** collection name for access in (25) */
		generalNameToSRBID.put( ACCESS_DIRECTORY_NAME, new Integer( 26 ));

		/** default path in logical resource */
		generalNameToSRBID.put( RESOURCE_DEFAULT_PATH, new Integer( 27 ));

		/** default path in physical resource */
		generalNameToSRBID.put( PHYSICAL_RESOURCE_DEFAULT_PATH, new Integer( 28 ));

		/** physical resource name */
		generalNameToSRBID.put( PHYSICAL_RESOURCE_NAME, new Integer( 29 ));

		/** physical resource type */
		generalNameToSRBID.put( PHYSICAL_RESOURCE_TYPE_NAME, new Integer( 30 ));

		/** index of physical rsrc in logical rsrc*/
		generalNameToSRBID.put( RESOURCE_REPLICATION_ENUM, new Integer( 31 ));

		/** access list for data */
		generalNameToSRBID.put( FILE_ACCESS_LIST, new Integer( 32 ));

		/** access list for resource */
		generalNameToSRBID.put( RESOURCE_ACCESS_LIST, new Integer( 33 ));

		/** data liveness */
		generalNameToSRBID.put( FILE_IS_DELETED, new Integer( 34 ));

		/** data creator name */
		generalNameToSRBID.put( OWNER, new Integer( 35 ));

		/** identifier for ticket given for data*/
		generalNameToSRBID.put( TICKET_D, new Integer( 36 ));

		/** data ticket validity start time */
		generalNameToSRBID.put( TICKET_BEGIN_TIME_D, new Integer( 37 ));

		/** data ticket validity end  time */
		generalNameToSRBID.put( TICKET_END_TIME_D, new Integer( 38 ));

		/** valid number of opens allowed on data ticket */
		generalNameToSRBID.put( TICKET_ACC_COUNT_D, new Integer( 39 ));

		/** valid access allowed on data ticket (currently readonly) */
		generalNameToSRBID.put( TICKET_ACC_LIST_D, new Integer( 40 ));

		/** data ticket creator */
		generalNameToSRBID.put( TICKET_OWNER_D, new Integer( 41 ));

		/** allowed ticket user or user group */
		generalNameToSRBID.put( TICKET_USER_D, new Integer( 42 ));

		/** identifier for ticket given for collection and sub collections*/
		generalNameToSRBID.put( TICKET_C, new Integer( 43 ));

		/** collection ticket validity start time*/
		generalNameToSRBID.put( TICKET_BEGIN_TIME_C, new Integer( 44 ));

		/** collection ticket validity end time*/
		generalNameToSRBID.put( TICKET_END_TIME_C, new Integer( 45 ));

		/** valid number of opens allowed on data in collection */
		generalNameToSRBID.put( TICKET_ACC_COUNT_C, new Integer( 46 ));

		/** valid access allowed on data in collection  (currently readonly) */
		generalNameToSRBID.put( TICKET_ACC_LIST_C, new Integer( 47 ));

		/** collection ticket creator */
		generalNameToSRBID.put( TICKET_OWNER_C, new Integer( 48 ));

		/** allowed collection ticket user */
		generalNameToSRBID.put( TICKET_USER_C, new Integer( 49 ));

		/** collection ticket user domain */
		generalNameToSRBID.put( TICKET_USER_DOMAIN_C, new Integer( 50 ));

		/** data ticket user domain */
		generalNameToSRBID.put( TICKET_USER_DOMAIN_D, new Integer( 51 ));

		/** collection ticket creator domain */
		generalNameToSRBID.put( TICKET_OWNER_DOMAIN_C, new Integer( 52 ));

		/** data ticket creator domain */
		generalNameToSRBID.put( TICKET_OWNER_DOMAIN_D, new Integer( 53 ));

		/** name of parent collection (15) */
		generalNameToSRBID.put( PARENT_DIRECTORY_NAME, new Integer( 54 ));

		/** location (of resource) name */
		generalNameToSRBID.put( LOCATION_NAME, new Integer( 55 ));

		/**
		 * data has been changed compared to other copies
		 * (currently used only for containered data)
		 */
		generalNameToSRBID.put( IS_DIRTY, new Integer( 56 ));

		/** maximum size of container */
		generalNameToSRBID.put( CONTAINER_MAX_SIZE, new Integer( 57 ));

		/** name of container */
		generalNameToSRBID.put( CONTAINER_NAME, new Integer( 58 ));

		/** classification of resource */
		generalNameToSRBID.put( RESOURCE_CLASS, new Integer( 59 ));

		/** maximum size of data object allowed in resource (not enforced by MCAT) */
		generalNameToSRBID.put( MAX_OBJ_SIZE, new Integer( 60 ));

		/** position of data in container */
		generalNameToSRBID.put( OFFSET, new Integer( 61 ));

		/** name of physical resource of container */
		generalNameToSRBID.put( CONTAINER_RESOURCE_NAME, new Integer( 62 ));

		/** class of physical resource of container */
		generalNameToSRBID.put( CONTAINER_RESOURCE_CLASS, new Integer( 63 ));

		/** current size of container */
		generalNameToSRBID.put( CONTAINER_SIZE, new Integer( 64 ));

		/** logical resource associated with a  container */
		generalNameToSRBID.put( CONTAINER_LOG_RESOURCE_NAME, new Integer( 65 ));

		/** domain of data creator */
		generalNameToSRBID.put( OWNER_DOMAIN, new Integer( 66 ));

		/** distinguished name of user (used by authentication systems ) */
		generalNameToSRBID.put( USER_DISTINGUISHED_NAME, new Integer( 67 ));

		/** user authentication scheme (67) */
		generalNameToSRBID.put( USER_AUTHENTICATION_SCHEME, new Integer( 68 ));

		/** location of SRB server */
		generalNameToSRBID.put( SERVER_LOCATION, new Integer( 69 ));

		/** net address of SRB server */
		generalNameToSRBID.put( SERVER_NETPREFIX, new Integer( 70 ));

		/** container copy number */
		generalNameToSRBID.put( CONTAINER_REPLICATION_ENUM, new Integer( 71 ));

		/** name of annotator */
		generalNameToSRBID.put( FILE_ANNOTATION_USERNAME, new Integer( 72 ));

		/** domain of annotator */
		generalNameToSRBID.put( FILE_ANNOTATION_USERDOMAIN, new Integer( 73 ));

		/** annotation on data */
		generalNameToSRBID.put( FILE_ANNOTATION, new Integer( 74 ));

		/** time of annotation */
		generalNameToSRBID.put( FILE_ANNOTATION_TIMESTAMP, new Integer( 75 ));

		/** data creation time stamp */
		generalNameToSRBID.put( CREATION_DATE, new Integer( 76 ));

		/** time stamp for comments on data */
		generalNameToSRBID.put( FILE_COMMENTS_TIMESTAMP, new Integer( 77 ));

		/** some user-defined location for the annotation */
		generalNameToSRBID.put( FILE_ANNOTATION_POSITION, new Integer( 78 ));

		/** access privilege on data */
		generalNameToSRBID.put( FILE_ACCESS_PRIVILEGE, new Integer( 79 ));

		/** physical resource estimated latency (max) */
		generalNameToSRBID.put( RESOURCE_MAX_LATENCY, new Integer( 80 ));

		/** physical resource estimated latency (min) */
		generalNameToSRBID.put( RESOURCE_MIN_LATENCY, new Integer( 81 ));

		/** physical resource estimated bandwidth */
		generalNameToSRBID.put( RESOURCE_BANDWIDTH, new Integer( 82 ));

		/** physical resource maximum concurrent requests */
		generalNameToSRBID.put( RESOURCE_MAX_CONCURRENCY, new Integer( 83 ));

		/** number of hierarchies in the physical resource */
		generalNameToSRBID.put( RESOURCE_NUM_OF_HIERARCHIES, new Integer( 84 ));

		/** number of striping of data in the physical resource */
		generalNameToSRBID.put( RESOURCE_NUM_OF_STRIPES, new Integer( 85 ));

		/** capacity of  the physical resource */
		generalNameToSRBID.put( RESOURCE_CAPACITY, new Integer( 86 ));

		/** comments on the resource */
		generalNameToSRBID.put( RESOURCE_DESCRIPTION, new Integer( 87 ));

		/** classifcation name for data different from (4)*/
		generalNameToSRBID.put( FILE_CLASS_NAME, new Integer( 88 ));

		/** classification type */
		generalNameToSRBID.put( FILE_CLASS_TYPE, new Integer( 89 ));

		/** type of user-inserted structured metadata for data in (92) or (93) */
		generalNameToSRBID.put( STRUCTURED_METADATA_TYPE, new Integer( 90 ));

		/** comments on the structured metadata in (92) or (93) */
		generalNameToSRBID.put( STRUCTURED_METADATA_COMMENTS, new Integer( 91 ));

		/**
		 * data name of user-inserted structured metadata stored as
		 * another data object inside SRB see also (112)
		 */
		generalNameToSRBID.put( STRUCTURED_METADATA_FILE_NAME, new Integer( 92 ));

		/** user-inserted structured metadata stored as a string inside MCAT */
		generalNameToSRBID.put( INTERNAL_STRUCTURED_METADATA, new Integer( 93 ));

		/** data name of index on data */
		generalNameToSRBID.put( INDEX_NAME_FOR_FILE, new Integer( 94 ));

		/** data name of index on data type */
		generalNameToSRBID.put( INDEX_NAME_FOR_DATATYPE, new Integer( 95 ));

		/** data name of index on collection */
		generalNameToSRBID.put( INDEX_NAME_FOR_DIRECTORY, new Integer( 96 ));

		/** data name of method on data */
		generalNameToSRBID.put( METHOD_NAME_FOR_FILE, new Integer( 97 ));

		/** data name of method on data type */
		generalNameToSRBID.put( METHOD_NAME_FOR_DATATYPE, new Integer( 98 ));

		/** data name of method on collection */
		generalNameToSRBID.put( METHOD_NAME_FOR_DIRECTORY, new Integer( 99 ));

		/** collection name of index on data (4) */
		generalNameToSRBID.put( IX_DIRECTORY_NAME_FOR_FILE, new Integer( 100 ));

		/** collection name of index on data type (95) */
		generalNameToSRBID.put( IX_DIRECTORY_NAME_FOR_DATATYPE, new Integer( 101 ));

		/** collection name of index on collection  (96) */
		generalNameToSRBID.put( IX_DIRECTORY_NAME_FOR_DIRECTORY, new Integer( 102 ));

		/** collection name of method on data  (97) */
		generalNameToSRBID.put( METHOD_DIRECTORY_NAME_FOR_FILE, new Integer( 103 ));

		/** collection name of method on data type (98) */
		generalNameToSRBID.put( METHOD_DIRECTORY_NAME_FOR_DATATYPE, new Integer( 104 ));

		/** collection name of method on collection (99) */
		generalNameToSRBID.put( METHOD_DIRECTORY_NAME_FOR_DIRECTORY, new Integer( 105 ));

		/** index type for (94) */
		generalNameToSRBID.put( IX_DATATYPE_FOR_FILE, new Integer( 106 ));

		/** index type for (95) */
		generalNameToSRBID.put( IX_DATATYPE_FOR_DATATYPE, new Integer( 107 ));

		/** index type for (96) */
		generalNameToSRBID.put( IX_DATATYPE_FOR_DIRECTORY, new Integer( 108 ));

		/** method type for (97) */
		generalNameToSRBID.put( METHOD_DATATYPE_FOR_FILE, new Integer( 109 ));

		/** method type for (98) */
		generalNameToSRBID.put( METHOD_DATATYPE_FOR_DATATYPE, new Integer( 110 ));

		/** method type for (99) */
		generalNameToSRBID.put( METHOD_DATATYPE_FOR_DIRECTORY, new Integer( 111 ));

		/**
		 * collection name of user-inserted structured metadata stored as
		 * another data object inside SRB see also (92)
		 */
		generalNameToSRBID.put( STRUCTURED_METADATA_DIRECTORY_NAME, new Integer( 112 ));

		/** path name of index in (94)*/
		generalNameToSRBID.put( IX_LOCATION_FOR_FILE, new Integer( 113 ));

		/** path name of index in (95)*/
		generalNameToSRBID.put( IX_LOCATION_FOR_DATATYPE, new Integer( 114 ));

		/** path name of index in (96)*/
		generalNameToSRBID.put( IX_LOCATION_FOR_DIRECTORY, new Integer( 115 ));


		//definable file********************************************************
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_FILES, new Integer( -1 ));

		/** metadata num */
		generalNameToSRBID.put( METADATA_NUM, new Integer( 116 ));

		/** user-defined string metadata  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_FILES0, new Integer( 117 ));
		/** user-defined string metadata  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_FILES1, new Integer( 118 ));
		/** user-defined string metadata  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_FILES2, new Integer( 119 ));
		/** user-defined string metadata  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_FILES3, new Integer( 120 ));
		/** user-defined string metadata  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_FILES4, new Integer( 121 ));
		/** user-defined string metadata  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_FILES5, new Integer( 122 ));
		/** user-defined string metadata  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_FILES6, new Integer( 123 ));
		/** user-defined string metadata  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_FILES7, new Integer( 124 ));
		/** user-defined string metadata  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_FILES8, new Integer( 125 ));
		/** user-defined string metadata  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_FILES9, new Integer( 126 ));
		/** user-defined string metadata  */
		generalNameToSRBID.put( INTEGER_DEFINABLE_METADATA0, new Integer( 127 ));
		/** user-defined string metadata  */
		generalNameToSRBID.put( INTEGER_DEFINABLE_METADATA1, new Integer( 128 ));

		/** user-defined string metadata  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_FILES0_0, new Integer( 117 ));
		/* user-defined string metadata  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_FILES0_1, new Integer( 184 ));
		/* user-defined string metadata  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_FILES0_2, new Integer( 185 ));
		/* user-defined string metadata  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_FILES0_3, new Integer( 186 ));
		/* user-defined string metadata  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_FILES0_4, new Integer( 187 ));
		/** user-defined string metadata  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_FILES1_0, new Integer( 118 ));
		/* user-defined string metadata  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_FILES1_1, new Integer( 188 ));
		/* user-defined string metadata  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_FILES1_2, new Integer( 189 ));
		/* user-defined string metadata  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_FILES1_3, new Integer( 190 ));
		/* user-defined string metadata  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_FILES1_4, new Integer( 191 ));
		//end definable********************************************************

		/** email of data creator */
		generalNameToSRBID.put( OWNER_EMAIL, new Integer( 129 ));

		/** audit user */
		generalNameToSRBID.put( AUDIT_USER, new Integer( 130 ));

		/** audit user domain */
		generalNameToSRBID.put( AUDIT_USER_DOMAIN, new Integer( 131 ));

		/** default container  for collection */
		generalNameToSRBID.put( CONTAINER_FOR_DIRECTORY, new Integer( 132 ));


		//definable dir********************************************************
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_DIRECTORIES, new Integer( -2 ));

		/** user-defined string metadata  for collection*/
		generalNameToSRBID.put( METADATA_NUM_DIRECTORY, new Integer( 133 ));

		/** user-defined string metadata  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_DIRECTORIES0, new Integer( 134 ));
		/** user-defined string metadata  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_DIRECTORIES1, new Integer( 135 ));
		/** user-defined string metadata  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_DIRECTORIES2, new Integer( 136 ));
		/** user-defined string metadata  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_DIRECTORIES3, new Integer( 137 ));
		/** user-defined string metadata  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_DIRECTORIES4, new Integer( 138 ));
		/** user-defined string metadata  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_DIRECTORIES5, new Integer( 139 ));
		/** user-defined string metadata  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_DIRECTORIES6, new Integer( 140 ));
		/** user-defined string metadata  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_DIRECTORIES7, new Integer( 141 ));
		/** user-defined string metadata  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_DIRECTORIES8, new Integer( 142 ));
		/** user-defined string metadata  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_DIRECTORIES9, new Integer( 143 ));
		/** user-defined string metadata  */
		generalNameToSRBID.put( INTEGER_DEFINABLE_METADATA_FOR_DIRECTORIES0, new Integer( 144 ));
		/** user-defined string metadata  */
		generalNameToSRBID.put( INTEGER_DEFINABLE_METADATA_FOR_DIRECTORIES1, new Integer( 145 ));

		/* user-defined string metadata for collections  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_DIRECTORIES0_0, new Integer( 134 ));
		/* user-defined string metadata for collections  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_DIRECTORIES0_1, new Integer( 192 ));
		/* user-defined string metadata for collections  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_DIRECTORIES0_2, new Integer( 193 ));
		/* user-defined string metadata for collections  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_DIRECTORIES0_3, new Integer( 194 ));
		/* user-defined string metadata for collections  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_DIRECTORIES0_4, new Integer( 195 ));
		/* user-defined string metadata for collections  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_DIRECTORIES1_0, new Integer( 135 ));
		/* user-defined string metadata for collections  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_DIRECTORIES1_1, new Integer( 196 ));
		/* user-defined string metadata for collections  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_DIRECTORIES1_2, new Integer( 197 ));
		/* user-defined string metadata for collections  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_DIRECTORIES1_3, new Integer( 198 ));
		/* user-defined string metadata for collections  */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_DIRECTORIES1_4, new Integer( 199 ));
		//end definable********************************************************


		/** MCAT's internal Id for Access Type */
		generalNameToSRBID.put( FILE_ACCESS_ID, new Integer( 146 ));

		/** MCAT's internal Id for the User */
		generalNameToSRBID.put( USER_ID, new Integer( 147 ));

		/** data last access time stamp */
		generalNameToSRBID.put( FILE_LAST_ACCESS_TIMESTAMP, new Integer( 148 ));

		/**  */
		generalNameToSRBID.put( DIRECTORY_OWNER_NAME, new Integer( 149 ));
		/** UNUSED */
		generalNameToSRBID.put( DIRECTORY_CREATE_TIMESTAMP, new Integer( 150 ));
		/** UNUSED */
		generalNameToSRBID.put( DIRECTORY_COMMENTS, new Integer( 151 ));

		/** Dublin Core Data Name same as (2) */
		generalNameToSRBID.put( DC_DATA_NAME, new Integer( 152 ));

		/** DC: Collection Name same as  (15) */
		generalNameToSRBID.put( DC_COLLECTION, new Integer( 153 ));

		/** DC: Contributor Type: Eg. Author, Illustrator */
		generalNameToSRBID.put( DC_CONTRIBUTOR_TYPE, new Integer( 154 ));

		/** DC: Subject Classification */
		generalNameToSRBID.put( DC_SUBJECT_CLASS, new Integer( 155 ));

		/** DC: Type of Description */
		generalNameToSRBID.put( DC_DESCRIPTION_TYPE, new Integer( 156 ));

		/** DC: Type of the Object */
		generalNameToSRBID.put( DC_TYPE, new Integer( 157 ));

		/** DC: Type of the Source */
		generalNameToSRBID.put( DC_SOURCE_TYPE, new Integer( 158 ));

		/** DC: Language of the Object */
		generalNameToSRBID.put( DC_LANGUAGE, new Integer( 159 ));

		/** DC: Relation with another Object in (170,171) */
		generalNameToSRBID.put( DC_RELATION_TYPE, new Integer( 160 ));

		/** DC: Coverage Type */
		generalNameToSRBID.put( DC_COVERAGE_TYPE, new Integer( 161 ));

		/** DC: Rights Type */
		generalNameToSRBID.put( DC_RIGHTS_TYPE, new Integer( 162 ));

		/** DC: Title of the Object */
		generalNameToSRBID.put( DC_TITLE, new Integer( 163 ));

		/** DC: Contributor Name. NOT same as (7) */
		generalNameToSRBID.put( DC_CONTRIBUTOR_NAME, new Integer( 164 ));

		/** DC: Subject */
		generalNameToSRBID.put( DC_SUBJECT_NAME, new Integer( 165 ));

		/** DC: Description */
		generalNameToSRBID.put( DC_DESCRIPTION, new Integer( 166 ));

		/** DC: Publisher Name */
		generalNameToSRBID.put( DC_PUBLISHER, new Integer( 167 ));

		/** DC: Source Name */
		generalNameToSRBID.put( DC_SOURCE, new Integer( 168 ));

		/** DC: Related Data Description */
		generalNameToSRBID.put( DC_RELATED_DATA_DESCRIPTION, new Integer( 169 ));

		/** DC: Date Related to (152,153) */
		generalNameToSRBID.put( DC_RELATED_DATA, new Integer( 170 ));

		/** DC:  */
		generalNameToSRBID.put( DC_RELATED_DIRECTORY, new Integer( 171 ));

		/** DC: Coverage Information */
		generalNameToSRBID.put( DC_COVERAGE, new Integer( 172 ));

		/** DC: Rights Information */
		generalNameToSRBID.put( DC_RIGHTS, new Integer( 173 ));

		/** DC: Publisher Address */
		generalNameToSRBID.put( DC_PUBLISHER_ADDR, new Integer( 174 ));

		/** DC: Contributro Address */
		generalNameToSRBID.put( DC_CONTRIBUTOR_ADDR, new Integer( 175 ));

		/** DC: Contributor Email */
		generalNameToSRBID.put( DC_CONTRIBUTOR_EMAIL, new Integer( 176 ));

		/** DC: Contributor Phone */
		generalNameToSRBID.put( DC_CONTRIBUTOR_PHONE, new Integer( 177 ));

		/** DC: Contributor Web Address */
		generalNameToSRBID.put( DC_CONTRIBUTOR_WEB, new Integer( 178 ));

		/** DC: Contributor Affiliation */
		generalNameToSRBID.put( DC_CONTRIBUTOR_AFFILIATION, new Integer( 179 ));

		/** directory access id */
		generalNameToSRBID.put( COLL_ACCS_ID, new Integer( 180 ));

		/*** UNUSED */
		generalNameToSRBID.put( COLL_ACCS_USER_ID, new Integer( 181 ));

		/*** UNUSED */
		generalNameToSRBID.put( DATA_ACCS_ID, new Integer( 182 ));

		/*** UNUSED */
		generalNameToSRBID.put( DATA_ACCS_USER_ID, new Integer( 183 ));


//SRB 2.0**************************************************************
		/* recursive flag for C_TICKET*/
		generalNameToSRBID.put( TICKET_RECURSIVE, new Integer( 200 ));

		/*  */
		generalNameToSRBID.put( TAPE_NUMBER, new Integer( 213 ));

		/*  */
		generalNameToSRBID.put( TAPE_OWNER, new Integer( 214 ));

		/*  */
		generalNameToSRBID.put( TAPE_TYPE_VAL, new Integer( 215 ));

		/*  */
		generalNameToSRBID.put( TAPE_LIBINX, new Integer( 216 ));

		/*  */
		generalNameToSRBID.put( TAPE_FULLFLAG, new Integer( 217 ));

		/*  */
		generalNameToSRBID.put( TAPE_CURRENT_FILE_SEQNUM, new Integer( 218 ));

		/*  */
		generalNameToSRBID.put( TAPE_CURRENT_ABS_POSITION, new Integer( 219 ));

		/*  */
		generalNameToSRBID.put( TAPE_BYTES_WRITTEN, new Integer( 220 ));

		/* dataset version number */
		generalNameToSRBID.put( FILE_VER_NUM, new Integer( 221 ));

		/* dataset lock */
		generalNameToSRBID.put( FILE_LOCK_NUM, new Integer( 222 ));


		generalNameToSRBID.put( FILE_LOCK_DESCRIPTION, new Integer( 223 ));

		/* dataset lock owner name*/
		generalNameToSRBID.put( FILE_LOCK_OWNER_NAME, new Integer( 224 ));

		/* dataset lock owner domain */
		generalNameToSRBID.put( FILE_LOCK_OWNER_DOMAIN, new Integer( 225 ));

		/* collection owner domain */
		generalNameToSRBID.put( DIRECTORY_OWNER_DOMAIN, new Integer( 226 ));

		/* collection for ticket */
		generalNameToSRBID.put( TICKET_DIRECTORY_NAME, new Integer( 227 ));

		/* ticket collction timestamp */
		generalNameToSRBID.put( TICKET_DIRECTORY_CREATE_TIMESTAMP, new Integer( 228 ));

		/* ticket collction comments */
		generalNameToSRBID.put( TICKET_DIRECTORY_COMMENTS, new Integer( 229 ));

		/* name of annotator */
		generalNameToSRBID.put( DIRECTORY_ANNOTATION_USERNAME, new Integer( 230 ));

		/* domain of annotator */
		generalNameToSRBID.put( DIRECTORY_ANNOTATION_USERDOMAIN, new Integer( 231 ));

		/* annotation on data */
		generalNameToSRBID.put( DIRECTORY_ANNOTATION, new Integer( 232 ));

		/* time of annotation */
		generalNameToSRBID.put( DIRECTORY_ANNOTATION_TIMESTAMP, new Integer( 233 ));

		/* some user-defined type for the annotation */
		generalNameToSRBID.put( DIRECTORY_ANNOTATION_TYPE, new Integer( 234 ));

		/* internal collection id */
		generalNameToSRBID.put( FILE_GROUP_ID, new Integer( 235 ));

		/* setting it more than 0 hides the data */
		generalNameToSRBID.put( FILE_HIDE, new Integer( 236 ));

		/* check sum for data 400 character string */
		generalNameToSRBID.put( FILE_CHECKSUM, new Integer( 237 ));

		/* flag to see whether the data needs to be audited */
		generalNameToSRBID.put( FILE_AUDITFLAG, new Integer( 238 ));

		/* lock expory dtimestamp */
		generalNameToSRBID.put( FILE_LOCK_EXPIRY, new Integer( 239 ));

		/* data replica pinned from being moved if set greater than 0 */
		generalNameToSRBID.put( FILE_PIN_VAL, new Integer( 240 ));

		/* owner of the pin */
		generalNameToSRBID.put( FILE_PIN_OWNER_NAME, new Integer( 241 ));

		/* domain of the pin owner */
		generalNameToSRBID.put( FILE_PIN_OWNER_DOMAIN, new Integer( 242 ));

		/* expiry timestamp for the pin */
		generalNameToSRBID.put( FILE_PIN_EXPIRY, new Integer( 243 ));

		/* expiry timestamp for the data replica itself  */
		generalNameToSRBID.put( FILE_EXPIRY_DATE, new Integer( 244 ));

		/* if data is compressed tell here how? */
		generalNameToSRBID.put( FILE_IS_COMPRESSED, new Integer( 245 ));

		/* if data is encrypted tell here how? not the password */
		generalNameToSRBID.put( FILE_IS_ENCRYPTED, new Integer( 246 ));

		/* another expiry time_stamp for data as per george req */
		generalNameToSRBID.put( FILE_EXPIRE_DATE_2, new Integer( 247 ));


		generalNameToSRBID.put( NEW_AUDIT_ACTION_DESCRIPTION, new Integer( 248 ));


		generalNameToSRBID.put( NEW_AUDIT_TIMESTAMP, new Integer( 249 ));


		generalNameToSRBID.put( NEW_AUDIT_COMMENTS, new Integer( 250 ));


		//definable resource********************************************************
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_RESOURCES, new Integer( -4 ));

		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_RESOURCES0, new Integer( 251 ));
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_RESOURCES1, new Integer( 252 ));
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_RESOURCES2, new Integer( 253 ));
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_RESOURCES3, new Integer( 254 ));
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_RESOURCES4, new Integer( 255 ));
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_RESOURCES5, new Integer( 256 ));
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_RESOURCES6, new Integer( 257 ));
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_RESOURCES7, new Integer( 258 ));
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_RESOURCES8, new Integer( 259 ));
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_RESOURCES9, new Integer( 260 ));
		generalNameToSRBID.put( DEFINABLE_INTEGER_METADATA_RESOURCE0, new Integer( 261 ));

		/* user-def str metadata for rsrc*/
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_RESOURCES0_0, new Integer( 251 ));
		/* user-def str metadata for rsrc*/
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_RESOURCES0_1, new Integer( 201 ));
		/* user-def str metadata for rsrc*/
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_RESOURCES0_2, new Integer( 202 ));
		/* user-def str metadata for rsrc*/
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_RESOURCES0_3, new Integer( 203 ));
		/* user-def str metadata for rsrc*/
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_RESOURCES1_0, new Integer( 252 ));
		/* user-def str metadata for rsrc*/
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_RESOURCES1_1, new Integer( 204 ));
		/* user-def str metadata for rsrc*/
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_RESOURCES1_2, new Integer( 205 ));
		/* user-def str metadata for rsrc*/
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_RESOURCES1_3, new Integer( 206 ));

		generalNameToSRBID.put( METADATA_NUM_RESOURCE, new Integer( 262 ));
		//end definable resource*****************************************************

		generalNameToSRBID.put( METADATA_NUM_USER, new Integer( 263 ));


		generalNameToSRBID.put( COMPOUND_RESOURCE_NAME, new Integer( 264 ));


		generalNameToSRBID.put( FILE_SEG_NUM, new Integer( 265 ));


		generalNameToSRBID.put( INT_REPLICATION_ENUM, new Integer( 266 ));


		generalNameToSRBID.put( INT_SEG_NUM, new Integer( 267 ));


		generalNameToSRBID.put( INT_PATH_NAME, new Integer( 268 ));


		generalNameToSRBID.put( INT_RESOURCE_NAME, new Integer( 269 ));


		generalNameToSRBID.put( INT_RESOURCE_ADDRESS_NETPREFIX, new Integer( 270 ));


		generalNameToSRBID.put( INT_RESOURCE_TYPE_NAME, new Integer( 271 ));


		generalNameToSRBID.put( INT_IS_DIRTY, new Integer( 272 ));


		generalNameToSRBID.put( INT_RESOURCE_CLASS, new Integer( 273 ));


		generalNameToSRBID.put( INT_OFFSET, new Integer( 274 ));


		generalNameToSRBID.put( INT_SIZE, new Integer( 275 ));


		generalNameToSRBID.put( COMP_OBJ_USER_NAME, new Integer( 276 ));


		generalNameToSRBID.put( COMP_OBJ_USER_DOMAIN, new Integer( 277 ));


		generalNameToSRBID.put( INT_PATH_NAME_FOR_REPL, new Integer( 278 ));

		/*** fake ****/
		generalNameToSRBID.put( DIRECTORY_LINK_NUM, new Integer( 279 ));

		/*** fake ****/
		generalNameToSRBID.put( REAL_DIRECTORY_NAME, new Integer( 280 ));

		/*** fake ****/
		generalNameToSRBID.put( FILE_CONTAINER_NAME, new Integer( 281 ));

		/*** fake ****/
		generalNameToSRBID.put( DIRECTORY_CONTAINER_NAME, new Integer( 282 ));

		//definable user********************************************************
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_USERS, new Integer( -3 ));

		/* user-defined string metadata for user */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_USERS0, new Integer( 283 ));
		/* USER EXTRA 25*/
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_USERS1, new Integer( 284 ));
		/* USER EXTRA 26*/
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_USERS2, new Integer( 285 ));
		/* USER EXTRA 27*/
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_USERS3, new Integer( 286 ));
		/* USER EXTRA 28*/
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_USERS4, new Integer( 287 ));
		/* USER EXTRA 29*/
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_USERS5, new Integer( 288 ));
		/* USER EXTRA 30*/
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_USERS6, new Integer( 289 ));
		/* USER EXTRA 31*/
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_USERS7, new Integer( 290 ));
		/* USER EXTRA 32*/
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_USERS8, new Integer( 291 ));
		/* change MDAS_USER_STRING_METADATA */
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_USERS9, new Integer( 292 ));

		/* user-defined integer metadata  */
		generalNameToSRBID.put( UDIMD_USER0, new Integer( 293 ));

		/* user-def str metadata for user*/
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_USERS0_0, new Integer( 283 ));
		/* user-def str metadata for user*/
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_USERS0_1, new Integer( 207 ));
		/* user-def str metadata for user*/
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_USERS0_2, new Integer( 208 ));
		/* user-def str metadata for user*/
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_USERS0_3, new Integer( 209 ));
		/* user-def str metadata for user*/
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_USERS1_0, new Integer( 284 ));
		/* user-def str metadata for user*/
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_USERS1_1, new Integer( 210 ));
		/* user-def str metadata for user*/
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_USERS1_2, new Integer( 211 ));
		/* user-def str metadata for user*/
		generalNameToSRBID.put( DEFINABLE_METADATA_FOR_USERS1_3, new Integer( 212 ));
		//end definable*********************************************************

		/* SET IF YOU WANT TO DIABLE DISTINCT OPTION IN SQL QUERY */
		generalNameToSRBID.put( NONDISTINCT, new Integer( 294 ));

		/* UDF FUNCTION */
		generalNameToSRBID.put( USERDEFFUNC01, new Integer( 295 ));

		/* UDF FUNCTION */
		generalNameToSRBID.put( USERDEFFUNC02, new Integer( 296 ));

		/* UDF FUNCTION */
		generalNameToSRBID.put( USERDEFFUNC03, new Integer( 297 ));

		/* UDF FUNCTION */
		generalNameToSRBID.put( USERDEFFUNC04, new Integer( 298 ));

		/* UDF FUNCTION */
		generalNameToSRBID.put( USERDEFFUNC05, new Integer( 299 ));



//SRB 3.0*****************************************************
	/** Name of MCAT Zone (char*)*/
		generalNameToSRBID.put( ZONE_NAME, new Integer( 300 ));

	/** set to 1 for local zone  0 o/w*/
		generalNameToSRBID.put( ZONE_LOCALITY, new Integer( 301 ));

	/** address where (remote) MCAT is */
		generalNameToSRBID.put( ZONE_NETPREFIX, new Integer( 302 ));

	/** port num to reach (remote) MCAT*/
		generalNameToSRBID.put( ZONE_PORT_NUM, new Integer( 303 ));

	/** admin's auth scheme of rem MCAT*/
		generalNameToSRBID.put( ZONE_ADMIN_AUTH_SCHEME_NAME, new Integer( 304 ));

	/** DN str  of rem MCAT */
		generalNameToSRBID.put( ZONE_ADMIN_DISTIN_NAME, new Integer( 305 ));

	/** 1 for a valid zone */
		generalNameToSRBID.put( ZONE_STATUS, new Integer( 306 ));

	/** when the zone was created */
		generalNameToSRBID.put( ZONE_CREATE_DATE, new Integer( 307 ));

	/** when the zone was modified */
		generalNameToSRBID.put( ZONE_MODIFY_DATE, new Integer( 308 ));

	/** any comments about the zone */
		generalNameToSRBID.put( ZONE_COMMENTS, new Integer( 309 ));

	/** who is in charge of the zone */
		generalNameToSRBID.put( ZONE_CONTACT, new Integer( 310 ));

	/** zone admin's username */
		generalNameToSRBID.put( ZONE_ADMIN_NAME, new Integer( 311 ));

	/** zone admin's user domain name */
		generalNameToSRBID.put( ZONE_ADMIN_DOMAIN_NAME, new Integer( 312 ));

	/** resource access privilege */
		generalNameToSRBID.put( RSRC_ACCESS_PRIVILEGE, new Integer( 313 ));

	/** resource access constraint namw*/
		generalNameToSRBID.put( RSRC_ACCESS_CONSTRAINT, new Integer( 314 ));

	/**change MAX_USER_INTEGER_METADATA*/
		generalNameToSRBID.put( UDIMD_USER1, new Integer( 315 ));

	/**change MAX_RSRC_INTEGER_METADATA*/
		generalNameToSRBID.put( UDIMD_RSRC1, new Integer( 316 ));

	/** group user info modify date */
		generalNameToSRBID.put( USER_GROUP_MODIFY_DATE, new Integer( 317 ));

	/** group user zone name */
		generalNameToSRBID.put( USER_GROUP_ZONE_NAME, new Integer( 318 ));

	/** group user domain name */
		generalNameToSRBID.put( USER_GROUP_DOMAIN_DESC, new Integer( 319 ));

	/** user-def metadata for data mod */
		generalNameToSRBID.put( DATA_UDEF_MDATA_MODIFY_DATE, new Integer( 320 ));

	/** user-def metadata for coll mod */
		generalNameToSRBID.put( COLL_UDEF_MDATA_MODIFY_DATE, new Integer( 321 ));

	/** user-def metadata for user mod */
		generalNameToSRBID.put( USER_UDEF_MDATA_MODIFY_DATE, new Integer( 322 ));

	/** user-def metadata for rsrc mod */
		generalNameToSRBID.put( RSRC_UDEF_MDATA_MODIFY_DATE, new Integer( 323 ));

	/** mime string for data type */
		generalNameToSRBID.put( DATA_TYPE_MIME_STRING, new Integer( 324 ));

	/** extender list for data type */
		generalNameToSRBID.put( DATA_TYPE_EXTENDERS, new Integer( 325 ));

	/** comments on log resource */
		generalNameToSRBID.put( RSRC_COMMENTS, new Integer( 326 ));

	/** log rsource creation timestamp */
		generalNameToSRBID.put( RSRC_CREATE_DATE, new Integer( 327 ));

	/** log resource modify timestamp */
		generalNameToSRBID.put( RSRC_MODIFY_DATE, new Integer( 328 ));

	/** user creation timestamp */
		generalNameToSRBID.put( USER_CREATE_DATE, new Integer( 329 ));

	/** user modify timestamp */
		generalNameToSRBID.put( USER_MODIFY_DATE, new Integer( 330 ));

	/** log resource max obj size */
		generalNameToSRBID.put( RSRC_MAX_OBJ_SIZE, new Integer( 331 ));

	/** owner of the log resource */
		generalNameToSRBID.put( RSRC_OWNER_NAME, new Integer( 332 ));

	/** domain of the log resource */
		generalNameToSRBID.put( RSRC_OWNER_DOMAIN, new Integer( 333 ));

	/** max latency of rsrc in millisec*/
		generalNameToSRBID.put( RSRC_MLSEC_LATENCY_MAX, new Integer( 334 ));

	/** min latency of rsrc in millisec*/
		generalNameToSRBID.put( RSRC_MLSEC_LATENCY_MIN, new Integer( 335 ));

	/** bandwidth of rsrc in mega bps */
		generalNameToSRBID.put( RSRC_MBPS_BANDWIDTH, new Integer( 336 ));

	/** max concurrency allowed in rsrc*/
		generalNameToSRBID.put( RSRC_CONCURRENCY_MAX, new Integer( 337 ));

	/** num of staging levels in rsrc */
		generalNameToSRBID.put( RSRC_NUM_OF_HIERARCHIES, new Integer( 338 ));

	/** num of striping done in rsrc */
		generalNameToSRBID.put( RSRC_NUM_OF_STRIPES, new Integer( 339 ));

	/** capacity of rsrc in megabytes */
		generalNameToSRBID.put( RSRC_MEGAB_CAPACITY, new Integer( 340 ));

	/** user name used for rsrc access */
		generalNameToSRBID.put( RSRC_ACCS_USER_NAME, new Integer( 341 ));

	/** user domn used for rsrc access */
		generalNameToSRBID.put( RSRC_ACCS_USER_DOMAIN, new Integer( 342 ));

	/** user zone used for rsrc access */
		generalNameToSRBID.put( RSRC_ACCS_USER_ZONE, new Integer( 343 ));

	/** zone for resource owner */
		generalNameToSRBID.put( RSRC_OWNER_ZONE, new Integer( 344 ));

	/** zone for data owner */
		generalNameToSRBID.put( DATA_OWNER_ZONE, new Integer( 345 ));

	/** zone for data ticket owner */
		generalNameToSRBID.put( TICKET_OWNER_ZONE_D, new Integer( 346 ));

	/** zone for data ticket user */
		generalNameToSRBID.put( TICKET_USER_ZONE_D, new Integer( 347 ));

	/** zone for coll ticket owner */
		generalNameToSRBID.put( TICKET_OWNER_ZONE_C, new Integer( 348 ));

	/** zone for coll ticket user */
		generalNameToSRBID.put( TICKET_USER_ZONE_C, new Integer( 349 ));

	/** zone for data annotator */
		generalNameToSRBID.put( DATA_ANNOTATION_USER_ZONE, new Integer( 350 ));

	/** zone for audited suer */
		generalNameToSRBID.put( AUDIT_USER_ZONE, new Integer( 351 ));

	/** zone for collection owner */
		generalNameToSRBID.put( COLL_OWNER_ZONE, new Integer( 352 ));

	/** zone for data lock owner */
		generalNameToSRBID.put( DATA_LOCK_OWNER_ZONE, new Integer( 353 ));

	/** zone for coll annotator */
		generalNameToSRBID.put( COLL_ANNOTATION_USER_ZONE, new Integer( 354 ));

	/** zone for data pin owner  */
		generalNameToSRBID.put( DATA_PIN_OWNER_ZONE, new Integer( 355 ));

	/** zone for composite obj owner */
		generalNameToSRBID.put( COMP_OBJ_USER_ZONE, new Integer( 356 ));

	/** parent of a location */
		generalNameToSRBID.put( PARENT_SERVER_LOCATION, new Integer( 357 ));

	/** access id number for rsrc access */
		generalNameToSRBID.put( RSRC_ACCESS_ID, new Integer( 358 ));

	/** parent of a data type */
		generalNameToSRBID.put( PARENT_DATA_TYPE, new Integer( 359 ));

	/** location desc for zone */
		generalNameToSRBID.put( ZONE_LOCN_DESC, new Integer( 360 ));

	/** parent of a domain */
		generalNameToSRBID.put( PARENT_DOMAIN_DESC, new Integer( 361 ));

	/** parent of a user type */
		generalNameToSRBID.put( PARENT_USER_TYPE, new Integer( 362 ));

	/** parent of a rsrc type  */
		generalNameToSRBID.put( PARENT_RSRC_TYPE, new Integer( 363 ));

	/** group user who has access to rsrc*/
		generalNameToSRBID.put( RSRC_ACCS_GRPUSER_NAME, new Integer( 364 ));

	/** rsrc group user domain */
		generalNameToSRBID.put( RSRC_ACCS_GRPUSER_DOMAIN, new Integer( 365 ));

	/** rsrc group user zone */
		generalNameToSRBID.put( RSRC_ACCS_GRPUSER_ZONE, new Integer( 366 ));

	/** collection modify_timestamp */
		generalNameToSRBID.put( COLL_MODIFY_TIMESTAMP, new Integer( 367 ));

	/** group user who has access to data*/
		generalNameToSRBID.put( DATA_ACCS_GRPUSER_NAME, new Integer( 368 ));

	/** data group user domain */
		generalNameToSRBID.put( DATA_ACCS_GRPUSER_DOMAIN, new Integer( 369 ));

	/** data group user zone */
		generalNameToSRBID.put( DATA_ACCS_GRPUSER_ZONE, new Integer( 370 ));

	/** group user who has access to coll*/
		generalNameToSRBID.put( COLL_ACCS_GRPUSER_NAME, new Integer( 371 ));

	/** coll group user domain */
		generalNameToSRBID.put( COLL_ACCS_GRPUSER_DOMAIN, new Integer( 372 ));

	/** coll group user zone */
		generalNameToSRBID.put( COLL_ACCS_GRPUSER_ZONE, new Integer( 373 ));

	/** coll name for finding coll access*/
		generalNameToSRBID.put( COLL_ACCS_COLLECTION_NAME, new Integer( 374 ));

	/** accs cons for finding coll access*/
		generalNameToSRBID.put( COLL_ACCS_ACCESS_CONSTRAINT, new Integer( 375 ));

	/** data type for container associated with collection */
		generalNameToSRBID.put( DATA_TYPE_FOR_CONTAINER_FOR_COLLECTION, new Integer( 376 ));

	/** DATA_UDEF_MDATA_CREATE_DATE */
		generalNameToSRBID.put( DATA_UDEF_MDATA_CREATE_DATE, new Integer( 377 ));

	/** USER EXTRA */
		generalNameToSRBID.put( COLL_UDEF_MDATA_CREATE_DATE, new Integer( 378 ));

	/** USER EXTRA */
		generalNameToSRBID.put( RSRC_UDEF_MDATA_CREATE_DATE, new Integer( 379 ));

	/** USER EXTRA */
		generalNameToSRBID.put( USER_UDEF_MDATA_CREATE_DATE, new Integer( 380 ));


	/** container id */
		generalNameToSRBID.put( CONTAINER_ID, new Integer( 381 ));

	/** guid */
		generalNameToSRBID.put( GUID, new Integer( 382 ));

	/** guid flag */
		generalNameToSRBID.put( GUID_FLAG, new Integer( 383 ));

	/** guid timestamp */
		generalNameToSRBID.put( GUID_TIME_STAMP, new Integer( 384 ));

	/** return the query values sorted according to another attribute */
		generalNameToSRBID.put( ORDERBY, new Integer( 385 ));


//SRB3.1
	/** data_id of deleted data object */
		generalNameToSRBID.put( DELETE_FILE_ID, new Integer( 386 ));

	/** repl_enum of deleted data object */
		generalNameToSRBID.put( DELETE_FILE_REPLICATION_ENUM, new Integer( 387 ));

	/** data_name of deleted data object */
		generalNameToSRBID.put( DELETE_FILE_NAME, new Integer( 388 ));

	/** timestamp of deleted data object */
		generalNameToSRBID.put( DELETE_MODIFICATION_DATE, new Integer( 389 ));

	/** collection_name of deleted data object */
		generalNameToSRBID.put( DELETE_DIRECTORY_NAME, new Integer( 390 ));


//*********************************************************************
// The static variables defined above, used to translated the SRBFieldName
// into the api fieldName for interpreting query results, are also be used
// to relate the API fieldName to the SRB ID integer.
//
// For example,
// create a new selection, MetaDataSet.newSelection( "FILE_NAME" );
// and FILE_NAME= "FILE_NAME";
// so the API gets the ID from the hashmap, FILE_NAME= 2;
// then the SRB Server is asked for metadata value 2.
// The server returns a attribute name string "data_name"
// and the value string "foo".
//*********************************************************************
	}

//----------------------------------------------------------------------
//	Constructor
//----------------------------------------------------------------------
	SRBMetaDataSet ( SRBProtocol protocol )
	{
		super();

		this.protocol = protocol;
	}
static SRBProtocol protocol;

//TODO static? I hope so.
	static {

		//register all MetaDataGroup subclasses

		//Collection
		MetaDataGroup group = new MetaDataGroup( GROUP_DIRECTORY, "Core meta-information about directories" );
		group.add( new MetaDataField(
			PARENT_DIRECTORY_NAME, "name of parent directory (15)", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DIRECTORY_NAME, "directory name", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			FILE_GROUP_ID, "internal directory id", MetaDataField.INT, protocol ) );
		group.add( new MetaDataField(
			DIRECTORY_OWNER_NAME, "directory owner", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DIRECTORY_OWNER_DOMAIN, "directory owner domain", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DIRECTORY_CREATE_TIMESTAMP, "directory create timestamp", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			CONTAINER_FOR_DIRECTORY, "default container for coll", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DIRECTORY_COMMENTS, "comments on directory", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			COLL_ACCS_ID, "directory access id", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			COLL_ACCS_USER_ID, "directory access user id", MetaDataField.STRING, protocol ) );
		add( group );

//TODO not all are .STRING


		//Data
		group = new MetaDataGroup( GROUP_DATA, "Core meta-information about datasets." );
		group.add( new MetaDataField(
			DIRECTORY_NAME, "collection name", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			CONTAINER_NAME, "name of container", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			FILE_NAME, "file name", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			FILE_ID, "internal data id", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			FILE_REPLICATION_ENUM, "replica copy number", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			FILE_VER_NUM, "dataset version number", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			FILE_SEG_NUM, "dataset segment number", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			PHYSICAL_RESOURCE_NAME, "physical resource name", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			PATH_NAME, "physical path name of data object", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			SIZE, "size of data", MetaDataField.INT, protocol ) );
		group.add( new MetaDataField(
			OFFSET, "position of data in container", MetaDataField.INT, protocol ) );
		group.add( new MetaDataField(
			FILE_TYPE_NAME, "data type", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			FILE_CLASS_NAME, "classifcation name for data", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			FILE_CLASS_TYPE, "classification type", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			CREATION_DATE, "data creation time stamp", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			MODIFICATION_DATE, "data modification time stamp", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			FILE_LAST_ACCESS_TIMESTAMP, "last access time stamp", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			OWNER, "data creator name", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			OWNER_DOMAIN, "domain of data creator", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			FILE_COMMENTS, "comments on data", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			FILE_IS_DELETED, "data liveness", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			IS_DIRTY, "data has changed compared to other copies", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			FILE_LOCK_NUM, "dataset lock type in numeric form", MetaDataField.INT, protocol ) );
		group.add( new MetaDataField(
			FILE_LOCK_DESCRIPTION, "lock type description", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			FILE_LOCK_OWNER_NAME, "dataset lock owner name", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			FILE_LOCK_OWNER_DOMAIN, "dataset lock owner domain", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			FILE_HIDE, "setting it more than 0 hides the data", MetaDataField.INT, protocol ) );
		group.add( new MetaDataField(
			FILE_CHECKSUM, "check sum for data string", MetaDataField.INT, protocol ) );
		group.add( new MetaDataField(
			FILE_AUDITFLAG, "flag set if data has to be audited", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			FILE_LOCK_EXPIRY, "lock expory data timestamp", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			FILE_PIN_VAL, "data replica pinned from moved if>=0", MetaDataField.INT, protocol ) );
		group.add( new MetaDataField(
			FILE_PIN_OWNER_NAME, "owner of the pin", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			FILE_PIN_OWNER_DOMAIN, "domain of the pin owner", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			FILE_PIN_EXPIRY, "expiry timestamp for the pin", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			FILE_EXPIRY_DATE, "expiry time for the data replica", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			FILE_IS_COMPRESSED, "data compressed or not and how", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			FILE_IS_ENCRYPTED, "data encrypted or not and how", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			FILE_EXPIRE_DATE_2, "another expiry timestamp", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DATA_ACCS_ID, "file access id", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DATA_ACCS_USER_ID, "file access user id", MetaDataField.STRING, protocol ) );
		add( group );




		//User
		group = new MetaDataGroup( GROUP_USER, "Core information about SRB-registered users." );
		group.add( new MetaDataField(
			USER_NAME, "user name", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			USER_DOMAIN, "user domain name", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			USER_ID, "MCAT's internal Id for the User", MetaDataField.INT, protocol ) );
		group.add( new MetaDataField(
			USER_TYPE_NAME, "user type", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			USER_ADDRESS, "user address", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			USER_PHONE, "user phone number", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			USER_EMAIL, "user email", MetaDataField.STRING, protocol ) );
		add( group );




		//Physical resource
		group = new MetaDataGroup( GROUP_PHYSICAL_RESOURCE, "Core meta-information about physical resources including SRB servers." );
		group.add( new MetaDataField(
			PHYSICAL_RESOURCE_NAME, "physical resource name", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			PHYSICAL_RESOURCE_TYPE_NAME, "physical resource type", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			PHYSICAL_RESOURCE_DEFAULT_PATH, "default path in physical resource", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			LOCATION_NAME, "location of physical resource name", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			RESOURCE_ADDRESS_NETPREFIX, "net address of physical resource", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			RESOURCE_DESCRIPTION, "comments on the resource", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			MAX_OBJ_SIZE, "max size of data allowed in resource", MetaDataField.INT, protocol ) );
		group.add( new MetaDataField(
			RESOURCE_MAX_LATENCY, "physical resource estimated latency (max)", MetaDataField.INT, protocol ) );
		group.add( new MetaDataField(
			RESOURCE_MIN_LATENCY, "physical resource estimated latency (min)", MetaDataField.INT, protocol ) );
		group.add( new MetaDataField(
			RESOURCE_BANDWIDTH, "physical resource estimated bandwidth", MetaDataField.INT, protocol ) );
		group.add( new MetaDataField(
			RESOURCE_MAX_CONCURRENCY, "physical resource max concurrent reqs", MetaDataField.INT, protocol ) );
		group.add( new MetaDataField(
			RESOURCE_NUM_OF_HIERARCHIES, "depth of hierarchy in resource", MetaDataField.INT, protocol ) );
		group.add( new MetaDataField(
			RESOURCE_NUM_OF_STRIPES, "number of striping in resource", MetaDataField.INT, protocol ) );
		group.add( new MetaDataField(
			RESOURCE_CAPACITY, "capacity of the physical resource", MetaDataField.INT, protocol ) );
		add( group );




		//Logical resource
		group = new MetaDataGroup( GROUP_LOGICAL_RESOURCE, "Core meta-information about logical resources." );
		group.add( new MetaDataField(
			RESOURCE_NAME, "name of logical resource", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			RESOURCE_TYPE_NAME, "logical resource type", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			PHYSICAL_RESOURCE_NAME, "physical resource name", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			RESOURCE_REPLICATION_ENUM, "index of physical resource in logical resource", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			RESOURCE_DEFAULT_PATH, "default path in logical resource", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			RESOURCE_CLASS, "classification of resource", MetaDataField.STRING, protocol ) );
		add( group );




		//Server
		group = new MetaDataGroup( GROUP_SERVER, "" );
		group.add( new MetaDataField(
			SERVER_LOCATION, "location of SRB server", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			SERVER_NETPREFIX, "net address of SRB server", MetaDataField.STRING, protocol ) );
		add( group );




		//User group
		group = new MetaDataGroup( GROUP_USER_GROUP, "" );
		group.add( new MetaDataField(
			USER_GROUP_NAME, "name of user group", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			USER_NAME, "user name", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			USER_DOMAIN, "user domain name", MetaDataField.STRING, protocol ) );
		add( group );




		//Authentication
		group = new MetaDataGroup( GROUP_AUTHENTICATION, "" );
		group.add( new MetaDataField(
			USER_NAME, "user name", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			USER_DOMAIN, "user domain name", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			USER_DISTINGUISHED_NAME, "distinguished name of user", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			USER_AUTHENTICATION_SCHEME, "user authentication scheme (67)", MetaDataField.STRING, protocol ) );
		add( group );




		//Authorization
		group = new MetaDataGroup( GROUP_AUTHORIZATION, "" );
		group.add( new MetaDataField(
			USER_NAME, "user name", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			USER_DOMAIN, "user domain name", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			TICKET_D, "identifier for ticket given for data", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			TICKET_C, "identifier for ticket given for coll", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DIRECTORY_NAME, "collection name", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			FILE_NAME, "data name", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			PHYSICAL_RESOURCE_NAME, "physical resource name", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			ACCESS_CONSTRAINT, "access restriction on data", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DIRECTORY_ACCESS_CONSTRAINT, "access on directory ACCESS_DIRECTORY_NAME", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			ACCESS_DIRECTORY_NAME, "directory name for access in DIRECTORY_ACCESS_CONSTRAINT", MetaDataField.STRING, protocol ) );
		add( group );





		//Auditing
		group = new MetaDataGroup( GROUP_AUDIT, "Audit information on users and on datasets." );
		group.add( new MetaDataField(
			AUDIT_USER, "audited user name", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			AUDIT_USER_DOMAIN, "audited user domain", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			AUDIT_ACTION_DESCRIPTION, "audit action description", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			NEW_AUDIT_ACTION_DESCRIPTION, "new audit action description", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			NEW_AUDIT_TIMESTAMP, "audit timestamp", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			NEW_AUDIT_COMMENTS, "audit comments", MetaDataField.STRING, protocol ) );
		add( group );




		//Ticket
		group = new MetaDataGroup( GROUP_TICKET, "Information about ticket-based access control for datasets, collections as well as recursively under a collection." );
		group.add( new MetaDataField(
			TICKET_D, "identifier for ticket given for data", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DIRECTORY_NAME, "collection name", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			FILE_NAME, "data name", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			TICKET_USER_D, "allowed ticket user or user group", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			TICKET_USER_DOMAIN_D, "data ticket user domain", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			TICKET_OWNER_D, "data ticket creator", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			TICKET_OWNER_DOMAIN_D, "data ticket creator domain", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			TICKET_BEGIN_TIME_D, "data ticket validity start time", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			TICKET_END_TIME_D, "data ticket validity endtime", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			TICKET_ACC_COUNT_D, "number of opens allowed on data ticket", MetaDataField.INT, protocol ) );
		group.add( new MetaDataField(
			TICKET_ACC_LIST_D, "access allowed on data ticket", MetaDataField.STRING, protocol ) );

		group.add( new MetaDataField(
			TICKET_C, "identifier for ticket given for collection", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			TICKET_DIRECTORY_NAME, "collection for ticket", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			TICKET_RECURSIVE, "recursive flag for C_TICKET", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			TICKET_USER_C, "allowed collection ticket user", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			TICKET_USER_DOMAIN_C, "collection ticket user domain", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			TICKET_OWNER_C, "collection ticket creator", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			TICKET_OWNER_DOMAIN_C, "collection ticket creator domain", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			TICKET_BEGIN_TIME_C, "collection ticket validity start time", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			TICKET_END_TIME_C, "collection ticket validity end time", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			TICKET_ACC_COUNT_C, "number of opens allowed on collection ticket", MetaDataField.INT, protocol ) );
		group.add( new MetaDataField(
			TICKET_ACC_LIST_C, "access allowed on collection ticket", MetaDataField.STRING, protocol ) );

		group.add( new MetaDataField(
			TICKET_OWNER_ZONE_D, "zone for data ticket owner", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			TICKET_USER_ZONE_D, "zone for data ticket user", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			TICKET_OWNER_ZONE_C, "zone for coll ticket owner", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			TICKET_USER_ZONE_C, "zone for coll ticket user", MetaDataField.STRING, protocol ) );
		add( group );





		//Container
		group = new MetaDataGroup( GROUP_CONTAINER, "Core meta-information about containers" );
		group.add( new MetaDataField(
			CONTAINER_NAME, "name of container is a dataname", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			CONTAINER_REPLICATION_ENUM, "container copy number", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			CONTAINER_MAX_SIZE, "maximum size of container", MetaDataField.INT, protocol ) );
		group.add( new MetaDataField(
			CONTAINER_SIZE, "current size of container", MetaDataField.INT, protocol ) );
		group.add( new MetaDataField(
			CONTAINER_LOG_RESOURCE_NAME, "logical resource associated with container", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			CONTAINER_RESOURCE_NAME, "container resource name", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			CONTAINER_RESOURCE_CLASS, "class of physical resource of container", MetaDataField.STRING, protocol ) );
		add( group );





		//Annotator
		group = new MetaDataGroup( GROUP_ANNOTATIONS, "core meta-information on annotating datasets. see also ACCESS_CONSTRAINT attribute for access control" );
		group.add( new MetaDataField(
			DIRECTORY_NAME, "collection name", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			FILE_NAME, "data name", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			FILE_ANNOTATION_USERNAME, "name of annotator", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			FILE_ANNOTATION_USERDOMAIN, "domain of annotator", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			FILE_ANNOTATION, "annotation on data", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			FILE_ANNOTATION_TIMESTAMP, "time of annotation", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			FILE_ANNOTATION_POSITION, "location of the annotation", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DIRECTORY_ANNOTATION_USERNAME, "name of annotator", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DIRECTORY_ANNOTATION_USERDOMAIN, "domain of annotator", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DIRECTORY_ANNOTATION, "annotation on data", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DIRECTORY_ANNOTATION_TIMESTAMP, "time of annotation", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DIRECTORY_ANNOTATION_TYPE, "type for the annotation", MetaDataField.STRING, protocol ) );
		add( group );




		//Compound resource
		group = new MetaDataGroup( GROUP_COMPOUND_RESOURCE, "" );
		group.add( new MetaDataField(
			COMPOUND_RESOURCE_NAME, "compound resource name", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			TAPE_NUMBER, "compound resource tape id", MetaDataField.INT, protocol ) );
		group.add( new MetaDataField(
			TAPE_OWNER, "compound resource tape owner id", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			TAPE_TYPE_VAL, "compound resource tape type", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			TAPE_LIBINX, "compound resource libinx", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			TAPE_FULLFLAG, "compound resource tape full flag", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			TAPE_CURRENT_FILE_SEQNUM, "file index in tape", MetaDataField.INT, protocol ) );
		group.add( new MetaDataField(
			TAPE_CURRENT_ABS_POSITION, "file position in tape", MetaDataField.INT, protocol ) );
		group.add( new MetaDataField(
			TAPE_BYTES_WRITTEN, "tape used space", MetaDataField.INT, protocol ) );
		group.add( new MetaDataField(
			INT_REPLICATION_ENUM, "replica number of file in compound resource", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			INT_SEG_NUM, "segment number of file in compound resource", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			INT_PATH_NAME, "path name of file in compound resource", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			INT_IS_DIRTY, "dirty flag of file in compound resource", MetaDataField.INT, protocol ) );
		group.add( new MetaDataField(
			INT_OFFSET, "offset of file in compound resource container", MetaDataField.INT, protocol ) );
		group.add( new MetaDataField(
			INT_SIZE, "size of file in compound resource", MetaDataField.INT, protocol ) );
		group.add( new MetaDataField(
			INT_PATH_NAME_FOR_REPL, "path of file in compound resource", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			INT_RESOURCE_NAME, "resource name of file in compound resource", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			INT_RESOURCE_ADDRESS_NETPREFIX, "net address of compound resource", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			INT_RESOURCE_TYPE_NAME, "type of resource in compound resource", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			INT_RESOURCE_CLASS, "class of resource in compound resource", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			COMP_OBJ_USER_NAME, "user name of compound object", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			COMP_OBJ_USER_DOMAIN, "user domn of compound object", MetaDataField.STRING, protocol ) );
		add( group );






		//Dublin Core
		group = new MetaDataGroup( GROUP_DUBLIN_CORE, "This set of metadata, even though part of MCAT core, is normally turned off in order to speed up processing. patches ned to be applied if this option needs to be used. For more information please check http://www.dublincore.org/" );
		group.add( new MetaDataField(
			DC_DATA_NAME, "DC: Data Name same as data name", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DC_COLLECTION, "DC: Collection Name same as collection name", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DC_CONTRIBUTOR_TYPE, "DC: Contributor Type:eg.Author", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DC_SUBJECT_CLASS, "DC: Subject Classification", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DC_DESCRIPTION_TYPE, "DC: Type of Description", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DC_TYPE, "DC: Type of the Object", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DC_SOURCE_TYPE, "DC: Type of the Source", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DC_LANGUAGE, "DC: Language of the Object", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DC_RELATION_TYPE, "DC: Relation with another Object", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DC_COVERAGE_TYPE, "DC: Coverage Type", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DC_RIGHTS_TYPE, "DC: Rights Type", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DC_TITLE, "DC: Title of the Object", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DC_CONTRIBUTOR_NAME, "DC: Contributor Name", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DC_SUBJECT_NAME, "DC: Subject", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DC_DESCRIPTION, "DC: Description", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DC_PUBLISHER, "DC: Publisher Name", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DC_SOURCE, "DC: Source Name", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DC_RELATED_DATA_DESCRIPTION, "DC: Related Data Description", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DC_RELATED_DATA, "DC: Date Related to (152,153)", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DC_RELATED_DIRECTORY, "DC: Collection related to (152,153)", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DC_COVERAGE, "DC: Coverage Information", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DC_RIGHTS, "DC: Rights Information", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DC_PUBLISHER_ADDR, "DC: Publisher Address", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DC_CONTRIBUTOR_ADDR, "DC: Contributor Address", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DC_CONTRIBUTOR_EMAIL, "DC: Contributor Email", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DC_CONTRIBUTOR_PHONE, "DC: Contributor Phone", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DC_CONTRIBUTOR_WEB, "DC: Contributor Web Address", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DC_CONTRIBUTOR_AFFILIATION, "DC: Contributor Affiliation", MetaDataField.STRING, protocol ) );
		add( group );






		//Zone
		group = new MetaDataGroup( GROUP_ZONE, "Name of MCAT Zone" );	/** Name of MCAT Zone (char*)*/
		group.add( new MetaDataField(
			ZONE_NAME, "Name of MCAT Zone (String)", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			ZONE_LOCALITY, "set to 1 for local zone, 0 otherwise", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			ZONE_NETPREFIX, "address where (remote) MCAT is", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			ZONE_PORT_NUM, "port num to reach (remote) MCAT", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			ZONE_ADMIN_AUTH_SCHEME_NAME, "admin's auth scheme of rem MCAT", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			ZONE_ADMIN_DISTIN_NAME, "DN str  of rem MCAT", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			ZONE_STATUS, "1 for a valid zone", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			ZONE_CREATE_DATE, "when the zone was created", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			ZONE_MODIFY_DATE, "when the zone was modified", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			ZONE_COMMENTS, "any comments about the zone", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			ZONE_CONTACT, "who is in charge of the zone", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			ZONE_ADMIN_NAME, "zone admin's username", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			ZONE_ADMIN_DOMAIN_NAME, "zone admin's user domain name", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			ZONE_LOCN_DESC, "location desc for zone", MetaDataField.STRING, protocol ) );
		add( group );



		//User-defined string metadata for data
		group = new MetaDataGroup( GROUP_UDMD,
			"The special catagory of metadata known as user definable metadata. "+
			"By using the MetaDataTable, the user can define their own "+
			"key-pair relationships. Creating in some sense their own new metadata "+
			"attributes. These definable metadata can be attached to the various "+
			"kinds of system objects, eg. files/datasets, directories/collections, "+
			"user, resources." );

		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_FILES, "definable metadata table for files", MetaDataField.TABLE, protocol ) );

		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_DIRECTORIES, "definable metadata table for directories", MetaDataField.TABLE, protocol ) );

		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_USERS, "definable metadata table for users", MetaDataField.TABLE, protocol ) );

		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_RESOURCES, "definable metadata table for resources", MetaDataField.TABLE, protocol ) );


		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_FILES0, "The first column of the definable "+
			"metadata table for files. The standard usage keeps attribute names of "+
			"the user defined metadata in this column. This field can be used in "+
			"conjuction with the metadata number, which specifies a row in the table,"+
			" to refer to a specific cell of the definable metadata table for files.",
			MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_FILES1, "The second column of the definable metadata table for users.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_FILES2, "The third column of the definable metadata table for users.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_FILES3, "The fourth column of the definable metadata table for users.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_FILES4, "The fifth column of the definable metadata table for users.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_FILES5, "The sixth column of the definable metadata table for users.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_FILES6, "The seventh column of the definable metadata table for users.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_FILES7, "The eigth column of the definable metadata table for users.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_FILES8, "The ninth column of the definable metadata table for users.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_FILES9, "The tenth column of the definable metadata table for users.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			METADATA_NUM, "Used to specify a certain row in the definable metadata "+
			"table for files.", MetaDataField.STRING, protocol ) );

		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_FILES0_0, "Used for comparator queuries of the metadata, user attribute", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_FILES0_1, "Used for comparator queuries of the metadata, user attribute", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_FILES0_2, "Used for comparator queuries of the metadata, user attribute", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_FILES0_3, "Used for comparator queuries of the metadata, user attribute", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_FILES0_4, "Used for comparator queuries of the metadata, user attribute", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_FILES1_0, "Used for comparator queuries of the metadata, user value", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_FILES1_1, "Used for comparator queuries of the metadata, user value", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_FILES1_2, "Used for comparator queuries of the metadata, user value", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_FILES1_3, "Used for comparator queuries of the metadata, user value", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_FILES1_4, "Used for comparator queuries of the metadata, user value", MetaDataField.STRING, protocol ) );
		//end definable files



		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_DIRECTORIES0, "The first column of the definable metadata table for directories.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_DIRECTORIES1, "The second column of the definable metadata table for directories.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_DIRECTORIES2, "The third column of the definable metadata table for directories.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_DIRECTORIES3, "The fourth column of the definable metadata table for directories.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_DIRECTORIES4, "The fifth column of the definable metadata table for directories.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_DIRECTORIES5, "The sixth column of the definable metadata table for directories.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_DIRECTORIES6, "The seventh column of the definable metadata table for directories.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_DIRECTORIES7, "The eigth column of the definable metadata table for directories.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_DIRECTORIES8, "The ninth column of the definable metadata table for directories.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_DIRECTORIES9, "The tenth column of the definable metadata table for directories.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			METADATA_NUM_DIRECTORY, "Used to specify a certain row in the definable metadata table for directories.", MetaDataField.STRING, protocol ) );

		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_DIRECTORIES0_0, "Used for comparator queuries of the metadata, user attribute", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_DIRECTORIES0_1, "Used for comparator queuries of the metadata, user attribute", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_DIRECTORIES0_2, "Used for comparator queuries of the metadata, user attribute", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_DIRECTORIES0_3, "Used for comparator queuries of the metadata, user attribute", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_DIRECTORIES0_4, "Used for comparator queuries of the metadata, user attribute", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_DIRECTORIES1_0, "Used for comparator queuries of the metadata, user value", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_DIRECTORIES1_1, "Used for comparator queuries of the metadata, user value", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_DIRECTORIES1_2, "Used for comparator queuries of the metadata, user value", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_DIRECTORIES1_3, "Used for comparator queuries of the metadata, user value", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_DIRECTORIES1_4, "Used for comparator queuries of the metadata, user value", MetaDataField.STRING, protocol ) );
		//end definable directories

		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_RESOURCES0, "The first column of the definable metadata table for resources.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_RESOURCES1, "The second column of the definable metadata table for resources.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_RESOURCES2, "The third column of the definable metadata table for resources.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_RESOURCES3, "The fourth column of the definable metadata table for resources.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_RESOURCES4, "The fifth column of the definable metadata table for resources.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_RESOURCES5, "The sixth column of the definable metadata table for resources.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_RESOURCES6, "The seventh column of the definable metadata table for resources.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_RESOURCES7, "The eigth column of the definable metadata table for resources.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_RESOURCES8, "The ninth column of the definable metadata table for resources.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_RESOURCES9, "The tenth column of the definable metadata table for resources.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			METADATA_NUM_RESOURCE, "Used to specify a certain row in the definable metadata table for resources.", MetaDataField.STRING, protocol ) );

		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_RESOURCES0_0, "Used for comparator queuries of the metadata, user attribute", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_RESOURCES0_1, "Used for comparator queuries of the metadata, user attribute", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_RESOURCES0_2, "Used for comparator queuries of the metadata, user attribute", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_RESOURCES0_3, "Used for comparator queuries of the metadata, user attribute", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_RESOURCES1_0, "Used for comparator queuries of the metadata, user value", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_RESOURCES1_1, "Used for comparator queuries of the metadata, user value", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_RESOURCES1_2, "Used for comparator queuries of the metadata, user value", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_RESOURCES1_3, "Used for comparator queuries of the metadata, user value", MetaDataField.STRING, protocol ) );
		//end definable resources

		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_USERS0, "The first column of the definable metadata table for users.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_USERS1, "The second column of the definable metadata table for users.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_USERS2, "The third column of the definable metadata table for users.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_USERS3, "The fourth column of the definable metadata table for users.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_USERS4, "The fifth column of the definable metadata table for users.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_USERS5, "The sixth column of the definable metadata table for users.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_USERS6, "The seventh column of the definable metadata table for users.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_USERS7, "The eigth column of the definable metadata table for users.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_USERS8, "The ninth column of the definable metadata table for users.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_USERS9, "The tenth column of the definable metadata table for users.", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			METADATA_NUM_USER, "Used to specify a certain row in the definable metadata table for users.", MetaDataField.STRING, protocol ) );

		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_USERS0_0, "Used for comparator queuries of the metadata, user attribute", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_USERS0_1, "Used for comparator queuries of the metadata, user attribute", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_USERS0_2, "Used for comparator queuries of the metadata, user attribute", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_USERS0_3, "Used for comparator queuries of the metadata, user attribute", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_USERS1_0, "Used for comparator queuries of the metadata, user value", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_USERS1_1, "Used for comparator queuries of the metadata, user value", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_USERS1_2, "Used for comparator queuries of the metadata, user value", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DEFINABLE_METADATA_FOR_USERS1_3, "Used for comparator queuries of the metadata, user value", MetaDataField.STRING, protocol ) );
		//end definable users



/*These um, I don't know
		group.add( new MetaDataField(
			NONDISTINCT, "USER EXTRA 35", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			USERDEFFUNC01, "USER EXTRA 36", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			USERDEFFUNC02, "USER EXTRA 37", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			USERDEFFUNC03, "USER EXTRA 38", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			USERDEFFUNC04, "USER EXTRA 39", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			USERDEFFUNC05, "USER EXTRA 40", MetaDataField.STRING, protocol ) );
*/
		add( group );



		group = new MetaDataGroup( GROUP_INDEX, "The user can index a dataset , datasets of given type or datasets in a collection. the index is treated as a SRB registered dataset. The user can download the index and search on it. The location can be collection-information (i.e., index is stored as several datasets inside a collection, or can be a URL!. Note that index is treated as a SRB registered dataset and hence inherits all meta information about datasets including structured metadata which can be used to store information about the index. see datacutter proxy for more information" );
		group.add( new MetaDataField(
			INDEX_NAME_FOR_FILE, "data name of index on data", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			IX_DIRECTORY_NAME_FOR_FILE, "collection name of index on data (4)", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			IX_DATATYPE_FOR_FILE, "index type for (94)", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			IX_LOCATION_FOR_FILE, "path name of index in (94)", MetaDataField.STRING, protocol ) );

		group.add( new MetaDataField(
			INDEX_NAME_FOR_DATATYPE, "data name of index on data type", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			IX_DIRECTORY_NAME_FOR_DATATYPE, "collection name of index on data type (95)", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			IX_DATATYPE_FOR_DATATYPE, "index type for (95)", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			IX_LOCATION_FOR_DATATYPE, "path name of index in (95)", MetaDataField.STRING, protocol ) );

		group.add( new MetaDataField(
			INDEX_NAME_FOR_DIRECTORY, "data name of index on collection", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			IX_DIRECTORY_NAME_FOR_DIRECTORY, "collection name of index on collection (96)", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			IX_DATATYPE_FOR_DIRECTORY, "index type for (96)", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			IX_LOCATION_FOR_DIRECTORY, "path name of index in (96)", MetaDataField.STRING, protocol ) );
		add( group );


		group = new MetaDataGroup( GROUP_STRUCTURED_METADATA, "The user can store structured (treated as a blob) metadata information." );
		group.add( new MetaDataField(
			STRUCTURED_METADATA_TYPE, "type of user-inserted structured metadata for data in (92) or (93)", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			STRUCTURED_METADATA_COMMENTS, "comments on the structured metadata in (92) or (93)", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			STRUCTURED_METADATA_FILE_NAME, "data name of user-inserted structured metadata stored as another data object inside SRB see also (112)", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			INTERNAL_STRUCTURED_METADATA, "user-inserted structured metadata stored as a string inside MCAT", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			STRUCTURED_METADATA_DIRECTORY_NAME, "collection name of user-inserted structured metadata stored as another data object inside SRB see also (92)", MetaDataField.STRING, protocol ) );
		add( group );


		//TODO what name?
		group = new MetaDataGroup( GROUP_METHOD, "users can associate methods on dataset , datasets of given type or datasets in a collection. the method is treated as a SRB registered dataset and hence inherits all meta information about datasets including structured metadata which can be used to store information about the arguments and method return values. see datacutter proxy for more information." );
		group.add( new MetaDataField(
			METHOD_NAME_FOR_FILE, "data name of method on data", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			METHOD_NAME_FOR_DATATYPE, "data name of method on data type", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			METHOD_NAME_FOR_DIRECTORY, "data name of method on collection", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			METHOD_DIRECTORY_NAME_FOR_FILE, "collection name of method on data (97)", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			METHOD_DIRECTORY_NAME_FOR_DATATYPE, "collection name of method on datatype (98)", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			METHOD_DIRECTORY_NAME_FOR_DIRECTORY, "collection name of method on collection (99)", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			METHOD_DATATYPE_FOR_FILE, "method type for (97)", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			METHOD_DATATYPE_FOR_DATATYPE, "method type for (98)", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			METHOD_DATATYPE_FOR_DIRECTORY, "method type for (99)", MetaDataField.STRING, protocol ) );
		add( group );


		group = new MetaDataGroup( GROUP_GUID, "Metadata attributes relating to GUIDs." );
		group.add( new MetaDataField(
			GUID, "guid", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			GUID_FLAG, "guid flag", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			GUID_TIME_STAMP, "guid timestamp", MetaDataField.STRING, protocol ) );
		add( group );

/*
		//Unsupported
		group = new MetaDataGroup( "Unsupported", "" );
		group.add( new MetaDataField(
			DIRECTORY_LINK_NUM, "fake", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			REAL_DIRECTORY_NAME, "fake", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			FILE_CONTAINER_NAME, "fake", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DIRECTORY_CONTAINER_NAME, "fake", MetaDataField.STRING, protocol ) );
		add( group );
*/

		group = new MetaDataGroup( GROUP_UNDEFINED, "Metadata attributes that have not yet been added to a group." ); //TODO because I don't know which group
		group.add( new MetaDataField(
			RSRC_ACCESS_PRIVILEGE, "resource access privilege", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			RSRC_ACCESS_CONSTRAINT, "resource access constraint namw", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			RSRC_COMMENTS, "comments on log resource", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			RSRC_CREATE_DATE, "log rsource creation timestamp", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			RSRC_MODIFY_DATE, "log resource modify timestamp", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			RSRC_MAX_OBJ_SIZE, "log resource max obj size", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			RSRC_OWNER_NAME, "owner of the log resource", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			RSRC_OWNER_DOMAIN, "domain of the log resource", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			RSRC_MLSEC_LATENCY_MAX, "max latency of rsrc in millisec", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			RSRC_MLSEC_LATENCY_MIN, "min latency of rsrc in millisec", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			RSRC_MBPS_BANDWIDTH, "bandwidth of rsrc in mega bps", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			RSRC_CONCURRENCY_MAX, "max concurrency allowed in rsrc", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			RSRC_NUM_OF_HIERARCHIES, "num of staging levels in rsrc", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			RSRC_NUM_OF_STRIPES, "num of striping done in rsrc", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			RSRC_MEGAB_CAPACITY, "capacity of rsrc in megabytes", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			RSRC_ACCS_USER_NAME, "user name used for rsrc access", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			RSRC_ACCS_USER_DOMAIN, "user domn used for rsrc access", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			RSRC_ACCS_USER_ZONE, "user zone used for rsrc access", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			RSRC_OWNER_ZONE, "zone for resource owner", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			RSRC_ACCS_GRPUSER_NAME, "group user who has access to rsrc", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			RSRC_ACCS_GRPUSER_DOMAIN, "rsrc group user domain", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			RSRC_ACCS_GRPUSER_ZONE, "rsrc group user zone", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			RSRC_ACCESS_ID, "access id number for rsrc access", MetaDataField.STRING, protocol ) );

		group.add( new MetaDataField(
			UDIMD_USER1, "change MAX_USER_INTEGER_METADATA", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			USER_UDEF_MDATA_MODIFY_DATE, "user-def metadata for user mod", MetaDataField.STRING, protocol ) );

		group.add( new MetaDataField(
			UDIMD_RSRC1, "change MAX_RSRC_INTEGER_METADATA", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			RSRC_UDEF_MDATA_MODIFY_DATE, "user-def metadata for rsrc mod", MetaDataField.STRING, protocol ) );

		group.add( new MetaDataField(
			USER_GROUP_MODIFY_DATE, "group user info modify date", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			USER_GROUP_ZONE_NAME, "group user zone name", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			USER_GROUP_DOMAIN_DESC, "group user domain name", MetaDataField.STRING, protocol ) );

		group.add( new MetaDataField(
			DATA_UDEF_MDATA_MODIFY_DATE, "user-def metadata for data mod", MetaDataField.STRING, protocol ) );

		group.add( new MetaDataField(
			COLL_UDEF_MDATA_MODIFY_DATE, "user-def metadata for coll mod", MetaDataField.STRING, protocol ) );


		group.add( new MetaDataField(
			DATA_TYPE_MIME_STRING, "mime string for data type", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DATA_TYPE_EXTENDERS, "extender list for data type", MetaDataField.STRING, protocol ) );

		group.add( new MetaDataField(
			USER_CREATE_DATE, "user creation timestamp", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			USER_MODIFY_DATE, "user modify timestamp", MetaDataField.STRING, protocol ) );

		group.add( new MetaDataField(
			DATA_OWNER_ZONE, "zone for data owner", MetaDataField.STRING, protocol ) );

		group.add( new MetaDataField(
			DATA_ANNOTATION_USER_ZONE, "zone for data annotator", MetaDataField.STRING, protocol ) );

		group.add( new MetaDataField(
			AUDIT_USER_ZONE, "zone for audited suer", MetaDataField.STRING, protocol ) );

		group.add( new MetaDataField(
			COLL_OWNER_ZONE, "zone for collection owner", MetaDataField.STRING, protocol ) );

		group.add( new MetaDataField(
			DATA_LOCK_OWNER_ZONE, "zone for data lock owner", MetaDataField.STRING, protocol ) );

		group.add( new MetaDataField(
			COLL_ANNOTATION_USER_ZONE, "zone for coll annotator", MetaDataField.STRING, protocol ) );

		group.add( new MetaDataField(
			DATA_PIN_OWNER_ZONE, "zone for data pin owner", MetaDataField.STRING, protocol ) );

		group.add( new MetaDataField(
			COMP_OBJ_USER_ZONE, "zone for composite obj owner", MetaDataField.STRING, protocol ) );

		group.add( new MetaDataField(
			PARENT_SERVER_LOCATION, "parent of a location", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			PARENT_DOMAIN_DESC, "parent of a domain", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			PARENT_USER_TYPE, "parent of a user type", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			PARENT_RSRC_TYPE, "parent of a rsrc type", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			PARENT_DATA_TYPE, "parent of a data type", MetaDataField.STRING, protocol ) );

		group.add( new MetaDataField(
			ZONE_LOCN_DESC, "location desc for zone", MetaDataField.STRING, protocol ) );

		group.add( new MetaDataField(
			COLL_MODIFY_TIMESTAMP, "collection modify_timestamp", MetaDataField.STRING, protocol ) );

		group.add( new MetaDataField(
			DATA_ACCS_GRPUSER_NAME, "group user who has access to data", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DATA_ACCS_GRPUSER_DOMAIN, "data group user domain", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DATA_ACCS_GRPUSER_ZONE, "data group user zone", MetaDataField.STRING, protocol ) );

		group.add( new MetaDataField(
			COLL_ACCS_GRPUSER_NAME, "group user who has access to coll", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			COLL_ACCS_GRPUSER_DOMAIN, "coll group user domain", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			COLL_ACCS_GRPUSER_ZONE, "coll group user zone", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			COLL_ACCS_COLLECTION_NAME, "coll name for finding coll access", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			COLL_ACCS_ACCESS_CONSTRAINT, "accs cons for finding coll access", MetaDataField.STRING, protocol ) );

		group.add( new MetaDataField(
			DATA_TYPE_FOR_CONTAINER_FOR_COLLECTION, "data type for container associated with collection", MetaDataField.STRING, protocol ) );

		group.add( new MetaDataField(
			DATA_UDEF_MDATA_CREATE_DATE, "DATA_UDEF_MDATA_CREATE_DATE", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			COLL_UDEF_MDATA_CREATE_DATE, "COLL_UDEF_MDATA_CREATE_DATE", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			RSRC_UDEF_MDATA_CREATE_DATE, "RSRC_UDEF_MDATA_CREATE_DATE", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			USER_UDEF_MDATA_CREATE_DATE, "USER_UDEF_MDATA_CREATE_DATE", MetaDataField.STRING, protocol ) );

		group.add( new MetaDataField(
			CONTAINER_ID, "container id", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			ORDERBY, "return the query values sorted according to another attribute. Used in the condition as a comma seperated list: ORDER_BY = FILE_TYPE_NAME, FILE_NAME", MetaDataField.STRING, protocol ) );

//SRB3.1
		group.add( new MetaDataField(
			DELETE_FILE_ID, "data_id of deleted data object", MetaDataField.INT, protocol ) );
		group.add( new MetaDataField(
			DELETE_FILE_REPLICATION_ENUM, "repl_enum of deleted data object", MetaDataField.INT, protocol ) );
		group.add( new MetaDataField(
			DELETE_FILE_NAME, "data_name of deleted data object", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DELETE_MODIFICATION_DATE, "timestamp of deleted data object", MetaDataField.STRING, protocol ) );
		group.add( new MetaDataField(
			DELETE_DIRECTORY_NAME, "collection_name of deleted data object", MetaDataField.STRING, protocol ) );

		add( group );
	}


//----------------------------------------------------------------------
//  Static Methods
//----------------------------------------------------------------------
	/**
	 * Given one of the static field names this method returns the SRB integer
	 * id for that name.
	 */
	static final int getSRBID( String fieldName )
	{
		Integer srbID = ((Integer) generalNameToSRBID.get( fieldName ));
		if (srbID == null)
			throw new NullPointerException(
				"Programming error, was this metadata value: "+fieldName+
				", given an integer value in SRBMetaDataSet.generalNameToSRBID?" );
		return srbID.intValue();
	}

	/**
	 * Given one of the static field names this method returns the SRB
	 * metadata attribute name, as a string.
	 */
//only use as of SRB 3.1 is ORDERBY
	static final String getSRBName( String fieldName )
	{
		int id = getSRBID( fieldName );

		return srbCatalogNames[id];
	}


	/**
	 * From the results of a SRB query the MetaDataField is returned.
	 */
	static final MetaDataField getGeneralMetaData( String srbTableName,
		String srbAttributeName )
	{
		MetaDataField field = null;

		for (int i=0;i<tableNames.length;i++) {
			if (srbTableName.indexOf( tableNames[i] ) >= 0) {
				if (srbAttributeName.equals( attributeNames[i] )) {
						field = getField( catalogNames[i] );
				}
			}
		}

		if (field == null) {
			throw new NullPointerException(
				"This srb return value: "+srbTableName+" : "+srbAttributeName+
				", is not supported in this version." );
		}

		return field;
	}


	/**
	 * Turns the JARGON api metadata attribute name into the
	 * SRB database table name.
	 */
	static final String getSRBTableName( String fieldName )
	{
		Integer srbID = ((Integer) generalNameToSRBID.get( fieldName ));
		if (srbID == null)
			throw new NullPointerException(
				"Programming error, was this metadata value: "+fieldName+
				", given an integer value in SRBMetaDataSet.generalNameToSRBID?" );
		return tableNames[srbID.intValue()];
	}

	/**
	 * Turns the JARGON api metadata attribute name into the
	 * SRB database attribute name.
	 */
	static final String getSRBAttributeName( String fieldName )
	{
		Integer srbID = ((Integer) generalNameToSRBID.get( fieldName ));
		if (srbID == null)
			throw new NullPointerException(
				"Programming error, was this metadata value: "+fieldName+
				", given an integer value in SRBMetaDataSet.generalNameToSRBID?" );
		return attributeNames[srbID.intValue()];
	}
}
