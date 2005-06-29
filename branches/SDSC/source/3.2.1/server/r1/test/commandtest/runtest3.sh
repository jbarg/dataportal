#!/bin/sh

# file runtest3.sh

if [ "$1" = "m" ]
then 
  muteFlag=1
  if [ "$2" = "" ]  
  then numoferrors=1
  else numoferrors=$2
  fi
elif [ "$1" = "n" ]
then
  muteFlag=0
  numoferrors=1
else 
  muteFlag=0
  numoferrors=$1
fi

if [ "$1" = "" ]
then numoferrors=1
fi

if [ $numoferrors -gt 0 ]
then :
else numoferrors=1
fi

if [ "$OS" = "Windows_NT" ]
then
  binDir=../../NT/scommands/bin
  $binDir/Sexit > flushout
else
  binDir=../../utilities/bin
fi

# set date variable

testid=`date "+%Y%m%d%H%M%S"`
TMP_DIR=../TMP

# now run test3

if [ $muteFlag -eq 0 ]
then
  echo "Starting test3 $testid ... This will take a long time, tests displayed after testing ends."

  if [ "$OS" = "Windows_NT" ]
  then
    sh -ex test3.user.sh $testid > $TMP_DIR/test3.$testid.srb 2>&1
    cp $TMP_DIR/test3.$testid.srb $TMP_DIR/test3.log

    # Make new comparison file
    if [ "$1" = "n" ]
    then cp $TMP_DIR/test3.log test3.result.out
    fi
    more.com < $TMP_DIR/test3.$testid.srb
  else
    sh -ex test3.user.sh $testid 2>&1 | tee $TMP_DIR/test3.$testid.srb
    cp $TMP_DIR/test3.$testid.srb $TMP_DIR/test3.log

    # Make new comparison file
    if [ "$1" = "n" ]
    then cp $TMP_DIR/test3.log test3.result.out
    fi
  fi
else
  echo "Starting test3 $testid ... This will take a long time, tests are not displayed"

  sh -ex test3.user.sh $testid > $TMP_DIR/test3.$testid.srb 2>&1
  cp $TMP_DIR/test3.$testid.srb $TMP_DIR/test3.log
fi

# compare results

echo "Starting mydiff test3.result.out $TMP_DIR/test3.$testid.srb"

wc test3.result.out $TMP_DIR/test3.log
./mydiff test3.result.out $TMP_DIR/test3.log $numoferrors

# now clean up

echo "Performing rm $TMP_DIR/test3.$testid.srb"
rm $TMP_DIR/test3.$testid.srb
echo "Successfull rm"
exit 0


