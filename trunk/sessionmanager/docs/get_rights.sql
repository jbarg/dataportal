select * from session, session_access_rights where 
session.sid = 7 and
last_accessed > (CURRENT_TIMESTAMP - interval '15 minutes')
and session.sid = session_access_rights.sid;
