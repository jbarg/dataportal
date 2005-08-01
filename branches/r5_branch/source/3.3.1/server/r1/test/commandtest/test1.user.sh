#!/bin/sh -ex

#####################################################################################
#  test1, uses some of the srb commands, mostly on srb Ojects in order to see if the
#  commands are all working according to their design purpose.
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
  binDir=../../NT/scommands/Debug
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
#  Make new test directory in srb space, and get meta attributes
#####################################################################################
$binDir/Smkdir Stest1Dir$testid
$binDir/Scd Stest1Dir$testid
for store1 in $storeset ; do
  $binDir/Smkdir S_sub1
  $binDir/Sls -lr
  $binDir/Scd S_sub1

#####################################################################################
#  Put local data files into newly created directories in srb space, do some 
#  description of file annotation, then modify some meta data.
#####################################################################################
  for file in $DATA_FILES ; do
      $binDir/Sput -S $store1 $file "$file&COPIES=all"
      $binDir/Sannotate -w page1 "Test Data Stuff" $file
      $binDir/Sannotate -Y 0 $file
      $binDir/SmodD -t text $file
  done

  $binDir/Scd ..

#####################################################################################
#  Display various data information about objects in srb space
#####################################################################################
  $binDir/SgetD  -Y 0 -L 500 "S_sub1/*"
  $binDir/Sls -lr

#####################################################################################
#  Display and Get all data files from srb space, store them in temp directory, 
#  and compare them with another copy of same file to see if they match.
#####################################################################################
  for file in $DATA_FILES ; do
      $binDir/Scat S_sub1/$file
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
#  Copy all data across different platforms, replicate, then get data to local 
#  environment, and compare with local copies of same data.
#####################################################################################
  for store in $storeset ; do
    for file in $DATA_FILES ; do
       $binDir/Scp -S $store S_sub2/$file S_sub1/$file.$store
       $binDir/Sreplicate -S $store S_sub1/$file.$store
    done

    $binDir/Sls -lr
    $binDir/SgetD  -Y 0 -L 200 "S_sub1/*"
    $binDir/SgetD  -Y 0 -L 200 "S_sub2/*"

    for file in $DATA_FILES ; do
      $binDir/Sget S_sub1/$file.$store $TMP_DIR/$file.$testid.tmp
      diff $file $TMP_DIR/$file.$testid.tmp
      wc $file $TMP_DIR/$file.$testid.tmp
      rm $TMP_DIR/$file.$testid.tmp
    done
  done

  $binDir/Scd S_sub2

#####################################################################################
#  copy data_1 to roman, then append all data files to roman file and cat it
#####################################################################################
  $binDir/Scp -S $store data_1 roman
  for file in $DATA_FILES ; do
     $binDir/Sappend -s $file roman
  done
  $binDir/Scat roman
  $binDir/Srm roman
  $binDir/Scd ..

#####################################################################################
#  Do all the clean up, remove data files and directories
#####################################################################################
  $binDir/Scd S_sub1
  $binDir/Srm "data*"
  $binDir/Scd ..
  $binDir/Srmdir S_sub1
  $binDir/Srm -r "S_sub*"
  $binDir/Sls -lr
done

#####################################################################################
#  Remove test directory and end session
#####################################################################################
$binDir/Scd ..
$binDir/Srmdir Stest1Dir$testid
$binDir/Srmtrash
$binDir/Sexit
exit 0
