@echo off
rem set TRANPATH=C:\Documents and Settings\gjd37\My Documents\theDataPortal\dataportalcvs\dataportal\transfer\build\WEB-INF\classes

if ""%1"" == ""-h"" goto help
goto setCD
:help
echo Usage: UrlCopy fromUrl toUrl credentialFile
goto end
 
:setCD
rem if no args dont set cd
if ""%4""=="""" goto run
goto doCD

set CLASSPATH=..\libTransfer\cog-jglobus.jar;..\libTransfer\cryptix.jar;..\libTransfer\cryptix32.jar;..\libTransfer\cryptix-asn1.jar;..\libTransfer\fastreversedns.jar;libTransfer\jass.jar;..\libTransfer\jce-jkd13-177.jar;..\libTransfer\jgss.jar;..\libTransfer\jndi.jar;..\libTransfer\junit.jar;..\libTransfer\log4j-1.2.8.jar;..\libTransfer\puretls.jar;.;..\libTransfer\axis.jar;..\libTransfer\jaxrpc.jar;..\libTransfer\commons-logging.jar;..\libTransfer\commons-discovery.jar;..\libTransfer\saaj.jar

:doCD

cd %4


:run

%JAVA_HOME%\bin\java uk.ac.clrc.dataportal.transfer.Transfer %1 %2 %3 

:end