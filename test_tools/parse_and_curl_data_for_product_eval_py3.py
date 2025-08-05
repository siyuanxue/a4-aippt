#!/usr/bin/env python
# encoding: utf-8

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import sys
import requests
import json
import random
import uuid
# import secrets
import base64
#import tqdm
import time
import os
import codecs
import ast


url_online = "http://a4-agg-quarkman.zeus.vipserver:26962/agg"
url_pub =    "http://a4-agg-quarkman-pub6.zeus.vipserver:26962/agg"


def _get_bts_bucket():
    buckets = {"llmplan":"2","llmrewrite":"7","llmcachewrite":"1"}
    #bts_hosts = get_ip_from_cm("sm_bts_pub6","em21_5u",datetime.datetime.now().minute)
    #if not bts_hosts:
    #    logger.info("get_bts_ip_list error,request_id:%s",request_id)
    #    return buckets
    #random.shuffle(bts_hosts)
    # bts_hosts = ['11.251.197.40:8956']
    # bts_hosts = ['11.180.51.1:8956']
    bts_hosts = ['sm_bts_pub6.zeus.vipserver:8956']
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
    return buckets


global_buckets = _get_bts_bucket()


def request_demo(url, query, sessid):
    buckets = global_buckets

    reqid = 'eps_andylp_20230928_' + str(uuid.uuid1())
    uid = "eps_andylp_20230928_a4-agg-quarkman-websearch"

    header = {
        "Accept":"text/event-stream"
    }

    data = {
        "version": 1,
        "target": "request",
        "communication": {
            "uid":uid,
            "sessionid":sessid,
            "reqid":reqid,
            "resid":0
        },
        "data": {
            "message": {
                "query":query,
                "status":"init"
            }
        },
        "bucket": buckets,
        "error_code":0,
        "error_msg": "",
        "Connection":"close",
        "timestamp":"1682390791"
    }
    #data = json.dumps(data,ensure_ascii=False).replace(' ', '').encode('utf-8')
    data = json.dumps(data).replace(' ', '')

    # ######## 同步请求 ###############
    max_try = 3
    while max_try > 0:
        try:
            resp = requests.post(url, data=data, headers=header)
            if resp.status_code != 200:
                print('Create request failed.')
                return ""
            resp_data = resp.content
            return resp_data
        except Exception as e:
            print(e)
            max_try -= 1
    return ""


def test_query(query):
    sessid = 'eps_andylp_20231007_' + str(uuid.uuid1())
    resp_data = request_demo(url_online, query, sessid)
    print(resp_data)
    jj = {'query': query, 'resp_data': resp_data}
    return jj


def test_query_diff(query):
    sessid = 'eps_andylp_20231007_' + str(uuid.uuid1())
    resp_data1 = request_demo(url_online, query, sessid)
    sessid = 'eps_andylp_20231007_' + str(uuid.uuid1())
    #resp_data2 = request_demo(url_pub, query, sessid)
    resp_data2 = resp_data1
    jj = {'query': query, 'resp_data_online': resp_data1.strip(), 'resp_data_pub': resp_data2.strip()}
    return jj


def proc_file(ifile):
    ofile = ifile + '.curl_result'
    querys = [line.strip() for line in open(ifile, 'r')]
    print('******** START')
    with codecs.open(ofile, "w", 'utf-8') as saved_file:
        for query in querys:
            jj = test_query_diff(query)
            tmp_data_json = json.dumps(jj, encoding='utf8', ensure_ascii=False)
            saved_file.write(tmp_data_json  + "\n")
    print("saved [%s] file done." % ofile)



if __name__ == "__main__":
    if sys.argv[1] == 'query':
        test_query(sys.argv[2])
    elif sys.argv[1] == 'file':
        proc_file(sys.argv[2])
