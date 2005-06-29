update MDAS_TD_DATA_GRP
set data_grp_name   = '/demozone' || data_grp_name
where data_grp_name  <>  '/'

commit;

update MDAS_TD_DATA_GRP
set   parent_grp_name = '/demozone' || parent_grp_name
where  parent_grp_name <> '/'
;

commit;

update MDAS_TD_DATA_GRP
set   parent_grp_name = '/null'
where parent_grp_name = '/demozone/null'
;

commit;

update MDAS_TD_DATA_GRP
set coll_mod_timestamp = coll_cr_timestamp

commit;

