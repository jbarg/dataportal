Had many problems with juddi and getting the facility module to work.

In the end i put all axis jars in shared/lib

axis
com-dis
com-log
log4j
jaxrpc
saaj

along with the tab libs jars

standard
jstl

if the axis was in the facility, it still worked.  If they were in the common it did not work..

the files needed for faciltiy in web-lib were

dp_common
jdom_1.0
soap
uddi4j
wasd4j
xml-dl.1.5.4