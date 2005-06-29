#!/usr/bin/php -q

<?php
################################################################################
## install-srbStats.mysql
##   by Roman Olschanowsky (c) 2004
##
## Used to build scheme for srbStats database
##
################################################################################
require ("../config.inc");
require "helper-funcs.inc";

// Connect to both databases
require ($dbcp_path . "mcat-connect.inc");
require ($dbcp_path . "web-connect.inc");

// Get all SRB domains we care about
$query = "select domain_id, domain_desc
from srb.mdas_td_domn
where
domain_desc <> 'groups' and
domain_desc <> 'npaci' and
domain_desc <> 'home' and
domain_desc <> 'sdsc' and
domain_desc <> 'gen-lvl1' and
domain_desc <> 'gen-lvl2' and
domain_desc <> 'gen-lvl3' and
domain_desc <> 'gen-lvl4'";

$result = dbx_query($mcat_db, $query);
$mcat_domains = $result->data;

// Get all domains currently in web db
$query = "select id, domain from domains";

$result = dbx_query($web_db, $query);
$web_domains = $result->data;

// Only interested in syncing new domains to web
$missing_domains = ndimension_array_diff($mcat_domains, $web_domains);

// Insert missing domains into web db
foreach($missing_domains as $missing_domain) {
   $query = "insert into domains values('$missing_domain[0]',
             '$missing_domain[1]')";

   dbx_query($web_db, $query);
}

// Get all SRB logical resources
$query = "select unique rsrc_id, rsrc_name
from mdas_ar_repl where
phy_rsrc_id <> rsrc_id";

$result = dbx_query($mcat_db, $query);
$mcat_logi_resources = $result->data;

// Get all logical resources currently in web db
$query = "select id, resource from logical_resources";

$result = dbx_query($web_db, $query);
$web_logi_resources = $result->data;

// Only interested in syncing new logical resources to web
$missing_resources = ndimension_array_diff($mcat_logi_resources, $web_logi_resources);

// Insert missing resources into web db
foreach($missing_resources as $missing_resource) {
   $query = "insert into logical_resources values('$missing_resource[0]',
             '$missing_resource[1]')";

   dbx_query($web_db, $query);
}

dbx_close($web_db);
dbx_close($mcat_db);

?>
