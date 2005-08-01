#!/bin/csh
source $HOME/.cshrc
echo "Patching MCAT: Please make sure that this patch has not been applied previously"
echo "Patching in 20 seconds"
echo "Ctrl-C to abort"
sleep 20
echo "connecting to mcat database"
db2 connect to mcat
echo "connected."
echo "patch started"
echo "patch ended"
db2 -f  data/catalog.patch.db2.118rel
echo "patch ended"

