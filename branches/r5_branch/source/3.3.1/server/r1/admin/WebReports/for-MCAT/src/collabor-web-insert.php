#!/usr/bin/php -q

<?php
################################################################################
## install-srbStats.mysql
##   by Roman Olschanowsky (c) 2004
##
## Used to build scheme for srbStats database
##
################################################################################
// Connect to both databases
require ("../config.inc");
require ($dbcp_path . "mcat-connect.inc");
require ($dbcp_path . "web-connect.inc");

// Variables
$domains = array();

// Setup date info
$lastmonth = mktime (0,0,0,date("m")-1,1,date("Y"));
$QueryMonth = date("Y-m", $lastmonth);
$iQueryMonth = date("Y-m-d", $lastmonth);

// Get all SRB Domains we are interested in
$query = "select id from domains"; 
$result = dbx_query($web_db, $query);
for($i=0; $i < $result->rows; $i++) {
   for($j=0; $j < $result->cols; $j++) {
       array_push($domains, $result->data[$i][$j]);
   }
}

// Do the n by n matrix of collaboration stats
for($i=0; $i < count($domains); $i++){
  $efc = 0;
  $ifc = 0;
  $efs = 0;
  $ifs = 0;

  for($j=0; $j < count($domains); $j++){
    $data = getShared($mcat_db, $domains[$i], $domains[$j], $QueryMonth);

    insertSiteCollab($web_db, $domains[$i], $domains[$j], $data[0], $data[1], $iQueryMonth);

    // File count data for collaboration totals
    if($i != $j)
      $efc += $data[0];
    else
      $ifc = $data[0];

    // File size data for collaboration totals
    if($i != $j)
      $efs += $data[1];
    else
      $ifs += $data[1];
  }

  insertTotCollab($web_db, $domains[$i], $efc, $ifc, $efs, $ifs, $iQueryMonth);
}

dbx_close($web_db);
dbx_close($mcat_db);

function getShared($mcat_db, $reader, $owner, $QueryMonth) {
  $site_stat = array();
  // Get file count and data size in MegaBytes.
  // First query is on files still active (mdas_ad_repl)
  $query = "select count(a.dataid),
            sum(b.data_size)/1048576
            from
            mdas_audit a, mdas_ad_repl b,
            mdas_au_domn c, mdas_au_domn d
            where
            a.aud_timestamp like '$QueryMonth%' and

            /* The actions wanted */
            a.actionid = '96' and

            /* Making sure it's not the owner of the file */
            a.dataid = b.data_id and
            a.userid <> b.data_owner and

            /* Getting the domain of the owner */
            b.data_owner = c.user_id and
            c.domain_id = '$owner' and

            /* Getting the domain of the reader */
            a.userid = d.user_id and
            d.domain_id = '$reader'";

  $result1 = dbx_query($mcat_db, $query);

  // Second query is on files that have been since deleted (mdas_ad_del_repl)
  $query = "select count(a.dataid),
            sum(b.data_size)/1048576
            from
            mdas_audit a, mdas_ad_del_repl b,
            mdas_au_domn c, mdas_au_domn d
            where
            a.aud_timestamp like '$QueryMonth%' and

            /* The actions wanted */
            a.actionid = '96' and

            /* Making sure it's not the owner of the file */
            a.dataid = b.data_id and
            a.userid <> b.data_owner and

            /* Getting the domain of the owner */
            b.data_owner = c.user_id and
            c.domain_id = '$owner' and

            /* Getting the domain of the reader */
            a.userid = d.user_id and
            d.domain_id = '$reader'";

  $result2 = dbx_query($mcat_db, $query);

  // Add results for whole picture
  array_push($site_stat, $result1->data[0][0] + $result2->data[0][0]);
  array_push($site_stat, $result1->data[0][1] + $result2->data[0][1]);

  return $site_stat;
}

function insertSiteCollab($web_db, $reader, $owner, $fileCount, $fileSize, $QueryMonth) {
  $query = "insert into collaboration values('$QueryMonth', '$reader', '$owner', '$fileCount', '$fileSize')";

   dbx_query($web_db, $query);
}

function insertTotCollab($web_db, $reader, $extern_fc, $intern_fc, $extern_fs, $intern_fs, $QueryMonth) {
  $query = "insert into collab_totals values('$QueryMonth', '$reader', '$extern_fc', '$intern_fc',
            '$extern_fs', '$intern_fs')";

   dbx_query($web_db, $query);
}

?>
