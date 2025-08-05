#ifndef AGG_COMMON_LLM_PROMPT_TOKENIZER_RESULT_H
#define AGG_COMMON_LLM_PROMPT_TOKENIZER_RESULT_H
#include <agg2/common/agg_common.h>
#include <agg2/common/result.h>
AGG_BEGIN_NAMESPACE(common);

class PromptTokenizerResult:public Result
{
public:
    PromptTokenizerResult():mCode(-1),mMessage("init"){

    }
    virtual ~PromptTokenizerResult(){

    }

public:
    virtual bool Serialize(std::string & bytes) override;
    virtual bool Deserialize(const std::string & bytes) override;

public:
    int mCode;
    std::string mMessage;
    std::string mModel;
    std::string mPrompt;
private:
    AGG_LOG_DECLARE();
};

AGG_TYPEDEF_PTR(PromptTokenizerResult);
AGG_END_NAMESPACE(common);
#endif //AGG_COMMON_LLM_PROMPT_TOKENIZER_RESULT_H
