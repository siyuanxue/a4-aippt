#! coding=utf-8
import sys
import os
import json
import time
import uuid
import requests


def _get_bts_bucket():
    buckets = {"llmplan":"2","llmrewrite":"7","llmcachewrite":"1"}
    #bts_hosts = get_ip_from_cm("sm_bts_pub6","em21_5u",datetime.datetime.now().minute)
    #if not bts_hosts:
    #    logger.info("get_bts_ip_list error,request_id:%s",request_id)
    #    return buckets
    #random.shuffle(bts_hosts)
    # bts_hosts = ['11.251.197.40:8956']
    bts_hosts = ['11.180.51.1:8956']
    # bts_hosts = ['sm_bts_pub6.zeus.vipserver:8956']
    try:
        url = "http://"+bts_hosts[0]+"/bts?group=all&cookie=204c646d1290196494f1947360ed03d9%7C%7C%7C1655969830&ip=42.120.75.239&fr=ios&nt=&nw=&query=%E4%BB%80%E4%B9%88%E6%98%AF%E4%B9%B3%E5%A4%B4%E7%8A%B6%E7%98%A4%E5%9B%BE%E7%89%87&from=sm_stress&buck_mode=close_interleaving_and_fix_max"
        res = requests.get(url)
        if res.status_code == 200:
            o = json.loads(res.text)
            if o is not None:
                for bucket in o:
                    buckets[bucket.get("experiment")] = bucket.get("bucket")
    except Exception as e:
        raise
    return buckets


def get_params(query, session_id, bucket):
    root = {}
    root["bucket"] = bucket
    root["version"] = 1
    root["params"] = {}
    root["target"] = "request"
    root["communication"] = {}
    root["communication"]["uid"] = "123"
    root["communication"]["sessionid"] = session_id
    root["communication"]["reqid"] = "123"
    root["communication"]["resid"] = 0
    root["data"] = {}
    root["data"]["message"] = {}
    root["data"]["message"]["query"] = query.decode('utf8')
    root["data"]["message"]["status"] = "init"
    root["error_code"] = 0
    root["error_msg"] = ""
    root["timestamp"] = "1682390791"
    return json.dumps(root, ensure_ascii=False).encode('utf8')


def visit_result(query, server, session_id, bucket, is_multi_retry=True):
    if is_multi_retry:
        retry_times = 3
    else:
        retry_times = 1
    while retry_times > 0:
        html = os.popen('curl "'+server+'" -d \''+get_params(query, session_id, bucket)+'\' 2>/dev/null').read().strip()
        retry_times -= 1
        if not html.strip():
            continue
        try:
            jj = json.loads(html.strip())
            docs = jj['search_doc']
            body = [x["body"] for x in docs[: 3]]
            debug_dict = {"query": query, "raw_text": html.strip(), "query_intent": jj['query_intent'], "qa_intent": str(jj['qa_intent']), 'is_med': str(jj['is_med'])}
            return [json.dumps(x, encoding='utf8', ensure_ascii=False) for x in docs[: 3]], body, debug_dict
        except Exception as e:
            continue
    return ["ERROR"], [""], dict()


def print_diff_html():
    bucket = _get_bts_bucket()
    print('<table width="100%"  cellpadding="0" cellspacing="0" style="table-layout:fixed">')
    cc = 0
    for line in open(sys.argv[2]):
        query = line.strip()
        # local:
        rr_new,body_new,debug_new = visit_result(query, "http://11.180.61.211:20000/web", str(uuid.uuid1()), bucket)
        # ude-pub:
        rr_base,body_base,debug_base = visit_result(query, "http://33.52.40.51:27150/web", str(uuid.uuid1()), bucket)

        query_tag = "DIFF"
        if len(rr_new) > 0 and len(rr_base) == 0:
            query_tag = "ONLY_LEFT_TRIGGER"
        if len(rr_new) == 0 and len(rr_base) > 0:
            query_tag = "ONLY_RIGHT_TRIGGER"

        if len(rr_new) != len(rr_base):
            max_n = max(len(rr_new), len(rr_base))
            for i in range(max_n-len(rr_new)):
                rr_new.append("")
                body_new.append("")
            for i in range(max_n-len(rr_base)):
                rr_base.append("")
                body_base.append("")

        if len(body_base) == 0 and len(body_new) == 0:
            query_tag = "BOTH_NO_TRIGGER"
        elif "[SEP]".join(body_base) == "[SEP]".join(body_new):
            query_tag = "SAME"
        elif "[SEP]".join(sorted(body_base)) == "[SEP]".join(sorted(body_new)):
            query_tag = "SAME_SET"

        cc+=1
        print("<tr><td colspan=2>QUERY_%d:<em style='color:red'>%s</em>\t%s</td></tr>" % (cc, query, query_tag))
        for i in range(len(rr_new)):
            tag = "DIFF"
            if body_new[i] == body_base[i]:
                tag = "SAME"
            print("<tr><td width='50%'>")
            print("<b>"+tag+"</b>  "+rr_new[i].encode('utf8'))
            print("</td><td width='50%'>")
            print("<b>"+tag+"</b>  "+rr_base[i].encode('utf8'))
            print("</td></tr>")
    print("</table>")


def print_trigger_html():
    bucket = _get_bts_bucket()
    print('<table width="100%"  cellpadding="0" cellspacing="0" style="table-layout:fixed">')
    cc = 0
    for line in open(sys.argv[2]):
        query = line.strip()
        # local:
        rr,body,debug_dict = visit_result(query, "http://11.180.61.211:15290/web", str(uuid.uuid1()), bucket)

        cc += 1
        if len(body) == 0:
            query_tag = "NO_TRIGGER"
        else:
            query_tag = "HAS_TRIGGER"
        query_tag += '|' + debug_dict['query_intent'].encode('utf8') + '|' + debug_dict['qa_intent'] + '|' + debug_dict['is_med']
        print("<tr><td colspan=1>QUERY_%d:<em style='color:red'>%s</em>\t%s</td></tr>" % (cc, query, query_tag))
        for i in range(len(rr)):
            print("<tr><td width='50%'>")
            print("<b>"+'docs'+"</b>  "+rr[i].encode('utf8'))
            print("</td></tr>")
    print("</table>")


def print_trigger_json():
    ofile = sys.argv[2] + '.res_json'
    querys = [line.strip() for line in open(sys.argv[2], 'r')]
    bucket = _get_bts_bucket()
    with codecs.open(ofile, 'w', 'utf8') as wf:
        for query in querys:
            # local:
            rr,body,debug_dict = visit_result(query, "http://11.180.61.211:16290/web", str(uuid.uuid1()), bucket)
            line = json.dumps(debug_dict, encoding='utf8', ensure_ascii=False)
            print('succ for query [%s]' % query)
            wf.write(line + '\n')



if __name__ == '__main__':
    handler = sys.argv[1]
    if handler == 'query':
        query = sys.argv[2]
        bucket = _get_bts_bucket()
        #rr,body,debug = visit_result(query, "http://11.180.61.211:15490/web", str(uuid.uuid1()), bucket)
        rr,body,debug = visit_result(query, "http://33.52.19.84:27150/web", str(uuid.uuid1()), bucket)
        for rr in rr_new:
            print(rr)
    elif handler == 'diff_html':
        print_diff_html()
    elif handler == "trigger_html":
        print_trigger_html()
    elif handler == "trigger_json":
        print_trigger_json()
