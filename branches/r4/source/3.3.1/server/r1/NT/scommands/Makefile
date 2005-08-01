# Makefile - This is the  makefile for the SRB client src

# The choices for 'BUILD_OPTION' is Debug or Release.
#BUILD_OPTION = Release
BUILD_OPTION = Debug


#buildDir=..\..
srcDir=..\..\utilities\src
#binDir=.\bin
#objDir=.\obj
otherObjs= $(objDir)\srbClientUtil.obj
BLOAD_OBJS = $(objDir)\bunloadLib.obj $(objDir)\bloadLib.obj


TARGETS = \
	$(binDir)\SgetR.exe $(binDir)\SgetU.exe $(binDir)\SgetD.exe	\
	$(binDir)\Scat.exe $(binDir)\Sget.exe $(binDir)\Sappend.exe  	\
	$(binDir)\Sput.exe $(binDir)\Sreplicate.exe  $(binDir)\Smv.exe 	\
	$(binDir)\Scd.exe $(binDir)\Spwd.exe $(binDir)\Sls.exe $(binDir)\Sphymove.exe \
	$(binDir)\Scp.exe  $(binDir)\Smkdir.exe $(binDir)\Srm.exe $(binDir)\Sinit.exe \
	$(binDir)\Sexit.exe $(binDir)\Schmod.exe $(binDir)\Srmdir.exe 	\
    $(binDir)\SmodD.exe $(binDir)\Stoken.exe  \
	$(binDir)\Senv.exe  $(binDir)\Sticket.exe $(binDir)\Srmticket.exe 	\
	$(binDir)\Stcat.exe $(binDir)\Stls.exe $(binDir)\SgetT.exe 		\
	$(binDir)\Sregister.exe $(binDir)\Spasswd.exe $(binDir)\Serror.exe 	\
	$(binDir)\Srmcont.exe $(binDir)\Smkcont.exe $(binDir)\Ssyncont.exe	\
	$(binDir)\Slscont.exe $(binDir)\Spcommand.exe $(binDir)\Sreplcont.exe   \
    $(binDir)\Sattrs.exe $(binDir)\Sannotate.exe  $(binDir)\Smeta.exe  $(binDir)\Scommands.exe  \
    $(binDir)\Sufmeta.exe $(binDir)\Spullmeta.exe $(binDir)\Spushmeta.exe \
    $(binDir)\Sbkupsrb.exe $(binDir)\Schdefres.exe $(binDir)\Schhost.exe \
    $(binDir)\Schksum.exe $(binDir)\SgetColl.exe $(binDir)\SmodColl.exe \
    $(binDir)\Srmtrash.exe $(binDir)\Srsync.exe $(binDir)\Ssfstage.exe \
    $(binDir)\Ssyncd.exe $(binDir)\Szone.exe \
    $(binDir)\Singestuser.exe $(binDir)\SmodifyUser.exe $(binDir)\SmodR.exe \
    $(binDir)\Sregisterlocation.exe $(binDir)\Sregisterresource.exe \
    $(binDir)\Sbunload.exe $(binDir)\Sbload.exe $(binDir)\Sln.exe \
    $(binDir)\Sauth.exe $(binDir)\Sbregister.exe \
    $(binDir)\SmodE.exe $(binDir)\Squery.exe $(binDir)\Sstage.exe


CC=cl.exe
LINK32=link.exe

DEBUG_CFLAGS=/nologo /MLd /W3 /Gm /GX /Zi /ZI /I "..\..\utilities\include" /I "..\..\src\include" /I "..\..\src\catalog\include" /I "..\..\src\nt_util" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "FED_MCAT" /YX /FD /GZ /c 

NON_DEBUG_CFLAGS =  /nologo /ML /W3 /GX /O2 /I "..\..\utilities\include" /I "..\..\src\include" \
        /I "..\..\src\catalog\include" /I "..\..\src\nt_util" \
         /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "MDAS_AUTH" /D FTP=3 /D HTTP=4 /D "_WIN32" /D "NTFS" \
        /D "_LARGEFILE_SOURCE" /D "_FILE_OFFSET_BITS=64" /D "FED_MCAT" /YX /FD /c

!IF "$(BUILD_OPTION)" == "Debug"
CFLAGS = $(DEBUG_CFLAGS)
LINK_FLAG = /nologo /subsystem:console /debug /incremental:yes /machine:I386 /nodefaultlib:"libc.lib"
binDir=.\Debug
objDir=.\Debug
!ELSE
CFLAGS = $(NON_DEBUG_CFLAGS)
LINK_FLAG = /nologo /subsystem:console /incremental:no /machine:I386
binDir=.\Release
objDir=.\Release
!ENDIF

SRB_LIBS=..\SrbClient\$(BUILD_OPTION)\SrbClient.lib ..\srbNtUtil\$(BUILD_OPTION)\srbNtUtil.lib
MSDEV_LIBS=ws2_32.lib advapi32.lib  winmm.lib
LIBS =  $(otherObjs) $(SRB_LIBS) $(MSDEV_LIBS) $(LINK_FLAG)

ALL: MK_OBJ_DIR $(otherObjs) $(BLOAD_OBJS) $(TARGETS)


$(objDir)\srbClientUtil.obj: $(srcDir)\srbClientUtil.c
    $(CC) $(CFLAGS) /Fo$@ $(srcDir)\srbClientUtil.c

$(objDir)\srbClientUtil2.obj: $(srcDir)\srbClientUtil2.c
    $(CC) $(CFLAGS) /Fo$@ $(srcDir)\srbClientUtil2.c


$(objDir)\bunloadLib.obj: $(srcDir)\bunloadLib.c
    $(CC) $(CFLAGS) /Fo$@ $(srcDir)\bunloadLib.c

MK_OBJ_DIR:
    if not exist "$(objDir)/$(NULL)" mkdir "$(objDir)"
    if not exist "$(binDir)/$(NULL)" mkdir "$(binDir)"

$(objDir)\bloadLib.obj: $(srcDir)\bloadLib.c
    $(CC) $(CFLAGS) /Fo$@ $(srcDir)\bloadLib.c

$(objDir)\Scat.obj:  $(srcDir)\Scat.c 
	 $(CC) $(CFLAGS) /Fo$@ $(srcDir)\Scat.c

$(binDir)\Scat.exe: $(objDir)\Scat.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Scat.obj $(LIBS)

$(objDir)\Sget.obj:  $(srcDir)\Sget.c
	 $(CC) $(CFLAGS) /Fo$@ $(srcDir)\Sget.c

$(binDir)\Sget.exe: $(objDir)\Sget.obj  $(otherObjs) $(objDir)\bunloadLib.obj $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Sget.obj $(objDir)\bunloadLib.obj $(LIBS)

$(objDir)\Scd.obj:  $(srcDir)\Scd.c
	 $(CC) $(CFLAGS) /Fo$@ $(srcDir)\Scd.c 

$(binDir)\Scd.exe: $(objDir)\Scd.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Scd.obj $(LIBS)

$(objDir)\Spwd.obj:  $(srcDir)\Spwd.c
	 $(CC) $(CFLAGS) /Fo$@ $(srcDir)\Spwd.c 

$(binDir)\Spwd.exe: $(objDir)\Spwd.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Spwd.obj $(LIBS)

$(objDir)\Sput.obj:  $(srcDir)\Sput.c
	 $(CC) $(CFLAGS) /Fo$@ $(srcDir)\Sput.c 

$(binDir)\Sput.exe: $(objDir)\Sput.obj  $(otherObjs) $(BLOAD_OBJS) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Sput.obj $(BLOAD_OBJS) $(LIBS)

$(objDir)\Sreplicate.obj:  $(srcDir)\Sreplicate.c
	 $(CC) $(CFLAGS) /Fo$@ $(srcDir)\Sreplicate.c 

$(binDir)\Sreplicate.exe: $(objDir)\Sreplicate.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Sreplicate.obj $(LIBS)

$(objDir)\Smv.obj:  $(srcDir)\Smv.c
	 $(CC) $(CFLAGS) /Fo$@ $(srcDir)\Smv.c 

$(binDir)\Smv.exe: $(objDir)\Smv.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Smv.obj $(LIBS)

$(objDir)\Sappend.obj:  $(srcDir)\Sappend.c
	 $(CC) $(CFLAGS) /Fo$@ $(srcDir)\Sappend.c 

$(binDir)\Sappend.exe: $(objDir)\Sappend.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Sappend.obj $(LIBS)

$(objDir)\SgetR.obj:  $(srcDir)\SgetR.c
	$(CC) $(CFLAGS) /Fo$@ $(srcDir)\SgetR.c $(LIBS)

$(binDir)\SgetR.exe: $(objDir)\SgetR.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\SgetR.obj $(LIBS)

$(objDir)\SgetU.obj:  $(srcDir)\SgetU.c
	$(CC) $(CFLAGS) /Fo$@ $(srcDir)\SgetU.c 

$(binDir)\SgetU.exe: $(objDir)\SgetU.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\SgetU.obj  $(LIBS)

$(objDir)\SgetD.obj:  $(srcDir)\SgetD.c
	$(CC) $(CFLAGS) /Fo$@ $(srcDir)\SgetD.c 

$(binDir)\SgetD.exe: $(objDir)\SgetD.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\SgetD.obj $(LIBS)

$(objDir)\Sls.obj:  $(srcDir)\Sls.c
	$(CC) $(CFLAGS) /Fo$@ $(srcDir)\Sls.c 

$(binDir)\Sls.exe: $(objDir)\Sls.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Sls.obj   $(LIBS)

$(objDir)\Sphymove.obj:  $(srcDir)\Sphymove.c
	$(CC) $(CFLAGS) /Fo$@ $(srcDir)\Sphymove.c 

$(binDir)\Sphymove.exe: $(objDir)\Sphymove.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Sphymove.obj    $(LIBS)

$(objDir)\Scp.obj:  $(srcDir)\Scp.c
	$(CC) $(CFLAGS) /Fo$@ $(srcDir)\Scp.c -I.\include 

$(binDir)\Scp.exe: $(objDir)\Scp.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Scp.obj    $(LIBS)

$(objDir)\Smkdir.obj:  $(srcDir)\Smkdir.c
	$(CC) $(CFLAGS) /Fo$@ $(srcDir)\Smkdir.c 

$(binDir)\Smkdir.exe: $(objDir)\Smkdir.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Smkdir.obj   $(LIBS)

$(objDir)\Srm.obj:  $(srcDir)\Srm.c
	$(CC) $(CFLAGS) /Fo$@ $(srcDir)\Srm.c 

$(binDir)\Srm.exe: $(objDir)\Srm.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Srm.obj   $(LIBS)

$(objDir)\Spasswd.obj:  $(srcDir)\Spasswd.c
	$(CC) $(CFLAGS) /Fo$@ $(srcDir)\Spasswd.c 

$(binDir)\Spasswd.exe: $(objDir)\Spasswd.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Spasswd.obj   $(LIBS)

$(objDir)\Sinit.obj:  $(srcDir)\Sinit.c
	$(CC) $(CFLAGS) /Fo$@ $(srcDir)\Sinit.c 

$(binDir)\Sinit.exe: $(objDir)\Sinit.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Sinit.obj   $(LIBS) user32.lib

$(objDir)\Sexit.obj:  $(srcDir)\Sexit.c
	$(CC) $(CFLAGS) /Fo$@ $(srcDir)\Sexit.c 

$(binDir)\Sexit.exe: $(objDir)\Sexit.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Sexit.obj   $(LIBS)

$(objDir)\Schmod.obj:  $(srcDir)\Schmod.c
	$(CC) $(CFLAGS) /Fo$@ $(srcDir)\Schmod.c 

$(binDir)\Schmod.exe: $(objDir)\Schmod.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Schmod.obj   $(LIBS)

$(objDir)\Srmdir.obj:  $(srcDir)\Srmdir.c
	$(CC) $(CFLAGS) /Fo$@ $(srcDir)\Srmdir.c 

$(binDir)\Srmdir.exe: $(objDir)\Srmdir.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Srmdir.obj   $(LIBS)

$(objDir)\SmodD.obj:  $(srcDir)\SmodD.c
	$(CC) $(CFLAGS) /Fo$@ $(srcDir)\SmodD.c 

$(binDir)\SmodD.exe: $(objDir)\SmodD.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\SmodD.obj   $(LIBS)

$(objDir)\Stoken.obj:  $(srcDir)\Stoken.c
	$(CC) $(CFLAGS) /Fo$@ $(srcDir)\Stoken.c 

$(binDir)\Stoken.exe: $(objDir)\Stoken.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Stoken.obj   $(LIBS)

$(objDir)\Senv.obj:  $(srcDir)\Senv.c
	$(CC) $(CFLAGS) /Fo$@ $(srcDir)\Senv.c 

$(binDir)\Senv.exe: $(objDir)\Senv.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Senv.obj   $(LIBS)

$(objDir)\Scopy.obj:  $(srcDir)\Scopy.c
	$(CC) $(CFLAGS) /Fo$@ $(srcDir)\Scopy.c 

$(binDir)\Scopy.exe: $(objDir)\Scopy.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Scopy.obj   $(LIBS)

$(objDir)\Shelp.obj:  $(srcDir)\Shelp.c
	$(CC) $(CFLAGS) /Fo$@ $(srcDir)\Shelp.c 

$(binDir)\Scommands.exe: $(objDir)\Shelp.obj  
	$(LINK32) /OUT:$@ $(objDir)\Shelp.obj  

$(objDir)\Sticket.obj:  $(srcDir)\Sticket.c
	$(CC) $(CFLAGS) /Fo$@ $(srcDir)\Sticket.c 

$(binDir)\Sticket.exe: $(objDir)\Sticket.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Sticket.obj   $(LIBS)

$(objDir)\Srmticket.obj:  $(srcDir)\Srmticket.c
	$(CC) $(CFLAGS) /Fo$@ $(srcDir)\Srmticket.c 

$(binDir)\Srmticket.exe: $(objDir)\Srmticket.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Srmticket.obj   $(LIBS)

$(objDir)\Stcat.obj:  $(srcDir)\Stcat.c
	$(CC) $(CFLAGS) /Fo$@ $(srcDir)\Stcat.c 

$(binDir)\Stcat.exe: $(objDir)\Stcat.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Stcat.obj   $(LIBS)

$(objDir)\Stls.obj:  $(srcDir)\Stls.c
	$(CC) $(CFLAGS) /Fo$@ $(srcDir)\Stls.c 

$(binDir)\Stls.exe: $(objDir)\Stls.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Stls.obj   $(LIBS)

$(objDir)\SgetT.obj:  $(srcDir)\SgetT.c
	$(CC) $(CFLAGS) /Fo$@ $(srcDir)\SgetT.c 

$(binDir)\SgetT.exe: $(objDir)\SgetT.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\SgetT.obj   $(LIBS)

$(objDir)\Sregister.obj:  $(srcDir)\Sregister.c
	$(CC) $(CFLAGS) /Fo$@ $(srcDir)\Sregister.c 

$(binDir)\Sregister.exe: $(objDir)\Sregister.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Sregister.obj  $(LIBS)

$(objDir)\Serror.obj:  $(srcDir)\Serror.c
	$(CC) $(CFLAGS) /Fo$@ $(srcDir)\Serror.c 

$(binDir)\Serror.exe: $(objDir)\Serror.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Serror.obj   $(LIBS)

$(objDir)\Srmcont.obj:  $(srcDir)\Srmcont.c
	$(CC) $(CFLAGS) /Fo$@ $(srcDir)\Srmcont.c 

$(binDir)\Srmcont.exe: $(objDir)\Srmcont.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Srmcont.obj   $(LIBS)

$(objDir)\Smkcont.obj:  $(srcDir)\Smkcont.c
	 $(CC) $(CFLAGS) /Fo$@ $(srcDir)\Smkcont.c 

$(binDir)\Smkcont.exe: $(objDir)\Smkcont.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Smkcont.obj   $(LIBS)

$(objDir)\Ssyncont.obj:  $(srcDir)\Ssyncont.c
	$(CC) $(CFLAGS) /Fo$@ $(srcDir)\Ssyncont.c

$(binDir)\Ssyncont.exe: $(objDir)\Ssyncont.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Ssyncont.obj   $(LIBS)

$(objDir)\Slscont.obj:  $(srcDir)\Slscont.c
	$(CC) $(CFLAGS) /Fo$@ $(srcDir)\Slscont.c 

$(binDir)\Slscont.exe: $(objDir)\Slscont.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Slscont.obj   $(LIBS)

$(objDir)\Spcommand.obj:  $(srcDir)\Spcommand.c
	$(CC) $(CFLAGS) /Fo$@ $(srcDir)\Spcommand.c 

$(binDir)\Spcommand.exe: $(objDir)\Spcommand.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Spcommand.obj   $(LIBS)

$(objDir)\Sreplcont.obj:  $(srcDir)\Sreplcont.c
	$(CC) $(CFLAGS) /Fo$@ $(srcDir)\Sreplcont.c 

$(binDir)\Sreplcont.exe: $(objDir)\Sreplcont.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Sreplcont.obj   $(LIBS)

$(objDir)\Sattrs.obj:  $(srcDir)\Sattrs.c
	$(CC) $(CFLAGS) /Fo$@ $(srcDir)\Sattrs.c 

$(binDir)\Sattrs.exe: $(objDir)\Sattrs.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Sattrs.obj   $(LIBS)

$(objDir)\Sannotate.obj:  $(srcDir)\Sannotate.c
	$(CC) $(CFLAGS) /Fo$@ $(srcDir)\Sannotate.c 

$(binDir)\Sannotate.exe: $(objDir)\Sannotate.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Sannotate.obj   $(LIBS)

$(objDir)\Smeta.obj:  $(srcDir)\Smeta.c
	$(CC) $(CFLAGS) /Fo$@ $(srcDir)\Smeta.c 

$(binDir)\Smeta.exe: $(objDir)\Smeta.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Smeta.obj   $(LIBS)

$(objDir)\Sport.obj:  $(srcDir)\Sport.c
	$(CC) $(CFLAGS) /Fo$@ $(srcDir)\Sport.c 

$(binDir)\Sport.exe: $(objDir)\Sport.obj  $(otherObjs) $(SRB_LIBS)
	$(LINK32) /OUT:$@ $(objDir)\Sport.obj   $(LIBS)

$(objDir)\Sufmeta.obj:  $(srcDir)\Sufmeta.c
    $(CC) $(CFLAGS) /Fo$@ $(srcDir)\Sufmeta.c

$(binDir)\Sufmeta.exe: $(objDir)\Sufmeta.obj  $(otherObjs) $(SRB_LIBS)
    $(LINK32) /OUT:$@ $(objDir)\Sufmeta.obj   $(LIBS)

$(objDir)\Spullmeta.obj: $(srcDir)\Spullmeta.c
    $(CC) $(CFLAGS) /Fo$@ $(srcDir)\Spullmeta.c

$(binDir)\Spullmeta.exe: $(objDir)\Spullmeta.obj
    $(LINK32) /OUT:$@ $(objDir)\Spullmeta.obj $(LIBS)

$(objDir)\Spushmeta.obj: $(srcDir)\Spushmeta.c
    $(CC) $(CFLAGS) /Fo$@ $(srcDir)\Spushmeta.c

$(binDir)\Spushmeta.exe: $(objDir)\Spushmeta.obj
    $(LINK32) /OUT:$@ $(objDir)\Spushmeta.obj $(LIBS)

$(objDir)\Sbkupsrb.obj: $(srcDir)\Sbkupsrb.c
    $(CC) $(CFLAGS) /Fo$@ $(srcDir)\Sbkupsrb.c

$(binDir)\Sbkupsrb.exe: $(objDir)\Sbkupsrb.obj
    $(LINK32) /OUT:$@ $(objDir)\Sbkupsrb.obj $(LIBS)

$(objDir)\Sbload.obj: $(srcDir)\Sbload.c
    $(CC) $(CFLAGS) /Fo$@ $(srcDir)\Sbload.c

$(binDir)\Sbload.exe: $(objDir)\Sbload.obj
    $(LINK32) /OUT:$@ $(objDir)\Sbload.obj $(BLOAD_OBJS) $(LIBS)

$(objDir)\Sbunload.obj: $(srcDir)\Sbunload.c
    $(CC) $(CFLAGS) /Fo$@ $(srcDir)\Sbunload.c

$(binDir)\Sbunload.exe: $(objDir)\Sbunload.obj
    $(LINK32) /OUT:$@ $(objDir)\Sbunload.obj $(BLOAD_OBJS) $(LIBS)

$(objDir)\Schdefres.obj: $(srcDir)\Schdefres.c
    $(CC) $(CFLAGS) /Fo$@ $(srcDir)\Schdefres.c

$(binDir)\Schdefres.exe: $(objDir)\Schdefres.obj
    $(LINK32) /OUT:$@ $(objDir)\Schdefres.obj $(LIBS)

$(objDir)\Schhost.obj: $(srcDir)\Schhost.c
    $(CC) $(CFLAGS) /Fo$@ $(srcDir)\Schhost.c

$(binDir)\Schhost.exe: $(objDir)\Schhost.obj
    $(LINK32) /OUT:$@ $(objDir)\Schhost.obj $(LIBS)

$(objDir)\Schksum.obj: $(srcDir)\Schksum.c
    $(CC) $(CFLAGS) /Fo$@ $(srcDir)\Schksum.c

$(binDir)\Schksum.exe: $(objDir)\Schksum.obj
    $(LINK32) /OUT:$@ $(objDir)\Schksum.obj $(LIBS)

$(objDir)\SgetColl.obj: $(srcDir)\SgetColl.c
    $(CC) $(CFLAGS) /Fo$@ $(srcDir)\SgetColl.c

$(binDir)\SgetColl.exe: $(objDir)\SgetColl.obj
    $(LINK32) /OUT:$@ $(objDir)\SgetColl.obj $(LIBS)

$(objDir)\Singestuser.obj: $(srcDir)\Singestuser.c
    $(CC) $(CFLAGS) /Fo$@ $(srcDir)\Singestuser.c

$(binDir)\Singestuser.exe: $(objDir)\Singestuser.obj
    $(LINK32) /OUT:$@ $(objDir)\Singestuser.obj $(LIBS)

$(objDir)\SmodColl.obj: $(srcDir)\SmodColl.c
    $(CC) $(CFLAGS) /Fo$@ $(srcDir)\SmodColl.c

$(binDir)\SmodColl.exe: $(objDir)\SmodColl.obj
    $(LINK32) /OUT:$@ $(objDir)\SmodColl.obj $(LIBS)

$(objDir)\SmodifyUser.obj: $(srcDir)\SmodifyUser.c
    $(CC) $(CFLAGS) /Fo$@ $(srcDir)\SmodifyUser.c

$(binDir)\SmodifyUser.exe: $(objDir)\SmodifyUser.obj
    $(LINK32) /OUT:$@ $(objDir)\SmodifyUser.obj $(LIBS)

$(objDir)\SmodR.obj: $(srcDir)\SmodR.c
    $(CC) $(CFLAGS) /Fo$@ $(srcDir)\SmodR.c

$(binDir)\SmodR.exe: $(objDir)\SmodR.obj
    $(LINK32) /OUT:$@ $(objDir)\SmodR.obj $(LIBS)

$(objDir)\Sregisterlocation.obj: $(srcDir)\Sregisterlocation.c
    $(CC) $(CFLAGS) /Fo$@ $(srcDir)\Sregisterlocation.c

$(binDir)\Sregisterlocation.exe: $(objDir)\Sregisterlocation.obj
    $(LINK32) /OUT:$@ $(objDir)\Sregisterlocation.obj $(LIBS)

$(objDir)\Sregisterresource.obj: $(srcDir)\Sregisterresource.c
    $(CC) $(CFLAGS) /Fo$@ $(srcDir)\Sregisterresource.c

$(binDir)\Sregisterresource.exe: $(objDir)\Sregisterresource.obj
    $(LINK32) /OUT:$@ $(objDir)\Sregisterresource.obj $(LIBS)

$(objDir)\Srmtrash.obj: $(srcDir)\Srmtrash.c
    $(CC) $(CFLAGS) /Fo$@ $(srcDir)\Srmtrash.c

$(binDir)\Srmtrash.exe: $(objDir)\Srmtrash.obj
    $(LINK32) /OUT:$@ $(objDir)\Srmtrash.obj $(LIBS)

$(objDir)\Srsync.obj: $(srcDir)\Srsync.c
    $(CC) $(CFLAGS) /Fo$@ $(srcDir)\Srsync.c

$(binDir)\Srsync.exe: $(objDir)\Srsync.obj
    $(LINK32) /OUT:$@ $(objDir)\Srsync.obj $(LIBS)

$(objDir)\Ssfstage.obj: $(srcDir)\Ssfstage.c
    $(CC) $(CFLAGS) /Fo$@ $(srcDir)\Ssfstage.c

$(binDir)\Ssfstage.exe: $(objDir)\Ssfstage.obj
    $(LINK32) /OUT:$@ $(objDir)\Ssfstage.obj $(LIBS)

$(objDir)\Ssyncd.obj: $(srcDir)\Ssyncd.c
    $(CC) $(CFLAGS) /Fo$@ $(srcDir)\Ssyncd.c

$(binDir)\Ssyncd.exe: $(objDir)\Ssyncd.obj
    $(LINK32) /OUT:$@ $(objDir)\Ssyncd.obj $(LIBS)

$(objDir)\Szone.obj: $(srcDir)\Szone.c
    $(CC) $(CFLAGS) /Fo$@ $(srcDir)\Szone.c

$(binDir)\Szone.exe: $(objDir)\Szone.obj
    $(LINK32) /OUT:$@ $(objDir)\Szone.obj $(LIBS)

$(objDir)\Sln.obj: $(srcDir)\Sln.c
    $(CC) $(CFLAGS) /Fo$@ $(srcDir)\Sln.c

$(binDir)\Sln.exe: $(objDir)\Sln.obj
    $(LINK32) /OUT:$@ $(objDir)\Sln.obj $(LIBS)

$(objDir)\Sauth.obj: $(srcDir)\Sauth.c
    $(CC) $(CFLAGS) /Fo$@ $(srcDir)\Sauth.c

$(binDir)\Sauth.exe: $(objDir)\Sauth.obj
    $(LINK32) /OUT:$@ $(objDir)\Sauth.obj $(LIBS)

$(objDir)\Sbregister.obj: $(srcDir)\Sbregister.c
    $(CC) $(CFLAGS) /Fo$@ $(srcDir)\Sbregister.c

$(binDir)\Sbregister.exe: $(objDir)\Sbregister.obj
    $(LINK32) /OUT:$@ $(objDir)\Sbregister.obj $(LIBS)

$(objDir)\Sconvert.obj: $(srcDir)\Sconvert.c
    $(CC) $(CFLAGS) /Fo$@ $(srcDir)\Sconvert.c

$(binDir)\Sconvert.exe: $(objDir)\Sconvert.obj
    $(LINK32) /OUT:$@ $(objDir)\Sconvert.obj $(LIBS)

$(objDir)\SmodE.obj: $(srcDir)\SmodE.c
    $(CC) $(CFLAGS) /Fo$@ $(srcDir)\SmodE.c

$(binDir)\SmodE.exe: $(objDir)\SmodE.obj
    $(LINK32) /OUT:$@ $(objDir)\SmodE.obj $(LIBS)

$(objDir)\Squery.obj: $(srcDir)\Squery.c
    $(CC) $(CFLAGS) /Fo$@ $(srcDir)\Squery.c

$(binDir)\Squery.exe: $(objDir)\Squery.obj
    $(LINK32) /OUT:$@ $(objDir)\Squery.obj $(LIBS)

$(objDir)\Ssh.obj: $(srcDir)\Ssh.c
    $(CC) $(CFLAGS) /Fo$@ $(srcDir)\Ssh.c

$(binDir)\Ssh.exe: $(objDir)\Ssh.obj
    $(LINK32) /OUT:$@ $(objDir)\Ssh.obj $(LIBS)

$(objDir)\Sstage.obj: $(srcDir)\Sstage.c
    $(CC) $(CFLAGS) /Fo$@ $(srcDir)\Sstage.c

$(binDir)\Sstage.exe: $(objDir)\Sstage.obj
    $(LINK32) /OUT:$@ $(objDir)\Sstage.obj $(LIBS)



clean:
    -del $(objDir)\srbClientUtil.obj
    -del $(objDir)\srbClientUtil2.obj
    -del "$(objDir)\S*.obj"
    -del "$(objDir)\b*.obj"
    del $(TARGETS)

MM:
    echo $(LINK_FLAG)
