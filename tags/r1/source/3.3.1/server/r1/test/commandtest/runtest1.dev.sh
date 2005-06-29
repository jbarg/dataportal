#!/bin/sh

# file runtest1.sh

if [ "$1" = "m" ]
then muteFlag=1
else muteFlag=0
fi

# set date variable

testid=`date "+%Y%m%d%H%M%S"`

# now run test1

if [ "$OS" = "Windows_NT" ]
then
  binDir=../../NT/scommands/bin
  $binDir/Sexit > flushout
else
  binDir=../../utilities/bin
fi

if [ $muteFlag -eq 0 ]
then
  echo "Starting test1 $testid ... This will take a long time, tests are displayed,
and results are saved in test1.dev.out"

  if [ "$OS" = "Windows_NT" ]
  then
    echo "Tests displayed after testing ends"
    sh -ex test1.dev.sh $testid > test1.dev.out 2>&1
    more.com < test1.dev.out
  else
    sh -ex test1.dev.sh $testid  2>&1 | tee test1.dev.out
  fi
else
  echo "Starting test1 $testid ... This will take a long time, tests are not displayed,
results are stored in test1.dev.out"

  sh -ex test1.dev.sh $testid > test1.dev.out 2>&1
fi


exit 0


