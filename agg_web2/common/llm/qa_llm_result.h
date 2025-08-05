#ifndef AGG_COMMON_LLM_QA_LLM_RESULT_H
#define AGG_COMMON_LLM_QA_LLM_RESULT_H
#include <agg2/common/agg_common.h>
#include <agg2/common/result.h>
AGG_BEGIN_NAMESPACE(common);

class QaLLMResult:public Result
{
public:
    QaLLMResult():mCode(-1),mMessage("init"),mFinishReason("null"){

    }
    virtual ~QaLLMResult(){

    }

public:
    virtual bool Serialize(std::string & bytes) override;
    virtual bool Deserialize(const std::string & bytes) override;

public:
    int mCode;
    std::string mRequestId;
    std::string mMessage;
    std::string mFinishReason;
    std::string mModel;
    std::string mPrompt;
    std::string mContent;
private:
    AGG_LOG_DECLARE();
};

AGG_TYPEDEF_PTR(QaLLMResult);
AGG_END_NAMESPACE(common);
#endif //AGGWEB_LLM_RESULT_H
