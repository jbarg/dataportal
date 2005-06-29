--- new table 
create table MDAS_TD_ZONE
         (zone_id           varchar(250) not null,
 	 local_zone_flag   int   default 0,
 	 locn_id           varchar(250),
 	 port_number       int,
 	 user_id           int,
          zone_status       int   default 1,
          zone_create_date  varchar(32),
 	 zone_modify_date  varchar(32),
 	 zone_comments     varchar(250),
 	 zone_contact      varchar(250)
         ) ;

create table MDAS_TD_DEL_DATA_GRP
        (data_grp_id     varchar(250),
         data_grp_name      varchar(500),
         parent_grp_name      varchar(500) ,
         coll_owner integer,    
         coll_cr_timestamp varchar(32),
         coll_comments varchar(700),
         coll_lock       integer,
         coll_lockowner      integer DEFAULT  0,
         coll_lockexpiry varchar(32),
         coll_link       integer ,
         coll_mod_timestamp varchar(32)
        ) ;

create table MDAS_AD_DEL_REPL
        (data_id integer  not null,                  
         repl_enum integer  not null,                
         data_name  varchar(250)  not null,                  
         data_typ_id varchar(250)   not null,        
         path_name varchar(699),
         rsrc_id integer  not null,                  
         repl_timestamp VARCHAR(32)  not null ,      
         data_grp_id varchar(250)  not null ,        
         data_size bigint,                                   
         is_deleted integer not null,                        
         data_owner integer  not null,  
         is_dirty   integer,
         offset     bigint,
         container_id integer,    
         modify_timestamp varchar(32),      
         last_accs_time varchar(32),
         datalock       integer DEFAULT 0,
         lockowner      integer DEFAULT  0,
         version_num    varchar(250) DEFAULT '0',
         container_td_id integer,
         d_comment varchar(250),
         dlockexpiry varchar(32) DEFAULT '0',
         dchecksum varchar(400) DEFAULT '0',
         dhide    integer  DEFAULT 0,
         auditflag   integer DEFAULT 0,
         seg_num integer DEFAULT -1
        ) ;

create table MDAS_AD_GUID
        (data_id integer  not null,
         guid    varchar(400),
         guid_flag integer,
         guid_time_stamp varchar(32)         
        );

insert into MDAS_TD_LOCN (locn_id,locn_desc,netprefix,server_user_id) values ('0001.0001.0001.0003','demozoneloc','',2);

insert into MDAS_TD_ZONE values ('demozone',1,'0001.0001.0001.0003',0,0,1,'2003-08-01-00.00.00','2003-08-01-00.00.00','this is a dummy zone. delete/modify it when installing a real zone','');

--- modify tables 

alter table mdas_td_data_grp add coll_mod_timestamp varchar(32);


update MDAS_TD_DATA_GRP
set data_grp_id = '0000'  where data_grp_name = '/';
commit;

insert into MDAS_TD_DATA_GRP values ('000a','/demozone', '/',2,'','',0,0,'0',0,'2003-09-01-00.00.00');
insert into MDAS_AD_GRP_ACCS values ('000a',2,999);
insert into MDAS_AD_GRP_ACCS values ('000a',8,999);

insert into MDAS_TD_DATA_GRP values ('000a.0000','/demozone/home', '/demozone',2,'','',0,0,'0',0,'2003-09-01-00.00.00');
insert into MDAS_TD_DATA_GRP values ('000a.0001','/demozone/container', '/demozone',2,'','',0,0,'0',0,'2003-09-01-00.00.00');

insert into MDAS_AD_GRP_ACCS values ('000a.0000',2,999);
insert into MDAS_AD_GRP_ACCS values ('000a.0000',8,999);
insert into MDAS_AD_GRP_ACCS values ('000a.0001',2,999);
insert into MDAS_AD_GRP_ACCS values ('000a.0001',8,999);

insert into MDAS_AD_GRP_ACCS values ('0000',2,999);
insert into MDAS_AD_GRP_ACCS values ('0000',8,999);

commit;
 
alter table MDAS_CD_USER add  zone_id  varchar(250);
alter table MDAS_CD_USER add  user_creation_date varchar(32);
alter table MDAS_CD_USER add  user_modify_date  varchar(32) ;

update MDAS_CD_USER
set zone_id ='demozone'  ,
    user_creation_date = '2003-09-01-00.00.00'  ,
    user_modify_date = '2003-09-01-00.00.00'
;

commit;

alter table MDAS_AR_REPL add rsrc_creation_date varchar(32);
alter table MDAS_AR_REPL add rsrc_modify_date  varchar(32);
alter table MDAS_AR_REPL add  rsrc_comments     varchar(250);

update MDAS_AR_REPL
set rsrc_creation_date='2003-09-01-00.00.00'    ,
    rsrc_modify_date='2003-09-01-00.00.00'      
;

commit;

alter table MDAS_AD_MDATA add userdef_creat_date varchar(32);
alter table MDAS_AD_MDATA add userdef_modif_date  varchar(32);

update MDAS_AD_MDATA
set userdef_creat_date ='2003-09-01-00.00.00'   , 
    userdef_modif_date='2003-09-01-00.00.00'
;

commit;
 	 

alter table MDAS_AD_COLLMDATA add userdef_creat_date varchar(32);
alter table MDAS_AD_COLLMDATA add userdef_modif_date  varchar(32);

update  MDAS_AD_COLLMDATA 
set userdef_creat_date = '2003-09-01-00.00.00'  ,
    userdef_modif_date= '2003-09-01-00.00.00'
;

commit;


alter table MDAS_AR_MDATA add  userdef_creat_date varchar(32);
alter table MDAS_AR_MDATA add userdef_modif_date  varchar(32) ;

update MDAS_AR_MDATA
set userdef_creat_date ='2003-09-01-00.00.00'    ,
    userdef_modif_date= '2003-09-01-00.00.00' 
;

commit;

alter table MDAS_AU_MDATA add userdef_creat_date varchar(32);
alter table MDAS_AU_MDATA add userdef_modif_date  varchar(32);

update MDAS_AU_MDATA
set userdef_creat_date = '2003-09-01-00.00.00'   ,
    userdef_modif_date = '2003-09-01-00.00.00' 
;

commit;

 create alias MDAS_TD_ZN_LOCN  as SELECT * from  MDAS_TD_LOCN;
 create alias MDAS_CD_ZN_USER  as SELECT * from MDAS_CD_USER;
 create alias MDAS_TD_ZN_DOMN  as SELECT * from MDAS_TD_DOMN;
 create alias MDAS_AU_ZN_DOMN  as SELECT * from MDAS_AU_DOMN;
 create alias MDAS_AU_ZN_ATHMAP   as SELECT * from MDAS_AU_AUTH_MAP;
 create alias MDAS_TD_ZN_ATHSCHM  as SELECT * from MDAS_TD_AUTH_SCHM;
 create alias MDAS_TD_PAR_DOMN    as SELECT * from MDAS_TD_DOMN;
 create alias MDAS_AU_PAR_DOMN    as SELECT * from MDAS_AU_DOMN;
 create alias MDAS_CD_RS_USER     as SELECT * from MDAS_CD_USER;
 create alias MDAS_TD_RS_DOMN     as SELECT * from MDAS_TD_DOMN;
 create alias MDAS_AU_RS_DOMN     as SELECT * from MDAS_AU_DOMN;
 create alias MDAS_CD_RSAC_USER   as SELECT * from MDAS_CD_USER;
 create alias MDAS_TD_RSAC_DOMN   as SELECT * from MDAS_TD_DOMN;
 create alias MDAS_AU_RSAC_DOMN   as SELECT * from MDAS_AU_DOMN;
 create alias MDAS_TD_PAR_LOCN    as SELECT * from  MDAS_TD_LOCN;
 create alias MDAS_TD_DTYP_PARNT  as SELECT * from MDAS_TD_DATA_TYP;
 create alias MDAS_TD_PARDOMN     as SELECT * from MDAS_TD_DOMN;
 create alias MDAS_TD_UTYP_PARNT  as SELECT * from MDAS_TD_USER_TYP;
 create alias MDAS_TD_RTYP_PARNT  as SELECT * from MDAS_TD_RSRC_TYP;
 create alias MDAS_CD_RAGRP_USER  as SELECT * from MDAS_CD_USER;
 create alias MDAS_TD_RAGRP_DOMN  as SELECT * from MDAS_TD_DOMN;
 create alias MDAS_AU_RAGRP_DOMN  as SELECT * from MDAS_AU_DOMN;
 create alias MDAS_CD_DAGRP_USER  as SELECT * from MDAS_CD_USER;
 create alias MDAS_TD_DAGRP_DOMN  as SELECT * from MDAS_TD_DOMN;
 create alias MDAS_AU_DAGRP_DOMN  as SELECT * from MDAS_AU_DOMN;
 create alias MDAS_CD_CAGRP_USER  as SELECT * from MDAS_CD_USER;
 create alias MDAS_TD_CAGRP_DOMN  as SELECT * from MDAS_TD_DOMN;
 create alias MDAS_AU_CAGRP_DOMN  as SELECT * from MDAS_AU_DOMN;
 create alias MDAS_TD_CAGRP_ACCS  as SELECT * from MDAS_TD_GRP_ACCS;
 create alias MDAS_TD_CADATA_GRP  as SELECT * from MDAS_TD_DATA_GRP;
 create alias MDAS_AD_CAGRP_ACCS  as SELECT * from MDAS_AD_GRP_ACCS;
 create alias MDAS_TD_CCDATA_TYP  as SELECT * from MDAS_TD_DATA_TYP;
 create alias MDAS_AD_2ACCS       for MDAS_AD_ACCS;
 
 insert into MDAS_AT_DATA_TYP_EXT values ('0000',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0000.0001',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0000.0002',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0002',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0001','text/text','.txt');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0001.0001','text/text','.txt');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0001.0001.0001',' ','.z,.zip,gz');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0001.0001.0002',' ','.z,.zip,gz');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0001.0002','text/text','.txt');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0001.0002.0001',' ','.z,.zip,gz');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0001.0002.0002',' ','.z,.zip,gz');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0002',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0002.0001','image/tiff','.tif,.tiff');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0002.0001.0001','text/text','.uu');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0002.0002','image/gif','.gif');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0002.0003','image/jpeg','.jpeg,.jpg');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0002.0004','image/pbm','.pbm');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0002.0005','image/fig','.fig');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0002.0006','application/x-fits','.fits,.fit');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0002.0007','application/dicom','.IMA,.ima');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0003',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0003.0001','text/text','.tex');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0003.0002','text/text','.trf,.trof');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0003.0003','application/postscript','.ps');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0003.0004','application/dvi','.dvi');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0003.0005','application/msword','.doc,.rtf');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0004',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0004.0001','text/text','.sql');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0004.0002','text/text','.c');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0004.0002.0001','text/text','.h');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0004.0003','text/text','.f');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0004.0004',' ','.o');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0004.0005',' ','.a');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0004.0006',' ','.dat');
commit; 
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0005','text/html','.htm,.html');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0006','text/sgml','.sgm,.sgml');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0007',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0008','text/text','.tar');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0008.0001',' ','.tz,.tgz');       
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0004.0007','text/text','.jav,.java');          
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0004.0008','text/text','.pl');               
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0004.0009','text/text','.tcl');                
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0004.000a',' ','.o'); 
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0009',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0009.0001',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.0009.0002',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000a',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000b',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000b.0001',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000b.0001.0001','audio/x-pn-realaudio','.ra');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000b.0002',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000b.0002.0001','audio/x-pn-realaudio','.rv');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000b.0002.0002','video/mpeg','.mpeg,.mpg');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000b.0002.0003','video/msvideo','.avi');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000c','text/xml','.xml');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000d',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000d.0001','application/vnd.ms-powerpoint','.ppt');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000e',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000e.0001','application/x-msexcel','.xls');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000f',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000f.0001','application/msword','.doc,.rtf');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000f.0002','application/pdf','.pdf');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000g',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000g.0001',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000g.0002',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000g.0003',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000g.0004',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000g.0004.0001',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000g.0005',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000g.0006',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000h',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000h.0001',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000h.0002',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000h.0003',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000h.0004',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000h.0005',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000h.0006',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000i',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000i.0001','video/mpeg','.mpeg,.mpg');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000i.0001.0001','video/mpeg','.mpeg,.mpg');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000i.0002','video/quicktime','.mov');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000j',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000j.0001',' ','.cif,.mmcif');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000j.0002',' ','.pdb');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000k','text/html','.htm,.html');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000l','text/text',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000m','text/text',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000n',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000o',' ',' ');
 insert into MDAS_AT_DATA_TYP_EXT values ('0001.0001.0001.0001.000p',' ',' ');
commit;
 
insert into MDAS_TD_AUTH_SCHM values (6, 'ENCRYPT1');
 
insert into MDAS_CD_USER values (8,'system','0001.0001.0001.0001.0005','demozone','1997-08-15-00.00.00','1997-08-15-00.00.00');

insert into MDAS_AU_DOMN values (8,'0001.0001.0001.0001.0002');
insert into MDAS_AU_GROUP values (8,8);
insert into MDAS_AU_GROUP values (2,8);
insert into MDAS_AU_INFO values (8,'SDSC, CA', '619 534-8378','srb@sdsc.edu');
insert into MDAS_AU_AUTH_KEY values (8, 'CANDO');
 
insert into MDAS_AU_AUTH_MAP values (0,'NULL' , 6);
insert into MDAS_AU_AUTH_MAP values (1,'NULL' , 6);
insert into MDAS_AU_AUTH_MAP values (4,'NULL' , 6);
insert into MDAS_AU_AUTH_MAP values (5,'NULL' , 6);
insert into MDAS_AU_AUTH_MAP values (6,'NULL' , 6);
insert into MDAS_AU_AUTH_MAP values (7,'NULL' , 6);
insert into MDAS_AU_AUTH_MAP values (8,'NULL' , 6);

insert into MDAS_AU_AUTH_MAP 
select user_id , 'NULL' , 6 from mdas_cd_user
where user_id not in ( select user_id from mdas_au_auth_map) 
;

insert into MDAS_AU_GROUP
select user_id, 8 from mdas_cd_user
where user_typ_id = '0001';

commit;

insert into mdas_audit_desc (actionDesc,actionid) values ('D_COPY_META_DATA_FROM_CONTAINER_TO_NEW_CONTAINER',145);
insert into mdas_audit_desc (actionDesc,actionid) values ('D_UPDATE_CONTAINER_FOR_COLLECTION', 146);
insert into mdas_audit_desc (actionDesc,actionid) values ('D_BULK_INSERT_UDEF_META_DATA_FOR_DATA',  147);
insert into mdas_audit_desc (actionDesc,actionid) values ('C_BULK_INSERT_UDEF_META_DATA_FOR_COLL',148);
insert into mdas_audit_desc (actionDesc,actionid) values ('U_BULK_INSERT_UDEF_META_DATA_FOR_USER',  149);
insert into mdas_audit_desc (actionDesc,actionid) values ('R_BULK_INSERT_UDEF_META_DATA_FOR_RSRC',   150);
insert into mdas_audit_desc (actionDesc,actionid) values ('Z_INSERT_NEW_LOCAL_ZONE',151);
insert into mdas_audit_desc (actionDesc,actionid) values ('Z_INSERT_NEW_ALIEN_ZONE', 152);
insert into mdas_audit_desc (actionDesc,actionid) values ('Z_MODIFY_ZONE_INFO', 153);
insert into mdas_audit_desc (actionDesc,actionid) values ('Z_MODIFY_ZONE_FOR_USER',  154);
insert into mdas_audit_desc (actionDesc,actionid) values ('Z_CHANGE_ZONE_NAME',  155);
insert into mdas_audit_desc (actionDesc,actionid) values ('Z_MODIFY_ZONE_LOCAL_FLAG',  156);
insert into mdas_audit_desc (actionDesc,actionid) values ('Z_MODIFY_ZONE_STATUS',  157);
insert into mdas_audit_desc (actionDesc,actionid) values ('C_DELETE_MULTI_ATTR', 158);
insert into mdas_audit_desc (actionDesc,actionid) values ('C_INSERT_MULTI_ATTR',  159);
insert into mdas_audit_desc (actionDesc,actionid) values ('C_MODIFY_MULTI_ATTR',  160);
insert into mdas_audit_desc (actionDesc,actionid) values ('C_DELETE_ATTR',       161);
insert into mdas_audit_desc (actionDesc,actionid) values ('C_INSERT_ATTR',        162);
insert into mdas_audit_desc (actionDesc,actionid) values ('C_MODIFY_ATTR',       163);

insert into mdas_audit_desc (actionDesc,actionid) values
('D_BULK_INSERT_UDEF_META_DATA_FOR_MANY_DATA',164);
insert into mdas_audit_desc (actionDesc,actionid) values
('R_CHANGE_OWNER',165);
insert into mdas_audit_desc (actionDesc,actionid) values
('D_SU_INSERT_ACCS',1001);
insert into mdas_audit_desc (actionDesc,actionid) values
('D_SU_DELETE_ACCS',1002);
insert into mdas_audit_desc (actionDesc,actionid) values
('C_SU_INSERT_COLL_ACCS',1003);
insert into mdas_audit_desc (actionDesc,actionid) values
('C_SU_DELETE_COLL_ACCS',1004);
insert into mdas_audit_desc (actionDesc,actionid) values
('C_SU_CHANGE_COLL_NAME',1005);

commit;

quit;
 
