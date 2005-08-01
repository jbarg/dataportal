# Makefile for tape routines

include ../mk/mk.config
include ../mk/mk.common

srbDir=..
srbDataDir='"$(srbDir)/data"'

buildDir = .
srcDir=$(buildDir)/src
objDir=$(buildDir)/obj
binDir=$(buildDir)/bin
srbObjDir=$(srbDir)/obj

vpath %.c $(srcDir) $(srbDir)/src/lib

CFLAGS=$(MY_CFLAG)
CFLAGS+= -D$(PORTNAME) -DSRB_DATADIR=$(srbDataDir) -DTAPE_DRIVE
CFLAGS+=-g -I$(buildDir)/include -I$(srbDir)/src/include	\
	-I$(srbDir)/src/catalog/include

LDADD=-lc -lm -lpthread -L$(srbObjDir) -lSrbClient

ifeq ($(PORTNAME), PORTNAME_solaris)
LDADD+=-lnsl -lsocket
endif

ifdef SEA_AUTH
CFLAGS+= -DSEA_AUTH
LDADD+= $(LIB_SEA)
endif

ifdef GSI_AUTH
ifeq ($(PORTNAME), PORTNAME_aix)
LDADD+= $(LIB_GSI_AUTH) $(KRB_LIBS)
else
LDADD+= $(LIB_GSI_AUTH) $(KRB_LIBS) -z muldefs
endif
endif

STANDALONE_OBJ = $(objDir)/standalone.o

TAPE_MISC_OBJ=$(objDir)/srbTapeMisc.o $(objDir)/genTape.o $(objDir)/tapeDvrSw.o $(objDir)/elog.o 

TAPE_APPS_OBJ=$(objDir)/inittape.o $(objDir)/dumptape.o 	\
$(objDir)/lstape.o $(objDir)/tapemeta.o

TARGETS = $(binDir)/inittape $(binDir)/dumptape 	\
$(binDir)/lstape $(binDir)/tapemeta

all:: $(TARGETS)

clean:
	rm -f $(TARGETS) $(TAPE_MISC_OBJ) $(TAPE_APPS_OBJ) $(STANDALONE_OBJ)

$(TAPE_MISC_OBJ): $(objDir)/%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(STANDALONE_OBJ): $(objDir)/%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(TAPE_APPS_OBJ): $(objDir)/%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(TARGETS): $(binDir)/%: $(objDir)/%.o $(TAPE_MISC_OBJ) $(STANDALONE_OBJ)
	$(LDR) -o $@ $^ $(LDADD)
