#!/bin/bash

#sas27
#to allow emat/emin schema to use dpal these views are applied
#not an external schema which can view the emat/emin schema is used
#to create the views as emat/emin.dataset and icat.dataset names clash otherwise

sqplus metaematview/viewb4rk1ng@minerva.dl.ac.uk <<EOF

create or replace view keyword as
select st.study_id investigation_id, t.topic name, '-99' mod_id, systimestamp mod_time
from metaematnew.study_topic st, metaematnew.topic t  
where t.leaf  = 'true' 
and t.topic_id = st.topic_id ;

create or replace view investigation as
select study_id id, name title, purpose inv_abstract,'experiment' inv_type, '-99' mod_id, row_mod_time mod_time 
from metaematnew.study ; 

create or replace view dataset as
select d.study_id investigation_id, d.dataset_name name, d.dataset_id id, 'complete' dataset_status, d.description description, 'data' dataset_type, '-99' mod_id, d.creation_date mod_time
from metaematnew.dataset d ;

create or replace view datafile as 
select d.data_name as name, d.data_id as id, d.dataset_id as dataset_id, d.uri as location, '-99' as mod_id, d.creation_date as mod_time
from metaematnew.data d;

commit;
exit;
EOF
