/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
#include "srbC_httpc_library.h"


int 
srbC_httpc_open(char *dataPathName, int fileFlags, int fileMode) 
{    
    char *myHostAddr;
    char *fullPathName;
    int portNumber;
    int sockFd;
    char *tmpStr;
    int status, len;


    if ((portNumber = parseHttpPath (dataPathName, &myHostAddr, 
      &fullPathName)) < 0) {
	elog (NOTICE, "srbC_httpc_open: Bad HTTP PathName: %s", dataPathName);
	return (HTTP_ERR_BAD_PATHNAME);
    }
    if ((sockFd = TrytoConnectTcp(myHostAddr, portNumber)) < 0) { 
	return(sockFd); 
    }

    free (myHostAddr);

    len = strlen (fullPathName) + 20;
    tmpStr = malloc (len);
    sprintf(tmpStr, "GET %s\n", fullPathName);

    free (fullPathName);

#ifdef _WIN32
    status = send(sockFd, tmpStr, len, 0);
#else
    status = write (sockFd, tmpStr, len);
#endif

    if (status < 0) {
	elog (NOTICE, "srbC_httpc_open: Socket write error. Status = %d", 
	  status);
	return (status);
    }

    return sockFd;
}

int 
srbC_httpc_close(int fd)
{
    int status;

#ifdef _WIN32
    status = closesocket (fd);
#else
    status = close (fd);
#endif

    if (status < 0) {
	elog (NOTICE, "srbC_httpc_close: Close error. Status = %d", status);
    }
    return (status);
}

int 
srbC_httpc_read (int fd, char *buffer, int length)
{
    int status;

#ifdef _WIN32
    status = recv(fd,buffer,length,0);
#else
    status = read (fd, buffer, length);
#endif

    return (status);
}
  
int parseHttpPath (char *dataPathName, char **myHostAddr, char **fullPathName) 
{
    char *tmpPtr, *tmpPtr1; 
    char *myPortName;
    int portNumber;
    char *tmpPath;
    
    *fullPathName = malloc (strlen (dataPathName) + 20);
    myPortName = malloc (strlen (dataPathName) + 1);
    *myHostAddr = malloc (strlen (dataPathName) + 1);
    tmpPath = malloc (strlen (dataPathName) + 1);

    strcpy (tmpPath, dataPathName);

    if ((tmpPtr = strstr (tmpPath, URL_HEADER)) == NULL) {
        strcpy (*fullPathName, HTTP_HEADER);
    	strcat (*fullPathName, tmpPath);
	tmpPtr = tmpPath;
    } else {	/* copy the whole thing */
    	strcpy (*fullPathName, tmpPath);
	tmpPtr += strlen (URL_HEADER);
    }


    if ((tmpPtr1 = strstr(tmpPtr, "/")) != NULL) {	/* replace 1st slash
							 * with a NULL */
	*tmpPtr1 = '\0';
    } else {		/* Add a / at the end */
	strcat (*fullPathName, "/");
    }

    if (strstr(tmpPtr, ":") != NULL) {
    	if (mySplitStr(tmpPtr, *myHostAddr,myPortName, ':') == NULL) {
            elog (NOTICE, "parseHttpPath: Illegal HTTP pathName format: %s",
              tmpPtr);
	    free (*fullPathName);
	    free (myPortName);
	    free (*myHostAddr);
	    free (tmpPath);
            return HTTP_ERR_BAD_PATHNAME;
        }
    } else {
       	strcpy(*myHostAddr ,tmpPtr);
       	strcpy(myPortName,"");
    }
    if (strlen(myPortName) != 0) {
	portNumber = atoi(myPortName);
    } else {
	portNumber = DEF_HTTP_PORT;
    }
    free (myPortName);
    free (tmpPath);

    return (portNumber);
}

