/*
 * packetReceive -- receive a packet on a port.
 *
 * Input : Port *port - The Port struct
 *         PacketBuf *buf - Input buffer
 *         char nonBlocking - nonBlocking or Blocking.
 *
 * Output : returns: connection id of the packet sender.
 *
 */
int
packetReceive(Port *port,       /* receive port */
              PacketBuf *buf,   /* MAX_PACKET_SIZE-worth of buffer space */
              char nonBlocking) /* NON_BLOCKING or BLOCKING i/o */
{
    PacketLen   max_size = sizeof(PacketBuf);
    PacketLen   cc;             /* character count -- bytes recvd */
    PacketLen   packetLen;      /* remaining packet chars to read */
    Addr        tmp;            /* curr recv buf pointer */
#if defined(PORTNAME_aix)
    size_t         addrLen = sizeof(struct sockaddr_in);
#else
    int         addrLen = sizeof(struct sockaddr_in);
#endif
    int         hdrLen;
    int         flag;
    int         decr;

#if defined(PORTNAME_c90)
    hdrLen = 4;
#else
    hdrLen = sizeof(buf->len);
#endif

    if (nonBlocking == NON_BLOCKING) {
        flag = MSG_PEEK;
        decr = 0;
    } else {
        flag = 0;
        decr = hdrLen;
    }
    /*
     * port->nBytes is zero unless it is interrupted during
     * non-blocking I/O.  The first recvfrom() is to get the hdr
     * information so we know how many bytes to read.
     */
    tmp = ((Addr)buf) + port->nBytes;

    if (port->nBytes >= hdrLen)  {
        packetLen = ntohl(buf->len) - port->nBytes;
    }
     else {
        /* peeking into the incoming message */
#if defined(PORTNAME_c90)
        cc = recvfrom(port->sock, (char *)buf, hdrLen, flag,
#else
        cc = recvfrom(port->sock, (char *)&(buf->len), hdrLen, flag,
#endif
            (struct sockaddr*) &(port->raddr), &addrLen);
        if (cc < hdrLen) {
            if (cc < 0) {
                return(STATUS_ERROR);
            }
            /*
             * cc == 0 means the connection was broken at the other end.
             */
            else if (! cc) {
                return(STATUS_INVALID);

            } else {
                return(STATUS_NOT_DONE);
            }
        } else {
            /*
             * Got the header. now get the length (including header size).
             */
            packetLen = ntohl(buf->len);
           if (packetLen > max_size) {
               port->nBytes = packetLen;
               return(STATUS_BAD_PACKET);
           }
            packetLen -= decr;
            tmp += decr - port->nBytes;
        }
    }

    /*
     * Read the packet.
     */
    while (packetLen) {
        cc = recvfrom(port->sock, tmp, packetLen, 0,
                      (struct sockaddr*) &(port->raddr), &addrLen);
        if (cc < 0)
            return(STATUS_ERROR);
        /*
         * cc == 0 means the connection was broken at the other end.
         */
        else if (! cc)
            return(STATUS_INVALID);

        tmp += cc;
        packetLen -= cc;

        /* if non-blocking, we're done. */
        if (nonBlocking && packetLen) {
            port->nBytes += cc;
            return(STATUS_NOT_DONE);
        }
    }

    port->nBytes = 0;
    return(STATUS_OK);
}

/*
 * packetSend()
 *
 * packetSend -- send a single-packet message.
 *
 * Input : Port *port
 *         PacketBuf *buf - The Buffer to send.
 *         PacketLen len - The length of the Packet.
 *
 * Output : STATUS_ERROR if the write fails, STATUS_OK otherwise.
 *
 */
int
packetSend(Port *port, PacketBuf *buf, PacketLen len)
{
    PacketLen   totalLen;
    int         addrLen = sizeof(struct sockaddr_in);
    char *tmpBuf;

/*
#if defined(PORTNAME_c90)
    buf->msgtype = buf->msgtype | ((buf->len - sizeof (int)) << 32);
    tmpBuf = (char *) buf + sizeof (int);
    totalLen = len - sizeof (int);
#else
*/
    tmpBuf = (char *) buf;
    totalLen = len;
/*
#endif
*/

    len = sendto(port->sock, (Addr) tmpBuf, totalLen, 0,
        (struct sockaddr *)&(port->raddr), addrLen);

    if (len < totalLen) {
        return(STATUS_ERROR);
    }

    return(STATUS_OK);
}

/* initSrbHost - Initial the distributed SRB hosts. i.e., read the
 * MCAT_HOST file and initialize the distHost link list.
 *
 * Input - None
 *
 * Output - None
 */

int
initSrbHost ()
{
    FILE *file;
    char buf[MAX_LINE_SZ];
    char *portNum;
    char *conFile;
    int i;
    char *myHost, *myPort;
    struct hostElement *tmpHElement, *tmpHElement1;
    int remoteFlag;
    struct nameElement *tmpNElement;
    char *hostName;
    struct hostent *hostEnt;
    struct in_addr in;
    char *tmpName, *tmpName1, *tmpHost;
    int status;

    if (localHostHead != NULL)
        return (0);

    /* Initialize the local host */

    myHost = malloc (MAX_TOKEN);
    if (myHost == NULL) {
        elog (NOTICE, "initSrbHost: malloc error");
        return UNIX_ENOMEM;
    }

    if ((status = gethostname (myHost, MAX_TOKEN)) != 0) {
        elog (NOTICE, "initSrbHost: gethostname error");
        return status;
    }

    myPort = getenv("srbPort");

    tmpHElement = enqueHost (myHost, myPort, &localHostHead);
    /* Enqueue all the address too */
    if ((hostEnt = gethostbyname (myHost)) == NULL) {
        elog (NOTICE, "initSrbHost: gethostbyname error. errno = %d", errno);
        return (unixErrConv (errno));
    }
    tmpName = malloc (MAX_TOKEN);
    strcpy (tmpName, "localhost");
    enqueName (tmpName, tmpHElement);
    if (strcasecmp (myHost, hostEnt->h_name) != 0) {
        tmpName = malloc (strlen (hostEnt->h_name) + 1);
        strcpy (tmpName, hostEnt->h_name);
        enqueName (tmpName, tmpHElement);
    }
    i = 0;
    while (1) {
        if (hostEnt->h_addr_list[i] != NULL) {
            bcopy (hostEnt->h_addr_list[i], &in, hostEnt->h_length);
            tmpName1 = inet_ntoa (in);
            tmpName = malloc (strlen (tmpName1) + 1);
            strcpy (tmpName, tmpName1);
            enqueName (tmpName, tmpHElement);
            i++;
        } else {
            break;
        }
    }


    /* put together the full pathname for the config file */

    conFile = (char *) malloc((strlen(getSrbDataDir()) +
        strlen(HOST_CONFIG)+2)*sizeof(char));

    strcpy(conFile, getSrbDataDir());
    strcat(conFile, "/");
    strcat(conFile, HOST_CONFIG);

    file = fopen(conFile, "r");

    if (file == NULL) {
        if (DebugLvl > 1)
            fprintf(stderr, "Unable to open HOST_CONFIG file %s\n", conFile);
        free (conFile);
        return (0);
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


        portNum = parseAddr (buf);

        remoteFlag = chkHost (buf, NULL, &tmpHElement);
        if (remoteFlag < 0) {   /* no match */
            /* assume a remote host */
            myHost = strdup (buf);
            tmpHElement = enqueHost (myHost, portNum, &remoteHostHead);
        }

        if (portNum != NULL) {          /* change port number */
            tmpHElement->portNum = malloc (strlen (portNum) + 1);
            strcpy (tmpHElement->portNum, portNum);
        }

        while (i == 0) {        /* read to end o fline */
            i = getNextStr(file, buf, sizeof(buf));
            hostName = (char *) malloc(strlen (buf) + 1);
            strcpy (hostName, buf);
            parseAddr (hostName);
            remoteFlag = chkHost (hostName, NULL, &tmpHElement1);
            if (remoteFlag < 0) {       /* no match */
                enqueName (hostName, tmpHElement);
            } else if (remoteFlag == LOCAL_HOST) {
                if (tmpHElement1 == tmpHElement) {      /* both are local */
                    free (hostName);
                } else {        /* inconsistency */
                    /* Move all remote name to local. Don't have to
                     * remove from remote queue because localHostHead is
                     * checked first.
                     */
                    tmpNElement = tmpHElement->alias;
                    while (tmpNElement != NULL) {
                        enqueName (tmpNElement->hostName, tmpHElement1);
                        tmpNElement = tmpNElement->next;
                    }
                }
            } else {    /* remote host */
                free (hostName);
            }
        }
    }

    if (initHostWithMCat () < 0) {
        fprintf (stderr, "initHostWithMCat failed\n");
        return -1;
    }

    return 0;
}

/* initHostWithMCat - Initial the distributed SRB hosts data structure
 * using MCAT.
 *
 * Input - None
 *
 * Output - None
 */

int
initHostWithMCat ()
{
    FILE *file;
    char buf[MAX_LINE_SZ];
    char *mcatHost = NULL;
    char *mcatPort, *myMcatHost;
    char *mcatAuthScheme = NULL;
    char *mcatDn = NULL;
    int readState = RS_BEGIN_READ;
    char *hostName, *localHostName;
    char *conFile;
    int j;
    struct hostElement *tmpHElement;
    char *myHost, *myPort;
    struct in_addr in;
    struct hostent *hostEnt;
    mdasC_sql_result_struct  *myresult = NULL;
    char *phyResType;
    char *phyResLoc;
    char *phyResName;
    char *phyDirPathName;
    char *logDirPathName;
    char *tmpName, *tmpName1, *tmpHost;
    int tinx;
    char *cptr, *lastSl, *tmpPath;
    int remoteFlag;
    int status;
    char *tmp = NULL;

    /* Read the MCAT_HOST file for the MCAT enabled host name */

    conFile = (char *) malloc((strlen(getSrbDataDir()) +
        strlen(MCAT_HOST)+2)*sizeof(char));

    strcpy(conFile, getSrbDataDir());
    strcat(conFile, "/");
    strcat(conFile, MCAT_HOST);

    file = fopen(conFile, "r");

    if (file == NULL) {
        fprintf(stderr, "Unable to open MCAT_HOST file %s\n", conFile);
        free (conFile);
        return -1;
    }

    free (conFile);

    /* read the file */

    while ((i = getNextLine(file, buf, sizeof(buf))) != EOF) {

        if (mcatHost == NULL) {
            mcatHost = strdup (buf);
        } else if (mcatAuthScheme == NULL) {
            mcatAuthScheme = strdup (buf);
        } else if (mcatDn == NULL) {
            mcatDn = strdup (buf);
        }
    }

    fclose (file);

    if (mcatHost == NULL) {
        fprintf (stderr, "initHostWithMCat: mcatHost not defined in file%s\n",
         MCAT_HOST);
        return (SYS_ERR_NO_MCAT_HOST);
    }
    if (mcatDn == NULL)
        mcatDn = strdup ("");

    mcatPort = parseAddr (mcatHost);
    /* dwwhite - check for env var srbPort */
    if (mcatPort == NULL) {
        if ((tmp = getenv ("srbPort")) != NULL) {
            mcatPort = strdup (tmp);
        }
    }

    McatServerAuthInfo = (struct serverAuthInfo *) malloc
     (sizeof (struct serverAuthInfo));

    status = fillServerAuthInfo (mcatHost, mcatAuthScheme,
     mcatDn, McatServerAuthInfo);

    /* KESTOCK - mem leak patch */
    if (mcatAuthScheme != NULL)
        free (mcatAuthScheme);

    free (mcatDn);

    if (status < 0) {
        free (McatServerAuthInfo);
        McatServerAuthInfo = NULL;
        fprintf (stderr, "initHostWithMCat: fillServerAuthInfo error\n");
        return (SYS_ERR_NO_MCAT_HOST);
    }

    /* Is localhost MCAT enabled ? */

    if (chkHost (mcatHost, mcatPort, &tmpHElement) >= 0) {
        tmpHElement->mdasEnabled = 1;
        /* KESTOCK - mem leak fix */
        free (mcatHost);
    } else {
        tmpHElement = enqueHost (mcatHost, mcatPort, &remoteHostHead);
        tmpHElement->mdasEnabled = 1;

        /* Enqueue all the address too */
        if ((hostEnt = gethostbyname (mcatHost)) == NULL) {
            elog (NOTICE,
              "initSrbHost: gethostbyname of mcatHost error,errno = %d", errno);
            return (unixErrConv (errno));
        }
        if (strcasecmp (mcatHost, hostEnt->h_name) != 0) {
            tmpName = malloc (strlen (hostEnt->h_name) + 1);
            strcpy (tmpName, hostEnt->h_name);
            enqueName (tmpName, tmpHElement);
        }

        i = 0;

        while (1) {
            if (hostEnt->h_addr_list[i] != NULL) {
                bcopy (hostEnt->h_addr_list[i], &in, hostEnt->h_length);
                tmpName1 = inet_ntoa (in);
                tmpName = malloc (strlen (tmpName1) + 1);
                strcpy (tmpName, tmpName1);
                enqueName (tmpName, tmpHElement);
                i++;
            } else {
                break;
            }
        }
    }

    /* Now get the resource info from MCAT */

    myresult = getRsrcInfo ();

    while (myresult != NULL) {
      int continueIndex;

      phyResType = (char *)get_from_result_struct(
                (mdasC_sql_result_struct *) myresult,
                  dcs_tname[PHY_RSRC_TYP_NAME], dcs_aname[PHY_RSRC_TYP_NAME]);
      phyResLoc = (char *)get_from_result_struct(

