# -*- coding: UTF-8 -*-
import sys
import json
import subprocess


def get_query_list_from_file(in_file):
    """
        每一行有多个Query，Query之间使用\t分割
    """
    query_list = []
    with open(in_file, mode='r') as fin:
        for line in fin:
            items = line.strip().split('\t')
            if len(items) == 3:     # 3轮对话的数据
                query_list.append(items)
    return query_list

def execute(command):
    """
        Constantly print Subprocess output while process is running
    """
    subprocess.check_call(command, shell=True, stdout=sys.stdout, stderr=subprocess.STDOUT)
    
def curl_commond(query, uid, session_id, req_id):
    cur_dict = {
        'version':1,
        'target': 'request',
        'communication': {
            'uid': uid,
            'session_id': session_id,
            'reqid': req_id,
            'resid':0
        },
        'data': {
            'message':{
                'query': query,
                'status': 'init'
            }
        },
        'bucket': {"llmrewrite":"3","llmcachewrite":"1","llmplan":"2"},
        'error_code':0,
        'error_msg':"",
        'timestamp':'1682390791',
        'params': {'debug': '1'}
    }
    param = json.dumps(cur_dict, ensure_ascii=False).replace(' ', '')
    print(param)
    cmd = 'curl' + ' "http://11.180.61.211:56781/agg"' + " -H 'Accept:text/event-stream'" + ' -d ' + "'" + param + "'" 
    print(cmd)
    return cmd

def request(query_list):
    """
        input: 相同session内的Query。 多轮对话测试
        output: terminal连续打印输出
    """
    # 1.计算session id
    commond = "openssl rand -base64 12"
    output = subprocess.check_output(commond, shell=True)
    session_id = output.decode()
    req_id = output.decode()
    # 2.同一轮内，使用session_id是相同的
    for query in query_list:
        commond = curl_commond(query, 'cuixuange.cxg', session_id, req_id)
        execute(commond)

if __name__ == "__main__":
    # 1.多轮的对话的输入文件
    in_file = 'batch_query_list'
    query_list = get_query_list_from_file(in_file)

    # 2.每一行是一轮对话，目前一轮的有3个测试Query
    for multi_query in query_list:
        request(multi_query)
        # break
