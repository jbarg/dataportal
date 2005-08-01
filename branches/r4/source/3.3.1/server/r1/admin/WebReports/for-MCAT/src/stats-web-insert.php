#!/usr/bin/php -q

<?php
require ("../config.inc");
require ($dbcp_path . "mcat-connect.inc");
require ($dbcp_path . "web-connect.inc");

$today = date("Y-m-d");

//Arrays for all statistics
$resources = array();
$domains = array();

//Other vars
$tot_fc = 0;
$tot_fs = 0.0;

//Query and execute to get total num of users
$query = "select count(user_id)
from mdas_cd_user where user_typ_id != '0001.0001.0001.0001.0005'";
$result = dbx_query($mcat_db, $query);

$num_user=$result->data[0][0];

//Query and execute to get total files
$query = "select count(data_id)
from mdas_ad_repl where repl_enum < 1";
$result = dbx_query($mcat_db, $query);

$num_file=$result->data[0][0];

//Query and execute to get total files
$query = "select sum(data_size)
from mdas_ad_repl where
data_id = container_id and
repl_enum < 1";
$result = dbx_query($mcat_db, $query);

$gig_file=$result->data[0][0]/1073741824;

//Query and execute to get file and size count for resources
$query = "select a.rsrc_id,
count(data_id),
sum(data_size)
from mdas_ar_repl a, mdas_ad_repl b
where
b.rsrc_id = a.phy_rsrc_id and
a.phy_rsrc_id <> a.rsrc_id
group by a.rsrc_id"; 
$result = dbx_query($mcat_db, $query);

for($i=0; $i < $result->rows; $i++) {
   $rname = $result->data[$i][0];
   $rcount = $result->data[$i][1];
   $rsize = $result->data[$i][2];

   //Add to total file count and size
   $tot_fc += $rcount;
   $tot_fs += $rsize;

   //Add new Resource element
   $resources[$rname] = array( 'count' => $rcount, 'size' => $rsize );
}

//Query and execute to get file and size for containers on resources
$query = "select a.rsrc_id,
count(b.data_id)
from mdas_ar_repl a, mdas_ad_repl b, mdas_ad_repl c
where
b.rsrc_id = '-1' and
b.container_id = c.data_id and
c.rsrc_id = a.phy_rsrc_id and
a.phy_rsrc_id <> a.rsrc_id
group by a.rsrc_id";
$result = dbx_query($mcat_db, $query);
 
for($i=0; $i < $result->rows; $i++) {
   $rname = $result->data[$i][0];
   $rcount = $result->data[$i][1];

   //Add to total file count and size
   $tot_fc += $rcount;

   //Add to existing Resource elements
   $resources[$rname]['count'] += $rcount;
}

//Query and execute to get file and size count for domains
$query = "select c.domain_id,
count(data_id),
sum(data_size)
from mdas_ad_repl a, mdas_au_domn b, mdas_td_domn c
where
a.rsrc_id (+) <> '-1' and
a.data_owner (+) = b.user_id and
b.domain_id (+) = c.domain_id and
c.domain_desc <> 'groups' and
c.domain_desc <> 'npaci' and
c.domain_desc <> 'home' and
c.domain_desc <> 'sdsc' and
c.domain_desc <> 'gen-lvl1' and
c.domain_desc <> 'gen-lvl2' and
c.domain_desc <> 'gen-lvl3' and
c.domain_desc <> 'gen-lvl4'
group by c.domain_id";
$result = dbx_query($mcat_db, $query);

for($i=0; $i < $result->rows; $i++) {
   $dname = $result->data[$i][0];
   $dcount = $result->data[$i][1];
   $dsize = $result->data[$i][2];
                                                                                                                  
   //Add new Domain element
   $domains[$dname] = array( 'count' => $dcount, 'size' => $dsize );
}

//Query and execute to get file and size for containers for domains
$query = "select c.domain_id,
count(data_id)
from mdas_ad_repl a, mdas_au_domn b, mdas_td_domn c
where
a.rsrc_id (+) = '-1' and
a.data_owner (+) = b.user_id and
b.domain_id (+) = c.domain_id and
c.domain_desc <> 'birn' and
c.domain_desc <> 'groups' and
c.domain_desc <> 'npaci' and
c.domain_desc <> 'home' and
c.domain_desc <> 'gen-lvl1' and
c.domain_desc <> 'gen-lvl2' and
c.domain_desc <> 'gen-lvl3' and
c.domain_desc <> 'gen-lvl4'
group by c.domain_id";
$result = dbx_query($mcat_db, $query);

for($i=0; $i < $result->rows; $i++) {
   $dname = $result->data[$i][0];
   $dcount = $result->data[$i][1];

   //Add to existing Domain element
   $domains[$dname]['count'] += $dcount;
}

//Now add all data to Web database
//Adding to general_stats
$tot_fs /= 1073741824;
$query = "insert into general_stats values('$today', '$num_user',
          '$num_file', '$tot_fc',
          '$gig_file', '$tot_fs')";
dbx_query($web_db, $query);

//Function to add to resource table
function resource_insert ($vals, $key) {
  global $web_db, $today;
  $tfc = $vals['count'];
  $tfs = $vals['size']/1073741824;
  $query = "insert into resource_stats values('$today', '$key',
             '$tfc', '$tfs')";
   dbx_query($web_db, $query);
}

//Function to add to domain table
function domain_insert ($vals, $key) {
  global $web_db, $today;
  $tfc = $vals['count'];
  $tfs = $vals['size']/1073741824;
  $query = "insert into domain_stats values('$today', '$key',
             '$tfc', '$tfs')";
   dbx_query($web_db, $query);
}

//Walk the arrays to add to web db
array_walk ($resources, 'resource_insert');
array_walk ($domains, 'domain_insert');

dbx_close($mcat_db);
dbx_close($web_db);

?>

