@echo off

rem ##########################################################################################
rem # Bing Zhu                                                                               #
rem # San Diego Supercomputer Center                                                         #
rem # University of California: San Diego                                                    #
rem # June, 2000                                                                             #
rem # Last Modified on Jan 27,2005                                                            #
rem ##########################################################################################

set SRB_NT_HOME=%~dp0

rem for VC++ 6.0 users, activate the following line (uncomment it).
rem call "C:\Program Files\Microsoft Visual Studio\VC98\Bin\vcvars32.bat"

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

echo "==========================================================="
echo "  building srbSvrClt ...
echo "==========================================================="
cd %SRB_NT_HOME%\srbSvrClt
nmake /f srbSvrClt.mak CFG="srbSvrClt - Win32 Debug"
nmake /f srbSvrClt.mak CFG="srbSvrClt - Win32 Release"

echo "==========================================================="
echo "  building srbMaster ...
echo "==========================================================="
cd %SRB_NT_HOME%\srbMaster
nmake /f srbMaster.mak CFG="srbMaster - Win32 Debug"
nmake /f srbMaster.mak CFG="srbMaster - Win32 Release"
 
echo "==========================================================="
echo "  building srbServer ...
echo "==========================================================="
cd %SRB_NT_HOME%\srbServer
nmake /f srbServer.mak CFG="srbServer - Win32 Debug"
nmake /f srbServer.mak CFG="srbServer - Win32 Release"

cd %SRB_NT_HOME%

echo " "  
echo build_srb finished !

