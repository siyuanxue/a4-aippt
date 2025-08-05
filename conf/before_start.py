# encoding=utf8

import subprocess
import sys
# install pip
subprocess.check_call(['yum', 'install', 'python-pip', '-y'])
subprocess.check_call(['yum', 'install', 'python-devel', 'libxslt', 'libxslt-devel', '-y', '--skip-broken'])
# subprocess.check_call([sys.executable, '-m', 'pip', 'install', 'lxml'])

sys.stdout.write("install lxml success. start convert config\n")

import lxml.etree as lx
import os
import argparse
import re
import socket
import time
import shutil

TEMPLATEFILE_PREFIX = "/a4/usr/local/conf/a4_agg_medchat"
ENV = os.environ.get("RUNNINGENV")
AMONNAME = os.environ.get("AMONNAME")


xmlStack = [] # father node stack
specialConvertMap = {
    "thread_count":[["main","256"]],
    "task_queue_count":[["main","1000"], ["threadpools", "1000"]],
    "amonitor_service_name":["main","a4_agg_web"], # this one will change in different evn
}


def findWorkDir():
    # find ude work dir
    workDirRoot = os.environ.get("HIPPO_APP_WORKDIR")
    if not workDirRoot:
        sys.stderr.write("找不到环境变量 HIPPO_APP_WORKDIR\n")
        sys.exit(-1)
    workDir = None
    for p in os.listdir(workDirRoot):
        p = os.path.join(workDirRoot, p)
        # 在ude上通常是zeus-xxx  本地包通常是workdir
        if ("zeus-" in p or
                p == os.path.join(workDirRoot, 'workdir')) and os.path.isdir(p):
            workDir = p
            break
    if workDir is None:
        sys.stderr.write("找不到日志输出目录 (在{}下寻找zeus-开头的目录/或者本地包的workdir目录)\n".format(workDirRoot))
        sys.exit(-1)
    return workDir

def text_convert(text, kvs):
    ret = text
    if ret == '' or ret == '\n' or ret.strip() == '':
        return ret
    pattern = re.compile(r'\$\{kvParam_.*?\}')
    res = pattern.findall(ret)
    for s in res:
        key = s[len('${kvParam_'):-1]
        if key in kvs:
            ret = ret.replace(s,kvs[key])
    return ret

def undo_ude_modify():
    currentTag = xmlStack[-1]
    if currentTag in specialConvertMap:
        kvs = specialConvertMap[currentTag]
        for kv in kvs:
            k = kv[0]
            if k in xmlStack[:-1]:
                return kv[1]
    return None

def convert_sub(node, kvs):
    # skip Comment
    if node == None or node.tag == lx.Comment:
        return True

    # convert sub nodes
    global xmlStack
    xmlStack.append(node.tag) # put current node tag into stack
    for child in node:
        if False == convert_sub(child, kvs):
            return False

    text = node.text
    attrib = node.attrib

    if not text is None and len(text) > 0:
        node.text = text_convert(text, kvs)

    if not attrib is None and len(attrib) > 0:
        for key in attrib:
            node.attrib[key] = text_convert(attrib[key], kvs)
    
    # ude will modify text for some specific tag
    # we don't want these tag change

    text = undo_ude_modify()
    if not text is None:
        node.text = text
   
    # modify node log path
    logDirRoot = findWorkDir()
    if node.tag == 'log_path' and 'node' in xmlStack:
        if not node.text is None and node.text != "":
            node.text = os.path.join(logDirRoot, node.text.split('/')[-1])

    xmlStack = xmlStack[:-1] # pop stack
    return True

def loadTemplate(path):
    kvMap = {}
    with open(path) as f:
        while True:
            line = f.readline()
            if not line:
                break
            line = line.strip()
            line = line.split('#')[0]
            if line == '':
                continue
            kv = line.split('=')
            if len(kv) != 2:
                continue
            k = kv[0].strip()
            v = kv[1].strip()
            if k == 'local_ip':
                v = socket.gethostbyname(socket.gethostname())
            kvMap[k] = v
    return kvMap


def convert(path):
    if not ENV:
        sys.stderr.write("缺少环境变量 : RUNNINGENV\n")
        sys.exit(-1)
    # load template file
    templateFile = None
    if ENV == "test":
        templateFile = "test_conf.template"
    elif ENV == "pub":
        templateFile = "pub_conf.template"
    elif ENV == "perf":
        templateFile = "perf_conf.template"
    elif ENV == "online_em21":
        templateFile = "em21_conf.template"
    elif ENV == "online_na61":
        templateFile = "na61_conf.template"
    else:
        sys.stderr.write("未知的运行环境 : RUNNINGENV = {}\n".format(ENV))
        sys.exit(-1)

    templateFile = os.path.join(TEMPLATEFILE_PREFIX, templateFile)
    if not os.path.exists(templateFile):
        sys.stderr.write("找不到配置 : {}\n".format(templateFile))
        sys.exit(-1)

    global specialConvertMap
    kvs = loadTemplate(templateFile)
    if ENV == "test" or ENV == "pub" or ENV == "perf":
        kvs['deploy_env'] = 'a4_agg_web/' + ENV
    elif ENV == "online_em21":
        kvs['deploy_env'] = 'a4_agg_web/em21'
    elif ENV == "online_na61":
        kvs['deploy_env'] = 'a4_agg_web/na61'
    
    if not AMONNAME or AMONNAME is "":
        sys.stderr.write("缺少环境变量 : AMONNAME\n")
        sys.exit(-1)

    specialConvertMap['amonitor_service_name'] = [["main", AMONNAME]]

    try:
        parser = lx.XMLParser(strip_cdata=False, encoding='utf8')
        tree = lx.parse(path, parser)
    except BaseException as e:
        sys.stdout.write("[ERROR] decode xml from [{}] failed. msg : [{}]\n".format(path, e))
        sys.exit(-1)
    sys.stdout.write("[INFO] decode success\n")
    sys.stdout.write("[INFO] converting ...\n")
    if not convert_sub(tree.getroot(), kvs):
        sys.stdout.write("[ERROR] convert failed\n")
        sys.exit(-1)
    try:
        tree.write(path, encoding='utf8')
    except BaseException as e:
        sys.stdout.write("[ERROR] save to [{}]. msg : [{}]\n".format(path, e))
        sys.exit(-1)
    sys.stdout.write("[INFO] convert success\n")
    return True


import ConfigParser
libPath = '/a4/usr/local/lib64/site-packages/'
sys.path.append(libPath)
from data_checker.config_parser import Config
from data_checker.logger import Logger
from data_checker.downloader import Downloader
import logging

def downloadData(path):
    if not os.path.exists(path):
        sys.stderr.write("找不到配置 : {}\n".format(path))
        sys.exit(-1)
    parser = ConfigParser.ConfigParser()
    parser.read(path)
    dataUrlList = parser.items('dataUrls')
    if len(dataUrlList) == 0:
        sys.stderr.write('dataUrlList is empty')
        sys.exit(-1)

    logDirRoot = findWorkDir() 
        
    logFormat = '%(asctime)s %(levelname)s %(message)s'
    logPath = os.path.join(logDirRoot, 'data.log')
    sys.stdout.write("数据下载脚本的日志存放路径 : {}\n".format(logPath))

    logLevel = logging.INFO
    installRoot = '/a4/'
    aggConfigFile = '/a4/usr/local/var/conf/a4_sc.xml'
    workDir = '/a4/var'
    dataDir = '/a4/usr/local/var/data/'
    baseUrl = 'http://agg-data.vip.tbsite.net'
    ipListUrl = 'http://agg-data.vip.tbsite.net/a4_agg_web_data/ipList'
    updateMode = 'reload'
    config = Config(logFormat, logPath, logLevel, installRoot, aggConfigFile,
                  workDir, dataDir, baseUrl, ipListUrl, dataUrlList, updateMode)
    logger = Logger(config.logPath, config.logFormat, config.logLevel).getLogger()
    startTime = time.time()
    if not logger:
        sys.stderr.write("创建数据初始化日志失败\n")
        sys.exit(-1)
    socket.setdefaulttimeout(5)
    try:
        sys.stdout.write("开始下载数据\n")
        downloader = Downloader(logger, config)
        if not downloader.download():
            sys.stderr.write("数据下载失败\n")
            sys.exit(-1)
    except Exception, e:
        sys.stderr.write("数据下载失败 : error {}\n".format(e))
        sys.exit(-1)
    endTime = time.time()

    sys.stdout.write("数据下载成功 耗时[{}]\n".format(endTime - startTime))

def replaceAlogConf():
    src = '/a4/usr/local/var/conf/alog_conf'
    dst = '/a4/usr/local/var/conf/alog.conf'
    shutil.copyfile(src, dst) 
    sys.stdout.write("拷贝alog文件成功, form [{}] to [{}]\n".format(src, dst))

def startLogCleaner():
    logDir = findWorkDir();
    cmd = "nohup python /a4/usr/local/var/conf/log_cleaner.py {} &".format(logDir)
    sys.stdout.write(cmd + "\n")
    os.system(cmd)

if __name__ == "__main__":
    convert('/a4/usr/local/var/conf/a4_sc.xml')
    downloadData('/a4/usr/local/conf/a4_agg_medchat/agg_medchat_data.conf')
    replaceAlogConf()
    startLogCleaner()
