#!/usr/local/bin/perl5.8
# On SDSC Solaris systems, change the above line to this:
#!/usr/local/bin/perl5.8

# This script performs all the steps required to do a basic full
# Oracle MCAT-enabled SRB installation.  It assumed that an Oracle
# instance for the MCAT has been created.  It performs an SRB
# configure and gmake, installs the MCAT tables, updates configuration
# files, brings up the MCAT-enabled SRB server, creates a local
# resource (providing access to local disk space), and configures the
# current user with access; the manual steps described in
# README.MCAT.INSTALL.

# This script sets up everything to run the servers and the SRB client
# programs from the current user login.

# Edit the defines a few lines below here for your installation, and
# then run "./install.ora.pl". It will keep track of which steps have
# been completed and continue where it left off, if something goes
# wrong.

# First you download the SRB from:
# http://www.npaci.edu/DICE/SRB/tarfiles/main.html

$startDir=`pwd`;
chop($startDir);

# ***********************************************************************

# These are the important settings that you must specify for the installation.
# You must fill in these with the files you have and options you want.
# If you prefer, you can put these settings into a separate file 
# "install.conf" which will be included (perl "require") if it exists;
# settings in "install.conf" will override those defined here:
$SRB_FILE="SRB3.0.0rel_pgp.tar";  # The SRB tar file
$SRB_FILE_ALREADY_DECRYPTED=0;    # Set this to 1 if the SRB Tar file is
                                  # already  decrypted (e.g., you did it 
                                  # manually).
$SRB_DIR="SRB3_0_0";              # Subdirectory that untar'ing the 
                                  # SRB_FILE creates.
$ORACLE_HOME="/usr/local/apps/oracle/product/9.2.0";  # where Oracle library can be located
$DB_INSTANCE="test";              # Oracle Instance already created (usually,
                                  # by your Oracle DB Administrator);
                                  # this will be used as the MCAT database.
$DB_SCHEMA="srb";                 # Oracle Instance schema user
$DB_SCHEMA_PASSWD="";             # Oracle Instance schema user password
$YOUR_ADMIN_NAME="srbAdmin";      # Change this to the name for the srb-admin 
                                  # login acct that you would like to use (this
                                  # can be what ever name you want).
$YOUR_ADMIN_PW="";                # Change this to a password you want to use
                                  # for this admin account (this script will
                                  # also change the builtin admin account 
                                  # password to this).  Later, 
                                  # for improved security, you should change
                                  # this value in this script file.
$YOUR_DOMAIN="demo";              # Change this to the SRB domain name you 
                                  # would like to use.
$YOUR_ZONE="A";                   # Change this to the local zone name you
                                  # would like to use.
$RESOURCE_NAME="demoResc";        # Name of the local SRB resource name that
                                  # this SRB server will create and use
$RESOURCE_DIR="$startDir/Vault";  # Subdirectory that will be the resource
                                    # (i.e., the real unix directory that 
                                    # will hold the SRB data files).
$SAFE_MODE="0";                   # Flag for how careful to be with files
                                  # in your home directory: ~/.srb/.MdasEnv,
                                  # ~/.srb/.MdasAuth, and ~/.odbc.ini.

                                  # If 0: (normal case) move existing files to 
                                  # name.old.'datestring', for example ~/.srb/
                                  # .MdasEnv.old.Wed_Sep_24_11:19:17_PDT_2003

                                  # If 1: don't overwrite existing files;
                                  #    If an old one exists and a new one 
                                  #    is needed, quit.

$IP_ADDRESS_LOCALHOST="";         # Normally, LEAVE THIS UNSET!
                                  # You may need to use this on Mac OS X 
                                  # systems but normally don't on others. 
                                  # Only set this if
                                  # the DNS-type commands will not
                                  # work for you.  That is, if you
                                  # need to use the IP address of your
                                  # host instead of the script running uname
                                  # and host (see setHostVars).
                                  # If you do set this, use the IP number
                                  # (for example "192.168.1.103").

# If you do NOT want to build/use the SRB/MCAT java admin tool, then
# change the following line to null.  If you need to specify the location
# of the Java JDK then add " --enable-jdkhome=<path>" to it.  To later
# run the tool, cd to .../MCAT/java and run "gmake run", which will build
# and then start the tool.
$SRB_CONFIGURE_OPTIONS_JAVA="";
# Define the following if you want to create remote zones:
$REMOTE_ZONE="";                  # Remote zone name
$REMOTE_LOCATION="";              # Name to use for the remote Location
$REMOTE_HOST_ADDR="";             # Full DNS name of remote host
$REMOTE_DOMAIN="";                # Remote domain zame
$REMOTE_ADMIN_NAME="";            # Remote SRB admin name (@REMOTE_DOMAIN)
$REMOTE_ADMIN_PW="";              # Remote SRB admin name (@REMOTE_DOMAIN)
                                  # This PW is arbitrary, as the real password
                                  # is kept at the remote domain.  Make this
                                  # long and complicated for added safety.

# ***********************************************************************

# include install.conf if it exists, defines like those above will
# override those set above.
if (-e "install.conf") {
    require "install.conf";
}

# You can change this if you like:
$SRB_INSTALL_DIR="$startDir/SRBInstall";


# You can extend the following SRB configure line with other options
# if you like, but it needs to include the currently defined ones:
$SRB_CONFIGURE_OPTIONS="--enable-installdir=$SRB_INSTALL_DIR --enable-oraver=920_32bit --enable-oraconn=TNS --enable-oramcat --enable-mcatver=20 --enable-orahome=$ORACLE_HOME  --enable-oralobj";


# You can change the name of this state file if you like.
# If you want to redo the installation steps, you can remove the stateFile.
$stateFile="install.state";
$state=0;     # Major state/steps: 
              #  A - build srb and srb-mcat
              #  B - create and test the MCAT database
              #  C - create local settings, user config, and start SRB
$subState=0;  # Substep within the major state, 1 thru n

$fullStateFile = "$startDir/$stateFile";

$thisOS=`uname -s`;
chop($thisOS);
$gmake="gmake";
$psOptions="-el";

if (!$SRB_FILE) {
    usage();
    die("SRB_FILE not set");
}

if (!$YOUR_ADMIN_PW) {
    usage();
    die("YOUR_ADMIN_PW (admin password) is not set");
}

if (!$DB_SCHEMA_PASSWD) {
    usage();
    die("DB_SCHEMA_PASSWD (schema password) is not set");
}


setHostVars(); # set a few host-specific variables

$srbDone=0;
readState();

($arg1)=@ARGV;

if ($arg1 eq "ps") {
    $srb=`ps $psOptions | grep srb`;
    print "Running srb server processes:\n";
    print $srb;
    die("Done listing processes");
}


if ($state eq "A") {
    buildSrb();
}

if ($state eq "B") {
    installMcatDB();
}

if ($state eq "C") {
    createLocalSettings();
}

if ($state eq "D") {
    doZones();
}

print "To use the SRB Scommands set your path to include the binaries:\n";
print "set path=($startDir/$SRB_DIR/utilities/bin \$path)\n";
print "Then Sinit, Sls, Sput, Sexit, etc should work.\n";
print "All done\n";


# set up $hostName and $hostFullNetName and $hostFullNetAddr
sub setHostVars() {
    if ($IP_ADDRESS_LOCALHOST) {
	$hostName = $IP_ADDRESS_LOCALHOST;
	$hostFullNetName = $IP_ADDRESS_LOCALHOST;
	$hostFullNetAddr = $IP_ADDRESS_LOCALHOST;
    }
    else {
	$hostName = `uname -n`; # on my Mac, this gives for example: dhcp-mac-016.sdsc.edu
	chop($hostName);
	if ($thisOS eq "SunOS") {
	    $tmp=`nslookup $hostName`;
	    $i = index($tmp,"Name: ");
	    $j = index($tmp, "Address:");
	    $hostFullNetName = substr($tmp,$i+6,$j-$i); # e.g. zuri.sdsc.edu
	    chop($hostFullNetName);
	    chop($hostFullNetName);
	    $i = rindex($hostFullNetName, " ");
	    $hostFullNetName = substr($hostFullNetName, $i+1);
	    $i = rindex($tmp," ");
	    $hostFullNetAddr = substr($tmp,$i+1);  # e.g. 132.249.32.192
	    chop($hostFullNetAddr);
	    chop($hostFullNetAddr);
	}
	else {
	    $tmp=`host $hostName`;  # returns for example: zuri.sdsc.edu has address 132.249.32.192
	    $i = index($tmp," ");
	    $hostFullNetName = substr($tmp,0,$i); # zuri.sdsc.edu
	    $i = rindex($tmp," ");
	    $hostFullNetAddr = substr($tmp,$i+1);  # 132.249.32.192
	    chop($hostFullNetAddr);
	}
    }
    print "This host is $hostName\n";
    print "This host full network name is $hostFullNetName\n";
    print "This host full network address is $hostFullNetAddr\n";

    $homeDir=$ENV{'HOME'};
    print "Your home directory is $homeDir\n";
}

sub createLocalSettings() {
    #  Extract the builtin password for "srb"  (so 
    #        nothing confidential is in this script).
    $keyLine=`grep "MDAS_AU_AUTH_KEY values (1" $startDir/$SRB_DIR/MCAT/data/catalog.install.ora`;
    $left=index($keyLine,"'");
    $right=rindex($keyLine,"'");
    $pw=substr($keyLine,$left+1,$right-$left-1);
#   print "The bootstrap srb user password is $pw \n";

    # set environment to use predefined admin acct (created as part of "E")
    $ENV{'srbUser'}="srb";
    $ENV{'srbAuth'}=$pw;
    $ENV{'mdasDomainName'}="sdsc";

    $bypassExitCheck=0;
    if ($thisOS eq "Darwin") {
	$bypassExitCheck=1;  # on Mac, we get weird exit codes somehow
    }

    print "chdir'ing to: $SRB_DIR/MCAT/bin\n";
    chdir "$SRB_DIR/MCAT/bin" || die "Can't chdir to $SRB_DIR/MCAT/bin";
    if ($subState eq "0") {
	runCmd(1,"./ingestToken Domain $YOUR_DOMAIN gen-lvl4");
#          (bypass the exit check on this because if the domain already
#           exists, this will fail.)
	$subState++;
	saveState();
    }
    if ($subState eq "1") {
	runCmd($bypassExitCheck,"./ingestUser $YOUR_ADMIN_NAME $YOUR_ADMIN_PW $YOUR_DOMAIN sysadmin '' '' ''");
	$subState++;
	saveState();
    }

    if ($subState eq "2") {
	runCmd($bypassExitCheck,"./ingestLocation '$hostName' '$hostFullNetName:NULL.NULL' 'level4' $YOUR_ADMIN_NAME $YOUR_DOMAIN");
	$subState++;
	saveState();
    }

    if ($subState eq "3") {
#       change the builtin srb@sdsc user to use your admin password instead
#       of the builtin one.
	runCmd($bypassExitCheck,"./modifyUser changePassword srb sdsc $YOUR_ADMIN_PW");
	$subState++;
	saveState();
    }


    # create the .srb directory and env files
    if ($subState eq "4") {
 	if (-e "$homeDir/.srb") {
 	    if (-e "$homeDir/.srb/.MdasEnv") {
		if ($SAFE_MODE eq "1") {
		    die("SAFE_MODE is 1 and you already have a ~/.srb/.MdasEnv file");
		}
		$dateStr=`date | sed 's/ /_/g'`;
		chop($dateStr);
 		runCmdNoLog(0,"mv $homeDir/.srb/.MdasEnv $homeDir/.srb/.MdasEnv.old.$dateStr");
 	    }
 	}
 	else {
	    print "mkdir'ing: $homeDir/.srb\n";
 	    mkdir("$homeDir/.srb");
 	}
	print "Creating $homeDir/.srb/.MdasEnv file\n";
	writeFile("$homeDir/.srb/.MdasEnv", "mdasCollectionName \'/$YOUR_ZONE/home/$YOUR_ADMIN_NAME.$YOUR_DOMAIN\'\nmdasCollectionHome \'/$YOUR_ZONE/home/$YOUR_ADMIN_NAME.$YOUR_DOMAIN\'\nmdasDomainName \'$YOUR_DOMAIN\'\nmdasDomainHome \'$YOUR_DOMAIN\'\nsrbUser \'$YOUR_ADMIN_NAME\'\nsrbHost \'$hostFullNetName\'\n#srbPort \'5544\'\ndefaultResource \'$RESOURCE_NAME\'\n#AUTH_SCHEME \'PASSWD_AUTH\'\n#AUTH_SCHEME \'GSI_AUTH\'\nAUTH_SCHEME \'ENCRYPT1\'\n");

	if (-e "$homeDir/.srb/.MdasAuth") {
	    if ($SAFE_MODE eq "1") {
		die("SAFE_MODE is 1 and you already have a ~/.srb/.MdasAuth file");
	    }
	    $dateStr=`date | sed 's/ /_/g'`;
	    chop($dateStr);
	    runCmdNoLog(0,"mv $homeDir/.srb/.MdasAuth $homeDir/.srb/.MdasAuth.old.$dateStr");
	}
	print "Creating $homeDir/.srb/.MdasAuth file\n";
	writeFile("$homeDir/.srb/.MdasAuth", $YOUR_ADMIN_PW);

	$subState++;
	saveState();
    }
    if ($subState eq "5") {
#        Switch to the new user at this point.
        $ENV{'srbUser'}=$YOUR_ADMIN_NAME;
	$ENV{'srbAuth'}=$YOUR_ADMIN_PW;
	$ENV{'mdasDomainName'}=$YOUR_DOMAIN;

	print "chdir'ing to: $SRB_INSTALL_DIR/bin\n";
	chdir "$SRB_INSTALL_DIR/bin";
	runCmdNoLog(0,"./runsrb");
	print "sleeping a couple seconds\n";
	sleep 2;
	runCmdNoLog(0, "ps $psOptions | grep srb");
	print "If the srb server is running OK, you should see srbMaster and srbServer here:\n";
	print $cmdStdout;
	print "chdir'ing to: $startDir/$SRB_DIR/MCAT/bin\n";
	chdir "$startDir/$SRB_DIR/MCAT/bin" || die "Can't chdir to $SRB_DIR/MCAT/bin";
	$subState++;
	saveState();
    }

# Do this last so the resource will be owned by the new admin account
# (especially important for Zones).
    if ($subState eq "6") {
#        Be sure we have switched to the new user at this point.
        $ENV{'srbUser'}=$YOUR_ADMIN_NAME;
	$ENV{'srbAuth'}=$YOUR_ADMIN_PW;
	$ENV{'mdasDomainName'}=$YOUR_DOMAIN;
	runCmd($bypassExitCheck,"./ingestResource '$RESOURCE_NAME' 'unix file system' '$hostName' '$RESOURCE_DIR/?USER.?DOMAIN/?SPLITPATH/?PATH?DATANAME.?RANDOM.?TIMESEC' permanent 0");
	$subState++;
	saveState();
    }

    $state++;
    $subState=0;
    saveStateQuiet();
    chdir "$startDir" || die "Can't chdir to $startDir";
}


sub doZones() {
#  Set up Zones (new in SRB 3.0.0)

#   Switch from the ENV variables to the new user (via the .MdasEnv).
    $ENV{'srbUser'}='';
    $ENV{'srbAuth'}='';
    $ENV{'mdasDomainName'}='';

    print "chdir'ing to: $startDir/$SRB_DIR\n";
    chdir "$startDir/$SRB_DIR" || die "Can't chdir to $startDir/$SRB_DIR";

# Change the local zone name
    if ($subState eq "0") {
	runCmd(0, "./utilities/bin/Szone -C demozone $YOUR_ZONE");
	$subState++;
	saveState();
    }

# Add the location to the zone, and also change the comment
# (this script uses hostName as the Location name)
    $now=`date`;
    chop($now);
    if ($subState eq "1") {
	runCmd(0, "./utilities/bin/Szone -M $YOUR_ZONE $hostName '' '" . "$YOUR_ADMIN_NAME" . "@" . "$YOUR_DOMAIN" . "' '' 'Created $now'");
	$subState++;
	saveState();
    }


    if (!$REMOTE_ZONE) {
	print "REMOTE_ZONE not defined, skipping remote zone installation\n";
	$state++;
	$subState=0;
	saveStateQuiet();
	chdir "$startDir" || die "Can't chdir to $startDir";
	return;
    }

    chdir "MCAT/bin" || die "Can't chdir to MCAT/bin";
# Create the remote Domain
    if ($subState eq "2") {
	runCmd(1,"./ingestToken Domain $REMOTE_DOMAIN gen-lvl4");
#          (bypass the exit check on this because if the domain already
#           exists, this will fail.)
	$subState++;
	saveState();
    }

# Create the remote zone user
    if ($subState eq "3") {
	runCmd($bypassExitCheck,"./ingestUser $REMOTE_ADMIN_NAME $REMOTE_ADMIN_PW $REMOTE_DOMAIN sysadmin '' '' ''");
	$subState++;
	saveState();
    }

# Create the remote Location
    if ($subState eq "4") {
	runCmd($bypassExitCheck,"./ingestLocation '$REMOTE_LOCATION' '$REMOTE_HOST_ADDR:NULL.NULL' 'level4' $REMOTE_ADMIN_NAME $REMOTE_DOMAIN");
	$subState++;
	saveState();
    }

# Create the remote Zone
    if ($subState eq "5") {
	runCmd(0, "../../utilities/bin/Szone -r $REMOTE_ZONE $REMOTE_LOCATION 0 $REMOTE_ADMIN_NAME" . "@" . "$REMOTE_DOMAIN '' ''");
	$subState++;
	saveState();
    }

# Change the remote admin user to the new Zone
    if ($subState eq "6") {
	runCmd(0, "../../utilities/bin/Szone -U $REMOTE_ZONE $REMOTE_ADMIN_NAME $REMOTE_DOMAIN");
	$subState++;
	saveState();
    }

    $state++;
    $subState=0;
    saveStateQuiet();
    chdir "$startDir" || die "Can't chdir to $startDir";
}


sub installMcatDB() {

#   Run MCAT Oracle table insert script
    if ($subState eq "0") {
	print "chdir'ing to: $SRB_DIR/MCAT/data\n";
	chdir "$SRB_DIR/MCAT/data" || die "Can't chdir to $SRB_DIR/MCAT/data";
	unlink("myinstall.results.ora");
	if ($thisOS eq "SunOS") {
	    runCmdNoLog(0,"$ORACLE_HOME/bin/sqlplus $DB_SCHEMA/$DB_SCHEMA_PASSWD\@$DB_INSTANCE < catalog.install.ora > myinstall.results.ora 2>&1")
	}
	else {
	    runCmdNoLog(0,"$ORACLE_HOME/bin/sqlplus $DB_SCHEMA/$DB_SCHEMA_PASSWD\@$DB_INSTANCE  < catalog.install.ora >& myinstall.results.ora");
	}
	print "It is probably OK, but you might compare myinstall.results.ora with install.results.ora to check differences\n";
	$subState++;
	saveState();
	print "chdir'ing to: $startDir\n";
	chdir "$startDir" || die "Can't chdir to $startDir";
    }

    # B2
    if ($subState eq "1") {
        # dummy to correct missing step drl 6/16/04
        $subState++;
        saveState();
    }

    if ($subState >= 2 && $subState <= 4) {
#       Need to run commands from MCAT/bin so that they can find ../data 
#       (Or could set srbData environment variable.)
	print "chdir'ing to: $SRB_DIR/MCAT/bin\n"; 
	chdir "$SRB_DIR/MCAT/bin" || die "Can't chdir to $SRB_DIR/MCAT/bin";
    }

#   Test commuincation to Oracle 
    if ($subState eq "2") {
        print "running: ./test_srb_mdas_auth a b c\n";
	$testResult=`./test_srb_mdas_auth a b c`;
	if (index($testResult,"-3206") eq -1) {
	    die("failed to communicate to Oracle");
	}
	$subState++;
	saveState();
    }

#   Do full MCAT test
    if ($subState eq "3") {
	$oldPath=$ENV{'PATH'};
	$ENV{'PATH'}="$startDir/$SRB_DIR/MCAT/bin:$oldPath";
	$bypassExitCheck=0;
	if ($thisOS eq "Darwin") {
	    $bypassExitCheck=1;  # on Mac, we get weird exit codes somehow
	}
	if ($thisOS eq "SunOS") {
	    $bypassExitCheck=1;  # also on Solaris.
	}
	runCmd($bypassExitCheck,"../data/test.catalog");
	$subState++;
	saveState();
    }

#   Try to check the results
    if ($subState eq "4") {
	runCmd(1,"diff $startDir/installB4.log ../data/test.results.ora");
	$wc=`cat $startDir/installB5.log | wc -l`;
	chop($wc);
	print "There are $wc lines of diffences.\n";
	if ($wc != 0) {
	    if ($wc < 40) {
		print "There are sometimes small differences like these\n";
		print "due to changes in Oracle versions.\n";
		print "This is probably OK, but you might examine these manually.\n";
	    }
	    else {
		die "Too many differences in test results; please check it.";
	    }
	}
        $subState++;
        saveState();
    }


    $state++;
    $subState=0;
    chdir "$startDir" || die "Can't chdir to $startDir";
}


sub buildSrb() {
    if ($subState eq "0") {
	if (-e "$startDir/$SRB_DIR") {
	    print "Skipping decrypting of tar file and unpacking as $SRB_DIR already exists\n";
	    $subState++; # Also skip tar xf below
	}
	else {
	    if ($SRB_FILE_ALREADY_DECRYPTED eq "0") {
		print "After it pauses, enter the SRB tar file password\n";
		if (index($SRB_FILE,"bf.tar") gt 0) {
		    runCmd(0,"openssl enc -d -bf-cbc -in $SRB_FILE -out $SRB_FILE.decrypted.tar");
		}
		else {
		    runCmd(0,"pgp -d $SRB_FILE -o $SRB_FILE.decrypted.tar");
		}
	    }
	}
	$subState++;
	saveState();
    }

    if ($subState eq "1") {
	if ($SRB_FILE_ALREADY_DECRYPTED eq "0") {
	    runCmd(0,"tar xf $SRB_FILE.decrypted.tar");
	    unlink "$SRB_FILE.decrypted.tar";
	}
	else {
	    runCmd(0,"tar xf $SRB_FILE");
	}
	$subState++;
	saveState();
    }
    
    print "chdir'ing to: $SRB_DIR\n";
    chdir "$SRB_DIR" || die "Can't chdir to $SRB_DIR";

    if ($subState eq "2") {
	runCmd(0,"./configure $SRB_CONFIGURE_OPTIONS $SRB_CONFIGURE_OPTIONS_JAVA");
	$subState++;
	saveState();
    }

    if ($subState eq "3") {
	runCmd(0,"gmake");
	$subState++;
	saveState();
    }

    if ($subState eq "4") {
	print "chdir'ing to: MCAT\n";
	chdir "MCAT" || die "Can't chdir to MCAT";
	runCmd(0,"gmake");
	$subState++;
	saveState();
	chdir ".." || die "Can't chdir to ..";
    }

    # create the MdasConfig file, set up for Postgres 
    if ($subState eq "5") {
	$userName = `whoami`;
	runCmdNoLog(0,"mv $startDir/$SRB_DIR/data/MdasConfig $startDir/$SRB_DIR/data/MdasConfig.old");
	print "Creating $startDir/$SRB_DIR/data/MdasConfig file\n";
	writeFile("$startDir/$SRB_DIR/data/MdasConfig", "MDASDBTYPE       oracle\nMDASSCHEMENAME       $DB_SCHEMA.\nMDASDBNAME       $DB_INSTANCE\nMDASINSERTSFILE  $startDir/$SRB_DIR/data/mdas_inserts\nMETADATA_FKREL_FILE metadata.fkrel\nDB2PASSWORD        $DB_SCHEMA_PASSWD\nDB2USER           $DB_SCHEMA\nDB2LOGFILE       $startDir/$SRB_DIR/data/db2logfile\nDBHOME        $ORACLE_HOME\n");
	$subState++;
        saveState();
    }

    # change host name in mcatHost file
    if ($subState eq "6") {
      runCmdNoLog(0, "cat $startDir/$SRB_DIR/data/mcatHost | sed s/srb.sdsc.edu/$hostFullNetName/g > $startDir/$SRB_DIR/data/mcatHost2");
        `cp $startDir/$SRB_DIR/data/mcatHost2 $startDir/$SRB_DIR/data/mcatHost`;
	unlink "$startDir/$SRB_DIR/data/mcatHost2";
	$subState++;
	saveState();
    }

    # add localhost to hostConfig (seems to be needed on some hosts)
    if ($subState eq "7") {
	runCmdNoLog(0, "echo localhost $hostFullNetName >> $startDir/$SRB_DIR/data/hostConfig");
	$subState++;
	saveState();
    }

    if ($subState eq "8") {
	runCmd(0,"gmake install");
	$subState++;
	saveState();
    }

    if ($thisOS eq "Darwin") {
        # Need different version of killsrb on Macs
	if ($subState eq "9") {
	    runCmdNoLog(0,"cp $startDir/$SRB_DIR/bin/killsrbMac $SRB_INSTALL_DIR/bin/killsrb");
	    $subState++;
	    saveState();
	}
    }

    $state++;
    $subState=0;
    chdir "$startDir" || die "Can't chdir to $startDir";
}

sub usage() {
    printf("usage: first edit a few defines in this file and then run it\n");
}

# run a command, and log its output to a file.
# if option is 0 (normal), check the exit code 
sub runCmd {
    my($option,$cmd) = @_;
    $workingState=$subState+1; # State/step we are currently working on
    $fullCmd = "$cmd > $startDir/install$state$workingState.log";
    print "running: $fullCmd \n";
    `$fullCmd`;
    $cmdStat=$?;
    if ($option == 0) {
	if ($cmdStat!=0) {
	    print "The following command failed:";
	    print "$fullCmd \n";
	    print "Exit code= $cmdStat \n";
	    die("command failed");
	}
    }
}

# run a command (without a log)
# if option is 0 (normal), check the exit code 
sub runCmdNoLog {
    my($option, $cmd) = @_;
    print "running: $cmd \n";
    $cmdStdout=`$cmd`;
    $cmdStat=$?;
    if ($option == 0) {
	if ($cmdStat!=0) {
	    print "The following command failed:";
	    print "$cmd \n";
	    print "Exit code= $cmdStat \n";
	    die("command failed");
	}
    }
}

sub saveState {
    open(F, ">$fullStateFile");
    $_ = $state;
    print F;
    $_ = $subState;
    print F;
    close(F);
    print "Step $state $subState completed.\n";
}

sub saveStateQuiet {
    open(F, ">$fullStateFile");
    $_ = $state;
    print F;
    $_ = $subState;
    print F;
    close(F);
}

sub readState {
    if (open(F, "<$fullStateFile")) {
	read(F, $state, 1);
	read(F,$subState,1);
	close(F);
    }
    else {
	$state="A";
	$subState=0;
    }
    if ($state eq "E" or $state eq "F") {
	$srbDone=1;
    }
}

sub writeFile {
    my($file, $text) = @_;
    open(F, ">$file");
    $_ = $text;
    print F;
    close F;
}
