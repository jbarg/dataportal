#!/usr/bin/php -q
<?php
################################################################################
# db-connection-test.php
#   This is to test connection to both the web and the mcat databases.
#   Connects to web database srbStats and prints domains.
#   Connects to mcat database and prints domains.
#
################################################################################
require ("../config.inc");
require ($dbcp_path . "web-connect.inc");
require ($dbcp_path . "mcat-connect.inc");

// Get all SRB domains
echo "All SRB domains\n";
$result = dbx_query($mcat_db, "SELECT domain_id, domain_desc FROM mdas_td_domn");
echo "rows: $result->rows\n"; 
echo "columns: $result->cols\n\n";
for($i=0; $i < $result->rows; $i++) {
   for($j=0; $j < $result->cols; $j++) {
       echo $result->data[$i][$j] . "\t";
   }
   echo "\n";
}

echo "\n\n";

// Get all domains currently in web db
echo "All domains in web db\n";
$result = dbx_query($web_db, "SELECT id, domain FROM domains");
echo "rows: $result->rows\n";
echo "columns: $result->cols\n\n";
for($i=0; $i < $result->rows; $i++) {
   for($j=0; $j < $result->cols; $j++) {
       echo $result->data[$i][$j] . "\t";
   }
   echo "\n";
}

echo "\n\n";

// Get all SRB logical resources
echo "All SRB logical resources\n";
$query = "select unique rsrc_id, rsrc_name
from mdas_ar_repl where
phy_rsrc_id <> rsrc_id";

$result = dbx_query($mcat_db, $query);
echo "rows: $result->rows\n";
echo "columns: $result->cols\n\n";
for($i=0; $i < $result->rows; $i++) {
   for($j=0; $j < $result->cols; $j++) {
       echo $result->data[$i][$j] . "\t";
   }
   echo "\n";
}

echo "\n\n";

// Get all logical resources currently in web db
echo "All logical resources currently in web db\n";
$query = "select id, resource from logical_resources";

$result = dbx_query($web_db, $query);
echo "rows: $result->rows\n";
echo "columns: $result->cols\n\n";
for($i=0; $i < $result->rows; $i++) {
   for($j=0; $j < $result->cols; $j++) {
       echo $result->data[$i][$j] . "\t";
   }
   echo "\n";
}

dbx_close($web_db);
dbx_close($mcat_db);

?>
