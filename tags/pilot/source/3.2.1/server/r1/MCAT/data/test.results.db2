test_srb_mdas_create_user testuser1 CACA npaci project '1 Main Street' '619-534 5000' 'testuser1@npaci.edu' srb CANDO npaci
opened
USER REGISTRATION STARTED...
DONE
test_srb_mdas_create_user testuser1 CACA npaci project '1 Main Street' '619-534 5000' 'testuser1@npaci.edu' srb CANDO npaci
opened
USER REGISTRATION STARTED...
ERROR: -3218
test_srb_mdas_create_user testuser2 CACA npaci project '1 Main Street' '619-534 5000' 'testuser2@npaci.edu' srb CANDO npaci
opened
USER REGISTRATION STARTED...
DONE
test_srb_mdas_modu 22 srb CANDO npaci testuser1 sysadmin
opened
authorized
DONE
test_srb_mdas_modu 22 srb CANDO npaci testuser1 aaaa
opened
authorized
ERROR: -3227
test_srb_mdas_modu 22 srb CANDO npaci testuser1 project
opened
authorized
DONE
test_srb_mdas_auth testuser1 CACA npaci
Authorization=0
test_srb_mdas_auth testuser1 CARA npaci
Authorization=-3206
test_srb_mdas_auth tuser1 CACA npaci
Authorization=-3206
test_srb_mdas_auth testuser1 CACA ssss
Authorization=-3206
test_srb_mdas_auth testuser1 CACA sdsc
Authorization=-3206
test_srb_mdas_modu 16 testuser1 CACA npaci BABA
opened
authorized
DONE
test_srb_mdas_auth testuser1 CACA npaci
Authorization=-3206
test_srb_mdas_auth testuser1 BABA npaci
Authorization=0
test_srb_mdas_modu 16  testu1 BABA npaci CACA
opened
Authorization Error:-3206
test_srb_mdas_modu 16 testuser1 BAB npaci CACA
opened
Authorization Error:-3206
test_srb_mdas_modu 16 testuser1 BABA sdsc CACA
opened
Authorization Error:-3206
test_srb_mdas_modu 16 testuser1 BABA dddd CACA
opened
Authorization Error:-3206
test_srb_mdas_modu 16 testuser1 BABA npaci CACA
opened
authorized
DONE
test_srb_mdas_auth testuser1 CACA npaci
Authorization=0
test_srb_mdas_modu 19 srb CANDO npaci testuser1 sdsc
opened
authorized
DONE
test_srb_mdas_modu 19 srb CANDO npaci testuser1 sdsc
opened
authorized
ERROR: -3218
test_srb_mdas_modu 19 testuser1 CACA npaci testuser1 sdsc
opened
authorized
ERROR: -3207
test_srb_mdas_modu 19 testuser1 CACA npaci testuser1 ucb
opened
authorized
ERROR: -3207
test_srb_mdas_modu 19 srb CANDO npaci testu1 ucb
opened
authorized
ERROR: -3203
test_srb_mdas_modu 19 srb CANDO npaci  testuser1 dddd
opened
authorized
ERROR: -4002
test_srb_mdas_modu 19 srb CANDO npaci testuser1 ucb
opened
authorized
DONE
test_srb_mdas_modu 18 srb CANDO npaci testuser1 sdsc
opened
authorized
DONE
test_srb_mdas_modu 18 srb CANDO npaci testuser1 sdsc
opened
authorized
DONE
test_srb_mdas_modu 18 testuser1 CACA npaci testuser1 sdsc
opened
authorized
ERROR: -3207
test_srb_mdas_modu 18 srb CANDO npaci testu1 sdsc
opened
authorized
ERROR: -3203
test_srb_mdas_modu 18 srb CANDO npaci testuser1 dddd
opened
authorized
ERROR: -4002
test_srb_mdas_modu 18 srb CANDO npaci testuser1 ucb
opened
authorized
DONE
test point 1
test_srb_mdas_create_ugroup ngroup1 CACA project '2 Main Street' '619-534 5000' 'ngroup1@sdsc.edu' srb CANDO npaci
opened
GROUP REGISTRATION STARTED...
DONE
test_srb_mdas_create_ugroup ngroup1 CACA project  '2 Main Street' '619-534 5000' 'ngroup1@sdsc.edu' srb CANDO npaci
opened
GROUP REGISTRATION STARTED...
ERROR: -3218
test_srb_mdas_modu 21 srb CANDO npaci testuser1 ngroup1
opened
authorized
DONE
test_srb_mdas_modu 21 srb CANDO npaci testuser1 ngroup1
opened
authorized
DONE
test_srb_mdas_modu 21 srb CANDO npaci testuser1 ngroup2
opened
authorized
ERROR: -3211
test_srb_mdas_modu 21 srb CANDO npaci tuser1 ngroup1
opened
authorized
ERROR: -3203
test_srb_mdas_modu 21 srb CANDO npaci srb ngroup1
opened
authorized
DONE
test_srb_mdas_modu 21 srb CANDO npaci srb ngroup1
opened
authorized
DONE
test_srb_mdas_modu 21 srb CANDO npaci srb ngroup2
opened
authorized
ERROR: -3211
test_srb_mdas_create_ugroup ngroup2 CACA project '2 Main Street' '619-534 5000' 'ngroup2@sdsc.edu' srb CANDO npaci
opened
GROUP REGISTRATION STARTED...
DONE
test_srb_mdas_modu 20 srb CANDO npaci srb ngroup1
opened
authorized
DONE
test_srb_mdas_create_collections '/g1/g1/g1/srbtest' 'dir1' srb CANDO npaci
opened
authorized
done
 test_srb_mdas_create_collections '/g1/g1/g1/srbtest' 'dir2' srb CANDO npaci
opened
authorized
done
 test_srb_mdas_create_collections '/g1/g1/g1/srbtest' 'dir2' srb CANDO npaci
opened
authorized
Error: -3219
done
 test_srb_mdas_create_collections '/srbtest' 'dir3' srb CANDO npaci
opened
authorized
done
 test_srb_mdas_modc 27 '/g1/g1/g1/srbtest/dir2' srb CANDO npaci testuser1 npaci all 
opened
authorized
DONE
test_srb_mdas_modc 27 '/g1/g1/g1/srbtest/dir2' srb CANDO npaci testuser1 npaci all 
opened
authorized
DONE
test_srb_mdas_modc 27 '/g1/g1/g1/srbtest/dir2' srb CANDO npaci testuser1 npaci read
opened
authorized
DONE
test_srb_mdas_modc 27 '/srbtest/dir2' srb CANDO npaci  testuser1 npaci read
opened
authorized
ERROR: -3221
test_srb_mdas_modc 27 '/g1/g1/g1/srbtest/dir2' srb CANDO npaci tuser1 npaci read
opened
authorized
ERROR: -3203
test_srb_mdas_modc 27 '/g1/g1/g1/srbtest/dir2' srb CANDO npaci testuser1 ddd all
opened
authorized
ERROR: -3203
test_srb_mdas_modc 27 '/g1/g1/g1/srbtest/dir2' srb CANDO npaci testuser1 sdsc all
opened
authorized
ERROR: -3203
test_srb_mdas_modc 27 '/g1/g1/g1/srbtest/dir2' srb CANDO npaci testuser1 npaci rd
opened
authorized
ERROR: -3226
test_srb_mdas_modc 28 '/g1/g1/g1/srbtest/dir2' srb CANDO npaci testuser1 npaci all 
opened
authorized
DONE
test_srb_mdas_modc 28 '/g1/g1/g1/srbtest/dir2' srb CANDO npaci testuser1 npaci read
opened
authorized
DONE
test_srb_mdas_modc 27 '/g1/g1/g1/srbtest/dir2' srb CANDO npaci testuser1 npaci read
opened
authorized
DONE
test_srb_mdas_modc 28 '/srbtest/dir2' srb CANDO npaci testuser1 npaci read 
opened
authorized
ERROR: -3221
test_srb_mdas_modc 28 '/g1/g1/g1/srbtest/dir2' srb CANDO npaci tuser1 npaci read
opened
authorized
ERROR: -3203
test_srb_mdas_modc 28 '/g1/g1/g1/srbtest/dir2' srb CANDO npaci testuser1 ddd read
opened
authorized
ERROR: -3203
test_srb_mdas_modc 28 '/g1/g1/g1/srbtest/dir2' srb CANDO npaci testuser1 sdsc read
opened
authorized
ERROR: -3203
test_srb_mdas_modc 28 '/g1/g1/g1/srbtest/dir2' srb CANDO npaci testuser1 npaci rd
opened
authorized
DONE
test_srb_mdas_modc 27 '/g1/g1/g1/srbtest/dir2' srb CANDO npaci testuser1 npaci write
opened
authorized
DONE
test_srb_mdas_get_collections R '/g1/g1/g1/srbtest'  testuser1 CACA npaci 200
opened
authorized
COLCNT:1
ROWCNT:4
STMTHN:-1


 ---------------------------- RESULT ------------------- 
 /g1/g1/g1/srbtest
   srbdatatest1
 /g1/g1/g1/srbtest/dir1
 /g1/g1/g1/srbtest/dir2


 ------------------------------------------------------- 
test_srb_mdas_get_collections C '/g1/g1/g1/srbtest'  testuser1 CACA npaci 200
opened
authorized
COLCNT:1
ROWCNT:4
STMTHN:-1


 ---------------------------- RESULT ------------------- 
 /g1/g1/g1/srbtest
   srbdatatest1
 /g1/g1/g1/srbtest/dir1
 /g1/g1/g1/srbtest/dir2


 ------------------------------------------------------- 
test_srb_mdas_get_collections C '/g1'  testuser1 CACA npaci 200
opened
authorized
COLCNT:1
ROWCNT:2
STMTHN:-1


 ---------------------------- RESULT ------------------- 
 /g1
 /g1/g1


 ------------------------------------------------------- 
test_srb_mdas_get_collections R '/g1'  testuser1 CACA npaci 200
opened
authorized
COLCNT:1
ROWCNT:8
STMTHN:-1


 ---------------------------- RESULT ------------------- 
 /g1
 /g1/g1
 /g1/g1/g1
 /g1/g1/g1/srbtest
   srbdatatest1
 /g1/g1/g1/srbtest/dir1
 /g1/g1/g1/srbtest/dir2
 /g1/g1/g1/digital-library


 ------------------------------------------------------- 
test_srb_mdas_get_rsrc sdsc-mda18-fs  testuser1 CACA npaci dn cn pn
opened
authorized


 --------------------------- FULLPATH RESULTS ---------------- 
rsrc_name :sdsc-mda18-fs
netprefix :mda-18.sdsc.edu:NULL:NULL
rsrc_typ_name :file system
default_path :/scratch0/srb/SRBVault/?USER.?DOMAIN/TEST.?PATH?DATANAME.?RANDOM
phy_default_path :/scratch0/srb/SRBVault/?USER.?DOMAIN/TEST.?PATH?DATANAME.?RANDOM
phy_rsrc_name :sdsc-mda18-fs
rsrc_typ_name :file system
repl_enum :0
full_path_name : not shown for sake of diff
----------------------------------------------------------
Number of rows: 0
End of Answer
test_srb_mdas_get_rsrc ssc-mda18-fs   testuser1 CACA npaci dn cn pn
opened
authorized
Main: Missing resource  in catalog:-3005
test_srb_mdas_get_rsrc sdsc-mda18-fs  tuser1    CACA npaci dn cn pn
opened
Authorization Error:-3206
test_srb_mdas_get_rsrc sdsc-mda18-fs  testuser1 CACA lgn dn cn pn
opened
Authorization Error:-3206
test_srb_mdas_get_rsrc sdsc-mda18-fs  testuser1 CACA sdsc dn cn pn
opened
Authorization Error:-3206
test_srb_mdas_get_rsrc sdsc-mda18-fs  testuser1 CACA npaci dn cn pn
opened
authorized


 --------------------------- FULLPATH RESULTS ---------------- 
rsrc_name :sdsc-mda18-fs
netprefix :mda-18.sdsc.edu:NULL:NULL
rsrc_typ_name :file system
default_path :/scratch0/srb/SRBVault/?USER.?DOMAIN/TEST.?PATH?DATANAME.?RANDOM
phy_default_path :/scratch0/srb/SRBVault/?USER.?DOMAIN/TEST.?PATH?DATANAME.?RANDOM
phy_rsrc_name :sdsc-mda18-fs
rsrc_typ_name :file system
repl_enum :0
full_path_name : not shown for sake of diff
----------------------------------------------------------
Number of rows: 0
End of Answer
test_srb_mdas_create ntest1  testuser1 all npaci 'C code' '/tmp/ntest1' 'sdsc-mda18-fs' '/g1/g1/g1/srbtest/dir2' 100 CACA
opened
authorized
REGISTRATION STARTED...
DONE
test_srb_mdas_create ntest1  testuser1 all npaci 'C code' '/tmp/ntest1' 'sdsc-mda18-fs' '/g1/g1/g1/srbtest/dir2' 100 CACA
opened
authorized
REGISTRATION STARTED...
ERROR: -3210
test_srb_mdas_create ntest10  testuser1 all npaci 'C code' '/tmp/ntest1' 'sdsc-mda18-fs' '/g1/g1/g1/srbtest/dir2' 100 CACA
opened
authorized
REGISTRATION STARTED...
ERROR: -3213
test_srb_mdas_create ntest1  testuser1 al npaci 'C code' '/tmp/ntest1' 'sdsc-mda18-fs' '/g1/g1/g1/srbtest/dir2' 100 CACA
opened
authorized
REGISTRATION STARTED...
ERROR: -3226
test_srb_mdas_create ntest1  testuser1 all npci 'C code' '/tmp/ntest1' 'sdsc-mda18-fs' '/g1/g1/g1/srbtest/dir2' 100 CACA
opened
Authorization Error:-3206
test_srb_mdas_create ntest1  testuser1 all sdsc 'C code' '/tmp/ntest1' 'sdsc-mda18-fs' '/g1/g1/g1/srbtest/dir2' 100 CACA
opened
Authorization Error:-3206
test_srb_mdas_create ntest1  testuser1 all npaci 'L code' '/tmp/ntest1' 'sdsc-mda18-fs' '/g1/g1/g1/srbtest/dir2' 100 CACA
opened
authorized
REGISTRATION STARTED...
ERROR: -3210
test_srb_mdas_create ntest1  testuser1 all npaci 'C code' '/tmp/ntest1' 'sdsc-mda19-fs' '/g1/g1/g1/srbtest/dir2' 100 CACA
opened
authorized
REGISTRATION STARTED...
ERROR: -3215
test_srb_mdas_create ntest2  testuser1 all npaci 'C code' '/tmp/ntest2' 'sdsc-mda18-fs' '/g1/g1/g1/srbtest/dir1' 100 CACA
opened
authorized
REGISTRATION STARTED...
ERROR: -3220
test_srb_mdas_create ntest2  testuser1 all npaci 'C code' '/tmp/ntest2' 'sdsc-mda18-fs' '/g1/g1/g1/srbtest/dir10' 100 CACA
opened
authorized
REGISTRATION STARTED...
ERROR: -3221
test_srb_mdas_create ntest2  testuser1 all npaci 'C code' '/tmp/ntest2' 'sdsc-mda18-fs' '/g1/g1/g1/srbtest/dir2' 100 CAC
opened
Authorization Error:-3206
test_srb_mdas_create ntest2  testuser1 all npaci 'C code' '/tmp/ntest2' 'sdsc-mda18-fs' '/g1/g1/g1/srbtest/dir2' 100 CACA
opened
authorized
REGISTRATION STARTED...
DONE
test_srb_mdas_get ntest1 testuser1 CACA all npaci '/g1/g1/g1/srbtest/dir2' 200
opened
authorized
COLCNT:6
ROWCNT:1
STMTHN:-1

 ---------------------------- RESULT ------------------- 
DATA  NAME    : ntest1
DATA TYPE NAME: C code
DATA PATH NAME: /tmp/ntest1
RESOURCE NAME : sdsc-mda18-fs
RESOURCE TYPE : file system
RESOURCE LOCN : mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 
test_srb_mdas_get ntest1 testuser1 CACA all npaci '' 200
opened
authorized
COLCNT:6
ROWCNT:1
STMTHN:-1

 ---------------------------- RESULT ------------------- 
DATA  NAME    : ntest1
DATA TYPE NAME: C code
DATA PATH NAME: /tmp/ntest1
RESOURCE NAME : sdsc-mda18-fs
RESOURCE TYPE : file system
RESOURCE LOCN : mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 
test_srb_mdas_get ntest1 testuser1 CACA all npaci '/g1/g1/g1/srbtest' 200
opened
authorized
Main: Missing object data  in database: -3205
test_srb_mdas_get ntest1 testuser1 CACA all npaci '%srbtest' 200
opened
authorized
Main: Missing object data  in database: -3205
test_srb_mdas_get ntest1 testuser1 CACA 'read audit' npaci '' 200
opened
authorized
COLCNT:6
ROWCNT:1
STMTHN:-1

 ---------------------------- RESULT ------------------- 
DATA  NAME    : ntest1
DATA TYPE NAME: C code
DATA PATH NAME: /tmp/ntest1
RESOURCE NAME : sdsc-mda18-fs
RESOURCE TYPE : file system
RESOURCE LOCN : mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 
test_srb_mdas_get ntest1 testuser1 CACA read npaci '' 200
opened
authorized
COLCNT:6
ROWCNT:1
STMTHN:-1

 ---------------------------- RESULT ------------------- 
DATA  NAME    : ntest1
DATA TYPE NAME: C code
DATA PATH NAME: /tmp/ntest1
RESOURCE NAME : sdsc-mda18-fs
RESOURCE TYPE : file system
RESOURCE LOCN : mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 
test_srb_mdas_get ntest1 testuser1 CACA 'write audit' npaci '' 200
opened
authorized
COLCNT:6
ROWCNT:1
STMTHN:-1

 ---------------------------- RESULT ------------------- 
DATA  NAME    : ntest1
DATA TYPE NAME: C code
DATA PATH NAME: /tmp/ntest1
RESOURCE NAME : sdsc-mda18-fs
RESOURCE TYPE : file system
RESOURCE LOCN : mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 
test_srb_mdas_get ntest1 testuser1 CACA write npaci '' 200
opened
authorized
COLCNT:6
ROWCNT:1
STMTHN:-1

 ---------------------------- RESULT ------------------- 
DATA  NAME    : ntest1
DATA TYPE NAME: C code
DATA PATH NAME: /tmp/ntest1
RESOURCE NAME : sdsc-mda18-fs
RESOURCE TYPE : file system
RESOURCE LOCN : mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 
test_srb_mdas_get ntest1 testuser1 CARA all npaci '' 200
opened
Authorization Error:-3206
test_srb_mdas_get ntest1 testuser1 CACA www npaci '' 200
opened
authorized
Main: Missing object data  in database: -3226
test_srb_mdas_get ntest1 testuser1 CACA all naci '' 200
opened
Authorization Error:-3206
test_srb_mdas_get ntest1 testuser1 CACA all sdsc '' 200
opened
Authorization Error:-3206
test_srb_mdas_get ntest2 testuser1 CACA all npaci '' 200
opened
authorized
COLCNT:6
ROWCNT:1
STMTHN:-1

 ---------------------------- RESULT ------------------- 
DATA  NAME    : ntest2
DATA TYPE NAME: C code
DATA PATH NAME: /tmp/ntest2
RESOURCE NAME : sdsc-mda18-fs
RESOURCE TYPE : file system
RESOURCE LOCN : mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 
test_srb_mdas_get ntest3 testuser1 CACA all npaci '' 200
opened
authorized
Main: Missing object data  in database: -3201
test_srb_mdas_get ntest1 srb CANDO all npaci '' 200
opened
authorized
Main: Missing object data  in database: -3204
test_srb_mdas_get ntest1 srb CANDO read npaci '' 200
opened
authorized
Main: Missing object data  in database: -3204
test point 3
test_srb_mdas_move ntest1 '' 'sdsc-mda18-fs' '/tmp/ntest1'  'sdsc-mda18-fs' '/tmp/new/ntest1'  testuser1 CACA npaci
opened
authorized
DONE
test_srb_mdas_move ntest1 '' 'sdsc-mda18-fs' '/tmp/ntest1'  'sdsc-mda18-fs' '/tmp/new/ntest1'  testuser1 CACA npaci
opened
authorized
ERROR: -3217
test_srb_mdas_move ntest2 '' 'sdsc-mda18-fs' '/tmp/ntest2'  'sdsc-mda18-fs' '/tmp/new/ntest1'  testuser1 CACA npaci
opened
authorized
ERROR: -3213
test_srb_mdas_copy  ntest1 ''  'sdsc-mda18-fs' '/tmp/new/ntest1'  'sdsc-mda18-fs' '/tmp/ntest1' testuser1 CACA npaci
opened
authorized
DONE
test_srb_mdas_copy  ntest1 ''  'sdsc-mda18-fs' '/tmp/new/ntest1'  'sdsc-mda18-fs' '/tmp/ntest1' testuser1 CACA npaci
opened
authorized
ERROR: -3213
test_srb_mdas_get ntest1 testuser1 CACA all npaci '/g1/g1/g1/srbtest/dir2' 200
opened
authorized
COLCNT:6
ROWCNT:2
STMTHN:-1

 ---------------------------- RESULT ------------------- 
DATA  NAME    : ntest1
DATA TYPE NAME: C code
DATA PATH NAME: /tmp/new/ntest1
RESOURCE NAME : sdsc-mda18-fs
RESOURCE TYPE : file system
RESOURCE LOCN : mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 
DATA  NAME    : ntest1
DATA TYPE NAME: C code
DATA PATH NAME: /tmp/ntest1
RESOURCE NAME : sdsc-mda18-fs
RESOURCE TYPE : file system
RESOURCE LOCN : mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 
test_srb_mdas_get ntest1 testuser1 CACA all npaci '' 200
opened
authorized
COLCNT:6
ROWCNT:2
STMTHN:-1

 ---------------------------- RESULT ------------------- 
DATA  NAME    : ntest1
DATA TYPE NAME: C code
DATA PATH NAME: /tmp/new/ntest1
RESOURCE NAME : sdsc-mda18-fs
RESOURCE TYPE : file system
RESOURCE LOCN : mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 
DATA  NAME    : ntest1
DATA TYPE NAME: C code
DATA PATH NAME: /tmp/ntest1
RESOURCE NAME : sdsc-mda18-fs
RESOURCE TYPE : file system
RESOURCE LOCN : mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 
test_srb_mdas_get ntest1 testuser1 CACA all npaci '/g1/g1/g1/srbtest' 200
opened
authorized
Main: Missing object data  in database: -3205
test_srb_mdas_all  ntest1 testuser1 CACA all npaci '/g1/g1/g1/srbtest/dir2' 'sdsc-mda18-fs'
opened
authorized


 ---------------------------- RESULT ------------------- 
DATA NAME     : testuser1
DATA TYPE NAME: C code
DATA PATH NAME: /tmp/new/ntest1
RESOURCE NAME : sdsc-mda18-fs
RESOURCE TYPE : file system
RESOURCE LOCN : mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 
DATA NAME     : testuser1
DATA TYPE NAME: C code
DATA PATH NAME: /tmp/ntest1
RESOURCE NAME : sdsc-mda18-fs
RESOURCE TYPE : file system
RESOURCE LOCN : mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 


 ---------------------------- RESULT ------------------- 
RESOURCE TYPE: file system
RESOURCE LOCN: mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 


 ---------------------------- RESULT ------------------- 
DATA NAME     : testuser1
DATA TYPE NAME: C code
DATA PATH NAME: /tmp/new/ntest1
RESOURCE NAME : sdsc-mda18-fs
RESOURCE TYPE : file system
RESOURCE LOCN : mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 
DATA NAME     : testuser1
DATA TYPE NAME: C code
DATA PATH NAME: /tmp/ntest1
RESOURCE NAME : sdsc-mda18-fs
RESOURCE TYPE : file system
RESOURCE LOCN : mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 


 ---------------------------- RESULT ------------------- 
RESOURCE TYPE: file system
RESOURCE LOCN: mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 


 ---------------------------- RESULT ------------------- 
DATA NAME     : testuser1
DATA TYPE NAME: C code
DATA PATH NAME: /tmp/new/ntest1
RESOURCE NAME : sdsc-mda18-fs
RESOURCE TYPE : file system
RESOURCE LOCN : mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 
DATA NAME     : testuser1
DATA TYPE NAME: C code
DATA PATH NAME: /tmp/ntest1
RESOURCE NAME : sdsc-mda18-fs
RESOURCE TYPE : file system
RESOURCE LOCN : mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 


 ---------------------------- RESULT ------------------- 
RESOURCE TYPE: file system
RESOURCE LOCN: mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 


 ---------------------------- RESULT ------------------- 
DATA NAME     : testuser1
DATA TYPE NAME: C code
DATA PATH NAME: /tmp/new/ntest1
RESOURCE NAME : sdsc-mda18-fs
RESOURCE TYPE : file system
RESOURCE LOCN : mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 
DATA NAME     : testuser1
DATA TYPE NAME: C code
DATA PATH NAME: /tmp/ntest1
RESOURCE NAME : sdsc-mda18-fs
RESOURCE TYPE : file system
RESOURCE LOCN : mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 


 ---------------------------- RESULT ------------------- 
RESOURCE TYPE: file system
RESOURCE LOCN: mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 
test_srb_mdas_all  ntest2 testuser1 CACA all npaci '/g1/g1/g1/srbtest/dir2' 'sdsc-mda18-fs'
opened
authorized


 ---------------------------- RESULT ------------------- 
DATA NAME     : testuser1
DATA TYPE NAME: C code
DATA PATH NAME: /tmp/ntest2
RESOURCE NAME : sdsc-mda18-fs
RESOURCE TYPE : file system
RESOURCE LOCN : mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 


 ---------------------------- RESULT ------------------- 
RESOURCE TYPE: file system
RESOURCE LOCN: mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 


 ---------------------------- RESULT ------------------- 
DATA NAME     : testuser1
DATA TYPE NAME: C code
DATA PATH NAME: /tmp/ntest2
RESOURCE NAME : sdsc-mda18-fs
RESOURCE TYPE : file system
RESOURCE LOCN : mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 


 ---------------------------- RESULT ------------------- 
RESOURCE TYPE: file system
RESOURCE LOCN: mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 


 ---------------------------- RESULT ------------------- 
DATA NAME     : testuser1
DATA TYPE NAME: C code
DATA PATH NAME: /tmp/ntest2
RESOURCE NAME : sdsc-mda18-fs
RESOURCE TYPE : file system
RESOURCE LOCN : mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 


 ---------------------------- RESULT ------------------- 
RESOURCE TYPE: file system
RESOURCE LOCN: mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 


 ---------------------------- RESULT ------------------- 
DATA NAME     : testuser1
DATA TYPE NAME: C code
DATA PATH NAME: /tmp/ntest2
RESOURCE NAME : sdsc-mda18-fs
RESOURCE TYPE : file system
RESOURCE LOCN : mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 


 ---------------------------- RESULT ------------------- 
RESOURCE TYPE: file system
RESOURCE LOCN: mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 
test_srb_mdas_mod 4 ntest1 '/tmp/ntest1' 'sdsc-mda18-fs'  '' testuser1 CACA npaci 400
opened
authorized
DONE
test_srb_mdas_mod 4 ntest1 '/tmp/ntest1' 'sdsc-mda18-fs'  '/g1/g1/g1/srbtest/dir2' testuser1 CACA npaci 500
opened
authorized
DONE
test_srb_mdas_mod 4 ntest1 '/tmp/ntest1' 'sdsc-mda18-fs'  '/g1/g1/g1/srbtest/dir2' testuser1 CACA npaci 600
opened
authorized
DONE
test_srb_mdas_mod 4 ntest1 '' '' ''  testuser1 CACA npaci 600
opened
authorized
DONE
test_srb_mdas_mod 5 ntest1 '/tmp/ntest1' 'sdsc-mda18-fs' '' testuser1 CACA npaci 'gif image'
opened
authorized
DONE
test_srb_mdas_mod 5 ntest1 '/tmp/ntest1' 'sdsc-mda18-fs' '' testuser1 CACA npaci alpha
opened
authorized
ERROR: -3224
test_srb_mdas_mod 5 ntest1 '' '' '' testuser1 CACA npaci 'jpeg image'
opened
authorized
DONE
test_srb_mdas_mod 5 ntest1 '/tmp/ntest1' 'sdsc-mda18-fs' '' testuser1 CACA npaci 'C code'
opened
authorized
DONE
test_srb_mdas_mod 5 ntest1 '' '' '' testuser1 CACA npaci 'C code'
opened
authorized
DONE
test_srb_mdas_mod 6 ntest2 '/tmp/ntest2' 'sdsc-mda18-fs' '/g1/g1/g1/srbtest/dir2' testuser1 CACA npaci '/g1/g1/g1/srbtest/dir1'
opened
authorized
DONE
test_srb_mdas_mod 6 ntest2 '/tmp/ntest2' 'sdsc-mda18-fs' '/g1/g1/g1/srbtest/dir1' testuser1 CACA npaci '/g1/g1/g1/srbtest/dir2'
opened
authorized
DONE
test_srb_mdas_mod 6 ntest2 '/tmp/ntest2' 'sdsc-mda18-fs' '/g1/g1/g1/srbtest/dir1' testuser1 CACA npaci '/g1/g1/g1/srbtest/dir2'
opened
authorized
ERROR: -3222
test_srb_mdas_mod 6 ntest2 '/tmp/ntest2' 'sdsc-mda18-fs' '/g1/g1/g1/srbtest/dir2' testuser1 CACA npaci '/g1/g1/g1/srbte'
opened
authorized
ERROR: -3221
test_srb_mdas_mod 6 ntest2 '/tmp/ntest2' 'sdsc-mda18-fs' '' testuser1 CACA npaci '/g1/g1/g1/srbtest/dir1'
opened
authorized
DONE
test_srb_mdas_mod 6 ntest2 '/tmp/ntest2' 'sdsc-mda18-fs' '' testuser1 CACA npaci '/g1/g1/g1/srbtest/dir2'
opened
authorized
DONE
test_srb_mdas_mod 6 ntest1 '' '' ''  testuser1 CACA npaci '/g1/g1/g1/srbtest/dir1'
opened
authorized
DONE
test_srb_mdas_mod 6 ntest1 '/tmp/ntest1' 'sdsc-mda18-fs' '' testuser1 CACA npaci '/g1/g1/g1/srbtest/dir2'
opened
authorized
DONE
test_srb_mdas_mod 8 ntest1 '/tmp/ntest1' 'sdsc-mda18-fs' '' testuser1 CACA npaci sekar@npaci all
opened
authorized
DONE
test_srb_mdas_mod 8 ntest1 '/tmp/ntest1' 'sdsc-mda18-fs' '' testuser1 CACA npaci sekar@npaci all
opened
authorized
DONE
test_srb_mdas_mod 8 ntest1 '/tmp/ntest1' 'sdsc-mda18-fs' '' testuser1 CACA npaci sekar read
opened
authorized
DONE
test_srb_mdas_mod 8 ntest1 '/tmp/ntest1' 'sdsc-mda18-fs' '' testuser1 CACA npaci srb@npaci  'read audit'
opened
authorized
DONE
test_srb_mdas_mod 8 ntest1 '/tmp/ntest1' 'sdsc-mda18-fs' '' testuser1 CACA npaci srb@npaci  'read noaudit'
opened
authorized
ERROR: -3226
test_srb_mdas_mod 8 ntest1 '/tmp/ntest1' 'sdsc-mda18-fs' '' testuser1 CACA npaci@npaci alpha read
opened
authorized
ERROR: -3203
test_srb_mdas_mod 8 ntest1 '/tmp/ntest1' 'sdsc-mda18-fs' '' testuser1 CACA npaci public@npaci read
opened
authorized
DONE
test_srb_mdas_mod 8 ntest1 '' '' '' testuser1 CACA npaci public read
opened
authorized
DONE
test_srb_mdas_get ntest1 public CANDO all npaci '' 200
opened
authorized
Main: Missing object data  in database: -3204
test_srb_mdas_get ntest1 testuser1 CACA all npaci '' 200
opened
authorized
COLCNT:6
ROWCNT:2
STMTHN:-1

 ---------------------------- RESULT ------------------- 
DATA  NAME    : ntest1
DATA TYPE NAME: C code
DATA PATH NAME: /tmp/new/ntest1
RESOURCE NAME : sdsc-mda18-fs
RESOURCE TYPE : file system
RESOURCE LOCN : mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 
DATA  NAME    : ntest1
DATA TYPE NAME: C code
DATA PATH NAME: /tmp/ntest1
RESOURCE NAME : sdsc-mda18-fs
RESOURCE TYPE : file system
RESOURCE LOCN : mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 
test_srb_mdas_get ntest1 testuser2 CACA all npaci '' 200
opened
authorized
Main: Missing object data  in database: -3204
test_srb_mdas_get ntest1 srb  CANDO read npaci '' 200
opened
authorized
COLCNT:6
ROWCNT:2
STMTHN:-1

 ---------------------------- RESULT ------------------- 
DATA  NAME    : ntest1
DATA TYPE NAME: C code
DATA PATH NAME: /tmp/new/ntest1
RESOURCE NAME : sdsc-mda18-fs
RESOURCE TYPE : file system
RESOURCE LOCN : mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 
DATA  NAME    : ntest1
DATA TYPE NAME: C code
DATA PATH NAME: /tmp/ntest1
RESOURCE NAME : sdsc-mda18-fs
RESOURCE TYPE : file system
RESOURCE LOCN : mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 
test_srb_mdas_get ntest1 srb  CANDO all npaci '' 200
opened
authorized
Main: Missing object data  in database: -3204
test_srb_mdas_get ntest1 srb  CANDO write npaci '' 200
opened
authorized
Main: Missing object data  in database: -3204
test_srb_mdas_get ntest1 srb  CANDO 'write audit' npaci '' 200
opened
authorized
Main: Missing object data  in database: -3204
test_srb_mdas_get ntest1 srb  CANDO 'read audit' npaci '' 200
opened
authorized
COLCNT:6
ROWCNT:2
STMTHN:-1

 ---------------------------- RESULT ------------------- 
DATA  NAME    : ntest1
DATA TYPE NAME: C code
DATA PATH NAME: /tmp/new/ntest1
RESOURCE NAME : sdsc-mda18-fs
RESOURCE TYPE : file system
RESOURCE LOCN : mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 
DATA  NAME    : ntest1
DATA TYPE NAME: C code
DATA PATH NAME: /tmp/ntest1
RESOURCE NAME : sdsc-mda18-fs
RESOURCE TYPE : file system
RESOURCE LOCN : mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 
test_srb_mdas_get ntest1 public CANDO  read npaci '' 200
opened
authorized
COLCNT:6
ROWCNT:2
STMTHN:-1

 ---------------------------- RESULT ------------------- 
DATA  NAME    : ntest1
DATA TYPE NAME: C code
DATA PATH NAME: /tmp/new/ntest1
RESOURCE NAME : sdsc-mda18-fs
RESOURCE TYPE : file system
RESOURCE LOCN : mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 
DATA  NAME    : ntest1
DATA TYPE NAME: C code
DATA PATH NAME: /tmp/ntest1
RESOURCE NAME : sdsc-mda18-fs
RESOURCE TYPE : file system
RESOURCE LOCN : mda-18.sdsc.edu:NULL:NULL
 ------------------------------------------------------------ 
test_srb_mdas_get ntest1 public CANDO  write npaci '' 200
opened
authorized
Main: Missing object data  in database: -3204
test point 4
test_srb_mdas_mod 9 ntest1 '/tmp/ntest1' 'sdsc-mda18-fs' '' testuser1 CACA npaci srb@npaci read
opened
authorized
DONE
test_srb_mdas_mod 9 ntest1 '/tmp/ntest1' 'sdsc-mda18-fs' '' testuser1 CACA npaci srb@npaci 'read audit'
opened
authorized
DONE
test_srb_mdas_mod 9 ntest1 '' '' '' testuser1 CACA npaci public@npaci read
opened
authorized
DONE
test_srb_mdas_mod 9 ntest1 '/tmp/ntest1' 'sdsc-mda18-fs' '' testuser1 CACA npaci testuser2@npaci write
opened
authorized
DONE
test_srb_mdas_mod 9 ntest1 '/tmp/ntest1' 'sdsc-mda18-fs' '' testuser1 CACA npaci srb@npaci 'rad'
opened
authorized
DONE
test_srb_mdas_mod 9 ntest1 '/tmp/ntest1' 'sdsc-mda18-fs' '' testuser1 CACA npaci aaa@npaci read
opened
authorized
ERROR: -3203
test_srb_mdas_mod 13 ntest1 '/tmp/ntest1' 'sdsc-mda18-fs' '' testuser1 CACA npaci 'this is a test dataset'
opened
authorized
DONE
test_srb_mdas_mod 13 ntest1 '/tmp/ntest1' 'sdsc-mda18-fs' '' testuser1 CACA npaci 'this is a test dataset'
opened
authorized
DONE
test_srb_mdas_mod 13 ntest1 '/tmp/ntest1' 'sdsc-mda18-fs' '' testuser1 CACA npaci 'this is a test dataset'
opened
authorized
DONE
test_srb_mdas_mod 13 ntest1 '/tmp/new/ntest1' 'sdsc-mda18-fs' '' testuser1 CACA npaci 'this is a test dataset'
opened
authorized
DONE
test_srb_mdas_mod 13 ntest2 '/tmp/ntest2' 'sdsc-mda18-fs' '' testuser1 CACA npaci 'this is a test dataset2'
opened
authorized
DONE
test_srb_mdas_mod 12 ntest1 '/tmp/ntest1' 'sdsc-mda18-fs' '' testuser1 CACA npaci ''
opened
authorized
DONE
test_srb_mdas_mod 12 ntest1 '' '' '' testuser1 CACA npaci ''
opened
authorized
DONE
test_srb_mdas_mod 12 ntest2 '' '' '' testuser1 CACA npaci ''
opened
authorized
DONE
test_srb_mdas_mod 14 ntest1 '/tmp/ntest1' 'sdsc-mda18-fs' '' testuser1 CACA npaci 'execute failure' 'installation test'
opened
authorized
DONE
test_srb_mdas_mod 14 ntest1 '/tmp/ntest1' 'sdsc-mda18-fs' '' testuser1 CACA npaci 'execute fialure' 'installation test'
opened
authorized
ERROR: -3228
test_srb_mdas_mod 15 ntest1 '/tmp/new/ntest1' 'sdsc-mda18-fs' '/g1/g1/g1/srbtest/dir2' testuser1 CACA npaci sekar sdsc
opened
authorized
ERROR: -3203
test_srb_mdas_mod 15 ntest1 '' '' '/g1/g1/g1/srbtest/dir2' testuser1 CACA npaci srb sdsc
opened
authorized
DONE
test_srb_mdas_mod 15 ntest1 '/tmp/new/ntest1' 'sdsc-mda18-fs' '/g1/g1/g1/srbtest/dir2' testuser1 CACA npaci sekar sdsc
opened
authorized
ERROR: -2866
test_srb_mdas_mod 15 ntest1 '/tmp/new/ntest1' 'sdsc-mda18-fs' '/g1/g1/g1/srbtest/dir2' testuser1 CACA npaci testuser1 sdsc
opened
authorized
ERROR: -2866
test_srb_mdas_mod 15 ntest1 '/tmp/new/ntest1' 'sdsc-mda18-fs' '/g1/g1/g1/srbtest/dir2' testuser1 CACA npaci testuser1 npaci
opened
authorized
ERROR: -2866
test_srb_mdas_mod 15 ntest1 '' '' '/g1/g1/g1/srbtest/dir2' testuser1 CACA npaci  testuser1 npaci
opened
authorized
ERROR: -2866
test_srb_mdas_mod 1 ntest2 '/tmp/ntest2' 'sdsc-mda18-fs' '' testuser1 CACA npaci 0
opened
authorized
DONE
test_srb_mdas_mod 1 ntest2 '/tmp/ntest2' 'sdsc-mda18-fs' '' testuser1 CACA npaci 0
opened
authorized
ERROR: -3201
test_srb_mdas_mod 1 ntest1 '' '' '' testuser1 CACA npaci 0
opened
authorized
DONE
test_srb_mdas_modu 17 srb CANDO npaci testuser1
opened
authorized
DONE
test_srb_mdas_modu 17 srb CANDO npaci testuser2
opened
authorized
DONE
test_srb_mdas_modu 17 srb CANDO groups ngroup1
opened
authorized
DONE
test_srb_mdas_modu 17 srb CANDO groups ngroup2
opened
authorized
DONE
test_srb_mdas_mod 8 srbdatatest1 '' '' '' srb CANDO npaci public@npaci 'read audit'
opened
authorized
DONE
test_srb_mdas_audit public  srbdatatest1 '/tmp/srbdatatest1' 'sdsc-mda18-fs' '' 'read audit' 'testing cat 1' 1  CANDO npaci
opened
authorized
Audit done
test_srb_mdas_audit public  srbdatatest1 '/tmp/srbdatatest1' 'sdsc-mda18-fs' '' 'read audit' 'testing cat 1' 0  CANDO npaci
opened
authorized
Audit done
test_srb_mdas_audit public  srbdatatest1 '/tmp/srbdatatest1' 'sdsc-mda18-fs' '' 'raudit' 'testing cat 1' 1  CANDO npaci
opened
authorized
Audit not performed: -3228
test_srb_mdas_audit public  srbdata1 '/tmp/srbdatatest1' 'sdsc-mda18-fs' '' 'read audit' 'testing cat 1' 1  CANDO npaci
opened
authorized
Audit not performed: -3201
test_srb_mdas_audit public  srbdatatest1 '/tmp/srbdata1' 'sdsc-mda18-fs' '' 'read audit' 'testing cat 1' 1  CANDO npaci
opened
authorized
Audit not performed: -3217
