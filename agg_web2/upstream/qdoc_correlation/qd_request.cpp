#include <agg_web2/upstream/qdoc_correlation/qd_request.h>
#include <sutil/net/url_util.h>
#include <sutil/net/url.h>
#include <agg2/util/string_tool.h>
#include <jsoncpp/json/json.h>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(common, QdRequest);

BEGIN_DEFINE_SERIALIZATION_FUNC(QdRequest){
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(HttpRequest);
} END_DEFINE_SERIALIZATION_FUNC(QdRequest)


QdRequest::QdRequest()
{
    SetHttpMethod(HM_POST);
}

QdRequest::QdRequest(const std::string& query, const std::string &title, const std::string &passage)
    : mQuery(query)
    , mTitle(title)
    , mPassage(passage)
{
    SetHttpMethod(HM_POST);
    SetPath("/process");
}

QdRequest::~QdRequest() 
{ 
}

bool QdRequest::Encode()
{
    SetUri("/process");
    Json::Value root;

    root["question"] = mQuery;
    root["title"] = mTitle;
    root["content"] = mPassage;

    Json::FastWriter writer;
    std::string body1 = writer.write(root);
    SetBody(body1);

    /* AGG_LOG(ERROR, "ar request body=%s", body1.c_str()); */
    return true;
}

bool QdRequest::DoEncode(std::string & uriQuery, std::string & body, String2String & headers)
{
    uriQuery = "";

    Json::Value root;

    root["question"] = mQuery;
    root["title"] = mTitle;
    root["content"] = mPassage;

    Json::FastWriter writer;
    body = writer.write(root);

    AGG_LOG(ERROR, "ar request uri=%s, body=%s", uriQuery.c_str(), body.c_str());
    return true;
}

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_IMPLEMENT(AGG_NS(agg_web2)::QdRequest);

