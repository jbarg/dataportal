/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * testexf.c--
 *    test using physical file API
 *
 *-------------------------------------------------------------------------
 */

#include "srbClient.h"
#ifdef ACL_TEST
#include <sys/acl.h>
#endif

#define BUFSIZE		262144
#define srbAuth		NULL
#define UNIX_AM		0	/* UNIX access method */
#define UTREE_AM	1
#define HPSS_AM		2
#define HOST_ADDR	NULL

#define UNIX_HOST	"ghidorah.sdsc.edu"

#define HPSS_HOST	"mda-18.sdsc.edu"

char *inFileName[] = {
	"../../testfiles/test_in",
	"../../testfiles/test_in",
	"../../testfiles/test_in"
};

char *rescHost[] = { UNIX_HOST, 
		     "", 
		     HPSS_HOST
};

char *outFileName[] = {
	"/projects/mdas/srb/SRBVault/srb.sdsc/test_out",
	"test_out",
	"/users/sdsc/srb/SRBVault/srb.sdsc/test_out"
	
};

char *mkdirDir[] = {
        "/projects/mdas/srb/SRBVault/srb.sdsc/testdir",
        "testdir",
        "/users/sdsc/srb/SRBVault/srb.sdsc/testdir"
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
    srbConn *conn;
    srbResult *res;
    int i, nbytes, tmp, in_fd, out_fd;
    char buf[BUFSIZE];
    struct srbStat *statbuf;
    int acc_method, num_of_loop, loop_cnt;
    struct vaultQueElement *vaultQueHead;
    int nVault;
    int dirDesc;
    struct srbDirent *srbDirent;
#ifdef ACL_TEST
    aclent_t aclbufp[10];
#endif

    if (argc != 2 && argc != 3) {
        fprintf(stderr, "Usage: %s access_method(0, 1 or 2)  num_of_loop\n",
                argv[0]);
        exit(1);
    }
    acc_method = atoi (argv[1]);

    if (argc == 3)
        num_of_loop = atoi (argv[2]);
    else
        num_of_loop = 1;

    statbuf = (struct srbStat *) malloc (sizeof (struct srbStat));

#ifdef ACL_TEST
    i = acl (inFileName[acc_method], GETACLCNT, 10, aclbufp);
#endif

    /*
     * set up the connection
     */

    conn = clConnect (HOST_ADDR, NULL, srbAuth);

    /* check to see that the backend connection was successfully made */
    if (clStatus(conn) != CLI_CONNECTION_OK) {
	fprintf(stderr,"Connection to exfMaster failed.\n");
	fprintf(stderr,"%s",clErrorMessage(conn));
	exit_nicely(conn);
    }
	
    /* get and print the vault info */

    vaultQueHead = srbVaultInfo(conn);
    if (vaultQueHead != NULL) {
    	srbPrintVaultInfo (vaultQueHead);
    	srbFreeVaultInfo (vaultQueHead);
    } else {
	fprintf(stderr,"%s",clErrorMessage(conn));
	printf ("There is no VaultInfo\n");
    }

    for (loop_cnt = 0; loop_cnt < num_of_loop; loop_cnt++) {

	printf ("Process loop %d of %d\n\n", loop_cnt, num_of_loop);

    	/* Delete the mkdirDir */

    	printf ("removing dir: %s\n", mkdirDir[acc_method]);
    	if (srbFileRmdir(conn, acc_method, rescHost[acc_method], 
	 mkdirDir[acc_method]) != 0) {
            fprintf(stderr,"Unable to rmdir %s\n", mkdirDir[acc_method]);
            fprintf(stderr,"%s",clErrorMessage(conn));
    	}

        /* mkdirDir */
 
        printf ("making dir: %s\n", mkdirDir[acc_method]);
        if (srbFileMkdir(conn, acc_method, rescHost[acc_method], 
	 mkdirDir[acc_method], 0777) != 0) {
            fprintf(stderr,"Unable to mkdir %s\n", mkdirDir[acc_method]);
            fprintf(stderr,"%s",clErrorMessage(conn));
        }

        /* chmod mkdirDir */
 
        printf ("About to chmod dir: %s\n", mkdirDir[acc_method]);
        if (srbFileChmod(conn, acc_method, rescHost[acc_method], 
	 mkdirDir[acc_method], 0755) != 0) {
            fprintf(stderr,"Unable to chmod %s\n", mkdirDir[acc_method]);
            fprintf(stderr,"%s",clErrorMessage(conn));
        }

    	/* Delete the output files */

    	printf ("Unlinking file: %s\n", outFileName[acc_method]);
    	if (srbFileUnlink(conn, acc_method, rescHost[acc_method], 
	 outFileName[acc_method]) != 0) {
            fprintf(stderr,"Unable to unlink %s\n", outFileName[acc_method]);
            fprintf(stderr,"%s",clErrorMessage(conn));
    	}

    	printf ("Reading %s and writing %s\n",
	inFileName[acc_method], outFileName[acc_method]);

    	in_fd = open (inFileName[acc_method], O_RDONLY, 0);
    	if (in_fd < 0)  {   /* error */
            fprintf(stderr, "can't open file\"%s\"\n", 
	    inFileName[acc_method]);
	    exit_nicely(conn);
    	}
    	out_fd = srbFileCreate (conn, acc_method, rescHost[acc_method], 
	outFileName[acc_method], 0666, -1);
    	if (out_fd < 0)  {   /* error */
            fprintf(stderr, "can't create ufile\"%s\", status = %d.\n", 
	    outFileName[acc_method], out_fd);
	    exit_nicely(conn);
    	}

    	i = 0;
    	while ((nbytes = read(in_fd, buf, BUFSIZE)) > 0) {
	    i += nbytes;
            tmp = srbFileWrite(conn, out_fd, buf, nbytes);
            if (tmp < nbytes) {
            	fprintf(stderr, "Error: Read %d bytes, Wrote %d bytes.\n ", 
	    	nbytes, tmp);
	    	exit_nicely(conn);
            }
    	}
    	printf ("Bytes written to %s = %d\n", outFileName[acc_method], i);

        close(in_fd);

    	srbFileClose (conn, out_fd);

    	printf ("Doing a stat of %s \n", outFileName[acc_method]);

    	i = srbFileStat (conn, acc_method, rescHost[acc_method], 
	outFileName[acc_method], statbuf);
    	if (i < 0)  {   /* error */
            fprintf(stderr, "can't stat file\"%s\"\n", outFileName[acc_method]);
    	} else {
	    printf ("Stat of %s:\n\n", outFileName[acc_method]);
            printf ("st_dev=%x \n", statbuf->st_dev);
            printf ("st_ino=%x \n", statbuf->st_ino);
            printf ("st_mode=%x \n", statbuf->st_mode);
            printf ("st_nlink=%d \n", statbuf->st_nlink);
            printf ("st_uid=%d \n", statbuf->st_uid);
            printf ("st_gid=%d \n", statbuf->st_gid);
            printf ("st_rdev=%x \n", statbuf->st_rdev);
            printf ("st_size=%lld \n", statbuf->st_size);
            printf ("st_atim=%d \n", statbuf->st_atim);
            printf ("st_spare1=%d \n", statbuf->st_spare1);
            printf ("st_mtim=%d \n", statbuf->st_mtim);
            printf ("st_spare2=%d \n", statbuf->st_spare2);
            printf ("st_ctim=%d \n", statbuf->st_ctim);
            printf ("st_spare3=%d \n", statbuf->st_spare3);
            printf ("st_blksize=%d \n", statbuf->st_blksize);
            printf ("st_blocks=%d \n", statbuf->st_blocks);
            printf ("st_flag=%x \n", statbuf->st_flag);
            printf ("st_gen=%x \n", statbuf->st_gen);
    	}
    }
    clFinish(conn);
    exit(0);
}
