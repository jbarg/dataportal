# Makefile - This is the main makefile for the SRB software


include mk/mk.config
include mk/mk.common

buildDir = .
libSrcDir=$(buildDir)/src/lib
clientSrcDir=$(buildDir)/src/client
objDir=$(buildDir)/obj
binDir=$(buildDir)/bin
dataDir=$(buildDir)/data

vpath %.c $(backSrcDir) $(frontSrcDir) $(libSrcDir) $(mdasSrcDir)       \
$(mdas-srbSrcDir) $(clientSrcDir) $(proxySrcDir) $(tapeSrcDir)


# for the SRB client.

SVR_CFLAGS=$(MY_CFLAG) 
SVR_CFLAGS+=-I$(buildDir)/src/include 

# MDAS_AUTH uses MDAS for authorization

ifdef MDAS_AUTH
SVR_CFLAGS+= -DMDAS_AUTH
endif

ifdef GSI_AUTH
SVR_CFLAGS+= -DGSI_AUTH -I$(AUTH_INCLUDE)
ifeq ($(PORTNAME), PORTNAME_solaris)
CL_LDADD+= $(LIB_GSI_AUTH) $(KRB_LIBS) -z muldefs
else
CL_LDADD+= $(LIB_GSI_AUTH) $(KRB_LIBS)
endif
endif

ifdef ADDR_64BIT
SVR_CFLAGS+= -DADDR_64BIT
endif

ifdef COMM_PORT_NUM_START
SVR_CFLAGS+= -DCOMM_PORT_NUM_START=$(COMM_PORT_NUM_START)
SVR_CFLAGS+= -DCOMM_PORT_NUM_COUNT=$(COMM_PORT_NUM_COUNT)
endif

SVR_CFLAGS+= -DSRB_SVR_ENV

CL_CFLAGS= $(SVR_CFLAGS)


ifdef WRITE_SYNC
SVR_CFLAGS+= -DWRITE_SYNC=1
endif

SVR_CFLAGS+= -I$(buildDir)/src/catalog/include
CL_CFLAGS+= -I$(buildDir)/src/catalog/include

CL_LDADD=-lm -lpthread

ifeq ($(PORTNAME), PORTNAME_solaris)
CL_LDADD+=-lnsl -lsocket 
endif

COBJS = $(objDir)/clAuth.o $(objDir)/clConnect.o $(objDir)/clChksum.o	\
$(objDir)/clExec.o $(objDir)/clMisc.o	$(objDir)/packMsg.o	\
$(objDir)/clStub.o $(objDir)/srb_perror.o  \
$(objDir)/mcatgeneral.o  $(objDir)/crypt1.o 

COBJS+=$(objDir)/clSfo.o

ifdef GSI_AUTH
COBJS+=$(objDir)/aid.o
endif


client:: $(objDir)/libSrbClient.a
	( cd utilities ; $(MAKE) )

clean:
	rm -f $(COBJS) $(objDir)/libSrbClient.a 
	rm -f $(objDir)/clGlobal.o
	( cd utilities ; $(MAKE) clean )


$(COBJS): $(objDir)/%.o: %.c
	$(CC) -c $(CL_CFLAGS) -o $@ $<

$(objDir)/clGlobal.o:	$(clientSrcDir)/clGlobal.c
	$(CC) -c $(CL_CFLAGS) -o $@ $<

$(objDir)/libSrbClient.a: $(COBJS) $(objDir)/clGlobal.o
	$(AR) $(AROPT) $(objDir)/libSrbClient.a $^;  \
	$(RANLIB) $(objDir)/libSrbClient.a

ifeq ($(PORTNAME), PORTNAME_solaris)
$(objDir)/libSRBClient.so: $(COBJS) $(objDir)/clGlobal.o
	$(LDR) -G -o $(objDir)/$(@F) $^
endif
