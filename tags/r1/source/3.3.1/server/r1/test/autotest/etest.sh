#!/bin/bash

# commandline: etest.sh Dir Resc1 LogResc Resc2

# These are some 'exotic' tests that Mike would run by hand that we
# are starting to automate now.  

# This should be called with different patterns of resources, local
# and remote; i.e. one test is with all resources local, but another
# good test would be with a remote resource to test network
# communication better.

# The arguments are:
# "quicker" or "complete" - quicker is less complete but a little faster
# Dir - A local file directory to use as input (typically ~5000 files)
# Resc1 - A physical resource to use
# LogResc - A logical resource to use
# Resc2 - A second physical resource to use

set -e # Exit with error if any command does

TestToDo=$1
Speed=$2
Dir=$3
Resc1=$4
LogResc=$5
Resc2=$6
if [ "$Resc2" = "" ]
then
  echo "Usage: etest.sh quicker|complete Dir Resc1 LogResc Resc2"
  exit 1
fi

isOK=no
if [ "$Speed" = "quicker" ]; then
    isOK=yes
    fast=yes
fi
if [ "$Speed" = "complete" ]; then
    isOK=yes
fi

if [ "$isOK" = "no" ]; then
  echo "Usage: etest.sh quicker|complete Dir Resc1 LogResc Resc2"
  exit 1
fi

rmOpt=-rf
if [ $fast ]; then
    echo fast
    rmOpt=-r
fi

# quicker option makes run a little faster (still fairly slow sometimes), and
# complete tests a little more but is slower


set -x # Show commands

# functions ##########################################
function bulkTest {
#Bulk test
    Sinit
    Smkcont -S $LogResc cont1
    Sbload -c cont1 $Dir srcx/cont
    Sput -b -S $Resc1 $Dir srcx
    Scp -b srcx/$Dir srcx/scpdir
    Sget -b srcx
    /bin/rm -f dif.log
    diff -r $Dir srcx/cont > dif.log
    /bin/rm -f dif.log
    diff -r $Dir srcx/$Dir > dif.log
    /bin/rm -f dif.log
    diff -r $Dir srcx/scpdir > dif.log
    /bin/rm -f dif.log
    /bin/rm -rf srcx
    Srmcont -f cont1
    Srm $rmOpt srcx
    Sexit
}

function phymoveTest {
#Phymove test
    Sinit
    Sput -b -S $Resc1 $Dir srcx
    Sphymove -b -s $Resc1 -S $Resc2 srcx
    Smkcont -S $LogResc cont1
    Sphymove -bc cont1 srcx
    Ssyncont -d cont1
    Sreplcont -S $Resc2 cont1
    Sget -b srcx
    /bin/rm -f dif.log
    diff -r srcx $Dir > dif.log
    /bin/rm -f dif.log
    /bin/rm -r srcx
    Srmcont -f cont1
    Srm $rmOpt srcx
    if [ $fast ]; then
	echo Skipping Srmtrash as too slow
    else
	Srmtrash  #slow, not-bulk, operation
    fi
    Sexit
}

function phymoveTest2 {
#Phymove test with -r instead of -b
    Sinit
    Sput -b -S $Resc1 $Dir srcx
    Sphymove -r -s $Resc1 -S $Resc2 srcx
    /bin/rm -rf srcx
    Sget -b srcx
    /bin/rm -f dif.log
    diff -r srcx $Dir > dif.log
    Srm $rmOpt srcx
    if [ $fast ]; then
	echo Skipping Srmtrash as too slow
    else
	Srmtrash  #slow, not-bulk, operation
    fi
    Sexit
}

function checksumTest {
# checksum test
    Sinit
# Still under development
    ls > $Dir/myfile1
    ls -l > $Dir/myfile2
    Sput -bK -S $Resc1 $Dir srcx
    /bin/rm -f chksum.log
    Schksum -lr srcx > chksum.log
# change some files in s:srcx
    /bin/rm -f myfile1 myfile2
    ls > myfile1
    ls -l > myfile2
    Sput -f myfile1 srcx
    Sput -f myfile2 srcx
    /bin/rm -f myfile1 myfile2
# Now Srsync the local files to the SRB files
    /bin/rm -f Srsync.log 
    Srsync -vr $Dir s:srcx > Srsync.log
# And check that the two files were updated (at least in the log)
    grep myfile1 Srsync.log
    if [ $? -ne 0 ]; then
	exit 2 # bad, myfile1 not in the log
    fi
    grep myfile2 Srsync.log
    if [ $? -ne 0 ]; then
	exit 2 # bad, myfile2 not in the log
    fi
    wc=`cat Srsync.log | wc -l | sed 's/ //g'`
    if [ $wc != 4 ]; then
	exit 3 # bad, should have 4 lines, two for each file
    fi
# Now Srsync the SRB space to local
    /bin/rm -f Srsync.log
    Srsync -vr s:srcx $Dir > Srsync.log
    wc=`cat Srsync.log | wc -l | sed 's/ //g'`
    if [ $wc != 0 ]; then
	exit 4 # bad, should have 0 lines
    fi
# change some files in the local directory
    touch testfile1  # so ls output is different
    ls > $Dir/myfile1
    ls -l > $Dir/myfile2
    /bin/rm testfile1
# Now Srsync the SRB space to local again
    /bin/rm -f Srsync.log
    Srsync -vr s:srcx $Dir > Srsync.log
# And check that the two files were updated (at least in the log)
    grep myfile1 Srsync.log
    if [ $? -ne 0 ]; then
	exit 5 # bad, myfile1 not in the log
    fi
    grep myfile2 Srsync.log
    if [ $? -ne 0 ]; then
	exit 6 # bad, myfile2 not in the log
    fi
    wc=`cat Srsync.log | wc -l | sed 's/ //g'`
    if [ $wc != 4 ]; then
	exit 7 # bad, should have 4 lines, two for each file
    fi
# Clean up
    Srm $rmOpt srcx
    /bin/rm -f chksum.log Srsync.log 
    Sexit
}

function parallelXferTest {
# Parallel transfer test
    Sinit
    /bin/rm -rf srcx 
    Sput -MKrvS $Resc1 $Dir srcx
    Sget -Mvkr srcx
    /bin/rm -f dif.log sput.log sget.log
    diff -r $Dir srcx > dif.log
    /bin/rm -f dif.log
    Srm $rmOpt srcx
    /bin/rm -r srcx

    Sput -mkKrvS $Resc1 $Dir srcx > sput.log
    Sget -mvkr srcx > sget.log
    diff -r $Dir srcx
    Srm $rmOpt srcx
    /bin/rm -r srcx
    /bin/rm -f sput.log sget.log
    if [ $fast ]; then
	echo Skipping Srmtrash as too slow
    else
	Srmtrash  #slow, not-bulk, operation
    fi
    Sexit
}
# end functions ######################################


if [ $TestToDo = "a" ]
then
    bulkTest
fi

if [ $TestToDo = "b" ]
then
    phymoveTest
fi

if [ $TestToDo = "c" ]
then
    phymoveTest2
fi

if [ $TestToDo = "d" ]
then
    parallelXferTest
fi

if [ $TestToDo = "e" ]
then
    checksumTest
fi

exit 0
