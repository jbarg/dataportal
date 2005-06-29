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
#include <netdb.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

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

typedef struct buffer_desc_struct {
  size_t length;
  void *value;
} buffer_desc, *buffer_t;

void usage()
{
    printf("Usage: aids2 [-p port] [-p2 port2] [-server2DN dnOfServer] [-host2 2ndHostName] [-msg message] [-k/g/b/d] name \n");
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
     /* Establish a context with the client with accepting delegate
     i = aid_establish_context_serverside_with_delegate(s, clientName, 200, client_type_flag);
     if (i < 0) {
printf("error %d from aid_establish_context_serverside\n",i);
       return(-1);
     }
     
     */
     /* Establish a context with the client */
     i = aid_establish_context_serverside(s, clientName, 200, client_type_flag);
     if (i < 0) {
		printf("error %d from aid_establish_context_serverside\n",i);
       return(-1);
     }
     
     /*     aid_display_ctx_flags();       */
     printf("Accepted connection from: \"%s\"\n", clientName);

     do {
       i = do_read(s);
     } while (i==1);

 		/*accepting delegation */
             
             if (aid_accept_delegation(s) == 0) {
             	printf("aid_accept_delegation() - delelgation accepted \n");
             	
             	/* debugging for delegation */
             	/* display remaining life of delegated cert */
             	printf(" requesting for delegate certificate's remaining lifetime \n");
             	int life=aid_get_remaining_delegate_lifetime(s);
             	printf("remaining life : %i \n",life);
             
             
             	printf("exporting delegate certificate\n");
             	if (aid_export_delegate(s) == 0)
             		printf("succeeded exporting delegate certificate\n");
             
             	printf("importing delegate certificate \n");
             	if (aid_import_delegate(s) == 0)
             		printf("succeeded importing delegate certificate \n");
             		
                life=aid_get_remaining_delegate_lifetime(s);
             	printf("remaining life : %i \n",life);
             }
             
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

/* client functions */
/*
 * Function: connect_to_server
 *
 * Purpose: Opens a TCP connection to the name host and port.
 *
 * Arguments:
 *
 * 	host		(r) the target host name
 * 	port		(r) the target port, in host byte order
 *
 * Returns: the established socket file desciptor, or -1 on failure
 *
 * Effects:
 *
 * The host name is resolved with gethostbyname(), and the socket is
 * opened and connected.  If an error occurs, an error message is
 * displayed and -1 is returned.
 */
int connect_to_server(host, port)
     char *host;
     u_short port;
{
     struct sockaddr_in saddr;
     struct hostent *hp;
     int s2;
     
     if ((hp = gethostbyname(host)) == NULL) {
	  fprintf(stderr, "Unknown host: %s\n", host);
	  return -1;
     }
      
     saddr.sin_family = hp->h_addrtype;
     memcpy((char *)&saddr.sin_addr, hp->h_addr, sizeof(saddr.sin_addr));
     saddr.sin_port = htons(port);

     if ((s2 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	  perror("creating socket");
	  return -1;
     }
     
     if (connect(s2, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
	  perror("connecting to server");
	  (void) close(s2);
	  return -1;
     }
     return s2;
}


/*
 * Function: call_server 
 * to call the second server using delegate credential
 * host - hostname
 * port - portnumber
 * service_name - DN of server
 * deleg_flag - delegation y/n
 * msg - string to be transmitted
 */
int call_server(host2, port2, service_name, deleg_flag, prev_s, msg)
     char *host2;
     u_short port2;
     char *service_name;
     int deleg_flag;
     int prev_s;
     char *msg;

{
     buffer_desc in_buf;
     char out_buf[20];
     int s2;
     int i;
     int type_flag=AID_GSI;
     char server2Name[200];

     /* Open connection */
     if ((s2 = connect_to_server(host2, port2)) < 0)
	  return -1;

     out_buf[0]=AID_GSI;
     i = write(s2,out_buf,1);
     if (i!=1) printf("error writing type byte\n");


     /* Establish context */
     if (aid_establish_context_using_client_delegate(s2, NULL/*service_name*/, server2Name, 200, prev_s, deleg_flag) < 0) {
	  (void) close(s2);
	  return -1;
     }
     printf (" established context using delegate certificate \n");
	 printf(" Accepted connection2 from: \"%s\"\n", server2Name);
     /* display the flags */
	 /*  aidi_display_ctx_flags();  */

	 in_buf.value = msg;
	 in_buf.length = strlen(msg) + 1;
     
 /* Seal and send the message */
i = aid_write(s2,in_buf.value,in_buf.length);
if (i < 0) {return i;}
	 
     aid_close(s2);

     (void) close(s2);
     
     return 0;
}


int
main(argc, argv)
     int argc;
     char **argv;
{
     char *service_name, *service_name2 , *server_host2, *msg;
     u_short port = 31001;
     u_short port2 = 31002;
     int s;
     int once = 0;
     int stmp;
     int type_both=0;
     char server2Name[200];

     argc--; argv++;
     while (argc) {
      if (strcmp(*argv, "-p") == 0) {
           argc--; argv++;
           if (!argc) usage();
           port = atoi(*argv);
      } else if (strcmp(*argv, "-p2") == 0) {
           argc--; argv++;
           if (!argc) usage();
           port2 = atoi(*argv);       
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
      } else if (strcmp(*argv, "-server2DN") == 0) {
      		argc--; argv++;
      		if (!argc) usage();
      		service_name2 = *argv;      		
      		printf("service_name2 : %s \n", service_name2);
      } else if (strcmp(*argv, "-host2") == 0) {
      		argc--; argv++;
      		if (!argc) usage();
      		server_host2 = *argv;
      		printf("server_host2 : %s \n", server_host2);
      }else if (strcmp(*argv, "-msg") == 0) {
      		argc--; argv++;
      		if (!argc) usage();
      		msg = *argv;
      		printf("msg : %s \n", msg);
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
             
             /*calling second server using delegate*/
             printf("calling second server using delegated proxy \n");

             if(call_server(server_host2, port2, service_name2, 1, s, msg) != 0 )
             	printf("unable to invoke server2 using delegate proxy \n");
	     	aid_close(s);
             }
         
      } while (!once);
    }

     close(stmp);
	printf("aid_close_all \n");
     (void) aid_close_all();

     /*NOTREACHED*/
     (void) close(s);
     return 0;
}

