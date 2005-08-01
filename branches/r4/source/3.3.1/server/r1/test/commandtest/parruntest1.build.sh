#!/bin/sh -e

# rm the file that each runtest1.build.sh will log Success in
rm -f runtest1.build.sh.log

if [ "$1" = "" ]
then
 echo "Usage: sh parruntest1.sh <count> <error count (default is 1)>"
 exit 1
fi

i=1

while [ $i -lt $1 ]
do
   sh runtest1.build.sh m $2 &
   i=`expr $i +  1`
   sleep 2
done
sleep 10
sh runtest1.build.sh m $2

# Check if all have completed successfully, and error exit if not.
# runtest1.build.sh will exit immediately with an error code if any
# subcommands error exit (because sh is running with -e), but for the
# background commands spawned in the above while loop, those exit codes
# will not propogate back up.  So this takes care of that.
wc=`cat runtest1.build.sh.log | wc -l | sed 's/ //g'`
if [ "$wc" != "$1" ]
then 
   echo "Only $wc of the $1 jobs completed successfully; waiting 10 seconds"
   sleep 10
   wc=`cat runtest1.build.sh.log | wc -l | sed 's/ //g'`
   if [ "$wc" != "$1" ]
   then 
       echo "Only $wc of the $1 jobs completed successfully; waiting 30 seconds"
       sleep 30
       wc=`cat runtest1.build.sh.log | wc -l | sed 's/ //g'`
       if [ "$wc" != "$1" ]
       then 
	   echo "Only $wc of the $1 jobs completed successfully; waiting 60 seconds"
	   sleep 60
	   wc=`cat runtest1.build.sh.log | wc -l | sed 's/ //g'`
	   if [ "$wc" != "$1" ]
	   then 
	       echo "Only $wc of the $1 jobs completed successfully; failed"
	       exit 2
	   fi
       fi
   fi
fi
echo "Parallel Test with $1 concurrent tests completed successfully"  
exit 0



