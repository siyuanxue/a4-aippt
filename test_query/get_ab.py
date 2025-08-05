#!/usr/bin/python
#****************************************************************#
# ScriptName: get_ab.py
# Author: $SHTERM_REAL_USER@alibaba-inc.com
# Create Date: 2023-08-11 20:57
# Modify Author: $SHTERM_REAL_USER@alibaba-inc.com
# Modify Date: 2023-08-11 20:57
# Function: 
#***************************************************************#

import json
import requests
import datetime

def _get_bts_bucket():
    buckets = {"llmplan":"2","llmrewrite":"4","llmcachewrite":"1"}
    #bts_hosts = get_ip_from_cm("sm_bts_pub6","em21_5u",datetime.datetime.now().minute)
    #if not bts_hosts:
    #    logger.info("get_bts_ip_list error,request_id:%s",request_id)
    #    return buckets
    #random.shuffle(bts_hosts)
    bts_hosts = ['11.251.197.40:8956']
    try:
        url = "http://"+bts_hosts[0]+"/bts?group=all&cookie=204c646d1290196494f1947360ed03d9%7C%7C%7C1655969830&ip=42.120.75.239&fr=ios&nt=&nw=&query=%E4%BB%80%E4%B9%88%E6%98%AF%E4%B9%B3%E5%A4%B4%E7%8A%B6%E7%98%A4%E5%9B%BE%E7%89%87&from=sm_stress&buck_mode=close_interleaving_and_fix_max"
        res = requests.get(url)
        if res.status_code == 200:
            o = json.loads(res.text)
            if o is not None:
                for bucket in o:
                    buckets[bucket.get("experiment")] = bucket.get("bucket")
    except Exception as e:
        raise
    return json.dumps(buckets).replace(' ', '')

print(_get_bts_bucket())

