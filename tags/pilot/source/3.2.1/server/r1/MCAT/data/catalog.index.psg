create index mdas_cmdata_idx2	
	on MDAS_AD_COLLMDATA(userdef_metastr0) ;

create index mdas_cmdata_idx3	
	on MDAS_AD_COLLMDATA(userdef_metastr1) ;

create index mdas_ad_mdata_idx1	
	on MDAS_AD_MDATA(userdef_metastr0) ;

create index mdas_ad_mdata_idx2	
	on MDAS_AD_MDATA(userdef_metastr1) ;

create index mdas_td_domn_inx	
	on MDAS_TD_DOMN(domain_desc) ;
create index mdas_td_dgrp_inx
	on MDAS_TD_DATA_GRP(data_grp_name) ;
create index mdas_ad_acs_moid 
       on MDAS_AD_ACCS (data_id) ;
create index mdas_ad_acs_usid 
       on MDAS_AD_ACCS (user_id) ;
create index mdas_ad_rep_dnam
       on MDAS_AD_REPL (data_name) ;
create index mdas_ad_rep_dnagrp
       on MDAS_AD_REPL (data_name,DATA_GRP_ID) ;
create index mdas_ad_rep_grpid
       on MDAS_AD_REPL (DATA_GRP_ID) ;
create index mdas_ad_rep_pathrs
       on MDAS_AD_REPL (path_name,rsrc_id)  ;
create index mdas_ad_rep_dataid
       on MDAS_AD_REPL (data_id)  ;
create index mdas_ad_rep_contid
       on MDAS_AD_REPL (container_id)  ;
create index mdas_td_cont_conid
       on MDAS_TD_CONTAINER (container_id)  ;
create index mdas_td_cont_name
       on MDAS_TD_CONTAINER (container_name) ;
create index mdas_ad_phyrsr_inx
       on MDAS_AR_PHYSICAL (phy_rsrc_id) ;
create index mdas_au_dm_monam
       on MDAS_AU_DOMN (user_id,domain_id) ;
create index mdas_cd_usrnam_inx 
       on MDAS_CD_USER (user_name) ;
create index mdas_au_grp_2moid
       on MDAS_AU_GROUP (group_user_id) ;
create index mdas_au_grp_3moid
       on MDAS_AU_GROUP (group_user_id, user_id) ;
create index mdas_ad_colinx_col on mdas_ad_coll_index (INDEXED_DATCOLL_ID) ;
create index mdas_ad_colinx_dat on mdas_ad_coll_index (DATA_ID_OF_INDEX) ;
create index mdas_ad_rep_pat  on MDAS_AD_REPL (path_name)  ;
create index mdas_ad_minx_col on mdas_ad_coll_meth (METHFOR_DATCOLL_ID)  ;
create index mdas_ad_minx_dat on mdas_ad_coll_meth (DATA_ID_OF_METH)  ;
create index mdas_ad_colcnt_col  on mdas_ad_collcont (DATA_GRP_ID) ;
create index mdas_ad_colmd_col  on mdas_ad_collmdata (DATA_GRP_ID) ;
create index mdas_ad_graccs_cid  on MDAS_AD_GRP_ACCS (data_grp_id)   ;
 

create index mdas_au_grp_moid
       on MDAS_AU_GROUP (user_id) ;

create index mdas_audit_did
       on MDAS_AUDIT (dataid) ;
create index mdas_audit_cid
       on MDAS_AUDIT (collName) ;
create index mdas_aud_desc_acid
       on MDAS_AUDIT_DESC (actionid) ;

