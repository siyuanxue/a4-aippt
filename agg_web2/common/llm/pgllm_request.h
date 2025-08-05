#ifndef AGG_WEB2_COMMON_LLM_PGLLM_REQUEST_H
#define AGG_WEB2_COMMON_LLM_PGLLM_REQUEST_H

#include <agg2/common/agg_common.h>
#include <agg2/common/http_request.h>
#include <agg2/common/agg_context.h>
#include <agg_web2/common/llm/qa_llm_request.h>

AGG_BEGIN_NAMESPACE(agg_web2);


class PgLLMRequest : public QaLLMRequest {
    DECLARE_SERIALIZATION_FUNC();

public:
    PgLLMRequest();
    PgLLMRequest(AggContext *aggContext,const std::string &model);
    virtual ~PgLLMRequest();
    
public:
    virtual bool DoEncode(std::string &uriQuery, std::string &body,
                  String2String &header) override;    
public:
    const std::string& GetHost() const { return mHost; }
    void SetHost(const std::string& host) { mHost = host;}
private:
    std::string mHost;
    AGG_LOG_DECLARE();
};

AGG_TYPEDEF_PTR(PgLLMRequest);

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_KEY(AGG_NS(agg_web2)::PgLLMRequest);

#endif

