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
    appName = "a4_"+sys.argv[1]
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
    
    a4Tool = os.path.join(binPath, "a4_agg_tool")
    a4StartCmd = ["python2.7", a4Tool, "stop","-c",a4ConfigFile]
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
