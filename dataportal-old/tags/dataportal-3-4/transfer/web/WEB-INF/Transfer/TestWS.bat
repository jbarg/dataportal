@echo off


if ""%1"" == ""-h"" goto help
if ""%1"" == ""-sid"" goto run2
goto run

:help
echo Usage: TestWS sessionmanagerUrl sid datatransferURL fromUrl toUrl
goto end

:run

set CLASSPATH=;..\libTransfer\cog-jglobus.jar;..\libTransfer\cryptix.jar;..\libTransfer\cryptix32.jar;..\libTransfer\cryptix-asn1.jar;..\libTransfer\fastreversedns.jar;libTransfer\jass.jar;..\libTransfer\jce-jkd13-177.jar;..\libTransfer\jgss.jar;..\libTransfer\jndi.jar;..\libTransfer\junit.jar;..\libTransfer\log4j-1.2.8.jar;..\libTransfer\puretls.jar;.;..\lib\axis.jar;..\lib\jaxrpc.jar;..\libTransfer\commons-logging.jar;..\libTransfer\commons-discovery.jar;..\lib\saaj.jar;..\libTransfer\servletapi-2.3.jar


%JAVA_HOME%\bin\java uk.ac.clrc.dataportal.TestTransfer %1 %2 %3 %4 %5
goto end

:run2

set CLASSPATH=;..\libTransfer\cog-jglobus.jar;..\libTransfer\cryptix.jar;..\libTransfer\cryptix32.jar;..\libTransfer\cryptix-asn1.jar;..\libTransfer\fastreversedns.jar;libTransfer\jass.jar;..\libTransfer\jce-jkd13-177.jar;..\libTransfer\jgss.jar;..\libTransfer\jndi.jar;..\libTransfer\junit.jar;..\libTransfer\log4j-1.2.8.jar;..\libTransfer\puretls.jar;.;..\lib\axis.jar;..\lib\jaxrpc.jar;..\libTransfer\commons-logging.jar;..\libTransfer\commons-discovery.jar;..\lib\saaj.jar;..\libTransfer\servletapi-2.3.jar;..\libTransfer\log4j-1.2.5.jar

call setEnv.bat

%JAVA_HOME%\bin\java uk.ac.clrc.dataportal.TestTransfer %SM% %2 %DT% %fromURL% %toURL%


:end