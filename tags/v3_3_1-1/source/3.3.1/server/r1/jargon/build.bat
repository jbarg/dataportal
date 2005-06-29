@echo off
javac -d bin/ src/api/edu/sdsc/grid/io/local/*.java src/api/edu/sdsc/grid/io/srb/*.java src/api/edu/sdsc/grid/io/metadata*.java src/api/edu/sdsc/grid/io/*.java
javac -d bin/ -classpath bin/ src/test/*.java
