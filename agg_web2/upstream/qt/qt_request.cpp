#include <agg_web2/upstream/qt/qt_request.h>
#include <sutil/net/url_util.h>
#include <sutil/net/url.h>
#include <agg2/util/string_tool.h>
#include <jsoncpp/json/json.h>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(common, QtRequest);

BEGIN_DEFINE_SERIALIZATION_FUNC(QtRequest){
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(HttpRequest);
} END_DEFINE_SERIALIZATION_FUNC(QtRequest)


QtRequest::QtRequest()
{
    SetHttpMethod(HM_POST);
}

QtRequest::QtRequest(const std::string& query, const vector<std::string>& titles)
    : mQuery(query)
    , mTitles(titles)
{
    SetHttpMethod(HM_POST);
}

QtRequest::~QtRequest() 
{ 
}

bool QtRequest::DoEncode(std::string & uriQuery, std::string & body, String2String & headers)
{
    uriQuery = "bid=kk_qt";

    Json::Value root;

    Json::Value datas = Json::Value(Json::arrayValue);

    Json::Value data = Json::Value(Json::objectValue);

    Json::Value model = Json::Value(Json::arrayValue);
    model.append(Json::Value("qt_12l_ipu_v2"));
    data["model"] = model;

    Json::Value query = Json::Value(Json::arrayValue);
    Json::Value title = Json::Value(Json::arrayValue);
    for (const std::string& t : mTitles){
        title.append(Json::Value(t));
        query.append(Json::Value(mQuery));
    }
    data["title"] = title;
    data["query"] = query;
    data["params"] = Json::Value(Json::objectValue);

    datas.append(data);
    root["data"] = datas;

    root["params"] = Json::Value(Json::objectValue);
    root["cache"] = true;

    Json::FastWriter writer;
    body = writer.write(root);

    AGG_LOG(ERROR, "qt request uri=%s, body=%s", uriQuery.c_str(), body.c_str());
    return true;
}

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_IMPLEMENT(AGG_NS(agg_web2)::QtRequest);

