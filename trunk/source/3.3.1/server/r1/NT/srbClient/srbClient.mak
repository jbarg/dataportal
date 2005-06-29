# Microsoft Developer Studio Generated NMAKE File, Based on srbClient.dsp
!IF "$(CFG)" == ""
CFG=srbClient - Win32 Debug
!MESSAGE No configuration specified. Defaulting to srbClient - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "srbClient - Win32 Release" && "$(CFG)" != "srbClient - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "srbClient.mak" CFG="srbClient - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "srbClient - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "srbClient - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "srbClient - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\srbClient.lib"


CLEAN :
	-@erase "$(INTDIR)\clAuth.obj"
	-@erase "$(INTDIR)\clConnect.obj"
    -@erase "$(INTDIR)\clChksum.obj"
	-@erase "$(INTDIR)\clExec.obj"
	-@erase "$(INTDIR)\clGlobal.obj"
	-@erase "$(INTDIR)\clMisc.obj"
	-@erase "$(INTDIR)\clSfo.obj"
	-@erase "$(INTDIR)\clStub.obj"
	-@erase "$(INTDIR)\crypt1.obj"
	-@erase "$(INTDIR)\mcatgeneral.obj"
	-@erase "$(INTDIR)\packMsg.obj"
	-@erase "$(INTDIR)\srb_perror.obj"
	-@erase "$(INTDIR)\srbSecureComm.obj"
	-@erase "$(INTDIR)\obf.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\srbClient.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "FED_MCAT" /U "..\..\src\include" /U "..\..\src\catalog\include" /U "..\..\src\nt_util" /U "..\..\utilities\include" /Fp"$(INTDIR)\srbClient.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\srbClient.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\srbClient.lib" 
LIB32_OBJS= \
	"$(INTDIR)\clAuth.obj" \
	"$(INTDIR)\clConnect.obj" \
    "$(INTDIR)\clChksum.obj" \
	"$(INTDIR)\clExec.obj" \
	"$(INTDIR)\clGlobal.obj" \
	"$(INTDIR)\clMisc.obj" \
	"$(INTDIR)\clSfo.obj" \
	"$(INTDIR)\clStub.obj" \
	"$(INTDIR)\crypt1.obj" \
	"$(INTDIR)\mcatgeneral.obj" \
	"$(INTDIR)\packMsg.obj" \
	"$(INTDIR)\srb_perror.obj" \
	"$(INTDIR)\srbSecureComm.obj" \
	"$(INTDIR)\obf.obj" 

"$(OUTDIR)\srbClient.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "srbClient - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\srbClient.lib"


CLEAN :
	-@erase "$(INTDIR)\clAuth.obj"
	-@erase "$(INTDIR)\clConnect.obj"
    -@erase "$(INTDIR)\clChksum.obj"
	-@erase "$(INTDIR)\clExec.obj"
	-@erase "$(INTDIR)\clGlobal.obj"
	-@erase "$(INTDIR)\clMisc.obj"
	-@erase "$(INTDIR)\clSfo.obj"
	-@erase "$(INTDIR)\clStub.obj"
	-@erase "$(INTDIR)\crypt1.obj"
	-@erase "$(INTDIR)\mcatgeneral.obj"
	-@erase "$(INTDIR)\packMsg.obj"
	-@erase "$(INTDIR)\srb_perror.obj"
	-@erase "$(INTDIR)\srbSecureComm.obj"
	-@erase "$(INTDIR)\obf.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\srbClient.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "FED_MCAT" /U "..\..\src\include" /U "..\..\src\catalog\include" /U "..\..\src\nt_util" /U "..\..\utilities\include" /Fp"$(INTDIR)\srbClient.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\srbClient.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\srbClient.lib" 
LIB32_OBJS= \
	"$(INTDIR)\clAuth.obj" \
	"$(INTDIR)\clConnect.obj" \
    "$(INTDIR)\clChksum.obj" \
	"$(INTDIR)\clExec.obj" \
	"$(INTDIR)\clGlobal.obj" \
	"$(INTDIR)\clMisc.obj" \
	"$(INTDIR)\clSfo.obj" \
	"$(INTDIR)\clStub.obj" \
	"$(INTDIR)\crypt1.obj" \
	"$(INTDIR)\mcatgeneral.obj" \
	"$(INTDIR)\packMsg.obj" \
	"$(INTDIR)\srb_perror.obj" \
	"$(INTDIR)\srbSecureComm.obj" \
	"$(INTDIR)\obf.obj"

"$(OUTDIR)\srbClient.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("srbClient.dep")
!INCLUDE "srbClient.dep"
!ELSE 
!MESSAGE Warning: cannot find "srbClient.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "srbClient - Win32 Release" || "$(CFG)" == "srbClient - Win32 Debug"
SOURCE=..\..\src\client\clAuth.c

!IF  "$(CFG)" == "srbClient - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "FED_MCAT" /Fp"$(INTDIR)\srbClient.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\clAuth.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "srbClient - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "FED_MCAT" /Fp"$(INTDIR)\srbClient.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\clAuth.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\src\client\clConnect.c

!IF  "$(CFG)" == "srbClient - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "FED_MCAT" /Fp"$(INTDIR)\srbClient.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\clConnect.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "srbClient - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "FED_MCAT" /Fp"$(INTDIR)\srbClient.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\clConnect.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


SOURCE=..\..\src\client\clChksum.c

!IF  "$(CFG)" == "srbClient - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "FED_MCAT" /Fp"$(INTDIR)\srbClient.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\clChksum.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "srbClient - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "FED_MCAT" /Fp"$(INTDIR)\srbClient.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\clChksum.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 




SOURCE=..\..\src\client\clExec.c

!IF  "$(CFG)" == "srbClient - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "FED_MCAT" /Fp"$(INTDIR)\srbClient.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\clExec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "srbClient - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "FED_MCAT" /Fp"$(INTDIR)\srbClient.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\clExec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\src\client\clGlobal.c

!IF  "$(CFG)" == "srbClient - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "FED_MCAT" /Fp"$(INTDIR)\srbClient.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\clGlobal.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "srbClient - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "FED_MCAT" /Fp"$(INTDIR)\srbClient.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\clGlobal.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\src\client\clMisc.c

!IF  "$(CFG)" == "srbClient - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "FED_MCAT" /Fp"$(INTDIR)\srbClient.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\clMisc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "srbClient - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "FED_MCAT" /Fp"$(INTDIR)\srbClient.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\clMisc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\src\client\clSfo.c

!IF  "$(CFG)" == "srbClient - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "FED_MCAT" /Fp"$(INTDIR)\srbClient.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\clSfo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "srbClient - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "FED_MCAT" /Fp"$(INTDIR)\srbClient.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\clSfo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\src\client\clStub.c

!IF  "$(CFG)" == "srbClient - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "FED_MCAT" /Fp"$(INTDIR)\srbClient.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\clStub.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "srbClient - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "FED_MCAT" /Fp"$(INTDIR)\srbClient.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\clStub.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\src\lib\crypt1.c

!IF  "$(CFG)" == "srbClient - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "FED_MCAT" /Fp"$(INTDIR)\srbClient.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\crypt1.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "srbClient - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "FED_MCAT" /Fp"$(INTDIR)\srbClient.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\crypt1.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\src\lib\mcatgeneral.c

!IF  "$(CFG)" == "srbClient - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "FED_MCAT" /Fp"$(INTDIR)\srbClient.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\mcatgeneral.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "srbClient - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "FED_MCAT" /Fp"$(INTDIR)\srbClient.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\mcatgeneral.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\src\client\packMsg.c

!IF  "$(CFG)" == "srbClient - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "FED_MCAT" /Fp"$(INTDIR)\srbClient.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\packMsg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "srbClient - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "FED_MCAT" /Fp"$(INTDIR)\srbClient.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\packMsg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\src\client\srb_perror.c

!IF  "$(CFG)" == "srbClient - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "FED_MCAT" /Fp"$(INTDIR)\srbClient.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\srb_perror.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "srbClient - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "FED_MCAT" /Fp"$(INTDIR)\srbClient.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\srb_perror.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\src\lib\srbSecureComm.c

!IF  "$(CFG)" == "srbClient - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "FED_MCAT" /Fp"$(INTDIR)\srbClient.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\srbSecureComm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "srbClient - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "FED_MCAT" /Fp"$(INTDIR)\srbClient.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\srbSecureComm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


SOURCE=..\..\src\lib\obf.c

!IF  "$(CFG)" == "srbClient - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "FED_MCAT" /Fp"$(INTDIR)\srbClient.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\obf.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<

!ELSEIF  "$(CFG)" == "srbClient - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "FED_MCAT" /Fp"$(INTDIR)\srbClient.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\obf.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF


!ENDIF 
