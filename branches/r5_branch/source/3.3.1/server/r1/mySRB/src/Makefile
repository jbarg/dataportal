SRBDIR = ../..
include $(SRBDIR)/mk/mk.config
include $(SRBDIR)/mk/mk.common

ifeq ($(PORTNAME), PORTNAME_solaris)
LDADD+=-lnsl -lsocket
RANLIB=/usr/ccs/bin/ranlib
else
RANLIB=ranlib
endif

clientObjDir=$(SRBDIR)/obj
utilObjDir=$(SRBDIR)/utilities/obj
utilBinDir=$(SRBDIR)/utilities/bin
LDADD+= $(utilObjDir)/srbClientUtil.o -L$(clientObjDir) -lSrbClient

ifdef GSI_AUTH
ifeq ($(PORTNAME), PORTNAME_aix)
LDADD+= $(LIB_GSI_AUTH) $(KRB_LIBS)
else
LDADD+= $(LIB_GSI_AUTH) $(KRB_LIBS) -z muldefs
endif
endif

# SGI doesn't like -lm in the middle of the CC line
ifeq ($(PORTNAME), PORTNAME_osx)
else
ifeq ($(PORTNAME), PORTNAME_sgi)
else
LDADD+=-lm
endif
endif

ifeq ($(PORTNAME), PORTNAME_solaris)
ifdef GSI_AUTH
SCRIPT=sample_script_solaris_gsi
else
SCRIPT=sample_script_solaris
endif
else
ifdef GSI_AUTH
SCRIPT=sample_script_linux_gsi
else
SCRIPT=sample_script_linux
endif
endif

ifeq ($(PORTNAME), PORTNAME_solaris)
ifdef GSI_AUTH
TARBALL=GSImySRB5Solaris.tar
else
TARBALL=mySRB5Solaris.tar
endif
else
ifdef GSI_AUTH
TARBALL=GSImySRB5Linux.tar
else
TARBALL=mySRB5Linux.tar
endif
endif

LDADD+=-lpthread

CFLAGS=$(MY_CFLAG)
CFLAGS+= -D$(PORTNAME)

ifdef FED_MCAT
CFLAGS+= -DFED_MCAT
endif

CFLAGS+=-I./include -I$(SRBDIR)/src/include -I$(SRBDIR)/src/catalog/include -I$(SRBDIR)/utilities/include

ifdef SEA_AUTH
CFLAGS+= -DSEA_AUTH
LDADD+= $(LIB_SEA)
endif

OBJECTS = alpha1.o sample_main.o

all: mysrb33.cgi

ship: mysrb33.cgi library package

mysrb33.cgi: $(OBJECTS)
	$(CC) $(OBJECTS) -o mysrb33.cgi $(LDADD)
alpha1.o: alpha1.c
	$(CC) -c alpha1.c $(CFLAGS)
sample_main.o: sample_main.c
	$(CC) -c sample_main.c $(CFLAGS)
library:
	ar cr libMySRB.a alpha1.o $(utilObjDir)/srbClientUtil.o
	$(RANLIB) libMySRB.a 
clean:
	-rm mySRB5Linux.tar.gz mysrb33.cgi libMySRB.a *.o
	-rm -r ship

package:
	-mkdir ship
	-mkdir ship/lib
	cp -R ../lang ship
	cp -R ../webserverdir ship
	cp ../misc/*.cgi ship
	cp ../misc/mySRBsetup.txt ship
	cp sample_main.c ship
	cp $(clientObjDir)/libSrbClient.a ship/lib
	mv libMySRB.a ship/lib
	cp $(utilBinDir)/Serror ship
	cp $(SCRIPT) ship/sample_script
	tar -cvf $(TARBALL) ship/*
	gzip -9 $(TARBALL)
