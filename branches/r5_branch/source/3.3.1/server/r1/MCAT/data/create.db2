CREATE DATABASE mcat 
	ON /db2/db2_2 
	COLLATE USING IDENTITY
	WITH "meta catalog for the SRB";





CREATE DATABASE mcat 
	ON /db2/db2_2 
	COLLATE USING IDENTITY
	USER TABLESPACE mcatusr1
		MANAGED BY DATABASE 
		USING ( FILE  '/db2/db2_1/user1' 50000,
			FILE  '/db2/db2_2/user2' 50000,
			FILE  '/db2/db2_3/user3' 50000,
			FILE  '/db2/db2_4/user4' 50000
		      )
		EXTENTSIZE 64 PREFETCHSIZE 64
	TEMPORARY TABLESPACE mcattmp1
		MANAGED BY DATABASE 
		USING ( FILE  '/db2/db2_3/temp1' 50000,
			FILE  '/db2/db2_3/temp2' 50000
		      )
		EXTENTSIZE 64 PREFETCHSIZE 64
	WITH "meta catalog for the SRB";

CREATE TABLESPACE mcatusr2
		MANAGED BY DATABASE 
		USING ( FILE  '/db2/db2_1/user1' 50000,
			FILE  '/db2/db2_2/user2' 50000,
			FILE  '/db2/db2_1/user3' 50000,
			FILE  '/db2/db2_2/user4' 50000
		      )
		EXTENTSIZE 64 PREFETCHSIZE 64;

CREATE TABLESPACE mcatinx1
		MANAGED BY DATABASE 
		USING ( FILE  '/db2/db2_3/index1' 50000,
			FILE  '/db2/db2_4/index2' 50000,
			FILE  '/db2/db2_3/index3' 50000,
			FILE  '/db2/db2_4/index4' 50000
		      )
		EXTENTSIZE 64 PREFETCHSIZE 64;

