@echo off

cd %4

set CLASSPATH=..\libTransfer\cog-jglobus-ch.jar;..\libTransfer\cryptix.jar;..\libTransfer\cryptix32.jar;..\libTransfer\cryptix-asn1.jar;..\libTransfer\fastreversedns.jar;..\libTransfer\jce-jkd13-177.jar;..\libTransfer\junit.jar;..\libTransfer\log4j-1.2.5.jar;..\libTransfer\puretls.jar;.;..\libTransfer\commons-logging.jar;..\libTransfer\commons-discovery.jar;





%JAVA_HOME%\bin\java uk.ac.clrc.dataportal.transfer.Transfer %1 %2 %3 

