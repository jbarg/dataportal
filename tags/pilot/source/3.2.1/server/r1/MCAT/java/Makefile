include ../../mk/mk.config

# If javaDir is defined (via configure), then use the java under that
# directory, otherwise use just "java" to use the one in the path (if
# there is one).
ifdef $(javaDir)
myjava = $(javaDir)/bin/java
else
myjava = java
endif

ifeq ($(PORTNAME), PORTNAME_osx)
# On OS X, make is actually gmake
myMake=make
else
myMake=gmake
endif

CLASSDIR_SHORT = classes
CLASSDIRS = $(CLASSDIR_SHORT):../../java/classes:../../jargon/bin

INSTALL_DIR = ./bin

SRCS=\
src/mcatAddLogResc.java \
src/mcatAddToCompResc.java \
src/mcatAdmin.java \
src/mcatDelLoc.java \
src/mcatDelLogResc.java \
src/mcatDelResc.java \
src/mcatDelUser.java \
src/mcatDelZone.java \
src/mcatModUser.java \
src/mcatModZone.java \
src/mcatNewCompoundResc.java \
src/mcatNewDomain.java \
src/mcatNewGroup.java \
src/mcatNewLoc.java \
src/mcatNewLogResc.java \
src/mcatNewResource.java \
src/mcatNewToken.java \
src/mcatNewUser.java \
src/mcatNewZone.java \
src/mcatShowRescsDetail.java \
src/mcatShowRescs.java \
src/mcatShowTokens.java \
src/mcatShowUsers.java \
src/mcatShowZonesDetail.java \
src/mcatShowZones.java \
src/mcatRmFromLogResc.java \
src/mcatConnect.java \
src/SrbCellRenderer.java \
src/SrbElement.java \

#
# if we're doing GSI, we'll need $(GLOBUS_LOCATION)/lib.
#
export LD_LIBRARY_PATH = $(GLOBUS_LOCATION)/lib

all:	mcatAdmin.jar

mcatAdmin.jar:	$(SRCS)
	( cd src ; $(myMake))
	jar cmf manifest.txt mcatAdmin.jar -C classes edu \
            -C ../../jargon/bin edu/sdsc/grid

clean:
	( cd src ; $(myMake) clean )
	rm -f mcatAdmin.jar

run: 	mcatAdmin.jar
	$(myjava) -jar mcatAdmin.jar
