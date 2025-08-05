说明：

A4框架代码，请迁移对应的A4代码到该git项目中

A4框架插件说明：https://yuque.antfin-inc.com/zeus/wiki/plugin-code#95c8d641

```
安装说明
sudo scons rpm quietrpm=false -j24  # 需要quietrpm模式，不可去掉
sudo ainst2 install a4_agg_quarkchat -i $(pwd)_worker --localrepo=$(pwd)/build/release64/rpm_build/RPMS/x86_64
sudo cd $(pwd)_worker
```


启动服务
```
首次启动，需要改端口号

sudo rm logs/a4_agg_quarkchat/*
sudo cp ../QuarkChatAgg/build/release64/lib/* lib64/

sudo ainst2 stop a4_agg_quarkchat -i ./
sudo ainst2 start a4_agg_quarkchat -i ./
```

请求agg
```
function run()
{
    sess=`openssl rand -base64 12`
    uid=jinru.yjr
    # web 接口
    #curl "http://11.180.61.211:34561/web" -d '{"version":1,"target":"request","communication":{"uid":"'${uid}'","sessionid":"'${sess}'","reqid":"282f7cf5-0680-446f-b8db-5cc5dfb14550","resid":0},"data":{"message":{"query":"'${1}'","status":"init"}},"bucket":{"k1":"v1","k2":"v2","k3":"v3"},"error_code":0,"error_msg":"","timestamp":"1682390791"}'
    
    # chatagg接口
    curl "http://11.180.61.211:34561/agg" -H 'Accept:text/event-stream' -d '{"version":1,"target":"request","communication":{"uid":"'${uid}'","sessionid":"'${sess}'","reqid":"282f7cf5-0680-446f-b8db-5cc5dfb14550","resid":0},"data":{"message":{"query":"'${1}'","status":"init"}},"bucket":{"k1":"v1","k2":"v2","k3":"v3"},"error_code":0,"error_msg":"","timestamp":"1682390791"}'
}

set -x
q="电脑黑屏怎么办"
run $q
```

