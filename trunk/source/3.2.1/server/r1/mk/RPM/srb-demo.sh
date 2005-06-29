#!/bin/sh

# This file was made using Rocks from the srb-demo.xml file with this command:
# kpp --client frontend-node srb-demo | kgen > srb-demo.sh

cat > ~srb/.srb/.MdasEnv << 'EOF'
mdasCollectionName '/home/linuxsrb.sdsc'
mdasCollectionHome '/home/linuxsrb.sdsc'
mdasDomainHome 'sdsc'
srbUser 'linuxsrb'
AUTH_SCHEME 'ENCRYPT1'
srbHost 'localhost'
srbPort '5544'
defaultResource 'unix-sdsc'
SERVER_DN  '/C=US/O=NPACI/OU=SDSC/USERID=srb/CN=Storage Resource Broker/Email=srb@sdsc.edu'
EOF


cat > ~srb/.srb/.MdasAuth << 'EOF'
iwannasrb
EOF


cat > ~srb/data/mcatHost << 'EOF'
# The MCAT enabled host file. 
# Line 1 is the host address of the MCAT enabled host.
srb.sdsc.edu
# Line 2 is the AUTH_SCHEME used to connect the MCAT enabled server. 
# valid schemes are : PASSWD_AUTH, SEA_AUTH, SEA_ENCRYPT,
# GSI_AUTH, GSI_SECURE_COMM
ENCRYPT1
# Line 3 is the Dn of the MCAT enabled server.
/C=US/O=NPACI/O=DICE/UID=srb/CN=Storage Resource Broker/Email=srb@sdsc.edu
EOF


cat > ~srb/data/hostConfig << 'EOF'
localhost bcc-gpop bcc-gpop.sdsc.edu
EOF



PORTCFG=~srb/bin/runsrb
gawk '                                                          \
/srbPort=/ {                                                     \
        printf("srbPort=5544");                                 \
        printf("\n");                                           \
        next;                                                   \
}                                                               \
{                                                               \
        print $0;                                               \
}' $PORTCFG > /tmp/port.cfg
mv /tmp/port.cfg $PORTCFG
chmod 755 $PORTCFG
chown srb:srb $PORTCFG


