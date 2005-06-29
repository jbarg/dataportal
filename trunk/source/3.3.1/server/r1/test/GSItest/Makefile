#
# Makefile for SDSC AID library (Authentication and Integrity of Data).
#
# AID calls Generic Security Service API (GSS-API) routines, either
# the Globus SSLeay/X.509 GSS-API system (Grid Security Infrastructure
# (GSI)) or the Kerberos GSS-API, or both, or DCE.
#
# use gmake on AIX and T3E systems.
#
# This is known to work on Solaris, T3E, T90, SP (sp129).

srcdir=.

#
# Use either -DKerberos to build for Kerberos, -DGSI for GSI, or both.
# Also set GSI=1 or Kerberos=1 for the Makefile.
# For DCE, use MY_DCE=1 (it appears that DCE is defined) and -DDCE
# 
#

GSI_AUTH=1
AID_TIMING=1
CFLAGS=-g -DGSI_AUTH -DAID_TIMING

ifndef $(GLOBUS_LOCATION) 
GLOBUS_LOCATION= /usr/local/apps/nmi-2.1
endif
GSI_INSTALL_TYPE=gcc32
INCLUDES= -I $(GLOBUS_LOCATION)/include/$(GSI_INSTALL_TYPE)
GSI_LIBS = \
	-L $(GLOBUS_LOCATION)/lib \
	-lglobus_gss_assist_$(GSI_INSTALL_TYPE) \
	-lglobus_gssapi_gsi_$(GSI_INSTALL_TYPE) \
	-lglobus_gsi_credential_$(GSI_INSTALL_TYPE) \
	-lglobus_gsi_proxy_core_$(GSI_INSTALL_TYPE) \
	-lglobus_gsi_callback_$(GSI_INSTALL_TYPE) \
	-lglobus_oldgaa_$(GSI_INSTALL_TYPE) \
	-lglobus_gsi_sysconfig_$(GSI_INSTALL_TYPE) \
	-lglobus_gsi_cert_utils_$(GSI_INSTALL_TYPE) \
	-lglobus_openssl_error_$(GSI_INSTALL_TYPE) \
	-lglobus_openssl_$(GSI_INSTALL_TYPE) \
	-lglobus_proxy_ssl_$(GSI_INSTALL_TYPE) \
	-lssl_$(GSI_INSTALL_TYPE) \
	-lcrypto_$(GSI_INSTALL_TYPE) \
	-lglobus_common_$(GSI_INSTALL_TYPE)

ifndef MY_DCE
CC=cc
else
CC=xlc_r4
endif

OSTYPE:=$(shell uname -s)

# -z muldefs is needed since there are multiple definitions of some
# routines (gss* for GSI and for Kerberos for example).  It tells
# ld to ignore multiple defs (i.e. not call it a fatal error).  It seems
# to use the routine from the first listed library, which is what is
# needed.  On some systems '-z muldefs' doesn't work tho, so I'm skipping
# it for now.

# So that both Kerberos and SSL versions of GSS-API can be linked at 
# the same time, the krb5_gss* routines (or generic_gss_release_buffer) 
# are called for Kerberos instead of the gss_* routines.  The gss_*
# routines are the SSL GSS-API routines, the krb5_gss* routines are the
# Kerberos GSS-API routines.  Fortunately, in the Kerberos GSS-API package,
# each gss_* routine calls a krb5_gss* counterpart with the same arguments,
# making this kind of blending possible.

ifndef MY_DCE
#LDFLAGS=-z muldefs
LDFLAGS=
else
LDFLAGS=
endif

# 
# libcryptokrb.a in this directory is a copy of libcrypto.a from the
# Kerberos directory.  This different name is necessary so that it
# will be linked (otherwise it'll stop with the first crypto library
# and some externals will be missing).
#
# the libgcc.a had been needed if linking with cc since the GSI crypto 
# library used a '__umoddi3', but it doesn't seem to be needed any more.
#	-L /usr/local/apps/gcc/lib/gcc-lib/sparc-sun-solaris2.5/2.8.1 -l gcc \
#

KRB_LIBS = \
	-L /usr/sdsc/lib \
	-lgssapi_krb5 -lkrb5 \
	-L /users/sy/schroede/aid \
	-lcryptokrb \
	-lcom_err\

ifeq ($(OSTYPE),SunOS)
OTHER_LIBS = \
	-lnsl -lsocket
else
OTHER_LIBS = 
endif

ifdef MY_DCE
LIBS	= -ldce -lnsl
else
LIBS1    = $(GSI_LIBS) 
ifdef Kerberos
LIBS2	= $(LIBS1) $(KRB_LIBS)
else
LIBS2	= $(LIBS1)
endif
LIBS	= $(LIBS2) $(OTHER_LIBS)
endif

ifdef GSI
INCLUDES = -I /usr/local/apps/nmi-2.1/include/gcc32
endif
ifdef Kerberos
INCLUDES = -I /usr/local/apps/gsi/include
endif
ifdef MY_DCE
INCLUDES = -I /usr/include/dce
endif

#
# The AID library is aid.c.  
# aids.c is a test server, aidc.c a test client, 
# testr a test read server, and testw a test write client.
#

all:: aids aids2 aidc aid.o

aids: aids.o aid.o
	$(CC) -o aids aids.o aid.o $(LDFLAGS) $(LIBS) 

aids2: aids2.o aid.o
	$(CC) -o aids2 aids2.o aid.o $(LDFLAGS) $(LIBS) 

aidc: aidc.o aid.o
	$(CC) -o aidc aidc.o aid.o $(LDFLAGS) $(LIBS)

testr: testr.o aid.o
	$(CC) -o testr testr.o aid.o $(LDFLAGS) $(LIBS)

testw: testw.o aid.o
	$(CC) -o testw testw.o aid.o $(LDFLAGS) $(LIBS)

aids.o:	$(srcdir)/aids.c aid.o
	$(CC) $(CFLAGS) $(INCLUDES) -c $(srcdir)/aids.c

aids2.o:$(srcdir)/aids2.c aid.o
	$(CC) $(CFLAGS) $(INCLUDES) -c $(srcdir)/aids2.c

aidc.o:	$(srcdir)/aidc.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $(srcdir)/aidc.c

testr.o:	$(srcdir)/testr.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $(srcdir)/testr.c

testw.o:	$(srcdir)/testw.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $(srcdir)/testw.c

aid.o:	../../src/lib/aid.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ../../src/lib/aid.c

clean::
	$ /bin/rm aidc aids aids2 aid.o aidc.o aids.o aids2.o 

