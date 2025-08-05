# -*- coding: UTF-8 -*-
import sys
sys.path.append('./proxy_pb')
sys.path.append('./site-packages/')

import urllib2
from google.protobuf.text_format import Merge, MessageToString
import ProxyResult_pb2

data = None
host = sys.argv[1]
url = sys.argv[2]
response = urllib2.urlopen('http://%s/?%s' % (host, url), data).read()

reply = ProxyResult_pb2.ProxyResult()
reply.MergeFromString(response)
print MessageToString(reply)
