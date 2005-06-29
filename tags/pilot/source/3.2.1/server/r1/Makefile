# Makefile - This is the main makefile for the SRB software


include mk/mk.config
include mk/mk.common

buildDir = .
backSrcDir=$(buildDir)/src/back
frontSrcDir=$(buildDir)/src/front
libSrcDir=$(buildDir)/src/lib
mdasSrcDir=$(buildDir)/src/catalog/mdas
mdas-srbSrcDir=$(buildDir)/src/catalog/mdas-srb
clientSrcDir=$(buildDir)/src/client
tapeSrcDir=$(buildDir)/tape/src
ifdef ADR_PROXY
proxyDir=$(buildDir)/proxy
ifdef TEST_ADR_PROXY
proxySrcDir=$(proxyDir)/test/src
endif
endif
objDir=$(buildDir)/obj
binDir=$(buildDir)/bin
dataDir=$(buildDir)/data

vpath %.c $(backSrcDir) $(frontSrcDir) $(libSrcDir) $(mdasSrcDir)       \
$(mdas-srbSrcDir) $(clientSrcDir) $(proxySrcDir) $(tapeSrcDir)

ifeq ($(ORACLE_CONNECT), ORACLE_CONNECT_TNS)
DBMS_INCLUDE+= -DORACLE_CONNECT_TNS
endif

ifeq ($(ORACLE_CONNECT), ORACLE_CONNECT_ONS)
DBMS_INCLUDE+= -DORACLE_CONNECT_ONS
endif

ifdef SYB_DBMS
ifdef EX_CTLIB_VERSION
DBMS_INCLUDE+= -DEX_CTLIB_VERSION=$(EX_CTLIB_VERSION)
endif
endif

ifdef UTREE
LIB_NSL=$(UTREEDIR)/lib/libnsltree.a $(UTREEDIR)/lib/libnsl.a   \
$(UTREEDIR)/lib/libint64.a $(UTREEDIR)/lib/liblinket.a  \
$(UTREEDIR)/lib/librpclib.a $(UTREEDIR)/lib/comlib.a
endif

ifdef HPSS
# Note - For HPSS version before 4.1.1, "-l interop -l hpssls -l hpssxdr"
# should be taken out
ifdef NO_DCE
LIB_HPSS=	\
	-L$(HPSS_LIB_DIR)	\
	-lhpssapi       \
	-L$(KRBLIB_PATH)	\
	-lkrb5 -lkrb5crypto -lcom_err -lresolv
ifeq ($(PORTNAME), PORTNAME_solaris)
LIB_HPSS+=-lhpss_extensions $(HPSS_LIB_DIR)/ai_threads.o
else
#LIB_HPSS+=-lhpss_extensions_dce
endif

else
ifeq ($(PORTNAME), PORTNAME_aix)
LIB_HPSS=       \
      -L$(HPSS_LIB_DIR)         \
      -L$(ENCINA_LIB_DIR) \
      -l metadata -l hpssapi -l hpsscs -l handles -l hpsscomm_nolog     \
      -l hsec_nolog -l gss_nolog -l pdata -l hpsslog -l traniod \
      -l interop -l hpssls -l hpssxdr -l hpssavlib  \
      -lEncSfs \
      -lEncina \
      -lEncClient \
      -ldce \
      -ldcepthreads -lc_r
endif
ifeq ($(PORTNAME), PORTNAME_solaris)
LIB_HPSS=       \
      -L$(HPSS_LIB_DIR)         \
      -L$(ENCINA_LIB_DIR) \
      -lhpss  \
      -lEncSfs \
      -lEncina \
      -lEncClient \
      -lc  \
      -lm  \
      -ldce  \
      -z muldefs
endif
endif
endif

ifdef DPSS
LIB_DPSS=  -L$(DPSS_LIB_DIR) -ldpssfs -ldpss -ldpssutils -ljnetutils -lnetlogger -ldb
endif

ifdef ADR_PROXY
ifdef TEST_ADR_PROXY
else
datacutterDir=$(proxyDir)/datacutter
datacutterLibDir=$(datacutterDir)/lib
rtreeDir=$(datacutterDir)/rtree/lib
LIB_ADR =	\
	-L$(datacutterLibDir)	\
	-L$(rtreeDir)	\
	-lDataCutter	\
	-lwrtree -lm
endif
endif

# for the SRB client.

SVR_CFLAGS=$(MY_CFLAG) 
SVR_CFLAGS+=-I$(buildDir)/src/include 

ifdef GSI_AUTH
SVR_CFLAGS+=-I$(AUTH_INCLUDE)
else
ifdef SRB_SECURE_COMM
SVR_CFLAGS+=-I$(AUTH_INCLUDE)
endif
endif

ifdef TAPE_DRIVE
SVR_CFLAGS+=-I$(buildDir)/tape/include
endif

# Timing Studies
ifdef LOGTIMINGSRB
SVR_CFLAGS+= -DLOGTIMINGSRB
endif
ifdef LOGTIMINGMCAT
SVR_CFLAGS+= -DLOGTIMINGMCAT
endif
ifdef LOGTIMINGCLIENT
SVR_CFLAGS+= -DLOGTIMINGCLIENT
endif

# Metadata Access Control
ifdef MDATAACCS
SVR_CFLAGS+= -DMDATAACCS
endif

# MySRB  Control
ifdef MYSRB
SVR_CFLAGS+= -DMYSRB
endif

# HOST_BASED_AUTH for use the host based authorization

ifdef HOST_BASED_AUTH
SVR_CFLAGS+= -DHOST_BASED_AUTH
endif

# MDAS_AUTH uses MDAS for authorization

ifdef MDAS_AUTH
SVR_CFLAGS+= -DMDAS_AUTH
endif

# SEA Auth (SDSC Encryption/Authentication)

ifdef SEA_AUTH
SVR_CFLAGS+= -DSEA_AUTH
LDADD+= $(LIB_SEA)
CL_LDADD+= $(LIB_SEA)
endif

ifdef GSI_AUTH
SVR_CFLAGS+= -DGSI_AUTH
ifeq ($(PORTNAME), PORTNAME_solaris)
CL_LDADD+= $(LIB_GSI_AUTH) $(KRB_LIBS) -z muldefs
else
CL_LDADD+= $(LIB_GSI_AUTH) $(KRB_LIBS)
endif
endif

ifdef SRB_SECURE_COMM
ifeq ($(PORTNAME), PORTNAME_solaris)
CL_LDADD+= $(LIB_GSI_AUTH) $(KRB_LIBS) -z muldefs
else
CL_LDADD+= $(LIB_GSI_AUTH) $(KRB_LIBS)
endif
endif

# LDADD+=-bnoquiet

ifdef MCAT_VERSION
SVR_CFLAGS+= -D$(MCAT_VERSION)
endif

ifdef ADDR_64BIT
SVR_CFLAGS+= -DADDR_64BIT
endif

ifdef COMM_PORT_NUM_START
SVR_CFLAGS+= -DCOMM_PORT_NUM_START=$(COMM_PORT_NUM_START)
SVR_CFLAGS+= -DCOMM_PORT_NUM_COUNT=$(COMM_PORT_NUM_COUNT)
endif

ifdef TAPE_DRIVE
SVR_CFLAGS+= -DTAPE_DRIVE
endif

SVR_CFLAGS+= -DSRB_DATADIR='"../data"' -DSRB_SVR_ENV

CL_CFLAGS= $(SVR_CFLAGS)

ifdef UTREE
SVR_CFLAGS+= -DUTREE=1
endif

ifdef UTREE
SVR_CFLAGS+= -I$(UTREE_HDR_DIR)
endif

ifdef ADS
SVR_CFLAGS+= -DADS=1
endif

ifdef HPSS
ifdef NO_DCE
ifeq ($(PORTNAME), PORTNAME_solaris)
SVR_CFLAGS+= -I$(HPSS_HDR_DIR) -DNO_DCE -DNO_H_ERRNO -Dsolaris -Dsolaris_2_5 \
	     -DTHREADS_ENABLED -DPTHREADS
else
# XXXXX assume linux
SVR_CFLAGS+= -I$(HPSS_HDR_DIR) -DNO_DCE -DNO_H_ERRNO -Dlinux \
           -DTHREADS_ENABLED -DPTHREADS
endif
else
SVR_CFLAGS+= -I$(DCE_HDR_DIR) -I$(ENCINA_HDR_DIR) -I$(HPSS_HDR_DIR)
endif
endif

ifdef HPSS
SVR_CFLAGS+= -DHPSS=2
endif

ifdef WRITE_SYNC
SVR_CFLAGS+= -DWRITE_SYNC=1
endif

ifdef DPSS
SVR_CFLAGS+= -I$(DPSS_HDR_DIR)
endif

ifdef DPSS
SVR_CFLAGS+= -DDPSS=5
endif

ifdef ADR_PROXY
SVR_CFLAGS+= -DADR_PROXY=1
ifdef TEST_ADR_PROXY
SVR_CFLAGS+= -DTEST_ADR_PROXY=1
else
SVR_CFLAGS+= -I$(datacutterDir)/include
endif
endif

ifdef LOCK_OBJ
SVR_CFLAGS+= -DLOCK_OBJ=1
endif

ifdef EXFILE
SVR_CFLAGS+= -DEXFILE=1
endif

SVR_CFLAGS+= -I$(buildDir)/src/catalog/include
CL_CFLAGS+= -I$(buildDir)/src/catalog/include

ifeq ($(PORTNAME), PORTNAME_solaris)
LDADD+= -lgen
CL_LDADD+=-lnsl -lsocket -lm -lpthread
endif

ifeq ($(PORTNAME), PORTNAME_linux)
LDADD+=-lm -lpthread
CL_LDADD+=-lm -lpthread
endif


ifeq ($(PORTNAME), PORTNAME_aix)
ifdef HPSS
# with AIX 5.1, no need for lpthread.
LDADD+=-lm
else
LDADD+=-lm -lpthread
endif
# LDADD+=-lm -lpthread
CL_LDADD+=-lm -lpthread
endif

ifeq ($(PORTNAME), PORTNAME_sgi)
LDADD+=-lm -lpthread
CL_LDADD+=-lm -lpthread
endif
ifdef UTREE
LDADD+= $(LIB_NSL)
endif

ifdef HPSS
LDADD+= $(LIB_HPSS)
endif

ifdef DPSS
LDADD+= $(LIB_DPSS)
endif

ifdef ADR_PROXY
ifdef TEST_ADR_PROXY
else
LDADD+= $(LIB_ADR)
endif
endif

ifdef SRB_MDAS
SVR_CFLAGS+= -DSRB_MDAS=1 -DMCAT
ifdef MDAS_DEBUG
SVR_CFLAGS+= -DDEBUGON
endif
ifdef LEANMCAT
SVR_CFLAGS+= -DLEANMCAT
endif
endif

ifdef DEMO
SVR_CFLAGS+= -DDEMO=1
endif

ifdef DBMS_LIB
LDADD+= $(DBMS_LIB)
endif

ifdef DBMS_INCLUDE
SVR_CFLAGS+= $(DBMS_INCLUDE)
endif


BOBJS = $(objDir)/exfSw.o $(objDir)/unixFileDvr.o 	\
$(objDir)/utFileDvr.o   \
$(objDir)/hpssFileDvr.o   \
$(objDir)/exfFunct.o 	\
$(objDir)/compoundObj.o \
$(objDir)/functionHandler.o $(objDir)/srbObjFunct.o \
$(objDir)/tableAccessMisc.o $(objDir)/logEval.o	\
$(objDir)/dbLobj.o $(objDir)/sockutil.o 	\
$(objDir)/srbServerMisc.o  $(objDir)/srbProxyFunct.o \
$(objDir)/dbTable.o $(objDir)/hpssParaOpr.o

ifdef DPSS
BOBJS+=$(objDir)/dpssFileDvr.o
endif

ifdef TAPE_DRIVE
BOBJS+=$(objDir)/srbTapeMisc.o $(objDir)/genTape.o $(objDir)/tapeDvrSw.o
endif

BOBJS+=$(objDir)/srbServerSfo.o

ifdef ADR_PROXY
ifdef TEST_ADR_PROXY
BOBJS+= $(objDir)/adrTestProxyOpr.o
endif
endif

ifdef DB_Illustra
BOBJS+= $(objDir)/illustraDB.o $(objDir)/illusTableDvr.o $(objDir)/illusDvrLib.o
endif

ifdef DB_DB2
BOBJS+= $(objDir)/srbC_db2_library.o $(objDir)/db2TableDvr.o $(objDir)/db2DvrLib.o
endif

ifdef DB_Oracle
BOBJS+= $(objDir)/srbC_ora_library.o $(objDir)/oraTableDvr.o $(objDir)/oraDvrLib.o
endif

ifdef DB_Mysql
# Just the table driver is supported so we can talk DAI to it, blobs aren't
#BOBJS+= $(objDir)/srbC_mys_library.o $(objDir)/mysTableDvr.o $(objDir)/mysDvrLib.o
BOBJS+= $(objDir)/mysTableDvr.o $(objDir)/mysDvrLib.o
endif

ifdef DB_Informix
# Just the table driver is supported so we can talk DAI to it, blobs aren't
#BOBJS+= $(objDir)/srbC_ifmx_library.o $(objDir)/ifmxTableDvr.o $(objDir)/ifmxDvrLib.o
BOBJS+= $(objDir)/ifmxTableDvr.o $(objDir)/ifmxDvrLib.o
endif

ifdef DB_Sybase
# Just the table driver is supported so we can talk DAI to it, blobs aren't
#BOBJS+= $(objDir)/srbC_syb_library.o $(objDir)/sybTableDvr.o $(objDir)/sybDvrLib.o
BOBJS+= $(objDir)/sybTableDvr.o $(objDir)/sybDvrLib.o
endif

ifdef DB_Postgres
# Just the table driver is supported so we can talk DAI to it, blobs aren't
#BOBJS+= $(objDir)/srbC_psg_library.o $(objDir)/psgTableDvr.o $(objDir)/psgDvrLib.o
BOBJS+= $(objDir)/psgTableDvr.o $(objDir)/psgDvrLib.o
endif

ifdef FTP
BOBJS+= $(objDir)/ftpFileDvr.o
SVR_CFLAGS+= -DFTP=3
endif

ifdef HTTP
BOBJS+= $(objDir)/srbC_httpc_library.o
SVR_CFLAGS+= -DHTTP=4
endif

FLOBJS =
FOBJS = $(FLOBJS) $(objDir)/srbMaster.o

LOBJS = $(objDir)/misc.o $(objDir)/comm.o 	\
$(objDir)/elog.o $(objDir)/distrib.o $(objDir)/auth.o

COBJS = $(objDir)/clAuth.o $(objDir)/clConnect.o $(objDir)/clChksum.o	\
$(objDir)/clExec.o $(objDir)/clMisc.o	$(objDir)/packMsg.o	\
$(objDir)/clStub.o $(objDir)/srb_perror.o  \
$(objDir)/mcatgeneral.o $(objDir)/crypt1.o \
$(objDir)/srbSecureComm.o $(objDir)/gen_uuid.o

COBJS+=$(objDir)/clSfo.o

ifdef GSI_AUTH
COBJS+=$(objDir)/aid.o
endif

LOBJS+= $(objDir)/strct_lib.o

SOBJS = $(BOBJS) $(FLOBJS) $(LOBJS)

ifdef SRB_MDAS
MDAS_OBJ = $(objDir)/srbC_mdas_library.o  \
$(objDir)/srbC_mdas_library2.o	$(objDir)/srbC_mdas_library3.o	\
$(objDir)/srbC_mdas_library6.o	\
$(objDir)/srbC_mdas_library8.o	\
$(objDir)/mdasC_db2_conn.o	\
$(objDir)/mdasC_db2_dcon.o $(objDir)/mdasC_db2_info_inquire.o	\
$(objDir)/mdasC_api.o 	\
$(objDir)/table_names_to_sql.o 
ifdef PSQMCAT
MDAS_OBJ += $(objDir)/mdasC_psq_internal.o
endif
ifdef DB2MCAT
MDAS_OBJ += $(objDir)/mdasC_db2_internal.o
endif
ifdef MYSMCAT
MDAS_OBJ += $(objDir)/mdasC_mys_internal.o
endif
ifdef ORAMCAT
MDAS_OBJ += $(objDir)/mdasC_ora_internal.o
endif
ifdef SYBMCAT
MDAS_OBJ += $(objDir)/mdasC_syb_internal.o
endif
ifdef IFMXMCAT
MDAS_OBJ += $(objDir)/mdasC_ifmx_internal.o
endif
SOBJS += $(MDAS_OBJ)

endif

ifdef ADR_PROXY
ifdef TEST_ADR_PROXY
else
TARGETS = $(datacutterLibDir)/libDataCutter.a
endif
endif

ifeq ($(PORTNAME), PORTNAME_solaris)
TARGETS+= $(binDir)/srbMaster-3.2.1 $(binDir)/srbServer 	\
$(objDir)/libSrbClient.a $(objDir)/libSRBClient.so 	\
$(objDir)/libSrbServer.a
else
TARGETS+= $(binDir)/srbMaster-3.2.1 $(binDir)/srbServer 	\
$(objDir)/libSrbClient.a $(objDir)/libSrbServer.a
endif

CONFIG_FILES = $(dataDir)/MdasConfig $(dataDir)/hostAuthConfig	\
$(dataDir)/hostConfig $(dataDir)/metadata.fkrel		\
$(dataDir)/vaultConfig $(dataDir)/LobjConfig $(dataDir)/mcatHost

all:: $(TARGETS)
	( cd utilities ; $(MAKE) )
ifdef JAVA_GUI
	( cd java ; $(MAKE) )
endif

srb:: $(TARGETS)

client:: $(objDir)/libSrbClient.a

util:
	( cd utilities ; $(MAKE) )

browser:
ifdef JAVA_GUI
	( cd java ; $(MAKE) )
endif

install:: $(TARGETS)
	-mkdir $(installDir);	\
	$(INSTALL) bin $(installDir);	\
	$(INSTALL) data $(installDir);	\
	chmod 755 $(installDir) $(installDir)/data $(installDir)/bin	\
	$(installDir)/bin/*; 	\
	chmod 644 $(installDir)/data/*; 	\
	chmod 600 $(installDir)/data/MdasConfig
	chmod 755 $(installDir)/data/lockDir

clean:
	rm -f $(TARGETS) $(SOBJS) $(MDAS_OBJ)	\
	$(COBJS) $(objDir)/libSrbClient.a $(objDir)/srbServer.o \
	$(objDir)/srbMaster.o $(objDir)/clGlobal.o; \
	( cd utilities ; $(MAKE) clean )
ifdef JAVA_GUI
	( cd java ; $(MAKE) clean )
endif

clean_srb:
	rm -f $(TARGETS) $(SOBJS) \
	$(COBJS) $(objDir)/libSrbClient.a $(objDir)/srbServer.o \
	$(objDir)/srbMaster.o $(objDir)/clGlobal.o
ifdef ADR_PROXY
ifdef TEST_ADR_PROXY
else
	( cd $(datacutterLibDir); $(MAKE) clean )
endif
endif

clean_browser: 
ifdef JAVA_GUI
	( cd java ; $(MAKE) clean )
endif

clean_util:
	( cd utilities ; $(MAKE) clean )

$(SOBJS): $(objDir)/%.o: %.c
	$(CC) -c $(SVR_CFLAGS) -o $@ $<

$(objDir)/srbMaster.o: $(objDir)/%.o: %.c
	$(CC) -c $(SVR_CFLAGS) -o $@ $<

$(objDir)/srbServer.o: $(objDir)/%.o: %.c
	$(CC) -c $(SVR_CFLAGS) -o $@ $<

$(binDir)/srbServer: $(BOBJS) $(LOBJS) $(MDAS_OBJ) $(objDir)/srbServer.o \
$(objDir)/libSrbClient.a
	$(LDR) -o $@ $^ $(LDADD) 

$(datacutterLibDir)/libDataCutter.a:
	( cd $(datacutterDir); $(MAKE) srbsvr; $(MAKE) all )

$(binDir)/srbMaster-3.2.1: $(FOBJS) $(LOBJS) $(MDAS_OBJ) $(objDir)/libSrbClient.a 
	$(LDR) -o $@ $^ $(LDADD)

$(COBJS): $(objDir)/%.o: %.c
	$(CC) -c $(CL_CFLAGS) -o $@ $<

$(objDir)/clGlobal.o:	$(clientSrcDir)/clGlobal.c
	$(CC) -c $(CL_CFLAGS) -o $@ $<

$(objDir)/libSrbClient.a: $(COBJS) $(objDir)/clGlobal.o
	$(AR) $(AROPT) $(objDir)/libSrbClient.a $^;  \
	$(RANLIB) $(objDir)/libSrbClient.a

$(objDir)/libSrbServer.a: $(SOBJS)
	$(AR) $(AROPT) $(objDir)/libSrbServer.a $^;  \
	$(RANLIB) $(objDir)/libSrbServer.a

ifeq ($(PORTNAME), PORTNAME_solaris)
$(objDir)/libSRBClient.so: $(COBJS) $(objDir)/clGlobal.o
	$(LDR) -G -o $(objDir)/$(@F) $^
endif
