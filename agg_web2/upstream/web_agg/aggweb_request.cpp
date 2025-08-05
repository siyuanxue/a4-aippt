#include <agg_web2/upstream/web_agg/aggweb_request.h>
#include <sutil/net/url_util.h>
#include <sutil/net/url.h>
#include <agg2/util/string_tool.h>
#include <agg2/helper/time_helper.h>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2, AggWebRequest);

BEGIN_DEFINE_SERIALIZATION_FUNC(AggWebRequest){
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(HttpRequest);
} END_DEFINE_SERIALIZATION_FUNC(AggWebRequest)


AggWebRequest::AggWebRequest() : mAggContext(nullptr)
{
    SetHttpMethod(HM_GET);  // 默认Arbiter为Get请求
}

AggWebRequest::AggWebRequest(AggContext *aggContext, const std::string& query)
    : mAggContext(aggContext)
    , mQuery(query)
{
    SetHttpMethod(HM_GET);  // 默认Arbiter为Get请求
}

AggWebRequest::~AggWebRequest() 
{ 

}

void ReplaceExtParams(String2String& params)
{
    // 关闭读写：close_cache  关闭写： close_update_cache
    if (params.find("ext_params") != params.end()) {
        params["ext_params"] += "&close_cache=yes";
    } else {
        params["ext_params"] = "close_cache=yes"; 
    }
}

/*std::string AggWebRequest::GetOsrWithTime()
{
    int nowSysTime = (TimeHelper::GetSystemCurrentTimeSec() + 8 * 3600) % (24 * 3600);
    AGG_LOG(DEBUG, "nowSysTime=%d", nowSysTime);

    int Hour_0100_TimeStamp = 1 * 3600;
    int Hour_0800_TimeStamp = 8 * 3600;
    int Hour_1100_TimeStamp = 11 * 3600;
    if (nowSysTime >= Hour_0800_TimeStamp && nowSysTime <= Hour_1100_TimeStamp) {
        // 白天，50qps
        return "searchqa";
    } else if(nowSysTime >= Hour_0100_TimeStamp && nowSysTime <= Hour_0800_TimeStamp) {
        // 夜间，200qps
        return "searchqa_night";
    } else {
        // 其它时间段，禁止抓取
        return "forbidden_searchqa";
    }
}*/

bool AggWebRequest::DoEncode(std::string & uriQuery, std::string & body, String2String & headers)
{
    if (mAggContext == NULL) {
        AGG_LOG(ERROR, "mAggContext is null");
        return false;
    }
    headers["Content-Type"] = "x-www-form-urlencoded";
    headers["User-Agent"] = "curl/7.29.0";
    headers["Host"] = mHost;
    headers["Accept"] = "*/*";
    String2String params;
    auto iter = mAggContext->BeginRequestParam();
    for (; iter != mAggContext->EndRequestParam(); iter++) {
        params.insert(make_pair(iter->first,iter->second));
    }

    /* std::string osr = GetOsrWithTime();
    if (osr == "forbidden_searchqa") {
        osr = "searchqa";
    }
    
    AGG_LOG(DEBUG, "[GetOsrWithTime] osr=[%s]", osr.c_str());
    if (params.find("debug") != params.end()) {
        if (osr == "forbidden_searchqa") {
            osr = "searchqa";
        }
    } else {
        if(osr == "forbidden_searchqa") {
            AGG_LOG(DEBUG,"params forbidden_searchqa");
            return false;
        }
    }
    AGG_LOG(DEBUG, "[GetOsrWithTime] osr=[%s]", osr.c_str());
    */
    
    string osr = "searchqa_gaokao";

    params["osr"]=osr;
    params["sessclose"]="1";
    params["hit"]="20";
    //params["debug"]="1"; // TODO 待去除
    //params.erase("debug"); // 去除debug
    params["from"]="kkframenew";
    params["buck_mode"]="close_interleaving_and_fix_max";
    params["belong"]="quark";
    params["ad"]="no";
    params["query"]=mQuery;
    ReplaceExtParams(params);

    string ecnodeParam;
    if (!sutil::net::UrlUtil::Encode(params,ecnodeParam)) {
        AGG_LOG(ERROR,"params Encode error");
        return false;
    }
    uriQuery = ecnodeParam;
    mAggRequestStr = uriQuery;
    AGG_LOG(DEBUG,"agg request:[%s]", uriQuery.c_str());
    return true;
}

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_IMPLEMENT(AGG_NS(agg_web2)::AggWebRequest);

