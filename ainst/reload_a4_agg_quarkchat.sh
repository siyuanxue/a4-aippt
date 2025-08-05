#!/bin/sh

if [ $# -lt 1 ]
then
    echo "usage: $0 AINST__INSTALLROOT"
    exit 1
fi

AINST__INSTALLROOT=$1

sleep $(( ( RANDOM % 100 )  + 1 ))s
python $AINST__INSTALLROOT/bin/a4_agg_tool reload -c $AINST__INSTALLROOT/conf/a4_agg_quarkchat/a4_agg_quarkchat_conf.xml
if [ $? -ne 0 ]
then
    echo "reload data failed"
    exit 1
fi

