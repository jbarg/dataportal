/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/*****************************************************************************/
/*                                                                           */
/*  srbClObj.java                                                  	     */
/*                                                                           */
/*****************************************************************************/

package edu.sdsc.SrbBrowser;

// import com.sun.java.swing.*;
// import com.sun.java.swing.text.*;
// import com.sun.java.swing.border.*;
// import com.sun.java.swing.event.*;
import javax.swing.*;
import javax.swing.text.*;
import javax.swing.border.*;
import javax.swing.event.*;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import edu.sdsc.SrbBrowser.SrbConnObj;
import edu.sdsc.SrbBrowser.SrbAuthObj;
import edu.sdsc.SrbGlue.SrbJavaGlue;


/**
 * srbClObj
 *
 */
public class srbClObj
{
    public final static int LARGE_ARRAY_SZ = 512;

    public StringBuffer commemtBuf = new StringBuffer ();

    // retractionType definition for the modifyDatasetMetaD call.
    // they are from srbC_mdas_externs.h. 

    public final static int D_DELETE_ONE = 1;
    public final static int D_DELETE_DOMN = 2;
    public final static int D_INSERT_DOMN = 3;
    public final static int D_CHANGE_SIZE = 4;
    public final static int D_CHANGE_TYPE = 5;
    public final static int D_CHANGE_GROUP = 6;
    public final static int D_CHANGE_SCHEMA = 7;
    public final static int D_INSERT_ACCS = 8;
    public final static int D_DELETE_ACCS = 9;
    public final static int D_DELETE_ALIAS = 10;
    public final static int D_INSERT_ALIAS = 11;
    public final static int D_DELETE_COMMENTS = 12;
    public final static int D_INSERT_COMMENTS = 13;
    public final static int D_INSERT_AUDIT = 14;
    public final static int D_CHANGE_OWNER = 15;
    public final static int U_CHANGE_PASSWORD = 16;
    public final static int U_DELETE = 17;
    public final static int U_DELETE_DOMN = 18;
    public final static int U_INSERT_DOMN = 19;
    public final static int U_DELETE_GROUP = 20;
    public final static int U_INSERT_GROUP = 21;
    public final static int U_CHANGE_TYPE = 22;
    public final static int U_DELETE_ALIAS = 23;
    public final static int U_INSERT_ALIAS = 24;
    public final static int U_CHANGE_VERKEY = 25;
    public final static int U_CHANGE_ENCKEY = 26;
    public final static int D_INSERT_COLL_ACCS = 27;
    public final static int D_DELETE_COLL_ACCS = 28;
    public final static int D_CHANGE_DNAME = 29;
    public final static int D_CHANGE_COLL_NAME = 30;
    public final static int D_DELETE_COLL = 31;
    public final static int D_UPDATE_COMMENTS = 32;
    public final static int D_APPEND_COMMENTS = 33;
    public final static int SU_CHANGE_PASSWORD = 34;
    public final static int D_CHANGE_OFFSET = 35;
    public final static int D_CHANGE_DIRTY = 36;
    public final static int U_DELETE_AUTH_MAP = 37;
    public final static int U_INSERT_AUTH_MAP = 38;

    // DCS-ATTRIBUTE-INDEX
    // They are from mdasC_db2_externs.h.

    public final static int   DATA_ID              =     0;
    public final static int   DATA_REPL_ENUM       =     1;
    public final static int   DATA_NAME            =     2;
    public final static int   USER_GROUP_NAME      =     3;
    public final static int   DATA_TYP_NAME        =     4;
    public final static int   USER_TYP_NAME        =     5;
    public final static int   USER_ADDRESS         =     6;
    public final static int   USER_NAME            =     7;
    public final static int   ACCESS_CONSTRAINT    =     8;
    public final static int   DOMAIN_DESC          =     9;
    public final static int   PATH_NAME            =    10;
    public final static int   RSRC_NAME            =    11;
    public final static int   RSRC_ADDR_NETPREFIX  =    12;
    public final static int   RSRC_TYP_NAME        =    13;
    public final static int   REPL_TIMESTAMP       =    14;
    public final static int   DATA_GRP_NAME        =    15;
    public final static int   USER_PHONE           =    16;
    public final static int   USER_EMAIL           =    17;
    public final static int   SIZE                 =    18;
    public final static int   USER_AUDIT_TIME_STAMP=    19;
    public final static int   USER_AUDIT_COMMENTS  =    20;
    public final static int   DATA_COMMENTS        =    21;
    public final static int   AUDIT_ACTION_DESC    =    22;
    public final static int   AUDIT_TIMESTAMP      =    23;
    public final static int   AUDIT_COMMENTS       =    24;
    public final static int   GROUP_ACCESS_CONSTRAINT=  25;
    public final static int   ACCESS_GROUP_NAME    =    26;
    public final static int   RSRC_DEFAULT_PATH    =    27;
    public final static int   PHY_RSRC_DEFAULT_PATH=    28;
    public final static int   PHY_RSRC_NAME        =    29;
    public final static int   PHY_RSRC_TYP_NAME    =    30;
    public final static int   RSRC_REPL_ENUM       =    31;
    public final static int   DATA_ACCESS_LIST     =    32;
    public final static int   RSRC_ACCESS_LIST     =    33;
    public final static int   DATA_IS_DELETED      =    34;
    public final static int  DATA_OWNER            =    35;
    public final static int  TICKET_D              =    36;
    public final static int  TICKET_BEGIN_TIME_D   =    37;
    public final static int  TICKET_END_TIME_D     =    38;
    public final static int  TICKET_ACC_COUNT_D    =    39;
    public final static int  TICKET_ACC_LIST_D     =    40;
    public final static int  TICKET_OWNER_D        =    41;
    public final static int  TICKET_USER_D         =    42;
    public final static int  TICKET_C              =    43;
    public final static int  TICKET_BEGIN_TIME_C   =    44;
    public final static int  TICKET_END_TIME_C     =    45;
    public final static int  TICKET_ACC_COUNT_C    =    46;
    public final static int  TICKET_ACC_LIST_C     =    47;
    public final static int  TICKET_OWNER_C        =    48;
    public final static int  TICKET_USER_C         =    49;
    public final static int  TICKET_USER_DOMAIN_C  =    50;
    public final static int  TICKET_USER_DOMAIN_D  =    51;
    public final static int  TICKET_OWNER_DOMAIN_C =    52;
    public final static int  TICKET_OWNER_DOMAIN_D =    53;
    public final static int  PARENT_COLLECTION_NAME=    54;
    public final static int  LOCATION_NAME=    		55;
    public final static int  IS_DIRTY=    		56;
    public final static int  CONTAINER_MAX_SIZE=	57;
    public final static int  CONTAINER_NAME=    	58;
    public final static int  RSRC_CLASS=    		59;
    public final static int  MAX_OBJ_SIZE=    		60;
    public final static int  OFFSET=    		61;
    public final static int  CONTAINER_RSRC_NAME=    	62;
    public final static int  CONTAINER_RSRC_CLASS=    	63;
    public final static int  CONTAINER_SIZE=    	64;
    public final static int  CONTAINER_LOG_RSRC_NAME=   65;
    public final static int  DATA_OWNER_DOMAIN=   	66;
    public final static int  USER_DISTIN_NAME=   	67;
    public final static int  USER_AUTH_SCHEME=   	68;


    public final static int  COMPOUND_RSRC_NAME=       264;

    public final static int ZONE_NAME=                  300;
    public final static int ZONE_LOCALITY=              301;
    public final static int ZONE_NETPREFIX=             302;
    public final static int ZONE_PORT_NUM=              303;
    public final static int ZONE_ADMIN_AUTH_SCHEME_NAME=304;
    public final static int ZONE_ADMIN_DISTIN_NAME=     305;
    public final static int ZONE_STATUS=                306;
    public final static int ZONE_CREATE_DATE=           307;
    public final static int ZONE_MODIFY_DATE=           308;
    public final static int ZONE_COMMENTS=              309;
    public final static int ZONE_CONTACT=               310;
    public final static int ZONE_ADMIN_NAME=            311;
    public final static int ZONE_ADMIN_DOMAIN_NAME=     312;

    public final static int RSRC_COMMENTS                = 326;
    public final static int RSRC_CREATE_DATE             = 327;
    public final static int RSRC_MODIFY_DATE             = 328;
    public final static int RSRC_MAX_OBJ_SIZE            = 331;
    public final static int RSRC_OWNER_NAME              = 332;
    public final static int RSRC_OWNER_DOMAIN            = 333;
    public final static int RSRC_MLSEC_LATENCY_MAX       = 334;
    public final static int RSRC_MLSEC_LATENCY_MIN       = 335;
    public final static int RSRC_MBPS_BANDWIDTH          = 336;
    public final static int RSRC_CONCURRENCY_MAX         = 337;
    public final static int RSRC_NUM_OF_HIERARCHIES      = 338;
    public final static int RSRC_NUM_OF_STRIPES          = 339;
    public final static int RSRC_MEGAB_CAPACITY          = 340;
    public final static int RSRC_OWNER_ZONE              = 344;

    public final static int ZONE_LOCN_DESC=             360;

    // From srbC_mdas_extern.h:
    public final static int Z_INSERT_NEW_LOCAL_ZONE=    151;
    public final static int Z_INSERT_NEW_ALIEN_ZONE=    152;
    public final static int Z_MODIFY_ZONE_INFO=         153;
    public final static int Z_MODIFY_ZONE_FOR_USER=     154;
    public final static int Z_CHANGE_ZONE_NAME=         155;
    public final static int Z_MODIFY_ZONE_LOCAL_FLAG=   156;
    public final static int Z_MODIFY_ZONE_STATUS=       157;

   // From srbC_mdas_externs.h:
    public final static int  DELETE_TYPE_LOCATION = 1;
    public final static int  DELETE_TYPE_USER     = 2;
    public final static int  DELETE_TYPE_RESOURCE = 3;

    /* Special format for the CONTAINER_NAME attribute */

    public final static String contHeader = new String ("Cont");
    public final static int  contLen = 4;
    public final static String contValueY = new String ("IC");  // inContainer
    public final static String contValueN = new String ("  ");  // not inCont

   
    public final static String [] attrHeader = { 
	"DataID",		// DATA_ID
	"ReplN",		// DATA_REPL_ENUM
	"DataName",		// DATA_NAME
	"UserGroup",		// USER_GROUP_NAME
	"DataType",		// DATA_TYP_NAME
	"UserType",		// USER_TYP_NAME
	"UserAddress",		// USER_ADDRESS
	"User",			// USER_NAME
	"AccessType",		// ACCESS_CONSTRAINT
	"Domain",		// DOMAIN_DESC
	"PathName", 		// PATH_NAME
	"Resource",		// RSRC_NAME
	"RescPrefix",		// RSRC_ADDR_NETPREFIX 
				// hostAddr:(dataBase):(instance)
	"RescType",		// RSRC_TYP_NAME
	"TimeStamp",		// REPL_TIMESTAMP
	"Collection",		// DATA_GRP_NAME
	"UserPhone",		// USER_PHONE
	"UserEMail",		// USER_EMAIL
	"Size",			// SIZE
	"UAuditTime",		// USER_AUDIT_TIME_STAMP
	"UAuditComment", 	// USER_AUDIT_COMMENTS
	"Comment",		// DATA_COMMENTS
	"AuditAction",		// AUDIT_ACTION_DESC
	"AuditTime",		// AUDIT_TIMESTAMP
	"AuditComment",		// AUDIT_COMMENTS
	"CollAccessType", 	// GROUP_ACCESS_CONSTRAINT
	"CollForAccQuery", 	// ACCESS_GROUP_NAME
	"LogRescDefPath", 	// RSRC_DEFAULT_PATH
	"PhyRescDefPath", 	// PHY_RSRC_DEFAULT_PATH
	"PhyResource",		// PHY_RSRC_NAME
	"PhyRescType",		// PHY_RSRC_TYP_NAME
	"PhyRescReplNum", 	// RSRC_REPL_ENUM, not used
	"DataAccList",		// DATA_ACCESS_LIST
	"RescAccList", 		// RSRC_ACCESS_LIST
	"dataIsDeleted", 	// DATA_IS_DELETED
	"Owner",		// DATA_OWNER
	"DataTicket",		// TICKET_D
	"DataTicketBegTime", 	// TICKET_BEGIN_TIME_D
	"DataTicketEndTime",   	// TICKET_END_TIME_D
	"DataTicketAccCnt",	// TICKET_ACC_COUNT_D
	"DataTicketAccList",	// TICKET_ACC_LIST_D
	"DataTicketIssuer",	// TICKET_OWNER_D
	"DataTicketUser",	// TICKET_USER_D
	"CollTicket",		// TICKET_C
	"CollTicketBegTime",	// TICKET_BEGIN_TIME_C
	"CollTicketEndTime",	// TICKET_END_TIME_C
	"CollTicketAccCnt",	// TICKET_ACC_COUNT_C
	"CollTicketAccList",	// TICKET_ACC_LIST_C
	"CollTicketIssuer",	// TICKET_OWNER_C
	"CollTicketUser",	// TICKET_USER_C
	"CollTicketUserDomain",	// TICKET_OWNER_DOMAIN_C
	"DataTicketUserDomain", // TICKET_OWNER_DOMAIN_D
	"CollTicketIssuerDomain", // TICKET_OWNER_DOMAIN_C	
	"DataTicketIssuerDomain", // TICKET_OWNER_DOMAIN_D
	"ParentColl",		// PARENT_COLLECTION_NAME, only used in mkdir
	"Location",		// LOCATION_NAME, the host location
	"IsDirty",		// IS_DIRTY
	"ContMaxSize",		// CONTAINER_MAX_SIZE
	"Container",		// CONTAINER_NAME
	"RescClass",		// RSRC_CLASS
	"ObjMaxSize",		// MAX_OBJ_SIZE
	"Offset",		// OFFSET
	"ContResc",		// CONTAINER_RSRC_NAME
	"ContRescClass",	// CONTAINER_RSRC_CLASS
	"ContSize",		// CONTAINER_SIZE
	"ContLogResc",		// CONTAINER_LOG_RSRC_NAME
	"OwnerDoamin",		// DATA_OWNER_DOMAIN
	"DnName",		// USER_DISTIN_NAME
	"AuthScheme"		// USER_AUTH_SCHEME
    };
	
    public final static int [] attrLen = { 
	8,		// DATA_ID
	5,		// DATA_REPL_ENUM
	12,		// DATA_NAME
	12,		// USER_GROUP_NAME
	8,		// DATA_TYP_NAME
	8,		// USER_TYP_NAME
	80,		// USER_ADDRESS
	10,		// USER_NAME
	10,		// ACCESS_CONSTRAINT
	10,		// DOMAIN_DESC
	80, 		// PATH_NAME
	15,		// RSRC_NAME
	40,		// RSRC_ADDR_NETPREFIX 
			// hostAddr:(dataBase):(instance)
	8,		// RSRC_TYP_NAME
	16,		// REPL_TIMESTAMP
	40,		// DATA_GRP_NAME
	20,		// USER_PHONE
	20,		// USER_EMAIL
	10,		// SIZE
	16,		// USER_AUDIT_TIME_STAMP
	80, 		// USER_AUDIT_COMMENTS
	80,		// DATA_COMMENTS
	12,		// AUDIT_ACTION_DESC
	16,		// AUDIT_TIMESTAMP
	80,		// AUDIT_COMMENTS
	14, 		// GROUP_ACCESS_CONSTRAINT
	40, 		// ACCESS_GROUP_NAME
	80, 		// RSRC_DEFAULT_PATH
	80, 		// PHY_RSRC_DEFAULT_PATH
	12,		// PHY_RSRC_NAME
	12,		// PHY_RSRC_TYP_NAME
	14, 		// RSRC_REPL_ENUM, not used
	11,		// DATA_ACCESS_LIST
	11, 		// RSRC_ACCESS_LIST
	13, 		// DATA_IS_DELETED
	12,		// DATA_OWNER
	16,		// TICKET_D
	16, 		// TICKET_BEGIN_TIME_D
	16,   		// TICKET_END_TIME_D
	16,		// TICKET_ACC_COUNT_D
	17,		// TICKET_ACC_LIST_D
	17,		// TICKET_OWNER_D
	14,		// TICKET_USER_D
	16,		// TICKET_C
	17,		// TICKET_BEGIN_TIME_C
	17,		// TICKET_END_TIME_C
	15,		// TICKET_ACC_COUNT_C
	17,		// TICKET_ACC_LIST_C
	16,		// TICKET_OWNER_C
	14,		// TICKET_USER_C
	20,		// TICKET_OWNER_DOMAIN_C
	20, 		// TICKET_OWNER_DOMAIN_D
	22, 		// TICKET_OWNER_DOMAIN_C	
	22, 		// TICKET_OWNER_DOMAIN_D
	40,		// PARENT_COLLECTION_NAME, only used in mkdir
	20, 		// LOCATION_NAME
	2, 		// IS_DIRTY
	12, 		// CONTAINER_MAX_SIZE
	40, 		// CONTAINER_NAME
	12, 		// RSRC_CLASS
	12, 		// MAX_OBJ_SIZE
	12, 		// OFFSET
	20, 		// CONTAINER_RSRC_NAME
	20, 		// CONTAINER_RSRC_CLASS
	12, 		// CONTAINER_SIZE
	20 		// CONTAINER_LOG_RSRC_NAME
    };
	
    public final static String [] accCntlType = {
	"read",
	"write",
	"all",
	"null"
    };
	
    // JNI (Java Native Interface) Routines for SRB
   int conn = 0;
   int catType = 0;
    static SrbJavaGlue srbJG = new SrbJavaGlue();

    // Authentication and Connection objects
    SrbAuthObj authObj;
    SrbConnObj connObj;
   
    // some MCAT default stuff
    public String cwd = null;		// The home collection
    public String defResc = null;	// the default resource
    public String defCont = null;	// the default container
    public String userName = null;	// the default client user
    public String domain = null;	// the default client domain
    // public String defLocalCacheDir = new String ("/tmp");
    public String defLocalCacheDir;
    public String[] rescList = null;	// the resource list

   // some temp stuff list Coll which should be deleted later
   String flag = new String( "C" );
   int types[] = new int[200];

    // Index to keep track of where data and result count is
    public int nDataAttri;
    public int dataCount = 0;
    public int dataInx = 0;
    public int nCollAttri;
    public int collCount = 0;
    public int collInx = 0;
    public int nGenQueAttri;
    public int queResultCount = 0;
    public int queResultInx = 0;

    public srbClObj(SrbAuthObj authObj, SrbConnObj connObj) throws Exception {
	this.authObj = authObj;
	this.connObj = connObj;
	this.nDataAttri = srbJG.getNDataAttriJ ();
	this.nCollAttri = srbJG.getNCollAttriJ ();
        conn = srbJG.clConnectJ( connObj.getHost(), connObj.getPort(),
          authObj.getPass() , authObj.getUser(), authObj.getDomain());
	if (conn < 0)
	    // System.exit(0);
            throw new Exception
              ("SRBConnection with server could not be established");
	this.cwd = srbJG.getDefCollection ();
	this.defResc = srbJG.getDefResource ();
	this.userName = srbJG.getUser (conn);
	this.domain = srbJG.getDomain (conn);
	this.rescList = listResc ();
	defLocalCacheDir = srbJG.getLocalCacheDir(); 
    }

    // a method to access the int conn
    public int getNumOfConn(){
        return conn;
    }

    public void disconnect() {
	int err;

	// The conn value is static for all instances of the srbClObj class.
	if ( conn != 0 ) {
	    // System.err.println( "DEBUG: clFinishJ" ); // DEBUG
	    err = srbJG.clFinishJ( conn );
	    conn = 0;
    	}
    }

    public String[] listResc() {
        int count = srbJG.srbListResourceJ(conn, 0);
        String[] list = new String[count];

        for ( int i=0; i<count; i++ ) {
            list[i] = srbJG.getResListItem(conn, i);
        }

        return( list );
    }

    public String[] listContainer (srbClObj mySrbClObj, int ownerFlag) {
        int i, length;
        String[] selResult;
        String[] qval = new String[3];
        int [] qvalInx = new int[3];
        int [] selInx = new int[1];
	String[] tmpArray = new String[LARGE_ARRAY_SZ];

	if (ownerFlag > 0)
            qvalInx[0] = mySrbClObj.DATA_OWNER;
	else
	    qvalInx[0] = mySrbClObj.USER_NAME;

        qvalInx[1] = mySrbClObj.DOMAIN_DESC;
        qvalInx[2] = mySrbClObj.CONTAINER_NAME;

        qval[0] = new String (" = '" + mySrbClObj.userName + "'");
        qval[1] = new String (" = '" + mySrbClObj.domain + "'");
        qval[2] = new String (" is not null");
        selInx[0] = mySrbClObj.CONTAINER_NAME;

        selResult = mySrbClObj.getFirstGenQueResult (qval, qvalInx, selInx);

	length = 0;
        while (selResult != null) {
	    tmpArray[length] = new String (selResult[0]);
            selResult = mySrbClObj.getNextGenQueResult();
	    length ++;
        }
	String[] listResult = new String[length];
	for (i = 0; i < length; i++) {
	    listResult[i] = tmpArray[i];
	}
	return (listResult);
    }

    public int listColl (String collectionName) {
	int count;

	count = srbJG.srbListCollectJ( conn, catType,
			collectionName, this.flag, this.types );

	return count;
    }

    public int objReplicate (String objID, String replNum, String collection, 
     String resource) {
    	int status;

    	status = srbJG.srbObjReplicateJ (conn, catType,
    	  objID, replNum, collection, resource);

    	return status;
    }

    public int chmodInColl (String collection, String myUser, String myDomain,
     String myAccCntl) {
        int status;

        status = srbJG.chmodInCollJ (conn, catType, collection, myUser,
	 myDomain, myAccCntl);

        return status;
    }

    public int replicateColl (String collection, String resource) {
        int status;

        status = srbJG.srbCollReplicateJ (conn, catType, collection, resource);

        return status;
    }

    public int objUnlink (String objID, String replNum, String collection) {
        int status;

        status = srbJG.srbObjUnlinkJ (conn, catType,
          objID, replNum, collection);

        return status;
    }

    public int objUnreg (String objID, String replNum, String collection) {
        int status;

        status = srbJG.srbUnregisterDatasetJ (conn, catType,
          objID, replNum, collection);

        return status;
    }

    public int createCollect (String parColl, String newColl) {
        int status;

        status = srbJG.srbCreateCollectJ (conn, catType, parColl, newColl);

        return status;
    }

    public int modifyDatasetMetaD (String objID, String collection, 
     String newValue1, String newValue2, int retractionType) {
        int status;

        status = srbJG.srbModifyDatasetMetaDJ (conn, catType, objID,
	collection, newValue1, newValue2, retractionType);

        return status;
    }

    public int modifyCollectMetaD (String collection,
     String newValue1, String newValue2, String newValue3, int retractionType) {
        int status;

        status = srbJG.srbModifyCollectMetaDJ (conn, catType,
        collection, newValue1, newValue2, newValue3, retractionType);

        return status;
    }

    public int srbRegisterDatasetJ (String objID, String dataType, 
     String resource, String collection, String pathName, int dataSize) {
        int status;

        status = srbJG.srbRegisterDatasetJ (conn, catType, objID,
        dataType, resource, collection, pathName, dataSize);

        return status;
    }

    public int srbRegisterDirJ (String srcDirPath, String targColl,
     String resource) {
        int status;

        status = srbJG.srbRegisterDirJ (conn, catType, srcDirPath,
        targColl, resource);

        return status;
    }

    public int collUnlink (String collection) {
        int status;

        status = srbJG.srbRmCollJ (conn, catType, collection);

        return status;
    }

    public int collUnreg (String collection) {
        int status;

        status = srbJG.srbUnregCollJ (conn, catType, collection);

        return status;
    }

    public int importToOldObj (String objID, String replNum, String collection,
      String resource, String localFile, String container) {
        int status;

        status = srbJG.importToOldObjJ (conn, catType,
          objID, replNum, collection, resource, localFile, container);

        return status;
    }

    public int importToNewObjJ (String objID, String collection, 
      String resource, String localFile, String container) {
        int status;

        status = srbJG.importToNewObjJ (conn, catType,
          objID, collection, resource, localFile, container);

        return status;
    }

    public int importDirJ (String collection,
      String resource, String localDir, String container) {
        int status;

        status = srbJG.importDirJ (conn, catType,
          collection, resource, localDir, container);

        return status;
    }

    public int exportToFileJ (String objID, String replNum, String srcColl,
     String targFile, int forceFlag) {
	int status;

        status = srbJG.exportToFileJ (conn, catType,
          objID, replNum, srcColl, targFile, forceFlag);

        return status;
    }

    public int exportToDirJ (String objID, String replNum, String srcColl,
     String targDir) {
	int status;

        status = srbJG.exportToDirJ (conn, catType,
          objID, replNum, srcColl, targDir);

        return status;
    }

    public int exportCollToDirJ (String srcColl, String targDir) {
        int status;

        status = srbJG.exportCollToDirJ (conn, catType,
          srcColl, targDir);

        return status;
    }

    public int copyToOldObjJ (String objID, String replNum, String collection,
     String targObj, String targReplNum, String targColl, String resource,
      String container) {
        int status;

        status = srbJG.copyToOldObjJ (conn, catType,
         objID, replNum, collection, targObj, targReplNum, targColl, 
	  resource, container);

        return status;
    }

    public int copyDataToCollJ (String objID, String replNum, String collection,
     String dataType, String size, String targColl, String resource,
      String container) {
        int status;

        status = srbJG.copyDataToCollJ (conn, catType,
         objID, replNum, collection, dataType, size, targColl, resource,
	  container);

        return status;
    }

    public int copyCollToCollJ (String collection, String targColl, 
      String resource, String container) {
        int status;

        status = srbJG.copyCollToCollJ (conn, catType,
          collection, targColl, resource, container);

        return status;
    }

    public int ingestUserJ (String userName, String passwd, String domain,
     String userType, String userAddr, String userPhone, String userEmail,
      String parentColl) {
        int status = 0;

        status = srbJG.ingestUserJ (conn, catType,
	 userName, passwd, domain, userType, userAddr, userPhone, userEmail, 
	  parentColl);

        return status;
    }

    public int modifyUserJ (int modOpr, String userName, String domain,
     String modValue) {
        int status = 0;

        status = srbJG.modifyUserJ (conn, catType,
        modOpr, userName, domain, modValue);

        return status;
    }

    public int ingestGroupJ (String groupName, String passwd,
     String groupAddr, String groupPhone, String groupEMail) {
        int status = 0;

        status = srbJG.ingestGroupJ (conn, catType,
         groupName, passwd, groupAddr, groupPhone, groupEMail);

        return status;
    }

    public int ingestLocJ (String locName, String fullAddr, 
     String parentLoc, String serverUser, String serverUserDomain) {
        int status = 0;

        status = srbJG.ingestLocJ (conn, catType,
         locName, fullAddr, parentLoc, serverUser, serverUserDomain);

        return status;
    }

    public int ingestTokenJ (String typeName, String newValue, 
     String parentValue) {
        int status = 0;

        status = srbJG.ingestTokenJ (conn, catType,
         typeName, newValue, parentValue);

        return status;
    }

    public int ingestResourceJ (String rescName, String rescType,
     String location, String phyPath, String rscClass, int size) {
        int status = 0;

        status = srbJG.ingestResourceJ (conn, catType,
         rescName, rescType, location, phyPath, rscClass, size);

        return status;
    }

    public int ingestLogRescJ (String rescName, String phyResc, String phyPath)
    {
        int status = 0;

        status = srbJG.ingestLogRescJ (conn, catType,
         rescName, phyResc, phyPath);

        return status;
    }

    public int addLogRescJ (String logResc, String phyResc, String phyPath)
    {
        int status = 0;

        status = srbJG.addLogRescJ (conn, catType,
         logResc, phyResc, phyPath);

        return status;
    }

    public int addCompRescJ (String compResc, String phyResc, String phyPath)
    {
        int status = 0;

        status = srbJG.addLogRescJ (conn, catType,
         compResc, phyResc, phyPath);

        return status;
    }

    public int containerCreateJ (String contName, String dataType, 
     String logResc, int size)
    {
        int status = 0;

        status = srbJG.containerCreateJ (conn, catType,
         contName, dataType, logResc, size);

        return status;
    }

    public int containerDeleteJ (String contName, int forceFlag)
    {
        int status = 0;

        status = srbJG.containerDeleteJ (conn, catType, contName, forceFlag);

        return status;
    }

    public int containerSyncJ (String contName, int purgeFlag)
    {
        int status = 0;

        status = srbJG.containerSyncJ (conn, catType, contName, purgeFlag);

        return status;
    }

    public String[] getFirstDataInColl (String collectionName) {

	dataInx = 0;
	dataCount = srbJG.queryDataInCollJ (conn, catType, collectionName);
	
	if (dataCount > 0) {
	    return (getNextDataInfo ());
	} else {
	    return null;
	}
    }

    public String[] getFirstDataInfo (String objName, String collectionName) {

        dataInx = 0;
        dataCount = srbJG.queryDataJ (conn, catType, objName, collectionName);

        return (getNextDataInfo ());
    }

    public String[] getFirstGenQueResult (String[] qval, int qvalInx[], 
     int selVal[]) {

	queResultInx = 0;

        queResultCount = srbJG.srbGenQuery (conn, catType, qval, 
	 qvalInx, selVal);

	nGenQueAttri = selVal.length;
        return (getNextGenQueResult ());
    }

    // Like getFirstGenQueResult (does a query and prepares for a series
    // of calls to getNextGenQueResult) but returns the count instead
    // of the first result.
    public int performFirstGenQuery(String[] qval, int qvalInx[], 
                                    int selVal[]) {

        queResultInx = 0;

        queResultCount = srbJG.srbGenQuery (conn, catType, qval, 
                                            qvalInx, selVal);

        nGenQueAttri = selVal.length;
        return (queResultCount);
    }

    public String[] getNextDataInfo () {
	int i;
	String[] myData;

	if (nDataAttri > 0)
	    myData = new String[nDataAttri];
	else
	    return null;

	if (dataInx >= dataCount) {
	    dataInx = 0;
	    dataCount = srbJG.getMoreDataRowsJ (conn, catType);
	    if (dataCount <= 0) 
	        return null;
	}

	for (i = 0; i < nDataAttri; i++) {
	    myData[i] = srbJG.getDataResultJ (i, dataInx);
	}
	dataInx ++;
	
	return (myData);
    }
        
    public String[] getNextGenQueResult () {
        int i;
	String[] myQueResult;
	
	if (nGenQueAttri > 0)
            myQueResult = new String[nGenQueAttri];
	else
	    return null;

        if (queResultInx >= queResultCount) {
            queResultInx = 0;
            queResultCount = srbJG.getMoreGenQueRowsJ (conn, catType);
            if (queResultCount <= 0)
                return null;
        }

        for (i = 0; i < nGenQueAttri; i++) {
            myQueResult[i] = srbJG.getGenQueResultJ (i, queResultInx);
        }
        queResultInx ++;

        return (myQueResult);
    }

    public String[] getFirstCollInColl (String collectionName) {

	collInx = 0;
        collCount = srbJG.querySubCollInCollJ (conn, catType, collectionName);

        return (getNextCollInColl ());
    }

    public String[] getNextCollInColl () {
        int i;
	String[] myColl;

	if (nCollAttri > 0)
            myColl = new String[nCollAttri];
	else 
	    return null;

        if (collInx >= collCount) {
            collInx = 0;
            collCount = srbJG.getMoreCollRowsJ (conn, catType);
            if (collCount <= 0)
                return null;
        }

        for (i = 0; i < nCollAttri; i++) {
            myColl[i] = srbJG.getCollResultJ (i, collInx);
        }
        collInx ++;

        return (myColl);
    }

    public void setDefComBuffer (String inpComment) {

	commemtBuf.setLength (0);
	commemtBuf.append (inpComment);
    }
	
    // convert an SRB error/status code to a string message
    public String getSrbErrorString(int errorCode) {
        return srbJG.getSrbErrorString( errorCode );
    }

    // delete a value
    public int deleteValueJ (int valueType, String deleteValue) {
        int status = 0;

        status = srbJG.deleteValueJ (conn, valueType, deleteValue);

        return status;
    }

    public int modifyZoneJ (String zoneName, String dataValue1,
			    String dataValue2, String dataValue3, 
			    String dataValue4, String dataValue5, 
			    int actionType) {
        int status = 0;

        status = srbJG.modifyZoneJ (conn, catType,
				    zoneName, dataValue1,
				    dataValue2, dataValue3,
				    dataValue4, dataValue5, actionType);

        return status;
    }



}

