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
    function getInvestigationsOr (keyword_array in vc_array, dn in varchar2) RETURN types.ref_cursor;
    function getInvestigationsById (inv_id_array in num_array, dn in varchar2) RETURN types.ref_cursor;
    function getDataSets (inv_id_array in num_array, dn in varchar2) RETURN types.ref_cursor;
    function getDataFiles (ds_id_array in num_array, dn in varchar2) RETURN types.ref_cursor;
end dpaccess;
/

--package body:
create or replace package body dpaccess as

    function getKeywords (dn in varchar2)
       RETURN types.ref_cursor
    is
       c_key types.ref_cursor;
    begin
       OPEN c_key FOR
          select  distinct(lower(name)) as name, '-99' as  id
             from keyword order by name asc ;
       RETURN c_key;
    end;

    function getInvestigationsOr (keyword_array in vc_array, dn in varchar2) 
       RETURN types.ref_cursor
    is
       c_inv types.ref_cursor;
    begin
       OPEN c_inv FOR 
          select * from (select i.title as title,i.id as id,i.inv_type as investigation_type, i.inv_abstract as inv_abstract, k.name as keyword
                            from investigation i, keyword k
                            where
			         lower(k.name) in (select * from TABLE(cast(keyword_array as VC_ARRAY)))
                            and 
                    	         i.id = k.investigation_id ) where rownum < 501 ;
       RETURN c_inv;
    end;

   function getInvestigationsById (inv_id_array in num_array, dn in varchar2)
       RETURN types.ref_cursor
    is
       c_inv types.ref_cursor;
    begin
       OPEN c_inv FOR
          select i.title as title,i.id as id,i.inv_type as investigation_type, i.inv_abstract as inv_abstract
             from investigation i
             where
                 i.id in (select * from TABLE(cast(inv_id_array as NUM_ARRAY))) ;
       RETURN c_inv;
    end;

    function  getDataSets (inv_id_array in num_array, dn in varchar2)
       RETURN types.ref_cursor
    is
       c_ds types.ref_cursor;
    begin      
       OPEN c_ds FOR
             select d.name as name,d.id as id, d.dataset_status as dataset_status,d.description as description,
                    d.dataset_type as dataset_type, d.investigation_id as inv_id
                    from dataset d
                    where
                        d.investigation_id in (select * from TABLE(cast(inv_id_array as NUM_ARRAY))) ;
       RETURN c_ds;
    end;

    function getDataFiles (ds_id_array in num_array, dn in varchar2) 
       RETURN types.ref_cursor
    is
       c_df types.ref_cursor;
    begin
       OPEN c_df FOR
             select d.name as name, d.id as id, d.location as uri, d.dataset_id as dataset_id
             from datafile d
             where
                d.dataset_id in (select * from TABLE(cast(ds_id_array as NUM_ARRAY))) ;
       RETURN c_df;
    end;

end dpaccess;
/



