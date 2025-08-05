#! /usr/bin/env python
# -*- coding:utf-8 -*-

import sys
import urllib
import urllib2

class AggClient:
    def __init__(self):
        pass

    def get(self, spec, uri="/", params=None):
        if spec is None:
            return None
        if uri is None:
            uri = "/"
        url = "http://" + spec + uri
        if params is not None:
            args = urllib.urlencode(params)
            url += "?" + args

        try:
            # print url;
            req = urllib2.Request(url)
            response = urllib2.urlopen(req)
            content = response.read()
        except Exception, e:
            print >> sys.stderr, "request http failed [%s]" % str(e)
            return None
        
        return content


    def post(self, spec, uri="/", params=None):
        if spec is None:
            return None
        if uri is None:
            uri = "/"
        url = "http://" + spec + uri

        try:
            body = ""
            if params is not None:
                body = urllib.urlencode(params)
            req = urllib2.Request(url, body)
            response = urllib2.urlopen(req)
            content = response.read()
        except Exception, e:
            print >> sys.stderr, "request http failed [%s]" % str(e)
            return None
        
        return content

if __name__ == "__main__":
    aggClient = AggClient()
    print aggClient.get("10.99.20.61:9096", "/", {"query":"abc"})
    print aggClient.post("10.99.20.61:9096", params={"query":"def"})

