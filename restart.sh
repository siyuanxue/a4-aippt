LD_LIBRARY_PATH=./_external/usr/local/lib64/:./_external/usr/local/lib/:./_external/usr/lib64/ ./_external/usr/local/bin/a4_tool stop -c ./conf/a4_agg_medchat_conf.xml
rm -rf *.log*
LD_LIBRARY_PATH=./_external/usr/local/lib64/:./_external/usr/local/lib/:./_external/usr/lib64/ ./_external/usr/local/bin/a4_tool start -c ./conf/a4_agg_medchat_conf.xml -l ./conf/alog.conf -t ./conf/a4_agg_medchat/hz_conf.template
