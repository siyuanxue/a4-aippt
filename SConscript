# -*- mode: python -*-
import sys, os, os.path, platform, re

Import('env')
env = env.Clone()
env['prjName'] = 'a4_agg_quarkchat'
env['version_num'] = '1.7.1'
env['release_num'] = '0'

env.Append(ACSUBST = {'BUILDDIR': env['RELATIVE_BUILD_DIR']})
env.Append(ACSUBST = {'TOP_BUILDDIR': env['RELATIVE_BUILD_DIR']})
env.Append(ACSUBST = {'TOP_SRCDIR': env['TOP_DIR']})
env.Append(ACSUBST = {'abs_top_srcdir': env['TOP_DIR']})

loggerconf = env['TOP_DIR'] + '/test/logger.conf'
env.Append(ACSUBST = {'DOTEST_LOGGER_CONF': loggerconf})
testdata_dir = env['TOP_DIR'] + '/agg_web2_testdata/'
env.Append(ACSUBST = {'TESTDATA_DIR': testdata_dir})

env['agg2_base_libs'] = [
    'agg2'
    ]

env['agg_base_libs'] = [
    'agg_common',
    'agg_wrapper',
    'agg_util'
]
env['common_libs'] = [
    'NlpToolkit',
    'm9',
    're2',
    'jsoncpp',
    'sm_gig_util',
    'multi_call',
    'cm_sub',
    'a4_config',
    'a4_common',
    'a4_util',
    'a4_tinyxml',
    'a4_data_engine',
    'a4_module',
    'a4_monitor',
    'a4_wrapper',
    'a4_upstream',
    'a4_network',
    'a4_processor',
    'a4_router',
    'cygnet',
    'memcached',
    'messer_utils',
    'strace_client',
    'snappy',
    'alog',
    'amonitor_client',
    'arpc',
    'pthread',
    'protobuf',
    'autil',
    'dl',
    ':libboost_serialization.so.1.66.0',
    'ufs_client',
    'sutil'
    ]


env['all_unittest_libs'] = env['common_libs'] + env['agg2_base_libs'] + [
    'umonitor_metrics_client',
    'umonitor_trace_client',
    'a4_agg_web',
    'cppunit'
]


env.aACSubst(target = ['test/test.h'],
             source = ['test/test.h.in'])

env.aDirs('test')
env.aDirs('agg_web2')
