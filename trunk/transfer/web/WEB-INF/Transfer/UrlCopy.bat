@echo off

set CLASSPATH=..\libTransfer\cog-jglobus.jar;..\libTransfer\cryptix.jar;..\libTransfer\cryptix32.jar;..\libTransfer\cryptix-asn1.jar;..\libTransfer\fastreversedns.jar;libTransfer\jass.jar;..\libTransfer\jce-jkd13-177.jar;..\libTransfer\jgss.jar;..\libTransfer\jndi.jar;..\libTransfer\junit.jar;..\libTransfer\log4j-1.2.8.jar;..\libTransfer\puretls.jar;.;..\libTransfer\axis.jar;..\libTransfer\jaxrpc.jar;..\libTransfer\commons-logging.jar;..\libTransfer\commons-discovery.jar;..\libTransfer\saaj.jar

cd %4




%JAVA_HOME%\bin\java uk.ac.clrc.dataportal.transfer.Transfer %1 %2 %3 

