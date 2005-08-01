#!/bin/sh -e

if [ "$1" = "" ]
then
 echo "Usage: sh parruntest1.sh <count> <error count (default is 1)>"
 exit 1
fi

i=0

while [ $i -lt $1 ]
do
   sh runtest1.sh m $2 &
   i=`expr $i +  1`
   sleep 2
done
exit 0



