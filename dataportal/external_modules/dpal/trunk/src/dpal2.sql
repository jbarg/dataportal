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
    function getInvestigationsOr (keyword_array in vc_array, fed_id in varchar2, limit_num IN INTEGER DEFAULT 500) RETURN types.ref_cursor;
    function getInvestigationsAnd (keyword_array in vc_array, fed_id in varchar2, limit_num IN INTEGER DEFAULT 500) RETURN types.ref_cursor;
    function getInvestigationsAnd1 (keyword_array in vc_array, fed_id in varchar2) RETURN types.ref_cursor;
    function getInvestigationsById (inv_id_array in num_array, fed_id in varchar2) RETURN types.ref_cursor;
    function getDataSets (inv_id_array in num_array, fed_id in varchar2) RETURN types.ref_cursor;
    function getDataFiles (ds_id_array in num_array, fed_id in varchar2) RETURN types.ref_cursor;
    function getInvestigationsOrFuz (keyword_array in vc_array, fed_id in varchar2, limit_num IN INTEGER DEFAULT 500) RETURN types.ref_cursor;
    function getInvestigationsAndFuz (keyword_array in vc_array, fed_id in varchar2, limit_num IN INTEGER DEFAULT 500) RETURN types.ref_cursor;
    function getInvestigations (fed_id in varchar2, limit_num IN INTEGER DEFAULT 500) RETURN types.ref_cursor;
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
    end getKeywords;

--

    function getInvestigationsOr (keyword_array in vc_array,
                                  fed_id in VARCHAR2,
                                  limit_num IN INTEGER DEFAULT 500)
       RETURN types.ref_cursor
    is
       c_inv types.ref_cursor;
    begin
       OPEN c_inv FOR
            with
             fed_inv AS(
                select g.investigation_id,
                       f.first_name||' '||f.last_name as user_name,
                       f.facility_user_id as user_number
                from investigator g, facility_user f
                where f.facility_user_id = g.facility_user_id
                and f.federal_id = fed_id
                union
                select id, null, null
                from investigation
                where id not in (select investigation_id from investigator)
              ),
             keyword_lower AS(
                select distinct investigation_id, lower(name) name from keyword
              )
            select * from (select i.title as title,i.id as id,i.inv_type as investigation_type,
                                  i.inv_abstract as inv_abstract, k.name as keyword,
                                  g.user_name, g.user_number,
                                  Dense_Rank() over (ORDER BY i.id) as dr
                              from investigation i, fed_inv g, keyword_lower k
                              where i.id = k.investigation_id
                              and k.name in (
                                select Lower(column_value) from TABLE(cast(keyword_array as VC_ARRAY)))
                              and i.id = g.investigation_id
             )where dr <= limit_num;

       RETURN c_inv;
    end getInvestigationsOr;

--

   function getInvestigationsAnd (keyword_array in vc_array,
                                  fed_id in VARCHAR2,
                                  limit_num IN INTEGER DEFAULT 500)
      RETURN types.ref_cursor
   is
       c_inv types.ref_cursor;
       num_keywords integer := keyword_array.count;
   begin
       OPEN c_inv FOR
          select * from (
             with
              fed_inv AS(
                select g.investigation_id
                from investigator g, facility_user f
                where f.facility_user_id = g.facility_user_id
                and f.federal_id = fed_id
                union
                select id from investigation
                where id not in (select investigation_id from investigator)
               ),
              keyword_lower AS(
                select distinct investigation_id, lower(name) name from keyword
              )
             select i.title as title, i.id as id,
                    i.inv_type as investigation_type,
                    i.inv_abstract as inv_abstract
             from investigation i, fed_inv g, keyword_lower k
             where k.name in (
               select /*+ CARDINALITY (t, 3) */
                  Lower(column_value) from TABLE(cast(keyword_array as VC_ARRAY)) t)
             and i.id = k.investigation_id
             and i.id = g.investigation_id
             group by i.title, i.id, i.inv_type, i.inv_abstract
             having count(*) = num_keywords)
          where rownum <= limit_num ;

       RETURN c_inv;
   end getInvestigationsAnd;

--

   -- for backwards compatibility
   function getInvestigationsAnd1 (keyword_array in vc_array, fed_id in VARCHAR2)
      RETURN types.ref_cursor
   is
   begin
      return getInvestigationsAnd(keyword_array, fed_id, 500);
   end getInvestigationsAnd1;

--

   function getInvestigationsById (inv_id_array in num_array, fed_id in varchar2)
       RETURN types.ref_cursor
    is
       c_inv types.ref_cursor;
    begin
       OPEN c_inv FOR
          select i.title as title,i.id as id,i.inv_type as investigation_type,
                 i.inv_abstract as inv_abstract
             from investigation i
             where
                 i.id in (select * from TABLE(cast(inv_id_array as NUM_ARRAY)))
             and (
               id in(
                select g.investigation_id
                  from investigator g, facility_user f
                  where f.facility_user_id = g.facility_user_id
                  and f.federal_id = fed_id)
               or id not in (
                select investigation_id from investigator)
               );
       RETURN c_inv;
    end getInvestigationsById;

--

    function  getDataSets (inv_id_array in num_array, fed_id in varchar2)
       RETURN types.ref_cursor
    is
       c_ds types.ref_cursor;
    begin
       OPEN c_ds FOR
             select d.name as name,d.id as id, d.dataset_status as dataset_status,d.description as description,
                    d.dataset_type as dataset_type, d.investigation_id as inv_id
             from dataset d
             where
                 d.investigation_id in (select * from TABLE(cast(inv_id_array as NUM_ARRAY)))
             and (
               investigation_id in(
                select g.investigation_id
                  from investigator g, facility_user f
                  where f.facility_user_id = g.facility_user_id
                  and f.federal_id = fed_id)
               or investigation_id not in (
                select investigation_id from investigator)
               );

       RETURN c_ds;
    end getDataSets;

--

    function getDataFiles (ds_id_array in num_array, fed_id in varchar2)
       RETURN types.ref_cursor
    is
       c_df types.ref_cursor;
    begin
       OPEN c_df FOR
        select d.name as name, d.id as id, d.location as uri, d.dataset_id as dataset_id
        from datafile d
        where d.dataset_id in (select * from TABLE(cast(ds_id_array as NUM_ARRAY)))
        and dataset_id in(
          select id
          from dataset
          where
               investigation_id in(
                select g.investigation_id
                  from investigator g, facility_user f
                  where f.facility_user_id = g.facility_user_id
                  and f.federal_id = fed_id)
               or investigation_id not in (
                select investigation_id from investigator)
          );
       RETURN c_df;
    end getDataFiles;

--

    function getInvestigationsOrFuz (keyword_array in vc_array,
                                     fed_id in VARCHAR2,
                                     limit_num IN INTEGER DEFAULT 500)
       RETURN types.ref_cursor
    is
       c_inv types.ref_cursor;
    begin
       OPEN c_inv FOR
            WITH
             fed_inv AS(
                select g.investigation_id,
                       f.first_name||' '||f.last_name as user_name,
                      f.facility_user_id as user_number
                from investigator g, facility_user f
                where f.facility_user_id = g.facility_user_id
                and f.federal_id = fed_id
                union
                select id, null, null
                from investigation
                where id not in (select investigation_id from investigator)
              ),
             keyword_lower AS(
                select distinct investigation_id, lower(name) name from keyword
              ),
             wordlist AS(
                select Lower(column_value) word from TABLE(cast(keyword_array as VC_ARRAY))
              )
            select DISTINCT * FROM
              (select i.title as title,i.id as id,i.inv_type as investigation_type,
                      i.inv_abstract as inv_abstract, k.name as keyword,
                      g.user_name, g.user_number,
                      Dense_Rank() over (ORDER BY i.id) as dr
               from investigation i, fed_inv g, keyword_lower k, wordlist w
               where i.id = k.investigation_id
               and k.name LIKE '%'||w.word||'%'
               and i.id = g.investigation_id
             )
           where dr <= limit_num;

       RETURN c_inv;
    end getInvestigationsOrFuz;

--

   function getInvestigationsAndFuz (keyword_array in vc_array,
                                     fed_id in VARCHAR2,
                                     limit_num IN INTEGER DEFAULT 500)
      RETURN types.ref_cursor
   is
       c_inv types.ref_cursor;
       num_keywords integer := keyword_array.count;
   begin
       OPEN c_inv FOR
          select * from (
             WITH
              fed_inv AS(
                select g.investigation_id
                from investigator g, facility_user f
                where f.facility_user_id = g.facility_user_id
                and f.federal_id = fed_id
                union
                select id from investigation
                where id not in (select investigation_id from investigator)
               ),
              keyword_lower AS(
                select distinct investigation_id, lower(name) name
                from keyword
              ),
             wordlist AS(
                select /*+ CARDINALITY (t, 3) */ Lower(column_value) word
                from TABLE(cast(keyword_array as VC_ARRAY))
              )
              select title, id, investigation_type, inv_abstract
              from(
                select i.title as title,
                        i.id as id,
                        i.inv_type as investigation_type,
                        i.inv_abstract as inv_abstract,
                        Row_Number() over(PARTITION BY i.id, w.word ORDER BY 1) rn
                from investigation i, fed_inv g, keyword_lower k, wordlist w
                where i.id = k.investigation_id
                and k.name LIKE '%'||w.word||'%'
                and i.id = g.investigation_id
               )
             where rn = 1
             group by title, id, investigation_type, inv_abstract
             having count(*) = num_keywords)
          where rownum <= limit_num ;

       RETURN c_inv;
   end getInvestigationsAndFuz;

--

    function getInvestigations (fed_id in varchar2, limit_num IN INTEGER DEFAULT 500)
      RETURN types.ref_cursor
    is
      c_inv types.ref_cursor;
    begin
      OPEN c_inv FOR
        select i.title as title, i.id as id, i.inv_type as investigation_type,
               i.inv_abstract as inv_abstract,
               f.first_name||' '||f.last_name as user_name,
               f.facility_user_id as user_number
        from investigation i, investigator g, facility_user f
        where f.facility_user_id = g.facility_user_id
        and f.federal_id = fed_id
        and i.id = g.investigation_id
        and rownum <= limit_num;

       RETURN c_inv;
    end getInvestigations;
--

end dpaccess;
/

