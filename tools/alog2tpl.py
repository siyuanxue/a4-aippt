# encoding=utf8
import os

def convert(path,dst):
    if not os.path.exists(path):
        print("[ERROR] convert alog conf to tpl failed. can't open [{}]".format(path))
        return False
    f = open(path)
    lines = []
    while True:
        line = f.readline()
        if not line:
            break
        if line.find('fileName') == -1:
            lines.append(line)
            continue
        tokens = line.split('=')
        if len(tokens) != 2:
            print("[ERROR] decode failed. [{}]".format(line))
        path = tokens[1]
        index = path.rfind('/')
        if index != -1:
            path = path[index+1:]
        # path = '/wings/_zeus_a4_agg_web_logs_/' + path
        line = tokens[0] + "=" + path
        lines.append(line)
    f.close()

    f = open(dst, 'w')
    for l in lines:
        f.write(l)
    f.close()

    print('[INFO] convert alog config to tpl success')
    return True

if __name__ == "__main__":
    convert('./ainst/alog.conf','./conf/alog_conf')
