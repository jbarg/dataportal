#include "srbClient.h"

void usage (char *prog);

int
main(int argc, char **argv)
{
    int c;
    int retVal, status;
    int mod1Cnt = 0;	/* first set of mod */
    int mod2Cnt = 0;	/* second set of mod */
    char *tapeNum = "";
    char *owner = "";
    char *tapeType = "";
    char *tapeLibInx = "";
    char *fullFlag = "";
    char *curFileSeqNum = "";
    char *curAbsPos = "";
    char *bytesWritten = "";

    srbConn *conn = NULL;

    while ((c=getopt(argc, argv,"n:u:t:l:f:s:p:b:h")) != EOF) {
        switch (c) {
            case 'h':
                usage (argv[0]);
                exit (0);
                break;
	    case 'n':
		tapeNum = strdup (optarg);
		mod2Cnt ++;
		break;
            case 'u':
                owner = strdup (optarg);
		mod2Cnt ++;
                break;
            case 't':
                tapeType = strdup (optarg);
		mod2Cnt ++;
                break;
            case 'l':
                tapeLibInx = strdup (optarg);
		mod2Cnt ++;
                break;
            case 'f':
		c = atoi (optarg);
		if (c != 0 & c != 1) {
		    fprintf (stderr, "Illegal input fullFlag value %s\n", 
		      fullFlag);
		    exit (1);
		}
                fullFlag = strdup (optarg);
		mod1Cnt ++;
                break;
            case 's':
                curFileSeqNum = strdup (optarg);
		mod1Cnt ++;
                break;
            case 'p':
                curAbsPos = strdup (optarg);
		mod1Cnt ++;
                break;
            case 'b':
                bytesWritten = strdup (optarg);
		mod1Cnt ++;
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
	fprintf (stderr, "input tape number > %d\n", TAPE_NUM_LEN);
	exit (1);
    }
    if (mod1Cnt + mod2Cnt <= 0) {
	fprintf (stderr, "Must have at least 1 metadata for mod\n");
        exit (1);
    }

    conn = srbConnect (NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    /* check to see that the backend connection was successfully made */
    if ((status = clStatus(conn)) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Connection to srbMaster failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
        clFinish(conn);
        exit (1);
    }

    if (mod2Cnt > 0) {
	retVal = srbModifyRescInfo (conn, MDAS_CATALOG,
          argv[optind], T_UPDATE_TAPE_INFO_2,
	  tapeNum,
	  owner,
	  tapeType,
	  tapeLibInx);

        if (retVal < 0) {
	    fprintf (stderr, "srbModifyRescInfo failed for set2, stat=%d\n", 
	      retVal);
	    clFinish(conn);
            exit (1);
        }
    }
    if (mod1Cnt > 0) {
        retVal = srbModifyRescInfo (conn, MDAS_CATALOG,
	  argv[optind], T_UPDATE_TAPE_INFO, 
          fullFlag,
          curFileSeqNum,
          curAbsPos,
          bytesWritten);

        if (retVal < 0) {
            fprintf (stderr, "srbModifyRescInfo failed for set1, stat=%d\n", 
              retVal);
            clFinish(conn);
            exit (1);
        }
    }

    clFinish(conn);
    exit (0);
}

void
usage (char *prog)
{
    fprintf(stderr,
      "Usage  :%s [-h] [-n newTapeNum] [-u tapeOwner] [-t tapeType] [-l tapeLibInx] [-f fullFlag] [-s curFileSeqNum] [-p curAbsPos] [-b bytesWritten] tapeNum\n", prog);
}

