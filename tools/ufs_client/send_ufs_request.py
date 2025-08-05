# -*- coding: UTF-8 -*-
import sys
sys.path.append('./pb')
sys.path.append('../arbiter_request/site-packages/')

import urllib2
from google.protobuf.text_format import Merge, MessageToString
import ufs_interface_pb2

'''
request = rf_pb2.Request()
doc = request.doc.add()
doc.title = u'威尼斯人娱乐城'
doc.url = 'http://url1'
doc.summary = 'this is summary'

doc = request.doc.add()
doc.title = u'赌博爆炸爆破七星彩'
doc.title = u'威尼斯人娱乐城'
doc.url = 'http://www.56case.com/index.html'
doc.summary = u'极速快3全天计划'
#data = request.SerializeToString()
'''
data = open('request_0_1', 'rb').read()
print len(data)
reply = ufs_interface_pb2.Request()
reply.MergeFromString(data)
print MessageToString(reply)

'''
a = ufs_interface_pb2.Request()
p = a.params.add()
p.key = 'keya'
p.value = 'valuea'
print '###'
print a.SerializeToString()
'''
