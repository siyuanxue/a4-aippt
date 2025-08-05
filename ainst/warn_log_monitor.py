#! /usr/bin/env python
# -*- coding:utf-8 -*-

import os, sys
import time
import socket, time

reload(sys)
sys.setdefaultencoding('utf8')

class XssSender():
    def __init__(self):
        self.prefix = "test.diff_tools.agg"
        self.upload_ip = '11.251.193.21'
        self.upload_port = 2003

    def send(self, metric_name, value):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((self.upload_ip, self.upload_port))
        s.settimeout(1)
        ts = int(time.time())
        data = "{}.{} {} {}\n".format(self.prefix, metric_name, value, ts)
        s.sendall(data)
        s.close()

def parse_line(line):
    if not line.startswith('['):
        return None
    pos = line.find(']')
    if pos < 0:
        return None
    cols = line[1:pos].split(' ')
    if len(cols) != 5:
        return None
    return cols

def get_ts_from_str(s):
    if len(s) > 19:
        s = s[0:19]
    return time.mktime(time.strptime(s, '%Y-%m-%d %H:%M:%S'))

def count_in_file(logfile, oldest_ts):
    total_log_num = 0
    warn_log_num = 0
    need_more = True
    in_count = False
    with open(logfile, 'r') as f:
        for line in f:
            ret = parse_line(line)
            if ret == None:
                continue
            day_str, time_str, level, tid, logpoint = ret
            if not in_count:
                ts = get_ts_from_str(day_str + ' ' + time_str)
                if ts < oldest_ts:
                    need_more = False
                    continue
                else:
                    in_count = True
            
            total_log_num += 1
            if level == 'WARN':
                warn_log_num += 1
    return total_log_num, warn_log_num, need_more

def collect_log(path, log_prefix, count_min):
    now = int(time.time())
    start_time = now - count_min * 60
    cur_file = '%s/%s.log' % (path, log_prefix)
    total_log_num, warn_log_num, need_more = count_in_file(cur_file, start_time)
    
    files = os.popen('ls %s/%s.log.* -t' % (path, log_prefix)).read().strip().split('\n')
    fileindex = 0
    while need_more:
        if fileindex >= len(files):
            break
        f = files[fileindex]
        add_total_log_num, add_warn_log_num, need_more = count_in_file(f, start_time)
        warn_log_num += add_warn_log_num
        total_log_num += add_total_log_num
        fileindex += 1
    print 'WARN log number in last %d minute: %d.' % (count_min, warn_log_num)

    total_log_speed = float(total_log_num) / count_min
    warn_log_speed = float(warn_log_num) / count_min
    print 'Total log speed: %f per minute.' % total_log_speed
    print 'WARN log speed: %f per minute.' % warn_log_speed

    # send to xss
    if 1:
      sender = XssSender()
      sender.send('log_speed.all', total_log_speed)
      sender.send('log_speed.warn', warn_log_speed)

def ut():
    collect_log('logs/a4_agg_quarkchat/', 'agg_web', 10)

def main():
    if len(sys.argv) < 4:
        print 'Usage: python warn_log_parse.py log_path log_prefix count_min'
        print '  log_path: the path where is log file'
        print '  log_prefix: main file name, for example: "agg_web" for agg_web.log'
        print '  count_min: minutes to count WARN log'
        print ''
        print 'For example:'
        print '  python warn_log_parse.py /home/admin/online/a4_agg_quarkchat/ agg_web 10'
        return

    collect_log(sys.argv[1], sys.argv[2], int(sys.argv[3]))

if __name__ == "__main__":
    main()
    #ut()
