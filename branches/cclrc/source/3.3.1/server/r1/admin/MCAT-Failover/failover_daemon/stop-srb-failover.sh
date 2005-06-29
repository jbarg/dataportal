#!/bin/sh
################################################################################
## stop-srb-failover.sh
##   by Roman Olschanowsky 2004
##
##   Stops the SRB Failover daemon
################################################################################

cd /opt/srb/failover_daemon

if [ ! -e run.pid ]
then
  echo "SRB Failover daemon is not running"
  exit 0;
fi

# Get pid to kill
dPid=`cat run.pid`

# Kill the daemon
/bin/kill $dPid

# Remove the file
rm run.pid

exit 0;
