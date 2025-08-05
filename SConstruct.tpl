# -*- mode: python -*-
import sys, os, os.path, platform, re, time, subprocess

CURRENT_PATH = Dir('.').srcnode().abspath
PARENT_PATH = Dir('..').srcnode().abspath
if CURRENT_PATH.find('build/release64/rpm_build/BUILD') > 0:
    PARENT_PATH = Dir(CURRENT_PATH[:CURRENT_PATH.find('build/release64/rpm_build/BUILD')] + '/../').srcnode().abspath
else:
    PARENT_PATH = Dir('..').srcnode().abspath

print CURRENT_PATH, PARENT_PATH

os.environ['build_options'] = '''
enable_strip = 'False'
heapchecktype = 'none'
'''

USE_CLANG = False
MEM_CHECK_BY_ADDRESS_SANITIZE = False
if ARGUMENTS.get("compiler", "") == "clang":
    print "compiler is clang"
    USE_CLANG = True
    if ARGUMENTS.get("mem_check", ""):
        MEM_CHECK_BY_ADDRESS_SANITIZE = True
        print "mem check by address sanitize"
    else:
        print "do not mem check by address sanitize"
else:
    print "compiler is g++"

if True:
    if USE_CLANG:
        print "use clang"
        env = Environment(tools=['default', 'ascons'],
                        CXXCOMSTR = "Compiling static object $TARGET",
                        SHCXXCOMSTR = "Compiling shared object $TARGET",
                        LINKCOMSTR = "Linking $TARGET",
                        SHLINKCOMSTR = "Linking shared $TARGET",
                        CXX = '%s/tools/ccache /usr/bin/clang++' % (CURRENT_PATH))
    else:
        print "use g++"
        env = Environment(tools=['default', 'ascons'],
                        CXXCOMSTR = "Compiling static object $TARGET",
                        SHCXXCOMSTR = "Compiling shared object $TARGET",
                        LINKCOMSTR = "Linking $TARGET",
                        SHLINKCOMSTR = "Linking shared $TARGET",
                        CXX = '%s/tools/ccache g++' % (CURRENT_PATH))
else:
    env = Environment(tools=['default', 'ascons'],
                    CXX = '%s/tools/ccache g++' % (CURRENT_PATH))

# make .tpl for ude
import tools.xml2tpl
import tools.alog2tpl
if not tools.xml2tpl.convert('./ainst/a4_agg_quarkchat_conf.xml', './a4_conf/a4_agg_quarkchat/a4_sc.xml.tpl'):
    exit(1)
if not tools.alog2tpl.convert('./ainst/alog.conf','./a4_conf/a4_agg_quarkchat/alog_conf'):
    exit(1)
###############################
# for ude compile
###############################
if os.getenv('CCACHE_PREFIX'):
    env['ENV']['CCACHE_PREFIX'] = os.getenv('CCACHE_PREFIX')
    env['ENV']['DISTCC_HOSTS'] = '"%s"'%os.getenv('DISTCC_HOSTS')
    env['ENV']['DISTCC_LOG'] = os.getenv('DISTCC_LOG')
    env['ENV']['DISTCC_DIR'] = os.getenv('DISTCC_DIR')

env['ENV']['CCACHE_DIR'] = '%s/.ccache' % PARENT_PATH
env['ENV']['CCACHE_BASEDIR'] = '%s' % PARENT_PATH
env['ENV']['CCACHE_NOHASHDIR'] = 'off'
env['_CURRENT_PATH'] = CURRENT_PATH
env['_PARENT_PATH'] = PARENT_PATH

if USE_CLANG and MEM_CHECK_BY_ADDRESS_SANITIZE:
    env.Append(CXXFLAGS = '-Wno-unused-function -Wno-strict-overflow -fno-strict-overflow -fdebug-prefix-map=%s=.'%CURRENT_PATH)
    env.Append(CXXFLAGS = "-O1 -fsanitize=address -fno-omit-frame-pointer -std=c++11 -DMEM_CHECK_BY_ADDRESS_SANITIZE")
    env.Append(LINKFLAGS = "-fsanitize=address -fno-omit-frame-pointer")
else:
    env.Append(CXXFLAGS = '-std=c++11')
    env.Append(CXXFLAGS = '-Wno-unused-function -Wno-unused-variable -Wno-strict-overflow -Wno-unused-but-set-variable -fno-strict-overflow -fdebug-prefix-map=%s=.'%CURRENT_PATH)
    
env.SConscript(
    dirs='.',
    exports='env',
    variant_dir=env['BUILD_DIR'],
    duplicate=1
    )

print(subprocess.check_output('env CCACHE_DIR=%s/.ccache %s/tools/ccache -s' % (PARENT_PATH, CURRENT_PATH), shell=True))

env.aInstall(env['install_libdir'], env['LIB_DIR']+'/*', False)
env.aInstall(env['install_prefix'] + '/bin', 'tools/a4_agg_tool')
env.aInstall(env['install_prefix'] + '/bin', 'ainst/log_parse.py')
env.aInstall(env['install_prefix'] + '/bin', 'ainst/warn_log_monitor.py')

env.aInstall(env['install_prefix'] + '/var/a4_conf', 'a4_conf/a4_biz_start.sh')
env.aInstall(env['install_prefix'] + '/var/data', 'data')

env.aInstall(env['install_prefix'] + '/conf/a4_agg_quarkchat', 'ainst/a4_agg_quarkchat_conf.xml')
env.aInstall(env['install_prefix'] + '/conf/a4_agg_quarkchat', 'ainst/alog.conf')
env.aInstall(env['install_prefix'] + '/conf/a4_agg_quarkchat', 'ainst/agg_quarkchat_data.conf')
env.aInstall(env['install_prefix'] + '/conf/a4_agg_quarkchat', 'ainst/agg_quarkchat_data_update.conf')
env.aInstall(env['install_prefix'] + '/conf/a4_agg_quarkchat', 'ainst/em21_conf.template')
env.aInstall(env['install_prefix'] + '/conf/a4_agg_quarkchat', 'ainst/na61_conf.template')
env.aInstall(env['install_prefix'] + '/conf/a4_agg_quarkchat', 'ainst/test_conf.template')
env.aInstall(env['install_prefix'] + '/conf/a4_agg_quarkchat', 'ainst/pub_conf.template')
env.aInstall(env['install_prefix'] + '/conf/a4_agg_quarkchat', 'ainst/perf_conf.template')
env.aInstall(env['install_prefix'] + '/conf/a4_agg_quarkchat/em21/', 'ainst/em21/*.json')
env.aInstall(env['install_prefix'] + '/conf/a4_agg_quarkchat/na61/', 'ainst/na61/*.json')
env.aInstall(env['install_prefix'] + '/conf/a4_agg_quarkchat/test/', 'ainst/test/*.json')
env.aInstall(env['install_prefix'] + '/conf/a4_agg_quarkchat/pub/', 'ainst/pub/*.json')
env.aInstall(env['install_prefix'] + '/conf/a4_agg_quarkchat', 'ainst/pre_process_dev_gig_config.py')
env.aInstall(env['install_prefix'] + '/conf/a4_agg_quarkchat', 'ainst/crontab.conf')
env.aInstall(env['install_prefix'] + '/conf/', 'ainst/domain_suffix.conf')
env.aInstall(env['install_prefix'] + '/bin', 'ainst/start_a4_agg_quarkchat.sh')
env.aInstall(env['install_prefix'] + '/bin', 'ainst/reload_a4_agg_quarkchat.sh')
env.aInstall(env['install_prefix'] + '/ainst', 'ainst/a4_agg_quarkchat.aicf')
env.aInstallEmptyDir(env['install_prefix'] + '/logs/a4_agg_quarkchat')
env.aInstallEmptyDir(env['install_prefix'] + '/var/a4_agg_quarkchat/')
env.aInstallEmptyDir(env['install_prefix'] + '/var/a4_agg_quarkchat/data/')
env.aInstall(env['install_prefix'] + '/var/a4_agg_quarkchat/html', 'tools/html/status')
env.aInstall(env['install_prefix'] + '/var/a4_agg_quarkchat/html', 'tools/html/context_dump_keys')
env.aInstall(env['install_prefix'] + '/var/a4_agg_quarkchat/html', 'tools/html/sub_context_dump_keys')
env.aInstallEmptyDir(env['install_prefix'] + '/var/a4_agg_quarkchat/html/objects_dump_dir/')
env.aInstall(env['install_prefix'] + '/var/a4_conf/a4_agg_quarkchat', 'a4_conf/a4_agg_quarkchat')

releaseStr = 'nightly_' + time.strftime("%Y%m%d%H%M", time.localtime())
if env['mode'] == 'release':
    releaseStr = 'rc_1'


env.aRpm(FILES = [env['install_libdir'],
                  env['install_bindir'],
                  env['install_prefix']+'/conf',
                  env['install_prefix']+'/ainst',
                  env['install_prefix']+'/logs',
                  env['install_prefix']+'/var'],
         NAME = '${PROCESSOR_NAME}',
         VERSION = '${VERSION}',
         RELEASE = releaseStr,
         AUTOREQPROV = 'no',
         REQUIRES = [
                'resys_google_re2 = 0.2.0',
                'agg2 = 0.3.56',
                'fslib-framework = 0.6.*',
                'sutil = 0.0.5',
                'm9codec-devel = 0.1.0' ],
         GROUP = 'Development/sdk',
         SUMMARY = 'this is the sdk of a4_agg_quarkchat')


