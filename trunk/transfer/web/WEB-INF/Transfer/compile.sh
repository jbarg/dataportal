#!/bin/bash

if [ "$1" == ""-h"" ] ; then
echo "Compilling"
else
for i in $(ls ../libTransfer/*.jar)
do 
export CLASSPATH=$CLASSPATH:$i
done


$JAVA_HOME/bin/javac uk/ac/clrc/dataportal/transfer/*.java
$JAVA_HOME/bin/javac uk/ac/clrc/dataportal/*.java

fi
