#!/usr/bin/env python2 

import os
import subprocess 
import argparse

def error(msg=""):  
  print "error: %s" % msg
  exit(1)

def get_root_path(anchor=".clang-format"):
  path = os.path.abspath(__file__)
  while True:
    path = os.path.dirname(path)
    if (os.path.exists(path + "/" + anchor)):
      return path
    if (path == "/"): 
      error("%s not found" % anchor)

def run():
  file_types = ["*.cc", "*.c", ".cpp", "*.hpp", "*.h"]
  sub_folders = ["agg2"]
  format_file = ".clang-format"
  root = get_root_path(format_file)
  print "format in [%s] with [%s]" % (", ".join(sub_folders), ", ".join(file_types)) 
  for folder in sub_folders:
    for file_type in file_types:
      cmd = "find %s/%s -name '%s'| xargs clang-format -i 2>/dev/null" %(
              root, folder, file_type) 
      os.system(cmd)

if __name__ == "__main__":
  run()
