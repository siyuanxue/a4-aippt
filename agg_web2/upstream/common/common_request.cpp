#include <agg_web2/upstream/common/common_request.h>
#include <sutil/net/url_util.h>
#include <sutil/net/url.h>
#include <agg2/util/string_tool.h>
#include <jsoncpp/json/json.h>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(common, CommonRequest);

BEGIN_DEFINE_SERIALIZATION_FUNC(CommonRequest){
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(HttpRequest);
} END_DEFINE_SERIALIZATION_FUNC(CommonRequest)


CommonRequest::CommonRequest()
{
    SetHttpMethod(HM_GET);
}

CommonRequest::CommonRequest(const std::string& query, const std::string& type)
    : mQuery(query), mType(type)
{
    SetHttpMethod(HM_GET);
    if (mType == "gaokao_intent") {
        SetHttpMethod(HM_POST);
    }

    if (mType == "common_post") {
        SetHttpMethod(HM_POST);
    }
}

CommonRequest::~CommonRequest() 
{ 
}

bool CommonRequest::DoEncode(std::string & uriQuery, std::string & body, String2String & headers)
{
    if (mType == "gaokao_intent") {
        uriQuery = "";
        Json::Value root;
        root["query"] = mQuery;
        Json::FastWriter writer;
        body = writer.write(root);
        AGG_LOG(ERROR, "%s request uri=%s, body=%s", mType.c_str(), uriQuery.c_str(), body.c_str());
        return true;
    }

    if (mType == "common_post") {
        uriQuery = "";
        body = mQuery;
        // AGG_LOG(ERROR, "%s request uri=%s, body=%s", mType.c_str(), uriQuery.c_str(), body.c_str());
        return true;
    }

    uriQuery = "";
    
    string ecnodeQuery;
    if (!sutil::net::UrlUtil::Encode(mQuery,ecnodeQuery)) {
        AGG_LOG(ERROR,"params Encode error");
        return false;
    }
    
    if (mType == "knowledge") {
        uriQuery = "hit=10&filter_h5=false&debug=true&query="+ecnodeQuery;
    }
    
    if (mType == "news") {
        uriQuery = "_chain=listpage_chain&belong=quark&hid=0&hit=10&start=0&uid=0&bucket=ab_feed_agg_return_ori_body%3Dyes&query=" + ecnodeQuery; 
    }

    AGG_LOG(ERROR, "%s request uri=%s, body=%s", mType.c_str(), uriQuery.c_str(), body.c_str());
    return true;
}

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_IMPLEMENT(AGG_NS(agg_web2)::CommonRequest);

