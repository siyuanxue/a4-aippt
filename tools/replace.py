#! /usr/bin/env python
# -*- coding:utf-8 -*-

import string
import os
import sys
import shutil
import json
import copy
import ConfigParser

nodeTemplate='\
\n\
    <!-- REPLACE BY SCRIPT FOR DEBUG MODEL -->\n\
    <node name="$(NODE_NAME)" type="http">\n\
      <conn_pool_size>200</conn_pool_size>\n \
      <timeout>10000</timeout>\n\
      <max_qps>0</max_qps>\n\
      <upstream>\n\
        <lb_strategy name="rr_strategy">\n\
          <params>\n\
          </params>\n\
        </lb_strategy>\n\
        <specs>\n\
          <spec group="1" weight="1" max_fails="10" fail_timeout="10000">$(VALUE)</spec>\n\
          <spec group="1" weight="1" max_fails="10" fail_timeout="10000">$(VALUE)</spec>\n\
          <spec group="1" weight="1" max_fails="10" fail_timeout="10000">$(VALUE)</spec>\n\
          <spec group="1" weight="1" max_fails="10" fail_timeout="10000">$(VALUE)</spec>\n\
          <spec group="1" weight="1" max_fails="10" fail_timeout="10000">$(VALUE)</spec>\n\
          <spec group="1" weight="1" max_fails="10" fail_timeout="10000">$(VALUE)</spec>\n\
        </specs>\n\
      </upstream>\n\
    </node>\n\
\n\
'

gigNodeTemplate = {
            "biz_name": "ha3_bigindex",
            "range": {
                "from": 0,
                "to": 1
                },
            "version": "0",
            "weight": 100,
            "ip": "11.251.200.28",
            "tcp_port": 10007,
            "http_port": 10007
        }

def ReplaceNode(allLine, key, value):
    # allLine = confStr.split("\n");
    # print len(allLine);
    targetStr = '<node name=\"'+key+'\"';
    
    res = nodeTemplate;
    res = res.replace("$(NODE_NAME)", key);
    res = res.replace("$(VALUE)", value);

    retRes = "";
    findNodes = False;
    resLine = [];
    replaceSuccessed = False;
    i = 0; 
    allLen = len(allLine);
    while(i < allLen):
        line = allLine[i];

        if line.find("<nodes>") != -1:
            findNodes = True;
            resLine.append(line);
            i = i+1;
            continue;

        if not findNodes:
            resLine.append(line);
            i = i+1;
            continue;

        if (line.find(targetStr) != -1) and (line.find("<!--") == -1):
            while True:
                i = i+1;
                line = allLine[i];
                if line.find("<node name=") != -1 or line.find("</nodes>") != -1:
                    resLine.append(res);
                    replaceSuccessed = True;
                    break;

        resLine.append(line);
        i = i+1;

    if replaceSuccessed:
        print "[%s] REPLACE SUCCESSED!" % key;
    else:
        print "[%s] REPLACE FAILED!" % key;

    return resLine;


def ReplaceProcessorParam(allLine, key, value):
    # allLine = confStr.split("\n");
    # print len(allLine);
    keys = key.split(",");
    if len(keys) != 2:
        print "KEYS:%s ERROR!" % key;
        return allLine;

    processorName = keys[0];
    paramKey = keys[1];
    targetStr = '<processor name=\"'+processorName+'\"';
    
    retRes = "";
    findNodes = False;
    resLine = [];
    replaceSuccessed = False;
    i = 0; 
    allLen = len(allLine);
    while(i < allLen):
        line = allLine[i];

        if line.find("<processors>") != -1:
            findNodes = True;
            resLine.append(line);
            i = i+1;
            continue;

        if not findNodes:
            resLine.append(line);
            i = i+1;
            continue;

        if (line.find(targetStr) != -1) and (line.find("<!--") == -1):
            resLine.append(line);
            while True:
                i = i+1;
                line = allLine[i];
                if line.find("<processor name=") != -1 or line.find("<processors>") != -1:
                    # resLine.append(line);
                    replaceSuccessed = True;
                    break;
                else:
                    if line.find(paramKey) == -1:
                        resLine.append(line);
                    else:
                        key1 = "<" + paramKey + ">";
                        key2 = "</" + paramKey + ">";
                        pos1 = line.find(key1);
                        pos2 = line.find(key2);
                        if pos1 != -1 and pos2 != -1:
                            str1 = line[0:(pos1+len(key1))];
                            str2 = line[pos2:len(line)];
                            replaceStr = str1 + value + str2;
                            line = replaceStr;
                        resLine.append(line);

        resLine.append(line);
        i = i+1;

    if replaceSuccessed:
        print "[%s] REPLACE SUCCESSED!" % key;
    else:
        print "[%s] REPLACE FAILED!" % key;

    return resLine;

def ProcessorOff(allLine, processor_name, method_name):
    targetStr = 'name=\"'+processor_name+'\"';
    expectionStr1 = 'class_name=';
    expectionStr2 = '<!--';
    
    targetStr2 = 'method=\"' + method_name + '\"'

    offSuccessed = False;
    resLine = [];
    allLen = len(allLine);
    i = 0;
    while(i < allLen):
        line = allLine[i];

        if line.find(targetStr) != -1 and line.find(expectionStr1) == -1 and line.find(expectionStr2) == -1 and line.find(targetStr2) != -1:
            offSuccessed = True;
            line = line.split("\n")[0];
            line = "<!-- " + line + " -->\n";

        resLine.append(line);
        i = i+1;

    if offSuccessed:
        print "[%s@%s] PROCESSOR OFF SUCCESSED!" % (processor_name, method_name);
    else:
        print "[%s@%s] PROCESSOR OFF FAILED!" % (processor_name, method_name);

    return resLine;

def ReplaceGigNode(outputGigConfigDir, key, value):
    gig_config_file = os.path.join(outputGigConfigDir, key + ".json")
    if not os.path.exists(gig_config_file):
        print "GIG CONFIG FILE {} NOT EXISTS".format(gig_config_file)
        exit(-1)
    
    value_list = value.split(":")
    if len(value_list) != 3:
        print "CONFIG[{}] VALUE[{}] IS INVALID".format(key, value)
        exit(-1)

    with open(gig_config_file, "r") as fin:
        try:
            gig_config = json.load(fin)

            local_node_config = copy.deepcopy(gigNodeTemplate)
            local_node_config["biz_name"] = value_list[0]
            local_node_config["ip"] = value_list[1]
            local_node_config["tcp_port"] = int(value_list[2])
            local_node_config["http_port"] = int(value_list[2])
            gig_config["gig_config"]["subscribe_config"]["local"] = [local_node_config]
            if gig_config["gig_config"]["subscribe_config"].has_key("cm2"):
                del gig_config["gig_config"]["subscribe_config"]["cm2"]
            if gig_config["flow_config"].has_key(key):
                gig_config["flow_config"][key]["flow_control_config"]["begin_degrade_latency"] = 500
                gig_config["flow_config"][key]["flow_control_config"]["full_degrade_latency"] = 1000
                gig_config["flow_config"][key]["timeout"] = 1000
        except Exception as ex:
            print "REPLACE GIG CONFIG [{}] FAILED".format(gig_config_file)
            print ex
            exit(-1)
    with open(gig_config_file, "w") as fout:
        json.dump(gig_config, fout, indent=4)
    print "## REPLACE GIG CONFIG [{}] success".format(gig_config_file)

def Replace(conf):
    inputFileName = conf.get("file", "input");
    outputFileName = conf.get("file", "output");
    inputGigConfigDir = conf.get("file", "input_gig_config_dir")
    outputGigConfigDir = conf.get("file", "output_gig_config_dir")
    
    if not os.path.exists(inputFileName) or not os.path.isfile(inputFileName):
        print "{} not exists or not file".format(inputFileName)
        Usage()
    
    if not os.path.exists(inputGigConfigDir) or not os.path.isdir(inputGigConfigDir):
        print "{} not exists or not dir".format(inputGigConfigDir)
        Usage()

    if os.path.exists(outputGigConfigDir):
        if not os.path.isdir(outputGigConfigDir):
            print "{} exists, but is not dir".format(outputGigConfigDir)
            Usage()

        print "DELETE OLD REPLACED GIG CONFIG DIR {}".format(outputGigConfigDir)
        shutil.rmtree(outputGigConfigDir)
    
    print "COPY GIG CONGIF DIR FROM {} TO {}".format(inputGigConfigDir, outputGigConfigDir)
    shutil.copytree(inputGigConfigDir, outputGigConfigDir)

    print "INPUT_FILE="+inputFileName;
    print "OUTPUT_FILE="+outputFileName;
    print "\n\n*******   START_REPLACE   ********";

    inputFile = open(inputFileName, "r");
    allLine = inputFile.readlines();

    inputFile.close();

    allNode =  conf.items("node_replace");
    print "\n\n#######   REPLACE_NODE    ########"
    for kv in allNode:
        allLine = ReplaceNode(allLine, kv[0], kv[1]);
    
    allGigNode = conf.items("gig_node_replace")
    print "\n\n#######   REPLACE_GIG NODE    ########"
    for kv in allGigNode:
        ReplaceGigNode(outputGigConfigDir, kv[0], kv[1])

    allParam = conf.items("processor_param_replace");
    print "\n\n#######  PROCESSOR_PARAM  ########";
    for kv in allParam:
        allLine = ReplaceProcessorParam(allLine, kv[0], kv[1]);

    allOff = conf.items("off");
    print "\n\n#######   OFF_PROCESSOR    ########"
    for kv in allOff:
        if kv[0] == "processor":
            print kv[1];
            allProcessor = kv[1].split(",");
            for processor in allProcessor:
                pm = processor.split("@")
                if len(pm) < 1:
                    print "error [off] processor@method config: ", processor
                    continue
                processor_name = pm[0]
                method_name = pm[1] if len(pm) > 1 else ""
                allLine = ProcessorOff(allLine, processor_name, method_name);

    res = "".join(allLine);
    outputFile = open(outputFileName, "w");
    print >> outputFile, res;
    outputFile.close();

def Usage():
    print '''
        cp $(AINST__INSTALLROOT)/conf/a4_agg_quarkchat/a4_agg_quarkchat_conf.xml ./
        cp -r $(AINST__INSTALLROOT)/conf/a4_agg_quarkchat/em21 ./
        
        python2.7 replace.py replace.config

        cp ./a4_agg_quarkchat_conf.xml.replace $(AINST__INSTALLROOT)/conf/a4_agg_quarkchat/a4_agg_quarkchat_conf.xml
        cp -r ./em21_replace $(AINST__INSTALLROOT)/conf/a4_agg_quarkchat/a4_agg_quarkchat_conf.xml
    '''
    exit(-1)

if __name__ == "__main__":
    if (len(sys.argv) < 2):
        Usage()

    print(sys.argv[1]);

    cf=ConfigParser.ConfigParser();
    cf.read(sys.argv[1]);
    Replace(cf);
