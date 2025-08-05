#! /bin/env python
# -*- coding: utf-8 -*-

import os, sys
import time
reload(sys)
sys.setdefaultencoding('utf8')

# 过滤新绝影的数据，只保留生效日期以内的

def main():
    input_file = sys.argv[1]
    output_file = sys.argv[2]

    now = int(time.time())

    fo = open(output_file+ '.tmp', 'w')
    lines = open(input_file, 'r').readlines()
    for line in lines:
        cols = line.split('\t')
        if len(cols) < 6:
            continue
        start = int(cols[3])
        end = int(cols[4])
        if start != 0 and now < start:
            continue
        if end != 0 and now > end:
            continue

        output_line = cols[0] + '\t' + cols[1] + '\t' + cols[2] + '\n'
        fo.write(output_line)
    fo.close()

    if os.path.exists(output_file):
        os.remove(output_file)
    os.rename(output_file+ '.tmp', output_file)

if __name__ == "__main__":
    main()
