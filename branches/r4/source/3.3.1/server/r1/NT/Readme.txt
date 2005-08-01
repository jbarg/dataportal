##########################################################################################
# Bing ZHu                                                                               #
# San Diego Supercomputer Center                                                         #
# University of California: San Diego                                                    #
# Jan 7,2003                                                                             #
##########################################################################################


Build Windows SRB Software
--------------------------
There are two batch files, 'build_srb.bat' and 'clean_srb.bat', for building Windows 
SRB software and cleaning the old SRB object files, binaries and library files.

The only configuration needed is to set the SRB_NT_HOME to the physical path of the 'NT'
directory within a machine.

To build and clean SRB software, simply type in 'build_srb' or 'clean_srb'.

In case there is an error message saying that the file 'vcvars32.bat' can not be found,
try any one of the followings.
 
1. Locate the file and modify the following line in the 'build_srb.bat'.
      call "C:\Program Files\Microsoft Visual Studio\VC98\Bin\vcvars32.bat"

2. MS .net Visual Studio users can simply use the console utility under tools menu. 
   (Remove the above-mentioned line if there is an error.)


SRB client library
-------------------
Two projects, srbClient and srbSvrClt, contain same files. They build libraries for 
different  purposes. 

The 'srbClient' creates a static library, srbCLient.lib which is used for building SRB 
client software such as S-commands, Windows browser, etc. For users who needs to use SRB 
client APIs, use this library file. 

The 'srbSvrClt' creates a static library, srbSvrClt.lib, which is used only by SRB server.
