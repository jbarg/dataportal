#include "srbClient.h"

void usage (char *prog);

int
main(int argc, char **argv)
{
    int c;
    int retVal, status;
    int allTape;
    int numTape;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    mdasC_sql_result_struct  myresult;
    srbConn *conn = NULL;
    int nFlag = 0;
    int fFlag = 0;

    while ((c=getopt(argc, argv,"hnf")) != EOF) {
        switch (c) {
            case 'h':
                usage (argv[0]);
                exit (0);
                break;
	    case 'n':		/* list 'not full' tape */
		nFlag = 1;
                break;
	    case 'f':           /* list 'full' tape */
                fFlag = 1;
                break;
            default:
                usage (argv[0]);
                exit (1);
        }
    }
    
    if (argc <= optind) {
	numTape = 1;
	allTape = 1;
	if (nFlag > 0 && fFlag > 0) {
            nFlag = 0;
            fFlag = 0;
        }
    } else {
	numTape = argc - optind;
	allTape = 0;
	nFlag = 0;
	fFlag = 0;
    }

    conn = srbConnect (NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    /* check to see that the backend connection was successfully made */
    if ((status = clStatus(conn)) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Connection to srbMaster failed.\n");
       fprintf(stderr,"%s",clErrorMessage(conn));
        clFinish(conn);
        exit (1);
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

    printf ("tapeNum owner tapeType tapeLibInx fullFlag");
    printf (" curFileSeqNum curAbsPos bytesWritten\n");

    for (i = 0; i < numTape; i++) {
	int j;
	char *tapeNum;
        char *owner;
        char *tapeType;
	char *tapeLibInx;
	char *fullFlag;
	char *curFileSeqNum;
	char *curAbsPos;
	char *bytesWritten;
	int firstTime;

        for (j = 0; j < MAX_DCS_NUM; j++) {
            sprintf(qval[j],"");
        }

	if (allTape == 0) {	/* one tape at a time */
	    sprintf(qval[TAPE_NUMBER], " = '%s'", argv[optind + i]);
	}

	if (fFlag > 0) {
            sprintf(qval[TAPE_FULLFLAG], " = '1'");
        }
	
        if (nFlag > 0) {
            sprintf(qval[TAPE_FULLFLAG], " = '0'");
        }

	firstTime = 1;
	myresult.continuation_index = 0;

	while (myresult.continuation_index >= 0) {
	    if (firstTime > 0) {
	        retVal = srbGetDataDirInfo(conn, MDAS_CATALOG, qval, selval,
                                      &myresult, DEF_NUM_ROWS );
		firstTime = 0;
	    } else {
                retVal = srbGetMoreRows(conn, MDAS_CATALOG,
		  myresult.continuation_index, &myresult, DEF_NUM_ROWS);
	    }
	    if (retVal < 0) {
		fprintf (stderr, "srbGetDataDirInfo error, stat=%d\n", retVal);
		clFinish(conn);
                exit (1);
	    }
            tapeNum = (char *)getFromResultStruct(
                  (mdasC_sql_result_struct *) &myresult,
                  dcs_tname[TAPE_NUMBER],
                  dcs_aname[TAPE_NUMBER]);

            owner = (char *)getFromResultStruct(
                  (mdasC_sql_result_struct *) &myresult,
                  dcs_tname[TAPE_OWNER],
                  dcs_aname[TAPE_OWNER]);

            tapeType = (char *)getFromResultStruct(
                  (mdasC_sql_result_struct *) &myresult,
                  dcs_tname[TAPE_TYPE_VAL],
                  dcs_aname[TAPE_TYPE_VAL]);

            tapeLibInx = (char *)getFromResultStruct(
                  (mdasC_sql_result_struct *) &myresult,
                  dcs_tname[TAPE_LIBINX],
                  dcs_aname[TAPE_LIBINX]);

            fullFlag = (char *)getFromResultStruct(
                  (mdasC_sql_result_struct *) &myresult,
                  dcs_tname[TAPE_FULLFLAG],
                  dcs_aname[TAPE_FULLFLAG]);

            curFileSeqNum = (char *)getFromResultStruct(
                  (mdasC_sql_result_struct *) &myresult,
                  dcs_tname[TAPE_CURRENT_FILE_SEQNUM],
                  dcs_aname[TAPE_CURRENT_FILE_SEQNUM]);

            curAbsPos = (char *)getFromResultStruct(
                  (mdasC_sql_result_struct *) &myresult,
                  dcs_tname[TAPE_CURRENT_ABS_POSITION],
                  dcs_aname[TAPE_CURRENT_ABS_POSITION]);

            bytesWritten = (char *)getFromResultStruct(
                  (mdasC_sql_result_struct *) &myresult,
                  dcs_tname[TAPE_BYTES_WRITTEN],
                  dcs_aname[TAPE_BYTES_WRITTEN]);

            for (j = 0; j < myresult.row_count; j++) {
	        printf ("%7s %-8s %-8s %-7s %8s %13s %9s %12s\n",
	          &tapeNum[j*MAX_DATA_SIZE], &owner[j*MAX_DATA_SIZE], 
	          &tapeType[j*MAX_DATA_SIZE], &tapeLibInx[j*MAX_DATA_SIZE],
	          &fullFlag[j*MAX_DATA_SIZE], &curFileSeqNum[j*MAX_DATA_SIZE], 
	          &curAbsPos[j*MAX_DATA_SIZE], &bytesWritten[j*MAX_DATA_SIZE]);
	    }
            clearSqlResult (&myresult);
	}
    }

    clFinish(conn);
    exit (0);
}

void
usage (char *prog)
{
    fprintf(stderr,
      "Usage  :%s [-hfn] [tapeNumber ...]\n", prog);
}

