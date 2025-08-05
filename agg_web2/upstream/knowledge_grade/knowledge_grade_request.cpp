#include <agg_web2/upstream/knowledge_grade/knowledge_grade_request.h>
#include <sutil/net/url_util.h>
#include <sutil/net/url.h>
#include <agg2/util/string_tool.h>
#include <jsoncpp/json/json.h>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(common, KnowledgeGradeRequest);

BEGIN_DEFINE_SERIALIZATION_FUNC(KnowledgeGradeRequest){
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(HttpRequest);
} END_DEFINE_SERIALIZATION_FUNC(KnowledgeGradeRequest)


KnowledgeGradeRequest::KnowledgeGradeRequest()
{
    SetHttpMethod(HM_POST);
}

KnowledgeGradeRequest::KnowledgeGradeRequest(const std::string& query, const std::string &doc)
    : mQuery(query)
    , mDoc(doc)
{
    SetHttpMethod(HM_POST);
}

KnowledgeGradeRequest::~KnowledgeGradeRequest() 
{ 
}

bool KnowledgeGradeRequest::DoEncode(std::string & uriQuery, std::string & body, String2String & headers)
{   
    // TODO: 这里肯定有大问题，但是目前先搁置，先让他可以跑通
    uriQuery = "";

    Json::Value root;

    root["query"] = mQuery; 
    root["content"] = mDoc;

    Json::FastWriter writer;
    body = writer.write(root);

    AGG_LOG(INFO, "[KG_DEBUG] KnoewledgeGrade request uri=%s, body=%s", uriQuery.c_str(), body.c_str());
    return true;
}

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_IMPLEMENT(AGG_NS(agg_web2)::KnowledgeGradeRequest);

