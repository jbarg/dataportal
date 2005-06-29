/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/


/* objMcatAdminFunct.c - Routines to handle mcat admin type SID.
 */

#include "objMcatAdminFunct.h"


int
svrRegisterUserGrp (int catType, struct varlena *vUserGrpName,
struct varlena *vUserGrpPasswd, struct varlena *vUserGrpType,
struct varlena *vUserGrpAddress, struct varlena *vUserGrpPhone,
struct varlena *vUserGrpEmail)
{
    char *userGrpName;
    char *userGrpPasswd;
    char *userGrpType;
    char *userGrpAddress;
    char *userGrpPhone;
    char *userGrpEmail;
    int status;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    userGrpName = varToStr (vUserGrpName);
    userGrpPasswd = varToStr (vUserGrpPasswd);
    userGrpType = varToStr (vUserGrpType);
    userGrpAddress = varToStr (vUserGrpAddress);
    userGrpPhone = varToStr (vUserGrpPhone);
    userGrpEmail = varToStr (vUserGrpEmail);

    remoteFlag = getMdasEnabledHost (NULL, &hostTabPtr);

    if (remoteFlag < 0) {
	status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        char *userName;

        userName = ClientUser->userName;
        status = register_user_group (catType, userGrpName, userGrpPasswd,
        userGrpType, userGrpAddress, userGrpPhone, 
	userGrpEmail, userName, ClientUser->userAuth, ClientUser->domainName);

	if (status < 0) {
            elog (NOTICE,
              "svrRegisterUserGrp: register_user_group failed status = %d",
	      status);
        } else {
            commit_db2_interaction (MDAS_CAT_COMMIT);
	} 
#endif
    } else {
        if ((status = remoteConnect (hostTabPtr)) < 0) {
            return status;
        }

        /* Do a client call to the remote host */

        status = srbRegisterUserGrp (hostTabPtr->conn, catType, userGrpName,
          userGrpPasswd, userGrpType, userGrpAddress, userGrpPhone,
	  userGrpEmail);

        if (status < 0) {       /* Failure */
            elog (NOTICE, "svrRegisterUserGrp failed: %s",
              clErrorMessage(hostTabPtr->conn));
        }
    }

    free (userGrpName);
    free (userGrpPasswd);
    free (userGrpType);
    free (userGrpAddress);
    free (userGrpPhone);
    free (userGrpEmail);

    return (status);
}

int
svrRegisterUser (int catType, struct varlena *vUserName, 
struct varlena *vUserDomain,
struct varlena *vUserPasswd, struct varlena *vUserType,
struct varlena *vUserAddress, struct varlena *vUserPhone,
struct varlena *vUserEmail)
{
    char *userName;
    char *userDomain;
    char *userPasswd;
    char *userType;
    char *userAddress;
    char *userPhone;
    char *userEmail;
    int status;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    userName = varToStr (vUserName);
    userDomain = varToStr (vUserDomain);
    userPasswd = varToStr (vUserPasswd);
    userType = varToStr (vUserType);
    userAddress = varToStr (vUserAddress);
    userPhone = varToStr (vUserPhone);
    userEmail = varToStr (vUserEmail);

    remoteFlag = getMdasEnabledHost (NULL, &hostTabPtr);

    if (remoteFlag < 0) {
	status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        char *srbUserName;

        srbUserName = ClientUser->userName;
        status = register_user_info (catType, userName, userPasswd, userDomain, 
	userType, userAddress, userPhone,
        userEmail, srbUserName, ClientUser->userAuth, ClientUser->domainName);

        if (status < 0) {
            elog (NOTICE,
              "svrRegisterUser: register_user_info failed status = %d",
              status);
        } else {
            commit_db2_interaction (MDAS_CAT_COMMIT);
	}
#endif
    } else {
        if ((status = remoteConnect (hostTabPtr)) < 0) {
            return status;
        }

        /* Do a client call to the remote host */

        status = srbRegisterUser (hostTabPtr->conn, catType, userName,
          userDomain, userPasswd, userType, userAddress, userPhone,
          userEmail);

        if (status < 0) {       /* Failure */
            elog (NOTICE, "svrRegisterUser failed: %s",
              clErrorMessage(hostTabPtr->conn));
        }
    }

    free (userName);
    free (userDomain);
    free (userPasswd);
    free (userType);
    free (userAddress);
    free (userPhone);
    free (userEmail);

    return (status);
}

int
svrModifyUser (int catType, struct varlena *vDataValue1,
struct varlena *vDataValue2, int retractionType)
{
    char *dataValue1;
    char *dataValue2;
    int status;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    dataValue1 = varToStr (vDataValue1);
    dataValue2 = varToStr (vDataValue2);

    /* XXXXXX potential problem. Don't know mcatName */
    remoteFlag = getMdasEnabledHost (NULL, &hostTabPtr);

    if (remoteFlag < 0) {
	status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
	int authCount;
	char userStr[MAX_TOKEN];
	char *domainPtr;
	

	if (retractionType == U_DELETE_AUTH_MAP) {
	    strcpy (userStr, dataValue1);
	    domainPtr = strstr(userStr,"@");
	    *domainPtr = '\0';
	    domainPtr ++;
	    authCount = getUserAuthSchemeCnt (userStr, domainPtr);
	    if (authCount <= 1) {
		elog (NOTICE, 
		 "svrModifyUser: removal of last AuthScheme for %s not allowed",
		 dataValue1);
		status = OBJ_ERR_RM_LAST_AUTH_SCHEME;
	    } else {
                status = modify_user_info (catType, ClientUser->userName, 
		 dataValue1, dataValue2, retractionType, ClientUser->userAuth, 
	         ClientUser->domainName);
	    }
	} else {
            status = modify_user_info (catType, ClientUser->userName,
             dataValue1, dataValue2, retractionType, ClientUser->userAuth,
             ClientUser->domainName);
	}

        if (status < 0) {
            elog (NOTICE,
              "svrModifyUser: modify_user_info failed status = %d",
              status);
        } else {
            commit_db2_interaction (MDAS_CAT_COMMIT);
	}
#endif
    } else {
        if ((status = remoteConnect (hostTabPtr)) < 0) {
            return status;
        }

        /* Do a client call to the remote host */

        status = srbModifyUser (hostTabPtr->conn, catType, dataValue1,
          dataValue2, retractionType);

        if (status < 0) {       /* Failure */
            elog (NOTICE, "svrModifyUser failed: %s",
              clErrorMessage(hostTabPtr->conn));
        }
    }

    free (dataValue1);
    free (dataValue2);

    return (status);
}

int
svrObjAudit (int catType, struct varlena *vUserName, struct varlena *vObjID,
struct varlena *vCollectionName, struct varlena *vDataPath, 
struct varlena *vResourceName, struct varlena *vAccessMode, 
struct varlena *vComment, int success, struct varlena *vDomainName)
{
    char *userName;
    char *objID;
    char *collectionName;
    char *dataPath;
    char *resourceName;
    char *accessMode;
    char *comment;
    char *domainName;
    int status;

    userName = varToStr (vUserName);
    objID = varToStr (vObjID);
    collectionName = varToStr (vCollectionName);
    dataPath = varToStr (vDataPath);
    resourceName = varToStr (vResourceName);
    accessMode = varToStr (vAccessMode);
    comment = varToStr (vComment);
    domainName = varToStr (vDomainName);

    status = _svrObjAudit (catType, userName, objID, collectionName,  
    dataPath,  resourceName, accessMode,  comment, success,  domainName);

    return status;
}
    
int
_svrObjAudit (int catType, char *userName, char *objID,
char *collectionName,  char *dataPath,  char *resourceName, 
char *accessMode,  char *comment, int success,  char *domainName)
{
    int status;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    if (set_audit_trail (GET_AUDIT_TRAIL_SETTING) != AUDIT_TRAIL_ON)
	return -1;

    remoteFlag = getMdasEnabledHost (collectionName, &hostTabPtr);

    if (remoteFlag < 0) {
	status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        status = auditDatasetAccess (catType, userName, objID,
        collectionName, dataPath, resourceName, accessMode,
        comment, success, domainName);

        if (status < 0) {
            elog (NOTICE,
              "_svrObjAudit: auditDatasetAccess failed status = %d",
              status);
        }
#endif
    } else {
        if ((status = remoteConnect (hostTabPtr)) < 0) {
            return status;
        }

        /* Do a client call to the remote host */

        status = srbObjAudit (hostTabPtr->conn, catType, userName,
	objID, collectionName, dataPath, resourceName, accessMode,
        comment, success, domainName);

        if (status < 0) {       /* Failure */
            elog (NOTICE, "_svrObjAudit: srbObjAudit failed: %s",
              clErrorMessage(hostTabPtr->conn));
        }
    }
    return (status);
}

/* svrGetPrivUsers - Entry point for the srbGetPrivUsers SID.
 *
 *
 * Input : int catalog - The catalog type. e.g., MDAS_CATALOG
 *	   int rowsWanted - No. or rows to be returned.
 *
 * Output : success - Returns the content in struct varlena
 *          failure - Returns NULL string.
 */

struct varlena *
svrGetPrivUsers (int catalog, int rowsWanted)
{
    struct varlena *retval;
    int status;
    mdasC_sql_result_struct  *myresult = NULL;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    remoteFlag = getMdasEnabledHost (NULL, &hostTabPtr);

    if (remoteFlag < 0) {
	status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
    	status = getSRBUserInfo (catalog, &myresult, rowsWanted);
#endif
    } else {
        status = remoteGetPrivUsers (catalog, &myresult, rowsWanted, 
	 hostTabPtr);
    }

    if (status == 0) {
        retval = sqlResultToVar (myresult, MULTI_ROW);
        freeSqlResult (myresult);
    } else {
	elog (NOTICE, "svrGetPrivUsers: getSRBUserInfo failed. Status = %d",
	  status);
    	retval = (struct varlena *)malloc(VAROUTHDRSZ);
    	VARSIZE(retval) = VAROUTHDRSZ;
    	VAROUTSTAT(retval) = htonl (status);
        if (myresult != NULL)
            free (myresult);
    }

    return retval;
}

int 
svrModifyRescInfo (int catType, struct varlena *vRescName, int retractionType,
struct varlena *vDataValue1, struct varlena *vDataValue2,
struct varlena *vDataValue3, struct varlena *vDataValue4)
{
    char *rescName;
    char *dataValue1;
    char *dataValue2;
    char *dataValue3;
    char *dataValue4;
    int retVal;

    if (ProxyUser->privUserFlag <= 0) {
        elog (NOTICE, 
	  "svrModifyRescInfo: User has no privilege for this operation");
        return (AUTH_ERR_PROXY_NOPRIV);
    }

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }
    rescName = varToStr (vRescName);
    dataValue1 = varToStr (vDataValue1);
    dataValue2 = varToStr (vDataValue2);
    dataValue3 = varToStr (vDataValue3);
    dataValue4 = varToStr (vDataValue4);

    retVal = _svrModifyRescInfo (catType, NULL, rescName, retractionType,
      dataValue1, dataValue2, dataValue3, dataValue4);

    free (rescName);
    free (dataValue1);
    free (dataValue2);
    free (dataValue3);
    free (dataValue4);

    return (retVal);
}

int
_svrModifyRescInfo (int catType, char *mcatName, char *rescName, 
int retractionType,
char *dataValue1, char *dataValue2, char *dataValue3, char *dataValue4)
{
    int retVal;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    /* Do the modify_dataset_info */

    /* XXXXXXX - does not know the mcat name */
    remoteFlag = getMdasEnabledHost (mcatName, &hostTabPtr);

    if (remoteFlag < 0) {
        retVal = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
	char *userName, *domainName;
	char resDeletedStr[MAX_TOKEN];
	int sizeMB;

	sizeMB = atoi (dataValue1);
	if (retractionType == R_INSERT_FREE_SPACE && 
	 sizeMB != RES_RESOURCE_DELETED && sizeMB != RES_RESOURCE_RESTART) {
	    userName = ThisUser->userName; 
	    domainName = ThisUser->domainName;
	} else {
	    userName = ClientUser->userName;
	    domainName = ClientUser->domainName;
	}
        retVal = modify_resource_info (catType, rescName, 
          userName, domainName,
	  retractionType, dataValue1, dataValue2, dataValue3, dataValue4);    
        if (retVal >= 0)
            commit_db2_interaction (MDAS_CAT_COMMIT);
#endif
    } else {
        if ((retVal = remoteConnect (hostTabPtr)) < 0) {
            return retVal;
        }

        /* Do a client call to the remote host */

        retVal = srbModifyRescInfo (hostTabPtr->conn, catType, rescName,
          retractionType, dataValue1, dataValue2, dataValue3, dataValue4);

        if (retVal < 0) {       /* Failure */
            elog (NOTICE, "srbModifyRescInfo failed: %s",
              clErrorMessage(hostTabPtr->conn));
        }
    }

    return retVal;
}

/*
 * svrRegisterLocation - Register Location
 *
 * Input - struct varlena *vLocName - location name
 *		  struct varlena *vFullAddr - full address
 *      	  struct varlena *vParentLoc - parent location
 *		  struct varlena *vServerUser - server user
 *                struct varlena *vServerUserDomain - server user domain
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */
int
svrRegisterLocation(   struct varlena *vLocName, 
 		 struct varlena *vFullAddr, 
		 struct varlena *vParentLoc,
		 struct varlena *vServerUser,
                 struct varlena *vServerUserDomain)
{

    int status;

    char *locName;
    char *fullAddr;
    char *parentLoc;
    char *serverUser;
    char *serverUserDomain;

    locName = varToStr (vLocName);
    fullAddr = varToStr (vFullAddr);
    parentLoc = varToStr (vParentLoc);
    serverUser = varToStr (vServerUser);
    serverUserDomain = varToStr (vServerUserDomain);

    status = _svrRegisterLocation(locName, fullAddr, parentLoc, serverUser,
				  serverUserDomain);

    free (locName);
    free (fullAddr);
    free (parentLoc);
    free (serverUser);
    free (serverUserDomain);

    return status;
}

/*
 * See svrRegisterLocation above for description.
 */
int
_svrRegisterLocation( char *locName, 
 		 char *fullAddr, 
		 char *parentLoc,
		 char *serverUser,
                 char *serverUserDomain)
{
    int status;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    if (ClientUser->privUserFlag != 1) {
        elog (NOTICE, 
          "svrRegisterLocation: User has no privilege for this operation");
	return (AUTH_ERR_PROXY_NOPRIV);
    }

    /* XXXXXX - potential problem. Does not know mcatHost */
    remoteFlag = getMdasEnabledHost (NULL, &hostTabPtr);

    if (remoteFlag < 0) {
        status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        char *userName;
        char *userAuth;
        char *userDomain;

        userName = ClientUser->userName;
        userAuth = "";
        userDomain = ClientUser->domainName;

       status = registerLocationInfo(locName, fullAddr, 
		parentLoc, serverUser, serverUserDomain,
		userName, userDomain,  userAuth);
       if (status == 0) {
	  commit_db2_interaction (MDAS_CAT_COMMIT);
       }
       else {
	  commit_db2_interaction (MDAS_CAT_ROLLBACK);
       }
#else
       status = CLI_FUNC_NOT_SUPPORTED;
#endif
    } 
    else {
      status = remoteRegisterLocation(locName, fullAddr, parentLoc,
		    serverUser, serverUserDomain, hostTabPtr);
    }

    if (status != 0) {
        elog( NOTICE, 
	      "registerLocation: registerLocationInfo Error: %i",status);
    }

    return status;
}


/*
 * svrIngestToken - Ingest Token
 *
 * Input - struct varlena *vTypeName - type name
 *	   struct varlena *vNewValue - new value
 *         struct varlena *vParentValue - parent value
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */

int
svrIngestToken(   struct varlena *vTypeName, 
		       struct varlena *vNewValue, 
		       struct varlena *vParentValue)
{

    int status;

    char *typeName;
    char *newValue;
    char *parentValue;

    typeName = varToStr (vTypeName);
    newValue = varToStr (vNewValue);
    parentValue = varToStr (vParentValue);

    status = _svrIngestToken(typeName, newValue, parentValue);

    free (typeName);
    free (newValue);
    free (parentValue);

    return status;
}

/*
 * See svrIngestToken above for description.
 */
int
_svrIngestToken(char *typeName, 
		char *newValue, 
		char *parentValue)
{
    int status;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    /**** RAJA REMOVED Nov 9, 2004  checking is done internally for
     admin user and domain user
 
    if (ClientUser->privUserFlag != 1) {
        elog (NOTICE, 
          "svrIngestToken: User has no privilege for this operation");
	return (AUTH_ERR_PROXY_NOPRIV);
    }
    ****/
    /* XXXXXXX - potential problem. Does not know mcatName */
    remoteFlag = getMdasEnabledHost (NULL, &hostTabPtr);

    if (remoteFlag < 0) {
        status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        char *userName;
        char *userAuth;
        char *userDomain;
        userName = ClientUser->userName;
        userAuth = "";
        userDomain = ClientUser->domainName;

       status = insertTokenInfo(typeName, newValue, parentValue,
				 userName, userDomain,  userAuth);
       if (status == 0) {
	  commit_db2_interaction (MDAS_CAT_COMMIT);
       }
       else {
	  commit_db2_interaction (MDAS_CAT_ROLLBACK);
       }
#else
       status = CLI_FUNC_NOT_SUPPORTED;
#endif
    } 
    else {
       status = remoteInsertTokenInfo(typeName, newValue, parentValue,
	hostTabPtr);
    }

    if (status != 0) {
        elog( NOTICE, 
	      "insertToken: insertTokenInfo Error: %i",status);
    }

    return status;
}

/*
 * svrRegisterResource
 *
 * Input -  struct varlena *vRescName - Resource Name
 *	    struct varlena *vRescType - Resource Type 
 *          struct varlena *vLocation - Resource Location
 *          struct varlena *vPhyPath  - Resource Physical Path
 *          struct varlena *vClass    - Resource Class name
 *         
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */
int
svrRegisterResource (struct varlena *vRescName,
				struct varlena *vRescType,
				struct varlena *vLocation,
				struct varlena *vPhyPath,
				struct varlena *vClass,
				int size)
{
    int status;

    char *rescName;
    char *rescType;
    char *location;
    char *phyPath;
    char *class;

    rescName = varToStr (vRescName);
    rescType = varToStr (vRescType);
    location = varToStr (vLocation);
    phyPath = varToStr (vPhyPath);
    class   = varToStr (vClass);

    status = _svrRegisterResource(rescName, rescType, location, 
				  phyPath, class, size);

    free (rescName);
    free (rescType);
    free (phyPath);
    free (class);

    return status;
}

/*
 * See svrRegisterResource above for description.
 */
int
_svrRegisterResource(   char *rescName, char *rescType, char *location, 
			char *phyPath, char *class, int size)
{
    char *userName;
    char *userAuth;
    char *userDomain;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    int status;

    if (ClientUser->privUserFlag != 1) {
        elog (NOTICE, 
          "svrRegisterResource: User has no privilege for this operation");
	return (AUTH_ERR_PROXY_NOPRIV);
    }
    userName = ClientUser->userName;
    userAuth = "";
    userDomain = ClientUser->domainName;

    /* XXXXXXX potential problem. does not know mcatName */
    remoteFlag = getMdasEnabledHost (NULL, &hostTabPtr);

    if (remoteFlag < 0) {
        status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
       status = registerResourceInfo(rescName, rescType, location,
	          userName, userDomain,  userAuth, 
                  phyPath, class, size);
       if (status == 0) {
	  commit_db2_interaction (MDAS_CAT_COMMIT);
       }
       else {
	  commit_db2_interaction (MDAS_CAT_ROLLBACK);
       }
#else
       status = CLI_FUNC_NOT_SUPPORTED;
#endif
    } 
    else {
       status = remoteRegisterResourceInfo(rescName, rescType, 
                  location,
	          userName, userDomain,  userAuth, phyPath, class, size,
		  hostTabPtr);
    }


    if (status != 0) {
        elog( NOTICE, 
	      "RegisterResource: registerResourceInfo Error: %i",
	      status);
    }

    return status;   
}


/*
 * svrRegisterLogicalResource
 *
 * Input -  struct varlena *vLogRescName - Logical resource name
 *	    struct varlena *vRescType - Resource Type 
 *          struct varlena *vPhysRescName - Physical resource name
 *          struct varlena *vPhysPath - Physical path
 *         
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */

int
svrRegisterLogicalResource(   struct varlena *vLogRescName, 
		       struct varlena *vRescType, 
		       struct varlena *vPhysRescName,
		       struct varlena *vPhysPath)
{

    int status;

    char *logRescName;
    char *rescType;
    char *physRescName;
    char *physPath;

    logRescName = varToStr (vLogRescName);
    rescType = varToStr (vRescType);
    physRescName = varToStr (vPhysRescName);
    physPath = varToStr (vPhysPath);

    status = _svrRegisterLogicalResource(logRescName,
              rescType, physRescName, physPath);

    free (logRescName);
    free (rescType);
    free (physRescName);
    free (physPath);

    return status;
}

/*
 * See svrRegisterLogicalResource above for description.
 */
int
_svrRegisterLogicalResource(   char *logRescName, 
		       char *rescType, 
		       char *physRescName,
		       char *physPath)
{
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    int status;

    if (ClientUser->privUserFlag != 1) {
        elog (NOTICE, 
       "svrRegisterLogicalResource: User has no privilege for this operation");
	return (AUTH_ERR_PROXY_NOPRIV);
    }

    /* XXXXXXX potential problem. does not know mcatName */
    remoteFlag = getMdasEnabledHost (NULL, &hostTabPtr);

    if (remoteFlag < 0) {
        status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        char *userName;
        char *userAuth;
        char *userDomain;
        userName = ClientUser->userName;
        userAuth = "";
        userDomain = ClientUser->domainName;

         status = registerLogicalResourceInfo(logRescName, rescType, 
			   physRescName,
			   userName, userDomain,  userAuth, physPath);
	 if (status == 0) {
	    commit_db2_interaction (MDAS_CAT_COMMIT);
	 }
	 else {
	    commit_db2_interaction (MDAS_CAT_ROLLBACK);
	 }

#else
	 status = CLI_FUNC_NOT_SUPPORTED;
#endif
    } 
    else {
         status = remoteRegisterLogicalResourceInfo (logRescName, rescType, 
		    physRescName,  physPath, hostTabPtr);
    }

    if (status != 0) {
        elog( NOTICE, 
	      "RegisterLogicalResource: registerLogicalResourceInfo Error: %i",
	      status);
    }
    return (0);
}

/*
 * svrRegisterReplicateResourceInfo
 *
 * Input -  struct varlena *vPhysRescName - Physical resource name
 *	    struct varlena *vRescType - Resource Type 
 *          struct varlena *vOldLogicalRescName - Old logical resource name
 *          struct varlena *vIndefaultPath - Indefault path
 *         
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */
int
svrRegisterReplicateResourceInfo(   struct varlena *vPhysRescName, 
		       struct varlena *vRescType, 
		       struct varlena *vOldLogicalRescName,
		       struct varlena *vIndefaultPath)
{

    int status;

    char *physRescName;
    char *rescType;
    char *oldLogRescName;
    char *indefaultPath;

    physRescName = varToStr (vPhysRescName);
    rescType = varToStr (vRescType);
    oldLogRescName = varToStr (vOldLogicalRescName);
    indefaultPath = varToStr (vIndefaultPath);

    status = _svrRegisterReplicateResourceInfo(physRescName, rescType, 
				      oldLogRescName, indefaultPath);

    free (physRescName);
    free (rescType);
    free (oldLogRescName);
    free (indefaultPath);

    return status;
}

/*
  See svrRegisterReplicateResourceInfo (above) for description.
 */
int
_svrRegisterReplicateResourceInfo(   char *physRescName, 
		       char *rescType, 
		       char *oldLogRescName,
		       char *indefaultPath)
{
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    int status;

    if (ClientUser->privUserFlag != 1) {
        elog (NOTICE, 
          "svrRegisterReplicateResourceInfo: User has no privilege for this operation");
	return (AUTH_ERR_PROXY_NOPRIV);
    }

    /* XXXXXXXX potential problem - don't know mcatName */
    remoteFlag = getMdasEnabledHost (NULL, &hostTabPtr);

    if (remoteFlag < 0) {
        status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        char *userName;
        char *userAuth;
        char *userDomain;

        userName = ClientUser->userName;
        userAuth = "";
        userDomain = ClientUser->domainName;

         if (strcmp(indefaultPath, "compound") == 0) {
	   /* oldLogRescName is actually the Compound Resource name */
            status = registerCompoundResourceInfo(oldLogRescName, 
					     physRescName, 
		     userName, userDomain,  userAuth);
	 }
	 else {
            status = registerReplicateResourceInfo(physRescName, rescType, 
					     oldLogRescName,
		     userName, userDomain,  userAuth, indefaultPath);
	 }
	 if (status == 0) {
            commit_db2_interaction (MDAS_CAT_COMMIT);
	 }
	 else {
	    commit_db2_interaction (MDAS_CAT_ROLLBACK);
	 }
#else
        status = CLI_FUNC_NOT_SUPPORTED;
#endif
    } 
    else {
        status = remoteRegisterReplicateResourceInfo (physRescName, rescType, 
		     oldLogRescName,  indefaultPath, hostTabPtr);
    }

    if (status != 0) {
        elog( NOTICE, 
	      "RegisterReplicateResourceInfo: registerReplicateResourceInfo Error: %i",
	      status);
    }
    return status;
}

/*
 * svrDeleteValue - Delete Value
 *
 * Input - int valueType - type of value being deleted
 *	   struct varlena *vDeleteValue - value (name) being deleted
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */

int
svrDeleteValue(   int valueType,
		  struct varlena *vDeleteValue)
{

    int status;

    char *deleteValue;

    deleteValue = varToStr (vDeleteValue);

    status = _svrDeleteValue(valueType, deleteValue);

    free (deleteValue);

    return status;
}

/*
 * See svrDeleteValue above for description.
 */
int
_svrDeleteValue(int valueType,
		char *deleteValue)
{
    int status;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    /**** RAJA REMOVED Nov 9, 2004  checking is done internally for
	  admin user and domain user
    if (ClientUser->privUserFlag != 1) {
        elog (NOTICE, 
          "svrDeleteValue: User has no privilege for this operation");
	return (AUTH_ERR_PROXY_NOPRIV);
    }
    ****/

    /* XXXXXX potential problem - don't know mcatName */
    remoteFlag = getMdasEnabledHost (NULL, &hostTabPtr);

    if (remoteFlag < 0) {
        status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
       status = deleteValueInfo(valueType, deleteValue, 
				ClientUser->userName,
				ClientUser->domainName);
       if (status == 0) {
	  commit_db2_interaction (MDAS_CAT_COMMIT);
       }
       else {
	  commit_db2_interaction (MDAS_CAT_ROLLBACK);
       }
#else
       status = CLI_FUNC_NOT_SUPPORTED;
#endif
    } 
    else {
       status = remoteDeleteValue(valueType, deleteValue, hostTabPtr);
    }

    if (status != 0) {
        elog( NOTICE, 
	      "deleteValue: deleteValueInfo Error: %i",status);
    }

    return status;
}

int
svrModifyZone (int catType, struct varlena *vZoneName,
struct varlena *vDataValue1, struct varlena *vDataValue2,
struct varlena *vDataValue3, struct varlena *vDataValue4, 
struct varlena *vDataValue5, int actionType)
{
    char *zoneName;
    char *dataValue1;
    char *dataValue2;
    char *dataValue3;
    char *dataValue4;
    char *dataValue5;
    int retVal;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    zoneName = varToStr (vZoneName);
    dataValue1 = varToStr (vDataValue1);
    dataValue2 = varToStr (vDataValue2);
    dataValue3 = varToStr (vDataValue3);
    dataValue4 = varToStr (vDataValue4);
    dataValue5 = varToStr (vDataValue5);


    retVal = _svrModifyZone (catType, zoneName, dataValue1, 
    dataValue2, dataValue3, dataValue4, dataValue5, actionType);

    free (zoneName);
    free (dataValue1);
    free (dataValue2);
    free (dataValue3);
    free (dataValue4);
    free (dataValue5);

    return retVal;
}

int
_svrModifyZone (int catType, char *zoneName, char *dataValue1,
char *dataValue2, char *dataValue3, char *dataValue4, char *dataValue5, 
int actionType)
{
    int retVal;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    /* Do a modify_zone_info */
    if (ClientUser->privUserFlag != 1) {
        elog (NOTICE, 
          "svrModifyZone: User has no privilege for this operation");
	return (AUTH_ERR_PROXY_NOPRIV);
    }

    remoteFlag = getMdasEnabledHost (NULL, &hostTabPtr);

    if (remoteFlag < 0) {
	retVal = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        retVal = modify_zone_info (catType, ClientUser->userName, 
	  ClientUser->domainName,
	  zoneName, dataValue1, dataValue2, dataValue3,
	  dataValue4, dataValue5, actionType);
	if (retVal == 0)
            commit_db2_interaction (MDAS_CAT_COMMIT);
#endif
    } else {
        retVal = remoteModifyZone (catType, zoneName,
          dataValue1, dataValue2, dataValue3, dataValue4, dataValue5, 
	  actionType, hostTabPtr);
    }

    return retVal;
}

int
svrModifyUserNonPriv (int catType, struct varlena *vUserNameDomain,
struct varlena *vDataValue1, struct varlena *vDataValue2,
struct varlena *vDataValue3, struct varlena *vDataValue4, 
struct varlena *vDataValue5, int actionType)
{
    char *userNameDomain;
    char *dataValue1;
    char *dataValue2;
    char *dataValue3;
    char *dataValue4;
    char *dataValue5;
    int retVal;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    userNameDomain = varToStr (vUserNameDomain);
    dataValue1 = varToStr (vDataValue1);
    dataValue2 = varToStr (vDataValue2);
    dataValue3 = varToStr (vDataValue3);
    dataValue4 = varToStr (vDataValue4);
    dataValue5 = varToStr (vDataValue5);


    retVal = _svrModifyUserNonPriv (catType, userNameDomain, dataValue1, 
    dataValue2, dataValue3, dataValue4, dataValue5, actionType);

    free (userNameDomain);
    free (dataValue1);
    free (dataValue2);
    free (dataValue3);
    free (dataValue4);
    free (dataValue5);

    return retVal;
}

int
_svrModifyUserNonPriv (int catType, char *userNameDomain, char *dataValue1,
char *dataValue2, char *dataValue3, char *dataValue4, char *dataValue5, 
int actionType)
{
    int retVal;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    remoteFlag = getMdasEnabledHost (NULL, &hostTabPtr);

    if (remoteFlag < 0) {
	retVal = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        char *userNameReg;
        char *userDomainReg;

        userNameReg = ClientUser->userName;
        userDomainReg = ClientUser->domainName;

        retVal = modify_user_info_nonpriv (catType, userNameReg, userDomainReg,
	  userNameDomain, dataValue1, dataValue2, dataValue3,
	  dataValue4, dataValue5, actionType);
	if (retVal == 0)
            commit_db2_interaction (MDAS_CAT_COMMIT);
#endif
    } else {
        retVal = remoteModifyUserNonPriv (catType, userNameDomain,
          dataValue1, dataValue2, dataValue3, dataValue4, dataValue5, 
	  actionType, hostTabPtr);
    }

    return retVal;
}


int
svrModifyResource (int catType, struct varlena *vResourceName,
struct varlena *vDataValue1, struct varlena *vDataValue2,
struct varlena *vDataValue3, struct varlena *vDataValue4, 
 int actionType)
{
    char *resourceName;
    char *dataValue1;
    char *dataValue2;
    char *dataValue3;
    char *dataValue4;
    int retVal;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    resourceName = varToStr (vResourceName);
    dataValue1 = varToStr (vDataValue1);
    dataValue2 = varToStr (vDataValue2);
    dataValue3 = varToStr (vDataValue3);
    dataValue4 = varToStr (vDataValue4);


    retVal = _svrModifyResource (catType, resourceName, dataValue1, 
    dataValue2, dataValue3, dataValue4,  actionType);

    free (resourceName);
    free (dataValue1);
    free (dataValue2);
    free (dataValue3);
    free (dataValue4);

    return retVal;
}

int
_svrModifyResource (int catType, char *resourceName, char *dataValue1,
char *dataValue2, char *dataValue3, char *dataValue4, int actionType)
{
    int retVal;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    /* Do a modify_resource_info */
    if (ClientUser->privUserFlag != 1) {
        elog (NOTICE, 
          "svrModifyResource: User has no privilege for this operation");
	return (AUTH_ERR_PROXY_NOPRIV);
    }

    remoteFlag = getMdasEnabledHost (NULL, &hostTabPtr);

    if (remoteFlag < 0) {
	retVal = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        retVal = modify_resource_info (catType, resourceName, 
				       ClientUser->userName, 
				       ClientUser->domainName,actionType,
				       dataValue1, dataValue2, dataValue3,
				       dataValue4);
	if (retVal == 0)
            commit_db2_interaction (MDAS_CAT_COMMIT);
#endif
    } else {
        retVal = remoteModifyResource (catType, resourceName,
          dataValue1, dataValue2, dataValue3, dataValue4, 
	  actionType, hostTabPtr);
    }

    return retVal;
}
