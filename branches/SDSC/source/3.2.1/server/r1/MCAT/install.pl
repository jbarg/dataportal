#!/usr/bin/perl
# On SDSC Solaris systems, change the above line to use this path:
# #!/usr/local/bin/perl5.8

$Version="install.pl version 3.x, last updated July 1, 2004";

# This script performs all the steps required to do a basic full
# Postgres MCAT-enabled SRB installation, starting with the
# distribution files, on Linux, Solaris, or Mac OS X (or a subset, see
# SUBSET paragraph). It configures 
# and builds Postgres, Postgres-ODBC, and SRB, initializes the database,
# ingests the MCAT tables, updates configuration files, brings up the
# system, provides access to local disk space as an SRB resource, and
# configures the current user with access; the manual steps described
# in README.MCAT.INSTALL.  This can be used directly by those wanting
# to install SRB for basic testing and operation, and can illustrate
# what is needed for those doing more elaborate installations.

# This script sets everything up to run the servers and the SRB client
# programs from the current user login.

# Edit the defines a few lines below here for your installation, and
# then run "./install.pl" (or perhaps "perl install.pl").  It will keep
# track of which steps have been completed and continue where it left
# off, if something goes wrong.

# First you download the SRB from:
# http://www.npaci.edu/DICE/SRB/tarfiles/main.html , Postgres from
# http://www.postgresql.org/ , and Postgres odbc from
# http://gborg.postgresql.org/ for example:
# ftp://ftp.postgresql.org/pub/odbc/versions/src/psqlodbc-07.03.0100.tar.gz

# You should move this script and the three .tar[.gz] files to a new
# directory.  By default, this script will install each package under
# this current directory, as well as the SRB storage area (Vault).  If
# you already have unpacked the SRB file, just put this script and the
# .tar[.gz] files above where you unpacked SRB (since this is kept
# in MCAT, that would be cp ../..).

# You can also use this script to start and stop the postgres and SRB
# servers, via ./install.pl start and ./install.pl stop.  There is
# also a ./install.pl ps that lists the postgres and srb processes
# (servers).

# While building postgresql you may see an error that Bison as install
# on your system is too old, it needs 1.875 or later.  In our
# experience, this can be safely ignored.  Postgresql seems to work
# fine even with the older Bison, at least for the light testing that
# we typically do.

# SUBSETs of the full installation can be selected, with the following
# options:
#    1) Use an existing Postgres installation.
#       You need to modify the $postgresInstallDir line below to point to
#       the existing postgres installation and set $SubsetMode (below) 
#       to 1.  This script will then skip the steps to build
#       postgres and postgres-odbc and to initialize postgres and 
#       will use the existing one.  It will still attempt to create 
#       the MCAT database so you will need to either use option 2 or 3 below,
#       or change the name of the MCAT DB ($DB_NAME), or drop the old one 
#       if you no longer want it (run the postgres command "dropdb name").
#
#       When in this mode, only the srbservers (not postgres) are started
#       or stopped via the start and stop commands.
#
#       Be sure that your SRB server host is allowed to connect to the
#       postgresql server.  You may need a line like this (with your
#       host IP address) in the data/pg_hba.conf file:
#       host    MCAT  all 132.249.32.192  255.255.255.255   trust
#
#    2) Do everything as in mode 1 but also skip the initialization of
#       the MCAT database.  In this mode, $DB_NAME must match an existing
#       MCAT database name.  More commonly, you'll want to use 3.
#
#       Set $postgresInstallDir line to an existing installation and
#       set $SubsetMode to 2.
#
#    3) Like 2 but also skip ingesting new domains, users, etc.
#       
#       Set $postgresInstallDir line to an existing installation and
#       set $SubsetMode to 3.


$startDir=`pwd`;
chomp($startDir);

# ***********************************************************************

# These are the important settings that you must specify for the installation.
# You must fill in these with the files you have and options you want.
# If you prefer, you can put these settings into a separate file 
# "install.conf" which will be included (perl "require") if it exists;
# settings in "install.conf" will override those defined here:
$SRB_FILE="SRB3.2rele_pgp.tar"; # The SRB tar file
                                  # Note: if you have a srb source tree 
                                  # (e.g. via cvs checkout) in $SRB_DIR 
                                  # (below), then you don't need a SRB_FILE.
$SRB_FILE_ALREADY_DECRYPTED=0;    # Set this to 1 if the SRB Tar file is
                                  # already  decrypted (e.g., you did it 
                                  # manually).
$SRB_DIR="SRB3_2";                # Subdirectory that untar'ing the 
                                  # SRB_FILE creates.
$POSTGRES_FILE="postgresql-7.4.1.tar.gz"; # the postgres release file
$ODBC_FILE="psqlodbc-07.03.0200.tar.gz";  # the odbc release file
$DB_NAME="MCAT";                  # Name of the Postgres database to create 
                                  # and use as the MCAT database.
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
       # Set this to "127.0.0.1", the loopback address, if you want to
       # build and run without network connectivity, for example on a
       # laptop that has no network available.  With this setting,
       # your SRB will not need the network, nor will it be able to
       # use the network.  That is, it can be used for testing or
       # demos but not normal operation.

       # You may need to use this on Mac OS X systems but normally
       # don't on others.  Only set this if the DNS-type commands will
       # not work for you.  That is, if you need to use the IP address
       # of your host instead of having this script run uname and host
       # (see setHostVars).  If you do set this, use either
       # "127.0.0.1" (see above) or use what is normally the actual IP
       # number (for example "192.168.1.103").

# If you do NOT want to build/use the SRB/MCAT java admin tool, then
# change the following line to null.  If you need to specify the location
# of the Java JDK then add " --enable-jdkhome=<path>" to it.  To later
# run the tool, cd to .../MCAT/java and run "gmake run", which will build
# and then start the tool.
#$SRB_CONFIGURE_OPTIONS_JAVA="--enable-javagui";

# Starting with SRB 3.1, the above --enable-javagui is no longer needed
# as the Java Admin tool uses Jargon now.  If you want this script to
# build Jargon and the Java Admin tool, set the flag below to non-zero.
$DO_JAVA=0;                       # By default, don't build Java stuff
   # Since the jar file is included, you don't need to build the Java stuff.
   # You don't even need the JDK installed (need java not javac). 
   # Just cd to MCAT/java and run 'java -jar mcatAdmin.jar'.

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


# You can change this if you like:
$SRB_INSTALL_DIR="$startDir/SRBInstall";

# You can also change this postgresInstall Directory:
$postgresInstallDir = "$startDir/pgsql";

#$SubsetMode=0;   # Set this to 1 to use an existing postgres
                  # Set this to 2 to do 1 and use an existing local MCAT db.
                  # Set this to 3 to do 1 and 2 and skip installing MCAT items.

# ***********************************************************************

# include install.conf if it exists, defines like those above will
# override those set above.
if (-e "install.conf") {
    require "install.conf";
}



# You can extend the following SRB configure line with other options
# if you like, but it needs to include the currently defined ones:
$SRB_CONFIGURE_OPTIONS="--enable-installdir=$SRB_INSTALL_DIR --enable-psgmcat --enable-psghome=$postgresInstallDir";

$postgresTarFile = $POSTGRES_FILE;
if (rindex($POSTGRES_FILE,"gz") gt 0) {
    $postgresTarFile = substr($POSTGRES_FILE,0,-3);
}
$postgresDir=substr($postgresTarFile,0,-4);

$postgresBin = "$postgresInstallDir/bin";
$postgresData = "$postgresInstallDir/data";


# You can change the name of this state file if you like.
# If you want to redo the installation steps, you can remove the stateFile.
$stateFile="install.state";
$state=0;     # Major state/steps: 
              #  A - build and install postgres
              #  B - build and install odbc
              #  C - build srb and srb-mcat
              #  D - configure and run postgres server, create db
              #  E - create and test the MCAT database
              #  F - create local settings, user config, and start SRB
              #  G - do zones: rename local zone, and maybe create remote zones
$subState=0;  # Substep within the major state, 1 thru n

$fullStateFile = "$startDir/$stateFile";

if (!$SRB_FILE) {
    if (! -d "$startDir/$SRB_DIR") {
	usage();
	die("SRB_FILE not set and SRB_DIR ($startDir/$SRB_DIR) does not exist");
    }
}

if (!$YOUR_ADMIN_PW) {
    usage();
    die("YOUR_ADMIN_PW (admin password) is not set");
}

$uid=$<;
if ($uid eq "0") {
    print "This script should not be run as root.\n";
    print "Postgres will not install as root\n";
    print "and the SRB should be run as a non-root user\n";
    die("Running as root");
}

# Needed for Postgres commands:
$ENV{'PGDATA'}="$postgresData";
$oldPath=$ENV{'PATH'};
$ENV{'PATH'}="$postgresBin:$oldPath";
$oldLibPath=$ENV{'LD_LIBRARY_PATH'};  
if (!$oldLibPath) {
#   create LD_LIBRARY_PATH to have postgres
    $ENV{'LD_LIBRARY_PATH'}="$postgresInstallDir/lib";  
}
else {
#   or add it to LD_LIBRARY_PATH (may have GSI libraries defined, for example)
    $ENV{'LD_LIBRARY_PATH'}="$oldLibPath:$postgresInstallDir/lib";
}

# used later for Mac OS X-specific stuff (Darwin)
$thisOS=`uname -s`;
chomp($thisOS);
$gmake="gmake";
$psOptions="-el";
if ($thisOS eq "Darwin") {
    $gmake="alias gmake=make\ngmake";
    $psOptions="-ax";
}
if ($thisOS eq "SunOS") {
    $psOptions="-ef";
}

# Also on OS X, check and possibly increase the stack size from 512K to 2MB
if ($thisOS eq "Darwin") {
    $stackLimit = getStackLimit();
    if ($stackLimit < 1800000) {
        print "Your current stack size limit is $stackLimit\n";
        print "This is too small for many of the SRB commands\n";
        print "Increasing stack size limit\n";
        setStackLimit(2000000);
        $stackLimit = getStackLimit();
        print "The stack size limit for this process (and children) is now $stackLimit\n";
        print "You will need it set it for your shell.\n";
        print "For tcsh would be 'limit stacksize 2000'\n";
    }
}

setHostVars(); # set a few host-specific variables

$srbDone=0;
readState();

if ($state eq "A" or $state eq "B" or $state eq "C") {
    $postmaster=`ps $psOptions | grep postmas | grep -v grep`;
    if ($postmaster) {
	if ($SubsetMode lt "1") {
	    print "There is another postgres already running on this host and";
	    print " this script is not configured to use an existing one.\n";
	    die("Aborting, cannot install a second postgres system");
	}
    }
}

($arg1)=@ARGV;

if ($arg1 eq "stop") {
    $Servers = "";
    if ($srbDone eq "1") {
    # have completed SRB installation, stop it too
	print "chdir'ing to: $SRB_INSTALL_DIR/bin\n";
	chdir "$SRB_INSTALL_DIR/bin";
	writeFile("tty_input_yes","y\n");
	print "You may see an error that a process is not killed,\n";
	print "this is normal as the killsrb script attempts to kill\n";
	print "each srb process twice.\n";
	runCmdNoLog(1, "./killsrb < tty_input_yes");
	$Servers = "SRB";
    }
    if ($SubsetMode lt "1") {
	runCmdNoLog(1, "$postgresBin/pg_ctl stop");
	if ($Servers) {
	    $Servers = $Servers . " and Postgres";
	}
	else {
	    $Servers = $Servers . "Postgres";
	}
    }
    die ("Done stopping " . $Servers . " servers");
}
if ($arg1 eq "start") {
    $Servers = "";
    if ($SubsetMode lt "1") {
	runCmdNoLog(0, "$postgresBin/pg_ctl start -o '-i' -l $postgresData/pgsql.log");
	$Servers = "Postgres";
    }
    if ($srbDone eq "1") {
    # have completed SRB installation, start it too
	print "chdir'ing to: $SRB_INSTALL_DIR/bin\n";
	chdir "$SRB_INSTALL_DIR/bin";
	print "sleeping a second\n";
	sleep 1; # postgres sometimes needs a little time to setup
	runCmdNoLog(0, "./runsrb");
	runCmdNoLog(0, "ps $psOptions | grep srb");
	print "If the srb server is running OK, you should see srbMaster and srbServer here:\n";
	print $cmdStdout;
	if ($Servers) {
	    $Servers = $Servers . " and SRB";
	}
	else {
	    $Servers = $Servers . "SRB";
	}
    }
    die("Done starting " . $Servers . " servers");
}

if ($arg1 eq "ps") {
    $srb=`ps $psOptions | grep srb`;
    print "Running srb server processes:\n";
    print $srb;
    $post=`ps $psOptions | grep post`;
    print "Running postgres server processes:\n";
    print $post;
    die("Done listing processes");
}

if ($state eq "A") {
    buildPostgres();
}

if ($state eq "B") {
    buildOdbc();
}

if ($state eq "C") {
    buildSrb();
}

if ($state eq "D") {
    runPostgres();
}

if ($state eq "E") {
    installMcatDB();
}

if ($state eq "F") {
    createLocalSettings();
}

if ($state eq "G") {
    doZones();
}

print "Some examples for testing and learning about the SRB are at:\n";
print "http://www.npaci.edu/dice/srb/Edinburgh-Tutorials-May-2004/scmds.txt .\n";
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
	chomp($hostName);
	if ($thisOS eq "SunOS") {
	    $tmp=`nslookup $hostName`;
	    $i = index($tmp,"Name:");
	    $tmp = substr($tmp,$i);
	    $j = index($tmp, "Address:");
	    $hostFullNetName = substr($tmp,5,$j-5); # e.g. zuri.sdsc.edu
	    chomp($hostFullNetName);
	    chomp($hostFullNetName);
	    $i = rindex($hostFullNetName, " ");
	    $hostFullNetName = substr($hostFullNetName, $i+1);
	    $i = rindex($tmp," ");
	    $hostFullNetAddr = substr($tmp,$i+1);  # e.g. 132.249.32.192
	    chomp($hostFullNetAddr);
	    chomp($hostFullNetAddr);
	}
	else {
	    $tmp=`host $hostName`;  # returns for example: zuri.sdsc.edu has address 132.249.32.192
	    $i = index($tmp," ");
	    $hostFullNetName = substr($tmp,0,$i); # zuri.sdsc.edu
	    $i = rindex($tmp," ");
	    $hostFullNetAddr = substr($tmp,$i+1);  # 132.249.32.192
	    chomp($hostFullNetAddr);
	}
    }
    print "This script is $Version\n";
    print "This host is $hostName\n";
    print "This host full network name is $hostFullNetName\n";
    print "This host full network address is $hostFullNetAddr\n";

    $homeDir=$ENV{'HOME'};
    print "Your home directory is $homeDir\n";
}

sub createLocalSettings() {

    if ($SubsetMode eq "3") {
# if reusing MCAT, skip all this stuff
	print "Skipping a few createLocalSettings steps for SubsetMode 3\n";
	$subState=4;
    }

    #  Extract the builtin password for "srb"  (so 
    #        nothing confidential is in this script).
    $keyLine=`grep "MDAS_AU_AUTH_KEY values (1" $startDir/$SRB_DIR/MCAT/data/catalog.install.psg`;
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
	runCmd($bypassExitCheck,"./ingestUser $YOUR_ADMIN_NAME \'$YOUR_ADMIN_PW\' $YOUR_DOMAIN sysadmin '' '' ''");
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
	runCmd($bypassExitCheck,"./modifyUser changePassword srb sdsc \'$YOUR_ADMIN_PW\'");
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
		chomp($dateStr);
 		runCmdNoLog(0,"mv $homeDir/.srb/.MdasEnv $homeDir/.srb/.MdasEnv.old.$dateStr");
 	    }
 	}
 	else {
	    print "mkdir'ing: $homeDir/.srb\n";
 	    mkdir("$homeDir/.srb", 0700);
 	}
	print "Creating $homeDir/.srb/.MdasEnv file\n";
	writeFile("$homeDir/.srb/.MdasEnv", "mdasCollectionName \'/$YOUR_ZONE/home/$YOUR_ADMIN_NAME.$YOUR_DOMAIN\'\nmdasCollectionHome \'/$YOUR_ZONE/home/$YOUR_ADMIN_NAME.$YOUR_DOMAIN\'\nmdasDomainName \'$YOUR_DOMAIN\'\nmdasDomainHome \'$YOUR_DOMAIN\'\nsrbUser \'$YOUR_ADMIN_NAME\'\nsrbHost \'$hostFullNetName\'\n#srbPort \'5544\'\ndefaultResource \'$RESOURCE_NAME\'\n#AUTH_SCHEME \'PASSWD_AUTH\'\n#AUTH_SCHEME \'GSI_AUTH\'\nAUTH_SCHEME \'ENCRYPT1\'\n");

	if (-e "$homeDir/.srb/.MdasAuth") {
	    if ($SAFE_MODE eq "1") {
		die("SAFE_MODE is 1 and you already have a ~/.srb/.MdasAuth file");
	    }
	    $dateStr=`date | sed 's/ /_/g'`;
	    chomp($dateStr);
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
    if ($subState eq "6" and $SubsetMode ne "3") {
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

    runCmdNoLog(1, "./utilities/bin/Sexit");  # clear possible old Sinit env
    print "sleeping a second\n";
    sleep 1;
    runCmdNoLog(0, "./utilities/bin/Sinit");  # initialize for Szone

    if ($SubsetMode eq "3") {
# if reusing MCAT, skip all the rest
	runCmdNoLog(1, "./utilities/bin/Sexit");
	$state++;
	$subState=0;
	saveStateQuiet();
	chdir "$startDir" || die "Can't chdir to $startDir";
	return;
    }

# Change the local zone name
    if ($subState eq "0") {
	runCmdTwice(0, "./utilities/bin/Szone -C demozone $YOUR_ZONE");
#       There is a rare problem where this will fail, so as a workaround
#       the runCmdTwice routine will try it up to two times.
	$subState++;
	print "running Sls to clear out the waiting Server with the old zone\n";
	runCmdNoLog(1, "./utilities/bin/Sls foo"); # clear out waiting Server
	saveState();
    }

# Add the location to the zone, and also change the comment
# (this script uses hostName as the Location name)
    $now=`date`;
    chomp($now);
    if ($subState eq "1") {
	runCmd(0, "./utilities/bin/Szone -M $YOUR_ZONE $hostName '' '" . "$YOUR_ADMIN_NAME" . "@" . "$YOUR_DOMAIN" . "' '' 'Created $now'");
	$subState++;
	saveState();
    }


    if (!$REMOTE_ZONE) {
	print "REMOTE_ZONE not defined, skipping remote zone installation\n";
	runCmdNoLog(1, "./utilities/bin/Sexit");
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

    runCmdNoLog(1, "./utilities/bin/Sexit");
    $state++;
    $subState=0;
    saveStateQuiet();
    chdir "$startDir" || die "Can't chdir to $startDir";
}


sub installMcatDB() {

#   Run MCAT postgres table insert script
    if ($subState eq "0" and $SubsetMode lt "2") {
	print "chdir'ing to: $SRB_DIR/MCAT/data\n";
	chdir "$SRB_DIR/MCAT/data" || die "Can't chdir to $SRB_DIR/MCAT/data";
	unlink("myinstall.results.psg");
	if ($thisOS eq "SunOS") {
	    runCmdNoLog(0,"$postgresInstallDir/bin/psql $DB_NAME < catalog.install.psg > myinstall.results.psg 2>&1");
	}
	else {
	    runCmdNoLog(0,"$postgresInstallDir/bin/psql $DB_NAME < catalog.install.psg >& myinstall.results.psg");
	}
	print "It is probably OK, but you might compare myinstall.results.psg with install.results.psg to check differences\n";
	$subState++;
	saveState();
	print "chdir'ing to: $startDir\n";
	chdir "$startDir" || die "Can't chdir to $startDir";
    }

#   Set up this user's ~/.odbc.ini file 
    if ($subState eq "1") {
	if (-e "$homeDir/.odbc.ini") {
	    if ($SAFE_MODE eq "1") {
		die("SAFE_MODE is 1 and you already have a ~/.odbc.ini file");
	    }
	    $dateStr=`date | sed 's/ /_/g'`;
	    chomp($dateStr);
	    runCmdNoLog(1,"mv $homeDir/.odbc.ini $homeDir/.odbc.ini.old.$dateStr");
	}
        runCmdNoLog(0,"echo '[PostgreSQL]\nDebug=0\nCommLog=0\nServername=$hostFullNetName\nDatabase=$DB_NAME\nUsername=$YOUR_ADMIN_NAME\n' > $homeDir/.odbc.ini");
	$subState++;
	saveState();
    }

    if ($subState >= 2 && $subState <= 4) {
#       Need to run commands from MCAT/bin so that they can find ../data 
#       (Or could set srbData environment variable.)
	print "chdir'ing to: $SRB_DIR/MCAT/bin\n"; 
	chdir "$SRB_DIR/MCAT/bin" || die "Can't chdir to $SRB_DIR/MCAT/bin";
    }

#   Test commuincation to postgres
    if ($subState eq "2") {
        print "running: ./test_srb_mdas_auth a b c\n";
	$testResult=`./test_srb_mdas_auth a b c`;
	if (index($testResult,"-3206") eq -1) {
	    die("failed to communicate to postgres");
	}
	$subState++;
	saveState();
    }

    if ($SubsetMode eq "2") {
	$state++;
	$subState=0;
	chdir "$startDir" || die "Can't chdir to $startDir";
	return;
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
	runCmd(1,"diff $startDir/installE4.log ../data/test.results.ora");
	$wc=`cat $startDir/installE5.log | wc -l`;
	chomp($wc);
	print "There are $wc lines of diffences.\n";
	if ($wc != 0) {
	    if ($wc < 40) {
		print "There are sometimes small differences like these\n";
		print "due to changes in postgresql versions.\n";
		print "This is probably OK, but you might examine these manually.\n";
	    }
	    else {
		die "Too many differences in test results; please check it.";
	    }
	}
    }
    $state++;
    $subState=0;
    chdir "$startDir" || die "Can't chdir to $startDir";
}

sub runPostgres() {

    if ($SubsetMode ge "2") {
	print "Skipping all runPostgres steps as this script is configured to use an existing one.\n";
	$state++;
	$subState=0;
	chdir "$startDir" || die "Can't chdir to $startDir";
	return;
    }
    if ($SubsetMode eq "1") {
	print "Skipping some runPostgres steps as this script is configured to use an existing one.\n";
	$subState=3;   # Skip down to create the db
    }

    if ($subState eq "0") {
        # Previously, this script would create the data subdir, but
        # that can cause problems in that the mode can't be set right
        # and initdb would fail trying to chmod it.  Seems to work OK
        # to just skip the mkdir.

	runCmd(0,"$postgresBin/initdb -D $postgresData");

	if ($thisOS eq "Darwin") {
        # Mac gets error in postges starting if LC_TIME isn't commented out
	    runCmdNoLog(0, "cat $postgresInstallDir/data/postgresql.conf | sed s/LC_TIME/#LC_TIME/g > $postgresInstallDir/data/postgresql.conf");
	} 
	$subState++;
	saveState();
   }
    if ($subState eq "1") {
	if (index($POSTGRES_FILE,"7.2.3") gt 0) {
            # older version had different format
	    runCmdNoLog(0,"echo host all $hostFullNetAddr 255.255.255.255 trust | cat >> $postgresData/pg_hba.conf");
	}
	else {
            # this is right for 7.3.3, not sure about intermediate
	    runCmdNoLog(0,"echo host all all $hostFullNetAddr 255.255.255.255 trust | cat >> $postgresData/pg_hba.conf");
	}
	$subState++;
	saveState();
    }
    if ($subState eq "2") {
        runCmdNoLog(0, "$postgresBin/pg_ctl start -o '-i' -l $postgresData/pgsql.log");
	$subState++;
	saveState();
	print "sleeping a couple seconds\n";
	sleep 2;
    }
    if ($subState eq "3") {
	runCmd(0,"$postgresBin/createdb $DB_NAME");
	$subState++;
	saveState();
    }
    $state++;
    $subState=0;
    chdir "$startDir" || die "Can't chdir to $startDir";
}

sub buildPostgres() {

    if ($SubsetMode ge "1") {
	print "Skipping postgres build as this script is configured to use an existing one.\n";
	$state++;
	$subState=0;
	chdir "$startDir" || die "Can't chdir to $startDir";
	return;
    }

    if (rindex($POSTGRES_FILE,"gz") gt 0) {
	if ($subState eq "0") {
	    if (-e $postgresTarFile) {
		print "Skipping gzip -d as the uncompressed tar file already exists\n";
	    }
	    else {
		runCmd(0,"gzip -d $POSTGRES_FILE");
	    }
	}
    }
    else {
	print "Skipping gzip -d as the tar file is already uncompressed\n";
    }
    if ($subState eq "0") {
	$subState++;
	saveState();
    }

    if ($subState eq "1") {
	runCmd(0,"tar xf $postgresTarFile");
	$subState++;
	saveState();
    }

    print "chdir'ing to: $postgresDir\n";
    chdir "$postgresDir" || die "Can't chdir to $postgresDir";
    if ($subState eq "2") {
	if ($thisOS eq "SunOS") {
	    runCmd(0,"./configure --prefix=$postgresInstallDir --enable-odbc --without-readline --without-zlib");
	}
	else {
	    runCmd(0,"./configure --prefix=$postgresInstallDir --enable-odbc --without-readline");
	}
	$subState++;
	saveState();
    }

    if ($subState eq "3") {
	runCmd(0,"$gmake");
	$subState++;
	saveState();
    }

    if ($subState eq "4") {
	runCmd(0,"$gmake install");
	$subState++;
	saveState();
    }

    $state++;
    $subState=0;
    chdir "$startDir" || die "Can't chdir to $startDir";
}

sub buildOdbc() {
    if ($SubsetMode ge "1") {
	print "Skipping odbc build as this script is configured to use an existing one\n";
	$state++;
	$subState=0;
	chdir "$startDir" || die "Can't chdir to $startDir";
	return;
    }

    $odbcTarFile = $ODBC_FILE;
    if (rindex($ODBC_FILE,"gz") gt 0) {
	$odbcTarFile = substr($ODBC_FILE,0,-3);
	if ($subState eq "0") {
	    if (-e $odbcTarFile) {
		print "Skipping gzip -d as the uncompressed tar file already exists\n";
	    }
	    else {
		runCmd(0,"gzip -d $ODBC_FILE");
	    }
	}
    }
    if ($subState eq "0") {
	$subState++;
	saveState();
    }
    $odbcDir=substr($odbcTarFile,0,-4);

    $postgresInt="$postgresDir/src/interfaces";

    print "chdir'ing to: $postgresInt\n";
    chdir "$postgresInt" || die "Can't chdir to $postgresInt";

    print "mkdir'ing: odbc\n";
    mkdir("odbc", 0700);
    print "chdir'ing to: odbc\n";
    chdir "odbc" || die "Can't chdir to odbc";

    if ($subState eq "1") {
	runCmd(0,"tar xf $startDir/$odbcTarFile");
	runCmdNoLog(0,"mv $odbcDir/* .");
	$subState++;
	saveState();
    }

    if ($subState eq "2") {
	runCmd(0,"./configure --prefix=$postgresInstallDir --enable-static");
	$subState++;
	saveState();
    }

    if ($subState eq "3") {
	runCmd(0,"$gmake");
	$subState++;
	saveState();
    }

    if ($subState eq "4") {
	runCmd(0,"$gmake install");
	if (-e "$postgresInstallDir/lib/psqlodbc.a") { # if psqlodbc.a  exists
#           copy it to proper lib file (seems to be needed on some machines).
	    runCmd(0,"cp $postgresInstallDir/lib/psqlodbc.a $postgresInstallDir/lib/libpsqlodbc.a");
	}
	runCmd(0,"cp iodbc.h isql.h isqlext.h $postgresInstallDir/include");
	$subState++;
	saveState();
    }

    if ($thisOS eq "Darwin") {
	if ($subState eq "5") {
# Thru trial and error, I found that these commands will create a link
# library with no missing externals.  This cp is a hack, but I'm not
# sure the right way to create what we want (it avoids a missing
# external of _globals).  Also, this whole thing of creating a .a file
# from all the .o's via libtool is odd, but the odbc configure/make
# doesn't seem to handle it right.
	    runCmdNoLog(0,"cp psqlodbc.lo psqlodbc.o");   
	    runCmdNoLog(0,"libtool -o libpsqlodbc.a *.o");
	    runCmdNoLog(0,"cp libpsqlodbc.a $postgresInstallDir/lib");
	    chdir "$postgresInstallDir/lib" || die "Can't chdir to $postgresInstallDir/lib";
	    runCmdNoLog(0,"ranlib libpsqlodbc.a");
	    $subState++;
	    saveState();
	}
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
		print "**** After it pauses, enter the SRB distribution file password ****\n";
		if (index($SRB_FILE,"bf.tar") gt 0) {
		    runCmd(0,"openssl enc -d -bf-cbc -in $SRB_FILE -out $SRB_FILE.decrypted.tar");
		}
		else {
                    # first, create and run a small script to prompt 
                    # when pgp is waiting for input. (pgp will print some 
                    # stuff first, so without this it 
                    # is hard to see that pgp is waiting.)
                    writeFile("install_reminder.pl", "sleep 1;\nprint \"Enter the SRB distribution password: \";\n");
		    system('perl install_reminder.pl&');  # run in background
		    runCmd(0,"pgp -d $SRB_FILE -o $SRB_FILE.decrypted.tar");
		    unlink("install_reminder.pl");
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
	runCmd(0,"$gmake");
	$subState++;
	saveState();
    }

    if ($subState eq "4") {
	print "chdir'ing to: MCAT\n";
	chdir "MCAT" || die "Can't chdir to MCAT";
	runCmd(0,"$gmake");
	$subState++;
	saveState();
	chdir ".." || die "Can't chdir to ..";
    }

    if ($subState eq "5") {
	if ($DO_JAVA) {
	    print "chdir'ing to: jargon\n";
	    chdir "jargon" || die "Can't chdir to jargon";
	    runCmd(0,"$gmake all");
	    chdir ".." || die "Can't chdir to ..";
	}
	else {
	    print "Build of Jargon configured out, skipping\n";
	}
	$subState++;
	saveState();
    }

    if ($subState eq "6") {
	if ($DO_JAVA) {
	    print "chdir'ing to: MCAT/java\n";
	    chdir "MCAT/java" || die "Can't chdir to MCAT/java";
	    runCmd(0,"$gmake");
	    chdir "../.." || die "Can't chdir to ../..";
	}
	else {
	    print "Build of Java Admin Tool configured out, skipping\n";
	}
	$subState++;
	saveState();
    }

    # create the MdasConfig file, set up for Postgres 
    if ($subState eq "7") {
	$userName = `whoami`;
	runCmdNoLog(0,"mv $startDir/$SRB_DIR/data/MdasConfig $startDir/$SRB_DIR/data/MdasConfig.old");
	print "Creating $startDir/$SRB_DIR/data/MdasConfig file\n";
	writeFile("$startDir/$SRB_DIR/data/MdasConfig", "MDASDBTYPE       postgres\nMDASDBNAME       PostgreSQL\nMDASINSERTSFILE  $startDir/$SRB_DIR/data/mdas_inserts\nMETADATA_FKREL_FILE metadata.fkrel\nDB2USER           $userName\nDB2LOGFILE       $startDir/$SRB_DIR/data/db2logfile\nDBHOME        $postgresInstallDir/data\n");
	$subState++;
        saveState();
    }

    # change host name in mcatHost file
    if ($subState eq "8") {
        runCmdNoLog(0, "cat $startDir/$SRB_DIR/data/mcatHost | sed s/srb.sdsc.edu/$hostFullNetName/g > $startDir/$SRB_DIR/data/mcatHost2");
	`cp $startDir/$SRB_DIR/data/mcatHost2 $startDir/$SRB_DIR/data/mcatHost`;
	unlink "$startDir/$SRB_DIR/data/mcatHost2";
	$subState++;
	saveState();
    }

    # add localhost to hostConfig (seems to be needed on some hosts)
    if ($subState eq "9") {
	runCmdNoLog(0, "echo localhost $hostFullNetName >> $startDir/$SRB_DIR/data/hostConfig");
	$subState++;
	saveState();
    }

    if ($subState eq "10") {
	runCmd(0,"$gmake install");
	$subState++;
	saveState();
    }

    if ($thisOS eq "Darwin") {
        # Need different version of killsrb on Macs
	if ($subState eq "11") {
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


# run a command once or twice, and log its output to a file.
# If the first attempt fails, sleep a second and try again; if it
# fails then, it fails.
sub runCmdTwice {
    my($option,$cmd) = @_;
    $workingState=$subState+1; # State/step we are currently working on
    $fullCmd = "$cmd > $startDir/install$state$workingState.log";
    print "running: $fullCmd \n";
    `$fullCmd`;
    $cmdStat=$?;
    if ($option == 0) {
	if ($cmdStat!=0) {
	    print "First try failed, sleeping a second to try again\n";
	    sleep 1;
	    $fullCmd = "$cmd >> $startDir/install$state$workingState.log";
	    `$fullCmd`;
	    $cmdStat=$?;
	    if ($cmdStat!=0) {
		print "The following command failed:";
		print "$fullCmd \n";
		print "Exit code= $cmdStat \n";
		die("command failed");
	    }
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
	read(F,$subState,2);
	close(F);
	chomp($subState); # remove possible trailing \n (vi forces one)
    }
    else {
	$state="A";
	$subState=0;
    }
    if ($state eq "G" or $state eq "H") {
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

sub getStackLimit {
    $SYS_getrlimit=194; # from /usr/include/sys/syscall.h
    $RLIMIT_STACK=3;    # from /usr/include/sys/resource.h
    $rlimit = pack(i2,0,0);
    $f=syscall($SYS_getrlimit,$RLIMIT_STACK,$rlimit);
    if ($f != 0) {
	print "Warning, syscall to getrlimit failed\n";
	return("0");
    }
    my($result1, $result2)=unpack(i2,$rlimit);
    return($result2);
}

sub setStackLimit {
    my($newValue)= @_;
    $SYS_setrlimit=195; # from /usr/include/sys/syscall.h
    $RLIMIT_STACK=3;    # from /usr/include/sys/resource.h
    $rlimit = pack(i2,0,$newValue);
    $f=syscall($SYS_setrlimit,$RLIMIT_STACK,$rlimit);
    if ($f != 0) {
	print "Warning, syscall to setrlimit failed\n";
    }
}

