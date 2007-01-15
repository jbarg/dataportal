
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
    function getKeywordsByInvestigationId (inv_id_array in num_array, fed_id in varchar2) RETURN types.ref_cursor;
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
                                  fed_id in varchar2,
                                  limit_num in integer default 500)
       RETURN types.ref_cursor
    is
       c_inv types.ref_cursor;
    begin
       OPEN c_inv FOR
          with
            fed_inv AS(
              -- all the user's investigations and the public investigations
              select g.investigation_id
              from investigator g, facility_user f
              where f.facility_user_id = g.facility_user_id
              and f.federal_id = fed_id
              union
              select id
              from investigation
              where id not in (select investigation_id from investigator)
            ),
            keyword_lower AS(
              -- the keywords for all investigations
              select distinct investigation_id, lower(name) name from keyword
            ),
            wordlist AS(
              -- the keywords entered as the search criteria
              select /*+ CARDINALITY (t 3) */ Lower(column_value) word
              from TABLE(cast(keyword_array as VC_ARRAY)) t
            ),
            inv_users AS(
              -- all the investigators for each investigation (there may be no
              -- investigators on some investigations - outer join)
              select g.investigation_id,
                     f.federal_id as user_fed_id,
                     f.first_name||' '||f.last_name as facility_user,
                     f.role as user_role
              from investigator g, facility_user f
              where f.facility_user_id = g.facility_user_id
            )
          select distinct
                 id, title, visit_id, investigation_type, inv_abstract,
                 user_fed_id, facility_user, user_role
          from (
            select i.id as id,
                   i.title as title,
                   i.visit_id as visit_id,
                   i.inv_type as investigation_type,
                   i.inv_abstract as inv_abstract,
                   iu.user_fed_id,
                   iu.facility_user,
                   iu.user_role,
                   Dense_Rank() over (ORDER BY i.id) as dr
            from investigation i, fed_inv g, keyword_lower k,
                 inv_users iu, wordlist w
            where i.id = g.investigation_id
            and iu.investigation_id(+) = i.id
            and k.investigation_id = i.id
-->  this line is the only difference between
-->  getInvestigationsOrFuz and getInvestigationsOr
            and k.name = w.word
--<
            )
          --
          -- limit to the first n investigations (may still be many times more
          -- rows than investigations)
          where dr <= limit_num;

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
          with
            fed_inv AS(
              -- all the user's investigations and the public investigations
              select g.investigation_id
              from investigator g, facility_user f
              where f.facility_user_id = g.facility_user_id
              and f.federal_id = fed_id
              union
              select id
              from investigation
              where id not in (select investigation_id from investigator)
            ),
            keyword_lower AS(
              -- the keywords for all investigations
              select distinct investigation_id, lower(name) name from keyword
            ),
            wordlist AS(
              -- the keywords entered as the search criteria
              select /*+ CARDINALITY (t 3) */ Lower(column_value) word
              from TABLE(cast(keyword_array as VC_ARRAY)) t
              /*testing...
              SELECT 'york' word FROM dual
              --*/
            ),
            inv_users AS(
              -- all the investigators for each investigation (there may be no
              -- investigators on some investigations - outer join)
              select g.investigation_id,
                     f.federal_id as user_fed_id,
                     f.first_name||' '||f.last_name as facility_user,
                     f.role as user_role
              from investigator g, facility_user f
              where f.facility_user_id = g.facility_user_id
            )
          select id, title, visit_id, investigation_type, inv_abstract,
                user_fed_id, facility_user, user_role
          from(
            select prelim.*,
                   Dense_Rank() over (ORDER BY id) as dr
            from (
              select distinct
                     i.id as id,
                     i.title as title,
                     i.visit_id as visit_id,
                     i.inv_type as investigation_type,
                     i.inv_abstract as inv_abstract,
                     iu.user_fed_id,
                     iu.facility_user,
                     iu.user_role,
                     count(DISTINCT w.word) over (PARTITION BY i.id, iu.user_fed_id) as cnt
              from investigation i, fed_inv g, keyword_lower k,
                  inv_users iu, wordlist w
              where i.id = g.investigation_id
              and iu.investigation_id(+) = i.id
              and k.investigation_id = i.id
-->  this line is the only difference between
-->  getInvestigationsAndFuz and getInvestigationsAnd
              and k.name = w.word
--<
              ) prelim
            --
            -- where each search criterion has matched a keyword (for fuzzy
            -- searches they may each mach one or more keywords)
            where cnt = num_keywords
            )
          --
          -- limit to the first n investigations (may still be many times more
          -- rows than investigations)
          where dr <= limit_num;

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
          with
            fed_inv AS(
              -- all the user's investigations and the public investigations
              select g.investigation_id
              from investigator g, facility_user f
              where f.facility_user_id = g.facility_user_id
              and f.federal_id = fed_id
              union
              select id
              from investigation
              where id not in (select investigation_id from investigator)
            ),
            inv_users AS(
              -- all the investigators for each investigation (there may be no
              -- investigators on some investigations - outer join)
              select g.investigation_id,
                     f.federal_id as user_fed_id,
                     f.first_name||' '||f.last_name as facility_user,
                     f.role as user_role
              from investigator g, facility_user f
              where f.facility_user_id = g.facility_user_id
            )
          select i.id as id,
                 i.title as title,
                 i.visit_id as visit_id,
                 i.inv_type as investigation_type,
                 i.inv_abstract as inv_abstract,
                 iu.user_fed_id,
                 iu.facility_user,
                 iu.user_role
             from investigation i, fed_inv g, inv_users iu
             where
                 i.id in (select * from TABLE(cast(inv_id_array as NUM_ARRAY)))
             and i.id = g.investigation_id
             and iu.investigation_id (+)= i.id;
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
          with
            fed_inv AS(
              -- all the user's investigations and the public investigations
              select g.investigation_id
              from investigator g, facility_user f
              where f.facility_user_id = g.facility_user_id
              and f.federal_id = fed_id
              union
              select id
              from investigation
              where id not in (select investigation_id from investigator)
            ),
            keyword_lower AS(
              -- the keywords for all investigations
              select distinct investigation_id, lower(name) name from keyword
            ),
            wordlist AS(
              -- the keywords entered as the search criteria
              select /*+ CARDINALITY (t 3) */ Lower(column_value) word
              from TABLE(cast(keyword_array as VC_ARRAY)) t
            ),
            inv_users AS(
              -- all the investigators for each investigation (there may be no
              -- investigators on some investigations - outer join)
              select g.investigation_id,
                     f.federal_id as user_fed_id,
                     f.first_name||' '||f.last_name as facility_user,
                     f.role as user_role
              from investigator g, facility_user f
              where f.facility_user_id = g.facility_user_id
            )
          select distinct
                 id, title, visit_id, investigation_type, inv_abstract,
                 user_fed_id, facility_user, user_role
          from (
            select i.id as id,
                   i.title as title,
                   i.visit_id as visit_id,
                   i.inv_type as investigation_type,
                   i.inv_abstract as inv_abstract,
                   iu.user_fed_id,
                   iu.facility_user,
                   iu.user_role,
                   Dense_Rank() over (ORDER BY i.id) as dr
            from investigation i, fed_inv g, keyword_lower k,
                 inv_users iu, wordlist w
            where i.id = g.investigation_id
            and iu.investigation_id(+) = i.id
            and k.investigation_id = i.id
-->  this line is the only difference between
-->  getInvestigationsOrFuz and getInvestigationsOr
            and k.name like '%'||w.word||'%'
--<
            )
          --
          -- limit to the first n investigations (may still be many times more
          -- rows than investigations)
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
          with
            fed_inv AS(
              -- all the user's investigations and the public investigations
              select g.investigation_id
              from investigator g, facility_user f
              where f.facility_user_id = g.facility_user_id
              and f.federal_id = fed_id
              union
              select id
              from investigation
              where id not in (select investigation_id from investigator)
            ),
            keyword_lower AS(
              -- the keywords for all investigations
              select distinct investigation_id, lower(name) name from keyword
            ),
            wordlist AS(
              -- the keywords entered as the search criteria
              select /*+ CARDINALITY (t 3) */ Lower(column_value) word
              from TABLE(cast(keyword_array as VC_ARRAY)) t
              /*testing...
              SELECT 'york' word FROM dual
              --*/
            ),
            inv_users AS(
              -- all the investigators for each investigation (there may be no
              -- investigators on some investigations - outer join)
              select g.investigation_id,
                     f.federal_id as user_fed_id,
                     f.first_name||' '||f.last_name as facility_user,
                     f.role as user_role
              from investigator g, facility_user f
              where f.facility_user_id = g.facility_user_id
            )
          select id, title, visit_id, investigation_type, inv_abstract,
                user_fed_id, facility_user, user_role
          from(
            select prelim.*,
                   Dense_Rank() over (ORDER BY id) as dr
            from (
              select distinct
                     i.id as id,
                     i.title as title,
                     i.visit_id as visit_id,
                     i.inv_type as investigation_type,
                     i.inv_abstract as inv_abstract,
                     iu.user_fed_id,
                     iu.facility_user,
                     iu.user_role,
                     count(DISTINCT w.word) over (PARTITION BY i.id, iu.user_fed_id) as cnt
              from investigation i, fed_inv g, keyword_lower k,
                  inv_users iu, wordlist w
              where i.id = g.investigation_id
              and iu.investigation_id(+) = i.id
              and k.investigation_id = i.id
-->  this line is the only difference between
-->  getInvestigationsAndFuz and getInvestigationsAnd
              and k.name like '%'||w.word||'%'
--<
              ) prelim
            --
            -- where each search criterion has matched a keyword (for fuzzy
            -- searches they may each mach one or more keywords)
            where cnt = num_keywords
            )
          --
          -- limit to the first n investigations (may still be many times more
          -- rows than investigations)
          where dr <= limit_num;

       RETURN c_inv;
   end getInvestigationsAndFuz;

--

    function getInvestigations (fed_id in varchar2, limit_num IN INTEGER DEFAULT 500)
      RETURN types.ref_cursor
    is
      c_inv types.ref_cursor;
    begin
      OPEN c_inv FOR
        with
          fed_inv AS(
            -- all the user's investigations
            select g.investigation_id
            from investigator g, facility_user f
            where f.facility_user_id = g.facility_user_id
            and f.federal_id = fed_id
          )
        select id, title, visit_id, investigation_type, inv_abstract,
              user_fed_id, facility_user, user_role
        from(
          select i.id as id,
                 i.title as title,
                 i.visit_id as visit_id,
                 i.inv_type as investigation_type,
                 i.inv_abstract as inv_abstract,
                 f.federal_id as user_fed_id,
                 f.first_name||' '||f.last_name as facility_user,
                 f.role as user_role,
                 dense_rank() over(ORDER BY i.id) dr
          from investigation i, fed_inv g, investigator ir, facility_user f
          where i.id = g.investigation_id
          and ir.investigation_id = i.id
          and f.facility_user_id = ir.facility_user_id
          )
        --
        -- limit to the first n investigations (may still be many times more
        -- rows than investigations)
        where dr <= limit_num;
      RETURN c_inv;
    end getInvestigations;

--

 function getKeywordsByInvestigationId (inv_id_array in num_array, fed_id in varchar2)
      RETURN types.ref_cursor
    is
      c_inv types.ref_cursor;
    begin
      OPEN c_inv FOR
        select distinct lower(name)as name, investigation_id, '-99' as  id
        from keyword
        where investigation_id in (select * from TABLE(cast(inv_id_array as NUM_ARRAY)))
        order by 1;
        -- need the fed_id check
       RETURN c_inv;
    end  getKeywordsByInvestigationId;


--

end dpaccess;
/

