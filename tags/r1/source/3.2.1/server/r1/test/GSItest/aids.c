#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <stdlib.h>
#include <ctype.h>

#ifdef USE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif

/* #include "aid.h"  just do it:*/
#define AID_Kerberos 1
#define AID_GSI 2
#define AID_DCE 3
/* end aid.h  */

int type_flag=AID_GSI;

void usage()
{
    printf("Usage: aids [-p port] [-k/g/b/d] name \n");
    exit(1);
}

/*
 * Function: create_socket
 *
 * Purpose: Opens a listening TCP socket.
 *
 * Arguments:
 *
 *     port        (r) the port number on which to listen
 *
 * Returns: the listening socket file descriptor, or -1 on failure
 *
 * Effects:
 *
 * A listening socket on the specified port and created and returned.
 * On error, an error message is displayed and -1 is returned.
 */
int create_socket(port)
     u_short port;
{
     struct sockaddr_in saddr;
     int s;
     int on = 1;
     
     saddr.sin_family = AF_INET;
     saddr.sin_port = htons(port);
     saddr.sin_addr.s_addr = INADDR_ANY;

     if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("creating socket");
      return -1;
     }
     /* Let the socket be reused right away */
     (void) setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
     if (bind(s, (struct sockaddr *) &saddr, sizeof(saddr)) < 0) {
      perror("binding socket");
fprintf(stderr,"port=%d\n",port);
      (void) close(s);
      return -1;
     }
     if (listen(s, 5) < 0) {
      perror("listening on socket");
      (void) close(s);
      return -1;
     }
     return s;
}

/*
 * Function: sign_server
 *
 * Purpose: Performs the "sign" service.
 *
 * Arguments:
 *
 *     s        (r) a TCP socket on which a connection has been
 *            accept()ed
 *     service_name    (r) the ASCII name of the GSS-API service to
 *             establish a context as
 * 
 * Returns: -1 on error
 *
 * Effects:
 *
 * sign_server establishes a context, and performs a single sign request.
 *
 * A sign request is a single GSS-API sealed token.  The token is
 * unsealed and a signature block, produced with gss_sign, is returned
 * to the sender.  The context is the destroyed and the connection
 * closed.
 *
 * If any error occurs, -1 is returned.
 */
int sign_server(s)
     int s;
{
     char clientName[200];
     char readbuffer[8];
     int client_type_flag;

     int i;

     i = read(s, readbuffer, 1);
     if (i!=1) {
         printf("did not receive type byte\n");
     }
     client_type_flag = 0;
     client_type_flag = (int) readbuffer[0];
     printf("client_type_flag=%d\n",client_type_flag);

// --debug
     /* Establish a context with the client with accepting delegate */
     i = aid_establish_context_serverside_with_delegate(s, clientName, 200, client_type_flag);
     if (i < 0) {
printf("error %d from aid_establish_context_serverside\n",i);
       return(-1);
     }
     
     
     /* Establish a context with the client 
     i = aid_establish_context_serverside(s, clientName, 200, client_type_flag);
     if (i < 0) {
printf("error %d from aid_establish_context_serverside\n",i);
       return(-1);
     }
     */
     /*     aid_display_ctx_flags();       */
     printf("Accepted connection from: \"%s\"\n", clientName);

     do {
       i = do_read(s);
     } while (i==1);

 		/*accepting delegation 
             
             if (aid_accept_delegation(s) == 0) {
             	printf("aid_accept_delegation() - delelgation accepted \n");
             	
             	/* debugging for delegation 
             	/* display remaining life of delegated cert 
             	printf(" requesting for delegate certificate's remaining lifetime \n");
             	int life=aid_get_remaining_delegate_lifetime(s);
             	printf("remaining life : %i \n",life);
             
             	printf("exporting delegate certificate\n");
             	if (aid_export_delegate(s) == 0)
             		printf("succeeded exporting delegate certificate\n");
             
             	life=aid_get_remaining_delegate_lifetime(s);
             	printf("remaining life : %i \n",life);
             
             	printf("importing delegate certificate \n");
             	if (aid_import_delegate(s) == 0)
             		printf("succeeded importing delegate certificate \n");
             		
            
             }
             */
             
     return(0);
}

int 
do_read(s)
int s;
{
#define RB_LEN 8000
     char readbuffer[RB_LEN];
     int i;
     char    *cp;

     i = aid_read(s, readbuffer, RB_LEN);
     printf("aid_read value=%d\n",i);
     if (i < 0) {return i;}

     printf("Received message: ");
     cp = readbuffer;
     if (isprint(cp[0]) && isprint(cp[1]) && i<20)
        printf("\"%s\"\n", cp);
     else {
        printf("%d bytes received\n",i);
     }

     if (i==RB_LEN) {  /* exact size, try to read more */
        return 1;
     }
     return 0;
}


int
main(argc, argv)
     int argc;
     char **argv;
{
     char *service_name;
     u_short port = 31001;
     int s;
     int once = 0;
     int stmp;
     int type_both=0;

     argc--; argv++;
     while (argc) {
      if (strcmp(*argv, "-p") == 0) {
           argc--; argv++;
           if (!argc) usage();
           port = atoi(*argv);
      } else if (strcmp(*argv, "-o") == 0) {
          once = 1;
      } else if (strcmp(*argv, "-k") == 0) {
          type_flag = AID_Kerberos;
      } else if (strcmp(*argv, "-g") == 0) {
          type_flag = AID_GSI;
      } else if (strcmp(*argv, "-d") == 0) {
          type_flag = AID_DCE;
      } else if (strcmp(*argv, "-b") == 0) {
          type_both = 1;
      } else
           break;
      argc--; argv++;
     }
     if (argc != 1) {
       if (argc == 0)  {
         service_name = NULL;
       } else
         usage();
     } else
       service_name = *argv;

     if (type_both==0) {
        if (aid_setup_creds(service_name,0,type_flag) < 0)  return -1; 
     }
     else {
        if (aid_setup_creds("/C=US/O=NPACI/OU=SDSC/CN=host/makalu.sdsc.edu",
                0,AID_GSI) < 0)  return -1;

        printf("done with setup_creds for GSI\n");
/*
        if (aid_setup_creds("srb",0,AID_Kerberos) < 0)  return -1; 

        printf("done with setup_creds for Kerberos\n");
*/
     }


    if ((stmp = create_socket(port))>0) {
printf("done with create\n");
      do {
printf("waiting for a connection\n");
         /* Accept a TCP connection */
         if ((s = accept(stmp, NULL, 0)) < 0) {
             perror("accepting connection");
         } else {
             /* this return value is not checked, because there's
            not really anything to do if it fails */
            printf("s %d", s);
			printf("calling sign_server\n");
             sign_server(s);
             
	     	aid_close(s);
             }
         
      } while (!once);
    }

     close(stmp);

     (void) aid_close_all();

     /*NOTREACHED*/
     (void) close(s);
     return 0;
}

