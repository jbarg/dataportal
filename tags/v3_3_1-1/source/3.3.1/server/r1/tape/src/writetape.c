/* writeTape.c */

#include "srbClient.h"
#define T_3590  1
#ifdef T_3590
#define BUF_LEN (256*1024)
#else
#define BUF_LEN (32*1024) 
#endif
#define DATA_DIR "/scratch/s1/srb/data"
#define PATH_LEN        256

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
    int inFd;
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

    while ((c=getopt(argc, argv,"hd:p:t:")) != EOF) {
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

    if (destPos.filePos.fileSeqNo <= 0) {
	printf ("Input filePos must be >= 1\n");
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
                 "initDrvByDevPath() of %s failed\n", 
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
 
        /* open the source file */

        sprintf (myPath, "%s/%s", DATA_DIR, argv[i]);
        inFd = open (myPath, O_RDONLY, 0);

        if (inFd < 0) {
	    printf ("Unable to open the source file %s\n", argv[optind]);
	    exit (1);
        }

        /* open the tape for write */

	if (tapeOpened == 0) {
            retVal = srbOpenTapeForWrite (&tapeDev, &destPos, argv[optind]);
	    tapeOpened = 1;
            if (retVal < 0) {
                printf ("srbOpenTapeForWrite failed, errno=%d\n", errno);
                exit (1);
            }
	} else {
	    retVal = writeHeader (&tapeDev, argv[optind]);
            tapeOpened = 1;
            if (retVal < 0) {
                printf ("writeHeader failed, errno=%d\n", errno);
                exit (1);
            }
	}

        while ((byteRead = read (inFd, buf, BUF_LEN)) > 0) {
	    byteWritten = srbWriteTape (&tapeDev, buf, byteRead);
	    if (byteWritten != byteRead) {
	        printf ("byteWritten %d not equal to byteRead %d, errno=%d\n", 
	         byteWritten, byteRead, errno);
		if (errno == ENXIO)
		    printf ("write EOT\n");
	        swCloseTape (tapeDev.drvInx, &tapeDev, 0);
	        exit (1);
	    }
        }
	close (inFd);

        /* done writing data, write a EOF mark */

        retVal = swWriteEOF (tapeDev.drvInx, &tapeDev);

        if (retVal < 0) {
            printf ("swWriteEOF failed, errno=%d\n", errno);
            exit (1);
        }
	printf ("finished writing %s\n", myPath);
    }

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
    fprintf(stderr,"Usage  :%s [-h] [-d devicePath] [-p tapePos] [-t tapeNum] fileName\n",
      prog);
}

