# -*- coding: UTF-8 -*-
import sys
sys.path.append('./arbiter_pb')
sys.path.append('./site-packages/')

import urllib2
from google.protobuf.text_format import Merge, MessageToString
import arbiter_pb2

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
data = None
host = sys.argv[1]
url = sys.argv[2]
response = urllib2.urlopen('http://%s/?%s' % (host, url), data).read()
reply = arbiter_pb2.ArbiterResult()
reply.MergeFromString(response)
print MessageToString(reply)
