include ../../mk/mk.config
include ../../mk/mk.common

srcDir =./src
objDir=./obj
binDir=./bin
dataDir=./data

vpath %.c $(srcDir)

##########################################
CFLAGS = $(MY_CFLAG)

# 
##########################################

##########################################
#
SRB_HOME =   ../..
#
SRB_INCL =   -I$(SRB_HOME)/src/include -I$(SRB_HOME)/src/catalog/include -I$(SRB_HOME)/tape/include
#
SRB_LIBS=$(SRB_HOME)/obj/libSrbClient.a $(LDADD)

ifeq ($(PORTNAME), PORTNAME_osx)
else
SRB_LIBS+=-lm
endif

##########################################

BINARIES =	$(binDir)/testargs \
		$(binDir)/srbhello $(binDir)/srbchkauth 	\
		$(binDir)/srbhostinfo $(binDir)/srbvaultinfo \
		$(binDir)/srbaudittrail \
		$(binDir)/srbftpbrowse \
		$(binDir)/srbmkdir $(binDir)/srblsdir $(binDir)/srbrmdir \
		$(binDir)/srbfcreate $(binDir)/srbfdel $(binDir)/srbfchmod  \
		$(binDir)/srbfstat $(binDir)/srbfget $(binDir)/srbfseek \
		$(binDir)/srbmkcoll $(binDir)/srblscoll $(binDir)/srbmodcoll \
		$(binDir)/srbocreate $(binDir)/srbodsinfo	\
		$(binDir)/srboddinfo $(binDir)/srboget 	\
		$(binDir)/srboticket $(binDir)/srboseek \
		$(binDir)/srbocopy $(binDir)/srbomove 	\
		$(binDir)/srbodel $(binDir)/srbocontainer \
		$(binDir)/srbdbcreate $(binDir)/srbdbdel	\
		$(binDir)/srbdbget $(binDir)/srbdbseek 	\
		$(binDir)/srbopcopy $(binDir)/httpget   \
		$(binDir)/srboappend $(binDir)/srboregister   \
		$(binDir)/srbtabodel 	\
		$(binDir)/srbtaboget $(binDir)/srbtabotruncate
# $(binDir)/srbtabomove $(binDir)/srbtabopcopy \

OBJS	 =      $(objDir)/testargs.o \
                $(objDir)/srbhello.o $(objDir)/srbchkauth.o         \
                $(objDir)/srbhostinfo.o $(objDir)/srbvaultinfo.o \
                $(objDir)/srbaudittrail.o \
                $(objDir)/srbftpbrowse.o \
                $(objDir)/srbmkdir.o $(objDir)/srblsdir.o $(objDir)/srbrmdir.o \
                $(objDir)/srbfcreate.o $(objDir)/srbfdel.o 	\
		$(objDir)/srbfchmod.o \
                $(objDir)/srbfstat.o $(objDir)/srbfget.o $(objDir)/srbfseek.o \
                $(objDir)/srbmkcoll.o $(objDir)/srblscoll.o	\
		$(objDir)/srbmodcoll.o \
                $(objDir)/srbocreate.o $(objDir)/srbodsinfo.o       \
                $(objDir)/srboddinfo.o $(objDir)/srboget.o  \
                $(objDir)/srboticket.o $(objDir)/srboseek.o \
                $(objDir)/srbocopy.o $(objDir)/srbomove.o   \
                $(objDir)/srbodel.o $(objDir)/srbocontainer.o \
                $(objDir)/srbdbcreate.o $(objDir)/srbdbdel.o        \
                $(objDir)/srbdbget.o $(objDir)/srbdbseek.o  \
                $(objDir)/srbopcopy.o $(objDir)/httpget.o    \
		$(objDir)/srboappend.o $(objDir)/srboregister.o     \
		$(objDir)/srbtabodel.o 	\
		$(objDir)/srbtaboget.o $(objDir)/srbtabotruncate.o
# $(objDir)/srbtabomove.o     $(objDir)/srbtabopcopy.o 
LIBOBJS = $(objDir)/srbUserArgs.o

##########################################

default : $(BINARIES)

test : $(BINARIES) testsuite
	testsuite

##########################################

$(LIBOBJS): $(objDir)/%.o: %.c $(srcDir)/srbUserArgs.h
	$(CC) -c $(CFLAGS) -o $@ $< $(SRB_INCL)

$(OBJS): $(objDir)/%.o: %.c $(srcDir)/srbUserArgs.h
	$(CC) -c $(CFLAGS) -o $@ $< $(SRB_INCL)

$(BINARIES): $(binDir)/%: $(objDir)/%.o $(LIBOBJS)
	$(LDR) $(OPTFLAGSC) -o $@ $< $(LIBOBJS) $(SRB_LIBS)

##########################################

clean :
	- rm -rf  $(LIBOBJS) $(OBJS) $(BINARIES)

harc : clear
	cd .. ; larchive /projects/mirage SRBhello SRBhello
	- chmod g+rw /projects/mirage/SRBhello*gz
	- chgrp sdsc /projects/mirage/SRBhello*gz

release : clear
	- rm -rf SRBhello.tar.gz ../SRBhello.tar.gz
	cd .. ; tar cf - SRBhello | gzip > SRBhello.tar.gz ; mv SRBhello.tar.gz SRBhello
	chmod g=rw SRBhello.tar.gz
	chmod o=r SRBhello.tar.gz
	chgrp sdsc SRBhello.tar.gz

webindex : clear
	mkindex -d README.html
	chmod g+rw *index*
	chmod o+r *index*
	chgrp sdsc *index*

