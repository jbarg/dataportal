#!/bin/csh
source $HOME/.cshrc
echo "connecting to mcat database"
db2 connect to mcat
echo "connected."
echo "CLEARING DATABASE  CLEARING DATABASE  CLEARING DATABASE  CLEARING DATABASE  "
echo "Cleaning and UnInstal starts in 20 seconds"
echo "Ctrl-C to abort"
sleep 20
echo "cleaning up mcat"
db2 -f  data/catalog.cleanup.db2
