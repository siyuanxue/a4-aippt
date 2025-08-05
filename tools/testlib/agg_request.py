#! /usr/bin/env python
# -*- coding:utf-8 -*-

import urllib

class AggRequest:
    queryTemplate = "cluster=fastindex,galaxy&&config=navigationalQuery:on,filtermode:11,start:%(start)s,hit:%(hit)s,searcher_return_hits:10,kvpairs:ip#%(ip)s;st#wap;tl#54;sl#100;fr#%(fr)s&&distinct=dist_key:V_HOST_HASH,dist_count:5;dist_key:V_HOST_HASH,dist_count:2&&query=phrase:'%(query)s'"
    arbiterTemplate = "ip=%(ip)s&ua=%(encodeUA)s&hid=%(hid)s&vendor=%(vendor)s&cid=%(cid)s&st=wap&tl=54&sl=100&fr=%(fr)s&q=%(encodeQuery)s&cookieid=%(uid)s&app_ve=0"
    defaultUa = "Mozilla/5.0 (Linux; U; Android 4.1.1; zh-CN; MI 2SC Build/JRO03L) AppleWebKit/533.1 (KHTML, like Gecko) Version/4.0 UCBrowser/9.9.2.467 U3/0.8.0 Mobile Safari/533.1"
    defaultFr = "android"
    defaultCid = "9"
    defaultVendor = "100002"

    def __init__(self):
        self.query = None
        self.start = "0"
        self.hit = "10"
        self.ip = "1.1.1.1"
        self.vendor = None
        self.fr = None
        self.uid = None
        self.hid = None
        self.cid = None
        self.ua = None
        self.debug = None
        self.bucket = None
        self.st = None;
        self.sl = None;
        self.tl = None;

    def encodeForNewAgg(self):
        params = {}
        for k,v in vars(self).items():
            if v is not None:
                params[k] = v
        if not params.has_key("fr"):
            params["fr"] = self.defaultFr
        if not params.has_key("ua"):
            params["ua"] = self.defaultUa
        if not params.has_key("vendor"):
            params["vendor"] = self.defaultVendor
        if not params.has_key("cid"):
            params["cid"] = self.defaultCid

        return params
    
    def encodeForOldAgg(self):
        if self.query is None:
            return None
        if self.vendor is None:
            self.vendor = self.defaultVendor
        if self.fr is None:
            self.fr = self.defaultFr
        if self.ua is None:
            self.ua = self.defaultUa
        if self.uid is None:
            self.uid = ""
        if self.hid is None:
            self.hid = ""
        if self.cid is None:
            self.cid = self.defaultCid

        encodeUa = urllib.quote(self.ua)
        encodeQuery = urllib.quote(self.query)

        query = self.queryTemplate % {"start":self.start, "hit":self.hit, "ip":self.ip, "fr":self.fr, "query":self.query}
        arbiterQuery = self.arbiterTemplate % {"ip":self.ip, "encodeUA":encodeUa, "hid":self.hid, "vendor":self.vendor, "cid":self.cid, "fr":self.fr, "encodeQuery":encodeQuery, "uid":self.uid}
        relativeQuery = "q=" + encodeQuery

        params = {}
        params["galaxy"] = query
        params["galaxy_key"] = query
        params["vendor"] = self.vendor
        params["ua"] = encodeUa
        params["arbiter"] = arbiterQuery
        if self.debug is not None:
            params["debug"] = "true"

        return params
        

if __name__ == "__main__":
    request = AggRequest()
    request.query = "nba"
    request.debug = "true"
    print request.encodeForNewAgg()
    print request.encodeForOldAgg()
