/* sgetdents.c - List the content of a  collection 
 * int srbObjGetdents (srbConn* conn, int catType, int fd, dirent_t *buf,
 * size_t nbyte)
 * int srbObjStat(srbConn* conn, int catType, char *path, struct stat *statbuf)
 */

#include <stdio.h>
#include <sys/file.h>
#include <sys/stat.h>
#if defined(PORTNAME_c90) || defined(PORTNAME_solaris)
#include <sys/fcntl.h> 
#endif
#include <unistd.h>

#include "srbClient.h"

#define HOST       	NULL
#define PORT            NULL
#define SRB_AUTH        NULL	/* For 'PASSWD_AUTH', 'ENCRYPT1' */
#define USER            NULL
#define DOMAIN          NULL
#define AUTH_SCHEME     NULL	/* 'PASSWD_AUTH', 'ENCRYPT1', 'GSI_AUTH' */
#define SERVER_DN       NULL

#define DATATYPE "ascii text"
#define RESOURCE "unix-sdsc"

#define NAME_LEN 256
#define DENT_LEN	4096
int
main(int argc, char **argv)
{
    srbConn *conn;
    srbResult *res;
    int retVal; 
    int rFlag = 0;
    int c;

    while ((c=getopt(argc, argv,"r")) != EOF) {
        switch (c) {
            case 'r':
		rFlag = 1;
		break;
            default:
		fprintf(stderr, "Usage: %s [-r] srb-collection\n",argv[0]);
                exit (1);
        }
    }

    if (optind != argc - 1) {
	fprintf(stderr, "Usage: %s [-r] srb-collection\n",argv[0]);
        exit(1);
    }

    conn = srbConnect (HOST, PORT, SRB_AUTH, USER, DOMAIN, AUTH_SCHEME, 
      SERVER_DN);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Connection to srbMaster failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
        srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        clFinish (conn);
        exit (1);
    }

    retVal = listcollContent (conn, argv[optind], rFlag);

    clFinish(conn);
    exit (0);
}

int 
listcollContent (srbConn *conn, char *collection, int rFlag)
{
    char myPath[NAME_LEN];
    struct stat64 statbuf;
    char mydentbuf[DENT_LEN];
    int fd;
    int len;
    int retVal;
    int status = 0;

    fd = srbObjOpen (conn, "", O_RDONLY, collection);

    if (fd < 0) {
        fprintf(stderr, "Unable to open %s, stat = %d\n",collection, fd);
        exit(1);
    }

    fd = fd & (~IS_COLL_FD);    /* strip out the IS_COLL_FD bit */


    printf ("\n**** Content of collection: %s\n\n", collection);

    while ((len = srbObjGetdents64 (conn, MDAS_CATALOG, fd, 
     (dirent64_t *) mydentbuf, DENT_LEN)) > 0) {
	dirent64_t *tmpdent;
	char *tmpcp;
	int tmplen = 0; 

	tmpcp = mydentbuf;

        while (tmplen < len) {
	    tmpdent = (dirent64_t *) tmpcp;

	    if (strcmp (tmpdent->d_name, ".") == 0 ||
	     strcmp (tmpdent->d_name, "..") == 0) {
	        printf ("special dir: %s\n", tmpdent->d_name);
	    } else {
		sprintf (myPath, "%s/%s", collection, tmpdent->d_name);
                retVal = srbObjStat64 (conn, MDAS_CATALOG, myPath, 
		 &statbuf);
                if (retVal < 0) { /* error */
                    fprintf(stderr, "can't stat srb file %s, status = %d\n", 
    	             myPath, retVal);
                    fprintf(stderr,"%s",clErrorMessage(conn));
                }
    	        if ((statbuf.st_mode & S_IFREG) != 0) {     /* A file */
                    printf ("%s a file type with size = %lld\n", 
		     myPath, statbuf.st_size);
    	        } else {
                    printf ("%s is a directory\n", myPath);
		    if (rFlag > 0) {
			retVal = listcollContent (conn, myPath, rFlag);
			if (retVal < 0) {
			    fprintf (stderr, "listcollContent of %s error\n",
			     myPath);
			    status = retVal;
			}
		    }
	        }
	    }
	    tmplen += tmpdent->d_reclen;
	    tmpcp += tmpdent->d_reclen;
	}
    }
    srbObjClose (conn, fd);
    return (status);
}
