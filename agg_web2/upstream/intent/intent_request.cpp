#include <agg_web2/upstream/intent/intent_request.h>
#include <sutil/net/url_util.h>
#include <sutil/net/url.h>
#include <agg2/util/string_tool.h>
#include <jsoncpp/json/json.h>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(common, IntentRequest);

BEGIN_DEFINE_SERIALIZATION_FUNC(IntentRequest){
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(HttpRequest);
} END_DEFINE_SERIALIZATION_FUNC(IntentRequest)


IntentRequest::IntentRequest()
{
    SetHttpMethod(HM_POST);
}

IntentRequest::IntentRequest(const std::string& query, const vector<std::string>& historys, const std::string& version)
    : mQuery(query)
    , mHistorys(historys)
    , mVersion(version)
{
    SetHttpMethod(HM_POST);
}

IntentRequest::~IntentRequest()
{
}

bool IntentRequest::DoEncode(std::string & uriQuery, std::string & body, String2String & headers)
{
    uriQuery = "";

    Json::Value root;

    root["query"] = mQuery;
    Json::Value historys = Json::Value(Json::arrayValue);
    for (const std::string& t : mHistorys) {
        historys.append(Json::Value(t));
    }

    root["history_query"] = historys;
    root["version_bucket"] = mVersion;

    Json::FastWriter writer;
    body = writer.write(root);

    AGG_LOG(ERROR, "qt request uri=%s, body=%s", uriQuery.c_str(), body.c_str());
    return true;
}

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_IMPLEMENT(AGG_NS(agg_web2)::IntentRequest);

