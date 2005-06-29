#!/usr/bin/php -q
<?php
################################################################################
## srb-failover.php
##   by Roman Olschanowsky 2004
##
##   SRB Failover daemon
##
##   Using a website you can failover the SRB data grid to use a different
##   MCAT node. You can also stop and restart the data grid.
##
##   Triggers for value on website:
##     mcat_stat equal to primary mcat: Do nothing or return to normal state
##     mcat_stat equal to secondary mcat: Failover to secondary mcat
##     mcat_stat equal to "reset": Fail back to primary mcat, also restart
##     mcat_stat equal to "stop": Stop server ( planned outage )
##     mcat_stat equal to "change:mcat1/mcat2": Change configuration of mcats
################################################################################

require ("config.inc");

################################################################################
# Global Variables
################################################################################
$p_file = "primary.mcat";
$s_file = "secondary.mcat";
$log = "message.log";
$date = trim( date("m/d/Y H:i:s") );


################################################################################
# MAIN
################################################################################
firstboot( $first_url );

$primary_mcat = trim( file_get_contents( $p_file ) );
$secondary_mcat = trim( file_get_contents( $s_file ) );

# Get MCAT status
@ $p = fopen( $mcat_url, "r" );
if($p) {
  $mcat_stat = trim( fgets($p) );
  fclose($p);
}
else { exit; }

# Logic and Operations to perform based on mcat status
# Check to see if database has failed over to secondary
if( $mcat_stat == $secondary_mcat ) {
  # Check to see if this server has already switched over, if flag do nothing
  if( ! file_exists("hasFailed") ) {
    # Kill and restart
    failoverServer( $secondary_mcat );
    exec( "touch hasFailed" );
    exec( "echo \"Failed Over: $secondary_mcat $date\" >> $log" );
  }
}
# Check to see if someone has manually reset database to primary
else if( $mcat_stat == "reset" ) {
  # Check to see if this server has already switched back, if flag do nothing
  if( ! file_exists("hasReset") ) {
    # Kill and restart
    failoverServer( $primary_mcat );
    exec( "touch hasReset" );
    exec( "echo \"Reset: $primary_mcat $date\" >> $log" );
  }
}
# Check to see if someone has manually issued stop command
else if( $mcat_stat == "stop" ) {
  # Kill
  exec( "cd $srb_bin_dir; ./killsrb now" );
  exec( "echo \"Stopped: $date\" >> $log" );
}
# Check to see if it's back to normal
else if( ($mcat_stat == $primary_mcat) &&
         (file_exists("hasReset") || file_exists("hasChanged")) ) {
  exec( "\\rm -f hasFailed" );
  exec( "\\rm -f hasReset" );
  exec( "\\rm -f hasChanged" );
  exec( "echo \"Normal: $date\" >> $log" );
}
# Check to see if mcat defaults for primary and secondary have changed
# Example of valid change command: "change:mcat1.sdsc.edu/mcat2.sdsc.edu"
else if( strstr($mcat_stat, "change" ) ) {
  # Check to see if this server has already changed, if flag do nothing
  if( ! file_exists("hasChanged") ) {
    $temp = explode( ":", $mcat_stat );
    $temp = explode( "/", $temp[1] );
    exec("echo $temp[0] > $p_file" );
    exec("echo $temp[1] > $s_file" );
    exec( "touch hasChanged" );
    exec( "echo \"MCAT Change: $temp[0]/$temp[1] $date\" >> $log" );
    failoverServer( $temp[0] );
    exec( "echo \"Changed Over: $temp[0] $date\" >> $log" );
  }
}

################################################################################
# Function for MCAT failover
################################################################################
function failoverServer( $mcat ) {
    global $srb_bin_dir, $srb_data_dir;
    exec( "cd $srb_bin_dir; ./killsrb now" );
    sleep(5);
    $f = fopen( $srb_data_dir . "mcatHost", "w" );
    fwrite( $f, $mcat . "\n" );
    fwrite( $f, "ENCRYPT1\n" );
    fwrite( $f, "/C=US/O=NPACI/O=DICE/UID=srb/CN=Storage Resource Broker/Email=srb@sdsc.edu\n" );
    fclose( $f);
    exec( "cd $srb_bin_dir; ./runsrb" );
}


################################################################################
# Function for first boot to set primary and secondary mcat config files
################################################################################
function firstboot( $first_url ) {
  global $p_file, $s_file, $log, $date;

  if ( (! file_exists($p_file)) || (! file_exists($s_file)) ) {
    @ $p = fopen( $first_url, "r" );

    if ($p) {
      $result = explode( "/", trim(fgets($p)) );
      exec( "echo $result[0] > $p_file" );
      exec( "echo $result[1] > $s_file" );
      exec( "echo \"First boot: $date\" >> $log" );
      exec( "echo \"primary: $result[0]\" >> $log" );
      exec( "echo \"secondary: $result[1]\" >> $log" );
      exec( "echo \"-------------------------------\n\" >> $log" );
    }

    exit;
  }
}

?>

