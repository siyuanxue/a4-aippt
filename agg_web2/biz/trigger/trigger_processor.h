#ifndef AGG_WEB2_BIZ_TRIGGER_TRIGGERPROCESSOR_H
#define AGG_WEB2_BIZ_TRIGGER_TRIGGERPROCESSOR_H

#include <agg2/common/agg_common.h>
#include <agg2/processor/base_processor.h>
#include <A4/util/string_tokenizer.h>

AGG_BEGIN_NAMESPACE(agg_web2);

class TriggerProcessor : public BaseProcessor
{
public:
    TriggerProcessor();
    virtual ~TriggerProcessor();
    
public:
    virtual bool DoInit(const String2String & params) override;
    virtual void DoRegisterMetric(AmonitorRegister *monitorRegister) override;
    virtual BaseProcessor * DoClone() override;
    virtual void DoDestroy() override;
    virtual int DoProcess(AggContext * aggContext) override;

private:
    std::vector<std::string> GetQueryGaokaoIntent(std::string& query);

public:
    // register UDF MethodFunc
    virtual bool RegisterMethods() override;

public:
    // register UDF actor
    virtual bool RegisterActors() override;

private:
    //std::vector<std::vector<std::string> > mQpRuleItemVec; 


private:
    AGG_LOG_DECLARE();
    DECLARE_SERIALIZATION_FUNC();
};

AGG_TYPEDEF_PTR(TriggerProcessor);

AGG_END_NAMESPACE(agg_web2);

#endif //AGG_WEB2_BIZ_TRIGGER_TRIGGERPROCESSOR_H
