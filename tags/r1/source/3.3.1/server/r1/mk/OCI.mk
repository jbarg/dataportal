# OCI specific section 

PRODUCT=rdbms
LIBKNLOPT=$(RDBMSHOME)/lib/libknlopt.a
LLIBKNLOPT=-lknlopt

PROCOPT= kkxwtp.o
DDBOPT= ktd.o
PARROPT= kcsm.o
PQOPT= kxfo.o
REPOPT= kkzo.o
SDOPT= kxmwsd.o
SCAFOPT = kksdopt.o
NO_PROCOPT= kkxntp.o
NO_DDBOPT= ksnktd.o
NO_PARROPT= ksnkcs.o
NO_PQOPT= ksnkxf.o
NO_REPOPT= ksnkkz.o
NO_SDOPT= kxmnsd.o
NO_SCAFOPT= kksdnop.o

CONFIG = $(ORACLE_HOME)/rdbms/lib/config.o

RANLIBKNLOPT=
RANLIBORA=
OTHERST=
PSOLIBS=/opt/udlm/lib/libudlm.a
DUMPSOLIBS=-lstublm
GENARGS=
LDFLAGS_ORA=
LIBLISTRDBMS=`cat $(ORACLE_HOME)/rdbms/lib/knlsyslibs`
RDBMSLIBS= $(LLIBORA) $(LLIBKNLOPT) $(LLIBPLSQL)
PLSPECFILES=
STLIBS= $(CONFIG) $(RDBMSLIBS) $(PLSPECFILES) $(LLIBSQLNET) $(LLIBORA) \
	$(LLIBSQLNET) $(CORELIBS) $(LIBEPC) $(LLIBPSO) $(LDLIBS) $(CLIBS) \
	$(LIBLISTRDBMS)

LDFLAGS=-L$(LIBHOME) -L$(ORACLE_HOME)/rdbms/lib
LLIBPSO=`cat $(ORACLE_HOME)/rdbms/lib/psoliblist`
 
LLAIO=

# directory that contain oratypes.h and other oci demo program header files
INCLUDE= -I$(ORACLE_HOME)/rdbms/demo -I$(ORACLE_HOME)/rdbms/public -I.

# module to be used for linking with non-deferred option
NDFOPT= $(ORACLE_HOME)/rdbms/lib/kpundf.o

# flag for linking with non-deferred option (default is deferred mode)
NONDEFER=false

# libraries for linking oci programs
OCILDLIBS= $(LIBCLIENT) $(DEVTTLIBS) $(LLIBCORE)

# object for linking oci programs in Fortran
OCIFOR=$(ORACLE_HOME)/rdbms/lib/ocifor.o
 
# object for linking oci programs in Cobol
OCICOB=$(ORACLE_HOME)/rdbms/lib/ocicob.o

PSOLIBLIST=$(ORACLE_HOME)/rdbms/lib/psoliblist
CLEANPSO=rm -f $(PSOLIBLIST); $(GENPSOLIB)


MAKEFILE = Makefile
