# Makefile - This is the main makefile for the SRB software

include ../mk/mk.config
include ../mk/mk.common

buildDir = ..
adminSrcDir=$(buildDir)/admin/src
binDir=$(buildDir)/admin/bin
objDir=$(buildDir)/admin/obj
libDir=$(buildDir)/obj

vpath %.c $(adminSrcDir)

# for the SRB client.

CL_CFLAGS = -g
CL_CFLAGS+= -I$(buildDir)/src/include
CL_CFLAGS+= -I$(buildDir)/src/catalog/include
CL_CFLAGS+= -I$(adminSrcDir)
CL_CFLAGS+= -DMON_DATA_DIR='"../data"'

CL_CFLAGS+=-D$(PORTNAME)

# SGI doesn't like -lm in the middle of the CC line
ifeq ($(PORTNAME), PORTNAME_osx)
else
ifeq ($(PORTNAME), PORTNAME_sgi)
else
CL_LDADD+=-lm
endif
endif

CL_LDADD+=-lpthread

ifeq ($(PORTNAME), PORTNAME_solaris)
CL_LDADD+=-lnsl -lsocket
endif

# SEA Auth (SDSC Encryption/Authentication)

ifdef SEA_AUTH
CL_CFLAGS+= -DSEA_AUTH
CL_LDADD+= $(LIB_SEA)
endif

ifdef GSI_AUTH
CL_LDADD+= $(LIB_GSI_AUTH) $(KRB_LIBS) -z muldefs
endif

ADMIN_OBJ = $(objDir)/srbMon.o

EXAMBIN = $(binDir)/srbMon

TARGETS:= $(EXAMBIN)

all:: $(TARGETS)

clean:
	rm -f $(TARGETS) $(ADMIN_OBJ)
 
$(ADMIN_OBJ): $(objDir)/%.o: %.c
	$(CC) -c $(CL_CFLAGS) -o $@ $<

#	$(CC) -o $@ $^ $(libDir)/libSrbClient.a $(libDir)/libSrbServer.a $(CL_LDADD)
$(EXAMBIN): $(binDir)/%: $(ADMIN_OBJ)
	$(CC) -o $@ $^ $(libDir)/libSrbClient.a $(CL_LDADD)
