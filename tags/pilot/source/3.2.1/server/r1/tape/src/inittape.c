#include "srbClient.h"

void usage (char *prog);

int
main(int argc, char **argv)
{
    tapeDev_t tapeDev;
    int c;
    int retVal;
    int status;
    char *myDevPath;
    char *tapeType = NULL;
    char *owner = NULL;
    srbConn *conn = NULL;
    int dFlag = 0;	/* input device path */
    int RFlag = 0;	/* Register only */
    int LFlag = 0;	/* Label Only */
    int fFlag = 0;	/* force Label even if the tape is in database */
    int tapeLibInx = 0;	/* tapeLib Index. Default to 0 */
    tapeLibConfig_t *tapeLibConfig = NULL;

    tapeDev.mountedCart = malloc (sizeof (tapeCart_t));
    memset (tapeDev.mountedCart, 0, sizeof (tapeCart_t));

    while ((c=getopt(argc, argv,"RLhfd:l:t:u:")) != EOF) {
        switch (c) {
            case 'h':
                usage (argv[0]);
                exit (0);
                break;
            case 'f':
		fFlag = 1;
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
	    case 'l': 
		tapeLibInx = atoi (optarg);
		break;
	    case 'u':
                owner = strdup (optarg);
                break;
            case 't': 
                if (getCartTypeInxByCartName (optarg) < 0) {
		    fprintf (stderr, "Invalid Input tapeType %s.\n", optarg);
		    exit (1);
		} 
		tapeType = strdup (optarg);
                break;
	    case 'R':		/* register tape only (no labelling) */
		RFlag = 1;
                break;
	    case 'L':           /* label tape only (no registration) */
                LFlag = 1;
                break;
            default:
                usage (argv[0]);
                exit (1);
        }
    }
    
    if (argc != optind + 1) {
	usage (argv[0]);
	exit (1);
    }

    if (strlen (argv[optind]) > TAPE_NUM_LEN) {
	printf ("input tape number > %d\n", TAPE_NUM_LEN);
	exit (1);
    }

    if (RFlag == 0 && LFlag == 0) {
	/* default - label and register */
	RFlag = 1;
	LFlag = 1;
    }

    memcpy (&tapeDev.mountedCart->tapeNum, argv[optind], TAPE_NUM_LEN);

    /* Initialize tape config */

    initTapeDevConfig ();
    initTapeLibConfig();

    tapeLibConfig = matchTapeLibByInx (tapeLibInx);
    if (tapeLibConfig == NULL) {
	fprintf (stderr,
          "matchTapeLibByInx failed. No match for input tapeLibInx %d",
        tapeLibInx);
        exit (1);
    }


    if (LFlag > 0) {
	if (fFlag == 0) {
	    /* check if the tape exist in the database */
	    if (tapeExist (argv[optind]) > 0) {
		fprintf (stderr, "Tape %s is already in the database.\n", 
		  argv[optind]);
		fprintf (stderr, 
		  "Use -f to force the labeling. Info on tape will be lost\n");
		exit (1);
	    }
	}
        if (dFlag == 0) {
	    char *stkHost, *stkPort;

	    /* mount the tape through the tape lib server */
	
            conn = srbConnect (tapeLibConfig->host, 
	      tapeLibConfig->port, NULL, NULL, NULL, NULL, NULL);

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
	        if (initDrvByDevPath (myDevPath, &tapeDev) < 0) {
                    fprintf (stderr,
                     "initDrvByDevPath() of %s failed\n", 
		     tapeDev.mountedCart->tapeNum);
	            free (myDevPath);
                    clFinish(conn);
                    exit (1);
                }
	        free (myDevPath);
		if (tapeType != NULL)
		    free (tapeType);
		tapeType = getCartNameByCartTypeInx (tapeDev.drvTypeInx);
		if (tapeType == NULL) {
                    fprintf (stderr,
                     "getCartNameByCartTypeInx() of %s failed\n",
                     tapeDev.mountedCart->tapeNum);
                    clFinish(conn);
                    exit (1);
                }
            } else {
                printf (
	         "err mounting %s, status = %d\n", tapeDev.mountedCart->tapeNum,
	         retVal);
                clFinish(conn);
                exit (1);
            }
        }
    
        retVal = swInitTape (tapeDev.drvInx, &tapeDev);

	if (conn != NULL) {
            status = srbTapelibDismntCart (conn, tapeDev.mountedCart->tapeNum);
            if (status >= 0) {
                printf ("%s dismounted \n", tapeDev.mountedCart->tapeNum);
            } else {
                printf ("errror dismounting %s, status = %d\n", 
                 tapeDev.mountedCart->tapeNum, status);
            }
            clFinish (conn);
	}

        if (retVal < 0) {
	    printf ("swInitTape failed, errno=%d\n", errno);
	    exit (1);
        }
    }
    if (RFlag > 0) {
	char cTapeLibInx[8];

	/* register the tape */
	if (owner == NULL) {
	    char errMsg[MAX_TOKEN];
	    owner = clGetUserName (NULL, errMsg);
	    if (owner == NULL) {
		fprintf (stderr, "clGetUserName failed\n");
		exit (1);
	    }
	}
	conn = srbConnect (NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        /* check to see that the backend connection was successfully made */
        if ((status = clStatus(conn)) != CLI_CONNECTION_OK) {
            fprintf(stderr,"Connection to srbMaster failed.\n");
            fprintf(stderr,"%s",clErrorMessage(conn));
            clFinish(conn);
            exit (1);
        }

	sprintf (cTapeLibInx, "%d", tapeLibInx);

	retVal = srbModifyRescInfo (conn, MDAS_CATALOG,
	  tapeDev.mountedCart->tapeNum, T_INSERT_TAPE_INFO, 
	  tapeDev.mountedCart->tapeNum,
	  owner,
	  tapeType,
	  cTapeLibInx);

	clFinish(conn);
	if (retVal < 0) {
	    fprintf (stderr, "srbModifyRescInfo failed, stat=%d\n", retVal);
            exit (1);
        }
    }
    exit (0);
}

void
usage (char *prog)
{
    fprintf(stderr,
      "Usage  :%s [-RLh] [-d devicePath] [-l tapeLibInx] [-t tapeType] [-u tapeOwner] tapeNumber\n\n", prog);
    fprintf(stderr, "    -h - help\n");
    fprintf(stderr, "    -R - tape registration only\n");
    fprintf(stderr, "    -L - tape labelling only\n");
}

int
tapeExist (char *tapeNum)
{
    int retVal, status;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    mdasC_sql_result_struct  myresult;
    srbConn *conn = NULL;

    conn = srbConnect (NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    /* check to see that the backend connection was successfully made */
    if ((status = clStatus(conn)) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Connection to srbMaster failed.\n");
       fprintf(stderr,"%s",clErrorMessage(conn));
        clFinish(conn);
	if (status < 0)
	    return (status);
	else
	    return (-1);
    }

    for (i = 0; i < MAX_DCS_NUM; i++) {
        selval[i] = 0;
    }

    selval[TAPE_NUMBER] = 1;
    selval[TAPE_OWNER] = 1;
    selval[TAPE_TYPE_VAL] = 1;
    selval[TAPE_LIBINX] = 1;
    selval[TAPE_FULLFLAG] = 1;
    selval[TAPE_CURRENT_FILE_SEQNUM] = 1;
    selval[TAPE_CURRENT_ABS_POSITION] = 1;
    selval[TAPE_BYTES_WRITTEN] = 1;

    for (j = 0; j < MAX_DCS_NUM; j++) {
        sprintf(qval[j],"");
    }

    sprintf(qval[TAPE_NUMBER], " = '%s'", tapeNum);

    retVal = srbGetDataDirInfo(conn, MDAS_CATALOG, qval, selval,
      &myresult, DEF_NUM_ROWS );

    if (retVal < 0)
	return (retVal);

    retVal = myresult.row_count;    
    clearSqlResult (&myresult);
    return (retVal);
}
