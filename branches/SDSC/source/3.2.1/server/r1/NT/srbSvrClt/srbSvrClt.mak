# Microsoft Developer Studio Generated NMAKE File, Based on srbSvrClt.dsp
!IF "$(CFG)" == ""
CFG=srbSvrClt - Win32 Debug
!MESSAGE No configuration specified. Defaulting to srbSvrClt - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "srbSvrClt - Win32 Release" && "$(CFG)" != "srbSvrClt - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "srbSvrClt - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\srbSvrClt.lib"


CLEAN :
	-@erase "$(INTDIR)\clAuth.obj"
	-@erase "$(INTDIR)\clConnect.obj"
	-@erase "$(INTDIR)\clExec.obj"
	-@erase "$(INTDIR)\clGlobal.obj"
	-@erase "$(INTDIR)\clMisc.obj"
	-@erase "$(INTDIR)\clSfo.obj"
	-@erase "$(INTDIR)\clStub.obj"
	-@erase "$(INTDIR)\comm.obj"
	-@erase "$(INTDIR)\crypt1.obj"
	-@erase "$(INTDIR)\distrib.obj"
	-@erase "$(INTDIR)\elog.obj"
	-@erase "$(INTDIR)\mcatgeneral.obj"
	-@erase "$(INTDIR)\misc.obj"
	-@erase "$(INTDIR)\packMsg.obj"
	-@erase "$(INTDIR)\srb_perror.obj"
	-@erase "$(INTDIR)\srbSecureComm.obj"
	-@erase "$(INTDIR)\strct_lib.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\srbSvrClt.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /D "FED_MCAT" /Fp"$(INTDIR)\srbSvrClt.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\srbSvrClt.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\srbSvrClt.lib" 
LIB32_OBJS= \
	"$(INTDIR)\clAuth.obj" \
	"$(INTDIR)\clConnect.obj" \
	"$(INTDIR)\clExec.obj" \
	"$(INTDIR)\clGlobal.obj" \
	"$(INTDIR)\clMisc.obj" \
	"$(INTDIR)\clSfo.obj" \
	"$(INTDIR)\clStub.obj" \
	"$(INTDIR)\comm.obj" \
	"$(INTDIR)\crypt1.obj" \
	"$(INTDIR)\distrib.obj" \
	"$(INTDIR)\elog.obj" \
	"$(INTDIR)\mcatgeneral.obj" \
	"$(INTDIR)\misc.obj" \
	"$(INTDIR)\packMsg.obj" \
	"$(INTDIR)\srb_perror.obj" \
	"$(INTDIR)\srbSecureComm.obj" \
	"$(INTDIR)\strct_lib.obj"

"$(OUTDIR)\srbSvrClt.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "srbSvrClt - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\srbSvrClt.lib"


CLEAN :
	-@erase "$(INTDIR)\clAuth.obj"
	-@erase "$(INTDIR)\clConnect.obj"
	-@erase "$(INTDIR)\clExec.obj"
	-@erase "$(INTDIR)\clGlobal.obj"
	-@erase "$(INTDIR)\clMisc.obj"
	-@erase "$(INTDIR)\clSfo.obj"
	-@erase "$(INTDIR)\clStub.obj"
	-@erase "$(INTDIR)\comm.obj"
	-@erase "$(INTDIR)\crypt1.obj"
	-@erase "$(INTDIR)\distrib.obj"
	-@erase "$(INTDIR)\elog.obj"
	-@erase "$(INTDIR)\mcatgeneral.obj"
	-@erase "$(INTDIR)\misc.obj"
	-@erase "$(INTDIR)\packMsg.obj"
	-@erase "$(INTDIR)\srb_perror.obj"
	-@erase "$(INTDIR)\srbSecureComm.obj"
	-@erase "$(INTDIR)\strct_lib.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\srbSvrClt.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "_SRB_WIN_SERVICE" /D "SRB_WIN_SERVICE" /D "SRB_SVR_ENV" /D "FED_MCAT" /Fp"$(INTDIR)\srbSvrClt.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\srbSvrClt.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\srbSvrClt.lib" 
LIB32_OBJS= \
	"$(INTDIR)\clAuth.obj" \
	"$(INTDIR)\clConnect.obj" \
	"$(INTDIR)\clExec.obj" \
	"$(INTDIR)\clGlobal.obj" \
	"$(INTDIR)\clMisc.obj" \
	"$(INTDIR)\clSfo.obj" \
	"$(INTDIR)\clStub.obj" \
	"$(INTDIR)\comm.obj" \
	"$(INTDIR)\crypt1.obj" \
	"$(INTDIR)\distrib.obj" \
	"$(INTDIR)\elog.obj" \
	"$(INTDIR)\mcatgeneral.obj" \
	"$(INTDIR)\misc.obj" \
	"$(INTDIR)\packMsg.obj" \
	"$(INTDIR)\srb_perror.obj" \
	"$(INTDIR)\srbSecureComm.obj" \
	"$(INTDIR)\strct_lib.obj"

"$(OUTDIR)\srbSvrClt.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("srbSvrClt.dep")
!INCLUDE "srbSvrClt.dep"
!ELSE 
!MESSAGE Warning: cannot find "srbSvrClt.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "srbSvrClt - Win32 Release" || "$(CFG)" == "srbSvrClt - Win32 Debug"
SOURCE=..\..\src\client\clAuth.c

"$(INTDIR)\clAuth.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\client\clConnect.c

"$(INTDIR)\clConnect.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\client\clExec.c

"$(INTDIR)\clExec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\client\clGlobal.c

"$(INTDIR)\clGlobal.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\client\clMisc.c

"$(INTDIR)\clMisc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\client\clSfo.c

"$(INTDIR)\clSfo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\client\clStub.c

"$(INTDIR)\clStub.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\lib\comm.c

"$(INTDIR)\comm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\lib\crypt1.c

"$(INTDIR)\crypt1.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\back\distrib.c

"$(INTDIR)\distrib.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\lib\elog.c

"$(INTDIR)\elog.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\lib\mcatgeneral.c

"$(INTDIR)\mcatgeneral.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\lib\misc.c

"$(INTDIR)\misc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\client\packMsg.c

"$(INTDIR)\packMsg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\client\srb_perror.c

"$(INTDIR)\srb_perror.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\lib\srbSecureComm.c

"$(INTDIR)\srbSecureComm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\catalog\mdas\strct_lib.c

"$(INTDIR)\strct_lib.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

