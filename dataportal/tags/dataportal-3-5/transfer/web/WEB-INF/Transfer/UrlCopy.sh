#!/bin/bash

cd $4
for i in $(ls ../libTransfer/*.jar)
do 
export CLASSPATH=$CLASSPATH:$i
done

$JAVA_HOME/bin/java uk.ac.clrc.dataportal.transfer.Transfer $1 $2 $3
