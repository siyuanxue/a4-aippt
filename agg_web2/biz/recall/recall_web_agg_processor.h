#ifndef AGG_WEB2_BIZ_RECALL_WEBAGG_PROCESSOR_H
#define AGG_WEB2_BIZ_RECALL_WEBAGG_PROCESSOR_H

#include <agg2/common/agg_common.h>
#include <agg2/processor/base_processor.h>
#include <A4/util/string_tokenizer.h>
#include <agg_web2/upstream/web_agg/aggweb_request.h>
#include <agg_web2/upstream/web_agg/aggweb_result.h>

AGG_BEGIN_NAMESPACE(agg_web2);

class RecallWebAggProcessor : public BaseProcessor
{
public:
    RecallWebAggProcessor();
    virtual ~RecallWebAggProcessor();
    
public:
    virtual bool DoInit(const String2String & params) override;
    virtual void DoRegisterMetric(AmonitorRegister *monitorRegister) override;
    virtual BaseProcessor * DoClone() override;
    virtual void DoDestroy() override;
    virtual int DoProcess(AggContext * aggContext) override;

public:
    // register UDF MethodFunc
    virtual bool RegisterMethods() override;

public:
    // register UDF actor
    virtual bool RegisterActors() override;
    
private:
    HttpResponsePtr AsyncRequestAgg(AggContext *aggContext, std::string query);

private:
    //std::vector<std::vector<std::string> > mQpRuleItemVec; 
    LLMDebugInfoPtr mDebugInfoPtr;
    std::string mAggHost;

private:
    AGG_LOG_DECLARE();
    DECLARE_SERIALIZATION_FUNC();
};

AGG_TYPEDEF_PTR(RecallWebAggProcessor);

AGG_END_NAMESPACE(agg_web2);

#endif //AGG_WEB2_BIZ_RECALL_WEBAGG_PROCESSOR_H
