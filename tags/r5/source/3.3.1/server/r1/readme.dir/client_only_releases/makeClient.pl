#!/usr/bin/perl

# This script makes a client release tar file.
# Unlike the previous version, this one starts with a full release tree
# structure, and creates the client tar file from that.  Just cd
# to the readme.dir/client_only_releases subdirectory, and run
# this script: 'makeClient.pl'
#
# If you want, you can change the two parameters below:
$WORKING_DIR="/tmp/clientrel";  # the subdir where files will me created
$CLIENT_TAR_FILE_NAME="client.tar"; 
#
# The name of the SRB top directory (two levels up) will be the one
# stored in the tar file.  Rename it before running this, if you wish.

$startDir=`pwd`;
chomp($startDir);

$readmeIndex=index($startDir,"readme.dir/client_only_releases");
if ($readmeIndex lt 0) {
    die ("can't find readme.dir/client_only_releases; must run from that dir");
}

$topPartOfDir=substr($startDir,0,$readmeIndex-1);
$i=rindex($topPartOfDir,"/");
$topDir=substr($topPartOfDir,$i+1);   # the name of the top dir of the release

chdir("../..");

`mv $startDir/README .`;      # Move (to keep timestamp) the README file
`mv configure configure.normal.release.version`;  # mv regular config
`mv $startDir/configure .`;   # Move client-only configure into place
`mv Makefile.in Makefile.in.normal.release.version`;  # mv regular Makefile.in
`mv $startDir/Makefile.in .`; # Move client-only Makefile.in

chdir("..");

mkdir("$WORKING_DIR");  # make it if not there

# Make tarList2 with the correct topDir name:
unlink("$WORKING_DIR/tarList2");
`cat $startDir/tarList | sed s/TOP_DIR/$topDir/g > $WORKING_DIR/tarList2`;

# Run tar, specifying the list of dirs and files to include:
`tar cf $WORKING_DIR/$CLIENT_TAR_FILE_NAME --files-from=$WORKING_DIR/tarList2`;

# Make tarDelList2 with the correct topDir name:
unlink("$WORKING_DIR/tarDelList2");
`cat $startDir/tarDelList | sed s/TOP_DIR/$topDir/g > $WORKING_DIR/tarDelList2`;

# Run tar to delete a few directories and files:
`tar f $WORKING_DIR/$CLIENT_TAR_FILE_NAME --delete --files-from=$WORKING_DIR/tarDelList2`;

# Undo the moves
`mv $topPartOfDir/README $startDir`;
`mv $topPartOfDir/configure $startDir`;
`mv $topPartOfDir/configure.normal.release.version $topPartOfDir/configure`;
`mv $topPartOfDir/Makefile.in $startDir`;
`mv $topPartOfDir/Makefile.in.normal.release.version $topPartOfDir/Makefile.in`;

exit();
