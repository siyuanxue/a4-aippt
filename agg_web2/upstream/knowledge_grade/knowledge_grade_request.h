#ifndef AGG_WEB2_UPSTREAM_KNOWLEDGE_GRADE_REQUEST_H
#define AGG_WEB2_UPSTREAM_KNOWLEDGE_GRADE_REQUEST_H

#include <agg2/common/agg_common.h>
#include <agg2/common/http_request.h>
#include <agg2/common/agg_context.h>

AGG_BEGIN_NAMESPACE(arbiter);

class KnowledgeGradeRequest: public HttpRequest
{
    DECLARE_SERIALIZATION_FUNC();

public:
    KnowledgeGradeRequest();
    KnowledgeGradeRequest(const std::string& query, const std::string& doc); 
    virtual ~KnowledgeGradeRequest();

public:
    bool DoEncode(std::string& uriQuery, std::string & body, String2String & header) override;

private:
    std::string mQuery;
    std::string mDoc;

private:
    AGG_LOG_DECLARE();
};
AGG_TYPEDEF_PTR(KnowledgeGradeRequest);

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_KEY(AGG_NS(agg_web2)::KnowledgeGradeRequest);

#endif

