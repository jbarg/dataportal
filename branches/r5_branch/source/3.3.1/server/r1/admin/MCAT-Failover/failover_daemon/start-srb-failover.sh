#!/bin/bash
################################################################################
## start-srb-failover.sh
##   by Roman Olschanowsky 2004
##
##   Starts the SRB Failover daemon
################################################################################

cd /opt/srb/failover_daemon

if [ -e run.pid ]
then
  echo "SRB Failover daemon already running"
  exit 0;
fi

nohup ./srb-failover-daemon &
