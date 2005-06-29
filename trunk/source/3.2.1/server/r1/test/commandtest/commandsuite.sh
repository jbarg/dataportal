#!/bin/sh


if [ "$1" = "d" ]
then muteFlag=0
else muteFlag=1
fi

####################################################################################
#  If errors occur then the line number 1 coresponds to the line after bottom dashed
#  line from the result of the first SgetU command in it's testX.roman.out file
####################################################################################

# now run all tests 
echo "Starting commandsuite this will take a VERY LONG time!!!"

if [ "$1" = "new" ]
then
  sh runtest1.sh n
  sh runtest2.sh n
  sh runtest3.sh n
elif [ $muteFlag -eq 0 ]
then
  sh runtest1.sh $2
  sh runtest2.sh $2
  sh runtest3.sh $2
else
  sh runtest1.sh m $1                                                                
  sh runtest2.sh m $1
  sh runtest3.sh m $1
fi

exit 0


