@echo off

rem ---------------------------------------------------------------
rem Script to set Classpath For Matrix Client
rem author Arun swaran Jagatheesan
rem ---------------------------------------------------------------


rem ----- Verify and Set Required Environment Variables ---------------------

if not "%MATRIX_HOME%" == "" goto gotMatrixHome
echo [Information] MATRIX_HOME not set. Assuming .\..
set MATRIX_HOME=.\..
:gotMatrixHome
echo Using MATRIX Home directory = %MATRIX_HOME%
goto allSet


rem ----------------- set Classpath -------------------------------------
:allSet
rem backup current Class path
set _CPATH=%CLASSPATH%
set CLASSPATH=.;%MATRIX_HOME%\examples;%MATRIX_HOME%\bin
set CLASSPATH=%CLASSPATH%;%MATRIX_HOME%\bin\matrixclient.jar
set CLASSPATH=%CLASSPATH%;%MATRIX_HOME%\lib\dom4j.jar;%MATRIX_HOME%\lib\xml-apis.jar
set CLASSPATH=%CLASSPATH%;%MATRIX_HOME%\lib\jax-qname.jar
set CLASSPATH=%CLASSPATH%;%MATRIX_HOME%\lib\jaxb-api.jar;%MATRIX_HOME%\lib\jaxb-impl.jar;%MATRIX_HOME%\lib\jaxb-libs.jar;%MATRIX_HOME%\lib\jaxb-xjc.jar
set CLASSPATH=%CLASSPATH%;%MATRIX_HOME%\lib\jaxm-api.jar;%MATRIX_HOME%\lib\jaxm-runtime.jar
set CLASSPATH=%CLASSPATH%;%MATRIX_HOME%\lib\namespace.jar;%MATRIX_HOME%\lib\relaxngDatatype.jar
set CLASSPATH=%CLASSPATH%;%MATRIX_HOME%\lib\saaj-api.jar;%MATRIX_HOME%\lib\saaj-impl.jar
set CLASSPATH=%CLASSPATH%;%MATRIX_HOME%\lib\xercesImpl.jar;%MATRIX_HOME%\lib\xsdlib.jar

rem most of the stuff are needed only for server. just included to run MatrixTester during development
set CLASSPATH=%CLASSPATH%;%MATRIX_HOME%\lib\activation.jar;%MATRIX_HOME%\lib\commons-logging.jar;%MATRIX_HOME%\lib\mail.jar
set CLASSPATH=%CLASSPATH%;%MATRIX_HOME%\lib\jargon.jar
set CLASSPATH=%CLASSPATH%;%MATRIX_HOME%\lib\cog-jglobus.jar;

echo [Information] Using classpath = %CLASSPATH%

rem the switch..
goto pre-appendCP


rem ----------------- Pre-Append new CP to existing --------------------------
:pre-appendCP
rem :: To have only the new Classpath change the line below to: set CLASSPATH=%CLASSPATH%;
set CLASSPATH=%CLASSPATH%;%_CPATH%
rem echo skiping cleanup
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
