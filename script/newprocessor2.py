#!/bin/env python

import sys
import os
import re
rootName = "agg_web"
prjName = 'agg_web2'
def getNamespace(path):
    ls = path.split("/")
    n = len(ls)
    while n >= 1 and len(ls[n-1]) == 0:
        n = n - 1
    if n < 1:
        return None
    if ls[n-1] == 'test':
        return ls[n-2]
    else:
        return ls[n-1]

def getHeaderPath(path):
    ls = path.split("/")
    n = len(ls)
    while n >= 1 and len(ls[n-1]) == 0:
        n = n - 1
    if n < 1:
        return None
    n = n-1;
    if ls[n] == 'test':
        n = n-1
    
    dirCount = n;
    while n >= 1 and ls[n] <> prjName:
        n = n - 1
    n = n + 1
    path = ""
    if n <= dirCount:
        path = ls[n]
    while n < dirCount :
        path =path + "/"+ls[n+1]
        n = n+1
    return path

def getClassName(path):
        LLL = path.split('_')
        for i in range(len(LLL)): LLL[i] = LLL[i][0].upper() + LLL[i][1:]
        return ''.join(LLL)


def createClassH(path, classname):
    ns = getNamespace(path)
    if not ns:
        return False
    
    context = ""
    context = context + "#ifndef " + rootName.upper() + "_" + ns.upper() + "_" +  classname.upper() + "_H" + "\n"
    context = context + "#define " + rootName.upper() + "_" + ns.upper() + "_" + classname.upper() + "_H" + "\n"
    context = context + "" + "\n"
    context = context + "#include <" + prjName + "/common/common.h>" + "\n"
    context = context + "#include <agg2/processor/base_processor.h>" + "\n"
    context = context + "" + "\n"
    context = context + "AGG_BEGIN_NAMESPACE(" + prjName + ");" + "\n"
    context = context + "" + "\n"
    context = context + "class " + getClassName(classname) + ": public BaseProcessor" + "\n"
    context = context + "{" + "\n"
    context = context + "public:" + "\n"
    context = context + "    "  + getClassName(classname) + "();" + "\n"
    context = context + "    virtual ~" + getClassName(classname) + "();" + "\n"
    context = context + "\n"
    context = context + "public:" + "\n"
    context = context + "    virtual bool DoInit(const String2String & params) override;" + "\n"
    context = context + "    virtual void DoRegisterMetric(AmonitorRegister *monitorRegister) override;" + "\n"
    context = context + "    virtual BaseProcessor* DoClone() override;" + "\n"
    context = context + "    virtual void DoDestroy() override;" + "\n"
    context = context + "\n"
    context = context + "public:" + "\n"
    context = context + "    // register UDF MethodFunc" + "\n" 
    context = context + "    virtual bool RegisterMethods() override;" + "\n"
    context = context + "\n"
    context = context + "public:" + "\n"
    context = context + "    // register UDF actor" + "\n"
    context = context + "    virtual bool RegisterActors() override;" + "\n"
    context = context + "public:" + "\n"
    context = context + "    // build in MethodFunc" + "\n"
    context = context + "    virtual int DoProcess(AggContext * aggContext) override;" + "\n"
    context = context + "" + "\n"
    context = context + "private:" + "\n"
    context = context + "    AGG_LOG_DECLARE();" + "\n"
    context = context + "\n"
    context = context + "private:" + "\n"
    context = context + "    DECLARE_SERIALIZATION_FUNC();" + "\n"
    context = context + "};" + "\n"
    context = context + "" + "\n"
    context = context + "AGG_TYPEDEF_PTR(" + getClassName(classname) + ");" + "\n\n"
    context = context + "AGG_END_NAMESPACE(" + prjName + ");" + "\n"
    context = context + "BOOST_CLASS_EXPORT_KEY(AGG_NS(" + prjName + ")::" + getClassName(classname) + ");" + "\n"
    context = context + "" + "\n"
    context = context + "#endif //" + rootName.upper() + "_" + ns.upper() + "_" + classname.upper() + "_H" + "\n"
    f = file(path + "/" + classname + ".h", "w")
    f.write(context)
    f.close()
    return True

def createClassCPP(path, classname):
    ns = getNamespace(path)
    hp = getHeaderPath(path)
    if not ns:
        return False
    context = ""
    if (not re.match(".*/test", path)):
        context = context + "#include <" + prjName + "/" + hp + "/" + classname + ".h>" + "\n"
    else:
        context = context + "#include <" + prjName + "/" + hp + "/test/" + classname + ".h>" + "\n"
    context = context + "" + "\n"
    context = context + "AGG_BEGIN_NAMESPACE(" + prjName + ");" + "\n"
    context = context + "AGG_LOG_SETUP(" + prjName  + ", " + getClassName(classname) + ");" + "\n\n"
    context = context + "// serialize class object" + "\n"
    context = context + "BEGIN_DEFINE_SERIALIZATION_FUNC(" + getClassName(classname) + ")" + "\n"
    context = context + "    // Example: serialize mField" + "\n"
    context = context + "    // ar & BOOST_SERIALIZATION_NVP(mField);" + "\n"
    context = context + "END_DEFINE_SERIALIZATION_FUNC(" + getClassName(classname) + ")" + "\n"
    context = context + "" + "\n"
    context = context + getClassName(classname) + "::" + getClassName(classname) + "()" + "\n"
    context = context + "{" + "\n"
    context = context + "}" + "\n"
    context = context + "\n"
    context = context + getClassName(classname) + "::~" + getClassName(classname) + "()" + "\n"
    context = context + "{" + "\n"
    context = context + "}" + "\n"
    context = context + "\n"
    context = context + "bool " + getClassName(classname) + "::DoInit(const String2String & params)" + "\n"
    context = context + "{" + "\n"
    context = context + "    return true;" + "\n"
    context = context + "}" + "\n"
    context = context + "\n"
    context = context + "void " + getClassName(classname) + "::DoRegisterMetric(AmonitorRegister * monitorRegister)" + "\n"
    context = context + "{" + "\n"
    context = context + "}" + "\n"
    context = context + "\n"
    context = context + "BaseProcessor* " + getClassName(classname) + "::DoClone()" + "\n"
    context = context + "{" + "\n"
    context = context + "    return new " + getClassName(classname) + "(*this);" + "\n"
    context = context + "}" + "\n"
    context = context + "\n"
    context = context + "void " + getClassName(classname) + "::DoDestroy()" + "\n"
    context = context + "{" + "\n"
    context = context + "    delete this;" + "\n"
    context = context + "}" + "\n"
    context = context + "\n"
    context = context + "bool " + getClassName(classname) + "::RegisterMethods()" + "\n"
    context = context + "{" + "\n"
    context = context + "    return true;" + "\n"
    context = context + "}" + "\n"
    context = context + "\n"
    context = context + "bool " + getClassName(classname) + "::RegisterActors()" + "\n"
    context = context + "{" + "\n"
    context = context + "    return true;" + "\n"
    context = context + "}" + "\n"
    context = context + "\n"
    context = context + "int " + getClassName(classname) + "::DoProcess(AggContext * aggContext)" + "\n"
    context = context + "{" + "\n"
    context = context + "    return PR_SUCCESS;" + "\n"
    context = context + "}" + "\n"
    context = context + "\n"
    context = context + "\n"
    context = context + "AGG_END_NAMESPACE(" + prjName + ");" + "\n"
    context = context + "BOOST_CLASS_EXPORT_IMPLEMENT(AGG_NS(" + prjName + ")::" + getClassName(classname) + ");" + "\n"
    context = context + "" + "\n"
    f = file(path + "/" + classname + ".cpp", "w")
    f.write(context)
    f.close()
    return True

def createClass(path, classname):
    ret = 0
    if os.path.exists(path + "/" + classname + ".h"):
        print "header file " + path + "/" + classname + ".h exists!" 
    else:
        if createClassH(path, classname):
            print "create header file " + path + "/" + classname + ".h succeed."
            ret = ret + 1
        else:
            print "create header file " + path + "/" + classname + ".h failed."
    if os.path.exists(path + "/" + classname + ".cpp"):
        print "source file " + path + "/" + classname + ".cpp exists!" 
    else:
        if createClassCPP(path, classname):
            print "create source file " + path + "/" + classname + ".cpp succeed."
            ret = ret + 1
        else:
            print "create source file " + path + "/" + classname + ".cpp failed."
    return ret

def insertMakefile(makefile, name):
    TIP = "source_list"
    TIPLEN = len(TIP)
    TIP2 = "EXTRA_DIST"
    TIP2LEN = len(TIP2)
    fin = file(makefile, "r")
    fbak = file(makefile + ".bak", "w")
    context = fin.read()
    fin.close()
    fbak.write(context)
    fbak.close()
    fout = file(makefile, "w")
    fin = file(makefile + ".bak", "r")
    while True:
        l = fin.readline()
        if not l:
            break
        if len(l) > TIPLEN and l[:TIPLEN] == TIP:
            l = l + "    " + name + ".cpp\\\n"
#      n = l.find("=")
#      if n != -1:
#    l = l[:n+1] + " " + name + ".cpp " + l[n+1:]
            if len(l) > TIP2LEN and l[:TIP2LEN] == TIP2:
                l = l + "    " + name + ".h\\\n"
#      n = l.find("=")
#      if n != -1:
#    l = l[:n+1] + " " + name + ".h " + l[n+1:]
                fout.write(l)
                fin.close()
                fout.close()
                print name + " has been inserted into " + makefile


def insertSConscript(scon_name, name):
    hasFindSource = False
    hasWritten = False
    os.system("mv " + scon_name + " " + scon_name + ".bak")
    TIP = ur"_sources\s*=\s*\["
    fin = file(scon_name + ".bak", "r");
    fout = file(scon_name, "w");
    while True:
        l = fin.readline()
        if not l:
            break;
        if not hasFindSource and not hasWritten:
            if re.search(TIP, l):
                hasFindSource = True
        elif not hasWritten and l.find(']') != -1:
            l = "    \'" + name + ".cpp\',\n" + l
            hasFindSource = False
            hasWritten = True
        fout.write(l)
    fin.close()
    fout.close()
    print name + " has been inserted into " + scon_name
    os.system("rm " + scon_name + ".bak")
    
if __name__ == "__main__":
    if len(sys.argv) != 2:
        print >> sys.stderr, "Usage:", sys.argv[0], "<classname>"
        sys.exit(1)
    path = os.getcwd()
    createClass(path, sys.argv[1])
#        insertSConscript(path + "/SConscript", sys.argv[1])
#           insertMakefile(path + "/Makefile.am", sys.argv[1])
