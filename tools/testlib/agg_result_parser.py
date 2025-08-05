#!/usr/bin/env python
# -*- coding:utf-8 -*- 

from lxml import etree
import mylxml
from agg_result import *

class AggResultParser:
    def __init__(self):
        self.root = None

    def parse(self, xmlContent):
        self.root = self.__parseXmlString(xmlContent)
        if self.root is None:
            return None
        aggResult = AggResult(self.root)

        elem = mylxml.get_uniq_child_element(self.root, "engine")
        hitResult = self.__parseHitResult(elem)
        if hitResult is None:
            return None
        aggResult.setHitResult(hitResult)

        elem = mylxml.get_uniq_child_element(self.root, "rs")
        rsResult = self.__parseRsResult(elem)
        aggResult.setRsResult(rsResult)

        elem = mylxml.get_uniq_child_element(self.root, "qp")
        qpResult = self.__parseQpResult(elem)
        aggResult.setQpResult(qpResult)

        elem = mylxml.get_uniq_child_element(self.root, "general_info")
        generalInfo = self.__parseGeneralInfo(elem)
        if generalInfo:
            aggResult.setGeneralInfo(generalInfo)

        elem = mylxml.get_uniq_child_element(self.root, "debug")
        debugResult = self.__parseDebugResult(elem)
        aggResult.setDebugResult(debugResult)
        
        return aggResult

    def __parseHitResult(self, root):
        if root is None:
            return None
        hitsElem = mylxml.get_uniq_child_element(root, "hits")
        if hitsElem is None:
            return None

        hitResult = HitResult(hitsElem)
        value = mylxml.get_attribute(hitsElem, "totalhits")
        if value:
            hitResult.setTotalHits(int(value))
        value = mylxml.get_attribute(hitsElem, "numhits")
        if value:
            hitResult.setNumHits(int(value))

        hitElems = mylxml.get_child_elements(hitsElem, "hit")
        for hitElem in hitElems:
            hit = self.__parseHit(hitElem)
            if not hit:
                return None
            hitResult.addHit(hit)
            
        return hitResult

    def __parseRsResult(self, root):
        if root is None:
            return None
        hintsElem = mylxml.get_uniq_child_element(root, 'hints')
        if hintsElem is None:
            return None

        rsResult = RsResult(hintsElem)
        hintElems = mylxml.get_child_elements(hintsElem, "hint")
        for elem in hintElems:
            rsResult.addHit(elem.text.encode("utf-8"))

        return rsResult

    def __parseQpResult(self, root):
        if root is None:
            return None
        
        qpResult = QpResult(root)
        qpElems = mylxml.get_child_elements(root, "search")
        for qpElem in qpElems:
            qpHit = QpHit(qpElem)
            elem = mylxml.get_uniq_child_element(qpElem, "spellcheck")
            if elem is not None:
                qpHit.setSpellCheck(elem.text)
            elem = mylxml.get_uniq_child_element(qpElem, "rw")
            if elem is not None:
                qpHit.setRewrite(elem.text)
            elem = mylxml.get_uniq_child_element(qpElem, "isurl")
            if elem is not None:
                qpHit.setIsUrl(elem.text)
            elem = mylxml.get_uniq_child_element(qpElem, "adult_level")
            if elem is not None:
                qpHit.setAdultLevel(elem.text)
            qpResult.addHit(qpHit)

        return qpResult

    def __parseGeneralInfo(self, root):
        if root is None:
            return None
        
        generalInfoMap = {}
        elems = mylxml.get_all_child_elements(root)
        for elem in elems:
            generalInfoMap[elem.tag] = elem.text.encode("utf-8")

        return generalInfoMap

    def __parseDebugResult(self, root):
        return None

    def __parseHit(self, root):
        if root is None:
            return None

        value = mylxml.get_attribute(root, "name")
        if value:
            hit = ScHit(root)
            hit.setName(value)
            return hit
        
        hit = QrsHit(root)
        hit.setClusterName(mylxml.get_attribute(root, "cluster"))
        hit.setHashId(mylxml.get_attribute(root, "hashid"))
        hit.setDocId(mylxml.get_attribute(root, "docid"))
        hit.setScore(mylxml.get_attribute(root, "score"))
    
        fieldsElem = mylxml.get_uniq_child_element(root, 'fields')
        if len(fieldsElem) < 3:
            return None
        for fieldElem in fieldsElem:
            hit.setField(fieldElem.tag, fieldElem.text.encode("utf-8"))

        traceElem = mylxml.get_uniq_child_element(root, 'trace_info')
        if traceElem is not None:
            hit.setTracer(traceElem.text)
    
        return hit

    def __parseXmlString(self, xmlContent):
        try:
            node = etree.XML(xmlContent)
            return node
        except Exception, err:
            print str(err)
        return None

class OldAggResultParser(AggResultParser):
    def __init__(self):
        AggResultParser.__init__(self)

    def __parseHit(self, root):
        if root is None:
            return None

        value = mylxml.get_attribute(root, "name")
        if value:
            hit = ScHit(root)
            hit.setName(value)
            return hit
        
        hit = QrsHit(root)
        hit.setClusterName(mylxml.get_attribute(root, "cluster"))
        hit.setHashId(mylxml.get_attribute(root, "hashid"))
        hit.setDocId(mylxml.get_attribute(root, "docid"))
    
        fieldsElem = mylxml.get_uniq_child_element(root, 'fields')
        if len(fieldsElem) < 3:
            return None
        for fieldElem in fieldsElem:
            hit.setField(fieldElem.tag, fieldElem.text.encode("utf-8"))

        traceElem = mylxml.get_uniq_child_element(root, 'trace_info')
        if traceElem is not None:
            hit.setTracer(traceElem.text)

        scoreElem = mylxml.get_uniq_child_element(root, 'agg_score')
        if scoreElem is not None:
            hit.setScore(scoreElem.text)
    
        return hit


if __name__ == "__main__":
    content = open('testdata/test_agg_result_parser.xml').read()
    aggResultParser = AggResultParser()
    aggResult = aggResultParser.parse(content)
    print aggResult

    content = open('testdata/test_old_agg_result_parser.xml').read()
    aggResultParser = OldAggResultParser()
    aggResult = aggResultParser.parse(content)
    print aggResult
