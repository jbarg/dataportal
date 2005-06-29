##  Here the trigger and tables created by user 'SRB'

create trigger SRB.delDataGrpTrig
   AFTER DELETE on SRB.mdas_td_data_grp
   REFERENCING OLD AS X
   FOR EACH ROW MODE DB2SQL 
   BEGIN ATOMIC
      insert into SRB.mdas_td_del_data_grp values (
X.data_grp_id,
X.data_grp_name,
X.parent_grp_name,
X.coll_owner,
X.coll_cr_timestamp,
X.coll_comments,
X.coll_lock,
X.coll_lockowner,
X.coll_lockexpiry,
X.coll_link,
X.coll_mod_timestamp);
END

create trigger SRB.delReplTrig 
  AFTER DELETE on SRB.mdas_ad_repl
   REFERENCING OLD AS X
   FOR EACH ROW MODE DB2SQL
BEGIN ATOMIC
   insert into SRB.mdas_ad_del_repl values (
         OLD.data_id,
         OLD.repl_enum,
         OLD.data_name,
         OLD.data_typ_id,
         OLD.path_name,
         OLD.rsrc_id,
         OLD.repl_timestamp,
         OLD.data_grp_id,
         OLD.data_size,
         OLD.is_deleted,
         OLD.data_owner,
         OLD.is_dirty,
         OLD.offset,
         OLD.container_id,
         OLD.modify_timestamp,
         OLD.last_accs_time,
         OLD.datalock,
         OLD.lockowner,
         OLD.version_num,
         OLD.container_td_id,
         OLD.d_comment,
         OLD.dlockexpiry,
         OLD.dchecksum,
         OLD.dhide,
         OLD.auditflag,
         OLD.seg_num);
END;

