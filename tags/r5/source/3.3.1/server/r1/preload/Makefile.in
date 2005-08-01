# Makefile - This is the main makefile for the preload SRB software

include ../mk/mk.config
include ../mk/mk.common


preloadDir = .
srcDir=$(preloadDir)/src
objDir=$(preloadDir)/src/lib
srbObjDir=$(preloadDir)/../obj
utilObjDir=$(preloadDir)/../utilities/obj
objDir=$(preloadDir)/obj

vpath %.c $(srcDir)

# CL_CFLAGS=$(MY_CFLAG)
ifeq ($(PORTNAME), PORTNAME_linux)
CL_CFLAGS+=-fPIC
endif
CL_CFLAGS+=-g -D$(PORTNAME) -I$(preloadDir)/include
CL_CFLAGS+=-I$(preloadDir)/../src/include
CL_CFLAGS+= -I$(preloadDir)/../src/catalog/include
CL_CFLAGS+= -I$(preloadDir)/../utilities/include

ifeq ($(PORTNAME), PORTNAME_solaris)
CPP=c++
LDR=cc
CC=cc
endif

ifeq ($(PORTNAME), PORTNAME_linux)
CPP=g++
# LDR=g++
CC=gcc
LDR=gcc
endif

ifeq ($(PORTNAME), PORTNAME_solaris)
# CL_LDADD+=-shared -ldl
CL_LDADD=-G -lsocket -lnsl -lm
endif

ifeq ($(PORTNAME), PORTNAME_linux)
CL_LDADD=-fPIC -shared -lm -lpthread
# CL_LDADD=-shared
endif

ifeq ($(PORTNAME), PORTNAME_aix)
CL_LDADD+=-lm
endif

ifeq ($(PORTNAME), PORTNAME_sgi)
CL_LDADD+=-lm
endif

# The SRB objects
COBJS = $(srbObjDir)/clAuth.o $(srbObjDir)/clConnect.o	\
$(srbObjDir)/clExec.o $(srbObjDir)/clMisc.o $(srbObjDir)/packMsg.o	\
$(srbObjDir)/clStub.o $(srbObjDir)/srb_perror.o $(srbObjDir)/clSfo.o   \
$(srbObjDir)/mcatgeneral.o $(srbObjDir)/clGlobal.o $(srbObjDir)/crypt1.o  \
$(srbObjDir)/clChksum.o	 \
$(utilObjDir)/srbClientUtil.o

ifdef GSI_AUTH
COBJS+=$(srbObjDir)/aid.o
ifeq ($(PORTNAME), PORTNAME_solaris)
CL_LDADD+= $(LIB_GSI_AUTH) $(KRB_LIBS) -z muldefs
else
CL_LDADD+= $(LIB_GSI_AUTH) $(KRB_LIBS)
endif
endif

# Preload objects
POBJS = $(objDir)/srbPrelMain.o $(objDir)/srbPrelMisc.o

TARGETS= $(objDir)/srbPreload.so

all:: $(TARGETS)

clean:
	rm -f $(TARGETS) $(POBJS)
 
$(POBJS): $(objDir)/%.o: %.c
	$(CC) -c $(CL_CFLAGS) -o $@ $<

$(objDir)/srbPreload.so: $(POBJS)
	$(LDR) -o $(objDir)/$(@F) $(COBJS) $^ $(CL_LDADD)
