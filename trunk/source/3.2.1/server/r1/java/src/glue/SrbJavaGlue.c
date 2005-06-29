/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/************************************************************************/
/*                                                                      */
/*  WHO:    Arcot Rajasekar   (STUBS)                                   */
/*          John L. Moreland  (GLUE)                                    */
/*                                                                      */
/*  WHAT:   SrbJavaGlue.c                                               */
/*                                                                      */
/*  WHY:    Provides Java/C interface glue for the SRB client library.  */
/*                                                                      */
/*  WHERE:  San Diego Supercomputer Center (SDSC)                       */
/*                                                                      */
/*  WHEN:   Thu Oct 23 14:50:35 PDT 1997 (ORIGINAL)                     */
/*          Wed Nov 19 16:02:23 PST 1997 (CURRENT)                      */
/*                                                                      */
/*  HOW:    ANSI C                                                      */
/*                                                                      */
/************************************************************************/


/************************************************************************/
/******************************  INCLUDES  ******************************/
/************************************************************************/

#include <scommands.h>
/* #include <mdasGlobalsExtern.h> */
#include <clAuthExtern.h>

#include <srbErrno.h>

#ifdef _WIN32
#include "SrbJavaGlue2.h"
#else
#include <jni.h>
#include "edu_sdsc_SrbGlue_SrbJavaGlue.h"
#endif /* WIN32 */

struct hostElement *LocalHostHead = NULL;
struct hostElement *RemoteHostHead = NULL;

int ElogMsgToClient=0;
mdasC_sql_result_struct *SrbUserList = NULL;
int TicketUserFlag;



#ifdef MY_DEBUG
FILE *dbgFd;
#endif

#define MY_MAX_ROW	400

#ifdef _WIN32
#define CACHE_DIR_NAME "srb/cache"
#else
#define CACHE_DIR_NAME  ".srb/cache"
#endif /* WIN32 */

/* The default Logical Resource type */
#define DEF_LOG_RESC_TYPE	"logical"
#define DEF_COMP_RESC_TYPE      "compound"

/* The default User Group type */
#define DEF_GROUP_TYPE	"group"

char LocalCacheDir[MAX_TOKEN];

/* struct for mapping selval index to the mdasC_sql_result_struct value */
struct attriToResultMap {
    int selInx;
    char *resultVal;
};

/************************************************************************/
/******************************  GLOBALS  *******************************/
/************************************************************************/
struct qBuf {
    char fileList[ MY_MAX_ROW ][ MAX_DATA_SIZE ];
    char resList[ MY_MAX_ROW ][ MAX_DATA_SIZE ];
    mdasC_sql_result_struct dataResult;
    mdasC_sql_result_struct collResult;
    mdasC_sql_result_struct genResult;
};

#define MAX_CONN	128

#ifdef _WIN32
srbConn * g_conn[MAX_CONN]={NULL};
struct qBuf *g_qBuf[MAX_CONN] = {NULL};
int g_connID[MAX_CONN]={0};
#else
srbConn * g_conn[MAX_CONN] = {NULL};
struct qBuf *g_qBuf[MAX_CONN] = {NULL};
int g_connID[MAX_CONN] = {0};
#endif /* WIN32 */

int AuthSchemeInx = -1;	/* just to make the glue library happy */	
userInfo *ClientUser = NULL; 
userInfo *ProxyUser = NULL;

#ifdef foo
char g_fileList[ MY_MAX_ROW ][ MAX_DATA_SIZE ];
char g_resList[ MY_MAX_ROW ][ MAX_DATA_SIZE ];
mdasC_sql_result_struct gDataResult;
mdasC_sql_result_struct gCollResult;
mdasC_sql_result_struct gGenResult;
#endif

#ifdef _WIN32
static int nConnStatus;
#endif

struct attriToResultMap collMap[] = {
    {DATA_GRP_NAME, 0}
};

struct attriToResultMap dataMap[] = {
    {DATA_NAME, NULL},
    {DATA_GRP_NAME, NULL},
    {SIZE, NULL},
    {DATA_TYP_NAME, NULL},    
    {DATA_OWNER, NULL},
    {REPL_TIMESTAMP, NULL},
    {DATA_REPL_ENUM, NULL},
/*
    {CONTAINER_REPL_ENUM, NULL},
*/
    {PHY_RSRC_NAME, NULL},
    {CONTAINER_NAME, NULL},
    {IS_DIRTY, NULL}
};

int nCollMap = (sizeof(collMap) / sizeof(struct attriToResultMap));
int nDataMap = (sizeof(dataMap) / sizeof(struct attriToResultMap));
int nSubDataMap = 4;

/* attriToResultMap for general query */
struct attriToResultMap genResultMap[MAX_DCS_NUM];  
int nGenAttri = 0;	/* Number of attri for the current general query */
int nGenRowCnt = 0;	/* Number of row for the current general query */

extern char * getFromResultStruct(mdasC_sql_result_struct *result,
                              char *tab_name, char *att_name);
extern void trylocaldatatype(char *datatypename, char *pathName);

/************************************************************************/
/*******************************  STUBS  ********************************/
/************************************************************************/


/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
int getConnID ()
{
    int i;

    for (i = 0; i < MAX_CONN; i++) {
	if (g_connID[i] == 0) {
	    g_connID[i] = 1;
	    g_qBuf[i] = (struct qBuf *) malloc (sizeof (struct qBuf));
	    return i;
	}
    }
    return (SYS_ERR_DESC_OVERFLOW);
}

int relConnID (int connID)
{
    if (g_conn[connID] != NULL) {
	clFinish (g_conn[connID]);
	g_conn[connID] = NULL;
	if (g_qBuf[connID] != NULL) {
	    free (g_qBuf[connID]);
	    g_qBuf[connID] = NULL;
	}
    }
    g_connID[connID] = 0;
    return (0);
}

int clConnectJ( char * srbHost, char * srbPort, char * srbAuth, char * srbUser,
char * srbDomain )
{
	int clStat;
	int connID;

#ifdef DEBUG
fprintf( stderr, "DEBUG: SrbJavaGlue.c clConnectJ: srbHost = '%s'\n", srbHost );
fprintf( stderr, "DEBUG: SrbJavaGlue.c clConnectJ: srbPort = '%s'\n", srbPort );
fprintf( stderr, "DEBUG: SrbJavaGlue.c clConnectJ: srbAuth = '%s'\n", srbAuth );
fprintf( stderr, "DEBUG: SrbJavaGlue.c clConnectJ: srbUser = '%s'\n", srbUser );
fprintf( stderr, 
 "DEBUG: SrbJavaGlue.c clConnectJ: srbDomain = '%s'\n", srbDomain );
#endif

#ifdef MY_DEBUG
	dbgFd = fopen ("./JNIDebug", "w+");
#endif

#ifdef _WIN32
	if(g_conn)
		return g_connID;
#endif

	if ((connID = getConnID ()) < 0) {
	    fprintf( stderr, "clConnectJ: getConnID error, status = %d\n", 
	    connID);
	    return (connID);
	}

	g_conn[connID] = srbConnect
	 (srbHost, srbPort, srbAuth, srbUser, srbDomain, NULL, NULL );

#ifdef DEBUG
fprintf( stderr, "DEBUG: SrbJavaGlue.c clConnectJ: g_conn = %x\n", 
 g_conn[connID] );
#endif

	clStat = clStatus( g_conn[connID]);

#ifdef DEBUG
	fprintf( stderr, "DEBUG: SrbJavaGlue.c clConnectJ: clStat = %d\n", clStat );
#endif

	if ( clStat != 0 )
	{
		fprintf( stderr, "SrbJavaGlue.c: Connection to srbMaster failed.\n" );
		fprintf( stderr, "SrbJavaGlue.c: %s", 
		 clErrorMessage( g_conn[connID]) );
		relConnID (connID);

#ifdef _WIN32
		nConnStatus = clStat;
#endif

		return clStat;
	}
#ifdef _WIN32
	/* In win32,the cahce directory is created before clConnect().
	   seed a rand number */
	srand(time(NULL));
#else
	strcpy(LocalCacheDir, getenv("HOME"));
	strcat (LocalCacheDir, "/");
	strcat (LocalCacheDir, CACHE_DIR_NAME);
	clStat = mkdir (LocalCacheDir, 0750);
#endif

	return(connID);
}


/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
void clFinishJ( int cid )
{
#ifdef DEBUG
	fprintf( stderr, "DEBUG: SrbJavaGlue.c clFinishJ: cid = %d\n", cid );
#endif

	relConnID (cid);
}

/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
int srbObjCopyNewJ( int cid, char * OldCollectionName, char * OldObjectName,
		    char * NewCollectionName,  char * NewObjectName,
		    char * NewResourceName )
{
	int  i, j, e_num;
	mdasC_sql_result_struct myresult;
	char *rname;
	char *pname;
	char qval[MAX_DCS_NUM][MAX_TOKEN];
	int  selval[MAX_DCS_NUM];
	srb_long_t datasize;
	char datatype[MAX_TOKEN];
	int  in_fd, out_fd;

	for ( i = 0; i < MAX_DCS_NUM; i++ )
	{
		sprintf( qval[i], "" );
		selval[i] = 0;
	}   

       selval[DATA_TYP_NAME] = 1;
       selval[SIZE] = 1;
       sprintf(qval[DATA_NAME], " = '%s'",  OldObjectName);
       sprintf(qval[DATA_GRP_NAME], " = '%s'",  OldCollectionName);
       j = srbGetDataDirInfo(g_conn[cid], MDAS_CATALOG, qval, selval, &myresult,
	MY_MAX_ROW);
       if (j != 0)
	 {   /* error */
	   fprintf(stderr,"Scp: error for '%s/%s' at srbGetDataDirInfo:%i\n",
                    OldCollectionName, OldObjectName, j);
            return(j);
	 }
        rname = (char *) getFromResultStruct(&myresult,
                         dcs_tname[DATA_TYP_NAME], dcs_aname[DATA_TYP_NAME]);
        pname = (char *) getFromResultStruct(&myresult,
                         dcs_tname[SIZE], dcs_aname[SIZE]);
        strcpy(datatype, rname);
        datasize = strtoll (pname, 0, 0);
        free (rname);
        free (pname);
       in_fd = srbObjOpen (g_conn[cid], OldObjectName, O_RDONLY, OldCollectionName);
        if (in_fd < 0)  
          {   /* error */
            fprintf(stderr,
                    "Scp: error for  object '%s/%s' at srbObjOpen: %i\n",
                    OldCollectionName, OldObjectName, in_fd);
            return(in_fd);
          }
        out_fd = srbObjCreate (g_conn[cid], MDAS_CATALOG, NewObjectName,
                               datatype, NewResourceName,NewCollectionName, 
                               "", datasize);
       if (out_fd < 0)  
          {   /* error */
            fprintf(stderr, "Scp: create error for object %s/%s in %s. Status=%i\n", 
                    OldCollectionName, OldObjectName, NewResourceName, out_fd);
            srbObjClose (g_conn[cid], in_fd);
            return(out_fd);
          }
       j = srbObjProxyOpr (g_conn[cid], OPR_COPY, in_fd, out_fd, 0, 0,
         NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        srbObjClose (g_conn[cid], out_fd);
        srbObjClose (g_conn[cid], in_fd);
        if (j < 0) {   /* error */
            fprintf(stderr, "Scp: Possible error in Copy: status = %i\n", j);
            return(j);
        }
  	return (0);
}

int srbImportToOldObjJ (int cid, int catType, char * objID, int replNum, 
char * collectionName, char *resource, char * localFile, char * container) {
    char tmpPath[MAX_TOKEN];
    int status;
    srbPathName srbPathName;
    int flagval = 0;

    if (replNum >= 0)
        sprintf (tmpPath, "%s&COPY=%i", objID, replNum);
    else
        sprintf (tmpPath, "%s", objID);

    srbPathName.inpArgv = localFile;

    /* fill in size, etc */
    status = chkFileName (&srbPathName);

    if (status < 0)
        return status;

    flagval |= F_FLAG;
    if (container != NULL && container[0] != '\0') {
        flagval |= c_FLAG;
    }

    status = fileToDataCopy (g_conn[cid], catType, flagval, DATANAME_T,
     &srbPathName, collectionName, tmpPath, "", "", resource, container, -1);

    /* clean up */
    free (srbPathName.outArgv);

    return status;
}

int srbImportToNewObjJ (int cid, int catType, char * objID, 
char * collectionName, char * resourceName, char * localFile, char *container) 
{
    int status;
    srbPathName srbPathName;
    int flagval = 0;

    srbPathName.inpArgv = localFile;

    /* fill in size, etc */
    status = chkFileName (&srbPathName);

    if (status < 0)
	return status;

    if (container != NULL && container[0] != '\0') {
        flagval |= c_FLAG;
    }

    status = fileToDataCopy (g_conn[cid], catType, flagval, -1, &srbPathName,
      collectionName, objID, "", "", resourceName, container, -1);

    /* clean up */
    free (srbPathName.outArgv);

    return status;
}

int srbImportDirJ (int cid, int catType, char * collectionName, 
char * resourceName, char * localDir, char *container)
{
    int status;
    srbPathName srbPathName;
    int flagval = 0;

    srbPathName.inpArgv = localDir;

    /* fill in size, etc */
    status = chkFileName (&srbPathName);

    if (status < 0)
        return status;

    if (container != NULL && container[0] != '\0') {
        flagval |= c_FLAG;
    }

    status = dirToCollCopy (g_conn[cid], catType, flagval, &srbPathName,
      collectionName, resourceName, container, "");

    /* clean up */
    free (srbPathName.outArgv);

    return status;
}

int srbDataToFileCopyJ (int cid, int catType, char * objID, int replNum,
char * collectionName, char *targFile, int forceFlag) {
    int status;
    mdasC_sql_result_struct myresult;
    int flagval;

    /* since dataToFileCopy expect a mdasC_sql_result_struct as input, have to
     * fake one.
     */

    myresult.result_count = 3;
    myresult.row_count = 1;
    myresult.sqlresult[0].tab_name = dcs_tname[DATA_GRP_NAME];
    myresult.sqlresult[0].att_name = dcs_aname[DATA_GRP_NAME];
    myresult.sqlresult[0].values = (mdasC_handle)  collectionName;
    myresult.sqlresult[1].tab_name = dcs_tname[DATA_NAME];
    myresult.sqlresult[1].att_name = dcs_aname[DATA_NAME];
    myresult.sqlresult[1].values = (mdasC_handle)  objID;
    myresult.sqlresult[2].tab_name = dcs_tname[SIZE];
    myresult.sqlresult[2].att_name = dcs_aname[SIZE];
    myresult.sqlresult[2].values = (mdasC_handle)  "1";

    if (forceFlag > 0) 
	flagval = F_FLAG;
    else
	flagval = 0;
    status = dataToFileCopy (g_conn[cid], catType, flagval, DATANAME_T,
     &myresult, replNum, targFile, "");

    return status;
}

int srbDataToDirCopyJ (int cid, int catType, char * objID, int replNum,
char * collectionName, char *targDir) {
    int status;
    mdasC_sql_result_struct myresult;

    /* since dataToDirCopy expect a mdasC_sql_result_struct as input, have to
     * fake one.
     */

    myresult.result_count = 3;
    myresult.row_count = 1;
    myresult.sqlresult[0].tab_name = dcs_tname[DATA_GRP_NAME];
    myresult.sqlresult[0].att_name = dcs_aname[DATA_GRP_NAME];
    myresult.sqlresult[0].values = (mdasC_handle)  collectionName;
    myresult.sqlresult[1].tab_name = dcs_tname[DATA_NAME];
    myresult.sqlresult[1].att_name = dcs_aname[DATA_NAME];
    myresult.sqlresult[1].values = (mdasC_handle)  objID;
    myresult.sqlresult[2].tab_name = dcs_tname[SIZE];
    myresult.sqlresult[2].att_name = dcs_aname[SIZE];
    myresult.sqlresult[2].values = (mdasC_handle)  "1";

    status = dataToDirCopy (g_conn[cid], catType, F_FLAG, &myresult,
     replNum, targDir, "");

    return status;
}

int srbCollToDirCopyJ (int cid, int catType, char * collectionName, 
char *targDir) {
    int status;
    mdasC_sql_result_struct myresult;
    int flagval = 0;

    /* since collToDirCopy expect a mdasC_sql_result_struct as input, have to
     * fake one.
     */

    myresult.result_count = 1;
    myresult.row_count = 1;
    myresult.sqlresult[0].tab_name = dcs_tname[DATA_GRP_NAME];
    myresult.sqlresult[0].att_name = dcs_aname[DATA_GRP_NAME];
    myresult.sqlresult[0].values = (mdasC_handle)  collectionName;

#ifdef _WIN32
	flagval = l_FLAG;
#else
    flagval |= F_FLAG|R_FLAG;
#endif

    status = collToDirCopy (g_conn[cid], catType, flagval, -1, &myresult,
     targDir, "");

    return status;
}

int srbCopyToOldObjJ (int cid, int catType, char * objID, int replNum,
char * collectionName, char *targObj, int targReplNum, char *targColl,
char * newResource, char *container) {
    char tmpPath[MAX_TOKEN];
    int status;
    mdasC_sql_result_struct myresult;
    int flagval = 0;

    /* since dataToDataCopy expect a mdasC_sql_result_struct as input, have to
     * fake one.
     */

    myresult.result_count = 3;
    myresult.row_count = 1;
    myresult.sqlresult[0].tab_name = dcs_tname[DATA_GRP_NAME];
    myresult.sqlresult[0].att_name = dcs_aname[DATA_GRP_NAME];
    myresult.sqlresult[0].values = (mdasC_handle)  collectionName;
    myresult.sqlresult[1].tab_name = dcs_tname[DATA_NAME];
    myresult.sqlresult[1].att_name = dcs_aname[DATA_NAME];
    myresult.sqlresult[1].values = (mdasC_handle)  objID;
    myresult.sqlresult[2].tab_name = dcs_tname[SIZE];
    myresult.sqlresult[2].att_name = dcs_aname[SIZE];
    myresult.sqlresult[2].values = (mdasC_handle)  "1";


    flagval |= F_FLAG;
    if (container != NULL && container[0] != '\0') {
	flagval |= c_FLAG;
        sprintf (tmpPath, "%s", targObj);
    } else {
        sprintf (tmpPath, "%s&COPY=%i", targObj, targReplNum);
    }
	

    status = dataToDataCopy (g_conn[cid], catType, flagval, DATANAME_T,
     &myresult, replNum, targColl, tmpPath, newResource, "", container);


    return status;
}

int srbCopyDataToCollJ (int cid, int catType, char * objID, int replNum,
char * collectionName, char *dataType, char *size,
char *targColl, char * newResource, char * container) {
    int status;
    mdasC_sql_result_struct myresult;
    int flagval = 0;
    
    /* since dataToCollCopy expect a mdasC_sql_result_struct as input, have to
     * fake one.
     */

    myresult.result_count = 4;
    myresult.row_count = 1;
    myresult.sqlresult[0].tab_name = dcs_tname[DATA_GRP_NAME];
    myresult.sqlresult[0].att_name = dcs_aname[DATA_GRP_NAME];
    myresult.sqlresult[0].values = (mdasC_handle)  collectionName;
    myresult.sqlresult[1].tab_name = dcs_tname[DATA_NAME];
    myresult.sqlresult[1].att_name = dcs_aname[DATA_NAME];
    myresult.sqlresult[1].values = (mdasC_handle)  objID;
    myresult.sqlresult[2].tab_name = dcs_tname[SIZE];
    myresult.sqlresult[2].att_name = dcs_aname[SIZE];
    myresult.sqlresult[2].values = (mdasC_handle)  size;
    myresult.sqlresult[3].tab_name = dcs_tname[DATA_TYP_NAME];
    myresult.sqlresult[3].att_name = dcs_aname[DATA_TYP_NAME];
    myresult.sqlresult[3].values = (mdasC_handle)  dataType;

    flagval |= F_FLAG;
    if (container != NULL && container[0] != '\0') {
        flagval |= c_FLAG;
    }

    status = dataToCollCopy (g_conn[cid], catType, flagval, &myresult,
     replNum, targColl, newResource, "", container);


    return status;
}

int srbCopyCollToCollJ (int cid, int catType, char * collectionName, 
char *targColl, char * newResource, char *container) {
    int status;
    mdasC_sql_result_struct myresult;
    int flagval = 0;

    /* since collToCollCopy expect a mdasC_sql_result_struct as input, have to
     * fake one.
     */

    myresult.result_count = 1;
    myresult.row_count = 1;
    myresult.sqlresult[0].tab_name = dcs_tname[DATA_GRP_NAME];
    myresult.sqlresult[0].att_name = dcs_aname[DATA_GRP_NAME];
    myresult.sqlresult[0].values = (mdasC_handle)  collectionName;

    flagval |= F_FLAG;
    if (container != NULL && container[0] != '\0') {
        flagval |= c_FLAG;
    }

    status = collToCollCopy (g_conn[cid], catType, flagval, COLLECTION_T,  
     &myresult, targColl, newResource, container);

    return status;
}

/************************************************************************/
/*                                                                      */
/*  Returns <0 if error, else returns number of items in the list.      */
/*                                                                      */
/************************************************************************/
int srbListResourceJ(int cid)
{
	int	i, j, k;
	mdasC_sql_result_struct myresult;
	int clStat;
	char qval[ MAX_DCS_NUM ][ MAX_TOKEN ];
	int  selval[ MAX_DCS_NUM ];
	char * rname, *origRname;

	for ( i = 0; i < MAX_DCS_NUM; i++ )
	{
		sprintf( qval[i], "" );
		selval[i] = 0;
	}   
	sprintf( qval[RSRC_NAME], " not like  '%%$deleted%%'" );
	selval[RSRC_NAME] = 1;
	j = srbGetDataDirInfo( g_conn[cid], MDAS_CATALOG, qval, selval,
	 &myresult, MY_MAX_ROW);

	clStat = clStatus( g_conn[cid] );

#ifdef DEBUG
fprintf( stderr, "DEBUG: SrbJavaGlue.c ListTHING: clStat = %d\n", clStat );
fflush (stderr);
#endif
	if ( clStat != 0 )
	{
		fprintf( stderr, "SrbJavaGlue.c: List to srbMaster failed.\n" );
		fprintf( stderr, "SrbJavaGlue.c: %s", clErrorMessage( g_conn[cid] ) );
	}

#ifdef DEBUG
fprintf( stderr, "DEBUG: %s:%d\n", __FILE__, __LINE__ );
fflush (stderr);
#endif

	if ( j < 0 )
	{
		if ( j != CLI_NO_ANSWER )
			fprintf( stderr, "srbListResourceJ: srbGetDataDirInfo error: %i\n", j );
		else
			fprintf( stderr, "No Answer\n" );
		return( j );
	}

#ifdef DEBUG
fprintf( stderr, "DEBUG: %s:%d\n", __FILE__, __LINE__ );
fflush (stderr);
#endif

	origRname = rname = (char *) getFromResultStruct( &myresult,
		dcs_tname[RSRC_NAME], dcs_aname[RSRC_NAME] );

#ifdef DEBUG
fprintf( stderr, "DEBUG: %s:%d\n", __FILE__, __LINE__ );
fflush (stderr);
#endif

	i = 0;
	k = 0;
	while ( i < myresult.row_count && strcmp( rname, RESWANTED ))
	{
		if ( strstr( rname, "$deleted" ) == NULL )
		{
		  strcpy( g_qBuf[cid]->resList[k], rname );
		  k++;
		}
		rname += MAX_DATA_SIZE;
		i++;
	}
	free( origRname );

#ifdef DEBUG
fprintf( stderr, "DEBUG: %s:%d\n", __FILE__, __LINE__ );
fflush (stderr);
#endif

	return( k );
}

/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
int srbObjUnlinkJ ( int cid, char * objID, int replNum, char * collectionName )
{
    char tmpPath[MAX_TOKEN];
    
    if (replNum >= 0) {
        sprintf (tmpPath, "%s&COPY=%i", objID, replNum);
    } else {
	sprintf (tmpPath, "%s", objID);
    }
    return( srbObjUnlink ( g_conn[cid], tmpPath, collectionName ) );
}

/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
int srbRmColl ( int cid, int catType, char * collectionName )
{
    mdasC_sql_result_struct myresult;

    /* since rmColl expect a mdasC_sql_result_struct as input, have to
     * fake one. 
     */

    myresult.result_count = 1;
    myresult.row_count = 1;
    myresult.sqlresult[0].tab_name = dcs_tname[DATA_GRP_NAME];
    myresult.sqlresult[0].att_name = dcs_aname[DATA_GRP_NAME];
    myresult.sqlresult[0].values = (mdasC_handle)  collectionName;
    return( rmColl ( g_conn[cid], catType, 0, &myresult, -1, 0 ) );
}

/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
int srbObjReplicateJ ( int cid, int catType, char * objID,
	int replNum, char * collectionName,
	char * newResourceName, char * newPathName )
{
    char tmpPath[MAX_TOKEN];

    sprintf (tmpPath, "%s&COPY=%i", objID, replNum);

    return( srbObjReplicate( g_conn[cid], catType, tmpPath, collectionName,
		newResourceName, newPathName ) );
}

/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
int queryDataInCollJ( int cid, int catType, char * collectionName,
mdasC_sql_result_struct *myresult)
{
    int status;
    int clStat;
    int i;
  
    clearSqlResult (myresult);
    status = queryDataInColl (g_conn[cid], catType, collectionName,
      l_FLAG, MY_MAX_ROW, myresult);

    if (myresult->result_count != nDataMap) {
	if (status != MCAT_INQUIRE_ERROR)
	    fprintf (stderr, 
	      "queryDataInCollJ: %s Expected %d attributes, got %d, status = %d\n",
	      collectionName, nDataMap, myresult->result_count, status);
	    return (SYS_ERR_MDAS_CAT_RET);
    }

    for (i = 0; i < nDataMap; i++) {
	dataMap[i].resultVal = getFromResultStruct (myresult, 
	  dcs_tname[dataMap[i].selInx], dcs_aname[dataMap[i].selInx]);
	if (dataMap[i].resultVal == NULL) {
	    fprintf (stderr, "Unable to map dataMap selInx %d\n",
	      dataMap[i].selInx);
	}
    }
    
    return (status);
}

/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
int queryDataJ( int cid, int catType, char *objID, char *collectionName,
mdasC_sql_result_struct *myresult)
{
    int status;
    int clStat;
    int i;
    char tmpPath[MAX_TOKEN];
  
    clearSqlResult (myresult);

    sprintf (tmpPath, "%s/%s", collectionName, objID);
    status = queryData (g_conn[cid], catType, tmpPath,
      l_FLAG, MY_MAX_ROW, myresult);

    if (myresult->result_count != nDataMap) {
	return (0);
    }

    for (i = 0; i < nDataMap; i++) {
	dataMap[i].resultVal = getFromResultStruct (myresult, 
	  dcs_tname[dataMap[i].selInx], dcs_aname[dataMap[i].selInx]);
	if (dataMap[i].resultVal == NULL) {
	    fprintf (stderr, "Unable to map dataMap selInx %d\n",
	      dataMap[i].selInx);
	}
    }
    
    return (status);
}

/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
int querySubCollInCollJ( int cid, int catType, char * collectionName,
mdasC_sql_result_struct *myresult)
{
    int status;
    int clStat;
    int i;
  
    clearSqlResult (myresult);
    status = querySubCollInColl (g_conn[cid], catType, collectionName, 0,
      MY_MAX_ROW, myresult);

    if (myresult->result_count != nCollMap) {
        return (0);
    }

    for (i = 0; i < nCollMap; i++) {
        collMap[i].resultVal = getFromResultStruct (myresult, 
          dcs_tname[collMap[i].selInx], dcs_aname[collMap[i].selInx]);
        if (collMap[i].resultVal == NULL) {
            fprintf (stderr, "Unable to map collMap selInx %d\n",
              collMap[i].selInx);
        }
    }

    return (status);
}

/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
int srbListCollectJ( int cid, int catType, char * collectionName,
	char * flag, jint * types )
{
	/* returns negative if error, else returns number of items in the list */
	int	i, j, k;
        mdasC_sql_result_struct myresult;
	char *tmpList;
	int clStat;

#ifdef DEBUG
fprintf( stderr, "DEBUG: SrbJavaGlue.c srbListCollectJ A\n" );
fprintf( stderr, "DEBUG: SrbJavaGlue.c srbListCollectJ g_conn = %x\n", 
 g_conn[cid] );
fprintf( stderr, "DEBUG: SrbJavaGlue.c srbListCollectJ cid = %x\n", cid );
fprintf( stderr, "DEBUG: SrbJavaGlue.c srbListCollectJ catType = %d\n", catType );
fprintf( stderr, "DEBUG: SrbJavaGlue.c srbListCollectJ collectionName = %s\n", collectionName );
fprintf( stderr, "DEBUG: SrbJavaGlue.c srbListCollectJ flag = %s\n", flag );
fprintf( stderr, "DEBUG: SrbJavaGlue.c srbListCollectJ types = %x\n", types );
fprintf( stderr, "DEBUG: SrbJavaGlue.c srbListCollectJ types[0] = %d\n", types[0] );
#endif

	j = srbListCollect( g_conn[cid], catType, collectionName, flag,
	 &myresult, MY_MAX_ROW);

	clStat = clStatus( g_conn[cid] );

#ifdef DEBUG
fprintf( stderr, "DEBUG: SrbJavaGlue.c ListTHING: clStat = %d\n", clStat );
#endif
	if ( clStat != 0 )
	{
		fprintf( stderr, "SrbJavaGlue.c: List to srbMaster failed.\n" );
		fprintf( stderr, "SrbJavaGlue.c: %s", clErrorMessage( g_conn[cid] ) );
	}

#ifdef DEBUG
fprintf( stderr, "DEBUG: SrbJavaGlue.c srbListCollectJ B\n" );
#endif

	if ( j < 0 ) return( j );
	i = 0;
	k = 0;
	tmpList = (char *) getFromResultStruct(
         (mdasC_sql_result_struct *) &myresult,
          dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

	for (i = 0; i < myresult.row_count; i++) {

		if ( strstr(tmpList, "$deleted" ) == NULL )
		{
			if ( (tmpList[0] == ' ') && (tmpList[1] == ' ') )
			{
				/* Its a file (dataset) */
				strcpy( g_qBuf[cid]->fileList[k], 
				 (char *) (tmpList + 2) );
				types[k] = 1;
			}
			else
			{
				/* Its a directory (collection) */
				strcpy( g_qBuf[cid]->fileList[k], tmpList );
				types[k] = 0;
			}
#ifdef DEBUG
fprintf( stderr, "DEBUG: SrbJavaGlue.c srbListCollectJ: fileList = %s\n",
	g_qBuf[cid]->fileList[k] );
#endif
			k++;
		}
		tmpList += MAX_DATA_SIZE;
	}

#ifdef DEBUG
fprintf( stderr, "DEBUG: SrbJavaGlue.c srbListCollectJ C\n" );
#endif

	return( k );
}


/************************************************************************
 Added glue: for WIN32 only By Bing
 ************************************************************************/
/* The function removes a collection recursively. i.e all its sub-collections 
   and datasets.
 */
#ifdef _WIN32
int srbRemoveCollJ(int cid, int catType, char * collectionName)
{
	mdasC_sql_result_struct myresult;

	myresult.result_count = 1;
    myresult.row_count = 1;
    myresult.sqlresult[0].tab_name = dcs_tname[DATA_GRP_NAME];
    myresult.sqlresult[0].att_name = dcs_aname[DATA_GRP_NAME];
    myresult.sqlresult[0].values = (mdasC_handle)  collectionName;

	return rmColl(g_conn[cid],catType,R_FLAG,&myresult,-1, 0);
}

int srbDataToLocalDirCopy(int cid, int catType, 
				char *objID, int replNum,char *dataType,
				char *collectionName, 
				char *targDir) 
{
    int status;
    mdasC_sql_result_struct myresult;

    /* since dataToDirCopy expect a mdasC_sql_result_struct as input, have to
     * fake one.
     */

    myresult.result_count = 4;
    myresult.row_count = 1;
    myresult.sqlresult[0].tab_name = dcs_tname[DATA_GRP_NAME];
    myresult.sqlresult[0].att_name = dcs_aname[DATA_GRP_NAME];
    myresult.sqlresult[0].values = (mdasC_handle)  collectionName;
    myresult.sqlresult[1].tab_name = dcs_tname[DATA_NAME];
    myresult.sqlresult[1].att_name = dcs_aname[DATA_NAME];
    myresult.sqlresult[1].values = (mdasC_handle)  objID;

	myresult.sqlresult[2].tab_name = dcs_tname[DATA_TYP_NAME];
    myresult.sqlresult[2].att_name = dcs_aname[DATA_TYP_NAME];
    myresult.sqlresult[2].values = (mdasC_handle)dataType;

    myresult.sqlresult[3].tab_name = dcs_tname[SIZE];
    myresult.sqlresult[3].att_name = dcs_aname[SIZE];
    myresult.sqlresult[3].values = (mdasC_handle)  "1";

    status = dataToDirCopy (g_conn[cid], catType, F_FLAG, &myresult,
					replNum,targDir, "");

    return status;
}

extern userInfo *proxyUser,*clientUser;
int srbModifyUserJ (int conn, int catType, 
			char *dataValue1,
			char *dataValue2, int retractionType)
{
	return srbModifyUser(g_conn[cid],catType,dataValue1,dataValue2,
	 retractionType);
}

int srbChangePasswdJ(int conn,char *NewPasswd)
{
	int localStatus;

	localStatus = srbModifyUser(g_conn[conn],MDAS_CATALOG,NewPasswd,"",U_CHANGE_PASSWORD);

	if(localStatus < 0)
		return localStatus;

	if(proxyUser->userAuth != NULL)
		free(proxyUser->userAuth);

	proxyUser->userAuth = strdup(NewPasswd);

	if(clientUser != NULL)
	  localStatus = clientUser->userId;
	
	return 0;
}

int srbContainerCreateJ(int conn, int catType, char *containerName,
			char *containerType, char *resourceName, int containerSize)
{
	return srbContainerCreate(g_conn[conn], catType, containerName,
				containerType, resourceName, containerSize);
}

int srbGenQuerySetResultMap(int cid)
{
	int nMoreRowCnt;

	filterDeleted (&g_qBuf[cid]->dataResult);

	nMoreRowCnt = g_qBuf[cid]->genResult.row_count;
	for (i=0; i<nGenAttri; i++) 
	{
		genResultMap[i].resultVal = getFromResultStruct(
		  &g_qBuf[cid]->genResult, dcs_tname[genResultMap[i].selInx], 
		    dcs_aname[genResultMap[i].selInx]);
		if (genResultMap[i].resultVal == NULL) 
		{
			/*
            	fprintf (stderr, "Unable to map genResultMap selInx %d\n",
                 genResultMap[i].selInx);
			*/
			return -1;
        }
	}

	return nMoreRowCnt;
}

int srbGenQueryJ(int    conn,		int		catType, 
				char** Qval,		int*	QvalInx,   int QvalCnt,
				int*   SelInx,		int		SelInxCnt,
				int    RowsWanted,
				int*   RowCount)
{
	char qval[MAX_DCS_NUM][MAX_TOKEN];
	int selval[MAX_DCS_NUM];
	int i,t;
	int localStatus;

	clearSqlResult (&g_qBuf[conn]->genResult);

	*RowCount = 0;
	
	for (i = 0; i < MAX_DCS_NUM; i++)
	{
		strcpy(qval[i],"");
		selval[i] = 0;
	}

	nGenAttri = SelInxCnt;
	for(i=0;i<SelInxCnt;i++)
	{
		t = SelInx[i];
		genResultMap[i].selInx = t;
		selval[t] = 1;
	}

	for(i=0;i<QvalCnt;i++)
	{
		t = QvalInx[i];

		if(Qval[i] != NULL)
			strcpy(qval[t],Qval[i]);
		else
			qval[t][0] = '\0';
	}

	localStatus = srbGetDataDirInfo(g_conn[conn],catType,
	  qval,selval, &g_qBuf[conn]->genResult,RowsWanted);
	/* &gGenResult,MAX_GET_RESULTS); */

	if(localStatus < 0)
		return localStatus;

	filterDeleted (&g_qBuf[cid]->dataResult);

	nGenRowCnt = g_qBuf[conn]->genResult.row_count;
	for (i=0; i<nGenAttri; i++) 
	{
		genResultMap[i].resultVal = getFromResultStruct(
		  &g_qBuf[conn]->genResult, dcs_tname[genResultMap[i].selInx], 
		    dcs_aname[genResultMap[i].selInx]);
		if (genResultMap[i].resultVal == NULL) 
		{
			/*
            	fprintf (stderr, "Unable to map genResultMap selInx %d\n",
                 genResultMap[i].selInx);
			*/
			return -1;
        	}
	}

    	*RowCount = g_qBuf[conn]->genResult.row_count;

	return 0;
}

int srbGenQueryMoreDataJ(int conn,int catType,int dataPageSize)
{
	int LocalStatus;

	if(g_qBuf[conn]->genResult.continuation_index < 0)
		return 0;

	LocalStatus = srbGetMoreRows(g_conn,catType,
	  g_qBuf[conn]->genResult.continuation_index,
		&g_qBuf[conn]->genResult,dataPageSize);

	if(LocalStatus < 0)
		return LocalStatus;

	if(g_qBuf[conn]->genResult.result_count == 0)
		return 0;

	return srbGenQuerySetResultMap();
}

char* srbGetGenQueResultJ(int MetaIdx,int RowIdx)
{
	char *ptr;
	ptr = genResultMap[MetaIdx].resultVal;
    ptr += RowIdx * MAX_DATA_SIZE;

	if(StrLen(ptr) > 0)
	  return (char*)strdup(ptr);
	
	return NULL;
}

int srbGetLogicalResourceJ(int conn,int catType,int* rescCnt,char ***logiResc)
{
	char *qval[10];
	int  qval_inx[10];
	int  sel_inx[10];
	int  data_cnt;
	char **sResult;

	int  localStatus;

	/* strcpy(qval[0],""); */
	qval[0] = NULL;
	qval_inx[0] = 0;
	sel_inx[0] = RSRC_NAME;

	localStatus = srbGenQueryJ(conn,catType,qval,qval_inx,1,sel_inx,1,MAX_GET_RESULTS,&data_cnt);

	if(localStatus < 0)
		return localStatus;

	sResult = (char **)calloc(data_cnt,sizeof(char*));

	for(i=0;i<data_cnt;i++)
	{
		sResult[i] = srbGetGenQueResultJ(0,i);
	}

	*rescCnt = data_cnt;
	*logiResc = sResult;
	sResult = NULL;

	return 0;
}

int srbGetUserContainers(int nConn,int nCatType,
						 int nOwnerFlag,
						 int *nCnt, char*** pContainers)
{
	char *qval[10];
	int  qval_cnt;
	int  qval_inx[10];
	int  sel_inx[10];
	int  DataCnt;
	char **ResultData;
	int  LocalStatus=0;
	int  i;
	char* TmpStr;
	char buff[1024];
	

	if(nOwnerFlag > 0)
		qval_inx[0] = DATA_OWNER;
	else
		qval_inx[0] = USER_NAME;
	qval_inx[1] = DOMAIN_DESC;
	qval_inx[2] = CONTAINER_NAME;

	TmpStr = srbGetUserJ();
	sprintf(buff," = '%s'",TmpStr);
	free(TmpStr);  TmpStr = NULL;
	qval[0] = strdup(buff);

	TmpStr = srbGetDimainJ();
	sprintf(buff," = '%s'",TmpStr);
	free(TmpStr);  TmpStr = NULL;
	qval[1] = strdup(buff);
	qval[2] = strdup(" is not null");
	qval_cnt = 3;

	sel_inx[0] = CONTAINER_NAME;

	LocalStatus = srbGenQueryJ(nConn,nCatType,qval,qval_inx,qval_cnt,sel_inx,1,MAX_GET_RESULTS,&DataCnt);

	if(LocalStatus < 0)
		goto exit_srbGetUserContainers;

	ResultData = (char**)calloc(DataCnt,sizeof(char*));
	for(i=0;i<DataCnt;i++)
		ResultData[i] = srbGetGenQueResultJ(0,i);

	*nCnt = DataCnt;
	*pContainers = ResultData;

	ResultData = NULL;

exit_srbGetUserContainers:
	for(i=0;i<qval_cnt;i++)
		free(qval[i]);

	return LocalStatus;
}

int srbModifyCollJ(int conn,int catType,
		char *collectionName, char *dataValue1, char *dataValue2, 
		char *dataValue3, int retractionType)
{
	return srbModifyCollect(g_conn[conn],catType,collectionName,dataValue1,dataValue2,dataValue3,retractionType);
}

int srbModifyDatasetJ(int conn, int catType, char *objID,
	char *collectionName, char *resourceName, char *pathName, char *dataValue1,
	char *dataValue2, int retractionType)
{
	return srbModifyDataset (g_conn[conn], catType, objID,
				collectionName, resourceName, pathName, 
				dataValue1,dataValue2, retractionType);
}

char* srbGetDataResultJ(int metaIndex, int rowIndex)
{
	char *ptr ;
	ptr = dataMap[metaIndex].resultVal;
	ptr += rowIndex * MAX_DATA_SIZE;

	if(ptr == NULL)
		return NULL;
	else if(strlen(ptr) == 0)
		return NULL;
	else
		return strdup(ptr);
}

int srbGetNDataAttriJ(int nFlagVal)
{
	if(nFlagVal == l_FLAG)
		return nDataMap;

	return nSubDataMap;
}

int srbGetNCollAttriJ()
{
	return nCollMap;
}

int srbGetMoreCollRowsJ(int nConn,int nCatType)
{
	int LocalStatus;
	int i;


	if(g_qBuf[nConn]->collResult.continuation_index < 0)
		return 0;
	clearSqlResult (&g_qBuf[nConn]->collResult);

	LocalStatus = srbGetMoreRows (g_conn[conn], nCatType, 
	  g_qBuf[nConn]->collResult.continuation_index, 
	    &g_qBuf[nConn]->collResult,MAX_GET_RESULTS);

	if(LocalStatus < 0)
		return 0;

	filterDeleted (&g_qBuf[nConn]->collResult);

	if (g_qBuf[nConn]->collResult.row_count > 0)
	{
		for(i=0;i<nCollMap;i++)
		{
			collMap[i].resultVal = getFromResultStruct (
			 &g_qBuf[nConn]->collResult,
                	  dcs_tname[collMap[i].selInx], 
			   dcs_aname[collMap[i].selInx]);

			if (collMap[i].resultVal == NULL)
			{
				return 0;
			}
		}

		return g_qBuf[nConn]->collResult.row_count;
	}

	return 0;
}

int srbQuerySubCollInCollJ(int conn,int catType,char* CollectionName)
{
	int status;

	status = querySubCollInCollJ(conn,catType,CollectionName,
	 &g_qBuf[conn]->collResult);

	if(status < 0)
		return 0;
	else
		return g_qBuf[conn]->collResult.row_count;
}

char* srbGetCollResultJ(int metaIndex, int rowIndex)
{
	char *ptr;
	ptr = collMap[metaIndex].resultVal;
	ptr += rowIndex * MAX_DATA_SIZE;

	if(ptr == NULL)
		return NULL;
	else if(strlen(ptr) == 0)
		return NULL;
	else
		return strdup(ptr);
}

void srbResetUserInfoJ()
{
	if(proxyUser != NULL)
		free(proxyUser);

	proxyUser = NULL;
}

char* srbFileGetCollectionHomeJ()
{
	return readMdasParam(DEF_COLL_KEYWORD);
}
char* srbFileGetDomainNameJ()
{
	return readMdasParam(DOMAIN_KEYWORD);
}
char* srbFileGetDomainHomeJ()
{
	return readMdasParam(DOMAIN_KEYWORD);
}
char* srbFileGetHostNameJ()
{
	return readMdasParam(HOST_KEYWORD);
}
char* srbFileGetUserNameJ()
{
	return readMdasParam(USER_KEYWORD);
}
char* srbFileGetDefaultResourceJ()
{
	return readMdasParam(DEF_RESC_KEYWORD);
}

/*
char* srbFileGetSeaOptionJ()
{
	return readMdasParam(SEA_KEYWORD);
}
*/

int srbCreateCollectJ(int conn,int CatType,char* ParentColl,char* NewColl)
{
	return srbCreateCollect(g_conn[conn],CatType,ParentColl,NewColl);
}

char* srbGetResListItemJ(int cid, int idx)
{
	return g_qBuf[cid]->resList[idx];
}

int srbRegisterDatasetJ(int conn, int catType, char *objID, 
		char *dataTypeName, char *resourceName, char *collectionName,
		char *pathName, int dataSize)
{
	char tmpDataType[1024];

	if (strlen(dataTypeName) == 0) 
        trylocaldatatype(tmpDataType, objID);
     else
        strcpy(tmpDataType, dataTypeName);

	return srbRegisterDataset(g_conn[conn],catType,objID,
				tmpDataType,resourceName,
				collectionName,pathName,dataSize);
}

int clConnectJsucceeded()
{
	if(g_conn == NULL)
		return 0;

	return 1;
}

int clConnErrStat()
{
	return nConnStatus;
}

int srbQueryDataInCollJ(int cid, int catType, int flagVal, char * collectionName)
{
	int lStatus;
    int i;
	int tmpMapData;
  
    clearSqlResult (&g_qBuf[cid]->dataResult);
    lStatus = queryDataInColl (g_conn[cid], catType, collectionName,
		flagVal, MAX_GET_RESULTS, &g_qBuf[cid]->dataResult);

	if(flagVal & l_FLAG)
	{
		if(g_qBuf[cid]->dataResult.result_count != nDataMap)
			return 0;

		tmpMapData = nDataMap;
	}
	else
	{
		if(g_qBuf[cid]->dataResult.result_count != nSubDataMap)
			return 0;

		tmpMapData = nSubDataMap;
	}

    for (i = 0; i < tmpMapData; i++) 
	{
		dataMap[i].resultVal = getFromResultStruct (
		  &g_qBuf[cid]->dataResult, dcs_tname[dataMap[i].selInx], 
		    dcs_aname[dataMap[i].selInx]);
	}
    
	if(lStatus < 0)
		return (0);
	
	return (g_qBuf[cid]->dataResult.row_count);
}

int srbGetMoreDataRowsJ(int cid,int nCatType, int nFlagVal)
{
	int LocalStatus;
	int tmpDataMap;

	if(g_qBuf[cid]->dataResult.continuation_index < 0)
		return 0;

	
	clearSqlResult (&g_qBuf[cid]->dataResult);
	LocalStatus = srbGetMoreRows (g_conn[cid], nCatType, 
	  g_qBuf[cid]->dataResult.continuation_index, &gDataResult, 
	   MAX_GET_RESULTS);

	if(LocalStatus < 0)
		return 0;

	filterDeleted (&g_qBuf[cid]->dataResult);

	if(nFlagVal & l_FLAG)
		tmpDataMap = nDataMap;
	else
		tmpDataMap = nSubDataMap;

	if(g_qBuf[cid]->dataResult.row_count > 0)
	{
		for (i = 0; i < tmpDataMap; i++)
		{
			dataMap[i].resultVal = getFromResultStruct (
			 &g_qBuf[cid]->dataResult,
          		  dcs_tname[dataMap[i].selInx], dcs_aname[dataMap[i].selInx]);

			if (dataMap[i].resultVal == NULL)
			{
					return 0;
			}
		}

		return g_qBuf[cid]->dataResult.row_count;
	}

	return 0;
}

int srbGetDataIntTypeJ(char* pDataTypeName)
{
	if(StrLen(pDataTypeName) == 0)
		return TEXT_FILE;

	for(i=0;i<TEXT_DATATYPE_CNT;i++)
	{
		if(strcmp(TextDataType[i],pDataTypeName) == 0)
			return TEXT_FILE;
	}

	if(strcmp(GraphicDataType[0],pDataTypeName) == 0)
		return JPEG_FILE;

	if(strcmp(GraphicDataType[1],pDataTypeName) == 0)
		return GIF_FILE;

	return -1;
}

int srbIsTextModeJ(char* pDataTypeName)
{
	if(StrLen(pDataTypeName) == 0)
		return 1;

	if(strcmp(pDataTypeName,GENERIC_TXT_TYPE) == 0)
		return 1;

	for(i=0;i<TEXT_DATATYPE_CNT;i++)
	{
		if(strcmp(TextDataType[i],pDataTypeName) == 0)
			return 1;
	}

	return 0;
}

char* srbGetLocalCacheDir()
{
	return strdup(LocalCacheDir);
}

void  srbSetLocalCacheDir(char* cache_dir)
{
	strcpy(LocalCacheDir, cache_dir);
}

char* srbGetUserJ()
{
	char *userName;

    if (g_conn->clientUser == NULL) {
        userName = g_conn->proxyUser->userName;
    } else {
        userName = g_conn->clientUser->userName;
    }

	return strdup(userName);
}

char* srbGetDimainJ()
{
	char *domainName;

    if (g_conn->clientUser == NULL) {
        domainName = g_conn->proxyUser->domainName;
    } else {
        domainName = g_conn->clientUser->domainName;
    }

	return strdup(domainName);
}

int srbContainerDeleteJ(int nConn,int nCaType,char* pContainerName,int nForceFlag)
{
	int LocalStatus;
	int FlagVal=0;

	if(nForceFlag == 1)
		FlagVal |= F_FLAG;

	LocalStatus = rmContainer(g_conn[nConn],nCaType,pContainerName,FlagVal);

	return LocalStatus;
}

int srbContainerSyncJ(int nConn,int nCatType,char* pContName,int nPurgeFlag)
{
	return srbSyncContainer(g_conn[nConn],nCatType,pContName,nPurgeFlag);
}


int srbGetDatasetCommentJ(int nConn,int nCatType,
						  char* pParColl,char* pObjId,int nReplNum,
						  char **pComment)
{
	char *qval[10];
	int	 qval_cnt;
	int  qval_inx[10];
	int  sel_inx[10];
	int  DataCnt;
	int  LocalStatus;
	int  i;
	char buff[1024];
	

	*pComment = NULL;

	qval_inx[0] = DATA_NAME;
	qval_inx[1] = DATA_GRP_NAME;
	/* qval_inx[2] = DATA_REPL_ENUM; */

	sprintf(buff," = '%s'",pObjId);
	qval[0] = strdup(buff);
	sprintf(buff," = '%s'",pParColl);
	qval[1] = strdup(buff);
	/*
	sprintf(buff," = '%d'",nReplNum);
	qval[2] = strdup(buff);
	*/

	qval_cnt = 2;

	sel_inx[0] = DATA_COMMENTS;
	LocalStatus = srbGenQueryJ(nConn,nCatType,qval,qval_inx,qval_cnt,sel_inx,1,MAX_GET_RESULTS,&DataCnt);

	if(LocalStatus < 0)
	{
		LocalStatus = 0;
		goto exit_srbGetDatasetCommentJ;   /* checked the srbGenQuery in Java part. There is no error is concerned here.*/
	}

	*pComment = srbGetGenQueResultJ(0,0);

exit_srbGetDatasetCommentJ:

	for(i=0;i<qval_cnt;i++)
		free(qval[i]);

	return LocalStatus;
}

void InitSrbData(SrbData* pData)
{
	pData->comment = NULL;
	pData->container = NULL;
	pData->dataType = NULL;
	pData->grpName = NULL;
	pData->name = NULL;
	pData->owner = NULL;
	pData->replNum = NULL;
	pData->resc = NULL;
	pData->size = NULL;
	pData->timeStamp = NULL;
}

void FreeSrbData(SrbData* pData)
{
	if(pData->comment != NULL)
		free(pData->comment);
	pData->comment = NULL;

	if(pData->container != NULL)
		free(pData->container);
	pData->container = NULL;

	if(pData->dataType != NULL)
		free(pData->dataType);
	pData->dataType = NULL;

	if(pData->grpName != NULL)
		free(pData->grpName);
	pData->grpName = NULL;

	if(pData->name != NULL)
		free(pData->name);
	pData->name = NULL;

	if(pData->owner != NULL)
		free(pData->owner);
	pData->owner = NULL;

	if(pData->replNum != NULL)
		free(pData->replNum);
	pData->replNum = NULL;

	if(pData->resc != NULL)
		free(pData->resc);
	pData->resc = NULL;

	if(pData->size != NULL)
		free(pData->size);
	pData->size = NULL;

	if(pData->timeStamp != NULL)
		free(pData->timeStamp);
	pData->timeStamp = NULL;
}

SrbData* NewSrbData()
{
	SrbData* ptr;
	ptr = (SrbData*)calloc(1,sizeof(SrbData));

	ptr->comment = NULL;
	ptr->container = NULL;
	ptr->dataType = NULL;
	ptr->grpName = NULL;
	ptr->name = NULL;
	ptr->owner = NULL;
	ptr->replNum = NULL;
	ptr->resc = NULL;
	ptr->size = NULL;
	ptr->timeStamp = NULL;

	return ptr;
}

#define MAX_NUM_REPLICA 200
int srbGetDatasetAllReplicaInfoJ(int nConn, int nCatType,
			char *pDatasetName, char *pCollName,
			SrbData **pMetadata,int *nMetaDataCnt)
{
	int LocalStatus;
	mdasC_sql_result_struct myresult;
	char dataname[2048];
	char *dataList, *sizeList, *ownerList, *timeList, *replNumList, *rsrcName;
    char *collList, *containerName,*datatype;
	char buff[1024];
	SrbData *tmpMetaData;
	int tmpCnt;
	int i;

	sprintf(dataname,"%s/%s",pCollName,pDatasetName);
	LocalStatus = queryData(g_conn[nConn],nCatType,dataname,l_FLAG,MAX_NUM_REPLICA,&myresult);

	if(LocalStatus < 0)
		return LocalStatus;

	dataList = (char *) getFromResultStruct(&myresult,
        dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
    sizeList = (char *) getFromResultStruct(&myresult,
        dcs_tname[SIZE], dcs_aname[SIZE]);
    ownerList = (char *) getFromResultStruct(&myresult,
        dcs_tname[DATA_OWNER], dcs_aname[DATA_OWNER]);
    timeList = (char *) getFromResultStruct(&myresult,
        dcs_tname[REPL_TIMESTAMP], dcs_aname[REPL_TIMESTAMP]);
    replNumList = (char *) getFromResultStruct(&myresult,

        dcs_tname[DATA_REPL_ENUM], dcs_aname[DATA_REPL_ENUM]);
/* BING
        dcs_tname[CONTAINER_REPL_ENUM], dcs_aname[CONTAINER_REPL_ENUM]); */
    rsrcName = (char *) getFromResultStruct(&myresult,
        dcs_tname[PHY_RSRC_NAME], dcs_aname[PHY_RSRC_NAME]);
    containerName = (char *) getFromResultStruct(&myresult,
        dcs_tname[CONTAINER_NAME], dcs_aname[CONTAINER_NAME]);
	collList = (char *) getFromResultStruct(&myresult,
            dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
	datatype = (char *) getFromResultStruct(&myresult,
      dcs_tname[DATA_TYP_NAME], dcs_aname[DATA_TYP_NAME]);

	tmpCnt = myresult.row_count;
	tmpMetaData = (SrbData *)calloc(tmpCnt,sizeof(SrbData));
	for(i=0;i<tmpCnt;i++)
	{
		if(StrLen(containerName) > 0)
			tmpMetaData[i].container = strdup(containerName);
		if(StrLen(datatype) > 0)
			tmpMetaData[i].dataType = strdup(datatype);
		if(StrLen(collList) > 0)
			tmpMetaData[i].grpName = strdup(collList);
		if(StrLen(dataList) > 0)
			tmpMetaData[i].name = strdup(dataList);
		if(StrLen(ownerList) > 0)
			tmpMetaData[i].owner = strdup(ownerList);
		if(StrLen(replNumList) > 0)
			tmpMetaData[i].replNum = strdup(replNumList);
		if(StrLen(rsrcName) > 0)
			tmpMetaData[i].resc = strdup(rsrcName);
		if(StrLen(sizeList) > 0)
			tmpMetaData[i].size = strdup(sizeList);
		if(StrLen(timeList) > 0)
			tmpMetaData[i].timeStamp = strdup(timeList);

		containerName += MAX_DATA_SIZE;
		datatype += MAX_DATA_SIZE;
		collList += MAX_DATA_SIZE;
		dataList += MAX_DATA_SIZE;
		ownerList += MAX_DATA_SIZE;
		replNumList += MAX_DATA_SIZE;
		rsrcName += MAX_DATA_SIZE;
		sizeList += MAX_DATA_SIZE;
		timeList += MAX_DATA_SIZE;
	}

	*pMetadata = tmpMetaData; tmpMetaData = NULL;
	*nMetaDataCnt = tmpCnt;
	return 0;
}

int srbGetDatasetMetadataJ(int nConn, int nCatType, 
						  char *pDatasetName, int nReplNum, char *pCollName, 
						  SrbData *pMetadata)
{
	int LocalStatus=0;
#if 0
	int myDataCnt;
	int  qval_cnt;
	char *qval[10];
	int  qval_inx[10];
	int  sel_cnt;
	int  sel_inx[NUM_SRB_DATA_ATTS];
	char buff[1024];
	int  i;
#endif

	char *tmpComment= NULL;
	mdasC_sql_result_struct myresult;
	char dataname[2048];char *dataList, *sizeList, *ownerList, *timeList, *replNumList, *rsrcName;
    char *collList, *containerName,*datatype;
	char buff[1024];


	sprintf(dataname,"%s/%s",pCollName,pDatasetName);
	LocalStatus = queryData(g_conn[nConn],nCatType,dataname,l_FLAG,20,&myresult);

	if(LocalStatus < 0)
	{
		free(tmpComment);
		return LocalStatus;
	}

	/*
	LocalStatus = srbGetDatasetCommentJ(nConn,nCatType,pCollName,pDatasetName,nReplNum,&tmpComment);

	if(LocalStatus < 0)
		return LocalStatus;
	*/

	sprintf(dataname,"%s/%s",pCollName,pDatasetName);
	LocalStatus = queryData(g_conn[nConn],nCatType,dataname,l_FLAG,20,&myresult);

	if(LocalStatus < 0)
	{
		free(tmpComment);
		return LocalStatus;
	}

	pMetadata->comment = tmpComment;	tmpComment = NULL;
	
	dataList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) &myresult,
        dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
    sizeList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) &myresult,
        dcs_tname[SIZE], dcs_aname[SIZE]);
    ownerList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) &myresult,
        dcs_tname[DATA_OWNER], dcs_aname[DATA_OWNER]);
    timeList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) &myresult,
        dcs_tname[REPL_TIMESTAMP], dcs_aname[REPL_TIMESTAMP]);
    replNumList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) &myresult,
/*
        dcs_tname[DATA_REPL_ENUM], dcs_aname[DATA_REPL_ENUM]);
*/
        dcs_tname[CONTAINER_REPL_ENUM], dcs_aname[CONTAINER_REPL_ENUM]);
    rsrcName = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) &myresult,
        dcs_tname[PHY_RSRC_NAME], dcs_aname[PHY_RSRC_NAME]);
    containerName = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) &myresult,
        dcs_tname[CONTAINER_NAME], dcs_aname[CONTAINER_NAME]);
	collList = (char *) getFromResultStruct(
          (mdasC_sql_result_struct *) &myresult,
            dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
	datatype = (char *) getFromResultStruct(&myresult,
      dcs_tname[DATA_TYP_NAME], dcs_aname[DATA_TYP_NAME]);

	if(StrLen(containerName) > 0)
		pMetadata->container = strdup(containerName);
	if(StrLen(datatype) > 0)
		pMetadata->dataType = strdup(datatype);
	pMetadata->grpName = strdup(pCollName);
	pMetadata->name = strdup(pDatasetName);
	if(StrLen(ownerList) > 0)
		pMetadata->owner = strdup(ownerList);
	sprintf(buff,"%d",nReplNum);
	pMetadata->replNum = strdup(buff);
	if(StrLen(rsrcName) > 0)
		pMetadata->resc = strdup(rsrcName);
	if(StrLen(sizeList) > 0)
		pMetadata->size = strdup(sizeList);
	if(StrLen(timeList) > 0)
		pMetadata->timeStamp = strdup(timeList);

	return 0;

#if 0
	qval_cnt = 3;

	sprintf(buff," = '%s'",pDatasetName);
	qval[0] = strdup(buff);
	sprintf(buff," = '%s'",pCollName);
	qval[1] = strdup(buff);
	sprintf(buff,"= '%d'",nReplNum);
	qval[2] = strdup(buff);

	qval_inx[0] = DATA_NAME;
	qval_inx[1] = DATA_GRP_NAME;
	qval_inx[2] = DATA_REPL_ENUM;
	
	
	sel_inx[0] = PHY_RSRC_NAME;
	sel_inx[1] = SIZE;
	sel_inx[2] = REPL_TIMESTAMP;
	sel_inx[3] = DATA_TYP_NAME;
	sel_inx[4] = CONTAINER_NAME;
	sel_inx[5] = DATA_OWNER;
	sel_cnt = 6;
	
	LocalStatus = srbGenQueryJ(nConn,nCatType,qval,qval_inx,qval_cnt,sel_inx,
					sel_cnt,
					10,&myDataCnt);

	if(LocalStatus < 0)
		goto exit_srbGetDatasetMetadataJ;

	if(myDataCnt != 1)
	{
		LocalStatus = -1;
		goto exit_srbGetDatasetMetadataJ;
	}

	pMetadata->comment = tmpComment;	tmpComment = NULL;
	pMetadata->container = srbGetGenQueResultJ(4,0);
	pMetadata->dataType = srbGetGenQueResultJ(3,0);
	pMetadata->grpName = strdup(pCollName);
	pMetadata->name = strdup(pDatasetName);
	pMetadata->owner = srbGetGenQueResultJ(5,0);
	sprintf(buff,"%d",nReplNum);
	pMetadata->replNum = strdup(buff);
	pMetadata->resc = srbGetGenQueResultJ(0,0);
	pMetadata->size = srbGetGenQueResultJ(1,0);
	pMetadata->timeStamp = srbGetGenQueResultJ(2,0);

exit_srbGetDatasetMetadataJ:

	for(i=0;i<qval_cnt;i++)
		free(qval[i]);

	return LocalStatus;
#endif
}

#define GLUE_PAGE_SIZE 200
int srbGetDataInCollJ(int nConn,int nCatType,char* pCollNameWP,
				int nQueryField,char* pQueryStr,
				int nIncludeSubColls,
				int nMaxRowOfDataReturn,   /*  MAX_GET_RESULTS=300 */
				int *pCnt,SrbData** pSrbData)
{
	int  qval_cnt;
	char *qval[10];
	int  qval_inx[10];
	int  sel_cnt;
	int  sel_inx[NUM_SRB_DATA_ATTS];
	int  LocalStatus=0;
	char buff[1024];
	SrbData* pTmpSrbData,*pMySrbData;
	int  tmpDataCnt,myDataCnt;
	int  i;
	int  nn;


	*pCnt = 0;
	pMySrbData = NULL;
	myDataCnt = 0;

	if(nIncludeSubColls)
		sprintf(buff," like '%s%%'",pCollNameWP);
	else
		sprintf(buff," = '%s'",pCollNameWP);
	qval[0] = strdup(buff);

	qval_cnt = 2;   /* default is 2 */
	if(StrLen(pQueryStr) == 0)   /* if there is no query string, it means don't do query. */
		qval_cnt = 1; 
	else
		sprintf(buff," like '%%%s%%'",pQueryStr);
	qval[1] = strdup(buff);
	
	qval_inx[0] = DATA_GRP_NAME;
	/* qval_inx[1] = DATA_COMMENTS; */
	qval_inx[1] = nQueryField;

	for(i=0;i<NUM_SRB_DATA_ATTS;i++)
		sel_inx[i] = SrbDataAtts[i];
	
	sel_cnt = NUM_SRB_DATA_ATTS;

	LocalStatus = srbGenQueryJ(nConn,nCatType,qval,qval_inx,qval_cnt,sel_inx,
					sel_cnt,
					GLUE_PAGE_SIZE,&myDataCnt);

	if(LocalStatus < 0)
	{
		if(nQueryField == DATA_COMMENTS)
			LocalStatus = 0;

		goto exit_srbGetDataInCollJ;
	}

	if(myDataCnt == 0)
		goto exit_srbGetDataInCollJ;

	pMySrbData = (SrbData*)calloc(myDataCnt,sizeof(SrbData));
	for(i=0;i<myDataCnt;i++)
		InitSrbData(&(pMySrbData[i]));

	for(i=0;i<myDataCnt;i++)
	{
		pMySrbData[i].comment = srbGetGenQueResultJ(3,i);
		pMySrbData[i].container = srbGetGenQueResultJ(9,i);
		pMySrbData[i].dataType = srbGetGenQueResultJ(8,i);
		pMySrbData[i].grpName = srbGetGenQueResultJ(1,i);
		pMySrbData[i].name = srbGetGenQueResultJ(0,i);
		pMySrbData[i].owner = srbGetGenQueResultJ(2,i);
		pMySrbData[i].replNum = srbGetGenQueResultJ(4,i);
		pMySrbData[i].resc = srbGetGenQueResultJ(5,i);
		pMySrbData[i].size = srbGetGenQueResultJ(6,i);
		pMySrbData[i].timeStamp = srbGetGenQueResultJ(7,i);
	}
	
	while((nn = srbGenQueryMoreDataJ(nConn,nCatType,GLUE_PAGE_SIZE)) > 0)
	{
		pTmpSrbData = pMySrbData;
		tmpDataCnt = myDataCnt;

		pMySrbData = NULL;
		
		/* realocate memory */
		myDataCnt = tmpDataCnt + nn;
		pMySrbData = (SrbData*)calloc(myDataCnt,sizeof(SrbData));

		for(i=0;i<myDataCnt;i++)
			InitSrbData(&(pMySrbData[i]));

		for(i=0;i<tmpDataCnt;i++)
		{
			pMySrbData[i].comment = pTmpSrbData[i].comment;			pTmpSrbData[i].comment = NULL;
			pMySrbData[i].container = pTmpSrbData[i].container;		pTmpSrbData[i].container = NULL;
			pMySrbData[i].dataType = pTmpSrbData[i].dataType;		pTmpSrbData[i].dataType =  NULL;
			pMySrbData[i].grpName = pTmpSrbData[i].grpName;			pTmpSrbData[i].grpName = NULL;
			pMySrbData[i].name = pTmpSrbData[i].name;				pTmpSrbData[i].name = NULL;
			pMySrbData[i].owner = pTmpSrbData[i].owner;				pTmpSrbData[i].owner = NULL;
			pMySrbData[i].replNum = pTmpSrbData[i].replNum;			pTmpSrbData[i].replNum = NULL;
			pMySrbData[i].resc = pTmpSrbData[i].resc;				pTmpSrbData[i].resc = NULL;
			pMySrbData[i].size = pTmpSrbData[i].size;				pTmpSrbData[i].size = NULL;
			pMySrbData[i].timeStamp = pTmpSrbData[i].timeStamp;		pTmpSrbData[i].timeStamp = NULL;
		}

		
		/* add new data */
		for(i=0;i<nn;i++)
		{
			pMySrbData[i+tmpDataCnt].comment = NULL;
			pMySrbData[i+tmpDataCnt].container = srbGetGenQueResultJ(9,i);
			pMySrbData[i+tmpDataCnt].dataType = srbGetGenQueResultJ(8,i);
			pMySrbData[i+tmpDataCnt].grpName = srbGetGenQueResultJ(1,i);
			pMySrbData[i+tmpDataCnt].name = srbGetGenQueResultJ(0,i);
			pMySrbData[i+tmpDataCnt].owner = srbGetGenQueResultJ(2,i);
			pMySrbData[i+tmpDataCnt].replNum = srbGetGenQueResultJ(4,i);
			pMySrbData[i+tmpDataCnt].resc = srbGetGenQueResultJ(5,i);
			pMySrbData[i+tmpDataCnt].size = srbGetGenQueResultJ(6,i);
			pMySrbData[i+tmpDataCnt].timeStamp = srbGetGenQueResultJ(7,i);
		}
		
		free(pTmpSrbData);		pTmpSrbData = NULL;
	}

	
	*pCnt = myDataCnt;
	*pSrbData = pMySrbData;
	pMySrbData = NULL;

exit_srbGetDataInCollJ:
	for(i=0;i<qval_cnt;i++)
		free(qval[i]);
	return LocalStatus;
}

int srbListCollAcessCntlJ(int nConn,int nCatType,
			char *pCollName,int *nCollAccsCnt,SrbCollAccessCntl **pCollAccess)
{
	char buff[1024];
	char *qval[10];
	int  qval_cnt;
	int  qval_inx[10];
	int  sel_inx[10];

	int LocalStatus=0;
	int DataCnt;
	int i;
	SrbCollAccessCntl *TmpCollAccs;
	

	*nCollAccsCnt = 0;

	qval[0] = strdup(" not like  '%$deleted%'");

	sprintf(buff," = '%s'",pCollName);
	qval[1] = strdup(buff);

	qval_inx[0] = USER_NAME;
	qval_inx[1] = ACCESS_GROUP_NAME;

	qval_cnt = 2;

	sel_inx[0] = ACCESS_GROUP_NAME;
	sel_inx[1] = USER_NAME;
	sel_inx[2] = DOMAIN_DESC;
	sel_inx[3] = GROUP_ACCESS_CONSTRAINT;

	LocalStatus = srbGenQueryJ(nConn,nCatType,
					qval,qval_inx,qval_cnt,sel_inx,4,
					MAX_GET_RESULTS, &DataCnt);

	if((LocalStatus < 0)||(DataCnt <= 0))
		goto exit_srbListCollAcessCntlJ;

	TmpCollAccs = (SrbCollAccessCntl *)calloc(DataCnt,sizeof(SrbCollAccessCntl));

	for(i=0;i<DataCnt;i++)
	{
		TmpCollAccs[i].accessGrpName = srbGetGenQueResultJ(0,i);
		TmpCollAccs[i].userName = srbGetGenQueResultJ(1,i);
		TmpCollAccs[i].domainDesc = srbGetGenQueResultJ(2,i);
		TmpCollAccs[i].GrpAccessCnstrnt = srbGetGenQueResultJ(3,i);
	}

	*nCollAccsCnt = DataCnt;
	*pCollAccess = TmpCollAccs;
	TmpCollAccs = NULL;

exit_srbListCollAcessCntlJ:
	for(i=0;i<qval_cnt;i++)
		free(qval[i]);

	return LocalStatus;
}

int srbListDataAcessCntlJ(int nConn,int nCatType,
			char* ParCollName,char* DataName,int *nDataAccsCnt,SrbDataAccessCntl** pDataAccess)
{
	char buff[1024];
	char *qval[10];
	int  qval_cnt;
	int  qval_inx[10];
	int  sel_inx[10];

	int LocalStatus=0;
	int DataCnt;
	int i;

	SrbDataAccessCntl *TmpDataAccs;

	*nDataAccsCnt = 0;

	qval[0] = strdup(" not like  '%$deleted%'");

	sprintf(buff," = '%s'",ParCollName);
	qval[1] = strdup(buff);

	sprintf(buff," = '%s'",DataName);
	qval[2] = strdup(buff);

	qval_cnt = 3;

	qval_inx[0] = USER_NAME;
	qval_inx[1] = DATA_GRP_NAME;
	qval_inx[2] = DATA_NAME;

	sel_inx[0] = DATA_NAME;
	sel_inx[1] = USER_NAME;
	sel_inx[2] = DOMAIN_DESC;
	sel_inx[3] = ACCESS_CONSTRAINT;

	LocalStatus = srbGenQueryJ(nConn,nCatType,
					qval,qval_inx,qval_cnt,sel_inx,4,
					MAX_GET_RESULTS, &DataCnt);

	if((LocalStatus < 0) || (DataCnt <= 0))
		/* permission value is null. */
	{
		goto exit_srbListDataAcessCntlJ;
	}

	TmpDataAccs = (SrbDataAccessCntl*)calloc(DataCnt,sizeof(SrbDataAccessCntl));

	for(i=0;i<DataCnt;i++)
	{
		TmpDataAccs[i].name = srbGetGenQueResultJ(0,i);
		TmpDataAccs[i].userName = srbGetGenQueResultJ(1,i);
		TmpDataAccs[i].domainDesc = srbGetGenQueResultJ(2,i);
		TmpDataAccs[i].accessCnstrnt = srbGetGenQueResultJ(3,i);
	}

	*nDataAccsCnt = DataCnt;
	*pDataAccess = TmpDataAccs;
	TmpDataAccs = NULL;

exit_srbListDataAcessCntlJ:
	for(i=0;i<qval_cnt;i++)
		free(qval[i]);
	return LocalStatus;
}

void FreeSrbDataAccessCntl(SrbDataAccessCntl* ptr)
{
	if(ptr->accessCnstrnt)
		free(ptr->accessCnstrnt);
	if(ptr->domainDesc)
		free(ptr->domainDesc);
	if(ptr->name)
		free(ptr->name);
	if(ptr->userName)
		free(ptr->userName);
}

void FreeSrbCollAccessCntl(SrbCollAccessCntl* ptr)
{
	if(ptr->accessGrpName)
		free(ptr->accessGrpName);
	if(ptr->domainDesc)
		free(ptr->domainDesc);
	if(ptr->GrpAccessCnstrnt)
		free(ptr->GrpAccessCnstrnt);
	if(ptr->userName)
		free(ptr->userName);
}


int srbGetDomainJ(int nConn,int nCatType,int* pDomainCnt,char *** pDomains)
{
	char *qval[10];
	int  qval_inx[10];
	int  sel_inx[10];
	int LocalStatus;
	int DataCnt;
	char** TmpDomains;
	int i;

	*pDomainCnt = 0;

	qval[0] = NULL;
	qval_inx[0] = 0;

	sel_inx[0] = DOMAIN_DESC;

	LocalStatus = srbGenQueryJ(nConn,nCatType,
					qval,qval_inx,0,sel_inx,1,
					MAX_GET_RESULTS, &DataCnt);

	if(LocalStatus < 0)
		return 0;

	if(DataCnt <= 0)
		return 0;

	TmpDomains = (char**)calloc(DataCnt,sizeof(char*));
	
	for(i=0;i<DataCnt;i++)
	{
		TmpDomains[i] = srbGetGenQueResultJ(0,i);
	}

	*pDomainCnt = DataCnt;
	*pDomains = TmpDomains;
	TmpDomains = NULL;

	return 0;
}

int srbGetDomainUsersJ(int nConn,int nCatType,char* pDomain,int *pUserCnt,char ***pUserNames)
{
	char buff[1024];
	char *qval[10];
	int  qval_inx[10];
	int  sel_inx[10];
	int LocalStatus;
	int DataCnt;
	char** TmpUsers;
	int i;
	
	*pUserCnt = 0;
	sprintf(buff," = '%s'",pDomain);
	qval[0] = strdup(buff);
	qval_inx[0] = DOMAIN_DESC;

	sel_inx[0] = USER_NAME;

	LocalStatus = srbGenQueryJ(nConn,nCatType,
					qval,qval_inx,1,sel_inx,1,
					MAX_GET_RESULTS, &DataCnt);

	if(LocalStatus < 0)
		goto exit_srbGetDomainUsersJ;

	if(DataCnt <= 0)
		goto exit_srbGetDomainUsersJ;

	TmpUsers = (char**)calloc(DataCnt,sizeof(char*));
	for(i=0;i<DataCnt;i++)
		TmpUsers[i] = srbGetGenQueResultJ(0,i);

	*pUserCnt = DataCnt;
	*pUserNames = TmpUsers;
	TmpUsers = NULL;

exit_srbGetDomainUsersJ:
	free(qval[0]);
	return 0;
}

int srbChmodInCollJ(int nConn,int nCatType,
					char* CollName,
					char* pUser,char* pDomain,char* pMode)
{
	char buff[1024];

	sprintf(buff,"%s@%s",pUser,pDomain);

	return chmodInColl(g_conn[nConn],nCatType,CollName,R_FLAG,pMode,buff);
}

int srbCollReplicateJ(int nConn,int nCatType,char* pColl,char* pResc)
{
	return replicateColl(g_conn[nConn],nCatType,R_FLAG,pColl,pResc);
}

int srbRegisterDirJ(int nConn,int nCatType,
					char* pSrcDirPath,char* pTargColl,char* pResc)
{
	int LocalStatus;

	srbPathName srbPathName;
	srbPathName.inpArgv = pSrcDirPath;
		
	LocalStatus = chkFileName (&srbPathName);

    if (LocalStatus < 0)
        return LocalStatus;

	return registerDir(g_conn[nConn],nCatType,&srbPathName,pTargColl,pResc);
}

int srbUnregisterDatasetJ(int nConn,int nCatType,
						  char* pObjId,int ReplNum,char* ParColl)
{
	char buff[1024];

	if(ReplNum < 0)
		strcpy(buff,pObjId);
	else
		sprintf(buff,"%s&COPY=%d",pObjId,ReplNum);

	return srbUnregisterDataset(g_conn[nConn],buff,ParColl);
}

int srbUnregCollJ(int nConn,int nCatType,char* pColl)
{
	mdasC_sql_result_struct myresult;

	myresult.result_count = 1;
    myresult.row_count = 1;
    myresult.sqlresult[0].tab_name = dcs_tname[DATA_GRP_NAME];
    myresult.sqlresult[0].att_name = dcs_aname[DATA_GRP_NAME];
    myresult.sqlresult[0].values = (mdasC_handle) pColl;

	return rmColl(g_conn[nConn],nCatType,unreg_FLAG,&myresult,-1, 0);
}

char* srbGetErrMsg(int nErrNo)
{
	int i;
	char buff[1024];

	for(i=0;i<Srb_numents;i++)
	{
		if(nErrNo == srb_errent[i].srb_err)
		{
			return strdup(srb_errent[i].srb_err_short);
		}
	}

	sprintf(buff,"An undefined error occured.");
	return strdup(buff);
}

int srbConnectJ(int *conn_id,
		char *coll_home, char *domain_home,
		char *default_resc, char *sea_option, char *srb_port,
		char *srb_host, char *user_name, char *srb_passwd)
{
	int cid;
	char buff[1024];

	srbResetUserInfoJ();

	sprintf(buff,"mdasCollectionHome=%s",coll_home);
	_putenv(buff);

	sprintf(buff,"mdasDomainName=%s",domain_home);
	_putenv(buff);

	sprintf(buff,"defaultResource=%s",default_resc);
	_putenv(buff);

	/* currently support password only */
	sprintf(buff,"SEA_OPT=%0");
	_putenv(buff);

	sprintf(buff,"srbUser=%s",user_name);
	_putenv(buff);

	sprintf(buff,"srbHost=%s",srb_host);
	_putenv(buff);

	srbResetUserInfoJ();
	cid = clConnectJ(srb_host,srb_port,srb_passwd);

	if(!clConnectJsucceeded())
	{
		return clConnErrStat();
	}

	*conn_id = cid;
	return 0;
}

void srbDisconnectJ(int conn_id)
{
	clFinishJ(conn_id);
	g_conn[conn_id] = NULL;
	g_connID[conn_id] = 0;
	srbResetUserInfoJ();
}

int srbIsDisconnectedJ()
{
	if(g_conn == NULL)
		return 1;
	return 0;
}

int srbGetConnId()
{
	return g_connID;
}

srbConn *srbGetConn(int conn_id)
{
	if(g_connID == conn_id)
		return g_conn;

	return NULL;
}

int srbGetDefaultPortNum()
{
	return DefaultPort;
}

#define SRB_TMPPREFIX "srbtmp"
char* SrbWin32GetFileType(char* FileName)
{
	char *fileExt;

	fileExt=strrchr(FileName,'.');

	if(StrLen(fileExt) <= 1)
		return strdup(GENERIC_BIN_TYPE);

	++fileExt;

	if(_stricmp(fileExt,"txt") == 0)
		return strdup(TextDataType[0]);

	else if(_stricmp(fileExt,"html") == 0)
		return strdup(TextDataType[4]);

	else if(_stricmp(fileExt,"htm") == 0)
		return strdup(TextDataType[4]);

	else if(_stricmp(fileExt,"bat") == 0)
		return strdup(GENERIC_TXT_TYPE);

	else if(_stricmp(fileExt,"sys") == 0)
		return strdup(GENERIC_TXT_TYPE);

	else if(_stricmp(fileExt,"c") == 0)
		return strdup(TextDataType[1]);

	else if(_stricmp(fileExt,"cpp") == 0)
		return strdup(GENERIC_TXT_TYPE);

	else if(_stricmp(fileExt,"h") == 0)
		return strdup(TextDataType[2]);

	else if(_stricmp(fileExt,"java") == 0)
		return strdup(TextDataType[3]);

	else if(_stricmp(fileExt,"jav") == 0)
		return strdup(TextDataType[3]);

	else if(_stricmp(fileExt,"cpp") == 0)
		return strdup(TextDataType[6]);

	else if(_stricmp(fileExt,"cxx") == 0)
		return strdup(GENERIC_TXT_TYPE);

	else if(_stricmp(fileExt,"hxx") == 0)
		return strdup(GENERIC_TXT_TYPE);

	else if(_stricmp(fileExt,"pl") == 0)
		return strdup(GENERIC_TXT_TYPE);

	/* binary file */
	else if(_stricmp(fileExt,"jpg") == 0)
		return strdup(GraphicDataType[0]);

	else if(_stricmp(fileExt,"jpeg") == 0)
		return strdup(GraphicDataType[0]);

	else if(_stricmp(fileExt,"gif") == 0)
		return strdup(GraphicDataType[1]);

	else if(_stricmp(fileExt,"bmp") == 0)
		return strdup(GraphicDataType[2]);

	else if(_stricmp(fileExt,"tif") == 0)
		return strdup(GraphicDataType[3]);

	else if(_stricmp(fileExt,"tiff") == 0)
		return strdup(GraphicDataType[3]);

/*
	else if(_stricmp(fileExt,"doc") == 0)
		return strdup(GENERIC_BIN_TYPE);

	else if(_stricmp(fileExt,"ppt") == 0)
		return strdup(GENERIC_BIN_TYPE);

	else if(_stricmp(fileExt,"exe") == 0)
		return strdup(GENERIC_BIN_TYPE);

	else if(_strcmp(fileExt,"mpg") == 0)
		return strdup(GENERIC_BIN_TYPE);

	else if(_strcmp(fileExt,"mpeg") == 0)
		return strdup(GENERIC_BIN_TYPE);
*/
	
	return strdup(GENERIC_BIN_TYPE);
}

int srbWin32IsTextFile(char *FileName)
{
	char *dataType;
	int  textMode;

	dataType = SrbWin32GetFileType(FileName);
	textMode = srbIsTextModeJ(dataType);

	free(dataType);

	return textMode;
}

char* SrbWin32GetTmpFileName()
{
	char buff[1024];

	int n = rand();
	n = n%10000;

	sprintf(buff,"%s%d",SRB_TMPPREFIX,n);

	return strdup(buff);
}

char* SrbWin32GetTmpFileNameWP(char* pDirName,char* pExt)
{
	char* RealPath;
	char buff[1024];
	int name_found = FALSE;
	char* TmpStr;
	int fd;

	if(StrLen(pExt) == 0)
		return NULL;

	if(StrLen(pDirName) == 0)
	{
		RealPath = LocalCacheDir;
	}
	else
	{
		RealPath = pDirName;
	}

	while(!name_found)
	{
		TmpStr = SrbWin32GetTmpFileName();

		sprintf(buff,"%s\\%s%s",RealPath,TmpStr,pExt);
		free(TmpStr);

		/* check if the file name already exist */
		fd = SrbNtFileOpen(buff,_O_RDONLY,0);
		if(fd == -1)
			name_found = TRUE;
		else
			_close(fd);
	}

	SrbNtPathBackSlash(buff);
	return strdup(buff);
}

void SrbWin32CleanTmpFiles(char* pDirName)
{
	char buff[1024];

	sprintf(buff,"rd /s /q %s",LocalCacheDir);

	system(buff);
}

void SystemJ(char* pCommand)
{
	system(pCommand);
}

#endif	/*_WIN32 */


/**********************end of added glue By Bing *****************/


#ifndef _WIN32

/************************************************************************/
/********************************  GLUE  ********************************/
/************************************************************************/


/************************************************************************/
/*                                                                      */
/*  Opens an SRB connection given host, port, and password.             */
/*  Returns connection id.                                              */
/*                                                                      */
/************************************************************************/
JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_clConnectJ
( JNIEnv * env, jobject obj,
 jstring jHost, jstring jPort, jstring jAuth, jstring jUser, jstring jDomain )
{
	int result = 0;
	char * host;
	char * port;
	char * auth;
	char * user;
	char * domain;

	const char * cHost = (char*) (*env)->GetStringUTFChars( env, jHost, 0 );
	const char * cPort = (char*) (*env)->GetStringUTFChars( env, jPort, 0 );
	const char * cAuth = (char*) (*env)->GetStringUTFChars( env, jAuth, 0 );
	const char * cUser = (char*) (*env)->GetStringUTFChars( env, jUser, 0 );
	const char * cDomain = (char*) (*env)->GetStringUTFChars( env, 
	 jDomain,0);

	result = clConnectJ( (char *)cHost, (char *)cPort, (char *)cAuth, 
	 (char *)cUser, (char *)cDomain);

	(*env)->ReleaseStringUTFChars( env, jHost, cHost );
	(*env)->ReleaseStringUTFChars( env, jPort, cPort );
	(*env)->ReleaseStringUTFChars( env, jAuth, cAuth );
	(*env)->ReleaseStringUTFChars( env, jUser, cUser );
	(*env)->ReleaseStringUTFChars( env, jDomain, cDomain );


	return( (jint) result );
}


/************************************************************************/
/*                                                                      */
/*  Closes an SRB connection given the connection id.                   */
/*                                                                      */
/************************************************************************/
JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_clFinishJ
( JNIEnv * env, jobject obj,
	jint jConn )
{
	int result = 0;

	int cConn = jConn;

	clFinishJ( cConn );

	return( (jint) result );
}


/************************************************************************/
/*                                                                      */
/*  Given a valid SRB connection ID, this routine generates a list      */
/*  of member names and member types for a specified collection name.   */
/*                                                                      */
/************************************************************************/
JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_srbListCollectJ
( JNIEnv * env, jobject obj,
	jint jConn, jint jCatType, jstring jCollectionName,
	jstring jFlag, jintArray jTypes )
{
	int count;
	int i;
	int cConn;
	int cCatType;
	const char * cCollectionName;
	const char * cFlag;
	jsize len;
	jint * cTypes;

#ifdef DEBUG
fprintf( stderr, "DEBUG: SrbJavaGlue.c Java_edu_sdsc_SrbGlue_SrbJavaGlue_srbListCollectJ A\n" );
#endif

	count = 0;
	cConn = jConn;
	cCatType = jCatType;

	cCollectionName = (*env)->GetStringUTFChars( env, jCollectionName, 0 );
	cFlag = (*env)->GetStringUTFChars( env, jFlag, 0 );

#ifdef DEBUG
fprintf( stderr, "DEBUG: SrbJavaGlue.c Java_edu_sdsc_SrbGlue_SrbJavaGlue_srbListCollectJ B\n" );
#endif

	len = (*env)->GetArrayLength( env, jTypes );

#ifdef DEBUG
fprintf( stderr, "DEBUG: SrbJavaGlue.c Java_edu_sdsc_SrbGlue_SrbJavaGlue_srbListCollectJ C\n" );
#endif

	cTypes = (*env)->GetIntArrayElements( env, jTypes, 0 );

#ifdef DEBUG
fprintf( stderr, "DEBUG: SrbJavaGlue.c Java_edu_sdsc_SrbGlue_SrbJavaGlue_srbListCollectJ D\n" );
#endif

	count = srbListCollectJ( cConn, cCatType, (char*) cCollectionName,
		(char*) cFlag, cTypes );

#ifdef DEBUG
fprintf( stderr, "DEBUG: SrbJavaGlue.c Java_edu_sdsc_SrbGlue_SrbJavaGlue_srbListCollectJ E\n" );
#endif

	(*env)->ReleaseStringUTFChars( env, jCollectionName, cCollectionName );
	(*env)->ReleaseStringUTFChars( env, jFlag, cFlag );
	(*env)->ReleaseIntArrayElements( env, jTypes, cTypes, 0 );

	return( (jint) count );
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_querySubCollInCollJ
( JNIEnv * env, jobject obj,
        jint jConn, jint jCatType, jstring jCollectionName)
{
    int count;
    int status;
    int cConn;
    int cCatType;
    const char * cCollectionName;

    cConn = jConn;
    cCatType = jCatType;
    cCollectionName = (*env)->GetStringUTFChars( env, jCollectionName, 0 );

    status = querySubCollInCollJ (cConn, cCatType, (char*) cCollectionName, 
      &g_qBuf[cConn]->collResult);

    (*env)->ReleaseStringUTFChars( env, jCollectionName, cCollectionName );

    if (status < 0)
	return ((jint) 0);
    else
	return ((jint) g_qBuf[cConn]->collResult.row_count);
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_chmodInCollJ 
( JNIEnv * env, jobject obj,
jint jConn, jint jCatType,
jstring jCollectionName, jstring jUser, jstring jDomain, jstring jMode)
{
    int status;
    int cConn;
    int cCatType;
    const char * cCollectionName;
    const char * cUser;
    const char * cDomain;
    const char * cMode;
    char userdom[MAX_TOKEN];

    cConn = jConn;
    cCatType = jCatType;
    cCollectionName = (*env)->GetStringUTFChars( env, jCollectionName, 0 );
    cUser = (*env)->GetStringUTFChars( env, jUser, 0 );
    cDomain = (*env)->GetStringUTFChars( env, jDomain, 0 );
    cMode = (*env)->GetStringUTFChars( env, jMode, 0 );

    sprintf(userdom,"%s@%s",cUser,cDomain);

    status = chmodInColl (g_conn[cConn], cCatType,
      (char *) cCollectionName, R_FLAG, (char *) cMode, userdom);

    (*env)->ReleaseStringUTFChars( env, jCollectionName, cCollectionName );
    (*env)->ReleaseStringUTFChars( env, jUser, cUser );
    (*env)->ReleaseStringUTFChars( env, jDomain, cDomain );
    (*env)->ReleaseStringUTFChars( env, jMode, cMode );

    return ((jint) status);
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_importToOldObjJ 
( JNIEnv * env, jobject obj,
jint jConn, jint jCatType, jstring jObjID, jstring jReplNum,
jstring jCollectionName, jstring jResource, jstring localFile, 
jstring jContainer)
{
    int status;
    int cConn;
    int cCatType;
    const char * cObjID;
    int cReplNum;
    const char * myReplNum;
    const char * cCollectionName;
    const char * cResource;
    const char * cLocalFile;
    const char * cContainer;

    cConn = jConn;
    cCatType = jCatType;
    cObjID = (*env)->GetStringUTFChars( env, jObjID, 0 );
    myReplNum = (*env)->GetStringUTFChars( env, jReplNum, 0 );
    cReplNum = atoi ((char *)myReplNum);
    cCollectionName = (*env)->GetStringUTFChars( env, jCollectionName, 0 );
    cResource = (*env)->GetStringUTFChars( env, jResource, 0 );
    cLocalFile = (*env)->GetStringUTFChars( env, localFile, 0 );
    cContainer = (*env)->GetStringUTFChars( env, jContainer, 0 );

    status = srbImportToOldObjJ (cConn, cCatType, (char *) cObjID, cReplNum,
     (char *) cCollectionName, (char *) cResource, (char *) cLocalFile,
      (char *) cContainer);

    (*env)->ReleaseStringUTFChars( env, jObjID, cObjID );
    (*env)->ReleaseStringUTFChars( env, jCollectionName, cCollectionName );
    (*env)->ReleaseStringUTFChars( env, jReplNum, myReplNum );
    (*env)->ReleaseStringUTFChars( env, jResource, cResource );
    (*env)->ReleaseStringUTFChars( env, localFile, cLocalFile );
    (*env)->ReleaseStringUTFChars( env, jContainer, cContainer );

    return ((jint) status);
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_importToNewObjJ 
( JNIEnv * env, jobject obj,
jint jConn, jint jCatType, jstring jObjID, 
jstring jCollectionName, jstring jResource, jstring localFile, 
jstring jContainer)
{
    int status;
    int cConn;
    int cCatType;
    const char * cObjID;
    const char * cCollectionName;
    const char * cResource;
    const char * cLocalFile;
    const char * cContainer;

    cConn = jConn;
    cCatType = jCatType;
    cObjID = (*env)->GetStringUTFChars( env, jObjID, 0 );
    cCollectionName = (*env)->GetStringUTFChars( env, jCollectionName, 0 );
    cResource = (*env)->GetStringUTFChars( env, jResource, 0 );
    cLocalFile = (*env)->GetStringUTFChars( env, localFile, 0 );
    cContainer = (*env)->GetStringUTFChars( env, jContainer, 0 );

    status = srbImportToNewObjJ (cConn, cCatType, (char *) cObjID,
     (char *) cCollectionName, (char *) cResource, (char *) cLocalFile,
      (char *) cContainer);

    (*env)->ReleaseStringUTFChars( env, jObjID, cObjID );
    (*env)->ReleaseStringUTFChars( env, jCollectionName, cCollectionName );
    (*env)->ReleaseStringUTFChars( env, jResource, cResource );
    (*env)->ReleaseStringUTFChars( env, localFile, cLocalFile );
    (*env)->ReleaseStringUTFChars( env, jContainer, cContainer );

    return ((jint) status);
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_importDirJ 
( JNIEnv * env, jobject obj,
jint jConn, jint jCatType, jstring jCollectionName, jstring jResource, 
jstring localDir, jstring jContainer)
{
    int status;
    int cConn;
    int cCatType;
    const char * cCollectionName;
    const char * cResource;
    const char * cLocalDir;
    const char * cContainer;

    cConn = jConn;
    cCatType = jCatType;
    cCollectionName = (*env)->GetStringUTFChars( env, jCollectionName, 0 );
    cResource = (*env)->GetStringUTFChars( env, jResource, 0 );
    cLocalDir = (*env)->GetStringUTFChars( env, localDir, 0 );
    cContainer = (*env)->GetStringUTFChars( env, jContainer, 0 );

    status = srbImportDirJ (cConn, cCatType, (char *) cCollectionName, 
      (char *) cResource, (char *) cLocalDir, (char *) cContainer);
    (*env)->ReleaseStringUTFChars( env, jCollectionName, cCollectionName );
    (*env)->ReleaseStringUTFChars( env, jResource, cResource );
    (*env)->ReleaseStringUTFChars( env, localDir, cLocalDir );
    (*env)->ReleaseStringUTFChars( env, jContainer, cContainer );

    return ((jint) status);
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_copyToOldObjJ 
( JNIEnv * env, jobject obj,
jint jConn, jint jCatType, jstring jObjID, jstring jReplNum,
jstring jCollectionName, jstring jTargObj, jstring jTargReplNum, 
jstring jTargColl, jstring jTargResource, jstring jContainer)
{
    int status;
    int cConn;
    int cCatType;
    const char * cObjID;
    int cReplNum;
    const char * cCollectionName;
    const char * cTargObj;
    const char * cTargColl;
    const char * myReplNum, *myReplNum1;
    int cTargReplNum;
    const char * cTargResource;
    const char * cContainer;

    cConn = jConn;
    cCatType = jCatType;
    cObjID = (*env)->GetStringUTFChars( env, jObjID, 0 );
    myReplNum = (*env)->GetStringUTFChars( env, jReplNum, 0 );
    cReplNum = atoi ((char *) myReplNum);
    cCollectionName = (*env)->GetStringUTFChars( env, jCollectionName, 0 );
    cTargObj = (*env)->GetStringUTFChars( env, jTargObj, 0 );
    myReplNum1 = (*env)->GetStringUTFChars( env, jTargReplNum, 0 );
    cTargReplNum = atoi ((char *) myReplNum1);
    cTargColl = (*env)->GetStringUTFChars( env, jTargColl, 0 );
    cTargResource = (*env)->GetStringUTFChars( env, jTargResource, 0 );
    cContainer = (*env)->GetStringUTFChars( env, jContainer, 0 );

    status = srbCopyToOldObjJ (cConn, cCatType, (char *) cObjID, cReplNum,
     (char *) cCollectionName, (char *) cTargObj, cTargReplNum, 
      (char *) cTargColl, (char *) cTargResource, (char *) cContainer);

    (*env)->ReleaseStringUTFChars( env, jObjID, cObjID );
    (*env)->ReleaseStringUTFChars( env, jReplNum, myReplNum );
    (*env)->ReleaseStringUTFChars( env, jCollectionName, cCollectionName );
    (*env)->ReleaseStringUTFChars( env, jTargObj, cTargObj );
    (*env)->ReleaseStringUTFChars( env, jTargReplNum, myReplNum1 );
    (*env)->ReleaseStringUTFChars( env, jTargColl, cTargColl );
    (*env)->ReleaseStringUTFChars( env, jContainer, cContainer );
    return ((jint) status);
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_copyDataToCollJ 
( JNIEnv * env, jobject obj,
jint jConn, jint jCatType, jstring jObjID, jstring jReplNum,
jstring jCollectionName, jstring jDataType, jstring jSize,
jstring jTargColl, jstring jTargResource, jstring jContainer)
{
    int status;
    int cConn;
    int cCatType;
    const char * cObjID;
    int cReplNum;
    const char * cCollectionName;
    const char * cDataType;
    const char * cSize;
    const char * cTargColl;
    const char * cTargResource;
    const char * cContainer;
    const char * myReplNum;

    cConn = jConn;
    cCatType = jCatType;
    cObjID = (*env)->GetStringUTFChars( env, jObjID, 0 );
    myReplNum = (*env)->GetStringUTFChars( env, jReplNum, 0 );
    cReplNum = atoi ((char *) myReplNum);
    cCollectionName = (*env)->GetStringUTFChars( env, jCollectionName, 0 );
    cDataType = (*env)->GetStringUTFChars( env, jDataType, 0 );
    cSize = (*env)->GetStringUTFChars( env, jSize, 0 );
    cTargColl = (*env)->GetStringUTFChars( env, jTargColl, 0 );
    cTargResource = (*env)->GetStringUTFChars( env, jTargResource, 0 );
    cContainer = (*env)->GetStringUTFChars( env, jContainer, 0 );

    status = srbCopyDataToCollJ (cConn, cCatType, (char *) cObjID, cReplNum,
     (char *) cCollectionName, (char *) cDataType, (char *) cSize,
      (char *) cTargColl, (char *) cTargResource, (char *) cContainer);

    (*env)->ReleaseStringUTFChars( env, jObjID, cObjID );
    (*env)->ReleaseStringUTFChars( env, jReplNum, myReplNum );
    (*env)->ReleaseStringUTFChars( env, jCollectionName, cCollectionName );
    (*env)->ReleaseStringUTFChars( env, jDataType, cDataType );
    (*env)->ReleaseStringUTFChars( env, jSize, cSize );
    (*env)->ReleaseStringUTFChars( env, jTargColl, cTargColl );
    (*env)->ReleaseStringUTFChars( env, jTargResource, cTargResource );
    (*env)->ReleaseStringUTFChars( env, jContainer, cContainer );

    return ((jint) status);
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_copyCollToCollJ 
( JNIEnv * env, jobject obj,
jint jConn, jint jCatType, jstring jCollectionName, jstring jTargColl, 
jstring jTargResource, jstring jContainer)
{
    int status;
    int cConn;
    int cCatType;
    const char * cCollectionName;
    const char * cTargColl;
    const char * cTargResource;
    const char * cContainer;

    cConn = jConn;
    cCatType = jCatType;
    cCollectionName = (*env)->GetStringUTFChars( env, jCollectionName, 0 );
    cTargColl = (*env)->GetStringUTFChars( env, jTargColl, 0 );
    cTargResource = (*env)->GetStringUTFChars( env, jTargResource, 0 );
    cContainer = (*env)->GetStringUTFChars( env, jContainer, 0 );

    status = srbCopyCollToCollJ (cConn, cCatType,
     (char *) cCollectionName, (char *) cTargColl, (char *) cTargResource,
      (char *) cContainer);
    (*env)->ReleaseStringUTFChars( env, jCollectionName, cCollectionName);
    (*env)->ReleaseStringUTFChars( env, jTargColl, cTargColl );
    (*env)->ReleaseStringUTFChars( env, jTargResource, cTargResource );
    (*env)->ReleaseStringUTFChars( env, jContainer, cContainer );

    return ((jint) status);
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_srbCreateCollectJ 
( JNIEnv * env, jobject obj,
jint jConn, jint jCatType, jstring jParColl, jstring jNewColl)
{
    int status;
    int cConn;
    int cCatType;
    const char * cParColl;
    const char * cNewColl;

    cConn = jConn;
    cCatType = jCatType;
    cParColl = (*env)->GetStringUTFChars( env, jParColl, 0 );
    cNewColl = (*env)->GetStringUTFChars( env, jNewColl, 0 );

    status = srbCreateCollect (g_conn[cConn], cCatType, (char *) cParColl, 
      (char *) cNewColl);

    (*env)->ReleaseStringUTFChars( env, jParColl, cParColl );
    (*env)->ReleaseStringUTFChars( env, jNewColl, cNewColl );

    return ((jint) status);
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_srbModifyDatasetMetaDJ 
(JNIEnv * env, jobject obj,
jint jConn, jint jCatType, jstring jObjID, jstring jCollection, 
jstring jNewValue1, jstring jNewValue2, jint jRetractionType)
{
    int status;
    int cConn;
    int cCatType;
    const char * cObjID;
    const char * cCollection;
    const char * cNewValue1;
    const char * cNewValue2;
    int cRetractionType;


    cConn = jConn;
    cCatType = jCatType;
    cObjID = (*env)->GetStringUTFChars( env, jObjID, 0 );
    cCollection = (*env)->GetStringUTFChars( env, jCollection, 0 );
    cNewValue1 = (*env)->GetStringUTFChars( env, jNewValue1, 0 );
    cNewValue2 = (*env)->GetStringUTFChars( env, jNewValue2, 0 );
    cRetractionType = jRetractionType;

    status = srbModifyDataset (g_conn[cConn], cCatType, (char *) cObjID, 
     (char *) cCollection, "", "", (char *) cNewValue1, (char *) cNewValue2, 
      cRetractionType);

    (*env)->ReleaseStringUTFChars( env, jObjID, cObjID );
    (*env)->ReleaseStringUTFChars( env, jCollection, cCollection );
    (*env)->ReleaseStringUTFChars( env, jNewValue1, cNewValue1 );
    (*env)->ReleaseStringUTFChars( env, jNewValue2, cNewValue2 );
    return ((jint) status);
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_srbModifyCollectMetaDJ 
(JNIEnv * env, jobject obj,
jint jConn, jint jCatType, jstring jCollection, 
jstring jNewValue1, jstring jNewValue2, jstring jNewValue3,
jint jRetractionType)
{
    int status;
    int cConn;
    int cCatType;
    const char * cCollection;
    const char * cNewValue1;
    const char * cNewValue2;
    const char * cNewValue3;
    int cRetractionType;


    cConn = jConn;
    cCatType = jCatType;
    cCollection = (*env)->GetStringUTFChars( env, jCollection, 0 );
    cNewValue1 = (*env)->GetStringUTFChars( env, jNewValue1, 0 );
    cNewValue2 = (*env)->GetStringUTFChars( env, jNewValue2, 0 );
    cNewValue3 = (*env)->GetStringUTFChars( env, jNewValue3, 0 );
    cRetractionType = jRetractionType;

    status = srbModifyCollect (g_conn[cConn], cCatType, 
     (char *) cCollection, (char *) cNewValue1, (char *) cNewValue2, 
      (char *) cNewValue3, cRetractionType);

    (*env)->ReleaseStringUTFChars( env, jCollection, cCollection );
    (*env)->ReleaseStringUTFChars( env, jNewValue1, cNewValue1 );
    (*env)->ReleaseStringUTFChars( env, jNewValue2, cNewValue2 );
    (*env)->ReleaseStringUTFChars( env, jNewValue3, cNewValue3 );
    return ((jint) status);
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_srbObjUnlinkJ 
( JNIEnv * env, jobject obj,
jint jConn, jint jCatType, jstring jObjID, jstring jReplNum, 
jstring jCollectionName)
{
    int status;
    int cConn;
    int cCatType;
    const char * cObjID;
    int cReplNum;
    const char * cCollectionName;
    const char * myReplNum;

    cConn = jConn;
    cCatType = jCatType;
    cObjID = (*env)->GetStringUTFChars( env, jObjID, 0 );
    myReplNum = (*env)->GetStringUTFChars( env, jReplNum, 0 );
    cReplNum = atoi ((char *) myReplNum);
    cCollectionName = (*env)->GetStringUTFChars( env, jCollectionName, 0 );

    status = srbObjUnlinkJ (jConn, (char *) cObjID, cReplNum, 
      (char *) cCollectionName);

    (*env)->ReleaseStringUTFChars( env, jObjID, cObjID );
    (*env)->ReleaseStringUTFChars( env, jReplNum, myReplNum );
    (*env)->ReleaseStringUTFChars( env, jCollectionName, cCollectionName );

    return ((jint) status);
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_srbRmCollJ 
( JNIEnv * env, jobject obj,
jint jConn, jint jCatType, jstring jCollectionName)
{
    int status;
    int cConn;
    int cCatType;
    const char * cCollectionName;

    cConn = jConn;
    cCatType = jCatType;
    cCollectionName = (*env)->GetStringUTFChars( env, jCollectionName, 0 );

    status = srbRmColl (jConn, cCatType, (char *) cCollectionName);

    (*env)->ReleaseStringUTFChars( env, jCollectionName, cCollectionName );

    return ((jint) status);
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_srbUnregCollJ
( JNIEnv * env, jobject obj,
jint jConn, jint jCatType, jstring jCollectionName)
{
    int status;
    int cConn;
    int cCatType;
    const char * cCollectionName;
    mdasC_sql_result_struct myresult;

    cConn = jConn;
    cCatType = jCatType;
    cCollectionName = (*env)->GetStringUTFChars( env, jCollectionName, 0 );

    /* since rmColl expect a mdasC_sql_result_struct as input, have to
     * fake one.
     */

    myresult.result_count = 1;
    myresult.row_count = 1;
    myresult.sqlresult[0].tab_name = dcs_tname[DATA_GRP_NAME];
    myresult.sqlresult[0].att_name = dcs_aname[DATA_GRP_NAME];
    myresult.sqlresult[0].values = (mdasC_handle)  cCollectionName;
    status = rmColl ( g_conn[cConn], cCatType, unreg_FLAG, &myresult, -1 , 0);

    (*env)->ReleaseStringUTFChars( env, jCollectionName, cCollectionName );

    return ((jint) status);
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_srbObjReplicateJ 
( JNIEnv * env, jobject obj, 
jint jConn, jint jCatType, jstring jObjID, jstring jReplNum,
jstring jCollectionName, jstring jResourceName)
{
    int status;
    int cConn;
    int cCatType;
    const char * cObjID;
    int cReplNum;
    const char * myReplNum;
    const char * cCollectionName;
    const char * cResourceName;

    cConn = jConn;
    cCatType = jCatType;
    cObjID = (*env)->GetStringUTFChars( env, jObjID, 0 );
    myReplNum = (*env)->GetStringUTFChars( env, jReplNum, 0 );
    cReplNum = atoi ((char *)myReplNum);
    cCollectionName = (*env)->GetStringUTFChars( env, jCollectionName, 0 );
    cResourceName = (*env)->GetStringUTFChars( env, jResourceName, 0 );

    status = srbObjReplicateJ (cConn, cCatType, (char*) cObjID, cReplNum,
      (char*) cCollectionName, (char*) cResourceName, "");

    (*env)->ReleaseStringUTFChars( env, jObjID, cObjID );
    (*env)->ReleaseStringUTFChars( env, jReplNum, myReplNum );
    (*env)->ReleaseStringUTFChars( env, jCollectionName, cCollectionName );
    (*env)->ReleaseStringUTFChars( env, jResourceName, cResourceName );

    return ((jint) status);
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_srbCollReplicateJ 
( JNIEnv * env, jobject obj, 
jint jConn, jint jCatType, jstring jCollectionName, jstring jResourceName)
{
    int status;
    int cConn;
    int cCatType;
    const char * myReplNum;
    const char * cCollectionName;
    const char * cResourceName;

    cConn = jConn;
    cCatType = jCatType;
    cCollectionName = (*env)->GetStringUTFChars( env, jCollectionName, 0 );
    cResourceName = (*env)->GetStringUTFChars( env, jResourceName, 0 );

    status = replicateColl (g_conn[cConn], cCatType, R_FLAG,
      (char*) cCollectionName, (char*) cResourceName);

    (*env)->ReleaseStringUTFChars( env, jCollectionName, cCollectionName );
    (*env)->ReleaseStringUTFChars( env, jResourceName, cResourceName );

    return ((jint) status);
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_getMoreDataRowsJ
( JNIEnv * env, jobject obj,
        jint jConn, jint jCatType)
{
    int count;
    int status;
    int cConn;
    int cCatType;

    cConn = jConn;
    cCatType = jCatType;

    while (g_qBuf[cConn]->dataResult.continuation_index >= 0) {
	clearSqlResult (&g_qBuf[cConn]->dataResult);
        status = srbGetMoreRows (g_conn[cConn], cCatType, 
	 g_qBuf[cConn]->dataResult.continuation_index, 
	  &g_qBuf[cConn]->dataResult, MY_MAX_ROW);

        if (status < 0)
            return ((jint) 0);
	filterDeleted (&g_qBuf[cConn]->dataResult);
	if (g_qBuf[cConn]->dataResult.row_count > 0) {
    	    for (i = 0; i < nDataMap; i++) {
        	dataMap[i].resultVal = getFromResultStruct (
		 &g_qBuf[cConn]->dataResult, dcs_tname[dataMap[i].selInx], 
		  dcs_aname[dataMap[i].selInx]);
        	if (dataMap[i].resultVal == NULL) {
            	    fprintf (stderr, "Unable to map dataMap selInx %d\n",
              	     dataMap[i].selInx);
		    return 0;
		}
            }
            return ((jint) g_qBuf[cConn]->dataResult.row_count);
	}
    }
    return 0;
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_getMoreCollRowsJ
( JNIEnv * env, jobject obj,
        jint jConn, jint jCatType)
{
    int count;
    int status;
    int cConn;
    int cCatType;

    cConn = jConn;
    cCatType = jCatType;

    while (g_qBuf[cConn]->collResult.continuation_index >= 0) {
        clearSqlResult (&g_qBuf[cConn]->collResult);
        status = srbGetMoreRows (g_conn[cConn], cCatType, 
         g_qBuf[cConn]->collResult.continuation_index, 
	  &g_qBuf[cConn]->collResult, MY_MAX_ROW);

        if (status < 0)
            return ((jint) 0);

	filterDeleted (&g_qBuf[cConn]->collResult);
        if (g_qBuf[cConn]->collResult.row_count > 0) {
            for (i = 0; i < nCollMap; i++) {
                collMap[i].resultVal = getFromResultStruct (
		 &g_qBuf[cConn]->collResult,
                  dcs_tname[collMap[i].selInx], dcs_aname[collMap[i].selInx]);
                if (collMap[i].resultVal == NULL) {
                    fprintf (stderr, "Unable to map collMap selInx %d\n",
                     collMap[i].selInx);
                    return 0;
                }
            }
            return ((jint) g_qBuf[cConn]->collResult.row_count);
        }
    }
    return 0;
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_getMoreGenQueRowsJ
( JNIEnv * env, jobject obj,
        jint jConn, jint jCatType)
{
    int count;
    int status;
    int cConn;
    int cCatType;

    cConn = jConn;
    cCatType = jCatType;

    while (g_qBuf[cConn]->genResult.continuation_index >= 0) {
        clearSqlResult (&g_qBuf[cConn]->genResult);
        status = srbGetMoreRows (g_conn[cConn], cCatType,
         g_qBuf[cConn]->genResult.continuation_index, 
	  &g_qBuf[cConn]->genResult, MY_MAX_ROW);

        if (status < 0)
            return ((jint) 0);
        filterDeleted (&g_qBuf[cConn]->genResult);
        if (g_qBuf[cConn]->genResult.row_count > 0) {
            for (i = 0; i < nGenAttri; i++) {
                genResultMap[i].resultVal = getFromResultStruct (
		  &g_qBuf[cConn]->genResult, dcs_tname[genResultMap[i].selInx], 
		   dcs_aname[genResultMap[i].selInx]);
                if (genResultMap[i].resultVal == NULL) {
                    fprintf (stderr, "Unable to map genResultMap selInx %d\n",
                     genResultMap[i].selInx);
                    return 0;
                }
            }
            return ((jint) g_qBuf[cConn]->genResult.row_count);
	}
    }
    return 0;
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_queryDataInCollJ
( JNIEnv * env, jobject obj,
        jint jConn, jint jCatType, jstring jCollectionName)
{
    int count;
    int status;
    int cConn;
    int cCatType;
    const char * cCollectionName;

    cConn = jConn;
    cCatType = jCatType;
    cCollectionName = (*env)->GetStringUTFChars( env, jCollectionName, 0 );

    status = queryDataInCollJ (cConn, cCatType, (char*) cCollectionName,
      &g_qBuf[cConn]->dataResult);

    (*env)->ReleaseStringUTFChars( env, jCollectionName, cCollectionName );

    if (status < 0)
        return ((jint) 0);
    else
        return ((jint) g_qBuf[cConn]->dataResult.row_count);
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_queryDataJ
( JNIEnv * env, jobject obj,
        jint jConn, jint jCatType, jstring jObjID, jstring jCollectionName)
{
    int count;
    int status;
    int cConn;
    int cCatType;
    const char * cObjID;
    const char * cCollectionName;

    cConn = jConn;
    cCatType = jCatType;
    cObjID = (*env)->GetStringUTFChars( env, jObjID, 0 );
    cCollectionName = (*env)->GetStringUTFChars( env, jCollectionName, 0 );

    status = queryDataJ (cConn, cCatType, (char*) cObjID,
      (char*) cCollectionName, &g_qBuf[cConn]->dataResult);

    (*env)->ReleaseStringUTFChars( env, jObjID, cObjID );
    (*env)->ReleaseStringUTFChars( env, jCollectionName, cCollectionName );

    if (status < 0)
        return ((jint) 0);
    else
        return ((jint) g_qBuf[cConn]->dataResult.row_count);
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_containerCreateJ 
( JNIEnv * env, jobject obj,
jint jConn, jint jCatType, jstring jContName, jstring jDataType,
jstring jLogResc, jint jSize)
{
    int status;
    int cConn;
    int cCatType;
    const char * cContName;
    const char * cDataType;
    const char * cLogResc;
    int cSize;

    cConn = jConn;
    cCatType = jCatType;
    cContName = (*env)->GetStringUTFChars( env, jContName, 0 );
    cDataType = (*env)->GetStringUTFChars( env, jDataType, 0 );
    cLogResc = (*env)->GetStringUTFChars( env, jLogResc, 0 );
    cSize = jSize;

    status = srbContainerCreate (g_conn[cConn], cCatType, (char *) cContName, 
      (char *) cDataType, (char *) cLogResc, cSize);

    (*env)->ReleaseStringUTFChars( env, jContName, cContName );
    (*env)->ReleaseStringUTFChars( env, jDataType, cDataType );
    (*env)->ReleaseStringUTFChars( env, jLogResc, cLogResc );

    return ((jint) status);
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_containerDeleteJ 
( JNIEnv * env, jobject obj,
jint jConn, jint jCatType, jstring jContName, jint jForceFlag)
{
    int status;
    int cConn;
    int cCatType;
    const char * cContName;
    int flagval = 0;

    cConn = jConn;
    cCatType = jCatType;
    cContName = (*env)->GetStringUTFChars( env, jContName, 0 );

    if (jForceFlag == 1) 
	flagval |= F_FLAG;
 
    status = rmContainer (g_conn[cConn], cCatType, (char *) cContName, flagval);

    (*env)->ReleaseStringUTFChars( env, jContName, cContName );

    return ((jint) status);
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_containerSyncJ
( JNIEnv * env, jobject obj,
jint jConn, jint jCatType, jstring jContName, jint jPurgeFlag)
{
    int status;
    int cConn;
    int cCatType;
    const char * cContName;
    int cPurgeFlag;

    cConn = jConn;
    cCatType = jCatType;
    cContName = (*env)->GetStringUTFChars( env, jContName, 0 );
    cPurgeFlag = jPurgeFlag;


    status = srbSyncContainer (g_conn[cConn], cCatType, (char *) cContName, 
     cPurgeFlag);

    (*env)->ReleaseStringUTFChars( env, jContName, cContName );

    return ((jint) status);
}

/************************************************************************/
/*                                                                      */
/*  Given a valid SRB connection ID, this routine generates a list      */
/*  of member names and member types for a specified collection name.   */
/*                                                                      */
/************************************************************************/
JNIEXPORT jstring JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_getFileListItem
( JNIEnv * env, jobject obj,
	jint jConn, jint index )
{
	int cid = jConn;

	return (*env)->NewStringUTF( env, g_qBuf[cid]->fileList[index] );
}

/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_srbObjCopyNewJ
( JNIEnv * env, jobject obj,
	jint jConn, jstring jOldCollection, jstring jOldDatasetName,
	jstring jNewCollection, jstring jNewDatasetName,
	jstring jNewResource )
{
	int result = 0;
	int cConn = jConn;

	char * cOldCollection =
		(char*) (*env)->GetStringUTFChars( env, jOldCollection,  0 );
	char * cOldDatasetName =
		(char*) (*env)->GetStringUTFChars( env, jOldDatasetName, 0 );
	char * cNewCollection =
		(char*) (*env)->GetStringUTFChars( env, jNewCollection,  0 );
	char * cNewDatasetName =
		(char*) (*env)->GetStringUTFChars( env, jNewDatasetName, 0 );
	char * cNewResource =
		(char*) (*env)->GetStringUTFChars( env, jNewResource,    0 );

	result = srbObjCopyNewJ( cConn, cOldCollection, cOldDatasetName,
		cNewCollection, cNewDatasetName, cNewResource );

	(*env)->ReleaseStringUTFChars( env, jOldCollection,  cOldCollection  );
	(*env)->ReleaseStringUTFChars( env, jOldDatasetName, cOldDatasetName );
	(*env)->ReleaseStringUTFChars( env, jNewCollection,  cNewCollection  );
	(*env)->ReleaseStringUTFChars( env, jNewDatasetName, cNewDatasetName );
	(*env)->ReleaseStringUTFChars( env, jNewResource,    cNewResource    );

	return( (jint) result );
}

/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_exportToFileJ
( JNIEnv * env, jobject obj,
	jint jConn, jint jCatType, jstring jObjID, jstring jReplNum,
	jstring jCollectionName, jstring jTargFile, jint jForceFlag)
{
    int status;
    int cConn;
    int cCatType;
    const char * cObjID;
    int cReplNum;
    const char * cCollectionName;
    const char * cTargFile;
    const char * myReplNum;
    int cForceFlag;		/* overwrite if local file exist */

    cConn = jConn;
    cCatType = jCatType;
    cObjID = (*env)->GetStringUTFChars( env, jObjID, 0 );
    myReplNum = (*env)->GetStringUTFChars( env, jReplNum, 0 );
    cReplNum = atoi ((char *) myReplNum);
    cCollectionName = (*env)->GetStringUTFChars( env, jCollectionName, 0 );
    cTargFile = (*env)->GetStringUTFChars( env, jTargFile, 0 );
    cForceFlag = jForceFlag;

    status = srbDataToFileCopyJ (cConn, cCatType, (char *) cObjID, 
     cReplNum, (char *) cCollectionName, (char *) cTargFile, cForceFlag);

    (*env)->ReleaseStringUTFChars( env, jObjID, cObjID );
    (*env)->ReleaseStringUTFChars( env, jCollectionName, cCollectionName );
    (*env)->ReleaseStringUTFChars( env, jReplNum, myReplNum );
    (*env)->ReleaseStringUTFChars( env, jTargFile, cTargFile );

    return ((jint) status);
}

/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_exportToDirJ
( JNIEnv * env, jobject obj,
        jint jConn, jint jCatType, jstring jObjID, jstring jReplNum,
        jstring jCollectionName, jstring jTargDir)
{
    int status;
    int cConn;
    int cCatType;
    const char * cObjID;
    int cReplNum;
    const char * cCollectionName;
    const char * cTargDir;
    const char * myReplNum;

    cConn = jConn;
    cCatType = jCatType;
    cObjID = (*env)->GetStringUTFChars( env, jObjID, 0 );
    myReplNum = (*env)->GetStringUTFChars( env, jReplNum, 0 );
    cReplNum = atoi ((char *) myReplNum);
    cCollectionName = (*env)->GetStringUTFChars( env, jCollectionName, 0 );
    cTargDir = (*env)->GetStringUTFChars( env, jTargDir, 0 );

    status = srbDataToDirCopyJ (cConn, cCatType, (char *) cObjID,
     cReplNum, (char *) cCollectionName, (char *) cTargDir);

    (*env)->ReleaseStringUTFChars( env, jObjID, cObjID );
    (*env)->ReleaseStringUTFChars( env, jCollectionName, cCollectionName );
    (*env)->ReleaseStringUTFChars( env, jReplNum, myReplNum );
    (*env)->ReleaseStringUTFChars( env, jTargDir, cTargDir );

    return ((jint) status);
}

/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_exportCollToDirJ
( JNIEnv * env, jobject obj,
        jint jConn, jint jCatType, jstring jCollection, jstring jTargDir)
{
    int status;
    int cConn;
    int cCatType;
    const char * cCollection;
    const char * cTargDir;

    cConn = jConn;
    cCatType = jCatType;
    cCollection = (*env)->GetStringUTFChars( env, jCollection, 0 );
    cTargDir = (*env)->GetStringUTFChars( env, jTargDir, 0 );

    status = srbCollToDirCopyJ (cConn, cCatType, (char *) cCollection, 
     (char *) cTargDir);

    (*env)->ReleaseStringUTFChars( env, jCollection, cCollection );
    (*env)->ReleaseStringUTFChars( env, jTargDir, cTargDir );

    return ((jint) status);
}


/************************************************************************/
/*                                                                      */
/*  Given a valid SRB connection ID, this routine generates a list      */
/*  of member names and member types for a specified collection name.   */
/*                                                                      */
/************************************************************************/
JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_srbListResourceJ
( JNIEnv * env, jobject obj,
	jint jConn, jint index )
{
	int cConn;

	cConn = jConn;
#ifdef DEBUG
fprintf( stderr, "DEBUG: SrbJavaGlue.c Java_edu_sdsc_SrbGlue_SrbJavaGlue_srbListResourceJ\n" );
#endif

	return( (jint) srbListResourceJ(cConn) );
}

/************************************************************************/
/*                                                                      */
/* Given the row index, return the attributes of the data 		*/ 
/*                                                                      */
/************************************************************************/
JNIEXPORT jstring JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_getDataResultJ
( JNIEnv * env, jobject obj,
        jint metaIndex, jint rowIndex )
{
    char *ptr;

    if (metaIndex < 0 || metaIndex >= nDataMap) {
	fprintf (stderr, 
	 "Java_edu_sdsc_SrbGlue_SrbJavaGlue_getDataResult: metaIndex %d out of range\n",
	  metaIndex);
	return (*env)->NewStringUTF( env, "");
    }
    ptr = dataMap[metaIndex].resultVal;
    ptr += rowIndex * MAX_DATA_SIZE;
    return (*env)->NewStringUTF( env, ptr);
}

/************************************************************************/
/*                                                                      */
/* Given the row index, return the attributes of the collection               */
/*                                                                      */
/************************************************************************/
JNIEXPORT jstring JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_getCollResultJ
( JNIEnv * env, jobject obj,
        jint metaIndex, jint rowIndex )
{
    char *ptr;

    if (metaIndex < 0 || metaIndex >= nCollMap) {
	fprintf (stderr, 
	 "Java_edu_sdsc_SrbGlue_SrbJavaGlue_getDataResult: metaIndex %d out of range\n",
	  metaIndex);
	return (*env)->NewStringUTF( env, "");
    }

    ptr = collMap[metaIndex].resultVal;
    ptr += rowIndex * MAX_DATA_SIZE;
    return (*env)->NewStringUTF( env, ptr);
}

/************************************************************************/
/*                                                                      */
/* Given the row index, return the attributes of a general query        */
/*                                                                      */
/************************************************************************/
JNIEXPORT jstring JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_getGenQueResultJ
( JNIEnv * env, jobject obj,
        jint metaIndex, jint rowIndex )
{
    char *ptr;

    if (metaIndex < 0 || metaIndex >= nGenAttri) {
        fprintf (stderr,
         "Java_edu_sdsc_SrbGlue_SrbJavaGlue_getGenQueResultJ: metaIndex %d out of range\n",
	  metaIndex);
        return (*env)->NewStringUTF( env, "");
    }

    if (rowIndex < 0 || rowIndex >= nGenRowCnt) {
        fprintf (stderr,
         "Java_edu_sdsc_SrbGlue_SrbJavaGlue_getGenQueResultJ: rowIndex %d out of range\n",
	  rowIndex);
        return (*env)->NewStringUTF( env, "");
    }

    ptr = genResultMap[metaIndex].resultVal;
    ptr += rowIndex * MAX_DATA_SIZE;
    return (*env)->NewStringUTF( env, ptr);
}

/************************************************************************/
/*                                                                      */
/*  Given a valid SRB connection ID, this routine generates a list      */
/*  of member names and member types for a specified collection name.   */
/*                                                                      */
/************************************************************************/
JNIEXPORT jstring JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_getResListItem
( JNIEnv * env, jobject obj,
	jint jConn, jint index )
{
	int cid = jConn;

	return (*env)->NewStringUTF( env, g_qBuf[cid]->resList[index] );
}


/************************************************************************/
/*                                                                      */
/*  Get the default collection from the CL_MDAS_ENV_FILE file           */
/*                                                                      */
/************************************************************************/
JNIEXPORT jstring JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_getDefCollection
( JNIEnv * env, jobject obj)
{
    char *myDefColl;
    struct mdasEnvData *myMdasEnvData;

    myMdasEnvData = readMdasParam ();
    if (myMdasEnvData == NULL) {
        return (NULL);
    }

    myDefColl = strdup (myMdasEnvData->defColl);
    return (*env)->NewStringUTF( env, myDefColl);
}

/************************************************************************/
/*                                                                      */
/*  Get the default collection from the CL_MDAS_ENV_FILE file           */
/*                                                                      */
/************************************************************************/
JNIEXPORT jstring JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_getDefResource
( JNIEnv * env, jobject obj)
{
    char *myDefResource;
    struct mdasEnvData *myMdasEnvData;

    myMdasEnvData = readMdasParam ();
    myDefResource = strdup (myMdasEnvData->defRes);
    return (*env)->NewStringUTF( env, myDefResource);
}

/************************************************************************/
/*                                                                      */
/*  Get the client userName				                */
/*                                                                      */
/************************************************************************/
JNIEXPORT jstring JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_getUser
( JNIEnv * env, jobject obj,
   jint jConn)
{
    char *userName;
    int cConn;

    cConn = jConn;
    if (g_conn[cConn]->clientUser == NULL) {
        userName = g_conn[cConn]->proxyUser->userName;
    } else {
        userName = g_conn[cConn]->clientUser->userName;
    }

    return (*env)->NewStringUTF( env, userName);
}

/************************************************************************/
/*                                                                      */
/*  Get the client domainName                                           */
/*                                                                      */
/************************************************************************/
JNIEXPORT jstring JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_getDomain
( JNIEnv * env, jobject obj,
  jint jConn)
{
    char *domainName;
    int cConn;

    cConn = jConn;
    if (g_conn[cConn]->clientUser == NULL) {
        domainName = g_conn[cConn]->proxyUser->domainName;
    } else {
        domainName = g_conn[cConn]->clientUser->domainName;
    }

    return (*env)->NewStringUTF( env, domainName);
}

/************************************************************************/
/*                                                                      */
/*  Get the default LocalCacheDir           				*/
/*                                                                      */
/************************************************************************/
JNIEXPORT jstring JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_getLocalCacheDir
( JNIEnv * env, jobject obj)
{
    return (*env)->NewStringUTF( env, LocalCacheDir);
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_getNDataAttriJ
( JNIEnv * env, jobject obj)
{
    return ((jint) nDataMap);
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_getNCollAttriJ
( JNIEnv * env, jobject obj)
{
    return ((jint) nCollMap);
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_srbGenQuery 
( JNIEnv * env, jobject obj,
   jint jConn, jint jCatType, jobjectArray jQval, jintArray jQvalInx, 
     jintArray jSelInx) 
{
    int cConn;
    int cCatType;
    jobject myObject;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    char *tmpStr;
    int qvalLen;
    int i;
    jint *qvalInx, *selInx;
    int selval[MAX_DCS_NUM];
    int status;
    
    cConn = jConn;
    cCatType = jCatType;

    nGenAttri = (*env)->GetArrayLength( env, jSelInx);

    selInx = (*env)->GetIntArrayElements( env, jSelInx, 0);

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }

    for (i = 0; i < nGenAttri; i++) {
	genResultMap[i].selInx = selInx[i];
	selval[selInx[i]] = 1;
    }
    
    qvalLen = (*env)->GetArrayLength( env, jQval);
    qvalInx = (*env)->GetIntArrayElements( env, jQvalInx, 0);

    for (i = 0; i < qvalLen; i++) {
	myObject = (*env)->GetObjectArrayElement (env, jQval, i);
	tmpStr = (char *) (*env)->GetStringUTFChars( env, myObject, 0);
	strcpy (qval[qvalInx[i]], tmpStr);
	(*env)->ReleaseStringUTFChars( env, myObject, tmpStr);
    }

    status = srbGetDataDirInfo (g_conn[cConn], cCatType, qval, selval, 
     &g_qBuf[cConn]->genResult, MY_MAX_ROW);

    (*env)->ReleaseIntArrayElements( env, jSelInx, selInx, 0);
    (*env)->ReleaseIntArrayElements( env, jQvalInx, qvalInx, 0);

    if (status < 0) {
        return ((jint) 0);
    } else {
	filterDeleted (&g_qBuf[cConn]->genResult);
	nGenRowCnt = g_qBuf[cConn]->genResult.row_count;
        for (i = 0; i < nGenAttri; i++) {
            genResultMap[i].resultVal = getFromResultStruct (
	      &g_qBuf[cConn]->genResult,
               dcs_tname[genResultMap[i].selInx], 
		dcs_aname[genResultMap[i].selInx]);
            if (genResultMap[i].resultVal == NULL) {
            	fprintf (stderr, "Unable to map genResultMap selInx %d\n",
                 genResultMap[i].selInx);
		return 0;
            }
    	}
        return ((jint) g_qBuf[cConn]->genResult.row_count);
    }

}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_srbRegisterDatasetJ
(JNIEnv * env, jobject obj,
jint jConn, jint jCatType, jstring jObjID, jstring jDataType,
jstring jResource, jstring jCollection, jstring jPathName,
jint jDataSize)
{
    int status;
    int cConn;
    int cCatType;
    const char * cObjID;
    const char * cResource;
    const char * cDataType;
    const char * cCollection;
    const char * cPathName;
    int cDataSize;
    char tmpDataType[MAX_TOKEN];



    cConn = jConn;
    cCatType = jCatType;
    cObjID = (*env)->GetStringUTFChars( env, jObjID, 0 );
    cResource = (*env)->GetStringUTFChars( env, jResource, 0 );
    cDataType = (*env)->GetStringUTFChars( env, jDataType, 0 );
    cCollection = (*env)->GetStringUTFChars( env, jCollection, 0 );
    cPathName = (*env)->GetStringUTFChars( env, jPathName, 0 );
    cDataSize = jDataSize;

    if (strlen(cDataType) == 0) 
        trylocaldatatype(tmpDataType, (char *) cPathName);
     else
        strcpy(tmpDataType, cDataType);

    status = srbRegisterDataset (g_conn[cConn], cCatType, (char *) cObjID,
     (char *) tmpDataType, (char *) cResource, (char *) cCollection,
      (char *) cPathName, cDataSize);

    (*env)->ReleaseStringUTFChars( env, jObjID, cObjID );
    (*env)->ReleaseStringUTFChars( env, jResource, cResource );
    (*env)->ReleaseStringUTFChars( env, jDataType, cDataType );
    (*env)->ReleaseStringUTFChars( env, jCollection, cCollection );
    (*env)->ReleaseStringUTFChars( env, jPathName, cPathName );
    return ((jint) status);
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_srbUnregisterDatasetJ
( JNIEnv * env, jobject obj,
jint jConn, jint jCatType, jstring jObjID, jstring jReplNum,
jstring jCollectionName)
{
    int status;
    int cConn;
    int cCatType;
    const char * cObjID;
    int cReplNum;
    const char * cCollectionName;
    const char * myReplNum;
    char tmpPath[MAX_TOKEN];

    cConn = jConn;
    cCatType = jCatType;
    cObjID = (*env)->GetStringUTFChars( env, jObjID, 0 );
    myReplNum = (*env)->GetStringUTFChars( env, jReplNum, 0 );
    cReplNum = atoi ((char *) myReplNum);
    cCollectionName = (*env)->GetStringUTFChars( env, jCollectionName, 0 );

    sprintf (tmpPath, "%s&COPY=%i", cObjID, cReplNum);

    status = srbUnregisterDataset (g_conn[cConn], tmpPath, 
     (char *) cCollectionName);

    (*env)->ReleaseStringUTFChars( env, jObjID, cObjID );
    (*env)->ReleaseStringUTFChars( env, jReplNum, myReplNum );
    (*env)->ReleaseStringUTFChars( env, jCollectionName, cCollectionName );

    return ((jint) status);
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_srbRegisterDirJ
(JNIEnv * env, jobject obj,
jint jConn, jint jCatType, jstring jSrcDirPath, jstring jTargColl,
jstring jResource)
{
    int status;
    int cConn;
    int cCatType;
    const char * cSrcDirPath;
    const char * cTargColl;
    const char * cResource;
    int cDataSize;
    char tmpDataType[MAX_TOKEN];
    srbPathName srbPathName;



    cConn = jConn;
    cCatType = jCatType;
    cSrcDirPath = (*env)->GetStringUTFChars( env, jSrcDirPath, 0 );
    cTargColl = (*env)->GetStringUTFChars( env, jTargColl, 0 );
    cResource = (*env)->GetStringUTFChars( env, jResource, 0 );

    srbPathName.inpArgv = (char *) cSrcDirPath;

    /* fill in size, etc */
    status = chkFileName (&srbPathName);

    if (status < 0)
        return ((jint) status);

    status = registerDir (g_conn[cConn], cCatType, &srbPathName,
     (char *) cTargColl, (char *) cResource);

    (*env)->ReleaseStringUTFChars( env, jSrcDirPath, cSrcDirPath );
    (*env)->ReleaseStringUTFChars( env, jTargColl, cTargColl );
    (*env)->ReleaseStringUTFChars( env, jResource, cResource );
    return ((jint) status);
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_ingestUserJ
(JNIEnv * env, jobject obj,
jint jConn, jint jCatType,
jstring jUserName, jstring jPasswd, jstring jDomain,
jstring jUserType, jstring jUserAddr, jstring jUserPhone, jstring jUserEmail,
jstring jParentColl)
{
    int status;
    int cConn;
    int cCatType;
    const char * cUserName;
    const char * cPasswd;
    const char * cDomain;
    const char * cUserType;
    const char * cUserAddr;
    const char * cUserPhone;
    const char * cUserEmail;
    const char * cParentColl;
    char *admUserName, *admDomainName, *admUserAuth; /* Admin User ID */
    char temp1[MAX_TOKEN];
    char collname[MAX_TOKEN];
    char clErrorMsg[MAX_TOKEN];

    cConn = jConn;
    cCatType = jCatType;
    cUserName = (*env)->GetStringUTFChars( env, jUserName, 0 );
    cPasswd = (*env)->GetStringUTFChars( env, jPasswd, 0 );
    cDomain = (*env)->GetStringUTFChars( env, jDomain, 0 );
    cUserType = (*env)->GetStringUTFChars( env, jUserType, 0 );
    cUserAddr = (*env)->GetStringUTFChars( env, jUserAddr, 0 );
    cUserPhone = (*env)->GetStringUTFChars( env, jUserPhone, 0 );
    cUserEmail = (*env)->GetStringUTFChars( env, jUserEmail, 0 );
    cParentColl = (*env)->GetStringUTFChars( env, jParentColl, 0 );

    status = srbRegisterUser(g_conn[cConn], 0, (char *)cUserName, 
                             (char *)cDomain,  (char *)cPasswd, 
                             (char *)cUserType, (char *)cUserAddr,  
                             (char *)cUserPhone, (char *)cUserEmail);
    if (status != 0) {
      fprintf(stderr, "ingestUser Register Error: %i\n",status);
      goto done;
    }

#if 0
    sprintf(collname, "%s.%s", cUserName, cDomain);
    status = srbCreateCollect(g_conn[cConn], 0, (char *)cParentColl, collname);
    if (status < 0) {
      fprintf(stderr, "ingestUser MakeCollection Error: %i\n", status);
      goto done;
    }

    sprintf(temp1, "%s/%s", cParentColl, collname);
    status = srbModifyCollect( g_conn[cConn], 0, temp1, 
                               (char *) cUserName, (char *) cDomain, "all",
                               D_INSERT_COLL_ACCS);
    if (status != 0) {
      fprintf (stderr, "ingestUser ModifyCollect Error: %i\n", status);
      goto done;
    }

    /* now make the container collection */
    status = srbCreateCollect(g_conn[cConn], 0, 
			      (char *) CONTAINER_HOME, collname);
    if (status < 0) {
      fprintf(stderr, "ingestUser Make container Collection Error: %i\n", 
              status);
      goto done;
    }

    sprintf(temp1, "%s/%s", CONTAINER_HOME, collname);
    status = srbModifyCollect(g_conn[cConn], 0, temp1,
                                 (char *) cUserName, (char *) cDomain, "all",
                                 D_INSERT_COLL_ACCS);
    if (status != 0) {
      fprintf (stderr, "ingestUser Modify Container Collect Error: %i\n", 
               status);
      goto done;
    }

    /* if sysadmin, give 'all' permission for home collection */
    if (!strcmp(cUserType,"sysadmin")) {
      status = srbModifyCollect(g_conn[cConn], 0, (char *) cParentColl,
                                (char *) cUserName, (char *) cDomain,
                                "all",  D_INSERT_COLL_ACCS);
      if (status != 0) {
        fprintf(stderr, "ingestUser SysAdmin Accs Modify Error for %s: %i\n",
                cParentColl,status);
        goto done;
      }
    }
    /* make user part of his domain-named user group    */
    sprintf(temp1,"%s@%s",(char *) cUserName, (char *) cDomain);
    status = srbModifyUser (g_conn[cConn], 0, temp1,(char *) cDomain, 
                            U_INSERT_GROUP);
    if (status != 0) {
      fprintf(stderr, 
              "ingestUser: Insert User Into Domain Group Error: %i\n",status);
      goto done;
    }
#endif

 done:

    (*env)->ReleaseStringUTFChars( env, jUserName, cUserName );
    (*env)->ReleaseStringUTFChars( env, jPasswd, cPasswd );
    (*env)->ReleaseStringUTFChars( env, jDomain, cDomain );
    (*env)->ReleaseStringUTFChars( env, jUserType, cUserType );
    (*env)->ReleaseStringUTFChars( env, jUserAddr, cUserAddr );
    (*env)->ReleaseStringUTFChars( env, jUserPhone, cUserPhone );
    (*env)->ReleaseStringUTFChars( env, jUserEmail, cUserEmail );
    (*env)->ReleaseStringUTFChars( env, jParentColl, cParentColl );
    return ((jint) status);
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_ingestGroupJ
(JNIEnv * env, jobject obj,
jint jConn, jint jCatType,
jstring jGroupName, jstring jPasswd,
jstring jGroupAddr, jstring jGroupPhone, jstring jGroupEmail)
{
    int status;
    int cConn;
    int cCatType;
    const char * cGroupName;
    const char * cPasswd;
    const char * cGroupAddr;
    const char * cGroupPhone;
    const char * cGroupEmail;
    const char * cParentColl;

    cConn = jConn;
    cCatType = jCatType;
    cGroupName = (*env)->GetStringUTFChars( env, jGroupName, 0 );
    cPasswd = (*env)->GetStringUTFChars( env, jPasswd, 0 );
    cGroupAddr = (*env)->GetStringUTFChars( env, jGroupAddr, 0 );
    cGroupPhone = (*env)->GetStringUTFChars( env, jGroupPhone, 0 );
    cGroupEmail = (*env)->GetStringUTFChars( env, jGroupEmail, 0 );

    status = srbRegisterUserGrp(g_conn[cConn], cCatType, (char *)cGroupName, 
                               (char *)cPasswd, 
                               (char *)DEF_GROUP_TYPE, (char *)cGroupAddr,  
                               (char *)cGroupPhone, (char *)cGroupEmail);
    if (status != 0) {
      fprintf(stderr, "ingestGroup Register Error: %i\n",status);
    }

    (*env)->ReleaseStringUTFChars( env, jGroupName, cGroupName );
    (*env)->ReleaseStringUTFChars( env, jPasswd, cPasswd );
    (*env)->ReleaseStringUTFChars( env, jGroupAddr, cGroupAddr );
    (*env)->ReleaseStringUTFChars( env, jGroupPhone, cGroupPhone );
    (*env)->ReleaseStringUTFChars( env, jGroupEmail, cGroupEmail );

    return ((jint) status);
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_ingestLocJ
(JNIEnv * env, jobject obj,
jint jConn, jint jCatType,
jstring jLocName, jstring jFullAddr, jstring jParentLoc,
jstring jServerUser, jstring jServerUserDomain)
{
    int status;
    int cConn;
    int cCatType;
    const char * cLocName;
    const char * cFullAddr;
    const char * cParentLoc;
    const char * cServerUser;
    const char * cServerUserDomain;
    char *admUserName, *admDomain, *admUserAuth; /* Admin User ID */
    char *serverUser, *serverUserDomain;
    char clErrorMsg[MAX_TOKEN];
    struct mdasEnvData *myMdasEnvData;

    cConn = jConn;
    cCatType = jCatType;
    cLocName = (*env)->GetStringUTFChars( env, jLocName, 0 );
    cFullAddr = (*env)->GetStringUTFChars( env, jFullAddr, 0 );
    cParentLoc = (*env)->GetStringUTFChars( env, jParentLoc, 0 );
    cServerUser = (*env)->GetStringUTFChars( env, jServerUser, 0 );
    cServerUserDomain = (*env)->GetStringUTFChars( env, jServerUserDomain, 0 );

    myMdasEnvData = readMdasParam ();   /* read the .MdasEnv File */
    if (myMdasEnvData == NULL) {
        return (CLI_ERR_MALLOC);
    }

    admUserName = clGetUserName(myMdasEnvData, clErrorMsg);
    if (admUserName == NULL) {
        fprintf (stderr,
         "Unable to get the registrar's userName.\n");
        return(CLI_ERR_INVAILD_USER);
    }

    admDomain = clGetDomainName(myMdasEnvData, clErrorMsg);
    if (admDomain == NULL) {
        fprintf (stderr,
         "Unable to get the registrar's domainName.\n");
        return(CLI_ERR_INVAILD_DOMAIN);
    }

    if (cServerUser[0] == '\0') {
	serverUser = admUserName;
	serverUserDomain = admDomain;
    } else {
        serverUser = (char *) cServerUser;
        serverUserDomain = (char *) cServerUserDomain;
    }

    status = srbRegisterLocation(g_conn[cConn],
				 (char *) cLocName, (char *) cFullAddr, 
				 (char *) cParentLoc,
				 serverUser, serverUserDomain);
    if (status != 0) {
        fprintf(stderr, "ingestLocJ Register Error: %i\n",status);
    }

    (*env)->ReleaseStringUTFChars( env, jLocName, cLocName );
    (*env)->ReleaseStringUTFChars( env, jFullAddr, cFullAddr );
    (*env)->ReleaseStringUTFChars( env, jParentLoc, cParentLoc );
    (*env)->ReleaseStringUTFChars( env, jServerUser, cServerUser );
    (*env)->ReleaseStringUTFChars( env, jServerUserDomain, cServerUserDomain );

    return ((jint) status);
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_ingestTokenJ
(JNIEnv * env, jobject obj,
jint jConn, jint jCatType,
jstring jTypeName, jstring jNewValue, jstring jParentValue)
{
    int status;
    int cConn;
    int cCatType;
    const char * cTypeName;
    const char * cNewValue;
    const char * cParentValue;
    char *admUserName, *admDomain, *admUserAuth; /* Admin User ID */
    char clErrorMsg[MAX_TOKEN];
    char gPId[MAX_TOKEN];

    cConn = jConn;
    cCatType = jCatType;
    cTypeName = (*env)->GetStringUTFChars( env, jTypeName, 0 );
    cNewValue = (*env)->GetStringUTFChars( env, jNewValue, 0 );
    cParentValue = (*env)->GetStringUTFChars( env, jParentValue, 0 );

    status = srbIngestToken(g_conn[cConn], (char *) cTypeName, 
			    (char *) cNewValue,  (char *) cParentValue);

    if (status != 0) {
        fprintf(stderr, "ingestTokenJ srbIngestToken Error: %i\n",status);
        goto done;
    }
    if (!strcmp(cTypeName,"Domain")) {
      sprintf(gPId, "%ld", getpid());
      status = srbRegisterUserGrp(g_conn[cConn], MDAS_CATALOG, 
				  (char *) cNewValue,
				  gPId, "group","","","");
      if (status != 0) {
	fprintf(stderr, 
		"ingestTokenJ Register Error:Error in Making User Group for Domain:%i\n"
		,status);
	goto done;
      }
    }

 done:
    (*env)->ReleaseStringUTFChars( env, jTypeName, cTypeName );
    (*env)->ReleaseStringUTFChars( env, jNewValue, cNewValue );
    (*env)->ReleaseStringUTFChars( env, jParentValue, cParentValue );

    return ((jint) status);
}


JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_ingestResourceJ
(JNIEnv * env, jobject obj,
jint jConn, jint jCatType,
jstring jRescName, jstring jRescType, jstring jLocation, jstring jPhyPath,
jstring jClass, jint jSize)
{
    int status;
    int cConn;
    int cCatType;
    const char * cRescName;
    const char * cRescType;
    const char * cLocation;
    const char * cPhyPath;
    const char * cClass;
    int cSize;
    char *admUserName, *admDomain, *admUserAuth; /* Admin User ID */
    char clErrorMsg[MAX_TOKEN];
    char pubName[MAX_TOKEN];

    cConn = jConn;
    cCatType = jCatType;
    cRescName = (*env)->GetStringUTFChars( env, jRescName, 0 );
    cRescType = (*env)->GetStringUTFChars( env, jRescType, 0 );
    cLocation = (*env)->GetStringUTFChars( env, jLocation, 0 );
    cPhyPath = (*env)->GetStringUTFChars( env, jPhyPath, 0 );
    cClass = (*env)->GetStringUTFChars( env, jClass, 0 );
    cSize = jSize;

    status = srbRegisterResource(g_conn[cConn],
     (char *) cRescName, (char *) cRescType, 
     (char *) cLocation, (char *) cPhyPath, (char *) cClass, cSize);

    if (status != 0) {
        fprintf(stderr, 
           "ingestResourceJ srbRegisterResource Error: %i\n",status);
	goto done;
    }
    strcpy(pubName,"public@npaci");

    status = srbModifyRescInfo(g_conn[cConn], MDAS_CATALOG, cRescName, 
        R_INSERT_ACCS,  pubName, "write", "", "");
    if (status != 0) {
        fprintf(stderr, 
           "ingestResourceJ srbModifyRescInfo Error: %i\n",status);
	goto done;
    }

 done:
    (*env)->ReleaseStringUTFChars( env, jRescName, cRescName );
    (*env)->ReleaseStringUTFChars( env, jRescType, cRescType );
    (*env)->ReleaseStringUTFChars( env, jLocation, cLocation );
    (*env)->ReleaseStringUTFChars( env, jPhyPath, cPhyPath );
    (*env)->ReleaseStringUTFChars( env, jClass, cClass );

    return ((jint) status);
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_ingestLogRescJ
(JNIEnv * env, jobject obj,
jint jConn, jint jCatType,
jstring jLogRescName, jstring jPhyResc, jstring jPhyPath)
{
    int status;
    int cConn;
    int cCatType;
    const char * cLogRescName;
    const char * cPhyResc;
    const char * cPhyPath;
    char *admUserName, *admDomain, *admUserAuth; /* Admin User ID */
    char clErrorMsg[MAX_TOKEN];

    cConn = jConn;
    cCatType = jCatType;
    cLogRescName = (*env)->GetStringUTFChars( env, jLogRescName, 0 );
    cPhyResc = (*env)->GetStringUTFChars( env, jPhyResc, 0 );
    cPhyPath = (*env)->GetStringUTFChars( env, jPhyPath, 0 );

    status = srbRegisterLogicalResource(g_conn[cConn], (char *) cLogRescName, 
     (char *) DEF_LOG_RESC_TYPE, (char *) cPhyResc, 
     (char *) cPhyPath);

    if (status != 0) {
        fprintf(stderr, 
           "ingestLogRescJ srbRegisterLogicalResource Error: %i\n",status);
    }

    (*env)->ReleaseStringUTFChars( env, jLogRescName, cLogRescName );
    (*env)->ReleaseStringUTFChars( env, jPhyResc, cPhyResc );
    (*env)->ReleaseStringUTFChars( env, jPhyPath, cPhyPath );

    return ((jint) status);
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_addLogRescJ
(JNIEnv * env, jobject obj,
jint jConn, jint jCatType,
jstring jLogResc, jstring jPhyResc, jstring jPhyPath)
{
    int status;
    int cConn;
    int cCatType;
    const char * cLogResc;
    const char * cPhyResc;
    const char * cPhyPath;
    char *admUserName, *admDomain, *admUserAuth; /* Admin User ID */
    char clErrorMsg[MAX_TOKEN];

    cConn = jConn;
    cCatType = jCatType;
    cLogResc = (*env)->GetStringUTFChars( env, jLogResc, 0 );
    cPhyResc = (*env)->GetStringUTFChars( env, jPhyResc, 0 );
    cPhyPath = (*env)->GetStringUTFChars( env, jPhyPath, 0 );

    status = srbRegisterReplicateResourceInfo(g_conn[cConn], 
     (char *) cPhyResc, 
     (char *) DEF_LOG_RESC_TYPE, (char *) cLogResc, 
     (char *) cPhyPath);

    if (status != 0) {
        fprintf(stderr, 
          "addLogRescJ srbRegisterReplicateResourceInfo Error: %i\n",status);
    }


    (*env)->ReleaseStringUTFChars( env, jLogResc, cLogResc );
    (*env)->ReleaseStringUTFChars( env, jPhyResc, cPhyResc );
    (*env)->ReleaseStringUTFChars( env, jPhyPath, cPhyPath );

    return ((jint) status);
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_addCompRescJ
(JNIEnv * env, jobject obj,
jint jConn, jint jCatType,
jstring jCompResc, jstring jPhyResc, jstring jPhyPath)
{
    int status;
    int cConn;
    int cCatType;
    const char * cCompResc;
    const char * cPhyResc;
    const char * cPhyPath;
    char *admUserName, *admDomain, *admUserAuth; /* Admin User ID */
    char clErrorMsg[MAX_TOKEN];

    cConn = jConn;
    cCatType = jCatType;
    cCompResc = (*env)->GetStringUTFChars( env, jCompResc, 0 );
    cPhyResc = (*env)->GetStringUTFChars( env, jPhyResc, 0 );
    cPhyPath = (*env)->GetStringUTFChars( env, jPhyPath, 0 );

    status = srbRegisterReplicateResourceInfo(g_conn[cConn], 
     (char *) cPhyResc, 
     (char *) DEF_COMP_RESC_TYPE, (char *) cCompResc, 
     (char *) cPhyPath);

    if (status != 0) {
        fprintf(stderr, 
          "addCompRescJ srbRegisterReplicateResourceInfo Error: %i\n",status);
    }


    (*env)->ReleaseStringUTFChars( env, jCompResc, cCompResc );
    (*env)->ReleaseStringUTFChars( env, jPhyResc, cPhyResc );
    (*env)->ReleaseStringUTFChars( env, jPhyPath, cPhyPath );

    return ((jint) status);
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_modifyUserJ
(JNIEnv * env, jobject obj,
jint jConn, jint jCatType,
jint jModOpr, jstring jUserName, jstring jDomain, jstring jModValue)
{
    int status;
    int cConn;
    int cCatType;
    int cModOpr;
    const char * cUserName;
    const char * cDomain;
    const char * cModValue;
    char clErrorMsg[MAX_TOKEN];
    char dataValue1[MAX_TOKEN];

    cConn = jConn;
    cCatType = jCatType;
    cModOpr = jModOpr;
    cUserName = (*env)->GetStringUTFChars( env, jUserName, 0 );
    cDomain = (*env)->GetStringUTFChars( env, jDomain, 0 );
    cModValue = (*env)->GetStringUTFChars( env, jModValue, 0 );

    sprintf (dataValue1, "%s@%s", (char *) cUserName, (char *) cDomain);
    status = srbModifyUser(g_conn[cConn], cCatType, dataValue1,
     (char *) cModValue, cModOpr);

    if (status != 0) {
        fprintf(stderr, "addUserNameJ srbModifyUser Error: %i\n",status);
        goto done;
    }

 done:

    (*env)->ReleaseStringUTFChars( env, jUserName, cUserName );
    (*env)->ReleaseStringUTFChars( env, jDomain, cDomain );
    (*env)->ReleaseStringUTFChars( env, jModValue, cModValue );

    return ((jint) status);
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_modifyZoneJ
(JNIEnv * env, jobject obj,
jint jConn, jint jCatType,
jstring zoneName, jstring dataValue1, jstring dataValue2, jstring dataValue3,
jstring dataValue4, jstring dataValue5, jint actionType)
{
    int status;
    int cConn;
    int cCatType;
    int cActionType;
    const char * cZoneName;
    const char * cDataValue1;
    const char * cDataValue2;
    const char * cDataValue3;
    const char * cDataValue4;
    const char * cDataValue5;
    char clErrorMsg[MAX_TOKEN];

    cConn = jConn;
    cCatType = jCatType;
    cZoneName = (*env)->GetStringUTFChars( env, zoneName, 0 );
    cDataValue1 = (*env)->GetStringUTFChars( env, dataValue1, 0 );
    cDataValue2 = (*env)->GetStringUTFChars( env, dataValue2, 0 );
    cDataValue3 = (*env)->GetStringUTFChars( env, dataValue3, 0 );
    cDataValue4 = (*env)->GetStringUTFChars( env, dataValue4, 0 );
    cDataValue5 = (*env)->GetStringUTFChars( env, dataValue5, 0 );
    cActionType = actionType;
    
    status = srbModifyZone(g_conn[cConn], cCatType, (char *)cZoneName, (char *)cDataValue1,
			   (char *)cDataValue2, (char *)cDataValue3, (char *)cDataValue4, (char *)cDataValue5,
			   cActionType);

    if (status != 0) {
        fprintf(stderr, "modifyZoneJ srbModifyZone Error: %i\n",status);
        goto done;
    }

 done:

    (*env)->ReleaseStringUTFChars( env, zoneName, cZoneName );
    (*env)->ReleaseStringUTFChars( env, dataValue1, cDataValue1 );
    (*env)->ReleaseStringUTFChars( env, dataValue2, cDataValue2 );
    (*env)->ReleaseStringUTFChars( env, dataValue3, cDataValue3 );
    (*env)->ReleaseStringUTFChars( env, dataValue4, cDataValue4 );
    (*env)->ReleaseStringUTFChars( env, dataValue5, cDataValue5 );

    return ((jint) status);
}

/************************************************************************/
/*                                                                      */
/*  Given an SRB error code ID, this routine returns a string           */
/*  describing the error.                                               */
/*                                                                      */
/************************************************************************/
JNIEXPORT jstring JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_getSrbErrorString
(  JNIEnv * env, jobject obj, jint jErrorCode )
{
	int errorCode = jErrorCode;
	int i;
	static char msg[SRB_ERR_NAMELEN + SRB_ERR_SHORTLEN +
				SRB_ERR_LONGLEN + SRB_ERR_ACTLEN + 50];
	for ( i = 0; i < Srb_numents; i++ )  {
	   if ( srb_errent[i].srb_err == errorCode )
	      break;
	}

	if ( i == Srb_numents ) {  	    /* Invalid error id */
  	   sprintf (msg, 
		    " Error %s does not correspond to any known SRB error.", 
		    errorCode);
	   goto done;
	}
	
	/* compose the message */
	sprintf (msg, " %s: %s %s %s", 
		 srb_errent[i].srb_err_name, 
		 srb_errent[i].srb_err_short,
		 srb_errent[i].srb_err_long,
		 srb_errent[i].srb_err_action);

 done:
	return (*env)->NewStringUTF( env, msg );
}

JNIEXPORT jint JNICALL Java_edu_sdsc_SrbGlue_SrbJavaGlue_deleteValueJ
(JNIEnv * env, jobject obj,
jint jConn, jint jValueType, jstring jDeleteValue)
{
    int status;
    int cConn;
    int cValueType;

    char clErrorMsg[MAX_TOKEN];
    const char *cDeleteValue;

    cConn = jConn;
    cValueType = jValueType;
    cDeleteValue = (*env)->GetStringUTFChars( env, jDeleteValue, 0 );

    status = srbDeleteValue(g_conn[cConn], cValueType, cDeleteValue);

    if (status != 0) {
        fprintf(stderr, "deleteValueJ srbDeleteValue Error: %i\n",status);
        goto done;
    }

 done:

    (*env)->ReleaseStringUTFChars( env, jDeleteValue, cDeleteValue );

    return ((jint) status);
}

#endif /* WIN32 */

