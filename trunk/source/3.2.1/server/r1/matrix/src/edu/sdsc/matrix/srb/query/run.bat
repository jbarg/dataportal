
echo [Information] jjtree Complete
goto start
:start
jjtree MatrixQueryParser.jjt
echo [Information] jjtree Complete
goto javacc

:javacc
javacc -debug_parser -LOOKAHEAD:2 -FORCE_LA_CHECK:true MatrixQueryParser.jj
goto javac

:javac
javac *.java
goto jjdoc

:jjdoc
jjdoc MatrixQueryParser.jj
goto java

:java
java MatrixQueryParser

:end


INSTRUCTIONS FOR JAVACC
--------------------------

Step 1
-------
jjtree -OUTPUT_DIRECTORY:"%MATRIX_HOME%\src\edu\sdsc\matrix\srb\query"  %MATRIX_HOME%\src\edu\sdsc\matrix\srb\query\MatrixQueryParser.jjt

Step 2
-------
javacc -LOOKAHEAD:2 -FORCE_LA_CHECK:true -OUTPUT_DIRECTORY:"%MATRIX_HOME%\src\edu\sdsc\matrix\srb\query"   %MATRIX_HOME%\src\edu\sdsc\matrix\srb\query\MatrixQueryParser.jj

OR 

With Debug Messages while parsing 
javacc -debug_parser -LOOKAHEAD:2 -FORCE_LA_CHECK:true -OUTPUT_DIRECTORY:"%MATRIX_HOME%\src\edu\sdsc\matrix\srb\query"   %MATRIX_HOME%\src\edu\sdsc\matrix\srb\query\MatrixQueryParser.jj
