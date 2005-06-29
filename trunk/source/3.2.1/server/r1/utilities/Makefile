# Generated automatically from Makefile.in by configure.
# Makefile - This is the  makefile for the SRB client src

include ../mk/mk.config
include ../mk/mk.common

buildDir = ..
binDir=bin
adminBinDir=admin-bin
objDir=obj
srcDir =src

vpath %.c $(srcDir)

BINARIES = $(binDir)/SgetR $(binDir)/SgetU $(binDir)/SgetD 	\
	$(binDir)/Scat $(binDir)/Sappend  	\
	$(binDir)/Sreplicate  $(binDir)/Smv 	\
	$(binDir)/Scd $(binDir)/Spwd $(binDir)/Sls $(binDir)/Sphymove \
	$(binDir)/Scp  $(binDir)/Smkdir $(binDir)/Srm $(binDir)/Sinit \
	$(binDir)/Sexit $(binDir)/Schmod $(binDir)/Srmdir 	\
	$(binDir)/SmodD $(binDir)/Stoken $(binDir)/Srmtrash	\
	$(binDir)/Senv  $(binDir)/Sticket $(binDir)/Srmticket 	\
	$(binDir)/Stcat $(binDir)/Stls $(binDir)/SgetT 		\
	$(binDir)/Sregister $(binDir)/Spasswd $(binDir)/Serror 	\
	$(binDir)/Srmcont $(binDir)/Smkcont $(binDir)/Ssyncont	\
	$(binDir)/Slscont $(binDir)/Spcommand $(binDir)/Sreplcont   \
	$(binDir)/Sattrs $(binDir)/Sannotate  $(binDir)/Smeta $(binDir)/Shelp \
	$(binDir)/SmodColl $(binDir)/SgetColl  $(binDir)/Schhost $(binDir)/Schdefres \
	$(binDir)/Ssyncd $(binDir)/Ssfstage $(binDir)/Sufmeta \
        $(binDir)/Szone $(binDir)/Spullmeta $(binDir)/Spushmeta \
        $(binDir)/Sln $(binDir)/Sbkupsrb $(binDir)/Schksum  $(binDir)/Srsync \
	$(binDir)/Ssh

ADMINBIN = $(adminBinDir)/Singestuser $(adminBinDir)/SmodifyUser $(adminBinDir)/Sregisterlocation $(adminBinDir)/Sregisterresource  $(adminBinDir)/SmodR $(adminBinDir)/SdelValue

UPLOADBIN = $(binDir)/Sput
ifdef PARA_OPR
UPLOADBIN+=$(binDir)/Sbload
endif

DNLOADBIN = $(binDir)/Sget
ifdef PARA_OPR
DNLOADBIN+=$(binDir)/Sbunload
endif

ifdef PARA_OPR
BINARIES+=$(binDir)/srbpget $(binDir)/srbpput  	\
	$(binDir)/srbparget $(binDir)/Sbregister 
endif

OBJS = $(objDir)/SgetR.o $(objDir)/SgetU.o $(objDir)/SgetD.o      \
        $(objDir)/Scat.o $(objDir)/Sget.o $(objDir)/Sappend.o         \
        $(objDir)/Sput.o $(objDir)/Sreplicate.o  $(objDir)/Smv.o      \
        $(objDir)/Scd.o $(objDir)/Spwd.o $(objDir)/Sls.o $(objDir)/Sphymove.o \
        $(objDir)/Scp.o  $(objDir)/Smkdir.o $(objDir)/Srm.o $(objDir)/Sinit.o \
        $(objDir)/Sexit.o $(objDir)/Schmod.o $(objDir)/Srmdir.o       \
        $(objDir)/SmodD.o $(objDir)/Stoken.o $(objDir)/Srmtrash.o     \
        $(objDir)/Senv.o  $(objDir)/Sticket.o $(objDir)/Srmticket.o   \
        $(objDir)/Stcat.o $(objDir)/Stls.o $(objDir)/SgetT.o          \
        $(objDir)/Sregister.o $(objDir)/Spasswd.o $(objDir)/Serror.o  \
        $(objDir)/Srmcont.o $(objDir)/Smkcont.o $(objDir)/Ssyncont.o  \
        $(objDir)/Slscont.o $(objDir)/Spcommand.o $(objDir)/Sreplcont.o   \
	$(objDir)/Sattrs.o $(objDir)/Sannotate.o $(objDir)/Smeta.o    \
	$(objDir)/Shelp.o $(objDir)/SmodColl.o $(objDir)/SgetColl.o $(objDir)/Schhost.o \
	$(objDir)/Ssyncd.o $(objDir)/Singestuser.o $(objDir)/Ssfstage.o  $(objDir)/Schdefres.o \
	$(objDir)/Sufmeta.o $(objDir)/Szone.o $(objDir)/Spullmeta.o 	\
	$(objDir)/Spushmeta.o $(objDir)/Sln.o $(objDir)/Sbkupsrb.o \
        $(objDir)/Schksum.o  $(objDir)/Srsync.o $(objDir)/SmodR.o $(objDir)/SmodifyUser.o \
        $(objDir)/Sregisterlocation.o $(objDir)/Sregisterresource.o \
	$(objDir)/SdelValue.o $(objDir)/Ssh.o


ifdef PARA_OPR
OBJS+=$(objDir)/Sbload.o $(objDir)/srbpget.o $(objDir)/srbpput.o	\
	$(objDir)/Sbunload.o $(objDir)/srbparget.o $(objDir)/Sbregister.o
endif

LIBOBJS = $(objDir)/srbClientUtil.o

UL_LIBOBJS=$(objDir)/bloadLib.o
DL_LIBOBJS=$(objDir)/bunloadLib.o

clientObjDir=$(buildDir)/obj
LDADD+=-L$(clientObjDir) -lSrbClient

ifdef SRB_SECURE_COMM
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

ifdef PARA_OPR
LDADD+=-lpthread
endif

all:: $(BINARIES) $(UPLOADBIN) $(DNLOADBIN) $(ADMINBIN)

clean: 
	 rm -f $(OBJS) $(LIBOBJS) $(UL_LIBOBJS) $(DL_LIBOBJS) $(BINARIES) $(UPLOADBIN) $(DNLOADBIN) $(ADMINBIN)


CFLAGS=$(MY_CFLAG)

ifdef MCAT_VERSION
CFLAGS+= -D$(MCAT_VERSION)
endif

CFLAGS+=-I./include -I$(buildDir)/src/include -I$(buildDir)/src/catalog/include -I$(buildDir)/tape/include

ifdef NO_TRASH_SYSTEM
CFLAGS+=-DNO_TRASH_SYSTEM
endif

$(LIBOBJS): $(objDir)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(UL_LIBOBJS): $(objDir)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(DL_LIBOBJS): $(objDir)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJS): $(objDir)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

# SGI doesn't like -lm in the middle of the CC line
ifeq ($(PORTNAME), PORTNAME_sgi)
$(BINARIES): $(binDir)/%: $(objDir)/%.o $(LIBOBJS) $(clientObjDir)/libSrbClient.a 
	$(LDR) -o $@ $< $(LIBOBJS) $(LDADD) -lm
else
$(BINARIES): $(binDir)/%: $(objDir)/%.o $(LIBOBJS) $(clientObjDir)/libSrbClient.a 
	$(LDR) -o $@ $< $(LIBOBJS) $(LDADD)
endif

ifeq ($(PORTNAME), PORTNAME_sgi)
$(UPLOADBIN): $(binDir)/%: $(objDir)/%.o $(LIBOBJS) $(UL_LIBOBJS) $(clientObjDir)/libSrbClient.a
	$(LDR) -o $@ $< $(LIBOBJS) $(UL_LIBOBJS) $(LDADD) -lm
else
$(UPLOADBIN): $(binDir)/%: $(objDir)/%.o $(LIBOBJS) $(UL_LIBOBJS) $(clientObjDir)/libSrbClient.a
	$(LDR) -o $@ $< $(LIBOBJS) $(UL_LIBOBJS) $(LDADD)
endif

ifeq ($(PORTNAME), PORTNAME_sgi)
$(DNLOADBIN): $(binDir)/%: $(objDir)/%.o $(LIBOBJS) $(DL_LIBOBJS) $(clientObjDir)/libSrbClient.a
	$(LDR) -o $@ $< $(LIBOBJS) $(DL_LIBOBJS) $(LDADD) -lm
else
$(DNLOADBIN): $(binDir)/%: $(objDir)/%.o $(LIBOBJS) $(DL_LIBOBJS) $(clientObjDir)/libSrbClient.a
	$(LDR) -o $@ $< $(LIBOBJS) $(DL_LIBOBJS) $(LDADD)
endif 

ifeq ($(PORTNAME), PORTNAME_sgi)
$(ADMINBIN): $(adminBinDir)/%: $(objDir)/%.o $(LIBOBJS) $(DL_LIBOBJS) $(clientObjDir)/libSrbClient.a 
	$(LDR) -o $@ $< $(LIBOBJS) $(DL_LIBOBJS) $(LDADD) -lm
else
$(ADMINBIN): $(adminBinDir)/%: $(objDir)/%.o $(LIBOBJS) $(DL_LIBOBJS) $(clientObjDir)/libSrbClient.a 
	$(LDR) -o $@ $< $(LIBOBJS) $(DL_LIBOBJS) $(LDADD)
endif

# XXXX note: For redhat version 7, have to link with /lib/ld-2.2.so

