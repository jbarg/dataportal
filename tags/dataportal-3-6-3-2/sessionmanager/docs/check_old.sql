select * from session where 
last_accessed < (CURRENT_TIMESTAMP - interval '15 minutes');
