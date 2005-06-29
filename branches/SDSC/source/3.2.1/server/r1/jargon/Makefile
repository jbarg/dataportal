#
#  NAME
#	Makefile	-  manage project compilation
#
#  DESCRIPTION
#	This Makefile manages building part or all of the API
#	packages, command-line tools, and documentation.
#
#  PRINCIPAL AUTHOR
#	Lucas Ammon Gilbert, San Diego Supercomputer Center
#
#  TARGETS
#	Compiling:
#		all			-  make the class files
#		lib			-  make the libraries
#
#	Cleaning:
#		clean		-  clean out all .class's
#
#	Admin:
#		doc		-  build the API documentation
#
.POSIX:
.SILENT:
.SUFFIXES:
.SUFFIXES:	.class .java
SHELL =	/bin/sh


#  Release title and version
VERSION =	"v1.3"
TITLE =		"JARGON"



########################################################################
#  HELP
########################################################################
.DEFAULT:
	echo "Unknown target:  $@"
	make help

usage:	help
	true

help:
	echo "Usage is:  make [targets...]"
	echo "  General build targets:"
	echo "    all          -  build all class files (with previous default)"
	echo "    gsi          -  build all class files with gsi enabled"
	echo "    notgsi       -  build all class files without gsi enabled"
	echo "    jar          -  create the jargon.jar"
	echo "  Admin targets:"
	echo "    doc          -  automatically build distributed documentation"
	echo "    clean        -  clean out built obj files for this host type"
	echo "    size         -  compute the number of lines of source code"




########################################################################
#  JAVA CONTROLS
########################################################################

#Change to your path. Use java version 1.4 and higher.
JAVADIR =
#/usr/local/apps/jdk1.4.1/bin/



JAVAC = $(JAVADIR)javac -d $(BIN) -classpath $(LIB)
JAVA = $(JAVADIR)java -classpath .:$(LIB)jargon.jar:$(BIN)




########################################################################
#  DIRECTORY AND FILE LOCATIONS
########################################################################


HOME = ./


BIN =		$(HOME)bin/
DOC = 		$(HOME)doc/
LIB =		$(HOME)lib/
APISRCDIR =	$(HOME)src/api/
TESTSRCDIR =	$(HOME)src/test/
PACKAGES = edu/sdsc/grid/
IOSRBDIR = $(PACKAGES)io/srb/

APISRC =	$(APISRCDIR)$(PACKAGES)io/local/*.java \
		$(APISRCDIR)$(IOSRBDIR)*.java \
		$(APISRCDIR)$(PACKAGES)io/*.java \
		$(APISRCDIR)$(PACKAGES)gui/*.java



CLASSES =	$(BIN)$(PACKAGES)local/*.class \
			$(BIN)$(IOSRBDIR)*.class \
			$(BIN)$(PACKAGES)*.class \
			$(BIN)*.class



########################################################################
#  MISC TARGETS
########################################################################
all:
	make api
	make test
	make jar

api:
	$(JAVAC) -classpath $(BIN):$(LIB)gsi-classes.jar $(APISRC)

gsi:	lib/gsi-classes.jar
	rm -f $(APISRCDIR)$(IOSRBDIR)GSIAuth.java
	cp $(APISRCDIR)$(IOSRBDIR)GSIAuth.java_real $(APISRCDIR)$(IOSRBDIR)GSIAuth.java
	make all

lib/gsi-classes.jar:
	echo "For GSI Authentication, download the gsi-classes.jar"
	echo "Available at http://www.npaci.edu/DICE/srb/jargon"
	false

notgsi:
	rm -f $(APISRCDIR)$(IOSRBDIR)GSIAuth.java
	cp $(APISRCDIR)$(IOSRBDIR)GSIAuth.java_fake $(APISRCDIR)$(IOSRBDIR)GSIAuth.java
	make all


#delete all the class and the jar files
clean:
	rm -rf $(CLASSES)
	rm -rf $(HOME)lib/jargon.jar

#delete everything created by this makefile
veryclean:
	rm -rf $(CLASSES)
	rm -rf $(BIN)edu/
	rm -rf $(DOC)*.html
	rm -rf $(DOC)package-list
	rm -rf $(DOC)edu/
	rm -rf $(LIB)jargon.jar

size:
	echo "Calculating source file sizes (line numbers)"
	find $(APISRCDIR) -name '*.java' -exec cat {} \; | wc -l



########################################################################
#  TEST TARGETS
########################################################################
test:
	$(JAVAC) -classpath $(BIN) $(TESTSRCDIR)*.java




########################################################################
#  DOCUMENTATION BUILD TARGETS
########################################################################
docs:
	$(JAVADIR)javadoc -link http://java.sun.com/jdk/1.4/docs/api \
	-d $(DOC) \
	-overview $(APISRCDIR)overview.html \
	-stylesheetfile $(DOC)jargon.css \
	$(APISRC)

pDocs:
	$(JAVADIR)javadoc -private -link http://java.sun.com/jdk/1.4/docs/api \
	-d $(DOC) \
	-overview $(APISRCDIR)overview.html \
	-stylesheetfile $(DOC)jargon.css \
	$(APISRC)



########################################################################
#  MULTI-PACKAGE BUILD TARGETS
########################################################################
jar:
	$(JAVADIR)jar cvfm $(LIB)jargon.jar $(LIB)MANIFEST.MF -C $(BIN) .
#need to add proper manifest
#	sign -o $(LIB)jargon.jar $(LIB)jargon_unsigned.jar




########################################################################
#  CONFIGURATION TARGETS
########################################################################
