#! /usr/bin/env python
# -*- coding:utf-8 -*-

import os
import sys
import multiprocessing
import threading

rootPath = os.path.dirname(os.path.abspath(os.path.split(os.path.realpath(__file__))[1]))
sys.path.append(rootPath + "/testlib/")

from agg_request import AggRequest
from agg_result_parser import AggResultParser, OldAggResultParser
from agg_result import *
from agg_client import AggClient

allResult = [];
mutex = threading.Lock();

def NormalizeStr(instr):
    instr = instr.replace("\r", "");
    instr = instr.replace("\n", "");
    instr = instr.replace("\t", "");
    return instr;

def requestInit(request):
    request.ua = "Mozilla/5.0 (Linux; Android 4.1.1; Nexus 7 Build/JRO03D) AppleWebKit/535.19 (KHTML, like Gecko) Chrome/18.0.1025.166 Safari/535.19 UCBrowser/8.6.0.199 Mobile aliyun-search-debugger";
    request.start = "0";
    request.hit = "10";
    request.ip = "61.158.152.142";
    request.vendor = "100002";
    request.fr = "android";
    request.uid = "f48fbe9016219605a4269aee52a448d0|||1418194364";
    request.cid = "9";
    request.st = "wap";
    request.sl = "100";
    request.tl = "54";

def requestOldAgg(query, spec):
    request = AggRequest()
    request.query = query
    params = request.encodeForOldAgg()
    if params is None:
        return None
    result = AggClient().post(spec, params=params)
    if result is None:
        return None
    
    aggResult = AggResultParser().parse(result)
    return getResultStr(query, aggResult);

def requestNewAgg(query, spec):
    request = AggRequest()
    requestInit(request);
    request.query = query
    params = request.encodeForNewAgg()
    params["debug"] = "fp=cache_read_processor,cache_update_processor";
    if params is None:
        return None
    result = AggClient().get(spec, "/agg",params=params)
    if result is None:
        return None
    aggResult = AggResultParser().parse(result)
    return getResultStr(query, aggResult);

def getResultStr(query, aggResult):
    bytes = ""
    if aggResult.hitResult is None:
        return ""
    hitList = aggResult.hitResult.getHitList();
    nowPos = 1;
    for hit in hitList:

        if isinstance(hit, ScHit):
            bytes += str(nowPos) + "\t" + query;
            bytes += "\tno url -- sc:" + hit.getName();
            bytes += "\t\t\t" + hit.getName();
            bytes += "\t\t\t\t\t\t";
        elif isinstance(hit, QrsHit):
            bytes += str(nowPos) + "\t" + query;
            bytes += "\t" + hit.getUrl();
            bytes += "\t\t\t" + NormalizeStr(hit.getTitle());
            bytes += "\t\t\t" + NormalizeStr(hit.getSummary());
            bytes += "\t\t\t ";
        else:
            continue ;

        bytes += "\n"
        nowPos = nowPos + 1;

    return bytes;

def addResult(res):
    mutex.acquire();
    allResult.append(res);
    if (len(allResult) % 5 == 0):
        print "Successed Crawl:", len(allResult);
    mutex.release();

def CrawlResult(service, infile, outfile):

    print "SERVICE:  ",service;
    print "IN_FILE:  ",infile;
    print "OUT_FILE: ",outfile;
    
    inFile = open(infile, "r");
    outFile = open(outfile, "w");

    manager = multiprocessing.Manager()
    pool = multiprocessing.Pool(processes=5)
    for line in inFile:
        line = line.split("\n")[0];
        pool.apply_async(requestNewAgg, args=(line, service, ), callback=addResult);

    pool.close()
    pool.join()
    
    for res in allResult:
        if (res != None):
            outFile.write(res);

    inFile.close();
    outFile.close();
    
if __name__ == "__main__":
    if (len(sys.argv) != 4):
        print "FormatError!";
        print "python crawl.py 10.99.88.15:9092 in.file out.file";
        exit(-1);

    CrawlResult(sys.argv[1], sys.argv[2], sys.argv[3]);
    # print requestOldAgg("微信", "10.99.88.15:9092")
    # print requestNewAgg("nba", "10.99.0.58:11111")
