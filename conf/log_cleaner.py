# encoding=utf8
import os
import sys
import logging
import logging.handlers
import time


workdir = sys.argv[1]
cleanerLogPath = os.path.join(workdir, 'log_cleaner.log')

logs = logging.getLogger('log_cleaner_logger')
logs.setLevel(logging.INFO)
fh = logging.handlers.RotatingFileHandler(
            cleanerLogPath,maxBytes=1024*1024*100,backupCount=5)
fh.setLevel(logging.INFO)
formatter = logging.Formatter(u'[%(asctime)s][%(levelname)s][%(message)s]')
fh.setFormatter(formatter)
logs.addHandler(fh) 

checkList = [
    "usefull_tair.log",
    "nu_cache_tair.log",
    "qc_score_cache_tair.log",
    "intention_model_tair.log",
    "acnn_cache_tair.log",
    "feedback_status_tair.log"
]

while True:
    for name in checkList:
        logPath = os.path.join(workdir, name)
        if not os.path.exists(logPath):
            logs.error("can't find log file : [{}]".format(logPath))
            continue
        fsize = os.path.getsize(logPath)
        fsize = round(fsize / (1024.0 * 1024), 2)
        logs.info("log [{}] size is [{}MB]".format(logPath, fsize))
        if fsize > 1024 * 5:
            logs.info("truncate log [{}]".format(logPath))
            res = os.system(": > {}".format(logPath))
            if res != 0:
                logs.error("truncate log file [{}] fialed. code : [{}]".format(logPath, res))
    time.sleep(60 * 10)
        
