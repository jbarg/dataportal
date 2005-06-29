#!/bin/sh

# file runtest4.sh

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

# now run test4

if [ $muteFlag -eq 0 ]
then
  echo "Starting test4 $testid ... This will take a long time, tests displayed after testing ends."

  if [ "$OS" = "Windows_NT" ]
  then
    sh -ex test4.build.sh $testid > $TMP_DIR/test4.$testid.srb 2>&1
    if [ "$?" -ne 0 ]; then
      echo "test4.build.sh script FAILED, exiting"
      exit 3
    fi
    cp $TMP_DIR/test4.$testid.srb $TMP_DIR/test4.log

    # Make new comparison file
    if [ "$1" = "n" ]
    then cp $TMP_DIR/test4.log test4.build.result.out
    fi
    more.com < $TMP_DIR/test4.$testid.srb
  else
#   sh -ex test4.build.sh $testid 2>&1 | tee $TMP_DIR/test4.$testid.srb
# At least on Linux (bash), can't use tee and still get the error
# exit codes from the test4.build.sh script; so use simple redirect:
    sh -ex test4.build.sh $testid > $TMP_DIR/test4.$testid.srb 2>&1
# check for failure
    if [ "$?" -ne 0 ]; then
      echo "test4.build.sh script FAILED, exiting"
      exit 3
    fi
    cp $TMP_DIR/test4.$testid.srb $TMP_DIR/test4.log

    # Make new comparison file
    if [ "$1" = "n" ]
    then cp $TMP_DIR/test4.log test4.build.result.out
    fi
  fi
else
  echo "Starting test4 $testid ... This will take a long time, tests are not displayed"

  sh -ex test4.build.sh $testid > $TMP_DIR/test4.$testid.srb 2>&1
  if [ "$?" -ne 0 ]; then
    echo "test4.build.sh script FAILED, exiting"
    exit 3
  fi
  cp $TMP_DIR/test4.$testid.srb $TMP_DIR/test4.log
fi

# compare build results

#echo "Starting mydiff test4.build.result.out $TMP_DIR/test4.$testid.srb"

wc test4.build.result.out $TMP_DIR/test4.$testid.srb
# ./mydiff test4.build.result.out $TMP_DIR/test4.$testid.srb $numoferrors

# now clean up

#echo "Performing rm $TMP_DIR/test4.$testid.srb"
rm $TMP_DIR/test4.$testid.srb
echo "Successfull rm"
now=`date`
echo "Success: Test $testid completed at $now" >> runtest4.build.sh.log
echo "Runtest4 ended Successfully"
exit 0


