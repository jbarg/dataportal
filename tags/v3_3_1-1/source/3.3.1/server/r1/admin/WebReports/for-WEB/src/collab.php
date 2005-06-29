#!/usr/bin/php -q

<?php
require ("../config.inc");
require ($dbcp_path . "web-connect.inc");
require ("collab-graph.php");
require ("stats-bar-graph.php");

//Setup date info
$lastmonth = mktime (0,0,0,date("m")-1,1,date("Y"));
$Qmonth = date("Y-m-d", $lastmonth);
$Lmonth = date("F Y", $lastmonth);

//Variables
$dates = array();
$collab_hist = array();
$collab_delta = array();
$c_h = 0;
$c_d = 0;

//Get the collaboration history
$query = "select month, sum(extern_fc), sum(intern_fc)
          from collab_totals group by month";
$result = dbx_query($web_db, $query);

for($i=0; $i < $result->rows; $i++) {
  //Get and convert date to pretty format, also get file count
  $db_date = explode("-",$result->data[$i][0]);
  $c_d = $result->data[$i][1] + $result->data[$i][2];
  $c_h += $c_d;
  $new_date = mktime (0,0,0,$db_date[1],1,$db_date[0]);
  array_push( $dates, date("M-y", $new_date));
  array_push( $collab_delta, $c_d);
  array_push( $collab_hist, $c_h);
}

//We are only interest in a 10 month period
$dates = array_slice($dates, -10);
$collab_hist = array_slice($collab_hist, -10);
$collab_delta = array_slice($collab_delta, -10);

// Two loops, to make two tables, first table all dates, second is current month
for($t=0; $t < 2; $t++){
  $domains = array();
  $datazero = array();
  $totfilecount = array();
  $locfilecount = array();
  $totfileavg = array();
  $locfileavg = array();

  //Query and execute to get all Data
  if($t == 0) {
    $query = "select domain, sum(extern_fc), sum(intern_fc),
              sum(extern_fs), sum(intern_fs)
              from domains, collab_totals
              where site_id = id group by domain";
  } else {
    $query = "select domain, extern_fc, intern_fc,
              extern_fs, intern_fs
              from domains, collab_totals
              where site_id = id and
              month = '$Qmonth' group by domain";
  }
  $result = dbx_query($web_db, $query);

  for($i=0; $i < $result->rows; $i++) {
    array_push( $domains, trim($result->data[$i][0]));
    array_push( $totfilecount, $result->data[$i][1]);
    array_push( $locfilecount, $result->data[$i][2]);

    $temp1 = $result->data[$i][3];
    $temp2 = $result->data[$i][4];

    if($result->data[$i][1] == 0)
      array_push( $totfileavg, 0);
    else
      array_push( $totfileavg, $temp1/$result->data[$i][1]);

    if($result->data[$i][2] == 0)
      array_push( $locfileavg, 0);
    else
      array_push( $locfileavg, $temp2/$result->data[$i][2]);

    array_push( $datazero, 0 );
  }

  if($t == 0) {
    sharegraph($domains, $totfilecount, $totfileavg, $datazero, "Accessing Other Sites",
               "Accumulative (March 2003 to " . $Lmonth . " )", $srb_web_path . "SRB_Total_Sharing.png");
    sharegraph($domains, $locfilecount, $locfileavg, $datazero, "Access Within Site",
               "Accumulative (March 2003 to " . $Lmonth . " )", $srb_web_path . "SRB_Local_Sharing.png");
  } else {
    sharegraph($domains, $totfilecount, $totfileavg, $datazero, "Accessing Other Sites",
               $Lmonth, $srb_web_path . "SRB_Total_Sharing_Month.png");
    sharegraph($domains, $locfilecount, $locfileavg, $datazero, "Access Within Site",
               $Lmonth, $srb_web_path . "SRB_Local_Sharing_Month.png");
  }
}

sizegraph($dates, $collab_hist, false, "Collaboration History\n", $Lmonth, $srb_web_path . "SRB_Collab_Hist.png");
sizegraph($dates, $collab_delta, false, "Collaboration Delta\n", $Lmonth, $srb_web_path . "SRB_Collab_Change.png");

dbx_close($web_db);

?>
