
/* Copyright   1999   The Regents of the University of California
 * All Rights Reserved
 *
 * Permission to use, copy, modify and distribute any part of this
 * Storage Resource Broker (SRB) software package, for educational,
 * research and non-profit purposes, without fee, and without a
 * written agreement is hereby granted, provided that the above
 * copyright notice, this paragraph and the following three paragraphs
 * appear in all copies.  Those desiring to incorporate this SRB
 * software into commercial products or use for commercial purposes
 * should contact the Technology Transfer Office, University of
 * California, San Diego, 9500 Gilman Drive, La Jolla, CA 92093-0910,
 * Ph: (619) 534-5815, FAX: (619) 534-7345.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY
 * PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
 * DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SRB
 * SOFTWARE, EVEN IF THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE SRB SOFTWARE PROVIDED HEREIN IS ON AN "AS IS" BASIS, AND THE
 * UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE,
 * SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.  THE UNIVERSITY
 * OF CALIFORNIA MAKES NO REPRESENTATIONS AND EXTENDS NO WARRANTIES OF
 * ANY KIND, EITHER IMPLIED OR EXPRESS, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A
 * PARTICULAR PURPOSE, OR THAT THE USE OF THE SRB SOFTWARE WILL NOT
 * INFRINGE ANY PATENT, TRADEMARK OR OTHER RIGHTS.  */

/*

 ---------------
 This AID library is now being integrated into the SRB source tree
 rather than maintained as a separate library so that we can better
 support GSI.  In the process, aid.c has been modified a little to fit
 (GSI for #ifdefs is now GSI_AUTH) into the SRB build environment and
 the SRB build environment modified to make it easier to configure and
 build with GSI.  Only GSI portion of this has been tested.  And this
 was with the current GSI version, 2.2.4.  The Kerberos and DCE code
 has been left here for now, but isn't being used.  We could convert
 this to use the globus_gss_assist API but that is only simplify it
 slightly.

 Wayne Schroeder
 3/2003
 ---------------

 LIBAID  Authentication and Integrity of Data interface

 This is an interface library to GSS-API security services, providing
 a simple API to calling applications.  It is to be used in the SDSC
 Storage Resource Broker (client/server) application in conjunction
 with the Globus GSS-API implementation on top of SSLeay (see
 www.globus.org).  X.509 Certificates are used for user and server
 authentication.  Data can be exchanged in a secure manner with data
 signing (not encryption).

 It also is used for Kerberos authentication/data integrity also via
 the GSS-API.  So that both Kerberos and SSL versions of GSS-API can
 be linked at the same time, the krb5_gss* routines (or
 generic_gss_release_buffer) are called for Kerberos instead of the
 gss_* routines.  The gss_* routines are the SSL GSS-API routines, the
 krb5_gss* routines are the Kerberos GSS-API routines.  Fortunately,
 in the Kerberos GSS-API package, each gss_* routine calls a krb5_gss*
 counterpart with the same arguments, making this kind of blending
 possible.

 Allocation of potentially large data buffers is left to the calling
 application rather than done in this library or below.  This is
 sometimes important in High Performance Computing as the arrays can
 get large and applications need to control the memory allocation.
 This is accomplished primarily by using gss_get_mic/gss_verify_mic
 instead of gss_wrap and gss_unwrap.  Some malloc's of small buffers
 are also avoided for efficiency and simplicity (matching the large
 buffers), although there is some risk that the scratch buffer may,
 someday, be too small.

 Two defines control whether this package makes GSI and/or Kerberos
 calls (GSI and Kerberos), so it can be built to link with either
 one or both.

 A third define (DCE) controls whether this builds for the DCE
 environment (and an earlier version of GSS-API).  We have not
 attempted to link DCE simultaneous with either Kerberos or GSI.

 The aid_setup_creds has a flag for type (AID_Kerberos, AID_GSI,
 AID_DCE) and so does each aid_establish_context.  For aid_read and
 aid_write this package knows which type of context has been
 established for the specified socket.

 The aid_read routine will malloc a (potentially large) buffer if the
 application's isn't big enough.  This allows aid_read to behave like
 the read system call and cache data if the app's buffer is too small.
 Currently it prints a message for each malloc so that we can tune
 applications (the SRB) to avoid a lot of malloc's.  I plan to make
 this (printing these messages) to be under the control of the
 application.
 
 Application callable routines:
   
   aid_setup_creds(service_name, client_flag, type_flag)
      reads user or server X.509 certificate/private key, or Kerberos 
      credential locally; server/client flag indicates if this is a server
      or client; type_flag indicates Kerberos, GSI, or DCE.

      For Kerberos servers, environment variable KRB5_KTNAME is the
      Keytab file for the server (handled in Kerberos layers).

      For DCE servers, environment variable DCE_KTNAME is the
      Keytab file for the server (handled at this layer).

   aid_establish_context_serverside(s,clientName,max_len_clientName,type_flag)
      mutual secure authentication across a socket, called by a server; 
      also prepares for possible use of aid_read/aid_write for data 
      integrity checking

   aid_establish_context_clientside(s, service_name, deleg_flag, type_flag)
      mutual authentication across a socket, called by client; prepare for
      possible use of aid_read/aid_write for data integrity checking

   aid_read(fd,buffer,length)
      like read system call, across a socket, but confirms that data
      originated from the authenticated party on the other side and has
      not been altered in transit.

   aid_write(fd,buffer,length) 
      like write system call, across a socket, adds MIC (Message Integrity 
      Check, an MD5 hash of data and session key, etc) for use by aid_read.

   aid_close(fd)
      clear context information from memory.

   aid_debug(val)
      adjust verbose level for debug messages.

------------------------------------------------------------------------------
New Functions added for delegation are :

aid_establish_context_serverside_with_delegate(fd, clientName, 
             max_len_clientName, type_flag)
        Function to be invoked by server when accepting delegation
        from client.  Complements
        aid_establish_context_serverside(). Possibly in the future it
        could be merged into one with an additional parameter
        deleg_flag.
        
aid_establish_context_clientside_without_server_dn(fd, serverName,
             max_len_serverName, deleg_flag, type_flag)
        Function to be invoked by client while establishing connection
        with sever.  Similar to aid_establish_context_clientside() but
        is not necessary to provide Server DN and rather returns
        Server Dn in serverName.
        
aid_establish_context_using_client_delegate(fd, server_dn_in, server_dn_out, 
             max_len_server_dn_out, received_delegation_fd, deleg_flag)
        Function used to establish connection using client's delegated
        certificate rather than using server's own certificate. The
        receiving end could either use
        aid_establish_context_serverside() if no delegation or
        otherwise use aid_establish_context_serverside_with_delegate()
        if deleg_flag is set to 1.
         
aid_get_remaining_delegate_lifetime(int fd)
        returns remaining lifetime of the delegated certificate.

aid_init_delegation(int fd, OM_uint32 delegation_time )
        function to initiate delegation after an context has already
        been established
                
aid_accept_delegation(int fd)
        function to accept delegation over an connection that is
        already established.
        
aid_export_delegate(int fd)
        function can be used to serialize credential to /tmp for future 
        retrieval or by other process that have same privilege.

aid_import_delegate(int fd)
        function to import credential that has been exported 
        by export_credential function.         

int aid_establish_context_serverside_consolidated(fd, clientName,
             max_len_clientName, deleg_flag, type_flag)
         Can be used in the future in place of both
         aid_establish_context_serverside() and
         aid_establish_context_serverside_with_delegate() with
         addition of deleg_flag if 1 then delegation is to be accepted
         or else not to be accepted.
         
Ananta 02/04
------------------------------------------------------------------------------

 All application callable routines are named aid_*.
 Internal routines are named aidi_*.

 In case of errors, messages are printed to stderr (both AID and GSS
 level) and negative values returned.


 Loosely based on GSS-API examples developed by Openvision distributed
 with Kerberos.

 With the addition of Kerberos, and then DCE, the number of ifdefs has
 gotten excessive in places and makes the code somewhat hard to read.
 But I believe it is best to leave it this way, since the calls and flow
 are quite similar across the three.

 Wayne Schroeder
 San Diego Supercomputer Center
 http://www.sdsc.edu/~schroede

 */

/* bypass everything if we are doing nothing */

#if defined(DCE) || defined(GSI_AUTH) || defined(Kerberos)

#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <stdlib.h>
#include <ctype.h>

#ifdef DCE
#include <dce/gssapi.h>
#else
#include <gssapi.h>
#endif

#include <stdio.h>
#include <sys/time.h>           /* for AID_TIMING only */
/* #define AID_TIMING 1 */

/* #include "aid.h"  just do it:*/
#define AID_Kerberos 1
#define AID_GSI 2
#define AID_DCE 3
/* end aid.h  */

int aidi_send_token(int fd, gss_buffer_t tok);
int aidi_rcv_token(int fd, gss_buffer_t tok);
void aidi_display_status
    (char *msg, OM_uint32 maj_stat, OM_uint32 min_stat, int type_flag);
void aidi_print_token(gss_buffer_t tok);
int aid_get_remaining_delegate_lifetime(int fd);

#if defined(AID_TIMING)
void aidi_tvsub(struct timeval *tdiff, struct timeval *t1,
                struct timeval *t0);
#endif

extern gss_OID gss_nt_service_name;

/* #define gss_nt_service_name_gsi GSS_C_NT_HOSTBASED_SERVICE 
 Changed to 0 for GSI 0.3.0 10/12/1999 */
#define gss_nt_service_name_gsi 0

#ifdef USE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif

#define SCRATCH_BUFFER_SIZE 8000
char aidi_scratch_buffer[SCRATCH_BUFFER_SIZE];

static int aidi_debug = 0;

static int aidi_token_header_mode = 1;  /* 1 is the normal mode, 
 0 means running in a non-token-header mode, ie Java; dynamically cleared. */

static gss_cred_id_t my_creds_gsi_dce = GSS_C_NO_CREDENTIAL;
static gss_cred_id_t my_creds_krb = GSS_C_NO_CREDENTIAL;

#define MAX_FDS 32
static gss_ctx_id_t context[MAX_FDS] = {
    GSS_C_NO_CONTEXT, GSS_C_NO_CONTEXT, GSS_C_NO_CONTEXT, GSS_C_NO_CONTEXT,
    GSS_C_NO_CONTEXT, GSS_C_NO_CONTEXT, GSS_C_NO_CONTEXT, GSS_C_NO_CONTEXT,
    GSS_C_NO_CONTEXT, GSS_C_NO_CONTEXT, GSS_C_NO_CONTEXT, GSS_C_NO_CONTEXT,
    GSS_C_NO_CONTEXT, GSS_C_NO_CONTEXT, GSS_C_NO_CONTEXT, GSS_C_NO_CONTEXT,
    GSS_C_NO_CONTEXT, GSS_C_NO_CONTEXT, GSS_C_NO_CONTEXT, GSS_C_NO_CONTEXT,
    GSS_C_NO_CONTEXT, GSS_C_NO_CONTEXT, GSS_C_NO_CONTEXT, GSS_C_NO_CONTEXT,
    GSS_C_NO_CONTEXT, GSS_C_NO_CONTEXT, GSS_C_NO_CONTEXT, GSS_C_NO_CONTEXT,
    GSS_C_NO_CONTEXT, GSS_C_NO_CONTEXT, GSS_C_NO_CONTEXT, GSS_C_NO_CONTEXT
};

static int type_fd[MAX_FDS] = {
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0
};

static char *in_cache_buf[MAX_FDS] = {
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0
};

static int in_cache_buf_state[MAX_FDS] = {
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0
};

static int in_cache_buf_len[MAX_FDS];
static int in_cache_buf_write_index[MAX_FDS];
static int in_cache_buf_read_index[MAX_FDS];

/*for delegation */
static gss_cred_id_t delegated_cred_gsi[MAX_FDS] = {
    GSS_C_NO_CREDENTIAL, GSS_C_NO_CREDENTIAL, GSS_C_NO_CREDENTIAL,
    GSS_C_NO_CREDENTIAL, GSS_C_NO_CREDENTIAL, GSS_C_NO_CREDENTIAL,
    GSS_C_NO_CREDENTIAL, GSS_C_NO_CREDENTIAL, GSS_C_NO_CREDENTIAL,
    GSS_C_NO_CREDENTIAL, GSS_C_NO_CREDENTIAL, GSS_C_NO_CREDENTIAL,
    GSS_C_NO_CREDENTIAL, GSS_C_NO_CREDENTIAL, GSS_C_NO_CREDENTIAL,
    GSS_C_NO_CREDENTIAL, GSS_C_NO_CREDENTIAL, GSS_C_NO_CREDENTIAL,
    GSS_C_NO_CREDENTIAL, GSS_C_NO_CREDENTIAL, GSS_C_NO_CREDENTIAL,
    GSS_C_NO_CREDENTIAL, GSS_C_NO_CREDENTIAL, GSS_C_NO_CREDENTIAL,
    GSS_C_NO_CREDENTIAL, GSS_C_NO_CREDENTIAL, GSS_C_NO_CREDENTIAL,
    GSS_C_NO_CREDENTIAL, GSS_C_NO_CREDENTIAL, GSS_C_NO_CREDENTIAL,
    GSS_C_NO_CREDENTIAL, GSS_C_NO_CREDENTIAL
};

static gss_buffer_t deleg_proxy_filename[MAX_FDS] = {
    GSS_C_NO_BUFFER, GSS_C_NO_BUFFER, GSS_C_NO_BUFFER, GSS_C_NO_BUFFER,
    GSS_C_NO_BUFFER, GSS_C_NO_BUFFER, GSS_C_NO_BUFFER, GSS_C_NO_BUFFER,
    GSS_C_NO_BUFFER, GSS_C_NO_BUFFER, GSS_C_NO_BUFFER, GSS_C_NO_BUFFER,
    GSS_C_NO_BUFFER, GSS_C_NO_BUFFER, GSS_C_NO_BUFFER, GSS_C_NO_BUFFER,
    GSS_C_NO_BUFFER, GSS_C_NO_BUFFER, GSS_C_NO_BUFFER, GSS_C_NO_BUFFER,
    GSS_C_NO_BUFFER, GSS_C_NO_BUFFER, GSS_C_NO_BUFFER, GSS_C_NO_BUFFER,
    GSS_C_NO_BUFFER, GSS_C_NO_BUFFER, GSS_C_NO_BUFFER, GSS_C_NO_BUFFER,
    GSS_C_NO_BUFFER, GSS_C_NO_BUFFER, GSS_C_NO_BUFFER, GSS_C_NO_BUFFER
};

static gss_buffer_desc deleg_proxy_filename_buffer = GSS_C_EMPTY_BUFFER;

/* end delegation */

#ifndef DCE
static OM_uint32 context_flags;
#else
int context_flags;
#endif

/* #define OBJ_CREATE_NEEDED  -- no longer needed */
#define OBJ_CREATE_NEEDED 1     /* 1/7/2000, I thought it wouldn't be needed
                                   with the new GSI, but it seems it is */
static int aid_obj_created = 0;

static void aidi_display_status_1
    (char *m, OM_uint32 code, int type, int type_flag);

/* Function for time test 
 * Returns the difference between start time and end time in tdiff 
 * struct in seconds */
#if defined(AID_TIMING)
void aidi_tvsub(struct timeval *tdiff, struct timeval *endTime,
                struct timeval *startTime)
{
    tdiff->tv_sec = endTime->tv_sec - startTime->tv_sec;
    tdiff->tv_usec = endTime->tv_usec - startTime->tv_usec;
    if (tdiff->tv_usec < 0) {
        tdiff->tv_sec--;
        tdiff->tv_usec += 1000000;
    }                           /* end_if */
}                               /* end_function aidi_tvsub() */
#endif

/*
 Check that the input type_flag (Kerberos, GSI, or DCE) is one that
 the current build of the library supports.
 */
int aidi_check_type(type_flag)
int type_flag;
{
#ifndef Kerberos
    if (type_flag == AID_Kerberos) {
        fprintf(stderr, "AID Library not built to support Kerberos\n");
        return -2;
    }
#endif
#ifndef GSI_AUTH
    if (type_flag == AID_GSI) {
        fprintf(stderr, "AID Library not built to support GSI\n");
        return -2;
    }
#endif
#ifndef DCE
    if (type_flag == AID_DCE) {
        fprintf(stderr, "AID Library not built to support DCE\n");
        return -2;
    }
#endif
    return 0;
}

/*
 Write a buffer to the network, continuing with subsequent writes if
 the write system call returns with only some of it sent.
 */
static int aidi_write_all(int fd, char *buf, unsigned int nbyte)
{
    int ret;
    char *ptr;

    for (ptr = buf; nbyte; ptr += ret, nbyte -= ret) {
        ret = write(fd, ptr, nbyte);
        if (ret < 0) {
            if (errno == EINTR)
                continue;
            return (ret);
        } else if (ret == 0) {
            return (ptr - buf);
        }
    }
    if (aidi_debug > 0)
        fprintf(stderr, "aidi_write_all, wrote=%d\n", ptr - buf);
    return (ptr - buf);
}

/* 
 Read a buffer, continuing until it is filled.
 */
static int aidi_read_all(int fd, char *buf, unsigned int nbyte)
{
    int ret;
    char *ptr;

    for (ptr = buf; nbyte; ptr += ret, nbyte -= ret) {
        ret = read(fd, ptr, nbyte);
        if (ret < 0) {
            if (errno == EINTR)
                continue;
            return (ret);
        } else if (ret == 0) {
            return (ptr - buf);
        }
    }

    return (ptr - buf);
}

/*
 Send a token (which is a buffer and a length); 
 write the token length (as a network long) and then the
 token data on the file descriptor.  It returns 0 on success, and
 -1 if an error occurs or if it could not write all the data.
*/
int aidi_send_token(fd, tok)
int fd;
gss_buffer_t tok;
{
    int len, ret;
    char *cp;

    if (aidi_debug > 0)
        fprintf(stderr, "sending tok->length=%d\n", tok->length);

    if (aidi_token_header_mode) {
        len = htonl(tok->length);

        cp = (char *) &len;
        if (sizeof(len) > 4)
            cp += sizeof(len) - 4;
        ret = aidi_write_all(fd, cp, 4);
        if (ret < 0) {
            perror("sending token length");
            return -1;
        } else if (ret != 4) {
            if (aidi_debug > 1)
                fprintf(stderr,
                        "sending token length: %d of %d bytes written\n",
                        ret, 4);
            return -1;
        }
    }

    ret = aidi_write_all(fd, tok->value, tok->length);
    if (ret < 0) {
        perror("sending token data");
        return -1;
    } else if (ret != tok->length) {
        fprintf(stderr,
                "sending token data: %d of %d bytes written\n",
                ret, tok->length);
        return -1;
    }

    return 0;
}


/*
 read the token hdr
 */

int aidi_rcv_token_hdr(fd)
int fd;
{
    int length;
    char *cp;
    int ret;

    length = 0;
    cp = (char *) &length;
    if (sizeof(length) > 4)
        cp += sizeof(length) - 4;
    ret = aidi_read_all(fd, cp, 4);
    if (ret < 0) {
        perror("reading token length");
        return -1;
    } else if (ret != 4) {
        if (ret == 0)
            return 0;
        fprintf(stderr,
                "reading token length: %d of %d bytes read\n", ret, 4);
        return -1;
    }

    length = ntohl(length);

    if (aidi_debug > 0)
        fprintf(stderr, "token length = %d\n", length);

    return length;
}

/*
 read the token body
 */
int aidi_rcv_token_body(fd, tok, length)
int fd;
gss_buffer_t tok;
int length;
{
    int ret;

    if (tok->length < length) {
        fprintf(stderr, "aidi_rcv_token error, token is too large for buffer,\
 %d bytes in token, buffer is %d bytes\n",
                length, tok->length);
        return -1;
    }

    if (tok->value == NULL) {
        if (aidi_debug > 1)
            fprintf(stderr, "Bad token passed to aidi_rcv_token\n");
        return -1;
    }

    tok->length = length;

    ret = aidi_read_all(fd, (char *) tok->value, tok->length);
    if (ret < 0) {
        perror("reading token data");
        return -1;
    } else if (ret != tok->length) {
        fprintf(stderr, "reading token data: %d of %d bytes written\n",
                ret, tok->length);
        return -1;
    }

    return tok->length;
}

/* 
 Read a token from a socket (file descriptor): read the token length
 (as a network long), and then read the token data from the file
 descriptor.  It blocks to read the length and data, if necessary.

 Divided into two routines so that each can be called from aid_read.
 */
int aidi_rcv_token(fd, tok)
int fd;
gss_buffer_t tok;
{
    int length;
    int i;

    int tmpLength;
    char *cp;

    if (aidi_token_header_mode) {
        /*
           First, if in normal mode, peek to see if the other side is sending
           headers and possibly switch into non-header mode.
         */
        tmpLength = 0;
        cp = (char *) &tmpLength;
        if (sizeof(tmpLength) > 4)
            cp += sizeof(tmpLength) - 4;
        i = recv(fd, cp, 4, MSG_PEEK);
        tmpLength = ntohl(tmpLength);
        if (aidi_debug > 0)
            fprintf(stderr, "peek length = %d\n", tmpLength);
        if (tmpLength > 100000) {
            aidi_token_header_mode = 0;
            if (aidi_debug > 0)
                fprintf(stderr, "switching to non-hdr mode\n");
        }
    }


    if (aidi_token_header_mode) {
        length = aidi_rcv_token_hdr(fd);
        if (length <= 0)
            return length;

        return (aidi_rcv_token_body(fd, tok, length));
    } else {
        i = read(fd, (char *) tok->value, tok->length);
        if (aidi_debug > 0)
            fprintf(stderr, "rcved token, length = %d\n", i);
        if (i <= 0)
            return (i);
        tok->length = i;        /* Assume all of token is rcv'ed */
        return 1;               /* success */
    }
}


/*
 Call GSS routines to print the current (error) status.
 */
static void aidi_display_status_1(char *m, OM_uint32 code, int type,
                                  int type_flag)
{
    OM_uint32 maj_stat, min_stat;
    gss_buffer_desc msg;
#ifndef DCE
    OM_uint32 msg_ctx;
#else
    int msg_ctx;
#endif

    msg_ctx = 0;
    while (1) {
        if (type_flag == AID_Kerberos) {
#ifdef Kerberos
            maj_stat = krb5_gss_display_status(&min_stat, code,
                                               type, GSS_C_NULL_OID,
                                               &msg_ctx, &msg);
#endif
        } else {
#if defined(GSI_AUTH) || defined(DCE)
            maj_stat = gss_display_status(&min_stat, code,
                                          type, GSS_C_NULL_OID,
                                          &msg_ctx, &msg);
#endif
        }
        fprintf(stderr, "GSS-API error %s: %s\n", m, (char *) msg.value);
        if (type_flag == AID_Kerberos) {
#ifdef Kerberos
            (void) generic_gss_release_buffer(&min_stat, &msg);
#endif
        } else {
#if defined(GSI_AUTH) || defined(DCE)
            (void) gss_release_buffer(&min_stat, &msg);
#endif
        }
        if (!msg_ctx)
            break;
    }
}

/*

Display an error message followed by GSS-API messages.
 */

void aidi_display_status(char *msg, OM_uint32 maj_stat, OM_uint32 min_stat,
                         int type_flag)
{
    fprintf(stderr, "Error %s\n", msg);
    aidi_display_status_1(msg, maj_stat, GSS_C_GSS_CODE, type_flag);
    aidi_display_status_1(msg, min_stat, GSS_C_MECH_CODE, type_flag);
}

/*
Display the context flags.
 */

void aidi_display_ctx_flags()
{
    if (context_flags & GSS_C_DELEG_FLAG)
        fprintf(stdout, "context flag: GSS_C_DELEG_FLAG\n");
    if (context_flags & GSS_C_MUTUAL_FLAG)
        fprintf(stdout, "context flag: GSS_C_MUTUAL_FLAG\n");
    if (context_flags & GSS_C_REPLAY_FLAG)
        fprintf(stdout, "context flag: GSS_C_REPLAY_FLAG\n");
    if (context_flags & GSS_C_SEQUENCE_FLAG)
        fprintf(stdout, "context flag: GSS_C_SEQUENCE_FLAG\n");
    if (context_flags & GSS_C_CONF_FLAG)
        fprintf(stdout, "context flag: GSS_C_CONF_FLAG \n");
    if (context_flags & GSS_C_INTEG_FLAG)
        fprintf(stdout, "context flag: GSS_C_INTEG_FLAG \n");
}

/*
 Print the contexts of a token (for debug).
*/

void aidi_print_token(tok)
gss_buffer_t tok;
{
    int i, j;
    unsigned char *p = tok->value;
    fprintf(stderr, "aidi_print_token, length=%d\n", tok->length);
    j = 0;
    for (i = 0; i < tok->length; i++, p++) {
        if (i < 16 || i > tok->length - 16) {
            fprintf(stderr, "%02x ", *p);
            if ((i % 16) == 15) {
                fprintf(stderr, "\n");
            }
        } else {
            j++;
            if (j < 4)
                fprintf(stderr, ".");
            if (j == 4)
                fprintf(stderr, "\n");
        }
    }
    fprintf(stderr, "\n");
    fflush(stderr);
}


/*

 Setup for GSSAPI functions, for either client or server side.

 If this is not called, the GSS routines will call the equivalent
 during the establish_context (aid_establish_context_serverside or
 aid_establish_context_clientside).

 In some cases, the GSS routines will prompt for a password to
 decrypt the local private key.

 This calls gss_import_name to convert input string (if not NULL) to
 an internal representation (if it is NULL, a default is taken).  Then
 the credentials are acquired (read into memory and converted) via
 gss_acquire_cred (for SSL this is the certificate (with public key)
 and the private key (hence the password to decrypt).  

 If successful, credentials are established in the global my_creds.
 If unsuccessful, the GSS-API error messages are displayed on stderr
 and -1 is returned. 

 */
int aid_setup_creds(service_name, client_flag, type_flag)
char *service_name;
int client_flag;
int type_flag;
{
    gss_buffer_desc name_buf;
    gss_name_t my_name = GSS_C_NO_NAME;
    OM_uint32 maj_stat, min_stat;

#if defined(AID_TIMING)
    float fSec;
    struct timeval startTimeFunc, endTimeFunc, sTimeDiff;
    /* Starting timer for function */
    (void) gettimeofday(&startTimeFunc, (struct timezone *) 0);

#endif

    if (aidi_check_type(type_flag))
        return (-2);

    if (service_name != NULL) {
        name_buf.value = service_name;
        name_buf.length = strlen(name_buf.value) + 1;
        if (type_flag == AID_Kerberos) {
#ifdef Kerberos
            maj_stat = krb5_gss_import_name(&min_stat, &name_buf,
                                            (gss_OID) gss_nt_service_name,
                                            &my_name);
#endif
        } else {
#ifdef GSI_AUTH
            maj_stat = gss_import_name(&min_stat, &name_buf,
                                       (gss_OID) gss_nt_service_name_gsi,
                                       &my_name);
#endif
#ifdef DCE
            maj_stat = gss_import_name(&min_stat, &name_buf,
                                       NULL, &my_name);

#endif
        }
        if (maj_stat != GSS_S_COMPLETE) {
            aidi_display_status("acquiring credentials", maj_stat,
                                min_stat, type_flag);
            return -1;
        }
    }
#ifdef DCE
    if (service_name != NULL) {
        char *keytab_filename;
        int warn = 0;
        keytab_filename = getenv("DCE_KTNAME");
        if (keytab_filename == 0) {
            keytab_filename = "/tmp/ktbfile";
            warn = 1;
        }
        maj_stat = gssdce_register_acceptor_identity(&min_stat, my_name,
                                                     NULL,
                                                     keytab_filename);
        if (maj_stat != GSS_S_COMPLETE) {
            if (warn == 1)
                fprintf(stderr, "Unable to convert DCE_KTNAME \
environment variable to valid keytab filename\n");
            aidi_display_status("gssdce_register", maj_stat, min_stat,
                                type_flag);
        }
    }
#endif

#if defined(Kerberos) || defined(DCE)
    /* For kerberos (and I believe DCE), we can't do acquire ahead of
       time for clients (but need to for servers). 
     */
    if (client_flag == 1
        && (type_flag == AID_Kerberos || type_flag == AID_DCE))
        return 0;
#endif

    if (type_flag == AID_Kerberos) {
#ifdef Kerberos
        maj_stat = krb5_gss_acquire_cred(&min_stat, my_name, 0,
                                         GSS_C_NULL_OID_SET, GSS_C_ACCEPT,
                                         &my_creds_krb, NULL, NULL);
#endif
    } else {
#if defined(GSI_AUTH) || defined(DCE)
        maj_stat = gss_acquire_cred(&min_stat, my_name, 0,
                                    GSS_C_NULL_OID_SET, GSS_C_ACCEPT,
                                    &my_creds_gsi_dce, NULL, NULL);
#endif
    }
    if (maj_stat != GSS_S_COMPLETE) {
        aidi_display_status("acquiring credentials", maj_stat, min_stat,
                            type_flag);
        return -1;
    }
    if (type_flag == AID_Kerberos) {
#ifdef Kerberos
        (void) krb5_gss_release_name(&min_stat, &my_name);
#endif
    } else {
#if defined(GSI_AUTH) || defined(DCE)
        (void) gss_release_name(&min_stat, &my_name);
#endif
    }

#if defined(GSI_AUTH) && defined(OBJ_CREATE_NEEDED)
    /*
       Found that if this is at the beginning of this routine and
       this routine is called for both Kerberos and GSI, then the
       Object seems to be forgotten (subsequent calls to use the
       Obj return a number in the DN, altho another OBJ_create gets
       a higher number).  But if we call OBJ_create after
       doing the GSI calls, it seems to stick.

       Changed this (and other blocks like this) to repeatedly create
       the object (up to 10 times).  Previously, this was a one-time
       flag but then it would sometimes not take.

     */
    if (aid_obj_created < 10 && type_flag == AID_GSI) {
        int obj_cre_val;
        /* create the SDSC UID object (in the running version of SSL) */
        obj_cre_val =
            OBJ_create("0.9.2342.19200300.100.1.1", "USERID", "userId");
        /* fprintf(stderr,"setup_creds obj_cre_val = %d\n",obj_cre_val); */
        aid_obj_created++;
    }
#endif

#if defined(AID_TIMING)

    (void) gettimeofday(&endTimeFunc, (struct timezone *) 0);
    (void) aidi_tvsub(&sTimeDiff, &endTimeFunc, &startTimeFunc);
    fSec =
        (float) sTimeDiff.tv_sec + ((float) sTimeDiff.tv_usec / 1000000.0);

    /* for AID_TIMING  print time */
    fprintf(stdout, " ---  %.5f sec time taken for executing"
            "aid_setup_creds() ---  \n", fSec);
#endif
    return 0;
}

/*
 Set up an session between this server and a connected new client.
 
 Establishses a GSS-API context (as the service specified in
 aid_setup_creds) with an incoming client, and returns the
 authenticated client name (the id on the other side of the socket).
 The other side (the client) must call
 aid_establish_context_clientside at about the same time for the
 exchanges across the network to work (each side will block waiting
 for the other).  

 If successful, the context handle is set in the global context array.
 If unsuccessful, an error message is displayed and -1 is returned.

 */
int aid_establish_context_serverside(fd, clientName, max_len_clientName,
                                     type_flag)
int fd;
char *clientName;
int max_len_clientName;
int type_flag;
{
    gss_buffer_desc send_buffer, recv_buffer;
    gss_buffer_desc client_name;
    gss_name_t client;
    gss_OID doid;
    OM_uint32 maj_stat, min_stat;
    gss_buffer_desc oid_name;
    int i;
#if defined(AID_TIMING)
    struct timeval startTimeFunc, endTimeFunc, sTimeDiff;
    float fSec;
    /* Starting timer for function */
    (void) gettimeofday(&startTimeFunc, (struct timezone *) 0);

#endif

#if defined(GSI_AUTH) && defined(OBJ_CREATE_NEEDED)
    if (aid_obj_created < 10) {
        int obj_cre_val;
        /* create the SDSC UID object (in the running version of SSL) */
        obj_cre_val =
            OBJ_create("0.9.2342.19200300.100.1.1", "USERID", "userId");
        /*fprintf(stderr,"obj_cre_val = %d\n",obj_cre_val); */
        aid_obj_created++;
    }
#endif

    if (aidi_check_type(type_flag))
        return (-2);

    context[fd] = GSS_C_NO_CONTEXT;

    recv_buffer.value = &aidi_scratch_buffer;

    do {
        recv_buffer.length = SCRATCH_BUFFER_SIZE;
        if (aidi_rcv_token(fd, &recv_buffer) <= 0) {
            fprintf(stderr,
                "aid_establish_context_serverside: error receiving token\n");
            return -1;
        }
        if (aidi_debug > 0) {
            fprintf(stderr, "Received token (size=%d): \n",
                    recv_buffer.length);
            aidi_print_token(&recv_buffer);
        }

        if (type_flag == AID_Kerberos) {
#ifdef Kerberos
            maj_stat = krb5_gss_accept_sec_context(&min_stat, 
                           &context[fd], my_creds_krb, &recv_buffer, 
                           GSS_C_NO_CHANNEL_BINDINGS, &client, &doid, 
                           &send_buffer, &context_flags, 
                           NULL,     /* ignore time_rec */
                           NULL);    /* ignore del_cred_handle */
#endif
        } else {
#if defined(GSI_AUTH) || defined(DCE)
            maj_stat = gss_accept_sec_context(&min_stat, 
                           &context[fd], my_creds_gsi_dce, &recv_buffer, 
                           GSS_C_NO_CHANNEL_BINDINGS, &client, &doid, 
                           &send_buffer, &context_flags, 
                           NULL,     /* ignore time_rec */
                           NULL);    /* ignore del_cred_handle */
#endif
        }

        if (maj_stat != GSS_S_COMPLETE
            && maj_stat != GSS_S_CONTINUE_NEEDED) {
            aidi_display_status("accepting context", maj_stat, min_stat,
                                type_flag);
            bzero(aidi_scratch_buffer, SCRATCH_BUFFER_SIZE);
            return -1;
        }

        /* since recv_buffer is not malloc'ed, don't need to call
           gss_release_buffer, instead clear it. */
        bzero(aidi_scratch_buffer, SCRATCH_BUFFER_SIZE);

        if (send_buffer.length != 0) {
            if (aidi_debug > 0) {
                fprintf(stderr,
                        "Sending accept_sec_context token (size=%d):\n",
                        send_buffer.length);
                aidi_print_token(&send_buffer);
            }
            if (aidi_send_token(fd, &send_buffer) < 0) {
                fprintf(stderr, "failure sending token\n");
                return -1;
            }
            if (type_flag == AID_Kerberos) {
#ifdef Kerberos
                (void) generic_gss_release_buffer(&min_stat, &send_buffer);
#endif
            } else {
#if defined(GSI_AUTH) || defined(DCE)
                (void) gss_release_buffer(&min_stat, &send_buffer);
#endif
            }
        }
        if (aidi_debug > 0) {
            if (maj_stat == GSS_S_CONTINUE_NEEDED)
                fprintf(stderr, "continue needed...\n");
        }
    } while (maj_stat == GSS_S_CONTINUE_NEEDED);

    /* convert name (internally represented) to a string */
    if (type_flag == AID_Kerberos) {
#ifdef Kerberos
        maj_stat =
            krb5_gss_display_name(&min_stat, client, &client_name, &doid);
#endif
    } else {
#if defined(GSI_AUTH) || defined(DCE)
        maj_stat =
            gss_display_name(&min_stat, client, &client_name, &doid);
#endif
    }
    if (maj_stat != GSS_S_COMPLETE) {
        aidi_display_status("displaying name", maj_stat, min_stat,
                            type_flag);
        return -1;
    }

    i = client_name.length;
    if (max_len_clientName < i)
        i = max_len_clientName;

    strncpy(clientName, client_name.value, i);
    if (max_len_clientName > client_name.length)
        clientName[client_name.length] = '\0';

    /* release the name structure */
    if (type_flag == AID_Kerberos) {
#ifdef Kerberos
        maj_stat = krb5_gss_release_name(&min_stat, &client);
#endif
    } else {
#if defined(GSI_AUTH) || defined(DCE)
        maj_stat = gss_release_name(&min_stat, &client);
#endif
    }

    if (maj_stat != GSS_S_COMPLETE) {
        aidi_display_status("releasing name", maj_stat, min_stat,
                            type_flag);
        return -1;
    }

    if (type_flag == AID_Kerberos) {
#ifdef Kerberos
        (void) generic_gss_release_buffer(&min_stat, &client_name);
#endif
    } else {
#if defined(GSI_AUTH) || defined(DCE)
        (void) gss_release_buffer(&min_stat, &client_name);
#endif
    }

    type_fd[fd] = type_flag;    /* record the type (GSI/Krb) for this fd */

#if defined(AID_TIMING)

    (void) gettimeofday(&endTimeFunc, (struct timezone *) 0);
    (void) aidi_tvsub(&sTimeDiff, &endTimeFunc, &startTimeFunc);
    fSec =
        (float) sTimeDiff.tv_sec + ((float) sTimeDiff.tv_usec / 1000000.0);

    /* for AID_TIMING  print time */
    fprintf(stdout, " ---  %.5f sec time taken for executing "
            "aid_establish_context_serverside() ---  \n", fSec);
#endif

    return 0;
}

/*
 Return data previous read
 */
int aidi_return_cached_data(fd, buffer, length)
int fd;
char *buffer;
int length;
{
    int mv, i;
    char *cp, *cp2;
    mv = in_cache_buf_write_index[fd] - in_cache_buf_read_index[fd];
    if (length < mv)
        mv = length;
    cp = buffer;
    cp2 = in_cache_buf[fd] + in_cache_buf_read_index[fd];
    for (i = 0; i < mv; i++)
        *cp++ = *cp2++;
    in_cache_buf_read_index[fd] += mv;
    if (in_cache_buf_read_index[fd] == in_cache_buf_write_index[fd])
        in_cache_buf_state[fd] = 0;
    return mv;
}

/*
 Read a buffer from the other side, verifying the integrity of the
 data.  It does this by reading the data, followed by the MIC, (each
 as tokens (with a length prepended), and then verifying the MIC.
 Error values are similar to read call, positive is number of bytes
 read, negative for an error, 0 if disconnected.

 */

int aid_read(fd, buffer, length)
int fd;
char *buffer;
int length;
{
    gss_buffer_desc recv_buf, mic_buf;
    OM_uint32 maj_stat, min_stat;
    int i;
#ifndef DCE
    gss_qop_t qop_state;
#else
    int qop_state;
#endif
    int tok_length;

/*
 First check to see if we've cached data from a previous read, if 
 so just return data from the buffer.
 */
    if (in_cache_buf_state[fd] != 0) {
        return (aidi_return_cached_data(fd, buffer, length));
    }

    recv_buf.value = buffer;
    recv_buf.length = length;

    if (aidi_token_header_mode) {
        tok_length = aidi_rcv_token_hdr(fd);
        if (tok_length <= 0)
            return tok_length;

        if (tok_length > length) {       /* user's buffer isn't large enough */
            if (in_cache_buf[fd] != 0) { /* already allocated */
                if (in_cache_buf_len[fd] <tok_length) {/* but not big enough */
                    if (aidi_debug > 0) {
                        fprintf(stderr,
                          "AID Info: freeing a buffer of %d bytes for fd %d\n",
                          in_cache_buf_len[fd], fd);
                    }
                    free(in_cache_buf[fd]);
                    in_cache_buf[fd] = 0;
                }
            }
            if (in_cache_buf[fd] == 0) {
                if (aidi_debug > 0) {
                    fprintf(stderr,
                       "AID Info: mallocing a buffer of %d bytes for fd %d\n",
                       tok_length, fd);
                }
                in_cache_buf[fd] = malloc(tok_length);
                in_cache_buf_len[fd] = tok_length;
            }
            in_cache_buf_read_index[fd] = 0;
            in_cache_buf_write_index[fd] = 0;
            in_cache_buf_state[fd] = 1;

            recv_buf.value = in_cache_buf[fd];
            recv_buf.length = tok_length;
        }

        i = aidi_rcv_token_body(fd, &recv_buf, tok_length);

    } else {

        i = read(fd, buffer, recv_buf.length);
        if (i <= 0)
            return (i);
        recv_buf.length = i;    /* Assume all of token is rcv'ed */
    }

    if (i <= 0)
        return (i);

    if (aidi_debug > 0) {
        fprintf(stderr, "Sealed message token:\n");
        aidi_print_token(&recv_buf);
    }

    /* Receive the sealed message MIC */
    mic_buf.value = &aidi_scratch_buffer;
    mic_buf.length = SCRATCH_BUFFER_SIZE;
    if (aidi_rcv_token(fd, &mic_buf) <= 0)
        return (-1);

    /* Verify signature block */
    if (type_fd[fd] == AID_Kerberos) {
#ifdef Kerberos
        maj_stat = krb5_gss_verify_mic(&min_stat, context[fd],
                                       &recv_buf, &mic_buf, &qop_state);
#endif
    } else {
#ifdef GSI_AUTH
        maj_stat = gss_verify_mic(&min_stat, context[fd],
                                  &recv_buf, &mic_buf, &qop_state);
#endif
#ifdef DCE
        maj_stat = gss_verify(&min_stat, context[fd],
                              &recv_buf, &mic_buf, &qop_state);
#endif
    }
    if (maj_stat != GSS_S_COMPLETE) {
        aidi_display_status("error verifying signiture", maj_stat,
                            min_stat, type_fd[fd]);
        bzero(aidi_scratch_buffer, SCRATCH_BUFFER_SIZE);
        (void) close(fd);
        if (type_fd[fd] == AID_Kerberos) {
#ifdef Kerberos
            (void) krb5_gss_delete_sec_context(&min_stat,
                                               &context[fd],
                                               GSS_C_NO_BUFFER);
#endif
        } else {
#if defined(GSI_AUTH) || defined(DCE)
            (void) gss_delete_sec_context(&min_stat,
                                          &context[fd], GSS_C_NO_BUFFER);
#endif
        }
        return -1;
    }

    /* since mic_buf is not malloc'ed, don't need to call
       gss_release_buffer, instead clear it. */
    bzero(aidi_scratch_buffer, SCRATCH_BUFFER_SIZE);

    if (aidi_debug > 0)
        fprintf(stderr, "Signature verified.\n");

    if (in_cache_buf_state[fd] == 1) {
        in_cache_buf_state[fd] = 2;
        in_cache_buf_write_index[fd] = recv_buf.length;
        return (aidi_return_cached_data(fd, buffer, length));
    }

    return (recv_buf.length);
}

/*
 Close out the context for the associated fd (socket).
 */
aid_close(fd)
int fd;
{
    OM_uint32 maj_stat, min_stat;

    if (in_cache_buf[fd] != 0) {        /* input cache allocated */
        if (aidi_debug > 0) {
            fprintf(stderr,
                    "AID Info: freeing a buffer of %d bytes for fd %d\n",
                    in_cache_buf_len[fd], fd);
        }
        free(in_cache_buf[fd]);
        in_cache_buf[fd] = 0;
        in_cache_buf_state[fd] = 0;
    }


    if (type_fd[fd] == AID_Kerberos) {
#ifdef Kerberos
        maj_stat =
            krb5_gss_delete_sec_context(&min_stat, &context[fd], NULL);
#endif
    } else {
#if defined(GSI_AUTH) || defined(DCE)
        maj_stat = gss_delete_sec_context(&min_stat, &context[fd], NULL);
#endif
    }

    if (maj_stat != GSS_S_COMPLETE && aidi_debug > 0) {
        aidi_display_status("deleting context", maj_stat, min_stat,
                            type_fd[fd]);
        return (-1);
    }

    /* verify delegated credential handle does not point to any credential */
    if (delegated_cred_gsi[fd] != GSS_C_NO_CREDENTIAL) {

        /* reset delegated credential handle */
        maj_stat = gss_release_cred(&min_stat, &delegated_cred_gsi[fd]);

        if (maj_stat != GSS_S_COMPLETE) {
            fprintf(stderr, "Unable to release delegated credential  \n");
            return -1;
        }
        /* not necessary as it is already done in library
           delegated_cred_gsi[fd] = GSS_C_NO_CREDENTIAL; */
    }
}

/*
 Close out all existing contextes and delete (erase) our credentials.
 */
aid_close_all()
{
    OM_uint32 maj_stat, min_stat;
    int i;

#ifdef Kerberos
    (void) krb5_gss_release_cred(&min_stat, &my_creds_krb);
#endif

#if defined(GSI_AUTH) || defined(DCE)
    if (my_creds_gsi_dce != 0) {   /* for DCE this test is needed since
                                      it is 0 and it prints an error message */
        maj_stat = gss_release_cred(&min_stat, &my_creds_gsi_dce);
        if (maj_stat != GSS_S_COMPLETE) {
            aidi_display_status("releasing cred", maj_stat, min_stat, 0);
        }
    }
#endif

    for (i = 0; i < MAX_FDS; i++) {
        if (context[i] != GSS_C_NO_CONTEXT) {
            aid_close(i);
        }
    }
}


/*
 Set up an session between this client and a connected server.
 
 Establishses a GSS-API context, confirming the identity of the server
 as matching the input service_name string.  Confirms the this
 process's (the client's) identity to the server (based on the id
 specified in aid_setup_creds).

 The other side (the client) must call
 aid_establish_context_serverside at about the same time for the
 exchanges across the network to work (each side will block waiting
 for the other).  If unsuccessful, an error message is displayed and
 -1 is returned.

 The service_name is imported as a GSS-API name (translated to
 internal format) and a GSS-API context is established with the
 corresponding service The default GSS-API mechanism is used, and
 mutual authentication and replay detection are requested.
 
 If successful, the context handle is set in the global context array.
 If unsuccessful, the GSS-API error messages are displayed on stderr
 and -1 is returned.  */

int aid_establish_context_clientside(fd, service_name, deleg_flag,
                                     type_flag)
int fd;
char *service_name;
int deleg_flag;
int type_flag;
{
    gss_OID oid = GSS_C_NULL_OID;
    gss_buffer_desc send_tok, recv_tok, *token_ptr, name_buffer;
    gss_name_t target_name;
    OM_uint32 maj_stat, min_stat;
    OM_uint32 flags = 0;

#if defined(AID_TIMING)
    struct timeval startTimeFunc, endTimeFunc, sTimeDiff;
    float fSec;
    /* Starting timer for function */
    (void) gettimeofday(&startTimeFunc, (struct timezone *) 0);

#endif

#if defined(GSI_AUTH) && defined(OBJ_CREATE_NEEDED)
    if (aid_obj_created < 10) {
        int obj_cre_val;
        /* create the SDSC UID object (in the running version of SSL) */
        obj_cre_val =
            OBJ_create("0.9.2342.19200300.100.1.1", "USERID", "userId");
        if (aidi_debug > 0)
            fprintf(stderr, "obj_cre_val = %d\n", obj_cre_val);
        aid_obj_created++;
    }
#endif

    if (aidi_check_type(type_flag))
        return (-2);

    /*
     * Import the name into target_name.
     */
    name_buffer.value = service_name;
    name_buffer.length = strlen(service_name) + 1;


    if (type_flag == AID_Kerberos) {
#ifdef Kerberos
        maj_stat = krb5_gss_import_name(&min_stat, &name_buffer,
                                        (gss_OID) gss_nt_service_name,
                                        &target_name);
#endif
    } else {
#ifdef GSI_AUTH
        maj_stat = gss_import_name(&min_stat, &name_buffer,
                                   (gss_OID) gss_nt_service_name_gsi,
                                   &target_name);
#endif
#ifdef DCE
        maj_stat = gss_import_name(&min_stat, &name_buffer,
                                   NULL, &target_name);
#endif
    }
    if (maj_stat != GSS_S_COMPLETE) {
        aidi_display_status
            ("importing name (aid_establish_context_clientside)", maj_stat,
             min_stat, type_flag);
    }
    /*
     * Perform the context-establishment loop.
     *
     * On each pass through the loop, token_ptr points to the token
     * to send to the server (or GSS_C_NO_BUFFER on the first pass).
     * Every generated token is stored in send_tok which is then
     * transmitted to the server; every received token is stored in
     * recv_tok, which token_ptr is then set to, to be processed by
     * the next call to gss_init_sec_context.
     * 
     * GSS-API guarantees that send_tok's length will be non-zero
     * if and only if the server is expecting another token from us,
     * and that gss_init_sec_context returns GSS_S_CONTINUE_NEEDED if
     * and only if the server has another token to send us.
     */

    token_ptr = GSS_C_NO_BUFFER;
    context[fd] = GSS_C_NO_CONTEXT;
    flags = GSS_C_MUTUAL_FLAG | GSS_C_REPLAY_FLAG;
    if (deleg_flag)
        flags |= GSS_C_DELEG_FLAG;

    do {
        if (aidi_debug > 0)
            fprintf(stderr, "--> calling gss_init_sec_context\n");
        if (type_flag == AID_Kerberos) {
#ifdef Kerberos
            maj_stat = krb5_gss_init_sec_context(&min_stat,
                      /* was: GSS_C_NO_CREDENTIAL,  */
                      my_creds_krb, &context[fd], target_name, oid, flags, 0,
                      NULL,  /* no channel bindings */
                      token_ptr, NULL,       /* ignore mech type */
                      &send_tok, &context_flags, 
                      NULL);      /* ignore time_rec */
#endif
        } else {
#if defined(GSI_AUTH) || defined(DCE)
            maj_stat = gss_init_sec_context(&min_stat,
                       /* was: GSS_C_NO_CREDENTIAL, */
                       my_creds_gsi_dce, &context[fd], target_name, oid, 
                       flags, 0, 
                       NULL,   /* no channel bindings */
                       token_ptr, NULL,    /* ignore mech type */
                       &send_tok, &context_flags, 
                       NULL);   /* ignore time_rec */
#endif
        }
        /* since recv_tok is not malloc'ed, don't need to call
           gss_release_buffer, instead clear it. */
        bzero(aidi_scratch_buffer, SCRATCH_BUFFER_SIZE);

        if (maj_stat != GSS_S_COMPLETE
            && maj_stat != GSS_S_CONTINUE_NEEDED) {
            aidi_display_status("initializing context", maj_stat, min_stat,
                                type_flag);
            if (type_flag == AID_Kerberos) {
#ifdef Kerberos
                (void) krb5_gss_release_name(&min_stat, &target_name);
#endif
            } else {
#if defined(GSI_AUTH) || defined(DCE)
                (void) gss_release_name(&min_stat, &target_name);
#endif
            }
            return -1;
        }

        if (send_tok.length != 0) {
            if (aidi_debug > 0)
                fprintf(stderr,
                        "Sending init_sec_context token (size=%d)\n",
                        send_tok.length);
            if (aidi_send_token(fd, &send_tok) < 0) {
                if (type_flag == AID_Kerberos) {
#ifdef Kerberos
                    (void) generic_gss_release_buffer(&min_stat,
                                                      &send_tok);
                    (void) krb5_gss_release_name(&min_stat, &target_name);
#endif
                } else {
#if defined(GSI_AUTH) || defined(DCE)
                    (void) gss_release_buffer(&min_stat, &send_tok);
                    (void) gss_release_name(&min_stat, &target_name);
#endif
                }
                return -1;
            }
        }
        if (type_flag == AID_Kerberos) {
#ifdef Kerberos
            (void) generic_gss_release_buffer(&min_stat, &send_tok);
#endif
        } else {
#if defined(GSI_AUTH) || defined(DCE)
            (void) gss_release_buffer(&min_stat, &send_tok);
#endif
        }

        if (maj_stat == GSS_S_CONTINUE_NEEDED) {
            if (aidi_debug > 0)
                fprintf(stderr, "continue needed...\n");
            recv_tok.value = &aidi_scratch_buffer;
            recv_tok.length = SCRATCH_BUFFER_SIZE;
            if (aidi_rcv_token(fd, &recv_tok) <= 0) {
                if (type_flag == AID_Kerberos) {
#ifdef Kerberos
                    (void) krb5_gss_release_name(&min_stat, &target_name);
#endif
                } else {
#if defined(GSI_AUTH) || defined(DCE)
                    (void) gss_release_name(&min_stat, &target_name);
#endif
                }
                return -1;
            }
            token_ptr = &recv_tok;
        }
    } while (maj_stat == GSS_S_CONTINUE_NEEDED);

    if (type_flag == AID_Kerberos) {
#ifdef Kerberos
        (void) krb5_gss_release_name(&min_stat, &target_name);
#endif
    } else {
#if defined(GSI_AUTH) || defined(DCE)
        (void) gss_release_name(&min_stat, &target_name);
#endif
    }

    if (aidi_debug > 0)
        aidi_display_ctx_flags();

    type_fd[fd] = type_flag;    /* record the type (GSI/Krb) for this fd */
#if defined(AID_TIMING)

    (void) gettimeofday(&endTimeFunc, (struct timezone *) 0);
    (void) aidi_tvsub(&sTimeDiff, &endTimeFunc, &startTimeFunc);
    fSec =
        (float) sTimeDiff.tv_sec + ((float) sTimeDiff.tv_usec / 1000000.0);

    /* for AID_TIMING  print time */
    fprintf(stdout, " ---  %.5f sec time taken for executing "
            "aid_establish_context_clientside() ---  \n", fSec);
#endif
    return 0;
}

/*
 Write a buffer on a socket, in a format for aid_read to accept.  It
 does this by sending the data, and generating and sending a MIC (each
 as tokens (with a length prepended).  The MIC is generated by the
 libraries GSSAPI/SSLeay and is an MD5 hash of the data, session key,
 and sequence number.  Error values are similar to the write call,
 positive is number of bytes written, negative for an error.  */
int aid_write(fd, buffer, length)
int fd;
char *buffer;
int length;
{
    gss_buffer_desc in_buf, mic_buf;
    int state;
    OM_uint32 maj_stat, min_stat;
    gss_name_t src_name, targ_name;
    gss_buffer_desc sname, tname;
    OM_uint32 lifetime;
    gss_OID mechanism, name_type;
    gss_buffer_desc oid_name;
    OM_uint32 use_encrypt = 0;

    in_buf.value = buffer;
    in_buf.length = length;
    /* Produce a signature block for the message */
    if (type_fd[fd] == AID_Kerberos) {
#ifdef Kerberos
        maj_stat = krb5_gss_get_mic(&min_stat, context[fd],
                                    GSS_C_QOP_DEFAULT, &in_buf, &mic_buf);
#endif
    } else {
#ifdef GSI_AUTH
        maj_stat = gss_get_mic(&min_stat, context[fd],
                               GSS_C_QOP_DEFAULT, &in_buf, &mic_buf);
#endif
#ifdef DCE
        maj_stat = gss_sign(&min_stat, context[fd],
                            GSS_C_QOP_DEFAULT, &in_buf, &mic_buf);
#endif
    }
    if (maj_stat != GSS_S_COMPLETE) {
        aidi_display_status("signing message", maj_stat, min_stat,
                            type_fd[fd]);
        return (-1);
    }

    /* Send to server */
    if (aidi_send_token(fd, &in_buf) < 0) {
        if (type_fd[fd] == AID_Kerberos) {
#ifdef Kerberos
            (void) krb5_gss_delete_sec_context(&min_stat,
                                               &context[fd],
                                               GSS_C_NO_BUFFER);
#endif
        } else {
#if defined(GSI_AUTH) || defined(DCE)
            (void) gss_delete_sec_context(&min_stat,
                                          &context[fd], GSS_C_NO_BUFFER);
#endif
        }
        return -1;
    }

    /* Send signature block */
    if (aidi_send_token(fd, &mic_buf) < 0) {
        if (type_fd[fd] == AID_Kerberos) {
#ifdef Kerberos
            (void) krb5_gss_delete_sec_context(&min_stat,
                                               &context[fd],
                                               GSS_C_NO_BUFFER);
#endif
        } else {
#if defined(GSI_AUTH) || defined(DCE)
            (void) gss_delete_sec_context(&min_stat,
                                          &context[fd], GSS_C_NO_BUFFER);
#endif
        }

        return -1;
    }

    if (type_fd[fd] == AID_Kerberos) {
#ifdef Kerberos
        (void) generic_gss_release_buffer(&min_stat, &mic_buf);
#endif
    } else {
#if defined(GSI_AUTH) || defined(DCE)
        (void) gss_release_buffer(&min_stat, &mic_buf);
#endif
    }

    return (in_buf.length);
}

/* 
 Set a the aidi_debug flag for displaying various levels of messages.
  0 - none (default)
  1 - some
  2 - more
 -1 - a particular set  
  (currently only checking for on/off)

 */
int aid_debug(val)
int val;
{
    aidi_debug = val;
}


/*

This routine can be defined to replace the default SSL one for
providing the key password.

int EVP_read_pw_string(buf,len,prompt,verify)
char *buf;
int len;
char *prompt;
int verify;
{
    int i;
    if (do_pw==1) {
       gets(buf);  This could be changed to read an encrypted pass phrase
                   from a file or something like that.
       return 0;
    }
 
    else {
       if ((prompt == NULL) && (prompt_string[0] != '\0'))
           prompt=prompt_string; 
       return(des_read_pw_string(buf,len,prompt,verify));
    }
}

*/

/*
 * Modifying aid_establish_context_serverside() to accept delegated 
 * certificate for GSI only.
 * 
 Set up an session between this server and a connected new client and server
 accepts delegated credential from client. 
 
 Establishses a GSS-API context (as the service specified in
 aid_setup_creds) with an incoming client, and returns the
 authenticated client name (the id on the other side of the socket).
 The other side (the client) must call
 aid_establish_context_clientside_with_delegation at about the same time for
 the exchanges across the network to work (each side will block waiting
 for the other).  

 If successful, the context handle is set in the global context array
 and delegated certificate is stored in the credential array.  If
 unsuccessful, an error message is displayed and -1 is returned.

 */
int aid_establish_context_serverside_with_delegate(fd, clientName,
                                                   max_len_clientName,
                                                   type_flag)
int fd;
char *clientName;
int max_len_clientName;
int type_flag;
{
    gss_buffer_desc send_buffer, recv_buffer;
    gss_buffer_desc client_name;
    gss_name_t client;
    gss_OID doid;
    OM_uint32 maj_stat, min_stat;
    int i;
#if defined(AID_TIMING)
    struct timeval startTimeFunc, endTimeFunc, sTimeDiff;
    float fSec;
    /* Starting timer for function */
    (void) gettimeofday(&startTimeFunc, (struct timezone *) 0);

#endif


#if defined(GSI_AUTH) && defined(OBJ_CREATE_NEEDED)
    if (aid_obj_created < 10) {
        int obj_cre_val;
        /* create the SDSC UID object (in the running version of SSL) */
        obj_cre_val =
            OBJ_create("0.9.2342.19200300.100.1.1", "USERID", "userId");
        fprintf(stderr, "obj_cre_val = %d\n", obj_cre_val);
        aid_obj_created++;
    }
#endif

    if (aidi_check_type(type_flag))
        return (-2);

    /* verify delegated credential handle does not point to any credential */
    if (delegated_cred_gsi[fd] != GSS_C_NO_CREDENTIAL) {

        /* reset delegated credential handle */
        maj_stat = gss_release_cred(&min_stat, &delegated_cred_gsi[fd]);

        if (maj_stat != GSS_S_COMPLETE) {
            fprintf(stderr, "Unable to release delegated credential  \n");
            return -1;
        }
    }

    context[fd] = GSS_C_NO_CONTEXT;
    recv_buffer.value = &aidi_scratch_buffer;

/* testing time for following subsection
#if defined(AID_TIMING)
          (void) gettimeofday(&startTime, (struct timezone *)0); 
#endif
*/

    do {
        recv_buffer.length = SCRATCH_BUFFER_SIZE;
        if (aidi_rcv_token(fd, &recv_buffer) <= 0) {
            fprintf(stderr,
                    "aid_establish_context_serverside_with_delegate: error receiving token\n");
            return -1;
        }
        if (aidi_debug > 0) {
            fprintf(stderr, "Received token (size=%d): \n",
                    recv_buffer.length);
            aidi_print_token(&recv_buffer);
        }
#if defined(GSI_AUTH) || defined(DCE)
        maj_stat = gss_accept_sec_context(&min_stat, &context[fd], 
                        my_creds_gsi_dce, &recv_buffer, 
                        GSS_C_NO_CHANNEL_BINDINGS, &client, &doid, 
                        &send_buffer, &context_flags, 
                        NULL,       /* ignore time_rec */
                        &delegated_cred_gsi[fd]);
                                    /* received delegated credential */
#endif

        if (maj_stat != GSS_S_COMPLETE
            && maj_stat != GSS_S_CONTINUE_NEEDED) {
            aidi_display_status("accepting context", maj_stat, min_stat,
                                type_flag);
            bzero(aidi_scratch_buffer, SCRATCH_BUFFER_SIZE);
            return -1;
        }

        /* since recv_buffer is not malloc'ed, don't need to call
           gss_release_buffer, instead clear it. */
        bzero(aidi_scratch_buffer, SCRATCH_BUFFER_SIZE);

        if (send_buffer.length != 0) {
            if (aidi_debug > 0) {
                fprintf(stderr,
                        "Sending accept_sec_context token (size=%d):\n",
                        send_buffer.length);
                aidi_print_token(&send_buffer);
            }
            if (aidi_send_token(fd, &send_buffer) < 0) {
                fprintf(stderr, "failure sending token\n");
                return -1;
            }
#if defined(GSI_AUTH) || defined(DCE)
            (void) gss_release_buffer(&min_stat, &send_buffer);
#endif
        }
        if (aidi_debug > 0) {
            if (maj_stat == GSS_S_CONTINUE_NEEDED)
                fprintf(stderr, "continue needed...\n");
        }
    } while (maj_stat == GSS_S_CONTINUE_NEEDED);

/*
#if defined(AID_TIMING)

    (void) gettimeofday (&endTime, (struct timezone *) 0);
    (void) aidi_tvsub (&sTimeDiff, &endTime, &startTime);
    fSec = (float) sTimeDiff.tv_sec + ((float) sTimeDiff.tv_usec / 1000000.0);
            

    fprintf (stdout," ---  %.5f sec time taken for executing Mutual Authentication loop and " 
    "accepting delegation in aid_establish_context_serverside_with_delegate()---  \n", fSec);
#endif
*/

    /* convert name (internally represented) to a string */
#if defined(GSI_AUTH) || defined(DCE)
    maj_stat = gss_display_name(&min_stat, client, &client_name, &doid);
#endif

    if (maj_stat != GSS_S_COMPLETE) {
        aidi_display_status("displaying name", maj_stat, min_stat,
                            type_flag);
        return -1;
    }

    i = client_name.length;
    if (max_len_clientName < i)
        i = max_len_clientName;

    strncpy(clientName, client_name.value, i);
    if (max_len_clientName > client_name.length)
        clientName[client_name.length] = '\0';

    /* release the name structure */
#if defined(GSI_AUTH) || defined(DCE)
    maj_stat = gss_release_name(&min_stat, &client);
#endif

    if (maj_stat != GSS_S_COMPLETE) {
        aidi_display_status("releasing name", maj_stat, min_stat,
                            type_flag);
        return -1;
    }
#if defined(GSI_AUTH) || defined(DCE)
    (void) gss_release_buffer(&min_stat, &client_name);
#endif

    type_fd[fd] = type_flag;    /* record the type (GSI/Krb) for this fd */

    if (aidi_debug > 0) {
        OM_uint32 lifetime;
        gss_name_t delegated_name = GSS_C_NO_NAME;
        gss_buffer_desc name_buffer;

        maj_stat = gss_inquire_cred(&min_stat,  /* (out) minor status */
                            delegated_cred_gsi[fd], /* (in) cred handle */
                            &delegated_name,    /* (out) name in cred */
                            &lifetime,  /* (out) lifetime */
                            NULL,       /* (out) cred usage
                                           GSS_C_BOTH
                                           GSS_C_INITIATE
                                           GSS_C_ACCEPT */
                            NULL);      /* (out) mechanisms */
        maj_stat = gss_display_name(&min_stat,
                                    delegated_name, &name_buffer, NULL);
        printf("%s, lifetime %i \n", (char *) name_buffer.value,
               (int) lifetime);
    }
#if defined(AID_TIMING)

    (void) gettimeofday(&endTimeFunc, (struct timezone *) 0);
    (void) aidi_tvsub(&sTimeDiff, &endTimeFunc, &startTimeFunc);
    fSec =
        (float) sTimeDiff.tv_sec + ((float) sTimeDiff.tv_usec / 1000000.0);

    /* for AID_TIMING  print time */
    fprintf(stdout, " ---  %.5f sec time taken for executing "
            "aid_establish_context_serverside_with_delegate() ---  \n",
            fSec);
#endif

    return 0;
}

/* Method used by client in calling the server to establish security
 * context without providing Server DN. The server DN is then returned
 * in serverName after the creation of context. This server DN can be
 * verified by application before proceeding further if wished.
 * 
 * Input Parameters 
 * fd: Socket File Descriptor of the connection with the server
 * max_len_serverName: Maximum length of ServerDN willing to be accepted
 *      in serverName by client
 * deleg_flag: Should be equal to 1 if delegation is to be initiated or 
 *                                0 if no delegation initiation.
 * type_flag: Specified to be AID_GSI or AID_DCE or AID_KERBEROS as accordingly
 * 
 * Return Parameters:
 * serverName: Pointer to the string contain DN of the connected server
 *  */
int aid_establish_context_clientside_without_server_dn(fd, serverName,
                                                       max_len_serverName,
                                                       deleg_flag,
                                                       type_flag)
int fd;
char *serverName;
int max_len_serverName;
int deleg_flag;
int type_flag;
{

    gss_OID oid = GSS_C_NULL_OID;
    gss_buffer_desc send_tok, recv_tok, *token_ptr;
    gss_name_t server_name = GSS_C_NO_NAME;
    gss_buffer_desc server_dn;
    OM_uint32 maj_stat, min_stat;
    OM_uint32 flags = 0;
    int i;

#if defined(AID_TIMING)
    struct timeval startTimeFunc, endTimeFunc, sTimeDiff;
    float fSec;
    /* Starting timer for function */
    (void) gettimeofday(&startTimeFunc, (struct timezone *) 0);

#endif

#if defined(GSI_AUTH) && defined(OBJ_CREATE_NEEDED)
    if (aid_obj_created < 10) {
        int obj_cre_val;
        /* create the SDSC UID object (in the running version of SSL) */
        obj_cre_val =
            OBJ_create("0.9.2342.19200300.100.1.1", "USERID", "userId");
        if (aidi_debug > 0)
            fprintf(stderr, "obj_cre_val = %d\n", obj_cre_val);
        aid_obj_created++;
    }
#endif

    if (aidi_check_type(type_flag))
        return (-2);

    /* performing context establishment loop */
    token_ptr = GSS_C_NO_BUFFER;
    context[fd] = GSS_C_NO_CONTEXT;
    flags = GSS_C_MUTUAL_FLAG | GSS_C_REPLAY_FLAG;
    if (deleg_flag)
        flags |= GSS_C_DELEG_FLAG;

    do {
        if (aidi_debug > 0)
            fprintf(stderr, "--> calling gss_init_sec_context\n");

#if defined(GSI_AUTH) || defined(DCE)
        maj_stat = gss_init_sec_context(&min_stat,
                       /* was: GSS_C_NO_CREDENTIAL,  */
                       my_creds_gsi_dce, &context[fd], 
                       GSS_C_NO_NAME,  /* no Server DN specified */
                       oid, flags, 0, NULL,    /* no channel bindings */
                       token_ptr, NULL,        /* ignore mech type */
                       &send_tok, &context_flags, 
                       NULL);       /* ignore time_rec */
#endif

        /* since recv_tok is not malloc'ed, don't need to call
           gss_release_buffer, instead clear it. */
        bzero(aidi_scratch_buffer, SCRATCH_BUFFER_SIZE);

        if (maj_stat != GSS_S_COMPLETE
            && maj_stat != GSS_S_CONTINUE_NEEDED) {
            aidi_display_status("initializing context", maj_stat, min_stat,
                                type_flag);

            return -1;
        }

        if (send_tok.length != 0) {
            if (aidi_debug > 0)
                fprintf(stderr,
                        "Sending init_sec_context token (size=%d)\n",
                        send_tok.length);
            if (aidi_send_token(fd, &send_tok) < 0) {
#if defined(GSI_AUTH) || defined(DCE)
                (void) gss_release_buffer(&min_stat, &send_tok);
#endif
                return -1;
            }
        }
#if defined(GSI_AUTH) || defined(DCE)
        (void) gss_release_buffer(&min_stat, &send_tok);
#endif

        if (maj_stat == GSS_S_CONTINUE_NEEDED) {
            if (aidi_debug > 0)
                fprintf(stderr, "continue needed...\n");
            recv_tok.value = &aidi_scratch_buffer;
            recv_tok.length = SCRATCH_BUFFER_SIZE;
            if (aidi_rcv_token(fd, &recv_tok) <= 0) {

                return -1;
            }
            token_ptr = &recv_tok;
        }
    } while (maj_stat == GSS_S_CONTINUE_NEEDED);

    type_fd[fd] = type_flag;    /* record the type (GSI/Krb) for this fd */

    /* reading Server DN from context */
#if defined(GSI_AUTH)

    maj_stat = gss_inquire_context(&min_stat,   /*minor_status */
                                   context[fd], /* context_handle_P */
                                   NULL,        /* gss_name_t * src_name_P, */
                                   &server_name,/* targ_name_P, */
                                   0,   /*OM_uint32 * lifetime_rec, */
                                   NULL,        /* gss_OID *  mech_type, */
                                   NULL,        /* OM_uint32 * ctx_flags, */
                                   NULL,        /* int * locally_initiated, */
                                   NULL);       /* int * open) */

    if (maj_stat != GSS_S_COMPLETE) {
        fprintf(stderr,
                "aid_establish_context_clientside_without_server_dn() - unable to inquire context \n");
        (void) gss_release_name(&min_stat, &server_name);
        return -1;
    }

    /* after successful completion
     * convert name (internally represented) to a string */

    maj_stat = gss_display_name(&min_stat, server_name, &server_dn, NULL);

    if (maj_stat != GSS_S_COMPLETE) {
        fprintf(stderr,
                "aid_establish_context_clientside_without_server_dn() - unable to read server name \n");
        (void) gss_release_name(&min_stat, &server_name);
        return -1;
    }

    i = server_dn.length;
    if (max_len_serverName < i)
        i = max_len_serverName;

    strncpy(serverName, server_dn.value, i);
    if (max_len_serverName > server_dn.length)
        serverName[server_dn.length] = '\0';

    /* release the name structure */
    maj_stat = gss_release_name(&min_stat, &server_name);

    if (maj_stat != GSS_S_COMPLETE) {
        fprintf(stderr,
                " aid_establish_context_clientside_without_server_dn() - unable to release name \n");
        return -1;
    }

    /* release buffer */

    (void) gss_release_buffer(&min_stat, &server_dn);
    if (maj_stat != GSS_S_COMPLETE) {
        fprintf(stderr,
                " aid_establish_context_clientside_without_server_dn() - unable to release buffer \n");
        return -1;
    }
#endif
#if defined(AID_TIMING)

    (void) gettimeofday(&endTimeFunc, (struct timezone *) 0);
    (void) aidi_tvsub(&sTimeDiff, &endTimeFunc, &startTimeFunc);
    fSec =
        (float) sTimeDiff.tv_sec + ((float) sTimeDiff.tv_usec / 1000000.0);

    /* for AID_TIMING  print time */
    fprintf(stdout, " ---  %.5f sec time taken for executing "
            "aid_establish_context_clientside_without_server_dn() ---  \n",
            fSec);
#endif
    return 0;
}

/*
 * Export delegated credential for future use or for use by other processes
 * input parameter - 
 * fd : Socket File descriptor from where the delegated credential was received
 * output:  filename of stored credential is saved in 
 *          deleg_proxy_filename_buffer[fd]
 */
int aid_export_delegate(int fd)
{
    OM_uint32 maj_stat, min_stat;

#if defined(AID_TIMING)
    struct timeval startTimeFunc, endTimeFunc, sTimeDiff;
    float fSec;
    /* Starting timer for function */
    (void) gettimeofday(&startTimeFunc, (struct timezone *) 0);

#endif

    if (aidi_check_type(AID_GSI))
        return (-2);
    /* test presence of delegated credential */
    if (delegated_cred_gsi[fd] == GSS_C_NO_CREDENTIAL) {
        fprintf(stderr, "No delegated credential found\n");
        return -1;
    }

    if (deleg_proxy_filename[fd] == GSS_C_NO_BUFFER) {
        deleg_proxy_filename[fd] = &deleg_proxy_filename_buffer;
    } else {
        /* verify if filename place holder is empty or not */
        if (deleg_proxy_filename[fd]->length > 0) {
            maj_stat =
                gss_release_buffer(&min_stat, deleg_proxy_filename[fd]);

            if (maj_stat != GSS_S_COMPLETE) {
                fprintf(stderr, "unable to clean buffer \n");
                return -1;
            }
        }
    }


    /* export credential to /tmp and save filename for future retrieval  */
    maj_stat = gss_export_cred(&min_stat, 
                   delegated_cred_gsi[fd], /* handle to delegated credential */
                   NULL,    /* desired mechanism - NUL default */
                   1,       /* option req 1- mech specifil */
                   deleg_proxy_filename[fd]);
                           /* filename to where proxy has been stored */

    /* if successful filename of the stored proxy is store in 
     * deleg_proxy_filename[fd] for future retrieval */

    if (maj_stat != GSS_S_COMPLETE) {
        fprintf(stderr,
          "aid_export_delegate() - Unable to export delegated credential  \n");
        return -1;
    }

    if (aidi_debug > 0) {
        /* print file location of delegated cert */
        fprintf(stderr, "printing filename Locaiton  %s \n",
                (char *) deleg_proxy_filename[fd]->value);
    }
#if defined(AID_TIMING)

    (void) gettimeofday(&endTimeFunc, (struct timezone *) 0);
    (void) aidi_tvsub(&sTimeDiff, &endTimeFunc, &startTimeFunc);
    fSec =
        (float) sTimeDiff.tv_sec + ((float) sTimeDiff.tv_usec / 1000000.0);

    /* for AID_TIMING  print time */
    fprintf(stdout,
            " ---  %.5f sec time taken for executing        aid_export_delegate() ---  \n",
            fSec);
#endif
    return 0;

}

/*
 * Import delegated credential from local storage. 
 * input parameter - 
 * fd: Socket File Descriptor from where the delegated credential has
 *     been recieved
 * output - credential_handle for initializing and acception security context
 */
int aid_import_delegate(int fd)
{
    OM_uint32 maj_stat, min_stat;

#if defined(AID_TIMING)
    struct timeval startTimeFunc, endTimeFunc, sTimeDiff;
    float fSec;
    /* Starting timer for function */
    (void) gettimeofday(&startTimeFunc, (struct timezone *) 0);

#endif

    if (aidi_check_type(AID_GSI))
        return (-2);
    /* verify delegated credential handle does not point to any credential */
    if (delegated_cred_gsi[fd] != GSS_C_NO_CREDENTIAL) {

        /* reset delegated credential handle */
        maj_stat = gss_release_cred(&min_stat, &delegated_cred_gsi[fd]);

        if (maj_stat != GSS_S_COMPLETE) {
            fprintf(stderr, "Unable to release delegated credential  \n");
            return -1;
        }
    }

    /* verify if filename place holder is not empty */
    if (deleg_proxy_filename[fd] == GSS_C_NO_BUFFER) {
        fprintf(stderr,
                "aid_import_delegate() - Proxy file location not has not been initialized \n");
        return -1;
    }

    if (deleg_proxy_filename[fd]->length == 0) {
        fprintf(stderr,
                "aid_import_delegate() - Proxy file location not present or not specified \n");
        return -1;
    }

    /* import credential from /tmp */
    maj_stat = gss_import_cred(&min_stat,       /* minor status */
         &delegated_cred_gsi[fd], /* handle to imported delegated credential */
         NULL,   /* desired mechanism - NULL default */
         1,      /* option req 1- mech specific */
         deleg_proxy_filename[fd],
                 /* filename to where proxy has been stored */
         0,      /* time req */
         NULL);  /*time rec - specifies remaining time for which it is valid */

    /* if successful proxy is retrieved from file and cred_handle can be used
     * for utilizing the the credential */

    if (maj_stat != GSS_S_COMPLETE) {
        fprintf(stderr,
          "aid_import_delegate() - Unable to import delegated credential  \n");
        return -1;
    }

    if (aidi_debug > 0) {
        /* print file location of delegated cert */
        printf
            ("Debug call from aid_import_cred() : remaining life : %i \n",
             aid_get_remaining_delegate_lifetime(fd));
    }
#if defined(AID_TIMING)

    (void) gettimeofday(&endTimeFunc, (struct timezone *) 0);
    (void) aidi_tvsub(&sTimeDiff, &endTimeFunc, &startTimeFunc);
    fSec =
        (float) sTimeDiff.tv_sec + ((float) sTimeDiff.tv_usec / 1000000.0);

    /* for AID_TIMING  print time */
    fprintf(stdout,
      " ---  %.5f sec time taken for executing aid_import_delegate() ---  \n",
      fSec);
#endif
    return 0;
}

/* before calling this function a security context needs to be
 * established between the parties input parameter - fd of the
 * connection between the client and server delegation_time_req -
 * request for no. of minutes that is to be provided to the remote
 * party maybe NULL for default */
int aid_init_delegation(int fd, OM_uint32 delegation_time)
{

    OM_uint32 maj_stat, min_stat;
    gss_buffer_desc send_buffer = GSS_C_EMPTY_BUFFER;
    gss_buffer_desc recv_buffer = GSS_C_EMPTY_BUFFER;
    gss_buffer_t recv_buffer_ptr = &recv_buffer;
    gss_buffer_t send_buffer_ptr = &send_buffer;
    OM_uint32 flags = GSS_C_GLOBUS_SSL_COMPATIBLE;

#if defined(AID_TIMING)
    struct timeval startTimeFunc, endTimeFunc, sTimeDiff;
    float fSec;
    /* Starting timer for function */
    (void) gettimeofday(&startTimeFunc, (struct timezone *) 0);

#endif
    if (aidi_check_type(AID_GSI))
        return (-2);
    delegation_time *= 60;    /* convert minutes to seconds - no. of seconds */
                              /* for which the delegation is valid  */
    do {

        maj_stat = gss_init_delegation(
                       &min_stat,         /* minor status */
                       context[fd],       /* context handle */
                                          /* created by gss_init_sec_context */
                       my_creds_gsi_dce,  /* credential used to create */
                                          /* delegate proxy certificate */
                       GSS_C_NO_OID,      /* desired mech */
                       GSS_C_NO_OID_SET,  /* extension_oids */
                       GSS_C_NO_BUFFER_SET,  /* extension_buffers */
                       recv_buffer_ptr,   /* input_token */
                       flags,             /* request flags */
                       delegation_time,   /* time request */
                       send_buffer_ptr);  /* output token */

        /* clean scratch area */
        bzero(aidi_scratch_buffer, SCRATCH_BUFFER_SIZE);

        if (maj_stat != GSS_S_COMPLETE
            && maj_stat != GSS_S_CONTINUE_NEEDED) {
            fprintf(stderr, "unable to delegate certifcate \n");
            bzero(aidi_scratch_buffer, SCRATCH_BUFFER_SIZE);
            return -1;
        }

        if (send_buffer.length != 0) {
            if (aidi_debug > 0) {
                fprintf(stderr,
                        "Sending init_delegation token (size=%d): \n",
                        send_buffer.length);
                aidi_print_token(&send_buffer);
            }

            /* send output token */
            if (aidi_send_token(fd, &send_buffer) < 0) {
                fprintf(stderr, "failure sending token \n");
                gss_release_buffer(&min_stat, &send_buffer);
                return -1;
            }

            (void) gss_release_buffer(&min_stat, &send_buffer);
        }


        if (maj_stat == GSS_S_CONTINUE_NEEDED) {
            if (aidi_debug > 0) {
                fprintf(stderr, "continue needed \n");
            }

            recv_buffer.value = &aidi_scratch_buffer;
            recv_buffer.length = SCRATCH_BUFFER_SIZE;
            /* receive output token from remote side */
            if (aidi_rcv_token(fd, &recv_buffer) <= 0) {
                fprintf(stderr,
                        "gss_init_delegation:  Error receiving token \n");
                return -1;
            }

            if (aidi_debug > 0) {
                fprintf(stderr, " Received token (size = %d) : \n ",
                        recv_buffer.length);
                aidi_print_token(&recv_buffer);
            }
        }
    } while (maj_stat == GSS_S_CONTINUE_NEEDED);

    if (maj_stat != GSS_S_COMPLETE) {
        fprintf(stderr, "unable to initiate delegation \n");
        return -1;
    }
#if defined(AID_TIMING)

    (void) gettimeofday(&endTimeFunc, (struct timezone *) 0);
    (void) aidi_tvsub(&sTimeDiff, &endTimeFunc, &startTimeFunc);
    fSec =
        (float) sTimeDiff.tv_sec + ((float) sTimeDiff.tv_usec / 1000000.0);

    /* for AID_TIMING  print time */
    fprintf(stdout,
       " ---  %.5f sec time taken for executing aid_init_delegation() ---  \n",
       fSec);
#endif
    return 0;

}

int aid_accept_delegation(int fd)
{

    OM_uint32 maj_stat, min_stat;
    gss_buffer_desc send_buffer = GSS_C_EMPTY_BUFFER;
    gss_buffer_desc recv_buffer = GSS_C_EMPTY_BUFFER;
    OM_uint32 delegation_time;
    gss_OID mech_type = GSS_C_NO_OID;
    OM_uint32 flags = GSS_C_GLOBUS_SSL_COMPATIBLE;

#if defined(AID_TIMING)
    struct timeval startTimeFunc, endTimeFunc, sTimeDiff;
    float fSec;
    /* Starting timer for function */
    (void) gettimeofday(&startTimeFunc, (struct timezone *) 0);

#endif

    if (aidi_check_type(AID_GSI))
        return (-2);

    /* verify delegated credential handle does not point to any credential */
    if (delegated_cred_gsi[fd] != GSS_C_NO_CREDENTIAL) {

        /* reset delegated credential handle */
        maj_stat = gss_release_cred(&min_stat, &delegated_cred_gsi[fd]);

        if (maj_stat != GSS_S_COMPLETE) {
            fprintf(stderr, "Unable to release delegated credential  \n");
            return -1;
        }
    }
    recv_buffer.value = &aidi_scratch_buffer;

    do {
        recv_buffer.length = SCRATCH_BUFFER_SIZE;
        /* receive output token from remote side */
        if (aidi_rcv_token(fd, &recv_buffer) <= 0) {
            fprintf(stderr,
                    "gss_accept_delegation : error receiving token \n");
            return -1;
        }

        if (aidi_debug > 0) {
            fprintf(stderr, " Received token (size = %d) : \n ",
                    recv_buffer.length);
            aidi_print_token(&recv_buffer);
        }

        maj_stat = gss_accept_delegation(
                        &min_stat,     /* minor status */
                        context[fd],   /* context handle */
                                       /* created by gss_init_sec_context */
                        GSS_C_NO_OID_SET,    /* extension_oids */
                        GSS_C_NO_BUFFER_SET, /* extension_buffers */
                        &recv_buffer,  /* input_token */
                        flags,         /* request flags */
                        0,             /* time request */
                        &delegation_time,    /* time received */
                        &delegated_cred_gsi[fd],
                                       /* delegated_cred_handle */
                        &mech_type,    /* mech_type */
                        &send_buffer); /* output token */

        if (maj_stat != GSS_S_COMPLETE
            && maj_stat != GSS_S_CONTINUE_NEEDED) {
            fprintf(stderr, "Unable to receive delegate certificate \n");
            bzero(aidi_scratch_buffer, SCRATCH_BUFFER_SIZE);
            return -1;
        }

        /* clear recv_buffer for next input token */
        bzero(aidi_scratch_buffer, SCRATCH_BUFFER_SIZE);

        if (send_buffer.length != 0) {
            if (aidi_debug > 0) {
                fprintf(stderr,
                        "Sending accept_sec_context token (size=%d): \n",
                        send_buffer.length);
                aidi_print_token(&send_buffer);
            }

            /* send output token */
            if (aidi_send_token(fd, &send_buffer) < 0) {
                fprintf(stderr, "failure sending token \n");
                (void) gss_release_buffer(&min_stat, &send_buffer);
                return -1;
            }

            (void) gss_release_buffer(&min_stat, &send_buffer);
        }

        if (aidi_debug > 0) {
            if (maj_stat == GSS_S_CONTINUE_NEEDED)
                fprintf(stderr, "continue needed \n");
        }

    } while (maj_stat == GSS_S_CONTINUE_NEEDED);

    if (maj_stat != GSS_S_COMPLETE) {
        fprintf(stderr, "unable to accept delegation \n");
        return -1;
    }
#if defined(AID_TIMING)

    (void) gettimeofday(&endTimeFunc, (struct timezone *) 0);
    (void) aidi_tvsub(&sTimeDiff, &endTimeFunc, &startTimeFunc);
    fSec =
        (float) sTimeDiff.tv_sec + ((float) sTimeDiff.tv_usec / 1000000.0);

    /* for AID_TIMING  print time */
    fprintf(stdout,
     " ---  %.5f sec time taken for executing aid_accept_delegation() ---  \n",
     fSec);
#endif
    return 0;

}

/* returns lifetime of the delegated certificate in seconds */

int aid_get_remaining_delegate_lifetime(int fd)
{

    OM_uint32 maj_stat, min_stat;
    OM_uint32 lifetime;
    gss_name_t delegated_name = GSS_C_NO_NAME;
    gss_buffer_desc name_buffer;

#if defined(AID_TIMING)
    struct timeval startTimeFunc, endTimeFunc, sTimeDiff;
    float fSec;
    /* Starting timer for function */
    (void) gettimeofday(&startTimeFunc, (struct timezone *) 0);

#endif

    if (aidi_check_type(AID_GSI))
        return (-2);

    if (delegated_cred_gsi[fd] == GSS_C_NO_CREDENTIAL) {
        fprintf(stderr,
                "aid_get_remaining_lifetime() - No delegated credential found \n");
        return -1;
    }

    maj_stat = gss_inquire_cred(&min_stat,      /* (out) minor status */
                        delegated_cred_gsi[fd], /* (in) cred handle */
                        &delegated_name,        /* (out) name in cred */
                        &lifetime,              /* (out) lifetime */
                        NULL,                   /* (out) cred usage
                                                         GSS_C_BOTH
                                                         GSS_C_INITIATE
                                                         GSS_C_ACCEPT */
                        NULL);  /* (out) mechanisms */

    if (maj_stat != GSS_S_COMPLETE) {

        fprintf(stderr,
                " aid_get_remaining_lifetime() - Unable to inquire delegated credential  \n");
        return -1;
    }

    if (aidi_debug > 0) {

        maj_stat = gss_display_name(&min_stat,
                                    delegated_name, &name_buffer, NULL);

        if (maj_stat != GSS_S_COMPLETE) {
            fprintf(stderr,
              "Failure to retrieve delegated credential  name or bad name \n");
            (void) gss_release_name(&min_stat, &delegated_name);
            return -1;
        }
        printf("%s, lifetime %i \n", (char *) name_buffer.value,
               (int) lifetime);
        (void) gss_release_buffer(&min_stat, &name_buffer);
    }

    (void) gss_release_name(&min_stat, &delegated_name);

#if defined(AID_TIMING)

    (void) gettimeofday(&endTimeFunc, (struct timezone *) 0);
    (void) aidi_tvsub(&sTimeDiff, &endTimeFunc, &startTimeFunc);
    fSec =
        (float) sTimeDiff.tv_sec + ((float) sTimeDiff.tv_usec / 1000000.0);

    /* for AID_TIMING  print time */
    fprintf(stdout, " ---  %.5f sec time taken for executing "
            "aid_get_remaining_delegate_lifetime() ---  \n", fSec);
#endif

    return lifetime;

}

/* Creates context with another server using the delegated credential 
 * input parameters 
 * fd: Socket File description to the connecting server
 * server_dn_in : NULL if no server_dn is provided otherwise provide DN
 *                for mutual authentication
 * max_len_server_dn_out : maximum length of the string that will store
 *                 recieved server dn
 * received_delegation_fd: Socket fd from where the delegate has been received
 * deleg_flag : 0 = no delegation 1 = initiate delegation
 * 
 * output parameters
 * server_dn_out : Returns a String containing the Server Dn of the 
 *                 connected server 
 * */
int aid_establish_context_using_client_delegate(fd, server_dn_in,
                                                server_dn_out,
                                                max_len_server_dn_out,
                                                received_delegation_fd,
                                                deleg_flag)
int fd;
char *server_dn_in;
char *server_dn_out;
int max_len_server_dn_out;
int received_delegation_fd;
int deleg_flag;
{

    gss_buffer_desc send_tok, recv_tok, *token_ptr, name_buffer;
    gss_name_t target_name = GSS_C_NO_NAME;
    OM_uint32 maj_stat, min_stat;
    OM_uint32 flags = 0;
    int i;

#if defined(AID_TIMING)
    struct timeval startTimeFunc, endTimeFunc, sTimeDiff;
    float fSec;
    /* Starting timer for function */
    (void) gettimeofday(&startTimeFunc, (struct timezone *) 0);

#endif

    if (aidi_check_type(AID_GSI))
        return (-2);

    /* check if server_dn is provided if provided then use it for
       mutual authentication or else only would verify the client and
       the client will recieve the Server DN after execution */
    if (server_dn_in != NULL) {
        /*
         * Import the name into target_name.
         */
        name_buffer.value = server_dn_in;
        name_buffer.length = strlen(server_dn_in) + 1;


        maj_stat = gss_import_name(&min_stat, &name_buffer,
                                   NULL, &target_name);

        if (maj_stat != GSS_S_COMPLETE) {
            fprintf(stderr,
                    "unable to import name (aid_establish_context_using_client_delegate) \n");
        }

        (void) gss_release_buffer(&min_stat, &name_buffer);
    }

    token_ptr = GSS_C_NO_BUFFER;
    context[fd] = GSS_C_NO_CONTEXT;
    flags = GSS_C_MUTUAL_FLAG | GSS_C_REPLAY_FLAG;
    if (deleg_flag)
        flags |= GSS_C_DELEG_FLAG;

    do {
        if (aidi_debug > 0)
            fprintf(stderr, "--> calling gss_init_sec_context\n");

        maj_stat = gss_init_sec_context(&min_stat, 
                        delegated_cred_gsi[received_delegation_fd], 
                        &context[fd], target_name, GSS_C_NO_OID, 
                        flags, 0, 
                        NULL, /* no channel bindings */
                        token_ptr, NULL,        /* ignore mech type */
                        &send_tok, &context_flags, 
                        NULL);       /* ignore time_rec */

        /* since recv_tok is not malloc'ed, don't need to call
           gss_release_buffer, instead clear it. */
        bzero(aidi_scratch_buffer, SCRATCH_BUFFER_SIZE);

        if (maj_stat != GSS_S_COMPLETE
            && maj_stat != GSS_S_CONTINUE_NEEDED) {
            fprintf(stderr,
                    "aid_establish_context_using_delegate_certificate() - unable to initiate security with delegated cred \n");
            (void) gss_release_name(&min_stat, &target_name);
            return -1;
        }

        if (send_tok.length != 0) {
            if (aidi_debug > 0)
                fprintf(stderr,
                        "Sending init_sec_context token (size=%d)\n",
                        send_tok.length);
            if (aidi_send_token(fd, &send_tok) < 0) {
                (void) gss_release_buffer(&min_stat, &send_tok);
                (void) gss_release_name(&min_stat, &target_name);
                return -1;
            }
        }
        (void) gss_release_buffer(&min_stat, &send_tok);

        if (maj_stat == GSS_S_CONTINUE_NEEDED) {
            if (aidi_debug > 0)
                fprintf(stderr, "continue needed...\n");
            recv_tok.value = &aidi_scratch_buffer;
            recv_tok.length = SCRATCH_BUFFER_SIZE;
            if (aidi_rcv_token(fd, &recv_tok) <= 0) {
                (void) gss_release_name(&min_stat, &target_name);
                return -1;
            }
            token_ptr = &recv_tok;
        }
    } while (maj_stat == GSS_S_CONTINUE_NEEDED);

    if (server_dn_in == NULL) {
        /* reading Server DN from context */

        maj_stat = gss_inquire_context(&min_stat,       /*minor_status */
                          context[fd],     /* context_handle_P */
                          NULL,            /* gss_name_t * src_name_P, */
                          &target_name,    /* targ_name_P, */
                          0,               /* OM_uint32 * lifetime_rec, */
                          NULL,            /* gss_OID * mech_type, */
                          NULL,            /* OM_uint32 * ctx_flags, */
                          NULL,            /* int * locally_initiated, */
                          NULL);           /* int * open) */

        if (maj_stat != GSS_S_COMPLETE) {
            fprintf(stderr,
                    "aid_establish_context_using_server_delegate() - unable to inquire context \n");
            (void) gss_release_name(&min_stat, &target_name);
            return -1;
        }
    }

    /* after successful completion
     * convert name (internally represented) to a string */

    maj_stat =
        gss_display_name(&min_stat, target_name, &name_buffer, NULL);

    if (maj_stat != GSS_S_COMPLETE) {
        fprintf(stderr,
                "aid_establish_context_using_server_delegate()- unable to read server name \n");
        (void) gss_release_name(&min_stat, &target_name);
        return -1;
    }

    i = name_buffer.length;
    if (max_len_server_dn_out < i)
        i = max_len_server_dn_out;

    strncpy(server_dn_out, name_buffer.value, i);
    if (max_len_server_dn_out > name_buffer.length)
        server_dn_out[name_buffer.length] = '\0';

    /* release buffer */

    (void) gss_release_name(&min_stat, &target_name);
    (void) gss_release_buffer(&min_stat, &name_buffer);
    type_fd[fd] = AID_GSI;

#if defined(AID_TIMING)

    (void) gettimeofday(&endTimeFunc, (struct timezone *) 0);
    (void) aidi_tvsub(&sTimeDiff, &endTimeFunc, &startTimeFunc);
    fSec =
        (float) sTimeDiff.tv_sec + ((float) sTimeDiff.tv_usec / 1000000.0);

    /* for AID_TIMING  print time */
    fprintf(stdout, " ---  %.5f sec time - for executing "
            "aid_establish_context_using_client_delegate() ---  \n", fSec);
#endif

    return 0;

}

/*
 * Modifying aid_establish_context_serverside() to accept delegated 
 * certificate for GSI only.
 * 
 * To be renamed to aid_establish_context_serverside() later.
 * replaces aid_establish_context_serverside() 
 * and aid_establish_context_serverside_with_delegate()
 */
int aid_establish_context_serverside_consolidated(fd, clientName,
                                                  max_len_clientName,
                                                  deleg_flag, type_flag)
int fd;
char *clientName;
int max_len_clientName;
int deleg_flag;
int type_flag;
{
    gss_buffer_desc send_buffer, recv_buffer;
    gss_buffer_desc client_name;
    gss_name_t client;
    gss_OID doid;
    OM_uint32 maj_stat, min_stat;
    int i;

#if defined(AID_TIMING)
    struct timeval startTimeFunc, endTimeFunc, sTimeDiff;
    float fSec;
    /* Starting timer for function */
    (void) gettimeofday(&startTimeFunc, (struct timezone *) 0);

#endif

#if defined(GSI_AUTH) && defined(OBJ_CREATE_NEEDED)
    if (aid_obj_created < 10) {
        int obj_cre_val;
        /* create the SDSC UID object (in the running version of SSL) */
        obj_cre_val =
            OBJ_create("0.9.2342.19200300.100.1.1", "USERID", "userId");
        fprintf(stderr, "obj_cre_val = %d\n", obj_cre_val);
        aid_obj_created++;
    }
#endif

    if (aidi_check_type(type_flag))
        return (-2);

    if (deleg_flag == 1) {
    /* verify delegated credential handle does not point to any credential */
        if (delegated_cred_gsi[fd] != GSS_C_NO_CREDENTIAL) {

            /* reset delegated credential handle */
            maj_stat = gss_release_cred(&min_stat,
                                        &delegated_cred_gsi[fd]);

            if (maj_stat != GSS_S_COMPLETE) {
                fprintf(stderr,
                        "Unable to release delegated credential  \n");
                return -1;
            }
        }
    }

    context[fd] = GSS_C_NO_CONTEXT;
    recv_buffer.value = &aidi_scratch_buffer;

    do {
        recv_buffer.length = SCRATCH_BUFFER_SIZE;
        if (aidi_rcv_token(fd, &recv_buffer) <= 0) {
            fprintf(stderr,
                    "aid_establish_context_serverside_with_delegate: error receiving token\n");
            return -1;
        }
        if (aidi_debug > 0) {
            fprintf(stderr, "Received token (size=%d): \n",
                    recv_buffer.length);
            aidi_print_token(&recv_buffer);
        }
#if defined(GSI_AUTH) || defined(DCE)
        if (deleg_flag == 1) {
            maj_stat = gss_accept_sec_context(&min_stat, 
                &context[fd], 
                my_creds_gsi_dce, 
                &recv_buffer, 
                GSS_C_NO_CHANNEL_BINDINGS, 
                &client, &doid, &send_buffer, 
                &context_flags, NULL,     /* ignore time_rec */
                &delegated_cred_gsi[fd]); /* received delegated credential */
        } else {
            maj_stat = gss_accept_sec_context(&min_stat, 
                &context[fd], my_creds_gsi_dce, &recv_buffer, 
                GSS_C_NO_CHANNEL_BINDINGS, &client, &doid, &send_buffer, 
                &context_flags, 
                NULL,   /* ignore time_rec */
                NULL);
        }
#endif

        if (maj_stat != GSS_S_COMPLETE
            && maj_stat != GSS_S_CONTINUE_NEEDED) {
            aidi_display_status("accepting context", maj_stat, min_stat,
                                type_flag);
            bzero(aidi_scratch_buffer, SCRATCH_BUFFER_SIZE);
            return -1;
        }

        /* since recv_buffer is not malloc'ed, don't need to call
           gss_release_buffer, instead clear it. */
        bzero(aidi_scratch_buffer, SCRATCH_BUFFER_SIZE);

        if (send_buffer.length != 0) {
            if (aidi_debug > 0) {
                fprintf(stderr,
                        "Sending accept_sec_context token (size=%d):\n",
                        send_buffer.length);
                aidi_print_token(&send_buffer);
            }
            if (aidi_send_token(fd, &send_buffer) < 0) {
                fprintf(stderr, "failure sending token\n");
                return -1;
            }
#if defined(GSI_AUTH) || defined(DCE)
            (void) gss_release_buffer(&min_stat, &send_buffer);
#endif
        }
        if (aidi_debug > 0) {
            if (maj_stat == GSS_S_CONTINUE_NEEDED)
                fprintf(stderr, "continue needed...\n");
        }
    } while (maj_stat == GSS_S_CONTINUE_NEEDED);

    /* possible cleanup of send token later */

    /* convert name (internally represented) to a string */
#if defined(GSI_AUTH) || defined(DCE)
    maj_stat = gss_display_name(&min_stat, client, &client_name, &doid);
#endif

    if (maj_stat != GSS_S_COMPLETE) {
        aidi_display_status("displaying name", maj_stat, min_stat,
                            type_flag);
        return -1;
    }

    i = client_name.length;
    if (max_len_clientName < i)
        i = max_len_clientName;

    strncpy(clientName, client_name.value, i);
    if (max_len_clientName > client_name.length)
        clientName[client_name.length] = '\0';

    /* release the name structure */
#if defined(GSI_AUTH) || defined(DCE)
    maj_stat = gss_release_name(&min_stat, &client);
#endif

    if (maj_stat != GSS_S_COMPLETE) {
        aidi_display_status("releasing name", maj_stat, min_stat,
                            type_flag);
        return -1;
    }
#if defined(GSI_AUTH) || defined(DCE)
    (void) gss_release_buffer(&min_stat, &client_name);
#endif

    type_fd[fd] = type_flag;    /* record the type (GSI/Krb) for this fd */

    if (aidi_debug > 0 && deleg_flag == 1) {
        OM_uint32 lifetime;
        gss_name_t delegated_name = GSS_C_NO_NAME;
        gss_buffer_desc name_buffer;

        maj_stat = gss_inquire_cred(&min_stat,  /* (out) minor status */
                    delegated_cred_gsi[fd],     /* (in) cred handle */
                    &delegated_name,    /* (out) name in cred */
                    &lifetime,  /* (out) lifetime */
                    NULL,       /* (out) cred usage
                                    GSS_C_BOTH
                                    GSS_C_INITIATE
                                    GSS_C_ACCEPT */
                    NULL);      /* (out) mechanisms */
        maj_stat = gss_display_name(&min_stat,
                                    delegated_name, &name_buffer, NULL);
        printf("%s, lifetime %i \n", (char *) name_buffer.value,
               (int) lifetime);
    }
#if defined(AID_TIMING)

    (void) gettimeofday(&endTimeFunc, (struct timezone *) 0);
    (void) aidi_tvsub(&sTimeDiff, &endTimeFunc, &startTimeFunc);
    fSec =
        (float) sTimeDiff.tv_sec + ((float) sTimeDiff.tv_usec / 1000000.0);

    /* for AID_TIMING  print time */
    fprintf(stdout, " ---  %.5f sec time taken for executing "
            "aid_establish_context_serverside_consolidated() ---  \n",
            fSec);
#endif

    return 0;
}
#endif /* end of if defined(DCE) || defined(GSI_AUTH) || defined(Kerberos) */
