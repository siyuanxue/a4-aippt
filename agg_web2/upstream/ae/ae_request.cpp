#include <agg_web2/upstream/ae/ae_request.h>
#include <sutil/net/url_util.h>
#include <sutil/net/url.h>
#include <agg2/util/string_tool.h>
#include <jsoncpp/json/json.h>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(common, AeRequest);

BEGIN_DEFINE_SERIALIZATION_FUNC(AeRequest){
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(HttpRequest);
} END_DEFINE_SERIALIZATION_FUNC(AeRequest)


AeRequest::AeRequest()
{
    SetHttpMethod(HM_POST);
}

AeRequest::AeRequest(const std::string& query, const std::string &title, const std::string &passage)
    : mQuery(query)
    , mTitle(title)
    , mPassage(passage)
{
    SetHttpMethod(HM_POST);
}

AeRequest::~AeRequest() 
{ 
}

bool AeRequest::DoEncode(std::string & uriQuery, std::string & body, String2String & headers)
{
    uriQuery = "";

    Json::Value root;

    root["query"] = mQuery; 
    root["title"] = mTitle;
    root["passage"] = mPassage;

    Json::FastWriter writer;
    body = writer.write(root);

    AGG_LOG(ERROR, "ae request uri=%s, body=%s", uriQuery.c_str(), body.c_str());
    return true;
}

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_IMPLEMENT(AGG_NS(agg_web2)::AeRequest);

