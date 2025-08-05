#!/bin/sh
searchAppName=$SEARCH_APP_NAME
if [ $SEARCH_APP_NAME ];then
    echo "env app:"$SEARCH_APP_NAME
else
    searchAppName='a4_agg_quarkchat'
fi
if [ -z $SEARCH_APP_NAME ];then 
    searchAppName='a4_agg_quarkchat'
fi
echo "a4_biz_start, app: "$searchAppName
ln -s /a4/usr/local/var/a4_conf/$searchAppName /a4/usr/local/var/conf
