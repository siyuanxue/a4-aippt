#!/bin/sh

if [ $# -lt 2 ]
then
    echo "usage: $0 AINST__INSTALLROOT DATA_UPDATE_FILE"
    exit 1
fi

AINST__INSTALLROOT=$1
DATA_UPDATE_FILE=$2
crontab_job="*/1 * * * * python $AINST__INSTALLROOT/bin/data_checker update -c $DATA_UPDATE_FILE"
( crontab -l | grep -v "$crontab_job"; echo "$crontab_job" ) | crontab -
#crontab */1 * * * * python $AINST__INSTALLROOT/bin/data_checker update -c $DATA_UPDATE_FILE
