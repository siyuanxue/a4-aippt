# -*- encoding: utf-8 -*-

import sys
import os
import json

# 格式: `文件名: [ (em_conf.template中的key,  流控策略名) ]`

cm_need_replaced = {

        }

def parse_template(template_file):
    if not os.path.exists(template_file):
        print >> sys.stderr, "{} not exists".format(template_file)
        return None

    if not os.path.isfile(template_file):
        print >> sys.stderr, "{} is not file".format(template_file)
        return None

    kv_map = {}
    with open(template_file, "r") as fin:
        for line in fin:
            line = line.decode("utf-8").strip()
            if not line or line.startswith("#"):
                continue
            kv = line.split("=")
            if len(kv) != 2:
                print >> sys.stderr, "error line {}".format(line)
                exit(-1)
            key = kv[0].strip()
            value = kv[1].strip()
            if not key or not value:
                print "error line {}".format(line)
                continue
            kv_map[key] = value

    if not kv_map:
        return None
    return kv_map

def process(config_dir, dest_dir, template_file):
    if not os.path.exists(config_dir):
        print >> sys.stderr, "{} not exists".format(config_dir)
        exit(-1)
    if not os.path.isdir(config_dir):
        print >> sys.stderr, "{} is not dir".format(config_dir)
        exit(-1)

    # 检查输出路径是否存在
    if not os.path.exists(dest_dir):
        os.makedirs(dest_dir)

    # 检查配置文件是否都存在
    for key in cm_need_replaced.keys():
        config_file_path = os.path.join(config_dir, key + ".json")
        if not os.path.exists(config_file_path):
            print >> sys.stderr, "{} is not exists".format(config_file_path)
            continue
        if not os.path.isfile(config_file_path):
            print >> sys.stderr, "{} is not file".format(config_file_path)
            exit(-1)

    # 读取template文件，解析kv
    template = parse_template(template_file)
    if not template:
        print >> sys.stderr, "parse template file {} failed".format(template_file)
        exit(-1)

    # 替换zk地址
    cm_zk_spec = template.get("cm_zk_spec")
    cm_zk_path = template.get("cm_zk_path")
    if not cm_zk_spec or not cm_zk_path:
        print >> sys.stderr, "get cm zk addr failed"
        exit(-1)

    for file_name, cm_list in cm_need_replaced.items():
        config_file_path = os.path.join(config_dir, file_name + ".json")
        if not os.path.exists(config_file_path):
            continue
        with open(config_file_path, "r") as fr:
            try:
                config = json.load(fr)
            except Exception as ex:
                print >> sys.stderr, "config file {} is not invalid json".format(config_file_path)
                print >> sys.stderr, ex
                exit(-1)

            # 获取em21_conf.template中相应的cm名
            cm_value_list = []
            for cm_pair in cm_list:
                cm = cm_pair[0]

                cm_value = template.get(cm)
                if not cm_value:
                    print >> sys.stderr, "no invalid cm {}".format(cm)
                    exit(-1)
                if cm_value not in cm_value_list:
                    cm_value_list.append(cm_value)

            # 调整cm
            try:
                config["gig_config"]["subscribe_config"]["cm2"]["cm2_part_no_topo_info"] = cm_value_list
                config["gig_config"]["subscribe_config"]["cm2"]["zk_host"] = cm_zk_spec
                config["gig_config"]["subscribe_config"]["cm2"]["zk_path"] = cm_zk_path
            except Exception as ex:
                print >> sys.stderr, "modify config file {} failed".format(config_file_path)
                print >> sys.stderr, ex
                exit(-1)

            # 调整降级区间
            for cm_pair in cm_list:
                flow_config_name = cm_pair[1]
                try:
                    flow_config = config["flow_config"].get(flow_config_name)
                    if not flow_config:
                        flow_config = config["flow_config"].get("default")

                    if flow_config:
                        flow_config["timeout"] = 1000
                        flow_config["flow_control_config"]["begin_degrade_latency"] = 500
                        flow_config["flow_control_config"]["full_degrade_latency"] = 1000
                except Exception as ex:
                    print >> sys.stderr, "modify config file {} failed".format(config_file_path)
                    print >> sys.stderr, ex
                    exit(-1)


            output_file = os.path.join(dest_dir, file_name + ".json")
            with open(output_file, "w") as fw:
                json.dump(config, fw, indent=4)
            print >> sys.stdout, "gen test config file {} success".format(config_file_path)
    return

def usage():
    print >> sys.stdout, '''
    uasge: python $AINST__INSTALLROOT/conf/a4_agg_quarkchat/dev $AINST__INSTALLROOT/conf/a4_agg_quarkchat/em21_conf.template
    '''
    exit(-1)

if __name__ == "__main__":
    if len(sys.argv) < 3:
        usage()

    config_dir = sys.argv[1]
    dest_dir = sys.argv[2]
    template_file = sys.argv[3]

    process(config_dir, dest_dir, template_file)
