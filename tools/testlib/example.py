#! /usr/bin/env python
# -*- coding:utf-8 -*-

from agg_request import AggRequest
from agg_result_parser import AggResultParser, OldAggResultParser
from agg_result import *
from agg_client import AggClient

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
    return aggResult

def requestNewAgg(query, spec):
    request = AggRequest()
    request.query = query
    params = request.encodeForNewAgg()
    if params is None:
        return None
    result = AggClient().get(spec, params=params)
    if result is None:
        return None
    aggResult = AggResultParser().parse(result)
    return aggResult
    
if __name__ == "__main__":
    print requestOldAgg("abc", "10.99.88.15:9092")
    print requestNewAgg("abc", "10.99.20.61:9096")
