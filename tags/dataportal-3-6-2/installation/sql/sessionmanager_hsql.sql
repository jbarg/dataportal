CREATE TABLE session (
    sid             varchar(200),
    user_id         varchar(200),
    certificate     varchar(400),
    last_accessed   varchar(100)
);


CREATE TABLE session_access_rights (
    sid             varchar(100),
    facility_code   varchar(10),
    permissions     varchar(1000)
);
