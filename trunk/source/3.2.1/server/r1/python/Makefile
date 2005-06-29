include ../mk/mk.config
include ../mk/mk.common

buildDir = ..
PY_LIB_DIR = ./lib
SRC_DIR = ./src

CFLAGS=$(MY_CFLAG)
CFLAGS+= -D$(PORTNAME)
CFLAGS+=-I./include -I$(buildDir)/src/include -I$(buildDir)/src/catalog/include -I$(buildDir)/tape/include \
	-I$(buildDir)/utilities/include -I$(buildDir)/python/include

LINK_SRB_LIBS = $(buildDir)/utilities/obj/srbClientUtil.o \
	$(buildDir)/obj/libSrbClient.a 

ifeq ($(PORTNAME), PORTNAME_solaris)
LDADD+=-lnsl -lsocket
endif

ifeq ($(PORTNAME), PORTNAME_solaris)
SO_FLAG = -G
CFLAGS+= -I/usr/local/apps/python/include/python2.1
endif

ifeq ($(PORTNAME), PORTNAME_linux)
SO_FLAG = -shared
CFLAGS+= -I/usr/include/python1.5
endif

ifeq ($(PORTNAME),PORTNAME_sunos)
SO_FLAG = -G
CFLAGS+= -I/usr/local/apps/python/include/python2.1
endif

ifeq ($(PORTNAME),PORTNAME_aix)
CFLAGS+= -I/usr/local/apps/Python-2.1/include/python2.1
SO_FLAG = -G
endif

ifeq ($(PORTNAME),PORTNAME_c90)
endif

ifeq ($(PORTNAME),PORTNAME_sgi)
endif

ifeq ($(CC),gcc)
EXTRA_CFLAG = -fPIC
endif


# SGI doesn't like -lm in the middle of the CC line
ifeq ($(PORTNAME), PORTNAME_osx)
else
ifeq ($(PORTNAME), PORTNAME_sgi)
else
LDADD+=-lm
endif
endif

ifdef GSI_AUTH
ifeq ($(PORTNAME), PORTNAME_aix)
LDADD+= $(LIB_GSI_AUTH) $(KRB_LIBS)
else
ifeq ($(PORTNAME), PORTNAME_c90)
LDADD+= $(LIB_GSI_AUTH) $(KRB_LIBS)
else
LDADD+= $(LIB_GSI_AUTH) $(KRB_LIBS) -z muldefs
endif
endif
endif

LDADD+=-lpthread

# TARGETS

all: $(PY_LIB_DIR)/srb.so

$(PY_LIB_DIR)/srb.so: $(PY_LIB_DIR)/pysrb_util.o $(PY_LIB_DIR)/py_srb.o
	$(CC) $(SO_FLAG) -o $(PY_LIB_DIR)/srb.so $^ $(LINK_SRB_LIBS) $(LDADD)


clean:
	( cd $(PY_LIB_DIR) ; rm -f *.o *.so)


$(PY_LIB_DIR)/pysrb_util.o: $(SRC_DIR)/pysrb_util.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(PY_LIB_DIR)/py_srb.o: $(SRC_DIR)/py_srb.c
	$(CC) $(EXTRA_CFLAG) -c -o $@ $< $(CFLAGS)

