#!/usr/bin/perl
#
# This script stores files into the SRB, either compressed, encrypted or both.
# See usage at tail of this.

# Defines you may need to change:
$OPENSSL="openssl";
$GZIP="gzip";
$SINIT="Sinit";
$SPUT="Sput";
$SMODD_ENC="SmodD -Y";
$SMODD_COM="SmodD -z";
$SEXIT="Sexit";
# You may want to set the full paths, for example:
# $OPENSSL="/usr/local/apps/nmi-2.1/bin/openssl";
# $GZIP="/bin/gzip";
# $SPUT="/scratch/slocal/SRB2a/SRB2/utilities/bin/Sput";
# $SMODD_ENC="/scratch/slocal/SRB2a/SRB2/utilities/bin/SmodD -Y";
# $SMODD_COM="/scratch/slocal/SRB2a/SRB2/utilities/bin/SmodD -z";
#
#Other defines:
$TEMPFILE="/tmp/%t4temp4e877f6";
$FILE_KEY_LENGTH=21;
$ALLOWED_ENCRYPTION_TYPES="rc5 rc5-cbc rc4 bf none";
$ALLOWED_COMPRESSION_TYPES="gzip gzip-1 gzip-9 none";
$ENCRYPT_DEFAULT="bf";
$COMPRESS_DEFAULT="gzip";
$FIELD_DELIMITER=":";

require "ctime.pl";

$start1=time();
#
# Parse command line and do some checking
#
$verboseLevel=1;
$fileNames="";
$encryptType=$ENCRYPT_DEFAULT;
$compressType=$COMPRESS_DEFAULT;
for ($i=0; $i<=$#ARGV; $i++) {
   if ($ARGV[$i] eq "-c") {
       $i++;
       $compressType = $ARGV[$i];
   }
   else {
       if ($ARGV[$i] eq "-e") {
	   $i++;
	   $encryptType = $ARGV[$i];
       }
       else {
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
   }
}

if ($verboseLevel ge "2") {
    printf ("compressType = %s\n", $compressType);
    printf ("encryptType = %s\n", $encryptType);
    printf ("fileNames = %s\n", $fileNames);
}

$OK = 0;
@eTypes = split(/ /,$ALLOWED_ENCRYPTION_TYPES);
foreach $eType (@eTypes) {
    if ($eType eq $encryptType) {
	$OK = 1;
    }
}
if ($OK==0) {
    die($encryptType . " is not a supported encryption type, quiting");
}
$encrypt=1;
if ($encryptType eq "none") {
    $encrypt=0;
}

$OK = 0;
@cTypes = split(/ /,$ALLOWED_COMPRESSION_TYPES);
foreach $cType (@cTypes) {
    if ($cType eq $compressType) {
	$OK = 1;
    }
}
if ($OK==0) {
    die($compressionType . " is not a supported compression type, quiting");
}
$compress=1;
if ($compressType eq "none") {
    $compress=0;
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
    if (!-e $fileName) {
	printf("Input file %s does not exist\n", $fileName);
	usage();
	die("No input file\n");
    }
    $fileCount++;
    $fileNameOriginal = $fileName;
    $sizeOriginal = -s $fileName;


#
# Get MD5 hash of original file
#
    if ($verboseLevel ge "1") {
	$Date= &ctime(time());
	$Date=substr($Date,11,8);
	printf("%s Computing MD5 hash value for %s\n", $Date, $fileName); 
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

#
# Compress the file
#
    if ($compress) {
	if ($verboseLevel ge "1") {
	    $Date= &ctime(time());
	    $Date=substr($Date,11,8);
	    printf("%s Compressing\n", $Date); 
	}
	
	if ($compressType eq "gzip") {
	    $cmd="$GZIP";
	    $nextFile = $fileName . ".gz";
	}
	if ($compressType eq "gzip-1") {
	    $cmd="$GZIP -1";
	    $nextFile = $fileName . ".gz";
	}
	if ($compressType eq "gzip-9") {
	    $cmd="$GZIP -9";
	    $nextFile = $fileName . ".gz";
	}
	$cmd = $cmd . " -c " . $fileName . " > " . $nextFile;
	if ($verboseLevel ge "2") {
	    print "running: " . "$cmd"."\n";
	}
	`$cmd`;
	$cmdStat=$?;
	if ($cmdStat!=0) {
	    print "The following command failed:";
	    print "$cmd"."\n";
	    die("cant compress the input file");
	}
	if ($verboseLevel ge "1") {
	    $sizeIn = -s $fileName;
	    $sizeOut = -s $nextFile;
	    $ratio = $sizeOut/$sizeIn;
	    $ratio = substr($ratio, 0, 5);
	    print "         Input file was $sizeIn bytes, output $sizeOut, ratio: $ratio\n"; 
	}
	$fileNameCompressed=$nextFile;
	$fileName = $nextFile;
    }

    if ($encrypt) {
#
# Generate a Random value, and remove special characters (leaving 
#  just a-z, A-Z, and 0-9).
#

	if ($verboseLevel ge "1") {
	    $Date= &ctime(time());
	    $Date=substr($Date,11,8);
	    printf("%s Encrypting\n", $Date); 
	}

	$rand_buf=`$OPENSSL rand -base64 32`;
	if ($? != 0) {
	    die "cant generate random number (OpenSSL not available?)";
	}
	$_ = $rand_buf;
	s/-//g;
	s/\+//g;
	s\/\\g;
	s/=//g;

	$rand_buf2=$_;
	$password=substr($rand_buf2, 3, $FILE_KEY_LENGTH);
	if ($verboseLevel gt "2") {
	    printf("rand =%s\n",$rand_buf);
	    printf("rand2=%s\n",$rand_buf2);
	}
	if ($verboseLevel ge "2") {
	    printf("         File encryption key=%s\n",$password);
	}

#   Write the file encryption key (password) into a file as the password
#   (more secure than having it on the command line (i.e. for 
#    openssl)).

	unlink($TEMPFILE);
	open(F, ">".$TEMPFILE) || die("cant open temp file for writing ".$TEMPFILE);
	printf F $password;
	close(F);

#
#   Run the openssl encryption command
#
	$outFile=$fileName . "." . $encryptType;
	$cmd="$OPENSSL enc -$encryptType -pass file:$TEMPFILE -in $fileName -out $outFile";
	if ($verboseLevel ge "2") {
	    print "running: " . "$cmd"."\n";
	}
	`$cmd`;
	$cmdStat=$?;
	if ($cmdStat!=0) {
	    print "The following command failed:";
	    print "$cmd"."\n";
	    die("cant encrypt the input file");
	}
	if ($verboseLevel ge "1") {
	    $sizeIn = -s $fileName;
	    $sizeOut = -s $outFile;
	    print "         Input file was $sizeIn bytes, output $sizeOut\n"; 
	}
	$fileName = $outFile;

#
#   Remove the TEMPFILE
#
	unlink($TEMPFILE);

}

#
# Put the file
#
    if ($fileName ne $fileNameOriginal) {
	rename($fileNameOriginal, $fileNameOriginal . ".original");
	rename($fileName, $fileNameOriginal);
	$fileName = $fileNameOriginal;
    }
    if ($verboseLevel ge "1") {
	$Date= &ctime(time());
	$Date=substr($Date,11,8);
	printf("%s Storing file %s\n", $Date, $fileName); 
    }
    $cmd = "$SPUT" . " -f " .  $fileName;
    if ($verboseLevel ge "2") {
	print "running: " . "$cmd"."\n";
    }
    `$cmd`;
    $cmdStat=$?;
    if ($cmdStat!=0) {
	print "The following command failed:";
	print "$cmd"."\n";
	die("cant sput the file");
    }

#
# Store the key
#
    if ($encrypt) {
	if ($verboseLevel ge "1") {
	    $Date= &ctime(time());
	    $Date=substr($Date,11,8);
	    printf("%s Saving the encryption key for %s\n", $Date, $fileName); 
    }
#    $cmd = "$SMODD_ENC" . " ".  $password . "$FIELD_DELIMITER" . " " . $fileName;
	$cmd = "$SMODD_ENC" . " ".  $password . " " . $fileName;
	if ($verboseLevel ge "2") {
	    print "running: " . "$cmd"."\n";
	}
	`$cmd`;
	$cmdStat=$?;
	if ($cmdStat!=0) {
	    print "The following command failed:";
	    print "$cmd"."\n";
	    die("cant save the encryption key");
	}
    }

#
# Store the file characteristics
#
    if ($verboseLevel ge "1") {
	$Date= &ctime(time());
	$Date=substr($Date,11,8);
	printf("%s Saving the attributes for %s\n", $Date, $fileName); 
    }
    $attr="MD5=".$MD5Value . "$FIELD_DELIMITER";
    if ($compress) {
	$attr = $attr . "Compress=". $compressType . "$FIELD_DELIMITER";
    }
    if ($encrypt) {
	$attr = $attr . "Encrypt=". $encryptType . "$FIELD_DELIMITER";
    }
    $attr = $attr . "OriginalSize=" . $sizeOriginal . "$FIELD_DELIMITER";

    $cmd = "$SMODD_COM" . " ".  $attr . " " . $fileName;
    if ($verboseLevel ge "2") {
	print "running: " . "$cmd"."\n";
    }
    `$cmd`;
    $cmdStat=$?;
    if ($cmdStat!=0) {
	print "The following command failed:";
	print "$cmd"."\n";
	die("cant save the file attributes");
    }

#
# Clean up
#
    if ($compress && $encrypt) {
# when doing both compression and encryption, have a second intermediate file
	unlink($fileNameCompressed);
    }

    if ($compress || $encrypt) {
# when encrypting or compressing, remove result file and put back the original
	unlink($fileName);
	rename($fileNameOriginal . ".original" , $fileNameOriginal);
    }


#
# Print a done message with timestamp
#
    if ($verboseLevel ge "1") {
	$now = time();
	$Date= &ctime($now);
	$Date=substr($Date,11,8);
	printf("%s Done.   Total elapsed time for this file = %d seconds.\n", 
	       $Date, $now-$start2); 
    }


} # end of foreach $fileName (@fileNames)


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
    if ($verboseLevel ge "1") {
	$now = time();
	$Date= &ctime($now);
	$Date=substr($Date,11,8);
	printf("%s Done.   Total elapsed time for all = %d seconds.\n",
	       $Date, $now-$start1); 
    }
}



sub
usage()
{
    printf("usage: Sput.pl [-h] [-v] [-q] [-c compress_type] [-e encrypt_type] file(s)\n");
    printf("Allowed encryption types are %s\n", $ALLOWED_ENCRYPTION_TYPES);
    printf("Allowed compression types are %s\n", $ALLOWED_COMPRESSION_TYPES);
    printf("By default, encryption is: %s\n", $ENCRYPT_DEFAULT);
    printf("By default, compression is: %s\n", $COMPRESS_DEFAULT);
    printf("-h help\n");
    printf("-v verbose\n");
    printf("-q quiet\n");
    printf("See http://www.npaci.edu/dice/srb/SecureAndOrCompressedData.html\n");
}
