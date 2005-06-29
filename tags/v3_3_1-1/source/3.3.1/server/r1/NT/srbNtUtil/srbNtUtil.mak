# Microsoft Developer Studio Generated NMAKE File, Based on srbNtUtil.dsp
!IF "$(CFG)" == ""
CFG=srbNtUtil - Win32 Debug
!MESSAGE No configuration specified. Defaulting to srbNtUtil - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "srbNtUtil - Win32 Release" && "$(CFG)" != "srbNtUtil - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "srbNtUtil.mak" CFG="srbNtUtil - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "srbNtUtil - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "srbNtUtil - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "srbNtUtil - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\srbNtUtil.lib"


CLEAN :
	-@erase "$(INTDIR)\dirent.obj"
	-@erase "$(INTDIR)\getopt.obj"
	-@erase "$(INTDIR)\misc_util.obj"
	-@erase "$(INTDIR)\nt_extra.obj"
	-@erase "$(INTDIR)\regex.obj"
	-@erase "$(INTDIR)\SrbNtUtil.obj"
	-@erase "$(INTDIR)\startsock.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\srbNtUtil.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "FED_MCAT" /Fp"$(INTDIR)\srbNtUtil.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\srbNtUtil.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\srbNtUtil.lib" 
LIB32_OBJS= \
	"$(INTDIR)\dirent.obj" \
	"$(INTDIR)\getopt.obj" \
	"$(INTDIR)\misc_util.obj" \
	"$(INTDIR)\nt_extra.obj" \
	"$(INTDIR)\regex.obj" \
	"$(INTDIR)\SrbNtUtil.obj" \
	"$(INTDIR)\startsock.obj"

"$(OUTDIR)\srbNtUtil.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "srbNtUtil - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\srbNtUtil.lib"


CLEAN :
	-@erase "$(INTDIR)\dirent.obj"
	-@erase "$(INTDIR)\getopt.obj"
	-@erase "$(INTDIR)\misc_util.obj"
	-@erase "$(INTDIR)\nt_extra.obj"
	-@erase "$(INTDIR)\regex.obj"
	-@erase "$(INTDIR)\SrbNtUtil.obj"
	-@erase "$(INTDIR)\startsock.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\srbNtUtil.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "FED_MCAT" /Fp"$(INTDIR)\srbNtUtil.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\srbNtUtil.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\srbNtUtil.lib" 
LIB32_OBJS= \
	"$(INTDIR)\dirent.obj" \
	"$(INTDIR)\getopt.obj" \
	"$(INTDIR)\misc_util.obj" \
	"$(INTDIR)\nt_extra.obj" \
	"$(INTDIR)\regex.obj" \
	"$(INTDIR)\SrbNtUtil.obj" \
	"$(INTDIR)\startsock.obj"

"$(OUTDIR)\srbNtUtil.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("srbNtUtil.dep")
!INCLUDE "srbNtUtil.dep"
!ELSE 
!MESSAGE Warning: cannot find "srbNtUtil.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "srbNtUtil - Win32 Release" || "$(CFG)" == "srbNtUtil - Win32 Debug"
SOURCE=..\..\src\nt_util\dirent.c

"$(INTDIR)\dirent.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\nt_util\getopt.c

"$(INTDIR)\getopt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\nt_util\misc_util.c

"$(INTDIR)\misc_util.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\nt_util\nt_extra.c

"$(INTDIR)\nt_extra.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\nt_util\regex.c

!IF  "$(CFG)" == "srbNtUtil - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "FED_MCAT" /D "REGEX_MALLOC" /D "STDC_HEADERS" /Fp"$(INTDIR)\srbNtUtil.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\regex.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "srbNtUtil - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /I "..\..\utilities\include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" /D "_LARGEFILE_SOURCE" /D _FILE_OFFSET_BITS=64 /D "FED_MCAT" /D "REGEX_MALLOC" /D "STDC_HEADERS" /Fp"$(INTDIR)\srbNtUtil.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\regex.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\src\nt_util\SrbNtUtil.c

"$(INTDIR)\SrbNtUtil.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\nt_util\startsock.c

"$(INTDIR)\startsock.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

