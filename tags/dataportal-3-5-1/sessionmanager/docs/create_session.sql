CREATE TABLE session (
    sid             varchar(200),
    user_id         varchar(200),
    certificate     bytea,
    last_accessed   timestamp
);
