#!/bin/sh


####################################################################################
#  If errors occur then the line number 1 coresponds to the line after bottom dashed
#  line from the result of the first SgetU command in it's testX.roman.out file
####################################################################################


if [ "$1" = "" ]
then
 echo "Usage: sh parsuite.sh <count> <error count (default is 1)>"
 exit 1
fi
i=0

# now run all tests
echo "Starting parsuite this will take a VERY LONG time!!!"

while [ $i -lt $1 ]
do
  sh runtest1.sh m $2 &
  sleep 2
  sh runtest2.sh m $2 &
  sleep 2
  sh runtest3.sh m $2 &
  sleep 2
  i=`expr $i +  1`
done

exit 0


