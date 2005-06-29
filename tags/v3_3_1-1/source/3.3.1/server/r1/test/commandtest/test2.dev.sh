#!/bin/csh -ex

#####################################################################################
#  test2, uses some of the srb commands on srb Objects inside of containers, in order 
#  to see if they are all working according to their design purpose.
#  
#  Unless otherwise stated, all commands with # in front of them, don't seem to be 
#  working properly.
#
#  By: Arcot Rajasekar
#  Modified By: Roman Olschanowsky
#####################################################################################

storeset=`cat ResourcesToTest`

if [ "$OS" = "Windows_NT" ]
then
  binDir=../../NT/scommands/bin
else
  binDir=../../utilities/bin
fi

DATA_FILES="data_1 data_2 data_3 data_4 datadb1.xml dataempty"
TMP_DIR=../TMP

#####################################################################################
#  Make unique names for directories using current date and time
#####################################################################################

if [ "$1" = "" ]
then testid=`date "+%Y%m%d%H%M%S"`
else testid=$1
fi

testdate=`date "+%Y-%m-%d"`
echo "TESTID = $testid"
echo "TESTDATE = $testdate"

#####################################################################################
#  Initialize srb client environment, and get User information
#####################################################################################
$binDir/Sinit -v
$binDir/SgetU -d -Y 0

#####################################################################################
#  Make new test directory in srb space
#####################################################################################

$binDir/Smkdir Stest2Dev$testid
$binDir/Scd Stest2Dev$testid
for store1 in $storeset ; do
  $binDir/Smkdir S_sub1
  $binDir/Sls -lr
  $binDir/Scd S_sub1

#####################################################################################
#  Put local data files into newly created container in srb space.
#####################################################################################

  $binDir/Smkcont Container$testid

  for file in $DATA_FILES ; do
      $binDir/Sput -c Container$testid -S $store1 $file "$file&COPIES=all" 
  done

  $binDir/Scd ..

# Raja doesn't want me to use Srename, he is having problems with it
# So I'll try move  $binDir/Srename -c Container$testid Container.2.$testid

# Doesn't work either  $binDir/Smv Container$testid Container.2.$testid

# Currently we can not move or rename a container

  $binDir/Slscont -l Container$testid
#takes too long  $binDir/Slscont -l

#####################################################################################
#  Display various information about objects in srb space
#####################################################################################
  $binDir/SgetD -Y 0 -L 500 "S_sub1/*"
  $binDir/Sls -lr

#####################################################################################
#  Get all data files from srb space, store them in temp directory, and compare them
#  with another copy of same file to see if they match.
#####################################################################################
  for file in $DATA_FILES ; do
      $binDir/Sget S_sub1/$file $TMP_DIR/$file.$testid.tmp
      diff $file $TMP_DIR/$file.$testid.tmp
      wc $file $TMP_DIR/$file.$testid.tmp
      rm $TMP_DIR/$file.$testid.tmp
  done

  $binDir/Smkdir S_sub2

#####################################################################################
#  Move all files from Sub1 to Sub2
#####################################################################################
  for file in $DATA_FILES ; do
     $binDir/Smv S_sub1/$file S_sub2
  done

  $binDir/Sls -lr
 
#####################################################################################
#  Copy all data across different platforms, then get data to local environment, and
#  compare with local copies of same data.
#####################################################################################

  for store in $storeset ; do
    $binDir/Smkcont Container$testid.$store

    for file in $DATA_FILES ; do
       $binDir/Scp -c Container$testid.$store -S $store S_sub2/$file S_sub1/$file.$store
    done

    $binDir/Ssyncont Container$testid.$store

    $binDir/Sls -lr
#takes too long    $binDir/Slscont 

    for file in $DATA_FILES ; do
      $binDir/Sget S_sub1/$file.$store $TMP_DIR/$file.$testid.tmp
      diff $file $TMP_DIR/$file.$testid.tmp
      wc $file $TMP_DIR/$file.$testid.tmp
      rm $TMP_DIR/$file.$testid.tmp   
    done

    $binDir/Scd S_sub1
    $binDir/Srm "data*"
    $binDir/Scd ..
    $binDir/Srmcont Container$testid.$store
  done

#####################################################################################
#  Do all the clean up, remove data files, directories, and containers
#####################################################################################
  $binDir/Srmdir S_sub1
  $binDir/Srm -r "S_sub*"
  $binDir/Srmcont Container$testid
#  $binDir/Slscont "Container$testid*"
  $binDir/Sls -lr
done

#####################################################################################
#  Remove test directory and end session
#####################################################################################
$binDir/Scd ..
$binDir/Srmdir Stest2Dev$testid
$binDir/Sexit
exit 0
