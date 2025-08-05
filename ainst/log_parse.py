#! /usr/bin/env python
# -*- coding:utf-8 -*-

import os, sys
import time
import smtplib
from email.mime.text import MIMEText

reload(sys)
sys.setdefaultencoding('utf8')

class MailSender():
    def __init__(self):
        self.sender = 'sm_sc_sys@alibaba-inc.com'
        self.smtpserver = 'smtp.alibaba-inc.com'
        self.port = 465
        self.username = 'sm_sc_sys@alibaba-inc.com'
        self.password = 'Only_for_send'

    def sendMail(self, receivers, subject, content):
        msg = MIMEText(content,'html','utf-8')
        msg['Subject'] = subject
        msg['From'] = self.sender
        msg['To'] = receivers
        msg['date'] = time.strftime('%a %m/%d/%Y %H:%M %p')
        try:
            smtp = smtplib.SMTP_SSL()
            smtp.connect(self.smtpserver, self.port)
            smtp.login(self.username, self.password)
            rec_vec = receivers.split(';')
            smtp.sendmail(self.sender, rec_vec, msg.as_string())
            smtp.quit()
        except Exception, e:
            print 'sent mail failed'
            return False
        else:
            print 'sent mail ok to %s\n' % receivers
            return True


def parse_line(line):
    if not line.startswith('['):
        return None
    pos = line.find(']')
    if pos < 0:
        return None
    cols = line[1:pos].split(' ')
    if len(cols) != 5:
        return None
    detail = line[pos+1:].strip()
    max_len = 60
    if len(detail) > max_len:
        detail = detail[0:max_len-3] + '...'
    cols.append(detail)
    return cols

def comp(x,y):
  if x[1][0] > y[1][0]:
      return -1
  elif x[1][0] < y[1][0]:
      return 1
  else:
      return 0

def get_tail_time_str(filename):
    lines = os.popen('tail %s' % filename).read().strip().split('\n')
    for i in range(len(lines) - 1, -1, -1):
        line = lines[i]
        ret = parse_line(line)
        if ret == None:
            continue
        day_str, time_str, level, tid, logpoint, detail = ret
        return day_str + ' ' + time_str
    return None

def get_ts_from_str(s):
    if len(s) > 19:
        s = s[0:19]
    return time.mktime(time.strptime(s, '%Y-%m-%d %H:%M:%S'))

def make_mail_subject(log_prefix):
    return '线上服务日志统计[%s]' % log_prefix

def parse_owner(owners):
    cols = owners.split(';')
    results = []
    for col in cols:
        r = col.split(',')
        if len(r) != 2:
            continue
        if r[0] == '':
            continue
        results.append(r)
    return results

def get_owner(owner_list, logpoint):
    for rule in owner_list:
        key, owner = rule
        if logpoint.find(key) >= 0:
            return owner
    return ''

def make_mail_content(start_time_str, end_time_str, total_log_num, error_log, warn_log, warn_log_speed, total_log_speed, owners):
    owner_list = parse_owner(owners)
    content = ''
    content += '单机日志收集时间：%s ~ %s<br>\n' % (start_time_str, end_time_str)
    content += '样机IP: %s<br>\n' % os.popen("ifconfig | grep inet | head -n 1 | awk '{print $2}'").read()
    content += 'IDC: %s<br>\n' % os.popen('hostname -d').read()
    
    content += '<br>\n'
    content += '收集日志条数：%d条<br>\n' % total_log_num
    content += '所有日志输出平均速度：%.1f条/每小时<br>\n' % total_log_speed
    content += 'WARN级日志输出平均速度：%.1f条/每小时<br>\n' % warn_log_speed

    content += '<br>'
    content += 'ERROR日志统计：<br>\n'
    content += '<table border="1">'
    content += '<tr>'
    content += '<td>输出位置</td>'
    content += '<td>占比</td>'
    content += '<td>Owner</td>'
    content += '<td>Detail</td>'
    content += '</tr>\n'

    for l in error_log:
        content += '<tr>'
        content += '<td>' + l[0].replace('build/release64/', '') + '</td>'
        content += '<td>' + str(l[1][0]) + '</td>'
        content += '<td>' + get_owner(owner_list, l[0]) + '</td>'
        content += '<td>' + l[1][1] + '</td>'
        content += '</tr>\n'
    content += '</table><br>\n'

    content += '<br>'
    content += 'WARN日志统计：<br>\n'
    content += '<table border="1">'
    content += '<tr>'
    content += '<td>输出位置</td>'
    content += '<td>占比</td>'
    content += '<td>Owner</td>'
    content += '<td>Detail</td>'
    content += '</tr>\n'

    for l in warn_log:
        content += '<tr>'
        content += '<td>' + l[0].replace('build/release64/', '') + '</td>'
        content += '<td>' + str(l[1][0]) + '</td>'
        content += '<td>' + get_owner(owner_list, l[0]) + '</td>'
        content += '<td>' + l[1][1] + '</td>'
        content += '</tr>\n'
    content += '</table><br>\n'
    return content

def collect_log(path, log_prefix, receiver, owners):
    filename = os.popen('ls %s/%s.log.* -rt | tail -n 1' % (path, log_prefix)).read().strip()
    #print filename
    start_time_str = ''
    end_time_str = get_tail_time_str(filename)
    total_log_num = int(os.popen("wc -l %s | awk '{print $1}'" % filename).read().strip())
    warn_log_num = 0
    error_log = dict()
    warn_log = dict()
    error_log_num = 0
    with open(filename, 'r') as f:
        for line in f:
            ret = parse_line(line)
            if ret == None:
                continue
            day_str, time_str, level, tid, logpoint, detail = ret
            if start_time_str == '':
                start_time_str = day_str + ' ' + time_str
            if level == 'WARN':
                warn_log_num += 1
                if not logpoint in warn_log:
                    warn_log[logpoint] = [1, detail]
                else:
                    warn_log[logpoint][0] += 1
            elif level == 'ERROR':
                error_log_num += 1
                if not logpoint in error_log:
                    error_log[logpoint] = [1, detail]
                else:
                    error_log[logpoint][0] += 1
    error_log_list = list(error_log.items())
    error_log_list.sort(comp)
    for i in range(len(error_log_list)):
        tmp = float(error_log_list[i][1][0]) / error_log_num
        error_log_list[i][1][0] = '%.1f%%' % (tmp * 100)
    warn_log_list = list(warn_log.items())
    warn_log_list.sort(comp)
    for i in range(len(warn_log_list)):
        tmp = float(warn_log_list[i][1][0]) / warn_log_num
        warn_log_list[i][1][0] = '%.1f%%' % (tmp * 100)
    time_len = get_ts_from_str(end_time_str) - get_ts_from_str(start_time_str)
    warn_log_speed = float(warn_log_num) / time_len * 3600
    total_log_speed = float(total_log_num) / time_len * 3600

    content = make_mail_content(start_time_str, end_time_str, total_log_num, error_log_list, warn_log_list, warn_log_speed, total_log_speed, owners)
    if receiver != '':
        # send mail
        subject = make_mail_subject(log_prefix)
        sender = MailSender()
        sender.sendMail(receiver, subject, content)
    else:
        print content

def ut():
    collect_log('logs/a4_agg_quarkchat/', 'agg_web', 'yu.suny1@alibaba-inc.com', '')

def main():
    if len(sys.argv) < 3:
        print 'Usage: python log_parse.py log_path log_prefix [receivers] [owners]'
        print '  log_path: the path where is log file'
        print '  log_prefix: main file name, for example: "agg_web" for agg_web.log'
        print '  receivers: email of receiver, separate by \';\', if there is no receiver, not send mail.'
        print '  owners: code owners. Example: key1,张三;key2;李四'
        print ''
        print 'For example:'
        print '  python log_parse.py /home/admin/online/a4_agg_quarkchat/ agg_web'
        print '  python log_parse.py /home/admin/online/a4_agg_quarkchat/ agg_web xxx@alibaba-inc.com;xxx@alibaba-inc.com'
        print '  python log_parse.py /home/admin/online/a4_agg_quarkchat/ agg_web xxx@alibaba-inc.com;xxx@alibaba-inc.com key1,张三;key2;李四'
        return

    receiver = ''
    if len(sys.argv) > 3:
        receiver = sys.argv[3]
    owners = ''
    if len(sys.argv) > 4:
        owners = sys.argv[4]

    collect_log(sys.argv[1], sys.argv[2], receiver, owners)

if __name__ == "__main__":
    main()
    #ut()
