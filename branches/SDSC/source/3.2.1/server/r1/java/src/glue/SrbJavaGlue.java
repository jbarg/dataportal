/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

package edu.sdsc.SrbGlue;

/**
 * SrbJavaGlue
 *
 * @version 1.0 10/23/97
 * @author John Moreland
 */
public class SrbJavaGlue
{
	// JNI (Java Native Interface) Glue Routines for the SRB client library.

	public native int clConnectJ( String host, String port, String auth,
	 String user, String domain );

	public native int srbListCollectJ( int conn, int catType,
		String collectionName, String flag, int types[] );

	public native String getFileListItem( int conn, int index );

	public native int clFinishJ( int conn );

	public native int srbObjCopyNewJ( int conn, String srcDir, 
		String srcFile, String dstDir, String dstFile, String dstRes );

	public native int srbListResourceJ( int conn, int bs );

	public native String getResListItem( int conn, int index );

	public native String getDefCollection();

	public native String getDefResource();

	public native String getUser(int conn);

	public native String getDomain(int conn);

	public native String getLocalCacheDir();

	public native int srbObjReplicateJ(int conn, int catType,
	  String objID, String replNum, String collection, String resource);

	public native int srbCollReplicateJ(int conn, int catType,
	  String collection, String resource);

	public native int queryDataInCollJ (int conn, int catType, 
	  String parColl);

	public native int queryDataJ (int conn, int catType, String objID,
	  String parColl);

	public native int querySubCollInCollJ (int conn, int catType, 
	  String parColl);

	public native String getDataResultJ( int metaIndex, int rowIndex );

	public native String getCollResultJ( int metaIndex, int rowIndex );

	public native String getGenQueResultJ( int metaIndex, int rowIndex );

	public native int getNCollAttriJ ();

	public native int getNDataAttriJ ();

	public native int srbObjUnlinkJ (int conn,  int catType, 
	  String objID, String replNum, String parColl);

	public native int srbCreateCollectJ (int conn,  int catType, 
	  String parColl, String newColl);

	public native int srbModifyDatasetMetaDJ (int conn,  int catType,
	  String objID, String collection, 
	   String newValue1, String newValue2, int retractionType);
	public native int srbModifyCollectMetaDJ (int conn,  int catType,
	  String collection, String newValue1, String newValue2, 
	   String newValue3, int retractionType);
	public native int importToOldObjJ (int conn,  int catType, 
	  String objID, String replNum, String parColl, String resource,
	   String localFile, String container);

	public native int importToNewObjJ (int conn,  int catType, 
	 String objID, String parColl, String resource, String localFile,
	  String container);

	public native int importDirJ (int conn,  int catType, 
	  String parColl, String resource, String localDir, String container);

	public native int srbRmCollJ (int conn, int catType, String parColl);

	public native int srbUnregCollJ (int conn, int catType, String parColl);

	public native int copyToOldObjJ (int conn,  int catType, 
	 String objID, String replNum, String srcColl, String targObj, 
	  String targReplNum, String targColl, String targResource, 
	   String container);

	public native int copyDataToCollJ (int conn,  int catType, 
	 String objID, String replNum, String srcColl,
	  String dataType, String size, String targColl, String targResource,
	   String container);

	public native int copyCollToCollJ (int conn, int catType, 
	 String srcColl, String targColl, String targResource, 
	  String container);

	public native int exportToFileJ(int conn, int catType,
	 String objID, String replNum, String srcColl,
	  String targFile, int forceFlag);

	public native int exportToDirJ(int conn, int catType,
	 String objID, String replNum, String srcColl,
	  String targDir);

	public native int exportCollToDirJ(int conn, int catType,
	 String srcColl, String targDir);

	public native int srbGenQuery (int conn, int catType,
	 String[] qval, int qvalInx[], int selVal[]);
	
        public native int srbRegisterDatasetJ(int conn, int catType,
         String objID, String dataType, String resource, String collection,
	  String pathName, int dataSize);

	public native int srbUnregisterDatasetJ (int conn, int catType,
	 String objID, String replNum, String collection);

        public native int srbRegisterDirJ (int conn, int catType,
	 String srcDirPath, String targColl, String resource);

        public native int getMoreDataRowsJ (int conn, int catType);

        public native int getMoreCollRowsJ (int conn, int catType);

        public native int getMoreGenQueRowsJ (int conn, int catType);
	
	public native int chmodInCollJ (int conn, int catType, 
	 String collection, String user, String domain, String mode);

	public native int ingestUserJ (int conn, int catType, 
	 String userName, String passwd, String domain, 
	  String userType, String userAddr, String userPhone, String userEmail,
	   String parentColl);

	public native int ingestGroupJ (int conn, int catType, 
	 String groupName, String passwd,
	  String groupAddr, String groupPhone, String groupEMail);

	public native int ingestLocJ (int conn, int catType, 
	 String locName, String fullAddr, String parentLoc,
	  String serverUser, String serverUserDomain); 

	public native int ingestTokenJ (int conn, int catType, 
	 String typeName, String newValue, String parentValue);

        public native int ingestResourceJ (int conn, int catType,
         String rescName, String rescType, String location, String phyPath,
	  String rscClass, int size);

        public native int ingestLogRescJ (int conn, int catType,
         String logRescName, String phyResc, String phyPath);

        public native int addLogRescJ (int conn, int catType,
         String logResc, String phyResc, String phyPath);

        public native int addCompRescJ (int conn, int catType,
         String compResc, String phyResc, String phyPath);

        public native int modifyUserJ (int conn, int catType,
         int modOpr, String userName, String domain, String modValue);

	public native int containerCreateJ (int conn, int catType,
	 String contName, String dataType, String logResc, int size);

	public native int containerDeleteJ (int conn, int catType,
	 String contName, int forceFlag);

        public native int containerSyncJ (int conn, int catType,
         String contName, int purgeFlag);

	public native String getSrbErrorString(int errorCode);

	public native int deleteValueJ(int conn, int valueType, 
				       String deleteValue);

        public native int modifyZoneJ (int conn, int catType, 
				       String userName, 
				       String dataValue1, String dataValue2, 
				       String dataValue3, String dataValue4,
				       String dataValue5, int actionType); 

	static
	{
		System.loadLibrary( "SrbJavaGlue" );
	}
}

