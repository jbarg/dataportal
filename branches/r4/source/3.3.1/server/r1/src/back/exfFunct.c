/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/* exfFunct.c - Server functions to handle File type SID - e.g., srbFileXXXX
 */

#include "exfFunct.h"
#ifdef _WIN32
#include "SrbNtUtil.h"
#include "misc_util.h"
static int NT_mkVaultDir (int storSysType, char *filePath, int pathType);
#endif

#ifdef PARA_OPR
pthread_mutex_t ExfLock;
#endif

/* chkdir - check whether a I/O operation should be done.
 * 
 * Input : int storSysType - Storage system type.
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP.
 *	   char *hostAddrBuf - The Host address of the file location
 *	   char *fileNameBuf - The file path name
 * 	   chkFlag - WRT_CHK, the calling user must own the directory 
 *	   	     it is writing to.
 *          	   - RD_CHK, the file must ed either owned by the calling 
 *		     user or the server user.
 *
 * Output : Returns 0 - OK to do the I/O
 *		   negative - Not OK to do the I/O
 */

int
chkdir (int storSysType, char *hostAddrBuf, char *fileNameBuf, 
int chkFlag, int pathType)
{
    char *dirname;
    int pathlen;
    struct stat statbuf;
    int uid, status;
    char *vaultPath, *userVaultDir;
#ifdef _WIN32
	int t;
	int InvalidFileName=0;
#endif
 
    if (storSysType < 0 || storSysType >= nExfswEntries) {
        elog (NOTICE, "The storSysType %d is not supported", storSysType);
        return INP_ERR_NO_SUPPORT;
    }

#ifdef _WIN32
	t = StrLen(fileNameBuf);
	if((t == 0) || (t == 1))
	{
		InvalidFileName = 1;
	}
	else if(t == 2)
	{
		if(fileNameBuf[1] != ':')
			InvalidFileName = 1;
	}
	else
	{
		if(fileNameBuf[0]=='/') 
		{
			if(fileNameBuf[2] != ':')
			{
				InvalidFileName = 1;
			}
		}
		else if(fileNameBuf[1] != ':')
		{
			InvalidFileName = 1;
		}

	}
	if(InvalidFileName)
	{
		elog (NOTICE, "filename must begin with a /\n");
		return INP_ERR_FILENAME;
	}
#else
    if (fileNameBuf[0] != '/') {
        elog (NOTICE, "filename must begin with a /\n");
        return INP_ERR_FILENAME;
    }
#endif

    /* Check if fileNameBuf is in the vaultPath */
#ifndef _WIN32
    vaultPath = matchVault (storSysType, fileNameBuf);

    if (vaultPath != NULL) {		/* In the vault */
        if (chkFlag == RD_CHK) {
            /* Trying to do a read in the vault. */
            return (0);
        } 

	if (chkFlag != NO_CHKDIR) {

	    /* we have to check the user vault path */

            userVaultDir = malloc (strlen (vaultPath) +
              strlen (ClientUser->userName) +
                strlen (ClientUser->domainName) + 3);

            strcpy (userVaultDir, vaultPath);
            strcat (userVaultDir, "/");
            strcat (userVaultDir, ClientUser->userName);
            strcat (userVaultDir, ".");
            strcat (userVaultDir, ClientUser->domainName);

	    if (strncmp (userVaultDir, fileNameBuf, 
	      strlen (userVaultDir)) != 0) {  /* client does not own the path */
	        elog(NOTICE, "chkdir: userVaultDir mismatch. %s", fileNameBuf);
	        free (userVaultDir);
	        return (UNIX_EACCES);
	    }
	    free (userVaultDir);
	}

	VaultpathStatus = PATH_IN_VAULT;

	if (chkFlag == CHK_FILE_PATH) {		/* check the file path only */
	    if ((status = exfstat(storSysType, hostAddrBuf, 
	      fileNameBuf, (char *) &statbuf)) < 0) 
            	elog(NOTICE, "chkdir: stat of path %s failed, status = %d",
                  fileNameBuf, status);
        } else {	/* make the needed path */
	    status = mkVaultDir (storSysType, hostAddrBuf, fileNameBuf, 
	     pathType);
	}

	return (status);
    }
#endif

    /* Got here because fileNameBuf is not in the vault */

    VaultpathStatus = PATH_NOT_IN_VAULT;

    if (storSysType == GRIDFTP_FT) {
	status = mkVaultDir (storSysType, hostAddrBuf, fileNameBuf, pathType);
	return (status);
    } else if (chkFlag == WRT_CHK) {
	/* check the file first */
        if ((status = exfstat(storSysType, hostAddrBuf, fileNameBuf,
          (char *) &statbuf)) < 0) {
	    if (DebugLvl > 1)
            	elog(NOTICE,
                  "chkdir: stat of non-vault file %s failed. status = %d",
                    fileNameBuf, status);
        } else {
            uid = (int) ClientUser->userId;
#ifdef _WIN32
            if ((statbuf.st_mode & _S_IWRITE) == 0) 
#else
            if (uid != statbuf.st_uid && ((statbuf.st_mode & S_IWOTH) == 0)) 
#endif
	        {
                elog(NOTICE,
                "chkdir: WRT_CHK failed.file %s not in Vault and not world writable, uid=%d",
                  fileNameBuf, uid);
                return SRB_EPERM;
            } else
            	return 0;
	}

	/* The file does not exist. Try the check the directory */
        pathlen = strlen (fileNameBuf);
        dirname = fileNameBuf + pathlen - 1;
	
	if (pathType == FILE_T) {	/* only go backward for file type */
            while (pathlen-- && *dirname != '/')
            	dirname --;
	}
 
        dirname = (char *) malloc (pathlen + 1);
        strncpy (dirname, fileNameBuf, pathlen);
        dirname[pathlen] = '\0';
 
        if (exfstat(storSysType, hostAddrBuf, dirname, (char *) &statbuf) < 0) {
            elog(NOTICE, 
	     "chkdir: Request failed - Unable to stat non-vault dir %s",
	      dirname);
            free (dirname);
            return INP_ERR_STAT;
        }
        uid = (int) ClientUser->userId;
#ifdef _WIN32
        if ((statbuf.st_mode & _S_IWRITE) == 0) 
#else
        if (uid != statbuf.st_uid && ((statbuf.st_mode & S_IWOTH) == 0)) 
#endif
	    {
            elog(NOTICE, 
	    "chkdir: Request failed - Only owner can write into dir:%s, uid=%d",
	      dirname, uid);
            free (dirname);
            return SRB_EPERM;
        } else {
            free (dirname);
            return 0;
        }
    } else if (chkFlag == CHK_FILE_PATH) {
	if ((status = exfstat(storSysType, hostAddrBuf, fileNameBuf, 
	  (char *) &statbuf)) < 0) { 
	    elog(NOTICE,
	      "chkdir: stat of file %s failed. status = %d", 
		fileNameBuf, status);
	    return (status);
	}
        uid = (int) ClientUser->userId;
#ifdef _WIN32
        if ((statbuf.st_mode & _S_IWRITE) == 0) 
#else
        if (uid != statbuf.st_uid && ((statbuf.st_mode & S_IWOTH) == 0)) 
#endif
	    {
            elog(NOTICE,
            "chkdir: CHK_FILE_PATH failed.Non-owner access for non-vault file:%s, uid=%d",
              fileNameBuf, uid);
            return SRB_EPERM;
        } else 
            return 0;
    } else if (chkFlag == RD_CHK) {
        return 0;
    } else if (chkFlag == NO_CHKDIR) {
    	return 0;
    } else {
        elog(NOTICE, "chkdir: Illegal chkFlag");
        return INP_ERR_CHKFLAG;
    }
}

/* getFilePath - Convert the file path in (varlena) format to (char) format.
 * If the input file path is an absolute path, just returns the absolute
 * path in (char) format. If the input file path is a relative path, the
 * file is assumed to be stored in the SRB storage vault with the
 * following absolute path name:
 *
 * 	vaultPath/userName/relativeFileName
 *
 * where vaultPath is the absolute path of the SRB vault for the given 
 * storage type, userName is the user login name.
 *
 * Input : int storSysType - Storage system type.
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP.
 *	   struct varlena *varBuf - The buffer that contains the input
 *		path name.
 *
 * Output : Returns the absolute path name.
 *                  NULL - Failure 
 */

char *getFilePath (int storSysType, char *filePath1)
{
    char *filePath2;
    char *vaultPath;

#ifdef _WIN32
    if ((filePath1[0] == '/') || 
        (filePath1[1] == ':')) {        /* This is an abs path */
	return (filePath1);
    }
#else
    if (filePath1[0] == '/') {		/* This is an abs path */
	return (filePath1);
    }
#endif

    /* This is a relative path. Must use one of the vault */

    vaultPath = getPathByType (storSysType);

    if (vaultPath == NULL) {
	elog (NOTICE, "Not storage vault corresponding to Access Method %d\n",
	storSysType);
	return (NULL);
    }

    filePath2 = (char *) malloc (strlen (vaultPath) + 
	strlen (ClientUser->userName) + strlen (filePath1) + 
	strlen (ClientUser->domainName) + 4);

    strcpy (filePath2, vaultPath);
    strcat (filePath2, "/");
    strcat (filePath2, ClientUser->userName);
    strcat (filePath2, ".");
    strcat (filePath2, ClientUser->domainName);
    strcat (filePath2, "/");
    strcat (filePath2, filePath1);

/* XXXXX - leave the original filePath alone
    free (filePath1);
*/

    return (filePath2);
}

/* initVault - Initialize the SRB vault info. i.e., read the VAULT_CONFIG file
 * and initialize that vault queue.
 *
 * Input - None
 *
 * Output - None
 */

void 
initVault ()
{
    FILE *file;
    char buf[MAX_LINE_SZ];
    int readState = RS_BEGIN;
    int storSysType;
    char *vaultPath;
    char *conFile;
    int i;
 
    /* put together the full pathname to the config file */
    conFile = (char *) malloc((strlen(getSrbDataDir()) +
	strlen(VAULT_CONFIG)+2)*sizeof(char));

    strcpy(conFile, getSrbDataDir());
    strcat(conFile, "/");
    strcat(conFile, VAULT_CONFIG);

    file = fopen(conFile, "r");

    if (file == NULL) {
	/* fprintf(stderr, "Unable to open VAULT_CONFIG file %s\n", conFile); */
    	free (conFile);
	return;
    }
	
    free (conFile);

    /* read the file */
 
    while ((i = getNextStr(file, buf, sizeof(buf))) != EOF) {
 
        /* Read until end of line then next line */

        if (buf[0] == '#')
        {
            while (i == 0)
                i = getNextStr(file, buf, sizeof(buf));
            continue;
        }
 
        switch (readState) {
            case RS_BEGIN:
                if (strcmp (buf, VAULT_KEY) == 0)
                    readState = RS_GOT_KEYWD;
                break;
 
            case RS_GOT_KEYWD:
                storSysType = atoi (buf);
                readState = RS_GOT_TYPE;
                break;
 
            case RS_GOT_TYPE:
                if (strcmp (buf, VAULT_KEY) == 0) {
                    readState = RS_GOT_KEYWD;
                    break;
                }
 
                if (buf[0] != '/') {
                    elog (NOTICE, "vaultPath %s must begin with a / \n", buf);
                    break;
                }
 
                vaultPath = (char *) malloc (strlen (buf) + 1);
                strcpy (vaultPath, buf);
                enqueVault (storSysType, vaultPath);
                break;
 
            default:
                /* Should not be here */
 
                break;
        }
    }
}
 
/*
 * getPathByType - Given the storSysType, return the first vaultPath
 * with a matching storSysType.
 *
 * Input : int storSysType - Storage system type.
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP.
 *
 * Output : Returns the SRB vault path for the input Storage system type.
 *	    NULL - vault path does not exist.
 */
 
char *getPathByType (int storSysType)
{
    struct vaultQueElement *tmpElement;
 
    tmpElement = vaultQueHead;
 
    while (tmpElement != NULL) {
        if (tmpElement->storSysType == storSysType)
            break;
        tmpElement = tmpElement->next;
    }
    if (tmpElement == NULL)
        return NULL;
 
    return (tmpElement->vaultPath);
}
 
/* mkVaultDir - Check whether the input file is in the SRB Vault. If it is
 * in, make the required directories if they don't exist.
 *
 * Input - The file Path.
 *
 * Output - Returns - 0 success
 *		      A negative vaule - error.
 */
int
mkVaultDir (int storSysType, char *hostAddrBuf, char *filePath, int pathType)
{
    struct stat statbuf;
    int pathlen, status;
    int tmpLen;
    char *dirname;

    pathlen = strlen (filePath);

    if (pathType == FILE_T) {       /* only go backward for file type */
    	while (pathlen && filePath[pathlen] != '/')
            pathlen --;
    }

    dirname = (char *) malloc (pathlen + 1);
    if (dirname == NULL)
	return SYS_ERR_MALLOC;

    strncpy (dirname, filePath, pathlen);
    dirname[pathlen] = '\0';

    /* Try to see if the directories exist. If not, make them */

    tmpLen = pathlen;

    while (tmpLen > 0) {
        if (exfstat(storSysType, hostAddrBuf, dirname, (char *) &statbuf) == 0)
	    break;	/* The directory exist */
	
	/* Go backward */

        while (tmpLen && dirname[tmpLen] != '/')
       	    tmpLen --;
	dirname[tmpLen] = '\0';
    }

    /* Now we go forward and mk the required dir */
    while (tmpLen < pathlen) {
	/* Put back the '\' */
	dirname[tmpLen] = '/';
	status = exfmkdir(storSysType, hostAddrBuf, dirname, 0750);
        if (status != 0) {
	    int n, remain;
	    /* it could already exist . created by another process */
	    n = status / 100;
	    remain = n * 100 - status;
	    if (remain != EEXIST) {
                elog (NOTICE, "mkVaultDir: Unable to make user vault dir %s",
                  dirname);
                free (dirname);
                if (status > 0)
                    return (-1);
                else
                    return status;
	    }
	}
        while (tmpLen && dirname[tmpLen] != '\0')
            tmpLen ++;
    }

    free (dirname);
    return 0;
}

#ifdef _WIN32
static int NT_mkVaultDir (int storSysType, char *filePath, int pathType)
{
	char fp[1024];
	strcpy(fp,filePath);
	SrbNtPathForwardSlash(fp);
	return mkVaultDir(storSysType,fp,pathType);
}
#endif

/* matchVault - Given a filePath, check if the filePath is underneath one
 * of the SRB storage vault path. If it is, returns the SRB storage vault
 * path that matches.
 *
 * Input : int storSysType - Storage system type.
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP.
 *	   char *filePath - The file path to match
 *
 * Output - Returns the SRB storage vault path
 *	    NULL - no match
 */

char *
matchVault (int storSysType, char *filePath)
{
    struct vaultQueElement *tmpElement;
    int len;
 
    tmpElement = vaultQueHead;
 
    while (tmpElement != NULL) {
        if (tmpElement->storSysType == storSysType) {
	    len = strlen (tmpElement->vaultPath);
	    if (strncmp (filePath, tmpElement->vaultPath, len) == 0) {
		    /* the next char must be a "/" or null */
		    if (filePath[len] == '/' || filePath[len] == '\0')
		    	return (tmpElement->vaultPath);
	    }
	}
        tmpElement = tmpElement->next;
    }
    return NULL;
}

/*
 * svrFileCreate Create an external file.
 * Input - int storSysType - Storage system type.
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP.
 *         struct varlena *hostAddr - The Host address of the storage.
 *         struct varlena *filename - The file Path name to create. 
 *	       Relative path => Storage Vault.
 *         int mode - same definition as in unix.
 *	   int size - the file size. -1 => don't know.
 * Output - Returns the created file descriptor
 *          Returns -1 upon failure.
 */

int
svrFileCreate(int storSysType, struct varlena *hostAddr, 
struct varlena *filename, int mode, int chdirFlag, srb_long_t *size)
{
    int fd;
    char *fileNameBuf, *hostAddrBuf;
    srb_long_t mySize;

    if (TicketUserFlag || ProxyUser->privUserFlag == 0) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return ILLEGAL_OPR_TICKET_USER;
    }

    mySize = *size;

    fileNameBuf = varToStr (filename);
    hostAddrBuf = varToStr (hostAddr);

    fd = _svrFileCreate (storSysType, hostAddrBuf, fileNameBuf, 
      mode, chdirFlag, mySize, GENERIC_CONN);

#ifdef UNIX_SYNC_ON_CLOSE
    if (fd >= 0 && exfDesc[fd].storSysType == UNIX_FT) {
        int dirFd;
        char myDir[MAX_TOKEN];
        char myFile[MAX_TOKEN];
        int status;

        splitbychar (exfDesc[fd].filename, myDir, myFile, '/');
        dirFd = exfopen (exfDesc[fd].storSysType,
          exfDesc[fd].hostAddr, myDir, O_RDONLY, 0600);
        if (dirFd < 0) {
            elog (NOTICE,
              "svrFileCreate: exfopendir of %s failed. errno = %d",
              myDir, errno);
        } else {
            status = exfsync (exfDesc[fd].storSysType,
              exfDesc[fd].hostAddr, dirFd);
            if (status < 0) {
                elog (NOTICE,
                  "svrFileCreate: exfsync of dir %s failed. errno = %d",
                  myDir, errno);
            } else {
                exfclose (exfDesc[fd].storSysType,
                  exfDesc[fd].hostAddr, dirFd);
            }
        }
    }
#endif

    free (fileNameBuf);
    free (hostAddrBuf);

    return fd;
}

/* _svrFileCreate () - This is the create with all varlena stuff stripped out.
 *
 * Input - int storSysType - Storage system type.
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP.
 *         char *hostAddr - The Host address of the storage.
 *         char *filename - The file Path name to create.
 *         int mode - same definition as in unix.
 *	   srb_long_t size - the file size. -1 => don't know.
 * Output - Returns the created file descriptor
 *          Returns -1 upon failure.
 */
int
_svrFileCreate (int storSysType, char *hostAddr, char *fileName, 
int mode, int chdirFlag, srb_long_t size, int remoteConnFlag)
{
    int fd;
    int i;
    int remoteFlag;
#ifndef FED_MCAT
    char *portNum, tmpResLoc[MAX_TOKEN];                            
#endif
    struct hostElement  *hostTabPtr;
    int status;
    srbConn *conn = NULL;

    /* Check whether the host is remote or local. Note: hostAddr now contains
     * only the host name */
 
#ifdef FED_MCAT
    remoteFlag = chkHostAndZone (hostAddr, &hostTabPtr);
#else
    /* Parse the hostAddr */
    strcpy (tmpResLoc, hostAddr);
    portNum = parseAddr (tmpResLoc);
    remoteFlag = chkHost (tmpResLoc, portNum, &hostTabPtr);
#endif
 
    if (remoteFlag < 0) {
        elog (NOTICE, "Invalid host addr %s", hostAddr);
        return INP_ERR_HOST_ADDR;		/* failure */
    } else if (remoteFlag == REMOTE_HOST) {
        if (remoteConnFlag == GENERIC_CONN) {
            fd = remoteFileCreate (storSysType, hostTabPtr, fileName, mode,
             chdirFlag, size, hostAddr);
        } else {
            fd = remoteFileCreateUni (storSysType, hostTabPtr, fileName, mode,
             chdirFlag, size, &conn);
	}
    } else if (remoteFlag == LOCAL_HOST) {
#ifdef _WIN32
		status = NT_mkVaultDir (storSysType, fileName, FILE_T);
		if(status < 0)
			return status;
#else
        if (chdirFlag == DO_CHKDIR || chdirFlag == CHK_AND_MK_VAULT) {
    	    if ((status = chkdir (storSysType, hostAddr, fileName, 
	      WRT_CHK, FILE_T)) < 0) {
            	return status;
    	    }
	} else {
	    /* Make the home dir if it does not exist */
	    mkVaultDir (storSysType, hostAddr, fileName, FILE_T);
	}
#endif
        fd = exfcreate(storSysType, hostAddr, fileName, mode, size);
    }

    if (fd < 0) { /* create failed */
	elog(NOTICE,"cannot create ext file %s", fileName);
        return fd;
    }
    
    i = newExfFd(storSysType, hostAddr, hostTabPtr->portNum, hostTabPtr, 
	fileName, fd, remoteFlag, conn);


    return i;
}

/*
 * svrFileUnlink - Unlink an external file
 *
 * Input - int storSysType - Storage system type.
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP.
 *         struct varlena *hostAddr - The Host address of the storage.
 *         struct varlena *filename - The file Path name to unlink. 
 *	   Relative path => Storage Vault.
 * Output - Returns 0 upon success.
 *          Returns -1 upon failure.
 */

int
svrFileUnlink(int storSysType, struct varlena *hostAddr, 
struct varlena *filename, int chdirFlag)
{
    char *fileNameBuf, *hostAddrBuf;
    int status;
  
    if (TicketUserFlag || ProxyUser->privUserFlag == 0) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    fileNameBuf = varToStr (filename);
    hostAddrBuf = varToStr (hostAddr);

    status = _svrFileUnlink (storSysType, hostAddrBuf, fileNameBuf, chdirFlag);
   
    free (fileNameBuf);
    free (hostAddrBuf);

    return status;
}
 
/*
 * _svrFileUnlink - The internal version of srbFileUnlink with all varlena 
 *  stuff stripped out.
 *
 * Input - int storSysType - Storage system type.
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP.
 *         char *hostAddr - The Host address of the storage.
 *         char *filename - The file Path name to unlink.
 * Output - Returns 0 upon success.
 *          Returns -1 upon failure.
 */

int
_svrFileUnlink(int storSysType, char *hostAddr, char *fileName, int chdirFlag)
{
    int status;
    int remoteFlag;
#ifndef FED_MCAT
    char *portNum, *tmpResLoc;
#endif
    struct hostElement  *hostTabPtr;
    char *fileNameBuf;

    /* Parse the hostAddr */
 
    /* Check whether the host is remote or local. Note: hostAddr now contains
     * only the host name */
 
#ifdef FED_MCAT
    remoteFlag = chkHostAndZone (hostAddr, &hostTabPtr);
#else
    tmpResLoc = strdup (hostAddr);
    portNum = parseAddr (tmpResLoc);
    remoteFlag = chkHost (tmpResLoc, portNum, &hostTabPtr);
#endif
 
    if (remoteFlag < 0) {
        elog (NOTICE, "Invalid host addr %s", hostAddr);
        return INP_ERR_HOST_ADDR;
    } else if (remoteFlag == REMOTE_HOST) {
        status = remoteFileUnlink (storSysType, hostTabPtr, fileName,
        chdirFlag, hostAddr);
    } else if (remoteFlag == LOCAL_HOST) {
        if (storSysType != FTP_FT && storSysType != HTTP_FT) {
            fileNameBuf = getFilePath (storSysType, fileName);
            if (fileNameBuf == NULL) {
                return INP_ERR_FILENAME;
            }
            if (chdirFlag == DO_CHKDIR) {
                if ((status = chkdir (storSysType, hostAddr, fileNameBuf, 
		  WRT_CHK, FILE_T)) < 0) {
		    if (fileNameBuf != fileName)
			free (fileNameBuf);
                     return status;
                }
            } else if (storSysType == GRIDFTP_FT) {
                if (matchVault (storSysType, fileName) == NULL)
                    VaultpathStatus = PATH_NOT_IN_VAULT;
                else
                    VaultpathStatus = PATH_IN_VAULT;
            }
        } else {
	    fileNameBuf = fileName;
	}
        status = exfunlink (storSysType, hostAddr, fileNameBuf);
	if (fileNameBuf != fileName)
            free (fileNameBuf);
    }

    return status;
}

/*
 * svrFileClose - close an external file
 *
 * Input - int fd_inx - The file descriptor to close (from srbFileOpen or
 *         srbFileCreate).
 *
 * Output - Returns 0 upon success.
 *          Returns -1 upon failure.
 */

int
svrFileClose(int fd_inx)
{
    int status;
    srbConn* conn;

    if (fd_inx < 0 || fd_inx >= MAX_EXF_FDS || exfDesc[fd_inx].fd < 0) {
        elog(NOTICE,"svrFileClose: ext file desc (%d) out of range", fd_inx);
	return INP_ERR_DESC;
    }
  
    if (exfDesc[fd_inx].remoteFlag == REMOTE_HOST) {
        if (exfDesc[fd_inx].conn != NULL) {
            conn = exfDesc[fd_inx].conn;
        } else {
	    conn = getRemoteConn (exfDesc[fd_inx].hostTabPtr);
    	    if (conn == NULL)
	        return SYS_ERR_REMOTE_CONN;
	}
        status = srbFileClose (conn, exfDesc[fd_inx].fd);
    } else {
#ifdef UNIX_SYNC_ON_CLOSE
	int dirFd;
        char myDir[MAX_TOKEN];
        char myFile[MAX_TOKEN];

	if (exfDesc[fd_inx].writeFlag > 0 && 
	  exfDesc[fd_inx].storSysType == UNIX_FT) {
	    status = exfsync(exfDesc[fd_inx].storSysType, 
	      exfDesc[fd_inx].hostAddr, exfDesc[fd_inx].fd);
	    if (status < 0) {
		elog (NOTICE,
		  "svrFileClose: exfsync of ext file %s failed. errno = %d", 
		  exfDesc[fd_inx].filename, errno);
	    }
	}
#endif	/* UNIX_SYNC_ON_CLOSE */

    	status = exfclose (exfDesc[fd_inx].storSysType, 
	  exfDesc[fd_inx].hostAddr, exfDesc[fd_inx].fd);
    }
    deleteExfFd (fd_inx);

    return status;
}

/*
 * _svrFileWrite - The internal version of svrFileWrite - Write len bytes 
 * of buf into the external file fd
 *
 * Input - int fd_inx - The file descriptor to write (from srbFileOpen or
 *         srbFileCreate).
 *         char *buf - The output buffer.
 *         int len - The length to write.
 *
 * Output - Returns the number of bytes written..
 *          Returns -1 upon failure.
 *
 */

int
_svrFileWrite(int fd_inx, char *buf, int len)
{
    srbConn* conn;
    int status;

    if (fd_inx < 0 || fd_inx >= MAX_EXF_FDS || exfDesc[fd_inx].fd < 0) {
	elog(NOTICE,"_svrFileWrite: ext file desc (%d) out of range", fd_inx);
        return INP_ERR_DESC;
    }

    if (exfDesc[fd_inx].remoteFlag == REMOTE_HOST) {
#ifdef DEMO
        elog (DEMOM,
	  "_svrFileWrite: File on remote host. Will do a remote write");
#endif
        if (exfDesc[fd_inx].conn != NULL) {
            conn = exfDesc[fd_inx].conn;
        } else {
            conn = getRemoteConn (exfDesc[fd_inx].hostTabPtr);
            if (conn == NULL)
                return SYS_ERR_REMOTE_CONN;
        }

	status = srbFileWrite (conn, exfDesc[fd_inx].fd, buf, len);
    } else {
#ifdef DEMO
        elog (DEMOM,
	  "_svrFileWrite: File on local host. Will do a local write");
#endif
    	status = exfwrite(exfDesc[fd_inx].storSysType, exfDesc[fd_inx].hostAddr,
		    exfDesc[fd_inx].fd, buf, len);
    }
    if (status > 0)
	exfDesc[fd_inx].writeFlag = 1;	/* We wrote something */
    return status;
}

/*
 * svrFileLockRange - set or check a lock on a range within a file
 *
 * Input - int fd_inx - The file descriptor to seek (from srbFileOpen or
 *         srbFileCreate).
 *         srb_long_t offset - The position of the operation
 *         int size - The length of the lock range.
 *         int mode - The operation mode (MDSPlus convention) :
 *            0 Unlock
 *            1 ReadLock
 *            2 WriteLock
 *
 * Output - Returns 0 upon success.
 *          Returns -1 upon failure.
*/
int
svrFileLockRange(int fd_inx, srb_long_t offset, int size, int mode) {
    int retVal;
    retVal = _svrFileLockRange (fd_inx, offset, size, mode);
    return (retVal);
}

/* 
 Internal version of svrFileLockRange, see it for a description.
*/
int 
_svrFileLockRange(int fd_inx, srb_long_t offset, int size, int mode) {
    srbConn* conn;
    int status;

    if (fd_inx < 0 || fd_inx >= MAX_EXF_FDS || exfDesc[fd_inx].fd < 0) {
        elog(NOTICE,"svrFileLockRange: ext file desc (%d) out of range",
             fd_inx);
        return INP_ERR_DESC;
    }
 
    if (exfDesc[fd_inx].remoteFlag == REMOTE_HOST) {
        if (exfDesc[fd_inx].conn != NULL) {
            conn = exfDesc[fd_inx].conn;
        } else {
            conn = getRemoteConn (exfDesc[fd_inx].hostTabPtr);
            if (conn == NULL)
                    return SYS_ERR_REMOTE_CONN;
        }
        return srbFileLockRange (conn, exfDesc[fd_inx].fd, offset, size, mode);
    } else {
        struct flock flock_info;
        flock_info.l_pid = 0;
        flock_info.l_start = 0;
        if (offset >= 0) flock_info.l_start = offset;
        flock_info.l_len = size;
        flock_info.l_whence = SEEK_END;
        if (mode == 0) {
            flock_info.l_type = F_UNLCK;
            flock_info.l_whence = SEEK_SET;
            flock_info.l_start = 0;
            flock_info.l_len = 0;
        }
        else if (mode==1) {
            flock_info.l_type = F_RDLCK;
            if (offset >=0) flock_info.l_whence = SEEK_SET; 
        }
        else {
            flock_info.l_type = F_WRLCK;
            if (offset >=0) flock_info.l_whence = SEEK_SET; 
        }
        status = fcntl(exfDesc[fd_inx].fd, F_SETLKW, &flock_info);
        if (status != -1) return 0;
        return -1;
        /*
        MDSPlus equivalent:
        flock_info.l_type = (mode == 0) ? F_UNLCK : ((mode == 1) ? F_RDLCK : F_WRLCK);
        flock_info.l_whence = (mode == 0) ? SEEK_SET : ((offset >= 0) ? SEEK_SET : SEEK_END);
        flock_info.l_start = (mode == 0) ? 0 : ((offset >= 0) ? offset : 0);
        flock_info.l_len = (mode == 0) ? 0 : size;
        status = (fcntl(exfDesc[fd_inx].fd,F_SETLKW, &flock_info) != -1) ? TreeSUCCESS : TreeLOCK_FAILURE;
        */
    }
}

/*
 * svrFileSeek - Change the current read or write location on an external file
 * currently.
 *
 * Input - int fd_inx - The file descriptor to seek (from srbFileOpen or
 *         srbFileCreate).
 *         srb_long_t offset - The position of the next operation
 *         int whence - Same definition as in Unix.
 *              SEEK_SET - pointer is set to the value of the Offset parameter.
 *              SEEK_CUR - pointer is set to its current location plus the
 *                      value of the Offset parameter.
 *              SEEK_END - pointer is set to the size of the file plus the
 *                      value of the Offset parameter.
 *
 * Output - Returns 0 upon success.
 *          Returns -1 upon failure.
 *
 */

srb_long_t *
svrFileSeek(int fd_inx, srb_long_t *offset, int whence)
{
    srb_long_t *retVal;

    retVal = (srb_long_t *) malloc (2*SIZEOF32);

    *retVal = _svrFileSeek (fd_inx, *offset, whence);

    return (retVal);
}

/*
 * _svrFileSeek - Internal version of svrFileSeek.
 *
 * Input - int fd_inx - The file descriptor to seek (from srbFileOpen or
 *         srbFileCreate).
 *         srb_long_t offset - The position of the next operation
 *         int whence - Same definition as in Unix.
 *              SEEK_SET - pointer is set to the value of the Offset parameter.
 *              SEEK_CUR - pointer is set to its current location plus the
 *                      value of the Offset parameter.
 *              SEEK_END - pointer is set to the size of the file plus the
 *                      value of the Offset parameter.
 *
 * Output - Returns 0 upon success.
 *          Returns -1 upon failure.
 *
 */

srb_long_t
_svrFileSeek(int fd_inx, srb_long_t offset, int whence)
{
    srbConn* conn;
    srb_long_t status;

    if (fd_inx < 0 || fd_inx >= MAX_EXF_FDS || exfDesc[fd_inx].fd < 0) {
        elog(NOTICE,"svrFileSeek: ext file desc (%d) out of range", fd_inx);
        return INP_ERR_DESC;
    }
 
    if (exfDesc[fd_inx].remoteFlag == REMOTE_HOST) {
	if (exfDesc[fd_inx].conn != NULL) {
	    conn = exfDesc[fd_inx].conn;
	} else {
            conn = getRemoteConn (exfDesc[fd_inx].hostTabPtr);
            if (conn == NULL)
            	return SYS_ERR_REMOTE_CONN;
	}
        return srbFileSeek (conn, exfDesc[fd_inx].fd, offset, whence);
    } else {
    	status = exfseek(exfDesc[fd_inx].storSysType, exfDesc[fd_inx].hostAddr,
	  exfDesc[fd_inx].fd, offset, whence);

        return status;
    }
}

/*
 * svrFileSync - Sync an external file
 *
 * Input - int fd_inx - The file descriptor to sync (from srbFileOpen or
 *         srbFileCreate).
 *
 * Output - Returns 0 upon success.
 *          Returns -1 upon failure.
 *
 */

int
svrFileSync(int fd_inx)
{
    srbConn* conn;
    int status;
 
    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    if (fd_inx < 0 || fd_inx >= MAX_EXF_FDS || exfDesc[fd_inx].fd < 0) {
        elog(NOTICE,"svrFileSync: ext file desc (%d) out of range", fd_inx);
        return INP_ERR_DESC;
    }
 
    if (exfDesc[fd_inx].remoteFlag == REMOTE_HOST) {
        if (exfDesc[fd_inx].conn != NULL) {
            conn = exfDesc[fd_inx].conn;
        } else {
            conn = getRemoteConn (exfDesc[fd_inx].hostTabPtr);
            if (conn == NULL)
            	return SYS_ERR_REMOTE_CONN;
	}
        return srbFileSync (conn, exfDesc[fd_inx].fd);
    } else {
    	status = exfsync(exfDesc[fd_inx].storSysType, exfDesc[fd_inx].hostAddr,
	 exfDesc[fd_inx].fd);

        return status;
    }
}

/*
 * svrFileOpen - Opens an external file
 *
 * Input - int storSysType - Storage system type.
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP.
 *         struct varlena *hostAddr - The Host address of the SRB.
 *         varlena *filename - The file Path name to open. Relative path =>
 *              Storage Vault.
 *         int flags - same definition as in unix.
 *         int mode - same definition as in unix.
 * Output - Returns the file descriptor for use in later srbFile* calls.
 *          Returns -1 upon failure.
 */

int
svrFileOpen(int storSysType, struct varlena *hostAddr, 
struct varlena *filename, int flags, int mode, int chdirFlag)
{
    int fd;
    char *fileNameBuf, *hostAddrBuf;

    /* the proxy user is not a privileged user */

    if (ProxyUser->privUserFlag == 0) {
	elog (NOTICE, "Illegal operation for a ticket user");
        return ILLEGAL_OPR_TICKET_USER;
    }

    fileNameBuf = varToStr (filename);
    hostAddrBuf = varToStr (hostAddr);

    /* Don't chkdir for HTTP, FTP type storage system */
    if (srbObjTypeEntry[storSysType].chdirFlag == NO_CHKDIR)
	chdirFlag = NO_CHKDIR;
    fd = _svrFileOpen(storSysType, hostAddrBuf, fileNameBuf, flags, mode,
	chdirFlag, GENERIC_CONN);

    free (fileNameBuf);
    free (hostAddrBuf);

    return fd;
}

/*
 * _svrFileOpen - Internal version of svrFileOpen. Opens an external file
 *
 * Input - int storSysType - Storage system type.
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP.
 *         char *hostAddr - The Host address of the SRB.
 *         char *filename - The file Path name to open.
 *         int flags - same definition as in unix.
 *         int mode - same definition as in unix.
 *	   char **fullName - The address to put the fullName.
 * Output - Returns the file descriptor for use in later srbFile* calls.
 *	      The address of the fullName is put in char **fullName.
 *          Returns -1 upon failure.
 *	
 */

int
_svrFileOpen(int storSysType, char *hostAddr, char *fileName, 
int flags, int mode, int chdirFlag, int remoteConnFlag)
{
    int fd;
    int i;
    int remoteFlag;
#ifndef FED_MCAT
    char *portNum, tmpResLoc[MAX_TOKEN];                                                             
#endif
    struct hostElement  *hostTabPtr;
    int status;
    srbConn *conn = NULL;

    /* Check whether the host is remote or local. Note: hostAddr now 
     * contains only the host name */

#ifdef FED_MCAT
    remoteFlag = chkHostAndZone (hostAddr, &hostTabPtr);
#else
    /* Parse the hostAddr */
    strcpy (tmpResLoc, hostAddr);
    portNum = parseAddr (tmpResLoc);
    remoteFlag = chkHost (tmpResLoc, portNum, &hostTabPtr);
#endif

    if (remoteFlag < 0) {
	elog (NOTICE, "Invalid host addr %s", hostAddr); 
	return INP_ERR_HOST_ADDR;
    } else if (remoteFlag == REMOTE_HOST) {
#ifdef DEMO
	elog (DEMOM,
	  "_svrFileOpen: %s is a remote host. Will do a remoteFileOpen",
	    hostAddr);
#endif
	if (remoteConnFlag == GENERIC_CONN) {
	    fd = remoteFileOpen (storSysType, hostTabPtr, fileName, flags, 
	     mode, chdirFlag, hostAddr);  
	} else {
            fd = remoteFileOpenUni (storSysType, hostTabPtr, fileName, flags, 
             mode, chdirFlag, &conn);
	}
        if (chdirFlag == CHK_FILE_PATH || chdirFlag == CHK_VAULT_PATH) {    
	    /* only check the file */
            return (fd);
	}
    } else if (remoteFlag == LOCAL_HOST) {

#ifdef DEMO
        elog (DEMOM, "_svrFileOpen: %s is a local host. Will Open file here.",
            hostAddr);
	elog(DEMOM, "_svrFileOpen: Storage type= %d, Path Name = %s\n",
	    storSysType, fileName);
#endif
       	if (chdirFlag == DO_CHKDIR) {
	    if (flags & O_RDWR || flags & O_WRONLY || flags & O_CREAT) {
                if ((status = chkdir (storSysType, hostAddr, fileName, 
	          WRT_CHK, FILE_T)) < 0){
            	        return status;
            	}
            }
    	} else if (chdirFlag == CHK_FILE_PATH) {    /* only check the file */
	    status = 
	      chkdir (storSysType, hostAddr, fileName, chdirFlag, FILE_T);
	    return (status);
	} else if (chdirFlag == CHK_VAULT_PATH) {
	    if (matchVault (storSysType, fileName) == NULL)
		return (SYS_ERR_FILE_NOT_IN_VAULT);
	    else
		return (0);
        } else if (storSysType == GRIDFTP_FT) {
            if (matchVault (storSysType, fileName) == NULL)
		VaultpathStatus = PATH_NOT_IN_VAULT;
            else
		VaultpathStatus = PATH_IN_VAULT;
	}

    	fd = exfopen(storSysType, hostAddr, fileName, flags, mode);
 
    }

    if (fd < 0) { /* lookup failed */
        elog(NOTICE,"cannot open ext file %s", fileName); 
        return fd;
    }

    i = newExfFd(storSysType, hostAddr, hostTabPtr->portNum, hostTabPtr, 
      fileName, fd, remoteFlag, conn);
    return i;
}

/*
 * _svrFileRead - The internal version of svrFileRead. Read len bytes of 
 * the external file into buf.
 *
 * Input - int fd_inx - The file descriptor to read (from srbFileOpen or
 *         srbFileCreate).
 *         char *buf - The input buffer.
 *         int len - The length to read.
 *
 * Output - Returns the number of bytes read..
 *          Returns -1 upon failure.
 */

int _svrFileRead(int fd_inx, char *buf, int len)
{
    srbConn* conn;
    int status;

    if (fd_inx < 0 || (fd_inx >= MAX_EXF_FDS) || (exfDesc[fd_inx].fd < 0)) {
	elog(NOTICE,"_svrFileRead: ext file desc (%d) out of range", fd_inx);
        return INP_ERR_DESC;
    }

    if (exfDesc[fd_inx].remoteFlag == REMOTE_HOST) {
#ifdef DEMO
        elog (DEMOM,
	  "_svrFileRead: File on remote host. Will do a remote read");
#endif

        if (exfDesc[fd_inx].conn != NULL) {
            conn = exfDesc[fd_inx].conn;
        } else {
            conn = getRemoteConn (exfDesc[fd_inx].hostTabPtr);
            if (conn == NULL)
                return SYS_ERR_REMOTE_CONN;
        }

        return srbFileRead (conn, exfDesc[fd_inx].fd, buf, len);
    } else {
#ifdef DEMO
        elog (DEMOM,
	  "_svrFileRead: File on local host. Will do a local read");
#endif
    	status =  exfread(exfDesc[fd_inx].storSysType, exfDesc[fd_inx].hostAddr,
    	  exfDesc[fd_inx].fd, buf, len);

        return status;
    }
}

/* exfinit - Initialize the exfDesc.
 *
 * Input : None
 *
 * Output : None
 */

void exfinit() 
{
    int i;

#ifdef PARA_OPR
    pthread_mutex_init (&ExfLock, pthread_mutexattr_default);
#endif

    for (i = 0; i < MAX_EXF_FDS; i++) {
        exfDesc[i].fd = -1;
        exfDesc[i].remoteFlag = 0;
        exfDesc[i].hostAddr = NULL;
        exfDesc[i].portNum = NULL;
        exfDesc[i].filename = NULL;
        exfDesc[i].storSysDepend = NULL;
        exfDesc[i].conn = NULL;
    }
}

/* newExfFd - Get a new exfDesc and fill in the details
 * 
 * Input : int storSysType - Storage system type.
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP.
 *	   char *hostAddr - The host address of the storage.
 *	   char *filename - The file Path name.
 *	   int fd - The file descriptor of the opened path.
 * Output - The index of the new exfDesc.
 */

static int
newExfFd(int storSysType, char *hostAddr, char *portNum, 
struct hostElement  *hostTabPtr, char *filename, int fd, int remoteFlag,
srbConn *conn)
{
    int i;
 
#ifdef PARA_OPR
    pthread_mutex_lock (&ExfLock);
#endif

    for (i = 0; i < MAX_EXF_FDS; i++) {
 
        if (exfDesc[i].fd == -1) {
            exfDesc[i].fd = fd;
            exfDesc[i].remoteFlag = remoteFlag;
	    if (portNum != NULL) {
		exfDesc[i].portNum = strdup (portNum);
#ifdef foo
 	    	exfDesc[i].portNum = malloc (strlen (portNum) + 1);
		strcpy (exfDesc[i].portNum, portNum);
#endif
	    } else {
		exfDesc[i].portNum = NULL;
	    }
            exfDesc[i].hostTabPtr = hostTabPtr;
	    exfDesc[i].hostAddr = strdup (hostAddr);
	    exfDesc[i].filename = strdup (filename);
#ifdef foo
	    exfDesc[i].hostAddr = malloc (strlen (hostAddr) + 1);
	    strcpy (exfDesc[i].hostAddr, hostAddr);
	    exfDesc[i].filename = malloc (strlen (filename) + 1);
	    strcpy (exfDesc[i].filename, filename);
#endif
            exfDesc[i].storSysType = storSysType;
            exfDesc[i].writeFlag = 0;
	    exfDesc[i].conn = conn;
#ifdef PARA_OPR
	    pthread_mutex_unlock (&ExfLock);
#endif
            return i;
        }
    }
#ifdef PARA_OPR
    pthread_mutex_unlock (&ExfLock);
#endif

    return SYS_ERR_DESC_OVERFLOW;
}

/* printExfFd - Print the content of a exfDesc.
 *
 * Input : int i - The index of the exfDesc to print.
 * Output - None
 */

void
printExfFd(int i)
{
  printf("ed(%i)::FD=%i:AM=%i\n",i, exfDesc[i].fd,
	 exfDesc[i].storSysType);
}

/* deleteExfFd - return a exfDesc to the free pool.
 *
 * Input : int i - The index of the exfDesc to delete.
 * Output - None
 */

static void
deleteExfFd(int i)
{
    exfDesc[i].fd = -1;
    if (exfDesc[i].hostAddr != NULL) {
        free (exfDesc[i].portNum);
        exfDesc[i].portNum = NULL;
    }
    if (exfDesc[i].hostAddr != NULL) {
        free (exfDesc[i].hostAddr);
        exfDesc[i].hostAddr = NULL;
    }
    if (exfDesc[i].filename != NULL) {
        free (exfDesc[i].filename);
        exfDesc[i].filename = NULL;
    }
    exfDesc[i].storSysDepend = NULL;
    if (exfDesc[i].conn != NULL) {
	clFinish (exfDesc[i].conn);
	exfDesc[i].conn = NULL;
    }
}

/*
 * svrFileRead - Read len bytes of the external file into buf.
 *
 * Input - int fd_inx - The file descriptor to read (from srbFileOpen or
 *         srbFileCreate).
 *         char *buf - The input buffer.
 *         int len - The length to read.
 *
 * Output - Returns the number of bytes read..
 *          Returns -1 upon failure.
 */

struct varlena *
svrFileRead(int fd, int len)
{
    struct varlena *retval;
    int totalread = 0;
    int status;

    retval = (struct varlena *)malloc(VAROUTHDRSZ + len);
    totalread = _svrFileRead(fd, VAROUTDATA(retval), len);

    if (totalread >= 0) {
	status = 0;
    } else {
	status = totalread;
	totalread = 0;
    }
    VARSIZE(retval) = totalread + VAROUTHDRSZ;
    VAROUTSTAT(retval) = htonl (status);
 
    return retval;
}

/*
 * svrFileWrite - Write len bytes of buf into the external file fd
 *
 * Input - int fd - The file descriptor to write (from srbFileOpen or
 *         srbFileCreate).
 *         struct varlena *wbuf - The output buffer.
 *         int len - The length to write.
 *
 * Output - Returns the number of bytes written..
 *          Returns -1 upon failure.
 *
 */

int svrFileWrite(int fd, struct varlena *wbuf)
{
    int totalwritten;
    int bytestowrite;
 
    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    bytestowrite = VARSIZE(wbuf) - SIZEOF32;
    totalwritten = _svrFileWrite(fd, VARDATA(wbuf), bytestowrite);
    return totalwritten;
}

/*
 * svrFileStat - Stat an external file
 *
 * Input - srbConn* conn - From clConnect ().
 *         int storSysType - Storage system type.
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP.
 *         struct varlena *hostAddr - The Host address of the storage.
 *         struct varlena *filename - The file Path name to stat. 
 *	   Relative path => Storage Vault.
 *
 * Output - stat result in varlena format.
 */

struct varlena *
svrFileStat(int storSysType, struct varlena *hostAddr, 
struct varlena *filename)
{
    struct varlena *retval;
    int status = 0;
    char *fileNameBuf, *hostAddrBuf;
#if defined(PORTNAME_c90)
    struct srbStat myStat;
    struct srbStat *retStat;
#else
    struct srbStat *retStat;
#endif
    char *portNum, tmpResLoc[MAX_TOKEN];
 
    fileNameBuf = varToStr (filename);
    hostAddrBuf = varToStr (hostAddr);

    /* Parse the hostAddr */
 
    strcpy (tmpResLoc, hostAddrBuf);
    portNum = parseAddr (tmpResLoc);
 
    /* Check whether the host is remote or local. Note: hostAddr now contains
     * only the host name */
 
 
    retval = (struct varlena *)malloc(VAROUTHDRSZ + sizeof (struct srbStat));
#if defined(PORTNAME_c90)
    retStat = &myStat;
#else
    retStat = (struct srbStat *) (VARDATA(retval) + sizeof(INT32));
#endif

    status = _svrFileStat(storSysType, tmpResLoc, portNum, 
      fileNameBuf, retStat);

ProcessStatus:

    free (fileNameBuf);
    free (hostAddrBuf);

    if (status < 0) {           /* error */
        VARSIZE(retval) = VAROUTHDRSZ;
        VAROUTSTAT(retval) = htonl (status);
    } else {
	htonStat (retStat);
#if defined(PORTNAME_c90)
    	memcpy (VAROUTDATA(retval), (char *) retStat, sizeof(struct srbStat));
#endif  /* PORTNAME_c90 */
        VARSIZE(retval) = sizeof (struct srbStat) + VAROUTHDRSZ;
        VAROUTSTAT(retval) = 0;
    }

    return retval;
}

struct varlena *
svrFileFstat(int storSysType, struct varlena *hostAddr, int fd_inx)
{
    struct varlena *retval;
    int status = 0;
    char *hostAddrBuf;
#if defined(PORTNAME_c90)
    struct srbStat myStat;
    struct srbStat *retStat;
#else
    struct srbStat *retStat;
#endif
    char *portNum;
 
    hostAddrBuf = varToStr (hostAddr);

    /* Parse the hostAddr */
 
    portNum = parseAddr (hostAddrBuf);
 
    /* Check whether the host is remote or local. Note: hostAddr now contains
     * only the host name */
 
 
    retval = (struct varlena *)malloc(VAROUTHDRSZ + sizeof (struct srbStat));
#if defined(PORTNAME_c90)
    retStat = &myStat;
#else
    retStat = (struct srbStat *) (VARDATA(retval) + sizeof(INT32));
#endif

    status = _svrFileFstat(storSysType, hostAddrBuf, portNum, 
     fd_inx, retStat);

ProcessStatus:

    free (hostAddrBuf);

    if (status < 0) {           /* error */
        VARSIZE(retval) = VAROUTHDRSZ;
        VAROUTSTAT(retval) = htonl (status);
    } else {
	htonStat (retStat);
#if defined(PORTNAME_c90)
    	memcpy (VAROUTDATA(retval), (char *) retStat, sizeof(struct srbStat));
#endif  /* PORTNAME_c90 */
        VARSIZE(retval) = sizeof (struct srbStat) + VAROUTHDRSZ;
        VAROUTSTAT(retval) = 0;
    }

    return retval;
}

int
_svrFileStat(int storSysType, char *hostAddrBuf, char *portNum, 
char *fileNameBuf, struct srbStat *retStat)
{
    int remoteFlag;
    struct hostElement  *hostTabPtr;
    struct stat statbuf;
    int status;

#ifdef FED_MCAT
    remoteFlag = chkHostAndZone (hostAddrBuf, &hostTabPtr);
#else
    remoteFlag = chkHost (hostAddrBuf, portNum, &hostTabPtr);
#endif

    if ((status = remoteFlag) < 0) {
        elog (NOTICE, "Invalid host addr %s", hostAddrBuf);
	return status;
    } else if (remoteFlag == REMOTE_HOST) {
	status = remoteFileStat (storSysType, hostTabPtr, fileNameBuf, retStat,
	 hostAddrBuf);
    } else if (remoteFlag == LOCAL_HOST) {
        if (storSysType == GRIDFTP_FT) {
            if (matchVault (storSysType, fileNameBuf) == NULL)
                VaultpathStatus = PATH_NOT_IN_VAULT;
            else
                VaultpathStatus = PATH_IN_VAULT;
        }
        status = exfstat(storSysType, hostAddrBuf, fileNameBuf, 
	 (char *) &statbuf);
	if (status >= 0) {		/* success */
	    /* Fill in the blanks */
	    fillSrbStatBuf (retStat, &statbuf);
        }
    }
    return (status);
}

int
_svrFileFstat(int storSysType, char *hostAddrBuf, char *portNum, 
int fd_inx, struct srbStat *retStat)
{
    int remoteFlag;
    struct hostElement  *hostTabPtr;
    struct stat statbuf;
    int status;

#ifdef FED_MCAT
    remoteFlag = chkHostAndZone (hostAddrBuf, &hostTabPtr);
#else
    remoteFlag = chkHost (hostAddrBuf, portNum, &hostTabPtr);
#endif

    if ((status = remoteFlag) < 0) {
        elog (NOTICE, "Invalid host addr %s", hostAddrBuf);
	return status;
    } else if (remoteFlag == REMOTE_HOST) {
	/* KESTOCK - change fd_inx to exfDesc[fd_inx].fd */
	status = remoteFileFstat (storSysType, hostTabPtr, exfDesc[fd_inx].fd, 
	 retStat);
    } else if (remoteFlag == LOCAL_HOST) {
        /* for HPSS_FT, hpss_FileGetAttributes to get the 64 bit size. */
        if (storSysType == HPSS_FT) {
            status = exfstat(storSysType, hostAddrBuf,
             exfDesc[fd_inx].filename, (char *) &statbuf);
        } else {
            status = exffstat(storSysType, hostAddrBuf, exfDesc[fd_inx].fd,
             (char *) &statbuf);
        }
	if (status >= 0) {		/* success */
	    /* Fill in the blanks */
	    fillSrbStatBuf (retStat, &statbuf);
        }
    }
    return (status);
}

static int 
fillSrbStatBuf (struct srbStat *retStat, struct stat *statbuf)
{
    if (retStat == NULL || statbuf == NULL)
	return (-1);

    retStat->st_dev = (srb_dev_t) (statbuf->st_dev);
    retStat->st_ino = (srb_ino_t) (statbuf->st_ino);
    retStat->st_mode = (srb_mode_t) (statbuf->st_mode);
    retStat->st_nlink = (short) (statbuf->st_nlink);
    retStat->st_uid = (srb_uid_t) (statbuf->st_uid);
    retStat->st_gid = (srb_gid_t) (statbuf->st_gid);
    retStat->st_rdev = (srb_dev_t) (statbuf->st_rdev);
    retStat->st_size = (srb_size_t) (statbuf->st_size);
    retStat->st_atim = (srb_time_t) (statbuf->st_atime);
    retStat->st_mtim = (srb_time_t) (statbuf->st_mtime);
    retStat->st_ctim = (srb_time_t) (statbuf->st_ctime);
#ifndef _WIN32
    retStat->st_blksize = (srb_size_t) (statbuf->st_blksize);
    retStat->st_blocks = (srb_size_t) (statbuf->st_blocks);
#endif
#if defined(PORTNAME_aix) || defined(PORTNAME_alpha) || defined(PORTNAME_sunos) || defined(PORTNAME_c90)
    retStat->st_spare1 = (int) (statbuf->st_spare1);
    retStat->st_spare2 = (int) (statbuf->st_spare2);
    retStat->st_spare3 = (int) (statbuf->st_spare3);
#endif  /* PORTNAME_aix */
#ifdef PORTNAME_aix
    retStat->st_vfstype = (int) (statbuf->st_vfstype);
    retStat->st_vfs = (srb_size_t) (statbuf->st_vfs);
    retStat->st_type = (srb_size_t) (statbuf->st_type);
    retStat->st_Access = (srb_ushort_t) (statbuf->st_access);
    retStat->st_flag = (srb_ushort_t) (statbuf->st_flag);
#endif	/* PORTNAME_aix */
#if defined(PORTNAME_aix) || defined(PORTNAME_alpha)
    retStat->st_gen = (srb_size_t) (statbuf->st_gen);
#endif  /* defined(PORTNAME_aix) || defined(PORTNAME_alpha) */
    return (0);
}

/*
 * svrFileMkdir - create a new directory
 *
 * Input - int storSysType - Storage system type.
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP.
 *         struct varlena *hostAddr - The Host address of the storage.
 *         struct varlena *filename - The Path name of the new directory. 
 *	   Relative path =>  Storage Vault.
 *         int mode - Same definition as in Unix.
 *
 * Output - Returns 0 upon success.
 *          Returns -1 upon failure.
 */

int
svrFileMkdir(int storSysType, struct varlena *hostAddr, 
struct varlena *filename, int mode, int chdirFlag)
{
    int status = 0;
    char *fileNameBuf, *hostAddrBuf;
 
    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    fileNameBuf = varToStr (filename);
    hostAddrBuf = varToStr (hostAddr);
 
    status = _svrFileMkdir (storSysType, hostAddrBuf, fileNameBuf, 
      mode, chdirFlag);

    free (fileNameBuf);
    free (hostAddrBuf);

    return status;
}

int
_svrFileMkdir(int storSysType, char *hostAddr, char *fileName, 
int mode, int chdirFlag)
{
    int status;
    int remoteFlag;
#ifndef FED_MCAT
    char *portNum, tmpResLoc[MAX_TOKEN]; 
#endif
    struct hostElement  *hostTabPtr;

    /* Check whether the host is remote or local. Note: hostAddr now contains
     * only the host name */
 
#ifdef FED_MCAT
    remoteFlag = chkHostAndZone (hostAddr, &hostTabPtr);
#else
    /* Parse the hostAddr */
    strcpy (tmpResLoc, hostAddr);
    portNum = parseAddr (tmpResLoc);
    remoteFlag = chkHost (tmpResLoc, portNum, &hostTabPtr);
#endif
 
    if (remoteFlag < 0) {
        elog (NOTICE, "Invalid host addr %s", hostAddr);
        return INP_ERR_HOST_ADDR;
    } else if (remoteFlag == REMOTE_HOST) {
        status = remoteFileMkdir (storSysType, hostTabPtr, fileName, mode,
        chdirFlag, hostAddr);
    } else if (remoteFlag == LOCAL_HOST) {
        if (chdirFlag == DO_CHKDIR) {
            if ((status =
	      chkdir (storSysType, hostAddr, fileName, WRT_CHK, FILE_T)) < 0) {
                    return status;
            }
        } else if (storSysType == GRIDFTP_FT) {
            if (matchVault (storSysType, fileName) == NULL)
                VaultpathStatus = PATH_NOT_IN_VAULT;
            else
                VaultpathStatus = PATH_IN_VAULT;
        }
        status = exfmkdir(storSysType, hostAddr, fileName, mode);
    }

    return status;
} 

/*
 * svrFileChmod - Change the mode of a file or directory.
 *
 * Input - int storSysType - Storage system type.
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP.
 *         struct varlena *hostAddr - The Host address of the SRB.
 *         struct varlena *filename - The Path name of the file or directory.
 *         Relative path => Storage Vault.
 *         int mode - Same definition as in Unix.
 *
 * Output - Returns 0 upon success.
 *          Returns -1 upon failure.
 *
 */

int
svrFileChmod(int storSysType, struct varlena *hostAddr, 
struct varlena *filename, int mode, int chdirFlag)
{
    int status = 0;
    char *fileNameBuf, *hostAddrBuf;
 
    if (TicketUserFlag || ProxyUser->privUserFlag == 0) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    fileNameBuf = varToStr (filename);
    hostAddrBuf = varToStr (hostAddr);
 
    status = _svrFileChmod(storSysType, hostAddrBuf, fileNameBuf, 
      mode, chdirFlag);
 
    free (fileNameBuf);
    free (hostAddrBuf);

    return status;
}

int
_svrFileChmod (int storSysType, char *hostAddr, char *fileName, 
int mode, int chdirFlag)
{
    int status;
    int remoteFlag;
#ifndef FED_MCAT
    char *portNum, tmpResLoc[MAX_TOKEN];
#endif
    struct hostElement  *hostTabPtr;

    /* Check whether the host is remote or local. Note: hostAddr now contains
     * only the host name */
 
#ifdef FED_MCAT
    remoteFlag = chkHostAndZone (hostAddr, &hostTabPtr);
#else
    /* Parse the hostAddr */
    strcpy (tmpResLoc, hostAddr);
    portNum = parseAddr (tmpResLoc);
    remoteFlag = chkHost (tmpResLoc, portNum, &hostTabPtr);
#endif
 
    if (remoteFlag < 0) {
        elog (NOTICE, "Invalid host addr %s", hostAddr);
        return INP_ERR_HOST_ADDR;
    } else if (remoteFlag == REMOTE_HOST) {
        status = remoteFileChmod (storSysType, hostTabPtr, fileName,
        mode, chdirFlag);
    } else if (remoteFlag == LOCAL_HOST) {
        if (chdirFlag == DO_CHKDIR) {
            if ((status =
	      chkdir (storSysType, hostAddr, fileName, WRT_CHK, FILE_T)) < 0) {
                    return status;
            }
	}
        status = exfchmod(storSysType, hostAddr, fileName, mode);
    }
 
    return status;
}

/*
 * svrFileRmdir - Remove a directory.
 *
 * Input - int storSysType - Storage system type.
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP.
 *         struct varlena *hostAddr - The Host address of the SRB.
 *         struct varlena *filename - The Path name of the directory to remove.
 *         Relative path => Storage Vault.
 *
 * Output - Returns 0 upon success.
 *          Returns -1 upon failure.
 */

int
svrFileRmdir(int storSysType, struct varlena *hostAddr, 
struct varlena *filename, int chdirFlag)
{
    int status = 0;
    char *fileNameBuf, *hostAddrBuf;
 
    if (TicketUserFlag || ProxyUser->privUserFlag == 0) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    fileNameBuf = varToStr (filename);
    hostAddrBuf = varToStr (hostAddr);
 
    status = _svrFileRmdir (storSysType, hostAddrBuf, fileNameBuf, chdirFlag);
 
    free (fileNameBuf);
    free (hostAddrBuf);

    return status;
}

int
_svrFileRmdir(int storSysType, char *hostAddr, char *fileName, int chdirFlag)
{
    int status;
    int remoteFlag;
#ifndef FED_MCAT
    char *portNum, tmpResLoc[MAX_TOKEN];
#endif
    struct hostElement  *hostTabPtr;

    /* Check whether the host is remote or local. Note: hostAddr now contains
     * only the host name */
 
#ifdef FED_MCAT
    remoteFlag = chkHostAndZone (hostAddr, &hostTabPtr);
#else
    /* Parse the hostAddr */
    strcpy (tmpResLoc, hostAddr);
    portNum = parseAddr (tmpResLoc);
    remoteFlag = chkHost (tmpResLoc, portNum, &hostTabPtr);
#endif
 
    if (remoteFlag < 0) {
        elog (NOTICE, "Invalid host addr %s", hostAddr);
        return INP_ERR_HOST_ADDR;
    } else if (remoteFlag == REMOTE_HOST) {
        status = remoteFileRmdir (storSysType, hostTabPtr, fileName,
        chdirFlag, hostAddr);
    } else if (remoteFlag == LOCAL_HOST) {
        if (chdirFlag == DO_CHKDIR) {
            if ((status =
	      chkdir (storSysType, hostAddr, fileName, WRT_CHK, DIR_T)) < 0) {
                    return status;
            }
        } else if (storSysType == GRIDFTP_FT) {
            if (matchVault (storSysType, fileName) == NULL)
                VaultpathStatus = PATH_NOT_IN_VAULT;
            else
                VaultpathStatus = PATH_IN_VAULT;
        }

        status = exfrmdir(storSysType, hostAddr, fileName);
    }
 
    return status;
}

/*
 * svrVaultInfo()
 * Get Info on the SRB storage vault.
 *
 * Input : None
 *
 * Output: A list of struct vaultQueElement in varlena format
 *
 *
 */

struct varlena *
svrVaultInfo()
{
    struct varlena *retval;
    int vaultCount = 0;
    int vaultInfoLen = 0;
    struct vaultQueElement *tmpElement;
    char *cptr;
    int i;
#if defined(PORTNAME_c90)
    struct int32Array *iptr;
#else
    int *iptr;
#endif
 
    /* count the number of vault and the length in bytes needed to store
     * the vault data.
     */
    tmpElement = vaultQueHead;
    while (tmpElement != NULL) {
	vaultCount ++;
	vaultInfoLen += strlen (tmpElement->vaultPath);
	tmpElement = tmpElement->next;
    }

    /* add 5 bytes for each vaultCount - 4 to accumodate the storSysType,
     * 1 byte for NULL following each vaultPath. Add 4 bytes to include a 
     * vaultCount.
     */

    vaultInfoLen += vaultCount * (1 + SIZEOF32) + SIZEOF32;
    retval = (struct varlena *)malloc(VAROUTHDRSZ + vaultInfoLen);
 
    /* Fill in the data */

    i = 1;
#if defined(PORTNAME_c90)
    iptr = (struct int32Array *) retval;
    iptr[i].myInt0 = htonl (vaultCount);
#else
    iptr = (int *) VARDATA(retval);
    iptr[i] = htonl (vaultCount);
#endif
    cptr = (char *) VAROUTDATA(retval) + (vaultCount + 1) * SIZEOF32;

    VAROUTSTAT(retval) = 0;		/* status */

    tmpElement = vaultQueHead;
    while (tmpElement != NULL) {
	i++;
#if defined(PORTNAME_c90)
	if (i - i / 2 * 2)
	    iptr[i/2 + 1].myInt0 = htonl (tmpElement->storSysType);   /* odd */
	else
	    iptr[i/2].myInt1 = htonl (tmpElement->storSysType);
#else
	iptr[i] = htonl (tmpElement->storSysType);
#endif
	strcpy (cptr, tmpElement->vaultPath);
	cptr += strlen (tmpElement->vaultPath);
	*cptr = '\0';
	cptr++;
	tmpElement = tmpElement->next;
    }
    VARSIZE(retval) = vaultInfoLen + SIZEOF32 * 2;
	
    return retval;
}

struct varlena *
svrGetVersion ()
{
    struct varlena *retval;
    int status;
    int len;
    char *srbVersion;

    srbVersion = getenv("srbVersion");
    if (srbVersion == NULL) {
	len = 0;
	status = CLI_INCOMPATIBLE_VERSION;
        retval = (struct varlena *)malloc(VAROUTHDRSZ + len + 2);
	VARSIZE(retval) = len + VAROUTHDRSZ;
	VAROUTSTAT(retval) = htonl (status);
	return retval;
    }

    len = strlen (SRB_VERSION) + 1;
    retval = (struct varlena *)malloc(VAROUTHDRSZ + len + 2);
    strcpy (VAROUTDATA(retval), SRB_VERSION);
    status = 0;

    VARSIZE(retval) = len + VAROUTHDRSZ;
    VAROUTSTAT(retval) = htonl (status);

    return retval;
}

/*
 * svrOpendir - Opens a directory
 *
 * Input - int storSysType - Storage system type.
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP.
 *         struct varlena *hostAddr - The Host address of the SRB.
 *         varlena *dirname - The file Path name to open. Relative path =>
 *              Storage Vault.
 * Output - Returns the file descriptor for use in later srbFile* calls.
 *          Returns -1 upon failure.
 */
 
int
svrOpendir(int storSysType, struct varlena *hostAddr, 
struct varlena *dirname, int chdirFlag)
{
    int status;
    char *dirNameBuf, *hostAddrBuf;

    if (TicketUserFlag || ProxyUser->privUserFlag == 0) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    dirNameBuf = varToStr (dirname);
    hostAddrBuf = varToStr (hostAddr);

    status = _svrOpendir (storSysType, hostAddrBuf, dirNameBuf,
      chdirFlag);

    free (hostAddrBuf);
    free (dirNameBuf);
 
    return status;
}

int
_svrOpendir(int storSysType, char *hostAddr, char *dirName, int chdirFlag)
{
    int retval;
    int fd;
    int i;
    int remoteFlag;
#ifndef FED_MCAT
    char *portNum, tmpResLoc[MAX_TOKEN];
#endif
    struct hostElement  *hostTabPtr;

    /* Check whether the host is remote or local. Note: hostAddr now contains
     * only the host name */
 
#ifdef FED_MCAT
    remoteFlag = chkHostAndZone (hostAddr, &hostTabPtr);
#else
    /* Parse the hostAddr */
    strcpy (tmpResLoc, hostAddr);
    portNum = parseAddr (tmpResLoc);
    remoteFlag = chkHost (tmpResLoc, portNum, &hostTabPtr);
#endif
 
    if (remoteFlag < 0) {
        elog (NOTICE, "Invalid host addr %s", hostAddr);
        return INP_ERR_HOST_ADDR;
    } else if (remoteFlag == REMOTE_HOST) {
        fd = remoteOpendir (storSysType, hostTabPtr, dirName,
        chdirFlag);

    	if (fd < 0) {
            return fd;
    	}
 
        i = newExfFd(storSysType, hostAddr, hostTabPtr->portNum, hostTabPtr,
          dirName, fd, remoteFlag, NULL);

    } else if (remoteFlag == LOCAL_HOST) {
    	i = newExfFd(storSysType, hostAddr, hostTabPtr->portNum, hostTabPtr, 
	  dirName, 9999, remoteFlag, NULL); /* 9999 is an arbitrary filler */
        retval = exfopendir(storSysType, hostAddr, dirName,
          &exfDesc[i].storSysDepend);
 
    	if (retval < 0) {
            deleteExfFd (i);
            return retval;
	}
    }

    return i;
}

/*
 * svrClosedir - close a directory 
 *
 * Input - int fd_inx - The file descriptor to close (from svrOpendir) 
 *
 * Output - Returns 0 upon success.
 *          Returns -1 upon failure.
 */
 
int
svrClosedir(int fd_inx)
{
 
    int retval;
    srbConn* conn;
 
    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    if (fd_inx < 0 || fd_inx >= MAX_EXF_FDS || exfDesc[fd_inx].fd < 0) {
        elog(NOTICE,"svrFileClose: ext file desc (%d) out of range", fd_inx);
        return INP_ERR_DESC;
    }

    if (exfDesc[fd_inx].remoteFlag == REMOTE_HOST) {
        conn = getRemoteConn (exfDesc[fd_inx].hostTabPtr);
        if (conn == NULL)
            return SYS_ERR_REMOTE_CONN;
        retval = srbClosedir (conn, exfDesc[fd_inx].fd);
    } else {
        retval = exfclosedir (exfDesc[fd_inx].storSysType, 
	  exfDesc[fd_inx].hostAddr, exfDesc[fd_inx].storSysDepend);
    }
    deleteExfFd (fd_inx);
    return retval;
}

/*
 * svrReaddir - Read the next directory entry.
 *
 * Input - int fd_inx - The file descriptor to read (from srbOpendir)
 *
 * Output - Returns struct varlena * containing the srbDirent read.
 */
 
struct varlena *
svrReaddir(int fd_inx)
{
    struct varlena *retval;
    struct srbDirent srbDirent, *tmpDirent;
    int status;
    srbConn* conn;
 
    if (fd_inx < 0 || fd_inx >= MAX_EXF_FDS || exfDesc[fd_inx].fd < 0) {
        elog(NOTICE,"svrFileClose: ext file desc (%d) out of range", fd_inx);
        status = INP_ERR_DESC;
        retval = (struct varlena *)malloc(VAROUTHDRSZ);
        VARSIZE(retval) = VAROUTHDRSZ;
        VAROUTSTAT(retval) = htonl (status);
        return retval;
    }
 
    if (exfDesc[fd_inx].remoteFlag == REMOTE_HOST) {
        conn = getRemoteConn (exfDesc[fd_inx].hostTabPtr);
        if (conn == NULL) {
            status = -1;
            retval = (struct varlena *)malloc(VAROUTHDRSZ);
            VARSIZE(retval) = VAROUTHDRSZ;
            VAROUTSTAT(retval) = htonl (status);
	    return retval;
	}
        tmpDirent = srbReaddir (conn, exfDesc[fd_inx].fd);
	if (tmpDirent != NULL) {
    	    srbDirent = *tmpDirent;
	    status = 0;
	    free (tmpDirent);
	} else {
	    status = -1;
	}
    } else {
    	status = exfreaddir(exfDesc[fd_inx].storSysType, 
	  exfDesc[fd_inx].hostAddr, exfDesc[fd_inx].storSysDepend, &srbDirent);
    }


    if (status < 0) {		/* error or reached the end */
    	retval = (struct varlena *)malloc(VAROUTHDRSZ);
    	VARSIZE(retval) = VAROUTHDRSZ;
	VAROUTSTAT(retval) = htonl (status);
    } else {
    	retval = (struct varlena *)malloc(VAROUTHDRSZ + sizeof(srbDirent));
    	VARSIZE(retval) = sizeof(srbDirent) + VAROUTHDRSZ;
	memcpy (VAROUTDATA(retval), (char *) &srbDirent, 
	  sizeof(struct srbDirent));
    	VAROUTSTAT(retval) = 0;
    }
    return retval;
}

/*
 * svrSetStorAttri - Set Attributes for a Storage System; establish connection.
 *   Currently only defined for FTP files, connects to an FTP Server and/or
 *   sets the current directory.
 *
 * Input - int storSysType - Storage system type. 
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP.
 *         struct varlena *hostAddr - host address for the storage system,
 *                for FTP this is host address and an optional :port.
 *         struct varlena *userAuth - user authorization string, for FTP
 *                this is username to login as and optional :password.
 *         struct varlena *dirPath - directory to change to (cd)
 *                (dirPath or hostAddr may be null to leave unchanged)
 *
 * Output - Returns 0 upon success.
 *          Returns -1 upon failure.
 *
 */

int
svrSetStorAttri(int storSysType, 
            struct varlena *hostAddr, struct varlena *userAuth,
            struct varlena *dirPath)
{
    int status = 0;
    char *userAuthBuf, *hostAddrBuf, *dirPathBuf;
 
    userAuthBuf = varToStr (userAuth);
    hostAddrBuf  = varToStr (hostAddr);
    dirPathBuf  = varToStr (dirPath);

    status = _svrSetStorAttri(storSysType, hostAddrBuf, userAuthBuf,
             dirPathBuf);
 
    free (userAuthBuf);
    free (hostAddrBuf);
    free (dirPathBuf);

    return status;
}

int
svrFileMigrate(int fd_inx, int srcLevel, int flags)
{
    int status = 0;
    srbConn* conn;

    if (fd_inx < 0 || fd_inx >= MAX_EXF_FDS || exfDesc[fd_inx].fd < 0) {
        elog(NOTICE,"svrFileMigrate: ext file desc (%d) out of range", fd_inx);
        return INP_ERR_DESC;
    }

    if (exfDesc[fd_inx].remoteFlag == REMOTE_HOST) {
        if (exfDesc[fd_inx].conn != NULL) {
            conn = exfDesc[fd_inx].conn;
        } else {
            conn = getRemoteConn (exfDesc[fd_inx].hostTabPtr);
            if (conn == NULL)
                return SYS_ERR_REMOTE_CONN;
	}
#ifdef HPSS_PRIV        /* required HPSS root user priv */
        status = srbFileMigrate (conn, exfDesc[fd_inx].fd, srcLevel, flags);
#endif 	/* HPSS_PRIV */
    } else {
        status = exfmigrate (exfDesc[fd_inx].storSysType,
          exfDesc[fd_inx].hostAddr, exfDesc[fd_inx].fd, srcLevel, flags);
    }

    return status;
}

int
svrFileStage(int fd_inx, int offset, int len, int srcLevel, int flags)
{
    int status = 0;
    srbConn* conn;

    if (fd_inx < 0 || fd_inx >= MAX_EXF_FDS || exfDesc[fd_inx].fd < 0) {
        elog(NOTICE,"svrFileStage: ext file desc (%d) out of range", fd_inx);
        return INP_ERR_DESC;
    }

    if (exfDesc[fd_inx].remoteFlag == REMOTE_HOST) {
        if (exfDesc[fd_inx].conn != NULL) {
            conn = exfDesc[fd_inx].conn;
        } else {
            conn = getRemoteConn (exfDesc[fd_inx].hostTabPtr);
            if (conn == NULL)
                return SYS_ERR_REMOTE_CONN;
	}
        status = srbFileStage (conn, exfDesc[fd_inx].fd, offset, len,
	 srcLevel, flags);
    } else {
        status = exfstage (exfDesc[fd_inx].storSysType,
         exfDesc[fd_inx].hostAddr, exfDesc[fd_inx].fd, offset, len,
	  srcLevel, flags);
    }

    return status;
}

int
svrFilePurge(int fd_inx, int offset, int len, int srcLevel, int flags)
{
    int status = 0;
    srbConn* conn;

    if (fd_inx < 0 || fd_inx >= MAX_EXF_FDS || exfDesc[fd_inx].fd < 0) {
        elog(NOTICE,"svrFilePurge: ext file desc (%d) out of range", fd_inx);
        return INP_ERR_DESC;
    }

    if (exfDesc[fd_inx].remoteFlag == REMOTE_HOST) {
        if (exfDesc[fd_inx].conn != NULL) {
            conn = exfDesc[fd_inx].conn;
        } else {
            conn = getRemoteConn (exfDesc[fd_inx].hostTabPtr);
            if (conn == NULL)
                return SYS_ERR_REMOTE_CONN;
	}
#ifdef HPSS_PRIV        /* required HPSS root user priv */
        status = srbFilePurge (conn, exfDesc[fd_inx].fd, offset, len,
         srcLevel, flags);
#endif	/* HPSS_PRIV */
    } else {
        status = exfpurge (exfDesc[fd_inx].storSysType,
         exfDesc[fd_inx].hostAddr, exfDesc[fd_inx].fd, offset, len,
          srcLevel, flags);
    }

    return status;
}

int
_svrSetStorAttri (int storSysType, char *hostAddr, char *userAuth,
                  char *dirPath)
{
    int status;

    status = exfsetstorattri(storSysType, hostAddr, userAuth, dirPath);
 
    return status;
}

srb_long_t 
_fileWriteSize (int fd_inx)
{
    int status;
    struct srbStat retStat;

    if (fd_inx < 0 || (fd_inx >= MAX_EXF_FDS) || (exfDesc[fd_inx].fd < 0)) {
        elog(NOTICE,"_fileWriteSize: ext file desc (%d) out of range", fd_inx);
        return INP_ERR_DESC;
    }

    if (exfDesc[fd_inx].writeFlag <= 0)	    /* have not been written into */
	return 0;

    if ((status = _svrFileFstat (exfDesc[fd_inx].storSysType, 
      exfDesc[fd_inx].hostAddr, exfDesc[fd_inx].portNum,
      fd_inx, &retStat)) < 0) {
	elog(NOTICE,"_fileWriteSize: _svrFileStat failed. status = %d",
	  status);
	return status;
    }

    return retStat.st_size;
}
    
srb_long_t
_getFileSize (int fd_inx)
{
    int status;
    struct srbStat retStat;

    if (fd_inx < 0 || (fd_inx >= MAX_EXF_FDS) || (exfDesc[fd_inx].fd < 0)) {
        elog(NOTICE,"getFileSize: ext file desc (%d) out of range", fd_inx);
        return INP_ERR_DESC;
    }

    if ((status = _svrFileStat (exfDesc[fd_inx].storSysType,
      exfDesc[fd_inx].hostAddr, exfDesc[fd_inx].portNum,
      exfDesc[fd_inx].filename, &retStat)) < 0) {
        elog(NOTICE,"_getFileSize: _svrFileStat failed. status = %d",
          status);
        return status;
    }

    return retStat.st_size;
}

srb_long_t 
_getFileSizeByPath (int storSysType, char *hostAddr, char *filename)
{
    int status;
    struct srbStat retStat;

    if ((status = _svrFileStat (storSysType, hostAddr, NULL, filename, 
     &retStat)) < 0) {
        elog (NOTICE,
	 "_getFileSizeByPath: _svrFileStat failed for %s. status = %d",
          filename, status);
        return status;
    }
    return retStat.st_size;
}

int
_fileWriteSync (int fd_inx)
{
    int status;

    if (fd_inx < 0 || (fd_inx >= MAX_EXF_FDS) || (exfDesc[fd_inx].fd < 0)) {
        elog(NOTICE,"_fileWriteSize: ext file desc (%d) out of range", fd_inx);
        return INP_ERR_DESC;
    }

    if (exfDesc[fd_inx].writeFlag <= 0)     /* have not been written into */
        return 0;

    if ((status = svrFileSync (fd_inx)) < 0) {
        elog(NOTICE,"_fileWriteSync: svrFileSync failed. status = %d",
          status);
        return status;
    }

    return status;
}

struct varlena *
svrFileChksum (int fd_inx, int flag, srb_long_t *offset, srb_long_t *size)
{
    struct varlena *retval;
    int status;
    srb_long_t myOffset, mySize;

    myOffset = *offset;
    mySize = *size;

    retval = (struct varlena *)malloc(VAROUTHDRSZ + CHKSUM_STR_LEN + 1);


    status = _svrFileChksum (fd_inx, flag, 
    VAROUTDATA(retval), myOffset, mySize);

    if (status >= 0)
	VARSIZE(retval) = CHKSUM_STR_LEN + VAROUTHDRSZ;
    else
	VARSIZE(retval) = VAROUTHDRSZ;

    VAROUTSTAT(retval) = htonl (status);

    return retval;
}
     
int
_svrFileChksum (int fd_inx, int flag, char *chksum, 
srb_long_t myOffset, srb_long_t mySize)
{
    int remoteFlag;
    struct hostElement  *hostTabPtr;
    int status;
    unsigned long ichksum;

    if (fd_inx < 0 || fd_inx >= MAX_EXF_FDS || exfDesc[fd_inx].fd < 0) {
        elog(NOTICE,"_svrFileChksum: ext file desc (%d) out of range", fd_inx);
        return INP_ERR_DESC;
    }

    remoteFlag = exfDesc[fd_inx].remoteFlag;
    hostTabPtr = exfDesc[fd_inx].hostTabPtr;

    if (remoteFlag == REMOTE_HOST) {
	status = remoteFileChksum (hostTabPtr, fd_inx, flag, &ichksum, 
	 myOffset, mySize);
    } else {
	status = svrFileSum (fd_inx, &ichksum,
	 myOffset, mySize);
    }  
    if (status >= 0)
        sprintf (chksum, "%d", ichksum);


    return (status);
}

/* svrFileSum - This is the server equivalent of clFileSum.
 * if mySize <= 0, read to eof
 */

int
svrFileSum (int fd_inx, unsigned long *ichksum, 
srb_long_t myOffset, srb_long_t mySize)
{
    int fd;
    unsigned char *buf;
    register int bytes_read, toRead;
    register unsigned long checksum = 0;
    srb_long_t status, remainingLen;
    int storSysType;
    char *hostAddr, *fileName;

    storSysType = exfDesc[fd_inx].storSysType;
    hostAddr = exfDesc[fd_inx].hostAddr;
    fileName = exfDesc[fd_inx].filename;
    fd = exfDesc[fd_inx].fd;

    status = exfseek(storSysType, hostAddr, fd, myOffset, SEEK_SET);
    if (status < 0) {
	fd = status;
        elog (NOTICE, "svrFileSum: Error seeking file %s, stat = %d",
         fileName, fd);
        *ichksum = 0;
        return (fd);
    }

    buf = malloc (READ_SIZE);

    /* Need binary I/O, or else byte counts and checksums are incorrect.  */
    SET_BINARY (fd);

    if (mySize > 0) {
        remainingLen = mySize;
	if (remainingLen >= READ_SIZE) 
	    toRead = READ_SIZE;
	else
	    toRead = remainingLen;
    } else {
	toRead = READ_SIZE;
    }

    while ((bytes_read = exfread (storSysType, hostAddr, fd, (char *) buf, 
     toRead)) > 0) {
        checksum += chksumBuf (bytes_read, buf);
        if (mySize > 0) {
	    remainingLen -= bytes_read;
	    if (remainingLen <= 0)
		break;
            if (remainingLen >= READ_SIZE) 
                toRead = READ_SIZE;
            else
                toRead = remainingLen;
        }
    }

    free (buf);

    if (bytes_read < 0) {
        elog (NOTICE, "svrFileSum: Error in reading file %s, stat = %d", 
	  fileName, bytes_read);
        /* exfclose (storSysType, hostAddr, fd); done by calling routine */
        *ichksum = 0;
        return (bytes_read);
    }


    *ichksum = checksum % 0xffff;
    return 0;
}

srb_long_t *
svrGetFsFreeSpace (int storSysType, struct varlena *hostAddr,
struct varlena  *vPath, int flag)
{
    char *path, *hostAddrBuf;
    srb_long_t *retVal;

    path = varToStr (vPath);
    hostAddrBuf = varToStr (hostAddr);

    retVal = (srb_long_t *) malloc (2*SIZEOF32);

    *retVal = _svrGetFsFreeSpace (storSysType, hostAddrBuf, path, flag);

    return (retVal);
}

srb_long_t
_svrGetFsFreeSpace (int storSysType, char *hostAddr, char *path, int flag)
{
    int remoteFlag;
#ifndef FED_MCAT
    char *portNum, tmpResLoc[MAX_TOKEN];
#endif
    struct hostElement  *hostTabPtr;
    srb_long_t status;


    /* Check whether the host is remote or local. Note: hostAddr now
     * contains only the host name */

#ifdef FED_MCAT
    remoteFlag = chkHostAndZone (hostAddr, &hostTabPtr);
#else
    /* Parse the hostAddr */
    strcpy (tmpResLoc, hostAddr);
    portNum = parseAddr (tmpResLoc);
    remoteFlag = chkHost (tmpResLoc, portNum, &hostTabPtr);
#endif

    if (remoteFlag < 0) {
        elog (NOTICE, "Invalid host addr %s", hostAddr);
        return INP_ERR_HOST_ADDR;
    } else if (remoteFlag == REMOTE_HOST) {
	status = remoteGetFsFreeSpace (storSysType, hostTabPtr, path, flag);
    } else if (remoteFlag == LOCAL_HOST) {
	    char tmpPath[MAX_TOKEN];
	    char *cptr, *lastSl;

	    strcpy (tmpPath, path);
            cptr = lastSl = tmpPath;

            while (*cptr != '\0') {
                if (*cptr == '/') {
                    lastSl = cptr;
                } else if (*cptr == '?') {
                    *lastSl = '\0';
                    break;
                }
                cptr ++;
            }

	status = exfgetFsFreeSpace (storSysType, hostAddr, tmpPath, flag);
    }
    return (status);
}

int
svrFileFStage(int storSysType, struct varlena *hostAddr, 
struct varlena *filename, int flags)
{
    int status;
    char *fileNameBuf, *hostAddrBuf;

    /* the proxy user is not a privileged user */

    if (ProxyUser->privUserFlag == 0) {
	elog (NOTICE, "svrFileStage: Illegal operation for %s@%s",
	 ProxyUser->userName, ProxyUser->domainName);
        return AUTH_ERR_PROXY_NOPRIV;
    }

    fileNameBuf = varToStr (filename);
    hostAddrBuf = varToStr (hostAddr);

    status = _svrFileFStage(storSysType, hostAddrBuf, fileNameBuf, flags); 

    free (fileNameBuf);
    free (hostAddrBuf);

    return status;
}

/*
 * _svrFileFStage - Stages an external file
 */

int
_svrFileFStage(int storSysType, char *hostAddr, char *fileName, 
int flags)
{
    int remoteFlag;
#ifndef FED_MCAT
    char *portNum, tmpResLoc[MAX_TOKEN];
#endif
    struct hostElement  *hostTabPtr;
    int status;

    /* Check whether the host is remote or local. Note: hostAddr now 
     * contains only the host name */

#ifdef FED_MCAT
    remoteFlag = chkHostAndZone (hostAddr, &hostTabPtr);
#else
    /* Parse the hostAddr */
    strcpy (tmpResLoc, hostAddr);
    portNum = parseAddr (tmpResLoc);
    remoteFlag = chkHost (tmpResLoc, portNum, &hostTabPtr);
#endif

    if (remoteFlag < 0) {
	elog (NOTICE, "Invalid host addr %s", hostAddr); 
	return INP_ERR_HOST_ADDR;
    } else if (remoteFlag == REMOTE_HOST) {
	status = remoteFileFStage (storSysType, hostTabPtr, fileName, flags);
    } else if (remoteFlag == LOCAL_HOST) {
    	status = exffstage(storSysType, hostAddr, fileName, flags);
    }

    return status;
}

/*
 * svrFileChown - Change the group of a file or directory. Change user which
 * requires root privilege, is now working at this time.
 *
 * Input - int storSysType - Storage system type.
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP.
 *         struct varlena *hostAddr - The Host address of the SRB.
 *         struct varlena *filename - The Path name of the file or directory.
 *         Relative path => Storage Vault.
 *	    struct varlena *fileUser - not used
 *	    struct varlena *fileGroup - the group of the file to change to
 *         int chownFlag - Not used.
 *
 * Output - Returns 0 upon success.
 *          Returns -1 upon failure.
 *
 */

int
svrFileChown (int storSysType, struct varlena *hostAddr, 
struct varlena *filename, struct varlena *fileUser, struct varlena *fileGroup,
int chownFlag)
{
    int status = 0;
    char *fileNameBuf, *hostAddrBuf;
    char *fileUserBuf, *fileGrouBuf;
 
    if (TicketUserFlag || ProxyUser->privUserFlag == 0) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    fileNameBuf = varToStr (filename);
    hostAddrBuf = varToStr (hostAddr);
    fileGrouBuf = varToStr (fileGroup);
 
    status = _svrFileChown(storSysType, hostAddrBuf, fileNameBuf, 
      NULL, fileGrouBuf, chownFlag);
 
    free (fileNameBuf);
    free (hostAddrBuf);
    free (fileGrouBuf);

    return status;
}

int
_svrFileChown (int storSysType, char *hostAddr, char *fileName, 
char *fileUser, char *fileGroup, int chownFlag)
{
    int status;
    int remoteFlag;
#ifndef FED_MCAT
    char *portNum, tmpResLoc[MAX_TOKEN];
#endif
    struct hostElement  *hostTabPtr;

    /* Check whether the host is remote or local. Note: hostAddr now contains
     * only the host name */
 
#ifdef FED_MCAT
    remoteFlag = chkHostAndZone (hostAddr, &hostTabPtr);
#else
    /* Parse the hostAddr */
    strcpy (tmpResLoc, hostAddr);
    portNum = parseAddr (tmpResLoc);
    remoteFlag = chkHost (tmpResLoc, portNum, &hostTabPtr);
#endif
 
    if (remoteFlag < 0) {
        elog (NOTICE, "Invalid host addr %s", hostAddr);
        return INP_ERR_HOST_ADDR;
    } else if (remoteFlag == REMOTE_HOST) {
        status = remoteFileChown (storSysType, hostTabPtr, fileName,
       fileUser, fileGroup, chownFlag);
    } else if (remoteFlag == LOCAL_HOST) {
        status = exfchown(storSysType, hostAddr, fileName, 
	 fileUser, fileGroup, chownFlag);
    }
 
    return status;
}

/*
 * _svrFileChownByInx - _svrFileChown by fd_inx
 *
 * Input - int fd_inx - The file descriptor of opened file
 *
 * Output - Returns upon success
 *          Returns -1 upon failure.
 */

int _svrFileChownByInx (int fd_inx, char *fileUser, char *fileGroup, 
int chownFlag)
{
    srbConn* conn;
    int status;

    if (fd_inx < 0 || (fd_inx >= MAX_EXF_FDS) || (exfDesc[fd_inx].fd < 0)) {
	elog(NOTICE,"_svrFileChownByInx: ext file desc (%d) out of range", 
	fd_inx);
        return INP_ERR_DESC;
    }

    if (exfDesc[fd_inx].remoteFlag == REMOTE_HOST) {
        if (exfDesc[fd_inx].conn != NULL) {
            conn = exfDesc[fd_inx].conn;
        } else {
            conn = getRemoteConn (exfDesc[fd_inx].hostTabPtr);
            if (conn == NULL)
                return SYS_ERR_REMOTE_CONN;
        }

        status =  _srbFileChown (conn, exfDesc[fd_inx].storSysType, NULL, 
	exfDesc[fd_inx].filename, fileUser, fileGroup, chownFlag);
    } else {
    	status = exfchown(exfDesc[fd_inx].storSysType, exfDesc[fd_inx].hostAddr,
	 exfDesc[fd_inx].filename, fileUser, fileGroup, chownFlag);
    }
    return status;
}

int
_setFileWriteFlag (int fd_inx, int writeFlag)
{
    int status;
    struct srbStat retStat;

    if (fd_inx < 0 || (fd_inx >= MAX_EXF_FDS) || (exfDesc[fd_inx].fd < 0)) {
        elog(NOTICE,"_setFileWriteSize: ext file desc (%d) out of range",
          fd_inx);
        return INP_ERR_DESC;
    }

    exfDesc[fd_inx].writeFlag = writeFlag;
    return 0;
}

