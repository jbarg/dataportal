#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

/* #include "aid.h"  just do it:*/
#define AID_Kerberos 1
#define AID_GSI 2
#define AID_DCE 3
/* end aid.h  */

int type_flag = AID_GSI;

typedef struct buffer_desc_struct {
  size_t length;
  void *value;
} buffer_desc, *buffer_t;

void usage()
{
     fprintf(stderr, "Usage: aidc [-port port] [-k] [-d] [-f] host service msg_or_filename\n");
     exit(1);
}

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
     int s;
     
     if ((hp = gethostbyname(host)) == NULL) {
	  fprintf(stderr, "Unknown host: %s\n", host);
	  return -1;
     }
     
     saddr.sin_family = hp->h_addrtype;
     memcpy((char *)&saddr.sin_addr, hp->h_addr, sizeof(saddr.sin_addr));
     saddr.sin_port = htons(port);

     if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	  perror("creating socket");
	  return -1;
     }
     if (connect(s, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
	  perror("connecting to server");
	  (void) close(s);
	  return -1;
     }
     return s;
}

void read_file(file_name, in_buf)
    char		*file_name;
    buffer_t	in_buf;
{
    int fd, bytes_in, count;
    struct stat stat_buf;
    
    if ((fd = open(file_name, O_RDONLY, 0)) < 0) {
	perror("open");
	fprintf(stderr, "Couldn't open file %s\n", file_name);
	exit(1);
    }
    if (fstat(fd, &stat_buf) < 0) {
	perror("fstat");
	exit(1);
    }
    in_buf->length = stat_buf.st_size;
    in_buf->value = malloc(in_buf->length);
    if (in_buf->value == 0) {
	fprintf(stderr, "Couldn't allocate %d byte buffer for reading file\n",
		in_buf->length);
	exit(1);
    }
    memset(in_buf->value, 0, in_buf->length);
    for (bytes_in = 0; bytes_in < in_buf->length; bytes_in += count) {
	count = read(fd, in_buf->value, in_buf->length);
	if (count < 0) {
	    perror("read");
	    exit(1);
	}
	if (count == 0)
	    break;
    }
    if (bytes_in != count)
	fprintf(stderr, "Warning, only read in %d bytes, expected %d\n",
		bytes_in, count);
}

/*
 * Function: call_server
 *
 * Purpose: Call the "sign" service.
 *
 * Arguments:
 *
 * 	host		(r) the host providing the service
 * 	port		(r) the port to connect to on host
 * 	service_name	(r) the GSS-API service name to authenticate to	
 * 	msg		(r) the message to have "signed"
 *
 * Returns: 0 on success, -1 on failure
 *
 * Effects:
 * 
 * call_server opens a TCP connection to <host:port> and establishes a
 * GSS-API context with service_name over the connection.  It then
 * seals msg in a GSS-API token with gss_seal, sends it to the server,
 * reads back a GSS-API signature block for msg from the server, and
 * verifies it with gss_verify.  -1 is returned if any step fails,
 * otherwise 0 is returned.
 */
int call_server(host, port, service_name, deleg_flag, msg, use_file, use_encrypt)
     char *host;
     u_short port;
     char *service_name;
     int deleg_flag;
     char *msg;
     int use_file;
     int use_encrypt;
{
     buffer_desc in_buf;
     char out_buf[20];
     int s;
     int i;
     char serverName[200];
     
     /* Open connection */
     if ((s = connect_to_server(host, port)) < 0)
	  return -1;

     out_buf[0]=AID_GSI;
     if (type_flag==AID_Kerberos) out_buf[0]=AID_Kerberos;
     if (type_flag==AID_DCE) out_buf[0]=AID_DCE;
     i = write(s,out_buf,1);
     if (i!=1) printf("error writing type byte\n");

	//-debug
     /* Establish context */
     if (aid_establish_context_clientside_without_server_dn(s, serverName, 200, deleg_flag, type_flag) < 0) {
	  (void) close(s);
	  return -1;
     }
     /* Establish context 
     if (aid_establish_context_clientside(s, service_name, deleg_flag, type_flag) < 0) {
	  (void) close(s);
	  return -1;
     }
     */
     /* display the flags */
    //      aid_display_ctx_flags(); 
 	printf(" Accepted connection from: \"%s\"\n", serverName);
 	
     if (use_file) {
	 read_file(msg, &in_buf);
     } else {
	 in_buf.value = msg;
	 in_buf.length = strlen(msg) + 1;
     }

 /* Seal and send the message */
i = aid_write(s,in_buf.value,in_buf.length);
if (i<0) {return i;}

     if (use_file)
	 free(in_buf.value);

 	/* initializing delegation inputs fd, minutes for validity  */
     
	printf ("requesting delegation \n");
	  if (aid_init_delegation(s,10) < 0) 
	  	printf("delegation initialization failed \n");
	printf("delegated credential to server \n");
     
	
     aid_close_all();

     (void) close(s);
     return 0;
}




int main(argc, argv)
     int argc;
     char **argv;
{
     char *service_name, *server_host, *msg;
     char *mechanism = 0;
     /*     u_short port = 4444; */
     u_short port = 31001;
     int use_file = 0;
	 int use_encrypt = 0;
     int deleg_flag=0;
     int debug_flag=0;
     
     /* Parse arguments. */
     argc--; argv++;
     while (argc) {
	  if (strcmp(*argv, "-port") == 0) {
	       argc--; argv++;
	       if (!argc) usage();
	       port = atoi(*argv);
	   } else if (strcmp(*argv, "-mech") == 0) {
	       argc--; argv++;
	       if (!argc) usage();
	       mechanism = *argv;
	   } else if (strcmp(*argv, "-d") == 0) {
	        deleg_flag = 1; 
	       /* type_flag = AID_DCE;*/
	   } else if (strcmp(*argv, "-k") == 0) {
	       type_flag = AID_Kerberos;
	   } else if (strcmp(*argv, "-D") == 0) {
	       debug_flag++;
	       aid_debug(debug_flag);
	  } else if (strcmp(*argv, "-f") == 0) {
	       use_file = 1;
	  } else if (strcmp(*argv, "-e") == 0) {
	       use_encrypt = 1;
	  } else 
	       break;
	  argc--; argv++;
     }
     if (argc != 3)
	  usage();

     server_host = *argv++;
     service_name = *argv++;
     msg = *argv++;

/*
  Can't be called ahead of time like this for Kerberos GSSAPI.
*/

     printf("calling aid_setup_creds\n");
     if (aid_setup_creds(0,1,type_flag) < 0)
       return -1;
     printf("aid_setup_creds completed successfully\n");

     if (call_server(server_host, port, service_name, 
		     deleg_flag, msg, use_file, use_encrypt) < 0)
	  exit(1);
	  

	  

     return 0;
}


