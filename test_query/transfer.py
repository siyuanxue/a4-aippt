#! /usr/bin/env python
# -*- coding:utf-8 -*-

import os
import sys
import time
import datetime
import json
import hashlib
import re
import getopt

def construct(attr):
    query_info = {}
    #data = json.loads(json.dumps(query_info))

    mlist = []
    for i in range(0,len(attr)):
        list = {}
        list["role"] = "user"
        list["content"] = attr[i]
        mlist.append(list)

    query_info["prompt"] = mlist
    empty_obj = {}
    query_info["passparams"] = {}
    param = json.dumps(query_info, ensure_ascii=False).replace(' ', '')

    return param


if __name__ == '__main__':
     if len(sys.argv) != 3:
         print('usage: python transfer.py batch_query_list.txt eps_batch_query_list.txt\n')
         sys.exit(255)
     input = sys.argv[1]
     output = sys.argv[2]

     fp = open(input, 'r')
     OUT = open(output,'w')
     while True:
         line = fp.readline()
         if not line:
             break
         content = line.strip()
         attr = content.split('\t')
         input = construct(attr)

         print >>OUT, '%s' % (input)
     fp.close()
     OUT.close()
     print "done"
