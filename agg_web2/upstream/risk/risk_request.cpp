#include <agg_web2/upstream/risk/risk_request.h>
#include <sutil/net/url_util.h>
#include <sutil/net/url.h>
#include <agg2/util/string_tool.h>
#include <jsoncpp/json/json.h>
AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(common, RiskRequest);
BEGIN_DEFINE_SERIALIZATION_FUNC(RiskRequest){
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(HttpRequest);
} END_DEFINE_SERIALIZATION_FUNC(RiskRequest)
RiskRequest::RiskRequest()
{
    SetHttpMethod(HM_POST);
}
RiskRequest::RiskRequest(const std::string& summary) : mSummary(summary)
{
    SetHttpMethod(HM_POST);
}
RiskRequest::~RiskRequest() 
{ 
}
bool RiskRequest::DoEncode(std::string & uriQuery, std::string & body, String2String & headers)
{
    Json::Value root;
    root["text"] = mSummary;
    root["cut_sentence"] = 1;      // 0:篇章识别，1:切句识别
    Json::FastWriter writer;
    body = writer.write(root);
    AGG_LOG(DEBUG, "risk request uri=%s, body=%s", uriQuery.c_str(), body.c_str());
    return true;
}

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_IMPLEMENT(AGG_NS(agg_web2)::RiskRequest);

