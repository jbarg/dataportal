#!/usr/bin/perl
#
# This script gets files stored by Sput.pl and decrypts and/or decompresses
# them if necessary.  See usage at the end of this script.

# Defines you may need to change:
$OPENSSL="openssl";
$SINIT="Sinit";
$SGET="Sget";
$SGETD="SgetD -f";
$SEXIT="Sexit";
$GZIP="gzip";
# You may want to set the full paths, for example:
# $OPENSSL="/usr/local/apps/nmi-2.1/bin/openssl";
# $SGET="/scratch/slocal/SRB2a/SRB2/utilities/bin/Sget";
# $SGETD="/scratch/slocal/SRB2a/SRB2/utilities/bin/SgetD -f";
# $GZIP="/bin/gzip";
#
#Other defines:
$TMP_PW_FILE="%TMP_File1_ef0d77ec5f07";
$TMP_FILE_BASE="%TMP_File_9644b5d0f";
$FILE_KEY_LENGTH=21;
$ALLOWED_ENCRYPTION_TYPES="rc5 rc5-cbc rc4 bf";
$ALLOWED_COMPRESSION_TYPES="gzip gzip-1 gzip-9";
$FIELD_DELIMITER=":";

require "ctime.pl";

$start1=time();

#
# Parse command line and do some checking
#
$verboseLevel=1;
$fileNames="";
for ($i=0; $i<=$#ARGV; $i++) {
    if ($ARGV[$i] eq "-v") {
	$verboseLevel++;
    }
    else {
	if ($ARGV[$i] eq "-q") {
	    $verboseLevel=0;
	}
	else {
	    if ($ARGV[$i] eq "-h") {
		usage();
		die("\n");
	    }
	    else {
		$fileNames = $fileNames . $ARGV[$i] . " ";
	    }
	}
    }
}

#
# First Sinit.   If the user has Sinit'ed from the tty, this is not
#    always needed, but sometimes it is, and it should give a clearer
#    error message if something fails.  Also, if the controlling shell
#    exits, this script can continue.
#
if ($verboseLevel ge "1") {
    $Date= &ctime(time());
    $Date=substr($Date,11,8);
    printf("%s Initializing a session with the SRB server (Sinit)\n",
	   $Date); 
}
$cmd = "$SINIT";
if ($verboseLevel ge "2") {
    print "running: " . "$cmd"."\n";
}
`$cmd`;
$cmdStat=$?;
if ($cmdStat!=0) {
    print "The Sinit command failed, check your SRB environment.";
    die("can't Sinit");
}
#


@fileNames = split(/ /,$fileNames);
$fileCount=0;
foreach $fileName (@fileNames) {
    $start2=time();
    $compressType="";
    $encryptType="";
    $password="";
    $fileCount++;
#
# Get the attributes and key
#
    if ($verboseLevel ge "1") {
	$Date= &ctime(time());
	$Date=substr($Date,11,8);
	printf("%s Getting attributes and key for %s\n", $Date, $fileName); 
    }
    $cmd = "$SGETD $fileName";
    if ($verboseLevel ge "2") {
	print "running: " . "$cmd"."\n";
    }
    $SGETM_Value = `$cmd`;
    $cmdStat=$?;
    if ($cmdStat!=0) {
	print "The following command failed:";
	print "$cmd"."\n";
	die("can't get the file attributes");
    }

    $ix=index($SGETM_Value, "MD5=");
    if ($ix < 2) {
	printf(" ERROR: MD5 not found\n");
	printf(" Most likely, 1) the file was not stored with Sput.pl or 2) doesn't exist.\n");
	printf(" For 1) use Sget to retrieve the file.\n");
	die("bad file attributes");
    }
    $MD5=substr($SGETM_Value, index($SGETM_Value, "MD5=")+4, 80);
    $MD5_Saved_Value=substr($MD5, 0, index($MD5, "$FIELD_DELIMITER"));

    $_ = $SGETM_Value;
    s/\n/$FIELD_DELIMITER/g;   # count EOL as a delimiter
    $SGETM_Value = $_;

    $ix=rindex($SGETM_Value, "Compress=");
    if ($ix gt 0) {
	$comp=substr($SGETM_Value, $ix+9, 80);
	$compressType=substr($comp, 0, index($comp, "$FIELD_DELIMITER"));
    }
    $ix=rindex($SGETM_Value, "Encrypt=");
    if ($ix gt 0) {
	$enc=substr($SGETM_Value, $ix+8, 80);
	$encryptType=substr($enc, 0, index($enc, "$FIELD_DELIMITER"));
    }
    $ix=rindex($SGETM_Value, "dencrypted :");
    if ($ix gt 0) {
	$enc=substr($SGETM_Value, $ix+12, 80);
	$password=substr($enc, 0, index($enc, "$FIELD_DELIMITER"));
    }

    if ($verboseLevel ge "2") {
	printf ("compressType = %s\n", $compressType);
	printf ("encryptType = %s\n", $encryptType);
	printf ("file key = %s\n", $password);
	printf ("fileName = %s\n", $fileName);
    }


#
# Get the file
#
    if ($verboseLevel ge "1") {
	$Date= &ctime(time());
	$Date=substr($Date,11,8);
	printf("%s Getting file %s\n", $Date, $fileName); 
    }
    $cmd = "$SGET" . " " .  $fileName;
    if ($verboseLevel ge "2") {
	print "running: " . "$cmd"."\n";
    }
    `$cmd`;
    $cmdStat=$?;
    if ($cmdStat!=0) {
	print "The following command failed:";
	print "$cmd"."\n";
	die("can't Sget the file");
    }


    if ($encryptType) {
#
# Write the file encryption key (password) into a file, since this 
# is more secure than having it on the command line.
#
	unlink($TMP_PW_FILE);
	open(F,">".$TMP_PW_FILE) || die("can't open temp file for writing ".$TMP_PW_FILE);
	printf F $password;
	close(F);

#
# Run the openssl decryption command
#
	$nextFile=$TMP_FILE_BASE . ".decrypted";
	if ($verboseLevel ge "1") {
	    $Date= &ctime(time());
	    $Date=substr($Date,11,8);
	    printf("%s Decrypting (%s) file %s\n", 
		   $Date, $encryptType, $fileName); 
	}
	$cmd="$OPENSSL enc -d -$encryptType -pass file:$TMP_PW_FILE -in $fileName -out $nextFile";
	if ($verboseLevel ge "2") {
	    print "running: " . "$cmd"."\n";
	}
	`$cmd`;
	$cmdStat=$?;
	if ($cmdStat!=0) {
	    print "The following command failed:";
	    print "$cmd"."\n";
	    die("can't encrypt the input file");
	}
	if ($verboseLevel ge "1") {
	    $sizeIn = -s $fileName;
	    $sizeOut = -s $nextFile;
	    print "         Input file was $sizeIn bytes, output $sizeOut \n";
	}
	if ($verboseLevel ge "2") {
	    print "Moving " . $nextFile . " to " . $fileName . "\n";
	}
	unlink ($fileName);
	rename($nextFile, $fileName);
	unlink($TMP_PW_FILE);
    }


    if ($compressType) {
	if ($verboseLevel ge "1") {
	    $Date= &ctime(time());
	    $Date=substr($Date,11,8);
	    printf("%s Decompressing (compression type: %s)\n", 
		   $Date, $compressType); 
	}

	$genType=substr($compressType, 0, 4);
	if ($genType eq "gzip") {
	    $cmd="$GZIP -d";
	    $currentFile = $TMP_FILE_BASE . ".gz";
	    if ($verboseLevel ge "2") {
		print "Moving " . $fileName . " to " . $currentFile . "\n";
	    }
	    rename($fileName, $currentFile);
	}
	$cmd = $cmd . " -c " . $currentFile . " > " . $fileName ;
	if ($verboseLevel ge "2") {
	    print "running: " . "$cmd"."\n";
	}
	`$cmd`;
	$cmdStat=$?;
	if ($cmdStat!=0) {
	    print "The following command failed:";
	    print "$cmd"."\n";
	    die("cant decompress the input file");
	}
	if ($verboseLevel ge "1") {
	    $sizeIn = -s $currentFile;
	    $sizeOut = -s $fileName;
	    $ratio = $sizeIn/$sizeOut;
	    $ratio = substr($ratio, 0, 5);
	    print "         Input file was $sizeIn bytes, output $sizeOut, ratio: $ratio\n";
	}

	unlink ($currentFile);
    }

#
# Get MD5 hash of recreated file
#
    if ($verboseLevel ge "1") {
	$Date= &ctime(time());
	$Date=substr($Date,11,8);
	printf("%s Computing MD5 hash value\n", $Date); 
    }

    if ($verboseLevel ge "2") {
	print "running: " . "$OPENSSL md5 $fileName \n";
    }

    $md5=`$OPENSSL md5 $fileName`;
    if ($? != 0) {
	die "cant generate md5 hash (OpenSSL not available?)";
    }
    @md5_split = split(/ /,$md5);
    $MD5Value = @md5_split[1];
    chop($MD5Value);
    if ($verboseLevel ge "1") {printf("         MD5=%s\n",$MD5Value); }
    
    if ($MD5Value eq $MD5_Saved_Value) {
	if ($verboseLevel ge "1") { 
	    printf(
           "         MD5 OK (current file's MD5 matches the stored value)\n"); 
	}
    }
    else {
	printf(" ERROR: MD5 not OK\n");
	die("bad file hash");
    }

#
# Print a done message with timestamp
#
    if ($verboseLevel ge "1") {
	$now=time();
	$Date= &ctime($now);
	$Date=substr($Date,11,8);    
	printf("%s Done.   Total elapsed time for this file = %d seconds.\n", 
	       $Date, $now-$start2); 
    }
} # end foreach $fileName (@fileNames)


if ($verboseLevel ge "1") {
    $Date= &ctime(time());
    $Date=substr($Date,11,8);
    printf("%s Ending the session with the SRB server (Sexit)\n",
	   $Date); 
}
$cmd = "$SEXIT";
if ($verboseLevel ge "2") {
    print "running: " . "$cmd"."\n";
}
`$cmd`;
$cmdStat=$?;
if ($cmdStat!=0) {
    print "The Sexit command failed, check your SRB environment.";
    die("can't Sexit");
}
#

if ($fileCount eq "0") {
    usage();
}
else {
    if ($verboseLevel ge "1" ) {
	printf(
	"%s Done.   Total elapsed time for all = %d seconds.\n", 
	       $Date, $now-$start1); 
    }
}


sub
usage()
{
    printf("usage: Sget.pl [-h] [-v] [-q] files(s)\n");
    printf("-h help\n");
    printf("-v verbose\n");
    printf("-q quiet\n");
    printf("See http://www.npaci.edu/dice/srb/SecureAndOrCompressedData.html\n");
}
