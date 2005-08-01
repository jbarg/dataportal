#!/usr/bin/php -q

<?php
require ("../config.inc");
require ($dbcp_path . "web-connect.inc");
require ("stats-bar-graph.php");
require ("stats-pie-graph.php");

$today = date("F j, Y");
$sqltoday = date("Y-m-d");
$lastyear = date("Y", mktime(0,0,0,1,1,date("Y")-1));
$month = date("m");
$MAIL = "";
$MAILSUBJECT = "SRB Stats";

//Arrays for all statistics
$dates = array();
$u_fcount_hist = array();
$fcount_hist = array();
$u_fsize_hist = array();
$fsize_hist = array();
$users_hist = array();
$resources = array();
$r_names = array();
$r_fcount = array();
$r_fsize = array();
$domains = array();
$d_names = array();
$d_fcount = array();
$d_fsize = array();

//Query and execute to get file and size growth history for last
//12 months
for($i=1; $i < 13; $i++) {
  $get_month = date("Y-m", mktime(0,0,0,$month + $i,1,$lastyear));
  
  $query = "select max(day) from general_stats where
  day like '$get_month%'";
  $result = dbx_query($web_db, $query);
  $qdate = $result->data[0][0];

  $query = "select * from
  general_stats where day = '$qdate'";
  $result = dbx_query($web_db, $query);

  array_push($dates, date("M-y", strtotime($result->data[0][0])));
  array_push($users_hist, $result->data[0][1]);
  array_push($u_fcount_hist, $result->data[0][2]/1000);
  array_push($fcount_hist, $result->data[0][3]/1000);
  array_push($u_fsize_hist, $result->data[0][4]);
  array_push($fsize_hist, $result->data[0][5]);
}

$MAIL .= "SRB Statistics,\n";
$MAIL .= date("D M j G:i:s T Y\n");
$MAIL .= "=======================================================\n";
$MAIL .= "\n\n\n";

$MAIL .= "Total number of Users\n";
$MAIL .= "-------------------------------------------------------\n";

$num_user=$result->data[0][1];
$MAIL .= $num_user . "\n";

$MAIL .= "\nTotal number and size (GB) of Unique Files\n";
$MAIL .= "-------------------------------------------------------\n";

$u_num_file=$result->data[0][2];
$u_gig_file=$result->data[0][4];
$MAIL .= str_pad(number_format($u_num_file),22);
$MAIL .= number_format($u_gig_file, 2) . "\n";

$MAIL .= "\nTotal number and size (GB) of ALL Files (w/ replicas)\n";
$MAIL .= "-------------------------------------------------------\n";

$num_file=$result->data[0][3];
$gig_file=$result->data[0][5];
$MAIL .= str_pad(number_format($num_file),22);
$MAIL .= number_format($gig_file, 2) . "\n";




$MAIL .= "\n\nResource              Number of Files     Gigabytes\n";
$MAIL .= "--------------------- ------------------- -------------\n";

//Query and execute to get file and size count for resources
$query = "select resource, fc, fs from resource_stats a,
logical_resources b where a.day = '$sqltoday' and
a.id = b.id group by resource"; 
$result = dbx_query($web_db, $query);

for($i=0; $i < $result->rows; $i++) {
   $rname = $result->data[$i][0];
   $rcount = $result->data[$i][1];
   $rsize = $result->data[$i][2];

   //Add new Resource element
   $resources[$rname] = array( 'count' => $rcount, 'size' => $rsize );
}

function resource_print ($vals, $key) {
    global $MAIL, $r_names, $r_fcount, $r_fsize;
    $MAIL .= str_pad( $key, 22 ) .
         str_pad( number_format($vals['count']), 20 ) .
         str_pad( number_format($vals['size'], 2), 13 ) . "\n";
    array_push( $r_names, $key . " (" . number_format($vals['count']) . ")" );
    array_push( $r_fcount, $vals['count'] );
    array_push( $r_fsize, $vals['size'] );
}

function domain_print ($vals, $key) {
    global $MAIL, $d_names, $d_fcount, $d_fsize;
    $MAIL .= str_pad( $key, 22 ) .
         str_pad( number_format($vals['count']), 20 ) .
         str_pad( number_format($vals['size'], 2), 13 ) . "\n";
    array_push( $d_names, $key . " (" . number_format($vals['count']) . ")" );
    array_push( $d_fcount, $vals['count'] );
    array_push( $d_fsize, $vals['size'] );
}


array_walk ($resources, 'resource_print');

$MAIL .= "\n\nDomain                Number of Files     Gigabytes\n";
$MAIL .= "--------------------- ------------------- -------------\n";

//Query and execute to get file and size count for domains
$query = "select domain, fc, fs from domain_stats a,
domains b where a.day = '$sqltoday' and
a.id = b.id group by domain";
$result = dbx_query($web_db, $query);

for($i=0; $i < $result->rows; $i++) {
   $dname = $result->data[$i][0];
   $dcount = $result->data[$i][1];
   $dsize = $result->data[$i][2];
                                                                                                                  
   //Add new Domain element
   $domains[$dname] = array( 'count' => $dcount, 'size' => $dsize );
}

array_walk ($domains, 'domain_print');

$MAIL = "Subject: $MAILSUBJECT\nTo: $stats_email_list\n\n$MAIL";
exec("/usr/sbin/sendmail -t << EOT\n$MAIL\nEOT");

sizegraph($dates, $fcount_hist, false, "Total Number of Files in Thousands\n" . number_format($num_file) . " Files",
          $today, $srb_web_path . "SRB_File_Growth.png");
sizegraph($dates, $fsize_hist, false, "Total Size of Storage in Gigabytes", $today, $srb_web_path . "SRB_Size_Growth.png");
sizegraph($dates, $users_hist, false, "Number of User Accounts", $today, $srb_web_path . "SRB_User_Growth.png");

piegraph($r_names, number_format($num_file), number_format($gig_file, 2), $r_fcount, $r_fsize, "Storage Breakdown by Resource",
         $today, $srb_web_path . "SRB_Storage_by_Resource.png");
piegraph($d_names, number_format($num_file), number_format($gig_file, 2), $d_fcount, $d_fsize, "Storage Breakdown by Domain",
         $today, $srb_web_path . "SRB_Storage_by_Domain.png");

dbx_close($web_db);

?>

