#!/bin/sh

# file runtest3.sh

if [ "$1" = "m" ]
then muteFlag=1
else muteFlag=0
fi

# set date variable

testid=`date "+%Y%m%d%H%M%S"`

# now run test3

if [ "$OS" = "Windows_NT" ]
then
  binDir=../../NT/scommands/bin
  $binDir/Sexit > flushout
else
  binDir=../../utilities/bin
fi

if [ $muteFlag -eq 0 ]
then
  echo "Starting test3 $testid ... This will take a long time, tests are displayed,
and results are saved in test3.dev.out"

  if [ "$OS" = "Windows_NT" ]
  then
    echo "Tests displayed after testing ends"
    sh -ex test3.dev.sh $testid > test3.dev.out 2>&1
    more.com < test3.dev.out
  else
    sh -ex test3.dev.sh $testid  2>&1 | tee test3.dev.out
  fi
else
  echo "Starting test3 $testid ... This will take a long time, tests are not displayed,
results are stored in test3.dev.out"

  sh -ex test3.dev.sh $testid > test3.dev.out 2>&1
fi


exit 0


