alter table MDAS_AD_MDATA alter column  
         userdef_metastr1    set data type varchar(2700) ;

alter table MDAS_AR_MDATA alter column 
         userdef_metastr1   set data type varchar(2700) ;

alter table MDAS_AU_MDATA alter column 
         userdef_metastr1   set data type varchar(2700) ;

alter table MDAS_AD_COLLMDATA  alter column  
         userdef_metastr1   set data type varchar(2700) ;

alter table mdas_ad_repl add unique (repl_enum, version_num, data_name, data_grp_id);
 
insert into mdas_td_data_typ values ('0001.0001.0001.0001.000q','DICOM header');

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000q',' ',' ');

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001.0003.0001.0002','mac osx-unix file system');
insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001.0003.0001.0003','UNIX_NOCHK file system');

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001.0003.0002.0001','HPSS_NOCHK file system');

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001.0003.000a','fake_ads');

insert into MDAS_TD_DATA_GRP values ('000b','/trash', '/',2,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00');
insert into MDAS_TD_DATA_GRP values ('000a.0001','/demozone/trash', '/demozone',2,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00');
insert into MDAS_TD_DATA_GRP values ('000b.0001','/trash/home', '/trash',2,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00');
insert into MDAS_TD_DATA_GRP values ('000b.0002','/trash/container', '/trash',2,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00');
insert into MDAS_TD_DATA_GRP values ('000a.0001.0001','/demozone/trash/home', '/demozone/trash',2,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00');
insert into MDAS_TD_DATA_GRP values ('000a.0001.0002','/demozone/trash/container', '/demozone/trash',2,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00');

insert into MDAS_AD_GRP_ACCS values ('000b',8,999);
insert into MDAS_AD_GRP_ACCS values ('000b',2,999);
insert into MDAS_AD_GRP_ACCS values ('000b',6,50);
insert into MDAS_AD_GRP_ACCS values ('000b',6,30);
insert into MDAS_AD_GRP_ACCS values ('000b.0001',1,50);
insert into MDAS_AD_GRP_ACCS values ('000b.0002',1,50);
insert into MDAS_AD_GRP_ACCS values ('000a',6,30);
insert into MDAS_AD_GRP_ACCS values ('000a.0001',8,999);
insert into MDAS_AD_GRP_ACCS values ('000a.0001',2,999);
insert into MDAS_AD_GRP_ACCS values ('000a.0001',6,50);
insert into MDAS_AD_GRP_ACCS values ('000a.0001.0001',1,50);
insert into MDAS_AD_GRP_ACCS values ('000a.0001.0002',1,50);
insert into MDAS_AD_GRP_ACCS values ('0000',6,30);
insert into MDAS_AD_GRP_ACCS values ('0002',6,30);
insert into MDAS_AD_GRP_ACCS values ('0004',6,30);
insert into MDAS_AD_GRP_ACCS values ('0005',6,30);
insert into MDAS_AD_GRP_ACCS values ('0006',6,30);
insert into MDAS_AD_GRP_ACCS values ('0008',6,30);
insert into MDAS_AD_GRP_ACCS values ('0009',6,30);
 
insert into mdas_audit_desc (actionDesc,actionid) values ('D_MOVE_DATA_TO_NEWCOLL',173);
insert into mdas_audit_desc (actionDesc,actionid) values ('D_MOVE_DATA_TO_TRASH',174);
insert into mdas_audit_desc (actionDesc,actionid) values ('C_MOVE_COLL_TO_NEWCOLL',175);
insert into mdas_audit_desc (actionDesc,actionid) values ('C_MOVE_COLL_TO_TRASH',176);
insert into mdas_audit_desc (actionDesc,actionid) values ('D_CHANGE_INCOLL_OWNER',177);
insert into mdas_audit_desc (actionDesc,actionid) values ('D_CHANGE_INCOLL_OWNER_RECUR',178);
insert into mdas_audit_desc (actionDesc,actionid) values ('R_CHANGE_USER_DEFINED_STRING_META_DATA',179);
insert into mdas_audit_desc (actionDesc,actionid) values ('R_INSERT_USER_DEFINED_STRING_META_DATA',180);
insert into mdas_audit_desc (actionDesc,actionid) values ('R_DELETE_USER_DEFINED_STRING_META_DATA',181);
insert into mdas_audit_desc (actionDesc,actionid) values ('U_INSERT_USER_DEFINED_STRING_META_DATA',182);
insert into mdas_audit_desc (actionDesc,actionid) values ('U_DELETE_USER_DEFINED_STRING_META_DATA',183);
insert into mdas_audit_desc (actionDesc,actionid) values ('U_CHANGE_USER_DEFINED_STRING_META_DATA',184);
insert into mdas_audit_desc (actionDesc,actionid) values ('R_ADJUST_LATENCY_MAX_IN_MILLISEC',185);
insert into mdas_audit_desc (actionDesc,actionid) values ('R_ADJUST_LATENCY_MIN_IN_MILLISEC',186);
insert into mdas_audit_desc (actionDesc,actionid) values ('R_ADJUST_BANDWIDTH_IN_MBITSPS',187);
insert into mdas_audit_desc (actionDesc,actionid) values ('R_ADJUST_MAXIMUM_CONCURRENCY',188);
insert into mdas_audit_desc (actionDesc,actionid) values ('R_ADJUST_NUM_OF_STRIPES',189);
insert into mdas_audit_desc (actionDesc,actionid) values ('R_ADJUST_NUM_OF_HIERARCHIES',190);
insert into mdas_audit_desc (actionDesc,actionid) values ('R_ADJUST_CAPACITY_IN_GIGABYTES',191);
insert into mdas_audit_desc (actionDesc,actionid) values ('R_ADJUST_DESCRIPTION',192);
insert into mdas_audit_desc (actionDesc,actionid) values ('R_INSERT_RSRC_USAGE_TOTAL_BY_DATE',193);
insert into mdas_audit_desc (actionDesc,actionid) values ('R_INSERT_RSRC_USAGE_AND_QUOTA',194);
insert into mdas_audit_desc (actionDesc,actionid) values ('R_DELETE_PHY_RSRC_FROM_LOG_RSRC',195);
insert into mdas_audit_desc (actionDesc,actionid) values ('D_DELETE_USER_DEFINED_ATTR_VAL_META_DATA',196);
insert into mdas_audit_desc (actionDesc,actionid) values ('R_DELETE_USER_DEFINED_ATTR_VAL_META_DATA',197);
insert into mdas_audit_desc (actionDesc,actionid) values ('U_DELETE_USER_DEFINED_ATTR_VAL_META_DATA',198);
insert into mdas_audit_desc (actionDesc,actionid) values ('C_DELETE_USER_DEFINED_ATTR_VAL_META_DATA',199);
insert into mdas_audit_desc (actionDesc,actionid) values ('R_INSERT_PHY_RSRC_INTO_LOG_RSRC',200);

insert into mdas_audit_desc (actionDesc,actionid) values ('D_SU_DELETE_TRASH_ONE',1006);
insert into mdas_audit_desc (actionDesc,actionid) values ('C_SU_DELETE_TRASH_COLL',1007);

commit;

exit;

