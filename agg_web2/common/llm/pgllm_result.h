#ifndef AGG_COMMON_LLM_PGLLM_RESULT_H
#define AGG_COMMON_LLM_PGLLM_RESULT_H
#include <agg2/common/agg_common.h>
#include <agg2/common/result.h>
#include <agg_web2/common/llm/qa_llm_result.h>
AGG_BEGIN_NAMESPACE(common);

class PgLLMResult:public QaLLMResult
{
public:
    PgLLMResult(){

    }
    virtual ~PgLLMResult(){

    }

public:
    virtual bool Serialize(std::string & bytes) override;
    virtual bool Deserialize(const std::string & bytes) override;

private:
    AGG_LOG_DECLARE();
};

AGG_TYPEDEF_PTR(PgLLMResult);
AGG_END_NAMESPACE(common);
#endif //AGGWEB_MEDLLM_RESULT_H
