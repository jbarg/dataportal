SHELL=/bin/csh

include ../../mk/mk.config

.SUFFIXES: .java .class

CLASSDIR_LONG = ../classes/edu/sdsc/SrbBrowser
CLASSDIR_SHORT = ../classes
CLASSDIR_SHORT_RUN = classes
CLASSDIR_GLUE = ../classes/edu/sdsc/SrbGlue
CLASSES = `ls $(CLASSDIR_LONG)/*.class | grep -v SrbJavaGlue`
GLUEDIR = ./glue
# for GSI or Secure_Comm need $(GLOBUS_LOCATION)/lib too:
export LD_LIBRARY_PATH = $(GLOBUS_LOCATION)/lib:lib:.:/usr/local/apps/Oracle/lib
myCLASSPATH = ../classes

# TARGETS

all: $(CLASSDIR_GLUE)SrbJavaGlue.class $(CLASSDIR_LONG)/srbBrowser.class

run: $(CLASSDIR_GLUE)SrbJavaGlue.class $(CLASSDIR_LONG)/srbBrowser.class
	( cd .. ; setenv LD_LIBRARY_PATH $(LD_LIBRARY_PATH); $(javaDir)/bin/java -classpath $(CLASSDIR_SHORT_RUN) edu/sdsc/SrbBrowser/srbBrowser )


$(CLASSDIR_GLUE)SrbJavaGlue.class:
	( cd $(GLUEDIR) ; gmake )

clean:
	( rm -f $(CLASSES) )
	( cd $(GLUEDIR) ; gmake clean )

# DEPENDENCIES

$(CLASSDIR_LONG)/srbBrowser.class: srbBrowser.java $(CLASSDIR_LONG)/srbTree.class \
	$(CLASSDIR_LONG)/srbSetACDiag.class	$(CLASSDIR_LONG)/srbNewContainer.class \
	$(CLASSDIR_LONG)/srbDelContainer.class $(CLASSDIR_LONG)/srbSyncContainer.class

$(CLASSDIR_LONG)/srbTree.class: srbTree.java $(CLASSDIR_GLUE)/SrbJavaGlue.class \
	$(CLASSDIR_LONG)/SrbConnObj.class $(CLASSDIR_LONG)/SrbAuthObj.class \
	$(CLASSDIR_LONG)/srbClObj.class $(CLASSDIR_LONG)/SrbElement.class \
        $(CLASSDIR_LONG)/SrbCellRenderer.class

$(CLASSDIR_LONG)/srbClObj.class: srbClObj.java $(CLASSDIR_GLUE)/SrbJavaGlue.class \
        $(CLASSDIR_LONG)/SrbAuthObj.class

$(CLASSDIR_LONG)/SrbAuthObj.class: SrbAuthObj.java

$(CLASSDIR_LONG)/SrbConnObj.class: SrbConnObj.java

$(CLASSDIR_LONG)/SrbElement.class: SrbElement.java

$(CLASSDIR_LONG)/SrbCellRenderer.class: SrbCellRenderer.java

$(CLASSDIR_LONG)/srbSetACDiag.class: srbSetACDiag.java

$(CLASSDIR_LONG)/srbNewContainer.class: srbNewContainer.java

$(CLASSDIR_LONG)/srbDelContainer.class: srbDelContainer.java

$(CLASSDIR_LONG)/srbSyncContainer.class: srbSyncContainer.java


# BUILD COMMANDS

$(CLASSDIR_LONG)/%.class:
	$(javaDir)/bin/javac -g -classpath $(myCLASSPATH) -d $(CLASSDIR_SHORT) $<

