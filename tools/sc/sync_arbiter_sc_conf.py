#! /usr/bin/env python
# -*- coding:utf-8 -*-

import os
import sys
import json
import urllib
import urllib2

ude_api = 'https://ude-open-api.alibaba-inc.com'
ude_private_token = 'Ds_iB3PpS_SH_ikuMw9LrQ'

all_sc_exclusive_conf = []

def add_rule(scname, exclusive_sc):
    global all_sc_exclusive_conf
    all_sc_exclusive_conf.append((scname, exclusive_sc))
    exclusive_sc_list = exclusive_sc.split(',')
    for sc_ex in exclusive_sc_list:
        #print '%s:%s' % (scname, sc_ex)
        pass

def load_sc_exclusive_info(sc_conf):
    sc_info = sc_conf['sc_info'] 
    for scname in sc_info:
        if scname.find('*') >= 0:
            print '%s -> %s' % (sc_conf['sc_app_name'], scname)
        exclusion_info = sc_info[scname]['exclusion']
        for k in exclusion_info:
            v = exclusion_info[k]
            if k == '':
                add_rule(scname, v)
            else:
                add_rule(k, v)


def get_http_result(url):
    try:
        req = urllib2.Request(url)
        res_data = urllib2.urlopen(req)
        res = res_data.read()
        return res 
    except:
        print 'request fail:%s' % url
        return None

def get_all_sc_conf():
    url = ude_api + "/api/v1/zeus-sc/sc-app-list?ude-private-token=" + ude_private_token
    res = get_http_result(url)
    json_obj = json.loads(res)
    if json_obj['status'] != 0:
        print 'Get sc list fail[%d]:%s' % (json_obj['status'], json_obj['msg'])
        return
    print 'SC number:%d' % len(json_obj['data'])
    scList = json_obj['data']
    for scname in scList:
        u = ude_api + '/api/v1/zeus-sc/sc-app-config?appName=%s&ude-private-token=%s' % (scname, ude_private_token)
        result = get_http_result(u)
        if result == None:
            continue
        root = json.loads(result)
        if root['status'] != 0:
            print 'Get sc config fail:%s' % (scname)
            continue
        load_sc_exclusive_info(root['data'])
        
def output(output_file):
    global all_sc_exclusive_conf
    fo = open(output_file, 'w')
    for conf in all_sc_exclusive_conf:
        fo.write('%s\t%s\n' % (conf[0], conf[1]))
    fo.close()
    print 'Output done : %s' % output_file



def main(output_file):
    get_all_sc_conf()
    output(output_file)
    pass

def Usage():
    print ' python sync_arbiter_sc_conf.py [output_file]'
    exit(-1)

if __name__ == "__main__":
    if (len(sys.argv) < 2):
        Usage()

    print 'Output file: %s' % sys.argv[1]
    main(sys.argv[1])
