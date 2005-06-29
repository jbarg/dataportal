#ifdef PARA_OPR

#define PARA_DEBUG	1 	/* parallel transfer debug msg */

#include "hpssParaOpr.h"

srb_long_t
paraHPSStoFileCopy (char *destPath, char *destResLoc, int destSystemType, 
char *srcPath, char *srcResLoc, int srcSystemType, srb_long_t size)
{
    srb_long_t retVal;

    retVal = paraHPSStoFAPCopy (READ_OPR, destPath, destResLoc, destSystemType,
     srcPath, srcResLoc, srcSystemType, size, NULL, -1);

    return (retVal);
}

srb_long_t
paraHPSSGet (char *srcPath, char *srcResLoc, int srcSystemType,
char *clHostAddr, int clPort)
{
    srb_long_t retVal;

    retVal = paraHPSStoFAPCopy (GET_OPR, NULL, NULL, -1,
     srcPath, srcResLoc, srcSystemType, -1, clHostAddr, clPort);

    return (retVal);
}

srb_long_t
paraHPSStoFAPCopy (int operation, char *destPath, char *destResLoc, 
int destSystemType, char *srcPath, char *srcResLoc, int srcSystemType, 
srb_long_t size, char *clHostAddr, int clPort)
{
#ifdef HPSS
   size_t          tmp;		/* Temporary variables */
   char            *s;
   IOD_t           iod;		/* IOD passed to hpss_ReadList */
   IOR_t           ior;		/* IOR returned from hpss_ReadList */
   srcsinkdesc_t   src, sink;	/* IOD source/sink descriptors */
   struct sockaddr_in controlSocketAddr;	/* control socket addresses */
   u_signed64      bytesMoved;	/* Bytes transferred, as returned from IOR */
   pthread_t       manageParaConnThread;	/* Spawned thread id */
#if defined(PORTNAME_aix)
   pthread_addr_t  pthreadStatus;
#else
   int  pthreadStatus;
#endif
   u_signed64 gapLength;	/* Number of "gap" bytes within the file */
   struct moverInput myMoverInput;
   struct session mySession;
#endif
   int             i;
   int             status;	/* HPSS return code/status */
   int paraOprFd;
   char *portNum, tmpResLoc[MAX_TOKEN];
   struct hostElement  *hostTabPtr;
   int remoteFlag;
   int readListFlags = 0;	/* hpss_ReadList call - could be set
				 * to HPSS_READ_SEQUENTIAL */

   if (srcSystemType != HPSS_FT) {
      elog (NOTICE, "paraHPSStoFileCopy: srcSystemType %d is not type HPSS",
       destSystemType);
      return (PARA_OPR_NOT_HPSS_FT);
   }

   /* The has to be run on a host that runs the HPSS driver. The srcResLoc
    * should be a HPSS address */


#ifdef FED_MCAT
    remoteFlag = chkHostAndZone (srcResLoc, &hostTabPtr);
#else
    /* Parse the srcResLoc */
    strcpy (tmpResLoc, srcResLoc);
    portNum = parseAddr (tmpResLoc);
    remoteFlag = chkHost (tmpResLoc, portNum, &hostTabPtr);
#endif

    if (remoteFlag < 0) {
        elog (NOTICE, "Invalid host addr %s", srcResLoc);
        return INP_ERR_HOST_ADDR;               /* failure */
    } else if (remoteFlag == REMOTE_HOST) {
	return SYS_ERR_NOT_REMOTE_OPR;
   }

#ifdef HPSS	/* only HPSS enable host can run this */

   uiSecs=MIN_30; /* only wait 30 minutes for HPSS open, time out withan error otherwise */

   if ((status = initSession (&mySession)) < 0)
      return (status);

   /* Open the source HPSS file */

    (void)sprintf(cpFunctionName,"hpss_Open(%s, O_RDONLY, 0777, NULL, NULL, NULL)", srcPath);

    signal(SIGALRM, hpss_timeout);
    (void)sprintf(cpLastFileName, "%s pid=%d", srcPath, getpid());

    if (setjmp(jenv) == 0) {
      alarm(uiSecs);
      paraOprFd = hpss_Open(srcPath, O_RDONLY, 0777, NULL, NULL, NULL);
      alarm(0);
    } else {
      /* get there by calling longjmp */
       elog (NOTICE, "timeout because of longjmp trigered by %s. Timeout %u seconds", cpFunctionName, uiSecs);
      status=HPSS_SRB_TIMEOUT_OPEN1;
      return (status);
    }
    strcpy(cpFunctionName, "none_set");
    alarm(0);

   if (paraOprFd < 0) {
      mySession.transferStatus = hpssErrConv (paraOprFd);
      elog (NOTICE, "paraHPSStoFileCopy: hpss_Open error for %s, status = %d",
       srcPath, mySession.transferStatus);
      free (mySession.hostList);
      free (mySession.paraConn);
      return (mySession.transferStatus);
   }

   status = setHpssSize (size, srcPath, &mySession);
   if (status < 0) {
      hpss_Close(paraOprFd);
      free (mySession.hostList);
      free (mySession.paraConn);
      return (mySession.transferStatus);
   }

   status = initControlSocket (&mySession, &controlSocketAddr);
   if (status < 0) {
      free (mySession.hostList);
      free (mySession.paraConn);
      return (status);
   }

   if (listen(mySession.controlSocket, SOMAXCONN) == -1) {
      elog (NOTICE, "paraHPSStoFileCopy: listen error. errno = %d",
        errno);
      free (mySession.hostList);
      free (mySession.paraConn);
      return (OBJ_ERR_SOCKET);
   }

   /*
    * Start the thread to receive control connections from Movers 
    */
   setMoverInput (&myMoverInput, -1, destPath, destResLoc, destSystemType, 
    clHostAddr, clPort, operation, &mySession);
   pthread_create(&manageParaConnThread, pthread_attr_default,
    (void *(*)(void *)) moverManager, (void *) &myMoverInput);
#if !defined(PORTNAME_solaris)
   pthread_yield();
#endif
   /*
    * Define source and sink descriptors and the IOD 
    */
   initHpssIodForRead (&iod, &src, &sink, paraOprFd, &mySession, 
    &controlSocketAddr);

   gapLength = mySession.totalBytesMoved = bytesMoved = cast64m(0);
   mySession.transferStatus = HPSS_E_NOERROR;
   /*
    * Loop as long as the total bytes moved plus all reported gaps are less
    * than the total size of the file AND no transfer error has been
    * encountered 
    */
   while (lt64m(add64m(bytesMoved, gapLength), mySession.fileSize) &&
	  mySession.transferStatus == HPSS_E_NOERROR) {
      /*
       * Set the source/sink length to the number of bytes we want 
       */
      src.Offset = sink.Offset = add64m(bytesMoved, gapLength);
      src.Length = sink.Length = sub64m(mySession.fileSize, src.Offset);
      src.SrcSinkAddr.Addr_u.ClientFileAddr.FileOffset = src.Offset;
      memset(&ior, 0, sizeof(ior));
      status = hpss_ReadList(&iod, readListFlags, &ior);
      if (status) {
	 if (ior.Status != HPSS_E_NOERROR) {
            elog (NOTICE, 
             "paraHPSStoFileCopy: hpss_ReadList error,status=%d,ior status=%d",
             status, ior.Status);
	    mySession.transferStatus = PARA_OPR_HPSS_READLIST_ERR;
	 } else if (mySession.transferStatus == HPSS_E_NOERROR) {
            elog (NOTICE, "paraHPSStoFileCopy: hpss_ReadList error, status= %d",
	     status);
	    mySession.transferStatus = PARA_OPR_HPSS_READLIST_ERR;
         }
      } else {
	 inc64m(bytesMoved, ior.SinkReplyList->BytesMoved);
	 /*
	  * See if data transfer stopped at a gap (hole) 
	  */
	 if (ior.Flags & IOR_GAPINFO_VALID) {
	    inc64m (gapLength, 
              ior.ReqSpecReply->ReqReply_s.ReqReply_u.GapInfo.Length);
#ifndef NO_DCE
	    if (ior.ReqSpecReply)
	       rpc_ss_client_free(ior.ReqSpecReply);
#endif
	 }
#ifdef NO_DCE
	 free_ior_mem (&ior);
#else
	 if (ior.SrcReplyList)
	    rpc_ss_client_free(ior.SrcReplyList);
	 if (ior.SinkReplyList)
	    rpc_ss_client_free(ior.SinkReplyList);
#endif
      }
   }				/* end while */

   /*
    * Close the HPSS file 
    */
   status = hpss_Close(paraOprFd);

   /*
    * Let's make sure that all data has actually been received before we kill
    * any active transfer threads 
    */
   pthread_mutex_lock(&mySession.paraOprMutex);
   if (neq64m(bytesMoved, mySession.totalBytesMoved)) {
#if defined(PORTNAME_aix)
      struct timespec delay = {1, 0};	/* 1 second */
#else
	int delay = 1;
#endif
      pthread_mutex_unlock(&mySession.paraOprMutex);
      /*
       * Wait for all transfer threads to complete before moving on 
       */
      for (i = 0; i < DEFAULT_MAX_CONNECTIONS; i++) {
	 pthread_mutex_lock(&mySession.paraOprMutex);
	 while (mySession.paraConn[i].active) {
	    pthread_mutex_unlock(&mySession.paraOprMutex);
#if defined(PORTNAME_aix)
	    (void) pthread_delay_np(&delay);
#else
	    srbThreadSleep (delay, 0);
#endif
	    pthread_mutex_lock(&mySession.paraOprMutex);
	 }
	 pthread_mutex_unlock(&mySession.paraOprMutex);
      }
   }

   pthread_mutex_unlock(&mySession.paraOprMutex);
   /*
    * Now cancel the moverManager thread 
    */
   (void) pthread_cancel(manageParaConnThread);
   for (i = 0; i < mySession.connCnt; i++) {
      (void) pthread_join(mySession.paraConn[i].threadId, NULL);
   }
#if !defined(PORTNAME_solaris)
   (void) pthread_join(manageParaConnThread, (void **) &pthreadStatus);
#endif

/* XXXXX this cause core dump. No longer true since adding thread_join
 * call 
*/
   free (mySession.hostList);
   free (mySession.paraConn);
   if (mySession.transferStatus == HPSS_E_NOERROR || 
    mySession.transferStatus == HPSS_ECONN) {
      srb_long_t tmpll;
#if defined(PORTNAME_linux)
      hpssToSrbLong (&bytesMoved, &tmpll);
#else
      memcpy (&tmpll, &bytesMoved, sizeof (tmpll));
#endif
      return (tmpll);
   } else
      return (mySession.transferStatus);
#else   /* HPSS */
   elog (NOTICE, "paraHPSStoFileCopy: HPSS not enabled for HPSS operation");
   return (PARA_OPR_HPSS_NOT_SUPPORTED);
#endif  /* HPSS */
}

#ifdef HPSS
/*
 * ===========================================================================
 * = Function: moverManager - Accept socket connections from HPSS
 * Movers & spawn a new thread to handle each Mover connection and data
 * transfer Return Values: <none>
 * ========================================================================== 
 */
void 
moverManager(struct moverInput *myMoverInput)
{
   int             moverSocketFd;	/* New Mover socket file descriptor */
   int             index;	/* Counters */
   int             tmp;		/* Temporary variable */
   struct sockaddr_in socketAddr;
   struct moverInput *thisMoverInput;
   /*
    * Loop until this thread is cancelled 
    */
   for (;;) {
      tmp = sizeof(socketAddr);
      while ((moverSocketFd =
	      accept(myMoverInput->mySession->controlSocket, 
		(struct sockaddr *) & socketAddr,
		     (size_t *) & tmp)) < 0) {
	 if ((errno != EINTR) && (errno != EAGAIN)) {
            elog (NOTICE, "moverManager: socket accept error, errno=%d", errno);
	    myMoverInput->mySession->transferStatus = OBJ_ERR_SOCKET;
	    break;
	 }
      }				/* end while */
      if (moverSocketFd < 0)
	 break;

      /*
       * Find a connection/transfer thread that is free to accept this
       * connection. If one is not free, sleep for a bit and try again. 
       */
      do {
	 pthread_mutex_lock(&myMoverInput->mySession->paraOprMutex);
	 for (index = 0; index < DEFAULT_MAX_CONNECTIONS; index++) {
	    if (!myMoverInput->mySession->paraConn[index].active) {
	       myMoverInput->mySession->paraConn[index].active = 1;
	       myMoverInput->mySession->paraConn[index].controlSocketFd = 
		moverSocketFd;
	       break;
	    }
	 }
	 pthread_mutex_unlock(&myMoverInput->mySession->paraOprMutex);
	 /*
	  * Sleep (without blocking the process) if no free buffer/thread was
	  * found 
	  */
	 if (index == DEFAULT_MAX_CONNECTIONS) {
	    srbThreadSleep (0, 500000);
	 }
      } while (index == DEFAULT_MAX_CONNECTIONS);
      srbSetsockopt (moverSocketFd);
      /*
       * Spawn a thread to handle this transfer request 
       * have to use a new moverInput because index changes 
       */

      thisMoverInput = (struct moverInput *) malloc (
       sizeof (struct moverInput));

      *thisMoverInput = *myMoverInput;

      thisMoverInput->index = index;

      if (myMoverInput->operation == READ_OPR) {
         pthread_create(&myMoverInput->mySession->paraConn[index].threadId, 
		     pthread_attr_default,
		     (void *(*)(void *)) readMover,
		     (void *) thisMoverInput);
      } else if (myMoverInput->operation == GET_OPR) {
         pthread_create(&myMoverInput->mySession->paraConn[index].threadId,
                     pthread_attr_default,
                     (void *(*)(void *)) readMover,
                     (void *) thisMoverInput);
      } else if (myMoverInput->operation == WRITE_OPR) {
         pthread_create(&myMoverInput->mySession->paraConn[index].threadId, 
		     pthread_attr_default,
		     (void *(*)(void *)) writeMover,
		     (void *) thisMoverInput);
      } else if (myMoverInput->operation == PUT_OPR) {
         pthread_create(&myMoverInput->mySession->paraConn[index].threadId, 
		     pthread_attr_default,
		     (void *(*)(void *)) writeMover,
		     (void *) thisMoverInput);
      }
      myMoverInput->mySession->connCnt++;
#if !defined(PORTNAME_solaris)
      pthread_yield();
#endif
   }				/* end for */
   return;
}

/*
 * ===========================================================================
 * = Function: readMover - Retrieve data transfer using mover protocol 
 *
 * Arguments: myMoverInput - pointer to a struct moverInput.
 *
 * Return Values: <none>
 * ========================================================================== 
 */
void 
readMover(struct moverInput *myMoverInput)
{
   int             status, tmp;	/* Return, temporary values */
   int             transferListenSocket;	/* Socket listen descriptors */
   int             transferSocketFd;	/* Transfer accept socket */
   struct sockaddr_in transferSocketAddr;	/* Transfer socket address */
   int             bytesReceived;
   initiator_msg_t initMessage, initReply;
   initiator_ipaddr_t ipAddr;	/* TCP socket address info */
   initiator_shmaddr_t shmAddr;	/* Shared memory address info */
   completion_msg_t completionMessage;
   struct sockaddr_in relaySocketAddr;
   pthread_t relayThread;
   struct dBuffer dBuf;
   pthread_mutex_t condMutex;
   pthread_cond_t myCondition;
   int index = myMoverInput->index;
   int curInx, compInx;

   transferListenSocket = transferSocketFd = -1;

   /*
    * Spawn a thread to handle relay
    */
   initDBuffer (&dBuf, &condMutex, &myCondition, myMoverInput);

   pthread_create(&relayThread, pthread_attr_default,
            (void *(*)(void *)) readRelay,
                  (void *) &dBuf);

   /*
    * Loop until we reach a condition to discontinue talking with Mover 
    */
   while (myMoverInput->mySession->transferStatus == HPSS_E_NOERROR) {
      /*
       * Get the next transfer initiation message from the Mover. HPSS_ECONN
       * will be returned when the Mover is done. 
       */
      status = mvrprot_recv_initmsg(
       		myMoverInput->mySession->paraConn[index].controlSocketFd,
				    &initMessage);
      if (status == HPSS_ECONN) {
	 break;			/* break out of the while loop */
      } else if (status != HPSS_E_NOERROR) {
	 myMoverInput->mySession->transferStatus = hpssErrConv (status);
         elog (NOTICE, "readMover: mvrprot_recv_initmsg error, status = %d",
          myMoverInput->mySession->transferStatus);
	 continue;
      }
      /*
       * Tell the Mover we will send the address next 
       */
      initReply.Flags = MVRPROT_COMP_REPLY | MVRPROT_ADDR_FOLLOWS;
      /*
       * Let's agree to use the transfer protocol selected by the Mover and
       * let's accept the offset. However, the number of bytes the Mover can
       * transfer at one time is limited by our buffer size, so we tell the
       * Mover how much of the data he has offerred that we are willing to
       * accept. 
       */
      initReply.Type = initMessage.Type;
      initReply.Offset = initMessage.Offset;
      if (gt64m(initMessage.Length, cast64m(MAX_BUFFER_SIZE)))
	 initReply.Length = cast64m(MAX_BUFFER_SIZE);
      else
	 initReply.Length = initMessage.Length;
      /*
       * Send our response back to the Mover 
       */
      status = mvrprot_send_initmsg(
		myMoverInput->mySession->paraConn[index].controlSocketFd,
				    &initReply);
      if (status != HPSS_E_NOERROR) {
         myMoverInput->mySession->transferStatus = hpssErrConv (status);
         elog (NOTICE, "readMover: mvrprot_send_initmsg error, status = %d",
          myMoverInput->mySession->transferStatus);
	 continue;
      }
      curInx = dBuf.curInx;
      compInx = curInx ^ 1;
      /*
       * Based on the type of transfer protocol, allocate memory, send
       * address information, and receive the data from the HPSS Mover 
       */
      switch (initMessage.Type) {
      case NET_ADDRESS:
	 /*
	  * The first time through, allocate the memory buffer and data
	  * transfer socket 
	  */
	 if (!dBuf.buf[0]) {
	    dBuf.buf[0] = malloc(MAX_BUFFER_SIZE);
	    if (!dBuf.buf[0]) {
	       myMoverInput->mySession->transferStatus = SYS_ERR_MALLOC;
	       continue;
	    }
            dBuf.buf[1] = malloc(MAX_BUFFER_SIZE);
            if (!dBuf.buf[1]) {
               myMoverInput->mySession->transferStatus = SYS_ERR_MALLOC;
               continue;
            }

	    transferListenSocket = socket(AF_INET, SOCK_STREAM, 0);
	    if (transferListenSocket == -1) {
               elog (NOTICE, "readMover: socket error, errno = %d", errno);
	       myMoverInput->mySession->transferStatus = OBJ_ERR_SOCKET;
	       continue;
	    }
	    (void) memset(&transferSocketAddr, 0, sizeof(transferSocketAddr));
	    transferSocketAddr.sin_family = AF_INET;
	    transferSocketAddr.sin_port = 0;
	    /*
	     * Select the hostname (IP address) in a round-robin fashion 
	     */
	    pthread_mutex_lock(&myMoverInput->mySession->paraOprMutex);
	    transferSocketAddr.sin_addr.s_addr = 
	     myMoverInput->mySession->hostList[0].ipAddr;
	    pthread_mutex_unlock(&myMoverInput->mySession->paraOprMutex);
	    if (bind(transferListenSocket,
		     (const struct sockaddr *) &transferSocketAddr,
		     sizeof(transferSocketAddr)) == -1) {
               elog (NOTICE, "readMover: socket bind error, errno = %d", errno);
	       myMoverInput->mySession->transferStatus = OBJ_ERR_SOCKET;
	       continue;
	    }
	    tmp = sizeof(transferSocketAddr);
	    (void) memset(&transferSocketAddr, 0, sizeof(transferSocketAddr));
	    if (getsockname(transferListenSocket,
			    (struct sockaddr *) & transferSocketAddr,
			    (size_t *) & tmp) == -1) {
               elog (NOTICE, "readMover: getsockname error, errno = %d", errno);
	       myMoverInput->mySession->transferStatus = OBJ_ERR_SOCKET;
	       continue;
	    }
	    if (listen(transferListenSocket, SOMAXCONN) == -1) {
	       perror("listen");
               elog (NOTICE, "readMover: listen error, errno = %d", errno);
               myMoverInput->mySession->transferStatus = OBJ_ERR_SOCKET;
	       continue;
	    }
	    memset(&ipAddr, 0, sizeof(ipAddr));
	    ipAddr.IpAddr.SockTransferID = 
	      cast64m(myMoverInput->mySession->requestId);
	    ipAddr.IpAddr.SockAddr.family = transferSocketAddr.sin_family;
	    ipAddr.IpAddr.SockAddr.addr = transferSocketAddr.sin_addr.s_addr;
	    ipAddr.IpAddr.SockAddr.port = transferSocketAddr.sin_port;
	    ipAddr.IpAddr.SockOffset = cast64m(0);
	 }
	 /*
	  * Tell the Mover what socket to send the data to 
	  */
	 status = mvrprot_send_ipaddr(
		myMoverInput->mySession->paraConn[index].controlSocketFd,
				      &ipAddr);
	 if (status != HPSS_E_NOERROR) {
            myMoverInput->mySession->transferStatus = hpssErrConv (status);
	    elog (NOTICE, "readMover: mvrprot_send_ipaddr error, status = %d", 
             myMoverInput->mySession->transferStatus);
	    continue;
	 }
	 /*
	  * Wait for the new Mover socket connection, if you don't already
	  * have one 
	  */
	 if (transferSocketFd == -1) {
	    tmp = sizeof(transferSocketAddr);
	    while ((transferSocketFd =
		    accept(transferListenSocket,
			   (struct sockaddr *) & transferSocketAddr,
			   (size_t *) & tmp)) < 0) {
	       if ((errno != EINTR) && (errno != EAGAIN)) {
                  elog (NOTICE, 
                   "readMover: accept error, errno = %d", errno);
                  myMoverInput->mySession->transferStatus = OBJ_ERR_SOCKET;
		  break;
	       }
	    }			/* end while */
	    srbSetsockopt (transferSocketFd);
	    if (transferSocketFd < 0)
	       continue;
	 }
         /* Create the output file */
         if (dBuf.fileFd == -1) {
            if (myMoverInput->operation == READ_OPR) {
               pthread_mutex_lock(&myMoverInput->mySession->paraOprMutex);
               if (myMoverInput->mySession->firstTimeOpen > 0) {
                  dBuf.fileFd =  _svrFileCreate (myMoverInput->systemType,
                   myMoverInput->resLoc, myMoverInput->path, StorVaultMode, 
                    NO_CHKDIR, -1, UNIQUE_CONN);
                  if (dBuf.fileFd < 0) 
                     dBuf.fileFd =  _svrFileOpen (myMoverInput->systemType,
                      myMoverInput->resLoc, myMoverInput->path, O_RDWR, 
		       StorVaultMode, NO_CHKDIR, UNIQUE_CONN);
                  myMoverInput->mySession->firstTimeOpen = 0;
               } else {
                  dBuf.fileFd =  _svrFileOpen (myMoverInput->systemType,
                   myMoverInput->resLoc, myMoverInput->path, O_RDWR, 
		   StorVaultMode, NO_CHKDIR, UNIQUE_CONN);
               }   
               pthread_mutex_unlock(&myMoverInput->mySession->paraOprMutex);
            } else {	/* a GET_OPR */
               dBuf.fileFd =
                portalConnect (myMoverInput->clPort, myMoverInput->clHostAddr);
               if (dBuf.fileFd  < 0) {
                  myMoverInput->mySession->transferStatus = dBuf.fileFd;
                  /* use fprintf intead of elog because multi error msg to 
		   * client can cause core dump */
		  fprintf (stderr,
                   "readMover: portalConnect of %s at %s failed, status= %d\n",
                    dBuf.fileFd);
                  continue;
               }
            }

            if (dBuf.fileFd  < 0) {
               myMoverInput->mySession->transferStatus = dBuf.fileFd;
               continue;
            }
         }

	 /*
	  * Receive the data from the Mover via our socket 
	  */
	 status = mover_socket_recv_data(transferSocketFd, 
	   cast64m(myMoverInput->mySession->requestId),
					 initMessage.Offset, dBuf.buf[curInx],
					 low32m(initReply.Length),
					 &bytesReceived, 1);
	 if (status <= 0) {
            myMoverInput->mySession->transferStatus = hpssErrConv (status);
            elog (NOTICE, 
             "readMover: mover_socket_recv_data error, status = %d", 
               myMoverInput->mySession->transferStatus);
	 }
         /* relay the data to client */
	 pthread_mutex_lock (dBuf.condMutex);
         while (1) {
            if (dBuf.status[compInx] == DBUF_NO_DATA) {  /* The other done ? */
#if defined(PORTNAME_linux)
	       hpssToSrbLong (&initMessage.Offset, &dBuf.offset[curInx]);
#else
               memcpy (&dBuf.offset[curInx], &initMessage.Offset, 
                sizeof (dBuf.offset[curInx]));
#endif
               dBuf.length[curInx] = low32m (initMessage.Length);
               dBuf.status[curInx] = DBUF_HAVE_DATA;
               /* reset the inx */
               dBuf.curInx = compInx;
               break;
            } else if (dBuf.status[compInx] == DBUF_ALL_DONE) {
               break;
            } else {
               pthread_cond_wait (dBuf.myCondition, dBuf.condMutex);
            }
         }
         pthread_cond_signal (dBuf.myCondition);
         pthread_mutex_unlock (dBuf.condMutex);
	 break;
      default:
	 break;
      }				/* end switch */
      /*
       * Get a transfer completion message from the Mover 
       */
      status = mvrprot_recv_compmsg(
		myMoverInput->mySession->paraConn[index].controlSocketFd,
				    &completionMessage);
#ifdef PARA_DEBUG
      printf ("Thread %d - %d received at offset %d\n",
       index + 1, low32m(completionMessage.BytesMoved),
        low32m(initMessage.Offset));
#endif
      if (status != HPSS_E_NOERROR) {
         myMoverInput->mySession->transferStatus = hpssErrConv (status);
         elog (NOTICE, "readMover: mvrprot_recv_compmsg error, status = %d",
          myMoverInput->mySession->transferStatus);
	 continue;
      }
      pthread_mutex_lock(&myMoverInput->mySession->paraOprMutex);
      inc64m(myMoverInput->mySession->totalBytesMoved, 
	completionMessage.BytesMoved);
      pthread_mutex_unlock(&myMoverInput->mySession->paraOprMutex);
   }				/* end while loop */

   /*
    * Clean up, based on the transfer protocol 
    */
   curInx = dBuf.curInx;
   compInx = curInx ^ 1;
   switch (initMessage.Type) {
   case NET_ADDRESS:
   default:
      /*
       * Tell the relay thread DBUF_ALL_DONE 
       */
      if (dBuf.fileFd  != -1) {
         pthread_mutex_lock (dBuf.condMutex);
         while (1) {
            if (dBuf.status[compInx] == DBUF_NO_DATA) {
               dBuf.operation = DONE_OPR;
#if defined(PORTNAME_linux)
               hpssToSrbLong (&initMessage.Offset, &dBuf.offset[compInx]);
#else
               memcpy (&dBuf.offset[compInx], &initMessage.Offset, 
                sizeof (dBuf.offset[compInx]));
#endif
               dBuf.length[compInx] = low32m (initMessage.Length);
               dBuf.status[compInx] = DBUF_ALL_DONE;
               break;
            } else if (dBuf.status[compInx] == DBUF_ALL_DONE) {
               break;
            } else {
               pthread_cond_wait (dBuf.myCondition, dBuf.condMutex);
            }
         }
         pthread_cond_signal (dBuf.myCondition);
         pthread_mutex_unlock (dBuf.condMutex);

         /* cancel the relay thread */
   	 (void) pthread_cancel(relayThread);
   	 (void) pthread_join(relayThread, NULL);

#ifdef PARA_DEBUG
         printf ("readMover:Thread %d exiting\n", myMoverInput->index + 1);
         fflush (stdout);
#endif
         /* Close the output file */
         if (myMoverInput->operation == READ_OPR) {
            svrFileClose (dBuf.fileFd);
         } else {
            sendDoneToPortal (dBuf.fileFd);
            close (dBuf.fileFd);        /* a portal */
         }
      }

      /*
       * Close down the TCP transfer socket if it got opened
       */
      if (transferSocketFd != -1) {
         (void) close(transferSocketFd);
      }

      break;
   }				/* end switch */
   /*
    * Free the buffer if it was allocated
    */
   if (dBuf.buf[0])
      (void) free(dBuf.buf[0]);
   if (dBuf.buf[1])
      (void) free(dBuf.buf[1]);

   /*
    * Close the control socket 
    */
   (void) close(transferListenSocket);
   /*
    * Close the control socket and mark this connection as not active 
    */
   pthread_mutex_lock(&myMoverInput->mySession->paraOprMutex);
   (void) close(myMoverInput->mySession->paraConn[index].controlSocketFd);
   myMoverInput->mySession->paraConn[index].active = 0;
   pthread_mutex_unlock(&myMoverInput->mySession->paraOprMutex);
   free (myMoverInput);
   return;
}

#endif /* HPSS */

void
readRelay(struct dBuffer *dBuf)
{
   struct relayHeader myHeader;
   srb_long_t status = 1;
   int compInx;

   while (1) {
      pthread_mutex_lock (dBuf->condMutex);
      compInx = dBuf->curInx ^ 1;
      while (1) {
         if (dBuf->status[compInx] == DBUF_HAVE_DATA) {
            break;
         } else if (dBuf->status[compInx] == DBUF_ALL_DONE) {
            pthread_mutex_unlock (dBuf->condMutex);
            return;
         } else {
            pthread_cond_wait (dBuf->myCondition, dBuf->condMutex);
            compInx = dBuf->curInx ^ 1;
         }
      }
      pthread_mutex_unlock (dBuf->condMutex);

      if (dBuf->operation == READ_OPR) {
         if (dBuf->curOffet != dBuf->offset[compInx]) {
            /* First time around */
            status = _svrFileSeek (dBuf->fileFd, dBuf->offset[compInx], 
             SEEK_SET);
            if (status < 0) {
               elog (NOTICE, "readRelay : _svrFileSeek error, status = %d",
                status);
               if (dBuf->myMoverInput != NULL)
	          dBuf->myMoverInput->mySession->transferStatus = status;
               else
                  dBuf->status[compInx] = status;
	       break;
            }
         }
         status = _svrFileWrite (dBuf->fileFd, dBuf->buf[compInx], 
          dBuf->length[compInx]);
      } else {	/* PUT_OPR */
         status = writeToPortal (dBuf->fileFd, dBuf->buf[compInx],
          dBuf->offset[compInx], dBuf->length[compInx]);
      }

      if (status < 0) {
         elog (NOTICE, "readRelay : write error, status = %d",
          status);
         if (dBuf->myMoverInput != NULL)
            dBuf->myMoverInput->mySession->transferStatus = status;
         else
            dBuf->status[compInx] = status;
         break;
      }
      dBuf->curOffet = dBuf->offset[compInx] + dBuf->length[compInx];
      pthread_mutex_lock (dBuf->condMutex);
      dBuf->status[compInx] = DBUF_NO_DATA;
      pthread_cond_signal (dBuf->myCondition);
      pthread_mutex_unlock (dBuf->condMutex);
   }
   pthread_mutex_lock (dBuf->condMutex);
   dBuf->status[compInx] = DBUF_ALL_DONE;
   pthread_cond_signal (dBuf->myCondition);
   pthread_mutex_unlock (dBuf->condMutex);
}


srb_long_t
paraFiletoHPSSCopy (char *destPath, char *destResLoc, int destSystemType,
char *srcPath, char *srcResLoc, int srcSystemType, srb_long_t size)
{ 
    srb_long_t retVal;

    retVal = paraFAPtoHPSSCopy (WRITE_OPR, destPath, destResLoc, destSystemType,
     srcPath, srcResLoc, srcSystemType, size, NULL, -1);

    return (retVal);
}
 
srb_long_t
paraHPSSPut (char *destPath, char *destResLoc, int destSystemType,
char *clHostAddr, int clPort, srb_long_t size)
{
    srb_long_t retVal;

    retVal = paraFAPtoHPSSCopy (PUT_OPR, destPath, destResLoc, destSystemType,
     NULL, NULL, -1, size, clHostAddr, clPort);

    return (retVal);
}


srb_long_t
paraFAPtoHPSSCopy (int operation, char *destPath, char *destResLoc,
int destSystemType, char *srcPath, char *srcResLoc, int srcSystemType,
srb_long_t size, char *clHostAddr, int clPort)
{
#ifdef HPSS
   size_t          tmp;		/* Temporary variables */
   int             cos = 0;	/* Specified COS (if any) */
   IOD_t           iod;		/* IOD passed to hpss_WriteList */
   IOR_t           ior;		/* IOR returned from hpss_WriteList */
   srcsinkdesc_t   src, sink;	/* IOD source/sink descriptors */
   char            *s;
   hpss_cos_hints_t hints;
   hpss_cos_priorities_t pris;
   hpss_cos_md_t   cosinfo;
   struct sockaddr_in controlSocketAddr;	/* control socket addresses */
   u_signed64      bytesMoved;  /* Bytes transferred, as returned from IOR */
   pthread_t       manageParaConnThread;	/* Spawned thread id */
#if defined(PORTNAME_aix)
   pthread_addr_t  pthreadStatus;
#else
   int  pthreadStatus;
#endif
   u_signed64 gapLength;        /* Number of "gap" bytes within the file */
   struct moverInput myMoverInput;
   struct session mySession;
#endif
   int             i;
   int             status;      /* HPSS return code/status */
   int paraOprFd;
   char *portNum, tmpResLoc[MAX_TOKEN];
   struct hostElement  *hostTabPtr;
   int remoteFlag;
   int             writeListFlags = 0;	/* Flags on hpss_WriteList call */


   if (destSystemType != HPSS_FT) {
      elog (NOTICE, "paraFiletoHPSSCopy: destSystemType %d is not type HPSS",
       destSystemType);
      return (PARA_OPR_NOT_HPSS_FT);
   }

   /* This has to be run on a host that runs the HPSS driver. The destResLoc
    * should be a HPSS address */

    /* Parse the destResLoc */

#ifdef FED_MCAT
    remoteFlag = chkHostAndZone (destResLoc, &hostTabPtr);
#else
    strcpy (tmpResLoc, destResLoc);
    portNum = parseAddr (tmpResLoc);
    remoteFlag = chkHost (tmpResLoc, portNum, &hostTabPtr);
#endif

    if (remoteFlag < 0) {
        elog (NOTICE, "Invalid host addr %s", destResLoc);
        return INP_ERR_HOST_ADDR;               /* failure */
    } else if (remoteFlag == REMOTE_HOST) {
        return SYS_ERR_NOT_REMOTE_OPR;
   }

#ifdef HPSS     /* only HPSS enable host can run this */

   if ((status = initSession (&mySession)) < 0)
      return (status);

   memset(&hints, 0, sizeof(hints));
   memset(&pris, 0, sizeof(pris));

   /*
    * Pass the COS as a hint, if it was specified. If not specified, a COS
    * will be selected automatically based on file size. Even if was give a
    * COS as a hint, still provide file size info so a proper segment size
    * will be selected. 
    */

   status = setSrbSize (size, srcPath, srcResLoc, srcSystemType, &mySession);

   if (status < 0) {
      free (mySession.hostList);
      free (mySession.paraConn);
      return (status);
   }

   hints.MinFileSize = mySession.fileSize;
   hints.MaxFileSize = mySession.fileSize;

   pris.MinFileSizePriority = REQUIRED_PRIORITY;
   pris.MaxFileSizePriority = REQUIRED_PRIORITY;

   if ((cos = getCosBySize ((srb_long_t) size)) >= 0) {
      hints.COSId = cos;
      pris.COSIdPriority = REQUIRED_PRIORITY;
   } else {
      elog (NOTICE, "paraFiletoHPSSCopy: getCosBySize error");
      free (mySession.hostList);
      free (mySession.paraConn);
      return cos;
   }

   uiSecs=MIN_30; /* only wait 30 minutes for HPSS open, time out withan error otherwise */

    /* done in _svrDataPut now. XXXXXX not quite. does not handle
     * _svrdataCopy */
    /*
     * Create the output HPSS file 
     */
   if ((status = chkdir (destSystemType, destResLoc, destPath, 
     NO_CHKDIR, FILE_T)) < 0) {
     elog (NOTICE, "paraFiletoHPSSCopy: chkdir error, status = %d", status);
      free (mySession.hostList);
      free (mySession.paraConn);
      return status;
   }

   
   (void)sprintf(cpFunctionName,
     "hpss_Open(%s, O_WRONLY | O_TRUNC | O_CREAT, %d,,, NULL)", 
      destPath, StorVaultMode);
   
   signal(SIGALRM, hpss_timeout);
   (void)sprintf(cpLastFileName, "%s pid=%d", destPath, getpid());
   
   if (setjmp(jenv) == 0) {
     alarm(uiSecs);

     (void) hpss_Umask((mode_t) 0000);
     paraOprFd = hpss_Open(destPath, O_WRONLY | O_TRUNC | O_CREAT, 
      StorVaultMode, &hints, &pris, NULL);
     alarm(0);
   } else {
     /* get there by calling longjmp */
     elog (NOTICE, "timeout because of longjmp trigered by %s. Timeout %u seconds", cpFunctionName, uiSecs);
     status=HPSS_SRB_TIMEOUT_OPEN2;
     return (status);
   }
   strcpy(cpFunctionName, "none_set");
   alarm(0);
   
   if (paraOprFd < 0) {
      mySession.transferStatus = hpssErrConv (paraOprFd);
      elog (NOTICE, "paraFiletoHPSSCopy: hpss_Open error for %s, status = %d",
       destPath, mySession.transferStatus);
      free (mySession.hostList);
      free (mySession.paraConn);
      return (mySession.transferStatus);
   }

   /*
    * Create the local control socket which all Movers will initially connect
    * to 
    */
   status = initControlSocket (&mySession, &controlSocketAddr);

   if (status < 0) {
      free (mySession.hostList);
      free (mySession.paraConn);
      return (status);
   }

   if (listen(mySession.controlSocket, SOMAXCONN) == -1) {
      elog (NOTICE, "paraHPSStoFileCopy: listen error. errno = %d",
        errno);
      free (mySession.hostList);
      free (mySession.paraConn);
      return (OBJ_ERR_SOCKET);
   }

   /*
    * Start the thread to receive control connections from individual Movers 
    */
   setMoverInput (&myMoverInput, -1, srcPath, srcResLoc, srcSystemType,
    clHostAddr, clPort, operation, &mySession);
   pthread_create(&manageParaConnThread, pthread_attr_default,
		  (void *(*)(void *)) moverManager,
		  (void *) &myMoverInput);
#if !defined(PORTNAME_solaris)
   pthread_yield();
#endif

   /*
    * Define source and sink descriptors and the IOD 
    */

   initHpssIodForWrite (&iod, &src, &sink, paraOprFd, &mySession,
     &controlSocketAddr);

   mySession.totalBytesMoved = cast64m(0);
   mySession.transferStatus = HPSS_E_NOERROR;

   memset(&ior, 0, sizeof(ior));
   status = hpss_WriteList(&iod, writeListFlags, &ior);
   if (status) {
      if (ior.Status != HPSS_E_NOERROR) {
         elog (NOTICE, 
          "paraHPSStoFileCopy: hpss_WriteList error,status=%d,ior status=%d",
           status, ior.Status);
	 mySession.transferStatus = PARA_OPR_HPSS_WRITELIST_ERR;
      } else if (mySession.transferStatus == HPSS_E_NOERROR) {
         elog (NOTICE, "paraHPSStoFileCopy: hpss_WriteList error, status= %d",
          status);
	 mySession.transferStatus = PARA_OPR_HPSS_WRITELIST_ERR;
      }
   }
   /*
    * Close the HPSS file 
    */
   status = hpss_Close(paraOprFd);
   /*
    * Let's make sure that all data has actually been sent before we kill any
    * active transfer threads 
    */
   pthread_mutex_lock(&mySession.paraOprMutex);
   if (neq64m(mySession.fileSize, mySession.totalBytesMoved)) {
#if defined(PORTNAME_aix)
      struct timespec delay = {1, 0};   /* 1 second */
#else
        int delay = 1;
#endif
      pthread_mutex_unlock(&mySession.paraOprMutex);
      /*
       * Wait for all transfer threads to complete before moving on 
       */
      for (tmp = 0, i = 0; i < DEFAULT_MAX_CONNECTIONS; i++) {
	 pthread_mutex_lock(&mySession.paraOprMutex);
	 while (mySession.paraConn[i].active) {
	    pthread_mutex_unlock(&mySession.paraOprMutex);
#if defined(PORTNAME_aix)
            (void) pthread_delay_np(&delay);
#else
            srbThreadSleep (delay, 0);
#endif
	    pthread_mutex_lock(&mySession.paraOprMutex);
	 }
	 pthread_mutex_unlock(&mySession.paraOprMutex);
      }
   }

   pthread_mutex_unlock(&mySession.paraOprMutex);
   /*
    * Now cancel the manage_mover_connections thread 
    */
   (void) pthread_cancel(manageParaConnThread);

   for (i = 0; i < mySession.connCnt; i++) {
      (void) pthread_join(mySession.paraConn[i].threadId, NULL);
   }

#if !defined(PORTNAME_solaris)
    /* For solaris, pthread_join will hang. reason not known */
   (void) pthread_join(manageParaConnThread, (void **) &pthreadStatus);
#endif
   /* (void) pthread_detach(&manageParaConnThread); */

   free (mySession.hostList);
   free (mySession.paraConn);
   if (mySession.transferStatus == HPSS_E_NOERROR || 
    mySession.transferStatus == HPSS_ECONN) {
      srb_long_t tmpll;
#if defined(PORTNAME_linux)
      hpssToSrbLong (&mySession.totalBytesMoved, &tmpll);
#else
      memcpy (&tmpll, &mySession.totalBytesMoved, sizeof (tmpll));
#endif
      return (tmpll);
   } else
      return (mySession.transferStatus);
#else   /* HPSS */
   elog (NOTICE, "paraHPSStoFileCopy: HPSS not enabled for HPSS operation");
   return (PARA_OPR_HPSS_NOT_SUPPORTED);
#endif  /* HPSS */

}

#ifdef HPSS
/* 
 * =========================================================================== =
 * Function: writeMover - Send data transfer using mover protocol 
 *
 * Arguments: myMoverInput - pointer to a struct moverInput.
 *
 * Return Values: <none>
 * ========================================================================== */
void
writeMover(struct moverInput *myMoverInput)
{
   int             status, tmp;	/* Return, temporary values */
   int             transferListenSocket;	/* Socket listen descriptors */
   int             transferSocketFd;	/* Transfer accept socket */
   struct sockaddr_in transferSocketAddr;	/* Transfer socket address */
   int             bytesSent;
   initiator_msg_t initMessage, initReply;
   initiator_ipaddr_t ipAddr;	/* TCP socket address info */
   initiator_shmaddr_t shmAddr;	/* Shared memory address info */
   completion_msg_t completionMessage;
   struct sockaddr_in relaySocketAddr;
   int nbytes, toread;
   char *bufptr;
   pthread_t relayThread;
   struct dBuffer dBuf;
   int curInx, compInx;
   pthread_mutex_t condMutex;
   pthread_cond_t  myCondition;
   int index = myMoverInput->index; 


   transferListenSocket = transferSocketFd = -1;

   /*
    * Spawn a thread to handle relay
    */
   initDBuffer (&dBuf, &condMutex, &myCondition, myMoverInput);

   pthread_create(&relayThread, pthread_attr_default,
             (void *(*)(void *)) writeRelay,
                  (void *) &dBuf);

   /*
    * Loop until we reach a condition to discontinue talking with Mover 
    */
   while (myMoverInput->mySession->transferStatus == HPSS_E_NOERROR) {
      /*
       * Get the next transfer initiation message from the Mover. HPSS_ECONN
       * will be returned when the Mover is done. 
       */
      status = mvrprot_recv_initmsg(
       myMoverInput->mySession->paraConn[index].controlSocketFd, 
        &initMessage);
      if (status == HPSS_ECONN) {
	 break;			/* break out of the while loop */
      } else if (status != HPSS_E_NOERROR) {
         myMoverInput->mySession->transferStatus = hpssErrConv (status);
         elog (NOTICE, "readMover: mvrprot_recv_initmsg error, status = %d",
          myMoverInput->mySession->transferStatus);
	 myMoverInput->mySession->transferStatus = status;
	 continue;
      }
      /*
       * Tell the Mover we will send the address next 
       */
      initReply.Flags = MVRPROT_COMP_REPLY | MVRPROT_ADDR_FOLLOWS;
      /*
       * Let's agree to use the transfer protocol selected by the Mover and
       * let's accept the offset. However, the number of bytes the Mover can
       * transfer at one time is limited by our buffer size, so we tell the
       * Mover how much of the data he has offerred that we are willing to
       * accept. 
       */
      initReply.Type = initMessage.Type;
      initReply.Offset = initMessage.Offset;
      if (gt64m(initMessage.Length, cast64m(MAX_BUFFER_SIZE)))
	 initReply.Length = cast64m(MAX_BUFFER_SIZE);
      else
	 initReply.Length = initMessage.Length;
      /*
       * Send our response back to the Mover 
       */
      status = mvrprot_send_initmsg(
       		myMoverInput->mySession->paraConn[index].controlSocketFd,
				    &initReply);
      if (status != HPSS_E_NOERROR) {
        myMoverInput->mySession->transferStatus = hpssErrConv (status);
         elog (NOTICE, "writeMover: mvrprot_send_initmsg error, status = %d",
          myMoverInput->mySession->transferStatus);
	 continue;
      }
      /*
       * Based on the type of transfer protocol, allocate memory, send
       * address information, and send the data to the HPSS Mover 
       */
      switch (initMessage.Type) {
      case NET_ADDRESS:
	 /*
	  * The first time through, allocate the memory buffer and data
	  * transfer socket 
	  */
         if (!dBuf.buf[0]) {
            dBuf.buf[0] = malloc(MAX_BUFFER_SIZE);
            if (!dBuf.buf[0]) {
               myMoverInput->mySession->transferStatus = SYS_ERR_MALLOC;
               continue;
            }

            dBuf.buf[1] = malloc(MAX_BUFFER_SIZE);
            if (!dBuf.buf[1]) {
               myMoverInput->mySession->transferStatus = SYS_ERR_MALLOC;
               continue;
            }

	    transferListenSocket = socket(AF_INET, SOCK_STREAM, 0);
	    if (transferListenSocket == -1) {
               elog (NOTICE, "writeMover: socket error, errno = %d", errno);
               myMoverInput->mySession->transferStatus = OBJ_ERR_SOCKET;
	       continue;
	    }

	    (void) memset(&transferSocketAddr, 0, sizeof(transferSocketAddr));
	    transferSocketAddr.sin_family = AF_INET;
	    transferSocketAddr.sin_port = 0;
	    /*
	     * Select the hostname (IP address) in a round-robin fashion 
	     */
	    pthread_mutex_lock(&myMoverInput->mySession->paraOprMutex);
	    transferSocketAddr.sin_addr.s_addr = 
	      myMoverInput->mySession->hostList[0].ipAddr;
	    pthread_mutex_unlock(&myMoverInput->mySession->paraOprMutex);
	    if (bind(transferListenSocket,
		     (const struct sockaddr *) &transferSocketAddr,
		     sizeof(transferSocketAddr)) == -1) {
               elog (NOTICE, "writeMover: socket bind error, errno=%d", errno);
               myMoverInput->mySession->transferStatus = OBJ_ERR_SOCKET;
	       continue;
	    }
	    tmp = sizeof(transferSocketAddr);
	    (void) memset(&transferSocketAddr, 0, sizeof(transferSocketAddr));
	    if (getsockname(transferListenSocket,
			    (struct sockaddr *) & transferSocketAddr,
			    (size_t *) & tmp) == -1) {
               elog (NOTICE, "writeMover: getsockname error,errno=%d", errno);
               myMoverInput->mySession->transferStatus = OBJ_ERR_SOCKET;
	       continue;
	    }
	    if (listen(transferListenSocket, SOMAXCONN) == -1) {
	       perror("listen");
               elog (NOTICE, "writeMover: listen error, errno = %d", errno);
               myMoverInput->mySession->transferStatus = OBJ_ERR_SOCKET;
	       continue;
	    }
	    memset(&ipAddr, 0, sizeof(ipAddr));
	    ipAddr.IpAddr.SockTransferID = 
	      cast64m(myMoverInput->mySession->requestId);
	    ipAddr.IpAddr.SockAddr.family = transferSocketAddr.sin_family;
	    ipAddr.IpAddr.SockAddr.addr = transferSocketAddr.sin_addr.s_addr;
	    ipAddr.IpAddr.SockAddr.port = transferSocketAddr.sin_port;
	    ipAddr.IpAddr.SockOffset = cast64m(0);
	 }
	 /*
	  * Tell the Mover what socket to receive the data from 
	  */
	 status = mvrprot_send_ipaddr(
		myMoverInput->mySession->paraConn[index].controlSocketFd,
				      &ipAddr);
	 if (status != HPSS_E_NOERROR) {
            myMoverInput->mySession->transferStatus = hpssErrConv (status);
            elog (NOTICE, "writeMover: mvrprot_send_ipaddr error, status =",
             myMoverInput->mySession->transferStatus);
	    continue;
	 }
	 /*
	  * Wait for the new Mover socket connection, if you don't already
	  * have one 
	  */
	 if (transferSocketFd == -1) {
	    tmp = sizeof(transferSocketAddr);
	    while ((transferSocketFd =
		    accept(transferListenSocket,
			   (struct sockaddr *) & transferSocketAddr,
			   (size_t *) & tmp)) < 0) {
	       if ((errno != EINTR) && (errno != EAGAIN)) {
                  elog (NOTICE,
                   "writeMover: accept error, errno = %d", errno);
                  myMoverInput->mySession->transferStatus = OBJ_ERR_SOCKET;
		  break;
	       }
	    }			/* end while */
	    srbSetsockopt (transferSocketFd);
	    if (transferSocketFd < 0)
	       continue;
	 }
         /* open the input file */
         if (dBuf.fileFd == -1) {
	    if (myMoverInput->operation == WRITE_OPR) {
               dBuf.fileFd =
                _svrFileOpen (myMoverInput->systemType, myMoverInput->resLoc, 
                 myMoverInput->path, O_RDONLY, StorVaultMode, NO_CHKDIR, 
		 UNIQUE_CONN);
               if (dBuf.fileFd  < 0) {
                  myMoverInput->mySession->transferStatus = dBuf.fileFd;
                  elog (NOTICE,
                   "writeMover: _svrFileOpen of %s at %s failed, status = %d",
                    myMoverInput->path, myMoverInput->resLoc, dBuf.fileFd);
                  continue;
               }
	    } else {	/* a PUT_OPR */
               dBuf.fileFd = 
                portalConnect (myMoverInput->clPort, myMoverInput->clHostAddr);
               if (dBuf.fileFd  < 0) {
                  myMoverInput->mySession->transferStatus = dBuf.fileFd;
                  /* use fprintf intead of elog because multi error msg to 
		   * client can cause core dump */
		  fprintf (stderr,
                   "writeMover: portalConnect of %s at %s failed, status=%d\n",
                    dBuf.fileFd);
                  continue;
               }
            }
         }

         /* relay the data to client */

         pthread_mutex_lock (dBuf.condMutex);
         curInx = dBuf.curInx;
         if (dBuf.length[0] == 0) {
#if defined(PORTNAME_linux)
            hpssToSrbLong (&initMessage.Offset, &dBuf.offset[curInx]);
#else
            memcpy (&dBuf.offset[curInx], &initMessage.Offset, 
	      sizeof (dBuf.offset[curInx]));
#endif
            dBuf.length[curInx] = low32m (initMessage.Length);
            /* signal the relay thread */
            pthread_cond_signal (dBuf.myCondition);
         }
         pthread_mutex_unlock (dBuf.condMutex);
         pthread_mutex_lock (dBuf.condMutex);
         while (1) {
	    srb_long_t tmpOff;
            int tmpLen;

	    tmpLen = low32m (initMessage.Length);
#if defined(PORTNAME_linux)
           hpssToSrbLong (&initMessage.Offset, &tmpOff);
#else
	    memcpy (&tmpOff, &initMessage.Offset, sizeof (tmpOff));
#endif

            if (dBuf.status[curInx] == DBUF_HAVE_DATA) {
               if (dBuf.offset[curInx] != tmpOff ||
                dBuf.length[curInx] < tmpLen) {
	          /* make up a new one */
                  dBuf.status[curInx] = DBUF_NO_DATA;
                  dBuf.offset[curInx] = tmpOff;
                  dBuf.length[curInx] = tmpLen;
                  pthread_cond_signal (dBuf.myCondition);
                  pthread_mutex_unlock (dBuf.condMutex);
                  pthread_mutex_lock (dBuf.condMutex);
                  continue;
               }

               compInx = curInx;
               dBuf.curInx = curInx = compInx ^ 1;
               break;
            } else if (dBuf.status[curInx] == DBUF_ALL_DONE) {
               myMoverInput->mySession->transferStatus = DBUF_ALL_DONE;
               break;
            } else {
               pthread_cond_wait (dBuf.myCondition, dBuf.condMutex);
            }
         }
         pthread_mutex_unlock (dBuf.condMutex);
         pthread_mutex_lock (dBuf.condMutex);

         dBuf.offset[curInx] = dBuf.offset[compInx] + dBuf.length[compInx];
         if (dBuf.offset[curInx] + dBuf.length[compInx] > 
          myMoverInput->mySession->srbFileSize) {
            dBuf.length[curInx] = myMoverInput->mySession->srbFileSize - 
             dBuf.offset[curInx];
         } else {
            dBuf.length[curInx] = dBuf.length[compInx];
         }
         /* signal the relay thread */
         pthread_cond_signal (dBuf.myCondition);

         pthread_mutex_unlock (dBuf.condMutex);

	 /*
	  * Send the data to the Mover via our socket 
	  */
	 status = mover_socket_send_requested_data(transferSocketFd,
	   			cast64m(myMoverInput->mySession->requestId),
						 initMessage.Offset, 
                                                  dBuf.buf[compInx],
						   low32m(initReply.Length),
						   &bytesSent, 1);
	 if (status <= 0) {
           myMoverInput->mySession->transferStatus = hpssErrConv (status);
            elog (NOTICE,
             "writeMover: mover_socket_send_requested_data error, status = %d",
               myMoverInput->mySession->transferStatus);
	 }
         dBuf.status[compInx] = DBUF_NO_DATA;
	 break;
      default:
	 break;
      }				/* end switch */
      /*
       * Get a transfer completion message from the Mover 
       */
      status = mvrprot_recv_compmsg(
		myMoverInput->mySession->paraConn[index].controlSocketFd,
				    &completionMessage);
#ifdef PARA_DEBUG
      {
         srb_long_t tmpll;
         memcpy (&tmpll, &initMessage.Offset, sizeof(tmpll));
         printf ("Thread %d - %d sent at offset %lld\n",
          index + 1, low32m(completionMessage.BytesMoved),
           tmpll);
         fflush (stdout);
      }
#endif
      if (status != HPSS_E_NOERROR) {
         myMoverInput->mySession->transferStatus = hpssErrConv (status);
         elog (NOTICE, "writeMover: mvrprot_recv_compmsg error, status = %d",
          myMoverInput->mySession->transferStatus);
         continue;
      }

      pthread_mutex_lock(&myMoverInput->mySession->paraOprMutex);
      inc64m(myMoverInput->mySession->totalBytesMoved, 
	completionMessage.BytesMoved);
      if (ge64m(myMoverInput->mySession->totalBytesMoved, 
	myMoverInput->mySession->fileSize)) {
	 myMoverInput->mySession->transferStatus = 0;
	 pthread_mutex_unlock(&myMoverInput->mySession->paraOprMutex);
	 break;
      }
      pthread_mutex_unlock(&myMoverInput->mySession->paraOprMutex);
   }				/* end while loop */
   /*
    * Clean up, based on the transfer protocol 
    */
   switch (initMessage.Type) {
   case NET_ADDRESS:
   default:
      /*
       * Inform relay client and close down the relay socket if it got opened
       */
      if (dBuf.fileFd  != -1) {
         pthread_mutex_lock (dBuf.condMutex);
         while (1) {
            if (dBuf.status[curInx] == DBUF_HAVE_DATA) {
               break;
            } else if (dBuf.status[0] == DBUF_ALL_DONE) {
               myMoverInput->mySession->transferStatus = DBUF_ALL_DONE;
               break;
            } else {
               if (dBuf.length[curInx] <= 0)
                   break;
               pthread_cond_wait (dBuf.myCondition, dBuf.condMutex);
            }
         }
         dBuf.operation = DONE_OPR;
	 dBuf.status[0] = dBuf.status[1] = DBUF_ALL_DONE;
         pthread_cond_signal (dBuf.myCondition);
         pthread_mutex_unlock (dBuf.condMutex);

         /* calcel the relay thread */
	 pthread_cancel (relayThread);
         (void) pthread_join(relayThread, NULL);

#ifdef PARA_DEBUG
	 printf ("writeMover:Thread %d exiting\n", myMoverInput->index + 1);
         fflush (stdout);
#endif
         if (myMoverInput->operation == WRITE_OPR) {
            svrFileClose (dBuf.fileFd);
	 } else {
            sendDoneToPortal (dBuf.fileFd);
	    close (dBuf.fileFd);	/* a portal */
         }
      }

      /*
       * Close down the TCP transfer socket if it got opened 
       */
      if (transferSocketFd != -1) {
	 (void) close(transferSocketFd);
      }

      break;
   }				/* end switch */

   /*
    * Free the buffer if it was allocated
    */

   if (dBuf.buf[0])
      (void) free(dBuf.buf[0]);
   if (dBuf.buf[1])
      (void) free(dBuf.buf[1]);

   /*
    * Close the control socket 
    */
   (void) close(transferListenSocket);
   /*
    * Close the control socket and mark this connection as not active 
    */
   pthread_mutex_lock(&myMoverInput->mySession->paraOprMutex);
   (void) close(myMoverInput->mySession->paraConn[index].controlSocketFd);
   myMoverInput->mySession->paraConn[index].active = 0;
   pthread_mutex_unlock(&myMoverInput->mySession->paraOprMutex);
   free (myMoverInput);
   return;
}

#endif  /* HPSS */

void
writeRelay(struct dBuffer *dBuf)
{
   struct relayHeader myHeader;
   srb_long_t status = 1;
   int toread, nbytes;
   char *bufptr;
   int retVal;
   int curInx;

   while (1) {
      pthread_mutex_lock (dBuf->condMutex);
      curInx = dBuf->curInx;
      while (1) {
         if (dBuf->status[curInx] == DBUF_NO_DATA && dBuf->length[curInx] > 0) {
            break;
         } else if (dBuf->status[curInx] == DBUF_ALL_DONE) {
            pthread_mutex_unlock (dBuf->condMutex);
            return;
         } else {
            pthread_cond_wait (dBuf->myCondition, dBuf->condMutex);
            curInx = dBuf->curInx;
         }
      }
      pthread_mutex_unlock (dBuf->condMutex);

      if (dBuf->operation == WRITE_OPR) {
         if (dBuf->curOffet != dBuf->offset[curInx]) {
            /* First time around */
            status = _svrFileSeek (dBuf->fileFd, dBuf->offset[curInx], 
             SEEK_SET);
            if (status < 0) {
               elog (NOTICE, "writeRelay : _svrFileSeek error, status = %d",
                status);
               if (dBuf->myMoverInput != NULL)
                   dBuf->myMoverInput->mySession->transferStatus = status;
               else
                  dBuf->status[curInx] = status;
               break;
            } else {
               dBuf->curOffet = dBuf->offset[curInx];
            }
         }

         nbytes = _svrFileRead (dBuf->fileFd, dBuf->buf[curInx], 
          dBuf->length[curInx]);
         if (status < 0) {
            elog (NOTICE, "writeRelay : _svrFileRead error, status = %d",
             status);
            if (dBuf->myMoverInput != NULL)
               dBuf->myMoverInput->mySession->transferStatus = status;
            else
               dBuf->status[curInx] = status;
            break;
         }
      }	else {	/* a PUT_OPR operation */
         int toread;
         int bytesRead;
         struct relayHeader myHeader;

         toread = dBuf->length[curInx];
         bytesRead = readFromPortal (dBuf->fileFd, dBuf->buf[curInx],
          dBuf->offset[curInx], toread);

        if (bytesRead < 0) {
            if (dBuf->myMoverInput != NULL)
               dBuf->myMoverInput->mySession->transferStatus = bytesRead;
            else
            dBuf->status[curInx] = bytesRead;
        } else if (bytesRead != toread) {   
	    if (dBuf->status[curInx] == DBUF_ALL_DONE)
		return;
            elog (NOTICE,
              "writeRelay: - To read %d b at offset %lld, %d b read",
                toread, dBuf->offset[curInx], bytesRead);
            dBuf->length[curInx] = bytesRead;
        }
      }

      pthread_mutex_lock (dBuf->condMutex);
      dBuf->curOffet += dBuf->length[curInx];
      dBuf->status[curInx] = DBUF_HAVE_DATA;
      pthread_cond_signal (dBuf->myCondition);
      pthread_cond_wait (dBuf->myCondition, dBuf->condMutex);
      pthread_mutex_unlock (dBuf->condMutex);
   }
   pthread_mutex_lock (dBuf->condMutex);
   dBuf->status[curInx] = DBUF_ALL_DONE;
   pthread_cond_signal (dBuf->myCondition);
   pthread_mutex_unlock (dBuf->condMutex);
}


#ifdef HPSS
int
initSession (struct session *mySession)
{
   char           *tmpHost;

   mySession->controlSocket = 0;
   mySession->connCnt = 0;
   mySession->firstTimeOpen = 1;
   mySession->totalBytesMoved = cast64m(0);
   mySession->transferStatus = 0;
   mySession->requestId = getpid();

   /* set up the host address */

   mySession->hostList = (tcphost_t *) malloc(sizeof(tcphost_t));
   if ((tmpHost = getenv("HPSS_HOSTNAME")) != NULL) {
      strncpy (mySession->hostList[0].hostname, tmpHost,
        sizeof(mySession->hostList[0].hostname));
   } else {
      if (gethostname(mySession->hostList[0].hostname,
        sizeof(mySession->hostList[0].hostname)) < 0) {
         elog (NOTICE, "initSession: gethostname error");
         free (mySession->hostList);
         return (INP_ERR_HOST_ADDR);
      }
   }
   mySession->hostList[0].hostEntry =
    gethostbyname(mySession->hostList[0].hostname);
   if (!(mySession->hostList[0].hostEntry)) {
      elog (NOTICE, "initSession: gethostbyname error");
      free (mySession->hostList);
      return (INP_ERR_HOST_ADDR);
   }
   mySession->hostList[0].ipAddr = *((unsigned32 *)
    (mySession->hostList[0].hostEntry->h_addr_list[0]));

   /*
    * Allocate the array of transfer/connection information
    */
   mySession->paraConn = (connection_info_t *) malloc(
                                            sizeof(mySession->paraConn[0]) *
                                             DEFAULT_MAX_CONNECTIONS);
   memset(mySession->paraConn, 0,
    sizeof(mySession->paraConn[0]) * DEFAULT_MAX_CONNECTIONS);
   pthread_mutex_init(&mySession->paraOprMutex, pthread_mutexattr_default);

    return (0);
}

int
initControlSocket (struct session *mySession, 
struct sockaddr_in *controlSocketAddr)
{
   int tmp; 

   /*
    * Create the local control socket which all Movers will initially connect
    * to
    */
   mySession->controlSocket = socket(AF_INET, SOCK_STREAM, 0);
   if (mySession->controlSocket < 0) {
      elog (NOTICE, "paraHPSStoFileCopy: open socket error. errno = %d",
        errno);
      return (OBJ_ERR_SOCKET);
   }

   (void) memset(controlSocketAddr, 0, sizeof(struct sockaddr_in));
   controlSocketAddr->sin_family = AF_INET;
   controlSocketAddr->sin_addr.s_addr = INADDR_ANY;
   controlSocketAddr->sin_port = 0;
   if (bind(mySession->controlSocket,
    (const struct sockaddr *) controlSocketAddr,
     sizeof(struct sockaddr_in)) == -1) {
      elog (NOTICE, "initControlSocket: socket bind error. errno = %d",
        errno);
      return (OBJ_ERR_SOCKET);
   }

   tmp = sizeof(struct sockaddr_in);

   if (getsockname(mySession->controlSocket,
                   (struct sockaddr *) controlSocketAddr,
                   (size_t *) & tmp) == -1) {
      elog (NOTICE, "initControlSocket: getsockname error. errno = %d",
        errno);
      return (OBJ_ERR_SOCKET);
   }

   return (0);
}

int setHpssSize (srb_long_t inSrbsize, char *srcPath, 
struct session *mySession)
{
   int status;

   if (inSrbsize < 0) {
      hpss_fileattr_t myAttr;

      status = hpss_FileGetAttributes (srcPath, &myAttr);

      if (status) {
         mySession->transferStatus = hpssErrConv (status);
         elog (NOTICE,
           "paraHPSStoFileCopy: hpss_FileGetAttributes error, status = %d",
         mySession->transferStatus);
         return (mySession->transferStatus);
      }
      mySession->fileSize = myAttr.BFSAttr.BfAttribMd.DataLen;
   } else {
#if defined(PORTNAME_linux)
       hpssToSrbLong ((u_signed64 *) &inSrbsize, 
        (srb_long_t *) &mySession->fileSize);
#else
      memcpy (&mySession->fileSize, &inSrbsize, sizeof (mySession->fileSize));
#endif
   }
#if defined(PORTNAME_linux)
   hpssToSrbLong ((u_signed64 *) &mySession->fileSize,
    (srb_long_t *) &mySession->srbFileSize);
#else
   memcpy (&mySession->srbFileSize, &mySession->fileSize,
     sizeof (mySession->srbFileSize));
#endif

   return (0);
}

int 
setSrbSize (srb_long_t inSrbsize, char *srcPath, char *srcResLoc, 
int srcSystemType, struct session *mySession)
{
   int status;

   if (inSrbsize < 0) {
      struct srbStat myStat;
      char *srcPortNum;

      srcPortNum = parseAddr (srcResLoc);
      status = _svrFileStat (srcSystemType, srcResLoc, srcPortNum, srcPath,
       &myStat);

      if (status < 0) {
         elog (NOTICE,
           "setSrbSize: _svrFileFstat of %s at %s error. status = %d",
         srcPath, srcResLoc, status);
         return (status);
      }
      mySession->srbFileSize = myStat.st_size;
   } else {
      mySession->srbFileSize = inSrbsize;
   }
#if defined(PORTNAME_linux)
   hpssToSrbLong ((u_signed64 *) &mySession->srbFileSize, 
    (srb_long_t *) &mySession->fileSize);
#else
   memcpy (&mySession->fileSize, &mySession->srbFileSize,
     sizeof (mySession->fileSize));
#endif

   return (0);
}

/* initHpssIodForRead - Must be done after initSession and initControlSocket
 */

int
initHpssIodForRead (IOD_t *iod,srcsinkdesc_t *src,srcsinkdesc_t *sink, 
int paraOprFd, struct session *mySession,struct sockaddr_in *controlSocketAddr)
{

   memset(iod, 0, sizeof(IOD_t));
   memset(src, 0, sizeof(srcsinkdesc_t));
   memset(sink, 0, sizeof(srcsinkdesc_t));

   sink->Flags = XFEROPT_IP;
   sink->Flags |= CONTROL_ADDR;  /* optionally add "| HOLD_RESOURCES" to keep
                                 * mover connections open until entire
                                 * transfer completes */
   src->SrcSinkAddr.Type = CLIENTFILE_ADDRESS;
   src->SrcSinkAddr.Addr_u.ClientFileAddr.FileDes = paraOprFd;
   mySession->requestId = getpid();
   sink->SrcSinkAddr.Type = NET_ADDRESS;
   sink->SrcSinkAddr.Addr_u.NetAddr.SockTransferID =
     cast64m(mySession->requestId);
   sink->SrcSinkAddr.Addr_u.NetAddr.SockAddr.addr =
    mySession->hostList[0].ipAddr;
   sink->SrcSinkAddr.Addr_u.NetAddr.SockOffset = cast64m(0);
   sink->SrcSinkAddr.Addr_u.NetAddr.SockAddr.port = controlSocketAddr->sin_port;
   sink->SrcSinkAddr.Addr_u.NetAddr.SockAddr.family =
      controlSocketAddr->sin_family;
   iod->Function = IOD_READ;
   iod->RequestID = mySession->requestId;
   iod->SrcDescLength = 1;
   iod->SinkDescLength = 1;
   iod->SrcDescList = src;
   iod->SinkDescList = sink;

   return (0);
}

/* initHpssIodForWrite - Must be done after initSession and initControlSocket
 */

int
initHpssIodForWrite (IOD_t *iod,srcsinkdesc_t *src,srcsinkdesc_t *sink,
int paraOprFd, struct session *mySession,struct sockaddr_in *controlSocketAddr)
{

   memset(iod, 0, sizeof(IOD_t));
   memset(src, 0, sizeof(srcsinkdesc_t));
   memset(sink, 0, sizeof(srcsinkdesc_t));

   src->Flags = XFEROPT_IP;
   src->Flags |= CONTROL_ADDR;
   /*
    * Set the source/sink->length to the number of bytes we want
    */
   sink->Offset = src->Offset = cast64m(0);
   sink->Length = src->Length = mySession->fileSize;
   /*
    * Define source and sink->descriptors and the IOD
    */
   sink->SrcSinkAddr.Type = CLIENTFILE_ADDRESS;
   sink->SrcSinkAddr.Addr_u.ClientFileAddr.FileDes = paraOprFd;
   sink->SrcSinkAddr.Addr_u.ClientFileAddr.FileOffset = cast64m(0);
   src->SrcSinkAddr.Type = NET_ADDRESS;
   src->SrcSinkAddr.Addr_u.NetAddr.SockTransferID = 
     cast64m(mySession->requestId);
   src->SrcSinkAddr.Addr_u.NetAddr.SockAddr.addr = 
     mySession->hostList[0].ipAddr;
   src->SrcSinkAddr.Addr_u.NetAddr.SockAddr.port = controlSocketAddr->sin_port;
   src->SrcSinkAddr.Addr_u.NetAddr.SockAddr.family = 
     controlSocketAddr->sin_family;
   src->SrcSinkAddr.Addr_u.NetAddr.SockOffset = cast64m(0);
   iod->Function = IOD_WRITE;
   iod->RequestID = mySession->requestId;
   iod->SrcDescLength = 1;
   iod->SinkDescLength = 1;
   iod->SrcDescList = src;
   iod->SinkDescList = sink;

   return (0);
}

int 
setMoverInput (struct moverInput *myMoverInput, int index, char *path,
char *resLoc, int systemType, char *clHostAddr, int clPort, int operation,
struct session *mySession)
{
   myMoverInput->index = index;
   myMoverInput->path = path;
   myMoverInput->resLoc = resLoc;
   myMoverInput->clHostAddr = clHostAddr;
   myMoverInput->clPort = clPort;
   myMoverInput->systemType = systemType;
   myMoverInput->operation = operation;
   myMoverInput->mySession = mySession;
 
   return (0);
}
#endif	/* HPSS */

int initDBuffer (struct dBuffer *dBuf, pthread_mutex_t *condMutex,
pthread_cond_t  *myCondition, struct moverInput *myMoverInput)
{
   dBuf->buf[0] = dBuf->buf[1] = NULL;
   dBuf->status[0] = dBuf->status[1] = DBUF_NO_DATA;
   dBuf->length[0] = dBuf->length[1] = 0;        /* signify first time too */
   dBuf->offset[0] = dBuf->offset[1] = 0;
   dBuf->status[0] = dBuf->status[1] = 0;
   dBuf->curOffet = 0;
   dBuf->fileFd = -1;
   dBuf->condMutex = condMutex;
   dBuf->myCondition = myCondition;
   dBuf->myMoverInput = myMoverInput;
   dBuf->curInx = 0;
   if (myMoverInput != NULL)
      dBuf->operation = myMoverInput->operation;
   pthread_mutex_init(condMutex, pthread_mutexattr_default);
   pthread_cond_init (myCondition, pthread_condattr_default);

   return (0);
}

#endif	/* PARA_OPR */
