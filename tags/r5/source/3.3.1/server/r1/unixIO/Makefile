# Makefile - This is the Makefile for the client library srbUio, another
# client interface to the SRB (See srbUio.c and srbUio.h for descriptions).
# Run gmake at the top level before you gmake here.
# This is known to work on Linux, but hasn't been tested elsewhere.

TOP = ..

include $(TOP)/mk/mk.config
include $(TOP)/mk/mk.common

CFLAGS =$(MY_CFLAG)
CFLAGS+=-D$(PORTNAME)


INCLUDES = -I $(TOP)/utilities/include -I $(TOP)/src/include -I $(TOP)/src/catalog/include

all:: libsrbUio.a srbUio.o

srbUio.o:	srbUio.c
	$(CC) $(CFLAGS) $(INCLUDES) -c srbUio.c

libsrbUio.a:	srbUio.o
	/bin/rm -f libsrbUio.a
#       start with the libSrbClient library:
	cp $(TOP)/obj/libSrbClient.a libsrbUio.a

#	add the srbUio routines:
	ar -crs libsrbUio.a srbUio.o

#       add in the Smv routines, with main renamed to notmain:
	objcopy --redefine-sym main=notmain $(TOP)/utilities/obj/Smv.o Smv.o
	ar -crs libsrbUio.a Smv.o

#       add in the srbClientUtil routines:
	ar -crs libsrbUio.a $(TOP)/utilities/obj/srbClientUtil.o

#       and add in the clFileSum routine:
	ar -crs libsrbUio.a $(TOP)/obj/clChksum.o

#       make a shared library in case someone wants to use it,
#       include the libSRBClient.a .o routines, srbUio.o, Smv.o (minus main),
#       the srbClientUtil.o routines, and the clChksum.o routines
	$(CC) -o libsrbUio.so -shared $(TOP)/obj/clAuth.o $(TOP)/obj/clConnect.o $(TOP)/obj/clExec.o $(TOP)/obj/clMisc.o $(TOP)/obj/packMsg.o $(TOP)/obj/clStub.o $(TOP)/obj/srb_perror.o $(TOP)/obj/clSfo.o $(TOP)/obj/mcatgeneral.o $(TOP)/obj/crypt1.o $(TOP)/obj/srbSecureComm.o $(TOP)/obj/clGlobal.o srbUio.o Smv.o  $(TOP)/utilities/obj/srbClientUtil.o $(TOP)/obj/clChksum.o

clean::
	$ /bin/rm -f srbUio.o libsrbUio.a Smv.o libsrbUio.so libSRBClient.so
