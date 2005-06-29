# Microsoft Developer Studio Generated NMAKE File, Based on srbServer.dsp
!IF "$(CFG)" == ""
CFG=srbServer - Win32 Debug
!MESSAGE No configuration specified. Defaulting to srbServer - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "srbServer - Win32 Release" && "$(CFG)" != "srbServer - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "srbServer.mak" CFG="srbServer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "srbServer - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "srbServer - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "srbServer - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\srbServer.exe"


CLEAN :
	-@erase "$(INTDIR)\auth.obj"
	-@erase "$(INTDIR)\compoundObj.obj"
	-@erase "$(INTDIR)\dbLobj.obj"
	-@erase "$(INTDIR)\dbTable.obj"
	-@erase "$(INTDIR)\exfFunct.obj"
	-@erase "$(INTDIR)\exfSw.obj"
	-@erase "$(INTDIR)\ftpFileDvr.obj"
	-@erase "$(INTDIR)\functionHandler.obj"
	-@erase "$(INTDIR)\logEval.obj"
	-@erase "$(INTDIR)\ntFileDvr.obj"
	-@erase "$(INTDIR)\sockutil.obj"
	-@erase "$(INTDIR)\srbC_httpc_library.obj"
	-@erase "$(INTDIR)\srbObjFunct.obj"
	-@erase "$(INTDIR)\srbProxyFunct.obj"
	-@erase "$(INTDIR)\srbServer.obj"
	-@erase "$(INTDIR)\srbServerMisc.obj"
	-@erase "$(INTDIR)\srbServerSfo.obj"
	-@erase "$(INTDIR)\tableAccessMisc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\srbServer.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\srbServer.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib srbSvrClt.lib srbNtUtil.lib ws2_32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\srbServer.pdb" /machine:I386 /out:"$(OUTDIR)\srbServer.exe" /libpath:"..\srbSvrClt\Release" /libpath:"..\srbNtUtil\Release" 
LINK32_OBJS= \
	"$(INTDIR)\auth.obj" \
	"$(INTDIR)\compoundObj.obj" \
	"$(INTDIR)\dbLobj.obj" \
	"$(INTDIR)\dbTable.obj" \
	"$(INTDIR)\exfFunct.obj" \
	"$(INTDIR)\exfSw.obj" \
	"$(INTDIR)\ftpFileDvr.obj" \
	"$(INTDIR)\functionHandler.obj" \
	"$(INTDIR)\logEval.obj" \
	"$(INTDIR)\ntFileDvr.obj" \
	"$(INTDIR)\sockutil.obj" \
	"$(INTDIR)\srbC_httpc_library.obj" \
	"$(INTDIR)\srbObjFunct.obj" \
	"$(INTDIR)\srbProxyFunct.obj" \
	"$(INTDIR)\srbServer.obj" \
	"$(INTDIR)\srbServerMisc.obj" \
	"$(INTDIR)\srbServerSfo.obj" \
	"$(INTDIR)\tableAccessMisc.obj"

"$(OUTDIR)\srbServer.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "srbServer - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\srbServer.exe"


CLEAN :
	-@erase "$(INTDIR)\auth.obj"
	-@erase "$(INTDIR)\compoundObj.obj"
	-@erase "$(INTDIR)\dbLobj.obj"
	-@erase "$(INTDIR)\dbTable.obj"
	-@erase "$(INTDIR)\exfFunct.obj"
	-@erase "$(INTDIR)\exfSw.obj"
	-@erase "$(INTDIR)\ftpFileDvr.obj"
	-@erase "$(INTDIR)\functionHandler.obj"
	-@erase "$(INTDIR)\logEval.obj"
	-@erase "$(INTDIR)\ntFileDvr.obj"
	-@erase "$(INTDIR)\sockutil.obj"
	-@erase "$(INTDIR)\srbC_httpc_library.obj"
	-@erase "$(INTDIR)\srbObjFunct.obj"
	-@erase "$(INTDIR)\srbProxyFunct.obj"
	-@erase "$(INTDIR)\srbServer.obj"
	-@erase "$(INTDIR)\srbServerMisc.obj"
	-@erase "$(INTDIR)\srbServerSfo.obj"
	-@erase "$(INTDIR)\tableAccessMisc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\srbServer.exe"
	-@erase "$(OUTDIR)\srbServer.ilk"
	-@erase "$(OUTDIR)\srbServer.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\srbServer.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib srbSvrClt.lib srbNtUtil.lib ws2_32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\srbServer.pdb" /debug /machine:I386 /out:"$(OUTDIR)\srbServer.exe" /pdbtype:sept /libpath:"..\srbSvrClt\Debug" /libpath:"..\srbNtUtil\Debug" 
LINK32_OBJS= \
	"$(INTDIR)\auth.obj" \
	"$(INTDIR)\compoundObj.obj" \
	"$(INTDIR)\dbLobj.obj" \
	"$(INTDIR)\dbTable.obj" \
	"$(INTDIR)\exfFunct.obj" \
	"$(INTDIR)\exfSw.obj" \
	"$(INTDIR)\ftpFileDvr.obj" \
	"$(INTDIR)\functionHandler.obj" \
	"$(INTDIR)\logEval.obj" \
	"$(INTDIR)\ntFileDvr.obj" \
	"$(INTDIR)\sockutil.obj" \
	"$(INTDIR)\srbC_httpc_library.obj" \
	"$(INTDIR)\srbObjFunct.obj" \
	"$(INTDIR)\srbProxyFunct.obj" \
	"$(INTDIR)\srbServer.obj" \
	"$(INTDIR)\srbServerMisc.obj" \
	"$(INTDIR)\srbServerSfo.obj" \
	"$(INTDIR)\tableAccessMisc.obj"

"$(OUTDIR)\srbServer.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("srbServer.dep")
!INCLUDE "srbServer.dep"
!ELSE 
!MESSAGE Warning: cannot find "srbServer.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "srbServer - Win32 Release" || "$(CFG)" == "srbServer - Win32 Debug"
SOURCE=..\..\src\lib\auth.c

!IF  "$(CFG)" == "srbServer - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\auth.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "srbServer - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\auth.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\src\back\compoundObj.c

!IF  "$(CFG)" == "srbServer - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\compoundObj.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "srbServer - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\compoundObj.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\src\back\dbLobj.c

!IF  "$(CFG)" == "srbServer - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\dbLobj.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "srbServer - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\dbLobj.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\src\back\dbTable.c

!IF  "$(CFG)" == "srbServer - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\dbTable.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "srbServer - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\dbTable.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\src\back\exfFunct.c

!IF  "$(CFG)" == "srbServer - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\exfFunct.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "srbServer - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\exfFunct.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\src\back\exfSw.c

!IF  "$(CFG)" == "srbServer - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\exfSw.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "srbServer - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\exfSw.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\src\back\ftpFileDvr.c

!IF  "$(CFG)" == "srbServer - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ftpFileDvr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "srbServer - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\ftpFileDvr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\src\back\functionHandler.c

!IF  "$(CFG)" == "srbServer - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\functionHandler.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "srbServer - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\functionHandler.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\src\lib\logEval.c

!IF  "$(CFG)" == "srbServer - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\logEval.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "srbServer - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\logEval.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\src\back\ntFileDvr.c

!IF  "$(CFG)" == "srbServer - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ntFileDvr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "srbServer - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\ntFileDvr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\src\lib\sockutil.c

!IF  "$(CFG)" == "srbServer - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\sockutil.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "srbServer - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\sockutil.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\src\back\srbC_httpc_library.c

!IF  "$(CFG)" == "srbServer - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\srbC_httpc_library.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "srbServer - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\srbC_httpc_library.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\src\back\srbObjFunct.c

!IF  "$(CFG)" == "srbServer - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\srbObjFunct.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "srbServer - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\srbObjFunct.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\src\back\srbProxyFunct.c

!IF  "$(CFG)" == "srbServer - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\srbProxyFunct.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "srbServer - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\srbProxyFunct.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\src\back\srbServer.c

!IF  "$(CFG)" == "srbServer - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\srbServer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "srbServer - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\srbServer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\src\back\srbServerMisc.c

!IF  "$(CFG)" == "srbServer - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\srbServerMisc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "srbServer - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\srbServerMisc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\src\back\srbServerSfo.c

!IF  "$(CFG)" == "srbServer - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\srbServerSfo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "srbServer - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\srbServerSfo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\src\lib\tableAccessMisc.c

!IF  "$(CFG)" == "srbServer - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\tableAccessMisc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "srbServer - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\tableAccessMisc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

