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
echo TESTID = $testid
echo TESTDATE = $testdate

#####################################################################################
#  Initialize srb client environment, and get User information
#####################################################################################
$binDir/Sinit -v
$binDir/SgetU -d -Y 0

$binDir/SgetU "test*"
$binDir/SgetU "*@sdsc"
$binDir/SgetU -p -Y 0
$binDir/SgetU -P -Y 0
#  $binDir/SgetU -a       goes into suspend and never comes out
#  $binDir/SgetU -a testuser   works, but too much output for script
#  $binDir/SgetU -t -Y 0  error -3005
$binDir/SgetU -g -Y 0
#  $binDir/SgetU -c -Y 0  goes into suspend and never comes out
#  $binDir/SgetU -c testuser   works, but -L doesn't work, so script would become interactive
$binDir/SgetU -Y 0 -T staff

#####################################################################################
#  Make new test directory in srb space, and get meta attributes
#####################################################################################
$binDir/Sattrs
$binDir/Stoken -L 200 Domain
$binDir/Smkdir Stest1Dev$testid
$binDir/Scd Stest1Dev$testid
for store1 in $storeset ; do
  $binDir/Smkdir S_sub1
  $binDir/Sls -lr
  $binDir/Scd S_sub1

#####################################################################################
#  Put local data files into newly created directories in srb space, do some 
#  description of file annotation, then modify some meta data.
#####################################################################################
  for file in $DATA_FILES ; do
      $binDir/Sput -af -S $store1 $file "$file&COPIES=all"
      $binDir/Sannotate -w page1 "Test Data Stuff" $file
      $binDir/Sannotate -Y 0 $file
      $binDir/Sannotate -p page3 -Y 0 $file
      $binDir/SmodD -t text $file
      $binDir/SmodD -c "My test comment" $file
  done

  $binDir/Sls -f
  $binDir/Scd ..
  $binDir/Sls -lr

#####################################################################################
#  Display various data information about objects in srb space
#####################################################################################
  $binDir/SgetD  -Y 0 -L 500 "S_sub1/*"

#####################################################################################
#  Display and Get all data files from srb space, store them in temp directory, 
#  and compare them with another copy of same file to see if they match.
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
#  Copy all data across different platforms, replicate, then get data to local 
#  environment, and compare with local copies of same data.
#####################################################################################
  for file in $DATA_FILES ; do
     $binDir/Scp -S $store1 S_sub2/$file S_sub1/$file.$store1
     $binDir/Sreplicate -S $store1 S_sub1/$file.$store1
     $binDir/Sphymove -C 1 S_sub1/$file.$store1 ora-sdsc
  done

  $binDir/Sls -lr
  $binDir/SgetD -Y 0 -L 200 "S_sub1/*"
  $binDir/SgetD -Y 0 -L 200 "S_sub2/*"
  $binDir/SgetD -p -Y 0 "S_sub1/*"
  $binDir/SgetD -e -Y 0 -L 200 "S_sub1/*"
  $binDir/SgetD -r -Y 0 "S_sub1/*"
#  $binDir/SgetD -R -Y 0 "S_sub1/*"
  $binDir/SgetD -a -Y 0 "S_sub1/*"
  $binDir/SgetD -s -Y 0 "S_sub1/*"
  $binDir/SgetD -d -Y 0 "S_sub1/*"
#  $binDir/SgetD -D -Y 0 "S_sub1/*"
  $binDir/SgetD -c -Y 0 "S_sub1/*"
  $binDir/SgetD -W -Y 0 "S_sub1/*"
  $binDir/SgetD -U testuser -Y 0 "S_sub1/*"

  $binDir/SgetR -Y 0 -L 200
  $binDir/SgetR -Y 0 -L 200 "unix*"
  $binDir/SgetR -l -Y 0 -L 200
#  $binDir/SgetR -d -Y 0 -L 200
#  $binDir/SgetR -D -Y 0 -L 200
  $binDir/SgetR -p -Y 0 -L 200
  $binDir/SgetR -p -Y 0 -L 200

  $binDir/Scd S_sub2

#####################################################################################
#  copy data_1 to roman, then append all data files to roman file and cat it
#####################################################################################
  $binDir/Scp data_1 roman
  for file in $DATA_FILES ; do
     $binDir/Sappend -s $file roman
  done
  $binDir/Scat roman
  $binDir/Srm roman
  $binDir/Scd ..

  $binDir/Scd S_sub1

  for file in $DATA_FILES ; do
    $binDir/Srename $file.$store1 $file.roman
    $binDir/Scat -C 1 $file.roman
    $binDir/Schmod n testuser sdsc $file.roman
    $binDir/Scat -C 1 $file.roman
    $binDir/Schmod a testuser sdsc $file.roman
    $binDir/Sget -n 1 $file.roman $TMP_DIR/$file.$testid.tmp
    diff $file $TMP_DIR/$file.$testid.tmp
    wc $file $TMP_DIR/$file.$testid.tmp
    rm $TMP_DIR/$file.$testid.tmp
  done

  $binDir/Scd ..
  $binDir/Sls -l
#####################################################################################
#  Do all the clean up, remove data files and directories
#####################################################################################
  $binDir/Srm -r "S_sub*"
  $binDir/Sls -lr
done

#####################################################################################
#  Remove test directory and end session
#####################################################################################
$binDir/Scd ..
$binDir/Srmdir Stest1Dev$testid
$binDir/Sexit
exit 0
