@echo off

rem ##########################################################################################
rem # Bing Zhu                                                                               #
rem # San Diego Supercomputer Center                                                         #
rem # University of California: San Diego                                                    #
rem # June, 2000                                                                             #
rem # Modified on Jan 7,2003                                                                 #
rem ##########################################################################################

set SRB_NT_HOME=%~dp0

rem for VC++ 6.0 users, activate the following line (uncomment it).
call "C:\Program Files\Microsoft Visual Studio\VC98\Bin\vcvars32.bat"

echo "==========================================================="
echo "  building srbNtUtil ...
echo "==========================================================="
cd %SRB_NT_HOME%\srbNtUtil
nmake /f srbNtUtil.mak CFG="srbNtUtil - Win32 Debug"
nmake /f srbNtUtil.mak CFG="srbNtUtil - Win32 Release"

echo "==========================================================="
echo "  building SrbClient ...
echo "==========================================================="
cd %SRB_NT_HOME%\SrbClient
nmake /f srbClient.mak CFG="srbClient - Win32 Debug"
nmake /f srbClient.mak CFG="srbClient - Win32 Release"

echo "==========================================================="
echo "  building Scommands ...
echo "==========================================================="
cd %SRB_NT_HOME%\scommands
nmake

cd %SRB_NT_HOME%

echo " "  
echo build_scmds finished !

