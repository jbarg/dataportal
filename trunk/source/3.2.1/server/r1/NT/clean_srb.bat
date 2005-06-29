@echo off

set SRB_NT_HOME=C:\SRB2_0_0rel\NT

rem obj,lib,pch,pdb,idb,pdb,exp,ilk


del /q %SRB_NT_HOME%\srbNtUtil\Debug\*.*
del /q %SRB_NT_HOME%\srbNtUtil\Release\*.*

del /q %SRB_NT_HOME%\SrbClient\Debug\*.*
del /q %SRB_NT_HOME%\SrbClient\Release\*.*

del /q %SRB_NT_HOME%\scommands\obj\*.*
del /q %SRB_NT_HOME%\scommands\bin\*.*

del /q %SRB_NT_HOME%\srbSvrClt\Debug\*.*
del /q %SRB_NT_HOME%\srbSvrClt\Release\*.*

del /q %SRB_NT_HOME%\srbMaster\Debug\*.*
del /q %SRB_NT_HOME%\srbMaster\Release\*.*

del /q %SRB_NT_HOME%\srbServer\Debug\*.*
del /q %SRB_NT_HOME%\srbServer\Release\*.*

echo srb_clean.bat finished !
