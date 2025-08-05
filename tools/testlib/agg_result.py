#!/usr/bin/env python
# -*- coding:utf-8 -*- 

class QpHit:
    def __init__(self, root):
        self.root = root
        self.spellCheck = None
        self.rewrite = None
        self.isUrl = None
        self.adultLevel = None

    def getSpellCheck(self):
        return self.spellCheck

    def setSpellCheck(self, spellCheck):
        self.spellCheck = spellCheck
    
    def getRewrite(self):
        return self.rewrite

    def setRewrite(self, rewrite):
        self.rewrite = rewrite

    def getIsUrl(self):
        return self.isUrl

    def setIsUrl(self, isUrl):
        self.isUrl = isUrl

    def getAdultLevel(self):
        return self.adultLevel

    def setAdultLevel(self, adultLevel):
        self.adultLevel = adultLevel

class QpResult:
    def __init__(self, root):
        self.root = root
        self.hitList = list()

    def getHitList(self):
        return self.hitList

    def addHit(self, hit):
        if hit:
            self.hitList.append(hit)

class RsResult:
    def __init__(self, root):
        self.root = root
        self.hitList = list()

    def getHitList(self):
        return self.hitList

    def addHit(self, hit):
        if hit:
            self.hitList.append(hit)

class ScHit:
    def __init__(self, root):
        self.root = root
        self.name = None

    def getName(self):
        return self.name

    def setName(self, name):
        self.name = name

class QrsHit:
    def __init__(self, root):
        self.root = root
        self.clusterName = None
        self.hashId = None
        self.docId = None
        self.tracer = None
        self.score = None
        self.fieldMap = dict()

    def getClusterName(self):
        return self.clusterName

    def setClusterName(self, clusterName):
        self.clusterName = clusterName

    def getHashId(self):
        return self.hashId

    def setHashId(self, hashId):
        self.hashId = hashId

    def getDocId(self):
        return self.docId

    def setDocId(self, docId):
        self.docId = docId

    def getTracer(self):
        return self.tracer

    def setTracer(self, tracer):
        self.tracer = tracer

    def getScore(self):
        return self.score

    def setScore(self, score):
        self.score = score

    def getFields(self):
        return self.fieldMap

    def setFields(self, fieldMap):
        if fieldMap:
            self.fieldMap = fieldMap

    def setField(self, key, value):
        if key and value:
            self.fieldMap[key] = value

    def getUrl(self):
        if self.fieldMap.has_key("RawUrl"):
            return self.fieldMap["RawUrl"]
        return None

    def getTitle(self):
        if self.fieldMap.has_key("Title"):
            return self.fieldMap["Title"]
        return None

    def getSummary(self):
        if self.fieldMap.has_key("V_MAIN_BODY"):
            return self.fieldMap["V_MAIN_BODY"]
        return None

class HitResult:
    def __init__(self, root):
        self.root = root
        self.totalHits = 0
        self.numHits = 0
        self.hitList = list()

    def getTotalHits(self):
        return self.totalHits
    
    def setTotalHits(self, totalHits):
        self.totalHits = totalHits

    def getNumHits(self):
        return self.numHits
        
    def setNumHits(self, numHits):
        self.numHits = numHits

    def getHitList(self):
        return self.hitList

    def addHit(self, hit):
        if hit:
            self.hitList.append(hit)

class AggResult:
    def __init__(self, root):
        self.root = root
        self.hitResult = None
        self.rsResult = None
        self.qpResult = None
        self.debugResult = None
        self.generalInfo = dict()

    def __str__(self):
        bytes = ""
        if self.hitResult is None:
            return ""
        hitList = self.hitResult.getHitList()
        for hit in hitList:
            if isinstance(hit, ScHit):
                bytes += "sc: " + hit.getName() + "\n"
            elif isinstance(hit, QrsHit):
                bytes += "url: " + hit.getUrl() + "\n"
                bytes += "title: " + hit.getTitle() + "\n"
                bytes += "summary: " + hit.getSummary() + "\n"
            else:
                return ""
            bytes += "\n"
        bytes += "\n"

        return bytes

    def getHitResult(self):
        return self.hitResult

    def setHitResult(self, result):
        if result:
            self.hitResult = result

    def getRsResult(self):
        return self.rsResult

    def setRsResult(self, result):
        if result:
            self.rsResult = result

    def getQpResult(self):
        return self.qpResult

    def setQpResult(self, result):
        if result:
            self.qpResult = result

    def getDebugResult(self):
        return self.debugResult

    def setDebugResult(self, result):
        if result:
            self.debugResult = result

    def getGeneralInfo(self):
        return self.generalInfo

    def setGeneralInfo(self, result):
        if result:
            self.generalInfo = result
    
