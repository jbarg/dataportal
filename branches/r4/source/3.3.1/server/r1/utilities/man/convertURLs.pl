#!/usr/bin/perl

# This is run with contents of an html file piped in (a man2html file),
# and modifies a few URLs and writes the whole thing to stdout.

while ( $line=<STDIN> ) {

# Change the cgi-bin form URLs to the ones we use;
# These are the links to other Scommand pages.
    $p = index($line, "localhost/cgi-bin/man/man2html?1+", 0);
    if ($p > 0 ) {
	$len = length("localhost/cgi-bin/man/man2html?1+");
        substr($line, $p, $len)="www.sdsc.edu/srb/srbcommands/";
        $p2 = rindex($line, '"');
	substr($line, $p2, 0)=".1.html";
    }

# Change the "Main Contents" URL to the one we use
    $p3 = index($line, "http://localhost/cgi-bin/man/man2html", 0);
    if ($p3 > 0) {
	$len = length("http://localhost/cgi-bin/man/man2html");
        substr($line, $p3, $len)="http://www.sdsc.edu/srb/srbcommands.html";
    }

# Now echo out either the input or modified line
    print $line;
}
