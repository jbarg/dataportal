rem @echo off

rem ---------------------------------------------------------------
rem Script to set Classpath For Matrix Client
rem author Matrix Team
rem ---------------------------------------------------------------


rem ----- Verify and Set Required Environment Variables ---------------------
if not "%MATRIX_HOME%" == "" goto gotMatrixHome
echo [Information] MATRIX_HOME not set. Assuming .\.. 
set MATRIX_HOME=.\..
:gotMatrixHome
goto allSet


:checkCATALINA
if not "%CATALINA_HOME%" == "" goto allSet
echo [Error] CATALINA_HOME not set
echo [Error] Tomcat home not found.
goto Abort


rem ----------------- set Classpath -------------------------------------
:allSet
set _CPATH=%CLASSPATH%
set CLASSPATH=%MATRIX_HOME%\examples;%MATRIX_HOME%\classes
rem set CLASSPATH=%CLASSPATH%;%CATALINA_HOME%\common\lib\activation.jar;%CATALINA_HOME%\server\lib\commons-logging.jar
set CLASSPATH=%CLASSPATH%;%MATRIX_HOME%\lib\clientLib\activation.jar;%MATRIX_HOME%\lib\clientLib\commons-logging.jar
set CLASSPATH=%CLASSPATH%;%MATRIX_HOME%\lib\dom4j.jar;%MATRIX_HOME%\lib\jargon.jar
set CLASSPATH=%CLASSPATH%;%MATRIX_HOME%\lib\jaxb-api.jar;%MATRIX_HOME%\lib\jaxb-libs.jar
set CLASSPATH=%CLASSPATH%;%MATRIX_HOME%\lib\jaxb-ri.jar;%MATRIX_HOME%\lib\jaxb-xjc.jar
set CLASSPATH=%CLASSPATH%;%MATRIX_HOME%\lib\jaxm-api.jar;%MATRIX_HOME%\lib\jaxm-runtime.jar
set CLASSPATH=%CLASSPATH%;%MATRIX_HOME%\lib\jax-qname.jar;%MATRIX_HOME%\lib\clientLib\mail.jar
set CLASSPATH=%CLASSPATH%;%MATRIX_HOME%\lib\saaj-api.jar;%MATRIX_HOME%\lib\saaj-ri.jar
rem set CLASSPATH=%CLASSPATH%;%CATALINA_HOME%\common\lib\servlet.jar;
echo [Information] Using classpath %CLASSPATH%

rem the switch..
goto pre-appendCP


rem ----------------- Pre-Append new CP to existing --------------------------
:pre-appendCP
set CLASSPATH=%CLASSPATH%;%_CPATH%
goto end

rem ------------------Run stuff ------------------------------------------

rem ----------------- clean up --------------------------------------------
:cleanup
set CLASSPATH=%_CPATH%
goto end


rem ------------------Run stuff ------------------------------------------
:Abort
echo [Error] Aborting ...
goto end

rem ----------------- clean up --------------------------------------------
:end
