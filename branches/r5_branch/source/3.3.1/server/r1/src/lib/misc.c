/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/*
 * misc.c - Misc routines for the srbServer and srbMaster.
 */

#include "srbMisc.h"
#ifdef _WIN32
#include "srb.h"
#include "SrbNtUtil.h"
#endif


/*
 * getUserId - Given the userName, return the uid (pw_uid) 
 *
 * Input - char *userName
 *
 * Output - the uid
 */

int
getUserId(char *userName)
{
#ifdef _WIN32
    return 0;
#else
    struct passwd *pw;
    int userId;

    if (!(pw = getpwnam(userName))) {
	if (DebugLvl > 1)
	    elog(NOTICE, "getUserId: %s not in passwd file", userName);
	userId = DEFAULT_USER;
    } else {
    	userId = (int) pw->pw_uid;
    }
    return (userId);
#endif
}

/* In NT, we get the data path from registry not like in UNIX, it
   is determined in build. */
#ifdef _SRB_WIN_SERVICE
char *win32SrbDatadir=NULL;
char *win32SrbExePath=NULL;
/* Here we deal with path using backslash. */
int ntSetSrbDataDir(char *inExecNamePath)
{
    char tmpdir[1024];
    int LocalStatus;
    char *p;

    if((inExecNamePath!=NULL)&&(strlen(inExecNamePath)>0))
    {
        strcpy(tmpdir,inExecNamePath);
        p = strrchr(tmpdir,'\\');
        if(p == NULL)
        {
            return -1;
        }

        p[0] = '\0';

        win32SrbExePath = strdup(tmpdir);

        /* sprintf(tmpdir,"%s\\..\\data",tmpdir); */
        p = strrchr(tmpdir,'\\');
        if(p == NULL)
        {
            return -1;
        }

        p[0] = '\0';
        strcat(tmpdir,"\\data");
        win32SrbDatadir = strdup(tmpdir);

        return 0;
    }

    /* now it is a service, we get path from registery. */
    tmpdir[0] = '\0';
    LocalStatus = SrbNtGetServerExecPathFromRegistry(tmpdir);

    if(LocalStatus < 0)
    {
        if(SrbNtRunInConsoleMode())
          fprintf(stderr,"Failed to get SRB service info from Windows registry.\n");
        else
          NtEmergencyMessage("\nFailed to get SRB service info from Windows registry.");
        return LocalStatus;
    }

    win32SrbExePath = strdup(tmpdir);

    p = strrchr(tmpdir,'\\');
    if(p == NULL)
        return -1;

    p[0] = '\0';
    /* sprintf(tmpdir,"%s\\..\\data",tmpdir); */
    strcat(tmpdir,"\\data");
    win32SrbDatadir = strdup(tmpdir);

    return 0;
}

/* A Windows service tries to get its default port from a file first. 
   iOtherwise uses the hard coded one. */
void ntSetSrbDefaultPort()
{
   char *srb_data_dir;
   FILE *file;
   char ln[1024];
   char tmpstr[100];
   char filename[2048];
   int d;

   if(DefaultPort != NULL)
     return;

   srb_data_dir = getSrbDataDir();

   if(srb_data_dir == NULL)
   {
      DefaultPort = strdup(ntDefaultPort);
      return;
   }

   sprintf(filename,"%s\\srbPort.txt",srb_data_dir);

   file = fp_fileopen(filename, "r");

   if(file == NULL)
   {
      DefaultPort = strdup(ntDefaultPort);
      return;
   }

   if(fgets(ln,1024,file) != NULL)
   {
      d = atoi(ln);
 
      if(d > 0)
      {
         sprintf(tmpstr,"%d",d);
         DefaultPort = strdup(tmpstr);
      }
   }

   if(DefaultPort == NULL)
   {
      DefaultPort = strdup(ntDefaultPort);
   }

   fclose(file);
}
#endif


/* getSrbDataDir - get the "data" directory (where configuration files are
 * kept). It will use the path defined in the srbData env variable first.
 * If it is not defined, use the constant string SRB_DATADIR which is
 * defined in the Makefile.
 *
 * Input - None
 *
 * Output - The "data" directory path
 */ 

char *
getSrbDataDir()
{
#ifdef _SRB_WIN_SERVICE
    return win32SrbDatadir;
#else
    char *p;
 
    if ((p = (char *) getenv("srbData")) != (char *) NULL) {
        return (p);
    }
    return (SRB_DATADIR);	/* SRB_DATADIR defined in the Makefile */
#endif
}

#ifdef FED_MCAT
int
isLocalPrivUser()
{
  if (ClientUser == NULL)
    return (AUTH_ERR_PROXY_NOPRIV);
  if (ClientUser->privUserFlag == LOCAL_PRIV_USER )
    return(MDAS_SUCCESS);
  else
    return(MDAS_FAILURE);
}
#endif

#ifdef _WIN32
/* Call ntSetSrbDataDir() first in main(), here we need to deal back slash.*/
int SrbNtFindServerExec(char *pFileNameWP)
{
    char tmpstr[1024];
    sprintf(tmpstr,"%s\\srbServer.exe",win32SrbExePath);
    strcpy(pFileNameWP,tmpstr);
    return 0;
}
#else   /* UNIX case use findServerExec. */
/*
 * findServerExec -- find the absolute path to a valid srbServer executable
 *
 */
int
findServerExec(char *backend, char *argv0)
{
    char        buf[MAXEXFPATH + 2];
    char        *p;
    char        *path, *startp, *endp;
    int         pathlen;
 
    /*
     * for the srbMaster:
     * First try: use the backend that's located in the same directory
     * as the srbMaster, if it was invoked with an explicit path.
     */
    if (argv0 && (p = strrchr(argv0, '/')) && *++p) {
        if (*argv0 == '/' || !getcwd(buf, MAXEXFPATH))
            buf[0] = '\0';
        else
            (void) strcat(buf, "/");
        (void) strcat(buf, argv0);
        p = strrchr(buf, '/');
        (void) strcpy(++p, "srbServer");
        if (!validateExec(buf)) {
            (void) strncpy(backend, buf, MAXEXFPATH);
            if (DebugLvl)
                fprintf(stderr, "findServerExec: found \"%s\" using argv[0]\n",
                        backend);
            return(0);
        }
        fprintf(stderr, "findServerExec: invalid backend \"%s\"\n",
                buf);
        return(-1);
    }
 
    /* Try the cwd */

    if (getcwd(buf, MAXEXFPATH) != NULL) {
	(void) strcat(buf, "/srbServer");
        if (!validateExec(buf)) {
            (void) strncpy(backend, buf, MAXEXFPATH);
            if (DebugLvl)
                fprintf(stderr, "findServerExec: found \"%s\" using argv[0]\n",
                        backend);
            return(0);
        }
    }

    /*
     * Since no explicit path was supplied. Use the the paths define by PATH.
     */
    if ((p = getenv("PATH")) && *p) {
        if (DebugLvl)
            fprintf(stderr, "findServerExec: searching PATH ...\n");
        pathlen = strlen(p);
        path = malloc(pathlen + 1);
        (void) strcpy(path, p);
        for (startp = path, endp = strchr(path, ':');
             startp && *startp;
             startp = endp + 1, endp = strchr(startp, ':')) {
            if (startp == endp) /* it's a "::" */
                continue;
            if (endp)
                *endp = '\0';
            if (*startp == '/' || !getcwd(buf, MAXEXFPATH))
                buf[0] = '\0';
            (void) strcat(buf, startp);
            (void) strcat(buf, "/srbServer");
            switch (validateExec(buf)) {
            case 0:             /* found ok */
                (void) strncpy(backend, buf, MAXEXFPATH);
                if (DebugLvl)
                    fprintf(stderr, "findServerExec: found \"%s\" using PATH\n",
                            backend);
                free(path);
                return(0);
            case -1:          
                break;
            case -2:         
                fprintf(stderr, 
		    "findServerExec: could not read backend \"%s\"\n", buf);
                free(path);
                return(-1);
            }
            if (!endp)          /* last one */
                break;
        }
        free(path);
    }
 
    fprintf(stderr, "findServerExec: could not find a backend to execute...\n");
    return(-1);
}
#endif

/*
 * validateExec -- validate "path" as a srbServer executable file
 *
 * Input - char *path - The path to validate.
 *
 * Output -
 * returns 0 if the file is found and no error is encountered.
 *        -1 if the regular file "path" does not exist or cannot be executed.
 *        -2 if the file is otherwise valid but cannot be read.
 */

int
validateExec(char *path)
{
    struct stat         buf;
#ifndef _WIN32
    uid_t               euid;
    struct group        *gp;
    struct passwd       *pwp;
#endif
    int                 i;
    int                 is_r = 0;
    int                 is_x = 0;
    int                 in_grp = 0;

    if (strlen(path) >= MAXEXFPATH) {
        if (DebugLvl > 1)
            fprintf(stderr, "validateExec: pathname \"%s\" is too long\n",
                    path);
        return(-1);
    }
 
    if (stat(path, &buf) < 0) {
        if (DebugLvl > 1)
            fprintf(stderr, "validateExec: can't stat \"%s\"\n",
                    path);
        return(-1);
    }
    if (!(buf.st_mode & S_IFREG)) {
        if (DebugLvl > 1)
            fprintf(stderr, "validateExec: \"%s\" is not a regular file\n",
                    path);
        return(-1);
    }
 
    /*
     * Ensure that the file is both executable and readable
     */
#ifdef _WIN32
    is_r = buf.st_mode & S_IREAD;
    is_x = buf.st_mode & S_IEXEC;
    if (DebugLvl > 1 && !(is_r && is_x))
        fprintf(stderr, "validateExec: \"%s\" is not other read/execute\n",
                path);
    return(is_x ? (is_r ? 0 : -2) : -1);

#else
    euid = geteuid();
    if (euid == buf.st_uid) {
        is_r = buf.st_mode & S_IRUSR;
        is_x = buf.st_mode & S_IXUSR;
        if (DebugLvl > 1 && !(is_r && is_x))
            fprintf(stderr,"validateExec: \"%s\" is not user read/execute\n",
                    path);
        return(is_x ? (is_r ? 0 : -2) : -1);
    }
    pwp = getpwuid(euid);
    if (pwp) {
        if (pwp->pw_gid == buf.st_gid) {
            ++in_grp;
        } else if (pwp->pw_name &&
                   (gp = getgrgid(buf.st_gid))) {
            for (i = 0; gp->gr_mem[i]; ++i) {
                if (!strcmp(gp->gr_mem[i], pwp->pw_name)) {
                    ++in_grp;
                    break;
                }
            }
        }
        if (in_grp) {
            is_r = buf.st_mode & S_IRGRP;
            is_x = buf.st_mode & S_IXGRP;
            if (DebugLvl > 1 && !(is_r && is_x))
                fprintf(stderr, 
		"validateExec: \"%s\" is not group read/execute\n",
                        path);
            return(is_x ? (is_r ? 0 : -2) : -1);
        }
    }
    is_r = buf.st_mode & S_IROTH;
    is_x = buf.st_mode & S_IXOTH;
    if (DebugLvl > 1 && !(is_r && is_x))
        fprintf(stderr, "validateExec: \"%s\" is not other read/execute\n",
                path);
    return(is_x ? (is_r ? 0 : -2) : -1);
#endif
}

#ifdef XXXXXX
/* exitSrb - A common place to exit
 *
 * Input : None
 *
 * Output : None
 */

void
exitSrb(int code)
{
    closeAllDesc ();		/* Close all opened obj descriptor */
#ifdef HPSS
    if (DebugLvl > 1)
        elog (NOTICE, "exitSrbServer: Will do a PurgeLoginContext");
    PurgeLoginContext();
#endif

#ifdef SRB_MDAS
    close_db2_interaction(MDAS_CAT_COMMIT);
#endif

/*
#if defined(PORTNAME_solaris) && defined(DB_Oracle)
    _exit (code);
#else 
    exit (code);
#endif
*/
    exit (code);
}
#endif	/* XXXXXX */

/*
 Split input string st1 into st2 and st3 breaking where key char exists
 *
 * Output : Returns the pointer to the beginning of the next string
 *	    Returns NULL on error.
 *
 */
char *
mySplitStr(char *st1, char *st2, char *st3, char key)
{
    int i;
    int inLen;

    inLen = strlen (st1);
    if (inLen <= 0) 
	return NULL;

    for (i = 0; (i < inLen && i < STRING_LEN1 && st1[i] != key) ; i++){
         st2[i] = st1[i];
    }

    st2[i] = '\0';

    if (st1[i] != key)
	return NULL;

    strcpy(st3, (char *) &st1[i+1]);
    
    return &st1[i+1];
}

/* path assumed to be full path name - split into collection and dataName.
 * 
 */
int
mySplitPath (char *path, char *collection, char *dataName)
{
    int i,j;
    int inLen;

    inLen = strlen (path);

    if (inLen <= 0)
        return OBJ_ERR_BAD_PATHNAME;

    if (path[inLen - 1] == '/') {	/* end with a '/' */
	strcpy (collection, path);
	if (inLen > 1)
	    collection[inLen - 1] = dataName[0] = '\0';
	return 0;
    }

    for (i = inLen - 1, j = 0; i >= 0; i--, j++) {
	if (path[i] == '/') 
	    break;
    }

    if (i == 0) {	/* root collection */
	strcpy (collection, path);
	dataName[0] = '\0';
	return (0);
    }

    if (i < 0) {
	collection[0] = '\0';
    } else {
    	strncpy (collection, path, i);
    	collection[i] = '\0';
    }
    strcpy (dataName, &path[i+1]);

    return 0;
}

/* get the length of the string terminated by '&', ' ' or '\0' */

int
getCondStrLen (char *st1)
{
    int inLen;

    inLen = 0;

    while (st1[inLen] != '&' && st1[inLen] != ' ' && st1[inLen] != '\0')
            inLen ++;
    return inLen;
}
int
checkString(char *st1, char key)
{
    int i;
    int inLen;

    inLen = strlen (st1);
    if (inLen <= 0)
        return 0;

    for (i = 0; (i < inLen && i < STRING_LEN1) ; i++){
	if (st1[i] == key)
	    return 0;
    }
    return -1;
}

int
trimsides(char *str)
{   
  
  char *tmpPtr2, *tmpPtr3;

  tmpPtr2 = str;
  while (isspace(*tmpPtr2)) tmpPtr2++;
  tmpPtr3 = str + strlen(str) -1;
  while (isspace(*tmpPtr3)) tmpPtr3--;
  *(++tmpPtr3) = '\0';
  memmove(str,tmpPtr2, (int) (tmpPtr3-tmpPtr2 + 1));
  return (0);
}

/* varToStr - Convert a string from (varlena) format to (char) format.
 *
 * Input : struct varlena *varBuf - The input string in varlena format.
 *
 * Output : Returns the output string in (char) format
 *                  NULL - Failure
 */

char *varToStr (struct varlena *varBuf)
{
int length;
char *strBuf;

    /* length = strlen(VARDATA(varBuf)) + 1;  */
    length = VARSIZE(varBuf) - VARHDRSZ + 1;
    if (length > 0) {
        strBuf = (char*) malloc (length);

        memcpy(strBuf, VARDATA(varBuf), length);
        strBuf[length-1] = '\0';
    } else {
        strBuf = NULL;
    }
    return (strBuf);
}

/* objTypeLookUp - Using the input resourceType, search the srbObjTypeEntry
 * table for a match. Returns the resourceType of the struct if a match is
 * found.
 *
 * Input : char *resourceType - The resource type
 *
 * Output : The srbObjTypeEntry[i].resourceType if a match is found
 *          OBJ_ERR_RES_TYPE - no match
 */

int
objTypeLookUp (char *resourceType)
{
    int i;

    if (resourceType == NULL)
        return OBJ_ERR_RES_TYPE;

    for (i = 0; i < nSrbObjType; i++) {
        if (strstr (resourceType, srbObjTypeEntry[i].resourceType) != NULL)
            return i;
    }
    /* elog(NOTICE, "objTypeLookUp: resourceType %s not supported. status = %d",
      resourceType, OBJ_ERR_RES_TYPE); */
    return OBJ_ERR_RES_TYPE;
}

/*
 * enqueVault - Enque a Vault entry to the end of the Vault queue.
 *
 * Input : int storSysType - Storage system type.
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP.
 *         char *vaultPath - The SRB vault path
 *
 * Output : None
 */

int enqueVault (int storSysType, char *vaultPath)
{
    struct vaultQueElement *tmpElement, *tmpElement1;

    tmpElement = (struct vaultQueElement *) malloc
        (sizeof (struct vaultQueElement));
    if (tmpElement == NULL) {
        printf ("enqueVault: Unable of malloc tmpElement\n");
	return  (UNIX_ENOMEM);
    }

    tmpElement->storSysType = storSysType;
    tmpElement->vaultPath = vaultPath;
    tmpElement->next = NULL;

    if (vaultQueHead == NULL) {
        vaultQueHead = tmpElement;
        return 0;
    }

    /* append tmpElement to the end */

    tmpElement1 = vaultQueHead;
    while (tmpElement1->next != NULL) {
        tmpElement1 = tmpElement1->next;
    }
    tmpElement1->next = tmpElement;

    return 0;
}

#ifdef _WIN32
static int _srbNtPathExist(char *path)
{
    char newpath[1024];
    struct stat statbuf;

    SrbPathToNtPath(newpath,path);

    if(stat(newpath,&statbuf) == 0)
        return 0;
    else
        return -1;
}
#endif

/* pathExist - Check the existence of the path.
 *
 * Input - None
 * Output - 0 = exists, -1 = does not exist.
 *
 */

int
pathExist(char *path)
{
#ifdef _WIN32
    return _srbNtPathExist(path);
#else
    struct stat statbuf;

    if (stat (path, &statbuf) == 0) {
        return (0);
    } else {
        return (-1);
    }
#endif
}



int
lower (char *str)
{
  int i;
  for (i = 0 ; i < strlen(str) ; i++)
    str[i] = tolower(str[i]);
  return (0);
}

int
upper (char *str)
{
  int i;
  for (i = 0 ; i < strlen(str) ; i++)
    str[i] = toupper(str[i]);
  return (0);
}

int
chkSrbUser (char *namebuf)
{
    int i;
    int matchFlag = 0;
    char *ptr, *zoneName;

    if (SrbUserList == NULL) {
        elog (NOTICE, 
	 "chkSrbUser:Priv Userlist is empty.Could be error in hostConfig file");
        return 0;
    }

    ptr = SrbUserList->sqlresult[0].values;

#ifdef FED_MCAT
    zoneName = (char *)getFromResultStruct(
                (mdasC_sql_result_struct *) SrbUserList,
                  dcs_tname[ZONE_NAME],
                  dcs_aname[ZONE_NAME]);
#endif

    for (i = 0; i < SrbUserList->row_count; i ++) {

        if (strcmp (ptr, namebuf) == 0) {
#ifdef FED_MCAT
	    if (LocalMcat != NULL && LocalMcat->serverAuthInfo != NULL &&
	     LocalMcat->serverAuthInfo->mcatName != NULL &&
	     strcmp (zoneName, LocalMcat->serverAuthInfo->mcatName) != 0) {
		matchFlag = FOREIGN_PRIV_USER;
	    } else {
		matchFlag = LOCAL_PRIV_USER;
	    }
#else
            matchFlag = 1;
#endif
            break;
        }
        ptr += MAX_DATA_SIZE;
#ifdef FED_MCAT
        zoneName += MAX_DATA_SIZE;
#endif
    }

    return (matchFlag);
}

int initSrbUserList ()
{
    int status;
    mdasC_sql_result_struct *tmpList;

    if (LocalHostHead == NULL)
        return (SYS_ERR_SRV_STARTUP);

    if (LocalHostHead->mdasEnabled == 1) {
#ifdef SRB_MDAS
        status = getSRBUserInfo (MDAS_CATALOG, &tmpList, DEF_NUM_ROWS);
#else
        status = SYS_ERR_MCAT_SERVER_DOWN;

#endif
    } else {
	struct hostElement  *hostTabPtr;

	status = getMdasEnabledHost (NULL, &hostTabPtr);
        status = remoteGetPrivUsers (MDAS_CATALOG, &tmpList, DEF_NUM_ROWS, 
	  hostTabPtr);
    }

    if (status >= 0 && tmpList != NULL) {
        freeSqlResult (SrbUserList);
        SrbUserList = tmpList;
    }

    return status;
}

int
initThisUserInfo ()
{
    char clErrorMsg[ERROR_MSG_LENGTH];
    struct authsvc *myAuthsvc;
    char *tmpAuthKeyWd = NULL;


    if (ThisMdasEnvData == NULL)
	ThisMdasEnvData = readMdasParam ();   /* read the .MdasEnv File */


    if (ThisMdasEnvData == NULL) {
	fprintf (stderr, "initThisUserInfo: readMdasParam() error\n");
        return (CLI_ENV_FILE_ERR);
    } else if (ThisMdasEnvData->retVal < 0) {
        free (ThisMdasEnvData);
	ThisMdasEnvData = NULL;
        return (ThisMdasEnvData->retVal);
    }

    ThisUser = (userInfo *) malloc (sizeof (userInfo));
    memset (ThisUser, 0, sizeof (userInfo));

    ThisUser->userName = clGetUserName(ThisMdasEnvData, clErrorMsg);
    
    if (ThisUser->userName == NULL) {
        free (ThisUser);
        ThisUser = NULL;
        return (CLI_ERR_INVAILD_USER);
    }

    ThisUser->domainName = clGetDomainName (ThisMdasEnvData, clErrorMsg);

    if (ThisUser->domainName == NULL) {
        free (ThisUser);
        ThisUser = NULL;
        return (CLI_ERR_INVAILD_USER);
    }

    ThisUser->privUserFlag = LOCAL_PRIV_USER;

#ifdef FED_MCAT
    ThisUser->mcatZone = clGetMcatZone (ThisMdasEnvData, clErrorMsg);
#endif

    ThisUser->userAuth = clGetAuth (clErrorMsg);


    /* only take the env variable. Otherwise mark it unset */

    if ((tmpAuthKeyWd = getenv (AUTH_KEYWD)) != NULL) {
 	ThisUser->authSchemeInx = matchAuthByKeyWd (tmpAuthKeyWd, &myAuthsvc);
    } else {
	ThisUser->authSchemeInx = -1;
    }
#ifdef foo
    } else if (ThisMdasEnvData->authScheme != NULL) {
        ThisUser->authSchemeInx =
        matchAuthByKeyWd (ThisMdasEnvData->authScheme, &myAuthsvc);
    }

    if (ThisUser->authSchemeInx < 0) {
	tmpAuthKeyWd = DEFAULT_CLIENT_AUTHSVC;
        ThisUser->authSchemeInx = matchAuthByKeyWd (tmpAuthKeyWd, &myAuthsvc);
    }
#endif
    /* setup a temp ClientUser if it does not exist. Should be overwritten
     * in procStartupMsg.
     */

    if (ClientUser == NULL) {
	ClientUser = (userInfo *) malloc (sizeof (userInfo));
	dupUserInfo (ClientUser, ThisUser);
    }
    return (0);
}

#ifdef foo	/* deplicated */
int initProxyUserInfo (char* errorMsg, int authSchemeInx)
{
    int status;

    if (ProxyUser == NULL || ClientUser == NULL) {
        ProxyUser = (userInfo *) malloc (sizeof (userInfo));
        ClientUser = (userInfo *) malloc (sizeof (userInfo));
	memset (ProxyUser, 0, sizeof (userInfo));
	memset (ClientUser, 0, sizeof (userInfo));
    } else {
        /* already done it at least once */
        if (ProxyUser->userName != NULL)
            free (ProxyUser->userName);
        if (ProxyUser->domainName != NULL)
            free (ProxyUser->domainName);
        if (ProxyUser->userAuth != NULL)
            free (ProxyUser->userAuth);
	if (ProxyUser->mcatZone != NULL)
            free (ProxyUser->mcatZone);
    }
       
    /* Fill in the userInfo. */

    ProxyUser->userName = clGetUserName(NULL, errorMsg);

    if (ProxyUser->userName == NULL) {
        return (CLI_ERR_INVAILD_USER);
    }

    ProxyUser->userId = getUserId (ProxyUser->userName);

    ProxyUser->domainName = clGetDomainName (NULL, errorMsg);

    if (ProxyUser->domainName == NULL) {
        return (CLI_ERR_INVAILD_DOMAIN);
    }

    ProxyUser->userAuth = clGetAuth (errorMsg);
#ifdef FED_MCAT
    ProxyUser->mcatZone = clGetMcatZone (NULL, errorMsg);
    if (ProxyUser->mcatZone == NULL && LocalMcat != NULL)
	ProxyUser->mcatZone = strdup(LocalMcat->serverAuthInfo->mcatName);

    ClientUser->mcatZone = getenv (CLIENT_MCATZONE_KEYWORD);
#endif

    ProxyUser->privUserFlag = atoi (getenv ("proxyPrivFlag"));

    ProxyUser->authSchemeInx = ClientUser->authSchemeInx = authSchemeInx;

    ClientUser->userName = getenv (CLIENT_USER_KEYWORD);


    ClientUser->privUserFlag = atoi (getenv (CLIENT_PRIV_KEYWORD));

    if (strcmp (ClientUser->userName, TICKET_USER) == 0) {
        TicketUserFlag = 1;     /* A ticket user */
    } else {
        TicketUserFlag = 0;     /* Not a ticket user */
    }

    ClientUser->userId = getUserId (ClientUser->userName);

    ClientUser->domainName = getenv (CLIENT_DOMAIN_KEYWORD);

    ClientUser->userAuth = getenv (CLIENT_AUTH_KEYWORD);

    return (0);
}
#endif	/* foo */

/* srbDaemonize - Daemonize the SRB.
 * Create a new session and disassociate from the controlling TTY.
 * Pipe all output to SRB_LOGFILE.
 *
 * Input - None
 * Output - None
 */

void
srbDaemonize(char *logfile)
{
    int i;
    char *logFile;

#ifndef _WIN32
    if (fork())
/*
#if defined(PORTNAME_solaris) && defined(DB_Oracle)
        _exit (0);
#else
        exit (0);
#endif
*/
        exit (0);

    if (setsid() < 0) {
        fprintf(stderr, "srbDaemonize");
        perror("cannot create a new session.");
        exit(1);
    }
#endif


    /* Put together the full pathname of the logFile */

    logFile = (char *) malloc((strlen(getSrbDataDir()) +
        strlen(logfile)+2)*sizeof(char));

    strcpy(logFile, getSrbDataDir());
    strcat(logFile, "/");
    strcat(logFile, logfile);

#ifdef _WIN32
    if ((i = SrbNtFileOpen(logFile, _O_CREAT|_O_RDWR|_O_TRUNC,1)) < 0)
        elog(WARN,"Unable to open logFile %s\n", logFile);
    else
        elog(NOTICE,"logFile: %s opened successfully.\n", logFile);
#else
    if ((i = open(logFile, O_CREAT|O_RDWR|O_TRUNC, 0644)) < 0) {
        fprintf(stderr, "Unable to open logFile %s\n", logFile);
        i = open(NULL_DEV, O_RDWR);
    } else {
        fprintf(stderr, "logFile: %s opened successfully.\n", logFile);
    }
#endif
    free (logFile);

#ifndef _WIN32
    close (0);
    close (1);
    close (2);
    (void) dup2(i, 0);
    (void) dup2(i, 1);
    (void) dup2(i, 2);
#endif
    (void) close(i);
}

struct varlena *
genIntRet (int status)
{
    struct varlena * retVal;

    retVal = (struct varlena *)malloc(VAROUTHDRSZ);
    VARSIZE(retVal) = VAROUTHDRSZ;
    VAROUTSTAT(retVal) = htonl (status);

    return retVal;
}



int
make_quote_to_two_quote(char *new_obj_path_name,char *my_new_obj_path_name)
{
  int i,j,k;
  char *andPtr;
  if (new_obj_path_name == NULL)
    {
      my_new_obj_path_name[0]  = '\0';
      return(0);
    }
  if ((andPtr = strstr(new_obj_path_name,"&")) != NULL) {
    *andPtr = '\0';
  }
  i = strlen(new_obj_path_name);
  for (j = 0, k = 0; j < i; j++,k++)
    {
      if (new_obj_path_name[j] == '\'')
	my_new_obj_path_name[k++] = '\'';
      my_new_obj_path_name[k] = new_obj_path_name[j];
    }
  my_new_obj_path_name[k] = '\0';
  if (andPtr != NULL) {
    *andPtr = '&';
    strcat(my_new_obj_path_name, andPtr);
  }
  
  return(0);
}

#ifdef FED_MCAT
char *
getLocalMcatName ()
{
    if (LocalMcat == NULL)
        return NULL;

    return (LocalMcat->serverAuthInfo->mcatName);
}
#endif

#ifdef _WIN32
#ifdef _SRB_WIN_SERVICE
int NtGetPortNumFromArgs(int myargc, char **myargv)
{
   char tmpstr[1024];
   int i;

   for(i=0;i<myargc;i++)
   {
      if( strncmp(myargv[i],"srbntport=",10) == 0)
      {
            strcpy(tmpstr,&(myargv[i][10]));
            DefaultPort = strdup(tmpstr);
            return 0;
      }
   }

   return -1;
}

/* return 0 means success. return a number less than zero means that it failed to get info from regisery. */
int SrbNtGetServerExecPathFromRegistry(char *pExecPath)
{
    char tmpdir[1024];
    HKEY KeyHandle;
    DWORD ValueType,ValueLength=1024;
    int LocalStatus;
    char SrbServiceKey[024];
    char *p;
    char srb_service_name[1024];

    sprintf(srb_service_name,"%s_%s",SRB_SERVICE_NAME,DefaultPort);

    sprintf(SrbServiceKey,"SYSTEM\\CurrentControlSet\\Services\\%s",srb_service_name);

    /*LocalStatus = RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software\\San Diego Supercomputer Center\\SRBServer",0,KEY_QUELUE,&KeyHandle);
    */
    LocalStatus = RegOpenKeyEx(HKEY_LOCAL_MACHINE,SrbServiceKey,0,KEY_QUERY_VALUE,&KeyHandle);
    if(LocalStatus != ERROR_SUCCESS)
    {
        return -1;
    }

    /*LocalStatus = RegQueryValueEx(KeyHandle,"AppPath",NULL,&ValueType,(BYTE*)&tmpdir,&ValueLength); */
    LocalStatus = RegQueryValueEx(KeyHandle,"ImagePath",NULL,&ValueType,(BYTE*)&tmpdir,&ValueLength);
    if(LocalStatus != ERROR_SUCCESS)
    {
        RegCloseKey(KeyHandle);
        return -1;
    }

    tmpdir[ValueLength] = '\0';
    p = strrchr(tmpdir,'\\');

    if(p)
        p[0] = '\0';

    strcpy(pExecPath,tmpdir);

    RegCloseKey(KeyHandle);

    return 0;
}


#endif
#endif
