@echo off

if ""%1"" ==""-help"" goto help
if ""%1"" ==""-h"" goto help

rem if no args dont set cd
if ""%1""=="""" goto compile
goto doCD

:help
echo Usage: compile.bat workingDir
echo        if no workingDir, default is .
goto end

:doCD

cd %1

:compile

set CLASSPATH=;..\libTransfer\cog-jglobus.jar;..\libTransfer\cryptix.jar;..\libTransfer\cryptix32.jar;..\libTransfer\cryptix-asn1.jar;..\libTransfer\fastreversedns.jar;libTransfer\jass.jar;..\libTransfer\jce-jkd13-177.jar;..\libTransfer\jgss.jar;..\libTransfer\jndi.jar;..\libTransfer\junit.jar;..\libTransfer\log4j-1.2.8.jar;..\libTransfer\puretls.jar;.;..\libTransfer\axis.jar;..\libTransfer\jaxrpc.jar;..\libTransfer\commons-logging.jar;..\libTransfer\commons-discovery.jar;..\libTransfer\saaj.jar;..\libTransfer\servletapi-2.3.jar



%JAVA_HOME%\bin\javac uk\ac\clrc\dataportal\transfer\*.java
%JAVA_HOME%\bin\javac uk\ac\clrc\dataportal\*.java
 

:end