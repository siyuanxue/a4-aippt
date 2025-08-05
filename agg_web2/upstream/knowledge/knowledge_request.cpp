#include <agg_web2/upstream/knowledge/knowledge_request.h>
#include <sutil/net/url_util.h>
#include <sutil/net/url.h>
#include <agg2/util/string_tool.h>
#include <jsoncpp/json/json.h>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(common, KnowledgeRequest);

BEGIN_DEFINE_SERIALIZATION_FUNC(KnowledgeRequest){
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(HttpRequest);
} END_DEFINE_SERIALIZATION_FUNC(KnowledgeRequest)


KnowledgeRequest::KnowledgeRequest()
{
    SetHttpMethod(HM_GET);
}

KnowledgeRequest::KnowledgeRequest(const std::string& query, const std::string& osr_shield)
    : mQuery(query), mOsrShield(osr_shield)
{
    SetHttpMethod(HM_GET);
}

KnowledgeRequest::~KnowledgeRequest() 
{ 
}

bool KnowledgeRequest::DoEncode(std::string & uriQuery, std::string & body, String2String & headers)
{
    uriQuery = "";
    
    string encodeQuery;
    if (!sutil::net::UrlUtil::Encode(mQuery,encodeQuery)) {
        AGG_LOG(ERROR,"params Encode error");
        return false;
    }

    uriQuery = "hit=10&filter_h5=false&debug=true&query=" + encodeQuery + "&osr_shield=" + mOsrShield;

    AGG_LOG(ERROR, "knowledge request uri=%s, body=%s", uriQuery.c_str(), body.c_str());
    return true;
}

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_IMPLEMENT(AGG_NS(agg_web2)::KnowledgeRequest);

