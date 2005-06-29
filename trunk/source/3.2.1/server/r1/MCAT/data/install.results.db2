create table MDAS_TD_DOMN (domain_id  varchar(250) not null , domain_desc varchar(250) not null, primary key (domain_id) )  
DB20000I  The SQL command completed successfully.

create table MDAS_TD_DATA_GRP (data_grp_id     varchar(250)  not null, data_grp_name      varchar(250) not null , parent_grp_name      varchar(250) not null , coll_owner integer, coll_cr_timestamp varchar(32), coll_comments varchar(700), coll_lock       integer DEFAULT 0, coll_lockowner      integer DEFAULT  0, coll_lockexpiry varchar(32) DEFAULT '0', coll_link       integer  not null DEFAULT 0, coll_mod_timestamp varchar(32), primary key (data_grp_id) )  
DB20000I  The SQL command completed successfully.

create table MDAS_TD_DATA_TYP (data_typ_id     varchar(250)  not null, data_typ_name      varchar(250)  not null, primary key (data_typ_id) )  
DB20000I  The SQL command completed successfully.

create table MDAS_TD_RSRC_TYP (rsrc_typ_id   varchar(250)  not null, rsrc_typ_name    varchar(250)  not null, primary key (rsrc_typ_id) )  
DB20000I  The SQL command completed successfully.

create table MDAS_TD_RSRC_CLASS (rsrc_class_id   integer  not null, rsrc_class_name    varchar(250)  not null, primary key (rsrc_class_id) )  
DB20000I  The SQL command completed successfully.

create table MDAS_TD_USER_TYP (user_typ_id varchar(250)  not null, user_typ_name varchar(250)  not null, primary key (user_typ_id) )  
DB20000I  The SQL command completed successfully.

create table MDAS_TD_LOCN (locn_id varchar(250)  not null, locn_desc varchar(250)  not null, country varchar(100), state varchar(100), city varchar(100), county varchar(100), site varchar(100), building varchar(250), wing varchar(100), floor varchar(100), room varchar(100), organization varchar(250), department varchar(250), division varchar(100), subdivision varchar(100), project varchar(250), domain varchar(100), my_group varchar(100), latitude varchar(100), longitude varchar(100), timezone varchar(100), zipcode varchar(100), netprefix varchar(250), nettyp varchar(100), server_user_id integer , primary key (locn_id) )  
DB20000I  The SQL command completed successfully.

create table MDAS_TD_ACTN (action_id integer  not null, action_desc varchar(250)  not null, primary key (action_id) )  
DB20000I  The SQL command completed successfully.

create table MDAS_TD_RSRC_ACCS (access_id  integer  not null, access_constraint varchar(250)  not null, access_list  varchar(250), access_privilege varchar(250), primary key (access_id) )  
DB20000I  The SQL command completed successfully.

create table MDAS_TD_DS_ACCS (access_id   integer  not null, access_constraint varchar(250) not null , access_list  varchar(250), access_privilege varchar(250), primary key (access_id) )  
DB20000I  The SQL command completed successfully.

create table MDAS_TD_CONTAINER (container_id             integer  not null, container_name           varchar(1000), container_max_size       bigint, rsrc_id    integer, primary key (container_id) )  
DB20000I  The SQL command completed successfully.

create table MDAS_TD_AUTH_SCHM (auth_scheme_id  int not null , auth_scheme  varchar(250) not null, primary key (auth_scheme_id) )  
DB20000I  The SQL command completed successfully.

create table MDAS_TD_ZONE (zone_id           varchar(250) not null, local_zone_flag   int   DEFAULT 0, locn_id           varchar(250), port_number       int, user_id           int, zone_status       int   DEFAULT 1, zone_create_date  varchar(32), zone_modify_date  varchar(32), zone_comments     varchar(250), zone_contact      varchar(250) )
DB20000I  The SQL command completed successfully.

create table MDAS_AT_DATA_TYP_EXT (data_typ_id     varchar(250)  not null, data_typ_mime       varchar(250) , data_typ_ext       varchar(700) )  
DB20000I  The SQL command completed successfully.

create table MDAS_CD_USER (user_id integer not null , user_name varchar(250) not null , user_typ_id varchar(250)  not null, zone_id  varchar(250), user_creation_date varchar(32), user_modify_date  varchar(32), primary key (user_id) )  
DB20000I  The SQL command completed successfully.

create table MDAS_AU_INFO (user_id integer not null , user_address varchar(250), user_phone  varchar(250), user_email varchar(250) )  
DB20000I  The SQL command completed successfully.

create table MDAS_AU_GROUP (user_id integer  not null, group_user_id integer not null )  
DB20000I  The SQL command completed successfully.

create table MDAS_AU_DOMN (user_id integer  not null, domain_id varchar(250)  not null )  
DB20000I  The SQL command completed successfully.

create table MDAS_AU_AUTH_KEY (user_id integer not null , priv_key varchar(250) not null )  
DB20000I  The SQL command completed successfully.

create table MDAS_AU_AUTH_MAP (user_id integer not null , distin_name varchar(250) not null, auth_scheme_id int )  
DB20000I  The SQL command completed successfully.

create table MDAS_AU_AUDIT (user_id integer  not null, action_id integer   not null, time_stamp VARCHAR(32)  not null, comments  varchar(250) )  
DB20000I  The SQL command completed successfully.

create table MDAS_AUDIT_DESC (actionid    integer, actiondesc   varchar(250) )  
DB20000I  The SQL command completed successfully.

create table MDAS_CD_RSRC (rsrc_id integer  not null, rsrc_name varchar(250)  not null, rsrc_typ_id varchar(250) not null , primary key (rsrc_id) )  
DB20000I  The SQL command completed successfully.

create table MDAS_AR_PHYSICAL (phy_rsrc_id integer  not null, phy_repl_enum integer  not null, phy_rsrc_name  varchar(250)  not null, rsrc_typ_id varchar(250)  not null , locn_id varchar(250)  not null, phy_repl_timestamp VARCHAR(32)  not null, phy_owner	integer  not null, phy_default_path varchar(2000), max_obj_size   bigint, rsrc_class_id     integer, rsrc_lock       integer DEFAULT 0, rsrc_lockowner      integer DEFAULT  0, rsrc_lockexpiry varchar(32) DEFAULT '0', primary key (phy_rsrc_id, phy_repl_enum) )  
DB20000I  The SQL command completed successfully.

create table MDAS_AR_REPL (rsrc_id integer  not null, repl_enum integer  not null, rsrc_name  varchar(250)  not null, rsrc_typ_id varchar(250)  not null, locn_id varchar(250)  not null, repl_timestamp VARCHAR(32)  not null, owner	integer  not null, phy_rsrc_id  integer  not null, default_path varchar(2000), max_obj_size   bigint, rsrc_class_id  integer, rsrc_creation_date varchar(32), rsrc_modify_date  varchar(32), rsrc_comments     varchar(250), primary key (rsrc_id, repl_enum) )  
DB20000I  The SQL command completed successfully.

create table MDAS_AR_CMPND_RSRC (cmpnd_rsrc_id integer  not null, phy_rsrc_id integer  not null, user_id integer  not null )  
DB20000I  The SQL command completed successfully.

create table MDAS_AR_ACCS (rsrc_id integer  not null, repl_enum integer  not null, user_id integer not null , access_id   integer not null )  
DB20000I  The SQL command completed successfully.

create table MDAS_AR_AUTH (rsrc_id integer  not null, repl_enum integer  not null, authority varchar(250), access_key   varchar(250), access_param_1 varchar(250), access_param_2 varchar(250) )  
DB20000I  The SQL command completed successfully.

create table MDAS_AR_AUDIT (rsrc_id integer not null , repl_enum integer  not null, user_id integer  not null, action_id integer  not null, time_stamp VARCHAR(32)  not null, comments  varchar(250) )  
DB20000I  The SQL command completed successfully.

create table MDAS_AR_INFO (rsrc_id integer not null , repl_enum integer  not null, mlsec_latency_max  integer, mlsec_latency_min  integer, mbps_bandwidth     integer, concurrency_max    integer, num_of_hierarchies integer, num_of_stripes     integer, megab_capacity     integer, description        varchar(700) )  
DB20000I  The SQL command completed successfully.

create table MDAS_CD_DATA (data_id integer not null , data_name varchar(250)  not null, data_typ_id varchar(250)  not null, primary key (data_id) )  
DB20000I  The SQL command completed successfully.

create table MDAS_AD_REPL (data_id integer  not null, repl_enum integer  not null, data_name  varchar(250)  not null, data_typ_id varchar(250)   not null, path_name varchar(250), rsrc_id integer  not null, repl_timestamp VARCHAR(32)  not null , data_grp_id varchar(250)  not null , data_size bigint, is_deleted integer not null, data_owner integer  not null, is_dirty   integer, offset     integer, container_id integer, modify_timestamp varchar(32), last_accs_time varchar(32), datalock       integer DEFAULT 0, lockowner      integer DEFAULT  0, version_num    varchar(250) not null DEFAULT '0', container_td_id integer, d_comment varchar(250), dlockexpiry varchar(32) DEFAULT '0', dchecksum varchar(400) DEFAULT '0', dhide    integer  DEFAULT 0, auditflag   integer DEFAULT 0, seg_num integer not null DEFAULT -1, primary key (data_id, repl_enum,version_num,seg_num) )  
DB20000I  The SQL command completed successfully.

create table MDAS_AD_MISC1 (data_id integer  not null, repl_enum integer  not null, dpin       integer DEFAULT 0, dpinowner      integer DEFAULT  0, dpinexpiry varchar(32) DEFAULT '0', dexpire_date varchar(32) DEFAULT '9999-12-15-00.00.00', dexpire_date_2 varchar(32) DEFAULT '9999-12-15-00.00.00', dcompressed varchar(250) DEFAULT '0', dencrypted  varchar(250) DEFAULT '0' )  
DB20000I  The SQL command completed successfully.

create table MDAS_AD_COMPOUND( data_id integer not null, repl_enum integer  not null, seg_num integer DEFAULT -1, user_id integer, int_repl_num integer  not null, int_seg_num integer DEFAULT -1, cmpd_path_name  varchar(699), cmpd_rsrc_id   integer, repl_timestamp VARCHAR(32)  not null , modify_timestamp varchar(32), last_accs_time varchar(32), data_size bigint, offset     integer, is_dirty   integer )  
DB20000I  The SQL command completed successfully.

create table MDAS_AD_MDATA (data_id integer  not null, metadatanum integer not null, userdef_metastr0   varchar(350), userdef_metastr1   varchar(350), userdef_metastr2   varchar(350), userdef_metastr3   varchar(350), userdef_metastr4   varchar(350), userdef_metastr5   varchar(350), userdef_metastr6   varchar(350), userdef_metastr7   varchar(350), userdef_metastr8   varchar(350), userdef_metastr9   varchar(350), userdef_metaint0   integer, userdef_metaint1   integer, userdef_creat_date varchar(32), userdef_modif_date  varchar(32) )  
DB20000I  The SQL command completed successfully.

create table MDAS_AD_COLLMDATA (data_grp_id  varchar(250)  not null, metadatanum integer not null, userdef_metastr0   varchar(350), userdef_metastr1   varchar(350), userdef_metastr2   varchar(350), userdef_metastr3   varchar(350), userdef_metastr4   varchar(350), userdef_metastr5   varchar(350), userdef_metastr6   varchar(350), userdef_metastr7   varchar(350), userdef_metastr8   varchar(350), userdef_metastr9   varchar(350), userdef_metaint0   integer, userdef_metaint1   integer, userdef_creat_date varchar(32), userdef_modif_date  varchar(32) )  
DB20000I  The SQL command completed successfully.

create table MDAS_AR_MDATA (rsrc_id  integer  not null, metadatanum integer not null, userdef_metastr0   varchar(350), userdef_metastr1   varchar(350), userdef_metastr2   varchar(350), userdef_metastr3   varchar(350), userdef_metastr4   varchar(350), userdef_metastr5   varchar(350), userdef_metastr6   varchar(350), userdef_metastr7   varchar(350), userdef_metastr8   varchar(350), userdef_metastr9   varchar(350), userdef_metaint0   integer, userdef_metaint1   integer, userdef_creat_date varchar(32), userdef_modif_date  varchar(32) )  
DB20000I  The SQL command completed successfully.

create table MDAS_AU_MDATA (user_id  integer  not null, metadatanum integer not null, userdef_metastr0   varchar(350), userdef_metastr1   varchar(350), userdef_metastr2   varchar(350), userdef_metastr3   varchar(350), userdef_metastr4   varchar(350), userdef_metastr5   varchar(350), userdef_metastr6   varchar(350), userdef_metastr7   varchar(350), userdef_metastr8   varchar(350), userdef_metastr9   varchar(350), userdef_metaint0   integer, userdef_metaint1   integer, userdef_creat_date varchar(32), userdef_modif_date  varchar(32) )  
DB20000I  The SQL command completed successfully.

create table MDAS_AD_COLLCONT (data_grp_id  varchar(250)  not null, container_id             integer  not null, data_typ_id    varchar(250)   not null )  
DB20000I  The SQL command completed successfully.

create table MDAS_AD_COMMENTS (data_id integer not null , repl_enum integer  not null, user_id integer  not null, com_date varchar(32)  not null, comments varchar(700) )   
DB20000I  The SQL command completed successfully.

create table MDAS_AC_ANNOTATION (data_grp_id  varchar(250)  not null, user_id integer  not null, anno_date VARCHAR(32) not null, annotations varchar(700), anno_type varchar(250) )   
DB20000I  The SQL command completed successfully.

create table MDAS_AD_ANNOTATION (data_id integer not null , repl_enum integer  not null, user_id integer  not null, anno_date VARCHAR(32) not null, annotations varchar(700) , anno_position varchar(250) )  
DB20000I  The SQL command completed successfully.

create table MDAS_AD_ACCS (data_id integer  not null, repl_enum integer  not null, user_id integer not null , access_id integer not null )  
DB20000I  The SQL command completed successfully.

create table MDAS_AD_GRP_ACCS (data_grp_id  varchar(250)  not null, user_id integer not null , access_id integer not null )  
DB20000I  The SQL command completed successfully.

create table MDAS_AD_AUDIT (data_id integer  not null, repl_enum integer not null , user_id integer  not null, action_id integer not null , time_stamp VARCHAR(32)  not null, comments   varchar(250) )  
DB20000I  The SQL command completed successfully.

create table MDAS_AU_TCKT_DATA ( ticket_id  varchar(250) not null, data_id integer  not null, user_id integer not null, begin_time varchar(32), end_time varchar(32), access_count integer, access_id integer not null, ticket_owner  integer  not null )  
DB20000I  The SQL command completed successfully.

create table MDAS_AU_TCKT_GRP ( ticket_id  varchar(250) not null, data_grp_id varchar(250)  not null, user_id integer not null, begin_time varchar(32), end_time varchar(32), access_count integer, access_id integer not null, is_recursive integer, ticket_owner  integer  not null )  
DB20000I  The SQL command completed successfully.

create table MDAS_AD_CLASS (data_id integer  not null, class_type varchar(250), class_name varchar(250) )  
DB20000I  The SQL command completed successfully.

create table MDAS_AD_STRCT_BLOB (strblngsto_data_id integer  not null, structure_type varchar(250), structure_comments varchar(250), strt_isin_data_id integer, internal_structure varchar(2000) )  
DB20000I  The SQL command completed successfully.

create table MDAS_AD_DATA_INDEX (data_id_of_index integer not null, indexed_data_id  integer not null )  
DB20000I  The SQL command completed successfully.

create table MDAS_AD_DTYP_INDEX (data_id_of_index integer not null, indexed_datatyp_id varchar(250)   not null )  
DB20000I  The SQL command completed successfully.

create table MDAS_AD_COLL_INDEX (data_id_of_index integer not null, indexed_datcoll_id varchar(250)   not null )  
DB20000I  The SQL command completed successfully.

create table MDAS_AD_DATA_METH (data_id_of_meth integer not null, methfor_data_id  integer not null )  
DB20000I  The SQL command completed successfully.

create table MDAS_AD_DTYP_METH (data_id_of_meth integer not null, methfor_datatyp_id varchar(250)   not null )  
DB20000I  The SQL command completed successfully.

create table MDAS_AD_COLL_METH (data_id_of_meth integer not null, methfor_datcoll_id varchar(250)   not null )  
DB20000I  The SQL command completed successfully.

create table MDAS_AR_DB_RSRC ( rsrc_id integer  not null, repl_enum integer  not null, rsrc_name  varchar(250)  not null, rsrc_typ_id varchar(250)  not null, rsrc_database_name varchar(250) not null, db_link_name varchar(250), db_user_name varchar(250), db_pass varchar(250), db_connect_string varchar(500) )  
DB20000I  The SQL command completed successfully.

create table MDAS_AR_LRSRC_RRBN ( rsrc_id integer  not null, phy_rsrc_id integer  not null )  
DB20000I  The SQL command completed successfully.

create table MDAS_AUDIT ( aud_timestamp   varchar(40), actionId        integer, userId          integer, dataId          integer, collName        varchar(250), aud_comments    varchar(700) )  
DB20000I  The SQL command completed successfully.

create table MDAS_COUNTER (cname varchar(250) not null , cvalue integer  not null, primary key (cname) )  
DB20000I  The SQL command completed successfully.

create table MDAS_AR_DAILY_TOT (usage_timestamp   char(20), data_owner        integer, rsrc_id           integer, sum_data_size     bigint )  
DB20000I  The SQL command completed successfully.

create table MDAS_AR_USE_QUOTA (data_owner        integer, rsrc_id           integer, sum_data_size     bigint, max_quota         integer )  
DB20000I  The SQL command completed successfully.

create table MDAS_AR_TAPE_INFO (tapeNumber     varchar(50), tapeOwner      varchar(400), tapeType       char(10), tapeLibInx     integer, fullFlag       integer, currentFileSeqNum   integer, currentAbsPosition  varchar(400), bytesWritten        integer )  
DB20000I  The SQL command completed successfully.

create table DC_TD_CONTR_TYPE ( dc_contr_type_name varchar(400)    not null, dc_contr_type_id integer        not null, primary key (dc_contr_type_id) )  
DB20000I  The SQL command completed successfully.

create table DC_TD_SUBJ_CLASS ( dc_subj_class_name varchar(400)    not null, dc_subj_class_id varchar(250)      not null, primary key (dc_subj_class_id) )  
DB20000I  The SQL command completed successfully.

create table DC_TD_DESCR_TYPE ( dc_desc_type_name varchar(400)     not null, dc_desc_type_id integer       not null, primary key (dc_desc_type_id) )  
DB20000I  The SQL command completed successfully.

create table DC_TD_TYPE ( dc_type_name varchar(400)         not null, dc_type_id integer           not null, primary key (dc_type_id) )  
DB20000I  The SQL command completed successfully.

create table DC_TD_SOURCE_TYPE ( source_type_name varchar(400)     not null, source_type_id integer       not null, primary key (source_type_id) )  
DB20000I  The SQL command completed successfully.

create table DC_TD_LANGUAGE ( language_name  varchar(400)       not null, language_id  integer         not null, primary key (language_id) )  
DB20000I  The SQL command completed successfully.

create table DC_TD_RELATION_TYP ( relation_type_name varchar(400)   not null, relation_type_id integer     not null, primary key (relation_type_id) )  
DB20000I  The SQL command completed successfully.

create table DC_TD_COVERAGE ( coverage_type_name varchar(400)   not null, coverage_type_id integer     not null, primary key (coverage_type_id) )  
DB20000I  The SQL command completed successfully.

create table DC_TD_RIGHTS ( rights_type_name varchar(400)   not null, rights_type_id integer     not null, primary key (rights_type_id) )  
DB20000I  The SQL command completed successfully.

create table DC_TITLE ( dc_title  varchar(400)            not null, data_id integer                 not null, repl_enum integer               not null )  
DB20000I  The SQL command completed successfully.

create table DC_AUTHCONTRIB ( dc_contr_id       integer     not null, dc_contr_name varchar(400)         not null, dc_contr_corp_name varchar(400), dc_contr_type_id integer        not null, dc_contr_email  varchar(400), dc_contr_phone varchar(100), dc_contr_web    varchar(100), dc_contr_address varchar(100), primary key (dc_contr_id) )  
DB20000I  The SQL command completed successfully.

create table DC_AUTHCONTR_DATA ( dc_contr_id        integer not null, data_id integer                 not null, repl_enum integer               not null )  
DB20000I  The SQL command completed successfully.

create table DC_SUBJECT ( dc_subject_name varchar(400)       not null, dc_subj_class_id varchar(250)      not null, data_id integer                 not null, repl_enum integer               not null )  
DB20000I  The SQL command completed successfully.

create table DC_DESCRIPTION ( dc_desc varchar(400)               not null, dc_desc_type_id integer       not null, data_id integer                 not null, repl_enum integer               not null )  
DB20000I  The SQL command completed successfully.

create table DC_PUBLISHER ( dc_pub_name varchar(400)          not null, dc_pub_addr varchar(400)          not null, data_id integer                 not null, repl_enum integer               not null )  
DB20000I  The SQL command completed successfully.

create table DC_TYPE ( dc_type_id integer             not null, data_id integer                 not null, repl_enum integer               not null )  
DB20000I  The SQL command completed successfully.

create table DC_SOURCE ( source_desc varchar(400)          not null, source_type_id integer       not null, data_id integer                 not null, repl_enum integer               not null )  
DB20000I  The SQL command completed successfully.

create table DC_LANGUAGE ( language_id  integer         not null, data_id integer                 not null, repl_enum integer               not null )  
DB20000I  The SQL command completed successfully.

create table DC_RELATION ( related_data_id  integer, related_data_desc varchar(400), relation_type_id integer     not null, data_id integer                 not null, repl_enum integer               not null )  
DB20000I  The SQL command completed successfully.

create table DC_COVERAGE ( coverage_desc varchar(400)        not null, coverage_type_id integer     not null, data_id integer                 not null, repl_enum integer               not null )  
DB20000I  The SQL command completed successfully.

create table DC_RIGHTS ( rights_data_id  integer, rights_data_desc  varchar(400), rights_type_id  integer      not null, data_id integer                 not null, repl_enum integer               not null )  
DB20000I  The SQL command completed successfully.

create table USEREXTRA (extra   VARCHAR(250))
DB20000I  The SQL command completed successfully.

create alias DC_DATA  for  MDAS_AD_REPL
DB20000I  The SQL command completed successfully.

create alias MDAS_AD_DCRELREPL for  MDAS_AD_REPL
DB20000I  The SQL command completed successfully.

create alias DC_COLLECTION for  MDAS_TD_DATA_GRP
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_DCRELGRP for  MDAS_TD_DATA_GRP
DB20000I  The SQL command completed successfully.

create alias MDAS_CD_PAR_USER     for MDAS_CD_USER
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_GRP_ACCS     for MDAS_TD_DS_ACCS
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_DATA_2GRP    for MDAS_TD_DATA_GRP
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_RSRC_2TYP    for MDAS_TD_RSRC_TYP
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_RSRC_2CLS    for MDAS_TD_RSRC_CLASS
DB20000I  The SQL command completed successfully.

create alias MDAS_CD_OWNER_USER   for MDAS_CD_USER
DB20000I  The SQL command completed successfully.

create alias MDAS_CD_DTKTOWNER    for MDAS_CD_USER
DB20000I  The SQL command completed successfully.

create alias MDAS_CD_CTKTOWNER    for MDAS_CD_USER
DB20000I  The SQL command completed successfully.

create alias MDAS_CD_DTKTUSER     for MDAS_CD_USER
DB20000I  The SQL command completed successfully.

create alias MDAS_CD_CTKTUSER     for MDAS_CD_USER
DB20000I  The SQL command completed successfully.

create alias MDAS_AU_CTKTU_DOMN   for MDAS_AU_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_AU_DTKTU_DOMN   for MDAS_AU_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_AU_CTKTO_DOMN   for MDAS_AU_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_AU_DTKTO_DOMN   for MDAS_AU_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_CTKTU_DOMN   for MDAS_TD_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_DTKTU_DOMN   for MDAS_TD_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_CTKTO_DOMN   for MDAS_TD_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_DTKTO_DOMN   for MDAS_TD_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_AU_CTKT_USGRP   for MDAS_AU_GROUP
DB20000I  The SQL command completed successfully.

create alias MDAS_AU_DTKT_USGRP   for MDAS_AU_GROUP
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_DSTKT_ACCS   for MDAS_TD_DS_ACCS
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_GRTKT_ACCS   for MDAS_TD_DS_ACCS
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_DATA_3GRP    for MDAS_TD_DATA_GRP
DB20000I  The SQL command completed successfully.

create alias MDAS_ADC_REPL        for MDAS_AD_REPL
DB20000I  The SQL command completed successfully.

create alias MDAS_AR_1PHYSICAL    for MDAS_AR_PHYSICAL
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_1RSRC_CLASS  for MDAS_TD_RSRC_CLASS
DB20000I  The SQL command completed successfully.

create alias MDAS_AR_1REPL        for MDAS_AR_REPL
DB20000I  The SQL command completed successfully.

create alias MDAS_AU_OWNER_DOMN   for MDAS_AU_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_OWNER_DOMN   for MDAS_TD_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_SRVR_LOCN    for MDAS_TD_LOCN
DB20000I  The SQL command completed successfully.

create alias MDAS_AU_OWNER_INFO   for MDAS_AU_INFO
DB20000I  The SQL command completed successfully.

create alias MDAS_CD_ANNO_USER    for MDAS_CD_USER
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_ANNO_DOMN    for MDAS_TD_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_AU_ANNO_DOMN    for MDAS_AU_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_ADSTRUCT_REPL   for MDAS_AD_REPL
DB20000I  The SQL command completed successfully.

create alias MDAS_ADIXDS_REPL     for MDAS_AD_REPL
DB20000I  The SQL command completed successfully.

create alias MDAS_ADMTHDS_REPL    for MDAS_AD_REPL
DB20000I  The SQL command completed successfully.

create alias MDAS_ADIXDTP_REPL    for MDAS_AD_REPL
DB20000I  The SQL command completed successfully.

create alias MDAS_ADMTHDTP_REPL   for MDAS_AD_REPL
DB20000I  The SQL command completed successfully.

create alias MDAS_ADIXCOLL_REPL   for MDAS_AD_REPL
DB20000I  The SQL command completed successfully.

create alias MDAS_ADMTHCOL_REPL   for MDAS_AD_REPL
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_DGRP_IXDS    for MDAS_TD_DATA_GRP
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_DGRP_IXDTP   for MDAS_TD_DATA_GRP
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_DGRP_IXCOL   for MDAS_TD_DATA_GRP
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_DGRP_MTHDS   for MDAS_TD_DATA_GRP
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_DGRP_MTDTP   for MDAS_TD_DATA_GRP
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_DGRP_MTCOL   for MDAS_TD_DATA_GRP
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_DTYP_IXDS    for MDAS_TD_DATA_TYP
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_DTYP_IXDTP   for MDAS_TD_DATA_TYP
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_DTYP_IXCOL   for MDAS_TD_DATA_TYP
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_DTYP_MTHDS   for MDAS_TD_DATA_TYP
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_DTYP_MTDTP   for MDAS_TD_DATA_TYP
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_DTYP_MTCOL   for MDAS_TD_DATA_TYP
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_DGRP_STRUC   for MDAS_TD_DATA_GRP
DB20000I  The SQL command completed successfully.

create alias MDAS_CD_DAUDT_USER   for MDAS_CD_USER
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_DAUDT_DOMN   for MDAS_TD_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_AU_DAUDT_DOMN   for MDAS_AU_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_COLLCONT     for  MDAS_TD_CONTAINER
DB20000I  The SQL command completed successfully.

create alias MDAS_AD_1COLLMDATA   for  MDAS_AD_COLLMDATA
DB20000I  The SQL command completed successfully.

create alias MDAS_AD_1MDATA       for  MDAS_AD_MDATA
DB20000I  The SQL command completed successfully.

create alias MDAS_AD_2COLLMDATA   for MDAS_AD_COLLMDATA
DB20000I  The SQL command completed successfully.

create alias MDAS_AD_2MDATA       for  MDAS_AD_MDATA
DB20000I  The SQL command completed successfully.

create alias MDAS_AD_3COLLMDATA   for  MDAS_AD_COLLMDATA
DB20000I  The SQL command completed successfully.

create alias MDAS_AD_3MDATA       for  MDAS_AD_MDATA
DB20000I  The SQL command completed successfully.

create alias MDAS_AD_4COLLMDATA   for  MDAS_AD_COLLMDATA
DB20000I  The SQL command completed successfully.

create alias MDAS_AD_4MDATA       for  MDAS_AD_MDATA
DB20000I  The SQL command completed successfully.

create alias MDAS_AD_ANNO_USER    for  MDAS_CD_USER
DB20000I  The SQL command completed successfully.

create alias MDAS_AD_GRP_2ACCS    for  MDAS_AD_GRP_ACCS
DB20000I  The SQL command completed successfully.

create alias MDAS_AD_VOCHNLREPL   for  MDAS_AD_REPL
DB20000I  The SQL command completed successfully.

create alias MDAS_AD_VOTNAILRPL   for  MDAS_AD_REPL
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_IDGRPUSER1   for  MDAS_AU_GROUP
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_IDGRPUSER2   for  MDAS_AU_GROUP
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_WC_ACTION    for  MDAS_TD_ACTN
DB20000I  The SQL command completed successfully.

create alias MDAS_ADCONT_REPL     for  MDAS_AD_REPL
DB20000I  The SQL command completed successfully.

create alias MDAS_CD_COOWN_USER   for  MDAS_CD_USER
DB20000I  The SQL command completed successfully.

create alias MDAS_CD_DLOWN_USER   for  MDAS_CD_USER
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_LCK_ACCS     for  MDAS_TD_DS_ACCS
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_DLOWN_DOMN   for  MDAS_TD_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_COOWN_DOMN   for  MDAS_TD_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_AU_COOWN_DOMN   for  MDAS_AU_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_AU_DLOWN_DOMN   for  MDAS_AU_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_AR_1MDATA       for  MDAS_AR_MDATA
DB20000I  The SQL command completed successfully.

create alias MDAS_AR_2MDATA       for  MDAS_AR_MDATA
DB20000I  The SQL command completed successfully.

create alias MDAS_AR_3MDATA       for  MDAS_AR_MDATA
DB20000I  The SQL command completed successfully.

create alias MDAS_AR_4MDATA       for  MDAS_AR_MDATA
DB20000I  The SQL command completed successfully.

create alias MDAS_AU_1MDATA       for  MDAS_AU_MDATA
DB20000I  The SQL command completed successfully.

create alias MDAS_AU_2MDATA       for  MDAS_AU_MDATA
DB20000I  The SQL command completed successfully.

create alias MDAS_AU_3MDATA       for  MDAS_AU_MDATA
DB20000I  The SQL command completed successfully.

create alias MDAS_AU_4MDATA       for  MDAS_AU_MDATA
DB20000I  The SQL command completed successfully.

create alias MDAS_AR_2PHYSICAL    for  MDAS_AR_PHYSICAL
DB20000I  The SQL command completed successfully.

create alias MDAS_AR_3PHYSICAL    for  MDAS_AR_PHYSICAL
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_INT_LOCN     for  MDAS_TD_LOCN
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_RSRC_3TYP    for  MDAS_TD_RSRC_TYP
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_2RSRC_CLASS  for  MDAS_TD_RSRC_CLASS
DB20000I  The SQL command completed successfully.

create alias MDAS_CD_COMP_USER    for  MDAS_CD_USER
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_COMP_DOMN    for  MDAS_TD_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_AU_COMP_DOMN    for  MDAS_AU_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_AD_1COMPOUND    for  MDAS_AD_COMPOUND
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_DATA_4GRP    for  MDAS_TD_DATA_GRP
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_DATA_5GRP    for  MDAS_TD_DATA_GRP
DB20000I  The SQL command completed successfully.

create alias MDAS_CD_PIN_USER     for  MDAS_CD_USER
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_PIN_DOMN     for  MDAS_TD_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_AU_PIN_DOMN     for  MDAS_AU_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_CD_CANNO_USER   for  MDAS_CD_USER
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_CANNO_DOMN   for  MDAS_TD_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_AU_CANNO_DOMN   for  MDAS_AU_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_ZN_LOCN      for  MDAS_TD_LOCN
DB20000I  The SQL command completed successfully.

create alias MDAS_CD_ZN_USER      for  MDAS_CD_USER
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_ZN_DOMN      for  MDAS_TD_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_AU_ZN_DOMN      for  MDAS_AU_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_AU_ZN_ATHMAP    for  MDAS_AU_AUTH_MAP
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_ZN_ATHSCHM   for  MDAS_TD_AUTH_SCHM
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_PAR_DOMN     for MDAS_TD_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_AU_PAR_DOMN     for MDAS_AU_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_CD_RS_USER      for MDAS_CD_USER
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_RS_DOMN      for MDAS_TD_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_AU_RS_DOMN      for MDAS_AU_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_CD_RSAC_USER   for MDAS_CD_USER
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_RSAC_DOMN   for MDAS_TD_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_AU_RSAC_DOMN   for MDAS_AU_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_PAR_LOCN    for  MDAS_TD_LOCN
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_DTYP_PARNT  for MDAS_TD_DATA_TYP
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_PARDOMN     for MDAS_TD_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_UTYP_PARNT  for MDAS_TD_USER_TYP
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_RTYP_PARNT  for MDAS_TD_RSRC_TYP
DB20000I  The SQL command completed successfully.

create alias MDAS_CD_RAGRP_USER  for MDAS_CD_USER
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_RAGRP_DOMN  for MDAS_TD_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_AU_RAGRP_DOMN  for MDAS_AU_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_CD_DAGRP_USER  for MDAS_CD_USER
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_DAGRP_DOMN  for MDAS_TD_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_AU_DAGRP_DOMN  for MDAS_AU_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_CD_CAGRP_USER  for MDAS_CD_USER
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_CAGRP_DOMN  for MDAS_TD_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_AU_CAGRP_DOMN  for MDAS_AU_DOMN
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_CAGRP_ACCS  for MDAS_TD_GRP_ACCS
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_CADATA_GRP  for MDAS_TD_DATA_GRP
DB20000I  The SQL command completed successfully.

create alias MDAS_AD_CAGRP_ACCS  for MDAS_AD_GRP_ACCS
DB20000I  The SQL command completed successfully.

create alias MDAS_TD_CCDATA_TYP  for MDAS_TD_DATA_TYP
DB20000I  The SQL command completed successfully.

alter table mdas_td_data_grp add unique (data_grp_name,parent_grp_name,coll_link)
DB20000I  The SQL command completed successfully.

alter table mdas_au_domn add unique (user_id, domain_id)
DB20000I  The SQL command completed successfully.

alter table mdas_ad_collcont add unique (data_grp_id,data_typ_id )
DB20000I  The SQL command completed successfully.

insert into MDAS_COUNTER values ('ACTION_ID', 2000)
DB20000I  The SQL command completed successfully.

insert into MDAS_COUNTER values ('ACCESS_ID', 2000)
DB20000I  The SQL command completed successfully.

insert into MDAS_COUNTER values ('USER_ID', 100)
DB20000I  The SQL command completed successfully.

insert into MDAS_COUNTER values ('RSRC_ID', 100)
DB20000I  The SQL command completed successfully.

insert into MDAS_COUNTER values ('DATA_ID', 100)
DB20000I  The SQL command completed successfully.

insert into MDAS_COUNTER values ('TABLE_ID', 1000)
DB20000I  The SQL command completed successfully.

insert into MDAS_COUNTER values ('ATTRIBUTE_ID', 1000)
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DOMN values ('0001','gen-lvl1')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DOMN values ('0001.0001','gen-lvl2')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DOMN values ('0001.0001.0001','gen-lvl3')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DOMN values ('0001.0001.0001.0001','gen-lvl4')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DOMN values ('0002','home')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DOMN values ('0001.0001.0001.0001.0001','npaci')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DOMN values ('0001.0001.0001.0001.0001.0001','sdsc')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DOMN values ('0001.0001.0001.0001.0001.0002','ucb')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DOMN values ('0001.0001.0001.0001.0002','groups')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0001','/g1', '/null',2,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0001.0001','/g1/g1','/g1',2,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0001.0001.0001','/g1/g1/g1','/g1/g1',2,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0001.0001.0001.0001','/g1/g1/g1/srbtest','/g1/g1/g1',2,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0001.0001.0001.0002','/g1/g1/g1/digital-library','/g1/g1/g1',2,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0002','home')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0002','home')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_USER_TYP values ('0001.0001.0001.0002','home')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0000','generic')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0000.0001','deleted')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0000.0002','printout')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001','level1')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001','level2')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001','level3')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001','level4')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0001','text')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0001.0001','ascii text')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0001.0001.0001','ascii compressed Lempel-Ziv')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0001.0001.0002','ascii compressed Huffman')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0001.0002','ebcdic text')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0001.0002.0001','ebcdic compressed Lempel-Ziv')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0001.0002.0002','ebcdic compressed Huffman')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0002','image')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0002.0001','tiff image')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0002.0001.0001','uuencoded tiff')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0002.0002','gif image')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0002.0003','jpeg image')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0002.0004','pbm image')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0002.0005','fig image')
DB20000I  The SQL command completed successfully.

insert into mdas_td_data_typ values ('0001.0001.0001.0001.0002.0006','FITS image')
DB20000I  The SQL command completed successfully.

insert into mdas_td_data_typ values ('0001.0001.0001.0001.0002.0007','DICOM image')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0003','print-format')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0003.0001','LaTeX format')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0003.0002','Troff format')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0003.0003','Postscript format')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0003.0004','DVI format')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0003.0005','Word format')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0004','program code')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0004.0001','SQL script')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0004.0002','C code')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0004.0002.0001','C include file')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0004.0003','fortran code')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0004.0004','object code')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0004.0005','library code')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0004.0006','data file')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0005','html')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0006','SGML File')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0007','Wave Audio')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0008','tar file')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0008.0001','compressed tar file')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0004.0007','java code')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0004.0008','perl script')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0004.0009','tcl script')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0004.000a','link code')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0009','shadow object')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0009.0001','database shadow object')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.0009.0002','directory shadow object')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000a','database')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000b','streams')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000b.0001','audio streams')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000b.0001.0001','realAudio')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000b.0002','video streams')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000b.0002.0001','realVideo')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000b.0002.0002','MPEG')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000b.0002.0003','AVI')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000c','xml')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000d','Slide')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000d.0001','Power Point Slide')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000e','Spread Sheet')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000e.0001','Excel Spread Sheet')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000f','Document')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000f.0001','MSWord Document')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000f.0002','PDF Document')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000g','Executable')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000g.0001','NT Executable')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000g.0002','Solaris Executable')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000g.0003','AIX Executable')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000g.0004','Mac Executable')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000g.0004.0001','Mac OSX Executable')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000g.0005','Cray Executable')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000g.0006','SGI Executable')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000h','DLL')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000h.0001','NT DLL')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000h.0002','Solaris DLL')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000h.0003','AIX DLL')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000h.0004','Mac DLL')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000h.0005','Cray DLL')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000h.0006','SGI DLL')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000i','Movie')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000i.0001','MPEG Movie')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000i.0001.0001','MPEG 3 Movie')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000i.0002','Quicktime Movie')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000j','compressed file')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000j.0001','compressed mmCIF file')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000j.0002','compressed PDB file')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_TYP values ('0001.0001.0001.0001.000k','binary file')
DB20000I  The SQL command completed successfully.

insert into mdas_td_data_typ values ('0001.0001.0001.0001.000l','URL')
DB20000I  The SQL command completed successfully.

insert into mdas_td_data_typ values ('0001.0001.0001.0001.000m','NSF Award Abstracts')
DB20000I  The SQL command completed successfully.

insert into mdas_td_data_typ values ('0001.0001.0001.0001.000n','email')
DB20000I  The SQL command completed successfully.

insert into mdas_td_data_typ values ('0001.0001.0001.0001.000o','orb data')
DB20000I  The SQL command completed successfully.

insert into mdas_td_data_typ values ('0001.0001.0001.0001.000p','datascope data')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0000',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0000.0001',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0000.0002',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0002',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0001','text/text','.txt')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0001.0001','text/text','.txt')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0001.0001.0001',' ','.z,.zip,gz')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0001.0001.0002',' ','.z,.zip,gz')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0001.0002','text/text','.txt')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0001.0002.0001',' ','.z,.zip,gz')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0001.0002.0002',' ','.z,.zip,gz')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0002',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0002.0001','image/tiff','.tif,.tiff')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0002.0001.0001','text/text','.uu')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0002.0002','image/gif','.gif')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0002.0003','image/jpeg','.jpeg,.jpg')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0002.0004','image/pbm','.pbm')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0002.0005','image/fig','.fig')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0002.0006','application/x-fits','.fits,.fit')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0002.0007','application/dicom','.IMA,.ima')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0003',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0003.0001','text/text','.tex')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0003.0002','text/text','.trf,.trof')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0003.0003','application/postscript','.ps')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0003.0004','application/dvi','.dvi')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0003.0005','application/msword','.doc,.rtf')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0004',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0004.0001','text/text','.sql')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0004.0002','text/text','.c')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0004.0002.0001','text/text','.h')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0004.0003','text/text','.f')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0004.0004',' ','.o')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0004.0005',' ','.a')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0004.0006',' ','.dat')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0005','text/html','.htm,.html')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0006','text/sgml','.sgm,.sgml')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0007',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0008','text/text','.tar')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0008.0001',' ','.tz,.tgz')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0004.0007','text/text','.jav,.java')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0004.0008','text/text','.pl')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0004.0009','text/text','.tcl')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0004.000a',' ','.o')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0009',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0009.0001',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0009.0002',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000a',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000b',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000b.0001',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000b.0001.0001','audio/x-pn-realaudio','.ra')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000b.0002',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000b.0002.0001','audio/x-pn-realaudio','.rv')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000b.0002.0002','video/mpeg','.mpeg,.mpg')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000b.0002.0003','video/msvideo','.avi')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000c','text/xml','.xml')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000d',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000d.0001','application/vnd.ms-powerpoint','.ppt')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000e',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000e.0001','application/x-msexcel','.xls')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000f',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000f.0001','application/msword','.doc,.rtf')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000f.0002','application/pdf','.pdf')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000g',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000g.0001',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000g.0002',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000g.0003',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000g.0004',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000g.0004.0001',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000g.0005',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000g.0006',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000h',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000h.0001',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000h.0002',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000h.0003',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000h.0004',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000h.0005',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000h.0006',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000i',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000i.0001','video/mpeg','.mpeg,.mpg')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000i.0001.0001','video/mpeg','.mpeg,.mpg')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000i.0002','video/quicktime','.mov')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000j',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000j.0001',' ','.cif,.mmcif')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000j.0002',' ','.pdb')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000k','text/html','.htm,.html')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000l','text/text',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000m','text/text',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000n',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000o',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000p',' ',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0000','generic')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0000.0001','deleted')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0002','logical')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0003','null')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0004','compound')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001','level1')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001.0001','level2')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001','level3')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001','level4')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001.0001','processor')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001.0001.0001','sp2 processor')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001.0002','database')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001.0002.0001','db2 dblobj database')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001.0002.0002','illustra dblobj database')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001.0002.0003','objectstore dblobj database')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001.0002.0004','oracle dblobj database')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001.0002.0005','sybase dblobj database')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001.0002.0006','postgres dblobj database')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001.0002.0007','db2 table database')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001.0002.0008','illustra table database')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001.0002.0009','objectstore table database')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001.0002.000a','oracle table database')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001.0002.000b','sybase table database')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001.0002.000c','postgres table database')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001.0003','file system')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001.0003.0001','unix file system')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001.0003.0001.0001','linux-unix file system')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001.0003.0002','hpss file system')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001.0003.0003','unitree file system')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001.0003.0004','adsm file system')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001.0003.0005','network file system')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001.0003.0006','http file system')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001.0003.0007','dpss file system')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001.0003.0008','cmstore file system')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001.0003.0009','windows ntfs file system')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001.0004','misc driver system')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001.0004.0001','antelope orb misc driver')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001.0004.0002','datascope misc driver')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001.0005','logicalArch')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001.0006','tape')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_TYP values ('0001.0001.0001.0001.0007','tapeCache')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_CLASS values (0, 'generic')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_CLASS values (1, 'archival')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_CLASS values (2, 'permanent')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_CLASS values (3, 'temporary')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_CLASS values (4, 'volatile')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_CLASS values (5, 'primary archival')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_CLASS values (6, 'primary cache')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_CLASS values (7, 'primary permanent')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_CLASS values (8, 'cache')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_CLASS values (9, 'hierarchical cache')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_CLASS values (10, 'hierarchical archival')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_CLASS values (11, 'hierarchical permanent')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_CLASS values (12, 'hierarchical temporary')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_CLASS values (13, 'hierarchical volatile')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_CLASS values (14, 'logical')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_CLASS values (15, 'compound')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_AUTH_SCHM values (0, 'generic')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_AUTH_SCHM values (1, 'SEA_AUTH')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_AUTH_SCHM values (2, 'GSI_AUTH')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_AUTH_SCHM values (3, 'DCE_AUTH')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_AUTH_SCHM values (4, 'KERBEROS_AUTH')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_AUTH_SCHM values (5, 'PASSWD_AUTH')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_AUTH_SCHM values (6, 'ENCRYPT1')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_USER_TYP values ('0000','deleted')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_USER_TYP values ('0001','sysadmin')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_USER_TYP values ('0001.0001','level2')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_USER_TYP values ('0001.0001.0001','level3')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_USER_TYP values ('0001.0001.0001.0001','level4')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_USER_TYP values ('0001.0001.0001.0001.0001','staff')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_USER_TYP values ('0001.0001.0001.0001.0001.0001','scientist staff')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_USER_TYP values ('0001.0001.0001.0001.0001.0001.0001','senior scientist staff')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_USER_TYP values ('0001.0001.0001.0001.0002','student')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_USER_TYP values ('0001.0001.0001.0001.0002.0001','undergrad student')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_USER_TYP values ('0001.0001.0001.0001.0002.0002','grad student')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_USER_TYP values ('0001.0001.0001.0001.0003','project')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_USER_TYP values ('0001.0001.0001.0001.0004','public')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_USER_TYP values ('0001.0001.0001.0001.0005','group')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_USER_TYP values ('0001.0001.0001.0001.0001.0002','manager')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_USER_TYP values ('0001.0001.0001.0001.0001.0002.0001','assistant manager')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_USER_TYP values ('0001.0001.0001.0001.0001.0002.0002','general manager')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_LOCN (locn_id,locn_desc,netprefix,server_user_id) values ('0000','null','',2)
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_LOCN (locn_id,locn_desc,netprefix,server_user_id) values ('0001','level1','',2)
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_LOCN (locn_id,locn_desc,netprefix,server_user_id) values ('0002','compound','',2)
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_LOCN (locn_id,locn_desc,netprefix,server_user_id) values ('0001.0001','level2','',2)
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_LOCN (locn_id,locn_desc,netprefix,server_user_id) values ('0001.0001.0001','level3','',2)
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_LOCN (locn_id,locn_desc,netprefix,server_user_id) values ('0001.0001.0001.0001','level4','',2)
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_LOCN (locn_id,locn_desc,netprefix,server_user_id) values ('0001.0001.0001.0001.0001','sdsc','sdsc.edu:NULL:NULL',2)
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_LOCN (locn_id,locn_desc,netprefix,server_user_id) values ('0001.0001.0001.0001.0001.0001','mda-18 sdsc','mda-18.sdsc.edu:NULL:NULL',2)
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_LOCN (locn_id,locn_desc,netprefix,server_user_id) values ('0001.0001.0001.0001.0001.0001.0001','unix file mda-18 sdsc','mda-18.sdsc.edu:NULL:NULL',2)
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_LOCN (locn_id,locn_desc,netprefix,server_user_id) values ('0001.0001.0001.0001.0001.0001.0002','db2 mda-18 sdsc','mda-18.sdsc.edu:lgndb2sd:db2v2',2)
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_LOCN (locn_id,locn_desc,netprefix,server_user_id) values ('0001.0001.0001.0001.0001.0002','sage sdsc','sage.sdsc.edu:NULL:NULL',2)
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_LOCN (locn_id,locn_desc,netprefix,server_user_id) values ('0001.0001.0001.0001.0001.0002.0001','file sage sdsc','sage.sdsc.edu:NULL:NULL',2)
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_LOCN (locn_id,locn_desc,netprefix,server_user_id) values ('0001.0001.0001.0001.0001.0002.0002','illustra sage sdsc','sage:srb:NULL',2)
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_LOCN (locn_id,locn_desc,netprefix,server_user_id) values ('0001.0001.0001.0001.0001.0003','rudolph doct sdsc','rudolph.doct.sdsc.edu:NULL:NULL',2)
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_LOCN (locn_id,locn_desc,netprefix,server_user_id) values ('0001.0001.0001.0001.0001.0003.0001','rudolph doct sdsc lobdb1','rudolph.doct.sdsc.edu:/doct5/MDASBlobDB/MDASBlobDB.db:NULL',2)
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_LOCN (locn_id,locn_desc,netprefix,server_user_id) values ('0001.0001.0001.0001.0002','ncsa','ncsa.uiuc.edu:NULL:NULL',2)
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_LOCN (locn_id,locn_desc,netprefix,server_user_id) values ('0001.0001.0001.0001.0001.0001.0003','mda-18 db2 Vault','mda-18.sdsc.edu:vaults:db2v2',2)
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_LOCN (locn_id,locn_desc,netprefix,server_user_id) values ('0001.0001.0001.0001.0001.0002.0003','sage illustra Vault','sage:srb:NULL',2)
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_LOCN (locn_id,locn_desc,netprefix,server_user_id) values ('0001.0001.0001.0001.0001.0004','ghidorah sdsc','ghidorah.sdsc.edu:NULL:NULL',2)
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_LOCN (locn_id,locn_desc,netprefix,server_user_id) values ('0001.0001.0001.0001.0001.0004.0001','ghidorah oracle Vault','ghidorah.sdsc.edu:mcat:foo',2)
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_LOCN (locn_id,locn_desc,netprefix,server_user_id) values ('0001.0001.0001.0002','home','',2)
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_LOCN (locn_id,locn_desc,netprefix,server_user_id) values ('0001.0001.0001.0003','demozoneloc','',2)
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DS_ACCS values (0,'null','','')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DS_ACCS values (10,'execute',' ''execute'',''read audit'',''read'',''write audit'',''write'',''create audit'',''create'',''curate audit'',''curate'',''all audit'',''all'',''annotate audit'',''annotate'' ', ' ''execute'' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DS_ACCS values (20,'read audit', ' ''read audit'',''read'',''write audit'',''write'',''create audit'',''create'',''curate audit'',''curate'',''all audit'',''all'',''annotate audit'',''annotate'' ' , ' ''execute'',''read audit'' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DS_ACCS values (30,'read',' ''read audit'',''read'',''write audit'',''write'',''create audit'',''create'',''curate audit'',''curate'',''all audit'',''all'',''annotate audit'',''annotate'' ', ' ''execute'',''read audit'',''read'' ' )
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DS_ACCS values (33,'annotate audit', ' ''write audit'',''write'',''create audit'',''create'',''curate audit'',''curate'',''all audit'',''all'',''annotate audit'',''annotate'' ', ' ''execute'',''read audit'',''read'',''annotate audit'' ' )
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DS_ACCS values (36,'annotate',' ''write audit'',''write'',''create audit'',''create'',''curate audit'',''curate'',''all audit'',''all'',''annotate audit'',''annotate'' ', ' ''execute'',''read audit'',''read'',''annotate audit'',''annotate'' ' )
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DS_ACCS values (40,'write audit', ' ''write audit'',''write'',''create audit'',''create'',''curate audit'',''curate'',''all audit'',''all'' ', ' ''execute'',''read audit'',''read'',''annotate audit'',''annotate'',''write audit'' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DS_ACCS values (50,'write', ' ''write audit'',''write'',''create audit'',''create'',''curate audit'',''curate'',''all audit'',''all'' ', ' ''execute'',''read audit'',''read'',''annotate audit'',''annotate'',''write audit'',''write'' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DS_ACCS values (140,'create audit', ' ''create audit'',''create'',''curate audit'',''curate'',''all audit'',''all'' ', ' ''execute'',''read audit'',''read'',''annotate audit'',''annotate'',''write audit'',''write'',''create audit'' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DS_ACCS values (150,'create', ' ''create audit'',''create'',''curate audit'',''curate'',''all audit'',''all'' ', ' ''execute'',''read audit'',''read'',''annotate audit'',''annotate'',''write audit'',''write'',''create audit'',''create''  ')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DS_ACCS values (998,'all audit', ' ''curate audit'',''curate'',''all audit'',''all'' ', ' ''execute'',''read audit'',''read'',''annotate audit'',''annotate'',''write audit'',''write'',''create audit'',''create'',''all audit'' ' )
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DS_ACCS values (999,'all', ' ''curate audit'',''curate'',''all audit'',''all'' ', ' ''execute'',''read audit'',''read'',''annotate audit'',''annotate'',''write audit'',''write'',''create audit'',''create'',''all audit'',''all'' ' )
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DS_ACCS values (2998,'curate audit', ' ''curate audit'',''curate'' ', ' ''execute'',''read audit'',''read'',''annotate audit'',''annotate'',''write audit'',''write'',''create audit'',''create'',''all audit'',''all'',''curate audit'' ' )
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DS_ACCS values (2999,'curate', ' ''curate audit'',''curate'' ', ' ''execute'',''read audit'',''read'',''annotate audit'',''annotate'',''write audit'',''write'',''create audit'',''create'',''all audit'',''all'',''curate audit'',''curate'' ' )
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_ACCS values (0,'null','','')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_ACCS values (10,'execute',' ''execute'',''read audit'',''read'',''write audit'',''write'',''create audit'',''create'',''curate audit'',''curate'',''all audit'',''all'',''annotate audit'',''annotate'' ', ' ''execute'' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_ACCS values (20,'read audit', ' ''read audit'',''read'',''write audit'',''write'',''create audit'',''create'',''curate audit'',''curate'',''all audit'',''all'',''annotate audit'',''annotate'' ' , ' ''execute'',''read audit'' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_ACCS values (30,'read',' ''read audit'',''read'',''write audit'',''write'',''create audit'',''create'',''curate audit'',''curate'',''all audit'',''all'',''annotate audit'',''annotate'' ', ' ''execute'',''read audit'',''read'' ' )
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_ACCS values (33,'annotate audit', ' ''write audit'',''write'',''create audit'',''create'',''curate audit'',''curate'',''all audit'',''all'',''annotate audit'',''annotate'' ', ' ''execute'',''read audit'',''read'',''annotate audit'' ' )
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_ACCS values (36,'annotate',' ''write audit'',''write'',''create audit'',''create'',''curate audit'',''curate'',''all audit'',''all'',''annotate audit'',''annotate'' ', ' ''execute'',''read audit'',''read'',''annotate audit'',''annotate'' ' )
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_ACCS values (40,'write audit', ' ''write audit'',''write'',''create audit'',''create'',''curate audit'',''curate'',''all audit'',''all'' ', ' ''execute'',''read audit'',''read'',''annotate audit'',''annotate'',''write audit'' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_ACCS values (50,'write', ' ''write audit'',''write'',''create audit'',''create'',''curate audit'',''curate'',''all audit'',''all'' ', ' ''execute'',''read audit'',''read'',''annotate audit'',''annotate'',''write audit'',''write'' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_ACCS values (140,'create audit', ' ''create audit'',''create'',''curate audit'',''curate'',''all audit'',''all'' ', ' ''execute'',''read audit'',''read'',''annotate audit'',''annotate'',''write audit'',''write'',''create audit'' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_ACCS values (150,'create', ' ''create audit'',''create'',''curate audit'',''curate'',''all audit'',''all'' ', ' ''execute'',''read audit'',''read'',''annotate audit'',''annotate'',''write audit'',''write'',''create audit'',''create''  ')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_ACCS values (998,'all audit', ' ''curate audit'',''curate'',''all audit'',''all'' ', ' ''execute'',''read audit'',''read'',''annotate audit'',''annotate'',''write audit'',''write'',''create audit'',''create'',''all audit'' ' )
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_ACCS values (999,'all', ' ''curate audit'',''curate'',''all audit'',''all'' ', ' ''execute'',''read audit'',''read'',''annotate audit'',''annotate'',''write audit'',''write'',''create audit'',''create'',''all audit'',''all'' ' )
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_ACCS values (2998,'curate audit', ' ''curate audit'',''curate'' ', ' ''execute'',''read audit'',''read'',''annotate audit'',''annotate'',''write audit'',''write'',''create audit'',''create'',''all audit'',''all'',''curate audit'' ' )
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_RSRC_ACCS values (2999,'curate', ' ''curate audit'',''curate'' ', ' ''execute'',''read audit'',''read'',''annotate audit'',''annotate'',''write audit'',''write'',''create audit'',''create'',''all audit'',''all'',''curate audit'',''curate'' ' )
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_ZONE values ('demozone',1,'0001.0001.0001.0003',0,0,1,'2003-08-01-00.00.00','2003-08-01-00.00.00','this is a dummy zone. delete/modify it when installing a real zone','')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (0,'null')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1,'read')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (2,'read audit')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (3,'read anonymous')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (4,'copy')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (5,'move')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (6,'modify')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (7,'execute')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (8,'touch')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (9,'version')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (10,'create')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (11,'delete')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (12,'partition')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (13,'collapse')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (14,'write')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (15,'write audit')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (16,'write anonymous')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (17,'inquire')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (18,'all audit')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (19,'all')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (26,'insert audit')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (27,'change owner')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (28,'change password')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (29,'change size')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (30,'insert domain')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (31,'delete domain')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (32,'change type')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (33,'change group')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (34,'change schema')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (35,'insert access')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (36,'delete access')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (37,'insert alias')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (38,'delete alias')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (39,'insert comments')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (40,'delete comments')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (41,'create new user')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (42,'drop user')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (43,'create dataset')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (44,'create method')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (45,'create resource')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (46,'drop dataset')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (47,'drop method')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (48,'drop resource')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (49,'delete group')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (50,'insert group')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (51,'change verkey')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (52,'change encrkey')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (53,'authorization')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (54,'data inquiry')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (55,'create collection')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (56,'modify user')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (57,'modify dataset')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (58,'modify collection')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (59,'insert collection access')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (60,'delete collection access')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (61,'change data name')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (62,'change collection name')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (63,'delete collection')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (64,'create logical resource')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (65,'update comments')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (66,'append comments')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (67,'create container')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (68,'delete container')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (69,'modify container')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (70,'change size')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (71,'change offset')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (72,'change dirty')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (73,'delete authorization mapping')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (74,'insert authorization mapping')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (75,'modify authorization mapping')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (76,'delete resource authorization')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (77,'insert resource authorization')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (78,'modify resource authorization')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (79,'webCron Move')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (80,'webCron Copy')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (81,'webCron Purge')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (82,'webCron Ingest')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (83,'webCron Export')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (84,'webCron Replicate')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (85,'webCron Cache')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (86,'webCron Archive')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (87,'delete annotations')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (88,'insert annotations')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (89,'update annotations')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (90,'change modify timestamp')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (91,'delete class')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (92,'insert class')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (93,'delete index for data')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (94,'insert index for data')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (95,'delete index for data type')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (96,'insert index for data type')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (97,'delete index for collection')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (98,'insert index for collection')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (99,'delete method for data')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (100,'insert method for data')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (101,'delete method for data type')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (102,'insert method for data type')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (103,'delete method for collection')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (104,'insert method for collection')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (105,'delete structured meta data')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (106,'insert external structured meta data')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (107,'insert internal structured meta data')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (108,'change user defined string meta data')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (109,'change user defined integer meta data')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (110,'insert user defined string meta data')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (111,'insert user defined integer meta data')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (112,'delete user defined string meta data')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (113,'delete user defined integer meta data')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (114,'delete external structured meta data')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (115,'delete internal structured meta data')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (116,'insert structured meta data')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (117,'insert container for collection')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (118,'delete container for collection')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (119,'change user defined integer meta data for collection')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (120,'insert user defined string meta data for collection')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (121,'insert user defined integer meta data for collection')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (122,'delete user defined string meta data for collection')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (123,'delete user defined integer meta data for collection')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (124,'change user defined string meta data for collection')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1001,'read failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1002,'read audit failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1003,'read anonymous failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1004,'copy failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1005,'move failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1006,'modify failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1007,'execute failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1008,'touch failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1009,'version failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1010,'create failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1011,'delete failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1012,'partition failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1013,'collapse failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1014,'write failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1015,'write audit failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1016,'write anonymous failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1017,'inquire failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1018,'all audit failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1019,'all failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1026,'insert audit failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1027,'change owner failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1028,'change password failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1029,'change size failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1030,'insert domain failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1031,'delete domain failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1032,'change type failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1033,'change group failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1034,'change schema failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1035,'insert access failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1036,'delete access failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1037,'insert alias failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1038,'delete alias failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1039,'insert comments failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1040,'delete comments failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1041,'create new user failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1042,'drop user failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1043,'create dataset failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1044,'create method failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1045,'create resource failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1046,'drop dataset failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1047,'drop method failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1048,'drop resource failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1049,'delete group failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1050,'insert group failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1051,'change verkey failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1052,'change encrkey failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1053,'authorization failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1054,'data inquiry failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1055,'create collection failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1056,'modify user failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1057,'modify dataset failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1058,'modify collection failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1059,'insert collection access failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1060,'delete collection access failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1061,'change data name failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1062,'change collection name failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1063,'delete collection failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1064,'create logical resource failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1065,'update comments failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1066,'append comments failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1067,'create container failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1068,'delete container failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1069,'modify container failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1070,'change size failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1071,'change offset failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1072,'change dirty failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1073,'delete authorization mapping failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1074,'insert authorization mapping failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1075,'modify authorization mapping failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1076,'delete resource authorization failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1077,'insert resource authorization failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1078,'modify resource authorization failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1079,'webCron Move failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1080,'webCron Copy failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1081,'webCron Purge failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1082,'webCron Ingest failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1083,'webCron Export failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1084,'webCron Replicate failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1085,'webCron Cache failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1086,'webCron Archive failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1087,'delete annotations failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1088,'insert annotations failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1089,'update annotations failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1090,'change modify timestamp failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1091,'delete class failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1092,'insert class failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1093,'delete index for data failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1094,'insert index for data failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1095,'delete index for data type failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1096,'insert index for data type failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1097,'delete index for collection failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1098,'insert index for collection failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1099,'delete method for data failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1100,'insert method for data failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1101,'delete method for data type failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1102,'insert method for data type failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1103,'delete method for collection failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1104,'insert method for collection failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1105,'delete structured meta data failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1106,'insert external structured meta data failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1107,'insert internal structured meta data failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1108,'change user defined string meta data failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1109,'change user defined integer meta data failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1110,'insert user defined string meta data failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1111,'insert user defined integer meta data failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1112,'delete user defined string meta data failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1113,'delete user defined integer meta data failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1114,'delete external structured meta data failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1115,'delete internal structured meta data failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1116,'insert structured meta data failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1117,'insert container for collection failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1118,'delete container for collection failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1119,'change user defined integer meta data for collection failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1120,'insert user defined string meta data for collection failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1121,'insert user defined integer meta data for collection failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1122,'delete user defined string meta data for collection failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1123,'delete user defined integer meta data for collection failure')
DB20000I  The SQL command completed successfully.

insert into  mdas_td_actn values (1124,'change user defined string meta data for collection failure')
DB20000I  The SQL command completed successfully.

insert into MDAS_CD_USER values(0,'unknown', '0001.0001.0001.0001.0001','demozone','1997-08-15-00.00.00','1997-08-15-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_DOMN values (0,'0002')
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_GROUP values (0,0)
DB20000I  The SQL command completed successfully.

insert into MDAS_CD_USER values (1,'public','0001.0001.0001.0001.0004','demozone','1997-08-15-00.00.00','1997-08-15-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_DOMN values (1,'0001.0001.0001.0001.0001')
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_GROUP values (1,1)
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_INFO values (1,'public', '','sekar@sdsc.edu')
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_AUTH_KEY values (1, 'CANDO')
DB20000I  The SQL command completed successfully.

insert into MDAS_CD_USER values (2,'srb','0001','demozone','1997-08-15-00.00.00','1997-08-15-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_DOMN values (2,'0001.0001.0001.0001.0001.0001')
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_GROUP values (2,2)
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_GROUP values (2,1)
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_INFO values (2,'Mike Wan, SDSC, University of California, San Diego, CA', '619 534-5029','mwan@sdsc.edu')
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_AUTH_KEY values (2, 'CANDO')
DB20000I  The SQL command completed successfully.

insert into MDAS_CD_USER values (3,'npaci','0001.0001.0001.0001.0005','demozone','1997-08-15-00.00.00','1997-08-15-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_DOMN values (3,'0001.0001.0001.0001.0002')
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_GROUP values (3,3)
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_INFO values (3,'NPACI', '619 534-5029','srb@sdsc.edu')
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_AUTH_KEY values (3, 'CANDO')
DB20000I  The SQL command completed successfully.

insert into MDAS_CD_USER values (4,'sdsc','0001.0001.0001.0001.0005','demozone','1997-08-15-00.00.00','1997-08-15-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_DOMN values (4,'0001.0001.0001.0001.0002')
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_GROUP values (4,4)
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_INFO values (4,'SDSC, CA', '619 534-8378','srb@sdsc.edu')
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_AUTH_KEY values (4, 'CANDO')
DB20000I  The SQL command completed successfully.

insert into MDAS_CD_USER values (6,'ticketuser','0001.0001.0001.0001.0004','demozone','1997-08-15-00.00.00','1997-08-15-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_DOMN values (6,'0001.0001.0001.0001.0001.0001')
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_GROUP values (6,6)
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_INFO values (6,'public', '','srb@sdsc.edu')
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_AUTH_KEY values (6, 'CANDO')
DB20000I  The SQL command completed successfully.

insert into MDAS_CD_USER values (7,'testuser','0001.0001.0001.0001.0004','demozone','1997-08-15-00.00.00','1997-08-15-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_DOMN values (7,'0001.0001.0001.0001.0001.0001')
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_GROUP values (7,7)
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_GROUP values (7,1)
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_INFO values (7,'testuser', '','srb@sdsc.edu')
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_AUTH_KEY values (7, 'TESTUSER')
DB20000I  The SQL command completed successfully.

insert into MDAS_CD_USER values (8,'system','0001.0001.0001.0001.0005','demozone','1997-08-15-00.00.00','1997-08-15-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_DOMN values (8,'0001.0001.0001.0001.0002')
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_GROUP values (8,8)
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_GROUP values (2,8)
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_INFO values (8,'SDSC, CA', '619 534-8378','srb@sdsc.edu')
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_AUTH_KEY values (8, 'CANDO')
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0001.0001.0001.0001',1,20)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0001.0001.0001.0001',2,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0001.0001.0001.0001',3,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0001.0001.0001.0001',4,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0001.0001.0001.0001',6,30)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0001.0001.0001.0001',7,50)
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_AUTH_MAP values (3,'/C=US/O=NPACI/OU=SDSC/USERID=mwan/CN=Michael Wan/Email=mwan@sdsc.edu' , 2)
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_AUTH_MAP values (2,'/C=US/O=NPACI/OU=SDSC/USERID=srb/CN=Storage Resource Broker/Email=srb@sdsc.edu' , 2)
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_AUTH_MAP values (0,'NULL' , 6)
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_AUTH_MAP values (1,'NULL' , 6)
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_AUTH_MAP values (4,'NULL' , 6)
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_AUTH_MAP values (5,'NULL' , 6)
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_AUTH_MAP values (6,'NULL' , 6)
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_AUTH_MAP values (7,'NULL' , 6)
DB20000I  The SQL command completed successfully.

insert into MDAS_AU_AUTH_MAP values (8,'NULL' , 6)
DB20000I  The SQL command completed successfully.

insert into MDAS_CD_RSRC values (-1,'pnull','0003')
DB20000I  The SQL command completed successfully.

insert into MDAS_CD_RSRC values (0,'null','0000')
DB20000I  The SQL command completed successfully.

insert into MDAS_AR_PHYSICAL values (0,0,'null','0000','0000','1997-08-15-00.00.00',2,'',0,0,0,0,'0')
DB20000I  The SQL command completed successfully.

insert into MDAS_AR_REPL values (0,0,'null','0000','0000','1997-08-15-00.00.00',2,0,'',0,0,'2003-09-01-00.00.00','2003-09-01-00.00.00',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_CD_RSRC values (1,'sdsc-mda18-fs','0001.0001.0001.0001.0003.0001')
DB20000I  The SQL command completed successfully.

insert into MDAS_AR_PHYSICAL values (1,0,'sdsc-mda18-fs','0001.0001.0001.0001.0003','0001.0001.0001.0001.0001.0001.0001','1997-08-15-00.00.00',2,'/scratch0/srb/SRBVault/?USER.?DOMAIN/TEST.?PATH?DATANAME.?RANDOM',0,2,0,0,'0')
DB20000I  The SQL command completed successfully.

insert into MDAS_AR_REPL values (1,0,'sdsc-mda18-fs','0001.0001.0001.0001.0003','0001.0001.0001.0001.0001.0001.0001','1997-08-15-00.00.00',2,1,'/scratch0/srb/SRBVault/?USER.?DOMAIN/TEST.?PATH?DATANAME.?RANDOM',0,2,'2003-09-01-00.00.00','2003-09-01-00.00.00',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_CD_RSRC values (10,'sdsc-fs','0001.0001.0001.0001.0003.0001')
DB20000I  The SQL command completed successfully.

insert into MDAS_AR_PHYSICAL values (10,0,'sdsc-fs','0001.0001.0001.0001.0003.0001','0001.0001.0001.0001.0001.0001.0001','1997-08-15-00.00.00',2,'/projects/mdas/srb/SRBVaultTest//?USER.?DOMAIN/TEST.?PATH?DATANAME.?RANDOM',0,2,0,0,'0')
DB20000I  The SQL command completed successfully.

insert into MDAS_AR_REPL values (10,0,'sdsc-fs','0001.0001.0001.0001.0003.0001','0001.0001.0001.0001.0001.0001.0001','1997-08-15-00.00.00',2,10,'/projects/mdas/srb/SRBVaultTest//?USER.?DOMAIN/TEST.?PATH?DATANAME.?RANDOM',0,2,'2003-09-01-00.00.00','2003-09-01-00.00.00',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_CD_RSRC values (2,'ora-sdsc','0001.0001.0001.0001.0002.0004')
DB20000I  The SQL command completed successfully.

insert into MDAS_AR_PHYSICAL values (2,0,'ora-sdsc','0001.0001.0001.0001.0002.0004','0001.0001.0001.0001.0001.0004.0001','1997-08-15-00.00.00',2,'/srbVault/OBJ_LOCATOR/OBJ_ID=%27TEST.?PATH/?DATANAME.?RANDOM%27',0,2,0,0,'0')
DB20000I  The SQL command completed successfully.

insert into MDAS_AR_REPL values (2,0,'ora-sdsc','0001.0001.0001.0001.0002.0004','0001.0001.0001.0001.0001.0004.0001','1997-08-15-00.00.00',2,2,'/srbVault/OBJ_LOCATOR/OBJ_ID=%27TEST.?PATH/?DATANAME.?RANDOM%27',0,2,'2003-09-01-00.00.00','2003-09-01-00.00.00',' ')
DB20000I  The SQL command completed successfully.

insert into MDAS_AR_ACCS values (1,0,1,20)
DB20000I  The SQL command completed successfully.

insert into MDAS_AR_ACCS values (1,0,2,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AR_ACCS values (1,0,3,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AR_ACCS values (1,0,4,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AR_ACCS values (1,0,5,50)
DB20000I  The SQL command completed successfully.

insert into MDAS_AR_ACCS values (1,0,6,20)
DB20000I  The SQL command completed successfully.

insert into MDAS_AR_ACCS values (1,0,7,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_CD_DATA values (0,'null','0000')
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_REPL (data_id,repl_enum,data_name,data_typ_id,path_name,rsrc_id,repl_timestamp,data_grp_id,data_size,is_deleted,data_owner,is_dirty,offset,container_id,modify_timestamp,last_accs_time) values (0,0,'null','0000','/tmp/null',1,'1997-08-15-00.00.00','0001.0001.0001.0001',100,0,2,0,0,1,'1997-08-15-00.00.00','1997-08-15-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_CD_DATA values (1,'srbdatatest1','0000')
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_REPL (data_id,repl_enum,data_name,data_typ_id,path_name,rsrc_id,repl_timestamp,data_grp_id,data_size,is_deleted,data_owner,is_dirty,offset,container_id,modify_timestamp,last_accs_time) values (1,0,'srbdatatest1','0000','/tmp/srbdatatest1',1,'1997-08-15-00.00.00','0001.0001.0001.0001',100,0,2,0,0,1,'1997-08-15-00.00.00','1997-08-15-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_CONTAINER (container_id,container_name,container_max_size,rsrc_id) values    (0,'',1000000000,1)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_ACCS values (1,0,1,20)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_ACCS values (1,0,2,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_ACCS values (1,0,3,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_ACCS values (1,0,4,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_ACCS values (1,0,5,50)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_ACCS values (1,0,6,50)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_ACCS values (1,0,7,50)
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0000','/','/null',2,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('000a','/demozone', '/',2,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0002','/demozone/home', '/demozone',2,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0002.0001','/demozone/home/public.npaci', '/demozone/home',1,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0002.0002','/demozone/home/srb.sdsc', '/demozone/home',2,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0002.0003','/demozone/home/npaci.groups', '/demozone/home',3,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0002.0004','/demozone/home/sdsc.groups', '/demozone/home',4,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0002.0006','/demozone/home/ticketuser.sdsc', '/demozone/home',6,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0002.0007','/demozone/home/testuser.sdsc', '/demozone/home',7,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0003','/demozone/srbtest', '/null',2,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0004','/demozone/container', '/demozone',2,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0004.0001','/demozone/container/public.npaci', '/demozone/container',1,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0004.0002','/demozone/container/srb.sdsc', '/demozone/container',2,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0004.0003','/demozone/container/npaci.groups', '/demozone/container',3,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0004.0004','/demozone/container/sdsc.groups', '/demozone/container',4,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0004.0006','/demozone/container/ticketuser.sdsc', '/demozone/container',6,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0004.0007','/demozone/container/testuser.sdsc', '/demozone/container',7,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0005','/demozone/styles', '/demozone',2,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0006','/home', '/',2,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0006.0001','/home/public.npaci', '/home',1,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0006.0002','/home/srb.sdsc', '/home',2,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0006.0003','/home/npaci.groups', '/home',3,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0006.0004','/home/sdsc.groups', '/home',4,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0006.0006','/home/ticketuser.sdsc', '/home',6,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0006.0007','/home/testuser.sdsc', '/home',7,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0007','/srbtest', '/null',2,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0008','/container', '/',2,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0008.0001','/container/public.npaci', '/container',1,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0008.0002','/container/srb.sdsc', '/container',2,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0008.0003','/container/npaci.groups', '/container',3,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0008.0004','/container/sdsc.groups', '/container',4,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0008.0006','/container/ticketuser.sdsc', '/container',6,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0008.0007','/container/testuser.sdsc', '/container',7,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_TD_DATA_GRP values ('0009','/styles', '/',2,'2003-09-01-00.00.00','',0,0,'0',0,'2003-09-01-00.00.00')
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0006',2,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0006.0001',1,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0006.0002',2,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0006.0003',3,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0006.0004',4,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0006.0005',5,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0006.0006',6,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0006.0007',7,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0007',1,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0007',2,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0007',3,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0007',4,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0007',5,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0007',6,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0007',7,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0008',2,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0008.0001',1,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0008.0002',2,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0008.0003',3,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0008.0004',4,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0008.0005',5,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0008.0006',6,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0008.0007',7,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0009',2,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('000a',2,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0005',2,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0002',2,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0002.0001',1,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0002.0002',2,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0002.0003',3,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0002.0004',4,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0002.0005',5,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0002.0006',6,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0002.0007',7,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0003',1,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0003',2,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0003',3,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0003',4,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0003',5,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0003',6,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0003',7,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0004',2,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0004.0001',1,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0004.0002',2,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0004.0003',3,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0004.0004',4,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0004.0005',5,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0004.0006',6,999)
DB20000I  The SQL command completed successfully.

insert into MDAS_AD_GRP_ACCS values ('0004.0007',7,999)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_DELETE_ONE',  1)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_DELETE_DOMN', 2)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_INSERT_DOMN', 3)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_CHANGE_SIZE',       4           )
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_CHANGE_TYPE',       5    )
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_CHANGE_GROUP',          6)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_CHANGE_SCHEMA',     7)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_INSERT_ACCS',       8)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_DELETE_ACCS',       9)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_DELETE_ALIAS',     10)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_INSERT_ALIAS',     11)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_DELETE_COMMENTS',  12)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_INSERT_COMMENTS',  13)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_INSERT_AUDIT',     14)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_CHANGE_OWNER',     15)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('U_CHANGE_PASSWORD',  16)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('U_DELETE',     17)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('U_DELETE_DOMN', 18)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('U_INSERT_DOMN', 19)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('U_DELETE_GROUP',20)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('U_INSERT_GROUP',21)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('U_CHANGE_TYPE', 22    )
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('U_DELETE_ALIAS',23)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('U_INSERT_ALIAS',24)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('U_CHANGE_VERKEY',    25 )
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('U_CHANGE_ENCKEY',    26 )
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_INSERT_COLL_ACCS', 27)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_DELETE_COLL_ACCS', 28)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_CHANGE_DNAME',29)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_CHANGE_COLL_NAME', 30)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_DELETE_COLL', 31)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_UPDATE_COMMENTS',  32)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_APPEND_COMMENTS',  33)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('SU_CHANGE_PASSWORD', 34)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_CHANGE_OFFSET',    35)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_CHANGE_DIRTY',36)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('U_DELETE_AUTH_MAP',  37)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('U_INSERT_AUTH_MAP',  38)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_DELETE_ANNOTATIONS',    39)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_INSERT_ANNOTATIONS',    40)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_UPDATE_ANNOTATIONS',    41)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_CHANGE_MODIFY_TIMESTAMP',    42)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_DELETE_CLASS',    43)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_INSERT_CLASS',    44)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_DELETE_INDEX_FOR_DATA',    45)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_INSERT_INDEX_FOR_DATA',    46)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_DELETE_INDEX_FOR_DATATYPE',    47)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_INSERT_INDEX_FOR_DATATYPE',    48)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_DELETE_INDEX_FOR_COLLECTION',    49)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_INSERT_INDEX_FOR_COLLECTION',    50)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_DELETE_METHOD_FOR_DATA',    51)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_INSERT_METHOD_FOR_DATA',    52)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_DELETE_METHOD_FOR_DATATYPE',53)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_INSERT_METHOD_FOR_DATATYPE',54)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_DELETE_METHOD_FOR_COLLECTION',   55)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_INSERT_METHOD_FOR_COLLECTION ',  56 )
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_DELETE_STRUCT_METADATA',    57)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_INSERT_EXTERNAL_STRUCT_METADATA',   58  )
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_INSERT_INTERNAL_STRUCT_METADATA',   59)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_CHANGE_USER_DEFINED_STRING_META_DATA',   60  )
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_CHANGE_USER_DEFINED_INTEGER_META_DATA',  61  )
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_INSERT_USER_DEFINED_STRING_META_DATA',   62)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_DELETE_USER_DEFINED_STRING_META_DATA',   63)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_INSERT_USER_DEFINED_INTEGER_META_DATA',  64)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_DELETE_USER_DEFINED_INTEGER_META_DATA',  65)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('C_INSERT_CONTAINER_FOR_COLLECTION',   66)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('C_DELETE_CONTAINER_FOR_COLLECTION',   67)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('C_CHANGE_USER_DEFINED_COLL_STRING_META_DATA',   68  )
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('C_CHANGE_USER_DEFINED_COLL_INTEGER_META_DATA',  69  )
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('C_INSERT_USER_DEFINED_COLL_STRING_META_DATA',   70)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('C_INSERT_USER_DEFINED_COLL_INTEGER_META_DATA',  71)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('C_DELETE_USER_DEFINED_COLL_STRING_META_DATA',   72)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('C_DELETE_USER_DEFINED_COLL_INTEGER_META_DATA',  73)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('R_DELETE_ACCS',   74)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('R_INSERT_ACCS',   75)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_CHANGE_LAST_ACCS_TIME',   76)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_DELETE_ATTR',   77)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_INSERT_ATTR',   78)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_MODIFY_ATTR',   79)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_DELETE_MULTI_ATTR',  80)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_INSERT_MULTI_ATTR',  81)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_MODIFY_MULTI_ATTR',  82)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_INSERT_LINK',   83)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_UPDATE_COLL_COMMENTS',    84)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_INSERT_LOCK',   85)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_UPGRADE_LOCK',  86)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_DELETE_LOCK',   87)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_CHANGE_VERSION',88)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_DELETE_ALL_FROM_CONTAINER',    89)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_EXTRACT_TEMPLATE_METADATA',    90)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('C_COPY_META_DATA_FROM_COLL_TO_COLL',  91)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_COPY_META_DATA_FROM_COLL_TO_DATA',  92)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('C_COPY_META_DATA_FROM_DATA_TO_COLL',  93)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_COPY_META_DATA_FROM_DATA_TO_DATA',  94)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_COPY_ANNOTATIONS_FROM_DATA_TO_DATA',95)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_READ',96)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_WRITE',    97)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_CREATE',   98)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_REPLICATE',99)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_MOVE',    100)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('C_CREATE',  101)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_TICKET',  102)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('C_TICKET',  103)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_LINK',    104)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_BULKINGEST',   105)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_DELETECONT',   106)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_AUDIT_SUCCESS',107)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_AUDIT_FAILURE',108)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_CONTAINER',    109)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('C_DELETE_ANNOTATIONS',110)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('C_INSERT_ANNOTATIONS',111)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('C_UPDATE_ANNOTATIONS',112)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('C_COPY_ANNOTATIONS_FROM_COLL_TO_COLL',    113)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_COMPOUND_CREATE',   114)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_DELETE_COMPOUND',   115)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_DELETE_SUBCOMPOUND',116)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_COMPOUND_MODIFY_CMPD_PATH_NAME',   117)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_COMPOUND_MODIFY_MODIFY_TIMESTAMP', 118)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_COMPOUND_MODIFY_LAST_ACCS_TIME',   119)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_COMPOUND_MODIFY_SIZE',   120)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_COMPOUND_MODIFY_OFFSET', 121)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_COMPOUND_MODIFY_IS_DIRTY',    122)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_COMPOUND_SET_MODIFY_TIMESTAMP',    123)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_COMPOUND_SET_LAST_ACCS_TIME', 124)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_COMPOUND_MODIFY_SIZE_AND_DIRTY',   125)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_CHANGE_DPATH', 126)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_INSERT_INCOLL_ACCS',127)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_INSERT_INCOLL_ACCS_RECUR',    128)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_DELETE_INCOLL_ACCS',129)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_DELETE_INCOLL_ACCS_RECUR',    130)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('C_INSERT_LINK',  131)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_CHANGE_REPLNUM_VERNUM',  132)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('T_INSERT_TAPE_INFO',  133)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('T_UPDATE_TAPE_INFO',  134)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('T_UPDATE_TAPE_INFO_2',135)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('T_DELETE_TAPE_INFO',  136)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_INSERT_DCHECKSUM',  137)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_INSERT_DHIDE', 138)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_INSERT_AUDITFLAG',  139)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_UPDATE_PIN',   140)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_UPDATE_DEXPIRE_DATE',    141)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_UPDATE_DEXPIRE_DATE_2',  142)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_UPDATE_DCOMPRESSED',143)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_UPDATE_DENCRYPTED',   144)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_COPY_META_DATA_FROM_CONTAINER_TO_NEW_CONTAINER',  145)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_UPDATE_CONTAINER_FOR_COLLECTION', 146)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('D_BULK_INSERT_UDEF_META_DATA_FOR_DATA',  147)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('C_BULK_INSERT_UDEF_META_DATA_FOR_COLL',148)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('U_BULK_INSERT_UDEF_META_DATA_FOR_USER',  149)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('R_BULK_INSERT_UDEF_META_DATA_FOR_RSRC',   150)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('Z_INSERT_NEW_LOCAL_ZONE',151)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('Z_INSERT_NEW_ALIEN_ZONE', 152)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('Z_MODIFY_ZONE_INFO', 153)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('Z_MODIFY_ZONE_FOR_USER',  154)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('Z_CHANGE_ZONE_NAME',  155)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('Z_MODIFY_ZONE_LOCAL_FLAG',  156)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('Z_MODIFY_ZONE_STATUS',  157)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('C_DELETE_MULTI_ATTR', 158)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('C_INSERT_MULTI_ATTR',  159)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('C_MODIFY_MULTI_ATTR',  160)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('C_DELETE_ATTR',       161)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('C_INSERT_ATTR',        162)
DB20000I  The SQL command completed successfully.

insert into mdas_audit_desc (actionDesc,actionid) values ('C_MODIFY_ATTR',       163)
DB20000I  The SQL command completed successfully.

select count(*) from MDAS_TD_DOMN

1          
-----------
          9

  1 record(s) selected.


select count(*) from MDAS_TD_DATA_GRP

1          
-----------
         39

  1 record(s) selected.


select count(*) from MDAS_TD_DATA_TYP

1          
-----------
         94

  1 record(s) selected.


select count(*) from MDAS_TD_RSRC_TYP

1          
-----------
         42

  1 record(s) selected.


select count(*) from MDAS_TD_USER_TYP

1          
-----------
         18

  1 record(s) selected.


select count(*) from MDAS_TD_LOCN

1          
-----------
         22

  1 record(s) selected.


select count(*) from MDAS_TD_ACTN

1          
-----------
        237

  1 record(s) selected.


select count(*) from MDAS_TD_RSRC_ACCS

1          
-----------
         14

  1 record(s) selected.


select count(*) from MDAS_TD_DS_ACCS

1          
-----------
         14

  1 record(s) selected.


select count(*) from MDAS_CD_USER

1          
-----------
          8

  1 record(s) selected.


select count(*) from MDAS_AU_INFO

1          
-----------
          7

  1 record(s) selected.


select count(*) from MDAS_AU_GROUP

1          
-----------
         11

  1 record(s) selected.


select count(*) from MDAS_AU_DOMN

1          
-----------
          8

  1 record(s) selected.


select count(*) from MDAS_AU_AUTH_KEY

1          
-----------
          7

  1 record(s) selected.


select count(*) from MDAS_AU_AUDIT

1          
-----------
          0

  1 record(s) selected.


select count(*) from MDAS_CD_RSRC

1          
-----------
          5

  1 record(s) selected.


select count(*) from MDAS_AR_REPL

1          
-----------
          4

  1 record(s) selected.


select count(*) from MDAS_AR_ACCS

1          
-----------
          7

  1 record(s) selected.


select count(*) from MDAS_AR_AUDIT

1          
-----------
          0

  1 record(s) selected.


select count(*) from MDAS_CD_DATA

1          
-----------
          2

  1 record(s) selected.


select count(*) from MDAS_AD_REPL

1          
-----------
          2

  1 record(s) selected.


select count(*) from MDAS_AD_ACCS

1          
-----------
          7

  1 record(s) selected.


select count(*) from MDAS_AD_GRP_ACCS

1          
-----------
         55

  1 record(s) selected.


select count(*) from MDAS_AD_AUDIT

1          
-----------
          0

  1 record(s) selected.


select count(*) from MDAS_COUNTER

1          
-----------
          7

  1 record(s) selected.


update mdas_td_data_grp set data_grp_id = '0000'  where data_grp_name = '/'
DB20000I  The SQL command completed successfully.

update mdas_td_data_grp set coll_mod_timestamp = coll_cr_timestamp
DB20000I  The SQL command completed successfully.

