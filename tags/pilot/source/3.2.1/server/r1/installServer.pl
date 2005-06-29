#!/usr/bin/perl
# On SDSC Solaris systems, change the above line to this:
# #!/usr/local/bin/perl5.8

# This script helps install a non-MCAT SRB server.
# See the install3.pl script for installing a full MCAT-enabled SRB server.

# To keep it simple and to help you understand the procedure, you need
# to do some of the steps yourself, choosing your configuration as you go.
#
# Here's what you need to do:
# 0) Install or otherwise have access to an MCAT-enabled Server.
# 1) On the computer you want to install the Server on:
#      a) unpack the SRB release
#      b) create ~/.srb/.MdasEnv and ~/.srb/.MdasAuth files.
#         If you used install3.pl you can copy them from that computer.
#         The srbHost should be the MCAT host.
# 2) Set the parameters below.
# 3) Run this script.
#      It will say "Installation complete" if it succeeds.
# 4) execute 'runsrb' to start the SRB
#      If you are running on Mac OS X, first run 'limit stacksize 2000'
#      to increase your stacksize before running 'runsrb'.
#      To stop the srb Server, run 'killsrb' or 'killsrbMac'.
#      Check the Log in data/srbLog.

# This script doesn't do a gmake install as it is often more
# convenient to just run it where it is built.

# This script doesn't keep track of which step has been completed but
# it is OK to rerun it again.  It will run configure and gmake again,
# but that's fine.  On subsequent runs, they will be quicker because
# they aren't doing as much.

$startDir=`pwd`;
chomp($startDir);  # the current directory

# ***********************************************************************

$YOUR_ADMIN_NAME="srbAdmin";      # Change this to the name for the srb-admin 
                                  # login acct that you are using.

$YOUR_DOMAIN="demo";              # Change this to the SRB domain name you 
                                  # are using.

$RESOURCE_NAME="";                # Name of the local SRB resource name that
                                  # this SRB server will create and use

$RESOURCE_DIR="$startDir/Vault";  # Subdirectory that will be the resource
                                  # (i.e., the real unix directory that 
                                  # will hold the SRB data files).

$MCAT_HOST="";                    # full DNS name of MCAT-enabled server host
                                  # for example, "srb.sdsc.edu"

# Leave $hostName and $hostFullNetName blank and it will try to
# determine their values, but if there is some problem in it doing
# that, set one or both of these by hand:
$hostName="";                     # For example $hostName = "client65-80";
$hostFullNetName="";              # "client65-80.sdsc.edu";

$locationName="";                 # leave this blank and it will use hostName
                                  # as the name of the location.  Set this
                                  # to some other name if you prefer.

# ***********************************************************************

if (!$YOUR_ADMIN_NAME) {
    usage();
    die("YOUR_ADMIN_NAME is not set");
}

if (!$YOUR_DOMAIN) {
    usage();
    die("YOUR_DOMAIN is not set");
}

if (!$RESOURCE_NAME) {
    usage();
    die("RESOURCE_NAME is not set");
}

if (!$MCAT_HOST) {
    usage();
    die("MCAT_HOST is not set");
}

setHostVars();

$thisOS=`uname -s`;
chomp($thisOS);
$gmake="gmake";
if ($thisOS eq "Darwin") {
    $gmake="alias gmake=make\ngmake";
}

if (!$locationName) {
    $locationName = $hostName;
}

runCmd("./configure", "install1.log");

runCmd("$gmake", "install2.log");

runCmd("utilities/admin-bin/Sregisterlocation $locationName $hostFullNetName:NULL:NULL level4 $YOUR_ADMIN_NAME $YOUR_DOMAIN","install3.log");
if ($cmdStat !=0) {
    if ($cmdStat == 2) {
        print "The Location is already registered\n";
        print "Will assume this is OK and continue\n";
    }
    else {
        print "Fatal error when registering the resource\n";
        die ("Exiting");
    }
}

runCmd("utilities/admin-bin/Sregisterresource '$RESOURCE_NAME' 'unix file system' '$locationName' '$RESOURCE_DIR/?USER.?DOMAIN/?SPLITPATH/?PATH?DATANAME.?RANDOM.?TIMESEC' permanent", "install4.log");

if ($cmdStat !=0) {
    if ($cmdStat == 2) {
        print "The Resource is already registered\n";
        print "Will assume this is OK and continue\n";
    }
    else {
        print "Fatal error when registering the resource\n";
        die ("Exiting");
    }
}

# change host name in mcatHost file
print "\nEditing data/mcatHost file to have $MCAT_HOST as the mcatHost\n\n";
`cat $startDir/$SRB_DIR/data/mcatHost | sed s/srb.sdsc.edu/$MCAT_HOST/g > $startDir/$SRB_DIR/data/mcatHost2`;
$cmdStat=($?>>8);
if ($cmdStat != 0) {
    print "Error running cat and sed\n";
    die("Exiting");
}
`cp $startDir/$SRB_DIR/data/mcatHost2 $startDir/$SRB_DIR/data/mcatHost`;
$cmdStat=($?>>8);
if ($cmdStat != 0) {
    print "Error running cp\n";
    die("Exiting");
}
unlink "$startDir/$SRB_DIR/data/mcatHost2";

# add localhost to hostConfig (seems to be needed on some hosts)
print "\nEditing data/hostConfig file to have localhost $hostFullNetName line\n\n";
`echo localhost $hostFullNetName >> $startDir/$SRB_DIR/data/hostConfig`;
$cmdStat=($?>>8);
if ($cmdStat != 0) {
    print "Error running echo to append to file\n";
    die("Exiting");
}

print "Installation complete\n";
print "You should be able to 'runsrb' now.\n";

# run a command, and log its output to a file.
sub runCmd {
    my($cmd,$logFile) = @_;
    $fullCmd = "$cmd > $logFile";
    print "running: $fullCmd \n";
    `$fullCmd`;
    $cmdStat=($?>>8);
}


sub setHostVars() {
    if (!$hostName) {
        $hostName= `uname -n`; 
    }
    chomp($hostName);
    if ($thisOS eq "SunOS") {
        $tmp=`nslookup $hostName`;
        $i = index($tmp,"Name:");
        $tmp = substr($tmp,$i);
        $j = index($tmp, "Address:");
        if (!$hostFullNetName) {
            $hostFullNetName = substr($tmp,5,$j-5); # e.g. zuri.sdsc.edu
            chomp($hostFullNetName);
            chomp($hostFullNetName);
        }
        $i = rindex($hostFullNetName, " ");
        $hostFullNetName = substr($hostFullNetName, $i+1);
        $i = rindex($tmp," ");
    }
    else {
        $tmp=`host $hostName`;  # returns for example: zuri.sdsc.edu has address 132.249.32.192
        $i = index($tmp," ");
        if (!$hostFullNetName) {
            $hostFullNetName = substr($tmp,0,$i); # zuri.sdsc.edu
        }
    }
    print "This host is $hostName\n";
    print "This host full network name is $hostFullNetName\n";
}

sub usage() {
    printf("usage: first edit a few defines in this file and then run it\n");
}
