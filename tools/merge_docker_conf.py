#!/usr/bin/env python2 
# -*- coding:utf-8 -*-

import os

# 工具简介：
# 这个工具用来在xonline迭代环境中调试另一个版本的agg_web时使用，
# 这种情况下，我们需要做以下几件事：
# 1. 保证自行编译的版本和迭代环境安装的版本，在依赖库上一致，尤其是agg2的版本
# 2. 将自己编译的liba4_agg_web2.so，传输到迭代环境的lib64目录下
# 3. 将调试版本的xml配置文件(文件1)和迭代环境下安装生成的xml(文件2)进行合并，
#    合并时xml中的涉及环境的部分要和文件2一致，但其余部分和文件1一致，相当于
#    伪造出一个用调试版本在迭代环境可运行的xml配置。
# 本脚本实现的就是为了完成上述第三点的遍历工具，输入两个xml文件，输出合并后的配置
# 将合并后的配置置于conf/a4_agg_quarkchat/目录下，即可启动服务
# 一般需要将本脚本复制到迭代环境的安装目录下使用

INPUT_FILE1 = './a4_agg_quarkchat_conf.xml'                   # 要调试的xml配置，需预先复制到本地
INPUT_FILE2 = 'conf/a4_agg_quarkchat/a4_agg_quarkchat_conf.xml'     # 迭代环境自动部署后默认的配置文件
OUTPUT_FILE = './a4_agg_web_conf_new.xml'               # 合并后的配置，置于conf/a4_agg_quarkchat/后启动服务

def get_substr(buf, prefix, suffix):
    start = buf.find(prefix)
    if start == -1:
        return ''
    end = buf.find(suffix, start + len(prefix))
    if end == -1:
        return ''
    return buf[start+len(prefix):end]

def save_substr_as_kv(content, prefix, suffix, key, data):
    value = get_substr(content, prefix, suffix)
    data[key] = value

def main():
    content = open(INPUT_FILE2, 'r').read()
    data = {}
    save_substr_as_kv(content, '<listen>0.0.0.0:', '</listen>', 'port', data)
    save_substr_as_kv(content, '<amonitor_service_name>', '</amonitor_service_name>', 'amonitor_service_name', data)
    save_substr_as_kv(content, '<amonitor_agent_port>', '</amonitor_agent_port>', 'amonitor_agent_port', data)
    save_substr_as_kv(content, '<workdir>', '/var/a4_agg_quarkchat/</workdir>', 'AINST__INSTALLROOT', data)

    content2 = open(INPUT_FILE1, 'r').read()
    for key in data:
        value = data[key]
        oldstr = '$(%s)' % key
        content2 = content2.replace(oldstr, value)

    fo = open(OUTPUT_FILE, 'w')
    fo.write(content2)
    fo.close()
    print 'New xml conf merged.'

if __name__ == "__main__":
    main()
