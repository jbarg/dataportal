create or replace type NUM_ARRAY as table of number;
/
create or replace type VC_ARRAY as table of varchar2(4000);
/

-- needed for returning a resultset
CREATE OR REPLACE PACKAGE types
AS
    TYPE ref_cursor IS REF CURSOR;
END;
/


-- package spec:
create or replace package dpaccess as
    function getKeywords (dn in varchar2) RETURN types.ref_cursor;
    function getStudies (keyword_array in vc_array, dn in varchar2) RETURN types.ref_cursor;
    function getStudiesById (study_id_array in vc_array, dn in varchar2) RETURN types.ref_cursor;
    function getInvestigations (study_id_array in num_array, dn in varchar2) RETURN types.ref_cursor;
    function getDataSets (inv_id_array in num_array, dn in varchar2) RETURN types.ref_cursor;
    function getDataFiles (ds_id_array in num_array, dn in varchar2) RETURN types.ref_cursor;
end dpaccess;
/

--package body:
create or replace package body dpaccess as

    function getKeywords (dn in varchar2)
       RETURN types.ref_cursor
    is
       c_study types.ref_cursor;
    begin
       OPEN c_study FOR
          select  distinct(lower(name)), '-99' as  id
             from keyword order by name asc ;
       RETURN c_study;
    end;

    function getStudies (keyword_array in vc_array, dn in varchar2) 
       RETURN types.ref_cursor
    is
       c_study types.ref_cursor;
    begin
       OPEN c_study FOR 
          select  distinct(s.id) as id, s.name as name,s.start_date as start_date,s.end_date as end_date, k.name as keyword 
             from study s, keyword_list kl, keyword k
             where 
                 lower(k.name) in (select * from TABLE(cast(keyword_array as VC_ARRAY)))
               and
                 s.id=kl.study_id
               and
                 kl.keyword_id=k.id  
               and
                 s.name is not null ;
       RETURN c_study;
    end;

    function getStudiesById (study_id_array in vc_array, dn in varchar2)
       RETURN types.ref_cursor
    is
       c_study types.ref_cursor;
    begin
       OPEN c_study FOR
          select  s.id as id, s.name as name,s.start_date as start_date,s.end_date as end_date
             from study s
             where
                 s.id in (select * from TABLE(cast(study_id_array as VC_ARRAY))) ;
       RETURN c_study;
    end;


    function getInvestigations (study_id_array in num_array, dn in varchar2) 
       RETURN types.ref_cursor
    is
       c_inv types.ref_cursor;
    begin
       OPEN c_inv FOR
          select i.title as title,i.id as id,i.investigation_type as investigation_type, i.inv_abstract as inv_abstract, s.id as study_id 
             from investigation i, investigation_list il, study s
             where 
                 s.id in (select * from TABLE(cast(study_id_array as NUM_ARRAY))) 
              and
                 i.id=il.investigation_id
              and
                 il.study_id=s.id ;
       RETURN c_inv;
    end;

    function  getDataSets (inv_id_array in num_array, dn in varchar2)
       RETURN types.ref_cursor
    is
       c_ds types.ref_cursor;
    begin      
       OPEN c_ds FOR
          select d.name as name,d.id as id,d.dataset_status as dataset_status,d.description as description,d.dataset_type as dataset_type, i.id as inv_id 
             from dataset d, dataset_list dl, investigation i
             where 
                 i.id in (select * from TABLE(cast(inv_id_array as NUM_ARRAY)))
              and
                 d.id=dl.dataset_id
              and
                 dl.investigation_id=i.id ;
       RETURN c_ds;
    end;

    function getDataFiles (ds_id_array in num_array, dn in varchar2) 
       RETURN types.ref_cursor
    is
       c_df types.ref_cursor;
    begin
       OPEN c_df FOR
          select d.name as name, d.id as id, d.uri as uri, ds.id as dataset_id
             from datafile d, datafile_list dl, dataset ds
             where 
                ds.id in (select * from TABLE(cast(ds_id_array as NUM_ARRAY)))
              and
                d.id=dl.datafile_id 
              and
                dl.dataset_id=ds.id ;

       RETURN c_df;
    end;

end dpaccess;
/



