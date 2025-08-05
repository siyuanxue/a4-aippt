# encoding=utf8
import lxml.etree as lx
import os
import argparse
import re


KVS = [['$(port)','${global_http_port}'],
        ['$(AINST__INSTALLROOT)','${A4_INSTALL_ROOT}'],
        ['$(amonitor_service_name)', '${kvParam_amonitor_service_name_on_ude}'],
        ['$(amonitor_agent_port)', '10086'],

        ['/lib64/','/usr/local/lib64/'],
        ['/conf/', '/usr/local/conf/'],

        ['/var/','/usr/local/var/'],
        ['/conf/a4_agg_quarkchat/','/conf/'],
        ['/var/a4_agg_quarkchat/','/var/']
       ]

HTTP_DATA_LIST = ['sc_template_data', 'rewrite_query_pool_data', 'query_rewrite_data', 'offline_sc_filter_data', 'source_bodyclean_data', 'query_filter_data', 'host_filter_data', 'host_authority_data', 'query_makeup_data', 'wenchuang_filter_host_data', 'wenchuang_good_host_data', 'gaokao_good_host_data', 'gaokao_filter_host_data', 'college_authority_data', 'zxf_view_data']

def sortFunc(x,y):
    if len(x[0]) > len(y[0]):
        return -1
    elif len(x[0]) < len(y[0]):
        return 1
    return 0

KVS = sorted(KVS, sortFunc)

def text_convert(text):
    ret = text

    for i in range(len(KVS)):
        k = KVS[i][0]
        v = KVS[i][1]
        if ret.find(k) >= 0:
            ret = ret.replace(k, v)

    pattern = re.compile(r'\$[(].*[)]')
    res = pattern.findall(ret)
    for s in res:
        r = s.replace('$(','${kvParam_')
        r = r.replace(')','}')
        ret = ret.replace(s,r)
    return ret

def convert_sub(node):
    # skip Comment
    if node == None or node.tag == lx.Comment:
        return True

    # convert sub nodes
    for child in node:
        if False == convert_sub(child):
            return False

    # convert self
    # <tag attrib_1='xxx' attrib_2='xxx'> text </tag>

    tag = node.tag
    text = node.text
    attrib = node.attrib

    if tag == 'debug_processor_package_dir':
        node.text = '${A4_INSTALL_ROOT}/var/ainst/packages/'
        return True

    if tag == 'workdir':
        node.text = '${A4_INSTALL_ROOT}/usr/local/var/'
        return True

    #if tag == 'include_xml':
    #    node.text = '${A4_INSTALL_ROOT}/usr/local/var/conf/main_app.xml'
    #    return True

    # 多个app更新
    if tag == "app" and not attrib is None and "name" in attrib:
        for c in node:
            if c.tag == 'include_xml':
                old_text = c.text
                prefix_pos = old_text.rfind('/', 0)
                suffix = old_text[prefix_pos+1 : ]
                c.text = '${A4_INSTALL_ROOT}/usr/local/var/conf/' + suffix

    # 需要动态更新的数据
    if tag == "loader" and not attrib is None and "name" in attrib:
        if not "class_name" in attrib or attrib["class_name"] is None or attrib["class_name"] == '':
            return False
        name = attrib["name"]
        for d in HTTP_DATA_LIST:
            if name == d + '_loader':
                attrib["class_name"] = "AggWebHttpDataLoader"
                # check if there is a child named data_dir
                # find <params> tag
                dataDirSetted = False
                for c in node:
                    dataDirChildExist = False
                    if c.tag == 'params':
                        for paramTag in c:
                            if paramTag.tag == 'data_dir':
                                dataDirChildExist = True
                                dataDirSetted = True
                    if not dataDirChildExist:
                        dataDir = lx.SubElement(c, "data_dir")
                        dataDir.text = "${A4_INSTALL_ROOT}/usr/local/var/data/"
                        dataDirSetted = True
                    if dataDirSetted:
                        break
    if tag == "data" and not attrib is None and "path" in attrib:
        if not 'name' in attrib or attrib['name'] is None or attrib['name'] == '':
            return False
        name = attrib['name']
        for d in HTTP_DATA_LIST:
            if name == d:
                kvPath = '${kvParam_data_' + name + "}"
                attrib['path'] = kvPath
                attrib['update_interval'] = '60000' # 1分钟

    if not text is None and len(text) > 0:
        node.text = text_convert(text)

    if not attrib is None and len(attrib) > 0:
        for key in attrib:
            node.attrib[key] = text_convert(attrib[key])

    return True

def convert(path, dst):
    try:
        parser = lx.XMLParser(strip_cdata=False, encoding='utf8')
        tree = lx.parse(path, parser)
    except BaseException as e:
        print("[ERROR] decode xml from [{}] failed. msg : [{}]".format(path, e))
        return False
    print("[INFO] decode success")
    print("[INFO] converting ...")
    if not convert_sub(tree.getroot()):
        print("[ERROR] convert failed")
        return False
    try:
        tree.write(dst, encoding='utf8')
    except BaseException as e:
        print("[ERROR] save to {} failed. msg : [{}]".format(dst, e))
        return False
    print("[INFO] convert xml to tpl success")
    return True

if __name__ == "__main__":
    convert('./ainst/a4_agg_quarkchat_conf.xml','./conf/a4_sc_tmp.xml')  #test
