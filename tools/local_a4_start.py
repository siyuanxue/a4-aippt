#!/usr/bin/env python2.7
# encoding=utf-8

import os
import sys
import json
import shutil
import signal
import subprocess
from string import Template

class SignalHandler(object):
    def __init__(self, popen):
        self.popen = popen

    def __call__(self, signalNumber, frame):
        print 'Received signal:', signalNumber
        if self.popen.poll() is None:
            os.kill(self.popen.pid, signal.SIGINT)

if __name__ == "__main__":
    if len(sys.argv) < 3:
        sys.stderr.write("miss app install_root param\n")
        sys.exit(-1)
    rawAppName = sys.argv[1]
    appName = "a4_"+rawAppName
    installRoot = sys.argv[2]

    configPath = installRoot+'/conf/'+appName
    if not os.path.exists(configPath):
        sys.stderr.write("%s not exist, please ensure this path id installed by RPM\n" % configPath)
        sys.exit(-1)
    binPath = installRoot+'/bin/'
    os.system("ulimit -c unlimited")
    configFileName = appName+"_conf.xml"
    a4ConfigFile = os.path.join(configPath, configFileName)
    a4AlogConfigFile = os.path.join(configPath, "alog.conf")
    dataFileName = rawAppName+"_data.conf"
    dataUpdateName = rawAppName+"_data_update.conf"
    dataFile = os.path.join(configPath,dataFileName)
    dataUpdate = os.path.join(configPath,dataUpdateName)
    content=''
    if not os.path.exists(a4ConfigFile):
        sys.stderr.write("%s not exist, please ensure this path id installed by RPM\n" % a4ConfigFile)
        sys.exit(-1)
    if not os.path.exists(dataFile):
        sys.stderr.write("%s not exist, please ensure this path id installed by RPM\n" % dataFile)
        sys.exit(-1)
    if not os.path.exists(dataUpdate):
        sys.stderr.write("%s not exist, please ensure this path id installed by RPM\n" % dataUpdate)
        sys.exit(-1)
    fp = open(a4ConfigFile,'r')
    content = fp.read()
    fp.close()
    content=content.replace('$(AINST__INSTALLROOT)',installRoot)
    fp = open(a4ConfigFile,'w')
    fp.write(content)
    fp.close()
    fp = open(dataFile,'r')
    content = fp.read()
    fp.close()
    content=content.replace('$(AINST__INSTALLROOT)',installRoot)
    fp = open(dataFile,'w')
    fp.write(content)
    fp.close()
    fp = open(dataUpdate,'r')
    content = fp.read()
    fp.close()
    content=content.replace('$(AINST__INSTALLROOT)',installRoot)
    fp = open(dataUpdate,'w')
    fp.write(content)
    fp.close()
    fp = open(a4AlogConfigFile,'r')
    content = fp.read()
    fp.close()
    content=content.replace('$(AINST__INSTALLROOT)',installRoot)
    fp = open(a4AlogConfigFile,'w')
    fp.write(content)
    fp.close()

    beforeStartSh = os.path.join(binPath,"local_before_start.sh")
    cmdargs=["sh",beforeStartSh,installRoot,dataUpdate]
    cmd=" ".join(cmdargs)
    print cmd
    retCode = os.system(cmd)
    if retCode:
        sys.stderr.write("%s failed\n" % cmd)
    startFileName = "start_"+appName+".sh"
    a4StartFile = os.path.join(binPath, startFileName)
    a4StartCmd = ["sh", a4StartFile, installRoot]
    print " ".join(a4StartCmd)

    popen = subprocess.Popen(args=a4StartCmd, bufsize=-1, shell=False)
    handler = SignalHandler(popen)
    signal.signal(signal.SIGINT, handler)
    signal.signal(signal.SIGUSR1, handler)
    signal.signal(signal.SIGUSR2, handler)
    signal.signal(signal.SIGTERM, handler)

    retCode = popen.wait()
    if retCode:
        sys.stderr.write("%s failed\n" % a4StartCmd)
    sys.exit(retCode)
