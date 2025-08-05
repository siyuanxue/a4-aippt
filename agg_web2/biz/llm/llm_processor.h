#ifndef MEDCHATAGG_LLM_LLM_PROCESSOR_H
#define MEDCHATAGG_LLM_LLM_PROCESSOR_H

#include <agg2/processor/base_processor.h>
#include <agg2/common/agg_context.h>
#include <agg_web2/common/common.h>
#include <agg2/util/string_tool.h>
#include <agg_web2/util/AggWebStringTool.h>
#include <jsoncpp/json/json.h>

AGG_BEGIN_NAMESPACE(agg_web2);

class LLMProcessor : public  BaseProcessor
{
public:
    LLMProcessor() {}
    ~LLMProcessor() {}

public:
    bool DoInit(const String2String& params) override;

    virtual void DoRegisterMetric(AmonitorRegister *monitorRegister) override;

    BaseProcessor* DoClone() override;
    void DoDestroy() override;
    int DoProcess(AggContext* aggContext) override;

private:
    void SelectDocs(std::vector<LLMDocPtr>& hitQueue);
    void FormatResult(std::vector<LLMDocPtr>& hitQueue, std::vector<LLMDocPtr>&notSelected);

    std::string GetLLmSummary(const std::string& query, std::vector<LLMDocPtr>& hitQueue);

protected:
    std::string mSummary;

    Context* mContext;
    AggContext* mAggContext;

private:
    AGG_LOG_DECLARE();
    AGG_LOG_DECLARE_WITH_NAME(displayLog);

private:
    DECLARE_SERIALIZATION_FUNC();
};

AGG_TYPEDEF_PTR(LLMProcessor);
AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_KEY(AGG_NS(aigc)::LLMProcessor);

#endif //MEDCHATAGG_RANK_PROCESSOR_H
