/* readtape.c */

#include "srbClient.h"
#define T_3590	1
#ifdef T_3590
#define BUF_LEN	(256*1024)
#else
#define BUF_LEN	(32*1024)
#endif
#define DATA_DIR "/scratch/s1/srb/data"
#define PUT_FILE_PREF "dest"
#define PATH_LEN	256

void usage (char *prog);

int
main(int argc, char **argv)
{
    tapeDev_t tapeDev;
    int c;
    int dFlag = 0;
    int tFlag = 0;
    int filePos = -1;
    tapePos_t destPos;
    int retVal;
    int outFd;
    char buf [BUF_LEN];
    int byteRead, byteWritten;
    int tapeOpened = 0;
    int i;
    char myPath[PATH_LEN];
    srbConn *conn;
    int status;
    char *myDevPath;

    memset (&tapeDev, 0, sizeof (tapeDev_t));
    memset (&destPos, 0, sizeof (tapePos_t));

    tapeDev.mountedCart = malloc (sizeof (tapeCart_t));
    memset (tapeDev.mountedCart, 0, sizeof (tapeCart_t));

    while ((c=getopt(argc, argv,"hd:t:")) != EOF) {
        switch (c) {
            case 'h':
                usage (argv[0]);
                exit (0);
                break;
	    case 'd':
		if (strlen (optarg) > TAPE_DEVNAME_LEN) {
		    printf ("input dev path length > %d\n", TAPE_DEVNAME_LEN);
		    usage (argv[0]);
                    exit (1);
		} else {
		    dFlag = 1;
		    strcpy (tapeDev.devPath, optarg);
		}
                break;
            case 'p':
		destPos.filePos.fileSeqNo = atoi (optarg);
                break;
            case 't':
	        if (strlen (optarg) > TAPE_NUM_LEN) {
	            printf ("input tape number > %d\n", TAPE_NUM_LEN);
	            exit (1);
	        } else {
	    	    memcpy (&tapeDev.mountedCart->tapeNum, optarg, 
		     TAPE_NUM_LEN);
		    tFlag = 1;
		}
                break;
            default:
                usage (argv[0]);
                exit (1);
        }
    }
    
    if (argc < optind + 1) {
	usage (argv[0]);
	exit (1);
    }

    if (tFlag <= 0) {
	printf ("Must Input tape number\n");
        usage (argv[0]);
        exit (1);
    }

    if (dFlag == 0) {
        char *stkHost, *stkPort;

        initTapeDevConfig ();
        initTapeLibConfig();

        /* mount the tape through the tape lib server */

        if ((stkHost = (char *) getenv("stkHost")) == (char *) NULL) {
            printf ("env Variable stkHost must be set\n");
            exit (1);
        }

        if ((stkPort = (char *) getenv("stkPort")) == (char *) NULL) {
            printf ("env Variable stkPort must be set\n");
            exit (1);
        }

        conn = clConnect (stkHost, stkPort, NULL);

        /* check to see that the backend connection was successfully made */
        if ((status = clStatus(conn)) != CLI_CONNECTION_OK) {
            fprintf(stderr,"Connection to exfMaster failed.\n");
            fprintf(stderr,"%s",clErrorMessage(conn));
            clFinish(conn);
            exit (1);
        }

        retVal = srbTapelibMntCart (conn, tapeDev.mountedCart->tapeNum, 1,
         &myDevPath);
        if (retVal >= 0) {
            printf ("%s mounted on %s\n", tapeDev.mountedCart->tapeNum,
             myDevPath);
            if ((retVal = initDrvByDevPath (myDevPath, &tapeDev)) < 0) {
                printf (
                 "getDrvInxByDevPath() of %s failed\n", 
                 tapeDev.mountedCart->tapeNum);
                free (myDevPath);
                clFinish(conn);
                exit (1);
            }
            free (myDevPath);
        } else {
            printf (
             "errror mounting %s, status = %d\n", tapeDev.mountedCart->tapeNum,
             retVal);
            clFinish(conn);
            exit (1);
        }
    }


    for (i = optind; i < argc; i++) {

	destPos.filePos.fileSeqNo = atoi (argv[i]);

        /* open the dest file */

	sprintf (myPath, "%s/%s%s", DATA_DIR, PUT_FILE_PREF, argv[i]);
        outFd = open (myPath, O_CREAT|O_RDWR, 0640);

        if (outFd < 0) {
            printf ("Unable to open the local file %s\n", argv[i]);
            exit (1);
        }

        /* open the tape for read */

        if (tapeOpened == 0) {
            retVal = srbOpenTapeForRead (&tapeDev, &destPos);
	    tapeOpened = 1;
            if (retVal < 0) {
	        printf ("srbOpenTapeForRead failed, errno=%d\n", errno);
	        exit (1);
            }
	} else {
	    retVal = swSeekTape (tapeDev.drvInx, &tapeDev, &destPos);

	    if (retVal< 0) {
                printf ("swSeekTape failed, errno=%d\n", errno);
                exit (1);
            }

	    retVal = readVerifyHdr (&tapeDev);
            if (retVal < 0) {
                printf ("readVerifyHdr failed, errno=%d\n", errno);
                exit (1);
            }
	}

        while ((byteRead = srbReadTape (&tapeDev, buf, BUF_LEN)) > 0) {
	    byteWritten = write (outFd, buf, byteRead);
            if (byteWritten != byteRead) {
                printf ("byteWritten %d not equal to byteRead %d\n",
                 byteWritten, byteRead);
                swCloseTape (tapeDev.drvInx, &tapeDev, 0);
                exit (1);
            }
        }
	if (byteRead < 0) {
	    printf ("reading of %s error, errno=%d\n", myPath, errno);
	    if (errno == EIO)
		printf ("read EOT\n");
	    break;
	} else {
	    printf ("finished reading in %s\n", myPath);
	}
	close (outFd);
    }

#ifdef foo
    while (readVerifyHdr (&tapeDev) >= 0) {
        while ((byteRead = srbReadTape (&tapeDev, buf, BUF_LEN)) > 0) {
	}
    }
#endif

    swCloseTape (tapeDev.drvInx, &tapeDev, 0);

    status = srbTapelibDismntCart (conn, tapeDev.mountedCart->tapeNum);
    if (status >= 0) {
        printf ("%s dismounted \n", tapeDev.mountedCart->tapeNum);
    } else {
        printf ("errror dismounting %s, status = %d\n",
         tapeDev.mountedCart->tapeNum, status);
    }
    clFinish (conn);

    exit (0);
}

void
usage (char *prog)
{
    fprintf(stderr,"Usage  :%s [-h] [-d devicePath] [-t tapeNum] tapePos1 tapePos2 ...\n",
      prog);
}

