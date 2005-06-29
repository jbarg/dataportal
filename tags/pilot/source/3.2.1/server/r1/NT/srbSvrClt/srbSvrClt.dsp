# Microsoft Developer Studio Project File - Name="srbSvrClt" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=srbSvrClt - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "srbSvrClt.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "srbSvrClt.mak" CFG="srbSvrClt - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "srbSvrClt - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "srbSvrClt - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "srbSvrClt - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "srbSvrClt - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "srbSvrClt - Win32 Release"
# Name "srbSvrClt - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\client\clAuth.c
# End Source File
# Begin Source File

SOURCE=..\..\src\client\clConnect.c
# End Source File
# Begin Source File

SOURCE=..\..\src\client\clExec.c
# End Source File
# Begin Source File

SOURCE=..\..\src\client\clGlobal.c
# End Source File
# Begin Source File

SOURCE=..\..\src\client\clMisc.c
# End Source File
# Begin Source File

SOURCE=..\..\src\client\clSfo.c
# End Source File
# Begin Source File

SOURCE=..\..\src\client\clStub.c
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\comm.c
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\crypt1.c
# End Source File
# Begin Source File

SOURCE=..\..\src\back\distrib.c
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\elog.c
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\mcatgeneral.c
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\misc.c
# End Source File
# Begin Source File

SOURCE=..\..\src\client\packMsg.c
# End Source File
# Begin Source File

SOURCE=..\..\src\client\srb_perror.c
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\srbSecureComm.c
# End Source File
# Begin Source File

SOURCE=..\..\src\catalog\mdas\strct_lib.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
