/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * testPackMsg.c-- Test the packMsg routines.
 *
 *-------------------------------------------------------------------------
 */
#include <stdio.h>
#include <sys/file.h>
#include <sys/stat.h>
#if defined(PORTNAME_c90) || defined(PORTNAME_solaris)
#include <sys/fcntl.h> 	/* For C-90 - define O_RDONLY */
#endif
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "srbClient.h"

#define BUFSIZE		65536
#define srbAuth		NULL
#define UNIX_AM		0	/* UNIX access method */
#define UTREE_AM	1
#define HPSS_AM		2
#define HOST_ADDR	NULL
#define ROW_COUNT	2
#define COL_COUNT	3

double myMin[] = {1.01, 2.02, 3.03};
double myMax[] = {10.1, 20.2, 30.3};

/* testChar Struct - test the char type */
struct testChar {
    int row_count;
    char myName1[ROW_COUNT][MAX_TOKEN];
    char *myName2;
} testChar;
struct testChar *testCharOut;

FormatDef testChar_PF = "int row_count; char myName1[row_count][MAX_TOKEN]; char *myName2[row_count][MAX_TOKEN];"; 

/* testInt Struct - test the int type */

struct testInt {
    int row_count;
    int col_count;
    int myInt1[ROW_COUNT][COL_COUNT];
    int *myInt2;
} testInt;
struct testInt *testIntOut;

FormatDef testInt_PF = "int row_count; int col_count; int myInt1[row_count][col_count]; int *myInt2[row_count][col_count];"; 

/* testDouble Struct - test the double type */

struct testDouble {
    int row_count;
    int col_count;
    double myDouble1[ROW_COUNT][COL_COUNT];
    double *myDouble2;
} testDouble;
struct testDouble *testDoubleOut;

FormatDef testDouble_PF = "int row_count; int col_count; double myDouble1[row_count][col_count]; double *myDouble2[row_count][col_count];";

FormatDefConst LocalDef[] = {
	{"testChar_PF", (int) &testChar_PF},
	{"testInt_PF", (int) &testInt_PF},
	{"testDouble", (int) &testDouble},
        {END_DEF_NAME, END_DEF_VAL},
};


extern struct vaultQueElement *srbVaultInfo();

void 
exit_nicely(srbConn* conn)
{
  clFinish(conn);
  exit(1);
}

int
main(int argc, char **argv)
{
    struct hostent *hostEnt;
    char sb[MAX_TOKEN];
    srbConn *conn;

    unsigned int mypid;
    int i, j;
    struct in_addr in;
    int status;
    char buf[MAX_TOKEN];
    struct varlena *outMsg;
    int portal;
    sfoMBR myStruct, *outStruct;
    mdasC_sql_result_struct myresult, *myoutresult;
    char *tmpPtr;
    int *tmpInt;
    double *tmpDouble;

#ifdef foo
    /*
     * set up the connection
     */
    conn = clConnect (HOST_ADDR, NULL, srbAuth);

    /* check to see that the backend connection was successfully made */
    if ((status = clStatus(conn)) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Connection to exfMaster failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
        exit_nicely(conn);
    }
#endif
    /* test the sfoMBR_PF */
    myStruct.dim = 3;
    myStruct.minD = myMin;
    myStruct.maxD = myMax;

    status = packMsg ((char *) &myStruct, &outMsg, sfoMBR_PF, LocalDef);
    if (status < 0) 
	fprintf (stderr, "packMsg of sfoMBR_PF error, status = %d\n", status);
    status = unpackMsg (VAROUTDATA(outMsg), (char **) &outStruct, 
      sfoMBR_PF, LocalDef);
    if (status < 0) 
	fprintf (stderr, "unpackMsg of sfoMBR_PF error, status = %d\n", status);

    printf ("Result of pack/unpack sfoMBR_PF: \n");
    printf ("dim = %d\n", outStruct->dim);
    printf ("min = ");
    for (i = 0; i < myStruct.dim; i++) {
	printf (" %f ", outStruct->minD[i]);
    }
    printf ("\n");

    printf ("max = ");
    for (i = 0; i < myStruct.dim; i++) {
        printf (" %f ", outStruct->maxD[i]);
    }
    printf ("\n");

    /* now pack/unpack a mdasC_sql_result_struct */

    for (i = 0; i < MAX_SQL_LIMIT; i++) {
        myresult.sqlresult[i].tab_name = NULL;
        myresult.sqlresult[i].att_name = NULL;
        myresult.sqlresult[i].values = NULL;
    }

    myresult.result_count = 3;
    myresult.row_count = 2;
    myresult.continuation_index = -1;
    myresult.sqlresult[0].tab_name = dcs_tname[DATA_GRP_NAME];
    myresult.sqlresult[0].att_name = dcs_aname[DATA_GRP_NAME];
    myresult.sqlresult[1].tab_name = dcs_tname[DATA_NAME];
    myresult.sqlresult[1].att_name = dcs_aname[DATA_NAME];
    myresult.sqlresult[2].tab_name = dcs_tname[SIZE];
    myresult.sqlresult[2].att_name = dcs_aname[SIZE];

    for (i = 0; i < myresult.result_count; i++) {
        tmpPtr = malloc (myresult.row_count * MAX_DATA_SIZE);
	myresult.sqlresult[i].values = tmpPtr;
	for (j = 0; j < myresult.row_count; j++) {
	    sprintf (tmpPtr, "test string %d %d", i, j);
	    tmpPtr += MAX_DATA_SIZE;
	}
    } 
    status = packMsg ((char *) &myresult, &outMsg, sqlResultStruct_PF, 
     LocalDef);
    if (status < 0) 
	fprintf (stderr, "packMsg of sqlResultStruct_PF error, status = %d\n", 
	 status);
    status = unpackMsg (VAROUTDATA(outMsg), (char **) &myoutresult, 
      sqlResultStruct_PF, LocalDef);

    if (status < 0) 
	fprintf (stderr, "unpackMsg of sqlResultStruct_PF error, status = %d\n",
	 status);
	    
    printf ("\n\nResult of pack/unpack sqlResultStruct_PF: \n");
    printf ("result_count = %d, row_count = %d, continuation_index = %d\n",
     myoutresult->result_count, myoutresult->row_count, 
      myoutresult->continuation_index);
    myoutresult->sqlresult[0].tab_name = dcs_tname[DATA_GRP_NAME];
    myoutresult->sqlresult[0].att_name = dcs_aname[DATA_GRP_NAME];
    myoutresult->sqlresult[1].tab_name = dcs_tname[DATA_NAME];
    myoutresult->sqlresult[1].att_name = dcs_aname[DATA_NAME];
    myoutresult->sqlresult[2].tab_name = dcs_tname[SIZE];
    myoutresult->sqlresult[2].att_name = dcs_aname[SIZE];

    for (i = 0; i < myoutresult->result_count; i++) {
	printf ("tab_name = %s, att_name = %s\n", 
	 myoutresult->sqlresult[i].tab_name, 
	  myoutresult->sqlresult[i].att_name);
	tmpPtr = myoutresult->sqlresult[i].values;
        for (j = 0; j < myoutresult->row_count; j++) {
            printf ("value = %s   ", tmpPtr);
            tmpPtr += MAX_DATA_SIZE;
        }
	printf ("\n");
    }

    /* testChar */

    testChar.row_count = ROW_COUNT;
    strcpy (testChar.myName1[0], "myName1 Row 0"); 
    strcpy (testChar.myName1[1], "myName1 Row 1");
    
    testChar.myName2 = malloc (ROW_COUNT * MAX_TOKEN);
    tmpPtr = testChar.myName2;
    strcpy (tmpPtr, "myName2 Row 0");
    tmpPtr += MAX_TOKEN;
    strcpy (tmpPtr, "myName2 Row 1");

    status = packMsg ((char *) &testChar, &outMsg, testChar_PF,
     LocalDef);
    if (status < 0)
        fprintf (stderr, "packMsg of testChar_PF error, status = %d\n",
         status);
    status = unpackMsg (VAROUTDATA(outMsg), (char **) &testCharOut,
      testChar_PF, LocalDef);

    if (status < 0)
        fprintf (stderr, "unpackMsg of testChar_PF error, status = %d\n",
         status);

    printf ("Result of testChar: row_count=%d\n", testCharOut->row_count);
    printf ("myName1: ");
    for (i = 0; i < testCharOut->row_count; i++) {
    	printf ("%s    ", testCharOut->myName1[i]);
    }
    printf ("\n");

    tmpPtr = testChar.myName2;
    printf ("myName2: ");
    for (i = 0; i < testCharOut->row_count; i++) {
        printf ("%s    ", tmpPtr);
    	tmpPtr += MAX_TOKEN;
    }
    printf ("\n");

    /* testInt */

    testInt.row_count = ROW_COUNT;
    testInt.col_count = COL_COUNT;

    for (i = 0; i < ROW_COUNT; i++) {
    	for (j = 0; j < COL_COUNT; j++) {
	    testInt.myInt1[i][j] = i*10 + j;
	}
    }

    testInt.myInt2 = malloc (ROW_COUNT * COL_COUNT * sizeof(int));
    tmpInt = testInt.myInt2;
    for (i = 0; i < ROW_COUNT; i++) {
    	for (j = 0; j < COL_COUNT; j++) {
	    *tmpInt = i*100 + j;
	    tmpInt++;
	}
    }

    status = packMsg ((char *) &testInt, &outMsg, testInt_PF,
     LocalDef);
    if (status < 0)
        fprintf (stderr, "packMsg of testInt_PF error, status = %d\n",
         status);
    status = unpackMsg (VAROUTDATA(outMsg), (char **) &testIntOut,
      testInt_PF, LocalDef);

    if (status < 0)
        fprintf (stderr, "unpackMsg of sqlResultStruct_PF error, status = %d\n",
         status);

    printf ("Result of testInt: row_count=%d, col_count=%d\n", 
     testIntOut->row_count, testIntOut->col_count);
    printf ("myInt1: ");
    for (i = 0; i < testIntOut->row_count; i++) {
    	for (j = 0; j < testIntOut->col_count; j++) {
            printf ("%d  ", testIntOut->myInt1[i][j]);
	}
    }
    printf ("\n");

    tmpInt = testIntOut->myInt2;
    printf ("myInt2: ");
    for (i = 0; i < testIntOut->row_count; i++) {
        for (j = 0; j < testIntOut->col_count; j++) {
            printf ("%d  ", *tmpInt);
	    tmpInt++;
        }
    }
    printf ("\n");

     /* testDouble */

    testDouble.row_count = ROW_COUNT;
    testDouble.col_count = COL_COUNT;

    for (i = 0; i < ROW_COUNT; i++) {
    	for (j = 0; j < COL_COUNT; j++) {
	    testDouble.myDouble1[i][j] = i*10 + j;
	}
    }

    testDouble.myDouble2 = malloc (
     ROW_COUNT * COL_COUNT * sizeof(double));
    tmpDouble = testDouble.myDouble2;
    for (i = 0; i < ROW_COUNT; i++) {
    	for (j = 0; j < COL_COUNT; j++) {
	    *tmpDouble = i*100 + j;
	    tmpDouble++;
	}
    }

    status = packMsg ((char *) &testDouble, &outMsg, testDouble_PF,
     LocalDef);
    if (status < 0)
        fprintf (stderr, "packMsg of testDouble_PF error, status = %d\n",
         status);
    status = unpackMsg (VAROUTDATA(outMsg), (char **) &testDoubleOut,
      testDouble_PF, LocalDef);

    if (status < 0)
        fprintf (stderr, "unpackMsg of sqlResultStruct_PF error, status = %d\n",
         status);

    printf ("Result of testDouble: row_count=%d, col_count=%d\n",
     testDoubleOut->row_count, testDoubleOut->col_count);
    printf ("myDouble1: ");
    for (i = 0; i < testDoubleOut->row_count; i++) {
        for (j = 0; j < testDoubleOut->col_count; j++) {
            printf ("%f  ", testDoubleOut->myDouble1[i][j]);
        }
    }
    printf ("\n");

    tmpDouble = testDoubleOut->myDouble2;
    printf ("myDouble2: ");
    for (i = 0; i < testDoubleOut->row_count; i++) {
        for (j = 0; j < testDoubleOut->col_count; j++) {
            printf ("%f  ", *tmpDouble);
            tmpDouble++;
        }
    }
    printf ("\n");


    exit(0);
}
