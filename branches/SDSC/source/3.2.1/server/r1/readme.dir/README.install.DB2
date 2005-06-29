This file contains just a simple notes how Jerry Rowley <jrowley@sdsc.edu> installed DB2 on a Linux machine.

Hi all,

Here are the details of the install, for those who are interested.

db2isetup was run to create the instance db2inst0.
The following users and groups were created in this process:
db2inst0:db2dba
db2as0:db2dba
db2fenc0:db2dba

db2inst0 is the instance name, with a home directory of: /home/db2inst0

2 databases were created with the following command:
create database SRB on /big/DB2/data/srb
create database MCAT on /big/DB2/data/mcat

1 tablespaces for each database was created as follows:
create tablespace SRB_TBS managed by system using ('/big/DB2/data/sms/SRB_TBS')
create tablespace MCAT_TBS managed by system using ('/big/DB2/data/sms/MCAT_TBS')


When tables are created, they need to specify the appropriate tablespace,
to they will end up in userspace. (i.e. create table X ( column def ) in tablespace SRB_TBS)

Also, the fixpaks should be installed for this system also.  I have installed them here
and it was relatively very painless.

Let me know if you need any more info or assistance.

Jerry

Date: Wed, 25 Jun 2003 10:45:09 -0700
From: Jerry Rowley <jrowley@sdsc.edu>
To: Sheau-Yen Chen <sheauc@sdsc.edu>

