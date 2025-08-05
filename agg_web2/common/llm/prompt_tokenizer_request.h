#ifndef AGG_WEB2_COMMON_LLM_PROMPT_TOKENIZER_REQUEST_H
#define AGG_WEB2_COMMON_LLM_PROMPT_TOKENIZER_REQUEST_H

#include <agg2/common/agg_common.h>
#include <agg2/common/http_request.h>
#include <agg2/common/agg_context.h>
#include <agg_web2/common/llm/qa_llm_request.h>

AGG_BEGIN_NAMESPACE(agg_web2);


class PromptTokenizerRequest : public QaLLMRequest {
    DECLARE_SERIALIZATION_FUNC();

public:
    PromptTokenizerRequest();
    PromptTokenizerRequest(AggContext *aggContext,const std::string &model);
    virtual ~PromptTokenizerRequest();
    
public:
    virtual bool DoEncode(std::string &uriQuery, std::string &body,
                  String2String &header) override;
     
private:
    AGG_LOG_DECLARE();
};

AGG_TYPEDEF_PTR(PromptTokenizerRequest);

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_KEY(AGG_NS(agg_web2)::PromptTokenizerRequest);

#endif

