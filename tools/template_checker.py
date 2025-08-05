#!/usr/bin/env python2 
# -*- coding:utf-8 -*-

import os

# 工具简介：
# 服务启动时如果我们遇到a4启动失败，但agg_web.log又没有任何输出
# 一般是template配置中缺失一些kv项，使xml配置中的template变量不能全部正确替换
# 进而导致xml解析失败，这时我们需要检查template中缺少哪些kv
# 这时就可以使用此脚本进行检查，方便的找到缺失的kv项

TEM_FILE = 'conf/a4_agg_quarkchat/em21_conf.template'     # 要比对的template文件，一般为em21机房配置
XML_FILE = 'conf/a4_agg_quarkchat/a4_agg_quarkchat_conf.xml'    # xml配置文件

def load_template_keys():
    lines = open(TEM_FILE, 'r').readlines()
    keys = set()
    for line in lines:
        l = line.strip()
        if l.startswith('#'):
            continue
        pos = l.find('=')
        if pos < 0:
            continue
        key = l[0:pos]
        keys.add(key)
    return keys

def load_xml_params():
    params = set()
    content = open(XML_FILE, 'r').read()
    pos = 0
    while True:
        start = content.find('$(', pos)
        if start < 0:
            break
        end = content.find(')', start)
        if end < 0:
            print 'xml format error: %s...' % content[start:start+20]
            break
        param = content[start+2:end]
        params.add(param)
        pos = end + 1
    return params

def main():
    keys = load_template_keys()
    params = load_xml_params()

    for param in params:
        if not param in keys:
            print 'Not found param: %s' % param

    print 'Check done.'

if __name__ == "__main__":
    main()
