#!/bin/bash
#set -x

#echo "Command Line Params: $*"

Usage() {
    echo "Usage: $0 AINST_INSTALLROOT [OPT_LIB_ASAN_PATH] [-j Num] [-d] [-s OPT_LIB_ASAN_PATH] [-h]"
    echo ""
    echo "options:"
    echo "  -j: concurrent num thread for download, default 1"
    echo "      set 0, skip download data, will speedup 3s, make sure you have download all data"
    echo "  -s: given mem check preload library path"
    echo "  -h: show this message"
    echo "  -d: dry run"
    echo ""
    echo "  -- attention --"
    echo "     If you use ainst, you can startup with: ainst2 start a4_agg_web --userparam=\"-o -s /path\""
    exit 0
}

if [ $# -lt 1 ]
then
    usage
fi

OPT_DOWNLOAD_THREAD_NUM=1
OPT_AINST_INSTALLROOT=""
OPT_LIB_ASAN_PATH=""
OPT_DRY_RUN=0

declare -a NoneOptionArray
while [ $# -ne 0 ]
do
    # if OPTIND > $#+1, getopts will not change OPTIND's value,so set it to 0
    OPTIND=0
    while getopts ":hj:ds:" opt; do
        case $opt in
            h)
                Usage
                ;;
            s)
                OPT_LIB_ASAN_PATH=$OPTARG
                ;;
            j)
                OPT_DOWNLOAD_THREAD_NUM=$OPTARG
                ;;
            d)
                OPT_DRY_RUN=1
                ;;
            *)
                echo "- unkown option $opt $OPTARG"
                ;;
        esac
    done

    if [ $OPTIND -ne $(($#+1)) ]
    then
        shift $(($OPTIND-1))
        # This is just my favorate way to append element to an array in shell.
        # Feel free to change it
        NoneOptionArray+=($1)
        shift
    else
        break; # getopts doesn't find any non-option argument
    fi
done

OPT_AINST_INSTALLROOT=${NoneOptionArray[0]}
if [ ${#NoneOptionArray[@]} -gt 1 ]; then
    OPT_LIB_ASAN_PATH=${NoneOptionArray[1]}
fi

echo "Option Summary: "
echo "    OPT_AINST_INSTALLROOT: \"$OPT_AINST_INSTALLROOT\""
echo "    OPT_DOWNLOAD_THREAD_NUM: \"$OPT_DOWNLOAD_THREAD_NUM\""
echo "    OPT_LIB_ASAN_PATH: \"$OPT_LIB_ASAN_PATH\""
echo ""

if [ $OPT_DRY_RUN -gt 0 ]; then
    exit 0
fi


date +'[STAT] start: %F %T.%3N'
python $OPT_AINST_INSTALLROOT/conf/a4_agg_quarkchat/pre_process_dev_gig_config.py $OPT_AINST_INSTALLROOT/conf/a4_agg_quarkchat/em21 $OPT_AINST_INSTALLROOT/conf/a4_agg_quarkchat/test $OPT_AINST_INSTALLROOT/conf/a4_agg_quarkchat/em21_conf.template
date +'[STAT] finish gig preprocess: %F %T.%3N'

if [ $OPT_DOWNLOAD_THREAD_NUM -gt 0 ]; then
    if [ $OPT_DOWNLOAD_THREAD_NUM -gt 1 ]; then
        sed -i 's/ndownload_thread_num.*\n//g' ${OPT_AINST_INSTALLROOT}/conf/a4_agg_quarkchat/agg_quarkchat_data.conf
        sed -i 's/\[common\]/\[common\]\ndownload_thread_num='$OPT_DOWNLOAD_THREAD_NUM'/g' ${OPT_AINST_INSTALLROOT}/conf/a4_agg_quarkchat/agg_quarkchat_data.conf
    fi
    python $OPT_AINST_INSTALLROOT/bin/data_checker download -c $OPT_AINST_INSTALLROOT/conf/a4_agg_quarkchat/agg_quarkchat_data.conf
    if [ $? -ne 0 ]
    then
        echo "[ERROR] download data failed"
        exit 1
    fi
    date +'[STAT] finish data download: %F %T.%3N'
else
    echo "[INFO] skip download data"
fi

hostname=`hostname -d`
if [ -z $hostname ]
then
    echo "[ERROR] get hostname failed"
    exit 1
fi

# em21 以及其他测试环境 统一使用em21配置
if [ "$hostname" != "na61" ]
then
    templateFile=$OPT_AINST_INSTALLROOT/conf/a4_agg_quarkchat/em21_conf.template
elif [ "$hostname" = "na61" ]
then
    templateFile=$OPT_AINST_INSTALLROOT/conf/a4_agg_quarkchat/na61_conf.template
else
    echo "[ERROR] unknow hostname: $hostname"
    exit 1
fi

mallocPath="$OPT_AINST_INSTALLROOT/lib64/libjemalloc.so.2"

ip=`hostname -i`
echo "IP: $ip"
str='sed -i s/$(local_ip)/'$ip'/g '$templateFile
echo "Replace IP: $str"
res=`$str`
echo $res

date +'[STAT] finish replace template: %F %T.%3N'
if [ "$mallocPath" != "" ]
then
    if [ "$OPT_LIB_ASAN_PATH" != "" ]
    then
        echo "Malloc Path: $mallocPath"
        MALLOC_CONF="prof:true,prof_active:false" $OPT_AINST_INSTALLROOT/bin/a4_agg_tool start -p $OPT_LIB_ASAN_PATH -p $mallocPath -c $OPT_AINST_INSTALLROOT/conf/a4_agg_quarkchat/a4_agg_quarkchat_conf.xml -l $OPT_AINST_INSTALLROOT/conf/a4_agg_quarkchat/alog.conf -t $templateFile
        if [ $? -ne 0 ]
        then
            echo "[ERROR] a4_tool start failed"
            exit 1
        fi
    else
        echo "Malloc Path: $mallocPath"
        MALLOC_CONF="prof:true,prof_active:false" $OPT_AINST_INSTALLROOT/bin/a4_agg_tool start -p $mallocPath -c $OPT_AINST_INSTALLROOT/conf/a4_agg_quarkchat/a4_agg_quarkchat_conf.xml -l $OPT_AINST_INSTALLROOT/conf/a4_agg_quarkchat/alog.conf -t $templateFile
        if [ $? -ne 0 ]
        then
            echo "[ERROR] a4_tool start failed"
            exit 1
        fi
        echo "[INFO] No libasan.so."
    fi
    date +'[STAT] finish: %F %T.%3N'
else
    echo "[ERROR] No jemalloc, start failed!"
    exit 1
fi
