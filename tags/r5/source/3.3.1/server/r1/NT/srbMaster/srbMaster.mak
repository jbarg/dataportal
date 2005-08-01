# Microsoft Developer Studio Generated NMAKE File, Based on srbMaster.dsp
!IF "$(CFG)" == ""
CFG=srbMaster - Win32 Debug
!MESSAGE No configuration specified. Defaulting to srbMaster - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "srbMaster - Win32 Release" && "$(CFG)" != "srbMaster - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "srbMaster.mak" CFG="srbMaster - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "srbMaster - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "srbMaster - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "srbMaster - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\srbMaster.exe"


CLEAN :
	-@erase "$(INTDIR)\srbMaster.obj"
	-@erase "$(INTDIR)\SrbNtMain.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\srbMaster.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /Fp"$(INTDIR)\srbMaster.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\srbMaster.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib srbSvrClt.lib srbNtUtil.lib ws2_32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\srbMaster.pdb" /machine:I386 /out:"$(OUTDIR)\srbMaster.exe" /libpath:"..\srbSvrClt\Release" /libpath:"..\srbNtUtil\Release" 
LINK32_OBJS= \
	"$(INTDIR)\srbMaster.obj" \
	"$(INTDIR)\SrbNtMain.obj"

"$(OUTDIR)\srbMaster.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "srbMaster - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\srbMaster.exe"


CLEAN :
	-@erase "$(INTDIR)\srbMaster.obj"
	-@erase "$(INTDIR)\SrbNtMain.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\srbMaster.exe"
	-@erase "$(OUTDIR)\srbMaster.ilk"
	-@erase "$(OUTDIR)\srbMaster.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /Fp"$(INTDIR)\srbMaster.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\srbMaster.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib srbSvrClt.lib srbNtUtil.lib ws2_32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\srbMaster.pdb" /debug /machine:I386 /out:"$(OUTDIR)\srbMaster.exe" /pdbtype:sept /libpath:"..\srbSvrClt\Debug" /libpath:"..\srbNtUtil\Debug" 
LINK32_OBJS= \
	"$(INTDIR)\srbMaster.obj" \
	"$(INTDIR)\SrbNtMain.obj"

"$(OUTDIR)\srbMaster.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("srbMaster.dep")
!INCLUDE "srbMaster.dep"
!ELSE 
!MESSAGE Warning: cannot find "srbMaster.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "srbMaster - Win32 Release" || "$(CFG)" == "srbMaster - Win32 Debug"
SOURCE=..\..\src\front\srbMaster.c

!IF  "$(CFG)" == "srbMaster - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbMaster.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\srbMaster.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "srbMaster - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbMaster.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\srbMaster.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\src\front\SrbNtMain.c

!IF  "$(CFG)" == "srbMaster - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbMaster.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\SrbNtMain.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "srbMaster - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FED_MCAT" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /Fp"$(INTDIR)\srbMaster.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\SrbNtMain.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

