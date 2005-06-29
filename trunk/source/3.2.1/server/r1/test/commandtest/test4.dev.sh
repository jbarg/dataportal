#!/bin/sh -ex

#####################################################################################
#  test4, tests whether  the wildcards  create problems or not
#  commands are all working according to their design purpose.
#  
#  Unless otherwise stated, all commands with # in front of them, don't seem to be 
#  working properly.
#
#  By: Arcot Rajasekar
#####################################################################################

storeset=`cat ResourcesToTest4`

if [ "$OS" = "Windows_NT" ]
then
  binDir=../../NT/scommands/bin
else
  binDir=../../utilities/bin
fi

DATA_FILES="data1 data2 data3 data4 datadb1.xml dataempty"
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

for store1 in $storeset ; do
 $binDir/Sinit -v
 $binDir/Smkdir wctest-$testid
 $binDir/Smkdir wctest-$testid/C1_a_b
 $binDir/Smkdir wctest-$testid/C1_a_b/C2_alpha_beta
 $binDir/Smkdir wctest-$testid/C1_a_b/C2_alphaobeta

 $binDir/Scd wctest-$testid/C1_a_b/C2_alpha_beta
 $binDir/Spwd
 $binDir/Sput -S $store1 data1 a_b_1 
 $binDir/Sput -S $store1 data1 a_b_2 
 $binDir/Sput -S $store1 data1 a_c_1
 $binDir/Sput -S $store1 data1 a_c_2
 $binDir/Sput -S $store1 data1 ab__2
 $binDir/Sput -S $store1 data1 a___2
 $binDir/Sls -lr 
 $binDir/Sls -lr  ..
 $binDir/Sls -lr  '../*b*'
 $binDir/Sls -lr  ../..
 $binDir/Sls -lr  '../*'
 $binDir/Scd ..
 $binDir/Spwd
 $binDir/Sput -S $store1 data1 'C2_a*_*/a_d_1' 
 $binDir/Sput -S $store1 data1 'C2_a*_*/adb_1' 
 $binDir/Sput -S $store1 data1 'C2_*o*/a_d_2'
 $binDir/Sls -lr 
 $binDir/Scd ../..
 $binDir/Spwd
 $binDir/SgetD -a -W -L 100 wctest-$testid/\*/\*/\* |wc
 $binDir/Schmod -r w sekar sdsc  wctest-$testid
 $binDir/SgetD -a -W -L 100 wctest-$testid/\*/\*/\* |wc
 $binDir/Schmod -r w mwan sdsc  wctest-$testid/C1_a_b/C2_alpha_beta/\* 
 $binDir/SgetD -a -W -L 100 wctest-$testid/\*/\*/\* |wc
 $binDir/Schmod -r w sheauc sdsc  wctest-$testid/C1_a_b/C2_\*beta/\*
 $binDir/SgetD -a -W -L 100 wctest-$testid/\*/\*/\* |wc
 $binDir/Sls -lr  wctest-$testid
 mkdir tmp.$testid
 $binDir/Sget wctest-$testid/C1_a_b/C2_alpha_beta/a_b\* tmp.$testid
 ls -lR tmp.$testid
 $binDir/Sget -rf wctest-$testid/C1_a_b/C2_alpha_beta  tmp.$testid
 ls -lR tmp.$testid 
 $binDir/Sget -rf wctest-$testid/C1_a_b  tmp.$testid
 ls -lR tmp.$testid 
 rm -r tmp.$testid
 $binDir/Sreplicate -S $store1 wctest-$testid/C1_a_b/C2_alpha_beta/a_b_1
 $binDir/Sls -lr wctest-$testid
 $binDir/Sls -lr wctest-$testid |wc
 $binDir/Sreplicate -S $store1 wctest-$testid/C1_a_b/C2_alpha_beta/a_b\*
 $binDir/Sls -lr wctest-$testid |wc
 $binDir/Sreplicate -S $store1 wctest-$testid/C1_a_b/C2_alpha_beta/a_c\?1
 $binDir/Sls -lr wctest-$testid |wc
 $binDir/Sreplicate -S $store1 wctest-$testid/C1_a_b/C2_alpha_beta/a_c\?2
 $binDir/Sls -lr wctest-$testid |wc
 $binDir/Sreplicate -S $store1 wctest-$testid/\*/C2_alpha_beta/a_d_1
 $binDir/Sls -lr wctest-$testid |wc
 $binDir/Sreplicate -S $store1 wctest-$testid/C1_a_b/C2_alpha_beta/a___2
 $binDir/Sls -lr wctest-$testid |wc
 $binDir/Sreplicate -S $store1 wctest-$testid/C1_a_b/C2_alpha_b\*/a___2
 $binDir/Sls -lr wctest-$testid |wc
 $binDir/Sreplicate -S $store1 wctest-$testid/C1_a_b/C2_alpha_beta/a_\?_2
 $binDir/Sls -lr wctest-$testid |wc
 $binDir/Sreplicate -S $store1 wctest-$testid/C1_a_b/C2_alpha\?beta/a_\?_2
 $binDir/Sls -lr wctest-$testid |wc
 $binDir/Srm -r wctest-$testid
 $binDir/Sexit
done
echo "test completed..."
exit 0
