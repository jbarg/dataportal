#!/bin/bash
#
# THIS IS NOT RECOMMENDED FOR USE OUTSIDE OF SDSC.  It is designed
# for use on SDSC systems and can put a heavy load on the srb server,
# postgres, and the OS.
#
# This script is part of an automatic build and test system at SDSC.
# It downloads needed files, cvs checkout's the current SRB
# source tree, and builds and tests a complete temporary
# SRB installation.
#
# Note: this runs the install.pl script which will change your .MdsaEnv,
# .MdasAuth files, and .odbc.ini, saving your current ones in files with a
# date/time extension.
#
# Assumptions:
#  1) Your build machine/userid has CVS access to the SRB repository.
#  2) There can be no postgres or srb server processes running on your 
#     build host.
#  3) Your build machine has network access and a normal build environment
#     (complier and such) and is one of the platforms supported by install.pl.
#
# Steps:
#
# 1) First, copy this file to an empty working directory with plenty of
#    disk space, preferably (for speed) on a local harddrive.
# 
# 2) run this script: 'autotest.sh'
#
# SUCCESS: If the full build, simple tests, and more complete test all
# succeed, the last message displayed with be "Success" and the exit code
# will be 0.  If something goes wrong, the script will quit with a failure 
# message and non-zero exit code.
#
# If everything goes well, this script will automatically stop the processes
# and remove all the files that were created.  If you instead want
# the system left in place, add an exit call before the section commented
# with 'clean up'.
#

# Command line arguments can also be used to control which steps are
# done: build/install, the various tests, and cleanup.  Include the
# numbers on on the command line of the steps you want to do.  Those
# included will be done, all others skipped.  See the code below (the
# checks on the doit array) for the numbers of the steps.  For
# example, 'autotest.sh 0 1' will do steps 0 (build/install) and step
# 1 (simpleTests). '0 1 2 3 4 5 6 7 8 9 10' will do everything except
# cleanup.  For safety, cleanup cannot be specified by number, use 
# 'autotest.sh cleanup' instead.

doitMax=12; # largest test item number (in the doit array)


# functions ################################################
function buildAll {
# get, build, and run everything
#
    set -x # display commands

# Check that the current working directory is empty except for
# a few possible standard files.  This is to prevent someone from
# inadvertantly running this from it's home in the cvs tree.
# If you are rerunning this, and know what you are doing, you
# can remove this logic.  The ls and grep line will have an exit code
# of 1 if the result is empty; meaning only the known-possible files
# exist.
    ls -C1 | grep -v postgresql | grep -v psqlodbc | grep -v autotest.sh | grep -v SRB2_0_0rel | grep -v date1.log | grep -v etest.sh
    if [ "$?" -ne 1 ]; then
	echo "This script should be run from an empty directory"
	echo "See the top of the script for more information"
	exit 2
    fi

# Check if we have the postgresql release file, if not go get
# it from an postgresql mirror site.
    ls | grep postgresql-7.4.3.tar
    if [ "$?" -ne 0 ]; then
	rm -f ftp.in
	echo "user anonymous schroede@sdsc.edu
cd postgresql/source/v7.4.3
bin
get postgresql-7.4.3.tar.gz
quit" > ftp.in
	ftp -n ftp8.us.postgresql.org < ftp.in
    fi

# Check if we have the ODBC release file, if not go get
# it from an postgresql mirror site.
    ls | grep psqlodbc-07.03.0200.tar
    if [ "$?" -ne 0 ]; then
	rm -f ftp.in
	echo "user anonymous schroede@sdsc.edu
cd postgresql/odbc/versions/src
bin
get psqlodbc-07.03.0200.tar.gz
quit" > ftp.in
	ftp -n ftp8.us.postgresql.org < ftp.in
    fi

# Checkout the current SRB source tree unless it already is here
    ls SRB2_0_0rel
    if [ "$?" -ne 0 ]; then
	rm -f cvsCheckout.log
	cvs checkout SRB2_0_0rel >& cvsCheckout.log
	if [ "$?" -ne 0 ]; then
	    exit 1
	fi
    fi

# Get the install script
    cp SRB2_0_0rel/MCAT/install.pl .

# Get the install configure file for this configuration                       
    if [ $optionalConfigurationOne ]; then
        cp SRB2_0_0rel/test/autotest/autotest.conf1 ./install.conf
        cat SRB2_0_0rel/test/autotest/autotest.conf >> ./install.conf
    else
	if [ $optionalConfigurationTwo ]; then
	    cp SRB2_0_0rel/test/autotest/autotest.conf2 ./install.conf
	    cat SRB2_0_0rel/test/autotest/autotest.conf >> ./install.conf
	else
	    if [ $optionalConfigurationThree ]; then
		cp SRB2_0_0rel/test/autotest/autotest.conf3 ./install.conf
		cat SRB2_0_0rel/test/autotest/autotest.conf >> ./install.conf
	    else
		cp SRB2_0_0rel/test/autotest/autotest.conf ./install.conf
	    fi
	fi
    fi

#
# run the install.pl
    perl install.pl
    if [ "$?" -ne 0 ]; then
	echo "install FAILED, stopping"
	exit 2
    fi
}


function rebuild {
# This can be used if you've already done a buildAll and want to
# do a rebuild of SRB but reuse the postgres installation (while
# still dropping and recreating the MCAT db).
#
    set -x # display commands

# Check for Mac OS X
   os=`uname -s`
   gmake="gmake";
   if [ $os == "Darwin" ]; then
       gmake="make";
   fi

# clean up the previous SRB build
    cd SRB2_0_0rel
    rm -f gmake.log
    $gmake clean  > gmake.log
    cd MCAT
    rm -f gmake.log
    $gmake clean > gmake.log
    cd ../..

# Stop the current srb servers 
    set -e
    echo "y" > ttyYes
    SRB2_0_0rel/bin/killsrb < ttyYes
    rm -f ttyYes

# drop the MCAT DB
    pgsql/bin/dropdb MCAT

# Stop the current postgres servers 
    cwd=`pwd`
    export PGDATA="$cwd/pgsql/data"
    pgsql/bin/pg_ctl stop

# Remove the postgres data directory
    rm -rf pgsql/data

# patch the install.state file to be after the postgres and odbc builds
    rm install.state
    echo "C0" > install.state

# clean up the Vault directories
    rm -rf Vault*

# Set the install configure file for this configuration
    if [ $optionalConfigurationOne ]; then
        cp SRB2_0_0rel/test/autotest/autotest.conf1 ./install.conf
        cat SRB2_0_0rel/test/autotest/autotest.conf >> ./install.conf
    else
	if [ $optionalConfigurationTwo ]; then
	    cp SRB2_0_0rel/test/autotest/autotest.conf2 ./install.conf
	    cat SRB2_0_0rel/test/autotest/autotest.conf >> ./install.conf
	else
	    if [ $optionalConfigurationThree ]; then
		cp SRB2_0_0rel/test/autotest/autotest.conf3 ./install.conf
		cat SRB2_0_0rel/test/autotest/autotest.conf >> ./install.conf
	    else
		cp SRB2_0_0rel/test/autotest/autotest.conf ./install.conf
	    fi
	fi
    fi


# run the install.pl
    perl install.pl
}


function simpleTests {
# Run a few simple tests
    Sinit
    if [ "$?" -ne 0 ]; then
	echo "Scommand FAILED, stopping"
	exit 3
    fi
    ls > foo
    Sput foo
    if [ "$?" -ne 0 ]; then
	echo "Scommand FAILED, stopping"
	exit 3
    fi
    Sls -l
    if [ "$?" -ne 0 ]; then
	echo "Scommand FAILED, stopping"
	exit 3
    fi
    Srm foo
    if [ "$?" -ne 0 ]; then
	echo "Scommand FAILED, stopping"
	exit 3
    fi
    Sls -l
    if [ "$?" -ne 0 ]; then
	echo "Scommand FAILED, stopping"
	exit 3
    fi
    Srmtrash
    if [ "$?" -ne 0 ]; then
	echo "Scommand FAILED, stopping"
	exit 3
    fi
    Sexit
    if [ "$?" -ne 0 ]; then
	echo "Scommand FAILED, stopping"
	exit 3
    fi
}

function commandTest {
# Run the command test script
    cd SRB2_0_0rel/test/commandtest
    echo "demoResc demoResc2" > ResourcesToTest

    runtest1.build.sh
    if [ "$?" -ne 0 ]; then
	echo "runtest1.build.sh FAILED, stopping"
	exit 4
    fi
    cd ../../..
}

function testSuite {
   set +e
   host1=`uname -n` # may not have domain on some systems
   host=`host $host1 | grep "has address" | cut -f 1 -d ' '`  # should be full name

   if [ $host ]; then
      echo $host
    else
# if still not set, try just name (SunOS has no 'host' command)
      host=`uname -n`
   fi
   set -e

# Check for Mac OS X
   os=`uname -s`
   gmake="gmake";
   if [ $os == "Darwin" ]; then
       gmake="make";
   fi

# configure, run, and check the testsuite
    cd SRB2_0_0rel/test/testsuite
    cat testsuite.in | sed s/andal.sdsc.edu/$host/g > testsuite
    $gmake > gmake.log
    testsuite >& testsuite.log
    grep "Testsuite Completed Successfully" testsuite.log
    if [ "$?" -ne 0 ]; then
	echo "testsuite FAILED, stopping"
	exit 5
    fi
    cd ../../..
}

function parallelCommandTest {
# Run the parallel command test script
    cd SRB2_0_0rel/test/commandtest
    echo "demoResc demoResc2" > ResourcesToTest
    parruntest1.build.sh 8 1
    if [ "$?" -ne 0 ]; then
	echo "parruntest1.build.sh FAILED, stopping"
	exit 6
    fi
    cd ../../..
}

function makeMoreResources {
    set -e # error exit on any command errors in following commands
# Create some additional resources for the following tests
    hostName=`uname -n`
    cwd=`pwd`
    vaultPath3="$cwd/Vault3/?USER.?DOMAIN/?SPLITPATH/?PATH?DATANAME.?RANDOM.?TIMESEC"
    vaultPath4="$cwd/Vault4/?USER.?DOMAIN/?SPLITPATH/?PATH?DATANAME.?RANDOM.?TIMESEC"
    vaultPath5="$cwd/Vault5/?USER.?DOMAIN/?SPLITPATH/?PATH?DATANAME.?RANDOM.?TIMESEC"
    cd ./SRB2_0_0rel/MCAT/bin
    ./ingestResource permResc 'unix file system' $hostName $vaultPath3 permanent 0
    ./ingestResource tempResc 'unix file system' $hostName $vaultPath4 temporary 0
    ./ingestLogicalResource  demoLogResc 'logical' tempResc ''
    ./addLogicalResource demoLogResc 'logical' permResc ''
    if [ $remoteResource ]; then
	echo "Not ingesting tempResc2 as it will be remote"
    else
	./ingestResource tempResc2 'unix file system' $hostName $vaultPath5 temporary 0
    fi

    cd ../../..
}

function exoticTest0 {
# Make a directory with input-arg empty files and test with that
    set -e # error exit on any command errors in following commands
    rm -rf tmpdir
    mkdir tmpdir
    cd tmpdir
    set +x
    i=0
    while [ $i -lt $1 ]
      do
#     touch f$i
      echo "a" > f$i
      i=`expr $i + 1`
    done
    set -x
    cd ..

# Make a fairly small file
    rm -f mytestfile1
    cat *log > mytestfile1

# Create small files for the file directory
    rm -rf smallFileDir
    mkdir smallFileDir
    cat mytestfile1 > smallFileDir/f1
    cat mytestfile1 mytestfile1 > smallFileDir/f2
    cat mytestfile1 mytestfile1 mytestfile1 > smallFileDir/f3
    ls -l  > smallFileDir/f4

    if [ -a etest.sh ]; then
	echo etest.sh exists
    else
	cp SRB2_0_0rel/test/autotest/etest.sh .
    fi

#    etest.sh quicker tmpdir demoResc demoLogResc tempResc2 smallFileDir
    etest.sh a complete tmpdir demoResc demoLogResc tempResc2
    etest.sh b complete tmpdir demoResc demoLogResc tempResc2
    etest.sh c complete tmpdir demoResc demoLogResc tempResc2
    etest.sh c complete smallFileDir demoResc demoLogResc tempResc2
    etest.sh d complete tmpdir demoResc demoLogResc tempResc2
    etest.sh e complete tmpdir demoResc demoLogResc tempResc2
    rm -rf tmpdir smallFileDir mytestfile1
}

function exoticTest1 {
    set +e # Dont quit on error as the following normally does
    install.pl index
    install.pl vacuum
    set -e # error exit on any command errors in following commands

# Create large files for parallel test
    rm -rf largeFileDir
    mkdir largeFileDir
    cat $1 $1 $1 $2 > largeFileDir/f1
    cat $1 $1 $1 $2 $2 > largeFileDir/f2
    cat $1 $1 $1 $2 $1 > largeFileDir/f3
    cat $2 > largeFileDir/f4

# Run an exotic test
    if [ -a etest.sh ]; then
	echo etest.sh exists
    else
	cp SRB2_0_0rel/test/autotest/etest.sh .
    fi
    etest.sh d complete largeFileDir demoResc demoLogResc tempResc2 
    etest.sh e complete largeFileDir demoResc demoLogResc tempResc2 
    rm -rf largeFileDir
}

function exoticTest2 {
    set +e # Dont quit on error as the following normally does
    install.pl index
    install.pl vacuum
    set -e # error exit on any command errors in following commands

# Make a directory with input-arg empty files and test with that
    rm -rf tmpdir
    mkdir tmpdir
    cd tmpdir
    set +x
    i=0
    while [ $i -lt $1 ]
      do
#     touch f$i
      echo "a" > f$i
      i=`expr $i + 1`
    done
    set -x
    cd ..
    if [ -a etest.sh ]; then
	echo etest.sh exists
    else
	cp SRB2_0_0rel/test/autotest/etest.sh .
    fi
    etest.sh a quicker tmpdir demoResc demoLogResc tempResc2
    etest.sh b quicker tmpdir demoResc demoLogResc tempResc2
    etest.sh c quicker tmpdir demoResc demoLogResc tempResc2
    etest.sh e complete tmpdir demoResc demoLogResc tempResc2
    rm -rf tmpdir
}

function exoticTest3 {
# Use the input argument as the test directory
    set +e # Dont quit on error as the following normally does
    install.pl index
    install.pl vacuum
    set -e # error exit on any command errors in following commands

    if [ -a etest.sh ]; then
	echo etest.sh exists
    else
	cp SRB2_0_0rel/test/autotest/etest.sh .
    fi
    etest.sh a quicker $1 demoResc demoLogResc tempResc2
    etest.sh b quicker $1 demoResc demoLogResc tempResc2
    etest.sh c quicker $1 demoResc demoLogResc tempResc2
    etest.sh e complete $1 demoResc demoLogResc tempResc2
    rm -rf tmpdir
}

function jargonTest {
    set -e # error exit on any command errors in following commands

# Add srbHost as localhost in the .MdasEnv file for Jargon
# (Scommands default to the local host)
    myHome=$HOME
    if [ $oldHome ]; then
	myHome=$oldHome  # if using testuser, need to still modify
                         # the real home .MdasEnv for Jargon
    fi
    echo "srbHost 'localhost'" >> $myHome/.srb/.MdasEnv

# Check for Mac OS X	
    os=`uname -s`
    gmake="gmake";
    if [ $os == "Darwin" ]; then
	gmake="make";
    fi

# Set the correct path to Java for SDSC Linux hosts
    myJava="java";
    if [ $os == "Linux" ]; then
	set +e
        uname -n | grep qa
        if [ "$?" -eq 0 ]; then
#           for SDSC hosts qa and qa-linux
            export JAVADIR=/usr/local/java/bin/
            myJava="/usr/local/java/bin/java";
        else
#           for other SDSC hosts
            export JAVADIR=/usr/local/apps/java/bin/
            myJava="/usr/local/apps/java/bin/java";
        fi
	set -e
    fi

    set -x # display commands
# gmake
    cd SRB2_0_0rel/jargon
    rm -f gmake.log
    $gmake notgsi > gmake.log

# and run the test
    cd bin
    rm -f test.log
    $myJava Test > test.log
    cd ../../..

# Remove srbHost=localhost in the .MdasEnv file
    cp $myHome/.srb/.MdasEnv MdasEnvTmp
    cat MdasEnvTmp | grep -v localhost >  $myHome/.srb/.MdasEnv
    rm -f MdasEnvTmp
}

function secureCommTest {
# Run a few simple tests on encrypt/compress and secure-comm                  
    set -e # error exit on any command errors in following commands           
    cat /etc/hosts /etc/passwd > testfile1
    Sinit
    Sput.pl testfile1
    mv testfile1 testfile1.orig
    Sget.pl testfile1
    diff testfile1 testfile1.orig
    Srm -f testfile1
    Sexit
    rm -f testfile1 testfile1.orig
}

function cleanUp {
    set +e  # continue even if errors after this

# stop the processes
    if [ $nonMcat ]; then
	echo "y" | SRB2_0_0rel/bin/killsrb
    else
	perl install.pl stop
    fi

# chmod a CVS directory so it can be removed
    chmod u+x SRBInstall/data/CVS

# Remove lots of stuff
    rm -rf pgsql
    rm -rf postgresql-7.4.3
    rm -f psqlodbc-07.03.0200.tar
    rm -f postgresql-7.4.3.tar
    rm -rf SRB2_0_0rel
    rm -rf Vault*
    rm -f install*.log
    rm -f cvsCheckout.log
    rm -f install.state install.pl install.conf
    rm -rf SRBInstall
    rm -f ftp.in foo etest.sh dif.log
    rm -f MdasEnv MdasAuth
    ls -lt date1.log date2.log
    rm -f date1.log date2.log
}

function newSrbUser {
# create a new SRB User and change env to run as it
    set -x 
    set +e # If user is already defined, this will error
    SRB2_0_0rel/utilities/admin-bin/Singestuser autotestUser TESTpw1 demo staff 'some address' '858 555-1234' 'test@foo.baz' '' ''
    mkdir .srb
    set -e # error exit on any command errors in following commands
    cwd=`pwd`
    cat ~/.srb/.MdasEnv | sed s/srbAdmin/autotestUser/g > .srb/.MdasEnv
    echo "srbHost 'localhost'" >> .srb/.MdasEnv
    echo TESTpw1 > .srb/.MdasAuth
    oldHome=$HOME
    echo "oldHome is $HOME"
    export HOME=$cwd
}

function oldSrbUser {
# switch back to the old srbUser (srbAdmin)
    export HOME=$oldHome
    echo "Home reset to $HOME"
}

function waitForRemoteResource {
# Wait for the other server to make the resource
    set +e
    SgetR tempResc2
    while [ "$?" -ne 0 ]; do
	sleep 30
	SgetR tempResc2
    done
    set -e
}

function checkSrbLog {
# Make sure nothing weird is in the SRB log
    set +e
    set +x
    srbLogFile="SRBInstall/data/srbLog";
    if [ $nonMcat ]; then
	srbLogFile="SRB2_0_0rel/data/srbLog";
    fi
    notice=`grep NOTICE $srbLogFile | grep -i error | wc -l `
    if [ $notice -ne 0 ]; then
        echo "NOTICE with 'error' error message in srbLog"
	exit 101
    fi
# check for WARN but ignore the NOTICE WARNING about gethostbyname taking
# too long (as that sometimes happens (on my Mac, at least).
    warn=`grep WARN $srbLogFile | grep -v gethostbyname | wc -l`
    if [ $warn -ne 0 ]; then
        echo "WARN error message in srbLog"
	exit 102
    fi
    fatal=`grep FATAL $srbLogFile | wc -l `
    if [ $fatal -ne 0 ]; then
        echo "FATAL error message in srbLog"
	exit 103
    fi
    set -ex
}

function buildNonMcat {
# Build a nonMcat Server and register a resource
#

#   These (non-mcat) parameters are assumed for now:
    nmMcatHost="zuri.sdsc.edu"  # Hardcode for now
    nmMyPw="1234abcd"           # Hardcode for now, matching autotest.conf
#   the resource used is also assumed and is: tempResc2

    set -x # display commands

# Checkout the current SRB source tree unless it already is here
    ls SRB2_0_0rel
    if [ "$?" -ne 0 ]; then
	rm -f cvsCheckout.log
	cvs checkout SRB2_0_0rel >& cvsCheckout.log
	if [ "$?" -ne 0 ]; then
	    exit 1
	fi
    fi

# Make the .MdasEnv /Auth files
    set +e
    mkdir ~/.srb
    ext=`date | sed 's/ /_/g'`
    mv ~/.srb/.MdasEnv ~/.srb/.MdasEnv.$ext
    mv ~/.srb/.MdasAuth ~/.srb/.MdasAuth.$ext
    echo "mdasCollectionName '/A/home/srbAdmin.demo'
mdasCollectionHome '/A/home/srbAdmin.demo'
mdasDomainName 'demo'
mdasDomainHome 'demo'
srbUser 'srbAdmin'
srbHost '$nmMcatHost'
defaultResource 'demoResc'
AUTH_SCHEME 'ENCRYPT1'" > ~/.srb/.MdasEnv
    echo "$nmMyPw" > ~/.srb/.MdasAuth
    set -e

# run the installServer.pl script
    cd SRB2_0_0rel

# Change some parameters in the script
    cat installServer.pl | sed s/RESOURCE_NAME=\"/RESOURCE_NAME=\"tempResc3/g | sed s/MCAT_HOST=\"/MCAT_HOST=\"$nmMcatHost/g > installServer2.pl

    set +e
    perl installServer2.pl
    while [ "$?" -ne 0 ]; do
	echo "installServer failed $?"
	sleep 30
	perl installServer2.pl
    done
    set -e

# Add our tempResc2 as a temporary resource (needs to be temp for Container test).
# The one built by installServer.pl (tempResc3) is 'permanent'.
    cwd=`pwd`
    hostName=`uname -n`
    vaultPath2="$cwd/Vault2/?USER.?DOMAIN/?SPLITPATH/?PATH?DATANAME.?RANDOM.?TIMESEC"
    ./utilities/admin-bin/Sregisterresource tempResc2 'unix file system' $hostName $vaultPath2 temporary


# Check for Mac OS X to adjust stack size
    os=`uname -s`
    if [ $os == "Darwin" ]; then
	ulimit -s 2000
    fi

    cd bin
    ./runsrb
}


function setDoit {
# set the doit array to the input argument
    i=0
    while [ $i -le $doitMax ]; do
	doit[$i]=$1
	let i=i+1
    done
}

function negateDoit {
# set the doit array to the input argument
    i=0
    while [ $i -le $doitMax ]; do
	if [ ${doit[$i]} == 1 ]; then
	    doit[$i]=0;
	else 
	    doit[$i]=1;
	fi
	let i=i+1
    done
}

function printDoit {
# set the doit array to the input argument
    i=0
    while [ $i -le $doitMax ]; do
	echo doit[$i] ${doit[$i]}
	let i=i+1
    done
}



# end fuctions ############################################

echo "autotest.sh command line:"
echo "autotest.sh" $* 

# By default, do all the steps:
setDoit 1
if [ $1 ]; then  # If there are arguments
    setDoit 0    # do only those requested
    for arg in $* 
      do
        if [ "$arg" == "cleanup" ]; then
	    doit[$doitMax]=1;
	fi
        if [ "$arg" == "testuser" ]; then
	    testuser=1;
	fi
        if [ "$arg" == "nm" ]; then
	    nonMcat=1;
	fi
        if [ "$arg" == "rr" ]; then
	    remoteResource=1;
	fi
        if [ "$arg" == "c1" ]; then
	    optionalConfigurationOne=1;
	fi
        if [ "$arg" == "c2" ]; then
	    optionalConfigurationTwo=1;
	fi
        if [ "$arg" == "c3" ]; then
	    optionalConfigurationThree=1;
	fi
        if [ "$arg" == "rb" -o "$arg" == "rebuild" ]; then
	    rebuild=1;
	    doit[0]=1;
	fi
        if [ "$arg" == "not" ]; then
	    negate=1;
	fi
        if [ "$arg" == "all" ]; then
	    setDoit 1
	fi
        # check for numeric args to set in doit array
        # (these first to avoid error on -ge check)
	if [ $arg == "0" -o $arg == "1" -o $arg == "2" \
 	  -o $arg == "3" -o $arg == "4" -o $arg == "5" \
 	  -o $arg == "6" -o $arg == "7" -o $arg == "8" \
 	  -o $arg == "9" -o $arg == "10" -o $arg == "11" \
 	  -o $arg == "12" ]; then
	    if [ "$arg" -ge "0" ]; then
		if [ $arg -lt $doitMax ]; then
		    doit[$arg]=1  # set each one listed on the command line
                              # (except cleanup)
		fi
	    fi
	fi 
    done
fi

# Some of the tests expect . in the path
PATH=$PATH:.

if [ $negate ]; then
    negateDoit
fi


set -x # display commands

ulimit -c unlimited # get a core dump if any process aborts


unset srbHost   # Make sure the srbHost environment variable is not set

date | tee date1.log

# Set GSI environment for finding dynamic libraries
if [ $optionalConfigurationTwo ]; then
    export  GLOBUS_LOCATION=/usr/local/apps/globus-3.2.1
    set +x
    . $GLOBUS_LOCATION/etc/globus-user-env.sh
    set -x
fi
# Set GSI environment for finding dynamic libraries
if [ $optionalConfigurationThree ]; then
    export  GLOBUS_LOCATION=/usr/local/apps/nmi-2.1
    set +x
    . $GLOBUS_LOCATION/etc/globus-user-env.sh
    set -x
fi

if [ ${doit[0]} == 1 ]; then
    if [ $nonMcat ]; then
	buildNonMcat  # build and run nonMcat Server
    else
	if [ $rebuild ]; then
	    rebuild
	else
	    buildAll  # build and run everything
	fi
    fi
fi

checkSrbLog

if [ $testuser ]; then
    newSrbUser # create and run as a new user
    checkSrbLog
fi

# Adjust path to first find the new utilities
PATH=./SRB2_0_0rel/utilities/bin:$PATH

# Run the tests:
if [ ${doit[1]} == 1 ]; then
    simpleTests         # run a few simple tests
    checkSrbLog
fi

if [ ${doit[2]} == 1 ]; then
    commandTest         # run the command test script
    checkSrbLog
fi
if [ ${doit[3]} == 1 ]; then
    testSuite           # run the testsuite
    checkSrbLog
fi
if [ ${doit[4]} == 1 ]; then
    parallelCommandTest # run the parallel command test script
    checkSrbLog
fi
if [ ${doit[5]} == 1 ]; then
    if [ $testuser ]; then
	oldSrbUser   # switch back to the old user (srbAdmin)
    fi
    makeMoreResources   # needed for following tests
    if [ $testuser ]; then
	newSrbUser   # back to the new user
    fi
    checkSrbLog
fi
if [ $remoteResource ]; then
    waitForRemoteResource # Wait for the other server to make the resource
    checkSrbLog
fi
if [ ${doit[6]} == 1 ]; then
# fast version of exotic test: small directory and small files
    exoticTest0 10   # exotic test, with small files and a small directory
    checkSrbLog
fi
if [ ${doit[7]} == 1 ]; then
    exoticTest1 postgresql-7.4.3.tar  psqlodbc-07.03.0200.tar   # exotic test 1
    checkSrbLog
fi
if [ ${doit[8]} == 1 ]; then
    exoticTest2 50    # exotic test 2 with 50 (for now) empty files
#                       (later this will be 5000?)
    checkSrbLog
fi
if [ ${doit[9]} == 1 ]; then
# Avoid an empty directory (which are not created in Sget -b op)
    ls > postgresql-7.4.3/src/interfaces/odbc/psqlodbc-07.03.0200/foo
    exoticTest3 postgresql-7.4.3  # test with this large multilevel tree
    checkSrbLog
fi

if [ ${doit[10]} == 1 ]; then
    jargonTest          # build jargon and run its test
    checkSrbLog
fi

if [ ${doit[11]} == 1 ]; then
    if [ $optionalConfigurationOne ]; then
	secureCommTest
	checkSrbLog
    else
	echo "Skipping secureCommTest as not configured (and so would fail)"
    fi
fi

# Record (for now) done time
date | tee date2.log

if [ ${doit[12]} == 1 ]; then
    cleanUp  # Remove everything created
fi

echo "autotest.sh command line was:"
echo "autotest.sh" $* 

echo "Success"
exit 0
