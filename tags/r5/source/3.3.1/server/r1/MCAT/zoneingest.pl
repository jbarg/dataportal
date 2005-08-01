#!/usr/bin/perl

# This script creates a remote Zone defined in the local MCAT.  It
# uses as input, the output of a 'Stoken Zone' command run at a Zone
# you wish to ingest.  This text could be sent via email from the
# other SRB administrator.  So, say you are zone A and want to
# federate with zone B.  The admin at B sets up his local zone, then
# run's 'Stoken Zone' and emails you the output.  You put it into a
# file and run 'zonei.pl <filename>'.  This script will parse the
# values it needs in the text file (looking for just the local zone in
# the file) and then runs MCAT commands to create the remote zone.

# Normally, the first command-line argument is a filename that
# contains output from a 'Stoken Zone' command run at a Zone you wish
# to import.  Alternatively, you can enter the parameters by hand by
# editing the parameters below.
#
# If you moved this from the .../MCAT/bin directory, then you need to
# edit the line below to be your full path to it. (This script will cd
# to it internally which is needed for the ingest commands to work.)
$MCAT_BIN="bin";
#$MCAT_BIN="/scratch/slocal/srb1217/SRB2_0_0rel/MCAT/bin";

$zone_id="";
$netprefix="";
$port_number="";
$zone_status="";
$user_name="";
$domain_desc="";
$locn_desc="";
###########################

if (!$zone_id) {
    if ($#ARGV < 0) {
	printf("Include a filename on the command line that contains Stoken Zone output\n");
	printf("Or edit parameters in the script.\n");
	die("No filename");
    }

    open(FileIn, $ARGV[0]) || die("Can't open input file " . "$ARGV[0]");

    $done=0;
    while($line = <FileIn> and $done==0) {
	chomp($line);
	@flds = split(/: /,$line);
	if ($flds[0] eq "zone_id") {
	    $zone_id=$flds[1];
	    $local_zone_flag="";
	    $netprefix="";
	    $port_number="";
	    $zone_status="";
	    $user_name="";
	    $domain_desc="";
	    $locn_desc="";
	}
	if ($flds[0] eq "local_zone_flag") {
	    $local_zone_flag=$flds[1];
	}
	if ($flds[0] eq "netprefix") {
	    $netprefix=$flds[1];
	}
	if ($flds[0] eq "port_number") {
	    $port_number=$flds[1];
	}
	if ($flds[0] eq "zone_status") {
	    $zone_status=$flds[1];
	}
	if ($flds[0] eq "user_name") {
	    $user_name=$flds[1];
	}
	if ($flds[0] eq "domain_desc") {
	    $domain_desc=$flds[1];
	}
	if ($flds[0] eq "locn_desc") {
	    $locn_desc=$flds[1];
	}
	if ($locn_desc && $local_zone_flag=="1") {
	    $done=1;
	}
    }
}
printf("zone_id=%s\n",$zone_id);
printf("netprefix=%s\n",$netprefix);
printf("port_number=%s\n",$port_number);
printf("zone_status=%s\n",$zone_status);
printf("user_name=%s\n",$user_name);
printf("domain_desc=%s\n",$domain_desc);
printf("locn_desc=%s\n",$locn_desc);

printf("Look OK?  Enter yes (or y) to proceed:");
$cmd=<STDIN>;
chomp($cmd);
if ($cmd eq "y" or $cmd eq "yes") {
}
else {
    print "Ingestion not performed\n";
    die("User cancelled");
}

# cd into the directory
if (-e "ingestToken") {
}
else {
    if (-e "$MCAT_BIN/ingestToken") {
	printf("chdir'ing to $MCAT_BIN\n");
	chdir("$MCAT_BIN") || die "Can't cd to $MCAT_BIN";
    }
    else {
	printf("Cannot find MCAT/bin directory\n");
	printf("$MCAT_BIN/ingestToken does not exist\n");
	die("no MCAT/bin");
    }
}

# create the domain
runCmd(-3119, "./ingestToken Domain $domain_desc home", 
       "TOKEN_ALREADY_IN_CAT");

# Generate a long arbitrary password
# and then ingest the user
$time=`date`;
chomp($time);
$RandomValue=rand(9999999);
$pw=$time . $RandomValue;
runCmd(-3218, "./ingestUser '$user_name' '$pw' '$domain_desc' 'sysadmin' '' '' ''",
       "USER_NOT_UNIQUE_IN_DOMN");

# ingest the location
runCmd(-3119, "./ingestLocation '$locn_desc' '$netprefix' 'home' '$user_name' '$domain_desc'",
       "TOKEN_ALREADY_IN_CAT");

$userAtDomain = $user_name . "@" . $domain_desc;
# create the remote zone
# (can't catch errors like others because msgs going to stderr)
runCmdStderr(-3312, "../../utilities/bin/Szone -r '$zone_id' '$locn_desc' '$port_number' '$userAtDomain' '' ''",
       "ZONE_NAME_ALREADY_EXISTS");

# change the user to be part of the remote zone
runCmdStderr(-999999, "../../utilities/admin-bin/SmodifyUser changeZone '$user_name' '$domain_desc' '$zone_id'",
       "Should not happen");


printf("Success\n");


# Run a command, print it, and die if an error unless allowed error type
sub runCmd {
    my($OkValue, $cmd, $errDesc) = @_;
    print "running: $cmd \n";
    $cmdStdout=`$cmd`;
    $errIx=index($cmdStdout,"Error: ");
    $errMsg="";
    if ($errIx>0) {
	$errMsg=substr($cmdStdout,$errIx+7);
	chomp($errMsg);
    }
    if ($errMsg == $OkValue) {
	printf("Command failed, but is it OK; the error was: %s\n",$errDesc);
	return;
    }

    $cmdStat=$?;
    if ($cmdStat!=0) {
	print "The command failed:\n";
	print "Stdout: $cmdStdout\n";
	die("command failed");
    }
}

# Like runCmdStderr but for commands that send their errors to Stderr
sub runCmdStderr{
    my($OkValue, $cmd, $errDesc) = @_;
    print "running: $cmd \n";
    $RandomValue=rand(9999999);
    $tmpFile="zoneiTmpFile" . $RandomValue;
    if (-e $tmpFile) {
	die("tmp File already exists");
    }
    $cmdStdout=`$cmd 2> $tmpFile`;
    $cmdStderr=`cat $tmpFile`;
    unlink $tmpFile;
    if ($cmdStderr) {
	$errIx=index($cmdStderr,"$OkValue");
	if ($errIx>0) {
	    printf("Command failed, but is it OK; the error was: %s\n",
		   $errDesc);
	    return;
	}
	print "The command failed:\n";
	print "Stderr: $cmdStderr\n";
	die("command failed");
    }
}
